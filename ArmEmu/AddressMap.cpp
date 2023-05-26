//! @file ArmEmu/AddressMap.cpp
//! @brief The definition of an object which indexes IAddressRegion objects
//! by the range of addresses they span.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Ag/Core/Binary.hpp"

#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/ArmSystem.hpp"
#include "ArmEmu/GuestEventQueue.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
size_t BitFloor(size_t value)
{
    int msb;
    return Ag::Bin::bitScanForward(value, msb) ? (static_cast<size_t>(1) << msb) :
                                                 0;
}

size_t BitCeil(size_t value)
{
    int msb;
    size_t result = 0;

    if (Ag::Bin::bitScanForward(value, msb))
    {
        result = (static_cast<size_t>(1) << msb);

        if (value > result)
            result <<= 1;
    }

    return result;
}

// Inspired by: https://probablydance.com/2023/04/27/beautiful-branchless-binary-search/
template<typename It, typename T, typename Cmp>
It branchless_lower_bound(It begin, It end, const T &value, Cmp &&compare)
{
    size_t length = std::distance(begin, end);

    if (length == 0)
        return end;

    size_t step = BitFloor(length);

    if ((step != length) && compare(*(begin + step), value))
    {
        length -= step + 1;

        if (length == 0)
            return end;

        step = BitCeil(length);
        begin = end - step;
    }

    // This inner loop can be encoded with conditional move, hence it
    // is branchless.
    for (step /= 2; step != 0; step /= 2)
    {
        if (compare(*(begin + step), value))
            begin += step;
    }

    return begin + compare(*begin, value);
}

template<typename It, typename T>
It branchless_lower_bound(It begin, It end, const T &value)
{
    return branchless_lower_bound(begin, end, value, std::less<T>());
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// ConnectionContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object used to connect emulated I/O devices to the
//! host system and each other.
//! @param[in] interopContext The context object which provides inter-operation
//! services for emulated hardware devices.
//! @param[in] readMap The address map listing readable regions of memory which
//! map to I/O devices of host memory.
//! @param[in] writeMap The address map listing writeable regions of memory which
//! map to I/O devices of host memory.
ConnectionContext::ConnectionContext(SystemContextPtr interopContext,
                                     const AddressMap &readMap,
                                     const AddressMap &writeMap) :
    _interopContext(interopContext)
{
    // Index the Memory Mapped I/O regions as devices which can be looked up
    // by name.
    for (uint8_t i = 0; i < 2; ++i)
    {
        const AddressMap &map = (i == 0) ? readMap : writeMap;

        for (const auto &mapping : map.getMappings())
        {
            IAddressRegionPtr region = mapping.Region;

            if (region->getType() == RegionType::MMIO)
            {
                Ag::string_cref_t deviceName = region->getName();
                auto insertResult = _devicesByName.try_emplace(deviceName, region);

                // Ensure that if the name was already in the map, it referred
                // to the same device.
                if ((insertResult.second == false) &&
                    (insertResult.first->second != region))
                {
                    std::string message("The memory mapped device name '");
                    Ag::appendAgString(message, deviceName);
                    message.append("' refers to multiple devices in the same address map.");

                    throw Ag::OperationException(std::string_view(message));
                }
            }
        }
    }
}

//! @brief Attempts to find a Memory Mapped I/O device mapped into the guest
//! system address space by its name.
//! @param[in] name The device name to search for.
//! @param[out] device Receives a pointer to the matching device if one
//! was found.
//! @retval true A matching device was found and its pointer returned.
//! @retval false name device was found with a matching name.
bool ConnectionContext::tryFindDevice(Ag::string_cref_t name,
                                      IAddressRegionPtr &device) const
{
    auto pos = _devicesByName.find(name);
    bool hasMatch = false;
    device = nullptr;

    if (pos != _devicesByName.end())
    {
        hasMatch = true;
        device = pos->second;
    }

    return hasMatch;
}

//! @brief Gets a pointer to and object which provides services to I/O devices
//! while the emulated system is running.
SystemContextPtr ConnectionContext::getInteropContext() const
{
    return _interopContext;
}

////////////////////////////////////////////////////////////////////////////////
// AddressMap Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! Constructs an empty address region mapping.
AddressMap::Mapping::Mapping() :
    Region(nullptr),
    Address(0),
    End(0)
{
}

//! @brief Constructs a mapping to be used as a look-up key.
//! @param[in] address The base address of the mapping.
AddressMap::Mapping::Mapping(uint32_t address) :
    Region(nullptr),
    Address(address),
    End(address + 4)
{
}

//! @brief Constructs an initialised address region mapping.
//! @param[in] address The base address of the region in the guest system
//! physical address space.
//! @param[in] region A pointer to the object which represents the contents of
//! the region.
AddressMap::Mapping::Mapping(uint32_t address, IAddressRegion *region) :
    Region(region),
    Address(address),
    End(region == nullptr ? address : address + region->getSize())
{
    // Round the end address up to the nearest multiple of 4 bytes.
    End = (End + 3) & ~3;
}

//! @brief An less then operator which compares mappings solely on their
//! base address.
//! @param[in] rhs The mapping to compare against.
//! @retval true The current mapping has a lower base address than rhs.
//! @retval false The current mapping has an equal or higher base
//! address to rhs.
bool AddressMap::Mapping::operator<(const AddressMap::Mapping &rhs) const
{
    // Note: We compare the lowest address because trying to make
    // overlapping ranges appear equal interferes with lower_bound().
    return Address < rhs.Address;
}

//! @brief Determines if the current mapping overlaps with another.
//! @param[in] rhs The mapping to test for overlap.
//! @retval true The current mapping has an address range which overlaps with
//! rhs.
//! @retval false The mapping may be contiguous, but don't overlap.
bool AddressMap::Mapping::isOverlapping(const Mapping &rhs) const
{
    bool isOverlapping = false;

    if (Address == rhs.Address)
    {
        isOverlapping = true;
    }
    else if (Address < rhs.Address)
    {
        isOverlapping = rhs.Address < End;
    }
    else // if (rhs.Address < Address)
    {
        isOverlapping = Address < rhs.End;
    }

    return isOverlapping;
}

//! @brief Gets the collection of mappings.
const AddressMap::MappingCollection &AddressMap::getMappings() const
{
    return _mappings;
}

//! @brief Attempts to find the region which contains a specified address.
//! @param[in] address The address to look up.
//! @param[out] region Receives the object describing the region the address
//! lies within.
//! @param[out] offset Receives the offset of the address within the region
//! if a matching region was found.
//! @param[out] remainingLength Receives the count of bytes within the region
//! after the specified address for which the region is valid.
//! @retval true The address fell within a mapped region. The region and offset
//! to the location within it were returned.
//! @retval false The address was not within any region.
bool AddressMap::tryFindRegion(uint32_t address, IAddressRegion *&region,
                               uint32_t &offset, uint32_t &remainingLength) const
{
    if (_mappings.empty())
        return false;

    Mapping key(address);
    bool isFound = false;

    auto pos = branchless_lower_bound(_mappings.begin(),
                                      _mappings.end(), key);

    // We've found the first entry with an equal or higher base address,
    // possibly even _mappings.end().
    if (pos == _mappings.end())
    {
        --pos;
    }

    isFound = pos->isOverlapping(key);

    if ((isFound == false) && (pos != _mappings.begin()))
    {
        // Try the previous block.
        --pos;

        isFound = pos->isOverlapping(key);
    }

    if (isFound)
    {
        region = pos->Region;
        offset = address - pos->Address;
        remainingLength = pos->End - address;
        return true;
    }
    else
    {
        return false;
    }
}

//! @brief Removes all mappings.
void AddressMap::clear()
{
    _mappings.clear();
}

//! @brief Attempts to insert a region mapping.
//! @param[in] baseAddress The base address of the region being mapped.
//! @param[in] region The region being mapped.
//! @retval true The region was successfully added.
//! @retval false The region overlapped at least one existing region already
//! in the collection.
bool AddressMap::tryInsert(uint32_t baseAddress, IAddressRegion *region)
{
    bool canInsert = true;

    if (_mappings.empty())
    {
        _mappings.emplace_back(baseAddress, region);
    }
    else
    {
        Mapping key(baseAddress, region);

        // Determine the location at which the mapping should be inserted.
        auto pos = branchless_lower_bound(_mappings.begin(),
                                          _mappings.end(), key);

        if (pos != _mappings.begin())
        {
            auto prev = pos - 1;

            canInsert = (prev->isOverlapping(key) == false);
        }

        if (canInsert && (pos != _mappings.end()))
        {
            canInsert = (pos->isOverlapping(key) == false);
        }

        if (canInsert)
        {
            _mappings.insert(pos, key);
        }
    }

    return canInsert;
}

////////////////////////////////////////////////////////////////////////////////
// GenericHostMap Member Definitions
////////////////////////////////////////////////////////////////////////////////
GenericHostBlock::GenericHostBlock() :
    _data(nullptr),
    _length(0)
{
}

//! @brief Constructs a generic mapping of a block of host memory into the
//! address space of a guest system.
//! @param[in] name The display name describing the block.
//! @param[in] desc A description of the block.
//! @param[in] hostBlock A pointer to the block of host memory which must have
//! a lifetime that is at least as long as the emulated system it is mapped it.
//! @param[in] byteCount The count of bytes in hostBlock.
GenericHostBlock::GenericHostBlock(const char *name, const char *desc,
                                   uint8_t *hostBlock, uint32_t byteCount) :
    _name(name),
    _description(desc),
    _data(hostBlock),
    _length(byteCount)
{
}

// Inherited from IAddressRegion/
RegionType GenericHostBlock::getType() const { return RegionType::HostBlock; }

// Inherited from IAddressRegion/
Ag::string_cref_t GenericHostBlock::getName() const { return _name; }

// Inherited from IAddressRegion/
Ag::string_cref_t GenericHostBlock::getDescription() const { return _description; }

// Inherited from IAddressRegion/
uint32_t GenericHostBlock::getSize() const { return _length; }

// Inherited from IHostBlock.
void *GenericHostBlock::getHostAddress() { return _data; }

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


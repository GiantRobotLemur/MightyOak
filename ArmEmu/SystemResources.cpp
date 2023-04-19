//! @file ArmEmu/SystemResources.cpp
//! @brief The definition of an object which manages resources for an emulated
//! ARM-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>

#include <algorithm>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Exception.hpp"

#include "SystemResources.hpp"

namespace Ag {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief Allows descriptions of guest mapped address regions to be sorted by
//! their base addresses.
struct CompareGuestMappings
{
    bool operator()(const GuestMMIO &lhs, const GuestMMIO &rhs) const
    {
        return lhs.GuestBaseAddr < rhs.GuestBaseAddr;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief A function to read a location in a range of guest addresses.
//! @param[in] context The block of guest memory mapped into the region.
//! @param[in] offset The word-aligned offset of the byte, half-word or
//! word to read.
//! @return The word read from the specified location.
uint32_t readGuestMemory(uintptr_t context, uint32_t offset)
{
    // Calculate the guest address and ensure it is word aligned.
    const uint32_t *guestAddr = reinterpret_cast<const uint32_t *>((context + offset) &
                                                                   ~static_cast<uintptr_t>(3));

    return *guestAddr;
}

//! @brief A function to write up to 32-bits to a location in
//! a range of guest addresses.
//! @param[in] context The context value associated with the MMIO address
//! range being written to.
//! @param[in] offset The word-aligned offset of the byte, half-word or 
//! word to write.
void writeGuestMemory(uintptr_t context, uint32_t offset, uint32_t value)
{
    uint32_t *guestAddr = reinterpret_cast<uint32_t *>((context + offset) &
                                                       ~static_cast<uintptr_t>(3));

    *guestAddr = value;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// SystemResources Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs the resources of an emulated ARM-based system.
//! @param[in] irqSink A pointer to the object sensitive to raised interrupts.
SystemResources::SystemResources(IIrqSink *irqSink) :
    _irqSink(irqSink)
{
    // Initially allocate 32 KB RAM.
    _ram.resize(32 * 1024, 0);
}

//! @brief Gets the size of the physical RAM in bytes.
uint32_t SystemResources::getRamSize() const { return static_cast<uint32_t>(_ram.size()); }

//! @brief Gets a pointer to the first byte of emulated RAM.
uint8_t *SystemResources::getRam() { return _ram.data(); }

//! @brief Gets a read-only pointer to the first byte of emulated RAM.
const uint8_t *SystemResources::getRam() const { return _ram.data(); }

//! @brief Gets the size of the emulated main ROM, in bytes.
uint32_t SystemResources::getMainRomSize() const { return static_cast<uint32_t>(_mainRom.size()); }

//! @brief Gets a read-only pointer to the first byte of the emulated main ROM.
const uint8_t *SystemResources::getMainRom() const { return _mainRom.data(); }

//! @brief Looks up the entry describing an address to be read in the guest
//! address space.
//! @param[in] addr The physical guest address to look up.
//! @return A pointer to the entry governing the range of addresses containing
//! the specified address, or nullptr if nothing is mapped to that address.
const GuestMMIO *SystemResources::queryReadAddressMap(uint32_t addr) const
{
    const GuestMMIO *region = nullptr;

    if (_readMappings.empty() == false)
    {
        GuestMMIO key;
        key.GuestBaseAddr = addr;

        // Find the block after the one containing the address.
        auto pos = std::upper_bound(_readMappings.begin(), _readMappings.end(),
                                    key, CompareGuestMappings());

        if (pos != _readMappings.begin())
        {
            // Move to the block before.
            --pos;

            if ((addr >= pos->GuestBaseAddr) &&
                (addr < (pos->GuestBaseAddr + pos->Size)))
            {
                region = _readMappings.data() + std::distance(_readMappings.begin(), pos);
            }
        }
    }

    return region;
}

//! @brief Looks up the entry describing an address to be read in the guest
//! address space. If the lookup fails, the address of the next block is returned.
//! @param[in] addr The physical guest address to look up.
//! @param[out] nextAddr Receives the address of the start of the next mapped
//! region after addr if there is none at the specified address.
//! @return A pointer to the entry governing the range of addresses containing
//! the specified address, or nullptr if nothing is mapped to that address.
const GuestMMIO *SystemResources::queryReadAddressMap(uint32_t addr,
                                                      uint32_t &nextAddr) const
{
    // Assume there are no more regions, return the highest possible address.
    nextAddr = ~3u;

    const GuestMMIO *region = nullptr;

    if (_readMappings.empty() == false)
    {
        GuestMMIO key;
        key.GuestBaseAddr = addr;

        // Find the block after the one containing the address.
        auto pos = std::upper_bound(_readMappings.begin(), _readMappings.end(),
                                    key, CompareGuestMappings());

        if (pos != _readMappings.begin())
        {
            // Move to the block before.
            auto next = pos;
            --pos;

            if ((addr >= pos->GuestBaseAddr) &&
                (addr < (pos->GuestBaseAddr + pos->Size)))
            {
                region = _readMappings.data() + std::distance(_readMappings.begin(), pos);
            }
            else if (next != _readMappings.end())
            {
                // Return the address of the start of the next region.
                nextAddr = next->GuestBaseAddr;
            }
        }
    }

    return region;
}

//! @brief Looks up then entry describing an address to be written to in the
//! guest address space.
//! @param[in] addr The physical guest address to look up.
//! @return A pointer to the entry governing the range of addresses containing
//! the specified address, or nullptr if nothing is mapped to that address.
const GuestMMIO *SystemResources::queryWriteAddressMap(uint32_t addr) const
{
    const GuestMMIO *region = nullptr;

    if (_readMappings.empty() == false)
    {
        GuestMMIO key;
        key.GuestBaseAddr = addr;

        // Find the block after the one containing the address.
        auto pos = std::upper_bound(_writeMappings.begin(), _writeMappings.end(),
                                    key, CompareGuestMappings());

        if (pos != _writeMappings.begin())
        {
            // Move to the block before.
            --pos;

            if ((addr >= pos->GuestBaseAddr) &&
                (addr < (pos->GuestBaseAddr + pos->Size)))
            {
                region = _writeMappings.data() + std::distance(_writeMappings.begin(), pos);
            }
        }
    }

    return region;
}

//! @brief Looks up then entry describing an address to be written to in the
//! guest address space.
//! @param[in] addr The physical guest address to look up.
//! @param[out] nextAddr Receives the address of the start of the next mapped
//! region after addr if there is none at the specified address.
//! @return A pointer to the entry governing the range of addresses containing
//! the specified address, or nullptr if nothing is mapped to that address.
const GuestMMIO *SystemResources::queryWriteAddressMap(uint32_t addr,
                                                       uint32_t &nextAddr) const
{
    // Assume there are no more regions, return the highest possible address.
    nextAddr = ~3u;

    const GuestMMIO *region = nullptr;

    if (_writeMappings.empty() == false)
    {
        GuestMMIO key;
        key.GuestBaseAddr = addr;

        // Find the block after the one containing the address.
        auto pos = std::upper_bound(_writeMappings.begin(), _writeMappings.end(),
                                    key, CompareGuestMappings());

        if (pos != _writeMappings.begin())
        {
            // Move to the block before.
            auto next = pos;
            --pos;

            if ((addr >= pos->GuestBaseAddr) &&
                (addr < (pos->GuestBaseAddr + pos->Size)))
            {
                // The address is within the mapped region.
                region = _writeMappings.data() + std::distance(_writeMappings.begin(), pos);
            }
            else if (next != _writeMappings.end())
            {
                // Return the base address of the next mapped region.
                nextAddr = next->GuestBaseAddr;
            }
        }
    }

    return region;
}

//! @brief Updates the main ROM with data.
//! @param[in] romData A pointer to the data to copy.
//! @param[in] byteCount The count of bytes romData points to.
//! @note The actual ROM size will be rounded up to the nearest 4 KB.
void SystemResources::loadMainRom(const void *romData, uint32_t byteCount)
{
    if (byteCount == 0)
    {
        _mainRom.clear();
    }
    else
    {
        // Round up the size to the nearest 4 KB.
        size_t preferredSize = Bin::roundUpPow2(byteCount, 12);

        // Re-allocate the ROM.
        _mainRom.resize(preferredSize);

        std::memcpy(_mainRom.data(), reinterpret_cast<const uint8_t *>(romData),
                    byteCount);

        if (preferredSize > byteCount)
        {
            // Blank the tail end of the ROM.
            std::memset(_mainRom.data() + byteCount, 0, preferredSize - byteCount);
        }
    }
}

//! @brief Adds a mapping into the guest address space for a block of
//! host-backed memory.
//! @param[in] baseAddr The guest address at which the memory is mapped, should be
//! word aligned.
//! @param[in] size The count of bytes to map, should be a multiple of 4.
//! @param[in] hostBlock A pointer to the block of host memory to map into the
//! guest address space. This should also be word aligned.
void SystemResources::addMemoryMapping(uint32_t baseAddr, uint32_t size,
                                       void *hostBlock)
{
    GuestMMIO region;
    region.GuestBaseAddr = baseAddr;
    region.Size = size;
    region.Context = reinterpret_cast<uintptr_t>(hostBlock);
    region.Read = readGuestMemory;
    region.Write = writeGuestMemory;

    _mappedDevices.push_back(region);
}

//! @brief Adds a mapping into the guest address space for a read-only block of
//! host-backed memory.
//! @param[in] baseAddr The guest address at which the memory is mapped, should be
//! word aligned.
//! @param[in] size The count of bytes to map, should be a multiple of 4.
//! @param[in] hostBlock A pointer to the block of host memory to map into the
//! guest address space. This should also be word aligned.
void SystemResources::addMemoryMapping(uint32_t baseAddr, uint32_t size,
                                       const void *hostBlock)
{
    GuestMMIO region;
    region.GuestBaseAddr = baseAddr;
    region.Size = size;
    region.Context = reinterpret_cast<uintptr_t>(hostBlock);
    region.Read = readGuestMemory;
    region.Write = nullptr;

    _mappedDevices.push_back(region);
}

//! @brief Creates a mapping in the guest address space for memory mapped I/O.
//! @param baseAddr The guest address at which the memory is mapped, should be
//! word aligned.
//! @param[in] size The count of bytes to map, should be a multiple of 4.
//! @param[in] context The value to pass to the read and write functions to
//! provide device-specific context.
//! @param[in] read A pointer to a function to read from the region, nullptr
//! if the region is write-only.
//! @param[in] write A pointer to a function to write to the region, nullptr
//! if the region is read-only.
void SystemResources::addMMIOMappping(uint32_t baseAddr, uint32_t size,
                                      uintptr_t context, ReadMemFn read,
                                      WriteMemFn write)
{
    GuestMMIO region;
    region.GuestBaseAddr = baseAddr;
    region.Size = size;
    region.Context = context;
    region.Read = read;
    region.Write = write;

    _mappedDevices.push_back(region);
}

//! @brief Re-generates a map of the address space based on the blocks mapped
//! into it.
void SystemResources::regenerateMemoryMap()
{
    _readMappings.clear();
    _writeMappings.clear();

    _readMappings.reserve(_mappedDevices.size());
    _writeMappings.reserve(_mappedDevices.size());

    for (const GuestMMIO &mapping : _mappedDevices)
    {
        if (mapping.Read != nullptr)
        {
            _readMappings.push_back(mapping);
        }

        if (mapping.Write != nullptr)
        {
            _writeMappings.push_back(mapping);
        }
    }

    std::sort(_readMappings.begin(), _readMappings.end(),
              CompareGuestMappings());

    std::sort(_writeMappings.begin(), _writeMappings.end(),
              CompareGuestMappings());
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


//! @file ArmEmu/AddressMap.hpp
//! @brief The declaration of an object which indexes IAddressRegion objects
//! by the range of addresses they span.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ADDRESS_REGION_MAP_HPP__
#define __ARM_EMU_ADDRESS_REGION_MAP_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <vector>

#include "Ag/Core/String.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Identifies the type of an IAddressRegion implementation in order to
//! remove the need for runtime dynamic casts.
enum class RegionType : uint8_t
{
    HostBlock,
    MMIO,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ConnectionContext;
class SystemContext;
using SystemContextPtr = SystemContext *;

//! @brief Describes a mapping of a virtual address to a physical address.
struct PageMapping
{
    //! @brief Defines bit patterns for the Access field.
    enum Permissions : uint32_t
    {
        IsPresent = 0x01,
        UserCanRead = 0x02,
        UserCanWrite = 0x04,
        SvcCanRead = 0x08,
        SvcCanWrtie = 0x10,

        Mask = 0x1F,
    };

    //! @brief The virtual address of the first byte of the memory page.
    uint32_t VirtualBaseAddr;

    //! @brief The physical address of the first byte in the memory page.
    uint32_t PageBaseAddr;

    //! @brief The size of the memory page, in bytes.
    //! @note This may not be a power of 2 if the region is unmapped.
    uint32_t PageSize;

    //! @brief Permission flags defined by the Permissions enumeration.
    uint32_t Access;

    PageMapping();
    PageMapping(uint32_t virtBaseAddr, uint32_t physBaseAddr,
                uint32_t size, uint32_t access);
};

//! @brief An interface to an object representing an emulation of a device or
//! memory area within the emulated system.
//! @details Implementations can provide entries in the memory map or merely
//! services which can be called by other memory mapped devices. Equally, they
//! can provide an interface to the host to interoperate with the
//! emulated system.
struct IHardwareDevice
{
    //! @brief Ensures all IHardwareDevice implementations share a common
    //! destructor ancestor.
    virtual ~IHardwareDevice() = default;

    //! @brief Gets a string identifying the region to the user.
    virtual Ag::string_cref_t getName() const = 0;

    //! @brief Gets a string describing the region to the user.
    virtual Ag::string_cref_t getDescription() const = 0;

    //! @brief Connects the device to the wider emulated system before it is
    //! started.
    //! @param[in] context An object which provides useful information and
    //! services before the emulator starts.
    virtual void connect(const ConnectionContext &context) = 0;
};

//! @brief An alias for a pointer to an implementation of the
//! IHardwareDevice interface.
using IHardwreDevicePtr = IHardwareDevice *;

//! @brief An alias for a unique pointer to an IHardwareDevice implementation.
using IHardwareDeviceUPtr = std::unique_ptr<IHardwareDevice>;

//! @brief An alias for a collection of unique pointers to IHardwareDevice
//! implementations which will ensure that each instance is disposed of at
//! destruction.
using HardwareDevicePool = std::vector<IHardwareDeviceUPtr>;

//! @brief An interface to a range of addresses in the guest address map which
//! interface with the host system.
struct IAddressRegion : public IHardwareDevice
{
    //! @brief Ensures all IAddressRegion implementations share a common
    //! destructor ancestor.
    virtual ~IAddressRegion() = default;

    //! @brief Identifies the type of address region the object implements.
    virtual RegionType getType() const = 0;

    //! @brief Gets the count of bytes in the range and of addresses mapped.
    //! @note This must be a whole multiple of 4.
    virtual uint32_t getSize() const = 0;

    // Overrides
    virtual void connect(const ConnectionContext &context) override;
};

//! @brief An alias for a raw pointer to an address region implementation.
using IAddressRegionPtr = IAddressRegion *;

//! @brief An alias for a unique pointer to an IAddressRegion implementation.
using IAddressRegionUPtr = std::unique_ptr<IAddressRegion>;

//! @brief Describes a block of memory shared between the guest and host system
//! which can be writeable or read-only.
struct IHostBlock : public IAddressRegion
{
    //! @brief Gets a pointer to the block of host memory currently mapped
    //! into the guest address space. Successive calls my produce different
    //! host addresses.
    virtual void *getHostAddress() = 0;
};

//! @brief An alias for a pointer to a block of host memory mapped into the
//! guest address space.
using IHostBlockPtr = IHostBlock *;

class AddressMap;

//! @brief A class used to connect emulated devices to the host system.
class ConnectionContext
{
public:
    // Construction/Destruction
    ConnectionContext(SystemContextPtr interopContext,
                      const HardwareDevicePool &devices,
                      const AddressMap &readMap,
                      const AddressMap &writeMap);
    ~ConnectionContext() = default;

    // Accessors
    bool tryFindDevice(Ag::string_cref_t name, IHardwreDevicePtr &device) const;
    SystemContextPtr getInteropContext() const;

    // Operations
private:
    // Internal Fields
    void addDevice(IHardwreDevicePtr device);

    // Internal Fields
    std::unordered_map<Ag::String, IHardwreDevicePtr> _devicesByName;
    SystemContextPtr _interopContext;
};

//! @brief An interface to a range of physical addresses on the emulated system
//! which interface with an emulated hardware device.
struct IMMIOBlock : public IAddressRegion
{
    //! @brief Reads a location in a range of guest addresses.
    //! @param[in] offset The word-aligned offset of the word to read.
    //! @return The word read from the specified location, possibly only
    //! the bottom 8 or 16 bits are significant.
    virtual uint32_t read(uint32_t offset) = 0;

    //! @brief Writes up to 32-bits to a location in a range of guest addresses.
    //! @param[in] offset The word-aligned offset of the byte, half-word or 
    //! word to write.
    //! @param[in] value The value to write to memory.
    virtual void write(uint32_t offset, uint32_t value) = 0;
};

//! @brief An alias for a pointer to a region of guest memory which triggers
//! host code execution on read and/or write.
using IMMIOBlockPtr = IMMIOBlock *;

//! @brief An object which indexes IAddressRegion objects by the range of guest
//! addresses they span.
class AddressMap
{
public:
    // Public Types
    struct Mapping
    {
        IAddressRegion *Region;
        uint32_t Address;
        uint32_t End;

        Mapping();
        Mapping(uint32_t address);
        Mapping(uint32_t address, IAddressRegion *region);

        bool operator<(const Mapping &rhs) const;
        bool isOverlapping(const Mapping &rhs) const;
    };

    using MappingCollection = std::vector<Mapping>;

    // Construction/Destruction
    AddressMap() = default;
    ~AddressMap() = default;

    // Accessors
    const MappingCollection &getMappings() const;
    bool tryFindRegion(uint32_t address, IAddressRegion *&region,
                       uint32_t &offset, uint32_t &remainingLength) const;

    // Operations
    void clear();
    bool tryInsert(uint32_t baseAddress, IAddressRegion *region);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    MappingCollection _mappings;
};

//! @brief An implementation of IHostBlock which maps a fixed block of host
//! memory into the address space of an emulated system.
struct GenericHostBlock : public IHostBlock
{
private:
    Ag::String _name;
    Ag::String _description;
    uint8_t *_data;
    uint32_t _length;
public:
    GenericHostBlock();
    GenericHostBlock(const char *name, const char *desc,
                     uint8_t *hostBlock = nullptr,
                     uint32_t byteCount = 0);

    virtual ~GenericHostBlock() = default;

    // Accessors
    void updateHostMapping(uint8_t *hostBlock, uint32_t byteCount);

    // Overrides
    RegionType getType() const override;
    Ag::string_cref_t getName() const override;
    Ag::string_cref_t getDescription() const override;
    uint32_t getSize() const override;
    void *getHostAddress() override;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

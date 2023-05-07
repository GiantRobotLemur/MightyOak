//! @file ArmEmu/AddressMap.hpp
//! @brief The declaration of an object which indexes IAddressRegion objects
//! by the range of addresses they span.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
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
//! @brief An interface to a range of addresses in the guest address map which
//! interface with the host system.
struct IAddressRegion
{
    //! @brief Ensures all IAddressRegion implementations share a common
    //! destructor ancestor.
    virtual ~IAddressRegion() = default;

    //! @brief Identifies the type of address region the object implements.
    virtual RegionType getType() const = 0;

    //! @brief Gets a string identifying the region to the user.
    virtual Ag::string_cref_t getName() const = 0;

    //! @brief Gets a string describing the region to the user.
    virtual Ag::string_cref_t getDescription() const = 0;

    //! @brief Gets the count of bytes in the range and of addresses mapped.
    //! @note This must be a whole multiple of 4.
    virtual uint32_t getSize() const = 0;
};

//! @brief An alias for a raw pointer to an address region implementation.
using IAddressRegionPtr = IAddressRegion *;

//! @brief An object which can delete an IAddressRegion implementation as part
//! of a unique_ptr.
struct IAddressRegionDeleter
{
    void operator()(IAddressRegion *region) const;
};

//! @brief An alias for a unique pointer to an IAddressRegion implementation.
using IAddressRegionUPtr = std::unique_ptr<IAddressRegion, IAddressRegionDeleter>;

//! @brief An alias for a collection of unique pointers to address region
//! implementations which will ensure that each instance is disposed of at
//! destruction.
using IAddressRegionPool = std::vector<IAddressRegionUPtr>;

//! @brief Describes a block of memory shared between the guest and host system
//! which can be writeable or read-only.
struct IHostBlock : public IAddressRegion
{
    //! @brief Gets a pointer to the block of host memory currently mapped
    //! into the guest address space. Successive calls my produce different
    //! host addresses.
    virtual void *getHostAddress() = 0;
};

//! @brief An alias for a pointer to a block of host memory mapping into the
//! guest address space.
using IHostBlockPtr = IHostBlock *;

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
                   uint8_t *hostBlock, uint32_t byteCount);

    virtual ~GenericHostBlock() = default;

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

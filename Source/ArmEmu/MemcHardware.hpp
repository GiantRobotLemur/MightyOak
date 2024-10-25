//! @file ArmEmu/MemcHardware.hpp
//! @brief The declaration of an object which emulates the hardware of a
//! MEMC-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_MEMC_HARDWARE_HPP__
#define __ARM_EMU_MEMC_HARDWARE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Utils.hpp"

#include "ArmEmu/EmuOptions.hpp"
#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/IOC.hpp"
#include "ArmEmu/VIDC10.hpp"

#include "ArmCore.hpp"
#include "Hardware.inl"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines constant addresses fixed by the MEMC hardware.
struct MEMC
{
    //! @brief The end of the logical address space and start of the physically
    //! mapped RAM.
    static constexpr uint32_t PhysRamStart      = 0x2000000;    // 32 MB

    //! @brief The start of the I/O address space.
    static constexpr uint32_t IOAddrStart       = 0x3000000;    // 48 MB

    //! @brief The start of the I/O address space mapped to the VIDC 10.
    static constexpr uint32_t VidcStart         = 0x3400000;    // 52 MB

    //! @brief The start of the MEMC registers in CAM.
    static constexpr uint32_t MemcStart         = 0x3600000;    // 54 MB

    //! @brief The start of the MEMC address translation table CAM.
    static constexpr uint32_t AddrTransStart    = 0x3800000;    // 56 MB

    //! @brief Marks the end of the MEMC physical address space.
    static constexpr uint32_t AddrSpaceEnd      = 0x4000000;    // 64 MB

    //! @brief Marks the beginning of the low ROM which overlaps CAM.
    static constexpr uint32_t LowRomStart       = 0x3400000;    // 52 MB for 4 MB

    //! @brief Marks the beginning of the high ROM which overlaps CAM.
    static constexpr uint32_t HighRomStart      = 0x3800000;    // 56MB for 8 MB
};

//! @brief Defines results of the MemcHardware address translation functions.
struct AddrMapResult
{
    //! @brief The address does not map to a valid block.
    static constexpr uint8_t NotMapped          = 0x00;

    //! @brief The processor has high enough privileges to access the block.
    static constexpr uint8_t AccessAllowed      = 0x01;

    //! @brief The binary digits to shift to get a 1 to the HasMapping bit.
    static constexpr uint8_t HasMappingShift    = 1;

    //! @brief The address maps to a host block.
    static constexpr uint8_t HasMapping         = static_cast<uint8_t>(1) << HasMappingShift;

    //! @brief The address is mapped and can be accessed.
    static constexpr uint8_t Success            = HasMapping | AccessAllowed;
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which emulates the hardware of a MEMC1/1a-based system.
class MemcHardware : public BasicIrqManagerHardware
{
private:
    // Internal Constants
    static constexpr size_t FuzzSize = 256;

    // Internal Fields
    IOC _ioc;
    VIDC10 _vidc;
    AddressMap _readAddrDecoder;
    AddressMap _writeAddrDecoder;
    std::vector<uint8_t> _ram;
    std::vector<uint8_t> _lowRom;
    std::vector<uint8_t> _highRom;
    std::vector<uint16_t> _pageMappings;
    uint8_t _fuzz[FuzzSize];
    uint32_t _pageOffsetMask;
    uint16_t _physicalPageCount;
    uint8_t _pageSizePow2;
    bool _osMode;
    bool _videoDMAEnabled;
    bool _soundDMAEnabled;

    // Non-cache intensive.
    GenericHostBlock _physicalRamBlock;
    GenericHostBlock _lowRomBlock;
    GenericHostBlock _highRomBlock;
private:
    // Internal Functions
    void setPageSize(uint8_t pageSizePow2);
    void writeMEMC(uint32_t offset, uint32_t value);

    uint8_t translateAddress(uint32_t logicalAddr, uint32_t &physAddr, bool isWrite) const;
    uint8_t tryGetReadHostMapping(uint32_t physAddr, void *&hostBlock,
                                  uint32_t &length);
    uint8_t tryGetWriteHostMapping(uint32_t physAddr, void *&hostBlock,
                                   uint32_t &length);

public:
    // Construction/Destruction
    MemcHardware(const Options &options, const AddressMap &readMap,
                 const AddressMap &writeMap);
    ~MemcHardware() = default;

    // Operations
    void setLowRom(const uint8_t *romBytes, size_t byteCount);
    void setHighRom(const uint8_t *romBytes, size_t byteCount);

    // Overrides
    // For compatibility with GenericHardware.
    void reset();

    // For compatibility with GenericHardware.
    template<typename T>
    bool write(uint32_t logicalAddr, T value)
    {
        void *hostBlock;
        uint32_t length;
        uint8_t result = tryGetWriteHostMapping(logicalAddr, hostBlock, length);
        bool isWritten = true;

        if (result == AddrMapResult::Success)
        {
            // The block maps to host memory and the processor has enough
            // privileges to write to it.
            *reinterpret_cast<T *>(hostBlock) = value;
            isWritten = true;
        }
        else if (result == AddrMapResult::AccessAllowed)
        {
            // The block doesn't map to host memory, but can be written.
            IAddressRegionPtr region;
            uint32_t offset;

            if (logicalAddr >= MEMC::VidcStart)
            {
                // All addresses beyond this point are Content Accessible Memory,
                // i.e. the data doesn't matter, the address encodes the value
                // being written.
                writeMEMC(logicalAddr, replicate(value));
            }
            else if (_writeAddrDecoder.tryFindRegion(logicalAddr, region, offset, length))
            {
                if (region->getType() == RegionType::HostBlock)
                {
                    hostBlock = reinterpret_cast<IHostBlockPtr>(region)->getHostAddress();

                    *Ag::offsetPtr<T>(hostBlock, offset) = value;
                }
                else
                {
                    reinterpret_cast<IMMIOBlockPtr>(region)->write(offset, value);
                }
            }
        }
        else
        {
            // We didn't have authority to write to that address.
            isWritten = false;
        }

        return isWritten;
    }

    // For compatibility with GenericHardware.
    template<typename T>
    bool read(uint32_t logicalAddr, T &value)
    {
        void *hostBlock;
        uint32_t length;
        uint8_t result = tryGetReadHostMapping(logicalAddr, hostBlock, length);
        bool isRead = true;

        if (result == AddrMapResult::Success)
        {
            // The block maps to host memory and the processor has enough
            // privileges to read from it.
            value = *reinterpret_cast<T *>(hostBlock);
        }
        else if (result == AddrMapResult::AccessAllowed)
        {
            // The block doesn't map to host memory, but can be read from.
            IAddressRegionPtr region;
            uint32_t offset;

            if (_readAddrDecoder.tryFindRegion(logicalAddr, region, offset, length))
            {
                if (region->getType() == RegionType::HostBlock)
                {
                    hostBlock = reinterpret_cast<IHostBlockPtr>(region)->getHostAddress();

                    value = *Ag::offsetPtr<T>(hostBlock, offset);
                }
                else
                {
                    // Read from memory mapped I/O.
                    uint32_t word = reinterpret_cast<IMMIOBlockPtr>(region)->read(offset);

                    // Truncate the value.
                    value = static_cast<T>(word);
                }
            }
            else
            {
                // Read static from the unmapped location.
                value = *reinterpret_cast<T *>(_fuzz);
            }
        }
        else
        {
            // We didn't have authority to read from that address.
            isRead = false;
        }

        return isRead;
    }

    // For compatibility with GenericHardware.
    template<typename T>
    bool exchange(uint32_t logicalAddr, T writeValue, T &readValue)
    {
        void *hostBlock;
        uint32_t length;
        uint8_t result = tryGetWriteHostMapping(logicalAddr, hostBlock, length);
        bool isWritten = false;

        // NOTE: Poetic license here: If the address doesn't map to some
        // kind of conventional RAM, raise the abort signal.
        if (result == AddrMapResult::Success)
        {
            T *target = reinterpret_cast<T *>(hostBlock);

            // TODO: Use atomic exchange? Is it worth it?
            readValue = *target;
            *target = writeValue;
            isWritten = true;
        }

        return isWritten;
    }

    bool logicalToPhysicalAddress(uint32_t logicalAddr, PageMapping &mapping) const;

    bool writeWords(uint32_t logicalAddr, const uint32_t *values, uint8_t count);
    bool readWords(uint32_t logicalAddr, uint32_t *results, uint8_t count);

    AddressMap createMasterReadMap();
    AddressMap createMasterWriteMap();
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

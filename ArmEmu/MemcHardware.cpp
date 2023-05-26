//! @file ArmEmu/MemcHardware.cpp
//! @brief The definition of an object which emulates the hardware of a
//! MEMC-based system.
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
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Utils.hpp"

#include "MemcHardware.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief Generates random bytes to appear when memory can be accessed, but the
//! contents is undefined.
struct GenerateFuzz
{
    uint8_t operator()()
    {
        return static_cast<uint8_t>(rand() & 0xFF);
    }
};

//! @brief Defines constants used to encode a MEMC page mapping table.
struct PageMapping
{
    static constexpr uint8_t PPLBitCount = 2;
    static constexpr uint8_t PPLShift = (sizeof(uint16_t) * 8) - PPLBitCount;
    static constexpr uint16_t PPLMask = ((static_cast<uint16_t>(1) << PPLBitCount) - 1) << PPLShift;
    static constexpr uint16_t PageNoMask = (static_cast<uint16_t>(1) << PPLShift) - 1;
};

//! @brief A functor which generates MEMC page map entries which point to a
//! specific area of physical memory, not necessarily the RAM.
struct GenerateRomPageMapping
{
private:
    uint32_t _baseAddr;
    uint16_t _logicalPageNo;
    uint8_t _pageSizePow2;
    uint8_t _ppl;
public:
    GenerateRomPageMapping(uint32_t baseAddr, uint8_t pageSizePow2, uint8_t ppl) :
        _baseAddr(baseAddr),
        _logicalPageNo(0),
        _pageSizePow2(pageSizePow2),
        _ppl(ppl & 0x03)
    {
    }

    uint16_t operator()()
    {
        uint32_t pageSize = static_cast<uint32_t>(1) << _pageSizePow2;
        uint32_t targetPhysicalAddress = _baseAddr + (pageSize * _logicalPageNo);

        ++_logicalPageNo;

        // Ensure all physical page numbers are relative to the physical RAM base.
        uint16_t physPageNo = static_cast<uint16_t>((targetPhysicalAddress - MEMC::PhysRamStart) >> _pageSizePow2);

        return (physPageNo & PageMapping::PageNoMask) |
               (static_cast<uint16_t>(_ppl) << PageMapping::PPLShift);
    }
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// MemcHardware Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Set the current page size and properties dependent upon it.
//! @param[in] pageSizePow2 The new physical page size. Must be between 12
//! and 15 inclusive.
void MemcHardware::setPageSize(uint8_t pageSizePow2)
{
    _pageSizePow2 = pageSizePow2;
    _physicalPageCount = static_cast<uint16_t>(_ram.size() >> _pageSizePow2);
    _pageOffsetMask = (static_cast<uint32_t>(1) << _pageSizePow2) - 1;
}

//! @brief Causes a write to the CAM associated with the MEMC/VIDC registers.
//! @param[in] offset The 26-bit address written to.
void MemcHardware::writeMEMC(uint32_t offset, uint32_t value)
{
    // Mask out bits we don't care about.
    offset &= 0x3FFFFFF;

    if (offset < 0x3600000)
    {
        // Its a write to the VIDC area. The address is not significant,
        // the identifier of the register written to is in buts 24-32, but
        // bits 24-25 should always be zero.
        // For ease of decoding, we omit the two least significant bits.
        uint8_t vidcRegisterId = Ag::Bin::extractBits<uint8_t, 26, 6>(value);

        if (vidcRegisterId < 20) // 0x00 - 0x4C
        {
            // Write to a palette register.
            // 0-15 - video palette.
            // 16 - border
            // 17-19 - cursor
            //uint16_t physicalColour = Ag::Bin::extractBits<uint16_t, 0, 13>(value);
        }
        else if (vidcRegisterId < 24) // 0x50 - ox5C
        {
            // Reserved.
        }
        else if (vidcRegisterId < 32) // 0x60 - 0x7C
        {
            // 24-31 - Stereo image registers
            //uint8_t stereoPosition = Ag::Bin::extractBits<uint8_t, 0, 3>(value);
        }
        else
        {
            switch (vidcRegisterId)
            {
            case 32: // 0x80 - Horizontal cycle register.
            case 33: // 0x84 - Horizontal sync width register.
            case 34: // 0x88 - Horizontal border start register.
            case 35: // 0x8C - Horizontal display start register.
            case 36: // 0x90 - Horizontal display end register.
            case 37: // 0x94 - Horizontal border end register.
            case 38: // 0x98 - Horizontal cursor start register.
            case 39: // 0x9C - Horizontal interlace register.
            case 40: // 0xA0 - Vertical cycle register.
            case 41: // 0xA4 - Vertical sync width register.
            case 42: // 0xA8 - Vertical border start register.
            case 43: // 0xAC - Vertical display start register.
            case 44: // 0xB0 - Vertical display end register.
            case 45: // 0xB4 - Vertical border end register.
            case 46: // 0xB8 - Vertical cursor start register.
            case 47: // 0xBC - Vertical cursor end register.
                // uint16_t data = Ag::Bin::extractBits<uint16_t, 14, 10>(value);

            case 48: // 0xC0 - Sound frequency register.
                // Theoretically 9 bits, but bit 8 is for testing only.
                // uint8_t data = static_cast<uint8_t>(value);

            case 56: // 0xE0 - VIDC Control Register
                // Theoretically 16 bits, but only the least significant
                // 8 bits are useful outside of testing.
                // uint8_t data = static_cast<uint8_t>(value);
                break;
            }
        }
    }
    else if ((offset & 0x3E00000) == 0x3600000)
    {
        // The address is a MEMC register
        switch (Ag::Bin::extractBits<uint8_t, 17, 3>(offset))
        {
        case 0: // Vinit
        case 1: // Vstart
        case 2: // Vend
        case 3: // Cinit
        case 4: // Sstart
        case 5: // SendN
        case 6: // Sptr
            break;

        case 7: // MEMC Control Register
            setPageSize(Ag::Bin::extractBits<uint8_t, 2, 2>(offset) + 12);
            _videoDMAEnabled = Ag::Bin::extractBit<10>(offset);
            _soundDMAEnabled = Ag::Bin::extractBit<11>(offset);
            _osMode = Ag::Bin::extractBit<12>(offset);

            if (Ag::Bin::extractBit<13>(offset))
            {
                // According to the MEMC data sheet page 25:
                // "Test mode must NEVER be enabled during normal operation as
                // it removes all sources of DRAM refresh, and halts the
                // processor."
                throw Ag::OperationException("MEMC test mode enabled!");
            }
            break;
        }
    }
    else if (offset >= MEMC::AddrTransStart)
    {
        // Bits 7 and 12 encode which MEMC chip is being addressed.
        // NOTE: When 4 KB pages are selected, bit 12 is used for the logical
        // page number being mapped, so only duel MEMCs are supported.
        uint8_t memcId = Ag::Bin::extractBit<7>(offset);
        uint8_t pageProtectionLevel = Ag::Bin::extractBits<uint8_t, 8, 2>(offset);

        // The physical page numbers are encoded differently depending on
        // the physical page size. See MEMC data sheet, page 28.
        uint16_t physicalPage;
        uint16_t logicalPage;

        // 128 physical pages x MEMC count.
        switch (_pageSizePow2)
        {
        case 12: // 4 KB pages, only duel MEMC allowed.
                 // 8192 logical pages.
            physicalPage = Ag::Bin::extractBits<uint16_t, 0, 7>(offset);
            logicalPage = Ag::Bin::extractBits<uint16_t, 12, 11>(offset);
            logicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 10, 11, 2>(offset);
            break;

        case 13: // 8 KB pages, quad MEMC allowed.
                 // 4096 logical pages.
            memcId |= static_cast<uint8_t>(offset >> 11) & 2;
            physicalPage = Ag::Bin::extractBits<uint16_t, 1, 6>(offset);
            physicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 0, 6, 1>(offset);
            logicalPage = Ag::Bin::extractBits<uint16_t, 13, 10>(offset);
            logicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 10, 10, 2>(offset);
            break;

        case 14: // 16 KB pages, quad MEMC allowed.
                 // 2048 logical pages.
            memcId |= static_cast<uint8_t>(offset >> 11) & 2;
            physicalPage = Ag::Bin::extractBits<uint16_t, 2, 5>(offset);
            physicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 0, 5, 2>(offset);
            logicalPage = Ag::Bin::extractBits<uint16_t, 14, 9>(offset);
            logicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 10, 9, 2>(offset);
            break;

        case 15: // 32 KB pages, quad MEMC allowed.
                 // 1024 logical pages.
            memcId |= static_cast<uint8_t>(offset >> 11) & 2;
            physicalPage = Ag::Bin::extractBits<uint16_t, 3, 4>(offset);
            physicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 0, 4, 1>(offset);
            physicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 2, 5, 1>(offset);
            physicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 1, 6, 1>(offset);
            logicalPage = Ag::Bin::extractBits<uint16_t, 15, 8>(offset);
            logicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 10, 8, 2>(offset);
            break;

        default:
            pageProtectionLevel = 0;
            logicalPage = physicalPage = 0;
            break;
        }

        // Apply the new mapping.

        // Apply the ID of the MEMC chip being programmed.
        physicalPage |= static_cast<uint16_t>(memcId) << 7;

        // Encode the page protection level.
        physicalPage |= static_cast<uint16_t>(pageProtectionLevel) << PageMapping::PPLShift;

        _pageMappings[logicalPage] = physicalPage;
    }
}

//! @brief Attempts to translate a logical to physical address and determine
//! whether the processor has enough privileges to access it.
//! @param[in] logicalAddr The logical address to translate.
//! @param[out] physAddr Receives the physical address the logical address was
//! mapped to, if any.
//! @param[in] isWrite True if the access check assumes the address is to
//! be written to, false if it is being read from.
//! @return A bit mask defined by the AddrMapResult structure defining whether
//! the processor can perform the memory access and whether the resultant
//! address definitely maps to host memory.
uint8_t MemcHardware::translateAddress(uint32_t logicalAddr, uint32_t &physAddr,
                                       bool isWrite) const
{
    uint8_t result;

    if (logicalAddr & 0xFE000000)
    {
        // The address is not in the lower 32 MB of the address space.
        result = AddrMapResult::NotMapped;
    }
    else
    {
        // The address is backed by host memory.
        result = AddrMapResult::HasMapping;

        uint16_t logicalPageNo = static_cast<uint16_t>(logicalAddr >> _pageSizePow2);
        uint16_t mapping = _pageMappings[logicalPageNo];

        // Calculate the offset of the page based on its number.
        physAddr = (mapping & PageMapping::PageNoMask) << _pageSizePow2;

        // Add that to the base of all physical RAM.
        physAddr += MEMC::PhysRamStart;

        // Apply the offset of the byte to access within the page.
        physAddr += logicalAddr & _pageOffsetMask;

        // Perform branchless access check.
        // 
        // Use the access mode, PPL and operation type (read or write) to
        // define a scalar value 0-31. Look up whether access is allowed
        // based on that value using a pre-calculated set of bits.
        // 
        // See unit test CoreLogic.MemcAccess for how the bits were
        // pre-calculated.
        uint8_t bit = (static_cast<uint8_t>(isPrivilegedMode()) << 1) |
                      static_cast<uint8_t>(_osMode);
        bit |= static_cast<uint8_t>((mapping & PageMapping::PPLMask) >> (PageMapping::PPLShift - 2));
        bit |= isWrite << 4;

        static constexpr uint32_t perms = 0xCCEFEEFF;

        // Use the access mode, access type and PPL to extract a single bit
        // indicating whether access is allowed or not and use that
        // to merge in AddrMapResult::AccessAllowed (which is 0x01).
        result |= static_cast<uint8_t>(perms >> bit) & 1;
    }

    return result;
}

//! @brief Attempts to quickly find the address in host memory which corresponds
//! to a physical memory address to be read from the guest system.
//! @param[in] logicalAddr The logical address to look up.
//! @param[out] hostBlock Receives a pointer to the corresponding byte in
//! host memory.
//! @param[out] length Receives the count of valid bytes of host memory
//! which start at the returned hostBlock address.
//! @param[out] accessDenied Receives a flag indicating if the mapped block could be
//! accessed in the event the function returned true.
//! @retval true A mappings was found, hostBlock and length were returned.
//! @retval false There was no fixed memory block at the specified location,
//! it may still correspond to a readable hardware element.
uint8_t MemcHardware::tryGetReadHostMapping(uint32_t logicalAddr, void *&hostBlock,
                                            uint32_t &length)
{
    // Decode addresses by order of the probability of access.
    uint8_t result = AddrMapResult::NotMapped;

    if (logicalAddr < MEMC::PhysRamStart)
    {
        // It's a translated logical address and so *may* map to host RAM.
        uint32_t physAddr;

        result = translateAddress(logicalAddr, physAddr, false);

        if (result == AddrMapResult::Success)
        {
            if (physAddr < MEMC::LowRomStart)
            {
                // The address was mapped and could be accessed.
                uint32_t offset = (physAddr - MEMC::PhysRamStart) % static_cast<uint32_t>(_ram.size());

                hostBlock = _ram.data() + offset;

                // Calculate the length based on the fact we are accessing a
                // translated memory page.
                uint32_t pageSize = static_cast<uint32_t>(1) << _pageSizePow2;
                uint32_t pageOffset = offset & (pageSize - 1);
                length = pageSize - pageOffset;
                return result;
            }
            else
            {
                // After a reset, the ROM is mapped to the bottom of the
                // logical address space. Allow address decoding to continue.
                logicalAddr = physAddr;
            }
        }
        else
        {
            // The address translated to nothing or the processor
            // didn't have the privileges to access the page.
            return result;
        }
    }

    if (logicalAddr < MEMC::IOAddrStart)
    {
        // It's an untranslated RAM address and definitely maps to host RAM,
        // but can only be access this way if in a privileged processor mode.
        result = AddrMapResult::HasMapping;
        result |= isPrivilegedMode() ? AddrMapResult::AccessAllowed : 0;

        // Calculate the offset based on the fact that the physical RAM
        // repeats throughout the physical address space.
        uint32_t offset = (logicalAddr - MEMC::PhysRamStart) % static_cast<uint32_t>(_ram.size());

        hostBlock = _ram.data() + offset;
        length = static_cast<uint32_t>(_ram.size() - offset);
    }
    else if (logicalAddr < MEMC::LowRomStart)
    {
        // It's memory mapped I/O. Perform an access permissions check
        // to possibly remove the need to search the address decoder map.
        result = isPrivilegedMode() ? AddrMapResult::AccessAllowed :
                                      AddrMapResult::NotMapped;
    }
    else // if (logicalAddr >= MEMC::LowRomStart)
    {
        if (logicalAddr >= MEMC::HighRomStart)
        {
            // Its in the high ROM, which may not exist.
            size_t offset = logicalAddr - MEMC::HighRomStart;

            if (offset < _highRom.size())
            {
                // Return a pointer to the actual ROM.
                hostBlock = _highRom.data() + offset;
                length = static_cast<uint32_t>(_highRom.size() - offset);
            }
            else
            {
                // Produce random data, the most we'll ever need in a
                // single memory transaction.
                hostBlock = _fuzz;
                length = static_cast<uint32_t>(std::size(_fuzz));
            }
        }
        else
        {
            // Its in the low ROM, which should exist.
            size_t offset = logicalAddr - MEMC::LowRomStart;

            if (offset < _lowRom.size())
            {
                // Return a pointer to the actual ROM.
                hostBlock = _lowRom.data() + offset;
                length = static_cast<uint32_t>(_lowRom.size() - offset);
            }
            else
            {
                // Produce random data, the most we'll ever need in a
                // single memory transaction.
                hostBlock = _fuzz;
                length = static_cast<uint32_t>(std::size(_fuzz));
            }
        }

        // The address can be read 
        result = AddrMapResult::Success;
    }

    return result;
}

//! @brief Attempts to quickly find the address in host memory which corresponds
//! to a physical memory address to be written to in the guest system.
//! @param[in] logicalAddr The logical address to look up.
//! @param[out] hostBlock Receives a pointer to the corresponding byte in
//! host memory.
//! @param[out] length Receives the count of valid bytes of host memory
//! which start at the returned hostBlock address.
//! @param[out] accessDenied Receives a flag indicating if the mapped block could be
//! accessed in the event the function returned true.
//! @retval true A mappings was found, hostBlock and length were returned.
//! @retval false There was no fixed memory block at the specified location,
//! it may still correspond to a writeable hardware element.
uint8_t MemcHardware::tryGetWriteHostMapping(uint32_t logicalAddr, void *&hostBlock,
                                             uint32_t &length)
{
    uint8_t result = AddrMapResult::NotMapped;

    if (logicalAddr < MEMC::PhysRamStart)
    {
        // It's a translated logical address and so *may* map to host RAM.
        uint32_t physAddr;

        result = translateAddress(logicalAddr, physAddr, true);

        if (result == AddrMapResult::Success)
        {
            // The address was mapped and could be accessed.
            // Calculate the offset based on the fact that the physical RAM
            // repeats throughout the physical address space.
            uint32_t offset = (physAddr - MEMC::PhysRamStart) % static_cast<uint32_t>(_ram.size());

            hostBlock = _ram.data() + offset;

            // Calculate the length based on the fact we are accessing a
            // translated memory page.
            uint32_t pageSize = static_cast<uint32_t>(1) << _pageSizePow2;
            uint32_t pageOffset = offset & (pageSize - 1);
            length = pageSize - pageOffset;
        }
    }
    else if (logicalAddr < MEMC::IOAddrStart)
    {
        // It's an untranslated RAM address and definitely maps to host RAM,
        // but can only be access this way if in a privileged processor mode.
        result = isPrivilegedMode() ? AddrMapResult::Success :
                                      AddrMapResult::HasMapping;

        uint32_t offset = (logicalAddr - MEMC::PhysRamStart) % static_cast<uint32_t>(_ram.size());

        hostBlock = _ram.data() + offset;
        length = static_cast<uint32_t>(_ram.size() - offset);
    }
    else
    {
        // It's an Memory mapped I/O or CAM address. It can only be accessed
        // in a privileged mode, and doesn't map to host memory.
        result = isPrivilegedMode() ? AddrMapResult::AccessAllowed :
                                      AddrMapResult::NotMapped;
        hostBlock = nullptr;
        length = 0;
    }

    return result;
}

//! @brief Constructs an emulation of hardware based on the MEMC memory
//! controller chip.
//! @param[in] options The configuration of the system to emulate.
//! @param[in] readMap A map of supplementary memory regions which can be
//! read from.
//! @param[in] writeMap A map of supplementary memory regions which can be
//! written to.
MemcHardware::MemcHardware(const Options &options,
                           const AddressMap &readMap,
                           const AddressMap &writeMap) :
    BasicIrqManagerHardware(readMap, writeMap),
    _ioc(*this),
    _vidc(*this),
    _readAddrDecoder(readMap),
    _writeAddrDecoder(writeMap),
    _pageOffsetMask(0),
    _physicalPageCount(0),
    _pageSizePow2(0),
    _osMode(false),
    _videoDMAEnabled(false),
    _soundDMAEnabled(false)
{
    // Generate random fuzz to use when memory can be accessed, but isn't mapped.
    std::generate_n(_fuzz, std::size(_fuzz), GenerateFuzz());

    // Add IOC and VIDC to the address map.
    if ((_readAddrDecoder.tryInsert(0x3200000, &_ioc) == false) ||
        (_writeAddrDecoder.tryInsert(0x3200000, &_ioc) == false))
    {
        throw Ag::OperationException("An I/O device conflicts with IOC at address 0x3200000.");
    }

    if (_writeAddrDecoder.tryInsert(0x3400000, &_vidc) == false)
    {
        throw Ag::OperationException("An I/O device conflicts with VIDC10 at address 0x3400000.");
    }

    // The largest size supported is 16 MB.
    uint32_t ramSize = 16384 * 1024;

    // Hard code the allowed RAM sizes based on MEMC limits.
    uint32_t allowedRamSizesKb[] = {
        512, 1024, 2048, 4096, 8192, 12288
    };

    for (uint32_t limit : allowedRamSizesKb)
    {
        if (options.getRamSizeKb() <= limit)
        {
            ramSize = limit * 1024;
            break;
        }
    }

    // Allocate the RAM and initialize it all to 0.
    _ram.resize(ramSize, 0);
    setPageSize(12);

    // Set up one mapping for each possible logical page.
    _pageMappings.resize(8192, 0);

    // On reset the page mappings will be initialised to a state where the
    // low ROM is mapped to the bottom of the logical address space.
    // 
    // See ARM Family Data Manual Page 4-9.
}

//! @brief Replaces the low ROM with a block of data.
//! @param[in] romBytes The bytes of the ROM image, up to 4 MB.
//! @param[in] byteCount The count of bytes romBytes.
//! @throws Ag::OperationException If romBytes contains more than 4 MB.
void MemcHardware::setLowRom(const uint8_t *romBytes, size_t byteCount)
{
    static constexpr size_t LowRomSize = 0x400000;

    if (byteCount > LowRomSize)
    {
        throw Ag::OperationException("Lower ROM data too large.");
    }

    _lowRom.resize(LowRomSize, 0);
    std::memcpy(_lowRom.data(), romBytes, byteCount);
}

//! @brief Replaces the high ROM with a block of data.
//! @param[in] romBytes The bytes of the ROM image, up to 8 MB.
//! @param[in] byteCount The count of bytes romBytes.
//! @throws Ag::OperationException If romBytes contains more than 8 MB.
void MemcHardware::setHighRom(const uint8_t *romBytes, size_t byteCount)
{
    static constexpr size_t HighRomSize = 0x800000;

    if (byteCount > HighRomSize)
    {
        throw Ag::OperationException("High ROM data too large.");
    }

    _highRom.resize(HighRomSize, 0);
    std::memcpy(_highRom.data(), romBytes, byteCount);
}

// Based on GenericHardware::reset().
void MemcHardware::reset()
{
    setPageSize(12);
    _osMode = false;

    // MEMC Data Sheet page 25: Sound DMA operations are disabled when RESET is
    // asserted. Video/Cursor operations are unaffected by RESET.
    // _videoDMAEnabled = false;
    _soundDMAEnabled = false;

    // Generate a set of mappings which map logical addresses from 0x0000
    // to physical addresses 0x3400000 where the low ROM is positioned.
    // The PPL is set so that the pages are read-only in user mode.
    std::generate(_pageMappings.begin(), _pageMappings.end(),
                  GenerateRomPageMapping(0x3400000, 12, 1));
}

// Based on GenericHardware::writeWords().
bool MemcHardware::writeWords(uint32_t logicalAddr, const uint32_t *values,
                              uint8_t count)
{
    uint8_t wordsWritten = 0;
    bool isWritten = false;

    do
    {
        void *hostBlock;
        uint32_t length;
        uint8_t result = tryGetWriteHostMapping(logicalAddr + (wordsWritten * 4),
                                                hostBlock, length);

        // The abort signal is only raised if the first word cannot be written.
        isWritten |= ((wordsWritten == 0) && (result & AddrMapResult::AccessAllowed)) ||
                     (wordsWritten != 0);

        if (result == AddrMapResult::Success)
        {
            // The block maps to host memory and the processor has enough
            // privileges to write to it.
            uint32_t wordsToWrite = std::min<uint32_t>(count - wordsWritten, length / 4);

            std::copy_n(values + wordsWritten, wordsToWrite,
                        reinterpret_cast<uint32_t *>(hostBlock));

            wordsWritten += static_cast<uint8_t>(wordsToWrite);
        }
        else if (result == AddrMapResult::AccessAllowed)
        {
            // The block doesn't map to host memory, but can be written.
            IAddressRegionPtr region;
            uint32_t offset;

            if (logicalAddr >= MEMC::VidcStart)
            {
                // All addresses beyond this point are Content Accessible Memory
                // (CAM), or to the VIDC, but the register address is encoded in
                // the data..
                uint32_t wordsToWrite = std::min<uint32_t>(count - wordsWritten, length / 4);
                offset = logicalAddr + (wordsWritten * 4);

                for (uint32_t i = 0; i < wordsToWrite; ++i)
                {
                    writeMEMC(offset + (i * 4), values[wordsWritten + i]);
                }

                // Assume we write all remaining words.
                wordsWritten += static_cast<uint8_t>(wordsToWrite);
            }
            else if (_writeAddrDecoder.tryFindRegion(logicalAddr, region, offset, length))
            {
                uint32_t wordsToWrite = std::min<uint32_t>(count - wordsWritten, length / 4);

                if (region->getType() == RegionType::HostBlock)
                {
                    hostBlock = reinterpret_cast<IHostBlockPtr>(region)->getHostAddress();

                    std::copy_n(values + wordsWritten, wordsToWrite,
                                Ag::offsetPtr<uint32_t>(hostBlock, offset));
                }
                else
                {
                    // Write to successive words of memory mapped I/O.
                    IMMIOBlockPtr mmio = reinterpret_cast<IMMIOBlockPtr>(region);

                    for (uint32_t i = 0; i < wordsToWrite; ++i)
                    {
                        mmio->write(offset + (i * 4), values[wordsWritten + i]);
                    }
                }

                wordsWritten += static_cast<uint8_t>(wordsToWrite);
            }
        }
        else
        {
            // We didn't have authority to write to that address, don't
            // bother attempting to write any more.
            break;
        }
    } while (wordsWritten < count);

    return isWritten;
}

// Based on GenericHardware::readWords().
bool MemcHardware::readWords(uint32_t logicalAddr, uint32_t *results, uint8_t count)
{
    uint8_t wordsRead = 0;
    bool isRead = false;

    do
    {
        void *hostBlock;
        uint32_t length;
        uint8_t result = tryGetReadHostMapping(logicalAddr + (wordsRead * 4),
                                               hostBlock, length);

        // Only abort if the first word is not read.
        isRead |= ((wordsRead == 0) && (result & AddrMapResult::AccessAllowed)) ||
                  (wordsRead != 0);

        if (result == AddrMapResult::Success)
        {
            // The block maps to host memory and the processor has enough
            // privileges to read from it.
            uint32_t wordsToRead = std::min<uint32_t>(count - wordsRead, length / 4);

            std::copy_n(reinterpret_cast<uint32_t *>(hostBlock),
                        wordsToRead, results + wordsRead);

            // Update the count.
            wordsRead += static_cast<uint8_t>(wordsToRead);
        }
        else if (result == AddrMapResult::AccessAllowed)
        {
            // The block doesn't map to host memory, but can be read from.
            IAddressRegionPtr region;
            uint32_t offset;

            if (_readAddrDecoder.tryFindRegion(logicalAddr, region, offset, length))
            {
                uint32_t wordsToRead = std::min<uint32_t>(count - wordsRead, length / 4);

                if (region->getType() == RegionType::HostBlock)
                {
                    hostBlock = reinterpret_cast<IHostBlockPtr>(region)->getHostAddress();

                    std::copy_n(Ag::offsetPtr<uint32_t>(hostBlock, offset),
                                wordsToRead, results + wordsRead);
                }
                else
                {
                    // Read from memory mapped I/O.
                    IMMIOBlockPtr mmio = reinterpret_cast<IMMIOBlockPtr>(region); // ->read(offset);

                    for (uint32_t i = 0; i < wordsToRead; ++i)
                    {
                        results[wordsRead + i] = mmio->read(offset + (i * 4));
                    }
                }

                // Update the count.
                wordsRead += static_cast<uint8_t>(wordsToRead);
            }
            else
            {
                // Read static from the unmapped location.
                std::copy_n(reinterpret_cast<uint32_t *>(_fuzz),
                            (count - wordsRead) * 4,
                            results + wordsRead);

                // Update the count.
                wordsRead = count;
            }
        }
        else
        {
            // We didn't have authority to read from that address.
            break;
        }
    } while (wordsRead < count);

    return isRead;
}

// Based on GenericHardware::logicalToPhysicalAddress().
bool MemcHardware::logicalToPhysicalAddress(uint32_t logicalAddr, uint32_t &physAddr) const
{
    bool isMapped;

    if (logicalAddr < MEMC::PhysRamStart)
    {
        // The address is in the logical address space and subject to translation.
        uint8_t result = translateAddress(logicalAddr, physAddr, false);

        isMapped = (result & AddrMapResult::HasMapping);
    }
    else
    {
        // It's not in the logical address space, the mapping is 1:1.
        physAddr = logicalAddr;
        isMapped = true;
    }

    return isMapped;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


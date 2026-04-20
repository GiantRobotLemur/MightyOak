//! @file ArmEmu/MEMC.inl
//! @brief The declaration of an object which emulates the hardware of a
//! MEMC-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_MEMC_INL__
#define __ARM_EMU_MEMC_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Stream.hpp"
#include "Ag/Core/Utils.hpp"

#include "ArmEmu/EmuOptions.hpp"
#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/WatchpointManager.hpp"

#include "ArmCore.hpp"
#include "Hardware.inl"
#include "IOC.hpp"
#include "VIDC10.inl"
#include "AcornKeyboardController.hpp"
#include "I2CBus.hpp"
#include "PCF8583.hpp"

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

    //! @brief The start of the Content Addressable Memory, where data
    //! written doesn't matter, the address bits specify the data.
    static constexpr uint32_t CAMStart          = 0x3400000;    // 52 MB

    //! @brief The end of the Content Addressable Memory region.
    static constexpr uint32_t CAMEnd            = AddrSpaceEnd; // 64 MB
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
struct MemcMapping
{
    // A 16-bit field per logical page describing the mapped physical page, if any.
    // Bits 0-8: Physical Page No.
    // Bits 9-10: Page Protection Level (See MEMC Data Sheet page 26)
    // Bit 11: Page Present
    static constexpr uint8_t MaxPhysRamSizePow2 = 26; // 64 MB - So that ROM can be mapped
                                                      // The base of the logically mapped RAM.
    static constexpr uint8_t MaxPageSizePow2 = 15; // 32 KB
    static constexpr uint8_t MaxPhysPageCountPow2 = MaxPhysRamSizePow2 - MaxPageSizePow2;
    static constexpr uint8_t PhysPageBitCount = MaxPhysPageCountPow2;
    static constexpr uint8_t PPLBitCount = 2; // See MEMC Data Sheet Page 26.

    static constexpr uint8_t PPLShift = MaxPhysPageCountPow2;
    static constexpr uint16_t PPLMask = Ag::Bin::makeMask<uint16_t>(PPLBitCount) << PPLShift;
    static constexpr uint16_t PageNoMask = Ag::Bin::makeMask<uint16_t>(PhysPageBitCount);

    static constexpr uint8_t PagePresentShift = PhysPageBitCount + PPLBitCount;
    static constexpr uint16_t PagePresentBit = static_cast<uint16_t>(1) << (PagePresentShift);
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which emulates the hardware of a MEMC1/1a-based system.
//! @tparam TAllowDiagnostics Used with if constexpr to determine of
//! IDiagnosticSink branches should be taken - otherwise they will be
//! optimised out.
template<bool TAllowDiagnostics>
class MemcHardware : public BasicIrqManagerHardware
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Internal Constants
    ///////////////////////////////////////////////////////////////////////////
    static constexpr size_t FuzzSize = 256;
    static constexpr size_t LowRomSize = 0x400000;
    static constexpr size_t HighRomSize = 0x800000;

    static constexpr bool AllowDiagnostics = TAllowDiagnostics;

public:
    ///////////////////////////////////////////////////////////////////////////
    // Internal Types
    ///////////////////////////////////////////////////////////////////////////
    using IOCType = IOCDevice<AllowDiagnostics>;
    using VIDCType = VIDC10<AllowDiagnostics>;

private:
    ///////////////////////////////////////////////////////////////////////////
    // Internal Fields
    ///////////////////////////////////////////////////////////////////////////
    IOCType _ioc;
    VIDCType _vidc;
    AcornKeyboardController _keyboard;
    I2CBus _i2cBus;
    PCF8583 _cmos;
    AddressMap _readAddrDecoder;
    AddressMap _writeAddrDecoder;
    DMABlock _ram;
    std::vector<uint8_t> _lowRom;
    std::vector<uint8_t> _highRom;
    std::vector<uint16_t> _pageMappings;
    std::vector<uint16_t> _camLogicalPage; //!< CAM entries indexed by physical page.
    std::vector<uint8_t> _camPPL;          //!< PPL per CAM entry.
    uint8_t _fuzz[FuzzSize];
    uint32_t _pageOffsetMask;
    uint16_t _physicalPageCount;
    uint8_t _pageSizePow2;
    bool _osMode;
    bool _videoDMAEnabled;
    bool _soundDMAEnabled;
    bool _romContinuallyEnabled; //!< After reset, ROM overrides MEMC page table.

    // Non-cache intensive.
    GenericHostBlock _physicalRamBlock;
    GenericHostBlock _lowRomBlock;
    GenericHostBlock _highRomBlock;
    IDiagnosticSink *_diagnosticSink;
    WatchpointManager *_watchpoints;

    ///////////////////////////////////////////////////////////////////////////
    // Internal Functions
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Set the current page size and properties dependent upon it.
    //! @param[in] pageSizePow2 The new physical page size. Must be between 12
    //! and 15 inclusive.
    void setPageSize(uint8_t pageSizePow2)
    {
        if (_pageSizePow2 == pageSizePow2)
            return;

        _pageSizePow2 = pageSizePow2;
        _physicalPageCount = static_cast<uint16_t>(std::max(_ram.size() >> _pageSizePow2,
                                                            Ag::toSize(1)));
        _pageOffsetMask = Ag::Bin::makeMask<uint32_t>(_pageSizePow2);

        // Invalidate all page mappings. The logical page numbers stored in
        // _camLogicalPage were decoded from CAM write addresses using the old
        // page size's bit extraction logic. After a page size change, those
        // decoded values are meaningless. On real MEMC hardware, the CAM
        // contents become garbled until software rewrites all entries.
        std::fill(_pageMappings.begin(), _pageMappings.end(),
                  static_cast<uint16_t>(0));
        std::fill(_camLogicalPage.begin(), _camLogicalPage.end(),
                  static_cast<uint16_t>(0xFFFF));
        std::fill(_camPPL.begin(), _camPPL.end(),
                  static_cast<uint8_t>(0));
    }

    //! @brief Causes a write to the CAM associated with the MEMC/VIDC registers.
    //! @param[in] offset The 26-bit address written to.
    void writeMEMC(uint32_t offset, uint32_t value)
    {
        // Mask out bits we don't care about.
        offset &= 0x3FFFFFF;

        if (offset < 0x3600000)
        {
            // It's a write to the VIDC area. The address is not significant;
            // the register ID and data are both encoded in the data word.
            // Forward the raw value to the VIDC10 for decoding and storage.
            _vidc.writeRegister(value);
        }
        else if ((offset & 0x3E00000) == 0x3600000)
        {
            // The address is a MEMC register
            switch (Ag::Bin::extractBits<uint8_t, 17, 3>(offset))
            {
            case 0: // Vinit
                _vidc.setVideoInitAddr(Ag::Bin::extractAndShiftBits<uint32_t, 2, 4, 15>(offset));
                break;
            case 1: // Vstart
                _vidc.setVideoStartAddr(Ag::Bin::extractAndShiftBits<uint32_t, 2, 4, 15>(offset));
                break;
            case 2: // Vend
                _vidc.setVideoEndAddr(Ag::Bin::extractAndShiftBits<uint32_t, 2, 4, 15>(offset));
                break;
            case 3: // Cinit
                _vidc.setCursorInitAddr(Ag::Bin::extractAndShiftBits<uint32_t, 2, 4, 15>(offset));
                break;
            case 4: // Sstart
            case 5: // SendN
            case 6: // Sptr
                break;

            case 7: // MEMC Control Register
            {
                uint8_t newPageSizePow2 = Ag::Bin::extractBits<uint8_t, 2, 2>(offset) + 12;
                bool newOsMode = Ag::Bin::extractBit<12>(offset);
                bool newVideoDMA = Ag::Bin::extractBit<10>(offset);
                bool newSoundDMA = Ag::Bin::extractBit<11>(offset);

                setPageSize(newPageSizePow2);
                _videoDMAEnabled = newVideoDMA;
                _soundDMAEnabled = newSoundDMA;
                _osMode = newOsMode;

                if (Ag::Bin::extractBit<13>(offset))
                {
                    // According to the MEMC data sheet page 25:
                    // "Test mode must NEVER be enabled during normal operation as
                    // it removes all sources of DRAM refresh, and halts the
                    // processor."
                    throw Ag::OperationException("MEMC test mode enabled!");
                }

                // Inform the VIDC if memory contents is being processed into
                // analogue display signals.
                _vidc.setVideoDMAActive(newVideoDMA);
            } break;
            } // switch (registerId)
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
                physicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 1, 5, 1>(offset);
                physicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 2, 6, 1>(offset);
                logicalPage = Ag::Bin::extractBits<uint16_t, 15, 8>(offset);
                logicalPage |= Ag::Bin::extractAndShiftBits<uint16_t, 10, 8, 2>(offset);
                break;

            default:
                pageProtectionLevel = 0;
                logicalPage = physicalPage = 0;
                break;
            }

            // Each MEMC chip provides up to 4 MB of DRAM. Ignore CAM writes
            // addressed to non-existent chips. RISC OS broadcasts CAM writes
            // to all four chip-select addresses; only the chips that physically
            // exist should update the page table.
            size_t memcChipCount = std::max<size_t>(1, _ram.size() >> 22);

            if (memcId >= memcChipCount)
                return;

            // Apply the new mapping using the CAM model.
            // Each MEMC physical page has a CAM entry storing its logical page.
            // The inverse mapping (_pageMappings) is rebuilt from the CAM entries.

            // Compute the absolute physical page index (across MEMC chips).
            physicalPage |= static_cast<uint16_t>(memcId) << 7;

            // Update the CAM entry for this physical page.
            uint16_t oldLogicalPage = _camLogicalPage[physicalPage];

            _camLogicalPage[physicalPage] = logicalPage;
            _camPPL[physicalPage] = pageProtectionLevel;

            // Rebuild the inverse mapping for both the old and new logical pages.
            if (oldLogicalPage != 0xFFFF && oldLogicalPage != logicalPage)
            {
                rebuildInverseMapping(oldLogicalPage);
            }

            rebuildInverseMapping(logicalPage);
        }
    }

    //! @brief Rebuilds the inverse page mapping for a specific logical page by
    //! scanning the CAM entries. On real MEMC hardware, the CAM is searched in
    //! parallel; when multiple physical pages claim the same logical page, the
    //! lowest physical page number wins (priority encoder behaviour).
    //! @param[in] logicalPage The logical page number to rebuild.
    void rebuildInverseMapping(uint16_t logicalPage)
    {
        // Scan CAM entries (indexed by physical page) to find which physical
        // page claims this logical page. Per MEMC1a datasheet section 6.6.2,
        // duplicate entries produce invalid results, so we pick the lowest
        // matching entry (lowest physical page wins).
        uint16_t camEntryCount = static_cast<uint16_t>(_camLogicalPage.size());
        uint16_t firstMatch = 0xFFFF;

        for (uint16_t p = 0; p < camEntryCount; ++p)
        {
            if (_camLogicalPage[p] == logicalPage)
            {
                if (firstMatch == 0xFFFF)
                {
                    firstMatch = p;
                }
            }
        }

        if (firstMatch != 0xFFFF)
        {
            // Use the lowest-numbered matching physical page.
            uint16_t encoded = firstMatch;
            encoded |= static_cast<uint16_t>(_camPPL[firstMatch]) << MemcMapping::PPLShift;
            encoded |= MemcMapping::PagePresentBit;

            _pageMappings[logicalPage] = encoded;
        }
        else
        {
            // No CAM entry maps to this logical page — mark as not present.
            _pageMappings[logicalPage] = 0;
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
    uint8_t translateAddress(uint32_t logicalAddr, uint32_t &physAddr,
                             bool isWrite) const
    {
        uint8_t result;

        if (logicalAddr & 0xFE000000)
        {
            // The address is not in the lower 16 MB of the address space.
            result = AddrMapResult::NotMapped;
        }
        else
        {
            // The address might be backed by host memory.
            uint16_t logicalPageNo = static_cast<uint16_t>(logicalAddr >> _pageSizePow2);
            uint16_t mapping = _pageMappings[logicalPageNo];

            // Determine if the page is mapped using branchless logic.
            result = (mapping & MemcMapping::PagePresentBit) >> (MemcMapping::PagePresentShift - AddrMapResult::HasMappingShift);

            // Calculate the offset of the page based on its number.
            physAddr = (mapping & MemcMapping::PageNoMask) << _pageSizePow2;

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
            bit |= static_cast<uint8_t>((mapping & MemcMapping::PPLMask) >> (MemcMapping::PPLShift - 2));
            bit |= isWrite << 4;

            static constexpr uint32_t perms = 0xCCEFEEFF;

            // Use the access mode, access type and PPL to extract a single bit
            // indicating whether access is allowed or not and use that
            // to merge in AddrMapResult::AccessAllowed (which is 0x01).
            // Only grant access if the page is actually present (HasMapping set).
            uint8_t accessBit = static_cast<uint8_t>(perms >> bit) & 1;
            accessBit &= static_cast<uint8_t>(result >> AddrMapResult::HasMappingShift);
            result |= accessBit;
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
    uint8_t tryGetReadHostMapping(uint32_t logicalAddr, void *&hostBlock,
                                  uint32_t &length)
    {
        // Decode addresses by order of the probability of access.
        uint8_t result = AddrMapResult::NotMapped;

        if (logicalAddr < MEMC::PhysRamStart)
        {
            if (_romContinuallyEnabled)
            {
                // After reset, the ROM is "continually enabled" — all reads
                // from the logical address space (0-32 MB) return high ROM data.
                // MEMC Data Sheet page 12: this overrides when a read is made
                // from an address of 0x02000000 or higher.
                size_t romOffset = logicalAddr % _highRom.size();

                if (!_highRom.empty())
                {
                    hostBlock = _highRom.data() + romOffset;
                    length = static_cast<uint32_t>(_highRom.size() - romOffset);
                }
                else
                {
                    hostBlock = _fuzz;
                    length = static_cast<uint32_t>(std::size(_fuzz));
                }

                return AddrMapResult::Success;
            }

            // It's a translated logical address and so *may* map to host RAM.
            uint32_t physAddr;

            result = translateAddress(logicalAddr, physAddr, false);

            if (result == AddrMapResult::Success)
            {
                // Ensure the ROM is disabled in low address space after a
                // read to high address space.
                _romContinuallyEnabled &= (physAddr < MEMC::PhysRamStart);

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
                    // The page table mapped a logical address to the ROM area.
                    // Allow address decoding to continue using the physical address.
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
        else
        {
            // Accessing an address with Addr[25] high will disable ROM
            // activation caused by reset.
            _romContinuallyEnabled = false;
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
            // The address is in the ROM.

            // Hopefully these two lines should be branchless.
            auto &rom = (logicalAddr < MEMC::HighRomStart) ? _lowRom : _highRom;
            uint32_t romBase = (logicalAddr < MEMC::HighRomStart) ? MEMC::LowRomStart : MEMC::HighRomStart;

            size_t offset = logicalAddr - romBase;

            if (offset < rom.size())
            {
                // Return a pointer to the actual ROM.
                hostBlock = rom.data() + offset;
                length = static_cast<uint32_t>(rom.size() - offset);
            }
            else
            {
                // Produce random data, the most we'll ever need in a
                // single memory transaction.
                hostBlock = _fuzz;
                length = static_cast<uint32_t>(std::size(_fuzz));
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
    uint8_t tryGetWriteHostMapping(uint32_t logicalAddr, void *&hostBlock,
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
                if (physAddr >= MEMC::LowRomStart)
                {
                    // The page table maps to ROM space — writes should be
                    // silently dropped as ROM is read-only.
                    return AddrMapResult::NotMapped;
                }

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
        else // if (logicalAddr < MEMC::CAMStart)
        {
            // It's an Memory mapped I/O or CAM address. It can only be accessed
            // in a privileged mode, and doesn't map to host memory.
            result = isPrivilegedMode() ? AddrMapResult::AccessAllowed :
                AddrMapResult::NotMapped;
            hostBlock = nullptr;
            length = MEMC::CAMEnd - logicalAddr;
        }

        return result;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Constructs an emulation of hardware based on the MEMC memory
    //! controller chip.
    //! @param[in] options The configuration of the system to emulate.
    //! @param[in] readMap A map of supplementary memory regions which can be
    //! read from.
    //! @param[in] writeMap A map of supplementary memory regions which can be
    //! written to.
    MemcHardware(const Options &options, const AddressMap &readMap,
                 const AddressMap &writeMap) :
        BasicIrqManagerHardware(readMap, writeMap),
        _ioc(*this),
        _readAddrDecoder(readMap),
        _writeAddrDecoder(writeMap),
        _pageOffsetMask(0),
        _physicalPageCount(0),
        _pageSizePow2(0),
        _osMode(false),
        _videoDMAEnabled(false),
        _soundDMAEnabled(false),
        _romContinuallyEnabled(false),
        _physicalRamBlock("Physical RAM", "The system RAM without any logical address mapping"),
        _lowRomBlock("Extension ROM", "The low ROM area, usually containing extensions ROMs."),
        _highRomBlock("System ROM", "The high ROM area, usually containing the operating system."),
        _diagnosticSink(nullptr),
        _watchpoints(nullptr)
    {
        // Generate random fuzz to use when memory can be accessed, but isn't mapped.
        std::generate_n(_fuzz, std::size(_fuzz), GenerateFuzz());

        // Add IOC and VIDC to the address map.
        if ((_readAddrDecoder.tryInsert(IOCType::BaseAddr, &_ioc) == false) ||
            (_writeAddrDecoder.tryInsert(IOCType::BaseAddr, &_ioc) == false))
        {
            throw Ag::OperationException("An I/O device conflicts with IOC at address 0x3200000.");
        }

        if (_writeAddrDecoder.tryInsert(MEMC::VidcStart, &_vidc) == false)
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
        _physicalRamBlock.updateHostMapping(_ram.data(),
                                            static_cast<uint32_t>(_ram.size()));

        // Connect VIDC to physical RAM.
        _vidc.initialiseRamAccess(_ram.data(), ramSize);

        // Set the initial page size to 4 KB.
        setPageSize(12);

        // Set up one mapping for each possible logical page.
        _pageMappings.resize(8192, 0);

        // Allocate the CAM arrays: 128 entries per MEMC chip, up to 4 chips.
        // Use 0xFFFF as "no mapping" sentinel for logical page numbers.
        size_t camSize = std::max<size_t>(128, _ram.size() >> 15);
        _camLogicalPage.resize(camSize, 0xFFFF);
        _camPPL.resize(camSize, 0);

        // On reset the page mappings will be initialised to a state where the
        // ROM is "continually enabled" to map it to the bottom of the logical
        // address space.
        // 
        // See ARM Family Data Manual Page 4-9.
        // It doesn't say whether that is the low ROM (0x03400000) or high ROM
        // (0x03800000), but the RISC OS ROMs suggest they are in the high ROM.

        // Load low ROM image.
        _lowRom.clear();
        _lowRom.reserve(LowRomSize);

        // Load the OS into the high ROM.
        _highRom.clear();
        _highRom.resize(HighRomSize, 0);
        _highRomBlock.updateHostMapping(_highRom.data(),
                                        static_cast<uint32_t>(_highRom.size()));

        const auto &romPath = options.getRomPath();

        if (romPath.isEmpty() == false)
        {
            // Open the file and read up to 8 MB.
            if (auto romFile = Ag::IFileStream::open(romPath, Ag::FileAccess::Read |
                                                     Ag::FileAccess::OpenExisting))
            {
                size_t romSize = romFile->read(_highRom.data(), _highRom.size());

                // On real hardware, ROM addresses wrap because higher address
                // lines aren't connected to the ROM chips. Replicate the ROM
                // data to fill the entire 8 MB address space so that aliased
                // addresses return the correct data.
                if (romSize > 0 && romSize < HighRomSize)
                {
                    for (size_t dest = romSize; dest < HighRomSize; dest += romSize)
                    {
                        size_t count = std::min(romSize, HighRomSize - dest);
                        std::memcpy(_highRom.data() + dest, _highRom.data(), count);
                    }
                }
            }

            _highRomBlock.updateHostMapping(_highRom.data(),
                                            static_cast<uint32_t>(_highRom.size()));
        }

        // Wire the I2C bus: register the PCF8583 CMOS/RTC device and connect
        // the bus to IOC control pins C0 (SDA) and C1 (SCL).
        _i2cBus.addDevice(&_cmos);
        _ioc.setI2CBus(&_i2cBus);
    }

    ~MemcHardware() = default;

    ///////////////////////////////////////////////////////////////////////////
    // Accessors
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Gets a direct pointer to the physical RAM.
    const uint8_t *getRamData() const
    {
        return _ram.data();
    }

    //! @brief Gets the total size of physical RAM in bytes.
    uint32_t getRamSize() const
    {
        return static_cast<uint32_t>(_ram.size());
    }

    //! @brief Gets whether video DMA is currently enabled.
    bool isVideoDMAEnabled() const
    {
        return _videoDMAEnabled;
    }

    // Operations

    //! @brief Replaces the low ROM with a block of data.
    //! @param[in] romBytes The bytes of the ROM image, up to 4 MB.
    //! @param[in] byteCount The count of bytes romBytes.
    //! @throws Ag::OperationException If romBytes contains more than 4 MB.
    void setLowRom(const uint8_t *romBytes, size_t byteCount)
    {
        if (byteCount > LowRomSize)
        {
            throw Ag::OperationException("Lower ROM data too large.");
        }

        _lowRom.resize(LowRomSize, 0);
        std::memcpy(_lowRom.data(), romBytes, byteCount);

        // Replicate ROM data to fill 4 MB, matching hardware aliasing behavior.
        if (byteCount > 0 && byteCount < LowRomSize)
        {
            for (size_t dest = byteCount; dest < LowRomSize; dest += byteCount)
            {
                size_t count = std::min(byteCount, LowRomSize - dest);
                std::memcpy(_lowRom.data() + dest, _lowRom.data(), count);
            }
        }

        _lowRomBlock.updateHostMapping(_lowRom.data(), LowRomSize);
    }

    //! @brief Replaces the high ROM with a block of data.
    //! @param[in] romBytes The bytes of the ROM image, up to 8 MB.
    //! @param[in] byteCount The count of bytes romBytes.
    //! @throws Ag::OperationException If romBytes contains more than 8 MB.
    void setHighRom(const uint8_t *romBytes, size_t byteCount)
    {
        if (byteCount > HighRomSize)
        {
            throw Ag::OperationException("High ROM data too large.");
        }

        _highRom.resize(HighRomSize, 0);
        std::memcpy(_highRom.data(), romBytes, byteCount);

        // Replicate ROM data to fill 8 MB, matching hardware aliasing behavior.
        if (byteCount > 0 && byteCount < HighRomSize)
        {
            for (size_t dest = byteCount; dest < HighRomSize; dest += byteCount)
            {
                size_t count = std::min(byteCount, HighRomSize - dest);
                std::memcpy(_highRom.data() + dest, _highRom.data(), count);
            }
        }

        _highRomBlock.updateHostMapping(_highRom.data(), HighRomSize);
    }

    //! @brief Raises the VSync IRQ (IOC IRQ A bit 3) on behalf of the VIDC.
    void raiseVSyncIrq()
    {
        // VSync is IOC IRQ A bit 3 (the IR latched interrupt).
        // This is a latched interrupt - once set, it stays set until the
        // OS explicitly clears it by writing to IRQ Clear register A.
        setGuestIrq(_ioc.raiseVSyncIrq());
    }

    ///////////////////////////////////////////////////////////////////////////
    // Overrides
    ///////////////////////////////////////////////////////////////////////////
    // Based on GenericHardware::reset().
    void MemcHardware::reset()
    {
        // MEMC Data Sheet page 12: After reset the page size is 4 KB and the
        // ROM is "continually enabled" — reads from the logical address space
        // (0-32 MB) return high ROM data until the first access to an address
        // with bit 25 set (>= 0x2000000).
        _pageSizePow2 = 12;
        _physicalPageCount = static_cast<uint16_t>(std::max(_ram.size() >> _pageSizePow2,
                                                            Ag::toSize(1)));
        _pageOffsetMask = Ag::Bin::makeMask<uint32_t>(_pageSizePow2);

        // It is assumed that the first read will be from address 0x00000000, the
        // reset vector. The next read from a 0x02000000 address will disable
        // the ROM activation.
        _romContinuallyEnabled = true;
        _osMode = false;

        // MEMC Data Sheet page 25: Sound DMA operations are disabled when RESET is
        // asserted. Video/Cursor operations are unaffected by RESET.
        _soundDMAEnabled = false;
        _vidc.reset();

        // Mark all page entries as not present. During "continually enabled"
        // mode, reads bypass the page table and return ROM data directly.
        std::fill(_pageMappings.begin(), _pageMappings.end(), static_cast<uint16_t>(0));

        // Clear the CAM entries (all physical pages unmapped).
        std::fill(_camLogicalPage.begin(), _camLogicalPage.end(),
                  static_cast<uint16_t>(0xFFFF));
        std::fill(_camPPL.begin(), _camPPL.end(), static_cast<uint8_t>(0));

        // Set the POR interrupt so that the OS knows it was a hard reset.
        _ioc.powerOnReset();
    }

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

            if (_watchpoints != nullptr &&
                _watchpoints->checkMemoryWrite(logicalAddr, sizeof(T),
                                               static_cast<uint32_t>(value)))
            {
                setDebugIrq(true);
            }

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

            if (_watchpoints != nullptr &&
                _watchpoints->checkMemoryRead(logicalAddr, sizeof(T),
                                              static_cast<uint32_t>(value)))
            {
                setDebugIrq(true);
            }
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

            // Use atomic exchange, it's probably not worth it, but Claude
            // made me do it...
            readValue = std::exchange(*target, writeValue);

            isWritten = true;
        }

        return isWritten;
    }

    // Based on GenericHardware::logicalToPhysicalAddress().
    bool logicalToPhysicalAddress(uint32_t logicalAddr, PageMapping &mapping) const
    {
        if (logicalAddr < MEMC::PhysRamStart)
        {
            // The address is in the logical address space and subject to translation.
            const uint32_t logicalPageNo = logicalAddr >> _pageSizePow2;
            const uint16_t memcMapping = _pageMappings[logicalPageNo];

            mapping.VirtualBaseAddr = logicalPageNo << _pageSizePow2;
            mapping.PageSize = 1u << _pageSizePow2;

            if (memcMapping & MemcMapping::PagePresentBit)
            {
                const uint16_t physPageNo = memcMapping & MemcMapping::PageNoMask;
                const uint16_t pageAccess = memcMapping >> MemcMapping::PPLShift;

                mapping.PageBaseAddr = (static_cast<uint32_t>(physPageNo) << _pageSizePow2) +
                    MEMC::PhysRamStart;
                mapping.Access = PageMapping::SvcCanRead |
                    PageMapping::SvcCanWrtie |
                    PageMapping::IsPresent;

                if (pageAccess < 2)
                {
                    mapping.Access |= PageMapping::UserCanRead;

                    if (pageAccess & 1)
                    {
                        mapping.Access |= PageMapping::UserCanWrite;
                    }
                }
            }
            else
            {
                mapping.PageBaseAddr = 0;
                mapping.Access = 0;
            }
        }
        else
        {
            // It's not in the logical address space, the mapping is 1:1.
            mapping.VirtualBaseAddr = MEMC::PhysRamStart;
            mapping.PageBaseAddr = MEMC::PhysRamStart;
            mapping.PageSize = MEMC::AddrSpaceEnd - MEMC::PhysRamStart;
            mapping.Access = PageMapping::IsPresent |
                PageMapping::SvcCanRead |
                PageMapping::SvcCanWrtie;
        }

        return (mapping.Access & PageMapping::IsPresent);
    }

    // Based on GenericHardware::writeWords().
    bool writeWords(uint32_t logicalAddr, const uint32_t *values, uint8_t count)
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

                if (_watchpoints != nullptr)
                {
                    uint32_t writeAddr = logicalAddr + (wordsWritten * 4);

                    for (uint32_t i = 0; i < wordsToWrite; ++i)
                    {
                        if (_watchpoints->checkMemoryWrite(writeAddr + (i * 4),
                                                           4, values[wordsWritten + i]))
                        {
                            setDebugIrq(true);
                            break;
                        }
                    }
                }

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
                else
                {
                    // The word was written to unmapped memory and lost.
                    ++wordsWritten;
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
    bool readWords(uint32_t logicalAddr, uint32_t *results, uint8_t count)
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

                if (_watchpoints != nullptr)
                {
                    uint32_t readAddr = logicalAddr + (wordsRead * 4);

                    for (uint32_t i = 0; i < wordsToRead; ++i)
                    {
                        if (_watchpoints->checkMemoryRead(readAddr + (i * 4),
                                                          4, results[wordsRead + i]))
                        {
                            setDebugIrq(true);
                            break;
                        }
                    }
                }

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
                                count - wordsRead, results + wordsRead);

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

    // Inherited from BasicIrqManagerHardware.
    AddressMap createMasterReadMap()
    {
        // Create copies of the address maps for non-real-time usage which describe all
        // physical memory areas, including those with optimised address decoding logic.
        AddressMap masterReadAddrMap = _readAddrDecoder;

        bool isOK = true;

        if (_physicalRamBlock.getSize() > 0)
        {
            isOK &= masterReadAddrMap.tryInsert(MEMC::PhysRamStart, &_physicalRamBlock);
        }

        if (isOK && _lowRom.size() > 0)
        {
            isOK &= masterReadAddrMap.tryInsert(MEMC::LowRomStart, &_lowRomBlock);
        }

        if (isOK && _highRom.size() > 0)
        {
            isOK &= masterReadAddrMap.tryInsert(MEMC::HighRomStart, &_highRomBlock);
        }

        if (!isOK)
        {
            throw Ag::OperationException("Fixed address map regions overlap.");
        }

        return masterReadAddrMap;
    }

    // Inherited from BasicIrqManagerHardware.
    AddressMap createMasterWriteMap()
    {
        AddressMap masterWriteAddrMap = _writeAddrDecoder;
        bool isOK = true;

        if (_physicalRamBlock.getSize() > 0)
            isOK = masterWriteAddrMap.tryInsert(MEMC::PhysRamStart, &_physicalRamBlock);

        if (!isOK)
        {
            throw Ag::OperationException("Fixed address map regions overlap.");
        }

        return masterWriteAddrMap;
    }

    // Based on GenericHardware::addIntegralHardware().
    void addIntegralHardware(IHardwareDeviceCollection &devices)
    {
        devices.push_back(&_ioc);
        devices.push_back(&_vidc);
        devices.push_back(&_keyboard);
    }

    // Based on GenericHardware::connect().
    void connect(SystemContext &context)
    {
        context.tryFindTypedDevice("DiagnosticSink", _diagnosticSink);
        context.tryFindTypedDevice("WatchpointManager", _watchpoints);
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/TestHardware.inl
//! @brief The declaration of an implementation of an emulator hardware layer
//! suitable for testing.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_TEST_HARDWARE_INL__
#define __ARM_EMU_TEST_HARDWARE_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/EmuOptions.hpp"
#include "Hardware.inl"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief An example of an implementation of a hardware layer underlying
//! register files and data transfer.
class TestBedHardware : public BasicIrqManagerHardware
{
public:
    // Public Constants
    static constexpr uint32_t RamSize = 32 * 1024;
    static constexpr uint32_t RomSize = 32 * 1024;
    static constexpr uint32_t AddrTop = 0x4000000;

    static constexpr uint32_t RomBase = 0x00000000;
    static constexpr uint32_t RomEnd = RomBase + RomSize;
    static constexpr uint32_t RamBase = RomEnd;
    static constexpr uint32_t RamEnd = RamBase + RamSize;
    static constexpr uint32_t HighRomBase = AddrTop - RomSize;
    static constexpr uint32_t HighRomEnd = AddrTop;

private:
    // Internal Fields
    HostBuffer _rom;
    HostBuffer _ram;
    AddressMap _readAddrDecoder;
    AddressMap _writeAddrDecoder;
    GenericHostBlock _romBlock;
    GenericHostBlock _ramBlock;

    // Internal Functions
    void initialise()
    {
        _rom.resize(RomSize, 0);
        _ram.resize(RamSize, 0);
        _romBlock = GenericHostBlock("ROM", "Main ROM", _rom.data(),
                                     static_cast<uint32_t>(_rom.size()));
        _ramBlock = GenericHostBlock("RAM", "Main RAM", _ram.data(),
                                     static_cast<uint32_t>(_ram.size()));

        _masterReadMap.tryInsert(0, &_romBlock);
        _masterReadMap.tryInsert(HighRomBase, &_romBlock);
        _masterWriteMap.tryInsert(RamBase, &_ramBlock);
        _masterReadMap.tryInsert(RamBase, &_ramBlock);
    }
public:
    // Construction/Destruction
    TestBedHardware()
    {
        initialise();
    }

    TestBedHardware(const Options &/*opts*/)
    {
        initialise();
    }

    TestBedHardware(const Options &/*opts*/, const AddressMap &readMap,
                    const AddressMap &writeMap) :
        BasicIrqManagerHardware(readMap, writeMap),
        _readAddrDecoder(readMap),
        _writeAddrDecoder(writeMap)
    {
        initialise();
    }

    // Accessors
    HostBuffer &getRom() { return _rom; }
    const HostBuffer &getRom() const { return _rom; }
    HostBuffer &getRam() { return _ram; }
    const HostBuffer &getRam() const { return _ram; }

    // Operations
    void reset()
    {
        // Nothing to do?
    }

    template<typename T>
    bool write(uint32_t logicalAddr, T value)
    {
        constexpr uint32_t AddrMask = AlignedAddr<T>::mask;

        bool isWritten = false;
        uint32_t alignedAddr = logicalAddr & AddrMask;

        if (alignedAddr < RamEnd)
        {
            if (alignedAddr >= RamBase)
            {
                *reinterpret_cast<T *>(_ram.data() + alignedAddr - RamBase) = value;
            }

            // NOTE: Writes to ROM are silently ignored.
            isWritten = true;
        }

        return isWritten;
    }

    bool writeWords(uint32_t logicalAddr, const uint32_t *values, uint8_t count)
    {
        // Allow memory protection to determine if the ABORT signal should be raised.
        bool isOK = true;

        // Write the other words ignoring errors.
        for (uint32_t i = 0; i < count; ++i)
        {
            write<uint32_t>(logicalAddr + (4 * i), values[i]);
        }

        return isOK;
    }

    template<typename T> bool read(uint32_t logicalAddr, T &result)
    {
        constexpr uint32_t AddrMask = AlignedAddr<T>::mask;
        constexpr uint64_t DebugPattern = 0xDFDFDFDFDFDFDFDF;

        // Use the memory protection system to determine if ABORT is asserted.
        bool isRead = false;
        uint32_t alignedAddr = logicalAddr & AddrMask;

        if (alignedAddr < RamEnd)
        {
            if (alignedAddr < RamBase)
            {
                // Read from ROM.
                result = *reinterpret_cast<T *>(_rom.data() + alignedAddr - RomBase);
            }
            else
            {
                // Read from RAM.
                result = *reinterpret_cast<T *>(_ram.data() + alignedAddr - RamBase);
            }

            isRead = true;
        }
        else if ((alignedAddr >= HighRomBase) && (alignedAddr < HighRomEnd))
        {
            // Replicate the ROM at the top of the memory.
            result = *reinterpret_cast<T *>(_rom.data() + alignedAddr - HighRomBase);

            isRead = true;
        }
        else
        {
            result = static_cast<T>(DebugPattern);
        }

        return isRead;
    }

    bool readWords(uint32_t logicalAddr, uint32_t *results, uint8_t count)
    {
        // Read the first word to determine if the ABORT signal should be raised.
        bool isOK = read<uint32_t>(logicalAddr, results[0]);

        // Read the other words ignoring errors.
        for (uint32_t i = 1; i < count; ++i)
        {
            if (read<uint32_t>(logicalAddr + (4 * i), results[i]) == false)
            {
                // Mark the result as 'random' for testing purposes.
                results[i] = 0xDFDFDFDF;
            }
        }

        return isOK;
    }

    template<typename T>
    bool exchange(uint32_t logicalAddr, T writeValue, T &readValue)
    {
        constexpr uint32_t AddrMask = AlignedAddr<T>::mask;

        bool isRead = false;
        uint32_t alignedAddr = logicalAddr & AddrMask;

        if (alignedAddr < RamEnd)
        {
            if (alignedAddr < RamBase)
            {
                // Read from ROM but silently fail to write.
                readValue = *reinterpret_cast<T *>(_rom.data() + alignedAddr - RomBase);
            }
            else
            {
                // Read from, then write to, RAM.
                T *hostAddr = reinterpret_cast<T *>(_ram.data() + alignedAddr - RamBase);
                readValue = *hostAddr;
                *hostAddr = writeValue;
            }

            isRead = true;
        }
        else if ((alignedAddr >= HighRomBase) && (alignedAddr < HighRomEnd))
        {
            // Replicate the ROM at the top of the memory.
            readValue = *reinterpret_cast<T *>(_rom.data() + alignedAddr - HighRomBase);
            isRead = true;
        }

        return isRead;
    }

    bool logicalToPhysicalAddress(uint32_t logicalAddr, PageMapping &mapping) const
    {
        // There is no address translation, the mapping from the logical to
        // physical address space is 1:1.

        // Address map:
        // 0x0000 - RomEnd - ROM
        // RomEnd - RamEnd - RAM
        // RamEnd - AddrTop - unmapped.

        if (logicalAddr < RamEnd)
        {
            mapping.VirtualBaseAddr = 0;
            mapping.PageBaseAddr = 0;
            mapping.PageSize = RamEnd;
            mapping.Access = PageMapping::Mask;
        }
        else
        {
            mapping.VirtualBaseAddr = RamEnd;
            mapping.PageBaseAddr = RamEnd;
            mapping.PageSize = AddrTop - RamEnd;
            mapping.Access = 0;
        }

        return (mapping.Access & PageMapping::IsPresent);
    }

    bool tryMapLogicalAddress(uint32_t logicalAddr, bool /*isRead*/,
                              MemoryMapping &mapping)
    {
        bool isMapped = false;

        if (logicalAddr < RamEnd)
        {
            if (logicalAddr < RomEnd)
            {
                mapping.GuestAddress = RomBase;
                mapping.HostAddress = _rom.data();
                mapping.Size = RomSize;
            }
            else
            {
                mapping.GuestAddress = RamBase;
                mapping.HostAddress = _ram.data();
                mapping.Size = RamSize;
            }

            isMapped = true;
        }

        return isMapped;
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

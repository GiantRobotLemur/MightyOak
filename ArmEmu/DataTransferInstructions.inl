//! @file ArmEmu/DataTransferInstructions.inl
//! @brief The declaration of template implementations of ARM data transfer
//! instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_DATA_TRANSFER_INSTRUCTIONS_INL__
#define __ARM_EMU_DATA_TRANSFER_INSTRUCTIONS_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Utils.hpp"

#include "ArmEmu.hpp"
#include "ArmCore.hpp"

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Implements the core LDR instruction to read a word or byte from memory.
//! @tparam THardware The data type of the object representing the physical
//! hardware being emulated which define the memory map which hopefully has an
//! interface like GenericHardware.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] hardware The object representing the emulated system's underlying
//! hardware and memory map.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The instruction bit field.
//! @param[in] baseAddr The base in the address calculation.
//! @param[in] offset The evaluated value of operand 2.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename THardware, typename TRegisterFile>
uint32_t execLoad(THardware &hardware, TRegisterFile &regs,
                  uint32_t instruction, uint32_t baseAddr, uint32_t offset)
{
    uint32_t result = 1;
    uint32_t effectiveAddr = (instruction & 0x800000) ? baseAddr + offset :
                                                        baseAddr - offset;

    if (instruction & 0x1000000)
    {
        // Pre-indexed addressing.
        bool isOK = false;

        if constexpr (TRegisterFile::HasCombinedPcPsr)
        {
            // For 26-bit mode, ensure the top 6 bits are clear.
            if (baseAddr >> 26)
            {
                // Raise Address Exception and exit.
                return regs.raiseAddressException() | 2;
            }
        }

        if (instruction & 0x400000)
        {
            // Load a byte.
            uint8_t value;

            if (hardware.read(effectiveAddr, value))
            {
                isOK = true;
                result = regs.setRn(extractEnum<GeneralRegister, 12, 4>(instruction),
                                    value) | 3;
            }
        }
        else
        {
            // Load a word.
            uint32_t value;

            if (hardware.read(effectiveAddr, value))
            {
                isOK = true;

                // Rotate words read from unaligned addresses.
                value = Bin::rotateRight(value, (effectiveAddr & 0x03) * 8);

                result = regs.setRn(extractEnum<GeneralRegister, 12, 4>(instruction),
                                    value) | 3;
            }
        }

        if (isOK)
        {
            if (instruction & 0x200000)
            {
                // Write-back.
                result = regs.setRn(extractEnum<GeneralRegister, 16, 4>(instruction),
                                    effectiveAddr) | 3;
            }
        }
        else
        {
            // The read failed at the hardware level.
            result = regs.raiseDataAbort() | 2;
        }
    }
    else
    {
        // Post-indexed addressing.
        if constexpr (TRegisterFile::HasCombinedPcPsr)
        {
            // For 26-bit mode, ensure the top 6 bits are clear.
            if (baseAddr >> 26)
            {
                // Raise Address Exception and exit.
                return regs.raiseAddressException() | 2;
            }
        }

        bool isOK = false;
        bool wasPrivilaged = hardware.isPrivilegedMode();

        if (instruction & 0x200000)
        {
            // User mode address translation - store privilege level.
            hardware.setPrivilegedMode(false);
        }

        if (instruction & 0x400000)
        {
            // Load a byte.
            uint8_t value;

            if (hardware.read(baseAddr, value))
            {
                isOK = true;
                result = regs.setRn(extractEnum<GeneralRegister, 12, 4>(instruction),
                                    value) | 3;
            }
        }
        else
        {
            // Load a word.
            uint32_t value;

            if (hardware.read(baseAddr, value))
            {
                // Rotate words read from unaligned addresses.
                value = Bin::rotateRight(value, (baseAddr & 0x03) * 8);
                isOK = true;

                result = regs.setRn(extractEnum<GeneralRegister, 12, 4>(instruction),
                                    value) | 3;
            }
        }

        if (instruction & 0x200000)
        {
            // User mode address translation - restore privilege level.
            hardware.setPrivilegedMode(wasPrivilaged);
        }

        if (isOK)
        {
            // Write-back to base register.
            result = regs.setRn(extractEnum<GeneralRegister, 16, 4>(instruction),
                                effectiveAddr) | 3;
        }
        else
        {
            result = regs.raiseDataAbort() | 2;
        }
    }

    return result;
}

//! @brief Implements the core STR instruction to write a word or byte to memory.
//! @tparam THardware The data type of the object representing the physical
//! hardware being emulated which define the memory map which hopefully has an
//! interface like GenericHardware.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] hardware The object representing the emulated system's underlying
//! hardware and memory map.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The instruction bit field.
//! @param[in] baseAddr The base in the address calculation.
//! @param[in] offset The evaluated value of operand 2.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename THardware, typename TRegisterFile>
uint32_t execStore(THardware &hardware, TRegisterFile &regs,
                   uint32_t instruction, uint32_t baseAddr, uint32_t offset)
{
    uint32_t result = 2;
    uint32_t effectiveAddr = (instruction & 0x800000) ? baseAddr + offset :
                                                        baseAddr - offset;
    uint32_t value = regs.getRd(extractEnum<GeneralRegister, 12, 4>(instruction));

    if (instruction & 0x1000000)
    {
        // Pre-indexed addressing.
        if constexpr (TRegisterFile::HasCombinedPcPsr)
        {
            // For 26-bit mode, ensure the top 6 bits are clear.
            if (effectiveAddr >> 26)
            {
                // The address is greater than the 64 MB address space.
                return regs.raiseAddressException() | 2;
            }
        }

        bool isOK = false;

        if (instruction & 0x400000)
        {
            // Store a byte.
            isOK = hardware.write(effectiveAddr, static_cast<uint8_t>(value));
        }
        else
        {
            // Unaligned addresses will be dealt with at the hardware level.
            isOK = hardware.write(effectiveAddr, value);
        }

        if (isOK)
        {
            if (instruction & 0x200000)
            {
                // Write-back - costs an extra cycle.
                result = regs.setRn(extractEnum<GeneralRegister, 16, 4>(instruction),
                                    effectiveAddr) | 3;
            }
        }
        else
        {
            result = regs.raiseDataAbort() | 2;
        }
    }
    else
    {
        // Post-indexed addressing.
        if constexpr (TRegisterFile::HasCombinedPcPsr)
        {
            // For 26-bit mode, ensure the top 6 bits are clear.
            if (effectiveAddr >> 26)
            {
                // The address is greater than the 64 MB address space.
                return regs.raiseAddressException() | 2;
            }
        }

        bool isOK = false;
        bool wasPrivilaged = hardware.isPrivilegedMode();

        if (instruction & 0x200000)
        {
            // User mode address translation - store privilege level.
            hardware.setPrivilegedMode(false);
        }

        if (instruction & 0x400000)
        {
            // Store a byte.
            isOK = hardware.write(baseAddr, static_cast<uint8_t>(value));
        }
        else
        {
            // Unaligned addresses will be dealt with at the hardware level.
            isOK = hardware.write(baseAddr, value);
        }

        if (instruction & 0x200000)
        {
            // User mode address translation - restore privilege level.
            hardware.setPrivilegedMode(wasPrivilaged);
        }

        if (isOK)
        {
            // Write-back to base register - costs an extra cycle.
            result = regs.setRn(extractEnum<GeneralRegister, 16, 4>(instruction),
                                effectiveAddr) | 3;
        }
        else
        {
            result = regs.raiseDataAbort() | 2;
        }
    }

    return result;
}

//! @brief Implements the core LDM instruction.
//! @tparam THardware The data type of the object representing the physical
//! hardware being emulated which define the memory map which hopefully has an
//! interface like GenericHardware.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] hardware The object representing the emulated system's underlying
//! hardware and memory map.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The instruction bit field.
//! @param[in] baseAddr The address to start loading words from.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename THardware, typename TRegisterFile>
uint32_t execLoadMultiple(THardware &hardware, TRegisterFile &regs,
                          uint32_t instruction, uint32_t baseAddr)
{
    uint8_t regCount = Bin::popCount(static_cast<uint16_t>(instruction));

    // Determine the data to read into a temporary buffer as a single transaction.
    uint32_t transferSize = regCount * 4;
    uint32_t blockStart, blockEnd;

    switch (extractBits<uint8_t, 23, 2>(instruction))
    {
    case 0x00: // LDMDA
        blockStart = baseAddr - transferSize + 4;
        blockEnd = baseAddr - transferSize;
        break;

    case 0x01: // LDMIA
    default:
        blockStart = baseAddr;
        blockEnd = baseAddr + transferSize;
        break;

    case 0x02: // LDMDB
        blockStart = baseAddr - transferSize;
        blockEnd = baseAddr - transferSize;
        break;

    case 0x03: // LDMIB
        blockStart = baseAddr + 4;
        blockEnd = baseAddr + transferSize;
        break;
    }

    if constexpr (TRegisterFile::HasCombinedPcPsr)
    {
        // For 26-bit mode, ensure the top 6 bits are clear.
        if (blockStart >> 26)
        {
            // Raise Address Exception and exit.
            return regs.raiseAddressException() | 2;
        }
    }

    uint32_t values[16];
    uint32_t result = regCount + 2;

    if (hardware.readWords(blockStart, values, regCount))
    {
        // Use bits 15 (Load R15) and 23 (The 'S' bit) to decide what to do
        // with the data we have read.
        uint32_t regList = static_cast<uint16_t>(instruction);
        int32_t regId;
        uint8_t usefulBits = (static_cast<uint8_t>(instruction >> 15) & 0x01) |
                             (static_cast<uint8_t>(instruction >> 21) & 0x02);
        uint8_t index = 0;

        switch (usefulBits)
        {
        case 0x00: // No R15, load into the current register bank.
            if (instruction & 0x200000)
            {
                // Write-back before possibly overwriting the base
                // register from memory.
                GeneralRegister baseReg = extractEnum<GeneralRegister, 16, 4>(instruction);

                if (baseReg != GeneralRegister::R15)
                {
                    regs.setRn(baseReg, blockEnd);
                }
            }

            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                regs.setRn(Ag::forceFromScalar<GeneralRegister>(regId),
                           values[index++]);
            }
            break;

        case 0x01: // Load into the current register bank, including R15 (PC-only).
            result |= ExecResult::FlushPipeline;

            if (instruction & 0x200000)
            {
                // Write-back before possibly overwriting the base
                // register from memory.
                GeneralRegister baseReg = extractEnum<GeneralRegister, 16, 4>(instruction);

                if (baseReg != GeneralRegister::R15)
                {
                    regs.setRn(baseReg, blockEnd);
                }
            }

            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                // Catch the pipeline flush/mode change from writing to R15.
                result |= regs.setRn(Ag::forceFromScalar<GeneralRegister>(regId),
                                     values[index++]);
            }
            break;

        case 0x02: // No R15, load into user register bank.
            if (instruction & 0x200000)
            {
                // Write-back to user bank base register before possibly
                // overwriting the base register from memory.
                GeneralRegister baseReg = extractEnum<GeneralRegister, 16, 4>(instruction);

                if (baseReg != GeneralRegister::R15)
                {
                    regs.setUserRn(baseReg, blockEnd);
                }
            }

            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                regs.setUserRn(Ag::forceFromScalar<GeneralRegister>(regId),
                               values[index++]);
            }
            break;

        case 0x03: // Load into current register bank, including R15 (PC+PSR).
            result |= ExecResult::FlushPipeline;

            if (instruction & 0x200000)
            {
                // Write-back before possibly overwriting the base
                // register from memory.
                GeneralRegister baseReg = extractEnum<GeneralRegister, 16, 4>(instruction);

                if (baseReg != GeneralRegister::R15)
                {
                    regs.setRn(baseReg, blockEnd);
                }
            }

            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                uint32_t value = values[index++];

                // Catch the pipeline flush/mode change from writing to R15.

                // NOTE: R15 will be set last, so repeatedly overwriting the
                // status flags with 0 is OK because the last write will ignore
                // 0 and overwrite with the PSR flags in value.
                result |= regs.setRdAndStatus(Ag::forceFromScalar<GeneralRegister>(regId),
                                              value, 0);
            }
            break;
        }
    }
    else
    {
        // Data abort.
        result = regs.raiseDataAbort() | 2;
    }

    return result;
}

//! @brief Implements the core STM instruction.
//! @tparam THardware The data type of the object representing the physical
//! hardware being emulated which define the memory map which hopefully has an
//! interface like GenericHardware.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] hardware The object representing the emulated system's underlying
//! hardware and memory map.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The instruction bit field.
//! @param[in] baseAddr The base address interpreted from the instruction.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename THardware, typename TRegisterFile>
uint32_t execStoreMultiple(THardware &hardware, TRegisterFile &regs,
                           uint32_t instruction, uint32_t baseAddr)
{
    uint8_t regCount = Bin::popCount(static_cast<uint16_t>(instruction));

    // Determine the data to read into a temporary buffer as a single transaction.
    uint32_t transferSize = regCount * 4;
    uint32_t blockStart, blockEnd;

    switch (extractBits<uint8_t, 23, 2>(instruction))
    {
    case 0x00: // STMDA
        blockStart = baseAddr - transferSize + 4;
        blockEnd = baseAddr - transferSize;
        break;

    case 0x01: // STMIA
    default:
        blockStart = baseAddr;
        blockEnd = baseAddr + transferSize;
        break;

    case 0x02: // STMDB
        blockStart = baseAddr - transferSize;
        blockEnd = baseAddr - transferSize;
        break;

    case 0x03: // STMIB
        blockStart = baseAddr + 4;
        blockEnd = baseAddr + transferSize;
        break;
    }

    if constexpr (TRegisterFile::HasCombinedPcPsr)
    {
        // For 26-bit mode, ensure the top 6 bits are clear.
        if (blockStart >> 26)
        {
            // Raise Address Exception and exit.
            return regs.raiseAddressException() | 2;
        }
    }

    uint32_t values[16];
    uint32_t result = regCount + 1;
    uint32_t regList = static_cast<uint16_t>(instruction);
    int32_t regId;
    uint8_t index = 0;

    if (instruction & 0x400000)
    {
        // Extract values from the user register bank.
        while (Bin::bitScanForward(regList, regId))
        {
            regList ^= 1 << regId;

            values[index++] = regs.getUserRn(Ag::forceFromScalar<GeneralRegister>(regId));
        }
    }
    else
    {
        // Extract the values from the current register bank.
        while (Bin::bitScanForward(regList, regId))
        {
            regList ^= 1 << regId;

            values[index++] = regs.getRx(Ag::forceFromScalar<GeneralRegister>(regId));
        }
    }

    // Attempt to write the words to memory.
    if (hardware.writeWords(blockStart, values, regCount))
    {
        // Perform write-back if required.
        if ((instruction & 0x208000) == 0x200000)
        {
            // Write-back, but not to R15.
            regs.setRn(extractEnum<GeneralRegister, 16, 4>(instruction),
                       blockEnd);
        }
    }
    else
    {
        result = regs.raiseDataAbort() | 2;
    }

    return result;
}

//! @brief Implements the SWP instruction.
//! @tparam THardware The data type of the object representing the physical
//! hardware being emulated which define the memory map which hopefully has an
//! interface like GenericHardware.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] hardware The object representing the emulated system's underlying
//! hardware and memory map.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The instruction bit field.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename THardware, typename TRegisterFile>
uint32_t execSwap(THardware &hardware, TRegisterFile &regs, uint32_t instruction)
{
    uint32_t addr = regs.getRd(extractEnum<GeneralRegister, 16, 4>(instruction));

    if constexpr (TRegisterFile::HasCombinedPcPsr)
    {
        // For 26-bit mode, ensure the top 6 bits are clear.
        if (addr >> 26)
        {
            // Raise Address Exception and exit.
            return regs.raiseAddressException() | 2;
        }
    }

    uint32_t result = 3;
    bool isOK = false;

    // Perform the exchange.
    uint32_t value = regs.getRd(extractEnum<GeneralRegister,0, 4>(instruction));

    if (instruction & 0x400000)
    {
        // Swap byte.
        uint8_t byteValue;

        isOK = hardware.exchange(addr, static_cast<uint8_t>(value), byteValue);
        value = byteValue;
    }
    else
    {
        // Swap word.
        // The underlying hardware will deal with unaligned addresses.
        uint32_t readValue = 0;
        isOK = hardware.exchange(addr, value, readValue);

        // Rotate the bits read from an unaligned address.
        value = Bin::rotateRight(readValue, (addr & 0x03) * 8);
    }

    if (isOK)
    {
        // Update the destination (only PC portion of R15).
        result = regs.setRn(extractEnum<GeneralRegister, 12, 4>(instruction),
                            value);
    }
    else
    {
        // Raise a data abort.
        result = regs.raiseDataAbort() | 1;
    }

    return result;
}

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

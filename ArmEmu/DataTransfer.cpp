//! @file ArmEmu/DataTransfer.cpp
//! @brief The definition of the layer of emulation which provides data
//! transfer functions for an ARM processor core.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////

#include "Ag/Core/Binary.hpp"

#include "DataTransfer.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// DataTransfer Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Implements the core LDR instruction to read a word or byte from memory.
//! @param[in] instruction The instruction bit field.
//! @param[in] baseAddr The base in the address calculation.
//! @param[in] offset The evaluated value of operand 2.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t DataTransfer::execLoad(uint32_t instruction, uint32_t baseAddr,
                                uint32_t offset)
{
    uint32_t result = 2;
    uint32_t effectiveAddr = (instruction & 0x800000) ? baseAddr + offset :
                                                        baseAddr - offset;

    if (instruction & 0x1000000)
    {
        // Pre-indexed addressing.
        bool isOK = false;

        if (!isValidAddress(effectiveAddr))
        {
            // The address is greater than the 64 MB address space.
            raiseAddressException();

            return 2 | IR_FlushPipeline;
        }
        else if (instruction & 0x400000)
        {
            // Load a byte.
            uint8_t value;
            if (readByte(effectiveAddr, value))
            {
                isOK = true;
                result |= setRd(decodeReg(instruction, 12), value, false) ? IR_FlushPipeline : 0;
            }
        }
        else
        {
            // Load a word.
            uint32_t value;
            if (readWord(effectiveAddr & ~0x03, value))
            {
                isOK = true;

                // Rotate words read from unaligned addresses.
                value = Bin::rotateRight(value, (effectiveAddr & 0x03) * 8);

                if (setRd(decodeReg(instruction, 12), value, false))
                {
                    // Updates to PC cost 5 cycles.
                    result = 5 | IR_FlushPipeline;
                }
            }
        }

        if (isOK)
        {
            if (instruction & 0x200000)
            {
                // Write-back.
                if (setRn(decodeReg(instruction, 16), effectiveAddr))
                {
                    // Updates to PC cost 5 cycles.
                    result = 5 | IR_FlushPipeline;
                }
            }
        }
        else
        {
            raiseDataAbort();
            result |= IR_FlushPipeline;
        }
    }
    else
    {
        // Post-indexed addressing.
        if (!isValidAddress(baseAddr))
        {
            // The address is greater than the 64 MB address space.
            raiseAddressException();

            return 2 | IR_FlushPipeline;
        }
        else
        {
            bool isOK = false;
            bool wasPrivilaged = isPrivilegedMode();

            if (instruction & 0x200000)
            {
                // User mode address translation - store privilege level.
                setPrivilegedMode(false);
            }

            if (instruction & 0x400000)
            {
                // Load a byte.
                uint8_t value;
                if (readByte(baseAddr, value))
                {
                    isOK = true;
                    result |= setRd(decodeReg(instruction, 12), value, false) ? IR_FlushPipeline : 0;
                }
            }
            else
            {
                // Load a word.
                uint32_t value;
                if (readWord(baseAddr & ~0x03, value))
                {
                    // Rotate words read from unaligned addresses.
                    value = Bin::rotateRight(value, (baseAddr & 0x03) * 8);
                    isOK = true;

                    if (setRd(decodeReg(instruction, 12), value, false))
                    {
                        // Updates to PC cost 5 cycles.
                        result = 5 | IR_FlushPipeline;
                    }
                }
            }

            if (instruction & 0x200000)
            {
                // User mode address translation - restore privilege level.
                setPrivilegedMode(wasPrivilaged);
            }

            if (isOK)
            {
                // Write-back to base register.
                if (setRn(decodeReg(instruction, 16), effectiveAddr))
                {
                    // Updates to PC cost 5 cycles.
                    result = 5 | IR_FlushPipeline;
                }
            }
            else
            {
                raiseDataAbort();
                result |= IR_FlushPipeline;
            }
        }
    }

    return result;
}

//! @brief Implements the core STR instruction to write a word or byte to memory.
//! @param[in] instruction The instruction bit field.
//! @param[in] baseAddr The base in the address calculation.
//! @param[in] offset The evaluated value of operand 2.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t DataTransfer::execStore(uint32_t instruction, uint32_t baseAddr,
                                 uint32_t offset)
{
    uint32_t result = 2;
    uint32_t effectiveAddr = (instruction & 0x800000) ? baseAddr + offset :
                                                        baseAddr - offset;
    uint32_t value = getRd(decodeReg(instruction, 12));

    if (instruction & 0x1000000)
    {
        // Pre-indexed addressing.
        bool isOK = false;

        if (effectiveAddr >> 26) // (!isValidAddress(effectiveAddr))
        {
            // The address is greater than the 64 MB address space.
            raiseAddressException();

            return 2 | IR_FlushPipeline;
        }
        else if (instruction & 0x400000)
        {
            // Store a byte.
            isOK = writeByte(effectiveAddr, static_cast<uint8_t>(value));
        }
        else
        {
            // Store a word at a word-aligned address - see ARMv2 Data Sheet, page 26.
            isOK = writeWord(effectiveAddr & ~0x03, value);
        }

        if (isOK)
        {
            if (instruction & 0x200000)
            {
                // Write-back.
                if (setRn(decodeReg(instruction, 16), effectiveAddr))
                {
                    // Updates to PC cost 5 cycles.
                    result = 5 | IR_FlushPipeline;
                }
            }
        }
        else
        {
            raiseDataAbort();
            result |= IR_FlushPipeline;
        }
    }
    else
    {
        // Post-indexed addressing.
        if (effectiveAddr >> 26) // (!isValidAddress(baseAddr))
        {
            // The address is greater than the 64 MB address space.
            raiseAddressException();

            return 2 | IR_FlushPipeline;
        }
        else
        {
            bool isOK = false;
            bool wasPrivilaged = isPrivilegedMode();

            if (instruction & 0x200000)
            {
                // User mode address translation - store privilege level.
                setPrivilegedMode(false);
            }

            if (instruction & 0x400000)
            {
                // Store a byte.
                isOK = writeByte(baseAddr, static_cast<uint8_t>(value));
            }
            else
            {
                // Store a word at a word-aligned address - see ARMv2
                // Data Sheet, page 26.

                isOK = writeWord(baseAddr & ~0x03, value);
            }

            if (instruction & 0x200000)
            {
                // User mode address translation - restore privilege level.
                setPrivilegedMode(wasPrivilaged);
            }

            if (isOK)
            {
                // Write-back to base register.
                if (setRn(decodeReg(instruction, 16), effectiveAddr))
                {
                    // Updates to PC cost 5 cycles.
                    result = 5 | IR_FlushPipeline;
                }
            }
            else
            {
                raiseDataAbort();
                result |= IR_FlushPipeline;
            }
        }
    }

    return result;
}

//! @brief Implements the core LDM instruction.
//! @param[in] instruction The instruction bit field.
//! @param[in] baseAddr The address to start loading words from.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t DataTransfer::execLoadMultiple(uint32_t instruction, uint32_t baseAddr)
{
    uint8_t regCount = Bin::popCount(static_cast<uint16_t>(instruction));

    // Determine the data to read into a temporary buffer as a single transaction.
    uint32_t transferSize = regCount * 4;
    uint32_t blockStart, blockEnd;

    switch (static_cast<uint8_t>(instruction >> 23) & 0x03)
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

    uint32_t values[16];
    uint32_t result = regCount + 1;

    if (!isValidAddress(blockStart))
    {
        // The first word to read was beyond the 64MB address space.
        raiseAddressException();
        result = 2 | IR_FlushPipeline;
    }
    else if (readWords(blockStart, values, regCount))
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
                GeneralRegister baseReg = decodeReg(instruction, 16);

                if (baseReg != GeneralRegister::R15)
                {
                    setRn(baseReg, blockEnd);
                }
            }

            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                setRn(toRegisterId(regId), values[index++]);
            }
            break;

        case 0x01: // Load into the current register bank, including R15 (PC-only).
            result |= IR_FlushPipeline;
            if (instruction & 0x200000)
            {
                // Write-back before possibly overwriting the base
                // register from memory.
                GeneralRegister baseReg = decodeReg(instruction, 16);

                if (baseReg != GeneralRegister::R15)
                {
                    setRn(baseReg, blockEnd);
                }
            }

            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                setRn(toRegisterId(regId), values[index++]);
            }
            break;

        case 0x02: // No R15, load into user register bank.
            if (instruction & 0x200000)
            {
                // Write-back to user bank base register before possibly
                // overwriting the base register from memory.
                GeneralRegister baseReg = decodeReg(instruction, 16);

                if (baseReg != GeneralRegister::R15)
                {
                    setUserRn(baseReg, blockEnd);
                }
            }

            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                setUserRn(toRegisterId(regId), values[index++]);
            }
            break;

        case 0x03: // Load into current register bank, including R15 (PC+PSR).
            if (instruction & 0x200000)
            {
                // Write-back before possibly overwriting the base
                // register from memory.
                GeneralRegister baseReg = decodeReg(instruction, 16);

                if (baseReg != GeneralRegister::R15)
                {
                    setRn(baseReg, blockEnd);
                }
            }

            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                setRd(toRegisterId(regId), values[index++], true);
            }
            break;
        }
    }
    else
    {
        // Data abort.
        raiseDataAbort();
        result = 2 | IR_FlushPipeline;
    }

    return result;
}

//! @brief Implements the core STM instruction.
//! @param[in] instruction The instruction bit field.
//! @param[in] baseAddr The base address interpreted from the instruction.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t DataTransfer::execStoreMultiple(uint32_t instruction, uint32_t baseAddr)
{
    uint8_t regCount = Bin::popCount(static_cast<uint16_t>(instruction));

    // Determine the data to read into a temporary buffer as a single transaction.
    uint32_t transferSize = regCount * 4;
    uint32_t blockStart, blockEnd;

    switch (static_cast<uint8_t>(instruction >> 23) & 0x03)
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

    uint32_t result = regCount + 1;

    if (!isValidAddress(blockStart))
    {
        // The first word to read was beyond the 64MB address space.
        raiseAddressException();
        result = 2 | IR_FlushPipeline;
    }
    else
    {
        uint32_t values[16];
        uint32_t regList = static_cast<uint16_t>(instruction);
        int32_t regId;
        uint8_t index = 0;

        if (instruction & 0x400000)
        {
            // Extract values from the user register bank.
            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                values[index++] = getUserRn(toRegisterId(regId));
            }
        }
        else
        {
            // Extract the values from the current register bank.
            while (Bin::bitScanForward(regList, regId))
            {
                regList ^= 1 << regId;

                values[index++] = getRx(toRegisterId(regId));
            }
        }

        // Attempt to write the words to memory.
        if (writeWords(blockStart, values, regCount))
        {
            // Perform write-back if required.
            if ((instruction & 0x208000) == 0x200000)
            {
                // Write-back, but not to R15.
                setRn(decodeReg(instruction, 16), blockEnd);
            }
        }
        else
        {
            raiseDataAbort();
            result = 2 | IR_FlushPipeline;
        }
    }

    return result;
}

//! @brief Implements the SWP instruction.
//! @param[in] instruction The instruction bit field.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t DataTransfer::execSwap(uint32_t instruction)
{
    uint32_t result = 2;
    uint32_t addr = getRd(decodeReg(instruction, 16));
    bool isOK = false;

    if (!isValidAddress(addr))
    {
        // The target address is beyond the 26-bit address space.
        raiseAddressException();
        result = 2 | IR_FlushPipeline;
    }
    else
    {
        // Perform the exchange.
        uint32_t value = getRd(decodeReg(instruction, 0));

        if (instruction & 0x400000)
        {
            // Swap byte.
            uint8_t byteValue = static_cast<uint8_t>(value);

            isOK = exchangeByte(addr, byteValue);
            value = byteValue;
        }
        else
        {
            // Swap word.
            isOK = exchangeWord(addr & ~3u, value);

            // Rotate the bits read from an unaligned address.
            value = Bin::rotateRight(value, (addr & 0x03) * 8);
        }

        if (isOK && setRd(decodeReg(instruction, 12), value, false))
        {
            result |= IR_FlushPipeline;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


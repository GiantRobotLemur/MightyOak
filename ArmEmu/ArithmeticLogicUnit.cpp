//! @file ArmEmu/ArithmeticLogicUnit.cpp
//! @brief The definition of a layer of emulation which provides the arithmetic
//! and logical operations of an ARM processor core.
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
#include "ArithmeticLogicUnit.hpp"
#include "AluOperations.h"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// ArithmeticLogicUnit Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Calculates the PSR status flags after a logical data processing
//! operation.
//! @param[in] result The result of the operation.
//! @param[in] carryOut The carry bit produced by the barrel shifter.
//! @return The new PSR status flag bits.
uint32_t ArithmeticLogicUnit::logicResultStatus(uint32_t result, uint8_t carryOut) const noexcept
{
    // Preserve the overflow flag.
    uint32_t status = static_cast<uint32_t>(getOverflow()) << PsrOverflowShift;
    status |= static_cast<uint32_t>(carryOut & 1) << PsrCarryShift;
    status |= (result == 0) ? PsrZeroMask : 0;
    status |= (result & 0x80000000);

    return status;
}

//! @brief Calculates the PSR status flags after an add operation.
//! @param[in] op1 The first operand to the add operation.
//! @param[in] op2 The second operand to the add operation.
//! @param[in] result The result of the operation.
//! @return The new PSR status flag bits.
constexpr uint32_t ArithmeticLogicUnit::addResultStatus(uint32_t op1, uint32_t op2, uint32_t result) noexcept
{
    uint32_t flags = (~(op1 ^ op2) & (op2 ^ result) & 0x80000000) >> (31 - PsrOverflowShift);
    flags |= (result < op1) ? PsrCarryMask : 0;
    flags |= (result == 0) ? PsrZeroMask : 0;
    flags |= result & 0x80000000;

    return flags;
}

//! @brief Calculates the PSR status flags after an subtract operation.
//! @param[in] op1 The first operand to the subtract operation.
//! @param[in] op2 The second operand to the subtract operation.
//! @param[in] result The result of the operation.
//! @return The new PSR status flag bit
constexpr uint32_t ArithmeticLogicUnit::subResultStatus(uint32_t op1, uint32_t op2, uint32_t result) noexcept
{
    uint32_t flags = ((op1 ^ op2) & (op1 ^ result) & 0x80000000) >> (31 - PsrOverflowShift);
    flags |= (op1 < op2) ? PsrCarryMask : 0;
    flags |= (result == 0) ? PsrZeroMask : 0;
    flags |= result & 0x80000000;

    return flags;
}

//! @brief Extracts the immediate constant from an ALU operation.
//! @param[in] instruction The instruction bit field.
//! @return The immediate constant derived from the instruction.
uint32_t ArithmeticLogicUnit::calculateConstantAluOperand(uint32_t instruction) const
{
    uint32_t value = instruction & 0xFF;

    // Note: The shift is only 4 bits and therefore always even.
    uint8_t shift = (instruction >> 7) & 0x1E;

    return Bin::rotateRight(value, shift);
}

//! @brief Calculates the value of the shifter operand of an ALU instruction.
//! @param[in] instruction An ALU data processing instruction bit field.
//! @param[out] carryOut Receives the carry flag produced by the barrel
//! shifter.
//! @returns The calculated value of the operand, having read registers and
//! shifted the contents as necessary.
//! @note The shifter operand is invalid if bit 7 == 1, which
//! suggests the instruction is a multiply. The results in that situation
//! are undefined.
uint32_t ArithmeticLogicUnit::calculateShiftedAluOperand(uint32_t instruction,
                                                         uint8_t &carryOut) const
{
    uint32_t rmValue = getRm(decodeReg(instruction, 0));
    uint32_t result = 0;
    carryOut = 0;

    if (instruction & 0x10)
    {
        // It's a shift by register, use the lower 8-bits, values greater than 31
        // need to be dealt with.
        uint8_t rsValue = static_cast<uint8_t>(getRs(decodeReg(instruction, 8)));

        if (rsValue == 0)
        {
            // Preserve the operand and carry flag, no matter what the shift mode.
            carryOut = getCarry();
            result = rmValue;
        }
        else
        {
            switch ((instruction >> 5) & 0x03)
            {
            case 0: // LSL
                if (rsValue < 32)
                {
                    result = rmValue << rsValue;
                    carryOut = static_cast<uint8_t>(rmValue >> (32 - rsValue)) & 0x01;
                }
                else if (rsValue == 32)
                {
                    carryOut = static_cast<uint8_t>(rmValue) & 0x01;
                }
                //else if (rsValue > 32) result = 0, carryOut = 0 as already set.
                break;

            case 1: // LSR
                if (rsValue < 32)
                {
                    result = rmValue >> rsValue;
                    carryOut = static_cast<uint8_t>((rmValue >> (rsValue - 1))) & 0x01;
                }
                else if (rsValue == 32)
                {
                    // Equates to Rm, LSR #32.
                    carryOut = static_cast<uint8_t>(rmValue >> 31) & 0x01;
                    result = 0;
                }
                //else if (rsValue > 32) result = 0, carryOut = 0 as already set.
                break;

            case 2: // ASR
                if (rsValue < 32)
                {
                    // Perform a signed shift.
                    result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> rsValue);
                    carryOut = static_cast<uint8_t>((rmValue >> (rsValue - 1))) & 0x01;
                }
                else
                {
                    // Equates to Rm, ASR #32, i.e. bit 31 is replicated throughout and
                    // carried out.
                    carryOut = static_cast<uint8_t>(rmValue >> 31) & 0x01;
                    result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> 31);
                }
                break;

            case 3: // ROR
                if (rsValue != 32)
                {
                    // Calculate a shift value modulus 32.
                    rsValue &= 0x1F;

                    result = Bin::rotateRight(rmValue, static_cast<int32_t>(rsValue));
                    carryOut = static_cast<uint8_t>(rmValue >> (rsValue - 1)) & 0x01;
                }
                else
                {
                    carryOut = rmValue >> 31;
                    result = rmValue;
                }
                break;
            }
        }
    }
    else
    {
        // The shift value is a constant, values can only range from 0-31 where
        // a value of 0 has a very specific interpretation for each shift mode.
        uint8_t rsValue = static_cast<uint8_t>((instruction >> 7) & 0x1F);

        switch ((instruction >> 5) & 0x03)
        {
        case 0: // LSL
            if (rsValue == 0)
            {
                // Preserve the carry flag and the operand.
                carryOut = static_cast<uint8_t>(getPSR() >> PsrCarryShift) & 0x01;
                result = rmValue;
            }
            else
            {
                result = rmValue << rsValue;
                carryOut = static_cast<uint8_t>(rmValue >> (32 - rsValue)) & 0x01;
            }
            break;

        case 1: // LSR
            if (rsValue == 0)
            {
                // Equates to Rm, LSR #32.
                carryOut = static_cast<uint8_t>(rmValue >> 31) & 0x01;
                result = 0;
            }
            else
            {
                result = rmValue >> rsValue;
                carryOut = static_cast<uint8_t>((rmValue >> (rsValue - 1))) & 0x01;
            }
            break;

        case 2: // ASR
            if (rsValue == 0)
            {
                // Equates to Rm, ASR #32, i.e. bit 31 is replicated throughout and
                // carried out.
                carryOut = static_cast<uint8_t>(rmValue >> 31) & 0x01;
                result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> 31);
            }
            else
            {
                // Perform a signed shift.
                result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> rsValue);
                carryOut = static_cast<uint8_t>((rmValue >> (rsValue - 1))) & 0x01;
            }
            break;

        case 3: // ROR/RRX
            if (rsValue == 0)
            {
                // Equates to Rm, RRX
                uint8_t oldCarry = getCarry();
                result = (rmValue >> 1) | (static_cast<uint32_t>(oldCarry) << 31);
                carryOut = static_cast<uint8_t>(rmValue) & 0x01;
            }
            else
            {
                // Calculate a shift value modulus 32.
                rsValue &= 0x1F;

                result = Bin::rotateRight(rmValue, static_cast<int32_t>(rsValue));
                carryOut = static_cast<uint8_t>(rmValue >> (rsValue - 1)) & 0x01;
            }
            break;
        }
    }

    return result;
}

//! @brief Calculates the offset operand of a word/byte LDR/STR instruction.
//! @param[in] instruction The LDR/STR instruction bit field to interpret.
//! @returns The unsigned offset used in the address calculation.
//! @note It is assumed that bit 25 of the instruction bit field is set indicating
//! that the offset is a shifted register, rather than a 12-bit constant.
uint32_t ArithmeticLogicUnit::calculateDataTransferOffset(uint32_t instruction) const
{
    uint32_t rmValue = getRm(decodeReg(instruction, 0));
    uint32_t result = 0;
    uint8_t rsValue = static_cast<uint8_t>(instruction >> 7) & 0x1F;

    switch ((instruction >> 5) & 0x03)
    {
    case 0: // LSL
        if (rsValue == 0)
        {
            // Preserve the carry flag and the operand.
            result = rmValue;
        }
        else
        {
            result = rmValue << rsValue;
        }
        break;

    case 1: // LSR
        if (rsValue == 0)
        {
            // Equates to Rm, LSR #32.
            result = 0;
        }
        else
        {
            result = rmValue >> rsValue;
        }
        break;

    case 2: // ASR
        if (rsValue == 0)
        {
            // Equates to Rm, ASR #32, i.e. bit 31 is replicated throughout and
            // carried out.
            result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> 31);
        }
        else
        {
            // Perform a signed shift.
            result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> rsValue);
        }
        break;

    case 3: // ROR/RRX
        if (rsValue == 0)
        {
            // Equates to Rm, RRX
            uint32_t oldCarry = (getPSR() >> PsrCarryShift) & 0x01;
            result = (rmValue >> 1) | (oldCarry << 31);
        }
        else
        {
            // Calculate a shift value modulus 32.
            rsValue &= 0x1F;

            result = Bin::rotateRight(rmValue, static_cast<int32_t>(rsValue));
        }
        break;
    }

    return result;
}

//! @brief Executes a partially decoded core data processing instruction and
//! updates the status flags in the PSR based on the result or the
//! PC and PSR if it is the destination register.
//! @param[in] instruction The bit field of the instruction to execute.
//! @param[in] op2 The evaluated value of the second operand.
//! @param[in] carryOut The carry value produced by the barrel shifter while
//! evaluating operand 2.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t ArithmeticLogicUnit::execDataProcOpStatus(uint32_t instruction, uint32_t op2,
                                                   uint8_t carryOut) noexcept
{
    uint32_t cycleCount = 1;
    uint32_t op1 = getRn(decodeReg(instruction, 16));
    uint32_t result;
    uint8_t status = 0;
    uint8_t opCode = static_cast<uint8_t>(instruction >> 21) & 0x0F;

    switch (opCode)
    {
    case 0: // AND
        status = (carryOut << 1) | getOverflow();
        result = ALU_And(op1, op2, status);
        break;

    case 1: // EOR
        status = (carryOut << 1) | getOverflow();
        result = ALU_Xor(op1, op2, status);
        break;

    case 2: // SUB
        result = ALU_Sub(op1, op2, status);
        break;

    case 3: // RSB
        result = ALU_Sub(op2, op1, status);
        break;

    case 4: // ADD
        result = ALU_Add(op1, op2, status);
        break;

    case 5: // ADC
        status = getCarry() << 1;
        result = ALU_Adc(op1, op2, status);
        break;

    case 6: // SBC
        status = getCarry() << 1;
        result = ALU_Sbc(op1, op2, status);
        break;

    case 7: // RSC
        status = getCarry() << 1;
        result = ALU_Rsc(op1, op2, status);
        break;

    case 8: // TST
        status = (carryOut << 1) | getOverflow();
        result = ALU_And(op1, op2, status);
        break;

    case 9: // TEQ
        status = (carryOut << 1) | getOverflow();
        result = ALU_Xor(op1, op2, status);
        break;

    case 10: // CMP
        result = ALU_Sub(op1, op2, status);
        break;

    case 11: // CMN
        result = ALU_Add(op1, op2, status);
        break;

    case 12: // ORR
        status = (carryOut << 1) | getOverflow();
        result = ALU_Or(op1, op2, status);
        break;

    case 13: // MOV
    default:
        status = (carryOut << 1) | getOverflow();
        status = ALU_Logic_Flags(op2, status);
        result = op2;
        break;

    case 14: // BIC
        status = (carryOut << 1) | getOverflow();
        result = ALU_Bic(op1, op2, status);
        break;

    case 15: // MVN
        result = ~op2;
        status = (carryOut << 1) | getOverflow();
        status = ALU_Logic_Flags(result, status);
        break;
    }

    GeneralRegister rd = decodeReg(instruction, 12);

    if ((opCode >= 8) && (opCode < 12))
    {
        // It's a TEQ, TST, CMP or CMN instruction.
        if (rd == GeneralRegister::R15)
        {
            // The 'P' suffix is in use. Update the PSR bits directly.
            updatePSR(result);
        }
        else if (rd == GeneralRegister::R0)
        {
            // Update the status flags in the PSR based on the result.
            setStatusFlags(static_cast<uint32_t>(status) << 28);
        }
        else
        {
            // TST/TEQ/CMP/CMN should have a destination of R0 or R15 when
            // used with the 'P' suffix. Anything else is invalid.
            raiseUndefinedInstruction();
            cycleCount |= IR_FlushPipeline;
        }
    }
    else
    {
        if (setRd(rd, result, true))
        {
            // The target was R15, the PC and PSR bits were directly overwritten
            cycleCount |= IR_FlushPipeline;
        }
        else
        {
            // Update the status flags in the PSR based on the result
            // of the operation.
            setStatusFlags(static_cast<uint32_t>(status) << 28);
        }
    }

    return cycleCount;
}

//! @brief Executes a partially decoded core data processing instruction.
//! @param[in] instruction The bit field of the instruction to execute.
//! @param[in] op2 The evaluated value of the second operand.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t ArithmeticLogicUnit::execDataProcOp(uint32_t instruction,
                                             uint32_t op2) noexcept
{
    uint32_t cycleCount = 1;
    uint32_t op1 = getRn(decodeReg(instruction, 16));
    uint32_t result;
    uint8_t opCode = static_cast<uint8_t>(instruction >> 21) & 0x0F;
    bool hasResult = true;

    switch (opCode)
    {
    case 0: // AND
        result = op1 & op2;
        break;

    case 1: // EOR
        result = op1 ^ op2;
        break;

    case 2: // SUB
        result = op1 - op2;
        break;

    case 3: // RSB
        result = op2 - op1;
        break;

    case 4: // ADD
        result = op1 + op2;
        break;

    case 5: // ADC
        result = op1 + op2 + getCarry();
        break;

    case 6: // SBC
        result = op1 - (op2 + getCarry());
        break;

    case 7: // RSC
        result = op2 - (op1 + getCarry());
        break;

    case 8: // TST
        // No implicit 'S' suffix makes this invalid as a data processing op.
        raiseUndefinedInstruction();
        result = 0;
        hasResult = false;
        cycleCount |= IR_FlushPipeline;
        break;

    case 9: // TEQ
        // No implicit 'S' suffix makes this invalid as a data processing op.
        raiseUndefinedInstruction();
        result = 0;
        hasResult = false;
        cycleCount |= IR_FlushPipeline;
        break;

    case 10: // CMP
        // No implicit 'S' suffix makes this invalid as a data processing op.
        raiseUndefinedInstruction();
        result = 0;
        hasResult = false;
        cycleCount |= IR_FlushPipeline;
        break;

    case 11: // CMN
        // No implicit 'S' suffix makes this invalid as a data processing op.
        raiseUndefinedInstruction();
        result = 0;
        hasResult = false;
        cycleCount |= IR_FlushPipeline;
        break;

    case 12: // ORR
        result = op1 | op2;
        break;

    case 13: // MOV
    default:
        result = op2;
        break;

    case 14: // BIC
        result = op1 & ~op2;
        break;

    case 15: // MVN
        result = ~op2;
        break;
    }

    if (hasResult)
    {
        GeneralRegister rd = decodeReg(instruction, 12);

        cycleCount |= setRd(rd, result, false) ? IR_FlushPipeline : 0u;
    }

    return cycleCount;
}

//! @brief Decodes and executes a core multiply instruction.
//! @param[in] instruction The instruction bit field.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
//! @note It is assumed that the instruction mask fits all relevant bits.
uint32_t ArithmeticLogicUnit::execMultiply(uint32_t instruction) noexcept
{
    // ARMv2: Theoretically if Rm == Rd, the result should be 0 for MUL and
    // meaningless for MLA.
    // Also, Rd = R15, no result will be written back unless the 'S' flag
    // is specified, in which case the status flags will be clobbered.

    // Note: The use of getR[snm]() doesn't match the instruction, but is
    // correct for the handling of R15.
    uint32_t cycleCount = 1;
    uint32_t rmValue = getRs(decodeReg(instruction, 0));
    uint32_t rsValue = getRn(decodeReg(instruction, 8));
    GeneralRegister rd = decodeReg(instruction, 16);
    uint32_t result;

    // Inherit the C and V status flags.
    uint8_t status = static_cast<uint8_t>(getPSR() >> 28) & 0x03;

    if (instruction & 0x200000)
    {
        // Its multiply with accumulate.
        result = ALU_Mla(rmValue, rsValue,
                            getRm(decodeReg(instruction, 12)),
                            status);
    }
    else
    {
        result = ALU_Mul(rmValue, rsValue, status);
    }

    // R15 is protected from update.
    if (rd != GeneralRegister::R15)
    {
        setRd(rd, result, false);
    }

    if (instruction & 0x100000)
    {
        // Update status flags.
        uint32_t psr = getPSR() & ~PsrStatusMask;
        psr |= static_cast<uint32_t>(status) << 28;

        setPSR(psr);
    }

    // Simulate the timing of the Booth's multiplier.
    int32_t msb;
    if (Bin::bitScanReverse(rmValue, msb))
    {
        cycleCount += (msb / 2);
    }

    return cycleCount;
}

//! @brief Executes one of the long multiply instructions [UMULL, UMLAL, SMULL, SMLAL].
//! @param[in] instruction The instruction bit field to interpret.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t ArithmeticLogicUnit::execLongMultiply(uint32_t instruction) noexcept
{
    // Given use or R15 is UNPREDICTABLE, we'll assume it gets the PC portion
    // in the unlikely event a long multiply is performed in 26-bit mode.
    uint32_t rmValue = getRn(decodeReg(instruction, 0));
    uint32_t rsValue = getRn(decodeReg(instruction, 8));
    GeneralRegister rdLo = decodeReg(instruction, 12);
    GeneralRegister rdHi = decodeReg(instruction, 16);
    LongWord result;

    // Inherit the C and V status flags.
    uint8_t status = static_cast<uint8_t>(getPSR() >> 28);

    switch (static_cast<uint8_t>(instruction >> 21) & 0x03)
    {
    case 0x00: // UMULL
        status = ALU_Umull(result, rsValue, rmValue, status);
        break;

    case 0x01: // UMLAL
        // Load the accumulator value in 32-bit components.
        result.Components.LoWord = getRn(rdLo);
        result.Components.HiWord = getRn(rdHi);

        status = ALU_Umlal(result, rsValue, rmValue, status);
        break;

    case 0x02: // SMULL
    default:
        status = ALU_Smull(result, rsValue, rmValue, status);
        break;

    case 0x03: // SMLAL
        // Load the accumulator value in 32-bit components.
        result.Components.LoWord = getRn(rdLo);
        result.Components.HiWord = getRn(rdHi);

        status = ALU_Smlal(result, rsValue, rmValue, status);
        break;
    }

    // Write the result, choosing to update the PC-only portion of R15 in this
    // implementation.
    setRd(rdLo, result.Components.LoWord, false);
    setRd(rdHi, result.Components.HiWord, false);

    if (instruction & 0x100000)
    {
        // Update status flags.
        uint32_t psr = getPSR() & ~PsrStatusMask;
        psr |= static_cast<uint32_t>(status) << 28;

        setPSR(psr);
    }

    // On StrongARM: 1..3 result delay cycles, 2 issue cycles.
    // Let's take an average...
    return 3;
}

//! @brief Executes a branch or branch with link instruction.
//! @param[in] instruction The instruction bit field to interpret.
//! @return The instruction execution time and other results defined by the
//! InstructionResult enumeration.
uint32_t ArithmeticLogicUnit::execBranch(uint32_t instruction) noexcept
{
    uint32_t pc = getPC();
    uint32_t cycleCount = 3;

    if (instruction & 0x1000000)
    {
        // Save the address of the instruction after the current one, noting
        // that the PC is 8 bytes ahead of the current instruction.
        // NOTE: Ensure the PSR flags are also stored so that they can be
        // optionally restored on return.
        setRd(GeneralRegister::R14, (pc - 4) | getPSR(), false);
    }

    // Ensure the offset is sign extended to 32 bits.
    int32_t offset = static_cast<int32_t>(instruction << 8) >> 6;

    pc += static_cast<uint32_t>(offset);
    setPC(pc);

    // Always flush pipeline because we are always updating the PC.
    return cycleCount | IR_FlushPipeline;
}

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


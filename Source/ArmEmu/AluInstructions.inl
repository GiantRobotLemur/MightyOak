//! @file ArmEmu/AluInstructions.inl
//! @brief The declaration of template implementations of various ARM ALU
//! instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ALU_INSTRUCTIONS_INL__
#define __ARM_EMU_ALU_INSTRUCTIONS_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Binary.hpp"

#include "ArmCore.hpp"
#include "AluOperations.h"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Calculates the value of the shifter operand of an ALU instruction.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction An ALU data processing instruction bit field.
//! @param[out] carryOut Receives the carry flag produced by the barrel
//! shifter.
//! @returns The calculated value of the operand, having read registers and
//! shifted the contents as necessary.
//! @note The shifter operand is invalid if bit 7 == 1, which
//! suggests the instruction is a multiply. The results in that situation
//! are undefined.
template<typename TRegisterFile>
uint32_t calculateShiftedAluOperand(TRegisterFile &regs, uint32_t instruction,
                                    uint8_t &carryOut) noexcept
{
    uint32_t rmValue = regs.getRm(Ag::Bin::extractEnum<GeneralRegister, 0, 4>(instruction));
    uint32_t result = 0;
    carryOut = 0;

    if (instruction & 0x10)
    {
        // It's a shift by register, use the lower 8-bits, values greater than 31
        // need to be dealt with.
        uint8_t rsValue =
            static_cast<uint8_t>(regs.getRs(Ag::Bin::extractEnum<GeneralRegister, 8, 4>(instruction)));

        if (rsValue == 0)
        {
            // Preserve the operand and carry flag, no matter what the shift mode.
            carryOut = Ag::Bin::extractBit<PsrShift::Carry>(regs.getPSR());
            result = rmValue;
        }
        else
        {
            switch (Ag::Bin::extractBits<uint8_t, 5, 2>(instruction))
            {
            case 0: // LSL
                if (rsValue < 32)
                {
                    result = rmValue << rsValue;
                    carryOut = static_cast<uint8_t>(rmValue >> (32 - rsValue));
                }
                else if (rsValue == 32)
                {
                    carryOut = static_cast<uint8_t>(rmValue);
                }
                //else if (rsValue > 32) result = 0, carryOut = 0 as already set.
                break;

            case 1: // LSR
                if (rsValue < 32)
                {
                    result = rmValue >> rsValue;
                    carryOut = static_cast<uint8_t>((rmValue >> (rsValue - 1)));
                }
                else if (rsValue == 32)
                {
                    // Equates to Rm, LSR #32.
                    carryOut = static_cast<uint8_t>(rmValue >> 31);
                    result = 0;
                }
                //else if (rsValue > 32) result = 0, carryOut = 0 as already set.
                break;

            case 2: // ASR
                if (rsValue < 32)
                {
                    // Perform a signed shift.
                    result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> rsValue);
                    carryOut = static_cast<uint8_t>((rmValue >> (rsValue - 1)));
                }
                else
                {
                    // Equates to Rm, ASR #32, i.e. bit 31 is replicated throughout and
                    // carried out.
                    carryOut = static_cast<uint8_t>(rmValue >> 31);
                    result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> 31);
                }
                break;

            case 3: // ROR
                if (rsValue != 32)
                {
                    // Calculate a shift value modulus 32.
                    rsValue &= 0x1F;

                    result = Ag::Bin::rotateRight(rmValue, static_cast<int32_t>(rsValue));
                    carryOut = static_cast<uint8_t>(rmValue >> (rsValue - 1));
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
        uint8_t rsValue = Ag::Bin::extractBits<uint8_t, 7, 5>(instruction);

        switch (Ag::Bin::extractBits<uint8_t, 5, 2>(instruction))
        {
        case 0: // LSL
            if (rsValue == 0)
            {
                // Preserve the carry flag and the operand.
                carryOut = Ag::Bin::extractBit<PsrShift::Carry>(regs.getPSR());
                result = rmValue;
            }
            else
            {
                result = rmValue << rsValue;
                carryOut = static_cast<uint8_t>(rmValue >> (32 - rsValue));
            }
            break;

        case 1: // LSR
            if (rsValue == 0)
            {
                // Equates to Rm, LSR #32.
                carryOut = static_cast<uint8_t>(rmValue >> 31);
                result = 0;
            }
            else
            {
                result = rmValue >> rsValue;
                carryOut = static_cast<uint8_t>((rmValue >> (rsValue - 1)));
            }
            break;

        case 2: // ASR
            if (rsValue == 0)
            {
                // Equates to Rm, ASR #32, i.e. bit 31 is replicated throughout and
                // carried out.
                carryOut = static_cast<uint8_t>(rmValue >> 31);
                result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> 31);
            }
            else
            {
                // Perform a signed shift.
                result = static_cast<uint32_t>(static_cast<int32_t>(rmValue) >> rsValue);
                carryOut = static_cast<uint8_t>((rmValue >> (rsValue - 1)));
            }
            break;

        case 3: // ROR/RRX
            if (rsValue == 0)
            {
                // Equates to Rm, RRX
                uint8_t oldCarry = Ag::Bin::extractBit<PsrShift::Carry>(regs.getPSR());
                result = (rmValue >> 1) | (static_cast<uint32_t>(oldCarry) << 31);
                carryOut = static_cast<uint8_t>(rmValue);
            }
            else
            {
                // Calculate a shift value modulus 32.
                rsValue &= 0x1F;

                result = Ag::Bin::rotateRight(rmValue, static_cast<int32_t>(rsValue));
                carryOut = static_cast<uint8_t>(rmValue >> (rsValue - 1));
            }
            break;
        }
    }


    // Ensure only the LSB of carryOut is relevant.
    carryOut &= 1;
    return result;
}

//! @brief Extracts the immediate constant from an ALU operation.
//! @param[in] instruction The instruction bit field.
//! @return The immediate constant derived from the instruction.
inline uint32_t calculateConstantAluOperand(uint32_t instruction) noexcept
{
    uint32_t value = instruction & 0xFF;

    // Note: The shift is only 4 bits and therefore always even.
    uint8_t shift = (instruction >> 7) & 0x1E;

    return Ag::Bin::rotateRight(value, shift);
}

//! @brief Calculates the offset operand of a word/byte LDR/STR instruction.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The LDR/STR instruction bit field to interpret.
//! @returns The unsigned offset used in the address calculation.
//! @note It is assumed that bit 25 of the instruction bit field is set indicating
//! that the offset is a shifted register, rather than a 12-bit constant.
template<typename TRegisterFile>
uint32_t calculateDataTransferOffset(TRegisterFile & regs, uint32_t instruction) noexcept
{
    uint32_t rmValue = regs.getRm(Ag::Bin::extractEnum<GeneralRegister, 0, 4>(instruction));
    uint32_t result = 0;
    uint8_t rsValue = Ag::Bin::extractBits<uint8_t, 7, 5>(instruction);

    switch (Ag::Bin::extractBits<uint8_t, 5, 2>(instruction))
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
            uint32_t oldCarry = Ag::Bin::extractBit<PsrShift::Carry>(regs.getPSR());
            result = (rmValue >> 1) | (oldCarry << 31);
        }
        else
        {
            // Calculate a shift value modulus 32.
            rsValue &= 0x1F;

            result = Ag::Bin::rotateRight(rmValue, static_cast<int32_t>(rsValue));
        }
        break;
    }

    return result;
}

//! @brief Executes a partially decoded core data processing instruction and
//! updates the status flags in the PSR based on the result or the
//! PC and PSR if it is the destination register.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The bit field of the instruction to execute.
//! @param[in] op2 The evaluated value of the second operand.
//! @param[in] carryOut The carry value produced by the barrel shifter while
//! evaluating operand 2.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename TRegisterFile>
uint32_t execDataProcOpStatus(TRegisterFile &regs, uint32_t instruction,
                              uint32_t op2, uint8_t carryOut) noexcept
{
    uint32_t cycleCount = 1;
    uint32_t op1 = regs.getRn(Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction));
    uint32_t result;
    uint8_t status = 0;
    uint8_t opCode = Ag::Bin::extractBits<uint8_t, 21, 4>(instruction);

    switch (opCode)
    {
    case 0: // AND
        // Combine ALU carry out with inherited overflow.
        status = (carryOut << PsrShift::LowCarry) |
                 Ag::Bin::extractBit<PsrShift::Overflow>(regs.getPSR());
        result = ALU_And(op1, op2, status);
        break;

    case 1: // EOR
        // Combine ALU carry out with inherited overflow.
        status = (carryOut << PsrShift::LowCarry) |
                 Ag::Bin::extractBit<PsrShift::Overflow>(regs.getPSR());
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
        // Inherit the current state of the carry flag.
        status = (regs.getPSR() >> PsrShift::Status) & PsrMask::LowCarry;
        result = ALU_Adc(op1, op2, status);
        break;

    case 6: // SBC
        // Inherit the current state of the carry flag.
        status = (regs.getPSR() & PsrMask::Carry) >> PsrShift::Status;
        result = ALU_Sbc(op1, op2, status);
        break;

    case 7: // RSC
        // Inherit the current state of the carry flag.
        status = (regs.getPSR() & PsrMask::Carry) >> PsrShift::Status;
        result = ALU_Rsc(op1, op2, status);
        break;

    case 8: // TST
        // Combine ALU carry out with inherited overflow.
        status = (carryOut << PsrShift::LowCarry) |
                 Ag::Bin::extractBit<PsrShift::Overflow>(regs.getPSR());
        result = ALU_And(op1, op2, status);
        break;

    case 9: // TEQ
        // Combine ALU carry out with inherited overflow.
        status = (carryOut << PsrShift::LowCarry) |
                 Ag::Bin::extractBit<PsrShift::Overflow>(regs.getPSR());
        result = ALU_Xor(op1, op2, status);
        break;

    case 10: // CMP
        result = ALU_Sub(op1, op2, status);
        break;

    case 11: // CMN
        result = ALU_Add(op1, op2, status);
        break;

    case 12: // ORR
        status = (carryOut << PsrShift::LowCarry) |
                 Ag::Bin::extractBit<PsrShift::Overflow>(regs.getPSR());
        result = ALU_Or(op1, op2, status);
        break;

    case 13: // MOV
    default:
        // Combine ALU carry out with inherited overflow.
        status = (carryOut << PsrShift::LowCarry) |
                 Ag::Bin::extractBit<PsrShift::Overflow>(regs.getPSR());
        status = ALU_Logic_Flags(op2, status);
        result = op2;
        break;

    case 14: // BIC
        // Combine ALU carry out with inherited overflow.
        status = (carryOut << PsrShift::LowCarry) |
                 Ag::Bin::extractBit<PsrShift::Overflow>(regs.getPSR());
        result = ALU_Bic(op1, op2, status);
        break;

    case 15: // MVN
        result = ~op2;
        // Combine ALU carry out with inherited overflow.
        status = (carryOut << PsrShift::LowCarry) |
                 Ag::Bin::extractBit<PsrShift::Overflow>(regs.getPSR());
        status = ALU_Logic_Flags(result, status);
        break;
    }

    GeneralRegister rd = Ag::Bin::extractEnum<GeneralRegister, 12, 4>(instruction);

    if constexpr (TRegisterFile::HasCombinedPcPsr)
    {
        // It's 26-bit mode where 26-bit PC is combined with PSR.
        // PSR can be updated by comparison instructions with the
        // 'P' suffix, i.e. Rd == R15.
        if ((opCode >= 8) && (opCode < 12))
        {
            // It's a TEQ, TST, CMP or CMN instruction.
            if (rd == GeneralRegister::R0)
            {
                // Update the status flags in the PSR based on the result.
                regs.setStatusFlags(status);
            }
            else if (rd == GeneralRegister::R15)
            {
                // The 'P' suffix is in use. Update the PSR bits directly.
                cycleCount |= regs.updatePSR(result);
            }
            else
            {
                // TST/TEQ/CMP/CMN should have a destination of R0 or R15 when
                // used with the 'P' suffix. Anything else is invalid.
                cycleCount |= regs.raiseUndefinedInstruction();
            }
        }
        else
        {
            // If rd == R15 the entire result is written to the PC/PSR with
            // only privileged PSR bits protected due to the current processor
            // mode.
            // else The status of the result is written to the status flags.
            cycleCount |= regs.setRdAndStatus(rd, result, status);
        }
    }
    else
    {
        // It's 32-bit mode with separate 32-bit PC and PSR.
        if ((opCode >= 8) && (opCode < 12))
        {
            // It's a TEQ, TST, CMP or CMN instruction.
            if (rd == GeneralRegister::R0)
            {
                // Update the status flags in the PSR based on the result.
                regs.setStatusFlags(status);
            }
            else
            {
                // Rd MUST be R0 in 32-bit mode.
                cycleCount |= regs.raiseUndefinedInstruction();
            }
        }
        else
        {
            // For 32-bit mode, only set status flags, R15 updates PC.
            regs.setStatusFlags(status);
            cycleCount |= regs.setRn(rd, result);
        }
    }

    return cycleCount;
}

//! @brief Executes a partially decoded core data processing instruction.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The bit field of the instruction to execute.
//! @param[in] op2 The evaluated value of the second operand.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename TRegisterFile>
uint32_t execDataProcOp(TRegisterFile &regs, uint32_t instruction,
                        uint32_t op2) noexcept
{
    uint32_t cycleCount = 1;
    uint32_t op1 = regs.getRn(Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction));
    uint32_t result;
    uint8_t opCode = Ag::Bin::extractBits<uint8_t, 21, 4>(instruction);
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
        result = op1 + op2 + Ag::Bin::extractBit<PsrShift::Carry>(regs.getPSR());
        break;

    case 6: // SBC
        result = op1 - (op2 + Ag::Bin::extractBit<PsrShift::Carry>(regs.getPSR()));
        break;

    case 7: // RSC
        result = op2 - (op1 + Ag::Bin::extractBit<PsrShift::Carry>(regs.getPSR()));
        break;

    case 8: // TST
    case 9: // TEQ
    case 10: // CMP
    case 11: // CMN
        // No implicit 'S' suffix makes comparison operations invalid as
        // a data processing op.
        cycleCount |= regs.raiseUndefinedInstruction();
        result = 0;
        hasResult = false;
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
        GeneralRegister rd = Ag::Bin::extractEnum<GeneralRegister, 12, 4>(instruction);

        cycleCount |= regs.setRn(rd, result);
    }

    return cycleCount;
}

//! @brief Decodes and executes a core multiply instruction.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The instruction bit field.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
//! @note It is assumed that the instruction mask fits all relevant bits.
template<typename TRegisterFile>
uint32_t execMultiply(TRegisterFile &regs, uint32_t instruction) noexcept
{
    // ARMv2: Theoretically if Rm == Rd, the result should be 0 for MUL and
    // meaningless for MLA.
    // Also, Rd = R15, no result will be written back unless the 'S' flag
    // is specified, in which case the status flags will be clobbered.

    // Note: The use of getR[snm]() doesn't match the instruction, but is
    // correct for the handling of R15.
    uint32_t cycleCount = 1;
    uint32_t rmValue = regs.getRs(Ag::Bin::extractEnum<GeneralRegister, 0, 4>(instruction));
    uint32_t rsValue = regs.getRn(Ag::Bin::extractEnum<GeneralRegister, 8, 4>(instruction));
    uint32_t result;

    // Inherit the C and V status flags.
    uint8_t status = Ag::Bin::extractBits<uint8_t, PsrShift::Status, 2>(regs.getPSR());

    if (instruction & 0x200000)
    {
        // Its multiply with accumulate.
        result = ALU_Mla(rmValue, rsValue,
                         regs.getRm(Ag::Bin::extractEnum<GeneralRegister, 12, 4>(instruction)),
                         status);
    }
    else
    {
        result = ALU_Mul(rmValue, rsValue, status);
    }

    GeneralRegister rd = Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction);

    // R15 is protected from update.
    if (rd != GeneralRegister::R15)
    {
        regs.setRn(rd, result);
    }

    if (instruction & 0x100000)
    {
        // Update status flags.
        regs.setStatusFlags(status);
    }

    // Simulate the timing of the Booth's multiplier.
    int32_t msb;
    if (Ag::Bin::bitScanReverse(rmValue, msb))
    {
        cycleCount += (msb / 2);
    }

    return cycleCount;
}

//! @brief Executes one of the long multiply instructions [UMULL, UMLAL, SMULL, SMLAL].
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The instruction bit field to interpret.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename TRegisterFile>
uint32_t execLongMultiply(TRegisterFile &regs, uint32_t instruction) noexcept
{
    // Given use or R15 is UNPREDICTABLE, we'll assume it gets the PC portion
    // in the unlikely event a long multiply is performed in 26-bit mode.
    uint32_t rmValue = regs.getRn(Ag::Bin::extractEnum<GeneralRegister, 0, 4>(instruction));
    uint32_t rsValue = regs.getRn(Ag::Bin::extractEnum<GeneralRegister, 8, 4>(instruction));
    GeneralRegister rdLo = Ag::Bin::extractEnum<GeneralRegister, 12, 4>(instruction);
    GeneralRegister rdHi = Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction);
    LongWord result;

    // Inherit the C and V status flags.
    uint8_t status = Ag::Bin::extractBits<uint8_t, PsrShift::Status, 2>(regs.getPSR());

    switch (Ag::Bin::extractBits<uint8_t, 21, 2>(instruction))
    {
    case 0x00: // UMULL
        status = ALU_Umull(result, rsValue, rmValue, status);
        break;

    case 0x01: // UMLAL
        // Load the accumulator value in 32-bit components.
        result.Components.LoWord = regs.getRn(rdLo);
        result.Components.HiWord = regs.getRn(rdHi);

        status = ALU_Umlal(result, rsValue, rmValue, status);
        break;

    case 0x02: // SMULL
    default:
        status = ALU_Smull(result, rsValue, rmValue, status);
        break;

    case 0x03: // SMLAL
        // Load the accumulator value in 32-bit components.
        result.Components.LoWord = regs.getRn(rdLo);
        result.Components.HiWord = regs.getRn(rdHi);

        status = ALU_Smlal(result, rsValue, rmValue, status);
        break;
    }

    // Write the result, choosing to update the PC-only portion of R15 in this
    // implementation.
    regs.setRn(rdLo, result.Components.LoWord);
    regs.setRn(rdHi, result.Components.HiWord);

    if (instruction & 0x100000)
    {
        // Update status flags.
        regs.updateStatusFlags(status);
    }

    // On StrongARM: 1..3 result delay cycles, 2 issue cycles.
    // Let's take an average...
    return 3;
}

//! @brief Executes a branch or branch with link instruction.
//! @tparam TRegisterFile The data type of the register file, preferably
//! following the pattern of GenericCoreRegisterFile.
//! @param[in] regs The register file the instruction uses to get and set the
//! state of the processor.
//! @param[in] instruction The instruction bit field to interpret.
//! @return The instruction execution time and other results defined by the
//! ExecResult structure.
template<typename TRegisterFile>
uint32_t execBranch(TRegisterFile &regs, uint32_t instruction) noexcept
{
    uint32_t currentPC = regs.getPC();

    if (instruction & 0x1000000)
    {
        // Save the address of the instruction after the current one, noting
        // that the PC is 8 bytes ahead of the current instruction.

        if constexpr (TRegisterFile::HasCombinedPcPsr)
        {
            // NOTE: Ensure the PSR flags are also stored so that they can be
            // optionally restored on return.
            regs.setRn(GeneralRegister::R14, (currentPC - 4) | regs.getPSR());
        }
        else
        {
            // The PSR flags should be saved to SPSR.
            regs.setRn(GeneralRegister::R14, currentPC - 4);
            regs.savePSR();
        }
    }

    // Ensure the offset is sign extended to 32 bits.
    int32_t offset = static_cast<int32_t>(instruction << 8) >> 6;

    // Apply the offset to the current PC.
    regs.setPC(currentPC + static_cast<uint32_t>(offset));

    // Always flush pipeline because we are always updating the PC.
    return 3 | ExecResult::FlushPipeline;
}

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

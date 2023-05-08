//! @file ArmEmu/ARMv2InstructionDecoder.inl
//! @brief The declaration of data types which decode and execute ARM
//! instructions for ARMv2 variants.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ARM_V2_INSTUCTION_DECOCDER_INL__
#define __ARM_EMU_ARM_V2_INSTUCTION_DECOCDER_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"

#include "ArmEmu.hpp"
#include "ArmCore.hpp"

#include "AluInstructions.inl"
#include "DataTransferInstructions.inl"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief An instruction decoder implementation which executes instructions
//! for basic ARMv2 processor variants.
template<typename THardware, typename TRegisterFile>
class ARMv2InstructionDecoder
{
public:
    // Public Types
    using HardwareType = THardware;
    using RegisterFileType = TRegisterFile;

private:
    // Internal Fields
    HardwareType &_hardware;
    RegisterFileType &_registers;

public:
    // Construction/Destruction
    ARMv2InstructionDecoder(HardwareType &hw, RegisterFileType &regs) :
        _hardware(hw),
        _registers(regs)
    {
    }

    ARMv2InstructionDecoder() = delete;
    ARMv2InstructionDecoder(const ARMv2InstructionDecoder &) = delete;
    ARMv2InstructionDecoder(ARMv2InstructionDecoder &&) = delete;
    ARMv2InstructionDecoder &operator=(const ARMv2InstructionDecoder &) = delete;
    ARMv2InstructionDecoder &operator=(ARMv2InstructionDecoder &&) = delete;
    ~ARMv2InstructionDecoder() = default;

    // Operations
    uint32_t decodeAndExecute(uint32_t instruction)
    {
        uint32_t result = 1;
        uint32_t op1, op2;
        uint8_t carryOut, opCode;

        // Switch on major op-code.
        switch (Ag::Bin::extractBits<uint8_t, 25, 3>(instruction))
        {
        case 0x00:
            // Core ALU operations using register/shifted register operand 2.
            // Core ALU multiply/long multiply.
            // ARMv2a+ Atomic Swap.
            // ARMv3+ Move from Status Register.
            // ARMv3+ Move register to Status Register.
            // ARMv4+ Branch/Exchange
            // ARMv4+ Load/Store half-word/signed byte.
            // ARMv5+ Breakpoint.
            //opCode = static_cast<uint8_t>(instruction >> 4) & 0x0F;

            if ((static_cast<uint8_t>(instruction) & 0x90) == 0x90)
            {
                // Bit 7 is set so it's not a data processing instruction.
                switch (static_cast<uint8_t>(instruction >> 5) & 0x03)
                {
                case 0: // It's a multiply or byte swap?
                    opCode = static_cast<uint8_t>(instruction >> 23) & 0x03;

                    switch (opCode)
                    {
                    case 0:
                        // Its a 32-bit MUL/MLA
                        result = execMultiply(_registers, instruction);
                        break;

                    case 1: // Its a long multiply (ARMv4).
                    case 2: // It could be an atomic swap (ARMv2a).
                    case 3:
                        result = _registers.raiseUndefinedInstruction();
                        break;
                    }
                    break;

                case 1: // LDR/STR unsigned half-word (ARMv4).
                case 2: // LDR/STR signed byte (ARMv4).
                case 3: // LDR/STR signed half-word (ARMv4).
                default:
                    result = _registers.raiseUndefinedInstruction();
                    break;
                }
            } // instruction & 0x90 = Not a Data Processing instruction
            else
            {
                // Its primarily a data processing instruction using a shifted
                // register as the second operand.
                opCode = static_cast<uint8_t>(instruction >> 21) & 0x0F;

                if (instruction & 0x100000)
                {
                    // Its a data processing instruction which affects the status flags.
                    op2 = calculateShiftedAluOperand(_registers, instruction, carryOut);

                    result = execDataProcOpStatus(_registers, instruction, op2, carryOut);
                }
                else if ((opCode & 0x0C) == 8) // Bit 3 set, bit 2 clear.
                {
                    // It's a comparison operation without the 'S' flag set.
                    switch (opCode & 0x03)
                    {
                    case 1:
                        if ((static_cast<uint8_t>(instruction >> 4) & 0x0F) == 0x07)
                        {
                            _hardware.setDebugIrq(true);
                        }
                        else
                        {
                            result = _registers.raiseUndefinedInstruction();
                        }
                        break;

                    default:
                        // Naughtiness? Or BX, Move to/from status register, 
                        result = _registers.raiseUndefinedInstruction();
                        break;
                    }
                }
                else
                {
                    // Its a data processing instruction which doesn't affect flags.
                    op2 = calculateShiftedAluOperand(_registers, instruction, carryOut);

                    result = execDataProcOp(_registers, instruction, op2);
                }
            }
            break;

        case 0x01:
            // Core ALU operations using immediate constant operand 2.
            // ARMv3+ Move immediate to Status Register.
            if (instruction & 0x100000)
            {
                op2 = calculateConstantAluOperand(instruction);

                result = execDataProcOpStatus(_registers, instruction, op2,
                                              Ag::Bin::extractBit<PsrShift::Carry>(_registers.getPSR()));
            }
            else if (Ag::Bin::extractBits<uint8_t, 23, 2>(instruction) == 0x02)
            {
                // Its a comparison op without the 'S' flag set. Naughtiness?
                result = _registers.raiseUndefinedInstruction();
            }
            else
            {
                op2 = calculateConstantAluOperand(instruction);

                result = execDataProcOp(_registers, instruction, op2);
            }
            break;

        case 0x02:
            // Load/Store with immediate offset.
            op1 = _registers.getRn(Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction));
            op2 = instruction & 0xFFF;

            result =
                (instruction & 0x100000) ? execLoad(_hardware, _registers, instruction, op1, op2) :
                execStore(_hardware, _registers, instruction, op1, op2);
            break;

        case 0x03:
            // Load/Store with register offset.
            op1 = _registers.getRn(Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction));
            op2 = calculateDataTransferOffset(_registers, instruction);

            result =
                (instruction & 0x100000) ? execLoad(_hardware, _registers, instruction, op1, op2) :
                execStore(_hardware, _registers, instruction, op1, op2);
            break;

        case 0x04:
            // Load/Store multiple registers.
            op1 = _registers.getRn(Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction));

            result =
                (instruction & 0x100000) ? execLoadMultiple(_hardware, _registers, instruction, op1) :
                execStoreMultiple(_hardware, _registers, instruction, op1);
            break;

        case 0x05:
            // Branch.
            // Branch/exchange.
            result = execBranch(_registers, instruction);
            break;

        case 0x06:
            // Co-processor load/store.
            result = _registers.raiseUndefinedInstruction();
            break;

        case 0x07:
            // Software interrupt.
            // Co-processor data processing.
            // Co-processor register transfer.
            if (Ag::Bin::extractBit<24>(instruction))
            {
                // It's a software interrupt.
                result = _registers.raiseSoftwareInterrupt();
            }
            else
            {
                result = _registers.raiseUndefinedInstruction();
            }
            break;

        default:
            // The instruction is unknown, raise a hardware exception.
            result = _registers.raiseUndefinedInstruction();
            break;
        }

        return result;
    }
};

//! @brief Executes the MRC instruction to copy a register from the System
//! Control Co-processor (CP15) to a core register.
//! @tparam TRegisterFile The data type of the register file encapsulating the
//! state of the processor.
//! @param[in] regs The register file holding the current state of the processor.
//! @param[in] instruction The instruction word to interpret.
//! @return An execution result based on constants defined in the ExecResult
//! structure.
template<typename TRegisterFile>
uint32_t execMrcARMv2aCP15(TRegisterFile &regs, uint32_t instruction)
{
    // MRC CP15, 0, Rd, CRn, CR0, 0 = > MOV Rd, CRn
    uint32_t result = 1;

    if (regs.isInPrivilegedMode())
    {
        GeneralRegister rd = Ag::Bin::extractEnum<GeneralRegister, 12, 4>(instruction);
        uint32_t value = regs.getCP15Register(Ag::Bin::extractEnum<CoProcRegister, 16, 4>(instruction));

        if (rd == GeneralRegister::R15)
        {
            regs.setStatusFlags(Ag::Bin::extractBits<uint8_t, PsrShift::Status, 4>(value));
        }
        else
        {
            regs.setRn(rd, value);
        }
    }
    else
    {
        // CP15 access is only allowed in a privileged processor mode.
        result = regs.raiseUndefinedInstruction();
    }

    return result;
}

//! @brief Executes the MCR instruction in an emulated ARM3 processor core to
//! copy a core register to co-processor register in the System Control
//! Co-processor (CP15).
//! @tparam TRegisterFile The data type of the register file encapsulating the
//! state of the processor.
//! @param[in] regs The register file holding the current state of the processor.
//! @param[in] instruction The instruction word to interpret.
//! @return An execution result based on constants defined in the ExecResult
//! structure.
template<typename TRegisterFile>
uint32_t execMcrARMv2aCP15(TRegisterFile &regs, uint32_t instruction)
{
    // MCR CP15,0,Rd,CRn,CR0,0 => MOV CRn,Rd
    uint32_t result = 1;

    if (regs.isInPrivilegedMode())
    {
        uint32_t value = regs.getRm(Ag::Bin::extractEnum<GeneralRegister, 12, 4>(instruction));

        regs.setCP15Register(Ag::Bin::extractEnum<CoProcRegister, 16, 4>(instruction), value);
    }
    else
    {
        // CP15 access is only allowed in a privileged processor mode.
        result = regs.raiseUndefinedInstruction();
    }

    return result;
}

//! @brief An instruction decoder implementation which executes instructions
//! for ARMv2a processor variants.
template<typename THardware, typename TRegisterFile>
class ARMv2aInstructionDecoder
{
public:
    // Public Types
    using HardwareType = THardware;
    using RegisterFileType = TRegisterFile;

private:
    // Internal Fields
    HardwareType &_hardware;
    RegisterFileType &_registers;

public:
    // Construction/Destruction
    ARMv2aInstructionDecoder(HardwareType &hw, RegisterFileType &regs) :
        _hardware(hw),
        _registers(regs)
    {
    }

    ARMv2aInstructionDecoder() = delete;
    ARMv2aInstructionDecoder(const ARMv2aInstructionDecoder &) = delete;
    ARMv2aInstructionDecoder(ARMv2aInstructionDecoder &&) = delete;
    ARMv2aInstructionDecoder &operator=(const ARMv2aInstructionDecoder &) = delete;
    ARMv2aInstructionDecoder &operator=(ARMv2aInstructionDecoder &&) = delete;
    ~ARMv2aInstructionDecoder() = default;

    // Operations
    uint32_t decodeAndExecute(uint32_t instruction)
    {
        uint32_t result = 1;
        uint32_t op1, op2;
        uint8_t carryOut, opCode;

        // Switch on major op-code.
        switch (Ag::Bin::extractBits<uint8_t, 25, 3>(instruction))
        {
        case 0x00:
            // Core ALU operations using register/shifted register operand 2.
            // Core ALU multiply/long multiply.
            // ARMv2a+ Atomic Swap.
            // ARMv3+ Move from Status Register.
            // ARMv3+ Move register to Status Register.
            // ARMv4+ Branch/Exchange
            // ARMv4+ Load/Store half-word/signed byte.
            // ARMv5+ Breakpoint.
            //opCode = static_cast<uint8_t>(instruction >> 4) & 0x0F;

            if ((static_cast<uint8_t>(instruction) & 0x90) == 0x90)
            {
                // Bit 7 is set so it's not a data processing instruction.
                switch (Ag::Bin::extractBits<uint8_t, 5, 2>(instruction))
                {
                case 0: // It's a multiply or byte swap?
                    switch (Ag::Bin::extractBits<uint8_t, 23, 2>(instruction))
                    {
                    case 0:
                        // Its a 32-bit MUL/MLA
                        result = execMultiply(_registers, instruction);
                        break;

                    case 2:
                        // Its an atomic swap (ARMv2a).
                        if ((instruction & 0x0FB00FF0) == 0x01000090)
                        {
                            result = execSwap(_hardware, _registers, instruction);
                        }
                        break;

                    case 1: // Its a long multiply (ARMv4).
                    case 3:
                    default:
                        result = _registers.raiseUndefinedInstruction();
                        break;
                    }
                    break;

                case 1: // LDR/STR unsigned half-word (ARMv4).
                case 2: // LDR/STR signed byte (ARMv4).
                case 3: // LDR/STR signed half-word (ARMv4).
                default:
                    result = _registers.raiseUndefinedInstruction();
                    break;
                }
            } // instruction & 0x90 = Not a Data Processing instruction
            else
            {
                // Its primarily a data processing instruction using a shifted
                // register as the second operand.
                opCode = static_cast<uint8_t>(instruction >> 21) & 0x0F;

                if (instruction & 0x100000)
                {
                    // Its a data processing instruction which affects the status flags.
                    op2 = calculateShiftedAluOperand(_registers, instruction, carryOut);

                    result = execDataProcOpStatus(_registers, instruction, op2, carryOut);
                }
                else if ((opCode & 0x0C) == 8) // Bit 3 set, bit 2 clear.
                {
                    // It's a comparison operation without the 'S' flag set.
                    switch (opCode & 0x03)
                    {
                    case 1:
                        if ((static_cast<uint8_t>(instruction >> 4) & 0x0F) == 0x07)
                        {
                            _hardware.setDebugIrq(true);
                        }
                        else
                        {
                            result = _registers.raiseUndefinedInstruction();
                        }
                        break;

                    default:
                        // Naughtiness? Or BX, Move to/from status register, 
                        result = _registers.raiseUndefinedInstruction();
                        break;
                    }
                }
                else
                {
                    // Its a data processing instruction which doesn't affect flags.
                    op2 = calculateShiftedAluOperand(_registers, instruction, carryOut);

                    result = execDataProcOp(_registers, instruction, op2);
                }
            }
            break;

        case 0x01:
            // Core ALU operations using immediate constant operand 2.
            // ARMv3+ Move immediate to Status Register.
            if (instruction & 0x100000)
            {
                op2 = calculateConstantAluOperand(instruction);

                result = execDataProcOpStatus(_registers, instruction, op2,
                                              Ag::Bin::extractBit<PsrShift::Carry>(_registers.getPSR()));
            }
            else if (Ag::Bin::extractBits<uint8_t, 23, 2>(instruction) == 0x02)
            {
                // Its a comparison op without the 'S' flag set. Naughtiness?
                result = _registers.raiseUndefinedInstruction();
            }
            else
            {
                op2 = calculateConstantAluOperand(instruction);

                result = execDataProcOp(_registers, instruction, op2);
            }
            break;

        case 0x02:
            // Load/Store with immediate offset.
            op1 = _registers.getRn(Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction));
            op2 = instruction & 0xFFF;

            result =
                (instruction & 0x100000) ? execLoad(_hardware, _registers, instruction, op1, op2) :
                execStore(_hardware, _registers, instruction, op1, op2);
            break;

        case 0x03:
            // Load/Store with register offset.
            op1 = _registers.getRn(Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction));
            op2 = calculateDataTransferOffset(_registers, instruction);

            result =
                (instruction & 0x100000) ? execLoad(_hardware, _registers, instruction, op1, op2) :
                execStore(_hardware, _registers, instruction, op1, op2);
            break;

        case 0x04:
            // Load/Store multiple registers.
            op1 = _registers.getRn(Ag::Bin::extractEnum<GeneralRegister, 16, 4>(instruction));

            result =
                (instruction & 0x100000) ? execLoadMultiple(_hardware, _registers, instruction, op1) :
                execStoreMultiple(_hardware, _registers, instruction, op1);
            break;

        case 0x05:
            // Branch.
            // Branch/exchange.
            result = execBranch(_registers, instruction);
            break;

        case 0x06:
            // Co-processor load/store.
            result = _registers.raiseUndefinedInstruction();
            break;

        case 0x07:
            // Software interrupt.
            // Co-processor data processing.
            // Co-processor register transfer.
            if (Ag::Bin::extractBit<24>(instruction))
            {
                // It's a software interrupt.
                result = _registers.raiseSoftwareInterrupt();
            }
            else if ((instruction & 0x0EE00FFF) == 0x0E000F10)
            {
                // It's MRC or MCR to CP15 (the System Control Co-processor).
                if (Ag::Bin::extractBit<20>(instruction))
                {
                    // Its MRC CP15,0,Rd,CRn,CR0,0 => MOV Rd,CRn
                    result = execMrcARMv2aCP15(_registers, instruction);
                }
                else
                {
                    // Its MCR CP15,0,Rd,CRn,CR0,0 => MOV CRn,Rd
                    result = execMcrARMv2aCP15(_registers, instruction);
                }
            }
            else
            {
                result = _registers.raiseUndefinedInstruction();
            }
            break;

        default:
            // The instruction is unknown, raise a hardware exception.
            result = _registers.raiseUndefinedInstruction();
            break;
        }

        return result;
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

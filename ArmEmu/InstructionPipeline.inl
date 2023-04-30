//! @file ARmEmu/InstructionPipeline.inl
//! @brief The declaration of template classes representing and instruction
//! execution pipelines in a single mode of operation (i.e. 26-bit, 32-bit
//! or thumb).
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_INSTRUCTION_PIPELINE_INL__
#define __ARM_EMU_INSTRUCTION_PIPELINE_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"

#include "ArmEmu.hpp"
#include "ArmCore.hpp"

#include "AluInstructions.inl"
#include "DataTransferInstructions.inl"

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief A template class representing the ARMv2 instruction execution
//! pipeline.
template<typename TPipelineTraits>
class InstructionPipeline
{
public:
    // Public Types
    using Hardware = typename TPipelineTraits::HardwareType;
    using RegisterFile = typename TPipelineTraits::RegisterFileType;
    using InstructionType = typename TPipelineTraits::InstructionWordType;
    static constexpr uint8_t PipelineIncrement = static_cast<uint8_t>(1) << TPipelineTraits::InstructionSizePow2;
    static constexpr uint8_t PipelineShift = TPipelineTraits::InstructionSizePow2 + 1;
    static constexpr uint8_t PipelineAdjust = static_cast<uint8_t>(1) << PipelineShift;

private:
    // Internal Fields
    Hardware &_hardware;
    RegisterFile &_registers;
    uint8_t _flushPending;

private:
    // Internal Functions
    uint32_t decodeAndExecute(uint32_t instruction);

public:
    // Construction/Destruction
    InstructionPipeline(Hardware &hw, RegisterFile &regs) :
        _hardware(hw),
        _registers(regs),
        _flushPending(1)
    {
    }

    // Accessors

    // Operations
    //! @brief Flushes the pre-fetch instruction queue after a direct write to
    //! the PC.
    void flushPipeline()
    {
        _flushPending = 1;
    }

    uint32_t executeNext()
    {
        // Run the pipeline as normal.
        uint32_t execResult = 1;

        // Adjust the PC if the previous action performed a pipeline flush.
        _registers.incrementPC(_flushPending << PipelineShift);

        // Load and decode the next instruction.
        uint32_t pc = _registers.getPC();
        uint32_t instruction;

        // Fetch the instruction.
        if (_hardware.read(pc - PipelineAdjust, instruction))
        {
            // Decode the instruction condition code.
            if (canExecuteInstruction(instruction,
                                      static_cast<uint8_t>(_registers.getPSR() >> 28)))
            {
                // Further decode and execute the instruction.
                execResult = decodeAndExecute(instruction);
            }

            if ((execResult & ExecResult::FlushPipeline) == 0)
            {
                // Advance the PC.
                _registers.incrementPC(PipelineIncrement);
            }
        }
        else
        {
            // The instruction could not be loaded.
            execResult = _registers.raisePreFetchAbort();
        }

        // Set _flushPending to either 0 or 1 without branching.
        _flushPending = static_cast<uint8_t>(execResult >> ExecResult::FlushShift) & 1;

        return execResult;
    }
};

template<typename TPipelineTraits>
uint32_t InstructionPipeline<TPipelineTraits>::decodeAndExecute(uint32_t instruction)
{
    uint32_t result = 1;
    uint32_t op1, op2;
    uint8_t carryOut, opCode;

    // Switch on major op-code.
    switch (extractBits<uint8_t, 25, 3>(instruction))
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

                case 1:
                    // Its a long multiply (ARMv4).
                    result = _registers.raiseUndefinedInstruction();
                    //result = execLongMultiply(instruction);
                    break;

                case 2:
                    // Its an atomic swap (ARMv2a).
                    if ((instruction & 0x0FB00FF0) == 0x01000090)
                    {
                        result = execSwap(_hardware, _registers, instruction);
                    }
                    break;

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
                                          extractBit<PsrShift::Carry>(_registers.getPSR()));
        }
        else if (extractBits<uint8_t, 23, 2>(instruction) == 0x02)
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
        op1 = _registers.getRn(extractEnum<GeneralRegister, 16, 4>(instruction));
        op2 = instruction & 0xFFF;

        result =
            (instruction & 0x100000) ? execLoad(_hardware, _registers, instruction, op1, op2) :
                                       execStore(_hardware, _registers, instruction, op1, op2);
        break;

    case 0x03:
        // Load/Store with register offset.
        op1 = _registers.getRn(extractEnum<GeneralRegister, 16, 4>(instruction));
        op2 = calculateDataTransferOffset(_registers, instruction);

        result =
            (instruction & 0x100000) ? execLoad(_hardware, _registers, instruction, op1, op2) :
                                       execStore(_hardware, _registers, instruction, op1, op2);
        break;

    case 0x04:
        // Load/Store multiple registers.
        op1 = _registers.getRn(extractEnum<GeneralRegister, 16, 4>(instruction));

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
        if (extractBit<24>(instruction))
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

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

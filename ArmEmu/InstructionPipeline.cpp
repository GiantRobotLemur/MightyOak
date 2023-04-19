//! @file ArmEmu/InstructionPipeline.cpp
//! @brief The definition of an object representing the instruction decoding
//! and execution portion of an emulated ARM processor.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "AsmTools.hpp"

#include "InstructionPipeline.hpp"

namespace Ag {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
// Useful for single-step instruction debugging.
//void outputDebug(uint32_t addr, uint32_t instruction)
//{
//    Asm::InstructionInfo info;
//    info.disassemble(instruction, addr);
//
//    Asm::FormatterOptions opts;
//    opts.setInstructionAddress(addr);
//    opts.setFlags(Asm::FormatterOptions::UseCoreRegAliases);
//
//    String instructionText = info.toString(&opts);
//
//     printf("0x%.6X: %s\n", addr, instructionText.getUtf8Bytes());
//}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// InstructionPipeline Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Initialises the local matrix of conditional execution decoding flags.
void InstructionPipeline::initialiseConditionMatrix() noexcept
{
    // Define shifted local copies of the status flags.
    static const uint8_t V = 0x01; // oVerflow
    static const uint8_t C = 0x02; // Carry
    static const uint8_t Z = 0x04; // Zero
    static const uint8_t N = 0x08; // Negative
    static const uint8_t NV = N | V;
    static const uint8_t ZNV = Z | N | V;

    // Enumerate all combinations of status flags.
    for (uint8_t statusFlagState = 0; statusFlagState < 16; ++statusFlagState)
    {
        // Determine whether execution can occur for each condition code.
        uint16_t state = 0;

        // Condition EQ (0).
        if (statusFlagState & Z)
        {
            state |= 0x0001;
        }

        // Condition NE (1).
        if ((statusFlagState & Z) == 0)
        {
            state |= 0x0002;
        }

        // Condition CS/HS (2).
        if (statusFlagState & C)
        {
            state |= 0x0004;
        }

        // Condition CC/LO (3).
        if ((statusFlagState & C) == 0)
        {
            state |= 0x0008;
        }

        // Condition MI (4).
        if (statusFlagState & N)
        {
            state |= 0x0010;
        }

        // Condition PL (5).
        if ((statusFlagState & N) == 0)
        {
            state |= 0x0020;
        }

        // Condition VS (6).
        if (statusFlagState & V)
        {
            state |= 0x0040;
        }

        // Condition VC (7).
        if ((statusFlagState & V) == 0)
        {
            state |= 0x0080;
        }

        // Condition HI (8).
        if ((statusFlagState & C) &&
            ((statusFlagState & Z) == 0))
        {
            state |= 0x0100;
        }

        // Condition LS (9).
        if (((statusFlagState & C) == 0) ||
            (statusFlagState & Z))
        {
            state |= 0x0200;
        }

        // Condition GE (10).
        if (((statusFlagState & NV) == NV) ||
            ((statusFlagState & NV) == 0))
        {
            state |= 0x0400;
        }

        // Condition LT (11).
        if (((statusFlagState & NV) == V) ||
            ((statusFlagState & NV) == N))
        {
            state |= 0x0800;
        }

        // Condition GT (12).
        if (((statusFlagState & ZNV) == N) ||
            ((statusFlagState & ZNV) == V) ||
            ((statusFlagState & NV) == 0))
        {
            state |= 0x1000;
        }

        // Condition LE (13).
        if ((statusFlagState & Z) ||
            ((statusFlagState & NV) == N) ||
            ((statusFlagState & NV) == V))
        {
            state |= 0x2000;
        }

        // Condition AL (14).
        state |= 0x4000;

        // Condition NV (15).
        // Never execute, always clear.

        // Write the conditions back.
        _conditionMatrix[statusFlagState] = state;
    }
}

//! @brief Determines if an instruction should be executed given the current
//! state of the PSR status flags.
//! @param[in] instruction The instruction bit-field to examine.
//! @retval true The instruction should be executed.
//! @retval false The instruction should not be executed.
bool InstructionPipeline::canExecute(uint32_t instruction) const noexcept
{
    // The word is addressed by the current status flags.
    // The bit is addressed by the condition encoded in the instruction.
    uint16_t bits = _conditionMatrix[getStatusFlags()];

    return (bits & (1 << static_cast<uint8_t>(instruction >> 28))) != 0;
}

//! @brief Resets and re-fills the instruction pipeline based on the current
//! PC value.
void InstructionPipeline::flushPipeline()
{
    uint32_t pc = getPC();
    _pipelineState = fetchInstructions(pc, _pipeline);
    _pipelineSources[0] = pc;
    _pipelineSources[1] = pc + 4;
    setPC(pc + 8);
    _currentSlot = 0;
}

//! @brief Decodes and executes an ARM machine code instruction.
//! @param[in] instruction The instruction bit field to decode into an
//! executable operation.
//! @returns The operation result defined by the InstructionResult enumeration.
uint32_t InstructionPipeline::decodeAndExec(uint32_t instruction)
{
    uint32_t result = 1;
    uint32_t op1, op2;
    uint8_t carryOut, opCode;

    // Switch on major op-code.
    switch (static_cast<uint8_t>(instruction >> 25) & 0x07)
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
                    result = execMultiply(instruction);
                    break;

                case 1:
                    // Its a long multiply (ARMv4).
                    //raiseUndefinedInstruction();
                    result = execLongMultiply(instruction);
                    result |= IR_FlushPipeline;
                    break;

                case 2:
                    // Its an atomic swap (ARMv2a).
                    if ((instruction & 0x0FB00FF0) == 0x01000090)
                    {
                        result = execSwap(instruction);
                    }
                    break;

                case 3:
                    raiseUndefinedInstruction();
                    result |= IR_FlushPipeline;
                }
                break;

            case 1:
                // LDR/STR unsigned half-word (ARMv4).
            case 2:
                // LDR/STR signed byte (ARMv4).
            case 3:
                // LDR/STR signed half-word (ARMv4).
            default:
                raiseUndefinedInstruction();
                result |= IR_FlushPipeline;
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
                op2 = calculateShiftedAluOperand(instruction, carryOut);

                result = execDataProcOpStatus(instruction, op2, carryOut);
            }
            else if ((opCode & 0x0C) == 8) // Bit 3 set, bit 2 clear.
            {
                // It's a comparison operation without the 'S' flag set.
                switch (opCode & 0x03)
                {
                case 1:
                    if ((static_cast<uint8_t>(instruction >> 4) & 0x0F) == 0x07)
                    {
                        raiseDebugIrq();
                    }
                    else
                    {
                        raiseUndefinedInstruction();
                        result |= IR_FlushPipeline;
                    }
                    break;

                default:
                    // Naughtiness? Or BX, Move to/from status register, 
                    raiseUndefinedInstruction();
                    result |= IR_FlushPipeline;
                    break;
                }
            }
            else
            {
                // Its a data processing instruction which doesn't affect flags.
                op2 = calculateShiftedAluOperand(instruction, carryOut);

                result = execDataProcOp(instruction, op2);
            }
        }
        break;

    case 0x01:
        // Core ALU operations using immediate constant operand 2.
        // ARMv3+ Move immediate to Status Register.
        if (instruction & 0x100000)
        {
            op2 = calculateConstantAluOperand(instruction);

            result = execDataProcOpStatus(instruction, op2, getCarry());
        }
        else if ((static_cast<uint8_t>(instruction >> 23) & 0x03) == 0x02)
        {
            // Its a comparison op without the 'S' flag set. Naughtiness?
            raiseUndefinedInstruction();
            result |= IR_FlushPipeline;
        }
        else
        {
            op2 = calculateConstantAluOperand(instruction);

            result = execDataProcOp(instruction, op2);
        }
        break;

    case 0x02:
        // Load/Store with immediate offset.
        op1 = getRn(decodeReg(instruction, 16));
        op2 = instruction & 0xFFF;

        result = (instruction & 0x100000) ? execLoad(instruction, op1, op2) :
                                            execStore(instruction, op1, op2);
        break;

    case 0x03:
        // Load/Store with register offset.
        op1 = getRn(decodeReg(instruction, 16));
        op2 = calculateDataTransferOffset(instruction);

        result = (instruction & 0x100000) ? execLoad(instruction, op1, op2) :
                                            execStore(instruction, op1, op2);
        break;

    case 0x04:
        // Load/Store multiple registers.
        op1 = getRn(decodeReg(instruction, 16));

        result = (instruction & 0x100000) ? execLoadMultiple(instruction, op1) :
                                            execStoreMultiple(instruction, op1);
        break;

    case 0x05:
        // Branch.
        // Branch/exchange.
        result = execBranch(instruction);
        break;

    case 0x06:
        // Co-processor load/store.
        raiseUndefinedInstruction();
        result |= IR_FlushPipeline;
        break;

    case 0x07:
        // Software interrupt.
        // Co-processor data processing.
        // Co-processor register transfer.
        if (static_cast<uint8_t>(instruction >> 24) & 1)
        {
            // It's a software interrupt.
            raiseSoftwareInterrupt();
            result |= IR_FlushPipeline;
        }
        else
        {
            raiseUndefinedInstruction();
            result |= IR_FlushPipeline;
        }
        break;

    default:
        // The instruction is unknown, raise a hardware exception.
        raiseUndefinedInstruction();
        result |= IR_FlushPipeline;
        break;
    }

    return result;
}

//! @brief Constructs an object which represents the instruction decoding and
//! execution portions of an emulated ARM processor.
InstructionPipeline::InstructionPipeline() :
    _pipelineState(0),
    _currentSlot(0)
{
    std::fill_n(_pipeline, PipelineStageCount, 0);
    std::fill_n(_pipelineSources, PipelineStageCount, 0);
    std::fill_n(_conditionMatrix, 16, static_cast<uint16_t>(0));

    initialiseConditionMatrix();
}

//! @brief Executes instructions until a host or debug interrupt is raised or
//! after the first run if in single step mode.
//! @param[in] singleStep True to only run the pipeline once, false to run
//! until a host or debug interrupt is triggered.
//! @returns The count of simulated CPU cycles executed before exit.
uint64_t InstructionPipeline::runPipeline(bool singleStep)
{
    uint64_t cycleCount = 0;

    // Ensure the pipeline only runs once in single-step mode.
    bool runPipeline = (singleStep == false);

    // Clear any external interrupts before running.
    clearDebugIrq();
    clearHostIrq();

    do
    {
        // Read the state of unmasked IRQs which might upset things.
        uint8_t pendingIrqs = getPendingIrqs();

        if (pendingIrqs)
        {
            // Deal with interrupts, both internal and external.
            if (pendingIrqs & IS_HostMask)
            {
                // Exit the pipeline without processing anything.
                runPipeline = false;
            }
            else if (pendingIrqs & IS_FirqPending)
            {
                // A fast interrupt has been signalled.
                raiseFirq();
                flushPipeline();
            }
            else // if (pendingIrqs & IS_IrqPending)
            {
                // A normal interrupt has been signalled.
                raiseIrq();
                flushPipeline();
            }
        }
        else // if (pendingIrqs == 0)
        {
            // Run the pipeline as normal.
            uint8_t slotMask = 1 << _currentSlot;

            if (_pipelineState & slotMask)
            {
                // The stage is valid, execute it.
                uint32_t result;

                // Check the condition code against the status
                // flags before executing.
                if (canExecute(_pipeline[_currentSlot]))
                {
                    // Perform decode and execute in a single step.
                    //outputDebug(_pipelineSources[_currentSlot],
                    //            _pipeline[_currentSlot]);

                    result = decodeAndExec(_pipeline[_currentSlot]);
                }
                else
                {
                    // One cycle to decode the skipped instruction.
                    result = 1;
                }

                // Count the cycles the instruction cost.
                cycleCount += result & IR_CycleCountMask;

                if (result & IR_FlushPipeline)
                {
                    // The PC was updated, flush the pipeline.
                    flushPipeline();
                }
                else
                {
                    // Fetch the instruction 8 bytes on from the one just
                    // executed.
                    uint32_t nextPC = getPC();
                    _pipelineSources[_currentSlot] = nextPC;

                    // Overwrite the executed instruction with the next one
                    // fetched from memory.
                    if (readWord(nextPC, _pipeline[_currentSlot]))
                    {
                        // The next instruction was successfully fetched,
                        // mark the pipeline slot as valid.
                        _pipelineState |= slotMask;
                    }
                    else
                    {
                        // The fetch failed, mark the pipeline slot as invalid.
                        // It will raise a pre-fetch abort if executed.
                        _pipelineState &= ~slotMask;
                    }

                    // Move on to the next instruction.
                    setPC(nextPC + 4);
                    _currentSlot = (_currentSlot + 1) & PipelineStageMask;
                }
            }
            else // if ((_pipelineState & slotMask) == 0)
            {
                // The pipeline slot was marked as invalid because the
                // instruction could not be read from memory.
                raisePreFetchAbort();
                flushPipeline();
            }
        } // if (pendingIrqs == 0)
    }
    while (runPipeline);

    return cycleCount;
}

//! @brief Prepares the processor for its first run.
void InstructionPipeline::reset()
{
    raiseReset();
    flushPipeline();
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


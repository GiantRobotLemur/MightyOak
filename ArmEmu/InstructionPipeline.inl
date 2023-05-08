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
// Dependent Header Files
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
//! @brief A template class representing the ARMv2 instruction execution
//! pipeline.
template<typename TPipelineTraits>
class InstructionPipeline
{
public:
    // Public Types
    using Hardware = typename TPipelineTraits::HardwareType;
    using RegisterFile = typename TPipelineTraits::RegisterFileType;
    using Decoder = typename TPipelineTraits::DecoderType;
    using InstructionType = typename TPipelineTraits::InstructionWordType;
    static constexpr uint8_t PipelineIncrement = static_cast<uint8_t>(1) << TPipelineTraits::InstructionSizePow2;
    static constexpr uint8_t PipelineShift = TPipelineTraits::InstructionSizePow2 + 1;
    static constexpr uint8_t PipelineAdjust = static_cast<uint8_t>(1) << PipelineShift;

private:
    // Internal Fields
    Hardware &_hardware;
    RegisterFile &_registers;
    Decoder _decoder;
    uint8_t _flushPending;

public:
    // Construction/Destruction
    InstructionPipeline(Hardware &hw, RegisterFile &regs) :
        _hardware(hw),
        _registers(regs),
        _decoder(hw, regs),
        _flushPending(1)
    {
    }

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
                execResult = _decoder.decodeAndExecute(instruction);
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

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ARmEmu/InstructionPipeline.inl
//! @brief The declaration of template classes representing and instruction
//! execution pipelines in a single mode of operation (i.e. 26-bit, 32-bit
//! or thumb).
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
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
    static constexpr uint8_t FlushIncrement = ExecResult::FlushShift - TPipelineTraits::InstructionSizePow2;

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

    // Accessors
    //! @brief Determines if the current PC points to the next instruction
    //! rather than 8-bytes beyond due to pipelining.
    //! @retval true The current PC points to the next instruction to execute,
    //! as it might after a executing a branch or a direct write.
    //! @retval false The current PC points to the next instruction to fetch,
    //! 8 bytes beyond the next instruction to execute.
    bool isFlushPending() const { return _flushPending != 0; }

    // Operations
    //! @brief Flushes the pre-fetch instruction queue after a direct write to
    //! the PC.
    void flushPipeline()
    {
        _flushPending = 1;
    }

    //! @brief Resets the PC to point to the next instruction and prepares to
    //! run as if the PC was just updated.
    void unflushPipeline()
    {
        if (_flushPending == 0)
        {
            _registers.incrementPC(static_cast<uint32_t>(-static_cast<int32_t>(PipelineAdjust)));
            _flushPending = 1;
        }
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

            const uint32_t pcIncrement =
                ((execResult & ExecResult::FlushPipeline) ^
                 ExecResult::FlushPipeline) >> FlushIncrement;

            // Advance the PC or not depending on whether the pipeline was flushed.
            _registers.incrementPC(pcIncrement);
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

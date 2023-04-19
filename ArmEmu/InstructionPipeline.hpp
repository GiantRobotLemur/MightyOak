//! @file ArmEmu/InstructionPipeline.hpp
//! @brief The declaration of an object representing the instruction decoding
//! and execution portion of an emulated ARM processor.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_INSTRUCTION_PIPELINE_HPP__
#define __ARM_EMU_INSTRUCTION_PIPELINE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "DataTransfer.hpp"

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing the instruction decoding
//! and execution portion of an emulated ARM processor.
class InstructionPipeline : public DataTransfer
{
private:
    // Internal Types

    // Internal Fields
    static const uint8_t PipelineStageCount = 2;
    static const uint8_t PipelineStageMask = 0x01;
    static const uint8_t ConditionMatrixSize = 16;

    uint16_t _conditionMatrix[ConditionMatrixSize];
    uint32_t _pipeline[PipelineStageCount];
    uint32_t _pipelineSources[PipelineStageCount];
    uint8_t _pipelineState;
    uint8_t _currentSlot;

    // Internal Functions
    void initialiseConditionMatrix() noexcept;
    bool canExecute(uint32_t instruction) const noexcept;
    void flushPipeline();
    uint32_t decodeAndExec(uint32_t instruction);
public:
    // Construction/Destruction
    InstructionPipeline();
    virtual ~InstructionPipeline() = default;

    // Operations
    void reset();
    uint64_t runPipeline(bool singleStep);
};

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

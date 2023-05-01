//! @file ArmEmu/ArmCore.hpp
//! @brief The declaration of functionality pervasive across all emulated
//! ARM core configurations.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ARM_CORE_HPP__
#define __ARM_EMU_ARM_CORE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <memory>

#include "ArmEmu.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Identifies a general purpose register.
enum class GeneralRegister : uint8_t
{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    Max,
};

//! @brief Defines the bit field returned by functions which emulate the execution
//! of ARM instructions.
struct ExecResult
{
    //! @brief A mask to extract the count of cycles an instruction took to
    //! execute, up to 255 allowed, good enough for a RISC processor.
    static constexpr uint32_t CycleCountMask = 0x00FF;

    //! @brief Specifies that the instruction pipeline should be flushed due to
    //! a direct write to the Program Counter.
    static constexpr uint32_t FlushPipeline = 0x0100;

    //! @brief Specifies the number of digits to shift an ExecResult to
    //! extract the FlushPipeline bit.
    static constexpr uint8_t FlushShift = 8;

    //! @brief Specifies that the execution mode of the processor has been
    //! changed so a new execution pipeline might be selected.
    static constexpr uint32_t ModeChange = 0x0200;

    //! @brief A mask indicating a flush and mode change occurred and that the
    //! current instruction pipeline might be switched out for another.
    static constexpr uint32_t PipelineChange = 0x0300;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
void initialiseConditionMatrix(uint16_t(&conditionMatrix)[16]) noexcept;
bool canExecuteInstruction(uint32_t instruction, uint8_t statusFlags) noexcept;

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

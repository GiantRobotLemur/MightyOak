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

//! @brief Represents the identifiers of generic co-processor registers.
enum class CoProcRegister : uint8_t
{
    CR0,
    CR1,
    CR2,
    CR3,
    CR4,
    CR5,
    CR6,
    CR7,
    CR8,
    CR9,
    CR10,
    CR11,
    CR12,
    CR13,
    CR14,
    CR15
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
bool canAccessMemcPage(uint8_t ppl, bool isPriviledged, bool isOsMode, bool isWrite);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Replicates a value across all lines of the emulated data bus.
//! @tparam The data type of the value to be replicated.
//! @param[in] value The value to replicate.
//! @return The data base contents.
template<typename T> constexpr uint32_t replicate(T value) noexcept
{
    return static_cast<uint32_t>(value);
}

//! @brief A specialization which replicates a 16-bit value twice across a
//! 32-bit data bus.
//! @param[in] value The 16-bit value to replicate.
//! @return The 32-bit data bus contents.
template<> constexpr uint32_t replicate(uint16_t value) noexcept
{
    return static_cast<uint32_t>(value) |
        (static_cast<uint32_t>(value) << 16);
}

//! @brief A specialization which replicates an 8-bit value four times
//! across a 32-bit data bus.
//! @param[in] value The 8-bit value to replicate.
//! @return The 32-bit data bus contents.
template<> constexpr uint32_t replicate(uint8_t value) noexcept
{
    return static_cast<uint32_t>(value) |
           (static_cast<uint32_t>(value) << 8) |
           (static_cast<uint32_t>(value) << 16) |
           (static_cast<uint32_t>(value) << 24);
}

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

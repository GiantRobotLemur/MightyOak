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
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <memory>

#include "ArmEmu.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
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
//! @brief Extracts a bit from a bitfield.
//! @tparam TBit The 0-based index of the bit to extract.
//! @tparam TField The data type of the bitfield be queried.
//! @param[in] bitfield The bit field to extract the bit from.
//! @returns Either 0 or 1 depending on the bit state.
template<uint8_t TBit, typename TField>
constexpr uint8_t extractBit(TField bitfield) noexcept
{
    constexpr uint8_t FieldBits = sizeof(TField) * 8;
    static_assert(TBit < FieldBits, "TBit out of range in extractBit<TBit>().");

    if constexpr (TBit == (FieldBits - 1))
    {
        // No need to mask, save ourselves an AND instruction.
        return static_cast<uint8_t>(bitfield >> TBit);
    }
    else
    {
        return static_cast<uint8_t>(bitfield >> TBit) & 1;
    }
}

// Tests for extratBit<>().
// Verify shift and mask.
static_assert(extractBit<12, uint32_t>(0) == 0, "extractBit() failed.");
static_assert(extractBit<12, uint32_t>(0x1000) == 1, "extractBit() failed.");

// Verify just shift.
static_assert(extractBit<31, uint32_t>(0) == 0, "extractBit() failed.");
static_assert(extractBit<31, uint32_t>(0x80000000) == 1, "extractBit() failed.");

//! @brief Extracts a scalar value from a bitfield.
//! @tparam TOutput The data type of the scalar value being extracted.
//! @tparam TInput The data type of the input value to extract from.
//! @tparam TLsb The 0-based index of the least significant bit of the value
//! to extract.
//! @tparam TCount The count of consecutive bits to extract.
//! @param bitfield The bitfield to extract bits from.
//! @return The extracted bits as the required enumeration type.
template<typename TOutput, uint8_t TLsb, uint8_t TCount, typename TInput>
constexpr TOutput extractBits(TInput bitfield) noexcept
{
    constexpr uint8_t InputBits = sizeof(TInput) * 8;
    static_assert((TLsb + TCount) <= InputBits);

    if constexpr (InputBits == (TLsb + TCount))
    {
        // No need to mask, save ourselves an AND instruction.
        return static_cast<TOutput>(bitfield >> TLsb);
    }
    else
    {
        constexpr TOutput Mask = (static_cast<TOutput>(1) << TCount) - 1;

        return static_cast<TOutput>(bitfield >> TLsb) & Mask;
    }
}

// Tests for extarctBits<>();
// Verify shift and mask.
static_assert(extractBits<uint8_t, 12, 4, uint32_t>(0) == 0, "extractBits() failed.");
static_assert(extractBits<uint8_t, 12, 4, uint32_t>(0x5ABCD) == 10, "extractBits() failed.");

// Verify just shift.
static_assert(extractBits<uint8_t, 28, 4, uint32_t>(0x60000000) == 6, "extractBits() failed.");
static_assert(extractBits<uint8_t, 24, 8, uint32_t>(0xDEADBEEF) == 0xDE, "extractBits() failed.");

//! @brief Extracts an enumeration value from a bitfield.
//! @tparam TEnum The data type of the enumeration being extracted.
//! @tparam TInput The data type of the input value to extract from.
//! @tparam TLsb The 0-based index of the least significant bit of the value
//! to extract.
//! @tparam TCount The count of consecutive bits to extract.
//! @param bitfield The bitfield to extract bits from.
//! @return The extracted bits as the required enumeration type.
template<typename TEnum, uint8_t TLsb, uint8_t TCount, typename TInput>
constexpr TEnum extractEnum(TInput bitfield) noexcept
{
    using Raw = std::underlying_type_t<TEnum>;
    constexpr uint8_t RawBits = sizeof(Raw) * 8;
    constexpr uint8_t InputBits = sizeof(TInput) * 8;
    constexpr uint8_t LastBit = TLsb + TCount;

    static_assert(TCount <= RawBits, "Too many bits requested from extractEnum<>().");
    static_assert(LastBit <= InputBits,
                  "The range of bits to extract is beyond the end of the input field.");

    if constexpr (LastBit == InputBits)
    {
        // No need to mask, save ourselves an AND instruction.
        return static_cast<TEnum>(static_cast<Raw>(bitfield >> TLsb));
    }
    else
    {
        constexpr Raw Mask = (static_cast<Raw>(1) << TCount) - 1;

        return static_cast<TEnum>(static_cast<Raw>(bitfield >> TLsb) & Mask);
    }
}

// Verify shift and mask.
static_assert(extractEnum<GeneralRegister, 12, 4, uint32_t>(0) == GeneralRegister::R0,
              "extractEnum() failed.");
static_assert(extractEnum<GeneralRegister, 12, 4, uint32_t>(0x5ABCD) == GeneralRegister::R10,
              "extractBits() failed.");

// Verify just shift.
static_assert(extractEnum<GeneralRegister, 12, 4, uint16_t>(0x6000) == GeneralRegister::R6,
              "extractBits() failed.");
static_assert(extractEnum<GeneralRegister, 8, 8, uint16_t>(0x0EEF) == GeneralRegister::R14,
              "extractBits() failed.");


}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

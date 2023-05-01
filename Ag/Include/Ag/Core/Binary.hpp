//! @file Ag/Core/Binary.hpp
//! @brief The declaration of various stand-alone operations on binary values.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_BINARY_HPP__
#define __AG_CORE_BINARY_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <type_traits>

#include "Configuration.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Bin {
bool bitScanForward(uint32_t value, int32_t &msb);
bool bitScanForward(uint64_t value, int32_t &msb);
bool bitScanReverse(uint32_t value, int32_t &msb);
bool bitScanReverse(uint64_t value, int32_t &msb);
int32_t log2(uint32_t value, bool roundUp);
int32_t log2(uint64_t value, bool roundUp);
uint32_t rotateLeft(uint32_t bits, int32_t shift);
uint64_t rotateLeft(uint64_t bits, int32_t shift);
uint32_t rotateRight(uint32_t bits, int32_t shift);
uint64_t rotateRight(uint64_t bits, int32_t shift);

uint16_t byteSwap(uint16_t bits);
uint32_t byteSwap(uint32_t bits);
uint64_t byteSwap(uint64_t bits);

uint8_t popCount(uint16_t bits);
uint8_t popCount(uint32_t bits);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Makes a mask of a specified number of consecutive set bits.
//! @tparam TIntegral The unsigned integer type of the mask to create.
//! @param[in] bitCount The count of consecutive bits to set.
//! @return A mask of bits in the appropriate integral type.
template<typename TIntegral> TIntegral makeMask(int32_t bitCount)
{
    TIntegral mask = 0;

    if (bitCount > 0)
    {
        mask = (static_cast<TIntegral>(1) << bitCount) - static_cast<TIntegral>(1);
    }

    return mask;
}

//! @brief Rounds an unsigned integral value up to the nearest whole multiple
//! of a specified power of 2.
//! @tparam TIntegral The unsigned integer type of the value to round up.
//! @param[in] value The value to round up.
//! @param[in] pow2 The power of 2 to round up to.
//! @return The rounded up value, possibly equal to the input value if value was
//! already a whole multiple of the specified power of 2.
template<typename TIntegral> TIntegral roundUpPow2(TIntegral value, int32_t pow2)
{
    if (pow2 > 0)
    {
        TIntegral mask = makeMask<TIntegral>(pow2);

        value = (value + mask) & ~mask;
    }

    return value;
}

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

//// Verify shift and mask.
//static_assert(extractEnum<GeneralRegister, 12, 4, uint32_t>(0) == GeneralRegister::R0,
//              "extractEnum() failed.");
//static_assert(extractEnum<GeneralRegister, 12, 4, uint32_t>(0x5ABCD) == GeneralRegister::R10,
//              "extractBits() failed.");
//
//// Verify just shift.
//static_assert(extractEnum<GeneralRegister, 12, 4, uint16_t>(0x6000) == GeneralRegister::R6,
//              "extractBits() failed.");
//static_assert(extractEnum<GeneralRegister, 8, 8, uint16_t>(0x0EEF) == GeneralRegister::R14,
//              "extractBits() failed.");


} // namespace Bin
} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file Core/Binary.cpp
//! @brief The definition of various stand-alone operations on binary values.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#include <intrin.h>
#include <stdlib.h>
#endif

#include <numeric>

#include "Ag/Core/Binary.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Bin {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Finds the index of the Least Significant Bit (LSB) in a bit field.
//! @param[in] value The value to scan.
//! @param[out] msb Receives the 0-based index of the bit set to 1 with the
//! lowest bit position.
//! @retval true At least one bit was set in value.
//! @retval false No bits were set, value was set to zero.
template<typename TIntegral> bool innerBitScanForward(TIntegral value, int32_t &msb)
{
    bool hasBit = (value != 0);

    if (hasBit)
    {
        for (msb = 0; (value & 1) == 0; value >>= 1)
        {
            ++msb;
        }
    }
    else
    {
        msb = -1;
    }

    return hasBit;
}

//! @brief Finds the index of the most significant bit (MSB) in a bit field.
//! @param[in] value The value to scan.
//! @param[out] msb Receives the 0-based index of the bit set to 1 with the
//! highest bit position.
//! @retval true At least one bit was set in value.
//! @retval false No bits were set, value was set to zero.
template<typename TIntegral> bool innerBitScanReverse(TIntegral value, int32_t &msb)
{
    bool hasBit = (value != 0);

    if (hasBit)
    {
        for (msb = -1; value != 0; value >>= 1)
        {
            ++msb;
        }
    }
    else
    {
        msb = -1;
    }

    return hasBit;
}

//! @brief Shifts an integral value left, reintroducing any bits which are shifted
//! out from the MSB end at the LSB end of the operand.
//! @param[in] bits The binary value to shift.
//! @param[in] shift The count of binary digits to shift by.
//! @returns The shifted value.
template<typename TIntegral> TIntegral innerRotateLeft(TIntegral bits, int32_t shift)
{
    return (bits << shift) | (bits >> (-shift & ((sizeof(TIntegral) * 8) - 1)));
}

//! @brief Shifts an integral value right, reintroducing any bits which are shifted
//! out from the LSB end at the MSB end of the operand.
//! @param[in] bits The binary value to shift.
//! @param[in] shift The count of binary digits to shift by.
//! @returns The shifted value.
template<typename TIntegral> TIntegral innerRotateRight(TIntegral bits, int32_t shift)
{
    return (bits >> shift) | (bits << (-shift & ((sizeof(TIntegral) * 8) - 1)));
}

// Define architecture/compiler specific specialisations.
#if defined _MSC_VER
// MSVC-specific solutions.

#if (defined _M_X64) || (defined _M_X86)
// x86 and x64 specific.
template<> bool innerBitScanReverse(uint32_t value, int32_t &msb)
{
    unsigned long pos;
    unsigned char result = _BitScanReverse(&pos, static_cast<unsigned long>(value));

    msb = static_cast<int32_t>(pos);
    return result != 0;
}
#endif

#if (defined _M_X64)
// x64-specific.
template<> bool innerBitScanForward(uint64_t value, int32_t &msb)
{
    unsigned long pos;
    unsigned char result = _BitScanForward64(&pos,
                                             static_cast<unsigned __int64>(value));

    msb = static_cast<int32_t>(pos);
    return result != 0;
}

template<> bool innerBitScanReverse(uint64_t value, int32_t &msb)
{
    unsigned long pos;
    unsigned char result = _BitScanReverse64(&pos,
                                             static_cast<unsigned __int64>(value));

    msb = static_cast<int32_t>(pos);
    return result != 0;
}
#elif (defined _M_X86)
// x86-specific.
template<> bool innerBitScanForward(uint64_t value, int32_t &msb)
{
    unsigned long pos;
    const unsigned long *words = reinterpret_cast<const unsigned long *>(&value);

    // Scan the high word first.
    unsigned char result = _BitScanForward(&pos, words[0]);

    if (result == 0)
    {
        // Scan the low word otherwise.
        result = _BitScanForward(&pos, words[1]);

        pos += 32;
    }
    else
    {
        // The high word contained the MSB.
        msb = static_cast<int32_t>(pos);
    }

    return result != 0;
}

template<> bool innerBitScanReverse(uint64_t value, int32_t &msb)
{
    unsigned long pos;
    const unsigned long *words = reinterpret_cast<const unsigned long *>(&value);

    // Scan the high word first.
    unsigned char result = _BitScanReverse(&pos, words[1]);

    if (result == 0)
    {
        // Scan the low word otherwise.
        result = _BitScanReverse(&pos, words[0]);

        msb = static_cast<int32_t>(pos);
    }
    else
    {
        // The high word contained the MSB.
        msb = static_cast<int32_t>(pos) + 32;
    }

    return result != 0;
}
#endif // if x86

// ifdef _MSC_VER
#elif (defined __GNUC__)
template<> bool innerBitScanForward<uint32_t>(uint32_t value, int32_t &msb)
{
    if (value != 0)
    {
        msb = __builtin_ctz(value);
    }

    return value != 0;
}

template<> bool innerBitScanForward<uint64_t>(uint64_t value, int32_t &msb)
{
    if (value != 0)
    {
        msb = __builtin_ctzl(value);
    }

    return value != 0;
}

template<> bool innerBitScanReverse<uint32_t>(uint32_t value, int32_t &msb)
{
    if (value != 0)
    {
        msb = ((sizeof(value) * 8) - 1) - __builtin_clz(value);
    }

    return value != 0;
}

template<> bool innerBitScanReverse<uint64_t>(uint64_t value, int32_t &msb)
{
    if (value != 0)
    {
        msb = ((sizeof(value) * 8) - 1) - __builtin_clzl(value);
    }

    return value != 0;
}
#endif // if defined __GNUC__

//! @brief Calculates the integer logarithm of value in base 2.
//! @param[in] value The value to take the logarithm of.
//! @param[in] roundUp True to produce a value which is rounded up to the next
//! power of 2 if value is not a whole power of 2. False to truncate.
//! @return The power of 2 which is equal to or greater than value.
template<typename TIntegral> int32_t innerLog2(TIntegral value, bool roundUp)
{
    int32_t msb = -1;

    if (innerBitScanReverse(value, msb))
    {
        if (roundUp && (value != (static_cast<TIntegral>(1) << msb)))
        {
            // Round up to the next power of 2.
            ++msb;
        }
    }

    return msb;
}

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////


} // TED

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Finds the index of the Least Significant Bit (LSB) in a bit field.
//! @param[in] value The value to scan.
//! @param[out] msb Receives the 0-based index of the bit set to 1 with the
//! lowest bit position.
//! @retval true At least one bit was set in value.
//! @retval false No bits were set, value was set to zero.
bool bitScanForward(uint32_t value, int32_t &msb)
{
    return innerBitScanForward(value, msb);
}

//! @copydoc Ag::Bin::bitScanForward()
bool bitScanForward(uint64_t value, int32_t &msb)
{
    return innerBitScanForward(value, msb);
}

//! @brief Finds the index of the most significant bit (MSB) in a bit field.
//! @param[in] value The value to scan.
//! @param[out] msb Receives the 0-based index of the bit set to 1 with the
//! highest bit position.
//! @retval true At least one bit was set in value.
//! @retval false No bits were set, value was set to zero.
bool bitScanReverse(uint32_t value, int32_t &msb)
{
    return innerBitScanReverse(value, msb);
}

//! @copydoc Ag::Bin::bitScanReverse()
bool bitScanReverse(uint64_t value, int32_t &msb)
{
    return innerBitScanReverse(value, msb);
}

//! @brief Calculates the integer logarithm of value in base 2.
//! @param[in] value The value to take the logarithm of.
//! @param[in] roundUp True to produce a value which is rounded up to the next
//! power of 2 if value is not a whole power of 2. False to truncate.
//! @return The power of 2 which is equal to or greater than value.
int32_t log2(uint32_t value, bool roundUp)
{
    return innerLog2(value, roundUp);
}

//! @copydoc Ag::Bin::log2()
int32_t log2(uint64_t value, bool roundUp)
{
    return innerLog2(value, roundUp);
}

//! @brief Shifts an integral value left, reintroducing any bits which are shifted
//! out from the MSB end at the LSB end of the operand.
//! @param[in] bits The binary value to shift.
//! @param[in] shift The count of binary digits to shift by.
//! @returns The shifted value.
uint32_t rotateLeft(uint32_t bits, int32_t shift)
{
    return innerRotateLeft(bits, shift);
}

//! @copydoc Ag::Bin::rotateLeft()
uint64_t rotateLeft(uint64_t bits, int32_t shift)
{
    return innerRotateLeft(bits, shift);
}

//! @brief Shifts an integral value right, reintroducing any bits which are shifted
//! out from the LSB end at the MSB end of the operand.
//! @param[in] bits The binary value to shift.
//! @param[in] shift The count of binary digits to shift by.
//! @returns The shifted value.
uint32_t rotateRight(uint32_t bits, int32_t shift)
{
    return innerRotateRight(bits, shift);
}

//! @copydoc Ag::Bin::rotateRight()
uint64_t rotateRight(uint64_t bits, int32_t shift)
{
    return innerRotateRight(bits, shift);
}

//! @brief Reverses the order of bytes encoding a 16-bit word.
//! @param bits The value to re-encode.
//! @return The same value with byte order swapped.
uint16_t byteSwap(uint16_t bits)
{
#ifdef _MSC_VER
    return _byteswap_ushort(bits);
#else
    // NOTE: This may not be valid. Try bits ROR #8 if not.
    return __builtin_bswap16(bits);
#endif
}

//! @brief Reverses the order of bytes encoding a 32-bit word.
//! @param bits The value to re-encode.
//! @return The same value with byte order swapped.
uint32_t byteSwap(uint32_t bits)
{
#ifdef _MSC_VER
    return _byteswap_ulong(bits);
#else
    return __builtin_bswap32(bits);
#endif
}

//! @brief Reverses the order of bytes encoding a 64-bit word.
//! @param bits The value to re-encode.
//! @return The same value with byte order swapped.
uint64_t byteSwap(uint64_t bits)
{
#ifdef _MSC_VER
    return _byteswap_uint64(bits);
#else
    return __builtin_bswap64(bits);
#endif
}

//! @brief Counts the number of set bits in a bit field.
//! @param[in] bits The bit field to analyse.
//! @return The count of bits set to 1, 0-16.
//! @note Inspired by https://graphics.stanford.edu/~seander/bithacks.html
uint8_t popCount(uint16_t bits)
{
    static const uint8_t S[] = { 1, 2, 4, 8, 16 }; // Magic Binary Numbers
    static const uint32_t B[] = { 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF };

    uint32_t c = bits - ((bits >> S[0]) & B[0]);
    c = ((c >> S[1]) & B[1]) + (c & B[1]);
    c = ((c >> S[2]) + c) & B[2];
    c = ((c >> S[3]) + c) & B[3];

    return static_cast<uint8_t>(c);
}

//! @brief Counts the number of set bits in a bit field.
//! @param[in] bits The bit field to analyse.
//! @return The count of bits set to 1, 0-32.
// @note Inspired by https://graphics.stanford.edu/~seander/bithacks.html
uint8_t popCount(uint32_t bits)
{
    static const uint8_t S[] = { 1, 2, 4, 8, 16 }; // Magic Binary Numbers
    static const uint32_t B[] = { 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF };

    uint32_t c = bits - ((bits >> S[0]) & B[0]);
    c = ((c >> S[1]) & B[1]) + (c & B[1]);
    c = ((c >> S[2]) + c) & B[2];
    c = ((c >> S[3]) + c) & B[3];
    c = ((c >> S[4]) + c) & B[4];

    return static_cast<uint8_t>(c);
}

}} // namespace Ag::Bin
////////////////////////////////////////////////////////////////////////////////


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
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "Configuration.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////


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
}

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

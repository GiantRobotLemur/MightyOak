//! @file Ag/Core/Math.hpp
//! @brief The declaration of various non-domain-specific mathematical functions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_MATH_HPP__
#define __AG_CORE_MATH_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cmath>

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Rounds a real scalar value up to the next highest integer and
//! casts to an integer type.
//! @tparam TReal The real scalar type to take as input.
//! @tparam TInt The integer scalar type to produce as output.
//! @param[in] value The value to round up.
//! @returns The value rounded up and cast to TInt.
template<typename TReal, typename TInt=int>
TInt roundUp(TReal value)
{
    return static_cast<TInt>(std::ceil(value));
}

//! @brief Rounds a real scalar value down to the next whole integer and
//! casts to an integer type.
//! @tparam TReal The real scalar type to take as input.
//! @tparam TInt The integer scalar type to produce as output.
//! @param[in] value The value to round down.
//! @returns The value rounded down and cast to TInt.
template<typename TReal, typename TInt = int>
TInt roundDown(TReal value)
{
    return static_cast<TInt>(std::floor(value));
}

//! @brief Rounds a real scalar value to the nearest whole integer and
//! casts to an integer type.
//! @tparam TReal The real scalar type to take as input.
//! @tparam TInt The integer scalar type to produce as output.
//! @param[in] value The value to round down.
//! @returns The value rounded down and cast to TInt.
template<typename TReal, typename TInt = int>
TInt roundNearest(TReal value)
{
    return static_cast<TInt>(std::rint(value));
}


} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

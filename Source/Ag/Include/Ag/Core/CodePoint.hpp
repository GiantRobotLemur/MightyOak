//! @file CodePoint.hpp
//! @brief The declaration of functions which manipulate and test Unicode
//! code point values.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_CODE_POINT_HPP__
#define __AG_CORE_CODE_POINT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cuchar>

namespace Ag {
namespace CodePoint {

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
char32_t toUpper(char32_t codePoint);
char32_t toLower(char32_t codePoint);
bool isWhiteSpace(char32_t codePoint);
bool isAlphaNumeric(char32_t codePoint);
bool isNumeric(char32_t codePoint);
bool isHexDigit(char32_t codePoint);
bool isLetter(char32_t codePoint);
bool isPrintable(char32_t codePoint);

}} // namespace Ag::CodePoint

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

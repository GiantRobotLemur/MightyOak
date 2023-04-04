//! @file CodePoint.cpp
//! @brief The definition of functions which manipulate and test Unicode
//! code point values.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cwchar>
#include <cwctype>

#include "Ag/Core/Configuration.hpp"
#include "Ag/Core/Utf.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

namespace CodePoint {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
typedef wint_t (*WideTransformFn)(wint_t);

//! @brief Uses a wide character transformation function to transform a
//! Unicode code point.
//! @tparam T %A pointer to the transformation function to call if the code point
//! can be converted to a single wide character.
//! @param[in] input The code point to convert.
//! @returns The converted code point, possibly the same as the original.
template<WideTransformFn T> char32_t transformCodePoint(char32_t input)
{
    char32_t result = input;
#ifdef WCHAR_IS_32BIT
    result = static_cast<char32_t>(T(static_cast<wchar_t>(input)));
#else
    Utf::ToWideConverter inConverter;

    uint32_t count = inConverter.setCodePoint(input);
    wchar_t wideChar;

    if ((count < 2) && inConverter.tryGetNextCharacter(wideChar))
    {
        Utf::FromWideConverter outConverter;
        bool hasError;

        if (outConverter.tryConvert(T(wideChar), result, hasError) == false)
        {
            result = input;
        }
    }
#endif

    return result;
}

typedef int (*WideClassifyFn)(wint_t);

//! @brief Uses a wide character classification function to test a Unicode code
//! point.
//! @tparam T A pointer to the wide character classification function to call
//! if the code point can be represented as a single wide character.
//! @param[in] codePoint The Unicode code point to test.
//! @returns The boolean result of the test.
template<WideClassifyFn T> bool classifyCodePoint(char32_t codePoint)
{
    int result = 0;
#ifdef WCHAR_IS_32BIT
    result = T(static_cast<wchar_t>(codePoint));
#else
    Utf::ToWideConverter inConverter;

    uint32_t count = inConverter.setCodePoint(codePoint);
    wchar_t wideChar;

    if ((count < 2) && inConverter.tryGetNextCharacter(wideChar))
    {
        result = T(wideChar);
    }
#endif

    return result != 0;
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Converts a Unicode code point to upper case.
//! @param[in] codePoint The code point to convert.
//! @returns The converted code point.
char32_t toUpper(char32_t codePoint)
{
    return transformCodePoint<towupper>(codePoint);
}

//! @brief Converts a Unicode code point to upper case.
//! @param[in] codePoint The code point to convert.
//! @returns The converted code point.
char32_t toLower(char32_t codePoint)
{
    return transformCodePoint<towlower>(codePoint);
}

//! @brief Determines if a Unicode code point represents a white space character.
//! @param[in] codePoint The code point to test.
//! @retval true The code point represents white space.
//! @retval false The code point is not white space.
bool isWhiteSpace(char32_t codePoint)
{
    return classifyCodePoint<iswspace>(codePoint);
}

//! @brief Determines if a Unicode code point represents a letter or number.
//! @param[in] codePoint The code point to test.
//! @retval true The code point represents a letter or number.
//! @retval false The code point is not a letter or number.
bool isAlphaNumeric(char32_t codePoint)
{
    return classifyCodePoint<iswalnum>(codePoint);
}

//! @brief Determines if a Unicode code point represents a decimal digit.
//! @param[in] codePoint The code point to test.
//! @retval true The code point represents a decimal digit.
//! @retval false The code point is not a decimal digit.
bool isNumeric(char32_t codePoint)
{
    return classifyCodePoint<iswdigit>(codePoint);
}

//! @brief Determines if the character represents a digit in a
//! hexadecimal value.
//! @param[in] codePoint The character to test.
//! @retval true The character is in the set [0-9A-Fa-f].
//! @retval false The character is not in the required set.
bool isHexDigit(char32_t codePoint)
{
    return ((codePoint >= U'0') && (codePoint <= U'9')) ||
        ((codePoint >= U'A') && (codePoint <= U'F')) ||
        ((codePoint >= U'a') && (codePoint <= U'f'));
}

//! @brief Determines if a Unicode code point represents a letter.
//! @param[in] codePoint The code point to test.
//! @retval true The code point represents a letter.
//! @retval false The code point is not a letter.
bool isLetter(char32_t codePoint)
{
    return classifyCodePoint<iswalpha>(codePoint);
}

//! @brief Determines if a Unicode code point represents a printable character.
//! @param[in] codePoint The code point to test.
//! @retval true The code point represents a printable character.
//! @retval false The code point is not a printable character.
bool isPrintable(char32_t codePoint)
{
    return classifyCodePoint<iswprint>(codePoint);
}

}} // namespace Ag::CodePoint
////////////////////////////////////////////////////////////////////////////////


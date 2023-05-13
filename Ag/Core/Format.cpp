//! @file Core/Format.cpp
//! @brief The definition of objects and functions used to format values as text.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "Ag/Core/String.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/ScalarParser.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Variant.hpp"
#include "Ag/Core/VariantTypes.hpp"
#include "CoreInternal.hpp"

#ifdef _WIN32
#include "Win32API.hpp"
#else
#include <nl_types.h>
#include <langinfo.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#ifndef _CVTBUFSIZE
#define _CVTBUFSIZE 308
#endif

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which can assemble digit characters into a formatted value.
class NumericCharacters
{
private:
    // Internal Fields
    std::string_view _whole;
    std::string_view _fraction;
    std::string_view _exponent;
    size_t _extraFraction;
    char _sign;
    char _expSign;
public:
    // Construction
    //! @brief Constructs an empty object to format a scalar.
    NumericCharacters() :
        _extraFraction(0),
        _sign('+'),
        _expSign('+')
    {
    }

    //! @brief Constructs an object to format a scalar with only a whole number
    //! portion.
    //! @param[in] sign The sign character, if any.
    //! @param[in] whole The null-terminated array whole number digits.
    NumericCharacters(char sign, const char *whole) :
        _whole(whole),
        _extraFraction(0),
        _sign((sign == '-') ? sign : '+'),
        _expSign('+')
    {
    }

    //! @brief Constructs an object to format a scalar with only a whole number
    //! portion.
    //! @param[in] sign The sign character, if any.
    //! @param[in] decimalPtPos The position of the decimal point, negative if
    //! there are extra zeros at the beginning of the fractional portion.
    //! @param[in] allDigits The null-terminated array of digits possibly including
    //! both whole number and fractional.
    NumericCharacters(char sign, int decimalPtPos, const char *allDigits) :
        _extraFraction((decimalPtPos < 0) ? static_cast<size_t>(-decimalPtPos) : 0),
        _sign((sign == '-') ? sign : '+'),
        _expSign('+')
    {
        if (decimalPtPos <= 0)
        {
            // All digits are fractional digits.
            _fraction = allDigits;

            // Assign the missing leading zero.
            _whole = "0";
        }
        else
        {
            // Some digits are whole number digits.
            _whole = std::string_view(allDigits, static_cast<size_t>(decimalPtPos));
            _fraction = allDigits + decimalPtPos;
        }
    }

    //! @brief Constructs an object to format a scalar with only a whole number
    //! portion.
    //! @param[in] sign The sign character, if any.
    //! @param[in] decimalPtPos The position of the decimal point, negative if
    //! there are extra zeros at the beginning of the fractional portion.
    //! @param[in] allDigits The null-terminated array of digits possibly including
    //! both whole number and fractional.
    NumericCharacters(char sign, int decimalPtPos, const char *allDigits,
                      char expSign, const char *exponentDigits) :
        _exponent(exponentDigits),
        _extraFraction((decimalPtPos < 0) ? static_cast<size_t>(-decimalPtPos) : 0),
        _sign((sign == '-') ? sign : '+'),
        _expSign(expSign)
    {
        if (decimalPtPos <= 0)
        {
            // All digits are fractional digits.
            _fraction = allDigits;

            // Assign the missing leading zero.
            _whole = "0";
        }
        else
        {
            // Some digits are whole number digits.
            _whole = std::string_view(allDigits, static_cast<size_t>(decimalPtPos));
            _fraction = allDigits + decimalPtPos;
        }
    }

    //! @brief Constructs an object to format a scalar with only a whole number
    //! portion.
    //! @param[in] sign The sign character, if any.
    //! @param[in] whole The bounded array of numeric whole number digits.
    //! @param[in] count The count of digits in the whole array.
    NumericCharacters(char sign, const char *whole, size_t count) :
        _whole(whole, count),
        _extraFraction(0),
        _sign((sign == '-') ? sign : '+'),
        _expSign('+')
    {
    }

    //! @brief Constructs an object to format a scalar with a whole number and
    //! fractional portions.
    //! @param[in] sign The sign character, if any.
    //! @param[in] whole The bounded array of numeric whole number digits.
    //! @param[in] wCount The count of digits in the whole array.
    //! @param[in] fract The bounded array of numeric fraction digits.
    //! @param[in] fCount The count of digits in the fraction array.
    NumericCharacters(char sign, const char *whole, size_t wCount,
                      const char *fract, size_t fCount) :
        _whole(whole, wCount),
        _fraction(fract, fCount),
        _extraFraction(0),
        _sign((sign == '-') ? sign : '+'),
        _expSign('+')
    {
    }

    //! @brief Constructs an object to format a scalar with a whole number,
    //! fractional and exponent portions.
    //! @param[in] sign The sign character, if any.
    //! @param[in] whole The bounded array of numeric whole number digits.
    //! @param[in] wCount The count of digits in the whole array.
    //! @param[in] fract The bounded array of numeric fraction digits.
    //! @param[in] fCount The count of digits in the fraction array.
    //! @param[in] expSign The sign character of the exponent, if any.
    //! @param[in] exp The bounded array of numeric exponent digits.
    //! @param[in] eCount The count of digits in the exp array.
    NumericCharacters(char sign, const char *whole, size_t wCount,
                      const char *fract, size_t fCount,
                      char expSign, const char *exp, size_t eCount) :
        _whole(whole, wCount),
        _fraction(fract, fCount),
        _exponent(exp, eCount),
        _extraFraction(0),
        _sign((sign == '-') ? sign : '+'),
        _expSign((expSign == '-') ? expSign : '+')
    {
    }

    // Operations
    //! @brief Appends a non-numeric value to a string using the characters of
    //! the whole number component.
    //! @param[in] options Defines how the string should be formatted.
    //! @param[out] destination Receives the formatted text.
    void formatString(const FormatInfo &options, std::string &destination) const
    {
        // Assembles a value composed of the following components (in order):
        //  {left padding}
        //  {whole digits characters}
        //  {right padding}
        size_t fieldLength = std::max(static_cast<size_t>(options.getMinimumFieldWidth()),
                                      _whole.length());
        size_t padding = fieldLength - _whole.length();

        if ((padding > 0) && options.isRightAligned())
        {
            // Add leading padding.
            destination.append(padding, ' ');
        }

        destination.append(_whole);

        if ((padding > 0) && (options.isRightAligned() == false))
        {
            // Add trailing padding.
            destination.append(padding, ' ');
        }
    }

    //! @brief Appends a real value to a string using the whole, fraction and
    //! exponent components of the value.
    //! @param[in] options Defines how the value should be formatted.
    //! @param[out] destination Receives the formatted text.
    void formatValue(const FormatInfo &options, std::string &destination) const
    {
        // Assembles a value composed of the following components (in order):
        //  {left padding}
        //  {sign}
        //  {whole digits/thousand separators}
        //  {decimal point}
        //  {extra fraction zeros}
        //  {fractional digits}
        //  {trailing zeros}
        //  {exponent sign}
        //  {exponent digits}
        //  {right padding}
        // Note field width is not the same as char count when it comes to
        // field width. We will count fully formed printable characters.
        size_t separatorCharCount = options.getThousandSeparator().getPrintLength();
        size_t wholeDigits = std::max(static_cast<size_t>(options.getMinimumWholeDigits()),
                                      _whole.length());
        size_t leadingZeros = wholeDigits - _whole.length();

        size_t fieldLength = wholeDigits;

        if (_whole.length() > 3)
        {
            // Add the number of (printable) characters for thousand separators.
            fieldLength += ((fieldLength - 1) / 3) * separatorCharCount;
        }

        if ((_sign == '-') || options.isSignForced())
        {
            ++fieldLength;
        }

        size_t trailingZeros = 0;
        bool hasFraction = false;

        if ((_extraFraction > 0) ||
            (options.getRequiredFractionDigits() > 0) ||
            (options.getRequiredSignificantFigures() > 0) ||
            (_fraction.empty() == false))
        {
            hasFraction = true;
            size_t reqFraction = toSize(options.getRequiredFractionDigits());
            size_t reqSigFig = toSize(options.getRequiredSignificantFigures());

            if (wholeDigits < reqSigFig)
            {
                // We need to fulfil our significant figure requirement by
                // increasing the number of fractional digits.
                reqFraction = std::max(reqFraction,
                                       reqSigFig - wholeDigits);
            }

            size_t fractionSize = std::max(reqFraction, _fraction.length() + _extraFraction);

            trailingZeros = fractionSize - _fraction.length() - _extraFraction;

            fieldLength += options.getDecimalSeparator().getPrintLength();
            fieldLength += fractionSize;
        }

        bool hasExponent = false;
        if (_exponent.empty() == false)
        {
            hasExponent = true;

            // Add space of the 'e'.
            ++fieldLength;

            if ((_expSign == '-') || options.isSignForced())
            {
                // Add space for the exponent sign.
                ++fieldLength;
            }

            fieldLength += _exponent.length();
        }

        size_t padding = std::max(static_cast<size_t>(options.getMinimumFieldWidth()),
                                  fieldLength) - fieldLength;

        // Assemble the characters.
        if (options.isRightAligned() && (padding > 0))
        {
            // Padding goes first.
            destination.append(padding, ' ');
        }

        if (options.isSignForced() || (_sign == '-'))
        {
            destination.push_back(_sign);
        }

        // Keep track of where we need to add thousand separators.
        size_t digitModulus = 3 - ((_whole.length() + leadingZeros) % 3);

        // Ensure there is no leading thousand separator.
        if (digitModulus == 3)
        {
            digitModulus = 0;
        }

        if (leadingZeros > 0)
        {
            if (separatorCharCount > 0)
            {
                const String &separator = options.getThousandSeparator();

                for (size_t zeroCount = 0; zeroCount < leadingZeros; ++zeroCount)
                {
                    destination.push_back('0');

                    ++digitModulus;

                    if (digitModulus >= 3)
                    {
                        digitModulus = 0;
                        destination.append(separator.getUtf8Bytes(),
                                           separator.getUtf8Length());
                    }
                }
            }
            else
            {
                // We need to pad taking thousand separators into account.
                destination.append(leadingZeros, '0');
            }
        }

        if (separatorCharCount > 0)
        {
            // Append digits interspersed with thousand separators.
            const String &separator = options.getThousandSeparator();

            for (size_t index = 0, count = _whole.length(); index < count; ++index)
            {
                destination.push_back(_whole[index]);

                ++digitModulus;

                if (digitModulus >= 3)
                {
                    digitModulus = 0;

                    if ((count - index) >= 3)
                    {
                        // Append the separator apart from after the least
                        // significant digit.
                        destination.append(separator.getUtf8Bytes(),
                                           separator.getUtf8Length());
                    }
                }
            }
        }
        else
        {
            // Simply append the value digits with no separators.
            destination.append(_whole);
        }

        if (hasFraction)
        {
            const String &separator = options.getDecimalSeparator();

            destination.append(separator.getUtf8Bytes(),
                               separator.getUtf8Length());

            if (_extraFraction > 0)
            {
                destination.append(_extraFraction, '0');
            }

            destination.append(_fraction);

            if (trailingZeros > 0)
            {
                destination.append(trailingZeros, '0');
            }
        }

        if (hasExponent)
        {
            destination.push_back(options.isUpperCase() ? 'E' : 'e');

            if (options.isExponentSignForced() || (_expSign == '-'))
            {
                destination.push_back(_expSign);
            }

            destination.append(_exponent);
        }

        if ((options.isRightAligned() == false) && (padding > 0))
        {
            // Padding goes last.
            destination.append(padding, ' ');
        }
    }
};

//! @brief An exception thrown when string formatting fails.
class FormatException : public Exception
{
public:
    //! @brief Constructs an exception indicating an error in format specification.
    //! @param[in] formatToken The value insertion token which was invalid.
    FormatException(size_t paramIndex, const std::string_view &tokenError)
    {
        std::string detail;
        detail.assign("While processing for insertion token {");
        appendValue(FormatInfo::getNeutral(), detail, paramIndex);
        detail.append("}.");

        initialise("FormatException", tokenError, detail, 0);
    }

    //! @brief Constructs an exception indicating an error in format specification.
    //! @param[in] formatToken The value insertion token which was invalid.
    FormatException(const std::string_view &formatToken)
    {
        std::string detail;
        detail.assign("The insertion token '");
        detail.append(formatToken);
        detail.append("' had an invalid format.");

        initialise("FormatException",
                   "A string format specification contained had an invalid "
                   "value insertion token.", detail, 0);
    }

    //! @brief Constructs an exception indicating an unknown type code.
    //! @param[in] typeCode The type code which was invalid.
    FormatException(char typeCode)
    {
        std::string detail;
        appendPrintf(detail, "The value type '%c' was not recognised.", typeCode);

        initialise("FormatException",
                   "A string format specification contained had an invalid "
                   "value insertion token.", detail, 0);
    }

    //! @brief Constructs an exception indicating an attempt to access a
    //! format value which is beyond the end of the set specified.
    //! @param[in] paramCount The count of parameters specified.
    //! @param[in] paramIndex The index of the invalid parameter.
    FormatException(size_t paramCount, size_t paramIndex)
    {
        std::string detail;
        appendPrintf(detail, "The parameter %zu cannot be accessed when "
                     "only %zu were specified.", paramIndex, paramCount);

        initialise("FormatException",
                   "A string format specification contained had a reference to"
                   "a value beyond the end of the set specified for insertion.",
                   detail, 0);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
//! @brief Represents the insertion parameters parsed from a string format
//! specification.
struct InsertionToken
{
    size_t ValueIndex;
    int32_t Precision;
    char TypeCode;

    // Construction
    InsertionToken() :
        ValueIndex(~0u),
        Precision(-1),
        TypeCode('\0')
    {
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
//! @brief Gets the value of a Win32 string locale property.
//! @param[in] localeName The name of the locale.
//! @param[in] prop The identifier of the property to query.
//! @param[out] value A string to receive the value of the property.
//! @retval true The property was successfully obtained.
//! @retval false The property could not be found.
bool tryGetLocaleInfo(wchar_cptr_t localeName, LCTYPE prop, std::wstring &value)
{
    int length = ::GetLocaleInfoEx(localeName, prop, nullptr, 0);
    bool isOK = false;

    if (length > 1)
    {
        std::vector<wchar_t> buffer;
        buffer.resize(static_cast<size_t>(length));

        length = ::GetLocaleInfoEx(localeName, prop, buffer.data(), length);

        if (length > 0)
        {
            value.assign(buffer.data(), static_cast<size_t>(length) - 1);
            isOK = true;
        }
    }

    return isOK;
}

//! @brief Gets the value of a Win32 numeric locale property.
//! @param[in] localeName The name of the locale.
//! @param[in] prop The identifier of the property to query.
//! @param[out] value A receives the value of the property.
//! @retval true The property was successfully obtained.
//! @retval false The property could not be found.
bool tryGetLocaleInfo(wchar_cptr_t localeName, LCTYPE prop, uint32_t &value)
{
    value = 0;
    int length = ::GetLocaleInfoEx(localeName, prop | LOCALE_RETURN_NUMBER,
                                   reinterpret_cast<wchar_ptr_t>(&value),
                                   sizeof(value) / sizeof(wchar_t));
    return length > 0;
}

#endif

//! @brief Appends the digits of an integer to a buffer.
//! @tparam T The data type of the integer value being converted.
//! @param[out] buffer The buffer to receive the digits.
//! @param[in,out] sign Zero for upper case digits, non-zero for lower case,
//! receives the sign character of the formatted value.
//! @param[in] maxBuffer The maximum number of characters which can be stored
//! in buffer.
//! @param[in] radix The radix used to convert the value.
//! @param[in] value The value to convert to digits.
//! @returns The count of digits stored.
template<typename T>
size_t appendDigits(char *buffer, char &sign, size_t maxBuffer, uint8_t radix, T value)
{
    using TypeInfo = std::numeric_limits<T>;
    const char *upperDigits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *lowerDigits = "0123456789abcdefghijklmnopqrstuvwxyz";
    const char *digits = (sign == '\0') ? upperDigits : lowerDigits;

    sign = '+';
    size_t bufferUsed = 0;

    if (value == static_cast<T>(0))
    {
        if (maxBuffer > 0)
        {
            buffer[0] = '0';
            bufferUsed = 1;
        }
    }
    else
    {
        const T zero = static_cast<T>(0);
        T denominator = static_cast<T>(radix);

        bool isFirst = false;

        if constexpr (TypeInfo::is_signed)
        {
            if (value < zero)
            {
                // Ensure the first time through we divide by a
                // negative radix.
                denominator = -denominator;
                sign = '-';
                isFirst = true;
            }
        }

        while ((value != zero) && (bufferUsed < maxBuffer))
        {
            int digit;

            digit = static_cast<int>(value % denominator);
            value /= denominator;

            if constexpr (TypeInfo::is_signed)
            {
                if (isFirst)
                {
                    isFirst = false;
                    digit = -digit;
                    denominator = -denominator;
                }
            }

            buffer[bufferUsed++] = digits[digit];
        }

        // Reverse the order of digits in the buffer.
        std::reverse(buffer, buffer + bufferUsed);
    }

    return bufferUsed;
}

//! @brief Formats a signed or unsigned integer value as text.
//! @tparam T The data type of the value to format.
//! @param[in] options An object defining how the value should be formatted.
//! @param[out] buffer The STL string to append the formatted value to.
//! @param[in] value The value to format and append.
template<typename T>
void appendIntegralValue(const FormatInfo &options, std::string &buffer, T value)
{
    // Create a temporary buffer big enough to hold the maximum number of digits
    // given the worst case (radix = 2).
    char digitBuffer[sizeof(T) * 8];
    char sign = options.isUpperCase() ? 0 : 1;

    size_t digitCount = appendDigits(digitBuffer, sign,
                                     std::size(digitBuffer),
                                     options.getRadix(), value);

    NumericCharacters characters(sign, digitBuffer, digitCount);

    characters.formatValue(options, buffer);
}

//! @brief Handles invalid real value representations.
//! @param[out] chars Receives whole number characters representing the abnormal
//! value, if the value is not normal.
//! @param[in] value The real value to analyse.
//! @param[out] isZero Receives true the value is zero, false if non-zero.
//! @retval true The value should be formatted normally.
//! @retval false The digits of the value have been set in chars.
bool handleUnreal(NumericCharacters &chars, double value, bool &isZero)
{
    int valueClass = std::fpclassify(value);
    bool isReal = false;
    isZero = false;

    switch (valueClass)
    {
    case FP_NAN: // Not A Number
        chars = NumericCharacters('+', "(NaN)");
        break;

    case FP_INFINITE:
        chars = NumericCharacters('+', "(Inf)");
        break;

    case FP_SUBNORMAL: // Too close to 0 to be able to represent.
    case FP_ZERO:
        isZero = true;
        // Intentional fall-through.
    case FP_NORMAL:
        isReal = true;
        break;
    }

    return isReal;
}

//! @brief Calculates the digits of a real value rounded to a fixed number
//! of significant figures.
//! @param[in] value The real value to format.
//! @param[in] sigFigs The count of significant figures to round to.
//! @param[out] digitBuffer The buffer to receive the digits.
//! @param[in] bufferSize The size of digitBuffer in characters.
//! @param[out] decPtIndex Receives the index of the decimal point.
//! @param[out] sign Receives the sign.
//! @retval 0 If successful
//! @retval !0 Conversion failed.
int realToSignificantDigits(double value, int sigFigs, char *digitBuffer,
                            size_t bufferSize, int *decPtIndex, int *sign)
{
#ifdef _MSC_VER
    return _ecvt_s(digitBuffer, bufferSize, value, sigFigs, decPtIndex, sign);
#else
    return ecvt_r(value, sigFigs, decPtIndex, sign, digitBuffer, bufferSize);
#endif
}

//! @brief Calculates the digits of a real value rounded to a fixed number
//! of decimal places.
//! @param[in] value The real value to format.
//! @param[in] fractDigits The count of decimal places to round to.
//! @param[out] digitBuffer The buffer to receive the digits.
//! @param[in] bufferSize The size of digitBuffer in characters.
//! @param[out] decPtIndex Receives the index of the decimal point.
//! @param[out] sign Receives the sign.
//! @retval 0 If successful
//! @retval !0 Conversion failed.
int realToFractionDigits(double value, int fractDigits, char *digitBuffer,
                         size_t bufferSize, int *decPtIndex, int *sign)
{
#ifdef _MSC_VER
    return _fcvt_s(digitBuffer, bufferSize, value, fractDigits, decPtIndex, sign);
#else
    return fcvt_r(value, fractDigits, decPtIndex, sign, digitBuffer, bufferSize);
#endif
}

//! @brief Calculates the real value best used to format a file size.
//! @param[in] format The format requested.
//! @param[in] byteCount The byte count to scale.
//! @param[out] scaledValue The scaled byte count.
//! @return The scale order of magnitude, 0 = 1, 1 = 1024, 2 = 1MB, 3 = 1GB, etc.
uint8_t fileSizeToReal(const FormatInfo &format, uint64_t byteCount, double &scaledValue)
{
    if (byteCount == 0)
    {
        scaledValue = 0.0;
        return 0;
    }

    // The maximum scale we can use for a size_t type.
    static constexpr uint8_t MaxMagnitude = (sizeof(uint64_t) * 8) / 10;

    int16_t requiredWholeDigits = 4;

    if (format.getMinimumWholeDigits() > 0)
    {
        requiredWholeDigits = format.getMinimumWholeDigits();
    }

    uint8_t magnitude = 0;
    uint8_t preferredMagnitude = 0;
    scaledValue = static_cast<double>(byteCount);

    while (magnitude < MaxMagnitude)
    {
        size_t divisor = static_cast<size_t>(1) << (magnitude * 10);
        double realValue = byteCount / static_cast<double>(divisor);

        int16_t wholeDigitCount = static_cast<int16_t>(std::floor(std::log10(realValue)) + 1.0);

        if (wholeDigitCount <= requiredWholeDigits)
        {
            preferredMagnitude = magnitude;
            scaledValue = realValue;
            break;
        }
        else
        {
            ++magnitude;
        }
    }

    return preferredMagnitude;
}

//! @brief Calculates the real value best used to format a file size.
//! @param[in] format The format requested.
//! @param[in] byteCount The byte count to scale.
//! @param[out] scaledValue The scaled byte count.
//! @return The scale order of magnitude, 0 = 1, 1 = 1KB, 2 = 1MB, 3 = 1GB, etc.
uint8_t fileSizeToReal(const FormatInfo &format, double byteCount, double &scaledValue)
{
    if (byteCount == 0.0)
    {
        scaledValue = 0.0;
        return 0;
    }

    // The maximum scale we can use for a size_t type.
    static constexpr uint8_t MaxMagnitude = 8; // Yotta bytes 2^80
    int16_t requiredWholeDigits = 4;

    if (format.getMinimumWholeDigits() > 0)
    {
        requiredWholeDigits = format.getMinimumWholeDigits();
    }

    double baseCount = std::abs(byteCount);

    uint8_t magnitude = 0;
    uint8_t preferredMagnitude = 0;
    scaledValue = baseCount;

    while (magnitude < MaxMagnitude)
    {
        double divisor = std::pow(2.0, 10.0 * magnitude);
        double realValue = baseCount / divisor;

        int16_t wholeDigitCount = static_cast<int16_t>(std::floor(std::log10(realValue)) + 1.0);

        if (wholeDigitCount <= requiredWholeDigits)
        {
            // Preserve the sign in the returned size.
            preferredMagnitude = magnitude;
            scaledValue = byteCount / divisor;
            break;
        }
        else
        {
            ++magnitude;
        }
    }

    return preferredMagnitude;
}

//! @brief Appends the appropriate units to a scaled file size.
//! @param[in] buffer The UTF-8 buffer to append to.
//! @param[in] magnitude The magnitude of the value scale, 0 for bytes,
//! 1 for KB, 2 for GB, etc.
//! @param[in] isOne True if the value formatted was +/- 1 byte in order
//! to format the unit as 'byte' or 'bytes'.
void appendFileSizeUnit(std::string &buffer, uint8_t magnitude, bool isOne)
{
    // Append the units.
    switch (magnitude)
    {
    case 0: buffer.append(isOne ? "byte" : "bytes"); break;
    case 1: buffer.append("KB"); break;
    case 2: buffer.append("MB"); break;
    case 3: buffer.append("GB"); break;
    case 4: buffer.append("TB"); break;
    case 5: buffer.append("PB"); break;
    case 6: buffer.append("EB"); break;
    case 7: buffer.append("ZB"); break;
    case 8: buffer.append("YB"); break;
    }
}

//! @brief Attempts to parse an insertion token embedded within a format
//! specification string.
//! @param[in] source The format specification.
//! @param[in,out] offset The offset of the character in source just after the
//! opening brace '{' of the insertion token. Receives the offset just after
//! the last character parsed, on success or failure.
//! @param[out] token A structure to fill in with details of the
//! insertion token.
//! @retval true The insertion token was successfully parsed, offset points to
//! the character after the closing brace '}'.
//! @retval false The token was invalid, offset is updated to the character
//! after the last one parsed.
bool tryParseInsertionToken(const std::string_view &source, size_t &offset,
                            InsertionToken &token)
{
    // We are attempting to parse '{' [0-9]+ (':' [A-Za-z] [0-9]*)? '}'.
    // The initial opening brace should already have been recognised, so offset
    // should point to the first digit.
    ScalarParser parser(LocaleInfo::getNeutral());
    parser.enableExponent(false);
    parser.enableFraction(false);
    parser.enableSign(false);
    parser.enableRadixPrefix(false);

    bool hasDigits = true;

    while (hasDigits && (offset < source.length()))
    {
        if (parser.tryProcessCharacter(source[offset]))
        {
            ++offset;
        }
        else
        {
            hasDigits = false;
        }
    }

    bool isOK = false;

    if (parser.tryGetValue(token.ValueIndex))
    {
        if (offset < source.length())
        {
            char next = source[offset];

            if (next == ':')
            {
                ++offset;

                if (offset < source.length())
                {
                    next = source[offset];

                    if (((next >= 'A') && (next <= 'Z')) ||
                        ((next >= 'a') && (next <= 'z')))
                    {
                        token.TypeCode = next;
                        ++offset;

                        if (offset < source.length())
                        {
                            next = source[offset];

                            if (next == '}')
                            {
                                // The token was properly closed.
                                ++offset;
                                isOK = true;
                            }
                            else if ((next >= '0') && (next <= '9'))
                            {
                                // Parse the precision digits.
                                parser.reset();
                                hasDigits = true;

                                while (hasDigits && (offset < source.length()))
                                {
                                    if (parser.tryProcessCharacter(source[offset]))
                                    {
                                        ++offset;
                                    }
                                    else
                                    {
                                        hasDigits = false;
                                    }
                                }

                                if (hasDigits == false)
                                {
                                    // We reached a non-digit before the end of source.
                                    if ((source[offset] == '}') &&
                                        parser.tryGetValue(token.Precision))
                                    {
                                        // We have a valid precision value and
                                        // the token was properly terminated.
                                        ++offset;
                                        isOK = true;
                                    }
                                }
                            }
                            else
                            {
                                // Update offset to indicate the extent of the invalid
                                // insertion token.
                                ++offset;
                            }
                        }
                    }
                    else
                    {
                        // Update offset to indicate the extent of the invalid
                        // insertion token.
                        ++offset;
                    }
                }
            }
            else if (next == '}')
            {
                // The token was properly closed.
                ++offset;
                isOK = true;
            }
        }
    }

    return isOK;
}

//! @brief Appends a value to a buffer.
//! @param[out] buffer The STL string buffer to append to.
//! @param[in] token Details of the token to be formatted.
//! @param[in] options The base options for formatting values as text.
//! @param[in] value The value to format.
void formatValue(std::string &buffer, const InsertionToken &token,
                 const FormatInfo &options, const Variant &value)
{
    if ((token.TypeCode == '\0') ||
        (token.TypeCode == 'C') ||
        (token.TypeCode == 'c'))
    {
        // No format code was specified or it was a character, which requires
        // no special formatting.
        value.appendToString(options, buffer);
    }
    else if ((token.TypeCode == 'I') ||
             (token.TypeCode == 'i') ||
             (token.TypeCode == 'D') ||
             (token.TypeCode == 'd') ||
             (token.TypeCode == 'U') ||
             (token.TypeCode == 'u'))
    {
        // Format a decimal integer.
        FormatInfo valueOptions(options);
        valueOptions.setRadix(10);

        if (token.Precision >= 0)
        {
            valueOptions.setMinimumWholeDigits(static_cast<uint16_t>(token.Precision));
        }

        valueOptions.setRequiredFractionDigits(0);

        value.appendToString(valueOptions, buffer);
    }
    else if ((token.TypeCode == 'X') ||
             (token.TypeCode == 'x'))
    {
        // Format a hexadecimal value.
        FormatInfo hexOptions(options);

        hexOptions.setRadix(16);
        hexOptions.enableUpperCase(token.TypeCode == 'X');

        value.appendToString(hexOptions, buffer);
    }
    else if ((token.TypeCode == 'P') ||
             (token.TypeCode == 'p'))
    {
        // Format a pointer.
        FormatInfo pointerOptions(options);
        pointerOptions.setRadix(16);
        pointerOptions.setMinimumWholeDigits(sizeof(void *) * 2);
        pointerOptions.setThousandSeparator(String::Empty);
        pointerOptions.setRequiredFractionDigits(0);
        pointerOptions.setRequiredSignificantFigures(0);
        pointerOptions.enableForcedSign(false);

        value.appendToString(pointerOptions, buffer);
    }
    else if ((token.TypeCode == 'E') ||
             (token.TypeCode == 'e'))
    {
        // Format a real value with a fixed number of significant digits.
        FormatInfo realOptions(options);

        realOptions.setMinimumWholeDigits(1);
        realOptions.setRequiredFractionDigits(-1);

        if (token.Precision >= 0)
        {
            realOptions.setRequiredSignificantFigures(static_cast<int16_t>(token.Precision));
        }

        value.appendToString(realOptions, buffer);
    }
    else if ((token.TypeCode == 'F') ||
             (token.TypeCode == 'f'))
    {
        // Format a real value with a fixed number of significant digits.
        FormatInfo realOptions(options);

        realOptions.setMinimumWholeDigits(1);
        realOptions.setRequiredSignificantFigures(-1);

        if (token.Precision >= 0)
        {
            realOptions.setRequiredFractionDigits(static_cast<int16_t>(token.Precision));
        }

        value.appendToString(realOptions, buffer);
    }
    else if ((token.TypeCode == 'G') ||
             (token.TypeCode == 'g'))
    {
        // Format a real value using the shortest representation.
        FormatInfo realOptions(options);

        realOptions.setMinimumWholeDigits(1);
        realOptions.setRequiredSignificantFigures(-1);
        realOptions.setRequiredFractionDigits(-1);

        if (token.Precision >= 0)
        {
            realOptions.setMinimumFieldWidth(static_cast<uint16_t>(token.Precision));
        }

        value.appendToString(realOptions, buffer);
    }
    else if ((token.TypeCode == 'S') ||
             (token.TypeCode == 's'))
    {
        // Format a string.
        FormatInfo stringOptions(options);

        if (token.Precision >= 0)
        {
            stringOptions.setRequiredSignificantFigures(static_cast<uint16_t>(token.Precision));
        }

        value.appendToString(stringOptions, buffer);
    }
    else if ((token.TypeCode == 'K') ||
             (token.TypeCode == 'k'))
    {
        // Format a string.
        FormatInfo sizeOptions(options);

        if (token.Precision >= 0)
        {
            sizeOptions.setRequiredFractionDigits(static_cast<int16_t>(token.Precision));
        }

        Variant scalarType;

        if (value.getType() == VariantTypes::Double)
        {
            appendRealFileSize(sizeOptions, buffer,
                               value.getRef<DoubleVariantType, double>());
        }
        else if (value.getType() == VariantTypes::Float)
        {
            float originalValue = value.getRef<FloatVariantType, float>();

            appendRealFileSize(sizeOptions, buffer, originalValue);
        }
        else if (value.tryConvert(VariantTypes::Uint64, scalarType))
        {
            appendFileSize(sizeOptions, buffer,
                           value.getRef<Uint64VariantType, uint64_t>());
        }
        else
        {
            throw FormatException(token.ValueIndex,
                                  "Only scalar values can be formatted as a file size.");
        }
    }
    else
    {
        // Unknown format type code.
        throw FormatException(token.TypeCode);
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// MemberFunction Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a neutral locale description.
LocaleInfo::LocaleInfo() :
    _decimalSeparator(U".", 1)
{
}

//! @brief Gets the text used to separate whole number digits when thousand
//! separators are in use.
const String &LocaleInfo::getThousandSeparator() const
{
    return _thousandSeparator;
}

//! @brief Sets the text used to separate whole number digits when thousand
//! separators are in use.
//! @param[in] separator The new separator value, if empty, use of separators
//! will be implicitly disabled.
void LocaleInfo::setThousandSeparator(const String &separator)
{
    _thousandSeparator = separator;
}

//! @brief Gets the value used to separate whole number digits from
//! fractional digits.
const String &LocaleInfo::getDecimalSeparator() const
{
    return _decimalSeparator;
}

//! @brief Sets the value used to separate whole number digits from
//! fractional digits.
//! @param[in] separator The new separator value.
void LocaleInfo::setDecimalSeparator(const String &separator)
{
    if (separator.isEmpty())
    {
        throw ArgumentException("separator");
    }

    _decimalSeparator = separator;
}

//! @brief Gets locale-agnostic formatting options.
const LocaleInfo &LocaleInfo::getNeutral()
{
    static FormatInfo neutralOptions;

    return neutralOptions;
}

//! @brief Gets current-locale-specific formatting options.
const LocaleInfo &LocaleInfo::getDisplay()
{
    static LocaleInfo displayOptions;
    static bool isInitialised = false;

    if (isInitialised == false)
    {
#ifdef _WIN32
        wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

        if (::GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) > 0)
        {
            std::wstring textValue;
            //uint32_t numericValue;

            //if (tryGetLocaleInfo(localeName, LOCALE_IDIGITS, numericValue))
            //{
            //    displayOptions.setPrecision(numericValue);
            //}

            if (tryGetLocaleInfo(localeName, LOCALE_SDECIMAL, textValue))
            {
                displayOptions.setDecimalSeparator(String(textValue));
            }

            if (tryGetLocaleInfo(localeName, LOCALE_STHOUSAND, textValue))
            {
                displayOptions.setThousandSeparator(String(textValue));
            }
        }
#else
        // Save the current locale.
        String oldLocale(setlocale(LC_NUMERIC, nullptr));

        // Set the process to the "user default" locale.
        setlocale(LC_ALL, "");

        // Interrogate values.
        char *dp = nl_langinfo(DECIMAL_POINT);
        displayOptions.setDecimalSeparator(String(dp, 1));

        char *sep = nl_langinfo(THOUSANDS_SEP);
        displayOptions.setThousandSeparator(String(sep, 1));

        // Restore the original locale.
        setlocale(LC_NUMERIC, oldLocale.getUtf8Bytes());
#endif
    }

    return displayOptions;
}

////////////////////////////////////////////////////////////////////////////////
// FormatInfo Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a neutral default set of formatting options.
FormatInfo::FormatInfo() :
    _minimumFieldWidth(0),
    _minimumWholeDigits(0),
    _requiredSigFigures(-1),
    _requiredFractonDigits(-1),
    _flags(Flag_UseUpperCase),
    _radix(10)
{
}

//! @brief Construct formatting options based on a specified locale.
//! @param[in] locale The locale information to copy to the new object.
FormatInfo::FormatInfo(const LocaleInfo &locale) :
    LocaleInfo(locale),
    _minimumFieldWidth(0),
    _minimumWholeDigits(0),
    _requiredSigFigures(-1),
    _requiredFractonDigits(-1),
    _flags(Flag_UseUpperCase),
    _radix(10)
{
}

//! @brief Gets the minimum field width of a formatted value.
uint16_t FormatInfo::getMinimumFieldWidth() const
{
    return _minimumFieldWidth;
}

//! @brief Sets the minimum field with of a formatted value.
//! @param[in] fieldWidth The new minimum field width value.
void FormatInfo::setMinimumFieldWidth(uint16_t fieldWidth)
{
    _minimumFieldWidth = fieldWidth;
}

//! @brief Gets the minimum number of whole number digits in a formatted value.
uint16_t FormatInfo::getMinimumWholeDigits() const
{
    return _minimumWholeDigits;
}

//! @brief Sets the minimum number of whole number digits in a formatted value.
//! @param[in] digitCount The new minimum digit count.
void FormatInfo::setMinimumWholeDigits(uint16_t digitCount)
{
    _minimumWholeDigits = digitCount;
}

//! @brief Gets the required number of significant figures which should appear
//! in a formatted value. A value of zero or less indicates no requirement.
int16_t FormatInfo::getRequiredSignificantFigures() const
{
    return _requiredSigFigures;
}

//! @brief Sets the required number of significant figures which should appear
//! in a formatted value.
//! @param[in] sigFigs The required number of significant figures. A value of
//! zero or less indicates no requirement.
void FormatInfo::setRequiredSignificantFigures(int16_t sigFigs)
{
    _requiredSigFigures = sigFigs;
}

//! @brief Gets the required number of fractional digits for a real number
//! value. A negative value implies the digit count should vary according to
//! the value being formatted.
int16_t FormatInfo::getRequiredFractionDigits() const
{
    return _requiredFractonDigits;
}

//! @brief Sets the number of maximum number of fractional digits or minimum number
//! of whole number digits for an integer.
//! @param[in] precision The new precision value.
void FormatInfo::setRequiredFractionDigits(int16_t precision)
{
    _requiredFractonDigits = precision;
}

//! @brief Gets the default radix used for the conversion.
uint8_t FormatInfo::getRadix() const { return _radix; }

//! @brief Sets the default radix used for the conversion.
//! @param[in] radix The new radix value. Only values between 2 and 36
//! are supported.
void FormatInfo::setRadix(uint8_t radix)
{
    if ((radix < 2) || (radix > 36))
    {
        throw ArgumentException("radix");
    }

    _radix = radix;
}


//! @brief Gets whether the formatted value should be right aligned within
//! the field.
//! @retval true The value should be right aligned.
//! @retval false The value should be left aligned.
bool FormatInfo::isRightAligned() const { return (_flags & Flag_RightAligned) != 0; }

//! @brief Sets whether the formatted value should be right aligned within
//! the field.
//! @param[in] isEnabled true to right align the value, false to left align it.
void FormatInfo::enableRightAlign(bool isEnabled)
{
    setOption(Flag_RightAligned, isEnabled);
}

//! @brief Determines whether a sign should appear in the formatted value,
//! even if it is positive.
//! @retval true A positive value will receive a leading plus.
//! @retval false A positive value will have no prefix.
bool FormatInfo::isSignForced() const { return (_flags & Flag_ForceSign) != 0; }

//! @brief Sets whether a sign should appear in the formatted value,
//! even if it is positive.
//! @param[in] isEnabled True if positive value will receive a leading plus.
//! False to format positive values with no prefix.
void FormatInfo::enableForcedSign(bool isEnabled)
{
    setOption(Flag_ForceSign, isEnabled);
}

//! @brief Gets whether an exponent should appear with a sign, even if positive.
//! @retval true The sign will always appear.
//! @retval false The sign will only appear if negative.
bool FormatInfo::isExponentSignForced() const
{
    return getOption(Flag_ForceExpSign);
}

//! @brief Sets whether an exponent should appear with a sign, even if positive.
//! @param[in] isEnabled True if the sign should always appear, false if the
//! sign should only appear if negative.
void FormatInfo::enableForcedExponentSign(bool isEnabled)
{
    setOption(Flag_ForceExpSign, isEnabled);
}

//! @brief Gets whether upper or lower case letters should be used when numbers
//! with a radix above 10 are formatted.
//! @retval true Upper case letters will be used.
//! @retval false Lower case letters will be used.
bool FormatInfo::isUpperCase() const
{
    return (_flags & Flag_UseUpperCase) != 0;
}

//! @brief Sets whether upper or lower case letters should be used when numbers
//! with a radix above 10 are formatted.
//! @param[in] isEnabled True for upper case letters to be used, false for
//! lower case letters to be used.
void FormatInfo::enableUpperCase(bool isEnabled)
{
    setOption(Flag_UseUpperCase, isEnabled);
}

//! @brief Determines if a specific flag was set.
//! @param[in] flag The flag or pattern of flags which must be set.
//! @retval true All flags specified were set.
//! @retval false Only some or none of the flags specified were set.
bool FormatInfo::getOption(uint32_t flag) const
{
    return (_flags & flag) == flag;
}

//! @brief Sets a specific option flag based on a boolean parameter.
//! @param[in] flag The flag(s) to set or clear.
//! @param[in] enabled True to set the flag(s), false to clear them.
void FormatInfo::setOption(uint32_t flag, bool enabled)
{
    if (enabled)
    {
        // Set flags.
        _flags |= flag;
    }
    else
    {
        // Clear flags.
        _flags &= ~flag;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Parses a format token in order to adapt the parameters used to
//! render a value as text.
char adaptFormat(FormatInfo &options, utf8_cptr_t boundedFormat,
                 size_t formatLength)
{
    char typeCode = '\0';

    if (formatLength > 0)
    {
        bool getMinFract = false;
        bool getMinDigits = false;
        size_t index = 1;

        switch(*boundedFormat)
        {
        case 'D':
        case 'd':
        case 'I':
        case 'i': // Signed integer types.
            options.setRadix(10);
            getMinDigits = true;
            typeCode = 'I';
            break;

        case 'u':
        case 'U': // Unsigned integer types.
            options.setRadix(10);
            getMinDigits = true;
            typeCode = 'U';
            break;

        case 'A':
        case 'a': // Address
        case 'P':
        case 'p': // Pointer
            options.setRadix(16);
            options.setMinimumWholeDigits(static_cast<uint16_t>(sizeof(void *) * 2));
            typeCode = 'U';
            break;

        case 'F':
        case 'f': // Real (fixed fractional precision)
            options.setRadix(10);
            getMinFract = true;
            getMinDigits = true;
            typeCode = 'F';
            break;

        case 'E':
        case 'e': // Real (fixed significant figure count, possible exponent)
            options.setRadix(10);
            getMinDigits = true;
            typeCode = 'E';
            break;

        case 'G':
        case 'g': // Real (most compact form)
            options.setRadix(10);
            typeCode = 'G';
            break;

        case 'c':
        case 'C': // Single character
            typeCode = 'C';
            break;

        case 's':
        case 'S': // String value
            typeCode = 'S';
            getMinDigits = true;
            break;

        case 'x':
        case 'X': // Hexadecimal
            options.setRadix(16);
            options.enableUpperCase((*boundedFormat) == 'u');
            typeCode = 'U';
            break;

        case 'o':
        case 'O': // Octal?
            options.setRadix(8);
            options.enableUpperCase((*boundedFormat) == 'u');
            typeCode = 'U';
            break;
        }

        if ((getMinDigits || getMinFract) &&
            (formatLength > 2) &&
            (boundedFormat[index] == ':'))
        {
            // Move past the colon.
            ++index;

            // Try to consume numeric digits.
            uint16_t value = 0;
            bool hasValue = false;

            while (index < formatLength)
            {
                char next = boundedFormat[index];

                if ((next > '0') && (next <= '9'))
                {
                    // Apply the digit to the accumulated value.
                    value = (value * 10) + static_cast<uint16_t>(next - '0');
                    ++index;
                    hasValue = true;
                }
                else
                {
                    // Exit the loop.
                    index = formatLength;
                }
            }

            if (hasValue)
            {
                if (getMinDigits)
                {
                    options.setMinimumWholeDigits(value);
                }
                else if (getMinFract)
                {
                    options.setRequiredFractionDigits(static_cast<int16_t>(value));
                }
            }
        }
    }

    return typeCode;
}

//! @brief Appends a signed 8-bit integer value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, int8_t value)
{
    appendIntegralValue(options, buffer, value);
}

//! @brief Appends an unsigned 8-bit integer value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, uint8_t value)
{
    appendIntegralValue(options, buffer, value);
}

//! @brief Appends a signed 16-bit integer value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, int16_t value)
{
    appendIntegralValue(options, buffer, value);
}

//! @brief Appends an unsigned 16-bit integer value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, uint16_t value)
{
    appendIntegralValue(options, buffer, value);
}

//! @brief Appends a signed 32-bit integer value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, int32_t value)
{
    appendIntegralValue(options, buffer, value);
}

//! @brief Appends an unsigned 32-bit integer value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, uint32_t value)
{
    appendIntegralValue(options, buffer, value);
}

//! @brief Appends a signed 64-bit integer value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, int64_t value)
{
    appendIntegralValue(options, buffer, value);
}

//! @brief Appends an unsigned 64-bit integer value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, uint64_t value)
{
    appendIntegralValue(options, buffer, value);
}

//! @brief Appends an real value to a UTF-8 encoded STL string.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The value to format.
void appendValue(const FormatInfo &options, std::string &buffer, double value)
{
    NumericCharacters characters;
    bool isZero;

    if (handleUnreal(characters, value, isZero) == false)
    {
        // Format as string set in whole number digits.
        characters.formatString(options, buffer);
    }
    else if (isZero)
    {
        // No need for a conversion, the value is zero.
        characters = NumericCharacters('+', "0");

        // Render the value as text.
        characters.formatValue(options, buffer);
    }
    else
    {
        // Create a temporary buffer big enough to hold the maximum number of digits
        // given the worst case.
        char digitBuffer[_CVTBUFSIZE];
        int decPtIndex = 0, sign = 0, errorCode = 0;
        digitBuffer[0] = '\0';

        if (options.getRequiredSignificantFigures() > 0)
        {
            errorCode = realToSignificantDigits(value, options.getRequiredSignificantFigures(),
                                                digitBuffer, std::size(digitBuffer),
                                                &decPtIndex, &sign);

            characters = NumericCharacters((sign == 0) ? '+' : '-',
                                           decPtIndex, digitBuffer);
        }
        else if (options.getRequiredFractionDigits() > 0)
        {
            errorCode = realToFractionDigits(value, options.getRequiredFractionDigits(),
                                             digitBuffer, std::size(digitBuffer),
                                             &decPtIndex, &sign);

            characters = NumericCharacters((sign == 0) ? '+' : '-',
                                           decPtIndex, digitBuffer);
        }
        else
        {
            // Format the value using the maximum number of significant digits
            // supported by the data type.
            errorCode = realToSignificantDigits(value,
                                                std::numeric_limits<double>::max_digits10,
                                                digitBuffer, std::size(digitBuffer),
                                                &decPtIndex, &sign);

            if (errorCode == 0)
            {
                // Calculate how many significant digits there really are.
                size_t sigFigs = 0;

                for (size_t index = 0; digitBuffer[index]; ++index)
                {
                    if (digitBuffer[index] != '0')
                    {
                        sigFigs = index + 1;
                    }
                }

                // Calculate the perspective length of the value in standard form.
                double roundedExponent = std::trunc(std::log10(value));
                int exponentDigitCount = static_cast<int>(std::ceil(std::log10(std::abs(roundedExponent))));

                if ((roundedExponent < 0) || options.isExponentSignForced())
                {
                    // Add one for the sign.
                    ++exponentDigitCount;
                }

                // Add one for the E/e.
                size_t expLength = static_cast<size_t>(exponentDigitCount) + 1;

                // Calculate the length of the value in standard form allowing
                // for the decimal point if necessary.
                size_t stdFormLength = ((sigFigs > 1) ? (sigFigs + 1) : sigFigs) + expLength;
                size_t normFormLength = 0;

                if (decPtIndex <= 0)
                {
                    // The value is fractional (magnitude < 1)

                    // Calculate the length in long form (add 2 for leading "0.").
                    normFormLength = toSize(-decPtIndex) + 2;
                }
                else if (sigFigs < toSize(decPtIndex))
                {
                    // The value is a whole number with trailing zeros.
                    normFormLength = toSize(decPtIndex);
                }
                else
                {
                    // The value has zero or more fraction digits.
                    normFormLength = sigFigs + 1;
                }

                if (normFormLength <= stdFormLength)
                {
                    // Format as normal, but cut out all of the
                    // unnecessary trailing zeros.
                    if (toSize(decPtIndex) >= sigFigs)
                    {
                        digitBuffer[decPtIndex] = '\0';
                    }

                    characters = NumericCharacters((sign == 0) ? '+' : '-',
                                                   decPtIndex, digitBuffer);
                }
                else if (decPtIndex > 0)
                {
                    size_t wholeLength = 1;
                    char *fractDigits = digitBuffer + wholeLength;
                    size_t fractLength = sigFigs - 1;

                    // Calculate the exponent digits and add them to the
                    // buffer after the fraction digits.
                    size_t expMaxLen = std::size(digitBuffer) - sigFigs;
                    char *expDigits = digitBuffer + sigFigs;
                    char expSign;

                    // There should only be a single whole number digit in
                    // standard form.
                    decPtIndex = 1;

                    expLength = appendDigits(expDigits, expSign, expMaxLen, 10,
                                             static_cast<int>(roundedExponent));

                    characters = NumericCharacters((sign == 0) ? '+' : '-', digitBuffer,
                                                   wholeLength, fractDigits, fractLength,
                                                   expSign, expDigits, expLength);
                }
                else
                {
                    // The value is fractional.
                    // Calculate the exponent digits and add them to the
                    // buffer after the fraction digits.
                    digitBuffer[sigFigs] = '\0';
                    size_t expMaxLen = std::size(digitBuffer) - sigFigs - 1;
                    char *expDigits = digitBuffer + sigFigs + 1;
                    char expSign;

                    // There should only be a single whole number digit in
                    // standard form.
                    decPtIndex = 1;

                    expLength = appendDigits(expDigits, expSign, expMaxLen, 10,
                                             static_cast<int>(roundedExponent));

                    characters = NumericCharacters((sign == 0) ? '+' : '-',
                                                   decPtIndex, digitBuffer,
                                                   expSign, expDigits);
                }
            }
        }

        if (errorCode == 0)
        {
            // Render the value as text.
            characters.formatValue(options, buffer);
        }
        else
        {
            characters = NumericCharacters('+', "(Err)");

            // Format as string set in whole number digits.
            characters.formatString(options, buffer);
        }
    }
}

//! @brief Appends an unsigned 64-bit integer representing a file size
//! value to a UTF-8 encoded STL string. The value will be automatically
//! scaled and units applied.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The file size to format.
void appendFileSize(const FormatInfo &options, std::string &buffer, uint64_t value)
{
    double scaledSize;
    uint8_t magnitude = fileSizeToReal(options, value, scaledSize);

    // Format the size as a real value.
    FormatInfo valueOptions = options;

    if (magnitude == 0)
    {
        // Force no decimal digits when the value is in bytes.
        valueOptions.setRequiredFractionDigits(0);
    }
    else if (valueOptions.getRequiredFractionDigits() < 0)
    {
        // Default to no decimal places.
        valueOptions.setRequiredFractionDigits(0);
    }

    // Prevent zero padding of whole digits.
    valueOptions.setMinimumWholeDigits(1);

    appendValue(valueOptions, buffer, scaledSize);
    buffer.push_back(' ');
    appendFileSizeUnit(buffer, magnitude, value == 1);
}

//! @brief Appends a signed real value representing a file size
//! value to a UTF-8 encoded STL string. The value will be automatically
//! scaled and units applied.
//! @param[in] options The options used to determine how the value is formatted.
//! @param[out] buffer The buffer to append the value to.
//! @param[in] value The file size to format.
void appendRealFileSize(const FormatInfo &options, std::string &buffer, double value)
{
    double scaledSize;
    uint8_t magnitude = fileSizeToReal(options, value, scaledSize);

    // Format the size as a real value.
    FormatInfo valueOptions = options;

    if (magnitude == 0)
    {
        // Force no decimal digits when the value is in bytes.
        valueOptions.setRequiredFractionDigits(0);
    }
    else if (valueOptions.getRequiredFractionDigits() < 0)
    {
        // Default to no decimal places.
        valueOptions.setRequiredFractionDigits(0);
    }

    // Prevent zero padding of whole digits.
    valueOptions.setMinimumWholeDigits(1);

    appendValue(valueOptions, buffer, scaledSize);
    buffer.push_back(' ');
    appendFileSizeUnit(buffer, magnitude, std::abs(value) == 1.0);
}

//! @brief Appends formatted values to an STL string using the default display settings.
//! @param[in] spec The format specification used as a template for the text
//! to generate.
//! @param[out] buffer An STL string to receive the generated text.
//! @param[in] params A list of parameters to be inserted into the generated text.
void appendFormat(const std::string_view &spec, std::string &buffer,
                  const std::initializer_list<Variant> &params)
{
    FormatInfo display(LocaleInfo::getDisplay());

    appendFormat(display, spec, buffer, params);
}

//! @brief Appends formatted values to an STL string.
//! @param[in] options The options used to format values.
//! @param[in] spec The format specification used as a template for the text
//! to generate.
//! @param[out] buffer An STL string to receive the generated text.
//! @param[in] params A list of parameters to be inserted into the generated text.
void appendFormat(const FormatInfo &options, const std::string_view &spec,
                  std::string &buffer, const std::initializer_list<Variant> &params)
{
    // Create a random-access index of parameter values.
    std::vector<const Variant *> paramIndex;
    paramIndex.reserve(params.size());
    for (const Variant &param : params)
    {
        paramIndex.push_back(&param);
    }

    size_t index = 0;

    while (index < spec.length())
    {
        char next = spec[index];

        if (next == '{')
        {
            ++index;

            if (index < spec.length())
            {
                next = spec[index];

                if ((next >= '0') && (next <= '9'))
                {
                    // It's a value insertion token.
                    size_t offset = index;

                    // FormatInfo valueOptions(options);
                    InsertionToken token;

                    if (tryParseInsertionToken(spec, offset, token))
                    {
                        if (token.ValueIndex < params.size())
                        {
                            // Format the value into the target string.
                            formatValue(buffer, token, options,
                                        *paramIndex[token.ValueIndex]);

                            // Move past the token.
                            index = offset;
                        }
                        else
                        {
                            throw FormatException(params.size(), token.ValueIndex);
                        }
                    }
                    else
                    {
                        // Format error: Value index out of range.
                        throw FormatException(spec.substr(index, offset - index));
                    }
                }
                else if (next == '{')
                {
                    // It's an escaped open brace '{'.
                    buffer.push_back('{');
                    ++index;
                }
            }
            else
            {
                // It's an open brace at the end of the string.
                buffer.push_back('{');
            }
        }
        else
        {
            buffer.push_back(next);
            ++index;
        }
    }
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


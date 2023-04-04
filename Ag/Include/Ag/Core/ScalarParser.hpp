//! @file Ag/Core/ScalarParser.hpp
//! @brief The declaration of an object which parses scalar values from text.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_SCALAR_PARSER_HPP__
#define __AG_CORE_SCALAR_PARSER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <limits>
#include <vector>

#include "Format.hpp"
#include "String.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which parses scalar values from text.
class ScalarParser
{
public:
    // Constructors/Destructors
    ScalarParser();
    ScalarParser(const LocaleInfo &locale);

    // Accessors
    bool allowRadixPrefix() const;
    void enableRadixPrefix(bool isEnabled);
    int getPreferredRadix() const;
    void setPreferredRadix(int radix);
    bool allowSign() const;
    void enableSign(bool isEnabled);
    bool allowFraction() const;
    void enableFraction(bool isEnabled);
    bool allowExponent() const;
    void enableExponent(bool isEnabled);

    // Operations
    void reset();
    bool tryProcessCharacter(char character);
    bool tryProcessString(const std::string_view &str);

    //! @brief Attempts to extract a value from the characters parsed.
    //! @tparam T The scalar data type of the value to extract.
    //! @param[out] value Receives the value created from the characters.
    //! @retval true A value was successfully extracted.
    //! @retval false The object had not processed sufficient characters to
    //! create a value from, the characters did not represent a valid value
    //! or the value they did represent could not be represented as the
    //! data type required.
    template<typename T> bool tryGetValue(T &value) const
    {
        bool isOK = false;

        if (isTerminalState(_currentState))
        {
            // The parsing process had reached a point where it was valid
            // to stop.
            using TypeInfo = std::numeric_limits<T>;

            if constexpr (TypeInfo::is_signed == false)
            {
                if (_sign < 0)
                {
                    // An unsigned value proceeded by a minus sign.
                    return false;
                }
            }

            isOK = true;
            value = static_cast<T>(0);
            T multiplier = static_cast<T>(_radix);

            if constexpr (TypeInfo::is_signed)
            {
                T signAdjust = static_cast<T>(1);

                if (_sign < 0)
                {
                    // We have to negate each digit because the maximum
                    // negative value can have a higher magnitude than the
                    // maximum positive number (127 vs -128).
                    signAdjust = static_cast<T>(-1);
                }

                for (uint8_t digit : _wholeDigits)
                {
                    // TODO: Check for overflow.
                    T digitValue = static_cast<T>(digit);

                    // We have to negate each digit because the maximum
                    // negative value can have a higher magnitude than the
                    // maximum positive number (127 vs -128).
                    digitValue *= signAdjust;

                    value = (value * multiplier) + digitValue;
                }
            }
            else
            {
                for (uint8_t digit : _wholeDigits)
                {
                    // TODO: Check for overflow.
                    T digitValue = static_cast<T>(digit);

                    value = (value * multiplier) + digitValue;
                }
            }

            if constexpr (TypeInfo::is_exact == false)
            {
                // It's a floating point type, apply the fractional component.
                if (_sign < 0)
                {
                    multiplier = static_cast<T>(-0.1);
                }
                else
                {
                    multiplier = static_cast<T>(0.1);
                }

                for (uint8_t digit : _fractionalDigits)
                {
                    T oldValue = value;

                    value += multiplier * static_cast<T>(digit);

                    if (oldValue == value)
                    {
                        // There was no change, we have reached underflow.
                        // TODO: Error, or just ignore the rest of the fraction?
                        break;
                    }
                }
            }

            if (_exponentDigits.empty() == false)
            {
                int exponent = 0;

                for (uint8_t digit : _exponentDigits)
                {
                    exponent = (exponent * 10) + static_cast<int>(digit);
                }

                exponent *= _exponentSign;

                value = pow(value, exponent);
            }
        }

        return isOK;
    }
private:
    // Internal Types
    typedef bool (ScalarParser:: *HandlerFnPtr)(char next);
    // Format:
    // { White Space }
    // { Sign }
    // { Radix Prefix }
    // { Whole Digits/Thousand Separators }
    // { Decimal Point }
    // { Fraction Digits }
    // { Exponent Symbol }
    // { Exponent Sign }
    // { Exponent Digits }
    // { Trailing White Space }

    //! @brief Expresses the states the parser can exist in.
    enum State
    {
        State_Start,
        State_BeforePrefix,
        State_BeforeWholeDigits,
        State_BeforeWholeHexDigits,
        State_AfterZeroPrefix,
        State_InWholeDigits,
        State_ExpectingWholeDigit,
        State_InThousandSeparator,
        State_InDecimalPoint,
        State_BeforeFraction,
        State_InFraction,
        State_AfterExponentSymbol,
        State_BeforeExponentDigits,
        State_InExponent,
        State_InTrailingWhiteSpace,

        State_Max,
    };

    enum Flags
    {
        Flag_AllowRadixPrefix = 0x01,
        Flag_AllowSign = 0x02,
        Flag_AllowFraction = 0x04,
        Flag_AllowExponent = 0x08
    };

    typedef std::vector<uint8_t> DigitVector;

    // Internal Functions
    bool hasFlag(uint32_t flag) const;
    void setFlag(uint32_t flag, bool isSet);
    bool handleStart(char next);
    bool handleBeforePrefix(char next);
    bool handleBeforeWholeDigits(char next);
    bool handleBeforeWholeHexDigits(char next);
    bool handleAfterZeroPrefix(char next);
    bool handleInWholeDigits(char next);
    bool handleExpectingWholeDigit(char next);
    bool handleInThousandSeparator(char next);
    bool handleInDecimalPoint(char next);
    bool handleBeforeFraction(char next);
    bool handleInFraction(char next);
    bool handleAfterExponentSymbol(char next);
    bool handleBeforeExponentDigits(char next);
    bool handleInExponent(char next);
    bool handleInTrailingWhiteSpace(char next);
    static bool isTerminalState(State state);
    static int8_t pow(int8_t value, int power);
    static uint8_t pow(uint8_t value, int power);
    static int16_t pow(int16_t value, int power);
    static uint16_t pow(uint16_t value, int power);
    static int32_t pow(int32_t value, int power);
    static uint32_t pow(uint32_t value, int power);
    static int64_t pow(int64_t value, int power);
    static uint64_t pow(uint64_t value, int power);
    static float pow(float value, int power);
    static double pow(double value, int power);

    // Internal Fields
    State _currentState;
    size_t _stateIndex;
    int _preferredRadix;
    int _radix;
    int _sign;
    int _exponentSign;
    uint32_t _flags;
    String _thousandSeparator;
    String _decimalPoint;
    DigitVector _wholeDigits;
    DigitVector _fractionalDigits;
    DigitVector _exponentDigits;
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

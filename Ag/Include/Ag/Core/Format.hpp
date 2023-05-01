//! @file Ag/Core/Format.hpp
//! @brief The declaration of objects and functions which format values as text.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_FORMAT_HPP__
#define __AG_CORE_FORMAT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>

#include "String.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class Variant;

//! @brief Provides locale-specific information required to parse values rendered
//! as text.
class LocaleInfo
{
public:
    // Construction/Destruction
    LocaleInfo();

    // Accessors
    const String &getThousandSeparator() const;
    void setThousandSeparator(const String &separator);
    const String &getDecimalSeparator() const;
    void setDecimalSeparator(const String &separator);

    static const LocaleInfo &getNeutral();
    static const LocaleInfo &getDisplay();
private:
    // Internal Fields
    String _thousandSeparator;
    String _decimalSeparator;
};

//! @brief An object which defines how values should be rendered in text.
class FormatInfo : public LocaleInfo
{
public:
    // Construction
    FormatInfo();
    FormatInfo(const LocaleInfo &locale);

    // Accessors
    uint16_t getMinimumFieldWidth() const;
    void setMinimumFieldWidth(uint16_t fieldWidth);
    uint16_t getMinimumWholeDigits() const;
    void setMinimumWholeDigits(uint16_t digitCount);
    int16_t getRequiredSignificantFigures() const;
    void setRequiredSignificantFigures(int16_t sigFigs);
    int16_t getRequiredFractionDigits() const;
    void setRequiredFractionDigits(int16_t digitCount);
    uint8_t getRadix() const;
    void setRadix(uint8_t radix);
    bool isRightAligned() const;
    void enableRightAlign(bool isEnabled);
    bool isSignForced() const;
    void enableForcedSign(bool isEnabled);
    bool isExponentSignForced() const;
    void enableForcedExponentSign(bool isEnabled);
    bool isUpperCase() const;
    void enableUpperCase(bool isEnabled);

private:
    // Internal Types
    enum FlagBits
    {
        Flag_RightAligned = 0x01,
        Flag_ForceSign = 0x02,
        Flag_ForceExpSign = 0x04,
        Flag_UseUpperCase = 0x08,
    };

    bool getOption(uint32_t flag) const;
    void setOption(uint32_t flag, bool enabled);

    // Internal Fields
    uint16_t _minimumFieldWidth;
    uint16_t _minimumWholeDigits;
    int16_t _requiredSigFigures;
    int16_t _requiredFractonDigits;
    uint8_t _flags;
    uint8_t _radix;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
char adaptFormat(FormatInfo &options, utf8_cptr_t boundedFormat,
                 size_t formatLength);
void appendValue(const FormatInfo &options, std::string &buffer, int8_t value);
void appendValue(const FormatInfo &options, std::string &buffer, uint8_t value);
void appendValue(const FormatInfo &options, std::string &buffer, int16_t value);
void appendValue(const FormatInfo &options, std::string &buffer, uint16_t value);
void appendValue(const FormatInfo &options, std::string &buffer, int32_t value);
void appendValue(const FormatInfo &options, std::string &buffer, uint32_t value);
void appendValue(const FormatInfo &options, std::string &buffer, int64_t value);
void appendValue(const FormatInfo &options, std::string &buffer, uint64_t value);
void appendValue(const FormatInfo &options, std::string &buffer, double value);
void appendFormat(const std::string_view &spec, std::string &buffer,
                  const std::initializer_list<Variant> &params);
void appendFormat(const FormatInfo &options, const std::string_view &spec,
                  std::string &buffer, const std::initializer_list<Variant> &params);

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

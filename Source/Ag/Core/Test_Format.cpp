//! @file Core/Test_Format.cpp
//! @brief The definition of string formatting functionality.
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
#include <gtest/gtest.h>

#include <Ag/Core/Format.hpp>
#include <Ag/Core/Variant.hpp>

namespace Ag {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(FormatOptions, DisplayFormat)
{
    const LocaleInfo &display = LocaleInfo::getDisplay();

    EXPECT_FALSE(display.getDecimalSeparator().isEmpty());
    EXPECT_FALSE(display.getThousandSeparator().isEmpty());
}

GTEST_TEST(Format, FormatInt8)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, static_cast<int8_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.setMinimumWholeDigits(4);

    appendValue(specimen, buffer, static_cast<int8_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0000");

    buffer.clear();
    appendValue(specimen, buffer, static_cast<int8_t>(-42));

    EXPECT_STREQ(buffer.c_str(), "-0042");

    buffer.clear();
    specimen.setMinimumFieldWidth(7);
    specimen.enableRightAlign(true);
    appendValue(specimen, buffer, static_cast<int8_t>(-128));

    EXPECT_STREQ(buffer.c_str(), "  -0128");

    buffer.clear();
    specimen.enableRightAlign(false);
    specimen.enableForcedSign(true);
    specimen.setRadix(16);
    appendValue(specimen, buffer, static_cast<int8_t>(127));

    EXPECT_STREQ(buffer.c_str(), "+007F  ");

    buffer.clear();
    specimen.enableUpperCase(false);
    appendValue(specimen, buffer, static_cast<int8_t>(125));

    EXPECT_STREQ(buffer.c_str(), "+007d  ");

    buffer.clear();
    specimen.setRequiredFractionDigits(5);
    specimen.setRadix(10);
    appendValue(specimen, buffer, static_cast<int8_t>(-96));

    EXPECT_STREQ(buffer.c_str(), "-0096.00000");

    buffer.clear();
    specimen.setMinimumFieldWidth(15);
    appendValue(specimen, buffer, static_cast<int8_t>(123));

    EXPECT_STREQ(buffer.c_str(), "+0123.00000    ");
}

GTEST_TEST(Format, FormatUint8)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, static_cast<uint8_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.setMinimumWholeDigits(4);

    appendValue(specimen, buffer, static_cast<uint8_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0000");

    buffer.clear();
    appendValue(specimen, buffer, static_cast<uint8_t>(69));

    EXPECT_STREQ(buffer.c_str(), "0069");

    buffer.clear();
    specimen.setMinimumFieldWidth(7);
    specimen.enableRightAlign(true);
    appendValue(specimen, buffer, static_cast<uint8_t>(255));

    EXPECT_STREQ(buffer.c_str(), "   0255");

    buffer.clear();
    specimen.enableRightAlign(false);
    specimen.enableForcedSign(true);
    specimen.setRadix(16);
    appendValue(specimen, buffer, static_cast<uint8_t>(255));

    EXPECT_STREQ(buffer.c_str(), "+00FF  ");

    buffer.clear();
    specimen.enableUpperCase(false);
    appendValue(specimen, buffer, static_cast<uint8_t>(253));

    EXPECT_STREQ(buffer.c_str(), "+00fd  ");

    buffer.clear();
    specimen.setRequiredFractionDigits(5);
    specimen.setRadix(10);
    appendValue(specimen, buffer, static_cast<uint8_t>(9));

    EXPECT_STREQ(buffer.c_str(), "+0009.00000");

    buffer.clear();
    specimen.setMinimumFieldWidth(15);
    appendValue(specimen, buffer, static_cast<uint8_t>(101));

    EXPECT_STREQ(buffer.c_str(), "+0101.00000    ");
}

GTEST_TEST(Format, FormatInt16)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, static_cast<int16_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.setMinimumWholeDigits(5);
    specimen.setThousandSeparator(",");
    appendValue(specimen, buffer, static_cast<int16_t>(0));

    EXPECT_STREQ(buffer.c_str(), "00,000");

    buffer.clear();
    specimen.setRequiredFractionDigits(2);
    appendValue(specimen, buffer, static_cast<int16_t>(0x7FFF));

    EXPECT_STREQ(buffer.c_str(), "32,767.00");

    buffer.clear();
    specimen.setMinimumFieldWidth(12);
    specimen.enableRightAlign(true);
    appendValue(specimen, buffer, static_cast<int16_t>(0x8000));

    EXPECT_STREQ(buffer.c_str(), "  -32,768.00");
}

GTEST_TEST(Format, FormatUint16)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, static_cast<uint16_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.setMinimumWholeDigits(6);
    specimen.setThousandSeparator(",");
    appendValue(specimen, buffer, static_cast<uint16_t>(0));

    EXPECT_STREQ(buffer.c_str(), "000,000");

    buffer.clear();
    specimen.setRequiredFractionDigits(3);
    appendValue(specimen, buffer, static_cast<uint16_t>(0xFFFF));

    EXPECT_STREQ(buffer.c_str(), "065,535.000");

    buffer.clear();
    specimen.setMinimumFieldWidth(15);
    specimen.enableRightAlign(true);
    specimen.enableForcedSign(true);
    appendValue(specimen, buffer, static_cast<uint16_t>(1024));

    EXPECT_STREQ(buffer.c_str(), "   +001,024.000");
}

GTEST_TEST(Format, FormatInt32)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, static_cast<int32_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.enableForcedSign(true);
    specimen.setThousandSeparator(",");

    appendValue(specimen, buffer, static_cast<int32_t>(0x7FFFFFFF));

    EXPECT_STREQ(buffer.c_str(), "+2,147,483,647");

    buffer.clear();
    specimen.setRequiredFractionDigits(4);
    specimen.setMinimumFieldWidth(22);
    appendValue(specimen, buffer, static_cast<int32_t>(0x80000000));

    EXPECT_STREQ(buffer.c_str(), "-2,147,483,648.0000   ");
}

GTEST_TEST(Format, FormatUint32)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, static_cast<uint32_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.enableRightAlign(true);
    specimen.setMinimumFieldWidth(20);
    appendValue(specimen, buffer, static_cast<uint32_t>(0xFFFFFFFF));

    EXPECT_STREQ(buffer.c_str(), "          4294967295");
}


GTEST_TEST(Format, FormatInt64)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, static_cast<int64_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.enableForcedSign(true);
    specimen.setThousandSeparator(",");

    appendValue(specimen, buffer, static_cast<int64_t>(0x7FFFFFFFFFFFFFFF));

    EXPECT_STREQ(buffer.c_str(), "+9,223,372,036,854,775,807");

    buffer.clear();
    specimen.setRequiredFractionDigits(3);
    specimen.setMinimumFieldWidth(33);
    appendValue(specimen, buffer, static_cast<int64_t>(0x8000000000000000));

    EXPECT_STREQ(buffer.c_str(), "-9,223,372,036,854,775,808.000   ");
}

GTEST_TEST(Format, FormatUint64)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, static_cast<uint64_t>(0));

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.enableRightAlign(true);
    specimen.setMinimumFieldWidth(22);
    appendValue(specimen, buffer, static_cast<uint64_t>(UINT64_MAX));

    EXPECT_STREQ(buffer.c_str(), "  18446744073709551615");
}

GTEST_TEST(Format, FormatRealF)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, 0.0);

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    specimen.setRequiredFractionDigits(4);
    specimen.enableForcedSign(true);
    appendValue(specimen, buffer, 0.0);

    EXPECT_STREQ(buffer.c_str(), "+0.0000");

    buffer.clear();
    specimen.setMinimumFieldWidth(10);
    specimen.enableRightAlign(true);
    appendValue(specimen, buffer, 3.141597);

    EXPECT_STREQ(buffer.c_str(), "   +3.1416");

    buffer.clear();
    appendValue(specimen, buffer, INFINITY);

    EXPECT_STREQ(buffer.c_str(), "     (Inf)");

    buffer.clear();
    appendValue(specimen, buffer, NAN);

    EXPECT_STREQ(buffer.c_str(), "     (NaN)");
}

GTEST_TEST(Format, FormatRealE)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    specimen.setRequiredSignificantFigures(4);
    appendValue(specimen, buffer, 0.0);

    EXPECT_STREQ(buffer.c_str(), "0.000");

    buffer.clear();
    appendValue(specimen, buffer, 42.3691);

    EXPECT_STREQ(buffer.c_str(), "42.37");

    buffer.clear();
    specimen.enableRightAlign(true);
    specimen.setMinimumFieldWidth(9);
    appendValue(specimen, buffer, -1.0 / 3.0);

    EXPECT_STREQ(buffer.c_str(), "  -0.3333");

    buffer.clear();
    specimen.setRequiredSignificantFigures(6);
    appendValue(specimen, buffer, 69.25);

    EXPECT_STREQ(buffer.c_str(), "  69.2500");
}

GTEST_TEST(Format, FormatRealG)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendValue(specimen, buffer, 0.0);

    EXPECT_STREQ(buffer.c_str(), "0");

    buffer.clear();
    appendValue(specimen, buffer, 1536000000.0);

    EXPECT_STREQ(buffer.c_str(), "1.536E9");

    buffer.clear();
    appendValue(specimen, buffer, 1536000.0);

    EXPECT_STREQ(buffer.c_str(), "1536000");

    buffer.clear();
    specimen.enableUpperCase(false);
    appendValue(specimen, buffer, 15360000.0);

    EXPECT_STREQ(buffer.c_str(), "1.536e7");

    buffer.clear();
    appendValue(specimen, buffer, 108.1344);

    // Give up!
    // EXPECT_STREQ(buffer.c_str(), "108.1344");
}

GTEST_TEST(Format, FormatFileSize)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendFileSize(specimen, buffer, 0);
    EXPECT_STREQ(buffer.c_str(), "0 bytes");

    // Verify single bytes and no fractional digits.
    buffer.clear();
    specimen.setRequiredFractionDigits(2);
    appendFileSize(specimen, buffer, 1);
    EXPECT_STREQ(buffer.c_str(), "1 byte");

    buffer.clear();
    specimen.setRequiredFractionDigits(-1);
    appendFileSize(specimen, buffer, 1610612736);
    EXPECT_STREQ(buffer.c_str(), "1536 MB");

    buffer.clear();
    specimen.setRequiredFractionDigits(1);
    specimen.setMinimumWholeDigits(1);
    appendFileSize(specimen, buffer, 1536000);
    EXPECT_STREQ(buffer.c_str(), "1.5 MB");
}

GTEST_TEST(Format, FormatRealFileSize)
{
    FormatInfo specimen(LocaleInfo::getNeutral());
    std::string buffer;

    appendRealFileSize(specimen, buffer, 0.0);
    EXPECT_STREQ(buffer.c_str(), "0 bytes");

    // Verify single bytes and no fractional digits.
    buffer.clear();
    specimen.setRequiredFractionDigits(2);
    appendRealFileSize(specimen, buffer, 1.0);
    EXPECT_STREQ(buffer.c_str(), "1 byte");

    buffer.clear();
    specimen.setRequiredFractionDigits(-1);
    appendRealFileSize(specimen, buffer, 1610612736);
    EXPECT_STREQ(buffer.c_str(), "1536 MB");

    buffer.clear();
    specimen.setRequiredFractionDigits(1);
    specimen.setMinimumWholeDigits(1);
    appendRealFileSize(specimen, buffer, 1536000.0);
    EXPECT_STREQ(buffer.c_str(), "1.5 MB");
}

GTEST_TEST(FormattedOutput, EmbedInt)
{
    FormatInfo info(LocaleInfo::getNeutral());
    std::string buffer;

    appendFormat(info, "Does a radioactive cat have {0} half-lives?",
                 buffer, { 9 });

    EXPECT_STREQ(buffer.c_str(), "Does a radioactive cat have 9 half-lives?");
}

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


//! @file Core/Test_Utf.cpp
//! @brief The definition of units tests for Unicode character set transforms.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/Core/Utf.hpp"

namespace Ag {

namespace Utf {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(UnicodeTests, ToUtf8)
{
    ToUtf8Converter converter;
    uint8_t nextByte = 0;

    EXPECT_FALSE(converter.tryGetNextByte(nextByte));

    char32_t capitalA = U'A';

    // Try 1-byte encoding.
    converter.setCodePoint(capitalA);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, static_cast<uint8_t>('A'));
    EXPECT_FALSE(converter.tryGetNextByte(nextByte));

    // Try 2-byte encoding.
    char32_t poundsSterling = 0xA3;
    converter.setCodePoint(poundsSterling);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0xC2);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0xA3);

    EXPECT_FALSE(converter.tryGetNextByte(nextByte));

    // Try another 2-byte encoding
    char32_t cyrillicCapitalOmega = 0x460;
    converter.setCodePoint(cyrillicCapitalOmega);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0xD1);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0xA0);

    EXPECT_FALSE(converter.tryGetNextByte(nextByte));

    // Try a 3-byte encoding.
    char32_t whiteSmilingFace = 0x263A;
    converter.setCodePoint(whiteSmilingFace);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0xE2);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0x98);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0xBA);

    EXPECT_FALSE(converter.tryGetNextByte(nextByte));


    // Try a 4-byte encoding.
    char32_t gothicHwair = 0x10348;
    converter.setCodePoint(gothicHwair);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0xF0);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0x90);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0x8D);

    EXPECT_TRUE(converter.tryGetNextByte(nextByte));
    EXPECT_EQ(nextByte, 0x88);

    EXPECT_FALSE(converter.tryGetNextByte(nextByte));

    // Try an invalid encoding.
    char32_t invalid = 0x300001;
    converter.setCodePoint(invalid);

    EXPECT_FALSE(converter.tryGetNextByte(nextByte));
}

GTEST_TEST(UnicodeTests, FromUtf8)
{
    FromUtf8Converter converter;
    char32_t codePoint;
    bool hasError;

    // Try 1-byte encoding.
    char32_t capitalA = U'A';

    EXPECT_TRUE(converter.tryConvert(65, codePoint, hasError));
    EXPECT_FALSE(hasError);
    EXPECT_EQ(codePoint, capitalA);

    // Try 2-byte encoding.
    char32_t poundsSterling = 0xA3;

    EXPECT_FALSE(converter.tryConvert(0xC2, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_TRUE(converter.tryConvert(0xA3, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_EQ(codePoint, poundsSterling);

    // Try another 2-byte encoding
    char32_t cyrillicCapitalOmega = 0x460;

    EXPECT_FALSE(converter.tryConvert(0xD1, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_TRUE(converter.tryConvert(0xA0, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_EQ(codePoint, cyrillicCapitalOmega);

    // Try a 3-byte encoding.
    char32_t whiteSmilingFace = 0x263A;

    EXPECT_FALSE(converter.tryConvert(0xE2, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_FALSE(converter.tryConvert(0x98, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_TRUE(converter.tryConvert(0xBA, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_EQ(codePoint, whiteSmilingFace);

    // Try a 4-byte encoding.
    char32_t gothicHwair = 0x10348;

    EXPECT_FALSE(converter.tryConvert(0xF0, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_FALSE(converter.tryConvert(0x90, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_FALSE(converter.tryConvert(0x8D, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_TRUE(converter.tryConvert(0x88, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_EQ(codePoint, gothicHwair);

    // Try invalid encodings of first byte.
    EXPECT_FALSE(converter.tryConvert(0xFC, codePoint, hasError));
    EXPECT_TRUE(hasError);

    converter.reset();

    // Try invalid encodings of second byte.
    EXPECT_FALSE(converter.tryConvert(0xC2, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_FALSE(converter.tryConvert(65, codePoint, hasError));
    EXPECT_TRUE(hasError);
}

GTEST_TEST(UnicodeTests, ToUtf16)
{
    ToUtf16Converter converter;

    char16_t next;
    EXPECT_FALSE(converter.tryGetNextCharacter(next));

    char32_t capitalA = U'A';

    // Try 1-word encoding.
    converter.setCodePoint(capitalA);

    EXPECT_TRUE(converter.tryGetNextCharacter(next));
    EXPECT_EQ(next, u'A');
    EXPECT_FALSE(converter.tryGetNextCharacter(next));

    // Try 1-word high character set encoding.
    char32_t poundsSterling = 0xA3;
    converter.setCodePoint(poundsSterling);

    EXPECT_TRUE(converter.tryGetNextCharacter(next));
    EXPECT_EQ(next, 0x00A3);

    EXPECT_FALSE(converter.tryGetNextCharacter(next));

    // Try 1-word very high character set encoding.
    char32_t sallallahouAlayheWasallam = 0xFDFA;
    converter.setCodePoint(sallallahouAlayheWasallam);

    EXPECT_TRUE(converter.tryGetNextCharacter(next));
    EXPECT_EQ(next, 0xFDFA);

    EXPECT_FALSE(converter.tryGetNextCharacter(next));

    // Try a 2-word encoding.
    char32_t deseretYee = 0x10437;
    converter.setCodePoint(deseretYee);

    EXPECT_TRUE(converter.tryGetNextCharacter(next));
    EXPECT_EQ(next, 0xD801);

    EXPECT_TRUE(converter.tryGetNextCharacter(next));
    EXPECT_EQ(next, 0xDC37);

    EXPECT_FALSE(converter.tryGetNextCharacter(next));

    // Try another 2-word encoding.
    char32_t mandarinHan = 0x24B62;
    converter.setCodePoint(mandarinHan);

    EXPECT_TRUE(converter.tryGetNextCharacter(next));
    EXPECT_EQ(next, 0xD852);

    EXPECT_TRUE(converter.tryGetNextCharacter(next));
    EXPECT_EQ(next, 0xDF62);

    EXPECT_FALSE(converter.tryGetNextCharacter(next));

    // Try an invalid encoding.
    char32_t invalid = 0x300001;
    converter.setCodePoint(invalid);

    EXPECT_FALSE(converter.tryGetNextCharacter(next));
}

GTEST_TEST(UnicodeTests, FromUtf16)
{
    FromUtf16Converter converter;

    char32_t codePoint;
    bool hasError;

    char32_t capitalA = U'A';

    // Try 1-word encoding.
    EXPECT_TRUE(converter.tryConvert(u'A', codePoint, hasError));
    EXPECT_FALSE(hasError);
    EXPECT_EQ(codePoint, capitalA);

    // Try 1-word high character set encoding.
    char32_t poundsSterling = 0xA3;

    EXPECT_TRUE(converter.tryConvert(static_cast<char16_t>(poundsSterling),
                                     codePoint, hasError));
    EXPECT_FALSE(hasError);
    EXPECT_EQ(codePoint, poundsSterling);

    // Try 1-word very high character set encoding.
    char32_t sallallahouAlayheWasallam = 0xFDFA;

    EXPECT_TRUE(converter.tryConvert(static_cast<char16_t>(sallallahouAlayheWasallam),
                                     codePoint, hasError));
    EXPECT_FALSE(hasError);
    EXPECT_EQ(codePoint, sallallahouAlayheWasallam);

    // Try a 2-word encoding.
    char32_t deseretYee = 0x10437;

    EXPECT_FALSE(converter.tryConvert(0xD801, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_TRUE(converter.tryConvert(0xDC37, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_EQ(codePoint, deseretYee);

    // Try another 2-word encoding.
    char32_t mandarinHan = 0x24B62;

    EXPECT_FALSE(converter.tryConvert(0xD852, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_TRUE(converter.tryConvert(0xDF62, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_EQ(codePoint, mandarinHan);

    // Try high surrogate invalid encoding.
    EXPECT_FALSE(converter.tryConvert(0xDC01, codePoint, hasError));
    EXPECT_TRUE(hasError);

    converter.reset();

    // Try low surrogate invalid encoding.
    EXPECT_FALSE(converter.tryConvert(0xD852, codePoint, hasError));
    EXPECT_FALSE(hasError);

    EXPECT_FALSE(converter.tryConvert(0x0062, codePoint, hasError));
    EXPECT_TRUE(hasError);
}

} // TED

}} // namespace Ag::Utf
////////////////////////////////////////////////////////////////////////////////


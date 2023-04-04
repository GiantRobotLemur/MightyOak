//! @file Core/Test_String.cpp
//! @brief The definition of unit tests for the Ag::String data type and
//! supporting types.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/Core/String.hpp"
#include "Ag/Core/Utils.hpp"

namespace Ag {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(StringValue, ConstructEmpty)
{
    String specimen;

    EXPECT_TRUE(specimen.isEmpty());
    EXPECT_TRUE(String::Empty.isEmpty());
    EXPECT_EQ(specimen.getUtf8Length(), 0);
    EXPECT_EQ(specimen.getUtf16Length(), 0);
    EXPECT_EQ(specimen.getUtf32Length(), 0);

    specimen = String(static_cast<utf8_cptr_t>(nullptr), 0);

    EXPECT_TRUE(specimen.isEmpty());
    EXPECT_TRUE(String::Empty.isEmpty());
    EXPECT_EQ(specimen.getUtf8Length(), 0);
    EXPECT_EQ(specimen.getUtf16Length(), 0);
    EXPECT_EQ(specimen.getUtf32Length(), 0);
}

GTEST_TEST(StringValue, ConstructNullTerminatedUTF8)
{
    // 0xC2 0xA3 is the UTF-8 encoding of UK pounds
    // 0xF0 0x9F 0x8D 0xBA is the UTF-8 encoding of the beer mug emoji.
    char sample[] = { "Hello \xC2\xA3 World \xF0\x9F\x8D\xBA!" };
    size_t byteCount = arraySize(sample) - 1;

    String specimen(sample);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_NE(String::Empty, specimen);
    EXPECT_STREQ(specimen.getUtf8Bytes(), sample);
    EXPECT_EQ(specimen.getUtf8Length(), byteCount);
    EXPECT_EQ(specimen.getUtf16Length(), byteCount - 3);
    EXPECT_EQ(specimen.getUtf32Length(), byteCount - 4);
    EXPECT_NE(specimen.getUtf8Bytes(), sample);
}

GTEST_TEST(StringValue, ConstructBoundedUTF8)
{
    // 0xC2 0xA3 is the UTF-8 encoding of UK pounds
    // 0xF0 0x9F 0x8D 0xBA is the UTF-8 encoding of the beer mug emoji.
    char sample[] = { "Hello \xC2\xA3 World \xF0\x9F\x8D\xBA!" };
    size_t byteCount = arraySize(sample) - 2;

    String specimen(sample, byteCount);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_NE(String::Empty, specimen);
    EXPECT_EQ(std::memcmp(specimen.getUtf8Bytes(), sample, byteCount), 0);
    EXPECT_EQ(specimen.getUtf8Bytes()[byteCount], 0);
    EXPECT_EQ(specimen.getUtf8Length(), byteCount);
    EXPECT_EQ(specimen.getUtf16Length(), byteCount - 3);
    EXPECT_EQ(specimen.getUtf32Length(), byteCount - 4);
}

GTEST_TEST(StringValue, ConstructInvalidUFT8)
{
    //ASSERT_THROW({ String specimen("Hello \xC2\xA3 World \xF0\x9F\xBA!"); },
    //             UnicodeConversionException);

    //ASSERT_THROW({ String specimen("Hello \xC2 World!"); },
    //             UnicodeConversionException);

    //// Try the largest value representable using the UTF-8 encoding, but
    //// an invalid code point.
    //ASSERT_THROW({ String specimen("Hello \xF7\xBF\xBF\xBF World!"); },
    //             UnicodeConversionException);
}

GTEST_TEST(StringValue, Sharing)
{
    // Create a string which is (almost) guaranteed to be unique.
    char unique[] = "{78CE5541-E65D-4602-B018-99C6792AFAF2-079A3DC0-0EF5-42C1-8C23-11A900E6E34C}";
    long refCount = 0;

    // Create a local scope.
    if (unique != nullptr)
    {
        String specimen = unique;
        refCount = specimen.getReferenceCount();

        EXPECT_FALSE(specimen.isEmpty());
        EXPECT_STREQ(specimen.getUtf8Bytes(), unique);

        if (specimen.isEmpty() == false)
        {
            // Create an inner scope.
            String assignedCopy = specimen;

            EXPECT_EQ(specimen, assignedCopy);
            EXPECT_EQ(specimen.getUtf8Bytes(), assignedCopy.getUtf8Bytes());
            EXPECT_EQ(specimen.getReferenceCount(), refCount + 1);

            String externalCopy = unique;

            EXPECT_EQ(specimen, externalCopy);
            EXPECT_EQ(specimen.getUtf8Bytes(), externalCopy.getUtf8Bytes());
            EXPECT_EQ(specimen.getReferenceCount(), refCount + 2);
        }

        EXPECT_EQ(specimen.getReferenceCount(), refCount);
    }

    String newSpecimen = unique;

    EXPECT_EQ(newSpecimen.getReferenceCount(), refCount);
    EXPECT_FALSE(newSpecimen.isEmpty());
    EXPECT_STREQ(newSpecimen.getUtf8Bytes(), unique);
}

GTEST_TEST(StringValue, AssignNullTerminatedUTF8)
{
    char initial[] = "Hello World!";
    char update[] = "Bring more \xF0\x9F\x8D\xBA!";
    uintptr_t refCount = 0;

    if (update != nullptr)
    {
        String specimen(initial);

        EXPECT_NE(specimen.getUtf8Bytes(), initial);
        EXPECT_STREQ(specimen.getUtf8Bytes(), initial);

        // Assign a different value.
        specimen = update;

        EXPECT_NE(specimen.getUtf8Bytes(), update);
        EXPECT_STREQ(specimen.getUtf8Bytes(), update);
        refCount = specimen.getReferenceCount();

        // Assign the same value.
        specimen = update;
    }

    // Try to fill the heap so that alternate doesn't inadvertently allocate
    // the same block of memory recorded by updateSpecimen.
    String filler("Something else");
    String filler2("Bring more 1234!");
    String filler3("Bring more 2134!");
    String filler4("Bring more 2143!");
    String filler5("Bring more 2413!");
    String alternate(initial);

    alternate = update;

    EXPECT_STREQ(alternate.getUtf8Bytes(), update);
    EXPECT_EQ(alternate.getReferenceCount(), refCount);
}

GTEST_TEST(StringValue, ConstructNullTerminatedUTF16)
{
    // 0xC2 0xA3 is the UTF-8 encoding of UK pounds
    // 0xF0 0x9F 0x8D 0xBA is the UTF-8 encoding of the beer mug emoji.
    char sample[] = "Hello \xC2\xA3 World \xF0\x9F\x8D\xBA!";
    char16_t sample16[] = u"Hello \x00A3 World \xd83c\xdf7a!";
    size_t wordCount = arraySize(sample16) - 1;

    String specimen(sample16);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_NE(String::Empty, specimen);
    EXPECT_STREQ(specimen.getUtf8Bytes(), sample);
    EXPECT_EQ(specimen.getUtf8Length(), wordCount + 3);
    EXPECT_EQ(specimen.getUtf16Length(), wordCount);
    EXPECT_EQ(specimen.getUtf32Length(), wordCount - 1);
    EXPECT_NE(specimen.getUtf8Bytes(), sample);
}

GTEST_TEST(StringValue, ConstructBoundedUTF16)
{
    // 0xC2 0xA3 is the UTF-8 encoding of UK pounds
    // 0xF0 0x9F 0x8D 0xBA is the UTF-8 encoding of the beer mug emoji.
    char sample[] = "Hello \xC2\xA3 World \xF0\x9F\x8D\xBA";
    char16_t sample16[] = u"Hello \x00A3 World \xd83c\xdf7a!";
    size_t wordCount = arraySize(sample16) - 2;

    String specimen(sample16, wordCount);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_NE(String::Empty, specimen);
    EXPECT_STREQ(specimen.getUtf8Bytes(), sample);
    EXPECT_EQ(specimen.getUtf8Length(), wordCount + 3);
    EXPECT_EQ(specimen.getUtf16Length(), wordCount);
    EXPECT_EQ(specimen.getUtf32Length(), wordCount - 1);
    EXPECT_NE(specimen.getUtf8Bytes(), sample);
}

GTEST_TEST(StringValue, ConstructInvalidUTF16)
{
    //ASSERT_THROW({ String specimen(u"Hello \x00A3 World \xd83c\xdb7a!"); },
    //             UnicodeConversionException);

    //ASSERT_THROW({ String specimen(u"Hello \xD8C2 World!"); },
    //             UnicodeConversionException);
}

GTEST_TEST(StringValue, AssignNullTerminatedUTF16)
{
    char initial[] = "Hello World!";
    char16_t initial16[] = u"Hello World!";
    char update[] = "Bring more \xF0\x9F\x8D\xBA!";
    char16_t update16[] = u"Bring more \xd83c\xdf7a!";
    uintptr_t refCount = 0;

    if (update != nullptr)
    {
        String specimen(initial16);

        EXPECT_NE(specimen.getUtf8Bytes(), initial);
        EXPECT_STREQ(specimen.getUtf8Bytes(), initial);

        // Assign a different value.
        specimen = update16;

        EXPECT_NE(specimen.getUtf8Bytes(), update);
        EXPECT_STREQ(specimen.getUtf8Bytes(), update);
        refCount = specimen.getReferenceCount();

        // Assign the same value.
        specimen = update16;
    }

    String alternate(initial);

    alternate = update16;

    EXPECT_STREQ(alternate.getUtf8Bytes(), update);
    EXPECT_EQ(alternate.getReferenceCount(), refCount);
}


GTEST_TEST(StringValue, ConstructNullTerminatedUTF32)
{
    // 0xC2 0xA3 is the UTF-8 encoding of UK pounds
    // 0xF0 0x9F 0x8D 0xBA is the UTF-8 encoding of the beer mug emoji.
    char sample[] = "Hello \xC2\xA3 World \xF0\x9F\x8D\xBA!";
    char32_t sample32[] = U"Hello \x00A3 World \x0001f37a!";
    size_t codePointCount = arraySize(sample32) - 1;

    String specimen(sample32);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_NE(String::Empty, specimen);
    EXPECT_STREQ(specimen.getUtf8Bytes(), sample);
    EXPECT_EQ(specimen.getUtf8Length(), codePointCount + 4);
    EXPECT_EQ(specimen.getUtf16Length(), codePointCount + 1);
    EXPECT_EQ(specimen.getUtf32Length(), codePointCount);
    EXPECT_NE(specimen.getUtf8Bytes(), sample);
}

GTEST_TEST(StringValue, ConstructBoundedUTF32)
{
    // 0xC2 0xA3 is the UTF-8 encoding of UK pounds
    // 0xF0 0x9F 0x8D 0xBA is the UTF-8 encoding of the beer mug emoji.
    char sample[] = { "Hello \xC2\xA3 World \xF0\x9F\x8D\xBA" };
    char32_t sample32[] = U"Hello \x00A3 World \x0001f37a!";
    size_t codePointCount = arraySize(sample32) - 2;

    String specimen(sample32, codePointCount);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_NE(String::Empty, specimen);
    EXPECT_STREQ(specimen.getUtf8Bytes(), sample);
    EXPECT_EQ(specimen.getUtf8Length(), codePointCount + 4);
    EXPECT_EQ(specimen.getUtf16Length(), codePointCount + 1);
    EXPECT_EQ(specimen.getUtf32Length(), codePointCount);
}

GTEST_TEST(StringValue, ConstructInvalidUTF32)
{
    //ASSERT_THROW({ String specimen(U"Hello \xD801 World \x0001f37a!"); },
    //             UnicodeConversionException);

    //ASSERT_THROW({ String specimen(U"Hello \xDFFF World \x0001f37a!"); },
    //             UnicodeConversionException);

    //ASSERT_THROW({ String specimen(U"Hello \xC2A3FE01 World \x0001f37a!"); },
    //             UnicodeConversionException);
}


GTEST_TEST(StringValue, AssignNullTerminatedUTF32)
{
    char initial[] = "Hello World!";
    char32_t initial32[] = U"Hello World!";
    char update[] = "Bring more \xF0\x9F\x8D\xBA!";
    char32_t update32[] = U"Bring more \x0001f37a!";
    uintptr_t refCount = 0;

    if (update != nullptr)
    {
        String specimen(initial32);

        EXPECT_NE(specimen.getUtf8Bytes(), initial);
        EXPECT_STREQ(specimen.getUtf8Bytes(), initial);

        // Assign a different value.
        specimen = update32;

        EXPECT_NE(specimen.getUtf8Bytes(), update);
        EXPECT_STREQ(specimen.getUtf8Bytes(), update);
        refCount = specimen.getReferenceCount();

        // Assign the same value.
        specimen = update32;
    }

    String alternate(initial32);

    alternate = update32;

    EXPECT_STREQ(alternate.getUtf8Bytes(), update);
    EXPECT_EQ(alternate.getReferenceCount(), refCount);
}

GTEST_TEST(StringValue, IteratorAdvance)
{
    char32_t source[] = U"Hello World \x0001f37a!";

    String specimen(source);
    const char32_t *sourcePtr = source;

    for (char32_t codePoint : specimen)
    {
        EXPECT_EQ(codePoint, *sourcePtr);
        ++sourcePtr;
    }
}

GTEST_TEST(StringValue, IteratorRetreat)
{
    char32_t source[] = U"Hello World \x0001f37a!";

    String specimen(source);
    const char32_t *sourcePtr = source + arraySize(source) - 1;

    auto pos = specimen.end();

    do
    {
        --pos;
        --sourcePtr;

        EXPECT_EQ(*pos, *sourcePtr);
    } while (pos != specimen.begin());
}

GTEST_TEST(StringValue, ToUpper)
{
    String specimen = "HeLlo WoRlD 1234!?";

    String result = specimen.toUpper();

    EXPECT_STREQ(result.getUtf8Bytes(), "HELLO WORLD 1234!?");

    // Ensure no new data is created when the operation doesn't change anything.
    String identical = "GOOD EVENING EVERYONE 964";
    result = identical.toUpper();

    EXPECT_EQ(identical.getUtf8Bytes(), result.getUtf8Bytes());
}

GTEST_TEST(StringValue, ToLower)
{
    String specimen = "HeLlo WoRlD 1234!?";

    String result = specimen.toLower();

    EXPECT_STREQ(result.getUtf8Bytes(), "hello world 1234!?");

    // Ensure no new data is created when the operation doesn't change anything.
    String identical = "good evening everyone 964";
    result = identical.toLower();

    EXPECT_EQ(identical.getUtf8Bytes(), result.getUtf8Bytes());
}

GTEST_TEST(StringValue, Compare)
{
    String specimen = "Hello World!";

    EXPECT_EQ(specimen.compare(specimen), 0);

    EXPECT_GT(specimen.compare(String::Empty), 0);
    EXPECT_GT(specimen.compare(String("Aardvark")), 0);
    EXPECT_LT(specimen.compare(String("Zebra")), 0);
    EXPECT_LT(specimen.compare(String("Hello World! And good day to you.")), 0);
    EXPECT_GT(specimen.compare(String("Hello")), 0);
}

GTEST_TEST(StringValue, CompareIgnoreCase)
{
    String specimen = "Hello World!";

    EXPECT_EQ(specimen.compareIgnoreCase(specimen), 0);

    EXPECT_GT(specimen.compareIgnoreCase(String::Empty), 0);
    EXPECT_GT(specimen.compareIgnoreCase(String("Aardvark")), 0);
    EXPECT_LT(specimen.compareIgnoreCase(String("Zebra")), 0);
    EXPECT_LT(specimen.compareIgnoreCase(String("Hello World! And good day to you.")), 0);
    EXPECT_GT(specimen.compareIgnoreCase(String("Hello")), 0);

    EXPECT_EQ(specimen.compareIgnoreCase(String("HELLO WORLD!")), 0);
    EXPECT_EQ(specimen.compareIgnoreCase(String("hello world!")), 0);
    EXPECT_EQ(specimen.compareIgnoreCase(String("hElLo wOrLd!")), 0);
}


GTEST_TEST(StringValue, TryParseUint32)
{
    String specimen = "0020";
    uint32_t result;

    ASSERT_TRUE(specimen.tryParseScalar(result, 16));
    EXPECT_EQ(result, 0x0020u);
}

} // TED

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


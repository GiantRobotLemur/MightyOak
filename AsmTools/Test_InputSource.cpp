//! @file Test_InputSource.cpp
//! @brief The definition of unit tests for implementations of the IInputSource
//! object.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <string>

#include <gtest/gtest.h>

#include "Ag/Core.hpp"
#include "InputSource.hpp"
#include "InputSet.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct IsZero
{
    bool operator()(char32_t ch) const { return ch == U'\0'; }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(InputSource, CreateEmptySource)
{
    const size_t BufferSize = 32;
    char32_t buffer[BufferSize];
    std::fill_n(buffer, BufferSize, U'\0');

    IInputSourcePtr specimen = createBufferInputSource(String::Empty);
    ASSERT_TRUE(specimen);

    size_t charsRead = specimen->readCharacters(buffer, BufferSize);
    EXPECT_EQ(charsRead, 0u);


    EXPECT_EQ(std::find_if_not(buffer, buffer + BufferSize, IsZero()),
              buffer + BufferSize);
}

GTEST_TEST(InputSource, CreateFilledSource)
{
    const size_t BufferSize = 32;
    char32_t buffer[BufferSize];
    std::fill_n(buffer, BufferSize, U'\0');

    String sample("Hello World!");
    IInputSourcePtr specimen = createBufferInputSource(sample);
    ASSERT_TRUE(specimen);

    size_t charsRead = specimen->readCharacters(buffer, BufferSize / 2);
    EXPECT_EQ(charsRead, sample.getUtf32Length());

    // Ensure the characters read are a correct conversion.
    std::u32string sample32 = sample.toUtf32();

    ASSERT_EQ(charsRead, sample32.length());
    EXPECT_TRUE(std::equal(sample32.begin(), sample32.end(),
                           buffer, buffer + charsRead));

    // Ensure the rest of the buffer was unmodified.
    EXPECT_EQ(std::find_if_not(buffer + (BufferSize / 2),
                               buffer + BufferSize, IsZero()),
              buffer + BufferSize);

    // Ensure we cannot read anything more from the buffer.
    EXPECT_EQ(specimen->readCharacters(buffer, 1), 0u);
}

GTEST_TEST(InputSource, PartialRead)
{
    const size_t BufferSize = 10;
    char32_t buffer[BufferSize];
    std::fill_n(buffer, BufferSize, U'\0');

    // 2.5 x 0-9
    String sample("0123456789012345678901234");
    char32_t expected[] = U"0123456789";
    const size_t expectedSize = 10;
    IInputSourcePtr specimen = createBufferInputSource(sample);

    ASSERT_TRUE(specimen);

    // Read the first sequence.
    size_t charsRead = specimen->readCharacters(buffer, BufferSize);

    ASSERT_EQ(charsRead, expectedSize);
    EXPECT_TRUE(std::equal(expected, expected + expectedSize,
                           buffer, buffer + expectedSize));

    // Read a second sequence.
    std::fill_n(buffer, BufferSize, U'\0');

    charsRead = specimen->readCharacters(buffer, BufferSize);

    ASSERT_EQ(charsRead, expectedSize);
    EXPECT_TRUE(std::equal(expected, expected + expectedSize,
                           buffer, buffer + expectedSize));

    // Read the third sequence and only get half.
    const size_t partialSize = 5;
    std::fill_n(buffer, BufferSize, U'\0');

    charsRead = specimen->readCharacters(buffer, BufferSize);

    ASSERT_EQ(charsRead, partialSize);
    EXPECT_TRUE(std::equal(expected, expected + partialSize,
                           buffer, buffer + partialSize));

    // Ensure the rest of the buffer was unmodified.
    EXPECT_EQ(std::find_if_not(buffer + partialSize,
                               buffer + BufferSize, IsZero()),
              buffer + BufferSize);

    // Ensure we cannot read anything more from the buffer.
    EXPECT_EQ(specimen->readCharacters(buffer, 1), 0u);
    EXPECT_EQ(specimen->readCharacters(buffer, 1), 0u);
    EXPECT_EQ(specimen->readCharacters(buffer, 1), 0u);
}

GTEST_TEST(InputSource, FromSet)
{
    String id = "Test.asm";
    String sample("ADD R0,R2,R5,LSL #16");

    // Create the input set with a single buffer source.
    IInputSetUPtr specimen = createStringInputSet(id, sample);

    ASSERT_TRUE(specimen);

    IInputSourcePtr source;
    String errorOrId;

    // Get the single source.
    ASSERT_TRUE(specimen->tryGetPrimaryInputSource(source, errorOrId));

    ASSERT_TRUE(source);
    EXPECT_STREQ(errorOrId.getUtf8Bytes(), id.getUtf8Bytes());

    const size_t BufferSize = 32;
    char32_t buffer[BufferSize];
    std::fill_n(buffer, BufferSize, U'\0');

    // Read from the source and ensure we get back what we put in.
    size_t charsRead = source->readCharacters(buffer, BufferSize);

    EXPECT_EQ(charsRead, sample.getUtf32Length());

    String result(buffer, charsRead);

    EXPECT_STREQ(result.getUtf8Bytes(), sample.getUtf8Bytes());

    // Ensure we have reached the end of the buffer.
    EXPECT_EQ(source->readCharacters(buffer, 1), 0);
}

GTEST_TEST(InputSource, BufferSetHasNoSecondarySources)
{
    String id = "Test.asm";
    String sample("ADD R0,R2,R5,LSL #16");

    // Create the input set with a single buffer source.
    IInputSetUPtr specimen = createStringInputSet(id, sample);

    ASSERT_TRUE(specimen);

    IInputSetUPtr source;
    String errorOrId;
    String sourceId = "Libs/Utils.asm";

    // Try to get a secondary source.
    EXPECT_FALSE(specimen->tryGetInputSource(sourceId, source, errorOrId));

    // Ensure we got an error message back, not the source identifier.
    EXPECT_FALSE(errorOrId.isEmpty());
    EXPECT_STRNE(sourceId.getUtf8Bytes(), errorOrId.getUtf8Bytes());
}

} // TED

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


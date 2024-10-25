//! @file Test_InputContext.cpp
//! @brief The definition of unit tests for the InputContext class.
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

#include "ParseContext.hpp"
#include "Ag/GTest_Core.hpp"

namespace Mo {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(InputContext, EmptyBuffer)
{
    IInputSourcePtr source = createBufferInputSource(Ag::String::Empty);
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");
    InputContext specimen(source, position, sourceId, 12);

    EXPECT_STREQ(specimen.getCurrentLocation().FileName.getUtf8Bytes(),
                 sourceId.getUtf8Bytes());
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);
    EXPECT_EQ(specimen.getBaseLexerStackLevel(), 12);

    // Verify we cannot read anything from the source.
    char32_t next = U'\0';

    EXPECT_FALSE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'\0');

    // Verify the position hasn't changed.
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);
}

GTEST_TEST(InputContext, ReadCharacters)
{
    IInputSourcePtr source = createBufferInputSource("EOR R0,R1,R4");
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");
    InputContext specimen(source, position, sourceId, 0);

    EXPECT_STREQ(specimen.getCurrentLocation().FileName.getUtf8Bytes(),
                 sourceId.getUtf8Bytes());
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    char32_t next = U'\0';

    // Read the first character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'E');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 1);

    // Read the second character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'O');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 2);

    // Read the third character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'R');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 3);
}

GTEST_TEST(InputContext, ReadLines)
{
    IInputSourcePtr source = createBufferInputSource("\nA\n");
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");
    InputContext specimen(source, position, sourceId, 0);

    EXPECT_STREQ(specimen.getCurrentLocation().FileName.getUtf8Bytes(),
                 sourceId.getUtf8Bytes());
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    char32_t next = U'\0';

    // Read the first character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'\n');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    // Read the second character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'A');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 1);

    // Read the third character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'\n');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 3);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    EXPECT_FALSE(specimen.tryGetNextCharacter(next));
}

GTEST_TEST(InputContext, UngetCharacters)
{
    IInputSourcePtr source = createBufferInputSource("\nAB");
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");
    InputContext specimen(source, position, sourceId, 0);

    EXPECT_STREQ(specimen.getCurrentLocation().FileName.getUtf8Bytes(),
                 sourceId.getUtf8Bytes());
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    char32_t next = U'\0';

    // Read the first character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'\n');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    // Read the second character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'A');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 1);

    // Read the third character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'B');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 2);

    // Unget the second and third characters.
    specimen.ungetCharacter();
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 1);

    specimen.ungetCharacter();
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    // Re-get the second character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'A');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 1);

    // Re-get the third character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'B');

    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 2);

    // Verify the end of the stream has been reached.
    EXPECT_FALSE(specimen.tryGetNextCharacter(next));
}

GTEST_TEST(InputContext, UngetOverNewLine)
{
    IInputSourcePtr source = createBufferInputSource("A\nB");
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");
    InputContext specimen(source, position, sourceId, 0);

    EXPECT_STRINGEQ(specimen.getCurrentLocation().FileName, sourceId);
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    char32_t next = U'\0';

    // Read the first character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'A');
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 1);

    // Get the new line character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'\n');
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    // Unget the new line character.
    specimen.ungetCharacter();
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);

    // Re-get the new line character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'\n');
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    // Get the first character on the new line.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'B');
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 2);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 1);

    // Verify end of input.
    EXPECT_FALSE(specimen.tryGetNextCharacter(next));
}

GTEST_TEST(InputContext, EnsureNextCharacter)
{
    IInputSourcePtr source = createBufferInputSource("AB");
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");
    InputContext specimen(source, position, sourceId, 0);

    char32_t next;

    // Ensure we can get the first character.
    EXPECT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'A');

    // Ensure we don't get an incorrect character.
    EXPECT_FALSE(specimen.ensureNextCharacter(U'C'));

    // Ensure we consume the correct character.
    EXPECT_TRUE(specimen.ensureNextCharacter(U'B'));

    // Ensure we get nothing at the end of the input stream.
    EXPECT_FALSE(specimen.ensureNextCharacter(U'B'));

    // Verify the end of the input stream.
    EXPECT_FALSE(specimen.tryGetNextCharacter(next));
}

GTEST_TEST(InputContext, BreakInputStream)
{
    // Base on the value defined inside InputContext.
    const size_t BufferCapacity = 256;

    std::string sample(BufferCapacity, '\n');
    char extra[] = "ADR R0,OverThere\n"
        "MOV R0,#&FF0000\n"
        "LDR R9,[R1],#8\n"
        "SWP R11,[R2]\n"
        "AND R3,R4,R12,LSR #11\n"
        "MRC R9\n"
        "STC R0,[R11]\n";
    sample.append(extra);

    IInputSourcePtr source = createBufferInputSource(sample);
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");
    InputContext specimen(source, position, sourceId, 0);

    EXPECT_STREQ(specimen.getCurrentLocation().FileName.getUtf8Bytes(),
                 sourceId.getUtf8Bytes());
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    char32_t next = U'\0';

    bool isOK = true;

    // Read the leading line breaks.
    for (size_t index = 0; isOK && (index < BufferCapacity); ++index)
    {
        ASSERT_TRUE(specimen.tryGetNextCharacter(next));
        EXPECT_EQ(next, U'\n');
    }

    specimen.breakInputStream();

    // Ensure we can read the next character.
    ASSERT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'A');

    // Ensure we can unget the character we just obtained.
    EXPECT_NO_THROW({ specimen.ungetCharacter(); });

    // Ensure we can't unget to a character before the break.
    EXPECT_THROW({ specimen.ungetCharacter(); }, Ag::OperationException);
}


GTEST_TEST(InputContext, BreakInputStreamWithShuffle)
{
    //const size_t BufferThreadhold = 34;
    const size_t BufferCapacity = 256;
    std::string sample(BufferCapacity, '\n');
    char extra[] = "ADR R0,OverThere\n"
        "MOV R0,#&FF0000\n"
        "LDR R9,[R1],#8\n"
        "SWP R11,[R2]\n"
        "AND R3,R4,R12,LSR #11\n"
        "MRC R9\n"
        "STC R0,[R11]\n";
    sample.append(extra);

    IInputSourcePtr source = createBufferInputSource(sample);
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");
    InputContext specimen(source, position, sourceId, 0);

    EXPECT_STREQ(specimen.getCurrentLocation().FileName.getUtf8Bytes(),
                 sourceId.getUtf8Bytes());
    EXPECT_EQ(specimen.getCurrentLocation().LineNo, 1);
    EXPECT_EQ(specimen.getCurrentLocation().Offset, 0);

    char32_t next = U'\0';

    bool isOK = true;

    // Read the leading line breaks, breaking the stream
    // half way through.
    for (size_t index = 0; isOK && (index < BufferCapacity); ++index)
    {
        ASSERT_TRUE(specimen.tryGetNextCharacter(next));
        EXPECT_EQ(next, U'\n');

        if (index == (BufferCapacity / 2))
        {
            specimen.breakInputStream();
        }
    }

    // Ensure we can read the next character.
    ASSERT_TRUE(specimen.tryGetNextCharacter(next));
    EXPECT_EQ(next, U'A');

    // Ensure we can unget the character we just obtained.
    EXPECT_NO_THROW({ specimen.ungetCharacter(); });

    // Ensure we can unget.past the last break because characters
    // were moved to the front of the buffer during the last re-fill.
    EXPECT_NO_THROW({ specimen.ungetCharacter(); });
}

} // Anonymous namespace

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file Test_CoreRegListLexer.cpp
//! @brief The definition of unit tests for the core register list
//! lexical analyser.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/GTest_Core.hpp"

#include "AsmEnums.hpp"
#include "InputContext.hpp"
#include "LexicalAnalysers.hpp"
#include "LexicalContext.hpp"

#include "AsmTools/InstructionInfo.hpp"
#include "AsmTools/Options.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
InputContext createInput(const char *sourceCode)
{
    IInputSourcePtr source = createBufferInputSource(String(sourceCode));
    Location position("MyProject/Libs/Here.asm");
    String sourceId("MyProject/Libs/Lib.asm");

    return InputContext(source, position, sourceId, 2);
}

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CoreRegListLexer, NoTextReturnsNoToken)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(CoreRegListLexer, WhiteSpaceReturnsNoToken)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("   \t\r  ");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(CoreRegListLexer, CommentReturnsNoToken)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("  ; This is a line comment.");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(CoreRegListLexer, RecogniseOpenBrace)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput(" { ");

    Token next;

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::OpenBrace);

    const Location &loc = next.getLocation();

    EXPECT_FALSE(loc.FileName.isEmpty());
    EXPECT_EQ(loc.LineNo, 1);
    EXPECT_EQ(loc.Offset, 2);
    EXPECT_TRUE(next.getValue().isEmpty());

    // Verify there are no more tokens.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Empty);
}

GTEST_TEST(CoreRegListLexer, RecogniseCloseBrace)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput(" \t}  ");

    Token next;

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::CloseBrace);

    const Location &loc = next.getLocation();

    EXPECT_FALSE(loc.FileName.isEmpty());
    EXPECT_EQ(loc.LineNo, 1);
    EXPECT_EQ(loc.Offset, 3);
    EXPECT_TRUE(next.getValue().isEmpty());

    // Verify there are no more tokens.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Empty);
}

GTEST_TEST(CoreRegListLexer, RecogniseSymbol)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput(
        "\tmyReg  ; Comment on the same line as the label.");

    Token next;

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Symbol);

    const Location &loc = next.getLocation();

    EXPECT_FALSE(loc.FileName.isEmpty());
    EXPECT_EQ(loc.LineNo, 1);
    EXPECT_EQ(loc.Offset, 2);
    EXPECT_STRINGEQC(next.getValue(), "myReg");

    // Verify there are no more tokens.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Empty);
}

GTEST_TEST(CoreRegListLexer, RecogniseDashToken)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput(" -\n; Commented out");

    Token next;

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Minus);

    const Location &loc = next.getLocation();

    EXPECT_FALSE(loc.FileName.isEmpty());
    EXPECT_EQ(loc.LineNo, 1);
    EXPECT_EQ(loc.Offset, 2);
    EXPECT_TRUE(next.getValue().isEmpty());

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);

    // Verify there are no more tokens.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Empty);
}


GTEST_TEST(CoreRegListLexer, ContinuationReturnsNoToken)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("  \\  \n , ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Comma);
}

GTEST_TEST(CoreRegListLexer, ContinuationWarnsOfTrailingCharacters)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("  \\ , \n { ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Warning);
    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::OpenBrace);
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(CoreRegListLexer, ContinuationIgnoresComments)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("  \\ ; Hello \n { ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::OpenBrace);
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(CoreRegListLexer, RecogniseNewLineAsStatementTerminatorToken)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("; header comment.\nsymbol");

    Token next;

    // Get the new line
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    EXPECT_EQ(next.getLocation().LineNo, 2);
    EXPECT_EQ(next.getLocation().Offset, 0);
    EXPECT_TRUE(next.getValue().isEmpty());

    // Get the label on the next line.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Symbol);
    EXPECT_EQ(next.getLocation().LineNo, 2);
    EXPECT_EQ(next.getLocation().Offset, 1);
    EXPECT_STRINGEQC(next.getValue(), "symbol");

    // Ensure that is the end of the stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(CoreRegListLexer, RecogniseColonAsStatementTerminatorToken)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("mySymbol:R11\n");

    Token next;

    // Get the label statement token.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Symbol);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 1);
    EXPECT_STRINGEQC(next.getValue(), "mySymbol");

    // Recognise the colon.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 9);
    EXPECT_TRUE(next.getValue().isEmpty());

    // Recognise the second label statement.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Symbol);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 10);
    EXPECT_STRINGEQC(next.getValue(), "R11");

    // Recognised the new line as a statement terminator.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    EXPECT_EQ(next.getLocation().LineNo, 2);
    EXPECT_EQ(next.getLocation().Offset, 0);
    EXPECT_TRUE(next.getValue().isEmpty());

    // Ensure that is the end of the stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(CoreRegListLexer, RecogniseGarbageAsErrorToken)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput("  @#!.hello\n");

    Token next;

    // Get the label statement token.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Error);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 3);
    EXPECT_FALSE(next.getValue().isEmpty());

    // Ensure statement terminator found after garbage.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(CoreRegListLexer, RecogniseCharacterTokens)
{
    ILexicalContext *specimen = getCoreRegListLexer();
    InputContext input = createInput(": { } , -\n");

    TokenClass expectedTokens[] =
    {
        TokenClass::StatementTerminator,
        TokenClass::OpenBrace,
        TokenClass::CloseBrace,
        TokenClass::Comma,
        TokenClass::Minus,
        TokenClass::StatementTerminator
    };

    Token token;

    for (TokenClass expectedToken : expectedTokens)
    {
        ASSERT_TRUE(specimen->tryGetNextToken(input, token));
        EXPECT_EQ(token.getClass(), expectedToken);
    }
}

} // TED

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


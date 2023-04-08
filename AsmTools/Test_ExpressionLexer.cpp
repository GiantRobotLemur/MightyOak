//! @file Test_ExpressionLexer.cpp
//! @brief The definition of unit tests for the expression lexical analyser.
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

#include "LexicalAnalysers.hpp"
#include "LexicalContext.hpp"
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
GTEST_TEST(ExpressionLexer, NoTextReturnsNoToken)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(ExpressionLexer, WhiteSpaceReturnsNoToken)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("   \t\r  ");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(ExpressionLexer, CommentReturnsNoToken)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("  ; This is a line comment.");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}


GTEST_TEST(ExpressionLexer, ContinuationReturnsNoToken)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("  \\  \n $ ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Dollar);
}

GTEST_TEST(ExpressionLexer, ContinuationWarnsOfTrailingCharacters)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("  \\ * \n + ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Warning);
    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Plus);
}

GTEST_TEST(ExpressionLexer, ContinuationIgnoresCommentsCharacters)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("  \\ ; Hi there! \n + ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Plus);
}

GTEST_TEST(ExpressionLexer, RecogniseCharacterTokens)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("+:- * / $()\n");

    TokenClass expectedTokens[] =
    {
        TokenClass::Plus,
        TokenClass::StatementTerminator,
        TokenClass::Minus,
        TokenClass::Asterisk,
        TokenClass::Slash,
        TokenClass::Dollar,
        TokenClass::OpenParenthesis,
        TokenClass::CloseParenthesis,
        TokenClass::StatementTerminator
    };

    Token token;

    for (TokenClass expectedToken : expectedTokens)
    {
        ASSERT_TRUE(specimen->tryGetNextToken(input, token));
        EXPECT_EQ(token.getClass(), expectedToken);
    }
}

GTEST_TEST(ExpressionLexer, RecogniseSimpleTokens)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("<< AND >> OR EOR >>> NOT XOR Hello");

    TokenClass expectedTokens[] =
    {
        TokenClass::LogicShiftLeft,
        TokenClass::KeywordAnd,
        TokenClass::LogicShiftRight,
        TokenClass::KeywordOr,
        TokenClass::KeywordXor,
        TokenClass::ArithmeticShiftRight,
        TokenClass::KeywordNot,
        TokenClass::KeywordXor,
    };

    Token token;

    for (TokenClass expectedToken : expectedTokens)
    {
        ASSERT_TRUE(specimen->tryGetNextToken(input, token));
        EXPECT_EQ(token.getClass(), expectedToken);
    }
}

GTEST_TEST(ExpressionLexer, RecogniseSymbolTokens)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput("FooBar Foo_Bar __FOO_BAR__");

    Token token;
    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Symbol);
    EXPECT_STRINGEQC(token.getValue(), "FooBar");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Symbol);
    EXPECT_STRINGEQC(token.getValue(), "Foo_Bar");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Symbol);
    EXPECT_STRINGEQC(token.getValue(), "__FOO_BAR__");

    // End of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(ExpressionLexer, RecogniseBinaryLiteral)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput(" %01001 0B1110 0b0010 % \n 0b");

    Token token;

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 2);
    EXPECT_STRINGEQC(token.getValue(), "01001");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 2);
    EXPECT_STRINGEQC(token.getValue(), "1110");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 2);
    EXPECT_STRINGEQC(token.getValue(), "0010");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}


GTEST_TEST(ExpressionLexer, RecogniseHexLiteral)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput(" &Dead1 & \n 0x921BeEf 0X : 0x0cAfE");

    Token token;

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 16);
    EXPECT_STRINGEQC(token.getValue(), "Dead1");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 16);
    EXPECT_STRINGEQC(token.getValue(), "921BeEf");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 16);
    EXPECT_STRINGEQC(token.getValue(), "0cAfE");

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}


GTEST_TEST(ExpressionLexer, RecogniseDecimalLiteral)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput(" 0000 9876543219876543210 0; comment");

    Token token;

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 10);
    EXPECT_STRINGEQC(token.getValue(), "0000");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 10);
    EXPECT_STRINGEQC(token.getValue(), "9876543219876543210");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::IntegerLiteral);
    EXPECT_EQ(getTokenScalar(token, TokenProperty::IntRadix, 99), 10);
    EXPECT_STRINGEQC(token.getValue(), "0");

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(ExpressionLexer, RecogniseRealLiteral)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput(
        "12.345 0.324 000.0001 1. \n 1.e \n"
        "0e19 45e+45 99E-99 12e \n"
        "3.14159e12 6.313e-9 4.21e- \n"
        "75.22e+23 99.009E+");

    Token token;

    // Line 1.
    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "12.345");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "0.324");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "000.0001");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());  // "1."

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());  // "1.e"

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    // Line 2
    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "0e19");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "45e+45");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "99E-99");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());  // "12e"

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    // Line 3
    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "3.14159e12");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "6.313e-9");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());  // "4.21e-"

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    // Line 4
    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RealLiteral);
    EXPECT_STRINGEQC(token.getValue(), "75.22e+23");

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());  // "99.009E+"

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(ExpressionLexer, RecogniseStringLiteralNoEscpaes)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput(
        "\"Hello World!\" \"Unterminated\n'Hi there' 'Not closed");

    Token token;

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "Hello World!");
    EXPECT_FALSE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "Unterminated");
    EXPECT_TRUE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "Hi there");
    EXPECT_FALSE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "Not closed");
    EXPECT_TRUE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    // Verify the end of the input stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(ExpressionLexer, RecogniseStringLiteralAsciiEscape)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput(
        "\"Hello\\x20World\\X21\" 'A\\x4243' '\\X4'\n"
        "'\\xG'\n'\\X");

    Token token;

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "Hello World!");
    EXPECT_FALSE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "AB43");
    EXPECT_FALSE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    // Verify the end of the input stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(ExpressionLexer, RecogniseStringLiteralUtf16Escape)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput(
        "\"Hello\\u0020World\\u0021\" 'A\\u004243' '\\u4'\n"
        "'\\u20G'\n'\\u");

    Token token;

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "Hello World!");
    EXPECT_FALSE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "AB43");
    EXPECT_FALSE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    // Verify the end of the input stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}


GTEST_TEST(ExpressionLexer, RecogniseStringLiteralUtf32Escape)
{
    ILexicalContext *specimen = getExpressionLexer();
    InputContext input = createInput(
        "\"Hello\\U00000020World\\U00000021\" 'A\\U0000004243' '\\U0040'\n"
        "'\\U0020G'\n'\\U");

    Token token;

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "Hello World!");
    EXPECT_FALSE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StringLiteral);
    EXPECT_STRINGEQC(token.getValue(), "AB43");
    EXPECT_FALSE(getTokenFlag(token, TokenProperty::UnterminatedString, false));

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::StatementTerminator);

    ASSERT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
    EXPECT_FALSE(token.getValue().isEmpty());

    // Verify the end of the input stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

} // TED

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


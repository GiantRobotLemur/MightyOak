//! @file Test_PsrLexer.cpp
//! @brief The definition of unit tests for the PSR lexical analyser.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
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
GTEST_TEST(PsrLexer, NoTextReturnsNoToken)
{
    ILexicalContext *specimen = getPsrNameLexer();
    InputContext input = createInput("");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(PsrLexer, WhiteSpaceReturnsNoToken)
{
    ILexicalContext *specimen = getPsrNameLexer();
    InputContext input = createInput("   \t\r  ");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(PsrLexer, CommentReturnsNoToken)
{
    ILexicalContext *specimen = getPsrNameLexer();
    InputContext input = createInput("  ; This is a line comment.");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}


GTEST_TEST(PsrLexer, ContinuationReturnsNoToken)
{
    ILexicalContext *specimen = getPsrNameLexer();
    InputContext input = createInput("  \\  \n CPsr ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
}

GTEST_TEST(PsrLexer, ContinuationWarnsOfTrailingCharacters)
{
    ILexicalContext *specimen = getPsrNameLexer();
    InputContext input = createInput("  \\ * \n SpsR ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Warning);
    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::SPSR);
}

GTEST_TEST(PsrLexer, ContinuationIgnoresCommentsCharacters)
{
    ILexicalContext *specimen = getPsrNameLexer();
    InputContext input = createInput("  \\ ; Hi there! \n cPsR ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
}

GTEST_TEST(PsrLexer, SuffixIsInvalid)
{
    ILexicalContext *specimen = getPsrNameLexer();
    InputContext input = createInput("\tCPSR_f ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
}

GTEST_TEST(PsrLexer, AllSuffixIsValid)
{
    ILexicalContext *specimen = getPsrNameLexer();
    InputContext input = createInput("  SPSR_all ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::SPSR);

    PsrComponent component = PsrComponent::None;
    EXPECT_FALSE(token.tryGetProperty<PsrComponent>(TokenProperty::PsrComponent, component));
}






GTEST_TEST(PsrComponentLexer, NoTextReturnsNoToken)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(PsrComponentLexer, WhiteSpaceReturnsNoToken)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("   \t\r  ");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(PsrComponentLexer, CommentReturnsNoToken)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  ; This is a line comment.");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(PsrComponentLexer, ContinuationReturnsNoToken)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  \\  \n CPsr ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
}

GTEST_TEST(PsrComponentLexer, ContinuationWarnsOfTrailingCharacters)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  \\ * \n SpsR ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Warning);
    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::SPSR);
}

GTEST_TEST(PsrComponentLexer, ContinuationIgnoresCommentsCharacters)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  \\ ; Hi there! \n cPsR ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
}

GTEST_TEST(PsrComponentLexer, AllSuffix)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  SPSR_all ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::SPSR);
    EXPECT_EQ(token.getProperty(TokenProperty::PsrComponent, PsrComponent::None), PsrComponent::All);
}

GTEST_TEST(PsrComponentLexer, FlgSuffix)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  cpsr_FLG ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
    EXPECT_EQ(token.getProperty(TokenProperty::PsrComponent, PsrComponent::None), PsrComponent::Flags);
}

GTEST_TEST(PsrComponentLexer, F_Suffix)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  cpsr_F ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
    EXPECT_EQ(token.getProperty(TokenProperty::PsrComponent, PsrComponent::None), PsrComponent::Flags);
}

GTEST_TEST(PsrComponentLexer, X_Suffix)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  SPsr_x ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::SPSR);
    EXPECT_EQ(token.getProperty(TokenProperty::PsrComponent, PsrComponent::None), PsrComponent::Extension);
}

GTEST_TEST(PsrComponentLexer, S_Suffix)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  CPSR_s ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
    EXPECT_EQ(token.getProperty(TokenProperty::PsrComponent, PsrComponent::None), PsrComponent::Status);
}

GTEST_TEST(PsrComponentLexer, C_Suffix)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  sPSR_C ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::SPSR);
    EXPECT_EQ(token.getProperty(TokenProperty::PsrComponent, PsrComponent::None), PsrComponent::Control);
}

GTEST_TEST(PsrComponentLexer, MultipleSuffixes)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  cpsr_xc ");

    Token token;
    constexpr uint8_t DefaultSuffix = 0;
    constexpr uint8_t ExpectedSuffix = toScalar(PsrComponent::Extension) |
                                       toScalar(PsrComponent::Control);

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
    EXPECT_EQ(token.getProperty(TokenProperty::PsrComponent, DefaultSuffix), ExpectedSuffix);
}

GTEST_TEST(PsrComponentLexer, AllSuffixes)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  cpsr_fSxc ");

    Token token;
    constexpr uint8_t DefaultSuffix = 0;
    constexpr uint8_t ExpectedSuffix = toScalar(PsrComponent::All);

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::RegisterStatus);
    EXPECT_EQ(token.getProperty(TokenProperty::RegisterIndex, CoreRegister::R0), CoreRegister::CPSR);
    EXPECT_EQ(token.getProperty(TokenProperty::PsrComponent, DefaultSuffix), ExpectedSuffix);
}

GTEST_TEST(PsrComponentLexer, RepeatedSuffixesFail)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  sPSR_Cfxsc ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
}

GTEST_TEST(PsrComponentLexer, InvalidSuffixesFail)
{
    ILexicalContext *specimen = getPsrComponentLexer();
    InputContext input = createInput("  CPSR_Cfxz ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Error);
}

} // TED

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


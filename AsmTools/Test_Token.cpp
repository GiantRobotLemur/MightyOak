//! @file Test_Token.cpp
//! @brief The definition of unit tests for the Token class.
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

#include "AsmTools/InstructionInfo.hpp"

#include "AsmEnums.hpp"
#include "Token.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Token, DefaultConstruct)
{
    Token specimen;

    EXPECT_EQ(specimen.getClass(), TokenClass::Empty);
    EXPECT_FALSE(specimen.getLocation().isValid());
    EXPECT_TRUE(specimen.getValue().isEmpty());
}

GTEST_TEST(Token, ConstructWithoutValue)
{
    Location here(__FILE__);
    Token specimen(here, TokenClass::Error);

    EXPECT_EQ(specimen.getClass(), TokenClass::Error);
    EXPECT_TRUE(specimen.getLocation().isValid());
    EXPECT_STREQ(specimen.getLocation().FileName.getUtf8Bytes(), __FILE__);
    EXPECT_GT(specimen.getLocation().LineNo, 0);
    EXPECT_GE(specimen.getLocation().Offset, 0);
    EXPECT_TRUE(specimen.getValue().isEmpty());
}

GTEST_TEST(Token, ConstructWithValue)
{
    Location here(__FILE__);
    Token specimen(here, TokenClass::Bang, "Hello World!");

    EXPECT_EQ(specimen.getClass(), TokenClass::Bang);
    EXPECT_TRUE(specimen.getLocation().isValid());
    EXPECT_STREQ(specimen.getLocation().FileName.getUtf8Bytes(), __FILE__);
    EXPECT_GT(specimen.getLocation().LineNo, 0);
    EXPECT_GE(specimen.getLocation().Offset, 0);
    EXPECT_STREQ(specimen.getValue().getUtf8Bytes(), "Hello World!");
}

GTEST_TEST(Token, ConstructInherited)
{
    Location there("Over there!");
    there.LineNo = 99;
    there.Offset = 80;

    Location here(__FILE__);
    here.LineNo = 3;
    here.Offset = 12;

    Token parent(there, TokenClass::StatementTerminator, "42");
    Token specimen(here, parent);

    EXPECT_STREQ(parent.getLocation().FileName.getUtf8Bytes(), "Over there!");
    EXPECT_EQ(parent.getLocation().LineNo, there.LineNo);
    EXPECT_EQ(parent.getLocation().Offset, there.Offset);

    EXPECT_EQ(specimen.getClass(), parent.getClass());
    ASSERT_TRUE(specimen.getLocation().isValid());
    EXPECT_STREQ(specimen.getLocation().FileName.getUtf8Bytes(), here.FileName.getUtf8Bytes());
    EXPECT_EQ(specimen.getLocation().LineNo, here.LineNo);
    EXPECT_EQ(specimen.getLocation().Offset, here.Offset);
    EXPECT_STREQ(specimen.getValue().getUtf8Bytes(), "42");
}

GTEST_TEST(Token, AddProperty)
{
    Location here(__FILE__);
    Token specimen(here, TokenClass::OpenBrace);

    specimen.addProperty(TokenProperty::IntRadix, 10);
    specimen.addProperty(TokenProperty::DirectiveType, AssemblyDirectiveType::AddressMode);

    int radix = -1;
    ASSERT_TRUE(specimen.tryGetProperty(TokenProperty::IntRadix, radix));
    EXPECT_EQ(radix, 10);

    AssemblyDirectiveType directiveType = AssemblyDirectiveType::Include;
    ASSERT_TRUE(specimen.tryGetProperty(TokenProperty::DirectiveType, directiveType));
    EXPECT_EQ(directiveType, AssemblyDirectiveType::AddressMode);

    EXPECT_FALSE(specimen.tryGetProperty(TokenProperty::RegisterIndex, radix));
}

GTEST_TEST(Token, AddBooleanProperty)
{
    Location here(__FILE__);
    Token specimen(here, TokenClass::MnemonicAluOp);

    specimen.addProperty(TokenProperty::UserPrivilage, true);
    specimen.addProperty(TokenProperty::UpdatePsr, false);

    bool value;
    ASSERT_TRUE(specimen.tryGetProperty(TokenProperty::UserPrivilage, value));
    EXPECT_TRUE(value);
    ASSERT_TRUE(specimen.tryGetProperty(TokenProperty::UpdatePsr, value));
    EXPECT_FALSE(value);
    EXPECT_FALSE(specimen.tryGetProperty(TokenProperty::DataType, value));

    EXPECT_TRUE(specimen.getProperty(TokenProperty::ConditionCode, true));
    EXPECT_FALSE(specimen.getProperty(TokenProperty::ConditionCode, false));
    EXPECT_TRUE(specimen.getProperty(TokenProperty::UserPrivilage, false));
    EXPECT_TRUE(specimen.getProperty(TokenProperty::UserPrivilage, true));
    EXPECT_FALSE(specimen.getProperty(TokenProperty::UpdatePsr, true));
    EXPECT_FALSE(specimen.getProperty(TokenProperty::UpdatePsr, false));
}

GTEST_TEST(Token, OverwriteProperty)
{
    Location here(__FILE__);
    Token specimen(here, TokenClass::OpenBrace);

    specimen.addProperty(TokenProperty::DirectiveType, AssemblyDirectiveType::AddressMode);

    AssemblyDirectiveType directiveType = AssemblyDirectiveType::Include;
    ASSERT_TRUE(specimen.tryGetProperty(TokenProperty::DirectiveType, directiveType));
    EXPECT_EQ(directiveType, AssemblyDirectiveType::AddressMode);

    specimen.addProperty(TokenProperty::DirectiveType, AssemblyDirectiveType::InstructionSet);
    ASSERT_TRUE(specimen.tryGetProperty(TokenProperty::DirectiveType, directiveType));
    EXPECT_EQ(directiveType, AssemblyDirectiveType::InstructionSet);
}

} // TED

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


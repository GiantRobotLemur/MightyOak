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

namespace Mo {
namespace Asm {

namespace {
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

    addTokenScalar(specimen, TokenProperty::IntRadix, 10);
    addTokenEnum(specimen, TokenProperty::DirectiveType, AssemblyDirectiveType::AddressMode);

    int radix = getTokenScalar(specimen, TokenProperty::IntRadix, 16);
    EXPECT_EQ(radix, 10);

    AssemblyDirectiveType directiveType = AssemblyDirectiveType::Include;
    ASSERT_TRUE(tryGetTokenEnum(specimen, TokenProperty::DirectiveType, directiveType));
    EXPECT_EQ(directiveType, AssemblyDirectiveType::AddressMode);

    CoreRegister reg = getTokenEnum(specimen, TokenProperty::RegisterIndex, CoreRegister::SPSR);
    EXPECT_EQ(reg, CoreRegister::SPSR);
}

GTEST_TEST(Token, AddBooleanProperty)
{
    Location here(__FILE__);
    Token specimen(here, TokenClass::MnemonicAluOp);

    addTokenFlag(specimen, TokenProperty::UserPrivilage, true);
    addTokenFlag(specimen, TokenProperty::UpdatePsr, false);

    EXPECT_TRUE(getTokenFlag(specimen, TokenProperty::UserPrivilage, false));
    EXPECT_FALSE(getTokenFlag(specimen, TokenProperty::UpdatePsr, true));

    EXPECT_TRUE(getTokenFlag(specimen, TokenProperty::ConditionCode, true));
    EXPECT_FALSE(getTokenFlag(specimen, TokenProperty::ConditionCode, false));
    EXPECT_TRUE(getTokenFlag(specimen, TokenProperty::UserPrivilage, false));
    EXPECT_TRUE(getTokenFlag(specimen, TokenProperty::UserPrivilage, true));
    EXPECT_FALSE(getTokenFlag(specimen, TokenProperty::UpdatePsr, true));
    EXPECT_FALSE(getTokenFlag(specimen, TokenProperty::UpdatePsr, false));
}

GTEST_TEST(Token, OverwriteProperty)
{
    Location here(__FILE__);
    Token specimen(here, TokenClass::OpenBrace);

    addTokenEnum(specimen, TokenProperty::DirectiveType, AssemblyDirectiveType::AddressMode);

    AssemblyDirectiveType directiveType = AssemblyDirectiveType::Include;
    ASSERT_TRUE(tryGetTokenEnum(specimen, TokenProperty::DirectiveType, directiveType));
    EXPECT_EQ(directiveType, AssemblyDirectiveType::AddressMode);

    addTokenEnum(specimen, TokenProperty::DirectiveType, AssemblyDirectiveType::InstructionSet);
    ASSERT_TRUE(tryGetTokenEnum(specimen, TokenProperty::DirectiveType, directiveType));
    EXPECT_EQ(directiveType, AssemblyDirectiveType::InstructionSet);
}

} // Anonymous namespace

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


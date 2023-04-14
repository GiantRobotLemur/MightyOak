//! @file Test_AssemblyLabel.cpp
//! @brief The definition of unit tests for assembling labels.
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

#include "TestTools.hpp"

namespace Ag {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(AssemblyLabel, PreDefinedLabel)
{
    Messages log;
    String source = "EQUD 0xCAFEBABE\n"
        ".myLabel\n"
        "EQUD myLabel";
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 8u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0xCAFEBABE);
    EXPECT_EQ(codeWords[1], 0x8004u);

    auto pos = code.getSymbols().find("myLabel");

    ASSERT_NE(pos, code.getSymbols().end());
    EXPECT_EQ(pos->second, 0x8004u);
}

GTEST_TEST(AssemblyLabel, PostDefinedLabel)
{
    Messages log;
    String source = "EQUD 0xCAFEBABE\n"
        "EQUD myLabel\n"
        "EQUS 'Hello World!',13,10\n"
        "ALIGN\n"
        ".myLabel\n";
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 24u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0xCAFEBABE);
    EXPECT_EQ(codeWords[1], 0x8018u);

    auto pos = code.getSymbols().find("myLabel");

    ASSERT_NE(pos, code.getSymbols().end());
    EXPECT_EQ(pos->second, 0x8018u);
}

GTEST_TEST(AssemblyLabel, DuplicateLabelCausesError)
{
    Messages log;
    String source = "EQUD 0xCAFEBABE\n"
        ".myLabel: EQUD myLabel\n"
        ".myLabel\n";
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 8u);
    EXPECT_TRUE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0xCAFEBABE);
    EXPECT_EQ(codeWords[1], 0x8004u);
}

GTEST_TEST(AssemblyLabel, PreDefinedLabelWithValue)
{
    Messages log;
    String source = ".myLabel 0xCAFEBABE\n"
        "EQUD myLabel";
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 4u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0xCAFEBABE);

    // Ensure the symbol is not defined, as it was given an arbitrary value.
    auto pos = code.getSymbols().find("myLabel");
    ASSERT_EQ(pos, code.getSymbols().end());
}

GTEST_TEST(AssemblyLabel, PostDefinedLabelWithValue)
{
    Messages log;
    String source = "EQUS myLabel\n"
        ".myLabel 'Hello World!\\0'\n";
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 13u);
    EXPECT_FALSE(log.hasErrors());

    const char *codeData = reinterpret_cast<const char *>(code.getCode());

    EXPECT_STREQ(codeData, "Hello World!");

    auto pos = code.getSymbols().find("myLabel");
    ASSERT_EQ(pos, code.getSymbols().end());
}

} // Anonymous namespace

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


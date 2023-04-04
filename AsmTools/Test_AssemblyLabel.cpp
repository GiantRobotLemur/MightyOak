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
}

GTEST_TEST(AssemblyLabel, PostDefinedLabel)
{
    Messages log;
    String source = "EQUD 0xCAFEBABE\n"
        "EQUD myLabel\n"
        ".myLabel\n";
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 8u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0xCAFEBABE);
    EXPECT_EQ(codeWords[1], 0x8008u);
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

} // TED

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


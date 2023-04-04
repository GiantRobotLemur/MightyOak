//! @file Test_DataDirective.cpp
//! @brief The definition of unit tests for the assembly of data directives.
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

#include "AsmTools.hpp"
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
GTEST_TEST(DataDirective, SingleDword)
{
    Messages log;
    ObjectCode code = assembleText("EQUD 0xDEADBEEF", getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 4u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(*codeWords, 0xDEADBEEF);
}

GTEST_TEST(DataDirective, MultipleDwordExprs)
{
    Messages log;
    ObjectCode code = assembleText("EQUD 1 + 2 + 3, %10011", getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 8u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 6u);
    EXPECT_EQ(codeWords[1], 19u);
}

GTEST_TEST(DataDirective, MultipleStatementsInline)
{
    Messages log;
    ObjectCode code = assembleText("EQUD 1 + 2 + 3: EQUD %10011",
                                   getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 8u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 6u);
    EXPECT_EQ(codeWords[1], 19u);
}

GTEST_TEST(DataDirective, MultipleStatementsOnMultipleLines)
{
    Messages log;
    ObjectCode code = assembleText("EQUD 1 + 2 + 3\nEQUD %10011",
                                   getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 8u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 6u);
    EXPECT_EQ(codeWords[1], 19u);
}

GTEST_TEST(DataDirective, CurrentPosition)
{
    Messages log;
    ObjectCode code = assembleText("EQUD $, $, $ ; Try using the current position",
                                   getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 12u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0x8000u);
    EXPECT_EQ(codeWords[1], 0x8004u);
    EXPECT_EQ(codeWords[2], 0x8008u);
}

GTEST_TEST(DataDirective, CurrentPositionExpression)
{
    Messages log;
    ObjectCode code = assembleText("EQUD ($ + 8) OR &F0000000",
                                   getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 4u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0xF0008008u);
}

GTEST_TEST(DataDirective, AlignDefault)
{
    Messages log;
    ObjectCode code = assembleText("EQUB 1, 2, 3 : ALIGN",
                                   getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 4u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0x00030201u);
}

GTEST_TEST(DataDirective, AlignValue)
{
    Messages log;
    ObjectCode code = assembleText("EQUB 1, 2, 3 : ALIGN 16",
                                   getDefaultOptions(), log);

    ASSERT_FALSE(code.isEmpty());
    ASSERT_EQ(code.getCodeSize(), 16u);
    EXPECT_FALSE(log.hasErrors());
    const uint32_t *codeWords = reinterpret_cast<const uint32_t *>(code.getCode());

    EXPECT_EQ(codeWords[0], 0x00030201u);
    EXPECT_EQ(codeWords[1], 0x00000000u);
    EXPECT_EQ(codeWords[2], 0x00000000u);
    EXPECT_EQ(codeWords[3], 0x00000000u);
}

} // TED

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


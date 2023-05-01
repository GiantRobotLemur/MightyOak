//! @file Test_Expr.cpp
//! @brief The definition of unit test for the Expr and derived classes.
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

#include "ConstantSet.hpp"
#include "Expr.hpp"
#include "SymbolTable.hpp"
#include "TestTools.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Expr, SimpleConstants)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("42", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 42u);

    EXPECT_TRUE(evalStaticConstExpr("1.125", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Float);
    EXPECT_EQ(result.asFloat(), 1.125f);

    EXPECT_TRUE(evalStaticConstExpr("3.14159", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Double);
    EXPECT_EQ(result.asDouble(), 3.14159);

    EXPECT_TRUE(evalStaticConstExpr("'H'", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Character);
    EXPECT_EQ(result.asCharacter(), U'H');

    EXPECT_TRUE(evalStaticConstExpr("'Hello World!'", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::String);
    EXPECT_STRINGEQC(result.asString(), "Hello World!");
}

GTEST_TEST(Expr, NegativeConstants)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("-42", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), -42);

    EXPECT_TRUE(evalStaticConstExpr("-1.125", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Float);
    EXPECT_EQ(result.asFloat(), -1.125f);

    EXPECT_TRUE(evalStaticConstExpr("-3.14159", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Double);
    EXPECT_EQ(result.asDouble(), -3.14159);

    EXPECT_FALSE(evalStaticConstExpr("-'H'", empty, result));

    EXPECT_FALSE(evalStaticConstExpr("-'Hello World!'", empty, result));
}

GTEST_TEST(Expr, AbsoluteConstants)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("+(-42)", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 42);

    EXPECT_TRUE(evalStaticConstExpr("+(-1.125)", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Float);
    EXPECT_EQ(result.asFloat(), 1.125f);

    EXPECT_TRUE(evalStaticConstExpr("+(-3.14159)", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Double);
    EXPECT_EQ(result.asDouble(), 3.14159);

    EXPECT_FALSE(evalStaticConstExpr("+'H'", empty, result));

    EXPECT_FALSE(evalStaticConstExpr("+'Hello World!'", empty, result));
}

GTEST_TEST(Expr, LogicalNotConstants)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("NOT 0xFF", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 0xFFFFFF00);

    EXPECT_TRUE(evalStaticConstExpr("NOT 0xF00000000", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint64);
    EXPECT_EQ(result.asUint64(), 0xFFFFFFF0FFFFFFFFull);

    EXPECT_FALSE(evalStaticConstExpr("NOT 1.125", empty, result));

    EXPECT_FALSE(evalStaticConstExpr("NOT 3.14159", empty, result));

    EXPECT_FALSE(evalStaticConstExpr("NOT 'H'", empty, result));

    EXPECT_FALSE(evalStaticConstExpr("NOT 'Hello World!'", empty, result));
}

GTEST_TEST(Expr, ConstantAddition)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("42 + 18", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 60u);

    EXPECT_TRUE(evalStaticConstExpr("1.125 + 8.875", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Float);
    EXPECT_EQ(result.asFloat(), 10.0f);

    EXPECT_TRUE(evalStaticConstExpr("3.14159 + 6.85841", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Double);
    EXPECT_EQ(result.asDouble(), 10.0);

    EXPECT_TRUE(evalStaticConstExpr("'H' + 'i'", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::String);
    EXPECT_STRINGEQC(result.asString(), "Hi");

    EXPECT_TRUE(evalStaticConstExpr("'Hello' + ' World!'", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::String);
    EXPECT_STRINGEQC(result.asString(), "Hello World!");
}


GTEST_TEST(Expr, ConstantSubtraction)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("50 - 18", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 32u);

    EXPECT_TRUE(evalStaticConstExpr("10.0 - 8.875", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Float);
    EXPECT_EQ(result.asFloat(), 1.125f);

    EXPECT_TRUE(evalStaticConstExpr("10 - 6.85841", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Double);
    EXPECT_EQ(result.asDouble(), 3.14159);

    EXPECT_FALSE(evalStaticConstExpr("'H' - 'i'", empty, result));

    EXPECT_FALSE(evalStaticConstExpr("'Hello' - ' World!'", empty, result));
}

GTEST_TEST(Expr, ConstantMultiplication)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 * -11", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), -55);

    EXPECT_TRUE(evalStaticConstExpr("2.25 * 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Float);
    EXPECT_EQ(result.asFloat(), 4.5f);

    EXPECT_TRUE(evalStaticConstExpr("21.1 * 20.0", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Double);
    EXPECT_EQ(result.asDouble(), 422.0);

    EXPECT_FALSE(evalStaticConstExpr("'H' * 'i'", empty, result));

    EXPECT_FALSE(evalStaticConstExpr("'Hello' * ' World!'", empty, result));
}

GTEST_TEST(Expr, ConstantDivision)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 / -11", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 0);

    EXPECT_TRUE(evalStaticConstExpr("2.25 / 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Float);
    EXPECT_EQ(result.asFloat(), 1.125f);

    EXPECT_TRUE(evalStaticConstExpr("422.1 / 20.0", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Double);
    EXPECT_EQ(result.asDouble(), 21.105);

    EXPECT_FALSE(evalStaticConstExpr("'H' / 'i'", empty, result));

    EXPECT_FALSE(evalStaticConstExpr("'Hello' / ' World!'", empty, result));
}

GTEST_TEST(Expr, DivisionByZero)
{
    ConstantSet empty;
    Value result;

    EXPECT_FALSE(evalStaticConstExpr("5 / 0", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("-11 / 0", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("2.25 / 0", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("422.1253 / 0.0", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'H' / 0.0", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'Hello' / 0", empty, result));
}

GTEST_TEST(Expr, ConstantModulus)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 MOD 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 1u);

    EXPECT_TRUE(evalStaticConstExpr("5 MOD -2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 1);

    EXPECT_FALSE(evalStaticConstExpr("2.25 MOD 2", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("422.1 MOD 20.0", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'H' MOD 'i'", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'Hello' MOD ' World!'", empty, result));
}

GTEST_TEST(Expr, ConstantLsl)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 << 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 20u);

    EXPECT_TRUE(evalStaticConstExpr("5 << -2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 1u);

    EXPECT_TRUE(evalStaticConstExpr("5 << 0", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 5u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 << 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 20u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 << -2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 1u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 << 0", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 5u);

    EXPECT_FALSE(evalStaticConstExpr("2.25 << 3", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("422.1 << 9", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'H' << 4", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'Hello' << 12.5", empty, result));
}

GTEST_TEST(Expr, ConstantLsr)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 >> -2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 20u);

    EXPECT_TRUE(evalStaticConstExpr("5 >> 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 1u);

    EXPECT_TRUE(evalStaticConstExpr("5 >> 0", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 5u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 >> -2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 20u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 >> 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 1u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 >> 0", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 5u);

    EXPECT_FALSE(evalStaticConstExpr("2.25 >> 3", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("422.1 >> 9", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'H' >> 4", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'Hello' >> 12.5", empty, result));
}

GTEST_TEST(Expr, ConstantAsr)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 >>> -2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 20);

    EXPECT_TRUE(evalStaticConstExpr("5 >>> 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 1);

    EXPECT_TRUE(evalStaticConstExpr("5 >>> 0", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 5);

    EXPECT_TRUE(evalStaticConstExpr("-5 >>> -2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), -20);

    EXPECT_TRUE(evalStaticConstExpr("-5 >>> 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), -2);

    EXPECT_TRUE(evalStaticConstExpr("-5 >>> 0", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), -5);

    EXPECT_FALSE(evalStaticConstExpr("2.25 >>> 3", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("422.1 >>> 9", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'H' >>> 4", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'Hello' >>> 12.5", empty, result));
}

GTEST_TEST(Expr, ConstantAnd)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 AND 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 0u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 AND 4", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 4);

    EXPECT_FALSE(evalStaticConstExpr("2.25 AND 3", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("422.1 AND 9", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'H'AND 4", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'Hello' AND 12.5", empty, result));
}

GTEST_TEST(Expr, ConstantOr)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 or 2", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 7u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 OR 4", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 5);

    EXPECT_FALSE(evalStaticConstExpr("2.25 OR 3", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("422.1 oR 9", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'H'OR 4", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'Hello' Or 12.5", empty, result));
}

GTEST_TEST(Expr, ConstantXor)
{
    ConstantSet empty;
    Value result;

    EXPECT_TRUE(evalStaticConstExpr("5 XOR 3", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 6u);

    EXPECT_TRUE(evalStaticConstExpr("+-5 EOR 4", empty, result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 1);

    EXPECT_FALSE(evalStaticConstExpr("2.25 eor 3", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("422.1 xOr 9", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'H'EoR 4", empty, result));
    EXPECT_FALSE(evalStaticConstExpr("'Hello' xor 12.5", empty, result));
}

} // Anonymous namespace

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


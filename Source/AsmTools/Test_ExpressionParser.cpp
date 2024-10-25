//! @file Test_ExpressionParser.cpp
//! @brief The definition of unit test to test expression parsing.
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

#include "TestTools.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(ExpressionParser, ParseBinaryIntegerLiteral)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("\\ ; My number\n %0101101", expr));
    ASSERT_TRUE(expr.isComplete());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::IntegerLiteral);

    const IntegerLiteralNode *valueNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), valueNode));
    EXPECT_EQ(valueNode->getRadix(), 2);
    EXPECT_STRINGEQC(valueNode->getValue(), "0101101");
}

GTEST_TEST(ExpressionParser, ParseHexIntegerLiteral)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("\t0xDEADBEEF", expr));
    ASSERT_TRUE(expr.isComplete());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::IntegerLiteral);

    const IntegerLiteralNode *valueNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), valueNode));
    EXPECT_EQ(valueNode->getRadix(), 16);
    EXPECT_STRINGEQC(valueNode->getValue(), "DEADBEEF");
}

GTEST_TEST(ExpressionParser, ParseDecimalIntegerLiteral)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  426991 \\ \n", expr));
    ASSERT_TRUE(expr.isComplete());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::IntegerLiteral);

    const IntegerLiteralNode *valueNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), valueNode));
    EXPECT_EQ(valueNode->getRadix(), 10);
    EXPECT_STRINGEQC(valueNode->getValue(), "426991");
}

GTEST_TEST(ExpressionParser, ParseNumericLiteral)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr(" 3.14159627390   ; Pi ", expr));
    ASSERT_TRUE(expr.isComplete());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::RealLiteral);

    const RealLiteralNode *numNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), numNode));
    EXPECT_STRINGEQC(numNode->getValue(), "3.14159627390");
}

GTEST_TEST(ExpressionParser, ParseStringLiteral)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("'Hello World!'", expr));
    ASSERT_TRUE(expr.isComplete());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::StringLiteral);

    const StringLiteralNode *strNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), strNode));
    EXPECT_STRINGEQC(strNode->getValue(), "Hello World!");
}

GTEST_TEST(ExpressionParser, ParseSymbol)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  MyValue\t \\", expr));
    ASSERT_TRUE(expr.isComplete());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::Symbol);

    const SymbolNode *symbolNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), symbolNode));
    EXPECT_STRINGEQC(symbolNode->getId(), "MyValue");
}

GTEST_TEST(ExpressionParser, ParseCurrentOffset)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("\t$ ", expr));
    ASSERT_TRUE(expr.isComplete());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::CurrentOffset);
}

GTEST_TEST(ExpressionParser, ParseMatchingParenthesis)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  ( \t 42 \\ \n )", expr));
    ASSERT_TRUE(expr.isComplete());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::Parenthesis);

    const ParenthesisNode *parenthesisNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), parenthesisNode));
    EXPECT_TRUE(parenthesisNode->isComplete());
    EXPECT_TRUE(parenthesisNode->isValid());

    const IntegerLiteralNode *valueNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(parenthesisNode->getChildExpr(), valueNode));
    EXPECT_EQ(valueNode->getRadix(), 10);
    EXPECT_STRINGEQC(valueNode->getValue(), "42");
}

GTEST_TEST(ExpressionParser, ParseEmptyParenthesis)
{
    ExpressionContainer expr;

    EXPECT_FALSE(parseStaticExpr("  ( ) ", expr));
    EXPECT_TRUE(expr.isComplete());
    EXPECT_FALSE(expr.isValid());
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::Parenthesis);

    const ParenthesisNode *parenthesisNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), parenthesisNode));
    EXPECT_TRUE(parenthesisNode->isComplete());
    EXPECT_EQ(parenthesisNode->getChildExpr(), nullptr);
    EXPECT_FALSE(parenthesisNode->isValid());
}

GTEST_TEST(ExpressionParser, ParseMismatchingParenthesis)
{
    ExpressionContainer expr;

    EXPECT_FALSE(parseStaticExpr("  ('Hello?' ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::Parenthesis);

    const ParenthesisNode *parenthesisNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), parenthesisNode));
    EXPECT_TRUE(parenthesisNode->isComplete());
    ASSERT_NE(parenthesisNode->getChildExpr(), nullptr);
    EXPECT_TRUE(parenthesisNode->isValid());
}

GTEST_TEST(ExpressionParser, ParseNegativeOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  -42 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::UnaryOperator);

    const UnaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), UnaryOperatorType::Minus);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getChildExpr(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const IntegerLiteralNode *valueNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getChildExpr(), valueNode));
    EXPECT_STRINGEQC(valueNode->getValue(), "42");
}

GTEST_TEST(ExpressionParser, ParseOrphanNegativeOperator)
{
    ExpressionContainer expr;

    EXPECT_FALSE(parseStaticExpr("  - ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::UnaryOperator);

    const UnaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), UnaryOperatorType::Minus);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_EQ(opNode->getChildExpr(), nullptr);
    EXPECT_FALSE(opNode->isValid());
}

GTEST_TEST(ExpressionParser, ParseAbsoluteOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  +  69 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::UnaryOperator);

    const UnaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), UnaryOperatorType::Absolute);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getChildExpr(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const IntegerLiteralNode *valueNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getChildExpr(), valueNode));
    EXPECT_STRINGEQC(valueNode->getValue(), "69");
}

GTEST_TEST(ExpressionParser, ParseOrphanAbsoluteOperator)
{
    ExpressionContainer expr;

    EXPECT_FALSE(parseStaticExpr("  + ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::UnaryOperator);

    const UnaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), UnaryOperatorType::Absolute);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_EQ(opNode->getChildExpr(), nullptr);
    EXPECT_FALSE(opNode->isValid());
}

GTEST_TEST(ExpressionParser, ParseNotOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  NoT 0 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::UnaryOperator);

    const UnaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), UnaryOperatorType::LogicalNot);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getChildExpr(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const IntegerLiteralNode *valueNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getChildExpr(), valueNode));
    EXPECT_STRINGEQC(valueNode->getValue(), "0");
}

GTEST_TEST(ExpressionParser, ParseOrphanNotOperator)
{
    ExpressionContainer expr;

    EXPECT_FALSE(parseStaticExpr("  NOT ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::UnaryOperator);

    const UnaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), UnaryOperatorType::LogicalNot);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_EQ(opNode->getChildExpr(), nullptr);
    EXPECT_FALSE(opNode->isValid());
}

GTEST_TEST(ExpressionParser, ParseAdditionOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  42 + 69 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Addition);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const IntegerLiteralNode *lhsNode = nullptr, *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "42");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "69");
}

GTEST_TEST(ExpressionParser, ParseSubtractionOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  'Hello' - 3.14159 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Subtraction);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const StringLiteralNode *lhsNode = nullptr;
    const RealLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "Hello");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "3.14159");
}

GTEST_TEST(ExpressionParser, ParseMultiplyOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  %101101 * 'digits' ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Multiplication);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const IntegerLiteralNode *lhsNode = nullptr;
    const StringLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "101101");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "digits");
}

GTEST_TEST(ExpressionParser, ParseDivisionOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  0xCAFEBABE / 0.0 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Division);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const IntegerLiteralNode *lhsNode = nullptr;
    const RealLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "CAFEBABE");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "0.0");
}

GTEST_TEST(ExpressionParser, ParseModulusOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  12.5 MOD 3 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Modulus);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const RealLiteralNode *lhsNode = nullptr;
    const IntegerLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "12.5");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "3");
}

GTEST_TEST(ExpressionParser, ParseAndOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  'A' AND &DF ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::And);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const StringLiteralNode *lhsNode = nullptr;
    const IntegerLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "A");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "DF");
}

GTEST_TEST(ExpressionParser, ParseOrOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  0xFFED or MyValue ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Or);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const IntegerLiteralNode *lhsNode = nullptr;
    const SymbolNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "FFED");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getId(), "MyValue");
}

GTEST_TEST(ExpressionParser, ParseXorOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  \"Hi\" xOR 'There' ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Xor);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const StringLiteralNode *lhsNode = nullptr;
    const StringLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "Hi");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "There");
}

GTEST_TEST(ExpressionParser, ParseEorOperator)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  \"My\" eor 'F00t' ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Xor);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const StringLiteralNode *lhsNode = nullptr;
    const StringLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "My");
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "F00t");
}

GTEST_TEST(ExpressionParser, UnaryOpPrecedenceInversion)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  -5 + 11 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Addition);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const UnaryOperatorNode *lhsNode = nullptr;
    const IntegerLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_EQ(lhsNode->getOperatorType(), UnaryOperatorType::Minus);
    ASSERT_NE(lhsNode->getChildExpr(), nullptr);

    const IntegerLiteralNode *childNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(lhsNode->getChildExpr(), childNode));
    EXPECT_STRINGEQC(childNode->getValue(), "5");

    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "11");
}

GTEST_TEST(ExpressionParser, BinaryOpLToRPrecedence)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  42 + 9 * 11 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Addition);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const IntegerLiteralNode *lhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_STRINGEQC(lhsNode->getValue(), "42");

    const BinaryOperatorNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_EQ(rhsNode->getOperatorType(), BinaryOperatorType::Multiplication);
    ASSERT_NE(rhsNode->getLeftChild(), nullptr);
    ASSERT_NE(rhsNode->getRightChild(), nullptr);

    const IntegerLiteralNode *lhsGrandChild = nullptr;
    const IntegerLiteralNode *rhsGrandChild = nullptr;

    ASSERT_TRUE(Ag::tryCast(rhsNode->getLeftChild(), lhsGrandChild));
    EXPECT_STRINGEQC(lhsGrandChild->getValue(), "9");
    ASSERT_TRUE(Ag::tryCast(rhsNode->getRightChild(), rhsGrandChild));
    EXPECT_STRINGEQC(rhsGrandChild->getValue(), "11");
}

GTEST_TEST(ExpressionParser, BinaryOpPrecedenceInversion)
{
    ExpressionContainer expr;

    EXPECT_TRUE(parseStaticExpr("  42 * 9 + 11 ", expr));
    ASSERT_NE(expr.getExpression(), nullptr);
    EXPECT_EQ(expr.getExpression()->getExprType(), ExpressionType::BinaryOperator);

    const BinaryOperatorNode *opNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(expr.getExpression(), opNode));
    EXPECT_EQ(opNode->getOperatorType(), BinaryOperatorType::Addition);
    EXPECT_TRUE(opNode->isComplete());
    ASSERT_NE(opNode->getLeftChild(), nullptr);
    ASSERT_NE(opNode->getRightChild(), nullptr);
    EXPECT_TRUE(opNode->isValid());

    const BinaryOperatorNode *lhsNode = nullptr;
    const IntegerLiteralNode *rhsNode = nullptr;
    ASSERT_TRUE(Ag::tryCast(opNode->getLeftChild(), lhsNode));
    EXPECT_EQ(lhsNode->getOperatorType(), BinaryOperatorType::Multiplication);
    ASSERT_NE(lhsNode->getLeftChild(), nullptr);
    ASSERT_NE(lhsNode->getRightChild(), nullptr);

    const IntegerLiteralNode *lhsGrandChild = nullptr;
    const IntegerLiteralNode *rhsGrandChild = nullptr;

    ASSERT_TRUE(Ag::tryCast(lhsNode->getLeftChild(), lhsGrandChild));
    EXPECT_STRINGEQC(lhsGrandChild->getValue(), "42");
    ASSERT_TRUE(Ag::tryCast(lhsNode->getRightChild(), rhsGrandChild));
    EXPECT_STRINGEQC(rhsGrandChild->getValue(), "9");

    ASSERT_TRUE(Ag::tryCast(opNode->getRightChild(), rhsNode));
    EXPECT_STRINGEQC(rhsNode->getValue(), "11");
}

} // Anonymous namespace

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


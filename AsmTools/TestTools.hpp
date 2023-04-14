//! @file TestTools.hpp
//! @brief The declaration of various tools shared between unit tests of the
//! AsmTools library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_TEST_TOOLS_HPP__
#define __ASM_TOOLS_TEST_TOOLS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "AsmTools.hpp"
#include "ParseContext.hpp"
#include "ExpressionNode.hpp"
#include "Value.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief A macro which captures the current source code location.
#define LOC TestLocation(__FILE__, __LINE__)

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which can contain a single expression.
class ExpressionContainer : public ExpressionNode
{
public:
    // Construction/Destruction
    ExpressionContainer();
    virtual ~ExpressionContainer() = default;

    // Accessors
    const ExpressionNode *getExpression() const;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual IExpr *compile(const ConstantSet &constants) const override;
private:
    // Internal Fields
    ExpressionNodeUPtr _expr;
    bool _isComplete;
};

//! @brief Captures a location in source code.
struct TestLocation
{
    const char *File;
    int Line;

    TestLocation();
    TestLocation(const char *file, int line);
};

//! @brief A base class for the parameters required for a dynamically
//! registered test.
class BaseTestPoint
{
public:
    // Construction/Destruction
    BaseTestPoint(const TestLocation &loc, const char *name);
    ~BaseTestPoint() = default;

    // Accessors
    const TestLocation &getLocation() const;
    string_cref_t getName() const;
private:
    // Internal Fields
    TestLocation _loc;
    String _name;
};

//! @brief The base class for dynamically registered tests.
class BaseTestFixture : public testing::Test
{
public:
    // Construction/Destruction
    BaseTestFixture(const char *suiteName);
    virtual ~BaseTestFixture() = default;

    // Accessors
    string_cref_t getSuiteName() const;
private:
    // Internal Fields
    String _suiteName;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
InputContext createInput(const char *sourceCode);
::testing::AssertionResult parseStaticStatements(const char *sourceCode,
                                                 ISyntaxNodePtr rootNode);
::testing::AssertionResult parseStaticExpr(const char *sourceCode,
                                           ExpressionContainer &rootNode);
::testing::AssertionResult evalStaticConstExpr(const char *sourceCode,
                                               const ConstantSet &constants,
                                               Value &result);
::testing::AssertionResult equalHex(uint32_t lhs, uint32_t rhs);


void appendLog(::testing::AssertionResult &result, const Messages &log);
::testing::AssertionResult compareWords(uint32_t lhs, uint32_t rhs);
::testing::AssertionResult assemblySuccess(const Messages &log);

const Options &getDefaultOptions();
void registerInstructionTests();
void registerAssemblyTests();

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Allows tests to be dynamically registered.
//! @tparam TTest The data type of the BaseTestFixture-derived fixture class.
//! @tparam TPoint The data type of the BaseTestPoint-derived test parameters class
//! with which TTest object instances are constructed.
//! @param[in] suiteName The name of the test suite to assign the tests to.
//! @param[in] testPoints A pointer to the array of test points to register.
//! @param[in] count The count of elements in testPoints.
template<typename TTest, typename TPoint>
void registerTestPoints(const char *suiteName, const TPoint *testPoints, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        const auto &point = testPoints[i];

        testing::RegisterTest(
            suiteName,                          // Suite name
            point.getName().getUtf8Bytes(),     // Test name
            nullptr,                            // Type parameter
            nullptr,                            // Value name
            point.getLocation().File, point.getLocation().Line,
            // Important to use the fixture type as the return type here.
            [=]() -> BaseTestFixture *{ return new TTest(point); });
    }
}

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

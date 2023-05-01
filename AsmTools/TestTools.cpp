//! @file TestTools.cpp
//! @brief The definition of various functions shared between unit tests of
//! the AsmTools library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "TestTools.hpp"
#include "Expr.hpp"
#include "LexicalAnalysers.hpp"
#include "Parser.hpp"
#include "SymbolTable.hpp"
#include "SyntaxNode.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void logMessages(testing::AssertionResult &result, const Messages &messages)
{
    bool isFirst = true;
    for (const auto &message : messages.getMessages())
    {
        if (isFirst)
        {
            isFirst = false;
        }
        else
        {
            result << std::endl;
        }

        switch (message.getSeverity())
        {
        case MessageSeverity::Info:
            result << "Info: ";
            break;

        case MessageSeverity::Warning:
            result << "Warning: ";
            break;

        case MessageSeverity::Error:
            result << "Error: ";
            break;

        case MessageSeverity::Fatal:
            result << "Fatal: ";
            break;

        default:
            result << "Message: ";
            break;
        }

        result << message.getMessage().getUtf8Bytes();
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// ExpressionContainer Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
ExpressionContainer::ExpressionContainer() :
    ExpressionNode(Location(), ExpressionType::Parenthesis),
    _isComplete(false)
{
}

const ExpressionNode *ExpressionContainer::getExpression() const { return _expr.get(); }

// Inherited from ISyntaxNode.
bool ExpressionContainer::isComplete() const { return _isComplete; }

// Inherited from ISyntaxNode.
bool ExpressionContainer::isValid() const
{
    return _expr ? _expr->isValid() : false;
}

// Inherited from ISyntaxNode.
ISyntaxNode *ExpressionContainer::applyToken(ParseContext &context,
                                             const Token &token)
{
    if (token.getClass() == TokenClass::StatementTerminator)
    {
        if (!_expr)
        {
            context.getMessages().appendError(token.getLocation(),
                                              "Unexpected end of statement.");
        }

        _isComplete = true;

        return this;
    }
    else
    {
        return nullptr;
    }
}

// Inherited from ISyntaxNode.
ISyntaxNode *ExpressionContainer::applyNode(ParseContext & /* context */,
                                            ISyntaxNode *childNode)
{
    ExpressionNodePtr childExpr = nullptr;

    if (!_expr && Ag::tryCast(childNode, childExpr))
    {
        _expr.reset(childExpr);
        return this;
    }
    else
    {
        return nullptr;
    }
}

// Inherited from ISyntaxNode.
void ExpressionContainer::recover(ParseContext & /* context */,
                                  ISyntaxNode *node)
{
    if (node != nullptr)
    {
        // Prevent a memory leak by deleting the node.
        delete node;
    }
}

// Inherited from ExpressionNode.
IExpr *ExpressionContainer::compile(const ConstantSet &constants) const
{
    if (_expr && _expr->isValid())
    {
        return _expr->compile(constants);
    }
    else
    {
        return new ConstantExpr(getPosition(), Value());
    }
}

////////////////////////////////////////////////////////////////////////////////
// TestLocation Member Definitions
////////////////////////////////////////////////////////////////////////////////
TestLocation::TestLocation() :
    File(""),
    Line(-1)
{
}

TestLocation::TestLocation(const char *file, int line) :
    File(file),
    Line(line)
{
}

////////////////////////////////////////////////////////////////////////////////
// BaseTestPoint Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object forming the basis for a test point.
//! @param[in] loc The location of the test definition.
//! @param[in] name The name of the test, unique within its suite.
BaseTestPoint::BaseTestPoint(const TestLocation &loc, const char *name) :
    _loc(loc),
    _name(name)
{
}

//! @brief Gets the source location of the test.
const TestLocation &BaseTestPoint::getLocation() const { return _loc; }

//! @brief Gets the name of the test.
Ag::string_cref_t BaseTestPoint::getName() const { return _name; }

////////////////////////////////////////////////////////////////////////////////
// BaseTestFixture Member Definitions
////////////////////////////////////////////////////////////////////////////////
BaseTestFixture::BaseTestFixture(const char *suiteName) :
    _suiteName(suiteName)
{
}

Ag::string_cref_t BaseTestFixture::getSuiteName() const { return _suiteName; }

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates an input context from a static buffer.
//! @param[in] sourceCode The input source code as a UTF-8 null-terminated
//! character array.
//! @return An object encapsulating the input text ready for parsing.
InputContext createInput(const char *sourceCode)
{
    IInputSourcePtr source = createBufferInputSource(Ag::String(sourceCode));
    Location position("MyProject/Libs/Here.asm");
    Ag::String sourceId("MyProject/Libs/Lib.asm");

    return InputContext(source, position, sourceId, 2);
}

//! @brief Attempts to parse a static statement text.
//! @param[in] sourceCode The source code to parse as a UTF-8 encoded
//! null-terminated character array.
//! @param[in,out] rootNode The root syntax node to update using tokens
//! and nodes parsed from sourceCode.
//! @retval true The source code was parsed with no errors.
//! @retval false Errors were encountered parsing the source.
::testing::AssertionResult parseStaticStatements(const char *sourceCode,
                                                 ISyntaxNodePtr rootNode)
{
    Messages output;
    VisitedSourceSet visitedSources;
    ParseContext parser(visitedSources,
                        createStringInputSet("Primary", sourceCode),
                        output);

    // Engage the parser.
    parseSourceCode(parser, rootNode);

    // Process the results
    auto result = output.hasErrors() ? testing::AssertionFailure() :
        testing::AssertionSuccess();

    logMessages(result, output);

    return result;
}

//! @brief Attempts to parse a static expression text.
//! @param[in] sourceCode The source code to parse as a UTF-8 encoded
//! null-terminated character array.
//! @param[out] rootNode An object or receive and manage the root expression
//! node created using tokens and nodes parsed from sourceCode.
//! @retval true The source code was parsed with no errors.
//! @retval false Errors were encountered parsing the source.
::testing::AssertionResult parseStaticExpr(const char *sourceCode,
                                           ExpressionContainer &rootNode)
{
    Messages output;
    VisitedSourceSet visitedSources;
    ParseContext parser(visitedSources,
                        createStringInputSet("Primary", sourceCode),
                        output);

    Ag::String openError;

    // Setup for parsing expressions from the outset.
    if (parser.tryBegin(getExpressionLexer(), openError))
    {
        // Engage the parser.
        parseSourceCode(parser, &rootNode);

        // Process the results
        auto result = output.hasErrors() ? testing::AssertionFailure() :
                                           testing::AssertionSuccess();

        logMessages(result, output);

        return result;
    }
    else
    {
        auto result = testing::AssertionFailure();

        result << "Failed to open initial input source: ";
        result << openError.getUtf8Bytes();
        result << std::endl;

        return result;
    }
}

::testing::AssertionResult evalStaticConstExpr(const char *sourceCode,
                                               const ConstantSet &constants,
                                               Value &result)
{
    result.setNull();

    ExpressionContainer container;
    testing::AssertionResult status = parseStaticExpr(sourceCode, container);

    if (status && container.isValid() && container.isComplete())
    {
        IExprUPtr expr(container.getExpression()->compile(constants));
        Ag::String error;

        if (expr)
        {
            if (expr->isConstant())
            {
                if (expr->tryConstantEvaluate(result, error) == false)
                {
                    status = testing::AssertionFailure() <<
                        "The expression cannot be evaluated: " <<
                        error.getUtf8Bytes();
                }
            }
            else
            {
                status = testing::AssertionFailure() << "The expression is not constant.";
            }
        }
        else
        {
            status = testing::AssertionFailure() << "Missing syntax tree.";
        }
    }

    return status;
}

::testing::AssertionResult equalHex(uint32_t lhs, uint32_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X8} vs 0x{1:X8}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

//! @brief Appends the entries of an assembly log to the testing output.
//! @param result The testing output to append to.
//! @param log The log containing the entries to append.
void appendLog(::testing::AssertionResult &result, const Messages &log)
{
    bool isFirst = true;
    for (const auto &entry : log.getMessages())
    {
        if (isFirst)
        {
            isFirst = false;
        }
        else
        {
            result << '\n';
        }

        Ag::String entryText = entry.toString();

        result << entryText.getUtf8Bytes();
    }
}

//! @brief Compares 32-bit words, expressing a failure result in hex.
//! @param[in] expected The word value expected.
//! @param[in] found The word value produced by the test.
//! @returns The gtest result, possibly annotated.
::testing::AssertionResult compareWords(uint32_t expected, uint32_t found)
{
    if (expected == found)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        auto failure = ::testing::AssertionFailure();
        std::string builder;

        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "Machine word expected 0x{0:X8} but found 0x{1:X8}.",
                         builder, { expected, found });

        failure << builder;

        return failure;
    }
}

//! @brief Tests to see is assembly was successful and appends messages to the
//! unit test log either way.
//! @param[in] log The log indicating if assembly was successful or not.
//! @return The assertion result.
::testing::AssertionResult assemblySuccess(const Messages &log)
{
    ::testing::AssertionResult result = log.hasErrors() ? ::testing::AssertionFailure() :
                                                          ::testing::AssertionSuccess();

    appendLog(result, log);

    return result;
}

//! @brief Gets some basic assembly settings suitable for an ARM3 assembling at
//! the 32KB address line.
const Options &getDefaultOptions()
{
    static Options opts;

    opts.setInstructionSet(InstructionSet::ArmV2a);
    opts.setLoadAddress(0x8000);

    return opts;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


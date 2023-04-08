//! @file ExpressionNode.cpp
//! @brief The definition of a number of objects representing nested elements
//! of expressions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cerrno>
#include <cstdlib>

#include <algorithm>

#include "ConstantSet.hpp"
#include "Expr.hpp"
#include "ExpressionNode.hpp"
#include "ParseContext.hpp"
#include "SymbolTable.hpp"
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
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the relative precedence for a unary operator.
//! @param[in] op The operator type.
//! @return The precedence of that operator, the higher the better.
int getOperatorPrecidence(UnaryOperatorType op)
{
    static const int precidences[] =
    {
        99, // Minus
        99, // Absolute
        99, // LogicalNot
    };

    static const size_t count = arraySize(precidences);

    static_assert(count == static_cast<size_t>(UnaryOperatorType::Max),
                  "The unary operator precedences need to be updated.");

    return precidences[static_cast<uint32_t>(op)];
}

//! @brief Gets the relative precedence for a binary operator.
//! @param[in] op The operator type.
//! @return The precedence of that operator, the higher the better.
int getOperatorPrecidence(BinaryOperatorType op)
{
    static const int precidences[] =
    {
        70, // Addition
        70, // Subtraction
        80, // Multiplication
        80, // Division
        80, // Modulus
        60, // Lsl
        60, // Lsr
        60, // Asr
        50, // And
        30, // Or
        40, // Xor
    };

    static const size_t count = arraySize(precidences);

    static_assert(count == static_cast<size_t>(BinaryOperatorType::Max),
                  "The binary operator precedences need to be updated.");

    return precidences[static_cast<uint32_t>(op)];
}

//! @brief Determines if a token indicates the end of an expression.
bool isEndOfExpr(const Token &token)
{
    static TokenClass exprEndTypes[] =
    {
        TokenClass::Hash,
        TokenClass::Hat,
        TokenClass::Comma,
        TokenClass::OpenSquare,
        TokenClass::CloseSquare,
        TokenClass::OpenBrace,
        TokenClass::CloseBrace,
        TokenClass::StatementTerminator,
    };

    static bool isSorted = false;

    TokenClass *exprEnd = exprEndTypes + arraySize(exprEndTypes);

    if (isSorted == false)
    {
        std::sort(exprEndTypes, exprEnd);
        isSorted = true;
    }

    return std::binary_search(exprEndTypes, exprEnd, token.getClass());
}

//! @brief Processes a token which follows a complete expression to possibly
//! create a new binary operator of which the original expression forms the
//! left operand.
//! @param[in] node A pointer to the completed expression node.
//! @param[in] token The token which follows the expression.
//! @return Either the original node pointer or a pointer to a newly created
//! partial binary operator expression.
ISyntaxNode *expandExpression(ExpressionNodePtr node, const Token &token)
{
    BinaryOperatorType op = BinaryOperatorType::Addition;
    bool hasType = true;

    switch (token.getClass())
    {
    case TokenClass::Plus: op = BinaryOperatorType::Addition; break;
    case TokenClass::Minus: op = BinaryOperatorType::Subtraction; break;
    case TokenClass::Asterisk: op = BinaryOperatorType::Multiplication; break;
    case TokenClass::Slash: op = BinaryOperatorType::Division; break;
    case TokenClass::KeywordMod: op = BinaryOperatorType::Modulus; break;
    case TokenClass::LogicShiftLeft: op = BinaryOperatorType::Lsl; break;
    case TokenClass::LogicShiftRight: op = BinaryOperatorType::Lsr; break;
    case TokenClass::ArithmeticShiftRight: op = BinaryOperatorType::Asr; break;
    case TokenClass::KeywordAnd: op = BinaryOperatorType::And; break;
    case TokenClass::KeywordOr: op = BinaryOperatorType::Or; break;
    case TokenClass::KeywordXor: op = BinaryOperatorType::Xor; break;
    default: hasType = false; break;
    }

    if (hasType)
    {
        return new BinaryOperatorNode(token.getLocation(), node, op);
    }
    else
    {
        return nullptr;
    }
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// ExperssionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node forming part or all of an expression which
//! can be evaluated.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] exprType The type of expression being represented.
ExpressionNode::ExpressionNode(const Location &at, ExpressionType exprType) :
    _position(at),
    _type(exprType)
{
}

//! @brief Gets the position of the text the node represents in source code.
const Location &ExpressionNode::getPosition() const
{
    return _position;
}

//! @brief Gets the type of expression being represented.
ExpressionType ExpressionNode::getExprType() const
{
    return _type;
}

////////////////////////////////////////////////////////////////////////////////
// TerminalExpressionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a terminal expression node.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] exprType The type of expression being represented.
TerminalExpressionNode::TerminalExpressionNode(const Location &at,
                                               ExpressionType exprType) :
    ExpressionNode(at, exprType)
{
}

// Inherited from ISyntaxNode.
bool TerminalExpressionNode::isComplete() const { return true; }

// Inherited from ISyntaxNode.
bool TerminalExpressionNode::isValid() const { return true; }

// Inherited from ISyntaxNode.
ISyntaxNode *TerminalExpressionNode::applyToken(ParseContext & /* context */,
                                                const Token &token)
{
    return expandExpression(this, token);
}

// Inherited from ISyntaxNode.
ISyntaxNode *TerminalExpressionNode::applyNode(ParseContext & /* context */,
                                               ISyntaxNode * /* childNode */)
{
    return nullptr;
}

// Inherited from ISyntaxNode.
void TerminalExpressionNode::recover(ParseContext & /* context */,
                                     ISyntaxNode *node)
{
    safeDelete(node);
}

////////////////////////////////////////////////////////////////////////////////
// IntegerLiteralNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing an integer literal.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] value The digits of the integer.
//! @param[in] radix The radix used to decode the digits.
IntegerLiteralNode::IntegerLiteralNode(const Location &at,
                                       string_cref_t value, int radix) :
    TerminalExpressionNode(at, ExpressionType::IntegerLiteral),
    _value(value),
    _radix(radix)
{
}

//! @brief Gets the radix which should be used to interpret the value digits.
int IntegerLiteralNode::getRadix() const { return _radix; }

//! @brief Gets the value digits without any radix prefix.
string_cref_t IntegerLiteralNode::getValue() const { return _value; }

// Inherited from ExpressionNode.
IExpr *IntegerLiteralNode::compile(const ConstantSet & /* constants */) const
{
    Value intValue;
    uint64_t parsedValue;

    if (_value.tryParseScalar(parsedValue, _radix))
    {
        if (parsedValue <= UINT32_MAX)
        {
            intValue = static_cast<uint32_t>(parsedValue);
        }
        else
        {
            intValue = parsedValue;
        }
    }

    return new ConstantExpr(getPosition(), intValue);
}

////////////////////////////////////////////////////////////////////////////////
// RealLiteralNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing an floating point literal.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] value The digits of the real value including decimal point and
//! exponent notation.
RealLiteralNode::RealLiteralNode(const Location &at, string_cref_t value) :
    TerminalExpressionNode(at, ExpressionType::RealLiteral),
    _value(value)
{
}

//! @brief Gets the text value of the numeric literal.
string_cref_t RealLiteralNode::getValue() const { return _value; }

// Inherited from ExpressionNode.
IExpr *RealLiteralNode::compile(const ConstantSet & /* constants */) const
{
    Value realValue;
    char *end = nullptr;
    double parsedValue = strtod(_value.getUtf8Bytes(), &end);

    if ((end != nullptr) && (parsedValue != HUGE_VAL) && (errno != ERANGE))
    {
        float floatValue = static_cast<float>(parsedValue);

        if (parsedValue == static_cast<double>(floatValue))
        {
            // There is no loss of accuracy to store the number as a float.
            realValue = floatValue;
        }
        else
        {
            // 64-bit precision is required to accurately represent the value.
            realValue = parsedValue;
        }
    }

    return new ConstantExpr(getPosition(), realValue);
}

////////////////////////////////////////////////////////////////////////////////
// StringLiteralNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing an string or character literal.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] value The un-escaped characters of the string.
//! @param[in] isUnterminted True if the string literal had no closing quote,
//! false if the literal was valid.
StringLiteralNode::StringLiteralNode(const Location &at, string_cref_t value,
                                     bool isUnterminted) :
    TerminalExpressionNode(at, ExpressionType::StringLiteral),
    _value(value),
    _isUnterminted(isUnterminted)
{
}

//! @brief Gets the value of the string with escape sequences resolved.
string_cref_t StringLiteralNode::getValue() const
{
    return _value;
}

// Inherited from ExpressionNode.
IExpr *StringLiteralNode::compile(const ConstantSet & /* constants */) const
{
    if (_value.getUtf32Length() == 1)
    {
        return new ConstantExpr(getPosition(), Value(*_value.begin()));
    }
    else
    {
        return new ConstantExpr(getPosition(), Value(_value));
    }
}

////////////////////////////////////////////////////////////////////////////////
// SymbolNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing an reference to a variable.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] id The identifier of the referenced variable.
SymbolNode::SymbolNode(const Location &at, string_cref_t id) :
    TerminalExpressionNode(at, ExpressionType::Symbol),
    _id(id)
{
}

//! @brief Gets the symbol identifier to be looked up in order to get a value.
string_cref_t SymbolNode::getId() const { return _id; }

// Inherited from ExpressionNode.
IExpr *SymbolNode::compile(const ConstantSet &constants) const
{
    Value fixedValue;

    if (constants.tryLookupValue(_id, fixedValue))
    {
        // The symbol resolves to a constant, encode it as such.
        return new ConstantExpr(getPosition(), fixedValue);
    }
    else
    {
        return new SymbolExpr(getPosition(), _id);
    }
}

////////////////////////////////////////////////////////////////////////////////
// CurrentAddressNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a nodes representing use of the current assembly offset.
//! @param[in] at The position of the text the node represents in source code.
CurrentAddressNode::CurrentAddressNode(const Location &at) :
    TerminalExpressionNode(at, ExpressionType::CurrentOffset)
{
}

// Inherited from ExpressionNode.
IExpr *CurrentAddressNode::compile(const ConstantSet & /* constants */) const
{
    return new CurrentAddressExpr(getPosition());
}

////////////////////////////////////////////////////////////////////////////////
// ParenthesisNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing a matching set of parenthesis.
//! @param[in] at The position of the text the node represents in source code.
ParenthesisNode::ParenthesisNode(const Location &at) :
    ExpressionNode(at, ExpressionType::Parenthesis),
    _state(State::BeforeExpr),
    _isValid(true)
{
}

//! @brief Gets a pointer to the inner expression node, null if incomplete.
ExpressionNodeCPtr ParenthesisNode::getChildExpr() const
{
    return _child.get();
}

// Inherited from ISyntaxNode.
bool ParenthesisNode::isComplete() const
{
    return (_state == State::Complete);
}

// Inherited from ISyntaxNode.
bool ParenthesisNode::isValid() const
{
    return (_state == State::Complete) && _isValid;
}

// Inherited from ISyntaxNode.
ISyntaxNode *ParenthesisNode::applyToken(ParseContext &context,
                                         const Token &token)
{
    bool usedToken = false;

    if (_state != State::Complete)
    {
        if (token.getClass() == TokenClass::CloseParenthesis)
        {
            usedToken = true;

            if (_state == State::BeforeExpr)
            {
                _isValid = false;
                context.getMessages().appendError(token.getLocation(),
                                                  "Empty parenthesis.");
            }

            _state = State::Complete;
        }
        else if (isEndOfExpr(token))
        {
            // Don't consume the token, it is meant for something else, but
            // mark the current expression as complete, if possibly invalid.
            _state = State::Complete;

            if (_state == State::AfterExpr)
            {
                context.getMessages().appendError(getPosition(),
                                                  "Missing closing parenthesis.");
            }
            else
            {
                context.getMessages().appendError(getPosition(),
                                                  "Orphan opening parenthesis.");
            }

            _isValid = _child.operator bool();
        }
    }

    if (usedToken)
    {
        return this;
    }
    else if (_state == State::Complete)
    {
        return expandExpression(this, token);
    }
    else
    {
        return nullptr;
    }
}

// Inherited from ISyntaxNode.
ISyntaxNode *ParenthesisNode::applyNode(ParseContext & /* context */,
                                   ISyntaxNode *childNode)
{
    bool usedNode = false;

    if (_state == State::BeforeExpr)
    {
        ExpressionNode *childExpr;

        if (tryCast(childNode, childExpr))
        {
            _child.reset(childExpr);
            _state = State::AfterExpr;
            _isValid = childExpr->isValid();
            usedNode = true;
        }
    }

    return usedNode ? this : nullptr;
}

// Inherited from ISyntaxNode.
void ParenthesisNode::recover(ParseContext & /* context */,
                              ISyntaxNode *node)
{
    ExpressionNode *childExpr;

    if (tryCast(node, childExpr))
    {
        if (_state == State::BeforeExpr)
        {
            // Take the incomplete child expression.
            _child.reset(childExpr);
            _state = State::AfterExpr;
            _isValid = false;
        }
        else
        {
            safeDelete(childExpr);
        }
    }
    else
    {
        safeDelete(node);
    }
}

// Inherited from ExpressionNode.
IExpr *ParenthesisNode::compile(const ConstantSet &constants) const
{
    if (_child && _child->isValid())
    {
        return _child->compile(constants);
    }
    else
    {
        // TODO: Should we return something else to prevent evaluation.
        return new ConstantExpr(getPosition(), Value());
    }
}

////////////////////////////////////////////////////////////////////////////////
// OperatorNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing the application of an operator to
//! one or more sub-expressions.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] exprType The type of expression being represented.
//! @param[in] precedence The precedence of the operator.
OperatorNode::OperatorNode(const Location &at, ExpressionType exprType, int precedence) :
    ExpressionNode(at, exprType),
    _precedence(precedence)
{
}

//! @brief Gets the precedence of the operator, the highest has the most importance.
int OperatorNode::getPresedence() const
{
    return _precedence;
}

////////////////////////////////////////////////////////////////////////////////
// UnaryOperatorNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing the application of an operator to a
//! single sub-expression.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] opType The type of operator to apply.
UnaryOperatorNode::UnaryOperatorNode(const Location &at, UnaryOperatorType opType) :
    OperatorNode(at, ExpressionType::UnaryOperator, getOperatorPrecidence(opType)),
    _operatorType(opType),
    _state(State::BeforeExpr),
    _isValid(false)
{
}

//! @brief Gets the type of operator being represented.
UnaryOperatorType UnaryOperatorNode::getOperatorType() const
{
    return _operatorType;
}

//! @brief Gets the inner expression if the node is valid.
ExpressionNodeCPtr UnaryOperatorNode::getChildExpr() const
{
    return _child.get();
}

// Inherited from ISyntaxNode.
bool UnaryOperatorNode::isComplete() const
{
    return (_state == State::Complete);
}

// Inherited from ISyntaxNode.
bool UnaryOperatorNode::isValid() const
{
    return (_state == State::Complete) && _isValid;
}

// Inherited from ISyntaxNode.
ISyntaxNode *UnaryOperatorNode::applyToken(ParseContext &context,
                                           const Token &token)
{
    if ((_state != State::Complete) && isEndOfExpr(token))
    {
        _state = State::Complete;
        context.getMessages().appendError(getPosition(),
                                          "Orphan unary operator.");
    }

    return (_state == State::Complete) ? expandExpression(this, token) :
                                         nullptr;
}

// Inherited from ISyntaxNode.
ISyntaxNode *UnaryOperatorNode::applyNode(ParseContext & /* context */,
                                          ISyntaxNode *childNode)
{
    ExpressionNode *childExpr = nullptr;
    ExpressionNode *current = this;

    if ((_state == State::BeforeExpr) && tryCast(childNode, childExpr))
    {
        if (childExpr->getExprType() == ExpressionType::BinaryOperator)
        {
            // Apply precedence rules.
            BinaryOperatorNode *binaryOp = static_cast<BinaryOperatorNode *>(childExpr);

            if (getPresedence() > binaryOp->getPresedence())
            {
                // Use the left side of the binary operator expression as
                // the current operator's child and embed the current
                // operator as the new left child of the binary op.
                childExpr = binaryOp->swapLeftChild(this);
                current = binaryOp;
            }
        }

        _child.reset(childExpr);
        _isValid = _child->isValid();
        _state = State::Complete;
    }
    else
    {
        current = nullptr;
    }

    return current;
}

// Inherited from ISyntaxNode.
void UnaryOperatorNode::recover(ParseContext &context,
                                ISyntaxNode *node)
{
    ExpressionNode *childExpr = nullptr;

    if ((_state == State::BeforeExpr))
    {
        if (tryCast(node, childExpr))
        {
            _child.reset(childExpr);
            _isValid = false;
            _state = State::Complete;
        }
        else
        {
            // We can't consume the new node, but we can certainly
            // force ourselves into a complete state with an appropriate
            // error.
            safeDelete(node);
            context.getMessages().appendError(getPosition(),
                                              "Orphan unary operator.");
            _state = State::Complete;
        }
    }
    else
    {
        // We could not apply the node, so dispose of it.
        safeDelete(node);
    }
}

// Inherited from ExpressionNode.
IExpr *UnaryOperatorNode::compile(const ConstantSet &constants) const
{
    IExprPtr expr = nullptr;

    if (_child && _child->isValid())
    {
        IExprPtr childExpr = _child->compile(constants);

        switch (_operatorType)
        {
        case UnaryOperatorType::Minus:
            expr = UnaryOpExpr::createMinus(getPosition(), childExpr);
            break;

        case UnaryOperatorType::Absolute:
            expr = UnaryOpExpr::createAbsolute(getPosition(), childExpr);
            break;

        case UnaryOperatorType::LogicalNot:
            expr = UnaryOpExpr::createNot(getPosition(), childExpr);
            break;

        default:
            expr = childExpr;
            break;
        }
    }

    // TODO: Should we return something else to prevent evaluation.
    if (expr == nullptr)
    {
        expr = new ConstantExpr(getPosition(), Value());
    }
    else
    {
        expr = constantOptimise(expr);
    }

    return expr;
}

////////////////////////////////////////////////////////////////////////////////
// BinaryOperatorNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing the application of an operator to a
//! single sub-expression.
//! @param[in] at The position of the text the node represents in source code.
//! @param[in] lhsExpr The already parsed left operand sub-tree of the binary
//! operator expression.
//! @param[in] opType The type of operator to apply.
BinaryOperatorNode::BinaryOperatorNode(const Location &at,
                                       ExpressionNode *lhsExpr,
                                       BinaryOperatorType opType) :
    OperatorNode(at, ExpressionType::BinaryOperator,
                 getOperatorPrecidence(opType)),
    _lhsChild(lhsExpr),
    _operatorType(opType),
    _state(State::BeforeRhsExpr),
    _isValid((lhsExpr == nullptr) ? false : lhsExpr->isValid())
{
}

//! @brief Gets the identifier of the operator being applied.
BinaryOperatorType BinaryOperatorNode::getOperatorType() const
{
    return _operatorType;
}

//! @brief Gets the expression evaluating to the left operand.
ExpressionNodeCPtr BinaryOperatorNode::getLeftChild() const
{
    return _lhsChild.get();
}

//! @brief Gets the expression evaluating to the right operand.
ExpressionNodeCPtr BinaryOperatorNode::getRightChild() const
{
    return _rhsChild.get();
}

//! @brief Swaps the left operand with a new sub-expression.
//! @param newLeft The new left sub-expression.
//! @return The old left sub-expression which the caller is responsible for
//! disposing if.
ExpressionNodePtr BinaryOperatorNode::swapLeftChild(ExpressionNodePtr newLeft)
{
    ExpressionNodePtr oldLeft = _lhsChild.release();
    _lhsChild.reset(newLeft);

    return oldLeft;
}

// Inherited from ISyntaxNode.
bool BinaryOperatorNode::isComplete() const { return (_state == State::Complete); }

// Inherited from ISyntaxNode.
bool BinaryOperatorNode::isValid() const
{
    return (_state == State::Complete) && _isValid;
}

// Inherited from ISyntaxNode.
ISyntaxNode *BinaryOperatorNode::applyToken(ParseContext &context,
                                            const Token &token)
{
    if (_state == State::BeforeRhsExpr)
    {
        if (isEndOfExpr(token))
        {
            // Force closure of the expression.
            context.getMessages().appendError(token.getLocation(),
                                              "Incomplete expression.");
            _state = State::Complete;
        }
    }

    return (_state == State::Complete) ? expandExpression(this, token) :
                                         nullptr;
}

// Inherited from ISyntaxNode.
ISyntaxNode *BinaryOperatorNode::applyNode(ParseContext & /* context */,
                                           ISyntaxNode *childNode)
{
    ExpressionNode *childExpr = nullptr;
    ExpressionNode *current = this;

    if ((_state == State::BeforeRhsExpr) && tryCast(childNode, childExpr))
    {
        if (childExpr->getExprType() == ExpressionType::BinaryOperator)
        {
            // Apply precedence rules.
            BinaryOperatorNode *binaryOp = static_cast<BinaryOperatorNode *>(childExpr);

            if (getPresedence() > binaryOp->getPresedence())
            {
                // Use the left side of the binary operator sub-node as
                // the current operator's right child and embed the current
                // operator as the new left child of the binary op sub-node
                // in order to promote it.
                childExpr = binaryOp->swapLeftChild(this);
                current = binaryOp;
            }
        }

        _rhsChild.reset(childExpr);
        _isValid &= _rhsChild->isValid();
        _state = State::Complete;
    }
    else
    {
        current = nullptr;
    }

    return current;
}

// Inherited from ISyntaxNode.
void BinaryOperatorNode::recover(ParseContext &context,
                                 ISyntaxNode *node)
{
    ExpressionNode *childExpr = nullptr;

    if ((_state == State::BeforeRhsExpr))
    {
        if (tryCast(node, childExpr))
        {
            _rhsChild.reset(childExpr);
            _isValid = false;
            _state = State::Complete;
        }
        else
        {
            // Force the expression to a complete state.
            _state = State::Complete;
            context.getMessages().appendError(getPosition(),
                                              "Incomplete operator expression.");
            safeDelete(node);
        }
    }
    else
    {
        // We could not apply the node, so dispose of it.
        safeDelete(node);
    }
}


// Inherited from ExpressionNode.
IExpr *BinaryOperatorNode::compile(const ConstantSet &constants) const
{
    IExprPtr expr = nullptr;

    if (_lhsChild && _lhsChild->isValid() &&
        _rhsChild && _rhsChild->isValid())
    {
        IExprPtr lhsExpr = _lhsChild->compile(constants);
        IExprPtr rhsExpr = _rhsChild->compile(constants);

        switch (_operatorType)
        {
        case BinaryOperatorType::Addition:
            expr = BinaryOpExpr::createAddition(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Subtraction:
            expr = BinaryOpExpr::createSubtraction(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Multiplication:
            expr = BinaryOpExpr::createMultiplication(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Division:
            expr = BinaryOpExpr::createDivision(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Modulus:
            expr = BinaryOpExpr::createModulus(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Lsl:
            expr = BinaryOpExpr::createLsl(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Lsr:
            expr = BinaryOpExpr::createLsr(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Asr:
            expr = BinaryOpExpr::createAsr(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::And:
            expr = BinaryOpExpr::createAnd(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Or:
            expr = BinaryOpExpr::createOr(getPosition(), lhsExpr, rhsExpr);
            break;

        case BinaryOperatorType::Xor:
            expr = BinaryOpExpr::createXor(getPosition(), lhsExpr, rhsExpr);
            break;

        default:
            break;
        }
    }

    // TODO: Should we return something else to prevent evaluation.
    if (expr == nullptr)
    {
        expr = new ConstantExpr(getPosition(), Value());
    }
    else
    {
        expr = constantOptimise(expr);
    }

    return expr;
}

////////////////////////////////////////////////////////////////////////////////
// ExprToCompile Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a record of a hierarchy of expression syntax nodes to
//! compile into an evaluable expression tree.
//! @param[in] expr The expression syntax tree to compile.
//! @param[in] name The used to refer to the expression in error messages.
ExprToCompile::ExprToCompile(const ExpressionNodeUPtr &expr, const char *name) :
    Expr(expr.get()),
    Name(name),
    FixedSymbols(ConstantSet::Empty)
{
}

//! @brief Constructs a record of a hierarchy of expression syntax nodes to
//! compile into an evaluable expression tree.
//! @param[in] expr The expression syntax tree to compile.
//! @param[in] name The used to refer to the expression in error messages.
//! @param[in] fixedSymbols The fixed set of symbols to recognise when the
//! syntax tree is compiled.
ExprToCompile::ExprToCompile(const ExpressionNodeUPtr &expr, const char *name,
              const ConstantSet &fixedSymbols) :
    Expr(expr.get()),
    Name(name),
    FixedSymbols(fixedSymbols)
{
}

//! @brief Attempts to compile the expression syntax tree described by the item.
//! @param[out] expr A managed pointer to received the compiled expression.
//! @param[in] log The log to receive error messages on failure.
//! @retval true The expression was successfully compiled and expr given
//! ownership of the result.
//! @retval false The expression could not be compiled, an error was added to
//! the log.
bool ExprToCompile::tryCompile(IExprUPtr &expr, Messages &log) const
{
    IExpr *compiledExpr = Expr->compile(FixedSymbols);

    if (compiledExpr == nullptr)
    {
        log.appendError(Expr->getPosition(), "Failed to compile {0} expression.",
                        { Name });
        expr.reset();
    }
    else
    {
        expr.reset(compiledExpr);
    }

    return static_cast<bool>(expr);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
bool compileExpressionNodes(ExprToCompile *exprNodes, IExprUPtr *exprs,
                            size_t count, Messages &log)
{
    bool isOK = true;

    // Compile all the expressions.
    for (size_t i = 0; isOK && (i < count); ++i)
    {
        isOK &= exprNodes[i].tryCompile(exprs[i], log);
    }

    return isOK;
}


}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


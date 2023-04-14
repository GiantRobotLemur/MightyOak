//! @file AsmTools/LabelStatement.cpp
//! @brief The definition of structures required to parse and process an
//! assembly language statement defining a label.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ConstantSet.hpp"
#include "LabelStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// LabelNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node which represents a partially parsed
//! statement defining an assembly language label.
//! @param[in] context The current state of the parsing process.
//! @param[in] labelToken The token providing the label text.
LabelNode::LabelNode(ParseContext &context, const Token &labelToken) :
    StatementNode(context, labelToken),
    _id(labelToken.getValue()),
    _state(State::AfterSymbol)
{
    // Attempt to parse an optional value expression.
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool LabelNode::isComplete() const
{
    return _state == State::Complete;
}

// Inherited from ISyntaxNode.
bool LabelNode::isValid() const
{
    return _id.isEmpty() == false;
}

// Inherited from ISyntaxNode.
ISyntaxNode *LabelNode::applyToken(ParseContext &context, const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::AfterSymbol:
        if (token.getClass() == TokenClass::StatementTerminator)
        {
            // There is no value, so the label represents the current
            // assembly address.
            restoreLexicalState(context);
            _state = State::Complete;
            result = this;
        }
        break;

    case State::Complete:
    default:
        break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *LabelNode::applyNode(ParseContext &context, ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr expr;

    switch (_state)
    {
    case State::AfterSymbol:
        if (tryCast(childNode, expr))
        {
            // The label was associated with a value.
            _valueExpr.reset(expr);
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::Complete:
    default:
        break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void LabelNode::recover(ParseContext &context, ISyntaxNode *node)
{
    restoreLexicalState(context);
    safeDelete(node);

    context.recover(TokenClass::StatementTerminator);
}

// Inherited from StatementNode.
Statement *LabelNode::compile(Messages &output) const
{
    IExprUPtr expr;

    if (_valueExpr)
    {
        expr.reset(_valueExpr->compile(ConstantSet::Empty));

        if (!expr)
        {
            output.appendError(_valueExpr->getPosition(),
                               "Failed to compile labelled value expression.");
            return nullptr;
        }
    }

    return new LabelStatement(_id, getStart(), std::move(expr));
}

////////////////////////////////////////////////////////////////////////////////
// LabelStatement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a new Statement-derived object which represents the
//! definition of an assembly language label.
//! @param[in] id The label identifier being defined.
//! @param[in] at The location at which the label was defined.
//! @param[in] value The optional expression evaluating to a value to associate
//! with the label.
LabelStatement::LabelStatement(string_cref_t id, const Location &at,
                               IExprUPtr &&value) :
    Statement(StatementType::Label),
    _at(at),
    _id(id),
    _valueExpr(std::move(value))
{
}

//! @brief Gets the identifier of the label.
string_cref_t LabelStatement::getID() const { return _id; }

//! @brief Gets the location in source code at which the label was defined.
const Location &LabelStatement::getSourcePosition() const { return _at; }

//! @brief Gets the optional expression giving the value to associate with the
//! label.
//! @return Either a pointer to an expression tree or nullptr if the label
//! represents the current object code address.
IExprCPtr LabelStatement::getValueExpr() const
{
    return _valueExpr.get();
}

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


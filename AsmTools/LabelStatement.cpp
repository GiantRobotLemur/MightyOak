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
#include "LabelStatement.hpp"
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
    _currentState(State::AfterSymbol)
{
}

// Inherited from ISyntaxNode.
bool LabelNode::isComplete() const
{
    return _currentState == State::Complete;
}

// Inherited from ISyntaxNode.
bool LabelNode::isValid() const
{
    return _id.isEmpty() == false;
}

// Inherited from ISyntaxNode.
ISyntaxNode *LabelNode::applyToken(ParseContext &/*context*/, const Token &token)
{
    ISyntaxNode *resultNode = nullptr;

    if (token.getClass() == TokenClass::StatementTerminator)
    {
        _currentState = State::Complete;
        resultNode = this;
    }

    return resultNode;
}

// Inherited from ISyntaxNode.
ISyntaxNode *LabelNode::applyNode(ParseContext & /* context */,
                                  ISyntaxNode */* childNode */)
{
    // This is a leaf node.
    return nullptr;
}

// Inherited from ISyntaxNode.
void LabelNode::recover(ParseContext &context, ISyntaxNode *node)
{
    safeDelete(node);

    context.recover(TokenClass::StatementTerminator);
}

// Inherited from StatementNode.
Statement *LabelNode::compile(Messages &/*output*/) const
{
    return new LabelStatement(_id, getStart());
}

////////////////////////////////////////////////////////////////////////////////
// LabelStatement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a new Statement-derived object which represents the
//! definition of an assembly language label.
LabelStatement::LabelStatement(string_cref_t id, const Location &at) :
    Statement(StatementType::Label),
    _at(at),
    _id(id)
{
}

//! @brief Gets the identifier of the label.
string_cref_t LabelStatement::getID() const { return _id; }

//! @brief Gets the location in source code at which the label was defined.
const Location &LabelStatement::getSourcePosition() const { return _at; }

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file IncludeStatement.cpp
//! @brief The definition of objects representing the use of the include
//! directive in assembly language source code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "IncludeStatement.hpp"
#include "ParseContext.hpp"
#include "Token.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// IncludeNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a partially-parsed include directive.
//! @param[in] context The current state of the parsing process.
//! @param[in] at The location of the directive in source code.
IncludeNode::IncludeNode(ParseContext &context, const Location &at) :
    StatementNode(context, at),
    _currentState(State::AfterKeyword)
{
}

// Inherited from ISyntaxNode.
bool IncludeNode::isComplete() const
{
    return _currentState == State::AfterKeyword;
}

// Inherited from ISyntaxNode.
bool IncludeNode::isValid() const
{
    return (_currentState == State::Complete) &&
           (_id.isEmpty() == false);
}

// Inherited from ISyntaxNode.
ISyntaxNode *IncludeNode::applyToken(ParseContext & /* context */,
                                     const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_currentState)
    {
    case State::AfterKeyword:
        if (token.getClass() == TokenClass::StringLiteral)
        {
            _id = token.getValue();
            _currentState = State::Complete;
            result = this;
        }
        break;

    case State::AfterFileId:
        if (token.getClass() == TokenClass::StatementTerminator)
        {
            _currentState = State::Complete;
            result = this;
        }
        break;

    default: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *IncludeNode::applyNode(ParseContext & /* context */,
                                    ISyntaxNode * /* childNode */)
{
    return nullptr;
}

// Inherited from ISyntaxNode.
void IncludeNode::recover(ParseContext &context, ISyntaxNode *node)
{
    Ag::safeDelete(node);

    context.recover(TokenClass::StatementTerminator);
}

// Inherited from StatementNode.
Statement *IncludeNode::compile(Messages & /* output */) const
{
    return new IncludeStatement(_id, getStart());
}

////////////////////////////////////////////////////////////////////////////////
// IncludeStatement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
IncludeStatement::IncludeStatement(Ag::string_cref_t id, const Location &at) :
    Statement(StatementType::Include),
    _at(at),
    _id(id)
{
}

//! @brief Gets the location of the include directive in source code.
const Location &IncludeStatement::getLocation() const { return _at; }

//! @brief Gets the name of the file to include as specified in source code.
Ag::string_cref_t IncludeStatement::getFileReference() const { return _id; }

// Inherited from Statement.
uint32_t IncludeStatement::calculateObjectCodeSize(IEvalContext */*context*/) const
{
    return 0;
}

// Inherited from Statement.
bool IncludeStatement::assemble(const AssemblyState &/*state*/,
                                IEvalContext * /* context */,
                                ObjectCodeBuilder & /* output */) const
{
    return false;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


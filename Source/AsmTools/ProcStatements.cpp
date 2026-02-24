//! @file AsmTools/ProcStatements.cpp
//! @brief The definition of objects related to the parsing or procedure-related
//! keywords.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ProcStatements.hpp"
#include "ParseContext.hpp"
#include "ExprContexts.hpp"
#include "LexicalAnalysers.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// ProcStatementNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a statement documenting the beginning of a subroutine
//! definition.
//! @param[in] context The context of the parsing process.
//! @param[in] start The location in source code of the statement.
ProcStatementNode::ProcStatementNode(ParseContext &context,
                                     const Location &start) :
    StatementNode(context, start),
    _state(State::AfterKeyword)
{
}

// Inherited from ISyntaxNode.
bool ProcStatementNode::isComplete() const
{
    return _state == State::Complete;
}

// Inherited from ISyntaxNode.
bool ProcStatementNode::isValid() const
{
    return _id.isEmpty() == false;
}

// Inherited from ISyntaxNode.
ISyntaxNode *ProcStatementNode::applyToken(ParseContext &context,
                                           const Token &token)
{
    ISyntaxNode *result = nullptr;

    if (_state == State::AfterKeyword)
    {
        switch (token.getClass())
        {
        case TokenClass::Symbol:
            _id = token.getValue();
            _state = State::Complete;
            result = this;
            break;

        case TokenClass::StatementTerminator:
            context.getMessages().appendError(token.getLocation(),
                "A subroutine name was expected after the PROC keyword.");

            _state = State::Complete;
            result = this;
            break;

        default:
            break;
        }
    }

    return result;
}
// Inherited from ISyntaxNode.
ISyntaxNode *ProcStatementNode::applyNode(ParseContext &/*context*/,
                                          ISyntaxNode */*childNode*/)
{
    // We only expect a symbol token after the initial keyword.
    return nullptr;
}

// Inherited from ISyntaxNode.
void ProcStatementNode::recover(ParseContext &context, ISyntaxNode *node)
{
    // Force completion of the statement.
    Token next;
    while (context.tryGetNextToken(next))
    {
        if (next.getClass() == TokenClass::StatementTerminator)
        {
            context.ungetToken(next);
            break;
        }
    }

    Ag::safeDelete(node);

    _state = State::Complete;
}

// Inherited from StatementNode.
Statement *ProcStatementNode::compile(Messages &/*output*/) const
{
    return new ProcStatement(getStart(), _id);
}

////////////////////////////////////////////////////////////////////////////////
// ProcStatement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a statement documenting the beginning of a subroutine
//! definition.
//! @param[in] at The location of the statement in source code.
//! @param[in] id The name of the subroutine being defined.
ProcStatement::ProcStatement(const Location &at, Ag::string_cref_t id) :
    Statement(StatementType::ProcedureStart),
    _at(at),
    _id(id)
{
}

//! @brief Gets the name used to refer to the subroutine.
Ag::string_cref_t ProcStatement::getId() const { return _id; }

//! @brief Gets the location of the statement in source code.
const Location &ProcStatement::getLocation() const { return _at; }

// Inherited from Statement.
bool ProcStatement::assemble(const AssemblyState &/*state*/,
                             IEvalContext */*context*/,
                             ObjectCodeBuilder &/*output*/) const
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// EndProcStatementNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a parse node for the keyword marking the end of a
//! subroutine definition.
//! @param[in] context The current state of the parsing context.
//! @param[in] start The location in source code of the keyword.
EndProcStatementNode::EndProcStatementNode(ParseContext &context,
                                           const Location &start) :
    StatementNode(context, start)
{
}

// Inherited from ISyntaxNode.
bool EndProcStatementNode::isComplete() const { return true; }

// Inherited from ISyntaxNode.
bool EndProcStatementNode::isValid() const { return true; }

// Inherited from ISyntaxNode.
ISyntaxNode *EndProcStatementNode::applyToken(ParseContext &/*context*/,
                                              const Token &/*token*/)
{
    // It's a single token statement.
    return nullptr;
}

// Inherited from ISyntaxNode.
ISyntaxNode *EndProcStatementNode::applyNode(ParseContext &/*context*/,
                                             ISyntaxNode */*childNode*/)
{
    // The node is constructed complete.
    return nullptr;
}

// Inherited from ISyntaxNode.
void EndProcStatementNode::recover(ParseContext &context, ISyntaxNode *node)
{
    // Skip stuff until the next statement terminator.
    Token next;

    while (context.tryGetNextToken(next))
    {
        if (next.getClass() == TokenClass::StatementTerminator)
        {
            context.ungetToken(next);
            break;
        }
    }

    Ag::safeDelete(node);
}

// Inherited from StatementNode.
Statement *EndProcStatementNode::compile(Messages &/*output*/) const
{
    return new EndProcStatement(getStart());
}

////////////////////////////////////////////////////////////////////////////////
// EndProcStatement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a statement which marks the end of a subroutine definition.
//! @param[in] at The location of the keyword which ends the definition.
EndProcStatement::EndProcStatement(const Location &at) :
    Statement(StatementType::ProcedureEnd),
    _at(at)
{
}

//! @brief Gets the location of the statement in source code.
const Location &EndProcStatement::getLocation() const
{
    return _at;
}

// Inherited from Statement.
bool EndProcStatement::assemble(const AssemblyState &/*state*/,
                                IEvalContext */*context*/,
                                ObjectCodeBuilder &/*output*/) const
{
    return false;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


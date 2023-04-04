//! @file Statement.cpp
//! @brief The definition of an object created to represent a parsed assembly
//! language statement.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <iterator>

#include "ParseContext.hpp"
#include "ObjectCodeBuilder.hpp"
#include "Statement.hpp"

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

} // TED


////////////////////////////////////////////////////////////////////////////////
// StatementNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing an assembly language statement.
//! @param[in] context The current state of the parsing process.
//! @param[in] start The location of the beginning of the statement.
StatementNode::StatementNode(const ParseContext &context, const Location &start) :
    BaseSyntaxNode(context, start)
{
}

//! @brief Constructs an object representing an assembly language statement.
//! @param[in] context The current state of the parsing process.
//! @param[in] startToken The token marking the beginning of the statement.
StatementNode::StatementNode(const ParseContext &context, const Token &startToken) :
    BaseSyntaxNode(context, startToken.getLocation())
{
}

//! @brief A default implementation for a StatementNode-derived object which
//! skips further tokens until the next statement terminator and deletes the
//! erroneous syntax node.
void StatementNode::recover(ParseContext &context, ISyntaxNode *node)
{
    // Skip further tokens until the next statement terminator.
    context.recover(TokenClass::StatementTerminator);

    BaseSyntaxNode::recover(context, node);
}

////////////////////////////////////////////////////////////////////////////////
// Statement Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a statement.
//! @param[in] type The data type of the statement object.
Statement::Statement(StatementType type) :
    _type(type)
{
}

//! @brief Gets the data type of the statement-derived object.
StatementType Statement::getType() const { return _type; }

//! @brief Optionally changes the current assembly state.
//! @param[in] state The assembly state to update.
//! @retval true The state was changed.
//! @retval false The state was unmodified.
//! @note The base class implementation always leaves the state unmodified.
bool Statement::updateAssemblyState(AssemblyState &/*state*/) const
{
    return false;
}

//! @brief Calculates the count of object code bytes the statement will consume.
//! @param[in] context The context used to get the offset of the statement
//! being assembled and the values of symbols referenced in expressions.
//! @note The base class implementation always returns 0.
uint32_t Statement::calculateObjectCodeSize(IEvalContext */*context*/) const
{
    return 0;
}

//! @brief Produces object code from the statement.
//! @param[in] state The state of the assembler when the instruction
//! is encountered.
//! @param[in] context The context used to get the offset of the statement
//! being assembled and the values of symbols referenced in expressions.
//! @param[in] output An object to receive assembled object code or error
//! messages.
//! @retval true If object code was produced.
//! @retval false If object code could not be produced.
//! @note The base class implementation always returns false without assembling
//! any code.
bool Statement::assemble(const AssemblyState &/*state*/,
                         IEvalContext */*context*/,
                         ObjectCodeBuilder &/*output*/) const
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


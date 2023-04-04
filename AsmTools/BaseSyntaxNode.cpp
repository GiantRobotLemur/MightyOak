//! @file BaseSyntaxNode.cpp
//! @brief The definition of a base class for ISyntaxNode implementations which
//! contain useful utilities to streamline the parsing process.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "BaseSyntaxNode.hpp"
#include "ParseContext.hpp"

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
// BaseSyntaxNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a base syntax node with no start location in source code.
//! @param[in] context The current state of the parsing process.
BaseSyntaxNode::BaseSyntaxNode(const ParseContext &context) :
    _lexerBaseState(context.getLexicalState())
{
}

//! @brief Constructs a base syntax node with a start location in source code.
//! @param[in] context The current state of the parsing process.
//! @param[in] start The location in source code associated with the first token
//! of the syntax node.
BaseSyntaxNode::BaseSyntaxNode(const ParseContext &context, const Location &start) :
    _start(start),
    _lexerBaseState(context.getLexicalState())
{
}

//! @brief Gets the location in source code of the first token of the syntax node.
const Location &BaseSyntaxNode::getStart() const { return _start; }

//! @brief Attempts to restore the state of the parsing process after a token or
//! syntax node is unrecognised.
//! @param[in] context The current state of the parsing process which will
//! have its lexical state restored to as it was before the current node was
//! created.
//! @param[in] node The unrecognised syntax node, which will be disposed of if
//! non-null.
void BaseSyntaxNode::recover(ParseContext &context, ISyntaxNode *node)
{
    context.restoreLexicalState(_lexerBaseState);

    safeDelete(node);
}

//! @brief Allows the start location of the syntax not to be set internally.
//! @param[in] start The new source code location marking the start of the node.
void BaseSyntaxNode::setStart(const Location &start)
{
    _start = start;
}

//! @brief Restores the lexical analysis state back to what it was when the
//! node was created.
//! @param[in] context The parser to restore the state of.
void BaseSyntaxNode::restoreLexicalState(ParseContext &context) const
{
    context.restoreLexicalState(_lexerBaseState);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


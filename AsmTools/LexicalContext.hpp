//! @file LexicalContext.hpp
//! @brief The declaration of an object which processes characters into tokens.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_LEXICAL_CONTEXT_HPP__
#define __ASM_TOOLS_LEXICAL_CONTEXT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ParseContext;
class InputContext;
class Token;
class ISyntaxNode;

//! @brief An object which processes characters into tokens.
class ILexicalContext
{
public:
    virtual ~ILexicalContext() = default;

    //! @brief Attempts to get the next token by processing characters from an
    //! input source.
    //! @param[in] input The input source to read characters from.
    //! @param[out] token Receives the token if one was recognised.
    //! @retval true A token was recognised and returned, possibly an error.
    //! @retval false There were not enough characters left to recognise
    //! a token, not even an error token.
    virtual bool tryGetNextToken(InputContext &input, Token &token) const = 0;

    //! @brief Attempts to create a partial abstract syntax tree node from a
    //! starting token.
    //! @param[in] context The current state of the parsing process, which can
    //! be altered if a new node is created.
    //! @param[in] token The token used to determine what kind of syntax node
    //! to create and to initially populate it.
    //! @param[out] node Receives a pointer to a new syntax node if successful.
    //! @retval true The token was consumed to create an object returned in node.
    //! @retval false The token was not consumed, no new node was created.
    virtual bool tryCreateNodeFromStartToken(ParseContext &context,
                                             const Token &token,
                                             ISyntaxNode *&node) const = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

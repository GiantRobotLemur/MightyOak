//! @file AsmTools/SyntaxNode.hpp
//! @brief The declaration of an interface to an node in the Abstract Syntax
//! Tree created by the parser.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_SYNTAX_NODE_HPP__
#define __ASM_TOOLS_SYNTAX_NODE_HPP__

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ParseContext;
class Token;

//! @brief An interface to an node in the Abstract Syntax Tree created by the
//! parser.
class ISyntaxNode
{
public:
    // Construction/Destruction
    virtual ~ISyntaxNode() = default;

    // Accessors
    //! @brief Indicates whether the current node is in a state where is does
    //! not require further tokens.
    //! @retval true The node is completed, though further tokens maybe accepted.
    //! @retval false The node is incomplete, further tokens or child nodes are
    //! required to complete it.
    virtual bool isComplete() const = 0;

    //! @brief Indicates whether the syntax node was completed without error so
    //! that post-parsing processing can be performed.
    virtual bool isValid() const = 0;

    // Operations
    //! @brief Attempts to further develop the node by allowing it to consume a
    //! token from the input stream.
    //! @param[in] context The state of the parsing process.
    //! @param[in] token The token to possibly consume.
    //! @returns Either nullptr if the token could not be applied, or a pointer
    //! to the resultant syntax node which replaces the current one if the token
    //! was consumed.
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) = 0;

    //! @brief Attempts to further develop the node by allowing it to consume a
    //! complete child syntax node.
    //! @param[in] context The state of the parsing process.
    //! @param[in] childNode The abstract syntax node to possibly consume.
    //! @return A pointer to of the node to replace the current one with if
    //! the child node is consumed, null if the child node is not consumed.
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) = 0;

    //! @brief Forces the node to consume a child which failed tryApplyNode().
    //! Error messages should be generated and the node should be kept or disposed
    //! of as required.
    virtual void recover(ParseContext &context, ISyntaxNode *node) = 0;
};

using ISyntaxNodePtr = ISyntaxNode *;
using ISyntaxNodeCPtr = const ISyntaxNode *;

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

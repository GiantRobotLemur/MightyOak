//! @file BaseSyntaxNode.hpp
//! @brief The declaration of a base class for ISyntaxNode implementations which
//! contain useful utilities to streamline the parsing process.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_BASE_SYNTAX_NODE_HPP__
#define __ASM_TOOLS_BASE_SYNTAX_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "SyntaxNode.hpp"
#include "Token.hpp"

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

//! @brief A base class for ISyntaxNode implementations which
//! contain useful utilities to streamline the parsing process.
class BaseSyntaxNode : public ISyntaxNode
{
public:
    // Construction/Destruction
    BaseSyntaxNode(const ParseContext &context);
    BaseSyntaxNode(const ParseContext &context, const Location &start);
    virtual ~BaseSyntaxNode() = default;

    // Accessors
    const Location &getStart() const;

    // Overrides
    // Inherited from ISyntaxNode
    // virtual bool isComplete() const = 0;
    // virtual bool isValid() const = 0;
    // virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) = 0;
    // virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) = 0;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;

protected:
    // Internal Functions
    void setStart(const Location &start);
    void restoreLexicalState(ParseContext &context) const;

    // Internal Functions
    //! @brief Confirms the next anonymous token has a specified class and
    //! updates the parse state as a result.
    //! @tparam TState The data type of the value used to track parse state.
    //! @param[in] token The token received by the node to query.
    //! @param[in] expected The expected token class.
    //! @param[in] newState The state the node should move to if the expected
    //! token type was recognised.
    //! @param[out] currentState The state to update with newState if the
    //! token is of the expected class.
    //! @param[out] result The value to be set to the current node if the
    //! expected token type was recognised.
    //! @retval true The token was of the expected type and various output
    //! parameters were updated.
    //! @retval false The token was not of the specified type, no output
    //! parameters were modified.
    //! @note This function is designed for use within applyToken().
    template<typename TState>
    bool confirmToken(const Token &token, TokenClass expected,
                      TState newState, TState &currentState,
                      ISyntaxNode *&result)
    {
        bool isMatched = false;

        if (token.getClass() == expected)
        {
            currentState = newState;
            result = this;
            isMatched = true;
        }

        return isMatched;
    }

    //! @brief Confirms that a child syntax node is of the required type, takes
    //! ownership of it and updates the parse state as a result.
    //! @tparam TState The data type of the value used to track parse state.
    //! @tparam TNodeType The expected data type of the child syntax node.
    //! @param[in] childNode The child syntax node to test.
    //! @param[out] expr Receives ownership of childNode if it was of the
    //! required data type.
    //! @param[in] newState The new parse state to set of childNode was of the
    //! required data type.
    //! @param[out] currentState The parse state to update if childNode was of
    //! the required data type.
    //! @param[out] result The syntax node to update with the this pointer if
    //! childNode is of the required type.
    //! @retval true childNode was of the required type, expr was granted
    //! ownership of it and currentState and result were updated accordingly.
    //! @retval false childNode was not of the expected type, no output
    //! parameters were modified.
    //! @note This function is designed for use inside applyNode().
    template<typename TState, typename TNodeType>
    bool confirmNodeType(ISyntaxNode *childNode, std::unique_ptr<TNodeType> &expr,
                         TState newState, TState &currentState,
                         ISyntaxNode *&result)
    {
        TNodeType *exprNode = nullptr;
        bool isExpr = false;

        if (tryCast(childNode, exprNode))
        {
            expr.reset(exprNode);
            currentState = newState;
            result = this;
            isExpr = true;
        }

        return isExpr;
    }

private:
    // Internal Fields
    Location _start;
    size_t _lexerBaseState;
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

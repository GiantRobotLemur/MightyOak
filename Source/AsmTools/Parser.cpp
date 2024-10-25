//! @file AsmTools/Parser.cpp
//! @brief The definition of the function which parses text into a set of
//! in-memory assembly language statements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Parser.hpp"
#include "ParseContext.hpp"
#include "SyntaxNode.hpp"
#include "Token.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Parses all source code to completion or fatal failure.
//! @param[in] context The object describing the input source(s) and which
//! holds the current state of the parsing process.
//! @param[in] rootNode The root abstract syntax node which is to be completed
//! by the parsing process.
void parseSourceCode(ParseContext &context, ISyntaxNode *rootNode)
{
    // Pair the syntax node stack back down to the root element.
    context.restoreSyntaxStack(0);
    context.pushSyntaxNode(rootNode);

    ISyntaxNode *currentNode = nullptr;

    // Process all tokens in the current input source.
    Token currentToken;

    while (context.tryGetNextToken(currentToken) &&
           context.tryCreateNode(currentToken, currentNode))
    {
        // Keep developing the current node, perhaps with more tokens
        // or by applying it to a parent node and making the parent the
        // current node.
        while (currentNode != nullptr)
        {
            bool nodeExpanded = false;

            // Attempt to augment the node with further tokens.
            while (context.tryGetNextToken(currentToken))
            {
                // Develop the node with more tokens, possibly replacing it.
                ISyntaxNode *replacement = currentNode->applyToken(context,
                                                                   currentToken);

                if (replacement == nullptr)
                {
                    context.ungetToken(currentToken);
                    break;
                }
                else
                {
                    currentNode = replacement;
                    nodeExpanded = true;
                }
            }

            ISyntaxNode *parentNode = nullptr;
            if (currentNode->isComplete() && context.tryPopSyntaxNode(parentNode))
            {
                // Try to apply the current node to the parent node.
                ISyntaxNode *updatedParentNode = parentNode->applyNode(context, currentNode);

                if (updatedParentNode == nullptr)
                {
                    // The current node could not be applied to the parent, i.e.
                    // it is an orphan.
                    context.pushSyntaxNode(parentNode);

                    // TODO: Do something sane here.
                    Ag::safeDelete(currentNode);
                }
                else
                {
                    // Pop the parent node from the stack for further development
                    // now that it has consumed the current node.
                    currentNode = updatedParentNode;
                    nodeExpanded = true;
                }
            }

            if ((currentNode != nullptr) && (nodeExpanded == false))
            {
                // No changes were made to the node on the last iteration.
                if (currentNode->isComplete())
                {
                    parentNode = nullptr;

                    if (context.tryPeekTopSyntaxNode(parentNode))
                    {
                        // It's a complete node which its parent doesn't
                        // want to consume.
                        parentNode->recover(context, currentNode);
                    }
                    else
                    {
                        // It's the root node, put it back.
                        context.pushSyntaxNode(currentNode);
                    }
                }
                else
                {
                    // Push the incomplete node back onto the stack.
                    context.pushSyntaxNode(currentNode);
                }

                // Either way, we don't want to develop this node further
                // at this time.
                currentNode = nullptr;
            }
        } // while (currentNode != nullptr)
    } // context.tryGetNextToken(...) && context.tryCreateNode(...))

    if (currentNode == nullptr)
    {
        if (currentToken.getClass() != TokenClass::Empty)
        {
            // We failed to process the token into a syntax node.
            if (currentToken.getValue().isEmpty())
            {
                context.getMessages().appendError(currentToken.getLocation(),
                                                  "Unexpected token.");
            }
            else
            {
                context.getMessages().appendError(currentToken.getLocation(),
                                                  "Unexpected token '{0}'.",
                                                  { currentToken.getValue() });
            }
        }
    }
    else
    {
        // The input source has been processed, try to revert back to the
        // one which it was referenced from, if any.
        context.pushSyntaxNode(currentNode);
        currentNode = nullptr;
    }
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


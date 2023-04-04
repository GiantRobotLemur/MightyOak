//! @file ParseContext.cpp
//! @brief The definition of an object which holds the state of the parsing
//! process.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Ag/Core.hpp"

#include "InputContext.hpp"
#include "LexicalContext.hpp"
#include "ParseContext.hpp"
#include "SyntaxNode.hpp"
#include "Token.hpp"

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
// ParseContext Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to manage parsing from a set of inputs.
//! @param[in] visitedSources A set of named sources which have already been
//! or are in the process of being parsed, to be shared with nested contexts.
//! @param[in] inputSet The set of inputs to parse then dispose of.
//! @param[in] messages A collection of messages to add to while parsing.
ParseContext::ParseContext(VisitedSourceSet &visitedSources,
                           IInputSetUPtr &&inputSet,
                           Messages &messages) :
    _visitedSources(visitedSources),
    _input(std::move(inputSet)),
    _messages(messages)
{
    _lexicalStack.reserve(4);
    _nodeStack.reserve(32);
}

//! @brief Gets the size of the lexical stack so that it can be reverted
//! at some later point.
size_t ParseContext::getLexicalState() const
{
    return _lexicalStack.size();
}

//! @brief Gets the collection of messages to add to during the parsing process.
Messages &ParseContext::getMessages() const
{
    return _messages;
}

//! @brief Gets the shared set of sources which have been parsed or are
//! currently being parsed.
VisitedSourceSet &ParseContext::getVisitedSources() const
{
    return _visitedSources;
}

//! @brief Gets the object which produces input streams from references in
//! the source code.
IInputSet *ParseContext::getInputSource() const
{
    return _input.get();
}

//! @brief Gets whether any incomplete syntax nodes are currently on the stack.
//! @retval true There is at least one syntax node on the stack.
//! @retval false There are no nodes on the stack.
bool ParseContext::hasIncompleteSyntaxNodes() const
{
    return _nodeStack.empty() == false;
}

//! @brief Attempts to open the primary input source.
//! @param[in] baseLexer The lexical context used to process characters
//! at the base level of parsing.
//! @param[out] error A string to receive an error message on failure.
//! @retval true The input source was successfully opened.
//! @retval false The input source could not be obtained, error should receive
//! a message detailing why.
bool ParseContext::tryBegin(ILexicalContext *baseLexer, String &error)
{
    // Empty the state before starting.
    _lexicalStack.clear();
    _cachedToken.clear();

    IInputSourcePtr inputSource;
    String errorOrId;
    bool isOK = false;

    if (_input->tryGetPrimaryInputSource(inputSource, errorOrId))
    {
        _visitedSources.insert(errorOrId);
        _lexicalStack.push_back(baseLexer);
        _inputContext = std::make_unique<InputContext>(inputSource, Location(),
                                                       errorOrId, 1);
        isOK = true;
    }
    else
    {
        error = errorOrId;
    }

    return isOK;
}

//! @brief Pushes a new context on the lexical stack.
//! @param[in] context The lexical context which the current object which take
//! ownership of.
//! @return The value to pass to restoreLexicalState() to ensure that the
//! previous lexical state is properly restored.
size_t ParseContext::pushLexicalContext(ILexicalContext *context)
{
    size_t previousState = _lexicalStack.size();

    _lexicalStack.push_back(context);

    return previousState;
}

//! @brief Pops the last lexical context pushed onto the lexical stack.
void ParseContext::popLexicalContext()
{
    if (_lexicalStack.empty())
    {
        throw OperationException("Cannot pop from an empty lexical stack.");
    }
    else if (_cachedToken.getClass() == TokenClass::Empty)
    {
        _lexicalStack.pop_back();
    }
    else
    {
        throw OperationException("Cannot pop the lexical stack with an outstanding token.");
    }
}

//! @brief Reverts the lexical stack back to a state previously returned by
//! getLexicalState().
//! @param[in] state The state to restore to.
void ParseContext::restoreLexicalState(size_t state)
{
    while (_lexicalStack.size() > state)
    {
        _lexicalStack.pop_back();
    }
}

//! @brief Pushes a new incomplete syntax node onto the stack of partially
//! complete nodes.
//! @param[in] node The syntax node to push onto the stack.
//! @returns The size of the stack before the new node was added.
size_t ParseContext::pushSyntaxNode(ISyntaxNode *node)
{
    size_t state = _nodeStack.size();

    if (node != nullptr)
    {
        _nodeStack.push_back(node);
    }

    return state;
}

//! @brief Removes and returns the top items from the syntax node stack.
//! @return The top item which was popped or nullptr.
ISyntaxNode *ParseContext::popSyntaxNode()
{
    ISyntaxNode *node = nullptr;

    if (_nodeStack.empty() == false)
    {
        node = _nodeStack.back();
        _nodeStack.pop_back();
    }

    return node;
}

//! @brief Attempts to pop an incomplete syntax node from the top of the stack.
//! @param[out] node Receives the popped node.
//! @retval true A node was successfully popped.
//! @retval false The stack was empty.
bool ParseContext::tryPopSyntaxNode(ISyntaxNode *&node)
{
    bool isPopped = false;

    if (_nodeStack.empty())
    {
        node = nullptr;
    }
    else
    {
        isPopped = true;
        node = _nodeStack.back();
        _nodeStack.pop_back();
    }

    return isPopped;
}

//! @brief Attempts to obtain a syntax node from the top of the stack without
//! popping it.
//! @param[out] node Receives the top node.
//! @retval true The stack contains nodes and the top one was returned.
//! @retval false The stack was empty, node is set to nullptr.
bool ParseContext::tryPeekTopSyntaxNode(ISyntaxNode *&node)
{
    node = nullptr;

    if (_nodeStack.empty() == false)
    {
        node = _nodeStack.back();
    }

    return node != nullptr;
}

//! @brief Disposes of syntax nodes on the top of the stack until it reaches
//! a specified size.
//! @param[in] state The required stack size, possibly returned by
//! pushSyntaxNode().
void ParseContext::restoreSyntaxStack(size_t state)
{
    while (_nodeStack.size() > state)
    {
        ISyntaxNode *node = _nodeStack.back();
        _nodeStack.pop_back();
        safeDelete(node);
    }
}

//! @brief Consumes tokens until a specific class is found to recover from a
//! previous error.
//! @param[in] recoveryMarker The class of token to search for to recover the
//! state of the parser to something predictable.
void ParseContext::recover(TokenClass recoveryMarker)
{
    Token next;

    while (tryGetNextToken(next))
    {
        if (next.getClass() == recoveryMarker)
        {
            break;
        }
    }
}

//! @brief Attempts to get the next token from the input stream.
//! @param[out] next Receives the next recognised token if there is input left
//! to process.
//! @retval true A token was generated, although it maybe erroneous input.
//! @retval false There were not enough characters left in the current input
//! source to generate any token.
bool ParseContext::tryGetNextToken(Token &next)
{
    bool hasToken = false;

    if (_cachedToken.getClass() == TokenClass::Empty)
    {
        if (tryGenerateToken(next))
        {
            hasToken = true;
        }
        else
        {
            next.clear();
        }
    }
    else
    {
        // Use the cached token.
        next = _cachedToken;
        _cachedToken.clear();
        hasToken = true;
    }

    return hasToken;
}

//! @brief Re-inserts a token back into the input stream.
//! @param[in] previous The token to re-insert so that it will be returned by
//! the next call to tryGetToken() or used by the next call to ensureToken().
//! @note Only a single token can be put back at once. The returned token
//! must be consumed before ungetToken() can be called again.
void ParseContext::ungetToken(const Token &previous)
{
    if (_cachedToken.getClass() == TokenClass::Empty)
    {
        _cachedToken = previous;
    }
    else
    {
        throw OperationException("Cannot unget more than one token at once.");
    }
}

//! @brief Peeks at the next token to see if it has a specific classification,
//! if it does, it is consumed.
//! @param[in] classification The class of token to check for.
//! @retval true The next token has a class which matched classification, it
//! has been discarded.
//! @retval false Either there were no further tokens or the next token did
//! not have the required classification.
bool ParseContext::ensureToken(TokenClass classification)
{
    bool isCorrectType = false;

    if (_cachedToken.getClass() == TokenClass::Empty)
    {
        // Try and cache another token.
        if (tryGenerateToken(_cachedToken))
        {
            if (_cachedToken.getClass() == classification)
            {
                isCorrectType = true;
                _cachedToken = Token();
            }
        }
    }
    else if (_cachedToken.getClass() == classification)
    {
        isCorrectType = true;
        _cachedToken = Token();
    }

    return isCorrectType;
}

//! @brief Attempts to create a new abstract syntax node from a single
//! starting token using the current lexical context.
//! @param[in] startToken The token used to determine what kind of syntax node
//! to create and to initially populate it.
//! @param[out] node Receives a pointer to a new syntax node if successful.
//! @retval true The token was consumed to create an object returned in node.
//! @retval false The token was not consumed, no new node was created.
bool ParseContext::tryCreateNode(const Token &startToken, ISyntaxNode *&node)
{
    node = nullptr;
    bool nodeCreated = false;

    if (_lexicalStack.empty() == false)
    {
        ILexicalContext *lexer = _lexicalStack.back();

        nodeCreated = lexer->tryCreateNodeFromStartToken(*this, startToken,
                                                         node);
    }

    return nodeCreated;
}

//! @brief Attempts to generate a new token using the current input and
//! lexical contexts.
//! @param[out] next Receives the generated token, if any.
//! @retval true A token was generated, although it maybe erroneous input.
//! @retval false There were not enough characters left in the current input
//! source to generate any token.
bool ParseContext::tryGenerateToken(Token &next)
{
    bool hasToken = false;

    if (_inputContext && (_lexicalStack.empty() == false))
    {
        ILexicalContext *lexer = _lexicalStack.back();

        while (hasToken == false)
        {
            if (lexer->tryGetNextToken(*_inputContext, next))
            {
                if (next.getClass() == TokenClass::Warning)
                {
                    // Record the warning, but don't return the token.
                    _messages.appendWarning(next.getLocation(), next.getValue());
                }
                else
                {
                    hasToken = true;
                }
            }
            else
            {
                // We have reached the end of the input stream.
                // Indicate that with a statement/line break token at the end
                // of the stream to break any hanging statements or expressions.
                next.reset(_inputContext->getCurrentLocation(), TokenClass::StatementTerminator);
                hasToken = true;

                // Dispose of the input stream to ensure no further tokens
                // are returned.
                _inputContext.reset();
            }
        }
    }

    return hasToken;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file RegisterListNode.cpp
//! @brief The definition of a syntax node which represents a specification of
//! a set of core registers.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"
#include "RegisterListNode.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// RegisterListNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing a single or range of symbols.
//! @param[in] at The source location of the first symbol.
//! @param[in] symbol The first register symbol in the range.
RegisterListNode::RegisterRange::RegisterRange(const Location &at,
                                               Ag::string_cref_t symbol) :
    At(at),
    First(symbol)
{
}

//! @brief Constructs a syntax node representing a partially parsed specification
//! of a set of core registers.
//! @param[in] context The current state of the parser.
RegisterListNode::RegisterListNode(ParseContext &context) :
    BaseSyntaxNode(context),
    _state(State::BeforeOpenBrace)
{
    _ranges.reserve(16);

    context.pushLexicalContext(getCoreRegListLexer());
}

//! @brief Gets the set of ranges accumulated during the parsing process.
const RegisterListNode::SymbolRangeCollection &RegisterListNode::getRanges() const
{
    return _ranges;
}

// Inherited from ISynaxNode.
bool RegisterListNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISynaxNode.
bool RegisterListNode::isValid() const { return _ranges.empty() == false; }

// Inherited from ISynaxNode.
ISyntaxNode *RegisterListNode::applyToken(ParseContext &context, const Token &token)
{
    ISyntaxNode *result = nullptr;
    TokenClass tokenType = token.getClass();

    switch (_state)
    {
    case State::BeforeOpenBrace:
        // Expects an open brace.
        if (tokenType == TokenClass::OpenBrace)
        {
            result = this;
            setStart(token.getLocation());
            _state = State::BeforeFirstReg;
        }
        break;

    case State::BeforeFirstReg:
        // Expects a symbol.
        if (tokenType == TokenClass::Symbol)
        {
            _ranges.emplace_back(token.getLocation(), token.getValue());
            _state = State::AfterFirstReg;
            result = this;
        }
        else if (tokenType == TokenClass::CloseBrace)
        {
            context.getMessages().appendWarning(token.getLocation(),
                                                "Empty register set.");
            _state = State::Complete;
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::AfterFirstReg:
        // Expects a separator, minus or close brace.
        if (tokenType == TokenClass::Comma)
        {
            // It was the specification of a single register.
            _state = State::AfterSeparator;
            result = this;
        }
        else if (tokenType == TokenClass::Minus)
        {
            _state = State::AfterDash;
            result = this;
        }
        else if (tokenType == TokenClass::CloseBrace)
        {
            // It was the specification of a single register at the end
            // of the list.
            _state = State::Complete;
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::AfterDash:
        // Expects a symbol.
        if (tokenType == TokenClass::Symbol)
        {
            if (_ranges.empty() == false)
            {
                // Annotate the last range with the second symbol.
                _ranges.back().Last = token.getValue();
            }

            _state = State::AfterSecondReg;
            result = this;
        }
        break;

    case State::AfterSecondReg:
        // Expects a separator or close brace,
        if (tokenType == TokenClass::Comma)
        {
            _state = State::BeforeFirstReg;
            result = this;
        }
        else if (tokenType == TokenClass::CloseBrace)
        {
            _state = State::Complete;
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::AfterSeparator:
        // Expects a symbol.
        if (tokenType == TokenClass::Symbol)
        {
            _ranges.emplace_back(token.getLocation(), token.getValue());
            _state = State::AfterFirstReg;
            result = this;
        }
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISynaxNode.
ISyntaxNode *RegisterListNode::applyNode(ParseContext &/*context*/, ISyntaxNode */*childNode*/)
{
    // No child nodes are accepted.
    return nullptr;
}

// Inherited from ISynaxNode.
void RegisterListNode::recover(ParseContext &context, ISyntaxNode *node)
{
    // Use the previous lexical analyser as it might have more range.
    restoreLexicalState(context);

    Token next;

    // Ensure the random unrecognised node is disposed of.
    Ag::safeDelete(node);

    while (context.tryGetNextToken(next))
    {
        if ((next.getClass() == TokenClass::StatementTerminator) ||
            (next.getClass() == TokenClass::CloseBrace))
        {
            break;
        }
    }

    _state = State::Complete;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file ParseContext.hpp
//! @brief The declaration of an object which holds the state of the parsing
//! process.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_PARSE_CONTEXT_HPP__
#define __ASM_TOOLS_PARSE_CONTEXT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <unordered_set>

#include "InputContext.hpp"
#include "InputSet.hpp"
#include "Token.hpp"
#include "AsmTools/Messages.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class Token;
class ParseContext;
class ILexicalContext;
class ISyntaxNode;

using VisitedSourceSet = std::unordered_set<Ag::String>;

//! @brief An object which holds the state of the parsing process.
class ParseContext
{
public:
    // Construction/Destruction
    ParseContext() = delete;
    ParseContext(const ParseContext &) = delete;
    ParseContext(ParseContext &&) = delete;
    ParseContext &operator=(const ParseContext &) = delete;
    ParseContext &operator=(ParseContext &&) = delete;

    ParseContext(VisitedSourceSet &visitedSources,
                 IInputSetUPtr &&inputSet,
                 Messages &messages);
    ~ParseContext() = default;

    // Accessors
    size_t getLexicalState() const;
    Messages &getMessages() const;
    VisitedSourceSet &getVisitedSources() const;
    IInputSet *getInputSource() const;
    bool hasIncompleteSyntaxNodes() const;

    // Operations
    bool tryBegin(ILexicalContext *baseLexer, Ag::string_ref_t error);
    size_t pushLexicalContext(ILexicalContext *context);
    void popLexicalContext();
    void restoreLexicalState(size_t state);
    size_t pushSyntaxNode(ISyntaxNode *node);
    ISyntaxNode *popSyntaxNode();
    bool tryPopSyntaxNode(ISyntaxNode *&node);
    bool tryPeekTopSyntaxNode(ISyntaxNode *&node);
    void restoreSyntaxStack(size_t state);

    void recover(TokenClass recoveryMarker);
    bool tryGetNextToken(Token &next);
    void ungetToken(const Token &previous);
    bool ensureToken(TokenClass classification);
    bool tryCreateNode(const Token &startToken, ISyntaxNode *&node);
private:
    // Internal Types
    using ILexicalContextPtr = ILexicalContext *;
    using LexicalStack = std::vector<ILexicalContextPtr>;
    using SyntaxNodeStack = std::vector<ISyntaxNode *>;

    // Internal Functions
    bool tryGenerateToken(Token &next);

    // Internal Fields
    VisitedSourceSet &_visitedSources;
    IInputSetUPtr _input;
    Messages &_messages;
    std::unique_ptr<InputContext> _inputContext;
    LexicalStack _lexicalStack;
    SyntaxNodeStack _nodeStack;
    Token _cachedToken;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

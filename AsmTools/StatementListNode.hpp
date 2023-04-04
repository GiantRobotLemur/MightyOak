//! @file StatementListNode.hpp
//! @brief The declaration of an ISyntaxNode implementation which holds the
//! top level collection of statements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_STATEMENT_LIST_NODE_HPP__
#define __ASM_TOOLS_STATEMENT_LIST_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <unordered_set>

#include "Ag/Core/LinearSortedMap.hpp"
#include "Ag/Core/FsPath.hpp"

#include "AssemblyState.hpp"
#include "ExprContexts.hpp"
#include "ObjectCodeBuilder.hpp"
#include "Statement.hpp"
#include "SyntaxNode.hpp"

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
//! @brief Represents a statement terminator with no content.
class EmptyStatement : public StatementNode
{
public:
    // Construction/Destruction
    EmptyStatement(ParseContext &context, const Location &position);
    virtual ~EmptyStatement() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;

    virtual Statement *compile(Messages &output) const override;
};

//! @brief A base class representing a block of statements which have either
//! already been assembled into object code or require deferred assembly.
class StatementBlock
{
protected:
    // Construction/Destruction
    StatementBlock(uint32_t offset, bool isPreAssebled);

public:
    virtual ~StatementBlock() = default;

    // Accessors
    bool isClosed() const;
    bool isPreAssembled() const;
    uint32_t getAssemblyOffset() const;
    uint32_t getEndOffset() const;

    //! @brief Gets the size of the object code in bytes, whether it has been
    //! assembled or not.
    virtual uint32_t getSize() const = 0;

    // Operations
    //! @brief Appends assembled object code to a buffer.
    //! @param[in] objectCode A collection to append the object code bytes to.
    virtual void appendObjectCode(ObjectCodeBuilder &objectCode) const = 0;

    virtual void finalise();
private:
    // Internal Fields
    uint32_t _offset;
    bool _isPreAssembled;
    bool _isClosed;
};

//! @brief An ISyntaxNode implementation which holds the top level collection
//! of statements.
class StatementListNode : public ISyntaxNode
{
public:
    // Construction/Destruction
    StatementListNode(const AssemblyStateSPtr &initialState,
                      const IScopedContextSPtr &sope,
                      uint32_t baseAddress,
                      uint32_t assemblyOffset = 0);
    virtual ~StatementListNode() = default;

    // Accessors

    // Accessors

    // Operations
    ObjectCode assemble(Messages &messages) const;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node)  override;
private:
    // Internal Types
    using StatementPtr = Statement *;
    using StatementBlockUPtr = std::unique_ptr<StatementBlock>;

    // Internal Functions
    uint32_t getAssemblyOffset() const;
    uint32_t getAssemblyAddress() const;
    void processStatementNode(ParseContext &context,
                              StatementNode *parsedStatment);
    void processIncludedFile(ParseContext &parentContext,
                             string_cref_t inputSourceId,
                             const Location &includedFrom);
    void appendObjectCode(const ObjectCodeBuilder &objectCode);
    void deferAssembly(StatementUPtr &&statement, uint32_t predictedSize);
    IScopedContext *getScope();

    // Internal Fields
    std::vector<StatementBlockUPtr> _blocks;
    std::vector<IScopedContextSPtr> _scopeStack;
    AssemblyStateSPtr _currentState;
    uint32_t _baseAddress;
    uint32_t _initialAssemblyOffset;
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

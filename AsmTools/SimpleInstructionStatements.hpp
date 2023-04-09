//! @file SimpleInstructionStatements.hpp
//! @brief The declaration of various syntax nodes which represent simple ARM
//! instruction statements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_SIMPLE_INSTRUCTION_STATEMENTS_HPP__
#define __ASM_TOOLS_SIMPLE_INSTRUCTION_STATEMENTS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

#include "ExpressionNode.hpp"
#include "Statement.hpp"
#include "Token.hpp"

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A syntax node representing an SWI instruction statement.
class SwiInstructionNode : public StatementNode
{
public:
    // Construction/Destruction
    SwiInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~SwiInstructionNode() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterMnemonic,
        AfterCommentExpr,
        Complete
    };

    // Internal Fields
    ExpressionNodeUPtr _comment;
    ConditionCode _condition;
    State _state;
};

//! @brief A syntax node representing a B or BL instruction statement.
class BranchInstructionNode : public StatementNode
{
public:
    // Construction/Destruction
    BranchInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~BranchInstructionNode() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterMnemonic,
        AfterTargetExpr,
        Complete
    };

    // Internal Fields
    ExpressionNodeUPtr _targetExpr;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    State _state;
};


//! @brief A syntax node representing a MUL, MLA, SMULL, SMLA, UMULL or UMLAL
//! instruction statement.
//! @note Instruction form &lt;mnemonic> &lt;Rd&gt;, &lt;Rm&gt;, &lt;Rs&gt; [, &lt;Rn&gt;]
class MultiplyInstructionNode : public StatementNode
{
public:
    // Construction/Destruction
    MultiplyInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~MultiplyInstructionNode() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterSeparator,
        AfterReg,
        Complete
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _regExprs[4];
    uint8_t _expectedRegCount;
    uint8_t _regCount;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    State _state;
    bool _updatesPsr;
};

//! @brief A syntax node representing MRS instructions for transferring
//! a status register to a core register.
class MoveFromStatusRegNode : public StatementNode
{
public:
    // Construction/Destruction
    MoveFromStatusRegNode(ParseContext &context, const Token &mnemonic);
    virtual ~MoveFromStatusRegNode() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterMnemonic,
        AfterRd,
        BeforePsrReg,
        Complete,
    };

    ExpressionNodeUPtr _rdExpr;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    State _state;
    CoreRegister _source;
};

//! @brief A syntax node representing MSR instructions for transferring
//! a status register from a core register.
class MoveToStatusRegNode : public StatementNode
{
public:
    // Construction/Destruction
    MoveToStatusRegNode(ParseContext &context, const Token &mnemonic);
    virtual ~MoveToStatusRegNode() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterMnemonic,
        AfterPsrReg,
        BeforeSourceExpr,
        BeforeSourceImmediateExpr,
        Complete,
    };

    ExpressionNodeUPtr _rmExpr;
    ExpressionNodeUPtr _immedExpr;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    State _state;
    CoreRegister _rd;
    uint8_t _psrComponents;
};

//! @brief A syntax node representing an BKPT instruction statement.
class BkptInstructionNode : public StatementNode
{
public:
    // Construction/Destruction
    BkptInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~BkptInstructionNode() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterMnemonic,
        AfterCommentExpr,
        Complete
    };

    // Internal Fields
    ExpressionNodeUPtr _comment;
    ConditionCode _condition;
    State _state;
};

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file AluInstructionNode.hpp
//! @brief The declaration of an object which represents a partially-parsed
//! core ALU instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_ALU_INSTRUCTION_NODE_HPP__
#define __ASM_TOOLS_ALU_INSTRUCTION_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

#include "ExpressionNode.hpp"
#include "ShifterOperandNode.hpp"
#include "Statement.hpp"
#include "Token.hpp"

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
//! @brief An object which represents a partially-parsed core ALU instruction.
class AluInstructionNode : public StatementNode
{
public:
    // Public Types
    enum Flags
    {
        UpdatesPsr      = 0x01,
        OverwritesPsr   = 0x02,
        NoRn            = 0x04,
        IsComparison    = 0x08,
    };

    // Construction/Destruction
    AluInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~AluInstructionNode() = default;

    // Accessors

    // Operations

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
    // <mnemonic> Rd,Rn,<shifter operand> or
    // <mnemonic> Rd,<shifter operand>
    enum class State
    {
        AfterMnemonic,
        AfterDestReg,
        BeforeOperand1,
        AfterOperand1,
        BeforeOperand2,
        Complete
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _destRegExpr;
    ExpressionNodeUPtr _operand1Expr;
    ShifterOperandNodeUPtr _operand2;
    State _state;
    uint8_t _flags;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
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

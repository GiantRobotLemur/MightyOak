//! @file CoProcInstructionStatements.hpp
//! @brief The declaration of syntax nodes which represent partially parsed
//! generic ARM co-processor instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_COPROCESSOR_INSTUCTION_STATEMENTS_HPP__
#define __ASM_TOOLS_COPROCESSOR_INSTUCTION_STATEMENTS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

#include "AddressOperandNode.hpp"
#include "ExpressionNode.hpp"
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
//! @brief A syntax node representing a generic ARM co-processor data processing
//! or register transfer instruction syntax node.
class CoProcDataProcInstructionNode : public StatementNode
{
public:
    // Construction/Destruction
    CoProcDataProcInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~CoProcDataProcInstructionNode() = default;

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
    // CDP{<cond>}     <cpId>, <opCode1>, CRd, CRn, CRm, <opCode2>
    // MRC/MCR{<cond>} <cpId>, <opCode1>, Rd, CRn, CRm, <opCode2>
    enum State
    {
        BeforeProcID,
        AfterProcID,
        BeforeOpCode1,
        AfterOpCode1,
        BeforeRd,
        AfterRd,
        BeforeRn,
        AfterRn,
        BeforeRm,
        AfterRm,
        BeforeOpCode2,

        Complete,
    };

    // Internal Fields
    ExpressionNodeUPtr _cpIdExpr;
    ExpressionNodeUPtr _opCode1Expr;
    ExpressionNodeUPtr _opCode2Expr;
    ExpressionNodeUPtr _rdExpr;
    ExpressionNodeUPtr _rnExpr;
    ExpressionNodeUPtr _rmExpr;
    size_t _lexicalBaseState;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    State _state;
};

//! @brief A syntax node representing a generic ARM co-processor data transfer
//! instruction syntax node.
class CoProcDataTransferInstructionNode : public StatementNode
{
public:
    // Construction/Destruction
    CoProcDataTransferInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~CoProcDataTransferInstructionNode() = default;

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
    // <opcode>{<cond>}{L} <cp#>, CRd, <PC-Relative Addr>
    // <opcode>{<cond>}{L} <cp#>, CRd, [Rn, #+/-(<8_bit_offset>*4)]
    // <opcode>{<cond>}{L} <cp#>, CRd, [Rn, #+/-(<8_bit_offset>*4)]!
    // <opcode>{<cond>}{L} <cp#>, CRd, [Rn], #+/-(<8_bit_offset>*4)
    enum State
    {
        BeforeProcID,
        AfterProcID,
        BeforeRd,
        AfterRd,
        BeforeAddress,
        Complete,
    };

    // Internal Fields
    ExpressionNodeUPtr _cpIdExpr;
    ExpressionNodeUPtr _rdExpr;
    AddressOperandNodeUPtr _addrNode;
    State _state;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    bool _longMode;
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

//! @file MultiTransferInstructionNode.hpp
//! @brief The declaration of a syntax node which represents a partially parsed
//! core multi-register data transfer instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_MULTI_TRANSFER_INSTRUCTION_NODE_HPP__
#define __ASM_TOOLS_MULTI_TRANSFER_INSTRUCTION_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"
#include "ExpressionNode.hpp"
#include "RegisterListNode.hpp"
#include "Statement.hpp"

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
class ParseContext;
class Token;

//! @brief A syntax node which represents a partially parsed
//! core multi-register data transfer instruction.
class MultiTransferInstructionNode : public StatementNode
{
public:
    // Public Types
    enum Flags
    {
        Writeback       = 0x01,
        UserModeRegs    = 0x02,
    };

    // Construction/Destruction
    MultiTransferInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~MultiTransferInstructionNode() = default;

    // Accessors

    // Operations

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;

    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterMnemonic,
        AfterDestReg,
        AfterWriteback,
        BeforeRegisterList,
        AfterRegList,
        AfterHat,

        Complete,
    };

    using RegisterListNodeUPtr = std::unique_ptr<RegisterListNode>;

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _destReg;
    RegisterListNodeUPtr _regList;
    State _state;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    MultiTransferMode _mode;
    uint8_t _flags;
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

//! @file DataTransferInstructionNode.hpp
//! @brief The declaration of an object which parses a core single register
//! data transfer instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_DATA_TRANSFER_INSTRUCTION_NODE_HPP__
#define __ASM_TOOLS_DATA_TRANSFER_INSTRUCTION_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

#include "AddressOperandNode.hpp"
#include "ExpressionNode.hpp"
#include "ShifterOperandNode.hpp"

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
//! @brief an object which parses a core single register
//! data transfer instruction.
class DataTransferInstructionNode : public StatementNode
{
public:
    // Public Types
    enum Flags
    {
        UserModePrivilages      = 0x01,
        PreIndexedAddressing    = 0x02,
        PCRelativeAddressing    = 0x04,
        NegativeOffset          = 0x08,
        WriteBack               = 0x10,
        HasOffset               = 0x20,
    };

    // Construction/Destruction
    DataTransferInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~DataTransferInstructionNode() = default;

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
    // <mnemonic> Rd, <addr operand>
    enum class State
    {
        AfterMnemonic,
        AfterBaseReg,
        BeforeAddr,

        Complete
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _destReg;
    AddressOperandNodeUPtr _addr;
    State _state;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    TransferDataType _dataType;
    bool _userPrivilages;
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

//! @file AtomicSwapInstructionNode.hpp
//! @brief The declaration of a syntax node representing the SWP instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_ATOMIC_SWAP_INSTRUCTION_NODE_HPP__
#define __ASM_TOOLS_ATOMIC_SWAP_INSTRUCTION_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"
#include "ExpressionNode.hpp"
#include "Statement.hpp"

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
class ParseContext;

//! @brief A syntax node representing the SWP instruction.
class AtomicSwapInstructionNode : public StatementNode
{
public:
    // Construction/Destruction
    AtomicSwapInstructionNode(ParseContext &context, const Token &mnemonic);
    virtual ~AtomicSwapInstructionNode() = default;

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
        BeforeSourceReg,
        AfterSourceReg,
        BeforeBase,
        BeforeBaseReg,
        AfterBaseReg,

        Complete,
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _destReg;
    ExpressionNodeUPtr _sourceReg;
    ExpressionNodeUPtr _baseReg;

    State _state;
    ConditionCode _condition;
    bool _isByte;
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

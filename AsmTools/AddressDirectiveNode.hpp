//! @file AddressDirectiveNode.hpp
//! @brief The declaration of a syntax node representing a partially parsed ADR
//! pseudo instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_ADDRESS_DIRECTIVE_NODE_HPP__
#define __ASM_TOOLS_ADDRESS_DIRECTIVE_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"
#include "ExpressionNode.hpp"
#include "Statement.hpp"

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
class ParseContext;
class Token;

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A syntax node representing a partially parsed ADR
//! pseudo instruction.
class AddressDirectiveNode : public StatementNode
{
public:
    // Construction/Destruction
    AddressDirectiveNode(ParseContext &context, const Token &mnemonic);
    virtual ~AddressDirectiveNode() = default;

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
        BeforeAddress,
        Complete,
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _destRegExpr;
    ExpressionNodeUPtr _addrExpr;
    size_t _oldLexerState;
    ConditionCode _condtion;
    MultiWordEncoding _encoding;
    State _state;
};

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

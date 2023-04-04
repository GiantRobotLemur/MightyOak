//! @file AddressOperandNode.hpp
//! @brief The declaration of a syntax node which represents a memory operand.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_ADDRESS_OPERAND_NODE_HPP__
#define __ASM_TOOLS_ADDRESS_OPERAND_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "BaseSyntaxNode.hpp"
#include "ShifterOperandNode.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the addressing mode 
enum class AddressingMode
{
    PreIndexed,
    PostIndexed,
    PCRelative, // Really a special case of PreIndexed addressing.
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class StatementAddressOperand;

//! @brief A syntax node which represents a memory operand.
class AddressOperandNode : public BaseSyntaxNode
{
public:
    // Public Types
    enum Flags
    {
        IsPCRelative    = 0x01,
        IsPreIndexed    = 0x02,
        WriteBack       = 0x04,
        NegativeOffset  = 0x08,
    };

    // Construction/Destruction
    AddressOperandNode(ParseContext &context);
    virtual ~AddressOperandNode() = default;

    // Accessors

    // Operations
    bool compile(StatementAddressOperand &statement,
                 Messages &log) const;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
private:
    // Internal Types
    // <PC-relative addr expression> <statement terminator>
    // '[' Rn ',' <shifter operand>] ']'[!] <statement terminator>
    // '[' Rn ']' [, <shifter operand>] <statement terminator>
    enum class State
    {
        BeforeOperand,

        BeforeBaseReg,
        AfterBaseReg,
        BeforePreIndexOffset,
        AfterPreIndexAddr,
        AfterWriteBack,

        AfterPostIndexBase,
        BeforePostIndexOffset,

        Complete,
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _base;
    ShifterOperandNodeUPtr _offset;
    Location _start;
    uint8_t _flags;
    State _state;
};

//! @brief An alias for a unique pointer to a AddressOperandNode syntax node.
using AddressOperandNodeUPtr = std::unique_ptr<AddressOperandNode>;

//! @brief An object representing a fully parsed address operand.
class StatementAddressOperand
{
public:
    // Construction/Destruction
    StatementAddressOperand();
    StatementAddressOperand(StatementAddressOperand &&rhs);
    ~StatementAddressOperand() = default;

    // Accessors
    AddressingMode getAddressMode() const;
    const IExpr *getBaseAddress() const;
    const StatementShifterOperand *getOffset() const;

    // Operations
    void initialisePCRelative(const Location &at, IExprUPtr &&addrExpr);
    void initialiseIndexed(const Location &at, IExprUPtr &&baseRegExpr,
                           StatementShifterOperandUPtr &&offsetExpr,
                           uint32_t flags);

    bool configure(InstructionInfo &instruction, IEvalContext *context,
                   Messages &log, bool isFinalPass) const;
private:
    // Internal Functions
    bool tryResolve(IEvalContext *context, Messages &log, bool isFinalPass,
                    AddrOperand &operand, uint32_t maxOffset) const;

    // Internal Fields
    Location _start;
    IExprUPtr _baseExpr;
    StatementShifterOperandUPtr _offset;
    AddressingMode _mode;
    bool _hasWriteback;
};

//! @brief An alias for a unique pointer to an StatementAddressOperand.
using StatementAddressOperandUPtr = std::unique_ptr<StatementAddressOperand>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

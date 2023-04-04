//! @file ShifterOperandNode.hpp
//! @brief The declaration of an object which represents a partially parsed
//! barrel shifter operand.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_SHIFTER_OPERAND_NODE_HPP__
#define __ASM_TOOLS_SHIFTER_OPERAND_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

#include "BaseSyntaxNode.hpp"
#include "ExpressionNode.hpp"
#include "Expr.hpp"
#include "Statement.hpp"
#include "Token.hpp"

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
class ShifterParentStatementNode;
class StatementShifterOperand;

//! @brief An object which represents a partially parsed barrel shifter operand.
class ShifterOperandNode : public BaseSyntaxNode
{
public:
    // Construction/Destruction
    ShifterOperandNode(ParseContext &context, bool allowSign,
                       TokenClass expectedEnd = TokenClass::StatementTerminator);
    virtual ~ShifterOperandNode() = default;

    // Accessors
    bool isNegative() const;

    // Operations
    bool compile(StatementShifterOperand &operand,
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
    // #<immediate constant>
    // <Reg> <End>
    // <Reg>, <shift> <Reg> <End>
    // <Reg>, <shift> #<immediate constant> <End>
    // <Reg>, RRX <End>
    enum class State
    {
        Start,
        AfterInitialSign,
        AfterInitialHash,
        AfterInitialReg,
        BeforeShift,
        AfterShift,
        AfterShiftHash,
        AfterShiftExpr,
        Complete,
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _baseExpr;
    ExpressionNodeUPtr _shiftExpr;
    TokenClass _expectedEnd;
    bool _allowSign;
    bool _isNegative;
    State _state;
    ShifterMode _mode;
    ShiftType _shiftType;
};

//! @brief Represents a fully parsed shifter operand.
class StatementShifterOperand
{
public:
    // Construction/Destruction
    StatementShifterOperand();
    StatementShifterOperand(StatementShifterOperand &&rhs) noexcept;
    ~StatementShifterOperand() = default;

    // Accessors
    bool isNegated() const;

    // Operations
    void initialiseConstant(const Location &at, IExprUPtr &&immediateExpr,
                            bool allowNegative);
    void initialiseRegister(const Location &at, IExprUPtr &&rmExpr, bool isNegated);
    void initialiseRrxShift(const Location &at, IExprUPtr &&rmExpr, bool isNegated);
    void initialiseShiftByRegister(const Location &at, IExprUPtr &&rmExpr,
                                   ShiftType shift, IExprUPtr &&rsExpr,
                                   bool isNegated);
    void initialiseShiftByConstant(const Location &at, IExprUPtr &&rmExpr,
                                   ShiftType shift, IExprUPtr &&constExpr,
                                   bool isNegated);
    bool configure(IEvalContext *context, ShifterOperand &operand,
                   bool &wasNegated, Messages &log, bool isFinalPass) const;
private:
    // Internal Fields
    Location _start;
    IExprUPtr _baseExpr;
    IExprUPtr _shiftExpr;
    ShifterMode _mode;
    ShiftType _shiftType;
    bool _allowsNegated;
    bool _isNegated;
};

//! @brief An alias for a unique pointer to a ShifterOperandNode syntax node.
using ShifterOperandNodeUPtr = std::unique_ptr<ShifterOperandNode>;

//! @brief An alias for a unique pointer to a StatementShifterOperand.
using StatementShifterOperandUPtr = std::unique_ptr<StatementShifterOperand>;
////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

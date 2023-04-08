//! @file AluInstructionNode.cpp
//! @brief The definition of an object which represents a partially-parsed
//! core ALU instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AluInstructionNode.hpp"
#include "ConstantSet.hpp"
#include "InstructionStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"
#include "ShifterOperandNode.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
class AluInstructionStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _rd;
    IExprUPtr _rn;
    StatementShifterOperand _shifterOperand;
    uint8_t _flags;

public:
    // Construction/Destruction
    AluInstructionStatement(const Location &start,
                            OperationClass opClass,
                            InstructionMnemonic mnemonic,
                            ConditionCode condition,
                            IExprUPtr &&rd,
                            IExprUPtr &&rn,
                            StatementShifterOperand &&shifterOperand,
                            uint8_t flags) :
        InstructionStatement(start, opClass, mnemonic, condition),
        _rd(std::move(rd)),
        _rn(std::move(rn)),
        _shifterOperand(std::move(shifterOperand)),
        _flags(flags)
    {
    }

protected:
    // Overrides
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        bool isOK = true;
        CoreRegister rd = CoreRegister::R0;
        CoreRegister rn = CoreRegister::R0;
        ShifterOperand op2;
        String error;
        bool wasNegated = false;

        // Evaluate operand 2 (the barrel shifter operand).
        isOK = _shifterOperand.configure(context, op2, wasNegated,
                                         log, isFinalPass);

        if (isFinalPass && (isOK == false))
        {
            // The shifter operand could not be configured.
            std::string message("Could not configure barrel shifter operand: ");
            appendAgString(message, error);

            log.appendError(getStart(), String(message));
        }

        // Evaluate the destination register.
        if (tryEvaluateCoreRegister(context, _rd.get(),
                                    rd, error) == false)
        {
            isOK = false;

            if (isFinalPass)
            {
                // The shifter operand could not be configured.
                std::string message("Could not evaluate base register expression: ");
                appendAgString(message, error);

                log.appendError(getStart(), String(message));
            }
        }

        // Optionally evaluate operand 1.
        if (((_flags & AluInstructionNode::NoRn) == 0) &&
            (tryEvaluateCoreRegister(context, _rn.get(), rn, error) == false))
        {
            isOK = false;

            if (isFinalPass)
            {
                // The shifter operand could not be configured.
                std::string message("Could not evaluate source register expression: ");
                appendAgString(message, error);

                log.appendError(getStart(), String(message));
            }
        }

        if (_flags & AluInstructionNode::IsComparison)
        {
            auto &params = instruction.getCoreCompareParamters();
            params.Rn = rd;
            params.Op2 = op2;
            params.OverwritesPSR = ((_flags & AluInstructionNode::OverwritesPsr) != 0);

            if (_flags & AluInstructionNode::UpdatesPsr)
            {
                log.appendWarning(getStart(),
                                  "A core ALU comparison instruction does not need the 'S' suffix, it is implicit.");
            }
        }
        else
        {
            auto &params = instruction.getCoreAluParamters();
            params.Rd = rd;
            params.Rn = rn;
            params.Op2 = op2;
            params.AffectsFlags = ((_flags & AluInstructionNode::UpdatesPsr) != 0);

            if (_flags & AluInstructionNode::OverwritesPsr)
            {
                log.appendError(getStart(),
                                "A core ALU processing instruction cannot have the 'P' suffix.");
            }
        }

        return isOK;
    }

    // Inherited from InstructionStatement.
    virtual bool validate(const InstructionInfo &instruction,
                          const AssemblyState &state,
                          Messages &log) const override
    {
        bool isValid = true;

        if ((instruction.getOperationClass() == OperationClass::CoreCompare) &&
            instruction.getCoreCompareParamters().OverwritesPSR)
        {
            // Using the 'P' suffix is only valid in 26-bit mode.
            isValid = validateAddressMode(state, log, AddressMode::Bits26);
        }

        return isValid;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// AluInstructionNode Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node representing an ALU instruction statement.
//! @param[in] context The current state of the parser.
//! @param[in] mnemonic The token representing the instruction mnemonic.
AluInstructionNode::AluInstructionNode(ParseContext &context, const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _state(State::AfterMnemonic),
    _flags(0),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::Adc)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al))
{
    context.pushLexicalContext(getExpressionLexer());

    if (getTokenFlag(mnemonic, TokenProperty::OverwritePsr, false))
    {
        _flags |= OverwritesPsr;
    }

    if (getTokenFlag(mnemonic, TokenProperty::UpdatePsr, false))
    {
        _flags |= UpdatesPsr;
    }

    switch (_mnemonic)
    {
    case InstructionMnemonic::Cmp:
    case InstructionMnemonic::Cmn:
    case InstructionMnemonic::Tst:
    case InstructionMnemonic::Teq:
        // Ensure the first operand is skipped for these instruction types.
        _flags |= NoRn;
        _flags |= IsComparison;
        break;

    case InstructionMnemonic::Mov:
    case InstructionMnemonic::Mvn:
        // Ensure the first operand is skipped for these instruction types.
        _flags |= NoRn;
        break;

    default:
        break;
    }
}

// Inherited from ISyntaxNode.
bool AluInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool AluInstructionNode::isValid() const
{
    bool isValid = false;

    if (_flags & NoRn)
    {
        isValid = _destRegExpr && _operand2 && _operand2->isValid();
    }
    else
    {
        isValid = _destRegExpr && _operand1Expr &&
                  _operand2 && _operand2->isValid();
    }

    return isValid;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AluInstructionNode::applyToken(ParseContext &context,
                                            const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic:
        // Expecting destination register expression.
        break;

    case State::AfterDestReg:
        // Expecting a comma.
        if (token.getClass() == TokenClass::Comma)
        {
            if (_flags & NoRn)
            {
                // We expect no operand 1, so start working on the
                // shifter operand.
                // Put the current node on the stack of incomplete syntax
                // nodes and start working on the shifter operand.
                context.pushSyntaxNode(this);
                auto shifter = new ShifterOperandNode(context, false);
                result = shifter;
                _state = State::BeforeOperand2;
            }
            else
            {
                // We expect expression giving the first operand.
                result = this;
                _state = State::BeforeOperand1;
                context.pushLexicalContext(getExpressionLexer());
            }
        }
        break;

    case State::BeforeOperand1:
        // Expecting a register expression.
        break;

    case State::AfterOperand1:
        // Expecting a comma.
        if (token.getClass() == TokenClass::Comma)
        {
            // Put the current node on the stack of incomplete syntax
            // nodes and start working on the shifter operand.
            context.pushSyntaxNode(this);
            auto shifter = new ShifterOperandNode(context, false);
            result = shifter;
            _state = State::BeforeOperand2;
        }
        break;

    case State::BeforeOperand2:
        // Expect a shifter operand node.
        break;

    default:
    case State::Complete:
        // Nothing left to do.
        break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AluInstructionNode::applyNode(ParseContext &context,
                                           ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr expr = nullptr;
    ShifterOperandNode *shifterNode = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic:
        // Expect a register expression.
        if (tryCast(childNode, expr))
        {
            _destRegExpr.reset(expr);
            _state = State::AfterDestReg;
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::AfterDestReg:
        // Expect a comma token.
        break;

    case State::BeforeOperand1:
        // Expect a register expression.
        if (tryCast(childNode, expr))
        {
            _operand1Expr.reset(expr);
            _state = State::AfterOperand1;
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::AfterOperand1:
        // Expect a comma token.
        break;

    case State::BeforeOperand2:
        // Expect the shifter operand node to have been completed.
        if (tryCast(childNode, shifterNode))
        {
            // Take ownership of the completed shifter operand.
            _operand2.reset(shifterNode);
            _state = State::Complete;
            result = this;
        }
        break;

    case State::Complete:
        // Nothing left to do.
        break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void AluInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;
    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *AluInstructionNode::compile(Messages &output) const
{
    AluInstructionStatement *statement = nullptr;
    OperationClass opClass = (_flags & IsComparison) ? OperationClass::CoreCompare :
                                                       OperationClass::CoreAlu;
    IExprUPtr rd;
    IExprUPtr rn;
    StatementShifterOperand op2;
    bool isOK = true;

    if (_destRegExpr)
    {
        rd.reset(constantOptimise(_destRegExpr->compile(getCoreRegSymbols())));

        if (!rd)
        {
            output.appendError(getStart(),
                               "Failed to compile destination register expression.");
            isOK = false;
        }
    }

    if (_operand1Expr)
    {
        rn.reset(constantOptimise(_operand1Expr->compile(getCoreRegSymbols())));

        if (!rn)
        {
            output.appendError(getStart(),
                               "Failed to compile source register expression.");
            isOK = false;
        }
    }

    if (_operand2)
    {
        isOK = _operand2->compile(op2, output);
    }
    else
    {
        isOK = false;
    }

    if (isOK)
    {
        statement = new AluInstructionStatement(getStart(), opClass,
                                                _mnemonic, _condition,
                                                std::move(rd),
                                                std::move(rn),
                                                std::move(op2),
                                                _flags);
    }

    return statement;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


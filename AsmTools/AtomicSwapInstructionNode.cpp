//! @file AtomicSwapInstructionNode.cpp
//! @brief The definition of a syntax node representing the SWP instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AtomicSwapInstructionNode.hpp"
#include "ConstantSet.hpp"
#include "Expr.hpp"
#include "ExprContexts.hpp"
#include "InstructionStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"


////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing a fully parsed core multi-register data
//! transfer instruction.
class SwapInstructionStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _rd;
    IExprUPtr _rm;
    IExprUPtr _rn;
    bool _isByte;

public:
    //! @brief Constructs an object representing a fully parsed SWP instruction
    //! statement.
    //! @param[in] start The source code location of the beginning of the statement.
    //! @param[in] condition The condition code on the instruction mnemonic.
    //! @param[in] rd The expression representing the destination register.
    //! @param[in] rm The expression representing the source register.
    //! @param[in] rn The expression representing the base register.
    //! @param[in] isByte True to indicate an 8-bit swap, false for a 32-bit word.
    SwapInstructionStatement(const Location &start,
                             ConditionCode condition,
                             IExprUPtr &&rd, IExprUPtr &&rm, IExprUPtr &&rn,
                             bool isByte) :
        InstructionStatement(start, OperationClass::AtomicSwap,
                             InstructionMnemonic::Swp, condition),
        _rd(std::move(rd)),
        _rm(std::move(rm)),
        _rn(std::move(rn)),
        _isByte(isByte)
    {
    }

protected:
    // Overrides
    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        bool isOK = true;
        auto &info = instruction.getCoreSwapParameters();
        info.IsByte = _isByte;

        // Resolve the destination register.
        String error;

        if ((tryEvaluateCoreRegister(context, _rd.get(), info.Rd, error) == false) &&
            isFinalPass)
        {
            std::string builder("Failed to evaluate destination register expression: ");
            appendAgString(builder, error);

            log.appendError(getStart(), builder);
            isOK = false;
        }
        else if ((tryEvaluateCoreRegister(context, _rm.get(), info.Rm, error) == false) &&
                 isFinalPass)
        {
            std::string builder("Failed to evaluate source register expression: ");
            appendAgString(builder, error);

            log.appendError(getStart(), builder);
            isOK = false;
        }
        else if ((tryEvaluateCoreRegister(context, _rn.get(), info.Rn, error) == false) &&
                 isFinalPass)
        {
            std::string builder("Failed to evaluate base register expression: ");
            appendAgString(builder, error);

            log.appendError(getStart(), builder);
            isOK = false;
        }

        return isOK;
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
// AtomicSwapInstructionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node which can parse a SWP instruction statement.
//! @param[in] context The current state of the parser.
//! @param[in] mnemonic The token representing the SWP instruction mnemonic.
AtomicSwapInstructionNode::AtomicSwapInstructionNode(ParseContext &context,
                                                     const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _state(State::AfterMnemonic),
    _condition(mnemonic.getProperty(TokenProperty::ConditionCode, ConditionCode::Al)),
    _isByte(mnemonic.getProperty(TokenProperty::TransferDataType,
                                 TransferDataType::Word) == TransferDataType::UnsignedByte)
{
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool AtomicSwapInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool AtomicSwapInstructionNode::isValid() const
{
    return _destReg && _sourceReg && _baseReg;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AtomicSwapInstructionNode::applyToken(ParseContext &context,
                                                   const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic: // Expect Rd expression.
        break;

    case State::AfterDestReg: // Expect comma.
        if (token.getClass() == TokenClass::Comma)
        {
            result = this;
            _state = State::BeforeSourceReg;
        }
        break;

    case State::BeforeSourceReg: // expect Rm expression.
        break;

    case State::AfterSourceReg: // Expect comma.
        if (token.getClass() == TokenClass::Comma)
        {
            result = this;
            _state = State::BeforeBase;
        }
        break;

    case State::BeforeBase: // Expect open square '['
        if (token.getClass() == TokenClass::OpenSquare)
        {
            result = this;
            _state = State::BeforeBaseReg;
        }
        break;

    case State::BeforeBaseReg: // Expect Rn expression.
        break;

    case State::AfterBaseReg: // Expect close square ']'
        if (token.getClass() == TokenClass::CloseSquare)
        {
            result = this;
            _state = State::Complete;
            restoreLexicalState(context);
        }
        break;

    case State::Complete:
    default:
        break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AtomicSwapInstructionNode::applyNode(ParseContext &/*context*/,
                                                  ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr exprNode;

    switch (_state)
    {
    case State::AfterMnemonic: // Expect Rd expression.
        if (tryCast(childNode, exprNode))
        {
            _destReg.reset(exprNode);
            result = this;
            _state = State::AfterDestReg;
        }
        break;

    case State::AfterDestReg: // Expect comma.
        break;

    case State::BeforeSourceReg: // expect Rm expression.
        if (tryCast(childNode, exprNode))
        {
            _sourceReg.reset(exprNode);
            result = this;
            _state = State::AfterSourceReg;
        }
        break;

    case State::AfterSourceReg: // Expect comma.
        break;

    case State::BeforeBase: // Expect open square '['
        break;

    case State::BeforeBaseReg: // Expect Rn expression.
        if (tryCast(childNode, exprNode))
        {
            _baseReg.reset(exprNode);
            result = this;
            _state = State::AfterBaseReg;
        }
        break;

    case State::AfterBaseReg: // Expect close square ']'
        break;

    case State::Complete:
    default:
        break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void AtomicSwapInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode
Statement *AtomicSwapInstructionNode::compile(Messages &/*output*/) const
{
    IExprUPtr rdExpr(constantOptimise(_destReg->compile(getCoreRegSymbols())));
    IExprUPtr rmExpr(constantOptimise(_sourceReg->compile(getCoreRegSymbols())));
    IExprUPtr rnExpr(constantOptimise(_baseReg->compile(getCoreRegSymbols())));

    return new SwapInstructionStatement(getStart(), _condition, std::move(rdExpr),
                                        std::move(rmExpr), std::move(rnExpr),
                                        _isByte);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


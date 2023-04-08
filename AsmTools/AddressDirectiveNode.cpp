//! @file AddressDirectiveNode.cpp
//! @brief The definition of a syntax node representing a partially parsed ADR
//! pseudo instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AddressDirectiveNode.hpp"
#include "ConstantSet.hpp"
#include "Expr.hpp"
#include "InstructionStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"
#include "Token.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing a statement encoding the ADR pseudo-instruction.
class AdrInstruction : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _rd;
    IExprUPtr _addr;

public:
    // Construction/Destruction
    //! @brief Constructs a statement representing an ADR pseudo-instruction.
    //! @param[in] at The location of the start of the statement in source code.
    //! @param[in] condition The condition code to encode with the instruction.
    //! @param[in] destReg The destination register expression to take ownership of.
    //! @param[in] addr The address expression to take ownership of.
    AdrInstruction(const Location &at, ConditionCode condition,
                   IExprUPtr &&destReg, IExprUPtr &&addr) :
        InstructionStatement(at, OperationClass::CoreAddress,
                             InstructionMnemonic::Adr, condition),
        _rd(std::move(destReg)),
        _addr(std::move(addr))
    {
    }

    virtual ~AdrInstruction() = default;

protected:
    // Overrides
    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        bool isOK = false;

        if (_rd && _addr)
        {
            String error;
            auto &info = instruction.getCoreAddressParameters();

            if (tryEvaluateCoreRegister(context, _rd.get(), info.Rd, error) == false)
            {
                if (isFinalPass)
                {
                    std::string builder("Failed to evaluate destination "
                                        "register expression: ");
                    appendAgString(builder, error);

                    log.appendError(getStart(), error);
                }
            }
            else if (tryEvaluateOrdinal(context, _addr.get(), info.Address, error) == false)
            {
                if (isFinalPass)
                {
                    std::string builder("Failed to evaluate address expression: ");
                    appendAgString(builder, error);

                    log.appendError(getStart(), error);
                }
            }
            else
            {
                isOK = true;
            }
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
// AddressDirectiveNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node to parse the ADR pseudo-instruction.
//! @param[in] context The state of the parsing process.
//! @param[in] mnemonic The token giving the instruction mnemonic.
AddressDirectiveNode::AddressDirectiveNode(ParseContext &context,
                                           const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _oldLexerState(context.pushLexicalContext(getExpressionLexer())),
    _condtion(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _state(State::AfterMnemonic)
{
}

// Inherited from ISyntaxNode.
bool AddressDirectiveNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool AddressDirectiveNode::isValid() const { return _destRegExpr && _addrExpr; }

// Inherited from ISyntaxNode.
ISyntaxNode *AddressDirectiveNode::applyToken(ParseContext &/*context*/, const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic:
        // Expect destination register.
        break;

    case State::AfterDestReg:
        // Expects comma.
        if (token.getClass() == TokenClass::Comma)
        {
            result = this;
            _state = State::BeforeAddress;
        }
        break;

    case State::BeforeAddress:
        // Expects address expression.
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AddressDirectiveNode::applyNode(ParseContext &context, ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr expr = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic:
        // Expect destination register.
        if (tryCast(childNode, expr))
        {
            _destRegExpr.reset(expr);
            _state = State::AfterDestReg;
            result = this;
        }
        break;

    case State::AfterDestReg:
        // Expects comma.
        break;

    case State::BeforeAddress:
        // Expects address expression.
        if (tryCast(childNode, expr))
        {
            _addrExpr.reset(expr);
            _state = State::Complete;
            result = this;
            context.restoreLexicalState(_oldLexerState);
        }
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void AddressDirectiveNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;
    context.restoreLexicalState(_oldLexerState);

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *AddressDirectiveNode::compile(Messages &/*output*/) const
{
    Statement *statement = nullptr;

    if (_destRegExpr && _addrExpr)
    {
        IExprUPtr rd(constantOptimise(_destRegExpr->compile(getCoreRegSymbols())));
        IExprUPtr addr(constantOptimise(_addrExpr->compile(ConstantSet::Empty)));

        statement = new AdrInstruction(getStart(), _condtion,
                                       std::move(rd), std::move(addr));
    }

    return statement;
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file MultiTransferInstructionNode.cpp
//! @brief The definition of a syntax node which represents a partially parsed
//! core multi-register data transfer instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ConstantSet.hpp"
#include "Expr.hpp"
#include "ExprContexts.hpp"
#include "InstructionStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "MultiTransferInstructionNode.hpp"
#include "ParseContext.hpp"
#include "RegisterListNode.hpp"
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
//! @brief An object representing a fully parsed core multi-register data
//! transfer instruction.
class MultiTransferInstructionStatement : public InstructionStatement
{
private:
    // Internal Types
    using CoreRegPair = std::pair<CoreRegister, CoreRegister>;

    // Internal Fields
    IExprUPtr _rd;
    RegisterListNode::SymbolRangeCollection _regRanges;
    MultiTransferMode _mode;
    uint8_t _flags;

    // Internal Functions
    static bool tryEvalRange(IEvalContext *context, Messages &log, bool logErrors,
                             const RegisterListNode::RegisterRange &range,
                             CoreRegPair &resolved)
    {
        CoreRegister first = CoreRegister::R0;
        CoreRegister last = CoreRegister::R0;
        Value value, regValue;
        bool isOK = true;

        if (getCoreRegSymbols().tryLookupValue(range.First, value) &&
            value.tryConvert(DataType::Uint32, regValue))
        {
            first = fromScalar<CoreRegister>(static_cast<uint8_t>(regValue.asUint32()));
        }
        else
        {
            // Evaluate the symbol as an expression.
            String error;
            SymbolExpr registerSymbol(range.At, range.First);
            if (tryEvaluateCoreRegister(context, &registerSymbol, first, error) == false)
            {
                if (logErrors)
                {
                    std::string builder("Failed to evaluate transfer register expression: ");
                    appendAgString(builder, error);

                    log.appendError(range.At, builder);
                }

                first = CoreRegister::R0;
                isOK = false;
            }
        }

        if (range.Last.isEmpty())
        {
            last = first;
        }
        else if (getCoreRegSymbols().tryLookupValue(range.Last, value) &&
                 value.tryConvert(DataType::Uint32, regValue))
        {
            last = fromScalar<CoreRegister>(static_cast<uint8_t>(regValue.asUint32()));
        }
        else
        {
            // Evaluate the symbol as an expression.
            String error;
            SymbolExpr registerSymbol(range.At, range.Last);
            if (tryEvaluateCoreRegister(context, &registerSymbol, last, error) == false)
            {
                if (logErrors)
                {
                    std::string builder("Failed to evaluate transfer register expression: ");
                    appendAgString(builder, error);

                    log.appendError(range.At, builder);
                }

                last = first;
                isOK = false;
            }
        }

        if (isOK)
        {
            if (first > last)
            {
                if (logErrors)
                {
                    log.appendWarning(range.At,
                                      String::format("Register range {0}-{1} was "
                                                     "expressed in the wrong order.",
                                                     { range.First, range.Last }));
                }

                // Return the reversed register range.
                resolved.first = last;
                resolved.second = first;
            }
            else
            {
                // Return the register range.
                resolved.first = first;
                resolved.second = last;
            }
        }

        return isOK;
    }
public:
    MultiTransferInstructionStatement(const Location &start,
                                      InstructionMnemonic mnemonic,
                                      ConditionCode condition,
                                      MultiTransferMode mode,
                                      IExprUPtr &&rd,
                                      const RegisterListNode::SymbolRangeCollection &ranges,
                                      uint8_t flags) :
        InstructionStatement(start, OperationClass::CoreMultiTransfer,
                             mnemonic, condition),
        _rd(std::move(rd)),
        _regRanges(ranges),
        _mode(mode),
        _flags(flags)
    {
    }

protected:
    // Overrides
    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        bool isOK = false;
        auto &info = instruction.getCoreMultiTransferParameters();

        info.Mode = _mode;
        info.Writeback = (_flags & MultiTransferInstructionNode::Writeback) != 0;
        info.UserModeRegs = (_flags & MultiTransferInstructionNode::UserModeRegs) != 0;

        // Resolve the destination register.
        String error;
        if (tryEvaluateCoreRegister(context, _rd.get(), info.Rd, error))
        {
            if (_regRanges.empty())
            {
                if (isFinalPass)
                {
                    log.appendError(getStart(), "Empty register set.");
                }
            }
            else
            {
                isOK = true;

                // Resolve each item in the register set.
                for (const auto &regRange : _regRanges)
                {
                    CoreRegPair resolvedRange;

                    if (tryEvalRange(context, log, isFinalPass,
                                     regRange, resolvedRange))
                    {
                        for (uint8_t i = toScalar(resolvedRange.first),
                             j = toScalar(resolvedRange.second); i <= j; ++i)
                        {
                            info.Registers |= static_cast<uint16_t>(1u << i);
                        }
                    }
                    else
                    {
                        // The range was invalid, an error has already been logged.
                        isOK = false;
                    }
                }
            }
        }
        else if (isFinalPass)
        {
            std::string builder("Failed to evaluate destination register expression: ");
            appendAgString(builder, error);

            log.appendError(getStart(), builder);
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
// MultiTransferInstructionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Construction a syntax node representing a partially parsed core
//! multi-data transfer instruction statement.
//! @param[in] context The current state of the parser.
//! @param[in] mnemonic The token representing the instruction mnemonic.
MultiTransferInstructionNode::MultiTransferInstructionNode(ParseContext &context,
                                                           const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _state(State::AfterMnemonic),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::Ldm)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _mode(getTokenEnum(mnemonic, TokenProperty::MultiTransferMode, MultiTransferMode::FullDescending)),
    _flags(0)
{
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool MultiTransferInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool MultiTransferInstructionNode::isValid() const
{
    return _destReg && _regList;
}

// Inherited from ISyntaxNode.
ISyntaxNode *MultiTransferInstructionNode::applyToken(ParseContext &context,
                                                      const Token &token)
{
    ISyntaxNode *result = nullptr;
    TokenClass tokenType = token.getClass();

    switch (_state)
    {
    case State::AfterMnemonic:
        // Expect register expression.
        break;

    case State::AfterDestReg:
        // Expect comma or bang.
        if (tokenType == TokenClass::Comma)
        {
            // Start parsing the register list.
            RegisterListNode *regList = new RegisterListNode(context);
            context.pushSyntaxNode(this);
            result = regList;
            _state = State::BeforeRegisterList;
        }
        else if (tokenType == TokenClass::Bang)
        {
            _flags |= Writeback;
            result = this;
            _state = State::AfterWriteback;
        }
        break;

    case State::AfterWriteback:
        // Expect comma.
        if (tokenType == TokenClass::Comma)
        {
            // Start parsing the register list.
            RegisterListNode *regList = new RegisterListNode(context);
            context.pushSyntaxNode(this);
            result = regList;
            _state = State::BeforeRegisterList;
        }
        break;

    case State::BeforeRegisterList:
        // Expect RegisterListNode.
        break;

    case State::AfterRegList:
        // Expect hat or statement terminator.
        if (tokenType == TokenClass::Hat)
        {
            _flags |= UserModeRegs;
            _state = State::AfterHat;
            result = this;
        }
        else if (tokenType == TokenClass::StatementTerminator)
        {
            _state = State::Complete;
            result = this;
        }
        break;

    case State::AfterHat:
        // Expect statement terminator.
        if (tokenType == TokenClass::StatementTerminator)
        {
            _state = State::Complete;
            result = this;
        }
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *MultiTransferInstructionNode::applyNode(ParseContext &context,
                                                     ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr expr = nullptr;
    RegisterListNode *regs = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic:
        // Expect register expression.
        if (tryCast(childNode, expr))
        {
            _destReg.reset(expr);
            result = this;
            restoreLexicalState(context);
            _state = State::AfterDestReg;
        }
        break;

    case State::AfterDestReg:
        // Expect comma or bang.
        break;

    case State::AfterWriteback:
        // Expect comma.
        break;

    case State::BeforeRegisterList:
        // Expect RegisterListNode.
        if (tryCast(childNode, regs))
        {
            _regList.reset(regs);
            result = this;
            restoreLexicalState(context);
            _state = State::AfterRegList;
        }
        break;

    case State::AfterRegList:
        // Expect hat or statement terminator.
        break;

    case State::AfterHat:
        // Expect statement terminator.
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void MultiTransferInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *MultiTransferInstructionNode::compile(Messages &/*output*/) const
{
    Statement *statement = nullptr;

    if (_destReg && _regList)
    {
        IExprUPtr rd(constantOptimise(_destReg->compile(getCoreRegSymbols())));

        statement = new MultiTransferInstructionStatement(getStart(),
                                                          _mnemonic, _condition,
                                                          _mode, std::move(rd),
                                                          _regList->getRanges(),
                                                          _flags);
    }

    return statement;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


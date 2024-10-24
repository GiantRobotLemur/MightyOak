//! @file CoProcInstructionStatements.cpp
//! @brief The definition of syntax nodes which represent partially parsed
//! generic ARM co-processor instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "CoProcInstructionStatements.hpp"

#include "AssemblyState.hpp"
#include "ConstantSet.hpp"
#include "Expr.hpp"
#include "InstructionStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"

namespace Mo {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing a fully parsed CDP, MCR or MRC instruction.
class CdpInstruction : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _cpId;
    IExprUPtr _opCode1;
    IExprUPtr _opCode2;
    IExprUPtr _rd;
    IExprUPtr _rn;
    IExprUPtr _rm;

public:
    CdpInstruction(const Location &at, InstructionMnemonic op, ConditionCode condition,
                   IExprUPtr &&cpId, IExprUPtr &&opCode1, IExprUPtr &&opCode2,
                   IExprUPtr &&rd, IExprUPtr &&rn, IExprUPtr &&rm) :
        InstructionStatement(at,
                             (op == InstructionMnemonic::Cdp) ? OperationClass::CoProcDataProcessing :
                                                                OperationClass::CoProcRegisterTransfer,
                             op, condition),
        _cpId(std::move(cpId)),
        _opCode1(std::move(opCode1)),
        _opCode2(std::move(opCode2)),
        _rd(std::move(rd)),
        _rn(std::move(rn)),
        _rm(std::move(rm))
    {
    }

    virtual ~CdpInstruction() = default;

    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        bool isCDP = (getMnemonic() == InstructionMnemonic::Cdp);

        ExprToEvaluate exprs[] = {
            { _cpId, "co-processor identifier", 15 },
            { _opCode1, "first op-code", isCDP ? 15u : 7u },
            { _opCode2, "second op-code", 7 },
            { _rd, "destination register", 15 },
            { _rn, "first operand register", 15 },
            { _rm, "second operand register", 15 },
        };

        uint32_t results[std::size(exprs)];

        bool isOK = tryEvaluateExpressionArray(exprs, results, context,
                                               log, isFinalPass);

        if (isOK)
        {
            // Feed in the parameters needed, whether valid or not.
            if (isCDP)
            {
                auto &params = instruction.getCoProcDataProcessingParameters();
                params.CoProcessor = Ag::forceFromScalar<CoProcId>(results[0]);
                params.OpCode1 = static_cast<uint8_t>(results[1]);
                params.OpCode2 = static_cast<uint8_t>(results[2]);
                params.Rd = Ag::forceFromScalar<CoProcRegister>(results[3]);
                params.Rn = Ag::forceFromScalar<CoProcRegister>(results[4]);
                params.Rm = Ag::forceFromScalar<CoProcRegister>(results[5]);
            }
            else
            {
                auto &params = instruction.getCoProcRegisterTransferParameters();
                params.CoProcessor = Ag::forceFromScalar<CoProcId>(results[0]);
                params.OpCode1 = static_cast<uint8_t>(results[1]);
                params.OpCode2 = static_cast<uint8_t>(results[2]);
                params.Rd = Ag::forceFromScalar<CoreRegister>(results[3]);
                params.Rn = Ag::forceFromScalar<CoProcRegister>(results[4]);
                params.Rm = Ag::forceFromScalar<CoProcRegister>(results[5]);
            }
        }

        return isOK || isFinalPass;
    }
};

class CoProcDataTransferInstruction : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _cpId;
    IExprUPtr _rd;
    StatementAddressOperand _addr;
    bool _longMode;

public:
    CoProcDataTransferInstruction(const Location &at, InstructionMnemonic op,
                                  ConditionCode condition, bool longMode,
                                  IExprUPtr &&cpId, IExprUPtr &&rd,
                                  StatementAddressOperand &&addr) :
        InstructionStatement(at, OperationClass::CoProcDataTransfer,
                             op, condition),
        _cpId(std::move(cpId)),
        _rd(std::move(rd)),
        _addr(std::move(addr)),
        _longMode(longMode)
    {
    }

    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        ExprToEvaluate exprs[] = {
            { _cpId, "co-processor identifier", 15 },
            { _rd, "destination register", 15 },
        };

        uint32_t results[std::size(exprs)];
        bool isOK = false;

        if (_addr.configure(instruction, context, log, isFinalPass) &&
            tryEvaluateExpressionArray(exprs, results, context, log, isFinalPass))
        {
            isOK = true;

            auto &info = instruction.getCoProcDataTransferParameters();
            info.CoProcessor = Ag::forceFromScalar<CoProcId>(results[0]);
            info.Rd = Ag::forceFromScalar<CoProcRegister>(results[1]);
            info.IsLong = _longMode;
        }

        return isOK || isFinalPass;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// CoProcDataProcInstructionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node to hold the parser state for a generic
//! ARM co-processor data processing instruction.
//! @param[in] context The current state of the parsing process.
//! @param[in] mnemonic The mnemonic token at the beginning of the statement.
CoProcDataProcInstructionNode::CoProcDataProcInstructionNode(ParseContext &context,
                                                             const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _lexicalBaseState(context.pushLexicalContext(getExpressionLexer())),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::Cdp)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _state(State::BeforeProcID)
{
}

// Inherited from ISyntaxNode.
bool CoProcDataProcInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool CoProcDataProcInstructionNode::isValid() const
{
    return _cpIdExpr && _opCode1Expr &&_opCode2Expr &&
           _rdExpr && _rnExpr && _rmExpr;
}

// Inherited from ISyntaxNode.
ISyntaxNode *CoProcDataProcInstructionNode::applyToken(ParseContext &context,
                                                       const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
        case State::BeforeProcID: break;
        case State::AfterProcID:
            if (confirmToken(token, TokenClass::Comma, State::BeforeOpCode1,
                             _state, result))
            {
                // Start parsing expression tokens.
                _lexicalBaseState = context.pushLexicalContext(getExpressionLexer());
            }
            break;

        case State::BeforeOpCode1: break;
        case State::AfterOpCode1:
            if (confirmToken(token, TokenClass::Comma, State::BeforeRd,
                             _state, result))
            {
                // Start parsing expression tokens.
                _lexicalBaseState = context.pushLexicalContext(getExpressionLexer());
            }
            break;

        case State::BeforeRd: break;
        case State::AfterRd:
            if (confirmToken(token, TokenClass::Comma, State::BeforeRn,
                             _state, result))
            {
                // Start parsing expression tokens.
                _lexicalBaseState = context.pushLexicalContext(getExpressionLexer());
            }
            break;

        case State::BeforeRn: break;
        case State::AfterRn:
            if (confirmToken(token, TokenClass::Comma, State::BeforeRm,
                             _state, result))
            {
                // Start parsing expression tokens.
                _lexicalBaseState = context.pushLexicalContext(getExpressionLexer());
            }
            break;

        case State::BeforeRm: break;
        case State::AfterRm:
            if (confirmToken(token, TokenClass::Comma, State::BeforeOpCode2,
                             _state, result))
            {
                // Start parsing expression tokens.
                _lexicalBaseState = context.pushLexicalContext(getExpressionLexer());
            }
            break;

        case State::BeforeOpCode2: break;
        case State::Complete: break;
    }

    if (checkForPrematureEndOfStatement(context, token, result))
    {
        // Handle premature end of statement.
        _state = State::Complete;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *CoProcDataProcInstructionNode::applyNode(ParseContext &context,
                                                      ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::BeforeProcID:
        if (confirmNodeType(childNode, _cpIdExpr, State::AfterProcID,
                            _state, result))
        {
            context.restoreLexicalState(_lexicalBaseState);
        }
        break;

    case State::AfterProcID: break;
    case State::BeforeOpCode1:
        if (confirmNodeType(childNode, _opCode1Expr, State::AfterOpCode1,
                            _state, result))
        {
            context.restoreLexicalState(_lexicalBaseState);
        }
        break;

    case State::AfterOpCode1: break;
    case State::BeforeRd:
        if (confirmNodeType(childNode, _rdExpr, State::AfterRd,
                        _state, result))
        {
            context.restoreLexicalState(_lexicalBaseState);
        }
        break;

    case State::AfterRd: break;
    case State::BeforeRn:
        if (confirmNodeType(childNode, _rnExpr, State::AfterRn,
                            _state, result))
        {
            context.restoreLexicalState(_lexicalBaseState);
        }
        break;

    case State::AfterRn: break;
    case State::BeforeRm:
        if (confirmNodeType(childNode, _rmExpr, State::AfterRm,
                            _state, result))
        {
            context.restoreLexicalState(_lexicalBaseState);
        }
        break;

    case State::AfterRm: break;
    case State::BeforeOpCode2:
        if (confirmNodeType(childNode, _opCode2Expr, State::Complete,
                            _state, result))
        {
            context.restoreLexicalState(_lexicalBaseState);
        }
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void CoProcDataProcInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *CoProcDataProcInstructionNode::compile(Messages &output) const
{
    ExprToCompile exprNodes[] = {
        { _cpIdExpr, "co-processor identifier", getCoProcIDSymbols() },
        { _opCode1Expr, "first op-code", ConstantSet::Empty },
        { _opCode2Expr, "second op-code", ConstantSet::Empty },
        { _rdExpr, "destination register",
           (_mnemonic == InstructionMnemonic::Cdp) ? getCoProcRegSymbols() :
                                                     getCoreRegSymbols() },
        { _rnExpr, "first operand register", getCoProcRegSymbols() },
        { _rmExpr, "second operand register", getCoProcRegSymbols() },
    };

    IExprUPtr exprs[std::size(exprNodes)];

    // Compile all the expressions.
    bool isOK = compileExpressionArray(getStart(), exprNodes, exprs, output);

    Statement *statement = nullptr;

    if (isOK)
    {
        // Create a new statement which will take ownership of the
        // various compiled expressions.
        statement = new CdpInstruction(getStart(), _mnemonic, _condition,
                                       std::move(exprs[0]), std::move(exprs[1]),
                                       std::move(exprs[2]), std::move(exprs[3]),
                                       std::move(exprs[4]), std::move(exprs[5]));
    }

    return statement;
}

////////////////////////////////////////////////////////////////////////////////
// CoProcDataTransferInstructionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node to hold the parser state for a generic
//! ARM co-processor data transfer instruction.
//! @param[in] context The current state of the parsing process.
//! @param[in] mnemonic The mnemonic token at the beginning of the statement.
CoProcDataTransferInstructionNode::CoProcDataTransferInstructionNode(ParseContext &context,
                                                                     const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _state(State::BeforeProcID),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::Ldc)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _longMode(getTokenFlag(mnemonic, TokenProperty::LongMode, false))
{
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool CoProcDataTransferInstructionNode::isComplete() const
{
    return _state == State::Complete;
}

// Inherited from ISyntaxNode.
bool CoProcDataTransferInstructionNode::isValid() const
{
    return _cpIdExpr && _rdExpr && _addrNode;
}

// Inherited from ISyntaxNode.
ISyntaxNode *CoProcDataTransferInstructionNode::applyToken(ParseContext &context,
                                                           const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::BeforeProcID: break;
    case State::AfterProcID:
        confirmToken(token, TokenClass::Comma, State::BeforeRd, _state, result);
        break;

    case State::BeforeRd: break;
    case State::AfterRd:
        if (confirmToken(token, TokenClass::Comma, State::BeforeAddress,
                         _state, result))
        {
            // Start parsing the address operand as a child of the current node.
            restoreLexicalState(context);
            context.pushSyntaxNode(this);

            result = new AddressOperandNode(context);
        }
        break;

    case State::BeforeAddress: break;
    case State::Complete: break;
    }

    if (checkForPrematureEndOfStatement(context, token, result))
    {
        // Handle premature end of statement.
        _state = State::Complete;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *CoProcDataTransferInstructionNode::applyNode(ParseContext &/*context*/,
                                                          ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::BeforeProcID:
        // Expect an expression giving the co-processor ID.
        confirmNodeType(childNode, _cpIdExpr, State::AfterProcID,
                        _state, result);
        break;

    case State::AfterProcID: break;
    case State::BeforeRd:
        confirmNodeType(childNode, _rdExpr, State::AfterRd, _state, result);
        break;

    case State::AfterRd: break;

    case State::BeforeAddress:
        // Expect to receive a fully populated address operand node.
        confirmNodeType(childNode, _addrNode, State::Complete,
                        _state, result);
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void CoProcDataTransferInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *CoProcDataTransferInstructionNode::compile(Messages &output) const
{
    StatementAddressOperand addrOperand;

    ExprToCompile exprNodes[] = {
        { _cpIdExpr, "co-processor identifier", getCoProcIDSymbols() },
        { _rdExpr, "destination register", getCoProcRegSymbols() },
    };

    IExprUPtr exprs[std::size(exprNodes)];

    Statement *statement = nullptr;

    // Compile all the expressions and operands.
    if (_addrNode->compile(addrOperand, output) &&
        compileExpressionArray(getStart(), exprNodes, exprs, output))
    {
        // Create a new statement which will take ownership of the
        // various compiled expressions.
        statement = new CoProcDataTransferInstruction(
            getStart(), _mnemonic, _condition, _longMode,
            std::move(exprs[0]), std::move(exprs[1]),
            std::move(addrOperand));
    }

    return statement;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


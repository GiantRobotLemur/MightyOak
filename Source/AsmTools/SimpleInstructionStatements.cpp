//! @file SimpleInstructionStatements.cpp
//! @brief The definition of various syntax nodes which represent simple ARM
//! instruction statements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AssemblyState.hpp"
#include "ConstantSet.hpp"
#include "Expr.hpp"
#include "InstructionStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"
#include "SimpleInstructionStatements.hpp"
#include "Token.hpp"

namespace Mo {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing a fully parsed SWI instruction.
class SwiStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _comment;

public:
    SwiStatement(const Location &at, ConditionCode condition,
                 IExprPtr commentExpr) :
        InstructionStatement(at, OperationClass::SoftwareIrq,
                             InstructionMnemonic::Swi, condition),
        _comment(commentExpr)
    {
    }

    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        bool canEvaluate = false;
        Ag::String error;
        uint32_t comment = 0;

        if (tryEvaluateOrdinal(context, _comment.get(), comment, error))
        {
            if (comment > 0x00FFFFFF)
            {
                log.appendWarning(getStart(),
                                    "The numeric SWI value is too big to be encoded.");
            }

            canEvaluate = true;
        }
        else if (isFinalPass)
        {
            std::string builder("Failed to evaluate the numeric SWI field: ");
            Ag::appendAgString(builder, error);

            log.appendError(getStart(), builder);
        }

        // Feed in the parameter needed, whether valid or not.
        instruction.getSoftwareIrqParameters().Comment = comment;

        return canEvaluate || isFinalPass;
    }
};

//! @brief An object representing a fully parsed Bkpt instruction.
class BkptStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _comment;

public:
    BkptStatement(const Location &at, ConditionCode condition,
                  IExprPtr commentExpr) :
        InstructionStatement(at, OperationClass::Breakpoint,
                             InstructionMnemonic::Bkpt, condition),
        _comment(commentExpr)
    {
    }

    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        bool canEvaluate = false;
        Ag::String error;
        uint32_t comment = 0;

        if (tryEvaluateOrdinal(context, _comment.get(), comment, error))
        {
            if (comment > 0x0000FFFF)
            {
                log.appendWarning(getStart(),
                                  "The numeric breakpoint value is too big to be encoded.");
            }

            canEvaluate = true;
        }
        else if (isFinalPass)
        {
            std::string builder("Failed to evaluate the numeric breakpoint field: ");
            Ag::appendAgString(builder, error);

            log.appendError(getStart(), builder);
        }

        // Feed in the parameter needed, whether valid or not.
        instruction.getBreakpointParameters().Comment = static_cast<uint16_t>(comment);

        return canEvaluate || isFinalPass;
    }
};

//! @brief An object representing a fully parsed branch instruction.
class BranchStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _target;
public:
    BranchStatement(const Location &at,
                    InstructionMnemonic mnemonic,
                    ConditionCode condition,
                    IExprPtr targetExpr) :
        InstructionStatement(at, OperationClass::Branch,
                             mnemonic, condition),
        _target(targetExpr)
    {
    }

    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        bool canEvaluate = false;
        Ag::String error;
        uint32_t targetAddr = 0;

        if (tryEvaluateOrdinal(context, _target.get(), targetAddr, error))
        {
            const int32_t offsetMin = -32 * 1024 * 1024;
            const int32_t offsetMax = 32 * 1024 * 1024;

            canEvaluate = true;
            uint32_t pcAddr = context->getAssemblyAddress() + 8;
            int32_t offsetValue = static_cast<int32_t>(targetAddr - pcAddr);

            if ((offsetValue < offsetMin) || (offsetValue > offsetMax))
            {
                log.appendWarning(getStart(),
                                  "The branch target is beyond the range "
                                  "which can be encoded.");
            }

            // Feed in the parameter needed, whether valid or not.
            instruction.getBranchParameters().Address = targetAddr;
        }
        else if (isFinalPass)
        {
            std::string builder("Failed to evaluate branch target field: ");
            Ag::appendAgString(builder, error);

            log.appendError(getStart(), builder);
        }

        return canEvaluate;
    }
};

//! @brief An object representing a fully parsed branch instruction.
class MultiplyStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _regs[4];
    uint8_t _regCount;
    bool _updatesPsr;

    static bool isLongMul(InstructionMnemonic mnemonic)
    {
        return (mnemonic != InstructionMnemonic::Mul) &&
               (mnemonic != InstructionMnemonic::Mla);
    }
public:
    MultiplyStatement(const Location &at,
                      InstructionMnemonic mnemonic,
                      ConditionCode condition, bool updatesPsr,
                      IExprUPtr *regExpr, uint8_t regCount) :
        InstructionStatement(at,
                             isLongMul(mnemonic) ? OperationClass::LongMultiply :
                                                   OperationClass::CoreMultiply,
                             mnemonic, condition),
        _regCount(regCount),
        _updatesPsr(updatesPsr)
    {
        // Take ownership of the register expressions.
        for (uint8_t i = 0, c = std::min(static_cast<uint8_t>(4), regCount); i < c; ++i)
        {
            _regs[i] = std::move(regExpr[i]);
        }
    }

    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        Ag::String error;
        bool isOK = true;
        bool isLong = isLongMul(getMnemonic());

        CoreRegister regIds[4];
        std::fill_n(regIds, 4, CoreRegister::R0);

        const char *mulFields[] = { "Rd", "Rm", "Rs", "Rn" };
        const char *lmulFields[] = { "RdLo", "RdHi", "Rm", "Rs" };

        const char **fieldNames = isLong ? lmulFields : mulFields;

        for (uint8_t i = 0; i < _regCount; ++i)
        {
            if (tryEvaluateCoreRegister(context, _regs[i].get(),
                                        regIds[i], error) == false)
            {
                isOK = false;

                if (isFinalPass)
                {
                    std::string message;
                    message.append(fieldNames[i]);
                    message.push_back(':');
                    message.push_back(' ');
                    appendAgString(message, error);
                    log.appendError(getStart(), message);
                }
            }
        }

        if (isOK || isFinalPass)
        {
            // Configure the instruction parameters whether we succeeded
            // or not.
            isOK = true;

            if (isLong)
            {
                auto &params = instruction.getLongMultiplyParameters();
                params.RdLo = regIds[0];
                params.RdHi = regIds[1];
                params.Rm = regIds[2];
                params.Rs = regIds[3];
                params.AffectsFlags = _updatesPsr;
            }
            else
            {
                auto &params = instruction.getCoreMultiplyParameters();
                params.Rd = regIds[0];
                params.Rm = regIds[1];
                params.Rs = regIds[2];

                if (getMnemonic() == InstructionMnemonic::Mla)
                {
                    params.Rn = regIds[3];
                }

                params.AffectsFlags = _updatesPsr;
            }
        }

        return isOK;
    }

protected:
    // Inherited from InstructionStatement.
    virtual bool validate(const InstructionInfo &instruction,
                          const AssemblyState &state,
                          Messages &log) const override
    {
        bool isValid = true;

        if (instruction.getOperationClass() == OperationClass::LongMultiply)
        {
            // Long multiply requires ARMv4+.
            isValid = validateInstructionSet(state, log, InstructionSet::ArmV4);
        }

        return isValid;
    }
};

//! @brief An object representing a fully parsed MRS instruction.
class MrsStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _rd;
    bool _isCPSR;
public:
    MrsStatement(const Location &at, ConditionCode condition,
                 IExprPtr rdExpr, CoreRegister source) :
        InstructionStatement(at, OperationClass::MoveFromPSR,
                             InstructionMnemonic::Msr, condition),
        _rd(rdExpr),
        _isCPSR(source == CoreRegister::CPSR)
    {
    }

    virtual ~MrsStatement() = default;

    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        auto &info = instruction.getMoveFromPsrParameters();
        Ag::String error;
        bool isOK = false;

        if (tryEvaluateCoreRegister(context, _rd.get(), info.Rd, error))
        {
            info.IsCPSR = _isCPSR;
            isOK = true;
        }
        else if (isFinalPass)
        {
            std::string message;
            message.append("Failed to evaluate destination register expression: ");
            Ag::appendAgString(message, error);

            log.appendError(getStart(), message);
        }

        return isOK || isFinalPass;
    }

protected:
    // Inherited from InstructionStatement.
    virtual bool validate(const InstructionInfo &/*instruction*/,
                          const AssemblyState &state,
                          Messages &log) const override
    {
        return validateInstructionSet(state, log, InstructionSet::ArmV3);
    }
};


//! @brief An object representing a fully parsed MSR instruction.
class MsrStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _sourceExpr;
    ConditionCode _condition;
    uint8_t _psrComponents;
    bool _isCPSR;
    bool _sourceIsReg;
public:
    MsrStatement(const Location &at, ConditionCode condition,
                 IExprPtr sourceExpr, bool sourceIsReg,
                 CoreRegister rd, uint8_t psrComponents) :
        InstructionStatement(at, OperationClass::MoveToPSR,
                             InstructionMnemonic::Mrs, condition),
        _sourceExpr(sourceExpr),
        _condition(condition),
        _psrComponents(psrComponents),
        _isCPSR(rd == CoreRegister::CPSR),
        _sourceIsReg(sourceIsReg)
    {
    }

    virtual ~MsrStatement() = default;

    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        auto &info = instruction.getMoveToPsrParameters();
        info.IsSourceReg = _sourceIsReg;
        info.IsCPSR = _isCPSR;
        info.PsrComponents = _psrComponents;

        Ag::String error;
        bool isOK = true;

        if (_sourceIsReg)
        {
            // The source is a core register.
            if (tryEvaluateCoreRegister(context, _sourceExpr.get(), info.SourceReg, error))
            {
                isOK = true;
            }
            else if (isFinalPass)
            {
                std::string message;
                message.append("Failed to evaluate source register expression: ");
                Ag::appendAgString(message, error);

                log.appendError(getStart(), message);
            }
        }
        else
        {
            // The source is an immediate constant.
            if (tryEvaluateOrdinal(context, _sourceExpr.get(), info.SourceImmediate, error))
            {
                isOK = true;
            }
            else if (isFinalPass)
            {
                std::string message;
                message.append("Failed to evaluate source immediate constant expression: ");
                Ag::appendAgString(message, error);

                log.appendError(getStart(), message);
            }
        }

        return isOK || isFinalPass;
    }

protected:
    // Inherited from InstructionStatement.
    virtual bool validate(const InstructionInfo &/*instruction*/,
                          const AssemblyState &state,
                          Messages &log) const override
    {
        return validateInstructionSet(state, log, InstructionSet::ArmV3);
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
// SwiInstructionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing a partially parsed SWI instruction
//! statement just after the mnemonic has been recognised.
//! @param[in] context The state of the parsing process.
//! @param[in] mnemonic The recognised SWI mnemonic token.
SwiInstructionNode::SwiInstructionNode(ParseContext &context,
                                       const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _state(State::AfterMnemonic)
{
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool SwiInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool SwiInstructionNode::isValid() const { return static_cast<bool>(_comment); }

// Inherited from ISyntaxNode.
ISyntaxNode *SwiInstructionNode::applyToken(ParseContext &/*context*/,
                                            const Token &token)
{
    ISyntaxNode *result = nullptr;

    if ((_state == State::AfterCommentExpr) &&
        (token.getClass() == TokenClass::StatementTerminator))
    {
        _state = State::Complete;
        result = this;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *SwiInstructionNode::applyNode(ParseContext &context,
                                           ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNode *expr = nullptr;

    if ((_state == State::AfterMnemonic) &&
        Ag::tryCast(childNode, expr))
    {
        // Stop looking for expression tokens.
        restoreLexicalState(context);

        // Take ownership of the comment expression and move to a new state.
        _comment.reset(expr);
        _state = State::AfterCommentExpr;
        result = this;
    }

    return result;
}

// Inherited from ISyntaxNode.
void SwiInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *SwiInstructionNode::compile(Messages &output) const
{
    Statement *result = nullptr;

    if (isComplete() && isValid())
    {
        IExpr *commentExpr = _comment->compile(ConstantSet::Empty);

        if (commentExpr == nullptr)
        {
            output.appendError(_comment->getPosition(),
                               "Failed to compile SWI comment expression.");
        }
        else
        {
            result = new SwiStatement(getStart(), _condition,
                                      constantOptimise(commentExpr));
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// BranchInstructionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing a partially parsed branch instruction
//! statement just after the mnemonic has been recognised.
//! @param[in] context The state of the parsing process.
//! @param[in] mnemonic The recognised branch mnemonic token.
BranchInstructionNode::BranchInstructionNode(ParseContext &context,
                                       const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::B)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _state(State::AfterMnemonic)
{
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool BranchInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool BranchInstructionNode::isValid() const { return static_cast<bool>(_targetExpr); }

// Inherited from ISyntaxNode.
ISyntaxNode *BranchInstructionNode::applyToken(ParseContext &/*context*/,
                                            const Token &token)
{
    ISyntaxNode *result = nullptr;

    if ((_state == State::AfterTargetExpr) &&
        (token.getClass() == TokenClass::StatementTerminator))
    {
        _state = State::Complete;
        result = this;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *BranchInstructionNode::applyNode(ParseContext &context,
                                              ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNode *expr = nullptr;

    if ((_state == State::AfterMnemonic) &&
        Ag::tryCast(childNode, expr))
    {
        // Stop looking for expression tokens.
        restoreLexicalState(context);

        // Take ownership of the comment expression and move to a new state.
        _targetExpr.reset(expr);
        _state = State::AfterTargetExpr;
        result = this;
    }

    return result;
}

// Inherited from ISyntaxNode.
void BranchInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *BranchInstructionNode::compile(Messages &output) const
{
    Statement *result = nullptr;

    if (isComplete() && isValid())
    {
        IExpr *targetExpr = _targetExpr->compile(ConstantSet::Empty);

        if (targetExpr == nullptr)
        {
            output.appendError(_targetExpr->getPosition(),
                               "Failed to compile branch target expression.");
        }
        else
        {
            result = new BranchStatement(getStart(), _mnemonic, _condition,
                                         constantOptimise(targetExpr));
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// MultiplyInstructionNode Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing a partially parsed multiply instruction
//! statement just after the mnemonic has been recognised.
//! @param[in] context The state of the parsing process.
//! @param[in] mnemonic The recognised branch mnemonic token.
MultiplyInstructionNode::MultiplyInstructionNode(ParseContext &context,
                                                 const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _expectedRegCount(0),
    _regCount(0),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::Mul)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _state(State::AfterSeparator),
    _updatesPsr(getTokenFlag(mnemonic, TokenProperty::UpdatePsr, false))
{
    _expectedRegCount = (_mnemonic == InstructionMnemonic::Mul) ? 3 : 4;

    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool MultiplyInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool MultiplyInstructionNode::isValid() const
{
    return (_expectedRegCount == _regCount);
}

// Inherited from ISyntaxNode.
ISyntaxNode *MultiplyInstructionNode::applyToken(ParseContext &context,
                                               const Token &token)
{
    ISyntaxNode *result = nullptr;

    if (_state == State::AfterReg)
    {
        if (token.getClass() == TokenClass::StatementTerminator)
        {
            if (_expectedRegCount > _regCount)
            {
                // The statement ended too early.
                context.getMessages().appendError(token.getLocation(),
                                                  "Missing operands for multiply instruction");
            }

            result = this;
            _state = State::Complete;
            restoreLexicalState(context);
        }
        else if ((token.getClass() == TokenClass::Comma) &&
                 (_regCount < _expectedRegCount))
        {
            // Start recognising tokens for an expression.
            context.pushLexicalContext(getExpressionLexer());
            _state = State::AfterSeparator;
            result = this;
        }
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *MultiplyInstructionNode::applyNode(ParseContext &context,
                                              ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNode *expr = nullptr;

    if ((_state == State::AfterSeparator) &&
        Ag::tryCast(childNode, expr))
    {
        // Stop looking for expression tokens.
        restoreLexicalState(context);

        if (_regCount < _expectedRegCount)
        {
            // Take ownership of the register expression and change state.
            _regExprs[_regCount].reset(expr);
            ++_regCount;
            _state = State::AfterReg;
            result = this;
        }
    }

    return result;
}

// Inherited from ISyntaxNode.
void MultiplyInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *MultiplyInstructionNode::compile(Messages &output) const
{
    Statement *result = nullptr;

    if (isComplete() && isValid())
    {
        IExprUPtr regExprs[4];
        bool isOK = true;

        for (uint8_t i = 0; i < _regCount; ++i)
        {
            IExprPtr expr = _regExprs[i]->compile(getCoreRegSymbols());

            if (expr == nullptr)
            {
                output.appendError(_regExprs[i]->getPosition(),
                                   "Failed to compile register expression.");
                isOK = false;
                break;
            }
            else
            {
                regExprs[i].reset(constantOptimise(expr));
            }
        }

        if (isOK)
        {
            // Create a statement to encode the instruction.
            result = new MultiplyStatement(getStart(), _mnemonic, _condition,
                                           _updatesPsr, regExprs, _regCount);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// MoveFromStatusRegNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
MoveFromStatusRegNode::MoveFromStatusRegNode(ParseContext &context, const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::Mrs)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _state(State::AfterMnemonic),
    _source(CoreRegister::R0)
{
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool MoveFromStatusRegNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool MoveFromStatusRegNode::isValid() const
{
    return ((_source == CoreRegister::CPSR) ||
            (_source == CoreRegister::SPSR)) &&
           _rdExpr;
}

// Inherited from ISyntaxNode.
ISyntaxNode *MoveFromStatusRegNode::applyToken(ParseContext &context,
                                             const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic: break;
    case State::AfterRd:
        if (token.getClass() == TokenClass::Comma)
        {
            // Expect the next item to be a status register name.
            context.pushLexicalContext(getPsrNameLexer());
            _state = State::BeforePsrReg;
            result = this;
        }
        break;

    case State::BeforePsrReg:
        if (token.getClass() == TokenClass::RegisterStatus)
        {
            restoreLexicalState(context);
            _source = getTokenEnum(token, TokenProperty::RegisterIndex, CoreRegister::R0);
            _state = State::Complete;
            result = this;
        }
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *MoveFromStatusRegNode::applyNode(ParseContext &context,
                                            ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr exprNode;

    switch (_state)
    {
    case State::AfterMnemonic:
        if (Ag::tryCast(childNode, exprNode))
        {
            restoreLexicalState(context);
            _rdExpr.reset(exprNode);
            result = this;
            _state = State::AfterRd;
        }
        break;

    case State::AfterRd: break;
    case State::BeforePsrReg: break;
    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void MoveFromStatusRegNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;
    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *MoveFromStatusRegNode::compile(Messages &output) const
{
    Statement *result = nullptr;

    if (isComplete() && isValid())
    {
        IExprPtr rdExpr = _rdExpr->compile(getCoreRegSymbols());

        if (rdExpr)
        {
            result = new MrsStatement(getStart(), _condition, rdExpr, _source);
        }
        else
        {
            // Failed to compile.
            output.appendError(_rdExpr->getPosition(),
                               "Failed to compile destination register expression.");
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// MoveToStatusRegNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
MoveToStatusRegNode::MoveToStatusRegNode(ParseContext &context, const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::Msr)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _state(State::AfterMnemonic),
    _rd(CoreRegister::R0),
    _psrComponents(0)
{
    context.pushLexicalContext(getPsrComponentLexer());
}

// Inherited from ISyntaxNode.
bool MoveToStatusRegNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool MoveToStatusRegNode::isValid() const
{
    return (_rmExpr || _immedExpr) && (_psrComponents != 0);
}

// Inherited from ISyntaxNode.
ISyntaxNode *MoveToStatusRegNode::applyToken(ParseContext &context,
                                               const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic:
        if (token.getClass() == TokenClass::RegisterStatus)
        {
            _rd = getTokenEnum(token, TokenProperty::RegisterIndex, CoreRegister::R0);

            if (token.tryGetScalarProperty(TokenProperty::PsrComponent,
                                           _psrComponents) == false)
            {
                _psrComponents = Ag::toScalar(PsrComponent::All);
            }

            _state = State::AfterPsrReg;
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::AfterPsrReg:
        if (token.getClass() == TokenClass::Comma)
        {
            _state = State::BeforeSourceExpr;
            context.pushLexicalContext(getExpressionLexer());
            result = this;
        }
        break;

    case State::BeforeSourceExpr:
        if (token.getClass() == TokenClass::Hash)
        {
            // It's an immediate constant
            _state = State::BeforeSourceImmediateExpr;
            result = this;
        }
        break;

    case State::BeforeSourceImmediateExpr: break;
    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *MoveToStatusRegNode::applyNode(ParseContext &context,
                                              ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr exprNode;

    switch (_state)
    {
    case State::AfterMnemonic: break;
    case State::AfterPsrReg: break;
    case State::BeforeSourceExpr:
    case State::BeforeSourceImmediateExpr:
        if (Ag::tryCast(childNode, exprNode))
        {
            restoreLexicalState(context);

            if (_state == State::BeforeSourceExpr)
            {
                // It's a register expression.
                _rmExpr.reset(exprNode);
            }
            else
            {
                // It's an immediate constant expression.
                _immedExpr.reset(exprNode);
            }

            _state = State::Complete;
            result = this;
        }
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void MoveToStatusRegNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *MoveToStatusRegNode::compile(Messages &output) const
{
    Statement *result = nullptr;

    if (isComplete() && isValid())
    {
        IExprPtr sourceExpr = nullptr;

        if (_rmExpr)
        {
            sourceExpr = _rmExpr->compile(getCoreRegSymbols());

            if (sourceExpr == nullptr)
            {
                output.appendError(_rmExpr->getPosition(),
                                   "Failed to compile source register expression.");
            }
            else
            {
                result = new MsrStatement(getStart(), _condition, sourceExpr,
                                          true, _rd, _psrComponents);
            }
        }
        else
        {
            sourceExpr = _immedExpr->compile(ConstantSet::Empty);

            if (sourceExpr == nullptr)
            {
                output.appendError(_immedExpr->getPosition(),
                                   "Failed to compile source immediate constant.");
            }
            else
            {
                result = new MsrStatement(getStart(), _condition, sourceExpr,
                                          false, _rd, _psrComponents);
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// BkptInstructionNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node representing a partially parsed SWI instruction
//! statement just after the mnemonic has been recognised.
//! @param[in] context The state of the parsing process.
//! @param[in] mnemonic The recognised SWI mnemonic token.
BkptInstructionNode::BkptInstructionNode(ParseContext &context,
                                       const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _state(State::AfterMnemonic)
{
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool BkptInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool BkptInstructionNode::isValid() const { return static_cast<bool>(_comment); }

// Inherited from ISyntaxNode.
ISyntaxNode *BkptInstructionNode::applyToken(ParseContext &context,
                                            const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic:
        if (token.getClass() == TokenClass::StatementTerminator)
        {
            // If we get here, there was no comment field.
            context.getMessages().appendError(token.getLocation(),
                                              "Missing BKPT instruction comment expression.");
            _state = State::Complete;

            // Stop looking for expression tokens.
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::AfterCommentExpr:
        if (token.getClass() == TokenClass::StatementTerminator)
        {
            _state = State::Complete;
            result = this;
        }
        break;

    default: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *BkptInstructionNode::applyNode(ParseContext &context,
                                           ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNode *expr = nullptr;

    if ((_state == State::AfterMnemonic) &&
        Ag::tryCast(childNode, expr))
    {
        // Stop looking for expression tokens.
        restoreLexicalState(context);

        // Take ownership of the comment expression and move to a new state.
        _comment.reset(expr);
        _state = State::AfterCommentExpr;
        result = this;
    }

    return result;
}

// Inherited from ISyntaxNode.
void BkptInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *BkptInstructionNode::compile(Messages &output) const
{
    Statement *result = nullptr;

    if (isComplete() && isValid())
    {
        IExpr *commentExpr = _comment->compile(ConstantSet::Empty);

        if (commentExpr == nullptr)
        {
            output.appendError(_comment->getPosition(),
                               "Failed to compile breakpoint comment expression.");
        }
        else
        {
            if (_condition != ConditionCode::Al)
            {
                output.appendError(getStart(),
                                   "The BKPT instruction cannot be conditionally executed.");
            }

            result = new BkptStatement(getStart(), _condition,
                                       constantOptimise(commentExpr));
        }
    }

    return result;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


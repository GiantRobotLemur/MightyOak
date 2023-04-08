//! @file ShifterOperandNode.cpp
//! @brief The definition of an object which represents a partially parsed
//! barrel shifter operand.
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
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"
#include "ShifterOperandNode.hpp"
#include "Statement.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// ShifterOperandNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to start recognising a barrel shifter operand.
//! @param[in] context The current state of the parsing process.
//! @param[in] allowSign True if the operand can be proceeded by a sign (+ or
//! ') as it might be in a memory offset operand.
//! @param[in] expectedEnd The class of token expected after the operand.
ShifterOperandNode::ShifterOperandNode(ParseContext &context, bool allowSign,
                                       TokenClass expectedEnd /*= TokenClass::StatementTerminator*/) :
    BaseSyntaxNode(context),
    _expectedEnd(expectedEnd),
    _allowSign(allowSign),
    _isNegative(false),
    _state(State::Start),
    _mode(ShifterMode::Register),
    _shiftType(ShiftType::None)
{
    context.pushLexicalContext(getExpressionLexer());
}

//! @brief Gets whether a minus signed was encountered before the shifter operand.
//! @retval true A negative sign prefix was encountered.
//! @retval false A positive sign or no sign was encountered.
bool ShifterOperandNode::isNegative() const
{
    return _isNegative;
}

//! @brief Attempts to compile the node into an object which is part of a
//! fully parsed instruction statement.
//! @param[out] operand Is updated with the resolved parameters of the
//! shifter operand.
bool ShifterOperandNode::compile(StatementShifterOperand &operand,
                                 Messages &log) const
{
    bool isOK = false;

    if (((_mode == ShifterMode::ImmediateConstant) ||
         (_mode == ShifterMode::Register) ||
         (_mode == ShifterMode::RotateWithExtend)) &&
        _baseExpr)
    {
        const ConstantSet &fixedSymbols =
            (_mode == ShifterMode::ImmediateConstant) ? ConstantSet::Empty :
            getCoreRegSymbols();

        IExprUPtr baseExpr(constantOptimise(_baseExpr->compile(fixedSymbols)));

        if (baseExpr)
        {
            isOK = true;

            switch (_mode)
            {
            case ShifterMode::ImmediateConstant:
                operand.initialiseConstant(getStart(), std::move(baseExpr), _allowSign);
                break;

            case ShifterMode::Register:
                operand.initialiseRegister(getStart(), std::move(baseExpr),
                                           _isNegative);
                break;

            case ShifterMode::RotateWithExtend:
                operand.initialiseRrxShift(getStart(), std::move(baseExpr),
                                           _isNegative);
                break;

            default:
                isOK = false;
                break;
            }
        }
        else
        {
            log.appendError(getStart(), "Failed to compile shifter operand base expression.");
        }
    }
    else if (((_mode == ShifterMode::ShiftByConstant) ||
              (_mode == ShifterMode::ShiftByRegister)) &&
             _baseExpr && _shiftExpr)
    {
        IExprUPtr regExpr(constantOptimise(_baseExpr->compile(getCoreRegSymbols())));

        const ConstantSet &fixedSymbols =
            (_mode == ShifterMode::ImmediateConstant) ? ConstantSet::Empty :
            getCoreRegSymbols();

        IExprUPtr shiftExpr(constantOptimise(_shiftExpr->compile(fixedSymbols)));

        if (regExpr && shiftExpr)
        {
            isOK = true;

            switch (_mode)
            {
            case ShifterMode::ShiftByConstant:
                operand.initialiseShiftByConstant(getStart(), std::move(regExpr),
                                                  _shiftType,
                                                  std::move(shiftExpr),
                                                  _isNegative);
                break;

            case ShifterMode::ShiftByRegister:
                operand.initialiseShiftByRegister(getStart(), std::move(regExpr),
                                                  _shiftType,
                                                  std::move(shiftExpr),
                                                  _isNegative);
                break;

            default:
                isOK = false;
                break;
            }
        }
        else
        {
            log.appendError(getStart(), "Failed to compile shift expression.");
        }
    }
    else
    {
        log.appendError(getStart(), "Invalid shifter operand.");
    }

    return isOK;
}

// Inherited from ISyntaxNode.
bool ShifterOperandNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool ShifterOperandNode::isValid() const
{
    bool isValid = false;

    switch (_mode)
    {
    case ShifterMode::ImmediateConstant:
    case ShifterMode::Register:
        isValid = (_shiftType == ShiftType::None) && _baseExpr;
        break;

    case ShifterMode::ShiftByRegister:
    case ShifterMode::ShiftByConstant:
        isValid = (_shiftType != ShiftType::None) && _baseExpr && _shiftExpr;
        break;

    case ShifterMode::RotateWithExtend:
        isValid = (_shiftType == ShiftType::Rrx) && _baseExpr;
        break;
    }

    return isValid;
}
// Inherited from ISyntaxNode.
ISyntaxNode *ShifterOperandNode::applyToken(ParseContext &context,
                                            const Token &token)
{
    ISyntaxNode *node = nullptr;

    switch (_state)
    {
    case State::Start:
        // Expecting a '#' before an immediate constant or a register expression.
        setStart(token.getLocation());

        if (token.getClass() == TokenClass::Hash)
        {
            _mode = ShifterMode::ImmediateConstant;
            _state = State::AfterInitialHash;
            node = this;
        }
        else if (_allowSign)
        {
            if (token.getClass() == TokenClass::Plus)
            {
                _isNegative = false;
                _state = State::AfterInitialSign;
                node = this;
            }
            else if (token.getClass() == TokenClass::Minus)
            {
                _isNegative = true;
                _state = State::AfterInitialSign;
                node = this;
            }
        }
        break;

    case State::AfterInitialSign:
        // Expecting a register expression.
        break;

    case State::AfterInitialHash:
        // Expect an ExpressionNode giving the value.
        break;

    case State::AfterInitialReg:
        if (token.getClass() == TokenClass::Comma)
        {
            _state = State::BeforeShift;
            _mode = ShifterMode::Register;
            node = this;
        }
        else if (token.getClass() == _expectedEnd)
        {
            _state = State::Complete;
            _mode = ShifterMode::Register;
            node = this;
        }
        break;

    case State::BeforeShift:
        _shiftType = ShiftType::None;

        switch (token.getClass())
        {
        case TokenClass::LogicShiftLeft: _shiftType = ShiftType::Lsl; break;
        case TokenClass::LogicShiftRight: _shiftType = ShiftType::Lsr; break;
        case TokenClass::ArithmeticShiftRight: _shiftType = ShiftType::Asr; break;
        case TokenClass::RotateRightShift: _shiftType = ShiftType::Ror; break;
        case TokenClass::RotateRightWithExtendShift: _shiftType = ShiftType::Rrx; break;
        default: break;
        }

        if (_shiftType == ShiftType::Rrx)
        {
            // Skip the expression after the shift mnemonic.
            _state = State::AfterShiftExpr;
            _mode = ShifterMode::RotateWithExtend;
            node = this;
        }
        else if (_shiftType != ShiftType::None)
        {
            // Start working on the shift expression.
            context.pushLexicalContext(getExpressionLexer());
            _mode = ShifterMode::ShiftByRegister;
            _state = State::AfterShift;
            node = this;
        }
        break;

    case State::AfterShift:
        // Expect '#' before a constant or a register expression.
        if (token.getClass() == TokenClass::Hash)
        {
            _mode = ShifterMode::ShiftByConstant;
            _state = State::AfterShiftHash;
            node = this;
        }
        break;

    case State::AfterShiftHash:
        // Expect a constant value expression.
        break;

    case State::AfterShiftExpr:
        // Expect the end of the statement.
        if (token.getClass() == _expectedEnd)
        {
            // We're done, continue parsing the parent.
            _state = State::Complete;
            node = this;
        }
        break;

    case State::Complete:
        // Nothing more to find.
        break;
    }

    return node;
}

// Inherited from ISyntaxNode.
ISyntaxNode *ShifterOperandNode::applyNode(ParseContext &context,
                                           ISyntaxNode *childNode)
{
    ISyntaxNode *node = nullptr;
    ExpressionNodePtr expr = nullptr;

    switch (_state)
    {
    case State::Start:
    case State::AfterInitialSign:
        // Expect a '#' before a constant or a register expression node.
        if (tryCast(childNode, expr))
        {
            node = this;
            _baseExpr.reset(expr);
            _mode = ShifterMode::Register;
            _state = State::AfterInitialReg;

            // Start looking for a shift mnemonic or the end of the statement.
            restoreLexicalState(context);
        }
        break;

    case State::AfterInitialHash:
        // Expect an immediate constant expression node.
        if (tryCast(childNode, expr))
        {
            // Its an immediate constant.
            node = this;
            _baseExpr.reset(expr);
            _mode = ShifterMode::ImmediateConstant;

            // Skip the shift mnemonic and expression.
            _state = State::AfterShiftExpr;

            // Start looking for the end of the statement.
            restoreLexicalState(context);
        }
        break;

    case State::AfterInitialReg:
        // Expect a comma ',' or the end of the statement.
        break;

    case State::BeforeShift:
        // Expect a shift mnemonic.
        break;

    case State::AfterShift:
        // Expect a '#' before a constant or a register expression.
        if (tryCast(childNode, expr))
        {
            node = this;
            _shiftExpr.reset(expr);
            _state = State::AfterShiftExpr;
            _mode = ShifterMode::ShiftByRegister;

            // Start looking for the end of the statement.
            restoreLexicalState(context);
        }
        break;

    case State::AfterShiftHash:
        // Expect a constant expression.
        if (tryCast(childNode, expr))
        {
            node = this;
            _shiftExpr.reset(expr);
            _state = State::AfterShiftExpr;
            _mode = ShifterMode::ShiftByConstant;

            // Start looking for the end of the statement.
            restoreLexicalState(context);
        }
        break;

    case State::AfterShiftExpr:
        // Expect the end of the statement.
        break;

    case State::Complete:
        // Nothing more to do.
        break;
    }

    return node;
}

// Inherited from ISyntaxNode.
void ShifterOperandNode::recover(ParseContext &context, ISyntaxNode *node)
{
    safeDelete(node);

    if (_expectedEnd == TokenClass::StatementTerminator)
    {
        // Skip tokens to the end of the statement.
        context.recover(TokenClass::StatementTerminator);
    }
    else
    {
        // Skip tokens to the end of the operand, but at worst, the end of
        // the statement.
        Token current;
        bool done = false;

        while (!done && context.tryGetNextToken(current))
        {
            auto type = current.getClass();

            if ((type == _expectedEnd) ||
                (type == TokenClass::StatementTerminator))
            {
                done = true;
            }
        }
    }

    restoreLexicalState(context);
}

////////////////////////////////////////////////////////////////////////////////
// StatementShifterOperand Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to receive the fully parsed parameters of a
//! barrel shifter operand.
StatementShifterOperand::StatementShifterOperand() :
    _mode(ShifterMode::Register),
    _shiftType(ShiftType::None),
    _allowsNegated(false),
    _isNegated(false)
{
}

//! @brief Constructs an object containing unresolved shifter operand parameters
//! by transferring expressions and values from another.
//! @param[in] rhs The object to transfer to the current one.
StatementShifterOperand::StatementShifterOperand(StatementShifterOperand &&rhs) noexcept :
    _baseExpr(std::move(rhs._baseExpr)),
    _shiftExpr(std::move(rhs._shiftExpr)),
    _mode(rhs._mode),
    _shiftType(rhs._shiftType),
    _allowsNegated(rhs._allowsNegated),
    _isNegated(rhs._isNegated)
{
    rhs._mode = ShifterMode::Register;
    rhs._shiftType = ShiftType::None;
    rhs._allowsNegated = false;
    rhs._isNegated = false;
}

//! @brief Gets whether the entire operand was proceeded by a minus '-' sign.
bool StatementShifterOperand::isNegated() const { return _isNegated; }

//! @brief Initialises the object with a description of an immediate constant 
//! operand.
//! @param[in] at The source position of the first token of the operand.
//! @param[in] immediateExpr The constant expression to take ownership of.
//! @param[in] allowNegative True to allow negative values and to encode them
//! as positive, but return true from the isNegated() member function.
void StatementShifterOperand::initialiseConstant(const Location &at,
                                                 IExprUPtr &&immediateExpr,
                                                 bool allowNegative)
{
    _start = at;
    _baseExpr = std::move(immediateExpr);
    _shiftExpr.reset();
    _mode = ShifterMode::ImmediateConstant;
    _shiftType = ShiftType::None;
    _allowsNegated = allowNegative;
    _isNegated = false;
}

//! @brief Initialises the object with a description of an un-shifted
//! register operand.
//! @param[in] at The source position of the first token of the operand.
//! @param[in] rmExpr The expression describing the register to take
//! ownership of.
//! @param[in] isNegated True if the operand was proceeded by a minus.
void StatementShifterOperand::initialiseRegister(const Location &at,
                                                 IExprUPtr &&rmExpr,
                                                 bool isNegated)
{
    _start = at;
    _baseExpr = std::move(rmExpr);
    _shiftExpr.reset();
    _mode = ShifterMode::Register;
    _shiftType = ShiftType::None;
    _isNegated = isNegated;
}

//! @brief Initialises the object with a description of a register shifted
//! using RRX.
//! @param[in] at The source position of the first token of the operand.
//! @param[in] rmExpr The expression describing the shifted register to take
//! ownership of.
//! @param[in] isNegated True if the operand was proceeded by a minus.
void StatementShifterOperand::initialiseRrxShift(const Location &at,
                                                 IExprUPtr &&rmExpr,
                                                 bool isNegated)
{
    _start = at;
    _baseExpr = std::move(rmExpr);
    _shiftExpr.reset();
    _mode = ShifterMode::RotateWithExtend;
    _shiftType = ShiftType::Rrx;
    _isNegated = isNegated;
}

//! @brief Initialises the object with a description of a register shifted
//! by another register.
//! @param[in] at The source position of the first token of the operand.
//! @param[in] rmExpr The expression describing the shifted register to take
//! ownership of.
//! @param[in] shift The shift operator being applied.
//! @param[in] rsExpr The expression describing the shift register to
//! take ownership of.
//! @param[in] isNegated True if the operand was proceeded by a minus.
void StatementShifterOperand::initialiseShiftByRegister(const Location &at,
                                                        IExprUPtr &&rmExpr,
                                                        ShiftType shift,
                                                        IExprUPtr &&rsExpr,
                                                        bool isNegated)
{
    _start = at;
    _baseExpr = std::move(rmExpr);
    _shiftExpr = std::move(rsExpr);
    _mode = ShifterMode::ShiftByRegister;
    _shiftType = shift;
    _isNegated = isNegated;
}

//! @brief Initialises the object with a description of a register shifted
//! by a constant.
//! @param[in] at The source position of the first token of the operand.
//! @param[in] rmExpr The expression describing the shifted register to take
//! ownership of.
//! @param[in] shift The shift operator being applied.
//! @param[in] constExpr The expression describing the shift constant to
//! take ownership of.
//! @param[in] isNegated True if the operand was proceeded by a minus.
void StatementShifterOperand::initialiseShiftByConstant(const Location &at,
                                                        IExprUPtr &&rmExpr,
                                                        ShiftType shift,
                                                        IExprUPtr &&constExpr,
                                                        bool isNegated)
{
    _start = at;
    _baseExpr = std::move(rmExpr);
    _shiftExpr = std::move(constExpr);
    _mode = ShifterMode::ShiftByConstant;
    _shiftType = shift;
    _isNegated = isNegated;
}

//! @brief Configures an operand within an instruction description according
//! to the current state of the object.
//! @param[in] context The context used to evaluate any expressions which
//! need to be resolved.
//! @param[out] operand The operand to initialise with parameters.
//! @param[out] wasNegated Receives a flag determining of the field was negated.
//! @param[in] log The log to add messages to if the operand cannot be configured.
//! @param[in] isFinalPass True if errors should be reported, false if only a
//! failure condition should be returned on exit.
//! @retval true The operand was properly resolved and configured.
//! @retval false The operand could not be resolved. If isFinalPass is true,
//! an error will be returned.
bool StatementShifterOperand::configure(IEvalContext *context,
                                        ShifterOperand &operand, bool &wasNegated,
                                        Messages &log, bool isFinalPass) const
{
    // Reset the operand state.
    operand.Mode = _mode;
    operand.Shift = _shiftType;
    operand.Rm = CoreRegister::R0;
    operand.Rs = CoreRegister::R0;
    operand.Immediate = 0;

    bool isOK = false;
    wasNegated = false;

    if ((_mode == ShifterMode::Register) ||
        (_mode == ShifterMode::RotateWithExtend) ||
        (_mode == ShifterMode::ShiftByRegister) ||
        (_mode == ShifterMode::ShiftByConstant))
    {
        isOK = true;
        wasNegated = _isNegated;
        String evalError;

        if (tryEvaluateCoreRegister(context, _baseExpr.get(),
                                    operand.Rm, evalError) == false)
        {
            if (isFinalPass)
            {
                std::string builder("Failed to evaluate shifted register expression: ");
                appendAgString(builder, evalError);

                log.appendError(_start, String(builder));
            }

            isOK = false;
        }

        if (_mode == ShifterMode::ShiftByRegister)
        {
            if (isOK && tryEvaluateCoreRegister(context, _shiftExpr.get(),
                                                operand.Rs, evalError) == false)
            {
                if (isFinalPass)
                {
                    std::string builder("Failed to evaluate shift register expression: ");
                    appendAgString(builder, evalError);

                    log.appendError(_start, String(builder));
                }

                isOK = false;
            }
        }
        else if (_mode == ShifterMode::ShiftByConstant)
        {
            if (isOK && tryEvaluateOrdinal(context, _shiftExpr.get(),
                                           operand.Immediate,
                                           evalError) == false)
            {
                if (isFinalPass)
                {
                    std::string builder("Failed to evaluate shift value expression: ");
                    appendAgString(builder, evalError);

                    log.appendError(_start, String(builder));
                }
                isOK = false;
            }
        }
    }
    else if (_mode == ShifterMode::ImmediateConstant)
    {
        String evalError;

        if (tryEvaluateOrdinal(context, _baseExpr.get(),
                               operand.Immediate,
                               evalError))
        {
            if (_allowsNegated)
            {
                int32_t signedImmedate = static_cast<int32_t>(operand.Immediate);

                if (signedImmedate < 0)
                {
                    operand.Immediate = static_cast<uint32_t>(-signedImmedate);
                    wasNegated = true;
                }
            }

            isOK = true;
        }
        else if (isFinalPass)
        {
            std::string builder("Failed to evaluate immediate constant expression: ");
            appendAgString(builder, evalError);

            log.appendError(_start, String(builder));
        }
    }

    return isOK;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


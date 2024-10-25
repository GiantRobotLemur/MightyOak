//! @file AddressOperandNode.cpp
//! @brief The definition of a syntax node which represents a memory operand.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AddressOperandNode.hpp"
#include "ConstantSet.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// AddressOperandNode Member Definitions
////////////////////////////////////////////////////////////////////////////////
AddressOperandNode::AddressOperandNode(ParseContext &context) :
    BaseSyntaxNode(context),
    _flags(0),
    _state(State::BeforeOperand)
{
    context.pushLexicalContext(getExpressionLexer());
}

//! @brief Compiles the parsed syntax node into an object which forms part of
//! an instruction statement.
//! @param[out] statement The address operand statement to initialise with what
//! was parsed from the source text.
//! @param[in] log A log to add errors to if the function fails.
//! @retval true The operand was successfully compiled, statement is updated.
//! @retval false The operand could not be compiled, messages were added to log.
bool AddressOperandNode::compile(StatementAddressOperand &statement,
                                 Messages &log) const
{
    bool isOK = false;

    if (_flags & IsPCRelative)
    {
        if (_base)
        {
            IExprUPtr addr(constantOptimise(_base->compile(ConstantSet::Empty)));

            if (addr)
            {
                statement.initialisePCRelative(_start, std::move(addr));
                isOK = true;
            }
        }

        if (isOK == false)
        {
            log.appendError(_start, "Unable to compile PC-relative address expression.");
        }
    }
    else
    {
        if (_base)
        {
            IExprUPtr baseRegExpr(constantOptimise(_base->compile(getCoreRegSymbols())));
            StatementShifterOperandUPtr offset;

            if (_offset)
            {
                offset = std::make_unique<StatementShifterOperand>();

                if (_offset->compile(*offset, log))
                {
                    isOK = true;
                }
            }
            else
            {
                // There was no offset operand and that was expected.
                isOK = true;
            }

            if (isOK)
            {
                statement.initialiseIndexed(_start, std::move(baseRegExpr),
                                            std::move(offset), _flags);
            }
        }
    }

    return isOK;
}

// Inherited from ISyntaxNode.
bool AddressOperandNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool AddressOperandNode::isValid() const
{
    bool isValid = false;

    if (_flags & IsPCRelative)
    {
        isValid = static_cast<bool>(_base);
    }
    else if (_flags & IsPreIndexed)
    {
        isValid = _base && _offset && _offset->isValid();
    }
    else
    {
        isValid = static_cast<bool>(_base);

        // In post-indexed addressing, the offset is optional.
        if (_offset)
        {
            isValid &= _offset->isValid();
        }
    }

    return isValid;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AddressOperandNode::applyToken(ParseContext &context,
                                            const Token &token)
{
    ISyntaxNode *result = nullptr;
    TokenClass tokenType = token.getClass();

    switch (_state)
    {
    case State::BeforeOperand:
        // Expects An address expression or an opening square '['.
        _start = token.getLocation();

        if (tokenType == TokenClass::OpenSquare)
        {
            result = this;
            _state = State::BeforeBaseReg;
        }
        break;

    case State::BeforeBaseReg:
        // Expects a register expression.
        break;

    case State::AfterBaseReg:
        // Expects: A close square ']' or a comma ','.
        if (tokenType == TokenClass::CloseSquare)
        {
            result = this;
            _state = State::AfterPostIndexBase;
        }
        else if (tokenType == TokenClass::Comma)
        {
            // We're before a shifter operand representing the
            // offset which will match the closing ']' of the address.
            auto shifter = new ShifterOperandNode(context, true, TokenClass::CloseSquare);
            result = shifter;
            context.pushSyntaxNode(this);
            _state = State::BeforePreIndexOffset;
            _flags |= IsPreIndexed;
        }
        break;

    case State::BeforePreIndexOffset:
        // Expects a ShifterOperandNode.
        break;

    case State::AfterPreIndexAddr:
        // Expects A bang '!' or a statement terminator.
        if (tokenType == TokenClass::Bang)
        {
            _flags |= WriteBack;
            result = this;
            _state = State::AfterWriteBack;
        }
        else if (tokenType == TokenClass::StatementTerminator)
        {
            result = this;
            _state = State::Complete;
        }
        break;

    case State::AfterWriteBack:
        // Expects a statement terminator.
        if (tokenType == TokenClass::StatementTerminator)
        {
            result = this;
            _state = State::Complete;
        }
        break;

    case State::AfterPostIndexBase:
        // Expects a comma or a statement terminator.
        if (tokenType == TokenClass::Comma)
        {
            // Expect a shifter operand giving the offset.
            context.pushSyntaxNode(this);
            auto shifter = new ShifterOperandNode(context, true);
            result = shifter;
            _state = State::BeforePostIndexOffset;
        }
        else if (tokenType == TokenClass::StatementTerminator)
        {
            // There is no offset operand.
            result = this;
            _state = State::Complete;
        }
        break;

    case State::BeforePostIndexOffset:
        // Expects a ShifterOperandNode.
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AddressOperandNode::applyNode(ParseContext &context,
                                           ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr expr = nullptr;
    ShifterOperandNode *shifter = nullptr;

    switch (_state)
    {
    case State::BeforeOperand:
        // Expects An address expression or an opening square '['.
        if (Ag::tryCast(childNode, expr))
        {
            // It was just an address expression, that was easy!
            _base.reset(expr);
            _flags |= IsPCRelative;
            _state = State::Complete;
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::BeforeBaseReg:
        // Expects a register expression.
        if (Ag::tryCast(childNode, expr))
        {
            _base.reset(expr);
            _state = State::AfterBaseReg;
            restoreLexicalState(context);
            result = this;
        }
        break;

    case State::AfterBaseReg:
        // Expects: A close square ']' or a comma ','.
        break;

    case State::BeforePreIndexOffset:
        // Expects a ShifterOperandNode.
        if (Ag::tryCast(childNode, shifter))
        {
            // We have the composite offset field, including the
            // closing square ']'.
            _offset.reset(shifter);

            if (shifter->isNegative())
            {
                _flags |= NegativeOffset;
            }

            _state = State::AfterPreIndexAddr;
            result = this;
        }
        break;

    case State::AfterPreIndexAddr:
        // Expects A bang '!' or a statement terminator.
        break;

    case State::AfterWriteBack:
        // Expects a statement terminator.
        break;

    case State::AfterPostIndexBase:
        // Expects a comma or a statement terminator.
        break;

    case State::BeforePostIndexOffset:
        // Expects a ShifterOperandNode.
        if (Ag::tryCast(childNode, shifter))
        {
            // Receive the offset shifter which includes the statement
            // terminator.
            _offset.reset(shifter);
            _state = State::Complete;

            if (shifter->isNegative())
            {
                _flags |= NegativeOffset;
            }

            result = this;
        }
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void AddressOperandNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;
    BaseSyntaxNode::recover(context, node);

    context.recover(TokenClass::StatementTerminator);
}

////////////////////////////////////////////////////////////////////////////////
// StatementAddressOperand Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a fully parsed address operand.
StatementAddressOperand::StatementAddressOperand() :
    _mode(AddressingMode::PostIndexed),
    _hasWriteback(false)
{
}

//! @brief Moves the contents of one address operand into another.
//! @param[in,out] rhs The operand to move from.
StatementAddressOperand::StatementAddressOperand(StatementAddressOperand &&rhs) :
    _start(std::move(rhs._start)),
    _baseExpr(std::move(rhs._baseExpr)),
    _offset(std::move(rhs._offset)),
    _mode(rhs._mode),
    _hasWriteback(rhs._hasWriteback)
{
    rhs._mode = AddressingMode::PostIndexed;
    rhs._hasWriteback = false;
}

//! @brief Gets the addressing mode the described operand uses.
AddressingMode StatementAddressOperand::getAddressMode() const
{
    return _mode;
}

//! @brief Gets the expression forming the base address, possibly the PC-relative
//! offset of the operand. Can be nullptr.
const IExpr *StatementAddressOperand::getBaseAddress() const
{
    return _baseExpr.get();
}

//! @brief Gets the shifter operand representing the offset applied to the
//! base address term. Can be nullptr.
const StatementShifterOperand *StatementAddressOperand::getOffset() const
{
    return _offset.get();
}

//! @brief Initialises the object with a PC-relative address.
//! @param[in] at The source location of the start of the operand.
//! @param[in] addrExpr The compiled expression giving the address to
//! calculate using a PC-relative offset to take ownership of.
void StatementAddressOperand::initialisePCRelative(const Location &at,
                                                   IExprUPtr &&addrExpr)
{
    _start = at;
    _baseExpr = std::move(addrExpr);
    _offset.reset();
    _mode = AddressingMode::PCRelative;
    _hasWriteback = false;
}

//! @brief Initialises the object with an indexed address.
//! @param[in] at The source location of the start of the operand.
//! @param[in] baseRegExpr The expression to take ownership of giving the
//! register specifying the base address.
//! @param[in] offsetExpr The shifter operand to take ownership of giving the
//! offset applied to be base address after use.
//! @param[in] flags Options produced by the AddressOperandNode during parsing.
void StatementAddressOperand::initialiseIndexed(const Location &at,
                                                IExprUPtr &&baseRegExpr,
                                                StatementShifterOperandUPtr &&offsetExpr,
                                                uint32_t flags)
{
    _start = at;
    _baseExpr = std::move(baseRegExpr);
    _offset = std::move(offsetExpr);
    _mode = (flags & AddressOperandNode::IsPreIndexed) ? AddressingMode::PreIndexed :
                                                         AddressingMode::PostIndexed;
    _hasWriteback = (flags & AddressOperandNode::WriteBack) != 0;
}

//! @brief Attempts to configure a partially initialised instruction with the
//! details of the address operand.
//! @param[in] instruction The partially initialised instruction to configure.
//! @param[in] context The context used to evaluate expressions.
//! @param[in] log The log to add messages to if the operand cannot be described
//! in the specified instruction class.
//! @param[in] isFinalPass True to indicate that errors should be reported.
//! @retval true The address operand was properly encoded.
//! @retval false The address operand could not be encoded given the instruction
//! class, an error will have been added to the log.
bool StatementAddressOperand::configure(InstructionInfo &instruction,
                                        IEvalContext *context,
                                        Messages &log, bool isFinalPass) const
{
    bool isOK = false;
    AddrOperand *addrOperand = nullptr;
    uint32_t maxOffset = 0;

    switch (instruction.getOperationClass())
    {
    case OperationClass::CoreDataTransfer: {
        CoreDataTransfer &info = instruction.getCoreDataTransferParameters();
        addrOperand = &info.Addr;

        if ((info.DataType != TransferDataType::UnsignedByte) &&
            (info.DataType != TransferDataType::Word))
        {
            maxOffset = 0xFF;
        }
    } break;

    case OperationClass::CoProcDataTransfer:
        addrOperand = &instruction.getCoProcDataTransferParameters().Addr;
        maxOffset = 0x3FC;
        break;

    default:
        break;
    }

    if (addrOperand == nullptr)
    {
        // We shouldn't get here, hence an exception rather than a logged error.
        throw Ag::NotSupportedException("Encoding an address operand in the required instruction type.");
    }
    else if (tryResolve(context, log, isFinalPass, *addrOperand, maxOffset))
    {
        // Verify the results.
        switch (instruction.getOperationClass())
        {
        case OperationClass::CoreDataTransfer:
            isOK = true;
            break;

        case OperationClass::CoProcDataTransfer:
            if (addrOperand->Offset.Mode == ShifterMode::ImmediateConstant)
            {
                if (addrOperand->Offset.Immediate & 0x03)
                {
                    if (isFinalPass)
                    {
                        log.appendError(_start, "The offset must be a multiple of 4.");
                    }
                }
                else
                {
                    isOK = true;
                }
            }
            else if (isFinalPass)
            {
                log.appendError(_start,
                                "Only constant offsets can be specified in "
                                "co-processor data transfer instructions.");
            }
            break;

        default:
            isOK = true;
            break;
        }

    }

    return isOK;
}

//! @brief Attempts to resolve any outstanding expressions to assemble the
//! address operand specification into an instruction.
//! @param[in] context The context used to evaluate expressions.
//! @param[in] log The log to add errors to if not on the final pass.
//! @param[in] isFinalPass True if errors should be produced on failure, false
//! to simply return a failure.
//! @param[out] operand Receives the base and offset operand to encode.
//! @param[in] maxOffset The maximum offset encodable, 0 for a full 12-bit
//! offset of shifter operand, non-zero for a smaller offset size and to
//! disable support for shifted offsets.
//! @retval true The address operand was successfully resolved.
//! @retval false The address operand could not be resolved, errors should
//! have been added to the log if isFinalPass was true.
bool StatementAddressOperand::tryResolve(IEvalContext *context, Messages &log,
                                         bool isFinalPass, AddrOperand &operand,
                                         uint32_t maxOffset) const
{
    Ag::String error;
    bool isOK = false;
    bool restrictAddrMode;

    if (maxOffset == 0)
    {
        maxOffset = 0xFFF;
        restrictAddrMode = false;
    }
    else
    {
        restrictAddrMode = true;
    }

    if (_mode == AddressingMode::PCRelative)
    {
        uint32_t addr;

        if (tryEvaluateOrdinal(context, _baseExpr.get(), addr, error))
        {
            operand.Rn = CoreRegister::R15;
            operand.Flags = AddrOperand::PreIndexed;
            operand.Offset.Mode = ShifterMode::ImmediateConstant;
            operand.Offset.Shift = ShiftType::None;
            operand.Offset.Rm = operand.Offset.Rs = CoreRegister::R0;

            uint32_t pcAddr = context->getAssemblyAddress() + 8;

            if (pcAddr <= addr)
            {
                operand.Offset.Immediate = addr - pcAddr;
            }
            else
            {
                operand.Flags |= AddrOperand::NegativeOffset;
                operand.Offset.Immediate = pcAddr - addr;
            }

            if (operand.Offset.Immediate > maxOffset)
            {
                if (isFinalPass)
                {
                    log.appendError(_start,
                                    "The PC-relative offset is too large to be encoded.");
                }
            }
            else
            {
                isOK = true;
            }
        }
        else if (isFinalPass)
        {
            std::string builder("Failed to resolve address expression: ");
            Ag::appendAgString(builder, error);

            log.appendError(_start, Ag::String(builder));
        }
    }
    else if (tryEvaluateCoreRegister(context, _baseExpr.get(), operand.Rn, error))
    {
        if (_offset)
        {
            bool wasNegated = false;

            if (_offset->configure(context, operand.Offset,
                                   wasNegated, log, isFinalPass))
            {
                if (wasNegated)
                {
                    operand.Flags |= AddrOperand::NegativeOffset;
                }

                if (_mode == AddressingMode::PreIndexed)
                {
                    operand.Flags |= AddrOperand::PreIndexed;

                    if (_hasWriteback)
                    {
                        // Only possible in a pre-indexed addressing mode.
                        operand.Flags |= AddrOperand::Writeback;
                    }
                }

                if (restrictAddrMode)
                {
                    if (operand.Offset.Mode == ShifterMode::ImmediateConstant)
                    {
                        if (operand.Offset.Immediate > maxOffset)
                        {
                            if (isFinalPass)
                            {
                                log.appendError(_start,
                                                "The offset value is too large to encode.");
                            }
                        }
                        else
                        {
                            isOK = true;
                        }
                    }
                    else if (operand.Offset.Mode != ShifterMode::Register)
                    {
                        if (isFinalPass)
                        {
                            log.appendError(_start,
                                            "Shifts cannot be used in the address field "
                                            "of a signed/half-word data transfer.");
                        }
                    }
                    else
                    {
                        isOK = true;
                    }
                }
                else
                {
                    isOK = true;
                }
            }
        }
        else
        {
            // No offset.
            operand.Flags = AddrOperand::PreIndexed;
            operand.Offset.Mode = ShifterMode::ImmediateConstant;
            operand.Offset.Immediate = 0;
            operand.Offset.Shift = ShiftType::None;
            operand.Offset.Rm = operand.Offset.Rs = CoreRegister::R0;
            isOK = true;
        }
    }
    else if (isFinalPass)
    {
        std::string builder("Failed to resolve base address register expression: ");
        appendAgString(builder, error);

        log.appendError(_start, Ag::String(builder));
    }

    return isOK;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


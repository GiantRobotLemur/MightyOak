//! @file DataTransferInstructionNode.cpp
//! @brief The definition of an object which parses a core single register
//! data transfer instruction.
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
#include "DataTransferInstructionNode.hpp"
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
//! @brief An object representing a fully parsed core single-register data
//! transfer instruction.
class DataTransferInstructionStatement : public InstructionStatement
{
private:
    // Internal Fields
    IExprUPtr _rd;
    StatementAddressOperandUPtr _addr;
    TransferDataType _dataType;
    bool _userPrivilages;
public:
    // Construction/Destruction
    DataTransferInstructionStatement(const Location &start,
                                     InstructionMnemonic mnemonic,
                                     ConditionCode condition,
                                     TransferDataType dataType,
                                     bool userPrivilages,
                                     IExprUPtr &&rd,
                                     StatementAddressOperandUPtr &&addr) :
        InstructionStatement(start, OperationClass::CoreDataTransfer,
                             mnemonic, condition),
        _rd(std::move(rd)),
        _addr(std::move(addr)),
        _dataType(dataType),
        _userPrivilages(userPrivilages)
    {
    }

    ~DataTransferInstructionStatement() = default;

protected:
    // Overrides
    // Inherited from InstructionStatement.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const override
    {
        auto &info = instruction.getCoreDataTransferParameters();
        String error;
        bool isOK = true;

        if (getMnemonic() == InstructionMnemonic::Str)
        {
            if (_dataType == TransferDataType::SignedByte)
            {
                log.appendError(getStart(), "The STR instruction does not support signed bytes.");
                isOK = false;
            }
            else if (_dataType == TransferDataType::SignedHalfWord)
            {
                log.appendError(getStart(), "The STR instruction does not support signed half-words.");
                isOK = false;
            }
        }

        if (tryEvaluateCoreRegister(context, _rd.get(), info.Rd, error))
        {
            if (_addr && _addr->configure(instruction, context, log, isFinalPass))
            {
                info.UserPrivilages = _userPrivilages;
                info.DataType = _dataType;
            }
            else
            {
                isOK = false;
            }
        }
        else
        {
            if (isFinalPass)
            {
                std::string builder("Failed to evaluate destination register: ");
                appendAgString(builder, error);

                log.appendError(getStart(), String(builder));

            }

            isOK = false;
        }

        return isOK;
    }

    // Inherited from InstructionStatement.
    virtual bool validate(const InstructionInfo &instruction,
                          const AssemblyState &state,
                          Messages &log) const override
    {
        bool isValid = true;

        TransferDataType dataType = instruction.getCoreDataTransferParameters().DataType;

        if ((dataType != TransferDataType::UnsignedByte) &&
            (dataType != TransferDataType::Word))
        {
            // Load/store with sign extension or half-words requires ARMv4.
            isValid = validateInstructionSet(state, log, InstructionSet::ArmV4);
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
// DataTransferInstructionNode Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node to parse a core single register data
//! transfer instruction.
//! @param[in] context The current state of the parser.
//! @param[in] mnemonic The token representing the instruction mnemonic.
DataTransferInstructionNode::DataTransferInstructionNode(ParseContext &context,
                                                         const Token &mnemonic) :
    StatementNode(context, mnemonic),
    _state(State::AfterMnemonic),
    _mnemonic(getTokenEnum(mnemonic, TokenProperty::Mnemonic, InstructionMnemonic::Ldr)),
    _condition(getTokenEnum(mnemonic, TokenProperty::ConditionCode, ConditionCode::Al)),
    _dataType(getTokenEnum(mnemonic, TokenProperty::TransferDataType, TransferDataType::Word)),
    _userPrivilages(getTokenFlag(mnemonic, TokenProperty::UserPrivilage, false))
{
    context.pushLexicalContext(getExpressionLexer());
}

// Inherited from ISyntaxNode.
bool DataTransferInstructionNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool DataTransferInstructionNode::isValid() const
{
    return _destReg && _addr && _addr->isValid();
}

// Inherited from ISyntaxNode.
ISyntaxNode *DataTransferInstructionNode::applyToken(ParseContext &context,
                                                     const Token &token)
{
    ISyntaxNode *result = nullptr;

    switch (_state)
    {
    case State::AfterMnemonic:
        // Expect destination register expression.
        break;

    case State::AfterBaseReg:
        // Expect a comma ','.
        if (token.getClass() == TokenClass::Comma)
        {
            // Push us onto the stack as partially complete.
            context.pushSyntaxNode(this);

            // Begin parsing the address operand.
            result = new AddressOperandNode(context);
            _state = State::BeforeAddr;
        }
        break;

    case State::BeforeAddr:
        // Expect an address operand.
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *DataTransferInstructionNode::applyNode(ParseContext &context,
                                                    ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    ExpressionNodePtr expr;
    AddressOperandNode *addr;

    switch (_state)
    {
    case State::AfterMnemonic:
        // Expect destination register expression.
        if (tryCast(childNode, expr))
        {
            restoreLexicalState(context);
            _destReg.reset(expr);
            result = this;
            _state = State::AfterBaseReg;
        }
        break;

    case State::AfterBaseReg:
        // Expect a comma ','.
        break;

    case State::BeforeAddr:
        // Expect an address operand.
        if (tryCast(childNode, addr))
        {
            _addr.reset(addr);
            result = this;
            _state = State::Complete;
        }
        break;

    case State::Complete: break;
    }

    return result;
}

// Inherited from ISyntaxNode.
void DataTransferInstructionNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;

    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *DataTransferInstructionNode::compile(Messages &output) const
{
    Statement *statement = nullptr;

    // Attempt to resolve the destination register.
    IExprUPtr rd(constantOptimise(_destReg->compile(getCoreRegSymbols())));

    if (_addr)
    {
        StatementAddressOperandUPtr addr = std::make_unique<StatementAddressOperand>();

        if (_addr->compile(*addr, output))
        {
            statement = new DataTransferInstructionStatement(getStart(),
                                                             _mnemonic,
                                                             _condition,
                                                             _dataType,
                                                             _userPrivilages,
                                                             std::move(rd),
                                                             std::move(addr));
        }
    }

    return statement;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


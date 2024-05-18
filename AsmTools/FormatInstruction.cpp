//! @file AsmTools/FormatInstruction.cpp
//! @brief The definition of an entry point to a function which will format a
//! 32-bit ARM machine code instruction as text.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/Utils.hpp"

#include "Disassembly.hpp"
#include "FormatInstruction.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Formats an a non-instruction as text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatNonInstruction(FormatParams& params)
{
    const auto& info = params.Params->Bits;
    auto& builder = params.Builder;

    builder.beginToken(FormattedInstruction::TokenType::DataDirectiveMnemonic);
    builder.append("EQUD");
    builder.endToken();
    builder.append(' ');
    builder.beginToken(FormattedInstruction::TokenType::DataValue, info.Bits[0]);
    builder.append('&');
    builder.appendHexValue(info.Bits[0], 8);
    builder.endToken();
}

//! @brief Formats the parameters of a barrel shifter operand in the instruction
//! text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
//! @param[in] shifter The barrel shifter operand to format.
//! @param[in] isNegated True if the shifter operand should be proceeded by a
//! minus sign, either inside the constant or before the register.
void formatShifterOperand(InstructionBuilder &builder,
                          uint32_t formatOptions,
                          const ShifterOperand &shifter,
                          bool isNegated = false)
{
    switch (shifter.Mode)
    {
    case ShifterMode::ImmediateConstant:
        if (isNegated)
        {
            builder.beginToken(FormattedInstruction::TokenType::ImmediateConstant,
                               static_cast<uint32_t>(static_cast<int32_t>(shifter.Immediate) * -1));
            builder.append('#');
            builder.append(static_cast<int32_t>(shifter.Immediate) * -1);
        }
        else
        {
            builder.beginToken(FormattedInstruction::TokenType::ImmediateConstant,
                               shifter.Immediate);
            builder.append('#');
            builder.append(shifter.Immediate);
        }

        builder.endToken();
        break;

    case ShifterMode::Register:
        builder.append(shifter.Rm, formatOptions, isNegated);
        break;

    case ShifterMode::ShiftByRegister:
        builder.append(shifter.Rm, formatOptions, isNegated);
        builder.appendSeparator();
        builder.append(shifter.Shift);
        builder.append(' ');
        builder.append(shifter.Rs, formatOptions);
        break;

    case ShifterMode::ShiftByConstant:
        builder.append(shifter.Rm, formatOptions, isNegated);
        builder.appendSeparator();
        builder.append(shifter.Shift);
        builder.append(' ');
        builder.beginToken(FormattedInstruction::TokenType::ImmediateConstant,
                           shifter.Immediate);
        builder.append('#');
        builder.append(shifter.Immediate);
        builder.endToken();
        break;

    case ShifterMode::RotateWithExtend:
        builder.append(shifter.Rm, formatOptions, isNegated);
        builder.appendSeparator();
        builder.beginToken(FormattedInstruction::TokenType::Shift,
                           Ag::toScalar(ShifterMode::RotateWithExtend));
        builder.append("RRX");
        builder.endToken();
        break;
    }
}

//! @brief Formats an ALU data processing instruction text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreAlu(FormatParams &params)
{
    const auto &info = params.Params->CoreAluOp;
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.appendSuffix('S', info.AffectsFlags);
    builder.endToken();

    builder.append(' ');
    builder.append(info.Rd, formatOptions);
    builder.appendSeparator();

    if ((params.Mnemonic != InstructionMnemonic::Mov) &&
        (params.Mnemonic != InstructionMnemonic::Mvn))
    {
        builder.append(info.Rn, formatOptions);
        builder.appendSeparator();
    }

    formatShifterOperand(params.Builder, formatOptions, info.Op2);
}

//! @brief Formats an ALU comparison instruction text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreComparison(FormatParams &params)
{
    const auto &info = params.Params->CoreCmpOp;
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.appendSuffix('P', info.OverwritesPSR);
    builder.endToken();
    builder.append(' ');
    builder.append(info.Rn, formatOptions);
    builder.appendSeparator();

    formatShifterOperand(params.Builder, formatOptions, info.Op2);
}

//! @brief Formats an address operand into in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatAddressOperand(FormatParams &params, const AddrOperand &addr)
{
    // There is an offset.
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    if (addr.Flags & AddrOperand::PreIndexed)
    {
        if ((addr.Offset.Mode == ShifterMode::ImmediateConstant) &&
            (addr.Offset.Immediate == 0))
        {
            // There is no offset.
            builder.appendToken('[', FormattedInstruction::TokenType::BeginAddrOperand);
            builder.append(addr.Rn, formatOptions);
            builder.appendToken(']', FormattedInstruction::TokenType::EndAddrOperand);

            // Check for erroneous write-back?
        }
        else if ((addr.Rn == CoreRegister::R15) &&
                 (addr.Offset.Mode == ShifterMode::ImmediateConstant))
        {
            // It's a PC-relative address.
            int32_t offset = static_cast<int32_t>(addr.Offset.Immediate);
            uint32_t absAddress = params.Options->getInstructionAddress() + 8;

            if (addr.Flags & AddrOperand::NegativeOffset)
            {
                absAddress -= offset;
            }
            else
            {
                absAddress += offset;
            }

            builder.appendAddress(absAddress, *params.Options);
        }
        else
        {
            // Pre-indexed with offset.
            builder.appendToken('[', FormattedInstruction::TokenType::BeginAddrOperand);
            builder.append(addr.Rn, formatOptions);
            builder.appendSeparator();

            formatShifterOperand(builder, formatOptions, addr.Offset,
                                 addr.Flags & AddrOperand::NegativeOffset);

            builder.appendToken(']', FormattedInstruction::TokenType::EndAddrOperand);

            builder.appendOptionalToken('!', FormattedInstruction::TokenType::WritebackMarker,
                                        addr.Flags & AddrOperand::Writeback);
        }
    }
    else
    {
        // Post-indexed addressing mode.
        builder.appendToken('[', FormattedInstruction::TokenType::BeginAddrOperand);
        builder.append(addr.Rn, formatOptions);
        builder.appendToken(']', FormattedInstruction::TokenType::EndAddrOperand);
        builder.appendSeparator();
        formatShifterOperand(builder, formatOptions, addr.Offset,
                             addr.Flags & AddrOperand::NegativeOffset);
    }
}

//! @brief Formats a core data transfer instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreDataTransfer(FormatParams &params)
{
    const auto &info = params.Params->DataTransOp;
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);

    switch (info.DataType)
    {
    default:
    case TransferDataType::Word:
        break;

    case TransferDataType::SignedByte: builder.append("SB"); break;
    case TransferDataType::UnsignedByte:  builder.append("B"); break;
    case TransferDataType::SignedHalfWord:  builder.append("SH"); break;
    case TransferDataType::UnsignedHalfWord:  builder.append("H"); break;
    }

    builder.appendSuffix('T', info.UserPrivilages);
    builder.endToken();

    builder.append(' ');
    builder.append(info.Rd, formatOptions);
    builder.appendSeparator();

    formatAddressOperand(params, info.Addr);
}


//! @brief Formats a core multi-register data transfer instruction in the text
//! being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreMultiTransfer(FormatParams &params)
{
    static const char *modeText[] = {
        "EA", // EmptyAscending
        "ED", // EmptyDescending
        "FA", // FullAscending
        "FD", // FullDescending
        "IA", // IncrementAfter
        "IB", // IncrementBefore
        "DA", // DecrementAfter
        "DB", // DecrementBefore
    };

    const auto &info = params.Params->MultiTransOp;
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);

    if (info.Mode < MultiTransferMode::Max)
    {
        builder.append(modeText[Ag::toScalar(info.Mode)]);
    }

    builder.endToken();

    builder.append(' ');
    builder.append(info.Rd, formatOptions);

    builder.appendOptionalToken('!', FormattedInstruction::TokenType::WritebackMarker,
                                info.Writeback);
    builder.appendSeparator();

    builder.appendRegisterList(info.Registers, formatOptions);
    builder.appendOptionalToken('^', FormattedInstruction::TokenType::ModifyPsrMaker,
                                info.UserModeRegs);
}

//! @brief Formats an atomic swap instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatAtomicSwap(FormatParams &params)
{
    const auto &info = params.Params->AtomicSwapOp;
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.appendSuffix('B', info.IsByte);
    builder.endToken();

    builder.append(' ');
    builder.append(info.Rd, formatOptions);
    builder.appendSeparator();
    builder.append(info.Rm, formatOptions);
    builder.appendSeparator();
    builder.appendToken('[', FormattedInstruction::TokenType::BeginAddrOperand);
    builder.append(info.Rn, formatOptions);
    builder.appendToken(']', FormattedInstruction::TokenType::EndAddrOperand);
}

//! @brief Formats an MRS instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatMoveToPsr(FormatParams &params)
{
    const auto &info = params.Params->MoveToPsrOp;
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.endToken();

    builder.append(' ');

    builder.append(info.IsCPSR ? CoreRegister::CPSR : CoreRegister::SPSR,
                   formatOptions);

    if (info.PsrComponents != Ag::toScalar(PsrComponent::All))
    {
        builder.append('_');

        builder.appendSuffix('C', info.PsrComponents & Ag::toScalar(PsrComponent::Control));
        builder.appendSuffix('X', info.PsrComponents & Ag::toScalar(PsrComponent::Extension));
        builder.appendSuffix('S', info.PsrComponents & Ag::toScalar(PsrComponent::Status));
        builder.appendSuffix('F', info.PsrComponents & Ag::toScalar(PsrComponent::Flags));

        builder.extendToken();
    }

    builder.appendSeparator();

    if (info.IsSourceReg)
    {
        builder.append(info.SourceReg, formatOptions);
    }
    else
    {
        builder.appendHexImmediate(info.SourceImmediate, 2,
                                   *params.Options);
    }
}

//! @brief Formats an ADR pseudo-instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreAdr(FormatParams &params)
{
    const auto &info = params.Params->CoreAdr;
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.endToken();

    builder.append(' ');
    builder.append(info.Rd, formatOptions);
    builder.appendSeparator();
    builder.appendAddress(info.Address, *params.Options);
}


//! @brief Formats a core multiply instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreMultiply(FormatParams &params)
{
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();
    const auto &info = params.Params->CoreMulOp;

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.appendSuffix('S', info.AffectsFlags);
    builder.endToken();

    builder.append(' ');
    builder.append(info.Rd, formatOptions);
    builder.appendSeparator();
    builder.append(info.Rm, formatOptions);
    builder.appendSeparator();
    builder.append(info.Rs, formatOptions);

    if (params.Mnemonic == InstructionMnemonic::Mla)
    {
        builder.appendSeparator();
        builder.append(info.Rn, formatOptions);
    }
}

//! @brief Formats a branch instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatBranch(FormatParams &params)
{
    auto &builder = params.Builder;

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.endToken();
    builder.append(' ');
    builder.appendAddress(params.Params->BranchOp.Address,
                          *params.Options);
}

//! @brief Formats a software interrupt instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatSoftwareIrq(FormatParams &params)
{
    auto &builder = params.Builder;

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.endToken();

    builder.append(' ');
    builder.appendSwiNumber(params.Params->SoftwareIrqOp.Comment,
                            *params.Options);
}


//! @brief Formats a break point instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatBreakPoint(FormatParams &params)
{
    auto &builder = params.Builder;

    builder.appendMnemonic(params.Mnemonic);
    builder.endToken();
    builder.append(' ');

    // Write a raw value for the comment field.
    builder.appendComment(params.Params->BreakpointOp.Comment,
                          params.Options->getFlags());
}

//! @brief Formats a MRS instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatMoveFromPSR(FormatParams &params)
{
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.endToken();

    builder.append(' ');
    builder.append(params.Params->MoveFromPsrOp.Rd, formatOptions);
    builder.appendSeparator();
    builder.append(params.Params->MoveFromPsrOp.IsCPSR ? CoreRegister::CPSR :
                                                         CoreRegister::SPSR,
                   formatOptions);
}

//! @brief Formats a core long multiply instruction in the text
//! being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatLongMultiply(FormatParams &params)
{
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();

    const auto &info = params.Params->LongMulOp;
    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.appendSuffix('S', info.AffectsFlags);
    builder.endToken();

    builder.append(' ');
    builder.append(info.RdLo, formatOptions);
    builder.appendSeparator();
    builder.append(info.RdHi, formatOptions);
    builder.appendSeparator();
    builder.append(info.Rm, formatOptions);
    builder.appendSeparator();
    builder.append(info.Rs, formatOptions);
}

//! @brief Formats a co-processor data processing instruction in the text
//! being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoProcDataOp(FormatParams &params)
{
    auto &builder = params.Builder;
    const auto &info = params.Params->CoProcDataProcOp;
    Ag::FormatInfo fmt = Ag::FormatInfo::getNeutral();

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.endToken();

    builder.append(' ');
    builder.append(info.CoProcessor);
    builder.appendSeparator();

    builder.beginToken(FormattedInstruction::TokenType::ImmediateConstant);
    builder.append(info.OpCode1);
    builder.endToken();

    builder.appendSeparator();
    builder.append(info.Rd);
    builder.appendSeparator();
    builder.append(info.Rn);
    builder.appendSeparator();
    builder.append(info.Rm);
    builder.appendSeparator();

    builder.beginToken(FormattedInstruction::TokenType::ImmediateConstant);
    builder.append(info.OpCode2);
    builder.endToken();
}

//! @brief Formats a co-processor register transfer instruction in the text
//! being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoProcRegisterTransfer(FormatParams &params)
{
    auto &builder = params.Builder;
    const uint32_t formatOptions = params.Options->getFlags();
    const auto &info = params.Params->CoProcRegTransOp;

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.endToken();

    builder.append(' ');
    builder.append(info.CoProcessor);
    builder.appendSeparator();

    builder.beginToken(FormattedInstruction::TokenType::ImmediateConstant);
    builder.append(info.OpCode1);
    builder.endToken();

    builder.appendSeparator();
    builder.append(info.Rd, formatOptions);
    builder.appendSeparator();
    builder.append(info.Rn);
    builder.appendSeparator();
    builder.append(info.Rm);
    builder.appendSeparator();

    builder.beginToken(FormattedInstruction::TokenType::ImmediateConstant);
    builder.append(info.OpCode2);
    builder.endToken();
}


//! @brief Formats a co-processor data transfer instruction in the text
//! being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoProcDataTransfer(FormatParams &params)
{
    auto &builder = params.Builder;
    const auto &info = params.Params->CoProcDataTransferOp;

    builder.appendMnemonic(params.Mnemonic);
    builder.appendConditionCode(params.Condition);
    builder.appendSuffix('L', info.IsLong);
    builder.endToken();

    builder.append(' ');
    builder.append(info.CoProcessor);
    builder.appendSeparator();
    builder.append(info.Rd);
    builder.appendSeparator();
    formatAddressOperand(params, info.Addr);
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// InstructionInfo Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to accumulate the text of an assembly language
//! instruction.
InstructionBuilder::InstructionBuilder() :
    _pendingTokenStart(0),
    _pendingTokenData(0),
    _pendingTokenType(FormattedInstruction::TokenType::Max),
    _isFormatting(false)
{
}

//! @brief Constructs an object to accumulate the text, and optionally the
//! tokens of an assembly language instruction.
//! @param[in] isFormatting True to accumulate tokens, false to discard them.
InstructionBuilder::InstructionBuilder(bool isFormatting) :
    _pendingTokenStart(0),
    _pendingTokenData(0),
    _pendingTokenType(FormattedInstruction::TokenType::Max),
    _isFormatting(isFormatting)
{
    _buffer.reserve(32);

    if (isFormatting)
    {
        _tokens.reserve(16);
    }
}

//! @brief Gets the token data to applied to the next pending token.
uint32_t InstructionBuilder::getPendingTokenData() const
{
    return _pendingTokenData;
}

//! @brief Updates the data stored when the next token is completed.
//! @param[in] data The new token data value.
void InstructionBuilder::setPendingTokenData(uint32_t data)
{
    _pendingTokenData = data;
}

//! @brief Determines if there is no accumulated text in the builder.
bool InstructionBuilder::isEmpty() const
{
    return _buffer.empty();
}

//! @brief Outputs the accumulated data as a string.
Ag::String InstructionBuilder::toString() const
{
    return Ag::String(_buffer);
}

//! @brief Outputs the accumulated data as a formatted instruction.
FormattedInstruction InstructionBuilder::format()
{
    return FormattedInstruction(_buffer, std::move(_tokens));
}

//! @brief Begins accumulating text of a token.
//! @param[in] tokenType The type of token being accumulated.
//! @param[in] data Optional data associated with the token.
void InstructionBuilder::beginToken(FormattedInstruction::TokenType tokenType,
                                    uint32_t data /*= 0*/)
{
    _pendingTokenStart = _buffer.size();
    _pendingTokenData = data;
    _pendingTokenType = tokenType;
}

//! @brief Completes the token by tagging text accumulated since the last call
//! to beginToken().
void InstructionBuilder::endToken()
{
    if (_isFormatting)
    {
        size_t tokenLength = _buffer.size() - _pendingTokenStart;

        _tokens.emplace_back(_pendingTokenStart, tokenLength,
                             _pendingTokenType, _pendingTokenData);

        _pendingTokenStart = _buffer.size();
        _pendingTokenType = FormattedInstruction::TokenType::Max;
        _pendingTokenData = 0;
    }
}

//! @brief Extends the last token to include all following characters.
void InstructionBuilder::extendToken()
{
    if (_tokens.empty() == false)
    {
        _tokens.back().Count = static_cast<uint16_t>(_buffer.size() - _tokens.back().Start);
    }
}

//! @brief Appends a single character token to the accumulated text.
//! @param[in] ch The single character token to append.
//! @param[in] tokenType The classification of the token the character
//! represents.
void InstructionBuilder::appendToken(char ch,
                                     FormattedInstruction::TokenType tokenType)
{
    size_t offset = _buffer.size();
    _buffer.push_back(ch);

    if (_isFormatting)
    {
        _tokens.emplace_back(offset, 1, tokenType, 0);
    }
}

//! @brief Appends a single character token to the accumulated text.
//! @param[in] ch The single character token to append.
//! @param[in] tokenType The classification of the token the character
//! represents.
//! @param[in] isAppended Indicates whether the character should be appended.
void InstructionBuilder::appendOptionalToken(char ch,
                                             FormattedInstruction::TokenType tokenType,
                                             bool isAppended)
{
    if (isAppended)
    {
        size_t offset = _buffer.size();
        _buffer.push_back(ch);

        if (_isFormatting)
        {
            _tokens.emplace_back(offset, 1, tokenType, 0);
        }
    }
}

//! @brief Appends a separator followed by a space to the string being built.
void InstructionBuilder::appendSeparator()
{
    if (_isFormatting)
    {
        _tokens.emplace_back(_buffer.size(), 1,
                             FormattedInstruction::TokenType::Separator,
                             0);
    }

    _buffer.push_back(',');
    _buffer.push_back(' ');
}

//! @brief Appends the text of the instruction mnemonic to the instruction string
//! being created.
//! @param[in] The mnemonic to append.
//! @note Begins a mnemonic token, but doesn't complete it.
void InstructionBuilder::appendMnemonic(InstructionMnemonic mnemonic)
{
    static const char *names[] = {
        "AND", "EOR", "SUB", "RSB",
        "ADD", "ADC", "SBC", "RSC",
        "TST", "TEQ", "CMP", "CMN",
        "ORR", "MOV", "BIC", "MVN",
        "MUL", "MLA", "B", "BL", "BX",
        "LDR", "STR", "LDM", "STM",
        "SWI", "SWP", "MCR", "MRC",
        "CDP", "LDC", "STC", "MRS", "MSR",
        "UMULL", "UMLAL", "SMULL", "SMLAL", "BKPT",
        "LDF", "STF", "LFM", "SFM",
        "ADF", "MUF", "SUF", "RSF",
        "DVF", "RDF", "POW", "RPW",
        "RMF", "FML", "FDV", "FRD",
        "POL", "(FPA undefined diadic op #1)",
        "(FPA undefined diadic op #2)",
        "(FPA undefined diadic op #3)",
        "MVF", "MNF", "ABS", "RND",
        "SQT", "LOG", "LGN", "EXP",
        "SIN", "COS", "TAN", "ASN",
        "ACS", "ATN", "URD", "NRM",
        "FLT", "FIX", "WFS", "RFS",
        "WFC", "RFC",
        "CMF", "CMFE", "CNF", "CNFE",
        "ADR", "ALIGN",
    };

    static_assert(std::size(names) == Ag::toScalar(InstructionMnemonic::MaxMnemonic),
                  "Instruction mnemonic names are out of sync with the InstructionMnemonic enumeration type.");

    if (mnemonic < InstructionMnemonic::MaxMnemonic)
    {
        if (_isFormatting)
        {
            FormattedInstruction::TokenType type = FormattedInstruction::TokenType::CoreMnemonic;

            if ((mnemonic >= InstructionMnemonic::Mcr) &&
                (mnemonic <= InstructionMnemonic::Msr))
            {
                type = FormattedInstruction::TokenType::CoProcMnemonic;
            }
            else if ((mnemonic >= InstructionMnemonic::Ldf) &&
                     (mnemonic <= InstructionMnemonic::Cnfe))
            {
                type = FormattedInstruction::TokenType::FpaMnemonic;
            }

            beginToken(type, Ag::toScalar(mnemonic));
        }

        _buffer.append(names[Ag::toScalar(mnemonic)]);
    }
    else
    {
        _buffer.append("(invalid instruction)");
    }
}

//! @brief Appends the suffix representing a condition code to the instruction
//! string being accumulated.
void InstructionBuilder::appendConditionCode(ConditionCode condition)
{
    static const char *codes[] = {
        "EQ", "NE", "CS", "CC",
        "MI", "PL", "VS", "VC",
        "HI", "LS", "GE", "LT",
        "GT", "LE", "AL", "NV",
        "HS", "LO",
    };

    static_assert(std::size(codes) == Ag::toScalar(ConditionCode::Max),
                  "Condition codes text is out of sync with ConditionCode enumeration definition.");

    // The AL (always) condition is implicit.
    if ((condition != ConditionCode::Al) &&
        (condition < ConditionCode::Max))
    {
        _buffer.append(codes[Ag::toScalar(condition)], 2u);
    }
}

//! @brief Appends a memory offset formatted as text to a string.
//! @param[in] offset The offset to format.
//! @param[in] formatFlags FormatterOptions flags used to determine how to
//! express the offset.
void InstructionBuilder::appendOffset(int32_t offset, uint32_t formatFlags)
{
    Ag::FormatInfo options;

    beginToken(FormattedInstruction::TokenType::Label,
               static_cast<uint32_t>(offset));

   // Express the offset as relative to the current address, denoted by '$'.
    _buffer.push_back('$');

    if (offset < 0)
    {
        offset = -offset;
        _buffer.push_back('-');
    }
    else // if (offset >= 0)
    {
        _buffer.push_back('+');
    }

    if ((formatFlags & FormatterOptions::UseDecimalOffsets) == 0)
    {
        // Show the hex specifier.
        if (formatFlags & FormatterOptions::UseBasicStyleHex)
        {
            _buffer.push_back('&');
        }
        else
        {
            _buffer.push_back('0');
            _buffer.push_back('x');
        }

        options.setRadix(16);
        options.setMinimumWholeDigits(2);
    }

    Ag::appendValue(options, _buffer, offset);
    endToken();
}

//! @brief Appends an absolute memory address formatted as text to the
//! instruction string being accumulated.
//! @param[in] address The address to render as text.
//! @param[in] formatFlags FormatterOptions flags used to determine how to
//! express the address.
void InstructionBuilder::appendAddress(uint32_t address, const FormatterOptions &options)
{
    size_t initialLength = _buffer.size();

    beginToken(FormattedInstruction::TokenType::Label, address);
    options.appendAddressSymbol(address, _buffer);

    if (_buffer.size() == initialLength)
    {
        // No text was written.
        if (options.getFlags() & FormatterOptions::ShowOffsets)
        {
            // Output the branch target as a relative offset.
            int32_t offset = static_cast<int32_t>(address -
                                                  options.getInstructionAddress());

            appendOffset(offset, options.getFlags());
        }
        else
        {
            // Output the branch target as an absolute address.
            beginToken(FormattedInstruction::TokenType::Label, address);
            Ag::FormatInfo numberFormat;
            numberFormat.setRadix(16);
            numberFormat.setMinimumWholeDigits(6);

            if (options.getFlags() & FormatterOptions::UseBasicStyleHex)
            {
                _buffer.push_back('&');
            }
            else
            {
                _buffer.push_back('0');
                _buffer.push_back('x');
            }

            Ag::appendValue(numberFormat, _buffer, address);
            endToken();
        }
    }
    else
    {
        endToken();
    }
}

//! @brief Formats an SWI number, possibly as a name.
//! @param[in] swiNo The SWI number to format.
//! @param[in] options The options used to determine how the number should
//! be expressed as text.
void InstructionBuilder::appendSwiNumber(uint32_t swiNo,
                                         const FormatterOptions &options)
{
    size_t initialLength = _buffer.size();

    options.appendSwiComment(swiNo, _buffer);

    if (_buffer.length() == initialLength)
    {
        // No symbol was written.
        appendComment(swiNo, options.getFlags());
    }
}

//! @brief Appends numeric comment value embedded in an SWI or BKPT instruction.
//! @param[in] value The comment value to render as text.
//! @param[in] formatFlags FormatterOptions flags used to determine how to
//! express the comment value.
void InstructionBuilder::appendComment(uint32_t value, uint32_t formatFlags)
{
    Ag::FormatInfo options;

    beginToken(FormattedInstruction::TokenType::Label, value);

    if ((formatFlags & FormatterOptions::UseDecimalComments) == 0)
    {
        options.setRadix(16);
        options.setMinimumWholeDigits(2);

        if (formatFlags & FormatterOptions::UseBasicStyleHex)
        {
            _buffer.push_back('&');
        }
        else
        {
            _buffer.push_back('0');
            _buffer.push_back('x');
        }
    }

    Ag::appendValue(options, _buffer, value);
    endToken();
}

//! @brief Appends a single character to the string being built based
//! on the value of a flag.
//! @param[in] suffix The suffix character to optionally append.
//! @param[in] hasSuffix Whether the suffix should be applied.
void InstructionBuilder::appendSuffix(char suffix, bool hasSuffix)
{
    if (hasSuffix)
    {
        _buffer.push_back(suffix);
    }
}

//! @brief Appends a list of core registers as a single token.
//! @param[in] regMask The mask defining the registers in the list.
//! @param[in] formatFlags Formatting flags defining how registers
//! should be formatted.
void InstructionBuilder::appendRegisterList(uint16_t regMask,
                                            uint32_t formatFlags)
{
    size_t tokenCount = _tokens.size();
    size_t tokenStart = _buffer.size();

    _buffer.push_back('{');

    int32_t firstReg = 16;
    int32_t lastReg = 16;
    int32_t nextReg = 0;
    uint32_t regSet = regMask;
    bool isFirst = true;

    while (regSet != 0)
    {
        // Get the next register in the set.
        Ag::Bin::bitScanForward(regSet, firstReg);
        lastReg = firstReg;
        regSet ^= (1u << firstReg);

        // Find the last register in the run.
        while (Ag::Bin::bitScanForward(regSet, nextReg) &&
               (nextReg == (lastReg + 1)))
        {
            lastReg = nextReg;
            regSet ^= 1u << nextReg;
        }

        if (isFirst)
        {
            isFirst = false;
        }
        else
        {
            _buffer.push_back(',');
        }

        if (firstReg == lastReg)
        {
            append(Ag::fromScalar<CoreRegister>(static_cast<uint8_t>(firstReg)),
                   formatFlags);
        }
        else
        {
            append(Ag::fromScalar<CoreRegister>(static_cast<uint8_t>(firstReg)),
                   formatFlags);
            _buffer.push_back('-');
            append(Ag::fromScalar<CoreRegister>(static_cast<uint8_t>(lastReg)),
                   formatFlags);
        }
    }

    _buffer.push_back('}');

    // Remove the erroneously added register tokens.
    _tokens.erase(_tokens.begin() + tokenCount, _tokens.end());

    // Add the register list token.
    _tokens.emplace_back(tokenStart, _buffer.size() - tokenStart,
                         FormattedInstruction::TokenType::CoreRegList,
                         regMask);
}

//! @brief Appends an immediate constant token formatted as hexadecimal.
//! @param[in] value The value to format.
//! @param[in] minDigits The minimum number of hex digits to output.
//! @param[in] options The options used to determine how values should be
//! formatted.
void InstructionBuilder::appendHexImmediate(uint32_t value, uint8_t minDigits,
                                            const FormatterOptions &options)
{
    beginToken(FormattedInstruction::TokenType::ImmediateConstant, value);

    _buffer.push_back('#');
    
    if (options.getFlags() & FormatterOptions::UseBasicStyleHex)
    {
        _buffer.push_back('&');
    }
    else
    {
        _buffer.push_back('0');
        _buffer.push_back('x');
    }

    appendHexValue(value, minDigits);
    endToken();
}

//! @brief Appends an integer constant formatted as hexadecimal.
//! @param[in] value The value to format.
//! @param[in] minDigits The minimum number of hex digits to output.
//! @note No token is began or ended by this function.
void InstructionBuilder::appendHexValue(uint32_t value, uint8_t minDigits)
{
    Ag::FormatInfo fmt = Ag::FormatInfo::getNeutral();
    fmt.setRadix(16);
    fmt.setMinimumWholeDigits(minDigits);

    Ag::appendValue(fmt, _buffer, value);
}

//! @brief Appends the identifier of a core register to the string
//! begin accumulated.
//! @param[in] reg The identifier of the register to append.
//! @param[in] formatFlags FormatterOptions flags used to determine how to
//! express the register name.
//! @param[in] negated Indicates whether the register should be proceeded
//! by a minus sign.
void InstructionBuilder::append(CoreRegister reg, uint32_t formatFlags,
                                bool negated /*= false*/)
{
    static char const *basicIds[] = {
        "R0", "R1", "R2", "R3",
        "R4", "R5", "R6", "R7",
        "R8", "R9", "R10", "R11",
        "R12", "R13", "R14", "R15",
        "CPSR", "SPSR"
    };

    static char const *apcsIds[] = {
        "a1", "a2", "a3", "a4",
        "v1", "v2", "v3", "v4",
        "v5", "v6", "sl", "fp",
        "ip", "sp", "lk", "pc",
        "cpsr", "spsr"
    };

    static char const *aliasIds[] = {
        "R0", "R1", "R2", "R3",
        "R4", "R5", "R6", "R7",
        "R8", "R9", "R10", "R11",
        "R12", "SP", "Link", "PC",
        "CPSR", "SPSR"
    };

    const char **ids = basicIds;

    if (formatFlags & FormatterOptions::UseAPCSRegAliases)
    {
        ids = apcsIds;
    }
    else if (formatFlags & FormatterOptions::UseCoreRegAliases)
    {
        ids = aliasIds;
    }

    beginToken(FormattedInstruction::TokenType::CoreRegister,
               Ag::toScalar(reg));

    if (negated)
    {
        _buffer.push_back('-');
    }

    _buffer.append(ids[Ag::toScalar(reg)]);
    endToken();
}

//! @brief Appends the identifier of a shift operation to the string
//! begin accumulated.
//! @param[in] shift The shift type to append.
void InstructionBuilder::append(ShiftType shift)
{
    const char *types[] = {
        "LSL", "LSR",
        "ASR", "ROR",
        "RRX",
    };

    if (shift < ShiftType::None)
    {
        beginToken(FormattedInstruction::TokenType::Shift,
                   Ag::toScalar(shift));
        _buffer.append(types[Ag::toScalar(shift)]);
        endToken();
    }
}

//! @brief Appends the identifier of a co-processor to the string
//! begin accumulated.
//! @param[in] cpId The identifier of the co-processor to format.
void InstructionBuilder::append(CoProcId cpId)
{
    beginToken(FormattedInstruction::TokenType::CoProcessorID,
               Ag::toScalar(cpId));

    _buffer.append("CP");
    Ag::appendValue(Ag::FormatInfo::getNeutral(), _buffer, Ag::toScalar(cpId));
    endToken();
}

//! @brief Appends the identifier of a co-processor register to the string
//! begin accumulated.
//! @param[in] reg The identifier of the register to format.
void InstructionBuilder::append(CoProcRegister reg)
{
    beginToken(FormattedInstruction::TokenType::CoProcessorRegister,
               Ag::toScalar(reg));
    _buffer.append("CR");
    Ag::appendValue(Ag::FormatInfo::getNeutral(), _buffer, Ag::toScalar(reg));
    endToken();
}

//! @brief Appends a single character to the accumulated text.
//! @param[in] ch The character to append.
void InstructionBuilder::append(char ch)
{
    _buffer.push_back(ch);
}

//! @brief Appends a signed integer value to the accumulated text.
//! @param[in] value The integer value to append.
void InstructionBuilder::append(int32_t value)
{
    Ag::appendValue(Ag::FormatInfo::getNeutral(),
                    _buffer, value);
}

//! @brief Appends an unsigned integer value to the accumulated text.
//! @param[in] value The integer value to append.
void InstructionBuilder::append(uint32_t value)
{
    Ag::appendValue(Ag::FormatInfo::getNeutral(),
                    _buffer, value);

}

//! @brief Appends a null-terminated array of UTF-8 encoded bytes to the 
//! accumulated text.
//! @param[in] text The characters to append.
void InstructionBuilder::append(Ag::utf8_cptr_t text)
{
    _buffer.append(text);
}

////////////////////////////////////////////////////////////////////////////////
// InstructionInfo Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a reference to an empty token.
FormattedInstruction::Token::Token() :
    Class(TokenType::Max),
    Data(0),
    Start(0),
    Count(0)
{
}

//! @brief Constructs a token reference.
//! @param[in] start The start index of the first byte or the token in the
//! source instruction text.
//! @param[in] count The count of UTF-8 encoded bytes in the token.
//! @param[in] tokenClass The classification of the token.
//! @param[in] data An item of token-specific data.
FormattedInstruction::Token::Token(size_t start, size_t count,
                                   FormattedInstruction::TokenType tokenClass,
                                   uint32_t data) :
    Class(tokenClass),
    Data(data),
    Start(static_cast<uint16_t>(start)),
    Count(static_cast<uint16_t>(count))
{
}

//! @brief Gets whether the token represents an instruction mnemonic of any type.
bool FormattedInstruction::Token::isMnemonic() const
{
    return (Class < TokenType::Separator);
}

//! @brief Constructs an object representing a formatted assembly language
//! instruction.
//! @param[in] source The source text of the instruction.
//! @param[in] tokens A collection which defines which bytes within the UTF-8
//! encoded source text correspond to which tokens.
FormattedInstruction::FormattedInstruction(Ag::string_cref_t source,
                                           FormattedInstruction::TokenCollection &&tokens) :
    _tokens(std::move(tokens)),
    _source(source)
{
}

//! @brief Determines if the object is in an empty state.
//! @retval true The object is in an empty state.
//! @retval false The object defines a tokenised assembly language instruction.
bool FormattedInstruction::isEmpty() const { return _tokens.empty(); }

//! @brief Gets the count of tokens defined in the instruction.
uint32_t FormattedInstruction::getTokenCount() const
{
    return static_cast<uint32_t>(_tokens.size());
}

//! @brief Gets a token definition.
//! @param[in] index The 0-based index of the token to obtain.
//! @throws Ag::IndexOutOfRangeException If index is invalid.
const FormattedInstruction::Token &FormattedInstruction::getToken(uint32_t index) const
{
    if (index < _tokens.size())
    {
        return _tokens[index];
    }

    throw Ag::IndexOutOfRangeException(static_cast<size_t>(index), _tokens.size());
}

//! @brief Gets a view of the UTF-8 encoded bytes of text corresponding to
//! a specified token.
//! @param[in] index The 0-based index of the token to obtain text for.
//! @return A view of the source instruction text corresponding to the desired
//! token, or an empty string view.
std::string_view FormattedInstruction::getTokenText(uint32_t index) const
{
    std::string_view tokenText;

    if (index < _tokens.size())
    {
        const auto &token = _tokens[index];

        tokenText = std::string_view(_source.getUtf8Bytes() + token.Start,
                                     static_cast<size_t>(token.Count));
    }

    return tokenText;
}

//! Gets the token text as an immutable UTF-8 string.
Ag::String FormattedInstruction::getTokenString(uint32_t index) const
{
    return { getTokenText(index) };
}

//! @brief Gets the collection of objects which defines the tokens in the
//! formatted assembly language instruction.
const FormattedInstruction::TokenCollection &FormattedInstruction::getTokens() const
{
    return _tokens;
}

//! @brief Gets the entire instruction as a single line of text.
Ag::string_cref_t FormattedInstruction::getSourceText() const
{
    return _source;
}

////////////////////////////////////////////////////////////////////////////////
// FormatParams Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing the parameters required to format
//! a 32-bit ARM instruction as text, and to receive the result.
//! @param[in] options An object defining the options required for formatting.
//! @param[in] params The operation-specific parameters of the instruction.
//! @param[in] opClass The class of operation being formatted.
//! @param[in] mnemonic The mnemonic of the instruction to format.
//! @param[in] condition The condition code of the instruction to format.
//! @param[in] isFormatting A flag indicating whether the location of tokens
//! should be tracked in the instruction string created.
FormatParams::FormatParams(const FormatterOptions *options,
                           const InstructionParams *params,
                           OperationClass opClass,
                           InstructionMnemonic mnemonic,
                           ConditionCode condition,
                           bool isFormatting) :
    Options(options),
    Params(params),
    Builder(isFormatting),
    OpClass(opClass),
    Mnemonic(mnemonic),
    Condition(condition)
{
}

//! @brief Determines if a formatting option flag has been set.
//! @param[in] flag The flag to test for.
//! @retval true The option was enabled.
//! @retval false The option was disabled.
bool FormatParams::hasOption(FormatterOptions::Flags flag) const
{
    return (Options->getFlags() & flag) != 0;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Formats a 32-bit ARM machine code instruction as text.
//! @param[in,out] params The parameters of the instruction to format, how to
//! format it and relieves the result.
//! @retval true The instruction of properly formatted.
//! @retval false The instruction was invalid.
bool formatInstruction(FormatParams &params)
{
    switch (params.OpClass)
    {
    case OperationClass::None:
    default:
        formatNonInstruction(params);
        break;

    case OperationClass::CoreAlu:
        formatCoreAlu(params);
        break;

    case OperationClass::CoreCompare:
        formatCoreComparison(params);
        break;

    case OperationClass::CoreAddress:
        formatCoreAdr(params);
        break;

    case OperationClass::CoreMultiply:
        formatCoreMultiply(params);
        break;

    case OperationClass::CoreDataTransfer:
        formatCoreDataTransfer(params);
        break;

    case OperationClass::CoreMultiTransfer:
        formatCoreMultiTransfer(params);
        break;

    case OperationClass::Branch:
        formatBranch(params);
        break;

    case OperationClass::SoftwareIrq:
        formatSoftwareIrq(params);
        break;

    case OperationClass::Breakpoint:
        formatBreakPoint(params);
        break;

    case OperationClass::AtomicSwap:
        formatAtomicSwap(params);
        break;

    case OperationClass::MoveFromPSR:
        formatMoveFromPSR(params);
        break;

    case OperationClass::MoveToPSR:
        formatMoveToPsr(params);
        break;

    case OperationClass::BranchExchange: break;

    case OperationClass::LongMultiply:
        formatLongMultiply(params);
        break;

    case OperationClass::CoProcDataProcessing:
        formatCoProcDataOp(params);
        break;

    case OperationClass::CoProcRegisterTransfer:
        formatCoProcRegisterTransfer(params);
        break;

    case OperationClass::CoProcDataTransfer:
        formatCoProcDataTransfer(params);
        break;

    case OperationClass::FpaDataTransfer: break;
    case OperationClass::FpaMultiTransfer: break;
    case OperationClass::FpaDyadicOperation: break;
    case OperationClass::FpaMonadicOperation: break;
    case OperationClass::FpaRegisterTransfer: break;
    case OperationClass::FpaComparison: break;
        break;
    }

    return params.Builder.isEmpty() == false;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


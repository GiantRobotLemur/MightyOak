//! @file FormatInstruction.cpp
//! @brief The definition of an entry point to a function which will format a
//! 32-bit ARM machine code instruction as text.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Binary.hpp"
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
//! @brief Formats the parameters of a barrel shifter operand in the instruction
//! text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
//! @param[in] shifter The barrel shifter operand to format.
//! @param[in] isNegated True if the shifter operand should be proceeded by a
//! minus sign, either inside the constant or before the register.
void formatShifterOperand(FormatParams &params, const ShifterOperand &shifter,
                          bool isNegated = false)
{
    switch (shifter.Mode)
    {
    case ShifterMode::ImmediateConstant:
        params.Builder.push_back('#');

        if (isNegated)
        {
            Ag::appendValue(Ag::FormatInfo::getNeutral(), params.Builder,
                            static_cast<int32_t>(shifter.Immediate) * -1);
        }
        else
        {
            Ag::appendValue(Ag::FormatInfo::getNeutral(), params.Builder,
                            shifter.Immediate);
        }
        break;

    case ShifterMode::Register:
        params.appendSuffix('-', isNegated);
        params.append(shifter.Rm);
        break;

    case ShifterMode::ShiftByRegister:
        params.appendSuffix('-', isNegated);
        params.append(shifter.Rm);
        params.appendSeparator();
        params.append(shifter.Shift);
        params.Builder.push_back(' ');
        params.append(shifter.Rs);
        break;

    case ShifterMode::ShiftByConstant:
        params.appendSuffix('-', isNegated);
        params.append(shifter.Rm);
        params.appendSeparator();
        params.append(shifter.Shift);
        params.Builder.push_back(' ');
        params.Builder.push_back('#');
        Ag::appendValue(Ag::FormatInfo::getNeutral(),
                        params.Builder, shifter.Immediate);
        break;

    case ShifterMode::RotateWithExtend:
        params.appendSuffix('-', isNegated);
        params.append(shifter.Rm);
        params.Builder.append(", RRX");
        break;
    }
}

//! @brief Formats an ALU data processing instruction text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreAlu(FormatParams &params)
{
    const auto &info = params.Params->CoreAluOp;

    params.appendMnemonic();
    params.appendConditionCode();
    params.appendSuffix('S', info.AffectsFlags);
    params.Builder.push_back(' ');
    params.append(info.Rd);
    params.appendSeparator();

    if ((params.Mnemonic != InstructionMnemonic::Mov) &&
        (params.Mnemonic != InstructionMnemonic::Mvn))
    {
        params.append(info.Rn);
        params.appendSeparator();
    }

    formatShifterOperand(params, info.Op2);
}

//! @brief Formats an ALU comparison instruction text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreComparison(FormatParams &params)
{
    const auto &info = params.Params->CoreCmpOp;

    params.appendMnemonic();
    params.appendConditionCode();
    params.appendSuffix('P', info.OverwritesPSR);
    params.Builder.push_back(' ');
    params.append(info.Rn);
    params.appendSeparator();

    formatShifterOperand(params, info.Op2);
}


//! @brief Formats an address operand into in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatAddressOperand(FormatParams &params, const AddrOperand &addr)
{
    // There is an offset.
    if (addr.Flags & AddrOperand::PreIndexed)
    {
        if ((addr.Offset.Mode == ShifterMode::ImmediateConstant) &&
            (addr.Offset.Immediate == 0))
        {
            // There is no offset.
            params.Builder.push_back('[');
            params.append(addr.Rn);
            params.Builder.push_back(']');

            // Check for erroneous write-back?
        }
        else if ((addr.Rn == CoreRegister::R15) &&
            (addr.Offset.Mode == ShifterMode::ImmediateConstant))
        {
            // It's a PC-relative address.
            int32_t offset = static_cast<int32_t>(addr.Offset.Immediate);
            offset -= 8;

            if (addr.Flags & AddrOperand::NegativeOffset)
            {
                offset *= -1;
            }

            params.appendOffset(offset);
        }
        else
        {
            // Pre-indexed with offset.
            params.Builder.push_back('[');
            params.append(addr.Rn);
            params.appendSeparator();

            formatShifterOperand(params, addr.Offset,
                                 addr.Flags & AddrOperand::NegativeOffset);

            params.Builder.push_back(']');
            params.appendSuffix('!', addr.Flags & AddrOperand::Writeback);
        }
    }
    else
    {
        // Post-indexed addressing mode.
        params.Builder.push_back('[');
        params.append(addr.Rn);
        params.Builder.push_back(']');
        params.appendSeparator();
        formatShifterOperand(params, addr.Offset,
                             addr.Flags & AddrOperand::NegativeOffset);
    }
}

//! @brief Formats a core data transfer instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatCoreDataTransfer(FormatParams &params)
{
    const auto &info = params.Params->DataTransOp;

    params.appendMnemonic();
    params.appendConditionCode();

    switch (info.DataType)
    {
    default:
    case TransferDataType::Word:
        break;

    case TransferDataType::SignedByte: params.Builder.append("SB", 2); break;
    case TransferDataType::UnsignedByte:  params.Builder.append("B", 1); break;
    case TransferDataType::SignedHalfWord:  params.Builder.append("SH", 2); break;
    case TransferDataType::UnsignedHalfWord:  params.Builder.append("H", 1); break;
    }

    params.appendSuffix('T', info.UserPrivilages);
    params.Builder.push_back(' ');
    params.append(info.Rd);
    params.appendSeparator();

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

    params.appendMnemonic();
    params.appendConditionCode();

    if (info.Mode < MultiTransferMode::Max)
    {
        params.Builder.append(modeText[Ag::toScalar(info.Mode)], 2);
    }

    params.Builder.push_back(' ');
    params.append(info.Rd);
    params.appendSuffix('!', info.Writeback);
    params.appendSeparator();
    params.Builder.push_back('{');
    uint32_t regSet = info.Registers;

    int32_t firstReg = 16;
    int32_t lastReg = 16;
    int32_t nextReg = 0;
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
            params.appendSeparator();
        }

        if (firstReg == lastReg)
        {
            params.append(Ag::fromScalar<CoreRegister>(static_cast<uint8_t>(firstReg)));
        }
        else
        {
            params.append(Ag::fromScalar<CoreRegister>(static_cast<uint8_t>(firstReg)));
            params.Builder.push_back('-');
            params.append(Ag::fromScalar<CoreRegister>(static_cast<uint8_t>(lastReg)));
        }
    }

    params.Builder.push_back('}');
    params.appendSuffix('^', info.UserModeRegs);
}

//! @brief Formats an atomic swap instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatAtomicSwap(FormatParams &params)
{
    const auto &info = params.Params->AtomicSwapOp;

    params.appendMnemonic();
    params.appendConditionCode();
    params.appendSuffix('B', info.IsByte);

    params.Builder.push_back(' ');
    params.append(info.Rd);
    params.appendSeparator();
    params.append(info.Rm);
    params.appendSeparator();
    params.Builder.push_back('[');
    params.append(info.Rn);
    params.Builder.push_back(']');
}

//! @brief Formats an MRS instruction in the text being accumulated.
//! @param[in,out] params The parameters to format and the string being
//! accumulated.
void formatMoveToPsr(FormatParams &params)
{
    const auto &info = params.Params->MoveToPsrOp;

    params.appendMnemonic();
    params.appendConditionCode();
    params.Builder.push_back(' ');
    params.append(info.IsCPSR ? CoreRegister::CPSR : CoreRegister::SPSR);

    if (info.PsrComponents != Ag::toScalar(PsrComponent::All))
    {
        params.Builder.push_back('_');

        params.appendSuffix('C', info.PsrComponents & Ag::toScalar(PsrComponent::Control));
        params.appendSuffix('X', info.PsrComponents & Ag::toScalar(PsrComponent::Extension));
        params.appendSuffix('S', info.PsrComponents & Ag::toScalar(PsrComponent::Status));
        params.appendSuffix('F', info.PsrComponents & Ag::toScalar(PsrComponent::Flags));
    }

    params.appendSeparator();

    if (info.IsSourceReg)
    {
        params.append(info.SourceReg);
    }
    else
    {
        params.Builder.push_back('#');
        params.Builder.push_back('&');

        Ag::FormatInfo fmt = Ag::FormatInfo::getNeutral();
        fmt.setRadix(16);
        fmt.setMinimumWholeDigits(2);

        Ag::appendValue(fmt, params.Builder, info.SourceImmediate);
    }
}

} // Anonymous namespace

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
FormatParams::FormatParams(const FormatterOptions *options,
                           const InstructionParams *params,
                           OperationClass opClass,
                           InstructionMnemonic mnemonic,
                           ConditionCode condition) :
    Options(options),
    Params(params),
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

//! @brief Appends a separator followed by a space to the string being built.
void FormatParams::appendSeparator()
{
    Builder.push_back(',');
    Builder.push_back(' ');
}

//! @brief Appends the text of the instruction mnemonic to the instruction string
//! being created.
void FormatParams::appendMnemonic()
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

    if (Mnemonic < InstructionMnemonic::MaxMnemonic)
    {
        Builder.append(names[Ag::toScalar(Mnemonic)]);
    }
    else
    {
        Builder.append("(invalid instruction)");
    }
}


//! @brief Appends the suffix representing a condition code to the instruction
//! string being accumulated.
void FormatParams::appendConditionCode()
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
    if ((Condition != ConditionCode::Al) &&
        (Condition < ConditionCode::Max))
    {
        Builder.append(codes[Ag::toScalar(Condition)], 2u);
    }
}

//! @brief Appends a memory offset formatted as text to a string.
//! @param[in] offset The offset to format.
void FormatParams::appendOffset(int32_t offset)
{
    Ag::FormatInfo options;

    // Express the offset as relative to the current address, denoted by '$'.
    Builder.push_back('$');
    Builder.push_back(' ');

    if (offset < 0)
    {
        offset = -offset;
        Builder.push_back('-');
    }
    else // if (offset >= 0)
    {
        Builder.push_back('+');
    }

    Builder.push_back(' ');

    if (hasOption(FormatterOptions::UseDecimalOffsets) == false)
    {
        // Show the hex specifier.
        Builder.push_back('&');
        options.setRadix(16);
        options.setMinimumWholeDigits(2);
    }

    Ag::appendValue(options, Builder, offset);
}

//! @brief Appends an absolute memory address formatted as text to the
//! instruction string being accumulated.
//! @param[in] address The address to render as text.
void FormatParams::appendAddress(uint32_t address)
{
    size_t initialLength = Builder.size();

    Options->appendAddressSymbol(Params->BranchOp.Address,
                                 Builder);

    if (Builder.size() == initialLength)
    {
        // No text was written.
        if (hasOption(FormatterOptions::ShowOffsets))
        {
            // Output the branch target as a relative offset.
            int32_t offset = static_cast<int32_t>(Params->BranchOp.Address -
                                                  Options->getInstructionAddress());

            appendOffset(offset);
        }
        else
        {
            // Output the branch target as an absolute address.
            Ag::FormatInfo options;
            options.setRadix(16);
            options.setMinimumWholeDigits(6);

            Builder.push_back('&');
            Ag::appendValue(options, Builder, address);
        }
    }
}

//! @brief Appends numeric comment value embedded in an SWI or BKPT instruction.
//! @param[in] value The comment value to render as text.
void FormatParams::appendComment(uint32_t value)
{
    Ag::FormatInfo options;

    if (hasOption(FormatterOptions::UseDecimalComments) == false)
    {
        options.setRadix(16);
        options.setMinimumWholeDigits(2);

        Builder.push_back('&');
    }

    Ag::appendValue(options, Builder, value);
}

//! @brief Appends a single character to the string being built based
//! on the value of a flag.
//! @param[in] suffix The suffix character to optionally append.
//! @param hasSuffix Whether the suffix should be applied.
void FormatParams::appendSuffix(char suffix, bool hasSuffix)
{
    if (hasSuffix)
    {
        Builder.push_back(suffix);
    }
}

//! @brief Appends the identifier of a core register to the string
//! begin accumulated.
//! @param[in] reg The identifier of the register to append.
void FormatParams::append(CoreRegister reg)
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

    if (hasOption(FormatterOptions::UseAPCSRegAliases))
    {
        ids = apcsIds;
    }
    else if (hasOption(FormatterOptions::UseCoreRegAliases))
    {
        ids = aliasIds;
    }

    Builder.append(ids[Ag::toScalar(reg)]);
}

//! @brief Appends the identifier of a shift operation to the string
//! begin accumulated.
//! @param[in] shift The shift type to append.
void FormatParams::append(ShiftType shift)
{
    const char *types[] = {
        "LSL", "LSR",
        "ASR", "ROR",
        "RRX",
    };

    if (shift < ShiftType::None)
    {
        Builder.append(types[Ag::toScalar(shift)]);
    }
}

//! @brief Appends the identifier of a co-processor to the string
//! begin accumulated.
//! @param[in] cpId The identifier of the co-processor to format.
void FormatParams::append(CoProcId cpId)
{
    Builder.append("CP");
    Ag::appendValue(Ag::FormatInfo::getNeutral(), Builder, Ag::toScalar(cpId));
}

//! @brief Appends the identifier of a co-processor register to the string
//! begin accumulated.
//! @param[in] reg The identifier of the register to format.
void FormatParams::append(CoProcRegister reg)
{
    Builder.append("CR");
    Ag::appendValue(Ag::FormatInfo::getNeutral(), Builder, Ag::toScalar(reg));
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
    params.Builder.clear();
    size_t initialLength = 0;

    switch (params.OpClass)
    {
    case OperationClass::CoreAlu:
        formatCoreAlu(params);
        break;

    case OperationClass::CoreCompare:
        formatCoreComparison(params);
        break;

    case OperationClass::CoreAddress: {
        const auto &info = params.Params->CoreAdr;

        params.appendMnemonic();
        params.appendConditionCode();
        params.Builder.push_back(' ');
        params.append(info.Rd);
        params.appendSeparator();
        params.appendAddress(info.Address);
    } break;

    case OperationClass::CoreMultiply: {
        const auto &info = params.Params->CoreMulOp;

        params.appendMnemonic();
        params.appendConditionCode();
        params.appendSuffix('S', info.AffectsFlags);
        params.Builder.push_back(' ');
        params.append(info.Rd);
        params.appendSeparator();
        params.append(info.Rm);
        params.appendSeparator();
        params.append(info.Rs);
        if (params.Mnemonic == InstructionMnemonic::Mla)
        {
            params.appendSeparator();
            params.append(info.Rn);
        }
    } break;

    case OperationClass::CoreDataTransfer:
        formatCoreDataTransfer(params);
        break;

    case OperationClass::CoreMultiTransfer:
        formatCoreMultiTransfer(params);
        break;

    case OperationClass::Branch:
        params.appendMnemonic();
        params.appendConditionCode();
        params.Builder.push_back(' ');
        params.appendAddress(params.Params->BranchOp.Address);
        break;

    case OperationClass::SoftwareIrq:
        params.appendMnemonic();
        params.appendConditionCode();
        params.Builder.push_back(' ');
        initialLength = params.Builder.size();

        params.Options->appendSwiComment(params.Params->SoftwareIrqOp.Comment,
                                         params.Builder);

        if (params.Builder.length() == initialLength)
        {
            // No symbol was written.
            params.appendComment(params.Params->SoftwareIrqOp.Comment);
        }
        break;

    case OperationClass::Breakpoint:
        params.appendMnemonic();
        params.Builder.push_back(' ');
        initialLength = params.Builder.size();

        // Write a raw value for the comment field.
        params.appendComment(params.Params->BreakpointOp.Comment);
        break;

    case OperationClass::AtomicSwap:
        formatAtomicSwap(params);
        break;

    case OperationClass::MoveFromPSR:
        params.appendMnemonic();
        params.appendConditionCode();
        params.Builder.push_back(' ');
        params.append(params.Params->MoveFromPsrOp.Rd);
        params.appendSeparator();
        params.append(params.Params->MoveFromPsrOp.IsCPSR ? CoreRegister::CPSR : CoreRegister::SPSR);
        break;

    case OperationClass::MoveToPSR:
        formatMoveToPsr(params);
        break;

    case OperationClass::BranchExchange: break;

    case OperationClass::LongMultiply: {
        const auto &info = params.Params->LongMulOp;
        params.appendMnemonic();
        params.appendConditionCode();
        params.appendSuffix('S', info.AffectsFlags);
        params.Builder.push_back(' ');
        params.append(info.RdLo);
        params.appendSeparator();
        params.append(info.RdHi);
        params.appendSeparator();
        params.append(info.Rm);
        params.appendSeparator();
        params.append(info.Rs);
    } break;

    case OperationClass::CoProcDataProcessing: {
        const auto &info = params.Params->CoProcDataProcOp;
        Ag::FormatInfo fmt = Ag::FormatInfo::getNeutral();

        params.appendMnemonic();
        params.appendConditionCode();
        params.Builder.push_back(' ');
        params.append(info.CoProcessor);
        params.appendSeparator();
        Ag::appendValue(fmt, params.Builder, info.OpCode1);
        params.appendSeparator();
        params.append(info.Rd);
        params.appendSeparator();
        params.append(info.Rn);
        params.appendSeparator();
        params.append(info.Rm);
        params.appendSeparator();
        appendValue(fmt, params.Builder, info.OpCode2);
    } break;

    case OperationClass::CoProcRegisterTransfer: {
        const auto &info = params.Params->CoProcRegTransOp;
        Ag::FormatInfo fmt = Ag::FormatInfo::getNeutral();

        params.appendMnemonic();
        params.appendConditionCode();
        params.Builder.push_back(' ');
        params.append(info.CoProcessor);
        params.appendSeparator();
        Ag::appendValue(fmt, params.Builder, info.OpCode1);
        params.appendSeparator();
        params.append(info.Rd);
        params.appendSeparator();
        params.append(info.Rn);
        params.appendSeparator();
        params.append(info.Rm);
        params.appendSeparator();
        appendValue(fmt, params.Builder, info.OpCode2);
    } break;

    case OperationClass::CoProcDataTransfer: {
        const auto &info = params.Params->CoProcDataTransferOp;
        params.appendMnemonic();
        params.appendConditionCode();
        params.appendSuffix('L', info.IsLong);
        params.Builder.push_back(' ');
        params.append(info.CoProcessor);
        params.appendSeparator();
        params.append(info.Rd);
        params.appendSeparator();
        formatAddressOperand(params, info.Addr);
    } break;

    case OperationClass::FpaDataTransfer: break;
    case OperationClass::FpaMultiTransfer: break;
    case OperationClass::FpaDyadicOperation: break;
    case OperationClass::FpaMonadicOperation: break;
    case OperationClass::FpaRegisterTransfer: break;
    case OperationClass::FpaComparison: break;

    case OperationClass::None:
    default:
        break;
    }

    return params.Builder.empty() == false;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


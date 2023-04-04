//! @file Assembly.cpp
//! @brief The definition of an entry point to assemble a 32-bit ARM machine
//! code instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Variant.hpp"

#include "AsmTools/InstructionInfo.hpp"

#include "Assembly.hpp"

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
//! @brief Encodes a condition in an instruction bit field.
//! @param[in] condition The condition code to encode.
//! @return A mask defining the condition code as it would appear in an ARM
//! machine code instruction.
uint32_t encodeCondition(ConditionCode condition)
{
    uint32_t encoding;

    switch (condition)
    {
    case ConditionCode::Hs:
        encoding = static_cast<uint32_t>(toScalar(ConditionCode::Cs)) << 28;
        break;

    case ConditionCode::Lo:
        encoding = static_cast<uint32_t>(toScalar(ConditionCode::Cc)) << 28;
        break;

    default:
        encoding = static_cast<uint32_t>(toScalar(condition)) << 28;
        break;
    }

    return encoding;
}

//! @brief Attempts to encode an immediate constant in an barrel shifter
//! operand.
//! @param[in,out] coding The instruction encoding to update.
//! @param[in] constant The immediate constant to encode.
//! @retval true The constant was encoded.
//! @retval false The constant could not be encoded.
bool tryEncodeImmediateConstant(uint32_t &coding, uint32_t constant)
{
    bool isEncoded = false;

    if (constant & ~0xFF)
    {
        // The constant cannot be encoded without a shift of some kind.
        for (int32_t shift = 0; (isEncoded == false) && (shift < 32); shift += 2)
        {
            uint32_t rotated = Bin::rotateLeft(constant, shift);

            if ((rotated & 0xFF) == rotated)
            {
                // The shift should be even, so the bottom bit is always clear.
                coding |= static_cast<uint32_t>(shift) << 7;
                coding |= rotated;
                isEncoded = true;
            }
        }
    }
    else
    {
        // The constant doesn't need to be shifted.
        coding |= constant;
        isEncoded = true;
    }

    return isEncoded;
}

//! @brief Creates an encodable shifter operand from a more complex
//! operand description.
//! @param[in] original The original shifter operand specification.
//! @param[out] encodable An operand specification which can be directly
//! assembled into machine code.
void createEncodableShifterOperand(const ShifterOperand &original,
                                   ShifterOperand &encodable)
{
    // Copy the original operand specification.
    encodable = original;

    switch (original.Mode)
    {
    case ShifterMode::Register:
        // <Rm> => <Rm>, LSL #0.
        encodable.Mode = ShifterMode::ShiftByConstant;
        encodable.Shift = ShiftType::Lsl;
        encodable.Immediate = 0;
        break;

    case ShifterMode::ShiftByConstant:
        // <Rm>, LSR/ASR #32 => <Rm>, LSR/ASR #0
        if ((original.Immediate == 32) &&
            ((original.Shift == ShiftType::Lsr) ||
             (original.Shift == ShiftType::Asr)))
        {
            encodable.Immediate = 0;
        }
        break;

    case ShifterMode::RotateWithExtend:
        // <Rm>, RRX => <Rm>, ROR #0
        encodable.Mode = ShifterMode::ShiftByConstant;
        encodable.Shift = ShiftType::Ror;
        encodable.Immediate = 0;
        break;
    }
}

//! @brief Assembles a core ALU data processing or comparison operation.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeCoreAlu(AssemblyParams &params)
{
    const ShifterOperand *operand2 = nullptr;
    params.encodeBits(toScalar(params.Mnemonic), 4, 21);

    if ((params.Mnemonic == InstructionMnemonic::Cmp) ||
        (params.Mnemonic == InstructionMnemonic::Cmn) ||
        (params.Mnemonic == InstructionMnemonic::Tst) ||
        (params.Mnemonic == InstructionMnemonic::Teq))
    {
        const auto &info = params.Params->CoreCmpOp;
        operand2 = &info.Op2;

        if (info.OverwritesPSR)
        {
            // Set the invisible Rd to R15.
            params.encodeCoreRegister(CoreRegister::R15, 12);
        }

        // The 'S' suffix is implicit in these instructions.
        params.encodeBit(true, 20);
        params.encodeCoreRegister(info.Rn, 16);
    }
    else
    {
        const auto &info = params.Params->CoreAluOp;
        operand2 = &info.Op2;

        params.encodeBit(info.AffectsFlags, 20);
        params.encodeCoreRegister(info.Rd, 12);
        params.encodeCoreRegister(info.Rn, 16);
    }

    bool isOK = false;

    if (operand2 != nullptr)
    {
        isOK = true;
        ShifterOperand shifter;
        uint8_t shiftValue = 0;

        // Ensure the operand is suitable for encoding.
        createEncodableShifterOperand(*operand2, shifter);

        // Encode the barrel shifter operand.
        switch (shifter.Mode)
        {
        case ShifterMode::ImmediateConstant:
            params.encodeBit(true, 25);
            if (tryEncodeImmediateConstant(params.Instructions[params.InstructionCount],
                                           shifter.Immediate) == false)
            {
                isOK = false;
                FormatInfo formatInfo(LocaleInfo::getNeutral());

                params.ErrorMessage =
                    String::format(formatInfo,
                                   "Cannot encode &{0:X} as an immediate constant.",
                                   { shifter.Immediate });
            }
            break;

        case ShifterMode::Register:
            params.encodeCoreRegister(shifter.Rm, 0);
            break;

        case ShifterMode::ShiftByRegister:
            params.encodeBit(true, 4);
            params.encodeShiftType(shifter.Shift);
            params.encodeCoreRegister(shifter.Rm, 0);
            params.encodeCoreRegister(shifter.Rs, 8);
            break;

        case ShifterMode::ShiftByConstant:
            switch (shifter.Shift)
            {
            case ShiftType::Lsl:
                if ((shifter.Immediate >= 0) && (shifter.Immediate < 32))
                {
                    shiftValue = static_cast<uint8_t>(shifter.Immediate);
                }
                else
                {
                    params.ErrorMessage = String::format("An immediate value of {0} cannot be "
                                                         "encode as a logical left shift.",
                                                         { shifter.Immediate });
                    isOK = false;
                }
                break;

            case ShiftType::Lsr:
                if (shifter.Immediate == 32)
                {
                    // LSR #32 is encoded as LSR #0.
                    shiftValue = 0;
                }
                else if ((shifter.Immediate >= 0) && (shifter.Immediate < 32))
                {
                    shiftValue = static_cast<uint8_t>(shifter.Immediate);
                }
                else
                {
                    params.ErrorMessage = String::format("An immediate value of {0} cannot be "
                                                         "encode as a logical right shift.",
                                                         { shifter.Immediate });
                    isOK = false;
                }
                break;

            case ShiftType::Asr:
                if (shifter.Immediate == 32)
                {
                    // ASR #32 is encoded as ASR #0.
                    shiftValue = 0;
                }
                else if ((shifter.Immediate >= 0) && (shifter.Immediate < 32))
                {
                    shiftValue = static_cast<uint8_t>(shifter.Immediate);
                }
                else
                {
                    params.ErrorMessage = String::format("An immediate value of {0} cannot be "
                                                         "encode as a arithmetic right shift.",
                                                         { shifter.Immediate });
                    isOK = false;
                }
                break;

            case ShiftType::Ror:
                if ((shifter.Immediate >= 0) && (shifter.Immediate < 32))
                {
                    shiftValue = static_cast<uint8_t>(shifter.Immediate);
                }
                else
                {
                    params.ErrorMessage = String::format("An immediate value of {0} cannot be "
                                                         "encode as a rotate right shift.",
                                                         { shifter.Immediate });
                    isOK = false;
                }
                break;
            }

            params.encodeBits(shiftValue, 5, 7);
            params.encodeShiftType(shifter.Shift);
            params.encodeCoreRegister(shifter.Rm, 0);
            break;

        case ShifterMode::RotateWithExtend:
            // Encode as <Rm>, ROR #0
            params.encodeShiftType(ShiftType::Ror);
            params.encodeCoreRegister(shifter.Rm, 0);
            break;
        }

    }

    if (isOK)
    {
        params.completeInstruction();
    }
}

//! @brief Assembles a core single-register data transfer operation.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeDataTransfer(AssemblyParams &params)
{
    bool isOK = false;
    const auto &info = params.Params->DataTransOp;

    params.encodeCoreRegister(info.Rd, 12);
    params.encodeCoreRegister(info.Addr.Rn, 16);
    params.encodeBit((info.Addr.Flags & AddrOperand::NegativeOffset) == 0, 23);

    if (info.Addr.Flags & AddrOperand::PreIndexed)
    {
        params.encodeBit(info.Addr.Flags & AddrOperand::Writeback, 21);
        params.encodeBit(24);

        if (info.UserPrivilages)
        {
            params.ErrorMessage = "Cannot apply user mode privilege 'T' "
                                  "in a pre-indexed addressing mode.";
            isOK = false;
        }
    }
    else
    {
        params.encodeBit(info.UserPrivilages, 21);

        if (info.Addr.Flags & AddrOperand::Writeback)
        {
            params.ErrorMessage = "Cannot apply write-back "
                                  "in a post-indexed addressing mode.";
            isOK = false;
        }
    }

    if ((info.DataType == TransferDataType::UnsignedByte) ||
        (info.DataType == TransferDataType::Word))
    {
        // Set load or store.
        isOK = true;
        params.encodeBits(1, 2, 26);
        params.encodeBit(params.Mnemonic == InstructionMnemonic::Ldr, 20);
        params.encodeBit(info.DataType == TransferDataType::UnsignedByte, 22);

        // Ensure the shifter operand has an appropriate encoding.
        ShifterOperand encodable;
        createEncodableShifterOperand(info.Addr.Offset, encodable);

        switch (encodable.Mode)
        {
        case ShifterMode::ImmediateConstant:
            if (encodable.Immediate & ~0xFFF)
            {
                params.ErrorMessage = "The offset is too big to encode "
                                      "as an immediate constant.";
                isOK = false;
            }
            else
            {
                // Encode the immediate offset.
                params.encodeBits(encodable.Immediate, 12, 0);
            }
            break;

        case ShifterMode::ShiftByRegister:
        case ShifterMode::RotateWithExtend:
            params.ErrorMessage = "Invalid offset shifter mode.";
            isOK = false;
            break;

        case ShifterMode::ShiftByConstant:
            params.encodeBit(25);
            params.encodeCoreRegister(encodable.Rm, 0);
            params.encodeShiftType(encodable.Shift);
            params.encodeBits(encodable.Immediate, 5, 7);

            if (encodable.Immediate > 31)
            {
                params.ErrorMessage = "Invalid offset shift constant value.";
                isOK = false;
            }
            break;
        }
    }
    else
    {
        // Load/Store Signed byte/half-word - places additional restrictions
        // on the addressing mode.
        params.encodeBits(0x9, 4, 4);

        switch (info.DataType)
        {
        case TransferDataType::SignedByte:
            if (params.Mnemonic == InstructionMnemonic::Str)
            {
                params.ErrorMessage = "The STR instruction is invalid for signed bytes.";
                isOK = false;
            }
            else
            {
                params.encodeBits(2, 2, 5);
                params.encodeBit(20); // LDR
            }
            break;

        case TransferDataType::SignedHalfWord:
            if (params.Mnemonic == InstructionMnemonic::Str)
            {
                params.ErrorMessage = "The STR instruction is invalid for signed half-words.";
                isOK = false;
            }
            else
            {
                params.encodeBits(3, 2, 5);
                params.encodeBit(20); // LDR
            }
            break;

        case TransferDataType::UnsignedHalfWord:
            params.encodeBits(1, 2, 5);
            params.encodeBit(params.Mnemonic == InstructionMnemonic::Ldr, 20);
            break;
        }

        const auto &offset = info.Addr.Offset;

        if (offset.Mode == ShifterMode::ImmediateConstant)
        {
            if (offset.Immediate > 0xFF)
            {
                // The immediate offset is greater than 8-bits.
                params.ErrorMessage =
                    "Signed or half-word data transfers are restricted "
                    "to an 8-bit immediate offset.";
            }
            else
            {
                // Encode the offset in two halves.
                params.encodeBit(22);
                params.encodeBits(offset.Immediate, 4, 0);
                params.encodeBits(offset.Immediate >> 4, 4, 8);
                isOK = true;
            }
        }
        else if (offset.Mode == ShifterMode::Register)
        {
            params.encodeCoreRegister(offset.Rm, 0);
            isOK = true;
        }
        else
        {
            params.ErrorMessage = "Signed or half-word data transfers cannot "
                                  "encode complex shifter offsets.";
        }
    }

    if (isOK)
    {
        params.completeInstruction();
    }
}

//! @brief Assembles a core multi-register data transfer operation.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeMultiDataTransfer(AssemblyParams &params)
{
    static const uint8_t loadModeBits[] = {
        2,  // EmptyAscending
        3,  // EmptyDescending
        0,  // FullAscending
        1,  // FullDescending
        1,  // IncrementAfter
        3,  // IncrementBefore
        0,  // DecrementAfter
        2,  // DecrementBefore
    };

    static const uint8_t storeModeBits[] = {
        1,  // EmptyAscending
        0,  // EmptyDescending
        3,  // FullAscending
        2,  // FullDescending
        1,  // IncrementAfter
        3,  // IncrementBefore
        0,  // DecrementAfter
        2,  // DecrementBefore
    };

    const auto &info = params.Params->MultiTransOp;

    params.encodeBits(4, 3, 25);
    params.encodeCoreRegister(info.Rd, 16);
    params.encodeBit(info.UserModeRegs, 22);
    params.encodeBit(info.Writeback, 21);

    if (params.Mnemonic == InstructionMnemonic::Ldm)
    {
        params.encodeBit(20);
        params.encodeBits(loadModeBits[toScalar(info.Mode)], 2, 23);
    }
    else
    {
        params.encodeBits(storeModeBits[toScalar(info.Mode)], 2, 23);
    }

    // Encode the bits defining the registers to load/store.
    params.encodeBits(info.Registers, 16, 0);

    params.completeInstruction();
}

//! @brief Assembles an effective address pseudo-instruction.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeCoreAddress(AssemblyParams &params)
{
    const auto &info = params.Params->CoreAdr;
    uint32_t pcAddr = params.LoadAddress + 8;
    uint32_t offset = 0;

    if (info.Address >= pcAddr)
    {
        // Encode an add instruction.
        params.encodeBits(0x28, 8, 20);
        offset = info.Address - pcAddr;
    }
    else
    {
        // Encode a sub instruction.
        params.encodeBits(0x24, 8, 20);
        offset = pcAddr - info.Address;
    }

    params.encodeCoreRegister(info.Rd, 12);

    // Add or subtract from the PC register.
    params.encodeCoreRegister(CoreRegister::R15, 16);

    if (tryEncodeImmediateConstant(params.Instructions[params.InstructionCount],
                                   offset))
    {
        params.completeInstruction();
    }
    else
    {
        FormatInfo formatInfo(LocaleInfo::getNeutral());

        params.ErrorMessage =
            String::format(formatInfo,
                           "Cannot encode the offset to address &{0:X} as an "
                           "immediate constant.", { info.Address });
    }
}

//! @brief Assembles an atomic swap instruction.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeCoreSwap(AssemblyParams &params)
{
    const auto &info = params.Params->AtomicSwapOp;

    params.Instructions[params.InstructionCount] |= 0x01000090;

    params.encodeBit(info.IsByte, 22);
    params.encodeCoreRegister(info.Rd, 12);
    params.encodeCoreRegister(info.Rm, 0);
    params.encodeCoreRegister(info.Rn, 16);
    params.completeInstruction();
}

//! @brief Assembles an MRS instruction.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeMoveToPsr(AssemblyParams &params)
{
    const auto &info = params.Params->MoveToPsrOp;
    auto &instruction = params.Instructions[params.InstructionCount];
    instruction |= 0x0120F000;
    params.encodeBit(info.IsCPSR == false, 22);
    params.encodeBits(info.PsrComponents, 4, 16);

    if (info.IsSourceReg)
    {
        params.encodeCoreRegister(info.SourceReg, 0);
        params.completeInstruction();
    }
    else if (tryEncodeImmediateConstant(instruction, info.SourceImmediate))
    {
        params.encodeBit(25);
        params.completeInstruction();
    }
    else
    {
        params.ErrorMessage = String::format(
            "The immediate constant value 0x{0:X8} cannot be encoded.",
            { info.SourceImmediate });
    }
}

//! @brief Assembles a generic co-processor data transfer instruction.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeCoProcDataTransfer(AssemblyParams &params)
{
    const auto &info = params.Params->CoProcDataTransferOp;
    auto &instruction = params.Instructions[params.InstructionCount];
    instruction |= 0x0C000000;

    params.encodeCoProcID(info.CoProcessor, 8);
    params.encodeCoProcRegister(info.Rd, 12);

    if (info.Addr.Offset.Mode != ShifterMode::ImmediateConstant)
    {
        params.ErrorMessage = "Invalid offset operand type.";
        return;
    }

    if (info.Addr.Flags & AddrOperand::PreIndexed)
    {
        params.encodeBit(24);
        params.encodeBit(info.Addr.Flags & AddrOperand::Writeback, 21);
    }
    else if (info.Addr.Flags & AddrOperand::Writeback)
    {
        params.ErrorMessage = "Write-back cannot be specified in a pre-indexed "
                              "addressing mode.";
        return;
    }

    params.encodeBit((info.Addr.Flags & AddrOperand::NegativeOffset) == 0, 23);
    params.encodeBit(info.IsLong, 22);
    params.encodeBit(params.Mnemonic == InstructionMnemonic::Ldc, 20);

    params.encodeCoreRegister(info.Addr.Rn, 16);
    params.encodeBits(static_cast<uint8_t>(info.Addr.Offset.Immediate >> 2), 8, 0);

    params.completeInstruction();
 }

//! @brief Assembles a generic co-processor data processing instruction.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeCoProcDataProc(AssemblyParams &params)
{
    const auto &info = params.Params->CoProcDataProcOp;

    if (info.OpCode1 > 0x0F)
    {
        params.ErrorMessage = "Op-code 1 is out of range.";
    }
    else if (info.OpCode2 > 0x07)
    {
        params.ErrorMessage = "Op-code 2 is out of range.";
    }
    else
    {
        params.encodeBits(7, 3, 25);
        params.encodeBits(info.OpCode1, 4, 20);
        params.encodeCoProcRegister(info.Rn, 16);
        params.encodeCoProcRegister(info.Rd, 12);
        params.encodeCoProcID(info.CoProcessor, 8);
        params.encodeBits(info.OpCode2, 3, 5);
        params.encodeCoProcRegister(info.Rm, 0);
        params.completeInstruction();
    }
}

//! @brief Assembles a generic co-processor register transfer instruction.
//! @param[in,out] params The object which contains the parameters of the
//! instruction to encode and is updated with the encoding.
void encodeCoProcRegTransfer(AssemblyParams &params)
{
    const auto &info = params.Params->CoProcRegTransOp;

    if (info.OpCode1 > 0x07)
    {
        params.ErrorMessage = "Op-code 1 is out of range.";
    }
    else if (info.OpCode2 > 0x07)
    {
        params.ErrorMessage = "Op-code 2 is out of range.";
    }
    else
    {
        if (params.Mnemonic == InstructionMnemonic::Mrc)
        {
            params.Instructions[params.InstructionCount] |= 0x0E100010;
        }
        else
        {
            params.Instructions[params.InstructionCount] |= 0x0E000010;
        }

        params.encodeBits(info.OpCode1, 3, 21);
        params.encodeCoProcRegister(info.Rn, 16);
        params.encodeCoreRegister(info.Rd, 12);
        params.encodeCoProcID(info.CoProcessor, 8);
        params.encodeBits(info.OpCode2, 3, 5);
        params.encodeCoProcRegister(info.Rm, 0);
        params.completeInstruction();
    }
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// AssemblyParams Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates a set of parameters used to assemble a 32-bit ARM machine
//! code instruction.
//! @param[in] opClass The class of operation being assembled.
//! @param[in] mnemonic The mnemonic of the instruction to assemble.
//! @param[in] condition The condition code to embed in the instruction.
//! @param[in] loadAddr The address at which the instruction is expected to
//! execute once assembled.
//! @param[in] params The operation-specific parameters to encode.
AssemblyParams::AssemblyParams(OperationClass opClass, InstructionMnemonic mnemonic,
                               ConditionCode condition, uint32_t loadAddr,
                               const InstructionParams *params) :
    Params(params),
    LoadAddress(loadAddr),
    InstructionCount(0),
    OpClass(opClass),
    Mnemonic(mnemonic),
    Condition(condition)
{
    std::fill_n(Instructions, arraySize(Instructions), 0u);
}

//! @brief Gets the bits of the instruction currently being encoded.
uint32_t &AssemblyParams::getInstruction()
{
    return Instructions[InstructionCount];
}

//! @brief Completes the current instruction.
void AssemblyParams::completeInstruction()
{
    ++InstructionCount;
}

//! @brief Encodes a specific condition on the current instruction.
//! @param[in] code The condition to encode.
void AssemblyParams::encodeCondition(ConditionCode code)
{
    uint32_t encoding;

    switch (code)
    {
    case ConditionCode::Hs:
        encoding = static_cast<uint32_t>(toScalar(ConditionCode::Cs) & 0x0F) << 28;
        break;

    case ConditionCode::Lo:
        encoding = static_cast<uint32_t>(toScalar(ConditionCode::Cc) & 0x0F) << 28;
        break;

    default:
        encoding = static_cast<uint32_t>(toScalar(code) & 0x0F) << 28;
        break;
    }

    Instructions[InstructionCount] |= encoding;
}

//! @brief Encodes a core register in the current instruction.
//! @param[in] reg The register to encode.
//! @param[in] lowestBit The 0-based index of the lowest bit of the encoding.
void AssemblyParams::encodeCoreRegister(CoreRegister reg, uint8_t lowestBit)
{
    uint32_t encoding = static_cast<uint32_t>(toScalar(reg) & 0x0F) << lowestBit;

    Instructions[InstructionCount] |= encoding;
}

//! @brief Encodes an ALU shift more in the current instruction.
//! @param[in] mode The shift type to encode, note Rrx and None will be handled
//! but additional bits will need to be set.
void AssemblyParams::encodeShiftType(ShiftType mode)
{
    uint8_t bits;

    if (mode == ShiftType::Rrx)
    {
        // Rm, RRX => Rm, ROR #0
        bits = toScalar(ShiftType::Ror);
    }
    else if (mode == ShiftType::None)
    {
        bits = toScalar(ShiftType::Lsl);
    }
    else
    {
        bits = toScalar(mode);
    }

    encodeBits(bits, 2, 5);
}

//! @brief Encodes an FPA register in the current instruction.
//! @param[in] reg The register to encode.
//! @param[in] lowestBit The 0-based index of the lowest bit of the encoding.
void AssemblyParams::encodeFpaRegister(FpaRegister reg, uint8_t lowestBit)
{
    uint32_t encoding = static_cast<uint32_t>(toScalar(reg) & 0x07) << lowestBit;

    Instructions[InstructionCount] |= encoding;
}

//! @brief Encodes a generic co-processor register in the current instruction.
//! @param[in] reg The register to encode.
//! @param[in] lowestBit The 0-based index of the lowest bit of the encoding.
void AssemblyParams::encodeCoProcRegister(CoProcRegister reg, uint8_t lowestBit)
{
    uint32_t encoding = static_cast<uint32_t>(toScalar(reg) & 0x0F) << lowestBit;

    Instructions[InstructionCount] |= encoding;
}

//! @brief Encodes a generic co-processor identifier in the current instruction.
//! @param[in] id The processor identifier to encode.
//! @param[in] lowestBit The 0-based index of the lowest bit of the encoding.
void AssemblyParams::encodeCoProcID(CoProcId id, uint8_t lowestBit)
{
    uint32_t encoding = static_cast<uint32_t>(toScalar(id) & 0x0F) << lowestBit;

    Instructions[InstructionCount] |= encoding;
}


//! @brief Encodes a single bit in the current instruction.
//! @param[in] at The 0-based index of the bit to set or clear.
void AssemblyParams::encodeBit(uint8_t at)
{
    uint32_t mask = 1u << at;

    Instructions[InstructionCount] |= mask;
}

//! @brief Encodes a single bit in the current instruction.
//! @param[in] value The value of the bit.
//! @param[in] at The 0-based index of the bit to set or clear.
void AssemblyParams::encodeBit(bool value, uint8_t at)
{
    uint32_t mask = 1u << at;

    if (value)
    {
        Instructions[InstructionCount] |= mask;
    }
    else
    {
        Instructions[InstructionCount] &= ~mask;
    }
}

//! @brief Encodes a value as a block of bits in the current instruction.
//! @param[in] data The data value to encode.
//! @param[in] bitCount The count of bits of data being encoded.
//! @param[in] lowestBit The 0-based index of the first bit to encode.
void AssemblyParams::encodeBits(uint32_t data, uint8_t bitCount, uint8_t lowestBit)
{
    uint32_t mask = (((1u << bitCount) - 1) << lowestBit);

    // Clear the bits before writing them
    Instructions[InstructionCount] &= ~mask;
    Instructions[InstructionCount] |= (data << lowestBit) & mask;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Assembled a 32-bit machine code instruction.
//! @param[in,out] params Details of the instruction to assemble, is updated
//! with the assembled machine code.
//! @retval true At least one instruction was assembled.
//! @retval false No instructions were assembled.
bool assembleInstruction(AssemblyParams &params)
{
    params.InstructionCount = 0;
    params.encodeCondition(params.Condition);

    switch (params.OpClass)
    {
    case OperationClass::Branch: {
        const int32_t MinOffset = static_cast<int32_t>(0xFE000008);
        const int32_t MaxOffset = (0x007FFFFF << 2) + 8;
        uint32_t pcAddr = params.LoadAddress + 8;
        int32_t offset = static_cast<int32_t>(params.Params->BranchOp.Address - pcAddr);

        if ((offset < MinOffset) || (offset > MaxOffset))
        {
            params.ErrorMessage = "Branch offset out of range.";
        }
        else
        {
        // Encode the op code.
            params.encodeBits((params.Mnemonic == InstructionMnemonic::Bl) ? 11u : 10u,
                              4, 24);

            // Encode the offset.
            params.encodeBits(static_cast<uint32_t>(offset >> 2),
                              24, 0);
            params.completeInstruction();
        }
    } break;

    case OperationClass::SoftwareIrq:
        if (params.Params->SoftwareIrqOp.Comment > 0x00FFFFFF)
        {
            params.ErrorMessage = "SWI comment value too big to be encoded.";
        }
        else
        {
            params.encodeBits(0x0F, 4, 24);
            params.encodeBits(params.Params->SoftwareIrqOp.Comment, 24, 0);
            params.completeInstruction();
        }
        break;


    case OperationClass::CoreMultiply: {
        const auto &info = params.Params->CoreMulOp;
        params.encodeBit(info.AffectsFlags, 20);
        params.encodeBits(9, 4, 4);
        params.encodeCoreRegister(info.Rd, 16);
        params.encodeCoreRegister(info.Rs, 8);
        params.encodeCoreRegister(info.Rm, 0);

        if (params.Mnemonic == InstructionMnemonic::Mla)
        {
            params.encodeBit(true, 21);
            params.encodeCoreRegister(info.Rn, 12);
        }
        params.completeInstruction();
    } break;

    case OperationClass::LongMultiply: {
        const auto &info = params.Params->LongMulOp;
        params.encodeBit(info.AffectsFlags, 20);
        params.encodeBits(9, 4, 4);
        params.encodeCoreRegister(info.RdHi, 16);
        params.encodeCoreRegister(info.RdLo, 12);
        params.encodeCoreRegister(info.Rs, 8);
        params.encodeCoreRegister(info.Rm, 0);

        switch (params.Mnemonic)
        {
        case InstructionMnemonic::Smull:
            params.encodeBits(6, 7, 21);
            break;

        case InstructionMnemonic::Smlal:
            params.encodeBits(7, 7, 21);
            break;

        case InstructionMnemonic::Umull:
            params.encodeBits(4, 7, 21);
            break;

        case InstructionMnemonic::Umlal:
            params.encodeBits(5, 7, 21);
            break;
        }

        params.completeInstruction();
    } break;


    case OperationClass::CoreAlu:
    case OperationClass::CoreCompare:
        encodeCoreAlu(params);
        break;

    case OperationClass::CoreAddress:
        encodeCoreAddress(params);
        break;

    case OperationClass::CoreDataTransfer:
        encodeDataTransfer(params);
        break;

    case OperationClass::CoreMultiTransfer:
        encodeMultiDataTransfer(params);
        break;

    case OperationClass::Breakpoint:
        // Encode the basic instruction bit pattern.
        // NOTE: The condition is AL (always) all the time.
        params.Instructions[params.InstructionCount] = 0xE1200070;

        // Encode the comment field.
        params.Instructions[params.InstructionCount] |=
            params.Params->BreakpointOp.Comment & 0x0F;
        params.Instructions[params.InstructionCount] |=
            (params.Params->BreakpointOp.Comment & 0xFFF0) << 4;
        params.completeInstruction();
        break;

    case OperationClass::AtomicSwap:
        encodeCoreSwap(params);
        break;

    case OperationClass::MoveFromPSR:
        params.Instructions[params.InstructionCount] |= 0x010F0000;
        params.encodeBit(params.Params->MoveFromPsrOp.IsCPSR == false, 22);
        params.encodeCoreRegister(params.Params->MoveFromPsrOp.Rd, 12);
        params.completeInstruction();
        break;

    case OperationClass::MoveToPSR:
        encodeMoveToPsr(params);
        break;

    case OperationClass::BranchExchange:

    case OperationClass::CoProcDataTransfer:
        encodeCoProcDataTransfer(params);
        break;

    case OperationClass::CoProcRegisterTransfer:
        encodeCoProcRegTransfer(params);
        break;

    case OperationClass::CoProcDataProcessing:
        encodeCoProcDataProc(params);
        break;

    case OperationClass::FpaDataTransfer:
    case OperationClass::FpaMultiTransfer:
    case OperationClass::FpaDyadicOperation:
    case OperationClass::FpaMonadicOperation:
    case OperationClass::FpaRegisterTransfer:
    case OperationClass::FpaComparison:

    case OperationClass::None:
    default:
        params.ErrorMessage = "Instruction class not supported.";
        break;
    }

    return params.InstructionCount > 0;
}

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


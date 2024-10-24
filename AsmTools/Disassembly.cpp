//! @file Disassembly.cpp
//! @brief The definition of logic to disassembly 32-bit ARM machine code.
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
#include "Ag/Core/Utils.hpp"

#include "Disassembly.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Interprets a co-processor data processing instruction, possibly based
//! on the instruction set of a specific co-processor.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretCoProcOp(DisassemblyParams &params)
{
    const uint32_t RequiredBits = 0x0E000000;
    const uint32_t RequiredMask = 0x0F000000;
    OperationClass opClass = OperationClass::None;

    if (params.isArchAllowed(InstructionInfo::ARMv2a) &&
        (params.Instructions[params.Decoded] & RequiredMask) == RequiredBits)
    {
        CoProcId coProc = params.decodeCoProcessorID(8);

        // TODO if ((coProc == ?) && (flags & AllowFPA))
        // Decode instruction as FPA operations.

        if (params.Instructions[params.Decoded] & (1u << 4))
        {
            // Its a register transfer (MCR/MRC)
            opClass = OperationClass::CoProcRegisterTransfer;
            params.Mnemonic = params.isSet(20) ? InstructionMnemonic::Mrc :
                                                 InstructionMnemonic::Mcr;

            CoProcRegisterTransfer &info = params.Params->CoProcRegTransOp;
            info.CoProcessor = coProc;
            info.Rd = params.decodeCoreRegister(12);
            info.Rn = params.decodeCoProcessorRegister(16);
            info.Rm = params.decodeCoProcessorRegister(0);
            info.OpCode1 = params.extract8(21, 3);
            info.OpCode2 = params.extract8(5, 3);
        }
        else
        {
            // Its a data processing instruction.
            opClass = OperationClass::CoProcDataProcessing;
            params.Mnemonic = InstructionMnemonic::Cdp;

            CoProcDataProcessing &info = params.Params->CoProcDataProcOp;
            info.CoProcessor = coProc;
            info.Rd = params.decodeCoProcessorRegister(12);
            info.Rn = params.decodeCoProcessorRegister(16);
            info.Rm = params.decodeCoProcessorRegister(0);
            info.OpCode1 = params.extract8(20, 4);
            info.OpCode2 = params.extract8(5, 3);
        }
        ++params.Decoded;
    }

    return opClass;
}

//! @brief Interprets a co-processor data transfer instruction.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretCoProcDataTransfer(DisassemblyParams &params)
{
    auto &info = params.Params->CoProcDataTransferOp;

    params.Mnemonic = params.isSet(20) ? InstructionMnemonic::Ldc :
                                         InstructionMnemonic::Stc;
    info.IsLong = params.isSet(22);
    info.CoProcessor = Ag::fromScalar<CoProcId>(params.extract8(8, 4));
    info.Rd = Ag::fromScalar<CoProcRegister>(params.extract8(12, 4));

    if (params.isSet(24))
    {
        info.Addr.Flags |= AddrOperand::PreIndexed;

        if (params.isSet(21))
        {
            info.Addr.Flags |= AddrOperand::Writeback;
        }
    }

    if (params.isClear(23))
    {
        info.Addr.Flags |= AddrOperand::NegativeOffset;
    }

    info.Addr.Offset.Mode = ShifterMode::ImmediateConstant;
    info.Addr.Rn = Ag::fromScalar<CoreRegister>(params.extract8(16, 4));
    info.Addr.Offset.Immediate = params.extract16(0, 8) << 2;
    ++params.Decoded;

    return OperationClass::CoProcDataTransfer;
}

//! @brief Interprets a core data processing instruction.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretCoreAlu(DisassemblyParams &params)
{
    // Bits Interpreted:
    // 0bXXXX 00?? ???? ???? ???? ???? ???? ????
    params.Mnemonic = Ag::fromScalar<InstructionMnemonic>(params.extract8(21, 4));
    OperationClass opClass = OperationClass::None;
    ShifterOperand *shifter = nullptr;

    if ((params.Mnemonic == InstructionMnemonic::Cmp) ||
        (params.Mnemonic == InstructionMnemonic::Cmn) ||
        (params.Mnemonic == InstructionMnemonic::Tst) ||
        (params.Mnemonic == InstructionMnemonic::Teq))
    {
        if (params.isSet(20))
        {
            // It's a core ALU comparison operation.
            opClass = OperationClass::CoreCompare;
            auto &info = params.Params->CoreCmpOp;
            shifter = &info.Op2;

            info.Rn = params.decodeCoreRegister(16);
            info.OverwritesPSR = (params.extract8(12, 4) == 0x0F);
        }
        else
        {
            // It's a move to/from the status register (MSR/MRS).
            opClass = OperationClass::MoveToPSR;
        }
    }
    else
    {
        opClass = OperationClass::CoreAlu;
        auto &info = params.Params->CoreAluOp;
        shifter = &info.Op2;
        info.AffectsFlags = params.isSet(20);
        info.Rd = params.decodeCoreRegister(12);
        info.Rn = params.decodeCoreRegister(16);
    }

    if (shifter != nullptr)
    {
        if (params.isSet(25))
        {
            // Its an immediate constant.
            shifter->Mode = ShifterMode::ImmediateConstant;
            shifter->Immediate = Ag::Bin::rotateRight(params.extract32(0, 8),
                                                      params.extract16(8, 4) * 2);
        }
        else
        {
            shifter->Rm = params.decodeCoreRegister(0);
            shifter->Shift = Ag::fromScalar<ShiftType>(params.extract8(5, 2));

            if (params.isSet(4) == false)
            {
                // Its an immediate shift.
                shifter->Immediate = params.extract8(7, 5);

                if (shifter->Immediate == 0)
                {
                    // Special cases:
                    if (shifter->Shift == ShiftType::Ror)
                    {
                        // <Rm>, ROR #0 => <Rm>, RRX
                        shifter->Shift = ShiftType::Rrx;
                        shifter->Mode = ShifterMode::RotateWithExtend;
                    }
                    else
                    {
                        // <Rm>, LSL #0 => <Rm>
                        shifter->Mode = ShifterMode::Register;
                    }
                }
                else
                {
                    shifter->Mode = ShifterMode::ShiftByConstant;
                }
            }
            else if (/* bit 4 set && */ params.isSet(7) == false)
            {
                // Its a register shift.
                shifter->Rs = params.decodeCoreRegister(8);
                shifter->Mode = ShifterMode::ShiftByRegister;
            }
            // else - Its a multiply instruction, we shouldn't be here.
        }

        ++params.Decoded;

        if ((shifter->Mode == ShifterMode::ImmediateConstant) &&
            (opClass == OperationClass::CoreAlu) &&
            (params.Params->CoreAluOp.Rn == CoreRegister::R15) &&
            ((params.Mnemonic == InstructionMnemonic::Add) ||
             (params.Mnemonic == InstructionMnemonic::Sub)))
        {
            // It's an ADR pseudo-instruction used to calculate a PC-relative
            // address and store it in a register.
            EffectiveAddress addr;

            // Extract values from the ALU operation before we overwrite them.
            addr.Rd = params.Params->CoreAluOp.Rd;
            addr.Encoding = MultiWordEncoding::Single;

            // The PC register is 8-bytes ahead of the current instruction due
            // to pipelining.
            uint32_t pc = params.LoadAddress + 8;

            // Make a copy of the original instruction template.
            uint32_t oldTemplate = params.Instructions[params.Decoded - 1];
            bool isPositiveOffset = false;

            if (params.Mnemonic == InstructionMnemonic::Add)
            {
                addr.Address = pc + shifter->Immediate;
                isPositiveOffset = true;
            }
            else // (params.Mnemonic == InstructionMnemonic::Sub)
            {
                addr.Address = pc - shifter->Immediate;
            }

            // Query further instruction to see if the encoding is longer.

            // Mask out the offset and Rn bits from the original instruction.
            oldTemplate &= ~0x000F0FFF;

            // Copy the original Rd to Rn.
            oldTemplate |= (oldTemplate & 0x00000F000) << 4;

            uint8_t maxSequence = std::min(params.MaxInstruction,
                                           static_cast<uint8_t>(3));

            while ((params.Decoded < maxSequence) &&
                   params.matches(0xFFFFF000, oldTemplate))
            {
                // The next instruction add to/subtracts from the
                // originally calculated PC-relative offset.
                uint32_t offset = Ag::Bin::rotateRight(params.extract32(0, 8),
                                                       params.extract8(8, 4) << 1);

                if (isPositiveOffset)
                {
                    addr.Address += offset;
                }
                else
                {
                    addr.Address -= offset;
                }

                ++params.Decoded;
            }

            // See if the sequence was padded with non-ops.
            while ((params.Decoded < maxSequence) && params.isNonOp())
            {
                ++params.Decoded;
            }

            switch (params.Decoded)
            {
            case 1: addr.Encoding = MultiWordEncoding::Single; break;
            case 2: addr.Encoding = MultiWordEncoding::Long; break;
            case 3:
            default: addr.Encoding = MultiWordEncoding::Extended; break;
            }

            opClass = OperationClass::CoreAddress;
            params.Mnemonic = InstructionMnemonic::Adr;
            params.Params->CoreAdr = addr;
        }
    }

    return opClass;
}

//! @brief Interprets a core multiply instruction.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretMultiply(DisassemblyParams &params)
{
    // Bits Interpreted:
    // 0bXXXX 0000 00?? ???? ???? ???? 1001 ????
    OperationClass opClass = OperationClass::None;

    if (params.matches(0x0FC000F0, 0x00000090))
    {
        // Core multiply.
        opClass = OperationClass::CoreMultiply;
        auto &info = params.Params->CoreMulOp;

        info.Rd = params.decodeCoreRegister(16);
        info.Rn = params.decodeCoreRegister(12);
        info.Rs = params.decodeCoreRegister(8);
        info.Rm = params.decodeCoreRegister(0);
        info.AffectsFlags = params.isSet(20);
        params.Mnemonic = params.isSet(21) ? InstructionMnemonic::Mla :
                                             InstructionMnemonic::Mul;
        ++params.Decoded;
    }

    return opClass;
}

//! @brief Interprets a long multiply instruction.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretLongMultiply(DisassemblyParams &params)
{
    // Bits Interpreted:
    // 0bXXXX 0000 1??? ???? ???? ???? 1001 ????
    // 
    // Core or Long multiply?
    OperationClass opClass = OperationClass::None;

    if (params.matches(0x0F8000F0, 0x00800090))
    {
        // Long multiply.
        opClass = OperationClass::LongMultiply;
        auto &info = params.Params->LongMulOp;

        info.RdHi = params.decodeCoreRegister(16);
        info.RdLo = params.decodeCoreRegister(12);
        info.Rs = params.decodeCoreRegister(8);
        info.Rm = params.decodeCoreRegister(0);
        info.AffectsFlags = params.isSet(20);

        switch (params.extract8(21, 2))
        {
        case 0: params.Mnemonic = InstructionMnemonic::Umull; break;
        case 1: params.Mnemonic = InstructionMnemonic::Umlal; break;
        case 2: params.Mnemonic = InstructionMnemonic::Smull; break;
        case 3: params.Mnemonic = InstructionMnemonic::Smlal; break;
        }

        ++params.Decoded;
    }

    return opClass;
}

//! @brief Interprets a core single register data transfer instructions.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretCoreDataTransfer(DisassemblyParams &params)
{
    OperationClass opClass = OperationClass::None;

    opClass = OperationClass::CoreDataTransfer;
    auto &info = params.Params->DataTransOp;
    params.Mnemonic = params.isSet(20) ? InstructionMnemonic::Ldr :
                                         InstructionMnemonic::Str;
    info.Rd = params.decodeCoreRegister(12);
    info.Addr.Rn = params.decodeCoreRegister(16);

    info.DataType = params.isSet(22) ? TransferDataType::UnsignedByte :
                                       TransferDataType::Word;

    if (params.isSet(23) == false)
    {
        // The offset is subtracted from the base address.
        info.Addr.Flags |= AddrOperand::NegativeOffset;
    }

    if (params.isSet(24))
    {
        // Uses pre-indexed addressing mode.
        if (params.isSet(21))
        {
            info.Addr.Flags |= AddrOperand::Writeback;
        }

        info.Addr.Flags |= AddrOperand::PreIndexed;
    }
    else
    {
        // Uses post-indexed addressing mode.
        info.UserPrivilages = params.isSet(21);
    }

    if (params.isSet(25))
    {
        // Operand 2 is a register, possibly shifted by a constant.
        info.Addr.Offset.Mode = ShifterMode::ShiftByConstant;
        info.Addr.Offset.Shift = Ag::fromScalar<ShiftType>(params.extract8(5, 2));
        info.Addr.Offset.Rm = params.decodeCoreRegister(0);
        info.Addr.Offset.Immediate = params.extract8(7, 5);

        // Fix up the operand specification.
        fixDisasmShifterMode(info.Addr.Offset);
    }
    else
    {
        // Operand 2 is an immediate constant.
        info.Addr.Offset.Mode = ShifterMode::ImmediateConstant;
        info.Addr.Offset.Immediate = params.extract16(0, 12);
    }

    ++params.Decoded;

    return opClass;
}

//! @brief Interprets a core half-word/signed byte data transfer instruction.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretSignedHalfWordTransfer(DisassemblyParams &params)
{
    OperationClass opClass = OperationClass::CoreDataTransfer;

    opClass = OperationClass::CoreDataTransfer;
    auto &info = params.Params->DataTransOp;
    info.Rd = params.decodeCoreRegister(12);
    info.Addr.Rn = params.decodeCoreRegister(16);

    switch (params.extract8(5, 2))
    {
    case 0: // Not valid?
        opClass = OperationClass::None;
        break;

    case 1:
        info.DataType = TransferDataType::UnsignedHalfWord;
        params.Mnemonic = params.isSet(20) ? InstructionMnemonic::Ldr :
                                             InstructionMnemonic::Str;
        break;

    case 2:
        info.DataType = TransferDataType::SignedByte;
        params.Mnemonic = InstructionMnemonic::Ldr;
        break;
    case 3:
        info.DataType = TransferDataType::SignedHalfWord;
        params.Mnemonic = InstructionMnemonic::Ldr;
        break;
    }

    if (params.isClear(23))
    {
        // The offset is subtracted from the base address.
        info.Addr.Flags |= AddrOperand::NegativeOffset;
    }

    if (params.isSet(24))
    {
        // Uses pre-indexed addressing mode.
        if (params.isSet(21))
        {
            info.Addr.Flags |= AddrOperand::Writeback;
        }

        info.Addr.Flags |= AddrOperand::PreIndexed;
    }
    else
    {
        // Uses post-indexed addressing mode.
        if (params.isSet(21))
        {
            // The write-back bit cannot be set for this address mode.
            opClass = OperationClass::None;
        }
    }

    if (params.isSet(22))
    {
        // Operand 2 is an immediate constant.
        info.Addr.Offset.Mode = ShifterMode::ImmediateConstant;
        info.Addr.Offset.Immediate = (params.extract8(8, 4) << 4) |
                                     params.extract8(0, 4);
    }
    else if (params.extract8(8, 4) == 0)
    {
        // Operand 2 is a register, never shifted.
        info.Addr.Offset.Mode = ShifterMode::Register;
        info.Addr.Offset.Rm = params.decodeCoreRegister(0);
    }
    else
    {
        // The Rs register should be 0 in this mode.
        opClass = OperationClass::None;
    }

    ++params.Decoded;

    return opClass;
}

//! @brief Interprets a core multi-register data transfer instruction.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretCoreMultiTransfer(DisassemblyParams &params)
{
    static const MultiTransferMode nonStackModes[]{
        MultiTransferMode::DecrementAfter,  // 0: Decrement, After, Store
        MultiTransferMode::IncrementAfter,  // 1: Increment, After, Store
        MultiTransferMode::DecrementBefore, // 2: Decrement, Before, Store
        MultiTransferMode::IncrementBefore, // 3: Increment, Before, Store
        MultiTransferMode::DecrementAfter,  // 4: Decrement, After, Load
        MultiTransferMode::IncrementAfter,  // 5: Increment, After, Load
        MultiTransferMode::DecrementBefore, // 6: Decrement, Before, Load
        MultiTransferMode::IncrementBefore, // 7: Increment, Before, Load
    };

    static const MultiTransferMode stackModes[]{
        MultiTransferMode::EmptyDescending, // 0: Decrement, After, Store
        MultiTransferMode::EmptyAscending,  // 1: Increment, After, Store
        MultiTransferMode::FullDescending,  // 2: Decrement, Before, Store
        MultiTransferMode::FullAscending,   // 3: Increment, Before, Store
        MultiTransferMode::FullAscending,   // 4: Decrement, After, Load
        MultiTransferMode::FullDescending,  // 5: Increment, After, Load
        MultiTransferMode::EmptyAscending,  // 6: Decrement, Before, Load
        MultiTransferMode::EmptyDescending, // 7: Increment, Before, Load
    };

    OperationClass opClass = OperationClass::CoreMultiTransfer;
    auto &info = params.Params->MultiTransOp;

    params.Mnemonic = params.isSet(20) ? InstructionMnemonic::Ldm :
                                         InstructionMnemonic::Stm;
    info.Writeback = params.isSet(21);
    info.UserModeRegs = params.isSet(22);
    info.Rd = params.decodeCoreRegister(16);
    info.Registers = params.extract16(0, 16);
    uint8_t bits = params.extract8(23, 2) | (params.extract8(20, 1) << 2);

    if ((params.Flags & InstructionInfo::UseStackModesAlways) ||
        ((params.Flags & InstructionInfo::UseStackModesOnR13) &&
         (info.Rd == CoreRegister::R13)))
    {
        info.Mode = stackModes[bits];
    }
    else
    {
        info.Mode = nonStackModes[bits];
    }

    ++params.Decoded;

    return opClass;
}

//! @brief Interprets a core atomic swap instruction.
//! @param[in,out] params The disassembly parameters and is updated with the
//! result.
//! @return The class of operation recognised.
OperationClass interpretSwap(DisassemblyParams &params)
{
    params.Mnemonic = InstructionMnemonic::Swp;
    auto &info = params.Params->AtomicSwapOp;

    info.IsByte = params.isSet(22);
    info.Rd = params.decodeCoreRegister(12);
    info.Rn = params.decodeCoreRegister(16);
    info.Rm = params.decodeCoreRegister(0);

    ++params.Decoded;

    return OperationClass::AtomicSwap;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// DisassemblyParams Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Initialises options ready for calling the disassembler.
//! @param[in] instruction The instruction bit field to disassemble.
//! @param[in] flags The disassembly options as defined by the
//! InsrtuctionInfo::DisasmBits enumeration type.
//! @param[in] loadAddr The address at which the instruction exists.
//! @param[in] params A pointer to the instruction parameter union to be filled
//! by the disassembly process.
DisassemblyParams::DisassemblyParams(uint32_t instruction, uint32_t flags,
                                     uint32_t loadAddr, InstructionParams *params) :
    Params(params),
    LoadAddress(loadAddr),
    Flags(flags),
    Condition(ConditionCode::Nv),
    Mnemonic(InstructionMnemonic::MaxMnemonic),
    MaxInstruction(1),
    Decoded(0)
{
    if (params == nullptr)
    {
        throw Ag::ArgumentException("params");
    }

    Instructions[0] = instruction;

    std::fill_n(Instructions + 1, std::size(Instructions) - 1, 0);
}

DisassemblyParams::DisassemblyParams(const uint32_t *instructions, uint8_t count,
                                     uint32_t flags, uint32_t loadAddr,
                                     InstructionParams *params) :
    Params(params),
    LoadAddress(loadAddr),
    Flags(flags),
    Condition(ConditionCode::Nv),
    Mnemonic(InstructionMnemonic::MaxMnemonic),
    MaxInstruction(std::min(static_cast<uint8_t>(std::size(Instructions)), count)),
    Decoded(0)
{
    if (params == nullptr)
    {
        throw Ag::ArgumentException("params");
    }
    else if (MaxInstruction < 1)
    {
        throw Ag::ArgumentException("count");
    }

    std::copy_n(instructions, MaxInstruction, Instructions);
    std::fill_n(Instructions + MaxInstruction,
                std::size(Instructions) - MaxInstruction, 0);
}

//! @brief Determines if the current instruction is MOVcc Rx,Rx.
//! @retval true The instruction has no effect.
//! @retval false The instruction does not follow the non-op pattern.
bool DisassemblyParams::isNonOp() const noexcept
{
    uint32_t instruction = Instructions[Decoded];

    // Check to see if the instruction is MOVcc Rx,Rx
    return ((instruction & 0x0FFF0FFF) == 0x01A00000) &&
           (((instruction >> 12) & 0x0F) == (instruction & 0x0F));
}

//! @brief Determines if a bit in the instruction field is set.
//! @param[in] offset The offset of the bit to extract.
//! @retval true The bit was set.
//! @retval false The bit was clear.
bool DisassemblyParams::isSet(uint8_t offset) const noexcept
{
    return (Instructions[Decoded] & (1u << offset));
}

//! @brief Determines if a bit in the instruction field is clear.
//! @param[in] offset The offset of the bit to extract.
//! @retval true The bit was clear.
//! @retval false The bit was set.
bool DisassemblyParams::isClear(uint8_t offset) const noexcept
{
    return (Instructions[Decoded] & (1u << offset)) == 0;
}

//! @brief Determines if certain significant bits of the instruction
//! match a specified value.
//! @param[in] mask The mask defining which instruction bits are significant.
//! @param[in] significantBits Defines the values of the significant bits.
//! @retval true All significant bits are properly set or clear.
//! @retval false At least one significant bit does not match.
bool DisassemblyParams::matches(uint32_t mask, uint32_t significantBits) const noexcept
{
    return (Instructions[Decoded] & mask) == significantBits;
}

//! @brief Determines whether a specific architecture is enabled in a bit-field
//! encoded using DiasmFlags.
//! @param[in] arch The architecture to test for, e.g. InstructionInfo::ARMv3.
//! @retval true Instructions from the set described by arch are enabled.
//! @retval false Instructions for the specified architecture should be
//! interpreted as invalid.
bool DisassemblyParams::isArchAllowed(uint32_t arch) const noexcept
{
    return (Flags & InstructionInfo::ModelMask) >= arch;
}

//! @brief Determines if a disassembly option was enabled.
//! @param[in] option The option to test for.
//! @retval true The disassembly option was enabled.
//! @retval false The disassembly option was disabled.
bool DisassemblyParams::hasOption(InstructionInfo::DisasmBits option) const noexcept
{
    return (Flags & option) != 0;
}

//! @brief Extracts up to 8 bits from the instruction bit field.
//! @param[in] offset The 0-based index of the first bit to extract.
//! @param[in] bitCount The count of bits (1-8) to extract.
//! @return The extracted bits.
uint8_t DisassemblyParams::extract8(uint8_t offset, uint8_t bitCount) const noexcept
{
    return static_cast<uint8_t>(Instructions[Decoded] >> offset) & ((1u << bitCount) - 1);
}

//! @brief Extracts up to 16 bits from the instruction bit field.
//! @param[in] offset The 0-based index of the first bit to extract.
//! @param[in] bitCount The count of bits (1-16) to extract.
//! @return The extracted bits.
uint16_t DisassemblyParams::extract16(uint8_t offset, uint8_t bitCount) const noexcept
{
    return static_cast<uint16_t>(Instructions[Decoded] >> offset) & ((1u << bitCount) - 1);
}

//! @brief Extracts up to 32 bits from the instruction bit field.
//! @param[in] offset The 0-based index of the first bit to extract.
//! @param[in] bitCount The count of bits (1-32) to extract.
//! @return The extracted bits.
uint32_t DisassemblyParams::extract32(uint8_t offset, uint8_t bitCount) const noexcept
{
    return static_cast<uint32_t>(Instructions[Decoded] >> offset) & ((1u << bitCount) - 1);
}

//! @brief Extracts the value of a core register from an instruction bit field.
//! @param[in] lowestBit The lowest index of the bit encoding the value to extract.
//! @return The extracted register value.
CoreRegister DisassemblyParams::decodeCoreRegister(uint8_t lowestBit) const noexcept
{
    return Ag::fromScalar<CoreRegister>(static_cast<uint8_t>((Instructions[Decoded] >> lowestBit) & 0x0F));
}

//! @brief Extracts the value of a co-processor register from an instruction bit field.
//! @param[in] lowestBit The lowest index of the bit encoding the value to extract.
//! @return The extracted register value.
CoProcRegister DisassemblyParams::decodeCoProcessorRegister(uint8_t lowestBit) const noexcept
{
    return Ag::fromScalar<CoProcRegister>(static_cast<uint8_t>((Instructions[Decoded] >> lowestBit) & 0x0F));
}

//! @brief Extracts the value of an FPA register from an instruction bit field.
//! @param[in] lowestBit The lowest index of the bit encoding the value to extract.
//! @return The extracted register value.
FpaRegister DisassemblyParams::decodeFPARegister(uint8_t lowestBit) const noexcept
{
    return Ag::fromScalar<FpaRegister>(static_cast<uint8_t>((Instructions[Decoded] >> lowestBit) & 0x07));
}

//! @brief Extracts the value of a co-processor identifier from an instruction bit field.
//! @param[in] lowestBit The lowest index of the bit encoding the value to extract.
//! @return The extracted co-processor identifier value.
CoProcId DisassemblyParams::decodeCoProcessorID(uint8_t lowestBit) const noexcept
{
    return Ag::fromScalar<CoProcId>(static_cast<uint8_t>((Instructions[Decoded] >> lowestBit) & 0x0F));
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Disassembles a 32-bit ARM machine code instruction.
//! @param[in,out] params Receives the instruction and options for disassembly
//! and is updated with the parameters extracted from the instruction.
OperationClass disassembleInstruction(DisassemblyParams &params)
{
    OperationClass opClass = OperationClass::None;

    // Initialise the condition code, it is unlikely but possible that it
    // might be overwritten.
    params.Condition = Ag::fromScalar<ConditionCode>(static_cast<uint8_t>(params.Instructions[params.Decoded] >> 28));
    uint8_t opCode;

    // Perform base decoding using bits 25-27.
    switch (static_cast<uint8_t>(params.Instructions[params.Decoded] >> 25) & 0x07)
    {
    case 0x00:
        // Core ALU operations using register/shifted register operand 2.
        // Core ALU multiply/long multiply.
        // ARMv2a+ Atomic Swap.
        // ARMv3+ Move from Status Register.
        // ARMv3+ Move register to Status Register.
        // ARMv4+ Branch/Exchange
        // ARMv4+ Load/Store half-word/signed byte.
        // ARMv5+ Breakpoint.
        opCode = params.extract8(21, 4);

        // Check for an invalid shifter operand.
        if (params.matches(0x0090, 0x0090))
        {
            // The shifter operand is invalid, this is used to encode
            // Multiply, half/double word data transfer and swap.
            switch (params.extract8(5, 2))
            {
            case 0x00:
                switch (params.extract8(21, 4))
                {
                case 0:
                case 1:
                    opClass = interpretMultiply(params);
                    break;

                case 4:
                case 5:
                case 6:
                case 7:
                    opClass = interpretLongMultiply(params);
                    break;

                case 8: // SWP
                case 10: // SWPB
                    if (params.isArchAllowed(InstructionInfo::ARMv2a))
                    {
                        opClass = interpretSwap(params);
                    }
                    break;
                }
                break;

            case 0x01:
                // Load/store half-word (ARMv4+)
                opClass = interpretSignedHalfWordTransfer(params);
                break;

            case 0x02:
            case 0x03:
            default:
                if (params.isSet(20))
                {
                    // Load/store 2 words (ARMv5+)
                }
                else
                {
                    // Load signed half-word/byte (ARMv4+)
                    opClass = interpretSignedHalfWordTransfer(params);
                }
                break;
            }
        }
        else if ((opCode >= 8) && (opCode <= 11) && params.isClear(20))
        {
            // TST, TEQ, CMP, CMN with no S bit, which is invalid on ARMv2
            // but opens up a slew if ARMv3+ instruction encodings.
            switch (params.extract8(21, 4))
            {
            case 8:
                if (params.matches(0x0FFF0FFF, 0x010F0000))
                {
                    // MRS Rd, CPSR
                    opClass = OperationClass::MoveFromPSR;
                    params.Mnemonic = InstructionMnemonic::Mrs;
                    params.Params->MoveFromPsrOp.Rd = params.decodeCoreRegister(12);
                    params.Params->MoveFromPsrOp.IsCPSR = true;
                    ++params.Decoded;
                }
                break;
            case 9:
                if (params.matches(0xFFF000F0, 0xE1200070))
                {
                    // BKPT
                    opClass = OperationClass::Breakpoint;
                    params.Mnemonic = InstructionMnemonic::Bkpt;

                    // Extract the 12 bits of the comment field.
                    uint32_t comment = params.Instructions[params.Decoded] & 0x0F;
                    comment |= (params.Instructions[params.Decoded] >> 4) & 0xFFF0;
                    params.Params->BreakpointOp.Comment = static_cast<uint16_t>(comment);
                    ++params.Decoded;
                }
                else if (params.matches(0x0FF0FFF0, 0x0120F000))
                {
                    // MSR CPSR_*,Rm
                    opClass = OperationClass::MoveToPSR;
                    params.Mnemonic = InstructionMnemonic::Msr;
                    auto &info = params.Params->MoveToPsrOp;
                    info.SourceImmediate = 0;
                    info.IsCPSR = true;
                    info.IsSourceReg = true;
                    info.SourceReg = params.decodeCoreRegister(0);
                    info.PsrComponents = params.extract8(16, 4);
                    ++params.Decoded;
                }
                break;

            case 10:
                if (params.matches(0x0FFF0FFF, 0x014F0000))
                {
                    // MRS Rd, SPSR
                    opClass = OperationClass::MoveFromPSR;
                    params.Mnemonic = InstructionMnemonic::Mrs;
                    params.Params->MoveFromPsrOp.Rd = params.decodeCoreRegister(12);
                    params.Params->MoveFromPsrOp.IsCPSR = false;
                    ++params.Decoded;
                }
                break;

            case 11:
                if (params.matches(0x0FF0FFF0, 0x0160F000))
                {
                    // MSR SPSR_*,Rm
                    opClass = OperationClass::MoveToPSR;
                    params.Mnemonic = InstructionMnemonic::Msr;
                    auto &info = params.Params->MoveToPsrOp;
                    info.SourceImmediate = 0;
                    info.IsCPSR = false;
                    info.IsSourceReg = true;
                    info.SourceReg = params.decodeCoreRegister(0);
                    info.PsrComponents = params.extract8(16, 4);
                    ++params.Decoded;
                }
                break;
            }
        }
        else
        {
            // It's an ALU operation with an immediate constant.
            opClass = interpretCoreAlu(params);
        }
        break;

    case 0x01:
        // Core ALU operations using immediate constant operand 2.
        // ARMv3+ Move immediate to Status Register.
        opCode = params.extract8(21, 4);

        if ((opCode >= 8) && (opCode <= 11) && params.isClear(20))
        {
            // TST, TEQ, CMP, CMN with no S bit, which is invalid on ARMv2.
            if (params.matches(0x0FB0F000, 0x0320F000))
            {
                // MSR ?PSR_*, #<immed>
                opClass = OperationClass::MoveToPSR;
                params.Mnemonic = InstructionMnemonic::Msr;
                auto &info = params.Params->MoveToPsrOp;
                info.IsCPSR = params.isClear(22);
                info.IsSourceReg = false;
                info.PsrComponents = params.extract8(16, 4);
                info.SourceImmediate = Ag::Bin::rotateRight(params.extract32(0, 8),
                                                            params.extract8(8, 4) << 1);
                info.SourceReg = CoreRegister::R0;
                ++params.Decoded;
            }
        }
        else
        {
            // Its an ALU operation with a register (possibly shifted).
            opClass = interpretCoreAlu(params);
        }
        break;

    case 0x02:
        // Load/Store with immediate offset.
    case 0x03:
        // Load/Store with register offset.
        opClass = interpretCoreDataTransfer(params);
        break;

    case 0x04:
        // Load/Store multiple registers.
        opClass = interpretCoreMultiTransfer(params);
        break;

    case 0x05:
        // Branch.
        // Branch/exchange.
        if ((params.Condition == ConditionCode::Nv) &&
            params.isArchAllowed(InstructionInfo::ARMv5))
        {
            // Branch/exchange.
        }
        else
        {
            // Branch.
            opClass = OperationClass::Branch;
            params.Mnemonic = params.isSet(24) ? InstructionMnemonic::Bl :
                                                 InstructionMnemonic::B;

            // Sign extend the offset field and multiply by 4.
            int32_t offset = static_cast<int32_t>(params.Instructions[params.Decoded] << 8) >> 6;
            uint32_t pcAddr = params.LoadAddress + 8;
            params.Params->BranchOp.Address = pcAddr + static_cast<uint32_t>(offset);
            ++params.Decoded;
        }
        break;

    case 0x06:
        // Co-processor load/store.
        opClass = interpretCoProcDataTransfer(params);
        break;

    case 0x07:
        // Software interrupt.
        // Co-processor data processing.
        // Co-processor register transfer.
        if (params.isSet(24))
        {
            // Its a software interrupt.
            opClass = OperationClass::SoftwareIrq;
            params.Mnemonic = InstructionMnemonic::Swi;
            params.Params->SoftwareIrqOp.Comment = params.extract32(0, 24);
            ++params.Decoded;
        }
        else
        {
            // Its a co-processor register transfer.
            opClass = interpretCoProcOp(params);
        }
        break;

    default:
        break;
    }

    if (opClass == OperationClass::None)
    {
        // If we can't decode the instruction, it essentially becomes EQUD 0xXXXXXXXX
        params.Params->Bits.Bits[0] = params.Instructions[0];
        params.Decoded = 1;
    }

    return opClass;
}

//! @brief Adjusts a shifter operand after being disassembled.
//! @param[in,out] shiftOperand The recently disassembled shifter operand to be
//! analysed and updated.
void fixDisasmShifterMode(ShifterOperand &shiftOperand)
{
    if ((shiftOperand.Mode == ShifterMode::ShiftByConstant) &&
        (shiftOperand.Immediate == 0))
    {
        switch (shiftOperand.Shift)
        {
        case ShiftType::Lsl:
            // <Rm> LSL #0 => <Rm>.
            shiftOperand.Mode = ShifterMode::Register;
            shiftOperand.Shift = ShiftType::None;
            break;

        case ShiftType::Lsr:
        case ShiftType::Asr:
            // <Rm> LSR/ASR #0 => <Rm> LSR/ASR #32.
            shiftOperand.Immediate = 32;
            break;

        case ShiftType::Ror:
            // <Rm> ROR #0 => <Rm> RRX.
            shiftOperand.Mode = ShifterMode::RotateWithExtend;
            shiftOperand.Shift = ShiftType::Rrx;
            break;

        default:
            break;
        }
    }
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


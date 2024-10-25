//! @file Disassembly.hpp
//! @brief The declaration of an entry point to the ARM disassembler.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_DISASSEMBLY_HPP__
#define __ASM_TOOLS_DISASSEMBLY_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object holding parameters used to communicate with the disassembler.
struct DisassemblyParams
{
    InstructionParams *Params;
    uint32_t Instructions[4];
    uint32_t LoadAddress;
    uint32_t Flags;
    ConditionCode Condition;
    InstructionMnemonic Mnemonic;
    uint8_t MaxInstruction;
    uint8_t Decoded;

    // Construction
    DisassemblyParams(uint32_t instruction, uint32_t flags,
                      uint32_t loadAddr, InstructionParams *params);

    DisassemblyParams(const uint32_t *instructions, uint8_t count,
                      uint32_t flags, uint32_t loadAddr,
                      InstructionParams *params);

    // Operations
    bool isNonOp() const noexcept;
    bool isSet(uint8_t offset) const noexcept;
    bool isClear(uint8_t offset) const noexcept;
    bool matches(uint32_t mask, uint32_t significantBits) const noexcept;
    bool isArchAllowed(uint32_t arch) const noexcept;
    bool hasOption(InstructionInfo::DisasmBits option) const noexcept;
    uint8_t extract8(uint8_t offset, uint8_t bitCount) const noexcept;
    uint16_t extract16(uint8_t offset, uint8_t bitCount) const noexcept;
    uint32_t extract32(uint8_t offset, uint8_t bitCount) const noexcept;
    CoreRegister decodeCoreRegister(uint8_t lowestBit) const noexcept;
    CoProcRegister decodeCoProcessorRegister(uint8_t lowestBit) const noexcept;
    FpaRegister decodeFPARegister(uint8_t lowestBit) const noexcept;
    CoProcId decodeCoProcessorID(uint8_t lowestBit) const noexcept;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
OperationClass disassembleInstruction(DisassemblyParams &params);
void fixDisasmShifterMode(ShifterOperand &shiftOperand);

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

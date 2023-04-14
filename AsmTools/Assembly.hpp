//! @file Assembly.hpp
//! @brief The declaration of an entry point to assemble a 32-bit ARM machine
//! code instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_ASSEMBLY_HPP__
#define __ASM_TOOLS_ASSEMBLY_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Common state used to assemble instructions.
struct AssemblyParams
{
    // Public Fields
    static const uint8_t MaxInstructions = 4;
    const InstructionParams *Params;
    String ErrorMessage;
    uint32_t Instructions[MaxInstructions];
    uint32_t LoadAddress;
    uint8_t InstructionCount;
    OperationClass OpClass;
    InstructionMnemonic Mnemonic;
    ConditionCode Condition;

    // Construction
    AssemblyParams(OperationClass opClass, InstructionMnemonic mnemonic,
                   ConditionCode condition, uint32_t loadAddr,
                   const InstructionParams *params);

    // Accessors
    uint32_t &getInstruction();

    // Operations
    void completeInstruction();
    void encodeCondition(ConditionCode code);
    void encodeCoreRegister(CoreRegister reg, uint8_t lowestBit);
    void encodeShiftType(ShiftType mode);
    void encodeFpaRegister(FpaRegister reg, uint8_t lowestBit);
    void encodeCoProcRegister(CoProcRegister reg, uint8_t lowestBit);
    void encodeCoProcID(CoProcId id, uint8_t lowestBit);
    void encodeBit(uint8_t at);
    void encodeBit(bool value, uint8_t at);
    void encodeBits(uint32_t data, uint8_t bitCount, uint8_t lowestBit);
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool assembleInstruction(AssemblyParams &params);

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

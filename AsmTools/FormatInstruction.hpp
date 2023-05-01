//! @file FormatInstruction.hpp
//! @brief The declaration of an entry point to a function which will format a
//! 32-bit ARM machine code instruction as text.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_FORMAT_INSTRUCTION_HPP__
#define __ASM_TOOLS_FORMAT_INSTRUCTION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"
#include "FormatInstruction.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A structure used to pass parameters which allow a 32-bit ARM
//! machine code instruction to be formatted as text.
struct FormatParams
{
    // Public Fields
    const FormatterOptions *Options;
    const InstructionParams *Params;
    OperationClass OpClass;
    InstructionMnemonic Mnemonic;
    ConditionCode Condition;
    std::string Builder;

    // Construction/Destruction
    FormatParams(const FormatterOptions *options,
                 const InstructionParams *params,
                 OperationClass opClass,
                 InstructionMnemonic mnemonic,
                 ConditionCode condition);

    // Accessors
    bool hasOption(FormatterOptions::Flags flag) const;

    // Operations
    void appendSeparator();
    void appendMnemonic();
    void appendConditionCode();
    void appendOffset(int32_t offset);
    void appendAddress(uint32_t address);
    void appendComment(uint32_t value);
    void appendSuffix(char suffix, bool hasSuffix);
    void append(CoreRegister reg);
    void append(ShiftType shift);
    void append(CoProcId cpId);
    void append(CoProcRegister reg);
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool formatInstruction(FormatParams &params);

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

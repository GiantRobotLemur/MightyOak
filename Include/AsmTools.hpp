//! @file AsmTools.hpp
//! @brief The root header file defining public elements of the ARM Assembler
//! Tools library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_ASM_TOOLS_HPP__
#define __AG_ASM_TOOLS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/Options.hpp"
#include "AsmTools/Messages.hpp"
#include "AsmTools/ObjectCode.hpp"
#include "AsmTools/InstructionInfo.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
ObjectCode assembleFile(const Ag::String &fliePath, const Options &assemblyOptions,
                        Messages &messages);

ObjectCode assembleText(const Ag::String &sourceCode, const Options &assemblyOptions,
                        Messages &messages);

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

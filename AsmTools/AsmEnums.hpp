//! @file AsmEnums.hpp
//! @brief The declaration of various enumeration types used to annotate tokens
//! in parsed assembly language.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_ASM_ENUMS_HPP__
#define __ASM_TOOLS_ASM_ENUMS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

//! @brief Expresses the type of an assembly language directive token.
enum class AssemblyDirectiveType : uint8_t
{
    Include,
    InstructionSet,
    ProcessorExtension,
    ProcessorMode,
    AddressMode,
};

////! @brief Expresses the processor extensions enabled during assembly.
//enum class ProcessorExtensionEnum : uint8_t
//{
//    None = 0x00,
//    Fpa = 0x01,
//    Vfp = 0x02,
//};

//! @brief Expresses the processor mode the code being assembled is
//! expected to be operating in.
enum class AddressMode : uint8_t
{
    Bits26,
    Bits32,
};

//! @brief Expresses the data types expected in data directives.
enum class DirectiveDataType
{
    Byte,
    HalfWord,
    Word,
    LongWord,
    NativeString,
    Utf8String,
    Utf16String,
    Utf32String,
    Real32,
    Real64,
    Real96,
};

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

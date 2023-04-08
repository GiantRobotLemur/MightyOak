//! @file Options.cpp
//! @brief The definition of a set of properties configuring the assembly
//! process.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/Options.hpp"

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

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a default set of assembly options.
Options::Options() :
    _loadAddress(0x0000),
    _instructionSet(InstructionSet::ArmV2a),
    _extensions(toScalar(ArchExtensionEnum::None)),
    _flags(toScalar(AssemblyFlagsEnum::None))
{
    _includePaths.reserve(16);
}

//! @brief Gets the instruction set allowed in the assembled code.
InstructionSet Options::getInstructionSet() const
{
    return _instructionSet;
}

//! @brief Sets the instruction set allowed in the assembled code.
//! @param[in] instructionSet The new instruction set.
void Options::setInstructionSet(InstructionSet instructionSet)
{
    _instructionSet = instructionSet;
}

//! @brief Sets the set of architecture extensions allowed in the assembled code.
ArchExtensions Options::getArchitectureExtensions() const
{
    return _extensions;
}

//! @brief Adds an extension to the set allowed in assembled code.
//! @param[in] extension The extension to enable.
void Options::addArchitecureExtension(ArchExtensionEnum extension)
{
    _extensions |= toScalar(extension);
}

//! @brief Queries whether an architecture extension is enabled.
//! @param[in] extension The extension to test for.
//! @retval true The extension was enabled.
//! @retval false The extension was disabled.
bool Options::hasArchitectureExtension(ArchExtensionEnum extension) const
{
    return (_extensions & toScalar(extension)) != 0;
}

//! @brief Gets the address the code expected to be loaded to if not position
//! independent code.
uint32_t Options::getLoadAddress() const
{
    return _loadAddress;
}

//! @brief Sets the address the code expected to be loaded to if not position
//! independent code.
//! @param[in] loadAddr The new load address, should be at least 4-byte aligned.
void Options::setLoadAddress(uint32_t loadAddr)
{
    _loadAddress = loadAddr;
}

//! @brief Gets binary options which affect the assembly process.
AssemblyFlags Options::getFlags() const
{
    return _flags;
}

//! @brief Determines if a binary assembly option is set.
//! @param[in] flag The option to test.
//! @retval true The option is enabled.
//! @retval false The option is disabled.
bool Options::hasFlag(AssemblyFlagsEnum flag) const
{
    return (_flags & toScalar(flag)) != 0;
}

//! @brief Sets one of the binary options which affect the assembly process.
//! @param[in] flag The option to set.
void Options::setFlag(AssemblyFlagsEnum flag)
{
    _flags |= toScalar(flag);
}

//! @brief Gets the set of all folders to be searched for include files
//! referenced in the source code.
const std::vector<Fs::Path> &Options::getIncludePaths() const
{
    return _includePaths;
}

//! @brief Adds a path to a directory to be searched for include files
//! referenced in the source code.
//! @param[in] path The path to add.
//! @throws Fs::InvalidFilePathException If the structure of the path is invalid.
//! @throws Fs::InvalidPathElementException If an element in the path is invalid.
void Options::addIncludePath(string_cref_t path)
{
    _includePaths.emplace_back(path);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets static metadata for the InstructionSet enumeration type.
const EnumInfo<InstructionSet> &getInstructionSetType()
{
    static const EnumInfo<InstructionSet> instance({
        { InstructionSet::ArmV2, "ArmV2", "ARM v2", "The original ARM 2/250 processor with no cache." },
        { InstructionSet::ArmV2a, "ArmV2a", "ARM v2a", "The ARM 2 + cache, i.e. the ARM 3." },
        { InstructionSet::ArmV3, "ArmV3", "ARM v3", "The version 3 architecture implemented by ARM 610 and ARM 710." },
        { InstructionSet::ArmV4, "ArmV4", "ARM v4", "The version 4 architecture implemented by the StrongARM and ARM 810."},
        { InstructionSet::ArmV5, "ArmV5", "ARM v5", "The version 5 architecture." },
        { InstructionSet::ArmV6, "ArmV6", "ARM v6", "The version 6 architecture implemented by the original Raspberry Pi."},
        { InstructionSet::ArmV7, "ArmV7", "ARM v7", "The last 32-bit ARM architecture." }
    });

    return instance;
}

//! @brief Gets static metadata for the ArchExtensionEnum flag enumeration.
const EnumInfo<ArchExtensionEnum> &getArchExtensionsType()
{
    static const EnumInfo<ArchExtensionEnum> instance({
        { ArchExtensionEnum::None, "None", "None", "No extensions enabled." },
        { ArchExtensionEnum::Fpa, "Fpa", "FPA", "The Floating Point Accelerator used with ARM v2 and v3." },
        { ArchExtensionEnum::VfpV1, "VfpV1", "VFP v1", "The version 1 Vector Floating Point extension." },
        { ArchExtensionEnum::VfpV2, "VfpV2", "VFP v2", "The version 2 Vector Floating Point extension used on ARM v5+." },
        { ArchExtensionEnum::VfpV3, "VfpV3", "VFP v3", "The version 3 Vector Floating Point extension used on ARM v7+." },
        { ArchExtensionEnum::VfpV4, "VfpV4", "VFP v4", "The version 4 Vector Floating Point extension used on ARM v7+." },
        { ArchExtensionEnum::Neon, "Neon", "Neon", "An advanced SIMD extension used on ARM v7." },
        { ArchExtensionEnum::ThumbV1, "ThumbV1", "Thumb V1", "Version 1 16-bit thumb instruction set used on ARM v3+." },
        { ArchExtensionEnum::ThumbV2, "ThumbV2", "Thumb V2", "Version 1 16-bit thumb instruction set used on ARM v5+." },
        { ArchExtensionEnum::BreakPt, "BreakPt", "Break Point", "The debug break point implemented by ARM v5+." },
    });

    return instance;
}


}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


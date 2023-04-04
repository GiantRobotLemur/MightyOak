//! @file AssemblyState.cpp
//! @brief The definition of an object which encapsulates the assembly state
//! which can change between statements.
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
#include "Ag/Core/Exception.hpp"
#include "AssemblyState.hpp"


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
// AssemblyState Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
const AssemblyState AssemblyState::Empty = AssemblyState(Asm::Options());

//! @brief Creates an assembly state based on an initial set of assembly options.
//! @param[in] initialState The set of options specified for assembly of the
//! entire source unit.
AssemblyState::AssemblyState(const Options &initialState) :
    _instructionSet(initialState.getInstructionSet()),
    _archExtensionFlags(initialState.getArchitectureExtensions()),
    _addressMode(AddressMode::Bits26),
    _operatingMode(ProcessorMode::Arm)
{
    if (_instructionSet >= InstructionSet::ArmV3)
    {
        _addressMode = AddressMode::Bits32;
    }
}

//! @brief Gets whether a specified instruction set is supported given the
//! current state of the assembler.
//! @param[in] querySet The instruction set to try and use.
//! @retval true The instruction set is valid the current context.
//! @retval false The instruction set is invalid.
bool AssemblyState::isValidInstructionSet(InstructionSet querySet) const
{
    return _instructionSet >= querySet;
}

//! @brief Gets the set of instructions which can be legally assembled.
InstructionSet AssemblyState::getInstructionSet() const
{
    return _instructionSet;
}

//! @brief Sets the set of instructions which can be legally assembled.
//! @param[in] instructionSet the new instruction set.
void AssemblyState::setInstructionSet(InstructionSet instructionSet)
{
    _instructionSet = instructionSet;
}

//! @brief Determines if an extension is legal in the current context.
//! @param[in] extension The extension to validate.
//! @retval true Instructions from the extension are legal to assemble.
//! @retval false The extension is not legal in the current context.
bool AssemblyState::isValidExtension(ArchExtensionEnum extension) const
{
    return (_archExtensionFlags & toScalar(extension)) != 0;
}

//! @brief Adds an extension to those which can be assembled in the
//! current context.
//! @param[in] extension The extension to add.
void AssemblyState::addExtension(ArchExtensionEnum extension)
{
    _archExtensionFlags |= toScalar(extension);
}

//! @brief Gets the addressing mode instructions are assumed to be assembled in.
AddressMode AssemblyState::getAddressMode() const
{
    return _addressMode;
}

//! @brief Sets the addressing mode instructions are assumed to be assembled in.
//! @param[in] mode The new address mode (26 or 32-bit).
void AssemblyState::setAddressMode(AddressMode mode)
{
    _addressMode = mode;
}

//! @brief Gets the execution mode the processor is expected to be in for the
//! following instructions.
ProcessorMode AssemblyState::getProcessorMode() const
{
    return _operatingMode;
}

//! @brief Sets the execution mode the processor is expected to be in for the
//! following instructions.
//! @param[in] mode The new execution mode.
void AssemblyState::setProcessorMode(ProcessorMode mode)
{
    _operatingMode = mode;
}

//! @brief Creates a clone of the current assembly state.
AssemblyStateSPtr AssemblyState::clone() const
{
    return std::make_shared<AssemblyState>(*this);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! Gets a display text string to represent an instruction set specification.
//! @param[in] instructionSet The instruction set specification to express.
//! @return A static text string representing the instruction set.
const char *instructionSetToString(InstructionSet instructionSet)
{
    static const char *names[] = {
        "ARM v2",
        "ARM v2a (ARM3)",
        "ARM v3",
        "ARM v4",
        "ARM v5",
        "ARM v6",
        "ARM v7",
    };

    static_assert(arraySize(names) == toScalar(InstructionSet::Max),
                  "The instruction set names are out of synch with the enumeration definition.");

    return (instructionSet < InstructionSet::Max) ? names[toScalar(instructionSet)] :
                                                    names[toScalar(InstructionSet::ArmV7)];
}

//! Gets a display text string to represent a processor operating mode.
//! @param[in] mode The processor mode to express.
//! @return A static text string representing the processor mode.
const char *processorModeToString(ProcessorMode mode)
{
    static const char *modes[] = {
        "32-bit ARM",
        "16-bit Thumb",
        "Jazelle byte code",
    };

    static_assert(arraySize(modes) == toScalar(ProcessorMode::Max),
                  "The processor mode names are out of synch with the enumeration definition.");

    return (mode < ProcessorMode::Max) ? modes[toScalar(mode)] :
                                         modes[toScalar(ProcessorMode::Arm)];
}

//! Gets a display text string to represent a processor extension.
//! @param[in] extension The processor extension to express.
//! @return A static text string representing the processor extension.
const char *processorExtensionToString(ArchExtensionEnum extension)
{
    static const char *extensions[] = {
        "FPA",
        "VFP v1",
        "VFP v2",
        "VFP v3",
        "VFP v4",
        "Neon",
        "Thumb v1",
        "Thumb v2",
        "Break Point",
    };

    static_assert(arraySize(extensions) == toScalar(ArchExtensionEnum::Max),
                  "The architecture extension names are out of synch with the enumeration definition.");

    int32_t msb;
    if (Bin::bitScanReverse(static_cast<uint32_t>(toScalar(extension)), msb) &&
        (static_cast<uint32_t>(msb) < toScalar(ArchExtensionEnum::Max)))
    {
        return extensions[msb];
    }
    else
    {
        return "Unknown";
    }
}

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


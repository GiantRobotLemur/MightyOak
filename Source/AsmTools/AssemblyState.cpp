//! @file AssemblyState.cpp
//! @brief The definition of an object which encapsulates the assembly state
//! which can change between statements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2026
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

namespace Mo {
namespace Asm {

namespace {

void appendScopeTypeName(std::string &buffer, AssemblyState::Scope scope)
{
    switch (scope)
    {
    case AssemblyState::Scope::Global:
        buffer.append("global");
        break;

    case AssemblyState::Scope::Subroutine:
        buffer.append("subroutine");
        break;

    case AssemblyState::Scope::Macro:
        buffer.append("macro");
        break;

    default:
        buffer.append("(unknown)");
        break;
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// AssemblyState Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
const AssemblyState AssemblyState::Empty = AssemblyState(Asm::Options());

//! @brief Creates an assembly state based on an initial set of assembly options.
//! @param[in] initialState The set of options specified for assembly of the
//! entire source unit.
AssemblyState::AssemblyState(const Options &initialState) :
    _scopeType(Scope::Global),
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
    return (_archExtensionFlags & Ag::toScalar(extension)) != 0;
}

//! @brief Adds an extension to those which can be assembled in the
//! current context.
//! @param[in] extension The extension to add.
void AssemblyState::addExtension(ArchExtensionEnum extension)
{
    _archExtensionFlags |= Ag::toScalar(extension);
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

//! @brief Gets the state of the parent scope, if there is one.
const std::shared_ptr<AssemblyState> &AssemblyState::getParentState() const
{
    return _globalState;
}

//! @brief Tries to alter the scope to indicate that a new subroutine is
//! being defined.
//! @param[in] messages The collection to append failure message to.
//! @param[in] at The location in source of the keyword that started the definition.
//! @param[in] name The name of the subroutine being started.
//! @param[in] baseState The assembly state before the subroutine was entered.
//! @retval true The scope was successfully updated.
//! @retval false The scope was not changed, but details were added to @p messages
//! to describe why.
bool AssemblyState::tryBeginSubroutine(Messages &messages, const Location &at,
                                       Ag::string_cref_t name,
                                       std::shared_ptr<AssemblyState> &baseState)
{
    return tryStartRecording(Scope::Subroutine, messages, at, name, baseState);
}

//! @brief Tries to alter the scope to indicate that the end of a subroutine
//! definition has been reached.
//! @param[in] messages The collection to append failure message to.
//! @param[in] at The location in source of the keyword that ends the definition.
//! @retval true The scope was successfully updated.
//! @retval false The scope was not changed, but details were added to @p messages
//! to describe why.
bool AssemblyState::tryEndSubroutine(Messages &messages, const Location &at)
{
    return tryEndRecording(Scope::Subroutine, messages, at);
}

//! @brief Tries to alter the scope to indicate that a new macro is
//! being defined.
//! @param[in] messages The collection to append failure message to.
//! @param[in] at The location in source of the keyword that started the definition.
//! @param[in] name The name of the macro being started.
//! @param[in] baseState The assembly state before the macro was entered.
//! @retval true The scope was successfully updated.
//! @retval false The scope was not changed, but details were added to @p messages
//! to describe why.
bool AssemblyState::tryBeginMacro(Messages &messages, const Location &at,
                                  Ag::string_cref_t name,
                                  std::shared_ptr<AssemblyState> &baseState)
{
    return tryStartRecording(Scope::Macro, messages, at, name, baseState);
}

//! @brief Tries to alter the scope to indicate that the end of a macro
//! definition has been reached.
//! @param[in] messages The collection to append failure message to.
//! @param[in] at The location in source of the keyword that ends the definition.
//! @retval true The scope was successfully updated.
//! @retval false The scope was not changed, but details were added to @p messages
//! to describe why.
bool AssemblyState::tryEndMacro(Messages &messages, const Location &at)
{
    return tryEndRecording(Scope::Macro, messages, at);
}

//! @brief Creates a clone of the current assembly state.
AssemblyStateSPtr AssemblyState::clone() const
{
    return std::make_shared<AssemblyState>(*this);
}

//! @brief Tries to alter the scope to indicate that a new scope is
//! being defined.
//! @param[in] scopeType The type of scope to start.
//! @param[in] messages The collection to append failure message to.
//! @param[in] at The location in source of the keyword that started the definition.
//! @param[in] name The name of the scope being started.
//! @param[in] baseState The assembly state before the new scope was entered.
//! @retval true The scope was successfully updated.
//! @retval false The scope was not changed, but details were added to @p messages
//! to describe why.
bool AssemblyState::tryStartRecording(Scope scopeType, Messages &messages,
                                      const Location &at, Ag::string_cref_t name,
                                      std::shared_ptr<AssemblyState> &baseState)
{
    if (_scopeType != Scope::Global)
    {
        std::string message;
        message.assign("Cannot begin defining a ");
        appendScopeTypeName(message, scopeType);
        message.append(" from within ");

        if (_scopeType == scopeType)
            message.append("another ");
        else
            message.append("a ");

        appendScopeTypeName(message, _scopeType);
        message.append(" definition.");

        messages.appendError(at, message);

        return false;
    }

    _scopeType = scopeType;
    _scopeName = name;
    _scopeDefinition = at;
    _globalState = baseState;

    return true;
}

//! @brief Tries to alter the scope to indicate that the end of a scope
//! definition has been reached.
//! @param[in] messages The collection to append failure message to.
//! @param[in] at The location in source of the keyword that ends the definition.
//! @retval true The scope was successfully updated.
//! @retval false The scope was not changed, but details were added to @p messages
//! to describe why.
bool AssemblyState::tryEndRecording(Scope scopeType, Messages &messages,
                                    const Location &at)
{
    if (_scopeType != scopeType)
    {
        std::string message;
        message.assign("Cannot end the definition of ");
        appendScopeTypeName(message, scopeType);
        message.append(" from within ");
        appendScopeTypeName(message, _scopeType);
        message.append(" scope.");

        messages.appendError(at, message);

        return false;
    }

    // Reset the current scope.
    _scopeType = Scope::Global;
    _scopeName = Ag::String::Empty;
    _scopeDefinition = Location();

    return true;
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

    static_assert(std::size(names) == Ag::toScalar(InstructionSet::Max),
                  "The instruction set names are out of synch with the enumeration definition.");

    return (instructionSet < InstructionSet::Max) ? names[Ag::toScalar(instructionSet)] :
                                                    names[Ag::toScalar(InstructionSet::ArmV7)];
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

    static_assert(std::size(modes) == Ag::toScalar(ProcessorMode::Max),
                  "The processor mode names are out of synch with the enumeration definition.");

    return (mode < ProcessorMode::Max) ? modes[Ag::toScalar(mode)] :
                                         modes[Ag::toScalar(ProcessorMode::Arm)];
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

    static_assert(std::size(extensions) == Ag::toScalar(ArchExtensionEnum::Max),
                  "The architecture extension names are out of synch with the enumeration definition.");

    int32_t msb;
    if (Ag::Bin::bitScanReverse(static_cast<uint32_t>(Ag::toScalar(extension)), msb) &&
        (static_cast<uint32_t>(msb) < Ag::toScalar(ArchExtensionEnum::Max)))
    {
        return extensions[msb];
    }
    else
    {
        return "Unknown";
    }
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


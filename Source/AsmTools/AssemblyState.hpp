//! @file AssemblyState.hpp
//! @brief The declaration of an object which encapsulates the assembly state
//! which can change between statements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_ASSEMBLY_STATE_HPP__
#define __ASM_TOOLS_ASSEMBLY_STATE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include "AsmTools/Options.hpp"
#include "AsmTools/Messages.hpp"
#include "AsmEnums.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Encapsulates the assembly state which can change between statements.
class AssemblyState : public std::enable_shared_from_this<AssemblyState>
{
public:
    // Public Constants
    //! @brief An empty state.
    static const AssemblyState Empty;

    // Public Types
    enum class Scope
    {
        //! @brief The state exists in the global scope.
        Global,

        //! @brief The state exists within the definition of a named subroutine.
        Subroutine,

        //! @brief The state exists within the definition of a named macro.
        Macro,
    };

    // Construction/Destruction
    AssemblyState(const Options &initialState);
    ~AssemblyState() = default;

    // Accessors
    bool isValidInstructionSet(InstructionSet querySet) const;
    InstructionSet getInstructionSet() const;
    void setInstructionSet(InstructionSet instructionSet);

    bool isValidExtension(ArchExtensionEnum extension) const;
    void addExtension(ArchExtensionEnum extension);

    AddressMode getAddressMode() const;
    void setAddressMode(AddressMode mode);

    ProcessorMode getProcessorMode() const;
    void setProcessorMode(ProcessorMode mode);

    const std::shared_ptr<AssemblyState> &getParentState() const;

    // Operations
    bool tryBeginSubroutine(Messages &messages, const Location &at,
                            Ag::string_cref_t name,
                            std::shared_ptr<AssemblyState> &baseState);
    bool tryEndSubroutine(Messages &messages, const Location &at);

    bool tryBeginMacro(Messages &messages, const Location &at,
                       Ag::string_cref_t name,
                       std::shared_ptr<AssemblyState> &baseState);
    bool tryEndMacro(Messages &messages, const Location &at);

    std::shared_ptr<AssemblyState> clone() const;
private:
    // Internal Functions
    bool tryStartRecording(Scope scopeType, Messages &messages,
                           const Location &at, Ag::string_cref_t name,
                           std::shared_ptr<AssemblyState> &baseState);
    bool tryEndRecording(Scope scopeType, Messages &messages,
                         const Location &at);

    // Internal Fields
    //! @brief The assembly state from before the scope was entered.
    std::shared_ptr<AssemblyState> _globalState;

    //! @brief The name of the subroutine or macro being recorded.
    Ag::String _scopeName;

    //! @brief The location at which the scope started.
    Location _scopeDefinition;

    //! @brief The type of scope the state exists within.
    Scope _scopeType;

    InstructionSet _instructionSet;     // ARMv2, etc.
    ArchExtensions _archExtensionFlags; // FPA, VFP, etc.
    AddressMode _addressMode;           // 26/32-bit
    ProcessorMode _operatingMode;       // ARM/Thumb
};

using AssemblyStateSPtr = std::shared_ptr<AssemblyState>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
const char *instructionSetToString(InstructionSet instructionSet);
const char *processorModeToString(ProcessorMode mode);
const char *processorExtensionToString(ArchExtensionEnum extension);

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

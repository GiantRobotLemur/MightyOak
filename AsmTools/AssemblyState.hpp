//! @file AssemblyState.hpp
//! @brief The declaration of an object which encapsulates the assembly state
//! which can change between statements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
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
#include "AsmEnums.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

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

    // Operations
    std::shared_ptr<AssemblyState> clone() const;
private:
    // Internal Fields
    InstructionSet _instructionSet;     // ARMv2, etc.
    ArchExtensions _archExtensionFlags; // FPA, VFP, etc.
    AddressMode _addressMode;    // 26/32-bit
    ProcessorMode _operatingMode; // ARM/Thumb
};

using AssemblyStateSPtr = std::shared_ptr<AssemblyState>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
const char *instructionSetToString(InstructionSet instructionSet);
const char *processorModeToString(ProcessorMode mode);
const char *processorExtensionToString(ArchExtensionEnum extension);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file InstructionStatement.hpp
//! @brief The declaration of a Statement-derived object which assembles
//! 32-bit ARM machine code instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_INSTRUCTION_STATMENT_HPP__
#define __ASM_TOOLS_INSTRUCTION_STATMENT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

#include "AsmEnums.hpp"
#include "Statement.hpp"

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
class InstructionInfo;
class AssemblyState;

//! @brief A Statement-derived object which assembles 32-bit ARM machine code
//! instructions.
class InstructionStatement : public Statement
{
public:
    // Construction/Destruction
    InstructionStatement(const Location &start,
                         OperationClass opClass,
                         InstructionMnemonic mnemonic,
                         ConditionCode condition);
    virtual ~InstructionStatement() = default;

    // Accessors
    const Location &getStart() const;

    // Overrides
    virtual uint32_t calculateObjectCodeSize(IEvalContext *context) const;
    virtual bool assemble(const AssemblyState &state, IEvalContext *context,
                          ObjectCodeBuilder &output) const;
protected:
    // Accessors
    InstructionMnemonic getMnemonic() const;
    ConditionCode getCondition() const;
    bool validateInstructionSet(const AssemblyState &state,
                                Messages &log,
                                InstructionSet minInstructionSet) const;
    bool validateProcessorMode(const AssemblyState &state,
                               Messages &log,
                               ProcessorMode mode) const;
    bool validateExtension(const AssemblyState &state,
                           Messages &log,
                           ArchExtensionEnum extension) const;
    bool validateAddressMode(const AssemblyState &state,
                             Messages &log,
                             AddressMode extension) const;

    // Operations

    //! @brief Is called to define the instruction to be assembled.
    //! @param[in,out] instruction The object to update with instruction-specific
    //! parameters.
    //! @param[in] context The context in which expressions can be evaluated.
    //! @param[in] log The message log to add errors to if the instruction
    //! cannot be formed.
    //! @param[in] isFinalPass True indicating that if expressions do not
    //! evaluate due to missing symbols errors should be logged. False to
    //! ignore missing symbols and simply fail to assemble.
    //! @retval true The instruction was properly configured for assembly.
    //! @retval false The instruction could not be configured for assembly,
    //! messages may have been added to the log to describe why.
    virtual bool configureInstruction(InstructionInfo &instruction,
                                      IEvalContext *context, Messages &log,
                                      bool isFinalPass) const = 0;

    virtual bool validate(const InstructionInfo &instruction,
                          const AssemblyState &state,
                          Messages &log) const;
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    Location _start;
    OperationClass _opClass;
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

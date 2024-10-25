//! @file InstructionStatement.cpp
//! @brief The definition of a Statement-derived object which assembles
//! 32-bit ARM machine code instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Format.hpp"

#include "AsmEnums.hpp"
#include "AssemblyState.hpp"
#include "ExprContexts.hpp"
#include "InstructionStatement.hpp"
#include "ObjectCodeBuilder.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// InstructionStatement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a statement object which can assemble a machine code
//! instruction.
//! @param[in] start The location of the beginning of the statement.
//! @param[in] opClass The class of operation the instruction represents.
//! @param[in] mnemonic The instruction mnemonic.
//! @param[in] condition The instruction condition code.
InstructionStatement::InstructionStatement(const Location &start,
                                           OperationClass opClass,
                                           InstructionMnemonic mnemonic,
                                           ConditionCode condition) :
    Statement(StatementType::Instruction),
    _start(start),
    _opClass(opClass),
    _mnemonic(mnemonic),
    _condition(condition)
{
}

//! @brief Gets the location in source code of the start of the statement.
const Location &InstructionStatement::getStart() const
{
    return _start;
}

// Inherited from Statement.
uint32_t InstructionStatement::calculateObjectCodeSize(IEvalContext */*context*/) const
{
    // Most instructions are 4 bytes long.
    // Extended versions of the ADR directive maybe 8-16 bytes, the only time
    // when this member function needs to be overridden.
    return 4;
}

// Inherited from Statement.
bool InstructionStatement::assemble(const AssemblyState &state,
                                    IEvalContext *context,
                                    ObjectCodeBuilder &output) const
{
    InstructionInfo instruction(_mnemonic, _opClass, _condition);
    bool isAssembled = false;

    // Configure the instruction parameters into the object.
    if (configureInstruction(instruction, context,
                             output.getOutput(),
                             output.isFinalPass()))
    {
        // Whether or not assembly failed, there is no point deferring
        // assembly of this instruction.
        isAssembled = true;

        // Encode the instruction description as machine code.
        Ag::String errorMessage;
        uint32_t instructions[4];
        size_t count = 0;

        // Ensure the instruction encoding is valid given the situation
        // before attempting to assemble it.
        if (validate(instruction, state, output.getOutput()))
        {
            count = instruction.assemble(instructions, output.getCurrentAddress(),
                                         4, errorMessage);

            if (count > 0)
            {
                // Copy the instructions to object code.
                isAssembled = true;

                for (size_t i = 0; i < count; ++i)
                {
                    output.writeWord(instructions[i]);
                }
            }
            else
            {
                // Nothing was assembled, issue an error and pad the output.
                if (errorMessage.isEmpty())
                {
                    errorMessage = "Invalid instruction parameters.";
                }

                output.getOutput().appendError(_start, errorMessage);
            }
        }

        uint32_t expectedSize = calculateObjectCodeSize(context);

        if ((count * 4) < expectedSize)
        {
            // Pad the output with zeros if necessary as theoretically
            // previously logged errors should report to the caller that
            // the assembled machine code is invalid.
            size_t paddingBytes = expectedSize - (count * 4);

            output.writeZeros(paddingBytes);
        }
    }

    return isAssembled;
}

//! @brief Gets the mnemonic passed to the constructor.
InstructionMnemonic InstructionStatement::getMnemonic() const
{
    return _mnemonic;
}

//! @brief Gets the condition code passed to the constructor.
ConditionCode InstructionStatement::getCondition() const
{
    return _condition;
}

//! @brief Verifies that the assembly state allows instructions from a specific
//! instruction set.
//! @param[in] state The assembly state to query.
//! @param[in] log The log to add an error to if the current instruction set does
//! not meet the required specification.
//! @param[in] minInstructionSet The minimum required instruction set.
//! @retval true The instruction set is supported.
//! @retval false The instruction set is not supported, an error has been logged.
bool InstructionStatement::validateInstructionSet(const AssemblyState &state,
                                                  Messages &log,
                                                  InstructionSet minInstructionSet) const
{
    bool isOK = true;

    if (state.isValidInstructionSet(minInstructionSet) == false)
    {
        isOK = false;
        log.appendError(_start,
                        Ag::String::format("The instruction is invalid in the "
                                           "selected instruction set ({0}). "
                                           "It requires at least the {1} "
                                           "instruction set.", {
                            instructionSetToString(state.getInstructionSet()),
                            instructionSetToString(minInstructionSet)
                        }));
    }

    return isOK;
}

//! @brief Verifies that the assembly state expects instructions to be executed
//! in a specific processor mode.
//! @param[in] state The assembly state to query.
//! @param[in] log The log to add an error to if the state doesn't match the
//! required mode.
//! @param[in] mode The required processor mode for the operation being
//! validated.
//! @retval true The assembler is expecting the specified mode.
//! @retval false The assembler in assembling instructions for a different
//! processor mode.
bool InstructionStatement::validateProcessorMode(const AssemblyState &state,
                                                 Messages &log,
                                                 ProcessorMode mode) const
{
    bool isOK = true;

    if (state.getProcessorMode() != mode)
    {
        isOK = false;

        log.appendError(_start,
                        Ag::String::format("The instruction is expected to be executed in "
                                           "the {0} processing mode, but instructions "
                                           "currently being assembled are expected to "
                                           "operate in the {1} mode.", {
                            processorModeToString(mode),
                            processorModeToString(state.getProcessorMode())
        }));
    }

    return isOK;
}

//! @brief Verifies that the assembly state allows instructions from a specific
//! extension instruction set.
//! @param[in] state The assembly state to query.
//! @param[in] log The log to add an error to if the state doesn't match the
//! required mode.
//! @param[in] extension The extension to determine if it is allowed.
//! @retval true The assembler allows the extension.
//! @retval false The assembler prohibits the extension.
bool InstructionStatement::validateExtension(const AssemblyState &state,
                                             Messages &log,
                                             ArchExtensionEnum extension) const
{
    bool isOK = true;

    if (state.isValidExtension(extension) == false)
    {
        isOK = false;

        log.appendError(_start,
                        Ag::String::format("The instruction requires the {0} processor "
                                           "extension to be enabled.",
                                           { processorExtensionToString(extension) }));
    }

    return isOK;
}

//! @brief Verifies that the assembly state allows instructions which require a
//! specific address mode.
//! @param[in] state The assembly state to query.
//! @param[in] log The log to add an error to if the state doesn't match the
//! required mode.
//! @param[in] mode The required address mode.
//! @retval true The assembler is in the required address mode.
//! @retval false The assembler is in the wrong address mode.
bool InstructionStatement::validateAddressMode(const AssemblyState &state,
                                               Messages &log,
                                               AddressMode mode) const
{
    bool isOK = true;

    if (state.getAddressMode() != mode)
    {
        isOK = false;

        uint32_t bits[] = { 26, 32 };

        log.appendError(_start,
                        Ag::String::format("The instruction requires the processor "
                                           "is expected to be in a {0}-bit address mode.",
                                           { bits[Ag::toScalar(mode)] }));
    }

    return isOK;
}

//! @brief Ensures the final instruction is valid given the state of
//! the assembler when it is being assembled.
//! @param[in] instruction The instruction to analyse.
//! @param[in] state The state of the assembler.
//! @param[in] log The log to receive error messages if the instruction
//! is invalid.
//! @note The base class implementation performs no validation checking, it
//! merely returns true.
bool InstructionStatement::validate(const InstructionInfo &/*instruction*/,
                                    const AssemblyState &/*state*/,
                                    Messages &/*log*/) const
{
    // Do nothing.
    return true;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


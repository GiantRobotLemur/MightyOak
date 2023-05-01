//! @file InstructionInfo.cpp
//! @brief The definition of data structures which describe any assembly
//! language instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Ag/Core/Exception.hpp"
#include "AsmTools/InstructionInfo.hpp"

#include "Assembly.hpp"
#include "Disassembly.hpp"
#include "FormatInstruction.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// InstructionInfo Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Ensures that the union is zeroed at construction.
InstructionParams::InstructionParams()
{
    reset();
}

//! @brief Zeros the contents of the union.
void InstructionParams::reset()
{
    std::fill_n(Bits.Bits, sizeof(InstructionParams) / sizeof(uint32_t), 0u);
}

////////////////////////////////////////////////////////////////////////////////
// FormatterOptions Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a default object which defines how an interpreted
//! instruction should be formatted as text.
FormatterOptions::FormatterOptions() :
    _address(0),
    _flags(0)
{
}

//! @brief Constructs an initialised object which defines how an interpreted
//! instruction should be formatted as text.
FormatterOptions::FormatterOptions(uint32_t address, uint32_t flags) :
    _address(address),
    _flags(flags)
{
}

//! @brief Gets bits representing formatting options described by the
//! IFormatter::Flags enumeration type.
uint32_t FormatterOptions::getFlags() const { return _flags; }


//! @brief Sets bits representing formatting options described by the
//! IFormatter::Flags enumeration type.
//! @param[in] flags The new formatting flags.
void FormatterOptions::setFlags(uint32_t flags) { _flags = flags; }

//! @brief Gets the absolute address at which the instruction being formatted
//! exists in memory. This is used if Flags::ShowOffsets is not set.
uint32_t FormatterOptions::getInstructionAddress() const { return _address; }

//! @brief Sets the absolute address at which the instruction being formatted
//! exists in memory. This is used if Flags::ShowOffsets is not set.
void FormatterOptions::setInstructionAddress(uint32_t addr) { _address = addr & ~0x3; }

//! @brief Optionally appends the value of a symbol based on its
//! absolute address.
//! @param[in] address The absolute address to format.
//! @param[in] buffer The buffer to append to.
//! @details If no characters are appended to buffer, a numeric value will be
//! formatted instead.
//! 
//! The base implementation appends nothing.
void FormatterOptions::appendAddressSymbol(uint32_t /*address*/,
                                          std::string &/*buffer*/) const
{
    // Do nothing in the base implementation.
}

//! @brief Optionally appends the value of a software interrupt routine
//! based on the ordinal encoded in an instruction.
//! @param[in] value The value encoded in the SWI instruction.
//! @param[in] buffer The buffer to append to.
//! @details If no characters are appended to buffer, a numeric value will be
//! formatted instead.
//! 
//! The base implementation appends nothing.
void FormatterOptions::appendSwiComment(uint32_t /*value*/,
                                        std::string &/*buffer*/) const
{
    // Do nothing in the base implementation.
}

//! @brief Optionally appends the value of a break point instruction
//! based on the ordinal encoded in an instruction.
//! @param[in] value The value encoded in the BKPT instruction.
//! @param[in] buffer The buffer to append to.
//! @details If no characters are appended to buffer, a numeric value will be
//! formatted instead.
//! 
//! The base implementation appends nothing.
void FormatterOptions::appendBreakPointComment(uint32_t /*value*/,
                                               std::string &/*buffer*/) const
{
    // Do nothing in the base implementation.
}

////////////////////////////////////////////////////////////////////////////////
// InstructionInfo Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty instruction information object.
InstructionInfo::InstructionInfo() :
    _condition(ConditionCode::Al),
    _mnemonic(InstructionMnemonic::MaxMnemonic),
    _opClass(OperationClass::None)
{
}

//! @brief Initialises the instruction with a type and default parameters.
//! @param[in] mnemonic The mnemonic of the instruction to encode.
//! @param[in] opClass The type of instruction being encoded.
//! @param[in] condition The condition code of the instruction.
InstructionInfo::InstructionInfo(InstructionMnemonic mnemonic,
                                 OperationClass opClass,
                                 ConditionCode condition /*= ConditionCode::Al*/) :
    _condition(condition),
    _mnemonic(mnemonic),
    _opClass(opClass)
{
}

//! @brief Determines whether the object is in an empty state.
//! @retval true The object does not describe a valid instruction.
//! @retval false The object describes a machine code instruction.
bool InstructionInfo::isEmpty() const
{
    return _opClass == OperationClass::None;
}

//! @brief Gets the instruction condition code.
ConditionCode InstructionInfo::getCondition() const
{
    return _condition;
}

//! @brief Sets the instruction condition code.
//! @param[in] condition The new instruction condition code.
void InstructionInfo::setConditionCode(ConditionCode condition)
{
    _condition = condition;
}

//! @brief Gets the instruction mnemonic.
InstructionMnemonic InstructionInfo::getMnemonic() const
{
    return _mnemonic;
}

//! @brief Gets the class of operation the object represent which defines the
//! set of parameters which are pertinent.
OperationClass InstructionInfo::getOperationClass() const
{
    return _opClass;
}

//! @brief Gets the parameters of a core ALU instruction.
const CoreAlu &InstructionInfo::getCoreAluParamters() const
{
    validateInstructionClass(OperationClass::CoreAlu);

    return _params.CoreAluOp;
}

//! @brief Gets the parameters of a core ALU instruction.
CoreAlu &InstructionInfo::getCoreAluParamters()
{
    validateInstructionClass(OperationClass::CoreAlu);

    return _params.CoreAluOp;
}

//! @brief Gets the parameters of a core ALU comparison instruction.
const CoreCompare &InstructionInfo::getCoreCompareParamters() const
{
    validateInstructionClass(OperationClass::CoreCompare);

    return _params.CoreCmpOp;
}

//! @brief Gets the parameters of a core ALU comparison instruction.
CoreCompare &InstructionInfo::getCoreCompareParamters()
{
    validateInstructionClass(OperationClass::CoreCompare);

    return _params.CoreCmpOp;
}

//! @brief Gets the parameters of an ADR pseudo-instruction.
const EffectiveAddress &InstructionInfo::getCoreAddressParameters() const
{
    validateInstructionClass(OperationClass::CoreAddress);

    return _params.CoreAdr;
}

//! @brief Gets the parameters of an ADR pseudo-instruction.
EffectiveAddress &InstructionInfo::getCoreAddressParameters()
{
    validateInstructionClass(OperationClass::CoreAddress);

    return _params.CoreAdr;
}

//! @brief Gets the parameters of a core ALU multiplication instruction.
const CoreMultiply &InstructionInfo::getCoreMultiplyParameters() const
{
    validateInstructionClass(OperationClass::CoreMultiply);

    return _params.CoreMulOp;
}

//! @brief Gets the parameters of a core ALU multiplication instruction.
CoreMultiply &InstructionInfo::getCoreMultiplyParameters()
{
    validateInstructionClass(OperationClass::CoreMultiply);

    return _params.CoreMulOp;
}

//! @brief Gets the parameters of a long multiplication instruction.
const LongMultiply &InstructionInfo::getLongMultiplyParameters() const
{
    validateInstructionClass(OperationClass::LongMultiply);

    return _params.LongMulOp;
}

//! @brief Gets the parameters of a long multiplication instruction.
LongMultiply &InstructionInfo::getLongMultiplyParameters()
{
    validateInstructionClass(OperationClass::LongMultiply);

    return _params.LongMulOp;
}


//! @brief Gets the parameters of a branch instruction.
const Branch &InstructionInfo::getBranchParameters() const
{
    validateInstructionClass(OperationClass::Branch);

    return _params.BranchOp;
}

//! @brief Gets the parameters of a branch instruction.
Branch &InstructionInfo::getBranchParameters()
{
    validateInstructionClass(OperationClass::Branch);

    return _params.BranchOp;
}

//! @brief Gets the parameters of a software interrupt instruction.
const SoftwareIrq &InstructionInfo::getSoftwareIrqParameters() const
{
    validateInstructionClass(OperationClass::SoftwareIrq);

    return _params.SoftwareIrqOp;
}

//! @brief Gets the parameters of a software interrupt instruction.
SoftwareIrq &InstructionInfo::getSoftwareIrqParameters()
{
    validateInstructionClass(OperationClass::SoftwareIrq);

    return _params.SoftwareIrqOp;
}

//! @brief Gets the parameters of the MSR instruction.
const MoveFromPsr &InstructionInfo::getMoveFromPsrParameters() const
{
    validateInstructionClass(OperationClass::MoveFromPSR);

    return _params.MoveFromPsrOp;
}

//! @brief Gets the parameters of the MSR instruction.
MoveFromPsr &InstructionInfo::getMoveFromPsrParameters()
{
    validateInstructionClass(OperationClass::MoveFromPSR);

    return _params.MoveFromPsrOp;
}

//! @brief gets parameters for the MRS instruction.
const MoveToPsr &InstructionInfo::getMoveToPsrParameters() const
{
    validateInstructionClass(OperationClass::MoveToPSR);

    return _params.MoveToPsrOp;
}

//! @brief gets parameters for the MRS instruction.
MoveToPsr &InstructionInfo::getMoveToPsrParameters()
{
    validateInstructionClass(OperationClass::MoveToPSR);

    return _params.MoveToPsrOp;
}

//! @brief Gets the parameters of a breakpoint interrupt instruction.
const Breakpoint &InstructionInfo::getBreakpointParameters() const
{
    validateInstructionClass(OperationClass::Breakpoint);

    return _params.BreakpointOp;
}

//! @brief Gets the parameters of a breakpoint interrupt instruction.
Breakpoint &InstructionInfo::getBreakpointParameters()
{
    validateInstructionClass(OperationClass::Breakpoint);

    return _params.BreakpointOp;
}

//! @brief Gets the parameters of a core single register data transfer
//! instruction.
const CoreDataTransfer &InstructionInfo::getCoreDataTransferParameters() const
{
    validateInstructionClass(OperationClass::CoreDataTransfer);

    return _params.DataTransOp;
}

//! @brief Gets the parameters of a core single register data transfer
//! instruction.
CoreDataTransfer &InstructionInfo::getCoreDataTransferParameters()
{
    validateInstructionClass(OperationClass::CoreDataTransfer);

    return _params.DataTransOp;
}

//! @brief Gets the parameters of a core multi-register data transfer
//! instruction.
const CoreMultiTransfer &InstructionInfo::getCoreMultiTransferParameters() const
{
    validateInstructionClass(OperationClass::CoreMultiTransfer);

    return _params.MultiTransOp;
}

//! @brief Gets the parameters of a core multi-register data transfer
//! instruction.
CoreMultiTransfer &InstructionInfo::getCoreMultiTransferParameters()
{
    validateInstructionClass(OperationClass::CoreMultiTransfer);

    return _params.MultiTransOp;
}

//! @brief Gets the parameters of an atomic swap instruction.
const CoreSwap &InstructionInfo::getCoreSwapParameters() const
{
    validateInstructionClass(OperationClass::AtomicSwap);

    return _params.AtomicSwapOp;
}

//! @brief Gets the parameters of an atomic swap instruction.
CoreSwap &InstructionInfo::getCoreSwapParameters()
{
    validateInstructionClass(OperationClass::AtomicSwap);

    return _params.AtomicSwapOp;
}

//! @brief Gets the parameters of a generic co-processor data processing
//! instruction.
const CoProcDataProcessing &InstructionInfo::getCoProcDataProcessingParameters() const
{
    validateInstructionClass(OperationClass::CoProcDataProcessing);

    return _params.CoProcDataProcOp;
}

//! @brief Gets the parameters of a generic co-processor data processing
//! instruction.
CoProcDataProcessing &InstructionInfo::getCoProcDataProcessingParameters()
{
    validateInstructionClass(OperationClass::CoProcDataProcessing);

    return _params.CoProcDataProcOp;
}

//! @brief Gets the parameters of a generic co-processor register transfer
//! instruction.
const CoProcRegisterTransfer &InstructionInfo::getCoProcRegisterTransferParameters() const
{
    validateInstructionClass(OperationClass::CoProcRegisterTransfer);

    return _params.CoProcRegTransOp;
}

//! @brief Gets the parameters of a generic co-processor register transfer
//! instruction.
CoProcRegisterTransfer &InstructionInfo::getCoProcRegisterTransferParameters()
{
    validateInstructionClass(OperationClass::CoProcRegisterTransfer);

    return _params.CoProcRegTransOp;
}

//! @brief Gets the parameters of a generic co-processor data transfer
//! instruction.
const CoProcDataTransfer &InstructionInfo::getCoProcDataTransferParameters() const
{
    validateInstructionClass(OperationClass::CoProcDataTransfer);

    return _params.CoProcDataTransferOp;
}

//! @brief Gets the parameters of a generic co-processor data transfer
//! instruction.
CoProcDataTransfer &InstructionInfo::getCoProcDataTransferParameters()
{
    validateInstructionClass(OperationClass::CoProcDataTransfer);

    return _params.CoProcDataTransferOp;
}

//! @brief Assembles the encoded instruction, if possible.
//! @param[out] instruction Receives the bit field encoding of the instruction.
//! @param[in] loadAddr The address at which the assembled instruction is
//! expected to execute.
//! @param[out] error Receives a message describing why the instruction
//! could not be assembled if the return value is false.
//! @retval true An instruction was successfully assembled.
//! @retval false The instruction specification was invalid, nothing was assembled.
bool InstructionInfo::assemble(uint32_t &instruction, uint32_t loadAddr,
                               Ag::string_ref_t error) const
{
    // Define the parameters for assembly.
    AssemblyParams params(_opClass, _mnemonic, _condition, loadAddr, &_params);

    // Perform assembly elsewhere (because it takes a lot of code).
    bool isOK = false;

    if (assembleInstruction(params))
    {
        if (params.InstructionCount != 1)
        {
            throw Ag::OperationException("The instruction was assembled to "
                                         "produce multiple words.");
        }
        else
        {
            instruction = params.Instructions[0];
            isOK = true;
        }
    }
    else
    {
        instruction = 0;
        error = params.ErrorMessage;
    }

    return isOK;
}

//! @brief Assembles the encoded instruction, if possible into one or more
//! machine code instructions.
//! @param[out] instructions An array which receives the bit fields encoding
//! of the instructions.
//! @param[in] loadAddr The address at which the assembled instruction is
//! expected to execute.
//! @param[in] maxCount The maximum number of instruction words which can be
//! stored in the array.
//! @param[out] error Receives a message describing why the instruction
//! could not be assembled if the return value is 0.
//! @returns The count of instruction words stored. At most this should be 4 to
//! store a maximal ADR directive.
size_t InstructionInfo::assemble(uint32_t *instructions, uint32_t loadAddr,
                                 size_t maxCount, Ag::string_ref_t error) const
{
    // Define the parameters for assembly.
    AssemblyParams params(_opClass, _mnemonic, _condition, loadAddr, &_params);

    size_t wordCount = 0;

    // Perform assembly elsewhere (because it takes a lot of code).
    if (assembleInstruction(params))
    {
        if (params.InstructionCount > maxCount)
        {
            throw Ag::OperationException("The instruction was too long to store.");
        }
        else
        {
            std::copy_n(params.Instructions, params.InstructionCount,
                        instructions);
            wordCount = params.InstructionCount;
        }
    }
    else
    {
        error = params.ErrorMessage;
    }

    return wordCount;
}

//! @brief Attempts to initialise the object by disassembling a machine
//! code instruction.
//! @param[in] instruction The instruction bit field to disassemble.
//! @param[in] loadAddress The address at which the disassembled instruction is
//! expected to be executed.
//! @param[in] flags A bit field defining how the instruction should be
//! interpreted using bits from the DisasmBits enumeration.
//! @retval true The bit field represented a valid instruction.
//! @retval false The bit field represented an invalid instruction encoding.
bool InstructionInfo::disassemble(uint32_t instruction, uint32_t loadAddress,
                                  uint32_t flags /* = All */)
{
    // Rest the object to an empty state.
    reset(InstructionMnemonic::MaxMnemonic,
          OperationClass::None,
          ConditionCode::Nv);

    // Prepare parameters for the disassembler.
    DisassemblyParams params(instruction, flags, loadAddress, &_params);

    // Perform disassembly somewhere else (as it requires quite a bit of code).
    _opClass = disassembleInstruction(params);

    if (_opClass != OperationClass::None)
    {
        // Copy over parameters on success.
        _mnemonic = params.Mnemonic;
        _condition = params.Condition;
    }

    return _opClass != OperationClass::None;
}

//! @brief Attempts to initialise the object by disassembling a sequence of
//! machine code instructions.
//! @param[in] instructions The instruction bit fields to disassemble.
//! @param[in] count The count of instruction words in instructions.
//! @param[in] loadAddress The address at which the disassembled instruction is
//! expected to be executed.
//! @param[in] flags A bit field defining how the instruction should be
//! interpreted using bits from the DisasmBits enumeration.
//! @returns The count of words successfully disassembled.
uint8_t InstructionInfo::disassemble(const uint32_t *instructions,
                                     uint8_t count, uint32_t loadAddress,
                                     uint32_t flags /*= 0x3F*/)
{
    // Rest the object to an empty state.
    reset(InstructionMnemonic::MaxMnemonic,
          OperationClass::None,
          ConditionCode::Nv);

    // Prepare parameters for the disassembler.
    DisassemblyParams params(instructions, count, flags, loadAddress, &_params);

    // Perform disassembly somewhere else (as it requires quite a bit of code).
    _opClass = disassembleInstruction(params);

    if (_opClass != OperationClass::None)
    {
        // Copy over parameters on success.
        _mnemonic = params.Mnemonic;
        _condition = params.Condition;
    }

    return (_opClass == OperationClass::None) ? 0u : params.Decoded;
}


//! @brief Outputs the instruction as text.
//! @param[in] options An optional object which specifies how the instruction
//! should be formatted and which can resolve symbols. Nullptr to use the
//! default options.
Ag::String InstructionInfo::toString(const FormatterOptions *options /*= nullptr*/) const
{
    static FormatterOptions defaultOptions(0x0000, FormatterOptions::ShowOffsets);

    // Prepare the formatting parameters.
    FormatParams params((options == nullptr) ? &defaultOptions : options,
                        &_params, _opClass, _mnemonic, _condition);

    // Format the instruction elsewhere (because it takes a lot of code).
    if (formatInstruction(params))
    {
        return Ag::String(params.Builder);
    }
    else
    {
        return Ag::String::Empty;
    }
}

//! @brief Outputs the instruction as text.
//! @param[in] loadAddr The address at which the instruction is executed.
//! @param[in] formatterOptionsFlags Formatting option flags as defined by
//! the FormatterOptions::Flags enumeration type.
Ag::String InstructionInfo::toString(uint32_t loadAddr, uint32_t formatterOptionsFlags) const
{
    FormatterOptions options(loadAddr, formatterOptionsFlags);

    return toString(&options);
}

//! @brief Resets the object to an empty state.
void InstructionInfo::clear()
{
    reset(InstructionMnemonic::MaxMnemonic,
          OperationClass::None,
          ConditionCode::Nv);
}

//! @brief Initialises the instruction with a type and default parameters.
//! @param[in] mnemonic The mnemonic of the instruction to encode.
//! @param[in] opClass The type of instruction being encoded.
//! @param[in] condition The condition code of the instruction.
void InstructionInfo::reset(InstructionMnemonic mnemonic, OperationClass opClass,
                            ConditionCode condition /*= ConditionCode::Al*/)
{
    // Zero the instruction parameters.
    _params.reset();

    _condition = condition;
    _mnemonic = mnemonic;
    _opClass = opClass;
}

//! @brief Ensures that the type of instruction parameters being accessed are
//! consistent with the instruction class.
//! @throws OperationException If the expected operation classification doesn't
//! match that which is currently assigned.
void InstructionInfo::validateInstructionClass(OperationClass classification) const
{
    if (_opClass != classification)
    {
        throw Ag::OperationException(
            "Getting the wrong type of instruction parameters for the instruction class.");
    }
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


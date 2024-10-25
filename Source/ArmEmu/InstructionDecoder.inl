//! @file ArmEmu/InstructionDecoder.inl
//! @brief The declaration of entities and a pattern for an instruction decoder
//! data type.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_INSTUCTION_DECODER_INL__
#define __ARM_EMU_INSTUCTION_DECODER_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmCore.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the pattern an InstructionDecoder implementation should
//! follow.
//! @tparam THardware The underlying hardware data transfer instructions might
//! access, preferably based on GenericHardware.
//! @tparam TRegisterFile The register file type instructions should use to
//! interact with the processor state, preferably based on
//! GenericCoreRegisterFile.
//! @tparam TInstruction The data type of an instruction word, generally either
//! uint32_t or uint16_t. This template parameter doesn't have to be explicitly
//! represented in implementations.
template<typename THardware, typename TRegisterFile, typename TInstruction>
struct GenericInstructionDecoder
{
public:
    // Public Types
    using HardwareType = THardware;
    using RegisterFileType = TRegisterFile;
    using InstructionWord = TInstruction;

private:
    // Internal Fields
    HardwareType &_hardware;
    RegisterFileType &_registers;
public:
    // Construction/Destruction

    //! @brief Constructs an object to decode and execute ARM instructions.
    //! @param hw The interface to the hardware data transfer instructions
    //! should interact with.
    //! @param regs The interface to the processor state most instructions
    //! should interact with.
    GenericInstructionDecoder(HardwareType &hw, RegisterFileType &regs) :
        _hardware(hw),
        _registers(regs)
    {
    }

    GenericInstructionDecoder() = delete;
    GenericInstructionDecoder(const GenericInstructionDecoder &) = delete;
    GenericInstructionDecoder(GenericInstructionDecoder &&) = delete;
    GenericInstructionDecoder &operator=(const GenericInstructionDecoder &) = delete;
    GenericInstructionDecoder &operator=(GenericInstructionDecoder &&) = delete;
    ~GenericInstructionDecoder() = default;

    // Operations
    //! @brief Decodes and executes the specified instruction making changes to
    //! the state of the emulated machine via the hardware and registers.
    //! @param[in] instruction The word defining the instruction to decode and
    //! execute.
    //! @return A cycle count and other flags based on the ExecResult structure.
    uint32_t decodeAndExecute(InstructionWord instruction)
    {
        // Return some value defined using ExecResult.
        return ExecResult::CycleCountMask;
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

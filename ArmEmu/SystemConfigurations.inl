//! @file ArmEmu/SystemConfigurations.inl
//! @brief The declaration of various traits types describing different
//! configurations of system to emulate.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_SYSTEM_CONFIGURATIONS_INL__
#define __ARM_EMU_SYSTEM_CONFIGURATIONS_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmCore.hpp"
#include "Hardware.inl"
#include "RegisterFile.inl"
#include "InstructionPipeline.inl"
#include "ExecutionUnit.inl"

#include "TestBedHardware.inl"
#include "ARMv2CoreRegisterFile.inl"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the model of a traits class describing the of configuration
//! a system to be emulated.
struct GenericSystemTraits
{
    // Public Types
    //! @brief The data type of the object which manages the physical address
    //! map and major hardware resources.
    using HardwareType = TestBedHardware;

    //! @brief The data type of the object which holds state of the processor
    //! in terms of register contents, this includes co-processor state.
    using RegisterFileType = ARMv2CoreRegisterFile<typename GenericSystemTraits::HardwareType>;

    //! @brief Specifies the traits of the primary mode of operation intended
    //! to create an appropriate instruction pipeline type.
    struct PrimaryPipelineTraits
    {
        using HardwareType = typename GenericSystemTraits::HardwareType;
        using RegisterFileType = typename GenericSystemTraits::RegisterFileType;
        using InstructionWordType = uint32_t; // or uint16_t
        static constexpr uint8_t InstructionSizePow2 = 2; // or 1
    };

    using PrimaryPipelineType = InstructionPipeline<typename GenericSystemTraits::PrimaryPipelineTraits>;

    //struct SecondaryPipelineTraits {};
    //using SecondaryPipelineType = InstructionPipeline<typename SecondaryPipelineTraits>;

    //struct TertiaryPipelineTraits {};
    //using TertiaryPipelineType = InstructionPipeline<typename TertiaryPipelineTraits>

    //! @brief The data type of the object which manages the instruction
    //! pipeline(s), i.e. one which can handle one, two or three instruction
    //! pipeline modes.
    using ExecutionUnitType = SingleModeExecutionUnit<typename GenericSystemTraits::HardwareType,
                                                      typename GenericSystemTraits::RegisterFileType,
                                                      typename GenericSystemTraits::PrimaryPipelineType>;
};

//! @brief Defines the traits of a basic ARMv2-based system with test bed hardware.
struct ArmV2TestSystemTraits
{
    // Public Types
    //! @brief The data type of the object which manages the physical address
    //! map and major hardware resources.
    using HardwareType = TestBedHardware;

    //! @brief The data type of the object which holds state of the processor
    //! in terms of register contents, this includes co-processor state.
    using RegisterFileType = ARMv2CoreRegisterFile<typename ArmV2TestSystemTraits::HardwareType>;

    struct PrimaryPipelineTraits
    {
        using HardwareType = ArmV2TestSystemTraits::HardwareType;
        using RegisterFileType = ArmV2TestSystemTraits::RegisterFileType;
        using InstructionWordType = uint32_t;
        static constexpr uint8_t InstructionSizePow2 = 2;
    };

    using PrimaryPipelineType = InstructionPipeline<typename ArmV2TestSystemTraits::PrimaryPipelineTraits>;

    using ExecutionUnitType = SingleModeExecutionUnit<typename ArmV2TestSystemTraits::HardwareType,
                                                      typename ArmV2TestSystemTraits::RegisterFileType,
                                                      typename ArmV2TestSystemTraits::PrimaryPipelineType>;
};


}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

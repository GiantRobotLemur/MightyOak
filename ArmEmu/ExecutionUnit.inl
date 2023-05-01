//! @file ArmEmu/ExecutionUnit.inl
//! @brief The declaration of a set of template components which manage the
//! emulated execution of ARM or Thumb instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_EXECUTION_UNIT_INL__
#define __ARM_EMU_EXECUTION_UNIT_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmCore.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief A template implementing execution of ARM instructions in a single
//! mode.
//! @tparam THardware The data type representing the memory map and hardware
//! modelled after GenericHardware.
//! @tparam The data type of the register file the execution unit accesses
//! modelled after GenericCoreRegisterFile.
//! @tparam TPrimaryPipeline The pipeline which executes instructions for the
//! single operating mode the execution units supports modelled on
//! InstructionPipeline.
template<typename THardware, typename TRegisterFile, typename TPrimaryPipeline>
class SingleModeExecutionUnit
{
public:
    // Public Types
    using PrimaryPipeline = TPrimaryPipeline;
    using Hardware = THardware;
    using RegisterFile = TRegisterFile;

private:
    // Internal Fields
    Hardware &_hardware;
    RegisterFile &_regs;
    PrimaryPipeline _pipeline;

public:
    // Construction/Destruction
    //! @brief Constructs an object which runs an instruction pipeline in
    //! particular operating mode.
    //! @param[in] hw The object providing access to the emulated memory map
    //! and hardware.
    //! @param[in] regs The object used to read and write the state of the
    //! emulated processor, possibly via an emulation layer (e.g. 26-bit on
    //! 32-bit).
    SingleModeExecutionUnit(Hardware &hw, RegisterFile &regs) :
        _hardware(hw),
        _regs(regs),
        _pipeline(_hardware, _regs)
    {
    }

    // Operations
    //! @brief Executes instructions until a host or debug interrupt is raised or
    //! after the first run if in single step mode.
    //! @param[in] singleStep True to only run the pipeline once, false to run
    //! until a host or debug interrupt is triggered.
    //! @returns The count of simulated CPU cycles executed before exit.
    ExecutionMetrics runPipeline(bool singleStep)
    {
        ExecutionMetrics metrics;

        // Ensure the pipeline only runs once in single-step mode.
        bool runPipeline = (singleStep == false);

        _pipeline.flushPipeline();

        // Clear any external interrupts before running.
        _hardware.setDebugIrq(false);
        _hardware.setHostIrq(false);

        // Capture the start time.
        Ag::MonotonicTicks startTime = Ag::HighResMonotonicTimer::getTime();
        uint32_t result = 0;

        do
        {
            // Read the state of unmasked IRQs which might upset things.
            uint8_t pendingIrqs = _hardware.getIrqStatus();

            if (pendingIrqs)
            {
                // Deal with interrupts, both internal and external.
                if (pendingIrqs & IrqState::HostIrqsMask)
                {
                    // Exit the pipeline without processing anything.
                    runPipeline = false;
                }
                else if (pendingIrqs & IrqState::FastIrqPending)
                {
                    // A fast interrupt has been signalled.
                    result = _regs.handleFirq();
                }
                else // if (pendingIrqs & IS_IrqPending)
                {
                    // A normal interrupt has been signalled.
                    result = _regs.handleIrq();
                }
            }
            else // if (pendingIrqs == 0)
            {
                // Decode and execute the next instruction.
                result = _pipeline.executeNext();

                // Update metrics.
                metrics.CycleCount += result & ExecResult::CycleCountMask;
                ++metrics.InstructionCount;
            } // if (pendingIrqs == 0)

            // TODO if (result & ExecResult::ModeChange) in a multi-pipeline
            // execution unit, switch pipelines.
        } while (runPipeline);

        // Capture the end time and therefore the duration of the run.
        metrics.ElapsedTime = Ag::HighResMonotonicTimer::getDuration(startTime);

        return metrics;
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

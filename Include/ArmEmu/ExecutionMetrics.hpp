//! @file ArmEmu/ExecutionMetrics.hpp
//! @brief The declaration of a structure which contains statistics gathered
//! by the execution pipeline of an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_EXECUTION_METRICS_HPP__
#define __ARM_EMU_EXECUTION_METRICS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Timer.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which describes the performance of the run of the
//! emulated instruction pipeline.
struct ExecutionMetrics
{
    // Public Types
    enum class Result
    {
        //! @brief No exist status has been set.
        Unset,

        //! @brief The exec() function exited due to the emulated processor
        //! executing a break point (BKPT) instruction.
        DebugIrq,

        //! @brief The exec() function exited due to an emulator interrupt
        //! raised by the host system.
        HostIrq,

        //! @brief The execSingleStep() function exited as expected.
        SingleStep,

        //! @brief An unexpected failure occurred from within the emulator.
        Failure,
    };

    // Public Fields
    //! @brief The count of emulated processor clock cycles used.
    uint64_t CycleCount;

    //! @brief The count of instructions executed.
    uint64_t InstructionCount;

    //! @brief The amount of physical time calculated using the
    //! High Resolution Monotonic timer.
    Ag::MonotonicTicks ElapsedTime;

    //! @brief Specifies the result of the last call to exec() or
    //! execSingleStep() IArmSystem member functions.
    Result ExecResult;

    // Construction
    ExecutionMetrics();

    // Accessors
    double calculateClockFrequency() const;
    double calculateSpeedInMIPS() const;

    // Operations
    void reset();
    ExecutionMetrics operator+(const ExecutionMetrics &rhs) const;
    ExecutionMetrics &operator+=(const ExecutionMetrics &rhs);
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

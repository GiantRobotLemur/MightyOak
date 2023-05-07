//! @file ArmEmu/ExecutionMetrics.cpp
//! @brief The definition of a structure which contains statistics gathered
//! by the execution pipeline of an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/ExecutionMetrics.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// ExecutionMetrics Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates an empty set of execution metrics.
ExecutionMetrics::ExecutionMetrics() :
    CycleCount(0),
    InstructionCount(0),
    ElapsedTime(0)
{
}

//! @brief Calculates the simulated processor clock frequency from the metrics.
//! @returns The simulated frequency in Hz (cycles per second).
double ExecutionMetrics::calculateClockFrequency() const
{
    double frequency = 0.0;

    if ((CycleCount > 0) && (ElapsedTime > 0))
    {
        // Express the elapsed time as a value in seconds.
        double timeSpan = Ag::HighResMonotonicTimer::getTimeSpan(ElapsedTime);

        // Calculate the simulated frequency in Hz.
        frequency = CycleCount / timeSpan;
    }

    return frequency;
}

//! @brief Calculates the simulated speed of the processor in Millions of
//! Instructions Per Second (MIPS)
//! @return The average count of instructions executed per second.
double ExecutionMetrics::calculateSpeedInMIPS() const
{
    double mips = 0.0;

    if ((InstructionCount > 0) && (ElapsedTime > 0))
    {
        double timeSpan = Ag::HighResMonotonicTimer::getTimeSpan(ElapsedTime);

        mips = InstructionCount / timeSpan;
        mips /= 1e6;
    }

    return mips;
}

//! @brief Resets all metric properties back to zero.
void ExecutionMetrics::reset()
{
    CycleCount = 0;
    InstructionCount = 0;
    ElapsedTime = 0;
}

//! @brief Calculates the sum of the current and another set of metrics.
//! @param[in] rhs The metrics to add to the current set.
//! @return The summed execution properties.
ExecutionMetrics ExecutionMetrics::operator+(const ExecutionMetrics &rhs) const
{
    ExecutionMetrics result = rhs;
    result.CycleCount += CycleCount;
    result.InstructionCount += InstructionCount;
    result.ElapsedTime += ElapsedTime;

    return result;
}

//! @brief Adds another set of metric properties to the current object.
//! @param[in] rhs The metrics to add to the current set.
//! @return A reference to the current set.
ExecutionMetrics &ExecutionMetrics::operator+=(const ExecutionMetrics &rhs)
{
    CycleCount += rhs.CycleCount;
    InstructionCount += rhs.InstructionCount;
    ElapsedTime += rhs.ElapsedTime;

    return *this;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


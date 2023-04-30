//! @file ArmEmu/ArmEmu.cpp
//! @brief The definition of public functions provided by the ArmEmu library.
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
#include <iterator>

#include "Ag/Core/Utils.hpp"
#include "ArmEmu.hpp"
#include "AsmTools.hpp"

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// IArmSystemDeleter Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Disposes of a dynamically allocated IArmSystem implementation.
//! @param[in] sys A pointer to the object to dispose of.
void IArmSystemDeleter::operator()(IArmSystem *sys) const
{
    if (sys != nullptr)
    {
        delete sys;
    }
}

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
        double timeSpan = HighResMonotonicTimer::getTimeSpan(ElapsedTime);

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
        double timeSpan = HighResMonotonicTimer::getTimeSpan(ElapsedTime);

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


////////////////////////////////////////////////////////////////////////////////
// ProcessorModeInfo Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to use as a look-up key.
//! @param[in] id The processor mode being looked up.
ProcessorModeInfo::ProcessorModeInfo(ProcessorMode id) :
    EnumSymbol<ProcessorMode>(id),
    _minArchVersion(0)
{
}

//! @brief Constructs a description of an ARM processor mode.
//! @param[in] id The identifier of the processor mode.
//! @param[in] symbol The internal symbol definition as text.
//! @param[in] displayName The symbol as text to be displayed to the user.
//! @param[in] description A description of the meaning of the symbol
//! which can be displayed to the user.
//! @note All strings should be static and UTF-8 encoded.
ProcessorModeInfo::ProcessorModeInfo(ProcessorMode id, const char *symbol,
                                     const char *displayName,
                                     const char *description,
                                     uint8_t minArchVersion) :
    EnumSymbol<ProcessorMode>(id, symbol, displayName, description),
    _minArchVersion(minArchVersion)
{
}

//! @brief Determines if the mode operates with 26-bit addressing.
bool ProcessorModeInfo::is26Bit() const
{
    return _minArchVersion < 3;
}

//! @brief Gets the minimum ARM architecture in which the mode is valid.
uint8_t ProcessorModeInfo::getMinimumArchitectureVersion() const
{
    return _minArchVersion;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets text identifying a CoreRegister value.
//! @param[in] regId The identifier of the register to get display text for.
//! @return The display text representing the register.
const char *coreRegisterToString(CoreRegister regId)
{
    static const char *coreRegIds[] = {
        "R0", "R1", "R2", "R3",
        "R4", "R5", "R6", "R7",
        "R8", "R9", "R10", "R11",
        "R12", "R13", "R14", "R15",
        "SPSR", "CPSR", "PC",
    };

    static_assert(arraySize(coreRegIds) == toScalar(CoreRegister::Max),
                  "CoreRegister type out of sync with strings.");

    if (regId < CoreRegister::Max)
    {
        return coreRegIds[toScalar(regId)];
    }
    else
    {
        return "(unknown Core Register)";
    }
}

//! @brief Gets static metadata for the ProcessorMode enumeration.
const ProcessorModeEnumInfo &getProcessorModeType()
{
    static const ProcessorModeEnumInfo instance({
        ProcessorModeInfo(ProcessorMode::User26, "USR26", "User Mode (26-bit)", nullptr, 2),
        ProcessorModeInfo(ProcessorMode::FastIrq26, "FIQ26", "Fast Interrupt Mode (26-bit)", nullptr, 2),
        ProcessorModeInfo(ProcessorMode::Irq26, "IRQ26", "Interrupt Mode (26-bit)", nullptr, 2),
        ProcessorModeInfo(ProcessorMode::Svc26, "SVC26", "Supervisor Mode (26-bit)", nullptr, 2),
        ProcessorModeInfo(ProcessorMode::User32, "USR32", "User Mode (32-bit)", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::FastIrq32, "FIQ32", "Fast Interrupt Mode (32-bit)", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::Irq32, "IRQ32", "Interrupt Mode (32-bit)", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::Svc32, "SVC32", "Supervisor Mode (32-bit)", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::Abort, "ABT", "Abort Mode", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::Undefined, "UND", "Undefined Mode", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::System, "SYS", "System Mode", nullptr, 4), // ARMv4+
    });

    return instance;
}

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


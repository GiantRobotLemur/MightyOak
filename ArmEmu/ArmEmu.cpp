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

#include "SystemConfigurations.inl"
#include "ArmSystem.inl"

namespace Ag {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct GenerateBreakPoint
{
private:
    Asm::InstructionInfo _instruction;
    uint16_t _index;
public:
    GenerateBreakPoint() :
        _index(0)
    {
        _instruction.reset(Asm::InstructionMnemonic::Bkpt,
                           Asm::OperationClass::Breakpoint);
    }

    uint32_t operator()()
    {
        _instruction.getBreakpointParameters().Comment = _index++;

        String error;
        uint32_t instruction = 0;
        if (_instruction.assemble(instruction, 0x0000, error) == false)
        {
            throw Ag::OperationException("Could not assemble numbered breakpoint.");
        }

        return instruction;
    }
};

} // Anonymous namespace

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
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an implementation of an object representing an emulation
//! of an ARM-based system for testing to run specified code assembled at the
//! 32KB address mark in user mode.
//! @param[in] assembler The assembly language source code to assemble into
//! the RAM at 32KB and run up to the first breakpoint.
IArmSystemUPtr createUserModeTestSystem(const char *assembler)
{
    // Assemble code into emulated RAM.
    Asm::Options opts;
    opts.setLoadAddress(TestBedHardware::RamBase);
    opts.setInstructionSet(Asm::InstructionSet::ArmV4);

    std::string source;
    source.append("%26bit\n");
    source.append("TSTP PC,#0\n"); // Force a switch to User mode with IRQs enabled.
    source.append(assembler);

    Asm::Messages log;
    Asm::ObjectCode ramObjectCode = Asm::assembleText(source, opts, log);

    if (log.hasErrors())
    {
        std::string builder;

        for (const auto &msg : log.getMessages())
        {
            if (builder.empty() == false)
            {
                builder.push_back('\n');
            }

            appendAgString(builder, msg.toString());
        }

        throw Ag::CustomException("Assembly", "Could not assemble test code.",
                                  builder);
    }

    // Create a ROM image filled with breakpoints.
    std::vector<uint32_t> rom;

    std::generate_n(std::back_inserter(rom),
                    TestBedHardware::RomSize / 4,
                    GenerateBreakPoint());

    // Create an instruction which at the hardware reset vector which
    // branches to the first work in memory.
    Asm::InstructionInfo resetBranch(Asm::InstructionMnemonic::B,
                                     Asm::OperationClass::Branch);
    resetBranch.getBranchParameters().Address = TestBedHardware::RamBase;

    String error;
    if (resetBranch.assemble(rom.front(), TestBedHardware::RomBase, error) == false)
    {
        throw Ag::OperationException("Could not assemble reset vector.");
    }

    //TestSystem *testSystem = new TestSystem();
    auto *testSystem = new ArmSystem<ArmV2TestSystemTraits>();

    // Fill the ROM with breakpoints and a branch to RAM on reset.
    testSystem->writeToLogicalAddress(TestBedHardware::RomBase,
                                      static_cast<uint32_t>(rom.size() * 4),
                                      rom.data());

    // Copy the assembled code into RAM.
    testSystem->writeToLogicalAddress(TestBedHardware::RamBase,
                                      static_cast<uint32_t>(ramObjectCode.getCodeSize()),
                                      ramObjectCode.getCode());

    // Assemble a breakpoint at the end of the program.
    resetBranch.reset(Asm::InstructionMnemonic::Bkpt,
                      Asm::OperationClass::Breakpoint);
    resetBranch.getBreakpointParameters().Comment = 0xFFFF;

    uint32_t bkptInstruction;
    if (resetBranch.assemble(bkptInstruction, 0x0000, error) == false)
    {
        throw Ag::OperationException("Could not assemble final break point.");
    }

    // Write the final breakpoint to RAM.
    testSystem->writeToLogicalAddress(static_cast<uint32_t>(TestBedHardware::RamBase + ramObjectCode.getCodeSize()),
                                      4, &bkptInstruction);

    // Return a unique pointer to the emulated system.
    return IArmSystemUPtr(testSystem);
}

//! @brief Creates an instance of the test system with a program loaded into
//! the bottom of the RAM and a descending stack created at the top.
//! @param program The machine code of the program to load.
//! @param byteCount The count of bytes in the program.
//! @return The test system instance in a ready-to-run state.
IArmSystemUPtr createEmbeddedTestSystem(const uint8_t *program, size_t byteCount)
{
    // Create a ROM image filled with breakpoints.
    std::vector<uint32_t> rom;

    std::generate_n(std::back_inserter(rom),
                    TestBedHardware::RomSize / 4,
                    GenerateBreakPoint());

    // Create an instruction which at the hardware reset vector which
    // branches to the first work in memory.
    Asm::InstructionInfo resetBranch(Asm::InstructionMnemonic::B,
                                     Asm::OperationClass::Branch);
    resetBranch.getBranchParameters().Address = TestBedHardware::RamBase;

    String error;
    if (resetBranch.assemble(rom.front(), TestBedHardware::RomBase, error) == false)
    {
        throw Ag::OperationException("Could not assemble reset vector.");
    }

    auto *testSystem = new ArmSystem<ArmV2TestSystemTraits>();

    // Fill the ROM with breakpoints and a branch to RAM on reset.
    testSystem->writeToLogicalAddress(TestBedHardware::RomBase,
                                      static_cast<uint32_t>(rom.size() * 4),
                                      rom.data());

    // Copy the assembled code into RAM.
    testSystem->writeToLogicalAddress(TestBedHardware::RamBase,
                                      static_cast<uint32_t>(byteCount),
                                      program);

    // Assemble a breakpoint at the end of the program.
    resetBranch.reset(Asm::InstructionMnemonic::Bkpt,
                      Asm::OperationClass::Breakpoint);
    resetBranch.getBreakpointParameters().Comment = 0xFFFF;

    uint32_t bkptInstruction;
    if (resetBranch.assemble(bkptInstruction, 0x0000, error) == false)
    {
        throw Ag::OperationException("Could not assemble final break point.");
    }

    // Write the final breakpoint to RAM.
    testSystem->writeToLogicalAddress(static_cast<uint32_t>(TestBedHardware::RamBase + byteCount),
                                      4, &bkptInstruction);

    // Setup a full-descending stack in R13 after the reset.
    uint32_t ramEnd = TestBedHardware::RamEnd - 4;
    testSystem->setCoreRegister(CoreRegister::R13, ramEnd);

    // Return a unique pointer to the emulated system.
    return IArmSystemUPtr(testSystem);
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


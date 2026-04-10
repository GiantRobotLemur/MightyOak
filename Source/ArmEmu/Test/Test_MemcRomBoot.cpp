//! @file ArmEmu/Test/Test_MemcRomBoot.cpp
//! @brief The definition of unit tests which exercise the MEMC-based system
//! boot sequence using the embedded MemcTestSystemRom.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "AsmTools.hpp"

#include <iostream>
#include <sstream>
#include <mutex>
#include <thread>

#include "ArmSystem.inl"
#include "SystemConfigurations.inl"
#include "MEMC.inl"
#include "MemcTestRom.hpp"
#include "ArmEmu/RingBufferTrace.hpp"
#include "ArmEmu/BootProgressMonitor.hpp"
#include "ArmEmu/CompositeDiagnosticSink.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief The concrete MEMC system type, which enables diagnostics, used for testing.
using MemcSystem = ArmSystem<ArmV2MemcSystemTraits<true>>;

//! @brief A test fixture which creates a MEMC-based system loaded with the
//! embedded test ROM and provides helpers for stepping/running.
class MemcRomBootTests : public ::testing::Test
{
private:
    std::unique_ptr<MemcSystem> _system;
    ExecutionMetrics _lastResult;
    uint32_t _lastPC;
    uint32_t _lastBkPt;

protected:

    void analyseExecution()
    {
        _lastBkPt = ~0u;
        _lastPC = _system->getCoreRegister(CoreRegister::PC);

        if (_lastResult.ExecResult == ExecutionMetrics::Result::DebugIrq)
        {
            // Extract the BKPT instruction which raised the debug IRQ.
            uint32_t bkpt = 0;

            if (Arm::readFromLogicalAddress(_system.get(), _lastPC - 4, &bkpt, 4) == 4)
            {
                Asm::InstructionInfo instruction;

                if (instruction.disassemble(bkpt, _lastPC - 4, Asm::InstructionInfo::ARMv2a) &&
                    (instruction.getMnemonic() == Asm::InstructionMnemonic::Bkpt))
                {
                    _lastBkPt = instruction.getBreakpointParameters().Comment;
                }
            }
        }
    }

    const ExecutionMetrics &getLastResult() const { return _lastResult; }

    uint32_t getLastPC() const { return _lastPC; }

    uint32_t getLastBreakPointID() const { return _lastBkPt; }

    bool tryGetLastBreakPointID(uint32_t &id) const
    {
        id = _lastBkPt;

        return id != ~0u;
    }

    IArmSystem *getSystem() { return _system.get(); }

    //! @brief The maximum number of single steps to run before considering
    //! the test to have timed out.
    static constexpr int32_t MaxSteps = 50000;

    //! @brief The base address on the guest system at which significant values
    //! are stored.
    static constexpr uint32_t OS_Workspace = 0x8000;

    enum class BootState : uint32_t
    {
        Started,        // The boot - up process has started
        MemDetected,    // OS workspace set up, RAM size detected.
        StacksInit,     // The various processor mode stacks are set.
        HwVectorsSet,   // Hardware vectors initialised in RAM.
        CPUDetected,    // CPU detection complete.
        IOCInit,        // IOC initialisation complete.
        KRTInit,        // KART initialisation complete.
        KbdInit,        // Keyboard initialisation complete.

        Complete = 100, // The system halted on completing boot.
    };

    //! @brief A structure reflecting the guest system workspace.
    struct GuestWorkspace
    {
        BootState State;
        uint32_t DetectedRamSize;
        uint32_t SysCtrlID;
        uint32_t KrtTxHead;
        uint32_t KrtTxTail;
        uint32_t KrtRxHead;
        uint32_t KrtRxTail;
    };

    //! @brief The base physical address of the low ROM.
    static constexpr uint32_t LowRomBase = MEMC::LowRomStart; // 0x3400000

    static constexpr uint32_t RamSizeKb = 1024;

    MemcRomBootTests() :
        _lastPC(0),
        _lastBkPt(~0u)
    {
    }

    IArmSystem *createSystem(IHardwareDeviceUPtr &&extraDevice)
    {
        if (_system)
            return _system.get();

        Options opts;
        opts.setHardwareArchitecture(SystemModel::Archimedies);
        opts.setProcessorVariant(ProcessorModel::ARM2);
        opts.setSystemRom(SystemROMPreset::Custom);
        opts.setRamSizeKb(RamSizeKb);

        HardwareDevicePool devicePool;

        if (extraDevice)
            devicePool.emplace_back(std::move(extraDevice));

        _system = std::make_unique<MemcSystem>(opts, std::move(devicePool),
                                               AddressMap(), AddressMap(), nullptr);

        // Load the embedded test ROM into low ROM.
        size_t romByteCount;
        const uint8_t *romData = getMemcTestRomData(romByteCount);

        _system->getHardare().setHighRom(romData, romByteCount);

        // Re-reset so that the CPU starts from address 0 with the ROM data
        // in place for the initial page mappings.
        _system->reset();

        return _system.get();
    }

    IArmSystem *createSystem()
    {
        return createSystem({ });
    }

    //! @brief Runs the system for up to maxSteps single steps, stopping early
    //! if the predicate returns true.
    //! @param[in] predicate A callable taking the system pointer and returning
    //! true when execution should stop.
    //! @param[in] maxSteps The maximum number of steps to execute.
    //! @return The number of steps actually executed.
    template<typename Predicate>
    uint32_t runUntil(Predicate predicate, uint32_t maxSteps = MaxSteps)
    {
        auto sysToRun = createSystem();

        uint32_t steps = 0;

        for (; steps < maxSteps; ++steps)
        {
            _lastResult = sysToRun->runSingleStep();

            if (_lastResult.ExecResult == ExecutionMetrics::Result::DebugIrq)
            {
                // Hit a BKPT instruction.
                break;
            }

            if (predicate(sysToRun))
            {
                break;
            }
        }

        analyseExecution();
        return steps;
    }

    //! @brief Runs the system for a fixed number of single steps.
    //! @param[in] count The number of steps to execute.
    //! @return The execution result of the last step.
    ExecutionMetrics::Result runSteps(uint32_t count)
    {
        auto sysToRun = createSystem();
        ExecutionMetrics::Result lastResult = ExecutionMetrics::Result::Unset;

        for (uint32_t i = 0; i < count; ++i)
        {
            _lastResult = sysToRun->runSingleStep();
            lastResult = _lastResult.ExecResult;

            if (lastResult == ExecutionMetrics::Result::DebugIrq)
            {
                break;
            }
        }

        analyseExecution();
        return lastResult;
    }

    //! @brief Runs in a separate thread to implement a timeout.
    //! @param[in] systemToWatch The emulator to raise a host IRQ on at timeout.
    //! @param[in] isRunning The lock indicating if the emulator is still running.
    //! @param[in] timeoutMs The maximum amount of time the emulator can run for.
    static void watchDogThread(IArmSystem *systemToWatch,
                               std::timed_mutex *isRunning,
                               uint32_t timeoutMs)
    {
        if (!isRunning->try_lock_until(std::chrono::steady_clock::now() +
                                       std::chrono::milliseconds(timeoutMs)))
        {
            // We failed to acquire the lock, the emulated system has run away.
            systemToWatch->raiseHostInterrupt();
        }
    }

    //! @brief Runs the system until the PC stops changing (branch to self) or
    //! a BKPT is hit.
    //! @return True if a BKPT was hit, false if the PC was stuck.
    bool runUntilHaltOrBkpt(int32_t maxSteps = MaxSteps, uint32_t timeoutMs = 30000)
    {
        std::timed_mutex isRunning;
        std::thread watchdog;

        auto sysToRun = createSystem();

        if (sysToRun != nullptr)
        {
            // Acquire the lock while the emulator is running.
            std::lock_guard guard(isRunning);

            // Run a watchdog thread to monitor the lock and trigger a
            // host IRQ after a timeout.
            watchdog = std::thread(watchDogThread, sysToRun, &isRunning, timeoutMs);

            // Run the emulator.
            _lastResult = sysToRun->runLimited(maxSteps);
            analyseExecution();
        }

        // Ensure the watchdog thread exits.
        if (watchdog.joinable())
            watchdog.join();

        return (_lastResult.ExecResult == ExecutionMetrics::Result::DebugIrq);
    }

    //! @brief Reads the OS workspace data from the guest system.
    //! @param[out] workspace Receives the workspace data.
    //! @retval true The data was successfully read.
    //! @retval false The data could not be read.
    bool tryGetGuestWorkspace(GuestWorkspace &workspace)
    {
        Ag::zeroFill(workspace);

        uint32_t bytesRead = readFromLogicalAddress(_system.get(), OS_Workspace,
                                                    &workspace, sizeof(workspace));

        return bytesRead == sizeof(workspace);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
//! @brief Verify that after reset, the PC advances from address 0 into the
//! ROM branch sequence, confirming low ROM is correctly mapped to logical
//! address 0.
TEST_F(MemcRomBootTests, ResetVectorExecutesFromRom)
{
    // After reset, the PC should be at 0 (the reset vector).
    // The reset vector contains B Initial_Reset, which should branch into ROM.
    // Run a few steps to verify the PC advances into the ROM code.
    ExecutionMetrics::Result result = runSteps(3);

    EXPECT_NE(result, ExecutionMetrics::Result::DebugIrq)
        << "Unexpected BKPT #" << getLastBreakPointID() <<
        " hit during initial reset sequence.";

    // After 3 steps (B Initial_Reset; ADR R0,Reset_MEMC; ADD R0,R0,#0x3400000)
    // the PC should still be in the low-mapped ROM area (0x00000000 - 0x00FFFFFF
    // from the initial 4MB page mapping).
    uint32_t pc = getLastPC();
    EXPECT_GT(pc, 0u) << "PC did not advance from the reset vector.";
    EXPECT_LT(pc, 0x400000u) << "PC is outside the expected ROM range.";
}

//! @brief Run until the PC reaches the physical ROM region (>= 0x3400000),
//! confirming the initial page mapping works and the MOV PC,R0 jump succeeds.
TEST_F(MemcRomBootTests, JumpsToPhysicalRom)
{
    // The ROM's Initial_Reset does:
    //   ADR R0,Reset_MEMC
    //   ADD R0,R0,#0x3400000
    //   MOV PC,R0
    // After which the PC should be >= 0x3400000.
    uint32_t steps = runUntil([](IArmSystem *sys) {
        uint32_t pc = sys->getCoreRegister(CoreRegister::PC);
        return pc >= MEMC::HighRomStart;
    }, 10);

    uint32_t pc = getLastPC();
    EXPECT_GE(pc, MEMC::HighRomStart)
        << "PC did not reach physical ROM region after " << steps << " steps.";
    EXPECT_LT(pc, MEMC::AddrSpaceEnd)
        << "PC is beyond the high ROM region.";
    EXPECT_LT(steps, 10u)
        << "Jump to physical ROM took too many steps.";
}

//! @brief Run the full boot sequence and verify the RAM size detection result.
//! After the ROM completes memory sizing, R11 should contain the detected RAM
//! size and the value should also be stored at the OS_Workspace address (0x8000).
TEST_F(MemcRomBootTests, MemorySizeDetection)
{
    // Run until the system halts (B $ or BKPT).
    runUntilHaltOrBkpt();

    GuestWorkspace workspace;

    ASSERT_TRUE(tryGetGuestWorkspace(workspace));
    ASSERT_GE(workspace.State, BootState::MemDetected);

    // The default Options RAM size is <RamSizeKb> KB.
    EXPECT_EQ(workspace.DetectedRamSize, RamSizeKb << 10)
        << "Detected RAM size incorrect: " << workspace.DetectedRamSize;
}

//! @brief After the ROM programs MEMC for 8KB pages and identity-maps the first
//! 1MB, verify that logical addresses in the low 1MB range can be read/written.
TEST_F(MemcRomBootTests, MEMCControlRegisterProgrammed)
{
    // Run until the system halts.
    runUntilHaltOrBkpt();

    // After the boot sequence, MEMC should be configured for 8KB pages
    // and the first 1MB should be identity-mapped. Verify by reading back
    // values stored at OS_Workspace (0x8000) and comparing it against
    // values stored at OS_Workspace + 16MB.
    GuestWorkspace workspace;

    ASSERT_TRUE(tryGetGuestWorkspace(workspace));
    ASSERT_GE(workspace.State, BootState::MemDetected);

    constexpr uint32_t SampleWordCount = 4;
    uint32_t logicalMemSample[SampleWordCount];
    uint32_t physicalMemSample[SampleWordCount];

    uint32_t bytesRead = readFromLogicalAddress(getSystem(), OS_Workspace,
                                                logicalMemSample,
                                                SampleWordCount * sizeof(uint32_t));

    // The value should be non-zero (it's the RAM size and other bits).
    ASSERT_EQ(bytesRead, SampleWordCount * sizeof(uint32_t))
        << "Could not read from identity-mapped logical addresses.";

    // Verify the same bytes exist in physically mapped RAM.
    bytesRead = readFromPhysicalAddress(getSystem(), OS_Workspace + MEMC::PhysRamStart,
                                        physicalMemSample,
                                        SampleWordCount * sizeof(uint32_t));

    ASSERT_EQ(bytesRead, SampleWordCount * sizeof(uint32_t))
        << "Could not read from physical addresses.";

    for (uint32_t i = 0; i < SampleWordCount; ++i)
    {
        EXPECT_EQ(logicalMemSample[i], physicalMemSample[i]) << "Word " << i <<
            " read from physical and logical addresses don't match.";
    }

    // Also verify we can read from another address in the mapped range.
    // The stacks are set up at known addresses, so the SVC stack area
    // (below 0x8000) should be accessible.
    uint32_t stackValue = 0;
    bytesRead = readFromLogicalAddress(getSystem(), 0x4000,
                                       &stackValue, 4);
    EXPECT_EQ(bytesRead, 4u)
        << "Could not read from logical address in mapped range.";
}

//! @brief After the ROM installs RAM-based vectors, verify that the words at
//! addresses 0x00-0x1C contain LDR PC,[PC,#24] instructions (0xE59FF018) and
//! that addresses 0x20-0x3C contain handler addresses pointing into the ROM.
TEST_F(MemcRomBootTests, HardwareVectorsInstalledInRAM)
{
    // Run until the system halts.
    runUntilHaltOrBkpt();

    GuestWorkspace workspace;

    ASSERT_TRUE(tryGetGuestWorkspace(workspace));
    ASSERT_GE(workspace.State, BootState::HwVectorsSet);

    // Obtain the HW vector table contents.
    constexpr uint32_t HWVectorCount = 8;
    uint32_t hwVectors[HWVectorCount];
    uint32_t vectorTargets[HWVectorCount];

    uint32_t bytesRead = readFromLogicalAddress(getSystem(), 0, &hwVectors, sizeof(hwVectors));
    ASSERT_EQ(bytesRead, sizeof(hwVectors));
    bytesRead = readFromLogicalAddress(getSystem(), 32, &vectorTargets, sizeof(vectorTargets));
    ASSERT_EQ(bytesRead, sizeof(vectorTargets));

    for (uint32_t i = 0; i < HWVectorCount; ++i)
    {
        uint32_t addr = i * 4;
        uint32_t handlerAddr = vectorTargets[i];

        // Ensure the vector table entry contains LDR PC,[PC,#24]
        //   to load branch to the address stored 32-bytes after the
        //   current vector table entry.
        EXPECT_EQ(hwVectors[i], 0xE59FF018);

        // Handler addresses should point into the ROM region (>= 0x3800000).
        EXPECT_GE(handlerAddr, MEMC::HighRomStart)
            << "Handler address at 0x" << std::hex << addr
            << " points to 0x" << handlerAddr
            << ", expected ROM region (>= 0x"
            << std::hex << MEMC::HighRomStart << ").";
        EXPECT_LT(handlerAddr, MEMC::AddrSpaceEnd)
            << "Handler address at 0x" << std::hex << addr
            << " points to 0x" << handlerAddr
            << ", expected low ROM region (< 0x"
            << std::hex << MEMC::AddrSpaceEnd << ").";
    }
}

//! @brief Run the full boot sequence until it halts. The ROM ends with `B $`
//! after completing: reset -> jump to ROM -> size RAM -> program MEMC ->
//! set up stacks -> install vectors -> detect CPU -> initialise IOC.
//! Verify the system reaches the halt rather than timing out.
TEST_F(MemcRomBootTests, IOCInitCompletes)
{
    // Run until the system halts (B $ or BKPT).
    // A BKPT indicates an unexpected exception was taken.
    // B $ (branch to self) indicates normal completion.
    runUntilHaltOrBkpt();
    GuestWorkspace workspace;

    ASSERT_TRUE(tryGetGuestWorkspace(workspace));
    ASSERT_GE(workspace.State, BootState::IOCInit);

    // Verify the PC is in the ROM region (the B $ is in the IOC init code).
    uint32_t pc = getLastPC();
    EXPECT_GE(pc, MEMC::HighRomStart)
        << "PC is not in the ROM region at halt.";
    EXPECT_LT(pc, MEMC::AddrSpaceEnd)
        << "PC is beyond the high ROM region at halt.";
}

//! @brief Unit test for the Cinit fix: write to MEMC address space with
//! Cinit encoding, verify getCursorInitAddr() returns the expected value.
TEST_F(MemcRomBootTests, CinitRegisterStored)
{
    // Create a standalone MemcHardware to test the Cinit register directly.
    using MemcType = typename MemcHardware<false>;
    AddressMap readDevices, writeDevices;
    MemcType memc(Options(), readDevices, writeDevices);
    IHardwareDeviceCollection devices;
    MemcType::VIDCType *specimen = nullptr;

    memc.addIntegralHardware(devices);

    for (auto device : devices)
    {
        if (Ag::tryCast(device, specimen))
            break;
    }

    ASSERT_NE(specimen, nullptr) << "VIDC10 instance not found!";

    memc.reset();
    memc.setPrivilegedMode(true);

    // Verify initial value is 0.
    EXPECT_EQ(specimen->getCursorInitAddr(), 0u);

    // Write a Cinit register value. The MEMC register encoding for Cinit is:
    // Base address 0x3600000, register ID = 3 (bits [19:17]).
    // Cinit = case 3, so bits [19:17] = 011 = 0x60000.
    // The address encodes the value: _cursorInitAddr = offset & 0x1FFFC.
    // Offset for Cinit: 0x3600000 | (3 << 17) = 0x3660000
    // With a value encoded in the lower bits: 0x3660000 | 0x1234C
    uint32_t memcAddr = 0x3660000 | 0x1234C;
    EXPECT_TRUE(memc.write<uint32_t>(memcAddr, 0));

    EXPECT_EQ(specimen->getCursorInitAddr(), 0x1234Cu)
        << "Cinit register did not store the expected value.";

    // Write a different value.
    memcAddr = 0x3660000 | 0x1FFF8;
    EXPECT_TRUE(memc.write<uint32_t>(memcAddr, 0));

    EXPECT_EQ(specimen->getCursorInitAddr(), 0x1FFF8u)
        << "Cinit register did not update to the new value.";

    // Verify reset clears Cinit.
    memc.reset();
    EXPECT_EQ(specimen->getCursorInitAddr(), 0u)
        << "Cinit register was not cleared on reset.";
}

//! @brief Run a moderate number of steps to verify the memory size detection
//! and MEMC programming phases don't crash.
TEST_F(MemcRomBootTests, BootRunsToCompletion)
{
    // Run the full boot sequence.
    runUntilHaltOrBkpt(100000000);

    GuestWorkspace workspace;

    ASSERT_TRUE(tryGetGuestWorkspace(workspace));
    EXPECT_EQ(workspace.State, BootState::Complete) <<
        "The boot process didn't run to completion, only to state #" <<
        Ag::toScalar(workspace.State);

    uint32_t pc = getLastPC();

    EXPECT_GE(pc, MEMC::HighRomStart)
        << "PC is not in the ROM region at halt.";
}

//! @brief Run the boot sequence with diagnostic sinks attached and verify
//! that the trace and boot monitor capture meaningful data.
TEST_F(MemcRomBootTests, DiagnosticBootTrace)
{
    RingBufferTrace trace(4096);
    BootProgressMonitor monitor;
    auto compSinkUPtr = std::make_unique<CompositeDiagnosticSink>(CompositeDiagnosticSink({ &trace, &monitor }));
    auto sys = createSystem(std::move(compSinkUPtr));

    // Ensure the sink was integrated into the system.
    CompositeDiagnosticSink *compSink = nullptr;

    EXPECT_TRUE(sys->tryFindTypedDevice("DiagnosticSink", compSink));
    EXPECT_NE(compSink, nullptr);

    // Run the full boot sequence with diagnostics.
    runUntilHaltOrBkpt(100000000);

    // Verify the trace captured instructions.
    EXPECT_GT(trace.getInstructionCount(), 0u)
        << "No instructions were captured by the trace.";

    // Verify the boot monitor detected progress.
    EXPECT_GT(monitor.getInstructionCount(), 0u)
        << "Boot monitor did not count any instructions.";

    // We should have seen IOC accesses during boot (the IOC is accessed
    // through the MMIO path and therefore captured by the diagnostic sink).
    EXPECT_GT(monitor.getIocAccessCount(), 0u)
        << "No IOC accesses detected during boot.";

    // Dump the boot progress to test output for inspection.
    std::ostringstream oss;
    monitor.printSummary(oss);
    oss << "\n";
    trace.dumpToStream(oss);

    // Print to test output so it can be reviewed.
    std::cout << oss.str() << std::endl;

    // The boot should have reached at least IOCInit.
    GuestWorkspace workspace;
    ASSERT_TRUE(tryGetGuestWorkspace(workspace));
    EXPECT_GE(workspace.State, BootState::IOCInit)
        << "Boot with diagnostics attached did not reach IOC init.";
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

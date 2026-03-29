//! @file ArmEmu/Test/Test_MemcModeSwitch.cpp
//! @brief The definition of unit tests which exercise ARM2 mode switching,
//! SWI handling, and TEQP behaviour using a custom test ROM.
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

#include <iostream>
#include <mutex>
#include <thread>

#include "ArmSystem.inl"
#include "SystemConfigurations.inl"
#include "MEMC.inl"
#include "ModeTestRom.hpp"
#include "TestFeedbackDevice.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
using MemcSystem = ArmSystem<ArmV2MemcSystemTraits<true>>;

//! @brief Milestone codes reported by the test ROM via the feedback device.
enum Milestone : uint32_t
{
    MS_BootStart       = 0x01,
    MS_BootComplete    = 0x02,

    MS_Test1_Begin     = 0x10,
    MS_Test1_Complete  = 0x11,

    MS_Test2_Begin     = 0x20,
    MS_Test2_Complete  = 0x21,

    MS_Test3_Begin     = 0x30,
    MS_Test3_Complete  = 0x31,

    MS_Test4_Begin     = 0x40,
    MS_Test4_Complete  = 0x41,

    MS_Test5_Begin     = 0x50,
    MS_Test5_Complete  = 0x51,

    MS_Test6_Begin     = 0x60,
    MS_Test6_Complete  = 0x61,

    MS_Test7_Begin     = 0x70,
    MS_Test7_Complete  = 0x71,

    MS_Test8_Begin     = 0x80,
    MS_Test8_Complete  = 0x81,

    MS_AllPassed       = 0xFF,
};

//! @brief A test fixture for mode switching tests using the custom ROM and
//! TestFeedbackDevice.
class MemcModeSwitchTests : public ::testing::Test
{
private:
    std::unique_ptr<MemcSystem> _system;
    TestFeedbackDevice *_feedback;
    ExecutionMetrics _lastResult;
    uint32_t _lastPC;

protected:
    MemcModeSwitchTests() :
        _feedback(nullptr),
        _lastPC(0)
    {
    }

    IArmSystem *getSystem() { return _system.get(); }

    TestFeedbackDevice *getFeedback() { return _feedback; }

    const ExecutionMetrics &getLastResult() const { return _lastResult; }

    uint32_t getLastPC() const { return _lastPC; }

    IArmSystem *createSystem()
    {
        if (_system)
            return _system.get();

        // Create the feedback device.
        auto feedbackUPtr = std::make_unique<TestFeedbackDevice>();
        _feedback = feedbackUPtr.get();

        // Set up address maps with the feedback device in I/O space.
        AddressMap readMap, writeMap;
        readMap.tryInsert(TestFeedbackDevice::BaseAddress, _feedback);
        writeMap.tryInsert(TestFeedbackDevice::BaseAddress, _feedback);

        // Move ownership to the device pool.
        HardwareDevicePool devicePool;
        devicePool.emplace_back(std::move(feedbackUPtr));

        // Configure options.
        Options opts;
        opts.setHardwareArchitecture(SystemModel::Archimedies);
        opts.setProcessorVariant(ProcessorModel::ARM2);
        opts.setSystemRom(SystemROMPreset::Custom);
        opts.setRamSizeKb(1024);

        _system = std::make_unique<MemcSystem>(opts, std::move(devicePool),
                                               readMap, writeMap);

        // Load the embedded mode test ROM.
        size_t romByteCount;
        const uint8_t *romData = getModeTestRomData(romByteCount);
        _system->getHardare().setHighRom(romData, romByteCount);

        // Re-reset so the CPU starts from address 0 with the ROM in place.
        _system->reset();
        _feedback->reset();

        return _system.get();
    }

    static void watchDogThread(IArmSystem *systemToWatch,
                               std::timed_mutex *isRunning,
                               uint32_t timeoutMs)
    {
        if (!isRunning->try_lock_until(std::chrono::steady_clock::now() +
                                       std::chrono::milliseconds(timeoutMs)))
        {
            systemToWatch->raiseHostInterrupt();
        }
    }

    //! @brief Runs the system until it halts (BKPT or host interrupt).
    //! @return True if a BKPT was hit, false if the system timed out or
    //! the feedback device triggered a halt.
    bool runUntilHalt(int32_t maxSteps = 100000, uint32_t timeoutMs = 30000)
    {
        std::timed_mutex isRunning;
        std::thread watchdog;

        auto sysToRun = createSystem();

        if (sysToRun != nullptr)
        {
            std::lock_guard guard(isRunning);
            watchdog = std::thread(watchDogThread, sysToRun, &isRunning, timeoutMs);

            _lastResult = sysToRun->runLimited(maxSteps);
            _lastPC = sysToRun->getCoreRegister(CoreRegister::PC);
        }

        if (watchdog.joinable())
            watchdog.join();

        return (_lastResult.ExecResult == ExecutionMetrics::Result::DebugIrq);
    }

    //! @brief Asserts that the ROM reached at least the specified milestone.
    //! On failure, prints diagnostic information from the feedback device.
    void assertMilestoneReached(uint32_t expectedMilestone,
                                const char *testName)
    {
        uint32_t milestone = _feedback->getMilestone();
        uint32_t result = _feedback->getResult();

        if (result != 0)
        {
            FAIL() << testName << " FAILED at milestone 0x"
                   << std::hex << milestone
                   << ": error code 0x" << result
                   << ", expected 0x" << _feedback->getExpected()
                   << ", actual 0x" << _feedback->getActual()
                   << ", PC=0x" << _lastPC;
        }

        EXPECT_GE(milestone, expectedMilestone)
            << testName << ": ROM only reached milestone 0x"
            << std::hex << milestone
            << " (needed 0x" << expectedMilestone << ")"
            << ", PC=0x" << _lastPC;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
//! @brief Verify the test ROM boots successfully: RAM detection, MEMC
//! programming, page mapping, stack setup, and vector installation.
TEST_F(MemcModeSwitchTests, BootCompletes)
{
    runUntilHalt();
    assertMilestoneReached(MS_BootComplete, "Boot");
}

//! @brief Test 1: TEQP R15, #3 forces SVC mode with IRQs enabled.
//! With PC-only R15 (bottom bits=00), XOR 3 → mode=11(SVC), I=0, F=0.
TEST_F(MemcModeSwitchTests, TeqpR15Forces_SvcMode)
{
    runUntilHalt();
    assertMilestoneReached(MS_Test1_Complete, "Test1_TeqpR15");
}

//! @brief Test 2: TEQP Rn, #0 transfers PSR from register.
//! TEQP R0, #0 sets PSR = R0, a standard RISC OS idiom.
TEST_F(MemcModeSwitchTests, TeqpRnTransfersPsr)
{
    runUntilHalt();
    assertMilestoneReached(MS_Test2_Complete, "Test2_TeqpRn");
}

//! @brief Test 3: SWI entry/return via MOVS PC, R14.
TEST_F(MemcModeSwitchTests, SwiMovsReturn)
{
    runUntilHalt();
    assertMilestoneReached(MS_Test3_Complete, "Test3_SwiMovs");
}

//! @brief Test 4: SWI with STMFD/LDMFD {PC}^ return pattern.
TEST_F(MemcModeSwitchTests, SwiLdmReturn)
{
    runUntilHalt();
    assertMilestoneReached(MS_Test4_Complete, "Test4_SwiLdm");
}

//! @brief Test 5: Nested SWI replicating the OS_WriteI → OS_WriteC pattern.
//! Outer handler saves state, enables IRQs via TEQP, calls inner SWI,
//! then returns.
TEST_F(MemcModeSwitchTests, NestedSwi)
{
    runUntilHalt();
    assertMilestoneReached(MS_Test5_Complete, "Test5_NestedSwi");
}

//! @brief Test 6: TEQP R15, #3 clears all flags and IRQ disable bits.
//! Starting from known state (N=1,C=1,I=1,F=1), result should be
//! mode=SVC, all other PSR bits clear.
TEST_F(MemcModeSwitchTests, TeqpR15ClearsFlags)
{
    runUntilHalt();
    assertMilestoneReached(MS_Test6_Complete, "Test6_TeqpFlags");
}

//! @brief Test 7: SWI number extraction from instruction at [R14-4].
TEST_F(MemcModeSwitchTests, SwiNumberExtraction)
{
    runUntilHalt();
    assertMilestoneReached(MS_Test7_Complete, "Test7_SwiNumber");
}

//! @brief Test 8: STM with R15 stores (PC+12)|PSR.
TEST_F(MemcModeSwitchTests, StmR15StoresPcPlusPsr)
{
    runUntilHalt();
    assertMilestoneReached(MS_Test8_Complete, "Test8_StmR15");
}

//! @brief Run the full ROM and verify all tests pass (milestone == AllPassed,
//! result == 0).
TEST_F(MemcModeSwitchTests, AllTestsPass)
{
    bool hitBkpt = runUntilHalt();
    uint32_t milestone = getFeedback()->getMilestone();
    uint32_t result = getFeedback()->getResult();

    if (result != 0)
    {
        FAIL() << "Test ROM FAILED at milestone 0x"
               << std::hex << milestone
               << ": error code 0x" << result
               << ", expected 0x" << getFeedback()->getExpected()
               << ", actual 0x" << getFeedback()->getActual()
               << ", PC=0x" << getLastPC();
    }

    EXPECT_TRUE(hitBkpt) << "ROM did not reach BKPT — timed out or stuck."
        << " Milestone: 0x" << std::hex << milestone
        << ", PC: 0x" << getLastPC();

    EXPECT_EQ(milestone, MS_AllPassed)
        << "ROM stopped at milestone 0x" << std::hex << milestone
        << " instead of AllPassed (0xFF)";

    EXPECT_EQ(result, 0u)
        << "ROM reported failure: error 0x" << std::hex << result;
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

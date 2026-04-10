//! @file ArmEmu/Test/Test_Diagnostics.cpp
//! @brief The definition of unit tests for the diagnostic system including
//! RingBufferTrace, BootProgressMonitor, and CompositeDiagnosticSink.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <sstream>

#include <gtest/gtest.h>

#include "ArmEmu/ArmSystemBuilder.hpp"
#include "ArmSystem.inl"
#include "SystemConfigurations.inl"
#include "MEMC.inl"
#include "ArmEmu/RingBufferTrace.hpp"
#include "ArmEmu/BootProgressMonitor.hpp"
#include "ArmEmu/CompositeDiagnosticSink.hpp"
#include "MemcTestRom.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// RingBufferTrace Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(DiagnosticsTest, RingBufferCapturesInstructions)
{
    RingBufferTrace trace(4);

    for (uint32_t i = 0; i < 6; ++i)
    {
        InstructionTraceEntry entry = {};
        entry.PC = i * 4;
        entry.Opcode = 0xE0000000 + i;
        entry.CycleCount = i * 100;
        entry.CyclesTaken = 1;
        entry.WasExecuted = true;
        trace.onInstruction(entry);
    }

    // Buffer capacity is 4, so we should have the last 4 entries.
    ASSERT_EQ(trace.getInstructionCount(), 4u);

    // Oldest entry should be index 2 (i=2).
    EXPECT_EQ(trace.getInstruction(0).PC, 8u);
    EXPECT_EQ(trace.getInstruction(1).PC, 12u);
    EXPECT_EQ(trace.getInstruction(2).PC, 16u);
    EXPECT_EQ(trace.getInstruction(3).PC, 20u);
}

GTEST_TEST(DiagnosticsTest, RingBufferCapturesInterrupts)
{
    RingBufferTrace trace(4096, 2);

    for (int i = 0; i < 5; ++i)
    {
        InterruptEvent evt = {};
        evt.CycleCount = static_cast<uint64_t>(i) * 1000;
        evt.IrqStatus = static_cast<uint16_t>(i);
        evt.CpuTookIrq = (i % 2) == 0;
        trace.onInterruptChange(evt);
    }

    ASSERT_EQ(trace.getInterruptCount(), 2u);
    EXPECT_EQ(trace.getInterrupt(0).IrqStatus, 3u);
    EXPECT_EQ(trace.getInterrupt(1).IrqStatus, 4u);
}

GTEST_TEST(DiagnosticsTest, RingBufferCapturesMemoryAccesses)
{
    RingBufferTrace trace(4096, 256, 3);

    for (int i = 0; i < 5; ++i)
    {
        MemoryAccessEntry entry = {};
        entry.Address = 0x3200000 + static_cast<uint32_t>(i) * 4;
        entry.Value = static_cast<uint32_t>(i);
        entry.IsWrite = true;
        entry.Size = 4;
        trace.onMemoryAccess(entry);
    }

    ASSERT_EQ(trace.getMemoryAccessCount(), 3u);
    EXPECT_EQ(trace.getMemoryAccess(0).Value, 2u);
}

GTEST_TEST(DiagnosticsTest, RingBufferClearResetsAll)
{
    RingBufferTrace trace(8);

    InstructionTraceEntry entry = {};
    entry.PC = 0x100;
    trace.onInstruction(entry);

    ASSERT_EQ(trace.getInstructionCount(), 1u);

    trace.clear();
    EXPECT_EQ(trace.getInstructionCount(), 0u);
    EXPECT_EQ(trace.getInterruptCount(), 0u);
    EXPECT_EQ(trace.getMemoryAccessCount(), 0u);
}

GTEST_TEST(DiagnosticsTest, RingBufferDumpToStream)
{
    RingBufferTrace trace(8);

    InstructionTraceEntry entry = {};
    entry.PC = 0x3800010;
    entry.Opcode = 0xE3A00000;
    entry.CycleCount = 42;
    entry.CyclesTaken = 1;
    entry.WasExecuted = true;
    trace.onInstruction(entry);

    std::ostringstream oss;
    trace.dumpToStream(oss);
    std::string output = oss.str();

    EXPECT_NE(output.find("Instruction Trace"), std::string::npos);
    EXPECT_NE(output.find("03800010"), std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
// BootProgressMonitor Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(DiagnosticsTest, BootMonitorDetectsMemcWrite)
{
    BootProgressMonitor monitor;

    MemoryAccessEntry entry = {};
    entry.Address = 0x3600000;
    entry.IsWrite = true;
    entry.Size = 4;
    entry.CycleCount = 100;
    monitor.onMemoryAccess(entry);

    EXPECT_EQ(monitor.getMemcWriteCount(), 1u);
    ASSERT_FALSE(monitor.getMilestones().empty());
    EXPECT_NE(monitor.getMilestones()[0].Description.find("MEMC"), std::string::npos);
}

GTEST_TEST(DiagnosticsTest, BootMonitorDetectsIocAccess)
{
    BootProgressMonitor monitor;

    MemoryAccessEntry entry = {};
    entry.Address = 0x3200010;
    entry.IsWrite = false;
    entry.Size = 4;
    entry.CycleCount = 200;
    monitor.onMemoryAccess(entry);

    EXPECT_EQ(monitor.getIocAccessCount(), 1u);
    ASSERT_GE(monitor.getMilestones().size(), 1u);
}

GTEST_TEST(DiagnosticsTest, BootMonitorDetectsVidcWrite)
{
    BootProgressMonitor monitor;

    MemoryAccessEntry entry = {};
    entry.Address = 0x3400000;
    entry.IsWrite = true;
    entry.Size = 4;
    entry.CycleCount = 300;
    monitor.onMemoryAccess(entry);

    EXPECT_EQ(monitor.getVidcWriteCount(), 1u);
    ASSERT_GE(monitor.getMilestones().size(), 1u);
}

GTEST_TEST(DiagnosticsTest, BootMonitorDetectsI2cToggle)
{
    BootProgressMonitor monitor;

    MemoryAccessEntry entry = {};
    entry.Address = 0x3200000; // IOC control register
    entry.IsWrite = true;
    entry.Value = 0x03;
    entry.Size = 4;
    entry.CycleCount = 400;
    monitor.onMemoryAccess(entry);

    EXPECT_EQ(monitor.getI2cToggleCount(), 1u);
}

GTEST_TEST(DiagnosticsTest, BootMonitorCountsInstructions)
{
    BootProgressMonitor monitor;

    InstructionTraceEntry entry = {};
    entry.WasExecuted = true;

    for (int i = 0; i < 10; ++i)
        monitor.onInstruction(entry);

    EXPECT_EQ(monitor.getInstructionCount(), 10u);
}

GTEST_TEST(DiagnosticsTest, BootMonitorCountsInterruptsTaken)
{
    BootProgressMonitor monitor;

    InterruptEvent evt = {};
    evt.CpuTookIrq = true;
    monitor.onInterruptChange(evt);

    evt.CpuTookIrq = false;
    evt.CpuTookFirq = true;
    monitor.onInterruptChange(evt);

    // Only events where CPU took an interrupt should count.
    evt.CpuTookIrq = false;
    evt.CpuTookFirq = false;
    monitor.onInterruptChange(evt);

    EXPECT_EQ(monitor.getInterruptsTaken(), 2u);
}

GTEST_TEST(DiagnosticsTest, BootMonitorPrintSummary)
{
    BootProgressMonitor monitor;

    MemoryAccessEntry entry = {};
    entry.Address = 0x3600000;
    entry.IsWrite = true;
    entry.Size = 4;
    monitor.onMemoryAccess(entry);

    std::ostringstream oss;
    monitor.printSummary(oss);
    std::string output = oss.str();

    EXPECT_NE(output.find("Boot Progress Summary"), std::string::npos);
    EXPECT_NE(output.find("MEMC register writes"), std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
// CompositeDiagnosticSink Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(DiagnosticsTest, CompositeForwardsToAllChildren)
{
    RingBufferTrace trace1(8);
    RingBufferTrace trace2(8);
    CompositeDiagnosticSink composite({&trace1, &trace2});

    InstructionTraceEntry entry = {};
    entry.PC = 0x1000;
    composite.onInstruction(entry);

    EXPECT_EQ(trace1.getInstructionCount(), 1u);
    EXPECT_EQ(trace2.getInstructionCount(), 1u);
    EXPECT_EQ(trace1.getInstruction(0).PC, 0x1000u);
    EXPECT_EQ(trace2.getInstruction(0).PC, 0x1000u);
}

GTEST_TEST(DiagnosticsTest, CompositeForwardsMemoryAccess)
{
    RingBufferTrace trace(8);
    BootProgressMonitor monitor;
    CompositeDiagnosticSink composite({&trace, &monitor});

    MemoryAccessEntry entry = {};
    entry.Address = 0x3600000;
    entry.IsWrite = true;
    entry.Size = 4;
    composite.onMemoryAccess(entry);

    EXPECT_EQ(trace.getMemoryAccessCount(), 1u);
    EXPECT_EQ(monitor.getMemcWriteCount(), 1u);
}

GTEST_TEST(DiagnosticsTest, CompositeForwardsInterrupt)
{
    RingBufferTrace trace(8, 8);
    BootProgressMonitor monitor;
    CompositeDiagnosticSink composite({&trace, &monitor});

    InterruptEvent evt = {};
    evt.CpuTookIrq = true;
    composite.onInterruptChange(evt);

    EXPECT_EQ(trace.getInterruptCount(), 1u);
    EXPECT_EQ(monitor.getInterruptsTaken(), 1u);
}

GTEST_TEST(DiagnosticsTest, CompositeAddSink)
{
    CompositeDiagnosticSink composite;
    EXPECT_EQ(composite.getSinkCount(), 0u);

    RingBufferTrace trace(4);
    composite.addSink(&trace);
    EXPECT_EQ(composite.getSinkCount(), 1u);

    InstructionTraceEntry entry = {};
    entry.PC = 0x2000;
    composite.onInstruction(entry);

    EXPECT_EQ(trace.getInstructionCount(), 1u);
}

////////////////////////////////////////////////////////////////////////////////
// Null Sink Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(DiagnosticsTest, NullSinkNoCallbacks)
{
    // Verify that when no diagnostic sink is attached, execution proceeds
    // without any issues.
    using MemcSystem = ArmSystem<ArmV2MemcSystemTraits<false>>;

    Options opts;
    opts.setHardwareArchitecture(SystemModel::Archimedies);
    opts.setProcessorVariant(ProcessorModel::ARM2);
    opts.setSystemRom(SystemROMPreset::Custom);
    opts.setRamSizeKb(1024);

    MemcSystem system(opts, HardwareDevicePool(), AddressMap(), AddressMap(), nullptr);

    size_t romByteCount;
    const uint8_t *romData = getMemcTestRomData(romByteCount);
    system.getHardare().setHighRom(romData, romByteCount);
    system.reset();

    // No diagnostic sink attached - should run without crashing.
    IDiagnosticSink *sink;

    EXPECT_FALSE(system.tryFindTypedDevice("DiagnosticSink", sink));
    EXPECT_EQ(sink, nullptr);
    ExecutionMetrics result = system.runLimited(100);
    EXPECT_GT(result.InstructionCount, 0u);
}

////////////////////////////////////////////////////////////////////////////////
// Integration: Instruction Trace With Execution
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(DiagnosticsTest, InstructionTraceCaptures)
{
    using MemcSystem = ArmSystem<ArmV2MemcSystemTraits<true>>;

    Options opts;
    opts.setHardwareArchitecture(SystemModel::Archimedies);
    opts.setProcessorVariant(ProcessorModel::ARM2);
    opts.setSystemRom(SystemROMPreset::Custom);
    opts.setRamSizeKb(1024);

    HardwareDevicePool devicePool;
    devicePool.emplace_back(std::make_unique<RingBufferTrace>(256));

    MemcSystem system(opts, std::move(devicePool), AddressMap(), AddressMap(), nullptr);

    RingBufferTrace *trace = nullptr;
    ASSERT_TRUE(system.tryFindTypedDevice("DiagnosticSink", trace));
    ASSERT_NE(trace, nullptr);

    size_t romByteCount;
    const uint8_t *romData = getMemcTestRomData(romByteCount);
    system.getHardare().setHighRom(romData, romByteCount);
    system.reset();

    // Run a small number of cycles.
    ExecutionMetrics result = system.runLimited(50);

    // We should have captured some instructions.
    ASSERT_GT(trace->getInstructionCount(), 0u);

    // Verify the first captured instruction has a valid PC.
    const auto &first = trace->getInstruction(0);
    EXPECT_NE(first.Opcode, 0u);
}

////////////////////////////////////////////////////////////////////////////////
// IHardwareDevice Interface Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(DiagnosticsTest, RingBufferTraceHasDeviceName)
{
    RingBufferTrace trace;
    EXPECT_FALSE(trace.getName().isEmpty());
    EXPECT_FALSE(trace.getDescription().isEmpty());
}

GTEST_TEST(DiagnosticsTest, BootMonitorHasDeviceName)
{
    BootProgressMonitor monitor;
    EXPECT_FALSE(monitor.getName().isEmpty());
    EXPECT_FALSE(monitor.getDescription().isEmpty());
}

GTEST_TEST(DiagnosticsTest, CompositeHasDeviceName)
{
    CompositeDiagnosticSink composite;
    EXPECT_FALSE(composite.getName().isEmpty());
    EXPECT_FALSE(composite.getDescription().isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
// ArmSystemBuilder Integration
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(DiagnosticsTest, SinkAutoConnectsViaAddDevice)
{
    // Verify that a diagnostic sink added via ArmSystemBuilder::addDevice()
    // is automatically connected during system initialisation.
    ArmSystemBuilder builder;
    Options opts;
    opts.setHardwareArchitecture(SystemModel::Archimedies);
    opts.setProcessorVariant(ProcessorModel::ARM2);
    opts.setSystemRom(SystemROMPreset::Custom);
    opts.setRamSizeKb(1024);
    builder.reset(opts);
    builder.setDiagnosticsEnabled(true);

    auto trace = std::make_unique<RingBufferTrace>(64);
    RingBufferTrace *tracePtr = trace.get();
    builder.addDevice(std::move(trace));

    IArmSystemUPtr system = builder.createSystem();

    // The sink should have been auto-connected during initialisation.
    RingBufferTrace *sink = nullptr;
    EXPECT_TRUE(system->tryFindTypedDevice("DiagnosticSink", sink));
    EXPECT_EQ(tracePtr, sink);
}

GTEST_TEST(DiagnosticsTest, SinkDiscoverableByName)
{
    // Verify that a diagnostic sink added as a device can be found by name.
    ArmSystemBuilder builder;
    Options opts;
    opts.setHardwareArchitecture(SystemModel::Archimedies);
    opts.setProcessorVariant(ProcessorModel::ARM2);
    opts.setSystemRom(SystemROMPreset::Custom);
    opts.setRamSizeKb(1024);
    builder.reset(opts);
    builder.setDiagnosticsEnabled(true);

    auto trace = std::make_unique<RingBufferTrace>(64);
    RingBufferTrace *originalTace = trace.get();
    builder.addDevice(std::move(trace));

    IArmSystemUPtr system = builder.createSystem();

    // Look up the device by name.
    IDiagnosticSink *foundSink = nullptr;
    EXPECT_TRUE(system->tryFindTypedDevice<IDiagnosticSink>(
        Ag::String("RingBufferTrace"), foundSink));
    EXPECT_NE(foundSink, nullptr);
    EXPECT_EQ(foundSink, originalTace);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

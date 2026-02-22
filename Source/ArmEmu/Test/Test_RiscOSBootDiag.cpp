//! @file ArmEmu/Test/Test_RiscOSBootDiag.cpp
//! @brief The definition of diagnostic investigation tests that use the
//! diagnostic sink infrastructure to discover where and why RISC OS 3.10
//! boot stalls in the emulator.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <iomanip>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

#include "AsmTools/InstructionInfo.hpp"
#include "ArmEmu/ArmSystemBuilder.hpp"
#include "ArmEmu/RingBufferTrace.hpp"
#include "ArmEmu/BootProgressMonitor.hpp"
#include "ArmEmu/BootStallDetector.hpp"
#include "ArmEmu/CompositeDiagnosticSink.hpp"
#include "MemcHardware.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Helper Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates an Archimedes ARM2 system configured with RISC OS 3.10,
//! 4 MB RAM, and diagnostics enabled, with the given composite sink attached.
IArmSystemUPtr createDiagnosticRiscOSSystem(
    std::unique_ptr<CompositeDiagnosticSink> compositeSink)
{
    Options opts;
    opts.setHardwareArchitecture(SystemModel::Archimedies);
    opts.setProcessorVariant(ProcessorModel::ARM2);
    opts.setSystemRom(SystemROMPreset::RiscOS_3_10);
    opts.setRamSizeKb(4096);

    ArmSystemBuilder builder(opts);
    builder.setDiagnosticsEnabled(true);
    builder.addDevice(std::move(compositeSink));

    return builder.createSystem();
}

//! @brief Disassembles a single instruction and writes it to a stream.
//! @param[in] stream The output stream.
//! @param[in] pc The address of the instruction.
//! @param[in] opcode The 32-bit opcode to disassemble.
void printDisassembly(std::ostream &stream, uint32_t pc, uint32_t opcode)
{
    Asm::InstructionInfo info;

    if (info.disassemble(opcode, pc, Asm::InstructionInfo::ARMv2))
    {
        Ag::String text = info.toString(pc, 0);
        stream << text.getUtf8Bytes();
    }
    else
    {
        stream << "DCD 0x" << std::hex << std::setfill('0') << std::setw(8)
               << opcode << std::dec << std::setfill(' ');
    }
}

//! @brief Dumps the core register state to a stream.
//! @param[in] stream The output stream.
//! @param[in] system The ARM system to read registers from.
void printRegisters(std::ostream &stream, IArmSystem *system)
{
    stream << "=== Register State ===\n";

    for (int i = 0; i <= 15; ++i)
    {
        CoreRegister reg = static_cast<CoreRegister>(i);
        uint32_t val = system->getCoreRegister(reg);
        stream << "  R" << std::setw(2) << std::setfill(' ') << std::left << i
               << std::right << " = 0x" << std::hex << std::setfill('0')
               << std::setw(8) << val << std::dec << std::setfill(' ');

        if ((i % 4) == 3)
            stream << "\n";
        else
            stream << "    ";
    }

    uint32_t cpsr = system->getCoreRegister(CoreRegister::CPSR);
    ProcessorMode mode = system->getMode();
    const auto &modeInfo = getProcessorModeType();
    std::string_view modeName = modeInfo.toDisplayName(mode);

    stream << "  CPSR = 0x" << std::hex << std::setfill('0') << std::setw(8)
           << cpsr << std::dec << std::setfill(' ')
           << "  Mode: " << (modeName.empty() ? "unknown" : modeName) << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////

//! @brief Attaches a BootStallDetector and RingBufferTrace to detect where
//! the boot gets stuck in a tight loop. Outputs stall report with disassembled
//! loop instructions, register state, and last instructions from trace.
GTEST_TEST(RiscOSBootDiag, StallDetection)
{
    auto stallDetector = std::make_unique<BootStallDetector>();
    auto ringTrace = std::make_unique<RingBufferTrace>(32);
    BootStallDetector *stallPtr = stallDetector.get();
    RingBufferTrace *tracePtr = ringTrace.get();

    auto composite = std::make_unique<CompositeDiagnosticSink>();
    composite->addSink(stallPtr);
    composite->addSink(tracePtr);

    IArmSystemUPtr system = createDiagnosticRiscOSSystem(std::move(composite));
    ASSERT_NE(system, nullptr) << "Failed to create RISC OS system.";

    // Run 10 million cycles.
    ExecutionMetrics result = system->runLimited(10000000);

    std::ostringstream report;
    report << "\n=== RiscOSBootDiag_StallDetection ===\n"
           << "Cycles executed: " << result.CycleCount << "\n"
           << "Instructions:    " << result.InstructionCount << "\n\n";

    // Output stall report.
    stallPtr->printReport(report, system.get());

    // Dump register state.
    report << "\n";
    printRegisters(report, system.get());

    // Dump last 32 instructions from trace.
    report << "\n=== Last " << tracePtr->getInstructionCount()
           << " Instructions ===\n";

    for (size_t i = 0; i < tracePtr->getInstructionCount(); ++i)
    {
        const auto &entry = tracePtr->getInstruction(i);
        report << "  [" << std::setw(12) << entry.CycleCount << "] "
               << "0x" << std::hex << std::setfill('0') << std::setw(8)
               << entry.PC << "  " << std::dec << std::setfill(' ');
        printDisassembly(report, entry.PC, entry.Opcode);
        report << (entry.WasExecuted ? "  EXEC" : "  skip") << "\n";
    }

    // Output to test log.
    std::cout << report.str() << std::endl;

    // Verify the system ran some instructions.
    EXPECT_GT(result.InstructionCount, 0u)
        << "No instructions executed — system may have failed to initialise.";
}

//! @brief Attaches a BootProgressMonitor and RingBufferTrace to observe
//! boot milestones, MMIO activity, and the instruction/MMIO trace.
GTEST_TEST(RiscOSBootDiag, BootProgressWithTrace)
{
    auto monitor = std::make_unique<BootProgressMonitor>();
    auto ringTrace = std::make_unique<RingBufferTrace>(64, 64, 64);
    BootProgressMonitor *monitorPtr = monitor.get();
    RingBufferTrace *tracePtr = ringTrace.get();

    auto composite = std::make_unique<CompositeDiagnosticSink>();
    composite->addSink(monitorPtr);
    composite->addSink(tracePtr);

    IArmSystemUPtr system = createDiagnosticRiscOSSystem(std::move(composite));
    ASSERT_NE(system, nullptr) << "Failed to create RISC OS system.";

    // Run 10 million cycles.
    ExecutionMetrics result = system->runLimited(10000000);

    std::ostringstream report;
    report << "\n=== RiscOSBootDiag_BootProgressWithTrace ===\n"
           << "Cycles executed: " << result.CycleCount << "\n"
           << "Instructions:    " << result.InstructionCount << "\n\n";

    // Output boot progress summary.
    monitorPtr->printSummary(report);

    // Output last N instructions.
    report << "\n=== Last " << tracePtr->getInstructionCount()
           << " Instructions ===\n";

    for (size_t i = 0; i < tracePtr->getInstructionCount(); ++i)
    {
        const auto &entry = tracePtr->getInstruction(i);
        report << "  [" << std::setw(12) << entry.CycleCount << "] "
               << "0x" << std::hex << std::setfill('0') << std::setw(8)
               << entry.PC << "  " << std::dec << std::setfill(' ');
        printDisassembly(report, entry.PC, entry.Opcode);
        report << (entry.WasExecuted ? "  EXEC" : "  skip") << "\n";
    }

    // Output last N MMIO accesses.
    report << "\n=== Last " << tracePtr->getMemoryAccessCount()
           << " MMIO Accesses ===\n";

    for (size_t i = 0; i < tracePtr->getMemoryAccessCount(); ++i)
    {
        const auto &entry = tracePtr->getMemoryAccess(i);
        report << "  [" << std::setw(12) << entry.CycleCount << "] "
               << (entry.IsWrite ? "W" : "R")
               << " 0x" << std::hex << std::setfill('0') << std::setw(8)
               << entry.Address << " = 0x" << std::setw(8) << entry.Value
               << std::dec << std::setfill(' ')
               << " [" << static_cast<unsigned>(entry.Size) << "]\n";
    }

    std::cout << report.str() << std::endl;

    EXPECT_GT(result.InstructionCount, 0u);
}

//! @brief Reads and disassembles the ARM exception vectors after early boot
//! to determine whether they are still ROM branch instructions or have been
//! replaced with RAM-based LDR PC stubs.
GTEST_TEST(RiscOSBootDiag, ExceptionVectorIntegrity)
{
    auto composite = std::make_unique<CompositeDiagnosticSink>();
    // No sinks needed — we just need diagnostics enabled for the template.

    IArmSystemUPtr system = createDiagnosticRiscOSSystem(std::move(composite));
    ASSERT_NE(system, nullptr) << "Failed to create RISC OS system.";

    // Run 1 million cycles — enough to get past early boot.
    ExecutionMetrics result = system->runLimited(1000000);

    std::ostringstream report;
    report << "\n=== RiscOSBootDiag_ExceptionVectorIntegrity ===\n"
           << "Instructions executed: " << result.InstructionCount << "\n"
           << "PC after run: 0x" << std::hex << std::setfill('0')
           << std::setw(8)
           << system->getCoreRegister(CoreRegister::PC)
           << std::dec << std::setfill(' ') << "\n\n";

    // Read exception vectors at logical 0x00-0x1C.
    static const char *vectorNames[] = {
        "Reset", "Undefined Instruction", "SWI",
        "Prefetch Abort", "Data Abort", "Address Exception",
        "IRQ", "FIQ"
    };

    report << "Exception Vectors (0x00-0x1C):\n";

    for (int i = 0; i < 8; ++i)
    {
        uint32_t addr = static_cast<uint32_t>(i) * 4;
        uint32_t opcode = 0;
        uint32_t bytesRead = readFromLogicalAddress(system.get(), addr,
                                                    &opcode, sizeof(opcode));

        report << "  0x" << std::hex << std::setfill('0') << std::setw(2)
               << addr << ": " << std::dec << std::setfill(' ')
               << std::setw(20) << std::left << vectorNames[i] << std::right
               << "  ";

        if (bytesRead == sizeof(opcode))
        {
            report << "0x" << std::hex << std::setfill('0') << std::setw(8)
                   << opcode << "  " << std::dec << std::setfill(' ');
            printDisassembly(report, addr, opcode);
        }
        else
        {
            report << "(read failed)";
        }

        report << "\n";
    }

    // Read handler addresses at 0x20-0x3C (used by LDR PC,[PC,#offset] stubs).
    report << "\nHandler Addresses (0x20-0x3C):\n";

    for (int i = 0; i < 8; ++i)
    {
        uint32_t addr = 0x20 + static_cast<uint32_t>(i) * 4;
        uint32_t value = 0;
        uint32_t bytesRead = readFromLogicalAddress(system.get(), addr,
                                                    &value, sizeof(value));

        report << "  0x" << std::hex << std::setfill('0') << std::setw(2)
               << addr << ": " << std::dec << std::setfill(' ')
               << std::setw(20) << std::left << vectorNames[i] << std::right;

        if (bytesRead == sizeof(value))
        {
            report << "  0x" << std::hex << std::setfill('0') << std::setw(8)
                   << value << std::dec << std::setfill(' ');
        }
        else
        {
            report << "  (read failed)";
        }

        report << "\n";
    }

    std::cout << report.str() << std::endl;

    EXPECT_GT(result.InstructionCount, 0u);
}

//! @brief Probes the MEMC page table after boot to verify key logical address
//! ranges are mapped to physical memory as expected.
GTEST_TEST(RiscOSBootDiag, MemcPageTableDump)
{
    auto composite = std::make_unique<CompositeDiagnosticSink>();

    IArmSystemUPtr system = createDiagnosticRiscOSSystem(std::move(composite));
    ASSERT_NE(system, nullptr) << "Failed to create RISC OS system.";

    // Run 5 million cycles.
    ExecutionMetrics result = system->runLimited(5000000);

    std::ostringstream report;
    report << "\n=== RiscOSBootDiag_MemcPageTableDump ===\n"
           << "Instructions executed: " << result.InstructionCount << "\n"
           << "PC after run: 0x" << std::hex << std::setfill('0')
           << std::setw(8)
           << system->getCoreRegister(CoreRegister::PC)
           << std::dec << std::setfill(' ') << "\n\n";

    // Probe key logical addresses.
    static const struct {
        uint32_t addr;
        const char *description;
    } probeAddrs[] = {
        { 0x0000000, "Zero page (exception vectors)" },
        { 0x0008000, "Application space start" },
        { 0x0100000, "1 MB — low application RAM" },
        { 0x1000000, "16 MB — logical RAM space" },
        { 0x2000000, "Physical RAM start" },
        { 0x3200000, "IOC region" },
        { 0x3800000, "High ROM start" },
    };

    report << "MEMC Page Mappings:\n"
           << std::setw(12) << "Logical" << "  "
           << std::setw(12) << "Physical" << "  "
           << std::setw(10) << "PageSize" << "  "
           << std::setw(8) << "Access" << "  "
           << "Description\n"
           << std::string(70, '-') << "\n";

    for (const auto &probe : probeAddrs)
    {
        PageMapping mapping;
        bool mapped = system->logicalToPhysicalAddress(probe.addr, mapping);

        report << "0x" << std::hex << std::setfill('0') << std::setw(8)
               << probe.addr << "  ";

        if (mapped && (mapping.Access & PageMapping::IsPresent))
        {
            uint32_t physAddr = mapping.PageBaseAddr +
                                (probe.addr - mapping.VirtualBaseAddr);
            report << "0x" << std::setw(8) << physAddr << "  "
                   << std::dec << std::setfill(' ') << std::setw(8)
                   << mapping.PageSize << "  ";

            // Decode access flags.
            std::string accessStr;

            if (mapping.Access & PageMapping::SvcCanRead)
                accessStr += "SR ";
            if (mapping.Access & PageMapping::SvcCanWrtie)
                accessStr += "SW ";
            if (mapping.Access & PageMapping::UserCanRead)
                accessStr += "UR ";
            if (mapping.Access & PageMapping::UserCanWrite)
                accessStr += "UW ";

            report << std::setw(8) << std::left << accessStr << std::right;
        }
        else
        {
            report << std::dec << std::setfill(' ')
                   << "(not mapped)                      ";
        }

        report << "  " << probe.description << "\n";
    }

    std::cout << report.str() << std::endl;

    EXPECT_GT(result.InstructionCount, 0u);
}

//! @brief Attaches a BootProgressMonitor to observe I2C and IOC activity
//! during early boot, and reads the IOC control register to verify state.
GTEST_TEST(RiscOSBootDiag, I2CAndIOCActivity)
{
    auto monitor = std::make_unique<BootProgressMonitor>();
    BootProgressMonitor *monitorPtr = monitor.get();

    auto composite = std::make_unique<CompositeDiagnosticSink>();
    composite->addSink(monitorPtr);

    IArmSystemUPtr system = createDiagnosticRiscOSSystem(std::move(composite));
    ASSERT_NE(system, nullptr) << "Failed to create RISC OS system.";

    // Run 5 million cycles.
    ExecutionMetrics result = system->runLimited(5000000);

    std::ostringstream report;
    report << "\n=== RiscOSBootDiag_I2CAndIOCActivity ===\n"
           << "Instructions executed: " << result.InstructionCount << "\n"
           << "I2C toggle count:      " << monitorPtr->getI2cToggleCount() << "\n"
           << "IOC access count:      " << monitorPtr->getIocAccessCount() << "\n"
           << "Instruction count:     " << monitorPtr->getInstructionCount() << "\n"
           << "Interrupts taken:      " << monitorPtr->getInterruptsTaken() << "\n";

    // Read IOC control register via physical address.
    uint32_t iocCtrlValue = 0;
    uint32_t bytesRead = readFromPhysicalAddress(system.get(), 0x3200000,
                                                 &iocCtrlValue,
                                                 sizeof(iocCtrlValue));

    if (bytesRead == sizeof(iocCtrlValue))
    {
        report << "IOC control register:  0x" << std::hex << std::setfill('0')
               << std::setw(8) << iocCtrlValue << std::dec
               << std::setfill(' ') << "\n";

        // Decode I2C bits.
        report << "  I2C SDA (C0):  " << ((iocCtrlValue & 0x01) ? "HIGH" : "LOW") << "\n"
               << "  I2C SCL (C1):  " << ((iocCtrlValue & 0x02) ? "HIGH" : "LOW") << "\n";
    }
    else
    {
        report << "IOC control register:  (read failed)\n";
    }

    std::cout << report.str() << std::endl;

    EXPECT_GT(result.InstructionCount, 0u);
    EXPECT_GT(monitorPtr->getIocAccessCount(), 0u)
        << "No IOC accesses detected — hardware may not be initialised.";
}

//! @brief Runs the system in 1M-cycle increments, recording PC, mode, and
//! instruction count at each interval to build a timeline of boot progression.
//! Detects stalls when PC is unchanged for 3+ consecutive intervals.
GTEST_TEST(RiscOSBootDiag, PCProgressionOverTime)
{
    auto composite = std::make_unique<CompositeDiagnosticSink>();

    IArmSystemUPtr system = createDiagnosticRiscOSSystem(std::move(composite));
    ASSERT_NE(system, nullptr) << "Failed to create RISC OS system.";

    struct Snapshot
    {
        uint32_t PC;
        ProcessorMode Mode;
        uint64_t CumulativeInstructions;
    };

    static constexpr int IntervalCount = 50;
    static constexpr int32_t CyclesPerInterval = 1000000;
    std::vector<Snapshot> timeline;
    timeline.reserve(IntervalCount);

    uint64_t totalInstructions = 0;

    for (int i = 0; i < IntervalCount; ++i)
    {
        ExecutionMetrics result = system->runLimited(CyclesPerInterval);
        totalInstructions += result.InstructionCount;

        Snapshot snap;
        snap.PC = system->getCoreRegister(CoreRegister::PC);
        snap.Mode = system->getMode();
        snap.CumulativeInstructions = totalInstructions;
        timeline.push_back(snap);
    }

    // Print the timeline.
    const auto &modeInfo = getProcessorModeType();

    std::ostringstream report;
    report << "\n=== RiscOSBootDiag_PCProgressionOverTime ===\n"
           << "Total intervals: " << IntervalCount << " x "
           << CyclesPerInterval << " cycles\n"
           << "Total instructions: " << totalInstructions << "\n\n"
           << std::setw(8) << "Interval" << "  "
           << std::setw(12) << "PC" << "  "
           << std::setw(12) << "Mode" << "  "
           << std::setw(16) << "Instructions" << "\n"
           << std::string(52, '-') << "\n";

    for (int i = 0; i < IntervalCount; ++i)
    {
        const Snapshot &snap = timeline[static_cast<size_t>(i)];
        std::string_view modeName = modeInfo.toDisplayName(snap.Mode);

        report << std::setw(8) << (i + 1) << "  "
               << "0x" << std::hex << std::setfill('0') << std::setw(8)
               << snap.PC << std::dec << std::setfill(' ') << "  "
               << std::setw(12) << std::left
               << (modeName.empty() ? "unknown" : modeName)
               << std::right << "  "
               << std::setw(16) << snap.CumulativeInstructions << "\n";
    }

    // Detect stall: PC unchanged for 3+ consecutive intervals.
    int stallStart = -1;

    for (int i = 2; i < IntervalCount; ++i)
    {
        size_t idx = static_cast<size_t>(i);

        if (timeline[idx].PC == timeline[idx - 1].PC &&
            timeline[idx].PC == timeline[idx - 2].PC)
        {
            stallStart = i - 2;
            break;
        }
    }

    if (stallStart >= 0)
    {
        uint32_t stallPC = timeline[static_cast<size_t>(stallStart)].PC;
        report << "\n*** STALL DETECTED at interval " << (stallStart + 1)
               << ", PC = 0x" << std::hex << std::setfill('0') << std::setw(8)
               << stallPC << std::dec << std::setfill(' ') << " ***\n\n";

        // Dump register state.
        printRegisters(report, system.get());

        // Disassemble code around the stall PC.
        report << "\nCode around stall PC:\n";

        uint32_t startAddr = (stallPC >= 32) ? (stallPC - 32) : 0;

        for (uint32_t addr = startAddr; addr <= stallPC + 32; addr += 4)
        {
            uint32_t opcode = 0;
            uint32_t bytesRead = readFromLogicalAddress(system.get(), addr,
                                                        &opcode,
                                                        sizeof(opcode));

            report << "  " << ((addr == stallPC) ? ">>>" : "   ")
                   << " 0x" << std::hex << std::setfill('0') << std::setw(8)
                   << addr << ": " << std::dec << std::setfill(' ');

            if (bytesRead == sizeof(opcode))
            {
                report << "0x" << std::hex << std::setfill('0') << std::setw(8)
                       << opcode << "  " << std::dec << std::setfill(' ');
                printDisassembly(report, addr, opcode);
            }
            else
            {
                report << "(read failed)";
            }

            report << "\n";
        }
    }
    else
    {
        report << "\nNo stall detected during " << IntervalCount
               << " intervals.\n";
    }

    std::cout << report.str() << std::endl;

    EXPECT_GT(totalInstructions, 0u);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

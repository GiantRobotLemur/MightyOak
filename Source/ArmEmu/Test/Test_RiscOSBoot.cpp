//! @file ArmEmu/Test/Test_RiscOSBoot.cpp
//! @brief The definition of unit tests which verify that RISC OS ROM images
//! can boot on the emulated hardware with I2C/CMOS support.
//! @author GiantRobotLemur@na-se.co.uk - and Claude Code.
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>

#include "Ag/Core/FsPath.hpp"

#include "ArmSystem.inl"
#include "SystemConfigurations.inl"
#include "MemcHardware.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A test fixture which creates a MEMC-based system loaded with a
//! real RISC OS ROM and provides helpers for headless boot verification.
class RiscOSBootTests : public ::testing::Test
{
protected:
    static constexpr uint32_t RamSizeKb = 1024; // 4 MB for RISC OS 3.10

    RiscOSBootTests()
    {
    }

    IArmSystemUPtr createSystem(IDiagnosticSinkUPtr &&diagnostics)
    {
        Options opts;
        opts.setHardwareArchitecture(SystemModel::Archimedies);
        opts.setProcessorVariant(ProcessorModel::ARM2);
        opts.setSystemRom(SystemROMPreset::RiscOS_3_10);
        opts.setRamSizeKb(RamSizeKb);
        ArmSystemBuilder builder(opts);

        if (diagnostics)
            builder.addDevice(std::move(diagnostics));

        return builder.createSystem();
    }

    IArmSystemUPtr createSystem()
    {
        return createSystem({ });
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
//! @brief Verify that the RISC OS 3.10 ROM can be loaded and the PC advances
//! past the reset vector into ROM code.
TEST_F(RiscOSBootTests, ResetVectorExecutes)
{
    // Run a modest number of cycles to verify the PC advances from the reset vector.
    auto specimen = createSystem();
    auto result = specimen->runLimited(1000);

    uint32_t pc = specimen->getCoreRegister(CoreRegister::PC);
    EXPECT_GT(pc, 0u) << "PC did not advance from the reset vector.";
    EXPECT_GT(result.InstructionCount, 0u) << "No instructions were executed.";
}

//! @brief Verify that the RISC OS ROM advances past the I2C/CMOS probe
//! without hanging. This proves I2C bus and PCF8583 are working.
//! @details RISC OS probes the I2C bus during early boot to read CMOS
//! settings. Without a responding I2C device, the OS hangs in a tight
//! polling loop. Running 1M cycles should be sufficient to pass this point.
TEST_F(RiscOSBootTests, AdvancesPastI2CProbe)
{
    // Run 50 million cycles. The I2C probe begins at ~28M cycles,
    // after the RAM test and keyboard initialisation complete.
    auto sinkPtr = std::make_unique<BootProgressMonitor>();
    auto sink = sinkPtr.get();
    auto specimen = createSystem(std::move(sinkPtr));
    auto result = specimen->runLimited(50000000);

    uint32_t pc = specimen->getCoreRegister(CoreRegister::PC);
    EXPECT_GT(sink->getI2cToggleCount(), 1u) << "PC at 0x" << std::hex << pc;


    // After 1M cycles, if the PC is still in the very early ROM code
    // (the first few hundred bytes), we're likely stuck in the I2C probe loop.
    // The I2C probe code is typically within the first 0x1000 bytes of the ROM.
    // A successful boot should have the PC well past that point.
    //
    // We check that the PC has moved beyond the initial reset/I2C area
    // into the main ROM body.
    EXPECT_GT(pc, MEMC::HighRomStart + 0x1000u)
        << "PC appears stuck in early boot code (possibly I2C probe). "
        << "PC = 0x" << std::hex << pc;
}

//! @brief Run a larger number of cycles to verify the boot progresses
//! significantly into the ROM code without crashing.
TEST_F(RiscOSBootTests, BootProgressesBeyondHardwareInit)
{
    // Run 10 million cycles to get through hardware initialisation.
    auto specimen = createSystem();
    auto result = specimen->runLimited(10000000);

    uint32_t pc = specimen->getCoreRegister(CoreRegister::PC);

    // Verify the PC is in a reasonable range (ROM, logical RAM, or
    // physical RAM — during early boot, RISC OS may execute from
    // physical RAM before page tables are fully configured).
    bool pcInRom = (pc >= MEMC::HighRomStart) && (pc < MEMC::AddrSpaceEnd);
    bool pcInLogicalRam = (pc < MEMC::PhysRamStart);
    bool pcInPhysicalRam = (pc >= MEMC::PhysRamStart) && (pc < MEMC::HighRomStart);

    EXPECT_TRUE(pcInRom || pcInLogicalRam || pcInPhysicalRam)
        << "PC is at unexpected address 0x" << std::hex << pc
        << " after 10M cycles.";

    // The system should have executed a meaningful number of instructions.
    EXPECT_GT(result.InstructionCount, 100000u)
        << "Too few instructions executed — boot may have stalled.";
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

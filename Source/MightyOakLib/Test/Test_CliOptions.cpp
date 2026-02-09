//! @file MightyOakLib/Test/Test_CliOptions.cpp
//! @brief  The definition of unit tests for the CliOptions class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "MightyOakLib/CliOptions.hpp"

namespace Mo {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
const Ag::AppMetadata metadata(Ag::Version(0, 1, 0, 0, "Test"),
                               "TestApp", "TestProduct", "Test",
                               "Testy McTestface",
                               "(c) Testy McTestFace. All rights reserved.");

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, DefaultConstruct)
{
    CliOptions specimen(metadata);

    EXPECT_EQ(specimen.getCommand(), Ag::Cli::StandardCommands::NoCommand);
}

GTEST_TEST(CliOptions, ParseNoArgsStartsDefaultSession)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"", error));

    EXPECT_EQ(specimen.getCommand(), Ag::toScalar(AppCommand::RunSession));
    EXPECT_TRUE(specimen.getEmulatedSystemConfig().validate(error));
}

GTEST_TEST(CliOptions, ParseArgs_UpgradedA3010)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--ram 4Mb", error));

    EXPECT_EQ(specimen.getCommand(), Ag::toScalar(AppCommand::RunSession));
    EXPECT_TRUE(specimen.getEmulatedSystemConfig().validate(error));
    EXPECT_EQ(specimen.getEmulatedSystemConfig().getRamSizeKb(), 4096u);
}

////////////////////////////////////////////////////////////////////////////////
// --base (Production Model) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseBase_A305)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base A305", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::Archimedies);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM2);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 8);
    EXPECT_EQ(config.getRamSizeKb(), 512u);
    EXPECT_EQ(config.getHardDiskTechnology(), Arm::HardDiskInterface::None);
    EXPECT_EQ(config.getHardDriveCount(), 0);
    EXPECT_EQ(config.getFloppyDiskCount(), 1);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::Arthur_1_20);
}

GTEST_TEST(CliOptions, ParseBase_A440)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base A440", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::Archimedies);
    EXPECT_EQ(config.getRamSizeKb(), 4096u);
    EXPECT_EQ(config.getHardDiskTechnology(), Arm::HardDiskInterface::ST506);
    EXPECT_EQ(config.getHardDriveCount(), 1);
}

GTEST_TEST(CliOptions, ParseBase_A3010)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base A3010", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::ASeries);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM250);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 12);
    EXPECT_EQ(config.getRamSizeKb(), 1024u);
    EXPECT_EQ(config.getDisplayType(), Arm::DisplayInterface::NormalTV);
    EXPECT_EQ(config.getJoystickType(), Arm::JoystickInterface::Digital);
    EXPECT_EQ(config.getJoystickCount(), 2);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::RiscOS_3_10);
}

GTEST_TEST(CliOptions, ParseBase_A540)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base A540", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM3);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 25);
    EXPECT_EQ(config.getRamSizeKb(), 4096u);
    EXPECT_EQ(config.getHardDiskTechnology(), Arm::HardDiskInterface::SCSI);
    EXPECT_EQ(config.getHardDriveCount(), 1);
}

GTEST_TEST(CliOptions, ParseBase_A5000)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base A5000", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::ASeries);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM3);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 25);
    EXPECT_EQ(config.getRamSizeKb(), 2048u);
    EXPECT_EQ(config.getHardDiskTechnology(), Arm::HardDiskInterface::IDE);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::RiscOS_3_00);
}

GTEST_TEST(CliOptions, ParseBase_RiscPC600)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base RiscPC_600", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::RiscPC);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM610);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 30);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::RiscOS_3_50);
}

GTEST_TEST(CliOptions, ParseBase_RiscPC_StrongARM)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base RiscPC_StrongARM", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::RiscPC);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::StrongARM);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 233);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::RiscOS_3_70);
}

GTEST_TEST(CliOptions, ParseBase_A7000)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base A7000", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::RiscPC);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM710);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 32);
    EXPECT_EQ(config.getJoystickType(), Arm::JoystickInterface::Analog);
    EXPECT_EQ(config.getJoystickCount(), 1);
}

GTEST_TEST(CliOptions, ParseBase_A7000Plus)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base A7000_Plus", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM710_FPA);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 48);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::RiscOS_3_71);
}

GTEST_TEST(CliOptions, ParseBase_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--base NotAModel", error));
}

////////////////////////////////////////////////////////////////////////////////
// --system (System Model) Tests
// Note: tryParse() calls validate() internally, so the system model must be
// combined with compatible processor, RAM, ROM, etc.
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseSystem_Archimedes)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // ARM250 (default CPU) is compatible with Archimedes.
    ASSERT_TRUE(specimen.tryParse(L"--system Archimedes --ram 1M --rom Arthur_1_20", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardwareArchitecture(),
              Arm::SystemModel::Archimedies);
}

GTEST_TEST(CliOptions, ParseSystem_ASeries)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // Default config is already ASeries-compatible.
    ASSERT_TRUE(specimen.tryParse(L"--system ASeries", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardwareArchitecture(),
              Arm::SystemModel::ASeries);
}

GTEST_TEST(CliOptions, ParseSystem_RiscPC)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // RiscPC requires ARM610+, speed>=20, valid RAM, and RISC OS 3.50+.
    ASSERT_TRUE(specimen.tryParse(
        L"--system RiscPC --cpu ARM610 --speed 30 --ram 4M "
        L"--rom RISCOS_3_50", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardwareArchitecture(),
              Arm::SystemModel::RiscPC);
}

GTEST_TEST(CliOptions, ParseSystem_TestBed)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // TestBed only supports ARM2/ARM3 with Custom ROM.
    ASSERT_TRUE(specimen.tryParse(
        L"--system TestBed --cpu ARM2 --rom Custom", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardwareArchitecture(),
              Arm::SystemModel::TestBed);
}

GTEST_TEST(CliOptions, ParseSystem_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--system FooBar", error));
}

////////////////////////////////////////////////////////////////////////////////
// --cpu (Processor Model) Tests
// Note: CPU must be compatible with the system model for validation to pass.
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseCpu_ARM2)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // ARM2 is compatible with the default ASeries-like config when using
    // an Archimedes base.
    ASSERT_TRUE(specimen.tryParse(
        L"--system Archimedes --cpu ARM2 --speed 8 --ram 1M --rom Arthur_1_20", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getProcessorVariant(),
              Arm::ProcessorModel::ARM2);
}

GTEST_TEST(CliOptions, ParseCpu_ARM250)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // ARM250 is the default CPU and compatible with default ASeries.
    ASSERT_TRUE(specimen.tryParse(L"--cpu ARM250", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getProcessorVariant(),
              Arm::ProcessorModel::ARM250);
}

GTEST_TEST(CliOptions, ParseCpu_ARM3)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--cpu ARM3 --speed 25", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getProcessorVariant(),
              Arm::ProcessorModel::ARM3);
}

GTEST_TEST(CliOptions, ParseCpu_ARM3_FPA)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--cpu ARM3_FPA --speed 25", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getProcessorVariant(),
              Arm::ProcessorModel::ARM3_FPA);
}

GTEST_TEST(CliOptions, ParseCpu_ARM610)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--system RiscPC --cpu ARM610 --speed 30 --ram 4M --rom RISCOS_3_50",
        error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getProcessorVariant(),
              Arm::ProcessorModel::ARM610);
}

GTEST_TEST(CliOptions, ParseCpu_StrongARM)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // StrongARM requires RiscPC with RISC OS 3.70+.
    ASSERT_TRUE(specimen.tryParse(
        L"--system RiscPC --cpu StrongARM --speed 233 --ram 4M "
        L"--rom RISCOS_3_70", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getProcessorVariant(),
              Arm::ProcessorModel::StrongARM);
}

GTEST_TEST(CliOptions, ParseCpu_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--cpu Z80", error));
}

////////////////////////////////////////////////////////////////////////////////
// --speed (Processor Speed) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseSpeed_Integer)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--speed 25", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getProcessorSpeedMHz(), 25);
}

GTEST_TEST(CliOptions, ParseSpeed_Fractional)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--speed 33.3", error));

    // 33.3 MHz -> 33300000 Hz -> truncated back to 33 MHz.
    EXPECT_EQ(specimen.getEmulatedSystemConfig().getProcessorSpeedMHz(), 33);
}

GTEST_TEST(CliOptions, ParseSpeed_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--speed notanumber", error));
}

////////////////////////////////////////////////////////////////////////////////
// --ram (RAM Size) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseRam_MegabytesSuffix)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--ram 2Mb", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getRamSizeKb(), 2048u);
}

GTEST_TEST(CliOptions, ParseRam_MegabytesSuffixAlone)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--ram 4M", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getRamSizeKb(), 4096u);
}

GTEST_TEST(CliOptions, ParseRam_KilobytesSuffix)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--system Archimedes --cpu ARM2 --speed 8 --ram 512K "
        L"--rom Arthur_1_20", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getRamSizeKb(), 512u);
}

GTEST_TEST(CliOptions, ParseRam_LargeValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--ram 16M", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getRamSizeKb(), 16384u);
}

GTEST_TEST(CliOptions, ParseRam_OneMegabyte)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--ram 1M", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getRamSizeKb(), 1024u);
}

GTEST_TEST(CliOptions, ParseRam_EightMegabytes)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--ram 8M", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getRamSizeKb(), 8192u);
}

GTEST_TEST(CliOptions, ParseRam_InvalidFormat)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--ram notasize", error));
}

////////////////////////////////////////////////////////////////////////////////
// --vram (Video RAM Size) Tests
// Note: VRAM is only valid on RiscPC systems (Archimedes/ASeries must have 0).
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseVRam_OneMegabyte)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--base RiscPC_600 --vram 1M", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getVideoRamSizeKb(), 1024u);
}

GTEST_TEST(CliOptions, ParseVRam_TwoMegabytes)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--base RiscPC_600 --vram 2M", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getVideoRamSizeKb(), 2048u);
}

GTEST_TEST(CliOptions, ParseVRam_FourMegabytes)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--base RiscPC_600 --vram 4M", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getVideoRamSizeKb(), 4096u);
}

GTEST_TEST(CliOptions, ParseVRam_KilobytesSuffix)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--base RiscPC_600 --vram 1024K", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getVideoRamSizeKb(), 1024u);
}

GTEST_TEST(CliOptions, ParseVRam_ZeroMegabytes)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // VRAM of 0 is valid on RiscPC (no dedicated VRAM).
    ASSERT_TRUE(specimen.tryParse(
        L"--base RiscPC_600 --vram 0M", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getVideoRamSizeKb(), 0u);
}

////////////////////////////////////////////////////////////////////////////////
// --display (Display Interface) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseDisplay_NormalTV)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // NormalTV is compatible with Archimedes/ASeries defaults.
    ASSERT_TRUE(specimen.tryParse(L"--display NormalTV", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getDisplayType(),
              Arm::DisplayInterface::NormalTV);
}

GTEST_TEST(CliOptions, ParseDisplay_HiResMono)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--display HiResMono", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getDisplayType(),
              Arm::DisplayInterface::HiResMono);
}

GTEST_TEST(CliOptions, ParseDisplay_MultiScan)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--display MultiScan", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getDisplayType(),
              Arm::DisplayInterface::MultiScan);
}

GTEST_TEST(CliOptions, ParseDisplay_VGA)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--display VGA", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getDisplayType(),
              Arm::DisplayInterface::VGA);
}

GTEST_TEST(CliOptions, ParseDisplay_SuperVGA)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--display SuperVGA", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getDisplayType(),
              Arm::DisplayInterface::SuperVGA);
}

GTEST_TEST(CliOptions, ParseDisplay_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--display OLED", error));
}

////////////////////////////////////////////////////////////////////////////////
// --hd_type (Hard Disk Interface) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseHDType_None)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // Must set hd_count to 0 when interface is None.
    ASSERT_TRUE(specimen.tryParse(L"--hd_type None --hd_count 0", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardDiskTechnology(),
              Arm::HardDiskInterface::None);
}

GTEST_TEST(CliOptions, ParseHDType_ST506)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--hd_type ST506", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardDiskTechnology(),
              Arm::HardDiskInterface::ST506);
}

GTEST_TEST(CliOptions, ParseHDType_IDE)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--hd_type IDE", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardDiskTechnology(),
              Arm::HardDiskInterface::IDE);
}

GTEST_TEST(CliOptions, ParseHDType_SCSI)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--hd_type SCSI", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardDiskTechnology(),
              Arm::HardDiskInterface::SCSI);
}

GTEST_TEST(CliOptions, ParseHDType_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--hd_type NVMe", error));
}

////////////////////////////////////////////////////////////////////////////////
// --hd_count (Hard Drive Count) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseHDCount_Zero)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--hd_count 0", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardDriveCount(), 0);
}

GTEST_TEST(CliOptions, ParseHDCount_Two)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // IDE supports max 2 drives.
    ASSERT_TRUE(specimen.tryParse(L"--hd_type IDE --hd_count 2", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getHardDriveCount(), 2);
}

GTEST_TEST(CliOptions, ParseHDCount_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--hd_count many", error));
}

////////////////////////////////////////////////////////////////////////////////
// --fd_count (Floppy Drive Count) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseFDCount_Zero)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--fd_count 0", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getFloppyDiskCount(), 0);
}

GTEST_TEST(CliOptions, ParseFDCount_Two)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--fd_count 2", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getFloppyDiskCount(), 2);
}

GTEST_TEST(CliOptions, ParseFDCount_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--fd_count lots", error));
}

////////////////////////////////////////////////////////////////////////////////
// --joystick_type (Joystick Interface) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseJoystickType_None)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--joystick_type None", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getJoystickType(),
              Arm::JoystickInterface::None);
}

GTEST_TEST(CliOptions, ParseJoystickType_Digital)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--joystick_type Digital", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getJoystickType(),
              Arm::JoystickInterface::Digital);
}

GTEST_TEST(CliOptions, ParseJoystickType_Analogue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--joystick_type Analogue", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getJoystickType(),
              Arm::JoystickInterface::Analog);
}

GTEST_TEST(CliOptions, ParseJoystickType_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--joystick_type Gyro", error));
}

////////////////////////////////////////////////////////////////////////////////
// --joy_count (Joystick Count) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseJoyCount_Zero)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--joy_count 0", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getJoystickCount(), 0);
}

GTEST_TEST(CliOptions, ParseJoyCount_Two)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--joy_count 2", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getJoystickCount(), 2);
}

GTEST_TEST(CliOptions, ParseJoyCount_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--joy_count abc", error));
}

////////////////////////////////////////////////////////////////////////////////
// --rom (System ROM Preset) Tests
// Note: ROM presets must be compatible with the system model.
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseRom_Custom)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--rom Custom", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getSystemRom(),
              Arm::SystemROMPreset::Custom);
}

GTEST_TEST(CliOptions, ParseRom_Arthur_1_20)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // Arthur ROMs are only compatible with Archimedes.
    ASSERT_TRUE(specimen.tryParse(
        L"--base A310 --rom Arthur_1_20", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getSystemRom(),
              Arm::SystemROMPreset::Arthur_1_20);
}

GTEST_TEST(CliOptions, ParseRom_RiscOS_2_00)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--base A310 --rom RISCOS_2_00", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getSystemRom(),
              Arm::SystemROMPreset::RiscOS_2_00);
}

GTEST_TEST(CliOptions, ParseRom_RiscOS_3_11)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // RiscOS 3.11 is compatible with the default ASeries config.
    ASSERT_TRUE(specimen.tryParse(L"--rom RISCOS_3_11", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getSystemRom(),
              Arm::SystemROMPreset::RiscOS_3_11);
}

GTEST_TEST(CliOptions, ParseRom_RiscOS_3_50)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // RiscOS 3.50 requires RiscPC with ARM610.
    ASSERT_TRUE(specimen.tryParse(
        L"--base RiscPC_600 --rom RISCOS_3_50", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getSystemRom(),
              Arm::SystemROMPreset::RiscOS_3_50);
}

GTEST_TEST(CliOptions, ParseRom_RiscOS_3_70)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // RiscOS 3.70 requires RiscPC with StrongARM.
    ASSERT_TRUE(specimen.tryParse(
        L"--base RiscPC_StrongARM --rom RISCOS_3_70", error));

    EXPECT_EQ(specimen.getEmulatedSystemConfig().getSystemRom(),
              Arm::SystemROMPreset::RiscOS_3_70);
}

GTEST_TEST(CliOptions, ParseRom_InvalidValue)
{
    CliOptions specimen(metadata);
    Ag::String error;

    EXPECT_FALSE(specimen.tryParse(L"--rom Windows_95", error));
}

////////////////////////////////////////////////////////////////////////////////
// --custom_rom (Custom ROM Path) Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseCustomRom_SetsCustomPresetAndPath)
{
    CliOptions specimen(metadata);
    Ag::String error;

#ifdef _WIN32
    ASSERT_TRUE(specimen.tryParse(L"--custom_rom C:\\ROMs\\MyRom.bin", error));
#else
    ASSERT_TRUE(specimen.tryParse(L"--custom_rom /tmp/ROMs/MyRom.bin", error));
#endif

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::Custom);
    EXPECT_FALSE(config.getRomPath().isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
// Combined Options Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, ParseCombined_ArchimedesConfiguration)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--system Archimedes --cpu ARM2 --speed 8 --ram 1M "
        L"--display MultiScan --hd_type ST506 --hd_count 1 --fd_count 1 "
        L"--rom Arthur_1_20", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::Archimedies);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM2);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 8);
    EXPECT_EQ(config.getRamSizeKb(), 1024u);
    EXPECT_EQ(config.getDisplayType(), Arm::DisplayInterface::MultiScan);
    EXPECT_EQ(config.getHardDiskTechnology(), Arm::HardDiskInterface::ST506);
    EXPECT_EQ(config.getHardDriveCount(), 1);
    EXPECT_EQ(config.getFloppyDiskCount(), 1);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::Arthur_1_20);
    EXPECT_TRUE(config.validate(error));
}

GTEST_TEST(CliOptions, ParseCombined_RiscPCConfiguration)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--system RiscPC --cpu StrongARM --speed 233 --ram 16M "
        L"--vram 2M --display VGA --hd_type IDE --hd_count 1 --fd_count 1 "
        L"--joystick_type None --joy_count 0 --rom RISCOS_3_70", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::RiscPC);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::StrongARM);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 233);
    EXPECT_EQ(config.getRamSizeKb(), 16384u);
    EXPECT_EQ(config.getVideoRamSizeKb(), 2048u);
    EXPECT_EQ(config.getDisplayType(), Arm::DisplayInterface::VGA);
    EXPECT_EQ(config.getHardDiskTechnology(), Arm::HardDiskInterface::IDE);
    EXPECT_EQ(config.getHardDriveCount(), 1);
    EXPECT_EQ(config.getFloppyDiskCount(), 1);
    EXPECT_EQ(config.getJoystickType(), Arm::JoystickInterface::None);
    EXPECT_EQ(config.getJoystickCount(), 0);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::RiscOS_3_70);
    EXPECT_TRUE(config.validate(error));
}

GTEST_TEST(CliOptions, ParseCombined_ASeriesConfiguration)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(
        L"--system ASeries --cpu ARM250 --speed 12 --ram 2M "
        L"--display MultiScan --hd_type IDE --hd_count 1 --fd_count 1 "
        L"--joystick_type Digital --joy_count 2 --rom RISCOS_3_10", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::ASeries);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM250);
    EXPECT_EQ(config.getProcessorSpeedMHz(), 12);
    EXPECT_EQ(config.getRamSizeKb(), 2048u);
    EXPECT_EQ(config.getVideoRamSizeKb(), 0u);
    EXPECT_EQ(config.getDisplayType(), Arm::DisplayInterface::MultiScan);
    EXPECT_EQ(config.getHardDiskTechnology(), Arm::HardDiskInterface::IDE);
    EXPECT_EQ(config.getHardDriveCount(), 1);
    EXPECT_EQ(config.getFloppyDiskCount(), 1);
    EXPECT_EQ(config.getJoystickType(), Arm::JoystickInterface::Digital);
    EXPECT_EQ(config.getJoystickCount(), 2);
    EXPECT_EQ(config.getSystemRom(), Arm::SystemROMPreset::RiscOS_3_10);
    EXPECT_TRUE(config.validate(error));
}

GTEST_TEST(CliOptions, ParseBase_OverriddenBySubsequentOptions)
{
    CliOptions specimen(metadata);
    Ag::String error;

    // Start with A305 base (512K RAM), then override RAM to 4M.
    ASSERT_TRUE(specimen.tryParse(L"--base A305 --ram 4M", error));

    const auto &config = specimen.getEmulatedSystemConfig();
    // The --base sets the A305 stereotype, then --ram overrides the RAM size.
    EXPECT_EQ(config.getHardwareArchitecture(), Arm::SystemModel::Archimedies);
    EXPECT_EQ(config.getProcessorVariant(), Arm::ProcessorModel::ARM2);
    EXPECT_EQ(config.getRamSizeKb(), 4096u);
}

GTEST_TEST(CliOptions, ParseAllCommandsSetsRunSession)
{
    CliOptions specimen(metadata);
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--base A3010", error));

    EXPECT_EQ(specimen.getCommand(), Ag::toScalar(AppCommand::RunSession));
}

} // Anonymous namespace

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


//! @file Test_Options.cpp
//! @brief The definition of unit tests for the Options class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "ArmEmu/EmuOptions.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Options, DefaultIsValid)
{
    Options specimen;
    Ag::String error;

    EXPECT_TRUE(specimen.validate(error));
    EXPECT_TRUE(error.isEmpty()) << error.toUtf8();
}

GTEST_TEST(Options, ValidTestBedConfiguration)
{
    Options specimen;
    Ag::String error;

    specimen.setHardwareArchitecture(SystemModel::TestBed);
    specimen.setProcessorVariant(ProcessorModel::ARM2);
    specimen.setSystemRom(SystemROMPreset::Custom);
    specimen.setRamSizeKb(32);

    EXPECT_TRUE(specimen.validate(error));
    EXPECT_TRUE(error.isEmpty()) << error.toUtf8();
}

GTEST_TEST(Options, ValidArchimedesConfiguration)
{
    Options specimen;
    Ag::String error;

    specimen.setHardwareArchitecture(SystemModel::Archimedies);
    specimen.setProcessorVariant(ProcessorModel::ARM3);
    specimen.setProcessorSpeedMHz(25);
    specimen.setSystemRom(SystemROMPreset::RiscOS_2_00);
    specimen.setRamSizeKb(1024);
    specimen.setHardDiskTechnology(HardDiskInterface::ST506);
    specimen.setHardDriveCount(1);
    specimen.setFloppyDiskCount(2);
    specimen.setDisplayType(DisplayInterface::HiResMono);
    specimen.setJoystickType(JoystickInterface::None);

    EXPECT_TRUE(specimen.validate(error));
    EXPECT_TRUE(error.isEmpty()) << error.toUtf8();
}

GTEST_TEST(Options, ValidA5000Configuration)
{
    Options specimen;
    Ag::String error;

    specimen.setHardwareArchitecture(SystemModel::ASeries);
    specimen.setProcessorVariant(ProcessorModel::ARM3_FPA);
    specimen.setProcessorSpeedMHz(33);
    specimen.setSystemRom(SystemROMPreset::RiscOS_3_00);
    specimen.setRamSizeKb(8192);
    specimen.setHardDiskTechnology(HardDiskInterface::IDE);
    specimen.setHardDriveCount(1);
    specimen.setFloppyDiskCount(1);
    specimen.setDisplayType(DisplayInterface::VGA);
    specimen.setJoystickType(JoystickInterface::None);

    EXPECT_TRUE(specimen.validate(error));
    EXPECT_TRUE(error.isEmpty()) << error.toUtf8();
}

GTEST_TEST(Options, ValidRiscPC600Configuration)
{
    Options specimen;
    Ag::String error;

    specimen.setHardwareArchitecture(SystemModel::RiscPC);
    specimen.setProcessorVariant(ProcessorModel::ARM610);
    specimen.setProcessorSpeedMHz(30);
    specimen.setSystemRom(SystemROMPreset::RiscOS_3_50);
    specimen.setRamSizeKb(4096);
    specimen.setVideoRamSizeKb(1024);
    specimen.setHardDiskTechnology(HardDiskInterface::IDE);
    specimen.setHardDriveCount(1);
    specimen.setFloppyDiskCount(1);
    specimen.setDisplayType(DisplayInterface::SuperVGA);

    EXPECT_TRUE(specimen.validate(error));
    EXPECT_TRUE(error.isEmpty()) << error.toUtf8();
}

GTEST_TEST(Options, ValidA7000Configuration)
{
    Options specimen;
    Ag::String error;

    specimen.setHardwareArchitecture(SystemModel::RiscPC);
    specimen.setProcessorVariant(ProcessorModel::ARM710_FPA);
    specimen.setProcessorSpeedMHz(48);
    specimen.setSystemRom(SystemROMPreset::RiscOS_3_60);
    specimen.setRamSizeKb(16384);
    specimen.setVideoRamSizeKb(0);
    specimen.setHardDiskTechnology(HardDiskInterface::IDE);
    specimen.setHardDriveCount(1);
    specimen.setFloppyDiskCount(1);
    specimen.setDisplayType(DisplayInterface::SuperVGA);
    specimen.setJoystickType(JoystickInterface::Analog);

    EXPECT_TRUE(specimen.validate(error));
    EXPECT_TRUE(error.isEmpty()) << error.toUtf8();
}

GTEST_TEST(Options, ValidStrongARMRiscPCConfiguration)
{
    Options specimen;
    Ag::String error;

    specimen.setHardwareArchitecture(SystemModel::RiscPC);
    specimen.setProcessorVariant(ProcessorModel::StrongARM);
    specimen.setProcessorSpeedMHz(233);
    specimen.setSystemRom(SystemROMPreset::RiscOS_3_71);
    specimen.setRamSizeKb(32768);
    specimen.setVideoRamSizeKb(2048);
    specimen.setHardDiskTechnology(HardDiskInterface::IDE);
    specimen.setHardDriveCount(2);
    specimen.setFloppyDiskCount(1);
    specimen.setDisplayType(DisplayInterface::SuperVGA);

    EXPECT_TRUE(specimen.validate(error));
    EXPECT_TRUE(error.isEmpty()) << error.toUtf8();
}


GTEST_TEST(Options, InvalidArchimedesConfiguration)
{
    Options specimen;
    Ag::String error;

    specimen.setHardwareArchitecture(SystemModel::Archimedies);
    specimen.setProcessorVariant(ProcessorModel::ARM3);
    specimen.setProcessorSpeedMHz(25);
    specimen.setSystemRom(SystemROMPreset::RiscOS_3_10);
    specimen.setRamSizeKb(1024);
    specimen.setHardDiskTechnology(HardDiskInterface::ST506);
    specimen.setHardDriveCount(1);
    specimen.setFloppyDiskCount(2);
    specimen.setDisplayType(DisplayInterface::HiResMono);
    specimen.setJoystickType(JoystickInterface::None);

    EXPECT_TRUE(specimen.validate(error));
    EXPECT_TRUE(error.isEmpty()) << error.toUtf8();

    specimen.setProcessorVariant(ProcessorModel::ARM810);
    EXPECT_FALSE(specimen.validate(error));
    EXPECT_TRUE(error.contains("processor"));
    specimen.setProcessorVariant(ProcessorModel::ARM3);

    specimen.setRamSizeKb(256);
    EXPECT_FALSE(specimen.validate(error));
    EXPECT_TRUE(error.contains("RAM"));
    specimen.setRamSizeKb(4096);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


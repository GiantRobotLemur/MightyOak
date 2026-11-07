//! @file Test_PCF8583.cpp
//! @brief The definition of unit tests for the PCF8583 RTC/CMOS RAM emulation,
//! verifying register pointer management, RAM read/write behaviour, auto-
//! increment, and default CMOS configuration values.
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

#include "PCF8583.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A test fixture for the PCF8583 RTC/CMOS RAM chip, providing a
//! pre-constructed device instance.
class PCF8583Test : public ::testing::Test
{
protected:
    PCF8583 _device;
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Address and Defaults
////////////////////////////////////////////////////////////////////////////////
TEST_F(PCF8583Test, Address_Returns0x50)
{
    EXPECT_EQ(_device.getI2CAddress(), 0x50);
}

TEST_F(PCF8583Test, DefaultRam_ControlRegister)
{
    // Register 0x00 should be 0x04 (32.768 kHz, clock mode, mask flag set).
    EXPECT_EQ(_device.getRam()[0x00], 0x04);
}

TEST_F(PCF8583Test, DefaultRam_CMOSDefaults)
{
    const uint8_t *ram = _device.getRam();
    constexpr uint8_t cmosBase = 0x10;

    // Spot-check known CMOS default values.
    EXPECT_EQ(ram[cmosBase + 10], 12);   // Screen mode = 12
    EXPECT_EQ(ram[cmosBase + 11], 12);   // Wimp mode = 12
    EXPECT_EQ(ram[cmosBase + 12], 0x01); // Monitor type = multiscan
    EXPECT_EQ(ram[cmosBase + 16], 32);   // Key repeat delay
    EXPECT_EQ(ram[cmosBase + 17], 8);    // Key repeat rate
    EXPECT_EQ(ram[cmosBase + 28], 2);    // Mouse step
    EXPECT_EQ(ram[cmosBase + 5], 0x03);  // FS/boot = ADFS
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Write Operations
////////////////////////////////////////////////////////////////////////////////
TEST_F(PCF8583Test, WriteRegisterPointer_SetsPosition)
{
    // First write (isFirst=true) sets the register pointer.
    _device.onI2CWrite(0x40, true);

    // Read should return contents at address 0x40.
    uint8_t data = 0;
    _device.onI2CRead(data);

    EXPECT_EQ(data, _device.getRam()[0x40]);
}

TEST_F(PCF8583Test, WriteData_StoresInRam)
{
    // Set pointer to 0x10.
    _device.onI2CWrite(0x10, true);

    // Write data byte.
    _device.onI2CWrite(0xAB, false);

    EXPECT_EQ(_device.getRam()[0x10], 0xAB);
}

TEST_F(PCF8583Test, WriteData_AutoIncrements)
{
    // Set pointer to 0x10.
    _device.onI2CWrite(0x10, true);

    // Write three consecutive bytes.
    _device.onI2CWrite(0xAA, false);
    _device.onI2CWrite(0xBB, false);
    _device.onI2CWrite(0xCC, false);

    EXPECT_EQ(_device.getRam()[0x10], 0xAA);
    EXPECT_EQ(_device.getRam()[0x11], 0xBB);
    EXPECT_EQ(_device.getRam()[0x12], 0xCC);
}

TEST_F(PCF8583Test, WritePointer_WrapAround)
{
    // Set pointer to 0xFF.
    _device.onI2CWrite(0xFF, true);

    // Write a byte at 0xFF.
    _device.onI2CWrite(0x11, false);

    EXPECT_EQ(_device.getRam()[0xFF], 0x11);

    // Next write should wrap to 0x00 (uint8_t overflow).
    _device.onI2CWrite(0x22, false);

    EXPECT_EQ(_device.getRam()[0x00], 0x22);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Read Operations
////////////////////////////////////////////////////////////////////////////////
TEST_F(PCF8583Test, ReadData_ReturnsRamContents)
{
    // Pre-fill RAM at a known address.
    _device.getRam()[0x30] = 0x5A;

    // Set pointer to 0x30.
    _device.onI2CWrite(0x30, true);

    // Read should return 0x5A.
    uint8_t data = 0;
    _device.onI2CRead(data);

    EXPECT_EQ(data, 0x5A);
}

TEST_F(PCF8583Test, ReadData_AutoIncrements)
{
    // Pre-fill three consecutive RAM addresses.
    _device.getRam()[0x20] = 0x01;
    _device.getRam()[0x21] = 0x02;
    _device.getRam()[0x22] = 0x03;

    // Set pointer to 0x20.
    _device.onI2CWrite(0x20, true);

    uint8_t d1 = 0, d2 = 0, d3 = 0;
    _device.onI2CRead(d1);
    _device.onI2CRead(d2);
    _device.onI2CRead(d3);

    EXPECT_EQ(d1, 0x01);
    EXPECT_EQ(d2, 0x02);
    EXPECT_EQ(d3, 0x03);
}

TEST_F(PCF8583Test, ReadPointer_WrapAround)
{
    _device.getRam()[0xFF] = 0xEE;
    _device.getRam()[0x00] = 0x04; // Default value, overwrite for clarity
    _device.getRam()[0x00] = 0xDD;

    // Set pointer to 0xFF.
    _device.onI2CWrite(0xFF, true);

    uint8_t d1 = 0, d2 = 0;
    _device.onI2CRead(d1);
    _device.onI2CRead(d2);

    EXPECT_EQ(d1, 0xEE); // From 0xFF
    EXPECT_EQ(d2, 0xDD); // Wrapped to 0x00
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Stop Behaviour
////////////////////////////////////////////////////////////////////////////////
TEST_F(PCF8583Test, Stop_ClearsPointerSetFlag)
{
    // Set pointer to 0x30.
    _device.onI2CWrite(0x30, true);

    // Stop clears the pointer-set flag.
    _device.onI2CStop();

    // On a new transaction, the first write (isFirst=true) should set a new
    // pointer rather than writing data. Set pointer to 0x40.
    _device.onI2CWrite(0x40, true);

    // Read should return contents at 0x40, not 0x30.
    uint8_t data = 0;
    _device.onI2CRead(data);

    EXPECT_EQ(data, _device.getRam()[0x40]);
}

TEST_F(PCF8583Test, Stop_PointerValuePersists)
{
    // Pre-fill RAM at 0x20.
    _device.getRam()[0x20] = 0x77;

    // Set pointer to 0x20.
    _device.onI2CWrite(0x20, true);

    // Stop the transaction.
    _device.onI2CStop();

    // New transaction: set pointer to 0x20 again.
    _device.onI2CWrite(0x20, true);

    // Read should return the value at 0x20.
    uint8_t data = 0;
    _device.onI2CRead(data);

    EXPECT_EQ(data, 0x77);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

//! @file Test_MemcHardware.cpp
//! @brief The definition of unit tests for the MemcHardware class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "MemcHardware.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
class MemcHardwareTests : public ::testing::Test
{
protected:
    AddressMap _readDevices, _writeDevices;
    MemcHardware specimen;

    MemcHardwareTests() :
        specimen(Options(), _readDevices, _writeDevices)
    {
        specimen.reset();
    }
};


////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
uint32_t make4KMapping(uint16_t logicalPage, uint16_t physPage, uint8_t ppl)
{
    uint32_t address = 0x3800000;

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 12, 10>(logicalPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 11, 10, 2>(logicalPage);

    address |= Ag::Bin::extractBits<uint32_t, 0, 7>(physPage);

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 8, 2>(ppl);

    // Only 2 MEMC chips are supported when the page size is 4 KB.
    address |= Ag::Bin::extractAndShiftBits<uint8_t, 7, 7, 1>(physPage);

    return address;
}

uint32_t make8KMapping(uint16_t logicalPage, uint16_t physPage, uint8_t ppl)
{
    uint32_t address = 0x3800000;

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 13, 10>(logicalPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 10, 10, 2>(logicalPage);

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 1, 6>(physPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 6, 0, 1>(physPage);

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 8, 2>(ppl);

    uint8_t memcId = Ag::Bin::extractBits<uint8_t, 7, 2>(physPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 7, 1>(memcId);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 1, 12, 1>(memcId);

    return address;
}

uint32_t make16KMapping(uint16_t logicalPage, uint16_t physPage, uint8_t ppl)
{
    uint32_t address = 0x3800000;

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 14, 9>(logicalPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 9, 10, 2>(logicalPage);

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 5, 0, 2>(physPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 2, 5>(physPage);

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 8, 2>(ppl);

    uint8_t memcId = Ag::Bin::extractBits<uint8_t, 7, 2>(physPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 7, 1>(memcId);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 1, 12, 1>(memcId);

    return address;
}

uint32_t make32KMapping(uint16_t logicalPage, uint16_t physPage, uint8_t ppl)
{
    uint32_t address = 0x3800000;

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 15, 8>(logicalPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 9, 10, 2>(logicalPage);

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 3, 4>(physPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 4, 0, 1>(physPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 5, 2, 1>(physPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 6, 1, 1>(physPage);

    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 8, 2>(ppl);

    uint8_t memcId = Ag::Bin::extractBits<uint8_t, 7, 2>(physPage);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 0, 7, 1>(memcId);
    address |= Ag::Bin::extractAndShiftBits<uint32_t, 1, 12, 1>(memcId);

    return address;
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
TEST_F(MemcHardwareTests, ReadWriteUnprivilged)
{
    specimen.setPrivilegedMode(false);

    // Try to write the MEMC control register.
    EXPECT_FALSE(specimen.write<uint32_t>(0x36E0008, 42));

    // Try writing to physical memory
    EXPECT_FALSE(specimen.write<uint32_t>(MEMC::PhysRamStart + 8, 42));

    // Try reading from physical memory.
    uint32_t value = 0xCAFEBABE;
    EXPECT_FALSE(specimen.read<uint32_t>(MEMC::PhysRamStart + 8, value));
    EXPECT_EQ(value, 0xCAFEBABEu);

    // Try reading from Low ROM.
    EXPECT_TRUE(specimen.read<uint32_t>(MEMC::LowRomStart + 8, value));

    // Try writing to Low ROM (actually VIDC address space).
    EXPECT_FALSE(specimen.write<uint32_t>(MEMC::LowRomStart + 8, 0xDEADBEEF));

    // Try reading from High ROM.
    EXPECT_TRUE(specimen.read<uint32_t>(MEMC::HighRomStart + 8, value));

    // Try writing to Low ROM (actually MEMC address translation address space).
    EXPECT_FALSE(specimen.write<uint32_t>(MEMC::LowRomStart + 8, 0xDEADBEEF));
}

TEST_F(MemcHardwareTests, EnablingTestModeThrows)
{
    specimen.setPrivilegedMode(true);

    // Try enabling test mode.
    EXPECT_THROW({ specimen.write<uint32_t>(0x36E2008, 42); },
                 Ag::OperationException);
}

TEST_F(MemcHardwareTests, Create4KLogicalAddressMapping)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 4 KB.
    constexpr uint8_t PageSizePow2 = 12;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 0.
    EXPECT_TRUE(specimen.write<uint32_t>(make4KMapping(2, 1, 0), 0));

    // Write to an address in logical page 0.
    static constexpr uint32_t SampleValue = 0xDEADBEEF;

    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Read the value back via address translation.
    uint32_t value;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0C;
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Try writing direct to untranslated memory and read from translated.
    static constexpr uint32_t SampleValue2 = 0xCAFEBABE;
    logicalAddr -= 4;
    physicalAddr -= 4;
    EXPECT_TRUE(specimen.write(physicalAddr, SampleValue2));

    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue2);
}

TEST_F(MemcHardwareTests, Create8KLogicalAddressMapping)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 8 KB.
    constexpr uint8_t PageSizePow2 = 13;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 0.
    EXPECT_TRUE(specimen.write<uint32_t>(make8KMapping(2, 1, 0), 0));

    // Write to an address in logical page 0.
    static constexpr uint32_t SampleValue = 0xDEADBEEF;

    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Read the value back via address translation.
    uint32_t value;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0C;
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Try writing direct to untranslated memory and read from translated.
    static constexpr uint32_t SampleValue2 = 0xCAFEBABE;
    logicalAddr -= 4;
    physicalAddr -= 4;
    EXPECT_TRUE(specimen.write(physicalAddr, SampleValue2));

    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue2);
}

TEST_F(MemcHardwareTests, Create16KLogicalAddressMapping)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 16 KB.
    constexpr uint8_t PageSizePow2 = 14;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 0.
    EXPECT_TRUE(specimen.write<uint32_t>(make16KMapping(2, 1, 0), 0));

    // Write to an address in logical page 0.
    static constexpr uint32_t SampleValue = 0xDEADBEEF;

    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Read the value back via address translation.
    uint32_t value;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0C;
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Try writing direct to untranslated memory and read from translated.
    static constexpr uint32_t SampleValue2 = 0xCAFEBABE;
    logicalAddr -= 4;
    physicalAddr -= 4;
    EXPECT_TRUE(specimen.write(physicalAddr, SampleValue2));

    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue2);
}

TEST_F(MemcHardwareTests, Create32KLogicalAddressMapping)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 32 KB.
    constexpr uint8_t PageSizePow2 = 15;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 0.
    EXPECT_TRUE(specimen.write<uint32_t>(make32KMapping(2, 1, 0), 0));

    // Write to an address in logical page 0.
    static constexpr uint32_t SampleValue = 0xDEADBEEF;

    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Read the value back via address translation.
    uint32_t value;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0C;
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Try writing direct to untranslated memory and read from translated.
    static constexpr uint32_t SampleValue2 = 0xCAFEBABE;
    logicalAddr -= 4;
    physicalAddr -= 4;
    EXPECT_TRUE(specimen.write(physicalAddr, SampleValue2));

    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue2);
}

TEST_F(MemcHardwareTests, PageProtectionLevel1)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 16 KB.
    constexpr uint8_t PageSizePow2 = 14;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 2, PPL = 1 (user read-only).
    EXPECT_TRUE(specimen.write<uint32_t>(make16KMapping(2, 1, 1), 0));

    // Write to an address in logical page 2.
    constexpr uint32_t SampleValue = 0xDEADBEEF;
    constexpr uint32_t Sample2 = 0xCAFEBABE;
    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0C;
    uint32_t value = 0;

    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Switch to an unprivileged mode /////////////////////////////////////////
    specimen.setPrivilegedMode(false);

    // Read the value back via address translation.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    value = 0;
    EXPECT_FALSE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, 0u);

    // Write the value via its logical address.
    EXPECT_FALSE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is unchanged.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);


    // Switch to OS mode //////////////////////////////////////////////////////
    specimen.setPrivilegedMode(true);
    EXPECT_TRUE(specimen.write<uint32_t>(0x36E1000 | (PageSizePow2 - 12) << 2, 0));
    specimen.setPrivilegedMode(false);

    // Read the value back via address translation.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    value = 0;
    EXPECT_FALSE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, 0u);

    // Write the value via its logical address.
    EXPECT_TRUE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is changed.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, Sample2);


    // Switch to privilege + OS mode //////////////////////////////////////////
    specimen.setPrivilegedMode(true);

    // Read the value back via address translation.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, Sample2);

    // Read the value back via direct access.
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, Sample2);

    // Write the value via its logical address.
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Confirm the value is changed.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);


    // Switch to privilege mode with OS mode disabled /////////////////////////
    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Read the value back via address translation.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Write the value via its logical address.
    EXPECT_TRUE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is changed.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, Sample2);
}

TEST_F(MemcHardwareTests, PageProtectionLevel2)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 16 KB.
    constexpr uint8_t PageSizePow2 = 14;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 2, PPL = 2 (OS read-only).
    EXPECT_TRUE(specimen.write<uint32_t>(make16KMapping(2, 1, 2), 0));

    // Write to an address in logical page 2.
    constexpr uint32_t SampleValue = 0xDEADBEEF;
    constexpr uint32_t Sample2 = 0xCAFEBABE;
    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0C;
    uint32_t value = 0;

    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Switch to an unprivileged mode /////////////////////////////////////////
    specimen.setPrivilegedMode(false);

    // Read the value back via address translation.
    EXPECT_FALSE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, 0u);

    // Read the value back via direct access.
    value = 0;
    EXPECT_FALSE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, 0u);

    // Write the value via its logical address.
    EXPECT_FALSE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is unchanged.
    value = 0;
    EXPECT_FALSE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, 0u);


    // Switch to OS mode //////////////////////////////////////////////////////
    specimen.setPrivilegedMode(true);
    EXPECT_TRUE(specimen.write<uint32_t>(0x36E1000 | (PageSizePow2 - 12) << 2, 0));
    specimen.setPrivilegedMode(false);

    // Read the value back via address translation.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    value = 0;
    EXPECT_FALSE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, 0u);

    // Write the value via its logical address.
    EXPECT_FALSE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is changed.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);


    // Switch to privilege + OS mode //////////////////////////////////////////
    specimen.setPrivilegedMode(true);

    // Read the value back via address translation.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Write the value via its logical address.
    EXPECT_TRUE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is changed.
    value = 0;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, Sample2);


    // Switch to privilege mode with OS mode disabled /////////////////////////
    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Read the value back via address translation.
    value = 0;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, Sample2);

    // Read the value back via direct access.
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, Sample2);

    // Write the value via its logical address.
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Confirm the value is changed.
    value = 0;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);
}

TEST_F(MemcHardwareTests, PageProtectionLevel3)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 16 KB.
    constexpr uint8_t PageSizePow2 = 14;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 2, PPL = 3 (OS read-only).
    EXPECT_TRUE(specimen.write<uint32_t>(make16KMapping(2, 1, 3), 0));

    // Write to an address in logical page 2.
    constexpr uint32_t SampleValue = 0xDEADBEEF;
    constexpr uint32_t Sample2 = 0xCAFEBABE;
    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0C;
    uint32_t value = 0;

    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Switch to an unprivileged mode /////////////////////////////////////////
    specimen.setPrivilegedMode(false);

    // Read the value back via address translation.
    EXPECT_FALSE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, 0u);

    // Read the value back via direct access.
    value = 0;
    EXPECT_FALSE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, 0u);

    // Write the value via its logical address.
    EXPECT_FALSE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is unchanged.
    value = 0;
    EXPECT_FALSE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, 0u);


    // Switch to OS mode //////////////////////////////////////////////////////
    specimen.setPrivilegedMode(true);
    EXPECT_TRUE(specimen.write<uint32_t>(0x36E1000 | (PageSizePow2 - 12) << 2, 0));
    specimen.setPrivilegedMode(false);

    // Read the value back via address translation.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    value = 0;
    EXPECT_FALSE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, 0u);

    // Write the value via its logical address.
    EXPECT_FALSE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is changed.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);


    // Switch to privilege + OS mode //////////////////////////////////////////
    specimen.setPrivilegedMode(true);

    // Read the value back via address translation.
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Read the value back via direct access.
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue);

    // Write the value via its logical address.
    EXPECT_TRUE(specimen.write(logicalAddr, Sample2));

    // Confirm the value is changed.
    value = 0;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, Sample2);


    // Switch to privilege mode with OS mode disabled /////////////////////////
    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Read the value back via address translation.
    value = 0;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, Sample2);

    // Read the value back via direct access.
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, Sample2);

    // Write the value via its logical address.
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Confirm the value is changed.
    value = 0;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue);
}

TEST_F(MemcHardwareTests, InitialROMMapping)
{
    // Initialise the low ROM.
    const uint32_t sampleRomBytes[] = {
        0xDEADBEEF,
        0xCAFEBABE,
        0x42692496,
    };

    specimen.setLowRom(reinterpret_cast<const uint8_t *>(sampleRomBytes),
                       sizeof(sampleRomBytes));

    // Verify that reading from low logical addresses read the ROM.
    uint32_t value = 0;
    EXPECT_TRUE(specimen.read(0, value));
    EXPECT_EQ(value, sampleRomBytes[0]);

    value = 0;
    EXPECT_TRUE(specimen.read(4, value));
    EXPECT_EQ(value, sampleRomBytes[1]);

    value = 0;
    EXPECT_TRUE(specimen.read(8, value));
    EXPECT_EQ(value, sampleRomBytes[2]);

    // Verify the contents of the ROM in-place.
    value = 0;
    EXPECT_TRUE(specimen.read(MEMC::LowRomStart, value));
    EXPECT_EQ(value, sampleRomBytes[0]);

    value = 0;
    EXPECT_TRUE(specimen.read(MEMC::LowRomStart + 4, value));
    EXPECT_EQ(value, sampleRomBytes[1]);

    value = 0;
    EXPECT_TRUE(specimen.read(MEMC::LowRomStart + 8, value));
    EXPECT_EQ(value, sampleRomBytes[2]);
}

TEST_F(MemcHardwareTests, ReadWriteByte)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 32 KB.
    constexpr uint8_t PageSizePow2 = 15;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 2.
    EXPECT_TRUE(specimen.write<uint32_t>(make32KMapping(2, 1, 0), 0));

    // Write to an address in logical page 2.
    static constexpr uint32_t SampleValue = 0xDEADBEEF;
    static constexpr uint8_t SampleValue8 = 0xBE;

    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Read the value back via address translation.
    ++logicalAddr;
    uint8_t value;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue8);

    // Read the value back via direct access.
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0D;
    value = 0;
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue8);

    // Try writing direct to untranslated memory and read from translated.
    static constexpr uint8_t SampleValue2 = 0x5A;
    logicalAddr -= 4;
    physicalAddr -= 4;
    EXPECT_TRUE(specimen.write(physicalAddr, SampleValue2));

    value = 0;
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue2);
}

TEST_F(MemcHardwareTests, ReadWriteWords)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 32 KB.
    constexpr uint8_t PageSizePow2 = 15;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 2.
    EXPECT_TRUE(specimen.write<uint32_t>(make32KMapping(2, 1, 0), 0));

    // Map physical page 0 to logical page 3.
    EXPECT_TRUE(specimen.write<uint32_t>(make32KMapping(3, 0, 0), 0));

    // Write to an address in logical page 2.
    const uint32_t SampleWords[] = {
        0xDEADBEEF,
        0xCAFEBABE,
        0x5A4269A5
    };

    // Ensure the write will cross page boundaries.
    uint32_t logicalAddr = (PageSize * 3) - 8;
    EXPECT_TRUE(specimen.writeWords(logicalAddr, SampleWords, 3));

    // Read the value back via address translation.
    uint32_t values[3];
    EXPECT_TRUE(specimen.readWords(logicalAddr, values, 3));
    EXPECT_TRUE(std::equal(SampleWords, SampleWords + 3, values));

    // Read the value back via direct access.
    uint32_t physicalAddr = MEMC::PhysRamStart + (PageSize * 2) - 8;
    std::fill_n(values, 3, 0);
    EXPECT_TRUE(specimen.readWords(physicalAddr, values, 3));

    // Expect the first two words to match, but the third to be from the wrong page.
    EXPECT_TRUE(std::equal(SampleWords, SampleWords + 2, values));
    EXPECT_NE(SampleWords[2], values[2]);

    // Try writing direct to untranslated memory and read from translated.
    logicalAddr -= 4;
    physicalAddr -= 4;
    EXPECT_TRUE(specimen.writeWords(physicalAddr, SampleWords, 3));

    std::fill_n(values, 3, 0);
    EXPECT_TRUE(specimen.readWords(logicalAddr, values, 3));
    EXPECT_TRUE(std::equal(SampleWords, SampleWords + 3, values));
}

TEST_F(MemcHardwareTests, ExchangeWords)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 16 KB.
    constexpr uint8_t PageSizePow2 = 14;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 0.
    EXPECT_TRUE(specimen.write<uint32_t>(make16KMapping(2, 1, 0), 0));

    // Write to an address in logical page 0.
    static constexpr uint32_t SampleValue = 0xDEADBEEF;
    static constexpr uint32_t SampleValue2 = 0xCAFEBABE;

    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));

    // Read the value back via address translation.
    uint32_t value;
    EXPECT_TRUE(specimen.exchange(logicalAddr, SampleValue2, value));
    EXPECT_EQ(value, SampleValue);
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, SampleValue2);

    // Read the value back via direct access.
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0C;
    value = 0;
    EXPECT_TRUE(specimen.exchange(physicalAddr, SampleValue, value));
    EXPECT_EQ(value, SampleValue2);
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, SampleValue);
}

TEST_F(MemcHardwareTests, ExchangeBytes)
{
    specimen.setPrivilegedMode(true);

    // Set page size to 16 KB.
    constexpr uint8_t PageSizePow2 = 14;
    constexpr uint32_t PageSize = static_cast<uint32_t>(1) << PageSizePow2;

    EXPECT_TRUE(specimen.write<uint32_t>(0x36E0000 | (PageSizePow2 - 12) << 2, 0));

    // Map physical page 1 to logical page 0.
    EXPECT_TRUE(specimen.write<uint32_t>(make16KMapping(2, 1, 0), 0));

    // Write to an address in logical page 0.
    static constexpr uint32_t SampleValue = 0xDEADBEEF;
    static constexpr uint8_t Sample8 = 0x5A;
    static constexpr uint8_t AltSample8 = 0x42;

    uint32_t logicalAddr = 0x0C + (PageSize * 2);
    EXPECT_TRUE(specimen.write(logicalAddr, SampleValue));
    ++logicalAddr;

    // Read the value back via address translation.
    uint8_t value;
    EXPECT_TRUE(specimen.exchange(logicalAddr, Sample8, value));
    EXPECT_EQ(value, 0xBE);
    EXPECT_TRUE(specimen.read(logicalAddr, value));
    EXPECT_EQ(value, Sample8);

    // Read the value back via direct access.
    uint32_t physicalAddr = MEMC::PhysRamStart + PageSize + 0x0D;
    value = 0;
    EXPECT_TRUE(specimen.exchange(physicalAddr, AltSample8, value));
    EXPECT_EQ(value, Sample8);
    EXPECT_TRUE(specimen.read(physicalAddr, value));
    EXPECT_EQ(value, AltSample8);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


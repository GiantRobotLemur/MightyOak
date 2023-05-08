//! @file Test_ArmSystemBuilder.cpp
//! @brief The definition of unit tests for the ArmSystemBuilder and related
//! classes.
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

#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/ArmSystemBuilder.hpp"
#include "TestBedHardware.inl"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct TestHostMap : public IHostBlock
{
private:
    Ag::String _name;
    Ag::String _description;
    std::vector<uint32_t> _data;
public:
    TestHostMap(const char *name, const char *desc, uint32_t wordCount) :
        _name(name),
        _description(desc)
    {
        _data.resize(wordCount, 0);
    }

    virtual ~TestHostMap() = default;

    // Accessors
    const uint32_t *getBlock() const { return _data.data(); }
    uint32_t getWordCount() const { return static_cast<uint32_t>(_data.size()); }

    // Overrides
    RegionType getType() const override { return RegionType::HostBlock; }
    Ag::string_cref_t getName() const override { return _name; }
    Ag::string_cref_t getDescription() const override { return _description; }
    uint32_t getSize() const override { return static_cast<uint32_t>(_data.size() * 4); }
    void *getHostAddress() override { return _data.data(); }
};

struct TestMMIO : public IMMIOBlock
{
private:
    Ag::String _name;
    Ag::String _description;
    uint8_t _controlRegister;
    uint8_t _statusRegister;
    uint8_t _dataRegister;
public:
    TestMMIO(const char *name, const char *desc) :
        _name(name),
        _description(desc),
        _controlRegister(4),
        _statusRegister(2),
        _dataRegister(0)
    {
    }

    virtual ~TestMMIO() = default;

    // Accessors
    uint8_t getControlRegister() const { return _controlRegister; }
    void setControlRegister(uint8_t value) { _controlRegister = value; }
    uint8_t getStatusRegister() const { return _statusRegister; }
    void setStatusRegister(uint8_t value) { _statusRegister = value; }
    uint8_t getDataRegister() const { return _dataRegister; }
    void setDataRegister(uint8_t value) { _dataRegister = value; }

    // Overrides
    RegionType getType() const override { return RegionType::MMIO; }
    Ag::string_cref_t getName() const override { return _name; }
    Ag::string_cref_t getDescription() const override { return _description; }
    uint32_t getSize() const override { return 12; }

    uint32_t read(uint32_t offset) override
    {
        switch (offset >> 2)
        {
        case 0: return _controlRegister;
        case 1: return _statusRegister;
        default: return _dataRegister;
        }
    }

    void write(uint32_t offset, uint32_t value)
    {
        switch (offset >> 2)
        {
        case 0:
            _controlRegister = static_cast<uint8_t>(value);
            break;

        case 2:
            _dataRegister = static_cast<uint8_t>(value);
            break;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(AddressMap, DefaultConstruct)
{
    AddressMap specimen;
    IAddressRegionPtr region = nullptr;
    uint32_t offset = 0;
    uint32_t length = 0;

    EXPECT_FALSE(specimen.tryFindRegion(0x00, region, offset, length));
    EXPECT_FALSE(specimen.tryFindRegion(0x8000, region, offset, length));
    EXPECT_FALSE(specimen.tryFindRegion(0xFFFFFFFF, region, offset, length));
}

GTEST_TEST(AddressMap, ConstructNonOverlapping)
{
    AddressMap specimen;
    TestHostMap extraRam("Extra RAM", "Additional volatile memory.", 1024);
    TestHostMap extraRom("Extra ROM", "Additional built-in software.", 256);
    TestMMIO streamDevice("Stream Device", "A byte stream device.");

    // Casting changes the pointer, so create pointers which can be compared.
    IAddressRegionPtr extraRamPtr = &extraRam;
    IAddressRegionPtr extraRomPtr = &extraRom;
    IAddressRegionPtr streamDevicePtr = &streamDevice;

    EXPECT_TRUE(specimen.tryInsert(0x1000, extraRamPtr)); // 4KB
    EXPECT_TRUE(specimen.tryInsert(0x3000000, extraRomPtr)); // 48MB
    EXPECT_TRUE(specimen.tryInsert(0x0000, extraRomPtr)); // Also at 0.
    EXPECT_TRUE(specimen.tryInsert(0x2000000, streamDevicePtr)); // 32MB

    IAddressRegionPtr region = nullptr;
    uint32_t offset = 0;
    uint32_t length = 0;

    // Find the low ROM.
    EXPECT_TRUE(specimen.tryFindRegion(0x100, region, offset, length));
    EXPECT_EQ(region, extraRomPtr);
    EXPECT_EQ(offset, 0x100u);
    EXPECT_EQ(length, 0x400u - 0x100);

    // Look just after extra RAM.
    EXPECT_FALSE(specimen.tryFindRegion(0x2000, region, offset, length));

    EXPECT_TRUE(specimen.tryFindRegion(0x1800, region, offset, length));
    EXPECT_EQ(region, extraRamPtr);
    EXPECT_EQ(offset, 0x800u);
    EXPECT_EQ(length, 0x800u);

    // Look-up the registers of the Stream Device.
    EXPECT_TRUE(specimen.tryFindRegion(0x2000000, region, offset, length));
    EXPECT_EQ(region, streamDevicePtr);
    EXPECT_EQ(offset, 0u);
    EXPECT_EQ(length, 12u);

    EXPECT_TRUE(specimen.tryFindRegion(0x2000004, region, offset, length));
    EXPECT_EQ(region, streamDevicePtr);
    EXPECT_EQ(offset, 4u);
    EXPECT_EQ(length, 8u);

    EXPECT_TRUE(specimen.tryFindRegion(0x2000008, region, offset, length));
    EXPECT_EQ(region, streamDevicePtr);
    EXPECT_EQ(offset, 8u);
    EXPECT_EQ(length, 4u);

    // Check just after the streaming device
    EXPECT_FALSE(specimen.tryFindRegion(0x200000C, region, offset, length));

    // Check just before the high copy of Extra ROM.
    EXPECT_FALSE(specimen.tryFindRegion(0x2FFFFFC, region, offset, length));

    // Check the high copy of the ROM.
    EXPECT_TRUE(specimen.tryFindRegion(0x3000000, region, offset, length));
    EXPECT_EQ(region, extraRomPtr);
    EXPECT_EQ(offset, 0x0u);
    EXPECT_EQ(length, 0x400u);

    EXPECT_TRUE(specimen.tryFindRegion(0x30003FC, region, offset, length));
    EXPECT_EQ(region, extraRomPtr);
    EXPECT_EQ(offset, 0x3FCu);
    EXPECT_EQ(length, 0x4u);

    // Check after end.
    EXPECT_FALSE(specimen.tryFindRegion(0x3000400, region, offset, length));
}

GTEST_TEST(AddressMap, ConstructOverlapping)
{
    AddressMap specimen;
    TestHostMap extraRam("Extra RAM", "Additional volatile memory.", 1024);
    TestHostMap extraRom("Extra ROM", "Additional built-in software.", 256);
    TestMMIO streamDevice("Stream Device", "A byte stream device.");

    // Casting changes the pointer, so create pointers which can be compared.
    IAddressRegionPtr extraRamPtr = &extraRam;
    IAddressRegionPtr extraRomPtr = &extraRom;
    IAddressRegionPtr streamDevicePtr = &streamDevice;

    EXPECT_TRUE(specimen.tryInsert(0x1000, extraRamPtr));
    EXPECT_TRUE(specimen.tryInsert(0x3000000, extraRomPtr));

    IAddressRegionPtr region = nullptr;
    uint32_t offset = 0;
    uint32_t length = 0;

    // Just check that addresses below the lowest block return nothing.
    EXPECT_FALSE(specimen.tryFindRegion(0x00, region, offset, length));
    EXPECT_FALSE(specimen.tryFindRegion(0xFFC, region, offset, length));

    // Check addresses between blocks return nothing.
    EXPECT_FALSE(specimen.tryFindRegion(0x2000000, region, offset, length));

    // Check addresses above the highest block return nothing.
    EXPECT_FALSE(specimen.tryFindRegion(0x3000400, region, offset, length));

    // Try to add overlapping regions.
    EXPECT_FALSE(specimen.tryInsert(0xFF8, streamDevicePtr));
    EXPECT_FALSE(specimen.tryInsert(0xFFC, streamDevicePtr));
    EXPECT_FALSE(specimen.tryInsert(0x1FFC, streamDevicePtr));
    EXPECT_FALSE(specimen.tryInsert(0x2FFFFF8, streamDevicePtr));
    EXPECT_FALSE(specimen.tryInsert(0x30003FC, streamDevicePtr));
}

GTEST_TEST(AddressMap, TestBedHardware)
{
    AddressMap masterReadMap;
    AddressMap masterWriteMap;

    TestHostMap romBlock("ROM", "Main ROM", TestBedHardware::RomSize / 4);
    TestHostMap ramBlock("RAM", "Main RAM", TestBedHardware::RamSize / 4);

    masterReadMap.tryInsert(0, &romBlock);
    masterReadMap.tryInsert(TestBedHardware::HighRomBase, &romBlock);
    masterWriteMap.tryInsert(TestBedHardware::RamBase, &ramBlock);
    masterReadMap.tryInsert(TestBedHardware::RamBase, &ramBlock);

    IAddressRegionPtr region;
    uint32_t offset, length;

    EXPECT_TRUE(masterReadMap.tryFindRegion(0x0000, region, offset, length));
    EXPECT_EQ(region, static_cast<IAddressRegionPtr>(&romBlock));
    EXPECT_EQ(offset, 0u);
    EXPECT_EQ(length, TestBedHardware::RomSize);
}

GTEST_TEST(ArmSystemBuilder, CanCreateArmV2TestSystem)
{
    Options opts;
    opts.setHardwareArchitecture(SystemModel::TestBed);
    opts.setProcessorVariant(ProcessorModel::ARM2);
    opts.setSystemRom(SystemROMPreset::Custom);

    ArmSystemBuilder specimen(opts);

    IArmSystemUPtr emulatedSystem = specimen.createSystem();

    ASSERT_TRUE(emulatedSystem);
}

GTEST_TEST(ArmSystemBuilder, CanCreateArmV2aTestSystem)
{
    Options opts;
    opts.setHardwareArchitecture(SystemModel::TestBed);
    opts.setProcessorVariant(ProcessorModel::ARM3);
    opts.setSystemRom(SystemROMPreset::Custom);

    ArmSystemBuilder specimen(opts);

    IArmSystemUPtr emulatedSystem = specimen.createSystem();

    ASSERT_TRUE(emulatedSystem);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


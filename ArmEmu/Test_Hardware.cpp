//! @file Test_Hardware.cpp
//! @brief The definition of unit tests of the hardware layer of an emulated
//! ARM-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include <gtest/gtest.h>

#include "Ag/Core/Utils.hpp"

#include "TestTools.hpp"
#include "Hardware.inl"
#include "TestBedHardware.inl"

namespace Ag {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct GenerateSequentialBytes
{
private:
    uint8_t _seed;
public:
    GenerateSequentialBytes() : _seed(0) {}

    uint8_t operator()()
    {
        return _seed++;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void initialiseBuffer(HostBuffer &buffer)
{
    std::generate(buffer.begin(), buffer.end(), GenerateSequentialBytes());
}

template<typename T> T getBufferValue(const HostBuffer &buffer, uint32_t offset)
{
    return *reinterpret_cast<const T *>(buffer.data() + offset);
}
////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(BasicHardware, SetStates)
{
    BasicIrqManagerHardware specimen;

    // Verify no interrupts pending.
    EXPECT_EQ(specimen.getIrqStatus(), 0u);

    // Raise a guest Fast IRQ and verify.
    specimen.setGuestFastIrq(true);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::FastIrqPending);

    // Raise a guest IRQ and verify.
    specimen.setGuestIrq(true);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::FastIrqPending |
                                       IrqState::IrqPending);

    // Raise a host debug IRQ and verify.
    specimen.setDebugIrq(true);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::FastIrqPending |
                                       IrqState::IrqPending |
                                       IrqState::DebugPending);

    // Raise a host IRQ and verify.
    specimen.setHostIrq(true);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::FastIrqPending |
                                       IrqState::IrqPending |
                                       IrqState::DebugPending |
                                       IrqState::HostPending);

    // Mask guest fast IRQs and verify.
    specimen.updateIrqMask(IrqState::FastIrqPending, IrqState::FastIrqPending);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::IrqPending |
                                       IrqState::DebugPending |
                                       IrqState::HostPending);

    // Mask guest IRQs and verify.
    specimen.updateIrqMask(IrqState::IrqPending, IrqState::IrqPending);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::DebugPending |
                                       IrqState::HostPending);

    // Mask host debug IRQs, unmask guest IRQs and verify.
    specimen.updateIrqMask(IrqState::DebugPending | IrqState::IrqPending,
                           IrqState::DebugPending);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::HostPending | IrqState::IrqPending);

    // Handle guest IRQ and verify.
    specimen.setGuestIrq(false);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::HostPending);

    // Unmask all guest IRQs and verify.
    specimen.updateIrqMask(IrqState::GuestIrqsMask, 0u);
    EXPECT_EQ(specimen.getIrqStatus(), IrqState::HostPending |
                                       IrqState::FastIrqPending);
}

GTEST_TEST(BasicHardware, ReadBytes)
{
    TestBedHardware specimen;

    static_assert(AlignedAddr<uint8_t>::mask == 0xFFFFFFFF, "Alignment incorrect");
    static_assert(AlignedAddr<uint16_t>::mask == 0xFFFFFFFE, "Alignment incorrect");
    static_assert(AlignedAddr<uint32_t>::mask == 0xFFFFFFFC, "Alignment incorrect");
    static_assert(AlignedAddr<uint64_t>::mask == 0xFFFFFFF8, "Alignment incorrect");

    // Initialise RAM and ROM.
    initialiseBuffer(specimen.getRom());
    initialiseBuffer(specimen.getRam());

    uint8_t value8;
    uint16_t value16;
    uint32_t value32;
    uint64_t value64;

    // Read from ROM.
    ASSERT_TRUE(specimen.read(5, value8));
    EXPECT_TRUE(isEqualHex(value8, 5));

    ASSERT_TRUE(specimen.read(22, value16));
    EXPECT_TRUE(isEqualHex(value16, 0x1716));

    ASSERT_TRUE(specimen.read(32, value32));
    EXPECT_TRUE(isEqualHex(value32, 0x23222120));

    ASSERT_TRUE(specimen.read(40, value64));
    EXPECT_TRUE(isEqualHex(value64, 0x2F2E2D2C2B2A2928));

    // Read from RAM.
    ASSERT_TRUE(specimen.read(5 + 0x8000, value8));
    EXPECT_TRUE(isEqualHex(value8, 5));

    ASSERT_TRUE(specimen.read(22 + 0x8000, value16));
    EXPECT_TRUE(isEqualHex(value16, 0x1716));

    ASSERT_TRUE(specimen.read(32 + 0x8000, value32));
    EXPECT_TRUE(isEqualHex(value32, 0x23222120));

    ASSERT_TRUE(specimen.read(40 + 0x8000, value64));
    EXPECT_TRUE(isEqualHex(value64, 0x2F2E2D2C2B2A2928));

    // Read unaligned.
    ASSERT_TRUE(specimen.read(21, value16));
    EXPECT_TRUE(isEqualHex(value16, 0x1514));

    ASSERT_TRUE(specimen.read(25, value32));
    EXPECT_TRUE(isEqualHex(value32, 0x1B1A1918));

    ASSERT_TRUE(specimen.read(36, value64));
    EXPECT_TRUE(isEqualHex(value64, 0x2726252423222120));

    // Read from bad locations - the test system doesn't raise the ABORT signal,
    // it just returns 'random' data.
    EXPECT_FALSE(specimen.read(0x10000, value8));
    EXPECT_FALSE(specimen.read(0x10000, value16));
    EXPECT_FALSE(specimen.read(0x10000, value32));
    EXPECT_FALSE(specimen.read(0x10000, value64));
    EXPECT_FALSE(specimen.read(0x2FFFFFF, value8));
    EXPECT_FALSE(specimen.read(0x2FFFFFF, value16));
    EXPECT_FALSE(specimen.read(0x2FFFFFF, value32));
    EXPECT_FALSE(specimen.read(0x2FFFFFF, value64));
}

GTEST_TEST(BasicHardware, WriteBytes)
{
    TestBedHardware specimen;

    // Initialise RAM and ROM.
    initialiseBuffer(specimen.getRom());
    initialiseBuffer(specimen.getRam());

    const uint8_t value8 = 0x5A;
    const uint16_t value16 = 0xABBA;
    const uint32_t value32 = 0xDEADBEEF;
    const uint64_t value64 = 0xDEADCAFEBEEFBABE;

    // Write to ROM with silent failure.
    ASSERT_TRUE(specimen.write(5, value8));
    EXPECT_TRUE(isEqualHex(specimen.getRom().at(5), 5));

    ASSERT_TRUE(specimen.write(22, value16));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint16_t>(specimen.getRom(), 22), 0x1716));

    ASSERT_TRUE(specimen.write(32, value32));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint32_t>(specimen.getRom(), 32), 0x23222120));

    ASSERT_TRUE(specimen.write(40, value64));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint64_t>(specimen.getRom(), 40), 0x2F2E2D2C2B2A2928));

    // Write to RAM.
    ASSERT_TRUE(specimen.write(5 + 0x8000, value8));
    EXPECT_TRUE(isEqualHex(specimen.getRam().at(5), value8));

    ASSERT_TRUE(specimen.write(22 + 0x8000, value16));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint16_t>(specimen.getRam(), 22), value16));

    ASSERT_TRUE(specimen.write(32 + 0x8000, value32));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint32_t>(specimen.getRam(), 32), value32));

    ASSERT_TRUE(specimen.write(40 + 0x8000, value64));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint64_t>(specimen.getRam(), 40), value64));

    // Write unaligned.
    ASSERT_TRUE(specimen.write(21 + 0x8000, value16));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint16_t>(specimen.getRam(), 20), value16));

    ASSERT_TRUE(specimen.write(25 + 0x8000, value32));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint32_t>(specimen.getRam(), 24), value32));

    ASSERT_TRUE(specimen.write(36 + 0x8000, value64));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint64_t>(specimen.getRam(), 32), value64));

    // Write to bad locations.
    EXPECT_FALSE(specimen.write(0x10000, value8));
    EXPECT_FALSE(specimen.write(0x10000, value16));
    EXPECT_FALSE(specimen.write(0x10000, value32));
    EXPECT_FALSE(specimen.write(0x10000, value64));
    EXPECT_FALSE(specimen.write(0x3FFFFFF, value8));
    EXPECT_FALSE(specimen.write(0x3FFFFFF, value16));
    EXPECT_FALSE(specimen.write(0x3FFFFFF, value32));
    EXPECT_FALSE(specimen.write(0x3FFFFFF, value64));
}

GTEST_TEST(BasicHardware, ExchangeBytes)
{
    TestBedHardware specimen;

    // Initialise RAM and ROM.
    initialiseBuffer(specimen.getRom());
    initialiseBuffer(specimen.getRam());

    const uint8_t writeValue8 = 0x5A;
    const uint16_t writeValue16 = 0xABBA;
    const uint32_t writeValue32 = 0xDEADBEEF;
    const uint64_t writeValue64 = 0xDEADCAFEBEEFBABE;

    uint8_t readValue8;
    uint16_t readValue16;
    uint32_t readValue32;
    uint64_t readValue64;

    // Exchange to ROM with silent failure.
    ASSERT_TRUE(specimen.exchange(5, writeValue8, readValue8));
    EXPECT_TRUE(isEqualHex(specimen.getRom().at(5), 5));
    EXPECT_TRUE(isEqualHex(readValue8, 5));

    ASSERT_TRUE(specimen.exchange(22, writeValue16, readValue16));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint16_t>(specimen.getRom(), 22), 0x1716));
    EXPECT_TRUE(isEqualHex(readValue16, 0x1716));

    ASSERT_TRUE(specimen.exchange(32, writeValue32, readValue32));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint32_t>(specimen.getRom(), 32), 0x23222120));
    EXPECT_TRUE(isEqualHex(readValue32, 0x23222120));

    ASSERT_TRUE(specimen.exchange(40, writeValue64, readValue64));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint64_t>(specimen.getRom(), 40), 0x2F2E2D2C2B2A2928));
    EXPECT_TRUE(isEqualHex(readValue64, 0x2F2E2D2C2B2A2928));

    // Read then write to RAM.
    ASSERT_TRUE(specimen.exchange(5 + 0x8000, writeValue8, readValue8));
    EXPECT_TRUE(isEqualHex(specimen.getRam().at(5), writeValue8));
    EXPECT_TRUE(isEqualHex(readValue8, 5));

    ASSERT_TRUE(specimen.exchange(22 + 0x8000, writeValue16, readValue16));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint16_t>(specimen.getRam(), 22), writeValue16));
    EXPECT_TRUE(isEqualHex(readValue16, 0x1716));

    ASSERT_TRUE(specimen.exchange(32 + 0x8000, writeValue32, readValue32));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint32_t>(specimen.getRam(), 32), writeValue32));
    EXPECT_TRUE(isEqualHex(readValue32, 0x23222120));

    ASSERT_TRUE(specimen.exchange(40 + 0x8000, writeValue64, readValue64));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint64_t>(specimen.getRam(), 40), writeValue64));
    EXPECT_TRUE(isEqualHex(readValue64, 0x2F2E2D2C2B2A2928));

    // Read then write unaligned.
    ASSERT_TRUE(specimen.exchange(49 + 0x8000, writeValue16, readValue16));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint16_t>(specimen.getRam(), 48), writeValue16));
    EXPECT_TRUE(isEqualHex(readValue16, 0x3130));

    ASSERT_TRUE(specimen.exchange(54 + 0x8000, writeValue32, readValue32));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint32_t>(specimen.getRam(), 52), writeValue32));
    EXPECT_TRUE(isEqualHex(readValue32, 0x37363534));

    ASSERT_TRUE(specimen.exchange(59 + 0x8000, writeValue64, readValue64));
    EXPECT_TRUE(isEqualHex(getBufferValue<uint64_t>(specimen.getRam(), 56), writeValue64));
    EXPECT_TRUE(isEqualHex(readValue64, 0x3F3E3D3C3B3A3938));

    // Exchange with bad locations.
    EXPECT_FALSE(specimen.exchange(0x10000, writeValue8, readValue8));
    EXPECT_FALSE(specimen.exchange(0x10000, writeValue16, readValue16));
    EXPECT_FALSE(specimen.exchange(0x10000, writeValue32, readValue32));
    EXPECT_FALSE(specimen.exchange(0x10000, writeValue64, readValue64));
    EXPECT_FALSE(specimen.exchange(0x2FFFFFF, writeValue8, readValue8));
    EXPECT_FALSE(specimen.exchange(0x2FFFFFF, writeValue16, readValue16));
    EXPECT_FALSE(specimen.exchange(0x2FFFFFF, writeValue32, readValue32));
    EXPECT_FALSE(specimen.exchange(0x2FFFFFF, writeValue64, readValue64));
}

} // Anonymous namespace

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


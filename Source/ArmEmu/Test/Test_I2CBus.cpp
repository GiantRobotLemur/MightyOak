//! @file Test_I2CBus.cpp
//! @brief The definition of unit tests for the I2CBus class, verifying the
//! bit-banged I2C protocol state machine including START/STOP detection,
//! address matching, and read/write transaction dispatching.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <cstdint>

#include <gtest/gtest.h>

#include "I2CBus.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A mock I2C device that records calls to its interface methods and
//! allows configuring return values for testing bus dispatch logic.
class MockI2CDevice : public II2CDevice
{
public:
    struct WriteRecord
    {
        uint8_t data;
        bool isFirst;
    };

    MockI2CDevice(uint8_t address) :
        _address(address),
        _readValue(0xFF),
        _writeAck(true),
        _stopCount(0)
    {
    }

    // Configuration
    void setReadValue(uint8_t value) { _readValue = value; }
    void setWriteAck(bool ack) { _writeAck = ack; }

    // Query
    const std::vector<WriteRecord> &getWrites() const { return _writes; }
    int getReadCount() const { return static_cast<int>(_readValues.size()); }
    int getStopCount() const { return _stopCount; }

    void clearRecords()
    {
        _writes.clear();
        _readValues.clear();
        _stopCount = 0;
    }

    // II2CDevice overrides
    uint8_t getI2CAddress() const override { return _address; }

    bool onI2CWrite(uint8_t data, bool isFirst) override
    {
        _writes.push_back({data, isFirst});
        return _writeAck;
    }

    bool onI2CRead(uint8_t &data) override
    {
        data = _readValue;
        _readValues.push_back(_readValue);
        return true;
    }

    void onI2CStop() override
    {
        ++_stopCount;
    }

private:
    uint8_t _address;
    uint8_t _readValue;
    bool _writeAck;
    int _stopCount;
    std::vector<WriteRecord> _writes;
    std::vector<uint8_t> _readValues;
};

//! @brief A test fixture for the I2CBus that provides helper methods to drive
//! the bit-banged I2C protocol.
class I2CBusTest : public ::testing::Test
{
protected:
    I2CBus _bus;
    bool _sda;
    bool _scl;
    bool _sdaInput;

    I2CBusTest() :
        _sda(true),
        _scl(true),
        _sdaInput(true)
    {
    }

    //! @brief Updates bus state and captures sdaInput.
    void tick()
    {
        _bus.update(_sda, _scl, _sdaInput);
    }

    //! @brief Sends an I2C START condition (SDA falls while SCL is high).
    void sendStart()
    {
        _sda = true;
        _scl = true;
        tick();

        _sda = false; // SDA falls while SCL high
        tick();

        _scl = false; // Pull SCL low to prepare for data
        tick();
    }

    //! @brief Sends an I2C STOP condition (SDA rises while SCL is high).
    void sendStop()
    {
        _sda = false;
        _scl = false;
        tick();

        _scl = true; // SCL goes high
        tick();

        _sda = true; // SDA rises while SCL high = STOP
        tick();
    }

    //! @brief Clocks a single bit onto the bus (rising-edge clocked).
    void clockBit(bool bit)
    {
        _sda = bit;
        _scl = false;
        tick();

        _scl = true; // Rising edge: data sampled
        tick();

        _scl = false; // Falling edge: prepare for next
        tick();
    }

    //! @brief Sends a byte MSB-first, then reads the ACK bit.
    //! @returns true if the device acknowledged (SDA pulled low).
    bool sendByte(uint8_t byte)
    {
        for (int i = 7; i >= 0; --i)
        {
            clockBit((byte >> i) & 1);
        }

        // ACK phase: the bus needs a falling edge to assert ACK,
        // then a rising edge for master to sample it, then falling
        // to complete.
        _sda = true; // Release SDA so device can drive it
        _scl = false;
        tick(); // Falling edge after last data bit - device asserts ACK

        _scl = true;
        tick(); // Rising edge: master samples ACK
        bool ack = !_sdaInput; // ACK = SDA pulled low

        _scl = false;
        tick(); // Falling edge: device releases, transition to next state

        return ack;
    }

    //! @brief Reads a byte from the bus MSB-first, then sends ACK/NACK.
    //! @param[in] ack True to acknowledge (request more data).
    //! @returns The byte read from the device.
    uint8_t readByte(bool ack)
    {
        uint8_t byte = 0;

        for (int i = 7; i >= 0; --i)
        {
            _sda = true; // Release SDA so device can drive
            _scl = false;
            tick(); // Falling edge: device drives next bit

            _scl = true;
            tick(); // Rising edge: master samples bit

            byte = (byte << 1) | (_sdaInput ? 1 : 0);
        }

        // Falling edge to complete the byte (transitions to DataAckRead).
        _scl = false;
        tick();

        // ACK/NACK phase: set SDA while SCL is low to avoid false START
        // detection (SDA falling while SCL high).
        _sda = !ack; // ACK = SDA low, NACK = SDA high
        tick();

        _scl = true; // Rising edge: device samples ACK/NACK
        tick();

        _scl = false;
        tick();

        return byte;
    }

    //! @brief Sends a full address byte (7-bit address + R/W bit).
    //! @returns true if the device acknowledged.
    bool sendAddress(uint8_t address7bit, bool read)
    {
        uint8_t addrByte = (address7bit << 1) | (read ? 1 : 0);
        return sendByte(addrByte);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Idle State
////////////////////////////////////////////////////////////////////////////////
TEST_F(I2CBusTest, IdleState_SdaHighByDefault)
{
    // After construction, bus should not be pulling SDA low.
    tick();
    EXPECT_TRUE(_sdaInput);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — START/STOP Detection
////////////////////////////////////////////////////////////////////////////////
TEST_F(I2CBusTest, StartCondition_Detected)
{
    MockI2CDevice device(0x50);
    _bus.addDevice(&device);

    sendStart();
    bool ack = sendAddress(0x50, false);

    EXPECT_TRUE(ack);
}

TEST_F(I2CBusTest, StopCondition_Detected)
{
    MockI2CDevice device(0x50);
    _bus.addDevice(&device);

    sendStart();
    sendAddress(0x50, false);
    sendByte(0xAB);
    sendStop();

    EXPECT_EQ(device.getStopCount(), 1);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Write Transactions
////////////////////////////////////////////////////////////////////////////////
TEST_F(I2CBusTest, WriteTransaction_SingleByte)
{
    MockI2CDevice device(0x50);
    _bus.addDevice(&device);

    sendStart();
    ASSERT_TRUE(sendAddress(0x50, false));
    sendByte(0x42);
    sendStop();

    auto &writes = device.getWrites();
    ASSERT_EQ(writes.size(), 1u);
    EXPECT_EQ(writes[0].data, 0x42);
    EXPECT_TRUE(writes[0].isFirst);
}

TEST_F(I2CBusTest, WriteTransaction_MultipleBytes)
{
    MockI2CDevice device(0x50);
    _bus.addDevice(&device);

    sendStart();
    ASSERT_TRUE(sendAddress(0x50, false));
    sendByte(0x10);
    sendByte(0xAB);
    sendStop();

    auto &writes = device.getWrites();
    ASSERT_EQ(writes.size(), 2u);
    EXPECT_EQ(writes[0].data, 0x10);
    EXPECT_TRUE(writes[0].isFirst);
    EXPECT_EQ(writes[1].data, 0xAB);
    EXPECT_FALSE(writes[1].isFirst);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Read Transactions
////////////////////////////////////////////////////////////////////////////////
TEST_F(I2CBusTest, ReadTransaction_SingleByte)
{
    MockI2CDevice device(0x50);
    device.setReadValue(0x5A);
    _bus.addDevice(&device);

    sendStart();
    ASSERT_TRUE(sendAddress(0x50, true));
    uint8_t data = readByte(false); // NACK = last byte
    sendStop();

    EXPECT_EQ(data, 0x5A);
    EXPECT_EQ(device.getReadCount(), 1);
}

TEST_F(I2CBusTest, ReadTransaction_MultipleBytes)
{
    MockI2CDevice device(0x50);
    device.setReadValue(0x33);
    _bus.addDevice(&device);

    sendStart();
    ASSERT_TRUE(sendAddress(0x50, true));
    readByte(true);  // ACK = want more
    readByte(false); // NACK = done
    sendStop();

    EXPECT_EQ(device.getReadCount(), 2);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Address Handling
////////////////////////////////////////////////////////////////////////////////
TEST_F(I2CBusTest, UnknownAddress_Nack)
{
    // No devices registered. Address should not be acknowledged.
    sendStart();
    bool ack = sendAddress(0x50, false);

    EXPECT_FALSE(ack);
}

TEST_F(I2CBusTest, MultipleDevices_CorrectDispatch)
{
    MockI2CDevice device1(0x50);
    MockI2CDevice device2(0x68);
    _bus.addDevice(&device1);
    _bus.addDevice(&device2);

    // Write to device 1
    sendStart();
    ASSERT_TRUE(sendAddress(0x50, false));
    sendByte(0xAA);
    sendStop();

    // Write to device 2
    sendStart();
    ASSERT_TRUE(sendAddress(0x68, false));
    sendByte(0xBB);
    sendStop();

    ASSERT_EQ(device1.getWrites().size(), 1u);
    EXPECT_EQ(device1.getWrites()[0].data, 0xAA);

    ASSERT_EQ(device2.getWrites().size(), 1u);
    EXPECT_EQ(device2.getWrites()[0].data, 0xBB);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Repeated Start and Reset
////////////////////////////////////////////////////////////////////////////////
TEST_F(I2CBusTest, RepeatedStart_ResetsTransaction)
{
    MockI2CDevice device(0x50);
    _bus.addDevice(&device);

    sendStart();
    sendAddress(0x50, false);
    sendByte(0x10); // register pointer

    // Repeated START (without STOP)
    sendStart();
    ASSERT_TRUE(sendAddress(0x50, true)); // Now read mode
    readByte(false);
    sendStop();

    // The device should have received the write and a read.
    EXPECT_EQ(device.getWrites().size(), 1u);
    EXPECT_EQ(device.getReadCount(), 1);
}

TEST_F(I2CBusTest, Reset_ClearsState)
{
    MockI2CDevice device(0x50);
    _bus.addDevice(&device);

    // Start a transaction mid-way.
    sendStart();
    sendAddress(0x50, false);

    // Reset the bus.
    _bus.reset();

    // Bus should be idle. A new START should work normally.
    sendStart();
    ASSERT_TRUE(sendAddress(0x50, false));
    sendByte(0xFF);
    sendStop();

    // Only the write after reset should be recorded (plus any from before).
    EXPECT_GE(device.getWrites().size(), 1u);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Edge Cases
////////////////////////////////////////////////////////////////////////////////
TEST_F(I2CBusTest, AddDevice_NullIgnored)
{
    _bus.addDevice(nullptr);

    // Should not crash; bus should still function.
    sendStart();
    bool ack = sendAddress(0x50, false);
    EXPECT_FALSE(ack); // No real device registered
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

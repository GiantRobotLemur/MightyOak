//! @file Test_KeyboardController.cpp
//! @brief The definition of unit tests for the AcornKeyboardController class,
//! verifying KART protocol handshake, key/mouse event queuing, and host
//! command handling.
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

#include <gtest/gtest.h>

#include "AcornKeyboardController.hpp"
#include "MemcHardware.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A test fixture that creates a MemcHardware with its IOC, wires up
//! an AcornKeyboardController, and provides helpers for exercising the KART
//! protocol.
class KeyboardControllerTest : public ::testing::Test
{
protected:
    AddressMap _readMap, _writeMap;
    MemcHardware _memc;
    AcornKeyboardController _keyboard;

    KeyboardControllerTest() :
        _memc(Options(), _readMap, _writeMap)
    {
        _memc.reset();

        // Build a ConnectionContext that contains the IOC from the
        // MemcHardware so the keyboard controller can find it by name.
        // The IOC is an MMIO region; the ConnectionContext indexes MMIO
        // regions from the address maps passed to it.
        AddressMap ioReadMap, ioWriteMap;
        ioReadMap.tryInsert(0x3200000, &_memc.getIOC());
        ioWriteMap.tryInsert(0x3200000, &_memc.getIOC());

        HardwareDevicePool devices;
        ConnectionContext context(nullptr, devices, ioReadMap, ioWriteMap);
        _keyboard.connect(context);
    }

    //! @brief Drains all pending KART Rx bytes from the IOC queue.
    //! @returns A vector of bytes in the order they were enqueued.
    std::vector<uint8_t> drainRxQueue()
    {
        std::vector<uint8_t> bytes;
        uint8_t byte;

        while (_memc.getIOC().tryReadKartRxByte(byte))
        {
            bytes.push_back(byte);
        }

        return bytes;
    }

    //! @brief Runs the full HRST/RAK1/RAK2 handshake to reach the Initialised
    //! state, then drains the Rx queue so subsequent assertions start clean.
    void completeHandshake()
    {
        _keyboard.receiveKARTByte(AcornKeyboardController::HRST);
        drainRxQueue(); // discard RAK1

        _keyboard.receiveKARTByte(AcornKeyboardController::RAK1);
        drainRxQueue(); // discard RAK2

        _keyboard.receiveKARTByte(AcornKeyboardController::RAK2);
        drainRxQueue(); // discard initial status byte (NACK)
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Handshake Protocol
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, HandshakeStep1_HRST)
{
    // Send HRST, expect RAK1 response.
    _keyboard.receiveKARTByte(AcornKeyboardController::HRST);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::RAK1);
}

TEST_F(KeyboardControllerTest, HandshakeStep2_RAK1)
{
    _keyboard.receiveKARTByte(AcornKeyboardController::HRST);
    drainRxQueue(); // consume RAK1

    // Send RAK1, expect RAK2 response.
    _keyboard.receiveKARTByte(AcornKeyboardController::RAK1);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::RAK2);
}

TEST_F(KeyboardControllerTest, HandshakeStep3_RAK2_Completes)
{
    // Run the handshake up to receiving RAK2.
    _keyboard.receiveKARTByte(AcornKeyboardController::HRST);
    drainRxQueue();

    _keyboard.receiveKARTByte(AcornKeyboardController::RAK1);
    drainRxQueue();

    // Send RAK2, expect a status byte (NACK since nothing is pending).
    _keyboard.receiveKARTByte(AcornKeyboardController::RAK2);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::NACK);
}

TEST_F(KeyboardControllerTest, HandshakeInvalidByte_ResetsProtocol)
{
    // Start handshake.
    _keyboard.receiveKARTByte(AcornKeyboardController::HRST);
    drainRxQueue();

    // Send an unexpected byte instead of RAK1.
    _keyboard.receiveKARTByte(0x42);

    // Should get HRST (error → reset).
    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::HRST);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Post-Initialisation Commands
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, RQID_ReturnsKeyboardId)
{
    completeHandshake();

    _keyboard.receiveKARTByte(AcornKeyboardController::RQID);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);

    // Keyboard ID byte: KBID_Bits (0x80) | KeyboardId (1).
    EXPECT_EQ(response[0], AcornKeyboardController::KBID_Bits | 1);
}

TEST_F(KeyboardControllerTest, LEDS_AcknowledgesWithStatus)
{
    completeHandshake();

    // Send LEDS command (all LEDs off = 0x00).
    _keyboard.receiveKARTByte(0x00);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);

    // No pending data → NACK.
    EXPECT_EQ(response[0], AcornKeyboardController::NACK);
}

TEST_F(KeyboardControllerTest, PRST_ResetsProtocol)
{
    completeHandshake();

    _keyboard.receiveKARTByte(AcornKeyboardController::PRST);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::HRST);
}

TEST_F(KeyboardControllerTest, HRST_InInitialisedState_Resets)
{
    completeHandshake();

    // Sending HRST while initialised restarts the handshake.
    _keyboard.receiveKARTByte(AcornKeyboardController::HRST);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::RAK1);
}

TEST_F(KeyboardControllerTest, RQPD_ReturnsEmptyPDAT)
{
    completeHandshake();

    // Request data for row 3.
    _keyboard.receiveKARTByte(AcornKeyboardController::RQPD_Bits | 3);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::PDAT_Bits | 0);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Key Event Queuing
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, KeyDown_QueuedAndSentOnAck)
{
    completeHandshake();

    // Set up a scancode mapping: host scancode 0x1E → Acorn scancode 0x3C.
    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard.setKeyMapping(&mapping, 1);

    // Press a key.
    _keyboard.keyDown(0x1E);

    // Ask for status with an ack — should get key data.
    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);

    auto response = drainRxQueue();

    // Expect: KDDA|row, KDDA|col, then status (NACK since queue is now empty).
    ASSERT_EQ(response.size(), 3u);
    EXPECT_EQ(response[0], AcornKeyboardController::KDDA_Bits | 0x03); // row 3
    EXPECT_EQ(response[1], AcornKeyboardController::KDDA_Bits | 0x0C); // col 12
    EXPECT_EQ(response[2], AcornKeyboardController::NACK);             // no more data
}

TEST_F(KeyboardControllerTest, KeyUp_QueuedAndSentOnAck)
{
    completeHandshake();

    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard.setKeyMapping(&mapping, 1);

    _keyboard.keyUp(0x1E);

    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);

    auto response = drainRxQueue();
    ASSERT_EQ(response.size(), 3u);
    EXPECT_EQ(response[0], AcornKeyboardController::KUDA_Bits | 0x03); // row 3
    EXPECT_EQ(response[1], AcornKeyboardController::KUDA_Bits | 0x0C); // col 12
    EXPECT_EQ(response[2], AcornKeyboardController::NACK);
}

TEST_F(KeyboardControllerTest, MultipleKeys_SentOnePerAck)
{
    completeHandshake();

    IKeyboardController::ScanCodeMapping mappings[] = {
        { 0x10, 0x27 }, // host 0x10 → row 2, col 7
        { 0x11, 0x55 }, // host 0x11 → row 5, col 5
    };
    _keyboard.setKeyMapping(mappings, 2);

    _keyboard.keyDown(0x10);
    _keyboard.keyDown(0x11);

    // First ack: get first key event.
    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);
    auto response1 = drainRxQueue();

    ASSERT_EQ(response1.size(), 3u);
    EXPECT_EQ(response1[0], AcornKeyboardController::KDDA_Bits | 0x02); // row 2
    EXPECT_EQ(response1[1], AcornKeyboardController::KDDA_Bits | 0x07); // col 7
    // Status should indicate more data pending (SACK = key data available).
    EXPECT_EQ(response1[2], AcornKeyboardController::SACK);

    // Second ack: get second key event.
    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);
    auto response2 = drainRxQueue();

    ASSERT_EQ(response2.size(), 3u);
    EXPECT_EQ(response2[0], AcornKeyboardController::KDDA_Bits | 0x05);
    EXPECT_EQ(response2[1], AcornKeyboardController::KDDA_Bits | 0x05);
    EXPECT_EQ(response2[2], AcornKeyboardController::NACK); // no more data
}

TEST_F(KeyboardControllerTest, UnmappedKey_NotQueued)
{
    completeHandshake();

    // No key mappings set. keyDown should have no effect.
    _keyboard.keyDown(0xFF);

    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);
    auto response = drainRxQueue();

    // Only status byte, no key data.
    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::NACK);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Mouse Button Events
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, MouseButtonDown_CorrectScancodes)
{
    completeHandshake();

    _keyboard.mouseButtonDown(IKeyboardController::LeftButton);

    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);
    auto response = drainRxQueue();

    // Left button = scancode 0x70 → row 7, col 0.
    ASSERT_EQ(response.size(), 3u);
    EXPECT_EQ(response[0], AcornKeyboardController::KDDA_Bits | 0x07);
    EXPECT_EQ(response[1], AcornKeyboardController::KDDA_Bits | 0x00);
    EXPECT_EQ(response[2], AcornKeyboardController::NACK);
}

TEST_F(KeyboardControllerTest, MouseButtonUp_CorrectScancodes)
{
    completeHandshake();

    _keyboard.mouseButtonUp(IKeyboardController::RightButton);

    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);
    auto response = drainRxQueue();

    // Right button = scancode 0x72 → row 7, col 2.
    ASSERT_EQ(response.size(), 3u);
    EXPECT_EQ(response[0], AcornKeyboardController::KUDA_Bits | 0x07);
    EXPECT_EQ(response[1], AcornKeyboardController::KUDA_Bits | 0x02);
    EXPECT_EQ(response[2], AcornKeyboardController::NACK);
}

TEST_F(KeyboardControllerTest, MiddleMouseButton_CorrectScancode)
{
    completeHandshake();

    _keyboard.mouseButtonDown(IKeyboardController::MiddleButton);

    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);
    auto response = drainRxQueue();

    // Middle button = scancode 0x71 → row 7, col 1.
    ASSERT_EQ(response.size(), 3u);
    EXPECT_EQ(response[0], AcornKeyboardController::KDDA_Bits | 0x07);
    EXPECT_EQ(response[1], AcornKeyboardController::KDDA_Bits | 0x01);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Mouse Delta
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, MouseDelta_SentOnRQMP)
{
    completeHandshake();

    _keyboard.mouseDelta(10, -5);

    _keyboard.receiveKARTByte(AcornKeyboardController::RQMP);
    auto response = drainRxQueue();

    ASSERT_EQ(response.size(), 2u);

    // 10 as 7-bit: 0x0A
    EXPECT_EQ(response[0], 0x0Au);

    // -5 as 7-bit two's complement masked to 0x7F: (uint8_t)(-5) & 0x7F = 0x7B
    EXPECT_EQ(response[1], static_cast<uint8_t>(-5) & 0x7F);
}

TEST_F(KeyboardControllerTest, MouseDelta_AccumulatesMultipleCalls)
{
    completeHandshake();

    _keyboard.mouseDelta(3, 4);
    _keyboard.mouseDelta(7, -2);

    _keyboard.receiveKARTByte(AcornKeyboardController::RQMP);
    auto response = drainRxQueue();

    ASSERT_EQ(response.size(), 2u);
    EXPECT_EQ(response[0], 10u);                                // 3 + 7 = 10
    EXPECT_EQ(response[1], static_cast<uint8_t>(2) & 0x7Fu);    // 4 + (-2) = 2
}

TEST_F(KeyboardControllerTest, MouseDelta_ClampsToRange)
{
    completeHandshake();

    // Exceed the 7-bit signed range.
    _keyboard.mouseDelta(200, -200);

    _keyboard.receiveKARTByte(AcornKeyboardController::RQMP);
    auto response = drainRxQueue();

    ASSERT_EQ(response.size(), 2u);
    EXPECT_EQ(response[0], 63u);                                    // clamped to +63
    EXPECT_EQ(response[1], static_cast<uint8_t>(-64) & 0x7Fu);     // clamped to -64
}

TEST_F(KeyboardControllerTest, MouseDelta_ResetAfterRead)
{
    completeHandshake();

    _keyboard.mouseDelta(10, 20);

    // First RQMP: should get the deltas.
    _keyboard.receiveKARTByte(AcornKeyboardController::RQMP);
    drainRxQueue();

    // Second RQMP: deltas should be reset to 0.
    _keyboard.receiveKARTByte(AcornKeyboardController::RQMP);
    auto response = drainRxQueue();

    ASSERT_EQ(response.size(), 2u);
    EXPECT_EQ(response[0], 0u);
    EXPECT_EQ(response[1], 0u);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Status Byte Logic
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, StatusByte_NACKWhenIdle)
{
    completeHandshake();

    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);
    auto response = drainRxQueue();

    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::NACK);
}

TEST_F(KeyboardControllerTest, StatusByte_SACKWithKeyData)
{
    completeHandshake();

    IKeyboardController::ScanCodeMapping mapping(0x01, 0x10);
    _keyboard.setKeyMapping(&mapping, 1);
    _keyboard.keyDown(0x01);

    // Send LEDS, which responds with status.
    _keyboard.receiveKARTByte(0x00); // LEDS all off
    auto response = drainRxQueue();

    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::SACK);
}

TEST_F(KeyboardControllerTest, StatusByte_MACKWithMouseData)
{
    completeHandshake();

    _keyboard.mouseDelta(5, 0);

    _keyboard.receiveKARTByte(0x00); // LEDS → status
    auto response = drainRxQueue();

    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::MACK);
}

TEST_F(KeyboardControllerTest, StatusByte_SMAKWithBothPending)
{
    completeHandshake();

    IKeyboardController::ScanCodeMapping mapping(0x01, 0x10);
    _keyboard.setKeyMapping(&mapping, 1);
    _keyboard.keyDown(0x01);
    _keyboard.mouseDelta(1, 1);

    _keyboard.receiveKARTByte(0x00); // LEDS → status
    auto response = drainRxQueue();

    ASSERT_EQ(response.size(), 1u);
    EXPECT_EQ(response[0], AcornKeyboardController::SMAK);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Full Protocol Sequence
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, FullBootSequence)
{
    // Simulate the RISC OS keyboard init sequence.

    // 1. Handshake.
    _keyboard.receiveKARTByte(AcornKeyboardController::HRST);
    auto r1 = drainRxQueue();
    ASSERT_EQ(r1.size(), 1u);
    EXPECT_EQ(r1[0], AcornKeyboardController::RAK1);

    _keyboard.receiveKARTByte(AcornKeyboardController::RAK1);
    auto r2 = drainRxQueue();
    ASSERT_EQ(r2.size(), 1u);
    EXPECT_EQ(r2[0], AcornKeyboardController::RAK2);

    _keyboard.receiveKARTByte(AcornKeyboardController::RAK2);
    auto r3 = drainRxQueue();
    ASSERT_EQ(r3.size(), 1u);
    EXPECT_EQ(r3[0], AcornKeyboardController::NACK);

    // 2. Request keyboard ID.
    _keyboard.receiveKARTByte(AcornKeyboardController::RQID);
    auto r4 = drainRxQueue();
    ASSERT_EQ(r4.size(), 1u);
    EXPECT_EQ(r4[0], AcornKeyboardController::KBID_Bits | 1);

    // 3. Set LEDs.
    _keyboard.receiveKARTByte(0x00); // All LEDs off.
    auto r5 = drainRxQueue();
    ASSERT_EQ(r5.size(), 1u);
    EXPECT_EQ(r5[0], AcornKeyboardController::NACK);

    // 4. Poll for key data (idle).
    _keyboard.receiveKARTByte(AcornKeyboardController::SACK);
    auto r6 = drainRxQueue();
    ASSERT_EQ(r6.size(), 1u);
    EXPECT_EQ(r6[0], AcornKeyboardController::NACK);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

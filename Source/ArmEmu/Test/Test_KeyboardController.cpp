//! @file ArmEmu/Test/Test_KeyboardController.cpp
//! @brief The definition of unit tests for the AcornKeyboardController class,
//! verifying KART protocol handshake, key/mouse data exchanges, and host
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
#include "MEMC.inl"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A test fixture that creates a MemcHardware with its IOC, wires up
//! an AcornKeyboardController, and provides helpers for exercising the KART
//! protocol.
//! @details Data exchanges follow the Archimedes keyboard protocol: the
//! keyboard sends the first byte of a two-byte packet unprompted (or in
//! response to RQMP), the host replies with BACK to request the second byte,
//! then the host completes the exchange with one of NACK, SACK, MACK or SMAK,
//! which also selects whether key scanning and unsolicited mouse reporting
//! are enabled.
class KeyboardControllerTest : public ::testing::Test
{
protected:
    using MemcType = MemcHardware<false>;
    using IOCType = MemcType::IOCType;
    Options _defaultOptions;
    SystemContext _context;
    AddressMap _readMap, _writeMap;
    MemcType _memc;
    IOCType *_ioc;
    AcornKeyboardController *_keyboard;

    KeyboardControllerTest() :
        _context(_defaultOptions, nullptr, nullptr),
        _memc(_defaultOptions, _readMap, _writeMap),
        _ioc(nullptr),
        _keyboard(nullptr)
    {
        // Allow the keyboard and IOC devices to connect to each other.
        IHardwareDeviceCollection allDevices;
        allDevices.reserve(16);
        _memc.addIntegralHardware(allDevices);
        _context.connectAllDevices(allDevices);

        // Extract the keyboard device instance from the MEMC Hardware
        // implementation.
        _context.tryFindTypedDevice("Keyboard Controller", _keyboard);
        _context.tryFindTypedDevice("IOC", _ioc);

        // Reset the hardware state.
        _memc.reset();
    }

    //! @brief Drains all pending KART Rx bytes from the IOC queue.
    //! @returns A vector of bytes in the order they were enqueued.
    std::vector<uint8_t> drainRxQueue()
    {
        std::vector<uint8_t> bytes;
        uint8_t byte;

        while (_ioc->tryReadKartRxByte(byte))
        {
            bytes.push_back(byte);
        }

        return bytes;
    }

    //! @brief Sends a byte from the host to the keyboard controller.
    //! @param[in] hostByte The byte to send.
    //! @returns The bytes the keyboard controller sent in response.
    std::vector<uint8_t> sendByte(uint8_t hostByte)
    {
        _keyboard->receiveKARTByte(hostByte);

        return drainRxQueue();
    }

    //! @brief Runs the full HRST/RAK1/RAK2 handshake to reach the Initialised
    //! state, then drains the Rx queue so subsequent assertions start clean.
    void completeHandshake()
    {
        sendByte(AcornKeyboardController::HRST);
        sendByte(AcornKeyboardController::RAK1);
        sendByte(AcornKeyboardController::RAK2);
    }

    //! @brief Completes the handshake, then sends an acknowledge byte to
    //! enable key scanning and/or unsolicited mouse reporting.
    //! @param[in] ackByte One of NACK, SACK, MACK or SMAK.
    void initialise(uint8_t ackByte)
    {
        completeHandshake();

        // Acknowledge bytes never elicit a response.
        EXPECT_TRUE(sendByte(ackByte).empty());
    }

    //! @brief Verifies the keyboard sent a single byte.
    //! @param[in] response The bytes received from the keyboard.
    //! @param[in] expected The byte expected.
    static void expectSingleByte(const std::vector<uint8_t> &response,
                                 uint8_t expected)
    {
        ASSERT_EQ(response.size(), 1u);
        EXPECT_EQ(response[0], expected);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Handshake Protocol
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, HandshakeStep1_HRST)
{
    // Send HRST, expect HRST echoed.
    expectSingleByte(sendByte(AcornKeyboardController::HRST),
                     AcornKeyboardController::HRST);
}

TEST_F(KeyboardControllerTest, HandshakeStep2_RAK1)
{
    sendByte(AcornKeyboardController::HRST);

    // Send RAK1, expect RAK1 echoed.
    expectSingleByte(sendByte(AcornKeyboardController::RAK1),
                     AcornKeyboardController::RAK1);
}

TEST_F(KeyboardControllerTest, HandshakeStep3_RAK2_Completes)
{
    sendByte(AcornKeyboardController::HRST);
    sendByte(AcornKeyboardController::RAK1);

    // Send RAK2, expect RAK2 echoed to complete the handshake.
    expectSingleByte(sendByte(AcornKeyboardController::RAK2),
                     AcornKeyboardController::RAK2);
}

TEST_F(KeyboardControllerTest, HandshakeInvalidByte_ResetsProtocol)
{
    sendByte(AcornKeyboardController::HRST);

    // Send an unexpected byte instead of RAK1, expect HRST (error → reset).
    expectSingleByte(sendByte(0x42), AcornKeyboardController::HRST);
}

TEST_F(KeyboardControllerTest, CommandBeforeHandshake_ResetsProtocol)
{
    expectSingleByte(sendByte(AcornKeyboardController::RQID),
                     AcornKeyboardController::HRST);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Post-Initialisation Commands
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, RQID_ReturnsKeyboardId)
{
    completeHandshake();

    // Keyboard ID byte: KBID_Bits (0x80) | KeyboardId (1).
    expectSingleByte(sendByte(AcornKeyboardController::RQID),
                     AcornKeyboardController::KBID_Bits | 1);
}

TEST_F(KeyboardControllerTest, LEDS_NoResponse)
{
    completeHandshake();

    // Send LEDS command (all LEDs off = 0x00).
    // The host does not wait for a response; none should be sent.
    EXPECT_TRUE(sendByte(0x00).empty());
}

TEST_F(KeyboardControllerTest, LEDS_UpdatesLEDStates)
{
    completeHandshake();

    EXPECT_TRUE(sendByte(0x05).empty());
    EXPECT_EQ(_keyboard->getLEDStates(), 0x05);
}

TEST_F(KeyboardControllerTest, AcknowledgeBytes_NoResponse)
{
    completeHandshake();

    EXPECT_TRUE(sendByte(AcornKeyboardController::NACK).empty());
    EXPECT_TRUE(sendByte(AcornKeyboardController::SACK).empty());
    EXPECT_TRUE(sendByte(AcornKeyboardController::MACK).empty());
    EXPECT_TRUE(sendByte(AcornKeyboardController::SMAK).empty());
}

TEST_F(KeyboardControllerTest, PRST_ResetsProtocol)
{
    completeHandshake();

    expectSingleByte(sendByte(AcornKeyboardController::PRST),
                     AcornKeyboardController::HRST);
}

TEST_F(KeyboardControllerTest, HRST_InInitialisedState_Resets)
{
    completeHandshake();

    // Sending HRST while initialised restarts the handshake.
    // HRST is always echoed regardless of state (Acorn keyboard protocol).
    expectSingleByte(sendByte(AcornKeyboardController::HRST),
                     AcornKeyboardController::HRST);
}

TEST_F(KeyboardControllerTest, RQPD_ReturnsEmptyPDAT)
{
    completeHandshake();

    // Request data for row 3.
    expectSingleByte(sendByte(AcornKeyboardController::RQPD_Bits | 3),
                     AcornKeyboardController::PDAT_Bits | 0);
}

TEST_F(KeyboardControllerTest, BACK_WithNoExchangeInProgress_ResetsProtocol)
{
    initialise(AcornKeyboardController::SMAK);

    // BACK is only valid after the first byte of a packet has been sent.
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::HRST);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Key Events
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, KeyDown_SendsRowThenColumnOnBACK)
{
    initialise(AcornKeyboardController::SACK);

    // Set up a scancode mapping: host scancode 0x1E → Acorn scancode 0x3C.
    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard->setKeyMapping(&mapping, 1);

    // The row byte is sent as soon as the key is pressed.
    _keyboard->keyDown(0x1E);
    expectSingleByte(drainRxQueue(), AcornKeyboardController::KDDA_Bits | 0x03);

    // BACK requests the column byte.
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::KDDA_Bits | 0x0C);

    // The host completes the exchange, with no response from the keyboard.
    EXPECT_TRUE(sendByte(AcornKeyboardController::SACK).empty());
}

TEST_F(KeyboardControllerTest, KeyUp_SendsRowThenColumnOnBACK)
{
    initialise(AcornKeyboardController::SACK);

    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard->setKeyMapping(&mapping, 1);

    _keyboard->keyUp(0x1E);
    expectSingleByte(drainRxQueue(), AcornKeyboardController::KUDA_Bits | 0x03);

    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::KUDA_Bits | 0x0C);

    EXPECT_TRUE(sendByte(AcornKeyboardController::SACK).empty());
}

TEST_F(KeyboardControllerTest, MultipleKeys_SentInSeparateExchanges)
{
    initialise(AcornKeyboardController::SACK);

    IKeyboardController::ScanCodeMapping mappings[] = {
        { 0x10, 0x27 }, // host 0x10 → row 2, col 7
        { 0x11, 0x55 }, // host 0x11 → row 5, col 5
    };
    _keyboard->setKeyMapping(mappings, 2);

    // First exchange.
    _keyboard->keyDown(0x10);
    expectSingleByte(drainRxQueue(), AcornKeyboardController::KDDA_Bits | 0x02);
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::KDDA_Bits | 0x07);
    EXPECT_TRUE(sendByte(AcornKeyboardController::SACK).empty());

    // Second exchange.
    _keyboard->keyDown(0x11);
    expectSingleByte(drainRxQueue(), AcornKeyboardController::KDDA_Bits | 0x05);
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::KDDA_Bits | 0x05);
    EXPECT_TRUE(sendByte(AcornKeyboardController::SACK).empty());
}

TEST_F(KeyboardControllerTest, UnmappedKey_NotSent)
{
    initialise(AcornKeyboardController::SACK);

    // No key mappings set. keyDown should have no effect.
    _keyboard->keyDown(0xFF);

    EXPECT_TRUE(drainRxQueue().empty());
}

TEST_F(KeyboardControllerTest, KeyDown_IgnoredBeforeScanningEnabled)
{
    completeHandshake();

    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard->setKeyMapping(&mapping, 1);

    // Key scanning is not enabled until the host sends SACK or SMAK.
    _keyboard->keyDown(0x1E);

    EXPECT_TRUE(drainRxQueue().empty());
}

TEST_F(KeyboardControllerTest, KeyDown_IgnoredWhenOnlyMouseEnabled)
{
    initialise(AcornKeyboardController::MACK);

    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard->setKeyMapping(&mapping, 1);

    _keyboard->keyDown(0x1E);

    EXPECT_TRUE(drainRxQueue().empty());
}

TEST_F(KeyboardControllerTest, NACK_AfterExchange_DisablesScanning)
{
    initialise(AcornKeyboardController::SACK);

    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard->setKeyMapping(&mapping, 1);

    _keyboard->keyDown(0x1E);
    drainRxQueue();
    sendByte(AcornKeyboardController::BACK);

    // Complete the exchange with NACK, disabling key scanning.
    EXPECT_TRUE(sendByte(AcornKeyboardController::NACK).empty());

    _keyboard->keyUp(0x1E);
    EXPECT_TRUE(drainRxQueue().empty());
}

TEST_F(KeyboardControllerTest, NonBACK_AfterFirstByte_ResetsProtocol)
{
    initialise(AcornKeyboardController::SACK);

    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard->setKeyMapping(&mapping, 1);

    _keyboard->keyDown(0x1E);
    drainRxQueue();

    expectSingleByte(sendByte(AcornKeyboardController::RQID),
                     AcornKeyboardController::HRST);
}

TEST_F(KeyboardControllerTest, NonAcknowledge_AfterSecondByte_ResetsProtocol)
{
    initialise(AcornKeyboardController::SACK);

    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard->setKeyMapping(&mapping, 1);

    _keyboard->keyDown(0x1E);
    drainRxQueue();
    sendByte(AcornKeyboardController::BACK);

    expectSingleByte(sendByte(AcornKeyboardController::RQID),
                     AcornKeyboardController::HRST);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Mouse Button Events
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, MouseButtonDown_CorrectScancodes)
{
    initialise(AcornKeyboardController::SACK);

    // Left button = scancode 0x70 → row 7, col 0.
    _keyboard->mouseButtonDown(IKeyboardController::LeftButton);
    expectSingleByte(drainRxQueue(), AcornKeyboardController::KDDA_Bits | 0x07);
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::KDDA_Bits | 0x00);
    EXPECT_TRUE(sendByte(AcornKeyboardController::SACK).empty());
}

TEST_F(KeyboardControllerTest, MouseButtonUp_CorrectScancodes)
{
    initialise(AcornKeyboardController::SACK);

    // Right button = scancode 0x72 → row 7, col 2.
    _keyboard->mouseButtonUp(IKeyboardController::RightButton);
    expectSingleByte(drainRxQueue(), AcornKeyboardController::KUDA_Bits | 0x07);
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::KUDA_Bits | 0x02);
    EXPECT_TRUE(sendByte(AcornKeyboardController::SACK).empty());
}

TEST_F(KeyboardControllerTest, MiddleMouseButton_CorrectScancode)
{
    initialise(AcornKeyboardController::SACK);

    // Middle button = scancode 0x71 → row 7, col 1.
    _keyboard->mouseButtonDown(IKeyboardController::MiddleButton);
    expectSingleByte(drainRxQueue(), AcornKeyboardController::KDDA_Bits | 0x07);
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::KDDA_Bits | 0x01);
}

TEST_F(KeyboardControllerTest, MouseButton_IgnoredBeforeScanningEnabled)
{
    completeHandshake();

    _keyboard->mouseButtonDown(IKeyboardController::LeftButton);

    EXPECT_TRUE(drainRxQueue().empty());
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Mouse Delta
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, MouseDelta_SentOnRQMP)
{
    completeHandshake();

    _keyboard->mouseDelta(10, -5);

    // RQMP elicits the X delta, BACK the Y delta.
    expectSingleByte(sendByte(AcornKeyboardController::RQMP), 0x0A);

    // -5 as 7-bit two's complement masked to 0x7F: (uint8_t)(-5) & 0x7F = 0x7B
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     static_cast<uint8_t>(-5) & 0x7F);

    EXPECT_TRUE(sendByte(AcornKeyboardController::NACK).empty());
}

TEST_F(KeyboardControllerTest, MouseDelta_AccumulatesMultipleCalls)
{
    completeHandshake();

    _keyboard->mouseDelta(3, 4);
    _keyboard->mouseDelta(7, -2);

    expectSingleByte(sendByte(AcornKeyboardController::RQMP), 10);  // 3 + 7
    expectSingleByte(sendByte(AcornKeyboardController::BACK), 2);   // 4 + (-2)
}

TEST_F(KeyboardControllerTest, MouseDelta_ClampsToRange)
{
    completeHandshake();

    // Exceed the 7-bit signed range.
    _keyboard->mouseDelta(200, -200);

    expectSingleByte(sendByte(AcornKeyboardController::RQMP), 63);  // clamped to +63
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     static_cast<uint8_t>(-64) & 0x7F);             // clamped to -64
}

TEST_F(KeyboardControllerTest, MouseDelta_ResetAfterRead)
{
    completeHandshake();

    _keyboard->mouseDelta(10, 20);

    // First RQMP exchange: should get the deltas.
    sendByte(AcornKeyboardController::RQMP);
    sendByte(AcornKeyboardController::BACK);
    sendByte(AcornKeyboardController::NACK);

    // Second RQMP exchange: deltas should be reset to 0.
    expectSingleByte(sendByte(AcornKeyboardController::RQMP), 0);
    expectSingleByte(sendByte(AcornKeyboardController::BACK), 0);
}

TEST_F(KeyboardControllerTest, MouseDelta_SentUnpromptedWhenMouseEnabled)
{
    initialise(AcornKeyboardController::MACK);

    // With mouse reporting enabled, movement starts an exchange immediately.
    _keyboard->mouseDelta(5, -3);
    expectSingleByte(drainRxQueue(), 5);
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     static_cast<uint8_t>(-3) & 0x7F);
    EXPECT_TRUE(sendByte(AcornKeyboardController::MACK).empty());
}

TEST_F(KeyboardControllerTest, MouseDelta_HeldUntilRQMPWhenMouseDisabled)
{
    initialise(AcornKeyboardController::SACK);

    // With mouse reporting disabled, movement is accumulated silently.
    _keyboard->mouseDelta(5, 6);
    EXPECT_TRUE(drainRxQueue().empty());

    expectSingleByte(sendByte(AcornKeyboardController::RQMP), 5);
    expectSingleByte(sendByte(AcornKeyboardController::BACK), 6);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests — Full Protocol Sequence
////////////////////////////////////////////////////////////////////////////////
TEST_F(KeyboardControllerTest, FullBootSequence)
{
    // Simulate the RISC OS keyboard init sequence.
    IKeyboardController::ScanCodeMapping mapping(0x1E, 0x3C);
    _keyboard->setKeyMapping(&mapping, 1);

    // 1. Handshake.
    expectSingleByte(sendByte(AcornKeyboardController::HRST),
                     AcornKeyboardController::HRST);
    expectSingleByte(sendByte(AcornKeyboardController::RAK1),
                     AcornKeyboardController::RAK1);
    expectSingleByte(sendByte(AcornKeyboardController::RAK2),
                     AcornKeyboardController::RAK2);

    // 2. Request keyboard ID.
    expectSingleByte(sendByte(AcornKeyboardController::RQID),
                     AcornKeyboardController::KBID_Bits | 1);

    // 3. Set LEDs — no response expected.
    EXPECT_TRUE(sendByte(0x00).empty()); // All LEDs off.

    // 4. Enable keyboard and mouse (SMAK) — no response expected.
    EXPECT_TRUE(sendByte(AcornKeyboardController::SMAK).empty());

    // 5. A key press is reported via a full two-byte exchange.
    _keyboard->keyDown(0x1E);
    expectSingleByte(drainRxQueue(), AcornKeyboardController::KDDA_Bits | 0x03);
    expectSingleByte(sendByte(AcornKeyboardController::BACK),
                     AcornKeyboardController::KDDA_Bits | 0x0C);
    EXPECT_TRUE(sendByte(AcornKeyboardController::SMAK).empty());
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/AcornAcornKeyboardController.cpp
//! @brief The definition of an object which represents the 87C51 keyboard
//! controller, translating between key press and mouse movement and the
//! internal KART protocol.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cstdint>

#include "ArmEmu/HostMessageID.hpp"

#include "AcornKeyboardController.hpp"
#include "IOC.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
bool tryMapMouseButton(AcornKeyboardController::MouseButton button,
                       uint32_t &guestScanCode)
{
    bool hasScanCode = true;

    switch (button)
    {
    case AcornKeyboardController::LeftButton:
        guestScanCode = 0x70; // Row 7, Col 0
        break;

    case AcornKeyboardController::MiddleButton:
        guestScanCode = 0x71; // Row 7, Col 1
        break;

    case AcornKeyboardController::RightButton:
        guestScanCode = 0x72; // Row 7, Col 2
        break;

    default:
        hasScanCode = false;
        guestScanCode = 0;
    }

    return hasScanCode;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// AcornKeyboardController Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing a native Acorn keyboard/mouse which
//! actually interfaces with the host system.
AcornKeyboardController::AcornKeyboardController() :
    _name("Keyboard Controller"),
    _description("Maps host key and mouse events to guest-compatible scan codes."),
    _txQueue(nullptr),
    _mouseDeltaX(0),
    _mouseDeltaY(0),
    _context(nullptr),
    _ledStates(LED_None),
    _state(ControllerState::PreReset),
    _keyScanningEnabled(false),
    _mouseTxEnabled(false),
    _secondTxByte(0)
{
}

//! @brief Processes a byte sent from IOC via the KART interface.
//! @param[in] nextByte The byte sent to the keyboard controller.
void AcornKeyboardController::receiveKARTByte(uint8_t nextByte)
{
    // We can't do anything if we can't send bytes back to IOC.
    if (_txQueue == nullptr)
        return;

    bool hasError = true;

    // HRST is always accepted regardless of state and resets the protocol.
    if (nextByte == HRST)
    {
        _state = ControllerState::ReceivedHRST;
        _txQueue->enqueue(HRST);
        return;
    }

    switch (_state)
    {
    case ControllerState::PreReset:
    default:
        // Only HRST is accepted in PreReset (handled above).
        break;

    case ControllerState::ReceivedHRST:
        if (nextByte == RAK1)
        {
            _state = ControllerState::ReceivedRAK1;
            _txQueue->enqueue(RAK1);
            hasError = false;
        }
        break;

    case ControllerState::ReceivedRAK1:
        if (nextByte == RAK2)
        {
            // Handshake complete. Transition to Initialised state.
            _state = ControllerState::Initialised;
            _txQueue->enqueue(RAK2);
            hasError = false;
        }
        break;

    case ControllerState::Initialised:
        hasError = false;

        if (nextByte == PRST)
        {
            // Protocol reset - restart the handshake.
            _state = ControllerState::PreReset;
            _txQueue->enqueue(HRST);
        }
        else if (nextByte == RQID)
        {
            // Host requested the keyboard ID.
            _txQueue->enqueue(KBID_Bits | KeyboardId);
        }
        else if (nextByte == RQMP)
        {
            // Host requested mouse position data.
            beginSendMouseData();
        }
        else if (nextByte <= LEDS_Mask)
        {
            // LED state control (0x00-0x07). No response needed; the
            // host does not wait for one.
            LEDStateBits newState = nextByte & LED_All;

            if (newState != _ledStates)
            {
                // The LED state has changed. Inform the host.
                _ledStates = newState;

                if (_context != nullptr)
                    _context->postMessageToHost(KeyboardLEDsChanged, newState, 0);
            }
        }
        else if ((nextByte & ~RQPD_Mask) == RQPD_Bits)
        {
            // Host requested key data for a specific row. Send an empty
            // response as row-level polling is not yet fully supported.
            _txQueue->enqueue(PDAT_Bits | 0);
        }
        else
        {
            // Unrecognised command.
            hasError = (handleAcknowledge(nextByte) == false);
        }
        break;

    case ControllerState::AfterFirstByteSent:
        if (nextByte == BACK)
        {
            // Host requests the second byte of the current key/mouse
            // exchange, or sends pending key/mouse data.
            _state = ControllerState::AfterSecondByteSent;

            if (_txQueue != nullptr)
                _txQueue->enqueue(_secondTxByte);

            hasError = false;
        }
        break;

    case ControllerState::AfterSecondByteSent:
        // Host enable/acknowledge bytes. These configure which data
        // the keyboard should report (keys, mouse, both, or neither)
        // but do not elicit a response. The host does not wait for
        // one; sending a response would de-synchronise the protocol.
        if (handleAcknowledge(nextByte))
        {
            _state = ControllerState::Initialised;

            hasError = false;
        }
        break;
    }

    if (hasError)
    {
        // Re-send the hard reset signal and reset the protocol.
        _state = ControllerState::PreReset;
        _txQueue->enqueue(HRST);
    }
}

// Inherited from IHardwareDevice.
Ag::string_cref_t AcornKeyboardController::getName() const
{
    return _name;
}

// Inherited from IHardwareDevice.
Ag::string_cref_t AcornKeyboardController::getDescription() const
{
    return _description;
}

void AcornKeyboardController::connect(SystemContext &context)
{
    // Capture the system context so that input events can be
    // scheduled.
    _context = &context;
}

// Inherited from IKeyboardController.
IKeyboardController::LEDStateBits AcornKeyboardController::getLEDStates() const
{
    return _ledStates;
}

// Inherited from IKeyboardController.
void AcornKeyboardController::connectToTxQueue(SynchronisedByteQueue *txQueue)
{
    _txQueue = txQueue;
}

// Inherited from IKeyboardController.
void AcornKeyboardController::keyDown(uint32_t hostScanCode)
{
    uint32_t guestScanCode;

    if (_keyScanningEnabled && _scanCodeMap.tryFind(hostScanCode, guestScanCode))
    {
        beginSendKeyEvent(static_cast<uint8_t>(guestScanCode), true);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::keyUp(uint32_t hostScanCode)
{
    uint32_t guestScanCode;

    if (_keyScanningEnabled && _scanCodeMap.tryFind(hostScanCode, guestScanCode))
    {
        beginSendKeyEvent(static_cast<uint8_t>(guestScanCode), false);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseButtonDown(AcornKeyboardController::MouseButton button)
{
    uint32_t guestScanCode;

    if (_keyScanningEnabled && tryMapMouseButton(button, guestScanCode))
    {
        beginSendKeyEvent(static_cast<uint8_t>(guestScanCode), true);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseButtonUp(AcornKeyboardController::MouseButton button)
{
    uint32_t guestScanCode;

    if (_keyScanningEnabled && tryMapMouseButton(button, guestScanCode))
    {
        beginSendKeyEvent(static_cast<uint8_t>(guestScanCode), false);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseDelta(int32_t deltaX, int32_t deltaY)
{
    _mouseDeltaX.fetch_add(deltaX, std::memory_order_relaxed);
    _mouseDeltaY.fetch_add(deltaY, std::memory_order_relaxed);

    if (_mouseTxEnabled)
        beginSendMouseData();
}

// Inherited from IKeyboardController.
void AcornKeyboardController::setKeyMapping(const ScanCodeMapping *mappings, size_t count)
{
    _scanCodeMap.clear();

    if (count > 0)
    {
        _scanCodeMap.reserve(count);
        _scanCodeMap.appendRange(mappings, mappings + count);
        _scanCodeMap.reindex();
    }
}

//! @brief Begins sending accumulated mouse position deltas, immediately
//! sending the first byte and setting the controller state to send the
//! second after an appropriate acknowledgement.
void AcornKeyboardController::beginSendMouseData()
{
    if (_state != ControllerState::Initialised)
        return;

    int32_t dx = _mouseDeltaX.exchange(0, std::memory_order_relaxed);
    int32_t dy = _mouseDeltaY.exchange(0, std::memory_order_relaxed);

    if (_txQueue == nullptr)
        return;

    // Clamp to 7-bit signed range (-64 to +63).
    dx = std::clamp(dx, -64, 63);
    dy = std::clamp(dy, -64, 63);

    // Prepare to end.
    _secondTxByte = static_cast<uint8_t>(dy) & MDAT_Mask;
    _state = ControllerState::AfterFirstByteSent;

    _txQueue->enqueue(static_cast<uint8_t>(dx) & MDAT_Mask);
}

//! @brief Begins sending a key event, immediately sending the first byte and
//! setting the controller state to send the second after an appropriate
//! acknowledgement.
//! @param[in] scanCode The row/column scan code to send.
//! @param[in] isPressed True for a key press event, false for a release event.
void AcornKeyboardController::beginSendKeyEvent(uint8_t scanCode, bool isPressed)
{
    if ((_state != ControllerState::Initialised) || (_txQueue == nullptr))
        return;

    uint8_t row = (scanCode >> 4) & KDDA_Mask;
    uint8_t col = scanCode & KDDA_Mask;

    if (isPressed)
    {
        _secondTxByte = KDDA_Bits | col;
        _state = ControllerState::AfterFirstByteSent;
        _txQueue->enqueue(KDDA_Bits | row);
    }
    else
    {
        _secondTxByte = KUDA_Bits | col;
        _state = ControllerState::AfterFirstByteSent;
        _txQueue->enqueue(KUDA_Bits | row);
    }
}

//! @brief Determines if a byte received by the controller is a form of
//! acknowledgement and processes it accordingly.
//! @param[in] rxByte The byte received by the controller.
//! @retval true @p rxByte was an acknowledgement code and was processed.
//! @retval false @p rxByte did not represent an acknowledgement.
bool AcornKeyboardController::handleAcknowledge(uint8_t rxByte)
{
    bool isAck = true;

    // Claude: Host enable/acknowledge bytes. These configure which data
    // the keyboard should report (keys, mouse, both, or neither)
    // but do not elicit a response. The host does not wait for
    // one; sending a response would de-synchronise the protocol.
    switch (rxByte)
    {
    case NACK:
        // Disable both key scanning mode and unsolicited mouse reporting.
        _keyScanningEnabled = false;
        _mouseTxEnabled = false;
        break;

    case SACK:
        // Enable key scanning mode, but disable unsolicited mouse reporting.
        _keyScanningEnabled = true;
        _mouseTxEnabled = false;
        break;

    case MACK:
        // Disable key scanning mode, but enable unsolicited mouse reporting.
        _keyScanningEnabled = false;
        _mouseTxEnabled = true;
        break;

    case SMAK:
        // Enable both key scanning mode and unsolicited mouse reporting.
        _keyScanningEnabled = true;
        _mouseTxEnabled = true;
        break;

    default:
        isAck = false;
        break;
    }

    return isAck;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


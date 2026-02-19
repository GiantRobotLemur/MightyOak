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

#include "AcornKeyboardController.hpp"
#include "ArmEmu/IOC.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

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
    _ioController(nullptr),
    _mouseDeltaX(0),
    _mouseDeltaY(0),
    _state(ControllerState::PreReset)
{
}

//! @brief Processes a byte sent from IOC via the KART interface.
//! @param[in] nextByte The byte sent to the keyboard controller.
void AcornKeyboardController::receiveKARTByte(uint8_t nextByte)
{
    // We can't do anything if we can't send bytes back to IOC.
    if (_ioController == nullptr)
        return;

    bool hasError = true;

    switch (_state)
    {
    case ControllerState::PreReset:
    default:
        if (nextByte == HRST)
        {
            _state = ControllerState::ReceivedHRST;
            _ioController->writeKartByte(HRST);
            hasError = false;
        }
        break;

    case ControllerState::ReceivedHRST:
        if (nextByte == RAK1)
        {
            _state = ControllerState::ReceivedRAK1;
            _ioController->writeKartByte(RAK1);
            hasError = false;
        }
        break;

    case ControllerState::ReceivedRAK1:
        if (nextByte == RAK2)
        {
            // Handshake complete. Transition to Initialised state.
            _state = ControllerState::Initialised;
            _ioController->writeKartByte(RAK2);
            hasError = false;
        }
        break;

    case ControllerState::Initialised:
        hasError = false;

        if (nextByte == HRST)
        {
            // Host requested a hard reset.
            _state = ControllerState::ReceivedHRST;
            _ioController->writeKartByte(RAK1);
        }
        else if (nextByte == PRST)
        {
            // Protocol reset - restart the handshake.
            _state = ControllerState::PreReset;
            _ioController->writeKartByte(HRST);
        }
        else if (nextByte == RQID)
        {
            // Host requested the keyboard ID.
            _ioController->writeKartByte(KBID_Bits | KeyboardId);
        }
        else if (nextByte == RQMP)
        {
            // Host requested mouse position data.
            sendMouseData();
        }
        else if (nextByte <= LEDS_Mask)
        {
            // LED state control (0x00-0x07). Acknowledge and send status.
            _ioController->writeKartByte(getStatusByte());
        }
        else if (nextByte == SACK || nextByte == NACK ||
                 nextByte == MACK || nextByte == SMAK ||
                 nextByte == BACK)
        {
            // Host acknowledgement. Send any pending key/mouse data
            // or a status byte.
            sendPendingData();
        }
        else if ((nextByte & ~RQPD_Mask) == RQPD_Bits)
        {
            // Host requested key data for a specific row. Send an empty
            // response as row-level polling is not yet fully supported.
            _ioController->writeKartByte(PDAT_Bits | 0);
        }
        else
        {
            // Unrecognised command.
            hasError = true;
        }
        break;
    }

    if (hasError)
    {
        // Re-send the hard reset signal and reset the protocol.
        _state = ControllerState::PreReset;
        _ioController->writeKartByte(HRST);
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

// Inherited from IHardwareDevice.
void AcornKeyboardController::connect(const ConnectionContext &context)
{
    IHardwareDevicePtr iocDevice;

    if (context.tryFindDevice("IOC", iocDevice))
    {
        _ioController = dynamic_cast<IOC *>(iocDevice);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::keyDown(uint32_t hostScanCode)
{
    uint32_t guestScanCode;

    if (_scanCodeMap.tryFind(hostScanCode, guestScanCode))
    {
        KeyEvent event;
        event.ScanCode = static_cast<uint8_t>(guestScanCode);
        event.IsDown = true;
        _pendingKeyEvents.enqueue(event);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::keyUp(uint32_t hostScanCode)
{
    uint32_t guestScanCode;

    if (_scanCodeMap.tryFind(hostScanCode, guestScanCode))
    {
        KeyEvent event;
        event.ScanCode = static_cast<uint8_t>(guestScanCode);
        event.IsDown = false;
        _pendingKeyEvents.enqueue(event);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseButtonDown(AcornKeyboardController::MouseButton button)
{
    uint32_t guestScanCode;

    if (tryMapMouseButton(button, guestScanCode))
    {
        KeyEvent event;
        event.ScanCode = static_cast<uint8_t>(guestScanCode);
        event.IsDown = true;
        _pendingKeyEvents.enqueue(event);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseButtonUp(AcornKeyboardController::MouseButton button)
{
    uint32_t guestScanCode;

    if (tryMapMouseButton(button, guestScanCode))
    {
        KeyEvent event;
        event.ScanCode = static_cast<uint8_t>(guestScanCode);
        event.IsDown = false;
        _pendingKeyEvents.enqueue(event);
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseDelta(int32_t deltaX, int32_t deltaY)
{
    _mouseDeltaX.fetch_add(deltaX, std::memory_order_relaxed);
    _mouseDeltaY.fetch_add(deltaY, std::memory_order_relaxed);
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

//! @brief Determines the status byte to send to the host based on pending
//! key and mouse data.
//! @returns One of SACK, NACK, MACK, or SMAK.
uint8_t AcornKeyboardController::getStatusByte() const
{
    bool hasKeyData = _pendingKeyEvents.peek() != nullptr;
    bool hasMouseData = (_mouseDeltaX.load(std::memory_order_relaxed) != 0) ||
                        (_mouseDeltaY.load(std::memory_order_relaxed) != 0);

    if (hasKeyData && hasMouseData)
        return SMAK;
    else if (hasKeyData)
        return SACK;
    else if (hasMouseData)
        return MACK;

    return NACK;
}

//! @brief Sends pending key event data to the host, preceded by a status byte.
//! If no key data is pending, sends a status byte only.
void AcornKeyboardController::sendPendingData()
{
    KeyEvent event;

    if (_pendingKeyEvents.try_dequeue(event))
    {
        // Send the key event data, then a status byte indicating
        // whether more data is available.
        sendKeyEvent(event);
        _ioController->writeKartByte(getStatusByte());
    }
    else
    {
        // No key data pending, send current status.
        _ioController->writeKartByte(getStatusByte());
    }
}

//! @brief Encodes and sends a key event as a two-byte KDDA or KUDA sequence.
//! @param[in] event The key event to send.
void AcornKeyboardController::sendKeyEvent(const KeyEvent &event)
{
    uint8_t row = (event.ScanCode >> 4) & KDDA_Mask;
    uint8_t col = event.ScanCode & KDDA_Mask;

    if (event.IsDown)
    {
        _ioController->writeKartByte(KDDA_Bits | row);
        _ioController->writeKartByte(KDDA_Bits | col);
    }
    else
    {
        _ioController->writeKartByte(KUDA_Bits | row);
        _ioController->writeKartByte(KUDA_Bits | col);
    }
}

//! @brief Sends accumulated mouse delta data to the host as two MDAT bytes.
void AcornKeyboardController::sendMouseData()
{
    int32_t dx = _mouseDeltaX.exchange(0, std::memory_order_relaxed);
    int32_t dy = _mouseDeltaY.exchange(0, std::memory_order_relaxed);

    // Clamp to 7-bit signed range (-64 to +63).
    dx = std::clamp(dx, -64, 63);
    dy = std::clamp(dy, -64, 63);

    _ioController->writeKartByte(static_cast<uint8_t>(dx) & MDAT_Mask);
    _ioController->writeKartByte(static_cast<uint8_t>(dy) & MDAT_Mask);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


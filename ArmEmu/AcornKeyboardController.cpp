//! @file ArmEmu/AcornAcornKeyboardController.cpp
//! @brief The definition of an object which represents the 87C51 keyboard
//! controller, translating between key press and mouse movement and the
//! internal KART protocol.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
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
        guestScanCode = 0;
        break;

    case AcornKeyboardController::MiddleButton:
        guestScanCode = 0;
        break;

    case AcornKeyboardController::RightButton:
        guestScanCode = 0;
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
            _ioController->writeKartByte(RAK1);
            hasError = false;
        }
        break;

    case ControllerState::ReceivedHRST:
        if (nextByte == RAK1)
        {
            _state = ControllerState::ReceivedRAK1;
            _ioController->writeKartByte(RAK2);
            hasError = false;
        }
        break;

    case ControllerState::ReceivedRAK1:
        if (nextByte == RAK2)
        {
            _state = ControllerState::ReceivedRAK1;
            _ioController->writeKartByte(RAK2);
            hasError = false;
        }
        break;

    case ControllerState::Initialised:
        if (nextByte == HRST)
        {
            _state = ControllerState::ReceivedHRST;
            _ioController->writeKartByte(RAK1);
            hasError = false;
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
    IHardwreDevicePtr iocDevice;

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
        // TODO: Queue traffic to send to the KART for a key press.
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::keyUp(uint32_t hostScanCode)
{
    uint32_t guestScanCode;

    if (_scanCodeMap.tryFind(hostScanCode, guestScanCode))
    {
        // TODO: Queue traffic to send to the KART for a key release.
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseButtonDown(AcornKeyboardController::MouseButton button)
{
    uint32_t guestScanCode;

    if (tryMapMouseButton(button, guestScanCode))
    {
        // TODO: Queue traffic to send to the KART for a mouse button release.
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseButtonUp(AcornKeyboardController::MouseButton button)
{
    uint32_t guestScanCode;

    if (tryMapMouseButton(button, guestScanCode))
    {
        // TODO: Queue traffic to send to the KART for a mouse button release.
    }
}

// Inherited from IKeyboardController.
void AcornKeyboardController::mouseDelta(int32_t deltaX, int32_t deltaY)
{
    // TODO: Queue traffic to send to the KART to report a mouse movement.
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

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


//! @file MightyOakLib/EmulatorSession.cpp
//! @brief The definition of an object which holds the runtime state of the
//! system being emulated.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"

#include "MightyOakLib/EmulatorSession.hpp"
#include "ArmEmu/ArmSystemBuilder.hpp"

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An implementation of IHostConnection which connects to an emulator
//! session.
class SessionConnection : public Arm::IHostConnection
{
public:
    // Construction/Destruction
    SessionConnection(uint32_t guestEventMessageId);
    virtual ~SessionConnection() = default;

    // Overrides
    virtual void onGuestEvent(Arm::IArmSystem *instance,
                              const Arm::GuestEvent &e) override;
private:
    // Internal Fields
    uint32_t _guestEventMessageId = 0;
};


////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
//! @brief A map of SDL Scan Codes (positional key definitions) to Acorn
//! keyboard scan codes.
//! @remarks Acorn scan codes are stored with the column in the least significant
//! nibble (bits 0-3), and the row in bits 4-7.
//! The following values were extracted from page 17-18 of the A3000 technical
//! reference manual (A0018018).
static const Arm::IKeyboardController::ScanCodeMapping sdlKeyMappings[] = {
    { SDL_SCANCODE_ESCAPE, 0x00 },
    { SDL_SCANCODE_F1, 0x01 },
    { SDL_SCANCODE_F2, 0x03 },
    { SDL_SCANCODE_F3, 0x03 },
    { SDL_SCANCODE_F4, 0x04 },
    { SDL_SCANCODE_F5, 0x05 },
    { SDL_SCANCODE_F6, 0x06 },
    { SDL_SCANCODE_F7, 0x07 },
    { SDL_SCANCODE_F8, 0x08 },
    { SDL_SCANCODE_F9, 0x09 },
    { SDL_SCANCODE_F10, 0x0A },
    { SDL_SCANCODE_F11, 0x0B },
    { SDL_SCANCODE_F12, 0x0C },
    { SDL_SCANCODE_PRINTSCREEN, 0x0D },
    { SDL_SCANCODE_SCROLLLOCK, 0x0E },
    { SDL_SCANCODE_PAUSE, 0x0F },

    { SDL_SCANCODE_GRAVE, 0x10 },
    { SDL_SCANCODE_1, 0x11 },
    { SDL_SCANCODE_2, 0x12 },
    { SDL_SCANCODE_3, 0x13 },
    { SDL_SCANCODE_4, 0x14 },
    { SDL_SCANCODE_5, 0x15 },
    { SDL_SCANCODE_6, 0x16 },
    { SDL_SCANCODE_7, 0x17 },
    { SDL_SCANCODE_8, 0x18 },
    { SDL_SCANCODE_9, 0x19 },
    { SDL_SCANCODE_0, 0x1A },
    { SDL_SCANCODE_MINUS, 0x1B },
    { SDL_SCANCODE_EQUALS, 0x1C },
    // { SDL_SCANCODE_, 0x1D }, // See images of Acorn keyboard for '£'/circle-with-diagonal extensions
    { SDL_SCANCODE_BACKSPACE, 0x1E },
    { SDL_SCANCODE_INSERT, 0x1F },

    { SDL_SCANCODE_HOME, 0x20 },
    { SDL_SCANCODE_PAGEUP, 0x21 },
    { SDL_SCANCODE_NUMLOCKCLEAR, 0x22 },
    { SDL_SCANCODE_KP_DIVIDE, 0x23 },
    { SDL_SCANCODE_KP_MULTIPLY, 0x24 },
    { SDL_SCANCODE_KP_HASH, 0x25 }, // The num pad '#' on Acorn keyboards, KP minus on PC
    { SDL_SCANCODE_TAB, 0x26 },
    { SDL_SCANCODE_Q, 0x27 },
    { SDL_SCANCODE_W, 0x28 },
    { SDL_SCANCODE_E, 0x29 },
    { SDL_SCANCODE_R, 0x2A },
    { SDL_SCANCODE_T, 0x2B },
    { SDL_SCANCODE_Y, 0x2C },
    { SDL_SCANCODE_U, 0x2D },
    { SDL_SCANCODE_I, 0x2E },
    { SDL_SCANCODE_O, 0x2F },

    { SDL_SCANCODE_P, 0x30 },
    { SDL_SCANCODE_LEFTBRACKET, 0x31 },
    { SDL_SCANCODE_RIGHTBRACKET, 0x32 },
    { SDL_SCANCODE_BACKSLASH, 0x33 }, // A 1.5 size key left of upper Return on the Acorn keyboard
    { SDL_SCANCODE_DELETE, 0x34 },
    { SDL_SCANCODE_END, 0x35 }, // Copy on the Acorn keyboard
    { SDL_SCANCODE_PAGEDOWN, 0x36 },
    { SDL_SCANCODE_KP_7, 0x37 },
    { SDL_SCANCODE_KP_8, 0x38 },
    { SDL_SCANCODE_KP_9, 0x39 },
    { SDL_SCANCODE_KP_MINUS, 0x3A }, // Mixed up with KP_HASH due to positioning on Acorn keyboard.
    { SDL_SCANCODE_CAPSLOCK, 0x3B }, // Left Ctrl on PC keyboards
    { SDL_SCANCODE_A, 0x3C },
    { SDL_SCANCODE_S, 0x3D },
    { SDL_SCANCODE_D, 0x3E },
    { SDL_SCANCODE_F, 0x3F },

    { SDL_SCANCODE_G, 0x40 },
    { SDL_SCANCODE_H, 0x41 },
    { SDL_SCANCODE_J, 0x42 },
    { SDL_SCANCODE_K, 0x43 },
    { SDL_SCANCODE_L, 0x44 },
    { SDL_SCANCODE_SEMICOLON, 0x45 },
    { SDL_SCANCODE_APOSTROPHE, 0x46 },
    { SDL_SCANCODE_RETURN, 0x47 },
    { SDL_SCANCODE_KP_4, 0x48 },
    { SDL_SCANCODE_KP_5, 0x49 },
    { SDL_SCANCODE_KP_6, 0x4A },
    { SDL_SCANCODE_KP_PLUS, 0x4B },
    { SDL_SCANCODE_LSHIFT, 0x4C },
    //{ SDL_SCANCODE_BACKSLASH, 0x4D }, // '\'/'|' key on PC keyboard, no mapping on Acorn keyboard.
    { SDL_SCANCODE_Z, 0x4E },
    { SDL_SCANCODE_X, 0x4F },

    { SDL_SCANCODE_C, 0x50 },
    { SDL_SCANCODE_V, 0x51 },
    { SDL_SCANCODE_B, 0x52 },
    { SDL_SCANCODE_N, 0x53 },
    { SDL_SCANCODE_M, 0x54 },
    { SDL_SCANCODE_COMMA, 0x55 },
    { SDL_SCANCODE_PERIOD, 0x56 },
    { SDL_SCANCODE_SLASH, 0x57 },
    { SDL_SCANCODE_RSHIFT, 0x58 },
    { SDL_SCANCODE_UP, 0x59 },
    { SDL_SCANCODE_KP_1, 0x5A },
    { SDL_SCANCODE_KP_2, 0x5B },
    { SDL_SCANCODE_KP_3, 0x5C },
    { SDL_SCANCODE_LCTRL, 0x5D }, // Caps lock on PC keyboard
    { SDL_SCANCODE_LALT, 0x5E },
    { SDL_SCANCODE_SPACE, 0x5F },

    { SDL_SCANCODE_RALT, 0x60 },
    { SDL_SCANCODE_RCTRL, 0x61 },
    { SDL_SCANCODE_LEFT, 0x62 },
    { SDL_SCANCODE_DOWN, 0x63 },
    { SDL_SCANCODE_RIGHT, 0x64 },
    { SDL_SCANCODE_KP_0, 0x65 },
    { SDL_SCANCODE_KP_PERIOD, 0x66 },
    { SDL_SCANCODE_KP_ENTER, 0x67 },
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// EmulatorSession Member Definitions
////////////////////////////////////////////////////////////////////////////////
EmulatorSession::EmulatorSession(const Arm::Options &configuration,
                                 uint32_t guestEventId) :
    _configuration(configuration),
    _hostConnection(std::make_shared<SessionConnection>(guestEventId))
{
}

//! @brief Returns the configuration of the system being emulated.
const Arm::Options &EmulatorSession::getConfiguration() const
{
    return _configuration;
}

//! @brief Gets the emulated ARM system, possibly nullptr if not yet created.
Arm::IArmSystem *EmulatorSession::getSystem()
{
    return _system.get();
}

//! @brief Gets the emulated ARM system, possibly nullptr if not yet created.
const Arm::IArmSystem *EmulatorSession::getSystem() const
{
    return _system.get();
}

//! @brief Creates the emulated ARM system based on the stored configuration.
//! @param[out] error Receives a description of any error that occurred.
//! @retval true The system was created successfully.
//! @retval false The system could not be created, error describes why.
bool EmulatorSession::createSystem(Ag::String &error)
{
    try
    {
        Arm::ArmSystemBuilder builder(_configuration);
        builder.setHostConnection(_hostConnection);
        _system = builder.createSystem();

        if (_system->tryFindTypedDevice("Keyboard Controller", _guestKeyboard))
        {
            // Initialise the keyboard controller with mappings.
            _guestKeyboard->setKeyMapping(sdlKeyMappings, std::size(sdlKeyMappings));
        }

        return true;
    }
    catch (const Ag::Exception &ex)
    {
        error = ex.getMessage();
    }
    catch (const std::exception &ex)
    {
        error = ex.what();
    }

    return false;
}

void EmulatorSession::onKeyChange(SDL_Scancode keyCode, bool isPressed)
{
    if (_guestKeyboard != nullptr)
    {
        if (isPressed)
            _guestKeyboard->keyDown(keyCode);
        else
            _guestKeyboard->keyUp(keyCode);
    }
}

void EmulatorSession::onMouseButtonChange(SDL_MouseButtonFlags sdlButton,
                                          bool isPressed)
{
    if (_guestKeyboard != nullptr)
    {
        Arm::IKeyboardController::MouseButton button;

        switch (sdlButton)
        {
        case SDL_BUTTON_LEFT: button = Arm::IKeyboardController::LeftButton; break;
        case SDL_BUTTON_MIDDLE: button = Arm::IKeyboardController::MiddleButton; break;
        case SDL_BUTTON_RIGHT: button = Arm::IKeyboardController::RightButton; break;
        default: return;
        }

        if (isPressed)
            _guestKeyboard->mouseButtonDown(button);
        else
            _guestKeyboard->mouseButtonUp(button);
    }
}

void EmulatorSession::onMouseMove(int dx, int dy)
{
    if (_guestKeyboard != nullptr)
    {
        _guestKeyboard->mouseDelta(dx, dy);
    }
}

////////////////////////////////////////////////////////////////////////////////
// SessionConnection Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an IHostConnection implementation which passes guest
//! events on to the SDL input thread.
//! @param[in] guestEventMessageId The identifier allocated by SDL for encoding
//! guest events as SDL_Event values.
SessionConnection::SessionConnection(uint32_t guestEventMessageId) :
    _guestEventMessageId(guestEventMessageId)
{
}

// Inherited from IHostConnection.
void SessionConnection::onGuestEvent(Arm::IArmSystem */*instance*/,
                                     const Arm::GuestEvent &e)
{
    if ((e.Type >= Arm::HostMessageID::VSyncOccurred) &&
        (e.Type < Arm::HostMessageID::LastHostMessage))
    {
        // Post the event to the input thread.
        SDL_Event guestEvent;
        Ag::zeroFill(guestEvent);

        guestEvent.type = _guestEventMessageId;
        guestEvent.user.timestamp = e.Timestamp;
        guestEvent.user.code = static_cast<decltype(guestEvent.user.code)>(e.Type);
        guestEvent.user.data1 = reinterpret_cast<void *>(e.Data1);
        guestEvent.user.data2 = reinterpret_cast<void *>(e.Data2);

        SDL_PushEvent(&guestEvent);
    }
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


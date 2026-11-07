//! @file MightyOakLib/AppContext.cpp
//! @brief The definition of an object passed between states of the application.
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

#include "MightyOakLib/AppContext.hpp"
#include "MightyOakLib/CliOptions.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// AppContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty context to share between application states.
AppContext::AppContext() :
    _guestMessageId(0)
{
    // Allocate a message which can be used to connect the emulator to
    // the input thread.
    _guestMessageId = SDL_RegisterEvents(1);

    if (_guestMessageId == 0)
        throw Ag::SDL3::ApiException("SDL_RegisterEvents()");
}

//! @brief Gets the current emulator session, possibly nullptr.
EmulatorSession *AppContext::getSession()
{
    return _currentSession.get();
}

//! @brief Gets the current emulator session, possibly nullptr.
const EmulatorSession *AppContext::getSession() const
{
    return _currentSession.get();
}

//! @brief Gets the object representing the main application window.
Ag::SDL3::Window &AppContext::getMainWindow()
{
    return _mainWindow;
}

//! @brief Gets the object representing the main application window.
const Ag::SDL3::Window &AppContext::getMainWindow() const
{
    return _mainWindow;
}

//! @brief Gets the SDL_Event type used to marshal guest events to the SDL
//! message queue.
//! @return A pre-allocated message ID.
uint32_t AppContext::getGuestEventMessageId() const
{
    return _guestMessageId;
}

//! @brief Determines whether an SDL event was posted by the guest system.
//! @param[in] sdlEvent The event to analyse.
//! @param[out] e Receives details of the event, if successful.
//! @retval true The @p sdlEvent represent a guest system event, the contents
//! of @p e have been updated.
//! @retval false The event is not from a guest system.
bool AppContext::isGuestEvent(const SDL_Event *sdlEvent,
                              Arm::GuestEvent &e) const
{
    if ((_guestMessageId != 0) &&
        (sdlEvent->type == _guestMessageId))
    {
        auto &user = sdlEvent->user;

        e.Timestamp = user.timestamp;
        e.Data1 = reinterpret_cast<uintptr_t>(user.data1);
        e.Data2 = reinterpret_cast<uintptr_t>(user.data2);
        e.Type = static_cast<decltype(e.Type)>(user.code);

        return true;
    }

    return false;
}

//! @brief Creates the initial windows and device resources required by all
//! application states.
//! @param[in] appOptions The options parsed from the command line.
void AppContext::initialise(const CliOptions &appOptions)
{
    _mainWindow.create("Mighty Oak", 640, 480, SDL_WINDOW_RESIZABLE);

    // Create an emulator session from the CLI options.
    _currentSession = std::make_unique<EmulatorSession>(appOptions.getEmulatedSystemConfig(),
                                                        _guestMessageId);

    // Create the emulated system from the session configuration.
    Ag::String error;

    if (_currentSession->createSystem(error) == false)
    {
        _currentSession.reset();

        throw Ag::OperationException(error.getUtf8Bytes());
    }
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


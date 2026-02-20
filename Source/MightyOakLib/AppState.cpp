//! @file MightyOakLib/AppState.cpp
//! @brief The definition of a base class for implementations which perform
//! application state-specific behaviour.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "MightyOakLib/AppContext.hpp"
#include "MightyOakLib/AppState.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

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

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// AppState Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a new application state.
AppState::AppState(AppContext *sharedContext) :
    _sharedContext(sharedContext)
{
}

//! @brief Determines if the state is currently actively running.
//! @retval true The state is processing input messages for the application.
//! @retval false The state is not yet processing messages or has exited.
bool AppState::isRunning() const
{
    return (_eventProcessor != nullptr) && _eventProcessor->isRunning();
}

//! @brief Gets the context shared between application states.
AppContext *AppState::getContext()
{
    return _sharedContext;
}

//! @brief Gets the context shared between application states.
const AppContext *AppState::getContext() const
{
    return _sharedContext;
}

//! @brief Runs the app state to completion.
//! @return The successor state, or nullptr to exit the application.
AppState *AppState::run()
{
    if ((_eventProcessor != nullptr) && _eventProcessor->isRunning())
        return this;

    _eventProcessor = configure();

    if (_eventProcessor)
    {
        _eventProcessor->getEventDispatcher().registerEventHandler(SDL_EVENT_WINDOW_CLOSE_REQUESTED,
                                                                   onWindowClose,
                                                                   reinterpret_cast<uintptr_t>(this));

        return runInternal();
    }

    // No event processor was instantiated, close the app.
    return nullptr;
}

//! @brief Allows the state to initialise and pre-configure an event processor object.
//! @return The object to process message while the state is active.
//! @note The base implementation instantiates an IdleEventProcessor.
std::unique_ptr<Ag::SDL3::EventProcessor> AppState::configure()
{
    return std::make_unique<Ag::SDL3::IdleEventProcessor>();
}

//! @brief Processes events while the state is active.
//! @return The successor state, or nullptr if the application should exit.
//! @note The base implementation simply runs the event processor and returns
//! with no successor state.
AppState *AppState::runInternal()
{
    getEventProcessor()->run();

    return nullptr;
}

//! @brief Gets the object which processes events while the state is active.
Ag::SDL3::EventProcessor *AppState::getEventProcessor()
{
    return _eventProcessor.get();
}

//! @brief Gets the object which processes events while the state is active.
const Ag::SDL3::EventProcessor *AppState::getEventProcessor() const
{
    return _eventProcessor.get();
}

//! @brief Handles the SDL_EVENT_WINDOW_CLOSE_REQUESTED to ensure the state
//! exits if the main window is closed.
//! @param[in] context A pointer to the AppState object.
//! @param[in] info Details of the window event which occurred.
//! @retval true To continue processing input events.
//! @retval false To request the message processing loop exit.
bool AppState::onWindowClose(uintptr_t context, SDL_Event *info)
{
    SDL_WindowEvent *eventInfo = reinterpret_cast<SDL_WindowEvent *>(info);
    AppState *thisState = reinterpret_cast<AppState *>(context);

    // Force the app to exit if the main window is closed.
    return (eventInfo->windowID != thisState->getContext()->getMainWindow().getID());
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


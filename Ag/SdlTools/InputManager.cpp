//! @file SdlTools/InputManager.cpp
//! @brief The definition of an object which runs a thread event loop.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"
#include "Ag/SdlTools/ApiException.hpp"
#include "Ag/SdlTools/InputManager.hpp"

namespace Ag {
namespace SDL {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the classifications of SDL events the InputManager
//! can process.
enum class EventClass : uint8_t
{
    Unknown,
    Window,
    System,
    Keyboard,
    Mouse,
    Joystick,
    Controller,
    Gesture,
    DragAndDrop,
    Audio,
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Classifies an SDL event and attempts to determine its source.
//! @param[in] currentEvent A structure defining the event to classify.
//! @param[out] eventType The classification of the event.
//! @return The integer identifier of the entity which generated the event, or
//! 0 if the source cannot be determined.
uint32_t getEventSource(const SDL_Event &currentEvent, EventClass &eventType)
{
    uint32_t source = 0;
    eventType = EventClass::Unknown;

    switch (currentEvent.type)
    {
    case SDL_WINDOWEVENT:
        source = currentEvent.window.windowID;
        eventType = EventClass::Window;
        break;

    case SDL_SYSWMEVENT:
        break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
        source = currentEvent.key.windowID;
        eventType = EventClass::Keyboard;
        break;

    case SDL_MOUSEMOTION:
        source = currentEvent.motion.windowID;
        eventType = EventClass::Mouse;
        break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        source = currentEvent.motion.windowID;
        eventType = EventClass::Mouse;
        break;

    case SDL_MOUSEWHEEL:
        source = currentEvent.wheel.windowID;
        eventType = EventClass::Mouse;
        break;

    case SDL_JOYAXISMOTION:
        source = currentEvent.jaxis.which;
        eventType = EventClass::Joystick;
        break;

    case SDL_JOYBALLMOTION:
        source = currentEvent.jball.which;
        eventType = EventClass::Joystick;
        break;

    case SDL_JOYHATMOTION:
        source = currentEvent.jhat.which;
        eventType = EventClass::Joystick;
        break;

    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        source = currentEvent.jbutton.which;
        eventType = EventClass::Joystick;
        break;

    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:
        source = currentEvent.jdevice.which;
        eventType = EventClass::Joystick;
        break;

    case SDL_CONTROLLERAXISMOTION:
        source = currentEvent.caxis.which;
        eventType = EventClass::Controller;
        break;

    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        source = currentEvent.cbutton.which;
        eventType = EventClass::Controller;
        break;

    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
    case SDL_CONTROLLERDEVICEREMAPPED:
        source = currentEvent.cdevice.which;
        eventType = EventClass::Controller;
        break;

    case SDL_CONTROLLERTOUCHPADDOWN:
    case SDL_CONTROLLERTOUCHPADMOTION:
    case SDL_CONTROLLERTOUCHPADUP:
    case SDL_CONTROLLERSENSORUPDATE:
        eventType = EventClass::Controller;
        break;

    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
    case SDL_FINGERMOTION:
        currentEvent.tfinger.windowID;
        eventType = EventClass::Gesture;
        break;

    case SDL_DOLLARGESTURE:
    case SDL_DOLLARRECORD:
        currentEvent.dgesture.touchId;
        eventType = EventClass::Gesture;
        break;

    case SDL_MULTIGESTURE:
        currentEvent.mgesture.touchId;
        eventType = EventClass::Gesture;
        break;

    case SDL_DROPFILE:
    case SDL_DROPTEXT:
    case SDL_DROPBEGIN:
    case SDL_DROPCOMPLETE:
        source = currentEvent.drop.windowID;
        eventType = EventClass::DragAndDrop;
        break;

    case SDL_AUDIODEVICEADDED:
    case SDL_AUDIODEVICEREMOVED:
        source = currentEvent.adevice.which;
        eventType = EventClass::Audio;
        break;

    default:
        source = 0;
        break;
    }

    return source;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// InputManager Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which manages input event and idle processing.
InputManager::InputManager() :
    _minCycleTimeMs(30),
    _processResult(0),
    _exitRequested(false),
    _quitOnClose(true)
{
}

//! @brief Gets whether the run() function will exit when the SDL_QUIT message
//! is processed.
bool InputManager::quitOnClose() const
{
    return _quitOnClose;
}

//! @brief Sets whether the run() function will exit when the SDL_QUIT message
//! is processed.
//! @param[in] quitOnLastWindowClose True to exit after the last window is closed,
//! false to only exit when explicitly requested to do so.
void InputManager::setQuitOnClose(bool quitOnLastWindowClose)
{
    _quitOnClose = quitOnLastWindowClose;
}

//! @brief Gets the maximum amount of time the thread will sleep while waiting
//! for an input message.
//! @return The cycle time in milliseconds.
uint64_t InputManager::getMinimumCycleTime() const
{
    return _minCycleTimeMs;
}

//! @brief Sets the maximum amount of time the thread will sleep while waiting
//! for an input message.
//! @param[in] cyclePeriodMs The maximum wait duration in milliseconds.
void InputManager::setMinimumCycleTime(uint64_t cyclePeriodMs)
{
    _minCycleTimeMs = cyclePeriodMs;
}

//! @brief Runs the message loop until exit is requested.
//! @return The process result specified when the quit message was processed.
int InputManager::run()
{
    _processResult = 0;
    _exitRequested = false;

    SDL_Event currentEvent;

    while (!_exitRequested)
    {
        uint64_t cycleStart = SDL_GetTicks64();

        // Process all outstanding messages.
        while (SDL_PollEvent(&currentEvent))
        {
            dispatchEvent(currentEvent);
        }

        // Perform idle processing.
        for (auto pos = _idleTasks.begin(); pos != _idleTasks.end(); )
        {
            IDispatcherTaskSPtr task = pos->lock();

            if (task)
            {
                // Dispatch the task.
                task->doProcessing(*this);
                ++pos;
            }
            else
            {
                // Remove the expired reference to the task.
                pos = _idleTasks.erase(pos);
            }
        }

        // Calculate the time all the event and idle processing took.
        uint64_t cycleDurationMs = SDL_GetTicks64() - cycleStart;

        if (_idleTasks.empty())
        {
            if (SDL_WaitEvent(nullptr) == 0)
            {
                throw ApiException("SDL_WaitEvent()");
            }
        }
        else if (cycleDurationMs < _minCycleTimeMs)
        {
            // Wait a for up to the remainder of the cycle before
            // performing idle processing.
            SDL_WaitEventTimeout(nullptr, static_cast<int>(_minCycleTimeMs - cycleDurationMs));
        }
    }

    return _processResult;
}

//! @brief Requests that the run() function exit the next time it reaches
//! the top of the message loop.
//! @param[in] processResult The value to report to the operating system at
//! process exit.
void InputManager::requestExit(int processResult)
{
    _processResult = processResult;
    _exitRequested = true;
}

//! @brief Registers a task to be called in each input processing cycle.
//! @param[in] task A pointer to the task to periodically perform.
void InputManager::registerIdleTask(const IDispatcherTaskSPtr &task)
{
    _idleTasks.emplace_back(task);
}

//! @brief Registers an object to process window event generated by a
//! specified window.
//! @param[in] window The window to route events from.
//! @param[in] handler The object to perform event processing.
void InputManager::registerWindowEventHandler(SDL_Window *window,
                                              const IDispatcherTargetSPtr &handler)
{
    if (handler && (window != nullptr))
    {
        Handler key(toScalar(EventClass::Window), SDL_GetWindowID(window), handler);

        _handlers.push_back(key);
    }
}

//! @brief Removes all objects which process window events generated by a
//! specified window.
//! @param[in] window The window to stop routing events for.
void InputManager::deregisterWindowEventHandler(SDL_Window *window)
{
    Handler key(toScalar(EventClass::Window), SDL_GetWindowID(window));

    _handlers.erase(key);
}

//! @brief Registers an object to process keyboard events generated by a
//! specified window.
//! @param[in] window The window to route events from.
//! @param[in] handler The object to perform event processing.
void InputManager::registerKeyboardEventHandler(SDL_Window *window,
                                                const IDispatcherTargetSPtr &handler)
{
    if (handler && (window != nullptr))
    {
        Handler key(toScalar(EventClass::Keyboard), SDL_GetWindowID(window), handler);

        _handlers.push_back(key);
    }
}

//! @brief Removes all objects which process keyboard events generated by a
//! specified window.
//! @param[in] window The window to stop routing events for.
void InputManager::deregisterKeyboardEventHandler(SDL_Window *window)
{
    Handler key(toScalar(EventClass::Keyboard), SDL_GetWindowID(window));

    _handlers.erase(key);
}

//! @brief Registers an object to process mouse events generated by a
//! specified window.
//! @param[in] window The window to route events from.
//! @param[in] handler The object to perform event processing.
void InputManager::registerMouseEventHandler(SDL_Window *window,
                                             const IDispatcherTargetSPtr &handler)
{
    if (handler && (window != nullptr))
    {
        Handler key(toScalar(EventClass::Mouse), SDL_GetWindowID(window), handler);

        _handlers.push_back(key);
    }
}

//! @brief Removes all objects which process mouse events generated by a
//! specified window.
//! @param[in] window The window to stop routing events for.
void InputManager::deregisterMouseEventHandler(SDL_Window *window)
{
    Handler key(toScalar(EventClass::Mouse), SDL_GetWindowID(window));

    _handlers.erase(key);
}

//! @brief Constructs an empty object representing a message handler.
InputManager::Handler::Handler() :
    Type(SDL_FIRSTEVENT),
    Source(0),
    Target()
{
}

//! @brief Constructs an object used as a key to find matching
//! message handlers.
//! @param[in] type The type of event to search for based on the SDL_EventType
//! enumerated type.
//! @param[in] source The numeric identifier of the source of the event.
InputManager::Handler::Handler(uint32_t type, uint32_t source) :
    Type(type),
    Source(source),
    Target()
{
}

//! @brief Constructs an object describing an object to route events to.
//! @param[in] type The type of event to search for based on the SDL_EventType
//! enumerated type.
//! @param[in] source The numeric identifier of the source of the event.
//! @param[in] handler A pointer to the object which will handle the event.
InputManager::Handler::Handler(uint32_t type, uint32_t source,
                               const IDispatcherTargetWPtr &handler) :
    Type(type),
    Source(source),
    Target(handler)
{
}

//! @brief An operator used to index event handler objects based on their
//! type and source.
//! @param[in] rhs The handler to compare against.
//! @return A boolean value representing the result of a less-than comparison.
bool InputManager::Handler::operator<(const Handler &rhs) const 
{
    bool isLess = false;

    if (Source == rhs.Source)
    {
        isLess = Type < rhs.Type;
    }
    else
    {
        isLess = Source < rhs.Source;
    }

    return isLess;
}

//! @brief Dispatches an event to any relevant handlers.
//! @param[in] currentEvent The event to dispatch.
void InputManager::dispatchEvent(const SDL_Event &currentEvent)
{
    EventClass eventClass;
    uint32_t source = getEventSource(currentEvent, eventClass);

    // Ensure the handlers are sorted for optimal look-up.
    _handlers.reindex();

    Handler key(Ag::toScalar(eventClass), source);
    auto handlerRange = _handlers.findRange(key);

    for (auto handlerPos = handlerRange.Begin; handlerPos != handlerRange.End; )
    {
        IDispatcherTargetSPtr target = handlerPos->Target.lock();

        if (target)
        {
            // Dispatch event.
            target->processEvent(*this, currentEvent);

            ++handlerPos;
        }
        else
        {
            // The target has been destroyed, no need for the entry any more.
            handlerPos = _handlers.erase(handlerPos);
        }
    }

    if (_quitOnClose && (currentEvent.type == SDL_QUIT))
    {
        requestExit(0);
    }
}

}} // namespace Ag::SDL
////////////////////////////////////////////////////////////////////////////////


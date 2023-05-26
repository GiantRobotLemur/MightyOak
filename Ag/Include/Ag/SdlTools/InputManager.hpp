//! @file Ag/SdlTools/InputManager.hpp
//! @brief The declaration of an object which runs a thread event loop.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __SDL_TOOLS_INPUT_MANAGER_HPP__
#define __SDL_TOOLS_INPUT_MANAGER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <deque>
#include <memory>

#include <SDL2/SDL.h>

#include "Ag/Core/LinearSortedSet.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace SDL {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class InputManager;

//! @brief The base of an object which can be weakly referenced to handle events.
class IDispatcherTarget : public std::enable_shared_from_this<IDispatcherTarget>
{
public:
    virtual void processEvent(InputManager &manager,
                              const SDL_Event &nextEvent) =0;
};

//! @brief An alias for a shared pointer to an IDispatcherTarget implementation.
using IDispatcherTargetSPtr = std::shared_ptr<IDispatcherTarget>;

//! @brief An alias for a weak pointer to an IDispatcherTarget implementation.
using IDispatcherTargetWPtr = std::weak_ptr<IDispatcherTarget>;

//! @brief The base of an object which can be weakly referenced to perform
//! idle-time processing.
class IDispatcherTask : public std::enable_shared_from_this<IDispatcherTask>
{
public:
    virtual void doProcessing(InputManager &manager) = 0;
};

//! @brief An alias for a shared pointer to an IDispatcherTask implementation.
using IDispatcherTaskSPtr = std::shared_ptr<IDispatcherTask>;

//! @brief An alias for a weak pointer to an IDispatcherTask implementation.
using IDispatcherTaskWPtr = std::weak_ptr<IDispatcherTask>;

//! @brief An object which manages input event processing for the input thread.
//! @note Member functions should only be called from the input thread.
class InputManager
{
public:
    // Construction/Destruction
    InputManager();
    ~InputManager() = default;

    // Accessors
    bool quitOnClose() const;
    void setQuitOnClose(bool quitOnLastWindowClose);
    uint64_t getMinimumCycleTime() const;
    void setMinimumCycleTime(uint64_t cyclePeriodMs);

    // Operations
    int run();
    void requestExit(int processResult);
    void registerIdleTask(const IDispatcherTaskSPtr &task);
    void registerWindowEventHandler(SDL_Window *window, const IDispatcherTargetSPtr &handler);
    void deregisterWindowEventHandler(SDL_Window *window);
    void registerKeyboardEventHandler(SDL_Window *window, const IDispatcherTargetSPtr &handler);
    void deregisterKeyboardEventHandler(SDL_Window *window);
    void registerMouseEventHandler(SDL_Window *window, const IDispatcherTargetSPtr &handler);
    void deregisterMouseEventHandler(SDL_Window *window);
private:
    // Internal Types
    struct Handler
    {
        uint32_t Type;
        uint32_t Source;
        IDispatcherTargetWPtr Target;

        Handler();
        Handler(uint32_t type, uint32_t source);
        Handler(uint32_t type, uint32_t source, const IDispatcherTargetWPtr &handler);

        bool operator<(const Handler &rhs) const;
    };

    using HandlerRegister = Ag::LinearSortedSet<Handler>;
    using TaskCollection = std::vector<IDispatcherTaskWPtr>;

    // Internal Functions
    void dispatchEvent(const SDL_Event &currentEvent);

    // Internal Fields
    HandlerRegister _handlers;
    TaskCollection _idleTasks;
    uint64_t _minCycleTimeMs;
    int _processResult;
    bool _exitRequested;
    bool _quitOnClose;
};

}} // namespace Ag::SDL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

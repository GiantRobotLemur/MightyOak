//! @file MightyOakLib/SessionRunningState.hpp
//! @brief The declaration of an application state in which an emulated system
//! is running.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MIGHTY_OAK_LIB_SESSION_RUNNING_STATE_HPP__
#define __MIGHTY_OAK_LIB_SESSION_RUNNING_STATE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <thread>
#include <vector>

#include <SDL3/SDL.h>
#include <Ag/Sdl3Tools.hpp>

#include "AppState.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Arm {
class IKeyboardController;
class IVideoFrameProvider;
} // namespace Arm

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An application state in which an emulated system is running.
class SessionRunningState : public AppState
{
public:
    // Construction/Destruction
    SessionRunningState(AppContext *sharedContext);
    virtual ~SessionRunningState();

    // Accessors

    // Operations

    // Overrides
protected:
    std::unique_ptr<Ag::SDL3::EventProcessor> configure() override;
    AppState *runInternal() override;

private:
    // Internal Functions
    static bool onKeyDown(uintptr_t context, SDL_Event *event);
    static bool onKeyUp(uintptr_t context, SDL_Event *event);
    static bool onMouseMotion(uintptr_t context, SDL_Event *event);
    static bool onMouseButton(uintptr_t context, SDL_Event *event);
    static bool onGuestEvent(uintptr_t context, SDL_Event *event);
    void onFrameReceived(uint32_t frameId);
    void onFrameConfigChanged(const Arm::FrameGeometry &newGeometry);

    // Internal Fields
    std::thread _emulatorThread;
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    Arm::IKeyboardController *_keyboard;
    Arm::IVideoFrameProvider *_frameProvider;
    Arm::FrameGeometry _currentFrameGeometry;
    std::vector<uint32_t> _argb32Buffer;
    uint32_t _palette[256];
};

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

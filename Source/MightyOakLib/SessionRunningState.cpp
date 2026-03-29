//! @file MightyOakLib/SessionRunningState.cpp
//! @brief The definition of an application state in which an emulated system
//! is running.
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

#include "ArmEmu/IKeyboardController.hpp"

#include "MightyOakLib/SessionRunningState.hpp"
#include "MightyOakLib/AppContext.hpp"

namespace Mo {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
//! @brief The maximum raw framebuffer size to pre-allocate (256 KB covers
//! all standard Archimedes display modes).
static constexpr size_t MaxRawFrameBytes = 256 * 1024;

//! @brief The maximum display dimensions for ARGB32 buffer pre-allocation.
static constexpr size_t MaxDisplayPixels = 1024 * 1024;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Expands raw indexed framebuffer bytes into an ARGB32 buffer using
//! the supplied palette, performing sub-byte pixel unpacking for 1/2/4 BPP.
//! @param[in] raw The raw indexed framebuffer bytes.
//! @param[in] palette The 256-entry ARGB32 palette.
//! @param[out] argb32 The destination ARGB32 pixel buffer.
//! @param[in] width The display width in pixels.
//! @param[in] height The display height in scanlines.
//! @param[in] pixelFormat The bits per pixel (1, 2, 4, or 8).
//! @param[in] bytesPerRow The raw bytes per scanline.
void expandToARGB32(const uint8_t *raw, const uint32_t *palette,
                    uint32_t *argb32, uint32_t width, uint32_t height,
                    Mo::Arm::AcornPixelFormat pixelFormat, uint32_t bytesPerRow)
{
    const uint8_t *srcRow = raw;

    for (uint32_t y = 0; y < height; ++y)
    {
        const uint8_t *src = srcRow;
        uint32_t *dest = argb32 + (static_cast<size_t>(y) * width);
        uint32_t pixel = 0;

        switch (pixelFormat)
        {
        case Mo::Arm::AcornPixelFormat::Palettised1Bpp:
            while (pixel < width)
            {
                uint8_t byte = *src++;

                for (uint8_t bit = 0; bit < 8 && pixel < width; ++bit, ++pixel)
                    dest[pixel] = palette[(byte >> bit) & 1];
            }
            break;

        case Mo::Arm::AcornPixelFormat::Palettised2Bpp:
            while (pixel < width)
            {
                uint8_t byte = *src++;

                for (uint8_t shift = 0; shift < 8 && pixel < width;
                     shift += 2, ++pixel)
                {
                    dest[pixel] = palette[(byte >> shift) & 0x03];
                }
            }
            break;

        case Mo::Arm::AcornPixelFormat::Palettised4Bpp:
            while (pixel < width)
            {
                uint8_t byte = *src++;

                if (pixel < width)
                    dest[pixel++] = palette[byte & 0x0F];

                if (pixel < width)
                    dest[pixel++] = palette[(byte >> 4) & 0x0F];
            }
            break;

        case Mo::Arm::AcornPixelFormat::Palettised8Bpp:
            for (; pixel < width; ++pixel)
                dest[pixel] = palette[src[pixel]];
            break;

        default:
            break;
        }

        srcRow += bytesPerRow;
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// SessionRunningState Member Definitions
////////////////////////////////////////////////////////////////////////////////
SessionRunningState::SessionRunningState(AppContext *sharedContext) :
    AppState(sharedContext),
    _renderer(nullptr),
    _texture(nullptr),
    _keyboard(nullptr),
    _frameProvider(nullptr),
    _lastWidth(0),
    _lastHeight(0)
{
    _rawFrameBuffer.resize(MaxRawFrameBytes);
    _argb32Buffer.resize(MaxDisplayPixels);
    std::memset(_palette, 0, sizeof(_palette));
}

SessionRunningState::~SessionRunningState()
{
    // Ensure the emulator thread is stopped.
    if (_emulatorThread.joinable())
    {
        Arm::IArmSystem *system = getContext()->getSession()->getSystem();

        if (system != nullptr)
            system->raiseHostInterrupt();

        _emulatorThread.join();
    }

    if (_texture != nullptr)
    {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
    }

    if (_renderer != nullptr)
    {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
}

//! @brief Creates the event processor and rendering resources.
//! @return A PeriodicEventProcessor synchronised to the display refresh rate.
std::unique_ptr<Ag::SDL3::EventProcessor> SessionRunningState::configure()
{
    Ag::SDL3::Window &mainWindow = getContext()->getMainWindow();
    SDL_Window *rawWindow = mainWindow.verifyAccess("SessionRunningState::configure");

    // Create an SDL renderer for the main window.
    _renderer = SDL_CreateRenderer(rawWindow, nullptr);

    if (_renderer == nullptr)
    {
        throw Ag::SDL3::ApiException("SDL_CreateRenderer");
    }

    // Create a periodic event processor synchronised to the primary display.
    auto eventProcessor = std::make_unique<Ag::SDL3::PeriodicEventProcessor>(
        mainWindow.getDisplayID());

    // Schedule the frame rendering callback.
    eventProcessor->schedulePeriodicTask(onRenderFrame,
                                         reinterpret_cast<uintptr_t>(this));

    // Register input event handlers.
    Ag::SDL3::EventHandlers &dispatcher = eventProcessor->getEventDispatcher();
    uintptr_t ctx = reinterpret_cast<uintptr_t>(this);

    dispatcher.registerEventHandler(SDL_EVENT_KEY_DOWN, onKeyDown, ctx);
    dispatcher.registerEventHandler(SDL_EVENT_KEY_UP, onKeyUp, ctx);
    dispatcher.registerEventHandler(SDL_EVENT_MOUSE_MOTION, onMouseMotion, ctx);
    dispatcher.registerEventHandler(SDL_EVENT_MOUSE_BUTTON_DOWN, onMouseButton, ctx);
    dispatcher.registerEventHandler(SDL_EVENT_MOUSE_BUTTON_UP, onMouseButton, ctx);

    return eventProcessor;
}

//! @brief Launches the emulator thread and runs the SDL event loop.
//! @return nullptr to exit the application when the loop ends.
AppState *SessionRunningState::runInternal()
{
    Arm::IArmSystem *system = getContext()->getSession()->getSystem();

    if (system == nullptr)
        return nullptr;

    if (!system->tryFindTypedDevice("Keyboard Controller", _keyboard))
        return nullptr;

    if (!system->tryFindTypedDevice("DISPLAY", _frameProvider))
        return nullptr;

    // Launch the emulator on a separate thread.
    _emulatorThread = std::thread([system]() {
        system->run();
    });

    // Run the SDL event loop on the main thread. This blocks until the
    // window is closed or requestExit() is called.
    getEventProcessor()->run();

    // The event loop has exited. Stop the emulator thread.
    system->raiseHostInterrupt();
    _emulatorThread.join();

    return nullptr;
}

//! @brief Periodic callback invoked at the display refresh rate.
//! @param[in] context A pointer to the SessionRunningState instance.
//! @param[in] timeDelta Seconds elapsed since the last call.
//! @retval true Continue processing.
//! @retval false Request the event loop to exit.
bool SessionRunningState::onRenderFrame(uintptr_t context,
                                        double /*timeDelta*/)
{
    auto *self = reinterpret_cast<SessionRunningState *>(context);

    // Drain guest events.
    Arm::IArmSystem *system = self->getContext()->getSession()->getSystem();
    Arm::GuestEvent event;

    while (system->tryGetNextMessage(event))
    {
        // Process events as needed. For now, we just drain the queue.
    }

    self->renderFrame();

    return true;
}

//! @brief Renders the current emulator display frame to the SDL window.
//! @retval true A frame was rendered.
//! @retval false No frame was available.
bool SessionRunningState::renderFrame()
{
    Arm::RawFrameInfo info;

    if ((_frameProvider == nullptr) ||
        !_frameProvider->getRawFrame(_rawFrameBuffer.data(),
                                     _rawFrameBuffer.size(),
                                     _palette, info))
    {
        return false;
    }

    // Recreate the SDL texture if the display dimensions have changed.
    if (info.DisplayWidth != _lastWidth || info.DisplayHeight != _lastHeight)
    {
        if (_texture != nullptr)
            SDL_DestroyTexture(_texture);

        _texture = SDL_CreateTexture(_renderer,
                                     SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     static_cast<int>(info.DisplayWidth),
                                     static_cast<int>(info.DisplayHeight));

        if (_texture == nullptr)
            return false;

        _lastWidth = info.DisplayWidth;
        _lastHeight = info.DisplayHeight;

        // Ensure the ARGB32 buffer is large enough.
        size_t pixelCount = static_cast<size_t>(info.DisplayWidth) * info.DisplayHeight;

        if (_argb32Buffer.size() < pixelCount)
            _argb32Buffer.resize(pixelCount);
    }

    // Expand raw indexed pixels to ARGB32 using the palette.
    expandToARGB32(_rawFrameBuffer.data(), _palette, _argb32Buffer.data(),
                   info.DisplayWidth, info.DisplayHeight, info.DisplayFormat,
                   info.BytesPerRow);

    // Upload the ARGB32 buffer to the SDL texture.
    SDL_UpdateTexture(_texture, nullptr, _argb32Buffer.data(),
                      static_cast<int>(info.DisplayWidth * sizeof(uint32_t)));

    // Clear and render.
    // Use border colour as clear colour.
    // TODO: Refactor to allow palette changes during a frame, for the border
    // and the display frame, too.
    uint8_t br = 0; // static_cast<uint8_t>((info.BorderColour >> 16) & 0xFF);
    uint8_t bg = 0; // static_cast<uint8_t>((info.BorderColour >> 8) & 0xFF);
    uint8_t bb = 0; // static_cast<uint8_t>(info.BorderColour & 0xFF);

    SDL_SetRenderDrawColor(_renderer, br, bg, bb, 255);
    SDL_RenderClear(_renderer);

    // Render the frame buffer texture scaled to fill the window while
    // maintaining the aspect ratio.
    SDL_RenderTexture(_renderer, _texture, nullptr, nullptr);
    SDL_RenderPresent(_renderer);

    return true;
}

//! @brief Handles SDL key down events and forwards to the keyboard controller.
bool SessionRunningState::onKeyDown(uintptr_t context, SDL_Event *event)
{
    auto *self = reinterpret_cast<SessionRunningState *>(context);
    Arm::IKeyboardController *keyboard = self->_keyboard;

    if (keyboard != nullptr)
        keyboard->keyDown(static_cast<uint32_t>(event->key.scancode));

    return true;
}

//! @brief Handles SDL key up events and forwards to the keyboard controller.
bool SessionRunningState::onKeyUp(uintptr_t context, SDL_Event *event)
{
    auto *self = reinterpret_cast<SessionRunningState *>(context);
    Arm::IKeyboardController *keyboard = self->_keyboard;

    if (keyboard != nullptr)
        keyboard->keyUp(static_cast<uint32_t>(event->key.scancode));

    return true;
}

//! @brief Handles SDL mouse motion events.
bool SessionRunningState::onMouseMotion(uintptr_t context, SDL_Event *event)
{
    auto *self = reinterpret_cast<SessionRunningState *>(context);
    Arm::IKeyboardController *keyboard = self->_keyboard;

    if (keyboard != nullptr)
        keyboard->mouseDelta(static_cast<int32_t>(event->motion.xrel),
                             static_cast<int32_t>(event->motion.yrel));


    return true;
}

//! @brief Handles SDL mouse button events.
bool SessionRunningState::onMouseButton(uintptr_t context, SDL_Event *event)
{
    auto *self = reinterpret_cast<SessionRunningState *>(context);
    Arm::IKeyboardController *keyboard = self->_keyboard;

    if (keyboard != nullptr)
    {
        auto button = static_cast<Arm::IKeyboardController::MouseButton>(
            event->button.button);

        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            keyboard->mouseButtonDown(button);
        else
            keyboard->mouseButtonUp(button);
    }

    return true;
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

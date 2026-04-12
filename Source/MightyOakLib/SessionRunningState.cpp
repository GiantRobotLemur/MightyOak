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
        case Mo::Arm::AcornPixelFormat::Palettised8BppVIDC10:
            // NOTE: The VIDC10 16-colour palette that combines with the pixel
            // to create a colour value has been resolved into a 256-colour palette.
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
    _frameProvider(nullptr)
{
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
    auto eventProcessor = std::make_unique<Ag::SDL3::EventProcessor>();

    // Register input event handlers.
    Ag::SDL3::EventHandlers &dispatcher = eventProcessor->getEventDispatcher();
    uintptr_t ctx = reinterpret_cast<uintptr_t>(this);

    dispatcher.registerEventHandler(SDL_EVENT_KEY_DOWN, onKeyDown, ctx);
    dispatcher.registerEventHandler(SDL_EVENT_KEY_UP, onKeyUp, ctx);
    dispatcher.registerEventHandler(SDL_EVENT_MOUSE_MOTION, onMouseMotion, ctx);
    dispatcher.registerEventHandler(SDL_EVENT_MOUSE_BUTTON_DOWN, onMouseButton, ctx);
    dispatcher.registerEventHandler(SDL_EVENT_MOUSE_BUTTON_UP, onMouseButton, ctx);
    dispatcher.registerCustomEventHandler(getContext()->getGuestEventMessageId(),
                                          onGuestEvent, ctx);

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

//! @brief Processes a captured video frame received from the emulated system.
//! @param[in] frameId The numeric identifier of the sampled frame.
void SessionRunningState::onFrameReceived(uint32_t frameId)
{
    if (_frameProvider == nullptr)
        return;

    auto frame = _frameProvider->getSampledFrame(frameId);

    if (frame == nullptr)
        return;

    static constexpr Arm::FrameDiffBits IsGeomChange = Arm::FrameDiff_FrameSize |
                                                       Arm::FrameDiff_DisplayFormat;

    auto &config = frame->getGeometry();
    auto diff = frame->getDifferenceFromLastFrame();

    if (diff == Arm::FrameDiff_None)
    {
        // Exploit frame coherence - nothing has changed.
        return;
    }
    else if (diff & IsGeomChange)
    {
        // The output video configuration has changed.
        onFrameConfigChanged(config);
    }

    if (frame->hasDisplay())
    {
        // Update the window contents with the captured frame.
        // Expand raw indexed pixels to ARGB32 using the palette.
        auto &palette = frame->getDisplayPalette();
        auto &format = Arm::getAcornPixelFormatInfo().getSymbolById(config.getDisplayFormat());

        if (format.isHybridPalette())
        {
            // Resolve a 256-colour palette which can use each pixel as a look-up
            // from a 16-colour palette which uses some of the pixel data to provide
            // a base colour.
            for (uint32_t i = 0; i < 256; ++i)
            {
                const Arm::CanonicalColour &paletteEntry = palette[i & 0x0F];

                _palette[i] = paletteEntry.combineVIDC10LogicalColour(static_cast<uint8_t>(i)).RawValue;
            }
        }
        else if (format.isPalettised())
        {
            for (uint16_t i = 0; i < format.getPaletteSize(); ++i)
            {
                _palette[i] = palette[i].RawValue;
            }
        }

        expandToARGB32(frame->getDisplayData().data(), _palette, _argb32Buffer.data(),
                       config.getDisplayWidth(), config.getDisplayHeight(),
                       config.getDisplayFormat(), frame->getGeometry().getBytesPerRow());

        // Upload the ARGB32 buffer to the SDL texture.
        SDL_UpdateTexture(_texture, nullptr, _argb32Buffer.data(),
                          static_cast<int>(config.getDisplayWidth() * sizeof(uint32_t)));
    }

    // Clear and render.
    if (frame->getGeometry().hasBorder())
    {
        // Use border colour as clear colour.
        auto &borderColour = frame->getBorderPalette().front();

        // TODO: Refactor to allow palette changes during a frame, for the border
        // and the display frame, too.
        SDL_SetRenderDrawColor(_renderer, borderColour.getRed(), borderColour.getGreen(), borderColour.getBlue(), 255);
    }
    else
    {
        // Clear to black.
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    }

    SDL_RenderClear(_renderer);

    if (config.hasDisplay())
    {
        // Render the frame buffer texture scaled to fill the window while
        // maintaining the aspect ratio.
        SDL_RenderTexture(_renderer, _texture, nullptr, nullptr);
    }

    SDL_RenderPresent(_renderer);
}

void SessionRunningState::onFrameConfigChanged(const Arm::FrameGeometry &newGeometry)
{
    // Dispose of previous video resources.
    if (_texture != nullptr)
        SDL_DestroyTexture(_texture);

    // Create new video resources.
    _texture = SDL_CreateTexture(_renderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 static_cast<int>(newGeometry.getDisplayWidth()),
                                 static_cast<int>(newGeometry.getDisplayHeight()));

    size_t bufferSize = static_cast<size_t>(newGeometry.getDisplayWidth()) *
        static_cast<size_t>(newGeometry.getDisplayHeight()) * 4;

    _argb32Buffer.resize(bufferSize);

    _currentFrameGeometry = newGeometry;
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

//! @brief Handles events from the running guest system.
bool SessionRunningState::onGuestEvent(uintptr_t context, SDL_Event *event)
{
    auto *self = reinterpret_cast<SessionRunningState *>(context);
    Arm::GuestEvent guestEvent;

    if (self->getContext()->isGuestEvent(event, guestEvent))
    {
        if (guestEvent.Type == Arm::HostMessageID::FrameBufferReady)
        {
            self->onFrameReceived(static_cast<uint32_t>(guestEvent.Data1));
        }
    }

    return true;
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

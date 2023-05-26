//! @file ObjectGL/SDL2Driver.cpp
//! @brief The definition of an implementation of an OpenGL driver which uses
//! libSDL2.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <string>

#include <SDL2/SDL.h>

#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/GLAPI/BaseTypes.hpp"
#include "Ag/ObjectGL/DisplayFormat.hpp"
#include "Ag/ObjectGL/ContextOptions.hpp"
#include "Ag/ObjectGL/SDL2Driver.hpp"

#include "DisplayContextPrivate.hpp"
#include "RenderContextPrivate.hpp"

namespace Ag {
namespace GL {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
class SDL2DisplayContext;

//! @brief An exception thrown when an SDL function returns an error.
class SDL2Exception : public Ag::Exception
{
public:
    static const char *Domain;

    SDL2Exception(const char *fnName, int errorCode = 0)
    {
        std::string message, detail;
        message.assign("The SDL2 function '");
        message.append(fnName);
        message.append("' returned an unexpected error condition.");

        initialise(Domain, message, SDL_GetError(),
                   static_cast<uintptr_t>(errorCode));
    }

    SDL2Exception(const char *fnName, const std::string &detail)
    {
        std::string message;
        message.assign("The SDL2 function '");
        message.append(fnName);
        message.append("' returned an unexpected error condition.");

        initialise(Domain, message, detail, 0);
    }
};

const char *SDL2Exception::Domain = "SDL2Exception";

//! @brief A structure representing the values which identify an OpenGL context
//! according to libSDL2.
struct GLContext
{
    // Public Fields
    SDL_Window *Window;
    SDL_GLContext Renderer;

    // Construction/Destruction
    GLContext() :
        Window(nullptr),
        Renderer(nullptr)
    {
    }

    GLContext(SDL_Window *window, SDL_GLContext renderer) :
        Window(window),
        Renderer(renderer)
    {
    }

    // Accessors
    //! @brief Determines if the object is populated with a valid renderer context.
    bool isValid() const
    {
        return (Window != nullptr) && (Renderer != nullptr);
    }

    // Operations
    //! @brief Populates the object with details of the OpenGL context selected
    //! on the current thread.
    //! @retval true A valid context was set on the current thread.
    //! @retval false No context was selected on the current thread.
    bool getCurrent()
    {
        Window = SDL_GL_GetCurrentWindow();
        Renderer = SDL_GL_GetCurrentContext();

        return (Window != nullptr) && (Renderer != nullptr);
    }

    //! @brief Selects the current OpenGL context.
    void makeCurrent() const
    {
        int result = SDL_GL_MakeCurrent(Window, Renderer);

        if (result != 0)
        {
            throw SDL2Exception("SDL_GL_MakeContext()", result);
        }
    }

    //! @brief De-selects any OpenGL context on the current thread.
    void doneCurrent() const
    {
        int result = SDL_GL_MakeCurrent(Window, nullptr);

        if (result != 0)
        {
            throw SDL2Exception("SDL_GL_MakeContext(null)", result);
        }
    }

    //! @brief Swaps the front and back buffers on the associated window.
    void swapBuffers() const
    {
        SDL_GL_SwapWindow(Window);
    }

    //! @brief Disposes of the render context, unselecting it if necessary.
    void dispose()
    {
        if (Renderer != nullptr)
        {
            if (Renderer == SDL_GL_GetCurrentContext())
            {
                doneCurrent();
            }

            SDL_GL_DeleteContext(Renderer);
        }

        Renderer = nullptr;
        Window = nullptr;
    }
};

//! @brief An object which uses RAII to capture and restore the selected OpenGL
//! context for the current thread.
struct ContextScope
{
private:
    GLContext _oldContext;

public:
    ContextScope()
    {
        _oldContext.getCurrent();
    }

    ~ContextScope()
    {
        _oldContext.makeCurrent();
    }
};

//! @brief Resolves OpenGL entry points for use with the SDL2 library.
class SDL2PrivateResolver : public gl::APIResolver
{
private:
    // Internal Fields
    gl::APIResolver::GetNextErrorFn _getError;

public:
    // Construction/Destruction
    SDL2PrivateResolver() :
        _getError(nullptr)
    {
    }

    virtual ~SDL2PrivateResolver() = default;

    // Operations
    void initialise()
    {
        _getError = reinterpret_cast<gl::APIResolver::GetNextErrorFn>(SDL_GL_GetProcAddress("glGetError"));
    }

    // Overrides
    // Inherited from APIResolver.
    virtual bool isExtensionPresent(const char *extName) const override
    {
        return SDL_GL_ExtensionSupported(extName) != SDL_FALSE;
    }

    // Inherited from APIResolver.
    virtual void *resolveEntryPoint(const char *fnName) const override
    {
        return SDL_GL_GetProcAddress(fnName);
    }

    // Inherited from APIResolver.
    virtual gl::APIResolver::GetNextErrorFn getErrorReporterProc() const override
    {
        return _getError;
    }
};

//! @brief An implementation of RenderContextPrivate which uses SDL2 as its
//! underlying platform.
class SDL2RenderContext : public RenderContextPrivate
{
public:
    // Construction/Destruction
    SDL2RenderContext(const DisplayContextPrivateSPtr &display, const GLContext &context);
    virtual ~SDL2RenderContext();

    // Operations

    // Overrides
    virtual void makeCurrent() override;
    virtual void doneCurrent() override;
    virtual void swapBuffers() override;
private:
    // Internal Fields
    GLContext _context;
};

//! @brief An implementation of DisplayContextPrivate which uses SDL2 as its
//! underlying platform.
class SDL2DisplayContext : public DisplayContextPrivate
{
public:
    // Construction/Destruction
    SDL2DisplayContext(const DisplayFormat &format);
    virtual ~SDL2DisplayContext();

    // Accessors
    GLContext &getRootContext() { return _rootContext; }

    // Operations
    void ensureRootContextCreated(const ContextOptions &options,
                                  bool ignoreVersion);

    // Overrides
    // Inherited from DisplayContextPrivate.
    virtual const gl::APIResolver *getResolver() const override;

    // Inherited from DisplayContextPrivate.
    virtual std::shared_ptr<RenderContextPrivate> createContext(uintptr_t drawable,
                                                                const ContextOptions &options) override;

    static void setContextAttributes(const ContextOptions &options,
                                     const DisplayFormat &format,
                                     bool ignoreVersion);
private:
    // Internal Fields
    DisplayFormat _format;
    SDL2PrivateResolver _resolver;
    GLContext _rootContext;
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
template<typename T, typename U>
void setSDLProperty(T id, SDL_GLattr attrib, const U &format)
{
    int value;

    if (format.tryGetTypedProperty(id, value))
    {
        SDL_GL_SetAttribute(attrib, value);
    }
}

template<typename T, typename U>
void setSDLFlag(T id, SDL_GLattr attrib, const U &format)
{
    uint32_t value;

    if (format.tryGetProperty(id, value))
    {
        SDL_GL_SetAttribute(attrib, (value == 0) ? SDL_TRUE : SDL_FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////////
// SDL2DisplayContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing a shared set of OpenGL contexts.
//! @param[in] format The preferred output format.
SDL2DisplayContext::SDL2DisplayContext(const DisplayFormat &format) :
    _format(format)
{
}

//! @brief Ensures that any root GL context and window is disposed of.
SDL2DisplayContext::~SDL2DisplayContext()
{
    // Get a copy of the root context hidden window before it gets blanked.
    SDL_Window *rootWindow = _rootContext.Window;

    // De-select and delete the OpenGL context.
    _rootContext.dispose();

    // Dispose of the window.
    if (rootWindow != nullptr)
    {
        SDL_DestroyWindow(rootWindow);
    }
}

void SDL2DisplayContext::ensureRootContextCreated(const ContextOptions &options,
                                                  bool ignoreVersion)
{
    if (_rootContext.isValid() == false)
    {
        // Create a root context to be shared by the one we return.
        int x = SDL_WINDOWPOS_UNDEFINED;
        int y = SDL_WINDOWPOS_UNDEFINED;
        int width = 256;
        int height = 256;

        // Create a hidden window.
        _rootContext.Window = SDL_CreateWindow("Hidden Window", x, y, width, height,
                                               SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);

        if (_rootContext.Window == nullptr)
        {
            throw SDL2Exception("SDL_CreateWindow(SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL)");
        }

        // Create a root OpenGL context from which resources will be shared.
        setContextAttributes(options, _format, ignoreVersion);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, SDL_FALSE);

        _rootContext.Renderer = SDL_GL_CreateContext(_rootContext.Window);

        if (_rootContext.Renderer == nullptr)
        {
            // Cache the message for the exception to throw after calling
            // subsequent SDL functions.
            std::string message(SDL_GetError());

            SDL_DestroyWindow(_rootContext.Window);
            _rootContext.Window = nullptr;

            throw SDL2Exception("SDL_GL_CreateContext(rootContext)", message);
        }

        // Make the context current.
        _rootContext.makeCurrent();

        // Initialise the resolver.
        _resolver.initialise();

        // Initialise the underlying API.
        initialiseAPI();
    }
}

// Inherited from DisplayContextPrivate.
const gl::APIResolver *SDL2DisplayContext::getResolver() const
{
    return &_resolver;
}

// Inherited from DisplayContextPrivate.
std::shared_ptr<RenderContextPrivate> SDL2DisplayContext::createContext(uintptr_t drawable,
                                                                        const ContextOptions &options)
{
    // The drawable parameter should be a pointer to a valid SDL_Window structure.
    if (drawable == 0)
    {
        throw Ag::ArgumentException("drawable");
    }

    SDL_Window *drawableWindow = reinterpret_cast<SDL_Window *>(drawable);

    ContextScope contextState;

    ensureRootContextCreated(options, true);

    _rootContext.makeCurrent();

    // Ensure the newly created context can share resources with the
    // root context.
    setContextAttributes(options, _format, false);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, SDL_TRUE);

    GLContext context(drawableWindow, SDL_GL_CreateContext(drawableWindow));

    if (context.Renderer == nullptr)
    {
        throw SDL2Exception("SDL_GL_CreateContext(renderContext)", 0);
    }

    // De-select the root context from the current thread.
    _rootContext.doneCurrent();

    return std::make_shared<SDL2RenderContext>(shared_from_this(), context);
}

//! @brief Maps preferred format and context to SDL GL attributes.
//! @param[in] options The set of requirements for the context to create.
//! @param[in] ignoreVersion True to not set GL major/minor version requirements
//! in order to get the highest possible version.
void SDL2DisplayContext::setContextAttributes(const ContextOptions &options,
                                              const DisplayFormat &format,
                                              bool ignoreVersion)
{
    // Reset any previously set attributes.
    SDL_GL_ResetAttributes();

    // Set the render format properties.
    setSDLProperty(DisplayPropertyID::RedBitCount, SDL_GL_RED_SIZE, format);
    setSDLProperty(DisplayPropertyID::GreenBitCount, SDL_GL_GREEN_SIZE, format);
    setSDLProperty(DisplayPropertyID::BlueBitCount, SDL_GL_BLUE_SIZE, format);
    setSDLProperty(DisplayPropertyID::AlphaBitCount, SDL_GL_ALPHA_SIZE, format);
    setSDLProperty(DisplayPropertyID::ColourBitCount, SDL_GL_BUFFER_SIZE, format);
    setSDLProperty(DisplayPropertyID::DepthBitCount, SDL_GL_DEPTH_SIZE, format);
    setSDLProperty(DisplayPropertyID::StencilBitCount, SDL_GL_STENCIL_SIZE, format);
    setSDLFlag(DisplayPropertyID::HasDoubleBuffer, SDL_GL_DOUBLEBUFFER, format);
    setSDLFlag(DisplayPropertyID::HasHardwareAcceleration, SDL_GL_ACCELERATED_VISUAL, format);

    // Set context requirements.
    if (ignoreVersion == false)
    {
        setSDLProperty(ContextPropertyID::MajorVersion, SDL_GL_CONTEXT_MAJOR_VERSION, options);
        setSDLProperty(ContextPropertyID::MinorVersion, SDL_GL_CONTEXT_MINOR_VERSION, options);
    }

    uint32_t contextFlags = 0;

    contextFlags |= options.getProperty(ContextPropertyID::EnableDebugging,
                                        false) ? SDL_GL_CONTEXT_DEBUG_FLAG :
                                                 0;

    contextFlags |= options.getProperty(ContextPropertyID::UseForwardCompatiblity,
                                        false) ? SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG :
                                                 0;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, static_cast<int>(contextFlags));

    uint32_t profileFlags = 0;

    profileFlags |= options.getProperty(ContextPropertyID::UseCompatabilityProfile,
                                        false) ? SDL_GL_CONTEXT_PROFILE_COMPATIBILITY :
                                                 0;

    profileFlags |= options.getProperty(ContextPropertyID::UseCoreProfile,
                                        false) ? SDL_GL_CONTEXT_PROFILE_CORE :
                                                 0;

    profileFlags |= options.getProperty(ContextPropertyID::UseESProfile,
                                        false) ? SDL_GL_CONTEXT_PROFILE_ES :
                                                 0;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, static_cast<int>(profileFlags));
}

////////////////////////////////////////////////////////////////////////////////
// SDL2RenderContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
SDL2RenderContext::SDL2RenderContext(const DisplayContextPrivateSPtr &display,
                                     const GLContext &context) :
    RenderContextPrivate(display),
    _context(context)
{
    _context.makeCurrent();
    getAPIInternal().resolve(display->getResolver());
    _context.doneCurrent();
}

//! @brief Disposes of the OpenGL context the object wrapped.
SDL2RenderContext::~SDL2RenderContext()
{
    _context.dispose();
}

// Inherited from RenderContextPrivate.
void SDL2RenderContext::makeCurrent()
{
    _context.makeCurrent();
}

// Inherited from RenderContextPrivate.
void SDL2RenderContext::doneCurrent()
{
    _context.doneCurrent();
}

// Inherited from RenderContextPrivate.
void SDL2RenderContext::swapBuffers()
{
    _context.swapBuffers();
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// SDL2Driver Member Definitions
////////////////////////////////////////////////////////////////////////////////
Version SDL2Driver::getMaxSupportedVersion(const ContextOptions &options,
                                           const DisplayFormat &format) const
{
    auto display = std::make_shared<SDL2DisplayContext>(format);
    display->ensureRootContextCreated(options, true);

    return display->getMaxSupportedVersion();
}

DisplayContext SDL2Driver::createDisplayDevice(const DisplayFormat &format) const
{
    std::shared_ptr<SDL2DisplayContext> context = std::make_shared<SDL2DisplayContext>(format);

    return AssignableDisplayContext(context);
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


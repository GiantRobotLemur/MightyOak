//! @file ObjectGL/WGLDriver.cpp
//! @brief The definition of an object which uses direct calls to the WGL API
//! to create an OpenGL context.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
//! @author Nick Arkell
//! @copyright (c) 2022 Nick Arkell : Software Engineer
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
// Must be included before GLAPI headers.
#include "Ag/ObjectGL/Win32API.hpp"

#include "DisplayContextPrivate.hpp"
#include "RenderContextPrivate.hpp"

#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Utils.hpp"

#include "Ag/GLAPI/APIResolvers.hpp"
#include "Ag/GLAPI/WGL_CommandSets.hpp"
#include "Ag/ObjectGL/ContextOptions.hpp"
#include "Ag/ObjectGL/DisplayFormat.hpp"
#include "Ag/ObjectGL/WGLDriver.hpp"

namespace Ag {
namespace GL {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which ensures that a window, associated device context
//! an OpenGL context are all handled with an exception safe guarantee.
struct WindowResource
{
    // Public Data
    HWND Window;
    HDC Device;
    HGLRC Context;
    BOOL(WINAPI *MakeCurrent)(HDC, HGLRC);
    BOOL(WINAPI *DeleteContext)(HGLRC);
    bool _disposeOfWindow;

    // Construction/Destruction
    //! @brief Constructs a object to manage the lifetime of resources with a
    //! guarantee of exception safety.
    //! @param[in] window A handle to a window to destroy on destruction.
    //! @param[in] disposeOfWindow Indicates whether the window should be disposed
    //! of at destruction.
    WindowResource(HWND window, bool disposeOfWindow) :
        Window(window),
        Device(nullptr),
        Context(nullptr),
        MakeCurrent(nullptr),
        DeleteContext(nullptr),
        _disposeOfWindow(disposeOfWindow)
    {
    }

    //! @brief Disposes of all resources held by the object.
    ~WindowResource()
    {
        releaseContext();
        releaseDC();

        if (_disposeOfWindow && (::IsWindow(Window) != FALSE))
        {
            ::DestroyWindow(Window);
        }

        Window = nullptr;
    }

    // Operations
    //! @brief Gets a device context from the window, if not already obtained.
    HDC getDC()
    {
        if (Device == nullptr)
        {
            Device = ::GetDC(Window);
        }

        return Device;
    }

    //! @brief Disposes of the device context, if it exists.
    void releaseDC()
    {

        if (Device != nullptr)
        {
            // Ensure the OpenGL render context is disposed of first.
            releaseContext();

            ::ReleaseDC(Window, Device);
            Device = nullptr;
        }
    }

    //! @brief De-selects and disposes of the OpenGL context, if it exists.
    void releaseContext()
    {
        if (Context != nullptr)
        {
            if (MakeCurrent != nullptr)
            {
                MakeCurrent(Device, nullptr);
            }

            if (DeleteContext != nullptr)
            {
                DeleteContext(Context);
            }

            Context = nullptr;
        }
    }
};

//! @brief An object representing a WGL-specific OpenGL context.
class WGLContext
{
public:
    // Construction
    WGLContext(const wgl::WGL &api);
    ~WGLContext();

    // Accessors
    bool hasContext() const;
    HWND getWindow() const;
    HDC getDevice() const;
    HGLRC getContext() const;

    // Operations
    void initialise(HWND window, HDC device, HGLRC context);
    void initialise(WindowResource &resource);
    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void dispose();

private:
    // Internal Fields
    const wgl::WGL &_API;
    HWND _window;
    HDC _device;
    HGLRC _context;
};

//! @brief An implementation of DisplayContextPrivate which uses the WGL API to
//! create an manage OpenGL contexts.
class WGLDisplayContext : public DisplayContextPrivate
{
public:
    // Construction/Destruction
    WGLDisplayContext(const DisplayFormat &format);
    virtual ~WGLDisplayContext();

    // Accessors
    const wgl::WGL &getWGLAPI() const;

    // Overrides
    const gl::APIResolver *getResolver() const override;
    virtual RenderContextPrivateSPtr createContext(uintptr_t drawable,
                                                   const ContextOptions &options) override;
private:
    // Internal Functions
    static LRESULT CALLBACK rootWindowProc(HWND handle, UINT id,
                                           WPARAM wParam, LPARAM lParam);
    void createRootContext(HWND original, const ContextOptions &options);

    // Internal Fields
    wgl::WGLResolver _resolver;
    wgl::WGL _wglAPI;
    wgl::ARBCreateContext _createContext;
    wgl::ARBPixelFormat _pixelFormat;
    DisplayFormat _format;
    WGLContext _rootContext;
    int _pixelFormatID;
};

//! @brief An implementation of RenderContextPrivate which uses Win32-specific
//! APIs.
class WGLRenderContext : public RenderContextPrivate
{
public:
    // Construction/Destruction
    WGLRenderContext(const DisplayContextPrivateSPtr &display,
                     const WGLContext &context);
    virtual ~WGLRenderContext();

    // Accessors

    // Overrides
    virtual void makeCurrent() override;
    virtual void doneCurrent() override;
    virtual void swapBuffers() override;
private:
    // Internal Fields
    WGLContext _context;
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief A call back function for a temporary window which does nothing.
//! @param[in] handle The handle of the window to process messages for.
//! @param[in] id The identifier of the message to process.
//! @param[in] wParam The word message parameter.
//! @param[in] lParam The long message parameter.
//! @return The result of processing the message.
LRESULT CALLBACK primitiveGLWinProc(HWND handle, UINT id,
                                    WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProcW(handle, id, wParam, lParam);
}

//! @brief Creates a temporary window which is not displayed and does nothing.
//! @param[in] displayArea The are the window should cover in order to select the
//! correct display device.
//! @returns A handle to the window, which it is up to the caller to destroy.
HWND createPrimitiveWindow(const RECT &displayArea)
{
    static ATOM windowClassId = 0;

    if (windowClassId == 0)
    {
        WNDCLASSEXW wc;
        Ag::zeroFill(wc);
        wc.cbSize = sizeof(wc);
        wc.lpszClassName = L"LegacyOpenGLWindow";
        wc.hInstance = ::GetModuleHandleW(nullptr);
        wc.lpfnWndProc = primitiveGLWinProc;
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

        windowClassId = ::RegisterClassExW(&wc);

        if (windowClassId == 0)
        {
            throw Ag::Win32Exception("RegisterClassEx(PrimitiveOpenGLWindow)",
                                     ::GetLastError());
        }
    }

    const DWORD Style = WS_OVERLAPPEDWINDOW;
    const DWORD StyleEx = WS_EX_OVERLAPPEDWINDOW;

    HWND handle = ::CreateWindowExW(StyleEx,
                                    reinterpret_cast<LPCWSTR>(windowClassId),
                                    nullptr, Style,
                                    displayArea.left, displayArea.top,
                                    displayArea.right - displayArea.left,
                                    displayArea.bottom - displayArea.top,
                                    ::GetDesktopWindow(), nullptr,
                                    ::GetModuleHandleW(nullptr), nullptr);

    if (handle == nullptr)
    {
        throw Ag::Win32Exception("CreateWindowEx(PrimitiveOpenGLWindow)",
                                 ::GetLastError());
    }

    return handle;
}

//! @brief Creates a legacy OpenGL context which can be used to resolve entry
//! points for creation of a more modern context.
//! @param[in,out] resource An object to hold the window, device context and
//! OpenGL context in order that they be properly disposed of.
//! @param[in] api The WGL API exported by OpenGL32.dll.
//! @param[in] format The preferred format to render to.
//! @return The created legacy context, also embedded in the resource object.
HGLRC createLegacyOpenGLContext(WindowResource &resource, const wgl::WGL &api,
                                const DisplayFormat &format)
{
    PIXELFORMATDESCRIPTOR formatInfo;
    Ag::zeroFill(formatInfo);
    formatInfo.nSize = sizeof(formatInfo);
    formatInfo.nVersion = 1;
    formatInfo.iPixelType = PFD_TYPE_RGBA;
    formatInfo.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    formatInfo.iLayerType = PFD_MAIN_PLANE;

    if (format.tryGetTypedProperty(DisplayPropertyID::DepthBitCount,
                                   formatInfo.cDepthBits) == false)
    {
        formatInfo.dwFlags |= PFD_DEPTH_DONTCARE;
    }

    formatInfo.dwFlags |= format.getProperty(DisplayPropertyID::HasHardwareAcceleration,
                                             true) ? PFD_GENERIC_ACCELERATED : 0;

    bool useDoubleBuffer = false;

    if (format.tryGetTypedProperty(DisplayPropertyID::HasDoubleBuffer, useDoubleBuffer))
    {
        if (useDoubleBuffer)
        {
            formatInfo.dwFlags |= PFD_DOUBLEBUFFER;
        }
    }
    else
    {
        formatInfo.dwFlags |= PFD_DOUBLEBUFFER_DONTCARE;
    }

    // Note: The following work because we know that the formatInfo fields start
    // off as zero.
    formatInfo.cColorBits = format.getTypedValue(DisplayPropertyID::ColourBitCount, formatInfo.cColorBits);

    // Not required for ChoosePixelFormat();
    //formatInfo.cRedBits = format.getTypedValue(DisplayPropertyID::ColourBitCount, formatInfo.cRedBits);
    //formatInfo.cGreenBits = format.getTypedValue(DisplayPropertyID::ColourBitCount, formatInfo.cGreenBits);
    //formatInfo.cBlueBits = format.getTypedValue(DisplayPropertyID::ColourBitCount, formatInfo.cBlueBits);
    formatInfo.cAlphaBits = format.getTypedValue(DisplayPropertyID::ColourBitCount, formatInfo.cAlphaBits);
    formatInfo.cDepthBits = format.getTypedValue(DisplayPropertyID::DepthBitCount, formatInfo.cDepthBits);
    formatInfo.cStencilBits = format.getTypedValue(DisplayPropertyID::DepthBitCount, formatInfo.cStencilBits);

    int formatId = api.ChoosePixelFormat(resource.getDC(), &formatInfo);

    if (formatId == 0)
    {
        throw Ag::Win32Exception("ChoosePixelFormat()", ::GetLastError());
    }

    PIXELFORMATDESCRIPTOR chosenFormat;
    Ag::zeroFill(chosenFormat);
    chosenFormat.nSize = sizeof(chosenFormat);

    if (api.DescribePixelFormat(resource.Device, formatId, sizeof(chosenFormat), &chosenFormat) == 0)
    {
        throw Ag::Win32Exception("DescribePixelFormat()", ::GetLastError());
    }

    if (api.SetPixelFormat(resource.Device, formatId, &chosenFormat) == FALSE)
    {
        throw Ag::Win32Exception("SetPixelFormat()", ::GetLastError());
    }

    resource.Context = api.wglCreateContext(resource.Device);

    if (resource.Context == nullptr)
    {
        throw Ag::Win32Exception("CreateContext()", ::GetLastError());
    }

    return resource.Context;
}

//! @brief Maps generic display format properties to WGL-specific pixel format
//! attributes.
//! @param[in] format The set of generic properties to map.
//! @param[out] intProps The collection to encode the property values in.
void mapPixelFormatProperties(const DisplayFormat &format,
                              std::vector<int> &intProps)
{
    intProps.clear();
    intProps.reserve(32);

    intProps.push_back(wgl::Global::DrawToWindow);
    intProps.push_back(TRUE);
    intProps.push_back(wgl::Global::SupportOpenGL);
    intProps.push_back(TRUE);
    intProps.push_back(wgl::Global::PixelType);
    intProps.push_back(wgl::Global::TypeRGBA);

    bool flag = false;
    if (format.tryGetTypedProperty(DisplayPropertyID::HasHardwareAcceleration, flag))
    {
        intProps.push_back(wgl::Global::Acceleration);
        intProps.push_back(flag ? wgl::Global::FullAcceleration : wgl::Global::NoAcceleration);
    }

    if (format.tryGetTypedProperty(DisplayPropertyID::HasDoubleBuffer, flag))
    {
        intProps.push_back(wgl::Global::DoubleBuffer);
        intProps.push_back(flag ? TRUE : FALSE);
    }

    int value = -1;
    if (format.tryGetTypedProperty(DisplayPropertyID::RedBitCount, value))
    {
        intProps.push_back(wgl::Global::RedBits);
        intProps.push_back(value);
    }

    if (format.tryGetTypedProperty(DisplayPropertyID::GreenBitCount, value))
    {
        intProps.push_back(wgl::Global::GreenBits);
        intProps.push_back(value);
    }

    if (format.tryGetTypedProperty(DisplayPropertyID::BlueBitCount, value))
    {
        intProps.push_back(wgl::Global::BlueBits);
        intProps.push_back(value);
    }

    if (format.tryGetTypedProperty(DisplayPropertyID::AlphaBitCount, value))
    {
        intProps.push_back(wgl::Global::AlphaBits);
        intProps.push_back(value);
    }


    if (format.tryGetTypedProperty(DisplayPropertyID::ColourBitCount, value))
    {
        intProps.push_back(wgl::Global::ColorBits);
        intProps.push_back(value);
    }

    if (format.tryGetTypedProperty(DisplayPropertyID::DepthBitCount, value))
    {
        intProps.push_back(wgl::Global::DepthBits);
        intProps.push_back(value);
    }

    if (format.tryGetTypedProperty(DisplayPropertyID::StencilBitCount, value))
    {
        intProps.push_back(wgl::Global::StencilBits);
        intProps.push_back(value);
    }

    // Terminate the attribute list.
    intProps.push_back(0);
}

//! @brief Maps generic context requirement properties to a set of WGL-specific
//! attributes.
//! @param[in] options The set of context requirements to map.
//! @param[in] noVersion True to ignore any OpenGL version requirement properties.
//! @param[out] intProps The collection to encode the property values in.
void mapContextProperties(const ContextOptions &options, bool noVersion,
                          std::vector<int> &intProps)
{
    intProps.clear();
    intProps.reserve(8);
    int value = 0;

    if (noVersion == false)
    {
        if (options.tryGetTypedProperty(ContextPropertyID::MajorVersion, value))
        {
            intProps.push_back(wgl::Global::ContextMajorVersion);
            intProps.push_back(value);
        }

        if (options.tryGetTypedProperty(ContextPropertyID::MinorVersion, value))
        {
            intProps.push_back(wgl::Global::ContextMinorVersion);
            intProps.push_back(value);
        }
    }

    bool hasFlags = false;
    bool hasProfile = false;
    bool flag = false;

    GLuint flags = 0;
    GLuint profile = 0;

    if (options.tryGetTypedProperty(ContextPropertyID::UseCoreProfile, flag))
    {
        hasProfile = true;
        profile |= flag ? wgl::Global::ContextCoreProfile : 0;
    }

    if (options.tryGetTypedProperty(ContextPropertyID::UseCompatabilityProfile, flag))
    {
        hasProfile = true;
        profile |= flag ? wgl::Global::ContextCompatibilityProfile : 0;
    }

    if (options.tryGetTypedProperty(ContextPropertyID::UseESProfile, flag))
    {
        hasProfile = true;
        profile |= flag ? wgl::Global::ContextES2Profile : 0;
    }

    if (options.tryGetTypedProperty(ContextPropertyID::UseForwardCompatiblity, flag))
    {
        hasFlags = true;
        flags |= flag ? wgl::Global::ContextForwardCompatible : 0;
    }

    if (options.tryGetTypedProperty(ContextPropertyID::EnableDebugging, flag))
    {
        hasFlags = true;
        flags |= flag ? wgl::Global::ContextDebug : 0;
    }

    if (hasFlags)
    {
        intProps.push_back(wgl::Global::ContextFlags);
        intProps.push_back(flags);
    }

    if (hasProfile)
    {
        intProps.push_back(wgl::Global::ContextProfileMask);
        intProps.push_back(profile);
    }

    // Terminate the list.
    intProps.push_back(0);
}

} // Anonymous namespace


////////////////////////////////////////////////////////////////////////////////
// WGLContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which can manipulate an OpenGL context created
//! using the WGL API.
//! @param[in] api A reference to the resolved API used to manipulate the context.
WGLContext::WGLContext(const wgl::WGL &api) :
    _API(api),
    _window(nullptr),
    _device(nullptr),
    _context(nullptr)
{
}

//! @brief Does NOT dispose of the inner context.
WGLContext::~WGLContext()
{
    _context = nullptr;
    _device = nullptr;
    _window = nullptr;
}

//! @brief Determines if the object contains a valid OpenGL context.
bool WGLContext::hasContext() const { return _context != nullptr; }

//! @brief Gets the window originally associated with the context.
HWND WGLContext::getWindow() const { return _window; }

//! @brief Gets the context of the device being rendered to.
HDC WGLContext::getDevice() const { return _device; }

//! @brief Gets a handle to the OpenGL render context.
HGLRC WGLContext::getContext() const { return _context; }

//! @brief Initialises the object with a live OpenGL context.
//! @param[in] window A handle to the window being rendered to.
//! @param[in] device The context of the device which renders to the window.
//! @param[in] context The OpenGL context associated with the device.
void WGLContext::initialise(HWND window, HDC device, HGLRC context)
{
    if (::IsWindow(window))
    {
        _window = window;
        _device = device;
        _context = context;
    }
    else
    {
        _window = nullptr;
        _device = nullptr;
        _context = nullptr;
    }
}

//! @brief Initialises the object by transferring a live OpenGL context.
//! @param[in] resource The object containing the window/DC/context to
//! acquire. It will be reset to an empty state if resources are acquired.
void WGLContext::initialise(WindowResource &resource)
{
    if (::IsWindow(resource.Window))
    {
        _window = resource.Window;
        _device = resource.Device;
        _context = resource.Context;

        resource.Context = nullptr;
        resource.Device = nullptr;
        resource.Window = nullptr;
        resource._disposeOfWindow = false;
    }
    else
    {
        _window = nullptr;
        _device = nullptr;
        _context = nullptr;
    }
}

//! @brief Selects the context for use on the current thread.
void WGLContext::makeCurrent()
{
    if (_API.wglMakeCurrent(_device, _context) == FALSE)
    {
        throw Ag::Win32Exception("wglMakeCurrent(OpenGL context)",
                                 ::GetLastError());
    }
}

//! @brief De-selects any OpenGL context on the current thread.
void WGLContext::doneCurrent()
{
    if (_API.wglMakeCurrent(nullptr, nullptr) == FALSE)
    {
        throw Ag::Win32Exception("wglMakeCurrent(null)",
                                 ::GetLastError());
    }
}

//! @brief Swaps front and back buffers on the default frame buffer.
void WGLContext::swapBuffers()
{
    if (_API.SwapBuffers(_device) == FALSE)
    {
        throw Ag::Win32Exception("SwapBuffers()", ::GetLastError());
    }
}

//! @brief Disposes of the render context the object wraps.
void WGLContext::dispose()
{
    if (_context != nullptr)
    {
        if (_API.wglGetCurrentContext() == _context)
        {
            // Ensure the context isn't current in this thread.
            _API.wglMakeCurrent(nullptr, nullptr);
        }

        _API.wglDeleteContext(_context);
        _context = nullptr;
        _device = nullptr;
        _window = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// WGLDisplayContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing a display device on which to
//! create OpenGL contexts.
//! @param[in] format The output format to render to.
WGLDisplayContext::WGLDisplayContext(const DisplayFormat &format) :
    _resolver(true),
    _format(format),
    _rootContext(_wglAPI),
    _pixelFormatID(-1)
{
    // Resolve the entry points exported directly from OpenGL32.dll.
    _wglAPI.resolveEntryPoints(&_resolver);
}

//! @brief Disposes of the root context, if one was created.
WGLDisplayContext::~WGLDisplayContext()
{
    if (_rootContext.hasContext())
    {
        _rootContext.dispose();
    }
}

//! @brief Gets the Win32-specific API used for manipulating OpenGL contexts.
const wgl::WGL &WGLDisplayContext::getWGLAPI() const
{
    return _wglAPI;
}

// Inherited from DisplayContextPrivate.
const gl::APIResolver *WGLDisplayContext::getResolver() const
{
    return &_resolver;
}

// Inherited from DisplayContextPrivate.
RenderContextPrivateSPtr WGLDisplayContext::createContext(uintptr_t drawable,
                                                          const ContextOptions &options)
{
    WindowResource target(reinterpret_cast<HWND>(drawable), false);

    if (::IsWindow(target.Window) == FALSE)
    {
        throw Ag::ArgumentException("drawable");
    }

    if ((::GetClassLongW(target.Window, GCL_STYLE) & CS_OWNDC) == 0)
    {
        throw Ag::OperationException("Cannot create an OpenGL context on a window "
                                     "which doesn't have the CS_OWNDC style.");
    }

    if (_rootContext.hasContext() == false)
    {
        createRootContext(target.Window, options);
    }

    PIXELFORMATDESCRIPTOR format;
    Ag::zeroFill(format);
    format.nSize = sizeof(format);
    format.nVersion = 1;

    if (_wglAPI.DescribePixelFormat(target.getDC(), _pixelFormatID,
                                    sizeof(format), &format) == 0)
    {
        throw Ag::Win32Exception("DescribePixelFormat()", ::GetLastError());
    }

    if (_wglAPI.SetPixelFormat(target.Device, _pixelFormatID, &format) == FALSE)
    {
        throw Ag::Win32Exception("SetPixelFormat()", ::GetLastError());
    }

    // Create a context which shares resources with the root context.
    std::vector<int> intProps;
    mapContextProperties(options, false, intProps);
    target.Context = _createContext.wglCreateContextAttribs(target.Device,
                                                            _rootContext.getContext(),
                                                            intProps.data());

    if (target.Context == nullptr)
    {
        throw Ag::Win32Exception("wglCreateContextAttribsARB()",
                                 ::GetLastError());
    }

    WGLContext renderContext(_wglAPI);
    renderContext.initialise(target);

    return std::make_shared<WGLRenderContext>(shared_from_this(), renderContext);
}

//! @brief Processes messages sent to the root context window.
//! @param[in] handle The handle of the window.
//! @param[in] id The identifier of the message, such as WM_CREATE.
//! @param[in] wParam The word message parameter.
//! @param[in] lParam The long message parameter.
//! @return The result of processing the message.
LRESULT CALLBACK WGLDisplayContext::rootWindowProc(HWND handle, UINT id,
                                                   WPARAM wParam, LPARAM lParam)
{
    WGLDisplayContext *instance = nullptr;

    if (id == WM_CREATE)
    {
        // Associate a display context with the window.
        LPCREATESTRUCTW cs = reinterpret_cast<LPCREATESTRUCTW>(lParam);

        if (cs != nullptr)
        {
            ::SetWindowLongPtrW(handle, GWLP_USERDATA,
                                reinterpret_cast<LONG_PTR>(cs->lpCreateParams));

            instance = static_cast<WGLDisplayContext *>(cs->lpCreateParams);
        }
    }

    if (instance != nullptr)
    {
        // TODO: Process top level messages.
    }

    LRESULT result = ::DefWindowProcW(handle, id, wParam, lParam);

    if (id == WM_DESTROY)
    {
        // Disassociate the window from the display context.
        ::SetWindowLongPtrW(handle, GWLP_USERDATA, 0);
    }

    return result;
}

//! @brief Creates a hidden window associated with the context which can process
//! top-level system messages and be used for OpenGL context creation.
//! @param[in] original The window used copy certain properties.
//! @param[in] options The options intended to create an OpenGL context.
//! @return A handle to the newly created window.
void WGLDisplayContext::createRootContext(HWND original, const ContextOptions &options)
{
    RECT outputArea;
    ::GetWindowRect(original, &outputArea);

    // Create a legacy OpenGL context on a dummy window so that we can use
    // wglGetProcAddress() to get the interface to create a more modern context.
    WindowResource primitiveWindow(createPrimitiveWindow(outputArea), true);

    // Add the entry points required to select and dispose of an OpenGL context.
    primitiveWindow.MakeCurrent = _wglAPI.wglMakeCurrent;
    primitiveWindow.DeleteContext = _wglAPI.wglDeleteContext;
    createLegacyOpenGLContext(primitiveWindow, _wglAPI, _format);

    WGLContext legacyContext(_wglAPI);
    legacyContext.initialise(primitiveWindow);

    // Select the legacy context and use it to resolve entry points.
    legacyContext.makeCurrent();
    _resolver.setDevice(legacyContext.getDevice());

    if ((_resolver.isExtensionPresent("WGL_ARB_create_context") == false) ||
        (_resolver.isExtensionPresent("WGL_ARB_pixel_format") == false))
    {
        throw Ag::NotSupportedException("Creating modern OpenGL contexts on the selected display");
    }

    _createContext.resolveEntryPoints(&_resolver);
    _pixelFormat.resolveEntryPoints(&_resolver);

    DisplayContextPrivate::initialiseAPI();

    // De-select and dispose of the legacy context so that we can create a
    // modern context.
    legacyContext.doneCurrent();
    legacyContext.dispose();

    // Create a window associated with this context to host a modern OpenGL context.
    static ATOM classId = 0;

    if (classId == 0)
    {
        WNDCLASSEXW wc;
        Ag::zeroFill(wc);
        wc.cbSize = sizeof(wc);
        wc.hInstance = ::GetModuleHandleW(nullptr);
        wc.style = CS_OWNDC;
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = L"RootContextWindow";
        wc.lpfnWndProc = rootWindowProc;

        classId = ::RegisterClassExW(&wc);

        if (classId == 0)
        {
            throw Ag::Win32Exception("RegisterClassEx(RootContextClass)",
                                     ::GetLastError());
        }
    }

    const DWORD Style = WS_OVERLAPPED;
    const DWORD StyleEx = WS_EX_OVERLAPPEDWINDOW;

    HWND rootWindow = ::CreateWindowExW(StyleEx, reinterpret_cast<LPWSTR>(classId),
                                        L"ObjectGL Root WGL Window", Style,
                                        outputArea.left, outputArea.top,
                                        outputArea.right - outputArea.left,
                                        outputArea.bottom - outputArea.top,
                                        ::GetDesktopWindow(), nullptr,
                                        ::GetModuleHandleW(nullptr), this);

    if (rootWindow == nullptr)
    {
        throw Ag::Win32Exception("CreateWindowEx(Root WGL Window)",
                                 ::GetLastError());
    }

    std::vector<int> intProps;

    mapPixelFormatProperties(_format, intProps);

    // Ensure the window and DC are destroyed if we throw an exception.
    WindowResource root(rootWindow, true);

    int formatId = -1;
    UINT formatCount = 0;

    if (_pixelFormat.wglChoosePixelFormat(root.getDC(), intProps.data(), nullptr,
                                          1, &formatId, &formatCount) == FALSE)
    {
        throw Ag::Win32Exception("wglChoosePixelFormatARB()", ::GetLastError());
    }
    else if (formatCount < 1)
    {
        throw Ag::OperationException("No matching pixel formats found.");
    }

    // Get full details of the pixel format chosen.
    PIXELFORMATDESCRIPTOR formatInfo;
    Ag::zeroFill(formatInfo);
    formatInfo.nSize = sizeof(formatInfo);
    formatInfo.nVersion = 1;

    if (_wglAPI.DescribePixelFormat(root.Device, formatId,
                                    sizeof(formatInfo), &formatInfo) == 0)
    {
        throw Ag::Win32Exception("DescribPixelFormat()", ::GetLastError());
    }

    // Set the pixel format on our root window/DC.
    if (_wglAPI.SetPixelFormat(root.getDC(), formatId, &formatInfo) == FALSE)
    {
        throw Ag::Win32Exception("SetPixelFormat()", ::GetLastError());
    }

    // Create a context for the root window.
    mapContextProperties(options, true, intProps);
    root.Context = _createContext.wglCreateContextAttribs(root.Device, nullptr,
                                                          intProps.data());

    if (root.Context == nullptr)
    {
        throw Ag::Win32Exception("wglCreateContextARB()", ::GetLastError());
    }

    // If we get this far, we have succeeded, we don't want to dispose of
    // anything, so transfer the window/DC/Context to a different object and
    // prevent their automatic disposal when they go out of scope.
    _rootContext.initialise(root);

    // Select the new context.
    _rootContext.makeCurrent();

    // Update all the entry points using a modern context.
    _pixelFormatID = formatId;
    _resolver.setDevice(_rootContext.getDevice());
    _createContext.resolveEntryPoints(&_resolver);
    _pixelFormat.resolveEntryPoints(&_resolver);

    // De-select the context again.
    _rootContext.doneCurrent();
}

////////////////////////////////////////////////////////////////////////////////
// WGLRenderContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to wrap a Win32-specific OpenGL context.
//! @param[in] display The display which owns the renderer.
//! @param[in] context The OpenGL context to wrap.
WGLRenderContext::WGLRenderContext(const DisplayContextPrivateSPtr &display,
                                   const WGLContext &context) :
    RenderContextPrivate(display),
    _context(context)
{
    if (!display)
    {
        throw Ag::ArgumentException("display");
    }

    if (context.hasContext() == false)
    {
        throw Ag::ArgumentException("context");
    }

    makeCurrent();
    getAPIInternal().resolve(display->getResolver());
    doneCurrent();
}

//! @brief Ensures the OpenGL context is properly disposed of.
WGLRenderContext::~WGLRenderContext()
{
    _context.dispose();
}

// Inherited from RenderContextPrivate.
void WGLRenderContext::makeCurrent()
{
    _context.makeCurrent();
}

// Inherited from RenderContextPrivate.
void WGLRenderContext::doneCurrent()
{
    _context.doneCurrent();
}

// Inherited from RenderContextPrivate.
void WGLRenderContext::swapBuffers()
{
    _context.swapBuffers();
}

////////////////////////////////////////////////////////////////////////////////
// WGLDriver Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates an object which can create OpenGL contexts to render to a
//! specific output format.
DisplayContext WGLDriver::createDisplayDevice(const DisplayFormat &format) const
{
    std::shared_ptr<WGLDisplayContext> context = std::make_shared<WGLDisplayContext>(format);

    return AssignableDisplayContext(context);
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


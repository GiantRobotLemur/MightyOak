//! @file ObjectGL/WGL_Tools.cpp
//! @brief The definition of functions to assist using OpenGL on Windows.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"
#include "Ag/ObjectGL/WGL_Tools.hpp"

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
    BOOL (WINAPI *MakeCurrent)(HDC, HGLRC);
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
        wc.lpszClassName = L"PrimitiveOpenGLWindow";
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

void applyFlag(FormatProperty boolPropId, DWORD flag,
               const PixelFormatIntAttribSet &properties, DWORD &flags)
{
    if (properties.getValue(boolPropId, 0) != 0)
    {
        flags |= flag;
    }
}

void applyTriState(FormatProperty boolPropId, DWORD flag, DWORD undefined,
                   const PixelFormatIntAttribSet &properties,
                   DWORD &flags)
{
    int prop = 0;

    if (properties.tryGetValue(boolPropId, prop))
    {
        if (prop != 0)
        {
            flags |= flag;
        }
    }
    else
    {
        flags |= undefined;
    }
}

//! @brief Attempts to map a much of a set of pixel format properties to the
//! legacy PIXELFORMATDESCRIPTOR data structure as possible.
void populateLegacyPixelFormat(PIXELFORMATDESCRIPTOR &format,
                               const PixelFormatIntAttribSet &properties)
{
    Ag::zeroFill(format);
    int prop = 0;

    format.nSize = sizeof(format);
    format.nVersion = 1;

    applyFlag(FormatProperty::DrawToWindow, PFD_DRAW_TO_WINDOW, properties, format.dwFlags);
    applyFlag(FormatProperty::DrawToBitmap, PFD_DRAW_TO_BITMAP, properties, format.dwFlags);
    applyFlag(FormatProperty::SupportGDI, PFD_SUPPORT_GDI, properties, format.dwFlags);
    applyFlag(FormatProperty::SupportOpenGL, PFD_SUPPORT_OPENGL, properties, format.dwFlags);

    applyTriState(FormatProperty::DoubleBuffer, PFD_DOUBLEBUFFER, PFD_DOUBLEBUFFER_DONTCARE,
                  properties, format.dwFlags);
    applyTriState(FormatProperty::Stereo, PFD_STEREO, PFD_STEREO_DONTCARE,
                  properties, format.dwFlags);

    prop = properties.getValue(FormatProperty::Acceleration,
                               wgl::Global::FullAcceleration);

    switch (prop)
    {
    case wgl::Global::NoAcceleration:
        format.dwFlags |= PFD_GENERIC_FORMAT;
        break;

    case wgl::Global::GenericAcceleration:
        format.dwFlags |= PFD_GENERIC_ACCELERATED;
        break;

    case wgl::Global::FullAcceleration:
        break;
    }

    bool needsPalette = properties.getValue(FormatProperty::NeedsPalette, 0) != 0;
    bool needsSystemPalette = properties.getValue(FormatProperty::NeedsSystemPalette, 0) != 0;
    bool useTransparency = properties.getValue(FormatProperty::Transparent, 0) != 0;

    if (properties.tryGetValue(FormatProperty::PixelType, prop))
    {
        switch (prop)
        {
        case wgl::Global::TypeColorindex:
            format.iPixelType = PFD_TYPE_COLORINDEX;
            break;

        case wgl::Global::TypeRGBA:
        default:
            format.iPixelType = PFD_TYPE_RGBA;
            break;
        }
    }
    else if (needsPalette || needsSystemPalette)
    {
        format.iPixelType = PFD_TYPE_COLORINDEX;
    }

    if (format.iPixelType == PFD_TYPE_COLORINDEX)
    {
        format.iPixelType = PFD_TYPE_COLORINDEX;

        if (useTransparency)
        {
            // Set the transparency colour key for over/underlays.
            format.dwVisibleMask =
                properties.getValue(FormatProperty::TransparentIndexValue, 0);
        }
    }
    else
    {
        format.iPixelType = PFD_TYPE_RGBA;

        if (useTransparency)
        {
            format.dwVisibleMask = RGB(properties.getValue(FormatProperty::TransparentRedValue, 0),
                                       properties.getValue(FormatProperty::TransparentGreenValue, 0),
                                       properties.getValue(FormatProperty::TransparentBlueValue, 0));
        }
    }

    if (properties.tryGetValue(FormatProperty::SwapMethod, prop))
    {
        switch (prop)
        {
        case wgl::Global::SwapExchange:
            format.dwFlags |= PFD_SWAP_EXCHANGE;
            break;

        case wgl::Global::SwapCopy:
            format.dwFlags |= PFD_SWAP_COPY;
            break;

        case wgl::Global::SwapUndefined:
            break;
        }
    }

    const BYTE empty = 0;
    format.cColorBits = properties.getTypedValue(FormatProperty::ColorBits, empty);
    format.cRedBits = properties.getTypedValue(FormatProperty::RedBits, empty);
    format.cRedShift = properties.getTypedValue(FormatProperty::RedShift, empty);
    format.cGreenBits = properties.getTypedValue(FormatProperty::GreenBits, empty);
    format.cGreenShift = properties.getTypedValue(FormatProperty::GreenShift, empty);
    format.cBlueBits = properties.getTypedValue(FormatProperty::BlueBits, empty);
    format.cBlueShift = properties.getTypedValue(FormatProperty::BlueShift, empty);
    format.cAlphaBits = properties.getTypedValue(FormatProperty::AlphaBits, empty);
    format.cAlphaShift = properties.getTypedValue(FormatProperty::AlphaShift, empty);
    format.cDepthBits = properties.getTypedValue(FormatProperty::DepthBits,
                                                 static_cast<BYTE>(PFD_DEPTH_DONTCARE));

    format.cStencilBits = properties.getTypedValue(FormatProperty::StencilBits, empty);
    format.cAccumBits = properties.getTypedValue(FormatProperty::AccumBits, empty);
    format.cAccumRedBits = properties.getTypedValue(FormatProperty::AccumRedBits, empty);
    format.cAccumGreenBits = properties.getTypedValue(FormatProperty::AccumGreenBits, empty);
    format.cAccumBlueBits = properties.getTypedValue(FormatProperty::AccumBlueBits, empty);
    format.cAccumAlphaBits = properties.getTypedValue(FormatProperty::AccumAlphaBits, empty);
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// WGL_Tools Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object useful for creating a modern OpenGL context.
WGL_Tools::WGL_Tools() :
    _resolver(true),
    _isPrepared(false)
{
    // Resolve the core WGL entry points from OpenGL32.dll only.
    _wgl.resolve(&_resolver);
}

//! @brief Gets an object which can resolve OpenGL entry points, but only for
//! the last device specified by the last call to prepare().
const gl::APIResolver &WGL_Tools::getResolver() const
{
    return _resolver;
}

//! @brief Gets the core WGL entry points, but only if a previous call to
//! prepare() was successful.
const wgl::WGLAPI &WGL_Tools::getWGL_API() const
{
    return _wgl;
}

//! @brief Gets the modern OpenGL context creation entry points, but only if
//! a previous call to prepare() was successful.
const wgl::ARBCreateContextAPI &WGL_Tools::getContextCreationAPI() const
{
    return _createContext;
}

//! @brief Gets the modern OpenGL pixel format entry points, but only if
//! a previous call to prepare() was successful.
const wgl::ARBPixelFormatAPI &WGL_Tools::getPixelFormatAPI() const
{
    return _pixelFormat;
}

//! @brief Gets the modern OpenGL GPU association entry points, but only if
//! a previous call to prepare() was successful.
const wgl::AMDGPUAssociationAPI &WGL_Tools::getGPUAssocAPI() const
{
    return _gpuAssociation;
}

//! @brief Resolves entry points and the set of supported extensions
//! using an existing OpenGL context.
//! @param[in] device A handle to a display device compatible with the context.
//! @param[in] context A handle to an existing OpenGL context.
void WGL_Tools::prepare(HDC device, HGLRC context)
{
    if (context == nullptr)
    {
        throw Ag::ArgumentException("context");
    }

    HGLRC oldContext = _wgl.getCurrentContext();
    HDC oldDevice = _wgl.getCurrentDC();

    if (_wgl.makeCurrent(device, context))
    {
        // Use the current context to extract useful information.
        _resolver.setDevice(device);
        _createContext.resolve(&_resolver);
        _pixelFormat.resolve(&_resolver);
        _gpuAssociation.resolve(&_resolver);
        _isPrepared = true;

        // Restore the previous context.
        _wgl.makeCurrent(oldDevice, oldContext);
    }
    else
    {
        throw Ag::Win32Exception("wglMakeCurrent()", ::GetLastError());
    }
}

//! @brief Resolves entry points and the set of supported extensions
//! using an existing OpenGL context.
//! @param[in] window A handle to a window used to create the OpenGL context.
//! @param[in] context A handle to an existing OpenGL context.
void WGL_Tools::prepare(HWND window, HGLRC context)
{
    // Ensure the DC is handled in an exception safe way.
    WindowResource resource(window, /* disposeOfWindow = */ false);

    prepare(resource.getDC(), context);
}

//! @brief Prepares the object for creation of OpenGL contexts on a
//! specific device.
//! @param[in] displayWindow A handle to a window created on the desired
//! display device on which SetPixelFormat() has not specifically been called.
//! @param[in] preferredFormat The preferred pixel format used to create
//! an initial OpenGL context to extract metadata from.
//! @throw Win32Exception If an error occurs attempting to create a a
//! temporary OpenGL context to interrogate.
void WGL_Tools::prepare(HWND displayWindow,
                        const PIXELFORMATDESCRIPTOR &preferredFormat)
{
    _isPrepared = false;

    // Determine which monitor the window is displayed on.
    HMONITOR display = nullptr;

    if (::IsWindow(displayWindow))
    {
        display = ::MonitorFromWindow(displayWindow,
                                      MONITOR_DEFAULTTOPRIMARY);
    }
    else
    {
        display = ::MonitorFromWindow(::GetDesktopWindow(),
                                      MONITOR_DEFAULTTOPRIMARY);
    }

    // Get the monitor outline so that we can create another window
    // on that monitor in order to create a primitive OpenGL context.
    MONITORINFOEXW monitorInfo;
    Ag::zeroFill(monitorInfo);
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (::GetMonitorInfoW(display, &monitorInfo) == FALSE)
    {
        throw Ag::Win32Exception("GetMonitorInfo()", ::GetLastError());
    }
    else
    {
        // Create a window on the display which is hidden and does nothing.
        WindowResource resources(createPrimitiveWindow(monitorInfo.rcWork),
                                 /* disposeOfWindow */ true);

        // Get a device context for the window an select a specific pixel
        // format based on the preferred settings.
        int formatId = ::ChoosePixelFormat(resources.getDC(), &preferredFormat);

        if (formatId == 0)
        {
            // Capture the error code before calling more API functions.
            throw Ag::Win32Exception("ChoosePixelFormat()", ::GetLastError());
        }
        else
        {
            // Get a full description of the selected pixel format.
            PIXELFORMATDESCRIPTOR fullDescription;
            Ag::zeroFill(fullDescription);

            int maxFormat = ::DescribePixelFormat(resources.Device, formatId,
                                                  sizeof(fullDescription),
                                                  &fullDescription);

            if (maxFormat <= 0)
            {
                throw Ag::Win32Exception("DescribePixelFormat()",
                                         ::GetLastError());
            }
            else
            {
                // Set the pixel format on the device context, something we can
                // do only once, hence we do it with a temporary window and DC.
                if (::SetPixelFormat(resources.Device, formatId,
                                     &fullDescription) == FALSE)
                {
                    throw Ag::Win32Exception("SetPixelFormat()", ::GetLastError());
                }
                else
                {
                    // Create a legacy OpenGL render context on the device.
                    _wgl.resolve(&_resolver);
                    resources.MakeCurrent = reinterpret_cast<decltype(resources.MakeCurrent)>(_resolver.resolveEntryPoint("wglMakeCurrent"));
                    resources.DeleteContext = reinterpret_cast<decltype(resources.DeleteContext)>(_resolver.resolveEntryPoint("wglDeleteContext"));

                    resources.Context = _wgl.createContext(resources.Device);

                    if (resources.Context == nullptr)
                    {
                        throw Ag::Win32Exception("wglCreateContext()",
                                                 ::GetLastError());
                    }
                    else
                    {
                        // Resolve entry points and the set of
                        // supported extensions.
                        prepare(resources.Device, resources.Context);

                        // De-select and dispose of the render context.
                        resources.releaseContext();
                    }
                }
            }
        }

        // Dispose of the temporary device context.
        resources.releaseDC();
    }
}

//! @brief Attempts to create a modern OpenGL context for a specified window.
//! @param[in] window The window which must have a class of CS_OWNDC and should
//! not have been used to create a previous OpenGL context.
//! @param[in] intFormatProps The integer properties of the pixel format to select
//! for rendering to the window.
//! @param[in] floatFormatProps The real properties of the pixel format to select
//! for rendering to the window.
//! @param[in] contextProps The properties of the context to create.
//! @note The function will end up calling SetPixelFormat(GetWindow(window)), this
//! can only be done once.
//! @throw Ag::Win32Exception If one of the platform API functions fails.
HGLRC WGL_Tools::createInitialContext(HWND window,
                                      const PixelFormatIntAttribSet &intFormatProps,
                                      const PixelFormatFloatAttribSet &floatFormatProps,
                                      const ContextAttribSet &contextProps)
{
    HGLRC context = nullptr;

    // Ensure the DC is handled in an exception safe way.
    WindowResource resource(window, /* disposeOfWindow = */ false);

    if (::GetPixelFormat(resource.getDC()) != 0)
    {
        throw Ag::OperationException("The pixel format of the window rendering "
                                     "device has already been set.");
    }

    // Create a legacy PIXELFORMATDESCRIPTOR in order to create a dummy context
    // that we can use to get useful entry points.
    PIXELFORMATDESCRIPTOR legacyPixelFormat;

    populateLegacyPixelFormat(legacyPixelFormat, intFormatProps);

    // Use the mapped properties to create a dummy context in order to resolve
    // entry points used to create a real context.
    prepare(window, legacyPixelFormat);

    if (_pixelFormat.isPresent() && _createContext.isPresent())
    {
        auto intFormatParams = intFormatProps.getValues();
        auto floatFormatParams = floatFormatProps.getValues();
        int preferredFormat = 0;
        UINT formatCount = 0;

        if (_pixelFormat.choosePixelFormat(resource.getDC(),
                                           intFormatParams.data(),
                                           floatFormatParams.data(),
                                           1, &preferredFormat,
                                           &formatCount) == FALSE)
        {
            throw Ag::Win32Exception("wglChoosePixelFormatARB()",
                                     ::GetLastError());
        }
        else if (formatCount == 0)
        {
            // No matching pixel formats were found.
            throw Ag::NotSupportedException("An OpenGL render target with the specified format");
        }

        PIXELFORMATDESCRIPTOR finalFormat;
        Ag::zeroFill(finalFormat);

        int maxPixelFormat = ::DescribePixelFormat(resource.Device, preferredFormat,
                                                   sizeof(finalFormat), &finalFormat);

        if (maxPixelFormat == 0)
        {
            throw Ag::Win32Exception("DescribePixelFormat()", ::GetLastError());
        }

        if (::SetPixelFormat(resource.Device, preferredFormat,
                             &finalFormat) == FALSE)
        {
            throw Ag::Win32Exception("SetPixelFormat()", ::GetLastError());
        }

        auto contextParams = contextProps.getValues();
        context = _createContext.createContextAttribs(resource.Device,
                                                      nullptr, contextParams.data());

        if (context == nullptr)
        {
            throw Ag::Win32Exception("wglCreateContextAttribsARB()",
                                     ::GetLastError());
        }
        else
        {
            // Use the new context to resolve various entry points.
            prepare(resource.Device, context);
        }
    }

    return context;
}

//! @brief Can be called to create further OpenGL contexts using the display
//! device associated with a specific window which was previously used in
//! in a call to createInitialContext().
//! @param[in] window The handle of the window which already has a selected
//! rendering format.
//! @param[in] shareContext The optional handle of a context to share resource with.
//! @param[in] contextProps The requirements of the context to create.
//! @return A handle to the newly created context.
//! @throw Ag::Win32Exception If one of the platform API functions fails.
HGLRC WGL_Tools::createSharedContext(HWND window, HGLRC shareContext,
                                     const ContextAttribSet &contextProps)
{
    if (_createContext.isPresent())
    {
        throw Ag::OperationException("The APIs required to create a modern "
                                     "OpenGL context have not yet been resolved.");
    }

    // Ensure the DC is handled in an exception safe way.
    WindowResource resource(window, /* disposeOfWindow = */ false);

    if (::GetPixelFormat(resource.getDC()) == 0)
    {
        throw Ag::OperationException("The pixel format of the window rendering "
                                     "device has not been previously set.");
    }

    auto attribs = contextProps.getValues();

    HGLRC context = _createContext.createContextAttribs(resource.Device,
                                                        shareContext,
                                                        attribs.data());

    if (context == nullptr)
    {
        throw Ag::Win32Exception("wglCreateContextAttribsARB()",
                                 ::GetLastError());
    }

    return context;
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


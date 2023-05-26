//! @file GLAPI/APIResolvers.cpp
//! @brief The definition of various implementations of APIResolver required
//! by the generated OpenGL API.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/GLAPI/APIResolvers.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Utils.hpp"

#ifdef _WIN32
#include "Ag/GLAPI/WGL_API.hpp"
#endif

#ifdef _WIN32
namespace wgl {
////////////////////////////////////////////////////////////////////////////////
// WGLResolver Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to resolve OpenGL functions in the current
//! context.
//! @param[in] loadOpenGL True to call LoadLibrary() if OpenGL32.dll is not
//! already loaded into the current process. False to only attempt to access
//! OpenGL32.dll if it is already loaded.
//! @note Once loaded (using loadOpenGL = true), the DLL will not be unloaded
//! at object destruction.
WGLResolver::WGLResolver(bool loadOpenGL /* = false */) :
    _gdi32Dll(nullptr),
    _openGL32Dll(nullptr),
    _glGetError(nullptr),
    _getProcAddr(nullptr)
{
    initialise(loadOpenGL);
}

//! @brief Updates basic entry point and the set of extensions available.
//! @param device The context of the device on which OpenGL operates.
void WGLResolver::setDevice(HDC device)
{
    typedef const char *(WINAPI *GetExtsStrFn)(HDC hdc);
    typedef const char *(WINAPI *GetStrFn)(GLenum name);
    typedef void (APIENTRY *GetIntFn)(GLenum pname, GLint *data);
    typedef const char *(WINAPI *GetStrItemFn)(GLenum name, GLuint index);

    _extensions.clear();
    _glGetError = nullptr;

    if ((device != nullptr) &&
        (::GetDeviceCaps(device, TECHNOLOGY) == DT_RASDISPLAY))
    {
        // Get the error reporter function.
        resolve("glGetError", _glGetError);

        GetExtsStrFn getExtensions;
        GetStrItemFn getStringI;
        GetIntFn getInt;
        GetStrFn getString;

        if (resolve("wglGetExtensionsStringARB", getExtensions))
        {
            // Use the Win32-approved method.
            const char *allExtensions = getExtensions(device);

            if (allExtensions != nullptr)
            {
                _extensions.reset(allExtensions);
            }
        }
        else if (resolve("glGetIntegerv", getInt) &&
                 resolve("glGetStringi", getStringI))
        {
            // Use the OpenGL 3.0+ mechanism.
            GLint count = 0;
            getInt(static_cast<GLenum>(gl::GetPName::NumExtensions), &count);

            _extensions.beginUpdate();

            for (GLint i = 0; i < count; ++i)
            {
                const char *extName =
                    getStringI(static_cast<GLenum>(gl::StringName::Extensions), i);

                _extensions.addExtension(extName);
            }

            _extensions.endUpdate();
        }
        else if (resolve("glGetString", getString))
        {
            // Use the legacy mechanism.
            const char *exts = getString(static_cast<GLenum>(gl::StringName::Extensions));

            if (exts != nullptr)
            {
                _extensions.reset(exts);
            }
        }
    }
}

// Inherited from APIResolver.
bool WGLResolver::isExtensionPresent(const char *extName) const
{
    return _extensions.containsExtension(extName);
}

// Inherited from APIResolver.
void *WGLResolver::resolveEntryPoint(const char *fnName) const
{
    void *fnPtr = nullptr;

    if (fnName != nullptr)
    {
        if (_getProcAddr != nullptr)
        {
            // Initially, try to look up the internal OpenGL entry point.
            fnPtr = _getProcAddr(fnName);
        }

        if ((fnPtr == nullptr) && (_openGL32Dll != nullptr))
        {
            // Try searching for the function in OpenGL32.dll.
            fnPtr = ::GetProcAddress(_openGL32Dll, fnName);
        }

        if ((fnPtr == nullptr) && (_gdi32Dll != nullptr))
        {
            // Try searching for the function in GDI32.dll.
            fnPtr = ::GetProcAddress(_gdi32Dll, fnName);
        }
    }

    return fnPtr;
}

// Inherited from APIResolver.
gl::APIResolver::GetNextErrorFn WGLResolver::getErrorReporterProc() const
{
    return _glGetError;
}

//! @brief Performs shared initialisation for various constructors.
//! @param[in] loadOpenGL True to load OpenGL32.dll into the process if it is
//! not already, false to only use it if already loaded.
void WGLResolver::initialise(bool loadOpenGL)
{
    if (::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                             L"GDI32.dll", &_gdi32Dll) == FALSE)
    {
        if (loadOpenGL)
        {
            _gdi32Dll = ::LoadLibraryW(L"GDI32.dll");

            if (_gdi32Dll == nullptr)
            {
                throw Ag::Win32Exception("LoadLibrary(GDI32)",
                                         ::GetLastError());
            }
        }
    }

    if (::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                             L"OpenGL32.dll", &_openGL32Dll) == FALSE)
    {
        if (loadOpenGL)
        {
            _openGL32Dll = ::LoadLibraryW(L"OpenGL32.dll");

            if (_openGL32Dll == nullptr)
            {
                throw Ag::Win32Exception("LoadLibrary(OpenGL32)",
                                         ::GetLastError());
            }
        }
    }

    if (_openGL32Dll != nullptr)
    {
        _getProcAddr = reinterpret_cast<GetProcAddrFn>(::GetProcAddress(_openGL32Dll,
                                                                        "wglGetProcAddress"));
    }
}

} // namespace wgl
#endif // ifdef _WIN32

////////////////////////////////////////////////////////////////////////////////


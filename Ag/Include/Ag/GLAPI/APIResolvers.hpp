//! @file Ag/GLAPI/APIResolvers.hpp
//! @brief The declaration of various implementations of ApiResolver required
//! by the generated OpenGL API.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_GLAPI_RESOLVERS_HPP__
#define __AG_GLAPI_RESOLVERS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "BaseTypes.hpp"
#include "EnumTypes.hpp"
#include "ExtensionSet.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
#ifdef SDL_video_h_
// This code will be enabled if SDL.h has been included, specifically SDL_video.h.

//! @brief Resolves OpenGL entry points for use with the SDL2 library.
class SDL2Resolver : public gl::APIResolver
{
public:
    // Construction/Destruction
    SDL2Resolver() = default;
    virtual ~SDL2Resolver() = default;

    // Overrides
    // Inherited from APIResolver.
    virtual bool isExtensionPresent(const char *extName) const
    {
        return SDL_GL_ExtensionSupported(extName) != SDL_FALSE;
    }

    // Inherited from APIResolver.
    virtual void *resolveEntryPoint(const char *fnName) const
    {
        return SDL_GL_GetProcAddress(fnName);
    }

    // Inherited from APIResolver.
    virtual gl::APIResolver::GetNextErrorFn getErrorReporterProc() const
    {
        return reinterpret_cast<gl::APIResolver::GetNextErrorFn>(SDL_GL_GetProcAddress("glGetError"));
    }
};
#endif // ifdef SDL_video_h_

#ifdef _WIN32
//! @brief A namespace encapsulating Win32-specific OpenGL APIs and tools.
namespace wgl {

//! @brief Resolves OpenGL entry points for use with a raw Win32 UI.
class WGLResolver : public gl::APIResolver
{
public:
    // Construction/Destruction
    WGLResolver(bool loadOpenGL = false);
    virtual ~WGLResolver() = default;

    // Operations
    void setDevice(HDC device);

    // Overrides
    virtual bool isExtensionPresent(const char *extName) const override;
    virtual void *resolveEntryPoint(const char *fnName) const override;
    virtual gl::APIResolver::GetNextErrorFn getErrorReporterProc() const override;
private:
    // Internal Types
    typedef PROC(WINAPI *GetProcAddrFn)(LPCSTR);

    // Internal Functions
    void initialise(bool loadOpenGL);

    template<typename T> bool resolve(const char *fnName, T &fnPtr) const
    {
        fnPtr = reinterpret_cast<T>(resolveEntryPoint(fnName));

        return fnPtr != nullptr;
    }

    // Internal Fields
    HMODULE _gdi32Dll;
    HMODULE _openGL32Dll;
    APIResolver::GetNextErrorFn _glGetError;
    GetProcAddrFn _getProcAddr;
    gl::ExtensionSet _extensions;
};

} // namespace wgl
#endif

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

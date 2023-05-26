//! @file SdlTools/OpenGL.cpp
//! @brief The definition of tools for operating with OpenGL on top of SDL.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>

#include "Ag/SdlTools/OpenGL.hpp"
#include "Ag/SdlTools/WrapperTypes.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif

namespace Ag {
namespace SDL {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct ApiInfoFunctions
{
    // Public Types
    using Enum = unsigned int;
    using Int = int;
    using Uint = unsigned int;

    // Public Constants
    static constexpr Enum GL_Vendor = 0x1F00;
    static constexpr Enum GL_Renderer = 0x1F01;
    static constexpr Enum GL_Version = 0x1F02;
    static constexpr Enum GL_Extensions = 0x1F03;

    static constexpr Enum GL_MajorVersion = 0x821B;
    static constexpr Enum GL_MinorVersion = 0x821C;
    static constexpr Enum GL_NumExtensions = 0x821D;

    // Public Fields
    const char *(APIENTRY *getString) (Enum name);
    void (APIENTRY *getInteger) (Enum pname, Int *data);
    const char *(APIENTRY *getStringI) (Enum name, Uint index);

    ApiInfoFunctions() :
        getString(reinterpret_cast<decltype(getString)>(SDL_GL_GetProcAddress("glGetString"))),
        getInteger(reinterpret_cast<decltype(getInteger)>(SDL_GL_GetProcAddress("glGetIntegerv"))),
        getStringI(reinterpret_cast<decltype(getStringI)>(SDL_GL_GetProcAddress("glGetStringi")))
    {
    }
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// OpenGLInfo Member Definitions
////////////////////////////////////////////////////////////////////////////////
OpenGLInfo::OpenGLInfo()
{
}

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool tryGetOpenGLCoreProfileInfo(OpenGLInfo &info)
{
    bool isOK = false;
    info.MaxVersion.clear();
    info.Vendor = String::Empty;
    info.Renderer = String::Empty;
    info.Extensions.clear();

    // Request the core profile.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create a hidden window from which to create an OpenGL context.
    WindowUPtr window(SDL_CreateWindow("OpenGL Core Profile Detection",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       640, 480,
                                       SDL_WINDOW_OPENGL |
                                       SDL_WINDOW_HIDDEN));

    if (window)
    {
        // Create an OpenGL context using that window.
        GLContextUPtr context(SDL_GL_CreateContext(window.get()));

        // Select the context on the current thread so we can interrogate it.
        if (context && (SDL_GL_MakeCurrent(window.get(), context.get()) == 0))
        {
            ApiInfoFunctions api;

            if (api.getString != nullptr)
            {
                info.Vendor = api.getString(ApiInfoFunctions::GL_Vendor);
                info.Renderer = api.getString(ApiInfoFunctions::GL_Renderer);

                std::string_view versionText = api.getString(ApiInfoFunctions::GL_Version);

                if (info.MaxVersion.tryParse(versionText))
                {
                    isOK = true;

                    if (info.MaxVersion >= Version(3) && (api.getStringI != nullptr))
                    {
                        // Harvest the available extensions using modern methods.
                        int count = 0;
                        api.getInteger(ApiInfoFunctions::GL_NumExtensions, &count);

                        info.Extensions.reserve(static_cast<size_t>(count));

                        for (int i = 0; i < count; ++i)
                        {
                            info.Extensions.emplace(api.getStringI(ApiInfoFunctions::GL_Extensions, i));
                        }
                    }
                }

                if (info.Extensions.empty())
                {
                    // Harvest the available extensions using the legacy method.
                    std::string_view extensions = api.getString(ApiInfoFunctions::GL_Extensions);
                    std::string buffer;

                    for (char next : extensions)
                    {
                        if (std::isspace(next))
                        {
                            if (buffer.empty() == false)
                            {
                                info.Extensions.emplace(buffer);
                                buffer.clear();
                            }
                        }
                        else
                        {
                            buffer.push_back(next);
                        }
                    }

                    if (buffer.empty() == false)
                    {
                        // Append the last extension.
                        info.Extensions.emplace(buffer);
                    }
                }
            }
        }

        // Unselect the context before destroying it and the window.
        SDL_GL_MakeCurrent(window.get(), nullptr);
    }

    return isOK;
}

}} // namespace Ag::SDL
////////////////////////////////////////////////////////////////////////////////


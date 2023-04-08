//! @file Win32API.cpp
//! @brief The definition of some helper functions which assist the user of
//! the Win32 API.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////

#include "Win32API.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Utf.hpp"

#include <cwchar>

#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to get the full path to a module file name.
//! @param[in] handle The handle of the module to obtain a file name for.
//! @param[out] buffer Receives the path in wide characters.
//! @throws Win32Exception If the module was invalid.
void getModuleFileName(uintptr_t handle, std::vector<wchar_t> &buffer)
{
    HMODULE hModule = reinterpret_cast<HMODULE>(handle);

    buffer.resize(256);

    // Attempt to get the module file name as Unicode text into the
    // existing buffer.
    size_t length = ::GetModuleFileNameW(hModule, buffer.data(),
                                         static_cast<DWORD>(buffer.size()));
    if (length > 0)
    {
        while (length == buffer.size())
        {
            // The buffer wasn't large enough. Resize and try again.
            buffer.resize(buffer.size() * 2, L'\0');

            length = ::GetModuleFileNameW(hModule, buffer.data(),
                                          static_cast<DWORD>(buffer.size()));
        }

        // Truncate the buffer to the required size.
        buffer.resize(length);
    }
    else
    {
        // The initial attempt failed, don't try again.
        throw Win32Exception("GetModuleFileName()", ::GetLastError());
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the absolute path to the currently running executable.
String getProgramFileName()
{
    std::vector<wchar_t> wideBuffer;

    getModuleFileName(static_cast<uintptr_t>(0), wideBuffer);

    return String(wideBuffer.data(), wideBuffer.size());
}

//! @brief Gets the path an file name of a module loaded into the current process.
//! @param[in] moduleHandle The handle of the loaded module.
//! @return The full path to the module and its file name.
String getModuleFileName(uintptr_t moduleHandle)
{
    std::vector<wchar_t> wideBuffer;

    getModuleFileName(moduleHandle, wideBuffer);

    return String(wideBuffer.data(), wideBuffer.size());
}

//! @brief Gets the path an directory containing a module loaded into the
//! current process.
//! @param[in] moduleHandle The handle of the loaded module.
//! @return The full path to the module minus its file name.
String getModuleDirectory(uintptr_t moduleHandle)
{
    std::vector<wchar_t> wideBuffer;

    getModuleFileName(moduleHandle, wideBuffer);

    size_t length;
    for (length = wideBuffer.size(); length > 0; --length)
    {
        wchar_t ch = wideBuffer[length - 1];

        if ((ch == L'\\') || (ch == '/'))
        {
            // We've found the last path element separator.
            --length;
            break;
        }
    }

    return String(wideBuffer.data(), length);
}

//! @brief Gets the full path of the current working directory of the
//! current process.
String getWorkingDirectory()
{
    uint32_t requiredSize = ::GetCurrentDirectoryW(0, nullptr);

    if (requiredSize > 0)
    {
        std::vector<wchar_t> buffer;
        buffer.resize(requiredSize);

        uint32_t charsWritten = ::GetCurrentDirectoryW(requiredSize, buffer.data());

        if (charsWritten > 0)
        {
            return String(buffer.data(), charsWritten);
        }
    }
    else
    {
        throw Win32Exception("GetCurrentDirectory()", ::GetLastError());
    }

    return String::Empty;
}

//! @brief Gets the profile directory of the current user.
String getHomeDirectory()
{
    wchar_t *profileDir = nullptr;
    size_t profileDirLength = 0;
    auto error = _wdupenv_s(&profileDir, &profileDirLength, L"USERPROFILE");

    String profilePath;

    if ((error == 0) && (profileDir != nullptr) && (profileDirLength > 0))
    {
        profilePath = String(profileDir, profileDirLength - 1);
    }

    if (profileDir != nullptr)
    {
        free(profileDir);
        profileDir = nullptr;
    }

    return profilePath;
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


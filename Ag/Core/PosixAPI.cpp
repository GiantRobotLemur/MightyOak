//! @file Core/PosixAPI.cpp
//! @brief The definition of some helper functions which assist the user of
//! the POSIX API.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>

#include <vector>

#include <unistd.h>

#include "PosixAPI.hpp"

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
std::vector<char> getProgramPath()
{
    // Read the path the symlink /proc/<pid>/exe points to.
    const size_t InBufferSize = 64;
    char inBuffer[InBufferSize];
    snprintf(inBuffer, InBufferSize, "/proc/%u/exe", getpid());
    std::vector<char> outBuffer;
    outBuffer.resize(256, '\0');

    ssize_t count = readlink(inBuffer, outBuffer.data(), outBuffer.size());

    if (count > 0)
    {
        while (toSize(count) == outBuffer.size())
        {
            // The buffer was too small, reallocate and try again.
            outBuffer.resize(outBuffer.size() * 2, '\0');

            count = readlink(inBuffer, outBuffer.data(), outBuffer.size());
        }

        // The string is not null terminated, but bounded by count.
        return String(outBuffer.data(), toSize(count));
    }
    else
    {
        return String::Empty;
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the absolute path to the currently running executable.
String getProgramFileName()
{
    std::vector<char> buffer = getProgramPath();

    if (buffer.empty())
    {
        return String::Empty;
    }
    else
    {
        return String(buffer.data(), buffer.size());
    }
}

//! @brief Gets the absolute path to the folder containing the currently
//! running executable.
String getProgramDirectory()
{
    std::vector<char> buffer = getProgramPath();

    if (buffer.empty())
    {
        return String::Empty;
    }
    else
    {
        auto pos = std::find(buffer.rbegin(), buffer.rend(), '/');
        size_t tail = static_cast<size_t>(std::distance(buffer.rbegin(), pos)) + 1;

        if (tail < buffer.size())
        {
            return String(buffer.data(), buffer.size() - tail);
        }
    }

    return String::Empty;
}

//! @brief Gets the full path of the current working directory of the
//! current process.
String getWorkingDirectory()
{
    struct Free
    {
        void operator()(char *block) const
        {
            free(block);
        }
    };

    std::unique_ptr<char, Free> cwd(getcwd(nullptr, 0));

    if (cwd)
    {
        return String(cwd.get());
    }
    else
    {
        return String("//", 1);
    }
}

//! @brief Gets the profile directory of the current user.
String getHomeDirectory()
{
    const char *homeDir = getenv("HOME");

    if (homeDir == nullptr)
    {
        // Return the file system root.
        return String("/", 1);
    }
    else
    {
        return String(homeDir);
    }
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


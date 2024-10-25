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

#include "Ag/Core/Utils.hpp"
#include "Platform.hpp"

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
String getProgramPath()
{
    // Read the path the symlink /proc/<pid>/exe points to.
    const char *procExePath = "/proc/self/exe";
    std::vector<char> outBuffer;
    outBuffer.resize(256, '\0');

    ssize_t count = readlink(procExePath, outBuffer.data(), outBuffer.size());

    if (count > 0)
    {
        while (toSize(count) == outBuffer.size())
        {
            // The buffer was too small, reallocate and try again.
            outBuffer.resize(outBuffer.size() * 2, '\0');

            count = readlink(procExePath, outBuffer.data(), outBuffer.size());
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
    return Ag::getProgramPath();
}

//! @brief Gets the absolute path to the folder containing the currently
//! running executable.
String getProgramDirectory()
{
    String buffer = getProgramPath();
    auto pos = buffer.reverseFind(U'/');

    if (pos == buffer.end())
    {
        return ".";
    }
    else
    {
        return String(buffer.getUtf8Bytes(), pos.getOffset());
    }
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
        return String("/", 1);
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

//! @brief Creates an array of program arguments from the command line of the
//! current process (read from /proc/self/cmdline).
//! @param[out] buffer Contains the data of the command line text partitioned into
//! null-termianted tokens.
//! @param[out] args Receives pointers to argument tokens in buffer.
void getProgramArgs(std::vector<char> &buffer, std::vector<utf8_cptr_t> &args)
{
     static const char *cmdLinePath = "/proc/self/cmdline";
    struct stat fileStats;

    buffer.clear();
    args.clear();

    // Open the cmdline file in the /proc file system for the current process.
    int fd = open(cmdLinePath, O_RDONLY);

    if (fd >= 0)
    {
        // Read the size of the file so that we can read its entire contents.
        if (fstat(fd, &fileStats) == 0)
        {
            // Ensure we have a large enough buffer to read the entire file.
            size_t fileSize = toSize(fileStats.st_size);
            size_t requiredSize = fileSize + 1;

            if (buffer.capacity() < requiredSize)
            {
                buffer.reserve(requiredSize);
            }

            buffer.resize(fileSize, '\0');

            // Read the file contents.
            size_t bytesRead = read(fd, buffer.data(), fileSize);

            if (bytesRead < fileSize)
            {
                // Truncate the buffer if necessary.
                buffer.erase(buffer.begin() + bytesRead, buffer.end());
            }

            // Parse the file text into command line argument tokens.
            bool isInQuotes = false;
            bool isInArg = false;
            char quote = '"';

            for (size_t index = 0; index < buffer.size(); ++index)
            {
                // Always flush at the end.
                bool flush = false;

                // We have characters left to process.
                char next = buffer[index];

                if (isInQuotes)
                {
                    // Anything goes unless its a closing quote.
                    if (next == quote)
                    {
                        // It's the closing quote of a quoted argument token.
                        isInArg = false;
                        isInQuotes = false;
                        flush = true;
                    }
                }
                else if (next == ' ')
                {
                    if (isInArg)
                    {
                        // It's a space between argument tokens.
                        isInArg = false;
                        flush = true;
                    }
                }
                else if (next == '"')
                {
                    isInQuotes = true;
                    quote = next;

                    if (isInArg == false)
                    {
                        // Start a new (quoted) argument.
                        isInArg = true;
                        args.push_back(buffer.data() + index);
                    }
                }
                else // if (unquoted non-space)
                {
                    if (isInArg == false)
                    {
                        // Start an un-quoted argument token.
                        isInArg = true;
                        args.push_back(buffer.data() + index);
                    }
                }

                if (flush)
                {
                    if (index < buffer.size())
                    {
                        // Null termiante the argument token.
                        buffer[index] = '\0';
                    }
                }
            }

            // Ensure the buffer and therefore the last token, is null-terminated.
            buffer.push_back('\0');
        }

        // Ensure the stream is closed.
        close(fd);
    }
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


//! @file ArmDbg/PipeClient.cpp
//! @brief The definition of a class which connects to a named pipe server
//! to send debugger commands and receive output.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <cstdint>
#include <vector>

#include "PipeClient.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// PipeClient Member Definitions
////////////////////////////////////////////////////////////////////////////////
PipeClient::PipeClient(const std::string &pipeName) :
    _pipeName("\\\\.\\pipe\\" + pipeName)
{
}

bool PipeClient::sendCommand(const std::string &command,
                             std::string &output, uint8_t &status)
{
    output.clear();
    status = 1;

    // Connect to the named pipe, retrying for up to 5 seconds if the pipe
    // is busy or momentarily absent (between server reconnect cycles).
    HANDLE hPipe = INVALID_HANDLE_VALUE;

    for (int attempt = 0; attempt < 50; ++attempt)
    {
        hPipe = CreateFileA(
            _pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0, nullptr, OPEN_EXISTING, 0, nullptr);

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        DWORD err = GetLastError();

        if (err == ERROR_PIPE_BUSY)
        {
            WaitNamedPipeA(_pipeName.c_str(), 5000);
            continue;
        }

        if (err == ERROR_FILE_NOT_FOUND)
        {
            Sleep(100);
            continue;
        }

        return false;
    }

    if (hPipe == INVALID_HANDLE_VALUE)
        return false;

    // Send request frame: [uint32_t length][payload].
    auto payloadSize = static_cast<uint32_t>(command.size());
    DWORD bytesWritten = 0;

    if (!WriteFile(hPipe, &payloadSize, sizeof(payloadSize),
                   &bytesWritten, nullptr))
    {
        CloseHandle(hPipe);
        return false;
    }

    if (payloadSize > 0)
    {
        if (!WriteFile(hPipe, command.data(), payloadSize,
                       &bytesWritten, nullptr))
        {
            CloseHandle(hPipe);
            return false;
        }
    }

    FlushFileBuffers(hPipe);

    // Read response frame: [uint32_t length][uint8_t status][text bytes].
    uint32_t responseLength = 0;
    DWORD bytesRead = 0;

    if (!ReadFile(hPipe, &responseLength, sizeof(responseLength),
                  &bytesRead, nullptr) ||
        bytesRead != sizeof(responseLength))
    {
        CloseHandle(hPipe);
        return false;
    }

    if (responseLength == 0)
    {
        CloseHandle(hPipe);
        status = 0;
        return true;
    }

    // Read the full response payload.
    std::vector<uint8_t> responsePayload(responseLength);
    DWORD totalRead = 0;

    while (totalRead < responseLength)
    {
        DWORD chunkRead = 0;

        if (!ReadFile(hPipe, responsePayload.data() + totalRead,
                      responseLength - totalRead, &chunkRead, nullptr) ||
            chunkRead == 0)
        {
            CloseHandle(hPipe);
            return false;
        }

        totalRead += chunkRead;
    }

    CloseHandle(hPipe);

    // Parse: first byte is status, rest is text.
    status = responsePayload[0];

    if (responseLength > 1)
    {
        output.assign(responsePayload.begin() + 1, responsePayload.end());
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

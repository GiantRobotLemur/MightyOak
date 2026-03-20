//! @file ArmDbg/PipeServer.cpp
//! @brief The definition of a class which hosts a named pipe server for
//! remote debugger command execution.
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

#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

#include "PipeServer.hpp"
#include "ArmDbg/ScriptRunner.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Reads a length-prefixed frame from a pipe handle.
bool readFrame(HANDLE hPipe, std::vector<uint8_t> &payload)
{
    payload.clear();

    uint32_t length = 0;
    DWORD bytesRead = 0;

    if (!ReadFile(hPipe, &length, sizeof(length), &bytesRead, nullptr) ||
        bytesRead != sizeof(length))
    {
        return false;
    }

    if (length == 0)
        return true;

    if (length > 16 * 1024 * 1024)
        return false;

    payload.resize(length);
    DWORD totalRead = 0;

    while (totalRead < length)
    {
        DWORD chunkRead = 0;

        if (!ReadFile(hPipe, payload.data() + totalRead,
                      length - totalRead, &chunkRead, nullptr) ||
            chunkRead == 0)
        {
            return false;
        }

        totalRead += chunkRead;
    }

    return true;
}

//! @brief Writes a response frame (status byte + text) to a pipe handle.
bool writeResponse(HANDLE hPipe, uint8_t status, const std::string &text)
{
    uint32_t payloadLength = 1 + static_cast<uint32_t>(text.size());
    DWORD bytesWritten = 0;

    if (!WriteFile(hPipe, &payloadLength, sizeof(payloadLength),
                   &bytesWritten, nullptr))
    {
        return false;
    }

    if (!WriteFile(hPipe, &status, 1, &bytesWritten, nullptr))
        return false;

    if (!text.empty())
    {
        DWORD totalWritten = 0;
        auto remaining = static_cast<DWORD>(text.size());

        while (totalWritten < remaining)
        {
            DWORD chunkWritten = 0;

            if (!WriteFile(hPipe, text.data() + totalWritten,
                           remaining - totalWritten, &chunkWritten, nullptr))
            {
                return false;
            }

            totalWritten += chunkWritten;
        }
    }

    FlushFileBuffers(hPipe);
    return true;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// PipeServer Member Definitions
////////////////////////////////////////////////////////////////////////////////
PipeServer::PipeServer(ScriptRunner &runner, std::ostringstream &outputStream,
                       const std::string &pipeName) :
    _runner(runner),
    _outputStream(outputStream),
    _pipeName("\\\\.\\pipe\\" + pipeName),
    _shutdownEvent(nullptr)
{
}

PipeServer::~PipeServer()
{
    if (_shutdownEvent != nullptr)
    {
        CloseHandle(static_cast<HANDLE>(_shutdownEvent));
        _shutdownEvent = nullptr;
    }
}

void PipeServer::requestShutdown()
{
    if (_shutdownEvent != nullptr)
    {
        SetEvent(static_cast<HANDLE>(_shutdownEvent));
    }
}

//! @brief Runs the server loop, blocking until shutdown is requested.
//! @retval 0 Clean shutdown.
//! @retval non-zero Error occurred.
int PipeServer::run()
{
    _shutdownEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);

    if (_shutdownEvent == nullptr)
    {
        fprintf(stderr, "Error: Failed to create shutdown event.\n");
        return 1;
    }

    printf("Listening on %s ...\n", _pipeName.c_str());
    fflush(stdout);

    bool shutdownRequested = false;

    while (!shutdownRequested)
    {
        HANDLE hPipe = CreateNamedPipeA(
            _pipeName.c_str(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1, 65536, 65536, 0, nullptr);

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            fprintf(stderr, "Error: Failed to create named pipe (error %lu).\n",
                    GetLastError());
            return 1;
        }

        // Overlapped connect for interruptible wait.
        OVERLAPPED ov = {};
        ov.hEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);
        ConnectNamedPipe(hPipe, &ov);

        DWORD err = GetLastError();

        if (err == ERROR_IO_PENDING)
        {
            HANDLE waitHandles[2] = { ov.hEvent,
                                      static_cast<HANDLE>(_shutdownEvent) };

            DWORD waitResult = WaitForMultipleObjects(2, waitHandles,
                                                      FALSE, INFINITE);

            if (waitResult == WAIT_OBJECT_0 + 1)
            {
                CancelIo(hPipe);
                CloseHandle(ov.hEvent);
                CloseHandle(hPipe);
                break;
            }
        }
        else if (err != ERROR_PIPE_CONNECTED && err != ERROR_SUCCESS)
        {
            fprintf(stderr, "Error: ConnectNamedPipe failed (error %lu).\n",
                    err);
            CloseHandle(ov.hEvent);
            CloseHandle(hPipe);
            continue;
        }

        CloseHandle(ov.hEvent);

        // Client command loop.
        size_t lineNumber = 0;

        while (!shutdownRequested)
        {
            std::vector<uint8_t> requestPayload;

            if (!readFrame(hPipe, requestPayload))
                break; // Client disconnected.

            std::string command(requestPayload.begin(), requestPayload.end());

            // Trim trailing whitespace.
            while (!command.empty() &&
                   (command.back() == '\n' || command.back() == '\r' ||
                    command.back() == ' '))
            {
                command.pop_back();
            }

            // Server meta-commands.
            if (command == "quit" || command == "shutdown")
            {
                writeResponse(hPipe, 0, "Server shutting down.\n");
                shutdownRequested = true;
                break;
            }

            if (command == "ping")
            {
                writeResponse(hPipe, 0, "pong\n");
                continue;
            }

            // Clear captured output before processing.
            _outputStream.str("");
            _outputStream.clear();

            // Process each line through the script runner.
            std::istringstream lines(command);
            std::string line;
            bool allOk = true;

            while (std::getline(lines, line))
            {
                ++lineNumber;

                if (!_runner.processLine(line, lineNumber))
                    allOk = false;
            }

            // Harvest captured output and send response.
            std::string output = _outputStream.str();
            writeResponse(hPipe, allOk ? 0 : 1, output);
        }

        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }

    printf("Server stopped.\n");
    fflush(stdout);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

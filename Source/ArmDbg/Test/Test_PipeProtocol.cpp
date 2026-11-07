//! @file ArmDbg/Test/Test_PipeProtocol.cpp
//! @brief Unit tests for the named pipe IPC protocol framing and
//! client-server integration.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <cstdint>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "ArmDbg/DebugSession.hpp"
#include "ArmDbg/ScriptRunner.hpp"
#include "PipeServer.hpp"
#include "PipeClient.hpp"

namespace Mo {
namespace Arm {
namespace {

////////////////////////////////////////////////////////////////////////////////
// Frame Encoding Tests
////////////////////////////////////////////////////////////////////////////////
//! @brief Verifies that a request frame can be written and read back via a
//! local named pipe.
TEST(PipeProtocol, FrameRoundTrip)
{
    const char *pipeName = "\\\\.\\pipe\\ArmDbgTest_FrameRoundTrip";

    HANDLE hServer = CreateNamedPipeA(
        pipeName,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1, 4096, 4096, 0, nullptr);

    ASSERT_NE(hServer, INVALID_HANDLE_VALUE);

    // Connect client in a separate thread.
    std::string testPayload = "regs";
    std::thread clientThread([&]() {
        HANDLE hClient = CreateFileA(pipeName, GENERIC_READ | GENERIC_WRITE,
                                     0, nullptr, OPEN_EXISTING, 0, nullptr);

        if (hClient == INVALID_HANDLE_VALUE)
            return;

        // Write a request frame.
        uint32_t len = static_cast<uint32_t>(testPayload.size());
        DWORD written = 0;
        WriteFile(hClient, &len, sizeof(len), &written, nullptr);
        WriteFile(hClient, testPayload.data(), len, &written, nullptr);
        FlushFileBuffers(hClient);

        // Read response frame.
        uint32_t respLen = 0;
        DWORD bytesRead = 0;
        ReadFile(hClient, &respLen, sizeof(respLen), &bytesRead, nullptr);

        std::vector<uint8_t> resp(respLen);
        ReadFile(hClient, resp.data(), respLen, &bytesRead, nullptr);

        CloseHandle(hClient);
    });

    ConnectNamedPipe(hServer, nullptr);

    // Read the request frame on the server side.
    uint32_t reqLen = 0;
    DWORD bytesRead = 0;

    ASSERT_TRUE(ReadFile(hServer, &reqLen, sizeof(reqLen), &bytesRead, nullptr));
    ASSERT_EQ(bytesRead, sizeof(reqLen));
    ASSERT_EQ(reqLen, testPayload.size());

    std::vector<char> reqData(reqLen);
    ASSERT_TRUE(ReadFile(hServer, reqData.data(), reqLen, &bytesRead, nullptr));
    ASSERT_EQ(bytesRead, reqLen);

    std::string received(reqData.begin(), reqData.end());
    EXPECT_EQ(received, testPayload);

    // Write a response frame.
    std::string responseText = "OK";
    uint32_t respPayloadLen = 1 + static_cast<uint32_t>(responseText.size());
    uint8_t status = 0;
    DWORD written = 0;

    WriteFile(hServer, &respPayloadLen, sizeof(respPayloadLen), &written, nullptr);
    WriteFile(hServer, &status, 1, &written, nullptr);
    WriteFile(hServer, responseText.data(),
              static_cast<DWORD>(responseText.size()), &written, nullptr);
    FlushFileBuffers(hServer);

    clientThread.join();

    DisconnectNamedPipe(hServer);
    CloseHandle(hServer);
}

////////////////////////////////////////////////////////////////////////////////
// Client-Server Integration Test
////////////////////////////////////////////////////////////////////////////////
//! @brief Starts a PipeServer on a background thread, sends commands via
//! PipeClient, and verifies responses.
TEST(PipeProtocol, ClientServerIntegration)
{
    const std::string pipeName = "ArmDbgTest_Integration";

    std::ostringstream capturedOutput;
    DebugSession session(capturedOutput);
    ScriptRunner runner(session);
    PipeServer server(runner, capturedOutput, pipeName);

    // Run the server on a background thread.
    std::thread serverThread([&server]() {
        server.run();
    });

    // Give the server a moment to start listening.
    Sleep(100);

    PipeClient client(pipeName);

    // Test ping.
    {
        std::string output;
        uint8_t status = 0xFF;
        ASSERT_TRUE(client.sendCommand("ping", output, status));
        EXPECT_EQ(status, 0);
        EXPECT_EQ(output, "pong\n");
    }

    // Test a real command (echo).
    {
        std::string output;
        uint8_t status = 0xFF;
        ASSERT_TRUE(client.sendCommand("echo Hello from pipe", output, status));
        EXPECT_EQ(status, 0);
        EXPECT_NE(output.find("Hello from pipe"), std::string::npos);
    }

    // Test shutdown.
    {
        std::string output;
        uint8_t status = 0xFF;
        ASSERT_TRUE(client.sendCommand("quit", output, status));
        EXPECT_EQ(status, 0);
    }

    serverThread.join();
}

//! @brief Verifies that the client returns false when no server is listening.
TEST(PipeProtocol, ClientNoServer)
{
    PipeClient client("ArmDbgTest_NoSuchPipe");
    std::string output;
    uint8_t status = 0;

    EXPECT_FALSE(client.sendCommand("ping", output, status));
}

} // anonymous namespace
}} // namespace Mo::Arm

#endif // _WIN32

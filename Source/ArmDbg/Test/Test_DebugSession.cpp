//! @file ArmDbg/Test/Test_DebugSession.cpp
//! @brief Unit tests for the DebugSession class.
//! @date 2026
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <gtest/gtest.h>

#include "ArmDbg/DebugSession.hpp"
#include "ArmDbg/CommandDefs.hpp"

namespace Mo {
namespace Arm {

GTEST_TEST(DebugSession, NotInitialisedByDefault)
{
    std::ostringstream out;
    DebugSession session(out);
    EXPECT_FALSE(session.isInitialised());
}

GTEST_TEST(DebugSession, CommandRequiresInit)
{
    std::ostringstream out;
    DebugSession session(out);

    ParsedCommand cmd;
    cmd.Command = DbgCommand::Regs;
    cmd.LineNumber = 1;

    EXPECT_FALSE(session.executeCommand(cmd));
    EXPECT_NE(out.str().find("init"), std::string::npos);
}

GTEST_TEST(DebugSession, EchoWorks)
{
    std::ostringstream out;
    DebugSession session(out);

    ParsedCommand cmd;
    cmd.Command = DbgCommand::Echo;
    cmd.Arguments.push_back("Hello World");
    cmd.LineNumber = 1;

    EXPECT_TRUE(session.executeCommand(cmd));
    EXPECT_NE(out.str().find("Hello World"), std::string::npos);
}

GTEST_TEST(DebugSession, ConfigAndInit)
{
    std::ostringstream out;
    DebugSession session(out);

    // Configure.
    ParsedCommand cmd;

    cmd.Command = DbgCommand::ConfigModel;
    cmd.Arguments = { "Archimedies" };
    cmd.LineNumber = 1;
    EXPECT_TRUE(session.executeCommand(cmd));

    cmd.Command = DbgCommand::ConfigCpu;
    cmd.Arguments = { "ARM2" };
    cmd.LineNumber = 2;
    EXPECT_TRUE(session.executeCommand(cmd));

    cmd.Command = DbgCommand::ConfigRom;
    cmd.Arguments = { "RiscOS_3_10" };
    cmd.LineNumber = 3;
    EXPECT_TRUE(session.executeCommand(cmd));

    cmd.Command = DbgCommand::ConfigRam;
    cmd.Arguments = { "4096" };
    cmd.LineNumber = 4;
    EXPECT_TRUE(session.executeCommand(cmd));

    // Init.
    cmd.Command = DbgCommand::Init;
    cmd.Arguments.clear();
    cmd.LineNumber = 5;

    bool initResult = session.executeCommand(cmd);

    if (!initResult)
    {
        // ROM may not be available in all test environments.
        GTEST_SKIP() << "ROM not available: " << out.str();
    }

    EXPECT_TRUE(session.isInitialised());
    EXPECT_NE(out.str().find("initialised"), std::string::npos);
}

GTEST_TEST(DebugSession, RunAfterInit)
{
    std::ostringstream out;
    DebugSession session(out);

    // Quick init.
    ParsedCommand cmd;
    cmd.Command = DbgCommand::Init;
    cmd.LineNumber = 1;

    if (!session.executeCommand(cmd))
        GTEST_SKIP() << "ROM not available";

    // Run 1000 cycles.
    out.str("");
    cmd.Command = DbgCommand::Run;
    cmd.Arguments = { "1000" };
    cmd.LineNumber = 2;

    EXPECT_TRUE(session.executeCommand(cmd));
    EXPECT_NE(out.str().find("instructions"), std::string::npos);
}

GTEST_TEST(DebugSession, RegsAfterInit)
{
    std::ostringstream out;
    DebugSession session(out);

    ParsedCommand cmd;
    cmd.Command = DbgCommand::Init;
    cmd.LineNumber = 1;

    if (!session.executeCommand(cmd))
        GTEST_SKIP() << "ROM not available";

    out.str("");
    cmd.Command = DbgCommand::Regs;
    cmd.Arguments.clear();
    cmd.LineNumber = 2;

    EXPECT_TRUE(session.executeCommand(cmd));
    EXPECT_NE(out.str().find("PC"), std::string::npos);
    EXPECT_NE(out.str().find("Registers"), std::string::npos);
}

GTEST_TEST(DebugSession, InvalidConfigModel)
{
    std::ostringstream out;
    DebugSession session(out);

    ParsedCommand cmd;
    cmd.Command = DbgCommand::ConfigModel;
    cmd.Arguments = { "ZX_Spectrum" };
    cmd.LineNumber = 1;

    EXPECT_FALSE(session.executeCommand(cmd));
    EXPECT_NE(out.str().find("Error"), std::string::npos);
}

GTEST_TEST(DebugSession, ParseUint32Hex)
{
    std::ostringstream out;
    DebugSession session(out);

    ParsedCommand cmd;
    cmd.Command = DbgCommand::Init;
    cmd.LineNumber = 1;

    if (!session.executeCommand(cmd))
        GTEST_SKIP() << "ROM not available";

    // Test hex address parsing via mem command.
    out.str("");
    cmd.Command = DbgCommand::Mem;
    cmd.Arguments = { "0x03800000", "16" };
    cmd.LineNumber = 2;

    EXPECT_TRUE(session.executeCommand(cmd));
    EXPECT_NE(out.str().find("03800000"), std::string::npos);
}

}} // namespace Mo::Arm

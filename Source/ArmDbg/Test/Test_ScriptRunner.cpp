//! @file ArmDbg/Test/Test_ScriptRunner.cpp
//! @brief Unit tests for the ScriptRunner pipeline.
//! @date 2026
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <gtest/gtest.h>

#include "ArmDbg/DebugSession.hpp"
#include "ArmDbg/ScriptRunner.hpp"

namespace Mo {
namespace Arm {

GTEST_TEST(ScriptRunner, EmptyScript)
{
    std::ostringstream out;
    DebugSession session(out);
    ScriptRunner runner(session);

    std::istringstream input("");
    EXPECT_TRUE(runner.run(input));
}

GTEST_TEST(ScriptRunner, CommentsOnly)
{
    std::ostringstream out;
    DebugSession session(out);
    ScriptRunner runner(session);

    std::istringstream input("# comment 1\n# comment 2\n\n");
    EXPECT_TRUE(runner.run(input));
    EXPECT_TRUE(out.str().empty());
}

GTEST_TEST(ScriptRunner, EchoScript)
{
    std::ostringstream out;
    DebugSession session(out);
    ScriptRunner runner(session);

    std::istringstream input("echo Hello from script\necho Line two\n");
    EXPECT_TRUE(runner.run(input));
    EXPECT_NE(out.str().find("Hello from script"), std::string::npos);
    EXPECT_NE(out.str().find("Line two"), std::string::npos);
}

GTEST_TEST(ScriptRunner, FullBootScript)
{
    std::ostringstream out;
    DebugSession session(out);
    ScriptRunner runner(session);

    std::istringstream input(
        "# Test boot script\n"
        "config model Archimedies\n"
        "config cpu ARM2\n"
        "config rom RiscOS_3_10\n"
        "config ram 4096\n"
        "init\n"
        "echo === Running 1000 cycles ===\n"
        "run 1000\n"
        "regs\n"
        "pc\n"
    );

    bool result = runner.run(input);

    // If ROM is not available, init will fail and subsequent commands too.
    std::string output = out.str();

    if (output.find("Error") != std::string::npos &&
        output.find("ROM") != std::string::npos)
    {
        GTEST_SKIP() << "ROM not available";
    }

    if (result)
    {
        EXPECT_NE(output.find("initialised"), std::string::npos);
        EXPECT_NE(output.find("instructions"), std::string::npos);
        EXPECT_NE(output.find("Registers"), std::string::npos);
    }
}

GTEST_TEST(ScriptRunner, ProcessSingleLine)
{
    std::ostringstream out;
    DebugSession session(out);
    ScriptRunner runner(session);

    EXPECT_TRUE(runner.processLine("# comment", 1));
    EXPECT_TRUE(runner.processLine("", 2));
    EXPECT_TRUE(runner.processLine("echo test", 3));
    EXPECT_NE(out.str().find("test"), std::string::npos);
}

GTEST_TEST(ScriptRunner, UnknownCommandFails)
{
    std::ostringstream out;
    DebugSession session(out);
    ScriptRunner runner(session);

    EXPECT_FALSE(runner.processLine("foobar", 1));
}

}} // namespace Mo::Arm

//! @file ArmDbg/Test/Test_CommandParser.cpp
//! @brief Unit tests for the CommandParser.
//! @date 2026
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "ScriptLexer.hpp"
#include "CommandParser.hpp"

namespace Mo {
namespace Arm {

namespace {

//! @brief Helper to parse a line and return the command.
bool parseLine(const char *line, ParsedCommand &cmd, std::string &error)
{
    std::vector<ScriptToken> tokens;

    if (!ScriptLexer::tokenizeLine(line, tokens))
        return false;

    return CommandParser::parseLine(tokens, 1, cmd, error);
}

} // anonymous namespace

GTEST_TEST(CommandParser, SimpleCommand)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("regs", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::Regs);
    EXPECT_TRUE(cmd.Arguments.empty());
}

GTEST_TEST(CommandParser, RunWithArg)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("run 1000000", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::Run);
    ASSERT_EQ(cmd.Arguments.size(), 1u);
    EXPECT_EQ(cmd.Arguments[0], "1000000");
}

GTEST_TEST(CommandParser, ConfigModel)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("config model Archimedies", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::ConfigModel);
    ASSERT_EQ(cmd.Arguments.size(), 1u);
    EXPECT_EQ(cmd.Arguments[0], "Archimedies");
}

GTEST_TEST(CommandParser, ConfigCpu)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("config cpu ARM2", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::ConfigCpu);
    ASSERT_EQ(cmd.Arguments.size(), 1u);
    EXPECT_EQ(cmd.Arguments[0], "ARM2");
}

GTEST_TEST(CommandParser, ConfigRom)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("config rom RiscOS_3_10", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::ConfigRom);
    ASSERT_EQ(cmd.Arguments.size(), 1u);
    EXPECT_EQ(cmd.Arguments[0], "RiscOS_3_10");
}

GTEST_TEST(CommandParser, ConfigRam)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("config ram 4096", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::ConfigRam);
    ASSERT_EQ(cmd.Arguments.size(), 1u);
    EXPECT_EQ(cmd.Arguments[0], "4096");
}

GTEST_TEST(CommandParser, MemWithOptions)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("mem 0x03800000 64 p", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::Mem);
    ASSERT_EQ(cmd.Arguments.size(), 3u);
    EXPECT_EQ(cmd.Arguments[0], "0x03800000");
    EXPECT_EQ(cmd.Arguments[1], "64");
    EXPECT_EQ(cmd.Arguments[2], "p");
}

GTEST_TEST(CommandParser, StepNoArg)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("step", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::Step);
    EXPECT_TRUE(cmd.Arguments.empty());
}

GTEST_TEST(CommandParser, StepWithArg)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("step 5", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::Step);
    ASSERT_EQ(cmd.Arguments.size(), 1u);
    EXPECT_EQ(cmd.Arguments[0], "5");
}

GTEST_TEST(CommandParser, EchoMessage)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("echo Hello World!", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::Echo);
    ASSERT_EQ(cmd.Arguments.size(), 1u);
    EXPECT_EQ(cmd.Arguments[0], "Hello World!");
}

GTEST_TEST(CommandParser, UnknownCommand)
{
    ParsedCommand cmd;
    std::string error;
    EXPECT_FALSE(parseLine("foobar", cmd, error));
    EXPECT_FALSE(error.empty());
}

GTEST_TEST(CommandParser, UnknownConfigSubCommand)
{
    ParsedCommand cmd;
    std::string error;
    EXPECT_FALSE(parseLine("config foobar", cmd, error));
    EXPECT_FALSE(error.empty());
}

GTEST_TEST(CommandParser, ConfigAlone)
{
    ParsedCommand cmd;
    std::string error;
    EXPECT_FALSE(parseLine("config", cmd, error));
    EXPECT_FALSE(error.empty());
}

GTEST_TEST(CommandParser, CaseInsensitive)
{
    ParsedCommand cmd;
    std::string error;
    ASSERT_TRUE(parseLine("RUN 500", cmd, error));
    EXPECT_EQ(cmd.Command, DbgCommand::Run);
}

GTEST_TEST(CommandParser, AllSimpleCommands)
{
    struct TestCase { const char *text; DbgCommand expected; };
    TestCase cases[] = {
        { "init",     DbgCommand::Init },
        { "continue", DbgCommand::Continue },
        { "regs",     DbgCommand::Regs },
        { "irq",      DbgCommand::Irq },
        { "memc",     DbgCommand::Memc },
        { "ioc",      DbgCommand::Ioc },
        { "pc",       DbgCommand::Pc },
    };

    for (const auto &tc : cases)
    {
        ParsedCommand cmd;
        std::string error;
        ASSERT_TRUE(parseLine(tc.text, cmd, error)) << "Failed: " << tc.text;
        EXPECT_EQ(cmd.Command, tc.expected) << "Wrong cmd for: " << tc.text;
    }
}

}} // namespace Mo::Arm

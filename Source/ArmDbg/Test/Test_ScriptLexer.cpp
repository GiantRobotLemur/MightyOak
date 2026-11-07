//! @file ArmDbg/Test/Test_ScriptLexer.cpp
//! @brief Unit tests for the ScriptLexer tokenizer.
//! @date 2026
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "ScriptLexer.hpp"

namespace Mo {
namespace Arm {

GTEST_TEST(ScriptLexer, EmptyLine)
{
    std::vector<ScriptToken> tokens;
    EXPECT_FALSE(ScriptLexer::tokenizeLine("", tokens));
    EXPECT_TRUE(tokens.empty());
}

GTEST_TEST(ScriptLexer, WhitespaceOnly)
{
    std::vector<ScriptToken> tokens;
    EXPECT_FALSE(ScriptLexer::tokenizeLine("   \t  ", tokens));
    EXPECT_TRUE(tokens.empty());
}

GTEST_TEST(ScriptLexer, CommentLine)
{
    std::vector<ScriptToken> tokens;
    EXPECT_FALSE(ScriptLexer::tokenizeLine("# this is a comment", tokens));
    EXPECT_TRUE(tokens.empty());
}

GTEST_TEST(ScriptLexer, IndentedComment)
{
    std::vector<ScriptToken> tokens;
    EXPECT_FALSE(ScriptLexer::tokenizeLine("   # indented comment", tokens));
    EXPECT_TRUE(tokens.empty());
}

GTEST_TEST(ScriptLexer, SingleToken)
{
    std::vector<ScriptToken> tokens;
    EXPECT_TRUE(ScriptLexer::tokenizeLine("regs", tokens));
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].Text, "regs");
    EXPECT_EQ(tokens[0].Column, 0u);
}

GTEST_TEST(ScriptLexer, MultipleTokens)
{
    std::vector<ScriptToken> tokens;
    EXPECT_TRUE(ScriptLexer::tokenizeLine("config model Archimedies", tokens));
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0].Text, "config");
    EXPECT_EQ(tokens[1].Text, "model");
    EXPECT_EQ(tokens[2].Text, "Archimedies");
}

GTEST_TEST(ScriptLexer, LeadingWhitespace)
{
    std::vector<ScriptToken> tokens;
    EXPECT_TRUE(ScriptLexer::tokenizeLine("  run 1000", tokens));
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].Text, "run");
    EXPECT_EQ(tokens[1].Text, "1000");
}

GTEST_TEST(ScriptLexer, InlineComment)
{
    std::vector<ScriptToken> tokens;
    EXPECT_TRUE(ScriptLexer::tokenizeLine("run 1000 # execute", tokens));
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].Text, "run");
    EXPECT_EQ(tokens[1].Text, "1000");
}

GTEST_TEST(ScriptLexer, EchoSpecialCase)
{
    std::vector<ScriptToken> tokens;
    EXPECT_TRUE(ScriptLexer::tokenizeLine("echo Hello World!", tokens));
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].Text, "echo");
    EXPECT_EQ(tokens[1].Text, "Hello World!");
}

GTEST_TEST(ScriptLexer, EchoWithLeadingWhitespace)
{
    std::vector<ScriptToken> tokens;
    EXPECT_TRUE(ScriptLexer::tokenizeLine("  echo   message text  ", tokens));
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].Text, "echo");
    EXPECT_EQ(tokens[1].Text, "message text  ");
}

GTEST_TEST(ScriptLexer, HexAddress)
{
    std::vector<ScriptToken> tokens;
    EXPECT_TRUE(ScriptLexer::tokenizeLine("mem 0x03800000 64", tokens));
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0].Text, "mem");
    EXPECT_EQ(tokens[1].Text, "0x03800000");
    EXPECT_EQ(tokens[2].Text, "64");
}

GTEST_TEST(ScriptLexer, TabSeparated)
{
    std::vector<ScriptToken> tokens;
    EXPECT_TRUE(ScriptLexer::tokenizeLine("config\tcpu\tARM2", tokens));
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0].Text, "config");
    EXPECT_EQ(tokens[1].Text, "cpu");
    EXPECT_EQ(tokens[2].Text, "ARM2");
}

}} // namespace Mo::Arm

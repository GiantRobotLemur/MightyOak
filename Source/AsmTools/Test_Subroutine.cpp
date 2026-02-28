//! @file AsmTools/Test_Subroutine.cpp
//! @brief The definition of unit tests for the use of the PROC/ENDPROC keywords.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "AsmTools.hpp"
#include "TestTools.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Subroutine, ProcWithoutSymbolFails)
{
    Messages log;
    ObjectCode code = assembleText("PROC ; no name", getDefaultOptions(), log);

    ASSERT_TRUE(log.hasErrors());
    ASSERT_GT(log.getMessages().size(), 0u);

    EXPECT_TRUE(log.getMessages().back().getMessage().contains("subroutine"));
}

GTEST_TEST(Subroutine, LoneEndProcFails)
{
    Messages log;
    ObjectCode code = assembleText("ENDPROC ; no name", getDefaultOptions(), log);

    ASSERT_TRUE(log.hasErrors());
    ASSERT_GT(log.getMessages().size(), 0u);

    EXPECT_TRUE(log.getMessages().back().getMessage().contains("subroutine"));
}

GTEST_TEST(Subroutine, AnEmptySubroutineIsFine)
{
    Options options = getDefaultOptions();
    Messages log;
    ObjectCode code = assembleText("PROC myProc : ENDPROC", options, log);

    EXPECT_TRUE(code.isEmpty());
    EXPECT_EQ(code.getCodeSize(), 0u);
    EXPECT_FALSE(log.hasErrors());

    const auto &symbols = code.getSymbols();
    EXPECT_EQ(symbols.size(), 1u);
    auto symbolPos = symbols.find("myProc");
    
    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress());
}

GTEST_TEST(Subroutine, WithContent)
{
    Ag::utf8_cptr_t source =
        "PROC myProc\n"
        "MOV R0,#69\n"
        "MOV PC,Link\n"
        "ENDPROC";

    Messages log;
    Options options = getDefaultOptions();
    ObjectCode code = assembleText(source, options, log);

    EXPECT_FALSE(code.isEmpty());
    EXPECT_EQ(code.getCodeSize(), 8u);
    EXPECT_FALSE(log.hasErrors());

    const auto &symbols = code.getSymbols();
    EXPECT_EQ(symbols.size(), 1u);
    auto symbolPos = symbols.find("myProc");

    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress());
}

GTEST_TEST(Subroutine, WithSymbols)
{
    Ag::utf8_cptr_t source =
        "PROC myProc\n"
        ".prologue\n"
        "STMFD SP!,{R1-R3,Link}\n"
        "MOV R0,#69\n"
        ".epilogue\n"
        "LDMFD SP!,{R1-R3,PC}\n"
        "ENDPROC";

    Messages log;
    Options options = getDefaultOptions();
    ObjectCode code = assembleText(source, options, log);

    EXPECT_FALSE(code.isEmpty());
    EXPECT_EQ(code.getCodeSize(), 12u);
    EXPECT_FALSE(log.hasErrors());

    const auto &symbols = code.getSymbols();
    EXPECT_EQ(symbols.size(), 3u);
    auto symbolPos = symbols.find("myProc");

    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress());

    symbolPos = symbols.find("myProc.prologue");
    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress());

    symbolPos = symbols.find("myProc.epilogue");
    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress() + 8);
}

GTEST_TEST(Subroutine, WithReferencedSymbols)
{
    Ag::utf8_cptr_t source =
        "PROC myProc\n"
        "STMFD SP!,{R1-R3,Link}\n"
        "MOV R0,#0x80000000\n"
        ".loop\n"
        "MOVS R0,R0,LSR #1\n"
        "BNE loop\n"
        "LDMFD SP!,{R1-R3,PC}\n"
        "ENDPROC";

    Messages log;
    Options options = getDefaultOptions();
    ObjectCode code = assembleText(source, options, log);

    EXPECT_FALSE(code.isEmpty());
    EXPECT_EQ(code.getCodeSize(), 20u);
    EXPECT_FALSE(log.hasErrors());

    const auto &symbols = code.getSymbols();
    EXPECT_EQ(symbols.size(), 2u);
    auto symbolPos = symbols.find("myProc");

    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress());

    symbolPos = symbols.find("myProc.loop");
    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress() + 8);
}

GTEST_TEST(Subroutine, WithDeferredSymbols)
{
    Ag::utf8_cptr_t source =
        "PROC myProc\n"
        "STMFD SP!,{R1-R3,Link}\n"
        "CMP R0,#69\n"
        "BEQ epilogue\n"
        "MOV R0,#42\n"
        ".epilogue\n"
        "LDMFD SP!,{R1-R3,PC}\n"
        "ENDPROC";

    Messages log;
    Options options = getDefaultOptions();
    ObjectCode code = assembleText(source, options, log);

    EXPECT_FALSE(code.isEmpty());
    EXPECT_EQ(code.getCodeSize(), 20u);
    EXPECT_FALSE(log.hasErrors());

    const auto &symbols = code.getSymbols();
    EXPECT_EQ(symbols.size(), 2u);
    auto symbolPos = symbols.find("myProc");

    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress());

    symbolPos = symbols.find("myProc.epilogue");
    ASSERT_NE(code.getSymbols().end(), symbolPos);
    EXPECT_EQ(symbolPos->second, options.getLoadAddress() + 16);
}

GTEST_TEST(Subroutine, NestedProcFails)
{
    Ag::utf8_cptr_t source =
        "PROC myProc\n"
        "STMFD SP!,{R1-R3,Link}\n"
        "CMP R0,#69\n"
        "BEQ epilogue\n"
        "PROC yourProc\n"
        "ADD R3,R1,#69\n"
        "ENDPROC\n"
        "MOV R0,#42\n"
        ".epilogue\n"
        "LDMFD SP!,{R1-R3,PC}\n"
        "ENDPROC";

    Messages log;
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    ASSERT_TRUE(log.hasErrors());
    ASSERT_GT(log.getMessages().size(), 0u);

    EXPECT_TRUE(log.getMessages().back().getMessage().contains("subroutine"));
}

GTEST_TEST(Subroutine, DupeNameFails)
{
    Ag::utf8_cptr_t source =
        "PROC myProc\n"
        "STMFD SP!,{R1-R3,Link}\n"
        "CMP R0,#69\n"
        "BEQ epilogue\n"
        "MOV R0,#42\n"
        ".epilogue\n"
        "LDMFD SP!,{R1-R3,PC}\n"
        "ENDPROC\n\n"
        "PROC myProc\n"
        "ADD R3,R1,#69\n"
        "ENDPROC\n";

    Messages log;
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    ASSERT_TRUE(log.hasErrors());
    ASSERT_GT(log.getMessages().size(), 0u);

    EXPECT_TRUE(log.getMessages().back().getMessage().contains("subroutine"));
}

GTEST_TEST(Subroutine, ScopedNamesDontInterfere)
{
    Ag::utf8_cptr_t source =
        "PROC myProc\n"
        "STMFD SP!,{R1-R3,Link}\n"
        "CMP R0,#69\n"
        "BEQ epilogue\n"
        "MOV R0,#42\n"
        ".epilogue\n"
        "LDMFD SP!,{R1-R3,PC}\n"
        "ENDPROC\n\n"
        "PROC yourProc\n"
        "ADD R3,R1,#69\n"
        ".epilogue\n"
        "MOV PC,Link\n"
        "ENDPROC\n";

    Messages log;
    ObjectCode code = assembleText(source, getDefaultOptions(), log);

    EXPECT_FALSE(code.isEmpty());
    EXPECT_FALSE(log.hasErrors());

    const auto &symbols = code.getSymbols();
    EXPECT_EQ(symbols.size(), 4u);
    EXPECT_NE(symbols.find("myProc"), symbols.end());
    EXPECT_NE(symbols.find("myProc.epilogue"), symbols.end());
    EXPECT_NE(symbols.find("yourProc"), symbols.end());
    EXPECT_NE(symbols.find("yourProc.epilogue"), symbols.end());
}

GTEST_TEST(Subroutine, ScopedNamesOverrideGlobal)
{
    Ag::utf8_cptr_t source =
        ".epilogue\n"
        "MOV PC,Link\n"
        "PROC myProc\n"
        "STMFD SP!,{R1-R3,Link}\n"
        "CMP R0,#69\n"
        "BEQ epilogue\n"
        "MOV R0,#42\n"
        ".epilogue\n"
        "LDMFD SP!,{R1-R3,PC}\n"
        "EQUD epilogue\n"
        "ENDPROC\n"
        "EQUD epilogue\n";

    // TODO: Defer subroutine code so that any late defined local symbols
    // referenced earlier use the late definitions rather than whatever is
    // available when they are assembled.

    Messages log;
    Options opts = getDefaultOptions();
    ObjectCode code = assembleText(source, opts, log);

    EXPECT_FALSE(code.isEmpty());
    EXPECT_FALSE(log.hasErrors());

    const auto &symbols = code.getSymbols();
    EXPECT_EQ(symbols.size(), 3u);
    const uint32_t *words = reinterpret_cast<const uint32_t *>(code.getCode());
    uint32_t wordCount = static_cast<uint32_t>(code.getCodeSize() / 4u);
    uint32_t base = opts.getLoadAddress();

    auto pos = symbols.find("myProc");
    ASSERT_NE(pos, symbols.end());
    EXPECT_EQ(pos->second, base + 4u);

    pos = symbols.find("myProc.epilogue");
    ASSERT_NE(pos, symbols.end());
    EXPECT_EQ(pos->second, base + 20u);

    // Ensure the resolved value matches the local symbol.
    EXPECT_EQ(words[wordCount - 2], pos->second);

    pos = symbols.find("epilogue");
    ASSERT_NE(pos, symbols.end());
    EXPECT_EQ(pos->second, base + 0u);

    // Ensure the resolved value matches the global symbol.
    EXPECT_EQ(words[wordCount - 1], pos->second);
}

} // Anonymous namespace

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


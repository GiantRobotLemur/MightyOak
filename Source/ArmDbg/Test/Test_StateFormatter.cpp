//! @file ArmDbg/Test/Test_StateFormatter.cpp
//! @brief Unit tests for the StateFormatter.
//! @date 2026
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <gtest/gtest.h>

#include "StateFormatter.hpp"

namespace Mo {
namespace Arm {

GTEST_TEST(StateFormatter, FormatRegisters)
{
    uint32_t regs[16] = {};
    regs[0] = 0x12345678;
    regs[15] = 0x03800000;

    std::ostringstream out;
    StateFormatter::formatRegisters(out, regs, 0x60000013, ProcessorMode::Svc26);

    std::string result = out.str();
    EXPECT_NE(result.find("12345678"), std::string::npos);
    EXPECT_NE(result.find("03800000"), std::string::npos);
    EXPECT_NE(result.find("SVC26"), std::string::npos);
}

GTEST_TEST(StateFormatter, FormatMemoryDump)
{
    uint8_t data[32];

    for (int i = 0; i < 32; ++i)
        data[i] = static_cast<uint8_t>(i);

    std::ostringstream out;
    StateFormatter::formatMemoryDump(out, 0x1000, data, 32);

    std::string result = out.str();
    EXPECT_NE(result.find("00001000"), std::string::npos);
    EXPECT_NE(result.find("00001010"), std::string::npos);
    EXPECT_NE(result.find("|"), std::string::npos);
}

GTEST_TEST(StateFormatter, FormatDisassembly)
{
    // MOV R0, #0
    uint32_t words[] = { 0xE3A00000, 0xE1A0F00E };

    std::ostringstream out;
    StateFormatter::formatDisassembly(out, 0x8000, words, 2);

    std::string result = out.str();
    EXPECT_NE(result.find("00008000"), std::string::npos);
    EXPECT_NE(result.find("00008004"), std::string::npos);
}

GTEST_TEST(StateFormatter, FormatDisassemblyWithHighlight)
{
    uint32_t words[] = { 0xE3A00000, 0xE3A01001 };

    std::ostringstream out;
    StateFormatter::formatDisassembly(out, 0x8000, words, 2, 0x8004);

    std::string result = out.str();
    EXPECT_NE(result.find(">>> "), std::string::npos);
}

GTEST_TEST(StateFormatter, FormatIrqState)
{
    std::ostringstream out;
    StateFormatter::formatIrqState(out, 0x0020, 0x0020, 0, 0);

    std::string result = out.str();
    EXPECT_NE(result.find("Timer0"), std::string::npos);
    EXPECT_NE(result.find("SET"), std::string::npos);
}

GTEST_TEST(StateFormatter, FormatIrqStateKart)
{
    std::ostringstream out;
    // KartRx = bit 15 = 0x8000
    StateFormatter::formatIrqState(out, 0x8000, 0xC000, 0, 0);

    std::string result = out.str();
    EXPECT_NE(result.find("KartRx"), std::string::npos);
    EXPECT_NE(result.find("KartTx"), std::string::npos);
}

}} // namespace Mo::Arm

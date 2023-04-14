//! @file Test_AddressDirective.cpp
//! @brief The definition of unit tests for the ADR-pseudo instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "TestTools.hpp"
#include "AsmTools.hpp"

namespace Ag {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(AddressDirective, SimpleAdd)
{
    Messages messages;
    ObjectCode result = assembleText("ADR R2,$ + 16", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 4);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Ensure we have an encoding for ADD R2,PC,#8.
    EXPECT_TRUE(equalHex(words[0], 0xE28F2F02));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_TRUE(instruction.disassemble(0xE28F2F02, baseAddr));

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Al);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Single);
    EXPECT_EQ(info.Rd, CoreRegister::R2);
    EXPECT_EQ(info.Address, baseAddr + 16);
}

GTEST_TEST(AddressDirective, AddLong)
{
    Messages messages;
    ObjectCode result = assembleText("ADRL R2,$ + 1234", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 8);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Ensure we have an encoding for ADD R2,PC,#202 : ADD R2,R2,#1024,
    // I.E. PC + 1234 - 8 (for piplining).
    EXPECT_TRUE(equalHex(words[0], 0xE28F20CA));
    EXPECT_TRUE(equalHex(words[1], 0xE2822B01));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_EQ(instruction.disassemble(words, 2, baseAddr), 2u);

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Al);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Long);
    EXPECT_EQ(info.Rd, CoreRegister::R2);
    EXPECT_EQ(info.Address, baseAddr + 1234);
}

GTEST_TEST(AddressDirective, AddLongPadded)
{
    Messages messages;
    ObjectCode result = assembleText("ADREQL R9,$ + 24", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 8);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Ensure we have an encoding for ADDEQ R9,PC,#16 : MOV R0,R0,
    // I.E. PC + 1234 - 8 (for piplining).
    EXPECT_TRUE(equalHex(words[0], 0x028F9E01));
    EXPECT_TRUE(equalHex(words[1], 0xE1A00000));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_EQ(instruction.disassemble(words, 2, baseAddr), 2u);

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Eq);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Long);
    EXPECT_EQ(info.Rd, CoreRegister::R9);
    EXPECT_EQ(info.Address, baseAddr + 24);
}

GTEST_TEST(AddressDirective, AddExtended)
{
    Messages messages;
    ObjectCode result = assembleText("ADRE R12,$ + 1234567", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 12);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Offset is PC + 12D67F
    // Ensure we have an encoding for:
    // ADD R12,PC,#&000007F,
    // ADD R12,R12,#&00D600,
    // ADD R12,R12,#&120000
    EXPECT_TRUE(equalHex(words[0], 0xE28FC07F));
    EXPECT_TRUE(equalHex(words[1], 0xE28CCCD6));
    EXPECT_TRUE(equalHex(words[2], 0xE28CC812));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_EQ(instruction.disassemble(words, 3, baseAddr), 3u);

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Al);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Extended);
    EXPECT_EQ(info.Rd, CoreRegister::R12);
    EXPECT_EQ(info.Address, baseAddr + 1234567);
}

GTEST_TEST(AddressDirective, AddExtendedPadded)
{
    Messages messages;
    ObjectCode result = assembleText("ADRE R12,$ + 1234", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 12);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    EXPECT_TRUE(equalHex(words[0], 0xE28FC0CA));
    EXPECT_TRUE(equalHex(words[1], 0xE28CCB01));
    EXPECT_TRUE(equalHex(words[2], 0xE1A00000));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_EQ(instruction.disassemble(words, 3, baseAddr), 3u);

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Al);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Extended);
    EXPECT_EQ(info.Rd, CoreRegister::R12);
    EXPECT_EQ(info.Address, baseAddr + 1234);
}

GTEST_TEST(AddressDirective, SimpleSubtract)
{
    Messages messages;
    ObjectCode result = assembleText("ADR R9,$ - 36", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 4);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Ensure we have an encoding for SUB R9,PC,#44.
    EXPECT_TRUE(equalHex(words[0], 0xE24F9F0B));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_TRUE(instruction.disassemble(0xE24F9F0B, baseAddr));

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Al);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Single);
    EXPECT_EQ(info.Rd, CoreRegister::R9);
    EXPECT_EQ(info.Address, baseAddr - 36);
}

GTEST_TEST(AddressDirective, SubtractLong)
{
    Messages messages;
    ObjectCode result = assembleText("ADRPLL R4,$ - 7654", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 8);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Offset = PC - &1DEE
    EXPECT_TRUE(equalHex(words[0], 0x524F40EE));
    EXPECT_TRUE(equalHex(words[1], 0x52444C1D));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_TRUE(instruction.disassemble(words, 2, baseAddr));

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Pl);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Long);
    EXPECT_EQ(info.Rd, CoreRegister::R4);
    EXPECT_EQ(info.Address, baseAddr - 7654);
}

GTEST_TEST(AddressDirective, SubtractLongPadded)
{
    Messages messages;
    ObjectCode result = assembleText("ADRL R8,$ - 99", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 8);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Offset = PC - &6B
    EXPECT_TRUE(equalHex(words[0], 0xE24F806B));
    EXPECT_TRUE(equalHex(words[1], 0xE1A00000));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_TRUE(instruction.disassemble(words, 2, baseAddr));

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Al);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Long);
    EXPECT_EQ(info.Rd, CoreRegister::R8);
    EXPECT_EQ(info.Address, baseAddr - 99);
}

GTEST_TEST(AddressDirective, SubtractExtended)
{
    const uint32_t baseAddr = 0x3300000;
    Messages messages;
    Options opts = getDefaultOptions();
    opts.setLoadAddress(baseAddr);
    ObjectCode result = assembleText("ADRCCE R4,$ - 1654321", opts, messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 12);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Offset = PC - &193E39
    EXPECT_TRUE(equalHex(words[0], 0x324F4039));
    EXPECT_TRUE(equalHex(words[1], 0x32444C3E));
    EXPECT_TRUE(equalHex(words[2], 0x32444819));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    ASSERT_TRUE(instruction.disassemble(words, 3, baseAddr));

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Cc);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Extended);
    EXPECT_EQ(info.Rd, CoreRegister::R4);
    EXPECT_EQ(info.Address, baseAddr - 1654321);
}

GTEST_TEST(AddressDirective, SubtractExtendedPadded)
{
    Messages messages;
    ObjectCode result = assembleText("ADRPLE R4,$ - 7654", getDefaultOptions(),
                                     messages);

    ASSERT_TRUE(assemblySuccess(messages));
    ASSERT_EQ(result.getCodeSize(), 12);

    const uint32_t *words = reinterpret_cast<const uint32_t *>(result.getCode());

    // Offset = PC - &1DEE
    EXPECT_TRUE(equalHex(words[0], 0x524F40EE));
    EXPECT_TRUE(equalHex(words[1], 0x52444C1D));
    EXPECT_TRUE(equalHex(words[2], 0xE1A00000));

    // Ensure the same pattern disassembles into appropriate parameters.
    InstructionInfo instruction;
    uint32_t baseAddr = getDefaultOptions().getLoadAddress();
    ASSERT_TRUE(instruction.disassemble(words, 3, baseAddr));

    ASSERT_EQ(instruction.getOperationClass(), OperationClass::CoreAddress);
    auto &info = instruction.getCoreAddressParameters();

    EXPECT_EQ(instruction.getMnemonic(), InstructionMnemonic::Adr);
    EXPECT_EQ(instruction.getCondition(), ConditionCode::Pl);
    EXPECT_EQ(info.Encoding, MultiWordEncoding::Extended);
    EXPECT_EQ(info.Rd, CoreRegister::R4);
    EXPECT_EQ(info.Address, baseAddr - 7654);
}

} // Anonymous namespace

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


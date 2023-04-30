//! @file ArmEmu/Test_ALU.cpp
//! @brief The definition of unit tests for the emulation of the Arithmetic
//! Logic Unit of an ARM processor core.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "Ag/Core/String.hpp"

#include "ArmEmu.hpp"

#include "TestTools.hpp"

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

namespace {

using ResultRequirement = std::pair<CoreRegister, uint32_t>;

struct ALUPoint
{
public:
    std::string_view Name;
    std::string_view SourceCode;
    std::vector<ResultRequirement> Requirements;

    ALUPoint(const char *name, const char *source, const std::initializer_list<ResultRequirement> &results) :
        Name(name),
        SourceCode(source),
        Requirements(results)
    {
    }
};

std::ostream &operator<<(std::ostream &os, const ALUPoint &value)
{
    return os << value.Name;
}

class ALU : public testing::TestWithParam<ALUPoint>
{
};

TEST_P(ALU, ExecuteCode)
{
    IArmSystemUPtr specimen = createUserModeTestSystem(GetParam().SourceCode.data());

    for (const auto &resultReq : GetParam().Requirements)
    {
        uint32_t initialValue = specimen->getCoreRegister(resultReq.first);

        if (resultReq.first == CoreRegister::CPSR)
        {
            EXPECT_EQ(initialValue, 0x0C000003u);
        }
        else
        {
            EXPECT_EQ(initialValue, 0x00000000u);
        }
    }

    // Put here to ensure that getMode() wasn't labelled as unused.
    EXPECT_EQ(specimen->getMode(), ProcessorMode::Svc26);

    ExecutionMetrics metrics = specimen->run();

    EXPECT_GT(metrics.CycleCount, 1u);

    for (const auto &resultReq : GetParam().Requirements)
    {
        uint32_t finalValue = specimen->getCoreRegister(resultReq.first);

        EXPECT_TRUE(isEqualHex(finalValue, resultReq.second));
    }
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////

ALUPoint testPoints[] = {

// Verify no 'S' suffix leaves status flags unaffected.
{ "MOV-Immediate", "MOV R0,#42", {
    { CoreRegister::R0, 42u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-Immediate-V-Preserved", "TEQP PC,#&10000000 : MOV R0,#42", {
    { CoreRegister::R0, 42u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-Zero", "MOV R2,#0", {
    { CoreRegister::R2, 0u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-Zero-V-Preserved", "TEQP PC,#&10000000 : MOV R2,#0", {
    { CoreRegister::R2, 0u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-NegImmediate", "MOV R3,#0xF0000000", {
    { CoreRegister::R3, 0xF0000000u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-NegImmediate-V-Preserved", "TEQP PC,#&10000000 : MOV R3,#0xF0000000", {
    { CoreRegister::R3, 0xF0000000u },
    { CoreRegister::CPSR, 0x10000000u } } },

// Verify no 'S' suffix ignores carry-out from the barrel shifter.
{ "MOV-LSL-NoCarryOut", "MOV R4,#0xF0000000 : MOV R5,R4,LSL # 1", {
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-LSL-NoCarryOut-V-Preserved", "TEQP PC,#&10000000 : MOV R4,#0xF0000000 : MOV R5,R4,LSL # 1", {
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-LSL-32", "MOV R3, #32 : MOV R4,#1 : MOV R5,R4,LSL R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-LSL-32-V-Preserved", "MOV R3, #32 : MOV R4,#1 : TEQP PC,#&10000000 : MOV R5,R4,LSL R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-LSL-33", "MOV R3, #33 : MOV R4,#1 : MOV R5,R4,LSL R3", {
    { CoreRegister::R3, 33u },
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-LSL-33-V-Preserved", "MOV R3, #33 : MOV R4,#1 : TEQP PC,#&10000000 : MOV R5,R4,LSL R3", {
    { CoreRegister::R3, 33u },
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-LSR-NoCarryOut", "MOV R4,#3 : MOV R5,R4,LSR #1", {
    { CoreRegister::R4, 3u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-LSR-NoCarryOut-V-Preserved", "MOV R4,#3 : TEQP PC,#&10000000 : MOV R5,R4,LSR #1", {
    { CoreRegister::R4, 3u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-LSR-32", "MOV R3, #32 : MOV R4,#0xF0000000 : MOV R5,R4,LSR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-LSR-32-V-Preserved", "MOV R3, #32 : MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOV R5,R4,LSR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-LSR-33", "MOV R3, #33 : MOV R4,#0xF0000000 : MOV R5,R4,LSR R3", {
    { CoreRegister::R3, 33u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-LSR-33-V-Preserved", "MOV R3, #33 : MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOV R5,R4,LSR R3", {
    { CoreRegister::R3, 33u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-ASR-NoCarryOut", "MOV R4,#0xF0000001 : MOV R5,R4,ASR #1", {
    { CoreRegister::R4, 0xF0000001u },
    { CoreRegister::R5, 0xF8000000u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-ASR-NoCarryOut-V-Preserved", "MOV R4,#0xF0000001 : TEQP PC,#&10000000 : MOV R5,R4,ASR #1", {
    { CoreRegister::R4, 0xF0000001u },
    { CoreRegister::R5, 0xF8000000u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-ASR-32", "MOV R3, #32 : MOV R4,#0x80000000 : MOV R5,R4,ASR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0x80000000u },
    { CoreRegister::R5, 0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-ASR-32-V-Preserved", "MOV R3, #32 : MOV R4,#0x80000000 : TEQP PC,#&10000000 : MOV R5,R4,ASR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0x80000000u },
    { CoreRegister::R5, 0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-ROR-NoCarryOut", "MOV R4,#1 : MOV R5,R4,ROR #1", {
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0x80000000u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-ROR-NoCarryOut-V-Preserved", "MOV R4,#1 : TEQP PC,#&10000000 : MOV R5,R4,ROR #1", {
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0x80000000u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-ROR-32", "MOV R3, #32 : MOV R4,#&F0000000 : MOV R5,R4,ROR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xF0000000u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-ROR-32-V-Preserved", "MOV R3, #32 : MOV R4,#&F0000000 : TEQP PC,#&10000000 : MOV R5,R4,ROR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xF0000000u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-ROR-MoreThan32", "MOV R3, #48 : MOV R4,#&F000 : MOV R5,R4,ROR R3", {
    { CoreRegister::R3, 48u },
    { CoreRegister::R4, 0x0000F000u },
    { CoreRegister::R5, 0xF0000000u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-ROR-MoreThan32-V-Preserved", "MOV R3, #48 : MOV R4,#&F000 : TEQP PC,#&10000000 : MOV R5,R4,ROR R3", {
    { CoreRegister::R3, 48u },
    { CoreRegister::R4, 0x0000F000u },
    { CoreRegister::R5, 0xF0000000u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-RRX-NoCarryOut", "MOV R4,#1 : MOV R5,R4,RRX", {
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOV-RRX-NoCarryOut-V-Preserved", "MOV R4,#1 : TEQP PC,#&10000000 : MOV R5,R4,RRX", {
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOV-PreserveStatus", "MOV R3, #0 : MOV R4,#0xF0000000 : MOVS R5,R4,LSL #1 : MOV R6, R3", {
    { CoreRegister::R3, 0u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::R6, 0u },
    { CoreRegister::CPSR, 0xA0000000u } } },

{ "MOV-PreserveStatus-V-Preserved", "MOV R3, #0 : MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOVS R5,R4,LSL #1 : MOV R6, R3", {
    { CoreRegister::R3, 0u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::R6, 0u },
    { CoreRegister::CPSR, 0xB0000000u } } },

// Verify 'S' changes status flags.
{ "MOVS-Immediate", "MOVS R4,#42", {
    { CoreRegister::R4, 42u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOVS-Immediate-V-Preserved", "TEQP PC,#&10000000 : MOVS R4,#42", {
    { CoreRegister::R4, 42u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOVS-NegImmediate", "MOVS R1,#&C0000003", {
    { CoreRegister::R1, 0xC0000003u },
    { CoreRegister::CPSR, 0x80000000u } } },

{ "MOVS-NegImmediate-V-Preserved", "TEQP PC,#&10000000 : MOVS R1,#&C0000003", {
    { CoreRegister::R1, 0xC0000003u },
    { CoreRegister::CPSR, 0x90000000u } } },

{ "MOVS-Zero", "MOVS R11,#0", {
    { CoreRegister::R11, 0x0u },
    { CoreRegister::CPSR, 0x40000000u } } },

{ "MOVS-Zero-V-Preserved", "TEQP PC,#&10000000 : MOVS R11,#0", {
    { CoreRegister::R11, 0x0u },
    { CoreRegister::CPSR, 0x50000000u } } },

// Verify 'S' suffix propagates carry-out from the barrel shifter.
{ "MOVS-LSL-NoCarryOut", "MOV R4,#0x30000000 : MOVS R5,R4,LSL #1", {
    { CoreRegister::R4, 0x30000000u },
    { CoreRegister::R5, 0x60000000u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOVS-LSL-NoCarryOut-V-Preserved", "MOV R4,#0x30000000 : TEQP PC,#&10000000 : MOVS R5,R4,LSL #1", {
    { CoreRegister::R4, 0x30000000u },
    { CoreRegister::R5, 0x60000000u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOVS-LSL-CarryOut", "MOV R4,#0xF0000000 : MOVS R5,R4,LSL #1", {
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::CPSR, 0xA0000000u } } },

{ "MOVS-LSL-CarryOut-V-Preserved", "MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOVS R5,R4,LSL #1", {
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::CPSR, 0xB0000000u } } },

{ "MOVS-LSL-Immed-0", "MOV R3, #0 : MOV R4,#0xF0000000 : MOVS R5,R4,LSL # 1 : MOVS R6, R3, LSL #0", {
    { CoreRegister::R3, 0u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::R6, 0u },
    { CoreRegister::CPSR, 0x60000000u } } },

{ "MOVS-LSL-Immed-0-V-Preserved",
  "MOV R3, #0 : MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOVS R5,R4,LSL # 1 : MOVS R6, R3, LSL #0", {
    { CoreRegister::R3, 0u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::R6, 0u },
    { CoreRegister::CPSR, 0x70000000u } } },

{ "MOVS-LSL-32", "MOV R3, #32 : MOV R4,#1 : MOVS R5,R4,LSL R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x60000000u } } },

{ "MOVS-LSL-32-V-Preserved", "MOV R3, #32 : MOV R4,#1 : TEQP PC,#&10000000 : MOVS R5,R4,LSL R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x70000000u } } },

{ "MOVS-LSL-33", "MOV R3, #33 : MOV R4,#1 : MOVS R5,R4,LSL R3", {
    { CoreRegister::R3, 33u },
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x40000000u } } },

{ "MOVS-LSL-33-V-Preserved", "MOV R3, #33 : MOV R4,#1 : TEQP PC,#&10000000 : MOVS R5,R4,LSL R3", {
    { CoreRegister::R3, 33u },
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x50000000u } } },

{ "MOVS-LSR-NoCarryOut", "MOV R4,#0x2 : MOVS R5,R4,LSR #1", {
    { CoreRegister::R4, 2u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOVS-LSR-NoCarryOut-V-Preserved", "MOV R4,#0x2 : TEQP PC,#&10000000 : MOVS R5,R4,LSR #1", {
    { CoreRegister::R4, 2u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOVS-LSR-CarryOut", "MOV R4,#3 : MOVS R5,R4,LSR #1", {
    { CoreRegister::R4, 3u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x20000000u } } },

{ "MOVS-LSR-CarryOut-V-Preserved", "MOV R4,#3 : TEQP PC,#&10000000 : MOVS R5,R4,LSR #1", {
    { CoreRegister::R4, 3u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x30000000u } } },

{ "MOVS-LSR-32", "MOV R3, #32 : MOV R4,#0xF0000000 : MOVS R5,R4,LSR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x60000000u } } },

{ "MOVS-LSR-32-V-Preserved", "MOV R3, #32 : MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOVS R5,R4,LSR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x70000000u } } },

{ "MOVS-LSR-32-Immed", "MOV R4,#0xF0000000 : MOVS R5,R4,LSR #32", {
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x60000000u } } },

{ "MOVS-LSR-32-Immed-V-Preserved",
  "MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOVS R5,R4,LSR #32", {
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x70000000u } } },

{ "MOVS-LSR-33", "MOV R3, #33 : MOV R4,#0xF0000000 : MOVS R5,R4,LSR R3", {
    { CoreRegister::R3, 33u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x40000000u } } },

{ "MOVS-LSR-33-V-Preserved",
  "MOV R3, #33 : MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOVS R5,R4,LSR R3", {
    { CoreRegister::R3, 33u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x50000000u } } },

{ "MOVS-ASR-NoCarryOut", "MOV R4,#0xF0000000 : MOVS R5,R4,ASR #1", {
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xF8000000u },
    { CoreRegister::CPSR, 0x80000000u } } },

{ "MOVS-ASR-NoCarryOut-V-Preserved", "MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOVS R5,R4,ASR #1", {
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xF8000000u },
    { CoreRegister::CPSR, 0x90000000u } } },

{ "MOVS-ASR-CarryOut", "MOV R4,#0xF0000001 : MOVS R5,R4,ASR #1", {
    { CoreRegister::R4, 0xF0000001u },
    { CoreRegister::R5, 0xF8000000u },
    { CoreRegister::CPSR, 0xA0000000u } } },

{ "MOVS-ASR-CarryOut-V-Preserved", "MOV R4,#0xF0000001 : TEQP PC,#&10000000 : MOVS R5,R4,ASR #1", {
    { CoreRegister::R4, 0xF0000001u },
    { CoreRegister::R5, 0xF8000000u },
    { CoreRegister::CPSR, 0xB0000000u } } },

{ "MOVS-ASR-32", "MOV R3, #32 : MOV R4,#0x80000000 : MOVS R5,R4,ASR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0x80000000u },
    { CoreRegister::R5, 0xFFFFFFFFu },
    { CoreRegister::CPSR, 0xA0000000u } } },

{ "MOVS-ASR-32-V-Preserved",
  "MOV R3, #32 : MOV R4,#0x80000000 : TEQP PC,#&10000000 : MOVS R5,R4,ASR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0x80000000u },
    { CoreRegister::R5, 0xFFFFFFFFu },
    { CoreRegister::CPSR, 0xB0000000u } } },

{ "MOVS-ASR-32-Immed", "MOV R4,#0x80000000 : MOVS R5,R4,ASR #32", {
    { CoreRegister::R4, 0x80000000u },
    { CoreRegister::R5, 0xFFFFFFFFu },
    { CoreRegister::CPSR, 0xA0000000u } } },

{ "MOVS-ASR-32-Immed-V-Preserved", "MOV R4,#0x80000000 : TEQP PC,#&10000000 : MOVS R5,R4,ASR #32", {
    { CoreRegister::R4, 0x80000000u },
    { CoreRegister::R5, 0xFFFFFFFFu },
    { CoreRegister::CPSR, 0xB0000000u } } },

{ "MOVS-ROR-NoCarryOut", "MOV R4,#2 : MOVS R5,R4,ROR #1", {
    { CoreRegister::R4, 2u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOVS-ROR-NoCarryOut-V-Preserved", "MOV R4,#2 : TEQP PC,#&10000000 : MOVS R5,R4,ROR #1", {
    { CoreRegister::R4, 2u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOVS-ROR-CarryOut", "MOV R4,#1 : MOVS R5,R4,ROR #1", {
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0x80000000u },
    { CoreRegister::CPSR, 0xA0000000u } } },

{ "MOVS-ROR-CarryOut-V-Preserved", "MOV R4,#1 : TEQP PC,#&10000000 : MOVS R5,R4,ROR #1", {
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0x80000000u },
    { CoreRegister::CPSR, 0xB0000000u } } },

{ "MOVS-ROR-32", "MOV R3, #32 : MOV R4,#&F0000000 : MOVS R5,R4,ROR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xF0000000u },
    { CoreRegister::CPSR, 0xA0000000u } } },

{ "MOVS-ROR-32-V-Preserved",
  "MOV R3, #32 : MOV R4,#&F0000000 : TEQP PC,#&10000000 : MOVS R5,R4,ROR R3", {
    { CoreRegister::R3, 32u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xF0000000u },
    { CoreRegister::CPSR, 0xB0000000u } } },

{ "MOVS-ROR-MoreThan32", "MOV R3, #48 : MOV R4,#&F000 : MOVS R5,R4,ROR R3", {
    { CoreRegister::R3, 48u },
    { CoreRegister::R4, 0x0000F000u },
    { CoreRegister::R5, 0xF0000000u },
    { CoreRegister::CPSR, 0xA0000000u } } },

{ "MOVS-ROR-MoreThan32-V-Preserved",
  "MOV R3, #48 : MOV R4,#&F000 : TEQP PC,#&10000000 : MOVS R5,R4,ROR R3", {
    { CoreRegister::R3, 48u },
    { CoreRegister::R4, 0x0000F000u },
    { CoreRegister::R5, 0xF0000000u },
    { CoreRegister::CPSR, 0xB0000000u } } },

{ "MOVS-RRX-NoCarryOut", "MOV R4,#2 : MOVS R5,R4,RRX", {
    { CoreRegister::R4, 2u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x00000000u } } },

{ "MOVS-RRX-NoCarryOut-V-Preserved", "MOV R4,#2 : TEQP PC,#&10000000 : MOVS R5,R4,RRX", {
    { CoreRegister::R4, 2u },
    { CoreRegister::R5, 1u },
    { CoreRegister::CPSR, 0x10000000u } } },

{ "MOVS-RRX-CarryOut", "MOV R4,#1 : MOVS R5,R4,RRX", {
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x60000000u } } },

{ "MOVS-RRX-CarryOut-V-Preserved", "MOV R4,#1 : TEQP PC,#&10000000 : MOVS R5,R4,RRX", {
    { CoreRegister::R4, 1u },
    { CoreRegister::R5, 0u },
    { CoreRegister::CPSR, 0x70000000u } } },

{ "MOVS-PreserveCarry", "MOV R3, #0 : MOV R4,#0xF0000000 : MOVS R5,R4,LSL # 1 : MOVS R6, R3", {
    { CoreRegister::R3, 0u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::R6, 0u },
    { CoreRegister::CPSR, 0x60000000u } } },

{ "MOVS-PreserveCarry-V-Preserved",
  "MOV R3, #0 : MOV R4,#0xF0000000 : TEQP PC,#&10000000 : MOVS R5,R4,LSL # 1 : MOVS R6, R3", {
    { CoreRegister::R3, 0u },
    { CoreRegister::R4, 0xF0000000u },
    { CoreRegister::R5, 0xE0000000u },
    { CoreRegister::R6, 0u },
    { CoreRegister::CPSR, 0x70000000u } } },

{ "AND-NoFlags", "MOV R9,#0xFF : MOV R10,#0xFF0 : AND R11, R9, R10", {
    { CoreRegister::R9, 0xFFu },
    { CoreRegister::R10, 0xFF0u },
    { CoreRegister::R11, 0xF0u },
    { CoreRegister::CPSR, 0x00000000u }
}},

{ "ANDS-NoFlags", "MOV R9,#0xFF : MOV R10,#0xFF0 : ANDS R11, R9, R10", {
    { CoreRegister::R9, 0xFFu },
    { CoreRegister::R10, 0xFF0u },
    { CoreRegister::R11, 0xF0u },
    { CoreRegister::CPSR, 0x00000000u }
}},

{ "AND-Flags", "MOV R9,#0xFF000000 : MOV R10,#0xF000000F : AND R11, R9, R10", {
    { CoreRegister::R9, 0xFF000000u },
    { CoreRegister::R10, 0xF000000Fu },
    { CoreRegister::R11, 0xF0000000u },
    { CoreRegister::CPSR, 0x00000000u }
}},

{ "ANDS-Flags", "MOV R9,#0xFF000000 : MOV R10,#0xF000000F : ANDS R11, R9, R10", {
    { CoreRegister::R9, 0xFF000000u },
    { CoreRegister::R10, 0xF000000Fu },
    { CoreRegister::R11, 0xF0000000u },
    { CoreRegister::CPSR, 0x80000000u }
}},

{ "EOR-NoFlags", "MOV R9,#0xFF : MOV R10,#0xFF0 : EOR R11, R9, R10", {
    { CoreRegister::R9, 0xFFu },
    { CoreRegister::R10, 0xFF0u },
    { CoreRegister::R11, 0xF0Fu },
    { CoreRegister::CPSR, 0x00000000u }
}},

{ "EORS-NoFlags", "MOV R9,#0xFF : MOV R10,#0xFF0 : EORS R11, R9, R10", {
    { CoreRegister::R9, 0xFFu },
    { CoreRegister::R10, 0xFF0u },
    { CoreRegister::R11, 0xF0Fu },
    { CoreRegister::CPSR, 0x00000000u }
}},

{ "EOR-Flags", "MOV R9,#0xFF : EOR R10, R9, R9", {
    { CoreRegister::R9, 0xFFu },
    { CoreRegister::R10, 0x0u },
    { CoreRegister::CPSR, 0x00000000u }
}},

{ "EORS-Flags", "MOV R9,#0xFF : EORS R10, R9, R9", {
    { CoreRegister::R9, 0xFFu },
    { CoreRegister::R10, 0x0u },
    { CoreRegister::CPSR, 0x40000000u }
}},

{ "TST-Flags", "MOV R9,#0xFF : MOV R10,#0xFF00 : TST R10, R9", {
    { CoreRegister::R9, 0xFFu },
    { CoreRegister::R10, 0xFF00u },
    { CoreRegister::CPSR, 0x40000000u }
}},

{ "TSTP-Flags", "MOV R0,PC : ORR R0,R0,#&F0000000 : TSTP R0,#&AC000003", {
    { CoreRegister::R0, 0xF000800Cu },
    { CoreRegister::CPSR, 0xA0000000u }
}},

{ "TEQ-Flags", "MOV R9,#0xFF : MOV R10,#0x80000000 : TEQ R10, R9", {
    { CoreRegister::R9, 0xFFu },
    { CoreRegister::R10, 0x80000000u },
    { CoreRegister::CPSR, 0x80000000u }
} },

{ "TEQP-Flags", "MOV R1,#&F0000000 : MOVS R0,#0 : TEQP R1,PC", {
    { CoreRegister::R0, 0x00u },
    { CoreRegister::R1, 0xF0000000u },
    { CoreRegister::CPSR, 0xB0000000u }
} },

{ "ORR-NoFlags", "MOV R9,#0xFF : MOV R10,#0xFF000 : ORR R11, R9, R10", {
    { CoreRegister::R9,   0x000000FFu },
    { CoreRegister::R10,  0x000FF000u },
    { CoreRegister::R11,  0x000FF0FFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ORRS-NoFlags", "MOV R9,#0xFF : MOV R10,#0xFF000 : ORRS R11, R9, R10", {
    { CoreRegister::R9,   0x000000FFu },
    { CoreRegister::R10,  0x000FF000u },
    { CoreRegister::R11,  0x000FF0FFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ORR-Flags", "MOV R9,#0xFF000000 : MOV R10,#0xF000000F : ORR R11, R9, R10", {
    { CoreRegister::R9,   0xFF000000u },
    { CoreRegister::R10,  0xF000000Fu },
    { CoreRegister::R11,  0xFF00000Fu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ORRS-Flags", "MOV R9,#0xFF000000 : MOV R10,#0xF000000F : ORRS R11, R9, R10", {
    { CoreRegister::R9,   0xFF000000u },
    { CoreRegister::R10,  0xF000000Fu },
    { CoreRegister::R11,  0xFF00000Fu },
    { CoreRegister::CPSR, 0x80000000u }
} },

{ "BIC-NoFlags", "MOV R9,#0xFF : MOV R10,#0xFF0 : BIC R11, R9, R10", {
    { CoreRegister::R9,   0x000000FFu },
    { CoreRegister::R10,  0x00000FF0u },
    { CoreRegister::R11,  0x0000000Fu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "BICS-NoFlags", "MOV R9,#0xFF : MOV R10,#0xFF0 : BIC R11, R9, R10", {
    { CoreRegister::R9,   0x000000FFu },
    { CoreRegister::R10,  0x00000FF0u },
    { CoreRegister::R11,  0x0000000Fu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "BIC-Flags", "MOV R9,#0xFF000000 : MOV R10,#0xF000000F : BIC R11, R9, R10", {
    { CoreRegister::R9,   0xFF000000u },
    { CoreRegister::R10,  0xF000000Fu },
    { CoreRegister::R11,  0x0F000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "BICS-Flags", "MOV R9,#0xF000000F : BICS R10, R9, R9", {
    { CoreRegister::R9,   0xF000000Fu },
    { CoreRegister::R10,  0x00000000u },
    { CoreRegister::CPSR, 0x40000000u }
} },

{ "MVN-NoFlags", "MVN R9,#0xFF000000", {
    { CoreRegister::R9,   0x00FFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "MVNS-NoFlags", "MVNS R9,#0xFF000000", {
    { CoreRegister::R9,   0x00FFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "MVN-Flags", "MVN R9,#0x000000FF", {
    { CoreRegister::R9,   0xFFFFFF00u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "MVNS-Flags", "MVNS R9,#0x000000FF", {
    { CoreRegister::R9,   0xFFFFFF00u },
    { CoreRegister::CPSR, 0x80000000u }
} },

{ "ADD-NoFlags", "MOV R0,#1 : ADD R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000002u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADD-Overflow", "MVN R0,#&80000000 : ADD R1,R0,#1", {
    { CoreRegister::R0,   0x7FFFFFFFu },
    { CoreRegister::R1,   0x80000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADD-Carry", "MVN R0,#0 : ADD R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADD-Zero", "MVN R0,#0 : ADD R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADD-Negative", "MVN R0,#1 : ADD R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFEu },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADDS-NoFlags", "MOV R0,#1 : ADDS R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000002u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADDS-Overflow", "MVN R0,#&80000000 : ADDS R1,R0,#1", {
    { CoreRegister::R0,   0x7FFFFFFFu },
    { CoreRegister::R1,   0x80000000u },
    { CoreRegister::CPSR, 0x90000000u }
} },

{ "ADDS-Carry", "MVN R0,#0 : ADDS R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x60000000u }
} },

{ "ADDS-Zero", "MVN R0,#0 : ADDS R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x60000000u }
} },

{ "ADDS-Negative", "MVN R0,#1 : ADDS R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFEu },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x80000000u }
} },

// ADC
{ "ADC-NoFlagsCC", "MOV R0,#1 : ADC R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000002u },
    { CoreRegister::CPSR, 0x00000000u }
}},

{ "ADC-NoFlagsCS", "MOV R0,#1 : MOVS R1,R0,RRX : ADC R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000003u },
    { CoreRegister::CPSR, 0x60000000u }
} },

{ "ADC-Overflow", "MVN R0,#&80000000 : ADC R1,R0,#1", {
    { CoreRegister::R0,   0x7FFFFFFFu },
    { CoreRegister::R1,   0x80000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADC-Carry", "MVN R0,#0 : ADC R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADC-Zero", "MVN R0,#0 : ADC R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADC-Negative", "MVN R0,#1 : ADC R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFEu },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADCS-NoFlagsCC", "MOV R0,#1 : ADCS R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000002u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADCS-NoFlagsCS", "MOV R0,#1 : MOVS R1,R0,RRX : ADCS R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000003u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "ADCS-Overflow", "MVN R0,#&80000000 : ADCS R1,R0,#1", {
    { CoreRegister::R0,   0x7FFFFFFFu },
    { CoreRegister::R1,   0x80000000u },
    { CoreRegister::CPSR, 0x90000000u }
} },

{ "ADCS-Carry", "MVN R0,#0 : ADCS R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x60000000u }
} },

{ "ADCS-Zero", "MVN R0,#0 : ADCS R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x60000000u }
} },

{ "ADCS-Negative", "MVN R0,#1 : ADCS R1,R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFEu },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x80000000u }
} },

// SUB
{ "SUB-NoFlags", "MOV R0,#2 : SUB R1,R0,#1", {
    { CoreRegister::R0,   0x00000002u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SUB-Overflow", "MOV R0,#&80000000 : SUB R1,R0,#1", {
    { CoreRegister::R0,   0x80000000u },
    { CoreRegister::R1,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SUB-Carry", "MVN R0,#&0 : MOV R1,#0 : SUB R2,R1,R0", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::R2,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SUB-Zero", "MOV R0,#1 : SUB R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SUB-Negative", "MOV R0,#0 : SUB R1,R0,#1", {
    { CoreRegister::R0,   0x00000000u },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },


{ "SUBS-NoFlags", "MOV R0,#2 : SUBS R1,R0,#1", {
    { CoreRegister::R0,   0x00000002u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SUBS-Overflow", "MOV R0,#&80000000 : SUBS R1,R0,#1", {
    { CoreRegister::R0,   0x80000000u },
    { CoreRegister::R1,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x10000000u }
} },

{ "SUBS-Carry", "MVN R0,#&0 : MOV R1,#0 : SUBS R2,R1,R0", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::R2,   0x00000001u },
    { CoreRegister::CPSR, 0x20000000u }
} },

{ "SUBS-Zero", "MOV R0,#1 : SUBS R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x40000000u }
} },

{ "SUBS-Negative", "MOV R0,#0 : SUBS R1,R0,#1", {
    { CoreRegister::R0,   0x00000000u },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0xA0000000u }
} },
 
// RSB
{ "RSB-NoFlags", "MOV R0,#1 : RSB R1,R0,#2", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSB-Overflow", "MOV R0,#1 : RSB R1,R0,#&80000000", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSB-Carry", "MVN R0,#&0 : MOV R1,#0 : RSB R2,R0,R1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::R2,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSB-Zero", "MOV R0,#1 : RSB R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSB-Negative", "MOV R0,#1 : RSB R1,R0,#0", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSBS-NoFlags", "MOV R0,#1 : RSBS R1,R0,#2", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSBS-Overflow", "MOV R0,#1 : RSBS R1,R0,#&80000000", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x10000000u }
} },

{ "RSBS-Carry", "MVN R0,#&0 : MOV R1,#0 : RSBS R2,R0,R1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::R2,   0x00000001u },
    { CoreRegister::CPSR, 0x20000000u }
} },

{ "RSBS-Zero", "MOV R0,#1 : RSBS R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x40000000u }
} },

{ "RSBS-Negative", "MOV R0,#1 : RSBS R1,R0,#0", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0xA0000000u }
} },
 
// SBC
{ "SBC-NoFlagsCC", "MOV R0,#2 : SBC R1,R0,#1", {
    { CoreRegister::R0,   0x00000002u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SBC-NoFlagsCS", "MOV R0,#3 : MOV R1,#3 : MOVS R1,R1,RRX : SBC R1,R0,#1", {
    { CoreRegister::R0,   0x00000003u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x20000000u }
} },

{ "SBC-Overflow", "MOV R0,#&80000000 : SBC R1,R0,#1", {
    { CoreRegister::R0,   0x80000000u },
    { CoreRegister::R1,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SBC-Carry", "MVN R0,#&0 : MOV R1,#0 : SBC R2,R1,R0", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::R2,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SBC-Zero", "MOV R0,#1 : SBC R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SBC-Negative", "MOV R0,#0 : SBC R1,R0,#1", {
    { CoreRegister::R0,   0x00000000u },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SBCS-NoFlagsCC", "MOV R0,#2 : SBCS R1,R0,#1", {
    { CoreRegister::R0,   0x00000002u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SBCS-NoFlagsCS", "MOV R0,#3 : MOV R1,#3 : MOVS R1,R1,RRX : SBCS R1,R0,#1", {
    { CoreRegister::R0,   0x00000003u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "SBCS-Overflow", "MOV R0,#&80000000 : SBCS R1,R0,#1", {
    { CoreRegister::R0,   0x80000000u },
    { CoreRegister::R1,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x10000000u }
} },

{ "SBCS-Carry", "MVN R0,#&0 : MOV R1,#0 : SBCS R2,R1,R0", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::R2,   0x00000001u },
    { CoreRegister::CPSR, 0x20000000u }
} },

{ "SBCS-Zero", "MOV R0,#1 : SBCS R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x40000000u }
} },

{ "SBCS-Negative", "MOV R0,#0 : SBCS R1,R0,#1", {
    { CoreRegister::R0,   0x00000000u },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0xA0000000u }
} },


// RSC
{ "RSC-NoFlagsCC", "MOV R0,#1 : RSC R1,R0,#2", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSC-NoFlagsCS", "MOV R0,#1 : MOV R1,#3 : MOVS R1,R1,RRX : RSC R1,R0,#3", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x20000000u }
} },

{ "RSC-Overflow", "MOV R0,#1 : RSC R1,R0,#&80000000", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSC-Carry", "MVN R0,#&0 : MOV R1,#0 : RSC R2,R0,R1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::R2,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSC-Zero", "MOV R0,#1 : RSC R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSC-Negative", "MOV R0,#1 : RSC R1,R0,#0", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSCS-NoFlagsCC", "MOV R0,#1 : RSCS R1,R0,#2", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSCS-NoFlagsCS", "MOV R0,#1 : MOV R1,#3 : MOVS R1,R1,RRX : RSCS R1,R0,#3", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "RSCS-Overflow", "MOV R0,#1 : RSCS R1,R0,#&80000000", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x10000000u }
} },

{ "RSCS-Carry", "MVN R0,#&0 : MOV R1,#0 : RSCS R2,R0,R1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::R2,   0x00000001u },
    { CoreRegister::CPSR, 0x20000000u }
} },

{ "RSCS-Zero", "MOV R0,#1 : RSCS R1,R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x40000000u }
} },

{ "RSCS-Negative", "MOV R0,#1 : RSCS R1,R0,#0", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::R1,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0xA0000000u }
} },


// CMP
{ "CMP-NoFlags", "MOV R0,#2 : CMP R0,#1", {
    { CoreRegister::R0,   0x00000002u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "CMP-Overflow", "MOV R0,#&80000000 : CMP R0,#1", {
    { CoreRegister::R0,   0x80000000u },
    { CoreRegister::CPSR, 0x10000000u }
} },

{ "CMP-Carry", "MVN R0,#&0 : MOV R1,#0 : CMP R1,R0", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::R1,   0x00000000u },
    { CoreRegister::CPSR, 0x20000000u }
} },

{ "CMP-Zero", "MOV R0,#1 : CMP R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::CPSR, 0x40000000u }
} },

{ "CMP-Negative", "MOV R0,#0 : CMP R0,#1", {
    { CoreRegister::R0,   0x00000000u },
    { CoreRegister::CPSR, 0xA0000000u }
} },

{ "CMPP-NoFlags", "MOV R0,#&0 : CMPP R0,#&0", {
    { CoreRegister::R0,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "CMPP-Flags", "MOV R0,#&F0000000 : CMPP R0,#&50000000", {
    { CoreRegister::R0,   0xF0000000u },
    { CoreRegister::CPSR, 0xA0000000u }
} },

// CMN
{ "CMN-NoFlags", "MOV R0,#1 : CMN R0,#1", {
    { CoreRegister::R0,   0x00000001u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "CMN-Overflow", "MVN R0,#&80000000 : CMN R0,#1", {
    { CoreRegister::R0,   0x7FFFFFFFu },
    { CoreRegister::CPSR, 0x90000000u }
} },

{ "CMN-Carry", "MVN R0,#0 : CMN R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x60000000u }
} },

{ "CMN-Zero", "MVN R0,#0 : CMN R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFFu },
    { CoreRegister::CPSR, 0x60000000u }
} },

{ "CMN-Negative", "MVN R0,#1 : CMN R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFEu },
    { CoreRegister::CPSR, 0x80000000u }
} },

{ "CMNS-Negative", "MVN R0,#1 : CMN R0,#1", {
    { CoreRegister::R0,   0xFFFFFFFEu },
    { CoreRegister::CPSR, 0x80000000u }
} },

{ "CMNP-NoFlags", "MOV R0,#0 : CMNP R0,#0", {
    { CoreRegister::R0,   0x00000000u },
    { CoreRegister::CPSR, 0x00000000u }
} },

{ "CMNP-Flags", "MOV R0,#&10000000 : CMNP R0,#&80000000", {
    { CoreRegister::R0,   0x10000000u },
    { CoreRegister::CPSR, 0x90000000u }
} },

};

INSTANTIATE_TEST_SUITE_P(CPU, ALU, testing::ValuesIn(testPoints));

} // Anonymous namespace.

}} // namespace Ag::Arm

////////////////////////////////////////////////////////////////////////////////


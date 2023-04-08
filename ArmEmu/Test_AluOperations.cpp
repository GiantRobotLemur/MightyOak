//! @file Test_AluOperations.cpp
//! @brief The definition of unit tests for the implementation of optimised ALU
//! operations.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "AluOperations.h"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct AluOpParams
{
public:
    const char *OpName;
    uint32_t Op1;
    uint32_t Op2;
    uint32_t ExpectedResult;
    uint8_t ExpectedFlags;

    AluOpParams(const char *opName, uint32_t op1, uint32_t op2, uint32_t result,
                uint8_t flags) :
        OpName(opName),
        Op1(op1),
        Op2(op2),
        ExpectedResult(result),
        ExpectedFlags(flags)
    {
    }
};

struct AluOpParams64
{
public:
    const char *OpName;
    uint32_t Op1;
    uint32_t Op2;
    uint64_t ExpectedResult;
    uint8_t ExpectedFlags;

    AluOpParams64(const char *opName, uint32_t op1, uint32_t op2, uint64_t result,
                  uint8_t flags) :
        OpName(opName),
        Op1(op1),
        Op2(op2),
        ExpectedResult(result),
        ExpectedFlags(flags)
    {
    }
};

struct AluFlagOpParams : public AluOpParams
{
    uint8_t InitialFlags;

    AluFlagOpParams(const char *opName, uint32_t op1, uint32_t op2,
                    uint8_t initFlags, uint32_t result, uint8_t resultFlags) :
        AluOpParams(opName, op1, op2, result, resultFlags),
        InitialFlags(initFlags)
    {
    }
};

class AddOperation : public testing::TestWithParam<AluOpParams> { };
class SubOperation : public testing::TestWithParam<AluOpParams> { };
class AdcOperation : public testing::TestWithParam<AluFlagOpParams> { };
class SbcOperation : public testing::TestWithParam<AluFlagOpParams> { };
class RscOperation : public testing::TestWithParam<AluFlagOpParams> { };
////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////

AluOpParams addTestParams[] = {
    AluOpParams("Zeros", 0, 0, 0, StatusFlag_Z),
    AluOpParams("NoFlags", 1, 2, 3, StatusFlag_None),
    AluOpParams("Overflow", 0x7FFFFFFF, 1, 0x80000000, StatusFlag_V | StatusFlag_N),
    AluOpParams("Carry", 0xFFFFFFFF, 1, 0, StatusFlag_Z | StatusFlag_C),
    AluOpParams("Zero", 0x8000, 0xFFFF8000, 0, StatusFlag_Z | StatusFlag_C),
    AluOpParams("Negative", 0xFFFFFFFE, 1, 0xFFFFFFFF, StatusFlag_N),
};

TEST_P(AddOperation, Add)
{
    const AluOpParams &data = GetParam();

    uint8_t flags = StatusFlag_Invalid;
    uint32_t result = ALU_Add(data.Op1, data.Op2, flags);

    EXPECT_EQ(result, data.ExpectedResult);
    EXPECT_EQ(flags & StatusFlag_Invalid, 0);
    EXPECT_EQ(flags & StatusFlag_Mask, data.ExpectedFlags);
}

INSTANTIATE_TEST_SUITE_P(ALU_Add, AddOperation, ::testing::ValuesIn(addTestParams),
                         [](const ::testing::TestParamInfo<AluOpParams> &test) {
                             // Can use test.param here to generate the test suffix
                             return test.param.OpName;
                         });

AluOpParams subTestParams[] = {
    AluOpParams("Zeros", 0, 0, 0, StatusFlag_Z),
    AluOpParams("NoFlags", 2, 1, 1, StatusFlag_None),
    AluOpParams("Overflow", 0x80000000, 1, 0x7FFFFFFF, StatusFlag_V),
    AluOpParams("Carry", 0, 0xFFFFFFFF, 1, StatusFlag_C),
    AluOpParams("Zero", 1, 1, 0, StatusFlag_Z),
    AluOpParams("Negative", 0, 1, 0xFFFFFFFF, StatusFlag_N | StatusFlag_C),
};

TEST_P(SubOperation, Sub)
{
    const AluOpParams &data = GetParam();

    uint8_t flags = StatusFlag_Invalid;
    uint32_t result = ALU_Sub(data.Op1, data.Op2, flags);

    EXPECT_EQ(result, data.ExpectedResult);
    EXPECT_EQ(flags & StatusFlag_Invalid, 0);
    EXPECT_EQ(flags & StatusFlag_Mask, data.ExpectedFlags);
}

INSTANTIATE_TEST_SUITE_P(ALU_Sub, SubOperation, ::testing::ValuesIn(subTestParams),
                         [](const ::testing::TestParamInfo<AluOpParams> &test) {
                             // Can use test.param here to generate the test suffix
                             return test.param.OpName;
                         });

AluFlagOpParams adcTestParams[] = {
    AluFlagOpParams("NoFlagsCC", 1, 1, StatusFlag_None, 2, StatusFlag_None),
    AluFlagOpParams("NoFlagsCS", 1, 1, StatusFlag_C, 3, StatusFlag_None),
    AluFlagOpParams("OverflowCC", 0x7FFFFFFF, 1, StatusFlag_None, 0x80000000, StatusFlag_N | StatusFlag_V),
    AluFlagOpParams("OverflowCS", 0x7FFFFFFF, 1, StatusFlag_C, 0x80000001, StatusFlag_N | StatusFlag_V),
    AluFlagOpParams("CarryCC", 0xFFFFFFFF, 1, StatusFlag_None, 0, StatusFlag_C | StatusFlag_Z),
    AluFlagOpParams("CarryCS", 0xFFFFFFFE, 1, StatusFlag_C, 0, StatusFlag_C | StatusFlag_Z),
    AluFlagOpParams("ZeroCC", 0xFFFFFFFF, 1, StatusFlag_None, 0, StatusFlag_Z | StatusFlag_C),
    AluFlagOpParams("ZeroCS", 0xFFFFFFFF, 0, StatusFlag_C, 0, StatusFlag_Z | StatusFlag_C),
    AluFlagOpParams("NegativeCC", 0xFFFFFFFE, 1, StatusFlag_None, 0xFFFFFFFF, StatusFlag_N),
    AluFlagOpParams("NegativeCS", 0xFFFFFFFD, 1, StatusFlag_C, 0xFFFFFFFF, StatusFlag_N),
};

TEST_P(AdcOperation, Adc)
{
    const AluFlagOpParams &data = GetParam();

    uint8_t flags = data.InitialFlags;
    uint32_t result = ALU_Adc(data.Op1, data.Op2, flags);

    EXPECT_EQ(result, data.ExpectedResult);
    EXPECT_EQ(flags & StatusFlag_Invalid, 0);
    EXPECT_EQ(flags & StatusFlag_Mask, data.ExpectedFlags);
}

INSTANTIATE_TEST_SUITE_P(ALU_Adc, AdcOperation, ::testing::ValuesIn(adcTestParams),
                         [](const ::testing::TestParamInfo<AluFlagOpParams> &test) {
                             // Can use test.param here to generate the test suffix
                             return test.param.OpName;
                         });

AluFlagOpParams sbcTestParams[] = {
    AluFlagOpParams("NoFlagsCC", 2, 1, StatusFlag_None, 1, StatusFlag_None),
    AluFlagOpParams("NoFlagsCS", 3, 1, StatusFlag_C, 1, StatusFlag_None),
    AluFlagOpParams("OverflowCC", 0x80000000, 1, StatusFlag_None, 0x7FFFFFFF, StatusFlag_V),
    AluFlagOpParams("OverflowCS", 0x80000000, 1, StatusFlag_C, 0x7FFFFFFE, StatusFlag_V),
    AluFlagOpParams("CarryCC", 0, 0xFFFFFFFF, StatusFlag_None, 1, StatusFlag_C),
    AluFlagOpParams("CarryCS", 0, 0xFFFFFFFE, StatusFlag_C, 1, StatusFlag_C),
    AluFlagOpParams("ZeroCC", 1, 1, StatusFlag_None, 0, StatusFlag_Z),
    AluFlagOpParams("ZeroCS", 2, 1, StatusFlag_C, 0, StatusFlag_Z),
    AluFlagOpParams("NegativeCC", 0, 1, StatusFlag_None, 0xFFFFFFFF, StatusFlag_N | StatusFlag_C),
    AluFlagOpParams("NegativeCS", 0, 1, StatusFlag_C, 0xFFFFFFFE, StatusFlag_N | StatusFlag_C),
};

TEST_P(SbcOperation, Sbc)
{
    const AluFlagOpParams &data = GetParam();

    uint8_t flags = data.InitialFlags;
    uint32_t result = ALU_Sbc(data.Op1, data.Op2, flags);

    EXPECT_EQ(result, data.ExpectedResult);
    EXPECT_EQ(flags & StatusFlag_Invalid, 0);
    EXPECT_EQ(flags & StatusFlag_Mask, data.ExpectedFlags);
}

INSTANTIATE_TEST_SUITE_P(ALU_Sbc, SbcOperation, ::testing::ValuesIn(sbcTestParams),
                         [](const ::testing::TestParamInfo<AluFlagOpParams> &test) {
                             // Can use test.param here to generate the test suffix
                             return test.param.OpName;
                         });


AluFlagOpParams rscTestParams[] = {
    AluFlagOpParams("NoFlagsCC", 1, 2, StatusFlag_None, 1, StatusFlag_None),
    AluFlagOpParams("NoFlagsCS", 1, 3, StatusFlag_C, 1, StatusFlag_None),
    AluFlagOpParams("OverflowCC", 1, 0x80000000, StatusFlag_None, 0x7FFFFFFF, StatusFlag_V),
    AluFlagOpParams("OverflowCS", 1, 0x80000000, StatusFlag_C, 0x7FFFFFFE, StatusFlag_V),
    AluFlagOpParams("CarryCC", 0xFFFFFFFF, 0, StatusFlag_None, 1, StatusFlag_C),
    AluFlagOpParams("CarryCS", 0xFFFFFFFE, 0, StatusFlag_C, 1, StatusFlag_C),
    AluFlagOpParams("ZeroCC", 1, 1, StatusFlag_None, 0, StatusFlag_Z),
    AluFlagOpParams("ZeroCS", 1, 2, StatusFlag_C, 0, StatusFlag_Z),
    AluFlagOpParams("NegativeCC", 1, 0, StatusFlag_None, 0xFFFFFFFF, StatusFlag_N | StatusFlag_C),
    AluFlagOpParams("NegativeCS", 1, 0, StatusFlag_C, 0xFFFFFFFE, StatusFlag_N | StatusFlag_C),
};

TEST_P(RscOperation, Rsc)
{
    const AluFlagOpParams &data = GetParam();

    uint8_t flags = data.InitialFlags;
    uint32_t result = ALU_Rsc(data.Op1, data.Op2, flags);

    EXPECT_EQ(result, data.ExpectedResult);
    EXPECT_EQ(flags & StatusFlag_Invalid, 0);
    EXPECT_EQ(flags & StatusFlag_Mask, data.ExpectedFlags);
}

INSTANTIATE_TEST_SUITE_P(ALU_Rsc, RscOperation, ::testing::ValuesIn(rscTestParams),
                         [](const ::testing::TestParamInfo<AluFlagOpParams> &test) {
                             // Can use test.param here to generate the test suffix
                             return test.param.OpName;
                         });

} // TED

////////////////////////////////////////////////////////////////////////////////


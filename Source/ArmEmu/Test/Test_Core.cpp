//! @file ArmEmu/Test_Core.cpp
//! @brief The definition of unit tests for an emulation of an ARM processor core.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include <cstdint>

#include "ArmCore.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CoreLogic, Generate)
{
    uint16_t conditionMatrix[16];

    initialiseConditionMatrix(conditionMatrix);

    puts("Condition Code vs PSR Matrix: {");

    for (uint8_t index = 0; index < 16; ++index)
    {
        // Ensure that AL (14) and NV (15) have appropriate conditions set.
        EXPECT_EQ(conditionMatrix[index] >> 14, 1);

        printf("    0x%.4X,\n", conditionMatrix[index]);
    }

    puts("};");
}

GTEST_TEST(CoreLogic, CanExecute)
{
    EXPECT_TRUE(canExecuteInstruction(0xE0000000 /* AL */, 0x0));
    EXPECT_FALSE(canExecuteInstruction(0xF0000000 /* NV */, 0x0));
}

GTEST_TEST(CoreLogic, MemcAccess)
{
    uint32_t permissions = 0;

    // Pre-calculates the bits used to define if access is allowed to a MEMC
    // memory page based on Page Protection Level (PPL) and operating mode
    // in order that access checks can be branchless.
    for (uint8_t i = 0; i < 32; ++i)
    {
        bool isOsMode = i & 1;
        bool isPriviledged = i & 2;
        uint8_t ppl = (i >> 2) & 3;
        bool isWrite = i & 16;

        if (canAccessMemcPage(ppl, isPriviledged, isOsMode, isWrite))
        {
            permissions |= static_cast<uint32_t>(1) << i;
        }
    }

    // Print the static result so that is can be used in MemcHardware to
    // perform access checks.
    printf("MEMC Permission Bits: 0x%.8X\n", permissions);

    // Can read in user mode.
    EXPECT_TRUE(canAccessMemcPage(0, false, false, false));
    EXPECT_TRUE(canAccessMemcPage(1, false, false, false));
    EXPECT_FALSE(canAccessMemcPage(2, false, false, false));
    EXPECT_FALSE(canAccessMemcPage(3, false, false, false));

    // Can read in privileged mode.
    EXPECT_TRUE(canAccessMemcPage(0, true, false, false));
    EXPECT_TRUE(canAccessMemcPage(1, true, false, false));
    EXPECT_TRUE(canAccessMemcPage(2, true, false, false));
    EXPECT_TRUE(canAccessMemcPage(3, true, false, false));

    // Can read in OS mode.
    EXPECT_TRUE(canAccessMemcPage(0, false, true, false));
    EXPECT_TRUE(canAccessMemcPage(1, false, true, false));
    EXPECT_TRUE(canAccessMemcPage(2, false, true, false));
    EXPECT_TRUE(canAccessMemcPage(3, false, true, false));

    // Can read in OS and privilege mode.
    EXPECT_TRUE(canAccessMemcPage(0, true, true, false));
    EXPECT_TRUE(canAccessMemcPage(1, true, true, false));
    EXPECT_TRUE(canAccessMemcPage(2, true, true, false));
    EXPECT_TRUE(canAccessMemcPage(3, true, true, false));

    // Can write in user mode.
    EXPECT_TRUE(canAccessMemcPage(0, false, false, true));
    EXPECT_FALSE(canAccessMemcPage(1, false, false, true));
    EXPECT_FALSE(canAccessMemcPage(2, false, false, true));
    EXPECT_FALSE(canAccessMemcPage(3, false, false, true));

    // Can write in privileged mode.
    EXPECT_TRUE(canAccessMemcPage(0, true, false, true));
    EXPECT_TRUE(canAccessMemcPage(1, true, false, true));
    EXPECT_TRUE(canAccessMemcPage(2, true, false, true));
    EXPECT_TRUE(canAccessMemcPage(3, true, false, true));

    // Can write in OS mode.
    EXPECT_TRUE(canAccessMemcPage(0, false, true, true));
    EXPECT_TRUE(canAccessMemcPage(1, false, true, true));
    EXPECT_FALSE(canAccessMemcPage(2, false, true, true));
    EXPECT_FALSE(canAccessMemcPage(3, false, true, true));

    // Can write in OS and privilege mode.
    EXPECT_TRUE(canAccessMemcPage(0, true, true, true));
    EXPECT_TRUE(canAccessMemcPage(1, true, true, true));
    EXPECT_TRUE(canAccessMemcPage(2, true, true, true));
    EXPECT_TRUE(canAccessMemcPage(3, true, true, true));
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


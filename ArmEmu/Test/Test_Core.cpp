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
GTEST_TEST(ConditionCodes, Generate)
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

GTEST_TEST(ConditionCodes, CanExecute)
{
    EXPECT_TRUE(canExecuteInstruction(0xE0000000 /* AL */, 0x0));
    EXPECT_FALSE(canExecuteInstruction(0xF0000000 /* NV */, 0x0));
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


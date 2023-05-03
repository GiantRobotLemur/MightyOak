//! @file ArmEmu/Test_Main.cpp
//! @brief The definition of the entry point to the Google Test runner.
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

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
namespace Mo {
namespace Arm {

void registerAluExecTests();
void registerDataTransferExecTests();

}} // namespace Mo::Arm

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // Register additional test points.
    Mo::Arm::registerAluExecTests();
    Mo::Arm::registerDataTransferExecTests();

    int result = RUN_ALL_TESTS();

    return result;
}
////////////////////////////////////////////////////////////////////////////////


//! @file MightyOakLib/Tests/Test_Main.cpp
//! @brief The definition of the entry point to the Google Test runner.
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

#include "Ag/Core/FsPath.hpp"
#include "ArmEmu/EmuOptions.hpp"

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // Initialise paths relative to the current binary.
    Mo::Arm::Options::findRomImagePath(Ag::Fs::Path::getProgramDirectory(),
                                       Ag::Fs::Path("ROMs"));

    int result = RUN_ALL_TESTS();

    return result;
}
////////////////////////////////////////////////////////////////////////////////


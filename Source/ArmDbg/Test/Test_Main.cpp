//! @file ArmDbg/Test/Test_Main.cpp
//! @brief The definition of the entry point to the ArmDbg test runner.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
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

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // Initialise ROM paths relative to the current binary.
    auto programDir = Ag::Fs::Path::getProgramDirectory();

    Ag::utf8_cptr_t paths[] = {
        "ROMs",
        "EmulatorApp/RelWithDebInfo/ROMs",
        "EmulatorApp/Debug/ROMs",
    };

    for (auto path : paths)
    {
        Ag::Fs::Path relPath(path);

        if (Mo::Arm::Options::findRomImagePath(programDir, relPath))
            break;
    }

    return RUN_ALL_TESTS();
}
////////////////////////////////////////////////////////////////////////////////

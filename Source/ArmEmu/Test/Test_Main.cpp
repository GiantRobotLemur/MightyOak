//! @file ArmEmu/Test_Main.cpp
//! @brief The definition of the entry point to the Google Test runner.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
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
void registerCoProcessorExecTests();
void registerPipelineTests();

}} // namespace Mo::Arm

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // Initialise paths relative to the current binary.
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

    // Register additional test points.
    Mo::Arm::registerAluExecTests();
    Mo::Arm::registerDataTransferExecTests();
    Mo::Arm::registerCoProcessorExecTests();
    Mo::Arm::registerPipelineTests();

    int result = RUN_ALL_TESTS();

    return result;
}
////////////////////////////////////////////////////////////////////////////////


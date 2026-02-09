//! @file EmulatorApp/Main.cpp
//! @brief The definition of the entry point for the Mighty Oak emulator
//! application.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "MightyOakLib.hpp"
#include "Ag/Core/App.hpp"
#include "AppVersion.hpp"

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
class VersionnedEmulator : public Mo::EmulatorApp
{
public:
    VersionnedEmulator() :
        Mo::EmulatorApp(MAKE_APP_METADATA_EX("Experimental"))
    {
    }

    virtual ~VersionnedEmulator() = default;
};

IMPLEMENT_MAIN(VersionnedEmulator);

////////////////////////////////////////////////////////////////////////////////


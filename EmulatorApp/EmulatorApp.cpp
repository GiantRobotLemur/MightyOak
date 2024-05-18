//! @file EmulatorApp/EmulatorApp.cpp
//! @brief The definition of an object at the root of the emulator
//! application hierarchy.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "EmulatorApp.hpp"
#include "CommandLineOptions.hpp"

#include "Ag/SdlTools.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// App Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Inherited from Ag::App.
Ag::CommandLineUPtr EmulatorApp::createCommandLineArguments() const
{
    return std::make_unique<CommandLineOptions>();
}

// Inherited from Ag::App.
bool EmulatorApp::initialise(const Ag::Cli::ProgramArguments *args)
{
    bool isOK = false;
    const CommandLineOptions *opts = dynamic_cast<const CommandLineOptions *>(args);

    if (opts != nullptr)
    {
        // TODO: Process command line options.

        // Initialise SDL.
        _sdl.initialise();

        _mainWindow = UI::MainWindow::create();

        isOK = true;
    }

    return isOK;
}

// Inherited from Ag::App.
int EmulatorApp::run()
{
    Ag::SDL::InputManager messagePump;
    _mainWindow->registerHandlers(messagePump);

    return messagePump.run();
}

// Inherited from Ag::App.
void EmulatorApp::shutdown()
{
    // Ensure the window and rendering resources are disposed of before
    // shutting down SDL.
    _mainWindow.reset();

    _sdl.shutdown();
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


//! @file MightyOakLib/EmulatorApp.hpp
//! @brief The declaration of the object representing the root object of the
//! emulator application.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MIGHTY_OAK_LIB_EMU_APP_HPP__
#define __MIGHTY_OAK_LIB_EMU_APP_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <Ag/Core.hpp>
#include <Ag/Sdl3Tools.hpp>

namespace Mo {


////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief The object representing the root object of the emulator application.
class EmulatorApp : public Ag::App
{
public:
    // Construction/Destruction
    EmulatorApp(const Ag::AppMetadata &appMetadata);
    virtual ~EmulatorApp() = default;

    // Accessors

    // Operations
    static void initialiseRuntimeGlobals();

    // Overrides
    // virtual void onUnhandledException(const std::exception &error);
protected:
    virtual Ag::CommandLineUPtr createCommandLineArguments() const;
    virtual bool initialise(const Ag::Cli::ProgramArguments *args);
    virtual int run(const Ag::Cli::ProgramArguments *args);
    virtual void shutdown();

private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    Ag::SDL3::Initialiser _sdlInitialiser;
    Ag::AppMetadata _appMetadata;
};

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

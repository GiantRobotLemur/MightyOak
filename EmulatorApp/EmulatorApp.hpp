//! @file EmulatorApp/EmulatorApp.hpp
//! @brief The declaration of an object at the root of the emulator
//! application hierarchy.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __EMU_APP_APPLICATION_HPP__
#define __EMU_APP_APPLICATION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/App.hpp"
#include "Ag/Core/ProgramArguments.hpp"
#include "Ag/SdlTools/Initialiser.hpp"
#include "EmuUI.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object at the root of the emulator application hierarchy.
class EmulatorApp : public Ag::App
{
public:
    // Construction/Destruction
    EmulatorApp() = default;
    virtual ~EmulatorApp() = default;

    // Overrides
    //virtual void onUnhandledException(const std::exception &error);
protected:
    virtual Ag::CommandLineUPtr createCommandLineArguments() const override;
    virtual bool initialise(const Ag::Cli::ProgramArguments *args) override;
    virtual int run() override;
    virtual void shutdown() override;
    //virtual void reportException(const std::exception &error);
    //virtual void reportError(Ag::utf8_cptr_t errorText);

private:
    // Internal Fields
    Ag::SDL::Initialiser _sdl;
    UI::MainWindowSPtr _mainWindow;
};

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

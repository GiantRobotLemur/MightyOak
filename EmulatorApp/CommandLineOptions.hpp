//! @file EmulatorApp/CommandLineOptions.hpp
//! @brief The declaration of an object which can parse command line arguments
//! to the emulator application.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __EMU_APP_COMMAND_LINE_OPTIONS_HPP__
#define __EMU_APP_COMMAND_LINE_OPTIONS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/CommandLineSchema.hpp"
#include "Ag/Core/ProgramArguments.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
enum class AppCommand
{
    ShowHelp,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which can parse command line arguments to the emulator
//! application.
class CommandLineOptions : public Ag::Cli::ProgramArguments
{
public:
    // Construction/Destruction
    CommandLineOptions();
    virtual ~CommandLineOptions() = default;

private:
};

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file MightyOakLib/EmulatorApp.cpp
//! @brief The definition of the object at the root of the Mighty Oak emulator
//! application object hierarchy.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <Ag/Core.hpp>

#include "MightyOakLib/CliOptions.hpp"
#include "MightyOakLib/EmulatorApp.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// EmulatorApp Member Definitions
////////////////////////////////////////////////////////////////////////////////
EmulatorApp::EmulatorApp(const Ag::AppMetadata &appMetadata) :
    _sdlInitialiser(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS),
    _appMetadata(appMetadata)
{
    // Application metadata should be set before calling SDL_Init()
    Ag::String versionText = _appMetadata.AppVersion.toString();

    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING,
                               _appMetadata.AppName.data());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING,
                               versionText.getUtf8Bytes());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING,
                               "uk.co.na-se.mightyoak");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING,
                               _appMetadata.Author.data());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING,
                               "https://github.com/GiantRobotLemur/MightyOak");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING,
                               "application");
}

// Inherited from Ag::App.
Ag::CommandLineUPtr EmulatorApp::createCommandLineArguments() const
{
    return std::make_unique<CliOptions>(_appMetadata);
}

// Inherited from Ag::App.
bool EmulatorApp::initialise(const Ag::Cli::ProgramArguments *args)
{
    // If the user requested a standard command, don't do any initialisation.
    if (args->hasStandardCommand())
        return true;

    const CliOptions *cliOptions;

    if (!Ag::tryCast(args, cliOptions))
        return false;

    Ag::String initError;

    if (_sdlInitialiser.tryInitialise(initError))
    {
        reportError(initError.getUtf8Bytes());
        return false;
    }

    return true;
}

// Inherited from Ag::App.
int EmulatorApp::run(const Ag::Cli::ProgramArguments *args)
{
    int processResult = 1;
    const CliOptions *options;

    if (args->tryProcessStandardCommand())
    {
        // One of the standard console of SDL commands was executed.
        processResult = 0;
    }
    else if (Ag::tryCast(args, options))
    {
        if (options->getCommand() == Ag::toScalar(AppCommand::RunSession))
        {
            // TODO: Run emulator session.
            const Arm::Options &sessionOptions = options->getEmulatedSystemConfig();

            processResult = 0;
        }
    }

    return processResult;
}

// Inherited from Ag::App.
void EmulatorApp::shutdown()
{
    _sdlInitialiser.shutdown();
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


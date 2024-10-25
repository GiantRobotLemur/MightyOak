//! @file ArmDebugger/CommandLineOptions.cpp
//! @brief The definition of an object which manages options specified on the
//! command line.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>

#include "Ag/Core/Utils.hpp"

#include "CommandLineOptions.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// CommandLineOptions Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
CommandLineOptions::CommandLineOptions() :
    _action(CliAction::None)
{
}

bool CommandLineOptions::needsUI() const
{
    return (_action != CliAction::ShowHelp) && (_action != CliAction::ShowVersion);
}

CliAction CommandLineOptions::getAction() const { return _action; }

const QString &CommandLineOptions::getSessionFileName() const { return _sessionFile; }

bool CommandLineOptions::tryParse(const QStringList &args, QString &error)
{
    error.clear();
    _action = CliAction::None;
    _sessionFile.clear();

    QCommandLineParser parser;
    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();
    QCommandLineOption runOption = QCommandLineOption("run", tr("Load a session file and begin execution."));
    QCommandLineOption configOption = QCommandLineOption("configure", tr("Optionally load a session file and displays the New Session dialog."));

    parser.addOption(runOption);
    parser.addOption(configOption);
    parser.addPositionalArgument(tr("session file"),
                                 tr("A file containing configuration for the emulated session and other annotations"));

    bool isOK = false;

    if (parser.parse(args))
    {
        if (parser.isSet(versionOption))
        {
            _action = CliAction::ShowVersion;

            if (!Ag::isStdoutEnabled())
            {
                Ag::enableStdout();
                putchar('\n');
            }

            parser.showVersion();
        }
        else if (parser.isSet(helpOption))
        {
            _action = CliAction::ShowHelp;

            if (!Ag::isStdoutEnabled())
            {
                Ag::enableStdout();
                putchar('\n');
            }

            parser.showHelp(EXIT_SUCCESS);
        }
        else if (parser.isSet(runOption))
        {
            _action = CliAction::RunSession;
        }
        else if (parser.isSet(configOption))
        {
            _action = CliAction::ConfigureSession;
        }

        QStringList positionalArgs = parser.positionalArguments();
        QString sessionFileName;
        isOK = true;

        if (positionalArgs.size() > 1)
        {
            error = tr("Too many positional arguments.");
            isOK = false;
        }
        else if (positionalArgs.isEmpty() == false)
        {
            sessionFileName = positionalArgs.first();
        }

        if (sessionFileName.isEmpty())
        {
            if (_action == CliAction::RunSession)
            {
                error = tr("A session file must be specified to begin execution.");
                isOK = false;
            }
        }
        else if ((_action == CliAction::ConfigureSession) ||
                 (_action == CliAction::RunSession))
        {
            // We should resolve the session file name.
            QFileInfo sessionFile(sessionFileName);

            if (sessionFile.exists())
            {
                _sessionFile = QDir::toNativeSeparators(sessionFile.absoluteFilePath());
            }
            else
            {
                error = tr("Session file '%1' not found.")
                    .arg(QDir::toNativeSeparators(sessionFileName));
                isOK = false;
            }
        }
    }
    else
    {
        error = parser.errorText();
    }

    return isOK;
}
////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


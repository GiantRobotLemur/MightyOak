//! @file ArmDbgCli/ArmDbgCli_Main.cpp
//! @brief The definition of the entry point for the ArmDbg CLI debugger tool.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>

#include "Ag/Core.hpp"

#include "ArmDbg.hpp"

using namespace Ag;

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum class ArmDbgCommand
{
    Auto,
    ShowHelp,
    RunScript,
};

//! @brief Defines command line arguments for the ArmDbg tool.
class ArmDbgArgs : public Cli::ProgramArguments
{
private:
    // Internal Types
    enum Option
    {
        ShowHelp,
        ScriptPath,
        RomPath,
    };

    // Internal Fields
    ArmDbgCommand _command;
    String _scriptPath;
    String _romPath;

public:
    static Cli::Schema createSchema()
    {
        Cli::SchemaBuilder builder;
        builder.setDescription("A scriptable ARM emulator debugger.");
        builder.defineValueArgument("script file", Cli::UpToOne);

        builder.defineOption(Option::ShowHelp,
                             "Display command line help.",
                             Cli::OptionValue::None);
        builder.defineAlias(Option::ShowHelp, U'?');
        builder.defineAlias(Option::ShowHelp, "help");

        builder.defineOption(Option::ScriptPath,
                             "Specifies the path to the debugger script file.",
                             Cli::OptionValue::Mandatory, "script path");
        builder.defineAlias(Option::ScriptPath, U's');
        builder.defineAlias(Option::ScriptPath, "script");

        builder.defineOption(Option::RomPath,
                             "Overrides the ROM search path.",
                             Cli::OptionValue::Mandatory, "ROM path");
        builder.defineAlias(Option::RomPath, U'r');
        builder.defineAlias(Option::RomPath, "rom");

        return builder.createSchema();
    }

    // Construction/Destruction
    ArmDbgArgs() :
        Cli::ProgramArguments(createSchema()),
        _command(ArmDbgCommand::Auto)
    {
    }

    virtual ~ArmDbgArgs() = default;

    // Accessors
    ArmDbgCommand getCommand() const { return _command; }
    const String &getScriptPath() const { return _scriptPath; }
    const String &getRomPath() const { return _romPath; }

protected:
    // Overrides

    // Inherited from Cli::ProgramArguments
    virtual bool processOption(uint32_t id, const String & /*original*/,
                               const String &value, String &error) override
    {
        bool isOK = true;

        switch (id)
        {
        case ShowHelp:
            _command = ArmDbgCommand::ShowHelp;
            break;

        case ScriptPath:
            _scriptPath = value;
            break;

        case RomPath:
            _romPath = value;
            break;

        default:
            isOK = false;
            error = "Unknown option.";
            break;
        }

        return isOK;
    }

    // Inherited from Cli::ProgramArguments
    virtual bool processArgument(const String &argument,
                                 String &error) override
    {
        if (_scriptPath.isEmpty())
        {
            _scriptPath = argument;
            return true;
        }
        else
        {
            error = "Only one script file can be specified.";
            return false;
        }
    }

    // Inherited from Cli::ProgramArguments
    virtual void postProcess() override
    {
        if (_command == ArmDbgCommand::Auto)
        {
            if (_scriptPath.isEmpty())
                _command = ArmDbgCommand::ShowHelp;
            else
                _command = ArmDbgCommand::RunScript;
        }
    }
};

//! @brief The root application object for the ArmDbg CLI tool.
class ArmDbgApp : public App
{
private:
    // Internal Fields
    ArmDbgCommand _command;
    String _scriptPath;
    String _romPath;

protected:
    // Inherited from Ag::App
    CommandLineUPtr createCommandLineArguments() const override
    {
        return std::make_unique<ArmDbgArgs>();
    }

    // Inherited from Ag::App
    bool initialise(const Cli::ProgramArguments *args) override
    {
        const auto *dbgArgs = static_cast<const ArmDbgArgs *>(args);

        _command = dbgArgs->getCommand();
        _scriptPath = dbgArgs->getScriptPath();
        _romPath = dbgArgs->getRomPath();

        return true;
    }

    // Inherited from Ag::App
    int run() override
    {
        if (_command == ArmDbgCommand::ShowHelp)
        {
            puts("Usage: ArmDbg [--script|-s] <script.dbg> [--rom|-r <path>]\n"
                 "\n"
                 "A scriptable ARM emulator debugger.\n"
                 "\n"
                 "Options:\n"
                 "  -s, --script <path>  Path to the debugger script file\n"
                 "  -r, --rom <path>     Override ROM search path\n"
                 "  -?, --help           Display this help message\n");
            return 0;
        }

        // Open the script file.
        std::string scriptPathStr(_scriptPath.getUtf8Bytes());
        std::ifstream scriptFile(scriptPathStr);

        if (!scriptFile.is_open())
        {
            printf("Error: Cannot open script file '%s'.\n",
                   scriptPathStr.c_str());
            return 1;
        }

        // Create a debug session writing to stdout.
        DebugSession session(std::cout);

        if (!_romPath.isEmpty())
        {
            session.setRomSearchPath(
                Fs::Path(_romPath.getUtf8Bytes()));
        }

        // Run the script.
        ScriptRunner runner(session);
        bool success = runner.run(scriptFile);

        return success ? 0 : 1;
    }
};

} // anonymous namespace
}} // namespace Mo::Arm

IMPLEMENT_MAIN(Mo::Arm::ArmDbgApp);

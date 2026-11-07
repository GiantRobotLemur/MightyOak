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
#include <sstream>

#include "Ag/Core.hpp"

#include "ArmDbg.hpp"

#ifdef _WIN32
#include "PipeServer.hpp"
#include "PipeClient.hpp"
#endif

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
    Serve,
    Send,
    SendScript,
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
        Serve,
        Send,
        SendScript,
        PipeName,
    };

    // Internal Fields
    ArmDbgCommand _command;
    String _scriptPath;
    String _romPath;
    String _sendCommand;
    String _sendScriptPath;
    String _pipeName;

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

        builder.defineOption(Option::Serve,
                             "Start as a persistent pipe server.",
                             Cli::OptionValue::None);
        builder.defineAlias(Option::Serve, "serve");

        builder.defineOption(Option::Send,
                             "Send a command to a running server.",
                             Cli::OptionValue::Mandatory, "command");
        builder.defineAlias(Option::Send, "send");

        builder.defineOption(Option::SendScript,
                             "Send a script file to a running server.",
                             Cli::OptionValue::Mandatory, "script path");
        builder.defineAlias(Option::SendScript, "send-script");

        builder.defineOption(Option::PipeName,
                             "Override the named pipe name (default: ArmDbg).",
                             Cli::OptionValue::Mandatory, "pipe name");
        builder.defineAlias(Option::PipeName, "pipe");

        return builder.createSchema();
    }

    // Construction/Destruction
    ArmDbgArgs() :
        Cli::ProgramArguments(createSchema()),
        _command(ArmDbgCommand::Auto),
        _pipeName("ArmDbg")
    {
    }

    virtual ~ArmDbgArgs() = default;

    // Accessors
    ArmDbgCommand getCommand() const { return _command; }
    const String &getScriptPath() const { return _scriptPath; }
    const String &getRomPath() const { return _romPath; }
    const String &getSendCommand() const { return _sendCommand; }
    const String &getSendScriptPath() const { return _sendScriptPath; }
    const String &getPipeName() const { return _pipeName; }

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

        case Serve:
            _command = ArmDbgCommand::Serve;
            break;

        case Send:
            _command = ArmDbgCommand::Send;
            _sendCommand = value;
            break;

        case SendScript:
            _command = ArmDbgCommand::SendScript;
            _sendScriptPath = value;
            break;

        case PipeName:
            _pipeName = value;
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
    String _sendCommand;
    String _sendScriptPath;
    String _pipeName;

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
        _sendCommand = dbgArgs->getSendCommand();
        _sendScriptPath = dbgArgs->getSendScriptPath();
        _pipeName = dbgArgs->getPipeName();

        return true;
    }

    // Inherited from Ag::App
    int run() override
    {
        if (_command == ArmDbgCommand::ShowHelp)
        {
            puts("Usage: ArmDbg [--script|-s] <script.dbg> [--rom|-r <path>]\n"
                 "       ArmDbg --serve [-s startup.dbg] [--pipe name]\n"
                 "       ArmDbg --send \"command\" [--pipe name]\n"
                 "       ArmDbg --send-script <file.dbg> [--pipe name]\n"
                 "\n"
                 "A scriptable ARM emulator debugger.\n"
                 "\n"
                 "Options:\n"
                 "  -s, --script <path>      Path to the debugger script file\n"
                 "  -r, --rom <path>         Override ROM search path\n"
                 "  --serve                  Start as a persistent pipe server\n"
                 "  --send <command>         Send a command to a running server\n"
                 "  --send-script <path>     Send a script to a running server\n"
                 "  --pipe <name>            Named pipe name (default: ArmDbg)\n"
                 "  -?, --help               Display this help message\n");
            return 0;
        }

#ifdef _WIN32
        if (_command == ArmDbgCommand::Send)
        {
            return runSendCommand();
        }

        if (_command == ArmDbgCommand::SendScript)
        {
            return runSendScript();
        }

        if (_command == ArmDbgCommand::Serve)
        {
            return runServe();
        }
#else
        if (_command == ArmDbgCommand::Serve ||
            _command == ArmDbgCommand::Send ||
            _command == ArmDbgCommand::SendScript)
        {
            puts("Error: Pipe server mode is only supported on Windows.\n");
            return 1;
        }
#endif

        return runScript();
    }

private:
    int runScript()
    {
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

#ifdef _WIN32
    int runServe()
    {
        // Create a debug session with captured output.
        std::ostringstream capturedOutput;
        DebugSession session(capturedOutput);

        if (!_romPath.isEmpty())
        {
            session.setRomSearchPath(
                Fs::Path(_romPath.getUtf8Bytes()));
        }

        ScriptRunner runner(session);

        // Run optional startup script.
        if (!_scriptPath.isEmpty())
        {
            std::string scriptPathStr(_scriptPath.getUtf8Bytes());
            std::ifstream scriptFile(scriptPathStr);

            if (!scriptFile.is_open())
            {
                printf("Error: Cannot open startup script '%s'.\n",
                       scriptPathStr.c_str());
                return 1;
            }

            // Redirect startup script output to stdout.
            // Temporarily swap the ostringstream content to stdout for
            // visibility during startup.
            printf("Running startup script: %s\n", scriptPathStr.c_str());
            fflush(stdout);

            std::string line;
            size_t lineNumber = 0;

            while (std::getline(scriptFile, line))
            {
                ++lineNumber;

                capturedOutput.str("");
                capturedOutput.clear();

                runner.processLine(line, lineNumber);

                std::string output = capturedOutput.str();

                if (!output.empty())
                {
                    fputs(output.c_str(), stdout);
                    fflush(stdout);
                }
            }

            printf("Startup script complete.\n");
            fflush(stdout);
        }

        // Enter pipe server loop.
        std::string pipeNameStr(_pipeName.getUtf8Bytes());
        PipeServer server(runner, capturedOutput, pipeNameStr);

        // Install Ctrl+C handler to allow graceful shutdown.
        static PipeServer *s_serverInstance = &server;

        SetConsoleCtrlHandler([](DWORD ctrlType) -> BOOL {
            if (ctrlType == CTRL_C_EVENT || ctrlType == CTRL_BREAK_EVENT)
            {
                if (s_serverInstance != nullptr)
                {
                    s_serverInstance->requestShutdown();
                }

                return TRUE;
            }

            return FALSE;
        }, TRUE);

        int result = server.run();

        // Remove Ctrl+C handler.
        s_serverInstance = nullptr;
        SetConsoleCtrlHandler(nullptr, FALSE);

        return result;
    }

    int runSendCommand()
    {
        std::string pipeNameStr(_pipeName.getUtf8Bytes());
        std::string commandStr(_sendCommand.getUtf8Bytes());

        PipeClient client(pipeNameStr);
        std::string output;
        uint8_t status = 0;

        if (!client.sendCommand(commandStr, output, status))
        {
            fprintf(stderr, "Error: Cannot connect to server on pipe '%s'.\n",
                    pipeNameStr.c_str());
            return 1;
        }

        if (!output.empty())
        {
            fputs(output.c_str(), stdout);
            fflush(stdout);
        }

        return status;
    }

    int runSendScript()
    {
        std::string scriptPathStr(_sendScriptPath.getUtf8Bytes());
        std::ifstream scriptFile(scriptPathStr);

        if (!scriptFile.is_open())
        {
            fprintf(stderr, "Error: Cannot open script file '%s'.\n",
                    scriptPathStr.c_str());
            return 1;
        }

        // Read the entire script and send it as one command.
        std::ostringstream contents;
        contents << scriptFile.rdbuf();

        std::string pipeNameStr(_pipeName.getUtf8Bytes());
        PipeClient client(pipeNameStr);
        std::string output;
        uint8_t status = 0;

        if (!client.sendCommand(contents.str(), output, status))
        {
            fprintf(stderr, "Error: Cannot connect to server on pipe '%s'.\n",
                    pipeNameStr.c_str());
            return 1;
        }

        if (!output.empty())
        {
            fputs(output.c_str(), stdout);
            fflush(stdout);
        }

        return status;
    }
#endif // _WIN32
};

} // anonymous namespace
}} // namespace Mo::Arm

IMPLEMENT_MAIN(Mo::Arm::ArmDbgApp);

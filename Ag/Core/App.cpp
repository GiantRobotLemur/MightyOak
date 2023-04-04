//! @file Core/App.cpp
//! @brief The definition of an object which represents the root of an
//! application object hierarchy.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <string>

#include "Ag/Core/App.hpp"
#include "Ag/Core/ProgramArguments.hpp"
#include "Ag/Core/ErrorGuard.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Utf.hpp"
#include "CoreInternal.hpp"
#include "Win32API.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object describing a command line to be processed.
struct CommandLineInfo
{
    //! @brief The object which will parse the command line.
    CommandLineUPtr Manager;

    //! @brief Receives an error message if command line parsing fails.
    String Error;

    //! @brief The WinMain() Unicode command line.
    wchar_cptr_t CommandLine;

    //! @brief The count of elements in the Args array.
    int ArgCount;

    //! @brief The main() command line arguments to parse.
    const char **Args;

    //! @brief The wmain() command line arguments to parse.
    const wchar_t * const *WideArgs;

    //! @brief Indicates whether parsing was successful.
    bool Success;

    //! @brief Constructs an object describing a console of POSIX command line.
    CommandLineInfo(Ag::CommandLineUPtr &&processor, int argc, const char **argv) :
        Manager(std::move(processor)),
        CommandLine(nullptr),
        ArgCount(argc),
        Args(argv),
        WideArgs(nullptr),
        Success(false)
    {
    }

    //! @brief Constructs an object describing a console of a Unicode command line.
    CommandLineInfo(Ag::CommandLineUPtr &&processor, int argc, const wchar_t *const *argv) :
        Manager(std::move(processor)),
        CommandLine(nullptr),
        ArgCount(argc),
        Args(nullptr),
        WideArgs(argv),
        Success(false)
    {
    }

    //! @brief Constructs an object describing a the command line for a Win32
    //! application which uses WinMain() as its entry point.
    CommandLineInfo(Ag::CommandLineUPtr &&processor, wchar_cptr_t commandLine) :
        Manager(std::move(processor)),
        CommandLine(commandLine),
        ArgCount(0),
        Args(nullptr),
        WideArgs(nullptr),
        Success(false)
    {
    }
};

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
static App *g_singletonApp = nullptr;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to parse command line arguments in a guarded context.
//! @param[in,out] info A structure holding the parameters to parse, the object
//! to parse them and which will be updated with the result.
void guardedTryParse(CommandLineInfo &info)
{
    if (info.CommandLine != nullptr)
    {
        info.Success = info.Manager->tryParse(info.CommandLine, info.Error);
    }
    else if (info.ArgCount > 0)
    {
        if (info.Args != nullptr)
        {
            info.Success = info.Manager->tryParse(info.ArgCount, info.Args,
                                                  info.Error);
        }
        else if (info.WideArgs != nullptr)
        {
            info.Success = info.Manager->tryParse(info.ArgCount, info.WideArgs,
                                                  info.Error);
        }
    }
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a singleton instance of a class which administers the
//! structure of the application.
//! @throw OperationException If any other instances of an App-derived class
//! are currently instantiated.
App::App()
{
    if (g_singletonApp == nullptr)
    {
        // Register the application object.
        g_singletonApp = this;
    }
    else
    {
        throw OperationException("Only one instance of an Ag::App-derived class "
                                 "can be instantiated at any one time.");
    }
}

//! @brief Disposes of resources owned by the object and de-registers this as
//! the singleton instance.
App::~App()
{
    if (g_singletonApp == this)
    {
        // De-register the application object.
        g_singletonApp = nullptr;
    }
}

//! @brief Gets the singleton instance of an App-derived object instantiated
//! within the process.
//! @returns A pointer to the current App instance, nullptr if no instances of
//! and App-derived class are currently instantiated.
App *App::get()
{
    return g_singletonApp;
}

//! @brief Executes the application using the command line arguments it was
//! launched with.
//! @returns The global process result value to return from main().
//! @details This function is intended to run a traditional POSIX application
//! or a Win32 console application.
int App::exec()
{
#ifdef _WIN32
    CommandLineInfo cliInfo(createCommandLineArguments(), ::GetCommandLineW());
#else
#error Implement default command line for Linux.
#endif

    return innerExec(cliInfo);
}

//! @brief Executes the application.
//! @param[in] argc The count of command line arguments.
//! @param[in] argv The array of command line argument tokens. The first argument
//! is expected to be the path to the binary used to launch the application.
//! @returns The global process result value to return from main().
//! @details This function is intended to run a traditional POSIX application
//! or a Win32 ANSI console application.
int App::exec(int argc, const char **argv)
{
    CommandLineInfo commandLineInfo(createCommandLineArguments(), argc, argv);

    return innerExec(commandLineInfo);
}

//! @brief Executes the application.
//! @param[in] argc The count of command line arguments.
//! @param[in] argv The array of wide character command line argument tokens.
//! The first argument is expected to be the path to the binary used to launch
//! the application.
//! @returns The global process result value to return from main().
//! @details This function is intended to run a Unicode-enabled Win32 console
//! application .
int App::exec(int argc, const wchar_t * const *argv)
{
    CommandLineInfo commandLineInfo(createCommandLineArguments(), argc, argv);

    return innerExec(commandLineInfo);
}

//! @brief Executes the application.
//! @param[in] commandLine The entire command line expressed as a null-terminated
//! array of wide characters. The path to the binary used to launch the
//! application is not expected to be included.
//! @returns The global process result value to return from main().
//! @details This function is intended to receive the command line string passed
//! to the Win32 GUI application entry point WinMain().
int App::exec(wchar_cptr_t commandLine)
{
    CommandLineInfo commandLineInfo(createCommandLineArguments(), commandLine);

    return innerExec(commandLineInfo);
}

//! @brief Can be called from anywhere in the program where an exception is
//! caught unexpectedly.
//! @param[in] error The exception which went unhandled.
void App::onUnhandledException(const std::exception &error)
{
    reportException(error);
}

//! @brief Overridden in derived classes in order to instantiate an
//! application-specific CommandLine-derived class to process command line
//! parameters.
//! @returns A unique pointer to a pre-configured CommandLine-derived object
//! which will be used to process command line options.
//! @details The this function will be called by exec() before initialise()
//! and the resultant instance disposed of after initialise() has returned.
//! The default implementation returns a CommandLine instance with an empty
//! schema which will ignore arguments and disallow any options.
CommandLineUPtr App::createCommandLineArguments() const
{
    Cli::SchemaBuilder builder;
    Cli::Schema emptySchema = builder.createSchema();

    return std::make_unique<Cli::ProgramArguments>(emptySchema);
}

//! @brief Overridden by derived classes in order to perform one-time
//! application setup tasks based on default behaviour or parameters passed
//! on the command line.
//! @param[in] args An instance of a CommandLine-derived object returned by
//! createCommandLineArguments().
//! @retval true The application has successfully initialised, it can now
//! operate by calling the run() member function followed by the shutdown()
//! member function.
//! @retval false Initialisation failed, the application will exit with a
//! non-zero process result.
//! @details The default implementation always returns true;
bool App::initialise(const Cli::ProgramArguments * /* args */)
{
    return true;
}

//! @brief Implemented in derived classes to perform the main function of the
//! application after a successful initialisation.
//! @return The global process result to be returned from the process entry
//! point function i.e. main() or WinMain().
//! @details The default implementation always returns 0 with no previous processing.
int App::run()
{
    return 0;
}

//! @brief Implemented in derived classes to perform clean-up tasks once the
//! run() member function has returned. Errors from this function will be ignored.
void App::shutdown()
{
    // Do nothing.
}

//! @brief Processes an exception caught at the outermost level of nesting.
//! @param[in] error The exception to report.
//! @note The default implementation forms an error message and calls the
//! reportError() member function.
void App::reportException(const std::exception &error)
{
    const Ag::Exception *agError = dynamic_cast<const Exception *>(&error);

    if ((agError != nullptr) && (agError->isEmpty() == false))
    {
        std::string buffer;

        appendPrintf(buffer, "%.*s caught: %.*s\n%.*s\n",
                     static_cast<int>(agError->getDomain().length()),
                     agError->getDomain().data(),
                     static_cast<int>(agError->getMessage().length()),
                     agError->getMessage().data(),
                     static_cast<int>(agError->getDetail().length()),
                     agError->getDetail().data());

        if (agError->getErrorCode() != 0)
        {
            appendPrintf(buffer, "Error code: 0x%.4jX\n", agError->getErrorCode());
        }

        if (agError->getStackTrace().isEmpty() == false)
        {
            buffer.append("\nStack Trace:\n");
            agError->getStackTrace().appendToString(buffer, true);
        }

        reportError(buffer.c_str());
    }
}

//! @brief Processes error message text.
//! @param[in] errorText The null-terminated UTF-8 encoded text definging the,
//! possibly multi-line, error output.
//! @note The default implementation writes to stderr and the Windows debugger
//! console if one is attached.
void App::reportError(utf8_cptr_t errorText)
{
#ifdef _WIN32
    if (::IsDebuggerPresent())
    {
        // Copy the text to the Win32 debug console.
        std::wstring wideBuffer;
        Utf::appendToWide(wideBuffer, errorText, strlen(errorText));

        ::OutputDebugStringW(wideBuffer.c_str());
    }
#endif

    // Write the text to the error stream.
    fputs(errorText, stderr);
}

//! @brief Common logic for the exec() member function.
//! @param[in] info Abstracted details of the command line arguments to be
//! processed.
//! @return The global process result.
int App::innerExec(CommandLineInfo &info)
{
    ErrorGuard guard;
    int processResult = 0;

    if (info.Manager)
    {
        // Attempt to process the command line arguments.
        if (guard.tryExecProcedure(guardedTryParse, info))
        {
            if (info.Success == false)
            {
                // The processing logic detected an error.
                processResult = 1;
                std::string buffer;

                appendPrintf(buffer, "Command line arguments were invalid: %s\n",
                             info.Error.getUtf8Bytes());

                reportError(buffer.c_str());
            }
        }
        else
        {
            // An exception was thrown while processing the command line.
            std::string buffer;
            appendPrintf(buffer, "Error processing the command line arguments: %.*s\n%.*s\n",
                         static_cast<int>(guard.getError().getMessage().length()),
                         guard.getError().getMessage().data(),
                         static_cast<int>(guard.getError().getDetail().length()),
                         guard.getError().getDetail().data());

            reportError(buffer.c_str());
            processResult = 1;
        }
    }

    if (processResult == 0)
    {
        // Attempt to perform application initialisation.
        bool isOK = false;
        if (guard.tryExecFunction<bool, App *, const Cli::ProgramArguments *>(
                guardedInitialise, isOK, this, info.Manager.get()))
        {
            if (isOK)
            {
                // Dispose of the command line data as it shouldn't be allowed to
                // linger for the duration of the application.
                info.Manager.reset();

                if (guard.tryExecFunction(guardedRun, processResult, this) == false)
                {
                    onUnhandledException(guard.getError());
                    processResult = 1;
                }

                if (guard.tryExecProcedure(guardedShutdown, this) == false)
                {
                    // Signal failure, but otherwise ignore the error.
                    processResult = 1;
                }
            }
            else
            {
                // No failure message, but exit the process without shutdown.
                processResult = 1;
            }
        }
        else
        {
            // Application initialisation threw an exception.
            std::string buffer;

            appendPrintf(buffer, "An initialisation error occurred: %.*s\n%.*s\n",
                         static_cast<int>(guard.getError().getMessage().length()),
                         guard.getError().getMessage().data(),
                         static_cast<int>(guard.getError().getDetail().length()),
                         guard.getError().getDetail().data());
            reportError(buffer.c_str());
            processResult = 1;
        }
    }

    return processResult;
}

//! @brief Makes a call to the overridden version of the initialise() member
//! function in a guarded context.
//! @param[in] instance The application instance to call initialise() on.
//! @param[in] args The parsed command line arguments.
//! @returns The initialisation result.
bool App::guardedInitialise(App *instance, const Cli::ProgramArguments *args)
{
    return instance->initialise(args);
}

//! @brief Makes a call to the overridden version of the run() member function
//! in a guarded context.
//! @param[in] instance The application instance to call run() on.
//! @returns The global process result.
int App::guardedRun(App *instance)
{
    return instance->run();
}

//! @brief Makes a call to the overridden version of the shutdown() member function
//! in a guarded context.
//! @param[in] instance The application instance to call shutdown() on.
void App::guardedShutdown(App *instance)
{
    instance->shutdown();
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


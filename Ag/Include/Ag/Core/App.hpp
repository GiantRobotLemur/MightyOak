//! @file Ag/Core/App.hpp
//! @brief The declaration of an object which represents the root of an
//! application object hierarchy.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_APP_HPP__
#define __AG_CORE_APP_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <thread>

#include "Configuration.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#ifdef _GUI // Defined by CMake script
#define IMPLEMENT_MAIN(AppType) int WINAPI wWinMain(HINSTANCE, HINSTANCE, \
LPWSTR cmdLine, int) { AppType theApp; return theApp.exec(cmdLine); }
#else // !defined _GUI
#define IMPLEMENT_MAIN(AppType) int wmain(int argc, wchar_t *argv[], wchar_t *[]) \
{ AppType theApp; return theApp.exec(argc, argv); }
#endif
#else // !defined _WIN32
#define IMPLEMENT_MAIN(AppType) int main(int argc, char *argv[]) \
{ AppType theApp; return theApp.exec(argc, argv); }
#endif

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Cli
{
class ProgramArguments;
}
struct CommandLineInfo;
class Exception;
typedef std::unique_ptr<Cli::ProgramArguments> CommandLineUPtr;

//! @brief An object which represents the root of an application object hierarchy.
class App
{
public:
    // Construction/Destruction
    App();
    virtual ~App();

    // Accessors
    static App *get();

    // Operations
    int exec();
    int exec(int argc, const char * const *argv);
    int exec(int argc, const wchar_t * const *argv);
    int exec(wchar_cptr_t commandLine);

    // Overrides
    virtual void onUnhandledException(const std::exception &error);
protected:
    virtual CommandLineUPtr createCommandLineArguments() const;
    virtual bool initialise(const Cli::ProgramArguments *args);
    virtual int run();
    virtual void shutdown();
    virtual void reportException(const std::exception &error);
    virtual void reportError(utf8_cptr_t errorText);

private:
    // Internal Types

    // Internal Functions
    int innerExec(CommandLineInfo &info);
    static bool guardedInitialise(App *instance, const Cli::ProgramArguments *args);
    static int guardedRun(App *instance);
    static void guardedShutdown(App *instance);

    // Internal Fields
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

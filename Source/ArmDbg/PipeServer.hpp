//! @file ArmDbg/PipeServer.hpp
//! @brief The declaration of a class which hosts a named pipe server for
//! remote debugger command execution.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DBG_PIPE_SERVER_HPP__
#define __ARM_DBG_PIPE_SERVER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <sstream>
#include <string>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ScriptRunner;

//! @brief Hosts a Win32 named pipe server that accepts debugger commands
//! from remote clients and dispatches them to a ScriptRunner.
class PipeServer
{
public:
    // Construction/Destruction
    PipeServer(ScriptRunner &runner, std::ostringstream &outputStream,
               const std::string &pipeName);
    ~PipeServer();

    // Operations
    int run();
    void requestShutdown();

private:
    // Internal Fields
    ScriptRunner &_runner;
    std::ostringstream &_outputStream;
    std::string _pipeName;
    void *_shutdownEvent; // HANDLE
};

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

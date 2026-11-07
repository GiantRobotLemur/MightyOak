//! @file ArmDbg/PipeClient.hpp
//! @brief The declaration of a class which connects to a named pipe server
//! to send debugger commands and receive output.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DBG_PIPE_CLIENT_HPP__
#define __ARM_DBG_PIPE_CLIENT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <string>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////

//! @brief Connects to an ArmDbg named pipe server, sends a command, and
//! retrieves the response.
class PipeClient
{
public:
    // Construction/Destruction
    PipeClient(const std::string &pipeName);
    ~PipeClient() = default;

    // Operations
    //! @brief Sends a command string and receives the response.
    //! @param[in] command The command text to send.
    //! @param[out] output The response text from the server.
    //! @param[out] status The status byte (0=ok, 1=fail).
    //! @retval true Communication succeeded.
    //! @retval false Communication failed.
    bool sendCommand(const std::string &command,
                     std::string &output, uint8_t &status);

private:
    // Internal Fields
    std::string _pipeName;
};

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

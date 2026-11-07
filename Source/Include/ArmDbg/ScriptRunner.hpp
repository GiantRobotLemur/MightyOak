//! @file ArmDbg/ScriptRunner.hpp
//! @brief The declaration of a class which reads debugger script lines from
//! an input stream and dispatches them to a DebugSession.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DBG_SCRIPT_RUNNER_HPP__
#define __ARM_DBG_SCRIPT_RUNNER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <istream>
#include <string_view>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class DebugSession;

//! @brief Reads script lines from an input stream and dispatches them to
//! a DebugSession.
class ScriptRunner
{
public:
    // Construction/Destruction
    ScriptRunner(DebugSession &session);
    ~ScriptRunner() = default;

    // Operations
    //! @brief Processes all lines from the input stream.
    bool run(std::istream &input);

    //! @brief Processes a single line of text.
    bool processLine(const std::string_view &line, size_t lineNumber);

private:
    // Internal Fields
    DebugSession &_session;
};

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

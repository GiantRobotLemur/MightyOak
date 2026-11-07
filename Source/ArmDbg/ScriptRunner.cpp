//! @file ArmDbg/ScriptRunner.cpp
//! @brief The definition of a class which reads debugger script lines from
//! an input stream and dispatches them to a DebugSession.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <string>

#include "ArmDbg/ScriptRunner.hpp"
#include "ArmDbg/DebugSession.hpp"
#include "ArmDbg/CommandDefs.hpp"
#include "ScriptLexer.hpp"
#include "CommandParser.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// ScriptRunner Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a script runner bound to a debug session.
//! @param[in] session The session to which parsed commands are dispatched.
ScriptRunner::ScriptRunner(DebugSession &session) :
    _session(session)
{
}

//! @brief Processes all lines from the input stream.
//! @param[in] input The input stream containing the script.
//! @retval true All commands executed successfully.
//! @retval false One or more commands failed.
bool ScriptRunner::run(std::istream &input)
{
    std::string line;
    size_t lineNumber = 0;
    bool allOk = true;

    while (std::getline(input, line))
    {
        ++lineNumber;

        if (!processLine(line, lineNumber))
            allOk = false;
    }

    return allOk;
}

//! @brief Processes a single line of text.
//! @param[in] line The line text.
//! @param[in] lineNumber The 1-based line number.
//! @retval true The line was processed successfully (or was blank/comment).
//! @retval false The line contained an error.
bool ScriptRunner::processLine(const std::string_view &line, size_t lineNumber)
{
    std::vector<ScriptToken> tokens;

    if (!ScriptLexer::tokenizeLine(line, tokens))
        return true; // Blank or comment line — not an error.

    ParsedCommand cmd;
    std::string parseError;

    if (!CommandParser::parseLine(tokens, lineNumber, cmd, parseError))
    {
        // TODO: Write error to session output stream when accessible.
        return false;
    }

    return _session.executeCommand(cmd);
}

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

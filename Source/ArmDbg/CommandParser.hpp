//! @file ArmDbg/CommandParser.hpp
//! @brief The declaration of a parser that converts script tokens into commands.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DBG_COMMAND_PARSER_HPP__
#define __ARM_DBG_COMMAND_PARSER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>

#include "ScriptLexer.hpp"
#include "ArmDbg/CommandDefs.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Parses tokenized script lines into ParsedCommand structures.
class CommandParser
{
public:
    //! @brief Attempts to parse a tokenized line into a command.
    //! @param[in] tokens The tokens from ScriptLexer.
    //! @param[in] lineNumber The 1-based line number for error reporting.
    //! @param[out] commandOut Receives the parsed command.
    //! @param[out] errorOut Receives error text if parsing fails.
    //! @retval true The line was parsed successfully.
    //! @retval false The line contained a syntax error.
    static bool parseLine(const std::vector<ScriptToken> &tokens,
                          size_t lineNumber,
                          ParsedCommand &commandOut,
                          std::string &errorOut);
};

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

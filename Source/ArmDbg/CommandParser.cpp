//! @file ArmDbg/CommandParser.cpp
//! @brief The definition of a parser that converts script tokens into commands.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "CommandParser.hpp"

namespace Mo {
namespace Arm {

namespace {

//! @brief Case-insensitive string comparison for string_views.
bool equalsIgnoreCase(const std::string_view &a, const char *b)
{
    size_t len = a.size();

    for (size_t i = 0; b[i] != '\0'; ++i)
    {
        if (i >= len)
            return false;

        char ac = a[i];
        char bc = b[i];

        // Convert to lowercase.
        if (ac >= 'A' && ac <= 'Z') ac += 32;
        if (bc >= 'A' && bc <= 'Z') bc += 32;

        if (ac != bc)
            return false;
    }

    return std::strlen(b) == len;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// CommandParser Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to parse a tokenized line into a command.
bool CommandParser::parseLine(const std::vector<ScriptToken> &tokens,
                              size_t lineNumber,
                              ParsedCommand &commandOut,
                              std::string &errorOut)
{
    if (tokens.empty())
    {
        errorOut = "No tokens to parse.";
        return false;
    }

    commandOut.Arguments.clear();
    commandOut.LineNumber = lineNumber;

    const std::string_view &verb = tokens[0].Text;

    // Handle "config" as a two-word command.
    if (equalsIgnoreCase(verb, "config"))
    {
        if (tokens.size() < 2)
        {
            errorOut = "'config' requires a sub-command (model, cpu, rom, ram).";
            return false;
        }

        const std::string_view &subCmd = tokens[1].Text;

        if (equalsIgnoreCase(subCmd, "model"))
            commandOut.Command = DbgCommand::ConfigModel;
        else if (equalsIgnoreCase(subCmd, "cpu"))
            commandOut.Command = DbgCommand::ConfigCpu;
        else if (equalsIgnoreCase(subCmd, "rom"))
            commandOut.Command = DbgCommand::ConfigRom;
        else if (equalsIgnoreCase(subCmd, "ram"))
            commandOut.Command = DbgCommand::ConfigRam;
        else
        {
            errorOut = "Unknown config sub-command '";
            errorOut.append(subCmd.data(), subCmd.size());
            errorOut += "'. Expected: model, cpu, rom, ram.";
            return false;
        }

        // Remaining tokens are arguments.
        for (size_t i = 2; i < tokens.size(); ++i)
        {
            commandOut.Arguments.emplace_back(tokens[i].Text);
        }

        return true;
    }

    // Single-word commands.
    struct CommandMapping
    {
        const char *Name;
        DbgCommand Command;
    };

    static const CommandMapping mappings[] = {
        { "init",     DbgCommand::Init },
        { "run",      DbgCommand::Run },
        { "step",     DbgCommand::Step },
        { "continue", DbgCommand::Continue },
        { "break",    DbgCommand::Break },
        { "watch",    DbgCommand::Watch },
        { "regs",     DbgCommand::Regs },
        { "reg",      DbgCommand::Reg },
        { "mem",      DbgCommand::Mem },
        { "disasm",   DbgCommand::Disasm },
        { "irq",      DbgCommand::Irq },
        { "memc",     DbgCommand::Memc },
        { "cam",      DbgCommand::Cam },
        { "ioc",      DbgCommand::Ioc },
        { "trace",    DbgCommand::Trace },
        { "pc",       DbgCommand::Pc },
        { "echo",     DbgCommand::Echo },
    };

    for (const auto &mapping : mappings)
    {
        if (equalsIgnoreCase(verb, mapping.Name))
        {
            commandOut.Command = mapping.Command;

            for (size_t i = 1; i < tokens.size(); ++i)
            {
                commandOut.Arguments.emplace_back(tokens[i].Text);
            }

            return true;
        }
    }

    errorOut = "Unknown command '";
    errorOut.append(verb.data(), verb.size());
    errorOut += "'.";
    return false;
}

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

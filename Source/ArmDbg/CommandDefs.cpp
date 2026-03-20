//! @file ArmDbg/CommandDefs.cpp
//! @brief The definition of debugger script command types.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ArmDbg/CommandDefs.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// ParsedCommand Member Definitions
////////////////////////////////////////////////////////////////////////////////
ParsedCommand::ParsedCommand() :
    Command(DbgCommand::Max),
    LineNumber(0)
{
}

//! @brief Returns a human-readable name for a debugger command.
const char *getDbgCommandName(DbgCommand cmd)
{
    switch (cmd)
    {
    case DbgCommand::ConfigModel: return "config model";
    case DbgCommand::ConfigCpu:   return "config cpu";
    case DbgCommand::ConfigRom:   return "config rom";
    case DbgCommand::ConfigRam:   return "config ram";
    case DbgCommand::Init:        return "init";
    case DbgCommand::Run:         return "run";
    case DbgCommand::Step:        return "step";
    case DbgCommand::Continue:    return "continue";
    case DbgCommand::Break:       return "break";
    case DbgCommand::Watch:       return "watch";
    case DbgCommand::Regs:        return "regs";
    case DbgCommand::Reg:         return "reg";
    case DbgCommand::Mem:         return "mem";
    case DbgCommand::Disasm:      return "disasm";
    case DbgCommand::Irq:         return "irq";
    case DbgCommand::Memc:        return "memc";
    case DbgCommand::Cam:         return "cam";
    case DbgCommand::Ioc:         return "ioc";
    case DbgCommand::Trace:       return "trace";
    case DbgCommand::Pc:          return "pc";
    case DbgCommand::Echo:        return "echo";
    default:                      return "unknown";
    }
}

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

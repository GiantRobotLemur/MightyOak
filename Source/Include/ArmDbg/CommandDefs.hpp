//! @file ArmDbg/CommandDefs.hpp
//! @brief The declaration of debugger script command types and structures.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DBG_COMMAND_DEFS_HPP__
#define __ARM_DBG_COMMAND_DEFS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <string>
#include <vector>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Identifies a debugger script command.
enum class DbgCommand : uint8_t
{
    // System Setup
    ConfigModel,    //!< config model <model>
    ConfigCpu,      //!< config cpu <cpu>
    ConfigRom,      //!< config rom <preset|path>
    ConfigRam,      //!< config ram <sizeKb>
    Init,           //!< init

    // Execution Control
    Run,            //!< run <cycles>
    Step,           //!< step [n]
    Continue,       //!< continue
    Break,          //!< break <addr>
    Watch,          //!< watch <addr> [read|write|both]

    // State Inspection
    Regs,           //!< regs
    Reg,            //!< reg <name>
    Mem,            //!< mem <addr> <count> [phys|log]
    Disasm,         //!< disasm <addr> <count> [phys|log]
    Irq,            //!< irq
    Memc,           //!< memc
    Ioc,            //!< ioc
    Trace,          //!< trace <count>
    Pc,             //!< pc

    // Output Control
    Echo,           //!< echo <message>

    Max,
};

//! @brief Holds the parsed representation of a single script command.
struct ParsedCommand
{
    DbgCommand Command;                     //!< The command type.
    std::vector<std::string> Arguments;     //!< Positional arguments.
    size_t LineNumber;                       //!< Source line number for errors.

    ParsedCommand();
};

//! @brief Returns a human-readable name for a debugger command.
const char *getDbgCommandName(DbgCommand cmd);

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

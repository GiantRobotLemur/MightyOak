//! @file ArmDbg/DebugSession.hpp
//! @brief The declaration of a class which manages an interactive debug session
//! against an emulated ARM system.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DBG_DEBUG_SESSION_HPP__
#define __ARM_DBG_DEBUG_SESSION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

#include "Ag/Core/FsPath.hpp"

#include "ArmEmu/EmuOptions.hpp"
#include "ArmEmu/ArmSystem.hpp"

#include "ArmDbg/CommandDefs.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class RingBufferTrace;

//! @brief Manages the life cycle of a debugger session: configuration,
//! system creation, command execution, and output.
class DebugSession
{
public:
    // Construction/Destruction
    DebugSession(std::ostream &output);
    ~DebugSession();

    // Accessors
    bool isInitialised() const;

    // Operations
    //! @brief Executes a single parsed command.
    bool executeCommand(const ParsedCommand &cmd);

    //! @brief Override the ROM search path.
    void setRomSearchPath(const Ag::Fs::Path &path);

private:
    // Internal Functions
    bool executeConfigModel(const ParsedCommand &cmd);
    bool executeConfigCpu(const ParsedCommand &cmd);
    bool executeConfigRom(const ParsedCommand &cmd);
    bool executeConfigRam(const ParsedCommand &cmd);
    bool executeInit(const ParsedCommand &cmd);
    bool executeRun(const ParsedCommand &cmd);
    bool executeStep(const ParsedCommand &cmd);
    bool executeContinue(const ParsedCommand &cmd);
    bool executeBreak(const ParsedCommand &cmd);
    bool executeRegs(const ParsedCommand &cmd);
    bool executeReg(const ParsedCommand &cmd);
    bool executeMem(const ParsedCommand &cmd);
    bool executeDisasm(const ParsedCommand &cmd);
    bool executeIrq(const ParsedCommand &cmd);
    bool executeMemc(const ParsedCommand &cmd);
    bool executeIoc(const ParsedCommand &cmd);
    bool executeTrace(const ParsedCommand &cmd);
    bool executePc(const ParsedCommand &cmd);
    bool executeEcho(const ParsedCommand &cmd);

    bool requireInit(const char *commandName);
    bool parseAddress(const std::string &text, uint32_t &addr);
    bool parseUint32(const std::string &text, uint32_t &value);

    // Internal Fields
    std::ostream &_output;
    Options _options;
    IArmSystemUPtr _system;
    std::unique_ptr<RingBufferTrace> _traceOwner;
    RingBufferTrace *_trace;
    Ag::Fs::Path _romSearchPath;
    bool _initialised;
};

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

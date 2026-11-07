//! @file ArmEmu/WatchpointManager.hpp
//! @brief The declaration of an object which manages memory and register
//! watchpoints for the debugger.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_WATCHPOINT_MANAGER_HPP__
#define __ARM_EMU_WATCHPOINT_MANAGER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <vector>

#include "ArmEmu/AddressMap.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the type of memory access a watchpoint monitors.
enum class WatchpointType : uint8_t
{
    Write,
    Read,
    Both,
};

//! @brief Describes a memory watchpoint.
struct MemoryWatchpoint
{
    uint32_t Address;   //!< Logical address to watch.
    uint32_t Id;        //!< Unique watchpoint identifier.
    WatchpointType Type;
};

//! @brief Describes a register watchpoint.
struct RegisterWatchpoint
{
    uint32_t MatchValue;    //!< Value to match (only if MatchSpecific is true).
    uint32_t Id;            //!< Unique watchpoint identifier.
    uint8_t RegisterId;     //!< Register index (0-15).
    bool MatchSpecific;     //!< True to match a specific value, false for any change.
};

//! @brief Details of the last watchpoint hit.
struct WatchpointHit
{
    uint32_t Id;            //!< ID of the watchpoint that triggered.
    uint32_t Address;       //!< Address or register index.
    uint32_t NewValue;      //!< Value that triggered the hit.
    uint32_t OldValue;      //!< Previous value (for register watchpoints).
    bool IsWrite;           //!< True if triggered by a write.
    bool IsRegister;        //!< True if this is a register watchpoint hit.
    bool IsValid;           //!< True if this structure contains valid hit data.
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class SystemContext;

//! @brief Manages memory and register watchpoints for the debugger.
class WatchpointManager : public IHardwareDevice
{
public:
    // Construction/Destruction
    WatchpointManager();
    ~WatchpointManager() = default;

    // Accessors
    bool hasMemoryWatchpoints() const;
    bool hasRegisterWatchpoints() const;
    const std::vector<MemoryWatchpoint> &getMemoryWatchpoints() const;
    const std::vector<RegisterWatchpoint> &getRegisterWatchpoints() const;
    const WatchpointHit &getLastHit() const;

    // Operations
    uint32_t addMemoryWatchpoint(uint32_t address, WatchpointType type);
    uint32_t addRegisterWatchpoint(uint8_t regId, uint32_t matchValue,
                                   bool matchSpecific);
    bool removeWatchpoint(uint32_t id);
    void clearAll();

    bool checkMemoryWrite(uint32_t address, uint32_t size, uint32_t value);
    bool checkMemoryRead(uint32_t address, uint32_t size, uint32_t value);
    bool checkRegister(uint8_t regId, uint32_t newValue);

    // Breakpoint operations
    uint32_t addBreakpoint(uint32_t address);
    bool removeBreakpoint(uint32_t id);
    bool hasBreakpoints() const;
    bool checkPC(uint32_t pc) const;

    // IHardwareDevice Overrides
    Ag::string_cref_t getName() const override;
    Ag::string_cref_t getDescription() const override;
    void registerDevice(SystemContext &context) override;

private:
    std::vector<MemoryWatchpoint> _memoryWatchpoints;
    std::vector<RegisterWatchpoint> _registerWatchpoints;
    std::vector<std::pair<uint32_t, uint32_t>> _breakpoints; //!< {address, id} pairs.
    uint32_t _registerShadow[16]; //!< Shadow copies for change detection.
    WatchpointHit _lastHit;
    uint32_t _nextId;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

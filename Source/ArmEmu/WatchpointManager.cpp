//! @file ArmEmu/WatchpointManager.cpp
//! @brief The definition of an object which manages memory and register
//! watchpoints for the debugger.
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
#include <cstring>

#include "ArmEmu/WatchpointManager.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
static Ag::String g_watchpointMgrName("WatchpointManager");
static Ag::String g_watchpointMgrDesc("Manages memory and register watchpoints.");

////////////////////////////////////////////////////////////////////////////////
// WatchpointManager Member Definitions
////////////////////////////////////////////////////////////////////////////////
WatchpointManager::WatchpointManager() :
    _nextId(1)
{
    std::memset(_registerShadow, 0, sizeof(_registerShadow));
    std::memset(&_lastHit, 0, sizeof(_lastHit));
}

bool WatchpointManager::hasMemoryWatchpoints() const
{
    return !_memoryWatchpoints.empty();
}

bool WatchpointManager::hasRegisterWatchpoints() const
{
    return !_registerWatchpoints.empty();
}

const std::vector<MemoryWatchpoint> &WatchpointManager::getMemoryWatchpoints() const
{
    return _memoryWatchpoints;
}

const std::vector<RegisterWatchpoint> &WatchpointManager::getRegisterWatchpoints() const
{
    return _registerWatchpoints;
}

const WatchpointHit &WatchpointManager::getLastHit() const
{
    return _lastHit;
}

uint32_t WatchpointManager::addMemoryWatchpoint(uint32_t address,
                                                 WatchpointType type)
{
    uint32_t id = _nextId++;

    MemoryWatchpoint wp;
    wp.Address = address;
    wp.Id = id;
    wp.Type = type;
    _memoryWatchpoints.push_back(wp);

    return id;
}

uint32_t WatchpointManager::addRegisterWatchpoint(uint8_t regId,
                                                   uint32_t matchValue,
                                                   bool matchSpecific)
{
    uint32_t id = _nextId++;

    RegisterWatchpoint wp;
    wp.MatchValue = matchValue;
    wp.Id = id;
    wp.RegisterId = regId;
    wp.MatchSpecific = matchSpecific;
    _registerWatchpoints.push_back(wp);

    return id;
}

bool WatchpointManager::removeWatchpoint(uint32_t id)
{
    // Search memory watchpoints.
    auto memIt = std::find_if(_memoryWatchpoints.begin(),
                              _memoryWatchpoints.end(),
                              [id](const MemoryWatchpoint &wp) {
                                  return wp.Id == id;
                              });

    if (memIt != _memoryWatchpoints.end())
    {
        _memoryWatchpoints.erase(memIt);
        return true;
    }

    // Search register watchpoints.
    auto regIt = std::find_if(_registerWatchpoints.begin(),
                              _registerWatchpoints.end(),
                              [id](const RegisterWatchpoint &wp) {
                                  return wp.Id == id;
                              });

    if (regIt != _registerWatchpoints.end())
    {
        _registerWatchpoints.erase(regIt);
        return true;
    }

    return false;
}

void WatchpointManager::clearAll()
{
    _memoryWatchpoints.clear();
    _registerWatchpoints.clear();
    _breakpoints.clear();
    _lastHit.IsValid = false;
}

//! @brief Checks whether a memory write hits any active write watchpoints.
//! @param[in] address The logical address being written to.
//! @param[in] size The size of the write in bytes.
//! @param[in] value The value being written (low bytes significant).
//! @retval true A watchpoint was hit.
//! @retval false No watchpoint was triggered.
bool WatchpointManager::checkMemoryWrite(uint32_t address, uint32_t size,
                                          uint32_t value)
{
    for (const auto &wp : _memoryWatchpoints)
    {
        if (wp.Type == WatchpointType::Read)
            continue;

        // Check for overlap: [address, address+size) intersects [wp.Address, wp.Address+4)
        if (address < wp.Address + 4 && address + size > wp.Address)
        {
            _lastHit.Id = wp.Id;
            _lastHit.Address = wp.Address;
            _lastHit.NewValue = value;
            _lastHit.OldValue = 0;
            _lastHit.IsWrite = true;
            _lastHit.IsRegister = false;
            _lastHit.IsValid = true;
            return true;
        }
    }

    return false;
}

//! @brief Checks whether a memory read hits any active read watchpoints.
//! @param[in] address The logical address being read from.
//! @param[in] size The size of the read in bytes.
//! @param[in] value The value that was read (low bytes significant).
//! @retval true A watchpoint was hit.
//! @retval false No watchpoint was triggered.
bool WatchpointManager::checkMemoryRead(uint32_t address, uint32_t size,
                                         uint32_t value)
{
    for (const auto &wp : _memoryWatchpoints)
    {
        if (wp.Type == WatchpointType::Write)
            continue;

        if (address < wp.Address + 4 && address + size > wp.Address)
        {
            _lastHit.Id = wp.Id;
            _lastHit.Address = wp.Address;
            _lastHit.NewValue = value;
            _lastHit.OldValue = 0;
            _lastHit.IsWrite = false;
            _lastHit.IsRegister = false;
            _lastHit.IsValid = true;
            return true;
        }
    }

    return false;
}

//! @brief Checks whether a register value change hits any active register
//! watchpoints.
//! @param[in] regId The register index (0-15).
//! @param[in] newValue The current value of the register.
//! @retval true A watchpoint was hit.
//! @retval false No watchpoint was triggered.
bool WatchpointManager::checkRegister(uint8_t regId, uint32_t newValue)
{
    if (regId >= 16)
        return false;

    uint32_t oldValue = _registerShadow[regId];
    _registerShadow[regId] = newValue;

    if (oldValue == newValue)
        return false;

    for (const auto &wp : _registerWatchpoints)
    {
        if (wp.RegisterId != regId)
            continue;

        if (wp.MatchSpecific)
        {
            if (newValue == wp.MatchValue)
            {
                _lastHit.Id = wp.Id;
                _lastHit.Address = regId;
                _lastHit.NewValue = newValue;
                _lastHit.OldValue = oldValue;
                _lastHit.IsWrite = true;
                _lastHit.IsRegister = true;
                _lastHit.IsValid = true;
                return true;
            }
        }
        else
        {
            // Match any change.
            _lastHit.Id = wp.Id;
            _lastHit.Address = regId;
            _lastHit.NewValue = newValue;
            _lastHit.OldValue = oldValue;
            _lastHit.IsWrite = true;
            _lastHit.IsRegister = true;
            _lastHit.IsValid = true;
            return true;
        }
    }

    return false;
}

uint32_t WatchpointManager::addBreakpoint(uint32_t address)
{
    uint32_t id = _nextId++;
    _breakpoints.emplace_back(address, id);
    return id;
}

bool WatchpointManager::removeBreakpoint(uint32_t id)
{
    auto it = std::find_if(_breakpoints.begin(), _breakpoints.end(),
                           [id](const std::pair<uint32_t, uint32_t> &bp) {
                               return bp.second == id;
                           });

    if (it != _breakpoints.end())
    {
        _breakpoints.erase(it);
        return true;
    }

    return false;
}

bool WatchpointManager::hasBreakpoints() const
{
    return !_breakpoints.empty();
}

bool WatchpointManager::checkPC(uint32_t pc) const
{
    // In 26-bit mode, the PC from the pipeline includes PSR bits
    // in bits 31:26 and 1:0. Mask to get just the address (bits 25:2).
    uint32_t maskedPC = pc & 0x03FFFFFC;

    for (const auto &bp : _breakpoints)
    {
        if ((bp.first & 0x03FFFFFC) == maskedPC)
            return true;
    }

    return false;
}

void WatchpointManager::registerDevice(SystemContext &context)
{
    context.addDevice(this);
    context.addDeviceAlias(getName(), g_watchpointMgrName);
}

Ag::string_cref_t WatchpointManager::getName() const
{
    return g_watchpointMgrName;
}

Ag::string_cref_t WatchpointManager::getDescription() const
{
    return g_watchpointMgrDesc;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

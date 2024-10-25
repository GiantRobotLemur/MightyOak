//! @file ArmEmu/Test/LoggerDevice.cpp
//! @brief The definition of a device within an emulated system which can
//! log output to the host.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "LoggerDevice.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// LoggerDevice Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an emulation of a memory mapped I/O device which can
//! allow code running on an emulated system to interact with a host test harness.
LoggerDevice::LoggerDevice() :
    _parentContext(nullptr)
{
}

// Inherited from IAddressRegion.
RegionType LoggerDevice::getType() const { return RegionType::MMIO; }

// Inherited from IAddressRegion.
Ag::string_cref_t LoggerDevice::getName() const
{
    static const Ag::String name("LoggerDevice");

    return name;
}

// Inherited from IAddressRegion.
Ag::string_cref_t LoggerDevice::getDescription() const
{
    static const Ag::String desc("A device to interface with emulated ARM systems under test.");

    return desc;
}

// Inherited from IAddressRegion.
uint32_t LoggerDevice::getSize() const
{
    return 16;
}

// Inherited from IMMIOBlock.
uint32_t LoggerDevice::read(uint32_t offset)
{
    uint32_t result = 0;

    switch (offset >> 2)
    {
    case 0: // ID Register. Read-only. Always returns Dead Beef.
        result = 0xDEADBEEF;
        break;

    default:
        break;
    }

    return result;
}

// Inherited from IMMIOBlock.
void LoggerDevice::write(uint32_t offset, uint32_t /*value*/)
{
    switch (offset >> 2)
    {
    case 0: // Command Register. Receives 32-bit command values.
        break;

    default:
        break;
    }
}

// Inherited from IMMIOBlock.
void LoggerDevice::connect(const ConnectionContext &context)
{
    // Connect the device to the host system.
    _parentContext = context.getInteropContext();
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


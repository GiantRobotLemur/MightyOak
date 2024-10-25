//! @file ArmDebugger/EmulatorIOAdapter.hpp
//! @brief The declaration of a base class which provides a unified I/O
//! interface to different models of emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_EMULATOR_IO_ADAPTER_HPP__
#define __ARM_DEBUGGER_EMULATOR_IO_ADAPTER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include "ArmEmu/GuestEventQueue.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A base class which provides a unified I/O interface to different
//! models of emulated system.
class IEmulatorIOAdapter
{
public:
    // Construction/Destruction
    virtual ~IEmulatorIOAdapter() = default;

    // Accessors

    // Operations
    virtual bool handleGuestEvent(const Arm::GuestEvent &args) =0;

    // Overrides
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
};

using IEmulatorIOAdapterUPtr = std::unique_ptr<IEmulatorIOAdapter>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
template<typename TDevice>
bool tryFindDevice(Arm::IArmSystem *emulator, const std::string_view &name,
                   TDevice *&device)
{
    device = nullptr;
    Arm::IMMIOBlockPtr rawDevice = nullptr;

    if (Arm::tryFindDeviceByName(emulator, name, rawDevice))
    {
        device = dynamic_cast<TDevice *>(rawDevice);
    }

    return device != nullptr;
}

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

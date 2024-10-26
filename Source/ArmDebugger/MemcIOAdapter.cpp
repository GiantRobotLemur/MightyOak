//! @file ArmDebugger/MemcIOAdapter.cpp
//! @brief The definition of an object which allows generic access to the I/O
//! interfaces of a MEMC-based emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"

#include "ArmEmu/HostMessageID.hpp"

#include "MemcIOAdapter.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

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
// MemcIOAdapter Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
MemcIOAdapter::MemcIOAdapter(Arm::IArmSystem *emulatedSystem,
                             const Arm::Options &/*options*/) :
    _ioc(nullptr),
    _vidc(nullptr)
{
    if (tryFindDevice(emulatedSystem, "IOC", _ioc) == false)
    {
        Ag::OperationException("Failed to connect to the IOC device in the emulated system.");
    }
    else if (tryFindDevice(emulatedSystem, "VIDC", _ioc) == false)
    {
        Ag::OperationException("Failed to connect to the VIDC device in the emulated system.");
    }
}

bool MemcIOAdapter::handleGuestEvent(const Arm::GuestEvent &args)
{
    bool isHandled = false;

    //switch (args.Type)
    //{
    //case Arm::HostMessageID::KartTxByte:
    //    // uint8_t txByte = static_cast<uint8_t>(args.Data1);
    //    isHandled = true;
    //    break;
    //}

    return isHandled;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

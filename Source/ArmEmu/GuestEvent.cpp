//! @file ArmEmu/GuestEvent.cpp
//! @brief The definition of an object which manages messages marshalled out
//! of the emulator thread and into an observer thread.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/GuestEvent.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// BaseGuestEvent Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty guest event.
BaseGuestEvent::BaseGuestEvent() :
    Timestamp(0),
    Data1(0),
    Data2(0),
    Type(0)
{
}

//! @brief Constructs an initialised guest event.
//! @param timestamp The source of the event.
//! @param type The time at which the event was generated in master clock ticks
//! of the guest system.
//! @param data1 The first event-type-specific parameter.
//! @param data2 The second event-type-specific parameter.
BaseGuestEvent::BaseGuestEvent(uintptr_t timestamp, int32_t type,
                               uintptr_t data1, uintptr_t data2) :
    Timestamp(timestamp),
    Data1(data1),
    Data2(data2),
    Type(type)
{
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


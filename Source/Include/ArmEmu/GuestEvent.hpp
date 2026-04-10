//! @file ArmEmu/GuestEvent.hpp
//! @brief The declaration of an object which manages messages marshalled out
//! of the emulator thread and into an observer thread.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_GUEST_EVENT_HPP__
#define __ARM_EMU_GUEST_EVENT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/AlignedTypes.hpp"
#include "Ag/Core/Memory.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A structure defining an event produced by an emulated system.
struct BaseGuestEvent
{
    //! @brief The time at which the event was generated in master clock ticks
    //! of the guest system.
    uintptr_t Timestamp;

    //! @brief The first event-type-specific parameter.
    uintptr_t Data1;

    //! @brief The second event-type-specific parameter.
    uintptr_t Data2;

    //! @brief See the HostMessageID enumeration for useful values.
    uint32_t Type;

    BaseGuestEvent();
    BaseGuestEvent(uintptr_t timeStamp, int32_t type,
                   uintptr_t data1, uintptr_t data2);
};

//! @brief A possibly padded version of GuestEvent.
using GuestEvent = Ag::AlignedBaseN_t<BaseGuestEvent, 16>;

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/IHostConnection.hpp
//! @brief The declaration of an interface to an object which connects an
//! emulated guest system to its host.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_IHOST_SYSTEM_HPP__
#define __ARM_EMU_IHOST_SYSTEM_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <Ag/Core.hpp>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class IArmSystem;

//! @brief An interface to an object representing that which hosts a running
//! IArmSystem implementation, to provide feedback.
class IHostConnection : public Ag::SharedVirtualBase
{
protected:
    // Construction/Destruction
    IHostConnection() = default;
public:
    virtual ~IHostConnection() = default;

    // Overrides

    //! @brief Is called when a running system generates an event to be fed
    //! back to the host, possibly being marshalled to another thread.
    //! @param[in] instance A pointer to the system which generated the event.
    //! @param[in] id The identifier of the event, possibly based on the
    //! HostMessageID enumeration.
    //! @param[in] param1 The first message-specific parameter.
    //! @param[in] param2 The second message-specific parameter.
    virtual void onGuestEvent(IArmSystem *instance, uint32_t id,
                              uintptr_t param1, uintptr_t param2) = 0;
};

using IHostConnectionSPtr = std::shared_ptr<IHostConnection>;
using IHostConnectionWPtr = std::weak_ptr<IHostConnection>;

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

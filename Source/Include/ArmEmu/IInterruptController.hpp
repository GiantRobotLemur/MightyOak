//! @file ArmEmu/IInterruptController.hpp
//! @brief The declaration of an abstraction of an object which emulates a
//! hardware component which marshals interrupt sources into the IRQ an FIRQ
//! pins of the emulated ARM processor.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_IINTERRUPT_CONTROLLER_HPP__
#define __ARM_EMU_IINTERRUPT_CONTROLLER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AddressMap.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An abstraction of an object which emulates a hardware component which
//! marshals interrupt sources into the IRQ an FIRQ pins of the emulated
//! ARM processor.
class IInterruptController : public IMMIOBlock
{
protected:
    // Construction/Destruction
    IInterruptController() = default;

public:
    virtual ~IInterruptController() = default;

    // Accessors

    //! @brief Sets whether the display output has finished reading output for
    //! a frame and is in the VSync period before starting to display the next
    //! frame.
    //! @param[in] isInProgress True if VSync is in progress, possibly
    //! triggering a level-triggered IRQ, false is VSync has finished and the
    //! new frame is being read from the frame buffer.
    virtual void setVSyncState(bool isInProgress) = 0;

    // Operations

    //! @brief Raises an interrupt which, if unmasked, will interrupt the
    //! emulated processor.
    //! @param[in] id The 0-based index of the interrupt to raise.
    virtual void raiseIrq(uint8_t id) = 0;

    //! @brief Raises a fast interrupt which, if unmasked, will interrupt the
    //! emulated processor.
    //! @param[in] id The 0-based index of the fast interrupt to raise.
    virtual void raiseFirq(uint8_t id) = 0;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

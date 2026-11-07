//! @file ArmEmu/HostMessageID.hpp
//! @brief The declaration of identifiers for specific messages sent from a
//! running emulator to the host system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_HOST_MESSAGE_ID_HPP__
#define __ARM_EMU_HOST_MESSAGE_ID_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines identifiers for specific messages sent from a running
//! emulator to the host system.
enum HostMessageID : uint32_t
{
    //! @brief A VSync event has occurred. The host should render a new frame.
    VSyncOccurred,

    //! @brief A complete frame has been rendered and is ready for display.
    //! @remarks
    //! The first parameter contains the index of the frame which has finished
    //! being sampled.
    FrameBufferReady,

    //! @brief Indicates a change in the state of keyboard LEDs.
    //! @remarks
    //! The first parameter contains a IKeyboardController::LEDStateBits value.
    KeyboardLEDsChanged,



    //! @brief A place-holder value, which should always be last.
    LastHostMessage
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

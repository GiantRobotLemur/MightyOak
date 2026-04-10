//! @file ArmEmu.hpp
//! @brief The declaration of public elements of the ArmEmu library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_HPP__
#define __ARM_EMU_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/ExecutionMetrics.hpp"
#include "ArmEmu/EmuOptions.hpp"
#include "ArmEmu/HostMessageID.hpp"
#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/IInterruptController.hpp"
#include "ArmEmu/IKeyboardController.hpp"
#include "ArmEmu/IVideoFrameProvider.hpp"
#include "ArmEmu/GuestEvent.hpp"
#include "ArmEmu/SystemContext.hpp"
#include "ArmEmu/IHostConnection.hpp"
#include "ArmEmu/ArmSystem.hpp"
#include "ArmEmu/ArmSystemBuilder.hpp"
#include "ArmEmu/IDiagnosticSink.hpp"
#include "ArmEmu/RingBufferTrace.hpp"
#include "ArmEmu/BootProgressMonitor.hpp"
#include "ArmEmu/BootStallDetector.hpp"
#include "ArmEmu/CompositeDiagnosticSink.hpp"
#include "ArmEmu/WatchpointManager.hpp"

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

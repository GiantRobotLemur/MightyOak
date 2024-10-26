//! @file ArmEmu/GuestEventQueue.hpp
//! @brief The declaration of an object which manages messages marshalled out
//! of the emulator thread and into an observer thread.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_GUEST_EVENT_QUEUE_HPP__
#define __ARM_EMU_GUEST_EVENT_QUEUE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "readerwriterqueue.h"

#include "Ag/Core/Memory.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A structure defining an event produced by an emulated system.
struct GuestEvent
{
    uintptr_t SourceID;
    uintptr_t Data1;
    uintptr_t Data2;

    //! @brief See the HostMessageID enumeration for useful values.
    uint32_t Type;
#if INTPTR_MAX > 0xFFFFFFFF
    // Only pad on 64-bit architectures.
    uint32_t Padding;
#endif

    GuestEvent();
    GuestEvent(uintptr_t sourceID, int32_t type,
               uintptr_t data1, uintptr_t data2);
};

//! @brief An object which manages messages marshalled out of the emulator
//! thread and into an observer thread.
class GuestEventQueue
{
public:
    // Construction/Destruction
    GuestEventQueue();
    GuestEventQueue(uintptr_t sourceID);
    ~GuestEventQueue() = default;

    // Accessors
    uintptr_t getSourceID() const;
    void setSourceID(uintptr_t sourceID);

    // Operations
    bool enque(int32_t type, uintptr_t data1, uintptr_t data2);
    bool tryDeque(GuestEvent &next);

private:
    // Internal Types
    using Queue = moodycamel::ReaderWriterQueue<GuestEvent>;

    // Internal Fields
    Queue _queue;
    uintptr_t _sourceID;
};

using GuestEventQueueUPtr = std::unique_ptr<GuestEventQueue, Ag::AlignedDeleter<GuestEventQueue>>;

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

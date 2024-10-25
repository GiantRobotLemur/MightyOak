//! @file ArmEmu/GuestEventQueue.cpp
//! @brief The definition of an object which manages messages marshalled out
//! of the emulator thread and into an observer thread.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/GuestEventQueue.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// GuestEvent Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty guest event.
GuestEvent::GuestEvent() :
    SourceID(0),
    Data1(0),
    Data2(0),
    Type(0)
#if INTPTR_MAX > 0xFFFFFFFF
    // Only pad on 64-bit architectures.
  , Padding(0)
#endif
{
}

//! @brief Constructs an initialised guest event.
//! @param sourceID The source of the event.
//! @param type The type of the event.
//! @param data1 The first event-type-specific parameter.
//! @param data2 The second event-type-specific parameter.
GuestEvent::GuestEvent(uintptr_t sourceID, int32_t type,
                       uintptr_t data1, uintptr_t data2) :
    SourceID(sourceID),
    Data1(data1),
    Data2(data2),
    Type(type)
#if INTPTR_MAX > 0xFFFFFFFF
    // Only pad on 64-bit architectures.
    , Padding(0)
#endif
{
}

////////////////////////////////////////////////////////////////////////////////
// GuestEventQueue Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty inter-thread event queue.
GuestEventQueue::GuestEventQueue() :
    _queue(63),
    _sourceID(0)
{
}

//! @brief Constructs an empty event queue.
//! @param[in] sourceID The identifier which tags every message added to
//! the queue.
GuestEventQueue::GuestEventQueue(uintptr_t sourceID) :
    _queue(63),
    _sourceID(sourceID)
{
}

//! @brief Sets the source identifier which all messages are tagged with.
uintptr_t GuestEventQueue::getSourceID() const { return _sourceID; }

//! @brief Gets the source identifier which all messages are tagged with.
//! @param[in] sourceID The new source identifier to be applied to all
//! new messages.
void GuestEventQueue::setSourceID(uintptr_t sourceID)
{
    _sourceID = sourceID;
}

//! @brief Attempts to add a guest event to the queue.
//! @param[in] type The type of event to add.
//! @param[in] data1 The first event-type-specific parameter.
//! @param[in] data2 The second event-type-specific parameter.
//! @retval true The event was successfully added to the queue.
//! @retval false The queue was full and could not be extended quickly, so the
//! event was not queued.
bool GuestEventQueue::enque(int32_t type, uintptr_t data1, uintptr_t data2)
{
    return _queue.try_emplace(_sourceID, type, data1, data2);
}

//! @brief Attempts to retrieve an item from the queue.
//! @param[out] next A structure to receive the next event, if there was one.
//! @retval true An event was successfully extracted from the queue.
//! @retval false There were no events waiting in the queue.
bool GuestEventQueue::tryDeque(GuestEvent &next)
{
    return _queue.try_dequeue(next);
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


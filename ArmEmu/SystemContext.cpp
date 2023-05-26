//! @file ArmEmu/SystemContext.cpp
//! @brief The definition of an object which shares information between
//! internal components of an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/EmuOptions.hpp"
#include "ArmEmu/GuestEventQueue.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// SystemContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which provides communications with the host
//! system to emulated hardware devices.
//! @param[in] options An object describing the system being emulated.
//! @param[in] eventQueue A reference to the FIFO used to pass message to the
//! host input thread.
SystemContext::SystemContext(const Options &sysConfig,
                             GuestEventQueue &eventQueue) :
    _eventQueue(eventQueue),
    _taskQueueHead(nullptr),
    _masterClock(0),
    _masterFreq(sysConfig.getProcessorSpeedMHz() * 1000000u),
    _cpuClockShift(0),
    _fuzzIndex(0)
{
    // Aim for a minimum master clock frequency of either 4 x CPU frequency or
    // 100 MHz, whichever is greater.
    uint64_t minFreq = std::max<uint64_t>(100000000, _masterFreq * 4);

    while (_masterFreq <= minFreq)
    {
        _masterFreq <<= 1;
        ++_cpuClockShift;
    }

    // Generate random noise to return be reads to undefined I/O memory.
    for (size_t i = 0; i < FuzzSize; ++i)
    {
        uint32_t fuzz = static_cast<uint32_t>(std::rand() % 255) |
                        (static_cast<uint32_t>(std::rand() % 255) << 8) |
                        (static_cast<uint32_t>(std::rand() % 255) << 16) |
                        (static_cast<uint32_t>(std::rand() % 255) << 24);

        _fuzz[i] = fuzz;
    }
}

//! @brief Gets the count of CPU cycles elapsed since the emulated system
//! was started.
//! @note This count is derived by scaling the master clock.
uint64_t SystemContext::getCPUClockTicks() const
{
    return _masterClock >> _cpuClockShift;
}

//! @brief Gets the count of system ticks elapsed since the emulated
//! system started.
uint64_t SystemContext::getMasterClockTicks() const
{
    return _masterClock;
}

//! @brief Gets the count of master clock ticker per second.
uint64_t SystemContext::getMasterClockFrequency() const
{
    return _masterFreq;
}

//! @brief Gets random data to report by reads to assigned regions of memory.
//! @return A random 32-bit value which changes after each call.
uint32_t SystemContext::getFuzz()
{
    uint32_t fuzz = _fuzz[_fuzzIndex++];

    // Ensure the index wraps around.
    _fuzzIndex &= FuzzSizeMask;

    return fuzz;
}

//! @brief Increments the master system clock.
//! @param[in] cycles The count of CPU cycles to add.
//! @note The CPU clock frequency will be less than the master clock frequency.
void SystemContext::incrementCPUClock(uint32_t cycles)
{
    _masterClock += static_cast<uint64_t>(cycles) << _cpuClockShift;

    // Perform an scheduled tasks which are now pending.
    while ((_taskQueueHead != nullptr) &&
           (_taskQueueHead->At <= _masterClock))
    {
        // Pop the head task.
        GuestTask *headTask = _taskQueueHead;
        _taskQueueHead = headTask->Next;

        // Perform the task.
        headTask->Task(*this, headTask->Context);
    }
}

//! @brief Schedules a task to be executed at a specific time.
//! @param[in] task The task description which is owned by the task owner.
void SystemContext::scheduleTask(GuestTask *task)
{
    task->Next = _taskQueueHead;

    if ((_taskQueueHead == nullptr) || (task->At < _taskQueueHead->At))
    {
        // The task goes at the head of the queue.
        _taskQueueHead = task;
    }
    else
    {
        GuestTask *current = _taskQueueHead;

        while ((current->Next != nullptr) && (current->Next->At < task->At))
        {
            current = current->Next;
        }

        // Insert the task into the queue.
        task->Next = current->Next;
        current->Next = task;
    }
}

//! @brief Attempts to post a message to the host input thread without blocking.
//! @param[in] eventID The type of the event to raise.
//! @param[in] data1 The first item of event-specific data.
//! @param[in] data2 The second item of event-specific data.
//! @retval true The message was successfully posted to the input thread.
//! @retval false The FIFO was full, no message could be posted without
//! blocking the current thread to allocate more memory.
bool SystemContext::postMessageToHost(uint32_t eventID, uintptr_t data1,
                                      uintptr_t data2)
{
    return _eventQueue.enque(eventID, data1, data2);
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


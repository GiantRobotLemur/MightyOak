//! @file ArmEmu/SystemContext.cpp
//! @brief The definition of an object which shares information between
//! internal components of an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
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
#include "ArmEmu/AddressMap.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// GuestTask Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty task in an unscheduled state.
GuestTask::GuestTask() :
    _at(0),
    _next(nullptr),
    _context(0),
    _task(nullptr)
{
}

//! @brief Defines the task to execute when the task reaches the front of the
//! queue.
//! @param[in] task A pointer to the function to execute.
//! @param[in] context The context to pass to the task function.
void GuestTask::defineTask(TaskFn task, uintptr_t context)
{
    _task = task;
    _context = context;
}

//! @brief Defines the task to execute when the task reaches the front of the
//! queue.
//! @param[in] task A pointer to the function to execute.
//! @param[in] context The context to pass to the task function.
void GuestTask::defineTask(TaskFn task, const void *context)
{
    _task = task;
    _context = reinterpret_cast<uintptr_t>(context);
}

//! @brief Adds the task to a linked list of scheduled tasks.
//! @param[in,out] listHead The item at the start of the singly-linked list of
//! scheduled tasks.
//! @param[in] time The absolute time in master clock ticks at which the task
//! function should be executed after.
void GuestTask::schedule(GuestTask *&listHead, uint64_t time)
{
    // The task is already in the queue, remove it.
    if (_at != 0)
        unschedule(listHead);

    // Don't schedule an empty task.
    if (_task == nullptr)
        return;

    // Mark the task as scheduled.
    _at = time;

    if ((listHead == nullptr) || (_at < listHead->_at))
    {
        // The task goes at the head of the queue.
        _next = listHead;
        listHead = this;
    }
    else
    {
        // Find the item which should proceed this task in the queue.
        GuestTask *current = listHead;

        while ((current->_next != nullptr) && (current->_next->_at < _at))
        {
            current = current->_next;
        }

        // Insert the task into the queue.
        _next = current->_next;
        current->_next = this;
    }
}

//! @brief Removes the task from a list, if it was scheduled.
//! @param[in,out] listHead The item at the start of the singly-linked list of
//! scheduled tasks which may contain this task.
//! @retval true The task was in a scheduled state and removed from the list.
//! @retval false The task was not found in the list.
bool GuestTask::unschedule(GuestTask *&listHead)
{
    if ((listHead == nullptr) || (_at == 0))
        return false;

    if (listHead == this)
    {
        // Remove the head task.
        listHead = _next;
        _next = nullptr;

        // Mark the task as unscheduled.
        _at = 0;

        return true;
    }

    GuestTask *current = listHead;

    while (current->_next != nullptr)
    {
        if (current->_next == this)
        {
            current->_next = _next;
            _next = nullptr;

            // Mark the task as unscheduled.
            _at = 0;

            return true;
        }
        else
        {
            // Move on to the next task.
            current = current->_next;
        }
    }

    return false;
}

//! @brief Executes the task.
//! @param[in] sysContext The context of the system the task executes in.
void GuestTask::execute(SystemContext &sysContext)
{
    // Reset this task into an unscheduled state.
    _next = nullptr;
    _at = 0;

    _task(sysContext, _context);
}

////////////////////////////////////////////////////////////////////////////////
// SystemContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which provides communications with the host
//! system to emulated hardware devices.
//! @param[in] options An object describing the system being emulated.
//! @param[in] eventQueue A reference to the FIFO used to pass message to the
//! host input thread.
//! @param[in] parentSystem The emulated system being interfaced.
SystemContext::SystemContext(const Options &sysConfig,
                             GuestEventQueue &eventQueue,
                             IArmSystem *parentSystem) :
    _eventQueue(eventQueue),
    _parentSystem(parentSystem),
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

//! @brief Gets the emulated system being interfaced with.
IArmSystem *SystemContext::getSystem() const
{
    return _parentSystem;
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

//! @brief Attempts to find a hardware device mapped which forms part of
//! the guest system by its name.
//! @param[in] name The device name to search for.
//! @param[out] device Receives a pointer to the matching device if one
//! was found.
//! @retval true A matching device was found and its pointer returned.
//! @retval false name device was found with a matching name.
bool SystemContext::tryFindDevice(Ag::string_cref_t name,
                                  IHardwareDevicePtr &device) const
{
    auto pos = _devicesByName.find(name);

    if (pos == _devicesByName.end())
    {
        device = nullptr;
        return false;
    }
    else
    {
        device = pos->second;
        return true;
    }
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
           _taskQueueHead->canExecute(_masterClock))
    {
        // Pop the head task.
        GuestTask *currentTask = _taskQueueHead;

        // Ensure we update the list head before executing the task,
        // which may update the list head internally.
        _taskQueueHead = currentTask->getNext();
        currentTask->execute(*this);
    }
}

//! @brief Schedules a task to be executed at a specific time.
//! @param[in] task The task description which is owned by the task owner.
//! @param[in] cpuCycleDelta The count of CPU cycles in the future of the
//! time when the task should be executed.
void SystemContext::scheduleTaskDeltaCycles(GuestTask *task,
                                            uint32_t cpuCycleDelta)
{
    scheduleTaskDeltaTicks(task,
                           static_cast<uint64_t>(cpuCycleDelta) << _cpuClockShift);
}

//! @brief Schedules a task to be executed at a specific time.
//! @param[in] task The task description which is owned by the task owner.
//! @param[in] masterTickDelta The count of ticks in the future of the
//! time when the task should be executed.
void SystemContext::scheduleTaskDeltaTicks(GuestTask *task,
                                           uint64_t masterTickDelta)
{
    if (task == nullptr)
        return;

    task->schedule(_taskQueueHead, _masterClock + masterTickDelta);
}

//! @brief Removes a task from the task queue.
//! @param[in] taskToRemove The task to remove.
//! @retval true The task was successfully removed.
//! @retval false The task was not in the queue.
bool SystemContext::SystemContext::unscheduleTask(GuestTask *taskToRemove)
{
    if (taskToRemove == nullptr)
        return false;

    return taskToRemove->unschedule(_taskQueueHead);
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

//! @brief Adds a device to the internal index.
//! @param[in] device The device implementation to add.
//! @throws Ag::OperationException If a device with the same name, but 
//! a different implementation already exists in the index.
void SystemContext::addDevice(IHardwareDevicePtr device)
{
    Ag::string_cref_t deviceName = device->getName();
    auto insertResult = _devicesByName.try_emplace(deviceName, device);

    // Ensure that if the name was already in the map, it referred
    // to the same device.
    if ((insertResult.second == false) &&
        (insertResult.first->second != device))
    {
        std::string message("The device name '");
        Ag::appendAgString(message, deviceName);
        message.append("' refers to multiple entities in the same emulated system.");

        throw Ag::OperationException(std::string_view(message));
    }
}

//! @brief A convenience function which registers all devices in a collection
//! and then calls their connect() member function.
//! @param[in] allDevices A collection of device instances, which will be
//! reordered and updated to remove duplicates.
//! @param[in] resetIndex True to remove all previously registered devices
//! before adding the instances from @p allDevices.
void SystemContext::connectAllDevices(IHardwareDeviceCollection &allDevices,
                                      bool resetIndex /*= true*/)
{
    // Ensure each entry in the list is unique.
    std::sort(allDevices.begin(), allDevices.end());
    auto last = std::unique(allDevices.begin(), allDevices.end());
    allDevices.erase(last, allDevices.end());

    // Connect all devices together and to inter-op services.
    if (resetIndex)
        _devicesByName.clear();

    // Create an index of named devices.
    for (IHardwareDevicePtr device : allDevices)
    {
        device->registerDevice(*this);
    }

    // Allow all devices the change to connect to any other device.
    for (IHardwareDevicePtr device : allDevices)
    {
        device->connect(*this);
    }
}

//! @brief Adds a generic alias name for an existing device.
//! @param[in] deviceName The name of the device to alias.
//! @param[in] alias An alternate name by which the device might be known.
void SystemContext::addDeviceAlias(Ag::string_cref_t deviceName,
                                   Ag::string_cref_t alias)
{
    auto pos = _devicesByName.find(deviceName);

    if (pos == _devicesByName.end())
    {
        std::string message("The device name '");
        Ag::appendAgString(message, deviceName);
        message.append("' does not exist within the emulated system.");

        throw Ag::OperationException(std::string_view(message));
    }

    auto aliasPos = _devicesByName.find(alias);

    if (aliasPos == _devicesByName.end())
    {
        // The device exists and the alias doesn't - add it.
        _devicesByName[alias] = pos->second;
    }
    else if (aliasPos->second != pos->second)
    {
        std::string message("The device name '");
        Ag::appendAgString(message, deviceName);
        message.append("' already exists within the emulated system.");

        throw Ag::OperationException(std::string_view(message));
    }
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


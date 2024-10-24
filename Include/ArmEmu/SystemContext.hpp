//! @file ArmEmu/SystemContext.hpp
//! @brief The declaration of an object which shares information between
//! internal components of an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_SYSTEM_CONTEXT_HPP__
#define __ARM_EMU_SYSTEM_CONTEXT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class IArmSystem;
class GuestEventQueue;
class SystemContext;
class Options;

//! @brief A description of a task scheduled and run on the emulator thread.
struct GuestTask
{
    using TaskFn = void (*)(SystemContext &guestContext, uintptr_t taskContext);

    //! @brief The system clock time at which the task should be executed.
    uint64_t At;

    //! @brief The user-supplied context to pass to the task.
    uintptr_t Context;

    //! @brief The next task in the queue.
    GuestTask *Next;

    //! @brief A pointer to the function to call to dispatch the task.
    TaskFn Task;
};

//! @brief Represents an object used to perform communications between the
//! emulated system and the host system while the emulated system is running.
class SystemContext
{
public:
    // Construction/Destruction
    SystemContext(const Options &sysConfig, GuestEventQueue &eventQueue,
                  IArmSystem *parentSystem);
    ~SystemContext() = default;

    // Accessors
    IArmSystem *getSystem() const;
    uint64_t getCPUClockTicks() const;
    uint64_t getMasterClockTicks() const;
    uint64_t getMasterClockFrequency() const;

    // Operations
    uint32_t getFuzz();
    void incrementCPUClock(uint32_t cycles);
    void scheduleTask(GuestTask *task);
    bool postMessageToHost(uint32_t eventID, uintptr_t data1, uintptr_t data2);
private:
    // Internal Constants
    // Ensure the size of the fuzz array is a power of 2 to allow easy wrapping.
    static constexpr uint8_t FuzzSizePow2 = 6;
    static constexpr uint8_t FuzzSizeMask = (static_cast<uint8_t>(1) << FuzzSizePow2) - 1;
    static constexpr size_t FuzzSize = 1 << FuzzSizePow2;

    // Internal Fields
    GuestEventQueue &_eventQueue;
    IArmSystem *_parentSystem;
    GuestTask *_taskQueueHead;
    uint64_t _masterClock;
    uint64_t _masterFreq;
    uint8_t _cpuClockShift;
    uint8_t _fuzzIndex;
    uint32_t _fuzz[FuzzSize];
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

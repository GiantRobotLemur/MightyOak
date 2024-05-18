//! @file ArmEmu/IOC.hpp
//! @brief The declaration of an object which emulates the function of the
//! VL86C410 IOC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_IOC_HPP__
#define __ARM_EMU_IOC_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <atomic>
#include <new>

#include "ReaderWriterQueue.h"

#include "Ag/Core/Memory.hpp"

#include "AddressMap.hpp"
#include "SystemContext.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class MemcHardware;
class SystemContext;
class AcornKeyboardController;

using SynchronisedByteQueue = moodycamel::ReaderWriterQueue<uint8_t>;

//! @brief A structure representing state which can be read/written by
//! both the emulation and input thread.
class IocIrqState
{
public:
    // Construction/Destruction
    IocIrqState();
    ~IocIrqState() = default;

    // Accessors
    bool getIrqPinState() const;

    uint16_t getUnmaskedIrqState() const;
    uint16_t getMaskedIrqState() const;
    uint16_t getIrqMask() const;
    bool setIrqMaskLow(uint8_t mask);
    bool setIrqMaskHigh(uint8_t mask);
    bool setIrqState(uint8_t irq, bool state);

    bool getFirqPinState() const;
    uint8_t getUnmaskedFirqState() const;
    uint8_t getMaskedFirqState() const;
    uint8_t getFirqMask() const;
    bool setFirqMask(uint8_t mask);
    bool setFirqState(uint8_t irq, bool state);

    uint8_t readCtrlRegister() const;
    void writeCtrlRegister(uint8_t value);
    uint8_t getControlPinOutputState() const;
    bool setControlPinInputState(uint8_t pin, bool state);

    // Operations
    bool clearIrqs(uint8_t mask);
    bool raiseIrq(uint8_t id);
    //bool raiseFirq(uint8_t id);

private:
    // Interrupt management registers.

    //! @brief The current activation of interrupts.
    std::atomic_uint16_t _irqStatus;

    //! @brief The state of the interrupt mask registers (A and B).
    std::atomic_uint16_t _irqMask;

    //! @brief The current activation of fast interrupts.
    std::atomic_uint8_t _firqStatus;

    //! @brief The state of the fast interrupt mask.
    std::atomic_uint8_t _firqMask;

    //! @brief The state of inputs to pins C0-C5 set by external devices.
    //! @note C3-C5 activate FIRQs.
    std::atomic_uint8_t _ctrlInput;

    //! @brief The state of output pins C0-C5.
    std::atomic_uint8_t _ctrlOutput;

    //! @brief Indicates whether pins C0-C5 can receive input (1) or
    //! transmit output (0).
    std::atomic_uint8_t _ctrlState;
};

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4324 ) // Disable: Structure was padded due to alignment specifier
                                  // Caused by aligning structure members on cache lines.
#pragma warning( disable : 4200 ) // Disable: Non-standard extension used: zero-sized array in struct/union
                                  // Caused by Padding having zero length due to a queue
                                  //   having a size which matches a cache line.
#endif

//! @brief A structure combining data structures which need to be
//! operated on by multiple threads and therefore need to be aligned along
//! cache line boundaries.
struct IocSynchronisedState
{
    static constexpr size_t CacheLineMask = ~static_cast<size_t>(std::hardware_destructive_interference_size - 1);
    static constexpr size_t SharedStateSize = (sizeof(IocIrqState) + std::hardware_destructive_interference_size - 1) & CacheLineMask;
    static constexpr size_t QueueSize = (sizeof(SynchronisedByteQueue) + std::hardware_destructive_interference_size - 1) & CacheLineMask;

    alignas(std::hardware_destructive_interference_size) IocIrqState InterruptState;
    alignas(std::hardware_destructive_interference_size) SynchronisedByteQueue RxQueue;
    alignas(std::hardware_destructive_interference_size) SynchronisedByteQueue TxQueue;
    uint8_t Padding[QueueSize - sizeof(SynchronisedByteQueue)];

    IocSynchronisedState() = default;
    ~IocSynchronisedState() = default;
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

using IocSyncStateTraits = Ag::AlignmentTraits<IocSynchronisedState,
                                               std::hardware_destructive_interference_size>;
using IocSyncStatePtr = IocSyncStateTraits::UPtr;

//! @brief An object which emulates the function of the VL86C410 IOC part.
class IOC : public IMMIOBlock
{
public:
    // Construction/Destruction
    IOC() = delete;
    IOC(const IOC &) = delete;
    IOC(IOC &&) = delete;
    IOC &operator=(const IOC &) = delete;
    IOC &operator=(IOC &&) = delete;

    IOC(MemcHardware &parent);
    virtual ~IOC() = default;

    // Accessors
    uint8_t getCtrlPinInputState() const;
    void setCtrlPinInputState(uint8_t pin, bool state);

    // Operations
    void powerOnReset();
    void setInterruptLow(uint8_t ilNo, bool state);
    void setFastHighInterrupt(uint8_t fhNo, bool state);
    void setFastLowInterrupt(bool state);
    void setControlPinInput(uint8_t ctrlLine, bool state);
    void writeKart(const uint8_t *bytes, uint8_t size);
    void writeKartByte(const uint8_t value);
    void flushKart();

    //void raiseIrq(uint8_t id);
    //void raiseFirq(uint8_t id);

    // Overrides
    virtual RegionType getType() const override;
    virtual Ag::string_cref_t getName() const override;
    virtual Ag::string_cref_t getDescription() const override;
    virtual uint32_t getSize() const override;

    virtual uint32_t read(uint32_t offset) override;
    virtual void write(uint32_t offset, uint32_t value) override;
    virtual void connect(const ConnectionContext &context) override;
private:
    // Internal Types
    class Counter
    {
    public:
        // Construction/Destruction
        Counter();
        ~Counter() = default;

        // Accessors
        bool isActive() const;
        uint16_t readCount();
        void writeLatchLow(uint8_t lowLatch);
        void writeLatchHigh(uint8_t highLatch);
        void setTriggerCallback(GuestTask::TaskFn fn, uintptr_t context);

        // Operations
        void go(SystemContext *context);
        void latch(SystemContext *context);

    protected:
        void start(SystemContext *context, uint64_t countFactor);

    private:
        GuestTask _triggerTask;
        uint64_t _masterTicksPerCount;
        uint64_t _startTime;
        uint16_t _inputLatch;
        uint16_t _outputLatch;
    };

    //! @brief A counter used to run the KART serial clock. It only raises
    //! the callback when the timer has cycled enough times to send/receive
    //! a whole data byte.
    class KartCounter : public Counter
    {
    public:
        // Construction/Destruction
        KartCounter() = default;
        ~KartCounter() = default;

        // Operations
        void go(SystemContext *context);
    };

    //! @brief Serves as a set of parameters to a call to onCounterReachesZero().
    struct CounterEventContext
    {
        IOC *Parent;
        uint8_t TimerID;
        uint8_t Irq;
    };

    // Internal Functions
    static void onCounterReachesZero(SystemContext &guestContext, uintptr_t taskContext);
    static void onKartCounterReachesZero(SystemContext &guestContext,
                                         uintptr_t taskContext);

    // Internal Fields
    IocSyncStatePtr _synchronisedData;
    MemcHardware &_parent;
    SystemContext *_context;
    IocIrqState *_irqState;
    AcornKeyboardController *_keyboard;

    // NOTE: These two must be consecutive to appear as an array of Timer[4].
    Counter _counters[3];
    KartCounter _kartCounter;

    CounterEventContext _timer0Context;
    CounterEventContext _timer1Context;
    SynchronisedByteQueue *_kartRxQueue;
    SynchronisedByteQueue *_kartTxQueue;
    uint8_t _kartRxByte;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

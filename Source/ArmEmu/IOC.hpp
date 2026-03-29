//! @file ArmEmu/IOC.hpp
//! @brief The declaration of an object which emulates the function of the
//! VL86C410 IOC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
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

#include "Ag/Core/Memory.hpp"

#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/SystemContext.hpp"
#include "ArmEmu/IDiagnosticSink.hpp"
#include "ArmEmu/IInterruptController.hpp"
#include "AcornKeyboardController.hpp"
#include "I2CBus.hpp"
#include "Hardware.inl"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
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

    uint16_t getIrqState() const;
    uint16_t getIrqRequestState() const;
    uint16_t getIrqMask() const;
    bool setIrqMaskLow(uint8_t mask);
    bool setIrqMaskHigh(uint8_t mask);
    bool setIrqState(uint8_t irq, bool state);

    bool getFirqPinState() const;
    uint8_t getFirqState() const;
    uint8_t getFirqRequestState() const;
    uint8_t getFirqMask() const;
    bool setFirqMask(uint8_t mask);
    bool setFirqState(uint8_t irq, bool state);

    uint8_t readCtrlRegister() const;
    void writeCtrlRegister(uint8_t value);
    uint8_t getControlPinOutputState() const;
    bool setControlPinInputState(uint8_t pin, bool state);

    bool setVSyncState(bool isInProgress);

    // Operations
    bool clearIrqs(uint8_t mask);
    bool raiseIrq(uint8_t id);
    bool raiseFirq(uint8_t id);

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

    //! @brief Sets the state of VSync, possibly triggering an IRQ if the
    //! state transitions from low to high.
    std::atomic_bool _vSyncInProgress;
};

//! @brief A structure combining data structures which need to be
//! operated on by multiple threads and therefore need to be aligned along
//! cache line boundaries.
struct IocSynchronisedState
{
    // Ensure the types are aligned and padded in such a way that they fit
    // into different cache lines.
    using IocStateType = STRUCT_ALIGN(std::hardware_destructive_interference_size) Ag::AlignedBaseN_t<IocIrqState, std::hardware_destructive_interference_size>;
    using ByteQueue = STRUCT_ALIGN(std::hardware_destructive_interference_size) Ag::AlignedBaseN_t<SynchronisedByteQueue, std::hardware_destructive_interference_size>;

    IocStateType InterruptState;
    ByteQueue RxQueue;
    ByteQueue TxQueue;

    IocSynchronisedState() = default;
    ~IocSynchronisedState() = default;
};

// Define a cache line aligned and padded inter-thread data type.
using IocSyncState = Ag::AlignedBaseN_t<IocSynchronisedState, std::hardware_destructive_interference_size>;
using IocSyncStateTraits = Ag::AlignmentTraits<IocSyncState>;
using IocSyncStatePtr = IocSyncStateTraits::UPtr;

//! @brief An object which emulates the function of the VL86C410 IOC part.
//! @tparam TAllowDiagnostics Used with if constexpr to determine of
//! IDiagnosticSink branches should be taken - otherwise they will be
//! optimised out.
template<bool TAllowDiagnostics>
class IOCDevice : public IInterruptController
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Internal Types
    ///////////////////////////////////////////////////////////////////////////
    class Counter
    {
    public:
        // Construction/Destruction

        //! @brief Constructs an object representing a hardware counter.
        Counter() :
            _masterTicksPerCount(1),
            _startTime(0),
            _inputLatch(0),
            _outputLatch(0)
        {
            _triggerTask.defineTask(onCounterReachesZero, this);
        }

        ~Counter() = default;

        // Accessors

        //! @brief Determines if the timer is currently running.
        //! @retval true The timer is counting down.
        //! @retval false The timer is not counting.
        bool isActive() const
        {
            return _inputLatch != 0;
        }

        //! @brief Reads the current output latch value.
        //! @return The calculated value of the timer.
        uint16_t readCount()
        {
            return _outputLatch;
        }

        //! @brief Writes the low 8 bits of the value loaded to the timer count when
        //! it passes 0.
        //! @param[in] lowLatch The low 8-bits of the new reset value.
        void writeLatchLow(uint8_t lowLatch)
        {
            _inputLatch &= 0xFF00;
            _inputLatch |= lowLatch;
        }

        //! @brief Writes the high 8 bits of the value loaded to the timer count when
        //! it passes 0.
        //! @param[in] lowLatch The high 8-bits of the new reset value.
        void writeLatchHigh(uint8_t highLatch)
        {
            _inputLatch &= 0x00FF;
            _inputLatch |= static_cast<uint16_t>(highLatch) << 8;
        }

        //! @brief Sets the context for the task executed when the counter reaches 0.
        //! @param[in] fn A pointer to the function to call.
        //! @param[in] context The context to pass to the function.
        void setTriggerCallback(GuestTask::TaskFn fn, uintptr_t context)
        {
            _triggerTask.defineTask(fn, context);
        }

        // Operations

        //! @brief Starts the timer running.
        //! @param[in] context An object providing access to the master system clock.
        void go(SystemContext *context)
        {
            start(context, 1);
        }

        //! @brief Updates the output latch with the current counter value.
        //! @param[in] context An object providing access to the master system clock.
        void latch(SystemContext *context)
        {
            // Calculate output latch value from start time, current time and freq.
            // The IOC timer counts from _inputLatch down to 0 (a period of
            // _inputLatch + 1 ticks), then reloads and generates an interrupt.
            uint64_t elapsed = context->getMasterClockTicks() - _startTime;
            uint64_t elapsedTicks = elapsed / _masterTicksPerCount;
            uint32_t period = static_cast<uint32_t>(_inputLatch) + 1;

            _outputLatch = _inputLatch - static_cast<uint16_t>(elapsedTicks % period);
        }

    protected:
        //! @brief Starts a timer counting, possibly re-starting it if already running.
        //! @param[in] context The context in which the counter will run.
        //! @param[in] countFactor How much the timer frequency is multiplied by to
        //! reduce the countdown rate.
        void start(SystemContext *context, uint64_t countFactor)
        {
            _startTime = context->getMasterClockTicks();
            _masterTicksPerCount = context->getMasterClockFrequency() / 2000000;

            // Schedule interrupt. The IOC timer counts _inputLatch+1 ticks
            // (from _inputLatch down to 0 inclusive) before triggering.
            uint64_t period = static_cast<uint64_t>(_inputLatch) + 1;
            context->scheduleTaskDeltaTicks(&_triggerTask, _masterTicksPerCount * period * countFactor);
        }

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

        //! @brief Activates the KART timer to start sending and receiving bytes.
        //! @param[in] context The context in which the counter will run.
        void go(SystemContext *context)
        {
            // IOC data sheet page 11:
            // The KART ... is of fixed format with 8 bits to a character which is
            // framed with one start bit and two stop bits.
            // ...
            // A clock of 16 times the data rate is used by the KART to clock in the
            // serial data from the KIN pin.
            // ...
            // The receive and transmit speeds are the same and programmed using
            // counter 3.
            constexpr uint32_t TicksPerBit = 16;
            constexpr uint32_t BitsPerFrame = 11;
            constexpr uint32_t TicksPerByte = TicksPerBit * BitsPerFrame;

            // Start the timer, but only trigger the callback once enough ticks have
            // taken place to send or receive a data byte.
            start(context, TicksPerByte);
        }
    };

    //! @brief Serves as a set of parameters to a call to onCounterReachesZero().
    struct CounterEventContext
    {
        IOCDevice *Parent;
        uint8_t TimerID;
        uint8_t Irq;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Internal Constants
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Used with if constexpr to determine of IDiagnosticSink branches
    //! should be taken - otherwise they will be optimised out.
    static constexpr bool AllowDiagnostics = TAllowDiagnostics;

    //! @brief The IRQ raised when transmission of a KART byte has completed.
    static constexpr uint8_t KartTxIrq = 14;

    //! @brief The IRQ raised when the KART has received a byte.
    static constexpr uint8_t KartRxIrq = 15;

    ///////////////////////////////////////////////////////////////////////////
    // Internal Fields
    ///////////////////////////////////////////////////////////////////////////
    IocSyncStatePtr _synchronisedData;
    BasicIrqManagerHardware &_parent;
    SystemContext *_context;
    IocIrqState *_irqState;
    AcornKeyboardController *_keyboard;
    Counter _counters[3];
    KartCounter _kartCounter;

    CounterEventContext _timer0Context;
    CounterEventContext _timer1Context;
    SynchronisedByteQueue *_kartRxQueue;
    SynchronisedByteQueue *_kartTxQueue;
    I2CBus *_i2cBus;
    IDiagnosticSink *_diagnosticSink;
    uint8_t _kartRxByte;

    ///////////////////////////////////////////////////////////////////////////
    // Internal Functions
    ///////////////////////////////////////////////////////////////////////////
    //! @brief A callback function which raises an interrupt with a specified
    //! hardware counter reaches 0.
    //! @param[in] guestContext The context shared across the emulated system.
    //! @param[in] taskContext A pointer to a TimerEventContext instance to operate on.
    static void onCounterReachesZero(SystemContext &guestContext, uintptr_t taskContext)
    {
        auto context = reinterpret_cast<CounterEventContext *>(taskContext);

        auto ioc = context->Parent;
        Counter &counter = ioc->_counters[context->TimerID];

        if (counter.isActive())
        {
            // Raise the interrupt.
            ioc->_parent.setGuestIrq(ioc->_irqState->raiseIrq(context->Irq));

            // Reset the counter.
            counter.go(&guestContext);
        }
    }

    //! @brief Handles the KART timer (counter 3) passing 0.
    //! @param[in] guestContext A reference to an object which can communicate
    //! with the host system.
    //! @param[in] taskContext A pointer to the IOC instance emulating the KART.
    static void onKartCounterReachesZero(SystemContext &guestContext,
                                         uintptr_t taskContext)
    {
        IOCDevice *ioc = reinterpret_cast<IOCDevice *>(taskContext);
        KartCounter &kartCounter = ioc->_kartCounter;

        if (kartCounter.isActive())
        {
            // Check for bytes received from the keyboard, or the host
            // implementation thereof. Only deliver if the previous Rx byte
            // has been read by the host (KartRxIrq is clear). This matches
            // real hardware behaviour where the KART Rx register holds only
            // one byte and won't accept a new one until it's been read.
            if (!(ioc->_irqState->getIrqState() & (1u << KartRxIrq)))
            {
                if (ioc->_kartRxQueue->try_dequeue(ioc->_kartRxByte))
                {
                    // A byte was received in IOC from the keyboard,
                    // raise an interrupt.
                    ioc->_parent.setGuestIrq(ioc->_irqState->raiseIrq(KartRxIrq));
                }
            }

            // Check for bytes we need to send to the keyboard, or the host
            // implementation thereof.
            uint8_t txByte;

            if (ioc->_kartTxQueue->try_dequeue(txByte) &&
                (ioc->_keyboard != nullptr))
            {
                // A byte is sent from IOC to the keyboard.
                // The STx register is now empty, raise an interrupt
                // to signify a byte was successfully transmitted.
                ioc->_parent.setGuestIrq(ioc->_irqState->raiseIrq(KartTxIrq));

                // Have the keyboard controller process the byte sent to it.
                ioc->_keyboard->receiveKARTByte(txByte);
            }

            // Reset the counter.
            kartCounter.go(&guestContext);
        }
    }
public:
    ///////////////////////////////////////////////////////////////////////////
    // Public Constants
    ///////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t BaseAddr = 0x3200000;

    ///////////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    ///////////////////////////////////////////////////////////////////////////
    IOCDevice() = delete;
    IOCDevice(const IOCDevice &) = delete;
    IOCDevice(IOCDevice &&) = delete;
    IOCDevice &operator=(const IOCDevice &) = delete;
    IOCDevice &operator=(IOCDevice &&) = delete;

    //! @brief Constructs a new object representing and instance of an I/O
    //! controller.
    //! @param[in] parent The object representing the system the controller
    //! belongs to.
    IOCDevice(BasicIrqManagerHardware &parent) :
        _synchronisedData(IocSyncStateTraits::create()),
        _parent(parent),
        _context(nullptr),
        _irqState(&_synchronisedData->InterruptState),
        _keyboard(nullptr),
        _kartRxQueue(&_synchronisedData->RxQueue),
        _kartTxQueue(&_synchronisedData->TxQueue),
        _i2cBus(nullptr),
        _diagnosticSink(nullptr),
        _kartRxByte(0)
    {
        // Enable HW counters 0 and 1 to raise interrupts.
        _timer0Context.Parent = this;
        _timer0Context.Irq = 5;
        _timer0Context.TimerID = 0;

        _counters[0].setTriggerCallback(IOCDevice::onCounterReachesZero,
                                        reinterpret_cast<uintptr_t>(&_timer0Context));

        _timer1Context.Parent = this;
        _timer1Context.Irq = 6;
        _timer1Context.TimerID = 1;
        _counters[1].setTriggerCallback(IOCDevice::onCounterReachesZero,
                                        reinterpret_cast<uintptr_t>(&_timer1Context));

        // Use HW counter 3 to service the KART interface.
        _kartCounter.setTriggerCallback(IOCDevice::onKartCounterReachesZero,
                                        reinterpret_cast<uintptr_t>(this));
    }

    virtual ~IOCDevice() = default;

    ///////////////////////////////////////////////////////////////////////////
    // Accessors
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Gets the state of the 5 control pins.
    uint8_t getCtrlPinInputState() const
    {
        return _irqState->getControlPinOutputState();
    }

    void setCtrlPinInputState(uint8_t pin, bool state)
    {
        _irqState->setControlPinInputState(pin, state);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Operations
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Sets the I2C bus connected to control pins C0 (SDA) and C1 (SCL).
    //! @param[in] bus A pointer to the I2C bus, or nullptr to disconnect.
    void setI2CBus(I2CBus *bus)
    {
        _i2cBus = bus;
    }

    //! @brief Raises the POR interrupt as if the system had just been switched on..
    void powerOnReset()
    {
        _parent.setGuestIrq(_irqState->raiseIrq(4));
    }

    //! @brief Activates one of the IL pins, i.e. drives it low.
    //! @param[in] ilNo The IL pin (0-7) to activate.
    //! @param[in] state The new state of the IL line, false for low (active), true
    //! for high (inactive).
    //! @retval true An unmasked interrupt became active as the result of
    //! this change.
    //! @retval false No additional interrupts were activated.
    void setInterruptLow(uint8_t ilNo, bool state)
    {
        if (ilNo == 0)
        {
            // IL[0] affects IRQ-8 and FIRQ-6.
            _parent.setGuestIrq(_irqState->setIrqState(8, !state));
            _parent.setGuestFastIrq(_irqState->setFirqState(6, !state));
        }
        else if (ilNo < 6)
        {
            // IL[1:5].
            _parent.setGuestIrq(_irqState->setIrqState(ilNo + 8, !state));
        }
        else if (ilNo < 8)
        {
            // IL[6:7].
            _parent.setGuestIrq(_irqState->setIrqState(ilNo - 6, !state));
        }

        if constexpr (AllowDiagnostics)
        {
            if (_diagnosticSink != nullptr && _context != nullptr)
            {
                InterruptEvent evt = {};
                evt.CycleCount = _context->getMasterClockTicks();
                evt.IrqStatus = _irqState->getIrqState();
                evt.IrqMask = _irqState->getIrqMask();
                evt.FirqStatus = _irqState->getFirqState();
                evt.FirqMask = _irqState->getFirqMask();
                _diagnosticSink->onInterruptChange(evt);
            }
        }
    }

    ////! @brief Activates one of the FH pins.
    ////! @param[in] fhNo The FH line to activate (0-1).
    ////! @param[in] state The new state of the FH line, true for high (active),
    ////! false for low (inactive).
    //void setHighFastInterrupt(uint8_t fhNo, bool state)
    //{
    //    if (fhNo < 2)
    //    {
    //        // The FH pins trigger FIRQ-0 and 1.
    //        _parent.setGuestFastIrq(_irqState->setFirqState(fhNo, state));
    //    }

    //    if (_diagnosticSink != nullptr && _context != nullptr)
    //    {
    //        InterruptEvent evt = {};
    //        evt.CycleCount = _context->getMasterClockTicks();
    //        evt.IrqStatus = _irqState->getIrqState();
    //        evt.IrqMask = _irqState->getIrqMask();
    //        evt.FirqStatus = _irqState->getFirqState();
    //        evt.FirqMask = _irqState->getFirqMask();
    //        _diagnosticSink->onInterruptChange(evt);
    //    }
    //}

    ////! @brief Activates the FL pin.
    ////! @param[in] state The new state of the pin, false for low (active), true
    ////! for high (inactive).
    //void setLowFastInterrupt(bool state)
    //{
    //    // The FL pin is an active low triggering FIRQ-2
    //    _parent.setGuestFastIrq(_irqState->setFirqState(2, !state));
    //}

    //! @brief Sets the input state of one of the control lines C0-C5.
    //! @param[in] ctrlLine The 0-based index of the pin state to update.
    //! @param[in] state The new state of the pin.
    void setControlPinInput(uint8_t ctrlLine, bool state)
    {
        if (ctrlLine < 6)
        {
            _parent.setGuestFastIrq(_irqState->setControlPinInputState(ctrlLine, state));
        }
    }

    ////! @brief Raises the VSync (IR) interrupt, IRQ A bit 3.
    ////! @retval true An unmasked IRQ is now pending.
    ////! @retval false No unmasked IRQs are pending.
    //bool raiseVSyncIrq()
    //{
    //    bool result = _irqState->raiseIrq(3);

    //    if constexpr (AllowDiagnostics)
    //    {
    //        if (_diagnosticSink != nullptr && _context != nullptr)
    //        {
    //            InterruptEvent evt = {};
    //            evt.CycleCount = _context->getMasterClockTicks();
    //            evt.IrqStatus = _irqState->getIrqState();
    //            evt.IrqMask = _irqState->getIrqMask();
    //            evt.FirqStatus = _irqState->getFirqState();
    //            evt.FirqMask = _irqState->getFirqMask();
    //            _diagnosticSink->onInterruptChange(evt);
    //        }
    //    }

    //    return result;
    //}

    ////! @brief Sends bytes to the IOC via the KART interface as if they were sent
    ////! from an attached keyboard.
    ////! @param[in] bytes The array of bytes to send.
    ////! @param[in] size The count of bytes to send, no more than 255.
    //void writeKart(const uint8_t *bytes, uint8_t size)
    //{
    //    auto &rxQueue = _synchronisedData->RxQueue;

    //    for (uint8_t i = 0; i < size; ++i)
    //    {
    //        rxQueue.enqueue(bytes[i]);
    //    }
    //}

    ////! @brief Send a byte to the IOC via the KART interface as if they were sent
    ////! from an attached keyboard.
    ////! @param[in] value The byte to send.
    //void writeKartByte(const uint8_t value)
    //{
    //    _synchronisedData->RxQueue.enqueue(value);
    //}

    //! @brief Attempts to dequeue a byte from the KART receive queue.
    //! @param[out] byte Receives the dequeued byte if successful.
    //! @returns true if a byte was dequeued, false if the queue was empty.
    //! @note This is intended for use in test harnesses only. In normal operation,
    //! the KART timer callback dequeues bytes from this queue.
    bool tryReadKartRxByte(uint8_t &byte)
    {
        return _synchronisedData->RxQueue.try_dequeue(byte);
    }

    ////! @brief Removes all bytes from the KART receive queue.
    //void flushKart()
    //{
    //    auto &rxQueue = _synchronisedData->RxQueue;

    //    // Dispose of any bytes in the queue to be received by the IOC.
    //    while (rxQueue.pop())
    //    {
    //        ;
    //    }
    //}

    ///////////////////////////////////////////////////////////////////////////
    // Overrides
    ///////////////////////////////////////////////////////////////////////////
    // Inherited from IAddressRegion.
    virtual RegionType getType() const override
    {
        return RegionType::MMIO;
    }

    // Inherited from IAddressRegion.
    virtual Ag::string_cref_t getName() const override
    {
        static const Ag::String name("IOC");

        return name;
    }

    // Inherited from IAddressRegion.
    virtual Ag::string_cref_t getDescription() const override
    {
        static const Ag::String description("The VL86C410 RISC I/O Controller");

        return description;
    }

    // Inherited from IAddressRegion.
    virtual uint32_t getSize() const override
    {
        return 0x80;
    }

    // Inherited from IMMIOBlock.
    virtual uint32_t read(uint32_t offset) override
    {
        uint32_t result = _context->getFuzz();
        uint8_t regId = Ag::Bin::extractBits<uint8_t, 0, 7>(offset) >> 2;

        if (regId < 16)
        {
            // It's an IRQ management register.
            switch (regId)
            {
            case 0:  // IOC Control Register
                result &= 0xFFFFFF00;
                result |= _irqState->readCtrlRegister();
                break;

            case 1:  // Serial Rx Data
                result = _kartRxByte;

                // Clear the interrupt condition.
                _parent.setGuestIrq(_irqState->setIrqState(KartRxIrq, false));
                break;

            case 4:  // IRQ Status A (read-only)
                result &= 0xFFFFFF00;
                // Bit 7 is always set
                result |= static_cast<uint8_t>(_irqState->getIrqState());
                break;

            case 5:  // IRQ Request A (read)/IRQ Clear (write)
                result &= 0xFFFFFF00;
                result |= static_cast<uint8_t>(_irqState->getIrqRequestState());
                break;

            case 6:  // IRQ Mask A
                result &= 0xFFFFFF00;
                result |= static_cast<uint8_t>(_irqState->getIrqMask());
                break;

            case 8:  // IRQ Status B
                result &= 0xFFFFFF00;
                result |= static_cast<uint8_t>(_irqState->getIrqState() >> 8);
                break;

            case 9:  // IRQ Request B
                result &= 0xFFFFFF00;
                result |= static_cast<uint8_t>(_irqState->getIrqRequestState() >> 8);
                break;

            case 10: // IRQ Mask B
                result &= 0xFFFFFF00;
                result |= static_cast<uint8_t>(_irqState->getIrqMask() >> 8);
                break;

            case 12: // FIRQ Status
                result &= 0xFFFFFF00;
                result |= _irqState->getFirqState();
                break;

            case 13: // FIRQ Request
                result &= 0xFFFFFF00;
                result |= _irqState->getFirqRequestState();
                break;

            case 14: // FIRQ Mask
                result &= 0xFFFFFF00;
                result |= _irqState->getFirqMask();
                break;

            case 2:  // Unused
            case 3:  // Unused
            case 7:  // Unused
            case 11: // Unused
            case 15: // Unused
            default:
                break;
            }
        }
        else if (regId < 32)
        {
            // It's a hardware timer register.
            uint8_t timerId = (regId - 16) >> 2;

            switch ((regId - 16) & 0x03)
            {
            case 0: // Count Low/Latch Low
                result &= 0xFFFFFF00;
                result |= _counters[timerId].readCount() & 0xFF;
                break;

            case 1: // Count High/Latch High
                result &= 0xFFFFFF00;
                result |= _counters[timerId].readCount() >> 8;
                break;

            case 2: // Go Command (write-only)
            case 3: // Latch Command (write-only)
            default:
                break;
            }
        }

        if constexpr (AllowDiagnostics)
        {
            if (_diagnosticSink != nullptr)
            {
                MemoryAccessEntry entry = {};
                entry.CycleCount = _context->getMasterClockTicks();
                entry.Address = BaseAddr + offset;
                entry.Value = result;
                entry.Size = 4;
                entry.IsWrite = false;
                _diagnosticSink->onMemoryAccess(entry);
            }
        }

        return result;
    }

    // Inherited from IMMIOBlock.
    virtual void write(uint32_t offset, uint32_t value) override
    {
        uint8_t regId = Ag::Bin::extractBits<uint8_t, 2, 5>(offset);

        if (regId < 16)
        {
            switch (regId)
            {
            case 0:  // IOC Control Register
                _irqState->writeCtrlRegister(static_cast<uint8_t>(value));

                // Notify the I2C bus of SDA/SCL state changes.
                // C0 = SDA, C1 = SCL. Open-drain: 0 = driven low, 1 = released (high).
                if (_i2cBus != nullptr)
                {
                    bool sda = (value & 0x01) != 0;
                    bool scl = (value & 0x02) != 0;
                    bool sdaInput;

                    _i2cBus->update(sda, scl, sdaInput);

                    // Drive SDA input back to IOC so the CPU can read the bus state.
                    _irqState->setControlPinInputState(0, sdaInput);
                }
                break;

            case 1:  // Serial Tx Data
                // Enqueue the byte which will be delivered to the keyboard
                // controller when timer 3 (the KART Baud timer) reaches 0.
                _kartTxQueue->enqueue(static_cast<uint8_t>(value));

                // Clear the pending KART Tx interrupt.
                _parent.setGuestIrq(_irqState->setIrqState(KartTxIrq, false));
                break;

            case 5:  // IRQ Request A (read)/IRQ Clear (write)
                _parent.setGuestIrq(_irqState->clearIrqs(static_cast<uint8_t>(value)));
                break;

            case 6:  // IRQ Mask A
                _parent.setGuestIrq(_irqState->setIrqMaskLow(static_cast<uint8_t>(value)));
                break;

            case 10: // IRQ Mask B
                _parent.setGuestIrq(_irqState->setIrqMaskHigh(static_cast<uint8_t>(value)));
                break;

            case 14: // FIRQ Mask
                _parent.setGuestFastIrq(_irqState->setFirqMask(static_cast<uint8_t>(value)));
                break;

            case 2:  // Unused
            case 3:  // Unused
            case 4:  // IRQ Status A (read-only)
            case 8:  // IRQ Status B (read-only)
            case 9:  // IRQ Request B (read-only)
            case 7:  // Unused
            case 11: // Unused
            case 12: // FIRQ Status (read-only)
            case 13: // FIRQ Request (read-only)
            case 15: // Unused
            default:
                break;
            }
        }
        else if (regId < 32)
        {
            // It's a hardware timer register.
            uint8_t timerId = (regId - 16) >> 2;

            // Timer 3 is the KART timer and has a different implementation.
            // The following avoids the need for virtual functions.
            if (timerId == 3)
            {
                switch (regId & 0x03)
                {
                case 0: // Latch Low
                    _kartCounter.writeLatchLow(static_cast<uint8_t>(value));
                    break;

                case 1: // Latch High
                    _kartCounter.writeLatchHigh(static_cast<uint8_t>(value));
                    break;

                case 2: // Go Command (write-only)
                    // We don't care what value, writing here simply
                    // activates the timer.

                    // Remove any bytes being received or transmitted after
                    // resetting the timer.
                    while (_kartRxQueue->try_dequeue(timerId)) {}
                    while (_kartTxQueue->try_dequeue(timerId)) {}

                    _kartCounter.go(_context);
                    break;

                case 3: // Latch Command (write-only)
                    // We don't care what value, writing here simply
                    // copies the current count to the output latch.
                    _kartCounter.latch(_context);
                    break;

                default:
                    break;
                }
            }
            else // Timers T0-T2
            {
                switch (regId & 0x03)
                {
                case 0: // Latch Low
                    _counters[timerId].writeLatchLow(static_cast<uint8_t>(value));
                    break;

                case 1: // Latch High
                    _counters[timerId].writeLatchHigh(static_cast<uint8_t>(value));
                    break;

                case 2: // Go Command (write-only)
                    // We don't care what value, writing here simply
                    // activates the timer.
                    _counters[timerId].go(_context);
                    break;

                case 3: // Latch Command (write-only)
                    // We don't care what value, writing here simply
                    // copies the current count to the output latch.
                    _counters[timerId].latch(_context);
                    break;

                default:
                    break;
                }
            }
        }

        if constexpr (AllowDiagnostics)
        {
            if (_diagnosticSink != nullptr)
            {
                MemoryAccessEntry entry = {};
                entry.CycleCount = _context->getMasterClockTicks();
                entry.Address = BaseAddr + offset;
                entry.Value = value;
                entry.Size = 4;
                entry.IsWrite = true;
                _diagnosticSink->onMemoryAccess(entry);
            }
        }
    }

    // Inherited from IMMIOBlock.
    virtual void connect(SystemContext &context) override
    {
        // Connect to the rest of the emulated system.
        _context = &context;

        if (context.tryFindTypedDevice("Keyboard Controller", _keyboard))
        {
            // Connect the keyboard device to the queue it should use to
            // transmits bytes to IOC via KART.
            _keyboard->connectToTxQueue(&_synchronisedData->RxQueue);
        }

        if constexpr (AllowDiagnostics)
        {
            context.tryFindTypedDevice("DiagnosticSink", _diagnosticSink);
        }
    }

    // Inherited from IInterruptController.
    virtual void setVSyncState(bool isInProgress)
    {
        // Raise an interrupt if VSync triggered an unmasked IRQ.
        _parent.setGuestIrq(_irqState->setVSyncState(isInProgress));
    }

    // Inherited from IInterruptController.
    virtual void raiseIrq(uint8_t id) override
    {
        // Raise an interrupt if an unmasked interrupt is in progress.
        _parent.setGuestIrq(_irqState->raiseIrq(id));
    }

    // Inherited from IInterruptController.
    virtual void raiseFirq(uint8_t id) override
    {
        // Raise an interrupt if an unmasked interrupt is in progress.
        _parent.setGuestFastIrq(_irqState->raiseFirq(id));
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

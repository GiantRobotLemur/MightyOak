//! @file ArmEmu/IOC.cpp
//! @brief The definition of an object which emulates the function of the
//! VL86C410 IOC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Utils.hpp"

#include "MemcHardware.hpp"
#include "AcornKeyboardController.hpp"

#include "ArmEmu/IOC.hpp"
#include "ArmEmu/HostMessageID.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

//! @brief The IRQ raised when transmission of a KART byte has completed.
constexpr uint8_t KartTxIrq = 14;

//! @brief The IRQ raised when the KART has received a byte.
constexpr uint8_t KartRxIrq = 15;

////////////////////////////////////////////////////////////////////////////////
// IocIrqState Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which holds state shared between threads.
IocIrqState::IocIrqState() :
    _irqStatus(0),
    _irqMask(0xFFFF),
    _firqStatus(0),
    _firqMask(0xFF),
    _ctrlInput(0xFF),
    _ctrlOutput(0x00),
    _ctrlState(0xFF)
{
}

//! @brief Gets the overall IRQ state to report to the CPU.
//! @retval true An IRQ is pending.
//! @retval false No IRQs are pending.
bool IocIrqState::getIrqPinState() const
{
    return ((_irqStatus.load() | 0x80) & ~_irqMask.load()) != 0;
}

//! @brief Gets the current state of all pending interrupts, ignoring masks.
//! @return A bitfield describing which interrupts are pending.
uint16_t IocIrqState::getUnmaskedIrqState() const
{
    return _irqStatus.load() | 0x80;
}

//! @brief Gets the masked state of interrupts, i.e. which unmasked interrupts
//! are pending.
//! @return A bitfield describing which unmasked interrupts are pending.
uint16_t IocIrqState::getMaskedIrqState() const
{
    return (_irqStatus.load() | 0x80) & ~_irqMask.load();
}

//! @brief Gets the current interrupt mask.
//! @return A bit field indicating which interrupts are currently masked.
uint16_t IocIrqState::getIrqMask() const
{
    return _irqMask.load();
}

//! @brief Sets the contents IRQ Mask register A.
//! @param[in] mask The low 8 bits of the interrupt mask.
//! @retval true If any unmasked interrupts are pending.
//! @retval false No unmasked interrupts are pending.
bool IocIrqState::setIrqMaskLow(uint8_t mask)
{
    _irqMask.store((_irqMask.load() & 0xFF00) | mask);

    return getIrqPinState();
}

//! @brief Sets the contents IRQ Mask register B.
//! @param[in] mask The high 8 bits of the interrupt mask.
//! @retval true If any unmasked interrupts are pending.
//! @retval false No unmasked interrupts are pending.
bool IocIrqState::setIrqMaskHigh(uint8_t mask)
{
    _irqMask.store((_irqMask.load() & 0xFF) | (static_cast<uint16_t>(mask) << 8));

    return getIrqPinState();
}

//! @brief Sets the pending state of an interrupt.
//! @param[in] irq The interrupt state to change.
//! @param[in] state True for pending, false for inactive.
//! @retval true An unmasked IRQ is now pending where previously no interrupts
//! were active.
bool IocIrqState::setIrqState(uint8_t irq, bool state)
{
    bool oldIrqState = getIrqPinState();

    Ag::Bin::updateBit(_irqStatus, irq, state);

    return (oldIrqState == false) && getIrqPinState();
}

//! @brief Gets the overall FIRQ state to report to the CPU.
//! @retval true An FIRQ is pending.
//! @retval false No FIRQs are pending.
bool IocIrqState::getFirqPinState() const
{
    return (_firqStatus.load() & _firqMask.load()) != 0;
}

//! @brief Gets the current state of all pending fast interrupts,
//! ignoring masks.
//! @return A bitfield describing which fast interrupts are pending.
uint8_t IocIrqState::getUnmaskedFirqState() const
{
    return _firqStatus.load();
}

//! @brief Gets the masked state of fast interrupts, i.e. which unmasked
//! fast interrupts are pending.
//! @return A bitfield describing which unmasked fast interrupts are pending.
uint8_t IocIrqState::getMaskedFirqState() const
{
    return _firqStatus.load() & ~_firqMask.load();
}

//! @brief Gets a bitfield defining which fast interrupts are masked.
uint8_t IocIrqState::getFirqMask() const
{
    return _firqMask.load();
}

//! @brief Sets the contents of the FIRQ Mask register.
//! @param[in] mask The bits of the fast interrupt mask.
//! @retval true If any unmasked fast interrupts are pending.
//! @retval false No unmasked fast interrupts are pending.
bool IocIrqState::setFirqMask(uint8_t mask)
{
    bool oldFirqState = getFirqPinState();

    _firqMask.store(mask);

    return (oldFirqState == false) && getFirqPinState();
}

//! @brief Sets the pending state of a fast interrupt.
//! @param[in] irq The fast interrupt state to change.
//! @param[in] state True for pending, false for inactive.
//! @retval true An unmasked FIRQ is now pending where previously no interrupts
//! were active.
bool IocIrqState::setFirqState(uint8_t irq, bool state)
{
    bool oldFirqState = getFirqPinState();

    Ag::Bin::updateBit(_firqStatus, irq, state);

    return (oldFirqState == false) && getFirqPinState();
}

//! @brief Reads the current value of the IOC control register.
uint8_t IocIrqState::readCtrlRegister() const
{
    // Bits 0-5 corresponds to input bits C[0:5] - disregarding whether the
    // corresponding bit is set in the control register to indicate input
    // is enabled.
    // Bit 6 is the state of the IF latched interrupt (IRQ register A, bit 2).
    // Bit 7 is the state of the IR latched interrupt (IRQ register A, bit 3).
    return static_cast<uint8_t>((_ctrlInput.load() & 0x3F) | ((_irqStatus.load() & 0x0C) << 4));
}

//! @brief Processes a write to the IOC control register by the CPU.
//! @param[in] value The byte written to the control register hardware address.
void IocIrqState::writeCtrlRegister(uint8_t value)
{
    _ctrlState.store((value & 0x3F) | 0xC0);
}

//! @brief Get the output state of IOC pins C[0:5], these will either be driven
//! low because the pin is set to an output mode or high, because the pin is
//! set to receive external input.
//! @return The output state of pins C[0:5].
uint8_t IocIrqState::getControlPinOutputState() const
{
    // IOC Data Sheet : Page 10
    // The control register allows the external control pins C[0:5] to be read
    // and written and the status if IR and IF[bar] to be inspected. The
    // [C0:C5] bits manipulated the C[0:5] IO Port.
    // When read, they reflect the current state of these pins.
    // When written LOW the output pin is driven LOW.
    // Those outputs are open-drain, and if programmed HIGH the pin is undriven
    // and may be treated as input.
    uint8_t outputMask = _ctrlState.load() & 0x3F;

    return _ctrlOutput.load() & outputMask;
}

//! @brief Sets the input state of one of the control lines C0-C5.
//! @param[in] pin The 0-based index of the pin state to update.
//! @param[in] state The new state of the pin.
bool IocIrqState::setControlPinInputState(uint8_t pin, bool state)
{
    if (pin < 6)
    {
        Ag::Bin::updateBit(_ctrlInput, pin, state);

        // Only bits C[3:5] are connected to FIRQs.
        // Update the _firqStatus based on the new values of the control pins.
        _firqStatus &= (_firqStatus & ~0x38) | _ctrlInput & 0x38;
    }

    return getFirqPinState();
}

//! @brief Clears the state of a set of latched interrupts.
//! @param[in] mask A bitfield defining the IRQs to clear.
//! @return Whether an unmasked IRQ is pending.
//! @note Only latched interrupts are cleared.
bool IocIrqState::clearIrqs(uint8_t mask)
{
    // Only allow clearing of latched interrupts.
    mask &= 0x7C;
    _irqStatus &= ~mask;

    // Return the new interrupt state.
    return getIrqPinState();
}

//! @brief Marks an IRQ as pending.
//! @param[in] id The 0-based index of the IRQ, 0-15.
//! @retval true An unmasked interrupt is pending.
//! @retval false No unmasked interrupts are pending.
bool IocIrqState::raiseIrq(uint8_t id)
{
    if (id < 16)
    {
        Ag::Bin::updateBit(_irqStatus, id, true);
    }

    // Raise an interrupt if an unmasked interrupt is in progress.
    return getIrqPinState();
}

////! @brief Raises a fast interrupt.
////! @param[in] id The 0-based index of the FIRQ to activate.
////! @retval true At least one unmasked fast interrupt is pending.
////! @retval false No unmasked fast interrupts are currently pending.
//bool IrqState::raiseFirq(uint8_t id)
//{
//    uint8_t state = 0;
//
//    if (id < 8)
//    {
//        state = _firqStatus |= static_cast<uint8_t>(1 << id);
//        state &= ~_firqMask.load();
//    }
//
//    // Raise an interrupt if an unmasked interrupt is in progress.
//    return state != 0;
//}

////////////////////////////////////////////////////////////////////////////////
// IOC Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a new object representing and instance of an I/O controller.
//! @param[in] parent The object representing the system the controller belongs to.
IOC::IOC(MemcHardware &parent) :
    _synchronisedData(IocSyncStateTraits::create()),
    _parent(parent),
    _irqState(&_synchronisedData->InterruptState),
    _keyboard(nullptr),
    _context(nullptr),
    _kartRxQueue(&_synchronisedData->RxQueue),
    _kartTxQueue(&_synchronisedData->TxQueue),
    _kartRxByte(0)
{
    // Enable HW counters 0 and 1 to raise interrupts.
    _timer0Context.Parent = this;
    _timer0Context.Irq = 5;
    _timer0Context.TimerID = 0;

    _counters[0].setTriggerCallback(IOC::onCounterReachesZero,
                                    reinterpret_cast<uintptr_t>(&_timer0Context));

    _timer1Context.Parent = this;
    _timer1Context.Irq = 6;
    _timer1Context.TimerID = 1;
    _counters[1].setTriggerCallback(IOC::onCounterReachesZero,
                                    reinterpret_cast<uintptr_t>(&_timer1Context));

    // Use HW counter 3 to service the KART interface.
    _kartCounter.setTriggerCallback(IOC::onKartCounterReachesZero,
                                    reinterpret_cast<uintptr_t>(this));
}

//! @brief Gets the state of the 5 control pins.
uint8_t IOC::getCtrlPinInputState() const
{
    return _irqState->getControlPinOutputState();
}

void IOC::setCtrlPinInputState(uint8_t pin, bool state)
{
    _irqState->setControlPinInputState(pin, state);
}

//! @brief Raises the POR interrupt as if the system had just been switched on..
void IOC::powerOnReset()
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
void IOC::setInterruptLow(uint8_t ilNo, bool state)
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
}

//! @brief Activates one of the FH pins.
//! @param[in] fhNo The FH line to activate (0-1).
//! @param[in] state The new state of the FH line, true for high (active),
//! false for low (inactive).
void IOC::setFastHighInterrupt(uint8_t fhNo, bool state)
{
    if (fhNo < 2)
    {
        // The FH pins tragger FIRQ-0 and 1.
        _parent.setGuestFastIrq(_irqState->setFirqState(fhNo, state));
    }
}

//! @brief Activates the FL pin.
//! @param[in] state The new state of the pin, false for low (active), true
//! for high (inactive).
void IOC::setFastLowInterrupt(bool state)
{
    // The FL pin is an active low triggering FIRQ-2
    _parent.setGuestFastIrq(_irqState->setFirqState(2, !state));
}

//! @brief Sets the input state of one of the control lines C0-C5.
//! @param[in] ctrlLine The 0-based index of the pin state to update.
//! @param[in] state The new state of the pin.
void IOC::setControlPinInput(uint8_t ctrlLine, bool state)
{
    if (ctrlLine < 6)
    {
        _parent.setGuestFastIrq(_irqState->setControlPinInputState(ctrlLine, state));
    }
}

//! @brief Sends bytes to the IOC via the KART interface as if they were sent
//! from an attached keyboard.
//! @param[in] bytes The array of bytes to send.
//! @param[in] size The count of bytes to send, no more than 255.
void IOC::writeKart(const uint8_t *bytes, uint8_t size)
{
    auto &rxQueue = _synchronisedData->RxQueue;

    for (uint8_t i = 0; i < size; ++i)
    {
        rxQueue.enqueue(bytes[i]);
    }
}

//! @brief Send a byte to the IOC via the KART interface as if they were sent
//! from an attached keyboard.
//! @param[in] value The byte to send.
void IOC::writeKartByte(const uint8_t value)
{
    _synchronisedData->RxQueue.enqueue(value);
}

//! @brief Removes all bytes from the KART receive queue.
void IOC::flushKart()
{
    auto &rxQueue = _synchronisedData->RxQueue;

    // Dispose of any bytes in the queue to be received by the IOC.
    while (rxQueue.pop())
    {
        ;
    }
}

//void IOC::raiseIrq(uint8_t id)
//{
//    // Raise an interrupt if an unmasked interrupt is in progress.
//    _parent.setGuestIrq(_irqState->raiseIrq(id));
//}
//
//void IOC::raiseFirq(uint8_t id)
//{
//    // Raise an interrupt if an unmasked interrupt is in progress.
//    _parent.setGuestFastIrq(_irqState->raiseFirq(id));
//}

// Inherited from IAddressRegion.
RegionType IOC::getType() const
{
    return RegionType::MMIO;
}

// Inherited from IAddressRegion.
Ag::string_cref_t IOC::getName() const
{
    static const Ag::String name("IOC");

    return name;
}

// Inherited from IAddressRegion.
Ag::string_cref_t IOC::getDescription() const
{
    static const Ag::String description("The VL86C410 RISC I/O Controller");

    return description;
}

// Inherited from IAddressRegion.
uint32_t IOC::getSize() const
{
    return 0x80;
}

// Inherited from IMMIOBlock.
uint32_t IOC::read(uint32_t offset)
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
            result |= static_cast<uint8_t>(_irqState->getUnmaskedIrqState());
            break;

        case 5:  // IRQ Request A (read)/IRQ Clear (write)
            result &= 0xFFFFFF00;
            result |= static_cast<uint8_t>(_irqState->getMaskedIrqState());
            break;

        case 6:  // IRQ Mask A
            result &= 0xFFFFFF00;
            result |= static_cast<uint8_t>(_irqState->getIrqMask());
            break;

        case 8:  // IRQ Status B
            result &= 0xFFFFFF00;
            result |= static_cast<uint8_t>(_irqState->getUnmaskedIrqState() >> 8);
            break;

        case 9:  // IRQ Request B
            result &= 0xFFFFFF00;
            result |= static_cast<uint8_t>(_irqState->getMaskedIrqState() >> 8);
            break;

        case 10: // IRQ Mask B
            result &= 0xFFFFFF00;
            result |= static_cast<uint8_t>(_irqState->getIrqMask() >> 8);
            break;

        case 12: // FIRQ Status
            result &= 0xFFFFFF00;
            result |= _irqState->getUnmaskedFirqState();
            break;

        case 13: // FIRQ Request
            result &= 0xFFFFFF00;
            result |= _irqState->getMaskedFirqState();
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

    return result;
}

// Inherited from IMMIOBlock.
void IOC::write(uint32_t offset, uint32_t value)
{
    uint8_t regId = Ag::Bin::extractBits<uint8_t, 2, 5>(offset);

    if (regId < 16)
    {
        switch (regId)
        {
        case 0:  // IOC Control Register
            _irqState->writeCtrlRegister(static_cast<uint8_t>(value));
            break;

        case 1:  // Serial Tx Data
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

        // HACK: When timerId is 3, the accesses will go past the end of the
        // _counters array and instead operate on the _kartCounter member,
        // which is identical in data layout, but has a different go()
        // member function.
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
            if (timerId == 3)
            {
                // NOTE: This is a filthy hack to avoid a virtual function call.
                _kartCounter.go(_context);
            }
            else
            {
                _counters[timerId].go(_context);
            }
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

// Inherited from IMMIOBlock.
void IOC::connect(const ConnectionContext &context)
{
    // Connect to the rest of the emulated system.
    _context = context.getInteropContext();
    IHardwareDevice *keyboardDevice = nullptr;

    if (context.tryFindDevice("Keyboard Controller", keyboardDevice))
    {
        _keyboard = dynamic_cast<AcornKeyboardController *>(keyboardDevice);
    }
}

//! @brief Constructs an object representing a hardware counter.
IOC::Counter::Counter() :
    _masterTicksPerCount(1),
    _startTime(0),
    _inputLatch(0),
    _outputLatch(0)
{
    Ag::zeroFill(_triggerTask);
    _triggerTask.Context = reinterpret_cast<uintptr_t>(this);
}

//! @brief Determines if the timer is currently running.
//! @retval true The timer is counting down.
//! @retval false The timer is not counting.
bool IOC::Counter::isActive() const
{
    return _inputLatch != 0;
}

//! @brief Reads the current output latch value.
//! @return The calculated value of the timer.
uint16_t IOC::Counter::readCount()
{
    return _outputLatch;
}

//! @brief Writes the low 8 bits of the value loaded to the timer count when
//! it passes 0.
//! @param[in] lowLatch The low 8-bits of the new reset value.
void IOC::Counter::writeLatchLow(uint8_t lowLatch)
{
    _inputLatch &= 0xFF00;
    _inputLatch |= lowLatch;
}

//! @brief Writes the high 8 bits of the value loaded to the timer count when
//! it passes 0.
//! @param[in] lowLatch The high 8-bits of the new reset value.
void IOC::Counter::writeLatchHigh(uint8_t highLatch)
{
    _inputLatch &= 0x00FF;
    _inputLatch |= static_cast<uint16_t>(highLatch) << 8;
}

//! @brief Sets the context for the task executed when the counter reaches 0.
//! @param[in] fn A pointer to the function to call.
//! @param[in] context The context to pass to the function.
void IOC::Counter::setTriggerCallback(GuestTask::TaskFn fn, uintptr_t context)
{
    _triggerTask.Task = fn;
    _triggerTask.Context = context;
}

//! @brief Starts the timer running.
//! @param[in] context An object providing access to the master system clock.
void IOC::Counter::go(SystemContext *context)
{
    start(context, 1);
}

//! @brief Updates the output latch with the current counter value.
//! @param context 
void IOC::Counter::latch(SystemContext *context)
{
    // Calculate output latch vale from start time, current time and freq.
    uint64_t elapsed = context->getMasterClockTicks() - _startTime;
    uint64_t elapsedTicks = elapsed / _masterTicksPerCount;

    _outputLatch = _inputLatch - static_cast<uint16_t>(elapsedTicks % _inputLatch);
}

void IOC::Counter::start(SystemContext *context, uint64_t countFactor)
{
    _startTime = context->getMasterClockTicks();
    _masterTicksPerCount = context->getMasterClockFrequency() / 2000000;

    // Schedule interrupt.
    if (_triggerTask.Task != nullptr)
    {
        _triggerTask.At = (_masterTicksPerCount * _inputLatch * countFactor) + _startTime;
        context->scheduleTask(&_triggerTask);
    }
}

void IOC::KartCounter::go(SystemContext *context)
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

//! @brief A callback function which raises an interrupt with a specified
//! hardware counter reaches 0.
//! @param[in] guestContext The context shared across the emulated system.
//! @param[in] taskContext A pointer to a TimerEventContext instance to operate on.
void IOC::onCounterReachesZero(SystemContext &guestContext, uintptr_t taskContext)
{
    auto context = reinterpret_cast<CounterEventContext *>(taskContext);

    IOC *ioc = context->Parent;
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
void IOC::onKartCounterReachesZero(SystemContext &guestContext,
                                   uintptr_t taskContext)
{
    IOC *ioc = reinterpret_cast<IOC *>(taskContext);
    KartCounter &kartCounter = ioc->_kartCounter;

    if (kartCounter.isActive())
    {
        // Check for bytes received from the keyboard, or the host
        // implementation thereof.
        if (ioc->_kartRxQueue->try_dequeue(ioc->_kartRxByte))
        {
            // A byte was received, raise an interrupt.
            ioc->_irqState->raiseIrq(KartRxIrq);
        }

        // Check for bytes we need to send to the keyboard, or the host
        // implementation thereof.
        uint8_t txByte;

        if (ioc->_kartTxQueue->try_dequeue(txByte) &&
            (ioc->_keyboard != nullptr))
        {
            // Transmit the byte to the host system.
            ioc->_keyboard->receiveKARTByte(txByte);
        }

        // Reset the counter.
        kartCounter.go(&guestContext);
    }
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


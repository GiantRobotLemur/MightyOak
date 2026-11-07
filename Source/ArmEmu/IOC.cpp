//! @file ArmEmu/IOC.cpp
//! @brief The definition of an object which emulates the function of the
//! VL86C410 IOC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
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

#include "IOC.hpp"
#include "ArmEmu/HostMessageID.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// IocIrqState Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which holds state shared between threads.
IocIrqState::IocIrqState() :
    _irqStatus(0),
    _irqMask(0x00),
    _firqStatus(0),
    _firqMask(0x00),
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
    return ((_irqStatus.load() | 0x80) & _irqMask.load()) != 0;
}

//! @brief Gets the current state of all pending interrupts, ignoring masks.
//! @return A bitfield describing which interrupts are pending.
uint16_t IocIrqState::getIrqState() const
{
    return _irqStatus.load() | 0x80;
}

//! @brief Gets the masked state of interrupts, i.e. which unmasked interrupts
//! are pending.
//! @return A bitfield describing which unmasked interrupts are pending.
uint16_t IocIrqState::getIrqRequestState() const
{
    return (_irqStatus.load() | 0x80) & _irqMask.load();
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
//! @note A 1 in a corresponding bit of the mask enables an interrupt to be
//! reported to the processor, a 0 disables reporting.
bool IocIrqState::setIrqMaskLow(uint8_t mask)
{
    _irqMask.store((_irqMask.load() & 0xFF00) | mask);

    return getIrqPinState();
}

//! @brief Sets the contents IRQ Mask register B.
//! @param[in] mask The high 8 bits of the interrupt mask.
//! @retval true If any unmasked interrupts are pending.
//! @retval false No unmasked interrupts are pending.
//! @note A 1 in a corresponding bit of the mask enables an interrupt to be
//! reported to the processor, a 0 disables reporting.
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
uint8_t IocIrqState::getFirqState() const
{
    return _firqStatus.load() | 0x80;
}

//! @brief Gets the masked state of fast interrupts, i.e. which unmasked
//! fast interrupts are pending.
//! @return A bitfield describing which unmasked fast interrupts are pending.
uint8_t IocIrqState::getFirqRequestState() const
{
    return (_firqStatus.load() | 0x80) & _firqMask.load();
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
//! @note A 1 in a corresponding bit of the mask enables an fast interrupt
//! to be reported to the processor, a 0 disables reporting.
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
    // Bits 0-1 corresponds with the state of the SDA and SCL I2C lines.
    // Bit 2 - The FL line state (active low) (floppy disk ready / density)
    // Bits 3-5 corresponds to input bits C[3:5] - disregarding whether the
    // corresponding bit is set in the control register to indicate input
    // is enabled.
    // Bit 6 is the state of the IF latched interrupt (IRQ register A, bit 2).
    // Bit 7 is the state of the IR latched interrupt (IRQ register A, bit 3),
    //   this is the VIDC VSync state.
    uint8_t ctrlReg = static_cast<uint8_t>(_ctrlInput.load() & 0x3F);
    ctrlReg |= _vSyncInProgress ? 0x80 : 0x00;

    // TODO: Implement state of IF pin in bit 6.
    //   Archimedes: Parallel port ACK
    //   A-Series:   FD index signal
    return ctrlReg;
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
        _firqStatus &= (_firqStatus & ~0x38) | (_ctrlInput & 0x38);
    }

    return getFirqPinState();
}

//! @brief Sets whether the display output has finished reading output for
//! a frame and is in the VSync period before starting to display the next
//! frame.
//! @param[in] isInProgress True if VSync is in progress, possibly
//! triggering a level-triggered IRQ, false is VSync has finished and the
//! new frame is being read from the frame buffer.
//! @retval true An unmasked IRQ is now pending where previously no interrupts
//! were active.
bool IocIrqState::setVSyncState(bool isInProgress)
{
    bool oldValue = _vSyncInProgress.exchange(isInProgress);

    if (isInProgress && !oldValue)
    {
        // The VSync line has transitioned from low to high, that
        // triggers IRQ 3.
        setIrqState(3, true);
    }

    return getIrqPinState();
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

//! @brief Raises a fast interrupt.
//! @param[in] id The 0-based index of the FIRQ to activate.
//! @retval true At least one unmasked fast interrupt is pending.
//! @retval false No unmasked fast interrupts are currently pending.
bool IocIrqState::raiseFirq(uint8_t id)
{
    uint8_t state = 0;

    if (id < 8)
    {
        state = _firqStatus |= static_cast<uint8_t>(1 << id);
        state &= ~_firqMask.load();
    }

    // Raise an interrupt if an unmasked interrupt is in progress.
    return state != 0;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


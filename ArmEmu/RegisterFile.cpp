//! @file ArmEmu/RegisterFile.cpp
//! @brief The definition of a layer of emulation which manages the internal
//! processor registers.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Ag/Core/Utils.hpp"

#include "RegisterFile.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
//! @brief A mask of bits in the PSR which can be changed from 26-bit privileged
//! code.
static const uint32_t PsrMaskPrivilaged26 = 0xFC000003;

//! @brief A mask of bits in the PSR which can be changed from 26-bit
//! non-privileged code.
static const uint32_t PsrMaskUser26 = 0xF0000000;

//! @brief A mask of bits which define the processor mode in the 26-bit PSR.
static const uint32_t PsrModeMask26 = 0x00000003;

//! @brief The IRQ bit in the 26-bit PSR.
static const uint32_t IrqDisableBit26 = 0x08000000;

//! @brief The IRQ bit in the 26-bit PSR.
static const uint32_t FirqDisableBit26 = 0x04000000;

//! @brief The shift needed to position the IRQ and FIRQ disable bits in the
//! PSR.
static const uint8_t PsrIrqDisableShift26 = 26;

//! @brief A mask of the IRQ and FIRQ disable bits in the 26-bit PSR.
static const uint32_t IrqDisableMask26 = IrqDisableBit26 | FirqDisableBit26;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// RegisterFile Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Changes the mode of the processor, updating register banks and PSR.
//! @param[in] newMode The mode to change the processor to.
//! @retval true The processor mode was changed.
//! @retval false The processor was already in the requested mode.
constexpr bool RegisterFile::changeMode(ProcessorMode newMode) noexcept
{
    bool isChanged = false;

    ProcessorMode oldMode = getMode();

    if (oldMode != newMode)
    {
        isChanged = true;

        // Store current register values in correct bank.
        switch (oldMode)
        {
        case ProcessorMode::User26:
            // Copy R8-R14 to the User bank.
            std::copy_n(_coreRegisters + 8, 7, _userModeRegBank);
            break;

        case ProcessorMode::Irq26:
            // Copy R8-R12 to the User bank and R13-R14 to IRQ bank.
            std::copy_n(_coreRegisters + 8, 5, _userModeRegBank);
            std::copy_n(_coreRegisters + 13, 2, _irqModeRegBank);
            break;

        case ProcessorMode::FastIrq26:
            // Copy R8-R14 to the FIRQ bank.
            std::copy_n(_coreRegisters + 8, 7, _firqModeRegBank);
            break;

        case ProcessorMode::Svc26:
            // Copy R8-R12 to the User bank and R13-R14 to SVC bank.
            std::copy_n(_coreRegisters + 8, 5, _userModeRegBank);
            std::copy_n(_coreRegisters + 13, 2, _svcModeRegBank);
            break;

        default:
            break;
        }

        // Unpack the banked registers for the new mode.
        switch (newMode)
        {
        case ProcessorMode::User26:
            // Copy R8-R14 to from User bank.
            std::copy_n(_userModeRegBank, 7, _coreRegisters + 8);
            break;

        case ProcessorMode::Irq32:
            // Copy R8-R12 from the User bank and R13-R14 from IRQ bank.
            std::copy_n(_userModeRegBank, 5, _coreRegisters + 8);
            std::copy_n(_irqModeRegBank, 2, _coreRegisters + 13);
            break;

        case ProcessorMode::FastIrq26:
            // Copy R8-R14 from the FIRQ bank.
            std::copy_n(_firqModeRegBank, 7, _coreRegisters + 8);
            break;

        case ProcessorMode::Svc26:
            // Copy R8-R12 to the User bank and R13-R14 to SVC bank.
            std::copy_n(_userModeRegBank, 5, _coreRegisters + 8);
            std::copy_n(_svcModeRegBank, 2, _coreRegisters + 13);
            break;

        default:
            break;
        }

        // Update the hardware layer about the new privilege level
        // we are operating at.
        setPrivilegedMode(newMode != ProcessorMode::User26);

        uint32_t &cpsr = _coreRegisters[toScalar(CoreRegister::CPSR)];

        // Update the mode in the CPSR register.
        cpsr = (cpsr & ~PsrModeMask26) | toScalar(newMode);
    }

    return isChanged;
}

//! @brief Updates the processor state in response to an exception.
//! @param[in] newPc The address of the hardware vector the processor
//! should branch to in order to handle the exception.
void RegisterFile::raiseException(uint32_t newPc) noexcept
{
    // Store the current PC + PSR in R14_Svc.
    _svcModeRegBank[1] = _coreRegisters[15] | _coreRegisters[toScalar(CoreRegister::CPSR)];

    // Disable normal interrupts.
    _coreRegisters[toScalar(CoreRegister::CPSR)] |= IrqDisableBit26;

    // Switch to 26-bit supervisor mode.
    changeMode(ProcessorMode::Svc26);

    // Branch through the appropriate hardware vector.
    _coreRegisters[15] = newPc;
}

//! @brief Gets the current value of the carry flag from the PSR.
//! @retval 1 The carry flag was set.
//! @retval 0 The carry flag was clear.
uint8_t RegisterFile::getCarry() const
{
    return static_cast<uint8_t>(getPSR() >> PsrCarryShift) & 1;
}

//! @brief Gets the current value of the overflow flag from the PSR.
//! @retval 1 The overflow flag was set.
//! @retval 0 The overflow flag was clear.
uint8_t RegisterFile::getOverflow() const
{
    return static_cast<uint8_t>(getPSR() >> PsrOverflowShift) & 1;
}

//! @brief Gets the current state of the status flags from the PSR.
//! @return The 4 status flags shifted to the least significant bits
//! of the returned value.
uint8_t RegisterFile::getStatusFlags() const
{
    return static_cast<uint8_t>(getPSR() >> PsrStatusShift);
}

//! @brief Constructs an object which manages the registers of an emulated ARM
//! processor core.
RegisterFile::RegisterFile()
{
    // Zero-fill the registers.
    std::fill_n(_coreRegisters, arraySize(_coreRegisters), 0);
    std::fill_n(_userModeRegBank, arraySize(_userModeRegBank), 0);
    std::fill_n(_firqModeRegBank, arraySize(_firqModeRegBank), 0);
    std::fill_n(_irqModeRegBank, arraySize(_irqModeRegBank), 0);
    std::fill_n(_svcModeRegBank, arraySize(_svcModeRegBank), 0);
}

//! @brief Gets the current Program Status Register value.
uint32_t RegisterFile::getPSR() const
{
    return _coreRegisters[toScalar(CoreRegister::CPSR)];
}

//! @brief Updates the Program Status Register value, possibly altering
//! the processor mode and the mask state of interrupts.
//! @retval true The processor mode was changed.
//! @retval false The processor mode remains unmodified.
bool RegisterFile::setPSR(uint32_t psr)
{
    // Possibly change the processor mode.
    bool isModeChanged = changeMode(fromScalar<ProcessorMode>(
        static_cast<uint8_t>(psr & PsrModeMask26)));

    // Update the PSR.
    _coreRegisters[toScalar(CoreRegister::CPSR)] = psr & PsrMaskPrivilaged26;

    // Mask IRQs as required.
    setIrqMask(static_cast<uint8_t>(psr >> PsrIrqDisableShift26) & IS_IrqMask);

    return isModeChanged;
}

//! @brief Updates the status flag values in the PSR.
//! @param[in] flags The new status flags in bits 28-31.
void RegisterFile::setStatusFlags(uint32_t flags)
{
    uint32_t &psr = _coreRegisters[toScalar(CoreRegister::CPSR)];

    // Clear the previous flags.
    psr &= ~0xF0000000;

    // Merge in the new flags.
    psr |= flags & 0xF0000000;
}

//! @brief Updates the bits of the PSR which can be changed given
//! the current processor mode.
//! @param psrBits The new PSR bit values.
//! @retval true The processor mode was changed.
//! @retval false The processor mode was left unmodified.
bool RegisterFile::updatePSR(uint32_t psrBits)
{
    // Determine what bits the instruction is allowed to update.
    uint32_t psrMask = isPrivilegedMode() ? PsrMaskPrivilaged26 :
                                            PsrMaskUser26;

    // Update the PSR bits, preserving those which an unprivileged mode
    // should not be able to change.
    uint32_t oldPsr = _coreRegisters[toScalar(CoreRegister::CPSR)];
    uint32_t newPsr = (oldPsr & ~psrMask) | (psrBits & psrMask);

    return setPSR(newPsr);
}

//! @brief Gets the current value of the Program Counter, 8-bytes advanced
//! from the address of the currently executing instruction.
uint32_t RegisterFile::getPC() const
{
    return _coreRegisters[toScalar(CoreRegister::R15)];
}

//! @brief Sets the value of the Program Counter, altering the program flow.
//! @param[in] pc The logical address of the next instruction to execute.
void RegisterFile::setPC(uint32_t pc)
{
    _coreRegisters[toScalar(CoreRegister::R15)] = pc;
}

//! @brief Interprets the current processor mode from the PSR.
constexpr ProcessorMode RegisterFile::getMode() const noexcept
{
    return fromScalar<ProcessorMode>(static_cast<uint8_t>(_coreRegisters[toScalar(CoreRegister::CPSR)] & PsrModeMask26));
}

//! @brief Reads a register value, if R15, the PC is read.
//! @param[in] regId The identifier of the register to read.
uint32_t RegisterFile::getRn(GeneralRegister regId) const
{
    return _coreRegisters[toScalar(regId)];
}

//! @brief Writes a register value, if R15, the PC is updated.
//! @param[in] regId The identifier of the register to write.
//! @param[in] value The new register value.
//! @retval true R15 was updated, a pipeline stall should occur.
//! @retval false The PC was not updated.
bool RegisterFile::setRn(GeneralRegister regId, uint32_t value)
{
    bool pcUpdated = false;

    if (regId == GeneralRegister::R15)
    {
        // Update the PC.
        _coreRegisters[15] = value & ~PsrMaskPrivilaged26;
        pcUpdated = true;
    }
    else
    {
        _coreRegisters[toScalar(regId)] = value;
    }

    return pcUpdated;
}

//! @brief Gets the value of a register from the user bank as used by the
//! STM instruction.
//! @param[in] regId The identifier of the register to read.
//! @return The register contents. For R15 that is the PC + 4 and PSR bits.
uint32_t RegisterFile::getUserRn(GeneralRegister regId) const noexcept
{
    uint32_t value;

    // Registers R0-R7 are never banked.
    if (regId < GeneralRegister::R8)
    {
        value = _coreRegisters[toScalar(regId)];
    }
    else if (regId == GeneralRegister::R15)
    {
        // STM instructions store the PC + 4 and PSR bits.
        value = (_coreRegisters[15] + 4) | _coreRegisters[toScalar(CoreRegister::CPSR)];
    }
    else
    {
        ProcessorMode mode = getMode();

        // Determine which user registers are currently hidden.
        switch (mode)
        {
        case ProcessorMode::User26:
        default:
            // User register bank selected.
            value = _coreRegisters[toScalar(regId)];
            break;

        case ProcessorMode::FastIrq26:
            // Registers R8-R14 are banked.
            value = _firqModeRegBank[toScalar(regId) - 8];
            break;

        case ProcessorMode::Irq26:
            // Registers R13-R14 are banked.
            if (regId < GeneralRegister::R13)
            {
                value = _coreRegisters[toScalar(regId)];
            }
            else
            {
                value = _irqModeRegBank[toScalar(regId) - 13];
            }
            break;

        case ProcessorMode::Svc26:
            // Registers R13-R14 are banked.
            if (regId < GeneralRegister::R13)
            {
                value = _coreRegisters[toScalar(regId)];
            }
            else
            {
                value = _svcModeRegBank[toScalar(regId) - 13];
            }
            break;
        }
    }

    return value;
}

//! @brief Sets the value of a register in the user bank.
//! @param[in] regId The identifier of the register to read.
//! @param[in] value The new register contents.
//! @note Should never be used to write R15, but this is not checked.
void RegisterFile::setUserRn(GeneralRegister regId, uint32_t value) noexcept
{
    // Registers R0-R7 are never banked.
    if (regId < GeneralRegister::R8)
    {
        _coreRegisters[toScalar(regId)] = value;
    }
    else
    {
        ProcessorMode mode = getMode();

        // Determine which user registers are currently hidden.
        switch (mode)
        {
        case ProcessorMode::User26:
        default:
            // User register bank selected.
            _coreRegisters[toScalar(regId)] = value;
            break;

        case ProcessorMode::FastIrq26:
            // Registers R8-R14 are banked.
            _firqModeRegBank[toScalar(regId) - 8] = value;
            break;

        case ProcessorMode::Irq26:
            // Registers R13-R14 are banked.
            if (regId < GeneralRegister::R13)
            {
                _coreRegisters[toScalar(regId)] = value;
            }
            else
            {
                _irqModeRegBank[toScalar(regId) - 13] = value;
            }
            break;

        case ProcessorMode::Svc26:
            // Registers R13-R14 are banked.
            if (regId < GeneralRegister::R13)
            {
                _coreRegisters[toScalar(regId)] = value;
            }
            else
            {
                _svcModeRegBank[toScalar(regId) - 13] = value;
            }
            break;
        }
    }
}

//! @brief Reads a register value, if R15, the combined PC/PSR is read.
//! @param[in] regId The identifier of the register to read.
uint32_t RegisterFile::getRm(GeneralRegister regId) const
{
    return (regId == GeneralRegister::R15) ?
        (_coreRegisters[15] | _coreRegisters[toScalar(CoreRegister::CPSR)]) :
        _coreRegisters[toScalar(regId)];
}

//! @brief Reads a register value, if R15, the PC + 4 is read, i.e. the
//! address of the current instruction + 12.
//! @param[in] regId The identifier of the register to read.
uint32_t RegisterFile::getRs(GeneralRegister regId) const
{
    return (regId == GeneralRegister::R15) ?
        (_coreRegisters[15] + 4) :
        _coreRegisters[toScalar(regId)];
}

//! @brief Reads a register value, if R15, the PSR is read.
//! @param[in] regId The identifier of the register to read.
uint32_t RegisterFile::getRd(GeneralRegister regId) const
{
    return (regId == GeneralRegister::R15) ?
        (_coreRegisters[toScalar(CoreRegister::CPSR)]) :
        _coreRegisters[toScalar(regId)];
}

//! @brief Writes a register value, if R15, the PSR is updated, possibly the
//! PSR flags too if updateStatus is true..
//! @param[in] regId The identifier of the register to write.
//! @param[in] value The new register value.
//! @param[in] updateStatus True if status flags should be updated if the
//! target register if R15. Either way, the Program Counter will be
//! overwritten.
//! @retval true The program counter was updated, a pipeline stall should occur.
//! @retval false The PC was not updated.
bool RegisterFile::setRd(GeneralRegister regId, uint32_t value, bool updateStatus)
{
    bool pcUpdated = false;

    if (regId == GeneralRegister::R15)
    {
        if (updateStatus)
        {
            updatePSR(value);
        }

        // Update the PC.
        _coreRegisters[15] = (value & ~PsrMaskPrivilaged26);

        pcUpdated = true;
    }
    else
    {
        _coreRegisters[toScalar(regId)] = value;
    }

    return pcUpdated;
}

//! @brief Reads a register value, if R15, the PC + 4 is read, i.e. the
//! address of the current instruction + 12, and combined with the PSR flags.
//! @param[in] regId The identifier of the register to read.
uint32_t RegisterFile::getRx(GeneralRegister regId) const
{
    return (regId == GeneralRegister::R15) ?
        ((_coreRegisters[15] + 4) | _coreRegisters[toScalar(CoreRegister::CPSR)]) :
        _coreRegisters[toScalar(regId)];
}


//! @brief Updates the processor state in response to the reset signal
//! being received.
void RegisterFile::raiseReset()
{
    // Store the current PC + PSR in R14_Svc.
    _svcModeRegBank[1] = _coreRegisters[15] | _coreRegisters[toScalar(CoreRegister::CPSR)];

    // Switch to 26-bit supervisor mode.
    changeMode(ProcessorMode::Svc26);

    // (Manually?) Disable all interrupts.
    _coreRegisters[toScalar(CoreRegister::CPSR)] |= IrqDisableMask26;
    setIrqMask(IS_IrqMask);

    // Branch through the reset hardware vector.
    _coreRegisters[15] = 0x00000000;
}

//! @brief Updates the processor state in response to an attempt to execute
//! an undefined instruction.
void RegisterFile::raiseUndefinedInstruction()
{
    // Raise an exception and branch through the Undefined Instruction
    // hardware vector.
    raiseException(0x00000004);
}

//! @brief Updates the processor state in response to execution of an SWI
//! instruction.
void RegisterFile::raiseSoftwareInterrupt()
{
    // Raise an exception and branch through the Software Interrupt
    // hardware vector.
    raiseException(0x00000008);
}

//! @brief Updates the processor state in response to fetching an instruction
//! without sufficient privileges.
void RegisterFile::raisePreFetchAbort()
{
    // Raise an exception and branch through the Pre-fetch Abort
    // hardware vector.
    raiseException(0x0000000C);
}

//! @brief Updates the processor state in response to accessing data without
//! sufficient privileges.
void RegisterFile::raiseDataAbort()
{
    // Raise an exception and branch through the Data Abort
    // hardware vector.
    raiseException(0x00000010);
}

//! @brief Updates the processor state in response to accessing data at an
//! address beyond the valid range.
void RegisterFile::raiseAddressException()
{
    // Raise an exception and branch through the Data Abort
    // hardware vector.
    raiseException(0x00000014);
}

//! @brief Updates the processor state in response to a pending fast interrupt.
void RegisterFile::handleIrq()
{
    // Store the current PC + PSR in R14_Irq.
    _irqModeRegBank[1] = _coreRegisters[15] | _coreRegisters[toScalar(CoreRegister::CPSR)];

    // Disable normal interrupts.
    _coreRegisters[toScalar(CoreRegister::CPSR)] |= IrqDisableBit26;

    // Switch to 26-bit IRQ mode.
    changeMode(ProcessorMode::Irq26);

    // Branch through the IRQ hardware vector.
    _coreRegisters[15] = 0x00000018;
}

//! @brief Updates the processor state in response to a pending fast interrupt.
void RegisterFile::handleFirq()
{
    // Store the current PC + PSR in R14_Firq.
    _firqModeRegBank[6] = _coreRegisters[15] | _coreRegisters[toScalar(CoreRegister::CPSR)];

    // Disable ALL interrupts.
    _coreRegisters[toScalar(CoreRegister::CPSR)] |= IrqDisableMask26;

    // Switch to 26-bit FIRQ mode.
    changeMode(ProcessorMode::FastIrq26);

    // Branch through the FIRQ hardware vector.
    _coreRegisters[15] = 0x0000001C;
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


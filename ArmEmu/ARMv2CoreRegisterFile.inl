//! @file ArmEmu/ARMv2CoreRegisterFile.inl
//! @brief The declaration of an implementation of the register file of an
//! ARMv2 processor.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_V2_REGISTER_FILE_INL__
#define __ARM_EMU_V2_REGISTER_FILE_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"

#include "Hardware.inl"
#include "RegisterFile.inl"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief an implementation of the register file of an ARMv2 processor.
//! @tparam An object representing the underlying hardware which supports the
//! setPrivilegedMode(bool) and setIrqMask(uint8_t) member functions.
template<typename THardware>
class ARMv2CoreRegisterFile // Implements GenericCoreRegisterFile
{
private:
    // Internal Fields
    THardware &_hardware;

    uint32_t _coreRegisters[16];
    uint32_t _cpsr;                 // PSR portion of R15.
    uint32_t _userModeRegBank[7];   // R8-R14
    uint32_t _firqModeRegBank[7];   // R8-R14
    uint32_t _irqModeRegBank[2];    // R13-R14
    uint32_t _svcModeRegBank[2];    // R13-R14

    // Internal Functions
    bool changeMode(ProcessorMode newMode) noexcept
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

            // Notify the hardware layer if we are operating in a privileged
            // mode for the purposes of memory access.
            _hardware.setPrivilegedMode(newMode != ProcessorMode::User26);

            // Update the mode in the CPSR register.
            _cpsr = (_cpsr & ~PsrMask26::ModeBits) | Ag::toScalar(newMode);
        }

        return isChanged;
    }

    uint32_t raiseException(uint32_t newPc) noexcept
    {
        // Store the current PC + PSR in R14_<mode>.
        uint32_t oldR15 = _coreRegisters[15] | _cpsr;

        // Disable normal interrupts.
        _cpsr |= PsrMask26::IrqDisableBit;

        // Update the IRQ mask at the hardware level.
        _hardware.updateIrqMask(static_cast<uint8_t>(_cpsr >> PsrShift26::IrqDisable),
                                IrqState::GuestIrqsMask);

        // Switch to the appropriate processor mode.
        uint32_t result = changeMode(ProcessorMode::Svc26) ? ExecResult::ModeChange : 0;

        // Set the link register in the (possibly new) processor mode.
        _coreRegisters[14] = oldR15;

        // Branch through the appropriate hardware vector.
        _coreRegisters[15] = newPc;

        return result | ExecResult::FlushPipeline;
    }

public:
    // Public Constants
    static constexpr bool HasCombinedPcPsr = true;

    // Construction/Destruction
    ARMv2CoreRegisterFile(THardware &hw) :
        _hardware(hw),
        _cpsr(Ag::toScalar(ProcessorMode::Svc26) | PsrMask26::IrqDisableBits)
    {
        // Zero-fill the registers.
        std::fill_n(_coreRegisters, std::size(_coreRegisters), 0);
        std::fill_n(_userModeRegBank, std::size(_userModeRegBank), 0);
        std::fill_n(_firqModeRegBank, std::size(_firqModeRegBank), 0);
        std::fill_n(_irqModeRegBank, std::size(_irqModeRegBank), 0);
        std::fill_n(_svcModeRegBank, std::size(_svcModeRegBank), 0);

        // Disable all IRQs and note we are in a privileged mode for the
        // purposes of hardware access.
        _hardware.updateIrqMask(IrqState::GuestIrqsMask, IrqState::GuestIrqsMask);
        _hardware.setPrivilegedMode(true);
    }

    ~ARMv2CoreRegisterFile() = default;

    // Accessors
    uint32_t getPSR() const noexcept
    {
        return _cpsr;
    }

    uint32_t setPSR(uint32_t psr) noexcept
    {
        // Possibly change the processor mode.
        bool isModeChanged = changeMode(Ag::forceFromScalar<ProcessorMode>(psr & PsrMask26::ModeBits));

        // Update the PSR.
        _cpsr = psr & PsrMask26::PrivilageBits;

        // Mask IRQs as required.
        _hardware.updateIrqMask(static_cast<uint8_t>(_cpsr >> PsrShift26::IrqDisable),
                                IrqState::GuestIrqsMask);

        return isModeChanged ? ExecResult::ModeChange : 0;
    }

    // 32-bit modes only!
    void savePSR() // noexcept
    {
        throw Ag::NotSupportedException("Saving PSR on an ARMv2.");
    }

    void setStatusFlags(uint8_t flags) noexcept
    {
        // Clear the previous flags.
        _cpsr &= ~PsrMask::Status;

        // Merge in the new flags.
        _cpsr |= static_cast<uint32_t>(flags & PsrMask::LowStatus) << PsrShift::Status;
    }

    uint32_t updatePSR(uint32_t psrBits) noexcept
    {
        // Determine what bits the instruction is allowed to update.
        uint32_t psrMask = (_cpsr & PsrMask26::ModeBits) ? PsrMask26::PrivilageBits :
                                                           PsrMask26::UserBits;

        // Update the PSR bits, preserving those which an unprivileged mode
        // should not be able to change.
        uint32_t newPsr = (_cpsr & ~psrMask) | (psrBits & psrMask);

        return setPSR(newPsr);
    }

    uint32_t getPC() const noexcept
    {
        return _coreRegisters[Ag::toScalar(CoreRegister::R15)];
    }

    void setPC(uint32_t pc) noexcept
    {
        _coreRegisters[Ag::toScalar(CoreRegister::R15)] = pc & ~PsrMask26::PrivilageBits;
    }

    void incrementPC(uint32_t delta) noexcept
    {
        _coreRegisters[Ag::toScalar(CoreRegister::R15)] += delta;
    }

    constexpr ProcessorMode getMode() const noexcept
    {
        return Ag::forceFromScalar<ProcessorMode>(_cpsr & PsrMask26::ModeBits);
    }

    constexpr bool isInPrivilegedMode() const noexcept
    {
        return Ag::forceFromScalar<ProcessorMode>(_cpsr & PsrMask26::ModeBits) != ProcessorMode::User26;
    }

    uint32_t getRn(GeneralRegister regId) const noexcept
    {
        return _coreRegisters[Ag::toScalar(regId)];
    }

    uint32_t setRn(GeneralRegister regId, uint32_t value) noexcept
    {
        uint32_t result;

        if (regId == GeneralRegister::R15)
        {
            // Update the PC.
            _coreRegisters[15] = value & ~PsrMask26::PrivilageBits;
            result = ExecResult::FlushPipeline;
        }
        else
        {
            _coreRegisters[Ag::toScalar(regId)] = value;
            result = 0;
        }

        return result;
    }

    uint32_t getUserRn(GeneralRegister regId) const noexcept
    {
        uint32_t value;

        // Registers R0-R7 are never banked.
        if (regId == GeneralRegister::R15)
        {
            // STM instructions store the PC + 4 and PSR bits.
            value = (_coreRegisters[15] + 4) | _cpsr;
        }
        else
        {
            ProcessorMode mode = getMode();

            if ((mode == ProcessorMode::User26) ||
                (regId < GeneralRegister::R8))
            {
                value = _coreRegisters[Ag::toScalar(regId)];
            }
            else if (mode == ProcessorMode::FastIrq26)
            {
                // Registers R8-R14 are banked.
                value = _userModeRegBank[Ag::toScalar(regId) - 8];
            }
            else if (regId < GeneralRegister::R13)
            {
                // The user mode register is in the current bank.
                value = _coreRegisters[Ag::toScalar(regId)];
            }
            else
            {
                // Get banked user mode R13 or R14.
                value = _userModeRegBank[Ag::toScalar(regId) - 13];
            }
        }

        return value;
    }

    void setUserRn(GeneralRegister regId, uint32_t value) noexcept
    {
        // Registers R0-R7 are never banked.
        if (regId < GeneralRegister::R15) // Should NEVER be R15.
        {
            ProcessorMode mode = getMode();

            if ((mode == ProcessorMode::User26) ||
                (regId < GeneralRegister::R8))
            {
                // The user bank is currently selected.
                _coreRegisters[Ag::toScalar(regId)] = value;
            }
            else if ((mode == ProcessorMode::FastIrq26) ||
                     (regId >= GeneralRegister::R13))
            {
                // User mode registers 8-14 (FIRQ mode) or
                // 13-14 (other non-user modes) are hidden.
                _userModeRegBank[Ag::toScalar(regId) - 8] = value;
            }
            else
            {
                // The register in question is not hidden.
                _coreRegisters[Ag::toScalar(regId)] = value;
            }
        }
    }

    uint32_t getRm(GeneralRegister regId) const noexcept
    {
        return (regId == GeneralRegister::R15) ?
            (_coreRegisters[15] | (_cpsr & PsrMask26::PrivilageBits)) :
            _coreRegisters[Ag::toScalar(regId)];
    }

    uint32_t getRs(GeneralRegister regId) const noexcept
    {
        return (regId == GeneralRegister::R15) ?
            (_coreRegisters[15] + 4) :
            _coreRegisters[Ag::toScalar(regId)];
    }

    uint32_t getRd(GeneralRegister regId) const noexcept
    {
        return (regId == GeneralRegister::R15) ? _cpsr :
                                                 _coreRegisters[Ag::toScalar(regId)];
    }

    uint32_t setRdAndStatus(GeneralRegister regId, uint32_t value,
                            uint8_t status) noexcept
    {
        uint32_t resultMask;

        if (regId == GeneralRegister::R15)
        {
            // Update the PSR with the result, ignoring the status value.
            resultMask = updatePSR(value) | ExecResult::FlushPipeline;

            // Update the PC.
            _coreRegisters[15] = (value & ~PsrMask26::PrivilageBits);
        }
        else
        {
            // Update the target register.
            _coreRegisters[Ag::toScalar(regId)] = value;

            // Update the status flags.
            _cpsr &= ~PsrMask::Status;
            _cpsr |= static_cast<uint32_t>(status) << PsrShift::Status;
            resultMask = 0;
        }

        return resultMask;
    }

    uint32_t getRx(GeneralRegister regId) const noexcept
    {
        return (regId == GeneralRegister::R15) ?
            ((_coreRegisters[15] + 4) | _cpsr) :
            _coreRegisters[Ag::toScalar(regId)];
    }

    // Operations
    uint32_t raiseReset() noexcept
    {
        // Store the current PC + PSR in R14_<mode>.
        uint32_t oldR15 = _coreRegisters[15] | _cpsr;

        // Disable ALL interrupts and update at the hardware level.
        _cpsr |= PsrMask26::IrqDisableBits;
        _hardware.updateIrqMask(IrqState::GuestIrqsMask, IrqState::GuestIrqsMask);

        // Switch to the appropriate processor mode.
        uint32_t result = changeMode(ProcessorMode::Svc26) ? ExecResult::ModeChange : 0;

        // Set the link register in the (possibly new) processor mode.
        _coreRegisters[14] = oldR15;

        // Branch through the reset hardware vector.
        _coreRegisters[15] = 0x00000000;

        return result | ExecResult::FlushPipeline;
    }

    uint32_t raiseUndefinedInstruction() noexcept
    {
        // Raise an exception and branch through the Undefined Instruction
        // hardware vector.
        return raiseException(0x00000004);
    }

    uint32_t raiseSoftwareInterrupt() noexcept
    {
        // Raise an exception and branch through the Software Interrupt
        // hardware vector.
        return raiseException(0x00000008);
    }

    uint32_t raisePreFetchAbort() noexcept
    {
        // Raise an exception and branch through the Pre-fetch Abort
        // hardware vector.
        return raiseException(0x0000000C);
    }

    uint32_t raiseDataAbort() noexcept
    {
        // Raise an exception and branch through the Data Abort
        // hardware vector.
        return raiseException(0x00000010);
    }

    uint32_t raiseAddressException() noexcept
    {
        // Raise an exception and branch through the Address Exception
        // hardware vector.
        return raiseException(0x00000014);
    }

    uint32_t handleIrq() noexcept
    {
        // Store the current PC + PSR in R14_Irq.
        uint32_t oldR15 = _coreRegisters[15] | _cpsr;

        // Disable normal interrupts.
        _cpsr |= PsrMask26::IrqDisableBit;
        _hardware.updateIrqMask(IrqState::IrqPending, IrqState::IrqPending);

        // Switch to 26-bit IRQ mode.
        uint32_t result = changeMode(ProcessorMode::Irq26) ? ExecResult::ModeChange : 0;

        // Set the link register in the (possibly new) processor mode.
        _coreRegisters[14] = oldR15;

        // Branch through the IRQ hardware vector.
        _coreRegisters[15] = 0x00000018;

        return result | ExecResult::FlushPipeline;
    }

    uint32_t handleFirq() noexcept
    {
        // Store the current PC + PSR in R14_Firq.
        uint32_t oldR15 = _coreRegisters[15] | _cpsr;

        // Disable ALL interrupts.
        _cpsr |= PsrMask26::IrqDisableBits;
        _hardware.updateIrqMask(IrqState::GuestIrqsMask, IrqState::GuestIrqsMask);

        // Switch to 26-bit FIRQ mode.
        uint32_t result = changeMode(ProcessorMode::FastIrq26) ? ExecResult::ModeChange : 0;

        // Set the link register in the (possibly new) processor mode.
        _coreRegisters[14] = oldR15;

        // Branch through the FIRQ hardware vector.
        _coreRegisters[15] = 0x0000001C;

        return result | ExecResult::FlushPipeline;
    }
};

//! @brief an implementation of the register file of an ARMv2a processor.
//! @tparam An object representing the underlying hardware which supports the
//! setPrivilegedMode(bool) and setIrqMask(uint8_t) member functions.
template<typename THardware>
class ARMv2aCoreRegisterFile : public ARMv2CoreRegisterFile<THardware>
{
private:
    // Internal Fields
    uint32_t _cp15Registers[6];

public:
    // Public Constants
    // The contents of CP15.CR0.
    // Bits 31-24: Designer code (0x41 = Acorn Computers Ltd.)
    // Bits 23-16: Manufacturer code (0x56 = VLSI Technology Inc.)
    // Bits 15-8: Part type (0x03 = VL86C020, i.e. ARM 3 processor)
    // Bits 7-0: Revision number (0x00 = Original part)
    static constexpr uint32_t IdRegisterValue = 0x41560300;

    // Construction/Destruction
    ARMv2aCoreRegisterFile(THardware &hw) :
        ARMv2CoreRegisterFile(hw)
    {
        // Set the ID register to a fixed value.
        _cp15Registers[0] = IdRegisterValue;

        std::fill_n(_cp15Registers + 1,
                    std::size(_cp15Registers) - 1, 0u);
    }

    ~ARMv2aCoreRegisterFile() = default;

    // Operations
    uint32_t getCP15Register(CoProcRegister regId) const noexcept
    {
        // NOTE: CP15 registers should only be accessible in a privileged
        // processor mode, otherwise the Undefined Instruction exception
        // should be raised.

        // Only CR0-CR5 are defined on ARM 3 processors.
        return (regId < CoProcRegister::CR6) ? _cp15Registers[Ag::toScalar(regId)] :
                                               0;
    }

    void setCP15Register(CoProcRegister regId, uint32_t value) noexcept
    {
        // NOTE: CP15 registers should only be accessible in a privileged
        // processor mode, otherwise the Undefined Instruction exception
        // should be raised.

        switch (regId)
        {
        case CoProcRegister::CR2: // [Read/Write] Cache Control
            _cp15Registers[2] = value & 0x7; // Only bits 0-2 writeable.
            break;

        case CoProcRegister::CR3: // [Read/Write] Cacheable Areas
        case CoProcRegister::CR4: // [Read/Write] Updatable Areas
        case CoProcRegister::CR5: // [Read/Write] Disruptive Areas
            _cp15Registers[Ag::toScalar(regId)] = value;
            break;

        case CoProcRegister::CR0: // [Read-only] ID Register
        case CoProcRegister::CR1: // [Write-only] Flush cache, value ignored.
        default: // CR6-CR15 not supported, values ignored.
            break;
        }
    }

    // Overrides
    uint32_t raiseReset() noexcept
    {
        // Reset the writeable CP15 register values.
        // NOTE ARM Family Data manual page 3-47 suggests the values
        // of registers CR3-CR5 are 'undefined at power-up'. We are assuming
        // that means every time the reset signal is asserted.
        std::fill_n(_cp15Registers + 1,
                    std::size(_cp15Registers) - 1, 0u);

        return ARMv2CoreRegisterFile::raiseReset();
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

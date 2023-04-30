//! @file ArmEmu/RegisterFile.inl
//! @brief The declaration of templates representing the state of an emulated
//! processor via its register values.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_REGISTER_FILE_INL__
#define __ARM_EMU_REGISTER_FILE_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "ArmEmu.hpp"
#include "ArmCore.hpp"

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Describes the bit offset of flags in the ARM Program Status Register
struct PsrShift
{
    // Shifts used when the status flags are in the low nibble.
    static constexpr uint8_t LowOverflow    = 0;
    static constexpr uint8_t LowCarry       = 1;
    static constexpr uint8_t LowZero        = 2;
    static constexpr uint8_t LowNegative    = 3;

    // Shifts used when the status flags are in a PSR.
    static constexpr uint8_t Status         = 28;
    static constexpr uint8_t Overflow       = Status + LowOverflow;
    static constexpr uint8_t Carry          = Status + LowCarry;
    static constexpr uint8_t Zero           = Status + LowZero;
    static constexpr uint8_t Negative       = Status + LowNegative;
};

//! @brief Describes masks of flags in the ARM Program Status Register.
struct PsrMask
{
    static constexpr uint8_t LowOverflow   = static_cast<uint8_t>(1) << PsrShift::LowOverflow;
    static constexpr uint8_t LowCarry      = static_cast<uint8_t>(1) << PsrShift::LowCarry;
    static constexpr uint8_t LowZero       = static_cast<uint8_t>(1) << PsrShift::LowZero;
    static constexpr uint8_t LowNegative   = static_cast<uint8_t>(1) << PsrShift::LowNegative;
    static constexpr uint8_t LowStatus     = static_cast<uint8_t>(0x0F);
    static constexpr uint8_t LowVC         = LowOverflow | LowCarry;

    static constexpr uint32_t Overflow      = static_cast<uint32_t>(1) << PsrShift::Overflow;
    static constexpr uint32_t Carry         = static_cast<uint32_t>(1) << PsrShift::Carry;
    static constexpr uint32_t Zero          = static_cast<uint32_t>(1) << PsrShift::Zero;
    static constexpr uint32_t Negative      = static_cast<uint32_t>(1) << PsrShift::Negative;
    static constexpr uint32_t Status        = static_cast<uint32_t>(LowStatus) << PsrShift::Status;
};

//! @brief Defines shift values specific to the ARMv2 PSR register.
struct PsrShift26
{
    //! @brief The shift needed to position the IRQ and FIRQ disable bits
    //! in the PSR.
    static constexpr uint8_t IrqDisable = 26;
};

//! @brief Defines mask values specific to the ARMv2 PSR register.
struct PsrMask26
{
    //! @brief A mask of bits in the PSR which can be changed from 26-bit privileged
    //! code.
    static constexpr uint32_t PrivilageBits = 0xFC000003;

    //! @brief A mask of bits in the PSR which can be changed from 26-bit
    //! non-privileged code.
    static constexpr uint32_t UserBits = 0xF0000000;

    //! @brief A mask of bits which define the processor mode in the 26-bit PSR.
    static constexpr uint32_t ModeBits = 0x00000003;

    //! @brief The IRQ bit in the 26-bit PSR.
    static constexpr uint32_t IrqDisableBit = 0x08000000;

    //! @brief The IRQ bit in the 26-bit PSR.
    static constexpr uint32_t FirqDisableBit = 0x04000000;

    //! @brief A mask of the IRQ and FIRQ disable bits in the 26-bit PSR.
    static constexpr uint32_t IrqDisableBits = IrqDisableBit | FirqDisableBit;
};

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Determines if a register file implementation has a combined PC/PSR
//! in R15. This exists for 26-bit mode register files. 32-bit register files
//! have separate PC and status registers.
template<typename TRegisterFile>
struct HasCombinedPcPsr
{
    static constexpr bool value = false;
};

//! @brief An example of an implementation of a register file supporting the
//! core registers.
class GenericCoreRegisterFile
{
public:
    // Constants
    //! @brief Defines whether the register file operates on the principle
    //! of a combined PC/PSR in R15, therefore making comparison instructions
    //! with the 'P' suffix legal.
    static constexpr bool IsCombinedPcPsr = true; // Or false for 32-bit modes.

    // Accessors
    //! @brief Gets the current Program Status Register value.
    uint32_t getPSR() const noexcept;

    //! @brief Updates the Program Status Register value, possibly altering
    //! the processor mode and the mask state of interrupts.
    //! @note All active bits of the PSR can be changed, there is no mode-based
    //! protection. For that, see updatePSR().
    //! @returns An InstructionResult mask indicating whether the processor
    //! mode was changed.
    uint32_t setPSR(uint32_t psr) noexcept;

    //! @brief Copies the contents of CPSR to SPSR.
    //! @note 32-bit modes only.
    void savePSR() noexcept;

    //! @brief Updates only the status flag values in the PSR.
    //! @param[in] flags The new status flags in the low nibble.
    void setStatusFlags(uint8_t flags) noexcept;

    //! @brief Updates the bits of the PSR which can be changed given
    //! the current processor mode.
    //! @param psrBits The new PSR bit values.
    //! @returns An InstructionResult mask indicating whether the processor
    //! mode was changed.
    uint32_t updatePSR(uint32_t psrBits) noexcept;

    //! @brief Gets the current value of the Program Counter, 8-bytes advanced
    //! from the address of the currently executing instruction.
    uint32_t getPC() const noexcept;

    //! @brief Sets the value of the Program Counter, altering the program flow.
    //! @param[in] pc The logical address of the next instruction to execute.
    void setPC(uint32_t pc) noexcept;

    //! @brief A specialised form of setPC() which doesn't cause a flush.
    //! @param[in] delta The amount to add to the PC.
    void incrementPC(uint32_t delta) noexcept;

    //! @brief Interprets the current processor mode from the PSR.
    ProcessorMode getMode() const noexcept;

    //! @brief Reads a register value, if R15, the PC is read.
    //! @param[in] regId The identifier of the register to read.
    uint32_t getRn(GeneralRegister regId) const noexcept;

    //! @brief Writes a register value, if R15, the PC is updated.
    //! @param[in] regId The identifier of the register to write.
    //! @param[in] value The new register value.
    //! @returns A mask of instruction results determining if the pipeline was
    //! flushed and/or if the processor mode was changed.
    uint32_t setRn(GeneralRegister regId, uint32_t value) noexcept;

    //! @brief Gets the value of a register from the user bank as used by the
    //! STM instruction.
    //! @param[in] regId The identifier of the register to read.
    //! @return The register contents. For R15 that is the PC + 4 and
    //! PSR bits if in a 26-bit mode.
    uint32_t getUserRn(GeneralRegister regId) const noexcept;

    //! @brief Sets the value of a register in the user bank as part of
    //! an LDM instruction.
    //! @param[in] regId The identifier of the register to read.
    //! @param[in] value The new register contents.
    //! @note Should never be used to write R15, but this is not checked.
    void setUserRn(GeneralRegister regId, uint32_t value) noexcept;

    //! @brief Reads a register value, if R15, the combined PC/PSR is read if
    //! operating in 26-bit mode.
    //! @param[in] regId The identifier of the register to read.
    uint32_t getRm(GeneralRegister regId) const noexcept;

    //! @brief Reads a register value, if R15, the PC + 4 is read, i.e. the
    //! address of the current instruction + 12.
    //! @param[in] regId The identifier of the register to read.
    uint32_t getRs(GeneralRegister regId) const noexcept;

    //! @brief Reads a register value, if R15 and running in a 26-bit mode
    //! the PSR is read.
    //! @param[in] regId The identifier of the register to read.
    //! @todo Why does this exist?
    uint32_t getRd(GeneralRegister regId) const noexcept;

    //! @brief Writes a register value, if R15 and running in 26-bit mode,
    //! the PSR flags are updated too.
    //! @param[in] regId The identifier of the register to write.
    //! @param[in] value The new register value.
    //! @param[in] status The status flag values in the low nibble to
    //! be set. If in a 26-bit mode and regId is R15, these will be ignored
    //! in favour of overwriting the PSR with value.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t setRdAndStatus(GeneralRegister regId, uint32_t value,
                            uint8_t status) noexcept;

    //! @brief Reads a register value, if R15, the PC + 4 is read, i.e. the
    //! address of the current instruction + 12, and combined with the PSR
    //! flags if running in a 26-bit mode. Used during STM.
    //! @param[in] regId The identifier of the register to read.
    uint32_t getRx(GeneralRegister regId) const noexcept;

    // Operations
    //! @brief Updates the processor state in response to the reset signal
    //! being received.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t raiseReset() noexcept;

    //! @brief Updates the processor state in response to an attempt to execute
    //! an undefined instruction.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t raiseUndefinedInstruction() noexcept;

    //! @brief Updates the processor state in response to execution of an SWI
    //! instruction.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t raiseSoftwareInterrupt() noexcept;

    //! @brief Updates the processor state in response to fetching an instruction
    //! without sufficient privileges.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t raisePreFetchAbort() noexcept;

    //! @brief Updates the processor state in response to accessing data without
    //! sufficient privileges.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t raiseDataAbort() noexcept;

    //! @brief Updates the processor state in response to accessing data at an
    //! address beyond the valid range.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t raiseAddressException() noexcept;

    //! @brief Updates the processor state in response to a pending interrupt.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t handleIrq() noexcept;

    //! @brief Updates the processor state in response to a pending fast interrupt.
    //! @returns A mask of InstructionResult bits indicating whether a
    //! pipeline flush or mode change occurred.
    uint32_t handleFirq() noexcept;
};

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

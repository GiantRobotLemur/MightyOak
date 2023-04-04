//! @file ArmEmu/RegisterFile.hpp
//! @brief The declaration of a layer of emulation which manages the internal
//! processor registers.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_REGISTER_FILE_HPP__
#define __ARM_EMU_REGISTER_FILE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"
#include "ArmEmu.hpp"

#include "Hardware.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Identifies a general purpose register.
enum class GeneralRegister : uint8_t
{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    Max,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Provides the registers defined within the processor and some fixed
//! operations for manipulating them.
class RegisterFile : public Hardware
{
private:
    // Internal Fields
    uint32_t _coreRegisters[toScalar(CoreRegister::Max)];
    uint32_t _userModeRegBank[7];   // R8-R14
    uint32_t _firqModeRegBank[7];   // R8-R14
    uint32_t _irqModeRegBank[2];    // R13-R14
    uint32_t _svcModeRegBank[2];    // R13-R14

    // Internal Functions
    constexpr bool changeMode(ProcessorMode newMode) noexcept;
    void raiseException(uint32_t newPc) noexcept;

protected:
    uint8_t getCarry() const;
    uint8_t getOverflow() const;
    uint8_t getStatusFlags() const;

public:
    // Public Constants
    static const uint8_t PsrStatusShift = 28;
    static const uint8_t PsrOverflowShift = 28;
    static const uint8_t PsrCarryShift = 29;
    static const uint8_t PsrZeroShift = 30;
    static const uint8_t PsrNegativeShift = 31;

    static const uint32_t PsrOverflowMask = 0x10000000;
    static const uint32_t PsrCarryMask = 0x20000000;
    static const uint32_t PsrZeroMask = 0x40000000;
    static const uint32_t PsrNegativeMask = 0x80000000;
    static const uint32_t PsrStatusMask = 0xF0000000;

    // Construction/Destruction
    RegisterFile();
    virtual ~RegisterFile() = default;

    // Accessors
    uint32_t getPSR() const;
    bool setPSR(uint32_t psr);
    void setStatusFlags(uint32_t flags);
    bool updatePSR(uint32_t psrBits);
    uint32_t getPC() const;
    void setPC(uint32_t pc);

    constexpr ProcessorMode getMode() const noexcept;
    uint32_t getRn(GeneralRegister regId) const;
    bool setRn(GeneralRegister regId, uint32_t value);
    uint32_t getUserRn(GeneralRegister regId) const noexcept;
    void setUserRn(GeneralRegister regId, uint32_t value) noexcept;
    uint32_t getRm(GeneralRegister regId) const;
    uint32_t getRs(GeneralRegister regId) const;
    uint32_t getRd(GeneralRegister regId) const;
    bool setRd(GeneralRegister regId, uint32_t value, bool updateStatus);
    uint32_t getRx(GeneralRegister regId) const;

    // Operations
    void raiseReset();
    void raiseUndefinedInstruction();
    void raiseSoftwareInterrupt();
    void raisePreFetchAbort();
    void raiseDataAbort();
    void raiseAddressException();
    void handleIrq();
    void handleFirq();
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Extracts a general purpose register identifier from an instruction
//! bit field.
//! @param[in] instruction The instruction bit field to query.
//! @param[in] lsb The 0-based index of the least significant bit of the
//! register identifier.
//! @return The register identifier.
inline constexpr GeneralRegister decodeReg(uint32_t instruction,
                                           uint8_t lsb) noexcept
{
    return fromScalar<GeneralRegister>(static_cast<uint8_t>(instruction >> lsb) & 0x0F);
}

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

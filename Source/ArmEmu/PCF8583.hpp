//! @file ArmEmu/PCF8583.hpp
//! @brief The declaration of an object which emulates a PCF8583 RTC/CMOS RAM
//! chip as used in the Acorn Archimedes for configuration storage.
//! @author GiantRobotLemur@na-se.co.uk - and Claude Code.
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_PCF8583_HPP__
#define __ARM_EMU_PCF8583_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "I2CBus.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Emulates a PCF8583 RTC/CMOS RAM chip at I2C address 0x50.
//! @details The PCF8583 has 256 bytes of addressable space:
//! - 0x00-0x0F: Control/status and BCD clock registers
//! - 0x10-0xFF: General purpose RAM (used by RISC OS for CMOS settings)
//!
//! On the Acorn Archimedes, RISC OS stores configuration settings in the
//! CMOS RAM area starting at offset 0x40. The device provides sensible
//! defaults for a Mode 12 (640x256 16-colour) desktop configuration.
class PCF8583 : public II2CDevice
{
public:
    // Public Constants
    static constexpr uint8_t I2CAddress = 0x50; // 7-bit address (0xA0 >> 1)
    static constexpr size_t RamSize = 256;

    // Construction/Destruction
    PCF8583();
    ~PCF8583() = default;

    // Accessors
    const uint8_t *getRam() const;
    uint8_t *getRam();

    // II2CDevice overrides
    virtual uint8_t getI2CAddress() const override;
    virtual bool onI2CWrite(uint8_t data, bool isFirst) override;
    virtual bool onI2CRead(uint8_t &data) override;
    virtual void onI2CStop() override;

private:
    // Internal Functions
    void initialiseDefaults();

    // Internal Fields
    uint8_t _ram[RamSize];
    uint8_t _regPointer;
    bool _pointerSet;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

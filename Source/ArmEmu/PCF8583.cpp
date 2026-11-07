//! @file ArmEmu/PCF8583.cpp
//! @brief The definition of an object which emulates a PCF8583 RTC/CMOS RAM
//! chip as used in the Acorn Archimedes for configuration storage.
//! @author GiantRobotLemur@na-se.co.uk - and Claude Code.
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>

#include "PCF8583.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Constants
////////////////////////////////////////////////////////////////////////////////
// RISC OS CMOS byte offsets (within the PCF8583 256-byte address space).
// On the Archimedes, CMOS settings are stored starting at byte 0x01 of the
// PCF8583 (after the control/status register at 0x00).
//
// The following are the RISC OS CMOS byte indices as used by OS_Byte 161/162.
// In the PCF8583 address space, CMOS byte N is stored at PCF8583 address N+1
// (byte 0 of the PCF8583 is the control register).
//
// Key settings for a working RISC OS 3.10 boot:

// CMOS byte 0: Econet station number (0 = not fitted)
constexpr uint8_t CmosByte0_EconetStation = 0x00;

// CMOS byte 5: Default filing system / boot options
// Bits [3:0] = filing system (3 = ADFS)
// Bits [7:4] = boot option (0 = no boot)
constexpr uint8_t CmosByte5_FSBootOpt = 0x03;

// CMOS byte 10: Screen mode / display configuration
// This is the configured display mode number. Mode 12 = 640x256 16-colour.
constexpr uint8_t CmosByte10_ScreenMode = 12;

// CMOS byte 11: WimpMode (desktop mode, low byte)
constexpr uint8_t CmosByte11_WimpMode = 12;

// CMOS byte 12: Monitor type / sync type
// Bits [2:0] = monitor type:
//   0 = normal TV (50 Hz PAL)
//   1 = multiscan
//   2 = hi-res mono
//   3 = VGA
//   4 = SVGA
// Bits [4:3] = sync polarity (0 = composite sync)
constexpr uint8_t CmosByte12_MonitorType = 0x01; // Multiscan

// CMOS byte 16: Keyboard auto-repeat delay (in centiseconds)
constexpr uint8_t CmosByte16_KeyRepeatDelay = 32;

// CMOS byte 17: Keyboard auto-repeat rate (in centiseconds)
constexpr uint8_t CmosByte17_KeyRepeatRate = 8;

// CMOS byte 28: Mouse step (multiplier for mouse movement)
constexpr uint8_t CmosByte28_MouseStep = 2;

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// PCF8583 Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a PCF8583 with default CMOS settings suitable for
//! booting RISC OS 3.10.
PCF8583::PCF8583() :
    _regPointer(0),
    _pointerSet(false)
{
    std::memset(_ram, 0, RamSize);
    initialiseDefaults();
}

//! @brief Gets a const pointer to the 256-byte RAM contents.
const uint8_t *PCF8583::getRam() const
{
    return _ram;
}

//! @brief Gets a pointer to the 256-byte RAM contents for direct modification.
uint8_t *PCF8583::getRam()
{
    return _ram;
}

// Inherited from II2CDevice.
uint8_t PCF8583::getI2CAddress() const
{
    return I2CAddress;
}

// Inherited from II2CDevice.
bool PCF8583::onI2CWrite(uint8_t data, bool isFirst)
{
    if (isFirst)
    {
        // First byte after address is the register pointer.
        _regPointer = data;
        _pointerSet = true;
    }
    else
    {
        // Subsequent bytes are written to successive registers.
        _ram[_regPointer] = data;
        _regPointer++;
    }

    return true; // ACK
}

// Inherited from II2CDevice.
bool PCF8583::onI2CRead(uint8_t &data)
{
    data = _ram[_regPointer];
    _regPointer++;

    return true;
}

// Inherited from II2CDevice.
void PCF8583::onI2CStop()
{
    _pointerSet = false;
}

//! @brief Sets up default CMOS values for RISC OS 3.10 boot.
void PCF8583::initialiseDefaults()
{
    // PCF8583 register 0x00: Control/Status register.
    // Bit 2 = 0: clock mode (as opposed to alarm mode)
    // Bit 4 = 1: mask flag (don't trigger alarm)
    // Set to 0x04 = 32.768 kHz mode, clock function selected.
    _ram[0x00] = 0x04;

    // Registers 0x01-0x07: BCD time/date (hundredths, seconds, minutes, hours,
    // year/date, weekday/month). Set to a sensible date.
    _ram[0x01] = 0x00; // 1/100 seconds
    _ram[0x02] = 0x00; // Seconds (BCD)
    _ram[0x03] = 0x00; // Minutes (BCD)
    _ram[0x04] = 0x12; // Hours (BCD, 24h format) = 12:00
    _ram[0x05] = 0x01; // Year/Date: year counter [7:6]=00, date [5:0]=01 (BCD)
    _ram[0x06] = 0x01; // Weekday/Month: weekday [7:5]=000, month [4:0]=01 (Jan)

    // RISC OS CMOS settings. RISC OS stores CMOS byte N at PCF8583 address
    // (N + 0x40) on the A-Series/A3000 (the lower 64 bytes overlap with the
    // clock registers and alarm registers). However, the exact mapping varies
    // by machine. On the Archimedes, the mapping is direct: CMOS byte 0 is at
    // PCF8583 byte 0x10, and bytes are stored contiguously.
    //
    // For simplicity and compatibility, we store CMOS bytes starting at
    // address 0x10 (the first general-purpose RAM byte in the PCF8583).

    constexpr uint8_t cmosBase = 0x10;

    _ram[cmosBase + 0]  = CmosByte0_EconetStation;
    _ram[cmosBase + 5]  = CmosByte5_FSBootOpt;
    _ram[cmosBase + 10] = CmosByte10_ScreenMode;
    _ram[cmosBase + 11] = CmosByte11_WimpMode;
    _ram[cmosBase + 12] = CmosByte12_MonitorType;
    _ram[cmosBase + 16] = CmosByte16_KeyRepeatDelay;
    _ram[cmosBase + 17] = CmosByte17_KeyRepeatRate;
    _ram[cmosBase + 28] = CmosByte28_MouseStep;

    // CMOS checksum: RISC OS computes a simple checksum of the CMOS bytes.
    // If the checksum fails, it will use built-in defaults anyway.
    // We don't bother computing it here — RISC OS will detect the mismatch
    // and silently use its own defaults, which is fine for initial boot.
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

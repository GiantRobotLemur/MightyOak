//! @file ArmEmu/VIDC10.cpp
//! @brief The definition of an object which emulates the function of the
//! VL86C310 VIDC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Utils.hpp"

#include "ArmEmu/VIDC10.hpp"
#include "ArmEmu/HostMessageID.hpp"
#include "ArmEmu/SystemContext.hpp"
#include "MemcHardware.hpp"

namespace Mo {
namespace Arm {

namespace {

//! @brief The pixel clock rates in MHz indexed by the 2-bit pixel rate field.
constexpr uint8_t PixelRates[] = { 8, 12, 16, 24 };

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// VIDC10 Member Definitions
////////////////////////////////////////////////////////////////////////////////
VIDC10::VIDC10(MemcHardware &parent) :
    _parent(parent),
    _context(nullptr),
    _vSyncActive(false),
    _borderColour(0),
    _soundFreq(0),
    _controlReg(0)
{
    std::memset(_palette, 0, sizeof(_palette));
    std::memset(_cursorColours, 0, sizeof(_cursorColours));
    std::memset(_stereoPositions, 0, sizeof(_stereoPositions));
    std::memset(_hRegs, 0, sizeof(_hRegs));
    std::memset(_vRegs, 0, sizeof(_vRegs));

    _vSyncTask.defineTask(&onVSync, this);
}

// Accessors
uint16_t VIDC10::getPaletteEntry(uint8_t index) const
{
    return (index < VIDCRegister::PaletteCount) ? _palette[index] : 0;
}

uint16_t VIDC10::getBorderColour() const
{
    return _borderColour;
}

uint16_t VIDC10::getCursorColour(uint8_t index) const
{
    return (index < VIDCRegister::CursorColourCount) ? _cursorColours[index] : 0;
}

uint16_t VIDC10::getHorizontalReg(uint8_t index) const
{
    return (index < VIDCRegister::HorizontalCount) ? _hRegs[index] : 0;
}

uint16_t VIDC10::getVerticalReg(uint8_t index) const
{
    return (index < VIDCRegister::VerticalCount) ? _vRegs[index] : 0;
}

uint8_t VIDC10::getControlReg() const
{
    return _controlReg;
}

uint8_t VIDC10::getSoundFreqReg() const
{
    return _soundFreq;
}

uint8_t VIDC10::getBitsPerPixel() const
{
    // Control register bits 2-3 encode BPP: 00=1, 01=2, 10=4, 11=8.
    return static_cast<uint8_t>(1) << ((_controlReg & VIDCControl::BppMask) >> VIDCControl::BppShift);
}

uint8_t VIDC10::getPixelRateMHz() const
{
    return PixelRates[_controlReg & VIDCControl::PixelRateMask];
}

uint32_t VIDC10::getDisplayWidth() const
{
    // The horizontal display region is between HDSR and HDER.
    // Each timing register value is in units of 2 pixels at the pixel rate.
    uint16_t hdsr = _hRegs[VIDCRegister::HDSR - VIDCRegister::HorizontalBase];
    uint16_t hder = _hRegs[VIDCRegister::HDER - VIDCRegister::HorizontalBase];

    if (hder > hdsr)
    {
        return static_cast<uint32_t>(hder - hdsr) * 2;
    }

    return 0;
}

uint32_t VIDC10::getDisplayHeight() const
{
    // The vertical display region is between VDSR and VDER.
    // Each timing register value is in units of 1 raster line.
    uint16_t vdsr = _vRegs[VIDCRegister::VDSR - VIDCRegister::VerticalBase];
    uint16_t vder = _vRegs[VIDCRegister::VDER - VIDCRegister::VerticalBase];

    if (vder > vdsr)
    {
        return static_cast<uint32_t>(vder - vdsr);
    }

    return 0;
}

uint64_t VIDC10::getFramePeriodTicks() const
{
    if (_context == nullptr)
    {
        return 0;
    }

    // The frame period is (VCR + 1) horizontal lines, each (HCR + 1) * 2
    // pixel clock periods wide.
    uint16_t hcr = _hRegs[VIDCRegister::HCR - VIDCRegister::HorizontalBase];
    uint16_t vcr = _vRegs[VIDCRegister::VCR - VIDCRegister::VerticalBase];

    if (hcr == 0 || vcr == 0)
    {
        return 0;
    }

    // Total pixel clocks per frame = (HCR + 1) * 2 * (VCR + 1)
    uint64_t pixelClocksPerFrame = static_cast<uint64_t>(hcr + 1) * 2 *
                                   static_cast<uint64_t>(vcr + 1);

    // Convert pixel clocks to master clock ticks.
    // Master clock frequency / pixel clock frequency = ticks per pixel clock.
    uint64_t masterFreq = _context->getMasterClockFrequency();
    uint64_t pixelFreq = static_cast<uint64_t>(getPixelRateMHz()) * 1000000;

    if (pixelFreq == 0)
    {
        return 0;
    }

    return (pixelClocksPerFrame * masterFreq) / pixelFreq;
}

void VIDC10::writeRegister(uint32_t value)
{
    // The register ID is encoded in bits 26-31 of the data word.
    // Bits 24-25 are always zero for valid writes.
    uint8_t regId = Ag::Bin::extractBits<uint8_t, 26, 6>(value);
    bool timingChanged = false;

    if (regId < VIDCRegister::PaletteCount)
    {
        // Video palette register 0-15.
        // Bits 0-12 hold the 13-bit physical colour (4:4:4 + supremacy).
        _palette[regId] = Ag::Bin::extractBits<uint16_t, 0, 13>(value);
    }
    else if (regId == VIDCRegister::BorderColour)
    {
        // Border colour register.
        _borderColour = Ag::Bin::extractBits<uint16_t, 0, 13>(value);
    }
    else if (regId >= VIDCRegister::CursorColourBase &&
             regId < VIDCRegister::CursorColourBase + VIDCRegister::CursorColourCount)
    {
        // Cursor colour registers 1-3 (register IDs 17-19).
        _cursorColours[regId - VIDCRegister::CursorColourBase] =
            Ag::Bin::extractBits<uint16_t, 0, 13>(value);
    }
    else if (regId >= VIDCRegister::StereoBase &&
             regId < VIDCRegister::StereoBase + VIDCRegister::StereoCount)
    {
        // Stereo image registers (3-bit stereo position).
        _stereoPositions[regId - VIDCRegister::StereoBase] =
            Ag::Bin::extractBits<uint8_t, 0, 3>(value);
    }
    else if (regId >= VIDCRegister::HorizontalBase &&
             regId < VIDCRegister::HorizontalBase + VIDCRegister::HorizontalCount)
    {
        // Horizontal timing registers (14-bit value, bits 14-23).
        _hRegs[regId - VIDCRegister::HorizontalBase] =
            Ag::Bin::extractBits<uint16_t, 14, 10>(value);
        timingChanged = true;
    }
    else if (regId >= VIDCRegister::VerticalBase &&
             regId < VIDCRegister::VerticalBase + VIDCRegister::VerticalCount)
    {
        // Vertical timing registers (14-bit value, bits 14-23).
        _vRegs[regId - VIDCRegister::VerticalBase] =
            Ag::Bin::extractBits<uint16_t, 14, 10>(value);
        timingChanged = true;
    }
    else if (regId == VIDCRegister::SoundFreq)
    {
        // Sound frequency register (bits 0-7, bit 8 is test only).
        _soundFreq = static_cast<uint8_t>(value);
    }
    else if (regId == VIDCRegister::Control)
    {
        // VIDC control register (bits 0-7 useful in normal operation).
        uint8_t oldCtrl = _controlReg;
        _controlReg = static_cast<uint8_t>(value);

        // If pixel rate changed, timing has changed.
        if ((oldCtrl & VIDCControl::PixelRateMask) !=
            (_controlReg & VIDCControl::PixelRateMask))
        {
            timingChanged = true;
        }
    }

    // If timing parameters changed, recalculate and reschedule VSync.
    if (timingChanged && _context != nullptr)
    {
        scheduleVSync();
    }
}

void VIDC10::scheduleVSync()
{
    uint64_t period = getFramePeriodTicks();

    if (period > 0)
    {
        _context->scheduleTaskDeltaTicks(&_vSyncTask, period);
        _vSyncActive = true;
    }
}

void VIDC10::onVSync(SystemContext &guestContext, uintptr_t taskContext)
{
    VIDC10 *vidc = reinterpret_cast<VIDC10 *>(taskContext);

    // Raise VSync IRQ (IOC IRQ A bit 3).
    vidc->_parent.raiseVSyncIrq();

    // Post a message to the host to signal that a frame boundary occurred.
    guestContext.postMessageToHost(
        static_cast<uint32_t>(HostMessageID::VSyncOccurred), 0, 0);

    // Schedule the next VSync.
    if (vidc->_vSyncActive)
    {
        uint64_t period = vidc->getFramePeriodTicks();

        if (period > 0)
        {
            guestContext.scheduleTaskDeltaTicks(&vidc->_vSyncTask, period);
        }
    }
}

// Inherited from IAddressRegion.
RegionType VIDC10::getType() const
{
    return RegionType::MMIO;
}

// Inherited from IAddressRegion.
Ag::string_cref_t VIDC10::getName() const
{
    static const Ag::String name("VIDC10");

    return name;
}

// Inherited from IAddressRegion.
Ag::string_cref_t VIDC10::getDescription() const
{
    static const Ag::String description("The VL86C310 RISC Video Controller");

    return description;
}

// Inherited from IAddressRegion.
uint32_t VIDC10::getSize() const
{
    return 0x200000;
}

// Inherited from IMMIOBlock.
uint32_t VIDC10::read(uint32_t /*offset*/)
{
    // VIDC registers are write-only, so this should never be called.
    return _context->getFuzz();
}

// Inherited from IMMIOBlock.
void VIDC10::write(uint32_t /*offset*/, uint32_t value)
{
    // The VIDC address space is write-only. The register is identified by
    // the data word itself, not the address offset.
    writeRegister(value);
}

// Inherited from IMMIOBlock.
void VIDC10::connect(const ConnectionContext &context)
{
    // Connect to the rest of the emulated system.
    _context = context.getInteropContext();
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

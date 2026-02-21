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
//! @brief Constructs an object to emulate the functions of the VIDC10 chip.
//! @param[in] parent The object providing access to the system memory map.
VIDC10::VIDC10(MemcHardware &parent) :
    _parent(parent),
    _context(nullptr),
    _videoInitAddr(0),
    _videoStartAddr(0),
    _videoEndAddr(0),
    _cursorInitAddr(0),
    _borderColour(0),
    _soundFreq(0),
    _controlReg(0),
    _vSyncActive(false)
{
    std::memset(_palette, 0, sizeof(_palette));
    std::memset(_cursorColours, 0, sizeof(_cursorColours));
    std::memset(_stereoPositions, 0, sizeof(_stereoPositions));
    std::memset(_hRegs, 0, sizeof(_hRegs));
    std::memset(_vRegs, 0, sizeof(_vRegs));

    _vSyncTask.defineTask(&onVSync, this);
}

//! @brief Gets the video DMA initial address (Vinit) as a physical byte offset.
uint32_t VIDC10::getVideoInitAddr() const
{
    return _videoInitAddr;
}

//! @brief Sets the video DMA initial address (Vinit) as a physical byte offset.
void VIDC10::setVideoInitAddr(uint32_t initAddr)
{
    _videoInitAddr = initAddr;
}

//! @brief Gets the video DMA start address (Vstart) as a physical byte offset.
uint32_t VIDC10::getVideoStartAddr() const
{
    return _videoStartAddr;
}

//! @brief Sets the video DMA start address (Vstart) as a physical byte offset.
void VIDC10::setVideoStartAddr(uint32_t startAddr)
{
    _videoStartAddr = startAddr;
}

//! @brief Gets the video DMA end address (Vend) as a physical byte offset.
uint32_t VIDC10::getVideoEndAddr() const
{
    return _videoEndAddr;
}

//! @brief Sets the video DMA end address (Vend) as a physical byte offset.
void VIDC10::setVideoEndAddr(uint32_t endAddr)
{
    _videoEndAddr = endAddr;
}

//! @brief Gets the cursor DMA initial address (Cinit) as a physical byte offset.
uint32_t VIDC10::getCursorInitAddr() const
{
    return _cursorInitAddr;
}

//! @brief Sets the cursor DMA initial address (Cinit) as a physical byte offset.
void VIDC10::setCursorInitAddr(uint32_t cursorAddr)
{
    _cursorInitAddr = cursorAddr;
}

//! @brief Gets a palette entry (0-15) as a 13-bit physical colour.
uint16_t VIDC10::getPaletteEntry(uint8_t index) const
{
    return (index < VIDCRegister::PaletteCount) ? _palette[index] : 0;
}

//! @brief Gets the border colour as a 13-bit physical colour.
uint16_t VIDC10::getBorderColour() const
{
    return _borderColour;
}

//! @brief Gets a cursor colour entry (0-2) as a 13-bit physical colour.
uint16_t VIDC10::getCursorColour(uint8_t index) const
{
    return (index < VIDCRegister::CursorColourCount) ? _cursorColours[index] : 0;
}

//! @brief Gets a horizontal timing register value.
//! @param[in] index The register index (0-7), corresponding to HCR, HSWR, HBSR,
//! HDSR, HDER, HBER, HCSR, HIR.
uint16_t VIDC10::getHorizontalReg(uint8_t index) const
{
    return (index < VIDCRegister::HorizontalCount) ? _hRegs[index] : 0;
}

//! @brief Gets a vertical timing register value.
//! @param[in] index The register index (0-7), corresponding to VCR, VSWR, VBSR,
//! VDSR, VDER, VBER, VCSR, VCER.
uint16_t VIDC10::getVerticalReg(uint8_t index) const
{
    return (index < VIDCRegister::VerticalCount) ? _vRegs[index] : 0;
}

//! @brief Gets the control register value.
uint8_t VIDC10::getControlReg() const
{
    return _controlReg;
}

//! @brief Gets the sound frequency register value.
uint8_t VIDC10::getSoundFreqReg() const
{
    return _soundFreq;
}

//! @brief Gets the bits per pixel from the control register (1, 2, 4, or 8).
uint8_t VIDC10::getBitsPerPixel() const
{
    // Control register bits 2-3 encode BPP: 00=1, 01=2, 10=4, 11=8.
    return static_cast<uint8_t>(1) << ((_controlReg & VIDCControl::BppMask) >> VIDCControl::BppShift);
}

//! @brief Gets the pixel clock rate in MHz from the control register.
uint8_t VIDC10::getPixelRateMHz() const
{
    return PixelRates[_controlReg & VIDCControl::PixelRateMask];
}

//! @brief Gets the display width in pixels derived from horizontal timing.
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

//! @brief Gets the display height in lines derived from vertical timing.
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

//! @brief Resets the state of the device.
void VIDC10::reset()
{
    // TODO: Should more be reset in here?
    _videoInitAddr = 0;
    _videoStartAddr = 0;
    _videoEndAddr = 0;
    _cursorInitAddr = 0;

    if (_vSyncTask.isScheduled() && (_context != nullptr))
    {
        _context->unscheduleTask(&_vSyncTask);
    }

    _vSyncActive = false;
}

//! @brief Writes a raw 32-bit VIDC register value. The register ID is
//! encoded in bits 24-31 of the value, with bits 24-25 always zero.
//! @param[in] value The 32-bit value written to the VIDC address space.
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

//! @brief Gets the frame period in master clock ticks calculated from
//! the current horizontal and vertical timing register values.
//! @return The frame period in master clock ticks, or 0 if timing
//! registers have not been configured.
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
    guestContext.postMessageToHost(Ag::toScalar(HostMessageID::VSyncOccurred), 0, 0);

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

// Inherited from IHardwareDevice.
void VIDC10::registerDevice(SystemContext &context)
{
    context.addDevice(this);

    // Add this object as the primary display device.
    context.addDeviceAlias(getName(), "DISPLAY");
}

// Inherited from IHardwareDevice.
void VIDC10::connect(SystemContext &context)
{
    // Connect to the rest of the emulated system.
    _context = &context;
}

// Inherited from IVideoFrameProvider.
bool VIDC10::getRawFrame(uint8_t *frameBuffer, size_t frameBufferSize,
                         uint32_t palette[256],
                         RawFrameInfo &info) const
{
    if (!_parent.isVideoDMAEnabled())
        return false;

    uint32_t width = getDisplayWidth();
    uint32_t height = getDisplayHeight();

    if (width == 0 || height == 0)
        return false;

    uint8_t bpp = getBitsPerPixel();
    uint32_t bytesPerRow = (width * bpp + 7) / 8;
    size_t totalBytes = static_cast<size_t>(bytesPerRow) * height;

    if (frameBufferSize < totalBytes)
        return false;

    // Copy raw frame buffer bytes, resolving DMA address wrapping.
    uint32_t vInit = getVideoInitAddr();
    uint32_t vStart = getVideoStartAddr();
    uint32_t vEnd = getVideoEndAddr();
    const uint8_t *ram = _parent.getRamData();
    uint32_t ramSize = _parent.getRamSize();

    uint32_t dmaAddr = vInit;
    uint8_t *dest = frameBuffer;

    for (uint32_t y = 0; y < height; ++y)
    {
        uint32_t bytesRemaining = bytesPerRow;

        while (bytesRemaining > 0)
        {
            uint32_t bytesBeforeWrap;

            if (vEnd > vStart && dmaAddr < vEnd)
                bytesBeforeWrap = vEnd - dmaAddr;
            else
                bytesBeforeWrap = bytesRemaining;

            uint32_t chunk = (bytesRemaining < bytesBeforeWrap)
                ? bytesRemaining : bytesBeforeWrap;

            uint32_t srcOffset = dmaAddr % ramSize;
            std::memcpy(dest, ram + srcOffset, chunk);

            dest += chunk;
            dmaAddr += chunk;
            bytesRemaining -= chunk;

            if (vEnd > vStart && dmaAddr >= vEnd)
                dmaAddr = vStart;
        }
    }

    // Build the 256-entry ARGB32 palette.
    if (bpp <= 4)
    {
        // For 1/2/4 BPP, convert the meaningful palette entries.
        uint32_t colourCount = 1u << bpp;

        for (uint32_t i = 0; i < colourCount; ++i)
        {
            palette[i] = Display::vidc13ToARGB32(
                getPaletteEntry(static_cast<uint8_t>(i)));
        }

        // Fill remaining entries with opaque black.
        for (uint32_t i = colourCount; i < 256; ++i)
            palette[i] = 0xFF000000;
    }
    else
    {
        // For 8 BPP, pre-expand all 256 byte values.
        // Low nibble selects a 13-bit base colour from the palette;
        // high nibble overrides the green channel (bits [7:4]).
        for (uint32_t byteVal = 0; byteVal < 256; ++byteVal)
        {
            uint16_t colour13 = getPaletteEntry(
                static_cast<uint8_t>(byteVal & 0x0F));

            uint8_t greenOverride = static_cast<uint8_t>(
                (byteVal >> 4) & 0x0F);

            colour13 = static_cast<uint16_t>(
                (colour13 & ~static_cast<uint16_t>(0x00F0)) |
                (static_cast<uint16_t>(greenOverride) << 4));

            palette[byteVal] = Display::vidc13ToARGB32(colour13);
        }
    }

    // Fill the frame info.
    info.Width = width;
    info.Height = height;
    info.BytesPerRow = bytesPerRow;
    info.BitsPerPixel = bpp;
    info.BorderColour = Display::vidc13ToARGB32(getBorderColour());

    return true;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

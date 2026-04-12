//! @file ArmEmu/VIDC10.inl
//! @brief The declaration of an object which emulates the function of the
//! VL86C310 VIDC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_VIDC10_INL__
#define __ARM_EMU_VIDC10_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstring>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Utils.hpp"

#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/HostMessageID.hpp"
#include "ArmEmu/SystemContext.hpp"
#include "ArmEmu/IVideoFrameProvider.hpp"
#include "ArmEmu/IInterruptController.hpp"
#include "VideoFrameSampler.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines constants for VIDC10 register group identification.
//! The top 6 bits of the data word (bits 26-31) select the register.
struct VIDCRegister
{
    // Palette registers: IDs 0-15 (0x00-0x3C)
    static constexpr uint8_t PaletteBase = 0;
    static constexpr uint8_t PaletteCount = 16;

    // Border colour: ID 16 (0x40)
    static constexpr uint8_t BorderColour = 16;

    // Cursor palette: IDs 17-19 (0x44-0x4C)
    static constexpr uint8_t CursorColourBase = 17;
    static constexpr uint8_t CursorColourCount = 3;

    // Stereo image registers: IDs 24-31 (0x60-0x7C)
    static constexpr uint8_t StereoBase = 24;
    static constexpr uint8_t StereoCount = 8;

    // Horizontal timing: IDs 32-39 (0x80-0x9C)
    static constexpr uint8_t HorizontalBase = 32;
    static constexpr uint8_t HorizontalCount = 8;
    static constexpr uint8_t HCR  = 32; //!< Horizontal cycle register.
    static constexpr uint8_t HSWR = 33; //!< Horizontal sync width register.
    static constexpr uint8_t HBSR = 34; //!< Horizontal border start register.
    static constexpr uint8_t HDSR = 35; //!< Horizontal display start register.
    static constexpr uint8_t HDER = 36; //!< Horizontal display end register.
    static constexpr uint8_t HBER = 37; //!< Horizontal border end register.
    static constexpr uint8_t HCSR = 38; //!< Horizontal cursor start register.
    static constexpr uint8_t HIR  = 39; //!< Horizontal interlace register.

    // Vertical timing: IDs 40-47 (0xA0-0xBC)
    static constexpr uint8_t VerticalBase = 40;
    static constexpr uint8_t VerticalCount = 8;
    static constexpr uint8_t VCR  = 40; //!< Vertical cycle register.
    static constexpr uint8_t VSWR = 41; //!< Vertical sync width register.
    static constexpr uint8_t VBSR = 42; //!< Vertical border start register.
    static constexpr uint8_t VDSR = 43; //!< Vertical display start register.
    static constexpr uint8_t VDER = 44; //!< Vertical display end register.
    static constexpr uint8_t VBER = 45; //!< Vertical border end register.
    static constexpr uint8_t VCSR = 46; //!< Vertical cursor start register.
    static constexpr uint8_t VCER = 47; //!< Vertical cursor end register.

    // Sound frequency: ID 48 (0xC0)
    static constexpr uint8_t SoundFreq = 48;

    // Control register: ID 56 (0xE0)
    static constexpr uint8_t Control = 56;
};

//! @brief Defines bit fields within the VIDC10 control register.
struct VIDCControl
{
    //! @brief Bits 0-1: Pixel rate selection.
    //! 00 = 8 MHz, 01 = 12 MHz, 10 = 16 MHz, 11 = 24 MHz.
    static constexpr uint8_t PixelRateMask = 0x03;

    //! @brief Bits 2-3: Bits per pixel.
    //! 00 = 1 BPP, 01 = 2 BPP, 10 = 4 BPP, 11 = 8 BPP.
    static constexpr uint8_t BppMask = 0x0C;
    static constexpr uint8_t BppShift = 2;

    //! @brief Bit 4: DMA request spacing.
    static constexpr uint8_t DMARequest = 0x10;

    //! @brief Bit 5: Interlace/progressive.
    //! 0 = progressive, 1 = interlaced.
    static constexpr uint8_t Interlace = 0x20;

    //! @brief Bit 6: Composite sync.
    static constexpr uint8_t CompositeSync = 0x40;

    //! @brief Bit 7: Test mode (not used in normal operation).
    static constexpr uint8_t TestMode = 0x80;
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which emulates the function of the VL86C310 VIDC part.
//! @tparam TAllowDiagnostics Used with if constexpr to determine of
//! IDiagnosticSink branches should be taken - otherwise they will be
//! optimised out.
template<bool TAllowDiagnostics>
class VIDC10 : public IVideoFrameProvider
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Internal Constants
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Used with if constexpr to determine of IDiagnosticSink branches
    //! should be taken - otherwise they will be optimised out.
    static constexpr bool AllowDiagnostics = TAllowDiagnostics;

    //! @brief The pixel clock rates in MHz indexed by the 2-bit pixel rate field.
    static constexpr uint32_t PixelRates[] = { 8000000, 12000000, 16000000, 24000000 };

    ///////////////////////////////////////////////////////////////////////////
    // Internal Fields
    ///////////////////////////////////////////////////////////////////////////
    IInterruptController *_irqController;
    SystemContext *_context;

    //! @brief An object to manage sampling the frame buffer.
    VideoFrameSampler _frameSamples;

    //! @brief A pointer to the host block representing the physical RAM of
    //! the guest system.
    void *_physicalRam;

    //! @brief The scheduled task for VSync generation.
    GuestTask _vSyncTask;

    //! @brief The size of the system RAM connected to the VIDC.
    uint32_t _physicalRamSize;

    //! @brief The video DMA Vinit value.
    uint32_t _videoInitAddr;

    //! @brief The video DMA Vstart value.
    uint32_t _videoStartAddr;

    //! @brief The video DMA Vend value.
    uint32_t _videoEndAddr;

    //! @brief The cursor DMA Cinit value.
    uint32_t _cursorInitAddr;

    //! @brief Video palette registers (16 entries, 13-bit physical colour).
    uint16_t _palette[VIDCRegister::PaletteCount];

    //! @brief Border colour register (13-bit physical colour).
    uint16_t _borderColour;

    //! @brief Cursor colour registers (3 entries, 13-bit physical colour).
    uint16_t _cursorColours[VIDCRegister::CursorColourCount];

    //! @brief Stereo image registers (8 entries, 3-bit stereo position).
    uint8_t _stereoPositions[VIDCRegister::StereoCount];

    //! @brief Holds the display configuration based on parameters
    //! written to the horizontal and vertical timing registers and
    //! the VIDC control register.
    FrameMetrics _displayConfig;

    //! @brief The vertical cycle time in scan lines.
    uint16_t _verticalCycleTicks = 0;

    //! @brief The vertical sync time in scan lines.
    uint16_t _verticalSyncTicks = 0;

    //! @brief The horizontal scan time length in 2-pixel units.
    uint16_t _horizontalCycleTicks = 0;

    //! @brief The horizontal sync time length in 2-pixel units.
    uint16_t _horizontalSyncTicks = 0;

    //! @brief The HDSR register contents, defining the timing of the start of
    //! the horizontal display, but dependent upon the current colour depth.
    uint16_t _horizontalDisplayStart = 0;

    //! @brief The HDER register contents, defining the timing of the end of
    //! the horizontal display, but dependent upon the current colour depth.
    uint16_t _horizontalDisplayEnd = 0;

    //! @brief Sound frequency register (8-bit, bit 8 is test only).
    uint8_t _soundFreq;

    //! @brief Control register (8 bits: pixel rate, BPP, interlace, etc.).
    uint8_t _controlReg;

    //! @brief Whether VSync scheduling is active.
    bool _vSyncActive;

    //! @brief Indicates whether VSync is in progress, as opposed to normal display.
    bool _isInVSync;

    //! @brief Indicates whether DMA is actively reading video data from
    //! memory to output to a display.
    bool _videoDMAActive;

private:
    ///////////////////////////////////////////////////////////////////////////
    // Internal Functions
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Copies data from a circular buffer in emulated physical memory
    //! to a host buffer as if it were transferred via DMA.
    //! @param[in] target The buffer to receive the data, already set to the
    //! correct size.
    //! @param[in] initAddr The guest address of the first byte of display data
    //! to sample.
    //! @param[in] startAddr The guest address of the beginning of the circular
    //! buffer being sampled.
    //! @param[in] endAddr The guest address of the end of the circular buffer
    //! begin sampled.
    //! @return The count of bytes copied to @p target.
    uint32_t transferDMADisplayData(Ag::ByteBlock &target, uint32_t initAddr,
                                    uint32_t startAddr, uint32_t endAddr)
    {
        // Ensure the circular buffer has a valid definition.
        if (startAddr >= endAddr)
            return 0;

        size_t dmaBufferSize = endAddr - startAddr;
        size_t dmaOffset = (initAddr >= endAddr) ? startAddr : initAddr;

        const uint8_t *ram = reinterpret_cast<const uint8_t *>(_physicalRam);
        size_t maxBytesToCopy = std::min(target.size(), dmaBufferSize);
        size_t bytesWritten;

        for (bytesWritten = 0; bytesWritten < maxBytesToCopy; )
        {
            // Calculate the amount of contiguous data we can copy.
            size_t blockSize = std::min(endAddr - dmaOffset,
                                        maxBytesToCopy - bytesWritten);

            // Transfer the block.
            memcpy(target.data() + bytesWritten,
                   ram + dmaOffset, blockSize);

            // Move on to the second part of the buffer.
            dmaOffset = startAddr;
            bytesWritten += blockSize;
        }

        return static_cast<uint32_t>(bytesWritten);
    }

    void scheduleVSync()
    {
        uint64_t frameTicks, vsyncTicks;

        if (getFrameTiming(frameTicks, vsyncTicks))
        {
            _isInVSync = false;

            // Ensure IOC.C7 needs reflects the current VSync state.
            if (_irqController != nullptr)
                _irqController->setVSyncState(false);

            _context->scheduleTaskDeltaTicks(&_vSyncTask, frameTicks - vsyncTicks);
            _vSyncActive = true;
        }
    }

    void onVSyncStart()
    {
        bool wasVSyncActive = _isInVSync;
        _isInVSync = !wasVSyncActive;

        // Raise VSync IRQ (IOC IRQ A bit 3) for the duration of VSync
        // also IOC.C7 needs to be high during VSync
        if (_irqController != nullptr)
            _irqController->setVSyncState(_isInVSync);

        if (_isInVSync)
        {
            if (_videoDMAActive)
            {
                // Only capture video data if it was actually being fed
                // to the output.
                _frameSamples.captureDisplayData(_physicalRam,
                                                 getVideoInitAddr(),
                                                 getVideoStartAddr(),
                                                 getVideoEndAddr());

                _frameSamples.captureCursorData(_physicalRam,
                                                getCursorInitAddr());
            }

            // Capture the final state of the sampled frame.
            uint32_t lastFrameIndex = _frameSamples.getCurrentFrameIndex();
            bool isLastFrameValid = _frameSamples.hasValidFrame();

            // Start the next frame.
            uint32_t pixelClock = getPixelRateHz();

            uint64_t ticksPerLine = (_context->getMasterClockFrequency() *
                                    static_cast<uint64_t>(_horizontalCycleTicks)) / pixelClock;

            _frameSamples.onVSyncStart(_context->getMasterClockTicks(),
                                       ticksPerLine,
                                       _displayConfig.VerticalDisplayStart);

            if (isLastFrameValid)
            {
                // Post a message to the host to signal that a frame boundary occurred
                // which identifies the sample containing the frame data.
                _context->postMessageToHost(HostMessageID::FrameBufferReady,
                                            lastFrameIndex, 0);
            }
        }
        else
        {
            // Start sampling the new frame.
            updateFrameConfiguration();

            // Use the current format or previous frame to initialise the next
            // frame to be sampled.
            _frameSamples.onVSyncEnd(this);
        }

        // Schedule the next VSync.
        if (_vSyncActive)
        {
            uint64_t syncPeriod;
            uint64_t framePeriod;

            if (getFrameTiming(framePeriod, syncPeriod))
            {
                uint64_t ticks = _isInVSync ? syncPeriod : (framePeriod - syncPeriod);

                _context->scheduleTaskDeltaTicks(&_vSyncTask, ticks);
            }
            else
            {
                _vSyncActive = false;
            }
        }
    }

    //! @brief Handles the system event when the VSync signal changes state.
    //! @param[in] guestContext The context of the system processing the event.
    //! @param[in] taskContext A raw pointer to the VIDC10 object.
    static void onVSyncStartCallback(SystemContext &guestContext, uintptr_t taskContext)
    {
        VIDC10 *self = reinterpret_cast<VIDC10 *>(taskContext);
        self->_context = &guestContext;

        self->onVSyncStart();
    }
public:
    ///////////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Constructs an object to emulate the functions of the VIDC10 chip.
    VIDC10() :
        _irqController(nullptr),
        _context(nullptr),
        _physicalRam(nullptr),
        _physicalRamSize(0),
        _videoInitAddr(0),
        _videoStartAddr(0),
        _videoEndAddr(0),
        _cursorInitAddr(0),
        _borderColour(0),
        _soundFreq(0),
        _controlReg(0),
        _vSyncActive(false),
        _isInVSync(false),
        _videoDMAActive(false)
    {
        std::memset(_palette, 0, sizeof(_palette));
        std::memset(_cursorColours, 0, sizeof(_cursorColours));
        std::memset(_stereoPositions, 0, sizeof(_stereoPositions));

        _vSyncTask.defineTask(&onVSyncStartCallback, this);
    }

    virtual ~VIDC10() = default;

    ///////////////////////////////////////////////////////////////////////////
    // Accessors
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Connects the VIDC to host memory which holds the contents of the
    //! physical RAM of the guest system.
    //! @param[in] physicalRam The address of the first byte of host memory
    //! which holds the contents of guest memory.
    //! @param[in] ramSize The count of bytes physicalRam points to.
    void initialiseRamAccess(void *physicalRam, uint32_t ramSize)
    {
        _physicalRam = physicalRam;
        _physicalRamSize = ramSize;
    }

    //! @brief Gets the video DMA initial address (Vinit) as a physical byte offset.
    uint32_t getVideoInitAddr() const
    {
        return _videoInitAddr;
    }

    //! @brief Sets the video DMA initial address (Vinit) as a physical byte offset.
    void setVideoInitAddr(uint32_t initAddr)
    {
        _videoInitAddr = initAddr;
    }

    //! @brief Gets the video DMA start address (Vstart) as a physical byte offset.
    uint32_t getVideoStartAddr() const
    {
        return _videoStartAddr;
    }

    //! @brief Sets the video DMA start address (Vstart) as a physical byte offset.
    void setVideoStartAddr(uint32_t startAddr)
    {
        _videoStartAddr = startAddr;
    }

    //! @brief Gets the video DMA end address (Vend) as a physical byte offset.
    uint32_t getVideoEndAddr() const
    {
        return _videoEndAddr;
    }

    //! @brief Sets the video DMA end address (Vend) as a physical byte offset.
    void setVideoEndAddr(uint32_t endAddr)
    {
        _videoEndAddr = endAddr;
    }

    //! @brief Gets the cursor DMA initial address (Cinit) as a physical byte offset.
    uint32_t getCursorInitAddr() const
    {
        return _cursorInitAddr;
    }

    //! @brief Sets the cursor DMA initial address (Cinit) as a physical byte offset.
    void setCursorInitAddr(uint32_t cursorAddr)
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
    uint16_t getCursorColour(uint8_t index) const
    {
        return (index < VIDCRegister::CursorColourCount) ? _cursorColours[index] : 0;
    }

    //! @brief Gets the control register value.
    uint8_t getControlReg() const
    {
        return _controlReg;
    }

    //! @brief Gets the sound frequency register value.
    uint8_t getSoundFreqReg() const
    {
        return _soundFreq;
    }

    //! @brief Gets the bits per pixel from the control register (1, 2, 4, or 8).
    uint8_t getBitsPerPixel() const
    {
        // Control register bits 2-3 encode BPP: 00=1, 01=2, 10=4, 11=8.
        return static_cast<uint8_t>(1) << ((_controlReg & VIDCControl::BppMask) >> VIDCControl::BppShift);
    }

    //! @brief Determines if the current pixel format is 8-bits.
    bool is8BitPalette() const
    {
        return Ag::Bin::extractBits<uint8_t, VIDCControl::BppShift, 2>(_controlReg) == 3;
    }

    //! @brief Gets the pixel clock rate in Hz from the control register.
    uint32_t getPixelRateHz() const
    {
        return PixelRates[_controlReg & VIDCControl::PixelRateMask];
    }

    //! @brief Gets the display width in pixels derived from horizontal timing.
    uint16_t getDisplayWidth() const
    {
        return (_displayConfig.HorizontalDisplayEnd > _displayConfig.HorizontalDisplayStart) ?
            (_displayConfig.HorizontalDisplayEnd - _displayConfig.HorizontalDisplayStart) : 0;
    }

    //! @brief Gets the display height in lines derived from vertical timing.
    uint16_t getDisplayHeight() const
    {
        return (_displayConfig.VerticalDisplayEnd > _displayConfig.VerticalDisplayStart) ?
            (_displayConfig.VerticalDisplayEnd - _displayConfig.VerticalDisplayStart) : 0;
    }

    //! @brief Ensure the display configuration contains correct values based on
    //! control and timing register contents.
    void updateFrameConfiguration()
    {
        // Fix up values which vary according to pixel format.
        // 
        // See page 15 of VIDC10 data sheet for the following values.
        static const uint16_t clockOffsets[] = { 19, 11, 7, 5 };
        static const AcornPixelFormat formats[] = { AcornPixelFormat::Palettised1Bpp,
                                                    AcornPixelFormat::Palettised2Bpp,
                                                    AcornPixelFormat::Palettised4Bpp,
                                                    AcornPixelFormat::Palettised8BppVIDC10 };

        uint8_t formatIndex = Ag::Bin::extractBits<uint8_t, VIDCControl::BppShift, 2>(_controlReg);
        uint16_t clockOffset = clockOffsets[formatIndex];

        _displayConfig.HorizontalDisplayStart = (_horizontalDisplayStart * 2) - clockOffset;
        _displayConfig.HorizontalDisplayEnd = (_horizontalDisplayEnd * 2) - clockOffset;
        _displayConfig.DisplayFormat = formats[formatIndex];
    }

    //! @brief Indicates whether the MEMC is generating reads of video data
    //! to output for display decoding.
    //! @retval true Video data is being read from main memory.
    //! @retval false No video data is being output.
    bool isVideoDMAActive() const
    {
        return _videoDMAActive;
    }

    //! @brief Sets whether video data is being read from main memory.
    //! @param[in] isActive True if video data is being output, false if the
    //! display should be blank due to no output.
    void setVideoDMAActive(bool isActive)
    {
        _videoDMAActive = isActive;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Operations
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Resets the state of the device.
    void reset()
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
    void writeRegister(uint32_t value)
    {
        // The register ID is encoded in bits 26-31 of the data word.
        // Bits 24-25 are always zero for valid writes.
        uint8_t regId = Ag::Bin::extractBits<uint8_t, 26, 6>(value);
        bool timingChanged = false;

        // Update the current frame sample with changes.
        FrameSample &sample = _frameSamples.getCurrentFrame();
        int16_t scanLine = (_context == nullptr) ? 0 :
            _frameSamples.getScanLineIndex(_context->getMasterClockTicks());

        if (regId < VIDCRegister::PaletteCount)
        {
            // Video palette register 0-15.
            // Bits 0-12 hold the 13-bit physical colour (4:4:4 + supremacy).
            uint16_t regValue = Ag::Bin::extractBits<uint16_t, 0, 13>(value);
            _palette[regId] = regValue;

            // Update the colour definition in the frame being sampled.
            CanonicalColour def = is8BitPalette() ? CanonicalColour::fromVIDC10FramePalette8Bit(regValue) :
                                                    CanonicalColour::fromVIDC10FramePalette(regValue);
            sample.addDisplayPaletteChange(scanLine, regId, def);
        }
        else if (regId == VIDCRegister::BorderColour)
        {
            // Border colour register.
            uint16_t regValue = Ag::Bin::extractBits<uint16_t, 0, 13>(value);

            _borderColour = regValue;
            sample.addBorderPaletteChange(scanLine,
                                          CanonicalColour::fromVIDC10FramePalette(regValue));
        }
        else if (regId >= VIDCRegister::CursorColourBase &&
                 regId < VIDCRegister::CursorColourBase + VIDCRegister::CursorColourCount)
        {
            // Cursor colour registers 1-3 (register IDs 17-19).
            const uint16_t regValue = Ag::Bin::extractBits<uint16_t, 0, 13>(value);
            const uint8_t colourId = regId - VIDCRegister::CursorColourBase;

            _cursorColours[colourId] = regValue;
            sample.addCursorPaletteChange(scanLine, colourId + 1,
                                          CanonicalColour::fromVIDC10CursorPalette(regValue,
                                                                                   colourId + 1));
        }
        else if (regId >= VIDCRegister::StereoBase &&
                 regId < VIDCRegister::StereoBase + VIDCRegister::StereoCount)
        {
            // Stereo image registers (3-bit stereo position).
            _stereoPositions[regId - VIDCRegister::StereoBase] =
                Ag::Bin::extractBits<uint8_t, 0, 3>(value);
        }
        else if (regId == VIDCRegister::HCR)
        {
            // HCR counts in units of 2 pixels. Retain the value in
            // pixel pairs in order to calculate the total pixel clocks
            // per horizontal line.
            _horizontalCycleTicks = (Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1) * 2;
            timingChanged = true;
        }
        else if (regId == VIDCRegister::HSWR)
        {
            // HSWR also counts in units of 2 pixels. Retain the value
            // in pixel pairs in order to calculate the total pixel clocks
            // per horizontal line.
            _horizontalSyncTicks = (Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1) * 2;
            timingChanged = true;
        }
        else if (regId == VIDCRegister::HBSR)
        {
            _displayConfig.HorizontalBorderStart = (Ag::Bin::extractBits<uint16_t, 14, 10>(value) * 2) + 1;
        }
        else if (regId == VIDCRegister::HDSR)
        {
            _horizontalDisplayStart = Ag::Bin::extractBits<uint16_t, 14, 10>(value);

            // NOTE: The value of K in the expression below is dependent upon
            // the pixel format, so can only be calculated at VSync.
            //
            // _displayConfig.HorizontalDisplayStart = (_horizontalDisplayStart * 2) - K
        }
        else if (regId == VIDCRegister::HDER)
        {
            _horizontalDisplayEnd = Ag::Bin::extractBits<uint16_t, 14, 10>(value);

            // NOTE: The value of K in the expression below is dependent upon
            // the pixel format, so can only be calculated at VSync.
            //
            // _displayConfig.HorizontalDisplayEnd = (_horizontalDisplayEnd * 2) - K
        }
        else if (regId == VIDCRegister::HBER)
        {
            _displayConfig.HorizontalBorderEnd =
                (Ag::Bin::extractBits<uint16_t, 14, 10>(value) * 2) + 1;
        }
        else if (regId == VIDCRegister::HCSR)
        {
            // The Horizontal Cursor Start register is actually 11 bits.
            // TODO: HCSR is actually 13-bits, but only in hi-res mono mode.
            _displayConfig.HorizontalCursorStart =
                Ag::Bin::extractBits<uint16_t, 13, 11>(value) + 6;
        }
        else if (regId == VIDCRegister::VCR)
        {
            _verticalCycleTicks = Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1;
            timingChanged = true;
        }
        else if (regId == VIDCRegister::VSWR)
        {
            _verticalSyncTicks = Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1;
            timingChanged = true;
        }
        else if (regId == VIDCRegister::VBSR)
        {
            _displayConfig.VerticalBorderStart = Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1;
        }
        else if (regId == VIDCRegister::VDSR)
        {
            _displayConfig.VerticalDisplayStart = Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1;
        }
        else if (regId == VIDCRegister::VDER)
        {
            _displayConfig.VerticalDisplayEnd = Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1;
        }
        else if (regId == VIDCRegister::VBER)
        {
            _displayConfig.VerticalBorderEnd = Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1;
        }
        else if (regId == VIDCRegister::VCSR)
        {
            _displayConfig.VerticalCursorStart = Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1;
        }
        else if (regId == VIDCRegister::VCER)
        {
            _displayConfig.VerticalCursorEnd = Ag::Bin::extractBits<uint16_t, 14, 10>(value) + 1;
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

    //! @brief Gets the total frame period and sync period in master clock ticks
    //! calculated from the current horizontal and vertical timing register values.
    //! @param[out] totalFrameTicks The total frame period in master clock ticks,
    //! or 0 if timing registers have not been configured.
    //! @param[out] vsyncTicks The VSync period in master clock ticks, which is
    //! included in @p totalFrameTicks.
    //! @retval true Frame timing has been configured.
    //! @retval false Frame timing has not yet been configured.
    bool getFrameTiming(uint64_t &totalFrameTicks, uint64_t &vsyncTicks) const
    {
        totalFrameTicks = vsyncTicks = 0;

        if ((_context == nullptr) ||
            _horizontalCycleTicks == 0 ||
            _verticalCycleTicks == 0)
            return false;

        uint64_t pixelClocksPerLine = _horizontalCycleTicks;
        uint64_t pixelClocksPerFrame = pixelClocksPerLine * _verticalCycleTicks;

        // Convert pixel clocks to master clock ticks.
        // Master clock frequency / pixel clock frequency = ticks per pixel clock.
        uint64_t masterFreq = _context->getMasterClockFrequency();
        uint64_t pixelFreq = getPixelRateHz();

        if (pixelFreq == 0)
            return false;

        uint64_t pixelClocksPerSync = pixelClocksPerLine * _verticalSyncTicks;
        vsyncTicks = (pixelClocksPerSync * masterFreq) / pixelFreq;
        totalFrameTicks = (pixelClocksPerFrame * masterFreq) / pixelFreq;

        return (vsyncTicks < totalFrameTicks) && (totalFrameTicks != 0);
    }

    //! @brief Converts a 13-bit VIDC physical colour to ARGB8888 format.
    //! @details VIDC 13-bit colour layout:
    //! - Bits [3:0] = Red (4-bit)
    //! - Bits [7:4] = Green (4-bit)
    //! - Bits [11:8] = Blue (4-bit)
    //! - Bit [12] = Supremacy (0 = opaque, 1 = transparent)
    //! Each 4-bit channel is expanded to 8-bit by nibble replication.
    static uint32_t vidc13ToARGB32(uint16_t vidc13)
    {
        uint8_t r4 = static_cast<uint8_t>(vidc13 & 0x0F);
        uint8_t g4 = static_cast<uint8_t>((vidc13 >> 4) & 0x0F);
        uint8_t b4 = static_cast<uint8_t>((vidc13 >> 8) & 0x0F);

        // Expand 4-bit to 8-bit by nibble replication: 0xN -> 0xNN.
        uint8_t r = static_cast<uint8_t>((r4 << 4) | r4);
        uint8_t g = static_cast<uint8_t>((g4 << 4) | g4);
        uint8_t b = static_cast<uint8_t>((b4 << 4) | b4);

        // Supremacy bit 12: clear = opaque (0xFF), set = transparent (0x00).
        uint8_t a = (vidc13 & 0x1000) ? 0x00 : 0xFF;

        return (static_cast<uint32_t>(a) << 24) |
            (static_cast<uint32_t>(r) << 16) |
            (static_cast<uint32_t>(g) << 8) |
            static_cast<uint32_t>(b);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Overrides
    ///////////////////////////////////////////////////////////////////////////
    // Inherited from IAddressRegion.
    virtual RegionType getType() const override
    {
        return RegionType::MMIO;
    }

    // Inherited from IAddressRegion.
    virtual Ag::string_cref_t getName() const override
    {
        static const Ag::String name("VIDC10");

        return name;
    }

    // Inherited from IAddressRegion.
    virtual Ag::string_cref_t getDescription() const override
    {
        static const Ag::String description("The VL86C310 RISC Video Controller");

        return description;
    }

    // Inherited from IAddressRegion.
    virtual uint32_t getSize() const override
    {
        return 0x200000;
    }

    // Inherited from IMMIOBlock.
    virtual uint32_t read(uint32_t /*offset*/) override
    {
        // VIDC registers are write-only, so this should never be called.
        return _context->getFuzz();
    }

    // Inherited from IMMIOBlock.
    virtual void write(uint32_t /*offset*/, uint32_t value) override
    {
        // The VIDC address space is write-only. The register is identified by
        // the data word itself, not the address offset.
        writeRegister(value);
    }

    // Inherited from IHardwareDevice.
    virtual void registerDevice(SystemContext &context) override
    {
        context.addDevice(this);

        // Add this object as the primary display device.
        context.addDeviceAlias(getName(), "DISPLAY");
    }

    // Inherited from IHardwareDevice.
    virtual void connect(SystemContext &context) override
    {
        // Connect to the rest of the emulated system.
        _context = &context;

        _context->tryFindTypedDevice("IOC", _irqController);
    }

    // Inherited from IVideoFrameProvider.
    virtual uint32_t captureDisplayPalette(CanonicalColour *definitions,
                                           uint32_t count) const override
    {
        uint32_t safeCount = std::min(count, static_cast<uint32_t>(VIDCRegister::PaletteCount));

        auto bpp = getBitsPerPixel();

        if (bpp == 8)
        {
            // There are only 16 palette entries, which are combined with the
            // target pixel to create a colour.
            for (uint32_t i = 0; i < safeCount; ++i)
            {
                definitions[i] = CanonicalColour::fromVIDC10FramePalette8Bit(_palette[i]);
            }
        }
        else
        {
            for (uint32_t i = 0; i < safeCount; ++i)
            {
                definitions[i] = CanonicalColour::fromVIDC10FramePalette(_palette[i]);
            }
        }

        return safeCount;
    }

    // Inherited from IVideoFrameProvider.
    virtual uint32_t captureCursorPalette(CanonicalColour *definitions,
                                          uint32_t count) const override
    {
        uint32_t safeCount = std::min(count, static_cast<uint32_t>(VIDCRegister::CursorColourCount + 1));

        if (safeCount > 0)
        {
            // Cursor colour #0 is always transparent.
            definitions[0] = CanonicalColour::Transparent;
        }

        for (uint32_t i = 1; i < safeCount; ++i)
        {
            definitions[i] = CanonicalColour::fromVIDC10FramePalette(_palette[i - 1]);
        }

        return safeCount;
    }

    // Inherited from IVideoFrameProvider.
    virtual CanonicalColour captureBorderColour() const override
    {
        return CanonicalColour::fromVIDC10FramePalette(_borderColour);
    }

    // Inherited from IVideoFrameProvider.
    virtual const FrameMetrics &getFrameConfiguration() const override
    {
        return _displayConfig;
    }

    // Inherited from IVideoFrameProvider.
    virtual const FrameSample *getSampledFrame(uint32_t id) const override
    {
        return &_frameSamples.getIndexedFrame(id);
    }

};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

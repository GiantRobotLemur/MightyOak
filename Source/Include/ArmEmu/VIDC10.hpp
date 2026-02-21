//! @file ArmEmu/VIDC10.hpp
//! @brief The declaration of an object which emulates the function of the
//! VL86C310 VIDC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_VIDC10_HPP__
#define __ARM_EMU_VIDC10_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AddressMap.hpp"
#include "SystemContext.hpp"
#include "IVideoFrameProvider.hpp"

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
class MemcHardware;

//! @brief An object which emulates the function of the VL86C310 VIDC part.
class VIDC10 : public IVideoFrameProvider
{
public:
    // Construction/Destruction
    VIDC10(MemcHardware &parent);
    virtual ~VIDC10() = default;

    // Accessors
    uint32_t getVideoInitAddr() const;
    void setVideoInitAddr(uint32_t initAddr);
    uint32_t getVideoStartAddr() const;
    void setVideoStartAddr(uint32_t startAddr);
    uint32_t getVideoEndAddr() const;
    void setVideoEndAddr(uint32_t endAddr);
    uint32_t getCursorInitAddr() const;
    void setCursorInitAddr(uint32_t cursorAddr);
    uint16_t getPaletteEntry(uint8_t index) const;
    uint16_t getBorderColour() const;
    uint16_t getCursorColour(uint8_t index) const;
    uint16_t getHorizontalReg(uint8_t index) const;
    uint16_t getVerticalReg(uint8_t index) const;
    uint8_t getControlReg() const;
    uint8_t getSoundFreqReg() const;
    uint8_t getBitsPerPixel() const;
    uint8_t getPixelRateMHz() const;
    uint32_t getDisplayWidth() const;
    uint32_t getDisplayHeight() const;

    // Operations
    void reset();
    void writeRegister(uint32_t value);
    uint64_t getFramePeriodTicks() const;

    // Overrides
    virtual RegionType getType() const override;
    virtual Ag::string_cref_t getName() const override;
    virtual Ag::string_cref_t getDescription() const override;
    virtual uint32_t getSize() const override;

    virtual uint32_t read(uint32_t offset) override;
    virtual void write(uint32_t offset, uint32_t value) override;
    virtual void registerDevice(SystemContext &context) override;
    virtual void connect(SystemContext &context) override;
    virtual bool getRawFrame(uint8_t *frameBuffer, size_t frameBufferSize,
                             uint32_t palette[256],
                             RawFrameInfo &info) const override;
private:
    // Internal Functions
    void scheduleVSync();
    static void onVSync(SystemContext &guestContext, uintptr_t taskContext);

    // Internal Fields
    MemcHardware &_parent;
    SystemContext *_context;

    //! @brief The scheduled task for VSync generation.
    GuestTask _vSyncTask;

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

    //! @brief Horizontal timing registers (8 entries, 10-bit values).
    uint16_t _hRegs[VIDCRegister::HorizontalCount];

    //! @brief Vertical timing registers (8 entries, 10-bit values).
    uint16_t _vRegs[VIDCRegister::VerticalCount];

    //! @brief Sound frequency register (8-bit, bit 8 is test only).
    uint8_t _soundFreq;

    //! @brief Control register (8 bits: pixel rate, BPP, interlace, etc.).
    uint8_t _controlReg;

    //! @brief Whether VSync scheduling is active.
    bool _vSyncActive;

};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

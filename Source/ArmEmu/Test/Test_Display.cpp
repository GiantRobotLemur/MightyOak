//! @file ArmEmu/Test/Test_Display.cpp
//! @brief The definition of unit tests for the Display framebuffer renderer.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include "Ag/Core/Binary.hpp"

#include "ArmEmu/Display.hpp"
#include "ArmEmu/VIDC10.hpp"
#include "MemcHardware.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

//! @brief Constructs a 32-bit VIDC register write value from a register ID
//! and data payload.
uint32_t makeVidcWord(uint8_t regId, uint32_t data)
{
    return (static_cast<uint32_t>(regId) << 26) | data;
}

//! @brief Constructs a horizontal/vertical timing register value.
uint32_t makeTimingWord(uint8_t regId, uint16_t timingValue)
{
    return makeVidcWord(regId, static_cast<uint32_t>(timingValue) << 14);
}

////////////////////////////////////////////////////////////////////////////////
// Test Fixture
////////////////////////////////////////////////////////////////////////////////

//! @brief A test fixture that creates a real MemcHardware instance with
//! default Options and provides helpers for configuring the emulated
//! video subsystem.
class DisplayTest : public ::testing::Test
{
protected:
    AddressMap _readDevices, _writeDevices;
    MemcHardware _hw;
    VIDC10 *_vidc;

    DisplayTest() :
        _hw(Options(), _readDevices, _writeDevices),
        _vidc(nullptr)
    {
        const Ag::String vidcName("VIDC10");

        IHardwareDeviceCollection devices;
        devices.reserve(16);
        _hw.addIntegralHardware(devices);

        for (auto device : devices)
        {
            if ((device->getName() == vidcName) &&
                Ag::tryCast(device, _vidc))
                break;
        }

        _hw.reset();
        _hw.setPrivilegedMode(true);
    }

    //! @brief Writes a VIDC register via the MEMC write path.
    void writeVidc(uint32_t vidcWord)
    {
        _hw.write<uint32_t>(MEMC::VidcStart, vidcWord);
    }

    //! @brief Sets a palette entry to a 13-bit VIDC colour.
    void setPalette(uint8_t index, uint16_t colour13)
    {
        writeVidc(makeVidcWord(index, colour13));
    }

    //! @brief Sets the VIDC control register.
    void setControlReg(uint8_t ctrl)
    {
        writeVidc(makeVidcWord(VIDCRegister::Control, ctrl));
    }

    //! @brief Configures a video mode with the given dimensions and BPP.
    //! @param[in] width The display width in pixels.
    //! @param[in] height The display height in pixels.
    //! @param[in] bppCode The BPP code (0=1bpp, 1=2bpp, 2=4bpp, 3=8bpp).
    void configureVideoMode(uint32_t width, uint32_t height, uint8_t bppCode)
    {
        // Horizontal timing: HDSR=10, HDER=10+(width/2), HCR=HDER+20
        uint16_t hdsr = 10;
        uint16_t hder = static_cast<uint16_t>(hdsr + width / 2);
        uint16_t hcr = static_cast<uint16_t>(hder + 20);

        writeVidc(makeTimingWord(VIDCRegister::HDSR, hdsr));
        writeVidc(makeTimingWord(VIDCRegister::HDER, hder));
        writeVidc(makeTimingWord(VIDCRegister::HCR, hcr));

        // Vertical timing: VDSR=10, VDER=10+height, VCR=VDER+20
        uint16_t vdsr = 10;
        uint16_t vder = static_cast<uint16_t>(vdsr + height);
        uint16_t vcr = static_cast<uint16_t>(vder + 20);

        writeVidc(makeTimingWord(VIDCRegister::VDSR, vdsr));
        writeVidc(makeTimingWord(VIDCRegister::VDER, vder));
        writeVidc(makeTimingWord(VIDCRegister::VCR, vcr));

        // Set control register: pixel rate 8 MHz (00) + BPP code.
        setControlReg(static_cast<uint8_t>(bppCode << VIDCControl::BppShift));
    }

    //! @brief Writes MEMC DMA registers (Vinit, Vstart, Vend) and enables
    //! video DMA via the MEMC control register.
    //! @param[in] vInit The Vinit physical byte offset.
    //! @param[in] vStart The Vstart physical byte offset.
    //! @param[in] vEnd The Vend physical byte offset.
    void setDmaRegisters(uint32_t vInit, uint32_t vStart, uint32_t vEnd)
    {
        // Vinit: MEMC register case 0, address = 0x3600000 | offset
        _hw.write<uint32_t>(0x3600000 | vInit, 0);

        // Vstart: MEMC register case 1, address = 0x3620000 | offset
        _hw.write<uint32_t>(0x3620000 | vStart, 0);

        // Vend: MEMC register case 2, address = 0x3640000 | offset
        _hw.write<uint32_t>(0x3640000 | vEnd, 0);
    }

    //! @brief Enables video DMA by writing the MEMC control register.
    void enableVideoDMA()
    {
        // MEMC control register: case 7, page size 4KB (bits 2-3 = 00),
        // video DMA enabled (bit 10), sound DMA disabled.
        // Address = 0x36E0000 | (page_size << 2) | (video_dma << 10)
        // page_size = 0 (4KB), video_dma = 1 => 0x36E0400
        _hw.write<uint32_t>(0x36E0400, 0);
    }

    //! @brief Writes raw bytes to physical RAM at a given offset.
    void writePhysicalRam(uint32_t offset, const uint8_t *data, size_t length)
    {
        for (size_t i = 0; i < length; ++i)
        {
            _hw.write<uint8_t>(MEMC::PhysRamStart + offset +
                               static_cast<uint32_t>(i), data[i]);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////

// Test 1: vidc13ToARGB32 colour conversion.
TEST_F(DisplayTest, Vidc13ToARGB32Conversion)
{
    // Black: all channels 0, no supremacy.
    EXPECT_EQ(Display::vidc13ToARGB32(0x0000), 0xFF000000u);

    // White: all channels 0xF => 0xFF.
    EXPECT_EQ(Display::vidc13ToARGB32(0x0FFF), 0xFFFFFFFFu);

    // Pure red: R=0xF, G=0, B=0.
    EXPECT_EQ(Display::vidc13ToARGB32(0x000F), 0xFFFF0000u);

    // Pure green: R=0, G=0xF, B=0.
    EXPECT_EQ(Display::vidc13ToARGB32(0x00F0), 0xFF00FF00u);

    // Pure blue: R=0, G=0, B=0xF.
    EXPECT_EQ(Display::vidc13ToARGB32(0x0F00), 0xFF0000FFu);

    // Supremacy bit set: transparent (alpha = 0).
    EXPECT_EQ(Display::vidc13ToARGB32(0x1000), 0x00000000u);

    // Supremacy + white: transparent white.
    EXPECT_EQ(Display::vidc13ToARGB32(0x1FFF), 0x00FFFFFFu);

    // Mid-range colour: R=5, G=A, B=3 => R=0x55, G=0xAA, B=0x33.
    EXPECT_EQ(Display::vidc13ToARGB32(0x03A5), 0xFF55AA33u);
}

// Test 2: 1 BPP rendering — 16x2 display with alternating 0xAA pattern.
TEST_F(DisplayTest, Render1Bpp)
{
    configureVideoMode(16, 2, 0); // 1 BPP

    // Set palette: index 0 = black (0x000), index 1 = white (0xFFF).
    setPalette(0, 0x0000);
    setPalette(1, 0x0FFF);

    // 16 pixels at 1 BPP = 2 bytes per scanline, 4 bytes total.
    // 0xAA = 10101010 binary. LSB first: pixel 0=0, pixel 1=1, pixel 2=0, ...
    uint8_t ramData[] = { 0xAA, 0xAA, 0xAA, 0xAA };

    setDmaRegisters(0, 0, 0x1000);
    enableVideoDMA();
    writePhysicalRam(0, ramData, sizeof(ramData));

    Display display(*_vidc, _hw);
    FrameInfo info = {};
    std::vector<uint32_t> pixels(16 * 2);

    ASSERT_TRUE(display.renderFrame(pixels.data(), pixels.size(), info));
    EXPECT_EQ(info.Width, 16u);
    EXPECT_EQ(info.Height, 2u);
    EXPECT_EQ(info.PixelCount, 32u);

    uint32_t black = 0xFF000000u;
    uint32_t white = 0xFFFFFFFFu;

    // 0xAA = bits 0,1,0,1,0,1,0,1 => black, white, black, white, ...
    for (uint32_t i = 0; i < 32; ++i)
    {
        uint32_t expected = (i % 2 == 0) ? black : white;
        EXPECT_EQ(pixels[i], expected) << "Pixel " << i;
    }
}

// Test 3: 2 BPP rendering — 8x1 display.
TEST_F(DisplayTest, Render2Bpp)
{
    configureVideoMode(8, 1, 1); // 2 BPP

    // Set palette entries 0-3 to distinct colours.
    setPalette(0, 0x0000); // Black
    setPalette(1, 0x000F); // Red
    setPalette(2, 0x00F0); // Green
    setPalette(3, 0x0F00); // Blue

    // 8 pixels at 2 BPP = 2 bytes per scanline.
    // Byte 0: pixels 0-3, Byte 1: pixels 4-7.
    // We want indices: 0, 1, 2, 3, 3, 2, 1, 0
    // Byte 0: (0 << 0) | (1 << 2) | (2 << 4) | (3 << 6) = 0x00 | 0x04 | 0x20 | 0xC0 = 0xE4
    // Byte 1: (3 << 0) | (2 << 2) | (1 << 4) | (0 << 6) = 0x03 | 0x08 | 0x10 | 0x00 = 0x1B
    uint8_t ramData[] = { 0xE4, 0x1B };

    setDmaRegisters(0, 0, 0x1000);
    enableVideoDMA();
    writePhysicalRam(0, ramData, sizeof(ramData));

    Display display(*_vidc, _hw);
    FrameInfo info = {};
    std::vector<uint32_t> pixels(8);

    ASSERT_TRUE(display.renderFrame(pixels.data(), pixels.size(), info));
    EXPECT_EQ(info.Width, 8u);
    EXPECT_EQ(info.Height, 1u);

    uint32_t black = 0xFF000000u;
    uint32_t red   = 0xFFFF0000u;
    uint32_t green = 0xFF00FF00u;
    uint32_t blue  = 0xFF0000FFu;

    EXPECT_EQ(pixels[0], black);
    EXPECT_EQ(pixels[1], red);
    EXPECT_EQ(pixels[2], green);
    EXPECT_EQ(pixels[3], blue);
    EXPECT_EQ(pixels[4], blue);
    EXPECT_EQ(pixels[5], green);
    EXPECT_EQ(pixels[6], red);
    EXPECT_EQ(pixels[7], black);
}

// Test 4: 4 BPP rendering — 4x1 display.
TEST_F(DisplayTest, Render4Bpp)
{
    configureVideoMode(4, 1, 2); // 4 BPP

    // Set palette entries.
    setPalette(0, 0x0000);  // Black
    setPalette(5, 0x000F);  // Red
    setPalette(10, 0x00F0); // Green
    setPalette(15, 0x0FFF); // White

    // 4 pixels at 4 BPP = 2 bytes.
    // Byte 0: low nibble = pixel 0 (index 0), high nibble = pixel 1 (index 5)  => 0x50
    // Byte 1: low nibble = pixel 2 (index 10), high nibble = pixel 3 (index 15) => 0xFA
    uint8_t ramData[] = { 0x50, 0xFA };

    setDmaRegisters(0, 0, 0x1000);
    enableVideoDMA();
    writePhysicalRam(0, ramData, sizeof(ramData));

    Display display(*_vidc, _hw);
    FrameInfo info = {};
    std::vector<uint32_t> pixels(4);

    ASSERT_TRUE(display.renderFrame(pixels.data(), pixels.size(), info));
    EXPECT_EQ(info.Width, 4u);
    EXPECT_EQ(info.Height, 1u);

    EXPECT_EQ(pixels[0], 0xFF000000u); // Black
    EXPECT_EQ(pixels[1], 0xFFFF0000u); // Red
    EXPECT_EQ(pixels[2], 0xFF00FF00u); // Green
    EXPECT_EQ(pixels[3], 0xFFFFFFFFu); // White
}

// Test 5: 8 BPP with green channel override.
TEST_F(DisplayTest, Render8BppGreenOverride)
{
    configureVideoMode(4, 1, 3); // 8 BPP

    // Set palette entry 0 to a known colour: R=0xF, G=0x0, B=0x0 (red).
    // 13-bit: 0x000F (R=F, G=0, B=0, no supremacy).
    setPalette(0, 0x000F);

    // 4 pixels at 8 BPP = 4 bytes.
    // Pixel 0: 0x00 => palette[0] with green override 0x0 => colour stays 0x000F
    // Pixel 1: 0x50 => palette[0] with green override 0x5 => colour becomes 0x005F (R=F, G=5, B=0)
    // Pixel 2: 0xA0 => palette[0] with green override 0xA => colour becomes 0x00AF (R=F, G=A, B=0)
    // Pixel 3: 0xF0 => palette[0] with green override 0xF => colour becomes 0x00FF (R=F, G=F, B=0)
    uint8_t ramData[] = { 0x00, 0x50, 0xA0, 0xF0 };

    setDmaRegisters(0, 0, 0x1000);
    enableVideoDMA();
    writePhysicalRam(0, ramData, sizeof(ramData));

    Display display(*_vidc, _hw);
    FrameInfo info = {};
    std::vector<uint32_t> pixels(4);

    ASSERT_TRUE(display.renderFrame(pixels.data(), pixels.size(), info));
    EXPECT_EQ(info.Width, 4u);
    EXPECT_EQ(info.Height, 1u);

    // Pixel 0: R=0xF->0xFF, G=0x0->0x00, B=0x0->0x00 => 0xFFFF0000
    EXPECT_EQ(pixels[0], 0xFFFF0000u);

    // Pixel 1: R=0xF->0xFF, G=0x5->0x55, B=0x0->0x00 => 0xFFFF5500
    EXPECT_EQ(pixels[1], 0xFFFF5500u);

    // Pixel 2: R=0xF->0xFF, G=0xA->0xAA, B=0x0->0x00 => 0xFFFFAA00
    EXPECT_EQ(pixels[2], 0xFFFFAA00u);

    // Pixel 3: R=0xF->0xFF, G=0xF->0xFF, B=0x0->0x00 => 0xFFFFFF00
    EXPECT_EQ(pixels[3], 0xFFFFFF00u);
}

// Test 6: renderFrame returns false when video DMA is disabled.
TEST_F(DisplayTest, RenderFailsWhenDMADisabled)
{
    configureVideoMode(16, 2, 0); // 1 BPP

    setPalette(0, 0x0000);
    setPalette(1, 0x0FFF);

    setDmaRegisters(0, 0, 0x1000);
    // Do NOT call enableVideoDMA().

    Display display(*_vidc, _hw);
    FrameInfo info = {};
    std::vector<uint32_t> pixels(32);

    EXPECT_FALSE(display.renderFrame(pixels.data(), pixels.size(), info));
}

// Test 7: renderFrame returns false with zero dimensions.
TEST_F(DisplayTest, RenderFailsWithZeroDimensions)
{
    // Do NOT configure timing registers, so dimensions are zero.
    enableVideoDMA();

    Display display(*_vidc, _hw);
    FrameInfo info = {};
    std::vector<uint32_t> pixels(1);

    EXPECT_FALSE(display.renderFrame(pixels.data(), pixels.size(), info));
}

// Test 8: Solid colour fill — all palette entries same colour.
TEST_F(DisplayTest, SolidColourFill)
{
    configureVideoMode(8, 4, 0); // 1 BPP, 8x4

    // Set both palette entries to the same colour (green).
    uint16_t green13 = 0x00F0;
    setPalette(0, green13);
    setPalette(1, green13);

    // Fill with any pattern — all pixels should be green.
    uint8_t ramData[4]; // 8 pixels/line * 1bpp = 1 byte/line, 4 lines = 4 bytes
    std::memset(ramData, 0x55, sizeof(ramData));

    setDmaRegisters(0, 0, 0x1000);
    enableVideoDMA();
    writePhysicalRam(0, ramData, sizeof(ramData));

    Display display(*_vidc, _hw);
    FrameInfo info = {};
    std::vector<uint32_t> pixels(8 * 4);

    ASSERT_TRUE(display.renderFrame(pixels.data(), pixels.size(), info));
    EXPECT_EQ(info.Width, 8u);
    EXPECT_EQ(info.Height, 4u);

    uint32_t expectedGreen = Display::vidc13ToARGB32(green13);

    for (size_t i = 0; i < pixels.size(); ++i)
    {
        EXPECT_EQ(pixels[i], expectedGreen) << "Pixel " << i;
    }
}

// Test 9: DMA wrap at Vend boundary.
TEST_F(DisplayTest, DMAWrapAtVend)
{
    configureVideoMode(8, 2, 0); // 1 BPP, 8x2

    // Set palette: index 0 = black, index 1 = white.
    setPalette(0, 0x0000);
    setPalette(1, 0x0FFF);

    // 8 pixels at 1 BPP = 1 byte per scanline, 2 bytes total needed.
    // Set up a DMA region that forces a wrap mid-frame.
    // Vinit = 0x100, Vstart = 0x100, Vend = 0x104 (only 4 bytes before wrap).
    // Since we need 2 bytes and the region has 4 bytes, we can fit both
    // scanlines. But let's make it wrap: Vend = 0x104, which is only 4 bytes
    // from Vinit = 0x100. So the first line reads from 0x100, second from 0x101.
    // Actually for a proper wrap test: set the region to be exactly 1 byte.
    // Vinit = 0x100, Vstart = 0x100, Vend = 0x104 (word-aligned, 4 bytes).
    // But we need at least the region to hold our data.

    // Better approach: use Vend just after the first scanline so it wraps
    // for the second scanline.
    // Vinit = 0x100 (start reading here).
    // Vstart = 0x200 (wrap target).
    // Vend = 0x104 (wrap after 4 bytes, i.e. after 1 byte of our 1-byte scanline).
    // Wait — Vend must be > Vstart for our wrap logic. Let me reconsider.
    // Actually, on real hardware: DMA starts at Vinit, and wraps Vend->Vstart.
    // Vstart < Vend always. The DMA region is [Vstart, Vend).
    // Vinit is the initial DMA address (usually = Vstart).
    //
    // For our test: place different data at two locations.
    // Vinit = 0x100, Vstart = 0x100, Vend = 0x104.
    // Region is 4 bytes [0x100, 0x104). We need 2 bytes (2 scanlines of 1 byte each).
    // First scanline reads byte at 0x100. DMA advances to 0x101.
    // Second scanline reads byte at 0x101. DMA advances to 0x102.
    // No wrap occurs because 0x102 < 0x104.
    //
    // For wrap: Vinit = 0x100, Vstart = 0x100, Vend = 0x104.
    // We need the DMA to hit 0x104.
    // 8x4 at 1BPP = 4 bytes, so 4 scanlines of 1 byte each.
    // After reading bytes at 0x100, 0x101, 0x102, 0x103, DMA = 0x104 = Vend => wraps to 0x100.
    // Use an 8x5 display so the 5th line wraps.
    configureVideoMode(8, 5, 0); // 1 BPP, 8x5

    // Place distinct patterns at RAM offset 0x100..0x103:
    // Line 0 (0x100): 0x00 = all black
    // Line 1 (0x101): 0xFF = all white
    // Line 2 (0x102): 0xAA = alternating
    // Line 3 (0x103): 0x55 = alternating (inverse)
    // After line 3, DMA = 0x104 = Vend, wraps to 0x100.
    // Line 4 (0x100 again): 0x00 = all black (same as line 0)
    uint8_t ramData[] = { 0x00, 0xFF, 0xAA, 0x55 };

    setDmaRegisters(0x100, 0x100, 0x104);
    enableVideoDMA();
    writePhysicalRam(0x100, ramData, sizeof(ramData));

    Display display(*_vidc, _hw);
    FrameInfo info = {};
    std::vector<uint32_t> pixels(8 * 5);

    ASSERT_TRUE(display.renderFrame(pixels.data(), pixels.size(), info));
    EXPECT_EQ(info.Width, 8u);
    EXPECT_EQ(info.Height, 5u);

    uint32_t black = 0xFF000000u;
    uint32_t white = 0xFFFFFFFFu;

    // Line 0: all black (0x00).
    for (int i = 0; i < 8; ++i)
    {
        EXPECT_EQ(pixels[0 * 8 + i], black) << "Line 0, pixel " << i;
    }

    // Line 1: all white (0xFF).
    for (int i = 0; i < 8; ++i)
    {
        EXPECT_EQ(pixels[1 * 8 + i], white) << "Line 1, pixel " << i;
    }

    // Line 4 (wrapped): should be identical to line 0 (all black).
    for (int i = 0; i < 8; ++i)
    {
        EXPECT_EQ(pixels[4 * 8 + i], black) << "Line 4 (wrapped), pixel " << i;
    }
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

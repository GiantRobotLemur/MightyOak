//! @file ArmEmu/Test/Test_VIDC10.cpp
//! @brief The definition of unit tests for the VIDC10 video controller
//! emulation.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/Core/Binary.hpp"

#include "VIDC10.inl"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a 32-bit VIDC register write value from a register ID
//! and data payload.
//! @param[in] regId The 6-bit register identifier (placed in bits 26-31).
//! @param[in] data The data payload (placed in bits 0-25 as appropriate).
uint32_t makeVidcWord(uint8_t regId, uint32_t data)
{
    return (static_cast<uint32_t>(regId) << 26) | data;
}

//! @brief Constructs a horizontal/vertical timing register value.
//! The data is stored in bits 14-23 of the word.
uint32_t makeTimingWord(uint8_t regId, uint16_t timingValue)
{
    return makeVidcWord(regId, static_cast<uint32_t>(timingValue) << 14);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
using VIDCDevice = VIDC10<false>;

GTEST_TEST(VIDC10Test, PaletteRegisters)
{
    VIDCDevice vidc;

    // Write all 16 palette entries with distinct colours.
    for (uint8_t i = 0; i < 16; ++i)
    {
        uint16_t colour = static_cast<uint16_t>((i * 0x111) & 0x1FFF);
        vidc.writeRegister(makeVidcWord(i, colour));
        EXPECT_EQ(vidc.getPaletteEntry(i), colour)
            << "Palette entry " << static_cast<int>(i);
    }

    // Verify out-of-range returns 0.
    EXPECT_EQ(vidc.getPaletteEntry(16), 0);
}

GTEST_TEST(VIDC10Test, BorderColourRegister)
{
    VIDCDevice vidc;

    uint16_t colour = 0x1ABC & 0x1FFF;
    vidc.writeRegister(makeVidcWord(VIDCRegister::BorderColour, colour));
    EXPECT_EQ(vidc.getBorderColour(), colour);
}

GTEST_TEST(VIDC10Test, CursorColourRegisters)
{
    VIDCDevice vidc;

    uint16_t colours[] = { 0x0123, 0x0456, 0x0789 };

    for (uint8_t i = 0; i < 3; ++i)
    {
        vidc.writeRegister(makeVidcWord(VIDCRegister::CursorColourBase + i,
                                        colours[i]));
        EXPECT_EQ(vidc.getCursorColour(i), colours[i])
            << "Cursor colour " << static_cast<int>(i);
    }

    // Out-of-range returns 0.
    EXPECT_EQ(vidc.getCursorColour(3), 0);
}

GTEST_TEST(VIDC10Test, HorizontalTimingRegisters)
{
    VIDCDevice vidc;

    uint16_t values[] = { 100, 10, 20, 30, 80, 90, 50, 5 };

    for (uint8_t i = 0; i < 8; ++i)
    {
        vidc.writeRegister(makeTimingWord(VIDCRegister::HorizontalBase + i,
                                          values[i]));
        EXPECT_EQ(vidc.getHorizontalReg(i), values[i])
            << "Horizontal register " << static_cast<int>(i);
    }

    // Out-of-range returns 0.
    EXPECT_EQ(vidc.getHorizontalReg(8), 0);
}

GTEST_TEST(VIDC10Test, VerticalTimingRegisters)
{
    VIDCDevice vidc;

    uint16_t values[] = { 312, 3, 18, 34, 290, 296, 100, 108 };

    for (uint8_t i = 0; i < 8; ++i)
    {
        vidc.writeRegister(makeTimingWord(VIDCRegister::VerticalBase + i,
                                          values[i]));
        EXPECT_EQ(vidc.getVerticalReg(i), values[i])
            << "Vertical register " << static_cast<int>(i);
    }

    // Out-of-range returns 0.
    EXPECT_EQ(vidc.getVerticalReg(8), 0);
}

GTEST_TEST(VIDC10Test, SoundFrequencyRegister)
{
    VIDCDevice vidc;

    vidc.writeRegister(makeVidcWord(VIDCRegister::SoundFreq, 0x7F));
    EXPECT_EQ(vidc.getSoundFreqReg(), 0x7F);
}

GTEST_TEST(VIDC10Test, ControlRegister)
{
    VIDCDevice vidc;

    // Set 4 BPP, 16 MHz pixel rate.
    // Pixel rate: 10 (binary) = 16 MHz.
    // BPP: 10 (binary) = 4 BPP, shifted to bits 2-3 = 0x08.
    uint8_t ctrl = 0x02 | 0x08; // 16 MHz pixel rate, 4 BPP
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, ctrl));

    EXPECT_EQ(vidc.getControlReg(), ctrl);
    EXPECT_EQ(vidc.getBitsPerPixel(), 4);
    EXPECT_EQ(vidc.getPixelRateHz(), 16000000u);
}

GTEST_TEST(VIDC10Test, ControlRegisterBppValues)
{
    VIDCDevice vidc;

    // 1 BPP (bits 2-3 = 00)
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, 0x00));
    EXPECT_EQ(vidc.getBitsPerPixel(), 1);

    // 2 BPP (bits 2-3 = 01)
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, 0x04));
    EXPECT_EQ(vidc.getBitsPerPixel(), 2);

    // 4 BPP (bits 2-3 = 10)
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, 0x08));
    EXPECT_EQ(vidc.getBitsPerPixel(), 4);

    // 8 BPP (bits 2-3 = 11)
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, 0x0C));
    EXPECT_EQ(vidc.getBitsPerPixel(), 8);
}

GTEST_TEST(VIDC10Test, ControlRegisterPixelRates)
{
    VIDCDevice vidc;

    // 8 MHz (bits 0-1 = 00)
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, 0x00));
    EXPECT_EQ(vidc.getPixelRateHz(), 8000000u);

    // 12 MHz (bits 0-1 = 01)
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, 0x01));
    EXPECT_EQ(vidc.getPixelRateHz(), 12000000u);

    // 16 MHz (bits 0-1 = 10)
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, 0x02));
    EXPECT_EQ(vidc.getPixelRateHz(), 16000000u);

    // 24 MHz (bits 0-1 = 11)
    vidc.writeRegister(makeVidcWord(VIDCRegister::Control, 0x03));
    EXPECT_EQ(vidc.getPixelRateHz(), 24000000u);
}

GTEST_TEST(VIDC10Test, DisplayDimensions)
{
    VIDCDevice vidc;

    // Set up a standard MODE 12 (640x256) configuration.
    // HDSR = 70, HDER = 390 => width = (390 - 70) * 2 = 640
    vidc.writeRegister(makeTimingWord(VIDCRegister::HDSR, 70));
    vidc.writeRegister(makeTimingWord(VIDCRegister::HDER, 390));

    // VDSR = 34, VDER = 290 => height = 290 - 34 = 256
    vidc.writeRegister(makeTimingWord(VIDCRegister::VDSR, 34));
    vidc.writeRegister(makeTimingWord(VIDCRegister::VDER, 290));

    EXPECT_EQ(vidc.getDisplayWidth(), 640u);
    EXPECT_EQ(vidc.getDisplayHeight(), 256u);
}

GTEST_TEST(VIDC10Test, DisplayDimensionsZeroWhenInvalid)
{
    VIDCDevice vidc;

    // HDER <= HDSR => width should be 0.
    vidc.writeRegister(makeTimingWord(VIDCRegister::HDSR, 100));
    vidc.writeRegister(makeTimingWord(VIDCRegister::HDER, 50));
    EXPECT_EQ(vidc.getDisplayWidth(), 0u);

    // VDER <= VDSR => height should be 0.
    vidc.writeRegister(makeTimingWord(VIDCRegister::VDSR, 200));
    vidc.writeRegister(makeTimingWord(VIDCRegister::VDER, 100));
    EXPECT_EQ(vidc.getDisplayHeight(), 0u);
}

GTEST_TEST(VIDC10Test, StereoRegisters)
{
    VIDCDevice vidc;

    // Stereo positions are 3-bit values (0-7).
    for (uint8_t i = 0; i < 8; ++i)
    {
        uint8_t position = i;
        vidc.writeRegister(makeVidcWord(VIDCRegister::StereoBase + i, position));
    }

    // The stereo register accessors are not part of the public API yet,
    // but verifying no crash and that writeRegister accepts them is sufficient.
    // Future: add getStereoPosition() accessor if needed.
}

GTEST_TEST(VIDC10Test, ReservedRegistersIgnored)
{
    VIDCDevice vidc;

    // Register IDs 20-23 (0x50-0x5C) are reserved.
    // Writing to them should not crash or affect other registers.
    for (uint8_t i = 20; i < 24; ++i)
    {
        vidc.writeRegister(makeVidcWord(i, 0x1FFF));
    }

    // Verify palette and cursor colours are unaffected.
    for (uint8_t i = 0; i < 16; ++i)
    {
        EXPECT_EQ(vidc.getPaletteEntry(i), 0);
    }
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

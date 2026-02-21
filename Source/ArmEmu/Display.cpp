//! @file ArmEmu/Display.cpp
//! @brief The definition of an object which renders the emulated display
//! framebuffer to an ARGB32 pixel buffer.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/Display.hpp"
#include "ArmEmu/VIDC10.hpp"
#include "MemcHardware.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Display Member Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief Constructs a Display renderer referencing the given VIDC and MEMC.
//! @param[in] vidc A const reference to the VIDC10 video controller.
//! @param[in] memc A const reference to the MEMC hardware.
Display::Display(const VIDC10 &vidc, const MemcHardware &memc) :
    _vidc(vidc),
    _memc(memc)
{
}

//! @brief Converts a 13-bit VIDC physical colour to ARGB8888 format.
//! @details VIDC 13-bit colour layout:
//! - Bits [3:0] = Red (4-bit)
//! - Bits [7:4] = Green (4-bit)
//! - Bits [11:8] = Blue (4-bit)
//! - Bit [12] = Supremacy (0 = opaque, 1 = transparent)
//! Each 4-bit channel is expanded to 8-bit by nibble replication.
uint32_t Display::vidc13ToARGB32(uint16_t vidc13)
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

size_t Display::getMaxBufferSize() const
{
    uint32_t width = _vidc.getDisplayWidth();
    uint32_t height = _vidc.getDisplayHeight();

    return static_cast<size_t>(width) * height;
}

bool Display::renderFrame(uint32_t *outputPixels, size_t outputSize,
                          FrameInfo &info) const
{
    // Check that video DMA is enabled.
    if (!_memc.isVideoDMAEnabled())
    {
        return false;
    }

    // Read display dimensions from VIDC.
    uint32_t width = _vidc.getDisplayWidth();
    uint32_t height = _vidc.getDisplayHeight();

    if (width == 0 || height == 0)
    {
        return false;
    }

    size_t totalPixels = static_cast<size_t>(width) * height;

    if (outputSize < totalPixels)
    {
        return false;
    }

    // Build the ARGB32 palette LUT from VIDC palette entries.
    uint32_t palette[16];

    for (uint8_t i = 0; i < 16; ++i)
    {
        palette[i] = vidc13ToARGB32(_vidc.getPaletteEntry(i));
    }

    // Get BPP from the VIDC control register.
    uint8_t bpp = _vidc.getBitsPerPixel();

    // Calculate bytes per scanline based on BPP.
    uint32_t bitsPerLine = width * bpp;
    uint32_t bytesPerLine = (bitsPerLine + 7) / 8;

    // Read DMA addresses and RAM pointer.
    uint32_t vInit = _vidc.getVideoInitAddr();
    uint32_t vStart = _vidc.getVideoStartAddr();
    uint32_t vEnd = _vidc.getVideoEndAddr();
    const uint8_t *ram = _memc.getRamData();
    uint32_t ramSize = _memc.getRamSize();

    // The DMA address starts at Vinit. At the end of each frame it wraps
    // from Vend back to Vstart.
    uint32_t dmaAddr = vInit;
    uint32_t *destPixel = outputPixels;

    for (uint32_t y = 0; y < height; ++y)
    {
        // Render one scanline, handling potential wrap at Vend boundary.
        uint32_t bytesRemaining = bytesPerLine;
        uint32_t lineOffset = 0;

        while (bytesRemaining > 0)
        {
            // Calculate how many bytes we can read before hitting Vend.
            uint32_t bytesBeforeWrap;

            if (vEnd > vStart && dmaAddr < vEnd)
            {
                bytesBeforeWrap = vEnd - dmaAddr;
            }
            else
            {
                bytesBeforeWrap = bytesRemaining;
            }

            uint32_t chunk = (bytesRemaining < bytesBeforeWrap) ?
                              bytesRemaining : bytesBeforeWrap;

            // Clamp to RAM size to prevent out-of-bounds access.
            uint32_t srcOffset = dmaAddr % ramSize;
            const uint8_t *src = ram + srcOffset;

            // Calculate pixel count for this chunk.
            uint32_t pixelsInChunk;

            switch (bpp)
            {
            case 1: pixelsInChunk = chunk * 8; break;
            case 2: pixelsInChunk = chunk * 4; break;
            case 4: pixelsInChunk = chunk * 2; break;
            case 8: pixelsInChunk = chunk; break;
            default: pixelsInChunk = 0; break;
            }

            // Clamp to remaining pixels on the line.
            uint32_t pixelsOnLine = width - lineOffset;

            if (pixelsInChunk > pixelsOnLine)
            {
                pixelsInChunk = pixelsOnLine;
            }

            // Render the chunk through the appropriate BPP renderer.
            switch (bpp)
            {
            case 1: renderScanline1Bpp(src, destPixel + lineOffset, pixelsInChunk, palette); break;
            case 2: renderScanline2Bpp(src, destPixel + lineOffset, pixelsInChunk, palette); break;
            case 4: renderScanline4Bpp(src, destPixel + lineOffset, pixelsInChunk, palette); break;
            case 8: renderScanline8Bpp(src, destPixel + lineOffset, pixelsInChunk, palette); break;
            default: break;
            }

            lineOffset += pixelsInChunk;
            dmaAddr += chunk;
            bytesRemaining -= chunk;

            // Wrap DMA address at Vend back to Vstart.
            if (vEnd > vStart && dmaAddr >= vEnd)
            {
                dmaAddr = vStart;
            }
        }

        destPixel += width;
    }

    // Fill in the frame info.
    info.Width = width;
    info.Height = height;
    info.PixelCount = static_cast<uint32_t>(totalPixels);

    return true;
}

//! @brief Renders a scanline in 1 BPP mode (8 pixels per byte, LSB first).
void Display::renderScanline1Bpp(const uint8_t *src, uint32_t *dest,
                                  uint32_t pixelCount,
                                  const uint32_t *palette) const
{
    uint32_t pixel = 0;

    while (pixel < pixelCount)
    {
        uint8_t byte = *src++;

        for (uint8_t bit = 0; bit < 8 && pixel < pixelCount; ++bit, ++pixel)
        {
            dest[pixel] = palette[(byte >> bit) & 1];
        }
    }
}

//! @brief Renders a scanline in 2 BPP mode (4 pixels per byte, LSB first).
void Display::renderScanline2Bpp(const uint8_t *src, uint32_t *dest,
                                  uint32_t pixelCount,
                                  const uint32_t *palette) const
{
    uint32_t pixel = 0;

    while (pixel < pixelCount)
    {
        uint8_t byte = *src++;

        for (uint8_t shift = 0; shift < 8 && pixel < pixelCount; shift += 2, ++pixel)
        {
            dest[pixel] = palette[(byte >> shift) & 0x03];
        }
    }
}

//! @brief Renders a scanline in 4 BPP mode (2 pixels per byte, LSB first).
void Display::renderScanline4Bpp(const uint8_t *src, uint32_t *dest,
                                  uint32_t pixelCount,
                                  const uint32_t *palette) const
{
    uint32_t pixel = 0;

    while (pixel < pixelCount)
    {
        uint8_t byte = *src++;

        // Low nibble = first pixel.
        if (pixel < pixelCount)
        {
            dest[pixel++] = palette[byte & 0x0F];
        }

        // High nibble = second pixel.
        if (pixel < pixelCount)
        {
            dest[pixel++] = palette[(byte >> 4) & 0x0F];
        }
    }
}

//! @brief Renders a scanline in 8 BPP mode (1 pixel per byte).
//! @details In 8 BPP mode, the pixel byte is split:
//! - Bits [3:0] select one of 16 palette entries to get a 13-bit base colour.
//! - Bits [7:4] replace bits [7:4] of the 13-bit colour (green channel nybble).
//! This allows 256 colours from a 16-entry hardware palette.
void Display::renderScanline8Bpp(const uint8_t *src, uint32_t *dest,
                                  uint32_t pixelCount,
                                  const uint32_t * /*palette*/) const
{
    for (uint32_t pixel = 0; pixel < pixelCount; ++pixel)
    {
        uint8_t byte = src[pixel];

        // Get the base 13-bit colour from the palette using the low nibble.
        uint8_t paletteIndex = byte & 0x0F;
        uint16_t colour13 = _vidc.getPaletteEntry(paletteIndex);

        // Replace bits [7:4] (green channel) with the pixel's upper nibble.
        uint8_t greenOverride = (byte >> 4) & 0x0F;
        colour13 = (colour13 & ~static_cast<uint16_t>(0x00F0)) |
                   (static_cast<uint16_t>(greenOverride) << 4);

        dest[pixel] = vidc13ToARGB32(colour13);
    }
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

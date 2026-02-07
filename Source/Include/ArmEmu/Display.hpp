//! @file ArmEmu/Display.hpp
//! @brief The declaration of a class which renders the emulated display
//! framebuffer to an ARGB32 pixel buffer.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_DISPLAY_HPP__
#define __ARM_EMU_DISPLAY_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <cstdint>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Describes the result of rendering a single frame.
struct FrameInfo
{
    //! @brief The width of the rendered frame in pixels.
    uint32_t Width;

    //! @brief The height of the rendered frame in pixels.
    uint32_t Height;

    //! @brief The total number of pixels rendered (Width * Height).
    uint32_t PixelCount;
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class VIDC10;
class MemcHardware;

//! @brief An object which renders the emulated display framebuffer to an
//! ARGB32 pixel buffer by reading VIDC10 palette/timing state and MEMC
//! video DMA RAM.
class Display
{
public:
    // Construction/Destruction
    Display(const VIDC10 &vidc, const MemcHardware &memc);
    ~Display() = default;

    // Static Operations
    //! @brief Converts a 13-bit VIDC physical colour to ARGB8888 format.
    //! @param[in] vidc13 The 13-bit VIDC colour value.
    //! @return The colour in 0xAARRGGBB format.
    static uint32_t vidc13ToARGB32(uint16_t vidc13);

    // Accessors
    //! @brief Gets the maximum output buffer size required in pixels.
    size_t getMaxBufferSize() const;

    // Operations
    //! @brief Renders the current video frame to a caller-provided ARGB32 buffer.
    //! @param[out] outputPixels The buffer to receive ARGB32 pixel data.
    //! @param[in] outputSize The size of the output buffer in pixels.
    //! @param[out] info Receives the frame dimensions and pixel count.
    //! @retval true A frame was rendered successfully.
    //! @retval false Rendering was skipped (DMA disabled, zero dimensions, or
    //! buffer too small).
    bool renderFrame(uint32_t *outputPixels, size_t outputSize,
                     FrameInfo &info) const;

private:
    // Internal Functions
    void renderScanline1Bpp(const uint8_t *src, uint32_t *dest,
                            uint32_t pixelCount,
                            const uint32_t *palette) const;
    void renderScanline2Bpp(const uint8_t *src, uint32_t *dest,
                            uint32_t pixelCount,
                            const uint32_t *palette) const;
    void renderScanline4Bpp(const uint8_t *src, uint32_t *dest,
                            uint32_t pixelCount,
                            const uint32_t *palette) const;
    void renderScanline8Bpp(const uint8_t *src, uint32_t *dest,
                            uint32_t pixelCount,
                            const uint32_t *palette) const;

    // Internal Fields
    const VIDC10 &_vidc;
    const MemcHardware &_memc;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

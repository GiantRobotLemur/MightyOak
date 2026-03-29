//! @file ArmEmu/IVideoFrameProvider.hpp
//! @brief The declaration of an abstract interface to a device which provides
//! video frame captured from an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_IVIDEO_FRAME_PROVIDER_HPP__
#define __ARM_EMU_IVIDEO_FRAME_PROVIDER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AddressMap.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Expresses pixel formats supported by the VIDC10 and VIDC20.
enum class AcornPixelFormat : uint8_t
{
    //! @brief 2-colour palette, 8 pixels per byte.
    Palettised1Bpp,

    //! @brief 4-colour palette, 4 pixels per byte.
    Palettised2Bpp,

    //! @brief 16-colour palette, 2 pixels per byte.
    Palettised4Bpp,

    //! @brief 256-colour palette with 1 pixel per byte.
    Palettised8Bpp,

    //! @brief 32K colours RGB, 16-bits per pixel.
    XRGB1555,

    //! @brief 64K colours RGB, 16-bits per pixel.
    RGB565,

    //! @brief 16M colours RGB, 32-bits per pixel.
    XRGB8888,
};

//! @brief Describes the geometry and format of the display data produced
//! by getRawFrame().
struct RawFrameInfo
{
    //! @brief The width of the display in pixels.
    uint16_t DisplayWidth;

    //! @brief The height of the display in scan lines.
    uint16_t DisplayHeight;

    //! @brief The number of raw bytes per scan line.
    uint16_t BytesPerRow;

    //! @brief The horizontal offset into the display of the left edge of the
    //! cursor image, in pixels.
    uint16_t CursorLeft;

    //! @brief The vertical offset into the display of the top edge of the
    //! cursor image, in scan lines.
    uint16_t CursorTop;

    //! @brief The width of the cursor image in pixels.
    uint16_t CursorWidth;

    //! @brief The height of the cursor image in scan lines.
    uint16_t CursorHeight;

    //! @brief The width of the border left of the display in pixels.
    uint16_t LeftBorderWidth;

    //! @brief The height of the border above the display in scan lines.
    uint16_t TopBorderHeight;

    //! @brief The width of the border right of the display in pixels.
    uint16_t RightBorderWidth;

    //! @brief The height of the border below the display in scan lines.
    uint16_t BottomBorderHeight;

    //! @brief The bits per pixel of the display mode (1, 2, 4, or 8).
    AcornPixelFormat DisplayFormat;
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An abstract interface to a device which provides video frame captured
//! from an emulated system.
class IVideoFrameProvider : public IMMIOBlock
{
public:
    // Construction/Destruction
    virtual ~IVideoFrameProvider() = default;

    // Accessors

    // Overrides
    //! @brief Copies the raw indexed frame buffer data and palette suitable
    //! for GPU-accelerated palette lookup.
    //! @param[out] frameBuffer Receives the raw frame buffer bytes with DMA
    //! address wrapping resolved into a contiguous buffer.
    //! @param[in] frameBufferSize The size of the output buffer in bytes.
    //! @param[out] palette Receives a 256-entry ARGB8888 palette. For
    //! 1/2/4 BPP modes, only the first 2/4/16 entries are meaningful.
    //! For 8 BPP, all 256 entries are pre-expanded to account for the
    //! VIDC10 green channel override.
    //! @param[out] info Receives the frame dimensions and format.
    //! @retval true A frame was captured successfully.
    //! @retval false No frame is available (DMA disabled, zero dimensions,
    //! buffer too small, or hardware does not support display).
    virtual bool getRawFrame(uint8_t *frameBuffer, size_t frameBufferSize,
                             uint32_t palette[256],
                             RawFrameInfo &info) const =0;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

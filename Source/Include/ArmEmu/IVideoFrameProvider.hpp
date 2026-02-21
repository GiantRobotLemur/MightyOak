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
//! @brief Describes the raw frame buffer data produced by getRawFrame().
struct RawFrameInfo
{
    //! @brief The width of the display in pixels.
    uint32_t Width;

    //! @brief The height of the display in scan lines.
    uint32_t Height;

    //! @brief The number of raw bytes per scan line.
    uint32_t BytesPerRow;

    //! @brief The border colour in ARGB8888 format.
    uint32_t BorderColour;

    //! @brief The bits per pixel of the display mode (1, 2, 4, or 8).
    uint8_t BitsPerPixel;
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

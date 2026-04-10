//! @file ArmEmu/VideoFrameSampler.hpp
//! @brief The declaration of an object which manages sampling of video frames
//! from the emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_VIDEO_FRAME_SAMPLER_HPP__
#define __ARM_EMU_VIDEO_FRAME_SAMPLER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/IVideoFrameProvider.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which manages sampling of video frames from the
//! emulated system.
class VideoFrameSampler
{
public:
    // Construction/Destruction
    VideoFrameSampler();
    ~VideoFrameSampler() = default;

    // Accessors
    int16_t getScanLineIndex(uint64_t masterTicks) const;
    uint32_t getCurrentFrameIndex() const;
    FrameSample &getCurrentFrame();
    const FrameSample &getCurrentFrame() const;
    const FrameSample &getIndexedFrame(uint32_t frame) const;

    // Operations
    uint32_t onVSyncStart(uint64_t currentTickCount, uint64_t ticksPerLine,
                          uint16_t firstDisplayScanLine);
    void onVSyncEnd(const IVideoFrameProvider *frameProvider);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    std::vector<FrameSample> _frames;
    uint64_t _lastVSyncTime;
    uint64_t _masterTicksPerScanLine;
    int16_t _firstDisplayScanLine;
    uint8_t _currentFrameIndex;
    uint8_t _lastFrameIndex;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

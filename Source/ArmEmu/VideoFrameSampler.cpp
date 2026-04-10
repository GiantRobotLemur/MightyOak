//! @file ArmEmu/VideoFrameSampler.cpp
//! @brief The definition of an object which manages sampling of video frames
//! from the emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "VideoFrameSampler.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
static constexpr uint8_t FrameCountPow2 = 2;
static constexpr uint8_t FrameCount = static_cast<uint8_t>(1) << FrameCountPow2;
static constexpr uint8_t FrameIndexMask = static_cast<uint8_t>((1u << FrameCountPow2) - 1);

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// VideoFrameSampler Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which manages sampling of video frame from an
//! emulated video controller device.
VideoFrameSampler::VideoFrameSampler() :
    _lastVSyncTime(0),
    _masterTicksPerScanLine(1),
    _firstDisplayScanLine(0),
    _currentFrameIndex(0),
    _lastFrameIndex(0)
{
    _frames.resize(FrameCount);
}

//! @brief Gets the current scan line being theoretically sampled, based on time.
//! @param[in] masterTicks The current tick count from the master clock.
//! @return The index of the current scan line being sampled relative to the
//! first scan line of the display, after any top border.
int16_t VideoFrameSampler::getScanLineIndex(uint64_t masterTicks) const
{
    if (masterTicks < _lastVSyncTime)
        return 0;

    static constexpr int64_t MaxScanLine = static_cast<int64_t>(INT16_MAX);
    int64_t scanLineIndex = static_cast<int64_t>((masterTicks - _lastVSyncTime) / _masterTicksPerScanLine);
    scanLineIndex -= _firstDisplayScanLine;

    return static_cast<int16_t>(std::min(scanLineIndex, MaxScanLine));
}

//! @brief Gets the 0-based index of the frame currently being sampled.
uint32_t VideoFrameSampler::getCurrentFrameIndex() const
{
    return _currentFrameIndex;
}

//! @brief Gets a writable reference to the frame currently being sampled.
FrameSample &VideoFrameSampler::getCurrentFrame()
{
    return _frames.at(_currentFrameIndex);
}

//! @brief Gets a read-only reference to the frame currently being sampled.
const FrameSample &VideoFrameSampler::getCurrentFrame() const
{
    return _frames.at(_currentFrameIndex);
}

//! @brief Gets a read-only reference to a frame.
//! @param[in] frame The 0-based index of the frame to obtain.
//! @return The frame at the index.
//! @remarks The index will be truncated according to the maximum number
//! available.
const FrameSample &VideoFrameSampler::getIndexedFrame(uint32_t frame) const
{
    static constexpr auto Mask = (1u << FrameCountPow2) - 1;

    return _frames.at(frame & Mask);
}

//! @brief Resets timing to keep track of the next video frame at the beginning
//! the VSync period and starts sampling a new frame.
//! @param[in] currentTickCount The current master clock tick count.
//! @param[in] ticksPerLine The count of master clock ticks per scan line.
//! @param[in] firstDisplayScanLine The count of scan lines from VSync to the first
//! line of frame sampled from display memory.
//! @returns The index of the frame which has just finished being sampled.
uint32_t VideoFrameSampler::onVSyncStart(uint64_t currentTickCount,
                                         uint64_t ticksPerLine,
                                         uint16_t firstDisplayScanLine)
{
    _lastVSyncTime = currentTickCount;
    _masterTicksPerScanLine = ticksPerLine;
    _firstDisplayScanLine = static_cast<int16_t>(firstDisplayScanLine);

    _lastFrameIndex = _currentFrameIndex;

    _currentFrameIndex = (_currentFrameIndex + 1) & FrameIndexMask;

    return _lastFrameIndex;
}

//! @brief Begins sampling a new frame now that the VSync period has ended.
//! @param[in] frameProvider An object from which the frame will be sampled.
void VideoFrameSampler::onVSyncEnd(const IVideoFrameProvider *frameProvider)
{
    FrameSample &frame = _frames[_currentFrameIndex];
    FrameGeometry nextFrameGeometry;

    nextFrameGeometry.initialise(frameProvider->getFrameConfiguration());

    if (_lastFrameIndex != _currentFrameIndex)
    {
        static constexpr auto Diff_Incompatible = FrameGeometry::Diff_FrameSize |
                                                  FrameGeometry::Diff_CursorSize |
                                                  FrameGeometry::Diff_Format;

        const FrameSample &previousFrame = _frames[_lastFrameIndex];
        auto diff = previousFrame.getGeometry().calculateDifferences(nextFrameGeometry);

        if ((diff & Diff_Incompatible) == 0)
        {
            // Initialise the frame based on the previous one.
            frame.initialise(previousFrame);
            return;
        }
    }

    // Initialise the frame based on the new geometry and format.
    frame.initialise(frameProvider);
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


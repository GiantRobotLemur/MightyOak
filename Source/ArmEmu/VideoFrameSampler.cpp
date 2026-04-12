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

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Copies bytes from one buffer to another, comparing the copies bytes
//! with a third buffer in the process.
//! @param[in] dest A pointer to the buffer to receive the data, should be
//! 16-byte aligned.
//! @param[in] source A point to the buffer to copy, should be
//! 16-byte aligned.
//! @param[in] reference A pointer to the buffer to compare source bytes to,
//! should be 16-byte aligned.
//! @param[in] byteCount The count of bytes to copy and compare, should be 
//! multiple of 16.
//! @retval true All bytes copied from @p source were the same as those read
//! from @p reference.
//! @note According to MEMC data sheet (page 29) "All DMA data must be
//! aligned on 16 byte boundaries." This applies to Vini, Vstart, Vend,
//! Cinit, Sstart, SendN
bool copyAndCompareDMAData(Ag::uint8_ptr_t dest, Ag::uint8_cptr_t source,
                           Ag::uint8_cptr_t reference, size_t byteCount)
{
    // TODO: Improve the performance of this routine using SIMD technology.
    size_t paragraphCount = byteCount / 16;

    if (paragraphCount == 0)
        return true;

    auto wordDest = reinterpret_cast<uint64_t *>(dest);
    auto wordSrc = reinterpret_cast<const uint64_t *>(source);
    auto wordRef = reinterpret_cast<const uint64_t *>(reference);
    size_t wordCount = paragraphCount * 2;
    bool isEqual = true;

    for (size_t i = 0; i < wordCount; ++i)
    {
        uint64_t sourceWord = wordSrc[i];
        isEqual &= (sourceWord == wordRef[i]);
        wordDest[i] = sourceWord;
    }

    return isEqual;
}

//! @brief Copies bytes from one 16-byte aligned buffer to another.
//! @param[in] dest The pointer to the buffer to receive data, must be
//! 16-byte aligned.
//! @param[in] source The pointer to the buffer of data to copy, must be
//! 16-byte aligned.
//! @param[in] byteCount The count of bytes to copy - must be a whole
//! multiple of 16.
void copyDMAData(Ag::uint8_ptr_t dest, Ag::uint8_cptr_t source, size_t byteCount)
{
    // TODO: Improve the performance of this routine using SIMD technology.
    size_t paragraphCount = byteCount / 16;

    if (paragraphCount == 0)
        return;

    auto wordDest = reinterpret_cast<uint64_t *>(dest);
    auto wordSrc = reinterpret_cast<const uint64_t *>(source);
    size_t wordCount = paragraphCount * 2;

    for (size_t i = 0; i < wordCount; ++i)
    {
        wordDest[i] = wordSrc[i];
    }
}

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

//! @brief Indicates whether the current frame has a valid geometry.
//! @retval true The current frame has a valid geometry for the border and/or
//! display, although possibly with no content.
//! @retval false The current frame has no size.
bool VideoFrameSampler::hasValidFrame() const
{
    return _frames[_currentFrameIndex].getGeometry().isValid();
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

//! @brief Captures the raw display data from the circular DMA buffer.
//! @param[in] vram A pointer to the DMA memory accessed by video hardware.
//! @param[in] initAddr The offset into @p vram of the first pixel displayed.
//! @param[in] startAddr The offset into @p vram of the beginning of the
//! circular DMA buffer, must be less than or equal to @p initAddr.
//! @param[in] endAddr The offset into @p vram of the end of the circular DMA
//! buffer, must be greater than or equal to both startAddr and initAddr.
void VideoFrameSampler::captureDisplayData(const void *vram, uint32_t initAddr,
                                           uint32_t startAddr, uint32_t endAddr)
{
    FrameSample &currentFrame = _frames[_currentFrameIndex];

    // Ensure the circular buffer has a valid definition.
    if (startAddr >= endAddr)
    {
        currentFrame.applyDifference(FrameDiff_DisplayContent);
        return;
    }
    else if (!currentFrame.getGeometry().hasDisplay())
    {
        // No display data to copy.
        return;
    }

    // Copy display pixels. If frame coherence can be exploited, compare the
    // pixels of the new frame with the old as they are copied. If not possible,
    // simply copy and mark the display pixels in the current frame as different.
    static constexpr FrameDiffBits Diff_DisplayIncompatible =
        FrameDiff_DisplayFormat | FrameDiff_DisplaySize;

    auto &target = currentFrame.getDisplayData();
    size_t dmaBufferSize = endAddr - startAddr;
    size_t dmaOffset = (initAddr >= endAddr) ? startAddr : initAddr;

    const uint8_t *ram = reinterpret_cast<const uint8_t *>(vram);
    size_t maxBytesToCopy = std::min(currentFrame.getGeometry().getDisplayDataSize(),
                                     dmaBufferSize);
    size_t bytesWritten;
    FrameDiffBits difference = FrameDiff_None;

    if (currentFrame.getDifferenceFromLastFrame() & Diff_DisplayIncompatible)
    {
        // Simply assume the display data is different and copy it.
        difference = FrameDiff_DisplayContent;

        for (bytesWritten = 0; bytesWritten < maxBytesToCopy; )
        {
            // Calculate the amount of contiguous data we can copy.
            size_t blockSize = std::min(endAddr - dmaOffset,
                                        maxBytesToCopy - bytesWritten);

            // Transfer the block.
            copyDMAData(target.data() + bytesWritten,
                        ram + dmaOffset, blockSize);

            // Move on to the second part of the buffer.
            dmaOffset = startAddr;
            bytesWritten += blockSize;
        }
    }
    else
    {
        // Copy display data and compare with the previous frame to exploit
        // frame coherence.
        FrameSample &lastFrame = _frames[_lastFrameIndex];
        Ag::uint8_cptr_t refData = lastFrame.getDisplayData().data();
        bool isSame = true;

        for (bytesWritten = 0; bytesWritten < maxBytesToCopy; )
        {
            // Calculate the amount of contiguous data we can copy.
            size_t blockSize = std::min(endAddr - dmaOffset,
                                        maxBytesToCopy - bytesWritten);

            if (isSame)
            {
                // Transfer the data and compare it with the previous frame.
                isSame = copyAndCompareDMAData(target.data() + bytesWritten,
                                               ram + dmaOffset,
                                               refData + dmaOffset,
                                               blockSize);
            }
            else
            {
                // Transfer the block.
                copyDMAData(target.data() + bytesWritten,
                            ram + dmaOffset, blockSize);
            }

            // Move on to the second part of the buffer.
            dmaOffset = startAddr;
            bytesWritten += blockSize;
        }

        difference = isSame ? FrameDiff_None : FrameDiff_DisplayContent;
    }

    // Note differences from the last frame, if any.
    currentFrame.applyDifference(difference);

    // Mark that the sample includes display data.
    currentFrame.addState(FrameState_HasDisplay);
}

//! @brief Captures raw cursor overlay data from the DMA buffer.
//! @param[in] vram A pointer to the DMA-addressable RAM to read from.
//! @param[in] initAddr The offset into @p vram to the first pixel of
//! cursor data.
void VideoFrameSampler::captureCursorData(const void *vram, uint32_t initAddr)
{
    FrameSample &currentFrame = _frames[_currentFrameIndex];

    if (currentFrame.getGeometry().hasCursor() == false)
    {
        // No display data to copy.
        return;
    }

    // Copy cursor pixels. If frame coherence can be exploited, compare the
    // pixels of the new cursor with the old as they are copied. If not, simply
    // copy and mark the cursor pixels in the current frame as different.
    static constexpr FrameDiffBits Diff_CursorIncompatible = FrameDiff_CursorSize;
    auto &target = currentFrame.getCursorData();
    const uint8_t *ram = reinterpret_cast<const uint8_t *>(vram);
    size_t blockSize = currentFrame.getGeometry().getCursorDataSize();
    FrameDiffBits difference = FrameDiff_None;

    if (currentFrame.getDifferenceFromLastFrame() & Diff_CursorIncompatible)
    {
        // Simply assume the display data is different and copy it.
        difference = FrameDiff_CursorContent;

        // Transfer the block.
        copyDMAData(target.data(), ram + initAddr, blockSize);
    }
    else
    {
        // Copy display data and compare with the previous frame to exploit
        // frame coherence.
        FrameSample &lastFrame = _frames[_lastFrameIndex];
        Ag::uint8_cptr_t refData = lastFrame.getCursorData().data();

        // Transfer the data and compare it with the previous frame.
        difference = copyAndCompareDMAData(target.data(),
                                            ram + initAddr,
                                            refData, blockSize) ?
            FrameDiff_None : FrameDiff_CursorContent;
    }

    // Note differences from the last frame, if any.
    currentFrame.applyDifference(difference);

    // Mark that the sample includes cursor data.
    currentFrame.addState(FrameState_HasCursor);
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
        static constexpr auto Diff_Incompatible = FrameDiff_FrameSize |
                                                  FrameDiff_CursorSize |
                                                  FrameDiff_DisplayFormat;

        const FrameSample &previousFrame = _frames[_lastFrameIndex];
        auto diff = previousFrame.getGeometry().calculateDifferences(nextFrameGeometry);

        if ((diff & Diff_Incompatible) == 0)
        {
            // Initialise the frame based on the previous one and the current
            // state giving mutable information, like cursor position.
            frame.initialise(previousFrame, nextFrameGeometry);
            return;
        }
    }

    // Initialise the frame based on the new geometry and format.
    frame.initialise(frameProvider);
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


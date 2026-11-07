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
#include <Ag/Core.hpp>

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

    //! @brief 256-colour palette with 1 pixel per byte, with a VIDC10-compatible
    //! 16-colour palette which combines pixel values with palette entries.
    Palettised8BppVIDC10,

    //! @brief 32K colours RGB, 16-bits per pixel.
    XRGB1555,

    //! @brief 64K colours RGB, 16-bits per pixel.
    RGB565,

    //! @brief 16M colours RGB, 32-bits per pixel.
    XRGB8888,
};

//! @brief An enumeration which defines a bitfield describing the difference
//! between two captured video frames.
enum FrameDiff : uint16_t
{
    FrameDiff_None              = 0x000,
    FrameDiff_DisplaySize       = 0x001,
    FrameDiff_BorderSize        = 0x002,
    FrameDiff_BorderPosition    = 0x004,
    FrameDiff_FrameSize         = 0x007,
    FrameDiff_CursorSize        = 0x008,
    FrameDiff_CursorPosition    = 0x010,
    FrameDiff_DisplayFormat     = 0x020,
    FrameDiff_DisplayContent    = 0x040,
    FrameDiff_CursorContent     = 0x080,
    FrameDiff_DisplayPalette    = 0x100,
    FrameDiff_BorderPalette     = 0x200,
    FrameDiff_CurorPalette      = 0x400,
    FrameDiff_All               = 0x7FF,
};

//! @brief An alias of the scalar type used to combine FrameDiff_* values as
//! a bitfield.
using FrameDiffBits = std::underlying_type_t<FrameDiff>;

//! @brief Expresses the recording state of a FrameSample.
enum FrameState : uint8_t
{
    FrameState_NoFrame      = 0x00,
    FrameState_HasDisplay   = 0x01,
    FrameState_HasCursor    = 0x02,
};

//! @brief An alias of the scalar type used to combine FrameState_* values as
//! a bitfield.
using FrameStateBits = std::underlying_type_t<FrameState>;

//! @brief An object which provides metadata for an AcornPixelFormat value.
class AcornPixelFormatValue : public Ag::EnumSymbol<AcornPixelFormat>
{
public:
    // Construction
    AcornPixelFormatValue(AcornPixelFormat id);

    //! @brief Constructs an object representing a symbol in an enumeration
    //! class.
    //! @param[in] id The binary value of the symbol.
    //! @param[in] symbol The internal symbol definition as text.
    //! @param[in] displayName The symbol as text to be displayed to the user.
    //! @param[in] description A description of the meaning of the symbol
    //! which can be displayed to the user.
    //! @note All strings should be static and UTF-8 encoded.
    AcornPixelFormatValue(AcornPixelFormat id, const char *symbol,
                          const char *displayName, const char *description,
                          uint8_t bpp, uint16_t paletteSize = 0);

    // Accessors
    bool isPalettised() const;
    bool isHybridPalette() const;
    uint8_t getBitsPerPixel() const;
    uint16_t getPaletteSize() const;
private:
    // Internal Fields
    uint16_t _paletteSize;
    uint8_t _bpp;
};

using AcornPixelFormatEnumInfo = Ag::EnumInfo<AcornPixelFormat, AcornPixelFormatValue>;

const AcornPixelFormatEnumInfo &getAcornPixelFormatInfo();
uint8_t getAcornFormatBpp(AcornPixelFormat format);
uint16_t calculateFramePitch(AcornPixelFormat format, uint16_t width);

//! @brief Defines a colour value in a fixed format compatible with all Acorn
//! VIDC formats.
struct CanonicalColour
{
    // Public Constants
    static constexpr uint8_t BitsPerComponent = 8;
    static constexpr uint8_t RedShift = 16;
    static constexpr uint32_t RedMask = 0xFF0000;
    static constexpr uint8_t GreenShift = 8;
    static constexpr uint32_t GreenMask = 0x00FF00;
    static constexpr uint8_t BlueShift = 0;
    static constexpr uint32_t BlueMask = 0x0000FF;
    static constexpr uint8_t AlphaShift = 24;
    static constexpr uint32_t AlphaMask = 0xFF000000;

    static constexpr uint8_t VIDC10SuppressionBits = 1;
    static constexpr uint8_t VIDC20SuppressionBits = 4;

    // Public Fields

    //! @brief The colour value defined as BGRA, LSB to MSB.
    uint32_t RawValue;

    // Accessors
    //! @brief Gets the 8-bit red component of the colour value.
    constexpr uint8_t getRed() const
    {
        return Ag::Bin::extractBits<uint8_t, RedShift, BitsPerComponent>(RawValue);
    }

    //! @brief Gets the 8-bit green component of the colour value.
    constexpr uint8_t getGreen() const
    {
        return Ag::Bin::extractBits<uint8_t, GreenShift, BitsPerComponent>(RawValue);
    }

    //! @brief Gets the 8-bit blue component of the colour value.
    constexpr uint8_t getBlue() const
    {
        return Ag::Bin::extractBits<uint8_t, BlueShift, BitsPerComponent>(RawValue);
    }

    //! @brief Tests two canonical colour values for equality.
    //! @param[in] rhs The value to compare against.
    //! @retval true If the raw colour values are identical.
    //! @retval false If there is a single difference between raw colour values.
    constexpr bool operator==(const CanonicalColour &rhs) const noexcept
    {
        return RawValue == rhs.RawValue;
    }

    //! @brief Tests two canonical colour values for inequality.
    //! @param[in] rhs The value to compare against.
    //! @retval true If there is a single difference between raw colour values.
    //! @retval false If the raw colour values are identical.
    constexpr bool operator!=(const CanonicalColour &rhs) const noexcept
    {
        return RawValue != rhs.RawValue;
    }

    // Public Constants
    static const CanonicalColour Transparent;
    static const CanonicalColour Black;
    static const CanonicalColour White;

    // Construction/Destruction
    CanonicalColour();
    CanonicalColour(uint32_t rawValue);
    ~CanonicalColour() = default;

    static CanonicalColour fromVIDC10FramePalette(uint16_t paletteEntry);
    static CanonicalColour fromVIDC10FramePalette8Bit(uint16_t paletteEntry);
    static CanonicalColour fromVIDC10CursorPalette(uint16_t paletteEntry, uint8_t index);
    static CanonicalColour fromVIDC20Palette(uint32_t paletteEntry);
    static CanonicalColour fromVIDC20CursorPalette(uint32_t paletteEntry, uint8_t index);

    // Operations
    CanonicalColour combineVIDC10LogicalColour(uint8_t logicalColour) const;
};

//! @brief A structure which stores the timing metrics used to configure a
//! display frame.
struct FrameMetrics
{
    //! @brief The first scan line of the display pixels, measured in scan lines
    //! from the beginning of VSync.
    uint16_t VerticalDisplayStart = 0;

    //! @brief The first scan line after the last display row, measured in scan
    //! lines from the beginning of VSync.
    uint16_t VerticalDisplayEnd = 0;

    //! @brief The first border scan line, measured in scan lines from the
    //! beginning of VSync.
    uint16_t VerticalBorderStart = 0;

    //! @brief The offset of the scan line after the last border row, measured
    //! in scan lines from the beginning of VSync.
    uint16_t VerticalBorderEnd = 0;

    //! @brief The first cursor scan line, measured in scan lines from the
    //! beginning of VSync.
    uint16_t VerticalCursorStart = 0;

    //! @brief The offset of the scan line after the last cursor row, measured
    //! in scan lines from the beginning of VSync.
    uint16_t VerticalCursorEnd = 0;

    //! @brief The count of pixels scanned from the start of HSync to the
    //! left edge of the display pixels.
    uint16_t HorizontalDisplayStart = 0;

    //! @brief The count of pixels scanned from the start of HSync to the
    //! pixel after the last one displayed.
    uint16_t HorizontalDisplayEnd = 0;

    //! @brief The count of pixels scanned from the start of HSync to the
    //! left edge of the left border.
    uint16_t HorizontalBorderStart = 0;

    //! @brief The count of pixels scanned from the start of HSync to the
    //! right edge of the right border.
    uint16_t HorizontalBorderEnd = 0;

    //! @brief The count of pixels scanned from the start of HSync to the
    //! first pixel of the cursor overlay.
    uint16_t HorizontalCursorStart = 0;

    //! @brief The width of the cursor, in pixels.
    //! @remarks This value is generally fixed.
    uint16_t HorizontalCursorSize = 0;

    //! @brief The format of display pixels.
    AcornPixelFormat DisplayFormat = AcornPixelFormat::Palettised1Bpp;
};

//! @brief Describes the geometry and format of the display produced by
//! IVideoFrameProvider.
//! @par Geometry
//! The vertical size of a display frame is measured in scan lines, for some of
//! the time taken to trace those scan lines, no display is being output because
//! the electron gun (of an old CRT monitor) is being re-targeted to the top left
//! of the screen in that time.
//! @par
//! Vertical measurements of a frame begin with a VSync pulse. Then border rows
//! are traced, read from the border palette, then display rows, read from display
//! memory, then the bottom border, then possibly nothing to the end of the vertical
//! frame. The cursor could be output instead of display or border pixels at any time
//! within the frame.
//! @par
//! Horizontal measurements of a frame begin with an HSync pulse. Then border pixels
//! are traced, read from the border palette, then display pixels, read from display
//! memory, then right border pixels, then possibly nothing to the start of the next
//! HSync pulse. The cursor pixels could be output anywhere in the frame, but are
//! limited to a fixed width (32 pixels for both VIDC10 and VIDC20).
class FrameGeometry
{
private:
    // Internal Fields
    //! @brief The width of the display in pixels.
    uint16_t _displayWidth = 0;

    //! @brief The height of the display in scan lines.
    uint16_t _displayHeight = 0;

    //! @brief The number of raw bytes per scan line.
    uint16_t _bytesPerDisplayRow = 0;

    //! @brief The horizontal offset into the display of the left edge of the
    //! cursor image, in pixels.
    int16_t _cursorLeft = 0;

    //! @brief The vertical offset into the display of the top edge of the
    //! cursor image, in scan lines.
    int16_t _cursorTop = 0;

    //! @brief The width of the cursor image in pixels.
    uint16_t _cursorWidth = 0;

    //! @brief The height of the cursor image in scan lines.
    uint16_t _cursorHeight = 0;

    //! @brief The number of raw bytes per cursor scan line.
    uint16_t _bytesPerCursorRow = 0;

    //! @brief The horizontal origin of the border relative to the left edge of
    //! the display in pixels.
    int16_t _borderLeft = 0;

    //! @brief The vertical origin of the border relative to the top edge of
    //! the display, in scan lines.
    int16_t _borderTop = 0;

    //! @brief The horizontal extent of the border relative to its origin.
    uint16_t _borderWidth = 0;

    //! @brief The vertical extent of the border relative to its origin.
    uint16_t _borderHeight = 0;

    //! @brief The format of the display pixels.
    AcornPixelFormat _displayFormat = AcornPixelFormat::Palettised1Bpp;

public:
    // Construction/Destruction
    FrameGeometry() = default;
    FrameGeometry(const FrameMetrics &metrics);
    ~FrameGeometry() = default;

    // Accessors
    bool isValid() const;
    bool hasCursor() const;
    bool hasBorder() const;
    bool hasDisplay() const;

    //! @brief The width of the display in pixels.
    constexpr uint16_t getDisplayWidth() const noexcept { return _displayWidth; }

    //! @brief The height of the display in scan lines.
    constexpr uint16_t getDisplayHeight() const noexcept { return _displayHeight; }

    //! @brief The number of raw bytes per scan line.
    constexpr uint16_t getBytesPerRow() const noexcept { return _bytesPerDisplayRow; }

    //! @brief Gets the count of bytes required to store a frame of display data.
    constexpr size_t getDisplayDataSize() const noexcept
    {
        return static_cast<size_t>(_bytesPerDisplayRow) * _displayHeight;
    }

    //! @brief The horizontal offset into the display of the left edge of the
    //! cursor image, in pixels.
    constexpr int16_t getCursorLeft() const noexcept { return _cursorLeft; }

    //! @brief The vertical offset into the display of the top edge of the
    //! cursor image, in scan lines.
    constexpr int16_t getCursorTop() const noexcept { return _cursorTop; }

    //! @brief The width of the cursor image in pixels.
    constexpr uint16_t getCursorWidth() const noexcept { return _cursorWidth; }

    //! @brief The height of the cursor image in scan lines.
    constexpr uint16_t getCursorHeight() const noexcept { return _cursorHeight; }

    //! @brief Gets the count of bytes required to store a frame of cursor data.
    constexpr size_t getCursorDataSize() const noexcept
    {
        return static_cast<size_t>(_bytesPerCursorRow) * _cursorHeight;
    }

    //! @brief Gets the left offset of the border from the display area.
    constexpr int16_t getBorderLeft() const noexcept { return _borderLeft; }

    //! @brief Gets the top offset of the border from the display area.
    constexpr int16_t getBorderTop() const noexcept { return _borderTop; }

    //! @brief The extent of the border relative to its horizontal origin.
    constexpr uint16_t getBorderWidth() const noexcept { return _borderWidth; }

    //! @brief The extent of the border relative to its vertical origin.
    constexpr uint16_t getBorderHeight() const noexcept { return _borderHeight; }

    //! @brief The display pixel format.
    constexpr AcornPixelFormat getDisplayFormat() const noexcept { return _displayFormat; }

    // Operations
    bool initialise(const FrameMetrics &metrics);
    FrameDiffBits calculateDifferences(const FrameGeometry &rhs) const;
    void invalidate();
private:
    // Internal Functions
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines a change to one or more palette entries mid-frame.
struct PaletteChange
{
    //! @brief The index of the scan line at which the palette entry
    //! was changed relative to the display, i.e. top border rows have
    //! negative indices.
    int16_t ScanLineIndex;

    //! @brief The offset of the first colour definition in the palette data.
    uint16_t SourceIndex;

    //! @brief The index of the first colour changed.
    uint8_t FirstEntry;

    //! @brief The count of colours changed.
    uint8_t Count;

    // Construction/Destruction
    PaletteChange();
    PaletteChange(int16_t lineIndex, uint16_t sourceIndex,
                  uint8_t paletteEntryIndex);
    ~PaletteChange() = default;

    // Operations
    bool tryCombine(const PaletteChange &next);
};

class IVideoFrameProvider;

//! @brief An object representing display data sampled at VSync which captures
//! changes to colour definitions mid-frame.
class FrameSample
{
public:
    // Public Types
    using PaletteEntries = std::vector<CanonicalColour>;
    using PaletteChanges = std::vector<PaletteChange>;
    using ScanLineIndices = std::vector<int16_t>;

    // Construction/Destruction
    FrameSample();
    ~FrameSample() = default;

    // Accessors
    bool hasFrame() const;
    bool hasCursor() const;
    bool hasDisplay() const;
    bool hasDisplayPaletteChanges() const;
    bool hasCursorPaletteChanges() const;
    bool hasBorderPaletteChanges() const;
    FrameDiffBits getDifferenceFromLastFrame() const;
    void applyDifference(FrameDiffBits difference);

    const FrameGeometry &getGeometry() const;
    bool isCompatible(const FrameGeometry &rhs) const;

    DMABlock &getDisplayData();
    const DMABlock &getDisplayData() const;
    const PaletteEntries &getDisplayPalette() const;
    const PaletteChanges &getDisplayColourChanges() const;

    const PaletteEntries &getBorderPalette() const;
    const ScanLineIndices &getBorderColourChanges() const;

    DMABlock &getCursorData();
    const DMABlock &getCursorData() const;
    const PaletteEntries &getCursorPalette() const;
    const PaletteChanges &getCursorColourChanges() const;

    // Operations
    void invalidateGeometry();
    void addState(FrameStateBits bits);
    bool initialise(const IVideoFrameProvider *provider);
    bool initialise(const FrameSample &previousFrame, const FrameGeometry &compatibleGeometry);
    void addDisplayPaletteChange(int16_t scanLine, uint8_t entryIndex, const CanonicalColour &definition);
    void addCursorPaletteChange(int16_t scanLine, uint8_t entryIndex, const CanonicalColour &definition);
    void addBorderPaletteChange(int16_t scanLine, const CanonicalColour &definition);
private:
    // Internal Functions
    static bool applyPaletteChanges(size_t basePaletteSize,
                                    PaletteEntries &target,
                                    const PaletteEntries &source,
                                    const PaletteChanges &changes);

    // Internal Fields
    DMABlock _displayData;
    DMABlock _cursorData;
    PaletteEntries _displayPalette;
    PaletteChanges _displayColourChanges;
    PaletteEntries _borderPalette;
    ScanLineIndices _borderColourChanges;
    PaletteEntries _cursorPalette;
    PaletteChanges _cursorColourChanges;
    FrameGeometry _geometry;
    FrameDiffBits _diff;
    FrameStateBits _state;
};

//! @brief An abstract interface to a device which provides video frame captured
//! from an emulated system.
class IVideoFrameProvider : public IMMIOBlock
{
public:
    // Construction/Destruction
    virtual ~IVideoFrameProvider() = default;

    // Overrides

    //! @brief Captures the current state of the display palette.
    //! @param[out] definitions An array to receive the current palette entries.
    //! @param[in] count The count of elements in @p definitions.
    //! @return The count of palette entries written.
    virtual uint32_t captureDisplayPalette(CanonicalColour *definitions, uint32_t count) const =0;

    //! @brief Captures the current state of the cursor palette.
    //! @param[out] definitions An array to receive the current palette entries.
    //! @param[in] count The count of elements in @p definitions.
    //! @return The count of palette entries written.
    virtual uint32_t captureCursorPalette(CanonicalColour *definitions, uint32_t count) const = 0;

    //! @brief Captures the current border colour definition.
    virtual CanonicalColour captureBorderColour() const = 0;

    //! @brief Gets the current configuration for the display frame.
    //! @return A read-only reference to the current configuration.
    virtual const FrameMetrics &getFrameConfiguration() const = 0;

    //! @brief Gets a video frame sampled from the device.
    //! @param[in] id The identifier of the cached frame to sample.
    //! @return A pointer to a sampled frame, or nullptr if @p id was invalid.
    virtual const FrameSample *getSampledFrame(uint32_t id) const = 0;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/IVideoFrameProvider.cpp
//! @brief The definition of code to support an abstract interface to a device
//! which provides video frame captured from an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <Ag/Core.hpp>

#include "ArmEmu/IVideoFrameProvider.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// AcornPixelFormatValue Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a symbol to be used solely as a search key.
//! @param[in] id The binary value of the symbol.
AcornPixelFormatValue::AcornPixelFormatValue(AcornPixelFormat id) :
    EnumSymbol<AcornPixelFormat>(id),
    _paletteSize(0),
    _bpp(0)
{
}

//! @brief Constructs an object representing a symbol in an enumeration
//! class.
//! @param[in] id The binary value of the symbol.
//! @param[in] symbol The internal symbol definition as text.
//! @param[in] displayName The symbol as text to be displayed to the user.
//! @param[in] description A description of the meaning of the symbol
//! which can be displayed to the user.
//! @param[in] bpp The count of bits per pixel.
//! @param[in] paletteSize The count of entries in a palette required to
//! support display modes using that format.
//! @note All strings should be static and UTF-8 encoded.
AcornPixelFormatValue::AcornPixelFormatValue(AcornPixelFormat id, const char *symbol,
                                             const char *displayName, const char *description,
                                             uint8_t bpp, uint16_t paletteSize /*= 0*/) :
    EnumSymbol<AcornPixelFormat>(id, symbol, displayName, description),
    _paletteSize(paletteSize),
    _bpp(bpp)
{
}

//! @brief Determines if the display format uses a palette.
bool AcornPixelFormatValue::isPalettised() const
{
    return _paletteSize > 0;
}

//! @brief Determines if the display format combines palette entries with
//! colour data embedded in pixels.
//! @retval true There is physical colour data in each pixel.
//! @retval false Each pixel is an index to a palette entry, or the format
//! does not require a palette.
bool AcornPixelFormatValue::isHybridPalette() const
{
    auto maxPaletteSize = static_cast<uint16_t>(1) << _bpp;

    return (_paletteSize > 0) && (maxPaletteSize > _paletteSize);
}

//! @brief Gets the count of bits per pixel, whether they are used or not.
uint8_t AcornPixelFormatValue::getBitsPerPixel() const
{
    return _bpp;
}

//! @brief Gets the number of entries in a palette required to support the
//! display format, 0 for an non-palette format.
uint16_t AcornPixelFormatValue::getPaletteSize() const
{
    return _paletteSize;
}

////////////////////////////////////////////////////////////////////////////////
// CanonicalColour Member Definitions
////////////////////////////////////////////////////////////////////////////////
const CanonicalColour CanonicalColour::Transparent(0);
const CanonicalColour CanonicalColour::Black(CanonicalColour::AlphaMask);
const CanonicalColour CanonicalColour::White(CanonicalColour::AlphaMask |
                                             CanonicalColour::RedMask |
                                             CanonicalColour::GreenMask |
                                             CanonicalColour::BlueMask);


//! @brief Constructs an empty colour value.
CanonicalColour::CanonicalColour() :
    RawValue(0)
{
}

//! @brief Constructs a colour value from a raw 32-bit word in the proper encoding.
//! @param[in] rawValue The already encoded colour value.
CanonicalColour::CanonicalColour(uint32_t rawValue) :
    RawValue(rawValue)
{
}

//! @brief Creates a colour value from a VIDC10-compatible display or border
//! palette entry.
//! @param[in] paletteEntry The palette entry to interpret.
//! @return A newly created colour value.
//! @remarks The suppression bit is used to define an 8-bit value, 0 = 0, 1 = 255.
CanonicalColour CanonicalColour::fromVIDC10FramePalette(uint16_t paletteEntry)
{
    // NOTE: 0 * 17 = 0
    //      15 * 17 = 255
    // So scale 4-bit values by 17.
    uint32_t r = (paletteEntry & 0x0F) * 17u;
    uint32_t g = ((paletteEntry >> 4) & 0x0F) * 17u;
    uint32_t b = ((paletteEntry >> 8) & 0x0F) * 17u;
    uint32_t a = (paletteEntry & 0x1000) ? 0x00 : AlphaMask;

    return CanonicalColour(a | (r << RedShift) |
                           (g << GreenShift) | b);
}

//! @brief Creates a colour value from a VIDC10-compatible palette entry used in
//! a 256-colour mode.
//! @param[in] paletteEntry The palette entry to interpret.
//! @return A newly created colour value which must be combined with the logical
//! colour data to create a valid colour value.
//! @remarks The suppression bit is used to define an 8-bit value, 0 = 0, 1 = 255.
CanonicalColour CanonicalColour::fromVIDC10FramePalette8Bit(uint16_t paletteEntry)
{
    uint32_t r = (paletteEntry & 0x07);
    uint32_t g = ((paletteEntry >> 4) & 0x03);
    uint32_t b = ((paletteEntry >> 8) & 0x07);
    uint32_t a = (paletteEntry & 0x1000) ? 0x00 : AlphaMask;

    return CanonicalColour(a | (r << RedShift) |
                           (g << GreenShift) | b);
}

//! @brief Creates a canonical colour from a VIDC10-compatible cursor colour.
//! @param[in] paletteEntry
//! @param[in] index The palette entry index.
//! @return An newly created colour value, possibly transparent.
CanonicalColour CanonicalColour::fromVIDC10CursorPalette(uint16_t paletteEntry, uint8_t index)
{
    // Index 0 is always transparent.
    if (index == 0)
        return Transparent;

    // NOTE: 0 * 17 = 0
    //      15 * 17 = 255
    // So scale 4-bit values by 17.
    uint32_t r = (paletteEntry & 0x0F) * 17u;
    uint32_t g = ((paletteEntry >> 4) & 0x0F) * 17u;
    uint32_t b = ((paletteEntry >> 8) & 0x0F) * 17u;

    // Create an opaque colour.
    return CanonicalColour((r << RedShift) |
                           (g << GreenShift) | b |
                           AlphaMask);
}

//! @brief Creates a colour value from a VIDC20-compatible palette entry.
//! @param[in] paletteEntry The palette entry to interpret.
//! @return A newly created colour value.
CanonicalColour CanonicalColour::fromVIDC20Palette(uint32_t paletteEntry)
{
    // Extract the 8-bit colour channel values.
    uint32_t r = (paletteEntry & 0xFF) << RedShift;
    uint32_t g = ((paletteEntry >> 8) & 0xFF) << GreenShift;
    uint32_t b = ((paletteEntry >> 16) & 0xFF) << BlueShift;

    // Extract the 4-bit suppression value and scale it to become an
    // 8-bit alpha transparency channel (0 = opaque, 15 = transparent).
    uint32_t a = (15 - (((paletteEntry >> 24) & 0x0F)) * 17) << AlphaShift;

    return CanonicalColour(r | g | b | a);
}

//! @brief Creates a canonical colour from a VIDC20-compatible cursor colour.
//! @param[in] paletteEntry
//! @param[in] index The palette entry index.
//! @return An newly created colour value, possibly transparent.
CanonicalColour CanonicalColour::fromVIDC20CursorPalette(uint32_t paletteEntry, uint8_t index)
{
    // Index 0 is always transparent.
    if (index == 0)
        return Transparent;

    // Extract the 8-bit colour channel values.
    uint32_t r = (paletteEntry & 0xFF) << RedShift;
    uint32_t g = ((paletteEntry >> 8) & 0xFF) << GreenShift;
    uint32_t b = ((paletteEntry >> 16) & 0xFF) << BlueShift;

    // Extract the 4-bit suppression value and scale it to become an
    // 8-bit alpha transparency channel (0 = opaque, 15 = transparent).
    uint32_t a = (15 - (((paletteEntry >> 24) & 0x0F)) * 17) << AlphaShift;

    return CanonicalColour(r | g | b | a);
}

//! @brief Combines a colour previously created with fromVIDC10FramePalette8Bit()
//! with a logical colour value to create a valid canonical colour.
//! @param[in] logicalColour The logical colour value, only the top 4 bits will
//! be used, having used the bottom 4 bits to look up the base palette entry.
//! @return A valid canonical colour value.
CanonicalColour CanonicalColour::combineVIDC10LogicalColour(uint8_t logicalColour) const
{
    uint32_t logRed = (logicalColour & 0x10) ? 0x08 : 0x00;
    uint32_t logGreen = (logicalColour & 0x60) << 2;
    uint32_t logBlue = (logicalColour & 0x80) >> 4;

    // Combine the palette entry and logical colours bits, then scale.
    uint32_t red = (logRed | getRed()) * 17;
    uint32_t green = (logGreen | getGreen()) * 17;
    uint32_t blue = (logBlue | getBlue()) * 17;

    // Preserve the alpha channel from the palette entry.
    uint32_t alpha = RawValue & AlphaMask;

    // Combine to define a valid canonical colour.
    return CanonicalColour(red | green | blue | alpha);
}

////////////////////////////////////////////////////////////////////////////////
// FrameGeometry Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an initialises a FrameGeometry object from the timing
//! values specified in hardware registers.
//! @param[in] metrics The metrics used to calculate the frame geometry.
FrameGeometry::FrameGeometry(const FrameMetrics &metrics)
{
    initialise(metrics);
}

//! @brief Determines if the frame has valid geometry.
//! @retval true The display has a non-zero area.
//! @retval false The display has zero area.
bool FrameGeometry::isValid() const
{
    return (_displayWidth > 0) && (_displayHeight > 0);
}

//! @brief Determines whether the frame includes a cursor overlay image.
//! @retval true The cursor image geometry is valid.
//! @retval false The cursor image geometry is invalid.
bool FrameGeometry::hasCursor() const
{
    return (_cursorWidth > 0) && (_cursorHeight > 0);
}

//! @brief Determines whether the frame includes a border.
//! @retval true A non-zero border has been configured.
//! @retval false The border has zero size.
bool FrameGeometry::hasBorder() const
{
    return (_borderWidth > 0) ||
        (_borderHeight > 0);
}

//! @brief Determines whether the frame includes a valid display.
//! @retval true The display has a non-zero area.
//! @retval false The display is not configured, the border should be used to
//! render the entire screen.
bool FrameGeometry::hasDisplay() const
{
    return (_displayWidth > 0) && (_displayHeight > 0);
}

//! @brief Initialises the geometry of the frame from frame timing metrics.
//! @param[in] metrics The metrics used to calculate the frame geometry.
//! @retval true The geometry is valid.
//! @retval false The geometry is invalid.
bool FrameGeometry::initialise(const FrameMetrics &metrics)
{
    // Ensure at least a border or a display is defined with a non-zero area.
    int vBorderStart = metrics.VerticalBorderStart;
    int vBorderEnd = metrics.VerticalBorderEnd;
    int vDisplayStart = metrics.VerticalDisplayStart;
    int vDisplayEnd = metrics.VerticalDisplayEnd;

    int hBorderStart = metrics.HorizontalBorderStart;
    int hBorderEnd = metrics.HorizontalBorderEnd;
    int hDisplayStart = metrics.HorizontalDisplayStart;
    int hDisplayEnd = metrics.HorizontalDisplayEnd;

    int vBorderSize = vBorderEnd - vBorderStart;
    int vDisplaySize = vDisplayEnd - vDisplayStart;
    int hBorderSize = hBorderEnd - hBorderStart;
    int hDisplaySize = hDisplayEnd - hDisplayStart;

    if (((vBorderSize > 0) && (hBorderSize > 0)) ||
        ((vDisplaySize > 0) && (hDisplaySize > 0)))
    {
        // Simply copy the display format.
        _displayFormat = metrics.DisplayFormat;
        auto &pixelFormat = getAcornPixelFormatInfo().getSymbolById(_displayFormat);

        // NOTE: The border and display defined the visible area, they can
        // be displayed independently, i.e. border with no display, display
        // with no border, or both.
        //
        // It is assumed that the origin of the display is 0, 0 in a right-
        // handed coordinate system (X goes left, Y goes down). The border
        // is defined relative to that.

        // Calculate vertical frame geometry.
        int vCursorStart = metrics.VerticalCursorStart;
        int vCursorEnd = metrics.VerticalCursorEnd;

        // The display and border define the origin of the visible area.
        int verticalOffset = std::min(vBorderStart, vDisplayStart);

        _borderHeight = static_cast<uint16_t>(std::max(vBorderSize, 0));
        _borderTop = (_borderHeight == 0) ? 0 : static_cast<int16_t>(vBorderStart - verticalOffset);
        _displayHeight = static_cast<uint16_t>(std::max(vDisplaySize, 0));

        if (vCursorStart < vCursorEnd)
        {
            _cursorTop = static_cast<int16_t>(vCursorStart - verticalOffset);
            _cursorHeight = static_cast<uint16_t>(std::max(vCursorEnd - vCursorStart, 0));
        }
        else
        {
            // Mark the cursor as invisible.
            _cursorTop = 0;
            _cursorHeight = 0;
        }

        // Calculate horizontal frame geometry.
        int hCursorStart = metrics.HorizontalCursorStart;

        // The display and border define the origin of the visible area.
        int horzOffset = std::min(hBorderStart, hDisplayStart);

        _borderWidth = static_cast<uint16_t>(std::max(hBorderSize, 0));
        _borderLeft = (_borderWidth == 0) ? 0 : static_cast<int16_t>(hBorderStart - horzOffset);
        _displayWidth = static_cast<uint16_t>(std::max(hDisplaySize, 0));

        if (_displayWidth > 0)
        {
            _bytesPerDisplayRow = static_cast<uint16_t>((hDisplaySize * pixelFormat.getBitsPerPixel()) / 8);
        }
        else
        {
            _bytesPerDisplayRow = 0;
        }

        if ((metrics.HorizontalCursorSize > 0) && (_cursorHeight > 0))
        {
            _cursorLeft = static_cast<int16_t>(hCursorStart - horzOffset);
            _cursorWidth = metrics.HorizontalCursorSize;

            // This assumes a cursor format of 2bpp.
            _bytesPerCursorRow = _cursorWidth / 4;
        }
        else
        {
            // Further mark the cursor as invisible.
            _cursorLeft = 0;
            _cursorWidth = 0;
            _bytesPerCursorRow = 0;
        }

        return true;
    }
    else
    {
        // Reset the geometry.
        _displayWidth = 0;
        _displayHeight = 0;
        _bytesPerDisplayRow = 0;
        _cursorLeft = 0;
        _cursorTop = 0;
        _cursorWidth = 0;
        _cursorHeight = 0;
        _bytesPerCursorRow = 0;
        _borderLeft = 0;
        _borderTop = 0;
        _borderWidth = 0;
        _borderHeight = 0;
        _displayFormat = AcornPixelFormat::Palettised1Bpp;

        return false;
    }
}

//! @brief Calculates the differences between the current and another frame geometry.
//! @param[in] rhs The frame geometry to compare with the current one.
//! @return A bit field of combined Diff_* values expressing the changes, a
//! value of 0 indicates the two geometries are the same.
uint8_t FrameGeometry::calculateDifferences(const FrameGeometry &rhs) const
{
    uint8_t diff = Diff_None;

    if ((_displayWidth != rhs._displayWidth) ||
        (_displayHeight != rhs._displayHeight))
        diff |= Diff_DisplaySize;

    if ((_borderLeft != rhs._borderLeft) ||
        (_borderTop != rhs._borderTop))
        diff |= Diff_BorderPosition;

    if((_borderWidth != rhs._borderWidth) ||
       (_borderHeight != rhs._borderHeight))
        diff |= Diff_BorderSize;

    if ((_cursorWidth != rhs._cursorWidth) ||
        (_cursorHeight != rhs._cursorHeight))
        diff |= Diff_CursorSize;

    if ((_cursorLeft != rhs._cursorLeft) ||
        (_cursorTop != rhs._cursorTop))
        diff |= Diff_CursorPosition;

    if (_displayFormat != rhs._displayFormat)
        diff |= Diff_Format;

    return diff;
}

//! @brief Ensures the frame geometry is not valid for display.
void FrameGeometry::invalidate()
{
    _displayHeight = _displayWidth = 0;
}

////////////////////////////////////////////////////////////////////////////////
// PaletteChange Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates an empty palette change record.
PaletteChange::PaletteChange() :
    ScanLineIndex(0),
    SourceIndex(0),
    FirstEntry(0),
    Count(0)
{
}

//! @brief Creates a record documenting a change to a single palette entry.
//! @param[in] lineIndex The index of the scan line from which the colour changes.
//! @param[in] sourceIndex The index of the element containing the new colour.
//! @param[in] paletteEntryIndex The index of the palette entry updated.
PaletteChange::PaletteChange(int16_t lineIndex, uint16_t sourceIndex,
                             uint8_t paletteEntryIndex) :
    ScanLineIndex(lineIndex),
    SourceIndex(sourceIndex),
    FirstEntry(paletteEntryIndex),
    Count(1)
{
}

//! @brief Determines if a successive change can be combined with the current one
//! and does so, if it can.
//! @param[in] next The next palette change record to possibly combine.
//! @retval true The @p next entry was combined with the current one.
//! @retval false The @p next entry was not contiguous with the current one and
//! cannot be combined, it will have to be recorded separately.
bool PaletteChange::tryCombine(const PaletteChange &next)
{
    if ((ScanLineIndex == next.ScanLineIndex) &&
        ((SourceIndex + Count) == next.SourceIndex) &&
        ((FirstEntry + Count) == next.FirstEntry))
    {
        // The entries are sequential.
        Count += next.Count;

        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////
// FrameSample Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty sampled frame.
FrameSample::FrameSample() :
    _displayPitch(0),
    _cursorPitch(16)
{
}

//! @brief Indicates whether the sample contains a valid frame.
//! @retval true The frame contains a display.
//! @retval false The frame is empty.
bool FrameSample::hasFrame() const
{
    return _geometry.isValid();
}

//! @brief Indicates whether the frame contains more than an initial display
//! palette definition.
bool FrameSample::hasDisplayPaletteChanges() const
{
    return _displayColourChanges.empty() == false;
}

//! @brief Indicates whether the frame contains more than an initial cursor
//! palette definition.
bool FrameSample::hasCursorPaletteChanges() const
{
    return _cursorColourChanges.empty() == false;
}

//! @brief Indicates whether the frame contains more than just a basic
//! border colour.
bool FrameSample::hasBorderPaletteChanges() const
{
    return _borderColourChanges.empty() == false;
}

//! @brief Gets the geometry and format of the sampled fram.e
const FrameGeometry &FrameSample::getGeometry() const
{
    return _geometry;
}

//! @brief Determines if a frame geometry is compatible with the current
//! sample.
//! @param[in] rhs The geometry to compare with the current sample.
//! @retval true The new geometry is compatible with the current sample.
//! @retval false A critical size or format differs between the current sample
//! and @p rhs.
bool FrameSample::isCompatible(const FrameGeometry &rhs) const
{
    // If the size of the display, border or cursor has changed, or the
    // display format has changed, the samples aren't compatible.
    static constexpr auto Diff_Incompatible = FrameGeometry::Diff_FrameSize |
        FrameGeometry::Diff_CursorSize |
        FrameGeometry::Diff_Format;

    auto diff = _geometry.calculateDifferences(rhs);

    return (diff & Diff_Incompatible) == 0;
}

//! @brief Gets a reference to the captured display memory contents.
Ag::ByteBlock &FrameSample::getDisplayData()
{
    return _displayData;
}

//! @brief Gets a read-only reference to the captured display memory contents.
const Ag::ByteBlock &FrameSample::getDisplayData() const
{
    return _displayData;
}

//! @brief Gets the count of bytes between successive lines of display data.
uint16_t FrameSample::getDisplayPitch() const
{
    return _displayPitch;
}

//! @brief Gets the palette entries for the display pixels.
const FrameSample::PaletteEntries &FrameSample::getDisplayPalette() const
{
    return _displayPalette;
}

//! @brief Gets the changes made to the display palette during the frame.
const FrameSample::PaletteChanges &FrameSample::getDisplayColourChanges() const
{
    return _displayColourChanges;
}

//! @brief Gets the colours defined for the border.
const FrameSample::PaletteEntries &FrameSample::getBorderPalette() const
{
    return _borderPalette;
}

//! @brief Gets the indices of scan lines where new border colours were applied.
const FrameSample::ScanLineIndices &FrameSample::getBorderColourChanges() const
{
    return _borderColourChanges;
}

//! @brief Gets a reference to the captured cursor memory contents.
Ag::ByteBlock &FrameSample::getCursorData()
{
    return _cursorData;
}

//! @brief Gets a read-only reference to the captured cursor memory contents.
const Ag::ByteBlock &FrameSample::getCursorData() const
{
    return _cursorData;
}

//! @brief Gets the count of bytes between successive lines of cursor data.
uint16_t FrameSample::getCursorPitch() const
{
    return _cursorPitch;
}

//! @brief Gets the colours used to define the cursor overlay.
const FrameSample::PaletteEntries &FrameSample::getCursorPalette() const
{
    return _cursorPalette;
}

//! @brief Gets the changes made to the cursor palette during the frame.
const FrameSample::PaletteChanges &FrameSample::getCursorColourChanges() const
{
    return _cursorColourChanges;
}

//! @brief Marks the display sample as invalid.
void FrameSample::invalidateGeometry()
{
    _geometry.invalidate();
}

//! @brief Initialises the frame sample from the video frame provider.
//! @param[in] provider The provider defining the initial palette definitions.
//! @retval true If the sampled frame was valid.
//! @retval false If the sampled frame held no output.
bool FrameSample::initialise(const IVideoFrameProvider *provider)
{
    bool isValid = _geometry.initialise(provider->getFrameConfiguration());
    _displayColourChanges.clear();
    _borderColourChanges.clear();
    _cursorColourChanges.clear();

    if (isValid)
    {
        // Reconfigure the buffer to receive the display data.
        _displayPitch = _geometry.getBytesPerRow();

        if (_geometry.hasDisplay())
        {
            auto &format = getAcornPixelFormatInfo().getSymbolById(_geometry.getDisplayFormat());

            size_t requiredDisplaySize = _displayPitch * _geometry.getDisplayHeight();
            Ag::ensureCapacity(_displayData, requiredDisplaySize);
            _displayData.resize(requiredDisplaySize);

            if (format.isPalettised())
            {
                uint32_t paletteSize = format.getPaletteSize();
                Ag::ensureCapacity(_displayPalette, paletteSize * 2);

                // Ensure there is enough space for the initial palette.
                _displayPalette.resize(paletteSize);

                // Copy what entries we can.
                uint32_t written = provider->captureDisplayPalette(_displayPalette.data(),
                                                                   paletteSize);

                // Fill any remaining colours with transparency.
                std::fill_n(_displayPalette.data() + written,
                            paletteSize - written,
                            CanonicalColour::Transparent);
            }
            else
            {
                // The display does not use a palette.
                _displayPalette.clear();
            }
        }
        else
        {
            // No display is configured.
            _displayData.clear();
            _displayPalette.clear();
        }

        // Configure to be able to capture border information.
        _borderPalette.clear();

        if (_geometry.hasBorder())
        {
            Ag::ensureCapacity(_borderPalette, 16);
            Ag::ensureCapacity(_borderColourChanges, 16);

            _borderPalette.push_back(provider->captureBorderColour());
        }

        // Configure to be able to capture cursor information.
        if (_geometry.hasCursor())
        {
            // Reconfigure the buffer to receive the cursor data.
            _cursorPitch = calculateFramePitch(AcornPixelFormat::Palettised2Bpp,
                                               _geometry.getCursorWidth());
            size_t requiredCursorSize = _cursorPitch * _geometry.getCursorHeight();
            Ag::ensureCapacity(_cursorData, requiredCursorSize);
            _cursorData.resize(requiredCursorSize);

            // Ensure there is enough space for the initial palette.
            Ag::ensureCapacity(_cursorPalette, 16);
            _cursorPalette.resize(4, CanonicalColour::Transparent);
            Ag::ensureCapacity(_cursorColourChanges, 16);

            // Capture the initial cursor palette.
            uint32_t written = provider->captureCursorPalette(_cursorPalette.data(), 4);

            // Ensure we have a full initial palette.
            std::fill_n(_cursorPalette.data() + written,
                        4 - written,
                        CanonicalColour::Transparent);
        }
        else
        {
            _cursorData.clear();
            _cursorPalette.clear();
            _cursorPitch = 0;
        }

        return true;
    }
    else
    {
        // The frame geometry was invalid.
        _displayPalette.clear();
        _borderPalette.clear();
        _cursorPalette.clear();

        return false;
    }
}

//! @brief Initialises the sample frame from the previous frame by resolving
//! new initial palette entries and buffer sizes.
//! @param[in] previousFrame The previous frame sample.
//! @retval true If the sampled frame was valid.
//! @retval false If the sampled frame held no output.
bool FrameSample::initialise(const FrameSample &previousFrame)
{
    _geometry = previousFrame._geometry;
    _displayColourChanges.clear();
    _borderPalette.clear();
    _borderColourChanges.clear();
    _cursorPalette.clear();
    _cursorColourChanges.clear();

    if (_geometry.isValid())
    {
        // Reconfigure the buffer to receive the display data.
        _displayPitch = previousFrame._displayPitch;
        _displayData.resize(previousFrame._displayData.size());
        auto &pixelFormat = getAcornPixelFormatInfo().getSymbolById(_geometry.getDisplayFormat());

        if (pixelFormat.isPalettised())
        {
            size_t paletteSize = pixelFormat.getPaletteSize();

            // Ensure there is enough space for the required palette entries.
            Ag::ensureCapacity(_displayPalette, previousFrame._displayPalette.capacity());

            _displayPalette.resize(paletteSize);

            // Copy the palette.
            std::copy_n(previousFrame._displayPalette.data(),
                        paletteSize, _displayPalette.data());

            // Process display palette changes.
            applyPaletteChanges(paletteSize, _displayPalette,
                                previousFrame._displayPalette,
                                previousFrame._displayColourChanges);
        }
        else
        {
            // There is no palette.
            _displayPalette.clear();
        }

        // Configure to be able to capture border information.
        if (_geometry.hasBorder())
        {
            Ag::ensureCapacity(_borderPalette, previousFrame._borderPalette.capacity());
            Ag::ensureCapacity(_borderColourChanges, previousFrame._borderColourChanges.capacity());

            // Capture the last border colour from the previous frame.
            _borderPalette.push_back(previousFrame._borderPalette.back());
        }

        // Configure to be able to capture cursor information.
        if (_geometry.hasCursor())
        {
            // Reconfigure the buffer to receive the cursor data.
            _cursorPitch = previousFrame._cursorPitch;
            _cursorData.resize(previousFrame._cursorData.size());

            // Ensure there is enough space for the initial palette.
            Ag::ensureCapacity(_cursorPalette, previousFrame._cursorPalette.capacity());
            _cursorPalette.resize(4);
            std::copy_n(previousFrame._cursorPalette.data(), 4,
                        _cursorPalette.data());

            // Process cursor palette changes.
            applyPaletteChanges(4, _cursorPalette, previousFrame._cursorPalette,
                                previousFrame._cursorColourChanges);
        }
        else
        {
            _cursorPitch = 0;
        }

        return true;
    }
    else
    {
        // The geometry was invalid.
        return false;
    }
}

//! @brief Documents a change to the display palette mid-frame.
//! @param[in] scanLine The index of the scan line at which the new colour
//! was applied, negative if in the top border.
//! @param[in] entryIndex The index of the display palette entry overwritten.
//! @param[in] definition The new colour written to the display palette entry.
void FrameSample::addDisplayPaletteChange(int16_t scanLine, uint8_t entryIndex,
                                          const CanonicalColour &definition)
{
    uint16_t sourceIndex = static_cast<uint16_t>(_displayPalette.size());
    _displayPalette.push_back(definition);

    if (_displayColourChanges.empty())
    {
        _displayColourChanges.emplace_back(scanLine, sourceIndex, entryIndex);
    }
    else
    {
        PaletteChange change(scanLine, sourceIndex, entryIndex);

        if (_displayColourChanges.back().tryCombine(change) == false)
            _displayColourChanges.push_back(change);
    }
}

//! @brief Documents a change to the cursor overlay palette mid-frame.
//! @param[in] scanLine The index of the scan line at which the new colour
//! was applied, negative if in the top border.
//! @param[in] entryIndex The index of the cursor palette entry overwritten.
//! @param[in] definition The new colour written to the cursor palette entry.
void FrameSample::addCursorPaletteChange(int16_t scanLine, uint8_t entryIndex,
                                         const CanonicalColour &definition)
{
    uint16_t sourceIndex = static_cast<uint16_t>(_cursorPalette.size());
    _cursorPalette.push_back(definition);

    if (_cursorColourChanges.empty())
    {
        _cursorColourChanges.emplace_back(scanLine, sourceIndex, entryIndex);
    }
    else
    {
        PaletteChange change(scanLine, sourceIndex, entryIndex);

        if (_cursorColourChanges.back().tryCombine(change) == false)
            _cursorColourChanges.push_back(change);
    }
}

//! @brief Documents a change to the border colour mid-frame.
//! @param[in] scanLine The index of the scan line at which the new colour
//! was applied, negative if in the top border.
//! @param[in] definition The new border colour.
void FrameSample::addBorderPaletteChange(int16_t scanLine, const CanonicalColour &definition)
{
    // Not the new colour and the scan line it is effective from.
    _borderPalette.push_back(definition);
    _borderColourChanges.push_back(scanLine);
}

//! @brief Applies changes to a palette after it was initialised to create
//! a new base palette.
//! @param[in] target The pre-initialised base palette to update with changes.
//! @param[in] source The colours referred to by the changes.
//! @param[in] changes The changes made to the palette.
void FrameSample::applyPaletteChanges(size_t basePaletteSize,
                                      PaletteEntries &target,
                                      const PaletteEntries &source,
                                      const PaletteChanges &changes)
{
    // Apply each change sequentially to the target palette.
    for (const auto &change : changes)
    {
        if (change.FirstEntry < basePaletteSize)
        {
            size_t safeCount = std::min(basePaletteSize - change.FirstEntry,
                                        static_cast<size_t>(change.Count));

            std::copy_n(source.data() + change.SourceIndex, safeCount,
                        target.data() + change.FirstEntry);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets metadata associated with elements of the AcornPixelFormat enum.
const AcornPixelFormatEnumInfo &getAcornPixelFormatInfo()
{
    static const AcornPixelFormatEnumInfo metadata({
        { AcornPixelFormat::Palettised1Bpp, "Palettised1Bpp", "1 Bpp (palette)", nullptr, 1, 2 },
        { AcornPixelFormat::Palettised2Bpp, "Palettised2Bpp", "2 Bpp (palette)", nullptr, 2, 4 },
        { AcornPixelFormat::Palettised4Bpp, "Palettised4Bpp", "4 Bpp (palette)", nullptr, 4, 16 },
        { AcornPixelFormat::Palettised8Bpp, "Palettised8Bpp", "8 Bpp (palette)", nullptr, 8, 256 },
        { AcornPixelFormat::Palettised8BppVIDC10, "Palettised8BppVIDC10", "8 Bpp (hybrid palette)", nullptr, 8, 16 },
        { AcornPixelFormat::XRGB1555, "XRGB1555", "15 Bpp", nullptr, 16, 0 },
        { AcornPixelFormat::RGB565, "RGB565", "16 Bpp", nullptr, 16, 0 },
        { AcornPixelFormat::XRGB8888, "XRGB8888", "32 Bpp", nullptr, 32, 0 },
    });

    return metadata;
}

//! @brief Gets the count of bits-per-pixel for an Acorn pixel format.
//! @param[in] format The format to look-up.
//! @return The count of bits per pixel, 0 on failure.
uint8_t getAcornFormatBpp(AcornPixelFormat format)
{
    auto &metadata = getAcornPixelFormatInfo();

    return metadata.getSymbolById(format).getBitsPerPixel();
}

//! @brief Calculates the count of bytes per scan line in a frame buffer.
//! @param[in] format The format of the frame buffer.
//! @param[in] width The count of horizontal pixels in the frame buffer.
//! @return The count of bytes per scan line.
uint16_t calculateFramePitch(AcornPixelFormat format, uint16_t width)
{
    uint32_t bpp = getAcornFormatBpp(format);

    return static_cast<uint16_t>((width * bpp) / 8);
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


//! @file ArmDebugger/UI/name_here.cpp
//! @brief The definition of an object which renders a view of a block of
//! memory as assembly language data directives.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "UI/DataBlockView.hpp"

#include "Ag/Core/Format.hpp"
#include "Ag/Core/VariantTypes.hpp"

#include "AsmTools/InstructionInfo.hpp"
#include "AsmTools/Options.hpp"

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
constexpr double MinLineWidth = 384;
constexpr int MinItemsPerLine = 4;
constexpr int MinBytesPerLine = 16;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void appendHexSpecifier(std::string &buffer, uint32_t formatterOptions)
{
    if (formatterOptions & Asm::FormatterOptions::UseBasicStyleHex)
    {
        buffer.push_back('&');
    }
    else
    {
        buffer.push_back('0');
        buffer.push_back('x');
    }
}

bool isText(Asm::DirectiveDataType dataType)
{
    return (dataType >= Asm::DirectiveDataType::NativeString) &&
        (dataType <= Asm::DirectiveDataType::Utf32String);
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// DataBlockView Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
DataBlockView::DataBlockView(uint32_t baseAddress, uint32_t length,
                             const void *hostData,
                             Asm::DirectiveDataType dataType) :
    MemoryBlockView(baseAddress, length),
    _sourceData(hostData),
    _dataType(dataType)
{
    switch (_dataType)
    {
    case Asm::DirectiveDataType::Byte: _mnemonic = "EQUB"; break;
    case Asm::DirectiveDataType::HalfWord: _mnemonic = "EQUW"; break;
    case Asm::DirectiveDataType::Word: _mnemonic = "EQUD"; break;
    case Asm::DirectiveDataType::LongWord: _mnemonic = "EQUQ"; break;
    case Asm::DirectiveDataType::NativeString: _mnemonic = "EQUS"; break;
    case Asm::DirectiveDataType::Utf8String: _mnemonic = "EQUSU8"; break;
    case Asm::DirectiveDataType::Utf16String: _mnemonic = "EQUSU16"; break;
    case Asm::DirectiveDataType::Utf32String: _mnemonic = "EQUSU32"; break;
    case Asm::DirectiveDataType::Real32: _mnemonic = "EQUF"; break;
    case Asm::DirectiveDataType::Real64: _mnemonic = "EQUR"; break;
    case Asm::DirectiveDataType::Real96: _mnemonic = "EQUE"; break;
    case Asm::DirectiveDataType::Max:
    default:
        _mnemonic = "<Unknown Data type>";
        break;
    }
}

bool DataBlockView::hitTest(const BlockViewContext &context,
                            const QPointF &position, uint32_t &address,
                            uint32_t &length) const
{
    return false;
}

bool DataBlockView::tryFindAddress(const BlockViewContext &context,
                                   uint32_t address, QRectF &area) const
{
    return false;
}

void DataBlockView::draw(const BlockViewContext &context,
                         QPainter *painter,
                         const QPointF &scrollOrigin,
                         const QRectF &updateRegion) const
{
    using TokenType = Asm::FormattedInstruction::TokenType;
    uint32_t bytesPerItem = Asm::getDirectiveDataTypeInfo().getSymbolById(_dataType).getUnitSize();
    const uint32_t formatOptions = context.getInstructionFormatter()->getFlags();

    QString lineText;
    QString itemText;
    const QString separator(", ");

    double separatorLength = context.calculateTextLength(separator);

    QRectF lineRect(context.calculateLeftEdge(BlockViewContext::UpToMnemonic) - scrollOrigin.x(),
                    getVerticalOffset() - scrollOrigin.y(),
                    context.calculateLeftEdge(BlockViewContext::UpToComments) - context.calculateLeftEdge(BlockViewContext::Margin),
                    context.getLineHeight());

    uint32_t lineOffset = 0;

    for (uint32_t lineByteCount : _lineLengths)
    {
        if (updateRegion.intersects(lineRect))
        {
            QPointF textOrigin = lineRect.topLeft();
            textOrigin.ry() += context.getBaselineAdjustment();

            context.prepareToPaint(painter, TokenType::DataDirectiveMnemonic);
            painter->drawText(textOrigin, _mnemonic);

            textOrigin.rx() += context.getMnemonicWidth();

            // Draw the data items in the line.
            for (uint32_t itemOffset = 0; itemOffset < lineByteCount; itemOffset += bytesPerItem)
            {
                if (tryGetNextScalarItemText(lineOffset + itemOffset, itemText,
                                             bytesPerItem, formatOptions))
                {
                    if (itemOffset > 0)
                    {
                        context.prepareToPaint(painter, TokenType::Separator);
                        painter->drawText(textOrigin, separator);
                        textOrigin.rx() += separatorLength;
                    }

                    context.prepareToPaint(painter, TokenType::DataValue);
                    painter->drawText(textOrigin, itemText);

                    textOrigin.rx() += context.calculateTextLength(itemText);
                }
            }
        }
        else if (updateRegion.bottom() < lineRect.top())
        {
            // We have no more text to draw.
            break;
        }

        // Move down to the next line.
        lineOffset += lineByteCount;
        lineRect.translate(0.0, context.getLineHeight());
    }
}

/*

TODO: Address headers are a fixed width, should be in BlockViewContext
A raw byte/word view should be optionally visible, based on BlockViewContext settings
DataType overrides should specify number of bytes/data values per line,
  AND optionally wrap at word-break for text-based data types.
Each block should do hit testing for pixel to address based on the data
item or instruction under the cursor.


*/


QSizeF DataBlockView::calculateExtents(BlockViewContext &context)
{
    uint32_t bytesPerItem = Asm::getDirectiveDataTypeInfo().getSymbolById(_dataType).getUnitSize();
    int minItemsPerLine = (bytesPerItem > 1) ? MinItemsPerLine : MinBytesPerLine;
    const uint32_t formatOptions = context.getInstructionFormatter()->getFlags();

    context.maximiseMnemonicWidth(context.calculateTextLength(_mnemonic));
    _lineLengths.clear();

    QString lineText;
    QString itemText;
    constexpr int widthInChars = 80;
    constexpr int maxMnemonicWidth = 7;
    const double minLineWidth = context.getCodeFontMetrics().averageCharWidth() * (widthInChars - maxMnemonicWidth);
    double maxLineLength = 0;
    uint32_t offset = 0;
    uint32_t prevOffset = 0;

    if (isText(_dataType))
    {

    }
    else
    {
        uint32_t bytesLeft = offset - getLength();
        int lineItemCount = 0;

        while ((bytesPerItem <= bytesLeft) &&
                tryGetNextScalarItemText(offset, itemText, bytesPerItem, formatOptions))
        {
            if (lineText.isEmpty() == false)
            {
                lineText.append(", ");
            }

            lineText.append(itemText);

            double lineLength = context.calculateTextLength(lineText);

            if ((lineLength > minLineWidth) &&
                (lineItemCount >= minItemsPerLine))
            {
                // We've reached the end of the current line.
                _lineLengths.push_back(offset - prevOffset);
                prevOffset = offset;

                // Start a new line.
                lineItemCount = 1;
                lineText = itemText;
            }
            else
            {
                // Add to the current line.
                ++lineItemCount;
            }

            offset += bytesPerItem;
            maxLineLength = std::max(maxLineLength, lineLength);
        }

        if (offset > prevOffset)
        {
            _lineLengths.push_back(offset - prevOffset);
        }
    }

    return QSizeF(context.calculateLeftEdge(BlockViewContext::UpToOperands) + maxLineLength,
                  context.getLineHeight() * static_cast<double>(_lineLengths.size()));
}

bool DataBlockView::tryGetNextScalarItemText(uint32_t offset, QString &text,
                                             uint32_t bytesPerItem,
                                             uint32_t formatOptions) const
{
    uint32_t bytesLeft = getLength() - offset;

    if (bytesLeft < bytesPerItem)
        return 0;

    const void *itemPtr = Ag::offsetPtr<void>(_sourceData, offset);
    Ag::FormatInfo format(Ag::LocaleInfo::getNeutral());
    std::string buffer;
    uint32_t bytesUsed = bytesPerItem;

    switch (_dataType)
    {
    case Asm::DirectiveDataType::HalfWord:
        appendHexSpecifier(buffer, formatOptions);
        format.setMinimumWholeDigits(4);
        format.setRadix(16);
        buffer.reserve(8);

        Ag::appendValue(format, buffer, *reinterpret_cast<const uint16_t *>(itemPtr));
        
        break;

    case Asm::DirectiveDataType::Word:
        appendHexSpecifier(buffer, formatOptions);
        format.setMinimumWholeDigits(8);
        format.setRadix(16);
        buffer.reserve(12);

        Ag::appendValue(format, buffer, *reinterpret_cast<const uint32_t *>(itemPtr));
        break;

    case Asm::DirectiveDataType::LongWord:
        appendHexSpecifier(buffer, formatOptions);
        format.setMinimumWholeDigits(16);
        format.setRadix(16);
        buffer.reserve(20);

        Ag::appendValue(format, buffer, *reinterpret_cast<const uint64_t *>(itemPtr));
        break;

    case Asm::DirectiveDataType::Real32:
        text.setNum(*reinterpret_cast<const float *>(itemPtr), 'g');
        break;

    case Asm::DirectiveDataType::Real64:
        text.setNum(*reinterpret_cast<const double *>(itemPtr), 'g');
        break;

    case Asm::DirectiveDataType::NativeString:
    case Asm::DirectiveDataType::Utf8String:
    case Asm::DirectiveDataType::Utf16String:
    case Asm::DirectiveDataType::Utf32String:

    case Asm::DirectiveDataType::Real96: // TODO: Support FPA 96-bit extended precision.
    case Asm::DirectiveDataType::Byte:
    case Asm::DirectiveDataType::Max:
    default:
        appendHexSpecifier(buffer, formatOptions);
        format.setMinimumWholeDigits(2);
        format.setRadix(16);
        buffer.reserve(4);

        Ag::appendValue(format, buffer, *reinterpret_cast<const uint8_t *>(itemPtr));
        break;
    }

    if (buffer.empty())
    {
        bytesUsed = 0;
    }
    else
    {
        text = QString::fromUtf8(buffer.c_str(), buffer.length());
    }

    return bytesUsed;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


//! @file ArmDebugger/UI/MemoryBlockView.cpp
//! @brief The definition of an object which can render a view of a block of
//! memory in a CodeViewWidget.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <set>

#include "MemoryBlockView.hpp"

#include <QFontDatabase>
#include <QGuiApplication>
#include <QStyleHints>

#include "Ag/Core/Format.hpp"
#include "Ag/Core/LinearSortedMap.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/VariantTypes.hpp"
#include "Ag/QtInterop/Conversion.hpp"
#include "AsmTools/InstructionInfo.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
class SessionFormatterOptions : public Asm::FormatterOptions
{
private:
    using LinearOrdinalMap = Ag::LinearSortedMap<uint32_t, QString>;

    LinearOrdinalMap _swiNames;
    LinearOrdinalMap _symbols;
public:
    // Construction/Destruction
    SessionFormatterOptions(const SessionSettings &settings) :
        Asm::FormatterOptions(0, settings.getMemoryViewOptions().getAssemblyFormatFlags()),
        _swiNames(settings.getSwiMap()),
        _symbols(settings.getSymbolMap())
    {
    }

    virtual ~SessionFormatterOptions() = default;

    // Operations
    void update(const SessionSettings &settings)
    {
        _swiNames.clear();
        _symbols.clear();

        const auto &importedSWIs = settings.getSwiMap();
        _swiNames.reserve(importedSWIs.size());
        _swiNames.appendRange(importedSWIs.begin(), importedSWIs.end());
        _swiNames.reindex();

        const auto &importedSymbols = settings.getSymbolMap();
        _symbols.reserve(importedSymbols.size());
        _symbols.appendRange(importedSymbols.begin(), importedSymbols.end());
        _symbols.reindex();
    }

    // Overrides
    virtual void appendAddressSymbol(uint32_t address,
                                     std::string &buffer) const override
    {
        auto symbolMapping = _symbols.find(address);

        if (symbolMapping != _symbols.end())
        {
            // The address has been annotated with a symbol, return it.
            QByteArray utf8Symbol = symbolMapping->second.toUtf8();

            buffer.append(utf8Symbol.constData(), utf8Symbol.size());
        }
    }

    virtual void appendSwiComment(uint32_t value,
                                  std::string &buffer) const override
    {
        auto swiMapping = _swiNames.find(value);

        if (swiMapping != _swiNames.end())
        {
            // The SWI was defined with a name, return it.
            QByteArray utf8SwiName = swiMapping->second.toUtf8();

            buffer.append(utf8SwiName.constData(), utf8SwiName.size());
        }
    }

    //virtual void appendBreakPointComment(uint32_t value,
    //                                     std::string &buffer) const override;
};


////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// BlockViewContext Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
BlockViewContext::BlockViewContext(const SessionSettings &settings) :
    _spacing(4),
    _marginWidth(16),
    _addressWidth(0),
    _rawDataWidth(0),
    _mnemonicWidth(0),
    _operandWidth(0),
    _commentWidth(0)
{
    reinitialise(settings);
}

double BlockViewContext::getMarginWidth() const { return _marginWidth; }
double BlockViewContext::getAddressWidth() const { return _addressWidth; }
double BlockViewContext::getDataWidth() const { return _rawDataWidth; }
double BlockViewContext::getMnemonicWidth() const { return _mnemonicWidth; }
double BlockViewContext::getOperandWidth() const { return _operandWidth; }
double BlockViewContext::getCommentWidth() const { return _commentWidth; }
const MemoryViewOptions &BlockViewContext::getOptions() const { return _viewOptions; }

void BlockViewContext::maximiseMnemonicWidth(double width)
{
    _mnemonicWidth = std::max(_mnemonicWidth, width);
}

void BlockViewContext::maximiseOperandWidth(double width)
{
    _operandWidth = std::max(_operandWidth, width);
}

void BlockViewContext::maximiseCommentWidth(double width)
{
    _commentWidth = std::max(_commentWidth, width);
}

double BlockViewContext::getLineHeight() const
{
    return _metrics ? _metrics->height() : 0.0;
}

double BlockViewContext::getBaselineAdjustment() const
{
    return _metrics ? _metrics->ascent() : 0.0;
}

const QFont &BlockViewContext::getCodeFont() const { return _codeFont; }

const QFontMetricsF &BlockViewContext::getCodeFontMetrics() const
{
    return *_metrics;
}

int BlockViewContext::calculateTextLength(const QString &text) const
{
    return _metrics->boundingRect(text).width();
}

QSizeF BlockViewContext::calculateTextSize(const QString &text) const
{
    return _metrics->boundingRect(text).size();
}

double BlockViewContext::calculateLeftEdge(uint32_t lineComponentFlags) const
{
    double offsetX = 0.0;

    if (lineComponentFlags & Margin)
    {
        offsetX += _marginWidth;
    }

    if (lineComponentFlags & Address)
    {
        if (offsetX > 0)
            offsetX += _spacing;

        offsetX += _addressWidth;
    }

    if (lineComponentFlags & RawData)
    {
        if (offsetX > 0)
            offsetX += _spacing;

        offsetX += _rawDataWidth;
    }

    if (lineComponentFlags & Mnemonic)
    {
        if (offsetX > 0)
            offsetX += _spacing;

        offsetX += _mnemonicWidth;
    }

    if (lineComponentFlags & Operands)
    {
        if (offsetX > 0)
            offsetX += _spacing;

        offsetX += _operandWidth;
    }

    if (lineComponentFlags & Comments)
    {
        if (offsetX > 0)
            offsetX += _spacing;

        offsetX += _commentWidth;
    }

    return offsetX;
}

const QColor &BlockViewContext::getColour(TokenType tokenClass) const
{
    auto mapping = _tokenColours.find(Ag::toScalar(tokenClass));

    return (mapping == _tokenColours.end()) ? _defaultColor : mapping->second;
}

const QColor &BlockViewContext::getColour(BlockElementType elementType) const
{
    auto mapping = _tokenColours.find(Ag::toScalar(elementType));

    return (mapping == _tokenColours.end()) ? _defaultColor : mapping->second;
}

Asm::FormatterOptions *BlockViewContext::getInstructionFormatter() const
{
    return _formatter.get();
}

void BlockViewContext::resetSizes()
{
    if (_metrics)
    {
        bool useBasicHex = _viewOptions.getAssemblyFormatFlags() & Asm::FormatterOptions::UseBasicStyleHex;
        QString addressText = formatAddress(0, useBasicHex);
        addressText.append(": ");

        _addressWidth = _metrics->boundingRect(addressText).width();
        _mnemonicWidth = _metrics->boundingRect("EQUSU32").width();

        _spacing = _metrics->boundingRect('#').width();
        _marginWidth = std::max(_metrics->height(), 16.0);
    }
    else
    {
        _addressWidth = 0;
        _mnemonicWidth = 0;
        _spacing = 0;
        _marginWidth = 0;
    }

    // TODO: Create (optional?) raw data view
    //_rawDataWidth = _metrics->boundingRect("0x00000000").width();
    _rawDataWidth = 0.0;
    _operandWidth = 0.0;
    _commentWidth = 0.0;
}

void BlockViewContext::reinitialise(const SessionSettings &settings)
{
    _codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    _codeFont.setPointSizeF(_codeFont.pointSizeF() * 1.25);
    _viewOptions = settings.getMemoryViewOptions();

    // Ensure we're created an object which specifies how to format
    // disassembled assembly language instructions.
    SessionFormatterOptions *formatter = dynamic_cast<SessionFormatterOptions *>(_formatter.get());

    if (formatter == nullptr)
    {
        auto formatObj = std::make_unique<SessionFormatterOptions>(settings);

        formatter = formatObj.get();
        _formatter = std::move(formatObj);
    }

    formatter->update(settings);

    _metrics = std::make_unique<QFontMetricsF>(_codeFont);
    resetSizes();

    _tokenColours.clear();

    QColor lightGrey(0xC0, 0xC0, 0xC0);
    QColor lightBlue(0x40, 0x40, 0xFF);

    // Set base colours for light theme.
    _tokenColours[Ag::toScalar(TokenType::CoreMnemonic)] = QColor(Qt::magenta);
    _tokenColours[Ag::toScalar(TokenType::CoProcMnemonic)] = QColor(Qt::blue);
    _tokenColours[Ag::toScalar(TokenType::FpaMnemonic)] = QColor(Qt::darkMagenta);
    _tokenColours[Ag::toScalar(TokenType::DataDirectiveMnemonic)] = QColor(Qt::magenta);
    _tokenColours[Ag::toScalar(TokenType::Separator)] = QColor(Qt::black);
    _tokenColours[Ag::toScalar(TokenType::WritebackMarker)] = QColor(Qt::darkGreen);
    _tokenColours[Ag::toScalar(TokenType::ModifyPsrMaker)] = QColor(Qt::darkGreen);
    _tokenColours[Ag::toScalar(TokenType::CoreRegister)] = QColor(Qt::darkCyan);
    _tokenColours[Ag::toScalar(TokenType::BeginAddrOperand)] = QColor(Qt::darkRed);
    _tokenColours[Ag::toScalar(TokenType::EndAddrOperand)] = QColor(Qt::darkRed);
    _tokenColours[Ag::toScalar(TokenType::CoreRegList)] = QColor(Qt::darkCyan);
    _tokenColours[Ag::toScalar(TokenType::CoProcessorID)] = QColor(Qt::blue);
    _tokenColours[Ag::toScalar(TokenType::CoProcessorRegister)] = QColor(Qt::darkGreen);
    _tokenColours[Ag::toScalar(TokenType::FpaRegister)] = QColor(Qt::darkGreen);
    _tokenColours[Ag::toScalar(TokenType::Shift)] = QColor(Qt::darkBlue);
    _tokenColours[Ag::toScalar(TokenType::ImmediateConstant)] = QColor(Qt::black);
    _tokenColours[Ag::toScalar(TokenType::Label)] = QColor(Qt::darkBlue);
    _tokenColours[Ag::toScalar(TokenType::DataValue)] = QColor(Qt::darkGray);

    _tokenColours[Ag::toScalar(BlockElementType::Address)] = QColor(Qt::darkGray);
    _tokenColours[Ag::toScalar(BlockElementType::Margin)] = lightGrey;
    _tokenColours[Ag::toScalar(BlockElementType::Comment)] = Qt::darkGreen;

    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    {
        _defaultColor = QColor(Qt::white);

        // Override colours for dark theme.
        _tokenColours[Ag::toScalar(TokenType::Separator)] = QColor(Qt::white);
        _tokenColours[Ag::toScalar(TokenType::WritebackMarker)] = QColor(Qt::green);
        _tokenColours[Ag::toScalar(TokenType::ModifyPsrMaker)] = QColor(Qt::green);
        _tokenColours[Ag::toScalar(TokenType::CoreRegister)] = QColor(Qt::cyan);
        _tokenColours[Ag::toScalar(TokenType::BeginAddrOperand)] = QColor(Qt::red);
        _tokenColours[Ag::toScalar(TokenType::EndAddrOperand)] = QColor(Qt::red);
        _tokenColours[Ag::toScalar(TokenType::CoreRegList)] = QColor(Qt::cyan);
        _tokenColours[Ag::toScalar(TokenType::CoProcessorRegister)] = QColor(Qt::green);
        _tokenColours[Ag::toScalar(TokenType::Shift)] = QColor(Qt::blue);
        _tokenColours[Ag::toScalar(TokenType::ImmediateConstant)] = QColor(Qt::white);
        _tokenColours[Ag::toScalar(TokenType::Label)] = lightBlue;
        _tokenColours[Ag::toScalar(TokenType::DataValue)] = lightGrey;
        _tokenColours[Ag::toScalar(BlockElementType::Address)] = lightGrey;
        _tokenColours[Ag::toScalar(BlockElementType::Margin)] = QColor(Qt::darkGray);
        _tokenColours[Ag::toScalar(BlockElementType::Comment)] = Qt::green;
    }
    else
    {
        _defaultColor = QColor(Qt::black);
    }
}

void BlockViewContext::prepareToPaint(QPainter *renderer,
                                      BlockViewContext::TokenType tokenClass) const
{
    prepareToPaint(renderer, Ag::toScalar(tokenClass));
}

void BlockViewContext::prepareToPaint(QPainter *renderer, BlockElementType tokenClass) const
{
    prepareToPaint(renderer, Ag::toScalar(tokenClass));
}

void BlockViewContext::prepareToPaint(QPainter *renderer, uint32_t style) const
{
    auto mapping = _tokenColours.find(style);

    if (mapping == _tokenColours.end())
    {
        renderer->setPen(_defaultColor);
    }
    else
    {
        renderer->setPen(mapping->second);
    }

    renderer->setFont(_codeFont);
}

void BlockViewContext::paintAddress(QPainter *renderer, const QPointF &at,
                                    uint32_t address) const
{
    bool useBasicHex = _viewOptions.getAssemblyFormatFlags() & Asm::FormatterOptions::UseBasicStyleHex;
    QString addressText = formatAddress(address, useBasicHex);
    addressText.append(':');

    prepareToPaint(renderer, BlockElementType::Address);
    renderer->drawText(at, addressText);
}

////////////////////////////////////////////////////////////////////////////////
// MemoryBlockView Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
MemoryBlockView::MemoryBlockView(uint32_t baseAddr, uint32_t length) :
    _extents(0, 0),
    _baseAddress(baseAddr),
    _length(length),
    _verticalOffset(0)
{
}

uint32_t MemoryBlockView::getBaseAddress() const { return _baseAddress; }
uint32_t MemoryBlockView::getEndAddress() const { return _baseAddress + _length; }
uint32_t MemoryBlockView::getLength() const { return _length; }
const QSizeF &MemoryBlockView::getExtents() const { return _extents; }
double MemoryBlockView::getVerticalOffset() const { return _verticalOffset; }
void MemoryBlockView::setVerticalOffset(double offsetY) { _verticalOffset = offsetY; }
double MemoryBlockView::getVerticalEnd() const { return _verticalOffset + _extents.height(); }

bool MemoryBlockView::containsAddress(uint32_t address) const
{
    uint32_t endAddress = _baseAddress + _length;

    return (address < endAddress) && (address >= _baseAddress);
}

bool MemoryBlockView::containsRow(double offsetY) const
{
    int endY = _verticalOffset + _extents.height();

    return (offsetY < endY) && (offsetY >= _verticalOffset);
}

void MemoryBlockView::measure(BlockViewContext &context)
{
    _replacementWords.reindex();

    // Measure the extents and store the results internally.
    _extents = calculateExtents(context);
}

void MemoryBlockView::addSubstitutedWord(uint32_t address, uint32_t word)
{
    _replacementWords.push_back(address, word);
}

void MemoryBlockView::clearSubstitutedWords()
{
    _replacementWords.clear();
}

const MemoryBlockView::ReplacementMap &MemoryBlockView::getReplacements() const
{
    return _replacementWords;
}

void MemoryBlockView::setAddressRange(uint32_t baseAddr, uint32_t length)
{
    _baseAddress = baseAddr;
    _length = length;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
QString formatAddress(uint32_t address, bool useBasicPrefix)
{
    std::string buffer;
    buffer.reserve(12);

    if (useBasicPrefix)
    {
        buffer.push_back('&');
    }
    else
    {
        buffer.append("0x", 2);
    }

    Ag::FormatInfo format(Ag::LocaleInfo::getNeutral());
    format.setRadix(16);
    format.setMinimumWholeDigits(8);
    format.enableUpperCase(true);

    Ag::appendValue(format, buffer, address);

    return QString::fromUtf8(buffer.c_str(), buffer.length());
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


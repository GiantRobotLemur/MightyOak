//! @file ArmDebugger/UI/MemoryBlockView.hpp
//! @brief The declaration of an object which can render a view of a block of
//! memory in a CodeViewWidget.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_MEMORY_BLOCK_VIEW_HPP__
#define __ARM_DEBUGGER_MEMORY_BLOCK_VIEW_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <map>
#include <memory>
#include <vector>

#include <QPainter>

#include "Ag/Core/LinearSortedMap.hpp"

#include "AsmTools/InstructionInfo.hpp"

#include "SessionSettings.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
enum class BlockElementType
{
    Address = Asm::FormattedInstruction::TokenType::Max,
    Margin,
    Comment,
    Divider,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which contains graphic resources and annotations required
//! to measure and draw MemoryBlockView implementations.
class BlockViewContext
{
public:
    // Public Types
    using TokenType = Asm::FormattedInstruction::TokenType;

    enum LineComponent : uint32_t
    {
        Margin      = 0x01,
        Address     = 0x02,
        RawData     = 0x04,
        Mnemonic    = 0x08,
        Operands    = 0x10,
        Comments    = 0x20,

        UpToRawData = 0x03,
        UpToMnemonic = 0x07,
        UpToOperands = 0x0F,
        UpToComments = 0x2F,

        All         = 0x3F

    };

    // Construction/Destruction
    BlockViewContext(const SessionSettings &settings);
    ~BlockViewContext() = default;

    // Accessors
    double getMarginWidth() const;
    double getAddressWidth() const;
    double getDataWidth() const;
    double getMnemonicWidth() const;
    double getOperandWidth() const;
    double getCommentWidth() const;

    const MemoryViewOptions &getOptions() const;

    void maximiseMnemonicWidth(double width);
    void maximiseOperandWidth(double width);
    void maximiseCommentWidth(double width);
    double getLineHeight() const;
    double getBaselineAdjustment() const;
    const QFont &getCodeFont() const;
    const QFontMetricsF &getCodeFontMetrics() const;
    int calculateTextLength(const QString &text) const;
    QSizeF calculateTextSize(const QString &text) const;
    double calculateLeftEdge(uint32_t lineComponentFlags) const;
    const QColor &getColour(TokenType tokenClass) const;
    const QColor &getColour(BlockElementType elementType) const;
    Asm::FormatterOptions *getInstructionFormatter() const;

    // Operations
    void resetSizes();
    void reinitialise(const SessionSettings &settings);
    void prepareToPaint(QPainter *renderer, TokenType tokenClass) const;
    void prepareToPaint(QPainter *renderer, BlockElementType tokenClass) const;
    void paintAddress(QPainter *renderer, const QPointF &at, uint32_t address) const;
private:
    // Internal Fields
    using QFontMetricsFUPtr = std::unique_ptr<QFontMetricsF>;
    using FormatterOptionsUPtr = std::unique_ptr<Asm::FormatterOptions>;

    // Internal Functions
    void prepareToPaint(QPainter *renderer, uint32_t style) const;

    // Internal Fields
    MemoryViewOptions _viewOptions;
    QFont _codeFont;
    QFontMetricsFUPtr _metrics;
    FormatterOptionsUPtr _formatter;
    QColor _defaultColor;
    std::map<uint32_t, QColor> _tokenColours;
    double _spacing;
    double _marginWidth;
    double _addressWidth;
    double _rawDataWidth;
    double _mnemonicWidth;
    double _operandWidth;
    double _commentWidth;
};

//! @brief An object which can render a view of a block of memory in a
//! CodeViewWidget.
class MemoryBlockView
{
public:
    // Construction/Destruction
    MemoryBlockView(uint32_t baseAddr, uint32_t length);
    virtual ~MemoryBlockView() = default;

    // Accessors
    uint32_t getBaseAddress() const;
    uint32_t getEndAddress() const;
    uint32_t getLength() const;
    const QSizeF &getExtents() const;
    double getVerticalOffset() const;
    void setVerticalOffset(double offsetY);
    double getVerticalEnd() const;
    bool containsAddress(uint32_t address) const;
    bool containsRow(double offsetY) const;

    // Operations
    void measure(BlockViewContext &context);
    void addSubstitutedWord(uint32_t address, uint32_t word);
    void clearSubstitutedWords();

    // Overrides
    //! @brief Determines if a position equates to some data in the block.
    //! @param[in] context Data shared between blocks.
    //! @param[in] position The position within the overall view to
    //! query.
    //! @param[out] address Receives the physical address of the object at the
    //! specified position.
    //! @param[out] length Receives the length, in bytes, of the object at the
    //! specified position.
    //! @retval true A piece of data was at the specified position, address and
    //! length have been updated to describe it.
    //! @retval false Nothing meaningful is at the specified position.
    virtual bool hitTest(const BlockViewContext &context,
                         const QPointF &position, uint32_t &address,
                         uint32_t &length) const =0;

    //! @brief Attempts to determine the visual area which represent a
    //! specified physical addresses.
    //! @param[in] context Data shared between blocks.
    //! @param[in] address The address to query.
    //! @param[out] area Receives the area displaying data at the specified
    //! address.
    //! @retval true The address had a corresponding display region, returned
    //! as the area rectangle.
    //! @retval false The address did not have a corresponding display region.
    virtual bool tryFindAddress(const BlockViewContext &context,
                                uint32_t address, QRectF &area) const =0;

    //! @brief Requests that the block render itself if necessary.
    //! @param[in] context Data shared between blocks.
    //! @param[in] painter The object to render to.
    //! @param[in] scrollOrigin The scroll offsets of the widget being drawn.
    //! @param[in] updateRegion The rectangle to be drawn in widget coordinates.
    virtual void draw(const BlockViewContext &context,
                      QPainter *painter,
                      const QPointF &scrollOrigin,
                      const QRectF &updateRegion) const =0;
protected:

    //! @brief Calculates the size of the view of the block of memory.
    //! @param[in] context Data shared between blocks and to be updated with
    //! common size values.
    //! @return The extents of the current block.
    virtual QSizeF calculateExtents(BlockViewContext &context) = 0;

    // Internal Types
    using ReplacementMap = Ag::LinearSortedMap<uint32_t, uint32_t>;

    // Internal Functions
    const ReplacementMap &getReplacements() const;
    void setAddressRange(uint32_t baseAddr, uint32_t length);
private:
    // Internal Fields
    QSizeF _extents;
    uint32_t _baseAddress;
    uint32_t _length;
    double _verticalOffset;
    ReplacementMap _replacementWords;

};

using MemoryBlockViewUPtr = std::unique_ptr<MemoryBlockView>;
using MemoryBlockViewCollection = std::vector<MemoryBlockViewUPtr>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
QString formatAddress(uint32_t address, bool useBasicPrefix);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

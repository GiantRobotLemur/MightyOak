//! @file ArmDebugger/UI/LabelBlockView.cpp
//! @brief The definition of a MemoryBlockView implementation which displays
//! an assembly label.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "UI/LabelBlockView.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// LabelBlockView Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
LabelBlockView::LabelBlockView(const QString &symbol, uint32_t address) :
    MemoryBlockView(address, 0)
{
    _symbolText.reserve(symbol.length() + 1);
    _symbolText.append('.');
    _symbolText.append(symbol);
}

bool LabelBlockView::hitTest(const BlockViewContext &context,
                             const QPointF &position, uint32_t &address,
                             uint32_t &length) const
{
    return false;
}

bool LabelBlockView::tryFindAddress(const BlockViewContext &context,
                                    uint32_t address, QRectF &area) const
{
    return false;
}

void LabelBlockView::draw(const BlockViewContext &context,
                          QPainter *painter,
                          const QPointF &scrollOrigin,
                          const QRectF &updateRegion) const
{
    QRectF lineRect(context.calculateLeftEdge(BlockViewContext::Margin) - scrollOrigin.x(),
                    getVerticalOffset() - scrollOrigin.y(),
                    getExtents().width(), getExtents().height());

    if (updateRegion.intersects(updateRegion))
    {
        QPointF textOrigin = lineRect.topLeft();
        textOrigin.ry() += context.getBaselineAdjustment();

        context.prepareToPaint(painter, Asm::FormattedInstruction::TokenType::Label);
        painter->drawText(textOrigin, _symbolText);
    }
}

QSizeF LabelBlockView::calculateExtents(BlockViewContext &context)
{
    double offsetX = context.calculateLeftEdge(BlockViewContext::UpToRawData);

    QSizeF labelSize = context.calculateTextSize(_symbolText);
    labelSize.rwidth() += offsetX;

    return labelSize;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


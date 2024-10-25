//! @file ArmDebugger/UI/UniformBlockView.cpp
//! @brief The definition of an implementation of MemoryBlockView which optimally
//! renders repetitive memory contents.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "UniformBlockView.hpp"

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
// UniformBlockView Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
UniformBlockView::UniformBlockView(uint32_t baseAddr, uint32_t length,
                                   const QString &mnemonic, const QString &operands,
                                   uint32_t repetitions) :
    MemoryBlockView(baseAddr, length),
    _mnemonic(mnemonic),
    _operands(operands)
{
    _comment = QCoreApplication::tr("; x %1 repetitions.",
                                    "UniformBlockView").arg(repetitions);
}

bool UniformBlockView::hitTest(const BlockViewContext &/*context*/,
                               const QPointF &/*position*/, uint32_t &/*address*/,
                               uint32_t &/*length*/) const
{
    return false;
}

bool UniformBlockView::tryFindAddress(const BlockViewContext &/*context*/,
                                      uint32_t /*address*/, QRectF &/*area*/) const
{
    return false;
}

void UniformBlockView::draw(const BlockViewContext &context,
                             QPainter *painter,
                             const QPointF &scrollOrigin,
                             const QRectF &/*updateRegion*/) const
{
    QPointF textOrigin(context.calculateLeftEdge(BlockViewContext::Margin) - scrollOrigin.x(),
                       getVerticalOffset() - scrollOrigin.y() +
                       context.getBaselineAdjustment() + context.getLineHeight());
    const double LineHeight = context.getLineHeight();
    double textHeight = getExtents().height() - LineHeight * 2.0;

    // Draw dividing lines.
    QPen divider(QBrush(context.getColour(BlockElementType::Divider)),
                 1.0, Qt::DotLine);
    painter->setPen(divider);

    for (uint32_t i = 0; i < 2; ++i)
    {
        double offsetY = (LineHeight + textHeight) * i;
        QPointF origin = QPointF(context.getMarginWidth(),
                                 getVerticalOffset() + (LineHeight * 0.5) + offsetY) - scrollOrigin;

        double rightOffset = context.calculateLeftEdge(BlockViewContext::All) - scrollOrigin.x();

        QLineF line(origin, QPointF(rightOffset, origin.y()));

        const double Spacing = context.calculateTextLength("#");

        // Create geometry for wiggles.
        constexpr int WiggleCount = 2;
        constexpr double StraightRuns = WiggleCount + 1.0;

        std::vector<QPointF> vertices;
        vertices.reserve(2 + WiggleCount * 4);

        vertices.push_back(line.pointAt(0.0));

        for (int j = 0; j < WiggleCount; ++j)
        {
            const QPointF centre = line.pointAt((j + 1.0) / StraightRuns);

            vertices.emplace_back(centre.x() - Spacing * 2.0, centre.y());
            vertices.emplace_back(centre.x() - Spacing, centre.y() + Spacing);
            vertices.emplace_back(centre.x() + Spacing, centre.y() - Spacing);
            vertices.emplace_back(centre.x() + Spacing * 2.0, centre.y());
        }

        vertices.push_back(line.pointAt(1.0));

        painter->drawPolyline(vertices.data(), static_cast<int>(vertices.size()));
    }

    context.paintAddress(painter, textOrigin, getBaseAddress());

    if (_mnemonic.isEmpty() == false)
    {
        textOrigin.setX(context.calculateLeftEdge(BlockViewContext::UpToMnemonic) - scrollOrigin.x());
        context.prepareToPaint(painter, BlockViewContext::TokenType::DataDirectiveMnemonic);
        painter->drawText(textOrigin, _mnemonic);
    }

    if (_operands.isEmpty() == false)
    {
        textOrigin.setX(context.calculateLeftEdge(BlockViewContext::UpToOperands) - scrollOrigin.x());
        context.prepareToPaint(painter, BlockViewContext::TokenType::DataValue);
        painter->drawText(textOrigin, _operands);
    }

    if (_operands.isEmpty() == false)
    {
        textOrigin.setX(context.calculateLeftEdge(BlockViewContext::UpToComments) - scrollOrigin.x());
        context.prepareToPaint(painter, BlockElementType::Comment);
        painter->drawText(textOrigin, _comment);
    }
}

QSizeF UniformBlockView::calculateExtents(BlockViewContext &context)
{
    QSizeF mnemonicSize = context.calculateTextSize(_mnemonic);
    QSizeF operandSize = context.calculateTextSize(_operands);
    QSizeF commentSize = context.calculateTextSize(_comment);

    context.maximiseMnemonicWidth(mnemonicSize.width());
    context.maximiseOperandWidth(operandSize.width());
    context.maximiseCommentWidth(commentSize.width());

    double height = std::max({ mnemonicSize.height(),
                               operandSize.height(),
                               commentSize.height() });

    height += context.getLineHeight() * 2.0;

    double width = context.calculateLeftEdge(BlockViewContext::All);

    return QSizeF(width, height);
}
////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


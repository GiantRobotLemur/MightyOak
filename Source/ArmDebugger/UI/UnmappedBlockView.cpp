//! @file ArmDebugger/UI/UnmappedBlockView.cpp
//! @brief The definition of an implementation of MemoryBlockView which shows
//! that a range of logical addresses aren't mapped to physical memory.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "UnmappedBlockView.hpp"

#include "Ag/Core/Format.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/VariantTypes.hpp"
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
// UnmappedBlockView Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
UnmappedBlockView::UnmappedBlockView(uint32_t baseAddr, uint32_t length) :
    MemoryBlockView(baseAddr, length)
{
    Ag::FormatInfo format(Ag::LocaleInfo::getNeutral());
    std::string buffer;
    Ag::appendFormat(format, "0x{0:X8}-0x{1:X8}: ",
                        buffer, { baseAddr, baseAddr + length });

    _addressText = QString::fromUtf8(buffer.c_str(), buffer.length());

    buffer.clear();
    Ag::appendFormat(format, "Non-present logical pages ({0:K})",
                        buffer, { length });

    _descriptionText = QString::fromUtf8(buffer.c_str(), buffer.length());
}

bool UnmappedBlockView::hitTest(const BlockViewContext &/*context*/,
                                const QPointF &position, uint32_t &address,
                                uint32_t &length) const
{
    double bottomY = getVerticalOffset() + getExtents().height();
    bool isInside = false;

    if ((position.y() >= getVerticalOffset()) && (position.y() < bottomY))
    {
        address = getBaseAddress();
        length = getLength();
        isInside = true;
    }

    return isInside;
}

bool UnmappedBlockView::tryFindAddress(const BlockViewContext &/*context*/,
                                       uint32_t address, QRectF &area) const
{
    bool hasAddress = false;

    if (containsAddress(address))
    {
        area.setX(0);
        area.setY(getVerticalOffset());
        area.setSize(getExtents());
        hasAddress = true;
    }

    return hasAddress;
}

void UnmappedBlockView::draw(const BlockViewContext &context,
                             QPainter *painter,
                             const QPointF &scrollOrigin,
                             const QRectF &/*updateRegion*/) const
{
    QPointF textOrigin(context.getMarginWidth() - scrollOrigin.x(),
                        getVerticalOffset() + context.getBaselineAdjustment() - scrollOrigin.y());

    context.prepareToPaint(painter, BlockElementType::Address);
    painter->drawText(textOrigin, _addressText);
    textOrigin.rx() += context.calculateTextLength(_addressText);

    context.prepareToPaint(painter, BlockElementType::Comment);
    painter->drawText(textOrigin, _descriptionText);
}

QSizeF UnmappedBlockView::calculateExtents(BlockViewContext &context)
{
    QSizeF addrSize = context.calculateTextSize(_addressText);
    QSizeF descSize = context.calculateTextSize(_descriptionText);

    return QSizeF(addrSize.width() + descSize.width() + context.getMarginWidth(),
                  std::max(addrSize.height(), descSize.height()));
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


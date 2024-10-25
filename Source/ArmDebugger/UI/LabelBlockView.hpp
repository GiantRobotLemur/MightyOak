//! @file ArmDebugger/UI/LabelBlockView.hpp
//! @brief The declaration of a MemoryBlockView implementation which displays
//! an assembly label.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_LABEL_BLOCK_VIEW_HPP__
#define __ARM_DEBUGGER_LABEL_BLOCK_VIEW_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "MemoryBlockView.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief a MemoryBlockView implementation which displays an assembly label.
class LabelBlockView : public MemoryBlockView
{
public:
    // Construction/Destruction
    LabelBlockView(const QString &symbol, uint32_t address);
    virtual ~LabelBlockView() = default;

    // Accessors

    // Operations

    // Overrides
    virtual bool hitTest(const BlockViewContext &context,
                         const QPointF &position, uint32_t &address,
                         uint32_t &length) const override;
    virtual bool tryFindAddress(const BlockViewContext &context,
                                uint32_t address, QRectF &area) const override;
    virtual void draw(const BlockViewContext &context,
                      QPainter *painter,
                      const QPointF &scrollOrigin,
                      const QRectF &updateRegion) const override;
protected:
    virtual QSizeF calculateExtents(BlockViewContext &context) override;

private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    QString _symbolText;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

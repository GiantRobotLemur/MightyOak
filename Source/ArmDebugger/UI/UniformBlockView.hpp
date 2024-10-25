//! @file ArmDebugger/UI/UniformBlockView.hpp
//! @brief The declaration of an implementation of MemoryBlockView which optimally
//! renders repetitive memory contents.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_UNIFORM_BLOCK_VIEW_HPP__
#define __ARM_DEBUGGER_UNIFORM_BLOCK_VIEW_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "MemoryBlockView.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An implementation of MemoryBlockView which optimally renders
//! repetitive memory contents.
class UniformBlockView : public MemoryBlockView
{
public:
    // Construction/Destruction
    UniformBlockView(uint32_t baseAddr, uint32_t length,
                     const QString &mnemonic, const QString &operands,
                     uint32_t repetitions);
    virtual ~UniformBlockView() = default;

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
    // Internal Fields
    QString _mnemonic;
    QString _operands;
    QString _comment;
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

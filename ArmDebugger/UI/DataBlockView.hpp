//! @file ArmDebugger/UI/DataBlockView.hpp
//! @brief The declaration of an object which renders a view of a block of
//! memory as assembly language data directives.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_DATA_BLOCK_VIEW_HPP__
#define __ARM_DEBUGGER_DATA_BLOCK_VIEW_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/Options.hpp"

#include "UI/MemoryBlockView.hpp"

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
//! @brief an object which renders a view of a block of memory as assembly
//! language data directives.
class DataBlockView : public MemoryBlockView
{
public:
    // Construction/Destruction
    DataBlockView(uint32_t baseAddress, uint32_t length,
                  const void *hostData, Asm::DirectiveDataType dataType);
    virtual ~DataBlockView() = default;

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
    bool tryGetNextScalarItemText(uint32_t offset, QString &text,
                                  uint32_t bytesPerItem,
                                  uint32_t formatOptions) const;

    // Internal Fields
    QString _mnemonic;
    std::vector<uint32_t> _lineLengths;
    const void *_sourceData;
    uint32_t _formatFlags;
    Asm::DirectiveDataType _dataType;
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

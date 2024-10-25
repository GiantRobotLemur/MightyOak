//! @file ArmDebugger/UI/MemoryViewWidget.hpp
//! @brief The declaration of a widget which displays the contents of memory in
//! the emulated machine as assembly language.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_MEMORY_VIEW_WIDGET_HPP__
#define __ARM_DEBUGGER_MEMORY_VIEW_WIDGET_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QAbstractScrollArea>

#include "MemoryBlockView.hpp"
#include "Tools.hpp"

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
namespace Arm {
class IArmSystem;
class Options;
} // namespace Arm

class EmulatorSession;

//! @brief A widget which displays the contents of memory in the emulated
//! machine as assembly language.
class MemoryViewWidget : public QAbstractScrollArea
{
Q_OBJECT
public:
    // Construction/Destruction
    MemoryViewWidget(QWidget *ownerObject, const SessionSettings &settings);
    virtual ~MemoryViewWidget() = default;

    // Accessors
    void setLogicallyMappedMemory(MemoryRegion &logicalMemory);

    // Operations
    void displayRegion(const MemoryRegion &region, uint32_t offset = 0);

    // Overrides
    virtual void paintEvent(QPaintEvent *args) override;
    virtual void resizeEvent(QResizeEvent *args) override;
    virtual void mouseReleaseEvent(QMouseEvent *args) override;
private slots:
    void onSessionStarted(const Arm::Options &options, Arm::IArmSystem *emulator);
    void onSessionEnded(Arm::IArmSystem *emulator);
    void updateStateFromEmulator(Arm::IArmSystem *emulator);
    void onColourSchemeChange(Qt::ColorScheme newColourScheme);
    void onDisplayOptionsChange();
    void onDisplayFormatChange();
    void onBreakpointChange(const EmulatorSession *session);
private:
    // Internal Types

    // Internal Functions
    QPoint getScrollOrigin() const;
    QPointF getScrollOriginF() const;
    void onExtentsUpdated(bool resetScrollOffsets = false);
    void paintBreakpoints(QPainter *painter, const MemoryBlockView *block);
    void updateMargin();

    bool tryFindBlockByAddress(uint32_t address, size_t &index) const;
    bool tryFindBlockByPosition(double offsetY, size_t &index) const;

    // Internal Fields
    MemoryBlockViewCollection _blocks;
    BlockViewContext _blockContext;
    Arm::IArmSystem *_emulator;
    QSizeF _extents;
    MemoryRegion _logicalMemory;
    MemoryRegion _currentRegion;
    uint32_t _currentOffset;
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

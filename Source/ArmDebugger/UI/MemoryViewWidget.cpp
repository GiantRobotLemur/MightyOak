//! @file ArmDebugger/UI/MemoryViewWidget.cpp
//! @brief The definition of a widget which displays the contents of memory in
//! the emulated machine as assembly language.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <QPaintEvent>
#include <QScrollBar>
#include <QStyleHints>

#include "Ag/Core/Math.hpp"
#include "AsmTools/Options.hpp"

#include "CodeBlockView.hpp"
#include "DataBlockView.hpp"
#include "DebuggerApp.hpp"
#include "EmulatorSession.hpp"
#include "LabelBlockView.hpp"
#include "MemoryViewWidget.hpp"
#include "UniformBlockView.hpp"
#include "UnmappedBlockView.hpp"

#include "Annotations/DataReplacement.hpp"
#include "Annotations/InstructionFormat.hpp"
#include "Annotations/TypeOverride.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A memory block view used to find others in a sorted index.
class KeyBlockView : public MemoryBlockView
{
public:
    KeyBlockView() :
        MemoryBlockView(0, 0)
    {
    }

    virtual ~KeyBlockView() = default;

    // Accessors
    void setMemoryKey(uint32_t baseAddr)
    {
        setAddressRange(baseAddr, 0);
    }

    virtual bool hitTest(const BlockViewContext &/*context*/,
                         const QPointF &/*position*/, uint32_t &/*address*/,
                         uint32_t &/*length*/) const override
    {
        return false;
    }

    virtual bool tryFindAddress(const BlockViewContext &/*context*/,
                                uint32_t /*address*/, QRectF &/*area*/) const override
    {
        return false;
    }


    virtual void draw(const BlockViewContext &/*context*/,
                      QPainter */*painter*/,
                      const QPointF &/*scrollOrigin*/,
                      const QRectF &/*updateRegion*/) const override
    {
        // Do nothing.
    }
protected:
    virtual QSizeF calculateExtents(BlockViewContext &/*context*/)
    {
        return QSizeF(0.0, 0.0);
    }
};

Q_DECLARE_FLAGS(FormatFlags, Asm::FormatterOptions::Flags)
Q_DECLARE_OPERATORS_FOR_FLAGS(FormatFlags)

struct BlockInfo
{
    const void *Source;
    uint32_t BaseAddress;
    uint32_t Length;
    FormatFlags DisasmOptions;
    Asm::DirectiveDataType DataType;
    bool IsUniform;

    static constexpr FormatFlags DefaultFormat = {
        Asm::FormatterOptions::ShowOffsets,
        Asm::FormatterOptions::UseCoreRegAliases,
        Asm::FormatterOptions::UseDecimalOffsets
    };

    BlockInfo() :
        Source(nullptr),
        BaseAddress(0),
        Length(0),
        DisasmOptions(DefaultFormat),
        DataType(Asm::DirectiveDataType::Max),
        IsUniform(false)
    {
    }

    BlockInfo(uint32_t baseAddr) :
        Source(nullptr),
        BaseAddress(baseAddr),
        Length(0),
        DisasmOptions(DefaultFormat),
        DataType(Asm::DirectiveDataType::Max),
        IsUniform(false)
    {
    }

    BlockInfo(const void *source, uint32_t baseAddr, uint32_t length,
              FormatFlags disasmOptions,
              Asm::DirectiveDataType dataType = Asm::DirectiveDataType::Max) :
        Source(source),
        BaseAddress(baseAddr),
        Length(length),
        DisasmOptions(disasmOptions),
        DataType(dataType),
        IsUniform(false)
    {
    }

    BlockInfo(BlockInfo &insertBefore, uint32_t splitOffset) :
        Source(insertBefore.Source),
        BaseAddress(insertBefore.BaseAddress),
        Length(splitOffset),
        DisasmOptions(insertBefore.DisasmOptions),
        DataType(insertBefore.DataType),
        IsUniform(insertBefore.IsUniform)
    {
        // Now update the block we inherit from to follow the
        // block we have constructed.
        insertBefore.Source = Ag::offsetPtr<void>(Source, splitOffset);
        insertBefore.BaseAddress = BaseAddress + splitOffset;
        insertBefore.Length -= splitOffset;
    }

    bool isCode() const { return DataType == Asm::DirectiveDataType::Max; }

    bool operator<(const BlockInfo &rhs) const
    {
        return BaseAddress < rhs.BaseAddress;
    }

    void applyAnnotation(const Annotation *annotation)
    {
        switch (annotation->getType())
        {
        case AnnotationType::DataReplacement:
        {
            auto replacement = static_cast<const DataReplacement *>(annotation);
            uint32_t offset = BaseAddress - annotation->getBaseAddress();

            Source = Ag::offsetPtr<void>(replacement->replacementData(), offset);
        } break;

        case AnnotationType::DataType:
            DataType = static_cast<const TypeOverride *>(annotation)->getOverrideType();
            break;

        case AnnotationType::Format:
            this->DisasmOptions =
                FormatFlags(static_cast<const InstructionFormat *>(annotation)->getDisassemblyFormatOptions());
            break;
        }
    }
};

using BlockInfoCollection = std::vector<BlockInfo>;
using BlockInfoIter = BlockInfoCollection::iterator;

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
bool toggleBreakpoint(uint32_t address, bool isLogical)
{
    bool isChanged = false;

    if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        auto &session = app->getSession();

        isChanged = session.toggleBreakpoint(address, isLogical);
    }

    return isChanged;
}

bool isContiguousMapping(const std::vector<Arm::PageMapping> &regions,
                         const Arm::PageMapping &mapping)
{
    bool isContiguous = false;

    if (regions.empty() == false)
    {
        const auto &lastMapping = regions.back();
        uint32_t lastEnd = lastMapping.PageBaseAddr + lastMapping.PageSize;

        isContiguous = ((mapping.Access & Arm::PageMapping::IsPresent) ==
                        (lastMapping.Access & Arm::PageMapping::IsPresent)) &&
            (lastEnd == mapping.PageBaseAddr);
    }

    return isContiguous;
}

bool canApplyAnnotation(const BlockInfo &block, const Annotation *annotation)
{
    bool canApply = false;

    switch (annotation->getType())
    {
    case AnnotationType::DataReplacement:
        canApply = true;
        break;

    case AnnotationType::DataType:
        canApply = static_cast<const TypeOverride *>(annotation)->getOverrideType() != block.DataType;
        break;

    case AnnotationType::Format:
        canApply = FormatFlags(static_cast<const InstructionFormat *>(annotation)->getDisassemblyFormatOptions()) != block.DisasmOptions;
        break;
    }

    return canApply;
}

BlockInfoIter splitBlockByAnnotation(BlockInfoCollection &blocks,
                                     BlockInfoIter blockToSplit,
                                     const Annotation *annotation)
{
    if (blockToSplit->BaseAddress < annotation->getBaseAddress())
    {
        // There needs to be an unaltered block before.
        uint32_t firstLength = annotation->getBaseAddress() - blockToSplit->BaseAddress;

        BlockInfo previous(*blockToSplit, firstLength);

        // Insert the new block after the first.
        blockToSplit = blocks.insert(blockToSplit, previous);
    }

    // If we get here, the blockToSplit and annotation should have the same
    // base address.
    if (blockToSplit->Length > annotation->getLength())
    {
        // The block needs to be split so that only come of it is affected.
        BlockInfo previous(*blockToSplit, annotation->getLength());

        blockToSplit = blocks.insert(blockToSplit, previous);

        // Retain a pointer to the item to which the annotation should be applied.
        --blockToSplit;
    }

    return blockToSplit;
}

void appendBlocks(MemoryBlockViewCollection &blocks, uint32_t baseAddress,
                  const void *hostAddress, uint32_t length,
                  const SessionSettings &settings)
{
    // Rules:
    // Default: Code
    // Where a label is defined, break the block - Insert label block.
    // If repetitive words (say 8+) - Uniform block
    // If data - Render data type
    // If odd address before aligned address - Align keyword

    //// Find the first label which might be applied.
    //const auto &labels = settings.getSymbolMap();
    //auto labelsEnd = labels.end();
    //auto labelsCurrent = labels.lower_bound(baseAddress);

    std::vector<BlockInfo> blockDefs;
    blockDefs.reserve(16);

    blockDefs.emplace_back(hostAddress, baseAddress, length, BlockInfo::DefaultFormat);

    // Split blocks around labels.
    const auto &symbolMap = settings.getSymbolMap();
    auto currentSymbol = symbolMap.lower_bound(baseAddress);
    auto endSymbol = settings.getSymbolMap().upper_bound(baseAddress + length);
    auto currentBlock = blockDefs.begin();
    size_t labelCount = 0;

    while ((currentSymbol != endSymbol) && (currentBlock != blockDefs.end()))
    {
        uint32_t blockEnd = currentBlock->BaseAddress + currentBlock->Length;
        uint32_t symbolAddr = currentSymbol->first;

        if (blockEnd == symbolAddr)
        {
            // The block is already split, move past both block and symbol.
            ++currentBlock;
            ++currentSymbol;
            ++labelCount;
        }
        else if (currentBlock->BaseAddress < symbolAddr)
        {
            if (blockEnd > symbolAddr)
            {
                // Split the block.
                BlockInfo original(*currentBlock, symbolAddr - currentBlock->BaseAddress);

                currentBlock = blockDefs.insert(currentBlock, original);

                // Move on to the second half of the split block and past the
                // applied symbol.
                ++currentBlock;
                ++currentSymbol;
                ++labelCount;
            }
            else
            {
                // There are no symbols in the block, move past it.
                ++currentBlock;
            }
        }
        else if (blockEnd < symbolAddr)
        {
            // The block is before the next symbol, move to the next block.
            ++currentBlock;
        }
        else
        {
            // The symbol was not applied (for some reason?) move past it?
            ++currentSymbol;
        }
    }

    // Find the first annotation which might be applied.
    const auto &annotations = settings.getAnnotations();

    for (auto current = annotations.begin() +
         findAnnotationLowerBound(annotations, baseAddress);
         current != annotations.end(); ++current)
    {
        const Annotation *annotation = current->get();
        BlockInfo key(annotation->getBaseAddress());

        // Find the first block affected by this annotation.
        auto blockPos = std::lower_bound(blockDefs.begin(), blockDefs.end(), key);

        for (; (blockPos != blockDefs.end()) &&
             (blockPos->BaseAddress < annotation->getEndAddress()); ++blockPos)
        {
            if (canApplyAnnotation(*blockPos, annotation))
            {
                // Split the block, if necessary, to apply the annotation to only
                // the portion required.
                blockPos = splitBlockByAnnotation(blockDefs, blockPos, annotation);

                blockPos->applyAnnotation(annotation);
            }
        }
    }

    // Section off areas of repeated data.
    for (auto blockPos = blockDefs.begin(); blockPos != blockDefs.end(); ++blockPos)
    {
        constexpr uint32_t MinRepeats = 8;

        if ((blockPos->Source == nullptr) || !blockPos->isCode() ||
            (blockPos->BaseAddress & 3) || (blockPos->Length < MinRepeats))
            continue;

        auto source = reinterpret_cast<const uint32_t *>(blockPos->Source);
        uint32_t wordCount = blockPos->Length / 4;
        uint32_t prevWord = *source;
        uint32_t blockStart = 0;
        uint32_t firstOfRun = 0;
        uint32_t runLength = 1;

        for (uint32_t offset = 1; offset <= wordCount; ++offset)
        {
            bool isEndOfRun = (offset >= wordCount);
            uint32_t currentWord = prevWord;

            if (isEndOfRun == false)
            {
                currentWord = source[offset];

                if (currentWord == prevWord)
                {
                    ++runLength;
                }
                else
                {
                    isEndOfRun = true;
                }
            }

            if (isEndOfRun)
            {
                if (runLength >= MinRepeats)
                {
                    // Create a run of consecutive data.
                    if (firstOfRun > blockStart)
                    {
                        // Insert a non-repeating block representing the data
                        // up to the beginning of the run.
                        BlockInfo nonUnformBlock(*blockPos, (firstOfRun - blockStart) * 4);
                        blockPos = blockDefs.insert(blockPos, nonUnformBlock);
                        blockStart = firstOfRun;
                        ++blockPos;
                    }

                    uint32_t endOfRun = firstOfRun + runLength;

                    if (endOfRun < wordCount)
                    {
                        // There are more words after this uniform block.
                        BlockInfo uniformBlock(*blockPos, runLength);
                        blockPos = blockDefs.insert(blockPos, uniformBlock);

                        // Configure the block as uniform data.
                        blockPos->IsUniform = true;
                        ++blockPos;
                    }
                    else
                    {
                        // Set the remaining block to be uniform.
                        blockPos->IsUniform = true;
                    }
                }

                // Start the next run.
                runLength = 1;
                prevWord = currentWord;
                firstOfRun = offset;
            }
        }
    }

    size_t requiredCapacity = blocks.size() + blockDefs.size() + labelCount;
    if (blocks.capacity() < requiredCapacity)
    {
        blocks.reserve(requiredCapacity);
    }

    for (auto &blockDef : blockDefs)
    {
        auto symbolPos = symbolMap.find(blockDef.BaseAddress);

        if (symbolPos != symbolMap.end())
        {
            // Insert a label before the current block.
            blocks.emplace_back(
                std::make_unique<LabelBlockView>(symbolPos->second,
                                                 symbolPos->first));
        }

        if (blockDef.isCode())
        {
            if (blockDef.IsUniform)
            {
                uint32_t repeatedWord = *reinterpret_cast<const uint32_t *>(blockDef.Source);

                blocks.emplace_back(
                    std::make_unique<UniformBlockView>(blockDef.BaseAddress,
                                                       blockDef.Length,
                                                       "EQUD",
                                                       formatAddress(repeatedWord, blockDef.DefaultFormat &Asm::FormatterOptions::UseBasicStyleHex),
                                                       blockDef.Length / 4));
            }
            else
            {
                blocks.emplace_back(
                    std::make_unique<CodeBlockView>(blockDef.BaseAddress,
                                                    blockDef.Length / 4,
                                                    blockDef.Source));
            }
        }
        else
        {
            // The block is data.
            blocks.emplace_back(
                std::make_unique<DataBlockView>(blockDef.BaseAddress,
                                                blockDef.Length,
                                                blockDef.Source,
                                                blockDef.DataType));

        }
    }
}

void drawPCMarker(QPainter *painter, const QPointF &origin, double size)
{
    constexpr double Margin = 0.1;
    constexpr double BodyHeight = 0.4;
    constexpr double HeadHeight = 0.8;
    constexpr double HeadProportion = 0.4;
    constexpr double BodyProportion = 1.0 - HeadProportion;
    constexpr double ArrowLength = 1.0 - (Margin * 2);
    constexpr double BodyLength = ArrowLength * BodyProportion;

    //          2
    //  0     1 |\
    //  +-------+ \
    //  |          > 3
    //  +-------+ /
    //  6     5 |/
    //          4

    QPointF vertices[] = {
        { 0.0, BodyHeight * -0.5 },             // 0
        { BodyLength, BodyHeight * -0.5 },      // 1
        { BodyLength, -0.5 * HeadHeight },      // 2
        { ArrowLength, 0.0 },                   // 3
        { BodyLength, 0.5 * HeadHeight },       // 4
        { BodyLength, BodyHeight * 0.5 },       // 5
        { 0.0, BodyHeight * 0.5 }               // 6
    };

    double baseX = origin.x() + (size * Margin);
    double baseY = origin.y() + (size * 0.5);

    for (QPointF &pt : vertices)
    {
        pt.setX((pt.x() * size) + baseX);
        pt.setY((pt.y() * size) + baseY);
    }

    painter->setBrush(Qt::yellow);
    painter->setPen(Qt::black);
    painter->drawPolygon(vertices, static_cast<int>(std::size(vertices)));
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// MemoryViewWidget Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
MemoryViewWidget::MemoryViewWidget(QWidget *ownerObject, const SessionSettings &settings) :
    QAbstractScrollArea(ownerObject),
    _blockContext(settings),
    _emulator(nullptr),
    _extents(0, 0),
    _currentOffset(0)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        connect(&app->getSession(), &EmulatorSession::sessionStarted,
                this, &MemoryViewWidget::onSessionStarted);

        connect(&app->getSession(), &EmulatorSession::sessionEnded,
                this, &MemoryViewWidget::onSessionEnded);

        connect(&app->getSession(), &EmulatorSession::sessionSingleStep,
                this, &MemoryViewWidget::updateStateFromEmulator);
        connect(&app->getSession(), &EmulatorSession::sessionPaused,
                this, &MemoryViewWidget::updateStateFromEmulator);

        connect(&app->getSession(), &EmulatorSession::breakpointsChanged,
                this, &MemoryViewWidget::onBreakpointChange);

        connect(&app->getSession().getSettings(), &SessionSettings::SWIsChanged,
                this, &MemoryViewWidget::onDisplayOptionsChange);

        connect(&app->getSession().getSettings(), &SessionSettings::memoryViewOptionsChanged,
                this, &MemoryViewWidget::onDisplayFormatChange);

        connect(&app->getSession().getSettings(), &SessionSettings::symbolsChanged,
                this, &MemoryViewWidget::onDisplayFormatChange);

        connect(&app->getSession().getSettings(), &SessionSettings::annotationsChanged,
                this, &MemoryViewWidget::onDisplayFormatChange);
    }

    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
            this, &MemoryViewWidget::onColourSchemeChange);
}

void MemoryViewWidget::setLogicallyMappedMemory(MemoryRegion &logicalMemory)
{
    _logicalMemory = logicalMemory;
}

void MemoryViewWidget::displayRegion(const MemoryRegion &region,
                                     uint32_t offset /*= 0*/)
{
    if (_emulator == nullptr)
        return;

    bool updatePending = false;
    bool resetScrollOffsets = false;

    if (_currentRegion != region)
    {
        // We need to renew the memory blocks on display.
        updatePending = true;
        resetScrollOffsets = true;
        _currentRegion = region;

        if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
        {
            // Create blocks to represent the region.
            _blocks.clear();
            _blockContext.resetSizes();
            _extents.setWidth(0);
            _extents.setHeight(0);

            const auto &addrMap = _emulator->getReadAddresses();
            Arm::IAddressRegionPtr addrRegion;
            uint32_t regionOffset, regionRemaining;

            std::vector<Arm::PageMapping> regions;

            if (region.IsLogical)
            {
                // The region uses logical (virtual) addresses and needs to
                // be mapped to physical pages to access the data.
                Arm::PageMapping mapping;

                const uint32_t endAddr = region.BaseAddress + region.Length;

                for (uint32_t currentAddr = region.BaseAddress;
                     currentAddr < endAddr;
                     currentAddr += mapping.PageSize)
                {
                    _emulator->logicalToPhysicalAddress(currentAddr, mapping);

                    if (mapping.PageSize == 0)
                    {
                        // If we don't have a size, we can't decide where the
                        // region ends, so mark the rest of the region as unmapped.
                        mapping.VirtualBaseAddr = currentAddr;
                        mapping.PageBaseAddr = 0;
                        mapping.PageSize = endAddr - currentAddr;
                        mapping.Access = 0;
                    }

                    if (isContiguousMapping(regions, mapping))
                    {
                        // The page is contiguous with the last one in physical memory,
                        // make the last block longer.
                        regions.back().PageSize += mapping.PageSize;
                    }
                    else
                    {
                        // Start a new block.
                        regions.push_back(mapping);
                    }
                }
            }
            else
            {
                // Create a single mapping for the run of physical addresses.
                regions.emplace_back(region.BaseAddress, region.BaseAddress,
                                     region.Length, Arm::PageMapping::IsPresent);
            }

            // Create blocks for each range of addresses, whether mapped or not.
            for (const auto &mapping : regions)
            {
                if ((mapping.Access & Arm::PageMapping::IsPresent) &&
                    addrMap.tryFindRegion(mapping.PageBaseAddr, addrRegion,
                                          regionOffset, regionRemaining) &&
                    (addrRegion->getType() == Arm::RegionType::HostBlock))
                {
                    Arm::IHostBlockPtr hostBlock = static_cast<Arm::IHostBlockPtr>(addrRegion);
                    const uint8_t *hostPtr = Ag::offsetPtr<uint8_t>(hostBlock->getHostAddress(),
                                                                    regionOffset);

                    appendBlocks(_blocks, mapping.VirtualBaseAddr, hostPtr,
                                 regionRemaining, app->getSession().getSettings());
                }
                else
                {
                    // Create an area marked as unmapped.
                    _blocks.emplace_back(
                        std::make_unique<UnmappedBlockView>(mapping.VirtualBaseAddr,
                                                            mapping.PageSize));
                }
            }

            // Update breakpoints within the viewed data.
            onBreakpointChange(&app->getSession());

            // Measure the resultant blocks.
            for (auto &blockPtr : _blocks)
            {
                blockPtr->setVerticalOffset(_extents.height());
                blockPtr->measure(_blockContext);
                const QSizeF &blockSize = blockPtr->getExtents();

                _extents.setWidth(std::max(_extents.width(), blockSize.width()));
                _extents.setHeight(_extents.height() + blockSize.height());
            }

            // Allow space for the margin.
            _extents.setWidth(std::max(_extents.width(),
                                       _blockContext.calculateLeftEdge(BlockViewContext::All)));
        }
    }

    size_t blockIndex;
    uint32_t currentAddr = offset + _currentRegion.BaseAddress;

    if ((updatePending || (_currentOffset != offset)) &&
        tryFindBlockByAddress(currentAddr, blockIndex))
    {
        QRectF addressRect;
        auto *block = _blocks[blockIndex].get();

        if (block->tryFindAddress(_blockContext, currentAddr, addressRect))
        {
            resetScrollOffsets = false;

            QRectF viewRect(getScrollOrigin(), viewport()->size());

            if (!viewRect.intersects(addressRect) ||
                (viewRect.y() > addressRect.y()) ||
                (viewRect.bottom() < addressRect.bottom()))
            {
                // Scroll to the required position.
                horizontalScrollBar()->setValue(0);
                verticalScrollBar()->setValue(Ag::roundDown(addressRect.y()));
                updatePending = true;
            }
        }
    }

    if ((updatePending == false) && (_currentOffset != offset))
    {
        // Update just the margin.
        QPoint scrollOrigin = getScrollOrigin();

        if (scrollOrigin.x() < _blockContext.getMarginWidth())
        {
            QRect marginRect(-scrollOrigin.x(), 0,
                             _blockContext.getMarginWidth() - scrollOrigin.x(),
                             viewport()->height());
            viewport()->update(marginRect);
        }
    }

    _currentOffset = offset;

    if (updatePending)
    {
        onExtentsUpdated(resetScrollOffsets);
        viewport()->update();
    }
}

void MemoryViewWidget::paintEvent(QPaintEvent *args)
{
    QPointF scrollOrigin = getScrollOriginF();

    // At least render the margin.
    QPainter renderer(viewport());
    renderer.setRenderHint(QPainter::Antialiasing, true);
    renderer.setRenderHint(QPainter::TextAntialiasing, true);

    // Draw margin.
    QRectF margin(-scrollOrigin.x(), 0, _blockContext.getMarginWidth(),
                  viewport()->height());

    if (margin.intersects(args->rect()))
    {
        renderer.fillRect(margin, _blockContext.getColour(BlockElementType::Margin));
    }

    renderer.setFont(_blockContext.getCodeFont());

    size_t blockIndex;
    uint32_t currentPC = 0;

    if (_emulator != nullptr)
    {
        currentPC = _emulator->getCoreRegister(Arm::CoreRegister::PC);
    }

    // Find the first block in the update region.
    if (tryFindBlockByPosition(args->rect().y() + scrollOrigin.y(), blockIndex))
    {
        // Iterate over blocks in the update region.
        for (double offsetY = _blocks[blockIndex]->getVerticalOffset() - scrollOrigin.y();
             (offsetY < args->rect().bottom()) && (blockIndex < _blocks.size());
             offsetY += _blocks[blockIndex++]->getExtents().height())
        {
            const MemoryBlockView *block = _blocks[blockIndex].get();

            block->draw(_blockContext, &renderer, scrollOrigin, args->rect());

            paintBreakpoints(&renderer, block);

            QRectF addressRegion;

            if (block->containsAddress(currentPC) &&
                block->tryFindAddress(_blockContext, currentPC, addressRegion))
            {
                QPointF pcOrigin(-scrollOrigin.x(), addressRegion.y() - scrollOrigin.y());

                drawPCMarker(&renderer, pcOrigin, _blockContext.getMarginWidth());
            }
        }
    }
}

void MemoryViewWidget::resizeEvent(QResizeEvent *args)
{
    QAbstractScrollArea::resizeEvent(args);

    // Update the page size for the scroll bars.
    onExtentsUpdated();
}

void MemoryViewWidget::mouseReleaseEvent(QMouseEvent *args)
{
    QAbstractScrollArea::mouseReleaseEvent(args);

    if ((args->flags() & Qt::LeftButton) == 0)
    {
        // The LMB was released.
        QPointF position = args->position() + getScrollOriginF();
        size_t blockIndex;

        if ((position.x() >= 0) &&
            (position.x() < _blockContext.getMarginWidth()) &&
            tryFindBlockByPosition(position.y(), blockIndex))
        {
            // The click ended in the margin.
            CodeBlockView *block = dynamic_cast<CodeBlockView *>(_blocks[blockIndex].get());
            uint32_t address, length;

            if ((block != nullptr) &&
                block->hitTest(_blockContext, position, address, length) &&
                toggleBreakpoint(address, _currentRegion.IsLogical))
            {
                // Redraw margin.
                updateMargin();
            }
        }
    }
}

void MemoryViewWidget::onSessionStarted(const Arm::Options &/*options*/,
                                        Arm::IArmSystem *emulator)
{
    _emulator = emulator;
}

void MemoryViewWidget::onSessionEnded(Arm::IArmSystem */*emulator*/)
{
    _emulator = nullptr;
}

void MemoryViewWidget::updateStateFromEmulator(Arm::IArmSystem *emulator)
{
    Arm::IAddressRegionPtr region;
    Arm::PageMapping mapping;
    uint32_t offset, bytesRemaining;
    uint32_t currentPC = emulator->getCoreRegister(Arm::CoreRegister::PC);

    if (_logicalMemory.containsAddress(currentPC))
    {
        displayRegion(_logicalMemory, currentPC - _logicalMemory.BaseAddress);
    }
    else if (emulator->getReadAddresses().tryFindRegion(currentPC, region,
                                                        offset, bytesRemaining))
    {
        MemoryRegion selectedRegion(currentPC - offset, region->getSize(), false);

        displayRegion(selectedRegion, offset);
    }
}

void MemoryViewWidget::onColourSchemeChange(Qt::ColorScheme /*newColourScheme*/)
{
    if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        _blockContext.reinitialise(app->getSession().getSettings());
        viewport()->update();
    }
}

void MemoryViewWidget::onDisplayOptionsChange()
{
    // Re-measure all blocks.
    if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        _blockContext.reinitialise(app->getSession().getSettings());

        // Remeasure all blocks.
        _blockContext.resetSizes();
        _extents.setWidth(0);
        _extents.setHeight(0);

        for (auto &blockPtr : _blocks)
        {
            blockPtr->setVerticalOffset(_extents.height());
            blockPtr->measure(_blockContext);
            const QSizeF &extents = blockPtr->getExtents();

            _extents.setWidth(std::max(_extents.width(), extents.width()));
            _extents.rheight() += extents.height();
        }

        onExtentsUpdated(false);
        viewport()->update();
    }
}

void MemoryViewWidget::onDisplayFormatChange()
{
    if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        _blockContext.reinitialise(app->getSession().getSettings());

        // TODO: Re-generate all blocks.

        viewport()->update();
    }
}

void MemoryViewWidget::onBreakpointChange(const EmulatorSession *session)
{
    if (_blocks.empty() == false)
    {
        const auto &breakpoints = session->getBreakpoints();
        Breakpoint key(nullptr, _blocks.front()->getBaseAddress(),
                       0, _currentRegion.IsLogical);

        auto bkptPos = std::lower_bound(breakpoints.begin(), breakpoints.end(),
                                        key, Breakpoint::CompareByAddress());
        auto bkptEnd = breakpoints.end();
        auto blockPos = _blocks.begin();
        auto blockEnd = _blocks.end();
        _blocks.front()->clearSubstitutedWords();

        while ((bkptPos != bkptEnd) &&
               (bkptPos->isLogicalAddress() == _currentRegion.IsLogical) &&
               (blockPos != blockEnd))
        {
            MemoryBlockView *blockView = blockPos->get();

            if (bkptPos->getAddress() < blockView->getBaseAddress())
            {
                // The breakpoint is before this block.
                ++bkptPos;
            }
            else if (bkptPos->getAddress() < blockView->getEndAddress())
            {
                // The breakpoint is in this block.
                blockView->addSubstitutedWord(bkptPos->getAddress(),
                                              bkptPos->getInstruction());
                ++bkptPos;
            }
            else
            {
                // The breakpoint is in a subsequent block, move on to the
                // next one.
                ++blockPos;

                if (blockPos != blockEnd)
                {
                    (*blockPos)->clearSubstitutedWords();
                }
            }
        }

        // Clear substitutions from any later blocks.
        if (blockPos != blockEnd)
        {
            for (++blockPos; blockPos != blockEnd; ++blockPos)
            {
                (*blockPos)->clearSubstitutedWords();
            }
        }

        viewport()->update();
    }
}

QPoint MemoryViewWidget::getScrollOrigin() const
{
    return QPoint(horizontalScrollBar()->value(),
                  verticalScrollBar()->value());
}

QPointF MemoryViewWidget::getScrollOriginF() const
{
    return QPointF(horizontalScrollBar()->value(),
                   verticalScrollBar()->value());
}

void MemoryViewWidget::onExtentsUpdated(bool resetScrollOffsets /*= false*/)
{
    QSize viewportArea = viewport()->size();
    QScrollBar *bar = horizontalScrollBar();

    bar->setRange(0, std::max(0, Ag::roundUp(_extents.width()) - viewportArea.width()));
    bar->setPageStep(viewportArea.width());

    if (resetScrollOffsets)
    {
        bar->setValue(0);
    }

    bar = verticalScrollBar();
    bar->setRange(0, std::max(0, Ag::roundUp(_extents.height()) - viewportArea.height()));
    bar->setPageStep(viewportArea.height());
    bar->setSingleStep(Ag::roundUp(_blockContext.getLineHeight()));

    if (resetScrollOffsets)
    {
        bar->setValue(0);
    }
}

void MemoryViewWidget::paintBreakpoints(QPainter *painter,
                                        const MemoryBlockView *block)
{
    QPoint scrollOffset = getScrollOrigin();

    if (scrollOffset.x() >= _blockContext.getMarginWidth())
    {
        // The breakpoints can't be seen anyway.
        return;
    }

    if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        constexpr double Margin = 1.0;
        const auto &breakpoints = app->getSession().getBreakpoints();
        Breakpoint key(nullptr, block->getBaseAddress(), 0, _currentRegion.IsLogical);

        auto bkptPos = std::lower_bound(breakpoints.begin(), breakpoints.end(),
                                        key, Breakpoint::CompareByAddress());

        const double marginSize = _blockContext.getMarginWidth();

        for ( ; (bkptPos != breakpoints.end()) &&
                (bkptPos->getAddress() < block->getEndAddress());
             ++bkptPos)
        {
            QRectF addressRegion;

            if (block->tryFindAddress(_blockContext, bkptPos->getAddress(),
                                      addressRegion))
            {
                QPointF bkptOrigin(-scrollOffset.x(),
                                   addressRegion.top() - scrollOffset.y());

                QRectF bkptRect(bkptOrigin, QSizeF(marginSize, marginSize));
                bkptRect.adjust(Margin, Margin, -Margin, -Margin);

                if (bkptPos->isEnabled())
                {
                    painter->setBrush(Qt::darkRed);
                    painter->setPen(Qt::NoPen);
                }
                else
                {
                    painter->setBrush(Qt::NoBrush);
                    painter->setPen(Qt::darkRed);
                }

                painter->drawEllipse(bkptRect);
            }
        }
    }
}

void MemoryViewWidget::updateMargin()
{
    QPointF scrollOffset = getScrollOriginF();

    if (scrollOffset.x() < _blockContext.getMarginWidth())
    {
        // Calculate the current visual rectangle for the margin.
        QRect margin(0, 0,
                     Ag::roundUp(_blockContext.getMarginWidth() - scrollOffset.x()),
                     viewport()->height());

        viewport()->update(margin);
    }
}

bool MemoryViewWidget::tryFindBlockByAddress(uint32_t address, size_t &index) const
{
    bool isFound = false;
    index = _blocks.size();

    // Configure the key to look up by address.
    KeyBlockView key;
    key.setMemoryKey(address);

    if (_blocks.empty() == false)
    {
        // NOTE: This pointer must be released before destruction.
        MemoryBlockViewUPtr keyPtr(&key);

        try
        {
            // Find the block with a base address greater or equal to offsetY.
            auto pos = std::lower_bound(_blocks.begin(), _blocks.end(), keyPtr,
                                        [](const MemoryBlockViewUPtr &lhs,
                                           const MemoryBlockViewUPtr &rhs) {
                                               return lhs->getBaseAddress() < rhs->getBaseAddress();
                                        });

            if ((pos != _blocks.end()) && (*pos)->containsAddress(address))
            {
                index = std::distance(_blocks.begin(), pos);
                isFound = true;
            }
            else if (pos != _blocks.begin())
            {
                // Try the proceeding block.
                --pos;

                if ((*pos)->containsAddress(address))
                {
                    index = std::distance(_blocks.begin(), pos);
                    isFound = true;
                }
            }
        }
        catch (...)
        {
            keyPtr.release();
            throw;
        }

        // Ensure the pointer is released before attempting to free an object
        // in stack memory.
        keyPtr.release();
    }

    return isFound;
}

bool MemoryViewWidget::tryFindBlockByPosition(double offsetY, size_t &index) const
{
    bool isFound = false;
    index = _blocks.size();
    KeyBlockView key;
    key.setVerticalOffset(offsetY);

    if (_blocks.empty() == false)
    {
        // Configure the key to look up by vertical position.
        // NOTE: This pointer must be released before destruction
        MemoryBlockViewUPtr keyPtr(&key);

        try
        {
            // Find the block with a vertical offset greater or equal to offsetY.
            auto pos = std::lower_bound(_blocks.begin(), _blocks.end(), keyPtr,
                                        [](const MemoryBlockViewUPtr &lhs,
                                           const MemoryBlockViewUPtr &rhs) {
                                               return lhs->getVerticalOffset() < rhs->getVerticalOffset();
                                        });

            if ((pos != _blocks.end()) && (*pos)->containsRow(offsetY))
            {
                index = std::distance(_blocks.begin(), pos);
                isFound = true;
            }
            else if (pos != _blocks.begin())
            {
                // Try the proceeding block.
                --pos;

                if ((*pos)->containsRow(offsetY))
                {
                    index = std::distance(_blocks.begin(), pos);
                    isFound = true;
                }
            }
        }
        catch (...)
        {
            keyPtr.release();
            throw;
        }

        // Ensure the pointer is released before attempting to free an object
        // in stack memory.
        keyPtr.release();
    }

    return isFound;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


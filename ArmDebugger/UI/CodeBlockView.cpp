//! @file ArmDebugger/UI/CodeBlockView.cpp
//! @brief The definition of an implementation of MemoryBlockView which displays
//! disassembled ARM machine code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "CodeBlockView.hpp"

#include <set>

#include "Ag/Core/Format.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/VariantTypes.hpp"
#include "Ag/QtInterop/Conversion.hpp"
#include "AsmTools/InstructionInfo.hpp"

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

static constexpr uint32_t DisasmFlags = Asm::InstructionInfo::ARMv6 |
                                        Asm::InstructionInfo::AllowFPA |
                                        Asm::InstructionInfo::UseStackModesOnR13;
////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// InstructionCacheKey Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
const InstructionCacheKey InstructionCacheKey::First(~static_cast<uint64_t>(0),
                                                     ~static_cast<uint64_t>(0));

InstructionCacheKey::InstructionCacheKey()
{
    std::fill_n(_data, std::size(_data), 0);
}

InstructionCacheKey::InstructionCacheKey(uint64_t first, uint64_t second)
{
    _data[0] = first;
    _data[1] = second;
}

InstructionCacheKey::InstructionCacheKey(const uint32_t *words, uint32_t count)
{
    // Copy up to 4 x 32-bit words.
    uint32_t safeCount = std::min(count, 4u);
    uint32_t *cacheWords = reinterpret_cast<uint32_t *>(_data);

    std::copy_n(words, safeCount, cacheWords);

    // Zero the rest.
    std::fill_n(cacheWords + safeCount, 4u - safeCount, 0u);
}

void InstructionCacheKey::clear()
{
    std::fill_n(_data, std::size(_data), 0);
}

bool InstructionCacheKey::operator==(const InstructionCacheKey &rhs) const
{
    return (_data[0] == rhs._data[0]) &&
        (_data[1] == rhs._data[1]);
}

bool InstructionCacheKey::operator!=(const InstructionCacheKey &rhs) const
{
    return (_data[0] != rhs._data[0]) ||
        (_data[1] != rhs._data[1]);
}

bool InstructionCacheKey::operator<(const InstructionCacheKey &rhs) const
{
    return (_data[1] < rhs._data[1]) ||
        ((_data[1] == rhs._data[1]) && (_data[0] < rhs._data[0]));
}

////////////////////////////////////////////////////////////////////////////////
// DecodedInstruction Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
DecodedInstruction::DecodedInstruction() :
    Key(InstructionCacheKey::First),
    ExecAddress(0),
    InstructionIndex(0),
    InstructionLength(0)
{
}

void DecodedInstruction::reset()
{
    Key = InstructionCacheKey::First;
    ExecAddress = InstructionIndex = InstructionLength = 0;
    Instruction.reset(Asm::InstructionMnemonic::MaxMnemonic,
                      Asm::OperationClass::Max,
                      Asm::ConditionCode::Max);
}

bool DecodedInstruction::tryGetNextInstruction(uint32_t baseAddress, const void *hostBlock, uint32_t length)
{
    bool hasNext = false;
    const uint32_t InstructionCount = length / 4;
    const uint32_t *instructions = reinterpret_cast<const uint32_t *>(hostBlock);

    do
    {
        if ((InstructionLength == 0) && (Key == InstructionCacheKey::First))
        {
            // It's the first after a call to reset.
            ExecAddress = baseAddress;
            InstructionIndex = 0;
        }
        else
        {
            InstructionIndex += InstructionLength;
            ExecAddress = baseAddress + (InstructionIndex * 4);
        }

        if (InstructionIndex < InstructionCount)
        {
            const uint8_t MaxInstructionLength =
                static_cast<uint8_t>(std::min(InstructionCount - InstructionIndex,
                                              4u));

            uint32_t delta = Instruction.disassemble(instructions + InstructionIndex,
                                                     MaxInstructionLength,
                                                     ExecAddress, DisasmFlags);

            InstructionLength = std::max(delta, 1u);

            if (delta > 0)
            {
                hasNext = true;

                Key = InstructionCacheKey(instructions + InstructionIndex, delta);
            }
            else
            {
                Key.clear();
                hasNext = false;
            }
        }

    } while (!hasNext && (InstructionIndex < InstructionCount));

    return hasNext;
}

////////////////////////////////////////////////////////////////////////////////
// CodeBlockView Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
CodeBlockView::CodeBlockView(uint32_t baseAddr, uint32_t wordCount, const void *hostBlock) :
    MemoryBlockView(baseAddr, wordCount * 4),
    _hostBlock(reinterpret_cast<const uint32_t *>(hostBlock)),
    _lineHeight(0),
    _addressWidth(0),
    _mnemonicWidth(0),
    _operandWidth(0),
    _spacing(0)
{
}

// Inherited from MemoryBlockView.
bool CodeBlockView::hitTest(const BlockViewContext &context,
                            const QPointF &position, uint32_t &address,
                            uint32_t &length) const
{
    bool isHit = false;

    if ((position.y() >= getVerticalOffset()) && (position.y() < getVerticalEnd()))
    {
        DecodedInstruction info;
        const double LineWidth = context.calculateLeftEdge(BlockViewContext::All);
        const double blockOffsetY = position.y() - getVerticalOffset();

        for (double offsetY = 0;
             (isHit == false) && (offsetY < blockOffsetY) && tryGetNextInstruction(info);
             offsetY += context.getLineHeight())
        {
            QRectF lineRect(0, getVerticalOffset() + offsetY,
                            LineWidth, context.getLineHeight());

            if (lineRect.contains(position))
            {
                isHit = true;
                address = info.ExecAddress;
                length = info.InstructionLength * 4;
            }
        }
    }

    return isHit;
}

// Inherited from MemoryBlockView.
bool CodeBlockView::tryFindAddress(const BlockViewContext &context,
                                   uint32_t address, QRectF &area) const
{
    bool isFound = false;

    if ((address >= getBaseAddress()) && (address < getEndAddress()))
    {
        DecodedInstruction info;
        int line = 0;

        while ((isFound == false) &&
               tryGetNextInstruction(info) &&
               (info.ExecAddress <= address))
        {
            uint32_t instructionEnd = info.ExecAddress + (info.InstructionLength * 4);

            if (address < instructionEnd)
            {
                isFound = true;
                address = info.ExecAddress;
                area.setX(0);
                area.setY(getVerticalOffset() + (context.getLineHeight() * line));
                area.setWidth(context.calculateLeftEdge(BlockViewContext::All));
                area.setHeight(context.getLineHeight());
            }

            ++line;
        }
    }

    return isFound;
}

// Inherited from MemoryBlockView.
void CodeBlockView::draw(const BlockViewContext &context,
                         QPainter *painter,
                         const QPointF &scrollOrigin,
                         const QRectF &updateRegion) const
{
    DecodedInstruction info;

    // Track the vertical position of each line in client coordinates.
    double clientY = getVerticalOffset() - scrollOrigin.y();

    auto *formatter = context.getInstructionFormatter();
    formatter->setFlags(context.getOptions().getAssemblyFormatFlags());

    while ((clientY < updateRegion.bottom()) &&
            tryGetNextInstruction(info))
    {
        QRectF lineBounds(context.getMarginWidth() - scrollOrigin.x(),
                          clientY, getExtents().width(), _lineHeight);

        if (lineBounds.intersects(updateRegion))
        {
            // Draw the instruction text.
            QPointF textOrigin = lineBounds.topLeft();

            // Adjust as the draw origin is the text baseline.
            textOrigin.ry() += context.getBaselineAdjustment();

            context.paintAddress(painter, textOrigin, info.ExecAddress);

            textOrigin.setX(context.calculateLeftEdge(BlockViewContext::UpToMnemonic) - scrollOrigin.x());

            // Create a formatted string representing the instruction.
            formatter->setInstructionAddress(info.ExecAddress);
            Asm::FormattedInstruction tokens = info.Instruction.format(formatter);

            // Draw each token individually, possibly in a different colour.
            uint32_t tokenCount = tokens.getTokenCount();

            for (uint32_t i = 0; i < tokenCount; ++i)
            {
                auto utf8View = tokens.getTokenText(i);
                QString tokenText = QString::fromUtf8(utf8View.data(), utf8View.length());
                auto &&token = tokens.getToken(i);

                context.prepareToPaint(painter, token.Class);
                painter->drawText(textOrigin, tokenText);
                textOrigin.rx() += context.calculateTextLength(tokenText) + 1.0;

                // Ensure operands are lined up after mnemonics.
                if (token.isMnemonic())
                {
                    textOrigin.setX(context.calculateLeftEdge(BlockViewContext::UpToOperands) - scrollOrigin.x());
                }
            }
        }

        clientY = lineBounds.bottom();
    }
}

// Inherited from MemoryBlockView.
QSizeF CodeBlockView::calculateExtents(BlockViewContext &context)
{
    _lineHeight = context.getLineHeight();
    _addressWidth = context.calculateTextLength("0x00000000:");
    _mnemonicWidth = 0;
    _operandWidth = 0;
    _spacing = context.calculateTextLength("-");
    int lineCount = 0;

    DecodedInstruction info;
    std::set<InstructionCacheKey> measuredInstructions;
    auto *formatter = context.getInstructionFormatter();
    formatter->setFlags(context.getOptions().getAssemblyFormatFlags());

    while (tryGetNextInstruction(info))
    {
        if (measuredInstructions.find(info.Key) == measuredInstructions.end())
        {
            // Prevent the same instruction from being measured twice.
            measuredInstructions.insert(info.Key);

            // Format the resultant instruction as a set of classified tokens.
            formatter->setInstructionAddress(info.ExecAddress);
            Asm::FormattedInstruction tokens = info.Instruction.format(formatter);

            uint16_t operandStart = static_cast<uint16_t>(tokens.getSourceText().getUtf8Length());

            // Iterate through the tokens to find the mnemonic and the first operand token.
            for (uint32_t j = 0; j < tokens.getTokenCount(); ++j)
            {
                const auto &token = tokens.getToken(j);

                if (token.isMnemonic())
                {
                    auto utf8TokenText = tokens.getTokenText(j);
                    QString text = QString::fromUtf8(utf8TokenText.data(), utf8TokenText.size());
                    QSizeF extents = context.calculateTextSize(text);

                    context.maximiseMnemonicWidth(extents.width());

                    _mnemonicWidth = std::max(_mnemonicWidth, extents.width());
                }
                else
                {
                    operandStart = std::min(operandStart, token.Start);
                }
            }

            Ag::string_cref_t source = tokens.getSourceText();
            QString sourceText = QString::fromUtf8(source.getUtf8Bytes() + operandStart,
                                                   source.getUtf8Length() - operandStart);

            QSizeF operandSize = context.calculateTextSize(sourceText);

            context.maximiseOperandWidth(operandSize.width());
            _operandWidth = std::max(_operandWidth, operandSize.width());
        }

        ++lineCount;
    }

    const QSizeF extents(context.calculateLeftEdge(BlockViewContext::All),
                         lineCount * _lineHeight);

    return extents;
}

bool CodeBlockView::tryGetNextInstruction(DecodedInstruction &instruction) const
{
    bool hasNext = false;
    const uint32_t InstructionCount = getLength() / 4;

    do
    {
        if (instruction.InstructionLength == 0)
        {
            // It's the first.
            instruction.ExecAddress = getBaseAddress();
            instruction.InstructionIndex = 0;
        }
        else
        {
            instruction.InstructionIndex += instruction.InstructionLength;
            instruction.ExecAddress = getBaseAddress() + (instruction.InstructionIndex * 4);
        }

        if (instruction.InstructionIndex < InstructionCount)
        {
            auto replacementPos = getReplacements().find(instruction.ExecAddress);
            uint32_t delta = 1;

            if (replacementPos == getReplacements().end())
            {
                delta = instruction.Instruction.disassemble(_hostBlock + instruction.InstructionIndex,
                                                            static_cast<uint8_t>(std::min(InstructionCount - instruction.InstructionIndex, 4u)),
                                                            instruction.ExecAddress,
                                                            DisasmFlags);
            }
            else
            {
                // Disassemble replacement data.
                delta = instruction.Instruction.disassemble(replacementPos->second,
                                                            instruction.ExecAddress,
                                                            DisasmFlags) ? 1 : 0;
            }

            instruction.InstructionLength = std::max(delta, 1u);

            if (delta > 0)
            {
                hasNext = true;

                instruction.Key = InstructionCacheKey(_hostBlock + instruction.InstructionIndex, delta);
            }
            else
            {
                instruction.Key = InstructionCacheKey();
                hasNext = false;
            }
        }

    } while (!hasNext && (instruction.InstructionIndex < InstructionCount));

    return hasNext;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


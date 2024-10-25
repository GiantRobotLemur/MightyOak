//! @file ArmDebugger/UI/CodeBlockView.hpp
//! @brief The declaration of an implementation of MemoryBlockView which displays
//! disassembled ARM machine code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_CODE_BLOCK_VIEW_HPP__
#define __ARM_DEBUGGER_CODE_BLOCK_VIEW_HPP__

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
//! @brief A key representing a unique instruction.
struct InstructionCacheKey
{
public:
    static const InstructionCacheKey First;

    // Construction/Destruction
    InstructionCacheKey();
    InstructionCacheKey(uint64_t first, uint64_t second);
    InstructionCacheKey(const uint32_t *words, uint32_t count);

    // Accessors
    bool isEmpty() const;

    // Operations
    void clear();
    bool operator==(const InstructionCacheKey &rhs) const;
    bool operator!=(const InstructionCacheKey &rhs) const;
    bool operator<(const InstructionCacheKey &rhs) const;
private:
    // Internal Functions

    // Internal Fields
    uint64_t _data[2];
};

struct DecodedInstruction
{
    InstructionCacheKey Key;
    Mo::Asm::InstructionInfo Instruction;
    uint32_t ExecAddress;
    uint32_t InstructionIndex;
    uint32_t InstructionLength;

    DecodedInstruction();
    ~DecodedInstruction() = default;

    // Operations
    void reset();
    bool tryGetNextInstruction(uint32_t baseAddress, const void *hostBlock, uint32_t length);
};

//! @brief An implementation of MemoryBlockView which displays disassembled
//! ARM machine code.
class CodeBlockView : public MemoryBlockView
{
public:
    // Construction/Destruction
    CodeBlockView(uint32_t baseAddr, uint32_t wordCount, const void *hostBlock);
    virtual ~CodeBlockView() = default;

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
    bool tryGetNextInstruction(DecodedInstruction &instruction) const;

    // Internal Fields
    const uint32_t *_hostBlock;
    double _lineHeight;
    double _addressWidth;
    double _mnemonicWidth;
    double _operandWidth;
    double _spacing;

    static constexpr uint32_t DisasmFlags = Asm::InstructionInfo::ARMv6 |
        Asm::InstructionInfo::AllowFPA |
        Asm::InstructionInfo::UseStackModesOnR13;

    static constexpr uint32_t FormatFlags = Asm::FormatterOptions::UseCoreRegAliases;
    static constexpr double Spacing = 4.0;
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

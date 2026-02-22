//! @file ArmEmu/RingBufferTrace.hpp
//! @brief The declaration of a fixed-size circular buffer implementation of
//! IDiagnosticSink for recording emulator trace events.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_RING_BUFFER_TRACE_HPP__
#define __ARM_EMU_RING_BUFFER_TRACE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <ostream>
#include <vector>

#include "Ag/Core/String.hpp"
#include "ArmEmu/IDiagnosticSink.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A fixed-size circular buffer implementation of IDiagnosticSink that
//! records the last N instruction, interrupt, and memory access events.
class RingBufferTrace : public IDiagnosticSink
{
public:
    // Construction/Destruction
    RingBufferTrace(size_t instructionCapacity = 4096,
                    size_t interruptCapacity = 256,
                    size_t memoryAccessCapacity = 1024);
    ~RingBufferTrace() override = default;

    // Accessors
    size_t getInstructionCount() const;
    size_t getInterruptCount() const;
    size_t getMemoryAccessCount() const;

    const InstructionTraceEntry &getInstruction(size_t index) const;
    const InterruptEvent &getInterrupt(size_t index) const;
    const MemoryAccessEntry &getMemoryAccess(size_t index) const;

    // Operations
    void clear();
    void dumpToStream(std::ostream &stream) const;

    // IHardwareDevice Overrides
    Ag::string_cref_t getName() const override;
    Ag::string_cref_t getDescription() const override;

    // IDiagnosticSink Overrides
    void onInstruction(const InstructionTraceEntry &entry) override;
    void onMemoryAccess(const MemoryAccessEntry &entry) override;
    void onInterruptChange(const InterruptEvent &event) override;
    void onModeChange(uint64_t cycleCount, ProcessorMode oldMode,
                      ProcessorMode newMode) override;
private:
    // Internal Types
    template<typename T>
    struct RingBuffer
    {
        std::vector<T> Data;
        size_t WritePos;
        size_t Count;

        RingBuffer(size_t capacity) :
            Data(capacity),
            WritePos(0),
            Count(0)
        {
        }

        void push(const T &item)
        {
            Data[WritePos] = item;
            WritePos = (WritePos + 1) % Data.size();

            if (Count < Data.size())
                ++Count;
        }

        const T &get(size_t index) const
        {
            // Index 0 = oldest, Count-1 = newest.
            size_t start = (Count < Data.size()) ? 0 :  WritePos;
            size_t pos = (start + index) % Data.size();
            return Data[pos];
        }

        void clear()
        {
            WritePos = 0;
            Count = 0;
        }
    };

    // Internal Fields
    Ag::String _name;
    Ag::String _description;
    RingBuffer<InstructionTraceEntry> _instructions;
    RingBuffer<InterruptEvent> _interrupts;
    RingBuffer<MemoryAccessEntry> _memoryAccesses;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

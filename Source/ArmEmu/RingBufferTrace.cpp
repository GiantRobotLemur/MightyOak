//! @file ArmEmu/RingBufferTrace.cpp
//! @brief The definition of a fixed-size circular buffer implementation of
//! IDiagnosticSink.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <iomanip>

#include "ArmEmu/RingBufferTrace.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// RingBufferTrace Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a ring buffer trace sink with specified capacities.
//! @param[in] instructionCapacity Maximum instruction trace entries to retain.
//! @param[in] interruptCapacity Maximum interrupt events to retain.
//! @param[in] memoryAccessCapacity Maximum memory access entries to retain.
RingBufferTrace::RingBufferTrace(size_t instructionCapacity,
                                 size_t interruptCapacity,
                                 size_t memoryAccessCapacity) :
    _name("RingBufferTrace"),
    _description("A fixed-size circular buffer for recording emulator trace events."),
    _instructions(instructionCapacity),
    _interrupts(interruptCapacity),
    _memoryAccesses(memoryAccessCapacity)
{
}

//! @brief Gets the number of instruction trace entries currently stored.
size_t RingBufferTrace::getInstructionCount() const
{
    return _instructions.Count;
}

//! @brief Gets the number of interrupt events currently stored.
size_t RingBufferTrace::getInterruptCount() const
{
    return _interrupts.Count;
}

//! @brief Gets the number of memory access entries currently stored.
size_t RingBufferTrace::getMemoryAccessCount() const
{
    return _memoryAccesses.Count;
}

//! @brief Gets an instruction trace entry by index (0 = oldest).
//! @param[in] index The 0-based index into the ring buffer.
//! @return A reference to the instruction trace entry.
const InstructionTraceEntry &RingBufferTrace::getInstruction(size_t index) const
{
    return _instructions.get(index);
}

//! @brief Gets an interrupt event by index (0 = oldest).
const InterruptEvent &RingBufferTrace::getInterrupt(size_t index) const
{
    return _interrupts.get(index);
}

//! @brief Gets a memory access entry by index (0 = oldest).
const MemoryAccessEntry &RingBufferTrace::getMemoryAccess(size_t index) const
{
    return _memoryAccesses.get(index);
}

//! @brief Clears all stored entries.
void RingBufferTrace::clear()
{
    _instructions.clear();
    _interrupts.clear();
    _memoryAccesses.clear();
}

//! @brief Dumps the trace contents to a text stream.
//! @param[in] stream The output stream to write to.
void RingBufferTrace::dumpToStream(std::ostream &stream) const
{
    stream << "=== Instruction Trace (last " << _instructions.Count << ") ===\n";

    for (size_t i = 0; i < _instructions.Count; ++i)
    {
        const auto &e = _instructions.get(i);
        stream << std::setw(12) << e.CycleCount << "  "
               << "PC=0x" << std::hex << std::setfill('0') << std::setw(8) << e.PC
               << "  Op=0x" << std::setw(8) << e.Opcode
               << "  PSR=0x" << std::setw(8) << e.PSR
               << std::dec << std::setfill(' ')
               << "  cyc=" << static_cast<unsigned>(e.CyclesTaken)
               << (e.WasExecuted ? "  EXEC" : "  skip") << "\n";
    }

    stream << "\n=== Interrupt Events (last " << _interrupts.Count << ") ===\n";

    for (size_t i = 0; i < _interrupts.Count; ++i)
    {
        const auto &e = _interrupts.get(i);
        stream << std::setw(12) << e.CycleCount
               << "  IRQ=0x" << std::hex << std::setfill('0') << std::setw(4) << e.IrqStatus
               << " mask=0x" << std::setw(4) << e.IrqMask
               << "  FIRQ=0x" << std::setw(2) << static_cast<unsigned>(e.FirqStatus)
               << " mask=0x" << std::setw(2) << static_cast<unsigned>(e.FirqMask)
               << std::dec << std::setfill(' ');

        if (e.CpuTookIrq) stream << "  CPU-IRQ";
        if (e.CpuTookFirq) stream << "  CPU-FIRQ";

        stream << "\n";
    }

    stream << "\n=== Memory Accesses (last " << _memoryAccesses.Count << ") ===\n";

    for (size_t i = 0; i < _memoryAccesses.Count; ++i)
    {
        const auto &e = _memoryAccesses.get(i);
        stream << std::setw(12) << e.CycleCount << "  "
               << (e.IsWrite ? "W" : "R")
               << " 0x" << std::hex << std::setfill('0') << std::setw(8) << e.Address
               << " = 0x" << std::setw(8) << e.Value
               << std::dec << std::setfill(' ')
               << " [" << static_cast<unsigned>(e.Size) << "]\n";
    }
}

// Inherited from IHardwareDevice.
Ag::string_cref_t RingBufferTrace::getName() const
{
    return _name;
}

// Inherited from IHardwareDevice.
Ag::string_cref_t RingBufferTrace::getDescription() const
{
    return _description;
}

// Inherited from IDiagnosticSink.
void RingBufferTrace::onInstruction(const InstructionTraceEntry &entry)
{
    _instructions.push(entry);
}

// Inherited from IDiagnosticSink.
void RingBufferTrace::onMemoryAccess(const MemoryAccessEntry &entry)
{
    _memoryAccesses.push(entry);
}

// Inherited from IDiagnosticSink.
void RingBufferTrace::onInterruptChange(const InterruptEvent &event)
{
    _interrupts.push(event);
}

// Inherited from IDiagnosticSink.
void RingBufferTrace::onModeChange(uint64_t /*cycleCount*/,
                                   ProcessorMode /*oldMode*/,
                                   ProcessorMode /*newMode*/)
{
    // Mode changes are not currently tracked in the ring buffer.
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

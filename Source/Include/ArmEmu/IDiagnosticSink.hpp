//! @file ArmEmu/IDiagnosticSink.hpp
//! @brief The declaration of an observer interface for emulator diagnostic
//! events such as instruction traces, interrupt changes, and MMIO accesses.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_DIAGNOSTIC_SINK_HPP__
#define __ARM_EMU_DIAGNOSTIC_SINK_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "ArmEmu/ArmSystem.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Describes a single instruction as it passes through the pipeline.
struct InstructionTraceEntry
{
    //! @brief The master clock cycle count at the time of execution.
    uint64_t CycleCount;

    //! @brief The program counter of the instruction.
    uint32_t PC;

    //! @brief The raw opcode fetched from memory.
    uint32_t Opcode;

    //! @brief The processor status register at execution time.
    uint32_t PSR;

    //! @brief The number of CPU cycles the instruction consumed.
    uint8_t CyclesTaken;

    //! @brief Whether the instruction's condition code passed and it executed.
    bool WasExecuted;
};

//! @brief Describes a memory-mapped I/O access.
struct MemoryAccessEntry
{
    //! @brief The master clock cycle count at the time of access.
    uint64_t CycleCount;

    //! @brief The physical address accessed.
    uint32_t Address;

    //! @brief The value read or written.
    uint32_t Value;

    //! @brief The size of the access in bytes (1, 2, or 4).
    uint8_t Size;

    //! @brief True if this was a write, false if a read.
    bool IsWrite;
};

//! @brief Describes a change in interrupt state.
struct InterruptEvent
{
    //! @brief The master clock cycle count when the event occurred.
    uint64_t CycleCount;

    //! @brief The current IRQ status bits.
    uint16_t IrqStatus;

    //! @brief The current IRQ mask bits.
    uint16_t IrqMask;

    //! @brief The current FIRQ status bits.
    uint8_t FirqStatus;

    //! @brief The current FIRQ mask bits.
    uint8_t FirqMask;

    //! @brief Whether the CPU took an IRQ on this cycle.
    bool CpuTookIrq;

    //! @brief Whether the CPU took a FIRQ on this cycle.
    bool CpuTookFirq;
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An abstract interface for receiving diagnostic events from the
//! emulator.
//! @remarks
//! When attached to a system, the emulator calls these methods
//! during execution to report trace data. When no sink is attached (nullptr),
//! diagnostics are disabled with near-zero overhead.
//!
//! Derives from IHardwareDevice so that instances can be introduced into
//! an emulated system via ArmSystemBuilder::addDevice(). The default
//! connect() implementation calls SystemContext::setDiagnosticSink(this)
//! to register the sink automatically during system initialisation.
class IDiagnosticSink : public IHardwareDevice
{
public:
    // Construction/Destruction
    virtual ~IDiagnosticSink() = default;

    // Inherited from IHardwareDevice.
    void registerDevice(SystemContext &context) override
    {
        context.addDevice(this);
        context.addDeviceAlias(getName(), "DiagnosticSink");
    }

    // Diagnostic Operations
    //! @brief Called after each instruction is fetched and (possibly) executed.
    //! @param[in] entry Details of the instruction that was processed.
    virtual void onInstruction(const InstructionTraceEntry &entry) = 0;

    //! @brief Called when a memory-mapped I/O access occurs.
    //! @param[in] entry Details of the MMIO access.
    virtual void onMemoryAccess(const MemoryAccessEntry &entry) = 0;

    //! @brief Called when the interrupt state changes.
    //! @param[in] event Details of the interrupt state change.
    virtual void onInterruptChange(const InterruptEvent &event) = 0;

    //! @brief Called when the processor mode changes.
    //! @param[in] cycleCount The master clock cycle count.
    //! @param[in] oldMode The previous processor mode.
    //! @param[in] newMode The new processor mode.
    virtual void onModeChange(uint64_t cycleCount, ProcessorMode oldMode,
                              ProcessorMode newMode) = 0;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

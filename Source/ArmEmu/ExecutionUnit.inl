//! @file ArmEmu/ExecutionUnit.inl
//! @brief The declaration of a set of template components which manage the
//! emulated execution of ARM or Thumb instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_EXECUTION_UNIT_INL__
#define __ARM_EMU_EXECUTION_UNIT_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmCore.hpp"
#include "ArmEmu/IDiagnosticSink.hpp"
#include "ArmEmu/WatchpointManager.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief A template implementing execution of ARM instructions in a single
//! mode.
//! @tparam THardware The data type representing the memory map and hardware
//! modelled after GenericHardware.
//! @tparam The data type of the register file the execution unit accesses
//! modelled after GenericCoreRegisterFile.
//! @tparam TPrimaryPipeline The pipeline which executes instructions for the
//! single operating mode the execution units supports modelled on
//! InstructionPipeline.
//! @tparam TAllowDiagnostics Used with if constexpr to determine of
//! IDiagnosticSink branches should be taken - otherwise they will be
//! optimised out.
template<typename THardware, typename TRegisterFile,
         typename TPrimaryPipeline, bool TAllowDiagnostics>
class SingleModeExecutionUnit
{
public:
    // Public Types
    using PrimaryPipeline = TPrimaryPipeline;
    using Hardware = THardware;
    using RegisterFile = TRegisterFile;
    static constexpr bool AllowDiagnostics = TAllowDiagnostics;

private:
    // Internal Fields
    Hardware &_hardware;
    RegisterFile &_regs;
    SystemContext &_context;
    PrimaryPipeline _pipeline;
    IDiagnosticSink *_diagnosticSink;
    WatchpointManager *_watchpointMgr;
    uint64_t _targetCycles;
    bool _hasTargetCycles;

    // Internal Functions
    static void onMaxCyclesElapsed(SystemContext &guestContext,
                                   uintptr_t /*taskContext*/)
    {
        guestContext.getSystem()->raiseHostInterrupt();
    }

public:
    // Construction/Destruction
    //! @brief Constructs an object which runs an instruction pipeline in
    //! particular operating mode.
    //! @param[in] hw The object providing access to the emulated memory map
    //! and hardware.
    //! @param[in] regs The object used to read and write the state of the
    //! emulated processor, possibly via an emulation layer (e.g. 26-bit on
    //! 32-bit).
    //! @param[in] context A pointer to an object which performs system time
    //! keeping and other communication services.
    SingleModeExecutionUnit(Hardware &hw, RegisterFile &regs,
                            SystemContext &context) :
        _hardware(hw),
        _regs(regs),
        _context(context),
        _pipeline(_hardware, _regs),
        _diagnosticSink(nullptr),
        _watchpointMgr(nullptr),
        _targetCycles(0),
        _hasTargetCycles(false)
    {
    }

    // Accessors
    //! @brief Determines if the current PC points to the next instruction
    //! to execute rather than the next instruction to fetch, 8-bytes beyond
    //! due to pipelining.
    //! @retval true The current PC points to the next instruction to execute,
    //! as it might after a executing a branch or a direct write.
    //! @retval false The current PC points to the next instruction to fetch,
    //! 8 bytes beyond the next instruction to execute.
    bool isFlushPending() const { return _pipeline.isFlushPending(); }

    // Operations
    //! @brief Flushes the pre-fetch instruction queue after a direct write to
    //! the PC.
    void flushPipeline() { _pipeline.flushPipeline(); }

    //! @brief Executes instructions until a host or debug interrupt is raised or
    //! after the first run if in single step mode.
    //! @param[in] maxCycles The maximum number of cycles to execute before
    //! returning, negative for infinite, 0 for single step and a positive
    //! value for a cycle limited run.
    //! @returns The count of simulated CPU cycles executed before exit.
    ExecutionMetrics runPipeline(int32_t maxCycles)
    {
        ExecutionMetrics metrics;
        uint64_t startTicks = _context.getCPUClockTicks();

        // Ensure the pipeline only runs once in single-step mode.
        bool runPipeline = true;

        GuestTask raiseHostIrq;
        GuestTask *limitTask = nullptr;

        if (maxCycles < 0)
        {
            // Single step mode. Invalidate the absolute cycle target
            // so the next cycle-limited run starts fresh.
            runPipeline = false;
            metrics.ExecResult = ExecutionMetrics::Result::SingleStep;
            _hasTargetCycles = false;
        }
        else if (maxCycles > 0)
        {
            // Schedule a task to raise a host interrupt at an absolute
            // cycle target. This ensures that N sequential runs of M
            // cycles each produce the same result as a single run of
            // N*M cycles, by compensating for any per-instruction
            // overshoot from previous runs.
            uint64_t currentCycles = _context.getCPUClockTicks();

            if (!_hasTargetCycles)
            {
                // First cycle-limited run or after a step/continue.
                // Start tracking from the current position.
                _targetCycles = currentCycles;
                _hasTargetCycles = true;
            }

            _targetCycles += static_cast<uint64_t>(maxCycles);

            uint64_t delta = (_targetCycles > currentCycles)
                ? (_targetCycles - currentCycles) : 1;

            raiseHostIrq.defineTask(onMaxCyclesElapsed, nullptr);
            limitTask = &raiseHostIrq;

            _context.scheduleTaskDeltaCycles(limitTask,
                                             static_cast<uint32_t>(delta));
        }
        else
        {
            // Infinite/continue mode. Invalidate the cycle target.
            _hasTargetCycles = false;
        }

        _pipeline.flushPipeline();

        // Clear any external interrupts before running.
        _hardware.setDebugIrq(false);
        _hardware.setHostIrq(false);

        // Capture the start time.
        Ag::MonotonicTicks startTime = Ag::HighResMonotonicTimer::getTime();
        uint32_t result = 0;

        do
        {
            // Apply any pending pipeline flush before checking interrupts.
            // This ensures _coreRegisters[15] is in the correct pipelined
            // state (instruction_addr + 8) so that handleIrq()/handleFirq()
            // compute the correct return address in R14.
            _pipeline.applyPendingFlush();

            // Read the state of unmasked IRQs which might upset things.
            uint8_t pendingIrqs = _hardware.getIrqStatus();

            if (pendingIrqs)
            {
                // Clear the stale result from the previous instruction
                // so that processNonExecResult() doesn't re-apply a
                // pipeline flush from a prior branch when only host
                // IRQs are pending.
                result = 0;

                // Process any pending guest IRQs before checking host
                // IRQs. This ensures that when an IOC timer interrupt
                // and a step-limit host IRQ fire on the same cycle, the
                // guest interrupt is taken at the correct time — just
                // as it would be during continuous execution. Without
                // this, stepping can defer guest interrupts, causing
                // non-deterministic execution paths.
                if (pendingIrqs & IrqState::FastIrqPending)
                {
                    // A fast interrupt has been signalled.
                    result = _regs.handleFirq();

                    if constexpr (AllowDiagnostics)
                    {
                        // Notify diagnostic sink of interrupt taken.
                        if (_diagnosticSink != nullptr)
                        {
                            InterruptEvent evt = {};
                            evt.CycleCount = _context.getMasterClockTicks();
                            evt.CpuTookFirq = true;
                            evt.CpuTookIrq = false;
                            _diagnosticSink->onInterruptChange(evt);
                        }
                    }
                }
                else if (pendingIrqs & IrqState::IrqPending)
                {
                    // A normal interrupt has been signalled.
                    result = _regs.handleIrq();

                    // Notify diagnostic sink of interrupt taken.
                    if constexpr (AllowDiagnostics)
                    {
                        if (_diagnosticSink != nullptr)
                        {
                            InterruptEvent evt = {};
                            evt.CycleCount = _context.getMasterClockTicks();
                            evt.CpuTookIrq = true;
                            evt.CpuTookFirq = false;
                            _diagnosticSink->onInterruptChange(evt);
                        }
                    }
                }

                if (pendingIrqs & IrqState::HostIrqsMask)
                {
                    // The host or debugger requested a stop.
                    runPipeline = false;

                    metrics.ExecResult =
                        (pendingIrqs & IrqState::DebugPending) ? ExecutionMetrics::Result::DebugIrq :
                                                                 ExecutionMetrics::Result::HostIrq;
                }

                // Ensure the results are properly applied to the pipeline.
                _pipeline.processNonExecResult(result);
            }
            else // if (pendingIrqs == 0)
            {
                // Decode and execute the next instruction.
                result = _pipeline.executeNext();

                if constexpr (AllowDiagnostics)
                {
                    // Notify diagnostic sink of instruction execution.
                    if (_diagnosticSink != nullptr)
                    {
                        InstructionTraceEntry entry = {};
                        entry.CycleCount = _context.getMasterClockTicks();
                        entry.PC = _pipeline.getLastPC();
                        entry.Opcode = _pipeline.getLastInstruction();
                        entry.PSR = _regs.getPSR();
                        entry.CyclesTaken = static_cast<uint8_t>(result & ExecResult::CycleCountMask);
                        entry.WasExecuted = _pipeline.getLastWasExecuted();
                        _diagnosticSink->onInstruction(entry);
                    }

                    // Check breakpoints after each instruction.
                    if (_watchpointMgr != nullptr &&
                        _watchpointMgr->hasBreakpoints())
                    {
                        if (_watchpointMgr->checkPC(_pipeline.getLastPC()))
                        {
                            _hardware.setDebugIrq(true);
                        }
                    }

                    // Check register watchpoints after each instruction.
                    if (_watchpointMgr != nullptr &&
                        _watchpointMgr->hasRegisterWatchpoints())
                    {
                        for (uint8_t r = 0; r < 16; ++r)
                        {
                            uint32_t val = _regs.getRn(static_cast<GeneralRegister>(r));

                            if (_watchpointMgr->checkRegister(r, val))
                            {
                                _hardware.setDebugIrq(true);
                                break;
                            }
                        }
                    }
                }

                // Update metrics.
                ++metrics.InstructionCount;
                _context.incrementCPUClock(result & ExecResult::CycleCountMask);
            } // if (pendingIrqs == 0)

            // TODO if (result & ExecResult::ModeChange) in a multi-pipeline
            // execution unit, switch pipelines.
        } while (runPipeline);

        // Capture the end time and therefore the duration of the run.
        metrics.ElapsedTime = Ag::HighResMonotonicTimer::getDuration(startTime);
        metrics.CycleCount = _context.getCPUClockTicks() - startTicks;

        // Ensure the PC reflects the next instruction to EXECUTE, not the
        // next one to FETCH.
        _pipeline.unflushPipeline();

        if (limitTask != nullptr)
        {
            // Ensure the time limit task doesn't linger.
            _context.unscheduleTask(limitTask);
        }

        return metrics;
    }

    //! @brief A function called after the SystemContext has been initialised,
    //! but before any execution takes place.
    void connect()
    {
        if constexpr (AllowDiagnostics)
        {
            _context.tryFindTypedDevice("DiagnosticSink", _diagnosticSink);
            _context.tryFindTypedDevice("WatchpointManager", _watchpointMgr);
        }
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

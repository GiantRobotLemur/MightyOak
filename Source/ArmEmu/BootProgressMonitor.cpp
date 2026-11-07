//! @file ArmEmu/BootProgressMonitor.cpp
//! @brief The definition of a diagnostic sink that detects RiscOS boot
//! milestones.
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

#include "ArmEmu/BootProgressMonitor.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

namespace {

// MEMC-based system address ranges for boot detection.
constexpr uint32_t MemcRegionStart = 0x3600000;
constexpr uint32_t MemcRegionEnd   = 0x3800000;
constexpr uint32_t IocRegionStart  = 0x3200000;
constexpr uint32_t IocRegionEnd    = 0x3400000;
constexpr uint32_t VidcRegionStart = 0x3400000;
constexpr uint32_t VidcRegionEnd   = 0x3600000;

// IOC control register address (for I2C detection).
constexpr uint32_t IocCtrlRegAddr  = 0x3200000;

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// BootProgressMonitor Member Definitions
////////////////////////////////////////////////////////////////////////////////
BootProgressMonitor::BootProgressMonitor() :
    _name("BootProgressMonitor"),
    _description("Detects RiscOS boot milestones by observing MMIO and processor state."),
    _memcWriteCount(0),
    _iocAccessCount(0),
    _vidcWriteCount(0),
    _i2cToggleCount(0),
    _instructionCount(0),
    _interruptsTaken(0),
    _seenMemcWrite(false),
    _seenIocAccess(false),
    _seenVidcWrite(false),
    _seenI2cToggle(false),
    _seenFirstModeChange(false),
    _seenInterrupt(false)
{
}

const std::vector<BootProgressMonitor::Milestone> &BootProgressMonitor::getMilestones() const
{
    return _milestones;
}

uint32_t BootProgressMonitor::getMemcWriteCount() const { return _memcWriteCount; }
uint32_t BootProgressMonitor::getIocAccessCount() const { return _iocAccessCount; }
uint32_t BootProgressMonitor::getVidcWriteCount() const { return _vidcWriteCount; }
uint32_t BootProgressMonitor::getI2cToggleCount() const { return _i2cToggleCount; }
uint32_t BootProgressMonitor::getInstructionCount() const { return _instructionCount; }
uint32_t BootProgressMonitor::getInterruptsTaken() const { return _interruptsTaken; }

//! @brief Prints a human-readable summary of boot progress to a stream.
void BootProgressMonitor::printSummary(std::ostream &stream) const
{
    stream << "=== Boot Progress Summary ===\n"
           << "Instructions executed: " << _instructionCount << "\n"
           << "MEMC register writes:  " << _memcWriteCount << "\n"
           << "IOC accesses:          " << _iocAccessCount << "\n"
           << "VIDC register writes:  " << _vidcWriteCount << "\n"
           << "I2C pin toggles:       " << _i2cToggleCount << "\n"
           << "Interrupts taken:      " << _interruptsTaken << "\n"
           << "\nMilestones reached:\n";

    if (_milestones.empty())
    {
        stream << "  (none)\n";
    }
    else
    {
        for (const auto &ms : _milestones)
        {
            stream << "  [cycle " << std::setw(12) << ms.CycleCount << "] "
                   << ms.Description << "\n";
        }
    }
}

void BootProgressMonitor::addMilestone(uint64_t cycleCount, const char *description)
{
    _milestones.push_back({cycleCount, description});
}

// Inherited from IHardwareDevice.
Ag::string_cref_t BootProgressMonitor::getName() const
{
    return _name;
}

// Inherited from IHardwareDevice.
Ag::string_cref_t BootProgressMonitor::getDescription() const
{
    return _description;
}

// Inherited from IDiagnosticSink.
void BootProgressMonitor::onInstruction(const InstructionTraceEntry & /*entry*/)
{
    ++_instructionCount;
}

// Inherited from IDiagnosticSink.
void BootProgressMonitor::onMemoryAccess(const MemoryAccessEntry &entry)
{
    uint32_t addr = entry.Address;

    if (entry.IsWrite)
    {
        if (addr >= MemcRegionStart && addr < MemcRegionEnd)
        {
            ++_memcWriteCount;

            if (!_seenMemcWrite)
            {
                _seenMemcWrite = true;
                addMilestone(entry.CycleCount, "First MEMC register write");
            }
        }
        else if (addr >= VidcRegionStart && addr < VidcRegionEnd)
        {
            ++_vidcWriteCount;

            if (!_seenVidcWrite)
            {
                _seenVidcWrite = true;
                addMilestone(entry.CycleCount, "First VIDC register write");
            }
        }
    }

    if (addr >= IocRegionStart && addr < IocRegionEnd)
    {
        ++_iocAccessCount;

        if (!_seenIocAccess)
        {
            _seenIocAccess = true;
            addMilestone(entry.CycleCount, "First IOC access");
        }

        // Detect I2C toggling on IOC control register (offset 0).
        if (addr == IocCtrlRegAddr && entry.IsWrite)
        {
            ++_i2cToggleCount;

            if (!_seenI2cToggle)
            {
                _seenI2cToggle = true;
                addMilestone(entry.CycleCount, "First I2C/CMOS pin toggle");
            }
        }
    }
}

// Inherited from IDiagnosticSink.
void BootProgressMonitor::onInterruptChange(const InterruptEvent &event)
{
    if (event.CpuTookIrq || event.CpuTookFirq)
    {
        ++_interruptsTaken;

        if (!_seenInterrupt)
        {
            _seenInterrupt = true;
            addMilestone(event.CycleCount, "First interrupt taken by CPU");
        }
    }
}

// Inherited from IDiagnosticSink.
void BootProgressMonitor::onModeChange(uint64_t cycleCount,
                                       ProcessorMode /*oldMode*/,
                                       ProcessorMode newMode)
{
    if (!_seenFirstModeChange)
    {
        _seenFirstModeChange = true;

        const auto &modeInfo = getProcessorModeType();
        std::string desc = "First mode change to ";
        std::string_view modeName = modeInfo.toDisplayName(newMode);
        desc += modeName.empty() ? "unknown" : modeName;
        addMilestone(cycleCount, desc.c_str());
    }
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

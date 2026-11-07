//! @file ArmEmu/BootStallDetector.cpp
//! @brief The definition of a diagnostic sink that detects tight PC loops
//! indicating a boot stall.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iomanip>
#include <set>

#include "AsmTools/InstructionInfo.hpp"
#include "ArmEmu/BootStallDetector.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// BootStallDetector Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a boot stall detector with configurable parameters.
//! @param[in] windowSize The number of recent PC values to track.
//! @param[in] checkInterval How often (in instructions) to check for stalls.
//! @param[in] distinctThreshold Maximum distinct PCs to consider a stall.
BootStallDetector::BootStallDetector(size_t windowSize,
                                     size_t checkInterval,
                                     size_t distinctThreshold) :
    _name("BootStallDetector"),
    _description("Detects tight PC loops indicating a boot stall."),
    _pcWindow(windowSize, 0),
    _windowWritePos(0),
    _windowCount(0),
    _windowSize(windowSize),
    _checkInterval(checkInterval),
    _distinctThreshold(distinctThreshold),
    _instructionCount(0),
    _intervalCounter(0),
    _highWaterPC(0)
{
}

//! @brief Determines whether a stall has been detected.
bool BootStallDetector::isStalled() const
{
    return !_stalls.empty();
}

//! @brief Gets the number of stalls detected so far.
size_t BootStallDetector::getStallCount() const
{
    return _stalls.size();
}

//! @brief Gets the stall information at the given index.
//! @param[in] index The 0-based index of the stall to retrieve.
const BootStallDetector::StallInfo &BootStallDetector::getStall(size_t index) const
{
    return _stalls[index];
}

//! @brief Gets the highest PC value observed during execution.
uint32_t BootStallDetector::getHighWaterPC() const
{
    return _highWaterPC;
}

//! @brief Gets the total number of instructions observed.
uint64_t BootStallDetector::getInstructionCount() const
{
    return _instructionCount;
}

//! @brief Prints a human-readable stall report with disassembled loop
//! instructions.
//! @param[in] stream The output stream to write to.
//! @param[in] system The ARM system to read instruction opcodes from, or
//! nullptr if disassembly from memory is not desired.
void BootStallDetector::printReport(std::ostream &stream,
                                    IArmSystem *system) const
{
    stream << "=== Boot Stall Detector Report ===\n"
           << "Instructions observed: " << _instructionCount << "\n"
           << "High-water PC:         0x" << std::hex << std::setfill('0')
           << std::setw(8) << _highWaterPC << std::dec << std::setfill(' ')
           << "\n"
           << "Stalls detected:       " << _stalls.size() << "\n\n";

    if (_stalls.empty())
    {
        stream << "No stalls detected.\n";
        return;
    }

    Asm::InstructionInfo info;

    for (size_t i = 0; i < _stalls.size(); ++i)
    {
        const StallInfo &stall = _stalls[i];
        stream << "--- Stall #" << (i + 1) << " ---\n"
               << "  Detected at cycle:       " << stall.DetectedAtCycle << "\n"
               << "  Detected at instruction: " << stall.DetectedAtInstruction << "\n"
               << "  Loop length (window):    " << stall.LoopLength << "\n"
               << "  Distinct PCs:            " << stall.LoopPCs.size() << "\n"
               << "  Loop PCs:\n";

        for (uint32_t pc : stall.LoopPCs)
        {
            stream << "    0x" << std::hex << std::setfill('0') << std::setw(8)
                   << pc << std::dec << std::setfill(' ');

            // Attempt to disassemble the instruction at this PC.
            if (system != nullptr)
            {
                uint32_t opcode = 0;
                uint32_t bytesRead = readFromLogicalAddress(system, pc,
                                                            &opcode,
                                                            sizeof(opcode));

                if (bytesRead == sizeof(opcode))
                {
                    if (info.disassemble(opcode, pc,
                                         Asm::InstructionInfo::ARMv2))
                    {
                        Ag::String text = info.toString(pc, 0);
                        stream << "  " << text.getUtf8Bytes();
                    }
                    else
                    {
                        stream << "  DCD 0x" << std::hex << std::setfill('0')
                               << std::setw(8) << opcode
                               << std::dec << std::setfill(' ');
                    }
                }
            }

            stream << "\n";
        }

        stream << "\n";
    }
}

// Inherited from IHardwareDevice.
Ag::string_cref_t BootStallDetector::getName() const
{
    return _name;
}

// Inherited from IHardwareDevice.
Ag::string_cref_t BootStallDetector::getDescription() const
{
    return _description;
}

// Inherited from IDiagnosticSink.
void BootStallDetector::onInstruction(const InstructionTraceEntry &entry)
{
    ++_instructionCount;

    // Track high-water PC.
    if (entry.PC > _highWaterPC)
        _highWaterPC = entry.PC;

    // Record PC in circular window.
    _pcWindow[_windowWritePos] = entry.PC;
    _windowWritePos = (_windowWritePos + 1) % _windowSize;

    if (_windowCount < _windowSize)
        ++_windowCount;

    // Periodically check for stalls.
    ++_intervalCounter;

    if (_intervalCounter >= _checkInterval)
    {
        _intervalCounter = 0;
        checkForStall(entry.CycleCount);
    }
}

// Inherited from IDiagnosticSink.
void BootStallDetector::onMemoryAccess(const MemoryAccessEntry & /*entry*/)
{
    // Not used for stall detection.
}

// Inherited from IDiagnosticSink.
void BootStallDetector::onInterruptChange(const InterruptEvent & /*event*/)
{
    // Not used for stall detection.
}

// Inherited from IDiagnosticSink.
void BootStallDetector::onModeChange(uint64_t /*cycleCount*/,
                                     ProcessorMode /*oldMode*/,
                                     ProcessorMode /*newMode*/)
{
    // Not used for stall detection.
}

//! @brief Checks the PC window for low diversity indicating a stall.
//! @param[in] cycleCount The current master clock cycle count.
void BootStallDetector::checkForStall(uint64_t cycleCount)
{
    if (_windowCount < _windowSize)
        return; // Not enough data yet.

    // Count distinct PCs in the window.
    std::set<uint32_t> distinctPCs(_pcWindow.begin(), _pcWindow.end());

    if (distinctPCs.size() > _distinctThreshold)
        return; // Not a stall.

    // Build the stall info with sorted PCs.
    StallInfo stall;
    stall.DetectedAtCycle = cycleCount;
    stall.DetectedAtInstruction = _instructionCount;
    stall.LoopPCs.assign(distinctPCs.begin(), distinctPCs.end());
    stall.LoopLength = static_cast<uint32_t>(_windowSize);

    // Deduplicate: skip if the last stall has the same set of PCs.
    if (!_stalls.empty())
    {
        const StallInfo &last = _stalls.back();

        if (last.LoopPCs == stall.LoopPCs)
            return; // Same stall, don't duplicate.
    }

    _stalls.push_back(std::move(stall));
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

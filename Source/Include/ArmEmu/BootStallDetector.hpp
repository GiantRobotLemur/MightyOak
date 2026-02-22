//! @file ArmEmu/BootStallDetector.hpp
//! @brief The declaration of a diagnostic sink that detects tight PC loops
//! indicating a boot stall in the emulated system.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_BOOT_STALL_DETECTOR_HPP__
#define __ARM_EMU_BOOT_STALL_DETECTOR_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <ostream>
#include <set>
#include <vector>

#include "Ag/Core/String.hpp"
#include "ArmEmu/IDiagnosticSink.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An IDiagnosticSink implementation that detects tight PC loops —
//! the primary symptom of a boot stall.
//! @details Maintains a circular buffer of recent PC values and periodically
//! checks for low PC diversity, which indicates the processor is stuck in a
//! tight loop. Records stall information including the loop PCs and the
//! high-water-mark PC (furthest into ROM the boot reached).
class BootStallDetector : public IDiagnosticSink
{
public:
    //! @brief Describes a detected stall — a tight PC loop.
    struct StallInfo
    {
        //! @brief The master clock cycle count when the stall was detected.
        uint64_t DetectedAtCycle;

        //! @brief The instruction count when the stall was detected.
        uint64_t DetectedAtInstruction;

        //! @brief The distinct PC values observed in the loop.
        std::vector<uint32_t> LoopPCs;

        //! @brief The number of instructions in the loop window.
        uint32_t LoopLength;
    };

    // Construction/Destruction
    BootStallDetector(size_t windowSize = 32,
                      size_t checkInterval = 1024,
                      size_t distinctThreshold = 8);
    ~BootStallDetector() override = default;

    // Accessors
    bool isStalled() const;
    size_t getStallCount() const;
    const StallInfo &getStall(size_t index) const;
    uint32_t getHighWaterPC() const;
    uint64_t getInstructionCount() const;

    // Operations
    void printReport(std::ostream &stream, IArmSystem *system) const;

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
    // Internal Functions
    void checkForStall(uint64_t cycleCount);

    // Internal Fields
    Ag::String _name;
    Ag::String _description;
    std::vector<uint32_t> _pcWindow;
    std::vector<StallInfo> _stalls;
    size_t _windowWritePos;
    size_t _windowCount;
    size_t _windowSize;
    size_t _checkInterval;
    size_t _distinctThreshold;
    uint64_t _instructionCount;
    uint64_t _intervalCounter;
    uint32_t _highWaterPC;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

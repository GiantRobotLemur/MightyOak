//! @file ArmEmu/BootProgressMonitor.hpp
//! @brief The declaration of a diagnostic sink that detects RiscOS boot
//! milestones by observing MMIO write patterns and processor mode changes.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_BOOT_PROGRESS_MONITOR_HPP__
#define __ARM_EMU_BOOT_PROGRESS_MONITOR_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <ostream>
#include <string>
#include <vector>

#include "Ag/Core/String.hpp"
#include "ArmEmu/IDiagnosticSink.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An IDiagnosticSink implementation that detects RiscOS boot milestones
//! by observing MMIO writes, interrupt patterns, and processor mode changes.
class BootProgressMonitor : public IDiagnosticSink
{
public:
    //! @brief Describes a detected boot milestone.
    struct Milestone
    {
        //! @brief The master clock cycle count when this milestone was detected.
        uint64_t CycleCount;

        //! @brief A human-readable description of the milestone.
        std::string Description;
    };

    // Construction/Destruction
    BootProgressMonitor();
    ~BootProgressMonitor() override = default;

    // Accessors
    const std::vector<Milestone> &getMilestones() const;
    uint32_t getMemcWriteCount() const;
    uint32_t getIocAccessCount() const;
    uint32_t getVidcWriteCount() const;
    uint32_t getI2cToggleCount() const;
    uint32_t getInstructionCount() const;
    uint32_t getInterruptsTaken() const;

    // Operations
    void printSummary(std::ostream &stream) const;

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
    void addMilestone(uint64_t cycleCount, const char *description);

    // Internal Fields
    Ag::String _name;
    Ag::String _description;
    std::vector<Milestone> _milestones;
    uint32_t _memcWriteCount;
    uint32_t _iocAccessCount;
    uint32_t _vidcWriteCount;
    uint32_t _i2cToggleCount;
    uint32_t _instructionCount;
    uint32_t _interruptsTaken;
    bool _seenMemcWrite;
    bool _seenIocAccess;
    bool _seenVidcWrite;
    bool _seenI2cToggle;
    bool _seenFirstModeChange;
    bool _seenInterrupt;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

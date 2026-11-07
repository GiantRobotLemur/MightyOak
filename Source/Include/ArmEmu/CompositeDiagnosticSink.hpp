//! @file ArmEmu/CompositeIDiagnosticSink.hpp
//! @brief The declaration of a diagnostic sink that forwards events to
//! multiple child sinks.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_COMPOSITE_DIAGNOSTIC_SINK_HPP__
#define __ARM_EMU_COMPOSITE_DIAGNOSTIC_SINK_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <initializer_list>
#include <vector>

#include "Ag/Core/String.hpp"
#include "ArmEmu/IDiagnosticSink.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A diagnostic sink that forwards all events to multiple child sinks.
class CompositeDiagnosticSink : public IDiagnosticSink
{
public:
    // Construction/Destruction
    CompositeDiagnosticSink();
    CompositeDiagnosticSink(std::initializer_list<IDiagnosticSink *> sinks);
    ~CompositeDiagnosticSink() override = default;

    // Operations
    void addSink(IDiagnosticSink *sink);
    size_t getSinkCount() const;

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
    // Internal Fields
    Ag::String _name;
    Ag::String _description;
    std::vector<IDiagnosticSink *> _sinks;
};

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions
////////////////////////////////////////////////////////////////////////////////
inline CompositeDiagnosticSink::CompositeDiagnosticSink() :
    _name("CompositeDiagnosticSink"),
    _description("Forwards diagnostic events to multiple child sinks.")
{
}

inline CompositeDiagnosticSink::CompositeDiagnosticSink(
    std::initializer_list<IDiagnosticSink *> sinks) :
    _name("CompositeDiagnosticSink"),
    _description("Forwards diagnostic events to multiple child sinks."),
    _sinks(sinks)
{
}

// Inherited from IHardwareDevice.
inline Ag::string_cref_t CompositeDiagnosticSink::getName() const
{
    return _name;
}

// Inherited from IHardwareDevice.
inline Ag::string_cref_t CompositeDiagnosticSink::getDescription() const
{
    return _description;
}

inline void CompositeDiagnosticSink::addSink(IDiagnosticSink *sink)
{
    _sinks.push_back(sink);
}

inline size_t CompositeDiagnosticSink::getSinkCount() const
{
    return _sinks.size();
}

// Inherited from IDiagnosticSink.
inline void CompositeDiagnosticSink::onInstruction(const InstructionTraceEntry &entry)
{
    for (auto *sink : _sinks)
        sink->onInstruction(entry);
}

// Inherited from IDiagnosticSink.
inline void CompositeDiagnosticSink::onMemoryAccess(const MemoryAccessEntry &entry)
{
    for (auto *sink : _sinks)
        sink->onMemoryAccess(entry);
}

// Inherited from IDiagnosticSink.
inline void CompositeDiagnosticSink::onInterruptChange(const InterruptEvent &event)
{
    for (auto *sink : _sinks)
        sink->onInterruptChange(event);
}

// Inherited from IDiagnosticSink.
inline void CompositeDiagnosticSink::onModeChange(uint64_t cycleCount,
                                                   ProcessorMode oldMode,
                                                   ProcessorMode newMode)
{
    for (auto *sink : _sinks)
        sink->onModeChange(cycleCount, oldMode, newMode);
}

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmDebugger/Annotations/InstructionFormat.hpp
//! @brief The declaration of an annotation which alters the way in which
//! instructions are interpreted for display to the user.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_INSTRUCTION_FORMAT_HPP__
#define __ARM_DEBUGGER_INSTRUCTION_FORMAT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Annotations/Annotation.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An annotation which alters the way in which instructions are
//! interpreted for display to the user.
class InstructionFormat : public Annotation
{
public:
    // Construction/Destruction
    InstructionFormat();
    InstructionFormat(uint32_t baseAddr, uint32_t length,
                      uint32_t asmFormatterOptions);
    virtual ~InstructionFormat() = default;

    // Accessors
    uint32_t getDisassemblyFormatOptions() const;

    // Operations

    // Overrides
    virtual QJsonObject write() const override;
    virtual void read(const QJsonObject &jsonObj) override;

private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    uint32_t _options;
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

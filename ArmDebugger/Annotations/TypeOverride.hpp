//! @file ArmDebugger/Annotations/TypeOverride.hpp
//! @brief The declaration of an annotation which specifies the data type of
//! a run of addresses in emulated memory.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_TYPE_OVERRIDE_HPP__
#define __ARM_DEBUGGER_TYPE_OVERRIDE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/Options.hpp"

#include "Annotations/Annotation.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An annotation which specifies the data type of a run of addresses in
//! emulated memory.
class TypeOverride : public Annotation
{
public:
    // Construction/Destruction
    TypeOverride();
    TypeOverride(uint32_t startAddr, uint32_t length,
                 Asm::DirectiveDataType dataType);
    virtual ~TypeOverride() = default;

    // Accessors
    Asm::DirectiveDataType getOverrideType() const;

    // Operations

    // Overrides
    virtual QJsonObject write() const override;
    virtual void read(const QJsonObject &jsonObj) override;
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    Asm::DirectiveDataType _dataType;
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

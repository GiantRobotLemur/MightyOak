//! @file ArmDebugger/Breakpoint.hpp
//! @brief The declaration of a class which represents a breakpoint set in the
//! emulated system by replacing an instruction with BKPT.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_BREAKPOINT_HPP__
#define __ARM_DEBUGGER_BREAKPOINT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <vector>

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

namespace Arm {
class IArmSystem;
} // namespace Arm

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A class which represents a breakpoint set in the emulated system by
//! replacing an instruction with BKPT.
class Breakpoint
{
public:
    // Public Types
    struct CompareById
    {
        bool operator()(const Breakpoint &lhs, const Breakpoint &rhs) const;
    };

    struct CompareByAddress
    {
        bool operator()(const Breakpoint &lhs, const Breakpoint &rhs) const;
    };

    // Construction/Destruction
    Breakpoint();
    Breakpoint(Arm::IArmSystem *emulator, uint32_t address,
               uint16_t id, bool isLogicalAddr);
    ~Breakpoint() = default;

    // Accessors
    bool isValid() const;
    bool isEnabled() const;
    bool isLogicalAddress() const;
    uint32_t getAddress() const;
    uint32_t getInstruction() const;
    uint16_t getBreakpointID() const;

    // Operations
    bool apply();
    void remove();
private:
    // Internal Fields
    uint32_t *_hostAddress;
    uint32_t _address;
    uint32_t _originalInstruction;
    uint16_t _breakpointID;
    bool _isLogicalAddress;
    bool _isSet;
};

using BreakpointCollection = std::vector<Breakpoint>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

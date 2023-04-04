//! @file ArmEmu/ArithmeticLogicUnit.hpp
//! @brief The declaration of a layer of emulation which provides the arithmetic
//! and logical operations of an ARM processor core.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ARITHMETIC_LOGIC_UNIT_HPP__
#define __ARM_EMU_ARITHMETIC_LOGIC_UNIT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "RegisterFile.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
enum InstructionResult : uint32_t
{
    IR_CycleCountMask   = 0x00FF,
    IR_FlushPipeline    = 0x0100,
};


////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines some fixed operations which perform arithmetic or logical
//! operations solely on register contents.
class ArithmeticLogicUnit : public RegisterFile
{
protected:
    uint32_t logicResultStatus(uint32_t result, uint8_t carryOut) const noexcept;
    static constexpr uint32_t addResultStatus(uint32_t op1, uint32_t op2, uint32_t result) noexcept;
    static constexpr uint32_t subResultStatus(uint32_t op1, uint32_t op2, uint32_t result) noexcept;

    uint32_t calculateConstantAluOperand(uint32_t instruction) const;
    uint32_t calculateShiftedAluOperand(uint32_t instruction, uint8_t &carryOut) const;
    uint32_t calculateDataTransferOffset(uint32_t instruction) const;
public:
    // Construction/Destruction
    ArithmeticLogicUnit() = default;
    virtual ~ArithmeticLogicUnit() = default;

    // Accessors

    // Operations
    uint32_t execDataProcOpStatus(uint32_t instruction, uint32_t op2, uint8_t carryOut) noexcept;
    uint32_t execDataProcOp(uint32_t instruction, uint32_t op2) noexcept;
    uint32_t execMultiply(uint32_t instruction) noexcept;
    uint32_t execLongMultiply(uint32_t instruction) noexcept;
    uint32_t execBranch(uint32_t instruction) noexcept;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

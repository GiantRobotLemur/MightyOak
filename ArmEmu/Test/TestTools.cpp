//! @file ArmEmu/TestTools.cpp
//! @brief The definition of various tools used in unit tests for the ARM
//! processor emulator.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "TestTools.hpp"

#include "Ag/Core/Format.hpp"
#include "Ag/Core/Variant.hpp"

#include "ArmSystem.inl"
#include "SystemConfigurations.inl"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// GenerateBreakPoint Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to produce sequential break point instructions.
GenerateBreakPoint::GenerateBreakPoint() :
    _index(0)
{
    _instruction.reset(Asm::InstructionMnemonic::Bkpt,
                        Asm::OperationClass::Breakpoint);
}

//! @brief Generates a BKPT instruction with a new comment value.
//! @return An ARM BKPT instruction.
uint32_t GenerateBreakPoint::operator()()
{
    _instruction.getBreakpointParameters().Comment = _index++;

    Ag::String error;
    uint32_t instruction = 0;

    if (_instruction.assemble(instruction, 0x0000, error) == false)
    {
        std::string buffer("Could not assemble numbered breakpoint:");
        Ag::appendAgString(buffer, error);

        throw Ag::OperationException(buffer.c_str());
    }

    return instruction;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
::testing::AssertionResult isEqualHex(uint8_t lhs, uint8_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X2} vs 0x{1:X2}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualHex(uint16_t lhs, uint16_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X4} vs 0x{1:X4}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualHex(uint32_t lhs, uint32_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X8} vs 0x{1:X8}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualHex(uint64_t lhs, uint64_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X8} vs 0x{1:X8}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualReg(CoreRegister regId, uint32_t lhs, uint32_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "R{0}: 0x{1:X8} vs 0x{2:X8}",
                         message, { Ag::toScalar(regId), lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualMemWord(uint32_t addr, uint32_t lhs, uint32_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "MEM 0x{0:X6}: 0x{1:X8} vs 0x{2:X8}",
                         message, { addr, lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


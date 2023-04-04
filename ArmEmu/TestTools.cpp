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

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
::testing::AssertionResult isEqualHex(uint32_t lhs, uint32_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        appendFormat(FormatInfo::getNeutral(),
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
        appendFormat(FormatInfo::getNeutral(),
                     "{0}: 0x{1:X8} vs 0x{2:X8}",
                     message, { coreRegisterToString(regId), lhs, rhs });

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
        appendFormat(FormatInfo::getNeutral(),
                     "MEM 0x{0:X6}: 0x{1:X8} vs 0x{2:X8}",
                     message, { addr, lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


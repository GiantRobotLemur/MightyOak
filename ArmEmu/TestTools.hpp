//! @file ArmEmu/TestTools.hpp
//! @brief The declaration of various tools used in unit tests for the ARM
//! processor emulator.
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_TEST_TOOLS_HPP__
#define __ARM_EMU_TEST_TOOLS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "AsmTools.hpp"
#include "ArmEmu.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
::testing::AssertionResult isEqualHex(uint8_t lhs, uint8_t rhs);
::testing::AssertionResult isEqualHex(uint16_t lhs, uint16_t rhs);
::testing::AssertionResult isEqualHex(uint32_t lhs, uint32_t rhs);
::testing::AssertionResult isEqualHex(uint64_t lhs, uint64_t rhs);
::testing::AssertionResult isEqualReg(CoreRegister regId, uint32_t lhs, uint32_t rhs);
::testing::AssertionResult isEqualMemWord(uint32_t addr, uint32_t lhs, uint32_t rhs);

IArmSystemUPtr createUserModeTestSystem(const char *assembler);

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

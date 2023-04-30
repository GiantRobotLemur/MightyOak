//! @file Test_Constraints.cpp
//! @brief The definition of constraint tools used in other unit tests (very
//! meta).
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/Core/Utils.hpp"
#include "ArmEmu.hpp"

#include "TestConstraints.hpp"

namespace Ag {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Constraints, ParseEmpty)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints("", items));
    EXPECT_TRUE(parseConstraints("   \t  ", items));
    EXPECT_TRUE(parseConstraints("\n\r\n\t ", items));
}

GTEST_TEST(Constraints, ParseCoreRegister)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints("R4=0xDEADBEEF,\tA2=0b11 \n v4 = 11,r3", items));
    ASSERT_EQ(items.size(), 4u);

    EXPECT_EQ(items[0].Location, SystemElement::CoreRegister);
    EXPECT_EQ(items[0].Index, 1);
    EXPECT_EQ(items[0].SubIndex, 0);
    EXPECT_EQ(items[0].Value, 3u);

    EXPECT_EQ(items[1].Location, SystemElement::CoreRegister);
    EXPECT_EQ(items[1].Index, 3);
    EXPECT_EQ(items[1].SubIndex, 0);
    EXPECT_EQ(items[1].Value, 0u);

    EXPECT_EQ(items[2].Location, SystemElement::CoreRegister);
    EXPECT_EQ(items[2].Index, 4);
    EXPECT_EQ(items[2].SubIndex, 0);
    EXPECT_EQ(items[2].Value, 0xDEADBEEFu);

    EXPECT_EQ(items[3].Location, SystemElement::CoreRegister);
    EXPECT_EQ(items[3].Index, 7);
    EXPECT_EQ(items[3].SubIndex, 0);
    EXPECT_EQ(items[3].Value, 11u);
}

GTEST_TEST(Constraints, ParseCoProcRegisters)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints("CP2,CR5=9,CR3=4,CP9,CP11=3,CR0=7", items));
    ASSERT_EQ(items.size(), 3u);

    EXPECT_EQ(items[0].Location, SystemElement::CoProcRegister);
    EXPECT_EQ(items[0].Index, 2);
    EXPECT_EQ(items[0].SubIndex, 3);
    EXPECT_EQ(items[0].Value, 4u);

    EXPECT_EQ(items[1].Location, SystemElement::CoProcRegister);
    EXPECT_EQ(items[1].Index, 2);
    EXPECT_EQ(items[1].SubIndex, 5);
    EXPECT_EQ(items[1].Value, 9u);

    EXPECT_EQ(items[2].Location, SystemElement::CoProcRegister);
    EXPECT_EQ(items[2].Index, 11);
    EXPECT_EQ(items[2].SubIndex, 0);
    EXPECT_EQ(items[2].Value, 7u);
}

GTEST_TEST(Constraints, ParseFpaRegisters)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints("F7=11,F0=9", items));
    ASSERT_EQ(items.size(), 2u);

    EXPECT_EQ(items[0].Location, SystemElement::FpaRegister);
    EXPECT_EQ(items[0].Index, 0);
    EXPECT_EQ(items[0].SubIndex, 0);
    EXPECT_EQ(items[0].Value, 9u);

    EXPECT_EQ(items[1].Location, SystemElement::FpaRegister);
    EXPECT_EQ(items[1].Index, 7);
    EXPECT_EQ(items[1].SubIndex, 0);
    EXPECT_EQ(items[1].Value, 11u);
}

GTEST_TEST(Constraints, ParseSystemRegisters)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints("PC=0x8000,CPSR=0x0C000003,SPSR=1\n"
                                 "Mode=Usr32,IrqStatus=3,IrqMask=0015",
                                 items));
    ASSERT_EQ(items.size(), 6u);
    EXPECT_EQ(items[0].Location, SystemElement::SystemRegister);
    EXPECT_EQ(items[0].Index, toScalar(SystemRegister::PC));
    EXPECT_EQ(items[0].SubIndex, 0);
    EXPECT_EQ(items[0].Value, 0x8000u);

    EXPECT_EQ(items[1].Location, SystemElement::SystemRegister);
    EXPECT_EQ(items[1].Index, toScalar(SystemRegister::CPSR));
    EXPECT_EQ(items[1].SubIndex, 0);
    EXPECT_EQ(items[1].Value, 0x0C000003u);

    EXPECT_EQ(items[2].Location, SystemElement::SystemRegister);
    EXPECT_EQ(items[2].Index, toScalar(SystemRegister::SPSR));
    EXPECT_EQ(items[2].SubIndex, 0);
    EXPECT_EQ(items[2].Value, 1u);

    EXPECT_EQ(items[3].Location, SystemElement::SystemRegister);
    EXPECT_EQ(items[3].Index, toScalar(SystemRegister::ProcessorMode));
    EXPECT_EQ(items[3].SubIndex, 0);
    EXPECT_EQ(items[3].Value, toScalar(ProcessorMode::User32));

    EXPECT_EQ(items[4].Location, SystemElement::SystemRegister);
    EXPECT_EQ(items[4].Index, toScalar(SystemRegister::IrqStatus));
    EXPECT_EQ(items[4].SubIndex, 0);
    EXPECT_EQ(items[4].Value, 3u);

    EXPECT_EQ(items[5].Location, SystemElement::SystemRegister);
    EXPECT_EQ(items[5].Index, toScalar(SystemRegister::IrqMask));
    EXPECT_EQ(items[5].SubIndex, 0);
    EXPECT_EQ(items[5].Value, 15u);
}

} // Anonymous namespace

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


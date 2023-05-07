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

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Constraints, ParseEmpty)
{
    ConstraintCollection items;
    

    EXPECT_TRUE(parseConstraints(TLOC, "", items));
    EXPECT_TRUE(parseConstraints(TLOC, "   \t  ", items));
    EXPECT_TRUE(parseConstraints(TLOC, "\n\r\n\t ", items));
}

GTEST_TEST(Constraints, ParseCoreRegister)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints(TLOC, "R4=0xDEADBEEF,\tA2=0b11 \n v4 = 11,r3", items));
    ASSERT_EQ(items.size(), 4u);

    EXPECT_EQ(items[0].Element, SystemElement::CoreRegister);
    EXPECT_EQ(items[0].ElementIndex, 1u);
    EXPECT_EQ(items[0].Value, 3u);

    EXPECT_EQ(items[1].Element, SystemElement::CoreRegister);
    EXPECT_EQ(items[1].ElementIndex, 3u);
    EXPECT_EQ(items[1].Value, 0u);

    EXPECT_EQ(items[2].Element, SystemElement::CoreRegister);
    EXPECT_EQ(items[2].ElementIndex, 4u);
    EXPECT_EQ(items[2].Value, 0xDEADBEEFu);

    EXPECT_EQ(items[3].Element, SystemElement::CoreRegister);
    EXPECT_EQ(items[3].ElementIndex, 7u);
    EXPECT_EQ(items[3].Value, 11u);
}

GTEST_TEST(Constraints, ParseCoProcRegisters)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints(TLOC, "CP2,CR5=9,CR3=4,CP9,CP11=3,CR0=7", items));
    ASSERT_EQ(items.size(), 3u);

    EXPECT_EQ(items[0].Element, SystemElement::CoProcRegister);
    EXPECT_EQ(items[0].ElementIndex, 0x23u);
    EXPECT_EQ(items[0].Value, 4u);

    EXPECT_EQ(items[1].Element, SystemElement::CoProcRegister);
    EXPECT_EQ(items[1].ElementIndex, 0x25u);
    EXPECT_EQ(items[1].Value, 9u);

    EXPECT_EQ(items[2].Element, SystemElement::CoProcRegister);
    EXPECT_EQ(items[2].ElementIndex, 0xB0u);
    EXPECT_EQ(items[2].Value, 7u);
}

GTEST_TEST(Constraints, ParseFpaRegisters)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints(TLOC, "F7=11,F0=9", items));
    ASSERT_EQ(items.size(), 2u);

    EXPECT_EQ(items[0].Element, SystemElement::FpaRegister);
    EXPECT_EQ(items[0].ElementIndex, 0u);
    EXPECT_EQ(items[0].Value, 9u);

    EXPECT_EQ(items[1].Element, SystemElement::FpaRegister);
    EXPECT_EQ(items[1].ElementIndex, 7u);
    EXPECT_EQ(items[1].Value, 11u);
}

GTEST_TEST(Constraints, ParseSystemRegisters)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints(TLOC, "PC=0x8000,CPSR=0x0C000003,SPSR=1\n"
                                 "Mode=Usr32,IrqStatus=3,IrqMask=0015\n"
                                 "Status=CVNZ",
                                 items));
    ASSERT_EQ(items.size(), 7u);
    EXPECT_EQ(items[0].Element, SystemElement::SystemRegister);
    EXPECT_EQ(items[0].ElementIndex, Ag::toScalar(SystemRegister::PC));
    EXPECT_EQ(items[0].Value, 0x8000u);

    EXPECT_EQ(items[1].Element, SystemElement::SystemRegister);
    EXPECT_EQ(items[1].ElementIndex, Ag::toScalar(SystemRegister::CPSR));
    EXPECT_EQ(items[1].Value, 0x0C000003u);

    EXPECT_EQ(items[2].Element, SystemElement::SystemRegister);
    EXPECT_EQ(items[2].ElementIndex, Ag::toScalar(SystemRegister::SPSR));
    EXPECT_EQ(items[2].Value, 1u);

    EXPECT_EQ(items[3].Element, SystemElement::SystemRegister);
    EXPECT_EQ(items[3].ElementIndex, Ag::toScalar(SystemRegister::Status));
    EXPECT_EQ(items[3].Value, 15u);

    EXPECT_EQ(items[4].Element, SystemElement::SystemRegister);
    EXPECT_EQ(items[4].ElementIndex, Ag::toScalar(SystemRegister::ProcessorMode));
    EXPECT_EQ(items[4].Value, Ag::toScalar(ProcessorMode::User32));

    EXPECT_EQ(items[5].Element, SystemElement::SystemRegister);
    EXPECT_EQ(items[5].ElementIndex, Ag::toScalar(SystemRegister::IrqStatus));
    EXPECT_EQ(items[5].Value, 3u);

    EXPECT_EQ(items[6].Element, SystemElement::SystemRegister);
    EXPECT_EQ(items[6].ElementIndex, Ag::toScalar(SystemRegister::IrqMask));
    EXPECT_EQ(items[6].Value, 15u);
}

GTEST_TEST(Constraints, ParseAddresses)
{
    ConstraintCollection items;

    EXPECT_TRUE(parseConstraints(TLOC, "PBYTE[11]=1,PHWORD[CAFE]=2,PWORD[BEEF]=3\n"
                                 "LBYTE[3FFFFFF]=4,LHWORD[8000]=5,LWORD[10000]=6\n"
                                 "Map[C000]",
                                 items));
    ASSERT_EQ(items.size(), 7u);
    EXPECT_EQ(items[0].Element, SystemElement::PhysicalByte);
    EXPECT_EQ(items[0].ElementIndex, 0x11u);
    EXPECT_EQ(items[0].Value, 1u);

    EXPECT_EQ(items[1].Element, SystemElement::PhysicalHalfWord);
    EXPECT_EQ(items[1].ElementIndex, 0xCAFEu);
    EXPECT_EQ(items[1].Value, 2u);

    EXPECT_EQ(items[2].Element, SystemElement::PhysicalWord);
    EXPECT_EQ(items[2].ElementIndex, 0xBEEFu);
    EXPECT_EQ(items[2].Value, 3u);

    EXPECT_EQ(items[3].Element, SystemElement::LogicalByte);
    EXPECT_EQ(items[3].ElementIndex, 0x3FFFFFFu);
    EXPECT_EQ(items[3].Value, 4u);

    EXPECT_EQ(items[4].Element, SystemElement::LogicalHalfWord);
    EXPECT_EQ(items[4].ElementIndex, 0x8000u);
    EXPECT_EQ(items[4].Value, 5u);

    EXPECT_EQ(items[5].Element, SystemElement::LogicalWord);
    EXPECT_EQ(items[5].ElementIndex, 0x10000u);
    EXPECT_EQ(items[5].Value, 6u);

    EXPECT_EQ(items[6].Element, SystemElement::MappedPhysicalAddress);
    EXPECT_EQ(items[6].ElementIndex, 0xC000u);
    EXPECT_EQ(items[6].Value, 0u);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


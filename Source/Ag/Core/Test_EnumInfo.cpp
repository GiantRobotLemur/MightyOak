//! @file Test_EnumInfo.cpp
//! @brief The definition of unit tests for the EnumInfo template class.
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

#include "Ag/Core/EnumInfo.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum class MyValues {
    This,
    That,
    TheOther,
    Max,

    Undefined,
};

using MyValuesInfo = EnumInfo<MyValues>;

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(EnumInfo, BasicUsage)
{
    MyValuesInfo specimen({
        { MyValues::This, "THIS", "This", "The first element" },
        { MyValues::That, "THAT", "That", "The second element" },
        { MyValues::TheOther, "OTHER", "Other", "The other element" },
        { MyValues::Max, "MAX" },
    });

    EXPECT_FALSE(specimen.getSymbols().empty());
    EXPECT_STREQ(specimen.toString(MyValues::This).data(), "THIS");
    EXPECT_STREQ(specimen.toDisplayName(MyValues::That).data(), "That");
    EXPECT_STREQ(specimen.getDescription(MyValues::TheOther).data(), "The other element");

    EXPECT_TRUE(specimen.toString(MyValues::Undefined).empty());
    EXPECT_TRUE(specimen.toDisplayName(MyValues::Undefined).empty());
    EXPECT_TRUE(specimen.getDescription(MyValues::Undefined).empty());

    MyValues value = MyValues::Undefined;
    EXPECT_TRUE(specimen.tryParse("MAX", value));
    EXPECT_EQ(value, MyValues::Max);

    EXPECT_FALSE(specimen.tryParse("UNDEFINED", value));
    EXPECT_FALSE(specimen.tryParse(std::string_view(), value));

    EXPECT_EQ(specimen.parse("THAT", MyValues::TheOther), MyValues::That);
    EXPECT_EQ(specimen.parse("Nothing", MyValues::TheOther), MyValues::TheOther);
    EXPECT_EQ(specimen.parse(std::string_view(), MyValues::TheOther), MyValues::TheOther);
}

GTEST_TEST(EnumInfo, BadUsage)
{
    EXPECT_THROW(MyValuesInfo({ }), ArgumentException);

    EXPECT_THROW(MyValuesInfo specimen({
        { MyValues::This, "THIS" },
        { MyValues::That, "THIS" },
    }), OperationException);

    EXPECT_THROW(MyValuesInfo specimen({
        { MyValues::This, "THIS" },
        { MyValues::This, "THAT" },
    }), OperationException);
}


} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


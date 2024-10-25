//! @file Test_Value.cpp
//! @brief The definition of unit tests for the Value class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/GTest_Core.hpp"
#include "Ag/Core/Exception.hpp"
#include "Value.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Value, DefaultsToNull)
{
    Value specimen;

    EXPECT_TRUE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::Unknown);

    EXPECT_THROW(specimen.asInt32(), Ag::BadCastException);
}

GTEST_TEST(Value, ConstructInt32)
{
    const int32_t value = 42;
    Value specimen(value);

    EXPECT_FALSE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::Int32);

    EXPECT_NO_THROW(specimen.asInt32());
    EXPECT_THROW(specimen.asUint32(), Ag::BadCastException);
}

GTEST_TEST(Value, ConstructUint32)
{
    const uint32_t value = 42;
    Value specimen(value);

    EXPECT_FALSE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::Uint32);

    EXPECT_NO_THROW(specimen.asUint32());
    EXPECT_THROW(specimen.asFloat(), Ag::BadCastException);
}

GTEST_TEST(Value, ConstructInt64)
{
    const int64_t value = -666596;
    Value specimen(value);

    EXPECT_FALSE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::Int64);

    EXPECT_NO_THROW(specimen.asInt64());
    EXPECT_THROW(specimen.asUint64(), Ag::BadCastException);
}

GTEST_TEST(Value, ConstructUint64)
{
    const uint64_t value = 4296996;
    Value specimen(value);

    EXPECT_FALSE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::Uint64);

    EXPECT_NO_THROW(specimen.asUint64());
    EXPECT_THROW(specimen.asFloat(), Ag::BadCastException);
}

GTEST_TEST(Value, ConstructFloat)
{
    const float value = -12.3f;
    Value specimen(value);

    EXPECT_FALSE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::Float);

    EXPECT_NO_THROW(specimen.asFloat());
    EXPECT_THROW(specimen.asDouble(), Ag::BadCastException);
}

GTEST_TEST(Value, ConstructDouble)
{
    const double value = 3.14159357;
    Value specimen(value);

    EXPECT_FALSE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::Double);

    EXPECT_NO_THROW(specimen.asDouble());
    EXPECT_THROW(specimen.asInt64(), Ag::BadCastException);
}

GTEST_TEST(Value, ConstructCharacter)
{
    const char32_t value = U'?';
    Value specimen(value);

    EXPECT_FALSE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::Character);

    EXPECT_NO_THROW(specimen.asCharacter());
    EXPECT_THROW(specimen.asInt64(), Ag::BadCastException);
}

GTEST_TEST(Value, ConstructString)
{
    const Ag::String value = "Hello World!";
    Value specimen(value);

    EXPECT_FALSE(specimen.isNull());
    EXPECT_EQ(specimen.getDataType(), DataType::String);

    EXPECT_NO_THROW(specimen.asString());
    EXPECT_THROW(specimen.asInt64(), Ag::BadCastException);
}

GTEST_TEST(Value, Conversion)
{
    Value result;
    EXPECT_TRUE(Value(static_cast<int32_t>(42)).tryConvert(DataType::Uint64, result));
    EXPECT_EQ(result.asUint64(), 42);
    EXPECT_TRUE(Value(static_cast<int32_t>(42)).tryConvert(DataType::String, result));
    EXPECT_STRINGEQC(result.asString(), "42");
    EXPECT_TRUE(Value(static_cast<uint64_t>(65)).tryConvert(DataType::Character, result));
    EXPECT_EQ(result.asCharacter(), U'A');
}

} // Anonymous namespace

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


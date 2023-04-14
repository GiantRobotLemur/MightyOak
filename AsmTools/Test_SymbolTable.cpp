//! @file Test_SymbolTable.cpp
//! @brief The definition of unit tests for the symbol table object.
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
#include "AsmTools/Messages.hpp"

#include "ConstantSet.hpp"
#include "SymbolTable.hpp"

namespace Ag {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(ConstantSet, Empty)
{
    ConstantSet specimen;
    Value result;

    EXPECT_FALSE(specimen.contains("cp13"));
    EXPECT_FALSE(specimen.contains("F3"));
    EXPECT_FALSE(specimen.tryLookupValue("R1", result));
    EXPECT_FALSE(specimen.tryLookupValue("PC", result));
}

GTEST_TEST(ConstantSet, FixedConstants)
{
    ConstantSet specimen;
    Value result;

    specimen.addConstant("Pc", 15u);
    specimen.addConstant("sp", Value("Stack Pointer"));

    EXPECT_FALSE(specimen.contains("R99"));
    EXPECT_FALSE(specimen.tryLookupValue("R1", result));

    EXPECT_TRUE(specimen.contains("pc"));
    EXPECT_TRUE(specimen.tryLookupValue("PC", result));
    ASSERT_EQ(result.getDataType(), DataType::Uint32);
    EXPECT_EQ(result.asUint32(), 15u);

    EXPECT_TRUE(specimen.contains("sP"));
    EXPECT_TRUE(specimen.tryLookupValue("SP", result));
    ASSERT_EQ(result.getDataType(), DataType::String);
    EXPECT_STRINGEQC(result.asString(), "Stack Pointer");
}

GTEST_TEST(ConstantSet, BasedConstants)
{
    ConstantSet specimen;
    Value result;

    specimen.addBasedConstant("R", 0, 15, 0);
    specimen.addBasedConstant("v", 1, 6, 3);
    specimen.addBasedConstant("cp", 0, 15, 42);

    EXPECT_FALSE(specimen.contains("SP"));
    EXPECT_FALSE(specimen.tryLookupValue("PC", result));

    EXPECT_FALSE(specimen.contains("R2_FQ"));
    EXPECT_FALSE(specimen.contains("R002"));
    EXPECT_FALSE(specimen.contains("R16"));
    EXPECT_FALSE(specimen.contains("V0"));
    EXPECT_FALSE(specimen.contains("V7"));
    EXPECT_FALSE(specimen.tryLookupValue("R2D2", result));
    EXPECT_FALSE(specimen.tryLookupValue("C3PO", result));

    EXPECT_TRUE(specimen.contains("R9"));
    EXPECT_TRUE(specimen.contains("r0"));
    EXPECT_TRUE(specimen.tryLookupValue("r1", result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 1);

    EXPECT_TRUE(specimen.contains("v6"));
    EXPECT_TRUE(specimen.tryLookupValue("V5", result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 8);

    EXPECT_FALSE(specimen.contains("CP001"));
    EXPECT_TRUE(specimen.contains("CP1"));
    EXPECT_TRUE(specimen.tryLookupValue("cP9", result));
    ASSERT_EQ(result.getDataType(), DataType::Int32);
    EXPECT_EQ(result.asInt32(), 51);
}

GTEST_TEST(SymbolTable, Empty)
{
    SymbolTable specimen;
    Location source;
    Value value;

    EXPECT_FALSE(specimen.contains("Foo", source));
    EXPECT_FALSE(specimen.contains("Bar", source));
    EXPECT_FALSE(specimen.tryLookupValue("Hello", value));
    EXPECT_FALSE(specimen.tryLookupValue("World", value));
}

GTEST_TEST(SymbolTable, QueryDefinedSymbols)
{
    SymbolTable specimen;
    Location source;
    Value value;

    Location mySource(__FILE__);
    mySource.LineNo = __LINE__;
    mySource.Offset = 42;

    specimen.defineSymbol("Bar", mySource, 11, false);
    specimen.defineSymbol("Hello", mySource, 3.14156, false);

    EXPECT_FALSE(specimen.contains("Foo", source));
    EXPECT_TRUE(specimen.contains("Bar", source));
    EXPECT_TRUE(specimen.tryLookupValue("Hello", value));
    ASSERT_EQ(value.getDataType(), DataType::Double);
    EXPECT_EQ(value.asDouble(), 3.14156);
    EXPECT_FALSE(specimen.tryLookupValue("World", value));
}

} // Anonymous namespace

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


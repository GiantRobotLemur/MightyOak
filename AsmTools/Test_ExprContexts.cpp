//! @file Test_ExprContexts.cpp
//! @brief The definition of unit tests for various implementations of the
//! IEvalContext interface.
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

#include "ExprContexts.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(RootEvalContext, Empty)
{
    RootEvalContext specimen(0x10000);
    String scope;
    Location at;
    Value value;

    EXPECT_FALSE(specimen.isSymbolDefined("FooBar", scope, at));
    EXPECT_FALSE(specimen.isSymbolDefined("_main", scope, at));
    EXPECT_FALSE(specimen.tryLookupSymbol("FooBar", value));
    EXPECT_FALSE(specimen.getScopeName().isEmpty());
    EXPECT_EQ(specimen.getAssemblyOffset(), 0x0000u);
}

GTEST_TEST(RootEvalContext, AssemblyOffset)
{
    RootEvalContext specimen(0x10000);

    EXPECT_EQ(specimen.getAssemblyOffset(), 0x0000u);

    specimen.setAssemblyOffset(0xDEADBEEF);
    EXPECT_EQ(specimen.getAssemblyOffset(), 0xDEADBEEFu);

    specimen.setAssemblyOffset(0xCAFEBABE);
    EXPECT_EQ(specimen.getAssemblyOffset(), 0xCAFEBABEu);
}

GTEST_TEST(RootEvalContext, WithSymbols)
{
    RootEvalContext specimen(0x10000);
    String scope;
    Location at;
    Value value;

    specimen.defineSymbol("FooBar", Location(__FILE__), Value(42));

    EXPECT_FALSE(specimen.isSymbolDefined("fOObAR", scope, at));
    EXPECT_TRUE(specimen.isSymbolDefined("FooBar", scope, at));
    EXPECT_STRINGEQ(specimen.getScopeName(), scope);
    EXPECT_STRINGEQC(at.FileName, __FILE__);

    EXPECT_FALSE(specimen.tryLookupSymbol("fooBAR", value));
    EXPECT_TRUE(specimen.tryLookupSymbol("FooBar", value));
    ASSERT_EQ(value.getDataType(), DataType::Int32);
    EXPECT_EQ(value.asInt32(), 42);

    // Try overwriting the value (should fail).
    specimen.defineSymbol("FooBar", Location(__FILE__), Value(1.125f));

    EXPECT_FALSE(specimen.tryLookupSymbol("fooBAR", value));
    EXPECT_TRUE(specimen.tryLookupSymbol("FooBar", value));
    ASSERT_EQ(value.getDataType(), DataType::Int32);
    EXPECT_EQ(value.asInt32(), 42);
}

GTEST_TEST(InnerEvalContext, Empty)
{
    RootEvalContext globalScope(0x10000);
    InnerEvalContext specimen(&globalScope, "Local");
    String scope;
    Location at;
    Value value;

    globalScope.setAssemblyOffset(0xFEBE);

    EXPECT_FALSE(specimen.isSymbolDefined("FooBar", scope, at));
    EXPECT_FALSE(specimen.isSymbolDefined("_main", scope, at));
    EXPECT_FALSE(specimen.tryLookupSymbol("FooBar", value));
    EXPECT_STRINGEQC(specimen.getScopeName(), "Local");
    EXPECT_EQ(specimen.getAssemblyOffset(), 0xFEBEu);
}

GTEST_TEST(InnerEvalContext, AssemblyOffset)
{
    RootEvalContext globalScope(0x10000);
    InnerEvalContext specimen(&globalScope, "Local");

    EXPECT_EQ(specimen.getAssemblyOffset(), 0x0000u);

    // Change the offset from the local scope.
    specimen.setAssemblyOffset(0xDEADBEEF);
    EXPECT_EQ(specimen.getAssemblyOffset(), 0xDEADBEEFu);

    // Change the offset from the global scope.
    globalScope.setAssemblyOffset(0xCAFEBABE);
    EXPECT_EQ(specimen.getAssemblyOffset(), 0xCAFEBABEu);
}

GTEST_TEST(InnerEvalContext, WithSymbols)
{
    RootEvalContext globalScope(0x10000);
    InnerEvalContext specimen(&globalScope, "Local");
    String scope;
    Location at;
    Value value;

    globalScope.defineSymbol("FooBar", Location(__FILE__), Value(42));

    EXPECT_FALSE(specimen.isSymbolDefined("fOObAR", scope, at));
    EXPECT_TRUE(specimen.isSymbolDefined("FooBar", scope, at));
    EXPECT_STRINGEQ(globalScope.getScopeName(), scope);
    EXPECT_STRINGEQC(at.FileName, __FILE__);

    EXPECT_FALSE(specimen.tryLookupSymbol("fooBAR", value));
    EXPECT_TRUE(specimen.tryLookupSymbol("FooBar", value));
    ASSERT_EQ(value.getDataType(), DataType::Int32);
    EXPECT_EQ(value.asInt32(), 42);

    // Try setting the value in the local scope.
    specimen.defineSymbol("FooBar", Location("Here"), Value(1.125f));

    EXPECT_TRUE(specimen.isSymbolDefined("FooBar", scope, at));
    EXPECT_STRINGEQ(specimen.getScopeName(), scope);
    EXPECT_STRINGEQC(at.FileName, "Here");

    EXPECT_FALSE(specimen.tryLookupSymbol("fooBAR", value));
    EXPECT_TRUE(specimen.tryLookupSymbol("FooBar", value));
    ASSERT_EQ(value.getDataType(), DataType::Float);
    EXPECT_EQ(value.asFloat(), 1.125f);
}

} // TED

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


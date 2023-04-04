//! @file Core/Test_Variant.cpp
//! @brief The definition of unit tests for the Ag::Variant class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/Core/Variant.hpp"
#include "Ag/Core/VariantTypes.hpp"

namespace Ag {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Variant, DefaultConstruct)
{
    Variant specimen;

    EXPECT_TRUE(specimen.isEmpty());
    EXPECT_EQ(specimen.getType(), nullptr);
}

GTEST_TEST(Variant, Int32Construct)
{
    int32_t testValue = 42;
    Variant specimen(VariantTypes::Int32, 42);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_EQ(specimen.getType(), VariantTypes::Int32);

    int32_t extracted = specimen.get<Int32VariantType, int32_t>();
    EXPECT_EQ(extracted, testValue);

    int32_t *ptr = specimen.getPtr<Int32VariantType, int32_t>();

    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, testValue);

    *ptr = 69;
    extracted = specimen.get<Int32VariantType, int32_t>();

    EXPECT_EQ(extracted, 69);
}


} // TED

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


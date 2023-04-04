//! @file Core/Test_ScalarParser.cpp
//! @brief The definition of unit tests for the ScalarParser class.
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

#include "Ag/Core/ScalarParser.hpp"

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
bool tryParse(ScalarParser &parser, const char *text)
{
    bool isOK = true;
    parser.reset();

    for (size_t index = 0; isOK && (text[index] != '\0'); ++index)
    {
        if (parser.tryProcessCharacter(text[index]) == false)
        {
            isOK = false;
        }
    }

    return isOK;
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(ScalarParser, ParseIntegers)
{
    ScalarParser specimen;

    ASSERT_TRUE(tryParse(specimen, "0"));

    uint8_t uint8 = 1;

    ASSERT_TRUE(specimen.tryGetValue(uint8));
    EXPECT_EQ(uint8, 0u);

    ASSERT_TRUE(tryParse(specimen, "42"));
    ASSERT_TRUE(specimen.tryGetValue(uint8));
    EXPECT_EQ(uint8, 42u);

    ASSERT_TRUE(tryParse(specimen, "127"));
    ASSERT_TRUE(specimen.tryGetValue(uint8));
    EXPECT_EQ(uint8, 127u);

    int8_t int8 = 0;
    ASSERT_TRUE(tryParse(specimen, "  -128  "));
    ASSERT_TRUE(specimen.tryGetValue(int8));
    EXPECT_EQ(int8, -128);
}

} // TED

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


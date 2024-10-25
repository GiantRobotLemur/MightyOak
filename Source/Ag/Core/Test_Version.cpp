//! @file Core/Test_Version.cpp
//! @brief The definition of unit tests for the Version class.
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

#include "Ag/GTest_Core.hpp"
#include "Ag/Core/Version.hpp"

namespace Ag {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Version, DefaultConstuct)
{
    Version specimen;

    EXPECT_TRUE(specimen.isEmpty());
    EXPECT_EQ(specimen.getMajor(), 0u);
    EXPECT_EQ(specimen.getMinor(), 0u);
    EXPECT_EQ(specimen.getRevision(), 0u);
    EXPECT_EQ(specimen.getPatch(), 0u);
    EXPECT_TRUE(specimen.getComment().isEmpty());
}

GTEST_TEST(Version, ConstructPartInitialised)
{
    Version specimen(5, 2);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_EQ(specimen.getMajor(), 5u);
    EXPECT_EQ(specimen.getMinor(), 2u);
    EXPECT_EQ(specimen.getRevision(), 0u);
    EXPECT_EQ(specimen.getPatch(), 0u);
    EXPECT_TRUE(specimen.getComment().isEmpty());
}

GTEST_TEST(Version, ConstructInitialised)
{
    Version specimen(1, 2, 3, 4, "New York Edition");

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_EQ(specimen.getMajor(), 1u);
    EXPECT_EQ(specimen.getMinor(), 2u);
    EXPECT_EQ(specimen.getRevision(), 3u);
    EXPECT_EQ(specimen.getPatch(), 4u);
    EXPECT_FALSE(specimen.getComment().isEmpty());
}

GTEST_TEST(Version, Compare)
{
    Version specimen(5, 6, 7, 8);

    EXPECT_TRUE(specimen == Version(5, 6, 7, 8));
    EXPECT_TRUE(specimen == Version(5, 6, 7, 8, "Hello Nurse!"));
    EXPECT_FALSE(specimen == Version(5, 6, 7, 0));
    EXPECT_FALSE(specimen == Version(5, 6, 4, 8));
    EXPECT_FALSE(specimen == Version(5, 1, 7, 8));
    EXPECT_FALSE(specimen == Version(9, 6, 7, 8));

    EXPECT_FALSE(specimen != Version(5, 6, 7, 8));
    EXPECT_FALSE(specimen != Version(5, 6, 7, 8, "Hello Nurse!"));
    EXPECT_TRUE(specimen != Version(5, 6, 7, 0));
    EXPECT_TRUE(specimen != Version(5, 6, 4, 8));
    EXPECT_TRUE(specimen != Version(5, 1, 7, 8));
    EXPECT_TRUE(specimen != Version(9, 6, 7, 8));

    // Verify < operator.
    EXPECT_FALSE(specimen < Version(4, 6, 7, 8));
    EXPECT_FALSE(specimen < Version(5, 5, 7, 8));
    EXPECT_FALSE(specimen < Version(5, 6, 6, 8));
    EXPECT_FALSE(specimen < Version(5, 6, 7, 7));
    EXPECT_FALSE(specimen < Version(5, 6, 7, 8));
    EXPECT_TRUE(specimen < Version(6, 6, 7, 8));
    EXPECT_TRUE(specimen < Version(5, 7, 7, 8));
    EXPECT_TRUE(specimen < Version(5, 6, 8, 8));
    EXPECT_TRUE(specimen < Version(5, 6, 7, 9));

    // Verify <= operator.
    EXPECT_FALSE(specimen <= Version(4, 6, 7, 8));
    EXPECT_FALSE(specimen <= Version(5, 5, 7, 8));
    EXPECT_FALSE(specimen <= Version(5, 6, 6, 8));
    EXPECT_FALSE(specimen <= Version(5, 6, 7, 7));
    EXPECT_TRUE(specimen <= Version(5, 6, 7, 8));
    EXPECT_TRUE(specimen <= Version(6, 6, 7, 8));
    EXPECT_TRUE(specimen <= Version(5, 7, 7, 8));
    EXPECT_TRUE(specimen <= Version(5, 6, 8, 8));
    EXPECT_TRUE(specimen <= Version(5, 6, 7, 9));

    // Verify > operator.
    EXPECT_TRUE(specimen > Version(4, 6, 7, 8));
    EXPECT_TRUE(specimen > Version(5, 5, 7, 8));
    EXPECT_TRUE(specimen > Version(5, 6, 6, 8));
    EXPECT_TRUE(specimen > Version(5, 6, 7, 7));
    EXPECT_FALSE(specimen > Version(5, 6, 7, 8));
    EXPECT_FALSE(specimen > Version(6, 6, 7, 8));
    EXPECT_FALSE(specimen > Version(5, 7, 7, 8));
    EXPECT_FALSE(specimen > Version(5, 6, 8, 8));
    EXPECT_FALSE(specimen > Version(5, 6, 7, 9));

    // Verify >= operator.
    EXPECT_TRUE(specimen >= Version(4, 6, 7, 8));
    EXPECT_TRUE(specimen >= Version(5, 5, 7, 8));
    EXPECT_TRUE(specimen >= Version(5, 6, 6, 8));
    EXPECT_TRUE(specimen >= Version(5, 6, 7, 7));
    EXPECT_TRUE(specimen >= Version(5, 6, 7, 8));
    EXPECT_FALSE(specimen >= Version(6, 6, 7, 8));
    EXPECT_FALSE(specimen >= Version(5, 7, 7, 8));
    EXPECT_FALSE(specimen >= Version(5, 6, 8, 8));
    EXPECT_FALSE(specimen >= Version(5, 6, 7, 9));
}

GTEST_TEST(Version, TryParse)
{
    Version specimen;

    EXPECT_TRUE(specimen.tryParse("1.2.3.4"));
    EXPECT_EQ(specimen, Version(1, 2, 3, 4));
    EXPECT_TRUE(specimen.getComment().isEmpty());

    EXPECT_TRUE(specimen.tryParse("   1.2.3.4   "));
    EXPECT_EQ(specimen, Version(1, 2, 3, 4));
    EXPECT_TRUE(specimen.getComment().isEmpty());

    EXPECT_TRUE(specimen.tryParse("   1.2.3.4.5.6.7   "));
    EXPECT_EQ(specimen, Version(1, 2, 3, 4));
    EXPECT_TRUE(specimen.getComment().isEmpty());

    EXPECT_TRUE(specimen.tryParse("   1.2.3.4  Well hello there! "));
    EXPECT_EQ(specimen, Version(1, 2, 3, 4));
    EXPECT_STRINGEQC(specimen.getComment(), "Well hello there!");

    EXPECT_TRUE(specimen.tryParse("   5.20 - NVIDIA PCI Ex SSE2 "));
    EXPECT_EQ(specimen, Version(5, 20, 0, 0));
    EXPECT_STRINGEQC(specimen.getComment(), "NVIDIA PCI Ex SSE2");

    EXPECT_TRUE(specimen.tryParse("   11.9.2 (Purple Edition) Moose!"));
    EXPECT_EQ(specimen, Version(11, 9, 2, 0));
    EXPECT_STRINGEQC(specimen.getComment(), "Purple Edition");

    EXPECT_TRUE(specimen.tryParse("   42 - [Red] Moose!"));
    EXPECT_EQ(specimen, Version(42, 0, 0, 0));
    EXPECT_STRINGEQC(specimen.getComment(), "[Red] Moose!");
}

GTEST_TEST(Version, ToString)
{
    EXPECT_STRINGEQC(Version(12, 3, 4, 0, "Deluxe Edition").toString(2, true),
                     "12.3 - Deluxe Edition");

    EXPECT_STRINGEQC(Version(12, 3, 4, 0, "Deluxe Edition").toString(1, false), "12");

    EXPECT_STRINGEQC(Version(12, 3, 4, 0, "Deluxe Edition").toString(4, false),
                     "12.3.4.0");
}

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


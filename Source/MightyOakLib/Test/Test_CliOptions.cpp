//! @file MightyOakLib/Test/Test_CliOptions.cpp
//! @brief  The definition of unit tests for the CliOptions class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "MightyOakLib/CliOptions.hpp"

namespace Mo {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CliOptions, DefaultConstruct)
{
    CliOptions specimen;

    EXPECT_EQ(specimen.getCommand(), Ag::Cli::StandardCommands::NoCommand);
}

GTEST_TEST(CliOptions, ParseNoArgsStartsDefaultSession)
{
    CliOptions specimen;
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"", error));

    EXPECT_EQ(specimen.getCommand(), Ag::toScalar(AppCommand::RunSession));
    EXPECT_TRUE(specimen.getEmulatedSystemConfig().validate(error));
}

GTEST_TEST(CliOptions, ParseArgs_UpgradedA3010)
{
    CliOptions specimen;
    Ag::String error;

    ASSERT_TRUE(specimen.tryParse(L"--ram 4Mb", error));

    EXPECT_EQ(specimen.getCommand(), Ag::toScalar(AppCommand::RunSession));
    EXPECT_TRUE(specimen.getEmulatedSystemConfig().validate(error));
}

} // Anonymous namespace

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


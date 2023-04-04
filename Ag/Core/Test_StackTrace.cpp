//! @file Core/Test_StackTrace.cpp
//! @brief The definition of unit test for the StackTrace class.
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

#include "Ag/Core/StackTrace.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(StackTrace, DefaultConstruct)
{
    StackTrace specimen;

    EXPECT_TRUE(specimen.isEmpty());
    EXPECT_EQ(specimen.getModuleCount(), 0u);
    EXPECT_EQ(specimen.getEntryCount(), 0u);
}

GTEST_TEST(StackTrace, Capture)
{
    StackTrace specimen;

    specimen.captureCurrentThread();

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_GT(specimen.getModuleCount(), 0u);
    ASSERT_GT(specimen.getEntryCount(), 0u);

    struct ExtraEntry
    {
        void *Address;
        FullStackTraceEntry Entry;
    };

    // Extract the stack data and create a void pointer so that
    // the derived data can be compared with the debug data.
    std::vector<ExtraEntry> entries;
    entries.reserve(specimen.getEntryCount());

    for (size_t index = 0, count = specimen.getEntryCount(); index < count; ++index)
    {
        entries.emplace_back();

        ExtraEntry &extra = entries.back();
        specimen.getFullEntry(index, extra.Entry);

        extra.Address = reinterpret_cast<void *>(extra.Entry.ModuleBaseAddress +
                                                 extra.Entry.Offset);
    }

    EXPECT_FALSE(entries.empty());

    std::string summary, moduleSummary;
    specimen.appendToString(summary, false);

    EXPECT_FALSE(summary.empty());

    specimen.appendToString(moduleSummary, true);
    EXPECT_GT(moduleSummary.length(), summary.length());

    // Symbols are extracted from the PDB and include functions in anonymous
    // namespaces (like this one). This only occurs on Microsoft Windows.
    std::string_view thisFn = specimen.getEntrySymbol(0);

    EXPECT_NE(thisFn.find(__FUNCTION__), std::string_view::npos);
}


} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


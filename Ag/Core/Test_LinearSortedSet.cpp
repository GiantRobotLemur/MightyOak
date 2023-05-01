//! @file Test_LinearSortedSet.cpp
//! @brief The definition of unit tests for the LinearSortedSet template class.
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

#include "Ag/Core/LinearSortedSet.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
using TestSet = LinearSortedSet; // <int>;

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(LinearSortedSet, DefaultConstruct)
{
    TestSet specimen;

    EXPECT_TRUE(specimen.isEmpty());
    EXPECT_EQ(specimen.getCount(), 0u);
    EXPECT_EQ(specimen.find(42), specimen.allEnd());
    EXPECT_FALSE(specimen.contains(69));
}

GTEST_TEST(LinearSortedSet, CanFindInSingleElementSet)
{
    TestSet specimen;

    specimen.push_back(42);

    auto pos = specimen.findIndexed(42);

    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 42);

    pos = specimen.find(42);

    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 42);

    pos = specimen.findIndexed(69);
    EXPECT_EQ(pos, specimen.allEnd());

    pos = specimen.find(69);
    EXPECT_EQ(pos, specimen.allEnd());
}

GTEST_TEST(LinearSortedSet, CanFindSortedElementSet)
{
    TestSet specimen;

    specimen.push_back(42);
    specimen.push_back(69);

    // Search for indexed value - positive.
    auto pos = specimen.findIndexed(69);

    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 69);

    // Search for possibly non-indexed value - positive.
    pos = specimen.find(69);

    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 69);

    // Search for non-indexed value - negative.
    pos = specimen.findIndexed(42);

    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 42);

    // Search for non-indexed value.
    pos = specimen.find(42);

    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 42);
}

GTEST_TEST(LinearSortedSet, CantFindUnsortedElementSet)
{
    TestSet specimen;

    specimen.push_back(69);
    specimen.push_back(42);

    auto pos = specimen.findIndexed(69);

    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 69);

    pos = specimen.find(69);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 69);

    pos = specimen.findIndexed(42);
    EXPECT_EQ(pos, specimen.allEnd());

    pos = specimen.find(42);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 42);
}

GTEST_TEST(LinearSortedSet, CantFindUnsortedMultiElementSet)
{
    TestSet specimen;

    specimen.push_back(69);
    specimen.push_back(42);
    specimen.push_back(11);
    specimen.push_back(22);

    auto pos = specimen.findIndexed(69);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 69);

    pos = specimen.find(69);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 69);

    pos = specimen.findIndexed(42);
    EXPECT_EQ(pos, specimen.allEnd());

    pos = specimen.find(42);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 42);

    pos = specimen.findIndexed(11);
    EXPECT_EQ(pos, specimen.allEnd());

    pos = specimen.find(11);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 11);

    pos = specimen.findIndexed(22);
    EXPECT_EQ(pos, specimen.allEnd());

    pos = specimen.find(22);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 22);
}

GTEST_TEST(LinearSortedSet, ExistsUnsortedElements)
{
    TestSet specimen;

    specimen.push_back(69);
    specimen.push_back(42);
    specimen.push_back(11);
    specimen.push_back(22);

    EXPECT_TRUE(specimen.contains(69));
    EXPECT_TRUE(specimen.contains(42));
    EXPECT_TRUE(specimen.contains(11));
    EXPECT_TRUE(specimen.contains(22));
    EXPECT_FALSE(specimen.contains(101));
    EXPECT_FALSE(specimen.contains(0));
}

GTEST_TEST(LinearSortedSet, CanFindAfterCompile)
{
    TestSet specimen;

    specimen.push_back(69);
    specimen.push_back(42);
    specimen.push_back(11);
    specimen.push_back(22);

    auto pos = specimen.findIndexed(69);

    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 69);

    EXPECT_EQ(specimen.findIndexed(42), specimen.allEnd());
    EXPECT_EQ(specimen.findIndexed(11), specimen.allEnd());
    EXPECT_EQ(specimen.findIndexed(22), specimen.allEnd());

    // Index the unsorted elements.
    specimen.reindex();

    pos = specimen.findIndexed(69);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 69);

    pos = specimen.findIndexed(42);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 42);

    pos = specimen.findIndexed(11);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 11);

    pos = specimen.findIndexed(22);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 22);
}

GTEST_TEST(LinearSortedSet, EraseKeyUnindexed)
{
    TestSet specimen;

    specimen.push_back(11);
    specimen.push_back(9);
    specimen.push_back(27);
    specimen.push_back(11);
    specimen.push_back(4);
    specimen.push_back(-9);
    specimen.push_back(27);
    specimen.push_back(11);
    specimen.push_back(42);

    size_t initialSize = specimen.getCount();
    auto pos = specimen.erase(11);

    // Ensure the next element is the first unsorted element.
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(pos, specimen.end());
    EXPECT_EQ(*pos, 9);
    EXPECT_EQ(specimen.getCount(), initialSize - 3);

    EXPECT_EQ(specimen.find(11), specimen.allEnd());
}

GTEST_TEST(LinearSortedSet, EraseKeyIndexed)
{
    TestSet specimen;

    specimen.push_back(11);
    specimen.push_back(9);
    specimen.push_back(27);
    specimen.push_back(11);
    specimen.push_back(4);
    specimen.push_back(-9);
    specimen.push_back(26);
    specimen.push_back(11);
    specimen.push_back(42);

    size_t initialSize = specimen.getCount();
    specimen.reindex();

    auto pos = specimen.erase(11);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 26);
    EXPECT_EQ(specimen.getCount(), initialSize - 3);

    EXPECT_EQ(specimen.find(11), specimen.allEnd());
}

GTEST_TEST(LinearSortedSet, EraseSingleIterator)
{
    TestSet specimen;

    specimen.push_back(11);
    specimen.push_back(9);
    specimen.push_back(27);
    specimen.push_back(11);
    specimen.push_back(4);
    specimen.push_back(-9);
    specimen.push_back(26);
    specimen.push_back(11);
    specimen.push_back(42);

    size_t initialSize = specimen.getCount();
    specimen.reindex();

    auto itemToErase = specimen.find(11);
    ASSERT_NE(itemToErase, specimen.allEnd());

    auto pos = specimen.erase(itemToErase);
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 11);
    EXPECT_EQ(specimen.getCount(), initialSize - 1);

    // Ensure the other mappings with the same key still exist.
    EXPECT_NE(specimen.find(11), specimen.allEnd());
}

GTEST_TEST(LinearSortedSet, EraseRange)
{
    TestSet specimen;

    specimen.push_back(11);
    specimen.push_back(9);
    specimen.push_back(27);
    specimen.push_back(11);
    specimen.push_back(4);
    specimen.push_back(-9);
    specimen.push_back(26);
    specimen.push_back(11);
    specimen.push_back(42);

    size_t initialSize = specimen.getCount();
    specimen.reindex();

    auto rangeToErase = specimen.findRange(11);
    ASSERT_FALSE(rangeToErase.isEmpty());

    // Remove one extra element.
    ++rangeToErase.End;
    ASSERT_EQ(rangeToErase.getCount(), 4u);
    auto pos = specimen.erase(rangeToErase.Begin, rangeToErase.End);

    // Ensure the correct elements were removed.
    ASSERT_NE(pos, specimen.allEnd());
    EXPECT_EQ(*pos, 27);
    EXPECT_EQ(specimen.getCount(), initialSize - 4);

    // Ensure range was removed.
    EXPECT_EQ(specimen.find(11), specimen.allEnd());
}

GTEST_TEST(LinearSortedSet, MultiSetFind)
{
    TestSet specimen;

    specimen.push_back(11);
    specimen.push_back(9);
    specimen.push_back(27);
    specimen.push_back(11);
    specimen.push_back(4);
    specimen.push_back(-9);
    specimen.push_back(27);
    specimen.push_back(11);
    specimen.push_back(42);

    // Without indexing, expect to find none.
    auto posPair = specimen.findRange(27);
    EXPECT_TRUE(posPair.isEmpty());
    EXPECT_EQ(posPair.End, specimen.allEnd());

    // Without indexing, expect to find only one match.
    posPair = specimen.findRange(11);
    ASSERT_FALSE(posPair.isEmpty());
    EXPECT_EQ(*posPair.Begin, 11);
    EXPECT_EQ(posPair.getCount(), 1u);

    // Index the entries.
    specimen.reindex();

    // Repeat the queries.
    posPair = specimen.findRange(27);
    ASSERT_FALSE(posPair.isEmpty());
    EXPECT_EQ(*posPair.Begin, 27);
    EXPECT_EQ(posPair.getCount(), 2u);

    posPair = specimen.findRange(11);
    ASSERT_FALSE(posPair.isEmpty());
    EXPECT_EQ(*posPair.Begin, 11);
    EXPECT_EQ(posPair.getCount(), 3u);
}

GTEST_TEST(LinearSortedSet, DeferReindex)
{
    TestSet specimen;

    if (specimen.isEmpty())
    {
        LinearSortedSetIndexer deferReindex(specimen);

        specimen.push_back(11);
        specimen.push_back(9);
        specimen.push_back(27);
        specimen.push_back(11);
        specimen.push_back(4);
        specimen.push_back(-9);
        specimen.push_back(27);
        specimen.push_back(11);
        specimen.push_back(42);

        EXPECT_TRUE(specimen.needsReindex());
    }

    EXPECT_FALSE(specimen.needsReindex());
}

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


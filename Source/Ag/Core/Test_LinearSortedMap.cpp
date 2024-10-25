//! @file Test_LinearSortedMap.cpp
//! @brief The definition of unit tests for the LinearSortedMap template class.
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

#include "Ag/Core/LinearSortedMap.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
using TestMap = LinearSortedMap<int, int>;

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(LinearSortedMap, ConstructEmpty)
{
    TestMap specimen;

    EXPECT_TRUE(specimen.isEmpty());
    EXPECT_EQ(specimen.getCount(), 0u);
    EXPECT_EQ(specimen.findIndexed(42), specimen.mappingsEnd());
    EXPECT_FALSE(specimen.containsKey(69));
}

GTEST_TEST(LinearSortedMap, CanFindInSingleElementMap)
{
    TestMap specimen;

    specimen.push_back(42, -9);

    auto pos = specimen.findIndexed(42);

    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, -9);

    pos = specimen.find(42);

    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, -9);

    pos = specimen.findIndexed(69);
    EXPECT_EQ(pos, specimen.mappingsEnd());

    pos = specimen.find(69);
    EXPECT_EQ(pos, specimen.mappingsEnd());
}

GTEST_TEST(LinearSortedMap, CanFindSortedElementMap)
{
    TestMap specimen;

    specimen.push_back(42, -9);
    specimen.push_back(69, 11);

    // Search for indexed value - positive.
    auto pos = specimen.findIndexed(69);

    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 11);

    // Search for possibly non-indexed value - positive.
    pos = specimen.find(69);

    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 11);

    // Search for non-indexed value - negative.
    pos = specimen.findIndexed(42);

    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, -9);

    // Search for non-indexed value.
    pos = specimen.find(42);

    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, -9);
}

GTEST_TEST(LinearSortedMap, CantFindUnsortedElementMap)
{
    TestMap specimen;

    specimen.push_back(69, 11);
    specimen.push_back(42, -9);

    auto pos = specimen.findIndexed(69);

    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 11);

    pos = specimen.find(69);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 11);

    pos = specimen.findIndexed(42);
    EXPECT_EQ(pos, specimen.mappingsEnd());

    pos = specimen.find(42);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, -9);
}

GTEST_TEST(LinearSortedMap, CantFindUnsortedMultiElementMap)
{
    TestMap specimen;

    specimen.push_back(69, 11);
    specimen.push_back(42, -9);
    specimen.push_back(11, 92);
    specimen.push_back(22, 0);

    auto pos = specimen.findIndexed(69);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 11);

    pos = specimen.find(69);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 11);

    pos = specimen.findIndexed(42);
    EXPECT_EQ(pos, specimen.mappingsEnd());

    pos = specimen.find(42);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, -9);

    pos = specimen.findIndexed(11);
    EXPECT_EQ(pos, specimen.mappingsEnd());

    pos = specimen.find(11);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 92);

    pos = specimen.findIndexed(22);
    EXPECT_EQ(pos, specimen.mappingsEnd());

    pos = specimen.find(22);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 0);
}

GTEST_TEST(LinearSortedMap, ExistsUnsortedElements)
{
    TestMap specimen;

    specimen.push_back(69, 11);
    specimen.push_back(42, -9);
    specimen.push_back(11, 92);
    specimen.push_back(22, 0);

    EXPECT_TRUE(specimen.containsKey(69));
    EXPECT_TRUE(specimen.containsKey(42));
    EXPECT_TRUE(specimen.containsKey(11));
    EXPECT_TRUE(specimen.containsKey(22));
    EXPECT_FALSE(specimen.containsKey(101));
    EXPECT_FALSE(specimen.containsKey(0));
}

GTEST_TEST(LinearSortedMap, CanFindAfterCompile)
{
    TestMap specimen;

    specimen.push_back(69, 11);
    specimen.push_back(42, -9);
    specimen.push_back(11, 92);
    specimen.push_back(22, 0);

    auto pos = specimen.findIndexed(69);

    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 11);

    EXPECT_EQ(specimen.findIndexed(42), specimen.mappingsEnd());
    EXPECT_EQ(specimen.findIndexed(11), specimen.mappingsEnd());
    EXPECT_EQ(specimen.findIndexed(22), specimen.mappingsEnd());

    // Index the unsorted elements.
    specimen.reindex();

    pos = specimen.findIndexed(69);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 11);

    pos = specimen.findIndexed(42);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, -9);

    pos = specimen.findIndexed(11);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 92);

    pos = specimen.findIndexed(22);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->second, 0);
}

GTEST_TEST(LinearSortedMap, EraseKeyUnindexed)
{
    TestMap specimen;

    specimen.push_back(11, 11);
    specimen.push_back(9, 3);
    specimen.push_back(27, 4);
    specimen.push_back(11, 9);
    specimen.push_back(4, 0);
    specimen.push_back(-9, 101);
    specimen.push_back(27, 3);
    specimen.push_back(11, 99);
    specimen.push_back(42, 6);

    size_t initialSize = specimen.getCount();
    auto pos = specimen.erase(11);

    // Ensure the next element is the first unsorted element.
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos, specimen.end());
    EXPECT_EQ(pos->first, 9);
    EXPECT_EQ(pos->second, 3);
    EXPECT_EQ(specimen.getCount(), initialSize - 3);

    EXPECT_EQ(specimen.find(11), specimen.mappingsEnd());
}

GTEST_TEST(LinearSortedMap, EraseKeyIndexed)
{
    TestMap specimen;

    specimen.push_back(11, 11);
    specimen.push_back(9, 3);
    specimen.push_back(27, 4);
    specimen.push_back(11, 9);
    specimen.push_back(4, 0);
    specimen.push_back(-9, 101);
    specimen.push_back(26, 3);
    specimen.push_back(11, 99);
    specimen.push_back(42, 6);

    size_t initialSize = specimen.getCount();
    specimen.reindex();

    auto pos = specimen.erase(11);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->first, 26);
    EXPECT_EQ(pos->second, 3);
    EXPECT_EQ(specimen.getCount(), initialSize - 3);

    EXPECT_EQ(specimen.find(11), specimen.mappingsEnd());
}

GTEST_TEST(LinearSortedMap, EraseSingleIterator)
{
    TestMap specimen;

    specimen.push_back(11, 11);
    specimen.push_back(9, 3);
    specimen.push_back(27, 4);
    specimen.push_back(11, 9);
    specimen.push_back(4, 0);
    specimen.push_back(-9, 101);
    specimen.push_back(26, 3);
    specimen.push_back(11, 99);
    specimen.push_back(42, 6);

    size_t initialSize = specimen.getCount();
    specimen.reindex();

    auto itemToErase = specimen.find(11);
    ASSERT_NE(itemToErase, specimen.mappingsEnd());

    auto pos = specimen.erase(itemToErase);
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->first, 11);
    EXPECT_EQ(specimen.getCount(), initialSize - 1);

    // Ensure the other mappings with the same key still exist.
    EXPECT_NE(specimen.find(11), specimen.mappingsEnd());
}

GTEST_TEST(LinearSortedMap, EraseRange)
{
    TestMap specimen;

    specimen.push_back(11, 11);
    specimen.push_back(9, 3);
    specimen.push_back(27, 4);
    specimen.push_back(11, 9);
    specimen.push_back(4, 0);
    specimen.push_back(-9, 101);
    specimen.push_back(26, 3);
    specimen.push_back(11, 99);
    specimen.push_back(42, 6);

    size_t initialSize = specimen.getCount();
    specimen.reindex();

    auto rangeToErase = specimen.findRange(11);
    ASSERT_NE(rangeToErase.Begin, specimen.mappingsEnd());

    // Remove one extra element.
    ++rangeToErase.End;
    ASSERT_EQ(rangeToErase.getCount(), 4u);
    auto pos = specimen.erase(rangeToErase.Begin, rangeToErase.End);

    // Ensure the correct elements were removed.
    ASSERT_NE(pos, specimen.mappingsEnd());
    EXPECT_EQ(pos->first, 27);
    EXPECT_EQ(specimen.getCount(), initialSize - 4);

    // Ensure range was removed.
    EXPECT_EQ(specimen.find(11), specimen.mappingsEnd());
}

GTEST_TEST(LinearSortedMap, MultiMapFind)
{
    TestMap specimen;

    specimen.push_back(11, 11);
    specimen.push_back(9, 3);
    specimen.push_back(27, 4);
    specimen.push_back(11, 9);
    specimen.push_back(4, 0);
    specimen.push_back(-9, 101);
    specimen.push_back(27, 3);
    specimen.push_back(11, 99);
    specimen.push_back(42, 6);

    // Without indexing, expect to find none.
    auto posPair = specimen.findRange(27);
    EXPECT_TRUE(posPair.isEmpty());
    EXPECT_EQ(posPair.End, specimen.mappingsEnd());

    // Without indexing, expect to find only one match.
    posPair = specimen.findRange(11);
    ASSERT_NE(posPair.Begin, posPair.End);
    EXPECT_EQ(posPair.Begin->second, 11);
    EXPECT_EQ(posPair.getCount(), 1u);

    // Index the entries.
    specimen.reindex();

    // Repeat the queries.
    posPair = specimen.findRange(27);
    ASSERT_FALSE(posPair.isEmpty());
    EXPECT_TRUE((posPair.Begin->second == 4) || (posPair.Begin->second == 3));
    EXPECT_EQ(posPair.getCount(), 2u);

    posPair = specimen.findRange(11);
    ASSERT_FALSE(posPair.isEmpty());
    EXPECT_TRUE((posPair.Begin->second == 11) ||
                (posPair.Begin->second == 9) ||
                (posPair.Begin->second == 99));
    EXPECT_EQ(posPair.getCount(), 3u);
}


GTEST_TEST(LinearSortedMap, MultiMapGroupBy)
{
    TestMap specimen;

    // KSorted keys: -9, 4, 9, 11, 11, 11, 27, 27, 42.
    specimen.push_back(11, 11);
    specimen.push_back(9, 3);
    specimen.push_back(27, 4);
    specimen.push_back(11, 9);
    specimen.push_back(4, 0);
    specimen.push_back(-9, 101);
    specimen.push_back(27, 3);
    specimen.push_back(11, 99);
    specimen.push_back(42, 6);

    TestMap::MatchingRange group;
    std::vector<int> values;

    ASSERT_TRUE(specimen.tryFindFirstKeyGroup(group));
    EXPECT_FALSE(group.isEmpty());
    EXPECT_EQ(group.Begin->first, -9);
    EXPECT_EQ(group.getCount(), 1u);
    EXPECT_TRUE(getGroupValues(group, values));
    ASSERT_EQ(values.size(), 1u);
    EXPECT_EQ(values.front(), 101);

    ASSERT_TRUE(specimen.tryFindNextKeyGroup(group));
    EXPECT_FALSE(group.isEmpty());
    EXPECT_EQ(group.Begin->first, 4);
    EXPECT_EQ(group.getCount(), 1u);

    ASSERT_TRUE(specimen.tryFindNextKeyGroup(group));
    EXPECT_FALSE(group.isEmpty());
    EXPECT_EQ(group.Begin->first, 9);
    EXPECT_EQ(group.getCount(), 1u);

    ASSERT_TRUE(specimen.tryFindNextKeyGroup(group));
    EXPECT_FALSE(group.isEmpty());
    EXPECT_EQ(group.Begin->first, 11);
    EXPECT_EQ(group.getCount(), 3u);

    ASSERT_TRUE(specimen.tryFindNextKeyGroup(group));
    EXPECT_FALSE(group.isEmpty());
    EXPECT_EQ(group.Begin->first, 27);
    EXPECT_EQ(group.getCount(), 2u);

    ASSERT_TRUE(specimen.tryFindNextKeyGroup(group));
    EXPECT_FALSE(group.isEmpty());
    EXPECT_EQ(group.Begin->first, 42);
    EXPECT_EQ(group.getCount(), 1u);

    ASSERT_FALSE(specimen.tryFindNextKeyGroup(group));
    EXPECT_TRUE(group.isEmpty());
}

GTEST_TEST(LinearSortedMap, DeferReindex)
{
    TestMap specimen;

    if (specimen.isEmpty())
    {
        LinearSortedMapIndexer deferReindex(specimen);

        specimen.push_back(11, 11);
        specimen.push_back(9, 3);
        specimen.push_back(27, 4);
        specimen.push_back(11, 9);
        specimen.push_back(4, 0);
        specimen.push_back(-9, 101);
        specimen.push_back(27, 3);
        specimen.push_back(11, 99);
        specimen.push_back(42, 6);

        EXPECT_TRUE(specimen.needsReindex());
    }

    EXPECT_FALSE(specimen.needsReindex());
}

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


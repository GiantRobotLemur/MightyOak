//! @file Ag/Core/CollectionTools.hpp
//! @brief The declaration of various useful collection-related template data
//! structures and functions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_COLLECTION_TOOLS_HPP__
#define __AG_CORE_COLLECTION_TOOLS_HPP__

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief An equivalent of std::pair<iterator> which can be used with
//! range-based for loops.
//! @tparam TIterator The data type of the iterators defining the range.
template<typename TIterator> struct IteratorRange
{
    // Public Types
    using Iterator = TIterator;
    using ValueType = typename TIterator::value_type;

    // Public Fields
    Iterator Begin;
    Iterator End;

    // Construction/Destruction
    IteratorRange() = default;
    ~IteratorRange() = default;

    //! @brief Constructs an empty range with both Begin and End set to the
    //! same position.
    //! @param noRange The beginning and end position value.
    IteratorRange(const Iterator &noRange) :
        Begin(noRange),
        End(noRange)
    {
    }

    //! @brief Constructs an active range.
    //! @param first The position of the first item in the range.
    //! @param afterLast The position of the item after the last in the range.
    IteratorRange(const Iterator &first, const IteratorRange &afterLast) :
        Begin(first),
        End(afterLast)
    {
    }

    //! @brief Converts an std::pair of iterators to an IteratorRange object.
    //! @param[in] stdPair The pair of iterators to convert, first mapping to Begin
    //! and second mapping to End.
    IteratorRange(const std::pair<Iterator, Iterator> &stdPair) :
        Begin(stdPair.first),
        End(stdPair.second)
    {
    }

    //! @brief Overwrites the object from an std::pair of iterators.
    //! @param[in] stdPair The pair of iterators to convert, first mapping to Begin
    //! and second mapping to End.
    //! @return A reference to the current object.
    IteratorRange &operator=(const std::pair<Iterator, Iterator> &stdPair)
    {
        Begin = stdPair.first;
        End = stdPair.second;

        return *this;
    }

    //! @brief Determines if the range is empty.
    //! @retval true The Begin and End iterators are identical, so no items
    //! would be iterated over.
    //! @retval false The Begin and End iterators are different, suggesting
    //! that the range defines a non-empty set of items.
    bool isEmpty() const { return Begin == End; }

    //! @brief Determines the count of items in the run using the
    //! std::distance() function.
    size_t getCount() const { return static_cast<size_t>(std::distance(Begin, End)); }

    //! @brief Gets the position of the first element in the range.
    //! @note This member function is defined for compatibility with
    //! range-based for loops.
    const Iterator &begin() const { return Begin; }

    //! @brief Gets the position after the last element in the range.
    //! @note This member function is defined for compatibility with
    //! range-based for loops.
    const Iterator &end() const { return End; }

    //! @brief Gets a reference to the first element in the range.
    ValueType &front() { return *Begin; }

    //! @brief Gets a read-only reference to the first element in the range.
    const ValueType &front() const { return *Begin; }
};

//! @brief Finds the first group in a sorted set of items.
//! @tparam TItem The data type of the items in the set.
//! @tparam TIterator The data type of the iterator used to defined positions
//! of items in the set.
//! @tparam TPred The data type of the predicate used to compare set items.
//! @param[in] begin The iterator pointing to the first time in the set.
//! @param[in] end The iterator pointing to the item after the last in the set.
//! @param[out] groupRange Receives a range of iterators describing the
//! first identical group in the set based on the predicate test.
//! @param[in] predicate The predicate used to compare items in the set.
//! @retval true An initial group was found and returned in groupRange.
//! @retval false The set was empty, groupRange was set to an empty state.
template<typename TIterator, typename TItem = typename TIterator::value_type,
    typename TPred = std::less<TItem> >
bool getFirstGroup(const TIterator &begin, const TIterator &end,
                   IteratorRange<TIterator> &groupRange,
                   const TPred &predicate = std::less<TItem>())
{
    bool hasGroup = false;

    if (begin == end)
    {
        // The range is empty.
        groupRange = IteratorRange<TIterator>(end);
    }
    else
    {
        groupRange.Begin = begin;
        groupRange.End = std::upper_bound(begin, end, *begin, predicate);
        hasGroup = true;
    }

    return hasGroup;
}

//! @brief Finds the next group of identical items in a sorted set.
//! @tparam TItem The data type of the items in the set.
//! @tparam TIterator The data type of the iterator used to defined positions
//! of items in the set.
//! @tparam TPred The data type of the predicate used to compare set items.
//! @param[in] end The iterator pointing to the item after the last in the set.
//! @param[in,out] groupRange Holds the range of values in the previous group
//! and is updated with a range of iterators describing the
//! next identical group in the set based on the predicate test.
//! @param[in] predicate The predicate used to compare items in the set.
//! @retval true Another group was found and returned in groupRange.
//! @retval false There were no more groups in the set, groupRange was set to
//! an empty state.
template<typename TIterator, typename TItem = typename TIterator::value_type,
    typename TPred = std::less<TItem> >
bool getNextGroup(const TIterator &end,
                  IteratorRange<TIterator> &groupRange,
                  const TPred &predicate = std::less<TItem>())
{
    bool hasGroup = false;

    if (groupRange.Begin != end)
    {
        // We have at least an (initially) active range.
        if (groupRange.End == end)
        {
            // We must end the range without accessing the item.
            groupRange.Begin = end;
        }
        else
        {
            // Find the end of the next group by searing the remaining sub-set.
            groupRange.Begin = groupRange.End;
            groupRange.End = std::upper_bound(groupRange.Begin, end,
                                              *groupRange.Begin, predicate);

            hasGroup = true;
        }
    }

    return hasGroup;
}

//! @brief Extracts values from a collection of mappings, i.e the second member
//! of each mapping pair.
//! @tparam TValue The data type of values to be extracted.
//! @tparam TMappingIterator An iterator which points to values of type
//! std::pair<?, TValue>.
//! @param[in] range The range of mappings to extract values from.
//! @param[out] values A collection to receive the extracted values.
//! @retval true At least one value was extracted from the mappings.
//! @retval false The range was empty.
template<typename TMappingIterator, typename TValue>
bool getGroupValues(const IteratorRange<TMappingIterator> &range,
                    std::vector<TValue> &values)
{
    values.clear();
    size_t count = range.getCount();

    if (values.capacity() < count)
    {
        values.reserve(count);
    }

    for (const auto &item : range)
    {
        values.push_back(item.second);
    }

    return count > 0;
}

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

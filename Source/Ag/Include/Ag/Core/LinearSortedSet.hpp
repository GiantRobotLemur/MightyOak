//! @file Ag/Core/LinearSortedSet.hpp
//! @brief The declaration of a sorted set based on a linear collection.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_LINEAR_SORTED_SET_HPP__
#define __AG_CORE_LINEAR_SORTED_SET_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <utility>
#include <vector>

#include "CollectionTools.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A sorted set implemented as a linear collection.
template<typename TValue, typename TComparer = std::less<TValue>>
class LinearSortedSet
{
public:
    // Public Types
    using Collection = std::vector<TValue>;
    using iterator = typename Collection::iterator;
    using const_iterator = typename Collection::const_iterator;
    using iterator_range = std::pair<iterator, iterator>;
    using const_iterator_range = std::pair<const_iterator, const_iterator>;
    using MatchingRange = IteratorRange<iterator>;
    using MatchingCRange = IteratorRange<const_iterator>;
private:
    // Internal Types
    //! @brief A predicate which uses the stateful comparer to determine if
    //! an element matches a specific instance.
    struct MatcherPredicate
    {
    private:
        const TComparer &_comparer;
        const TValue &_target;
    public:
        MatcherPredicate(const TComparer &comparer, const TValue &target) :
            _comparer(comparer),
            _target(target)
        {
        }

        bool operator()(const TValue &rhs) const
        {
            return (_comparer(_target, rhs) == false) &&
                   (_comparer(rhs, _target) == false);
        }
    };

    // Internal Fields
    Collection _index;
    TComparer _comparer;
    size_t _sortedCount;
public:
    // Construction/Destruction
    //! @brief Constructs an empty set.
    LinearSortedSet() :
        _sortedCount(0)
    {
    }

    //! @brief Constructs an empty set which inherits the state of its
    //! comparer.
    //! @param[in] comparer The comparer to copy for use managing elements.
    LinearSortedSet(const TComparer &comparer) :
        _comparer(comparer),
        _sortedCount(0)
    {
    }

    ~LinearSortedSet() = default;

    // Accessors
    //! @brief Determines if the map is empty.
    //! @retval true The map contains no associations.
    //! @retval false The map contains at least one association.
    bool isEmpty() const { return _index.empty(); }

    //! @brief Gets the count of associations the map contains.
    size_t getCount() const { return _index.size(); }

    //! @brief Gets the collection of all items, sorted and otherwise.
    const Collection &getCollection() const { return _index; }

    //! @brief Determines if there are elements in the collection which
    //! are not indexed for searching.
    bool needsReindex() const { return _sortedCount < _index.size(); }

    //! @brief Gets the state of the object used to compare elements.
    TComparer &getComparer() { return _comparer; }

    //! @brief Gets a read-only reference to the object used to compare elements.
    const TComparer &getComparer() const { return _comparer; }

    //! @brief Gets the position of the first indexed element.
    iterator begin() { return _index.begin(); }

    //! @brief Gets the position of the first read-only indexed element.
    const_iterator begin() const { return _index.begin(); }

    //! @brief Gets the position after the first indexed element, not
    //! necessarily the last element in the entire collection.
    iterator end() { return _index.begin() + _sortedCount; }

    //! @brief Gets the position after the first read-only indexed element,
    //! not necessarily the last element in the entire collection.
    const_iterator end() const { return _index.begin() + _sortedCount; }

    //! @brief Gets the position after the last element in the entire
    //! collection, both indexed and non-indexed.
    iterator allEnd() { return _index.end(); }

    //! @brief Gets the position after the last read-only element in the entire
    //! collection, both indexed and non-indexed.
    const_iterator allEnd() const { return _index.end(); }

    //! @brief Determines if the set contains a specified value.
    //! @param[in] value The value to search for.
    //! @retval true The set contains at least one instance of the value.
    //! @retval false No values in the set matches the specified value.
    bool contains(const TValue &value) const
    {
        bool hasMatch = false;

        if (_sortedCount > 0)
        {
            // Binary search the sorted elements.
            hasMatch = std::binary_search(_index.begin(),
                                          _index.begin() + _sortedCount,
                                          value, _comparer);
        }

        if ((hasMatch == false) &&
            (_sortedCount < _index.size()))
        {
            // Linear search any remaining elements.
            hasMatch = std::any_of(_index.begin() + _sortedCount,
                                   _index.end(),
                                   MatcherPredicate(_comparer, value));
        }

        return hasMatch;
    }

    //! @brief Searches all items in the collection for the first which
    //! matches a specified value.
    //! @param[in] value The value to search for.
    //! @return The position of the first matching value or allEnd() if no
    //! matches were found.
    iterator find(const TValue &value)
    {
        auto match = findIndexed(value);

        if ((_sortedCount < _index.size()) &&
            (match == _index.end()))
        {
            // Perform a linear search on the unsorted items.
            match = std::find_if(_index.begin() + _sortedCount,
                                 _index.end(),
                                 MatcherPredicate(_comparer, value));
        }

        return match;
    }

    //! @brief Searches all items in the collection for the first which
    //! matches a specified value.
    //! @param[in] value The value to search for.
    //! @return The read-only position of the first matching value or
    //! allEnd() if no matches were found.
    const_iterator find(const TValue &value) const
    {
        auto match = findIndexed(value);

        if ((_sortedCount < _index.size()) &&
            (match == _index.end()))
        {
            // Perform a linear search on the unsorted items.
            match = std::find_if(_index.begin() + _sortedCount,
                                 _index.end(),
                                 MatcherPredicate(_comparer, value));
        }

        return match;
    }

    //! @brief Searches for a value in the set of all indexed values.
    //! @param[in] value The value to search for.
    //! @return The position of the first matching value, or
    //! allEnd() if no matches were found.
    iterator findIndexed(const TValue &value)
    {
        iterator matchPos = _index.end();

        if (_index.empty() == false)
        {
            auto sortedEnd = _index.begin() + _sortedCount;
            auto pos = std::lower_bound(_index.begin(), sortedEnd,
                                        value, _comparer);

            if ((pos != sortedEnd) &&
                (_comparer(*pos, value) == false) &&
                (_comparer(value, *pos) == false))
            {
                // A match was found.
                matchPos = pos;
            }
        }

        return matchPos;
    }

    //! @brief Searches for a read-only value in the set of all indexed values.
    //! @param[in] value The value to search for.
    //! @return The position of the first matching read-only value, or
    //! allEnd() if no matches were found.
    const_iterator findIndexed(const TValue &value) const
    {
        const_iterator matchPos = _index.end();

        if (_index.empty() == false)
        {
            auto sortedEnd = _index.begin() + _sortedCount;
            auto pos = std::lower_bound(_index.begin(), sortedEnd,
                                        value, _comparer);

            if ((pos != sortedEnd) && 
                (_comparer(*pos, value) == false) &&
                (_comparer(value, *pos) == false))
            {
                // A match was found.
                matchPos = pos;
            }
        }

        return matchPos;
    }

    //! @brief Searches for all sorted items which match a specified value.
    //! @param[in] value The value to match.
    //! @return An range of iterators giving the first matching item and
    //! the item after the last match. If no matches were found, both elements
    //! will be allEnd().
    MatchingRange findRange(const TValue &value)
    {
        MatchingRange matches(_index.end());

        if (_index.empty() == false)
        {
            auto matchedRange = std::equal_range(_index.begin(),
                                                 _index.begin() + _sortedCount,
                                                 value, _comparer);

            if (matchedRange.first != matchedRange.second)
            {
                matches = matchedRange;
            }
        }

        return matches;
    }

    //! @brief Searches for all sorted items which match a specified value.
    //! @param[in] value The value to match.
    //! @return An std::pair of const_iterators giving the first matching
    //! item and the item after the last match. If no matches were found,
    //! both elements will be allEnd().
    MatchingCRange findRange(const TValue &value) const
    {
        MatchingCRange matches(_index.end());

        if (_index.empty() == false)
        {
            matches = std::equal_range(_index.begin(),
                                       _index.begin() + _sortedCount,
                                       value, _comparer);
        }

        return matches;
    }

    // Operations
    //! @brief Appends an item to the end of the collection.
    //! @param[in] value The value to copy.
    //! @return A reference to the copy of the value appended to the
    //! collection.
    TValue &push_back(const TValue &value)
    {
        if (_index.empty())
        {
            _sortedCount = 1;
        }
        else if (_sortedCount == _index.size())
        {
            if (_comparer(_index.back(), value) ||
                (_comparer(value, _index.back()) == false))
            {
                // The sorted nature of the collection will be
                // preserved after we append the item.
                ++_sortedCount;
            }
        }

        // Append the item.
        _index.push_back(value);

        return _index.back();
    }

    //! @brief Constructs an item at the end of the collection.
    //! @tparam TArgs The parameters to pass to the constructor of the item.
    //! @param[in] args The construction parameters of the new item.
    //! @return A reference to the value constructed in the collection.
    template<class... TArgs>
    TValue &emplace(TArgs&&... args)
    {
        size_t originalSize = _index.size();
        TValue &emplacedValue = _index.emplace_back(std::forward(args)...);

        if (originalSize < 1)
        {
            // A collection of one item is sorted by definition.
            _sortedCount = 1;
        }
        else if (_sortedCount == originalSize)
        {
            // See if the new item has a value equal to or greater than
            // the previous value.
            TValue &previous = _index.at(originalSize - 1);

            if (_comparer(previous, emplacedValue) &&
                (_comparer(emplacedValue, previous) == false))
            {
                // The entire set is still sorted.
                ++_sortedCount;
            }
        }

        return emplacedValue;
    }

    //! @brief Removes all elements, both indexed and non-indexed, which match a
    //! specified value.
    //! @param[in] value The value to match.
    //! @return The iterator pointing to the item after the last indexed item
    //! removed, or allEnd() if no indexed items were removed.
    iterator erase(const TValue &value)
    {
        iterator next = _index.end();

        if (_sortedCount > 0)
        {
            auto itemRange = std::equal_range(_index.begin(),
                                              _index.begin() + _sortedCount,
                                              value, _comparer);

            if (itemRange.first != itemRange.second)
            {
                size_t deleteCount = std::distance(itemRange.first,
                                                   itemRange.second);
                _sortedCount -= deleteCount;

                next = _index.erase(itemRange.first, itemRange.second);
            }
        }

        if (_sortedCount < _index.size())
        {
            // Linear search for other affected items.
            auto firstMatch = std::remove_if(_index.begin() + _sortedCount,
                                             _index.end(),
                                             MatcherPredicate(_comparer, value));

            // Remove the items shuffled to the end of the collection.
            _index.erase(firstMatch, _index.end());
        }

        return next;
    }

    //! @brief Removes a specific element from the collection.
    //! @param[in] pos The position of the element to remove.
    //! @return The position after the removed element.
    iterator erase(iterator pos)
    {
        iterator next = _index.end();

        if (pos != _index.end())
        {
            size_t offset = std::distance(_index.begin(), pos);

            if (offset < _sortedCount)
            {
                // Record that a sorted element was removed.
                --_sortedCount;
            }

            // Remove the mapping and get the position of the next.
            next = _index.erase(pos);
        }

        return next;
    }

    //! @brief Removes a range of elements from the collection.
    //! @param[in] first The position of the first element to remove.
    //! @param[in] end The position after the last element to remove.
    //! @return The position of the element after the last one removed.
    iterator erase(iterator first, iterator end)
    {
        iterator next = _index.end();

        if (first != _index.end())
        {
            size_t offsetStart = std::distance(_index.begin(), first);
            auto count = std::distance(first, end);

            if (count > 0)
            {
                size_t offsetEnd = offsetStart + count;

                if (_sortedCount <= offsetStart)
                {
                    // No indexed entries are being erased.
                }
                else if (_sortedCount <= offsetEnd)
                {
                    size_t indexedRemoved = _sortedCount - offsetStart;
                    _sortedCount -= indexedRemoved;
                }
                else
                {
                    // All items being erased were indexed.
                    _sortedCount -= count;
                }

                // Remove the elements.
                next = _index.erase(first, end);
            }
        }

        return next;
    }

    //! @brief Ensures that all items in the set are sorted.
    //! @param[in] all True to force all elements to be sorted, perhaps because
    //! the state of the comparer has changed, false to only sort elements
    //! added since the last call to this member function.
    //! @retval true Items within the collection may have moved.
    //! @retval false No sorting took place as the collection contained one
    //! item or less or all items were already sorted.
    bool reindex(bool all = false)
    {
        bool wasSorted = false;

        if (all)
        {
            if (_index.size() > 1)
            {
                std::sort(_index.begin(), _index.end(), _comparer);
                wasSorted = true;
            }
        }
        else if (_sortedCount < _index.size())
        {
            // Sort the non-indexed items.
            std::sort(_index.begin() + _sortedCount,
                      _index.end(), _comparer);

            // Merge the newly sorted items in with the previously
            // sorted items.
            std::inplace_merge(_index.begin(),
                               _index.begin() + _sortedCount,
                               _index.end(),
                               _comparer);

            wasSorted = true;
        }

        // Mark all items as sorted.
        _sortedCount = _index.size();

        return wasSorted;
    }
};

//! @brief An RAII object which defers re-indexing a LinearSortedSet until the
//! exit from a lexical scope.
template<typename TValue, typename TComparer = std::less<TValue>>
class LinearSortedSetIndexer
{
public:
    // Public Types
    using Set = LinearSortedSet<TValue, TComparer>;
private:
    Set &_set;
    bool _all;
public:
    //! @brief Constructs an object which will compile a set on exit.
    //! @param[in] set A reference to the set to compile.
    //! @param[in] all True to force re-indexing all elements, false to only
    //! re-index elements non-indexed elements.
    LinearSortedSetIndexer(Set &set, bool all = false) :
        _set(set),
        _all(all)
    {
    }

    //! @brief Ensures all elements of the set passed to the constructor are
    //! properly indexed.
    ~LinearSortedSetIndexer()
    {
        _set.reindex(_all);
    }
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

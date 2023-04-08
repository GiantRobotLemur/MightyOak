//! @file LinearSortedMap.hpp
//! @brief The declaration of a sorted linear collection containing key/value
//! pairs.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_LINEAR_SORTED_MAP_HPP__
#define __AG_CORE_LINEAR_SORTED_MAP_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstring>

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

#include "CollectionTools.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A sorted linear collection containing key/value pairs.
//! @tparam TKey The data type used to index values.
//! @tparam TValue The data type of values to be indexed.
//! @tparam TComparer The functor object which can perform the less-than
//! comparison on values of type TKey.
//! @details
//! The collection operates like a combination of STL map/multimap and vector.
//! Mappings are stored as std::pair<Key, Value> objects in a vector.
//! Indexed elements appear first ordered by their key. Non-indexed elements
//! appear at the end of the collection.
//! 
//! New elements are always added to the end of the collection, the sorted state
//! will be maintained as long as possible without actually sorting depending on
//! the keys of mappings added. The non-indexed elements will be sorted and
//! integrated into the set of sorted elements by calling the reindex()
//! member function, which does nothing if all elements are already sorted.
//! 
//! The needsReindexing() member function determines if any elements are
//! unsorted. The containsKey() and find() member functions operate across all
//! elements using a combination of binary and linear search. The findIndexed()
//! member function only operates on sorted mappings.
//! 
//! @note The key comparer is stateful. A new comparer is created at
//! construction, but it has a state which can be manipulated between
//! re-indexes.
template<typename TKey, typename TValue, typename TComparer = std::less<TKey>>
class LinearSortedMap
{
public:
    // Public Types
    using KeyValuePair = std::pair<TKey, TValue>;
    using MappingCollection = std::vector<KeyValuePair>;
    using MappingIter = typename MappingCollection::iterator;
    using MappingCIter = typename MappingCollection::const_iterator;
    using MatchingRange = IteratorRange<MappingIter>;
    using MatchingCRange = IteratorRange<MappingCIter>;

private:
    // Internal Types
    //! @brief An adapter which compares mapping objects using the
    //! key comparer for usee with std::lower_bound() and std::equal_range().
    struct MappingComparer
    {
    private:
        const TComparer &_keyComparer;
    public:
        MappingComparer(const TComparer &keyComparer) :
            _keyComparer(keyComparer)
        {
        }

        bool operator()(const KeyValuePair &lhs,
                        const KeyValuePair &rhs) const
        {
            return _keyComparer(lhs.first, rhs.first);
        }
    };

    //! @brief A predicate which performs equality comparisons between a fixed
    //! key and a mapping object for use with std::find_if().
    struct MappingEquality
    {
    private:
        const TComparer &_keyComparer;
        const TKey &_target;
    public:
        MappingEquality(const TComparer &keyComparer, const TKey &target) :
            _keyComparer(keyComparer),
            _target(target)
        {
        }

        bool operator()(const KeyValuePair &rhs) const
        {
            return (_keyComparer(_target, rhs.first) == false) &&
                   (_keyComparer(rhs.first, _target) == false);
        }
    };

    //! @brief Defines a block of memory large enough to fit a mapping in.
    using KeyValuePairBlock = uint8_t[sizeof(KeyValuePair)];

    //! @brief A functor which manually destroys the key part of a mapping.
    struct DeleteKeyOnlyMapping
    {
        void operator()(KeyValuePair *mapping)
        {
            // Manually call the destructor on the in-place
            // constructed key.
            mapping->first.~TKey();
        }
    };

    //! @brief A unique_ptr to an in-place allocated mapping in which only
    //! the key has been constructed and therefore requires manual destruction.
    using KeyOnlyMappingUPtr = std::unique_ptr<KeyValuePair, DeleteKeyOnlyMapping>;

    // Internal Functions
    bool isKeyLessOrEqual(const TKey &lhs, const TKey &rhs)
    {
        return (_keyComparer(lhs, rhs) ||
                (_keyComparer(rhs, lhs) == false));
    }

    //! @brief Creates a mapping in a provided block of memory in which only
    //! the key has been constructed, the value remains uninitialised.
    //! @param[in] key The value to copy into the key field of the mapping.
    //! @param[in] block The block in which to construct the mapping.
    //! @return A unique pointer which will ensure only the key field of
    //! the in-place constructed mapping is disposed of at destruction.
    //! @detail
    //! This is a very cheeky bit of code which (reasonably?) assumes that
    //! an std::pair merely constructs its two fields in turn and destroys
    //! them, with no other processing.
    //! 
    //! The code uses an appropriately sized block of bytes to in-place
    //! initialise the key (first) field, but leaves the other blank. The
    //! resultant pointer is wrapped in a unique_ptr which will ensure that
    //! only the key field will have its destructor called, if required,
    //! when the pointer goes out of scope.
    //! 
    //! The whole reason for this is to be able to use the STL lower_bound()
    //! and equal_range() functions without having to default construct a
    //! dummy mapping containing a value. The comparer should only operate
    //! on key data, so the value field (second) should never be accessed.
    static KeyOnlyMappingUPtr makeKeyOnlyMapping(const TKey &key,
                                                 KeyValuePairBlock &block)
    {
        KeyValuePair *kvp = reinterpret_cast<KeyValuePair *>(block);

        // In-place construct the key.
        new (&kvp->first) TKey(key);

        // Blank the value if it's more than a machine word.
        if constexpr (sizeof(kvp->second) > sizeof(size_t))
        {
            // Although we're being naughty, we should at least zero
            // the field, no matter what it's structure.
            std::memset(static_cast<void *>(&kvp->second),
                        0, sizeof(kvp->second));
        }

        return KeyOnlyMappingUPtr(kvp);
    }

    // Internal Fields
    MappingCollection _mappings;
    TComparer _keyComparer;
    size_t _sortedCount;
public:
    // Construction/Destruction
    //! @brief Constructs an empty map.
    LinearSortedMap() :
        _sortedCount(0)
    {
    }

    //! @brief Constructs a map which inherits the state of its comparer.
    //! @param[in] keyComparer The comparer to copy in order to manage keys
    //! in the collection.
    LinearSortedMap(const TComparer &keyComparer) :
        _keyComparer(keyComparer),
        _sortedCount(0)
    {
    }

    //! @brief Constructs a map which inherits the state of its comparer.
    //! @tparam TCollection A collection which supports the size(), begin() and
    //! end() member functions defined by standard STL collections.
    //! @param[in] stlCollection A collection of KeyValuePair objects to copy.
    //! @note The collection will be re-indexed after the elements are copied.
    template<typename TCollection>
    LinearSortedMap(const TCollection &stlCollection) :
        _sortedCount(0)
    {
        _mappings.reserve(stlCollection.size());

        // Append the elements.
        appendRange(stlCollection.begin(), stlCollection.end());

        // Re-index if necessary.
        reindex();
    }

    //! @brief Ensures that all elements are properly disposed of.
    ~LinearSortedMap() = default;

    // Accessors
    //! @brief Determines if the map is empty.
    //! @retval true The map contains no associations.
    //! @retval false The map contains at least one association.
    bool isEmpty() const { return _mappings.empty(); }

    //! @brief Gets the count of associations the map contains.
    size_t getCount() const { return _mappings.size(); }

    //! @brief Gets the count of unique keys which occur in the map.
    size_t getUniqueCount() const
    {
        size_t uniqueCount = 0;

        if (_mappings.empty() == false)
        {
            uniqueCount = 1;
            auto prev = _mappings.begin();
            auto next = prev;
            MappingComparer comparer(_keyComparer);

            for (++next; next != _mappings.end(); ++next)
            {
                if (_keyComparer(prev->first, next->first))
                {
                    // Next is different.
                    ++uniqueCount;
                    prev = next;
                }
            }
        }

        return uniqueCount;
    }

    //! @brief Gets the collection of all mappings, sorted and otherwise.
    const MappingCollection &getMappings() const { return _mappings; }

    //! @brief Determines if there are elements in the collection which
    //! are not indexed for searching.
    bool needsReindex() const { return _sortedCount < _mappings.size(); }

    //! @brief Gets the state of the object used to compare keys.
    TComparer &getKeyComparer() { return _keyComparer; }

    //! @brief Gets a read-only reference to the object used to compare keys.
    const TComparer &getKeyComparer() const { return _keyComparer; }

    //! @brief Searches for a mapping which matches a specified key amongst
    //! all those currently defined in the map, compiled and unordered.
    //! @param[in] key The value of the key to search for.
    //! @retval true A mapping matching the key exists.
    //! @retval false No mappings had a matching key, compiled or otherwise.
    bool containsKey(const TKey &key) const
    {
        bool hasMatch = false;

        if (_mappings.empty() == false)
        {
            // Creates a KeyValuePair object only populated with a key.
            KeyValuePairBlock keyBlock;
            KeyOnlyMappingUPtr keyMapping = makeKeyOnlyMapping(key, keyBlock);
            MappingCIter sortedEnd = _mappings.begin() + _sortedCount;

            hasMatch = std::binary_search(_mappings.begin(), sortedEnd,
                                          *keyMapping.get(),
                                          MappingComparer(_keyComparer));

            if ((hasMatch == false) && (sortedEnd != _mappings.end()))
            {
                // Perform brute force search on the unsorted set.
                MappingEquality matcher(_keyComparer, key);

                hasMatch = std::any_of(sortedEnd,
                                       _mappings.end(), matcher);
            }
        }

        return hasMatch;
    }

    //! @brief Gets the position of the first mapping in the collection.
    MappingIter begin() { return _mappings.begin(); }

    //! @brief Gets the position after the last indexed element in the
    //! collection.
    //! @return An iterator which may be the position of the first non-indexed
    //! element in the collection, or the end.
    MappingIter end() { return _mappings.begin() + _sortedCount; }

    //! @brief Gets the position after the last mapping in the entire
    //! collection.
    MappingIter mappingsEnd() { return _mappings.end(); }

    //! @brief Gets the position of the first read-only mapping in the
    //! collection.
    MappingCIter begin() const { return _mappings.begin(); }

    //! @brief Gets the position after the last read-only indexed element
    //! in the collection.
    //! @return A read-only iterator which may be the position of the first
    //! non-indexed element in the collection, or the end.
    MappingCIter end() const { return _mappings.begin() + _sortedCount; }

    //! @brief Gets the position after the last read-only mapping in
    //! the entire collection.
    MappingCIter mappingsEnd() const { return _mappings.end(); }

    //! @brief Erases all mappings matching a specific key.
    //! @param[in] key The key of elements to match.
    //! @return The position of the next indexed element after the last
    //! one removed, or mappingsEnd() if no indexed elements were removed.
    MappingIter erase(const TKey &key)
    {
        MappingIter next = _mappings.end();

        if (_sortedCount > 0)
        {
            KeyValuePairBlock keyBlock;
            KeyOnlyMappingUPtr keyMapping = makeKeyOnlyMapping(key, keyBlock);
            MappingIter sortedEnd = _mappings.begin() + _sortedCount;

            auto sortedRange = std::equal_range(_mappings.begin(),
                                                sortedEnd, *keyMapping,
                                                MappingComparer(_keyComparer));

            if (sortedRange.first != sortedRange.second)
            {
                // Remove the indexed elements.
                size_t removedCount = std::distance(sortedRange.first,
                                                    sortedRange.second);

                next = _mappings.erase(sortedRange.first,
                                       sortedRange.second);

                _sortedCount -= removedCount;
            }

            if (_sortedCount < _mappings.size())
            {
                MappingEquality matcher(_keyComparer, key);

                auto firstMatch = std::remove_if(_mappings.begin() + _sortedCount,
                                                 _mappings.end(), matcher);

                _mappings.erase(firstMatch, _mappings.end());
            }
        }

        return next;
    }

    //! @brief Erases a specific mapping.
    //! @param[in] pos The iterator giving the position of the element top remove.
    //! @return The position of the next element after the one removed,
    //! or mappingsEnd() if specified element was at the end of the collection.
    MappingIter erase(MappingIter pos)
    {
        MappingIter next = _mappings.end();

        if (pos != _mappings.end())
        {
            size_t offset = std::distance(_mappings.begin(), pos);

            if (offset < _sortedCount)
            {
                // Record that a sorted element was removed.
                --_sortedCount;
            }

            // Remove the mapping and get the position of the next.
            next = _mappings.erase(pos);
        }

        return next;
    }

    //! @brief Removes a range of mappings from the collection.
    //! @param[in] first The position of the first mapping to remove.
    //! @param[in] end The position after the last mapping to remove.
    //! @return The position of the element after the last one removed.
    MappingIter erase(MappingIter first, MappingIter end)
    {
        MappingIter next = _mappings.end();

        if (first != _mappings.end())
        {
            size_t offsetStart = std::distance(_mappings.begin(), first);
            ptrdiff_t count = std::distance(first, end);

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

                // Remove the mappings.
                next = _mappings.erase(first, end);
            }
        }

        return next;
    }

    //! @brief Attempts to find the first value with an associated key.
    //! @param[in] key The key used to index the value.
    //! @param[out] value Receives a copy of the value if one is found.
    //! @retval true A matching mapping was found and the value returned.
    //! @retval false No matching value was found.
    bool tryFind(const TKey &key, TValue &value) const
    {
        auto pos = find(key);
        bool hasMatch = false;

        if (pos != _mappings.end())
        {
            value = pos->second;
            hasMatch = true;
        }

        return hasMatch;
    }

    //! @brief Searches for the first mapping which matches a specific key,
    //! first in the indexed mappings, then in the non-indexed mappings.
    //! @param[in] key The key to search for in the collection.
    //! @returns Either the position of the first matching element or
    //! mappingsEnd() if no matches are found.
    MappingIter find(const TKey &key)
    {
        MappingIter match = findIndexed(key);

        if ((match == _mappings.end()) &&
            (_sortedCount < _mappings.size()))
        {
            MappingEquality matcher(_keyComparer, key);

            match = std::find_if(_mappings.begin() + _sortedCount,
                                 _mappings.end(), matcher);
        }

        return match;
    }

    //! @brief Searches for the first mapping which matches a specific key,
    //! first in the indexed mappings, then in the non-indexed mappings.
    //! @param[in] key The key to search for in the collection.
    //! @returns Either the position of the first matching element or
    //! mappingsEnd() if no matches are found.
    MappingCIter find(const TKey &key) const
    {
        MappingCIter match = findIndexed(key);

        if ((match == _mappings.end()) &&
            (_sortedCount < _mappings.size()))
        {
            MappingEquality matcher(_keyComparer, key);

            match = std::find_if(_mappings.begin() + _sortedCount,
                                 _mappings.end(), matcher);
        }

        return match;
    }

    //! @brief Searches for the first mapping in the compiled range which
    //! has a specified key.
    //! @param[in] key The value of the key to search for.
    //! @return The position of the first matching mapping or mappingsEnd().
    //! @note Any unsorted mappings will not be searched.
    MappingIter findIndexed(const TKey &key)
    {
        MappingIter match = _mappings.end();

        if (_mappings.empty() == false)
        {
            // Creates a KeyValuePair object only populated with a key.
            KeyValuePairBlock keyBlock;
            KeyOnlyMappingUPtr keyMapping = makeKeyOnlyMapping(key, keyBlock);
            MappingIter sortedEnd = _mappings.begin() + _sortedCount;

            auto pos = std::lower_bound(_mappings.begin(),
                                        sortedEnd,
                                        *keyMapping,
                                        MappingComparer(_keyComparer));

            if ((pos != sortedEnd) &&
                (_keyComparer(pos->first, key) == false) &&
                (_keyComparer(key, pos->first) == false))
            {
                // The keys are equal.
                match = pos;
            }
        }

        return match;
    }

    //! @brief Searches for the first mapping in the compiled range which
    //! has a specified key.
    //! @param[in] key The value of the key to search for.
    //! @return The position of the first matching mapping or mappingsEnd().
    //! @note Any unsorted mappings will not be searched.
    MappingCIter findIndexed(const TKey &key) const
    {
        MappingCIter match = _mappings.end();

        if (_mappings.empty() == false)
        {
            // Creates a KeyValuePair object only populated with a key.
            KeyValuePairBlock keyBlock;
            KeyOnlyMappingUPtr keyMapping = makeKeyOnlyMapping(key, keyBlock);
            MappingCIter sortedEnd = _mappings.begin() + _sortedCount;

            auto pos = std::lower_bound(_mappings.begin(),
                                        sortedEnd,
                                        *keyMapping,
                                        MappingComparer(_keyComparer));

            if ((pos != sortedEnd) &&
                (_keyComparer(pos->first, key) == false) &&
                (_keyComparer(key, pos->first) == false))
            {
                // The keys are equal.
                match = pos;
            }
        }

        return match;
    }

    //! @brief Searches for the range of indexed mappings which match a
    //! specified key.
    //! @param[in] key The value of the key to search for.
    //! @returns A std::pair of mapping iterators point to the first
    //! matching item and the item after the last match, or both set
    //! to the end of the mapping collection.
    //! @note Only indexed mappings are searched. Unsorted mappings are
    //! ignored.
    MatchingRange findRange(const TKey &key)
    {
        MatchingRange range(_mappings.end());

        if (_mappings.empty() == false)
        {
            // Creates a KeyValuePair object only populated with a key.
            KeyValuePairBlock keyBlock;
            KeyOnlyMappingUPtr keyMapping = makeKeyOnlyMapping(key, keyBlock);
            MappingIter sortedEnd = _mappings.begin() + _sortedCount;

            auto equal = std::equal_range(_mappings.begin(),
                                          sortedEnd,
                                          *keyMapping,
                                          MappingComparer(_keyComparer));

            if (equal.first != equal.second)
            {
                // Only assign the range if it is valid.
                range = equal;
            }
        }

        return range;
    }

    //! @brief Searches for the range of indexed mappings which match a
    //! specified key.
    //! @param[in] key The value of the key to search for.
    //! @returns A std::pair of mapping iterators point to the first
    //! matching item and the item after the last match, or both set
    //! to the end of the mapping collection.
    //! @note Only indexed mappings are searched. Unsorted mappings are
    //! ignored.
    MatchingCRange findRange(const TKey &key) const
    {
        MatchingCRange range(_mappings.end(), _mappings.end());

        if (_mappings.empty() == false)
        {
            // Creates a KeyValuePair object only populated with a key.
            KeyValuePairBlock keyBlock;
            KeyOnlyMappingUPtr keyMapping = makeKeyOnlyMapping(key, keyBlock);
            MappingCIter sortedEnd = _mappings.begin() + _sortedCount;

            auto equal = std::equal_range(_mappings.begin(),
                                          sortedEnd,
                                          *keyMapping,
                                          MappingComparer(_keyComparer));

            if (equal.first != equal.second)
            {
                // Only assign the range if it is valid.
                range = equal;
            }
        }

        return range;
    }

    //! @brief Attempts to find the first run of mappings with the same key.
    //! @param[out] range An object to receive the pair of iterators defining
    //! the first group.
    //! @retval true A group was found and returned.
    //! @retval false The collection was empty.
    //! @note The function will ensure the collection is properly indexed
    //! before querying with a call to reindex().
    bool tryFindFirstKeyGroup(MatchingRange &range)
    {
        reindex();

        return getFirstGroup(_mappings.begin(), _mappings.end(),
                             range, MappingComparer(_keyComparer));
    }

    //! @brief Attempts to find the next run of mappings with the same key.
    //! @param[in,out] range An object defining the previous range to be
    //! updated with the next range of items, if any.
    //! @retval true Another group of identical keys was found and returned.
    //! @retval false The range pointed to the end of the collection.
    bool tryFindNextKeyGroup(MatchingRange &range)
    {
        return getNextGroup(_mappings.end(), range, MappingComparer(_keyComparer));
    }

    //! @brief Attempts to find the first run of mappings with the same key.
    //! @param[out] range An object to receive the pair of iterators defining
    //! the first group.
    //! @retval true A group was found and returned.
    //! @retval false The collection was empty.
    //! @note As this function is const, it cannot ensure the collection is
    //! properly indexed before querying. The caller must take care of this.
    bool tryFindFirstKeyGroup(MatchingCRange &range) const
    {
        return getFirstGroup(_mappings.begin(), _mappings.end(),
                             range, MappingComparer(_keyComparer));
    }

    //! @brief Attempts to find the next run of mappings with the same key.
    //! @param[in,out] range An object defining the previous range to be
    //! updated with the next range of items, if any.
    //! @retval true Another group of identical keys was found and returned.
    //! @retval false The range pointed to the end of the collection.
    bool tryFindNextKeyGroup(MatchingCRange &range) const
    {
        return getNextGroup(_mappings.end(), range, MappingComparer(_keyComparer));
    }

    // Operations
    //! @brief Provides a hint for the number of mappings the collection may
    //! have to store.
    void reserve(size_t hintCount)
    {
        _mappings.reserve(hintCount);
    }

    //! @brief Removes all mappings from the collection.
    void clear()
    {
        _sortedCount = 0;
        _mappings.clear();
    }
    
    //! @brief Appends a range of key/value pairs into the map, maintaining
    //! the sorted order is possible.
    //! @tparam MapIterator An iterator which points to a mapping.
    //! @param[in] rangeBegin An external iterator pointing to the first
    //! mapping to copy.
    //! @param[in] rangeEnd An external iterator pointing to the mapping after
    //! the last one to copy.
    template<typename MapIterator>
    void appendRange(const MapIterator &rangeBegin, const MapIterator &rangeEnd)
    {
        if (_sortedCount == _mappings.size())
        {
            // The collection is currently sorted, attempt to maintain
            // that property.
            for (MapIterator pos = rangeBegin; pos != rangeEnd; ++pos)
            {
                if (_mappings.empty())
                {
                    // A map of size 1 is sorted by definition.
                    _sortedCount = 1;
                    _mappings.push_back(*pos);
                }
                else
                {
                    // See if the next key is greater or equal to the last one.
                    const auto &previousKey = _mappings.back().first;
                    const auto &currentKey = pos->first;

                    if (isKeyLessOrEqual(previousKey, currentKey))
                    {
                        // The next key was in sorted order.
                        ++_sortedCount;
                        _mappings.push_back(*pos);
                    }
                    else
                    {
                        // The run of sorted mappings has ended, simply copy
                        // the rest of the unsorted elements.
                        std::copy(pos, rangeEnd, std::back_inserter(_mappings));
                        break;
                    }
                }
            }
        }
        else
        {
            // Just copy in the unsorted items.
            std::copy(rangeBegin, rangeEnd,
                      std::back_inserter(_mappings));
        }
    }

    //! @brief Ensures that any unsorted mappings are integrated into
    //! the sorted collection.
    //! @param[in] all True to resort all mappings, whether they were
    //! previously in a sorted state or not. False to only sort the
    //! unsorted elements and merge them into the sorted set.
    //! @retval true The collection was re-indexed.
    //! @retval false The collection was already in a sorted state.
    //! @note The all option maybe necessary if the state of the comparer
    //! has changed to the point where the previous sorted state is invalidated.
    bool reindex(bool all = false)
    {
        bool wasUpdated = false;

        if (all)
        {
            if (_mappings.size() > 1)
            {
                // Sort all elements, no matter if they were previously in
                // a sorted state.
                std::sort(_mappings.begin(), _mappings.end(),
                          MappingComparer(_keyComparer));
                wasUpdated = true;
            }
        }
        else
        {
            // Only sort elements which weren't previously sorted.
            if ((_mappings.size() > 1) ||
                (_sortedCount < _mappings.size()))
            {
                MappingComparer comp(_keyComparer);

                // Sort the trailing elements.
                std::sort(_mappings.begin() + _sortedCount,
                          _mappings.end(), comp);

                std::inplace_merge(_mappings.begin(),
                                   _mappings.begin() + _sortedCount,
                                   _mappings.end(), comp);

                wasUpdated = true;
            }
        }

        // Mark all elements as sorted.
        _sortedCount = _mappings.size();

        return wasUpdated;
    }

    //! @brief Adds a mapping to the end of the collection, possibly
    //! invalidating the sorted state of the map.
    //! @param[in] key The key used to index the value.
    //! @param[in] value The value to index.
    //! @returns A reference to the mapping added to the collection.
    KeyValuePair &push_back(const TKey &key, const TValue &value)
    {
        // Determine if we can append the item and remain sorted.
        if (_mappings.empty())
        {
            // A set of one mapping is always sorted.
            _mappings.emplace_back(key, value);
            _sortedCount = _mappings.size();
        }
        else if (_sortedCount == _mappings.size())
        {
            if (isKeyLessOrEqual(_mappings.back().first, key))
            {
                // The new key is less than or equal to the last in
                // the sorted range, so we can add to the end without
                // starting an unsorted range.
                _mappings.emplace_back(key, value);
                _sortedCount = _mappings.size();
            }
            else
            {
                // The new key is greater than the last item in
                // the sorted range, so it will be added as the first
                // of the unsorted range.
                _mappings.emplace_back(key, value);
                _sortedCount = _mappings.size() - 1;
            }
        }
        else
        {
            // An unsorted range exists at the end of the sorted elements.
            // Add the mapping anyway and we'll sort it in the next call
            // to compile().
            _mappings.emplace_back(key, value);
        }

        return _mappings.back();
    }
};

//! @brief An RAII object which defers re-indexing a LinearSortedMap until the
//! exit from a lexical scope.
template<typename TKey, typename TValue, typename TComparer = std::less<TKey>>
class LinearSortedMapIndexer
{
public:
    // Public Types
    using Map = LinearSortedMap<TKey, TValue, TComparer>;
private:
    Map &_map;
    bool _all;
public:
    //! @brief Constructs an object which will compile a map on exit.
    //! @param[in] map A reference to the map to compile.
    //! @param[in] all True to force re-indexing all elements, false to only
    //! re-index elements non-indexed elements.
    LinearSortedMapIndexer(Map &map, bool all = false) :
        _map(map),
        _all(all)
    {
    }

    //! @brief Ensures all elements of the map passed to the constructor are
    //! properly indexed.
    ~LinearSortedMapIndexer()
    {
        _map.reindex(_all);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

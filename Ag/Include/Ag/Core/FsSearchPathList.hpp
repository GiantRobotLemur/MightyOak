//! @file Ag/Core/FsSearchPathList.hpp
//! @brief The declaration of an object representing an ordered list of search
//! paths.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_FS_SEARCH_PATH_LIST_HPP__
#define __AG_CORE_FS_SEARCH_PATH_LIST_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <deque>

#include "FsPath.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Fs {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class Entry;

//! @brief A collection containing FsPath objects in when entries can be repeated.
//! @note Intended for defining a set of directory paths to search in a specific
//! order, perhaps with items added to the beginning of the order which might
//! be duplicated further down.
class SearchPathList
{
private:
    // Internal Types
    struct SearchItem
    {
        Path Source;
        uint32_t UsageCount;

        SearchItem(const Path &path);
        ~SearchItem() = default;
    };

    using SearchItemQueue = std::deque<SearchItem>;
public:
    // Public Types
    class iterator
    {
    private:
        const SearchItemQueue *_source;
        size_t _position;

        bool tryFindNext(size_t &index) const;
        bool tryFindPrevious(size_t &index) const;
    public:
        iterator();
        iterator(const SearchItemQueue *source, size_t position);
        ~iterator() = default;

        // Required Bi-directional Iterator Members

        // Public Types
        //! @brief The standard iterator typedef for the type of object the
        //! iterator dereferences into.
        typedef Path value_type;

        //! @brief The standard iterator typedef for the type of reference
        // which exposes the item the iterator represents.
        typedef Path &reference;

        //! @brief The standard iterator typedef for the type of pointer
        // which exposes the item the iterator represents.
        typedef Path *pointer;

        //! @brief The standard iterator typedef for the category the iterator
        //! belongs to.
        typedef std::bidirectional_iterator_tag iterator_category;

        //! @brief The standard iterator typedef for the type returned when
        //! one iterator instance is subtracted from another.
        typedef SearchItemQueue::difference_type difference_type;

        // Required Member Functions
        const Path &operator*() const;
        const Path *operator->() const;
        iterator &operator++();
        iterator operator++(int);
        iterator &operator--();
        iterator operator--(int);
        bool operator==(const iterator &rhs) const;
        bool operator!=(const iterator &rhs) const;
    };

    // Construction/Destruction
    SearchPathList() = default;
    ~SearchPathList() = default;

    // Accessors
    bool isEmpty() const;
    size_t getCount() const;
    size_t getUniqueCount() const;
    bool tryFind(string_cref_t fsPath, Entry &result) const;
    bool tryFind(const Path &fsItem, Entry &result) const;
    iterator begin() const;
    iterator end() const;

    // Operations
    void append(const Path &searchPath);
    void prepend(const Path &searchPath);
    void popFront();
    void popBack();
private:
    // Internal Functions
    void incrementMatching(const Path &path);
    void decrementMatching(const Path &path);
    uint32_t countMatching(const Path &path) const;

    // Internal Fields
    SearchItemQueue _searchItems;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Fs

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

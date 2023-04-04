//! @file Core/FsSearchPathList.cpp
//! @brief The definition of an object representing an ordered set of search
//! paths.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/FsDirectory.hpp"
#include "Ag/Core/FsSearchPathList.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Fs {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// SearchPathList Member Definitions
////////////////////////////////////////////////////////////////////////////////
SearchPathList::SearchItem::SearchItem(const Path &path) :
    Source(path),
    UsageCount(0)
{
}

bool SearchPathList::iterator::tryFindNext(size_t &index) const
{
    bool hasNext = false;

    if ((_source != nullptr) && (index < _source->size()))
    {
        size_t maxItem = _source->size();

        for (++index; index < maxItem; ++index)
        {
            if (_source->at(index).UsageCount == 0)
            {
                hasNext = true;
                break;
            }
        }
    }

    return hasNext;
}

bool SearchPathList::iterator::tryFindPrevious(size_t &index) const
{
    bool hasPrevious = false;

    if ((_source != nullptr) && (index > 0))
    {
        size_t maxItem = _source->size();

        // Loop until a unique item is found, or the counter wraps.
        for (--index; index < maxItem; --index)
        {
            if (_source->at(index).UsageCount == 0)
            {
                hasPrevious = true;
                break;
            }
        }
    }

    return hasPrevious;
}

SearchPathList::iterator::iterator() :
    _source(nullptr),
    _position(0)
{
}

SearchPathList::iterator::iterator(const SearchItemQueue *source, size_t position) :
    _source(source),
    _position(position)
{
}


const Path &SearchPathList::iterator::operator*() const
{
    return _source->at(_position).Source;
}

const Path *SearchPathList::iterator::operator->() const
{
    return &_source->at(_position).Source;
}

SearchPathList::iterator &SearchPathList::iterator::operator++()
{
    if (_source != nullptr)
    {
        // Iterate until the end or a unique path is found.
        size_t next = _position;

        if (tryFindNext(next))
        {
            _position = next;
        }
        else
        {
            _position = _source->size();
        }
    }

    return *this;
}

SearchPathList::iterator SearchPathList::iterator::operator++(int)
{
    size_t original = _position;

    if (_source != nullptr)
    {
        // Iterate until the end or a unique path is found.
        size_t next = _position;

        if (tryFindNext(next))
        {
            _position = next;
        }
        else
        {
            _position = _source->size();
        }
    }

    return iterator(_source, original);
}

SearchPathList::iterator &SearchPathList::iterator::operator--()
{
    if (_source != nullptr)
    {
        // Iterate until the end or a unique path is found.
        size_t previous = _position;

        if (tryFindPrevious(previous))
        {
            _position = previous;
        }
        else
        {
            _position = 0;
        }
    }

    return *this;
}

SearchPathList::iterator SearchPathList::iterator::operator--(int)
{
    size_t original = _position;

    if (_source != nullptr)
    {
        // Iterate until the end or a unique path is found.
        size_t previous = _position;

        if (tryFindPrevious(previous))
        {
            _position = previous;
        }
        else
        {
            _position = 0;
        }
    }

    return iterator(_source, original);
}

bool SearchPathList::iterator::operator==(const SearchPathList::iterator &rhs) const
{
    return (_source == rhs._source) &&
           (_position == rhs._position);
}

bool SearchPathList::iterator::operator!=(const SearchPathList::iterator &rhs) const
{
    return (_source != rhs._source) ||
           (_position != rhs._position);
}

bool SearchPathList::isEmpty() const
{
    return _searchItems.empty();
}

size_t SearchPathList::getCount() const
{
    return _searchItems.size();
}

size_t SearchPathList::getUniqueCount() const
{
    size_t count = 0;

    for (const SearchItem &item : _searchItems)
    {
        if (item.UsageCount == 0)
            ++count;
    }

    return count;
}

bool SearchPathList::tryFind(string_cref_t fsPath, Entry &result) const
{
    Path target;
    String error;

    if (Path::tryParse(fsPath, target, error))
    {
        return tryFind(target, result);
    }
    else
    {
        throw InvalidFilePathException(fsPath, error);
    }
}

bool SearchPathList::tryFind(const Path &fsItem, Entry &result) const
{
    bool hasMatch = false;

    if (fsItem.hasRoot())
    {
        // The path is absolute, no need to search through paths.
        Entry item(fsItem);
        item.refresh();

        if (item.exists())
        {
            result = item;
            hasMatch = true;
        }
    }
    else
    {
        for (const SearchItem &searchPath : _searchItems)
        {
            // Ensure the path isn't a duplicate.
            if (searchPath.UsageCount != 0)
                continue;

            // It's the first occurrence of the path in this set.
            Path resolvedPath = fsItem.convertToAbsolute(searchPath.Source);
            Entry item(resolvedPath);
            item.refresh();

            if (item.exists())
            {
                result = item;
                hasMatch = true;
                break;
            }
        }
    }

    return hasMatch;
}

SearchPathList::iterator SearchPathList::begin() const
{
    return iterator(&_searchItems, 0);
}

SearchPathList::iterator SearchPathList::end() const
{
    return iterator(&_searchItems, _searchItems.size());
}

void SearchPathList::append(const Path &searchPath)
{
    // Simply add to the back.
    Path absPath = searchPath;
    absPath.convertToAbsolute();
    Path key = absPath.makeCanonical();

    uint32_t count = countMatching(key);

    SearchItem &item = _searchItems.emplace_back(key);
    item.UsageCount = count;
}

void SearchPathList::prepend(const Path &searchPath)
{
    // Increase the usage count of all duplicate items
    // in the set.
    Path absPath = searchPath;
    absPath.convertToAbsolute();
    Path key = absPath.makeCanonical();

    incrementMatching(key);

    _searchItems.push_front(key);
}

void SearchPathList::popFront()
{
    if (_searchItems.empty() == false)
    {
        decrementMatching(_searchItems.front().Source);
        _searchItems.pop_front();
    }
}

void SearchPathList::popBack()
{
    if (_searchItems.empty() == false)
    {
        // Remove the item without changing any usage counts as the current
        // item should have had the highest count.
        _searchItems.pop_back();
    }
}

void SearchPathList::incrementMatching(const Path &path)
{
    for (SearchItem &item : _searchItems)
    {
        if (item.Source == path)
        {
            ++item.UsageCount;
        }
    }
}

void SearchPathList::decrementMatching(const Path &path)
{
    for (SearchItem &item : _searchItems)
    {
        if (item.Source == path)
        {
            --item.UsageCount;
        }
    }
}

uint32_t SearchPathList::countMatching(const Path &path) const
{
    uint32_t count = 0;

    for (const SearchItem &item : _searchItems)
    {
        if (item.Source == path)
        {
            ++count;
        }
    }

    return count;
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Fs
////////////////////////////////////////////////////////////////////////////////


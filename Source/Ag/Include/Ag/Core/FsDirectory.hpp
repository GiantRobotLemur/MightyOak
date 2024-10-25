//! @file Ag/Core/FsDirectory.hpp
//! @brief The declaration of abstractions of file system elements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_FS_DIRECTORY_HPP__
#define __AG_CORE_FS_DIRECTORY_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "FsPath.hpp"

namespace Ag {

//! @brief Defines primitives for use in manipulating the file system.
namespace Fs {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class EntryPrivate;

//! @brief Represents an object stored on a file system.
class Entry
{
public:
    // Construction/Destruction
    Entry() = default;
    Entry(const Path &path);
    Entry(string_cref_t path);
    Entry(const Path &path, string_cref_t fileName);
    Entry(std::shared_ptr<EntryPrivate> &status);

    // Accessors
    bool exists() const;
    bool isFile() const;
    bool isDirectory() const;
    String getName() const;
    const Path &getPath() const;
    int64_t getSize() const;

    // Operations
    void refresh();
private:
    // Internal Fields
    std::shared_ptr<EntryPrivate> _entry;
};

using EntryVector = std::vector<Entry>;

//! @brief Represents a group of file system objects under a single path.
class Directory
{
public:
    // Public Types
    //! @brief Defines the flags passed to the getNames() and getEntries()
    //! member functions.
    enum Query
    {
        //! @brief Indicates returned entries should be sorted by name.
        SortAscending = 0x01,

        //! @brief Indicates returned entries should be reverse sorted by name.
        SortDescending = 0x02,

        //! @brief Indicates that the '.' and '..' entries should be included.
        IncludeVirtualEntries = 0x04,

        //! @brief Indicates that files should be returned.
        IncludeFiles = 0x08,

        //! @brief Indicates that directories should be included.
        IncludeDirectories = 0x10,

        //! @brief A mask used to determine what file system elements should be
        //! returned by a query. Specifying nothing returns all elements.
        IncludeMask = 0x18,
    };

    // Construction/Destruction
    Directory(const Path &path);
    Directory(string_cref_t path);
    Directory(const Path &parentPath, string_cref_t dirName);
    Directory(std::shared_ptr<EntryPrivate> &entry);

    // Accessors
    bool exists() const;
    String getName() const;
    const Path &getPath() const;

    StringCollection getNames() const;
    StringCollection getNames(string_cref_t pattern, uint32_t queryFlags) const;
    EntryVector getEntries() const;
    EntryVector getEntries(string_cref_t pattern, uint32_t queryFlags) const;

    // Operations
private:
    // Internal Fields
    std::shared_ptr<EntryPrivate> _dir;
};

//! @brief An exception thrown when the program fails to access a file.
class FileNotFoundException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    FileNotFoundException(const Path &path);
    ~FileNotFoundException() = default;
};

}} // namespace Ag::Fs

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file FsDirectory.cpp
//! @brief The definition of various abstractions of file system elements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>

#include <algorithm>
#include <memory>

#include "Platform.hpp"
#include "Ag/Core/CodePoint.hpp"
#include "Ag/Core/FsDirectory.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Variant.hpp"

namespace Ag {

//! @brief Groups classes and functions which abstract the file system.
namespace Fs {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct DescendingStrings
{
    bool operator()(string_cref_t lhs, string_cref_t rhs) const
    {
        return rhs < lhs;
    }
};

class EntryPrivate : public std::enable_shared_from_this<EntryPrivate>
{
private:
    // Internal Fields
    Path _location;
    int64_t _size;
    uint32_t _flags;
public:
#ifdef _WIN32
    static const uint32_t Win32NotFile = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE;
#endif

    // Public Types
    enum class Flags : uint32_t
    {
        Exists = 0x01,
        IsFile = 0x02,
        IsDirectory = 0x04,
    };

    // Construction/Destruction
    EntryPrivate() :
        _size(-1),
        _flags(0)
    {
    }

    EntryPrivate(string_cref_t id) :
        _location(id),
        _size(-1),
        _flags(0)
    {
        refresh();
    }

    EntryPrivate(const Path &id) :
        _location(id),
        _size(-1),
        _flags(0)
    {
        refresh();
    }

    EntryPrivate(const Path &parent, string_cref_t fileName) :
        _location(parent, fileName),
        _size(-1),
        _flags(0)
    {
        refresh();
    }

#ifdef _WIN32
    EntryPrivate(const Path &parent, const WIN32_FIND_DATAW &fileInfo) :
        _location(parent, String(fileInfo.cFileName)),
        _size(-1),
        _flags(toScalar(Flags::Exists))
    {
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            _flags |= toScalar(Flags::IsDirectory);
        }
        else if ((fileInfo.dwFileAttributes & Win32NotFile) == 0)
        {
            _flags |= toScalar(Flags::IsFile);
            LARGE_INTEGER size;
            size.LowPart = fileInfo.nFileSizeLow;
            size.HighPart = static_cast<LONG>(fileInfo.nFileSizeHigh);

            _size = size.QuadPart;
        }
    }
#else
    EntryPrivate(const Path &parent, const struct dirent *fileInfo) :
        _location(parent, String(fileInfo->d_name)),
        _size(-1),
        _flags(toScalar(Flags::Exists))
    {
        if (fileInfo->d_type & DT_DIR)
        {
            _flags |= toScalar(Flags::IsDirectory);
        }
        else if (fileInfo->d_type & DT_REG)
        {
            _flags |= toScalar(Flags::IsFile);

            struct stat fileStats;
            String fullPath = _location.toString(PathUsage::Kernel);

            if (stat(fullPath.getUtf8Bytes(), &fileStats) == 0)
            {
                _size = fileStats.st_size;
            }
            else
            {
                // The call to stat() failed. It shouldn't have, so report it.
                std::string fnName;
                fnName.reserve(fullPath.getUtf8Length() + 8);

                fnName.assign("stat('");
                appendAgString(fnName, fullPath);
                fnName.push_back('\'');
                fnName.push_back(')');

                throw RuntimeLibraryException(fnName.c_str(), errno);
            }
        }
    }
#endif

    const Path &getLocation() const { return _location; }

    bool exists() const
    {
        return (_flags & static_cast<std::underlying_type_t<Flags>>(Flags::Exists)) != 0;
    }

    bool isFile() const
    {
        return (_flags & static_cast<std::underlying_type_t<Flags>>(Flags::IsFile)) != 0;
    }

    bool isDirectory() const
    {
        return (_flags & static_cast<std::underlying_type_t<Flags>>(Flags::IsDirectory)) != 0;
    }

    int64_t getSize() const { return _size; }

    // Operations
    void refresh()
    {
        _flags = 0;
        _size = -1;

        if (_location.isEmpty())
            return;

#ifdef _WIN32
        std::wstring path = _location.toWideString(PathUsage::Kernel);

        WIN32_FILE_ATTRIBUTE_DATA attribData;

        if (::GetFileAttributesExW(path.data(), GetFileExInfoStandard,
                                   &attribData) == FALSE)
        {
            uint32_t error = ::GetLastError();

            if (error != ERROR_FILE_NOT_FOUND)
            {
                std::string fn;
                appendFormat("GetFileAttributesEx('{0}')",
                             fn, { String(path) });

                throw Win32Exception(fn.c_str(), error);
            }
        }
        else
        {
            _flags |= toScalar(Flags::Exists);

            if (attribData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                _flags |= toScalar(Flags::IsDirectory);
            }
            else if ((attribData.dwFileAttributes & Win32NotFile) == 0)
            {
                _flags |= toScalar(Flags::IsFile);

                LARGE_INTEGER size;
                size.LowPart = attribData.nFileSizeLow;
                size.HighPart = attribData.nFileSizeHigh;

                _size = size.QuadPart;
            }
        }
#else
        String fullPath = _location.toString(PathUsage::Kernel);
        struct stat fileInfo;

        if (stat(fullPath.getUtf8Bytes(), &fileInfo) == 0)
        {
            _flags |= toScalar(Flags::Exists);

            if (S_ISREG(fileInfo.st_mode))
            {
                _flags |= toScalar(Flags::IsFile);
                _size = fileInfo.st_size;
            }
            else if (S_ISDIR(fileInfo.st_mode))
            {
                _flags |= toScalar(Flags::IsDirectory);
            }
        }
        else if (errno != ENOENT)
        {
            // Throw and exception using the error code and file path.
            std::string fnName;
            fnName.assign("stat('");
            appendAgString(fnName, fullPath);
            fnName.push_back('\'');
            fnName.push_back(')');
            throw RuntimeLibraryException(fnName.c_str(), errno);
        }
#endif
    }
};

using EntryPrivateSPtr = std::shared_ptr<EntryPrivate>;

namespace {

struct AscendingEntries
{
    bool operator()(const Entry &lhs, const Entry &rhs) const
    {
        return lhs.getName() < rhs.getName();
    }
};

struct DescendingEntries
{
    bool operator()(const Entry &lhs, const Entry &rhs) const
    {
        return rhs.getName() < lhs.getName();
    }
};

#ifdef _WIN32
//! @brief A structure which closes a POSIX directory.
struct DirCloser
{
    void operator()(HANDLE findHandle) const
    {
        if ((findHandle != nullptr) && (findHandle != INVALID_HANDLE_VALUE))
        {
            ::FindClose(findHandle);
        }
    }
};

using DirUPtr = std::unique_ptr<void, DirCloser>;
#else
//! @brief A structure which closes a POSIX directory.
struct DirCloser
{
    void operator()(DIR *dirInfo) const
    {
        if (dirInfo != nullptr)
        {
            closedir(dirInfo);
        }
    }
};

//! @brief An alias for a managed POSIX DIR pointer.
using DirUPtr = std::unique_ptr<DIR, DirCloser>;
#endif

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
//! @brief Filters entries produced by the Win32 FindFile() API;
//! @param[in] fileInfo The Win32 file metadata.
//! @param[in] flags The flags defined by the Directory::Query enumeration.
//! @retval true The entry should be included.
//! @retval false The entry should be excluded.
bool filterByQueryFlags(const WIN32_FIND_DATAW &fileInfo, uint32_t flags)
{
    static LPCWSTR CurrentDir = L".";
    static LPCWSTR ParentDir = L"..";

    bool isAllowed = true;

    if ((flags & Directory::IncludeVirtualEntries) == 0)
    {
        if (fileInfo.cFileName[0] == L'.')
        {
            if (fileInfo.cFileName[1] == L'.')
            {
                // If it's '..', that isn't allowed.
                isAllowed = (fileInfo.cFileName[2] != L'\0');
            }
            else if (fileInfo.cFileName[1] == L'\0')
            {
                // It's '.'.
                isAllowed = false;
            }
        }
    }

    if (isAllowed && ((flags & Directory::IncludeMask) != 0))
    {
        // Apply the type filter.
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            isAllowed = ((flags & Directory::IncludeDirectories) != 0);
        }
        else if ((fileInfo.dwFileAttributes & EntryPrivate::Win32NotFile) == 0)
        {
            isAllowed = ((flags & Directory::IncludeFiles) != 0);
        }
    }

    return isAllowed;
}
#else

//! @brief Attempts to decode the next Unicode character from a UTF-8 encoded
//! null-terminated string.
//! @param[in] str The array of bytes to decode.
//! @param[in,out] offeset The offset into str of the next byte to interpret,
//! updated to point to the next uninterpreted byte.
//! @param[out] next Receives the next Unicode character interpreted from the
//! string
//! @retval true A Unicode character was decoded.
//! @retval false There were no more characters or the encoding was invalid.
bool tryGetNextChar(const char *str, size_t &offset, char32_t &next)
{
    Utf::FromUtf8Converter converter;
    uint8_t nextByte = static_cast<uint8_t>(str[offset]);
    bool hasChar = false;
    bool hasError = false;
    next = U'\0';

    while (nextByte != 0)
    {
        ++offset;

        if (converter.tryConvert(nextByte, next, hasError))
        {
            hasChar = true;
            break;
        }
        else if (hasError)
        {
            // The encoding is invalid.
            converter.reset();
            next = U'\0';
            break;
        }
        else
        {
            // Get the next byte.
            nextByte = static_cast<uint8_t>(str[offset]);
        }
    }

    return hasChar;
}

//! @brief Filters the name of a POSIX directory entry.
//! @param[in] entry The entry to examine.
//! @param[in] pattern The wildcard pattern used to match the string.
//! The asterisk character '*' will match zero or more characters and the
//! question mark character '?' will match exactly one character.
//! @param[in] queryFlags A bit field defined using values from the
//! Fs::Directory::Query enumeration.
//! @retval true The entry passed the filter.
//! @retval false The entry did not fit the filter.
bool filterDirEntry(const dirent *entry, string_cref_t pattern, uint32_t queryFlags)
{
    if ((queryFlags & Directory::IncludeVirtualEntries) == 0)
    {
        // Reject virtual entries before attepting matching.
        if (entry->d_name[0] == '.')
        {
            if (entry->d_name[1] == '.')
            {
                if (entry->d_name[2] == '\0')
                {
                    // It's the parent directory virtual entry.
                    return false;
                }
            }
            else if (entry->d_name[1] == '\0')
            {
                // It's the current directory virtual entry.
                return false;
            }
        }
    }

    size_t offset = 0;
    bool isAllowed = true;

    for (auto patternPos = pattern.begin(), end = pattern.end();
         isAllowed && (patternPos != end); ++patternPos)
    {
        char32_t patternChar = *patternPos;
        char32_t source;

        if (patternChar == U'*')
        {
            // Match any character upto whatever is next in the pattern.
            ++patternPos;

            if (patternPos == end)
            {
                // There was nothing else, the string must match.
                break;
            }
            else
            {
                patternChar = *patternPos;
                bool hasMatch = false;

                while (tryGetNextChar(entry->d_name, offset, source))
                {
                    if (source == patternChar)
                    {
                        // We've found the first character the '*' doesn't match.
                        hasMatch = true;
                        break;
                    }
                }

                // Ensure we matched the character after before we ran out
                // of source string.
                isAllowed = hasMatch;
            }
        }
        else if (patternChar == U'?')
        {
            // Match any single character.
            isAllowed = tryGetNextChar(entry->d_name, offset, source);
        }
        else
        {
            // Exactly match patternChar.
            isAllowed = tryGetNextChar(entry->d_name, offset, source) &&
                        (source == patternChar);
        }
    }

    return isAllowed;
}

#endif

} // Anonymous namespace



////////////////////////////////////////////////////////////////////////////////
// Entry Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a file system entry from a path.
//! @param[in] path The path to the element to represent.
Entry::Entry(const Path &path)
{
    Path absLocation = path.convertToAbsolute();

    _entry = std::make_shared<EntryPrivate>(absLocation);
}

//! @brief Constructs a file system entry from a path string.
//! @param[in] path A string specifying the path to the element to represent.
Entry::Entry(string_cref_t path)
{
    Path absLocation = Path(path).convertToAbsolute();

    _entry = std::make_shared<EntryPrivate>(absLocation);
}

//! @brief Constructs a file system entry from a path to a parent folder
//! and the name of an element within it.
//! @param[in] parent The parent folder path.
//! @param[in] fileName The name of the element within the parent folder.
Entry::Entry(const Path &parent, string_cref_t fileName)
{
    Path absLocation = Path(parent, fileName).convertToAbsolute();

    _entry = std::make_shared<EntryPrivate>(absLocation);
}

//! @brief Constructs a file system entry an existing shared entry.
//! @param[in] status The entry to share.
Entry::Entry(std::shared_ptr<EntryPrivate> &status) :
    _entry(status)
{
}

//! @brief Determines if the object identified exists in the file system.
bool Entry::exists() const { return _entry && _entry->exists(); }

//! @brief Determines if the entry represent a file.
bool Entry::isFile() const { return _entry && _entry->isFile(); }

//! @brief Determines if the entry represents a directory.
bool Entry::isDirectory() const { return _entry && _entry->isDirectory(); }

//! @brief Gets the leaf name of the entry.
String Entry::getName() const
{
    return _entry ? _entry->getLocation().getFileName() : String::Empty;
}

//! @brief Gets the full path to the object the entry represents.
const Path &Entry::getPath() const
{
    static const Path empty;

    return _entry ? _entry->getLocation() : empty;
}

//! @brief Gets the length of a file in bytes or a negative value if the entry
//! does not represent a file.
int64_t Entry::getSize() const
{
    return _entry ? _entry->getSize() : 0ll;
}

//! @brief Updates the information about the object by re-querying the
//! file system.
void Entry::refresh()
{
    if (_entry)
    {
        _entry->refresh();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Directory Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing a directory.
//! @param[in] path The path to the directory to represent.
//! @throws ArgumentException If path is empty.
//! @throws InvalidFilePathException If path is invalid.
//! @throws OperationException If path points to a file.
Directory::Directory(const Path &path)
{
    if (path.isEmpty())
    {
        throw ArgumentException("path");
    }

    _dir = std::make_shared<EntryPrivate>(path.convertToAbsolute());

    if ((_dir->exists() == false) || (_dir->isDirectory() == false))
    {
        std::string message;
        appendFormat("The path '{0}' does not point to a directory.",
                     message, { _dir->getLocation().toString() });

        throw OperationException(std::string_view(message));
    }
}
//! @brief Constructs an object representing a directory.
//! @param[in] path The path string defining the directory to represent.
//! @throws ArgumentException If path is empty.
//! @throws InvalidFilePathException If path is invalid.
//! @throws OperationException If path points to a file.
Directory::Directory(string_cref_t path)
{
    if (path.isEmpty())
    {
        throw ArgumentException("path");
    }

    Path dirPath(path);
    _dir = std::make_shared<EntryPrivate>(dirPath.convertToAbsolute());

    if ((_dir->exists() == false) || (_dir->isDirectory() == false))
    {
        std::string message;
        appendFormat("The path '{0}' does not point to a directory.",
                     message, { _dir->getLocation().toString() });

        throw OperationException(std::string_view(message));
    }
}

//! @brief Constructs an object representing a directory.
//! @param[in] parentPath The parent directory of the directory to create.
//! @param[in] dirName The name of the directory within the parent.
//! @throws ArgumentException If parentPath or dirName are empty.
//! @throws InvalidFilePathException If the resultant path is invalid.
//! @throws OperationException If the resultant path points to a file.
Directory::Directory(const Path &parentPath, string_cref_t dirName)
{
    if (parentPath.isEmpty())
    {
        throw ArgumentException("parentPath");
    }
    else if (dirName.isEmpty())
    {
        throw ArgumentException("dirName");
    }

    Path dirPath(parentPath, dirName);

    _dir = std::make_shared<EntryPrivate>(dirPath.convertToAbsolute());

    if ((_dir->exists() == false) || (_dir->isDirectory() == false))
    {
        std::string message;
        appendFormat("The path '{0}' does not point to a directory.",
                     message, { _dir->getLocation().toString() });

        throw OperationException(std::string_view(message));
    }
}

//! @brief Constructs an object representing a directory.
//! @param[in] entry The object representing the file system object.
Directory::Directory(std::shared_ptr<EntryPrivate> &entry) :
    _dir(entry)
{
    if ((_dir->exists() == false) || (_dir->isDirectory() == false))
    {
        std::string message;
        appendFormat("The path '{0}' does not point to a directory.",
                     message, { _dir->getLocation().toString() });

        throw OperationException(std::string_view(message));
    }
}

//! @brief Determines whether the directory exists.
bool Directory::exists() const
{
    return _dir->exists();
}

//! @brief Gets the name of the directory with no proceeding path.
String Directory::getName() const
{
    return _dir->getLocation().getFileName();
}

//! @brief Gets the full path to the directory.
const Path &Directory::getPath() const
{
    return _dir->getLocation();
}

//! @brief Gets the names of files and directories in the directory the current
//! object represents.
StringCollection Directory::getNames() const
{
    return getNames("*", SortAscending | IncludeFiles | IncludeDirectories);
}

//! @brief Gets the names of files and directories in the directory the current
//! object represents.
//! @param[in] pattern A wildcard specification using '*' and '?' characters
//! used to match the names of elements to return.
//! @param[in] queryFlags Flags from the Query enumeration which determine
//! what to return.
StringCollection Directory::getNames(string_cref_t pattern, uint32_t queryFlags) const
{
    StringCollection childNames;

#ifdef _WIN32
    std::wstring widePath = _dir->getLocation().toWideString(PathUsage::Kernel);
    const wchar_t win32ElementSep = L'\\';

    if ((widePath.empty() == false) &&
        (widePath.back() != win32ElementSep))
    {
        widePath.push_back(win32ElementSep);
    }

    // Append the pattern.
    Utf::appendToWide(widePath, pattern.getUtf8Bytes(),
                      pattern.getUtf8Length(),
                      pattern.getWideLength());

    WIN32_FIND_DATAW fileInfo;

    HANDLE hFind = ::FindFirstFileExW(widePath.c_str(), FindExInfoStandard,
                                      &fileInfo, FindExSearchNameMatch,
                                      nullptr, 0);

    uint32_t errorCode = ::GetLastError();

    if (hFind == INVALID_HANDLE_VALUE)
    {
        if (errorCode != ERROR_FILE_NOT_FOUND)
        {
            String fn = String::format("FindFirstFileEx('{0}')",
                                       { String(widePath) });

            throw Win32Exception(fn.getUtf8Bytes(), errorCode);
        }
    }
    else
    {
        DirUPtr handleCloser(hFind);

        do
        {
            if (filterByQueryFlags(fileInfo, queryFlags))
            {
                childNames.emplace_back(fileInfo.cFileName);
            }
        } while (::FindNextFileW(hFind, &fileInfo) != FALSE);
    }
#else
    String fullPath = _dir->getLocation().toString(PathUsage::Kernel);
    unsigned char typeMatch = 0;

    if (queryFlags & Directory::IncludeFiles)
    {
        typeMatch |= DT_REG;
    }

    if (queryFlags & Directory::IncludeDirectories)
    {
        typeMatch |= DT_DIR;
    }

    // Create an auto-closing pointer to the open directory.
    DirUPtr dirInfo(opendir(fullPath.getUtf8Bytes()));

    if (dirInfo)
    {
        // Reset errno before call.
        errno = 0;
        struct dirent *entryInfo = readdir(dirInfo.get());

        while (entryInfo != nullptr)
        {
            if ((entryInfo->d_type & typeMatch) &&
                filterDirEntry(entryInfo, pattern, queryFlags))
            {
                childNames.emplace_back(entryInfo->d_name);
            }

            // Get the next entry.
            errno = 0;
            entryInfo = readdir(dirInfo.get());
        }

        if (errno != 0)
        {
            // An error occurred from calling readdir().
            std::string fnName;
            fnName.assign("readdir('");
            appendAgString(fnName, fullPath);
            fnName.push_back('\'');
            fnName.push_back(')');

            throw RuntimeLibraryException(fnName.c_str(), errno);
        }
    }
    else
    {
        // Throw exception based on errno.
        std::string fnName;
        fnName.assign("opendir('");
        appendAgString(fnName, fullPath);
        fnName.push_back('\'');
        fnName.push_back(')');

        throw RuntimeLibraryException(fnName.c_str(), errno);
    }
#endif

    if ((queryFlags & SortAscending) != 0)
    {
        std::sort(childNames.begin(), childNames.end());
    }
    else if ((queryFlags & SortDescending) != 0)
    {
        std::sort(childNames.begin(), childNames.end(),
                  DescendingStrings());
    }

    return childNames;
}

//! @brief Gets entries representing the elements in the directory the current
//! object represents.
EntryVector Directory::getEntries() const
{
    return getEntries("*", SortAscending | IncludeFiles | IncludeDirectories);
}

//! @brief Gets entries representing the elements in the directory the current
//! object represents.
EntryVector Directory::getEntries(string_cref_t pattern, uint32_t queryFlags) const
{
    EntryVector entries;
    const Path &parentPath = _dir->getLocation();

#ifdef _WIN32
    std::wstring widePath = parentPath.toWideString(PathUsage::Kernel);

    const wchar_t win32ElementSep = L'\\';

    if ((widePath.empty() == false) &&
        (widePath.back() != win32ElementSep))
    {
        widePath.push_back(win32ElementSep);
    }

    // Append the pattern.
    Utf::appendToWide(widePath, pattern.getUtf8Bytes(),
                      pattern.getUtf8Length(),
                      pattern.getWideLength());

    WIN32_FIND_DATAW fileInfo;

    HANDLE hFind = ::FindFirstFileExW(widePath.c_str(), FindExInfoStandard,
                                      &fileInfo, FindExSearchNameMatch,
                                      nullptr, 0);

    uint32_t errorCode = ::GetLastError();

    if (hFind == INVALID_HANDLE_VALUE)
    {
        if (errorCode != ERROR_FILE_NOT_FOUND)
        {
            String fn = String::format("FindFirstFileEx('{0}')",
                                       { String(widePath) });

            throw Win32Exception(fn.getUtf8Bytes(), errorCode);
        }
    }
    else
    {
        do
        {
            if (filterByQueryFlags(fileInfo, queryFlags))
            {
                auto entry = std::make_shared<EntryPrivate>(parentPath,
                                                            fileInfo);

                entries.emplace_back(entry);
            }
        } while (::FindNextFileW(hFind, &fileInfo) != FALSE);

        // Close the search handle.
        ::FindClose(hFind);
    }
#else
    String fullPath = parentPath.toString(PathUsage::Kernel);
    unsigned char typeMatch = 0;

    if (queryFlags & Directory::IncludeFiles)
    {
        typeMatch |= DT_REG;
    }

    if (queryFlags & Directory::IncludeDirectories)
    {
        typeMatch |= DT_DIR;
    }

    // Create an auto-closing pointer to the open directory.
    DirUPtr dirInfo(opendir(fullPath.getUtf8Bytes()));

    if (dirInfo)
    {
        // Reset errno before call.
        errno = 0;
        struct dirent *entryInfo = readdir(dirInfo.get());

        while (entryInfo != nullptr)
        {
            if ((entryInfo->d_type & typeMatch) &&
                filterDirEntry(entryInfo, pattern, queryFlags))
            {
                auto entry = std::make_shared<EntryPrivate>(parentPath,
                                                            entryInfo);

                entries.emplace_back(entry);
            }

            // Get the next entry.
            errno = 0;
            entryInfo = readdir(dirInfo.get());
        }

        if (errno != 0)
        {
            // An error occurred from calling readdir().
            std::string fnName;
            fnName.assign("readdir('");
            appendAgString(fnName, fullPath);
            fnName.push_back('\'');
            fnName.push_back(')');

            throw RuntimeLibraryException(fnName.c_str(), errno);
        }
    }
    else
    {
        // Throw exception based on errno.
        std::string fnName;
        fnName.assign("opendir('");
        appendAgString(fnName, fullPath);
        fnName.push_back('\'');
        fnName.push_back(')');

        throw RuntimeLibraryException(fnName.c_str(), errno);
    }
#endif

    if ((queryFlags & SortAscending) != 0)
    {
        std::sort(entries.begin(), entries.end(),
                  AscendingEntries());
    }
    else if ((queryFlags & SortDescending) != 0)
    {
        std::sort(entries.begin(), entries.end(),
                  DescendingEntries());
    }

    return entries;
}


const char *FileNotFoundException::Domain = "FileNotFoundException";

//! @brief An exception documenting a failed attempt to access a file.
//! @param[in] path The path to the file upon which access failed.
FileNotFoundException::FileNotFoundException(const Path &path)
{
    std::string buffer;
    buffer.assign("The file '");
    appendAgString(buffer, path.toString(PathUsage::Display));
    buffer.append("' could not be found.");

    initialise(Domain,
               "The application attempted to access a non-existent file.",
               buffer, 0);
}

}} // namespace Ag::Fs
////////////////////////////////////////////////////////////////////////////////


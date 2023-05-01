//! @file Ag/Core/FsPath.hpp
//! @brief The declaration of an object representing a file path.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_FS_PATH_HPP__
#define __AG_CORE_FS_PATH_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <string>

#include "Exception.hpp"
#include "String.hpp"

namespace Ag {
namespace Fs {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Expresses the type of root a file path has.
enum class PathRootType
{
    //! @brief The path has no root, it is relative.
    None,

    //! @brief The root refers to a DOS drive letter (Windows only).
    DosDrive,

    //! @brief The root refers to the DOS drive of the current working folder,
    //! i.e. '\' (Windows only).
    CurrentDrive,

    //! @brief The root refers to a UNC name specifying host and volume (Windows
    //! only).
    UncName,

    //! @brief The path is specified from the system root i.e. '/' (POSIX only).
    SysRoot,

    //! @brief The path is specified from the current users home directory, i.e.
    //! '~/' (POSIX only).
    UserHome,
};

//! @brief Expresses how a file path should be rendered as a string.
enum class PathUsage
{
    //! @brief The path string should be appropriate for display.
    //! @details
    //! On Windows, no special prefixes will be applied and no error given if
    //! the path is longer than 260 characters.
    //! On Linux, the ~/ root will be left in-tact.
    Display,

    //! @brief The path string should be appropriate for use with base system
    //! calls.
    //! @details
    //! On Windows this means using the Win32 file namespace prefix
    //! '\\?\' in order to increase the 260 character path length limit. Note
    //! this will only work if the path is passed to a wide character form of
    //! a Kernel32.dll function.
    //! On Linux this means that the ~/ prefix will be substituted with an
    //! appropriate home directory path or / if there isn't one.
    Kernel,

    //! @brief The path string should be appropriate for use with the
    //! shell.
    //! @details
    //! On Windows the path has no special prefixes and will produce an
    //! error if the length is greater than 260 characters.
    //! On Linux the ~/ root will be left in-tact.
    Shell,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class PathSchema;
using PathSchemaID = const PathSchema *;

//! @brief An exception thrown when an invalid file path was submitted for
//! processing.
class InvalidFilePathException : public Exception
{
public:
    InvalidFilePathException();
    InvalidFilePathException(string_cref_t path, string_cref_t reason);
};

//! @brief An exception thrown when an invalid file path element was submitted
//! for processing.
class InvalidPathElementException : public Exception
{
public:
    InvalidPathElementException();
    InvalidPathElementException(string_cref_t element, string_cref_t reason);
};

//! @brief An exception thrown when a file path is too long for the
//! system to handle.
class PathTooLongException : public Exception
{
public:
    PathTooLongException(string_cref_t filePath, size_t limit);
    PathTooLongException(const std::string &filePath, size_t limit);
    PathTooLongException(const std::wstring &filePath, size_t limit);
};

class Path;

//! @brief An object representing an editable identifier of a file system element.
class PathBuilder
{
public:
    // Construction/Destruction
    PathBuilder(PathSchemaID schema = nullptr);
    PathBuilder(string_cref_t filePath, PathSchemaID schema = nullptr);
    PathBuilder(const Path &filePath);

    // Accessors
    PathSchemaID getSchema() const;
    PathRootType getRootType() const;
    bool isEmpty() const;
    bool hasRoot() const;
    bool hasElements() const;
    size_t getElementCount();
    string_cref_t getElement(size_t index) const;
    string_cref_t getRoot() const;
    void setRoot(string_cref_t root);
    string_cref_t getFileName() const;
    void setFileName(string_cref_t fileName);
    String getDirectory() const;
    String toString(PathUsage usage = PathUsage::Display) const;
    std::wstring toWideString(PathUsage usage = PathUsage::Display) const;
    String getFileExtension() const;
    void setFileExtension(string_cref_t extension);

    // Operations
    void assignProgramFile();
    void assignProgramDirectory();
    void assignHomeDirectory();
    void assignWorkingDirectory();

    void makeCanonical();
    bool tryParse(string_cref_t filePath);
    bool tryParse(string_cref_t filePath, String &error);
    void convertToAbsolute();
    void convertToAbsolute(const PathBuilder &basePath);
    void makeRelative();
    void makeRelative(const PathBuilder &basePath);

    void pushElement(string_cref_t element);
    void popElement();

    PathBuilder &operator=(string_cref_t &rhs);
private:
    // Internal Functions
    bool isElementEqual(string_cref_t lhs, string_cref_t rhs) const;

    // Internal Fields
    PathSchemaID _schema;
    PathRootType _rootType;
    String _root;
    StringCollection _pathElements;
};

//! @brief An object representing an immutable identifier of a file system element.
class Path
{
public:
    // Construction/Destruction
    Path();
    Path(string_cref_t filePath, PathSchemaID schema = nullptr);
    Path(const PathBuilder &builder);
    Path(const Path &parent, string_cref_t fileName);

    // Accessors
    bool isEmpty() const;
    bool hasRoot() const;
    PathRootType getRootType() const;
    PathSchemaID getSchema() const;
    bool hasElements() const;
    String getRoot() const;
    String getDirectory() const;
    Path getDirectoryPath() const;
    String getFileName() const;
    String getFileBaseName() const;
    String getFileExtension() const;
    String getLastExtension() const;
    String toString(PathUsage usage = PathUsage::Display) const;
    std::wstring toWideString(PathUsage usage = PathUsage::Display) const;
    size_t getHashCode() const;

    // Operations
    static Path getProgramFile();
    static Path getProgramDirectory();
    static Path getHomeDirectory();
    static Path getWorkingDirectory();

    Path makeCanonical() const;
    Path append(string_cref_t fileName) const;
    static bool tryParse(string_cref_t filePath, Path &result,
                         PathSchemaID schema = nullptr);
    static bool tryParse(string_cref_t filePath, Path &result, String &error,
                         PathSchemaID schema = nullptr);
    Path convertToAbsolute() const;
    Path convertToAbsolute(const Path &basePath) const;
    Path convertToAbsolute(const PathBuilder &basePath) const;
    Path makeRelative() const;
    Path makeRelative(const Path
                      &basePath) const;
    Path makeRelative(const PathBuilder &basePath) const;

    Path &operator=(string_cref_t &rhs);
    Path &operator=(const PathBuilder &rhs);
    int compare(const Path &rhs) const;
    bool operator==(const Path &rhs) const;
    bool operator!=(const Path &rhs) const;
    bool operator<(const Path &rhs) const;
    bool operator<=(const Path &rhs) const;
    bool operator>(const Path &rhs) const;
    bool operator>=(const Path &rhs) const;
private:
    // Internal Functions
    bool innerParse(string_cref_t &path, string_ref_t error, PathSchemaID schema);
    void assignBuilder(const PathBuilder &builder);
    void refreshFilenameInfo();

    // Internal Fields
    PathSchemaID _schema;
    PathRootType _rootType;
    String _source;
    size_t _rootLength;
    size_t _fileNameLength;
    size_t _fileExtLength;
};


////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
PathSchemaID getNativeSchema();
PathSchemaID getWin32Schema();
PathSchemaID getPosixSchema();

}} // namespace Ag::Fs

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

namespace std
{
//! @brief A specialisation of the std::hash<> template which makes Ag::Fs::Path
//! values compatible with STL hash-based data structures.
template<> struct hash<Ag::Fs::Path>
{
    //! @brief Obtains the hash of the contents of a  file Path value.
    //! @param[in] rhs The Path value to get a hash code for.
    //! @return The hash code calculated for the path.
    size_t operator()(const Ag::Fs::Path &rhs) const
    {
        return rhs.getHashCode();
    }
};

////! @brief A specialisation of the std::less<> template which makes Ag::Fs::Path
////! values compatible with STL sorting and searching algorithms.
//template<> struct less<Ag::Fs::Path>
//{
//    //! @brief Compares the value of two Ag::Strings.
//    //! @param[in] lhs The left Path to compare.
//    //! @param[in] rhs The right Path to compare.
//    //! @retval true The lhs Path has a lower value than rhs.
//    //! @retval false The lhs Path has an equal or higher value than rhs.
//    bool operator()(const Ag::Fs::Path &lhs, const Ag::Fs::Path &rhs) const
//    {
//        return lhs.compare(rhs) < 0;
//    }
//};
}
#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

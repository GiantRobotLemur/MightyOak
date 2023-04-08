//! @file FsPath.cpp
//! @brief The definition of an object representing a file path.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>

#include <algorithm>
#include <iterator>
#include <memory>

#include "Ag/Core/CodePoint.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/FsPath.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Variant.hpp"
#include "FsPathSchema.hpp"
#include "Platform.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Fs {

////////////////////////////////////////////////////////////////////////////////
// InvalidFilePathException Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an exception describing an attempt to use an empty file path.
InvalidFilePathException::InvalidFilePathException()
{
    initialise("InvalidFilePathException",
               "A valid file path cannot be empty.",
               Utf::getEmpty());
}

//! @brief Constructs an exception describing an attempt to use a file path which
//! was in an invalid format.
//! @param[in] path The path string which was in error.
//! @param[in] reason Text detailing why the path string was invalid.
InvalidFilePathException::InvalidFilePathException(string_cref_t path,
                                                   string_cref_t reason)
{
    std::string detail;
    detail.assign("The file path '");
    detail.append(path.getUtf8Bytes(), path.getUtf8Length());

    if (reason.isEmpty())
    {
        detail.append("' is invalid.");
    }
    else
    {
        detail.append("' is invalid: ");
        detail.append(reason.getUtf8Bytes(), reason.getUtf8Length());
    }

    initialise("InvalidFilePathException",
               "A file path specified for processing was malformed.", detail);
}

////////////////////////////////////////////////////////////////////////////////
// InvalidFilePathException Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an exception describing attempted use of an empty
//! path element.
InvalidPathElementException::InvalidPathElementException()
{
    initialise("InvalidPathElementException",
               "A valid file path element cannot be empty.",
               Utf::getEmpty());
}

//! @brief Constructs an exception describing attempted use of an invalid
//! path element.
//! @param[in] element The text of the element which was mal-formed.
//! @param[in] reason The reason why the element was mal-formed.
InvalidPathElementException::InvalidPathElementException(string_cref_t element,
                                                         string_cref_t reason)
{
    std::string detail;
    detail.assign("The file path element '");
    detail.append(element.getUtf8Bytes(), element.getUtf8Length());

    if (reason.isEmpty())
    {
        detail.append("' is invalid.");
    }
    else
    {
        detail.append("' is invalid: ");
        detail.append(reason.getUtf8Bytes(), reason.getUtf8Length());
    }

    initialise("InvalidFilePathException",
               "A file path element specified for processing was malformed.",
               detail);
}

////////////////////////////////////////////////////////////////////////////////
// PathBuilder Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an exception describing the attempt to use a file path
//! which was too long for its intended usage.
//! @param[in] filePath The path which was to be used.
//! @param[in] limit The maximum number of characters allowed in the path.
PathTooLongException::PathTooLongException(string_cref_t filePath, size_t limit)
{
    String detail = String::format("The file path '{0}' was longer than {1} characters.",
                                   { filePath, limit });

    initialise("PathTooLongException",
               "A file path was specified which was too long for its intended usage.",
               detail.toUtf8View());
}

//! @brief Constructs an exception describing the attempt to use a file path
//! which was too long for its intended usage.
//! @param[in] filePath The path which was to be used.
//! @param[in] limit The maximum number of characters allowed in the path.
PathTooLongException::PathTooLongException(const std::string &filePath,
                                           size_t limit)
{
    String detail = String::format("The file path '{0}' was longer than {1} characters.",
                                   { String(filePath), limit });

    initialise("PathTooLongException",
               "A file path was specified which was too long for its intended usage.",
               detail.toUtf8View());
}

//! @brief Constructs an exception describing the attempt to use a file path
//! which was too long for its intended usage.
//! @param[in] filePath The path which was to be used.
//! @param[in] limit The maximum number of characters allowed in the path.
PathTooLongException::PathTooLongException(const std::wstring &filePath, size_t limit)
{
    String detail = String::format("The file path '{0}' was longer than {1} characters.",
                                   { String(filePath), limit });

    initialise("PathTooLongException",
               "A file path was specified which was too long for its intended usage.",
               detail.toUtf8View());
}

////////////////////////////////////////////////////////////////////////////////
// PathBuilder Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object in an empty state.
//! @param[in] schema The identifier of the schema used to express the path,
//! null to use the host native schema.
PathBuilder::PathBuilder(PathSchemaID schema /*= nullptr*/) :
    _schema((schema == nullptr) ? getNativeSchema() : schema),
    _rootType(PathRootType::None)
{
}

//! @brief Constructs an object to manipulate a file path specified as a string.
//! @param[in] filePath The path to manipulate expressed as a text string.
//! @param[in] schema The identifier of the schema used to express the path,
//! null to use the host native schema.
PathBuilder::PathBuilder(string_cref_t filePath,
                         PathSchemaID schema /*= nullptr*/) :
    _schema((schema == nullptr) ? getNativeSchema() : schema)
{
    String error;

    if (tryParse(filePath, error) == false)
    {
        throw InvalidFilePathException(filePath, error);
    }
}

//! @brief Constructs an editable path from an immutable path.
//! @param[in] filePath The immutable path to create an editable copy of.
PathBuilder::PathBuilder(const Path &filePath) :
    _schema(filePath.getSchema()),
    _rootType(PathRootType::None)
{
    // Get the un-altered path string.
    String fullPath = filePath.toString(PathUsage::Display);

    if (fullPath.isEmpty() == false)
    {
        _rootType = filePath.getRootType();
        _root = filePath.getRoot();

        std::u32string buffer;
        buffer.reserve(32);
        Utf::FromUtf8Converter converter;
        char32_t ch;
        bool hasError;
        const uint8_cptr_t path = reinterpret_cast<uint8_cptr_t>(fullPath.getUtf8Bytes());

        for (size_t offset = _root.getUtf8Length(), count = fullPath.getUtf8Length();
             offset <= count; ++offset)
        {
            bool flush = false;

            if (offset == count)
            {
                flush = true;
            }
            else if (converter.tryConvert(path[offset], ch, hasError))
            {
                if (_schema->isValidElementSeparator(ch))
                {
                    flush = true;
                }
                else
                {
                    buffer.push_back(ch);
                }
            }
            else
            {
                converter.reset();
            }

            if (flush && (buffer.empty() == false))
            {
                _pathElements.emplace_back(buffer);
            }
        }
    }
}

//! @brief Gets the schema used to express the path.
PathSchemaID PathBuilder::getSchema() const
{
    return _schema;
}

//! @brief Gets the type of root the path has, if any.
PathRootType PathBuilder::getRootType() const
{
    return _rootType;
}

//! @brief Determines if the path is in an empty state.
//! @retval true The path has no elements.
//! @retval false The path as at least a root or one path element.
bool PathBuilder::isEmpty() const
{
    return (_rootType == PathRootType::None) &&
        _pathElements.empty();
}

//! @brief Determines if the path has a root.
//! @retval true The path has a root and is therefore absolute, note however
//! that under Windows, it still may use the '\' root, indicating it is relative
//! to the current drive.
//! @retval false The path has no root, it is relative to the current working
//! directory.
bool PathBuilder::hasRoot() const { return _rootType != PathRootType::None; }

//! @brief Indicates whether the path has any directory elements.
//! @retval true The path has at least one directory element.
//! @retval false The path as no directory elements.
bool PathBuilder::hasElements() const { return _pathElements.empty() == false; }

//! @brief Gets the count of directory elements in the path.
size_t PathBuilder::getElementCount() { return _pathElements.size(); }

//! @brief Gets a directory element from the path.
//! @param[in] index The 0-based index of the element to get.
//! @return The specified path element as a string.
string_cref_t PathBuilder::getElement(size_t index) const
{
    if (index < _pathElements.size())
    {
        return _pathElements[index];
    }
    else
    {
        throw IndexOutOfRangeException(index, _pathElements.size());
    }
}

//! @brief Gets the root of the path not including any trailing directory
//! separator.
string_cref_t PathBuilder::getRoot() const
{
    return _root;
}

//! @brief Overwrites the current root of the path.
//! @param[in] root A valid root or an empty string to make the path relative.
//! @throws InvalidFilePathException If the non-empty root string was invalid.
void PathBuilder::setRoot(string_cref_t root)
{
    if (root.isEmpty())
    {
        _root = String::Empty;
        _rootType = PathRootType::None;
    }
    else
    {
        String error;
        PathRootType rootType;
        std::u32string newRoot;
        auto pos = root.begin();

        if (_schema->tryParsePathRoot(pos, root.end(), newRoot, error, rootType))
        {
            _root = newRoot;
            _rootType = rootType;
        }
        else
        {
            throw InvalidFilePathException(root, newRoot);
        }
    }
}

//! @brief Gets the last element in the path, empty if there are no path elements.
string_cref_t PathBuilder::getFileName() const
{
    return _pathElements.empty() ? String::Empty : _pathElements.back();
}

//! @brief Sets the last element in the path, appending it as a new one if there
//! are no existing path elements.
//! @param[in] fileName The new leaf path element.
//! @throws ArgumentException If fileName is empty.
//! @throws InvalidPathElementException If fileName contains invalid characters.
void PathBuilder::setFileName(string_cref_t fileName)
{
    if (fileName.isEmpty())
    {
        throw ArgumentException("A file name cannot be empty.");
    }

    for (char32_t ch : fileName)
    {
        if (_schema->isValidElementCharacter(ch) == false)
        {
            String message = String::format("The character '{0}' cannot appear in a file name.",
                                            { ch });

            throw InvalidPathElementException(fileName, message);
        }
    }

    if (_pathElements.empty())
    {
        _pathElements.push_back(fileName);
    }
    else
    {
        _pathElements.back() = fileName;
    }
}

//! @brief Gets the root and path elements apart from the last one as a string.
String PathBuilder::getDirectory() const
{
    // Guess how much memory to allocate for the resultant string.
    size_t expectedSize = 0;

    if (_rootType != PathRootType::None)
    {
        expectedSize += _root.getUtf8Length();
    }

    if (_pathElements.size() > 1)
    {
        size_t elementCount = _pathElements.size() - 1;

        for (size_t index = 0; index < elementCount; ++index)
        {
            expectedSize += _pathElements[index].getUtf8Length() + 1;
        }
    }

    // Create a string containing only the required elements.
    std::string builder;
    builder.reserve(expectedSize);

    if (_rootType != PathRootType::None)
    {
        builder.append(_root.getUtf8Bytes(), _root.getUtf8Length());
    }

    if (_pathElements.size() > 1)
    {
        size_t elementCount = _pathElements.size() - 1;
        const char sep = _schema->getElementSeparator();

        for (size_t index = 0; index < elementCount; ++index)
        {
            if (index > 0)
            {
                builder.push_back(sep);
            }

            string_cref_t element = _pathElements[index];

            builder.append(element.getUtf8Bytes(), element.getUtf8Length());
        }
    }

    // Create an immutable string from the builder buffer.
    return String(builder);
}

//! @brief Renders the current state of the object as a string.
//! @param[in] usage The usage to which the resultant path will be put.
//! @returns The path represented as a string.
String PathBuilder::toString(PathUsage usage /* = PathUsage::Display */) const
{
    std::string path = _schema->rootToString(_rootType, _root, usage);
    char sep = _schema->getElementSeparator();
    bool isFirst = true;

    for (const String &element : _pathElements)
    {
        if (isFirst)
        {
            // The root (if any) should have contained a trailing slash.
            isFirst = false;
        }
        else
        {
            path.push_back(sep);
        }

        path.append(element.getUtf8Bytes(), element.getUtf8Length());
    }

    if ((usage == PathUsage::Shell) && (path.length() > _MAX_PATH))
    {
        throw PathTooLongException(path, _MAX_PATH);
    }

    return String(path);
}

//! @brief Renders the current state of the object as a wide string.
//! @param[in] usage The usage to which the resultant path will be put.
//! @returns The path represented as a string.
std::wstring PathBuilder::toWideString(PathUsage usage /* = PathUsage::Display */) const
{
    std::wstring widePath = _schema->rootToWideString(_rootType, _root, usage);
    wchar_t sep = _schema->getWideElementSeparator();
    bool isFirst = true;

    for (const String &element : _pathElements)
    {
        if (isFirst)
        {
            // The root, if there was one, should have contained
            // a trailing slash.
            isFirst = false;
        }
        else
        {
            widePath.push_back(sep);
        }

        Utf::appendToWide(widePath, element.getUtf8Bytes(),
                          element.getUtf8Length(),
                          element.getUtf16Length());
    }

    if ((usage == PathUsage::Shell) && (widePath.length() > _MAX_PATH))
    {
        throw PathTooLongException(widePath, _MAX_PATH);
    }

    return widePath;
}

//! @brief Gets the characters after the last dot in the last path element.
//! @returns The file extension without any leading period or an empty string
//! if there is no file name or the last element has no extension.
String PathBuilder::getFileExtension() const
{
    String extension;

    if (_pathElements.empty() == false)
    {
        const String &fileName = _pathElements.back();

        auto dotPos = fileName.reverseFind(U'.');

        if (dotPos != fileName.end())
        {
            ++dotPos;

            extension = fileName.substring(dotPos, fileName.end());
        }
    }

    return extension;
}

//! @brief Changes or sets the file extension in the last element of the path.
//! @param[in] extension The new file extension, possibly empty, possibly with
//! leading period '.' characters.
void PathBuilder::setFileExtension(string_cref_t extension)
{
    if (_pathElements.empty())
    {
        throw OperationException("Cannot set the extension of a path which "
                                 "doesn't contain a file name.");
    }
    else
    {
        const String &fileName = _pathElements.back();

        std::u32string buffer, newExt;
        Utf::appendToUtf32(buffer, fileName.getUtf8Bytes(),
                           fileName.getUtf8Length(),
                           fileName.getUtf32Length());

        // Look for the first dot after the first character.
        size_t dotPos = buffer.find(U'.', 1);

        if (dotPos != std::u32string::npos)
        {
            // There was a previous extension, delete it and the dot.
            buffer.erase(buffer.begin() + dotPos, buffer.end());
        }

        if (extension.isEmpty() == false)
        {
            // Skip any leading dots in the new extension.
            for (auto pos = extension.begin(); pos != extension.end(); ++pos)
            {
                if (*pos != U'.')
                {
                    // We've found the first character of the new extension,
                    // copy the rest to buffer and terminate the loop.
                    buffer.push_back(U'.');
                    std::copy(pos, extension.end(), std::back_inserter(buffer));

                    break;
                }
            }
        }

        // Overwrite the last path element.
        _pathElements.back() = buffer;
    }
}


//! @brief Overwrites the object with the full path to the module which was
//! used to create the current process.
void PathBuilder::assignProgramFile()
{
    String modulePath = getProgramFileName();
    String error;
    _schema = getNativeSchema();

    if (tryParse(modulePath, error) == false)
    {
        throw InvalidFilePathException(modulePath, error);
    }
}

//! @brief Overwrites the object with the full path to the folder containing
//! the module which was used to create the current process.
void PathBuilder::assignProgramDirectory()
{
    String modulePath = getProgramFileName();
    String error;
    _schema = getNativeSchema();

    if (tryParse(modulePath, error))
    {
        if (_pathElements.empty() == false)
        {
            _pathElements.pop_back();
        }
    }
    else
    {
        throw InvalidFilePathException(modulePath, error);
    }
}

//! @brief Overwrites the object with the full path of the home directory of
//! the user running the process.
void PathBuilder::assignHomeDirectory()
{
    String homeDir = getHomeDirectory();
    String error;
    _schema = getNativeSchema();

    if (tryParse(homeDir, error) == false)
    {
        throw InvalidFilePathException(homeDir, error);
    }
}

//! @brief Overwrites the object with the fill path of the working directory
//! of the current process.
void PathBuilder::assignWorkingDirectory()
{
    String workingDir = getWorkingDirectory();
    String error;
    _schema = getNativeSchema();

    if (tryParse(workingDir, error) == false)
    {
        throw InvalidFilePathException(workingDir, error);
    }
}

//! @brief Removes relative path references such as '.' and '..' where possible.
void PathBuilder::makeCanonical()
{
    if ((_rootType != PathRootType::None) || (_pathElements.empty() != false))
    {
        // Only process the path if it isn't empty.
        bool removedDot = false;

        auto pos = _pathElements.begin();
        String dot(".");
        String dotDot("..");

        while (pos != _pathElements.end())
        {
            if (*pos == dot)
            {
                pos = _pathElements.erase(pos);
                removedDot = true;
            }
            else if (*pos == dotDot)
            {
                if (pos == _pathElements.begin())
                {
                    // We have no previous elements to remove.
                    ++pos;
                }
                else
                {
                    // See if the previous element can be removed.
                    auto prev = pos;
                    --prev;

                    if (*prev == dotDot)
                    {
                        // It can't.
                        ++pos;
                    }
                    else
                    {
                        // It can, remove both.
                        ++pos;
                        pos = _pathElements.erase(prev, pos);
                    }
                }
            }
        }

        if (_pathElements.empty() &&
            (_rootType == PathRootType::None) &&
            removedDot)
        {
            // We've made the path empty, add the reference to the current
            // folder back in.
            _pathElements.push_back(dot);
        }
    }
}

//! @brief Attempts to initialise the object by parsing a file path string.
//! @param[in] filePath The path string to parse.
//! @retval true The path was valid and the object has been initialised.
//! @retval false The path was invalid, the object is left in an empty state.
bool PathBuilder::tryParse(string_cref_t filePath)
{
    String errorToIgnore;

    return tryParse(filePath, errorToIgnore);
}

//! @brief Attempts to initialise the object by parsing a file path string.
//! @param[in] filePath The path string to parse.
//! @param[out] error Receives a message detailing why parsing failed, if it did.
//! @retval true The path was valid and the object has been initialised.
//! @retval false The path was invalid, error is updated with the reason why.
//! The object is left in an empty state.
bool PathBuilder::tryParse(string_cref_t filePath, String &error)
{
    std::u32string root;
    PathRootType rootType = PathRootType::None;
    bool isOK = false;
    auto pos = filePath.begin();

    if (pos == filePath.end())
    {
        // Empty paths are invalid.
        error = "A file path cannot be empty.";
    }
    else if (_schema->tryParsePathRoot(pos, filePath.end(), root, error, rootType) ||
             root.empty())
    {
        // Accumulate the path elements.
        StringCollection elements;
        std::u32string buffer;
        isOK = true;

        elements.reserve(16);
        buffer.reserve(256);

        bool isInPathElement = true;

        while (pos != filePath.end())
        {
            char32_t next = *pos;

            if (isInPathElement)
            {
                if (_schema->isValidElementCharacter(next))
                {
                    buffer.push_back(next);
                    ++pos;
                }
                else if (_schema->isValidElementSeparator(next))
                {
                    if (buffer.empty() == false)
                    {
                        elements.emplace_back(buffer);
                        buffer.clear();
                    }

                    isInPathElement = false;
                    ++pos;
                }
                else
                {
                    error = String::format("Unknown character '{0}' in path element.",
                                           { next });
                    pos = filePath.end();
                    isOK = false;
                }
            }
            else if (_schema->isValidElementCharacter(next))
            {
                buffer.push_back(next);
                isInPathElement = true;
                ++pos;
            }
            else if (_schema->isValidElementSeparator(next))
            {
                // Its a superfluous separator. Skip it.
                ++pos;
            }
            else if (_schema->isValidElementSeparator(next) == false)
            {
                error = String::format("Unknown character '{0}' between path elements.",
                                       { next });
                pos = filePath.end();
                isOK = false;
            }
        }

        if (isOK)
        {
            if (buffer.empty() == false)
            {
                // Store the last path element.
                elements.emplace_back(buffer);
            }

            // Update the object with the new path.
            _rootType = rootType;
            _root = root;
            _pathElements = std::move(elements);
        }
    }
    else
    {
        // The path root was invalid.
        error = root;
    }

    return isOK;
}

//! @brief Converts the object to an absolute path relative to the current
//! working directory or drive.
void PathBuilder::convertToAbsolute()
{
    if (_rootType == PathRootType::None)
    {
        PathBuilder workingFolder;
        workingFolder.assignWorkingDirectory();

        convertToAbsolute(workingFolder);
    }
}

//! @brief Converts the object to an absolute path relative to the current
//! working directory or drive.
//! @param[in] basePath An absolute path used to resolve the current one.
void PathBuilder::convertToAbsolute(const PathBuilder &basePath)
{
    if ((_rootType == PathRootType::None) ||
        (_rootType == PathRootType::CurrentDrive))
    {
        if (basePath._rootType == PathRootType::None)
        {
            throw OperationException("Cannot resolve a file path using a relative base path.");
        }

        // Take the root of the base path at the very least.
        _root = basePath._root;
        _rootType = basePath._rootType;

        if (_rootType == PathRootType::None)
        {
            // Concatenate the two sets of elements into the same collection.
            std::vector<String> elements;
            elements.reserve(basePath._pathElements.size() + _pathElements.size());

            std::copy(basePath._pathElements.begin(), basePath._pathElements.end(),
                      std::back_inserter(elements));
            std::copy(_pathElements.begin(), _pathElements.end(),
                      std::back_inserter(elements));

            // Overwrite the path elements with the combined collection.
            _pathElements = std::move(elements);
        }
    }
}

//! @brief Attempts to make the current path relative to the current working
//! directory.
//! @details
//! If the current path is already relative, nothing happens. If the current
//! path is absolute but doesn't share a root with the current working folder
//! path, it is left absolute.
void PathBuilder::makeRelative()
{
    if (_rootType != PathRootType::None)
    {
        PathBuilder workingFolder;
        workingFolder.assignWorkingDirectory();

        if ((workingFolder._rootType == _rootType) &&
            isElementEqual(workingFolder._root, _root))
        {
            makeRelative(workingFolder);
        }
    }
}

//! @brief Makes the current path relative to a base path if it isn't relative
//! already.
//! @param[in] basePath The absolute path used to make the current one relative.
//! @throws OperationException If basePath is relative or empty or does not share
//! the same root as the current path.
//! @details
//! This function will only work properly if basePath and the current path share
//! the same root and don't have any relative references ('.' or '..') in them. 
void PathBuilder::makeRelative(const PathBuilder &basePath)
{
    if (_rootType != PathRootType::None)
    {
        if (basePath._rootType == PathRootType::None)
        {
            throw OperationException("Cannot make a path relative to a relative path.");
        }
        else if ((basePath._rootType != _rootType) ||
                 (isElementEqual(basePath._root, _root) == false))
        {
            throw OperationException("One absolute path cannot be made relative "
                                     "to another if their roots differ.");
        }

        auto basePos = basePath._pathElements.begin();
        auto baseEnd = basePath._pathElements.end();
        auto thisPos = _pathElements.begin();
        auto thisEnd = _pathElements.end();

        while ((basePos != baseEnd) && (thisPos != thisEnd))
        {
            if (isElementEqual(*basePos, *thisPos))
            {
                ++basePos;
                ++thisPos;
            }
            else
            {
                break;
            }
        }

        // Move up a level for each path element which differs.
        std::vector<String> elements;
        String cdUp("..");

        while (basePos != baseEnd)
        {
            elements.push_back(cdUp);
        }

        // Copy the remainder of the current path.
        std::copy(thisPos, thisEnd, std::back_inserter(elements));

        // Update the current object.
        _root = String::Empty;
        _rootType = PathRootType::None;
        _pathElements = std::move(elements);
    }
}

//! @brief Adds an element to the end of the path.
//! @param[in] element The file path element to append.
//! @throw InvalidPathElementException If element is empty or contains invalid
//! characters.
void PathBuilder::pushElement(string_cref_t element)
{
    if (element.isEmpty())
    {
        throw InvalidPathElementException();
    }

    for (char32_t ch : element)
    {
        if (_schema->isValidElementCharacter(ch) == false)
        {
            String reason = String::format("The character '{0}' is not valid in a file path element.",
                                           { ch });

            throw InvalidPathElementException(element, reason);
        }
    }

    _pathElements.push_back(element);
}

//! @brief Removes the last element from the path.
//! @throws OperationException If there are no elements to pop.
void PathBuilder::popElement()
{
    if (_pathElements.empty())
    {
        throw OperationException("An element cannot be popped when there are none left in the file path.");
    }

    _pathElements.pop_back();
}

//! @brief Replaces the contents of the object with the result of parsing
//! a specified file path string.
//! @param[in] rhs The path string to parse.
//! @return A reference to the current object.
//! @throws InvalidFilePathException If rhs is not a valid file path.
PathBuilder &PathBuilder::operator=(string_cref_t &rhs)
{
    String error;

    if (tryParse(rhs, error) == false)
    {
        throw InvalidFilePathException(rhs, error);
    }

    return *this;
}

//! @brief Compares file path elements according to the current schema.
//! @param[in] lhs The first path element to compare.
//! @param[in] rhs The second path element to compare.
//! @retval true The two path elements refer to the same file system object.
//! @retval false The elements refer to different file system objects.
bool PathBuilder::isElementEqual(string_cref_t lhs, string_cref_t rhs) const
{
    return _schema->isCaseSensitive() ? (lhs == rhs) :
                                        (lhs.compareIgnoreCase(rhs) == 0);

}

////////////////////////////////////////////////////////////////////////////////
// Path Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a file path in an empty state.
Path::Path() :
    _schema(getNativeSchema()),
    _rootType(PathRootType::None),
    _rootLength(0),
    _fileNameLength(0),
    _fileExtLength(0)
{
}

//! @brief Constructs a file path initialised from a string.
//! @param[in] filePath The path string to parse.
//! @param[in] schema The identifier of the schema used to express the path,
//! null to use the host native schema.
//! @throw InvalidFilePathException If filePath is empty or invalid.
Path::Path(string_cref_t filePath, PathSchemaID schema /*= nullptr*/) :
    _schema((schema == nullptr) ? getNativeSchema() : schema),
    _rootType(PathRootType::None),
    _rootLength(0),
    _fileNameLength(0),
    _fileExtLength(0)
{
    String error;

    if (filePath.isEmpty())
    {
        // The path was empty.
        throw InvalidFilePathException();
    }
    else if (innerParse(filePath, error, _schema) == false)
    {
        throw InvalidFilePathException(filePath, error);
    }
}

//! @brief Constructs a file path from a path builder object.
//! @param[in] builder The builder object to extract the path from.
Path::Path(const PathBuilder &builder) :
    _schema(builder.getSchema()),
    _rootType(PathRootType::None),
    _rootLength(0),
    _fileNameLength(0),
    _fileExtLength(0)
{
    assignBuilder(builder);
}

//! @brief Creates a path where a file name is appended to an existing path.
//! @param[in] parent The path to expand.
//! @param[in] fileName The name of the file system element to add to the
//! end of the path.
Path::Path(const Path &parent, string_cref_t fileName) :
    _schema(parent._schema),
    _rootType(parent._rootType),
    _rootLength(parent._rootLength),
    _fileNameLength(fileName.getUtf8Length()),
    _fileExtLength(0)
{
    if (fileName.isEmpty())
    {
        throw InvalidPathElementException();
    }

    for (auto pos = fileName.begin(), end = fileName.end(); pos != end; ++pos)
    {
        char32_t ch = *pos;

        if (_schema->isValidElementCharacter(ch) == false)
        {
            String reason = String::format("The character '{0}' is invalid in a file name.",
                                           { ch });
            throw InvalidPathElementException(fileName, reason);
        }
    }

    if (_rootType == PathRootType::None)
    {
        _source = fileName;
    }
    else
    {
        std::string buffer;
        buffer.reserve(parent._source.getUtf8Length() + fileName.getUtf8Length() + 1);

        if (parent.hasRoot())
        {
            buffer.append(parent._source.getUtf8Bytes(),
                          parent._source.getUtf8Length());
        }

        if (parent.hasElements())
        {
            buffer.push_back(_schema->getElementSeparator());
        }

        buffer.append(fileName.getUtf8Bytes(),
                      fileName.getUtf8Length());

        _source = buffer;
    }

    auto dotPos = fileName.reverseFind(U'.');

    if (dotPos != fileName.end())
    {
        ++dotPos;

        _fileExtLength = fileName.getUtf8Length() - dotPos.getOffset();
    }
}

//! @brief Determines if the object is in an empty state.
bool Path::isEmpty() const
{
    return _source.isEmpty();
}

//! @brief Determines if the path is rooted.
bool Path::hasRoot() const
{
    return _rootLength > 0;
}

//! @brief Gets the kind of root the path has, if any.
PathRootType Path::getRootType() const { return _rootType; }

//! @brief Gets the schema used to interpret and express the path.
PathSchemaID Path::getSchema() const { return _schema; }

//! @brief Determines if the path as any elements beyond the root.
bool Path::hasElements() const
{
    return _source.getUtf8Length() > _rootLength;
}

//! @brief Gets the root portion of the path (if any), including the trailing slash.
String Path::getRoot() const
{
    if (_rootLength > 0)
    {
        if (_source.getUtf8Length() == _rootLength)
        {
            return _source;
        }
        else
        {
            return String(_source.getUtf8Bytes(), _rootLength);
        }
    }
    else
    {
        return String::Empty;
    }
}

//! @brief Gets the root and all path elements except the last one as a string.
String Path::getDirectory() const
{
    if (_source.isEmpty())
    {
        return String::Empty;
    }
    else if (_rootLength >= _source.getUtf8Length())
    {
        // There are no directory elements.
        return _source;
    }
    else
    {
        size_t totalLength = _source.getUtf8Length();
        size_t dirLength = totalLength - _rootLength;

        if (_fileNameLength < dirLength)
        {
            // There are multiple path elements, ensure the separator
            // before the last element is omitted.
            dirLength = totalLength - _fileNameLength - 1;
        }
        else
        {
            // There is only one path element, so just return the root.
            dirLength = _rootLength;
        }

        return String(_source.getUtf8Bytes(), dirLength);
    }
}

//! @brief Gets the root and non-leaf directory elements of the path as
//! a new path value.
//! @return An independent path value.
Path Path::getDirectoryPath() const
{
    Path result;

    if (_source.isEmpty() == false)
    {
        result._schema = _schema;
        result._rootType = _rootType;
        result._rootLength = _rootLength;

        // The path was not empty or just a root.
        size_t totalLength = _source.getUtf8Length();
        size_t dirLength = totalLength - _rootLength;

        if (_fileNameLength < dirLength)
        {
            // There is at least one directory element and a leaf element.
            // Omit the leaf element.
            dirLength = totalLength - _fileNameLength - 1;
            result._source = String(_source.getUtf8Bytes(), dirLength);

            result.refreshFilenameInfo();
        }
        else
        {
            // There is only the root left.
            result._source = String(_source.getUtf8Bytes(), _rootLength);
            result._fileNameLength = 0;
            result._fileExtLength = 0;
        }
    }

    return result;
}

//! @brief Returns the last element in the path, if there are any.
String Path::getFileName() const
{
    size_t totalLength = _source.getUtf8Length();

    if (totalLength > 0)
    {
        if (totalLength == _fileNameLength)
        {
            // The path only consists of a file name.
            return _source;
        }
        else
        {
            // Return a portion of the path string.
            size_t dirLength = totalLength - _fileNameLength;

            return String(_source.getUtf8Bytes() + dirLength,
                          _fileNameLength);
        }
    }
    else
    {
        // The path is empty.
        return String::Empty;
    }
}

String Path::getFileBaseName() const
{

    size_t totalLength = _source.getUtf8Length();

    if (totalLength > 0)
    {
        if (totalLength == _fileNameLength)
        {
            // The path only consists of a file name.
            return _source;
        }
        else
        {
            // Return a portion of the path string.
            size_t dirLength = totalLength - _fileNameLength;
            size_t extLength = (_fileExtLength == 0) ? 0 : _fileExtLength + 1;

            return String(_source.getUtf8Bytes() + dirLength,
                          _fileNameLength - extLength);
        }
    }
    else
    {
        // The path is empty.
        return String::Empty;
    }
}

//! @brief Returns the entire extension of the file identified by the path,
//! if there is one. If multiple extensions were specified, they will all be
//! included.
//! @returns The entire extension without a leading period '.'.
String Path::getFileExtension() const
{
    if (_fileExtLength > 0)
    {
        size_t prefixLength = _source.getUtf8Length() - _fileExtLength;

        return String(_source.getUtf8Bytes() + prefixLength,
                      _fileExtLength);
    }
    else
    {
        return String::Empty;
    }
}

//! @brief Returns the last extension of the file identified by the path,
//! if there are any, minus the leading period '.'.
String Path::getLastExtension() const
{
    if (_fileExtLength > 0)
    {
        size_t prefixLength = _source.getUtf8Length() - _fileExtLength;
        auto pos = _source.reverseFind(U'.');

        if ((pos != _source.end()) && (pos.getOffset() >= prefixLength))
        {
            // Move past the period.
            ++pos;

            return String(_source.getUtf8Bytes() + pos.getOffset());
        }
    }

    return String::Empty;
}

//! @brief Returns a string representation of the path based on the intended usage.
//! @param[in] usage The use to which the path string will be put.
//! @returns The path rendered as a single string, possibly adapted for use.
//! @throws PathTooLongException If the path is too long for the intended usage.
String Path::toString(PathUsage usage /* = PathUsage::Display */) const
{
    return _schema->pathToString(usage, _rootType, _rootLength, _source);
}

//! @brief Returns a wide string representation of the path based on the
//! intended usage.
//! @param[in] usage The use to whic the path string will be put.
//! @returns The path rendered as a single wide string, possibly adapted for use.
//! @throws PathTooLongException If the path is too long for the intended usage.
std::wstring Path::toWideString(PathUsage usage /* = PathUsage::Display */) const
{
    return _schema->pathToWideString(usage, _rootType, _rootLength, _source);
}

//! @brief Gets the hash code calculated from the underlying representation of the path.
size_t Path::getHashCode() const
{
    return _source.getHashCode();
}

//! @brief Gets a path representing the full path to the program binary.
Path Path::getProgramFile()
{
#ifdef _WIN32
    String fileName = getModuleFileName(reinterpret_cast<uintptr_t>(::GetModuleHandleW(nullptr)));
#else
    String fileName = Ag::getProgramFileName();
#endif

    return Path(fileName);
}

//! @brief Gets a path representing the full path to the directory containing
//! the program binary.
Path Path::getProgramDirectory()
{
#ifdef _WIN32
    String directory = getModuleDirectory(reinterpret_cast<uintptr_t>(::GetModuleHandleW(nullptr)));
#else
    String directory = Ag::getProgramDirectory();
#endif

    return Path(directory);
}

//! @brief Gets a path representing the full path to the current user's home
//! directory.
Path Path::getHomeDirectory()
{
    return Path(Ag::getHomeDirectory());
}

//! @brief Gets the full path of the current working directory of the
//! current process.
Path Path::getWorkingDirectory()
{
    return Path(Ag::getWorkingDirectory());
}

//! @brief Removes all relative references within the path.
Path Path::makeCanonical() const
{
    auto pos = _source.find(U'.');

    if (pos == _source.end())
    {
        return *this;
    }
    else
    {
        PathBuilder builder(*this);

        builder.makeCanonical();

        return Path(builder);
    }
}

//! @brief Creates a path which is a copy of the current one with an
//! additional element added to the end.
//! @param[in] fileName The name of the file to append to the path in order to
//! create a new one.
//! @returns A new path with fileName appended to the end.
Path Path::append(string_cref_t fileName) const
{
    return Path(*this, fileName);
}

//! @brief Attempts to create a Path object from a string representation.
//! @param[in] filePath The path string to parse.
//! @param[out] result Receives the parsed path value.
//! @param[in] schema The identifier of the schema used to express the path,
//! null to use the host native schema.
//! @retval true The string was a valid file path and result has been updated.
//! @retval false The path string was empty or invalid.
bool Path::tryParse(string_cref_t filePath, Path &result,
                    PathSchemaID schema /*= nullptr*/)
{
    bool hasPath = false;

    if (filePath.isEmpty() == false)
    {
        String error;

        hasPath = result.innerParse(filePath, error,
                                    (schema == nullptr) ? getNativeSchema() : schema);
    }

    return hasPath;
}

//! @brief Attempts to create a Path object from a string representation.
//! @param[in] filePath The path string to parse.
//! @param[out] result Receives the parsed path value.
//! @param[out] error Receives a message detailing why filePath was invalid.
//! @param[in] schema The identifier of the schema used to express the path,
//! null to use the host native schema.
//! @retval true The string was a valid file path and result has been updated.
//! @retval false The path string was empty or invalid, error details
//! which and why.
bool Path::tryParse(string_cref_t filePath, Path &result, String &error,
                    PathSchemaID schema /*= nullptr*/)
{
    return result.innerParse(filePath, error,
                             (schema == nullptr) ? getNativeSchema() : schema);
}

//! @brief Creates an absolute path resolved using the current working folder.
Path Path::convertToAbsolute() const
{
    if (_rootType == PathRootType::None)
    {
        String workingFolder = Ag::getWorkingDirectory();

        if (_source.isEmpty())
        {
            return Path(workingFolder);
        }
        else
        {
            std::string buffer;
            buffer.reserve(workingFolder.getUtf8Length() +
                           _source.getUtf8Length() + 1);

            buffer.append(workingFolder.getUtf8Bytes(),
                          workingFolder.getUtf8Length());

            // It doesn't matter if it's a duplicate, the constructor of Path
            // will sort it all out.
            buffer.push_back(_schema->getElementSeparator());

            buffer.append(_source.getUtf8Bytes(),
                          _source.getUtf8Length());

            String newPath(buffer);

            return Path(newPath);
        }
    }
    else if (_rootType == PathRootType::CurrentDrive)
    {
        PathBuilder workingDir;
        workingDir.assignWorkingDirectory();
        const String &workingDrive = workingDir.getRoot();

        std::string buffer;
        buffer.reserve(workingDrive.getUtf8Length() +
                       _source.getUtf8Length());

        buffer.append(workingDrive.getUtf8Bytes(),
                      workingDrive.getUtf8Length());

        // Don't worry about doubling up the first slash, it will come
        // out in construction of the new path.
        buffer.append(_source.getUtf8Bytes(),
                      _source.getUtf8Length());

        String newPath(buffer);

        return Path(newPath);
    }
    else
    {
        return *this;
    }
}

//! @brief Creates an absolute path resolved using a specified folder.
//! @param[in] basePath The folder to use if the current path does not have
//! a root.
//! @return An absolute path.
Path Path::convertToAbsolute(const Path &basePath) const
{
    if (_rootType == PathRootType::None)
    {
        if (basePath.hasRoot() == false)
        {
            throw OperationException("Cannot resolve a relative path "
                                     "using another relative path.");
        }
        else if (_source.isEmpty())
        {
            return basePath;
        }
        else
        {
            String basePathStr = basePath.toString(PathUsage::Display);

            std::string buffer;

            buffer.reserve(basePathStr.getUtf8Length() + _source.getUtf8Length() + 1);

            buffer.append(basePathStr.getUtf8Bytes(),
                          basePathStr.getUtf8Length());

            // It doesn't matter if it's a duplicate, the constructor of Path
            // will sort it all out.
            buffer.push_back(_schema->getElementSeparator());

            buffer.append(_source.getUtf8Bytes(),
                          _source.getUtf8Length());

            String newPath(buffer);

            return Path(newPath);
        }
    }
    else if (_rootType == PathRootType::CurrentDrive)
    {
        if (basePath.getRootType() != PathRootType::DosDrive)
        {
            throw OperationException("Cannot resolve a path relative to the "
                                     "current drive using a path with no "
                                     "drive specification.");
        }

        PathBuilder baseDir(basePath);
        const String &baseDrive = baseDir.getRoot();

        std::string buffer;
        buffer.reserve(baseDrive.getUtf8Length() +
                       _source.getUtf8Length());

        buffer.append(baseDrive.getUtf8Bytes(),
                      baseDrive.getUtf8Length());

        // Don't worry about doubling up the first slash, it will come
        // out in construction of the new path.
        buffer.append(_source.getUtf8Bytes(),
                      _source.getUtf8Length());

        String newPath(buffer);

        return Path(newPath);
    }
    else
    {
        return *this;
    }
}

//! @brief Creates an absolute path resolved using a specified folder.
//! @param[in] basePath The folder to use if the current path does not have
//! a root.
//! @return An absolute path.
Path Path::convertToAbsolute(const PathBuilder &basePath) const
{
    if (_rootType == PathRootType::None)
    {
        if (basePath.hasRoot() == false)
        {
            throw OperationException("Cannot resolve a relative path "
                                     "using another relative path.");
        }
        else if (_source.isEmpty())
        {
            return basePath;
        }
        else
        {
            String basePathStr = basePath.toString(PathUsage::Display);

            std::string buffer;

            buffer.reserve(basePathStr.getUtf8Length() +
                           _source.getUtf8Length() + 1);

            buffer.append(basePathStr.getUtf8Bytes(),
                          basePathStr.getUtf8Length());

            // It doesn't matter if it's a duplicate, the constructor of Path
            // will sort it all out.
            buffer.push_back(_schema->getElementSeparator());

            buffer.append(_source.getUtf8Bytes(),
                          _source.getUtf8Length());

            String newPath(buffer);

            return Path(newPath);
        }
    }
    else if (_rootType == PathRootType::CurrentDrive)
    {
        if (basePath.getRootType() != PathRootType::DosDrive)
        {
            throw OperationException("Cannot resolve a path relative to the "
                                     "current drive using a path with no "
                                     "drive specification.");
        }

        PathBuilder baseDir(basePath);
        const String &baseDrive = baseDir.getRoot();

        std::string buffer;
        buffer.reserve(baseDrive.getUtf8Length() +
                       _source.getUtf8Length());

        buffer.append(baseDrive.getUtf8Bytes(),
                      baseDrive.getUtf8Length());

        // Don't worry about doubling up the first slash, it will come
        // out in construction of the new path.
        buffer.append(_source.getUtf8Bytes(),
                      _source.getUtf8Length());

        String newPath(buffer);

        return Path(newPath);
    }
    else
    {
        return *this;
    }
}

//! @brief Creates a version of the current path which is expressed relative
//! to the current working directory.
Path Path::makeRelative() const
{
    if ((_rootType == PathRootType::None) ||
        (_rootType == PathRootType::CurrentDrive))
    {
        // The path is already relative.
        return *this;
    }
    else
    {
        PathBuilder basePath;
        basePath.assignWorkingDirectory();

        return makeRelative(basePath);
    }
}

//! @brief Creates a version of the current path which is expressed relative
//! to a specifid directory.
//! @param[in] basePath The absolute path to make the current path relative to.
Path Path::makeRelative(const Path &basePath) const
{
    if ((_rootType == PathRootType::None) ||
        (_rootType == PathRootType::CurrentDrive))
    {
        // The path is already relative.
        return *this;
    }
    else
    {
        PathBuilder baseBuilder(basePath);
        PathBuilder current(*this);

        current.makeRelative(baseBuilder);

        return Path(current);
    }
}

//! @brief Creates a version of the current path which is expressed relative
//! to a specifid directory.
//! @param[in] basePath The absolute path to make the current path relative to.
Path Path::makeRelative(const PathBuilder &basePath) const
{
    if ((_rootType == PathRootType::None) ||
        (_rootType == PathRootType::CurrentDrive))
    {
        // The path is already relative.
        return *this;
    }
    else
    {
        PathBuilder current(*this);

        current.makeRelative(basePath);

        return Path(current);
    }
}

//! @brief Assigns the object from a path string.
//! @param[in] rhs The path string to parse and assign.
//! @returns A reference to the current object.
//! @throws InvalidFilePathException If rhs is not a valid path.
Path &Path::operator=(string_cref_t &rhs)
{
    String error;

    if (innerParse(rhs, error, getNativeSchema()) == false)
    {
        throw InvalidFilePathException(rhs, error);
    }

    return *this;
}

//! @brief Overwrites the value of the path from a path builder object.
//! @param[in] rhs The builder object to make a copy of.
//! @return A reference to the current object.
Path &Path::operator=(const PathBuilder &rhs)
{
    assignBuilder(rhs);

    return *this;
}

//! @brief Compares a path against the current one.
//! @param[in] rhs The path to compare with the current one.
//! @retval &lt;0 The current path has a lower value than rhs.
//! @retval 0 The current path has a value equal to rhs.
//! @retval &gt;0 The current path has a higher value than rhs.
//! @note Paths will only appear to point to the same file if they do
//! so in the same way, i.e. canonical form is the only way to guarantee that
//! two paths identify the same file system object.
int Path::compare(const Path &rhs) const
{
    int diff = 0;

    if (_rootType == rhs._rootType)
    {
        if (_schema->isCaseSensitive())
        {
            diff = _source.compare(rhs._source);
        }
        else
        {
            diff = _source.compareIgnoreCase(rhs._source);
        }
    }
    else
    {
        diff = (_rootType < rhs._rootType) ? -1 : 1;
    }

    return diff;
}

//! @brief Determines if two paths have the same value.
//! @param[in] rhs The path to compare with the current one.
//! @retval true The paths have the same values.
//! @retval false The paths have differing values.
bool Path::operator==(const Path &rhs) const
{
    return (_rootType == rhs._rootType) &&
        (_source == rhs._source);
}

//! @brief Determines if two paths have differing values.
//! @param[in] rhs The path to compare with the current one.
//! @retval true The paths have differing values.
//! @retval false The paths have the same values.
bool Path::operator!=(const Path &rhs) const
{
    return (_rootType != rhs._rootType) ||
        (_source != rhs._source);
}

//! @brief Determines if the current path has a lower value than another.
//! @param[in] rhs The path to compare with the current one.
//! @retval true The current path has a lower value than rhs.
//! @retval false The current path has an equal or higher value than rhs.
bool Path::operator<(const Path &rhs) const
{
    return compare(rhs) < 0;
}

//! @brief Determines if the current path has an equal or lower value
//! than another.
//! @param[in] rhs The path to compare with the current one.
//! @retval true The current path has an equal or lower value than rhs.
//! @retval false The current path has a higher value than rhs.
bool Path::operator<=(const Path &rhs) const
{
    return compare(rhs) <= 0;
}

//! @brief Determines if the current path has a higher value than another.
//! @param[in] rhs The path to compare with the current one.
//! @retval true The current path has a higher value than rhs.
//! @retval false The current path has an equal or lower value than rhs.
bool Path::operator>(const Path &rhs) const
{
    return compare(rhs) > 0;
}

//! @brief Determines if the current path has an equal or higher value
//! than another.
//! @param[in] rhs The path to compare with the current one.
//! @retval true The current path has an equal or higher value than rhs.
//! @retval false The current path has a lower value than rhs.
bool Path::operator>=(const Path &rhs) const
{
    return compare(rhs) >= 0;
}

//! @brief Performs a common parsing process of the file path and overwrites
//! the value of the current object with the result.
//! @param[in] filePath The path string to parse.
//! @param[out] error Receives an error message detailing why the parsing
//! process failed.
//! @param[out] error Receives a message detailing why filePath was invalid.
//! @param[in] schema The identifier of the schema used to express the path,
//! null to use the host native schema.
//! @retval true The string was a valid file path and result has been updated.
//! @retval false The path string was empty or invalid, error details
//! which and why.
bool Path::innerParse(string_cref_t &filePath, string_ref_t error, PathSchemaID schema)
{
    if (filePath.isEmpty())
    {
        error = "A file path cannot be empty.";
        return false;
    }

    std::u32string path;
    PathRootType rootType = PathRootType::None;
    auto pos = filePath.begin();
    auto end = filePath.end();
    bool hasPath = false;
    size_t rootLength = 0;

    if (schema->tryParsePathRoot(pos, end, path, error, rootType) ||
        path.empty())
    {
        hasPath = true;
        rootLength = Utf::calculateConvertedLength(path, Utf::Utf8);

        std::u32string element;
        bool isInSeparator = false;
        element.reserve(32);

        while (pos != end)
        {
            char32_t ch = *pos;
            ++pos;

            if (isInSeparator)
            {
                if (schema->isValidElementCharacter(ch))
                {
                    if (element.empty() == false)
                    {
                        // There have been previous elements, so we need
                        // a separator in the path before we (eventually)
                        // append the new element.
                        path.push_back(schema->getWideElementSeparator());
                        element.clear();
                    }

                    element.push_back(ch);
                    isInSeparator = false;
                }
                else if (schema->isValidElementSeparator(ch) == false)
                {
                    error = String::format("Unknown character '{0}' in file "
                                           "path element.", { ch });
                    pos = end;
                    hasPath = false;
                }
            }
            else if (schema->isValidElementCharacter(ch))
            {
                element.push_back(ch);
            }
            else if (schema->isValidElementSeparator(ch))
            {
                if (element.empty() == false)
                {
                    path.append(element);
                    isInSeparator = true;
                }
            }
            else
            {
                error = String::format("Unknown character '{0}' in file "
                                       "path element.", { ch });
                pos = end;
                hasPath = false;
            }
        }

        if ((element.empty() == false) && (isInSeparator == false))
        {
            // Append the last element.
            path.append(element);
        }
    }

    if (hasPath)
    {
        _schema = schema;
        _source = path;
        _rootType = rootType;
        _rootLength = rootLength;

        // Update file name statistics.
        refreshFilenameInfo();
    }

    return hasPath;
}

//! @brief Overwrites the value of the object from a path builder.
//! @param[in] builder The builder object to make a copy of.
void Path::assignBuilder(const PathBuilder &builder)
{
    if (builder.isEmpty())
    {
        _source = String::Empty;
        _rootType = PathRootType::None;
        _rootLength = 0;
        _fileNameLength = 0;
        _fileExtLength = 0;
    }
    else
    {
        _source = builder.toString(PathUsage::Display);
        _rootType = builder.getRootType();
        _rootLength = builder.getRoot().getUtf8Length();

        if (builder.hasElements())
        {
            string_cref_t filename = builder.getFileName();
            _fileNameLength = filename.getUtf8Length();

            auto dotPos = filename.reverseFind(U'.');

            if (dotPos != filename.end())
            {
                ++dotPos;
                _fileExtLength = filename.getUtf8Length() - dotPos.getOffset();
            }
        }
    }
}

//! @brief Updates the _fileNameLength and _fileExtLength based on the current
//! source path text.
void Path::refreshFilenameInfo()
{
    size_t nonRootLength = _source.getUtf8Length() - _rootLength;
    _fileNameLength = 0;
    _fileExtLength = 0;

    if (nonRootLength > 0)
    {
        auto pos = _source.reverseFind(_schema->getElementSeparator());
        size_t fileNameOffset = _rootLength;

        if (pos == _source.end())
        {
            _fileNameLength = nonRootLength;
        }
        else
        {
            // Move past the last path element separator.
            ++pos;
            fileNameOffset = pos.getOffset();
            _fileNameLength = _source.getUtf8Length() - fileNameOffset;
        }

        if (_fileNameLength > 0)
        {
            // Create a view of the filename excluding the first character so
            // that we don't count a leading period as defining an extension.
            std::string_view name(_source.getUtf8Bytes() + fileNameOffset + 1);
            auto dotPos = name.find('.');

            if (dotPos != std::string_view::npos)
            {
                // Move past the dot.
                ++dotPos;
                _fileExtLength = name.length() - dotPos;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Fs
////////////////////////////////////////////////////////////////////////////////


//! @file Utils.cpp
//! @brief The definition of stand-alone helper functions for use by the
//! symbol packager tool.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <climits>
#include <cstring>
#include <string>
#include <vector>

#include "Utils.hpp"

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
bool isPathRooted(const std::string &path)
{
#ifdef _WIN32
    if (path.length() > 2)
    {
        char first = path.at(0);
        char second = path.at(1);

        if ((second == ':') &&
            (((first >= 'A') && (first <= 'Z')) ||
             ((first >= 'a') && (first <= 'z'))))
        {
            // It's possibly a DOS drive specification.
            char third = path.at(2);
            return (third == '\\') || (third == '/');
        }
        else if ((first == '\\') && (second == '\\'))
        {
            // It's a UNC name.
            return true;
        }
    }
#else
    if (path.empty() == false)
    {
        char first = path.at(0);

        if (first == '/')
        {
            return true;
        }
        else
        {
            return (first == '~') && (path.length() > 1) && (path.at(1) == '/');
        }
    }
#endif
    return false;
}

void appendPathElements(std::string &target, const std::string &elements)
{
    if (target.empty())
    {
        target.assign(elements);
    }
    else
    {
        size_t offset;

#ifdef _WIN32
        const char separator = '\\';
        offset = target.find_last_not_of("\\/");
#else
        const char separator = '/';
        offset = target.find_last_not_of(separator);
#endif

        if (offset != std::string::npos)
        {
            ++offset;
            target.erase(offset, target.length() - offset);
        }

        target.push_back(separator);
        target.append(elements);
    }
}

//! @brief Calculates a simple hash code for a bounded string.
//! @param[in] data The array of characters to hash.
//! @param[in] length The count of characters to hash.
//! @return A primitive hash code calculated from data and length.
size_t calculateHashCode(const char *data, size_t length)
{
    size_t hashCode = 0;
    const int shift = 7;

    for (size_t index = 0; index < length; ++index)
    {
        hashCode = (hashCode << 7) | (hashCode >> ((sizeof(size_t) * 8) - shift));
        hashCode ^= data[index];
    }

    return hashCode;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing a bounded string from a
//! null-terminated character array.
//! @param[in] text A null-terminated text string.
BoundedString::BoundedString(const char *text) :
    Text(text),
    Length(0),
    HashCode(0)
{
    if (Text != nullptr)
    {
        Length = std::strlen(Text);
        HashCode = calculateHashCode(Text, Length);
    }
}

//! @brief Constructs an object from a bounded array of characters.
//! @param[in] text The array of characters defining the string.
//! @param[in] length The count of characters in the array, not including
//! any termination characters.
BoundedString::BoundedString(const char *text, size_t length) :
    Text(text),
    Length(length),
    HashCode(calculateHashCode(text, length))
{
}

//! @brief Constructs an object from an STL string.
//! @param[in] text The STL string to reference.
BoundedString::BoundedString(const std::string &text) :
    Text(text.c_str()),
    Length(text.length()),
    HashCode(calculateHashCode(text.c_str(), text.length()))
{
}

//! @brief Determines if another bounded string has the same value.
//! @param[in] rhs The string to compare against.
//! @retval true The strings have the same values.
//! @retval false The strings are different.
bool BoundedString::operator==(const BoundedString &rhs) const
{
    bool isEqual = false;

    if (Text == rhs.Text)
    {
        isEqual = (Length == rhs.Length);
    }
    else if ((Length == rhs.Length) && (HashCode == rhs.HashCode))
    {
        isEqual = (std::memcmp(Text, rhs.Text, Length) == 0);
    }

    return isEqual;
}

//! @brief Determines if another bounded string has a different value.
//! @param[in] rhs The string to compare against.
//! @retval true The strings are different.
//! @retval false The strings have the same values.
bool BoundedString::operator!=(const BoundedString &rhs) const
{
    bool isDifferent = true;

    if (Text == rhs.Text)
    {
        isDifferent = (Length != rhs.Length);
    }
    else if ((Length == rhs.Length) && (HashCode == rhs.HashCode))
    {
        isDifferent = (std::memcmp(Text, rhs.Text, Length) != 0);
    }

    return isDifferent;
}

//! @brief Determines if another bounded string has the same value ignoring
//! differences in case.
//! @param[in] rhs The string to compare against.
//! @retval true The strings have the same values ignoring differences between
//! upper and lower case.
//! @retval false The strings are different.
bool BoundedString::isEqualIgnoreCase(const BoundedString &rhs) const
{
    bool isEqual = false;

    if (Text == rhs.Text)
    {
        isEqual = (Length == rhs.Length);
    }
    else if (Length == rhs.Length)
    {
#ifdef _MSC_VER
        isEqual = (_strnicmp(Text, rhs.Text, Length) == 0);
#else
        isEqual = (strncasecmp(Text, rhs.Text, Length) == 0);
#endif
    }

    return isEqual;
}

//! @brief Determines if the string has a specified prefix.
//! @param[in] rhs The prefix to check for.
//! @retval true The current string has rhs as a prefix.
//! @retval false The current string is not prefixed with rhs.
bool BoundedString::startsWith(const BoundedString &rhs) const
{
    bool hasPrefix = false;

    if (rhs.Length <= Length)
    {
        hasPrefix = std::memcmp(Text, rhs.Text, rhs.Length) == 0;
    }

    return hasPrefix;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to open a file.
//! @param[in] fileName The name of the file to open.
//! @param[in] mode The desired access to the file formatted in a manner
//! compatible with fopen().
//! @param[out] file Receives the file handle if the file was successfully open.
bool tryOpenFile(const char *fileName, const char *mode, StdFilePtr &file)
{
    FILE *fp = nullptr;
    bool isOK = false;

#ifdef _MSC_VER
    auto errorCode = fopen_s(&fp, fileName, mode);

    if (errorCode == 0)
    {
        file.reset(fp);
        isOK = true;
    }
#else
    fp = fopen(fileName, mode);

    if (fp != nullptr)
    {
        file.reset(fp);
        isOK = true;
    }
#endif

    return isOK;
}

//! @brief Attempts to read a block of bytes from an input stream.
//! @param[in] fp The input stream to read from.
//! @param[out] buffer The buffer to receive the bytes read.
//! @param[in] byteCount The count of bytes to read.
//! @retval true The required number of bytes were read and copied into buffer.
//! @retval false The required number of bytes could not be read.
bool tryRead(FILE *fp, void *buffer, size_t byteCount)
{
    return fread(buffer, 1, byteCount, fp) == byteCount;
}

//! @brief Attempts to read a block of bytes from an input stream.
//! @param[in] fp The input stream to read from.
//! @param[out] buffer The buffer to receive the bytes read.
//! @param[in] byteCount The count of bytes to read.
//! @retval true The required number of bytes were read and copied into buffer.
//! @retval false The required number of bytes could not be read.
bool tryRead(StdFilePtr &fp, void *buffer, size_t byteCount)
{
    return fread(buffer, 1, byteCount, fp.get()) == byteCount;
}

//! @brief Attempts to parse an integer from a string.
bool tryParseHex(const char *str, uint64_t &value)
{
    value = 0;
    bool hasValue = false;

    if (str != nullptr)
    {
        value = std::strtoull(str, nullptr, 16);

        hasValue = value != ULLONG_MAX;
    }

    return hasValue;
}

//! @brief Appends text formatted with printf() to an STL string.
//! @param[out] target The STL string to append to.
//! @param[in] format The format specification to process.
//! @param[in] ... The values to be formatted into the string.
void appendFormat(std::string &target, const char *format, ...)
{
    va_list sizeArgs, formatArgs;
    va_start(sizeArgs, format);
    char dummy = '\0';

    va_copy(formatArgs, sizeArgs);
    int requiredLength = vsnprintf(&dummy, 0, format, sizeArgs);

    if (requiredLength > 0)
    {
        size_t bufferSize = static_cast<size_t>(requiredLength) + 1;

        std::vector<char> buffer;
        buffer.resize(bufferSize, '\0');

        int length = vsnprintf(buffer.data(), bufferSize, format, formatArgs);

        if (length > 0)
        {
            target.append(buffer.data(), static_cast<size_t>(length));
        }
    }
}

//! @brief Compares an STL string to a static string ignoring case.
//! @param[in] lhs The STL string to compare.
//! @param[in] rhs The static string to compare against.
//! @retval true The strings have identical values, ignoring differences in case.
//! @retval false The strings have different values.
bool isEqualIgnoreCase(const std::string &lhs, const char *rhs)
{
#ifdef _MSC_VER
    return _strcmpi(lhs.c_str(), rhs) == 0;
#else
    return strcasecmp(lhs.c_str(), rhs) == 0;
#endif
}

//! @brief Creates a path which is absolute, if it isn't already.
//! @param[in] basePath The absolute path used to offset path if it is relative.
//! @param[in] path The possibly relative path to make absolute.
//! @return An absolute file path.
std::string makeFullPath(const std::string &basePath, const std::string &path)
{
    if (isPathRooted(path))
    {
        return path;
    }
    else
    {
        std::string result;
        result.reserve(path.length() + basePath.length() + 1);

        result.assign(basePath);

        appendPathElements(result, path);

        return result;
    }
}

////////////////////////////////////////////////////////////////////////////////


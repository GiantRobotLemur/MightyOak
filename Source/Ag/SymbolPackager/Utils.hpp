//! @file SymbolPackager/Utils.hpp
//! @brief The declaration of stand-alone helper functions for use by the
//! symbol packager tool.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SYMBOL_PACKAGER_HPP__
#define __AG_SYMBOL_PACKAGER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdarg>
#include <string>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A structure referencing a bounded character string.
struct BoundedString
{
    const char *Text;
    size_t Length;
    size_t HashCode;

    BoundedString(const char *text);
    BoundedString(const char *text, size_t length);
    BoundedString(const std::string &text);

    bool operator==(const BoundedString &rhs) const;
    bool operator!=(const BoundedString &rhs) const;
    bool isEqualIgnoreCase(const BoundedString &rhs) const;
    bool startsWith(const BoundedString &rhs) const;
};

//! @brief An object which can automatically close an std::FILE as part of a
//! std::unique_ptr<>.
struct StdFileCloser
{
    //! @brief Closes a file if the pointer is valid.
    //! @param[in] file The file to close.
    void operator()(FILE *file) const
    {
        if (file != nullptr)
        {
            fclose(file);
        }
    }
};

//! @brief A unique pointer to an stdio-FILE object which calls fclose() on the
//! pointer at destruction.
typedef std::unique_ptr<FILE, StdFileCloser> StdFilePtr;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool tryOpenFile(const char *fileName, const char *mode, StdFilePtr &file);
bool tryRead(FILE *fp, void *buffer, size_t byteCount);
bool tryRead(StdFilePtr &fp, void *buffer, size_t byteCount);
bool tryParseHex(const char *str, uint64_t &value);
void appendFormat(std::string &target, const char *format, ...);
bool isEqualIgnoreCase(const std::string &lhs, const char *rhs);
std::string makeFullPath(const std::string &basePath, const std::string &path);

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

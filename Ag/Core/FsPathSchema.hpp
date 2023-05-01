//! @file Ag/Core/FsPathSchema.hpp
//! @brief The declaration of an object which abstracts the differences between
//! file path formats.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_FS_PATH_SCHEMA_HPP__
#define __AG_CORE_FS_PATH_SCHEMA_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/FsPath.hpp"

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
//! @brief An object which abstracts the differences between
//! file path formats.
class PathSchema
{
protected:
    // Construction/Destruction
    PathSchema() = default;
public:
    virtual ~PathSchema() = default;

    // Accessors
    //! @brief Determines if file path elements in the current schema are
    //! case sensitive when compared.
    virtual bool isCaseSensitive() const =0;

    //! @brief Gets the preferred separator character for complete file paths.
    virtual char getPathSeparator() const =0;

    //! @brief Gets the preferred wide separator character for complete file paths.
    virtual wchar_t getWidePathSeparator() const =0;

    //! @brief Gets the preferred separator character for elements within a
    //! path expressed using the current schema.
    virtual char getElementSeparator() const =0;

    //! @brief Gets the preferred wide separator character for elements within
    //! a path expressed using the current schema.
    virtual wchar_t getWideElementSeparator() const =0;

    //! @brief Determines if a character can be used to separate path elements.
    //! @param[in] character The character to test.
    //! @retval true The character is a valid separator.
    //! @retval false The character is not a valid separator.
    virtual bool isValidElementSeparator(char32_t character) const = 0;

    //! @brief Determines if a character is valid in a path element.
    //! @param[in] ch The character to test.
    //! @retval true The character can be included in a path element.
    //! @retval false The character is illegal in a path element.
    virtual bool isValidElementCharacter(char32_t ch) const =0;

    //! @brief Attempts to parse the root of a file path expressed using the
    //! current schema.
    //! @param[in,out] pos The position of the next character to analyse, is
    //! updated to a position after the last character of the root on success.
    //! @param[in] end The position after the last character of the path string.
    //! @param[out] root A string to receive the characters of the valid
    //! path root, if one is successfully parsed.
    //! @param[out] error Receives text describing why the operation failed
    //! if it was not successful.
    //! @param[out] rootType Receives the type of path root parsed.
    //! @retval true The root of the path pointed to by pos was successfully
    //! parsed, pos, root, and root type were all updated.
    //! @retval false The characters formed an invalid root of a file path.
    //! Error is updated with details.
    //! @note A relative path will return true with rootType set to
    //! PathRootType::None.
    virtual bool tryParsePathRoot(String::iterator &pos, const String::iterator &end,
                                  std::u32string &root, string_ref_t error,
                                  PathRootType &rootType) const =0;

    //! @brief Formats the root of a file path based on the current schema.
    //! @param[in] rootType The type of root to format.
    //! @param[in] rootText The text of the path root to express.
    //! @param[in] usage The usage to which the path will be put.
    //! @return A UTF-8 encoded string holding the path root.
    virtual std::string rootToString(PathRootType rootType,
                                     string_cref_t rootText,
                                     PathUsage usage) const = 0;

    //! @brief Formats the root of a file path based on the current schema.
    //! @param[in] rootType The type of root to format.
    //! @param[in] rootText The text of the path root to express.
    //! @param[in] usage The usage to which the path will be put.
    //! @return A wide character string holding the path root.
    virtual std::wstring rootToWideString(PathRootType rootType,
                                          string_cref_t rootText,
                                          PathUsage usage) const = 0;

    //! @brief Formats a path string based on the current schema.
    //! @param[in] usage The usage to which the path string will be put.
    //! @param[in] rootType The type of root on the path string.
    //! @param[in] rootLength The length of the root in the path string.
    //! @param[in] path The characters of the path string.
    //! @return A properly formatted path string.
    virtual String pathToString(PathUsage usage, PathRootType rootType,
                                size_t rootLength, string_cref_t path) const =0;

    //! @brief Formats a wide path string based on the current schema.
    //! @param[in] usage The usage to which the path string will be put.
    //! @param[in] rootType The type of root on the path string.
    //! @param[in] rootLength The length of the root in the path string.
    //! @param[in] path The characters of the path string.
    //! @return A properly formatted wide path string.
    virtual std::wstring pathToWideString(PathUsage usage,
                                          PathRootType rootType,
                                          size_t rootLength,
                                          string_cref_t path) const = 0;

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

//! @file GLAPI/ExtensionSet.cpp
//! @brief The definition of an object which holds a set of OpenGL
//! extension names.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <cstring>

#include <algorithm>
#include <iterator>
#include <string_view>

#include "Ag/GLAPI/ExtensionSet.hpp"

namespace gl {

////////////////////////////////////////////////////////////////////////////////
// ExtensionSet Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an indexable token from a null-terminated string.
//! @param[in] token The token to wrap.
ExtensionSet::ExtensionToken::ExtensionToken(const char *token) :
    Token(token),
    Length(strlen(token)),
    Hash(0)
{
    std::hash<std::string_view> stringHasher;

    std::string_view str(token, Length);
    Hash = stringHasher(str);
}

//! @brief Determines of the current token has the same value as another.
//! @param[in] rhs The token to compare against.
//! @retval true The tokens have the same value.
//! @retval false The tokens have differing values.
bool ExtensionSet::ExtensionToken::operator==(const ExtensionToken &rhs) const
{
    return (Hash == rhs.Hash) && (Length == rhs.Length) &&
        (memcmp(Token, rhs.Token, Length) == 0);
}

//! @brief Retrieves the hash code calculated from an extension name token.
//! @param[in] rhs The token to extract a hash code from.
//! @return The hash code cached in rhs.
size_t ExtensionSet::ExtensionTokenHash::operator()(const ExtensionToken &rhs) const
{
    return rhs.Hash;
}

//! @brief Determines if an extension names exists in the set.
//! @retval true The extension is in the set.
//! @retval false The extension is not in the set.
bool ExtensionSet::containsExtension(const char *extName) const
{
    bool hasExtension = false;

    if (extName != nullptr)
    {
        ExtensionToken key(extName);

        hasExtension = (_tokens.find(key) != _tokens.end());
    }

    return hasExtension;
}

//! @brief Sets the set to an empty state.
void ExtensionSet::clear()
{
    _tokens.clear();
    _tokenData.clear();
}

//! @brief Empties the set in preparation for successive calls to addExtension().
void ExtensionSet::beginUpdate()
{
    clear();
}

//! @brief Adds an extension to the set.
//! @param[in] extName A null terminated string giving the extension name.
void ExtensionSet::addExtension(const char *extName)
{
    if (extName != nullptr)
    {
        size_t length = strlen(extName);

        if (length > 0)
        {
            // Copy the token to the buffer.
            std::copy_n(extName, length, std::back_inserter(_tokenData));

            // Ensure it is null terminated.
            _tokenData.push_back('\0');
        }
    }
}

//! @brief Indexes all tokens registered using addExtension() in order that the
//! containsExtension() function will operate.
void ExtensionSet::endUpdate()
{
    // Now we have all the data, index the tokens.
    _tokens.clear();

    for (size_t i = 0, c = _tokenData.size(); i < c; )
    {
        ExtensionToken token(_tokenData.data() + i);

        _tokens.insert(token);

        // Move on to the next token.
        i += token.Length + 1;
    }
}

//! @brief Resets the contents of the set to a new set of values.
//! @param[in] allExtensions A space-separated set of token names. A null
//! pointer indicates an empty set.
void ExtensionSet::reset(const char *allExtensions)
{
    beginUpdate();

    if (allExtensions != nullptr)
    {
        size_t hintLength = strlen(allExtensions);

        _tokenData.reserve(hintLength);
        bool isInToken = false;

        for (size_t i = 0; i < hintLength; ++i)
        {
            uint32_t state = isInToken ? 1 : 0;
            state |= (isspace(allExtensions[i]) == 0) ? 0 : 2;

            switch (state)
            {
            case 0: // Not in token, non-space character.
                _tokenData.push_back(allExtensions[i]);
                isInToken = true;
                break;

            case 1: // Is in a token, non-space character.
                _tokenData.push_back(allExtensions[i]);
                break;

            case 2: // Not in a token, space character.
            default:
                break;

            case 3: // In a token, space character.
                _tokenData.push_back('\0');
                isInToken = false;
                break;
            }
        }

        endUpdate();
    }
}

} // namespace gl
////////////////////////////////////////////////////////////////////////////////


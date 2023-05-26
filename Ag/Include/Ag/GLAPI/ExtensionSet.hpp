//! @file Ag/GLAPI/ExtensionSet.hpp
//! @brief The declaration of an object which holds a set of OpenGL
//! extension names.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_EXTENSION_SET_HPP__
#define __AG_OBJECT_GL_EXTENSION_SET_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <unordered_set>
#include <vector>

namespace gl {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which holds a set of OpenGL extension names.
class ExtensionSet
{
public:
    // Construction
    ExtensionSet() = default;
    ~ExtensionSet() = default;

    // Accessors
    bool containsExtension(const char *extName) const;

    // Operations
    void clear();
    void beginUpdate();
    void addExtension(const char *extName);
    void endUpdate();
    void reset(const char *allExtensions);
private:
    // Internal Types
    //! @brief An object representing an indexable string token stored in an
    //! external buffer.
    struct ExtensionToken
    {
        const char *Token;
        size_t Length;
        size_t Hash;

        ExtensionToken(const char *token);
        bool operator==(const ExtensionToken &rhs) const;
    };

    struct ExtensionTokenHash
    {
        size_t operator()(const ExtensionToken &rhs) const;
    };

    // Internal Fields
    std::unordered_set<ExtensionToken, ExtensionTokenHash> _tokens;
    std::vector<char> _tokenData;
};

} // namespace gl

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file Ag/Core/Version.hpp
//! @brief The declaration of a value type which represents the version of
//! a component.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_VERSION_HPP__
#define __AG_CORE_VERSION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "String.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A value type which represents the version of a component.
class Version
{
public:
    // Construction/Destruction
    Version();
    Version(uint16_t major, uint16_t minor = 0, uint16_t revision = 0,
            uint16_t patch = 0, string_cref_t comment = String::Empty);

    // Accessors
    bool isEmpty() const;
    uint16_t getMajor() const;
    void setMajor(uint16_t component);
    uint16_t getMinor() const;
    void setMinor(uint16_t component);
    uint16_t getRevision() const;
    void setRevision(uint16_t component);
    uint16_t getPatch() const;
    void setPatch(uint16_t component);
    string_cref_t getComment() const;
    void setComment(string_cref_t comment);
    String toString(uint8_t componentCount = 4, bool includeComment = true) const;

    // Operations
    void clear();
    bool operator==(const Version &rhs) const;
    bool operator!=(const Version &rhs) const;
    bool operator<(const Version &rhs) const;
    bool operator<=(const Version &rhs) const;
    bool operator>(const Version &rhs) const;
    bool operator>=(const Version &rhs) const;
    bool tryParse(utf8_cptr_t text);
    bool tryParse(string_cref_t text);
    bool tryParse(const std::string_view &text);
private:
    // Internal Constants
    static constexpr uint8_t ComponentCount = 4;

    // Internal Fields
    uint16_t _components[ComponentCount];
    String _comment;
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

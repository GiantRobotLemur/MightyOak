//! @file Core/Version.cpp
//! @brief The definition of a value type which represents the version of
//! a component.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>

#include <algorithm>

#include "Ag/Core/Format.hpp"
#include "Ag/Core/Version.hpp"


namespace Ag {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
std::string_view trim(const std::string_view &text)
{
    size_t i;
    size_t firstNonSpace = SIZE_MAX;
    size_t lastNonSpace = 0;

    for (i = 0; i < text.length(); ++i)
    {
        if (std::isspace(text[i]) == 0)
        {
            firstNonSpace = std::min(firstNonSpace, i);
            lastNonSpace = std::max(lastNonSpace, i);
        }
    }

    if (firstNonSpace <= lastNonSpace)
    {
        return std::string_view(text.data() + firstNonSpace,
                                lastNonSpace - firstNonSpace + 1);
    }
    else
    {
        return std::string_view();
    }

}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Version Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a version value in an empty state.
Version::Version()
{
    std::fill_n(_components, std::size(_components),
                static_cast<uint16_t>(0));
}

//! @brief Constructs an initialised version value 
//! @param[in] major The major component of the version number.
//! @param[in] minor The minor component of the version number.
//! @param[in] revision The revision component of the version number.
//! @param[in] patch The patch component of the version number.
//! @param[in] comment The optional comment associated with the version.
Version::Version(uint16_t major, uint16_t minor /*= 0*/, uint16_t revision /*= 0*/,
                 uint16_t patch /*= 0*/, string_cref_t comment /*= String::Empty*/)
{
    _components[0] = major;
    _components[1] = minor;
    _components[2] = revision;
    _components[3] = patch;
    _comment = comment;
}

//! @brief Determines if the version number components are all set to 0.
//! @retval true The version number components are all set to 0.
//! @retval false At least one version number component is non-zero.
//! @note The comment field doesn't affect the empty state.
bool Version::isEmpty() const
{
    uint16_t result = 0;

    for (uint8_t i = 0; i < ComponentCount; ++i)
    {
        result |= _components[i];
    }

    return result == 0;
}

//! @brief Gets the major component of the version number.
uint16_t Version::getMajor() const { return _components[0]; }

//! @brief Sets the major component of the version number.
//! @param[in] component The new component value.
void Version::setMajor(uint16_t component) { _components[0] = component; }

//! @brief Gets the minor component of the version number.
uint16_t Version::getMinor() const { return _components[1]; }

//! @brief Sets the minor component of the version number.
//! @param[in] component The new component value.
void Version::setMinor(uint16_t component) { _components[1] = component; }

//! @brief Gets the revision component of the version number.
uint16_t Version::getRevision() const { return _components[2]; }

//! @brief Sets the revision component of the version number.
//! @param[in] component The new component value.
void Version::setRevision(uint16_t component) { _components[2] = component; }

//! @brief Gets the patch component of the version number.
uint16_t Version::getPatch() const { return _components[3]; }

//! @brief Sets the patch component of the version number.
//! @param[in] component The new component value.
void Version::setPatch(uint16_t component) { _components[3] = component; }

//! @brief Gets the optional comment associated with the version.
string_cref_t Version::getComment() const { return _comment; }

//! @brief Sets the optional comment component.
//! @param[in] comment The new value of the comment field.
void Version::setComment(string_cref_t comment) { _comment = comment; }

//! @brief Formats the object as a string.
//! @param[in] componentCount The count of significant components to display.
//! @param[in] includeComment True to include the comment if it is non-empty.
//! @return The version formatted as a UTF-8 encoded string.
String Version::toString(uint8_t componentCount /*= 4*/,
                         bool includeComment /*= true*/) const
{
    FormatInfo format = FormatInfo::getNeutral();
    std::string buffer;
    size_t hintSize = (componentCount * 4);

    if (includeComment)
    {
        hintSize += _comment.getUtf8Length() + 1;
    }

    // Try to format the string without re-allocating it.
    buffer.reserve(hintSize);

    // Always add the first version component.
    appendValue(format, buffer, _components[0]);

    // Add optional subsequent components.
    for (uint8_t i = 1, c = std::min(ComponentCount, componentCount); i < c; ++i)
    {
        buffer.push_back('.');
        appendValue(format, buffer, _components[i]);
    }

    // Optionally add the comment.
    if (includeComment && (_comment.isEmpty() == false))
    {
        buffer.append(" - ");

        appendAgString(buffer, _comment);
    }

    return String(buffer);
}

//! @brief Sets the object to an empty value.
void Version::clear()
{
    std::fill_n(_components, ComponentCount, static_cast<uint16_t>(0));
    _comment = String::Empty;
}

//! @brief Determines if the numeric components of the version are equal to
//! those of another.
//! @param[in] rhs The version to compare against.
//! @retval true The version values are equal.
//! @retval false The version values differ.
//! @note The comment field is ignored.
bool Version::operator==(const Version &rhs) const
{
    return std::equal(_components, _components + ComponentCount, rhs._components);
}

//! @brief Determines if the numeric components of the version differs with
//! that of another.
//! @param[in] rhs The version to compare against.
//! @retval true The version values differ.
//! @retval false The version values are equal.
//! @note The comment field is ignored.
bool Version::operator!=(const Version &rhs) const
{
    return std::equal(_components, _components + ComponentCount,
                      rhs._components) == false;
}

//! @brief Determines if the version value of the current object is less than
//! that of another.
//! @param[in] rhs The version to compare against.
//! @retval true The current object has a lower version than rhs.
//! @retval false The current object has an equal or higher version than rhs.
//! @note The comment field is ignored.
bool Version::operator<(const Version &rhs) const
{
    for (uint8_t i = 0; i < ComponentCount; ++i)
    {
        if (_components[i] != rhs._components[i])
        {
            return (_components[i] < rhs._components[i]);
        }
    }

    return false;
}

//! @brief Determines if the version value of the current object is less than
//! or equal to that of another.
//! @param[in] rhs The version to compare against.
//! @retval true The current object has a lower or equal version than rhs.
//! @retval false The current object has a higher version than rhs.
//! @note The comment field is ignored.
bool Version::operator<=(const Version &rhs) const
{
    for (uint8_t i = 0; i < ComponentCount; ++i)
    {
        if (_components[i] != rhs._components[i])
        {
            return (_components[i] < rhs._components[i]);
        }
    }

    // All components are equal.
    return true;
}

//! @brief Determines if the version value of the current object is greater than
//! that of another.
//! @param[in] rhs The version to compare against.
//! @retval true The current object has a higher version than rhs.
//! @retval false The current object has an equal or lower version than rhs.
//! @note The comment field is ignored.
bool Version::operator>(const Version &rhs) const
{
    for (uint8_t i = 0; i < ComponentCount; ++i)
    {
        if (_components[i] != rhs._components[i])
        {
            return (_components[i] > rhs._components[i]);
        }
    }

    return false;
}

//! @brief Determines if the version value of the current object is greater than
//! or equal to that of another.
//! @param[in] rhs The version to compare against.
//! @retval true The current object has an equal or higher version than rhs.
//! @retval false The current object has a lower version than rhs.
//! @note The comment field is ignored.
bool Version::operator>=(const Version &rhs) const
{
    for (uint8_t i = 0; i < ComponentCount; ++i)
    {
        if (_components[i] != rhs._components[i])
        {
            return (_components[i] > rhs._components[i]);
        }
    }

    // All components are equal.
    return true;
}

//! @brief Attempts to parse the version value from an array of UTF-8
//! encoded characters.
//! @param[in] text The string to parse.
//! @retval true The string represented a valid version value.
//! @retval false The string was not a valid version.
bool Version::tryParse(utf8_cptr_t text)
{
    return (text != nullptr) && tryParse(std::string_view(text));
}

//! @brief Attempts to parse the version value from an immutable UTF-8
//! text string.
//! @param[in] text The string to parse.
//! @retval true The string represented a valid version value.
//! @retval false The string was not a valid version.
bool Version::tryParse(string_cref_t text)
{
    return tryParse(text.toUtf8View());
}

//! @brief Attempts to parse the version value from a text string.
//! @param[in] text The string to parse.
//! @retval true The string represented a valid version value.
//! @retval false The string was not a valid version.
bool Version::tryParse(const std::string_view &text)
{
    enum class State : uint8_t
    {
        BeforeVersion,
        BeforeComponent,
        InComponent,
        AfterComponents,
        AfterSeparator,
        Complete,
        Error,
    };

    uint16_t components[ComponentCount];
    uint8_t componentCount = 0;
    State state = State::BeforeVersion;
    std::string_view comment;

    for (size_t i = 0; i < text.length(); ++i)
    {
        char next = text[i];

        switch (state)
        {
        case State::BeforeVersion:
            // Before anything.
            if ((next >= '0') && (next <= '9'))
            {
                components[0] = next - '0';
                state = State::InComponent;
            }
            else if (std::isspace(next) == 0)
            {
                state = State::Error;
            }
            break;

        case State::BeforeComponent: // After a '.'
            if ((next >= '0') && (next <= '9'))
            {
                if (componentCount < ComponentCount)
                {
                    components[componentCount] = next - '0';
                }

                state = State::InComponent;
            }
            else
            {
                state = State::Error;
            }
            break;

        case State::InComponent:
            if ((next >= '0') && (next <= '9'))
            {
                if (componentCount < ComponentCount)
                {
                    components[componentCount] = (components[componentCount] * 10) +
                                                 (next - '0');
                }
            }
            else if (next == '.')
            {
                ++componentCount;
                state = State::BeforeComponent;
            }
            else if (std::isspace(next))
            {
                ++componentCount;
                state = State::AfterComponents;
            }
            break;

        case State::AfterComponents:
            if ((next == '-') || (next == ':'))
            {
                state = State::AfterSeparator;
            }
            else if (std::isspace(next) == 0)
            {
                char closing;

                switch (next)
                {
                case '(': closing = ')'; break;
                case '[': closing = ']'; break;
                case '{': closing = '}'; break;
                default: closing = '\0'; break;
                }

                size_t offset = (closing == '\0') ? std::string_view::npos :
                                                    text.find(')', i + 1);

                if (offset == std::string_view::npos)
                {
                    // There was no closing bracket.
                    comment = trim(text.substr(i));
                }
                else
                {
                    // There was a closing bracket, capture the contents.
                    comment = trim(text.substr(i + 1, offset - i - 1));
                }

                state = State::Complete;
            }
            break;

        case State::AfterSeparator:
            if (std::isspace(next) == 0)
            {
                char closing;

                switch (next)
                {
                case '(': closing = ')'; break;
                case '[': closing = ']'; break;
                case '{': closing = '}'; break;
                default: closing = '\0'; break;
                }

                size_t offset = (closing == '\0') ? std::string_view::npos :
                    text.find(')', i + 1);

                if (offset == std::string_view::npos)
                {
                    // There was no closing bracket.
                    comment = trim(text.substr(i));
                }
                else
                {
                    // There was a closing bracket, capture the contents.
                    comment = trim(text.substr(i + 1, offset - i - 1));
                }

                state = State::Complete;
            }
            break;

        case State::Complete: break;
        case State::Error: break;
        default: state = State::Error; break;
        }

        if (state >= State::Complete)
        {
            break;
        }
    }

    if (state == State::InComponent)
        ++componentCount;

    // If the resultant state was terminal, the parsing process was successful.
    bool isOK;

    switch (state)
    {
    case State::BeforeVersion:
    case State::BeforeComponent:
    case State::Error:
    default:
        isOK = false;
        break;

    case State::InComponent:
    case State::AfterComponents:
    case State::AfterSeparator:
    case State::Complete:
        componentCount = std::min(componentCount, ComponentCount);

        // Copy the parsed components and blank the others.
        std::copy_n(components, componentCount, _components);
        std::fill_n(_components + componentCount,
                    ComponentCount - componentCount,
                    static_cast<uint16_t>(0));
        _comment = comment;
        isOK = true;
        break;
    }

    return isOK;
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


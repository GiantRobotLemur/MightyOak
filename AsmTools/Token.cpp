//! @file AsmTools/Token.cpp
//! @brief The definition of an object representing a set of characters
//! recognised with a specific classification.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Token.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates an empty token.
Token::Token() :
    _classification(TokenClass::Empty)
{
    clearProperties();
}

//! @brief Creates a copy of a token with a different location.
//! @param[in] at The location for the new token.
//! @param[in] original The token to inherit classification, value and
//! properties from.
Token::Token(const Location &at, Token &original) :
    _location(at),
    _value(original._value),
    _classification(original._classification)
{
    // Copy the property values.
    std::copy_n(original._properties, MaxPropCount, _properties);
}

//! @brief Creates a token with a specific classification.
//! @param[in] at The location for the new token.
//! @param[in] classification The classification of the recognised token.
Token::Token(const Location &at, TokenClass classification) :
    _location(at),
    _classification(classification)
{
    clearProperties();
}

//! @brief Creates a token with a specific classification and value.
//! @param[in] at The location for the new token.
//! @param[in] classification The classification of the recognised token.
//! @param[in] value The significant characters of the token.
Token::Token(const Location &at, TokenClass classification, const String &value) :
    _location(at),
    _value(value),
    _classification(classification)
{
    clearProperties();
}

//! @brief Gets the classification of the recognised token.
TokenClass Token::getClass() const
{
    return _classification;
}

//! @brief Gets the significant characters of the token, if any.
const String &Token::getValue() const
{
    return _value;
}

//! @brief Gets the source location of the first character of the token.
const Location &Token::getLocation() const
{
    return _location;
}

//! @brief Annotates the token with a property value.
//! @param[in] propId The identifier of the property.
//! @param[in] value The value to associate with the property.
void Token::addScalarProperty(TokenProperty propId, uint8_t value)
{
    size_t index;

    for (index = 0; index < MaxPropCount; ++index)
    {
        PropValue &prop = _properties[index];
        TokenProperty id = static_cast<TokenProperty>(prop.first);

        if (id == TokenProperty::Null)
        {
            prop.first = static_cast<uint8_t>(propId);
            prop.second = value;
            break;
        }
        else if (id == propId)
        {
            prop.second = value;
            break;
        }
    }

    if (index >= MaxPropCount)
    {
        throw OperationException("Too many token properties.");
    }
}

//! @brief Attempts to find the value of a property annotating the token.
//! @param[in] propId The identifier of the property to find.
//! @param[out] value Receives the property value if it was defined.
//! @retval true The property was defined and its value returned.
//! @retval false The property was not defined on this token.
bool Token::tryGetScalarProperty(TokenProperty propId, uint8_t &value) const
{
    bool hasValue = false;

    for (size_t index = 0; (hasValue == false) && (index < MaxPropCount); ++index)
    {
        const PropValue &prop = _properties[index];
        TokenProperty id = static_cast<TokenProperty>(prop.first);

        if (id == propId)
        {
            value = prop.second;
            hasValue = true;
        }
    }

    return hasValue;
}

//! @brief Resets the token to an empty state.
void Token::clear()
{
    _classification = TokenClass::Empty;
    _value = String::Empty;
    clearProperties();
}

//! @brief Resets all properties to an empty state.
void Token::clearProperties()
{
    PropValue empty(static_cast<uint8_t>(0), static_cast<uint8_t>(0));

    std::fill_n(_properties, MaxPropCount, empty);
}

//! @brief Updates the identifying properties of the token and clears
//! any property values which were previously set.
//! @param[in] at The location for the new token.
//! @param[in] classification The classification of the recognised token.
void Token::reset(const Location &at, TokenClass classification)
{
    _location = at;
    _classification = classification;
    _value = String::Empty;

    clearProperties();
}

//! @brief Updates the identifying properties of the token and clears
//! any property values which were previously set.
//! @param[in] at The location for the new token.
//! @param[in] classification The classification of the recognised token.
//! @param[in] value The significant characters of the token.
void Token::reset(const Location &at, TokenClass classification, const String &value)
{
    _location = at;
    _classification = classification;
    _value = value;

    clearProperties();
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


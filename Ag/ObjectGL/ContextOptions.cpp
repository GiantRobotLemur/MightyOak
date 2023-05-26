//! @file ObjectGL/ContextOptions.cpp
//! @brief The definition of an object which specifies the requirements of an
//! OpenGL renderer to be created.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/ObjectGL/ContextOptions.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// ContextOptions Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty set of OpenGL context requirements.
ContextOptions::ContextOptions()
{
}

//! @brief Sets the preferred API version the context should implement.
//! @param[in] majorVersion The major context version, i.e. 4 in 4.6.
//! @param[in] minorVersion The minor context version, i.e. 6, in 4.6.
void ContextOptions::setPreferredVersion(uint32_t majorVersion, uint32_t minorVersion)
{
    setProperty(ContextPropertyID::MajorVersion, majorVersion);
    setProperty(ContextPropertyID::MinorVersion, minorVersion);
}

//! @brief Defines whether the context should implement the Core profile.
//! @param[in] isEnabled True to require the core profile API to be supported.
void ContextOptions::enableCoreProfile(bool isEnabled)
{
    setProperty(ContextPropertyID::UseCoreProfile, isEnabled ? ~0u : 0);
}
//! @brief Defines whether the context should implement the Compatibility profile.
//! @param[in] isEnabled True to require the compatibility profile API to be supported.
void ContextOptions::enableCompatibilityProfile(bool isEnabled)
{
    setProperty(ContextPropertyID::UseCompatabilityProfile, isEnabled ? ~0u : 0);
}

//! @brief Defines whether the context should implement the OpenGL ES profile.
//! @param[in] isEnabled True to require the compatibility profile API to be supported.
void ContextOptions::enableEmbeddedProfile(bool isEnabled)
{
    setProperty(ContextPropertyID::UseESProfile, isEnabled ? ~0u : 0);
}

//! @brief Defines whether the context should require forward compatibility.
//! @param[in] isEnabled True to require that no deprecated function will be supported.
void ContextOptions::enableForwardCompatibility(bool isEnabled)
{
    setProperty(ContextPropertyID::UseForwardCompatiblity, isEnabled ? ~0u : 0);
}

//! @brief Defines whether the context should support debug output.
//! @param[in] isEnabled True to enable debug extensions which may reduce
//! performance but will allow additional diagnostics.
void ContextOptions::enableDebugging(bool isEnabled)
{
    setProperty(ContextPropertyID::EnableDebugging, isEnabled ? ~0u : 0);
}

//! @brief Removes definitions for all property values.
void ContextOptions::reset()
{
    _properties.clear();
}

//! @brief Gets the value of a property, returning a default if it wasn't defined.
//! @param[in] id The identifier of the property to get.
//! @param[in] defaultValue The value to return if the property was not defined.
//! @return Either the property value or default value.
uint32_t ContextOptions::getProperty(ContextPropertyID id, uint32_t defaultValue) const
{
    auto pos = _properties.find(id);

    return (pos == _properties.end()) ? defaultValue : pos->second;
}

//! @brief Gets the value of a boolean property, returning a default if it wasn't defined.
//! @param[in] id The identifier of the property to get.
//! @param[in] defaultValue The value to return if the property was not defined.
//! @return Either the property value or default value.
bool ContextOptions::getProperty(ContextPropertyID id, bool defaultValue) const
{
    auto pos = _properties.find(id);

    return (pos == _properties.end()) ? defaultValue : pos->second;
}

//! @brief Attempts to get the value of a property.
//! @tparam T The data type of the property to get.
//! @param[in] id The identifier of the property to get.
//! @param[out] value Receives the value of the property.
//! @retval true The property was defined and its value returned.
//! @retval false The property was not defined.
bool ContextOptions::tryGetProperty(ContextPropertyID id, uint32_t &value) const
{
    auto pos = _properties.find(id);

    if (pos == _properties.end())
    {
        return false;
    }
    else
    {
        value = pos->second;
        return true;
    }
}

//! @brief Sets the value of a specific property.
//! @param[in] id The identifier of the property to set.
//! @param[in] value The new value of the property.
void ContextOptions::setProperty(ContextPropertyID id, uint32_t value)
{
    auto resultPair = _properties.try_emplace(id, value);

    if (resultPair.second == false)
    {
        // Overwrite the existing value.
        resultPair.first->second = value;
    }
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


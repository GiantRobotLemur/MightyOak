//! @file ObjectGL/ContextOptions.hpp
//! @brief The declaration of an object which specifies the requirements of an
//! OpenGL renderer to be created.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_CONTEXT_OPTIONS_HPP__
#define __AG_OBJECT_GL_CONTEXT_OPTIONS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "Ag/Core/LinearSortedMap.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Expresses the properties which can be set to define the requirements
//! of an OpenGL context which is to be created.
enum class ContextPropertyID
{
    MajorVersion,
    MinorVersion,
    UseCoreProfile,
    UseCompatabilityProfile,
    UseESProfile,
    UseForwardCompatiblity,
    EnableDebugging,

    MaxID,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which specifies the requirements of an OpenGL renderer to
//! be created.
class ContextOptions
{
public:
    // Construction/Destruction
    ContextOptions();

    // Accessors
    void setPreferredVersion(uint32_t majorVersion, uint32_t minorVersion);
    void enableCoreProfile(bool isEnabled);
    void enableCompatibilityProfile(bool isEnabled);
    void enableEmbeddedProfile(bool isEnabled);
    void enableForwardCompatibility(bool isEnabled);
    void enableDebugging(bool isEnabled);

    // Operations
    void reset();
    uint32_t getProperty(ContextPropertyID id, uint32_t defaultValue) const;
    bool getProperty(ContextPropertyID id, bool defaultValue) const;
    bool tryGetProperty(ContextPropertyID id, uint32_t &value) const;

    //! @brief Attempts to get a strongly typed property.
    //! @tparam T The data type of the property to get.
    //! @param[in] id The identifier of the property to get.
    //! @param[out] value Receives the value of the property.
    //! @retval true The property was defined and its value returned.
    //! @retval false The property was not defined.
    template<typename T> bool tryGetTypedProperty(ContextPropertyID id, T &value) const
    {
        bool hasValue = false;
        uint32_t rawValue = 0;

        if (tryGetProperty(id, rawValue))
        {
            hasValue = true;
            value = static_cast<T>(rawValue);
        }

        return hasValue;
    }

    //! @brief Gets a value for a strongly typed property.
    //! @tparam T The data type of the property to get.
    //! @param[in] id The identifier of the property to get.
    //! @param[in] defaultValue The value to return if the property was not defined.
    //! @return Either the property value or default value.
    template<typename T> T getTypedValue(ContextPropertyID id, T defaultValue) const
    {
        uint32_t rawValue = 0;

        return tryGetProperty(id, rawValue) ? static_cast<T>(rawValue) : defaultValue;
    }

private:
    // Internal Types
    using PropertyMap = std::map<ContextPropertyID, uint32_t>;

    // Internal Functions
    void setProperty(ContextPropertyID id, uint32_t value);

    // Internal Fields
    PropertyMap _properties;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

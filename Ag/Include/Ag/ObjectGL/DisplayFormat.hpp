//! @file ObjectGL/DisplayFormat.hpp
//! @brief The declaration of an object specifying the output format of an
//! OpenGL renderer.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_DISPLAY_FORMAT_HPP__
#define __AG_OBJECT_GL_DISPLAY_FORMAT_HPP__

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
enum class DisplayPropertyID
{
    HasDoubleBuffer,
    HasHardwareAcceleration,
    RedBitCount,
    GreenBitCount,
    BlueBitCount,
    AlphaBitCount,
    ColourBitCount,
    DepthBitCount,
    StencilBitCount,

    MaxID,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object specifying the output format of an OpenGL renderer.
class DisplayFormat
{
public:
    // Construction/Destruction
    DisplayFormat();
    ~DisplayFormat() = default;

    // Accessors
    void enableDoubleBuffering(bool isEnabled);
    void enableHardwareAcceleration(bool isEnabled);

    void setRedBitCount(uint8_t bitCount);
    void setGreenBitCount(uint8_t bitCount);
    void setBlueBitCount(uint8_t bitCount);
    void setAlphaBitCount(uint8_t bitCount);
    void setColourBitCount(uint8_t bitCount);
    void setDepthBitCount(uint8_t bitCount);
    void setStencilBitCount(uint8_t bitCount);

    // Operations
    void reset();
    uint32_t getProperty(DisplayPropertyID id, uint32_t defaultValue) const;
    bool getProperty(DisplayPropertyID id, bool defaultValue) const;
    bool tryGetProperty(DisplayPropertyID id, uint32_t &value) const;

    //! @brief Attempts to get a strongly typed property.
    //! @tparam T The data type of the property to get.
    //! @param[in] id The identifier of the property to get.
    //! @param[out] value Receives the value of the property.
    //! @retval true The property was defined and its value returned.
    //! @retval false The property was not defined.
    template<typename T> bool tryGetTypedProperty(DisplayPropertyID id, T &value) const
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

    bool tryGetFlagProperty(DisplayPropertyID id, bool &value) const;

    //! @brief Gets a value for a strongly typed property.
    //! @tparam T The data type of the property to get.
    //! @param[in] id The identifier of the property to get.
    //! @param[in] defaultValue The value to return if the property was not defined.
    //! @return Either the property value or default value.
    template<typename T> T getTypedValue(DisplayPropertyID id, T defaultValue) const
    {
        uint32_t rawValue = 0;

        return tryGetProperty(id, rawValue) ? static_cast<T>(rawValue) : defaultValue;
    }

private:
    // Internal Types
    using PropertyMap = std::map<DisplayPropertyID, uint32_t>;

    // Internal Functions
    void setProperty(DisplayPropertyID id, uint32_t value);

    // Internal Fields
    PropertyMap _properties;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

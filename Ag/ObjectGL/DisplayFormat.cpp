//! @file ObjectGL/DisplayFormat.cpp
//! @brief The definition of an object specifying the output format of an
//! OpenGL renderer.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/ObjectGL/DisplayFormat.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// DisplayFormat Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty display format with no properties set.
DisplayFormat::DisplayFormat()
{
}

//! @brief Sets whether double buffering should be enabled.
//! @param[in] isEnabled True to enable double buffering, false to write
//! directly to the screen.
void DisplayFormat::enableDoubleBuffering(bool isEnabled)
{
    setProperty(DisplayPropertyID::HasDoubleBuffer, isEnabled ? ~0u : 0);
}

//! @brief Sets whether hardware accelerated rendering should be enabled.
//! @param[in] isEnabled True to require hardware accelerated rendering, false
//! to allow software emulation.
void DisplayFormat::enableHardwareAcceleration(bool isEnabled)
{
    setProperty(DisplayPropertyID::HasHardwareAcceleration, isEnabled ? ~0u : 0);
}

//! @brief Specifies the required number of bits representing the red colour
//! component in output.
//! @param[in] bitCount The required component bit count.
void DisplayFormat::setRedBitCount(uint8_t bitCount)
{
    setProperty(DisplayPropertyID::RedBitCount, bitCount);
}

//! @brief Specifies the required number of bits representing the green colour
//! component in output.
//! @param[in] bitCount The required component bit count.
void DisplayFormat::setGreenBitCount(uint8_t bitCount)
{
    setProperty(DisplayPropertyID::GreenBitCount, bitCount);
}

//! @brief Specifies the required number of bits representing the blue colour
//! component in output.
//! @param[in] bitCount The required component bit count.
void DisplayFormat::setBlueBitCount(uint8_t bitCount)
{
    setProperty(DisplayPropertyID::BlueBitCount, bitCount);
}

//! @brief Specifies the required number of bits representing transparency
//! in colour output.
//! @param[in] bitCount The required component bit count.
void DisplayFormat::setAlphaBitCount(uint8_t bitCount)
{
    setProperty(DisplayPropertyID::AlphaBitCount, bitCount);
}

//! @brief Specifies the required number of bits representing an output pixel.
//! @param[in] bitCount The required total bit count.
void DisplayFormat::setColourBitCount(uint8_t bitCount)
{
    setProperty(DisplayPropertyID::ColourBitCount, bitCount);
}

//! @brief Specifies the required number of bits representing depth in any
//! depth buffer associated with the output.
//! @param[in] bitCount The required total bit count.
void DisplayFormat::setDepthBitCount(uint8_t bitCount)
{
    setProperty(DisplayPropertyID::DepthBitCount, bitCount);
}

//! @brief Specifies the required number of stencil bits in any stencil buffer
//! associated with the output.
//! @param[in] bitCount The required total bit count.
void DisplayFormat::setStencilBitCount(uint8_t bitCount)
{
    setProperty(DisplayPropertyID::StencilBitCount, bitCount);
}

//! @brief Removes definitions for all property values.
void DisplayFormat::reset()
{
    _properties.clear();
}

//! @brief Gets the value of a property, returning a default if it wasn't defined.
//! @param[in] id The identifier of the property to get.
//! @param[in] defaultValue The value to return if the property was not defined.
//! @return Either the property value or default value.
uint32_t DisplayFormat::getProperty(DisplayPropertyID id, uint32_t defaultValue) const
{
    auto pos = _properties.find(id);

    return (pos == _properties.end()) ? defaultValue : pos->second;
}

//! @brief Gets the value of a boolean property, returning a default if it wasn't defined.
//! @param[in] id The identifier of the property to get.
//! @param[in] defaultValue The value to return if the property was not defined.
//! @return Either the property value or default value.
bool DisplayFormat::getProperty(DisplayPropertyID id, bool defaultValue) const
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
bool DisplayFormat::tryGetProperty(DisplayPropertyID id, uint32_t &value) const
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
void DisplayFormat::setProperty(DisplayPropertyID id, uint32_t value)
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


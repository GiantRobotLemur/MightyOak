//! @file ObjectGL/WGL_Tools.hpp
//! @brief The declaration of  functions to assist using OpenGL on Windows.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_WGL_TOOLS_HPP__
#define __AG_OBJECT_GL_WGL_TOOLS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <initializer_list>
#include <map>
#include <utility>

// Must be included before GLAPI headers.
#include "Win32API.hpp"

#include "Ag/GLAPI/APIResolvers.hpp"
#include "Ag/GLAPI/WGL_CommandSets.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Identifiers of properties which can be specified when describing
//! the rendering format of an OpenGL context yet to be created.
enum class FormatProperty : GLenum
{
    NumPixelFormat        = wgl::Global::NumberPixelFormats,     // WGL_NUMBER_PIXEL_FORMATS_ARB
    DrawToWindow          = wgl::Global::DrawToWindow,           // WGL_DRAW_TO_WINDOW_ARB
    DrawToBitmap          = wgl::Global::DrawToBitmap,           // WGL_DRAW_TO_BITMAP_ARB
    Acceleration          = wgl::Global::Acceleration,           // WGL_ACCELERATION_ARB
    NeedsPalette          = wgl::Global::NeedPalette,            // WGL_NEED_PALETTE_ARB
    NeedsSystemPalette    = wgl::Global::NeedSystemPalette,      // WGL_NEED_SYSTEM_PALETTE_ARB
    SwapLayerBuffers      = wgl::Global::SwapLayerBuffers,       // WGL_SWAP_LAYER_BUFFERS_ARB
    SwapMethod            = wgl::Global::SwapMethod,             // WGL_SWAP_METHOD_ARB
    NumOverlays           = wgl::Global::NumberOverlays,         // WGL_NUMBER_OVERLAYS_ARB
    NumUnderlays          = wgl::Global::NumberUnderlays,        // WGL_NUMBER_UNDERLAYS_ARB
    Transparent           = wgl::Global::Transparent,            // WGL_TRANSPARENT_ARB
    TransparentRedValue   = wgl::Global::TransparentRedValue,    // WGL_TRANSPARENT_RED_VALUE_ARB
    TransparentGreenValue = wgl::Global::TransparentGreenValue,  // WGL_TRANSPARENT_GREEN_VALUE_ARB
    TransparentBlueValue  = wgl::Global::TransparentBlueValue,   // WGL_TRANSPARENT_BLUE_VALUE_ARB
    TransparentAlphaValue = wgl::Global::TransparentAlphaValue,  // WGL_TRANSPARENT_ALPHA_VALUE_ARB
    TransparentIndexValue = wgl::Global::TransparentIndexValue,  // WGL_TRANSPARENT_INDEX_VALUE_ARB
    ShareDepth            = wgl::Global::ShareDepth,             // WGL_SHARE_DEPTH_ARB
    ShareStencil          = wgl::Global::ShareStencil,           // WGL_SHARE_STENCIL_ARB
    ShareAccum            = wgl::Global::ShareAccum,             // WGL_SHARE_ACCUM_ARB
    SupportGDI            = wgl::Global::SupportGDI,             // WGL_SUPPORT_GDI_ARB
    SupportOpenGL         = wgl::Global::SupportOpenGL,          // WGL_SUPPORT_OPENGL_ARB
    DoubleBuffer          = wgl::Global::DoubleBuffer,           // WGL_DOUBLE_BUFFER_ARB
    Stereo                = wgl::Global::Stereo,                 // WGL_STEREO_ARB
    PixelType             = wgl::Global::PixelType,              // WGL_PIXEL_TYPE_ARB
    ColorBits             = wgl::Global::ColorBits,              // WGL_COLOR_BITS_ARB
    RedBits               = wgl::Global::RedBits,                // WGL_RED_BITS_ARB
    RedShift              = wgl::Global::RedShift,               // WGL_RED_SHIFT_ARB
    GreenBits             = wgl::Global::GreenShift,             // WGL_GREEN_BITS_ARB
    GreenShift            = wgl::Global::GreenShift,             // WGL_GREEN_SHIFT_ARB
    BlueBits              = wgl::Global::BlueBits,               // WGL_BLUE_BITS_ARB
    BlueShift             = wgl::Global::BlueShift,              // WGL_BLUE_SHIFT_ARB
    AlphaBits             = wgl::Global::AlphaBits,              // WGL_ALPHA_BITS_ARB
    AlphaShift            = wgl::Global::AlphaShift,             // WGL_ALPHA_SHIFT_ARB
    AccumBits             = wgl::Global::AccumBits,              // WGL_ACCUM_BITS_ARB
    AccumRedBits          = wgl::Global::AccumRedBits,           // WGL_ACCUM_RED_BITS_ARB
    AccumGreenBits        = wgl::Global::AccumGreenBits,         // WGL_ACCUM_GREEN_BITS_ARB
    AccumBlueBits         = wgl::Global::AccumBlueBits,          // WGL_ACCUM_BLUE_BITS_ARB
    AccumAlphaBits        = wgl::Global::AccumAlphaBits,         // WGL_ACCUM_ALPHA_BITS_ARB
    DepthBits             = wgl::Global::DepthBits,              // WGL_DEPTH_BITS_ARB
    StencilBits           = wgl::Global::StencilBits,            // WGL_STENCIL_BITS_ARB
    AuxBuffers            = wgl::Global::AuxBuffers,             // WGL_AUX_BUFFERS_ARB
};

//! @brief Identifiers of properties which can be specified when creating a
//! modern OpenGL context.
enum class ContextProperty : GLenum
{
    MajorVersion = wgl::Global::ContextMajorVersion, // WGL_CONTEXT_MAJOR_VERSION_ARB
    MinorVersion = wgl::Global::ContextMinorVersion, // WGL_CONTEXT_MINOR_VERSION_ARB
    LayerPlane   = wgl::Global::ContextLayerPlane,   // WGL_CONTEXT_LAYER_PLANE_ARB
    Flags        = wgl::Global::ContextFlags,        // WGL_CONTEXT_FLAGS_ARB
    ProfileMask  = wgl::Global::ContextProfileMask,  // WGL_CONTEXT_PROFILE_MASK_ARB
};

//! @brief An object which defines properties within a specific schema.
//! @tparam TPropID The type defining the identifiers of properties to set.
//! @tparam TPropValue The data type of property values.
template<typename TPropID, typename TPropValue>
class PropertySet
{
public:
    // Public Types
    using ValueSet = std::vector<TPropValue>;
    using ValueMap = std::map<TPropID, TPropValue>;
private:
    std::map<TPropID, TPropValue> _properties;
public:
    // Construction/Destruction
    PropertySet() = default;
    ~PropertySet() = default;

    // Accessors
    //! @brief Gets the property values as an array of ID/value pairs,
    //! terminated a zero property ID.
    //! @returns A collection of ID/Value pairs all as value types.
    ValueSet getValues() const
    {
        ValueSet values;
        values.reserve((_properties.size() + 1) * 2);

        for (const auto &propPair : _properties)
        {
            values.push_back(static_cast<TPropValue>(propPair.first));
            values.push_back(propPair.second);
        }

        values.push_back(static_cast<TPropValue>(0));

        return values;
    }

    //! @brief Gets a value from the property set, using a default if it was
    //! not defined.
    //! @param[in] id The identifier of the property to look up.
    //! @param[in] defaultValue The value to return if the property was not
    //! defined.
    //! @return Ether the defined property value or defaultValue.
    TPropValue getValue(TPropID id, TPropValue defaultValue) const
    {
        auto pos = _properties.find(id);

        return (pos == _properties.end()) ? defaultValue : pos->second;
    }

    //! @brief Gets a value from the property set, converting its type. A
    //! default value will be returned if the property was not defined.
    //! @tparam T The data type to convert the property value to.
    //! @param[in] id The identifier of the property to look up.
    //! @param[in] defaultValue The value to return if the property was not
    //! defined.
    //! @return Ether the defined property value or defaultValue.
    template<typename T> T getTypedValue(TPropID id, T defaultValue) const
    {
        auto pos = _properties.find(id);

        return (pos == _properties.end()) ? defaultValue :
                                            static_cast<T>(pos->second);
    }

    //! @brief Attempts to get the value of a property, if it is defined.
    //! @param[in] id The identifier of the property to look up.
    //! @param[out] value Receives the value of the property, if defined.
    //! @retval true The property was defined and returned in value.
    //! @retval false The property was not defined, value is unmodified.
    bool tryGetValue(TPropID id, TPropValue &value) const
    {
        bool hasValue = false;

        auto pos = _properties.find(id);

        if (pos != _properties.end())
        {
            value = pos->second;
            hasValue = true;
        }

        return hasValue;
    }

    //! @brief Gets the mapping of property IDs to property values.
    const ValueMap &getMapping() const
    {
        return _properties;
    }

    // Operations
    //! @brief Defines a specified property, overwriting any previously set
    //! value with the same ID.
    //! @param[in] id The identifier of the property to set.
    //! @param[in] value The new value of the property.
    void defineProperty(TPropID id, TPropValue value)
    {
        _properties[id] = value;
    }
};

using PixelFormatIntAttribSet = PropertySet<FormatProperty, int>;
using PixelFormatFloatAttribSet = PropertySet<FormatProperty, float>;
using ContextAttribSet = PropertySet<ContextProperty, int>;

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which provides useful functions for working with OpenGL
//! on Windows.
class WGL_Tools
{
public:
    // Construction/Destruction
    WGL_Tools();

    // Accessors
    const gl::APIResolver &getResolver() const;
    const wgl::WGLAPI &getWGL_API() const;
    const wgl::ARBCreateContextAPI &getContextCreationAPI() const;
    const wgl::ARBPixelFormatAPI &getPixelFormatAPI() const;
    const wgl::AMDGPUAssociationAPI &getGPUAssocAPI() const;

    // Operations
    void prepare(HDC device, HGLRC context);
    void prepare(HWND window, HGLRC context);
    void prepare(HWND displayWindow,
                 const PIXELFORMATDESCRIPTOR &preferredFormat);
    HGLRC createInitialContext(HWND window,
                               const PixelFormatIntAttribSet &intFormatProps,
                               const PixelFormatFloatAttribSet &floatFormatProps,
                               const ContextAttribSet &contextProps);
    HGLRC createSharedContext(HWND window, HGLRC shareContext,
                              const ContextAttribSet &contextProps);
private:
    // Internal Fields
    wgl::WGLResolver _resolver;
    wgl::WGLAPI _wgl;
    wgl::ARBCreateContextAPI _createContext;
    wgl::ARBPixelFormatAPI _pixelFormat;
    wgl::AMDGPUAssociationAPI _gpuAssociation;
    bool _isPrepared;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

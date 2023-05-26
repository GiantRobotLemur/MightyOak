//! @file ObjectGL/DisplayContext.cpp
//! @brief The definition of an object representing a context in which OpenGL
//! resources exist in order to be used by render contexts.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"
#include "Ag/ObjectGL/DisplayContext.hpp"
#include "DisplayContextPrivate.hpp"
#include "RenderContextPrivate.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// DisplayContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a display context object bound to an underlying display.
//! @param[in] context A reference to the display to bind the object to.
DisplayContext::DisplayContext(const std::shared_ptr<DisplayContextPrivate> &context) :
    _context(context)
{
}

//! @brief Determines whether the object is bound to a display.
//! @retval true The object is bound to a valid display.
//! @retval false The object is in an unbound state.
bool DisplayContext::isBound() const
{
    return _context.operator bool();
}

//! @brief Gets the maximum supported version of the OpenGL API.
Ag::Version DisplayContext::getMaxSupportedVersion() const
{
    return (_context) ? _context->getMaxSupportedVersion() : Ag::Version();
}

//! @brief Creates an OpenGL context targeted at a specific drawable object.
//! @param[in] drawable A value representing a driver-specific object which
//! can be rendered to, such as a pointer to an SDL_Window object, Win32
//! Window handle, or the like.
//! @param[in] options The requirements of the context to OpenGL create.
//! @return An object wrapping the newly created OpenGL context.
RenderContext DisplayContext::createRenderer(uintptr_t drawable,
                                             const ContextOptions &options)
{
    if (_context)
    {
        return AssignableRenderContext(_context->createContext(drawable, options));
    }
    else
    {
        throw Ag::ObjectNotBoundException("DisplayContext",
                                          "createRenderer()");
    }
}

//! @brief Disposes of any OpenGL resources queued for disposal since the
//! last flush.
void DisplayContext::flushResources()
{
    if (_context)
    {
        _context->flushResources();
    }
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


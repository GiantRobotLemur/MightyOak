//! @file ObjectGL/RenderContextPrivate.cpp
//! @brief The definition of the implementation of the inner RenderContext object.
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
#include "DisplayContextPrivate.hpp"
#include "RenderContextPrivate.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// RenderContextPrivate Member Definitions
////////////////////////////////////////////////////////////////////////////////
RenderContextPrivate::RenderContextPrivate(const DisplayContextPrivateSPtr &display) :
    _display(display)
{
    if (display.operator bool() == false)
    {
        throw Ag::ArgumentException("display");
    }
}

const DisplayContextPrivateSPtr &RenderContextPrivate::getDisplay() const
{
    return _display;
}

const gl::GLAPI &RenderContextPrivate::getAPI() const
{
    return _api;
}

gl::GLAPI &RenderContextPrivate::getAPIInternal()
{
    return _api;
}

////////////////////////////////////////////////////////////////////////////////
// AssignableRenderContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a RenderContext-derived object which is bound to an
//! existing implementation.
AssignableRenderContext::AssignableRenderContext(const RenderContextPrivateSPtr &context) :
    RenderContext(context)
{
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


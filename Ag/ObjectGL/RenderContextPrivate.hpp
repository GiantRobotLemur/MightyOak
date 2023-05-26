//! @file ObjectGL/RenderContextPrivate.hpp
//! @brief The declaration of the implementation of the inner RenderContext object.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_RENDER_CONTEXT_PRIVATE_HPP__
#define __AG_OBJECT_GL_RENDER_CONTEXT_PRIVATE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include "Ag/GLAPI/CommandSets.hpp"
#include "Ag/ObjectGL/RenderContext.hpp"

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
namespace gl {
struct APIResolver;
}

namespace Ag {
namespace GL {
////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class DisplayContextPrivate;

using DisplayContextPrivateSPtr = std::shared_ptr<DisplayContextPrivate>;

//! @brief The implementation of the inner RenderContext object.
class RenderContextPrivate : public std::enable_shared_from_this<RenderContextPrivate>
{
protected:
    // Construction/Destruction
    RenderContextPrivate(const DisplayContextPrivateSPtr &display);
public:
    virtual ~RenderContextPrivate() = default;

    // Accessors
    const DisplayContextPrivateSPtr &getDisplay() const;
    const gl::GLAPI &getAPI() const;

    // Operations
    virtual void makeCurrent() =0;
    virtual void doneCurrent() =0;
    virtual void swapBuffers() =0;
protected:
    // Internal Types

    // Internal Functions
    gl::GLAPI &getAPIInternal();

private:
    // Internal Fields
    DisplayContextPrivateSPtr _display;
    gl::GLAPI _api;
};

using RenderContextPrivateSPtr = std::shared_ptr<RenderContextPrivate>;

//! @brief An internal RenderContext object which allows an inner context to
//! be bound.
class AssignableRenderContext : public RenderContext
{
public:
    AssignableRenderContext(const RenderContextPrivateSPtr &context);
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

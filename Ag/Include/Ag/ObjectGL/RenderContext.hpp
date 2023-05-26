//! @file ObjectGL/RenderContext.hpp
//! @brief The declaration of an object representing an OpenGL context which can
//! create resources and render using them.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_RENDER_CONTEXT_HPP__
#define __AG_OBJECT_GL_RENDER_CONTEXT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include "Ag/GLAPI/CommandSets.hpp"

#include "Shader.hpp"
#include "Program.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "VertexArrayObject.hpp"

namespace gl {
struct APIResolver;
}

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class RenderContextPrivate;

//! @brief An object representing an OpenGL context which can create resources
//! and render using them.
class RenderContext
{
protected:
    // Construction/Destruction
    RenderContext(const std::shared_ptr<RenderContextPrivate> &context);
public:
    RenderContext() = default;
    ~RenderContext() = default;

    // Accessors
    bool isBound() const;
    const gl::GLAPI &getAPI() const;
    const gl::APIResolver *getResolver() const;

    // Operations
    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    Shader createShader(gl::ShaderType type);
    Program createProgram();
    IndexBuffer createIndexBuffer();
    VertexBuffer createVertexBuffer();
    VertexArrayObject createVertexArray();

    void flushResources();
private:
    // Internal Functions
    const gl::GLAPI &verifyAccess(const char *fnName) const;

    // Internal Fields
    std::shared_ptr<RenderContextPrivate> _context;
};

//! @brief An RAII object which makes a RenderContext current and calls
//! doneCurrent() on destruction.
class RenderScope
{
public:
    // Construction/Destruction
    RenderScope() = delete;
    RenderScope(RenderScope &&) = delete;
    RenderScope(const RenderScope &) = delete;
    RenderScope &operator=(const RenderScope &) = delete;
    RenderScope &operator=(RenderScope &&) = delete;

    RenderScope(const RenderContext &context);
    ~RenderScope();
private:
    RenderContext _context;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

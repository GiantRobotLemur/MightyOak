//! @file ObjectGL/RenderContext.cpp
//! @brief The definition of an object representing an OpenGL context which can
//! create resources and render using them.
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
#include "Ag/ObjectGL/RenderContext.hpp"
#include "RenderContextPrivate.hpp"
#include "ResourceWrappers.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// RenderContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object bound to an inner render context.
//! @param[in] context A reference to the context to bind the object to.
RenderContext::RenderContext(const std::shared_ptr<RenderContextPrivate> &context) :
    _context(context)
{
}

//! @brief Indicates whether the object is bound to a renderer.
//! @retval true The object is bound to valid render context.
//! @retval false The object is in an unbound state.
bool RenderContext::isBound() const
{
    return _context.operator bool();
}

//! @brief Gets the OpenGL API which pertains to the current context.
const gl::GLAPI &RenderContext::getAPI() const
{
    verifyAccess("getAPI()");

    return _context->getAPI();
}

//! @brief Gets an object which can resolve API entry points and detect
//! extensions on the current display device.
const gl::APIResolver *RenderContext::getResolver() const
{
    verifyAccess("getResolver()");

    return _context->getDisplay()->getResolver();
}

//! @brief Selects the context for use on the current thread.
void RenderContext::makeCurrent()
{
    verifyAccess("makeCurrent()");

    _context->makeCurrent();
}

//! @brief De-selects any context for use on the current thread.
void RenderContext::doneCurrent()
{
    verifyAccess("doneCurrent()");

    _context->doneCurrent();
}

//! @brief Swaps the front and back buffers if in a double buffer system.
void RenderContext::swapBuffers()
{
    verifyAccess("swapBuffers()");

    _context->swapBuffers();
}

//! @brief Create a shader using the current context.
//! @param[in] type The type of shader to create.
//! @return An object wrapping the newly created shader.
Shader RenderContext::createShader(gl::ShaderType type)
{
    const auto &api = verifyAccess("createShader()");

    gl::ShaderName name = api.createShader(type);
    auto resource = std::make_shared<ShaderResource>(_context->getDisplay(),
                                                     name);

    return AssignableShader(resource);
}

//! @brief Creates a shader program using the current context.
//! @return An object wrapping the newly created program.
Program RenderContext::createProgram()
{
    const auto &api = verifyAccess("createProgram()");

    gl::ProgramName name = api.createProgram();
    auto resource = std::make_shared<ProgramResource>(_context->getDisplay(),
                                                     name);

    return AssignableProgram(resource);
}

//! @brief Creates a buffer used to hold vertex indices using the
//! current context.
//! @return An object wrapping a newly created buffer.
IndexBuffer RenderContext::createIndexBuffer()
{
    const auto &api = verifyAccess("createIndexBuffer()");

    gl::BufferName bufferName;

    // Allocate a name for the buffer.
    api.createBuffers(1, &bufferName);

    // Wrap it in a resource object.
    auto resource = std::make_shared<IndexBufferResource>(_context->getDisplay(),
                                                          bufferName);

    // Wrap the resource in a management object.
    return AssignableIndexBuffer(resource);
}

//! @brief Creates a buffer used to hold vertices using the current context.
//! @return An object wrapping a newly created buffer.
VertexBuffer RenderContext::createVertexBuffer()
{
    const auto &api = verifyAccess("createVertexBuffer()");

    gl::BufferName bufferName;

    // Allocate a name for the buffer.
    api.createBuffers(1, &bufferName);

    // Wrap it in a resource object.
    auto resource = std::make_shared<VertexBufferResource>(_context->getDisplay(),
                                                           bufferName);

    // Wrap the resource in a management object.
    return AssignableVertexBuffer(resource);
}

//! @brief Creates a Vertex Array Object in the current OpenGL context.
VertexArrayObject RenderContext::createVertexArray()
{
    const auto &api = verifyAccess("createVertexArray()");

    gl::VertexArrayName vaoName;

    // Allocate a new VAO in the current context.
    api.genVertexArrays(1, &vaoName);

    // Create an object to track the lifetime of the resource.
    auto vaoResource = std::make_shared<VAOResource>(_context->getDisplay(),
                                                     vaoName);

    // Wrap the resource in a management object.
    return AssignableVAO(vaoResource);
}

//! @brief Disposes of any OpenGL resources queued for disposal since the
//! last flush.
void RenderContext::flushResources()
{
    verifyAccess("flushResources()");

    _context->getDisplay()->flushResources();
}

//! @brief Throws an exception if the object is in an unbound state.
//! @param[in] fnName The name of the function which the program was attempting
//! to perform.
//! @returns A reference to the resolve API if no exception was thrown.
//! @throws Ag::ObjectNotBoundException If the object is not bound to a valid
//! RenderContext resource.
const gl::GLAPI &RenderContext::verifyAccess(const char *fnName) const
{
    if (!_context)
    {
        throw Ag::ObjectNotBoundException("Render Context", fnName);
    }

    return _context->getAPI();
}

////////////////////////////////////////////////////////////////////////////////
// RenderScope Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Makes a context current for the lifetime of the object.
//! @param[in] context The context to select on the current thread.
RenderScope::RenderScope(const RenderContext &context) :
    _context(context)
{
    _context.makeCurrent();
}

//! @brief Calls doneCurrent() on the context the object was constructed with.
RenderScope::~RenderScope()
{
    _context.doneCurrent();
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


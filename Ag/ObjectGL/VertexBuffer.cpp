//! @file ObjectGL/VertexBuffer.cpp
//! @brief The definition of an object holding vertices in GPU memory.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/ObjectGL/VertexBuffer.hpp"
#include "ResourceWrappers.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// VertexBuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(const std::shared_ptr<VertexBufferResource> &resource) :
    _buffer(resource)
{
}

//! @brief Gets whether the object is bound to a valid vertex buffer resource.
bool VertexBuffer::isBound() const { return static_cast<bool>(_buffer); }

//! @brief Gets the identifier of the vertex buffer resource.
gl::BufferName VertexBuffer::getName() const
{
    return _buffer ? _buffer->getName() : gl::BufferName();
}

//! @brief Gets the schema which annotates the buffer resource.
//! @return A reference to the schema or an empty schema if the object is unbound.
const VertexSchema &VertexBuffer::getSchema() const
{
    static const VertexSchema Empty;

    return _buffer ? _buffer->getSchema() : Empty;
}

//! @brief Sets the schema which annotates the buffer resource.
//! @param[in] schema The object which describes the format of the buffer.
void VertexBuffer::setSchema(const VertexSchema &schema)
{
    verifyAccess("setSchema()");

    _buffer->setSchema(schema);
}

//! @brief Binds the vertex buffer to the current OpenGL context.
void VertexBuffer::select() const
{
    const auto &api = verifyAccess("select()");

    api.bindBuffer(gl::BufferTarget::ArrayBuffer, _buffer->getName());
}

//! @brief Unbinds any vertex buffer from the current OpenGL context.
void VertexBuffer::deselect() const
{
    const auto &api = verifyAccess("deselect()");

    api.bindBuffer(gl::BufferTarget::ArrayBuffer, gl::BufferName());
}

//! @brief Binds the vertex buffer to the current context and defines
//! its format.
//! @param[in] mapping A mapping defining how logical vertex attributes
//! in the buffer map to physical vertex attributes in a shader program.
void VertexBuffer::define(const VertexAttribMapping &mapping) const
{
    const auto &api = verifyAccess("define()");

    // Bind the buffer in the vertex slot before defining its format.
    api.bindBuffer(gl::BufferTarget::ArrayBuffer, _buffer->getName());

    _buffer->getSchema().define(api, mapping);
}

//! @brief Transfers vertex data from system memory to GPU memory.
//! @param[in] vertexData The vertex data to transfer.
//! @param[in] byteCount The count of bytes in vertexData.
//! @param[in] usage A hint at the lifetime and usage of the vertex data.
void VertexBuffer::fill(const void *vertexData, size_t byteCount,
                        gl::BufferUsage usage)
{
    const auto &api = verifyAccess("fill()");

    // Bind the buffer in the vertex slot before defining its format.
    api.bindBuffer(gl::BufferTarget::ArrayBuffer, _buffer->getName());

    api.bufferData(gl::BufferTarget::ArrayBuffer,
                   static_cast<GLsizeiptr>(byteCount),
                   vertexData, usage);
}

//! @brief Verifies that the object is associated with a valid resource and
//! returns an API used to operate on it.
//! @param[in] operation The name of the operation being performed.
//! @return A reference to an OpenGL API used to manipulate the resource.
//! @throws ObjectNotBoundException If the object is not bound to a
//! valid resource.
const gl::GLAPI &VertexBuffer::verifyAccess(const char *operation) const
{
    if (_buffer)
    {
        return _buffer->getDisplay()->getAPI();
    }
    else
    {
        throw ObjectNotBoundException("VertexBuffer", operation);
    }
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


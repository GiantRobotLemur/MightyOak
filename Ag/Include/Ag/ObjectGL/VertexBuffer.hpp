//! @file Ag/ObjectGL/VertexBuffer.hpp
//! @brief The declaration of an object holding vertices in GPU memory.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_VERTEX_BUFFER_HPP__
#define __AG_OBJECT_GL_VERTEX_BUFFER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <vector>

#include "Ag/GLAPI/EnumTypes.hpp"
#include "Ag/GLAPI/Identifiers.hpp"
#include "Ag/GLAPI/CommandSets.hpp"
#include "Ag/ObjectGL/VertexSchema.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class VertexBufferResource;

//! @brief An object holding vertices in GPU memory.
class VertexBuffer
{
protected:
    // Construction/Destruction
    VertexBuffer(const std::shared_ptr<VertexBufferResource> &resource);
public:
    VertexBuffer() = default;
    ~VertexBuffer() = default;

    // Accessors
    bool isBound() const;
    gl::BufferName getName() const;
    const VertexSchema &getSchema() const;
    void setSchema(const VertexSchema &schema);

    // Operations
    void select() const;
    void deselect() const;
    void define(const VertexAttribMapping &mapping) const;
    void fill(const void *vertexData, size_t byteCount, gl::BufferUsage usage);
private:
    // Internal Functions
    const gl::GLAPI &verifyAccess(const char *operation) const;

    // Internal Fields
    std::shared_ptr<VertexBufferResource> _buffer;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

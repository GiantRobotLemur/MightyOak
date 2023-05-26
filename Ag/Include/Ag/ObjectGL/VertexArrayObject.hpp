//! @file Ag/ObjectGL/VertexArrayObject.hpp
//! @brief The declaration of an object configuring a Vertex input pipeline
//! for OpenGL rendering.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_VERTEX_ARRAY_OBJECT_HPP__
#define __AG_OBJECT_GL_VERTEX_ARRAY_OBJECT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/GLAPI/Identifiers.hpp"

namespace gl {
class GLAPI;
}

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class VAOResource;

//! @brief An object configuring a Vertex input pipeline for OpenGL rendering.
class VertexArrayObject
{
protected:
    // Construction/Destruction
    VertexArrayObject(const std::shared_ptr<VAOResource> &vao);
public:
    VertexArrayObject() = default;
    ~VertexArrayObject() = default;

    // Accessors
    bool isBound() const;
    gl::VertexArrayName getName() const;

    // Operations
    void select() const;
    void deselect() const;
private:
    // Internal Types

    // Internal Functions
    const gl::GLAPI &verifyAccess(const char *operation) const;

    // Internal Fields
    std::shared_ptr<VAOResource> _vao;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

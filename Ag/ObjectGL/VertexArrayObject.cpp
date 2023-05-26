//! @file ObjectGL/VertexArrayObject.cpp
//! @brief The definition of an object configuring a Vertex input pipeline
//! for OpenGL rendering.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/ObjectGL/VertexArrayObject.hpp"
#include "ResourceWrappers.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// VertexArrayObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to wrap a Vertex Array Object resource.
//! @param[in] vao The resource to wrap.
VertexArrayObject::VertexArrayObject(const std::shared_ptr<VAOResource> &vao) :
    _vao(vao)
{
}

//! @brief Gets whether the object is bound to a valid vertex array
//! object resource.
bool VertexArrayObject::isBound() const { return static_cast<bool>(_vao); }

//! @brief Gets the identifier of the vertex array object resource.
gl::VertexArrayName VertexArrayObject::getName() const
{
    return _vao ? _vao->getName() : gl::VertexArrayName();
}

//! @brief Binds the VAO to the current OpenGL context.
void VertexArrayObject::select() const
{
    const auto &api = verifyAccess("select()");
    
    api.bindVertexArray(_vao->getName());
}

//! @brief Unbinds any VAO from the current OpenGL context.
void VertexArrayObject::deselect() const
{
    const auto &api = verifyAccess("deselect()");

    api.bindVertexArray(gl::VertexArrayName());
}

//! @brief Verifies that the object is associated with a valid resource and
//! returns an API used to operate on it.
//! @param[in] operation The name of the operation being performed.
//! @return A reference to an OpenGL API used to manipulate the resource.
//! @throws ObjectNotBoundException If the object is not bound to a
//! valid resource
const gl::GLAPI &VertexArrayObject::verifyAccess(const char *operation) const
{
    if (_vao)
    {
        return _vao->getDisplay()->getAPI();
    }
    else
    {
        throw ObjectNotBoundException("VertexArrayObject", operation);
    }
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


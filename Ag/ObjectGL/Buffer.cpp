//! @file ObjectGL/Buffer.cpp
//! @brief The definition of a wrapper for an OpenGL buffer resource.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/ObjectGL/Buffer.hpp"
#include "ResourceWrappers.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Buffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to wrap a specified buffer resource.
//! @param[in] resource A shared reference to the resource to wrap.
Buffer::Buffer(const std::shared_ptr<BufferResource> &resource) :
    _buffer(resource)
{
}

//! @brief Determines whether the object is currently bound to a valid
//! underlying buffer resource.
bool Buffer::isBound() const { return _buffer.operator bool(); }

//! @brief Gets the identifier of the wrapped buffer resource.
gl::BufferName Buffer::getName() const
{
    return (_buffer) ? _buffer->getName() : gl::BufferName();
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


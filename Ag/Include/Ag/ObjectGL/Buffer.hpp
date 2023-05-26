//! @file ObjectGL/Buffer.hpp
//! @brief The declaration of a wrapper for an OpenGL buffer resource.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_BUFFER_HPP__
#define __AG_OBJECT_GL_BUFFER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include "Ag/GLAPI/Identifiers.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class BufferResource;

//! @brief A wrapper for an OpenGL buffer resource.
class Buffer
{
    // Construction/Destruction
protected:
    Buffer(const std::shared_ptr<BufferResource> &resource);
public:
    Buffer() = default;

    // Accessors
    bool isBound() const;
    gl::BufferName getName() const;

private:
    // Internal Fields
    std::shared_ptr<BufferResource> _buffer;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ObjectGL/Shader.hpp
//! @brief The declaration of an object wrapping an OpenGL shader resource.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_SHADER_HPP__
#define __AG_OBJECT_GL_SHADER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include "Ag/Core/String.hpp"

#include "Ag/GLAPI/Identifiers.hpp"

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace gl
{
class GLAPI;
}

namespace Ag {
namespace GL {
class ShaderResource;

//! @brief An object wrapping an OpenGL shader resource.
class Shader
{
protected:
    // Construction/Destruction
    Shader(const std::shared_ptr<ShaderResource> &resource);
public:
    Shader() = default;
    ~Shader() = default;

    // Accessors
    bool isBound() const;
    gl::ShaderType getType() const;
    gl::ShaderName getName() const;
    bool isMarkedForDeletion() const;
    bool isCompiled() const;
    Ag::String getSource() const;
    void setSource(const char *source);
    void setSource(const std::string_view &source);
    void setSource(const char *source, size_t length);
    void setSource(const Ag::String &source);
    Ag::String getCompilationLog() const;

    // Operations
    bool compile();
private:
    // Internal Functions
    const gl::GLAPI &verifyAccess(const char *operation) const;

    // Internal Fields
    std::shared_ptr<ShaderResource> _shader;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

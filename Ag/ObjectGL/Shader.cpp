//! @file ObjectGL/Shader.cpp
//! @brief The definition of an object wrapping an OpenGL shader resource.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>

#include "Ag/Core/Exception.hpp"
#include "Ag/GLAPI/Identifiers.hpp"
#include "Ag/ObjectGL/Shader.hpp"
#include "DisplayContextPrivate.hpp"
#include "ResourceWrappers.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Shader Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object bound to a shader resource.
//! @param[in] resource The resource to bind to.
Shader::Shader(const std::shared_ptr<ShaderResource> &resource) :
    _shader(resource)
{
}

//! @brief Gets whether the object is bound to a valid underlying resource.
bool Shader::isBound() const
{
    return _shader.operator bool();
}

//! @brief Gets the stage of the pipeline the shader is intended to fit into.
//! @return The shader type.
//! @throws Ag::ObjectNotBoundException If the object is not bound to
//! an underlying shader resource.
gl::ShaderType Shader::getType() const
{
    auto &api = verifyAccess("getType()");

    GLint value = 0;
    api.getShaderIV(_shader->getName(), gl::ShaderParameterName::ShaderType, &value);

    return static_cast<gl::ShaderType>(value);
}

//! @brief Gets the identifier of the underlying OpenGL shader.
gl::ShaderName Shader::getName() const
{
    return (_shader) ? _shader->getName() : gl::ShaderName();
}

//! @brief Determines if the shader is flagged for deletion.
bool Shader::isMarkedForDeletion() const
{
    auto &api = verifyAccess("isMarkedForDeletion()");

    GLint value = 0;
    api.getShaderIV(_shader->getName(),
                    gl::ShaderParameterName::DeleteStatus,
                    &value);

    return value != 0;
}

//! @brief Determines if the shader has been successfully compiled.
bool Shader::isCompiled() const
{
    auto &api = verifyAccess("isCompiled()");

    GLint value = 0;
    api.getShaderIV(_shader->getName(),
                    gl::ShaderParameterName::CompileStatus,
                    &value);

    return value != 0;
}

//! @brief Gets the source code of the shader component.
Ag::String Shader::getSource() const
{
    auto &api = verifyAccess("getSource()");

    // Get the length of the source code.
    GLint length = 0;
    api.getShaderIV(_shader->getName(),
                    gl::ShaderParameterName::ShaderSourceLength,
                    &length);

    if (length > 0)
    {
        // Create a  temporary buffer to hold the source code.
        size_t size = static_cast<size_t>(length) + 1;

        std::vector<char> buffer;
        GLsizei actualLength = 0;

        // Get the source code.
        buffer.resize(size, '\0');
        api.getShaderSource(_shader->getName(), static_cast<GLsizei>(size),
                            &actualLength, buffer.data());

        if (actualLength > 0)
        {
            // Package up the text as an immutable string,
            return Ag::String(buffer.data(), static_cast<size_t>(actualLength));
        }
    }

    return Ag::String::Empty;
}

//! @brief Associates source code with the shader program stage.
//! @param[in] source A null-terminated UTF-8 encoded string containing the
//! source code of the shader.
void Shader::setSource(const char *source)
{
    if (source == nullptr)
    {
        throw Ag::ArgumentException("source");
    }

    size_t length = std::strlen(source);
    setSource(source, length);
}

//! @brief Associates source code with the shader program stage.
//! @param[in] source A bounded UTF-8 encoded string containing the
//! source code of the shader.
void Shader::setSource(const std::string_view &source)
{
    if (source.empty())
    {
        throw Ag::ArgumentException("source");
    }

    setSource(source.data(), source.length());
}

//! @brief Associates source code with the shader program stage.
//! @param[in] source A bounded UTF-8 encoded string containing the source code
//! of the shader.
//! @param[in] length The count of bytes in source, excluding any terminating
//! null characters.
void Shader::setSource(const char *source, size_t length)
{
    if (source == nullptr)
    {
        throw Ag::ArgumentException("source");
    }
    else if ((length < 1) || (length > INT_MAX))
    {
        throw Ag::ArgumentException("length");
    }

    auto &api = verifyAccess("setSource()");

    GLint size = static_cast<GLint>(length);

    api.shaderSource(_shader->getName(), 1,
                     reinterpret_cast<const GLchar *const *>(&source),
                     &size);
}

//! @brief Associates source code with the shader program stage.
//! @param[in] source A UTF-8 encoded string containing the source code
//! of the shader.
void Shader::setSource(const Ag::String &source)
{
    if (source.isEmpty())
    {
        throw Ag::ArgumentException("source");
    }

    setSource(source.getUtf8Bytes(), source.getUtf8Length());
}

//! @brief Gets the log produced the last time the shader was compiled.
Ag::String Shader::getCompilationLog() const
{
    auto &api = verifyAccess("isCompiled()");

    // Get the length of the log text.
    GLint length = 0;
    api.getShaderIV(_shader->getName(),
                    gl::ShaderParameterName::InfoLogLength,
                    &length);

    if (length > 0)
    {
        // Create a temporary buffer to hold the log data.
        size_t size = static_cast<size_t>(length) + 1;

        std::vector<char> buffer;
        GLsizei actualLength = 0;

        // Get the log data.
        buffer.resize(size, '\0');
        api.getShaderInfoLog(_shader->getName(), static_cast<GLsizei>(size),
                             &actualLength,buffer.data());

        if (actualLength > 0)
        {
            // Return as an immutable string.
            return Ag::String(buffer.data(), static_cast<size_t>(actualLength));
        }
    }

    return Ag::String::Empty;
}

//! @brief Compiles the shader component.
//! @return A boolean value indicating whether compilation was successful.
bool Shader::compile()
{
    auto &api = verifyAccess("compile()");

    api.compileShader(_shader->getName());

    GLint result;
    api.getShaderIV(_shader->getName(),
                    gl::ShaderParameterName::CompileStatus, &result);

    return (result != 0);
}

//! @brief Verifies that the object is associated with a valid resource and
//! returns an API used to operate on it.
//! @param[in] operation The name of the operation being performed.
//! @return A reference to an OpenGL API used to manipulate the resource.
//! @throws Ag::ObjectNotBoundException If the object is not bound to a
//! valid resource.
const gl::GLAPI &Shader::verifyAccess(const char *operation) const
{
    if (_shader)
    {
        return _shader->getDisplay()->getAPI();
    }
    else
    {
        throw Ag::ObjectNotBoundException("Shader", operation);
    }
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


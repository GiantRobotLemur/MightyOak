//! @file ObjectGL/Program.hpp
//! @brief The declaration of an object wrapping an OpenGL program resource.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_PROGRAM_HPP__
#define __AG_OBJECT_GL_PROGRAM_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <vector>

#include "Ag/Core/LinearSortedMap.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/GLAPI/EnumTypes.hpp"

namespace gl
{
class GLAPI;
}

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ProgramResource;
class Shader;
class VertexSchema;

//! @brief Describes an attribute within a linked shader program.
struct ProgramAttribInfo
{
    //! @brief The name of the attribute.
    String Name;

    //! @brief The data type of the attribute.
    gl::AttributeType DataType;

    //! @brief The size of the attribute, in bytes?
    GLint Size;

    //! @brief The location of the attribute to link a vertex buffer to.
    GLint Location;
};

using ProgramAttribCollection = std::vector<ProgramAttribInfo>;

//! @brief An alias for an optimised map of the index of a VertexAttribute
//! in a schema to its index as referenced in a vertex shader program.
using VertexAttribMapping = Ag::LinearSortedMap<size_t, uint32_t>;

//! @brief Describes an uniform within a linked shader program.
struct ProgramUniformInfo
{
    //! @brief The name of the uniform.
    Ag::String Name;

    //! @brief The data type of the uniform.
    gl::UniformType DataType;

    //! @brief The size of the uniform, in bytes?
    GLint Size;

    //! @brief The location of the uniform to use to associate
    //! values with the uniform.
    GLint Location;
};

using ProgramUniformCollection = std::vector<ProgramUniformInfo>;

//! @brief An object wrapping a compiled OpenGL program resource.
class Program
{
protected:
   // Construction/Destruction
    Program(const std::shared_ptr<ProgramResource> &resource);
public:
    Program() = default;
    ~Program() = default;

    // Accessors
    bool isBound() const;
    gl::ProgramName getName() const;
    size_t getActiveAttribCount() const;
    void getActiveAttrib(size_t index, ProgramAttribInfo &info) const;
    ProgramAttribCollection getActiveAttribs() const;
    size_t getActiveUniformCount() const;
    void getActiveUniform(size_t index, ProgramUniformInfo &info) const;
    ProgramUniformCollection getActiveUniforms() const;
    String getInfoLog() const;

    // Operations
    bool link();
    void select();
    void deselect();
    void attachShader(const Shader &shader);
    void detachShader(const Shader &shader);
    VertexAttribMapping createAttribMapping(const VertexSchema &schema) const;
private:
    // Internal Functions
    const gl::GLAPI &verifyAccess(const char *operation) const;

    // Internal Fields
    std::shared_ptr<ProgramResource> _program;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

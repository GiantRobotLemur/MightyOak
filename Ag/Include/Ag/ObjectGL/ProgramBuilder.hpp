//! @file ObjectGL/ProgramBuilder.hpp
//! @brief The declaration of a tool to help construct and compile OpenGL
//! shader programs.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_PROGRAM_BUILDER_HPP__
#define __AG_OBJECT_GL_PROGRAM_BUILDER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"
#include "Ag/GLAPI/CommandSets.hpp"
#include "Ag/ObjectGL/RenderContext.hpp"
#include "Program.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ShaderCompilationException : public Exception
{
public:
    static const char *Domain;

    ShaderCompilationException(gl::ShaderType type, const std::string_view &errors);
};

class ProgramLinkException : public Exception
{
public:
    static const char *Domain;

    ProgramLinkException(const std::string_view &errors);
};

//! @brief A tool to help construct and compile OpenGL shader programs.
class ProgramBuilder
{
public:
    // Construction/Destruction
    ProgramBuilder() = delete;
    ProgramBuilder(ProgramBuilder &&) = delete;
    ProgramBuilder(const ProgramBuilder &) = delete;
    ProgramBuilder &operator=(const ProgramBuilder &) = delete;
    ProgramBuilder &operator=(ProgramBuilder &&) = delete;

    ProgramBuilder(const RenderContext &context);

    // Operations
    size_t addShader(gl::ShaderType type,
                     const std::string_view &sourceCode);

    Program create();
    bool tryCreate(Program &program, string_ref_t &errors);
private:
    // Internal Types
    using ShaderResourceCollection = std::vector<Shader>;

    // Internal Fields
    RenderContext _parentContext;
    ShaderResourceCollection _shaders;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

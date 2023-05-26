//! @file ObjectGL/ProgramBuilder.cpp
//! @brief The definition of a tool to help construct and compile OpenGL
//! shader programs.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/ObjectGL/ProgramBuilder.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// ShaderCompilationException Member Definitions
////////////////////////////////////////////////////////////////////////////////
const char *ShaderCompilationException::Domain = "ShaderCompilationException";

//! @brief An exception thrown in response to the failed compilation of an
//! OpenGL shader module.
//! @param[in] type The type of shader which failed to compile.
//! @param[in] errors The compilation log.
ShaderCompilationException::ShaderCompilationException(gl::ShaderType type,
                                                       const std::string_view &errors)
{
    std::string message("An OpenGL ");

    switch (type)
    {
    case gl::ShaderType::FragmentShader: message.append("fragment "); break;
    case gl::ShaderType::VertexShader: message.append("vertex "); break;
    case gl::ShaderType::GeometryShader: message.append("geometry "); break;
    case gl::ShaderType::TessEvaluationShader: message.append("tessellator evaluation "); break;
    case gl::ShaderType::TessControlShader: message.append("tessellator control "); break;
    case gl::ShaderType::ComputeShader: message.append("compute "); break;
    }

    message.append("shader failed to compile.");

    initialise(Domain, message, errors, 0);
}

////////////////////////////////////////////////////////////////////////////////
// ProgramLinkException Member Definitions
////////////////////////////////////////////////////////////////////////////////
const char *ProgramLinkException::Domain = "ProgramLinkException";

//! @brief An exception thrown when an OpenGL program failed to link.
//! @param[in] errors The program log.
ProgramLinkException::ProgramLinkException(const std::string_view &errors)
{
    initialise(Domain, "An OpenGL shader program failed to link.",
               errors, 0);
}

////////////////////////////////////////////////////////////////////////////////
// ProgramBuilder Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which can build OpenGL program resources.
//! @param[in] context The render context in which the program should be created.
ProgramBuilder::ProgramBuilder(const RenderContext &context) :
    _parentContext(context)
{
}

//! @brief Adds a shader component to the program being accumulated.
//! @param[in] type The type of shader to compile and add.
//! @param[in] sourceCode The source code of the shader to add.
//! @return The 0-based index of the shader in the builder.
size_t ProgramBuilder::addShader(gl::ShaderType type,
                                 const std::string_view &sourceCode)
{
    Shader shader = _parentContext.createShader(type);
    shader.setSource(sourceCode);

    if (shader.compile() == false)
    {
        throw ShaderCompilationException(type,
                                         shader.getCompilationLog().toUtf8View());
    }

    size_t index = _shaders.size();
    _shaders.push_back(shader);

    return index;
}

//! @brief Link a program from the accumulated compiled shaders
//! @return The linked program resource.
//! @throws Exception if the link step fails.
Program ProgramBuilder::create()
{
    Program program = _parentContext.createProgram();

    for (const Shader &shader : _shaders)
    {
        program.attachShader(shader);
    }

    if (program.link() == false)
    {
        throw ProgramLinkException(program.getInfoLog().toUtf8View());
    }

    // Now that the program is linked, it is safe to detach the shaders so
    // that the can be deleted naturally.
    for (const Shader &shader : _shaders)
    {
        program.detachShader(shader);
    }

    return program;
}

//! @brief Attempts to link the accumulated compiled shaders into a complete
//! shader program.
//! @param[out] program Receives the program if the link was successful.
//! @param[out] errors Receives text describing why the link failed.
//! @retval true The link was successful and the program returned.
//! @retval false The link failed and errors describes why.
bool ProgramBuilder::tryCreate(Program &program, string_ref_t &errors)
{
    bool isOK = false;
    Program theProgram = _parentContext.createProgram();

    for (const Shader &shader : _shaders)
    {
        theProgram.attachShader(shader);
    }

    if (program.link())
    {
        // Now that the program is linked (or not), it is safe to detach the
        // shaders so that the can be deleted naturally.
        for (const Shader &shader : _shaders)
        {
            theProgram.detachShader(shader);
        }

        program = theProgram;
        errors = String::Empty;
        isOK = true;
    }
    else
    {
        // Return the errors and allow the program resource to be disposed
        // of naturally.
        errors = theProgram.getInfoLog();
    }

    return isOK;
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


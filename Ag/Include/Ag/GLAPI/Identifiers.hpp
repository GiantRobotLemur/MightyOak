//! @file Ag/GLAPI/Identifiers.hpp
//! @brief A file generated from the OpenGL API schema definition to create
//! C++ 17 language bindings, specifically namespaces for ubiquitous numeric
//! resource identifiers.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __51910DFB_44D6_40BF_B02F_1B39809379E8_INCLUDED__
#define __51910DFB_44D6_40BF_B02F_1B39809379E8_INCLUDED__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "APIs.hpp"

namespace gl {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
// A structure aliasing GLuint values which represent textures.
struct TextureName
{
    GLuint ID;

    TextureName();
    TextureName(GLuint id);
};

// A structure aliasing GLuint values which represent querys.
struct QueryName
{
    GLuint ID;

    QueryName();
    QueryName(GLuint id);
};

// A structure aliasing GLuint values which represent buffers.
struct BufferName
{
    GLuint ID;

    BufferName();
    BufferName(GLuint id);
};

// A structure aliasing GLuint values which represent programs.
struct ProgramName
{
    GLuint ID;

    ProgramName();
    ProgramName(GLuint id);
};

// A structure aliasing GLuint values which represent shaders.
struct ShaderName
{
    GLuint ID;

    ShaderName();
    ShaderName(GLuint id);
};

// A structure aliasing GLuint values which represent frame buffers.
struct FrameBufferName
{
    GLuint ID;

    FrameBufferName();
    FrameBufferName(GLuint id);
};

// A structure aliasing GLuint values which represent render buffers.
struct RenderBufferName
{
    GLuint ID;

    RenderBufferName();
    RenderBufferName(GLuint id);
};

// A structure aliasing GLuint values which represent vertex arrays.
struct VertexArrayName
{
    GLuint ID;

    VertexArrayName();
    VertexArrayName(GLuint id);
};

// A structure aliasing GLsync values which represent syncs.
struct SyncName
{
    GLsync ID;

    SyncName();
    SyncName(GLsync id);
};

// A structure aliasing GLuint values which represent samplers.
struct SamplerName
{
    GLuint ID;

    SamplerName();
    SamplerName(GLuint id);
};

// A structure aliasing GLuint values which represent transform feedbacks.
struct TransformFeedbackName
{
    GLuint ID;

    TransformFeedbackName();
    TransformFeedbackName(GLuint id);
};

// A structure aliasing GLuint values which represent program pipelines.
struct ProgramPipelineName
{
    GLuint ID;

    ProgramPipelineName();
    ProgramPipelineName(GLuint id);
};

} // namespace gl

#endif // ifndef __51910DFB_44D6_40BF_B02F_1B39809379E8_INCLUDED__

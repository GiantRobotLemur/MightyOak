//! @file GLAPI/APIs.cpp
//! @brief The definition of structures defining the C-language dynamic binding
//! to the OpenGL API generated from the API registry.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Required Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <utility>
#include "Ag/GLAPI/APIs.hpp"

namespace gl {

namespace {

// A value type representing and entry point to resolve
using EntryOffset = std::pair<size_t, const char *>;

// A base class for functors which access API function pointers.
struct PerEntryPoint
{
private:
    void *_basePtr;

protected:
    // Gets a reference to the function pointer described by the entry.
    void *&getEntryPoint(const EntryOffset &entry)
    {
        return *reinterpret_cast<void **>(reinterpret_cast<uint8_t *>(_basePtr) +
                                          entry.first);
    }

public:
    // Constructs an object to access function pointers in a specified structure.
    PerEntryPoint(void *basePtr) :
        _basePtr(basePtr)
    {
    }
};

// A functor which sets function pointers to null.
struct ClearEntryPoint : public PerEntryPoint
{
    ClearEntryPoint(void *basePtr) :
        PerEntryPoint(basePtr)
    {
    }

    void operator()(const EntryOffset &entry)
    {
        getEntryPoint(entry) = nullptr;
    }
};

// A functor which resolves function pointers at runtime.
struct ResolveEntryPoint : public PerEntryPoint
{
private:
    const APIResolver *_resolver;
public:
    ResolveEntryPoint(void *basePtr, const APIResolver *resolver) :
        PerEntryPoint(basePtr),
        _resolver(resolver)
    {
    }

    void operator()(const EntryOffset &entry)
    {
        getEntryPoint(entry) = _resolver->resolveEntryPoint(entry.second);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
static const EntryOffset __GLEntryPoints[] = {
    { offsetof(GL, glBlendFunc), "glBlendFunc" },
    { offsetof(GL, glClear), "glClear" },
    { offsetof(GL, glClearColor), "glClearColor" },
    { offsetof(GL, glClearDepth), "glClearDepth" },
    { offsetof(GL, glClearStencil), "glClearStencil" },
    { offsetof(GL, glColorMask), "glColorMask" },
    { offsetof(GL, glCullFace), "glCullFace" },
    { offsetof(GL, glDepthFunc), "glDepthFunc" },
    { offsetof(GL, glDepthMask), "glDepthMask" },
    { offsetof(GL, glDepthRange), "glDepthRange" },
    { offsetof(GL, glDisable), "glDisable" },
    { offsetof(GL, glDrawBuffer), "glDrawBuffer" },
    { offsetof(GL, glEnable), "glEnable" },
    { offsetof(GL, glFinish), "glFinish" },
    { offsetof(GL, glFlush), "glFlush" },
    { offsetof(GL, glFrontFace), "glFrontFace" },
    { offsetof(GL, glGetBooleanv), "glGetBooleanv" },
    { offsetof(GL, glGetDoublev), "glGetDoublev" },
    { offsetof(GL, glGetError), "glGetError" },
    { offsetof(GL, glGetFloatv), "glGetFloatv" },
    { offsetof(GL, glGetIntegerv), "glGetIntegerv" },
    { offsetof(GL, glGetString), "glGetString" },
    { offsetof(GL, glGetTexImage), "glGetTexImage" },
    { offsetof(GL, glGetTexLevelParameterfv), "glGetTexLevelParameterfv" },
    { offsetof(GL, glGetTexLevelParameteriv), "glGetTexLevelParameteriv" },
    { offsetof(GL, glGetTexParameterfv), "glGetTexParameterfv" },
    { offsetof(GL, glGetTexParameteriv), "glGetTexParameteriv" },
    { offsetof(GL, glHint), "glHint" },
    { offsetof(GL, glIsEnabled), "glIsEnabled" },
    { offsetof(GL, glLineWidth), "glLineWidth" },
    { offsetof(GL, glLogicOp), "glLogicOp" },
    { offsetof(GL, glPixelStoref), "glPixelStoref" },
    { offsetof(GL, glPixelStorei), "glPixelStorei" },
    { offsetof(GL, glPointSize), "glPointSize" },
    { offsetof(GL, glPolygonMode), "glPolygonMode" },
    { offsetof(GL, glReadBuffer), "glReadBuffer" },
    { offsetof(GL, glReadPixels), "glReadPixels" },
    { offsetof(GL, glScissor), "glScissor" },
    { offsetof(GL, glStencilFunc), "glStencilFunc" },
    { offsetof(GL, glStencilMask), "glStencilMask" },
    { offsetof(GL, glStencilOp), "glStencilOp" },
    { offsetof(GL, glTexImage1D), "glTexImage1D" },
    { offsetof(GL, glTexImage2D), "glTexImage2D" },
    { offsetof(GL, glTexParameterf), "glTexParameterf" },
    { offsetof(GL, glTexParameterfv), "glTexParameterfv" },
    { offsetof(GL, glTexParameteri), "glTexParameteri" },
    { offsetof(GL, glTexParameteriv), "glTexParameteriv" },
    { offsetof(GL, glViewport), "glViewport" },
    { offsetof(GL, glBindTexture), "glBindTexture" },
    { offsetof(GL, glCopyTexImage1D), "glCopyTexImage1D" },
    { offsetof(GL, glCopyTexImage2D), "glCopyTexImage2D" },
    { offsetof(GL, glCopyTexSubImage1D), "glCopyTexSubImage1D" },
    { offsetof(GL, glCopyTexSubImage2D), "glCopyTexSubImage2D" },
    { offsetof(GL, glDeleteTextures), "glDeleteTextures" },
    { offsetof(GL, glDrawArrays), "glDrawArrays" },
    { offsetof(GL, glDrawElements), "glDrawElements" },
    { offsetof(GL, glGenTextures), "glGenTextures" },
    { offsetof(GL, glIsTexture), "glIsTexture" },
    { offsetof(GL, glPolygonOffset), "glPolygonOffset" },
    { offsetof(GL, glTexSubImage1D), "glTexSubImage1D" },
    { offsetof(GL, glTexSubImage2D), "glTexSubImage2D" },
    { offsetof(GL, glCopyTexSubImage3D), "glCopyTexSubImage3D" },
    { offsetof(GL, glDrawRangeElements), "glDrawRangeElements" },
    { offsetof(GL, glTexImage3D), "glTexImage3D" },
    { offsetof(GL, glTexSubImage3D), "glTexSubImage3D" },
    { offsetof(GL, glActiveTexture), "glActiveTexture" },
    { offsetof(GL, glCompressedTexImage1D), "glCompressedTexImage1D" },
    { offsetof(GL, glCompressedTexImage2D), "glCompressedTexImage2D" },
    { offsetof(GL, glCompressedTexImage3D), "glCompressedTexImage3D" },
    { offsetof(GL, glCompressedTexSubImage1D), "glCompressedTexSubImage1D" },
    { offsetof(GL, glCompressedTexSubImage2D), "glCompressedTexSubImage2D" },
    { offsetof(GL, glCompressedTexSubImage3D), "glCompressedTexSubImage3D" },
    { offsetof(GL, glGetCompressedTexImage), "glGetCompressedTexImage" },
    { offsetof(GL, glSampleCoverage), "glSampleCoverage" },
    { offsetof(GL, glBlendColor), "glBlendColor" },
    { offsetof(GL, glBlendEquation), "glBlendEquation" },
    { offsetof(GL, glBlendFuncSeparate), "glBlendFuncSeparate" },
    { offsetof(GL, glMultiDrawArrays), "glMultiDrawArrays" },
    { offsetof(GL, glMultiDrawElements), "glMultiDrawElements" },
    { offsetof(GL, glPointParameterf), "glPointParameterf" },
    { offsetof(GL, glPointParameterfv), "glPointParameterfv" },
    { offsetof(GL, glPointParameteri), "glPointParameteri" },
    { offsetof(GL, glPointParameteriv), "glPointParameteriv" },
    { offsetof(GL, glBeginQuery), "glBeginQuery" },
    { offsetof(GL, glBindBuffer), "glBindBuffer" },
    { offsetof(GL, glBufferData), "glBufferData" },
    { offsetof(GL, glBufferSubData), "glBufferSubData" },
    { offsetof(GL, glDeleteBuffers), "glDeleteBuffers" },
    { offsetof(GL, glDeleteQueries), "glDeleteQueries" },
    { offsetof(GL, glEndQuery), "glEndQuery" },
    { offsetof(GL, glGenBuffers), "glGenBuffers" },
    { offsetof(GL, glGenQueries), "glGenQueries" },
    { offsetof(GL, glGetBufferParameteriv), "glGetBufferParameteriv" },
    { offsetof(GL, glGetBufferPointerv), "glGetBufferPointerv" },
    { offsetof(GL, glGetBufferSubData), "glGetBufferSubData" },
    { offsetof(GL, glGetQueryObjectiv), "glGetQueryObjectiv" },
    { offsetof(GL, glGetQueryObjectuiv), "glGetQueryObjectuiv" },
    { offsetof(GL, glGetQueryiv), "glGetQueryiv" },
    { offsetof(GL, glIsBuffer), "glIsBuffer" },
    { offsetof(GL, glIsQuery), "glIsQuery" },
    { offsetof(GL, glMapBuffer), "glMapBuffer" },
    { offsetof(GL, glUnmapBuffer), "glUnmapBuffer" },
    { offsetof(GL, glAttachShader), "glAttachShader" },
    { offsetof(GL, glBindAttribLocation), "glBindAttribLocation" },
    { offsetof(GL, glBlendEquationSeparate), "glBlendEquationSeparate" },
    { offsetof(GL, glCompileShader), "glCompileShader" },
    { offsetof(GL, glCreateProgram), "glCreateProgram" },
    { offsetof(GL, glCreateShader), "glCreateShader" },
    { offsetof(GL, glDeleteProgram), "glDeleteProgram" },
    { offsetof(GL, glDeleteShader), "glDeleteShader" },
    { offsetof(GL, glDetachShader), "glDetachShader" },
    { offsetof(GL, glDisableVertexAttribArray), "glDisableVertexAttribArray" },
    { offsetof(GL, glDrawBuffers), "glDrawBuffers" },
    { offsetof(GL, glEnableVertexAttribArray), "glEnableVertexAttribArray" },
    { offsetof(GL, glGetActiveAttrib), "glGetActiveAttrib" },
    { offsetof(GL, glGetActiveUniform), "glGetActiveUniform" },
    { offsetof(GL, glGetAttachedShaders), "glGetAttachedShaders" },
    { offsetof(GL, glGetAttribLocation), "glGetAttribLocation" },
    { offsetof(GL, glGetProgramInfoLog), "glGetProgramInfoLog" },
    { offsetof(GL, glGetProgramiv), "glGetProgramiv" },
    { offsetof(GL, glGetShaderInfoLog), "glGetShaderInfoLog" },
    { offsetof(GL, glGetShaderSource), "glGetShaderSource" },
    { offsetof(GL, glGetShaderiv), "glGetShaderiv" },
    { offsetof(GL, glGetUniformLocation), "glGetUniformLocation" },
    { offsetof(GL, glGetUniformfv), "glGetUniformfv" },
    { offsetof(GL, glGetUniformiv), "glGetUniformiv" },
    { offsetof(GL, glGetVertexAttribPointerv), "glGetVertexAttribPointerv" },
    { offsetof(GL, glGetVertexAttribdv), "glGetVertexAttribdv" },
    { offsetof(GL, glGetVertexAttribfv), "glGetVertexAttribfv" },
    { offsetof(GL, glGetVertexAttribiv), "glGetVertexAttribiv" },
    { offsetof(GL, glIsProgram), "glIsProgram" },
    { offsetof(GL, glIsShader), "glIsShader" },
    { offsetof(GL, glLinkProgram), "glLinkProgram" },
    { offsetof(GL, glShaderSource), "glShaderSource" },
    { offsetof(GL, glStencilFuncSeparate), "glStencilFuncSeparate" },
    { offsetof(GL, glStencilMaskSeparate), "glStencilMaskSeparate" },
    { offsetof(GL, glStencilOpSeparate), "glStencilOpSeparate" },
    { offsetof(GL, glUniform1f), "glUniform1f" },
    { offsetof(GL, glUniform1fv), "glUniform1fv" },
    { offsetof(GL, glUniform1i), "glUniform1i" },
    { offsetof(GL, glUniform1iv), "glUniform1iv" },
    { offsetof(GL, glUniform2f), "glUniform2f" },
    { offsetof(GL, glUniform2fv), "glUniform2fv" },
    { offsetof(GL, glUniform2i), "glUniform2i" },
    { offsetof(GL, glUniform2iv), "glUniform2iv" },
    { offsetof(GL, glUniform3f), "glUniform3f" },
    { offsetof(GL, glUniform3fv), "glUniform3fv" },
    { offsetof(GL, glUniform3i), "glUniform3i" },
    { offsetof(GL, glUniform3iv), "glUniform3iv" },
    { offsetof(GL, glUniform4f), "glUniform4f" },
    { offsetof(GL, glUniform4fv), "glUniform4fv" },
    { offsetof(GL, glUniform4i), "glUniform4i" },
    { offsetof(GL, glUniform4iv), "glUniform4iv" },
    { offsetof(GL, glUniformMatrix2fv), "glUniformMatrix2fv" },
    { offsetof(GL, glUniformMatrix3fv), "glUniformMatrix3fv" },
    { offsetof(GL, glUniformMatrix4fv), "glUniformMatrix4fv" },
    { offsetof(GL, glUseProgram), "glUseProgram" },
    { offsetof(GL, glValidateProgram), "glValidateProgram" },
    { offsetof(GL, glVertexAttrib1d), "glVertexAttrib1d" },
    { offsetof(GL, glVertexAttrib1dv), "glVertexAttrib1dv" },
    { offsetof(GL, glVertexAttrib1f), "glVertexAttrib1f" },
    { offsetof(GL, glVertexAttrib1fv), "glVertexAttrib1fv" },
    { offsetof(GL, glVertexAttrib1s), "glVertexAttrib1s" },
    { offsetof(GL, glVertexAttrib1sv), "glVertexAttrib1sv" },
    { offsetof(GL, glVertexAttrib2d), "glVertexAttrib2d" },
    { offsetof(GL, glVertexAttrib2dv), "glVertexAttrib2dv" },
    { offsetof(GL, glVertexAttrib2f), "glVertexAttrib2f" },
    { offsetof(GL, glVertexAttrib2fv), "glVertexAttrib2fv" },
    { offsetof(GL, glVertexAttrib2s), "glVertexAttrib2s" },
    { offsetof(GL, glVertexAttrib2sv), "glVertexAttrib2sv" },
    { offsetof(GL, glVertexAttrib3d), "glVertexAttrib3d" },
    { offsetof(GL, glVertexAttrib3dv), "glVertexAttrib3dv" },
    { offsetof(GL, glVertexAttrib3f), "glVertexAttrib3f" },
    { offsetof(GL, glVertexAttrib3fv), "glVertexAttrib3fv" },
    { offsetof(GL, glVertexAttrib3s), "glVertexAttrib3s" },
    { offsetof(GL, glVertexAttrib3sv), "glVertexAttrib3sv" },
    { offsetof(GL, glVertexAttrib4Nbv), "glVertexAttrib4Nbv" },
    { offsetof(GL, glVertexAttrib4Niv), "glVertexAttrib4Niv" },
    { offsetof(GL, glVertexAttrib4Nsv), "glVertexAttrib4Nsv" },
    { offsetof(GL, glVertexAttrib4Nub), "glVertexAttrib4Nub" },
    { offsetof(GL, glVertexAttrib4Nubv), "glVertexAttrib4Nubv" },
    { offsetof(GL, glVertexAttrib4Nuiv), "glVertexAttrib4Nuiv" },
    { offsetof(GL, glVertexAttrib4Nusv), "glVertexAttrib4Nusv" },
    { offsetof(GL, glVertexAttrib4bv), "glVertexAttrib4bv" },
    { offsetof(GL, glVertexAttrib4d), "glVertexAttrib4d" },
    { offsetof(GL, glVertexAttrib4dv), "glVertexAttrib4dv" },
    { offsetof(GL, glVertexAttrib4f), "glVertexAttrib4f" },
    { offsetof(GL, glVertexAttrib4fv), "glVertexAttrib4fv" },
    { offsetof(GL, glVertexAttrib4iv), "glVertexAttrib4iv" },
    { offsetof(GL, glVertexAttrib4s), "glVertexAttrib4s" },
    { offsetof(GL, glVertexAttrib4sv), "glVertexAttrib4sv" },
    { offsetof(GL, glVertexAttrib4ubv), "glVertexAttrib4ubv" },
    { offsetof(GL, glVertexAttrib4uiv), "glVertexAttrib4uiv" },
    { offsetof(GL, glVertexAttrib4usv), "glVertexAttrib4usv" },
    { offsetof(GL, glVertexAttribPointer), "glVertexAttribPointer" },
    { offsetof(GL, glUniformMatrix2x3fv), "glUniformMatrix2x3fv" },
    { offsetof(GL, glUniformMatrix2x4fv), "glUniformMatrix2x4fv" },
    { offsetof(GL, glUniformMatrix3x2fv), "glUniformMatrix3x2fv" },
    { offsetof(GL, glUniformMatrix3x4fv), "glUniformMatrix3x4fv" },
    { offsetof(GL, glUniformMatrix4x2fv), "glUniformMatrix4x2fv" },
    { offsetof(GL, glUniformMatrix4x3fv), "glUniformMatrix4x3fv" },
    { offsetof(GL, glBeginConditionalRender), "glBeginConditionalRender" },
    { offsetof(GL, glBeginTransformFeedback), "glBeginTransformFeedback" },
    { offsetof(GL, glBindBufferBase), "glBindBufferBase" },
    { offsetof(GL, glBindBufferRange), "glBindBufferRange" },
    { offsetof(GL, glBindFragDataLocation), "glBindFragDataLocation" },
    { offsetof(GL, glBindFramebuffer), "glBindFramebuffer" },
    { offsetof(GL, glBindRenderbuffer), "glBindRenderbuffer" },
    { offsetof(GL, glBindVertexArray), "glBindVertexArray" },
    { offsetof(GL, glBlitFramebuffer), "glBlitFramebuffer" },
    { offsetof(GL, glCheckFramebufferStatus), "glCheckFramebufferStatus" },
    { offsetof(GL, glClampColor), "glClampColor" },
    { offsetof(GL, glClearBufferfi), "glClearBufferfi" },
    { offsetof(GL, glClearBufferfv), "glClearBufferfv" },
    { offsetof(GL, glClearBufferiv), "glClearBufferiv" },
    { offsetof(GL, glClearBufferuiv), "glClearBufferuiv" },
    { offsetof(GL, glColorMaski), "glColorMaski" },
    { offsetof(GL, glDeleteFramebuffers), "glDeleteFramebuffers" },
    { offsetof(GL, glDeleteRenderbuffers), "glDeleteRenderbuffers" },
    { offsetof(GL, glDeleteVertexArrays), "glDeleteVertexArrays" },
    { offsetof(GL, glDisablei), "glDisablei" },
    { offsetof(GL, glEnablei), "glEnablei" },
    { offsetof(GL, glEndConditionalRender), "glEndConditionalRender" },
    { offsetof(GL, glEndTransformFeedback), "glEndTransformFeedback" },
    { offsetof(GL, glFlushMappedBufferRange), "glFlushMappedBufferRange" },
    { offsetof(GL, glFramebufferRenderbuffer), "glFramebufferRenderbuffer" },
    { offsetof(GL, glFramebufferTexture1D), "glFramebufferTexture1D" },
    { offsetof(GL, glFramebufferTexture2D), "glFramebufferTexture2D" },
    { offsetof(GL, glFramebufferTexture3D), "glFramebufferTexture3D" },
    { offsetof(GL, glFramebufferTextureLayer), "glFramebufferTextureLayer" },
    { offsetof(GL, glGenFramebuffers), "glGenFramebuffers" },
    { offsetof(GL, glGenRenderbuffers), "glGenRenderbuffers" },
    { offsetof(GL, glGenVertexArrays), "glGenVertexArrays" },
    { offsetof(GL, glGenerateMipmap), "glGenerateMipmap" },
    { offsetof(GL, glGetBooleani_v), "glGetBooleani_v" },
    { offsetof(GL, glGetFragDataLocation), "glGetFragDataLocation" },
    { offsetof(GL, glGetFramebufferAttachmentParameteriv), "glGetFramebufferAttachmentParameteriv" },
    { offsetof(GL, glGetIntegeri_v), "glGetIntegeri_v" },
    { offsetof(GL, glGetRenderbufferParameteriv), "glGetRenderbufferParameteriv" },
    { offsetof(GL, glGetStringi), "glGetStringi" },
    { offsetof(GL, glGetTexParameterIiv), "glGetTexParameterIiv" },
    { offsetof(GL, glGetTexParameterIuiv), "glGetTexParameterIuiv" },
    { offsetof(GL, glGetTransformFeedbackVarying), "glGetTransformFeedbackVarying" },
    { offsetof(GL, glGetUniformuiv), "glGetUniformuiv" },
    { offsetof(GL, glGetVertexAttribIiv), "glGetVertexAttribIiv" },
    { offsetof(GL, glGetVertexAttribIuiv), "glGetVertexAttribIuiv" },
    { offsetof(GL, glIsEnabledi), "glIsEnabledi" },
    { offsetof(GL, glIsFramebuffer), "glIsFramebuffer" },
    { offsetof(GL, glIsRenderbuffer), "glIsRenderbuffer" },
    { offsetof(GL, glIsVertexArray), "glIsVertexArray" },
    { offsetof(GL, glMapBufferRange), "glMapBufferRange" },
    { offsetof(GL, glRenderbufferStorage), "glRenderbufferStorage" },
    { offsetof(GL, glRenderbufferStorageMultisample), "glRenderbufferStorageMultisample" },
    { offsetof(GL, glTexParameterIiv), "glTexParameterIiv" },
    { offsetof(GL, glTexParameterIuiv), "glTexParameterIuiv" },
    { offsetof(GL, glTransformFeedbackVaryings), "glTransformFeedbackVaryings" },
    { offsetof(GL, glUniform1ui), "glUniform1ui" },
    { offsetof(GL, glUniform1uiv), "glUniform1uiv" },
    { offsetof(GL, glUniform2ui), "glUniform2ui" },
    { offsetof(GL, glUniform2uiv), "glUniform2uiv" },
    { offsetof(GL, glUniform3ui), "glUniform3ui" },
    { offsetof(GL, glUniform3uiv), "glUniform3uiv" },
    { offsetof(GL, glUniform4ui), "glUniform4ui" },
    { offsetof(GL, glUniform4uiv), "glUniform4uiv" },
    { offsetof(GL, glVertexAttribI1i), "glVertexAttribI1i" },
    { offsetof(GL, glVertexAttribI1iv), "glVertexAttribI1iv" },
    { offsetof(GL, glVertexAttribI1ui), "glVertexAttribI1ui" },
    { offsetof(GL, glVertexAttribI1uiv), "glVertexAttribI1uiv" },
    { offsetof(GL, glVertexAttribI2i), "glVertexAttribI2i" },
    { offsetof(GL, glVertexAttribI2iv), "glVertexAttribI2iv" },
    { offsetof(GL, glVertexAttribI2ui), "glVertexAttribI2ui" },
    { offsetof(GL, glVertexAttribI2uiv), "glVertexAttribI2uiv" },
    { offsetof(GL, glVertexAttribI3i), "glVertexAttribI3i" },
    { offsetof(GL, glVertexAttribI3iv), "glVertexAttribI3iv" },
    { offsetof(GL, glVertexAttribI3ui), "glVertexAttribI3ui" },
    { offsetof(GL, glVertexAttribI3uiv), "glVertexAttribI3uiv" },
    { offsetof(GL, glVertexAttribI4bv), "glVertexAttribI4bv" },
    { offsetof(GL, glVertexAttribI4i), "glVertexAttribI4i" },
    { offsetof(GL, glVertexAttribI4iv), "glVertexAttribI4iv" },
    { offsetof(GL, glVertexAttribI4sv), "glVertexAttribI4sv" },
    { offsetof(GL, glVertexAttribI4ubv), "glVertexAttribI4ubv" },
    { offsetof(GL, glVertexAttribI4ui), "glVertexAttribI4ui" },
    { offsetof(GL, glVertexAttribI4uiv), "glVertexAttribI4uiv" },
    { offsetof(GL, glVertexAttribI4usv), "glVertexAttribI4usv" },
    { offsetof(GL, glVertexAttribIPointer), "glVertexAttribIPointer" },
    { offsetof(GL, glCopyBufferSubData), "glCopyBufferSubData" },
    { offsetof(GL, glDrawArraysInstanced), "glDrawArraysInstanced" },
    { offsetof(GL, glDrawElementsInstanced), "glDrawElementsInstanced" },
    { offsetof(GL, glGetActiveUniformBlockName), "glGetActiveUniformBlockName" },
    { offsetof(GL, glGetActiveUniformBlockiv), "glGetActiveUniformBlockiv" },
    { offsetof(GL, glGetActiveUniformName), "glGetActiveUniformName" },
    { offsetof(GL, glGetActiveUniformsiv), "glGetActiveUniformsiv" },
    { offsetof(GL, glGetUniformBlockIndex), "glGetUniformBlockIndex" },
    { offsetof(GL, glGetUniformIndices), "glGetUniformIndices" },
    { offsetof(GL, glPrimitiveRestartIndex), "glPrimitiveRestartIndex" },
    { offsetof(GL, glTexBuffer), "glTexBuffer" },
    { offsetof(GL, glUniformBlockBinding), "glUniformBlockBinding" },
    { offsetof(GL, glClientWaitSync), "glClientWaitSync" },
    { offsetof(GL, glDeleteSync), "glDeleteSync" },
    { offsetof(GL, glDrawElementsBaseVertex), "glDrawElementsBaseVertex" },
    { offsetof(GL, glDrawElementsInstancedBaseVertex), "glDrawElementsInstancedBaseVertex" },
    { offsetof(GL, glDrawRangeElementsBaseVertex), "glDrawRangeElementsBaseVertex" },
    { offsetof(GL, glFenceSync), "glFenceSync" },
    { offsetof(GL, glFramebufferTexture), "glFramebufferTexture" },
    { offsetof(GL, glGetBufferParameteri64v), "glGetBufferParameteri64v" },
    { offsetof(GL, glGetInteger64i_v), "glGetInteger64i_v" },
    { offsetof(GL, glGetInteger64v), "glGetInteger64v" },
    { offsetof(GL, glGetMultisamplefv), "glGetMultisamplefv" },
    { offsetof(GL, glGetSynciv), "glGetSynciv" },
    { offsetof(GL, glIsSync), "glIsSync" },
    { offsetof(GL, glMultiDrawElementsBaseVertex), "glMultiDrawElementsBaseVertex" },
    { offsetof(GL, glProvokingVertex), "glProvokingVertex" },
    { offsetof(GL, glSampleMaski), "glSampleMaski" },
    { offsetof(GL, glTexImage2DMultisample), "glTexImage2DMultisample" },
    { offsetof(GL, glTexImage3DMultisample), "glTexImage3DMultisample" },
    { offsetof(GL, glWaitSync), "glWaitSync" },
    { offsetof(GL, glBindFragDataLocationIndexed), "glBindFragDataLocationIndexed" },
    { offsetof(GL, glBindSampler), "glBindSampler" },
    { offsetof(GL, glDeleteSamplers), "glDeleteSamplers" },
    { offsetof(GL, glGenSamplers), "glGenSamplers" },
    { offsetof(GL, glGetFragDataIndex), "glGetFragDataIndex" },
    { offsetof(GL, glGetQueryObjecti64v), "glGetQueryObjecti64v" },
    { offsetof(GL, glGetQueryObjectui64v), "glGetQueryObjectui64v" },
    { offsetof(GL, glGetSamplerParameterIiv), "glGetSamplerParameterIiv" },
    { offsetof(GL, glGetSamplerParameterIuiv), "glGetSamplerParameterIuiv" },
    { offsetof(GL, glGetSamplerParameterfv), "glGetSamplerParameterfv" },
    { offsetof(GL, glGetSamplerParameteriv), "glGetSamplerParameteriv" },
    { offsetof(GL, glIsSampler), "glIsSampler" },
    { offsetof(GL, glQueryCounter), "glQueryCounter" },
    { offsetof(GL, glSamplerParameterIiv), "glSamplerParameterIiv" },
    { offsetof(GL, glSamplerParameterIuiv), "glSamplerParameterIuiv" },
    { offsetof(GL, glSamplerParameterf), "glSamplerParameterf" },
    { offsetof(GL, glSamplerParameterfv), "glSamplerParameterfv" },
    { offsetof(GL, glSamplerParameteri), "glSamplerParameteri" },
    { offsetof(GL, glSamplerParameteriv), "glSamplerParameteriv" },
    { offsetof(GL, glVertexAttribDivisor), "glVertexAttribDivisor" },
    { offsetof(GL, glVertexAttribP1ui), "glVertexAttribP1ui" },
    { offsetof(GL, glVertexAttribP1uiv), "glVertexAttribP1uiv" },
    { offsetof(GL, glVertexAttribP2ui), "glVertexAttribP2ui" },
    { offsetof(GL, glVertexAttribP2uiv), "glVertexAttribP2uiv" },
    { offsetof(GL, glVertexAttribP3ui), "glVertexAttribP3ui" },
    { offsetof(GL, glVertexAttribP3uiv), "glVertexAttribP3uiv" },
    { offsetof(GL, glVertexAttribP4ui), "glVertexAttribP4ui" },
    { offsetof(GL, glVertexAttribP4uiv), "glVertexAttribP4uiv" },
    { offsetof(GL, glBeginQueryIndexed), "glBeginQueryIndexed" },
    { offsetof(GL, glBindTransformFeedback), "glBindTransformFeedback" },
    { offsetof(GL, glBlendEquationSeparatei), "glBlendEquationSeparatei" },
    { offsetof(GL, glBlendEquationi), "glBlendEquationi" },
    { offsetof(GL, glBlendFuncSeparatei), "glBlendFuncSeparatei" },
    { offsetof(GL, glBlendFunci), "glBlendFunci" },
    { offsetof(GL, glDeleteTransformFeedbacks), "glDeleteTransformFeedbacks" },
    { offsetof(GL, glDrawArraysIndirect), "glDrawArraysIndirect" },
    { offsetof(GL, glDrawElementsIndirect), "glDrawElementsIndirect" },
    { offsetof(GL, glDrawTransformFeedback), "glDrawTransformFeedback" },
    { offsetof(GL, glDrawTransformFeedbackStream), "glDrawTransformFeedbackStream" },
    { offsetof(GL, glEndQueryIndexed), "glEndQueryIndexed" },
    { offsetof(GL, glGenTransformFeedbacks), "glGenTransformFeedbacks" },
    { offsetof(GL, glGetActiveSubroutineName), "glGetActiveSubroutineName" },
    { offsetof(GL, glGetActiveSubroutineUniformName), "glGetActiveSubroutineUniformName" },
    { offsetof(GL, glGetActiveSubroutineUniformiv), "glGetActiveSubroutineUniformiv" },
    { offsetof(GL, glGetProgramStageiv), "glGetProgramStageiv" },
    { offsetof(GL, glGetQueryIndexediv), "glGetQueryIndexediv" },
    { offsetof(GL, glGetSubroutineIndex), "glGetSubroutineIndex" },
    { offsetof(GL, glGetSubroutineUniformLocation), "glGetSubroutineUniformLocation" },
    { offsetof(GL, glGetUniformSubroutineuiv), "glGetUniformSubroutineuiv" },
    { offsetof(GL, glGetUniformdv), "glGetUniformdv" },
    { offsetof(GL, glIsTransformFeedback), "glIsTransformFeedback" },
    { offsetof(GL, glMinSampleShading), "glMinSampleShading" },
    { offsetof(GL, glPatchParameterfv), "glPatchParameterfv" },
    { offsetof(GL, glPatchParameteri), "glPatchParameteri" },
    { offsetof(GL, glPauseTransformFeedback), "glPauseTransformFeedback" },
    { offsetof(GL, glResumeTransformFeedback), "glResumeTransformFeedback" },
    { offsetof(GL, glUniform1d), "glUniform1d" },
    { offsetof(GL, glUniform1dv), "glUniform1dv" },
    { offsetof(GL, glUniform2d), "glUniform2d" },
    { offsetof(GL, glUniform2dv), "glUniform2dv" },
    { offsetof(GL, glUniform3d), "glUniform3d" },
    { offsetof(GL, glUniform3dv), "glUniform3dv" },
    { offsetof(GL, glUniform4d), "glUniform4d" },
    { offsetof(GL, glUniform4dv), "glUniform4dv" },
    { offsetof(GL, glUniformMatrix2dv), "glUniformMatrix2dv" },
    { offsetof(GL, glUniformMatrix2x3dv), "glUniformMatrix2x3dv" },
    { offsetof(GL, glUniformMatrix2x4dv), "glUniformMatrix2x4dv" },
    { offsetof(GL, glUniformMatrix3dv), "glUniformMatrix3dv" },
    { offsetof(GL, glUniformMatrix3x2dv), "glUniformMatrix3x2dv" },
    { offsetof(GL, glUniformMatrix3x4dv), "glUniformMatrix3x4dv" },
    { offsetof(GL, glUniformMatrix4dv), "glUniformMatrix4dv" },
    { offsetof(GL, glUniformMatrix4x2dv), "glUniformMatrix4x2dv" },
    { offsetof(GL, glUniformMatrix4x3dv), "glUniformMatrix4x3dv" },
    { offsetof(GL, glUniformSubroutinesuiv), "glUniformSubroutinesuiv" },
    { offsetof(GL, glActiveShaderProgram), "glActiveShaderProgram" },
    { offsetof(GL, glBindProgramPipeline), "glBindProgramPipeline" },
    { offsetof(GL, glClearDepthf), "glClearDepthf" },
    { offsetof(GL, glCreateShaderProgramv), "glCreateShaderProgramv" },
    { offsetof(GL, glDeleteProgramPipelines), "glDeleteProgramPipelines" },
    { offsetof(GL, glDepthRangeArrayv), "glDepthRangeArrayv" },
    { offsetof(GL, glDepthRangeIndexed), "glDepthRangeIndexed" },
    { offsetof(GL, glDepthRangef), "glDepthRangef" },
    { offsetof(GL, glGenProgramPipelines), "glGenProgramPipelines" },
    { offsetof(GL, glGetDoublei_v), "glGetDoublei_v" },
    { offsetof(GL, glGetFloati_v), "glGetFloati_v" },
    { offsetof(GL, glGetProgramBinary), "glGetProgramBinary" },
    { offsetof(GL, glGetProgramPipelineInfoLog), "glGetProgramPipelineInfoLog" },
    { offsetof(GL, glGetProgramPipelineiv), "glGetProgramPipelineiv" },
    { offsetof(GL, glGetShaderPrecisionFormat), "glGetShaderPrecisionFormat" },
    { offsetof(GL, glGetVertexAttribLdv), "glGetVertexAttribLdv" },
    { offsetof(GL, glIsProgramPipeline), "glIsProgramPipeline" },
    { offsetof(GL, glProgramBinary), "glProgramBinary" },
    { offsetof(GL, glProgramParameteri), "glProgramParameteri" },
    { offsetof(GL, glProgramUniform1d), "glProgramUniform1d" },
    { offsetof(GL, glProgramUniform1dv), "glProgramUniform1dv" },
    { offsetof(GL, glProgramUniform1f), "glProgramUniform1f" },
    { offsetof(GL, glProgramUniform1fv), "glProgramUniform1fv" },
    { offsetof(GL, glProgramUniform1i), "glProgramUniform1i" },
    { offsetof(GL, glProgramUniform1iv), "glProgramUniform1iv" },
    { offsetof(GL, glProgramUniform1ui), "glProgramUniform1ui" },
    { offsetof(GL, glProgramUniform1uiv), "glProgramUniform1uiv" },
    { offsetof(GL, glProgramUniform2d), "glProgramUniform2d" },
    { offsetof(GL, glProgramUniform2dv), "glProgramUniform2dv" },
    { offsetof(GL, glProgramUniform2f), "glProgramUniform2f" },
    { offsetof(GL, glProgramUniform2fv), "glProgramUniform2fv" },
    { offsetof(GL, glProgramUniform2i), "glProgramUniform2i" },
    { offsetof(GL, glProgramUniform2iv), "glProgramUniform2iv" },
    { offsetof(GL, glProgramUniform2ui), "glProgramUniform2ui" },
    { offsetof(GL, glProgramUniform2uiv), "glProgramUniform2uiv" },
    { offsetof(GL, glProgramUniform3d), "glProgramUniform3d" },
    { offsetof(GL, glProgramUniform3dv), "glProgramUniform3dv" },
    { offsetof(GL, glProgramUniform3f), "glProgramUniform3f" },
    { offsetof(GL, glProgramUniform3fv), "glProgramUniform3fv" },
    { offsetof(GL, glProgramUniform3i), "glProgramUniform3i" },
    { offsetof(GL, glProgramUniform3iv), "glProgramUniform3iv" },
    { offsetof(GL, glProgramUniform3ui), "glProgramUniform3ui" },
    { offsetof(GL, glProgramUniform3uiv), "glProgramUniform3uiv" },
    { offsetof(GL, glProgramUniform4d), "glProgramUniform4d" },
    { offsetof(GL, glProgramUniform4dv), "glProgramUniform4dv" },
    { offsetof(GL, glProgramUniform4f), "glProgramUniform4f" },
    { offsetof(GL, glProgramUniform4fv), "glProgramUniform4fv" },
    { offsetof(GL, glProgramUniform4i), "glProgramUniform4i" },
    { offsetof(GL, glProgramUniform4iv), "glProgramUniform4iv" },
    { offsetof(GL, glProgramUniform4ui), "glProgramUniform4ui" },
    { offsetof(GL, glProgramUniform4uiv), "glProgramUniform4uiv" },
    { offsetof(GL, glProgramUniformMatrix2dv), "glProgramUniformMatrix2dv" },
    { offsetof(GL, glProgramUniformMatrix2fv), "glProgramUniformMatrix2fv" },
    { offsetof(GL, glProgramUniformMatrix2x3dv), "glProgramUniformMatrix2x3dv" },
    { offsetof(GL, glProgramUniformMatrix2x3fv), "glProgramUniformMatrix2x3fv" },
    { offsetof(GL, glProgramUniformMatrix2x4dv), "glProgramUniformMatrix2x4dv" },
    { offsetof(GL, glProgramUniformMatrix2x4fv), "glProgramUniformMatrix2x4fv" },
    { offsetof(GL, glProgramUniformMatrix3dv), "glProgramUniformMatrix3dv" },
    { offsetof(GL, glProgramUniformMatrix3fv), "glProgramUniformMatrix3fv" },
    { offsetof(GL, glProgramUniformMatrix3x2dv), "glProgramUniformMatrix3x2dv" },
    { offsetof(GL, glProgramUniformMatrix3x2fv), "glProgramUniformMatrix3x2fv" },
    { offsetof(GL, glProgramUniformMatrix3x4dv), "glProgramUniformMatrix3x4dv" },
    { offsetof(GL, glProgramUniformMatrix3x4fv), "glProgramUniformMatrix3x4fv" },
    { offsetof(GL, glProgramUniformMatrix4dv), "glProgramUniformMatrix4dv" },
    { offsetof(GL, glProgramUniformMatrix4fv), "glProgramUniformMatrix4fv" },
    { offsetof(GL, glProgramUniformMatrix4x2dv), "glProgramUniformMatrix4x2dv" },
    { offsetof(GL, glProgramUniformMatrix4x2fv), "glProgramUniformMatrix4x2fv" },
    { offsetof(GL, glProgramUniformMatrix4x3dv), "glProgramUniformMatrix4x3dv" },
    { offsetof(GL, glProgramUniformMatrix4x3fv), "glProgramUniformMatrix4x3fv" },
    { offsetof(GL, glReleaseShaderCompiler), "glReleaseShaderCompiler" },
    { offsetof(GL, glScissorArrayv), "glScissorArrayv" },
    { offsetof(GL, glScissorIndexed), "glScissorIndexed" },
    { offsetof(GL, glScissorIndexedv), "glScissorIndexedv" },
    { offsetof(GL, glShaderBinary), "glShaderBinary" },
    { offsetof(GL, glUseProgramStages), "glUseProgramStages" },
    { offsetof(GL, glValidateProgramPipeline), "glValidateProgramPipeline" },
    { offsetof(GL, glVertexAttribL1d), "glVertexAttribL1d" },
    { offsetof(GL, glVertexAttribL1dv), "glVertexAttribL1dv" },
    { offsetof(GL, glVertexAttribL2d), "glVertexAttribL2d" },
    { offsetof(GL, glVertexAttribL2dv), "glVertexAttribL2dv" },
    { offsetof(GL, glVertexAttribL3d), "glVertexAttribL3d" },
    { offsetof(GL, glVertexAttribL3dv), "glVertexAttribL3dv" },
    { offsetof(GL, glVertexAttribL4d), "glVertexAttribL4d" },
    { offsetof(GL, glVertexAttribL4dv), "glVertexAttribL4dv" },
    { offsetof(GL, glVertexAttribLPointer), "glVertexAttribLPointer" },
    { offsetof(GL, glViewportArrayv), "glViewportArrayv" },
    { offsetof(GL, glViewportIndexedf), "glViewportIndexedf" },
    { offsetof(GL, glViewportIndexedfv), "glViewportIndexedfv" },
    { offsetof(GL, glBindImageTexture), "glBindImageTexture" },
    { offsetof(GL, glDrawArraysInstancedBaseInstance), "glDrawArraysInstancedBaseInstance" },
    { offsetof(GL, glDrawElementsInstancedBaseInstance), "glDrawElementsInstancedBaseInstance" },
    { offsetof(GL, glDrawElementsInstancedBaseVertexBaseInstance), "glDrawElementsInstancedBaseVertexBaseInstance" },
    { offsetof(GL, glDrawTransformFeedbackInstanced), "glDrawTransformFeedbackInstanced" },
    { offsetof(GL, glDrawTransformFeedbackStreamInstanced), "glDrawTransformFeedbackStreamInstanced" },
    { offsetof(GL, glGetActiveAtomicCounterBufferiv), "glGetActiveAtomicCounterBufferiv" },
    { offsetof(GL, glGetInternalformativ), "glGetInternalformativ" },
    { offsetof(GL, glMemoryBarrier), "glMemoryBarrier" },
    { offsetof(GL, glTexStorage1D), "glTexStorage1D" },
    { offsetof(GL, glTexStorage2D), "glTexStorage2D" },
    { offsetof(GL, glTexStorage3D), "glTexStorage3D" },
    { offsetof(GL, glBindVertexBuffer), "glBindVertexBuffer" },
    { offsetof(GL, glClearBufferData), "glClearBufferData" },
    { offsetof(GL, glClearBufferSubData), "glClearBufferSubData" },
    { offsetof(GL, glCopyImageSubData), "glCopyImageSubData" },
    { offsetof(GL, glDebugMessageCallback), "glDebugMessageCallback" },
    { offsetof(GL, glDebugMessageControl), "glDebugMessageControl" },
    { offsetof(GL, glDebugMessageInsert), "glDebugMessageInsert" },
    { offsetof(GL, glDispatchCompute), "glDispatchCompute" },
    { offsetof(GL, glDispatchComputeIndirect), "glDispatchComputeIndirect" },
    { offsetof(GL, glFramebufferParameteri), "glFramebufferParameteri" },
    { offsetof(GL, glGetDebugMessageLog), "glGetDebugMessageLog" },
    { offsetof(GL, glGetFramebufferParameteriv), "glGetFramebufferParameteriv" },
    { offsetof(GL, glGetInternalformati64v), "glGetInternalformati64v" },
    { offsetof(GL, glGetObjectLabel), "glGetObjectLabel" },
    { offsetof(GL, glGetObjectPtrLabel), "glGetObjectPtrLabel" },
    { offsetof(GL, glGetPointerv), "glGetPointerv" },
    { offsetof(GL, glGetProgramInterfaceiv), "glGetProgramInterfaceiv" },
    { offsetof(GL, glGetProgramResourceIndex), "glGetProgramResourceIndex" },
    { offsetof(GL, glGetProgramResourceLocation), "glGetProgramResourceLocation" },
    { offsetof(GL, glGetProgramResourceLocationIndex), "glGetProgramResourceLocationIndex" },
    { offsetof(GL, glGetProgramResourceName), "glGetProgramResourceName" },
    { offsetof(GL, glGetProgramResourceiv), "glGetProgramResourceiv" },
    { offsetof(GL, glInvalidateBufferData), "glInvalidateBufferData" },
    { offsetof(GL, glInvalidateBufferSubData), "glInvalidateBufferSubData" },
    { offsetof(GL, glInvalidateFramebuffer), "glInvalidateFramebuffer" },
    { offsetof(GL, glInvalidateSubFramebuffer), "glInvalidateSubFramebuffer" },
    { offsetof(GL, glInvalidateTexImage), "glInvalidateTexImage" },
    { offsetof(GL, glInvalidateTexSubImage), "glInvalidateTexSubImage" },
    { offsetof(GL, glMultiDrawArraysIndirect), "glMultiDrawArraysIndirect" },
    { offsetof(GL, glMultiDrawElementsIndirect), "glMultiDrawElementsIndirect" },
    { offsetof(GL, glObjectLabel), "glObjectLabel" },
    { offsetof(GL, glObjectPtrLabel), "glObjectPtrLabel" },
    { offsetof(GL, glPopDebugGroup), "glPopDebugGroup" },
    { offsetof(GL, glPushDebugGroup), "glPushDebugGroup" },
    { offsetof(GL, glShaderStorageBlockBinding), "glShaderStorageBlockBinding" },
    { offsetof(GL, glTexBufferRange), "glTexBufferRange" },
    { offsetof(GL, glTexStorage2DMultisample), "glTexStorage2DMultisample" },
    { offsetof(GL, glTexStorage3DMultisample), "glTexStorage3DMultisample" },
    { offsetof(GL, glTextureView), "glTextureView" },
    { offsetof(GL, glVertexAttribBinding), "glVertexAttribBinding" },
    { offsetof(GL, glVertexAttribFormat), "glVertexAttribFormat" },
    { offsetof(GL, glVertexAttribIFormat), "glVertexAttribIFormat" },
    { offsetof(GL, glVertexAttribLFormat), "glVertexAttribLFormat" },
    { offsetof(GL, glVertexBindingDivisor), "glVertexBindingDivisor" },
    { offsetof(GL, glBindBuffersBase), "glBindBuffersBase" },
    { offsetof(GL, glBindBuffersRange), "glBindBuffersRange" },
    { offsetof(GL, glBindImageTextures), "glBindImageTextures" },
    { offsetof(GL, glBindSamplers), "glBindSamplers" },
    { offsetof(GL, glBindTextures), "glBindTextures" },
    { offsetof(GL, glBindVertexBuffers), "glBindVertexBuffers" },
    { offsetof(GL, glBufferStorage), "glBufferStorage" },
    { offsetof(GL, glClearTexImage), "glClearTexImage" },
    { offsetof(GL, glClearTexSubImage), "glClearTexSubImage" },
    { offsetof(GL, glBindTextureUnit), "glBindTextureUnit" },
    { offsetof(GL, glBlitNamedFramebuffer), "glBlitNamedFramebuffer" },
    { offsetof(GL, glCheckNamedFramebufferStatus), "glCheckNamedFramebufferStatus" },
    { offsetof(GL, glClearNamedBufferData), "glClearNamedBufferData" },
    { offsetof(GL, glClearNamedBufferSubData), "glClearNamedBufferSubData" },
    { offsetof(GL, glClearNamedFramebufferfi), "glClearNamedFramebufferfi" },
    { offsetof(GL, glClearNamedFramebufferfv), "glClearNamedFramebufferfv" },
    { offsetof(GL, glClearNamedFramebufferiv), "glClearNamedFramebufferiv" },
    { offsetof(GL, glClearNamedFramebufferuiv), "glClearNamedFramebufferuiv" },
    { offsetof(GL, glClipControl), "glClipControl" },
    { offsetof(GL, glCompressedTextureSubImage1D), "glCompressedTextureSubImage1D" },
    { offsetof(GL, glCompressedTextureSubImage2D), "glCompressedTextureSubImage2D" },
    { offsetof(GL, glCompressedTextureSubImage3D), "glCompressedTextureSubImage3D" },
    { offsetof(GL, glCopyNamedBufferSubData), "glCopyNamedBufferSubData" },
    { offsetof(GL, glCopyTextureSubImage1D), "glCopyTextureSubImage1D" },
    { offsetof(GL, glCopyTextureSubImage2D), "glCopyTextureSubImage2D" },
    { offsetof(GL, glCopyTextureSubImage3D), "glCopyTextureSubImage3D" },
    { offsetof(GL, glCreateBuffers), "glCreateBuffers" },
    { offsetof(GL, glCreateFramebuffers), "glCreateFramebuffers" },
    { offsetof(GL, glCreateProgramPipelines), "glCreateProgramPipelines" },
    { offsetof(GL, glCreateQueries), "glCreateQueries" },
    { offsetof(GL, glCreateRenderbuffers), "glCreateRenderbuffers" },
    { offsetof(GL, glCreateSamplers), "glCreateSamplers" },
    { offsetof(GL, glCreateTextures), "glCreateTextures" },
    { offsetof(GL, glCreateTransformFeedbacks), "glCreateTransformFeedbacks" },
    { offsetof(GL, glCreateVertexArrays), "glCreateVertexArrays" },
    { offsetof(GL, glDisableVertexArrayAttrib), "glDisableVertexArrayAttrib" },
    { offsetof(GL, glEnableVertexArrayAttrib), "glEnableVertexArrayAttrib" },
    { offsetof(GL, glFlushMappedNamedBufferRange), "glFlushMappedNamedBufferRange" },
    { offsetof(GL, glGenerateTextureMipmap), "glGenerateTextureMipmap" },
    { offsetof(GL, glGetCompressedTextureImage), "glGetCompressedTextureImage" },
    { offsetof(GL, glGetCompressedTextureSubImage), "glGetCompressedTextureSubImage" },
    { offsetof(GL, glGetGraphicsResetStatus), "glGetGraphicsResetStatus" },
    { offsetof(GL, glGetNamedBufferParameteri64v), "glGetNamedBufferParameteri64v" },
    { offsetof(GL, glGetNamedBufferParameteriv), "glGetNamedBufferParameteriv" },
    { offsetof(GL, glGetNamedBufferPointerv), "glGetNamedBufferPointerv" },
    { offsetof(GL, glGetNamedBufferSubData), "glGetNamedBufferSubData" },
    { offsetof(GL, glGetNamedFramebufferAttachmentParameteriv), "glGetNamedFramebufferAttachmentParameteriv" },
    { offsetof(GL, glGetNamedFramebufferParameteriv), "glGetNamedFramebufferParameteriv" },
    { offsetof(GL, glGetNamedRenderbufferParameteriv), "glGetNamedRenderbufferParameteriv" },
    { offsetof(GL, glGetQueryBufferObjecti64v), "glGetQueryBufferObjecti64v" },
    { offsetof(GL, glGetQueryBufferObjectiv), "glGetQueryBufferObjectiv" },
    { offsetof(GL, glGetQueryBufferObjectui64v), "glGetQueryBufferObjectui64v" },
    { offsetof(GL, glGetQueryBufferObjectuiv), "glGetQueryBufferObjectuiv" },
    { offsetof(GL, glGetTextureImage), "glGetTextureImage" },
    { offsetof(GL, glGetTextureLevelParameterfv), "glGetTextureLevelParameterfv" },
    { offsetof(GL, glGetTextureLevelParameteriv), "glGetTextureLevelParameteriv" },
    { offsetof(GL, glGetTextureParameterIiv), "glGetTextureParameterIiv" },
    { offsetof(GL, glGetTextureParameterIuiv), "glGetTextureParameterIuiv" },
    { offsetof(GL, glGetTextureParameterfv), "glGetTextureParameterfv" },
    { offsetof(GL, glGetTextureParameteriv), "glGetTextureParameteriv" },
    { offsetof(GL, glGetTextureSubImage), "glGetTextureSubImage" },
    { offsetof(GL, glGetTransformFeedbacki64_v), "glGetTransformFeedbacki64_v" },
    { offsetof(GL, glGetTransformFeedbacki_v), "glGetTransformFeedbacki_v" },
    { offsetof(GL, glGetTransformFeedbackiv), "glGetTransformFeedbackiv" },
    { offsetof(GL, glGetVertexArrayIndexed64iv), "glGetVertexArrayIndexed64iv" },
    { offsetof(GL, glGetVertexArrayIndexediv), "glGetVertexArrayIndexediv" },
    { offsetof(GL, glGetVertexArrayiv), "glGetVertexArrayiv" },
    { offsetof(GL, glGetnCompressedTexImage), "glGetnCompressedTexImage" },
    { offsetof(GL, glGetnTexImage), "glGetnTexImage" },
    { offsetof(GL, glGetnUniformdv), "glGetnUniformdv" },
    { offsetof(GL, glGetnUniformfv), "glGetnUniformfv" },
    { offsetof(GL, glGetnUniformiv), "glGetnUniformiv" },
    { offsetof(GL, glGetnUniformuiv), "glGetnUniformuiv" },
    { offsetof(GL, glInvalidateNamedFramebufferData), "glInvalidateNamedFramebufferData" },
    { offsetof(GL, glInvalidateNamedFramebufferSubData), "glInvalidateNamedFramebufferSubData" },
    { offsetof(GL, glMapNamedBuffer), "glMapNamedBuffer" },
    { offsetof(GL, glMapNamedBufferRange), "glMapNamedBufferRange" },
    { offsetof(GL, glMemoryBarrierByRegion), "glMemoryBarrierByRegion" },
    { offsetof(GL, glNamedBufferData), "glNamedBufferData" },
    { offsetof(GL, glNamedBufferStorage), "glNamedBufferStorage" },
    { offsetof(GL, glNamedBufferSubData), "glNamedBufferSubData" },
    { offsetof(GL, glNamedFramebufferDrawBuffer), "glNamedFramebufferDrawBuffer" },
    { offsetof(GL, glNamedFramebufferDrawBuffers), "glNamedFramebufferDrawBuffers" },
    { offsetof(GL, glNamedFramebufferParameteri), "glNamedFramebufferParameteri" },
    { offsetof(GL, glNamedFramebufferReadBuffer), "glNamedFramebufferReadBuffer" },
    { offsetof(GL, glNamedFramebufferRenderbuffer), "glNamedFramebufferRenderbuffer" },
    { offsetof(GL, glNamedFramebufferTexture), "glNamedFramebufferTexture" },
    { offsetof(GL, glNamedFramebufferTextureLayer), "glNamedFramebufferTextureLayer" },
    { offsetof(GL, glNamedRenderbufferStorage), "glNamedRenderbufferStorage" },
    { offsetof(GL, glNamedRenderbufferStorageMultisample), "glNamedRenderbufferStorageMultisample" },
    { offsetof(GL, glReadnPixels), "glReadnPixels" },
    { offsetof(GL, glTextureBarrier), "glTextureBarrier" },
    { offsetof(GL, glTextureBuffer), "glTextureBuffer" },
    { offsetof(GL, glTextureBufferRange), "glTextureBufferRange" },
    { offsetof(GL, glTextureParameterIiv), "glTextureParameterIiv" },
    { offsetof(GL, glTextureParameterIuiv), "glTextureParameterIuiv" },
    { offsetof(GL, glTextureParameterf), "glTextureParameterf" },
    { offsetof(GL, glTextureParameterfv), "glTextureParameterfv" },
    { offsetof(GL, glTextureParameteri), "glTextureParameteri" },
    { offsetof(GL, glTextureParameteriv), "glTextureParameteriv" },
    { offsetof(GL, glTextureStorage1D), "glTextureStorage1D" },
    { offsetof(GL, glTextureStorage2D), "glTextureStorage2D" },
    { offsetof(GL, glTextureStorage2DMultisample), "glTextureStorage2DMultisample" },
    { offsetof(GL, glTextureStorage3D), "glTextureStorage3D" },
    { offsetof(GL, glTextureStorage3DMultisample), "glTextureStorage3DMultisample" },
    { offsetof(GL, glTextureSubImage1D), "glTextureSubImage1D" },
    { offsetof(GL, glTextureSubImage2D), "glTextureSubImage2D" },
    { offsetof(GL, glTextureSubImage3D), "glTextureSubImage3D" },
    { offsetof(GL, glTransformFeedbackBufferBase), "glTransformFeedbackBufferBase" },
    { offsetof(GL, glTransformFeedbackBufferRange), "glTransformFeedbackBufferRange" },
    { offsetof(GL, glUnmapNamedBuffer), "glUnmapNamedBuffer" },
    { offsetof(GL, glVertexArrayAttribBinding), "glVertexArrayAttribBinding" },
    { offsetof(GL, glVertexArrayAttribFormat), "glVertexArrayAttribFormat" },
    { offsetof(GL, glVertexArrayAttribIFormat), "glVertexArrayAttribIFormat" },
    { offsetof(GL, glVertexArrayAttribLFormat), "glVertexArrayAttribLFormat" },
    { offsetof(GL, glVertexArrayBindingDivisor), "glVertexArrayBindingDivisor" },
    { offsetof(GL, glVertexArrayElementBuffer), "glVertexArrayElementBuffer" },
    { offsetof(GL, glVertexArrayVertexBuffer), "glVertexArrayVertexBuffer" },
    { offsetof(GL, glVertexArrayVertexBuffers), "glVertexArrayVertexBuffers" },
    { offsetof(GL, glMultiDrawArraysIndirectCount), "glMultiDrawArraysIndirectCount" },
    { offsetof(GL, glMultiDrawElementsIndirectCount), "glMultiDrawElementsIndirectCount" },
    { offsetof(GL, glPolygonOffsetClamp), "glPolygonOffsetClamp" },
    { offsetof(GL, glSpecializeShader), "glSpecializeShader" },
};

static const size_t __GLEntryCount = sizeof(__GLEntryPoints) /
    sizeof(__GLEntryPoints[0]);

static const EntryOffset ___3DFXTbufferEntryPoints[] = {
    { offsetof(_3DFXTbuffer, glTbufferMask), "glTbufferMask3DFX" },
};

static const size_t ___3DFXTbufferEntryCount = sizeof(___3DFXTbufferEntryPoints) /
    sizeof(___3DFXTbufferEntryPoints[0]);

static const EntryOffset __AMDDebugOutputEntryPoints[] = {
    { offsetof(AMDDebugOutput, glDebugMessageCallback), "glDebugMessageCallbackAMD" },
    { offsetof(AMDDebugOutput, glDebugMessageEnable), "glDebugMessageEnableAMD" },
    { offsetof(AMDDebugOutput, glDebugMessageInsert), "glDebugMessageInsertAMD" },
    { offsetof(AMDDebugOutput, glGetDebugMessageLog), "glGetDebugMessageLogAMD" },
};

static const size_t __AMDDebugOutputEntryCount = sizeof(__AMDDebugOutputEntryPoints) /
    sizeof(__AMDDebugOutputEntryPoints[0]);

static const EntryOffset __AMDDrawBuffersBlendEntryPoints[] = {
    { offsetof(AMDDrawBuffersBlend, glBlendEquationIndexed), "glBlendEquationIndexedAMD" },
    { offsetof(AMDDrawBuffersBlend, glBlendEquationSeparateIndexed), "glBlendEquationSeparateIndexedAMD" },
    { offsetof(AMDDrawBuffersBlend, glBlendFuncIndexed), "glBlendFuncIndexedAMD" },
    { offsetof(AMDDrawBuffersBlend, glBlendFuncSeparateIndexed), "glBlendFuncSeparateIndexedAMD" },
};

static const size_t __AMDDrawBuffersBlendEntryCount = sizeof(__AMDDrawBuffersBlendEntryPoints) /
    sizeof(__AMDDrawBuffersBlendEntryPoints[0]);

static const EntryOffset __AMDFramebufferMultisampleAdvancedEntryPoints[] = {
    { offsetof(AMDFramebufferMultisampleAdvanced, glNamedRenderbufferStorageMultisampleAdvanced), "glNamedRenderbufferStorageMultisampleAdvancedAMD" },
    { offsetof(AMDFramebufferMultisampleAdvanced, glRenderbufferStorageMultisampleAdvanced), "glRenderbufferStorageMultisampleAdvancedAMD" },
};

static const size_t __AMDFramebufferMultisampleAdvancedEntryCount = sizeof(__AMDFramebufferMultisampleAdvancedEntryPoints) /
    sizeof(__AMDFramebufferMultisampleAdvancedEntryPoints[0]);

static const EntryOffset __AMDFramebufferSamplePositionsEntryPoints[] = {
    { offsetof(AMDFramebufferSamplePositions, glFramebufferSamplePositionsfv), "glFramebufferSamplePositionsfvAMD" },
    { offsetof(AMDFramebufferSamplePositions, glGetFramebufferParameterfv), "glGetFramebufferParameterfvAMD" },
    { offsetof(AMDFramebufferSamplePositions, glGetNamedFramebufferParameterfv), "glGetNamedFramebufferParameterfvAMD" },
    { offsetof(AMDFramebufferSamplePositions, glNamedFramebufferSamplePositionsfv), "glNamedFramebufferSamplePositionsfvAMD" },
};

static const size_t __AMDFramebufferSamplePositionsEntryCount = sizeof(__AMDFramebufferSamplePositionsEntryPoints) /
    sizeof(__AMDFramebufferSamplePositionsEntryPoints[0]);

static const EntryOffset __AMDGPUShaderInt64EntryPoints[] = {
    { offsetof(AMDGPUShaderInt64, glGetUniformi64v), "glGetUniformi64vNV" },
    { offsetof(AMDGPUShaderInt64, glGetUniformui64v), "glGetUniformui64vNV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform1i64), "glProgramUniform1i64NV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform1i64v), "glProgramUniform1i64vNV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform1ui64), "glProgramUniform1ui64NV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform1ui64v), "glProgramUniform1ui64vNV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform2i64), "glProgramUniform2i64NV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform2i64v), "glProgramUniform2i64vNV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform2ui64), "glProgramUniform2ui64NV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform2ui64v), "glProgramUniform2ui64vNV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform3i64), "glProgramUniform3i64NV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform3i64v), "glProgramUniform3i64vNV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform3ui64), "glProgramUniform3ui64NV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform3ui64v), "glProgramUniform3ui64vNV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform4i64), "glProgramUniform4i64NV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform4i64v), "glProgramUniform4i64vNV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform4ui64), "glProgramUniform4ui64NV" },
    { offsetof(AMDGPUShaderInt64, glProgramUniform4ui64v), "glProgramUniform4ui64vNV" },
    { offsetof(AMDGPUShaderInt64, glUniform1i64), "glUniform1i64NV" },
    { offsetof(AMDGPUShaderInt64, glUniform1i64v), "glUniform1i64vNV" },
    { offsetof(AMDGPUShaderInt64, glUniform1ui64), "glUniform1ui64NV" },
    { offsetof(AMDGPUShaderInt64, glUniform1ui64v), "glUniform1ui64vNV" },
    { offsetof(AMDGPUShaderInt64, glUniform2i64), "glUniform2i64NV" },
    { offsetof(AMDGPUShaderInt64, glUniform2i64v), "glUniform2i64vNV" },
    { offsetof(AMDGPUShaderInt64, glUniform2ui64), "glUniform2ui64NV" },
    { offsetof(AMDGPUShaderInt64, glUniform2ui64v), "glUniform2ui64vNV" },
    { offsetof(AMDGPUShaderInt64, glUniform3i64), "glUniform3i64NV" },
    { offsetof(AMDGPUShaderInt64, glUniform3i64v), "glUniform3i64vNV" },
    { offsetof(AMDGPUShaderInt64, glUniform3ui64), "glUniform3ui64NV" },
    { offsetof(AMDGPUShaderInt64, glUniform3ui64v), "glUniform3ui64vNV" },
    { offsetof(AMDGPUShaderInt64, glUniform4i64), "glUniform4i64NV" },
    { offsetof(AMDGPUShaderInt64, glUniform4i64v), "glUniform4i64vNV" },
    { offsetof(AMDGPUShaderInt64, glUniform4ui64), "glUniform4ui64NV" },
    { offsetof(AMDGPUShaderInt64, glUniform4ui64v), "glUniform4ui64vNV" },
};

static const size_t __AMDGPUShaderInt64EntryCount = sizeof(__AMDGPUShaderInt64EntryPoints) /
    sizeof(__AMDGPUShaderInt64EntryPoints[0]);

static const EntryOffset __AMDInterleavedElementsEntryPoints[] = {
    { offsetof(AMDInterleavedElements, glVertexAttribParameteri), "glVertexAttribParameteriAMD" },
};

static const size_t __AMDInterleavedElementsEntryCount = sizeof(__AMDInterleavedElementsEntryPoints) /
    sizeof(__AMDInterleavedElementsEntryPoints[0]);

static const EntryOffset __AMDMultiDrawIndirectEntryPoints[] = {
    { offsetof(AMDMultiDrawIndirect, glMultiDrawArraysIndirect), "glMultiDrawArraysIndirectAMD" },
    { offsetof(AMDMultiDrawIndirect, glMultiDrawElementsIndirect), "glMultiDrawElementsIndirectAMD" },
};

static const size_t __AMDMultiDrawIndirectEntryCount = sizeof(__AMDMultiDrawIndirectEntryPoints) /
    sizeof(__AMDMultiDrawIndirectEntryPoints[0]);

static const EntryOffset __AMDNameGenDeleteEntryPoints[] = {
    { offsetof(AMDNameGenDelete, glDeleteNames), "glDeleteNamesAMD" },
    { offsetof(AMDNameGenDelete, glGenNames), "glGenNamesAMD" },
    { offsetof(AMDNameGenDelete, glIsName), "glIsNameAMD" },
};

static const size_t __AMDNameGenDeleteEntryCount = sizeof(__AMDNameGenDeleteEntryPoints) /
    sizeof(__AMDNameGenDeleteEntryPoints[0]);

static const EntryOffset __AMDOcclusionQueryEventEntryPoints[] = {
    { offsetof(AMDOcclusionQueryEvent, glQueryObjectParameterui), "glQueryObjectParameteruiAMD" },
};

static const size_t __AMDOcclusionQueryEventEntryCount = sizeof(__AMDOcclusionQueryEventEntryPoints) /
    sizeof(__AMDOcclusionQueryEventEntryPoints[0]);

static const EntryOffset __AMDPerformanceMonitorEntryPoints[] = {
    { offsetof(AMDPerformanceMonitor, glBeginPerfMonitor), "glBeginPerfMonitorAMD" },
    { offsetof(AMDPerformanceMonitor, glDeletePerfMonitors), "glDeletePerfMonitorsAMD" },
    { offsetof(AMDPerformanceMonitor, glEndPerfMonitor), "glEndPerfMonitorAMD" },
    { offsetof(AMDPerformanceMonitor, glGenPerfMonitors), "glGenPerfMonitorsAMD" },
    { offsetof(AMDPerformanceMonitor, glGetPerfMonitorCounterData), "glGetPerfMonitorCounterDataAMD" },
    { offsetof(AMDPerformanceMonitor, glGetPerfMonitorCounterInfo), "glGetPerfMonitorCounterInfoAMD" },
    { offsetof(AMDPerformanceMonitor, glGetPerfMonitorCounterString), "glGetPerfMonitorCounterStringAMD" },
    { offsetof(AMDPerformanceMonitor, glGetPerfMonitorCounters), "glGetPerfMonitorCountersAMD" },
    { offsetof(AMDPerformanceMonitor, glGetPerfMonitorGroupString), "glGetPerfMonitorGroupStringAMD" },
    { offsetof(AMDPerformanceMonitor, glGetPerfMonitorGroups), "glGetPerfMonitorGroupsAMD" },
    { offsetof(AMDPerformanceMonitor, glSelectPerfMonitorCounters), "glSelectPerfMonitorCountersAMD" },
};

static const size_t __AMDPerformanceMonitorEntryCount = sizeof(__AMDPerformanceMonitorEntryPoints) /
    sizeof(__AMDPerformanceMonitorEntryPoints[0]);

static const EntryOffset __AMDSamplePositionsEntryPoints[] = {
    { offsetof(AMDSamplePositions, glSetMultisamplefv), "glSetMultisamplefvAMD" },
};

static const size_t __AMDSamplePositionsEntryCount = sizeof(__AMDSamplePositionsEntryPoints) /
    sizeof(__AMDSamplePositionsEntryPoints[0]);

static const EntryOffset __AMDSparseTextureEntryPoints[] = {
    { offsetof(AMDSparseTexture, glTexStorageSparse), "glTexStorageSparseAMD" },
    { offsetof(AMDSparseTexture, glTextureStorageSparse), "glTextureStorageSparseAMD" },
};

static const size_t __AMDSparseTextureEntryCount = sizeof(__AMDSparseTextureEntryPoints) /
    sizeof(__AMDSparseTextureEntryPoints[0]);

static const EntryOffset __AMDStencilOperationExtendedEntryPoints[] = {
    { offsetof(AMDStencilOperationExtended, glStencilOpValue), "glStencilOpValueAMD" },
};

static const size_t __AMDStencilOperationExtendedEntryCount = sizeof(__AMDStencilOperationExtendedEntryPoints) /
    sizeof(__AMDStencilOperationExtendedEntryPoints[0]);

static const EntryOffset __AMDVertexShaderTessellatorEntryPoints[] = {
    { offsetof(AMDVertexShaderTessellator, glTessellationFactor), "glTessellationFactorAMD" },
    { offsetof(AMDVertexShaderTessellator, glTessellationMode), "glTessellationModeAMD" },
};

static const size_t __AMDVertexShaderTessellatorEntryCount = sizeof(__AMDVertexShaderTessellatorEntryPoints) /
    sizeof(__AMDVertexShaderTessellatorEntryPoints[0]);

static const EntryOffset __APPLEElementArrayEntryPoints[] = {
    { offsetof(APPLEElementArray, glDrawElementArray), "glDrawElementArrayAPPLE" },
    { offsetof(APPLEElementArray, glDrawRangeElementArray), "glDrawRangeElementArrayAPPLE" },
    { offsetof(APPLEElementArray, glElementPointer), "glElementPointerAPPLE" },
    { offsetof(APPLEElementArray, glMultiDrawElementArray), "glMultiDrawElementArrayAPPLE" },
    { offsetof(APPLEElementArray, glMultiDrawRangeElementArray), "glMultiDrawRangeElementArrayAPPLE" },
};

static const size_t __APPLEElementArrayEntryCount = sizeof(__APPLEElementArrayEntryPoints) /
    sizeof(__APPLEElementArrayEntryPoints[0]);

static const EntryOffset __APPLEFenceEntryPoints[] = {
    { offsetof(APPLEFence, glDeleteFences), "glDeleteFencesAPPLE" },
    { offsetof(APPLEFence, glFinishFence), "glFinishFenceAPPLE" },
    { offsetof(APPLEFence, glFinishObject), "glFinishObjectAPPLE" },
    { offsetof(APPLEFence, glGenFences), "glGenFencesAPPLE" },
    { offsetof(APPLEFence, glIsFence), "glIsFenceAPPLE" },
    { offsetof(APPLEFence, glSetFence), "glSetFenceAPPLE" },
    { offsetof(APPLEFence, glTestFence), "glTestFenceAPPLE" },
    { offsetof(APPLEFence, glTestObject), "glTestObjectAPPLE" },
};

static const size_t __APPLEFenceEntryCount = sizeof(__APPLEFenceEntryPoints) /
    sizeof(__APPLEFenceEntryPoints[0]);

static const EntryOffset __APPLEFlushBufferRangeEntryPoints[] = {
    { offsetof(APPLEFlushBufferRange, glBufferParameteri), "glBufferParameteriAPPLE" },
    { offsetof(APPLEFlushBufferRange, glFlushMappedBufferRange), "glFlushMappedBufferRangeAPPLE" },
};

static const size_t __APPLEFlushBufferRangeEntryCount = sizeof(__APPLEFlushBufferRangeEntryPoints) /
    sizeof(__APPLEFlushBufferRangeEntryPoints[0]);

static const EntryOffset __APPLEObjectPurgeableEntryPoints[] = {
    { offsetof(APPLEObjectPurgeable, glGetObjectParameteriv), "glGetObjectParameterivAPPLE" },
    { offsetof(APPLEObjectPurgeable, glObjectPurgeable), "glObjectPurgeableAPPLE" },
    { offsetof(APPLEObjectPurgeable, glObjectUnpurgeable), "glObjectUnpurgeableAPPLE" },
};

static const size_t __APPLEObjectPurgeableEntryCount = sizeof(__APPLEObjectPurgeableEntryPoints) /
    sizeof(__APPLEObjectPurgeableEntryPoints[0]);

static const EntryOffset __APPLETextureRangeEntryPoints[] = {
    { offsetof(APPLETextureRange, glGetTexParameterPointerv), "glGetTexParameterPointervAPPLE" },
    { offsetof(APPLETextureRange, glTextureRange), "glTextureRangeAPPLE" },
};

static const size_t __APPLETextureRangeEntryCount = sizeof(__APPLETextureRangeEntryPoints) /
    sizeof(__APPLETextureRangeEntryPoints[0]);

static const EntryOffset __APPLEVertexArrayObjectEntryPoints[] = {
    { offsetof(APPLEVertexArrayObject, glBindVertexArray), "glBindVertexArrayAPPLE" },
    { offsetof(APPLEVertexArrayObject, glDeleteVertexArrays), "glDeleteVertexArraysAPPLE" },
    { offsetof(APPLEVertexArrayObject, glGenVertexArrays), "glGenVertexArraysAPPLE" },
    { offsetof(APPLEVertexArrayObject, glIsVertexArray), "glIsVertexArrayAPPLE" },
};

static const size_t __APPLEVertexArrayObjectEntryCount = sizeof(__APPLEVertexArrayObjectEntryPoints) /
    sizeof(__APPLEVertexArrayObjectEntryPoints[0]);

static const EntryOffset __APPLEVertexArrayRangeEntryPoints[] = {
    { offsetof(APPLEVertexArrayRange, glFlushVertexArrayRange), "glFlushVertexArrayRangeAPPLE" },
    { offsetof(APPLEVertexArrayRange, glVertexArrayParameteri), "glVertexArrayParameteriAPPLE" },
    { offsetof(APPLEVertexArrayRange, glVertexArrayRange), "glVertexArrayRangeAPPLE" },
};

static const size_t __APPLEVertexArrayRangeEntryCount = sizeof(__APPLEVertexArrayRangeEntryPoints) /
    sizeof(__APPLEVertexArrayRangeEntryPoints[0]);

static const EntryOffset __APPLEVertexProgramEvaluatorsEntryPoints[] = {
    { offsetof(APPLEVertexProgramEvaluators, glDisableVertexAttrib), "glDisableVertexAttribAPPLE" },
    { offsetof(APPLEVertexProgramEvaluators, glEnableVertexAttrib), "glEnableVertexAttribAPPLE" },
    { offsetof(APPLEVertexProgramEvaluators, glIsVertexAttribEnabled), "glIsVertexAttribEnabledAPPLE" },
    { offsetof(APPLEVertexProgramEvaluators, glMapVertexAttrib1d), "glMapVertexAttrib1dAPPLE" },
    { offsetof(APPLEVertexProgramEvaluators, glMapVertexAttrib1f), "glMapVertexAttrib1fAPPLE" },
    { offsetof(APPLEVertexProgramEvaluators, glMapVertexAttrib2d), "glMapVertexAttrib2dAPPLE" },
    { offsetof(APPLEVertexProgramEvaluators, glMapVertexAttrib2f), "glMapVertexAttrib2fAPPLE" },
};

static const size_t __APPLEVertexProgramEvaluatorsEntryCount = sizeof(__APPLEVertexProgramEvaluatorsEntryPoints) /
    sizeof(__APPLEVertexProgramEvaluatorsEntryPoints[0]);

static const EntryOffset __ARBES32CompatibilityEntryPoints[] = {
    { offsetof(ARBES32Compatibility, glPrimitiveBoundingBox), "glPrimitiveBoundingBoxARB" },
};

static const size_t __ARBES32CompatibilityEntryCount = sizeof(__ARBES32CompatibilityEntryPoints) /
    sizeof(__ARBES32CompatibilityEntryPoints[0]);

static const EntryOffset __ARBBindlessTextureEntryPoints[] = {
    { offsetof(ARBBindlessTexture, glGetImageHandle), "glGetImageHandleARB" },
    { offsetof(ARBBindlessTexture, glGetTextureHandle), "glGetTextureHandleARB" },
    { offsetof(ARBBindlessTexture, glGetTextureSamplerHandle), "glGetTextureSamplerHandleARB" },
    { offsetof(ARBBindlessTexture, glGetVertexAttribLui64v), "glGetVertexAttribLui64vARB" },
    { offsetof(ARBBindlessTexture, glIsImageHandleResident), "glIsImageHandleResidentARB" },
    { offsetof(ARBBindlessTexture, glIsTextureHandleResident), "glIsTextureHandleResidentARB" },
    { offsetof(ARBBindlessTexture, glMakeImageHandleNonResident), "glMakeImageHandleNonResidentARB" },
    { offsetof(ARBBindlessTexture, glMakeImageHandleResident), "glMakeImageHandleResidentARB" },
    { offsetof(ARBBindlessTexture, glMakeTextureHandleNonResident), "glMakeTextureHandleNonResidentARB" },
    { offsetof(ARBBindlessTexture, glMakeTextureHandleResident), "glMakeTextureHandleResidentARB" },
    { offsetof(ARBBindlessTexture, glProgramUniformHandleui64), "glProgramUniformHandleui64ARB" },
    { offsetof(ARBBindlessTexture, glProgramUniformHandleui64v), "glProgramUniformHandleui64vARB" },
    { offsetof(ARBBindlessTexture, glUniformHandleui64), "glUniformHandleui64ARB" },
    { offsetof(ARBBindlessTexture, glUniformHandleui64v), "glUniformHandleui64vARB" },
    { offsetof(ARBBindlessTexture, glVertexAttribL1ui64), "glVertexAttribL1ui64ARB" },
    { offsetof(ARBBindlessTexture, glVertexAttribL1ui64v), "glVertexAttribL1ui64vARB" },
};

static const size_t __ARBBindlessTextureEntryCount = sizeof(__ARBBindlessTextureEntryPoints) /
    sizeof(__ARBBindlessTextureEntryPoints[0]);

static const EntryOffset __ARBClEventEntryPoints[] = {
    { offsetof(ARBClEvent, glCreateSyncFromCLevent), "glCreateSyncFromCLeventARB" },
};

static const size_t __ARBClEventEntryCount = sizeof(__ARBClEventEntryPoints) /
    sizeof(__ARBClEventEntryPoints[0]);

static const EntryOffset __ARBColorBufferFloatEntryPoints[] = {
    { offsetof(ARBColorBufferFloat, glClampColor), "glClampColorARB" },
};

static const size_t __ARBColorBufferFloatEntryCount = sizeof(__ARBColorBufferFloatEntryPoints) /
    sizeof(__ARBColorBufferFloatEntryPoints[0]);

static const EntryOffset __ARBComputeVariableGroupSizeEntryPoints[] = {
    { offsetof(ARBComputeVariableGroupSize, glDispatchComputeGroupSize), "glDispatchComputeGroupSizeARB" },
};

static const size_t __ARBComputeVariableGroupSizeEntryCount = sizeof(__ARBComputeVariableGroupSizeEntryPoints) /
    sizeof(__ARBComputeVariableGroupSizeEntryPoints[0]);

static const EntryOffset __ARBDebugOutputEntryPoints[] = {
    { offsetof(ARBDebugOutput, glDebugMessageCallback), "glDebugMessageCallbackARB" },
    { offsetof(ARBDebugOutput, glDebugMessageControl), "glDebugMessageControlARB" },
    { offsetof(ARBDebugOutput, glDebugMessageInsert), "glDebugMessageInsertARB" },
    { offsetof(ARBDebugOutput, glGetDebugMessageLog), "glGetDebugMessageLogARB" },
};

static const size_t __ARBDebugOutputEntryCount = sizeof(__ARBDebugOutputEntryPoints) /
    sizeof(__ARBDebugOutputEntryPoints[0]);

static const EntryOffset __ARBDrawBuffersEntryPoints[] = {
    { offsetof(ARBDrawBuffers, glDrawBuffers), "glDrawBuffersARB" },
};

static const size_t __ARBDrawBuffersEntryCount = sizeof(__ARBDrawBuffersEntryPoints) /
    sizeof(__ARBDrawBuffersEntryPoints[0]);

static const EntryOffset __ARBDrawBuffersBlendEntryPoints[] = {
    { offsetof(ARBDrawBuffersBlend, glBlendEquationSeparatei), "glBlendEquationSeparateiARB" },
    { offsetof(ARBDrawBuffersBlend, glBlendEquationi), "glBlendEquationiARB" },
    { offsetof(ARBDrawBuffersBlend, glBlendFuncSeparatei), "glBlendFuncSeparateiARB" },
    { offsetof(ARBDrawBuffersBlend, glBlendFunci), "glBlendFunciARB" },
};

static const size_t __ARBDrawBuffersBlendEntryCount = sizeof(__ARBDrawBuffersBlendEntryPoints) /
    sizeof(__ARBDrawBuffersBlendEntryPoints[0]);

static const EntryOffset __ARBDrawInstancedEntryPoints[] = {
    { offsetof(ARBDrawInstanced, glDrawArraysInstanced), "glDrawArraysInstancedARB" },
    { offsetof(ARBDrawInstanced, glDrawElementsInstanced), "glDrawElementsInstancedARB" },
};

static const size_t __ARBDrawInstancedEntryCount = sizeof(__ARBDrawInstancedEntryPoints) /
    sizeof(__ARBDrawInstancedEntryPoints[0]);

static const EntryOffset __ARBFragmentProgramEntryPoints[] = {
    { offsetof(ARBFragmentProgram, glBindProgram), "glBindProgramARB" },
    { offsetof(ARBFragmentProgram, glDeletePrograms), "glDeleteProgramsARB" },
    { offsetof(ARBFragmentProgram, glGenPrograms), "glGenProgramsARB" },
    { offsetof(ARBFragmentProgram, glGetProgramEnvParameterdv), "glGetProgramEnvParameterdvARB" },
    { offsetof(ARBFragmentProgram, glGetProgramEnvParameterfv), "glGetProgramEnvParameterfvARB" },
    { offsetof(ARBFragmentProgram, glGetProgramLocalParameterdv), "glGetProgramLocalParameterdvARB" },
    { offsetof(ARBFragmentProgram, glGetProgramLocalParameterfv), "glGetProgramLocalParameterfvARB" },
    { offsetof(ARBFragmentProgram, glGetProgramString), "glGetProgramStringARB" },
    { offsetof(ARBFragmentProgram, glGetProgramiv), "glGetProgramivARB" },
    { offsetof(ARBFragmentProgram, glIsProgram), "glIsProgramARB" },
    { offsetof(ARBFragmentProgram, glProgramEnvParameter4d), "glProgramEnvParameter4dARB" },
    { offsetof(ARBFragmentProgram, glProgramEnvParameter4dv), "glProgramEnvParameter4dvARB" },
    { offsetof(ARBFragmentProgram, glProgramEnvParameter4f), "glProgramEnvParameter4fARB" },
    { offsetof(ARBFragmentProgram, glProgramEnvParameter4fv), "glProgramEnvParameter4fvARB" },
    { offsetof(ARBFragmentProgram, glProgramLocalParameter4d), "glProgramLocalParameter4dARB" },
    { offsetof(ARBFragmentProgram, glProgramLocalParameter4dv), "glProgramLocalParameter4dvARB" },
    { offsetof(ARBFragmentProgram, glProgramLocalParameter4f), "glProgramLocalParameter4fARB" },
    { offsetof(ARBFragmentProgram, glProgramLocalParameter4fv), "glProgramLocalParameter4fvARB" },
    { offsetof(ARBFragmentProgram, glProgramString), "glProgramStringARB" },
};

static const size_t __ARBFragmentProgramEntryCount = sizeof(__ARBFragmentProgramEntryPoints) /
    sizeof(__ARBFragmentProgramEntryPoints[0]);

static const EntryOffset __ARBGeometrySHADER4EntryPoints[] = {
    { offsetof(ARBGeometrySHADER4, glFramebufferTexture), "glFramebufferTextureARB" },
    { offsetof(ARBGeometrySHADER4, glFramebufferTextureFace), "glFramebufferTextureFaceARB" },
    { offsetof(ARBGeometrySHADER4, glFramebufferTextureLayer), "glFramebufferTextureLayerARB" },
    { offsetof(ARBGeometrySHADER4, glProgramParameteri), "glProgramParameteriARB" },
};

static const size_t __ARBGeometrySHADER4EntryCount = sizeof(__ARBGeometrySHADER4EntryPoints) /
    sizeof(__ARBGeometrySHADER4EntryPoints[0]);

static const EntryOffset __ARBGlSpirvEntryPoints[] = {
    { offsetof(ARBGlSpirv, glSpecializeShader), "glSpecializeShaderARB" },
};

static const size_t __ARBGlSpirvEntryCount = sizeof(__ARBGlSpirvEntryPoints) /
    sizeof(__ARBGlSpirvEntryPoints[0]);

static const EntryOffset __ARBGPUShaderInt64EntryPoints[] = {
    { offsetof(ARBGPUShaderInt64, glGetUniformi64v), "glGetUniformi64vARB" },
    { offsetof(ARBGPUShaderInt64, glGetUniformui64v), "glGetUniformui64vARB" },
    { offsetof(ARBGPUShaderInt64, glGetnUniformi64v), "glGetnUniformi64vARB" },
    { offsetof(ARBGPUShaderInt64, glGetnUniformui64v), "glGetnUniformui64vARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform1i64), "glProgramUniform1i64ARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform1i64v), "glProgramUniform1i64vARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform1ui64), "glProgramUniform1ui64ARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform1ui64v), "glProgramUniform1ui64vARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform2i64), "glProgramUniform2i64ARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform2i64v), "glProgramUniform2i64vARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform2ui64), "glProgramUniform2ui64ARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform2ui64v), "glProgramUniform2ui64vARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform3i64), "glProgramUniform3i64ARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform3i64v), "glProgramUniform3i64vARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform3ui64), "glProgramUniform3ui64ARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform3ui64v), "glProgramUniform3ui64vARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform4i64), "glProgramUniform4i64ARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform4i64v), "glProgramUniform4i64vARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform4ui64), "glProgramUniform4ui64ARB" },
    { offsetof(ARBGPUShaderInt64, glProgramUniform4ui64v), "glProgramUniform4ui64vARB" },
    { offsetof(ARBGPUShaderInt64, glUniform1i64), "glUniform1i64ARB" },
    { offsetof(ARBGPUShaderInt64, glUniform1i64v), "glUniform1i64vARB" },
    { offsetof(ARBGPUShaderInt64, glUniform1ui64), "glUniform1ui64ARB" },
    { offsetof(ARBGPUShaderInt64, glUniform1ui64v), "glUniform1ui64vARB" },
    { offsetof(ARBGPUShaderInt64, glUniform2i64), "glUniform2i64ARB" },
    { offsetof(ARBGPUShaderInt64, glUniform2i64v), "glUniform2i64vARB" },
    { offsetof(ARBGPUShaderInt64, glUniform2ui64), "glUniform2ui64ARB" },
    { offsetof(ARBGPUShaderInt64, glUniform2ui64v), "glUniform2ui64vARB" },
    { offsetof(ARBGPUShaderInt64, glUniform3i64), "glUniform3i64ARB" },
    { offsetof(ARBGPUShaderInt64, glUniform3i64v), "glUniform3i64vARB" },
    { offsetof(ARBGPUShaderInt64, glUniform3ui64), "glUniform3ui64ARB" },
    { offsetof(ARBGPUShaderInt64, glUniform3ui64v), "glUniform3ui64vARB" },
    { offsetof(ARBGPUShaderInt64, glUniform4i64), "glUniform4i64ARB" },
    { offsetof(ARBGPUShaderInt64, glUniform4i64v), "glUniform4i64vARB" },
    { offsetof(ARBGPUShaderInt64, glUniform4ui64), "glUniform4ui64ARB" },
    { offsetof(ARBGPUShaderInt64, glUniform4ui64v), "glUniform4ui64vARB" },
};

static const size_t __ARBGPUShaderInt64EntryCount = sizeof(__ARBGPUShaderInt64EntryPoints) /
    sizeof(__ARBGPUShaderInt64EntryPoints[0]);

static const EntryOffset __ARBIndirectParametersEntryPoints[] = {
    { offsetof(ARBIndirectParameters, glMultiDrawArraysIndirectCount), "glMultiDrawArraysIndirectCountARB" },
    { offsetof(ARBIndirectParameters, glMultiDrawElementsIndirectCount), "glMultiDrawElementsIndirectCountARB" },
};

static const size_t __ARBIndirectParametersEntryCount = sizeof(__ARBIndirectParametersEntryPoints) /
    sizeof(__ARBIndirectParametersEntryPoints[0]);

static const EntryOffset __ARBInstancedArraysEntryPoints[] = {
    { offsetof(ARBInstancedArrays, glVertexAttribDivisor), "glVertexAttribDivisorARB" },
};

static const size_t __ARBInstancedArraysEntryCount = sizeof(__ARBInstancedArraysEntryPoints) /
    sizeof(__ARBInstancedArraysEntryPoints[0]);

static const EntryOffset __ARBMatrixPaletteEntryPoints[] = {
    { offsetof(ARBMatrixPalette, glCurrentPaletteMatrix), "glCurrentPaletteMatrixARB" },
    { offsetof(ARBMatrixPalette, glMatrixIndexPointer), "glMatrixIndexPointerARB" },
    { offsetof(ARBMatrixPalette, glMatrixIndexubv), "glMatrixIndexubvARB" },
    { offsetof(ARBMatrixPalette, glMatrixIndexuiv), "glMatrixIndexuivARB" },
    { offsetof(ARBMatrixPalette, glMatrixIndexusv), "glMatrixIndexusvARB" },
};

static const size_t __ARBMatrixPaletteEntryCount = sizeof(__ARBMatrixPaletteEntryPoints) /
    sizeof(__ARBMatrixPaletteEntryPoints[0]);

static const EntryOffset __ARBMultisampleEntryPoints[] = {
    { offsetof(ARBMultisample, glSampleCoverage), "glSampleCoverageARB" },
};

static const size_t __ARBMultisampleEntryCount = sizeof(__ARBMultisampleEntryPoints) /
    sizeof(__ARBMultisampleEntryPoints[0]);

static const EntryOffset __ARBMultitextureEntryPoints[] = {
    { offsetof(ARBMultitexture, glActiveTexture), "glActiveTextureARB" },
    { offsetof(ARBMultitexture, glClientActiveTexture), "glClientActiveTextureARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord1d), "glMultiTexCoord1dARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord1dv), "glMultiTexCoord1dvARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord1f), "glMultiTexCoord1fARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord1fv), "glMultiTexCoord1fvARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord1i), "glMultiTexCoord1iARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord1iv), "glMultiTexCoord1ivARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord1s), "glMultiTexCoord1sARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord1sv), "glMultiTexCoord1svARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord2d), "glMultiTexCoord2dARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord2dv), "glMultiTexCoord2dvARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord2f), "glMultiTexCoord2fARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord2fv), "glMultiTexCoord2fvARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord2i), "glMultiTexCoord2iARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord2iv), "glMultiTexCoord2ivARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord2s), "glMultiTexCoord2sARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord2sv), "glMultiTexCoord2svARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord3d), "glMultiTexCoord3dARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord3dv), "glMultiTexCoord3dvARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord3f), "glMultiTexCoord3fARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord3fv), "glMultiTexCoord3fvARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord3i), "glMultiTexCoord3iARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord3iv), "glMultiTexCoord3ivARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord3s), "glMultiTexCoord3sARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord3sv), "glMultiTexCoord3svARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord4d), "glMultiTexCoord4dARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord4dv), "glMultiTexCoord4dvARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord4f), "glMultiTexCoord4fARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord4fv), "glMultiTexCoord4fvARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord4i), "glMultiTexCoord4iARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord4iv), "glMultiTexCoord4ivARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord4s), "glMultiTexCoord4sARB" },
    { offsetof(ARBMultitexture, glMultiTexCoord4sv), "glMultiTexCoord4svARB" },
};

static const size_t __ARBMultitextureEntryCount = sizeof(__ARBMultitextureEntryPoints) /
    sizeof(__ARBMultitextureEntryPoints[0]);

static const EntryOffset __ARBOcclusionQueryEntryPoints[] = {
    { offsetof(ARBOcclusionQuery, glBeginQuery), "glBeginQueryARB" },
    { offsetof(ARBOcclusionQuery, glDeleteQueries), "glDeleteQueriesARB" },
    { offsetof(ARBOcclusionQuery, glEndQuery), "glEndQueryARB" },
    { offsetof(ARBOcclusionQuery, glGenQueries), "glGenQueriesARB" },
    { offsetof(ARBOcclusionQuery, glGetQueryObjectiv), "glGetQueryObjectivARB" },
    { offsetof(ARBOcclusionQuery, glGetQueryObjectuiv), "glGetQueryObjectuivARB" },
    { offsetof(ARBOcclusionQuery, glGetQueryiv), "glGetQueryivARB" },
    { offsetof(ARBOcclusionQuery, glIsQuery), "glIsQueryARB" },
};

static const size_t __ARBOcclusionQueryEntryCount = sizeof(__ARBOcclusionQueryEntryPoints) /
    sizeof(__ARBOcclusionQueryEntryPoints[0]);

static const EntryOffset __ARBParallelShaderCompileEntryPoints[] = {
    { offsetof(ARBParallelShaderCompile, glMaxShaderCompilerThreads), "glMaxShaderCompilerThreadsARB" },
};

static const size_t __ARBParallelShaderCompileEntryCount = sizeof(__ARBParallelShaderCompileEntryPoints) /
    sizeof(__ARBParallelShaderCompileEntryPoints[0]);

static const EntryOffset __ARBPointParametersEntryPoints[] = {
    { offsetof(ARBPointParameters, glPointParameterf), "glPointParameterfARB" },
    { offsetof(ARBPointParameters, glPointParameterfv), "glPointParameterfvARB" },
};

static const size_t __ARBPointParametersEntryCount = sizeof(__ARBPointParametersEntryPoints) /
    sizeof(__ARBPointParametersEntryPoints[0]);

static const EntryOffset __ARBRobustnessEntryPoints[] = {
    { offsetof(ARBRobustness, glGetGraphicsResetStatus), "glGetGraphicsResetStatusARB" },
    { offsetof(ARBRobustness, glGetnCompressedTexImage), "glGetnCompressedTexImageARB" },
    { offsetof(ARBRobustness, glGetnTexImage), "glGetnTexImageARB" },
    { offsetof(ARBRobustness, glGetnUniformdv), "glGetnUniformdvARB" },
    { offsetof(ARBRobustness, glGetnUniformfv), "glGetnUniformfvARB" },
    { offsetof(ARBRobustness, glGetnUniformiv), "glGetnUniformivARB" },
    { offsetof(ARBRobustness, glGetnUniformuiv), "glGetnUniformuivARB" },
    { offsetof(ARBRobustness, glReadnPixels), "glReadnPixelsARB" },
};

static const size_t __ARBRobustnessEntryCount = sizeof(__ARBRobustnessEntryPoints) /
    sizeof(__ARBRobustnessEntryPoints[0]);

static const EntryOffset __ARBSampleLocationsEntryPoints[] = {
    { offsetof(ARBSampleLocations, glEvaluateDepthValues), "glEvaluateDepthValuesARB" },
    { offsetof(ARBSampleLocations, glFramebufferSampleLocationsfv), "glFramebufferSampleLocationsfvARB" },
    { offsetof(ARBSampleLocations, glNamedFramebufferSampleLocationsfv), "glNamedFramebufferSampleLocationsfvARB" },
};

static const size_t __ARBSampleLocationsEntryCount = sizeof(__ARBSampleLocationsEntryPoints) /
    sizeof(__ARBSampleLocationsEntryPoints[0]);

static const EntryOffset __ARBSampleShadingEntryPoints[] = {
    { offsetof(ARBSampleShading, glMinSampleShading), "glMinSampleShadingARB" },
};

static const size_t __ARBSampleShadingEntryCount = sizeof(__ARBSampleShadingEntryPoints) /
    sizeof(__ARBSampleShadingEntryPoints[0]);

static const EntryOffset __ARBShaderObjectsEntryPoints[] = {
    { offsetof(ARBShaderObjects, glAttachObject), "glAttachObjectARB" },
    { offsetof(ARBShaderObjects, glCompileShader), "glCompileShaderARB" },
    { offsetof(ARBShaderObjects, glCreateProgramObject), "glCreateProgramObjectARB" },
    { offsetof(ARBShaderObjects, glCreateShaderObject), "glCreateShaderObjectARB" },
    { offsetof(ARBShaderObjects, glDeleteObject), "glDeleteObjectARB" },
    { offsetof(ARBShaderObjects, glDetachObject), "glDetachObjectARB" },
    { offsetof(ARBShaderObjects, glGetActiveUniform), "glGetActiveUniformARB" },
    { offsetof(ARBShaderObjects, glGetAttachedObjects), "glGetAttachedObjectsARB" },
    { offsetof(ARBShaderObjects, glGetHandle), "glGetHandleARB" },
    { offsetof(ARBShaderObjects, glGetInfoLog), "glGetInfoLogARB" },
    { offsetof(ARBShaderObjects, glGetObjectParameterfv), "glGetObjectParameterfvARB" },
    { offsetof(ARBShaderObjects, glGetObjectParameteriv), "glGetObjectParameterivARB" },
    { offsetof(ARBShaderObjects, glGetShaderSource), "glGetShaderSourceARB" },
    { offsetof(ARBShaderObjects, glGetUniformLocation), "glGetUniformLocationARB" },
    { offsetof(ARBShaderObjects, glGetUniformfv), "glGetUniformfvARB" },
    { offsetof(ARBShaderObjects, glGetUniformiv), "glGetUniformivARB" },
    { offsetof(ARBShaderObjects, glLinkProgram), "glLinkProgramARB" },
    { offsetof(ARBShaderObjects, glShaderSource), "glShaderSourceARB" },
    { offsetof(ARBShaderObjects, glUniform1f), "glUniform1fARB" },
    { offsetof(ARBShaderObjects, glUniform1fv), "glUniform1fvARB" },
    { offsetof(ARBShaderObjects, glUniform1i), "glUniform1iARB" },
    { offsetof(ARBShaderObjects, glUniform1iv), "glUniform1ivARB" },
    { offsetof(ARBShaderObjects, glUniform2f), "glUniform2fARB" },
    { offsetof(ARBShaderObjects, glUniform2fv), "glUniform2fvARB" },
    { offsetof(ARBShaderObjects, glUniform2i), "glUniform2iARB" },
    { offsetof(ARBShaderObjects, glUniform2iv), "glUniform2ivARB" },
    { offsetof(ARBShaderObjects, glUniform3f), "glUniform3fARB" },
    { offsetof(ARBShaderObjects, glUniform3fv), "glUniform3fvARB" },
    { offsetof(ARBShaderObjects, glUniform3i), "glUniform3iARB" },
    { offsetof(ARBShaderObjects, glUniform3iv), "glUniform3ivARB" },
    { offsetof(ARBShaderObjects, glUniform4f), "glUniform4fARB" },
    { offsetof(ARBShaderObjects, glUniform4fv), "glUniform4fvARB" },
    { offsetof(ARBShaderObjects, glUniform4i), "glUniform4iARB" },
    { offsetof(ARBShaderObjects, glUniform4iv), "glUniform4ivARB" },
    { offsetof(ARBShaderObjects, glUniformMatrix2fv), "glUniformMatrix2fvARB" },
    { offsetof(ARBShaderObjects, glUniformMatrix3fv), "glUniformMatrix3fvARB" },
    { offsetof(ARBShaderObjects, glUniformMatrix4fv), "glUniformMatrix4fvARB" },
    { offsetof(ARBShaderObjects, glUseProgramObject), "glUseProgramObjectARB" },
    { offsetof(ARBShaderObjects, glValidateProgram), "glValidateProgramARB" },
};

static const size_t __ARBShaderObjectsEntryCount = sizeof(__ARBShaderObjectsEntryPoints) /
    sizeof(__ARBShaderObjectsEntryPoints[0]);

static const EntryOffset __ARBShadingLanguageIncludeEntryPoints[] = {
    { offsetof(ARBShadingLanguageInclude, glCompileShaderInclude), "glCompileShaderIncludeARB" },
    { offsetof(ARBShadingLanguageInclude, glDeleteNamedString), "glDeleteNamedStringARB" },
    { offsetof(ARBShadingLanguageInclude, glGetNamedString), "glGetNamedStringARB" },
    { offsetof(ARBShadingLanguageInclude, glGetNamedStringiv), "glGetNamedStringivARB" },
    { offsetof(ARBShadingLanguageInclude, glIsNamedString), "glIsNamedStringARB" },
    { offsetof(ARBShadingLanguageInclude, glNamedString), "glNamedStringARB" },
};

static const size_t __ARBShadingLanguageIncludeEntryCount = sizeof(__ARBShadingLanguageIncludeEntryPoints) /
    sizeof(__ARBShadingLanguageIncludeEntryPoints[0]);

static const EntryOffset __ARBSparseBufferEntryPoints[] = {
    { offsetof(ARBSparseBuffer, glBufferPageCommitment), "glBufferPageCommitmentARB" },
    { offsetof(ARBSparseBuffer, glNamedBufferPageCommitmentARB), "glNamedBufferPageCommitmentARB" },
    { offsetof(ARBSparseBuffer, glNamedBufferPageCommitmentEXT), "glNamedBufferPageCommitmentEXT" },
};

static const size_t __ARBSparseBufferEntryCount = sizeof(__ARBSparseBufferEntryPoints) /
    sizeof(__ARBSparseBufferEntryPoints[0]);

static const EntryOffset __ARBSparseTextureEntryPoints[] = {
    { offsetof(ARBSparseTexture, glTexPageCommitment), "glTexPageCommitmentARB" },
};

static const size_t __ARBSparseTextureEntryCount = sizeof(__ARBSparseTextureEntryPoints) /
    sizeof(__ARBSparseTextureEntryPoints[0]);

static const EntryOffset __ARBTextureBufferObjectEntryPoints[] = {
    { offsetof(ARBTextureBufferObject, glTexBuffer), "glTexBufferARB" },
};

static const size_t __ARBTextureBufferObjectEntryCount = sizeof(__ARBTextureBufferObjectEntryPoints) /
    sizeof(__ARBTextureBufferObjectEntryPoints[0]);

static const EntryOffset __ARBTextureCompressionEntryPoints[] = {
    { offsetof(ARBTextureCompression, glCompressedTexImage1D), "glCompressedTexImage1DARB" },
    { offsetof(ARBTextureCompression, glCompressedTexImage2D), "glCompressedTexImage2DARB" },
    { offsetof(ARBTextureCompression, glCompressedTexImage3D), "glCompressedTexImage3DARB" },
    { offsetof(ARBTextureCompression, glCompressedTexSubImage1D), "glCompressedTexSubImage1DARB" },
    { offsetof(ARBTextureCompression, glCompressedTexSubImage2D), "glCompressedTexSubImage2DARB" },
    { offsetof(ARBTextureCompression, glCompressedTexSubImage3D), "glCompressedTexSubImage3DARB" },
    { offsetof(ARBTextureCompression, glGetCompressedTexImage), "glGetCompressedTexImageARB" },
};

static const size_t __ARBTextureCompressionEntryCount = sizeof(__ARBTextureCompressionEntryPoints) /
    sizeof(__ARBTextureCompressionEntryPoints[0]);

static const EntryOffset __ARBTransposeMatrixEntryPoints[] = {
    { offsetof(ARBTransposeMatrix, glLoadTransposeMatrixd), "glLoadTransposeMatrixdARB" },
    { offsetof(ARBTransposeMatrix, glLoadTransposeMatrixf), "glLoadTransposeMatrixfARB" },
    { offsetof(ARBTransposeMatrix, glMultTransposeMatrixd), "glMultTransposeMatrixdARB" },
    { offsetof(ARBTransposeMatrix, glMultTransposeMatrixf), "glMultTransposeMatrixfARB" },
};

static const size_t __ARBTransposeMatrixEntryCount = sizeof(__ARBTransposeMatrixEntryPoints) /
    sizeof(__ARBTransposeMatrixEntryPoints[0]);

static const EntryOffset __ARBVertexBlendEntryPoints[] = {
    { offsetof(ARBVertexBlend, glVertexBlend), "glVertexBlendARB" },
    { offsetof(ARBVertexBlend, glWeightPointer), "glWeightPointerARB" },
    { offsetof(ARBVertexBlend, glWeightbv), "glWeightbvARB" },
    { offsetof(ARBVertexBlend, glWeightdv), "glWeightdvARB" },
    { offsetof(ARBVertexBlend, glWeightfv), "glWeightfvARB" },
    { offsetof(ARBVertexBlend, glWeightiv), "glWeightivARB" },
    { offsetof(ARBVertexBlend, glWeightsv), "glWeightsvARB" },
    { offsetof(ARBVertexBlend, glWeightubv), "glWeightubvARB" },
    { offsetof(ARBVertexBlend, glWeightuiv), "glWeightuivARB" },
    { offsetof(ARBVertexBlend, glWeightusv), "glWeightusvARB" },
};

static const size_t __ARBVertexBlendEntryCount = sizeof(__ARBVertexBlendEntryPoints) /
    sizeof(__ARBVertexBlendEntryPoints[0]);

static const EntryOffset __ARBVertexBufferObjectEntryPoints[] = {
    { offsetof(ARBVertexBufferObject, glBindBuffer), "glBindBufferARB" },
    { offsetof(ARBVertexBufferObject, glBufferData), "glBufferDataARB" },
    { offsetof(ARBVertexBufferObject, glBufferSubData), "glBufferSubDataARB" },
    { offsetof(ARBVertexBufferObject, glDeleteBuffers), "glDeleteBuffersARB" },
    { offsetof(ARBVertexBufferObject, glGenBuffers), "glGenBuffersARB" },
    { offsetof(ARBVertexBufferObject, glGetBufferParameteriv), "glGetBufferParameterivARB" },
    { offsetof(ARBVertexBufferObject, glGetBufferPointerv), "glGetBufferPointervARB" },
    { offsetof(ARBVertexBufferObject, glGetBufferSubData), "glGetBufferSubDataARB" },
    { offsetof(ARBVertexBufferObject, glIsBuffer), "glIsBufferARB" },
    { offsetof(ARBVertexBufferObject, glMapBuffer), "glMapBufferARB" },
    { offsetof(ARBVertexBufferObject, glUnmapBuffer), "glUnmapBufferARB" },
};

static const size_t __ARBVertexBufferObjectEntryCount = sizeof(__ARBVertexBufferObjectEntryPoints) /
    sizeof(__ARBVertexBufferObjectEntryPoints[0]);

static const EntryOffset __ARBVertexProgramEntryPoints[] = {
    { offsetof(ARBVertexProgram, glDisableVertexAttribArray), "glDisableVertexAttribArrayARB" },
    { offsetof(ARBVertexProgram, glEnableVertexAttribArray), "glEnableVertexAttribArrayARB" },
    { offsetof(ARBVertexProgram, glGetVertexAttribPointerv), "glGetVertexAttribPointervARB" },
    { offsetof(ARBVertexProgram, glGetVertexAttribdv), "glGetVertexAttribdvARB" },
    { offsetof(ARBVertexProgram, glGetVertexAttribfv), "glGetVertexAttribfvARB" },
    { offsetof(ARBVertexProgram, glGetVertexAttribiv), "glGetVertexAttribivARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib1d), "glVertexAttrib1dARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib1dv), "glVertexAttrib1dvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib1f), "glVertexAttrib1fARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib1fv), "glVertexAttrib1fvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib1s), "glVertexAttrib1sARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib1sv), "glVertexAttrib1svARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib2d), "glVertexAttrib2dARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib2dv), "glVertexAttrib2dvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib2f), "glVertexAttrib2fARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib2fv), "glVertexAttrib2fvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib2s), "glVertexAttrib2sARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib2sv), "glVertexAttrib2svARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib3d), "glVertexAttrib3dARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib3dv), "glVertexAttrib3dvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib3f), "glVertexAttrib3fARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib3fv), "glVertexAttrib3fvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib3s), "glVertexAttrib3sARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib3sv), "glVertexAttrib3svARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4Nbv), "glVertexAttrib4NbvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4Niv), "glVertexAttrib4NivARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4Nsv), "glVertexAttrib4NsvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4Nub), "glVertexAttrib4NubARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4Nubv), "glVertexAttrib4NubvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4Nuiv), "glVertexAttrib4NuivARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4Nusv), "glVertexAttrib4NusvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4bv), "glVertexAttrib4bvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4d), "glVertexAttrib4dARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4dv), "glVertexAttrib4dvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4f), "glVertexAttrib4fARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4fv), "glVertexAttrib4fvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4iv), "glVertexAttrib4ivARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4s), "glVertexAttrib4sARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4sv), "glVertexAttrib4svARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4ubv), "glVertexAttrib4ubvARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4uiv), "glVertexAttrib4uivARB" },
    { offsetof(ARBVertexProgram, glVertexAttrib4usv), "glVertexAttrib4usvARB" },
    { offsetof(ARBVertexProgram, glVertexAttribPointer), "glVertexAttribPointerARB" },
};

static const size_t __ARBVertexProgramEntryCount = sizeof(__ARBVertexProgramEntryPoints) /
    sizeof(__ARBVertexProgramEntryPoints[0]);

static const EntryOffset __ARBVertexShaderEntryPoints[] = {
    { offsetof(ARBVertexShader, glBindAttribLocation), "glBindAttribLocationARB" },
    { offsetof(ARBVertexShader, glGetActiveAttrib), "glGetActiveAttribARB" },
    { offsetof(ARBVertexShader, glGetAttribLocation), "glGetAttribLocationARB" },
};

static const size_t __ARBVertexShaderEntryCount = sizeof(__ARBVertexShaderEntryPoints) /
    sizeof(__ARBVertexShaderEntryPoints[0]);

static const EntryOffset __ARBViewportArrayEntryPoints[] = {
    { offsetof(ARBViewportArray, glDepthRangeArraydv), "glDepthRangeArraydvNV" },
    { offsetof(ARBViewportArray, glDepthRangeIndexedd), "glDepthRangeIndexeddNV" },
};

static const size_t __ARBViewportArrayEntryCount = sizeof(__ARBViewportArrayEntryPoints) /
    sizeof(__ARBViewportArrayEntryPoints[0]);

static const EntryOffset __ARBWindowPosEntryPoints[] = {
    { offsetof(ARBWindowPos, glWindowPos2d), "glWindowPos2dARB" },
    { offsetof(ARBWindowPos, glWindowPos2dv), "glWindowPos2dvARB" },
    { offsetof(ARBWindowPos, glWindowPos2f), "glWindowPos2fARB" },
    { offsetof(ARBWindowPos, glWindowPos2fv), "glWindowPos2fvARB" },
    { offsetof(ARBWindowPos, glWindowPos2i), "glWindowPos2iARB" },
    { offsetof(ARBWindowPos, glWindowPos2iv), "glWindowPos2ivARB" },
    { offsetof(ARBWindowPos, glWindowPos2s), "glWindowPos2sARB" },
    { offsetof(ARBWindowPos, glWindowPos2sv), "glWindowPos2svARB" },
    { offsetof(ARBWindowPos, glWindowPos3d), "glWindowPos3dARB" },
    { offsetof(ARBWindowPos, glWindowPos3dv), "glWindowPos3dvARB" },
    { offsetof(ARBWindowPos, glWindowPos3f), "glWindowPos3fARB" },
    { offsetof(ARBWindowPos, glWindowPos3fv), "glWindowPos3fvARB" },
    { offsetof(ARBWindowPos, glWindowPos3i), "glWindowPos3iARB" },
    { offsetof(ARBWindowPos, glWindowPos3iv), "glWindowPos3ivARB" },
    { offsetof(ARBWindowPos, glWindowPos3s), "glWindowPos3sARB" },
    { offsetof(ARBWindowPos, glWindowPos3sv), "glWindowPos3svARB" },
};

static const size_t __ARBWindowPosEntryCount = sizeof(__ARBWindowPosEntryPoints) /
    sizeof(__ARBWindowPosEntryPoints[0]);

static const EntryOffset __ATIDrawBuffersEntryPoints[] = {
    { offsetof(ATIDrawBuffers, glDrawBuffers), "glDrawBuffersATI" },
};

static const size_t __ATIDrawBuffersEntryCount = sizeof(__ATIDrawBuffersEntryPoints) /
    sizeof(__ATIDrawBuffersEntryPoints[0]);

static const EntryOffset __ATIElementArrayEntryPoints[] = {
    { offsetof(ATIElementArray, glDrawElementArray), "glDrawElementArrayATI" },
    { offsetof(ATIElementArray, glDrawRangeElementArray), "glDrawRangeElementArrayATI" },
    { offsetof(ATIElementArray, glElementPointer), "glElementPointerATI" },
};

static const size_t __ATIElementArrayEntryCount = sizeof(__ATIElementArrayEntryPoints) /
    sizeof(__ATIElementArrayEntryPoints[0]);

static const EntryOffset __ATIEnvmapBumpmapEntryPoints[] = {
    { offsetof(ATIEnvmapBumpmap, glGetTexBumpParameterfv), "glGetTexBumpParameterfvATI" },
    { offsetof(ATIEnvmapBumpmap, glGetTexBumpParameteriv), "glGetTexBumpParameterivATI" },
    { offsetof(ATIEnvmapBumpmap, glTexBumpParameterfv), "glTexBumpParameterfvATI" },
    { offsetof(ATIEnvmapBumpmap, glTexBumpParameteriv), "glTexBumpParameterivATI" },
};

static const size_t __ATIEnvmapBumpmapEntryCount = sizeof(__ATIEnvmapBumpmapEntryPoints) /
    sizeof(__ATIEnvmapBumpmapEntryPoints[0]);

static const EntryOffset __ATIFragmentShaderEntryPoints[] = {
    { offsetof(ATIFragmentShader, glAlphaFragmentOp1), "glAlphaFragmentOp1ATI" },
    { offsetof(ATIFragmentShader, glAlphaFragmentOp2), "glAlphaFragmentOp2ATI" },
    { offsetof(ATIFragmentShader, glAlphaFragmentOp3), "glAlphaFragmentOp3ATI" },
    { offsetof(ATIFragmentShader, glBeginFragmentShader), "glBeginFragmentShaderATI" },
    { offsetof(ATIFragmentShader, glBindFragmentShader), "glBindFragmentShaderATI" },
    { offsetof(ATIFragmentShader, glColorFragmentOp1), "glColorFragmentOp1ATI" },
    { offsetof(ATIFragmentShader, glColorFragmentOp2), "glColorFragmentOp2ATI" },
    { offsetof(ATIFragmentShader, glColorFragmentOp3), "glColorFragmentOp3ATI" },
    { offsetof(ATIFragmentShader, glDeleteFragmentShader), "glDeleteFragmentShaderATI" },
    { offsetof(ATIFragmentShader, glEndFragmentShader), "glEndFragmentShaderATI" },
    { offsetof(ATIFragmentShader, glGenFragmentShaders), "glGenFragmentShadersATI" },
    { offsetof(ATIFragmentShader, glPassTexCoord), "glPassTexCoordATI" },
    { offsetof(ATIFragmentShader, glSampleMap), "glSampleMapATI" },
    { offsetof(ATIFragmentShader, glSetFragmentShaderConstant), "glSetFragmentShaderConstantATI" },
};

static const size_t __ATIFragmentShaderEntryCount = sizeof(__ATIFragmentShaderEntryPoints) /
    sizeof(__ATIFragmentShaderEntryPoints[0]);

static const EntryOffset __ATIMapObjectBufferEntryPoints[] = {
    { offsetof(ATIMapObjectBuffer, glMapObjectBuffer), "glMapObjectBufferATI" },
    { offsetof(ATIMapObjectBuffer, glUnmapObjectBuffer), "glUnmapObjectBufferATI" },
};

static const size_t __ATIMapObjectBufferEntryCount = sizeof(__ATIMapObjectBufferEntryPoints) /
    sizeof(__ATIMapObjectBufferEntryPoints[0]);

static const EntryOffset __ATIPnTrianglesEntryPoints[] = {
    { offsetof(ATIPnTriangles, glPNTrianglesf), "glPNTrianglesfATI" },
    { offsetof(ATIPnTriangles, glPNTrianglesi), "glPNTrianglesiATI" },
};

static const size_t __ATIPnTrianglesEntryCount = sizeof(__ATIPnTrianglesEntryPoints) /
    sizeof(__ATIPnTrianglesEntryPoints[0]);

static const EntryOffset __ATISeparateStencilEntryPoints[] = {
    { offsetof(ATISeparateStencil, glStencilFuncSeparate), "glStencilFuncSeparateATI" },
    { offsetof(ATISeparateStencil, glStencilOpSeparate), "glStencilOpSeparateATI" },
};

static const size_t __ATISeparateStencilEntryCount = sizeof(__ATISeparateStencilEntryPoints) /
    sizeof(__ATISeparateStencilEntryPoints[0]);

static const EntryOffset __ATIVertexArrayObjectEntryPoints[] = {
    { offsetof(ATIVertexArrayObject, glArrayObject), "glArrayObjectATI" },
    { offsetof(ATIVertexArrayObject, glFreeObjectBuffer), "glFreeObjectBufferATI" },
    { offsetof(ATIVertexArrayObject, glGetArrayObjectfv), "glGetArrayObjectfvATI" },
    { offsetof(ATIVertexArrayObject, glGetArrayObjectiv), "glGetArrayObjectivATI" },
    { offsetof(ATIVertexArrayObject, glGetObjectBufferfv), "glGetObjectBufferfvATI" },
    { offsetof(ATIVertexArrayObject, glGetObjectBufferiv), "glGetObjectBufferivATI" },
    { offsetof(ATIVertexArrayObject, glGetVariantArrayObjectfv), "glGetVariantArrayObjectfvATI" },
    { offsetof(ATIVertexArrayObject, glGetVariantArrayObjectiv), "glGetVariantArrayObjectivATI" },
    { offsetof(ATIVertexArrayObject, glIsObjectBuffer), "glIsObjectBufferATI" },
    { offsetof(ATIVertexArrayObject, glNewObjectBuffer), "glNewObjectBufferATI" },
    { offsetof(ATIVertexArrayObject, glUpdateObjectBuffer), "glUpdateObjectBufferATI" },
    { offsetof(ATIVertexArrayObject, glVariantArrayObject), "glVariantArrayObjectATI" },
};

static const size_t __ATIVertexArrayObjectEntryCount = sizeof(__ATIVertexArrayObjectEntryPoints) /
    sizeof(__ATIVertexArrayObjectEntryPoints[0]);

static const EntryOffset __ATIVertexAttribArrayObjectEntryPoints[] = {
    { offsetof(ATIVertexAttribArrayObject, glGetVertexAttribArrayObjectfv), "glGetVertexAttribArrayObjectfvATI" },
    { offsetof(ATIVertexAttribArrayObject, glGetVertexAttribArrayObjectiv), "glGetVertexAttribArrayObjectivATI" },
    { offsetof(ATIVertexAttribArrayObject, glVertexAttribArrayObject), "glVertexAttribArrayObjectATI" },
};

static const size_t __ATIVertexAttribArrayObjectEntryCount = sizeof(__ATIVertexAttribArrayObjectEntryPoints) /
    sizeof(__ATIVertexAttribArrayObjectEntryPoints[0]);

static const EntryOffset __ATIVertexStreamsEntryPoints[] = {
    { offsetof(ATIVertexStreams, glClientActiveVertexStream), "glClientActiveVertexStreamATI" },
    { offsetof(ATIVertexStreams, glNormalStream3b), "glNormalStream3bATI" },
    { offsetof(ATIVertexStreams, glNormalStream3bv), "glNormalStream3bvATI" },
    { offsetof(ATIVertexStreams, glNormalStream3d), "glNormalStream3dATI" },
    { offsetof(ATIVertexStreams, glNormalStream3dv), "glNormalStream3dvATI" },
    { offsetof(ATIVertexStreams, glNormalStream3f), "glNormalStream3fATI" },
    { offsetof(ATIVertexStreams, glNormalStream3fv), "glNormalStream3fvATI" },
    { offsetof(ATIVertexStreams, glNormalStream3i), "glNormalStream3iATI" },
    { offsetof(ATIVertexStreams, glNormalStream3iv), "glNormalStream3ivATI" },
    { offsetof(ATIVertexStreams, glNormalStream3s), "glNormalStream3sATI" },
    { offsetof(ATIVertexStreams, glNormalStream3sv), "glNormalStream3svATI" },
    { offsetof(ATIVertexStreams, glVertexBlendEnvf), "glVertexBlendEnvfATI" },
    { offsetof(ATIVertexStreams, glVertexBlendEnvi), "glVertexBlendEnviATI" },
    { offsetof(ATIVertexStreams, glVertexStream1d), "glVertexStream1dATI" },
    { offsetof(ATIVertexStreams, glVertexStream1dv), "glVertexStream1dvATI" },
    { offsetof(ATIVertexStreams, glVertexStream1f), "glVertexStream1fATI" },
    { offsetof(ATIVertexStreams, glVertexStream1fv), "glVertexStream1fvATI" },
    { offsetof(ATIVertexStreams, glVertexStream1i), "glVertexStream1iATI" },
    { offsetof(ATIVertexStreams, glVertexStream1iv), "glVertexStream1ivATI" },
    { offsetof(ATIVertexStreams, glVertexStream1s), "glVertexStream1sATI" },
    { offsetof(ATIVertexStreams, glVertexStream1sv), "glVertexStream1svATI" },
    { offsetof(ATIVertexStreams, glVertexStream2d), "glVertexStream2dATI" },
    { offsetof(ATIVertexStreams, glVertexStream2dv), "glVertexStream2dvATI" },
    { offsetof(ATIVertexStreams, glVertexStream2f), "glVertexStream2fATI" },
    { offsetof(ATIVertexStreams, glVertexStream2fv), "glVertexStream2fvATI" },
    { offsetof(ATIVertexStreams, glVertexStream2i), "glVertexStream2iATI" },
    { offsetof(ATIVertexStreams, glVertexStream2iv), "glVertexStream2ivATI" },
    { offsetof(ATIVertexStreams, glVertexStream2s), "glVertexStream2sATI" },
    { offsetof(ATIVertexStreams, glVertexStream2sv), "glVertexStream2svATI" },
    { offsetof(ATIVertexStreams, glVertexStream3d), "glVertexStream3dATI" },
    { offsetof(ATIVertexStreams, glVertexStream3dv), "glVertexStream3dvATI" },
    { offsetof(ATIVertexStreams, glVertexStream3f), "glVertexStream3fATI" },
    { offsetof(ATIVertexStreams, glVertexStream3fv), "glVertexStream3fvATI" },
    { offsetof(ATIVertexStreams, glVertexStream3i), "glVertexStream3iATI" },
    { offsetof(ATIVertexStreams, glVertexStream3iv), "glVertexStream3ivATI" },
    { offsetof(ATIVertexStreams, glVertexStream3s), "glVertexStream3sATI" },
    { offsetof(ATIVertexStreams, glVertexStream3sv), "glVertexStream3svATI" },
    { offsetof(ATIVertexStreams, glVertexStream4d), "glVertexStream4dATI" },
    { offsetof(ATIVertexStreams, glVertexStream4dv), "glVertexStream4dvATI" },
    { offsetof(ATIVertexStreams, glVertexStream4f), "glVertexStream4fATI" },
    { offsetof(ATIVertexStreams, glVertexStream4fv), "glVertexStream4fvATI" },
    { offsetof(ATIVertexStreams, glVertexStream4i), "glVertexStream4iATI" },
    { offsetof(ATIVertexStreams, glVertexStream4iv), "glVertexStream4ivATI" },
    { offsetof(ATIVertexStreams, glVertexStream4s), "glVertexStream4sATI" },
    { offsetof(ATIVertexStreams, glVertexStream4sv), "glVertexStream4svATI" },
};

static const size_t __ATIVertexStreamsEntryCount = sizeof(__ATIVertexStreamsEntryPoints) /
    sizeof(__ATIVertexStreamsEntryPoints[0]);

static const EntryOffset __EXTEglImageStorageEntryPoints[] = {
    { offsetof(EXTEglImageStorage, glEGLImageTargetTexStorage), "glEGLImageTargetTexStorageEXT" },
    { offsetof(EXTEglImageStorage, glEGLImageTargetTextureStorage), "glEGLImageTargetTextureStorageEXT" },
};

static const size_t __EXTEglImageStorageEntryCount = sizeof(__EXTEglImageStorageEntryPoints) /
    sizeof(__EXTEglImageStorageEntryPoints[0]);

static const EntryOffset __EXTBindableUniformEntryPoints[] = {
    { offsetof(EXTBindableUniform, glGetUniformBufferSize), "glGetUniformBufferSizeEXT" },
    { offsetof(EXTBindableUniform, glGetUniformOffset), "glGetUniformOffsetEXT" },
    { offsetof(EXTBindableUniform, glUniformBuffer), "glUniformBufferEXT" },
};

static const size_t __EXTBindableUniformEntryCount = sizeof(__EXTBindableUniformEntryPoints) /
    sizeof(__EXTBindableUniformEntryPoints[0]);

static const EntryOffset __EXTBlendColorEntryPoints[] = {
    { offsetof(EXTBlendColor, glBlendColor), "glBlendColorEXT" },
};

static const size_t __EXTBlendColorEntryCount = sizeof(__EXTBlendColorEntryPoints) /
    sizeof(__EXTBlendColorEntryPoints[0]);

static const EntryOffset __EXTBlendEquationSeparateEntryPoints[] = {
    { offsetof(EXTBlendEquationSeparate, glBlendEquationSeparate), "glBlendEquationSeparateEXT" },
};

static const size_t __EXTBlendEquationSeparateEntryCount = sizeof(__EXTBlendEquationSeparateEntryPoints) /
    sizeof(__EXTBlendEquationSeparateEntryPoints[0]);

static const EntryOffset __EXTBlendFuncSeparateEntryPoints[] = {
    { offsetof(EXTBlendFuncSeparate, glBlendFuncSeparate), "glBlendFuncSeparateEXT" },
};

static const size_t __EXTBlendFuncSeparateEntryCount = sizeof(__EXTBlendFuncSeparateEntryPoints) /
    sizeof(__EXTBlendFuncSeparateEntryPoints[0]);

static const EntryOffset __EXTBlendMinmaxEntryPoints[] = {
    { offsetof(EXTBlendMinmax, glBlendEquation), "glBlendEquationEXT" },
};

static const size_t __EXTBlendMinmaxEntryCount = sizeof(__EXTBlendMinmaxEntryPoints) /
    sizeof(__EXTBlendMinmaxEntryPoints[0]);

static const EntryOffset __EXTColorSubtableEntryPoints[] = {
    { offsetof(EXTColorSubtable, glColorSubTable), "glColorSubTableEXT" },
    { offsetof(EXTColorSubtable, glCopyColorSubTable), "glCopyColorSubTableEXT" },
};

static const size_t __EXTColorSubtableEntryCount = sizeof(__EXTColorSubtableEntryPoints) /
    sizeof(__EXTColorSubtableEntryPoints[0]);

static const EntryOffset __EXTCompiledVertexArrayEntryPoints[] = {
    { offsetof(EXTCompiledVertexArray, glLockArrays), "glLockArraysEXT" },
    { offsetof(EXTCompiledVertexArray, glUnlockArrays), "glUnlockArraysEXT" },
};

static const size_t __EXTCompiledVertexArrayEntryCount = sizeof(__EXTCompiledVertexArrayEntryPoints) /
    sizeof(__EXTCompiledVertexArrayEntryPoints[0]);

static const EntryOffset __EXTConvolutionEntryPoints[] = {
    { offsetof(EXTConvolution, glConvolutionFilter1D), "glConvolutionFilter1DEXT" },
    { offsetof(EXTConvolution, glConvolutionFilter2D), "glConvolutionFilter2DEXT" },
    { offsetof(EXTConvolution, glConvolutionParameterf), "glConvolutionParameterfEXT" },
    { offsetof(EXTConvolution, glConvolutionParameterfv), "glConvolutionParameterfvEXT" },
    { offsetof(EXTConvolution, glConvolutionParameteri), "glConvolutionParameteriEXT" },
    { offsetof(EXTConvolution, glConvolutionParameteriv), "glConvolutionParameterivEXT" },
    { offsetof(EXTConvolution, glCopyConvolutionFilter1D), "glCopyConvolutionFilter1DEXT" },
    { offsetof(EXTConvolution, glCopyConvolutionFilter2D), "glCopyConvolutionFilter2DEXT" },
    { offsetof(EXTConvolution, glGetConvolutionFilter), "glGetConvolutionFilterEXT" },
    { offsetof(EXTConvolution, glGetConvolutionParameterfv), "glGetConvolutionParameterfvEXT" },
    { offsetof(EXTConvolution, glGetConvolutionParameteriv), "glGetConvolutionParameterivEXT" },
    { offsetof(EXTConvolution, glGetSeparableFilter), "glGetSeparableFilterEXT" },
    { offsetof(EXTConvolution, glSeparableFilter2D), "glSeparableFilter2DEXT" },
};

static const size_t __EXTConvolutionEntryCount = sizeof(__EXTConvolutionEntryPoints) /
    sizeof(__EXTConvolutionEntryPoints[0]);

static const EntryOffset __EXTCoordinateFrameEntryPoints[] = {
    { offsetof(EXTCoordinateFrame, glBinormal3b), "glBinormal3bEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3bv), "glBinormal3bvEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3d), "glBinormal3dEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3dv), "glBinormal3dvEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3f), "glBinormal3fEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3fv), "glBinormal3fvEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3i), "glBinormal3iEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3iv), "glBinormal3ivEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3s), "glBinormal3sEXT" },
    { offsetof(EXTCoordinateFrame, glBinormal3sv), "glBinormal3svEXT" },
    { offsetof(EXTCoordinateFrame, glBinormalPointer), "glBinormalPointerEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3b), "glTangent3bEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3bv), "glTangent3bvEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3d), "glTangent3dEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3dv), "glTangent3dvEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3f), "glTangent3fEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3fv), "glTangent3fvEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3i), "glTangent3iEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3iv), "glTangent3ivEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3s), "glTangent3sEXT" },
    { offsetof(EXTCoordinateFrame, glTangent3sv), "glTangent3svEXT" },
    { offsetof(EXTCoordinateFrame, glTangentPointer), "glTangentPointerEXT" },
};

static const size_t __EXTCoordinateFrameEntryCount = sizeof(__EXTCoordinateFrameEntryPoints) /
    sizeof(__EXTCoordinateFrameEntryPoints[0]);

static const EntryOffset __EXTCopyTextureEntryPoints[] = {
    { offsetof(EXTCopyTexture, glCopyTexImage1D), "glCopyTexImage1DEXT" },
    { offsetof(EXTCopyTexture, glCopyTexImage2D), "glCopyTexImage2DEXT" },
    { offsetof(EXTCopyTexture, glCopyTexSubImage1D), "glCopyTexSubImage1DEXT" },
    { offsetof(EXTCopyTexture, glCopyTexSubImage2D), "glCopyTexSubImage2DEXT" },
    { offsetof(EXTCopyTexture, glCopyTexSubImage3D), "glCopyTexSubImage3DEXT" },
};

static const size_t __EXTCopyTextureEntryCount = sizeof(__EXTCopyTextureEntryPoints) /
    sizeof(__EXTCopyTextureEntryPoints[0]);

static const EntryOffset __EXTCullVertexEntryPoints[] = {
    { offsetof(EXTCullVertex, glCullParameterdv), "glCullParameterdvEXT" },
    { offsetof(EXTCullVertex, glCullParameterfv), "glCullParameterfvEXT" },
};

static const size_t __EXTCullVertexEntryCount = sizeof(__EXTCullVertexEntryPoints) /
    sizeof(__EXTCullVertexEntryPoints[0]);

static const EntryOffset __EXTDebugLabelEntryPoints[] = {
    { offsetof(EXTDebugLabel, glGetObjectLabel), "glGetObjectLabelEXT" },
    { offsetof(EXTDebugLabel, glLabelObject), "glLabelObjectEXT" },
};

static const size_t __EXTDebugLabelEntryCount = sizeof(__EXTDebugLabelEntryPoints) /
    sizeof(__EXTDebugLabelEntryPoints[0]);

static const EntryOffset __EXTDebugMarkerEntryPoints[] = {
    { offsetof(EXTDebugMarker, glInsertEventMarker), "glInsertEventMarkerEXT" },
    { offsetof(EXTDebugMarker, glPopGroupMarker), "glPopGroupMarkerEXT" },
    { offsetof(EXTDebugMarker, glPushGroupMarker), "glPushGroupMarkerEXT" },
};

static const size_t __EXTDebugMarkerEntryCount = sizeof(__EXTDebugMarkerEntryPoints) /
    sizeof(__EXTDebugMarkerEntryPoints[0]);

static const EntryOffset __EXTDepthBoundsTestEntryPoints[] = {
    { offsetof(EXTDepthBoundsTest, glDepthBounds), "glDepthBoundsEXT" },
};

static const size_t __EXTDepthBoundsTestEntryCount = sizeof(__EXTDepthBoundsTestEntryPoints) /
    sizeof(__EXTDepthBoundsTestEntryPoints[0]);

static const EntryOffset __EXTDirectStateAccessEntryPoints[] = {
    { offsetof(EXTDirectStateAccess, glBindMultiTexture), "glBindMultiTextureEXT" },
    { offsetof(EXTDirectStateAccess, glCheckNamedFramebufferStatus), "glCheckNamedFramebufferStatusEXT" },
    { offsetof(EXTDirectStateAccess, glClearNamedBufferData), "glClearNamedBufferDataEXT" },
    { offsetof(EXTDirectStateAccess, glClearNamedBufferSubData), "glClearNamedBufferSubDataEXT" },
    { offsetof(EXTDirectStateAccess, glClientAttribDefault), "glClientAttribDefaultEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedMultiTexImage1D), "glCompressedMultiTexImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedMultiTexImage2D), "glCompressedMultiTexImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedMultiTexImage3D), "glCompressedMultiTexImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedMultiTexSubImage1D), "glCompressedMultiTexSubImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedMultiTexSubImage2D), "glCompressedMultiTexSubImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedMultiTexSubImage3D), "glCompressedMultiTexSubImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedTextureImage1D), "glCompressedTextureImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedTextureImage2D), "glCompressedTextureImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedTextureImage3D), "glCompressedTextureImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedTextureSubImage1D), "glCompressedTextureSubImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedTextureSubImage2D), "glCompressedTextureSubImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glCompressedTextureSubImage3D), "glCompressedTextureSubImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyMultiTexImage1D), "glCopyMultiTexImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyMultiTexImage2D), "glCopyMultiTexImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyMultiTexSubImage1D), "glCopyMultiTexSubImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyMultiTexSubImage2D), "glCopyMultiTexSubImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyMultiTexSubImage3D), "glCopyMultiTexSubImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyTextureImage1D), "glCopyTextureImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyTextureImage2D), "glCopyTextureImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyTextureSubImage1D), "glCopyTextureSubImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyTextureSubImage2D), "glCopyTextureSubImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glCopyTextureSubImage3D), "glCopyTextureSubImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glDisableClientStateIndexed), "glDisableClientStateIndexedEXT" },
    { offsetof(EXTDirectStateAccess, glDisableClientStatei), "glDisableClientStateiEXT" },
    { offsetof(EXTDirectStateAccess, glDisableIndexed), "glDisableIndexedEXT" },
    { offsetof(EXTDirectStateAccess, glDisableVertexArrayAttrib), "glDisableVertexArrayAttribEXT" },
    { offsetof(EXTDirectStateAccess, glDisableVertexArray), "glDisableVertexArrayEXT" },
    { offsetof(EXTDirectStateAccess, glEnableClientStateIndexed), "glEnableClientStateIndexedEXT" },
    { offsetof(EXTDirectStateAccess, glEnableClientStatei), "glEnableClientStateiEXT" },
    { offsetof(EXTDirectStateAccess, glEnableIndexed), "glEnableIndexedEXT" },
    { offsetof(EXTDirectStateAccess, glEnableVertexArrayAttrib), "glEnableVertexArrayAttribEXT" },
    { offsetof(EXTDirectStateAccess, glEnableVertexArray), "glEnableVertexArrayEXT" },
    { offsetof(EXTDirectStateAccess, glFlushMappedNamedBufferRange), "glFlushMappedNamedBufferRangeEXT" },
    { offsetof(EXTDirectStateAccess, glFramebufferDrawBuffer), "glFramebufferDrawBufferEXT" },
    { offsetof(EXTDirectStateAccess, glFramebufferDrawBuffers), "glFramebufferDrawBuffersEXT" },
    { offsetof(EXTDirectStateAccess, glFramebufferReadBuffer), "glFramebufferReadBufferEXT" },
    { offsetof(EXTDirectStateAccess, glGenerateMultiTexMipmap), "glGenerateMultiTexMipmapEXT" },
    { offsetof(EXTDirectStateAccess, glGenerateTextureMipmap), "glGenerateTextureMipmapEXT" },
    { offsetof(EXTDirectStateAccess, glGetBooleanIndexedv), "glGetBooleanIndexedvEXT" },
    { offsetof(EXTDirectStateAccess, glGetCompressedMultiTexImage), "glGetCompressedMultiTexImageEXT" },
    { offsetof(EXTDirectStateAccess, glGetCompressedTextureImage), "glGetCompressedTextureImageEXT" },
    { offsetof(EXTDirectStateAccess, glGetDoubleIndexedv), "glGetDoubleIndexedvEXT" },
    { offsetof(EXTDirectStateAccess, glGetDoublei_v), "glGetDoublei_vEXT" },
    { offsetof(EXTDirectStateAccess, glGetFloatIndexedv), "glGetFloatIndexedvEXT" },
    { offsetof(EXTDirectStateAccess, glGetFloati_v), "glGetFloati_vEXT" },
    { offsetof(EXTDirectStateAccess, glGetFramebufferParameteriv), "glGetFramebufferParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetIntegerIndexedv), "glGetIntegerIndexedvEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexEnvfv), "glGetMultiTexEnvfvEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexEnviv), "glGetMultiTexEnvivEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexGendv), "glGetMultiTexGendvEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexGenfv), "glGetMultiTexGenfvEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexGeniv), "glGetMultiTexGenivEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexImage), "glGetMultiTexImageEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexLevelParameterfv), "glGetMultiTexLevelParameterfvEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexLevelParameteriv), "glGetMultiTexLevelParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexParameterIiv), "glGetMultiTexParameterIivEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexParameterIuiv), "glGetMultiTexParameterIuivEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexParameterfv), "glGetMultiTexParameterfvEXT" },
    { offsetof(EXTDirectStateAccess, glGetMultiTexParameteriv), "glGetMultiTexParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedBufferParameteriv), "glGetNamedBufferParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedBufferPointerv), "glGetNamedBufferPointervEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedBufferSubData), "glGetNamedBufferSubDataEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedFramebufferAttachmentParameteriv), "glGetNamedFramebufferAttachmentParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedFramebufferParameteriv), "glGetNamedFramebufferParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedProgramLocalParameterIiv), "glGetNamedProgramLocalParameterIivEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedProgramLocalParameterIuiv), "glGetNamedProgramLocalParameterIuivEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedProgramLocalParameterdv), "glGetNamedProgramLocalParameterdvEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedProgramLocalParameterfv), "glGetNamedProgramLocalParameterfvEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedProgramString), "glGetNamedProgramStringEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedProgramiv), "glGetNamedProgramivEXT" },
    { offsetof(EXTDirectStateAccess, glGetNamedRenderbufferParameteriv), "glGetNamedRenderbufferParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetPointerIndexedv), "glGetPointerIndexedvEXT" },
    { offsetof(EXTDirectStateAccess, glGetPointeri_v), "glGetPointeri_vEXT" },
    { offsetof(EXTDirectStateAccess, glGetTextureImage), "glGetTextureImageEXT" },
    { offsetof(EXTDirectStateAccess, glGetTextureLevelParameterfv), "glGetTextureLevelParameterfvEXT" },
    { offsetof(EXTDirectStateAccess, glGetTextureLevelParameteriv), "glGetTextureLevelParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetTextureParameterIiv), "glGetTextureParameterIivEXT" },
    { offsetof(EXTDirectStateAccess, glGetTextureParameterIuiv), "glGetTextureParameterIuivEXT" },
    { offsetof(EXTDirectStateAccess, glGetTextureParameterfv), "glGetTextureParameterfvEXT" },
    { offsetof(EXTDirectStateAccess, glGetTextureParameteriv), "glGetTextureParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glGetVertexArrayIntegeri_v), "glGetVertexArrayIntegeri_vEXT" },
    { offsetof(EXTDirectStateAccess, glGetVertexArrayIntegerv), "glGetVertexArrayIntegervEXT" },
    { offsetof(EXTDirectStateAccess, glGetVertexArrayPointeri_v), "glGetVertexArrayPointeri_vEXT" },
    { offsetof(EXTDirectStateAccess, glGetVertexArrayPointerv), "glGetVertexArrayPointervEXT" },
    { offsetof(EXTDirectStateAccess, glIsEnabledIndexed), "glIsEnabledIndexedEXT" },
    { offsetof(EXTDirectStateAccess, glMapNamedBuffer), "glMapNamedBufferEXT" },
    { offsetof(EXTDirectStateAccess, glMapNamedBufferRange), "glMapNamedBufferRangeEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixFrustum), "glMatrixFrustumEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixLoadIdentity), "glMatrixLoadIdentityEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixLoadTransposed), "glMatrixLoadTransposedEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixLoadTransposef), "glMatrixLoadTransposefEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixLoadd), "glMatrixLoaddEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixLoadf), "glMatrixLoadfEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixMultTransposed), "glMatrixMultTransposedEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixMultTransposef), "glMatrixMultTransposefEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixMultd), "glMatrixMultdEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixMultf), "glMatrixMultfEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixOrtho), "glMatrixOrthoEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixPop), "glMatrixPopEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixPush), "glMatrixPushEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixRotated), "glMatrixRotatedEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixRotatef), "glMatrixRotatefEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixScaled), "glMatrixScaledEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixScalef), "glMatrixScalefEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixTranslated), "glMatrixTranslatedEXT" },
    { offsetof(EXTDirectStateAccess, glMatrixTranslatef), "glMatrixTranslatefEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexBuffer), "glMultiTexBufferEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexCoordPointer), "glMultiTexCoordPointerEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexEnvf), "glMultiTexEnvfEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexEnvfv), "glMultiTexEnvfvEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexEnvi), "glMultiTexEnviEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexEnviv), "glMultiTexEnvivEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexGend), "glMultiTexGendEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexGendv), "glMultiTexGendvEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexGenf), "glMultiTexGenfEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexGenfv), "glMultiTexGenfvEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexGeni), "glMultiTexGeniEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexGeniv), "glMultiTexGenivEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexImage1D), "glMultiTexImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexImage2D), "glMultiTexImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexImage3D), "glMultiTexImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexParameterIiv), "glMultiTexParameterIivEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexParameterIuiv), "glMultiTexParameterIuivEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexParameterf), "glMultiTexParameterfEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexParameterfv), "glMultiTexParameterfvEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexParameteri), "glMultiTexParameteriEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexParameteriv), "glMultiTexParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexRenderbuffer), "glMultiTexRenderbufferEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexSubImage1D), "glMultiTexSubImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexSubImage2D), "glMultiTexSubImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glMultiTexSubImage3D), "glMultiTexSubImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glNamedBufferData), "glNamedBufferDataEXT" },
    { offsetof(EXTDirectStateAccess, glNamedBufferStorage), "glNamedBufferStorageEXT" },
    { offsetof(EXTDirectStateAccess, glNamedBufferSubData), "glNamedBufferSubDataEXT" },
    { offsetof(EXTDirectStateAccess, glNamedCopyBufferSubData), "glNamedCopyBufferSubDataEXT" },
    { offsetof(EXTDirectStateAccess, glNamedFramebufferParameteri), "glNamedFramebufferParameteriEXT" },
    { offsetof(EXTDirectStateAccess, glNamedFramebufferRenderbuffer), "glNamedFramebufferRenderbufferEXT" },
    { offsetof(EXTDirectStateAccess, glNamedFramebufferTexture1D), "glNamedFramebufferTexture1DEXT" },
    { offsetof(EXTDirectStateAccess, glNamedFramebufferTexture2D), "glNamedFramebufferTexture2DEXT" },
    { offsetof(EXTDirectStateAccess, glNamedFramebufferTexture3D), "glNamedFramebufferTexture3DEXT" },
    { offsetof(EXTDirectStateAccess, glNamedFramebufferTexture), "glNamedFramebufferTextureEXT" },
    { offsetof(EXTDirectStateAccess, glNamedFramebufferTextureFace), "glNamedFramebufferTextureFaceEXT" },
    { offsetof(EXTDirectStateAccess, glNamedFramebufferTextureLayer), "glNamedFramebufferTextureLayerEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameter4d), "glNamedProgramLocalParameter4dEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameter4dv), "glNamedProgramLocalParameter4dvEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameter4f), "glNamedProgramLocalParameter4fEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameter4fv), "glNamedProgramLocalParameter4fvEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameterI4i), "glNamedProgramLocalParameterI4iEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameterI4iv), "glNamedProgramLocalParameterI4ivEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameterI4ui), "glNamedProgramLocalParameterI4uiEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameterI4uiv), "glNamedProgramLocalParameterI4uivEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParameters4fv), "glNamedProgramLocalParameters4fvEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParametersI4iv), "glNamedProgramLocalParametersI4ivEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramLocalParametersI4uiv), "glNamedProgramLocalParametersI4uivEXT" },
    { offsetof(EXTDirectStateAccess, glNamedProgramString), "glNamedProgramStringEXT" },
    { offsetof(EXTDirectStateAccess, glNamedRenderbufferStorage), "glNamedRenderbufferStorageEXT" },
    { offsetof(EXTDirectStateAccess, glNamedRenderbufferStorageMultisampleCoverage), "glNamedRenderbufferStorageMultisampleCoverageEXT" },
    { offsetof(EXTDirectStateAccess, glNamedRenderbufferStorageMultisample), "glNamedRenderbufferStorageMultisampleEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform1d), "glProgramUniform1dEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform1dv), "glProgramUniform1dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform1f), "glProgramUniform1fEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform1fv), "glProgramUniform1fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform1i), "glProgramUniform1iEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform1iv), "glProgramUniform1ivEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform1ui), "glProgramUniform1uiEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform1uiv), "glProgramUniform1uivEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform2d), "glProgramUniform2dEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform2dv), "glProgramUniform2dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform2f), "glProgramUniform2fEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform2fv), "glProgramUniform2fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform2i), "glProgramUniform2iEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform2iv), "glProgramUniform2ivEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform2ui), "glProgramUniform2uiEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform2uiv), "glProgramUniform2uivEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform3d), "glProgramUniform3dEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform3dv), "glProgramUniform3dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform3f), "glProgramUniform3fEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform3fv), "glProgramUniform3fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform3i), "glProgramUniform3iEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform3iv), "glProgramUniform3ivEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform3ui), "glProgramUniform3uiEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform3uiv), "glProgramUniform3uivEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform4d), "glProgramUniform4dEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform4dv), "glProgramUniform4dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform4f), "glProgramUniform4fEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform4fv), "glProgramUniform4fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform4i), "glProgramUniform4iEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform4iv), "glProgramUniform4ivEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform4ui), "glProgramUniform4uiEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniform4uiv), "glProgramUniform4uivEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix2dv), "glProgramUniformMatrix2dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix2fv), "glProgramUniformMatrix2fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix2x3dv), "glProgramUniformMatrix2x3dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix2x3fv), "glProgramUniformMatrix2x3fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix2x4dv), "glProgramUniformMatrix2x4dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix2x4fv), "glProgramUniformMatrix2x4fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix3dv), "glProgramUniformMatrix3dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix3fv), "glProgramUniformMatrix3fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix3x2dv), "glProgramUniformMatrix3x2dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix3x2fv), "glProgramUniformMatrix3x2fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix3x4dv), "glProgramUniformMatrix3x4dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix3x4fv), "glProgramUniformMatrix3x4fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix4dv), "glProgramUniformMatrix4dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix4fv), "glProgramUniformMatrix4fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix4x2dv), "glProgramUniformMatrix4x2dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix4x2fv), "glProgramUniformMatrix4x2fvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix4x3dv), "glProgramUniformMatrix4x3dvEXT" },
    { offsetof(EXTDirectStateAccess, glProgramUniformMatrix4x3fv), "glProgramUniformMatrix4x3fvEXT" },
    { offsetof(EXTDirectStateAccess, glPushClientAttribDefault), "glPushClientAttribDefaultEXT" },
    { offsetof(EXTDirectStateAccess, glTextureBuffer), "glTextureBufferEXT" },
    { offsetof(EXTDirectStateAccess, glTextureBufferRange), "glTextureBufferRangeEXT" },
    { offsetof(EXTDirectStateAccess, glTextureImage1D), "glTextureImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glTextureImage2D), "glTextureImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glTextureImage3D), "glTextureImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glTexturePageCommitment), "glTexturePageCommitmentEXT" },
    { offsetof(EXTDirectStateAccess, glTextureParameterIiv), "glTextureParameterIivEXT" },
    { offsetof(EXTDirectStateAccess, glTextureParameterIuiv), "glTextureParameterIuivEXT" },
    { offsetof(EXTDirectStateAccess, glTextureParameterf), "glTextureParameterfEXT" },
    { offsetof(EXTDirectStateAccess, glTextureParameterfv), "glTextureParameterfvEXT" },
    { offsetof(EXTDirectStateAccess, glTextureParameteri), "glTextureParameteriEXT" },
    { offsetof(EXTDirectStateAccess, glTextureParameteriv), "glTextureParameterivEXT" },
    { offsetof(EXTDirectStateAccess, glTextureRenderbuffer), "glTextureRenderbufferEXT" },
    { offsetof(EXTDirectStateAccess, glTextureStorage1D), "glTextureStorage1DEXT" },
    { offsetof(EXTDirectStateAccess, glTextureStorage2D), "glTextureStorage2DEXT" },
    { offsetof(EXTDirectStateAccess, glTextureStorage2DMultisample), "glTextureStorage2DMultisampleEXT" },
    { offsetof(EXTDirectStateAccess, glTextureStorage3D), "glTextureStorage3DEXT" },
    { offsetof(EXTDirectStateAccess, glTextureStorage3DMultisample), "glTextureStorage3DMultisampleEXT" },
    { offsetof(EXTDirectStateAccess, glTextureSubImage1D), "glTextureSubImage1DEXT" },
    { offsetof(EXTDirectStateAccess, glTextureSubImage2D), "glTextureSubImage2DEXT" },
    { offsetof(EXTDirectStateAccess, glTextureSubImage3D), "glTextureSubImage3DEXT" },
    { offsetof(EXTDirectStateAccess, glUnmapNamedBuffer), "glUnmapNamedBufferEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayBindVertexBuffer), "glVertexArrayBindVertexBufferEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayColorOffset), "glVertexArrayColorOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayEdgeFlagOffset), "glVertexArrayEdgeFlagOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayFogCoordOffset), "glVertexArrayFogCoordOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayIndexOffset), "glVertexArrayIndexOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayMultiTexCoordOffset), "glVertexArrayMultiTexCoordOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayNormalOffset), "glVertexArrayNormalOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArraySecondaryColorOffset), "glVertexArraySecondaryColorOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayTexCoordOffset), "glVertexArrayTexCoordOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexAttribBinding), "glVertexArrayVertexAttribBindingEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexAttribDivisor), "glVertexArrayVertexAttribDivisorEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexAttribFormat), "glVertexArrayVertexAttribFormatEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexAttribIFormat), "glVertexArrayVertexAttribIFormatEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexAttribIOffset), "glVertexArrayVertexAttribIOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexAttribLFormat), "glVertexArrayVertexAttribLFormatEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexAttribLOffset), "glVertexArrayVertexAttribLOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexAttribOffset), "glVertexArrayVertexAttribOffsetEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexBindingDivisor), "glVertexArrayVertexBindingDivisorEXT" },
    { offsetof(EXTDirectStateAccess, glVertexArrayVertexOffset), "glVertexArrayVertexOffsetEXT" },
};

static const size_t __EXTDirectStateAccessEntryCount = sizeof(__EXTDirectStateAccessEntryPoints) /
    sizeof(__EXTDirectStateAccessEntryPoints[0]);

static const EntryOffset __EXTDrawBUFFERS2EntryPoints[] = {
    { offsetof(EXTDrawBUFFERS2, glColorMaskIndexed), "glColorMaskIndexedEXT" },
};

static const size_t __EXTDrawBUFFERS2EntryCount = sizeof(__EXTDrawBUFFERS2EntryPoints) /
    sizeof(__EXTDrawBUFFERS2EntryPoints[0]);

static const EntryOffset __EXTDrawInstancedEntryPoints[] = {
    { offsetof(EXTDrawInstanced, glDrawArraysInstanced), "glDrawArraysInstancedEXT" },
    { offsetof(EXTDrawInstanced, glDrawElementsInstanced), "glDrawElementsInstancedEXT" },
};

static const size_t __EXTDrawInstancedEntryCount = sizeof(__EXTDrawInstancedEntryPoints) /
    sizeof(__EXTDrawInstancedEntryPoints[0]);

static const EntryOffset __EXTDrawRangeElementsEntryPoints[] = {
    { offsetof(EXTDrawRangeElements, glDrawRangeElements), "glDrawRangeElementsEXT" },
};

static const size_t __EXTDrawRangeElementsEntryCount = sizeof(__EXTDrawRangeElementsEntryPoints) /
    sizeof(__EXTDrawRangeElementsEntryPoints[0]);

static const EntryOffset __EXTExternalBufferEntryPoints[] = {
    { offsetof(EXTExternalBuffer, glBufferStorageExternal), "glBufferStorageExternalEXT" },
    { offsetof(EXTExternalBuffer, glNamedBufferStorageExternal), "glNamedBufferStorageExternalEXT" },
};

static const size_t __EXTExternalBufferEntryCount = sizeof(__EXTExternalBufferEntryPoints) /
    sizeof(__EXTExternalBufferEntryPoints[0]);

static const EntryOffset __EXTFogCoordEntryPoints[] = {
    { offsetof(EXTFogCoord, glFogCoordPointer), "glFogCoordPointerEXT" },
    { offsetof(EXTFogCoord, glFogCoordd), "glFogCoorddEXT" },
    { offsetof(EXTFogCoord, glFogCoorddv), "glFogCoorddvEXT" },
    { offsetof(EXTFogCoord, glFogCoordf), "glFogCoordfEXT" },
    { offsetof(EXTFogCoord, glFogCoordfv), "glFogCoordfvEXT" },
};

static const size_t __EXTFogCoordEntryCount = sizeof(__EXTFogCoordEntryPoints) /
    sizeof(__EXTFogCoordEntryPoints[0]);

static const EntryOffset __EXTFramebufferBlitEntryPoints[] = {
    { offsetof(EXTFramebufferBlit, glBlitFramebuffer), "glBlitFramebufferEXT" },
};

static const size_t __EXTFramebufferBlitEntryCount = sizeof(__EXTFramebufferBlitEntryPoints) /
    sizeof(__EXTFramebufferBlitEntryPoints[0]);

static const EntryOffset __EXTFramebufferMultisampleEntryPoints[] = {
    { offsetof(EXTFramebufferMultisample, glRenderbufferStorageMultisample), "glRenderbufferStorageMultisampleEXT" },
};

static const size_t __EXTFramebufferMultisampleEntryCount = sizeof(__EXTFramebufferMultisampleEntryPoints) /
    sizeof(__EXTFramebufferMultisampleEntryPoints[0]);

static const EntryOffset __EXTFramebufferObjectEntryPoints[] = {
    { offsetof(EXTFramebufferObject, glBindFramebuffer), "glBindFramebufferEXT" },
    { offsetof(EXTFramebufferObject, glBindRenderbuffer), "glBindRenderbufferEXT" },
    { offsetof(EXTFramebufferObject, glCheckFramebufferStatus), "glCheckFramebufferStatusEXT" },
    { offsetof(EXTFramebufferObject, glDeleteFramebuffers), "glDeleteFramebuffersEXT" },
    { offsetof(EXTFramebufferObject, glDeleteRenderbuffers), "glDeleteRenderbuffersEXT" },
    { offsetof(EXTFramebufferObject, glFramebufferRenderbuffer), "glFramebufferRenderbufferEXT" },
    { offsetof(EXTFramebufferObject, glFramebufferTexture1D), "glFramebufferTexture1DEXT" },
    { offsetof(EXTFramebufferObject, glFramebufferTexture2D), "glFramebufferTexture2DEXT" },
    { offsetof(EXTFramebufferObject, glFramebufferTexture3D), "glFramebufferTexture3DEXT" },
    { offsetof(EXTFramebufferObject, glGenFramebuffers), "glGenFramebuffersEXT" },
    { offsetof(EXTFramebufferObject, glGenRenderbuffers), "glGenRenderbuffersEXT" },
    { offsetof(EXTFramebufferObject, glGenerateMipmap), "glGenerateMipmapEXT" },
    { offsetof(EXTFramebufferObject, glGetFramebufferAttachmentParameteriv), "glGetFramebufferAttachmentParameterivEXT" },
    { offsetof(EXTFramebufferObject, glGetRenderbufferParameteriv), "glGetRenderbufferParameterivEXT" },
    { offsetof(EXTFramebufferObject, glIsFramebuffer), "glIsFramebufferEXT" },
    { offsetof(EXTFramebufferObject, glIsRenderbuffer), "glIsRenderbufferEXT" },
    { offsetof(EXTFramebufferObject, glRenderbufferStorage), "glRenderbufferStorageEXT" },
};

static const size_t __EXTFramebufferObjectEntryCount = sizeof(__EXTFramebufferObjectEntryPoints) /
    sizeof(__EXTFramebufferObjectEntryPoints[0]);

static const EntryOffset __EXTGeometrySHADER4EntryPoints[] = {
    { offsetof(EXTGeometrySHADER4, glProgramParameteri), "glProgramParameteriEXT" },
};

static const size_t __EXTGeometrySHADER4EntryCount = sizeof(__EXTGeometrySHADER4EntryPoints) /
    sizeof(__EXTGeometrySHADER4EntryPoints[0]);

static const EntryOffset __EXTGPUProgramParametersEntryPoints[] = {
    { offsetof(EXTGPUProgramParameters, glProgramEnvParameters4fv), "glProgramEnvParameters4fvEXT" },
    { offsetof(EXTGPUProgramParameters, glProgramLocalParameters4fv), "glProgramLocalParameters4fvEXT" },
};

static const size_t __EXTGPUProgramParametersEntryCount = sizeof(__EXTGPUProgramParametersEntryPoints) /
    sizeof(__EXTGPUProgramParametersEntryPoints[0]);

static const EntryOffset __EXTGPUSHADER4EntryPoints[] = {
    { offsetof(EXTGPUSHADER4, glBindFragDataLocation), "glBindFragDataLocationEXT" },
    { offsetof(EXTGPUSHADER4, glGetFragDataLocation), "glGetFragDataLocationEXT" },
    { offsetof(EXTGPUSHADER4, glGetUniformuiv), "glGetUniformuivEXT" },
    { offsetof(EXTGPUSHADER4, glGetVertexAttribIiv), "glGetVertexAttribIivEXT" },
    { offsetof(EXTGPUSHADER4, glGetVertexAttribIuiv), "glGetVertexAttribIuivEXT" },
    { offsetof(EXTGPUSHADER4, glUniform1ui), "glUniform1uiEXT" },
    { offsetof(EXTGPUSHADER4, glUniform1uiv), "glUniform1uivEXT" },
    { offsetof(EXTGPUSHADER4, glUniform2ui), "glUniform2uiEXT" },
    { offsetof(EXTGPUSHADER4, glUniform2uiv), "glUniform2uivEXT" },
    { offsetof(EXTGPUSHADER4, glUniform3ui), "glUniform3uiEXT" },
    { offsetof(EXTGPUSHADER4, glUniform3uiv), "glUniform3uivEXT" },
    { offsetof(EXTGPUSHADER4, glUniform4ui), "glUniform4uiEXT" },
    { offsetof(EXTGPUSHADER4, glUniform4uiv), "glUniform4uivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI1i), "glVertexAttribI1iEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI1iv), "glVertexAttribI1ivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI1ui), "glVertexAttribI1uiEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI1uiv), "glVertexAttribI1uivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI2i), "glVertexAttribI2iEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI2iv), "glVertexAttribI2ivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI2ui), "glVertexAttribI2uiEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI2uiv), "glVertexAttribI2uivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI3i), "glVertexAttribI3iEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI3iv), "glVertexAttribI3ivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI3ui), "glVertexAttribI3uiEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI3uiv), "glVertexAttribI3uivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI4bv), "glVertexAttribI4bvEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI4i), "glVertexAttribI4iEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI4iv), "glVertexAttribI4ivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI4sv), "glVertexAttribI4svEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI4ubv), "glVertexAttribI4ubvEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI4ui), "glVertexAttribI4uiEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI4uiv), "glVertexAttribI4uivEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribI4usv), "glVertexAttribI4usvEXT" },
    { offsetof(EXTGPUSHADER4, glVertexAttribIPointer), "glVertexAttribIPointerEXT" },
};

static const size_t __EXTGPUSHADER4EntryCount = sizeof(__EXTGPUSHADER4EntryPoints) /
    sizeof(__EXTGPUSHADER4EntryPoints[0]);

static const EntryOffset __EXTHistogramEntryPoints[] = {
    { offsetof(EXTHistogram, glGetHistogram), "glGetHistogramEXT" },
    { offsetof(EXTHistogram, glGetHistogramParameterfv), "glGetHistogramParameterfvEXT" },
    { offsetof(EXTHistogram, glGetHistogramParameteriv), "glGetHistogramParameterivEXT" },
    { offsetof(EXTHistogram, glGetMinmax), "glGetMinmaxEXT" },
    { offsetof(EXTHistogram, glGetMinmaxParameterfv), "glGetMinmaxParameterfvEXT" },
    { offsetof(EXTHistogram, glGetMinmaxParameteriv), "glGetMinmaxParameterivEXT" },
    { offsetof(EXTHistogram, glHistogram), "glHistogramEXT" },
    { offsetof(EXTHistogram, glMinmax), "glMinmaxEXT" },
    { offsetof(EXTHistogram, glResetHistogram), "glResetHistogramEXT" },
    { offsetof(EXTHistogram, glResetMinmax), "glResetMinmaxEXT" },
};

static const size_t __EXTHistogramEntryCount = sizeof(__EXTHistogramEntryPoints) /
    sizeof(__EXTHistogramEntryPoints[0]);

static const EntryOffset __EXTIndexFuncEntryPoints[] = {
    { offsetof(EXTIndexFunc, glIndexFunc), "glIndexFuncEXT" },
};

static const size_t __EXTIndexFuncEntryCount = sizeof(__EXTIndexFuncEntryPoints) /
    sizeof(__EXTIndexFuncEntryPoints[0]);

static const EntryOffset __EXTIndexMaterialEntryPoints[] = {
    { offsetof(EXTIndexMaterial, glIndexMaterial), "glIndexMaterialEXT" },
};

static const size_t __EXTIndexMaterialEntryCount = sizeof(__EXTIndexMaterialEntryPoints) /
    sizeof(__EXTIndexMaterialEntryPoints[0]);

static const EntryOffset __EXTLightTextureEntryPoints[] = {
    { offsetof(EXTLightTexture, glApplyTexture), "glApplyTextureEXT" },
    { offsetof(EXTLightTexture, glTextureLight), "glTextureLightEXT" },
    { offsetof(EXTLightTexture, glTextureMaterial), "glTextureMaterialEXT" },
};

static const size_t __EXTLightTextureEntryCount = sizeof(__EXTLightTextureEntryPoints) /
    sizeof(__EXTLightTextureEntryPoints[0]);

static const EntryOffset __EXTMemoryObjectEntryPoints[] = {
    { offsetof(EXTMemoryObject, glBufferStorageMem), "glBufferStorageMemEXT" },
    { offsetof(EXTMemoryObject, glCreateMemoryObjects), "glCreateMemoryObjectsEXT" },
    { offsetof(EXTMemoryObject, glDeleteMemoryObjects), "glDeleteMemoryObjectsEXT" },
    { offsetof(EXTMemoryObject, glGetMemoryObjectParameteriv), "glGetMemoryObjectParameterivEXT" },
    { offsetof(EXTMemoryObject, glGetUnsignedBytei_v), "glGetUnsignedBytei_vEXT" },
    { offsetof(EXTMemoryObject, glGetUnsignedBytev), "glGetUnsignedBytevEXT" },
    { offsetof(EXTMemoryObject, glIsMemoryObject), "glIsMemoryObjectEXT" },
    { offsetof(EXTMemoryObject, glMemoryObjectParameteriv), "glMemoryObjectParameterivEXT" },
    { offsetof(EXTMemoryObject, glNamedBufferStorageMem), "glNamedBufferStorageMemEXT" },
    { offsetof(EXTMemoryObject, glTexStorageMem1D), "glTexStorageMem1DEXT" },
    { offsetof(EXTMemoryObject, glTexStorageMem2D), "glTexStorageMem2DEXT" },
    { offsetof(EXTMemoryObject, glTexStorageMem2DMultisample), "glTexStorageMem2DMultisampleEXT" },
    { offsetof(EXTMemoryObject, glTexStorageMem3D), "glTexStorageMem3DEXT" },
    { offsetof(EXTMemoryObject, glTexStorageMem3DMultisample), "glTexStorageMem3DMultisampleEXT" },
    { offsetof(EXTMemoryObject, glTextureStorageMem1D), "glTextureStorageMem1DEXT" },
    { offsetof(EXTMemoryObject, glTextureStorageMem2D), "glTextureStorageMem2DEXT" },
    { offsetof(EXTMemoryObject, glTextureStorageMem2DMultisample), "glTextureStorageMem2DMultisampleEXT" },
    { offsetof(EXTMemoryObject, glTextureStorageMem3D), "glTextureStorageMem3DEXT" },
    { offsetof(EXTMemoryObject, glTextureStorageMem3DMultisample), "glTextureStorageMem3DMultisampleEXT" },
};

static const size_t __EXTMemoryObjectEntryCount = sizeof(__EXTMemoryObjectEntryPoints) /
    sizeof(__EXTMemoryObjectEntryPoints[0]);

static const EntryOffset __EXTMemoryObjectFdEntryPoints[] = {
    { offsetof(EXTMemoryObjectFd, glImportMemoryFd), "glImportMemoryFdEXT" },
};

static const size_t __EXTMemoryObjectFdEntryCount = sizeof(__EXTMemoryObjectFdEntryPoints) /
    sizeof(__EXTMemoryObjectFdEntryPoints[0]);

static const EntryOffset __EXTMemoryObjectWIN32EntryPoints[] = {
    { offsetof(EXTMemoryObjectWIN32, glImportMemoryWin32Handle), "glImportMemoryWin32HandleEXT" },
    { offsetof(EXTMemoryObjectWIN32, glImportMemoryWin32Name), "glImportMemoryWin32NameEXT" },
};

static const size_t __EXTMemoryObjectWIN32EntryCount = sizeof(__EXTMemoryObjectWIN32EntryPoints) /
    sizeof(__EXTMemoryObjectWIN32EntryPoints[0]);

static const EntryOffset __EXTMultiDrawArraysEntryPoints[] = {
    { offsetof(EXTMultiDrawArrays, glMultiDrawArrays), "glMultiDrawArraysEXT" },
    { offsetof(EXTMultiDrawArrays, glMultiDrawElements), "glMultiDrawElementsEXT" },
};

static const size_t __EXTMultiDrawArraysEntryCount = sizeof(__EXTMultiDrawArraysEntryPoints) /
    sizeof(__EXTMultiDrawArraysEntryPoints[0]);

static const EntryOffset __EXTMultisampleEntryPoints[] = {
    { offsetof(EXTMultisample, glSampleMask), "glSampleMaskEXT" },
    { offsetof(EXTMultisample, glSamplePattern), "glSamplePatternEXT" },
};

static const size_t __EXTMultisampleEntryCount = sizeof(__EXTMultisampleEntryPoints) /
    sizeof(__EXTMultisampleEntryPoints[0]);

static const EntryOffset __EXTPalettedTextureEntryPoints[] = {
    { offsetof(EXTPalettedTexture, glColorTable), "glColorTableEXT" },
    { offsetof(EXTPalettedTexture, glGetColorTable), "glGetColorTableEXT" },
    { offsetof(EXTPalettedTexture, glGetColorTableParameterfv), "glGetColorTableParameterfvEXT" },
    { offsetof(EXTPalettedTexture, glGetColorTableParameteriv), "glGetColorTableParameterivEXT" },
};

static const size_t __EXTPalettedTextureEntryCount = sizeof(__EXTPalettedTextureEntryPoints) /
    sizeof(__EXTPalettedTextureEntryPoints[0]);

static const EntryOffset __EXTPixelTransformEntryPoints[] = {
    { offsetof(EXTPixelTransform, glGetPixelTransformParameterfv), "glGetPixelTransformParameterfvEXT" },
    { offsetof(EXTPixelTransform, glGetPixelTransformParameteriv), "glGetPixelTransformParameterivEXT" },
    { offsetof(EXTPixelTransform, glPixelTransformParameterf), "glPixelTransformParameterfEXT" },
    { offsetof(EXTPixelTransform, glPixelTransformParameterfv), "glPixelTransformParameterfvEXT" },
    { offsetof(EXTPixelTransform, glPixelTransformParameteri), "glPixelTransformParameteriEXT" },
    { offsetof(EXTPixelTransform, glPixelTransformParameteriv), "glPixelTransformParameterivEXT" },
};

static const size_t __EXTPixelTransformEntryCount = sizeof(__EXTPixelTransformEntryPoints) /
    sizeof(__EXTPixelTransformEntryPoints[0]);

static const EntryOffset __EXTPointParametersEntryPoints[] = {
    { offsetof(EXTPointParameters, glPointParameterf), "glPointParameterfEXT" },
    { offsetof(EXTPointParameters, glPointParameterfv), "glPointParameterfvEXT" },
};

static const size_t __EXTPointParametersEntryCount = sizeof(__EXTPointParametersEntryPoints) /
    sizeof(__EXTPointParametersEntryPoints[0]);

static const EntryOffset __EXTPolygonOffsetEntryPoints[] = {
    { offsetof(EXTPolygonOffset, glPolygonOffset), "glPolygonOffsetEXT" },
};

static const size_t __EXTPolygonOffsetEntryCount = sizeof(__EXTPolygonOffsetEntryPoints) /
    sizeof(__EXTPolygonOffsetEntryPoints[0]);

static const EntryOffset __EXTPolygonOffsetClampEntryPoints[] = {
    { offsetof(EXTPolygonOffsetClamp, glPolygonOffsetClamp), "glPolygonOffsetClampEXT" },
};

static const size_t __EXTPolygonOffsetClampEntryCount = sizeof(__EXTPolygonOffsetClampEntryPoints) /
    sizeof(__EXTPolygonOffsetClampEntryPoints[0]);

static const EntryOffset __EXTProvokingVertexEntryPoints[] = {
    { offsetof(EXTProvokingVertex, glProvokingVertex), "glProvokingVertexEXT" },
};

static const size_t __EXTProvokingVertexEntryCount = sizeof(__EXTProvokingVertexEntryPoints) /
    sizeof(__EXTProvokingVertexEntryPoints[0]);

static const EntryOffset __EXTRasterMultisampleEntryPoints[] = {
    { offsetof(EXTRasterMultisample, glRasterSamples), "glRasterSamplesEXT" },
};

static const size_t __EXTRasterMultisampleEntryCount = sizeof(__EXTRasterMultisampleEntryPoints) /
    sizeof(__EXTRasterMultisampleEntryPoints[0]);

static const EntryOffset __EXTSecondaryColorEntryPoints[] = {
    { offsetof(EXTSecondaryColor, glSecondaryColor3b), "glSecondaryColor3bEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3bv), "glSecondaryColor3bvEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3d), "glSecondaryColor3dEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3dv), "glSecondaryColor3dvEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3f), "glSecondaryColor3fEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3fv), "glSecondaryColor3fvEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3i), "glSecondaryColor3iEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3iv), "glSecondaryColor3ivEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3s), "glSecondaryColor3sEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3sv), "glSecondaryColor3svEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3ub), "glSecondaryColor3ubEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3ubv), "glSecondaryColor3ubvEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3ui), "glSecondaryColor3uiEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3uiv), "glSecondaryColor3uivEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3us), "glSecondaryColor3usEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColor3usv), "glSecondaryColor3usvEXT" },
    { offsetof(EXTSecondaryColor, glSecondaryColorPointer), "glSecondaryColorPointerEXT" },
};

static const size_t __EXTSecondaryColorEntryCount = sizeof(__EXTSecondaryColorEntryPoints) /
    sizeof(__EXTSecondaryColorEntryPoints[0]);

static const EntryOffset __EXTSemaphoreEntryPoints[] = {
    { offsetof(EXTSemaphore, glDeleteSemaphores), "glDeleteSemaphoresEXT" },
    { offsetof(EXTSemaphore, glGenSemaphores), "glGenSemaphoresEXT" },
    { offsetof(EXTSemaphore, glGetSemaphoreParameterui64v), "glGetSemaphoreParameterui64vEXT" },
    { offsetof(EXTSemaphore, glIsSemaphore), "glIsSemaphoreEXT" },
    { offsetof(EXTSemaphore, glSemaphoreParameterui64v), "glSemaphoreParameterui64vEXT" },
    { offsetof(EXTSemaphore, glSignalSemaphore), "glSignalSemaphoreEXT" },
    { offsetof(EXTSemaphore, glWaitSemaphore), "glWaitSemaphoreEXT" },
};

static const size_t __EXTSemaphoreEntryCount = sizeof(__EXTSemaphoreEntryPoints) /
    sizeof(__EXTSemaphoreEntryPoints[0]);

static const EntryOffset __EXTSemaphoreFdEntryPoints[] = {
    { offsetof(EXTSemaphoreFd, glImportSemaphoreFd), "glImportSemaphoreFdEXT" },
};

static const size_t __EXTSemaphoreFdEntryCount = sizeof(__EXTSemaphoreFdEntryPoints) /
    sizeof(__EXTSemaphoreFdEntryPoints[0]);

static const EntryOffset __EXTSemaphoreWIN32EntryPoints[] = {
    { offsetof(EXTSemaphoreWIN32, glImportSemaphoreWin32Handle), "glImportSemaphoreWin32HandleEXT" },
    { offsetof(EXTSemaphoreWIN32, glImportSemaphoreWin32Name), "glImportSemaphoreWin32NameEXT" },
};

static const size_t __EXTSemaphoreWIN32EntryCount = sizeof(__EXTSemaphoreWIN32EntryPoints) /
    sizeof(__EXTSemaphoreWIN32EntryPoints[0]);

static const EntryOffset __EXTSeparateShaderObjectsEntryPoints[] = {
    { offsetof(EXTSeparateShaderObjects, glActiveProgram), "glActiveProgramEXT" },
    { offsetof(EXTSeparateShaderObjects, glCreateShaderProgram), "glCreateShaderProgramEXT" },
    { offsetof(EXTSeparateShaderObjects, glUseShaderProgram), "glUseShaderProgramEXT" },
};

static const size_t __EXTSeparateShaderObjectsEntryCount = sizeof(__EXTSeparateShaderObjectsEntryPoints) /
    sizeof(__EXTSeparateShaderObjectsEntryPoints[0]);

static const EntryOffset __EXTShaderFramebufferFetchNonCoherentEntryPoints[] = {
    { offsetof(EXTShaderFramebufferFetchNonCoherent, glFramebufferFetchBarrier), "glFramebufferFetchBarrierEXT" },
};

static const size_t __EXTShaderFramebufferFetchNonCoherentEntryCount = sizeof(__EXTShaderFramebufferFetchNonCoherentEntryPoints) /
    sizeof(__EXTShaderFramebufferFetchNonCoherentEntryPoints[0]);

static const EntryOffset __EXTShaderImageLoadStoreEntryPoints[] = {
    { offsetof(EXTShaderImageLoadStore, glBindImageTexture), "glBindImageTextureEXT" },
    { offsetof(EXTShaderImageLoadStore, glMemoryBarrier), "glMemoryBarrierEXT" },
};

static const size_t __EXTShaderImageLoadStoreEntryCount = sizeof(__EXTShaderImageLoadStoreEntryPoints) /
    sizeof(__EXTShaderImageLoadStoreEntryPoints[0]);

static const EntryOffset __EXTStencilClearTagEntryPoints[] = {
    { offsetof(EXTStencilClearTag, glStencilClearTag), "glStencilClearTagEXT" },
};

static const size_t __EXTStencilClearTagEntryCount = sizeof(__EXTStencilClearTagEntryPoints) /
    sizeof(__EXTStencilClearTagEntryPoints[0]);

static const EntryOffset __EXTStencilTwoSideEntryPoints[] = {
    { offsetof(EXTStencilTwoSide, glActiveStencilFace), "glActiveStencilFaceEXT" },
};

static const size_t __EXTStencilTwoSideEntryCount = sizeof(__EXTStencilTwoSideEntryPoints) /
    sizeof(__EXTStencilTwoSideEntryPoints[0]);

static const EntryOffset __EXTSubtextureEntryPoints[] = {
    { offsetof(EXTSubtexture, glTexSubImage1D), "glTexSubImage1DEXT" },
    { offsetof(EXTSubtexture, glTexSubImage2D), "glTexSubImage2DEXT" },
};

static const size_t __EXTSubtextureEntryCount = sizeof(__EXTSubtextureEntryPoints) /
    sizeof(__EXTSubtextureEntryPoints[0]);

static const EntryOffset __EXTTEXTURE3DEntryPoints[] = {
    { offsetof(EXTTEXTURE3D, glTexImage3D), "glTexImage3DEXT" },
    { offsetof(EXTTEXTURE3D, glTexSubImage3D), "glTexSubImage3DEXT" },
};

static const size_t __EXTTEXTURE3DEntryCount = sizeof(__EXTTEXTURE3DEntryPoints) /
    sizeof(__EXTTEXTURE3DEntryPoints[0]);

static const EntryOffset __EXTTextureArrayEntryPoints[] = {
    { offsetof(EXTTextureArray, glFramebufferTextureLayer), "glFramebufferTextureLayerEXT" },
};

static const size_t __EXTTextureArrayEntryCount = sizeof(__EXTTextureArrayEntryPoints) /
    sizeof(__EXTTextureArrayEntryPoints[0]);

static const EntryOffset __EXTTextureBufferObjectEntryPoints[] = {
    { offsetof(EXTTextureBufferObject, glTexBuffer), "glTexBufferEXT" },
};

static const size_t __EXTTextureBufferObjectEntryCount = sizeof(__EXTTextureBufferObjectEntryPoints) /
    sizeof(__EXTTextureBufferObjectEntryPoints[0]);

static const EntryOffset __EXTTextureIntegerEntryPoints[] = {
    { offsetof(EXTTextureInteger, glClearColorIi), "glClearColorIiEXT" },
    { offsetof(EXTTextureInteger, glClearColorIui), "glClearColorIuiEXT" },
    { offsetof(EXTTextureInteger, glGetTexParameterIiv), "glGetTexParameterIivEXT" },
    { offsetof(EXTTextureInteger, glGetTexParameterIuiv), "glGetTexParameterIuivEXT" },
    { offsetof(EXTTextureInteger, glTexParameterIiv), "glTexParameterIivEXT" },
    { offsetof(EXTTextureInteger, glTexParameterIuiv), "glTexParameterIuivEXT" },
};

static const size_t __EXTTextureIntegerEntryCount = sizeof(__EXTTextureIntegerEntryPoints) /
    sizeof(__EXTTextureIntegerEntryPoints[0]);

static const EntryOffset __EXTTextureObjectEntryPoints[] = {
    { offsetof(EXTTextureObject, glAreTexturesResident), "glAreTexturesResidentEXT" },
    { offsetof(EXTTextureObject, glBindTexture), "glBindTextureEXT" },
    { offsetof(EXTTextureObject, glDeleteTextures), "glDeleteTexturesEXT" },
    { offsetof(EXTTextureObject, glGenTextures), "glGenTexturesEXT" },
    { offsetof(EXTTextureObject, glIsTexture), "glIsTextureEXT" },
    { offsetof(EXTTextureObject, glPrioritizeTextures), "glPrioritizeTexturesEXT" },
};

static const size_t __EXTTextureObjectEntryCount = sizeof(__EXTTextureObjectEntryPoints) /
    sizeof(__EXTTextureObjectEntryPoints[0]);

static const EntryOffset __EXTTexturePerturbNormalEntryPoints[] = {
    { offsetof(EXTTexturePerturbNormal, glTextureNormal), "glTextureNormalEXT" },
};

static const size_t __EXTTexturePerturbNormalEntryCount = sizeof(__EXTTexturePerturbNormalEntryPoints) /
    sizeof(__EXTTexturePerturbNormalEntryPoints[0]);

static const EntryOffset __EXTTextureStorageEntryPoints[] = {
    { offsetof(EXTTextureStorage, glTexStorage1D), "glTexStorage1DEXT" },
    { offsetof(EXTTextureStorage, glTexStorage2D), "glTexStorage2DEXT" },
    { offsetof(EXTTextureStorage, glTexStorage3D), "glTexStorage3DEXT" },
};

static const size_t __EXTTextureStorageEntryCount = sizeof(__EXTTextureStorageEntryPoints) /
    sizeof(__EXTTextureStorageEntryPoints[0]);

static const EntryOffset __EXTTimerQueryEntryPoints[] = {
    { offsetof(EXTTimerQuery, glGetQueryObjecti64v), "glGetQueryObjecti64vEXT" },
    { offsetof(EXTTimerQuery, glGetQueryObjectui64v), "glGetQueryObjectui64vEXT" },
};

static const size_t __EXTTimerQueryEntryCount = sizeof(__EXTTimerQueryEntryPoints) /
    sizeof(__EXTTimerQueryEntryPoints[0]);

static const EntryOffset __EXTTransformFeedbackEntryPoints[] = {
    { offsetof(EXTTransformFeedback, glBeginTransformFeedback), "glBeginTransformFeedbackEXT" },
    { offsetof(EXTTransformFeedback, glBindBufferBase), "glBindBufferBaseEXT" },
    { offsetof(EXTTransformFeedback, glBindBufferOffset), "glBindBufferOffsetEXT" },
    { offsetof(EXTTransformFeedback, glBindBufferRange), "glBindBufferRangeEXT" },
    { offsetof(EXTTransformFeedback, glEndTransformFeedback), "glEndTransformFeedbackEXT" },
    { offsetof(EXTTransformFeedback, glGetTransformFeedbackVarying), "glGetTransformFeedbackVaryingEXT" },
    { offsetof(EXTTransformFeedback, glTransformFeedbackVaryings), "glTransformFeedbackVaryingsEXT" },
};

static const size_t __EXTTransformFeedbackEntryCount = sizeof(__EXTTransformFeedbackEntryPoints) /
    sizeof(__EXTTransformFeedbackEntryPoints[0]);

static const EntryOffset __EXTVertexArrayEntryPoints[] = {
    { offsetof(EXTVertexArray, glArrayElement), "glArrayElementEXT" },
    { offsetof(EXTVertexArray, glColorPointer), "glColorPointerEXT" },
    { offsetof(EXTVertexArray, glDrawArrays), "glDrawArraysEXT" },
    { offsetof(EXTVertexArray, glEdgeFlagPointer), "glEdgeFlagPointerEXT" },
    { offsetof(EXTVertexArray, glGetPointerv), "glGetPointervEXT" },
    { offsetof(EXTVertexArray, glIndexPointer), "glIndexPointerEXT" },
    { offsetof(EXTVertexArray, glNormalPointer), "glNormalPointerEXT" },
    { offsetof(EXTVertexArray, glTexCoordPointer), "glTexCoordPointerEXT" },
    { offsetof(EXTVertexArray, glVertexPointer), "glVertexPointerEXT" },
};

static const size_t __EXTVertexArrayEntryCount = sizeof(__EXTVertexArrayEntryPoints) /
    sizeof(__EXTVertexArrayEntryPoints[0]);

static const EntryOffset __EXTVertexAttrib64BITEntryPoints[] = {
    { offsetof(EXTVertexAttrib64BIT, glGetVertexAttribLdv), "glGetVertexAttribLdvEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribL1d), "glVertexAttribL1dEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribL1dv), "glVertexAttribL1dvEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribL2d), "glVertexAttribL2dEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribL2dv), "glVertexAttribL2dvEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribL3d), "glVertexAttribL3dEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribL3dv), "glVertexAttribL3dvEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribL4d), "glVertexAttribL4dEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribL4dv), "glVertexAttribL4dvEXT" },
    { offsetof(EXTVertexAttrib64BIT, glVertexAttribLPointer), "glVertexAttribLPointerEXT" },
};

static const size_t __EXTVertexAttrib64BITEntryCount = sizeof(__EXTVertexAttrib64BITEntryPoints) /
    sizeof(__EXTVertexAttrib64BITEntryPoints[0]);

static const EntryOffset __EXTVertexShaderEntryPoints[] = {
    { offsetof(EXTVertexShader, glBeginVertexShader), "glBeginVertexShaderEXT" },
    { offsetof(EXTVertexShader, glBindLightParameter), "glBindLightParameterEXT" },
    { offsetof(EXTVertexShader, glBindMaterialParameter), "glBindMaterialParameterEXT" },
    { offsetof(EXTVertexShader, glBindParameter), "glBindParameterEXT" },
    { offsetof(EXTVertexShader, glBindTexGenParameter), "glBindTexGenParameterEXT" },
    { offsetof(EXTVertexShader, glBindTextureUnitParameter), "glBindTextureUnitParameterEXT" },
    { offsetof(EXTVertexShader, glBindVertexShader), "glBindVertexShaderEXT" },
    { offsetof(EXTVertexShader, glDeleteVertexShader), "glDeleteVertexShaderEXT" },
    { offsetof(EXTVertexShader, glDisableVariantClientState), "glDisableVariantClientStateEXT" },
    { offsetof(EXTVertexShader, glEnableVariantClientState), "glEnableVariantClientStateEXT" },
    { offsetof(EXTVertexShader, glEndVertexShader), "glEndVertexShaderEXT" },
    { offsetof(EXTVertexShader, glExtractComponent), "glExtractComponentEXT" },
    { offsetof(EXTVertexShader, glGenSymbols), "glGenSymbolsEXT" },
    { offsetof(EXTVertexShader, glGenVertexShaders), "glGenVertexShadersEXT" },
    { offsetof(EXTVertexShader, glGetInvariantBooleanv), "glGetInvariantBooleanvEXT" },
    { offsetof(EXTVertexShader, glGetInvariantFloatv), "glGetInvariantFloatvEXT" },
    { offsetof(EXTVertexShader, glGetInvariantIntegerv), "glGetInvariantIntegervEXT" },
    { offsetof(EXTVertexShader, glGetLocalConstantBooleanv), "glGetLocalConstantBooleanvEXT" },
    { offsetof(EXTVertexShader, glGetLocalConstantFloatv), "glGetLocalConstantFloatvEXT" },
    { offsetof(EXTVertexShader, glGetLocalConstantIntegerv), "glGetLocalConstantIntegervEXT" },
    { offsetof(EXTVertexShader, glGetVariantBooleanv), "glGetVariantBooleanvEXT" },
    { offsetof(EXTVertexShader, glGetVariantFloatv), "glGetVariantFloatvEXT" },
    { offsetof(EXTVertexShader, glGetVariantIntegerv), "glGetVariantIntegervEXT" },
    { offsetof(EXTVertexShader, glGetVariantPointerv), "glGetVariantPointervEXT" },
    { offsetof(EXTVertexShader, glInsertComponent), "glInsertComponentEXT" },
    { offsetof(EXTVertexShader, glIsVariantEnabled), "glIsVariantEnabledEXT" },
    { offsetof(EXTVertexShader, glSetInvariant), "glSetInvariantEXT" },
    { offsetof(EXTVertexShader, glSetLocalConstant), "glSetLocalConstantEXT" },
    { offsetof(EXTVertexShader, glShaderOp1), "glShaderOp1EXT" },
    { offsetof(EXTVertexShader, glShaderOp2), "glShaderOp2EXT" },
    { offsetof(EXTVertexShader, glShaderOp3), "glShaderOp3EXT" },
    { offsetof(EXTVertexShader, glSwizzle), "glSwizzleEXT" },
    { offsetof(EXTVertexShader, glVariantPointer), "glVariantPointerEXT" },
    { offsetof(EXTVertexShader, glVariantbv), "glVariantbvEXT" },
    { offsetof(EXTVertexShader, glVariantdv), "glVariantdvEXT" },
    { offsetof(EXTVertexShader, glVariantfv), "glVariantfvEXT" },
    { offsetof(EXTVertexShader, glVariantiv), "glVariantivEXT" },
    { offsetof(EXTVertexShader, glVariantsv), "glVariantsvEXT" },
    { offsetof(EXTVertexShader, glVariantubv), "glVariantubvEXT" },
    { offsetof(EXTVertexShader, glVariantuiv), "glVariantuivEXT" },
    { offsetof(EXTVertexShader, glVariantusv), "glVariantusvEXT" },
    { offsetof(EXTVertexShader, glWriteMask), "glWriteMaskEXT" },
};

static const size_t __EXTVertexShaderEntryCount = sizeof(__EXTVertexShaderEntryPoints) /
    sizeof(__EXTVertexShaderEntryPoints[0]);

static const EntryOffset __EXTVertexWeightingEntryPoints[] = {
    { offsetof(EXTVertexWeighting, glVertexWeightPointer), "glVertexWeightPointerEXT" },
    { offsetof(EXTVertexWeighting, glVertexWeightf), "glVertexWeightfEXT" },
    { offsetof(EXTVertexWeighting, glVertexWeightfv), "glVertexWeightfvEXT" },
};

static const size_t __EXTVertexWeightingEntryCount = sizeof(__EXTVertexWeightingEntryPoints) /
    sizeof(__EXTVertexWeightingEntryPoints[0]);

static const EntryOffset __EXTWIN32KeyedMutexEntryPoints[] = {
    { offsetof(EXTWIN32KeyedMutex, glAcquireKeyedMutexWin32), "glAcquireKeyedMutexWin32EXT" },
    { offsetof(EXTWIN32KeyedMutex, glReleaseKeyedMutexWin32), "glReleaseKeyedMutexWin32EXT" },
};

static const size_t __EXTWIN32KeyedMutexEntryCount = sizeof(__EXTWIN32KeyedMutexEntryPoints) /
    sizeof(__EXTWIN32KeyedMutexEntryPoints[0]);

static const EntryOffset __EXTWindowRectanglesEntryPoints[] = {
    { offsetof(EXTWindowRectangles, glWindowRectangles), "glWindowRectanglesEXT" },
};

static const size_t __EXTWindowRectanglesEntryCount = sizeof(__EXTWindowRectanglesEntryPoints) /
    sizeof(__EXTWindowRectanglesEntryPoints[0]);

static const EntryOffset __EXTX11SyncObjectEntryPoints[] = {
    { offsetof(EXTX11SyncObject, glImportSync), "glImportSyncEXT" },
};

static const size_t __EXTX11SyncObjectEntryCount = sizeof(__EXTX11SyncObjectEntryPoints) /
    sizeof(__EXTX11SyncObjectEntryPoints[0]);

static const EntryOffset __GremedyFrameTerminatorEntryPoints[] = {
    { offsetof(GremedyFrameTerminator, glFrameTerminatorGREMEDY), "glFrameTerminatorGREMEDY" },
};

static const size_t __GremedyFrameTerminatorEntryCount = sizeof(__GremedyFrameTerminatorEntryPoints) /
    sizeof(__GremedyFrameTerminatorEntryPoints[0]);

static const EntryOffset __GremedyStringMarkerEntryPoints[] = {
    { offsetof(GremedyStringMarker, glStringMarkerGREMEDY), "glStringMarkerGREMEDY" },
};

static const size_t __GremedyStringMarkerEntryCount = sizeof(__GremedyStringMarkerEntryPoints) /
    sizeof(__GremedyStringMarkerEntryPoints[0]);

static const EntryOffset __HPImageTransformEntryPoints[] = {
    { offsetof(HPImageTransform, glGetImageTransformParameterfv), "glGetImageTransformParameterfvHP" },
    { offsetof(HPImageTransform, glGetImageTransformParameteriv), "glGetImageTransformParameterivHP" },
    { offsetof(HPImageTransform, glImageTransformParameterf), "glImageTransformParameterfHP" },
    { offsetof(HPImageTransform, glImageTransformParameterfv), "glImageTransformParameterfvHP" },
    { offsetof(HPImageTransform, glImageTransformParameteri), "glImageTransformParameteriHP" },
    { offsetof(HPImageTransform, glImageTransformParameteriv), "glImageTransformParameterivHP" },
};

static const size_t __HPImageTransformEntryCount = sizeof(__HPImageTransformEntryPoints) /
    sizeof(__HPImageTransformEntryPoints[0]);

static const EntryOffset __IBMMultimodeDrawArraysEntryPoints[] = {
    { offsetof(IBMMultimodeDrawArrays, glMultiModeDrawArrays), "glMultiModeDrawArraysIBM" },
    { offsetof(IBMMultimodeDrawArrays, glMultiModeDrawElements), "glMultiModeDrawElementsIBM" },
};

static const size_t __IBMMultimodeDrawArraysEntryCount = sizeof(__IBMMultimodeDrawArraysEntryPoints) /
    sizeof(__IBMMultimodeDrawArraysEntryPoints[0]);

static const EntryOffset __IBMStaticDataEntryPoints[] = {
    { offsetof(IBMStaticData, glFlushStaticData), "glFlushStaticDataIBM" },
};

static const size_t __IBMStaticDataEntryCount = sizeof(__IBMStaticDataEntryPoints) /
    sizeof(__IBMStaticDataEntryPoints[0]);

static const EntryOffset __IBMVertexArrayListsEntryPoints[] = {
    { offsetof(IBMVertexArrayLists, glColorPointerList), "glColorPointerListIBM" },
    { offsetof(IBMVertexArrayLists, glEdgeFlagPointerList), "glEdgeFlagPointerListIBM" },
    { offsetof(IBMVertexArrayLists, glFogCoordPointerList), "glFogCoordPointerListIBM" },
    { offsetof(IBMVertexArrayLists, glIndexPointerList), "glIndexPointerListIBM" },
    { offsetof(IBMVertexArrayLists, glNormalPointerList), "glNormalPointerListIBM" },
    { offsetof(IBMVertexArrayLists, glSecondaryColorPointerList), "glSecondaryColorPointerListIBM" },
    { offsetof(IBMVertexArrayLists, glTexCoordPointerList), "glTexCoordPointerListIBM" },
    { offsetof(IBMVertexArrayLists, glVertexPointerList), "glVertexPointerListIBM" },
};

static const size_t __IBMVertexArrayListsEntryCount = sizeof(__IBMVertexArrayListsEntryPoints) /
    sizeof(__IBMVertexArrayListsEntryPoints[0]);

static const EntryOffset __IngrBlendFuncSeparateEntryPoints[] = {
    { offsetof(IngrBlendFuncSeparate, glBlendFuncSeparateINGR), "glBlendFuncSeparateINGR" },
};

static const size_t __IngrBlendFuncSeparateEntryCount = sizeof(__IngrBlendFuncSeparateEntryPoints) /
    sizeof(__IngrBlendFuncSeparateEntryPoints[0]);

static const EntryOffset __INTELFramebufferCmaaEntryPoints[] = {
    { offsetof(INTELFramebufferCmaa, glApplyFramebufferAttachmentCMAA), "glApplyFramebufferAttachmentCMAAINTEL" },
};

static const size_t __INTELFramebufferCmaaEntryCount = sizeof(__INTELFramebufferCmaaEntryPoints) /
    sizeof(__INTELFramebufferCmaaEntryPoints[0]);

static const EntryOffset __INTELMapTextureEntryPoints[] = {
    { offsetof(INTELMapTexture, glMapTexture2D), "glMapTexture2DINTEL" },
    { offsetof(INTELMapTexture, glSyncTexture), "glSyncTextureINTEL" },
    { offsetof(INTELMapTexture, glUnmapTexture2D), "glUnmapTexture2DINTEL" },
};

static const size_t __INTELMapTextureEntryCount = sizeof(__INTELMapTextureEntryPoints) /
    sizeof(__INTELMapTextureEntryPoints[0]);

static const EntryOffset __INTELParallelArraysEntryPoints[] = {
    { offsetof(INTELParallelArrays, glColorPointerv), "glColorPointervINTEL" },
    { offsetof(INTELParallelArrays, glNormalPointerv), "glNormalPointervINTEL" },
    { offsetof(INTELParallelArrays, glTexCoordPointerv), "glTexCoordPointervINTEL" },
    { offsetof(INTELParallelArrays, glVertexPointerv), "glVertexPointervINTEL" },
};

static const size_t __INTELParallelArraysEntryCount = sizeof(__INTELParallelArraysEntryPoints) /
    sizeof(__INTELParallelArraysEntryPoints[0]);

static const EntryOffset __INTELPerformanceQueryEntryPoints[] = {
    { offsetof(INTELPerformanceQuery, glBeginPerfQuery), "glBeginPerfQueryINTEL" },
    { offsetof(INTELPerformanceQuery, glCreatePerfQuery), "glCreatePerfQueryINTEL" },
    { offsetof(INTELPerformanceQuery, glDeletePerfQuery), "glDeletePerfQueryINTEL" },
    { offsetof(INTELPerformanceQuery, glEndPerfQuery), "glEndPerfQueryINTEL" },
    { offsetof(INTELPerformanceQuery, glGetFirstPerfQueryId), "glGetFirstPerfQueryIdINTEL" },
    { offsetof(INTELPerformanceQuery, glGetNextPerfQueryId), "glGetNextPerfQueryIdINTEL" },
    { offsetof(INTELPerformanceQuery, glGetPerfCounterInfo), "glGetPerfCounterInfoINTEL" },
    { offsetof(INTELPerformanceQuery, glGetPerfQueryData), "glGetPerfQueryDataINTEL" },
    { offsetof(INTELPerformanceQuery, glGetPerfQueryIdByName), "glGetPerfQueryIdByNameINTEL" },
    { offsetof(INTELPerformanceQuery, glGetPerfQueryInfo), "glGetPerfQueryInfoINTEL" },
};

static const size_t __INTELPerformanceQueryEntryCount = sizeof(__INTELPerformanceQueryEntryPoints) /
    sizeof(__INTELPerformanceQueryEntryPoints[0]);

static const EntryOffset __KHRBlendEquationAdvancedEntryPoints[] = {
    { offsetof(KHRBlendEquationAdvanced, glBlendBarrier), "glBlendBarrierKHR" },
};

static const size_t __KHRBlendEquationAdvancedEntryCount = sizeof(__KHRBlendEquationAdvancedEntryPoints) /
    sizeof(__KHRBlendEquationAdvancedEntryPoints[0]);

static const EntryOffset __KHRParallelShaderCompileEntryPoints[] = {
    { offsetof(KHRParallelShaderCompile, glMaxShaderCompilerThreads), "glMaxShaderCompilerThreadsKHR" },
};

static const size_t __KHRParallelShaderCompileEntryCount = sizeof(__KHRParallelShaderCompileEntryPoints) /
    sizeof(__KHRParallelShaderCompileEntryPoints[0]);

static const EntryOffset __MESAFramebufferFlipYEntryPoints[] = {
    { offsetof(MESAFramebufferFlipY, glFramebufferParameteri), "glFramebufferParameteriMESA" },
    { offsetof(MESAFramebufferFlipY, glGetFramebufferParameteriv), "glGetFramebufferParameterivMESA" },
};

static const size_t __MESAFramebufferFlipYEntryCount = sizeof(__MESAFramebufferFlipYEntryPoints) /
    sizeof(__MESAFramebufferFlipYEntryPoints[0]);

static const EntryOffset __MESAResizeBuffersEntryPoints[] = {
    { offsetof(MESAResizeBuffers, glResizeBuffers), "glResizeBuffersMESA" },
};

static const size_t __MESAResizeBuffersEntryCount = sizeof(__MESAResizeBuffersEntryPoints) /
    sizeof(__MESAResizeBuffersEntryPoints[0]);

static const EntryOffset __MESAWindowPosEntryPoints[] = {
    { offsetof(MESAWindowPos, glWindowPos2d), "glWindowPos2dMESA" },
    { offsetof(MESAWindowPos, glWindowPos2dv), "glWindowPos2dvMESA" },
    { offsetof(MESAWindowPos, glWindowPos2f), "glWindowPos2fMESA" },
    { offsetof(MESAWindowPos, glWindowPos2fv), "glWindowPos2fvMESA" },
    { offsetof(MESAWindowPos, glWindowPos2i), "glWindowPos2iMESA" },
    { offsetof(MESAWindowPos, glWindowPos2iv), "glWindowPos2ivMESA" },
    { offsetof(MESAWindowPos, glWindowPos2s), "glWindowPos2sMESA" },
    { offsetof(MESAWindowPos, glWindowPos2sv), "glWindowPos2svMESA" },
    { offsetof(MESAWindowPos, glWindowPos3d), "glWindowPos3dMESA" },
    { offsetof(MESAWindowPos, glWindowPos3dv), "glWindowPos3dvMESA" },
    { offsetof(MESAWindowPos, glWindowPos3f), "glWindowPos3fMESA" },
    { offsetof(MESAWindowPos, glWindowPos3fv), "glWindowPos3fvMESA" },
    { offsetof(MESAWindowPos, glWindowPos3i), "glWindowPos3iMESA" },
    { offsetof(MESAWindowPos, glWindowPos3iv), "glWindowPos3ivMESA" },
    { offsetof(MESAWindowPos, glWindowPos3s), "glWindowPos3sMESA" },
    { offsetof(MESAWindowPos, glWindowPos3sv), "glWindowPos3svMESA" },
    { offsetof(MESAWindowPos, glWindowPos4d), "glWindowPos4dMESA" },
    { offsetof(MESAWindowPos, glWindowPos4dv), "glWindowPos4dvMESA" },
    { offsetof(MESAWindowPos, glWindowPos4f), "glWindowPos4fMESA" },
    { offsetof(MESAWindowPos, glWindowPos4fv), "glWindowPos4fvMESA" },
    { offsetof(MESAWindowPos, glWindowPos4i), "glWindowPos4iMESA" },
    { offsetof(MESAWindowPos, glWindowPos4iv), "glWindowPos4ivMESA" },
    { offsetof(MESAWindowPos, glWindowPos4s), "glWindowPos4sMESA" },
    { offsetof(MESAWindowPos, glWindowPos4sv), "glWindowPos4svMESA" },
};

static const size_t __MESAWindowPosEntryCount = sizeof(__MESAWindowPosEntryPoints) /
    sizeof(__MESAWindowPosEntryPoints[0]);

static const EntryOffset __NVXConditionalRenderEntryPoints[] = {
    { offsetof(NVXConditionalRender, glBeginConditionalRender), "glBeginConditionalRenderNVX" },
    { offsetof(NVXConditionalRender, glEndConditionalRender), "glEndConditionalRenderNVX" },
};

static const size_t __NVXConditionalRenderEntryCount = sizeof(__NVXConditionalRenderEntryPoints) /
    sizeof(__NVXConditionalRenderEntryPoints[0]);

static const EntryOffset __NVXGPUMULTICAST2EntryPoints[] = {
    { offsetof(NVXGPUMULTICAST2, glAsyncCopyBufferSubData), "glAsyncCopyBufferSubDataNVX" },
    { offsetof(NVXGPUMULTICAST2, glAsyncCopyImageSubData), "glAsyncCopyImageSubDataNVX" },
    { offsetof(NVXGPUMULTICAST2, glMulticastScissorArrayv), "glMulticastScissorArrayvNVX" },
    { offsetof(NVXGPUMULTICAST2, glMulticastViewportArrayv), "glMulticastViewportArrayvNVX" },
    { offsetof(NVXGPUMULTICAST2, glMulticastViewportPositionWScale), "glMulticastViewportPositionWScaleNVX" },
    { offsetof(NVXGPUMULTICAST2, glUploadGpuMask), "glUploadGpuMaskNVX" },
};

static const size_t __NVXGPUMULTICAST2EntryCount = sizeof(__NVXGPUMULTICAST2EntryPoints) /
    sizeof(__NVXGPUMULTICAST2EntryPoints[0]);

static const EntryOffset __NVXLinkedGPUMulticastEntryPoints[] = {
    { offsetof(NVXLinkedGPUMulticast, glLGPUCopyImageSubData), "glLGPUCopyImageSubDataNVX" },
    { offsetof(NVXLinkedGPUMulticast, glLGPUInterlock), "glLGPUInterlockNVX" },
    { offsetof(NVXLinkedGPUMulticast, glLGPUNamedBufferSubData), "glLGPUNamedBufferSubDataNVX" },
};

static const size_t __NVXLinkedGPUMulticastEntryCount = sizeof(__NVXLinkedGPUMulticastEntryPoints) /
    sizeof(__NVXLinkedGPUMulticastEntryPoints[0]);

static const EntryOffset __NVXProgressFenceEntryPoints[] = {
    { offsetof(NVXProgressFence, glClientWaitSemaphoreui64), "glClientWaitSemaphoreui64NVX" },
    { offsetof(NVXProgressFence, glCreateProgressFence), "glCreateProgressFenceNVX" },
    { offsetof(NVXProgressFence, glSignalSemaphoreui64), "glSignalSemaphoreui64NVX" },
    { offsetof(NVXProgressFence, glWaitSemaphoreui64), "glWaitSemaphoreui64NVX" },
};

static const size_t __NVXProgressFenceEntryCount = sizeof(__NVXProgressFenceEntryPoints) /
    sizeof(__NVXProgressFenceEntryPoints[0]);

static const EntryOffset __NVAlphaToCoverageDitherControlEntryPoints[] = {
    { offsetof(NVAlphaToCoverageDitherControl, glAlphaToCoverageDitherControl), "glAlphaToCoverageDitherControlNV" },
};

static const size_t __NVAlphaToCoverageDitherControlEntryCount = sizeof(__NVAlphaToCoverageDitherControlEntryPoints) /
    sizeof(__NVAlphaToCoverageDitherControlEntryPoints[0]);

static const EntryOffset __NVBindlessMultiDrawIndirectEntryPoints[] = {
    { offsetof(NVBindlessMultiDrawIndirect, glMultiDrawArraysIndirectBindless), "glMultiDrawArraysIndirectBindlessNV" },
    { offsetof(NVBindlessMultiDrawIndirect, glMultiDrawElementsIndirectBindless), "glMultiDrawElementsIndirectBindlessNV" },
};

static const size_t __NVBindlessMultiDrawIndirectEntryCount = sizeof(__NVBindlessMultiDrawIndirectEntryPoints) /
    sizeof(__NVBindlessMultiDrawIndirectEntryPoints[0]);

static const EntryOffset __NVBindlessMultiDrawIndirectCountEntryPoints[] = {
    { offsetof(NVBindlessMultiDrawIndirectCount, glMultiDrawArraysIndirectBindlessCount), "glMultiDrawArraysIndirectBindlessCountNV" },
    { offsetof(NVBindlessMultiDrawIndirectCount, glMultiDrawElementsIndirectBindlessCount), "glMultiDrawElementsIndirectBindlessCountNV" },
};

static const size_t __NVBindlessMultiDrawIndirectCountEntryCount = sizeof(__NVBindlessMultiDrawIndirectCountEntryPoints) /
    sizeof(__NVBindlessMultiDrawIndirectCountEntryPoints[0]);

static const EntryOffset __NVBindlessTextureEntryPoints[] = {
    { offsetof(NVBindlessTexture, glGetImageHandle), "glGetImageHandleNV" },
    { offsetof(NVBindlessTexture, glGetTextureHandle), "glGetTextureHandleNV" },
    { offsetof(NVBindlessTexture, glGetTextureSamplerHandle), "glGetTextureSamplerHandleNV" },
    { offsetof(NVBindlessTexture, glIsImageHandleResident), "glIsImageHandleResidentNV" },
    { offsetof(NVBindlessTexture, glIsTextureHandleResident), "glIsTextureHandleResidentNV" },
    { offsetof(NVBindlessTexture, glMakeImageHandleNonResident), "glMakeImageHandleNonResidentNV" },
    { offsetof(NVBindlessTexture, glMakeImageHandleResident), "glMakeImageHandleResidentNV" },
    { offsetof(NVBindlessTexture, glMakeTextureHandleNonResident), "glMakeTextureHandleNonResidentNV" },
    { offsetof(NVBindlessTexture, glMakeTextureHandleResident), "glMakeTextureHandleResidentNV" },
    { offsetof(NVBindlessTexture, glProgramUniformHandleui64), "glProgramUniformHandleui64NV" },
    { offsetof(NVBindlessTexture, glProgramUniformHandleui64v), "glProgramUniformHandleui64vNV" },
    { offsetof(NVBindlessTexture, glUniformHandleui64), "glUniformHandleui64NV" },
    { offsetof(NVBindlessTexture, glUniformHandleui64v), "glUniformHandleui64vNV" },
};

static const size_t __NVBindlessTextureEntryCount = sizeof(__NVBindlessTextureEntryPoints) /
    sizeof(__NVBindlessTextureEntryPoints[0]);

static const EntryOffset __NVBlendEquationAdvancedEntryPoints[] = {
    { offsetof(NVBlendEquationAdvanced, glBlendBarrier), "glBlendBarrierNV" },
    { offsetof(NVBlendEquationAdvanced, glBlendParameteri), "glBlendParameteriNV" },
};

static const size_t __NVBlendEquationAdvancedEntryCount = sizeof(__NVBlendEquationAdvancedEntryPoints) /
    sizeof(__NVBlendEquationAdvancedEntryPoints[0]);

static const EntryOffset __NVClipSpaceWScalingEntryPoints[] = {
    { offsetof(NVClipSpaceWScaling, glViewportPositionWScale), "glViewportPositionWScaleNV" },
};

static const size_t __NVClipSpaceWScalingEntryCount = sizeof(__NVClipSpaceWScalingEntryPoints) /
    sizeof(__NVClipSpaceWScalingEntryPoints[0]);

static const EntryOffset __NVCommandListEntryPoints[] = {
    { offsetof(NVCommandList, glCallCommandList), "glCallCommandListNV" },
    { offsetof(NVCommandList, glCommandListSegments), "glCommandListSegmentsNV" },
    { offsetof(NVCommandList, glCompileCommandList), "glCompileCommandListNV" },
    { offsetof(NVCommandList, glCreateCommandLists), "glCreateCommandListsNV" },
    { offsetof(NVCommandList, glCreateStates), "glCreateStatesNV" },
    { offsetof(NVCommandList, glDeleteCommandLists), "glDeleteCommandListsNV" },
    { offsetof(NVCommandList, glDeleteStates), "glDeleteStatesNV" },
    { offsetof(NVCommandList, glDrawCommandsAddress), "glDrawCommandsAddressNV" },
    { offsetof(NVCommandList, glDrawCommands), "glDrawCommandsNV" },
    { offsetof(NVCommandList, glDrawCommandsStatesAddress), "glDrawCommandsStatesAddressNV" },
    { offsetof(NVCommandList, glDrawCommandsStates), "glDrawCommandsStatesNV" },
    { offsetof(NVCommandList, glGetCommandHeader), "glGetCommandHeaderNV" },
    { offsetof(NVCommandList, glGetStageIndex), "glGetStageIndexNV" },
    { offsetof(NVCommandList, glIsCommandList), "glIsCommandListNV" },
    { offsetof(NVCommandList, glIsState), "glIsStateNV" },
    { offsetof(NVCommandList, glListDrawCommandsStatesClient), "glListDrawCommandsStatesClientNV" },
    { offsetof(NVCommandList, glStateCapture), "glStateCaptureNV" },
};

static const size_t __NVCommandListEntryCount = sizeof(__NVCommandListEntryPoints) /
    sizeof(__NVCommandListEntryPoints[0]);

static const EntryOffset __NVConditionalRenderEntryPoints[] = {
    { offsetof(NVConditionalRender, glBeginConditionalRender), "glBeginConditionalRenderNV" },
    { offsetof(NVConditionalRender, glEndConditionalRender), "glEndConditionalRenderNV" },
};

static const size_t __NVConditionalRenderEntryCount = sizeof(__NVConditionalRenderEntryPoints) /
    sizeof(__NVConditionalRenderEntryPoints[0]);

static const EntryOffset __NVConservativeRasterEntryPoints[] = {
    { offsetof(NVConservativeRaster, glSubpixelPrecisionBias), "glSubpixelPrecisionBiasNV" },
};

static const size_t __NVConservativeRasterEntryCount = sizeof(__NVConservativeRasterEntryPoints) /
    sizeof(__NVConservativeRasterEntryPoints[0]);

static const EntryOffset __NVConservativeRasterDilateEntryPoints[] = {
    { offsetof(NVConservativeRasterDilate, glConservativeRasterParameterf), "glConservativeRasterParameterfNV" },
};

static const size_t __NVConservativeRasterDilateEntryCount = sizeof(__NVConservativeRasterDilateEntryPoints) /
    sizeof(__NVConservativeRasterDilateEntryPoints[0]);

static const EntryOffset __NVConservativeRasterPreSnapTrianglesEntryPoints[] = {
    { offsetof(NVConservativeRasterPreSnapTriangles, glConservativeRasterParameteri), "glConservativeRasterParameteriNV" },
};

static const size_t __NVConservativeRasterPreSnapTrianglesEntryCount = sizeof(__NVConservativeRasterPreSnapTrianglesEntryPoints) /
    sizeof(__NVConservativeRasterPreSnapTrianglesEntryPoints[0]);

static const EntryOffset __NVCopyImageEntryPoints[] = {
    { offsetof(NVCopyImage, glCopyImageSubData), "glCopyImageSubDataNV" },
};

static const size_t __NVCopyImageEntryCount = sizeof(__NVCopyImageEntryPoints) /
    sizeof(__NVCopyImageEntryPoints[0]);

static const EntryOffset __NVDepthBufferFloatEntryPoints[] = {
    { offsetof(NVDepthBufferFloat, glClearDepthd), "glClearDepthdNV" },
    { offsetof(NVDepthBufferFloat, glDepthBoundsd), "glDepthBoundsdNV" },
    { offsetof(NVDepthBufferFloat, glDepthRanged), "glDepthRangedNV" },
};

static const size_t __NVDepthBufferFloatEntryCount = sizeof(__NVDepthBufferFloatEntryPoints) /
    sizeof(__NVDepthBufferFloatEntryPoints[0]);

static const EntryOffset __NVDrawTextureEntryPoints[] = {
    { offsetof(NVDrawTexture, glDrawTexture), "glDrawTextureNV" },
};

static const size_t __NVDrawTextureEntryCount = sizeof(__NVDrawTextureEntryPoints) /
    sizeof(__NVDrawTextureEntryPoints[0]);

static const EntryOffset __NVDrawVulkanImageEntryPoints[] = {
    { offsetof(NVDrawVulkanImage, glDrawVkImage), "glDrawVkImageNV" },
    { offsetof(NVDrawVulkanImage, glGetVkProcAddr), "glGetVkProcAddrNV" },
    { offsetof(NVDrawVulkanImage, glSignalVkFence), "glSignalVkFenceNV" },
    { offsetof(NVDrawVulkanImage, glSignalVkSemaphore), "glSignalVkSemaphoreNV" },
    { offsetof(NVDrawVulkanImage, glWaitVkSemaphore), "glWaitVkSemaphoreNV" },
};

static const size_t __NVDrawVulkanImageEntryCount = sizeof(__NVDrawVulkanImageEntryPoints) /
    sizeof(__NVDrawVulkanImageEntryPoints[0]);

static const EntryOffset __NVEvaluatorsEntryPoints[] = {
    { offsetof(NVEvaluators, glEvalMaps), "glEvalMapsNV" },
    { offsetof(NVEvaluators, glGetMapAttribParameterfv), "glGetMapAttribParameterfvNV" },
    { offsetof(NVEvaluators, glGetMapAttribParameteriv), "glGetMapAttribParameterivNV" },
    { offsetof(NVEvaluators, glGetMapControlPoints), "glGetMapControlPointsNV" },
    { offsetof(NVEvaluators, glGetMapParameterfv), "glGetMapParameterfvNV" },
    { offsetof(NVEvaluators, glGetMapParameteriv), "glGetMapParameterivNV" },
    { offsetof(NVEvaluators, glMapControlPoints), "glMapControlPointsNV" },
    { offsetof(NVEvaluators, glMapParameterfv), "glMapParameterfvNV" },
    { offsetof(NVEvaluators, glMapParameteriv), "glMapParameterivNV" },
};

static const size_t __NVEvaluatorsEntryCount = sizeof(__NVEvaluatorsEntryPoints) /
    sizeof(__NVEvaluatorsEntryPoints[0]);

static const EntryOffset __NVExplicitMultisampleEntryPoints[] = {
    { offsetof(NVExplicitMultisample, glGetMultisamplefv), "glGetMultisamplefvNV" },
    { offsetof(NVExplicitMultisample, glSampleMaskIndexed), "glSampleMaskIndexedNV" },
    { offsetof(NVExplicitMultisample, glTexRenderbuffer), "glTexRenderbufferNV" },
};

static const size_t __NVExplicitMultisampleEntryCount = sizeof(__NVExplicitMultisampleEntryPoints) /
    sizeof(__NVExplicitMultisampleEntryPoints[0]);

static const EntryOffset __NVFenceEntryPoints[] = {
    { offsetof(NVFence, glDeleteFences), "glDeleteFencesNV" },
    { offsetof(NVFence, glFinishFence), "glFinishFenceNV" },
    { offsetof(NVFence, glGenFences), "glGenFencesNV" },
    { offsetof(NVFence, glGetFenceiv), "glGetFenceivNV" },
    { offsetof(NVFence, glIsFence), "glIsFenceNV" },
    { offsetof(NVFence, glSetFence), "glSetFenceNV" },
    { offsetof(NVFence, glTestFence), "glTestFenceNV" },
};

static const size_t __NVFenceEntryCount = sizeof(__NVFenceEntryPoints) /
    sizeof(__NVFenceEntryPoints[0]);

static const EntryOffset __NVFragmentCoverageToColorEntryPoints[] = {
    { offsetof(NVFragmentCoverageToColor, glFragmentCoverageColor), "glFragmentCoverageColorNV" },
};

static const size_t __NVFragmentCoverageToColorEntryCount = sizeof(__NVFragmentCoverageToColorEntryPoints) /
    sizeof(__NVFragmentCoverageToColorEntryPoints[0]);

static const EntryOffset __NVFragmentProgramEntryPoints[] = {
    { offsetof(NVFragmentProgram, glGetProgramNamedParameterdv), "glGetProgramNamedParameterdvNV" },
    { offsetof(NVFragmentProgram, glGetProgramNamedParameterfv), "glGetProgramNamedParameterfvNV" },
    { offsetof(NVFragmentProgram, glProgramNamedParameter4d), "glProgramNamedParameter4dNV" },
    { offsetof(NVFragmentProgram, glProgramNamedParameter4dv), "glProgramNamedParameter4dvNV" },
    { offsetof(NVFragmentProgram, glProgramNamedParameter4f), "glProgramNamedParameter4fNV" },
    { offsetof(NVFragmentProgram, glProgramNamedParameter4fv), "glProgramNamedParameter4fvNV" },
};

static const size_t __NVFragmentProgramEntryCount = sizeof(__NVFragmentProgramEntryPoints) /
    sizeof(__NVFragmentProgramEntryPoints[0]);

static const EntryOffset __NVFramebufferMixedSamplesEntryPoints[] = {
    { offsetof(NVFramebufferMixedSamples, glCoverageModulation), "glCoverageModulationNV" },
    { offsetof(NVFramebufferMixedSamples, glCoverageModulationTable), "glCoverageModulationTableNV" },
    { offsetof(NVFramebufferMixedSamples, glGetCoverageModulationTable), "glGetCoverageModulationTableNV" },
};

static const size_t __NVFramebufferMixedSamplesEntryCount = sizeof(__NVFramebufferMixedSamplesEntryPoints) /
    sizeof(__NVFramebufferMixedSamplesEntryPoints[0]);

static const EntryOffset __NVFramebufferMultisampleCoverageEntryPoints[] = {
    { offsetof(NVFramebufferMultisampleCoverage, glRenderbufferStorageMultisampleCoverage), "glRenderbufferStorageMultisampleCoverageNV" },
};

static const size_t __NVFramebufferMultisampleCoverageEntryCount = sizeof(__NVFramebufferMultisampleCoverageEntryPoints) /
    sizeof(__NVFramebufferMultisampleCoverageEntryPoints[0]);

static const EntryOffset __NVGeometryPROGRAM4EntryPoints[] = {
    { offsetof(NVGeometryPROGRAM4, glFramebufferTexture), "glFramebufferTextureEXT" },
    { offsetof(NVGeometryPROGRAM4, glFramebufferTextureFace), "glFramebufferTextureFaceEXT" },
    { offsetof(NVGeometryPROGRAM4, glProgramVertexLimit), "glProgramVertexLimitNV" },
};

static const size_t __NVGeometryPROGRAM4EntryCount = sizeof(__NVGeometryPROGRAM4EntryPoints) /
    sizeof(__NVGeometryPROGRAM4EntryPoints[0]);

static const EntryOffset __NVGPUMulticastEntryPoints[] = {
    { offsetof(NVGPUMulticast, glMulticastBarrier), "glMulticastBarrierNV" },
    { offsetof(NVGPUMulticast, glMulticastBlitFramebuffer), "glMulticastBlitFramebufferNV" },
    { offsetof(NVGPUMulticast, glMulticastBufferSubData), "glMulticastBufferSubDataNV" },
    { offsetof(NVGPUMulticast, glMulticastCopyBufferSubData), "glMulticastCopyBufferSubDataNV" },
    { offsetof(NVGPUMulticast, glMulticastCopyImageSubData), "glMulticastCopyImageSubDataNV" },
    { offsetof(NVGPUMulticast, glMulticastFramebufferSampleLocationsfv), "glMulticastFramebufferSampleLocationsfvNV" },
    { offsetof(NVGPUMulticast, glMulticastGetQueryObjecti64v), "glMulticastGetQueryObjecti64vNV" },
    { offsetof(NVGPUMulticast, glMulticastGetQueryObjectiv), "glMulticastGetQueryObjectivNV" },
    { offsetof(NVGPUMulticast, glMulticastGetQueryObjectui64v), "glMulticastGetQueryObjectui64vNV" },
    { offsetof(NVGPUMulticast, glMulticastGetQueryObjectuiv), "glMulticastGetQueryObjectuivNV" },
    { offsetof(NVGPUMulticast, glMulticastWaitSync), "glMulticastWaitSyncNV" },
    { offsetof(NVGPUMulticast, glRenderGpuMask), "glRenderGpuMaskNV" },
};

static const size_t __NVGPUMulticastEntryCount = sizeof(__NVGPUMulticastEntryPoints) /
    sizeof(__NVGPUMulticastEntryPoints[0]);

static const EntryOffset __NVGPUPROGRAM4EntryPoints[] = {
    { offsetof(NVGPUPROGRAM4, glGetProgramEnvParameterIiv), "glGetProgramEnvParameterIivNV" },
    { offsetof(NVGPUPROGRAM4, glGetProgramEnvParameterIuiv), "glGetProgramEnvParameterIuivNV" },
    { offsetof(NVGPUPROGRAM4, glGetProgramLocalParameterIiv), "glGetProgramLocalParameterIivNV" },
    { offsetof(NVGPUPROGRAM4, glGetProgramLocalParameterIuiv), "glGetProgramLocalParameterIuivNV" },
    { offsetof(NVGPUPROGRAM4, glProgramEnvParameterI4i), "glProgramEnvParameterI4iNV" },
    { offsetof(NVGPUPROGRAM4, glProgramEnvParameterI4iv), "glProgramEnvParameterI4ivNV" },
    { offsetof(NVGPUPROGRAM4, glProgramEnvParameterI4ui), "glProgramEnvParameterI4uiNV" },
    { offsetof(NVGPUPROGRAM4, glProgramEnvParameterI4uiv), "glProgramEnvParameterI4uivNV" },
    { offsetof(NVGPUPROGRAM4, glProgramEnvParametersI4iv), "glProgramEnvParametersI4ivNV" },
    { offsetof(NVGPUPROGRAM4, glProgramEnvParametersI4uiv), "glProgramEnvParametersI4uivNV" },
    { offsetof(NVGPUPROGRAM4, glProgramLocalParameterI4i), "glProgramLocalParameterI4iNV" },
    { offsetof(NVGPUPROGRAM4, glProgramLocalParameterI4iv), "glProgramLocalParameterI4ivNV" },
    { offsetof(NVGPUPROGRAM4, glProgramLocalParameterI4ui), "glProgramLocalParameterI4uiNV" },
    { offsetof(NVGPUPROGRAM4, glProgramLocalParameterI4uiv), "glProgramLocalParameterI4uivNV" },
    { offsetof(NVGPUPROGRAM4, glProgramLocalParametersI4iv), "glProgramLocalParametersI4ivNV" },
    { offsetof(NVGPUPROGRAM4, glProgramLocalParametersI4uiv), "glProgramLocalParametersI4uivNV" },
};

static const size_t __NVGPUPROGRAM4EntryCount = sizeof(__NVGPUPROGRAM4EntryPoints) /
    sizeof(__NVGPUPROGRAM4EntryPoints[0]);

static const EntryOffset __NVGPUPROGRAM5EntryPoints[] = {
    { offsetof(NVGPUPROGRAM5, glGetProgramSubroutineParameteruiv), "glGetProgramSubroutineParameteruivNV" },
    { offsetof(NVGPUPROGRAM5, glProgramSubroutineParametersuiv), "glProgramSubroutineParametersuivNV" },
};

static const size_t __NVGPUPROGRAM5EntryCount = sizeof(__NVGPUPROGRAM5EntryPoints) /
    sizeof(__NVGPUPROGRAM5EntryPoints[0]);

static const EntryOffset __NVHalfFloatEntryPoints[] = {
    { offsetof(NVHalfFloat, glColor3h), "glColor3hNV" },
    { offsetof(NVHalfFloat, glColor3hv), "glColor3hvNV" },
    { offsetof(NVHalfFloat, glColor4h), "glColor4hNV" },
    { offsetof(NVHalfFloat, glColor4hv), "glColor4hvNV" },
    { offsetof(NVHalfFloat, glFogCoordh), "glFogCoordhNV" },
    { offsetof(NVHalfFloat, glFogCoordhv), "glFogCoordhvNV" },
    { offsetof(NVHalfFloat, glMultiTexCoord1h), "glMultiTexCoord1hNV" },
    { offsetof(NVHalfFloat, glMultiTexCoord1hv), "glMultiTexCoord1hvNV" },
    { offsetof(NVHalfFloat, glMultiTexCoord2h), "glMultiTexCoord2hNV" },
    { offsetof(NVHalfFloat, glMultiTexCoord2hv), "glMultiTexCoord2hvNV" },
    { offsetof(NVHalfFloat, glMultiTexCoord3h), "glMultiTexCoord3hNV" },
    { offsetof(NVHalfFloat, glMultiTexCoord3hv), "glMultiTexCoord3hvNV" },
    { offsetof(NVHalfFloat, glMultiTexCoord4h), "glMultiTexCoord4hNV" },
    { offsetof(NVHalfFloat, glMultiTexCoord4hv), "glMultiTexCoord4hvNV" },
    { offsetof(NVHalfFloat, glNormal3h), "glNormal3hNV" },
    { offsetof(NVHalfFloat, glNormal3hv), "glNormal3hvNV" },
    { offsetof(NVHalfFloat, glSecondaryColor3h), "glSecondaryColor3hNV" },
    { offsetof(NVHalfFloat, glSecondaryColor3hv), "glSecondaryColor3hvNV" },
    { offsetof(NVHalfFloat, glTexCoord1h), "glTexCoord1hNV" },
    { offsetof(NVHalfFloat, glTexCoord1hv), "glTexCoord1hvNV" },
    { offsetof(NVHalfFloat, glTexCoord2h), "glTexCoord2hNV" },
    { offsetof(NVHalfFloat, glTexCoord2hv), "glTexCoord2hvNV" },
    { offsetof(NVHalfFloat, glTexCoord3h), "glTexCoord3hNV" },
    { offsetof(NVHalfFloat, glTexCoord3hv), "glTexCoord3hvNV" },
    { offsetof(NVHalfFloat, glTexCoord4h), "glTexCoord4hNV" },
    { offsetof(NVHalfFloat, glTexCoord4hv), "glTexCoord4hvNV" },
    { offsetof(NVHalfFloat, glVertex2h), "glVertex2hNV" },
    { offsetof(NVHalfFloat, glVertex2hv), "glVertex2hvNV" },
    { offsetof(NVHalfFloat, glVertex3h), "glVertex3hNV" },
    { offsetof(NVHalfFloat, glVertex3hv), "glVertex3hvNV" },
    { offsetof(NVHalfFloat, glVertex4h), "glVertex4hNV" },
    { offsetof(NVHalfFloat, glVertex4hv), "glVertex4hvNV" },
    { offsetof(NVHalfFloat, glVertexAttrib1h), "glVertexAttrib1hNV" },
    { offsetof(NVHalfFloat, glVertexAttrib1hv), "glVertexAttrib1hvNV" },
    { offsetof(NVHalfFloat, glVertexAttrib2h), "glVertexAttrib2hNV" },
    { offsetof(NVHalfFloat, glVertexAttrib2hv), "glVertexAttrib2hvNV" },
    { offsetof(NVHalfFloat, glVertexAttrib3h), "glVertexAttrib3hNV" },
    { offsetof(NVHalfFloat, glVertexAttrib3hv), "glVertexAttrib3hvNV" },
    { offsetof(NVHalfFloat, glVertexAttrib4h), "glVertexAttrib4hNV" },
    { offsetof(NVHalfFloat, glVertexAttrib4hv), "glVertexAttrib4hvNV" },
    { offsetof(NVHalfFloat, glVertexAttribs1hv), "glVertexAttribs1hvNV" },
    { offsetof(NVHalfFloat, glVertexAttribs2hv), "glVertexAttribs2hvNV" },
    { offsetof(NVHalfFloat, glVertexAttribs3hv), "glVertexAttribs3hvNV" },
    { offsetof(NVHalfFloat, glVertexAttribs4hv), "glVertexAttribs4hvNV" },
    { offsetof(NVHalfFloat, glVertexWeighth), "glVertexWeighthNV" },
    { offsetof(NVHalfFloat, glVertexWeighthv), "glVertexWeighthvNV" },
};

static const size_t __NVHalfFloatEntryCount = sizeof(__NVHalfFloatEntryPoints) /
    sizeof(__NVHalfFloatEntryPoints[0]);

static const EntryOffset __NVInternalformatSampleQueryEntryPoints[] = {
    { offsetof(NVInternalformatSampleQuery, glGetInternalformatSampleiv), "glGetInternalformatSampleivNV" },
};

static const size_t __NVInternalformatSampleQueryEntryCount = sizeof(__NVInternalformatSampleQueryEntryPoints) /
    sizeof(__NVInternalformatSampleQueryEntryPoints[0]);

static const EntryOffset __NVMemoryAttachmentEntryPoints[] = {
    { offsetof(NVMemoryAttachment, glBufferAttachMemory), "glBufferAttachMemoryNV" },
    { offsetof(NVMemoryAttachment, glGetMemoryObjectDetachedResourcesuiv), "glGetMemoryObjectDetachedResourcesuivNV" },
    { offsetof(NVMemoryAttachment, glNamedBufferAttachMemory), "glNamedBufferAttachMemoryNV" },
    { offsetof(NVMemoryAttachment, glResetMemoryObjectParameter), "glResetMemoryObjectParameterNV" },
    { offsetof(NVMemoryAttachment, glTexAttachMemory), "glTexAttachMemoryNV" },
    { offsetof(NVMemoryAttachment, glTextureAttachMemory), "glTextureAttachMemoryNV" },
};

static const size_t __NVMemoryAttachmentEntryCount = sizeof(__NVMemoryAttachmentEntryPoints) /
    sizeof(__NVMemoryAttachmentEntryPoints[0]);

static const EntryOffset __NVMemoryObjectSparseEntryPoints[] = {
    { offsetof(NVMemoryObjectSparse, glBufferPageCommitmentMem), "glBufferPageCommitmentMemNV" },
    { offsetof(NVMemoryObjectSparse, glNamedBufferPageCommitmentMem), "glNamedBufferPageCommitmentMemNV" },
    { offsetof(NVMemoryObjectSparse, glTexPageCommitmentMem), "glTexPageCommitmentMemNV" },
    { offsetof(NVMemoryObjectSparse, glTexturePageCommitmentMem), "glTexturePageCommitmentMemNV" },
};

static const size_t __NVMemoryObjectSparseEntryCount = sizeof(__NVMemoryObjectSparseEntryPoints) /
    sizeof(__NVMemoryObjectSparseEntryPoints[0]);

static const EntryOffset __NVMeshShaderEntryPoints[] = {
    { offsetof(NVMeshShader, glDrawMeshTasksIndirect), "glDrawMeshTasksIndirectNV" },
    { offsetof(NVMeshShader, glDrawMeshTasks), "glDrawMeshTasksNV" },
    { offsetof(NVMeshShader, glMultiDrawMeshTasksIndirectCount), "glMultiDrawMeshTasksIndirectCountNV" },
    { offsetof(NVMeshShader, glMultiDrawMeshTasksIndirect), "glMultiDrawMeshTasksIndirectNV" },
};

static const size_t __NVMeshShaderEntryCount = sizeof(__NVMeshShaderEntryPoints) /
    sizeof(__NVMeshShaderEntryPoints[0]);

static const EntryOffset __NVOcclusionQueryEntryPoints[] = {
    { offsetof(NVOcclusionQuery, glBeginOcclusionQuery), "glBeginOcclusionQueryNV" },
    { offsetof(NVOcclusionQuery, glDeleteOcclusionQueries), "glDeleteOcclusionQueriesNV" },
    { offsetof(NVOcclusionQuery, glEndOcclusionQuery), "glEndOcclusionQueryNV" },
    { offsetof(NVOcclusionQuery, glGenOcclusionQueries), "glGenOcclusionQueriesNV" },
    { offsetof(NVOcclusionQuery, glGetOcclusionQueryiv), "glGetOcclusionQueryivNV" },
    { offsetof(NVOcclusionQuery, glGetOcclusionQueryuiv), "glGetOcclusionQueryuivNV" },
    { offsetof(NVOcclusionQuery, glIsOcclusionQuery), "glIsOcclusionQueryNV" },
};

static const size_t __NVOcclusionQueryEntryCount = sizeof(__NVOcclusionQueryEntryPoints) /
    sizeof(__NVOcclusionQueryEntryPoints[0]);

static const EntryOffset __NVParameterBufferObjectEntryPoints[] = {
    { offsetof(NVParameterBufferObject, glProgramBufferParametersIiv), "glProgramBufferParametersIivNV" },
    { offsetof(NVParameterBufferObject, glProgramBufferParametersIuiv), "glProgramBufferParametersIuivNV" },
    { offsetof(NVParameterBufferObject, glProgramBufferParametersfv), "glProgramBufferParametersfvNV" },
};

static const size_t __NVParameterBufferObjectEntryCount = sizeof(__NVParameterBufferObjectEntryPoints) /
    sizeof(__NVParameterBufferObjectEntryPoints[0]);

static const EntryOffset __NVPathRenderingEntryPoints[] = {
    { offsetof(NVPathRendering, glCopyPath), "glCopyPathNV" },
    { offsetof(NVPathRendering, glCoverFillPathInstanced), "glCoverFillPathInstancedNV" },
    { offsetof(NVPathRendering, glCoverFillPath), "glCoverFillPathNV" },
    { offsetof(NVPathRendering, glCoverStrokePathInstanced), "glCoverStrokePathInstancedNV" },
    { offsetof(NVPathRendering, glCoverStrokePath), "glCoverStrokePathNV" },
    { offsetof(NVPathRendering, glDeletePaths), "glDeletePathsNV" },
    { offsetof(NVPathRendering, glGenPaths), "glGenPathsNV" },
    { offsetof(NVPathRendering, glGetPathCommands), "glGetPathCommandsNV" },
    { offsetof(NVPathRendering, glGetPathCoords), "glGetPathCoordsNV" },
    { offsetof(NVPathRendering, glGetPathDashArray), "glGetPathDashArrayNV" },
    { offsetof(NVPathRendering, glGetPathLength), "glGetPathLengthNV" },
    { offsetof(NVPathRendering, glGetPathMetricRange), "glGetPathMetricRangeNV" },
    { offsetof(NVPathRendering, glGetPathMetrics), "glGetPathMetricsNV" },
    { offsetof(NVPathRendering, glGetPathParameterfv), "glGetPathParameterfvNV" },
    { offsetof(NVPathRendering, glGetPathParameteriv), "glGetPathParameterivNV" },
    { offsetof(NVPathRendering, glGetPathSpacing), "glGetPathSpacingNV" },
    { offsetof(NVPathRendering, glGetProgramResourcefv), "glGetProgramResourcefvNV" },
    { offsetof(NVPathRendering, glInterpolatePaths), "glInterpolatePathsNV" },
    { offsetof(NVPathRendering, glIsPath), "glIsPathNV" },
    { offsetof(NVPathRendering, glIsPointInFillPath), "glIsPointInFillPathNV" },
    { offsetof(NVPathRendering, glIsPointInStrokePath), "glIsPointInStrokePathNV" },
    { offsetof(NVPathRendering, glMatrixLoad3x2f), "glMatrixLoad3x2fNV" },
    { offsetof(NVPathRendering, glMatrixLoad3x3f), "glMatrixLoad3x3fNV" },
    { offsetof(NVPathRendering, glMatrixLoadTranspose3x3f), "glMatrixLoadTranspose3x3fNV" },
    { offsetof(NVPathRendering, glMatrixMult3x2f), "glMatrixMult3x2fNV" },
    { offsetof(NVPathRendering, glMatrixMult3x3f), "glMatrixMult3x3fNV" },
    { offsetof(NVPathRendering, glMatrixMultTranspose3x3f), "glMatrixMultTranspose3x3fNV" },
    { offsetof(NVPathRendering, glPathCommands), "glPathCommandsNV" },
    { offsetof(NVPathRendering, glPathCoords), "glPathCoordsNV" },
    { offsetof(NVPathRendering, glPathCoverDepthFunc), "glPathCoverDepthFuncNV" },
    { offsetof(NVPathRendering, glPathDashArray), "glPathDashArrayNV" },
    { offsetof(NVPathRendering, glPathGlyphIndexArray), "glPathGlyphIndexArrayNV" },
    { offsetof(NVPathRendering, glPathGlyphIndexRange), "glPathGlyphIndexRangeNV" },
    { offsetof(NVPathRendering, glPathGlyphRange), "glPathGlyphRangeNV" },
    { offsetof(NVPathRendering, glPathGlyphs), "glPathGlyphsNV" },
    { offsetof(NVPathRendering, glPathMemoryGlyphIndexArray), "glPathMemoryGlyphIndexArrayNV" },
    { offsetof(NVPathRendering, glPathParameterf), "glPathParameterfNV" },
    { offsetof(NVPathRendering, glPathParameterfv), "glPathParameterfvNV" },
    { offsetof(NVPathRendering, glPathParameteri), "glPathParameteriNV" },
    { offsetof(NVPathRendering, glPathParameteriv), "glPathParameterivNV" },
    { offsetof(NVPathRendering, glPathStencilDepthOffset), "glPathStencilDepthOffsetNV" },
    { offsetof(NVPathRendering, glPathStencilFunc), "glPathStencilFuncNV" },
    { offsetof(NVPathRendering, glPathString), "glPathStringNV" },
    { offsetof(NVPathRendering, glPathSubCommands), "glPathSubCommandsNV" },
    { offsetof(NVPathRendering, glPathSubCoords), "glPathSubCoordsNV" },
    { offsetof(NVPathRendering, glPointAlongPath), "glPointAlongPathNV" },
    { offsetof(NVPathRendering, glProgramPathFragmentInputGen), "glProgramPathFragmentInputGenNV" },
    { offsetof(NVPathRendering, glStencilFillPathInstanced), "glStencilFillPathInstancedNV" },
    { offsetof(NVPathRendering, glStencilFillPath), "glStencilFillPathNV" },
    { offsetof(NVPathRendering, glStencilStrokePathInstanced), "glStencilStrokePathInstancedNV" },
    { offsetof(NVPathRendering, glStencilStrokePath), "glStencilStrokePathNV" },
    { offsetof(NVPathRendering, glStencilThenCoverFillPathInstanced), "glStencilThenCoverFillPathInstancedNV" },
    { offsetof(NVPathRendering, glStencilThenCoverFillPath), "glStencilThenCoverFillPathNV" },
    { offsetof(NVPathRendering, glStencilThenCoverStrokePathInstanced), "glStencilThenCoverStrokePathInstancedNV" },
    { offsetof(NVPathRendering, glStencilThenCoverStrokePath), "glStencilThenCoverStrokePathNV" },
    { offsetof(NVPathRendering, glTransformPath), "glTransformPathNV" },
    { offsetof(NVPathRendering, glWeightPaths), "glWeightPathsNV" },
};

static const size_t __NVPathRenderingEntryCount = sizeof(__NVPathRenderingEntryPoints) /
    sizeof(__NVPathRenderingEntryPoints[0]);

static const EntryOffset __NVPixelDataRangeEntryPoints[] = {
    { offsetof(NVPixelDataRange, glFlushPixelDataRange), "glFlushPixelDataRangeNV" },
    { offsetof(NVPixelDataRange, glPixelDataRange), "glPixelDataRangeNV" },
};

static const size_t __NVPixelDataRangeEntryCount = sizeof(__NVPixelDataRangeEntryPoints) /
    sizeof(__NVPixelDataRangeEntryPoints[0]);

static const EntryOffset __NVPointSpriteEntryPoints[] = {
    { offsetof(NVPointSprite, glPointParameteri), "glPointParameteriNV" },
    { offsetof(NVPointSprite, glPointParameteriv), "glPointParameterivNV" },
};

static const size_t __NVPointSpriteEntryCount = sizeof(__NVPointSpriteEntryPoints) /
    sizeof(__NVPointSpriteEntryPoints[0]);

static const EntryOffset __NVPresentVideoEntryPoints[] = {
    { offsetof(NVPresentVideo, glGetVideoi64v), "glGetVideoi64vNV" },
    { offsetof(NVPresentVideo, glGetVideoiv), "glGetVideoivNV" },
    { offsetof(NVPresentVideo, glGetVideoui64v), "glGetVideoui64vNV" },
    { offsetof(NVPresentVideo, glGetVideouiv), "glGetVideouivNV" },
    { offsetof(NVPresentVideo, glPresentFrameDualFill), "glPresentFrameDualFillNV" },
    { offsetof(NVPresentVideo, glPresentFrameKeyed), "glPresentFrameKeyedNV" },
};

static const size_t __NVPresentVideoEntryCount = sizeof(__NVPresentVideoEntryPoints) /
    sizeof(__NVPresentVideoEntryPoints[0]);

static const EntryOffset __NVPrimitiveRestartEntryPoints[] = {
    { offsetof(NVPrimitiveRestart, glPrimitiveRestartIndex), "glPrimitiveRestartIndexNV" },
    { offsetof(NVPrimitiveRestart, glPrimitiveRestart), "glPrimitiveRestartNV" },
};

static const size_t __NVPrimitiveRestartEntryCount = sizeof(__NVPrimitiveRestartEntryPoints) /
    sizeof(__NVPrimitiveRestartEntryPoints[0]);

static const EntryOffset __NVQueryResourceEntryPoints[] = {
    { offsetof(NVQueryResource, glQueryResource), "glQueryResourceNV" },
};

static const size_t __NVQueryResourceEntryCount = sizeof(__NVQueryResourceEntryPoints) /
    sizeof(__NVQueryResourceEntryPoints[0]);

static const EntryOffset __NVQueryResourceTagEntryPoints[] = {
    { offsetof(NVQueryResourceTag, glDeleteQueryResourceTag), "glDeleteQueryResourceTagNV" },
    { offsetof(NVQueryResourceTag, glGenQueryResourceTag), "glGenQueryResourceTagNV" },
    { offsetof(NVQueryResourceTag, glQueryResourceTag), "glQueryResourceTagNV" },
};

static const size_t __NVQueryResourceTagEntryCount = sizeof(__NVQueryResourceTagEntryPoints) /
    sizeof(__NVQueryResourceTagEntryPoints[0]);

static const EntryOffset __NVRegisterCombinersEntryPoints[] = {
    { offsetof(NVRegisterCombiners, glCombinerInput), "glCombinerInputNV" },
    { offsetof(NVRegisterCombiners, glCombinerOutput), "glCombinerOutputNV" },
    { offsetof(NVRegisterCombiners, glCombinerParameterf), "glCombinerParameterfNV" },
    { offsetof(NVRegisterCombiners, glCombinerParameterfv), "glCombinerParameterfvNV" },
    { offsetof(NVRegisterCombiners, glCombinerParameteri), "glCombinerParameteriNV" },
    { offsetof(NVRegisterCombiners, glCombinerParameteriv), "glCombinerParameterivNV" },
    { offsetof(NVRegisterCombiners, glFinalCombinerInput), "glFinalCombinerInputNV" },
    { offsetof(NVRegisterCombiners, glGetCombinerInputParameterfv), "glGetCombinerInputParameterfvNV" },
    { offsetof(NVRegisterCombiners, glGetCombinerInputParameteriv), "glGetCombinerInputParameterivNV" },
    { offsetof(NVRegisterCombiners, glGetCombinerOutputParameterfv), "glGetCombinerOutputParameterfvNV" },
    { offsetof(NVRegisterCombiners, glGetCombinerOutputParameteriv), "glGetCombinerOutputParameterivNV" },
    { offsetof(NVRegisterCombiners, glGetFinalCombinerInputParameterfv), "glGetFinalCombinerInputParameterfvNV" },
    { offsetof(NVRegisterCombiners, glGetFinalCombinerInputParameteriv), "glGetFinalCombinerInputParameterivNV" },
};

static const size_t __NVRegisterCombinersEntryCount = sizeof(__NVRegisterCombinersEntryPoints) /
    sizeof(__NVRegisterCombinersEntryPoints[0]);

static const EntryOffset __NVRegisterCOMBINERS2EntryPoints[] = {
    { offsetof(NVRegisterCOMBINERS2, glCombinerStageParameterfv), "glCombinerStageParameterfvNV" },
    { offsetof(NVRegisterCOMBINERS2, glGetCombinerStageParameterfv), "glGetCombinerStageParameterfvNV" },
};

static const size_t __NVRegisterCOMBINERS2EntryCount = sizeof(__NVRegisterCOMBINERS2EntryPoints) /
    sizeof(__NVRegisterCOMBINERS2EntryPoints[0]);

static const EntryOffset __NVSampleLocationsEntryPoints[] = {
    { offsetof(NVSampleLocations, glFramebufferSampleLocationsfv), "glFramebufferSampleLocationsfvNV" },
    { offsetof(NVSampleLocations, glNamedFramebufferSampleLocationsfv), "glNamedFramebufferSampleLocationsfvNV" },
    { offsetof(NVSampleLocations, glResolveDepthValues), "glResolveDepthValuesNV" },
};

static const size_t __NVSampleLocationsEntryCount = sizeof(__NVSampleLocationsEntryPoints) /
    sizeof(__NVSampleLocationsEntryPoints[0]);

static const EntryOffset __NVScissorExclusiveEntryPoints[] = {
    { offsetof(NVScissorExclusive, glScissorExclusiveArrayv), "glScissorExclusiveArrayvNV" },
    { offsetof(NVScissorExclusive, glScissorExclusive), "glScissorExclusiveNV" },
};

static const size_t __NVScissorExclusiveEntryCount = sizeof(__NVScissorExclusiveEntryPoints) /
    sizeof(__NVScissorExclusiveEntryPoints[0]);

static const EntryOffset __NVShaderBufferLoadEntryPoints[] = {
    { offsetof(NVShaderBufferLoad, glGetBufferParameterui64v), "glGetBufferParameterui64vNV" },
    { offsetof(NVShaderBufferLoad, glGetIntegerui64v), "glGetIntegerui64vNV" },
    { offsetof(NVShaderBufferLoad, glGetNamedBufferParameterui64v), "glGetNamedBufferParameterui64vNV" },
    { offsetof(NVShaderBufferLoad, glIsBufferResident), "glIsBufferResidentNV" },
    { offsetof(NVShaderBufferLoad, glIsNamedBufferResident), "glIsNamedBufferResidentNV" },
    { offsetof(NVShaderBufferLoad, glMakeBufferNonResident), "glMakeBufferNonResidentNV" },
    { offsetof(NVShaderBufferLoad, glMakeBufferResident), "glMakeBufferResidentNV" },
    { offsetof(NVShaderBufferLoad, glMakeNamedBufferNonResident), "glMakeNamedBufferNonResidentNV" },
    { offsetof(NVShaderBufferLoad, glMakeNamedBufferResident), "glMakeNamedBufferResidentNV" },
    { offsetof(NVShaderBufferLoad, glProgramUniformui64), "glProgramUniformui64NV" },
    { offsetof(NVShaderBufferLoad, glProgramUniformui64v), "glProgramUniformui64vNV" },
    { offsetof(NVShaderBufferLoad, glUniformui64), "glUniformui64NV" },
    { offsetof(NVShaderBufferLoad, glUniformui64v), "glUniformui64vNV" },
};

static const size_t __NVShaderBufferLoadEntryCount = sizeof(__NVShaderBufferLoadEntryPoints) /
    sizeof(__NVShaderBufferLoadEntryPoints[0]);

static const EntryOffset __NVShadingRateImageEntryPoints[] = {
    { offsetof(NVShadingRateImage, glBindShadingRateImage), "glBindShadingRateImageNV" },
    { offsetof(NVShadingRateImage, glGetShadingRateImagePalette), "glGetShadingRateImagePaletteNV" },
    { offsetof(NVShadingRateImage, glGetShadingRateSampleLocationiv), "glGetShadingRateSampleLocationivNV" },
    { offsetof(NVShadingRateImage, glShadingRateImageBarrier), "glShadingRateImageBarrierNV" },
    { offsetof(NVShadingRateImage, glShadingRateImagePalette), "glShadingRateImagePaletteNV" },
    { offsetof(NVShadingRateImage, glShadingRateSampleOrderCustom), "glShadingRateSampleOrderCustomNV" },
    { offsetof(NVShadingRateImage, glShadingRateSampleOrder), "glShadingRateSampleOrderNV" },
};

static const size_t __NVShadingRateImageEntryCount = sizeof(__NVShadingRateImageEntryPoints) /
    sizeof(__NVShadingRateImageEntryPoints[0]);

static const EntryOffset __NVTextureBarrierEntryPoints[] = {
    { offsetof(NVTextureBarrier, glTextureBarrier), "glTextureBarrierNV" },
};

static const size_t __NVTextureBarrierEntryCount = sizeof(__NVTextureBarrierEntryPoints) /
    sizeof(__NVTextureBarrierEntryPoints[0]);

static const EntryOffset __NVTextureMultisampleEntryPoints[] = {
    { offsetof(NVTextureMultisample, glTexImage2DMultisampleCoverage), "glTexImage2DMultisampleCoverageNV" },
    { offsetof(NVTextureMultisample, glTexImage3DMultisampleCoverage), "glTexImage3DMultisampleCoverageNV" },
    { offsetof(NVTextureMultisample, glTextureImage2DMultisampleCoverage), "glTextureImage2DMultisampleCoverageNV" },
    { offsetof(NVTextureMultisample, glTextureImage2DMultisample), "glTextureImage2DMultisampleNV" },
    { offsetof(NVTextureMultisample, glTextureImage3DMultisampleCoverage), "glTextureImage3DMultisampleCoverageNV" },
    { offsetof(NVTextureMultisample, glTextureImage3DMultisample), "glTextureImage3DMultisampleNV" },
};

static const size_t __NVTextureMultisampleEntryCount = sizeof(__NVTextureMultisampleEntryPoints) /
    sizeof(__NVTextureMultisampleEntryPoints[0]);

static const EntryOffset __NVTimelineSemaphoreEntryPoints[] = {
    { offsetof(NVTimelineSemaphore, glCreateSemaphores), "glCreateSemaphoresNV" },
    { offsetof(NVTimelineSemaphore, glGetSemaphoreParameteriv), "glGetSemaphoreParameterivNV" },
    { offsetof(NVTimelineSemaphore, glSemaphoreParameteriv), "glSemaphoreParameterivNV" },
};

static const size_t __NVTimelineSemaphoreEntryCount = sizeof(__NVTimelineSemaphoreEntryPoints) /
    sizeof(__NVTimelineSemaphoreEntryPoints[0]);

static const EntryOffset __NVTransformFeedbackEntryPoints[] = {
    { offsetof(NVTransformFeedback, glActiveVarying), "glActiveVaryingNV" },
    { offsetof(NVTransformFeedback, glBeginTransformFeedback), "glBeginTransformFeedbackNV" },
    { offsetof(NVTransformFeedback, glBindBufferBase), "glBindBufferBaseNV" },
    { offsetof(NVTransformFeedback, glBindBufferOffset), "glBindBufferOffsetNV" },
    { offsetof(NVTransformFeedback, glBindBufferRange), "glBindBufferRangeNV" },
    { offsetof(NVTransformFeedback, glEndTransformFeedback), "glEndTransformFeedbackNV" },
    { offsetof(NVTransformFeedback, glGetActiveVarying), "glGetActiveVaryingNV" },
    { offsetof(NVTransformFeedback, glGetTransformFeedbackVarying), "glGetTransformFeedbackVaryingNV" },
    { offsetof(NVTransformFeedback, glGetVaryingLocation), "glGetVaryingLocationNV" },
    { offsetof(NVTransformFeedback, glTransformFeedbackAttribs), "glTransformFeedbackAttribsNV" },
    { offsetof(NVTransformFeedback, glTransformFeedbackStreamAttribs), "glTransformFeedbackStreamAttribsNV" },
    { offsetof(NVTransformFeedback, glTransformFeedbackVaryings), "glTransformFeedbackVaryingsNV" },
};

static const size_t __NVTransformFeedbackEntryCount = sizeof(__NVTransformFeedbackEntryPoints) /
    sizeof(__NVTransformFeedbackEntryPoints[0]);

static const EntryOffset __NVTransformFeedback2EntryPoints[] = {
    { offsetof(NVTransformFeedback2, glBindTransformFeedback), "glBindTransformFeedbackNV" },
    { offsetof(NVTransformFeedback2, glDeleteTransformFeedbacks), "glDeleteTransformFeedbacksNV" },
    { offsetof(NVTransformFeedback2, glDrawTransformFeedback), "glDrawTransformFeedbackNV" },
    { offsetof(NVTransformFeedback2, glGenTransformFeedbacks), "glGenTransformFeedbacksNV" },
    { offsetof(NVTransformFeedback2, glIsTransformFeedback), "glIsTransformFeedbackNV" },
    { offsetof(NVTransformFeedback2, glPauseTransformFeedback), "glPauseTransformFeedbackNV" },
    { offsetof(NVTransformFeedback2, glResumeTransformFeedback), "glResumeTransformFeedbackNV" },
};

static const size_t __NVTransformFeedback2EntryCount = sizeof(__NVTransformFeedback2EntryPoints) /
    sizeof(__NVTransformFeedback2EntryPoints[0]);

static const EntryOffset __NVVdpauInteropEntryPoints[] = {
    { offsetof(NVVdpauInterop, glVDPAUFini), "glVDPAUFiniNV" },
    { offsetof(NVVdpauInterop, glVDPAUGetSurfaceiv), "glVDPAUGetSurfaceivNV" },
    { offsetof(NVVdpauInterop, glVDPAUInit), "glVDPAUInitNV" },
    { offsetof(NVVdpauInterop, glVDPAUIsSurface), "glVDPAUIsSurfaceNV" },
    { offsetof(NVVdpauInterop, glVDPAUMapSurfaces), "glVDPAUMapSurfacesNV" },
    { offsetof(NVVdpauInterop, glVDPAURegisterOutputSurface), "glVDPAURegisterOutputSurfaceNV" },
    { offsetof(NVVdpauInterop, glVDPAURegisterVideoSurface), "glVDPAURegisterVideoSurfaceNV" },
    { offsetof(NVVdpauInterop, glVDPAUSurfaceAccess), "glVDPAUSurfaceAccessNV" },
    { offsetof(NVVdpauInterop, glVDPAUUnmapSurfaces), "glVDPAUUnmapSurfacesNV" },
    { offsetof(NVVdpauInterop, glVDPAUUnregisterSurface), "glVDPAUUnregisterSurfaceNV" },
};

static const size_t __NVVdpauInteropEntryCount = sizeof(__NVVdpauInteropEntryPoints) /
    sizeof(__NVVdpauInteropEntryPoints[0]);

static const EntryOffset __NVVdpauINTEROP2EntryPoints[] = {
    { offsetof(NVVdpauINTEROP2, glVDPAURegisterVideoSurfaceWithPictureStructure), "glVDPAURegisterVideoSurfaceWithPictureStructureNV" },
};

static const size_t __NVVdpauINTEROP2EntryCount = sizeof(__NVVdpauINTEROP2EntryPoints) /
    sizeof(__NVVdpauINTEROP2EntryPoints[0]);

static const EntryOffset __NVVertexArrayRangeEntryPoints[] = {
    { offsetof(NVVertexArrayRange, glFlushVertexArrayRange), "glFlushVertexArrayRangeNV" },
    { offsetof(NVVertexArrayRange, glVertexArrayRange), "glVertexArrayRangeNV" },
};

static const size_t __NVVertexArrayRangeEntryCount = sizeof(__NVVertexArrayRangeEntryPoints) /
    sizeof(__NVVertexArrayRangeEntryPoints[0]);

static const EntryOffset __NVVertexAttribInteger64BITEntryPoints[] = {
    { offsetof(NVVertexAttribInteger64BIT, glGetVertexAttribLi64v), "glGetVertexAttribLi64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glGetVertexAttribLui64v), "glGetVertexAttribLui64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL1i64), "glVertexAttribL1i64NV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL1i64v), "glVertexAttribL1i64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL1ui64), "glVertexAttribL1ui64NV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL1ui64v), "glVertexAttribL1ui64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL2i64), "glVertexAttribL2i64NV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL2i64v), "glVertexAttribL2i64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL2ui64), "glVertexAttribL2ui64NV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL2ui64v), "glVertexAttribL2ui64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL3i64), "glVertexAttribL3i64NV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL3i64v), "glVertexAttribL3i64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL3ui64), "glVertexAttribL3ui64NV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL3ui64v), "glVertexAttribL3ui64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL4i64), "glVertexAttribL4i64NV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL4i64v), "glVertexAttribL4i64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL4ui64), "glVertexAttribL4ui64NV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribL4ui64v), "glVertexAttribL4ui64vNV" },
    { offsetof(NVVertexAttribInteger64BIT, glVertexAttribLFormat), "glVertexAttribLFormatNV" },
};

static const size_t __NVVertexAttribInteger64BITEntryCount = sizeof(__NVVertexAttribInteger64BITEntryPoints) /
    sizeof(__NVVertexAttribInteger64BITEntryPoints[0]);

static const EntryOffset __NVVertexBufferUnifiedMemoryEntryPoints[] = {
    { offsetof(NVVertexBufferUnifiedMemory, glBufferAddressRange), "glBufferAddressRangeNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glColorFormat), "glColorFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glEdgeFlagFormat), "glEdgeFlagFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glFogCoordFormat), "glFogCoordFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glGetIntegerui64i_v), "glGetIntegerui64i_vNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glIndexFormat), "glIndexFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glNormalFormat), "glNormalFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glSecondaryColorFormat), "glSecondaryColorFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glTexCoordFormat), "glTexCoordFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glVertexAttribFormat), "glVertexAttribFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glVertexAttribIFormat), "glVertexAttribIFormatNV" },
    { offsetof(NVVertexBufferUnifiedMemory, glVertexFormat), "glVertexFormatNV" },
};

static const size_t __NVVertexBufferUnifiedMemoryEntryCount = sizeof(__NVVertexBufferUnifiedMemoryEntryPoints) /
    sizeof(__NVVertexBufferUnifiedMemoryEntryPoints[0]);

static const EntryOffset __NVVertexProgramEntryPoints[] = {
    { offsetof(NVVertexProgram, glAreProgramsResident), "glAreProgramsResidentNV" },
    { offsetof(NVVertexProgram, glBindProgram), "glBindProgramNV" },
    { offsetof(NVVertexProgram, glDeletePrograms), "glDeleteProgramsNV" },
    { offsetof(NVVertexProgram, glExecuteProgram), "glExecuteProgramNV" },
    { offsetof(NVVertexProgram, glGenPrograms), "glGenProgramsNV" },
    { offsetof(NVVertexProgram, glGetProgramParameterdv), "glGetProgramParameterdvNV" },
    { offsetof(NVVertexProgram, glGetProgramParameterfv), "glGetProgramParameterfvNV" },
    { offsetof(NVVertexProgram, glGetProgramString), "glGetProgramStringNV" },
    { offsetof(NVVertexProgram, glGetProgramiv), "glGetProgramivNV" },
    { offsetof(NVVertexProgram, glGetTrackMatrixiv), "glGetTrackMatrixivNV" },
    { offsetof(NVVertexProgram, glGetVertexAttribPointerv), "glGetVertexAttribPointervNV" },
    { offsetof(NVVertexProgram, glGetVertexAttribdv), "glGetVertexAttribdvNV" },
    { offsetof(NVVertexProgram, glGetVertexAttribfv), "glGetVertexAttribfvNV" },
    { offsetof(NVVertexProgram, glGetVertexAttribiv), "glGetVertexAttribivNV" },
    { offsetof(NVVertexProgram, glIsProgram), "glIsProgramNV" },
    { offsetof(NVVertexProgram, glLoadProgram), "glLoadProgramNV" },
    { offsetof(NVVertexProgram, glProgramParameter4d), "glProgramParameter4dNV" },
    { offsetof(NVVertexProgram, glProgramParameter4dv), "glProgramParameter4dvNV" },
    { offsetof(NVVertexProgram, glProgramParameter4f), "glProgramParameter4fNV" },
    { offsetof(NVVertexProgram, glProgramParameter4fv), "glProgramParameter4fvNV" },
    { offsetof(NVVertexProgram, glProgramParameters4dv), "glProgramParameters4dvNV" },
    { offsetof(NVVertexProgram, glProgramParameters4fv), "glProgramParameters4fvNV" },
    { offsetof(NVVertexProgram, glRequestResidentPrograms), "glRequestResidentProgramsNV" },
    { offsetof(NVVertexProgram, glTrackMatrix), "glTrackMatrixNV" },
    { offsetof(NVVertexProgram, glVertexAttrib1d), "glVertexAttrib1dNV" },
    { offsetof(NVVertexProgram, glVertexAttrib1dv), "glVertexAttrib1dvNV" },
    { offsetof(NVVertexProgram, glVertexAttrib1f), "glVertexAttrib1fNV" },
    { offsetof(NVVertexProgram, glVertexAttrib1fv), "glVertexAttrib1fvNV" },
    { offsetof(NVVertexProgram, glVertexAttrib1s), "glVertexAttrib1sNV" },
    { offsetof(NVVertexProgram, glVertexAttrib1sv), "glVertexAttrib1svNV" },
    { offsetof(NVVertexProgram, glVertexAttrib2d), "glVertexAttrib2dNV" },
    { offsetof(NVVertexProgram, glVertexAttrib2dv), "glVertexAttrib2dvNV" },
    { offsetof(NVVertexProgram, glVertexAttrib2f), "glVertexAttrib2fNV" },
    { offsetof(NVVertexProgram, glVertexAttrib2fv), "glVertexAttrib2fvNV" },
    { offsetof(NVVertexProgram, glVertexAttrib2s), "glVertexAttrib2sNV" },
    { offsetof(NVVertexProgram, glVertexAttrib2sv), "glVertexAttrib2svNV" },
    { offsetof(NVVertexProgram, glVertexAttrib3d), "glVertexAttrib3dNV" },
    { offsetof(NVVertexProgram, glVertexAttrib3dv), "glVertexAttrib3dvNV" },
    { offsetof(NVVertexProgram, glVertexAttrib3f), "glVertexAttrib3fNV" },
    { offsetof(NVVertexProgram, glVertexAttrib3fv), "glVertexAttrib3fvNV" },
    { offsetof(NVVertexProgram, glVertexAttrib3s), "glVertexAttrib3sNV" },
    { offsetof(NVVertexProgram, glVertexAttrib3sv), "glVertexAttrib3svNV" },
    { offsetof(NVVertexProgram, glVertexAttrib4d), "glVertexAttrib4dNV" },
    { offsetof(NVVertexProgram, glVertexAttrib4dv), "glVertexAttrib4dvNV" },
    { offsetof(NVVertexProgram, glVertexAttrib4f), "glVertexAttrib4fNV" },
    { offsetof(NVVertexProgram, glVertexAttrib4fv), "glVertexAttrib4fvNV" },
    { offsetof(NVVertexProgram, glVertexAttrib4s), "glVertexAttrib4sNV" },
    { offsetof(NVVertexProgram, glVertexAttrib4sv), "glVertexAttrib4svNV" },
    { offsetof(NVVertexProgram, glVertexAttrib4ub), "glVertexAttrib4ubNV" },
    { offsetof(NVVertexProgram, glVertexAttrib4ubv), "glVertexAttrib4ubvNV" },
    { offsetof(NVVertexProgram, glVertexAttribPointer), "glVertexAttribPointerNV" },
    { offsetof(NVVertexProgram, glVertexAttribs1dv), "glVertexAttribs1dvNV" },
    { offsetof(NVVertexProgram, glVertexAttribs1fv), "glVertexAttribs1fvNV" },
    { offsetof(NVVertexProgram, glVertexAttribs1sv), "glVertexAttribs1svNV" },
    { offsetof(NVVertexProgram, glVertexAttribs2dv), "glVertexAttribs2dvNV" },
    { offsetof(NVVertexProgram, glVertexAttribs2fv), "glVertexAttribs2fvNV" },
    { offsetof(NVVertexProgram, glVertexAttribs2sv), "glVertexAttribs2svNV" },
    { offsetof(NVVertexProgram, glVertexAttribs3dv), "glVertexAttribs3dvNV" },
    { offsetof(NVVertexProgram, glVertexAttribs3fv), "glVertexAttribs3fvNV" },
    { offsetof(NVVertexProgram, glVertexAttribs3sv), "glVertexAttribs3svNV" },
    { offsetof(NVVertexProgram, glVertexAttribs4dv), "glVertexAttribs4dvNV" },
    { offsetof(NVVertexProgram, glVertexAttribs4fv), "glVertexAttribs4fvNV" },
    { offsetof(NVVertexProgram, glVertexAttribs4sv), "glVertexAttribs4svNV" },
    { offsetof(NVVertexProgram, glVertexAttribs4ubv), "glVertexAttribs4ubvNV" },
};

static const size_t __NVVertexProgramEntryCount = sizeof(__NVVertexProgramEntryPoints) /
    sizeof(__NVVertexProgramEntryPoints[0]);

static const EntryOffset __NVVideoCaptureEntryPoints[] = {
    { offsetof(NVVideoCapture, glBeginVideoCapture), "glBeginVideoCaptureNV" },
    { offsetof(NVVideoCapture, glBindVideoCaptureStreamBuffer), "glBindVideoCaptureStreamBufferNV" },
    { offsetof(NVVideoCapture, glBindVideoCaptureStreamTexture), "glBindVideoCaptureStreamTextureNV" },
    { offsetof(NVVideoCapture, glEndVideoCapture), "glEndVideoCaptureNV" },
    { offsetof(NVVideoCapture, glGetVideoCaptureStreamdv), "glGetVideoCaptureStreamdvNV" },
    { offsetof(NVVideoCapture, glGetVideoCaptureStreamfv), "glGetVideoCaptureStreamfvNV" },
    { offsetof(NVVideoCapture, glGetVideoCaptureStreamiv), "glGetVideoCaptureStreamivNV" },
    { offsetof(NVVideoCapture, glGetVideoCaptureiv), "glGetVideoCaptureivNV" },
    { offsetof(NVVideoCapture, glVideoCapture), "glVideoCaptureNV" },
    { offsetof(NVVideoCapture, glVideoCaptureStreamParameterdv), "glVideoCaptureStreamParameterdvNV" },
    { offsetof(NVVideoCapture, glVideoCaptureStreamParameterfv), "glVideoCaptureStreamParameterfvNV" },
    { offsetof(NVVideoCapture, glVideoCaptureStreamParameteriv), "glVideoCaptureStreamParameterivNV" },
};

static const size_t __NVVideoCaptureEntryCount = sizeof(__NVVideoCaptureEntryPoints) /
    sizeof(__NVVideoCaptureEntryPoints[0]);

static const EntryOffset __NVViewportSwizzleEntryPoints[] = {
    { offsetof(NVViewportSwizzle, glViewportSwizzle), "glViewportSwizzleNV" },
};

static const size_t __NVViewportSwizzleEntryCount = sizeof(__NVViewportSwizzleEntryPoints) /
    sizeof(__NVViewportSwizzleEntryPoints[0]);

static const EntryOffset __OESByteCoordinatesEntryPoints[] = {
    { offsetof(OESByteCoordinates, glMultiTexCoord1b), "glMultiTexCoord1bOES" },
    { offsetof(OESByteCoordinates, glMultiTexCoord1bv), "glMultiTexCoord1bvOES" },
    { offsetof(OESByteCoordinates, glMultiTexCoord2b), "glMultiTexCoord2bOES" },
    { offsetof(OESByteCoordinates, glMultiTexCoord2bv), "glMultiTexCoord2bvOES" },
    { offsetof(OESByteCoordinates, glMultiTexCoord3b), "glMultiTexCoord3bOES" },
    { offsetof(OESByteCoordinates, glMultiTexCoord3bv), "glMultiTexCoord3bvOES" },
    { offsetof(OESByteCoordinates, glMultiTexCoord4b), "glMultiTexCoord4bOES" },
    { offsetof(OESByteCoordinates, glMultiTexCoord4bv), "glMultiTexCoord4bvOES" },
    { offsetof(OESByteCoordinates, glTexCoord1b), "glTexCoord1bOES" },
    { offsetof(OESByteCoordinates, glTexCoord1bv), "glTexCoord1bvOES" },
    { offsetof(OESByteCoordinates, glTexCoord2b), "glTexCoord2bOES" },
    { offsetof(OESByteCoordinates, glTexCoord2bv), "glTexCoord2bvOES" },
    { offsetof(OESByteCoordinates, glTexCoord3b), "glTexCoord3bOES" },
    { offsetof(OESByteCoordinates, glTexCoord3bv), "glTexCoord3bvOES" },
    { offsetof(OESByteCoordinates, glTexCoord4b), "glTexCoord4bOES" },
    { offsetof(OESByteCoordinates, glTexCoord4bv), "glTexCoord4bvOES" },
    { offsetof(OESByteCoordinates, glVertex2b), "glVertex2bOES" },
    { offsetof(OESByteCoordinates, glVertex2bv), "glVertex2bvOES" },
    { offsetof(OESByteCoordinates, glVertex3b), "glVertex3bOES" },
    { offsetof(OESByteCoordinates, glVertex3bv), "glVertex3bvOES" },
    { offsetof(OESByteCoordinates, glVertex4b), "glVertex4bOES" },
    { offsetof(OESByteCoordinates, glVertex4bv), "glVertex4bvOES" },
};

static const size_t __OESByteCoordinatesEntryCount = sizeof(__OESByteCoordinatesEntryPoints) /
    sizeof(__OESByteCoordinatesEntryPoints[0]);

static const EntryOffset __OESFixedPointEntryPoints[] = {
    { offsetof(OESFixedPoint, glAccumx), "glAccumxOES" },
    { offsetof(OESFixedPoint, glAlphaFuncx), "glAlphaFuncxOES" },
    { offsetof(OESFixedPoint, glBitmapx), "glBitmapxOES" },
    { offsetof(OESFixedPoint, glBlendColorx), "glBlendColorxOES" },
    { offsetof(OESFixedPoint, glClearAccumx), "glClearAccumxOES" },
    { offsetof(OESFixedPoint, glClearColorx), "glClearColorxOES" },
    { offsetof(OESFixedPoint, glClearDepthx), "glClearDepthxOES" },
    { offsetof(OESFixedPoint, glClipPlanex), "glClipPlanexOES" },
    { offsetof(OESFixedPoint, glColor3x), "glColor3xOES" },
    { offsetof(OESFixedPoint, glColor3xv), "glColor3xvOES" },
    { offsetof(OESFixedPoint, glColor4x), "glColor4xOES" },
    { offsetof(OESFixedPoint, glColor4xv), "glColor4xvOES" },
    { offsetof(OESFixedPoint, glConvolutionParameterx), "glConvolutionParameterxOES" },
    { offsetof(OESFixedPoint, glConvolutionParameterxv), "glConvolutionParameterxvOES" },
    { offsetof(OESFixedPoint, glDepthRangex), "glDepthRangexOES" },
    { offsetof(OESFixedPoint, glEvalCoord1x), "glEvalCoord1xOES" },
    { offsetof(OESFixedPoint, glEvalCoord1xv), "glEvalCoord1xvOES" },
    { offsetof(OESFixedPoint, glEvalCoord2x), "glEvalCoord2xOES" },
    { offsetof(OESFixedPoint, glEvalCoord2xv), "glEvalCoord2xvOES" },
    { offsetof(OESFixedPoint, glFeedbackBufferx), "glFeedbackBufferxOES" },
    { offsetof(OESFixedPoint, glFogx), "glFogxOES" },
    { offsetof(OESFixedPoint, glFogxv), "glFogxvOES" },
    { offsetof(OESFixedPoint, glFrustumx), "glFrustumxOES" },
    { offsetof(OESFixedPoint, glGetClipPlanex), "glGetClipPlanexOES" },
    { offsetof(OESFixedPoint, glGetConvolutionParameterxv), "glGetConvolutionParameterxvOES" },
    { offsetof(OESFixedPoint, glGetFixedv), "glGetFixedvOES" },
    { offsetof(OESFixedPoint, glGetHistogramParameterxv), "glGetHistogramParameterxvOES" },
    { offsetof(OESFixedPoint, glGetLightx), "glGetLightxOES" },
    { offsetof(OESFixedPoint, glGetMapxv), "glGetMapxvOES" },
    { offsetof(OESFixedPoint, glGetMaterialx), "glGetMaterialxOES" },
    { offsetof(OESFixedPoint, glGetPixelMapxv), "glGetPixelMapxv" },
    { offsetof(OESFixedPoint, glGetTexEnvxv), "glGetTexEnvxvOES" },
    { offsetof(OESFixedPoint, glGetTexGenxv), "glGetTexGenxvOES" },
    { offsetof(OESFixedPoint, glGetTexLevelParameterxv), "glGetTexLevelParameterxvOES" },
    { offsetof(OESFixedPoint, glGetTexParameterxv), "glGetTexParameterxvOES" },
    { offsetof(OESFixedPoint, glIndexx), "glIndexxOES" },
    { offsetof(OESFixedPoint, glIndexxv), "glIndexxvOES" },
    { offsetof(OESFixedPoint, glLightModelx), "glLightModelxOES" },
    { offsetof(OESFixedPoint, glLightModelxv), "glLightModelxvOES" },
    { offsetof(OESFixedPoint, glLightx), "glLightxOES" },
    { offsetof(OESFixedPoint, glLightxv), "glLightxvOES" },
    { offsetof(OESFixedPoint, glLineWidthx), "glLineWidthxOES" },
    { offsetof(OESFixedPoint, glLoadMatrixx), "glLoadMatrixxOES" },
    { offsetof(OESFixedPoint, glLoadTransposeMatrixx), "glLoadTransposeMatrixxOES" },
    { offsetof(OESFixedPoint, glMap1x), "glMap1xOES" },
    { offsetof(OESFixedPoint, glMap2x), "glMap2xOES" },
    { offsetof(OESFixedPoint, glMapGrid1x), "glMapGrid1xOES" },
    { offsetof(OESFixedPoint, glMapGrid2x), "glMapGrid2xOES" },
    { offsetof(OESFixedPoint, glMaterialx), "glMaterialxOES" },
    { offsetof(OESFixedPoint, glMaterialxv), "glMaterialxvOES" },
    { offsetof(OESFixedPoint, glMultMatrixx), "glMultMatrixxOES" },
    { offsetof(OESFixedPoint, glMultTransposeMatrixx), "glMultTransposeMatrixxOES" },
    { offsetof(OESFixedPoint, glMultiTexCoord1x), "glMultiTexCoord1xOES" },
    { offsetof(OESFixedPoint, glMultiTexCoord1xv), "glMultiTexCoord1xvOES" },
    { offsetof(OESFixedPoint, glMultiTexCoord2x), "glMultiTexCoord2xOES" },
    { offsetof(OESFixedPoint, glMultiTexCoord2xv), "glMultiTexCoord2xvOES" },
    { offsetof(OESFixedPoint, glMultiTexCoord3x), "glMultiTexCoord3xOES" },
    { offsetof(OESFixedPoint, glMultiTexCoord3xv), "glMultiTexCoord3xvOES" },
    { offsetof(OESFixedPoint, glMultiTexCoord4x), "glMultiTexCoord4xOES" },
    { offsetof(OESFixedPoint, glMultiTexCoord4xv), "glMultiTexCoord4xvOES" },
    { offsetof(OESFixedPoint, glNormal3x), "glNormal3xOES" },
    { offsetof(OESFixedPoint, glNormal3xv), "glNormal3xvOES" },
    { offsetof(OESFixedPoint, glOrthox), "glOrthoxOES" },
    { offsetof(OESFixedPoint, glPassThroughx), "glPassThroughxOES" },
    { offsetof(OESFixedPoint, glPixelMapx), "glPixelMapx" },
    { offsetof(OESFixedPoint, glPixelStorex), "glPixelStorex" },
    { offsetof(OESFixedPoint, glPixelTransferx), "glPixelTransferxOES" },
    { offsetof(OESFixedPoint, glPixelZoomx), "glPixelZoomxOES" },
    { offsetof(OESFixedPoint, glPointParameterxv), "glPointParameterxvOES" },
    { offsetof(OESFixedPoint, glPointSizex), "glPointSizexOES" },
    { offsetof(OESFixedPoint, glPolygonOffsetx), "glPolygonOffsetxOES" },
    { offsetof(OESFixedPoint, glPrioritizeTexturesx), "glPrioritizeTexturesxOES" },
    { offsetof(OESFixedPoint, glRasterPos2x), "glRasterPos2xOES" },
    { offsetof(OESFixedPoint, glRasterPos2xv), "glRasterPos2xvOES" },
    { offsetof(OESFixedPoint, glRasterPos3x), "glRasterPos3xOES" },
    { offsetof(OESFixedPoint, glRasterPos3xv), "glRasterPos3xvOES" },
    { offsetof(OESFixedPoint, glRasterPos4x), "glRasterPos4xOES" },
    { offsetof(OESFixedPoint, glRasterPos4xv), "glRasterPos4xvOES" },
    { offsetof(OESFixedPoint, glRectx), "glRectxOES" },
    { offsetof(OESFixedPoint, glRectxv), "glRectxvOES" },
    { offsetof(OESFixedPoint, glRotatex), "glRotatexOES" },
    { offsetof(OESFixedPoint, glScalex), "glScalexOES" },
    { offsetof(OESFixedPoint, glTexCoord1x), "glTexCoord1xOES" },
    { offsetof(OESFixedPoint, glTexCoord1xv), "glTexCoord1xvOES" },
    { offsetof(OESFixedPoint, glTexCoord2x), "glTexCoord2xOES" },
    { offsetof(OESFixedPoint, glTexCoord2xv), "glTexCoord2xvOES" },
    { offsetof(OESFixedPoint, glTexCoord3x), "glTexCoord3xOES" },
    { offsetof(OESFixedPoint, glTexCoord3xv), "glTexCoord3xvOES" },
    { offsetof(OESFixedPoint, glTexCoord4x), "glTexCoord4xOES" },
    { offsetof(OESFixedPoint, glTexCoord4xv), "glTexCoord4xvOES" },
    { offsetof(OESFixedPoint, glTexEnvx), "glTexEnvxOES" },
    { offsetof(OESFixedPoint, glTexEnvxv), "glTexEnvxvOES" },
    { offsetof(OESFixedPoint, glTexGenx), "glTexGenxOES" },
    { offsetof(OESFixedPoint, glTexGenxv), "glTexGenxvOES" },
    { offsetof(OESFixedPoint, glTexParameterx), "glTexParameterxOES" },
    { offsetof(OESFixedPoint, glTexParameterxv), "glTexParameterxvOES" },
    { offsetof(OESFixedPoint, glTranslatex), "glTranslatexOES" },
    { offsetof(OESFixedPoint, glVertex2x), "glVertex2xOES" },
    { offsetof(OESFixedPoint, glVertex2xv), "glVertex2xvOES" },
    { offsetof(OESFixedPoint, glVertex3x), "glVertex3xOES" },
    { offsetof(OESFixedPoint, glVertex3xv), "glVertex3xvOES" },
    { offsetof(OESFixedPoint, glVertex4x), "glVertex4xOES" },
    { offsetof(OESFixedPoint, glVertex4xv), "glVertex4xvOES" },
};

static const size_t __OESFixedPointEntryCount = sizeof(__OESFixedPointEntryPoints) /
    sizeof(__OESFixedPointEntryPoints[0]);

static const EntryOffset __OESQueryMatrixEntryPoints[] = {
    { offsetof(OESQueryMatrix, glQueryMatrixx), "glQueryMatrixxOES" },
};

static const size_t __OESQueryMatrixEntryCount = sizeof(__OESQueryMatrixEntryPoints) /
    sizeof(__OESQueryMatrixEntryPoints[0]);

static const EntryOffset __OESSinglePrecisionEntryPoints[] = {
    { offsetof(OESSinglePrecision, glClearDepthf), "glClearDepthfOES" },
    { offsetof(OESSinglePrecision, glClipPlanef), "glClipPlanefOES" },
    { offsetof(OESSinglePrecision, glDepthRangef), "glDepthRangefOES" },
    { offsetof(OESSinglePrecision, glFrustumf), "glFrustumfOES" },
    { offsetof(OESSinglePrecision, glGetClipPlanef), "glGetClipPlanefOES" },
    { offsetof(OESSinglePrecision, glOrthof), "glOrthofOES" },
};

static const size_t __OESSinglePrecisionEntryCount = sizeof(__OESSinglePrecisionEntryPoints) /
    sizeof(__OESSinglePrecisionEntryPoints[0]);

static const EntryOffset __OvrMultiviewEntryPoints[] = {
    { offsetof(OvrMultiview, glFramebufferTextureMultiviewOVR), "glFramebufferTextureMultiviewOVR" },
};

static const size_t __OvrMultiviewEntryCount = sizeof(__OvrMultiviewEntryPoints) /
    sizeof(__OvrMultiviewEntryPoints[0]);

static const EntryOffset __PGIMiscHintsEntryPoints[] = {
    { offsetof(PGIMiscHints, glHint), "glHintPGI" },
};

static const size_t __PGIMiscHintsEntryCount = sizeof(__PGIMiscHintsEntryPoints) /
    sizeof(__PGIMiscHintsEntryPoints[0]);

static const EntryOffset __SGISDetailTextureEntryPoints[] = {
    { offsetof(SGISDetailTexture, glDetailTexFunc), "glDetailTexFuncSGIS" },
    { offsetof(SGISDetailTexture, glGetDetailTexFunc), "glGetDetailTexFuncSGIS" },
};

static const size_t __SGISDetailTextureEntryCount = sizeof(__SGISDetailTextureEntryPoints) /
    sizeof(__SGISDetailTextureEntryPoints[0]);

static const EntryOffset __SGISFogFunctionEntryPoints[] = {
    { offsetof(SGISFogFunction, glFogFunc), "glFogFuncSGIS" },
    { offsetof(SGISFogFunction, glGetFogFunc), "glGetFogFuncSGIS" },
};

static const size_t __SGISFogFunctionEntryCount = sizeof(__SGISFogFunctionEntryPoints) /
    sizeof(__SGISFogFunctionEntryPoints[0]);

static const EntryOffset __SGISMultisampleEntryPoints[] = {
    { offsetof(SGISMultisample, glSampleMask), "glSampleMaskSGIS" },
    { offsetof(SGISMultisample, glSamplePattern), "glSamplePatternSGIS" },
};

static const size_t __SGISMultisampleEntryCount = sizeof(__SGISMultisampleEntryPoints) /
    sizeof(__SGISMultisampleEntryPoints[0]);

static const EntryOffset __SGISPixelTextureEntryPoints[] = {
    { offsetof(SGISPixelTexture, glGetPixelTexGenParameterfv), "glGetPixelTexGenParameterfvSGIS" },
    { offsetof(SGISPixelTexture, glGetPixelTexGenParameteriv), "glGetPixelTexGenParameterivSGIS" },
    { offsetof(SGISPixelTexture, glPixelTexGenParameterf), "glPixelTexGenParameterfSGIS" },
    { offsetof(SGISPixelTexture, glPixelTexGenParameterfv), "glPixelTexGenParameterfvSGIS" },
    { offsetof(SGISPixelTexture, glPixelTexGenParameteri), "glPixelTexGenParameteriSGIS" },
    { offsetof(SGISPixelTexture, glPixelTexGenParameteriv), "glPixelTexGenParameterivSGIS" },
};

static const size_t __SGISPixelTextureEntryCount = sizeof(__SGISPixelTextureEntryPoints) /
    sizeof(__SGISPixelTextureEntryPoints[0]);

static const EntryOffset __SGISPointParametersEntryPoints[] = {
    { offsetof(SGISPointParameters, glPointParameterf), "glPointParameterfSGIS" },
    { offsetof(SGISPointParameters, glPointParameterfv), "glPointParameterfvSGIS" },
};

static const size_t __SGISPointParametersEntryCount = sizeof(__SGISPointParametersEntryPoints) /
    sizeof(__SGISPointParametersEntryPoints[0]);

static const EntryOffset __SGISSharpenTextureEntryPoints[] = {
    { offsetof(SGISSharpenTexture, glGetSharpenTexFunc), "glGetSharpenTexFuncSGIS" },
    { offsetof(SGISSharpenTexture, glSharpenTexFunc), "glSharpenTexFuncSGIS" },
};

static const size_t __SGISSharpenTextureEntryCount = sizeof(__SGISSharpenTextureEntryPoints) /
    sizeof(__SGISSharpenTextureEntryPoints[0]);

static const EntryOffset __SGISTEXTURE4DEntryPoints[] = {
    { offsetof(SGISTEXTURE4D, glTexImage4D), "glTexImage4DSGIS" },
    { offsetof(SGISTEXTURE4D, glTexSubImage4D), "glTexSubImage4DSGIS" },
};

static const size_t __SGISTEXTURE4DEntryCount = sizeof(__SGISTEXTURE4DEntryPoints) /
    sizeof(__SGISTEXTURE4DEntryPoints[0]);

static const EntryOffset __SGISTextureColorMaskEntryPoints[] = {
    { offsetof(SGISTextureColorMask, glTextureColorMask), "glTextureColorMaskSGIS" },
};

static const size_t __SGISTextureColorMaskEntryCount = sizeof(__SGISTextureColorMaskEntryPoints) /
    sizeof(__SGISTextureColorMaskEntryPoints[0]);

static const EntryOffset __SGISTextureFILTER4EntryPoints[] = {
    { offsetof(SGISTextureFILTER4, glGetTexFilterFunc), "glGetTexFilterFuncSGIS" },
    { offsetof(SGISTextureFILTER4, glTexFilterFunc), "glTexFilterFuncSGIS" },
};

static const size_t __SGISTextureFILTER4EntryCount = sizeof(__SGISTextureFILTER4EntryPoints) /
    sizeof(__SGISTextureFILTER4EntryPoints[0]);

static const EntryOffset __SGIXAsyncEntryPoints[] = {
    { offsetof(SGIXAsync, glAsyncMarker), "glAsyncMarkerSGIX" },
    { offsetof(SGIXAsync, glDeleteAsyncMarkers), "glDeleteAsyncMarkersSGIX" },
    { offsetof(SGIXAsync, glFinishAsync), "glFinishAsyncSGIX" },
    { offsetof(SGIXAsync, glGenAsyncMarkers), "glGenAsyncMarkersSGIX" },
    { offsetof(SGIXAsync, glIsAsyncMarker), "glIsAsyncMarkerSGIX" },
    { offsetof(SGIXAsync, glPollAsync), "glPollAsyncSGIX" },
};

static const size_t __SGIXAsyncEntryCount = sizeof(__SGIXAsyncEntryPoints) /
    sizeof(__SGIXAsyncEntryPoints[0]);

static const EntryOffset __SGIXFlushRasterEntryPoints[] = {
    { offsetof(SGIXFlushRaster, glFlushRaster), "glFlushRasterSGIX" },
};

static const size_t __SGIXFlushRasterEntryCount = sizeof(__SGIXFlushRasterEntryPoints) /
    sizeof(__SGIXFlushRasterEntryPoints[0]);

static const EntryOffset __SGIXFragmentLightingEntryPoints[] = {
    { offsetof(SGIXFragmentLighting, glFragmentColorMaterial), "glFragmentColorMaterialSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentLightModelf), "glFragmentLightModelfSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentLightModelfv), "glFragmentLightModelfvSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentLightModeli), "glFragmentLightModeliSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentLightModeliv), "glFragmentLightModelivSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentLightf), "glFragmentLightfSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentLightfv), "glFragmentLightfvSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentLighti), "glFragmentLightiSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentLightiv), "glFragmentLightivSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentMaterialf), "glFragmentMaterialfSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentMaterialfv), "glFragmentMaterialfvSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentMateriali), "glFragmentMaterialiSGIX" },
    { offsetof(SGIXFragmentLighting, glFragmentMaterialiv), "glFragmentMaterialivSGIX" },
    { offsetof(SGIXFragmentLighting, glGetFragmentLightfv), "glGetFragmentLightfvSGIX" },
    { offsetof(SGIXFragmentLighting, glGetFragmentLightiv), "glGetFragmentLightivSGIX" },
    { offsetof(SGIXFragmentLighting, glGetFragmentMaterialfv), "glGetFragmentMaterialfvSGIX" },
    { offsetof(SGIXFragmentLighting, glGetFragmentMaterialiv), "glGetFragmentMaterialivSGIX" },
    { offsetof(SGIXFragmentLighting, glLightEnvi), "glLightEnviSGIX" },
};

static const size_t __SGIXFragmentLightingEntryCount = sizeof(__SGIXFragmentLightingEntryPoints) /
    sizeof(__SGIXFragmentLightingEntryPoints[0]);

static const EntryOffset __SGIXFramezoomEntryPoints[] = {
    { offsetof(SGIXFramezoom, glFrameZoom), "glFrameZoomSGIX" },
};

static const size_t __SGIXFramezoomEntryCount = sizeof(__SGIXFramezoomEntryPoints) /
    sizeof(__SGIXFramezoomEntryPoints[0]);

static const EntryOffset __SGIXIglooInterfaceEntryPoints[] = {
    { offsetof(SGIXIglooInterface, glIglooInterface), "glIglooInterfaceSGIX" },
};

static const size_t __SGIXIglooInterfaceEntryCount = sizeof(__SGIXIglooInterfaceEntryPoints) /
    sizeof(__SGIXIglooInterfaceEntryPoints[0]);

static const EntryOffset __SGIXInstrumentSEntryPoints[] = {
    { offsetof(SGIXInstrumentS, glGetInstruments), "glGetInstrumentsSGIX" },
    { offsetof(SGIXInstrumentS, glInstrumentsBuffer), "glInstrumentsBufferSGIX" },
    { offsetof(SGIXInstrumentS, glPollInstruments), "glPollInstrumentsSGIX" },
    { offsetof(SGIXInstrumentS, glReadInstruments), "glReadInstrumentsSGIX" },
    { offsetof(SGIXInstrumentS, glStartInstruments), "glStartInstrumentsSGIX" },
    { offsetof(SGIXInstrumentS, glStopInstruments), "glStopInstrumentsSGIX" },
};

static const size_t __SGIXInstrumentSEntryCount = sizeof(__SGIXInstrumentSEntryPoints) /
    sizeof(__SGIXInstrumentSEntryPoints[0]);

static const EntryOffset __SGIXListPriorityEntryPoints[] = {
    { offsetof(SGIXListPriority, glGetListParameterfv), "glGetListParameterfvSGIX" },
    { offsetof(SGIXListPriority, glGetListParameteriv), "glGetListParameterivSGIX" },
    { offsetof(SGIXListPriority, glListParameterf), "glListParameterfSGIX" },
    { offsetof(SGIXListPriority, glListParameterfv), "glListParameterfvSGIX" },
    { offsetof(SGIXListPriority, glListParameteri), "glListParameteriSGIX" },
    { offsetof(SGIXListPriority, glListParameteriv), "glListParameterivSGIX" },
};

static const size_t __SGIXListPriorityEntryCount = sizeof(__SGIXListPriorityEntryPoints) /
    sizeof(__SGIXListPriorityEntryPoints[0]);

static const EntryOffset __SGIXPixelTextureEntryPoints[] = {
    { offsetof(SGIXPixelTexture, glPixelTexGen), "glPixelTexGenSGIX" },
};

static const size_t __SGIXPixelTextureEntryCount = sizeof(__SGIXPixelTextureEntryPoints) /
    sizeof(__SGIXPixelTextureEntryPoints[0]);

static const EntryOffset __SGIXPolynomialFfdEntryPoints[] = {
    { offsetof(SGIXPolynomialFfd, glDeform), "glDeformSGIX" },
    { offsetof(SGIXPolynomialFfd, glDeformationMap3d), "glDeformationMap3dSGIX" },
    { offsetof(SGIXPolynomialFfd, glDeformationMap3f), "glDeformationMap3fSGIX" },
    { offsetof(SGIXPolynomialFfd, glLoadIdentityDeformationMap), "glLoadIdentityDeformationMapSGIX" },
};

static const size_t __SGIXPolynomialFfdEntryCount = sizeof(__SGIXPolynomialFfdEntryPoints) /
    sizeof(__SGIXPolynomialFfdEntryPoints[0]);

static const EntryOffset __SGIXReferencePlaneEntryPoints[] = {
    { offsetof(SGIXReferencePlane, glReferencePlane), "glReferencePlaneSGIX" },
};

static const size_t __SGIXReferencePlaneEntryCount = sizeof(__SGIXReferencePlaneEntryPoints) /
    sizeof(__SGIXReferencePlaneEntryPoints[0]);

static const EntryOffset __SGIXSpriteEntryPoints[] = {
    { offsetof(SGIXSprite, glSpriteParameterf), "glSpriteParameterfSGIX" },
    { offsetof(SGIXSprite, glSpriteParameterfv), "glSpriteParameterfvSGIX" },
    { offsetof(SGIXSprite, glSpriteParameteri), "glSpriteParameteriSGIX" },
    { offsetof(SGIXSprite, glSpriteParameteriv), "glSpriteParameterivSGIX" },
};

static const size_t __SGIXSpriteEntryCount = sizeof(__SGIXSpriteEntryPoints) /
    sizeof(__SGIXSpriteEntryPoints[0]);

static const EntryOffset __SGIXTagSampleBufferEntryPoints[] = {
    { offsetof(SGIXTagSampleBuffer, glTagSampleBuffer), "glTagSampleBufferSGIX" },
};

static const size_t __SGIXTagSampleBufferEntryCount = sizeof(__SGIXTagSampleBufferEntryPoints) /
    sizeof(__SGIXTagSampleBufferEntryPoints[0]);

static const EntryOffset __SGIColorTableEntryPoints[] = {
    { offsetof(SGIColorTable, glColorTableParameterfv), "glColorTableParameterfvSGI" },
    { offsetof(SGIColorTable, glColorTableParameteriv), "glColorTableParameterivSGI" },
    { offsetof(SGIColorTable, glColorTable), "glColorTableSGI" },
    { offsetof(SGIColorTable, glCopyColorTable), "glCopyColorTableSGI" },
    { offsetof(SGIColorTable, glGetColorTableParameterfv), "glGetColorTableParameterfvSGI" },
    { offsetof(SGIColorTable, glGetColorTableParameteriv), "glGetColorTableParameterivSGI" },
    { offsetof(SGIColorTable, glGetColorTable), "glGetColorTableSGI" },
};

static const size_t __SGIColorTableEntryCount = sizeof(__SGIColorTableEntryPoints) /
    sizeof(__SGIColorTableEntryPoints[0]);

static const EntryOffset __SUNXConstantDataEntryPoints[] = {
    { offsetof(SUNXConstantData, glFinishTexture), "glFinishTextureSUNX" },
};

static const size_t __SUNXConstantDataEntryCount = sizeof(__SUNXConstantDataEntryPoints) /
    sizeof(__SUNXConstantDataEntryPoints[0]);

static const EntryOffset __SUNGlobalAlphaEntryPoints[] = {
    { offsetof(SUNGlobalAlpha, glGlobalAlphaFactorb), "glGlobalAlphaFactorbSUN" },
    { offsetof(SUNGlobalAlpha, glGlobalAlphaFactord), "glGlobalAlphaFactordSUN" },
    { offsetof(SUNGlobalAlpha, glGlobalAlphaFactorf), "glGlobalAlphaFactorfSUN" },
    { offsetof(SUNGlobalAlpha, glGlobalAlphaFactori), "glGlobalAlphaFactoriSUN" },
    { offsetof(SUNGlobalAlpha, glGlobalAlphaFactors), "glGlobalAlphaFactorsSUN" },
    { offsetof(SUNGlobalAlpha, glGlobalAlphaFactorub), "glGlobalAlphaFactorubSUN" },
    { offsetof(SUNGlobalAlpha, glGlobalAlphaFactorui), "glGlobalAlphaFactoruiSUN" },
    { offsetof(SUNGlobalAlpha, glGlobalAlphaFactorus), "glGlobalAlphaFactorusSUN" },
};

static const size_t __SUNGlobalAlphaEntryCount = sizeof(__SUNGlobalAlphaEntryPoints) /
    sizeof(__SUNGlobalAlphaEntryPoints[0]);

static const EntryOffset __SUNMeshArrayEntryPoints[] = {
    { offsetof(SUNMeshArray, glDrawMeshArrays), "glDrawMeshArraysSUN" },
};

static const size_t __SUNMeshArrayEntryCount = sizeof(__SUNMeshArrayEntryPoints) /
    sizeof(__SUNMeshArrayEntryPoints[0]);

static const EntryOffset __SUNTriangleListEntryPoints[] = {
    { offsetof(SUNTriangleList, glReplacementCodePointer), "glReplacementCodePointerSUN" },
    { offsetof(SUNTriangleList, glReplacementCodeub), "glReplacementCodeubSUN" },
    { offsetof(SUNTriangleList, glReplacementCodeubv), "glReplacementCodeubvSUN" },
    { offsetof(SUNTriangleList, glReplacementCodeui), "glReplacementCodeuiSUN" },
    { offsetof(SUNTriangleList, glReplacementCodeuiv), "glReplacementCodeuivSUN" },
    { offsetof(SUNTriangleList, glReplacementCodeus), "glReplacementCodeusSUN" },
    { offsetof(SUNTriangleList, glReplacementCodeusv), "glReplacementCodeusvSUN" },
};

static const size_t __SUNTriangleListEntryCount = sizeof(__SUNTriangleListEntryPoints) /
    sizeof(__SUNTriangleListEntryPoints[0]);

static const EntryOffset __SUNVertexEntryPoints[] = {
    { offsetof(SUNVertex, glColor3fVertex3f), "glColor3fVertex3fSUN" },
    { offsetof(SUNVertex, glColor3fVertex3fv), "glColor3fVertex3fvSUN" },
    { offsetof(SUNVertex, glColor4fNormal3fVertex3f), "glColor4fNormal3fVertex3fSUN" },
    { offsetof(SUNVertex, glColor4fNormal3fVertex3fv), "glColor4fNormal3fVertex3fvSUN" },
    { offsetof(SUNVertex, glColor4ubVertex2f), "glColor4ubVertex2fSUN" },
    { offsetof(SUNVertex, glColor4ubVertex2fv), "glColor4ubVertex2fvSUN" },
    { offsetof(SUNVertex, glColor4ubVertex3f), "glColor4ubVertex3fSUN" },
    { offsetof(SUNVertex, glColor4ubVertex3fv), "glColor4ubVertex3fvSUN" },
    { offsetof(SUNVertex, glNormal3fVertex3f), "glNormal3fVertex3fSUN" },
    { offsetof(SUNVertex, glNormal3fVertex3fv), "glNormal3fVertex3fvSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiColor3fVertex3f), "glReplacementCodeuiColor3fVertex3fSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiColor3fVertex3fv), "glReplacementCodeuiColor3fVertex3fvSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiColor4fNormal3fVertex3f), "glReplacementCodeuiColor4fNormal3fVertex3fSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiColor4fNormal3fVertex3fv), "glReplacementCodeuiColor4fNormal3fVertex3fvSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiColor4ubVertex3f), "glReplacementCodeuiColor4ubVertex3fSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiColor4ubVertex3fv), "glReplacementCodeuiColor4ubVertex3fvSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiNormal3fVertex3f), "glReplacementCodeuiNormal3fVertex3fSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiNormal3fVertex3fv), "glReplacementCodeuiNormal3fVertex3fvSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3f), "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fv), "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiTexCoord2fNormal3fVertex3f), "glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiTexCoord2fNormal3fVertex3fv), "glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiTexCoord2fVertex3f), "glReplacementCodeuiTexCoord2fVertex3fSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiTexCoord2fVertex3fv), "glReplacementCodeuiTexCoord2fVertex3fvSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiVertex3f), "glReplacementCodeuiVertex3fSUN" },
    { offsetof(SUNVertex, glReplacementCodeuiVertex3fv), "glReplacementCodeuiVertex3fvSUN" },
    { offsetof(SUNVertex, glTexCoord2fColor3fVertex3f), "glTexCoord2fColor3fVertex3fSUN" },
    { offsetof(SUNVertex, glTexCoord2fColor3fVertex3fv), "glTexCoord2fColor3fVertex3fvSUN" },
    { offsetof(SUNVertex, glTexCoord2fColor4fNormal3fVertex3f), "glTexCoord2fColor4fNormal3fVertex3fSUN" },
    { offsetof(SUNVertex, glTexCoord2fColor4fNormal3fVertex3fv), "glTexCoord2fColor4fNormal3fVertex3fvSUN" },
    { offsetof(SUNVertex, glTexCoord2fColor4ubVertex3f), "glTexCoord2fColor4ubVertex3fSUN" },
    { offsetof(SUNVertex, glTexCoord2fColor4ubVertex3fv), "glTexCoord2fColor4ubVertex3fvSUN" },
    { offsetof(SUNVertex, glTexCoord2fNormal3fVertex3f), "glTexCoord2fNormal3fVertex3fSUN" },
    { offsetof(SUNVertex, glTexCoord2fNormal3fVertex3fv), "glTexCoord2fNormal3fVertex3fvSUN" },
    { offsetof(SUNVertex, glTexCoord2fVertex3f), "glTexCoord2fVertex3fSUN" },
    { offsetof(SUNVertex, glTexCoord2fVertex3fv), "glTexCoord2fVertex3fvSUN" },
    { offsetof(SUNVertex, glTexCoord4fColor4fNormal3fVertex4f), "glTexCoord4fColor4fNormal3fVertex4fSUN" },
    { offsetof(SUNVertex, glTexCoord4fColor4fNormal3fVertex4fv), "glTexCoord4fColor4fNormal3fVertex4fvSUN" },
    { offsetof(SUNVertex, glTexCoord4fVertex4f), "glTexCoord4fVertex4fSUN" },
    { offsetof(SUNVertex, glTexCoord4fVertex4fv), "glTexCoord4fVertex4fvSUN" },
};

static const size_t __SUNVertexEntryCount = sizeof(__SUNVertexEntryPoints) /
    sizeof(__SUNVertexEntryPoints[0]);

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// GL Member Definitions
////////////////////////////////////////////////////////////////////////////////
GL::GL()
{
    std::for_each_n(__GLEntryPoints, __GLEntryCount,
                    ClearEntryPoint(this));
}

void GL::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__GLEntryPoints, __GLEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// _3DFXTbuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
_3DFXTbuffer::_3DFXTbuffer()
{
    std::for_each_n(___3DFXTbufferEntryPoints, ___3DFXTbufferEntryCount,
                    ClearEntryPoint(this));
}

bool _3DFXTbuffer::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_3DFX_tbuffer");
}

void _3DFXTbuffer::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(___3DFXTbufferEntryPoints, ___3DFXTbufferEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDDebugOutput Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDDebugOutput::AMDDebugOutput()
{
    std::for_each_n(__AMDDebugOutputEntryPoints, __AMDDebugOutputEntryCount,
                    ClearEntryPoint(this));
}

bool AMDDebugOutput::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_debug_output");
}

void AMDDebugOutput::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDDebugOutputEntryPoints, __AMDDebugOutputEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDDrawBuffersBlend Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDDrawBuffersBlend::AMDDrawBuffersBlend()
{
    std::for_each_n(__AMDDrawBuffersBlendEntryPoints, __AMDDrawBuffersBlendEntryCount,
                    ClearEntryPoint(this));
}

bool AMDDrawBuffersBlend::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_draw_buffers_blend");
}

void AMDDrawBuffersBlend::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDDrawBuffersBlendEntryPoints, __AMDDrawBuffersBlendEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDFramebufferMultisampleAdvanced Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDFramebufferMultisampleAdvanced::AMDFramebufferMultisampleAdvanced()
{
    std::for_each_n(__AMDFramebufferMultisampleAdvancedEntryPoints, __AMDFramebufferMultisampleAdvancedEntryCount,
                    ClearEntryPoint(this));
}

bool AMDFramebufferMultisampleAdvanced::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_framebuffer_multisample_advanced");
}

void AMDFramebufferMultisampleAdvanced::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDFramebufferMultisampleAdvancedEntryPoints, __AMDFramebufferMultisampleAdvancedEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDFramebufferSamplePositions Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDFramebufferSamplePositions::AMDFramebufferSamplePositions()
{
    std::for_each_n(__AMDFramebufferSamplePositionsEntryPoints, __AMDFramebufferSamplePositionsEntryCount,
                    ClearEntryPoint(this));
}

bool AMDFramebufferSamplePositions::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_framebuffer_sample_positions");
}

void AMDFramebufferSamplePositions::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDFramebufferSamplePositionsEntryPoints, __AMDFramebufferSamplePositionsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDGPUShaderInt64 Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDGPUShaderInt64::AMDGPUShaderInt64()
{
    std::for_each_n(__AMDGPUShaderInt64EntryPoints, __AMDGPUShaderInt64EntryCount,
                    ClearEntryPoint(this));
}

bool AMDGPUShaderInt64::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_gpu_shader_int64");
}

void AMDGPUShaderInt64::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDGPUShaderInt64EntryPoints, __AMDGPUShaderInt64EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDInterleavedElements Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDInterleavedElements::AMDInterleavedElements()
{
    std::for_each_n(__AMDInterleavedElementsEntryPoints, __AMDInterleavedElementsEntryCount,
                    ClearEntryPoint(this));
}

bool AMDInterleavedElements::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_interleaved_elements");
}

void AMDInterleavedElements::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDInterleavedElementsEntryPoints, __AMDInterleavedElementsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDMultiDrawIndirect Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDMultiDrawIndirect::AMDMultiDrawIndirect()
{
    std::for_each_n(__AMDMultiDrawIndirectEntryPoints, __AMDMultiDrawIndirectEntryCount,
                    ClearEntryPoint(this));
}

bool AMDMultiDrawIndirect::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_multi_draw_indirect");
}

void AMDMultiDrawIndirect::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDMultiDrawIndirectEntryPoints, __AMDMultiDrawIndirectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDNameGenDelete Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDNameGenDelete::AMDNameGenDelete()
{
    std::for_each_n(__AMDNameGenDeleteEntryPoints, __AMDNameGenDeleteEntryCount,
                    ClearEntryPoint(this));
}

bool AMDNameGenDelete::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_name_gen_delete");
}

void AMDNameGenDelete::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDNameGenDeleteEntryPoints, __AMDNameGenDeleteEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDOcclusionQueryEvent Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDOcclusionQueryEvent::AMDOcclusionQueryEvent()
{
    std::for_each_n(__AMDOcclusionQueryEventEntryPoints, __AMDOcclusionQueryEventEntryCount,
                    ClearEntryPoint(this));
}

bool AMDOcclusionQueryEvent::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_occlusion_query_event");
}

void AMDOcclusionQueryEvent::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDOcclusionQueryEventEntryPoints, __AMDOcclusionQueryEventEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDPerformanceMonitor Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDPerformanceMonitor::AMDPerformanceMonitor()
{
    std::for_each_n(__AMDPerformanceMonitorEntryPoints, __AMDPerformanceMonitorEntryCount,
                    ClearEntryPoint(this));
}

bool AMDPerformanceMonitor::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_performance_monitor");
}

void AMDPerformanceMonitor::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDPerformanceMonitorEntryPoints, __AMDPerformanceMonitorEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDSamplePositions Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDSamplePositions::AMDSamplePositions()
{
    std::for_each_n(__AMDSamplePositionsEntryPoints, __AMDSamplePositionsEntryCount,
                    ClearEntryPoint(this));
}

bool AMDSamplePositions::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_sample_positions");
}

void AMDSamplePositions::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDSamplePositionsEntryPoints, __AMDSamplePositionsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDSparseTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDSparseTexture::AMDSparseTexture()
{
    std::for_each_n(__AMDSparseTextureEntryPoints, __AMDSparseTextureEntryCount,
                    ClearEntryPoint(this));
}

bool AMDSparseTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_sparse_texture");
}

void AMDSparseTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDSparseTextureEntryPoints, __AMDSparseTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDStencilOperationExtended Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDStencilOperationExtended::AMDStencilOperationExtended()
{
    std::for_each_n(__AMDStencilOperationExtendedEntryPoints, __AMDStencilOperationExtendedEntryCount,
                    ClearEntryPoint(this));
}

bool AMDStencilOperationExtended::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_stencil_operation_extended");
}

void AMDStencilOperationExtended::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDStencilOperationExtendedEntryPoints, __AMDStencilOperationExtendedEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDVertexShaderTessellator Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDVertexShaderTessellator::AMDVertexShaderTessellator()
{
    std::for_each_n(__AMDVertexShaderTessellatorEntryPoints, __AMDVertexShaderTessellatorEntryCount,
                    ClearEntryPoint(this));
}

bool AMDVertexShaderTessellator::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_AMD_vertex_shader_tessellator");
}

void AMDVertexShaderTessellator::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__AMDVertexShaderTessellatorEntryPoints, __AMDVertexShaderTessellatorEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// APPLEElementArray Member Definitions
////////////////////////////////////////////////////////////////////////////////
APPLEElementArray::APPLEElementArray()
{
    std::for_each_n(__APPLEElementArrayEntryPoints, __APPLEElementArrayEntryCount,
                    ClearEntryPoint(this));
}

bool APPLEElementArray::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_APPLE_element_array");
}

void APPLEElementArray::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__APPLEElementArrayEntryPoints, __APPLEElementArrayEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// APPLEFence Member Definitions
////////////////////////////////////////////////////////////////////////////////
APPLEFence::APPLEFence()
{
    std::for_each_n(__APPLEFenceEntryPoints, __APPLEFenceEntryCount,
                    ClearEntryPoint(this));
}

bool APPLEFence::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_APPLE_fence");
}

void APPLEFence::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__APPLEFenceEntryPoints, __APPLEFenceEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// APPLEFlushBufferRange Member Definitions
////////////////////////////////////////////////////////////////////////////////
APPLEFlushBufferRange::APPLEFlushBufferRange()
{
    std::for_each_n(__APPLEFlushBufferRangeEntryPoints, __APPLEFlushBufferRangeEntryCount,
                    ClearEntryPoint(this));
}

bool APPLEFlushBufferRange::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_APPLE_flush_buffer_range");
}

void APPLEFlushBufferRange::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__APPLEFlushBufferRangeEntryPoints, __APPLEFlushBufferRangeEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// APPLEObjectPurgeable Member Definitions
////////////////////////////////////////////////////////////////////////////////
APPLEObjectPurgeable::APPLEObjectPurgeable()
{
    std::for_each_n(__APPLEObjectPurgeableEntryPoints, __APPLEObjectPurgeableEntryCount,
                    ClearEntryPoint(this));
}

bool APPLEObjectPurgeable::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_APPLE_object_purgeable");
}

void APPLEObjectPurgeable::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__APPLEObjectPurgeableEntryPoints, __APPLEObjectPurgeableEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// APPLETextureRange Member Definitions
////////////////////////////////////////////////////////////////////////////////
APPLETextureRange::APPLETextureRange()
{
    std::for_each_n(__APPLETextureRangeEntryPoints, __APPLETextureRangeEntryCount,
                    ClearEntryPoint(this));
}

bool APPLETextureRange::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_APPLE_texture_range");
}

void APPLETextureRange::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__APPLETextureRangeEntryPoints, __APPLETextureRangeEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// APPLEVertexArrayObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
APPLEVertexArrayObject::APPLEVertexArrayObject()
{
    std::for_each_n(__APPLEVertexArrayObjectEntryPoints, __APPLEVertexArrayObjectEntryCount,
                    ClearEntryPoint(this));
}

bool APPLEVertexArrayObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_APPLE_vertex_array_object");
}

void APPLEVertexArrayObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__APPLEVertexArrayObjectEntryPoints, __APPLEVertexArrayObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// APPLEVertexArrayRange Member Definitions
////////////////////////////////////////////////////////////////////////////////
APPLEVertexArrayRange::APPLEVertexArrayRange()
{
    std::for_each_n(__APPLEVertexArrayRangeEntryPoints, __APPLEVertexArrayRangeEntryCount,
                    ClearEntryPoint(this));
}

bool APPLEVertexArrayRange::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_APPLE_vertex_array_range");
}

void APPLEVertexArrayRange::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__APPLEVertexArrayRangeEntryPoints, __APPLEVertexArrayRangeEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// APPLEVertexProgramEvaluators Member Definitions
////////////////////////////////////////////////////////////////////////////////
APPLEVertexProgramEvaluators::APPLEVertexProgramEvaluators()
{
    std::for_each_n(__APPLEVertexProgramEvaluatorsEntryPoints, __APPLEVertexProgramEvaluatorsEntryCount,
                    ClearEntryPoint(this));
}

bool APPLEVertexProgramEvaluators::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_APPLE_vertex_program_evaluators");
}

void APPLEVertexProgramEvaluators::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__APPLEVertexProgramEvaluatorsEntryPoints, __APPLEVertexProgramEvaluatorsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBES32Compatibility Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBES32Compatibility::ARBES32Compatibility()
{
    std::for_each_n(__ARBES32CompatibilityEntryPoints, __ARBES32CompatibilityEntryCount,
                    ClearEntryPoint(this));
}

bool ARBES32Compatibility::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_ES3_2_compatibility");
}

void ARBES32Compatibility::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBES32CompatibilityEntryPoints, __ARBES32CompatibilityEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBBindlessTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBBindlessTexture::ARBBindlessTexture()
{
    std::for_each_n(__ARBBindlessTextureEntryPoints, __ARBBindlessTextureEntryCount,
                    ClearEntryPoint(this));
}

bool ARBBindlessTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_bindless_texture");
}

void ARBBindlessTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBBindlessTextureEntryPoints, __ARBBindlessTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBClEvent Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBClEvent::ARBClEvent()
{
    std::for_each_n(__ARBClEventEntryPoints, __ARBClEventEntryCount,
                    ClearEntryPoint(this));
}

bool ARBClEvent::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_cl_event");
}

void ARBClEvent::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBClEventEntryPoints, __ARBClEventEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBColorBufferFloat Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBColorBufferFloat::ARBColorBufferFloat()
{
    std::for_each_n(__ARBColorBufferFloatEntryPoints, __ARBColorBufferFloatEntryCount,
                    ClearEntryPoint(this));
}

bool ARBColorBufferFloat::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_color_buffer_float");
}

void ARBColorBufferFloat::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBColorBufferFloatEntryPoints, __ARBColorBufferFloatEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBComputeVariableGroupSize Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBComputeVariableGroupSize::ARBComputeVariableGroupSize()
{
    std::for_each_n(__ARBComputeVariableGroupSizeEntryPoints, __ARBComputeVariableGroupSizeEntryCount,
                    ClearEntryPoint(this));
}

bool ARBComputeVariableGroupSize::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_compute_variable_group_size");
}

void ARBComputeVariableGroupSize::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBComputeVariableGroupSizeEntryPoints, __ARBComputeVariableGroupSizeEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBDebugOutput Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBDebugOutput::ARBDebugOutput()
{
    std::for_each_n(__ARBDebugOutputEntryPoints, __ARBDebugOutputEntryCount,
                    ClearEntryPoint(this));
}

bool ARBDebugOutput::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_debug_output");
}

void ARBDebugOutput::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBDebugOutputEntryPoints, __ARBDebugOutputEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBDrawBuffers Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBDrawBuffers::ARBDrawBuffers()
{
    std::for_each_n(__ARBDrawBuffersEntryPoints, __ARBDrawBuffersEntryCount,
                    ClearEntryPoint(this));
}

bool ARBDrawBuffers::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_draw_buffers");
}

void ARBDrawBuffers::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBDrawBuffersEntryPoints, __ARBDrawBuffersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBDrawBuffersBlend Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBDrawBuffersBlend::ARBDrawBuffersBlend()
{
    std::for_each_n(__ARBDrawBuffersBlendEntryPoints, __ARBDrawBuffersBlendEntryCount,
                    ClearEntryPoint(this));
}

bool ARBDrawBuffersBlend::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_draw_buffers_blend");
}

void ARBDrawBuffersBlend::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBDrawBuffersBlendEntryPoints, __ARBDrawBuffersBlendEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBDrawInstanced Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBDrawInstanced::ARBDrawInstanced()
{
    std::for_each_n(__ARBDrawInstancedEntryPoints, __ARBDrawInstancedEntryCount,
                    ClearEntryPoint(this));
}

bool ARBDrawInstanced::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_draw_instanced");
}

void ARBDrawInstanced::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBDrawInstancedEntryPoints, __ARBDrawInstancedEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBFragmentProgram Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBFragmentProgram::ARBFragmentProgram()
{
    std::for_each_n(__ARBFragmentProgramEntryPoints, __ARBFragmentProgramEntryCount,
                    ClearEntryPoint(this));
}

bool ARBFragmentProgram::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_fragment_program");
}

void ARBFragmentProgram::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBFragmentProgramEntryPoints, __ARBFragmentProgramEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBGeometrySHADER4 Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBGeometrySHADER4::ARBGeometrySHADER4()
{
    std::for_each_n(__ARBGeometrySHADER4EntryPoints, __ARBGeometrySHADER4EntryCount,
                    ClearEntryPoint(this));
}

bool ARBGeometrySHADER4::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_geometry_shader4");
}

void ARBGeometrySHADER4::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBGeometrySHADER4EntryPoints, __ARBGeometrySHADER4EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBGlSpirv Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBGlSpirv::ARBGlSpirv()
{
    std::for_each_n(__ARBGlSpirvEntryPoints, __ARBGlSpirvEntryCount,
                    ClearEntryPoint(this));
}

bool ARBGlSpirv::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_gl_spirv");
}

void ARBGlSpirv::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBGlSpirvEntryPoints, __ARBGlSpirvEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBGPUShaderInt64 Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBGPUShaderInt64::ARBGPUShaderInt64()
{
    std::for_each_n(__ARBGPUShaderInt64EntryPoints, __ARBGPUShaderInt64EntryCount,
                    ClearEntryPoint(this));
}

bool ARBGPUShaderInt64::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_gpu_shader_int64");
}

void ARBGPUShaderInt64::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBGPUShaderInt64EntryPoints, __ARBGPUShaderInt64EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBIndirectParameters Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBIndirectParameters::ARBIndirectParameters()
{
    std::for_each_n(__ARBIndirectParametersEntryPoints, __ARBIndirectParametersEntryCount,
                    ClearEntryPoint(this));
}

bool ARBIndirectParameters::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_indirect_parameters");
}

void ARBIndirectParameters::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBIndirectParametersEntryPoints, __ARBIndirectParametersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBInstancedArrays Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBInstancedArrays::ARBInstancedArrays()
{
    std::for_each_n(__ARBInstancedArraysEntryPoints, __ARBInstancedArraysEntryCount,
                    ClearEntryPoint(this));
}

bool ARBInstancedArrays::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_instanced_arrays");
}

void ARBInstancedArrays::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBInstancedArraysEntryPoints, __ARBInstancedArraysEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBMatrixPalette Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBMatrixPalette::ARBMatrixPalette()
{
    std::for_each_n(__ARBMatrixPaletteEntryPoints, __ARBMatrixPaletteEntryCount,
                    ClearEntryPoint(this));
}

bool ARBMatrixPalette::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_matrix_palette");
}

void ARBMatrixPalette::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBMatrixPaletteEntryPoints, __ARBMatrixPaletteEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBMultisample Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBMultisample::ARBMultisample()
{
    std::for_each_n(__ARBMultisampleEntryPoints, __ARBMultisampleEntryCount,
                    ClearEntryPoint(this));
}

bool ARBMultisample::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_multisample");
}

void ARBMultisample::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBMultisampleEntryPoints, __ARBMultisampleEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBMultitexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBMultitexture::ARBMultitexture()
{
    std::for_each_n(__ARBMultitextureEntryPoints, __ARBMultitextureEntryCount,
                    ClearEntryPoint(this));
}

bool ARBMultitexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_multitexture");
}

void ARBMultitexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBMultitextureEntryPoints, __ARBMultitextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBOcclusionQuery Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBOcclusionQuery::ARBOcclusionQuery()
{
    std::for_each_n(__ARBOcclusionQueryEntryPoints, __ARBOcclusionQueryEntryCount,
                    ClearEntryPoint(this));
}

bool ARBOcclusionQuery::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_occlusion_query");
}

void ARBOcclusionQuery::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBOcclusionQueryEntryPoints, __ARBOcclusionQueryEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBParallelShaderCompile Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBParallelShaderCompile::ARBParallelShaderCompile()
{
    std::for_each_n(__ARBParallelShaderCompileEntryPoints, __ARBParallelShaderCompileEntryCount,
                    ClearEntryPoint(this));
}

bool ARBParallelShaderCompile::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_parallel_shader_compile");
}

void ARBParallelShaderCompile::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBParallelShaderCompileEntryPoints, __ARBParallelShaderCompileEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBPointParameters Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBPointParameters::ARBPointParameters()
{
    std::for_each_n(__ARBPointParametersEntryPoints, __ARBPointParametersEntryCount,
                    ClearEntryPoint(this));
}

bool ARBPointParameters::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_point_parameters");
}

void ARBPointParameters::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBPointParametersEntryPoints, __ARBPointParametersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBRobustness Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBRobustness::ARBRobustness()
{
    std::for_each_n(__ARBRobustnessEntryPoints, __ARBRobustnessEntryCount,
                    ClearEntryPoint(this));
}

bool ARBRobustness::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_robustness");
}

void ARBRobustness::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBRobustnessEntryPoints, __ARBRobustnessEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBSampleLocations Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBSampleLocations::ARBSampleLocations()
{
    std::for_each_n(__ARBSampleLocationsEntryPoints, __ARBSampleLocationsEntryCount,
                    ClearEntryPoint(this));
}

bool ARBSampleLocations::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_sample_locations");
}

void ARBSampleLocations::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBSampleLocationsEntryPoints, __ARBSampleLocationsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBSampleShading Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBSampleShading::ARBSampleShading()
{
    std::for_each_n(__ARBSampleShadingEntryPoints, __ARBSampleShadingEntryCount,
                    ClearEntryPoint(this));
}

bool ARBSampleShading::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_sample_shading");
}

void ARBSampleShading::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBSampleShadingEntryPoints, __ARBSampleShadingEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBShaderObjects Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBShaderObjects::ARBShaderObjects()
{
    std::for_each_n(__ARBShaderObjectsEntryPoints, __ARBShaderObjectsEntryCount,
                    ClearEntryPoint(this));
}

bool ARBShaderObjects::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_shader_objects");
}

void ARBShaderObjects::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBShaderObjectsEntryPoints, __ARBShaderObjectsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBShadingLanguageInclude Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBShadingLanguageInclude::ARBShadingLanguageInclude()
{
    std::for_each_n(__ARBShadingLanguageIncludeEntryPoints, __ARBShadingLanguageIncludeEntryCount,
                    ClearEntryPoint(this));
}

bool ARBShadingLanguageInclude::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_shading_language_include");
}

void ARBShadingLanguageInclude::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBShadingLanguageIncludeEntryPoints, __ARBShadingLanguageIncludeEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBSparseBuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBSparseBuffer::ARBSparseBuffer()
{
    std::for_each_n(__ARBSparseBufferEntryPoints, __ARBSparseBufferEntryCount,
                    ClearEntryPoint(this));
}

bool ARBSparseBuffer::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_sparse_buffer");
}

void ARBSparseBuffer::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBSparseBufferEntryPoints, __ARBSparseBufferEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBSparseTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBSparseTexture::ARBSparseTexture()
{
    std::for_each_n(__ARBSparseTextureEntryPoints, __ARBSparseTextureEntryCount,
                    ClearEntryPoint(this));
}

bool ARBSparseTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_sparse_texture");
}

void ARBSparseTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBSparseTextureEntryPoints, __ARBSparseTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBTextureBufferObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBTextureBufferObject::ARBTextureBufferObject()
{
    std::for_each_n(__ARBTextureBufferObjectEntryPoints, __ARBTextureBufferObjectEntryCount,
                    ClearEntryPoint(this));
}

bool ARBTextureBufferObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_texture_buffer_object");
}

void ARBTextureBufferObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBTextureBufferObjectEntryPoints, __ARBTextureBufferObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBTextureCompression Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBTextureCompression::ARBTextureCompression()
{
    std::for_each_n(__ARBTextureCompressionEntryPoints, __ARBTextureCompressionEntryCount,
                    ClearEntryPoint(this));
}

bool ARBTextureCompression::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_texture_compression");
}

void ARBTextureCompression::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBTextureCompressionEntryPoints, __ARBTextureCompressionEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBTransposeMatrix Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBTransposeMatrix::ARBTransposeMatrix()
{
    std::for_each_n(__ARBTransposeMatrixEntryPoints, __ARBTransposeMatrixEntryCount,
                    ClearEntryPoint(this));
}

bool ARBTransposeMatrix::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_transpose_matrix");
}

void ARBTransposeMatrix::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBTransposeMatrixEntryPoints, __ARBTransposeMatrixEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBVertexBlend Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBVertexBlend::ARBVertexBlend()
{
    std::for_each_n(__ARBVertexBlendEntryPoints, __ARBVertexBlendEntryCount,
                    ClearEntryPoint(this));
}

bool ARBVertexBlend::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_vertex_blend");
}

void ARBVertexBlend::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBVertexBlendEntryPoints, __ARBVertexBlendEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBVertexBufferObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBVertexBufferObject::ARBVertexBufferObject()
{
    std::for_each_n(__ARBVertexBufferObjectEntryPoints, __ARBVertexBufferObjectEntryCount,
                    ClearEntryPoint(this));
}

bool ARBVertexBufferObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_vertex_buffer_object");
}

void ARBVertexBufferObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBVertexBufferObjectEntryPoints, __ARBVertexBufferObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBVertexProgram Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBVertexProgram::ARBVertexProgram()
{
    std::for_each_n(__ARBVertexProgramEntryPoints, __ARBVertexProgramEntryCount,
                    ClearEntryPoint(this));
}

bool ARBVertexProgram::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_vertex_program");
}

void ARBVertexProgram::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBVertexProgramEntryPoints, __ARBVertexProgramEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBVertexShader Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBVertexShader::ARBVertexShader()
{
    std::for_each_n(__ARBVertexShaderEntryPoints, __ARBVertexShaderEntryCount,
                    ClearEntryPoint(this));
}

bool ARBVertexShader::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_vertex_shader");
}

void ARBVertexShader::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBVertexShaderEntryPoints, __ARBVertexShaderEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBViewportArray Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBViewportArray::ARBViewportArray()
{
    std::for_each_n(__ARBViewportArrayEntryPoints, __ARBViewportArrayEntryCount,
                    ClearEntryPoint(this));
}

bool ARBViewportArray::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_viewport_array");
}

void ARBViewportArray::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBViewportArrayEntryPoints, __ARBViewportArrayEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBWindowPos Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBWindowPos::ARBWindowPos()
{
    std::for_each_n(__ARBWindowPosEntryPoints, __ARBWindowPosEntryCount,
                    ClearEntryPoint(this));
}

bool ARBWindowPos::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ARB_window_pos");
}

void ARBWindowPos::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ARBWindowPosEntryPoints, __ARBWindowPosEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIDrawBuffers Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIDrawBuffers::ATIDrawBuffers()
{
    std::for_each_n(__ATIDrawBuffersEntryPoints, __ATIDrawBuffersEntryCount,
                    ClearEntryPoint(this));
}

bool ATIDrawBuffers::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_draw_buffers");
}

void ATIDrawBuffers::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIDrawBuffersEntryPoints, __ATIDrawBuffersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIElementArray Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIElementArray::ATIElementArray()
{
    std::for_each_n(__ATIElementArrayEntryPoints, __ATIElementArrayEntryCount,
                    ClearEntryPoint(this));
}

bool ATIElementArray::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_element_array");
}

void ATIElementArray::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIElementArrayEntryPoints, __ATIElementArrayEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIEnvmapBumpmap Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIEnvmapBumpmap::ATIEnvmapBumpmap()
{
    std::for_each_n(__ATIEnvmapBumpmapEntryPoints, __ATIEnvmapBumpmapEntryCount,
                    ClearEntryPoint(this));
}

bool ATIEnvmapBumpmap::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_envmap_bumpmap");
}

void ATIEnvmapBumpmap::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIEnvmapBumpmapEntryPoints, __ATIEnvmapBumpmapEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIFragmentShader Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIFragmentShader::ATIFragmentShader()
{
    std::for_each_n(__ATIFragmentShaderEntryPoints, __ATIFragmentShaderEntryCount,
                    ClearEntryPoint(this));
}

bool ATIFragmentShader::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_fragment_shader");
}

void ATIFragmentShader::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIFragmentShaderEntryPoints, __ATIFragmentShaderEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIMapObjectBuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIMapObjectBuffer::ATIMapObjectBuffer()
{
    std::for_each_n(__ATIMapObjectBufferEntryPoints, __ATIMapObjectBufferEntryCount,
                    ClearEntryPoint(this));
}

bool ATIMapObjectBuffer::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_map_object_buffer");
}

void ATIMapObjectBuffer::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIMapObjectBufferEntryPoints, __ATIMapObjectBufferEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIPnTriangles Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIPnTriangles::ATIPnTriangles()
{
    std::for_each_n(__ATIPnTrianglesEntryPoints, __ATIPnTrianglesEntryCount,
                    ClearEntryPoint(this));
}

bool ATIPnTriangles::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_pn_triangles");
}

void ATIPnTriangles::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIPnTrianglesEntryPoints, __ATIPnTrianglesEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATISeparateStencil Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATISeparateStencil::ATISeparateStencil()
{
    std::for_each_n(__ATISeparateStencilEntryPoints, __ATISeparateStencilEntryCount,
                    ClearEntryPoint(this));
}

bool ATISeparateStencil::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_separate_stencil");
}

void ATISeparateStencil::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATISeparateStencilEntryPoints, __ATISeparateStencilEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIVertexArrayObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIVertexArrayObject::ATIVertexArrayObject()
{
    std::for_each_n(__ATIVertexArrayObjectEntryPoints, __ATIVertexArrayObjectEntryCount,
                    ClearEntryPoint(this));
}

bool ATIVertexArrayObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_vertex_array_object");
}

void ATIVertexArrayObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIVertexArrayObjectEntryPoints, __ATIVertexArrayObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIVertexAttribArrayObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIVertexAttribArrayObject::ATIVertexAttribArrayObject()
{
    std::for_each_n(__ATIVertexAttribArrayObjectEntryPoints, __ATIVertexAttribArrayObjectEntryCount,
                    ClearEntryPoint(this));
}

bool ATIVertexAttribArrayObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_vertex_attrib_array_object");
}

void ATIVertexAttribArrayObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIVertexAttribArrayObjectEntryPoints, __ATIVertexAttribArrayObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ATIVertexStreams Member Definitions
////////////////////////////////////////////////////////////////////////////////
ATIVertexStreams::ATIVertexStreams()
{
    std::for_each_n(__ATIVertexStreamsEntryPoints, __ATIVertexStreamsEntryCount,
                    ClearEntryPoint(this));
}

bool ATIVertexStreams::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_ATI_vertex_streams");
}

void ATIVertexStreams::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__ATIVertexStreamsEntryPoints, __ATIVertexStreamsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTEglImageStorage Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTEglImageStorage::EXTEglImageStorage()
{
    std::for_each_n(__EXTEglImageStorageEntryPoints, __EXTEglImageStorageEntryCount,
                    ClearEntryPoint(this));
}

bool EXTEglImageStorage::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_EGL_image_storage");
}

void EXTEglImageStorage::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTEglImageStorageEntryPoints, __EXTEglImageStorageEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTBindableUniform Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTBindableUniform::EXTBindableUniform()
{
    std::for_each_n(__EXTBindableUniformEntryPoints, __EXTBindableUniformEntryCount,
                    ClearEntryPoint(this));
}

bool EXTBindableUniform::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_bindable_uniform");
}

void EXTBindableUniform::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTBindableUniformEntryPoints, __EXTBindableUniformEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTBlendColor Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTBlendColor::EXTBlendColor()
{
    std::for_each_n(__EXTBlendColorEntryPoints, __EXTBlendColorEntryCount,
                    ClearEntryPoint(this));
}

bool EXTBlendColor::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_blend_color");
}

void EXTBlendColor::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTBlendColorEntryPoints, __EXTBlendColorEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTBlendEquationSeparate Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTBlendEquationSeparate::EXTBlendEquationSeparate()
{
    std::for_each_n(__EXTBlendEquationSeparateEntryPoints, __EXTBlendEquationSeparateEntryCount,
                    ClearEntryPoint(this));
}

bool EXTBlendEquationSeparate::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_blend_equation_separate");
}

void EXTBlendEquationSeparate::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTBlendEquationSeparateEntryPoints, __EXTBlendEquationSeparateEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTBlendFuncSeparate Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTBlendFuncSeparate::EXTBlendFuncSeparate()
{
    std::for_each_n(__EXTBlendFuncSeparateEntryPoints, __EXTBlendFuncSeparateEntryCount,
                    ClearEntryPoint(this));
}

bool EXTBlendFuncSeparate::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_blend_func_separate");
}

void EXTBlendFuncSeparate::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTBlendFuncSeparateEntryPoints, __EXTBlendFuncSeparateEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTBlendMinmax Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTBlendMinmax::EXTBlendMinmax()
{
    std::for_each_n(__EXTBlendMinmaxEntryPoints, __EXTBlendMinmaxEntryCount,
                    ClearEntryPoint(this));
}

bool EXTBlendMinmax::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_blend_minmax");
}

void EXTBlendMinmax::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTBlendMinmaxEntryPoints, __EXTBlendMinmaxEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTColorSubtable Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTColorSubtable::EXTColorSubtable()
{
    std::for_each_n(__EXTColorSubtableEntryPoints, __EXTColorSubtableEntryCount,
                    ClearEntryPoint(this));
}

bool EXTColorSubtable::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_color_subtable");
}

void EXTColorSubtable::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTColorSubtableEntryPoints, __EXTColorSubtableEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTCompiledVertexArray Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTCompiledVertexArray::EXTCompiledVertexArray()
{
    std::for_each_n(__EXTCompiledVertexArrayEntryPoints, __EXTCompiledVertexArrayEntryCount,
                    ClearEntryPoint(this));
}

bool EXTCompiledVertexArray::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_compiled_vertex_array");
}

void EXTCompiledVertexArray::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTCompiledVertexArrayEntryPoints, __EXTCompiledVertexArrayEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTConvolution Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTConvolution::EXTConvolution()
{
    std::for_each_n(__EXTConvolutionEntryPoints, __EXTConvolutionEntryCount,
                    ClearEntryPoint(this));
}

bool EXTConvolution::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_convolution");
}

void EXTConvolution::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTConvolutionEntryPoints, __EXTConvolutionEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTCoordinateFrame Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTCoordinateFrame::EXTCoordinateFrame()
{
    std::for_each_n(__EXTCoordinateFrameEntryPoints, __EXTCoordinateFrameEntryCount,
                    ClearEntryPoint(this));
}

bool EXTCoordinateFrame::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_coordinate_frame");
}

void EXTCoordinateFrame::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTCoordinateFrameEntryPoints, __EXTCoordinateFrameEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTCopyTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTCopyTexture::EXTCopyTexture()
{
    std::for_each_n(__EXTCopyTextureEntryPoints, __EXTCopyTextureEntryCount,
                    ClearEntryPoint(this));
}

bool EXTCopyTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_copy_texture");
}

void EXTCopyTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTCopyTextureEntryPoints, __EXTCopyTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTCullVertex Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTCullVertex::EXTCullVertex()
{
    std::for_each_n(__EXTCullVertexEntryPoints, __EXTCullVertexEntryCount,
                    ClearEntryPoint(this));
}

bool EXTCullVertex::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_cull_vertex");
}

void EXTCullVertex::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTCullVertexEntryPoints, __EXTCullVertexEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTDebugLabel Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTDebugLabel::EXTDebugLabel()
{
    std::for_each_n(__EXTDebugLabelEntryPoints, __EXTDebugLabelEntryCount,
                    ClearEntryPoint(this));
}

bool EXTDebugLabel::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_debug_label");
}

void EXTDebugLabel::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTDebugLabelEntryPoints, __EXTDebugLabelEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTDebugMarker Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTDebugMarker::EXTDebugMarker()
{
    std::for_each_n(__EXTDebugMarkerEntryPoints, __EXTDebugMarkerEntryCount,
                    ClearEntryPoint(this));
}

bool EXTDebugMarker::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_debug_marker");
}

void EXTDebugMarker::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTDebugMarkerEntryPoints, __EXTDebugMarkerEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTDepthBoundsTest Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTDepthBoundsTest::EXTDepthBoundsTest()
{
    std::for_each_n(__EXTDepthBoundsTestEntryPoints, __EXTDepthBoundsTestEntryCount,
                    ClearEntryPoint(this));
}

bool EXTDepthBoundsTest::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_depth_bounds_test");
}

void EXTDepthBoundsTest::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTDepthBoundsTestEntryPoints, __EXTDepthBoundsTestEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTDirectStateAccess Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTDirectStateAccess::EXTDirectStateAccess()
{
    std::for_each_n(__EXTDirectStateAccessEntryPoints, __EXTDirectStateAccessEntryCount,
                    ClearEntryPoint(this));
}

bool EXTDirectStateAccess::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_direct_state_access");
}

void EXTDirectStateAccess::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTDirectStateAccessEntryPoints, __EXTDirectStateAccessEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTDrawBUFFERS2 Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTDrawBUFFERS2::EXTDrawBUFFERS2()
{
    std::for_each_n(__EXTDrawBUFFERS2EntryPoints, __EXTDrawBUFFERS2EntryCount,
                    ClearEntryPoint(this));
}

bool EXTDrawBUFFERS2::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_draw_buffers2");
}

void EXTDrawBUFFERS2::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTDrawBUFFERS2EntryPoints, __EXTDrawBUFFERS2EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTDrawInstanced Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTDrawInstanced::EXTDrawInstanced()
{
    std::for_each_n(__EXTDrawInstancedEntryPoints, __EXTDrawInstancedEntryCount,
                    ClearEntryPoint(this));
}

bool EXTDrawInstanced::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_draw_instanced");
}

void EXTDrawInstanced::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTDrawInstancedEntryPoints, __EXTDrawInstancedEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTDrawRangeElements Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTDrawRangeElements::EXTDrawRangeElements()
{
    std::for_each_n(__EXTDrawRangeElementsEntryPoints, __EXTDrawRangeElementsEntryCount,
                    ClearEntryPoint(this));
}

bool EXTDrawRangeElements::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_draw_range_elements");
}

void EXTDrawRangeElements::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTDrawRangeElementsEntryPoints, __EXTDrawRangeElementsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTExternalBuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTExternalBuffer::EXTExternalBuffer()
{
    std::for_each_n(__EXTExternalBufferEntryPoints, __EXTExternalBufferEntryCount,
                    ClearEntryPoint(this));
}

bool EXTExternalBuffer::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_external_buffer");
}

void EXTExternalBuffer::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTExternalBufferEntryPoints, __EXTExternalBufferEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTFogCoord Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTFogCoord::EXTFogCoord()
{
    std::for_each_n(__EXTFogCoordEntryPoints, __EXTFogCoordEntryCount,
                    ClearEntryPoint(this));
}

bool EXTFogCoord::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_fog_coord");
}

void EXTFogCoord::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTFogCoordEntryPoints, __EXTFogCoordEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTFramebufferBlit Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTFramebufferBlit::EXTFramebufferBlit()
{
    std::for_each_n(__EXTFramebufferBlitEntryPoints, __EXTFramebufferBlitEntryCount,
                    ClearEntryPoint(this));
}

bool EXTFramebufferBlit::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_framebuffer_blit");
}

void EXTFramebufferBlit::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTFramebufferBlitEntryPoints, __EXTFramebufferBlitEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTFramebufferMultisample Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTFramebufferMultisample::EXTFramebufferMultisample()
{
    std::for_each_n(__EXTFramebufferMultisampleEntryPoints, __EXTFramebufferMultisampleEntryCount,
                    ClearEntryPoint(this));
}

bool EXTFramebufferMultisample::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_framebuffer_multisample");
}

void EXTFramebufferMultisample::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTFramebufferMultisampleEntryPoints, __EXTFramebufferMultisampleEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTFramebufferObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTFramebufferObject::EXTFramebufferObject()
{
    std::for_each_n(__EXTFramebufferObjectEntryPoints, __EXTFramebufferObjectEntryCount,
                    ClearEntryPoint(this));
}

bool EXTFramebufferObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_framebuffer_object");
}

void EXTFramebufferObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTFramebufferObjectEntryPoints, __EXTFramebufferObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTGeometrySHADER4 Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTGeometrySHADER4::EXTGeometrySHADER4()
{
    std::for_each_n(__EXTGeometrySHADER4EntryPoints, __EXTGeometrySHADER4EntryCount,
                    ClearEntryPoint(this));
}

bool EXTGeometrySHADER4::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_geometry_shader4");
}

void EXTGeometrySHADER4::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTGeometrySHADER4EntryPoints, __EXTGeometrySHADER4EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTGPUProgramParameters Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTGPUProgramParameters::EXTGPUProgramParameters()
{
    std::for_each_n(__EXTGPUProgramParametersEntryPoints, __EXTGPUProgramParametersEntryCount,
                    ClearEntryPoint(this));
}

bool EXTGPUProgramParameters::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_gpu_program_parameters");
}

void EXTGPUProgramParameters::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTGPUProgramParametersEntryPoints, __EXTGPUProgramParametersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTGPUSHADER4 Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTGPUSHADER4::EXTGPUSHADER4()
{
    std::for_each_n(__EXTGPUSHADER4EntryPoints, __EXTGPUSHADER4EntryCount,
                    ClearEntryPoint(this));
}

bool EXTGPUSHADER4::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_gpu_shader4");
}

void EXTGPUSHADER4::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTGPUSHADER4EntryPoints, __EXTGPUSHADER4EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTHistogram Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTHistogram::EXTHistogram()
{
    std::for_each_n(__EXTHistogramEntryPoints, __EXTHistogramEntryCount,
                    ClearEntryPoint(this));
}

bool EXTHistogram::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_histogram");
}

void EXTHistogram::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTHistogramEntryPoints, __EXTHistogramEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTIndexFunc Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTIndexFunc::EXTIndexFunc()
{
    std::for_each_n(__EXTIndexFuncEntryPoints, __EXTIndexFuncEntryCount,
                    ClearEntryPoint(this));
}

bool EXTIndexFunc::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_index_func");
}

void EXTIndexFunc::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTIndexFuncEntryPoints, __EXTIndexFuncEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTIndexMaterial Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTIndexMaterial::EXTIndexMaterial()
{
    std::for_each_n(__EXTIndexMaterialEntryPoints, __EXTIndexMaterialEntryCount,
                    ClearEntryPoint(this));
}

bool EXTIndexMaterial::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_index_material");
}

void EXTIndexMaterial::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTIndexMaterialEntryPoints, __EXTIndexMaterialEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTLightTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTLightTexture::EXTLightTexture()
{
    std::for_each_n(__EXTLightTextureEntryPoints, __EXTLightTextureEntryCount,
                    ClearEntryPoint(this));
}

bool EXTLightTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_light_texture");
}

void EXTLightTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTLightTextureEntryPoints, __EXTLightTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTMemoryObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTMemoryObject::EXTMemoryObject()
{
    std::for_each_n(__EXTMemoryObjectEntryPoints, __EXTMemoryObjectEntryCount,
                    ClearEntryPoint(this));
}

bool EXTMemoryObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_memory_object");
}

void EXTMemoryObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTMemoryObjectEntryPoints, __EXTMemoryObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTMemoryObjectFd Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTMemoryObjectFd::EXTMemoryObjectFd()
{
    std::for_each_n(__EXTMemoryObjectFdEntryPoints, __EXTMemoryObjectFdEntryCount,
                    ClearEntryPoint(this));
}

bool EXTMemoryObjectFd::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_memory_object_fd");
}

void EXTMemoryObjectFd::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTMemoryObjectFdEntryPoints, __EXTMemoryObjectFdEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTMemoryObjectWIN32 Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTMemoryObjectWIN32::EXTMemoryObjectWIN32()
{
    std::for_each_n(__EXTMemoryObjectWIN32EntryPoints, __EXTMemoryObjectWIN32EntryCount,
                    ClearEntryPoint(this));
}

bool EXTMemoryObjectWIN32::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_memory_object_win32");
}

void EXTMemoryObjectWIN32::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTMemoryObjectWIN32EntryPoints, __EXTMemoryObjectWIN32EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTMultiDrawArrays Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTMultiDrawArrays::EXTMultiDrawArrays()
{
    std::for_each_n(__EXTMultiDrawArraysEntryPoints, __EXTMultiDrawArraysEntryCount,
                    ClearEntryPoint(this));
}

bool EXTMultiDrawArrays::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_multi_draw_arrays");
}

void EXTMultiDrawArrays::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTMultiDrawArraysEntryPoints, __EXTMultiDrawArraysEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTMultisample Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTMultisample::EXTMultisample()
{
    std::for_each_n(__EXTMultisampleEntryPoints, __EXTMultisampleEntryCount,
                    ClearEntryPoint(this));
}

bool EXTMultisample::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_multisample");
}

void EXTMultisample::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTMultisampleEntryPoints, __EXTMultisampleEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTPalettedTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTPalettedTexture::EXTPalettedTexture()
{
    std::for_each_n(__EXTPalettedTextureEntryPoints, __EXTPalettedTextureEntryCount,
                    ClearEntryPoint(this));
}

bool EXTPalettedTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_paletted_texture");
}

void EXTPalettedTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTPalettedTextureEntryPoints, __EXTPalettedTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTPixelTransform Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTPixelTransform::EXTPixelTransform()
{
    std::for_each_n(__EXTPixelTransformEntryPoints, __EXTPixelTransformEntryCount,
                    ClearEntryPoint(this));
}

bool EXTPixelTransform::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_pixel_transform");
}

void EXTPixelTransform::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTPixelTransformEntryPoints, __EXTPixelTransformEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTPointParameters Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTPointParameters::EXTPointParameters()
{
    std::for_each_n(__EXTPointParametersEntryPoints, __EXTPointParametersEntryCount,
                    ClearEntryPoint(this));
}

bool EXTPointParameters::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_point_parameters");
}

void EXTPointParameters::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTPointParametersEntryPoints, __EXTPointParametersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTPolygonOffset Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTPolygonOffset::EXTPolygonOffset()
{
    std::for_each_n(__EXTPolygonOffsetEntryPoints, __EXTPolygonOffsetEntryCount,
                    ClearEntryPoint(this));
}

bool EXTPolygonOffset::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_polygon_offset");
}

void EXTPolygonOffset::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTPolygonOffsetEntryPoints, __EXTPolygonOffsetEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTPolygonOffsetClamp Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTPolygonOffsetClamp::EXTPolygonOffsetClamp()
{
    std::for_each_n(__EXTPolygonOffsetClampEntryPoints, __EXTPolygonOffsetClampEntryCount,
                    ClearEntryPoint(this));
}

bool EXTPolygonOffsetClamp::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_polygon_offset_clamp");
}

void EXTPolygonOffsetClamp::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTPolygonOffsetClampEntryPoints, __EXTPolygonOffsetClampEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTProvokingVertex Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTProvokingVertex::EXTProvokingVertex()
{
    std::for_each_n(__EXTProvokingVertexEntryPoints, __EXTProvokingVertexEntryCount,
                    ClearEntryPoint(this));
}

bool EXTProvokingVertex::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_provoking_vertex");
}

void EXTProvokingVertex::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTProvokingVertexEntryPoints, __EXTProvokingVertexEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTRasterMultisample Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTRasterMultisample::EXTRasterMultisample()
{
    std::for_each_n(__EXTRasterMultisampleEntryPoints, __EXTRasterMultisampleEntryCount,
                    ClearEntryPoint(this));
}

bool EXTRasterMultisample::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_raster_multisample");
}

void EXTRasterMultisample::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTRasterMultisampleEntryPoints, __EXTRasterMultisampleEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTSecondaryColor Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTSecondaryColor::EXTSecondaryColor()
{
    std::for_each_n(__EXTSecondaryColorEntryPoints, __EXTSecondaryColorEntryCount,
                    ClearEntryPoint(this));
}

bool EXTSecondaryColor::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_secondary_color");
}

void EXTSecondaryColor::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTSecondaryColorEntryPoints, __EXTSecondaryColorEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTSemaphore Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTSemaphore::EXTSemaphore()
{
    std::for_each_n(__EXTSemaphoreEntryPoints, __EXTSemaphoreEntryCount,
                    ClearEntryPoint(this));
}

bool EXTSemaphore::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_semaphore");
}

void EXTSemaphore::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTSemaphoreEntryPoints, __EXTSemaphoreEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTSemaphoreFd Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTSemaphoreFd::EXTSemaphoreFd()
{
    std::for_each_n(__EXTSemaphoreFdEntryPoints, __EXTSemaphoreFdEntryCount,
                    ClearEntryPoint(this));
}

bool EXTSemaphoreFd::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_semaphore_fd");
}

void EXTSemaphoreFd::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTSemaphoreFdEntryPoints, __EXTSemaphoreFdEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTSemaphoreWIN32 Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTSemaphoreWIN32::EXTSemaphoreWIN32()
{
    std::for_each_n(__EXTSemaphoreWIN32EntryPoints, __EXTSemaphoreWIN32EntryCount,
                    ClearEntryPoint(this));
}

bool EXTSemaphoreWIN32::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_semaphore_win32");
}

void EXTSemaphoreWIN32::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTSemaphoreWIN32EntryPoints, __EXTSemaphoreWIN32EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTSeparateShaderObjects Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTSeparateShaderObjects::EXTSeparateShaderObjects()
{
    std::for_each_n(__EXTSeparateShaderObjectsEntryPoints, __EXTSeparateShaderObjectsEntryCount,
                    ClearEntryPoint(this));
}

bool EXTSeparateShaderObjects::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_separate_shader_objects");
}

void EXTSeparateShaderObjects::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTSeparateShaderObjectsEntryPoints, __EXTSeparateShaderObjectsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTShaderFramebufferFetchNonCoherent Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTShaderFramebufferFetchNonCoherent::EXTShaderFramebufferFetchNonCoherent()
{
    std::for_each_n(__EXTShaderFramebufferFetchNonCoherentEntryPoints, __EXTShaderFramebufferFetchNonCoherentEntryCount,
                    ClearEntryPoint(this));
}

bool EXTShaderFramebufferFetchNonCoherent::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_shader_framebuffer_fetch_non_coherent");
}

void EXTShaderFramebufferFetchNonCoherent::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTShaderFramebufferFetchNonCoherentEntryPoints, __EXTShaderFramebufferFetchNonCoherentEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTShaderImageLoadStore Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTShaderImageLoadStore::EXTShaderImageLoadStore()
{
    std::for_each_n(__EXTShaderImageLoadStoreEntryPoints, __EXTShaderImageLoadStoreEntryCount,
                    ClearEntryPoint(this));
}

bool EXTShaderImageLoadStore::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_shader_image_load_store");
}

void EXTShaderImageLoadStore::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTShaderImageLoadStoreEntryPoints, __EXTShaderImageLoadStoreEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTStencilClearTag Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTStencilClearTag::EXTStencilClearTag()
{
    std::for_each_n(__EXTStencilClearTagEntryPoints, __EXTStencilClearTagEntryCount,
                    ClearEntryPoint(this));
}

bool EXTStencilClearTag::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_stencil_clear_tag");
}

void EXTStencilClearTag::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTStencilClearTagEntryPoints, __EXTStencilClearTagEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTStencilTwoSide Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTStencilTwoSide::EXTStencilTwoSide()
{
    std::for_each_n(__EXTStencilTwoSideEntryPoints, __EXTStencilTwoSideEntryCount,
                    ClearEntryPoint(this));
}

bool EXTStencilTwoSide::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_stencil_two_side");
}

void EXTStencilTwoSide::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTStencilTwoSideEntryPoints, __EXTStencilTwoSideEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTSubtexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTSubtexture::EXTSubtexture()
{
    std::for_each_n(__EXTSubtextureEntryPoints, __EXTSubtextureEntryCount,
                    ClearEntryPoint(this));
}

bool EXTSubtexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_subtexture");
}

void EXTSubtexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTSubtextureEntryPoints, __EXTSubtextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTEXTURE3D Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTEXTURE3D::EXTTEXTURE3D()
{
    std::for_each_n(__EXTTEXTURE3DEntryPoints, __EXTTEXTURE3DEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTEXTURE3D::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_texture3D");
}

void EXTTEXTURE3D::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTEXTURE3DEntryPoints, __EXTTEXTURE3DEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTextureArray Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTextureArray::EXTTextureArray()
{
    std::for_each_n(__EXTTextureArrayEntryPoints, __EXTTextureArrayEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTextureArray::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_texture_array");
}

void EXTTextureArray::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTextureArrayEntryPoints, __EXTTextureArrayEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTextureBufferObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTextureBufferObject::EXTTextureBufferObject()
{
    std::for_each_n(__EXTTextureBufferObjectEntryPoints, __EXTTextureBufferObjectEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTextureBufferObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_texture_buffer_object");
}

void EXTTextureBufferObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTextureBufferObjectEntryPoints, __EXTTextureBufferObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTextureInteger Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTextureInteger::EXTTextureInteger()
{
    std::for_each_n(__EXTTextureIntegerEntryPoints, __EXTTextureIntegerEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTextureInteger::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_texture_integer");
}

void EXTTextureInteger::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTextureIntegerEntryPoints, __EXTTextureIntegerEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTextureObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTextureObject::EXTTextureObject()
{
    std::for_each_n(__EXTTextureObjectEntryPoints, __EXTTextureObjectEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTextureObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_texture_object");
}

void EXTTextureObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTextureObjectEntryPoints, __EXTTextureObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTexturePerturbNormal Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTexturePerturbNormal::EXTTexturePerturbNormal()
{
    std::for_each_n(__EXTTexturePerturbNormalEntryPoints, __EXTTexturePerturbNormalEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTexturePerturbNormal::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_texture_perturb_normal");
}

void EXTTexturePerturbNormal::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTexturePerturbNormalEntryPoints, __EXTTexturePerturbNormalEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTextureStorage Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTextureStorage::EXTTextureStorage()
{
    std::for_each_n(__EXTTextureStorageEntryPoints, __EXTTextureStorageEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTextureStorage::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_texture_storage");
}

void EXTTextureStorage::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTextureStorageEntryPoints, __EXTTextureStorageEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTimerQuery Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTimerQuery::EXTTimerQuery()
{
    std::for_each_n(__EXTTimerQueryEntryPoints, __EXTTimerQueryEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTimerQuery::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_timer_query");
}

void EXTTimerQuery::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTimerQueryEntryPoints, __EXTTimerQueryEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTTransformFeedback Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTTransformFeedback::EXTTransformFeedback()
{
    std::for_each_n(__EXTTransformFeedbackEntryPoints, __EXTTransformFeedbackEntryCount,
                    ClearEntryPoint(this));
}

bool EXTTransformFeedback::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_transform_feedback");
}

void EXTTransformFeedback::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTTransformFeedbackEntryPoints, __EXTTransformFeedbackEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTVertexArray Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTVertexArray::EXTVertexArray()
{
    std::for_each_n(__EXTVertexArrayEntryPoints, __EXTVertexArrayEntryCount,
                    ClearEntryPoint(this));
}

bool EXTVertexArray::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_vertex_array");
}

void EXTVertexArray::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTVertexArrayEntryPoints, __EXTVertexArrayEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTVertexAttrib64BIT Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTVertexAttrib64BIT::EXTVertexAttrib64BIT()
{
    std::for_each_n(__EXTVertexAttrib64BITEntryPoints, __EXTVertexAttrib64BITEntryCount,
                    ClearEntryPoint(this));
}

bool EXTVertexAttrib64BIT::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_vertex_attrib_64bit");
}

void EXTVertexAttrib64BIT::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTVertexAttrib64BITEntryPoints, __EXTVertexAttrib64BITEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTVertexShader Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTVertexShader::EXTVertexShader()
{
    std::for_each_n(__EXTVertexShaderEntryPoints, __EXTVertexShaderEntryCount,
                    ClearEntryPoint(this));
}

bool EXTVertexShader::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_vertex_shader");
}

void EXTVertexShader::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTVertexShaderEntryPoints, __EXTVertexShaderEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTVertexWeighting Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTVertexWeighting::EXTVertexWeighting()
{
    std::for_each_n(__EXTVertexWeightingEntryPoints, __EXTVertexWeightingEntryCount,
                    ClearEntryPoint(this));
}

bool EXTVertexWeighting::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_vertex_weighting");
}

void EXTVertexWeighting::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTVertexWeightingEntryPoints, __EXTVertexWeightingEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTWIN32KeyedMutex Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTWIN32KeyedMutex::EXTWIN32KeyedMutex()
{
    std::for_each_n(__EXTWIN32KeyedMutexEntryPoints, __EXTWIN32KeyedMutexEntryCount,
                    ClearEntryPoint(this));
}

bool EXTWIN32KeyedMutex::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_win32_keyed_mutex");
}

void EXTWIN32KeyedMutex::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTWIN32KeyedMutexEntryPoints, __EXTWIN32KeyedMutexEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTWindowRectangles Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTWindowRectangles::EXTWindowRectangles()
{
    std::for_each_n(__EXTWindowRectanglesEntryPoints, __EXTWindowRectanglesEntryCount,
                    ClearEntryPoint(this));
}

bool EXTWindowRectangles::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_window_rectangles");
}

void EXTWindowRectangles::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTWindowRectanglesEntryPoints, __EXTWindowRectanglesEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTX11SyncObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTX11SyncObject::EXTX11SyncObject()
{
    std::for_each_n(__EXTX11SyncObjectEntryPoints, __EXTX11SyncObjectEntryCount,
                    ClearEntryPoint(this));
}

bool EXTX11SyncObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_EXT_x11_sync_object");
}

void EXTX11SyncObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__EXTX11SyncObjectEntryPoints, __EXTX11SyncObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// GremedyFrameTerminator Member Definitions
////////////////////////////////////////////////////////////////////////////////
GremedyFrameTerminator::GremedyFrameTerminator()
{
    std::for_each_n(__GremedyFrameTerminatorEntryPoints, __GremedyFrameTerminatorEntryCount,
                    ClearEntryPoint(this));
}

bool GremedyFrameTerminator::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_GREMEDY_frame_terminator");
}

void GremedyFrameTerminator::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__GremedyFrameTerminatorEntryPoints, __GremedyFrameTerminatorEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// GremedyStringMarker Member Definitions
////////////////////////////////////////////////////////////////////////////////
GremedyStringMarker::GremedyStringMarker()
{
    std::for_each_n(__GremedyStringMarkerEntryPoints, __GremedyStringMarkerEntryCount,
                    ClearEntryPoint(this));
}

bool GremedyStringMarker::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_GREMEDY_string_marker");
}

void GremedyStringMarker::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__GremedyStringMarkerEntryPoints, __GremedyStringMarkerEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// HPImageTransform Member Definitions
////////////////////////////////////////////////////////////////////////////////
HPImageTransform::HPImageTransform()
{
    std::for_each_n(__HPImageTransformEntryPoints, __HPImageTransformEntryCount,
                    ClearEntryPoint(this));
}

bool HPImageTransform::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_HP_image_transform");
}

void HPImageTransform::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__HPImageTransformEntryPoints, __HPImageTransformEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// IBMMultimodeDrawArrays Member Definitions
////////////////////////////////////////////////////////////////////////////////
IBMMultimodeDrawArrays::IBMMultimodeDrawArrays()
{
    std::for_each_n(__IBMMultimodeDrawArraysEntryPoints, __IBMMultimodeDrawArraysEntryCount,
                    ClearEntryPoint(this));
}

bool IBMMultimodeDrawArrays::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_IBM_multimode_draw_arrays");
}

void IBMMultimodeDrawArrays::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__IBMMultimodeDrawArraysEntryPoints, __IBMMultimodeDrawArraysEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// IBMStaticData Member Definitions
////////////////////////////////////////////////////////////////////////////////
IBMStaticData::IBMStaticData()
{
    std::for_each_n(__IBMStaticDataEntryPoints, __IBMStaticDataEntryCount,
                    ClearEntryPoint(this));
}

bool IBMStaticData::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_IBM_static_data");
}

void IBMStaticData::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__IBMStaticDataEntryPoints, __IBMStaticDataEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// IBMVertexArrayLists Member Definitions
////////////////////////////////////////////////////////////////////////////////
IBMVertexArrayLists::IBMVertexArrayLists()
{
    std::for_each_n(__IBMVertexArrayListsEntryPoints, __IBMVertexArrayListsEntryCount,
                    ClearEntryPoint(this));
}

bool IBMVertexArrayLists::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_IBM_vertex_array_lists");
}

void IBMVertexArrayLists::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__IBMVertexArrayListsEntryPoints, __IBMVertexArrayListsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// IngrBlendFuncSeparate Member Definitions
////////////////////////////////////////////////////////////////////////////////
IngrBlendFuncSeparate::IngrBlendFuncSeparate()
{
    std::for_each_n(__IngrBlendFuncSeparateEntryPoints, __IngrBlendFuncSeparateEntryCount,
                    ClearEntryPoint(this));
}

bool IngrBlendFuncSeparate::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_INGR_blend_func_separate");
}

void IngrBlendFuncSeparate::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__IngrBlendFuncSeparateEntryPoints, __IngrBlendFuncSeparateEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// INTELFramebufferCmaa Member Definitions
////////////////////////////////////////////////////////////////////////////////
INTELFramebufferCmaa::INTELFramebufferCmaa()
{
    std::for_each_n(__INTELFramebufferCmaaEntryPoints, __INTELFramebufferCmaaEntryCount,
                    ClearEntryPoint(this));
}

bool INTELFramebufferCmaa::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_INTEL_framebuffer_CMAA");
}

void INTELFramebufferCmaa::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__INTELFramebufferCmaaEntryPoints, __INTELFramebufferCmaaEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// INTELMapTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
INTELMapTexture::INTELMapTexture()
{
    std::for_each_n(__INTELMapTextureEntryPoints, __INTELMapTextureEntryCount,
                    ClearEntryPoint(this));
}

bool INTELMapTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_INTEL_map_texture");
}

void INTELMapTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__INTELMapTextureEntryPoints, __INTELMapTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// INTELParallelArrays Member Definitions
////////////////////////////////////////////////////////////////////////////////
INTELParallelArrays::INTELParallelArrays()
{
    std::for_each_n(__INTELParallelArraysEntryPoints, __INTELParallelArraysEntryCount,
                    ClearEntryPoint(this));
}

bool INTELParallelArrays::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_INTEL_parallel_arrays");
}

void INTELParallelArrays::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__INTELParallelArraysEntryPoints, __INTELParallelArraysEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// INTELPerformanceQuery Member Definitions
////////////////////////////////////////////////////////////////////////////////
INTELPerformanceQuery::INTELPerformanceQuery()
{
    std::for_each_n(__INTELPerformanceQueryEntryPoints, __INTELPerformanceQueryEntryCount,
                    ClearEntryPoint(this));
}

bool INTELPerformanceQuery::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_INTEL_performance_query");
}

void INTELPerformanceQuery::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__INTELPerformanceQueryEntryPoints, __INTELPerformanceQueryEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// KHRBlendEquationAdvanced Member Definitions
////////////////////////////////////////////////////////////////////////////////
KHRBlendEquationAdvanced::KHRBlendEquationAdvanced()
{
    std::for_each_n(__KHRBlendEquationAdvancedEntryPoints, __KHRBlendEquationAdvancedEntryCount,
                    ClearEntryPoint(this));
}

bool KHRBlendEquationAdvanced::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_KHR_blend_equation_advanced");
}

void KHRBlendEquationAdvanced::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__KHRBlendEquationAdvancedEntryPoints, __KHRBlendEquationAdvancedEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// KHRParallelShaderCompile Member Definitions
////////////////////////////////////////////////////////////////////////////////
KHRParallelShaderCompile::KHRParallelShaderCompile()
{
    std::for_each_n(__KHRParallelShaderCompileEntryPoints, __KHRParallelShaderCompileEntryCount,
                    ClearEntryPoint(this));
}

bool KHRParallelShaderCompile::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_KHR_parallel_shader_compile");
}

void KHRParallelShaderCompile::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__KHRParallelShaderCompileEntryPoints, __KHRParallelShaderCompileEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// MESAFramebufferFlipY Member Definitions
////////////////////////////////////////////////////////////////////////////////
MESAFramebufferFlipY::MESAFramebufferFlipY()
{
    std::for_each_n(__MESAFramebufferFlipYEntryPoints, __MESAFramebufferFlipYEntryCount,
                    ClearEntryPoint(this));
}

bool MESAFramebufferFlipY::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_MESA_framebuffer_flip_y");
}

void MESAFramebufferFlipY::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__MESAFramebufferFlipYEntryPoints, __MESAFramebufferFlipYEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// MESAResizeBuffers Member Definitions
////////////////////////////////////////////////////////////////////////////////
MESAResizeBuffers::MESAResizeBuffers()
{
    std::for_each_n(__MESAResizeBuffersEntryPoints, __MESAResizeBuffersEntryCount,
                    ClearEntryPoint(this));
}

bool MESAResizeBuffers::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_MESA_resize_buffers");
}

void MESAResizeBuffers::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__MESAResizeBuffersEntryPoints, __MESAResizeBuffersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// MESAWindowPos Member Definitions
////////////////////////////////////////////////////////////////////////////////
MESAWindowPos::MESAWindowPos()
{
    std::for_each_n(__MESAWindowPosEntryPoints, __MESAWindowPosEntryCount,
                    ClearEntryPoint(this));
}

bool MESAWindowPos::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_MESA_window_pos");
}

void MESAWindowPos::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__MESAWindowPosEntryPoints, __MESAWindowPosEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVXConditionalRender Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVXConditionalRender::NVXConditionalRender()
{
    std::for_each_n(__NVXConditionalRenderEntryPoints, __NVXConditionalRenderEntryCount,
                    ClearEntryPoint(this));
}

bool NVXConditionalRender::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NVX_conditional_render");
}

void NVXConditionalRender::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVXConditionalRenderEntryPoints, __NVXConditionalRenderEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVXGPUMULTICAST2 Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVXGPUMULTICAST2::NVXGPUMULTICAST2()
{
    std::for_each_n(__NVXGPUMULTICAST2EntryPoints, __NVXGPUMULTICAST2EntryCount,
                    ClearEntryPoint(this));
}

bool NVXGPUMULTICAST2::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NVX_gpu_multicast2");
}

void NVXGPUMULTICAST2::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVXGPUMULTICAST2EntryPoints, __NVXGPUMULTICAST2EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVXLinkedGPUMulticast Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVXLinkedGPUMulticast::NVXLinkedGPUMulticast()
{
    std::for_each_n(__NVXLinkedGPUMulticastEntryPoints, __NVXLinkedGPUMulticastEntryCount,
                    ClearEntryPoint(this));
}

bool NVXLinkedGPUMulticast::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NVX_linked_gpu_multicast");
}

void NVXLinkedGPUMulticast::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVXLinkedGPUMulticastEntryPoints, __NVXLinkedGPUMulticastEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVXProgressFence Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVXProgressFence::NVXProgressFence()
{
    std::for_each_n(__NVXProgressFenceEntryPoints, __NVXProgressFenceEntryCount,
                    ClearEntryPoint(this));
}

bool NVXProgressFence::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NVX_progress_fence");
}

void NVXProgressFence::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVXProgressFenceEntryPoints, __NVXProgressFenceEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVAlphaToCoverageDitherControl Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVAlphaToCoverageDitherControl::NVAlphaToCoverageDitherControl()
{
    std::for_each_n(__NVAlphaToCoverageDitherControlEntryPoints, __NVAlphaToCoverageDitherControlEntryCount,
                    ClearEntryPoint(this));
}

bool NVAlphaToCoverageDitherControl::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_alpha_to_coverage_dither_control");
}

void NVAlphaToCoverageDitherControl::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVAlphaToCoverageDitherControlEntryPoints, __NVAlphaToCoverageDitherControlEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVBindlessMultiDrawIndirect Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVBindlessMultiDrawIndirect::NVBindlessMultiDrawIndirect()
{
    std::for_each_n(__NVBindlessMultiDrawIndirectEntryPoints, __NVBindlessMultiDrawIndirectEntryCount,
                    ClearEntryPoint(this));
}

bool NVBindlessMultiDrawIndirect::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_bindless_multi_draw_indirect");
}

void NVBindlessMultiDrawIndirect::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVBindlessMultiDrawIndirectEntryPoints, __NVBindlessMultiDrawIndirectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVBindlessMultiDrawIndirectCount Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVBindlessMultiDrawIndirectCount::NVBindlessMultiDrawIndirectCount()
{
    std::for_each_n(__NVBindlessMultiDrawIndirectCountEntryPoints, __NVBindlessMultiDrawIndirectCountEntryCount,
                    ClearEntryPoint(this));
}

bool NVBindlessMultiDrawIndirectCount::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_bindless_multi_draw_indirect_count");
}

void NVBindlessMultiDrawIndirectCount::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVBindlessMultiDrawIndirectCountEntryPoints, __NVBindlessMultiDrawIndirectCountEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVBindlessTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVBindlessTexture::NVBindlessTexture()
{
    std::for_each_n(__NVBindlessTextureEntryPoints, __NVBindlessTextureEntryCount,
                    ClearEntryPoint(this));
}

bool NVBindlessTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_bindless_texture");
}

void NVBindlessTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVBindlessTextureEntryPoints, __NVBindlessTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVBlendEquationAdvanced Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVBlendEquationAdvanced::NVBlendEquationAdvanced()
{
    std::for_each_n(__NVBlendEquationAdvancedEntryPoints, __NVBlendEquationAdvancedEntryCount,
                    ClearEntryPoint(this));
}

bool NVBlendEquationAdvanced::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_blend_equation_advanced");
}

void NVBlendEquationAdvanced::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVBlendEquationAdvancedEntryPoints, __NVBlendEquationAdvancedEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVClipSpaceWScaling Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVClipSpaceWScaling::NVClipSpaceWScaling()
{
    std::for_each_n(__NVClipSpaceWScalingEntryPoints, __NVClipSpaceWScalingEntryCount,
                    ClearEntryPoint(this));
}

bool NVClipSpaceWScaling::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_clip_space_w_scaling");
}

void NVClipSpaceWScaling::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVClipSpaceWScalingEntryPoints, __NVClipSpaceWScalingEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVCommandList Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVCommandList::NVCommandList()
{
    std::for_each_n(__NVCommandListEntryPoints, __NVCommandListEntryCount,
                    ClearEntryPoint(this));
}

bool NVCommandList::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_command_list");
}

void NVCommandList::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVCommandListEntryPoints, __NVCommandListEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVConditionalRender Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVConditionalRender::NVConditionalRender()
{
    std::for_each_n(__NVConditionalRenderEntryPoints, __NVConditionalRenderEntryCount,
                    ClearEntryPoint(this));
}

bool NVConditionalRender::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_conditional_render");
}

void NVConditionalRender::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVConditionalRenderEntryPoints, __NVConditionalRenderEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVConservativeRaster Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVConservativeRaster::NVConservativeRaster()
{
    std::for_each_n(__NVConservativeRasterEntryPoints, __NVConservativeRasterEntryCount,
                    ClearEntryPoint(this));
}

bool NVConservativeRaster::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_conservative_raster");
}

void NVConservativeRaster::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVConservativeRasterEntryPoints, __NVConservativeRasterEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVConservativeRasterDilate Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVConservativeRasterDilate::NVConservativeRasterDilate()
{
    std::for_each_n(__NVConservativeRasterDilateEntryPoints, __NVConservativeRasterDilateEntryCount,
                    ClearEntryPoint(this));
}

bool NVConservativeRasterDilate::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_conservative_raster_dilate");
}

void NVConservativeRasterDilate::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVConservativeRasterDilateEntryPoints, __NVConservativeRasterDilateEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVConservativeRasterPreSnapTriangles Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVConservativeRasterPreSnapTriangles::NVConservativeRasterPreSnapTriangles()
{
    std::for_each_n(__NVConservativeRasterPreSnapTrianglesEntryPoints, __NVConservativeRasterPreSnapTrianglesEntryCount,
                    ClearEntryPoint(this));
}

bool NVConservativeRasterPreSnapTriangles::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_conservative_raster_pre_snap_triangles");
}

void NVConservativeRasterPreSnapTriangles::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVConservativeRasterPreSnapTrianglesEntryPoints, __NVConservativeRasterPreSnapTrianglesEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVCopyImage Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVCopyImage::NVCopyImage()
{
    std::for_each_n(__NVCopyImageEntryPoints, __NVCopyImageEntryCount,
                    ClearEntryPoint(this));
}

bool NVCopyImage::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_copy_image");
}

void NVCopyImage::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVCopyImageEntryPoints, __NVCopyImageEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVDepthBufferFloat Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVDepthBufferFloat::NVDepthBufferFloat()
{
    std::for_each_n(__NVDepthBufferFloatEntryPoints, __NVDepthBufferFloatEntryCount,
                    ClearEntryPoint(this));
}

bool NVDepthBufferFloat::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_depth_buffer_float");
}

void NVDepthBufferFloat::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVDepthBufferFloatEntryPoints, __NVDepthBufferFloatEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVDrawTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVDrawTexture::NVDrawTexture()
{
    std::for_each_n(__NVDrawTextureEntryPoints, __NVDrawTextureEntryCount,
                    ClearEntryPoint(this));
}

bool NVDrawTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_draw_texture");
}

void NVDrawTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVDrawTextureEntryPoints, __NVDrawTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVDrawVulkanImage Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVDrawVulkanImage::NVDrawVulkanImage()
{
    std::for_each_n(__NVDrawVulkanImageEntryPoints, __NVDrawVulkanImageEntryCount,
                    ClearEntryPoint(this));
}

bool NVDrawVulkanImage::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_draw_vulkan_image");
}

void NVDrawVulkanImage::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVDrawVulkanImageEntryPoints, __NVDrawVulkanImageEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVEvaluators Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVEvaluators::NVEvaluators()
{
    std::for_each_n(__NVEvaluatorsEntryPoints, __NVEvaluatorsEntryCount,
                    ClearEntryPoint(this));
}

bool NVEvaluators::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_evaluators");
}

void NVEvaluators::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVEvaluatorsEntryPoints, __NVEvaluatorsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVExplicitMultisample Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVExplicitMultisample::NVExplicitMultisample()
{
    std::for_each_n(__NVExplicitMultisampleEntryPoints, __NVExplicitMultisampleEntryCount,
                    ClearEntryPoint(this));
}

bool NVExplicitMultisample::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_explicit_multisample");
}

void NVExplicitMultisample::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVExplicitMultisampleEntryPoints, __NVExplicitMultisampleEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVFence Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVFence::NVFence()
{
    std::for_each_n(__NVFenceEntryPoints, __NVFenceEntryCount,
                    ClearEntryPoint(this));
}

bool NVFence::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_fence");
}

void NVFence::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVFenceEntryPoints, __NVFenceEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVFragmentCoverageToColor Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVFragmentCoverageToColor::NVFragmentCoverageToColor()
{
    std::for_each_n(__NVFragmentCoverageToColorEntryPoints, __NVFragmentCoverageToColorEntryCount,
                    ClearEntryPoint(this));
}

bool NVFragmentCoverageToColor::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_fragment_coverage_to_color");
}

void NVFragmentCoverageToColor::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVFragmentCoverageToColorEntryPoints, __NVFragmentCoverageToColorEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVFragmentProgram Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVFragmentProgram::NVFragmentProgram()
{
    std::for_each_n(__NVFragmentProgramEntryPoints, __NVFragmentProgramEntryCount,
                    ClearEntryPoint(this));
}

bool NVFragmentProgram::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_fragment_program");
}

void NVFragmentProgram::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVFragmentProgramEntryPoints, __NVFragmentProgramEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVFramebufferMixedSamples Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVFramebufferMixedSamples::NVFramebufferMixedSamples()
{
    std::for_each_n(__NVFramebufferMixedSamplesEntryPoints, __NVFramebufferMixedSamplesEntryCount,
                    ClearEntryPoint(this));
}

bool NVFramebufferMixedSamples::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_framebuffer_mixed_samples");
}

void NVFramebufferMixedSamples::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVFramebufferMixedSamplesEntryPoints, __NVFramebufferMixedSamplesEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVFramebufferMultisampleCoverage Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVFramebufferMultisampleCoverage::NVFramebufferMultisampleCoverage()
{
    std::for_each_n(__NVFramebufferMultisampleCoverageEntryPoints, __NVFramebufferMultisampleCoverageEntryCount,
                    ClearEntryPoint(this));
}

bool NVFramebufferMultisampleCoverage::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_framebuffer_multisample_coverage");
}

void NVFramebufferMultisampleCoverage::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVFramebufferMultisampleCoverageEntryPoints, __NVFramebufferMultisampleCoverageEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVGeometryPROGRAM4 Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVGeometryPROGRAM4::NVGeometryPROGRAM4()
{
    std::for_each_n(__NVGeometryPROGRAM4EntryPoints, __NVGeometryPROGRAM4EntryCount,
                    ClearEntryPoint(this));
}

bool NVGeometryPROGRAM4::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_geometry_program4");
}

void NVGeometryPROGRAM4::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVGeometryPROGRAM4EntryPoints, __NVGeometryPROGRAM4EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVGPUMulticast Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVGPUMulticast::NVGPUMulticast()
{
    std::for_each_n(__NVGPUMulticastEntryPoints, __NVGPUMulticastEntryCount,
                    ClearEntryPoint(this));
}

bool NVGPUMulticast::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_gpu_multicast");
}

void NVGPUMulticast::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVGPUMulticastEntryPoints, __NVGPUMulticastEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVGPUPROGRAM4 Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVGPUPROGRAM4::NVGPUPROGRAM4()
{
    std::for_each_n(__NVGPUPROGRAM4EntryPoints, __NVGPUPROGRAM4EntryCount,
                    ClearEntryPoint(this));
}

bool NVGPUPROGRAM4::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_gpu_program4");
}

void NVGPUPROGRAM4::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVGPUPROGRAM4EntryPoints, __NVGPUPROGRAM4EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVGPUPROGRAM5 Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVGPUPROGRAM5::NVGPUPROGRAM5()
{
    std::for_each_n(__NVGPUPROGRAM5EntryPoints, __NVGPUPROGRAM5EntryCount,
                    ClearEntryPoint(this));
}

bool NVGPUPROGRAM5::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_gpu_program5");
}

void NVGPUPROGRAM5::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVGPUPROGRAM5EntryPoints, __NVGPUPROGRAM5EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVHalfFloat Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVHalfFloat::NVHalfFloat()
{
    std::for_each_n(__NVHalfFloatEntryPoints, __NVHalfFloatEntryCount,
                    ClearEntryPoint(this));
}

bool NVHalfFloat::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_half_float");
}

void NVHalfFloat::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVHalfFloatEntryPoints, __NVHalfFloatEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVInternalformatSampleQuery Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVInternalformatSampleQuery::NVInternalformatSampleQuery()
{
    std::for_each_n(__NVInternalformatSampleQueryEntryPoints, __NVInternalformatSampleQueryEntryCount,
                    ClearEntryPoint(this));
}

bool NVInternalformatSampleQuery::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_internalformat_sample_query");
}

void NVInternalformatSampleQuery::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVInternalformatSampleQueryEntryPoints, __NVInternalformatSampleQueryEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVMemoryAttachment Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVMemoryAttachment::NVMemoryAttachment()
{
    std::for_each_n(__NVMemoryAttachmentEntryPoints, __NVMemoryAttachmentEntryCount,
                    ClearEntryPoint(this));
}

bool NVMemoryAttachment::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_memory_attachment");
}

void NVMemoryAttachment::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVMemoryAttachmentEntryPoints, __NVMemoryAttachmentEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVMemoryObjectSparse Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVMemoryObjectSparse::NVMemoryObjectSparse()
{
    std::for_each_n(__NVMemoryObjectSparseEntryPoints, __NVMemoryObjectSparseEntryCount,
                    ClearEntryPoint(this));
}

bool NVMemoryObjectSparse::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_memory_object_sparse");
}

void NVMemoryObjectSparse::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVMemoryObjectSparseEntryPoints, __NVMemoryObjectSparseEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVMeshShader Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVMeshShader::NVMeshShader()
{
    std::for_each_n(__NVMeshShaderEntryPoints, __NVMeshShaderEntryCount,
                    ClearEntryPoint(this));
}

bool NVMeshShader::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_mesh_shader");
}

void NVMeshShader::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVMeshShaderEntryPoints, __NVMeshShaderEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVOcclusionQuery Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVOcclusionQuery::NVOcclusionQuery()
{
    std::for_each_n(__NVOcclusionQueryEntryPoints, __NVOcclusionQueryEntryCount,
                    ClearEntryPoint(this));
}

bool NVOcclusionQuery::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_occlusion_query");
}

void NVOcclusionQuery::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVOcclusionQueryEntryPoints, __NVOcclusionQueryEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVParameterBufferObject Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVParameterBufferObject::NVParameterBufferObject()
{
    std::for_each_n(__NVParameterBufferObjectEntryPoints, __NVParameterBufferObjectEntryCount,
                    ClearEntryPoint(this));
}

bool NVParameterBufferObject::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_parameter_buffer_object");
}

void NVParameterBufferObject::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVParameterBufferObjectEntryPoints, __NVParameterBufferObjectEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVPathRendering Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVPathRendering::NVPathRendering()
{
    std::for_each_n(__NVPathRenderingEntryPoints, __NVPathRenderingEntryCount,
                    ClearEntryPoint(this));
}

bool NVPathRendering::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_path_rendering");
}

void NVPathRendering::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVPathRenderingEntryPoints, __NVPathRenderingEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVPixelDataRange Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVPixelDataRange::NVPixelDataRange()
{
    std::for_each_n(__NVPixelDataRangeEntryPoints, __NVPixelDataRangeEntryCount,
                    ClearEntryPoint(this));
}

bool NVPixelDataRange::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_pixel_data_range");
}

void NVPixelDataRange::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVPixelDataRangeEntryPoints, __NVPixelDataRangeEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVPointSprite Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVPointSprite::NVPointSprite()
{
    std::for_each_n(__NVPointSpriteEntryPoints, __NVPointSpriteEntryCount,
                    ClearEntryPoint(this));
}

bool NVPointSprite::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_point_sprite");
}

void NVPointSprite::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVPointSpriteEntryPoints, __NVPointSpriteEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVPresentVideo Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVPresentVideo::NVPresentVideo()
{
    std::for_each_n(__NVPresentVideoEntryPoints, __NVPresentVideoEntryCount,
                    ClearEntryPoint(this));
}

bool NVPresentVideo::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_present_video");
}

void NVPresentVideo::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVPresentVideoEntryPoints, __NVPresentVideoEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVPrimitiveRestart Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVPrimitiveRestart::NVPrimitiveRestart()
{
    std::for_each_n(__NVPrimitiveRestartEntryPoints, __NVPrimitiveRestartEntryCount,
                    ClearEntryPoint(this));
}

bool NVPrimitiveRestart::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_primitive_restart");
}

void NVPrimitiveRestart::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVPrimitiveRestartEntryPoints, __NVPrimitiveRestartEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVQueryResource Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVQueryResource::NVQueryResource()
{
    std::for_each_n(__NVQueryResourceEntryPoints, __NVQueryResourceEntryCount,
                    ClearEntryPoint(this));
}

bool NVQueryResource::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_query_resource");
}

void NVQueryResource::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVQueryResourceEntryPoints, __NVQueryResourceEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVQueryResourceTag Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVQueryResourceTag::NVQueryResourceTag()
{
    std::for_each_n(__NVQueryResourceTagEntryPoints, __NVQueryResourceTagEntryCount,
                    ClearEntryPoint(this));
}

bool NVQueryResourceTag::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_query_resource_tag");
}

void NVQueryResourceTag::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVQueryResourceTagEntryPoints, __NVQueryResourceTagEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVRegisterCombiners Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVRegisterCombiners::NVRegisterCombiners()
{
    std::for_each_n(__NVRegisterCombinersEntryPoints, __NVRegisterCombinersEntryCount,
                    ClearEntryPoint(this));
}

bool NVRegisterCombiners::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_register_combiners");
}

void NVRegisterCombiners::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVRegisterCombinersEntryPoints, __NVRegisterCombinersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVRegisterCOMBINERS2 Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVRegisterCOMBINERS2::NVRegisterCOMBINERS2()
{
    std::for_each_n(__NVRegisterCOMBINERS2EntryPoints, __NVRegisterCOMBINERS2EntryCount,
                    ClearEntryPoint(this));
}

bool NVRegisterCOMBINERS2::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_register_combiners2");
}

void NVRegisterCOMBINERS2::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVRegisterCOMBINERS2EntryPoints, __NVRegisterCOMBINERS2EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVSampleLocations Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVSampleLocations::NVSampleLocations()
{
    std::for_each_n(__NVSampleLocationsEntryPoints, __NVSampleLocationsEntryCount,
                    ClearEntryPoint(this));
}

bool NVSampleLocations::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_sample_locations");
}

void NVSampleLocations::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVSampleLocationsEntryPoints, __NVSampleLocationsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVScissorExclusive Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVScissorExclusive::NVScissorExclusive()
{
    std::for_each_n(__NVScissorExclusiveEntryPoints, __NVScissorExclusiveEntryCount,
                    ClearEntryPoint(this));
}

bool NVScissorExclusive::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_scissor_exclusive");
}

void NVScissorExclusive::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVScissorExclusiveEntryPoints, __NVScissorExclusiveEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVShaderBufferLoad Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVShaderBufferLoad::NVShaderBufferLoad()
{
    std::for_each_n(__NVShaderBufferLoadEntryPoints, __NVShaderBufferLoadEntryCount,
                    ClearEntryPoint(this));
}

bool NVShaderBufferLoad::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_shader_buffer_load");
}

void NVShaderBufferLoad::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVShaderBufferLoadEntryPoints, __NVShaderBufferLoadEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVShadingRateImage Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVShadingRateImage::NVShadingRateImage()
{
    std::for_each_n(__NVShadingRateImageEntryPoints, __NVShadingRateImageEntryCount,
                    ClearEntryPoint(this));
}

bool NVShadingRateImage::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_shading_rate_image");
}

void NVShadingRateImage::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVShadingRateImageEntryPoints, __NVShadingRateImageEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVTextureBarrier Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVTextureBarrier::NVTextureBarrier()
{
    std::for_each_n(__NVTextureBarrierEntryPoints, __NVTextureBarrierEntryCount,
                    ClearEntryPoint(this));
}

bool NVTextureBarrier::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_texture_barrier");
}

void NVTextureBarrier::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVTextureBarrierEntryPoints, __NVTextureBarrierEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVTextureMultisample Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVTextureMultisample::NVTextureMultisample()
{
    std::for_each_n(__NVTextureMultisampleEntryPoints, __NVTextureMultisampleEntryCount,
                    ClearEntryPoint(this));
}

bool NVTextureMultisample::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_texture_multisample");
}

void NVTextureMultisample::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVTextureMultisampleEntryPoints, __NVTextureMultisampleEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVTimelineSemaphore Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVTimelineSemaphore::NVTimelineSemaphore()
{
    std::for_each_n(__NVTimelineSemaphoreEntryPoints, __NVTimelineSemaphoreEntryCount,
                    ClearEntryPoint(this));
}

bool NVTimelineSemaphore::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_timeline_semaphore");
}

void NVTimelineSemaphore::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVTimelineSemaphoreEntryPoints, __NVTimelineSemaphoreEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVTransformFeedback Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVTransformFeedback::NVTransformFeedback()
{
    std::for_each_n(__NVTransformFeedbackEntryPoints, __NVTransformFeedbackEntryCount,
                    ClearEntryPoint(this));
}

bool NVTransformFeedback::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_transform_feedback");
}

void NVTransformFeedback::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVTransformFeedbackEntryPoints, __NVTransformFeedbackEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVTransformFeedback2 Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVTransformFeedback2::NVTransformFeedback2()
{
    std::for_each_n(__NVTransformFeedback2EntryPoints, __NVTransformFeedback2EntryCount,
                    ClearEntryPoint(this));
}

bool NVTransformFeedback2::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_transform_feedback2");
}

void NVTransformFeedback2::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVTransformFeedback2EntryPoints, __NVTransformFeedback2EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVdpauInterop Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVdpauInterop::NVVdpauInterop()
{
    std::for_each_n(__NVVdpauInteropEntryPoints, __NVVdpauInteropEntryCount,
                    ClearEntryPoint(this));
}

bool NVVdpauInterop::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_vdpau_interop");
}

void NVVdpauInterop::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVVdpauInteropEntryPoints, __NVVdpauInteropEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVdpauINTEROP2 Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVdpauINTEROP2::NVVdpauINTEROP2()
{
    std::for_each_n(__NVVdpauINTEROP2EntryPoints, __NVVdpauINTEROP2EntryCount,
                    ClearEntryPoint(this));
}

bool NVVdpauINTEROP2::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_vdpau_interop2");
}

void NVVdpauINTEROP2::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVVdpauINTEROP2EntryPoints, __NVVdpauINTEROP2EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVertexArrayRange Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVertexArrayRange::NVVertexArrayRange()
{
    std::for_each_n(__NVVertexArrayRangeEntryPoints, __NVVertexArrayRangeEntryCount,
                    ClearEntryPoint(this));
}

bool NVVertexArrayRange::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_vertex_array_range");
}

void NVVertexArrayRange::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVVertexArrayRangeEntryPoints, __NVVertexArrayRangeEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVertexAttribInteger64BIT Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVertexAttribInteger64BIT::NVVertexAttribInteger64BIT()
{
    std::for_each_n(__NVVertexAttribInteger64BITEntryPoints, __NVVertexAttribInteger64BITEntryCount,
                    ClearEntryPoint(this));
}

bool NVVertexAttribInteger64BIT::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_vertex_attrib_integer_64bit");
}

void NVVertexAttribInteger64BIT::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVVertexAttribInteger64BITEntryPoints, __NVVertexAttribInteger64BITEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVertexBufferUnifiedMemory Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVertexBufferUnifiedMemory::NVVertexBufferUnifiedMemory()
{
    std::for_each_n(__NVVertexBufferUnifiedMemoryEntryPoints, __NVVertexBufferUnifiedMemoryEntryCount,
                    ClearEntryPoint(this));
}

bool NVVertexBufferUnifiedMemory::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_vertex_buffer_unified_memory");
}

void NVVertexBufferUnifiedMemory::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVVertexBufferUnifiedMemoryEntryPoints, __NVVertexBufferUnifiedMemoryEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVertexProgram Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVertexProgram::NVVertexProgram()
{
    std::for_each_n(__NVVertexProgramEntryPoints, __NVVertexProgramEntryCount,
                    ClearEntryPoint(this));
}

bool NVVertexProgram::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_vertex_program");
}

void NVVertexProgram::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVVertexProgramEntryPoints, __NVVertexProgramEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVideoCapture Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVideoCapture::NVVideoCapture()
{
    std::for_each_n(__NVVideoCaptureEntryPoints, __NVVideoCaptureEntryCount,
                    ClearEntryPoint(this));
}

bool NVVideoCapture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_video_capture");
}

void NVVideoCapture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVVideoCaptureEntryPoints, __NVVideoCaptureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVViewportSwizzle Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVViewportSwizzle::NVViewportSwizzle()
{
    std::for_each_n(__NVViewportSwizzleEntryPoints, __NVViewportSwizzleEntryCount,
                    ClearEntryPoint(this));
}

bool NVViewportSwizzle::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_NV_viewport_swizzle");
}

void NVViewportSwizzle::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__NVViewportSwizzleEntryPoints, __NVViewportSwizzleEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// OESByteCoordinates Member Definitions
////////////////////////////////////////////////////////////////////////////////
OESByteCoordinates::OESByteCoordinates()
{
    std::for_each_n(__OESByteCoordinatesEntryPoints, __OESByteCoordinatesEntryCount,
                    ClearEntryPoint(this));
}

bool OESByteCoordinates::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_OES_byte_coordinates");
}

void OESByteCoordinates::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__OESByteCoordinatesEntryPoints, __OESByteCoordinatesEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// OESFixedPoint Member Definitions
////////////////////////////////////////////////////////////////////////////////
OESFixedPoint::OESFixedPoint()
{
    std::for_each_n(__OESFixedPointEntryPoints, __OESFixedPointEntryCount,
                    ClearEntryPoint(this));
}

bool OESFixedPoint::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_OES_fixed_point");
}

void OESFixedPoint::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__OESFixedPointEntryPoints, __OESFixedPointEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// OESQueryMatrix Member Definitions
////////////////////////////////////////////////////////////////////////////////
OESQueryMatrix::OESQueryMatrix()
{
    std::for_each_n(__OESQueryMatrixEntryPoints, __OESQueryMatrixEntryCount,
                    ClearEntryPoint(this));
}

bool OESQueryMatrix::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_OES_query_matrix");
}

void OESQueryMatrix::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__OESQueryMatrixEntryPoints, __OESQueryMatrixEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// OESSinglePrecision Member Definitions
////////////////////////////////////////////////////////////////////////////////
OESSinglePrecision::OESSinglePrecision()
{
    std::for_each_n(__OESSinglePrecisionEntryPoints, __OESSinglePrecisionEntryCount,
                    ClearEntryPoint(this));
}

bool OESSinglePrecision::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_OES_single_precision");
}

void OESSinglePrecision::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__OESSinglePrecisionEntryPoints, __OESSinglePrecisionEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// OvrMultiview Member Definitions
////////////////////////////////////////////////////////////////////////////////
OvrMultiview::OvrMultiview()
{
    std::for_each_n(__OvrMultiviewEntryPoints, __OvrMultiviewEntryCount,
                    ClearEntryPoint(this));
}

bool OvrMultiview::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_OVR_multiview");
}

void OvrMultiview::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__OvrMultiviewEntryPoints, __OvrMultiviewEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// PGIMiscHints Member Definitions
////////////////////////////////////////////////////////////////////////////////
PGIMiscHints::PGIMiscHints()
{
    std::for_each_n(__PGIMiscHintsEntryPoints, __PGIMiscHintsEntryCount,
                    ClearEntryPoint(this));
}

bool PGIMiscHints::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_PGI_misc_hints");
}

void PGIMiscHints::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__PGIMiscHintsEntryPoints, __PGIMiscHintsEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISDetailTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISDetailTexture::SGISDetailTexture()
{
    std::for_each_n(__SGISDetailTextureEntryPoints, __SGISDetailTextureEntryCount,
                    ClearEntryPoint(this));
}

bool SGISDetailTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_detail_texture");
}

void SGISDetailTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISDetailTextureEntryPoints, __SGISDetailTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISFogFunction Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISFogFunction::SGISFogFunction()
{
    std::for_each_n(__SGISFogFunctionEntryPoints, __SGISFogFunctionEntryCount,
                    ClearEntryPoint(this));
}

bool SGISFogFunction::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_fog_function");
}

void SGISFogFunction::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISFogFunctionEntryPoints, __SGISFogFunctionEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISMultisample Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISMultisample::SGISMultisample()
{
    std::for_each_n(__SGISMultisampleEntryPoints, __SGISMultisampleEntryCount,
                    ClearEntryPoint(this));
}

bool SGISMultisample::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_multisample");
}

void SGISMultisample::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISMultisampleEntryPoints, __SGISMultisampleEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISPixelTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISPixelTexture::SGISPixelTexture()
{
    std::for_each_n(__SGISPixelTextureEntryPoints, __SGISPixelTextureEntryCount,
                    ClearEntryPoint(this));
}

bool SGISPixelTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_pixel_texture");
}

void SGISPixelTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISPixelTextureEntryPoints, __SGISPixelTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISPointParameters Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISPointParameters::SGISPointParameters()
{
    std::for_each_n(__SGISPointParametersEntryPoints, __SGISPointParametersEntryCount,
                    ClearEntryPoint(this));
}

bool SGISPointParameters::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_point_parameters");
}

void SGISPointParameters::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISPointParametersEntryPoints, __SGISPointParametersEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISSharpenTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISSharpenTexture::SGISSharpenTexture()
{
    std::for_each_n(__SGISSharpenTextureEntryPoints, __SGISSharpenTextureEntryCount,
                    ClearEntryPoint(this));
}

bool SGISSharpenTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_sharpen_texture");
}

void SGISSharpenTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISSharpenTextureEntryPoints, __SGISSharpenTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISTEXTURE4D Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISTEXTURE4D::SGISTEXTURE4D()
{
    std::for_each_n(__SGISTEXTURE4DEntryPoints, __SGISTEXTURE4DEntryCount,
                    ClearEntryPoint(this));
}

bool SGISTEXTURE4D::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_texture4D");
}

void SGISTEXTURE4D::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISTEXTURE4DEntryPoints, __SGISTEXTURE4DEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISTextureColorMask Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISTextureColorMask::SGISTextureColorMask()
{
    std::for_each_n(__SGISTextureColorMaskEntryPoints, __SGISTextureColorMaskEntryCount,
                    ClearEntryPoint(this));
}

bool SGISTextureColorMask::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_texture_color_mask");
}

void SGISTextureColorMask::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISTextureColorMaskEntryPoints, __SGISTextureColorMaskEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGISTextureFILTER4 Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGISTextureFILTER4::SGISTextureFILTER4()
{
    std::for_each_n(__SGISTextureFILTER4EntryPoints, __SGISTextureFILTER4EntryCount,
                    ClearEntryPoint(this));
}

bool SGISTextureFILTER4::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIS_texture_filter4");
}

void SGISTextureFILTER4::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGISTextureFILTER4EntryPoints, __SGISTextureFILTER4EntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXAsync Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXAsync::SGIXAsync()
{
    std::for_each_n(__SGIXAsyncEntryPoints, __SGIXAsyncEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXAsync::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_async");
}

void SGIXAsync::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXAsyncEntryPoints, __SGIXAsyncEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXFlushRaster Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXFlushRaster::SGIXFlushRaster()
{
    std::for_each_n(__SGIXFlushRasterEntryPoints, __SGIXFlushRasterEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXFlushRaster::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_flush_raster");
}

void SGIXFlushRaster::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXFlushRasterEntryPoints, __SGIXFlushRasterEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXFragmentLighting Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXFragmentLighting::SGIXFragmentLighting()
{
    std::for_each_n(__SGIXFragmentLightingEntryPoints, __SGIXFragmentLightingEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXFragmentLighting::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_fragment_lighting");
}

void SGIXFragmentLighting::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXFragmentLightingEntryPoints, __SGIXFragmentLightingEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXFramezoom Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXFramezoom::SGIXFramezoom()
{
    std::for_each_n(__SGIXFramezoomEntryPoints, __SGIXFramezoomEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXFramezoom::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_framezoom");
}

void SGIXFramezoom::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXFramezoomEntryPoints, __SGIXFramezoomEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXIglooInterface Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXIglooInterface::SGIXIglooInterface()
{
    std::for_each_n(__SGIXIglooInterfaceEntryPoints, __SGIXIglooInterfaceEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXIglooInterface::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_igloo_interface");
}

void SGIXIglooInterface::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXIglooInterfaceEntryPoints, __SGIXIglooInterfaceEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXInstrumentS Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXInstrumentS::SGIXInstrumentS()
{
    std::for_each_n(__SGIXInstrumentSEntryPoints, __SGIXInstrumentSEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXInstrumentS::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_instruments");
}

void SGIXInstrumentS::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXInstrumentSEntryPoints, __SGIXInstrumentSEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXListPriority Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXListPriority::SGIXListPriority()
{
    std::for_each_n(__SGIXListPriorityEntryPoints, __SGIXListPriorityEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXListPriority::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_list_priority");
}

void SGIXListPriority::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXListPriorityEntryPoints, __SGIXListPriorityEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXPixelTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXPixelTexture::SGIXPixelTexture()
{
    std::for_each_n(__SGIXPixelTextureEntryPoints, __SGIXPixelTextureEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXPixelTexture::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_pixel_texture");
}

void SGIXPixelTexture::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXPixelTextureEntryPoints, __SGIXPixelTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXPolynomialFfd Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXPolynomialFfd::SGIXPolynomialFfd()
{
    std::for_each_n(__SGIXPolynomialFfdEntryPoints, __SGIXPolynomialFfdEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXPolynomialFfd::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_polynomial_ffd");
}

void SGIXPolynomialFfd::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXPolynomialFfdEntryPoints, __SGIXPolynomialFfdEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXReferencePlane Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXReferencePlane::SGIXReferencePlane()
{
    std::for_each_n(__SGIXReferencePlaneEntryPoints, __SGIXReferencePlaneEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXReferencePlane::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_reference_plane");
}

void SGIXReferencePlane::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXReferencePlaneEntryPoints, __SGIXReferencePlaneEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXSprite Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXSprite::SGIXSprite()
{
    std::for_each_n(__SGIXSpriteEntryPoints, __SGIXSpriteEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXSprite::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_sprite");
}

void SGIXSprite::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXSpriteEntryPoints, __SGIXSpriteEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIXTagSampleBuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIXTagSampleBuffer::SGIXTagSampleBuffer()
{
    std::for_each_n(__SGIXTagSampleBufferEntryPoints, __SGIXTagSampleBufferEntryCount,
                    ClearEntryPoint(this));
}

bool SGIXTagSampleBuffer::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGIX_tag_sample_buffer");
}

void SGIXTagSampleBuffer::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIXTagSampleBufferEntryPoints, __SGIXTagSampleBufferEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SGIColorTable Member Definitions
////////////////////////////////////////////////////////////////////////////////
SGIColorTable::SGIColorTable()
{
    std::for_each_n(__SGIColorTableEntryPoints, __SGIColorTableEntryCount,
                    ClearEntryPoint(this));
}

bool SGIColorTable::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SGI_color_table");
}

void SGIColorTable::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SGIColorTableEntryPoints, __SGIColorTableEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SUNXConstantData Member Definitions
////////////////////////////////////////////////////////////////////////////////
SUNXConstantData::SUNXConstantData()
{
    std::for_each_n(__SUNXConstantDataEntryPoints, __SUNXConstantDataEntryCount,
                    ClearEntryPoint(this));
}

bool SUNXConstantData::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SUNX_constant_data");
}

void SUNXConstantData::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SUNXConstantDataEntryPoints, __SUNXConstantDataEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SUNGlobalAlpha Member Definitions
////////////////////////////////////////////////////////////////////////////////
SUNGlobalAlpha::SUNGlobalAlpha()
{
    std::for_each_n(__SUNGlobalAlphaEntryPoints, __SUNGlobalAlphaEntryCount,
                    ClearEntryPoint(this));
}

bool SUNGlobalAlpha::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SUN_global_alpha");
}

void SUNGlobalAlpha::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SUNGlobalAlphaEntryPoints, __SUNGlobalAlphaEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SUNMeshArray Member Definitions
////////////////////////////////////////////////////////////////////////////////
SUNMeshArray::SUNMeshArray()
{
    std::for_each_n(__SUNMeshArrayEntryPoints, __SUNMeshArrayEntryCount,
                    ClearEntryPoint(this));
}

bool SUNMeshArray::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SUN_mesh_array");
}

void SUNMeshArray::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SUNMeshArrayEntryPoints, __SUNMeshArrayEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SUNTriangleList Member Definitions
////////////////////////////////////////////////////////////////////////////////
SUNTriangleList::SUNTriangleList()
{
    std::for_each_n(__SUNTriangleListEntryPoints, __SUNTriangleListEntryCount,
                    ClearEntryPoint(this));
}

bool SUNTriangleList::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SUN_triangle_list");
}

void SUNTriangleList::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SUNTriangleListEntryPoints, __SUNTriangleListEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// SUNVertex Member Definitions
////////////////////////////////////////////////////////////////////////////////
SUNVertex::SUNVertex()
{
    std::for_each_n(__SUNVertexEntryPoints, __SUNVertexEntryCount,
                    ClearEntryPoint(this));
}

bool SUNVertex::isPresent(const APIResolver *resolver) const
{
    return resolver->isExtensionPresent("GL_SUN_vertex");
}

void SUNVertex::resolveEntryPoints(const APIResolver *resolver)
{
    std::for_each_n(__SUNVertexEntryPoints, __SUNVertexEntryCount,
                    ResolveEntryPoint(this, resolver));
}

} // namespace gl

//! @file Ag/GLAPI/APIs.hpp
//! @brief A file generated from the OpenGL API schema definition to create
//! C++ 17 compatible language bindings.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __061B23F4_7783_415E_B538_DD0F5B335134_INCLUDED__
#define __061B23F4_7783_415E_B538_DD0F5B335134_INCLUDED__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "EnumTypes.hpp"

namespace gl {
////////////////////////////////////////////////////////////////////////////////
// API Command Groups
////////////////////////////////////////////////////////////////////////////////
// Main API specification.
struct GL {
    // Public Members

    // GL 1.0
    void (APIENTRY *glBlendFunc)(GLenum sfactor, GLenum dfactor);
    void (APIENTRY *glClear)(GLbitfield mask);
    void (APIENTRY *glClearColor)(GLfloat red, GLfloat green, GLfloat blue,
                                  GLfloat alpha);
    void (APIENTRY *glClearDepth)(GLdouble depth);
    void (APIENTRY *glClearStencil)(GLint s);
    void (APIENTRY *glColorMask)(GLboolean red, GLboolean green, GLboolean blue,
                                 GLboolean alpha);
    void (APIENTRY *glCullFace)(GLenum mode);
    void (APIENTRY *glDepthFunc)(GLenum func);
    void (APIENTRY *glDepthMask)(GLboolean flag);
    void (APIENTRY *glDepthRange)(GLdouble n, GLdouble f);
    void (APIENTRY *glDisable)(GLenum cap);
    void (APIENTRY *glDrawBuffer)(GLenum buf);
    void (APIENTRY *glEnable)(GLenum cap);
    void (APIENTRY *glFinish)();
    void (APIENTRY *glFlush)();
    void (APIENTRY *glFrontFace)(GLenum mode);
    void (APIENTRY *glGetBooleanv)(GLenum pname, GLboolean *data);
    void (APIENTRY *glGetDoublev)(GLenum pname, GLdouble *data);
    GLenum (APIENTRY *glGetError)();
    void (APIENTRY *glGetFloatv)(GLenum pname, GLfloat *data);
    void (APIENTRY *glGetIntegerv)(GLenum pname, GLint *data);
    const char *(APIENTRY *glGetString)(GLenum name);
    void (APIENTRY *glGetTexImage)(GLenum target, GLint level, GLenum format,
                                   GLenum type, void *pixels);
    void (APIENTRY *glGetTexLevelParameterfv)(GLenum target, GLint level,
                                              GLenum pname, GLfloat *params);
    void (APIENTRY *glGetTexLevelParameteriv)(GLenum target, GLint level,
                                              GLenum pname, GLint *params);
    void (APIENTRY *glGetTexParameterfv)(GLenum target, GLenum pname,
                                         GLfloat *params);
    void (APIENTRY *glGetTexParameteriv)(GLenum target, GLenum pname,
                                         GLint *params);
    void (APIENTRY *glHint)(GLenum target, GLenum mode);
    GLboolean (APIENTRY *glIsEnabled)(GLenum cap);
    void (APIENTRY *glLineWidth)(GLfloat width);
    void (APIENTRY *glLogicOp)(GLenum opcode);
    void (APIENTRY *glPixelStoref)(GLenum pname, GLfloat param);
    void (APIENTRY *glPixelStorei)(GLenum pname, GLint param);
    void (APIENTRY *glPointSize)(GLfloat size);
    void (APIENTRY *glPolygonMode)(GLenum face, GLenum mode);
    void (APIENTRY *glReadBuffer)(GLenum src);
    void (APIENTRY *glReadPixels)(GLint x, GLint y, GLsizei width,
                                  GLsizei height, GLenum format, GLenum type,
                                  void *pixels);
    void (APIENTRY *glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
    void (APIENTRY *glStencilFunc)(GLenum func, GLint ref, GLuint mask);
    void (APIENTRY *glStencilMask)(GLuint mask);
    void (APIENTRY *glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
    void (APIENTRY *glTexImage1D)(GLenum target, GLint level,
                                  GLint internalformat, GLsizei width,
                                  GLint border, GLenum format, GLenum type,
                                  const void *pixels);
    void (APIENTRY *glTexImage2D)(GLenum target, GLint level,
                                  GLint internalformat, GLsizei width,
                                  GLsizei height, GLint border, GLenum format,
                                  GLenum type, const void *pixels);
    void (APIENTRY *glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
    void (APIENTRY *glTexParameterfv)(GLenum target, GLenum pname,
                                      const GLfloat *params);
    void (APIENTRY *glTexParameteri)(GLenum target, GLenum pname, GLint param);
    void (APIENTRY *glTexParameteriv)(GLenum target, GLenum pname,
                                      const GLint *params);
    void (APIENTRY *glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

    // GL 1.1
    void (APIENTRY *glBindTexture)(GLenum target, GLuint texture);
    void (APIENTRY *glCopyTexImage1D)(GLenum target, GLint level,
                                      GLenum internalformat, GLint x, GLint y,
                                      GLsizei width, GLint border);
    void (APIENTRY *glCopyTexImage2D)(GLenum target, GLint level,
                                      GLenum internalformat, GLint x, GLint y,
                                      GLsizei width, GLsizei height,
                                      GLint border);
    void (APIENTRY *glCopyTexSubImage1D)(GLenum target, GLint level,
                                         GLint xoffset, GLint x, GLint y,
                                         GLsizei width);
    void (APIENTRY *glCopyTexSubImage2D)(GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset, GLint x,
                                         GLint y, GLsizei width, GLsizei height);
    void (APIENTRY *glDeleteTextures)(GLsizei n, const GLuint *textures);
    void (APIENTRY *glDrawArrays)(GLenum mode, GLint first, GLsizei count);
    void (APIENTRY *glDrawElements)(GLenum mode, GLsizei count, GLenum type,
                                    const void *indices);
    void (APIENTRY *glGenTextures)(GLsizei n, GLuint *textures);
    GLboolean (APIENTRY *glIsTexture)(GLuint texture);
    void (APIENTRY *glPolygonOffset)(GLfloat factor, GLfloat units);
    void (APIENTRY *glTexSubImage1D)(GLenum target, GLint level, GLint xoffset,
                                     GLsizei width, GLenum format, GLenum type,
                                     const void *pixels);
    void (APIENTRY *glTexSubImage2D)(GLenum target, GLint level, GLint xoffset,
                                     GLint yoffset, GLsizei width,
                                     GLsizei height, GLenum format, GLenum type,
                                     const void *pixels);

    // GL 1.2
    void (APIENTRY *glCopyTexSubImage3D)(GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLint x, GLint y,
                                         GLsizei width, GLsizei height);
    void (APIENTRY *glDrawRangeElements)(GLenum mode, GLuint start, GLuint end,
                                         GLsizei count, GLenum type,
                                         const void *indices);
    void (APIENTRY *glTexImage3D)(GLenum target, GLint level,
                                  GLint internalformat, GLsizei width,
                                  GLsizei height, GLsizei depth, GLint border,
                                  GLenum format, GLenum type, const void *pixels);
    void (APIENTRY *glTexSubImage3D)(GLenum target, GLint level, GLint xoffset,
                                     GLint yoffset, GLint zoffset, GLsizei width,
                                     GLsizei height, GLsizei depth,
                                     GLenum format, GLenum type,
                                     const void *pixels);

    // GL 1.3
    void (APIENTRY *glActiveTexture)(GLenum texture);
    void (APIENTRY *glCompressedTexImage1D)(GLenum target, GLint level,
                                            GLenum internalformat, GLsizei width,
                                            GLint border, GLsizei imageSize,
                                            const void *data);
    void (APIENTRY *glCompressedTexImage2D)(GLenum target, GLint level,
                                            GLenum internalformat, GLsizei width,
                                            GLsizei height, GLint border,
                                            GLsizei imageSize, const void *data);
    void (APIENTRY *glCompressedTexImage3D)(GLenum target, GLint level,
                                            GLenum internalformat, GLsizei width,
                                            GLsizei height, GLsizei depth,
                                            GLint border, GLsizei imageSize,
                                            const void *data);
    void (APIENTRY *glCompressedTexSubImage1D)(GLenum target, GLint level,
                                               GLint xoffset, GLsizei width,
                                               GLenum format, GLsizei imageSize,
                                               const void *data);
    void (APIENTRY *glCompressedTexSubImage2D)(GLenum target, GLint level,
                                               GLint xoffset, GLint yoffset,
                                               GLsizei width, GLsizei height,
                                               GLenum format, GLsizei imageSize,
                                               const void *data);
    void (APIENTRY *glCompressedTexSubImage3D)(GLenum target, GLint level,
                                               GLint xoffset, GLint yoffset,
                                               GLint zoffset, GLsizei width,
                                               GLsizei height, GLsizei depth,
                                               GLenum format, GLsizei imageSize,
                                               const void *data);
    void (APIENTRY *glGetCompressedTexImage)(GLenum target, GLint level,
                                             void *img);
    void (APIENTRY *glSampleCoverage)(GLfloat value, GLboolean invert);

    // GL 1.4
    void (APIENTRY *glBlendColor)(GLfloat red, GLfloat green, GLfloat blue,
                                  GLfloat alpha);
    void (APIENTRY *glBlendEquation)(GLenum mode);
    void (APIENTRY *glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB,
                                         GLenum sfactorAlpha,
                                         GLenum dfactorAlpha);
    void (APIENTRY *glMultiDrawArrays)(GLenum mode, const GLint *first,
                                       const GLsizei *count, GLsizei drawcount);
    void (APIENTRY *glMultiDrawElements)(GLenum mode, const GLsizei *count,
                                         GLenum type, const void *const*indices,
                                         GLsizei drawcount);
    void (APIENTRY *glPointParameterf)(GLenum pname, GLfloat param);
    void (APIENTRY *glPointParameterfv)(GLenum pname, const GLfloat *params);
    void (APIENTRY *glPointParameteri)(GLenum pname, GLint param);
    void (APIENTRY *glPointParameteriv)(GLenum pname, const GLint *params);

    // GL 1.5
    void (APIENTRY *glBeginQuery)(GLenum target, GLuint id);
    void (APIENTRY *glBindBuffer)(GLenum target, GLuint buffer);
    void (APIENTRY *glBufferData)(GLenum target, GLsizeiptr size,
                                  const void *data, GLenum usage);
    void (APIENTRY *glBufferSubData)(GLenum target, GLintptr offset,
                                     GLsizeiptr size, const void *data);
    void (APIENTRY *glDeleteBuffers)(GLsizei n, const GLuint *buffers);
    void (APIENTRY *glDeleteQueries)(GLsizei n, const GLuint *ids);
    void (APIENTRY *glEndQuery)(GLenum target);
    void (APIENTRY *glGenBuffers)(GLsizei n, GLuint *buffers);
    void (APIENTRY *glGenQueries)(GLsizei n, GLuint *ids);
    void (APIENTRY *glGetBufferParameteriv)(GLenum target, GLenum pname,
                                            GLint *params);
    void (APIENTRY *glGetBufferPointerv)(GLenum target, GLenum pname,
                                         void **params);
    void (APIENTRY *glGetBufferSubData)(GLenum target, GLintptr offset,
                                        GLsizeiptr size, void *data);
    void (APIENTRY *glGetQueryObjectiv)(GLuint id, GLenum pname, GLint *params);
    void (APIENTRY *glGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint *params);
    void (APIENTRY *glGetQueryiv)(GLenum target, GLenum pname, GLint *params);
    GLboolean (APIENTRY *glIsBuffer)(GLuint buffer);
    GLboolean (APIENTRY *glIsQuery)(GLuint id);
    void *(APIENTRY *glMapBuffer)(GLenum target, GLenum access);
    GLboolean (APIENTRY *glUnmapBuffer)(GLenum target);

    // GL 2.0
    void (APIENTRY *glAttachShader)(GLuint program, GLuint shader);
    void (APIENTRY *glBindAttribLocation)(GLuint program, GLuint index,
                                          const GLchar *name);
    void (APIENTRY *glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
    void (APIENTRY *glCompileShader)(GLuint shader);
    GLuint (APIENTRY *glCreateProgram)();
    GLuint (APIENTRY *glCreateShader)(GLenum type);
    void (APIENTRY *glDeleteProgram)(GLuint program);
    void (APIENTRY *glDeleteShader)(GLuint shader);
    void (APIENTRY *glDetachShader)(GLuint program, GLuint shader);
    void (APIENTRY *glDisableVertexAttribArray)(GLuint index);
    void (APIENTRY *glDrawBuffers)(GLsizei n, const GLenum *bufs);
    void (APIENTRY *glEnableVertexAttribArray)(GLuint index);
    void (APIENTRY *glGetActiveAttrib)(GLuint program, GLuint index,
                                       GLsizei bufSize, GLsizei *length,
                                       GLint *size, GLenum *type, GLchar *name);
    void (APIENTRY *glGetActiveUniform)(GLuint program, GLuint index,
                                        GLsizei bufSize, GLsizei *length,
                                        GLint *size, GLenum *type, GLchar *name);
    void (APIENTRY *glGetAttachedShaders)(GLuint program, GLsizei maxCount,
                                          GLsizei *count, GLuint *shaders);
    GLint (APIENTRY *glGetAttribLocation)(GLuint program, const GLchar *name);
    void (APIENTRY *glGetProgramInfoLog)(GLuint program, GLsizei bufSize,
                                         GLsizei *length, GLchar *infoLog);
    void (APIENTRY *glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
    void (APIENTRY *glGetShaderInfoLog)(GLuint shader, GLsizei bufSize,
                                        GLsizei *length, GLchar *infoLog);
    void (APIENTRY *glGetShaderSource)(GLuint shader, GLsizei bufSize,
                                       GLsizei *length, GLchar *source);
    void (APIENTRY *glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
    GLint (APIENTRY *glGetUniformLocation)(GLuint program, const GLchar *name);
    void (APIENTRY *glGetUniformfv)(GLuint program, GLint location,
                                    GLfloat *params);
    void (APIENTRY *glGetUniformiv)(GLuint program, GLint location,
                                    GLint *params);
    void (APIENTRY *glGetVertexAttribPointerv)(GLuint index, GLenum pname,
                                               void **pointer);
    void (APIENTRY *glGetVertexAttribdv)(GLuint index, GLenum pname,
                                         GLdouble *params);
    void (APIENTRY *glGetVertexAttribfv)(GLuint index, GLenum pname,
                                         GLfloat *params);
    void (APIENTRY *glGetVertexAttribiv)(GLuint index, GLenum pname,
                                         GLint *params);
    GLboolean (APIENTRY *glIsProgram)(GLuint program);
    GLboolean (APIENTRY *glIsShader)(GLuint shader);
    void (APIENTRY *glLinkProgram)(GLuint program);
    void (APIENTRY *glShaderSource)(GLuint shader, GLsizei count,
                                    const GLchar *const*string,
                                    const GLint *length);
    void (APIENTRY *glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref,
                                           GLuint mask);
    void (APIENTRY *glStencilMaskSeparate)(GLenum face, GLuint mask);
    void (APIENTRY *glStencilOpSeparate)(GLenum face, GLenum sfail,
                                         GLenum dpfail, GLenum dppass);
    void (APIENTRY *glUniform1f)(GLint location, GLfloat v0);
    void (APIENTRY *glUniform1fv)(GLint location, GLsizei count,
                                  const GLfloat *value);
    void (APIENTRY *glUniform1i)(GLint location, GLint v0);
    void (APIENTRY *glUniform1iv)(GLint location, GLsizei count,
                                  const GLint *value);
    void (APIENTRY *glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
    void (APIENTRY *glUniform2fv)(GLint location, GLsizei count,
                                  const GLfloat *value);
    void (APIENTRY *glUniform2i)(GLint location, GLint v0, GLint v1);
    void (APIENTRY *glUniform2iv)(GLint location, GLsizei count,
                                  const GLint *value);
    void (APIENTRY *glUniform3f)(GLint location, GLfloat v0, GLfloat v1,
                                 GLfloat v2);
    void (APIENTRY *glUniform3fv)(GLint location, GLsizei count,
                                  const GLfloat *value);
    void (APIENTRY *glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
    void (APIENTRY *glUniform3iv)(GLint location, GLsizei count,
                                  const GLint *value);
    void (APIENTRY *glUniform4f)(GLint location, GLfloat v0, GLfloat v1,
                                 GLfloat v2, GLfloat v3);
    void (APIENTRY *glUniform4fv)(GLint location, GLsizei count,
                                  const GLfloat *value);
    void (APIENTRY *glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2,
                                 GLint v3);
    void (APIENTRY *glUniform4iv)(GLint location, GLsizei count,
                                  const GLint *value);
    void (APIENTRY *glUniformMatrix2fv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLfloat *value);
    void (APIENTRY *glUniformMatrix3fv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLfloat *value);
    void (APIENTRY *glUniformMatrix4fv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLfloat *value);
    void (APIENTRY *glUseProgram)(GLuint program);
    void (APIENTRY *glValidateProgram)(GLuint program);
    void (APIENTRY *glVertexAttrib1d)(GLuint index, GLdouble x);
    void (APIENTRY *glVertexAttrib1dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib1f)(GLuint index, GLfloat x);
    void (APIENTRY *glVertexAttrib1fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib1s)(GLuint index, GLshort x);
    void (APIENTRY *glVertexAttrib1sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
    void (APIENTRY *glVertexAttrib2dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
    void (APIENTRY *glVertexAttrib2fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib2s)(GLuint index, GLshort x, GLshort y);
    void (APIENTRY *glVertexAttrib2sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y,
                                      GLdouble z);
    void (APIENTRY *glVertexAttrib3dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y,
                                      GLfloat z);
    void (APIENTRY *glVertexAttrib3fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib3s)(GLuint index, GLshort x, GLshort y,
                                      GLshort z);
    void (APIENTRY *glVertexAttrib3sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib4Nbv)(GLuint index, const GLbyte *v);
    void (APIENTRY *glVertexAttrib4Niv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttrib4Nsv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y,
                                        GLubyte z, GLubyte w);
    void (APIENTRY *glVertexAttrib4Nubv)(GLuint index, const GLubyte *v);
    void (APIENTRY *glVertexAttrib4Nuiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttrib4Nusv)(GLuint index, const GLushort *v);
    void (APIENTRY *glVertexAttrib4bv)(GLuint index, const GLbyte *v);
    void (APIENTRY *glVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y,
                                      GLdouble z, GLdouble w);
    void (APIENTRY *glVertexAttrib4dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y,
                                      GLfloat z, GLfloat w);
    void (APIENTRY *glVertexAttrib4fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib4iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttrib4s)(GLuint index, GLshort x, GLshort y,
                                      GLshort z, GLshort w);
    void (APIENTRY *glVertexAttrib4sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib4ubv)(GLuint index, const GLubyte *v);
    void (APIENTRY *glVertexAttrib4uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttrib4usv)(GLuint index, const GLushort *v);
    void (APIENTRY *glVertexAttribPointer)(GLuint index, GLint size, GLenum type,
                                           GLboolean normalized, GLsizei stride,
                                           const void *pointer);

    // GL 2.1
    void (APIENTRY *glUniformMatrix2x3fv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLfloat *value);
    void (APIENTRY *glUniformMatrix2x4fv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLfloat *value);
    void (APIENTRY *glUniformMatrix3x2fv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLfloat *value);
    void (APIENTRY *glUniformMatrix3x4fv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLfloat *value);
    void (APIENTRY *glUniformMatrix4x2fv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLfloat *value);
    void (APIENTRY *glUniformMatrix4x3fv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLfloat *value);

    // GL 3.0
    void (APIENTRY *glBeginConditionalRender)(GLuint id, GLenum mode);
    void (APIENTRY *glBeginTransformFeedback)(GLenum primitiveMode);
    void (APIENTRY *glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
    void (APIENTRY *glBindBufferRange)(GLenum target, GLuint index,
                                       GLuint buffer, GLintptr offset,
                                       GLsizeiptr size);
    void (APIENTRY *glBindFragDataLocation)(GLuint program, GLuint color,
                                            const GLchar *name);
    void (APIENTRY *glBindFramebuffer)(GLenum target, GLuint framebuffer);
    void (APIENTRY *glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
    void (APIENTRY *glBindVertexArray)(GLuint array);
    void (APIENTRY *glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1,
                                       GLint srcY1, GLint dstX0, GLint dstY0,
                                       GLint dstX1, GLint dstY1, GLbitfield mask,
                                       GLenum filter);
    GLenum (APIENTRY *glCheckFramebufferStatus)(GLenum target);
    void (APIENTRY *glClampColor)(GLenum target, GLenum clamp);
    void (APIENTRY *glClearBufferfi)(GLenum buffer, GLint drawbuffer,
                                     GLfloat depth, GLint stencil);
    void (APIENTRY *glClearBufferfv)(GLenum buffer, GLint drawbuffer,
                                     const GLfloat *value);
    void (APIENTRY *glClearBufferiv)(GLenum buffer, GLint drawbuffer,
                                     const GLint *value);
    void (APIENTRY *glClearBufferuiv)(GLenum buffer, GLint drawbuffer,
                                      const GLuint *value);
    void (APIENTRY *glColorMaski)(GLuint index, GLboolean r, GLboolean g,
                                  GLboolean b, GLboolean a);
    void (APIENTRY *glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
    void (APIENTRY *glDeleteRenderbuffers)(GLsizei n,
                                           const GLuint *renderbuffers);
    void (APIENTRY *glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);
    void (APIENTRY *glDisablei)(GLenum target, GLuint index);
    void (APIENTRY *glEnablei)(GLenum target, GLuint index);
    void (APIENTRY *glEndConditionalRender)();
    void (APIENTRY *glEndTransformFeedback)();
    void (APIENTRY *glFlushMappedBufferRange)(GLenum target, GLintptr offset,
                                              GLsizeiptr length);
    void (APIENTRY *glFramebufferRenderbuffer)(GLenum target, GLenum attachment,
                                               GLenum renderbuffertarget,
                                               GLuint renderbuffer);
    void (APIENTRY *glFramebufferTexture1D)(GLenum target, GLenum attachment,
                                            GLenum textarget, GLuint texture,
                                            GLint level);
    void (APIENTRY *glFramebufferTexture2D)(GLenum target, GLenum attachment,
                                            GLenum textarget, GLuint texture,
                                            GLint level);
    void (APIENTRY *glFramebufferTexture3D)(GLenum target, GLenum attachment,
                                            GLenum textarget, GLuint texture,
                                            GLint level, GLint zoffset);
    void (APIENTRY *glFramebufferTextureLayer)(GLenum target, GLenum attachment,
                                               GLuint texture, GLint level,
                                               GLint layer);
    void (APIENTRY *glGenFramebuffers)(GLsizei n, GLuint *framebuffers);
    void (APIENTRY *glGenRenderbuffers)(GLsizei n, GLuint *renderbuffers);
    void (APIENTRY *glGenVertexArrays)(GLsizei n, GLuint *arrays);
    void (APIENTRY *glGenerateMipmap)(GLenum target);
    void (APIENTRY *glGetBooleani_v)(GLenum target, GLuint index,
                                     GLboolean *data);
    GLint (APIENTRY *glGetFragDataLocation)(GLuint program, const GLchar *name);
    void (APIENTRY *glGetFramebufferAttachmentParameteriv)(GLenum target,
                                                           GLenum attachment,
                                                           GLenum pname,
                                                           GLint *params);
    void (APIENTRY *glGetIntegeri_v)(GLenum target, GLuint index, GLint *data);
    void (APIENTRY *glGetRenderbufferParameteriv)(GLenum target, GLenum pname,
                                                  GLint *params);
    const char *(APIENTRY *glGetStringi)(GLenum name, GLuint index);
    void (APIENTRY *glGetTexParameterIiv)(GLenum target, GLenum pname,
                                          GLint *params);
    void (APIENTRY *glGetTexParameterIuiv)(GLenum target, GLenum pname,
                                           GLuint *params);
    void (APIENTRY *glGetTransformFeedbackVarying)(GLuint program, GLuint index,
                                                   GLsizei bufSize,
                                                   GLsizei *length,
                                                   GLsizei *size, GLenum *type,
                                                   GLchar *name);
    void (APIENTRY *glGetUniformuiv)(GLuint program, GLint location,
                                     GLuint *params);
    void (APIENTRY *glGetVertexAttribIiv)(GLuint index, GLenum pname,
                                          GLint *params);
    void (APIENTRY *glGetVertexAttribIuiv)(GLuint index, GLenum pname,
                                           GLuint *params);
    GLboolean (APIENTRY *glIsEnabledi)(GLenum target, GLuint index);
    GLboolean (APIENTRY *glIsFramebuffer)(GLuint framebuffer);
    GLboolean (APIENTRY *glIsRenderbuffer)(GLuint renderbuffer);
    GLboolean (APIENTRY *glIsVertexArray)(GLuint array);
    void *(APIENTRY *glMapBufferRange)(GLenum target, GLintptr offset,
                                       GLsizeiptr length, GLbitfield access);
    void (APIENTRY *glRenderbufferStorage)(GLenum target, GLenum internalformat,
                                           GLsizei width, GLsizei height);
    void (APIENTRY *glRenderbufferStorageMultisample)(GLenum target,
                                                      GLsizei samples,
                                                      GLenum internalformat,
                                                      GLsizei width,
                                                      GLsizei height);
    void (APIENTRY *glTexParameterIiv)(GLenum target, GLenum pname,
                                       const GLint *params);
    void (APIENTRY *glTexParameterIuiv)(GLenum target, GLenum pname,
                                        const GLuint *params);
    void (APIENTRY *glTransformFeedbackVaryings)(GLuint program, GLsizei count,
                                                 const GLchar *const*varyings,
                                                 GLenum bufferMode);
    void (APIENTRY *glUniform1ui)(GLint location, GLuint v0);
    void (APIENTRY *glUniform1uiv)(GLint location, GLsizei count,
                                   const GLuint *value);
    void (APIENTRY *glUniform2ui)(GLint location, GLuint v0, GLuint v1);
    void (APIENTRY *glUniform2uiv)(GLint location, GLsizei count,
                                   const GLuint *value);
    void (APIENTRY *glUniform3ui)(GLint location, GLuint v0, GLuint v1,
                                  GLuint v2);
    void (APIENTRY *glUniform3uiv)(GLint location, GLsizei count,
                                   const GLuint *value);
    void (APIENTRY *glUniform4ui)(GLint location, GLuint v0, GLuint v1,
                                  GLuint v2, GLuint v3);
    void (APIENTRY *glUniform4uiv)(GLint location, GLsizei count,
                                   const GLuint *value);
    void (APIENTRY *glVertexAttribI1i)(GLuint index, GLint x);
    void (APIENTRY *glVertexAttribI1iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttribI1ui)(GLuint index, GLuint x);
    void (APIENTRY *glVertexAttribI1uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttribI2i)(GLuint index, GLint x, GLint y);
    void (APIENTRY *glVertexAttribI2iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttribI2ui)(GLuint index, GLuint x, GLuint y);
    void (APIENTRY *glVertexAttribI2uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttribI3i)(GLuint index, GLint x, GLint y, GLint z);
    void (APIENTRY *glVertexAttribI3iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttribI3ui)(GLuint index, GLuint x, GLuint y,
                                        GLuint z);
    void (APIENTRY *glVertexAttribI3uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttribI4bv)(GLuint index, const GLbyte *v);
    void (APIENTRY *glVertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z,
                                       GLint w);
    void (APIENTRY *glVertexAttribI4iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttribI4sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttribI4ubv)(GLuint index, const GLubyte *v);
    void (APIENTRY *glVertexAttribI4ui)(GLuint index, GLuint x, GLuint y,
                                        GLuint z, GLuint w);
    void (APIENTRY *glVertexAttribI4uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttribI4usv)(GLuint index, const GLushort *v);
    void (APIENTRY *glVertexAttribIPointer)(GLuint index, GLint size,
                                            GLenum type, GLsizei stride,
                                            const void *pointer);

    // GL 3.1
    void (APIENTRY *glCopyBufferSubData)(GLenum readTarget, GLenum writeTarget,
                                         GLintptr readOffset,
                                         GLintptr writeOffset, GLsizeiptr size);
    void (APIENTRY *glDrawArraysInstanced)(GLenum mode, GLint first,
                                           GLsizei count, GLsizei instancecount);
    void (APIENTRY *glDrawElementsInstanced)(GLenum mode, GLsizei count,
                                             GLenum type, const void *indices,
                                             GLsizei instancecount);
    void (APIENTRY *glGetActiveUniformBlockName)(GLuint program,
                                                 GLuint uniformBlockIndex,
                                                 GLsizei bufSize,
                                                 GLsizei *length,
                                                 GLchar *uniformBlockName);
    void (APIENTRY *glGetActiveUniformBlockiv)(GLuint program,
                                               GLuint uniformBlockIndex,
                                               GLenum pname, GLint *params);
    void (APIENTRY *glGetActiveUniformName)(GLuint program, GLuint uniformIndex,
                                            GLsizei bufSize, GLsizei *length,
                                            GLchar *uniformName);
    void (APIENTRY *glGetActiveUniformsiv)(GLuint program, GLsizei uniformCount,
                                           const GLuint *uniformIndices,
                                           GLenum pname, GLint *params);
    GLuint (APIENTRY *glGetUniformBlockIndex)(GLuint program,
                                              const GLchar *uniformBlockName);
    void (APIENTRY *glGetUniformIndices)(GLuint program, GLsizei uniformCount,
                                         const GLchar *const*uniformNames,
                                         GLuint *uniformIndices);
    void (APIENTRY *glPrimitiveRestartIndex)(GLuint index);
    void (APIENTRY *glTexBuffer)(GLenum target, GLenum internalformat,
                                 GLuint buffer);
    void (APIENTRY *glUniformBlockBinding)(GLuint program,
                                           GLuint uniformBlockIndex,
                                           GLuint uniformBlockBinding);

    // GL 3.2
    GLenum (APIENTRY *glClientWaitSync)(GLsync sync, GLbitfield flags,
                                        GLuint64 timeout);
    void (APIENTRY *glDeleteSync)(GLsync sync);
    void (APIENTRY *glDrawElementsBaseVertex)(GLenum mode, GLsizei count,
                                              GLenum type, const void *indices,
                                              GLint basevertex);
    void (APIENTRY *glDrawElementsInstancedBaseVertex)(GLenum mode,
                                                       GLsizei count,
                                                       GLenum type,
                                                       const void *indices,
                                                       GLsizei instancecount,
                                                       GLint basevertex);
    void (APIENTRY *glDrawRangeElementsBaseVertex)(GLenum mode, GLuint start,
                                                   GLuint end, GLsizei count,
                                                   GLenum type,
                                                   const void *indices,
                                                   GLint basevertex);
    GLsync (APIENTRY *glFenceSync)(GLenum condition, GLbitfield flags);
    void (APIENTRY *glFramebufferTexture)(GLenum target, GLenum attachment,
                                          GLuint texture, GLint level);
    void (APIENTRY *glGetBufferParameteri64v)(GLenum target, GLenum pname,
                                              GLint64 *params);
    void (APIENTRY *glGetInteger64i_v)(GLenum target, GLuint index,
                                       GLint64 *data);
    void (APIENTRY *glGetInteger64v)(GLenum pname, GLint64 *data);
    void (APIENTRY *glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat *val);
    void (APIENTRY *glGetSynciv)(GLsync sync, GLenum pname, GLsizei count,
                                 GLsizei *length, GLint *values);
    GLboolean (APIENTRY *glIsSync)(GLsync sync);
    void (APIENTRY *glMultiDrawElementsBaseVertex)(GLenum mode,
                                                   const GLsizei *count,
                                                   GLenum type,
                                                   const void *const*indices,
                                                   GLsizei drawcount,
                                                   const GLint *basevertex);
    void (APIENTRY *glProvokingVertex)(GLenum mode);
    void (APIENTRY *glSampleMaski)(GLuint maskNumber, GLbitfield mask);
    void (APIENTRY *glTexImage2DMultisample)(GLenum target, GLsizei samples,
                                             GLenum internalformat,
                                             GLsizei width, GLsizei height,
                                             GLboolean fixedsamplelocations);
    void (APIENTRY *glTexImage3DMultisample)(GLenum target, GLsizei samples,
                                             GLenum internalformat,
                                             GLsizei width, GLsizei height,
                                             GLsizei depth,
                                             GLboolean fixedsamplelocations);
    void (APIENTRY *glWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);

    // GL 3.3
    void (APIENTRY *glBindFragDataLocationIndexed)(GLuint program,
                                                   GLuint colorNumber,
                                                   GLuint index,
                                                   const GLchar *name);
    void (APIENTRY *glBindSampler)(GLuint unit, GLuint sampler);
    void (APIENTRY *glDeleteSamplers)(GLsizei count, const GLuint *samplers);
    void (APIENTRY *glGenSamplers)(GLsizei count, GLuint *samplers);
    GLint (APIENTRY *glGetFragDataIndex)(GLuint program, const GLchar *name);
    void (APIENTRY *glGetQueryObjecti64v)(GLuint id, GLenum pname,
                                          GLint64 *params);
    void (APIENTRY *glGetQueryObjectui64v)(GLuint id, GLenum pname,
                                           GLuint64 *params);
    void (APIENTRY *glGetSamplerParameterIiv)(GLuint sampler, GLenum pname,
                                              GLint *params);
    void (APIENTRY *glGetSamplerParameterIuiv)(GLuint sampler, GLenum pname,
                                               GLuint *params);
    void (APIENTRY *glGetSamplerParameterfv)(GLuint sampler, GLenum pname,
                                             GLfloat *params);
    void (APIENTRY *glGetSamplerParameteriv)(GLuint sampler, GLenum pname,
                                             GLint *params);
    GLboolean (APIENTRY *glIsSampler)(GLuint sampler);
    void (APIENTRY *glQueryCounter)(GLuint id, GLenum target);
    void (APIENTRY *glSamplerParameterIiv)(GLuint sampler, GLenum pname,
                                           const GLint *param);
    void (APIENTRY *glSamplerParameterIuiv)(GLuint sampler, GLenum pname,
                                            const GLuint *param);
    void (APIENTRY *glSamplerParameterf)(GLuint sampler, GLenum pname,
                                         GLfloat param);
    void (APIENTRY *glSamplerParameterfv)(GLuint sampler, GLenum pname,
                                          const GLfloat *param);
    void (APIENTRY *glSamplerParameteri)(GLuint sampler, GLenum pname,
                                         GLint param);
    void (APIENTRY *glSamplerParameteriv)(GLuint sampler, GLenum pname,
                                          const GLint *param);
    void (APIENTRY *glVertexAttribDivisor)(GLuint index, GLuint divisor);
    void (APIENTRY *glVertexAttribP1ui)(GLuint index, GLenum type,
                                        GLboolean normalized, GLuint value);
    void (APIENTRY *glVertexAttribP1uiv)(GLuint index, GLenum type,
                                         GLboolean normalized,
                                         const GLuint *value);
    void (APIENTRY *glVertexAttribP2ui)(GLuint index, GLenum type,
                                        GLboolean normalized, GLuint value);
    void (APIENTRY *glVertexAttribP2uiv)(GLuint index, GLenum type,
                                         GLboolean normalized,
                                         const GLuint *value);
    void (APIENTRY *glVertexAttribP3ui)(GLuint index, GLenum type,
                                        GLboolean normalized, GLuint value);
    void (APIENTRY *glVertexAttribP3uiv)(GLuint index, GLenum type,
                                         GLboolean normalized,
                                         const GLuint *value);
    void (APIENTRY *glVertexAttribP4ui)(GLuint index, GLenum type,
                                        GLboolean normalized, GLuint value);
    void (APIENTRY *glVertexAttribP4uiv)(GLuint index, GLenum type,
                                         GLboolean normalized,
                                         const GLuint *value);

    // GL 4.0
    void (APIENTRY *glBeginQueryIndexed)(GLenum target, GLuint index, GLuint id);
    void (APIENTRY *glBindTransformFeedback)(GLenum target, GLuint id);
    void (APIENTRY *glBlendEquationSeparatei)(GLuint buf, GLenum modeRGB,
                                              GLenum modeAlpha);
    void (APIENTRY *glBlendEquationi)(GLuint buf, GLenum mode);
    void (APIENTRY *glBlendFuncSeparatei)(GLuint buf, GLenum srcRGB,
                                          GLenum dstRGB, GLenum srcAlpha,
                                          GLenum dstAlpha);
    void (APIENTRY *glBlendFunci)(GLuint buf, GLenum src, GLenum dst);
    void (APIENTRY *glDeleteTransformFeedbacks)(GLsizei n, const GLuint *ids);
    void (APIENTRY *glDrawArraysIndirect)(GLenum mode, const void *indirect);
    void (APIENTRY *glDrawElementsIndirect)(GLenum mode, GLenum type,
                                            const void *indirect);
    void (APIENTRY *glDrawTransformFeedback)(GLenum mode, GLuint id);
    void (APIENTRY *glDrawTransformFeedbackStream)(GLenum mode, GLuint id,
                                                   GLuint stream);
    void (APIENTRY *glEndQueryIndexed)(GLenum target, GLuint index);
    void (APIENTRY *glGenTransformFeedbacks)(GLsizei n, GLuint *ids);
    void (APIENTRY *glGetActiveSubroutineName)(GLuint program, GLenum shadertype,
                                               GLuint index, GLsizei bufSize,
                                               GLsizei *length, GLchar *name);
    void (APIENTRY *glGetActiveSubroutineUniformName)(GLuint program,
                                                      GLenum shadertype,
                                                      GLuint index,
                                                      GLsizei bufSize,
                                                      GLsizei *length,
                                                      GLchar *name);
    void (APIENTRY *glGetActiveSubroutineUniformiv)(GLuint program,
                                                    GLenum shadertype,
                                                    GLuint index, GLenum pname,
                                                    GLint *values);
    void (APIENTRY *glGetProgramStageiv)(GLuint program, GLenum shadertype,
                                         GLenum pname, GLint *values);
    void (APIENTRY *glGetQueryIndexediv)(GLenum target, GLuint index,
                                         GLenum pname, GLint *params);
    GLuint (APIENTRY *glGetSubroutineIndex)(GLuint program, GLenum shadertype,
                                            const GLchar *name);
    GLint (APIENTRY *glGetSubroutineUniformLocation)(GLuint program,
                                                     GLenum shadertype,
                                                     const GLchar *name);
    void (APIENTRY *glGetUniformSubroutineuiv)(GLenum shadertype, GLint location,
                                               GLuint *params);
    void (APIENTRY *glGetUniformdv)(GLuint program, GLint location,
                                    GLdouble *params);
    GLboolean (APIENTRY *glIsTransformFeedback)(GLuint id);
    void (APIENTRY *glMinSampleShading)(GLfloat value);
    void (APIENTRY *glPatchParameterfv)(GLenum pname, const GLfloat *values);
    void (APIENTRY *glPatchParameteri)(GLenum pname, GLint value);
    void (APIENTRY *glPauseTransformFeedback)();
    void (APIENTRY *glResumeTransformFeedback)();
    void (APIENTRY *glUniform1d)(GLint location, GLdouble x);
    void (APIENTRY *glUniform1dv)(GLint location, GLsizei count,
                                  const GLdouble *value);
    void (APIENTRY *glUniform2d)(GLint location, GLdouble x, GLdouble y);
    void (APIENTRY *glUniform2dv)(GLint location, GLsizei count,
                                  const GLdouble *value);
    void (APIENTRY *glUniform3d)(GLint location, GLdouble x, GLdouble y,
                                 GLdouble z);
    void (APIENTRY *glUniform3dv)(GLint location, GLsizei count,
                                  const GLdouble *value);
    void (APIENTRY *glUniform4d)(GLint location, GLdouble x, GLdouble y,
                                 GLdouble z, GLdouble w);
    void (APIENTRY *glUniform4dv)(GLint location, GLsizei count,
                                  const GLdouble *value);
    void (APIENTRY *glUniformMatrix2dv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLdouble *value);
    void (APIENTRY *glUniformMatrix2x3dv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLdouble *value);
    void (APIENTRY *glUniformMatrix2x4dv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLdouble *value);
    void (APIENTRY *glUniformMatrix3dv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLdouble *value);
    void (APIENTRY *glUniformMatrix3x2dv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLdouble *value);
    void (APIENTRY *glUniformMatrix3x4dv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLdouble *value);
    void (APIENTRY *glUniformMatrix4dv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLdouble *value);
    void (APIENTRY *glUniformMatrix4x2dv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLdouble *value);
    void (APIENTRY *glUniformMatrix4x3dv)(GLint location, GLsizei count,
                                          GLboolean transpose,
                                          const GLdouble *value);
    void (APIENTRY *glUniformSubroutinesuiv)(GLenum shadertype, GLsizei count,
                                             const GLuint *indices);

    // GL 4.1
    void (APIENTRY *glActiveShaderProgram)(GLuint pipeline, GLuint program);
    void (APIENTRY *glBindProgramPipeline)(GLuint pipeline);
    void (APIENTRY *glClearDepthf)(GLfloat d);
    GLuint (APIENTRY *glCreateShaderProgramv)(GLenum type, GLsizei count,
                                              const GLchar *const*strings);
    void (APIENTRY *glDeleteProgramPipelines)(GLsizei n, const GLuint *pipelines);
    void (APIENTRY *glDepthRangeArrayv)(GLuint first, GLsizei count,
                                        const GLdouble *v);
    void (APIENTRY *glDepthRangeIndexed)(GLuint index, GLdouble n, GLdouble f);
    void (APIENTRY *glDepthRangef)(GLfloat n, GLfloat f);
    void (APIENTRY *glGenProgramPipelines)(GLsizei n, GLuint *pipelines);
    void (APIENTRY *glGetDoublei_v)(GLenum target, GLuint index, GLdouble *data);
    void (APIENTRY *glGetFloati_v)(GLenum target, GLuint index, GLfloat *data);
    void (APIENTRY *glGetProgramBinary)(GLuint program, GLsizei bufSize,
                                        GLsizei *length, GLenum *binaryFormat,
                                        void *binary);
    void (APIENTRY *glGetProgramPipelineInfoLog)(GLuint pipeline,
                                                 GLsizei bufSize,
                                                 GLsizei *length,
                                                 GLchar *infoLog);
    void (APIENTRY *glGetProgramPipelineiv)(GLuint pipeline, GLenum pname,
                                            GLint *params);
    void (APIENTRY *glGetShaderPrecisionFormat)(GLenum shadertype,
                                                GLenum precisiontype,
                                                GLint *range, GLint *precision);
    void (APIENTRY *glGetVertexAttribLdv)(GLuint index, GLenum pname,
                                          GLdouble *params);
    GLboolean (APIENTRY *glIsProgramPipeline)(GLuint pipeline);
    void (APIENTRY *glProgramBinary)(GLuint program, GLenum binaryFormat,
                                     const void *binary, GLsizei length);
    void (APIENTRY *glProgramParameteri)(GLuint program, GLenum pname,
                                         GLint value);
    void (APIENTRY *glProgramUniform1d)(GLuint program, GLint location,
                                        GLdouble v0);
    void (APIENTRY *glProgramUniform1dv)(GLuint program, GLint location,
                                         GLsizei count, const GLdouble *value);
    void (APIENTRY *glProgramUniform1f)(GLuint program, GLint location,
                                        GLfloat v0);
    void (APIENTRY *glProgramUniform1fv)(GLuint program, GLint location,
                                         GLsizei count, const GLfloat *value);
    void (APIENTRY *glProgramUniform1i)(GLuint program, GLint location, GLint v0);
    void (APIENTRY *glProgramUniform1iv)(GLuint program, GLint location,
                                         GLsizei count, const GLint *value);
    void (APIENTRY *glProgramUniform1ui)(GLuint program, GLint location,
                                         GLuint v0);
    void (APIENTRY *glProgramUniform1uiv)(GLuint program, GLint location,
                                          GLsizei count, const GLuint *value);
    void (APIENTRY *glProgramUniform2d)(GLuint program, GLint location,
                                        GLdouble v0, GLdouble v1);
    void (APIENTRY *glProgramUniform2dv)(GLuint program, GLint location,
                                         GLsizei count, const GLdouble *value);
    void (APIENTRY *glProgramUniform2f)(GLuint program, GLint location,
                                        GLfloat v0, GLfloat v1);
    void (APIENTRY *glProgramUniform2fv)(GLuint program, GLint location,
                                         GLsizei count, const GLfloat *value);
    void (APIENTRY *glProgramUniform2i)(GLuint program, GLint location, GLint v0,
                                        GLint v1);
    void (APIENTRY *glProgramUniform2iv)(GLuint program, GLint location,
                                         GLsizei count, const GLint *value);
    void (APIENTRY *glProgramUniform2ui)(GLuint program, GLint location,
                                         GLuint v0, GLuint v1);
    void (APIENTRY *glProgramUniform2uiv)(GLuint program, GLint location,
                                          GLsizei count, const GLuint *value);
    void (APIENTRY *glProgramUniform3d)(GLuint program, GLint location,
                                        GLdouble v0, GLdouble v1, GLdouble v2);
    void (APIENTRY *glProgramUniform3dv)(GLuint program, GLint location,
                                         GLsizei count, const GLdouble *value);
    void (APIENTRY *glProgramUniform3f)(GLuint program, GLint location,
                                        GLfloat v0, GLfloat v1, GLfloat v2);
    void (APIENTRY *glProgramUniform3fv)(GLuint program, GLint location,
                                         GLsizei count, const GLfloat *value);
    void (APIENTRY *glProgramUniform3i)(GLuint program, GLint location, GLint v0,
                                        GLint v1, GLint v2);
    void (APIENTRY *glProgramUniform3iv)(GLuint program, GLint location,
                                         GLsizei count, const GLint *value);
    void (APIENTRY *glProgramUniform3ui)(GLuint program, GLint location,
                                         GLuint v0, GLuint v1, GLuint v2);
    void (APIENTRY *glProgramUniform3uiv)(GLuint program, GLint location,
                                          GLsizei count, const GLuint *value);
    void (APIENTRY *glProgramUniform4d)(GLuint program, GLint location,
                                        GLdouble v0, GLdouble v1, GLdouble v2,
                                        GLdouble v3);
    void (APIENTRY *glProgramUniform4dv)(GLuint program, GLint location,
                                         GLsizei count, const GLdouble *value);
    void (APIENTRY *glProgramUniform4f)(GLuint program, GLint location,
                                        GLfloat v0, GLfloat v1, GLfloat v2,
                                        GLfloat v3);
    void (APIENTRY *glProgramUniform4fv)(GLuint program, GLint location,
                                         GLsizei count, const GLfloat *value);
    void (APIENTRY *glProgramUniform4i)(GLuint program, GLint location, GLint v0,
                                        GLint v1, GLint v2, GLint v3);
    void (APIENTRY *glProgramUniform4iv)(GLuint program, GLint location,
                                         GLsizei count, const GLint *value);
    void (APIENTRY *glProgramUniform4ui)(GLuint program, GLint location,
                                         GLuint v0, GLuint v1, GLuint v2,
                                         GLuint v3);
    void (APIENTRY *glProgramUniform4uiv)(GLuint program, GLint location,
                                          GLsizei count, const GLuint *value);
    void (APIENTRY *glProgramUniformMatrix2dv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix2fv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix2x3dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix2x3fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix2x4dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix2x4fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix3dv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix3fv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix3x2dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix3x2fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix3x4dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix3x4fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix4dv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix4fv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix4x2dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix4x2fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix4x3dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix4x3fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glReleaseShaderCompiler)();
    void (APIENTRY *glScissorArrayv)(GLuint first, GLsizei count, const GLint *v);
    void (APIENTRY *glScissorIndexed)(GLuint index, GLint left, GLint bottom,
                                      GLsizei width, GLsizei height);
    void (APIENTRY *glScissorIndexedv)(GLuint index, const GLint *v);
    void (APIENTRY *glShaderBinary)(GLsizei count, const GLuint *shaders,
                                    GLenum binaryFormat, const void *binary,
                                    GLsizei length);
    void (APIENTRY *glUseProgramStages)(GLuint pipeline, GLbitfield stages,
                                        GLuint program);
    void (APIENTRY *glValidateProgramPipeline)(GLuint pipeline);
    void (APIENTRY *glVertexAttribL1d)(GLuint index, GLdouble x);
    void (APIENTRY *glVertexAttribL1dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttribL2d)(GLuint index, GLdouble x, GLdouble y);
    void (APIENTRY *glVertexAttribL2dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttribL3d)(GLuint index, GLdouble x, GLdouble y,
                                       GLdouble z);
    void (APIENTRY *glVertexAttribL3dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttribL4d)(GLuint index, GLdouble x, GLdouble y,
                                       GLdouble z, GLdouble w);
    void (APIENTRY *glVertexAttribL4dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttribLPointer)(GLuint index, GLint size,
                                            GLenum type, GLsizei stride,
                                            const void *pointer);
    void (APIENTRY *glViewportArrayv)(GLuint first, GLsizei count,
                                      const GLfloat *v);
    void (APIENTRY *glViewportIndexedf)(GLuint index, GLfloat x, GLfloat y,
                                        GLfloat w, GLfloat h);
    void (APIENTRY *glViewportIndexedfv)(GLuint index, const GLfloat *v);

    // GL 4.2
    void (APIENTRY *glBindImageTexture)(GLuint unit, GLuint texture, GLint level,
                                        GLboolean layered, GLint layer,
                                        GLenum access, GLenum format);
    void (APIENTRY *glDrawArraysInstancedBaseInstance)(GLenum mode, GLint first,
                                                       GLsizei count,
                                                       GLsizei instancecount,
                                                       GLuint baseinstance);
    void (APIENTRY *glDrawElementsInstancedBaseInstance)(GLenum mode,
                                                         GLsizei count,
                                                         GLenum type,
                                                         const void *indices,
                                                         GLsizei instancecount,
                                                         GLuint baseinstance);
    void (APIENTRY *glDrawElementsInstancedBaseVertexBaseInstance)(GLenum mode,
                                                                   GLsizei count,
                                                                   GLenum type,
                                                                   const void *indices,
                                                                   GLsizei instancecount,
                                                                   GLint basevertex,
                                                                   GLuint baseinstance);
    void (APIENTRY *glDrawTransformFeedbackInstanced)(GLenum mode, GLuint id,
                                                      GLsizei instancecount);
    void (APIENTRY *glDrawTransformFeedbackStreamInstanced)(GLenum mode,
                                                            GLuint id,
                                                            GLuint stream,
                                                            GLsizei instancecount);
    void (APIENTRY *glGetActiveAtomicCounterBufferiv)(GLuint program,
                                                      GLuint bufferIndex,
                                                      GLenum pname,
                                                      GLint *params);
    void (APIENTRY *glGetInternalformativ)(GLenum target, GLenum internalformat,
                                           GLenum pname, GLsizei count,
                                           GLint *params);
    void (APIENTRY *glMemoryBarrier)(GLbitfield barriers);
    void (APIENTRY *glTexStorage1D)(GLenum target, GLsizei levels,
                                    GLenum internalformat, GLsizei width);
    void (APIENTRY *glTexStorage2D)(GLenum target, GLsizei levels,
                                    GLenum internalformat, GLsizei width,
                                    GLsizei height);
    void (APIENTRY *glTexStorage3D)(GLenum target, GLsizei levels,
                                    GLenum internalformat, GLsizei width,
                                    GLsizei height, GLsizei depth);

    // GL 4.3
    void (APIENTRY *glBindVertexBuffer)(GLuint bindingindex, GLuint buffer,
                                        GLintptr offset, GLsizei stride);
    void (APIENTRY *glClearBufferData)(GLenum target, GLenum internalformat,
                                       GLenum format, GLenum type,
                                       const void *data);
    void (APIENTRY *glClearBufferSubData)(GLenum target, GLenum internalformat,
                                          GLintptr offset, GLsizeiptr size,
                                          GLenum format, GLenum type,
                                          const void *data);
    void (APIENTRY *glCopyImageSubData)(GLuint srcName, GLenum srcTarget,
                                        GLint srcLevel, GLint srcX, GLint srcY,
                                        GLint srcZ, GLuint dstName,
                                        GLenum dstTarget, GLint dstLevel,
                                        GLint dstX, GLint dstY, GLint dstZ,
                                        GLsizei srcWidth, GLsizei srcHeight,
                                        GLsizei srcDepth);
    void (APIENTRY *glDebugMessageCallback)(GLDEBUGPROC callback,
                                            const void *userParam);
    void (APIENTRY *glDebugMessageControl)(GLenum source, GLenum type,
                                           GLenum severity, GLsizei count,
                                           const GLuint *ids, GLboolean enabled);
    void (APIENTRY *glDebugMessageInsert)(GLenum source, GLenum type, GLuint id,
                                          GLenum severity, GLsizei length,
                                          const GLchar *buf);
    void (APIENTRY *glDispatchCompute)(GLuint num_groups_x, GLuint num_groups_y,
                                       GLuint num_groups_z);
    void (APIENTRY *glDispatchComputeIndirect)(GLintptr indirect);
    void (APIENTRY *glFramebufferParameteri)(GLenum target, GLenum pname,
                                             GLint param);
    GLuint (APIENTRY *glGetDebugMessageLog)(GLuint count, GLsizei bufSize,
                                            GLenum *sources, GLenum *types,
                                            GLuint *ids, GLenum *severities,
                                            GLsizei *lengths, GLchar *messageLog);
    void (APIENTRY *glGetFramebufferParameteriv)(GLenum target, GLenum pname,
                                                 GLint *params);
    void (APIENTRY *glGetInternalformati64v)(GLenum target,
                                             GLenum internalformat, GLenum pname,
                                             GLsizei count, GLint64 *params);
    void (APIENTRY *glGetObjectLabel)(GLenum identifier, GLuint name,
                                      GLsizei bufSize, GLsizei *length,
                                      GLchar *label);
    void (APIENTRY *glGetObjectPtrLabel)(const void *ptr, GLsizei bufSize,
                                         GLsizei *length, GLchar *label);
    void (APIENTRY *glGetPointerv)(GLenum pname, void **params);
    void (APIENTRY *glGetProgramInterfaceiv)(GLuint program,
                                             GLenum programInterface,
                                             GLenum pname, GLint *params);
    GLuint (APIENTRY *glGetProgramResourceIndex)(GLuint program,
                                                 GLenum programInterface,
                                                 const GLchar *name);
    GLint (APIENTRY *glGetProgramResourceLocation)(GLuint program,
                                                   GLenum programInterface,
                                                   const GLchar *name);
    GLint (APIENTRY *glGetProgramResourceLocationIndex)(GLuint program,
                                                        GLenum programInterface,
                                                        const GLchar *name);
    void (APIENTRY *glGetProgramResourceName)(GLuint program,
                                              GLenum programInterface,
                                              GLuint index, GLsizei bufSize,
                                              GLsizei *length, GLchar *name);
    void (APIENTRY *glGetProgramResourceiv)(GLuint program,
                                            GLenum programInterface,
                                            GLuint index, GLsizei propCount,
                                            const GLenum *props, GLsizei count,
                                            GLsizei *length, GLint *params);
    void (APIENTRY *glInvalidateBufferData)(GLuint buffer);
    void (APIENTRY *glInvalidateBufferSubData)(GLuint buffer, GLintptr offset,
                                               GLsizeiptr length);
    void (APIENTRY *glInvalidateFramebuffer)(GLenum target,
                                             GLsizei numAttachments,
                                             const GLenum *attachments);
    void (APIENTRY *glInvalidateSubFramebuffer)(GLenum target,
                                                GLsizei numAttachments,
                                                const GLenum *attachments,
                                                GLint x, GLint y, GLsizei width,
                                                GLsizei height);
    void (APIENTRY *glInvalidateTexImage)(GLuint texture, GLint level);
    void (APIENTRY *glInvalidateTexSubImage)(GLuint texture, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint zoffset, GLsizei width,
                                             GLsizei height, GLsizei depth);
    void (APIENTRY *glMultiDrawArraysIndirect)(GLenum mode, const void *indirect,
                                               GLsizei drawcount, GLsizei stride);
    void (APIENTRY *glMultiDrawElementsIndirect)(GLenum mode, GLenum type,
                                                 const void *indirect,
                                                 GLsizei drawcount,
                                                 GLsizei stride);
    void (APIENTRY *glObjectLabel)(GLenum identifier, GLuint name,
                                   GLsizei length, const GLchar *label);
    void (APIENTRY *glObjectPtrLabel)(const void *ptr, GLsizei length,
                                      const GLchar *label);
    void (APIENTRY *glPopDebugGroup)();
    void (APIENTRY *glPushDebugGroup)(GLenum source, GLuint id, GLsizei length,
                                      const GLchar *message);
    void (APIENTRY *glShaderStorageBlockBinding)(GLuint program,
                                                 GLuint storageBlockIndex,
                                                 GLuint storageBlockBinding);
    void (APIENTRY *glTexBufferRange)(GLenum target, GLenum internalformat,
                                      GLuint buffer, GLintptr offset,
                                      GLsizeiptr size);
    void (APIENTRY *glTexStorage2DMultisample)(GLenum target, GLsizei samples,
                                               GLenum internalformat,
                                               GLsizei width, GLsizei height,
                                               GLboolean fixedsamplelocations);
    void (APIENTRY *glTexStorage3DMultisample)(GLenum target, GLsizei samples,
                                               GLenum internalformat,
                                               GLsizei width, GLsizei height,
                                               GLsizei depth,
                                               GLboolean fixedsamplelocations);
    void (APIENTRY *glTextureView)(GLuint texture, GLenum target,
                                   GLuint origtexture, GLenum internalformat,
                                   GLuint minlevel, GLuint numlevels,
                                   GLuint minlayer, GLuint numlayers);
    void (APIENTRY *glVertexAttribBinding)(GLuint attribindex,
                                           GLuint bindingindex);
    void (APIENTRY *glVertexAttribFormat)(GLuint attribindex, GLint size,
                                          GLenum type, GLboolean normalized,
                                          GLuint relativeoffset);
    void (APIENTRY *glVertexAttribIFormat)(GLuint attribindex, GLint size,
                                           GLenum type, GLuint relativeoffset);
    void (APIENTRY *glVertexAttribLFormat)(GLuint attribindex, GLint size,
                                           GLenum type, GLuint relativeoffset);
    void (APIENTRY *glVertexBindingDivisor)(GLuint bindingindex, GLuint divisor);

    // GL 4.4
    void (APIENTRY *glBindBuffersBase)(GLenum target, GLuint first,
                                       GLsizei count, const GLuint *buffers);
    void (APIENTRY *glBindBuffersRange)(GLenum target, GLuint first,
                                        GLsizei count, const GLuint *buffers,
                                        const GLintptr *offsets,
                                        const GLsizeiptr *sizes);
    void (APIENTRY *glBindImageTextures)(GLuint first, GLsizei count,
                                         const GLuint *textures);
    void (APIENTRY *glBindSamplers)(GLuint first, GLsizei count,
                                    const GLuint *samplers);
    void (APIENTRY *glBindTextures)(GLuint first, GLsizei count,
                                    const GLuint *textures);
    void (APIENTRY *glBindVertexBuffers)(GLuint first, GLsizei count,
                                         const GLuint *buffers,
                                         const GLintptr *offsets,
                                         const GLsizei *strides);
    void (APIENTRY *glBufferStorage)(GLenum target, GLsizeiptr size,
                                     const void *data, GLbitfield flags);
    void (APIENTRY *glClearTexImage)(GLuint texture, GLint level, GLenum format,
                                     GLenum type, const void *data);
    void (APIENTRY *glClearTexSubImage)(GLuint texture, GLint level,
                                        GLint xoffset, GLint yoffset,
                                        GLint zoffset, GLsizei width,
                                        GLsizei height, GLsizei depth,
                                        GLenum format, GLenum type,
                                        const void *data);

    // GL 4.5
    void (APIENTRY *glBindTextureUnit)(GLuint unit, GLuint texture);
    void (APIENTRY *glBlitNamedFramebuffer)(GLuint readFramebuffer,
                                            GLuint drawFramebuffer, GLint srcX0,
                                            GLint srcY0, GLint srcX1,
                                            GLint srcY1, GLint dstX0,
                                            GLint dstY0, GLint dstX1,
                                            GLint dstY1, GLbitfield mask,
                                            GLenum filter);
    GLenum (APIENTRY *glCheckNamedFramebufferStatus)(GLuint framebuffer,
                                                     GLenum target);
    void (APIENTRY *glClearNamedBufferData)(GLuint buffer, GLenum internalformat,
                                            GLenum format, GLenum type,
                                            const void *data);
    void (APIENTRY *glClearNamedBufferSubData)(GLuint buffer,
                                               GLenum internalformat,
                                               GLintptr offset, GLsizeiptr size,
                                               GLenum format, GLenum type,
                                               const void *data);
    void (APIENTRY *glClearNamedFramebufferfi)(GLuint framebuffer, GLenum buffer,
                                               GLint drawbuffer, GLfloat depth,
                                               GLint stencil);
    void (APIENTRY *glClearNamedFramebufferfv)(GLuint framebuffer, GLenum buffer,
                                               GLint drawbuffer,
                                               const GLfloat *value);
    void (APIENTRY *glClearNamedFramebufferiv)(GLuint framebuffer, GLenum buffer,
                                               GLint drawbuffer,
                                               const GLint *value);
    void (APIENTRY *glClearNamedFramebufferuiv)(GLuint framebuffer,
                                                GLenum buffer, GLint drawbuffer,
                                                const GLuint *value);
    void (APIENTRY *glClipControl)(GLenum origin, GLenum depth);
    void (APIENTRY *glCompressedTextureSubImage1D)(GLuint texture, GLint level,
                                                   GLint xoffset, GLsizei width,
                                                   GLenum format,
                                                   GLsizei imageSize,
                                                   const void *data);
    void (APIENTRY *glCompressedTextureSubImage2D)(GLuint texture, GLint level,
                                                   GLint xoffset, GLint yoffset,
                                                   GLsizei width, GLsizei height,
                                                   GLenum format,
                                                   GLsizei imageSize,
                                                   const void *data);
    void (APIENTRY *glCompressedTextureSubImage3D)(GLuint texture, GLint level,
                                                   GLint xoffset, GLint yoffset,
                                                   GLint zoffset, GLsizei width,
                                                   GLsizei height, GLsizei depth,
                                                   GLenum format,
                                                   GLsizei imageSize,
                                                   const void *data);
    void (APIENTRY *glCopyNamedBufferSubData)(GLuint readBuffer,
                                              GLuint writeBuffer,
                                              GLintptr readOffset,
                                              GLintptr writeOffset,
                                              GLsizeiptr size);
    void (APIENTRY *glCopyTextureSubImage1D)(GLuint texture, GLint level,
                                             GLint xoffset, GLint x, GLint y,
                                             GLsizei width);
    void (APIENTRY *glCopyTextureSubImage2D)(GLuint texture, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint x, GLint y, GLsizei width,
                                             GLsizei height);
    void (APIENTRY *glCopyTextureSubImage3D)(GLuint texture, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint zoffset, GLint x, GLint y,
                                             GLsizei width, GLsizei height);
    void (APIENTRY *glCreateBuffers)(GLsizei n, GLuint *buffers);
    void (APIENTRY *glCreateFramebuffers)(GLsizei n, GLuint *framebuffers);
    void (APIENTRY *glCreateProgramPipelines)(GLsizei n, GLuint *pipelines);
    void (APIENTRY *glCreateQueries)(GLenum target, GLsizei n, GLuint *ids);
    void (APIENTRY *glCreateRenderbuffers)(GLsizei n, GLuint *renderbuffers);
    void (APIENTRY *glCreateSamplers)(GLsizei n, GLuint *samplers);
    void (APIENTRY *glCreateTextures)(GLenum target, GLsizei n, GLuint *textures);
    void (APIENTRY *glCreateTransformFeedbacks)(GLsizei n, GLuint *ids);
    void (APIENTRY *glCreateVertexArrays)(GLsizei n, GLuint *arrays);
    void (APIENTRY *glDisableVertexArrayAttrib)(GLuint vaobj, GLuint index);
    void (APIENTRY *glEnableVertexArrayAttrib)(GLuint vaobj, GLuint index);
    void (APIENTRY *glFlushMappedNamedBufferRange)(GLuint buffer,
                                                   GLintptr offset,
                                                   GLsizeiptr length);
    void (APIENTRY *glGenerateTextureMipmap)(GLuint texture);
    void (APIENTRY *glGetCompressedTextureImage)(GLuint texture, GLint level,
                                                 GLsizei bufSize, void *pixels);
    void (APIENTRY *glGetCompressedTextureSubImage)(GLuint texture, GLint level,
                                                    GLint xoffset, GLint yoffset,
                                                    GLint zoffset, GLsizei width,
                                                    GLsizei height,
                                                    GLsizei depth,
                                                    GLsizei bufSize,
                                                    void *pixels);
    GLenum (APIENTRY *glGetGraphicsResetStatus)();
    void (APIENTRY *glGetNamedBufferParameteri64v)(GLuint buffer, GLenum pname,
                                                   GLint64 *params);
    void (APIENTRY *glGetNamedBufferParameteriv)(GLuint buffer, GLenum pname,
                                                 GLint *params);
    void (APIENTRY *glGetNamedBufferPointerv)(GLuint buffer, GLenum pname,
                                              void **params);
    void (APIENTRY *glGetNamedBufferSubData)(GLuint buffer, GLintptr offset,
                                             GLsizeiptr size, void *data);
    void (APIENTRY *glGetNamedFramebufferAttachmentParameteriv)(GLuint framebuffer,
                                                                GLenum attachment,
                                                                GLenum pname,
                                                                GLint *params);
    void (APIENTRY *glGetNamedFramebufferParameteriv)(GLuint framebuffer,
                                                      GLenum pname, GLint *param);
    void (APIENTRY *glGetNamedRenderbufferParameteriv)(GLuint renderbuffer,
                                                       GLenum pname,
                                                       GLint *params);
    void (APIENTRY *glGetQueryBufferObjecti64v)(GLuint id, GLuint buffer,
                                                GLenum pname, GLintptr offset);
    void (APIENTRY *glGetQueryBufferObjectiv)(GLuint id, GLuint buffer,
                                              GLenum pname, GLintptr offset);
    void (APIENTRY *glGetQueryBufferObjectui64v)(GLuint id, GLuint buffer,
                                                 GLenum pname, GLintptr offset);
    void (APIENTRY *glGetQueryBufferObjectuiv)(GLuint id, GLuint buffer,
                                               GLenum pname, GLintptr offset);
    void (APIENTRY *glGetTextureImage)(GLuint texture, GLint level,
                                       GLenum format, GLenum type,
                                       GLsizei bufSize, void *pixels);
    void (APIENTRY *glGetTextureLevelParameterfv)(GLuint texture, GLint level,
                                                  GLenum pname, GLfloat *params);
    void (APIENTRY *glGetTextureLevelParameteriv)(GLuint texture, GLint level,
                                                  GLenum pname, GLint *params);
    void (APIENTRY *glGetTextureParameterIiv)(GLuint texture, GLenum pname,
                                              GLint *params);
    void (APIENTRY *glGetTextureParameterIuiv)(GLuint texture, GLenum pname,
                                               GLuint *params);
    void (APIENTRY *glGetTextureParameterfv)(GLuint texture, GLenum pname,
                                             GLfloat *params);
    void (APIENTRY *glGetTextureParameteriv)(GLuint texture, GLenum pname,
                                             GLint *params);
    void (APIENTRY *glGetTextureSubImage)(GLuint texture, GLint level,
                                          GLint xoffset, GLint yoffset,
                                          GLint zoffset, GLsizei width,
                                          GLsizei height, GLsizei depth,
                                          GLenum format, GLenum type,
                                          GLsizei bufSize, void *pixels);
    void (APIENTRY *glGetTransformFeedbacki64_v)(GLuint xfb, GLenum pname,
                                                 GLuint index, GLint64 *param);
    void (APIENTRY *glGetTransformFeedbacki_v)(GLuint xfb, GLenum pname,
                                               GLuint index, GLint *param);
    void (APIENTRY *glGetTransformFeedbackiv)(GLuint xfb, GLenum pname,
                                              GLint *param);
    void (APIENTRY *glGetVertexArrayIndexed64iv)(GLuint vaobj, GLuint index,
                                                 GLenum pname, GLint64 *param);
    void (APIENTRY *glGetVertexArrayIndexediv)(GLuint vaobj, GLuint index,
                                               GLenum pname, GLint *param);
    void (APIENTRY *glGetVertexArrayiv)(GLuint vaobj, GLenum pname, GLint *param);
    void (APIENTRY *glGetnCompressedTexImage)(GLenum target, GLint lod,
                                              GLsizei bufSize, void *pixels);
    void (APIENTRY *glGetnTexImage)(GLenum target, GLint level, GLenum format,
                                    GLenum type, GLsizei bufSize, void *pixels);
    void (APIENTRY *glGetnUniformdv)(GLuint program, GLint location,
                                     GLsizei bufSize, GLdouble *params);
    void (APIENTRY *glGetnUniformfv)(GLuint program, GLint location,
                                     GLsizei bufSize, GLfloat *params);
    void (APIENTRY *glGetnUniformiv)(GLuint program, GLint location,
                                     GLsizei bufSize, GLint *params);
    void (APIENTRY *glGetnUniformuiv)(GLuint program, GLint location,
                                      GLsizei bufSize, GLuint *params);
    void (APIENTRY *glInvalidateNamedFramebufferData)(GLuint framebuffer,
                                                      GLsizei numAttachments,
                                                      const GLenum *attachments);
    void (APIENTRY *glInvalidateNamedFramebufferSubData)(GLuint framebuffer,
                                                         GLsizei numAttachments,
                                                         const GLenum *attachments,
                                                         GLint x, GLint y,
                                                         GLsizei width,
                                                         GLsizei height);
    void *(APIENTRY *glMapNamedBuffer)(GLuint buffer, GLenum access);
    void *(APIENTRY *glMapNamedBufferRange)(GLuint buffer, GLintptr offset,
                                            GLsizeiptr length, GLbitfield access);
    void (APIENTRY *glMemoryBarrierByRegion)(GLbitfield barriers);
    void (APIENTRY *glNamedBufferData)(GLuint buffer, GLsizeiptr size,
                                       const void *data, GLenum usage);
    void (APIENTRY *glNamedBufferStorage)(GLuint buffer, GLsizeiptr size,
                                          const void *data, GLbitfield flags);
    void (APIENTRY *glNamedBufferSubData)(GLuint buffer, GLintptr offset,
                                          GLsizeiptr size, const void *data);
    void (APIENTRY *glNamedFramebufferDrawBuffer)(GLuint framebuffer, GLenum buf);
    void (APIENTRY *glNamedFramebufferDrawBuffers)(GLuint framebuffer, GLsizei n,
                                                   const GLenum *bufs);
    void (APIENTRY *glNamedFramebufferParameteri)(GLuint framebuffer,
                                                  GLenum pname, GLint param);
    void (APIENTRY *glNamedFramebufferReadBuffer)(GLuint framebuffer, GLenum src);
    void (APIENTRY *glNamedFramebufferRenderbuffer)(GLuint framebuffer,
                                                    GLenum attachment,
                                                    GLenum renderbuffertarget,
                                                    GLuint renderbuffer);
    void (APIENTRY *glNamedFramebufferTexture)(GLuint framebuffer,
                                               GLenum attachment, GLuint texture,
                                               GLint level);
    void (APIENTRY *glNamedFramebufferTextureLayer)(GLuint framebuffer,
                                                    GLenum attachment,
                                                    GLuint texture, GLint level,
                                                    GLint layer);
    void (APIENTRY *glNamedRenderbufferStorage)(GLuint renderbuffer,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height);
    void (APIENTRY *glNamedRenderbufferStorageMultisample)(GLuint renderbuffer,
                                                           GLsizei samples,
                                                           GLenum internalformat,
                                                           GLsizei width,
                                                           GLsizei height);
    void (APIENTRY *glReadnPixels)(GLint x, GLint y, GLsizei width,
                                   GLsizei height, GLenum format, GLenum type,
                                   GLsizei bufSize, void *data);
    void (APIENTRY *glTextureBarrier)();
    void (APIENTRY *glTextureBuffer)(GLuint texture, GLenum internalformat,
                                     GLuint buffer);
    void (APIENTRY *glTextureBufferRange)(GLuint texture, GLenum internalformat,
                                          GLuint buffer, GLintptr offset,
                                          GLsizeiptr size);
    void (APIENTRY *glTextureParameterIiv)(GLuint texture, GLenum pname,
                                           const GLint *params);
    void (APIENTRY *glTextureParameterIuiv)(GLuint texture, GLenum pname,
                                            const GLuint *params);
    void (APIENTRY *glTextureParameterf)(GLuint texture, GLenum pname,
                                         GLfloat param);
    void (APIENTRY *glTextureParameterfv)(GLuint texture, GLenum pname,
                                          const GLfloat *param);
    void (APIENTRY *glTextureParameteri)(GLuint texture, GLenum pname,
                                         GLint param);
    void (APIENTRY *glTextureParameteriv)(GLuint texture, GLenum pname,
                                          const GLint *param);
    void (APIENTRY *glTextureStorage1D)(GLuint texture, GLsizei levels,
                                        GLenum internalformat, GLsizei width);
    void (APIENTRY *glTextureStorage2D)(GLuint texture, GLsizei levels,
                                        GLenum internalformat, GLsizei width,
                                        GLsizei height);
    void (APIENTRY *glTextureStorage2DMultisample)(GLuint texture,
                                                   GLsizei samples,
                                                   GLenum internalformat,
                                                   GLsizei width, GLsizei height,
                                                   GLboolean fixedsamplelocations);
    void (APIENTRY *glTextureStorage3D)(GLuint texture, GLsizei levels,
                                        GLenum internalformat, GLsizei width,
                                        GLsizei height, GLsizei depth);
    void (APIENTRY *glTextureStorage3DMultisample)(GLuint texture,
                                                   GLsizei samples,
                                                   GLenum internalformat,
                                                   GLsizei width, GLsizei height,
                                                   GLsizei depth,
                                                   GLboolean fixedsamplelocations);
    void (APIENTRY *glTextureSubImage1D)(GLuint texture, GLint level,
                                         GLint xoffset, GLsizei width,
                                         GLenum format, GLenum type,
                                         const void *pixels);
    void (APIENTRY *glTextureSubImage2D)(GLuint texture, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLsizei width, GLsizei height,
                                         GLenum format, GLenum type,
                                         const void *pixels);
    void (APIENTRY *glTextureSubImage3D)(GLuint texture, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLsizei width,
                                         GLsizei height, GLsizei depth,
                                         GLenum format, GLenum type,
                                         const void *pixels);
    void (APIENTRY *glTransformFeedbackBufferBase)(GLuint xfb, GLuint index,
                                                   GLuint buffer);
    void (APIENTRY *glTransformFeedbackBufferRange)(GLuint xfb, GLuint index,
                                                    GLuint buffer,
                                                    GLintptr offset,
                                                    GLsizeiptr size);
    GLboolean (APIENTRY *glUnmapNamedBuffer)(GLuint buffer);
    void (APIENTRY *glVertexArrayAttribBinding)(GLuint vaobj, GLuint attribindex,
                                                GLuint bindingindex);
    void (APIENTRY *glVertexArrayAttribFormat)(GLuint vaobj, GLuint attribindex,
                                               GLint size, GLenum type,
                                               GLboolean normalized,
                                               GLuint relativeoffset);
    void (APIENTRY *glVertexArrayAttribIFormat)(GLuint vaobj, GLuint attribindex,
                                                GLint size, GLenum type,
                                                GLuint relativeoffset);
    void (APIENTRY *glVertexArrayAttribLFormat)(GLuint vaobj, GLuint attribindex,
                                                GLint size, GLenum type,
                                                GLuint relativeoffset);
    void (APIENTRY *glVertexArrayBindingDivisor)(GLuint vaobj,
                                                 GLuint bindingindex,
                                                 GLuint divisor);
    void (APIENTRY *glVertexArrayElementBuffer)(GLuint vaobj, GLuint buffer);
    void (APIENTRY *glVertexArrayVertexBuffer)(GLuint vaobj, GLuint bindingindex,
                                               GLuint buffer, GLintptr offset,
                                               GLsizei stride);
    void (APIENTRY *glVertexArrayVertexBuffers)(GLuint vaobj, GLuint first,
                                                GLsizei count,
                                                const GLuint *buffers,
                                                const GLintptr *offsets,
                                                const GLsizei *strides);

    // GL 4.6
    void (APIENTRY *glMultiDrawArraysIndirectCount)(GLenum mode,
                                                    const void *indirect,
                                                    GLintptr drawcount,
                                                    GLsizei maxdrawcount,
                                                    GLsizei stride);
    void (APIENTRY *glMultiDrawElementsIndirectCount)(GLenum mode, GLenum type,
                                                      const void *indirect,
                                                      GLintptr drawcount,
                                                      GLsizei maxdrawcount,
                                                      GLsizei stride);
    void (APIENTRY *glPolygonOffsetClamp)(GLfloat factor, GLfloat units,
                                          GLfloat clamp);
    void (APIENTRY *glSpecializeShader)(GLuint shader, const GLchar *pEntryPoint,
                                        GLuint numSpecializationConstants,
                                        const GLuint *pConstantIndex,
                                        const GLuint *pConstantValue);

    // Construction
    GL();

    // Operations
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct GL

// GL_3DFX_tbuffer extension API specification.
struct _3DFXTbuffer {
    // Public Members
    void (APIENTRY *glTbufferMask)(GLuint mask);

    // Construction
    _3DFXTbuffer();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct _3DFXTbuffer

// GL_AMD_debug_output extension API specification.
struct AMDDebugOutput {
    // Public Members
    void (APIENTRY *glDebugMessageCallback)(GLDEBUGPROCAMD callback,
                                            void *userParam);
    void (APIENTRY *glDebugMessageEnable)(GLenum category, GLenum severity,
                                          GLsizei count, const GLuint *ids,
                                          GLboolean enabled);
    void (APIENTRY *glDebugMessageInsert)(GLenum category, GLenum severity,
                                          GLuint id, GLsizei length,
                                          const GLchar *buf);
    GLuint (APIENTRY *glGetDebugMessageLog)(GLuint count, GLsizei bufSize,
                                            GLenum *categories,
                                            GLuint *severities, GLuint *ids,
                                            GLsizei *lengths, GLchar *message);

    // Construction
    AMDDebugOutput();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDDebugOutput

// GL_AMD_draw_buffers_blend extension API specification.
struct AMDDrawBuffersBlend {
    // Public Members
    void (APIENTRY *glBlendEquationIndexed)(GLuint buf, GLenum mode);
    void (APIENTRY *glBlendEquationSeparateIndexed)(GLuint buf, GLenum modeRGB,
                                                    GLenum modeAlpha);
    void (APIENTRY *glBlendFuncIndexed)(GLuint buf, GLenum src, GLenum dst);
    void (APIENTRY *glBlendFuncSeparateIndexed)(GLuint buf, GLenum srcRGB,
                                                GLenum dstRGB, GLenum srcAlpha,
                                                GLenum dstAlpha);

    // Construction
    AMDDrawBuffersBlend();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDDrawBuffersBlend

// GL_AMD_framebuffer_multisample_advanced extension API specification.
struct AMDFramebufferMultisampleAdvanced {
    // Public Members
    void (APIENTRY *glNamedRenderbufferStorageMultisampleAdvanced)(GLuint renderbuffer,
                                                                   GLsizei samples,
                                                                   GLsizei storageSamples,
                                                                   GLenum internalformat,
                                                                   GLsizei width,
                                                                   GLsizei height);
    void (APIENTRY *glRenderbufferStorageMultisampleAdvanced)(GLenum target,
                                                              GLsizei samples,
                                                              GLsizei storageSamples,
                                                              GLenum internalformat,
                                                              GLsizei width,
                                                              GLsizei height);

    // Construction
    AMDFramebufferMultisampleAdvanced();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDFramebufferMultisampleAdvanced

// GL_AMD_framebuffer_sample_positions extension API specification.
struct AMDFramebufferSamplePositions {
    // Public Members
    void (APIENTRY *glFramebufferSamplePositionsfv)(GLenum target,
                                                    GLuint numsamples,
                                                    GLuint pixelindex,
                                                    const GLfloat *values);
    void (APIENTRY *glGetFramebufferParameterfv)(GLenum target, GLenum pname,
                                                 GLuint numsamples,
                                                 GLuint pixelindex, GLsizei size,
                                                 GLfloat *values);
    void (APIENTRY *glGetNamedFramebufferParameterfv)(GLuint framebuffer,
                                                      GLenum pname,
                                                      GLuint numsamples,
                                                      GLuint pixelindex,
                                                      GLsizei size,
                                                      GLfloat *values);
    void (APIENTRY *glNamedFramebufferSamplePositionsfv)(GLuint framebuffer,
                                                         GLuint numsamples,
                                                         GLuint pixelindex,
                                                         const GLfloat *values);

    // Construction
    AMDFramebufferSamplePositions();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDFramebufferSamplePositions

// GL_AMD_gpu_shader_int64 extension API specification.
struct AMDGPUShaderInt64 {
    // Public Members
    void (APIENTRY *glGetUniformi64v)(GLuint program, GLint location,
                                      GLint64EXT *params);
    void (APIENTRY *glGetUniformui64v)(GLuint program, GLint location,
                                       GLuint64EXT *params);
    void (APIENTRY *glProgramUniform1i64)(GLuint program, GLint location,
                                          GLint64EXT x);
    void (APIENTRY *glProgramUniform1i64v)(GLuint program, GLint location,
                                           GLsizei count,
                                           const GLint64EXT *value);
    void (APIENTRY *glProgramUniform1ui64)(GLuint program, GLint location,
                                           GLuint64EXT x);
    void (APIENTRY *glProgramUniform1ui64v)(GLuint program, GLint location,
                                            GLsizei count,
                                            const GLuint64EXT *value);
    void (APIENTRY *glProgramUniform2i64)(GLuint program, GLint location,
                                          GLint64EXT x, GLint64EXT y);
    void (APIENTRY *glProgramUniform2i64v)(GLuint program, GLint location,
                                           GLsizei count,
                                           const GLint64EXT *value);
    void (APIENTRY *glProgramUniform2ui64)(GLuint program, GLint location,
                                           GLuint64EXT x, GLuint64EXT y);
    void (APIENTRY *glProgramUniform2ui64v)(GLuint program, GLint location,
                                            GLsizei count,
                                            const GLuint64EXT *value);
    void (APIENTRY *glProgramUniform3i64)(GLuint program, GLint location,
                                          GLint64EXT x, GLint64EXT y,
                                          GLint64EXT z);
    void (APIENTRY *glProgramUniform3i64v)(GLuint program, GLint location,
                                           GLsizei count,
                                           const GLint64EXT *value);
    void (APIENTRY *glProgramUniform3ui64)(GLuint program, GLint location,
                                           GLuint64EXT x, GLuint64EXT y,
                                           GLuint64EXT z);
    void (APIENTRY *glProgramUniform3ui64v)(GLuint program, GLint location,
                                            GLsizei count,
                                            const GLuint64EXT *value);
    void (APIENTRY *glProgramUniform4i64)(GLuint program, GLint location,
                                          GLint64EXT x, GLint64EXT y,
                                          GLint64EXT z, GLint64EXT w);
    void (APIENTRY *glProgramUniform4i64v)(GLuint program, GLint location,
                                           GLsizei count,
                                           const GLint64EXT *value);
    void (APIENTRY *glProgramUniform4ui64)(GLuint program, GLint location,
                                           GLuint64EXT x, GLuint64EXT y,
                                           GLuint64EXT z, GLuint64EXT w);
    void (APIENTRY *glProgramUniform4ui64v)(GLuint program, GLint location,
                                            GLsizei count,
                                            const GLuint64EXT *value);
    void (APIENTRY *glUniform1i64)(GLint location, GLint64EXT x);
    void (APIENTRY *glUniform1i64v)(GLint location, GLsizei count,
                                    const GLint64EXT *value);
    void (APIENTRY *glUniform1ui64)(GLint location, GLuint64EXT x);
    void (APIENTRY *glUniform1ui64v)(GLint location, GLsizei count,
                                     const GLuint64EXT *value);
    void (APIENTRY *glUniform2i64)(GLint location, GLint64EXT x, GLint64EXT y);
    void (APIENTRY *glUniform2i64v)(GLint location, GLsizei count,
                                    const GLint64EXT *value);
    void (APIENTRY *glUniform2ui64)(GLint location, GLuint64EXT x, GLuint64EXT y);
    void (APIENTRY *glUniform2ui64v)(GLint location, GLsizei count,
                                     const GLuint64EXT *value);
    void (APIENTRY *glUniform3i64)(GLint location, GLint64EXT x, GLint64EXT y,
                                   GLint64EXT z);
    void (APIENTRY *glUniform3i64v)(GLint location, GLsizei count,
                                    const GLint64EXT *value);
    void (APIENTRY *glUniform3ui64)(GLint location, GLuint64EXT x, GLuint64EXT y,
                                    GLuint64EXT z);
    void (APIENTRY *glUniform3ui64v)(GLint location, GLsizei count,
                                     const GLuint64EXT *value);
    void (APIENTRY *glUniform4i64)(GLint location, GLint64EXT x, GLint64EXT y,
                                   GLint64EXT z, GLint64EXT w);
    void (APIENTRY *glUniform4i64v)(GLint location, GLsizei count,
                                    const GLint64EXT *value);
    void (APIENTRY *glUniform4ui64)(GLint location, GLuint64EXT x, GLuint64EXT y,
                                    GLuint64EXT z, GLuint64EXT w);
    void (APIENTRY *glUniform4ui64v)(GLint location, GLsizei count,
                                     const GLuint64EXT *value);

    // Construction
    AMDGPUShaderInt64();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDGPUShaderInt64

// GL_AMD_interleaved_elements extension API specification.
struct AMDInterleavedElements {
    // Public Members
    void (APIENTRY *glVertexAttribParameteri)(GLuint index, GLenum pname,
                                              GLint param);

    // Construction
    AMDInterleavedElements();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDInterleavedElements

// GL_AMD_multi_draw_indirect extension API specification.
struct AMDMultiDrawIndirect {
    // Public Members
    void (APIENTRY *glMultiDrawArraysIndirect)(GLenum mode, const void *indirect,
                                               GLsizei primcount, GLsizei stride);
    void (APIENTRY *glMultiDrawElementsIndirect)(GLenum mode, GLenum type,
                                                 const void *indirect,
                                                 GLsizei primcount,
                                                 GLsizei stride);

    // Construction
    AMDMultiDrawIndirect();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDMultiDrawIndirect

// GL_AMD_name_gen_delete extension API specification.
struct AMDNameGenDelete {
    // Public Members
    void (APIENTRY *glDeleteNames)(GLenum identifier, GLuint num,
                                   const GLuint *names);
    void (APIENTRY *glGenNames)(GLenum identifier, GLuint num, GLuint *names);
    GLboolean (APIENTRY *glIsName)(GLenum identifier, GLuint name);

    // Construction
    AMDNameGenDelete();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDNameGenDelete

// GL_AMD_occlusion_query_event extension API specification.
struct AMDOcclusionQueryEvent {
    // Public Members
    void (APIENTRY *glQueryObjectParameterui)(GLenum target, GLuint id,
                                              GLenum pname, GLuint param);

    // Construction
    AMDOcclusionQueryEvent();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDOcclusionQueryEvent

// GL_AMD_performance_monitor extension API specification.
struct AMDPerformanceMonitor {
    // Public Members
    void (APIENTRY *glBeginPerfMonitor)(GLuint monitor);
    void (APIENTRY *glDeletePerfMonitors)(GLsizei n, GLuint *monitors);
    void (APIENTRY *glEndPerfMonitor)(GLuint monitor);
    void (APIENTRY *glGenPerfMonitors)(GLsizei n, GLuint *monitors);
    void (APIENTRY *glGetPerfMonitorCounterData)(GLuint monitor, GLenum pname,
                                                 GLsizei dataSize, GLuint *data,
                                                 GLint *bytesWritten);
    void (APIENTRY *glGetPerfMonitorCounterInfo)(GLuint group, GLuint counter,
                                                 GLenum pname, void *data);
    void (APIENTRY *glGetPerfMonitorCounterString)(GLuint group, GLuint counter,
                                                   GLsizei bufSize,
                                                   GLsizei *length,
                                                   GLchar *counterString);
    void (APIENTRY *glGetPerfMonitorCounters)(GLuint group, GLint *numCounters,
                                              GLint *maxActiveCounters,
                                              GLsizei counterSize,
                                              GLuint *counters);
    void (APIENTRY *glGetPerfMonitorGroupString)(GLuint group, GLsizei bufSize,
                                                 GLsizei *length,
                                                 GLchar *groupString);
    void (APIENTRY *glGetPerfMonitorGroups)(GLint *numGroups, GLsizei groupsSize,
                                            GLuint *groups);
    void (APIENTRY *glSelectPerfMonitorCounters)(GLuint monitor,
                                                 GLboolean enable, GLuint group,
                                                 GLint numCounters,
                                                 GLuint *counterList);

    // Construction
    AMDPerformanceMonitor();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDPerformanceMonitor

// GL_AMD_sample_positions extension API specification.
struct AMDSamplePositions {
    // Public Members
    void (APIENTRY *glSetMultisamplefv)(GLenum pname, GLuint index,
                                        const GLfloat *val);

    // Construction
    AMDSamplePositions();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDSamplePositions

// GL_AMD_sparse_texture extension API specification.
struct AMDSparseTexture {
    // Public Members
    void (APIENTRY *glTexStorageSparse)(GLenum target, GLenum internalFormat,
                                        GLsizei width, GLsizei height,
                                        GLsizei depth, GLsizei layers,
                                        GLbitfield flags);
    void (APIENTRY *glTextureStorageSparse)(GLuint texture, GLenum target,
                                            GLenum internalFormat, GLsizei width,
                                            GLsizei height, GLsizei depth,
                                            GLsizei layers, GLbitfield flags);

    // Construction
    AMDSparseTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDSparseTexture

// GL_AMD_stencil_operation_extended extension API specification.
struct AMDStencilOperationExtended {
    // Public Members
    void (APIENTRY *glStencilOpValue)(GLenum face, GLuint value);

    // Construction
    AMDStencilOperationExtended();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDStencilOperationExtended

// GL_AMD_vertex_shader_tessellator extension API specification.
struct AMDVertexShaderTessellator {
    // Public Members
    void (APIENTRY *glTessellationFactor)(GLfloat factor);
    void (APIENTRY *glTessellationMode)(GLenum mode);

    // Construction
    AMDVertexShaderTessellator();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct AMDVertexShaderTessellator

// GL_APPLE_element_array extension API specification.
struct APPLEElementArray {
    // Public Members
    void (APIENTRY *glDrawElementArray)(GLenum mode, GLint first, GLsizei count);
    void (APIENTRY *glDrawRangeElementArray)(GLenum mode, GLuint start,
                                             GLuint end, GLint first,
                                             GLsizei count);
    void (APIENTRY *glElementPointer)(GLenum type, const void *pointer);
    void (APIENTRY *glMultiDrawElementArray)(GLenum mode, const GLint *first,
                                             const GLsizei *count,
                                             GLsizei primcount);
    void (APIENTRY *glMultiDrawRangeElementArray)(GLenum mode, GLuint start,
                                                  GLuint end, const GLint *first,
                                                  const GLsizei *count,
                                                  GLsizei primcount);

    // Construction
    APPLEElementArray();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct APPLEElementArray

// GL_APPLE_fence extension API specification.
struct APPLEFence {
    // Public Members
    void (APIENTRY *glDeleteFences)(GLsizei n, const GLuint *fences);
    void (APIENTRY *glFinishFence)(GLuint fence);
    void (APIENTRY *glFinishObject)(GLenum object, GLint name);
    void (APIENTRY *glGenFences)(GLsizei n, GLuint *fences);
    GLboolean (APIENTRY *glIsFence)(GLuint fence);
    void (APIENTRY *glSetFence)(GLuint fence);
    GLboolean (APIENTRY *glTestFence)(GLuint fence);
    GLboolean (APIENTRY *glTestObject)(GLenum object, GLuint name);

    // Construction
    APPLEFence();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct APPLEFence

// GL_APPLE_flush_buffer_range extension API specification.
struct APPLEFlushBufferRange {
    // Public Members
    void (APIENTRY *glBufferParameteri)(GLenum target, GLenum pname, GLint param);
    void (APIENTRY *glFlushMappedBufferRange)(GLenum target, GLintptr offset,
                                              GLsizeiptr size);

    // Construction
    APPLEFlushBufferRange();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct APPLEFlushBufferRange

// GL_APPLE_object_purgeable extension API specification.
struct APPLEObjectPurgeable {
    // Public Members
    void (APIENTRY *glGetObjectParameteriv)(GLenum objectType, GLuint name,
                                            GLenum pname, GLint *params);
    GLenum (APIENTRY *glObjectPurgeable)(GLenum objectType, GLuint name,
                                         GLenum option);
    GLenum (APIENTRY *glObjectUnpurgeable)(GLenum objectType, GLuint name,
                                           GLenum option);

    // Construction
    APPLEObjectPurgeable();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct APPLEObjectPurgeable

// GL_APPLE_texture_range extension API specification.
struct APPLETextureRange {
    // Public Members
    void (APIENTRY *glGetTexParameterPointerv)(GLenum target, GLenum pname,
                                               void **params);
    void (APIENTRY *glTextureRange)(GLenum target, GLsizei length,
                                    const void *pointer);

    // Construction
    APPLETextureRange();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct APPLETextureRange

// GL_APPLE_vertex_array_object extension API specification.
struct APPLEVertexArrayObject {
    // Public Members
    void (APIENTRY *glBindVertexArray)(GLuint array);
    void (APIENTRY *glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);
    void (APIENTRY *glGenVertexArrays)(GLsizei n, GLuint *arrays);
    GLboolean (APIENTRY *glIsVertexArray)(GLuint array);

    // Construction
    APPLEVertexArrayObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct APPLEVertexArrayObject

// GL_APPLE_vertex_array_range extension API specification.
struct APPLEVertexArrayRange {
    // Public Members
    void (APIENTRY *glFlushVertexArrayRange)(GLsizei length, void *pointer);
    void (APIENTRY *glVertexArrayParameteri)(GLenum pname, GLint param);
    void (APIENTRY *glVertexArrayRange)(GLsizei length, void *pointer);

    // Construction
    APPLEVertexArrayRange();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct APPLEVertexArrayRange

// GL_APPLE_vertex_program_evaluators extension API specification.
struct APPLEVertexProgramEvaluators {
    // Public Members
    void (APIENTRY *glDisableVertexAttrib)(GLuint index, GLenum pname);
    void (APIENTRY *glEnableVertexAttrib)(GLuint index, GLenum pname);
    GLboolean (APIENTRY *glIsVertexAttribEnabled)(GLuint index, GLenum pname);
    void (APIENTRY *glMapVertexAttrib1d)(GLuint index, GLuint size, GLdouble u1,
                                         GLdouble u2, GLint stride, GLint order,
                                         const GLdouble *points);
    void (APIENTRY *glMapVertexAttrib1f)(GLuint index, GLuint size, GLfloat u1,
                                         GLfloat u2, GLint stride, GLint order,
                                         const GLfloat *points);
    void (APIENTRY *glMapVertexAttrib2d)(GLuint index, GLuint size, GLdouble u1,
                                         GLdouble u2, GLint ustride,
                                         GLint uorder, GLdouble v1, GLdouble v2,
                                         GLint vstride, GLint vorder,
                                         const GLdouble *points);
    void (APIENTRY *glMapVertexAttrib2f)(GLuint index, GLuint size, GLfloat u1,
                                         GLfloat u2, GLint ustride, GLint uorder,
                                         GLfloat v1, GLfloat v2, GLint vstride,
                                         GLint vorder, const GLfloat *points);

    // Construction
    APPLEVertexProgramEvaluators();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct APPLEVertexProgramEvaluators

// GL_ARB_ES3_2_compatibility extension API specification.
struct ARBES32Compatibility {
    // Public Members
    void (APIENTRY *glPrimitiveBoundingBox)(GLfloat minX, GLfloat minY,
                                            GLfloat minZ, GLfloat minW,
                                            GLfloat maxX, GLfloat maxY,
                                            GLfloat maxZ, GLfloat maxW);

    // Construction
    ARBES32Compatibility();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBES32Compatibility

// GL_ARB_bindless_texture extension API specification.
struct ARBBindlessTexture {
    // Public Members
    GLuint64 (APIENTRY *glGetImageHandle)(GLuint texture, GLint level,
                                          GLboolean layered, GLint layer,
                                          GLenum format);
    GLuint64 (APIENTRY *glGetTextureHandle)(GLuint texture);
    GLuint64 (APIENTRY *glGetTextureSamplerHandle)(GLuint texture,
                                                   GLuint sampler);
    void (APIENTRY *glGetVertexAttribLui64v)(GLuint index, GLenum pname,
                                             GLuint64EXT *params);
    GLboolean (APIENTRY *glIsImageHandleResident)(GLuint64 handle);
    GLboolean (APIENTRY *glIsTextureHandleResident)(GLuint64 handle);
    void (APIENTRY *glMakeImageHandleNonResident)(GLuint64 handle);
    void (APIENTRY *glMakeImageHandleResident)(GLuint64 handle, GLenum access);
    void (APIENTRY *glMakeTextureHandleNonResident)(GLuint64 handle);
    void (APIENTRY *glMakeTextureHandleResident)(GLuint64 handle);
    void (APIENTRY *glProgramUniformHandleui64)(GLuint program, GLint location,
                                                GLuint64 value);
    void (APIENTRY *glProgramUniformHandleui64v)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 const GLuint64 *values);
    void (APIENTRY *glUniformHandleui64)(GLint location, GLuint64 value);
    void (APIENTRY *glUniformHandleui64v)(GLint location, GLsizei count,
                                          const GLuint64 *value);
    void (APIENTRY *glVertexAttribL1ui64)(GLuint index, GLuint64EXT x);
    void (APIENTRY *glVertexAttribL1ui64v)(GLuint index, const GLuint64EXT *v);

    // Construction
    ARBBindlessTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBBindlessTexture

// GL_ARB_cl_event extension API specification.
struct ARBClEvent {
    // Public Members
    GLsync (APIENTRY *glCreateSyncFromCLevent)(struct _cl_context *context,
                                               struct _cl_event *event,
                                               GLbitfield flags);

    // Construction
    ARBClEvent();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBClEvent

// GL_ARB_color_buffer_float extension API specification.
struct ARBColorBufferFloat {
    // Public Members
    void (APIENTRY *glClampColor)(GLenum target, GLenum clamp);

    // Construction
    ARBColorBufferFloat();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBColorBufferFloat

// GL_ARB_compute_variable_group_size extension API specification.
struct ARBComputeVariableGroupSize {
    // Public Members
    void (APIENTRY *glDispatchComputeGroupSize)(GLuint num_groups_x,
                                                GLuint num_groups_y,
                                                GLuint num_groups_z,
                                                GLuint group_size_x,
                                                GLuint group_size_y,
                                                GLuint group_size_z);

    // Construction
    ARBComputeVariableGroupSize();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBComputeVariableGroupSize

// GL_ARB_debug_output extension API specification.
struct ARBDebugOutput {
    // Public Members
    void (APIENTRY *glDebugMessageCallback)(GLDEBUGPROCARB callback,
                                            const void *userParam);
    void (APIENTRY *glDebugMessageControl)(GLenum source, GLenum type,
                                           GLenum severity, GLsizei count,
                                           const GLuint *ids, GLboolean enabled);
    void (APIENTRY *glDebugMessageInsert)(GLenum source, GLenum type, GLuint id,
                                          GLenum severity, GLsizei length,
                                          const GLchar *buf);
    GLuint (APIENTRY *glGetDebugMessageLog)(GLuint count, GLsizei bufSize,
                                            GLenum *sources, GLenum *types,
                                            GLuint *ids, GLenum *severities,
                                            GLsizei *lengths, GLchar *messageLog);

    // Construction
    ARBDebugOutput();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBDebugOutput

// GL_ARB_draw_buffers extension API specification.
struct ARBDrawBuffers {
    // Public Members
    void (APIENTRY *glDrawBuffers)(GLsizei n, const GLenum *bufs);

    // Construction
    ARBDrawBuffers();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBDrawBuffers

// GL_ARB_draw_buffers_blend extension API specification.
struct ARBDrawBuffersBlend {
    // Public Members
    void (APIENTRY *glBlendEquationSeparatei)(GLuint buf, GLenum modeRGB,
                                              GLenum modeAlpha);
    void (APIENTRY *glBlendEquationi)(GLuint buf, GLenum mode);
    void (APIENTRY *glBlendFuncSeparatei)(GLuint buf, GLenum srcRGB,
                                          GLenum dstRGB, GLenum srcAlpha,
                                          GLenum dstAlpha);
    void (APIENTRY *glBlendFunci)(GLuint buf, GLenum src, GLenum dst);

    // Construction
    ARBDrawBuffersBlend();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBDrawBuffersBlend

// GL_ARB_draw_instanced extension API specification.
struct ARBDrawInstanced {
    // Public Members
    void (APIENTRY *glDrawArraysInstanced)(GLenum mode, GLint first,
                                           GLsizei count, GLsizei primcount);
    void (APIENTRY *glDrawElementsInstanced)(GLenum mode, GLsizei count,
                                             GLenum type, const void *indices,
                                             GLsizei primcount);

    // Construction
    ARBDrawInstanced();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBDrawInstanced

// GL_ARB_fragment_program extension API specification.
struct ARBFragmentProgram {
    // Public Members
    void (APIENTRY *glBindProgram)(GLenum target, GLuint program);
    void (APIENTRY *glDeletePrograms)(GLsizei n, const GLuint *programs);
    void (APIENTRY *glGenPrograms)(GLsizei n, GLuint *programs);
    void (APIENTRY *glGetProgramEnvParameterdv)(GLenum target, GLuint index,
                                                GLdouble *params);
    void (APIENTRY *glGetProgramEnvParameterfv)(GLenum target, GLuint index,
                                                GLfloat *params);
    void (APIENTRY *glGetProgramLocalParameterdv)(GLenum target, GLuint index,
                                                  GLdouble *params);
    void (APIENTRY *glGetProgramLocalParameterfv)(GLenum target, GLuint index,
                                                  GLfloat *params);
    void (APIENTRY *glGetProgramString)(GLenum target, GLenum pname,
                                        void *string);
    void (APIENTRY *glGetProgramiv)(GLenum target, GLenum pname, GLint *params);
    GLboolean (APIENTRY *glIsProgram)(GLuint program);
    void (APIENTRY *glProgramEnvParameter4d)(GLenum target, GLuint index,
                                             GLdouble x, GLdouble y, GLdouble z,
                                             GLdouble w);
    void (APIENTRY *glProgramEnvParameter4dv)(GLenum target, GLuint index,
                                              const GLdouble *params);
    void (APIENTRY *glProgramEnvParameter4f)(GLenum target, GLuint index,
                                             GLfloat x, GLfloat y, GLfloat z,
                                             GLfloat w);
    void (APIENTRY *glProgramEnvParameter4fv)(GLenum target, GLuint index,
                                              const GLfloat *params);
    void (APIENTRY *glProgramLocalParameter4d)(GLenum target, GLuint index,
                                               GLdouble x, GLdouble y,
                                               GLdouble z, GLdouble w);
    void (APIENTRY *glProgramLocalParameter4dv)(GLenum target, GLuint index,
                                                const GLdouble *params);
    void (APIENTRY *glProgramLocalParameter4f)(GLenum target, GLuint index,
                                               GLfloat x, GLfloat y, GLfloat z,
                                               GLfloat w);
    void (APIENTRY *glProgramLocalParameter4fv)(GLenum target, GLuint index,
                                                const GLfloat *params);
    void (APIENTRY *glProgramString)(GLenum target, GLenum format, GLsizei len,
                                     const void *string);

    // Construction
    ARBFragmentProgram();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBFragmentProgram

// GL_ARB_geometry_shader4 extension API specification.
struct ARBGeometrySHADER4 {
    // Public Members
    void (APIENTRY *glFramebufferTexture)(GLenum target, GLenum attachment,
                                          GLuint texture, GLint level);
    void (APIENTRY *glFramebufferTextureFace)(GLenum target, GLenum attachment,
                                              GLuint texture, GLint level,
                                              GLenum face);
    void (APIENTRY *glFramebufferTextureLayer)(GLenum target, GLenum attachment,
                                               GLuint texture, GLint level,
                                               GLint layer);
    void (APIENTRY *glProgramParameteri)(GLuint program, GLenum pname,
                                         GLint value);

    // Construction
    ARBGeometrySHADER4();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBGeometrySHADER4

// GL_ARB_gl_spirv extension API specification.
struct ARBGlSpirv {
    // Public Members
    void (APIENTRY *glSpecializeShader)(GLuint shader, const GLchar *pEntryPoint,
                                        GLuint numSpecializationConstants,
                                        const GLuint *pConstantIndex,
                                        const GLuint *pConstantValue);

    // Construction
    ARBGlSpirv();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBGlSpirv

// GL_ARB_gpu_shader_int64 extension API specification.
struct ARBGPUShaderInt64 {
    // Public Members
    void (APIENTRY *glGetUniformi64v)(GLuint program, GLint location,
                                      GLint64 *params);
    void (APIENTRY *glGetUniformui64v)(GLuint program, GLint location,
                                       GLuint64 *params);
    void (APIENTRY *glGetnUniformi64v)(GLuint program, GLint location,
                                       GLsizei bufSize, GLint64 *params);
    void (APIENTRY *glGetnUniformui64v)(GLuint program, GLint location,
                                        GLsizei bufSize, GLuint64 *params);
    void (APIENTRY *glProgramUniform1i64)(GLuint program, GLint location,
                                          GLint64 x);
    void (APIENTRY *glProgramUniform1i64v)(GLuint program, GLint location,
                                           GLsizei count, const GLint64 *value);
    void (APIENTRY *glProgramUniform1ui64)(GLuint program, GLint location,
                                           GLuint64 x);
    void (APIENTRY *glProgramUniform1ui64v)(GLuint program, GLint location,
                                            GLsizei count, const GLuint64 *value);
    void (APIENTRY *glProgramUniform2i64)(GLuint program, GLint location,
                                          GLint64 x, GLint64 y);
    void (APIENTRY *glProgramUniform2i64v)(GLuint program, GLint location,
                                           GLsizei count, const GLint64 *value);
    void (APIENTRY *glProgramUniform2ui64)(GLuint program, GLint location,
                                           GLuint64 x, GLuint64 y);
    void (APIENTRY *glProgramUniform2ui64v)(GLuint program, GLint location,
                                            GLsizei count, const GLuint64 *value);
    void (APIENTRY *glProgramUniform3i64)(GLuint program, GLint location,
                                          GLint64 x, GLint64 y, GLint64 z);
    void (APIENTRY *glProgramUniform3i64v)(GLuint program, GLint location,
                                           GLsizei count, const GLint64 *value);
    void (APIENTRY *glProgramUniform3ui64)(GLuint program, GLint location,
                                           GLuint64 x, GLuint64 y, GLuint64 z);
    void (APIENTRY *glProgramUniform3ui64v)(GLuint program, GLint location,
                                            GLsizei count, const GLuint64 *value);
    void (APIENTRY *glProgramUniform4i64)(GLuint program, GLint location,
                                          GLint64 x, GLint64 y, GLint64 z,
                                          GLint64 w);
    void (APIENTRY *glProgramUniform4i64v)(GLuint program, GLint location,
                                           GLsizei count, const GLint64 *value);
    void (APIENTRY *glProgramUniform4ui64)(GLuint program, GLint location,
                                           GLuint64 x, GLuint64 y, GLuint64 z,
                                           GLuint64 w);
    void (APIENTRY *glProgramUniform4ui64v)(GLuint program, GLint location,
                                            GLsizei count, const GLuint64 *value);
    void (APIENTRY *glUniform1i64)(GLint location, GLint64 x);
    void (APIENTRY *glUniform1i64v)(GLint location, GLsizei count,
                                    const GLint64 *value);
    void (APIENTRY *glUniform1ui64)(GLint location, GLuint64 x);
    void (APIENTRY *glUniform1ui64v)(GLint location, GLsizei count,
                                     const GLuint64 *value);
    void (APIENTRY *glUniform2i64)(GLint location, GLint64 x, GLint64 y);
    void (APIENTRY *glUniform2i64v)(GLint location, GLsizei count,
                                    const GLint64 *value);
    void (APIENTRY *glUniform2ui64)(GLint location, GLuint64 x, GLuint64 y);
    void (APIENTRY *glUniform2ui64v)(GLint location, GLsizei count,
                                     const GLuint64 *value);
    void (APIENTRY *glUniform3i64)(GLint location, GLint64 x, GLint64 y,
                                   GLint64 z);
    void (APIENTRY *glUniform3i64v)(GLint location, GLsizei count,
                                    const GLint64 *value);
    void (APIENTRY *glUniform3ui64)(GLint location, GLuint64 x, GLuint64 y,
                                    GLuint64 z);
    void (APIENTRY *glUniform3ui64v)(GLint location, GLsizei count,
                                     const GLuint64 *value);
    void (APIENTRY *glUniform4i64)(GLint location, GLint64 x, GLint64 y,
                                   GLint64 z, GLint64 w);
    void (APIENTRY *glUniform4i64v)(GLint location, GLsizei count,
                                    const GLint64 *value);
    void (APIENTRY *glUniform4ui64)(GLint location, GLuint64 x, GLuint64 y,
                                    GLuint64 z, GLuint64 w);
    void (APIENTRY *glUniform4ui64v)(GLint location, GLsizei count,
                                     const GLuint64 *value);

    // Construction
    ARBGPUShaderInt64();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBGPUShaderInt64

// GL_ARB_indirect_parameters extension API specification.
struct ARBIndirectParameters {
    // Public Members
    void (APIENTRY *glMultiDrawArraysIndirectCount)(GLenum mode,
                                                    const void *indirect,
                                                    GLintptr drawcount,
                                                    GLsizei maxdrawcount,
                                                    GLsizei stride);
    void (APIENTRY *glMultiDrawElementsIndirectCount)(GLenum mode, GLenum type,
                                                      const void *indirect,
                                                      GLintptr drawcount,
                                                      GLsizei maxdrawcount,
                                                      GLsizei stride);

    // Construction
    ARBIndirectParameters();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBIndirectParameters

// GL_ARB_instanced_arrays extension API specification.
struct ARBInstancedArrays {
    // Public Members
    void (APIENTRY *glVertexAttribDivisor)(GLuint index, GLuint divisor);

    // Construction
    ARBInstancedArrays();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBInstancedArrays

// GL_ARB_matrix_palette extension API specification.
struct ARBMatrixPalette {
    // Public Members
    void (APIENTRY *glCurrentPaletteMatrix)(GLint index);
    void (APIENTRY *glMatrixIndexPointer)(GLint size, GLenum type,
                                          GLsizei stride, const void *pointer);
    void (APIENTRY *glMatrixIndexubv)(GLint size, const GLubyte *indices);
    void (APIENTRY *glMatrixIndexuiv)(GLint size, const GLuint *indices);
    void (APIENTRY *glMatrixIndexusv)(GLint size, const GLushort *indices);

    // Construction
    ARBMatrixPalette();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBMatrixPalette

// GL_ARB_multisample extension API specification.
struct ARBMultisample {
    // Public Members
    void (APIENTRY *glSampleCoverage)(GLfloat value, GLboolean invert);

    // Construction
    ARBMultisample();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBMultisample

// GL_ARB_multitexture extension API specification.
struct ARBMultitexture {
    // Public Members
    void (APIENTRY *glActiveTexture)(GLenum texture);
    void (APIENTRY *glClientActiveTexture)(GLenum texture);
    void (APIENTRY *glMultiTexCoord1d)(GLenum target, GLdouble s);
    void (APIENTRY *glMultiTexCoord1dv)(GLenum target, const GLdouble *v);
    void (APIENTRY *glMultiTexCoord1f)(GLenum target, GLfloat s);
    void (APIENTRY *glMultiTexCoord1fv)(GLenum target, const GLfloat *v);
    void (APIENTRY *glMultiTexCoord1i)(GLenum target, GLint s);
    void (APIENTRY *glMultiTexCoord1iv)(GLenum target, const GLint *v);
    void (APIENTRY *glMultiTexCoord1s)(GLenum target, GLshort s);
    void (APIENTRY *glMultiTexCoord1sv)(GLenum target, const GLshort *v);
    void (APIENTRY *glMultiTexCoord2d)(GLenum target, GLdouble s, GLdouble t);
    void (APIENTRY *glMultiTexCoord2dv)(GLenum target, const GLdouble *v);
    void (APIENTRY *glMultiTexCoord2f)(GLenum target, GLfloat s, GLfloat t);
    void (APIENTRY *glMultiTexCoord2fv)(GLenum target, const GLfloat *v);
    void (APIENTRY *glMultiTexCoord2i)(GLenum target, GLint s, GLint t);
    void (APIENTRY *glMultiTexCoord2iv)(GLenum target, const GLint *v);
    void (APIENTRY *glMultiTexCoord2s)(GLenum target, GLshort s, GLshort t);
    void (APIENTRY *glMultiTexCoord2sv)(GLenum target, const GLshort *v);
    void (APIENTRY *glMultiTexCoord3d)(GLenum target, GLdouble s, GLdouble t,
                                       GLdouble r);
    void (APIENTRY *glMultiTexCoord3dv)(GLenum target, const GLdouble *v);
    void (APIENTRY *glMultiTexCoord3f)(GLenum target, GLfloat s, GLfloat t,
                                       GLfloat r);
    void (APIENTRY *glMultiTexCoord3fv)(GLenum target, const GLfloat *v);
    void (APIENTRY *glMultiTexCoord3i)(GLenum target, GLint s, GLint t, GLint r);
    void (APIENTRY *glMultiTexCoord3iv)(GLenum target, const GLint *v);
    void (APIENTRY *glMultiTexCoord3s)(GLenum target, GLshort s, GLshort t,
                                       GLshort r);
    void (APIENTRY *glMultiTexCoord3sv)(GLenum target, const GLshort *v);
    void (APIENTRY *glMultiTexCoord4d)(GLenum target, GLdouble s, GLdouble t,
                                       GLdouble r, GLdouble q);
    void (APIENTRY *glMultiTexCoord4dv)(GLenum target, const GLdouble *v);
    void (APIENTRY *glMultiTexCoord4f)(GLenum target, GLfloat s, GLfloat t,
                                       GLfloat r, GLfloat q);
    void (APIENTRY *glMultiTexCoord4fv)(GLenum target, const GLfloat *v);
    void (APIENTRY *glMultiTexCoord4i)(GLenum target, GLint s, GLint t, GLint r,
                                       GLint q);
    void (APIENTRY *glMultiTexCoord4iv)(GLenum target, const GLint *v);
    void (APIENTRY *glMultiTexCoord4s)(GLenum target, GLshort s, GLshort t,
                                       GLshort r, GLshort q);
    void (APIENTRY *glMultiTexCoord4sv)(GLenum target, const GLshort *v);

    // Construction
    ARBMultitexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBMultitexture

// GL_ARB_occlusion_query extension API specification.
struct ARBOcclusionQuery {
    // Public Members
    void (APIENTRY *glBeginQuery)(GLenum target, GLuint id);
    void (APIENTRY *glDeleteQueries)(GLsizei n, const GLuint *ids);
    void (APIENTRY *glEndQuery)(GLenum target);
    void (APIENTRY *glGenQueries)(GLsizei n, GLuint *ids);
    void (APIENTRY *glGetQueryObjectiv)(GLuint id, GLenum pname, GLint *params);
    void (APIENTRY *glGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint *params);
    void (APIENTRY *glGetQueryiv)(GLenum target, GLenum pname, GLint *params);
    GLboolean (APIENTRY *glIsQuery)(GLuint id);

    // Construction
    ARBOcclusionQuery();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBOcclusionQuery

// GL_ARB_parallel_shader_compile extension API specification.
struct ARBParallelShaderCompile {
    // Public Members
    void (APIENTRY *glMaxShaderCompilerThreads)(GLuint count);

    // Construction
    ARBParallelShaderCompile();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBParallelShaderCompile

// GL_ARB_point_parameters extension API specification.
struct ARBPointParameters {
    // Public Members
    void (APIENTRY *glPointParameterf)(GLenum pname, GLfloat param);
    void (APIENTRY *glPointParameterfv)(GLenum pname, const GLfloat *params);

    // Construction
    ARBPointParameters();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBPointParameters

// GL_ARB_robustness extension API specification.
struct ARBRobustness {
    // Public Members
    GLenum (APIENTRY *glGetGraphicsResetStatus)();
    void (APIENTRY *glGetnCompressedTexImage)(GLenum target, GLint lod,
                                              GLsizei bufSize, void *img);
    void (APIENTRY *glGetnTexImage)(GLenum target, GLint level, GLenum format,
                                    GLenum type, GLsizei bufSize, void *img);
    void (APIENTRY *glGetnUniformdv)(GLuint program, GLint location,
                                     GLsizei bufSize, GLdouble *params);
    void (APIENTRY *glGetnUniformfv)(GLuint program, GLint location,
                                     GLsizei bufSize, GLfloat *params);
    void (APIENTRY *glGetnUniformiv)(GLuint program, GLint location,
                                     GLsizei bufSize, GLint *params);
    void (APIENTRY *glGetnUniformuiv)(GLuint program, GLint location,
                                      GLsizei bufSize, GLuint *params);
    void (APIENTRY *glReadnPixels)(GLint x, GLint y, GLsizei width,
                                   GLsizei height, GLenum format, GLenum type,
                                   GLsizei bufSize, void *data);

    // Construction
    ARBRobustness();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBRobustness

// GL_ARB_sample_locations extension API specification.
struct ARBSampleLocations {
    // Public Members
    void (APIENTRY *glEvaluateDepthValues)();
    void (APIENTRY *glFramebufferSampleLocationsfv)(GLenum target, GLuint start,
                                                    GLsizei count,
                                                    const GLfloat *v);
    void (APIENTRY *glNamedFramebufferSampleLocationsfv)(GLuint framebuffer,
                                                         GLuint start,
                                                         GLsizei count,
                                                         const GLfloat *v);

    // Construction
    ARBSampleLocations();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBSampleLocations

// GL_ARB_sample_shading extension API specification.
struct ARBSampleShading {
    // Public Members
    void (APIENTRY *glMinSampleShading)(GLfloat value);

    // Construction
    ARBSampleShading();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBSampleShading

// GL_ARB_shader_objects extension API specification.
struct ARBShaderObjects {
    // Public Members
    void (APIENTRY *glAttachObject)(GLhandleARB containerObj, GLhandleARB obj);
    void (APIENTRY *glCompileShader)(GLhandleARB shaderObj);
    GLhandleARB (APIENTRY *glCreateProgramObject)();
    GLhandleARB (APIENTRY *glCreateShaderObject)(GLenum shaderType);
    void (APIENTRY *glDeleteObject)(GLhandleARB obj);
    void (APIENTRY *glDetachObject)(GLhandleARB containerObj,
                                    GLhandleARB attachedObj);
    void (APIENTRY *glGetActiveUniform)(GLhandleARB programObj, GLuint index,
                                        GLsizei maxLength, GLsizei *length,
                                        GLint *size, GLenum *type,
                                        GLcharARB *name);
    void (APIENTRY *glGetAttachedObjects)(GLhandleARB containerObj,
                                          GLsizei maxCount, GLsizei *count,
                                          GLhandleARB *obj);
    GLhandleARB (APIENTRY *glGetHandle)(GLenum pname);
    void (APIENTRY *glGetInfoLog)(GLhandleARB obj, GLsizei maxLength,
                                  GLsizei *length, GLcharARB *infoLog);
    void (APIENTRY *glGetObjectParameterfv)(GLhandleARB obj, GLenum pname,
                                            GLfloat *params);
    void (APIENTRY *glGetObjectParameteriv)(GLhandleARB obj, GLenum pname,
                                            GLint *params);
    void (APIENTRY *glGetShaderSource)(GLhandleARB obj, GLsizei maxLength,
                                       GLsizei *length, GLcharARB *source);
    GLint (APIENTRY *glGetUniformLocation)(GLhandleARB programObj,
                                           const GLcharARB *name);
    void (APIENTRY *glGetUniformfv)(GLhandleARB programObj, GLint location,
                                    GLfloat *params);
    void (APIENTRY *glGetUniformiv)(GLhandleARB programObj, GLint location,
                                    GLint *params);
    void (APIENTRY *glLinkProgram)(GLhandleARB programObj);
    void (APIENTRY *glShaderSource)(GLhandleARB shaderObj, GLsizei count,
                                    const GLcharARB **string,
                                    const GLint *length);
    void (APIENTRY *glUniform1f)(GLint location, GLfloat v0);
    void (APIENTRY *glUniform1fv)(GLint location, GLsizei count,
                                  const GLfloat *value);
    void (APIENTRY *glUniform1i)(GLint location, GLint v0);
    void (APIENTRY *glUniform1iv)(GLint location, GLsizei count,
                                  const GLint *value);
    void (APIENTRY *glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
    void (APIENTRY *glUniform2fv)(GLint location, GLsizei count,
                                  const GLfloat *value);
    void (APIENTRY *glUniform2i)(GLint location, GLint v0, GLint v1);
    void (APIENTRY *glUniform2iv)(GLint location, GLsizei count,
                                  const GLint *value);
    void (APIENTRY *glUniform3f)(GLint location, GLfloat v0, GLfloat v1,
                                 GLfloat v2);
    void (APIENTRY *glUniform3fv)(GLint location, GLsizei count,
                                  const GLfloat *value);
    void (APIENTRY *glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
    void (APIENTRY *glUniform3iv)(GLint location, GLsizei count,
                                  const GLint *value);
    void (APIENTRY *glUniform4f)(GLint location, GLfloat v0, GLfloat v1,
                                 GLfloat v2, GLfloat v3);
    void (APIENTRY *glUniform4fv)(GLint location, GLsizei count,
                                  const GLfloat *value);
    void (APIENTRY *glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2,
                                 GLint v3);
    void (APIENTRY *glUniform4iv)(GLint location, GLsizei count,
                                  const GLint *value);
    void (APIENTRY *glUniformMatrix2fv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLfloat *value);
    void (APIENTRY *glUniformMatrix3fv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLfloat *value);
    void (APIENTRY *glUniformMatrix4fv)(GLint location, GLsizei count,
                                        GLboolean transpose,
                                        const GLfloat *value);
    void (APIENTRY *glUseProgramObject)(GLhandleARB programObj);
    void (APIENTRY *glValidateProgram)(GLhandleARB programObj);

    // Construction
    ARBShaderObjects();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBShaderObjects

// GL_ARB_shading_language_include extension API specification.
struct ARBShadingLanguageInclude {
    // Public Members
    void (APIENTRY *glCompileShaderInclude)(GLuint shader, GLsizei count,
                                            const GLchar *const*path,
                                            const GLint *length);
    void (APIENTRY *glDeleteNamedString)(GLint namelen, const GLchar *name);
    void (APIENTRY *glGetNamedString)(GLint namelen, const GLchar *name,
                                      GLsizei bufSize, GLint *stringlen,
                                      GLchar *string);
    void (APIENTRY *glGetNamedStringiv)(GLint namelen, const GLchar *name,
                                        GLenum pname, GLint *params);
    GLboolean (APIENTRY *glIsNamedString)(GLint namelen, const GLchar *name);
    void (APIENTRY *glNamedString)(GLenum type, GLint namelen,
                                   const GLchar *name, GLint stringlen,
                                   const GLchar *string);

    // Construction
    ARBShadingLanguageInclude();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBShadingLanguageInclude

// GL_ARB_sparse_buffer extension API specification.
struct ARBSparseBuffer {
    // Public Members
    void (APIENTRY *glBufferPageCommitment)(GLenum target, GLintptr offset,
                                            GLsizeiptr size, GLboolean commit);
    void (APIENTRY *glNamedBufferPageCommitmentARB)(GLuint buffer,
                                                    GLintptr offset,
                                                    GLsizeiptr size,
                                                    GLboolean commit);
    void (APIENTRY *glNamedBufferPageCommitmentEXT)(GLuint buffer,
                                                    GLintptr offset,
                                                    GLsizeiptr size,
                                                    GLboolean commit);

    // Construction
    ARBSparseBuffer();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBSparseBuffer

// GL_ARB_sparse_texture extension API specification.
struct ARBSparseTexture {
    // Public Members
    void (APIENTRY *glTexPageCommitment)(GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLsizei width,
                                         GLsizei height, GLsizei depth,
                                         GLboolean commit);

    // Construction
    ARBSparseTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBSparseTexture

// GL_ARB_texture_buffer_object extension API specification.
struct ARBTextureBufferObject {
    // Public Members
    void (APIENTRY *glTexBuffer)(GLenum target, GLenum internalformat,
                                 GLuint buffer);

    // Construction
    ARBTextureBufferObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBTextureBufferObject

// GL_ARB_texture_compression extension API specification.
struct ARBTextureCompression {
    // Public Members
    void (APIENTRY *glCompressedTexImage1D)(GLenum target, GLint level,
                                            GLenum internalformat, GLsizei width,
                                            GLint border, GLsizei imageSize,
                                            const void *data);
    void (APIENTRY *glCompressedTexImage2D)(GLenum target, GLint level,
                                            GLenum internalformat, GLsizei width,
                                            GLsizei height, GLint border,
                                            GLsizei imageSize, const void *data);
    void (APIENTRY *glCompressedTexImage3D)(GLenum target, GLint level,
                                            GLenum internalformat, GLsizei width,
                                            GLsizei height, GLsizei depth,
                                            GLint border, GLsizei imageSize,
                                            const void *data);
    void (APIENTRY *glCompressedTexSubImage1D)(GLenum target, GLint level,
                                               GLint xoffset, GLsizei width,
                                               GLenum format, GLsizei imageSize,
                                               const void *data);
    void (APIENTRY *glCompressedTexSubImage2D)(GLenum target, GLint level,
                                               GLint xoffset, GLint yoffset,
                                               GLsizei width, GLsizei height,
                                               GLenum format, GLsizei imageSize,
                                               const void *data);
    void (APIENTRY *glCompressedTexSubImage3D)(GLenum target, GLint level,
                                               GLint xoffset, GLint yoffset,
                                               GLint zoffset, GLsizei width,
                                               GLsizei height, GLsizei depth,
                                               GLenum format, GLsizei imageSize,
                                               const void *data);
    void (APIENTRY *glGetCompressedTexImage)(GLenum target, GLint level,
                                             void *img);

    // Construction
    ARBTextureCompression();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBTextureCompression

// GL_ARB_transpose_matrix extension API specification.
struct ARBTransposeMatrix {
    // Public Members
    void (APIENTRY *glLoadTransposeMatrixd)(const GLdouble *m);
    void (APIENTRY *glLoadTransposeMatrixf)(const GLfloat *m);
    void (APIENTRY *glMultTransposeMatrixd)(const GLdouble *m);
    void (APIENTRY *glMultTransposeMatrixf)(const GLfloat *m);

    // Construction
    ARBTransposeMatrix();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBTransposeMatrix

// GL_ARB_vertex_blend extension API specification.
struct ARBVertexBlend {
    // Public Members
    void (APIENTRY *glVertexBlend)(GLint count);
    void (APIENTRY *glWeightPointer)(GLint size, GLenum type, GLsizei stride,
                                     const void *pointer);
    void (APIENTRY *glWeightbv)(GLint size, const GLbyte *weights);
    void (APIENTRY *glWeightdv)(GLint size, const GLdouble *weights);
    void (APIENTRY *glWeightfv)(GLint size, const GLfloat *weights);
    void (APIENTRY *glWeightiv)(GLint size, const GLint *weights);
    void (APIENTRY *glWeightsv)(GLint size, const GLshort *weights);
    void (APIENTRY *glWeightubv)(GLint size, const GLubyte *weights);
    void (APIENTRY *glWeightuiv)(GLint size, const GLuint *weights);
    void (APIENTRY *glWeightusv)(GLint size, const GLushort *weights);

    // Construction
    ARBVertexBlend();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBVertexBlend

// GL_ARB_vertex_buffer_object extension API specification.
struct ARBVertexBufferObject {
    // Public Members
    void (APIENTRY *glBindBuffer)(GLenum target, GLuint buffer);
    void (APIENTRY *glBufferData)(GLenum target, GLsizeiptrARB size,
                                  const void *data, GLenum usage);
    void (APIENTRY *glBufferSubData)(GLenum target, GLintptrARB offset,
                                     GLsizeiptrARB size, const void *data);
    void (APIENTRY *glDeleteBuffers)(GLsizei n, const GLuint *buffers);
    void (APIENTRY *glGenBuffers)(GLsizei n, GLuint *buffers);
    void (APIENTRY *glGetBufferParameteriv)(GLenum target, GLenum pname,
                                            GLint *params);
    void (APIENTRY *glGetBufferPointerv)(GLenum target, GLenum pname,
                                         void **params);
    void (APIENTRY *glGetBufferSubData)(GLenum target, GLintptrARB offset,
                                        GLsizeiptrARB size, void *data);
    GLboolean (APIENTRY *glIsBuffer)(GLuint buffer);
    void *(APIENTRY *glMapBuffer)(GLenum target, GLenum access);
    GLboolean (APIENTRY *glUnmapBuffer)(GLenum target);

    // Construction
    ARBVertexBufferObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBVertexBufferObject

// GL_ARB_vertex_program extension API specification.
struct ARBVertexProgram {
    // Public Members
    void (APIENTRY *glDisableVertexAttribArray)(GLuint index);
    void (APIENTRY *glEnableVertexAttribArray)(GLuint index);
    void (APIENTRY *glGetVertexAttribPointerv)(GLuint index, GLenum pname,
                                               void **pointer);
    void (APIENTRY *glGetVertexAttribdv)(GLuint index, GLenum pname,
                                         GLdouble *params);
    void (APIENTRY *glGetVertexAttribfv)(GLuint index, GLenum pname,
                                         GLfloat *params);
    void (APIENTRY *glGetVertexAttribiv)(GLuint index, GLenum pname,
                                         GLint *params);
    void (APIENTRY *glVertexAttrib1d)(GLuint index, GLdouble x);
    void (APIENTRY *glVertexAttrib1dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib1f)(GLuint index, GLfloat x);
    void (APIENTRY *glVertexAttrib1fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib1s)(GLuint index, GLshort x);
    void (APIENTRY *glVertexAttrib1sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
    void (APIENTRY *glVertexAttrib2dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
    void (APIENTRY *glVertexAttrib2fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib2s)(GLuint index, GLshort x, GLshort y);
    void (APIENTRY *glVertexAttrib2sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y,
                                      GLdouble z);
    void (APIENTRY *glVertexAttrib3dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y,
                                      GLfloat z);
    void (APIENTRY *glVertexAttrib3fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib3s)(GLuint index, GLshort x, GLshort y,
                                      GLshort z);
    void (APIENTRY *glVertexAttrib3sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib4Nbv)(GLuint index, const GLbyte *v);
    void (APIENTRY *glVertexAttrib4Niv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttrib4Nsv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y,
                                        GLubyte z, GLubyte w);
    void (APIENTRY *glVertexAttrib4Nubv)(GLuint index, const GLubyte *v);
    void (APIENTRY *glVertexAttrib4Nuiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttrib4Nusv)(GLuint index, const GLushort *v);
    void (APIENTRY *glVertexAttrib4bv)(GLuint index, const GLbyte *v);
    void (APIENTRY *glVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y,
                                      GLdouble z, GLdouble w);
    void (APIENTRY *glVertexAttrib4dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y,
                                      GLfloat z, GLfloat w);
    void (APIENTRY *glVertexAttrib4fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib4iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttrib4s)(GLuint index, GLshort x, GLshort y,
                                      GLshort z, GLshort w);
    void (APIENTRY *glVertexAttrib4sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib4ubv)(GLuint index, const GLubyte *v);
    void (APIENTRY *glVertexAttrib4uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttrib4usv)(GLuint index, const GLushort *v);
    void (APIENTRY *glVertexAttribPointer)(GLuint index, GLint size, GLenum type,
                                           GLboolean normalized, GLsizei stride,
                                           const void *pointer);

    // Construction
    ARBVertexProgram();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBVertexProgram

// GL_ARB_vertex_shader extension API specification.
struct ARBVertexShader {
    // Public Members
    void (APIENTRY *glBindAttribLocation)(GLhandleARB programObj, GLuint index,
                                          const GLcharARB *name);
    void (APIENTRY *glGetActiveAttrib)(GLhandleARB programObj, GLuint index,
                                       GLsizei maxLength, GLsizei *length,
                                       GLint *size, GLenum *type,
                                       GLcharARB *name);
    GLint (APIENTRY *glGetAttribLocation)(GLhandleARB programObj,
                                          const GLcharARB *name);

    // Construction
    ARBVertexShader();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBVertexShader

// GL_ARB_viewport_array extension API specification.
struct ARBViewportArray {
    // Public Members
    void (APIENTRY *glDepthRangeArraydv)(GLuint first, GLsizei count,
                                         const GLdouble *v);
    void (APIENTRY *glDepthRangeIndexedd)(GLuint index, GLdouble n, GLdouble f);

    // Construction
    ARBViewportArray();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBViewportArray

// GL_ARB_window_pos extension API specification.
struct ARBWindowPos {
    // Public Members
    void (APIENTRY *glWindowPos2d)(GLdouble x, GLdouble y);
    void (APIENTRY *glWindowPos2dv)(const GLdouble *v);
    void (APIENTRY *glWindowPos2f)(GLfloat x, GLfloat y);
    void (APIENTRY *glWindowPos2fv)(const GLfloat *v);
    void (APIENTRY *glWindowPos2i)(GLint x, GLint y);
    void (APIENTRY *glWindowPos2iv)(const GLint *v);
    void (APIENTRY *glWindowPos2s)(GLshort x, GLshort y);
    void (APIENTRY *glWindowPos2sv)(const GLshort *v);
    void (APIENTRY *glWindowPos3d)(GLdouble x, GLdouble y, GLdouble z);
    void (APIENTRY *glWindowPos3dv)(const GLdouble *v);
    void (APIENTRY *glWindowPos3f)(GLfloat x, GLfloat y, GLfloat z);
    void (APIENTRY *glWindowPos3fv)(const GLfloat *v);
    void (APIENTRY *glWindowPos3i)(GLint x, GLint y, GLint z);
    void (APIENTRY *glWindowPos3iv)(const GLint *v);
    void (APIENTRY *glWindowPos3s)(GLshort x, GLshort y, GLshort z);
    void (APIENTRY *glWindowPos3sv)(const GLshort *v);

    // Construction
    ARBWindowPos();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ARBWindowPos

// GL_ATI_draw_buffers extension API specification.
struct ATIDrawBuffers {
    // Public Members
    void (APIENTRY *glDrawBuffers)(GLsizei n, const GLenum *bufs);

    // Construction
    ATIDrawBuffers();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIDrawBuffers

// GL_ATI_element_array extension API specification.
struct ATIElementArray {
    // Public Members
    void (APIENTRY *glDrawElementArray)(GLenum mode, GLsizei count);
    void (APIENTRY *glDrawRangeElementArray)(GLenum mode, GLuint start,
                                             GLuint end, GLsizei count);
    void (APIENTRY *glElementPointer)(GLenum type, const void *pointer);

    // Construction
    ATIElementArray();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIElementArray

// GL_ATI_envmap_bumpmap extension API specification.
struct ATIEnvmapBumpmap {
    // Public Members
    void (APIENTRY *glGetTexBumpParameterfv)(GLenum pname, GLfloat *param);
    void (APIENTRY *glGetTexBumpParameteriv)(GLenum pname, GLint *param);
    void (APIENTRY *glTexBumpParameterfv)(GLenum pname, const GLfloat *param);
    void (APIENTRY *glTexBumpParameteriv)(GLenum pname, const GLint *param);

    // Construction
    ATIEnvmapBumpmap();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIEnvmapBumpmap

// GL_ATI_fragment_shader extension API specification.
struct ATIFragmentShader {
    // Public Members
    void (APIENTRY *glAlphaFragmentOp1)(GLenum op, GLuint dst, GLuint dstMod,
                                        GLuint arg1, GLuint arg1Rep,
                                        GLuint arg1Mod);
    void (APIENTRY *glAlphaFragmentOp2)(GLenum op, GLuint dst, GLuint dstMod,
                                        GLuint arg1, GLuint arg1Rep,
                                        GLuint arg1Mod, GLuint arg2,
                                        GLuint arg2Rep, GLuint arg2Mod);
    void (APIENTRY *glAlphaFragmentOp3)(GLenum op, GLuint dst, GLuint dstMod,
                                        GLuint arg1, GLuint arg1Rep,
                                        GLuint arg1Mod, GLuint arg2,
                                        GLuint arg2Rep, GLuint arg2Mod,
                                        GLuint arg3, GLuint arg3Rep,
                                        GLuint arg3Mod);
    void (APIENTRY *glBeginFragmentShader)();
    void (APIENTRY *glBindFragmentShader)(GLuint id);
    void (APIENTRY *glColorFragmentOp1)(GLenum op, GLuint dst, GLuint dstMask,
                                        GLuint dstMod, GLuint arg1,
                                        GLuint arg1Rep, GLuint arg1Mod);
    void (APIENTRY *glColorFragmentOp2)(GLenum op, GLuint dst, GLuint dstMask,
                                        GLuint dstMod, GLuint arg1,
                                        GLuint arg1Rep, GLuint arg1Mod,
                                        GLuint arg2, GLuint arg2Rep,
                                        GLuint arg2Mod);
    void (APIENTRY *glColorFragmentOp3)(GLenum op, GLuint dst, GLuint dstMask,
                                        GLuint dstMod, GLuint arg1,
                                        GLuint arg1Rep, GLuint arg1Mod,
                                        GLuint arg2, GLuint arg2Rep,
                                        GLuint arg2Mod, GLuint arg3,
                                        GLuint arg3Rep, GLuint arg3Mod);
    void (APIENTRY *glDeleteFragmentShader)(GLuint id);
    void (APIENTRY *glEndFragmentShader)();
    GLuint (APIENTRY *glGenFragmentShaders)(GLuint range);
    void (APIENTRY *glPassTexCoord)(GLuint dst, GLuint coord, GLenum swizzle);
    void (APIENTRY *glSampleMap)(GLuint dst, GLuint interp, GLenum swizzle);
    void (APIENTRY *glSetFragmentShaderConstant)(GLuint dst,
                                                 const GLfloat *value);

    // Construction
    ATIFragmentShader();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIFragmentShader

// GL_ATI_map_object_buffer extension API specification.
struct ATIMapObjectBuffer {
    // Public Members
    void *(APIENTRY *glMapObjectBuffer)(GLuint buffer);
    void (APIENTRY *glUnmapObjectBuffer)(GLuint buffer);

    // Construction
    ATIMapObjectBuffer();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIMapObjectBuffer

// GL_ATI_pn_triangles extension API specification.
struct ATIPnTriangles {
    // Public Members
    void (APIENTRY *glPNTrianglesf)(GLenum pname, GLfloat param);
    void (APIENTRY *glPNTrianglesi)(GLenum pname, GLint param);

    // Construction
    ATIPnTriangles();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIPnTriangles

// GL_ATI_separate_stencil extension API specification.
struct ATISeparateStencil {
    // Public Members
    void (APIENTRY *glStencilFuncSeparate)(GLenum frontfunc, GLenum backfunc,
                                           GLint ref, GLuint mask);
    void (APIENTRY *glStencilOpSeparate)(GLenum face, GLenum sfail,
                                         GLenum dpfail, GLenum dppass);

    // Construction
    ATISeparateStencil();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATISeparateStencil

// GL_ATI_vertex_array_object extension API specification.
struct ATIVertexArrayObject {
    // Public Members
    void (APIENTRY *glArrayObject)(GLenum array, GLint size, GLenum type,
                                   GLsizei stride, GLuint buffer, GLuint offset);
    void (APIENTRY *glFreeObjectBuffer)(GLuint buffer);
    void (APIENTRY *glGetArrayObjectfv)(GLenum array, GLenum pname,
                                        GLfloat *params);
    void (APIENTRY *glGetArrayObjectiv)(GLenum array, GLenum pname,
                                        GLint *params);
    void (APIENTRY *glGetObjectBufferfv)(GLuint buffer, GLenum pname,
                                         GLfloat *params);
    void (APIENTRY *glGetObjectBufferiv)(GLuint buffer, GLenum pname,
                                         GLint *params);
    void (APIENTRY *glGetVariantArrayObjectfv)(GLuint id, GLenum pname,
                                               GLfloat *params);
    void (APIENTRY *glGetVariantArrayObjectiv)(GLuint id, GLenum pname,
                                               GLint *params);
    GLboolean (APIENTRY *glIsObjectBuffer)(GLuint buffer);
    GLuint (APIENTRY *glNewObjectBuffer)(GLsizei size, const void *pointer,
                                         GLenum usage);
    void (APIENTRY *glUpdateObjectBuffer)(GLuint buffer, GLuint offset,
                                          GLsizei size, const void *pointer,
                                          GLenum preserve);
    void (APIENTRY *glVariantArrayObject)(GLuint id, GLenum type, GLsizei stride,
                                          GLuint buffer, GLuint offset);

    // Construction
    ATIVertexArrayObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIVertexArrayObject

// GL_ATI_vertex_attrib_array_object extension API specification.
struct ATIVertexAttribArrayObject {
    // Public Members
    void (APIENTRY *glGetVertexAttribArrayObjectfv)(GLuint index, GLenum pname,
                                                    GLfloat *params);
    void (APIENTRY *glGetVertexAttribArrayObjectiv)(GLuint index, GLenum pname,
                                                    GLint *params);
    void (APIENTRY *glVertexAttribArrayObject)(GLuint index, GLint size,
                                               GLenum type, GLboolean normalized,
                                               GLsizei stride, GLuint buffer,
                                               GLuint offset);

    // Construction
    ATIVertexAttribArrayObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIVertexAttribArrayObject

// GL_ATI_vertex_streams extension API specification.
struct ATIVertexStreams {
    // Public Members
    void (APIENTRY *glClientActiveVertexStream)(GLenum stream);
    void (APIENTRY *glNormalStream3b)(GLenum stream, GLbyte nx, GLbyte ny,
                                      GLbyte nz);
    void (APIENTRY *glNormalStream3bv)(GLenum stream, const GLbyte *coords);
    void (APIENTRY *glNormalStream3d)(GLenum stream, GLdouble nx, GLdouble ny,
                                      GLdouble nz);
    void (APIENTRY *glNormalStream3dv)(GLenum stream, const GLdouble *coords);
    void (APIENTRY *glNormalStream3f)(GLenum stream, GLfloat nx, GLfloat ny,
                                      GLfloat nz);
    void (APIENTRY *glNormalStream3fv)(GLenum stream, const GLfloat *coords);
    void (APIENTRY *glNormalStream3i)(GLenum stream, GLint nx, GLint ny,
                                      GLint nz);
    void (APIENTRY *glNormalStream3iv)(GLenum stream, const GLint *coords);
    void (APIENTRY *glNormalStream3s)(GLenum stream, GLshort nx, GLshort ny,
                                      GLshort nz);
    void (APIENTRY *glNormalStream3sv)(GLenum stream, const GLshort *coords);
    void (APIENTRY *glVertexBlendEnvf)(GLenum pname, GLfloat param);
    void (APIENTRY *glVertexBlendEnvi)(GLenum pname, GLint param);
    void (APIENTRY *glVertexStream1d)(GLenum stream, GLdouble x);
    void (APIENTRY *glVertexStream1dv)(GLenum stream, const GLdouble *coords);
    void (APIENTRY *glVertexStream1f)(GLenum stream, GLfloat x);
    void (APIENTRY *glVertexStream1fv)(GLenum stream, const GLfloat *coords);
    void (APIENTRY *glVertexStream1i)(GLenum stream, GLint x);
    void (APIENTRY *glVertexStream1iv)(GLenum stream, const GLint *coords);
    void (APIENTRY *glVertexStream1s)(GLenum stream, GLshort x);
    void (APIENTRY *glVertexStream1sv)(GLenum stream, const GLshort *coords);
    void (APIENTRY *glVertexStream2d)(GLenum stream, GLdouble x, GLdouble y);
    void (APIENTRY *glVertexStream2dv)(GLenum stream, const GLdouble *coords);
    void (APIENTRY *glVertexStream2f)(GLenum stream, GLfloat x, GLfloat y);
    void (APIENTRY *glVertexStream2fv)(GLenum stream, const GLfloat *coords);
    void (APIENTRY *glVertexStream2i)(GLenum stream, GLint x, GLint y);
    void (APIENTRY *glVertexStream2iv)(GLenum stream, const GLint *coords);
    void (APIENTRY *glVertexStream2s)(GLenum stream, GLshort x, GLshort y);
    void (APIENTRY *glVertexStream2sv)(GLenum stream, const GLshort *coords);
    void (APIENTRY *glVertexStream3d)(GLenum stream, GLdouble x, GLdouble y,
                                      GLdouble z);
    void (APIENTRY *glVertexStream3dv)(GLenum stream, const GLdouble *coords);
    void (APIENTRY *glVertexStream3f)(GLenum stream, GLfloat x, GLfloat y,
                                      GLfloat z);
    void (APIENTRY *glVertexStream3fv)(GLenum stream, const GLfloat *coords);
    void (APIENTRY *glVertexStream3i)(GLenum stream, GLint x, GLint y, GLint z);
    void (APIENTRY *glVertexStream3iv)(GLenum stream, const GLint *coords);
    void (APIENTRY *glVertexStream3s)(GLenum stream, GLshort x, GLshort y,
                                      GLshort z);
    void (APIENTRY *glVertexStream3sv)(GLenum stream, const GLshort *coords);
    void (APIENTRY *glVertexStream4d)(GLenum stream, GLdouble x, GLdouble y,
                                      GLdouble z, GLdouble w);
    void (APIENTRY *glVertexStream4dv)(GLenum stream, const GLdouble *coords);
    void (APIENTRY *glVertexStream4f)(GLenum stream, GLfloat x, GLfloat y,
                                      GLfloat z, GLfloat w);
    void (APIENTRY *glVertexStream4fv)(GLenum stream, const GLfloat *coords);
    void (APIENTRY *glVertexStream4i)(GLenum stream, GLint x, GLint y, GLint z,
                                      GLint w);
    void (APIENTRY *glVertexStream4iv)(GLenum stream, const GLint *coords);
    void (APIENTRY *glVertexStream4s)(GLenum stream, GLshort x, GLshort y,
                                      GLshort z, GLshort w);
    void (APIENTRY *glVertexStream4sv)(GLenum stream, const GLshort *coords);

    // Construction
    ATIVertexStreams();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct ATIVertexStreams

// GL_EXT_EGL_image_storage extension API specification.
struct EXTEglImageStorage {
    // Public Members
    void (APIENTRY *glEGLImageTargetTexStorage)(GLenum target,
                                                GLeglImageOES image,
                                                const GLint*attrib_list);
    void (APIENTRY *glEGLImageTargetTextureStorage)(GLuint texture,
                                                    GLeglImageOES image,
                                                    const GLint*attrib_list);

    // Construction
    EXTEglImageStorage();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTEglImageStorage

// GL_EXT_bindable_uniform extension API specification.
struct EXTBindableUniform {
    // Public Members
    GLint (APIENTRY *glGetUniformBufferSize)(GLuint program, GLint location);
    GLintptr (APIENTRY *glGetUniformOffset)(GLuint program, GLint location);
    void (APIENTRY *glUniformBuffer)(GLuint program, GLint location,
                                     GLuint buffer);

    // Construction
    EXTBindableUniform();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTBindableUniform

// GL_EXT_blend_color extension API specification.
struct EXTBlendColor {
    // Public Members
    void (APIENTRY *glBlendColor)(GLfloat red, GLfloat green, GLfloat blue,
                                  GLfloat alpha);

    // Construction
    EXTBlendColor();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTBlendColor

// GL_EXT_blend_equation_separate extension API specification.
struct EXTBlendEquationSeparate {
    // Public Members
    void (APIENTRY *glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);

    // Construction
    EXTBlendEquationSeparate();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTBlendEquationSeparate

// GL_EXT_blend_func_separate extension API specification.
struct EXTBlendFuncSeparate {
    // Public Members
    void (APIENTRY *glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB,
                                         GLenum sfactorAlpha,
                                         GLenum dfactorAlpha);

    // Construction
    EXTBlendFuncSeparate();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTBlendFuncSeparate

// GL_EXT_blend_minmax extension API specification.
struct EXTBlendMinmax {
    // Public Members
    void (APIENTRY *glBlendEquation)(GLenum mode);

    // Construction
    EXTBlendMinmax();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTBlendMinmax

// GL_EXT_color_subtable extension API specification.
struct EXTColorSubtable {
    // Public Members
    void (APIENTRY *glColorSubTable)(GLenum target, GLsizei start, GLsizei count,
                                     GLenum format, GLenum type,
                                     const void *data);
    void (APIENTRY *glCopyColorSubTable)(GLenum target, GLsizei start, GLint x,
                                         GLint y, GLsizei width);

    // Construction
    EXTColorSubtable();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTColorSubtable

// GL_EXT_compiled_vertex_array extension API specification.
struct EXTCompiledVertexArray {
    // Public Members
    void (APIENTRY *glLockArrays)(GLint first, GLsizei count);
    void (APIENTRY *glUnlockArrays)();

    // Construction
    EXTCompiledVertexArray();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTCompiledVertexArray

// GL_EXT_convolution extension API specification.
struct EXTConvolution {
    // Public Members
    void (APIENTRY *glConvolutionFilter1D)(GLenum target, GLenum internalformat,
                                           GLsizei width, GLenum format,
                                           GLenum type, const void *image);
    void (APIENTRY *glConvolutionFilter2D)(GLenum target, GLenum internalformat,
                                           GLsizei width, GLsizei height,
                                           GLenum format, GLenum type,
                                           const void *image);
    void (APIENTRY *glConvolutionParameterf)(GLenum target, GLenum pname,
                                             GLfloat params);
    void (APIENTRY *glConvolutionParameterfv)(GLenum target, GLenum pname,
                                              const GLfloat *params);
    void (APIENTRY *glConvolutionParameteri)(GLenum target, GLenum pname,
                                             GLint params);
    void (APIENTRY *glConvolutionParameteriv)(GLenum target, GLenum pname,
                                              const GLint *params);
    void (APIENTRY *glCopyConvolutionFilter1D)(GLenum target,
                                               GLenum internalformat, GLint x,
                                               GLint y, GLsizei width);
    void (APIENTRY *glCopyConvolutionFilter2D)(GLenum target,
                                               GLenum internalformat, GLint x,
                                               GLint y, GLsizei width,
                                               GLsizei height);
    void (APIENTRY *glGetConvolutionFilter)(GLenum target, GLenum format,
                                            GLenum type, void *image);
    void (APIENTRY *glGetConvolutionParameterfv)(GLenum target, GLenum pname,
                                                 GLfloat *params);
    void (APIENTRY *glGetConvolutionParameteriv)(GLenum target, GLenum pname,
                                                 GLint *params);
    void (APIENTRY *glGetSeparableFilter)(GLenum target, GLenum format,
                                          GLenum type, void *row, void *column,
                                          void *span);
    void (APIENTRY *glSeparableFilter2D)(GLenum target, GLenum internalformat,
                                         GLsizei width, GLsizei height,
                                         GLenum format, GLenum type,
                                         const void *row, const void *column);

    // Construction
    EXTConvolution();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTConvolution

// GL_EXT_coordinate_frame extension API specification.
struct EXTCoordinateFrame {
    // Public Members
    void (APIENTRY *glBinormal3b)(GLbyte bx, GLbyte by, GLbyte bz);
    void (APIENTRY *glBinormal3bv)(const GLbyte *v);
    void (APIENTRY *glBinormal3d)(GLdouble bx, GLdouble by, GLdouble bz);
    void (APIENTRY *glBinormal3dv)(const GLdouble *v);
    void (APIENTRY *glBinormal3f)(GLfloat bx, GLfloat by, GLfloat bz);
    void (APIENTRY *glBinormal3fv)(const GLfloat *v);
    void (APIENTRY *glBinormal3i)(GLint bx, GLint by, GLint bz);
    void (APIENTRY *glBinormal3iv)(const GLint *v);
    void (APIENTRY *glBinormal3s)(GLshort bx, GLshort by, GLshort bz);
    void (APIENTRY *glBinormal3sv)(const GLshort *v);
    void (APIENTRY *glBinormalPointer)(GLenum type, GLsizei stride,
                                       const void *pointer);
    void (APIENTRY *glTangent3b)(GLbyte tx, GLbyte ty, GLbyte tz);
    void (APIENTRY *glTangent3bv)(const GLbyte *v);
    void (APIENTRY *glTangent3d)(GLdouble tx, GLdouble ty, GLdouble tz);
    void (APIENTRY *glTangent3dv)(const GLdouble *v);
    void (APIENTRY *glTangent3f)(GLfloat tx, GLfloat ty, GLfloat tz);
    void (APIENTRY *glTangent3fv)(const GLfloat *v);
    void (APIENTRY *glTangent3i)(GLint tx, GLint ty, GLint tz);
    void (APIENTRY *glTangent3iv)(const GLint *v);
    void (APIENTRY *glTangent3s)(GLshort tx, GLshort ty, GLshort tz);
    void (APIENTRY *glTangent3sv)(const GLshort *v);
    void (APIENTRY *glTangentPointer)(GLenum type, GLsizei stride,
                                      const void *pointer);

    // Construction
    EXTCoordinateFrame();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTCoordinateFrame

// GL_EXT_copy_texture extension API specification.
struct EXTCopyTexture {
    // Public Members
    void (APIENTRY *glCopyTexImage1D)(GLenum target, GLint level,
                                      GLenum internalformat, GLint x, GLint y,
                                      GLsizei width, GLint border);
    void (APIENTRY *glCopyTexImage2D)(GLenum target, GLint level,
                                      GLenum internalformat, GLint x, GLint y,
                                      GLsizei width, GLsizei height,
                                      GLint border);
    void (APIENTRY *glCopyTexSubImage1D)(GLenum target, GLint level,
                                         GLint xoffset, GLint x, GLint y,
                                         GLsizei width);
    void (APIENTRY *glCopyTexSubImage2D)(GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset, GLint x,
                                         GLint y, GLsizei width, GLsizei height);
    void (APIENTRY *glCopyTexSubImage3D)(GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLint x, GLint y,
                                         GLsizei width, GLsizei height);

    // Construction
    EXTCopyTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTCopyTexture

// GL_EXT_cull_vertex extension API specification.
struct EXTCullVertex {
    // Public Members
    void (APIENTRY *glCullParameterdv)(GLenum pname, GLdouble *params);
    void (APIENTRY *glCullParameterfv)(GLenum pname, GLfloat *params);

    // Construction
    EXTCullVertex();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTCullVertex

// GL_EXT_debug_label extension API specification.
struct EXTDebugLabel {
    // Public Members
    void (APIENTRY *glGetObjectLabel)(GLenum type, GLuint object,
                                      GLsizei bufSize, GLsizei *length,
                                      GLchar *label);
    void (APIENTRY *glLabelObject)(GLenum type, GLuint object, GLsizei length,
                                   const GLchar *label);

    // Construction
    EXTDebugLabel();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTDebugLabel

// GL_EXT_debug_marker extension API specification.
struct EXTDebugMarker {
    // Public Members
    void (APIENTRY *glInsertEventMarker)(GLsizei length, const GLchar *marker);
    void (APIENTRY *glPopGroupMarker)();
    void (APIENTRY *glPushGroupMarker)(GLsizei length, const GLchar *marker);

    // Construction
    EXTDebugMarker();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTDebugMarker

// GL_EXT_depth_bounds_test extension API specification.
struct EXTDepthBoundsTest {
    // Public Members
    void (APIENTRY *glDepthBounds)(GLclampd zmin, GLclampd zmax);

    // Construction
    EXTDepthBoundsTest();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTDepthBoundsTest

// GL_EXT_direct_state_access extension API specification.
// DSA extension doesn't identify which interfaces are core profile and keeps getting expanded. This is in sync with revision 34, 2010/09/07
struct EXTDirectStateAccess {
    // Public Members
    void (APIENTRY *glBindMultiTexture)(GLenum texunit, GLenum target,
                                        GLuint texture);
    GLenum (APIENTRY *glCheckNamedFramebufferStatus)(GLuint framebuffer,
                                                     GLenum target);
    void (APIENTRY *glClearNamedBufferData)(GLuint buffer, GLenum internalformat,
                                            GLenum format, GLenum type,
                                            const void *data);
    void (APIENTRY *glClearNamedBufferSubData)(GLuint buffer,
                                               GLenum internalformat,
                                               GLsizeiptr offset,
                                               GLsizeiptr size, GLenum format,
                                               GLenum type, const void *data);
    void (APIENTRY *glClientAttribDefault)(GLbitfield mask);
    void (APIENTRY *glCompressedMultiTexImage1D)(GLenum texunit, GLenum target,
                                                 GLint level,
                                                 GLenum internalformat,
                                                 GLsizei width, GLint border,
                                                 GLsizei imageSize,
                                                 const void *bits);
    void (APIENTRY *glCompressedMultiTexImage2D)(GLenum texunit, GLenum target,
                                                 GLint level,
                                                 GLenum internalformat,
                                                 GLsizei width, GLsizei height,
                                                 GLint border, GLsizei imageSize,
                                                 const void *bits);
    void (APIENTRY *glCompressedMultiTexImage3D)(GLenum texunit, GLenum target,
                                                 GLint level,
                                                 GLenum internalformat,
                                                 GLsizei width, GLsizei height,
                                                 GLsizei depth, GLint border,
                                                 GLsizei imageSize,
                                                 const void *bits);
    void (APIENTRY *glCompressedMultiTexSubImage1D)(GLenum texunit,
                                                    GLenum target, GLint level,
                                                    GLint xoffset, GLsizei width,
                                                    GLenum format,
                                                    GLsizei imageSize,
                                                    const void *bits);
    void (APIENTRY *glCompressedMultiTexSubImage2D)(GLenum texunit,
                                                    GLenum target, GLint level,
                                                    GLint xoffset, GLint yoffset,
                                                    GLsizei width,
                                                    GLsizei height,
                                                    GLenum format,
                                                    GLsizei imageSize,
                                                    const void *bits);
    void (APIENTRY *glCompressedMultiTexSubImage3D)(GLenum texunit,
                                                    GLenum target, GLint level,
                                                    GLint xoffset, GLint yoffset,
                                                    GLint zoffset, GLsizei width,
                                                    GLsizei height,
                                                    GLsizei depth, GLenum format,
                                                    GLsizei imageSize,
                                                    const void *bits);
    void (APIENTRY *glCompressedTextureImage1D)(GLuint texture, GLenum target,
                                                GLint level,
                                                GLenum internalformat,
                                                GLsizei width, GLint border,
                                                GLsizei imageSize,
                                                const void *bits);
    void (APIENTRY *glCompressedTextureImage2D)(GLuint texture, GLenum target,
                                                GLint level,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height,
                                                GLint border, GLsizei imageSize,
                                                const void *bits);
    void (APIENTRY *glCompressedTextureImage3D)(GLuint texture, GLenum target,
                                                GLint level,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height,
                                                GLsizei depth, GLint border,
                                                GLsizei imageSize,
                                                const void *bits);
    void (APIENTRY *glCompressedTextureSubImage1D)(GLuint texture, GLenum target,
                                                   GLint level, GLint xoffset,
                                                   GLsizei width, GLenum format,
                                                   GLsizei imageSize,
                                                   const void *bits);
    void (APIENTRY *glCompressedTextureSubImage2D)(GLuint texture, GLenum target,
                                                   GLint level, GLint xoffset,
                                                   GLint yoffset, GLsizei width,
                                                   GLsizei height, GLenum format,
                                                   GLsizei imageSize,
                                                   const void *bits);
    void (APIENTRY *glCompressedTextureSubImage3D)(GLuint texture, GLenum target,
                                                   GLint level, GLint xoffset,
                                                   GLint yoffset, GLint zoffset,
                                                   GLsizei width, GLsizei height,
                                                   GLsizei depth, GLenum format,
                                                   GLsizei imageSize,
                                                   const void *bits);
    void (APIENTRY *glCopyMultiTexImage1D)(GLenum texunit, GLenum target,
                                           GLint level, GLenum internalformat,
                                           GLint x, GLint y, GLsizei width,
                                           GLint border);
    void (APIENTRY *glCopyMultiTexImage2D)(GLenum texunit, GLenum target,
                                           GLint level, GLenum internalformat,
                                           GLint x, GLint y, GLsizei width,
                                           GLsizei height, GLint border);
    void (APIENTRY *glCopyMultiTexSubImage1D)(GLenum texunit, GLenum target,
                                              GLint level, GLint xoffset,
                                              GLint x, GLint y, GLsizei width);
    void (APIENTRY *glCopyMultiTexSubImage2D)(GLenum texunit, GLenum target,
                                              GLint level, GLint xoffset,
                                              GLint yoffset, GLint x, GLint y,
                                              GLsizei width, GLsizei height);
    void (APIENTRY *glCopyMultiTexSubImage3D)(GLenum texunit, GLenum target,
                                              GLint level, GLint xoffset,
                                              GLint yoffset, GLint zoffset,
                                              GLint x, GLint y, GLsizei width,
                                              GLsizei height);
    void (APIENTRY *glCopyTextureImage1D)(GLuint texture, GLenum target,
                                          GLint level, GLenum internalformat,
                                          GLint x, GLint y, GLsizei width,
                                          GLint border);
    void (APIENTRY *glCopyTextureImage2D)(GLuint texture, GLenum target,
                                          GLint level, GLenum internalformat,
                                          GLint x, GLint y, GLsizei width,
                                          GLsizei height, GLint border);
    void (APIENTRY *glCopyTextureSubImage1D)(GLuint texture, GLenum target,
                                             GLint level, GLint xoffset, GLint x,
                                             GLint y, GLsizei width);
    void (APIENTRY *glCopyTextureSubImage2D)(GLuint texture, GLenum target,
                                             GLint level, GLint xoffset,
                                             GLint yoffset, GLint x, GLint y,
                                             GLsizei width, GLsizei height);
    void (APIENTRY *glCopyTextureSubImage3D)(GLuint texture, GLenum target,
                                             GLint level, GLint xoffset,
                                             GLint yoffset, GLint zoffset,
                                             GLint x, GLint y, GLsizei width,
                                             GLsizei height);
    void (APIENTRY *glDisableClientStateIndexed)(GLenum array, GLuint index);
    void (APIENTRY *glDisableClientStatei)(GLenum array, GLuint index);
    void (APIENTRY *glDisableIndexed)(GLenum target, GLuint index);
    void (APIENTRY *glDisableVertexArrayAttrib)(GLuint vaobj, GLuint index);
    void (APIENTRY *glDisableVertexArray)(GLuint vaobj, GLenum array);
    void (APIENTRY *glEnableClientStateIndexed)(GLenum array, GLuint index);
    void (APIENTRY *glEnableClientStatei)(GLenum array, GLuint index);
    void (APIENTRY *glEnableIndexed)(GLenum target, GLuint index);
    void (APIENTRY *glEnableVertexArrayAttrib)(GLuint vaobj, GLuint index);
    void (APIENTRY *glEnableVertexArray)(GLuint vaobj, GLenum array);
    void (APIENTRY *glFlushMappedNamedBufferRange)(GLuint buffer,
                                                   GLintptr offset,
                                                   GLsizeiptr length);
    void (APIENTRY *glFramebufferDrawBuffer)(GLuint framebuffer, GLenum mode);
    void (APIENTRY *glFramebufferDrawBuffers)(GLuint framebuffer, GLsizei n,
                                              const GLenum *bufs);
    void (APIENTRY *glFramebufferReadBuffer)(GLuint framebuffer, GLenum mode);
    void (APIENTRY *glGenerateMultiTexMipmap)(GLenum texunit, GLenum target);
    void (APIENTRY *glGenerateTextureMipmap)(GLuint texture, GLenum target);
    void (APIENTRY *glGetBooleanIndexedv)(GLenum target, GLuint index,
                                          GLboolean *data);
    void (APIENTRY *glGetCompressedMultiTexImage)(GLenum texunit, GLenum target,
                                                  GLint lod, void *img);
    void (APIENTRY *glGetCompressedTextureImage)(GLuint texture, GLenum target,
                                                 GLint lod, void *img);
    void (APIENTRY *glGetDoubleIndexedv)(GLenum target, GLuint index,
                                         GLdouble *data);
    void (APIENTRY *glGetDoublei_v)(GLenum pname, GLuint index, GLdouble *params);
    void (APIENTRY *glGetFloatIndexedv)(GLenum target, GLuint index,
                                        GLfloat *data);
    void (APIENTRY *glGetFloati_v)(GLenum pname, GLuint index, GLfloat *params);
    void (APIENTRY *glGetFramebufferParameteriv)(GLuint framebuffer,
                                                 GLenum pname, GLint *params);
    void (APIENTRY *glGetIntegerIndexedv)(GLenum target, GLuint index,
                                          GLint *data);
    void (APIENTRY *glGetMultiTexEnvfv)(GLenum texunit, GLenum target,
                                        GLenum pname, GLfloat *params);
    void (APIENTRY *glGetMultiTexEnviv)(GLenum texunit, GLenum target,
                                        GLenum pname, GLint *params);
    void (APIENTRY *glGetMultiTexGendv)(GLenum texunit, GLenum coord,
                                        GLenum pname, GLdouble *params);
    void (APIENTRY *glGetMultiTexGenfv)(GLenum texunit, GLenum coord,
                                        GLenum pname, GLfloat *params);
    void (APIENTRY *glGetMultiTexGeniv)(GLenum texunit, GLenum coord,
                                        GLenum pname, GLint *params);
    void (APIENTRY *glGetMultiTexImage)(GLenum texunit, GLenum target,
                                        GLint level, GLenum format, GLenum type,
                                        void *pixels);
    void (APIENTRY *glGetMultiTexLevelParameterfv)(GLenum texunit, GLenum target,
                                                   GLint level, GLenum pname,
                                                   GLfloat *params);
    void (APIENTRY *glGetMultiTexLevelParameteriv)(GLenum texunit, GLenum target,
                                                   GLint level, GLenum pname,
                                                   GLint *params);
    void (APIENTRY *glGetMultiTexParameterIiv)(GLenum texunit, GLenum target,
                                               GLenum pname, GLint *params);
    void (APIENTRY *glGetMultiTexParameterIuiv)(GLenum texunit, GLenum target,
                                                GLenum pname, GLuint *params);
    void (APIENTRY *glGetMultiTexParameterfv)(GLenum texunit, GLenum target,
                                              GLenum pname, GLfloat *params);
    void (APIENTRY *glGetMultiTexParameteriv)(GLenum texunit, GLenum target,
                                              GLenum pname, GLint *params);
    void (APIENTRY *glGetNamedBufferParameteriv)(GLuint buffer, GLenum pname,
                                                 GLint *params);
    void (APIENTRY *glGetNamedBufferPointerv)(GLuint buffer, GLenum pname,
                                              void **params);
    void (APIENTRY *glGetNamedBufferSubData)(GLuint buffer, GLintptr offset,
                                             GLsizeiptr size, void *data);
    void (APIENTRY *glGetNamedFramebufferAttachmentParameteriv)(GLuint framebuffer,
                                                                GLenum attachment,
                                                                GLenum pname,
                                                                GLint *params);
    void (APIENTRY *glGetNamedFramebufferParameteriv)(GLuint framebuffer,
                                                      GLenum pname,
                                                      GLint *params);
    void (APIENTRY *glGetNamedProgramLocalParameterIiv)(GLuint program,
                                                        GLenum target,
                                                        GLuint index,
                                                        GLint *params);
    void (APIENTRY *glGetNamedProgramLocalParameterIuiv)(GLuint program,
                                                         GLenum target,
                                                         GLuint index,
                                                         GLuint *params);
    void (APIENTRY *glGetNamedProgramLocalParameterdv)(GLuint program,
                                                       GLenum target,
                                                       GLuint index,
                                                       GLdouble *params);
    void (APIENTRY *glGetNamedProgramLocalParameterfv)(GLuint program,
                                                       GLenum target,
                                                       GLuint index,
                                                       GLfloat *params);
    void (APIENTRY *glGetNamedProgramString)(GLuint program, GLenum target,
                                             GLenum pname, void *string);
    void (APIENTRY *glGetNamedProgramiv)(GLuint program, GLenum target,
                                         GLenum pname, GLint *params);
    void (APIENTRY *glGetNamedRenderbufferParameteriv)(GLuint renderbuffer,
                                                       GLenum pname,
                                                       GLint *params);
    void (APIENTRY *glGetPointerIndexedv)(GLenum target, GLuint index,
                                          void **data);
    void (APIENTRY *glGetPointeri_v)(GLenum pname, GLuint index, void **params);
    void (APIENTRY *glGetTextureImage)(GLuint texture, GLenum target,
                                       GLint level, GLenum format, GLenum type,
                                       void *pixels);
    void (APIENTRY *glGetTextureLevelParameterfv)(GLuint texture, GLenum target,
                                                  GLint level, GLenum pname,
                                                  GLfloat *params);
    void (APIENTRY *glGetTextureLevelParameteriv)(GLuint texture, GLenum target,
                                                  GLint level, GLenum pname,
                                                  GLint *params);
    void (APIENTRY *glGetTextureParameterIiv)(GLuint texture, GLenum target,
                                              GLenum pname, GLint *params);
    void (APIENTRY *glGetTextureParameterIuiv)(GLuint texture, GLenum target,
                                               GLenum pname, GLuint *params);
    void (APIENTRY *glGetTextureParameterfv)(GLuint texture, GLenum target,
                                             GLenum pname, GLfloat *params);
    void (APIENTRY *glGetTextureParameteriv)(GLuint texture, GLenum target,
                                             GLenum pname, GLint *params);
    void (APIENTRY *glGetVertexArrayIntegeri_v)(GLuint vaobj, GLuint index,
                                                GLenum pname, GLint *param);
    void (APIENTRY *glGetVertexArrayIntegerv)(GLuint vaobj, GLenum pname,
                                              GLint *param);
    void (APIENTRY *glGetVertexArrayPointeri_v)(GLuint vaobj, GLuint index,
                                                GLenum pname, void **param);
    void (APIENTRY *glGetVertexArrayPointerv)(GLuint vaobj, GLenum pname,
                                              void **param);
    GLboolean (APIENTRY *glIsEnabledIndexed)(GLenum target, GLuint index);
    void *(APIENTRY *glMapNamedBuffer)(GLuint buffer, GLenum access);
    void *(APIENTRY *glMapNamedBufferRange)(GLuint buffer, GLintptr offset,
                                            GLsizeiptr length, GLbitfield access);
    void (APIENTRY *glMatrixFrustum)(GLenum mode, GLdouble left, GLdouble right,
                                     GLdouble bottom, GLdouble top,
                                     GLdouble zNear, GLdouble zFar);
    void (APIENTRY *glMatrixLoadIdentity)(GLenum mode);
    void (APIENTRY *glMatrixLoadTransposed)(GLenum mode, const GLdouble *m);
    void (APIENTRY *glMatrixLoadTransposef)(GLenum mode, const GLfloat *m);
    void (APIENTRY *glMatrixLoadd)(GLenum mode, const GLdouble *m);
    void (APIENTRY *glMatrixLoadf)(GLenum mode, const GLfloat *m);
    void (APIENTRY *glMatrixMultTransposed)(GLenum mode, const GLdouble *m);
    void (APIENTRY *glMatrixMultTransposef)(GLenum mode, const GLfloat *m);
    void (APIENTRY *glMatrixMultd)(GLenum mode, const GLdouble *m);
    void (APIENTRY *glMatrixMultf)(GLenum mode, const GLfloat *m);
    void (APIENTRY *glMatrixOrtho)(GLenum mode, GLdouble left, GLdouble right,
                                   GLdouble bottom, GLdouble top, GLdouble zNear,
                                   GLdouble zFar);
    void (APIENTRY *glMatrixPop)(GLenum mode);
    void (APIENTRY *glMatrixPush)(GLenum mode);
    void (APIENTRY *glMatrixRotated)(GLenum mode, GLdouble angle, GLdouble x,
                                     GLdouble y, GLdouble z);
    void (APIENTRY *glMatrixRotatef)(GLenum mode, GLfloat angle, GLfloat x,
                                     GLfloat y, GLfloat z);
    void (APIENTRY *glMatrixScaled)(GLenum mode, GLdouble x, GLdouble y,
                                    GLdouble z);
    void (APIENTRY *glMatrixScalef)(GLenum mode, GLfloat x, GLfloat y, GLfloat z);
    void (APIENTRY *glMatrixTranslated)(GLenum mode, GLdouble x, GLdouble y,
                                        GLdouble z);
    void (APIENTRY *glMatrixTranslatef)(GLenum mode, GLfloat x, GLfloat y,
                                        GLfloat z);
    void (APIENTRY *glMultiTexBuffer)(GLenum texunit, GLenum target,
                                      GLenum internalformat, GLuint buffer);
    void (APIENTRY *glMultiTexCoordPointer)(GLenum texunit, GLint size,
                                            GLenum type, GLsizei stride,
                                            const void *pointer);
    void (APIENTRY *glMultiTexEnvf)(GLenum texunit, GLenum target, GLenum pname,
                                    GLfloat param);
    void (APIENTRY *glMultiTexEnvfv)(GLenum texunit, GLenum target, GLenum pname,
                                     const GLfloat *params);
    void (APIENTRY *glMultiTexEnvi)(GLenum texunit, GLenum target, GLenum pname,
                                    GLint param);
    void (APIENTRY *glMultiTexEnviv)(GLenum texunit, GLenum target, GLenum pname,
                                     const GLint *params);
    void (APIENTRY *glMultiTexGend)(GLenum texunit, GLenum coord, GLenum pname,
                                    GLdouble param);
    void (APIENTRY *glMultiTexGendv)(GLenum texunit, GLenum coord, GLenum pname,
                                     const GLdouble *params);
    void (APIENTRY *glMultiTexGenf)(GLenum texunit, GLenum coord, GLenum pname,
                                    GLfloat param);
    void (APIENTRY *glMultiTexGenfv)(GLenum texunit, GLenum coord, GLenum pname,
                                     const GLfloat *params);
    void (APIENTRY *glMultiTexGeni)(GLenum texunit, GLenum coord, GLenum pname,
                                    GLint param);
    void (APIENTRY *glMultiTexGeniv)(GLenum texunit, GLenum coord, GLenum pname,
                                     const GLint *params);
    void (APIENTRY *glMultiTexImage1D)(GLenum texunit, GLenum target,
                                       GLint level, GLint internalformat,
                                       GLsizei width, GLint border,
                                       GLenum format, GLenum type,
                                       const void *pixels);
    void (APIENTRY *glMultiTexImage2D)(GLenum texunit, GLenum target,
                                       GLint level, GLint internalformat,
                                       GLsizei width, GLsizei height,
                                       GLint border, GLenum format, GLenum type,
                                       const void *pixels);
    void (APIENTRY *glMultiTexImage3D)(GLenum texunit, GLenum target,
                                       GLint level, GLint internalformat,
                                       GLsizei width, GLsizei height,
                                       GLsizei depth, GLint border,
                                       GLenum format, GLenum type,
                                       const void *pixels);
    void (APIENTRY *glMultiTexParameterIiv)(GLenum texunit, GLenum target,
                                            GLenum pname, const GLint *params);
    void (APIENTRY *glMultiTexParameterIuiv)(GLenum texunit, GLenum target,
                                             GLenum pname, const GLuint *params);
    void (APIENTRY *glMultiTexParameterf)(GLenum texunit, GLenum target,
                                          GLenum pname, GLfloat param);
    void (APIENTRY *glMultiTexParameterfv)(GLenum texunit, GLenum target,
                                           GLenum pname, const GLfloat *params);
    void (APIENTRY *glMultiTexParameteri)(GLenum texunit, GLenum target,
                                          GLenum pname, GLint param);
    void (APIENTRY *glMultiTexParameteriv)(GLenum texunit, GLenum target,
                                           GLenum pname, const GLint *params);
    void (APIENTRY *glMultiTexRenderbuffer)(GLenum texunit, GLenum target,
                                            GLuint renderbuffer);
    void (APIENTRY *glMultiTexSubImage1D)(GLenum texunit, GLenum target,
                                          GLint level, GLint xoffset,
                                          GLsizei width, GLenum format,
                                          GLenum type, const void *pixels);
    void (APIENTRY *glMultiTexSubImage2D)(GLenum texunit, GLenum target,
                                          GLint level, GLint xoffset,
                                          GLint yoffset, GLsizei width,
                                          GLsizei height, GLenum format,
                                          GLenum type, const void *pixels);
    void (APIENTRY *glMultiTexSubImage3D)(GLenum texunit, GLenum target,
                                          GLint level, GLint xoffset,
                                          GLint yoffset, GLint zoffset,
                                          GLsizei width, GLsizei height,
                                          GLsizei depth, GLenum format,
                                          GLenum type, const void *pixels);
    void (APIENTRY *glNamedBufferData)(GLuint buffer, GLsizeiptr size,
                                       const void *data, GLenum usage);
    void (APIENTRY *glNamedBufferStorage)(GLuint buffer, GLsizeiptr size,
                                          const void *data, GLbitfield flags);
    void (APIENTRY *glNamedBufferSubData)(GLuint buffer, GLintptr offset,
                                          GLsizeiptr size, const void *data);
    void (APIENTRY *glNamedCopyBufferSubData)(GLuint readBuffer,
                                              GLuint writeBuffer,
                                              GLintptr readOffset,
                                              GLintptr writeOffset,
                                              GLsizeiptr size);
    void (APIENTRY *glNamedFramebufferParameteri)(GLuint framebuffer,
                                                  GLenum pname, GLint param);
    void (APIENTRY *glNamedFramebufferRenderbuffer)(GLuint framebuffer,
                                                    GLenum attachment,
                                                    GLenum renderbuffertarget,
                                                    GLuint renderbuffer);
    void (APIENTRY *glNamedFramebufferTexture1D)(GLuint framebuffer,
                                                 GLenum attachment,
                                                 GLenum textarget,
                                                 GLuint texture, GLint level);
    void (APIENTRY *glNamedFramebufferTexture2D)(GLuint framebuffer,
                                                 GLenum attachment,
                                                 GLenum textarget,
                                                 GLuint texture, GLint level);
    void (APIENTRY *glNamedFramebufferTexture3D)(GLuint framebuffer,
                                                 GLenum attachment,
                                                 GLenum textarget,
                                                 GLuint texture, GLint level,
                                                 GLint zoffset);
    void (APIENTRY *glNamedFramebufferTexture)(GLuint framebuffer,
                                               GLenum attachment, GLuint texture,
                                               GLint level);
    void (APIENTRY *glNamedFramebufferTextureFace)(GLuint framebuffer,
                                                   GLenum attachment,
                                                   GLuint texture, GLint level,
                                                   GLenum face);
    void (APIENTRY *glNamedFramebufferTextureLayer)(GLuint framebuffer,
                                                    GLenum attachment,
                                                    GLuint texture, GLint level,
                                                    GLint layer);
    void (APIENTRY *glNamedProgramLocalParameter4d)(GLuint program,
                                                    GLenum target, GLuint index,
                                                    GLdouble x, GLdouble y,
                                                    GLdouble z, GLdouble w);
    void (APIENTRY *glNamedProgramLocalParameter4dv)(GLuint program,
                                                     GLenum target, GLuint index,
                                                     const GLdouble *params);
    void (APIENTRY *glNamedProgramLocalParameter4f)(GLuint program,
                                                    GLenum target, GLuint index,
                                                    GLfloat x, GLfloat y,
                                                    GLfloat z, GLfloat w);
    void (APIENTRY *glNamedProgramLocalParameter4fv)(GLuint program,
                                                     GLenum target, GLuint index,
                                                     const GLfloat *params);
    void (APIENTRY *glNamedProgramLocalParameterI4i)(GLuint program,
                                                     GLenum target, GLuint index,
                                                     GLint x, GLint y, GLint z,
                                                     GLint w);
    void (APIENTRY *glNamedProgramLocalParameterI4iv)(GLuint program,
                                                      GLenum target,
                                                      GLuint index,
                                                      const GLint *params);
    void (APIENTRY *glNamedProgramLocalParameterI4ui)(GLuint program,
                                                      GLenum target,
                                                      GLuint index, GLuint x,
                                                      GLuint y, GLuint z,
                                                      GLuint w);
    void (APIENTRY *glNamedProgramLocalParameterI4uiv)(GLuint program,
                                                       GLenum target,
                                                       GLuint index,
                                                       const GLuint *params);
    void (APIENTRY *glNamedProgramLocalParameters4fv)(GLuint program,
                                                      GLenum target,
                                                      GLuint index,
                                                      GLsizei count,
                                                      const GLfloat *params);
    void (APIENTRY *glNamedProgramLocalParametersI4iv)(GLuint program,
                                                       GLenum target,
                                                       GLuint index,
                                                       GLsizei count,
                                                       const GLint *params);
    void (APIENTRY *glNamedProgramLocalParametersI4uiv)(GLuint program,
                                                        GLenum target,
                                                        GLuint index,
                                                        GLsizei count,
                                                        const GLuint *params);
    void (APIENTRY *glNamedProgramString)(GLuint program, GLenum target,
                                          GLenum format, GLsizei len,
                                          const void *string);
    void (APIENTRY *glNamedRenderbufferStorage)(GLuint renderbuffer,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height);
    void (APIENTRY *glNamedRenderbufferStorageMultisampleCoverage)(GLuint renderbuffer,
                                                                   GLsizei coverageSamples,
                                                                   GLsizei colorSamples,
                                                                   GLenum internalformat,
                                                                   GLsizei width,
                                                                   GLsizei height);
    void (APIENTRY *glNamedRenderbufferStorageMultisample)(GLuint renderbuffer,
                                                           GLsizei samples,
                                                           GLenum internalformat,
                                                           GLsizei width,
                                                           GLsizei height);
    void (APIENTRY *glProgramUniform1d)(GLuint program, GLint location,
                                        GLdouble x);
    void (APIENTRY *glProgramUniform1dv)(GLuint program, GLint location,
                                         GLsizei count, const GLdouble *value);
    void (APIENTRY *glProgramUniform1f)(GLuint program, GLint location,
                                        GLfloat v0);
    void (APIENTRY *glProgramUniform1fv)(GLuint program, GLint location,
                                         GLsizei count, const GLfloat *value);
    void (APIENTRY *glProgramUniform1i)(GLuint program, GLint location, GLint v0);
    void (APIENTRY *glProgramUniform1iv)(GLuint program, GLint location,
                                         GLsizei count, const GLint *value);
    void (APIENTRY *glProgramUniform1ui)(GLuint program, GLint location,
                                         GLuint v0);
    void (APIENTRY *glProgramUniform1uiv)(GLuint program, GLint location,
                                          GLsizei count, const GLuint *value);
    void (APIENTRY *glProgramUniform2d)(GLuint program, GLint location,
                                        GLdouble x, GLdouble y);
    void (APIENTRY *glProgramUniform2dv)(GLuint program, GLint location,
                                         GLsizei count, const GLdouble *value);
    void (APIENTRY *glProgramUniform2f)(GLuint program, GLint location,
                                        GLfloat v0, GLfloat v1);
    void (APIENTRY *glProgramUniform2fv)(GLuint program, GLint location,
                                         GLsizei count, const GLfloat *value);
    void (APIENTRY *glProgramUniform2i)(GLuint program, GLint location, GLint v0,
                                        GLint v1);
    void (APIENTRY *glProgramUniform2iv)(GLuint program, GLint location,
                                         GLsizei count, const GLint *value);
    void (APIENTRY *glProgramUniform2ui)(GLuint program, GLint location,
                                         GLuint v0, GLuint v1);
    void (APIENTRY *glProgramUniform2uiv)(GLuint program, GLint location,
                                          GLsizei count, const GLuint *value);
    void (APIENTRY *glProgramUniform3d)(GLuint program, GLint location,
                                        GLdouble x, GLdouble y, GLdouble z);
    void (APIENTRY *glProgramUniform3dv)(GLuint program, GLint location,
                                         GLsizei count, const GLdouble *value);
    void (APIENTRY *glProgramUniform3f)(GLuint program, GLint location,
                                        GLfloat v0, GLfloat v1, GLfloat v2);
    void (APIENTRY *glProgramUniform3fv)(GLuint program, GLint location,
                                         GLsizei count, const GLfloat *value);
    void (APIENTRY *glProgramUniform3i)(GLuint program, GLint location, GLint v0,
                                        GLint v1, GLint v2);
    void (APIENTRY *glProgramUniform3iv)(GLuint program, GLint location,
                                         GLsizei count, const GLint *value);
    void (APIENTRY *glProgramUniform3ui)(GLuint program, GLint location,
                                         GLuint v0, GLuint v1, GLuint v2);
    void (APIENTRY *glProgramUniform3uiv)(GLuint program, GLint location,
                                          GLsizei count, const GLuint *value);
    void (APIENTRY *glProgramUniform4d)(GLuint program, GLint location,
                                        GLdouble x, GLdouble y, GLdouble z,
                                        GLdouble w);
    void (APIENTRY *glProgramUniform4dv)(GLuint program, GLint location,
                                         GLsizei count, const GLdouble *value);
    void (APIENTRY *glProgramUniform4f)(GLuint program, GLint location,
                                        GLfloat v0, GLfloat v1, GLfloat v2,
                                        GLfloat v3);
    void (APIENTRY *glProgramUniform4fv)(GLuint program, GLint location,
                                         GLsizei count, const GLfloat *value);
    void (APIENTRY *glProgramUniform4i)(GLuint program, GLint location, GLint v0,
                                        GLint v1, GLint v2, GLint v3);
    void (APIENTRY *glProgramUniform4iv)(GLuint program, GLint location,
                                         GLsizei count, const GLint *value);
    void (APIENTRY *glProgramUniform4ui)(GLuint program, GLint location,
                                         GLuint v0, GLuint v1, GLuint v2,
                                         GLuint v3);
    void (APIENTRY *glProgramUniform4uiv)(GLuint program, GLint location,
                                          GLsizei count, const GLuint *value);
    void (APIENTRY *glProgramUniformMatrix2dv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix2fv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix2x3dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix2x3fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix2x4dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix2x4fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix3dv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix3fv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix3x2dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix3x2fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix3x4dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix3x4fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix4dv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix4fv)(GLuint program, GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix4x2dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix4x2fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glProgramUniformMatrix4x3dv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLdouble *value);
    void (APIENTRY *glProgramUniformMatrix4x3fv)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value);
    void (APIENTRY *glPushClientAttribDefault)(GLbitfield mask);
    void (APIENTRY *glTextureBuffer)(GLuint texture, GLenum target,
                                     GLenum internalformat, GLuint buffer);
    void (APIENTRY *glTextureBufferRange)(GLuint texture, GLenum target,
                                          GLenum internalformat, GLuint buffer,
                                          GLintptr offset, GLsizeiptr size);
    void (APIENTRY *glTextureImage1D)(GLuint texture, GLenum target, GLint level,
                                      GLint internalformat, GLsizei width,
                                      GLint border, GLenum format, GLenum type,
                                      const void *pixels);
    void (APIENTRY *glTextureImage2D)(GLuint texture, GLenum target, GLint level,
                                      GLint internalformat, GLsizei width,
                                      GLsizei height, GLint border,
                                      GLenum format, GLenum type,
                                      const void *pixels);
    void (APIENTRY *glTextureImage3D)(GLuint texture, GLenum target, GLint level,
                                      GLint internalformat, GLsizei width,
                                      GLsizei height, GLsizei depth,
                                      GLint border, GLenum format, GLenum type,
                                      const void *pixels);
    void (APIENTRY *glTexturePageCommitment)(GLuint texture, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint zoffset, GLsizei width,
                                             GLsizei height, GLsizei depth,
                                             GLboolean commit);
    void (APIENTRY *glTextureParameterIiv)(GLuint texture, GLenum target,
                                           GLenum pname, const GLint *params);
    void (APIENTRY *glTextureParameterIuiv)(GLuint texture, GLenum target,
                                            GLenum pname, const GLuint *params);
    void (APIENTRY *glTextureParameterf)(GLuint texture, GLenum target,
                                         GLenum pname, GLfloat param);
    void (APIENTRY *glTextureParameterfv)(GLuint texture, GLenum target,
                                          GLenum pname, const GLfloat *params);
    void (APIENTRY *glTextureParameteri)(GLuint texture, GLenum target,
                                         GLenum pname, GLint param);
    void (APIENTRY *glTextureParameteriv)(GLuint texture, GLenum target,
                                          GLenum pname, const GLint *params);
    void (APIENTRY *glTextureRenderbuffer)(GLuint texture, GLenum target,
                                           GLuint renderbuffer);
    void (APIENTRY *glTextureStorage1D)(GLuint texture, GLenum target,
                                        GLsizei levels, GLenum internalformat,
                                        GLsizei width);
    void (APIENTRY *glTextureStorage2D)(GLuint texture, GLenum target,
                                        GLsizei levels, GLenum internalformat,
                                        GLsizei width, GLsizei height);
    void (APIENTRY *glTextureStorage2DMultisample)(GLuint texture, GLenum target,
                                                   GLsizei samples,
                                                   GLenum internalformat,
                                                   GLsizei width, GLsizei height,
                                                   GLboolean fixedsamplelocations);
    void (APIENTRY *glTextureStorage3D)(GLuint texture, GLenum target,
                                        GLsizei levels, GLenum internalformat,
                                        GLsizei width, GLsizei height,
                                        GLsizei depth);
    void (APIENTRY *glTextureStorage3DMultisample)(GLuint texture, GLenum target,
                                                   GLsizei samples,
                                                   GLenum internalformat,
                                                   GLsizei width, GLsizei height,
                                                   GLsizei depth,
                                                   GLboolean fixedsamplelocations);
    void (APIENTRY *glTextureSubImage1D)(GLuint texture, GLenum target,
                                         GLint level, GLint xoffset,
                                         GLsizei width, GLenum format,
                                         GLenum type, const void *pixels);
    void (APIENTRY *glTextureSubImage2D)(GLuint texture, GLenum target,
                                         GLint level, GLint xoffset,
                                         GLint yoffset, GLsizei width,
                                         GLsizei height, GLenum format,
                                         GLenum type, const void *pixels);
    void (APIENTRY *glTextureSubImage3D)(GLuint texture, GLenum target,
                                         GLint level, GLint xoffset,
                                         GLint yoffset, GLint zoffset,
                                         GLsizei width, GLsizei height,
                                         GLsizei depth, GLenum format,
                                         GLenum type, const void *pixels);
    GLboolean (APIENTRY *glUnmapNamedBuffer)(GLuint buffer);
    void (APIENTRY *glVertexArrayBindVertexBuffer)(GLuint vaobj,
                                                   GLuint bindingindex,
                                                   GLuint buffer,
                                                   GLintptr offset,
                                                   GLsizei stride);
    void (APIENTRY *glVertexArrayColorOffset)(GLuint vaobj, GLuint buffer,
                                              GLint size, GLenum type,
                                              GLsizei stride, GLintptr offset);
    void (APIENTRY *glVertexArrayEdgeFlagOffset)(GLuint vaobj, GLuint buffer,
                                                 GLsizei stride, GLintptr offset);
    void (APIENTRY *glVertexArrayFogCoordOffset)(GLuint vaobj, GLuint buffer,
                                                 GLenum type, GLsizei stride,
                                                 GLintptr offset);
    void (APIENTRY *glVertexArrayIndexOffset)(GLuint vaobj, GLuint buffer,
                                              GLenum type, GLsizei stride,
                                              GLintptr offset);
    void (APIENTRY *glVertexArrayMultiTexCoordOffset)(GLuint vaobj,
                                                      GLuint buffer,
                                                      GLenum texunit, GLint size,
                                                      GLenum type,
                                                      GLsizei stride,
                                                      GLintptr offset);
    void (APIENTRY *glVertexArrayNormalOffset)(GLuint vaobj, GLuint buffer,
                                               GLenum type, GLsizei stride,
                                               GLintptr offset);
    void (APIENTRY *glVertexArraySecondaryColorOffset)(GLuint vaobj,
                                                       GLuint buffer, GLint size,
                                                       GLenum type,
                                                       GLsizei stride,
                                                       GLintptr offset);
    void (APIENTRY *glVertexArrayTexCoordOffset)(GLuint vaobj, GLuint buffer,
                                                 GLint size, GLenum type,
                                                 GLsizei stride, GLintptr offset);
    void (APIENTRY *glVertexArrayVertexAttribBinding)(GLuint vaobj,
                                                      GLuint attribindex,
                                                      GLuint bindingindex);
    void (APIENTRY *glVertexArrayVertexAttribDivisor)(GLuint vaobj, GLuint index,
                                                      GLuint divisor);
    void (APIENTRY *glVertexArrayVertexAttribFormat)(GLuint vaobj,
                                                     GLuint attribindex,
                                                     GLint size, GLenum type,
                                                     GLboolean normalized,
                                                     GLuint relativeoffset);
    void (APIENTRY *glVertexArrayVertexAttribIFormat)(GLuint vaobj,
                                                      GLuint attribindex,
                                                      GLint size, GLenum type,
                                                      GLuint relativeoffset);
    void (APIENTRY *glVertexArrayVertexAttribIOffset)(GLuint vaobj,
                                                      GLuint buffer,
                                                      GLuint index, GLint size,
                                                      GLenum type,
                                                      GLsizei stride,
                                                      GLintptr offset);
    void (APIENTRY *glVertexArrayVertexAttribLFormat)(GLuint vaobj,
                                                      GLuint attribindex,
                                                      GLint size, GLenum type,
                                                      GLuint relativeoffset);
    void (APIENTRY *glVertexArrayVertexAttribLOffset)(GLuint vaobj,
                                                      GLuint buffer,
                                                      GLuint index, GLint size,
                                                      GLenum type,
                                                      GLsizei stride,
                                                      GLintptr offset);
    void (APIENTRY *glVertexArrayVertexAttribOffset)(GLuint vaobj, GLuint buffer,
                                                     GLuint index, GLint size,
                                                     GLenum type,
                                                     GLboolean normalized,
                                                     GLsizei stride,
                                                     GLintptr offset);
    void (APIENTRY *glVertexArrayVertexBindingDivisor)(GLuint vaobj,
                                                       GLuint bindingindex,
                                                       GLuint divisor);
    void (APIENTRY *glVertexArrayVertexOffset)(GLuint vaobj, GLuint buffer,
                                               GLint size, GLenum type,
                                               GLsizei stride, GLintptr offset);

    // Construction
    EXTDirectStateAccess();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTDirectStateAccess

// GL_EXT_draw_buffers2 extension API specification.
struct EXTDrawBUFFERS2 {
    // Public Members
    void (APIENTRY *glColorMaskIndexed)(GLuint index, GLboolean r, GLboolean g,
                                        GLboolean b, GLboolean a);

    // Construction
    EXTDrawBUFFERS2();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTDrawBUFFERS2

// GL_EXT_draw_instanced extension API specification.
struct EXTDrawInstanced {
    // Public Members
    void (APIENTRY *glDrawArraysInstanced)(GLenum mode, GLint start,
                                           GLsizei count, GLsizei primcount);
    void (APIENTRY *glDrawElementsInstanced)(GLenum mode, GLsizei count,
                                             GLenum type, const void *indices,
                                             GLsizei primcount);

    // Construction
    EXTDrawInstanced();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTDrawInstanced

// GL_EXT_draw_range_elements extension API specification.
struct EXTDrawRangeElements {
    // Public Members
    void (APIENTRY *glDrawRangeElements)(GLenum mode, GLuint start, GLuint end,
                                         GLsizei count, GLenum type,
                                         const void *indices);

    // Construction
    EXTDrawRangeElements();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTDrawRangeElements

// GL_EXT_external_buffer extension API specification.
struct EXTExternalBuffer {
    // Public Members
    void (APIENTRY *glBufferStorageExternal)(GLenum target, GLintptr offset,
                                             GLsizeiptr size,
                                             GLeglClientBufferEXT clientBuffer,
                                             GLbitfield flags);
    void (APIENTRY *glNamedBufferStorageExternal)(GLuint buffer, GLintptr offset,
                                                  GLsizeiptr size,
                                                  GLeglClientBufferEXT clientBuffer,
                                                  GLbitfield flags);

    // Construction
    EXTExternalBuffer();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTExternalBuffer

// GL_EXT_fog_coord extension API specification.
struct EXTFogCoord {
    // Public Members
    void (APIENTRY *glFogCoordPointer)(GLenum type, GLsizei stride,
                                       const void *pointer);
    void (APIENTRY *glFogCoordd)(GLdouble coord);
    void (APIENTRY *glFogCoorddv)(const GLdouble *coord);
    void (APIENTRY *glFogCoordf)(GLfloat coord);
    void (APIENTRY *glFogCoordfv)(const GLfloat *coord);

    // Construction
    EXTFogCoord();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTFogCoord

// GL_EXT_framebuffer_blit extension API specification.
struct EXTFramebufferBlit {
    // Public Members
    void (APIENTRY *glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1,
                                       GLint srcY1, GLint dstX0, GLint dstY0,
                                       GLint dstX1, GLint dstY1, GLbitfield mask,
                                       GLenum filter);

    // Construction
    EXTFramebufferBlit();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTFramebufferBlit

// GL_EXT_framebuffer_multisample extension API specification.
struct EXTFramebufferMultisample {
    // Public Members
    void (APIENTRY *glRenderbufferStorageMultisample)(GLenum target,
                                                      GLsizei samples,
                                                      GLenum internalformat,
                                                      GLsizei width,
                                                      GLsizei height);

    // Construction
    EXTFramebufferMultisample();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTFramebufferMultisample

// GL_EXT_framebuffer_object extension API specification.
struct EXTFramebufferObject {
    // Public Members
    void (APIENTRY *glBindFramebuffer)(GLenum target, GLuint framebuffer);
    void (APIENTRY *glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
    GLenum (APIENTRY *glCheckFramebufferStatus)(GLenum target);
    void (APIENTRY *glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
    void (APIENTRY *glDeleteRenderbuffers)(GLsizei n,
                                           const GLuint *renderbuffers);
    void (APIENTRY *glFramebufferRenderbuffer)(GLenum target, GLenum attachment,
                                               GLenum renderbuffertarget,
                                               GLuint renderbuffer);
    void (APIENTRY *glFramebufferTexture1D)(GLenum target, GLenum attachment,
                                            GLenum textarget, GLuint texture,
                                            GLint level);
    void (APIENTRY *glFramebufferTexture2D)(GLenum target, GLenum attachment,
                                            GLenum textarget, GLuint texture,
                                            GLint level);
    void (APIENTRY *glFramebufferTexture3D)(GLenum target, GLenum attachment,
                                            GLenum textarget, GLuint texture,
                                            GLint level, GLint zoffset);
    void (APIENTRY *glGenFramebuffers)(GLsizei n, GLuint *framebuffers);
    void (APIENTRY *glGenRenderbuffers)(GLsizei n, GLuint *renderbuffers);
    void (APIENTRY *glGenerateMipmap)(GLenum target);
    void (APIENTRY *glGetFramebufferAttachmentParameteriv)(GLenum target,
                                                           GLenum attachment,
                                                           GLenum pname,
                                                           GLint *params);
    void (APIENTRY *glGetRenderbufferParameteriv)(GLenum target, GLenum pname,
                                                  GLint *params);
    GLboolean (APIENTRY *glIsFramebuffer)(GLuint framebuffer);
    GLboolean (APIENTRY *glIsRenderbuffer)(GLuint renderbuffer);
    void (APIENTRY *glRenderbufferStorage)(GLenum target, GLenum internalformat,
                                           GLsizei width, GLsizei height);

    // Construction
    EXTFramebufferObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTFramebufferObject

// GL_EXT_geometry_shader4 extension API specification.
struct EXTGeometrySHADER4 {
    // Public Members
    void (APIENTRY *glProgramParameteri)(GLuint program, GLenum pname,
                                         GLint value);

    // Construction
    EXTGeometrySHADER4();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTGeometrySHADER4

// GL_EXT_gpu_program_parameters extension API specification.
struct EXTGPUProgramParameters {
    // Public Members
    void (APIENTRY *glProgramEnvParameters4fv)(GLenum target, GLuint index,
                                               GLsizei count,
                                               const GLfloat *params);
    void (APIENTRY *glProgramLocalParameters4fv)(GLenum target, GLuint index,
                                                 GLsizei count,
                                                 const GLfloat *params);

    // Construction
    EXTGPUProgramParameters();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTGPUProgramParameters

// GL_EXT_gpu_shader4 extension API specification.
struct EXTGPUSHADER4 {
    // Public Members
    void (APIENTRY *glBindFragDataLocation)(GLuint program, GLuint color,
                                            const GLchar *name);
    GLint (APIENTRY *glGetFragDataLocation)(GLuint program, const GLchar *name);
    void (APIENTRY *glGetUniformuiv)(GLuint program, GLint location,
                                     GLuint *params);
    void (APIENTRY *glGetVertexAttribIiv)(GLuint index, GLenum pname,
                                          GLint *params);
    void (APIENTRY *glGetVertexAttribIuiv)(GLuint index, GLenum pname,
                                           GLuint *params);
    void (APIENTRY *glUniform1ui)(GLint location, GLuint v0);
    void (APIENTRY *glUniform1uiv)(GLint location, GLsizei count,
                                   const GLuint *value);
    void (APIENTRY *glUniform2ui)(GLint location, GLuint v0, GLuint v1);
    void (APIENTRY *glUniform2uiv)(GLint location, GLsizei count,
                                   const GLuint *value);
    void (APIENTRY *glUniform3ui)(GLint location, GLuint v0, GLuint v1,
                                  GLuint v2);
    void (APIENTRY *glUniform3uiv)(GLint location, GLsizei count,
                                   const GLuint *value);
    void (APIENTRY *glUniform4ui)(GLint location, GLuint v0, GLuint v1,
                                  GLuint v2, GLuint v3);
    void (APIENTRY *glUniform4uiv)(GLint location, GLsizei count,
                                   const GLuint *value);
    void (APIENTRY *glVertexAttribI1i)(GLuint index, GLint x);
    void (APIENTRY *glVertexAttribI1iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttribI1ui)(GLuint index, GLuint x);
    void (APIENTRY *glVertexAttribI1uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttribI2i)(GLuint index, GLint x, GLint y);
    void (APIENTRY *glVertexAttribI2iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttribI2ui)(GLuint index, GLuint x, GLuint y);
    void (APIENTRY *glVertexAttribI2uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttribI3i)(GLuint index, GLint x, GLint y, GLint z);
    void (APIENTRY *glVertexAttribI3iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttribI3ui)(GLuint index, GLuint x, GLuint y,
                                        GLuint z);
    void (APIENTRY *glVertexAttribI3uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttribI4bv)(GLuint index, const GLbyte *v);
    void (APIENTRY *glVertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z,
                                       GLint w);
    void (APIENTRY *glVertexAttribI4iv)(GLuint index, const GLint *v);
    void (APIENTRY *glVertexAttribI4sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttribI4ubv)(GLuint index, const GLubyte *v);
    void (APIENTRY *glVertexAttribI4ui)(GLuint index, GLuint x, GLuint y,
                                        GLuint z, GLuint w);
    void (APIENTRY *glVertexAttribI4uiv)(GLuint index, const GLuint *v);
    void (APIENTRY *glVertexAttribI4usv)(GLuint index, const GLushort *v);
    void (APIENTRY *glVertexAttribIPointer)(GLuint index, GLint size,
                                            GLenum type, GLsizei stride,
                                            const void *pointer);

    // Construction
    EXTGPUSHADER4();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTGPUSHADER4

// GL_EXT_histogram extension API specification.
struct EXTHistogram {
    // Public Members
    void (APIENTRY *glGetHistogram)(GLenum target, GLboolean reset,
                                    GLenum format, GLenum type, void *values);
    void (APIENTRY *glGetHistogramParameterfv)(GLenum target, GLenum pname,
                                               GLfloat *params);
    void (APIENTRY *glGetHistogramParameteriv)(GLenum target, GLenum pname,
                                               GLint *params);
    void (APIENTRY *glGetMinmax)(GLenum target, GLboolean reset, GLenum format,
                                 GLenum type, void *values);
    void (APIENTRY *glGetMinmaxParameterfv)(GLenum target, GLenum pname,
                                            GLfloat *params);
    void (APIENTRY *glGetMinmaxParameteriv)(GLenum target, GLenum pname,
                                            GLint *params);
    void (APIENTRY *glHistogram)(GLenum target, GLsizei width,
                                 GLenum internalformat, GLboolean sink);
    void (APIENTRY *glMinmax)(GLenum target, GLenum internalformat,
                              GLboolean sink);
    void (APIENTRY *glResetHistogram)(GLenum target);
    void (APIENTRY *glResetMinmax)(GLenum target);

    // Construction
    EXTHistogram();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTHistogram

// GL_EXT_index_func extension API specification.
struct EXTIndexFunc {
    // Public Members
    void (APIENTRY *glIndexFunc)(GLenum func, GLclampf ref);

    // Construction
    EXTIndexFunc();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTIndexFunc

// GL_EXT_index_material extension API specification.
struct EXTIndexMaterial {
    // Public Members
    void (APIENTRY *glIndexMaterial)(GLenum face, GLenum mode);

    // Construction
    EXTIndexMaterial();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTIndexMaterial

// GL_EXT_light_texture extension API specification.
struct EXTLightTexture {
    // Public Members
    void (APIENTRY *glApplyTexture)(GLenum mode);
    void (APIENTRY *glTextureLight)(GLenum pname);
    void (APIENTRY *glTextureMaterial)(GLenum face, GLenum mode);

    // Construction
    EXTLightTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTLightTexture

// GL_EXT_memory_object extension API specification.
struct EXTMemoryObject {
    // Public Members
    void (APIENTRY *glBufferStorageMem)(GLenum target, GLsizeiptr size,
                                        GLuint memory, GLuint64 offset);
    void (APIENTRY *glCreateMemoryObjects)(GLsizei n, GLuint *memoryObjects);
    void (APIENTRY *glDeleteMemoryObjects)(GLsizei n,
                                           const GLuint *memoryObjects);
    void (APIENTRY *glGetMemoryObjectParameteriv)(GLuint memoryObject,
                                                  GLenum pname, GLint *params);
    void (APIENTRY *glGetUnsignedBytei_v)(GLenum target, GLuint index,
                                          GLubyte *data);
    void (APIENTRY *glGetUnsignedBytev)(GLenum pname, GLubyte *data);
    GLboolean (APIENTRY *glIsMemoryObject)(GLuint memoryObject);
    void (APIENTRY *glMemoryObjectParameteriv)(GLuint memoryObject, GLenum pname,
                                               const GLint *params);
    void (APIENTRY *glNamedBufferStorageMem)(GLuint buffer, GLsizeiptr size,
                                             GLuint memory, GLuint64 offset);
    void (APIENTRY *glTexStorageMem1D)(GLenum target, GLsizei levels,
                                       GLenum internalFormat, GLsizei width,
                                       GLuint memory, GLuint64 offset);
    void (APIENTRY *glTexStorageMem2D)(GLenum target, GLsizei levels,
                                       GLenum internalFormat, GLsizei width,
                                       GLsizei height, GLuint memory,
                                       GLuint64 offset);
    void (APIENTRY *glTexStorageMem2DMultisample)(GLenum target, GLsizei samples,
                                                  GLenum internalFormat,
                                                  GLsizei width, GLsizei height,
                                                  GLboolean fixedSampleLocations,
                                                  GLuint memory, GLuint64 offset);
    void (APIENTRY *glTexStorageMem3D)(GLenum target, GLsizei levels,
                                       GLenum internalFormat, GLsizei width,
                                       GLsizei height, GLsizei depth,
                                       GLuint memory, GLuint64 offset);
    void (APIENTRY *glTexStorageMem3DMultisample)(GLenum target, GLsizei samples,
                                                  GLenum internalFormat,
                                                  GLsizei width, GLsizei height,
                                                  GLsizei depth,
                                                  GLboolean fixedSampleLocations,
                                                  GLuint memory, GLuint64 offset);
    void (APIENTRY *glTextureStorageMem1D)(GLuint texture, GLsizei levels,
                                           GLenum internalFormat, GLsizei width,
                                           GLuint memory, GLuint64 offset);
    void (APIENTRY *glTextureStorageMem2D)(GLuint texture, GLsizei levels,
                                           GLenum internalFormat, GLsizei width,
                                           GLsizei height, GLuint memory,
                                           GLuint64 offset);
    void (APIENTRY *glTextureStorageMem2DMultisample)(GLuint texture,
                                                      GLsizei samples,
                                                      GLenum internalFormat,
                                                      GLsizei width,
                                                      GLsizei height,
                                                      GLboolean fixedSampleLocations,
                                                      GLuint memory,
                                                      GLuint64 offset);
    void (APIENTRY *glTextureStorageMem3D)(GLuint texture, GLsizei levels,
                                           GLenum internalFormat, GLsizei width,
                                           GLsizei height, GLsizei depth,
                                           GLuint memory, GLuint64 offset);
    void (APIENTRY *glTextureStorageMem3DMultisample)(GLuint texture,
                                                      GLsizei samples,
                                                      GLenum internalFormat,
                                                      GLsizei width,
                                                      GLsizei height,
                                                      GLsizei depth,
                                                      GLboolean fixedSampleLocations,
                                                      GLuint memory,
                                                      GLuint64 offset);

    // Construction
    EXTMemoryObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTMemoryObject

// GL_EXT_memory_object_fd extension API specification.
struct EXTMemoryObjectFd {
    // Public Members
    void (APIENTRY *glImportMemoryFd)(GLuint memory, GLuint64 size,
                                      GLenum handleType, GLint fd);

    // Construction
    EXTMemoryObjectFd();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTMemoryObjectFd

// GL_EXT_memory_object_win32 extension API specification.
struct EXTMemoryObjectWIN32 {
    // Public Members
    void (APIENTRY *glImportMemoryWin32Handle)(GLuint memory, GLuint64 size,
                                               GLenum handleType, void *handle);
    void (APIENTRY *glImportMemoryWin32Name)(GLuint memory, GLuint64 size,
                                             GLenum handleType, const void *name);

    // Construction
    EXTMemoryObjectWIN32();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTMemoryObjectWIN32

// GL_EXT_multi_draw_arrays extension API specification.
struct EXTMultiDrawArrays {
    // Public Members
    void (APIENTRY *glMultiDrawArrays)(GLenum mode, const GLint *first,
                                       const GLsizei *count, GLsizei primcount);
    void (APIENTRY *glMultiDrawElements)(GLenum mode, const GLsizei *count,
                                         GLenum type, const void *const*indices,
                                         GLsizei primcount);

    // Construction
    EXTMultiDrawArrays();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTMultiDrawArrays

// GL_EXT_multisample extension API specification.
struct EXTMultisample {
    // Public Members
    void (APIENTRY *glSampleMask)(GLclampf value, GLboolean invert);
    void (APIENTRY *glSamplePattern)(GLenum pattern);

    // Construction
    EXTMultisample();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTMultisample

// GL_EXT_paletted_texture extension API specification.
struct EXTPalettedTexture {
    // Public Members
    void (APIENTRY *glColorTable)(GLenum target, GLenum internalFormat,
                                  GLsizei width, GLenum format, GLenum type,
                                  const void *table);
    void (APIENTRY *glGetColorTable)(GLenum target, GLenum format, GLenum type,
                                     void *data);
    void (APIENTRY *glGetColorTableParameterfv)(GLenum target, GLenum pname,
                                                GLfloat *params);
    void (APIENTRY *glGetColorTableParameteriv)(GLenum target, GLenum pname,
                                                GLint *params);

    // Construction
    EXTPalettedTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTPalettedTexture

// GL_EXT_pixel_transform extension API specification.
struct EXTPixelTransform {
    // Public Members
    void (APIENTRY *glGetPixelTransformParameterfv)(GLenum target, GLenum pname,
                                                    GLfloat *params);
    void (APIENTRY *glGetPixelTransformParameteriv)(GLenum target, GLenum pname,
                                                    GLint *params);
    void (APIENTRY *glPixelTransformParameterf)(GLenum target, GLenum pname,
                                                GLfloat param);
    void (APIENTRY *glPixelTransformParameterfv)(GLenum target, GLenum pname,
                                                 const GLfloat *params);
    void (APIENTRY *glPixelTransformParameteri)(GLenum target, GLenum pname,
                                                GLint param);
    void (APIENTRY *glPixelTransformParameteriv)(GLenum target, GLenum pname,
                                                 const GLint *params);

    // Construction
    EXTPixelTransform();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTPixelTransform

// GL_EXT_point_parameters extension API specification.
struct EXTPointParameters {
    // Public Members
    void (APIENTRY *glPointParameterf)(GLenum pname, GLfloat param);
    void (APIENTRY *glPointParameterfv)(GLenum pname, const GLfloat *params);

    // Construction
    EXTPointParameters();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTPointParameters

// GL_EXT_polygon_offset extension API specification.
struct EXTPolygonOffset {
    // Public Members
    void (APIENTRY *glPolygonOffset)(GLfloat factor, GLfloat bias);

    // Construction
    EXTPolygonOffset();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTPolygonOffset

// GL_EXT_polygon_offset_clamp extension API specification.
struct EXTPolygonOffsetClamp {
    // Public Members
    void (APIENTRY *glPolygonOffsetClamp)(GLfloat factor, GLfloat units,
                                          GLfloat clamp);

    // Construction
    EXTPolygonOffsetClamp();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTPolygonOffsetClamp

// GL_EXT_provoking_vertex extension API specification.
struct EXTProvokingVertex {
    // Public Members
    void (APIENTRY *glProvokingVertex)(GLenum mode);

    // Construction
    EXTProvokingVertex();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTProvokingVertex

// GL_EXT_raster_multisample extension API specification.
struct EXTRasterMultisample {
    // Public Members
    void (APIENTRY *glRasterSamples)(GLuint samples,
                                     GLboolean fixedsamplelocations);

    // Construction
    EXTRasterMultisample();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTRasterMultisample

// GL_EXT_secondary_color extension API specification.
struct EXTSecondaryColor {
    // Public Members
    void (APIENTRY *glSecondaryColor3b)(GLbyte red, GLbyte green, GLbyte blue);
    void (APIENTRY *glSecondaryColor3bv)(const GLbyte *v);
    void (APIENTRY *glSecondaryColor3d)(GLdouble red, GLdouble green,
                                        GLdouble blue);
    void (APIENTRY *glSecondaryColor3dv)(const GLdouble *v);
    void (APIENTRY *glSecondaryColor3f)(GLfloat red, GLfloat green, GLfloat blue);
    void (APIENTRY *glSecondaryColor3fv)(const GLfloat *v);
    void (APIENTRY *glSecondaryColor3i)(GLint red, GLint green, GLint blue);
    void (APIENTRY *glSecondaryColor3iv)(const GLint *v);
    void (APIENTRY *glSecondaryColor3s)(GLshort red, GLshort green, GLshort blue);
    void (APIENTRY *glSecondaryColor3sv)(const GLshort *v);
    void (APIENTRY *glSecondaryColor3ub)(GLubyte red, GLubyte green,
                                         GLubyte blue);
    void (APIENTRY *glSecondaryColor3ubv)(const GLubyte *v);
    void (APIENTRY *glSecondaryColor3ui)(GLuint red, GLuint green, GLuint blue);
    void (APIENTRY *glSecondaryColor3uiv)(const GLuint *v);
    void (APIENTRY *glSecondaryColor3us)(GLushort red, GLushort green,
                                         GLushort blue);
    void (APIENTRY *glSecondaryColor3usv)(const GLushort *v);
    void (APIENTRY *glSecondaryColorPointer)(GLint size, GLenum type,
                                             GLsizei stride, const void *pointer);

    // Construction
    EXTSecondaryColor();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTSecondaryColor

// GL_EXT_semaphore extension API specification.
struct EXTSemaphore {
    // Public Members
    void (APIENTRY *glDeleteSemaphores)(GLsizei n, const GLuint *semaphores);
    void (APIENTRY *glGenSemaphores)(GLsizei n, GLuint *semaphores);
    void (APIENTRY *glGetSemaphoreParameterui64v)(GLuint semaphore, GLenum pname,
                                                  GLuint64 *params);
    GLboolean (APIENTRY *glIsSemaphore)(GLuint semaphore);
    void (APIENTRY *glSemaphoreParameterui64v)(GLuint semaphore, GLenum pname,
                                               const GLuint64 *params);
    void (APIENTRY *glSignalSemaphore)(GLuint semaphore,
                                       GLuint numBufferBarriers,
                                       const GLuint *buffers,
                                       GLuint numTextureBarriers,
                                       const GLuint *textures,
                                       const GLenum *dstLayouts);
    void (APIENTRY *glWaitSemaphore)(GLuint semaphore, GLuint numBufferBarriers,
                                     const GLuint *buffers,
                                     GLuint numTextureBarriers,
                                     const GLuint *textures,
                                     const GLenum *srcLayouts);

    // Construction
    EXTSemaphore();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTSemaphore

// GL_EXT_semaphore_fd extension API specification.
struct EXTSemaphoreFd {
    // Public Members
    void (APIENTRY *glImportSemaphoreFd)(GLuint semaphore, GLenum handleType,
                                         GLint fd);

    // Construction
    EXTSemaphoreFd();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTSemaphoreFd

// GL_EXT_semaphore_win32 extension API specification.
struct EXTSemaphoreWIN32 {
    // Public Members
    void (APIENTRY *glImportSemaphoreWin32Handle)(GLuint semaphore,
                                                  GLenum handleType,
                                                  void *handle);
    void (APIENTRY *glImportSemaphoreWin32Name)(GLuint semaphore,
                                                GLenum handleType,
                                                const void *name);

    // Construction
    EXTSemaphoreWIN32();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTSemaphoreWIN32

// GL_EXT_separate_shader_objects extension API specification.
struct EXTSeparateShaderObjects {
    // Public Members
    void (APIENTRY *glActiveProgram)(GLuint program);
    GLuint (APIENTRY *glCreateShaderProgram)(GLenum type, const GLchar *string);
    void (APIENTRY *glUseShaderProgram)(GLenum type, GLuint program);

    // Construction
    EXTSeparateShaderObjects();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTSeparateShaderObjects

// GL_EXT_shader_framebuffer_fetch_non_coherent extension API specification.
struct EXTShaderFramebufferFetchNonCoherent {
    // Public Members
    void (APIENTRY *glFramebufferFetchBarrier)();

    // Construction
    EXTShaderFramebufferFetchNonCoherent();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTShaderFramebufferFetchNonCoherent

// GL_EXT_shader_image_load_store extension API specification.
struct EXTShaderImageLoadStore {
    // Public Members
    void (APIENTRY *glBindImageTexture)(GLuint index, GLuint texture,
                                        GLint level, GLboolean layered,
                                        GLint layer, GLenum access, GLint format);
    void (APIENTRY *glMemoryBarrier)(GLbitfield barriers);

    // Construction
    EXTShaderImageLoadStore();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTShaderImageLoadStore

// GL_EXT_stencil_clear_tag extension API specification.
struct EXTStencilClearTag {
    // Public Members
    void (APIENTRY *glStencilClearTag)(GLsizei stencilTagBits,
                                       GLuint stencilClearTag);

    // Construction
    EXTStencilClearTag();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTStencilClearTag

// GL_EXT_stencil_two_side extension API specification.
struct EXTStencilTwoSide {
    // Public Members
    void (APIENTRY *glActiveStencilFace)(GLenum face);

    // Construction
    EXTStencilTwoSide();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTStencilTwoSide

// GL_EXT_subtexture extension API specification.
struct EXTSubtexture {
    // Public Members
    void (APIENTRY *glTexSubImage1D)(GLenum target, GLint level, GLint xoffset,
                                     GLsizei width, GLenum format, GLenum type,
                                     const void *pixels);
    void (APIENTRY *glTexSubImage2D)(GLenum target, GLint level, GLint xoffset,
                                     GLint yoffset, GLsizei width,
                                     GLsizei height, GLenum format, GLenum type,
                                     const void *pixels);

    // Construction
    EXTSubtexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTSubtexture

// GL_EXT_texture3D extension API specification.
struct EXTTEXTURE3D {
    // Public Members
    void (APIENTRY *glTexImage3D)(GLenum target, GLint level,
                                  GLenum internalformat, GLsizei width,
                                  GLsizei height, GLsizei depth, GLint border,
                                  GLenum format, GLenum type, const void *pixels);
    void (APIENTRY *glTexSubImage3D)(GLenum target, GLint level, GLint xoffset,
                                     GLint yoffset, GLint zoffset, GLsizei width,
                                     GLsizei height, GLsizei depth,
                                     GLenum format, GLenum type,
                                     const void *pixels);

    // Construction
    EXTTEXTURE3D();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTEXTURE3D

// GL_EXT_texture_array extension API specification.
struct EXTTextureArray {
    // Public Members
    void (APIENTRY *glFramebufferTextureLayer)(GLenum target, GLenum attachment,
                                               GLuint texture, GLint level,
                                               GLint layer);

    // Construction
    EXTTextureArray();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTextureArray

// GL_EXT_texture_buffer_object extension API specification.
struct EXTTextureBufferObject {
    // Public Members
    void (APIENTRY *glTexBuffer)(GLenum target, GLenum internalformat,
                                 GLuint buffer);

    // Construction
    EXTTextureBufferObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTextureBufferObject

// GL_EXT_texture_integer extension API specification.
struct EXTTextureInteger {
    // Public Members
    void (APIENTRY *glClearColorIi)(GLint red, GLint green, GLint blue,
                                    GLint alpha);
    void (APIENTRY *glClearColorIui)(GLuint red, GLuint green, GLuint blue,
                                     GLuint alpha);
    void (APIENTRY *glGetTexParameterIiv)(GLenum target, GLenum pname,
                                          GLint *params);
    void (APIENTRY *glGetTexParameterIuiv)(GLenum target, GLenum pname,
                                           GLuint *params);
    void (APIENTRY *glTexParameterIiv)(GLenum target, GLenum pname,
                                       const GLint *params);
    void (APIENTRY *glTexParameterIuiv)(GLenum target, GLenum pname,
                                        const GLuint *params);

    // Construction
    EXTTextureInteger();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTextureInteger

// GL_EXT_texture_object extension API specification.
struct EXTTextureObject {
    // Public Members
    GLboolean (APIENTRY *glAreTexturesResident)(GLsizei n,
                                                const GLuint *textures,
                                                GLboolean *residences);
    void (APIENTRY *glBindTexture)(GLenum target, GLuint texture);
    void (APIENTRY *glDeleteTextures)(GLsizei n, const GLuint *textures);
    void (APIENTRY *glGenTextures)(GLsizei n, GLuint *textures);
    GLboolean (APIENTRY *glIsTexture)(GLuint texture);
    void (APIENTRY *glPrioritizeTextures)(GLsizei n, const GLuint *textures,
                                          const GLclampf *priorities);

    // Construction
    EXTTextureObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTextureObject

// GL_EXT_texture_perturb_normal extension API specification.
struct EXTTexturePerturbNormal {
    // Public Members
    void (APIENTRY *glTextureNormal)(GLenum mode);

    // Construction
    EXTTexturePerturbNormal();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTexturePerturbNormal

// GL_EXT_texture_storage extension API specification.
struct EXTTextureStorage {
    // Public Members
    void (APIENTRY *glTexStorage1D)(GLenum target, GLsizei levels,
                                    GLenum internalformat, GLsizei width);
    void (APIENTRY *glTexStorage2D)(GLenum target, GLsizei levels,
                                    GLenum internalformat, GLsizei width,
                                    GLsizei height);
    void (APIENTRY *glTexStorage3D)(GLenum target, GLsizei levels,
                                    GLenum internalformat, GLsizei width,
                                    GLsizei height, GLsizei depth);

    // Construction
    EXTTextureStorage();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTextureStorage

// GL_EXT_timer_query extension API specification.
struct EXTTimerQuery {
    // Public Members
    void (APIENTRY *glGetQueryObjecti64v)(GLuint id, GLenum pname,
                                          GLint64 *params);
    void (APIENTRY *glGetQueryObjectui64v)(GLuint id, GLenum pname,
                                           GLuint64 *params);

    // Construction
    EXTTimerQuery();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTimerQuery

// GL_EXT_transform_feedback extension API specification.
struct EXTTransformFeedback {
    // Public Members
    void (APIENTRY *glBeginTransformFeedback)(GLenum primitiveMode);
    void (APIENTRY *glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
    void (APIENTRY *glBindBufferOffset)(GLenum target, GLuint index,
                                        GLuint buffer, GLintptr offset);
    void (APIENTRY *glBindBufferRange)(GLenum target, GLuint index,
                                       GLuint buffer, GLintptr offset,
                                       GLsizeiptr size);
    void (APIENTRY *glEndTransformFeedback)();
    void (APIENTRY *glGetTransformFeedbackVarying)(GLuint program, GLuint index,
                                                   GLsizei bufSize,
                                                   GLsizei *length,
                                                   GLsizei *size, GLenum *type,
                                                   GLchar *name);
    void (APIENTRY *glTransformFeedbackVaryings)(GLuint program, GLsizei count,
                                                 const GLchar *const*varyings,
                                                 GLenum bufferMode);

    // Construction
    EXTTransformFeedback();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTTransformFeedback

// GL_EXT_vertex_array extension API specification.
struct EXTVertexArray {
    // Public Members
    void (APIENTRY *glArrayElement)(GLint i);
    void (APIENTRY *glColorPointer)(GLint size, GLenum type, GLsizei stride,
                                    GLsizei count, const void *pointer);
    void (APIENTRY *glDrawArrays)(GLenum mode, GLint first, GLsizei count);
    void (APIENTRY *glEdgeFlagPointer)(GLsizei stride, GLsizei count,
                                       const GLboolean *pointer);
    void (APIENTRY *glGetPointerv)(GLenum pname, void **params);
    void (APIENTRY *glIndexPointer)(GLenum type, GLsizei stride, GLsizei count,
                                    const void *pointer);
    void (APIENTRY *glNormalPointer)(GLenum type, GLsizei stride, GLsizei count,
                                     const void *pointer);
    void (APIENTRY *glTexCoordPointer)(GLint size, GLenum type, GLsizei stride,
                                       GLsizei count, const void *pointer);
    void (APIENTRY *glVertexPointer)(GLint size, GLenum type, GLsizei stride,
                                     GLsizei count, const void *pointer);

    // Construction
    EXTVertexArray();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTVertexArray

// GL_EXT_vertex_attrib_64bit extension API specification.
struct EXTVertexAttrib64BIT {
    // Public Members
    void (APIENTRY *glGetVertexAttribLdv)(GLuint index, GLenum pname,
                                          GLdouble *params);
    void (APIENTRY *glVertexAttribL1d)(GLuint index, GLdouble x);
    void (APIENTRY *glVertexAttribL1dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttribL2d)(GLuint index, GLdouble x, GLdouble y);
    void (APIENTRY *glVertexAttribL2dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttribL3d)(GLuint index, GLdouble x, GLdouble y,
                                       GLdouble z);
    void (APIENTRY *glVertexAttribL3dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttribL4d)(GLuint index, GLdouble x, GLdouble y,
                                       GLdouble z, GLdouble w);
    void (APIENTRY *glVertexAttribL4dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttribLPointer)(GLuint index, GLint size,
                                            GLenum type, GLsizei stride,
                                            const void *pointer);

    // Construction
    EXTVertexAttrib64BIT();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTVertexAttrib64BIT

// GL_EXT_vertex_shader extension API specification.
struct EXTVertexShader {
    // Public Members
    void (APIENTRY *glBeginVertexShader)();
    GLuint (APIENTRY *glBindLightParameter)(GLenum light, GLenum value);
    GLuint (APIENTRY *glBindMaterialParameter)(GLenum face, GLenum value);
    GLuint (APIENTRY *glBindParameter)(GLenum value);
    GLuint (APIENTRY *glBindTexGenParameter)(GLenum unit, GLenum coord,
                                             GLenum value);
    GLuint (APIENTRY *glBindTextureUnitParameter)(GLenum unit, GLenum value);
    void (APIENTRY *glBindVertexShader)(GLuint id);
    void (APIENTRY *glDeleteVertexShader)(GLuint id);
    void (APIENTRY *glDisableVariantClientState)(GLuint id);
    void (APIENTRY *glEnableVariantClientState)(GLuint id);
    void (APIENTRY *glEndVertexShader)();
    void (APIENTRY *glExtractComponent)(GLuint res, GLuint src, GLuint num);
    GLuint (APIENTRY *glGenSymbols)(GLenum datatype, GLenum storagetype,
                                    GLenum range, GLuint components);
    GLuint (APIENTRY *glGenVertexShaders)(GLuint range);
    void (APIENTRY *glGetInvariantBooleanv)(GLuint id, GLenum value,
                                            GLboolean *data);
    void (APIENTRY *glGetInvariantFloatv)(GLuint id, GLenum value, GLfloat *data);
    void (APIENTRY *glGetInvariantIntegerv)(GLuint id, GLenum value, GLint *data);
    void (APIENTRY *glGetLocalConstantBooleanv)(GLuint id, GLenum value,
                                                GLboolean *data);
    void (APIENTRY *glGetLocalConstantFloatv)(GLuint id, GLenum value,
                                              GLfloat *data);
    void (APIENTRY *glGetLocalConstantIntegerv)(GLuint id, GLenum value,
                                                GLint *data);
    void (APIENTRY *glGetVariantBooleanv)(GLuint id, GLenum value,
                                          GLboolean *data);
    void (APIENTRY *glGetVariantFloatv)(GLuint id, GLenum value, GLfloat *data);
    void (APIENTRY *glGetVariantIntegerv)(GLuint id, GLenum value, GLint *data);
    void (APIENTRY *glGetVariantPointerv)(GLuint id, GLenum value, void **data);
    void (APIENTRY *glInsertComponent)(GLuint res, GLuint src, GLuint num);
    GLboolean (APIENTRY *glIsVariantEnabled)(GLuint id, GLenum cap);
    void (APIENTRY *glSetInvariant)(GLuint id, GLenum type, const void *addr);
    void (APIENTRY *glSetLocalConstant)(GLuint id, GLenum type, const void *addr);
    void (APIENTRY *glShaderOp1)(GLenum op, GLuint res, GLuint arg1);
    void (APIENTRY *glShaderOp2)(GLenum op, GLuint res, GLuint arg1, GLuint arg2);
    void (APIENTRY *glShaderOp3)(GLenum op, GLuint res, GLuint arg1, GLuint arg2,
                                 GLuint arg3);
    void (APIENTRY *glSwizzle)(GLuint res, GLuint in, GLenum outX, GLenum outY,
                               GLenum outZ, GLenum outW);
    void (APIENTRY *glVariantPointer)(GLuint id, GLenum type, GLuint stride,
                                      const void *addr);
    void (APIENTRY *glVariantbv)(GLuint id, const GLbyte *addr);
    void (APIENTRY *glVariantdv)(GLuint id, const GLdouble *addr);
    void (APIENTRY *glVariantfv)(GLuint id, const GLfloat *addr);
    void (APIENTRY *glVariantiv)(GLuint id, const GLint *addr);
    void (APIENTRY *glVariantsv)(GLuint id, const GLshort *addr);
    void (APIENTRY *glVariantubv)(GLuint id, const GLubyte *addr);
    void (APIENTRY *glVariantuiv)(GLuint id, const GLuint *addr);
    void (APIENTRY *glVariantusv)(GLuint id, const GLushort *addr);
    void (APIENTRY *glWriteMask)(GLuint res, GLuint in, GLenum outX, GLenum outY,
                                 GLenum outZ, GLenum outW);

    // Construction
    EXTVertexShader();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTVertexShader

// GL_EXT_vertex_weighting extension API specification.
struct EXTVertexWeighting {
    // Public Members
    void (APIENTRY *glVertexWeightPointer)(GLint size, GLenum type,
                                           GLsizei stride, const void *pointer);
    void (APIENTRY *glVertexWeightf)(GLfloat weight);
    void (APIENTRY *glVertexWeightfv)(const GLfloat *weight);

    // Construction
    EXTVertexWeighting();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTVertexWeighting

// GL_EXT_win32_keyed_mutex extension API specification.
struct EXTWIN32KeyedMutex {
    // Public Members
    GLboolean (APIENTRY *glAcquireKeyedMutexWin32)(GLuint memory, GLuint64 key,
                                                   GLuint timeout);
    GLboolean (APIENTRY *glReleaseKeyedMutexWin32)(GLuint memory, GLuint64 key);

    // Construction
    EXTWIN32KeyedMutex();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTWIN32KeyedMutex

// GL_EXT_window_rectangles extension API specification.
struct EXTWindowRectangles {
    // Public Members
    void (APIENTRY *glWindowRectangles)(GLenum mode, GLsizei count,
                                        const GLint *box);

    // Construction
    EXTWindowRectangles();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTWindowRectangles

// GL_EXT_x11_sync_object extension API specification.
struct EXTX11SyncObject {
    // Public Members
    GLsync (APIENTRY *glImportSync)(GLenum external_sync_type,
                                    GLintptr external_sync, GLbitfield flags);

    // Construction
    EXTX11SyncObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct EXTX11SyncObject

// GL_GREMEDY_frame_terminator extension API specification.
struct GremedyFrameTerminator {
    // Public Members
    void (APIENTRY *glFrameTerminatorGREMEDY)();

    // Construction
    GremedyFrameTerminator();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct GremedyFrameTerminator

// GL_GREMEDY_string_marker extension API specification.
struct GremedyStringMarker {
    // Public Members
    void (APIENTRY *glStringMarkerGREMEDY)(GLsizei len, const void *string);

    // Construction
    GremedyStringMarker();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct GremedyStringMarker

// GL_HP_image_transform extension API specification.
struct HPImageTransform {
    // Public Members
    void (APIENTRY *glGetImageTransformParameterfv)(GLenum target, GLenum pname,
                                                    GLfloat *params);
    void (APIENTRY *glGetImageTransformParameteriv)(GLenum target, GLenum pname,
                                                    GLint *params);
    void (APIENTRY *glImageTransformParameterf)(GLenum target, GLenum pname,
                                                GLfloat param);
    void (APIENTRY *glImageTransformParameterfv)(GLenum target, GLenum pname,
                                                 const GLfloat *params);
    void (APIENTRY *glImageTransformParameteri)(GLenum target, GLenum pname,
                                                GLint param);
    void (APIENTRY *glImageTransformParameteriv)(GLenum target, GLenum pname,
                                                 const GLint *params);

    // Construction
    HPImageTransform();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct HPImageTransform

// GL_IBM_multimode_draw_arrays extension API specification.
struct IBMMultimodeDrawArrays {
    // Public Members
    void (APIENTRY *glMultiModeDrawArrays)(const GLenum *mode,
                                           const GLint *first,
                                           const GLsizei *count,
                                           GLsizei primcount, GLint modestride);
    void (APIENTRY *glMultiModeDrawElements)(const GLenum *mode,
                                             const GLsizei *count, GLenum type,
                                             const void *const*indices,
                                             GLsizei primcount, GLint modestride);

    // Construction
    IBMMultimodeDrawArrays();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct IBMMultimodeDrawArrays

// GL_IBM_static_data extension API specification.
struct IBMStaticData {
    // Public Members
    void (APIENTRY *glFlushStaticData)(GLenum target);

    // Construction
    IBMStaticData();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct IBMStaticData

// GL_IBM_vertex_array_lists extension API specification.
struct IBMVertexArrayLists {
    // Public Members
    void (APIENTRY *glColorPointerList)(GLint size, GLenum type, GLint stride,
                                        const void **pointer, GLint ptrstride);
    void (APIENTRY *glEdgeFlagPointerList)(GLint stride,
                                           const GLboolean **pointer,
                                           GLint ptrstride);
    void (APIENTRY *glFogCoordPointerList)(GLenum type, GLint stride,
                                           const void **pointer, GLint ptrstride);
    void (APIENTRY *glIndexPointerList)(GLenum type, GLint stride,
                                        const void **pointer, GLint ptrstride);
    void (APIENTRY *glNormalPointerList)(GLenum type, GLint stride,
                                         const void **pointer, GLint ptrstride);
    void (APIENTRY *glSecondaryColorPointerList)(GLint size, GLenum type,
                                                 GLint stride,
                                                 const void **pointer,
                                                 GLint ptrstride);
    void (APIENTRY *glTexCoordPointerList)(GLint size, GLenum type, GLint stride,
                                           const void **pointer, GLint ptrstride);
    void (APIENTRY *glVertexPointerList)(GLint size, GLenum type, GLint stride,
                                         const void **pointer, GLint ptrstride);

    // Construction
    IBMVertexArrayLists();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct IBMVertexArrayLists

// GL_INGR_blend_func_separate extension API specification.
struct IngrBlendFuncSeparate {
    // Public Members
    void (APIENTRY *glBlendFuncSeparateINGR)(GLenum sfactorRGB,
                                             GLenum dfactorRGB,
                                             GLenum sfactorAlpha,
                                             GLenum dfactorAlpha);

    // Construction
    IngrBlendFuncSeparate();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct IngrBlendFuncSeparate

// GL_INTEL_framebuffer_CMAA extension API specification.
struct INTELFramebufferCmaa {
    // Public Members
    void (APIENTRY *glApplyFramebufferAttachmentCMAA)();

    // Construction
    INTELFramebufferCmaa();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct INTELFramebufferCmaa

// GL_INTEL_map_texture extension API specification.
struct INTELMapTexture {
    // Public Members
    void *(APIENTRY *glMapTexture2D)(GLuint texture, GLint level,
                                     GLbitfield access, GLint *stride,
                                     GLenum *layout);
    void (APIENTRY *glSyncTexture)(GLuint texture);
    void (APIENTRY *glUnmapTexture2D)(GLuint texture, GLint level);

    // Construction
    INTELMapTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct INTELMapTexture

// GL_INTEL_parallel_arrays extension API specification.
struct INTELParallelArrays {
    // Public Members
    void (APIENTRY *glColorPointerv)(GLint size, GLenum type,
                                     const void **pointer);
    void (APIENTRY *glNormalPointerv)(GLenum type, const void **pointer);
    void (APIENTRY *glTexCoordPointerv)(GLint size, GLenum type,
                                        const void **pointer);
    void (APIENTRY *glVertexPointerv)(GLint size, GLenum type,
                                      const void **pointer);

    // Construction
    INTELParallelArrays();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct INTELParallelArrays

// GL_INTEL_performance_query extension API specification.
struct INTELPerformanceQuery {
    // Public Members
    void (APIENTRY *glBeginPerfQuery)(GLuint queryHandle);
    void (APIENTRY *glCreatePerfQuery)(GLuint queryId, GLuint *queryHandle);
    void (APIENTRY *glDeletePerfQuery)(GLuint queryHandle);
    void (APIENTRY *glEndPerfQuery)(GLuint queryHandle);
    void (APIENTRY *glGetFirstPerfQueryId)(GLuint *queryId);
    void (APIENTRY *glGetNextPerfQueryId)(GLuint queryId, GLuint *nextQueryId);
    void (APIENTRY *glGetPerfCounterInfo)(GLuint queryId, GLuint counterId,
                                          GLuint counterNameLength,
                                          GLchar *counterName,
                                          GLuint counterDescLength,
                                          GLchar *counterDesc,
                                          GLuint *counterOffset,
                                          GLuint *counterDataSize,
                                          GLuint *counterTypeEnum,
                                          GLuint *counterDataTypeEnum,
                                          GLuint64 *rawCounterMaxValue);
    void (APIENTRY *glGetPerfQueryData)(GLuint queryHandle, GLuint flags,
                                        GLsizei dataSize, void *data,
                                        GLuint *bytesWritten);
    void (APIENTRY *glGetPerfQueryIdByName)(GLchar *queryName, GLuint *queryId);
    void (APIENTRY *glGetPerfQueryInfo)(GLuint queryId, GLuint queryNameLength,
                                        GLchar *queryName, GLuint *dataSize,
                                        GLuint *noCounters, GLuint *noInstances,
                                        GLuint *capsMask);

    // Construction
    INTELPerformanceQuery();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct INTELPerformanceQuery

// GL_KHR_blend_equation_advanced extension API specification.
struct KHRBlendEquationAdvanced {
    // Public Members
    void (APIENTRY *glBlendBarrier)();

    // Construction
    KHRBlendEquationAdvanced();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct KHRBlendEquationAdvanced

// GL_KHR_parallel_shader_compile extension API specification.
struct KHRParallelShaderCompile {
    // Public Members
    void (APIENTRY *glMaxShaderCompilerThreads)(GLuint count);

    // Construction
    KHRParallelShaderCompile();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct KHRParallelShaderCompile

// GL_MESA_framebuffer_flip_y extension API specification.
struct MESAFramebufferFlipY {
    // Public Members
    void (APIENTRY *glFramebufferParameteri)(GLenum target, GLenum pname,
                                             GLint param);
    void (APIENTRY *glGetFramebufferParameteriv)(GLenum target, GLenum pname,
                                                 GLint *params);

    // Construction
    MESAFramebufferFlipY();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct MESAFramebufferFlipY

// GL_MESA_resize_buffers extension API specification.
struct MESAResizeBuffers {
    // Public Members
    void (APIENTRY *glResizeBuffers)();

    // Construction
    MESAResizeBuffers();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct MESAResizeBuffers

// GL_MESA_window_pos extension API specification.
struct MESAWindowPos {
    // Public Members
    void (APIENTRY *glWindowPos2d)(GLdouble x, GLdouble y);
    void (APIENTRY *glWindowPos2dv)(const GLdouble *v);
    void (APIENTRY *glWindowPos2f)(GLfloat x, GLfloat y);
    void (APIENTRY *glWindowPos2fv)(const GLfloat *v);
    void (APIENTRY *glWindowPos2i)(GLint x, GLint y);
    void (APIENTRY *glWindowPos2iv)(const GLint *v);
    void (APIENTRY *glWindowPos2s)(GLshort x, GLshort y);
    void (APIENTRY *glWindowPos2sv)(const GLshort *v);
    void (APIENTRY *glWindowPos3d)(GLdouble x, GLdouble y, GLdouble z);
    void (APIENTRY *glWindowPos3dv)(const GLdouble *v);
    void (APIENTRY *glWindowPos3f)(GLfloat x, GLfloat y, GLfloat z);
    void (APIENTRY *glWindowPos3fv)(const GLfloat *v);
    void (APIENTRY *glWindowPos3i)(GLint x, GLint y, GLint z);
    void (APIENTRY *glWindowPos3iv)(const GLint *v);
    void (APIENTRY *glWindowPos3s)(GLshort x, GLshort y, GLshort z);
    void (APIENTRY *glWindowPos3sv)(const GLshort *v);
    void (APIENTRY *glWindowPos4d)(GLdouble x, GLdouble y, GLdouble z,
                                   GLdouble w);
    void (APIENTRY *glWindowPos4dv)(const GLdouble *v);
    void (APIENTRY *glWindowPos4f)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void (APIENTRY *glWindowPos4fv)(const GLfloat *v);
    void (APIENTRY *glWindowPos4i)(GLint x, GLint y, GLint z, GLint w);
    void (APIENTRY *glWindowPos4iv)(const GLint *v);
    void (APIENTRY *glWindowPos4s)(GLshort x, GLshort y, GLshort z, GLshort w);
    void (APIENTRY *glWindowPos4sv)(const GLshort *v);

    // Construction
    MESAWindowPos();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct MESAWindowPos

// GL_NVX_conditional_render extension API specification.
struct NVXConditionalRender {
    // Public Members
    void (APIENTRY *glBeginConditionalRender)(GLuint id);
    void (APIENTRY *glEndConditionalRender)();

    // Construction
    NVXConditionalRender();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVXConditionalRender

// GL_NVX_gpu_multicast2 extension API specification.
struct NVXGPUMULTICAST2 {
    // Public Members
    GLuint (APIENTRY *glAsyncCopyBufferSubData)(GLsizei waitSemaphoreCount,
                                                const GLuint *waitSemaphoreArray,
                                                const GLuint64 *fenceValueArray,
                                                GLuint readGpu,
                                                GLbitfield writeGpuMask,
                                                GLuint readBuffer,
                                                GLuint writeBuffer,
                                                GLintptr readOffset,
                                                GLintptr writeOffset,
                                                GLsizeiptr size,
                                                GLsizei signalSemaphoreCount,
                                                const GLuint *signalSemaphoreArray,
                                                const GLuint64 *signalValueArray);
    GLuint (APIENTRY *glAsyncCopyImageSubData)(GLsizei waitSemaphoreCount,
                                               const GLuint *waitSemaphoreArray,
                                               const GLuint64 *waitValueArray,
                                               GLuint srcGpu,
                                               GLbitfield dstGpuMask,
                                               GLuint srcName, GLenum srcTarget,
                                               GLint srcLevel, GLint srcX,
                                               GLint srcY, GLint srcZ,
                                               GLuint dstName, GLenum dstTarget,
                                               GLint dstLevel, GLint dstX,
                                               GLint dstY, GLint dstZ,
                                               GLsizei srcWidth,
                                               GLsizei srcHeight,
                                               GLsizei srcDepth,
                                               GLsizei signalSemaphoreCount,
                                               const GLuint *signalSemaphoreArray,
                                               const GLuint64 *signalValueArray);
    void (APIENTRY *glMulticastScissorArrayv)(GLuint gpu, GLuint first,
                                              GLsizei count, const GLint *v);
    void (APIENTRY *glMulticastViewportArrayv)(GLuint gpu, GLuint first,
                                               GLsizei count, const GLfloat *v);
    void (APIENTRY *glMulticastViewportPositionWScale)(GLuint gpu, GLuint index,
                                                       GLfloat xcoeff,
                                                       GLfloat ycoeff);
    void (APIENTRY *glUploadGpuMask)(GLbitfield mask);

    // Construction
    NVXGPUMULTICAST2();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVXGPUMULTICAST2

// GL_NVX_linked_gpu_multicast extension API specification.
struct NVXLinkedGPUMulticast {
    // Public Members
    void (APIENTRY *glLGPUCopyImageSubData)(GLuint sourceGpu,
                                            GLbitfield destinationGpuMask,
                                            GLuint srcName, GLenum srcTarget,
                                            GLint srcLevel, GLint srcX,
                                            GLint srxY, GLint srcZ,
                                            GLuint dstName, GLenum dstTarget,
                                            GLint dstLevel, GLint dstX,
                                            GLint dstY, GLint dstZ,
                                            GLsizei width, GLsizei height,
                                            GLsizei depth);
    void (APIENTRY *glLGPUInterlock)();
    void (APIENTRY *glLGPUNamedBufferSubData)(GLbitfield gpuMask, GLuint buffer,
                                              GLintptr offset, GLsizeiptr size,
                                              const void *data);

    // Construction
    NVXLinkedGPUMulticast();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVXLinkedGPUMulticast

// GL_NVX_progress_fence extension API specification.
struct NVXProgressFence {
    // Public Members
    void (APIENTRY *glClientWaitSemaphoreui64)(GLsizei fenceObjectCount,
                                               const GLuint *semaphoreArray,
                                               const GLuint64 *fenceValueArray);
    GLuint (APIENTRY *glCreateProgressFence)();
    void (APIENTRY *glSignalSemaphoreui64)(GLuint signalGpu,
                                           GLsizei fenceObjectCount,
                                           const GLuint *semaphoreArray,
                                           const GLuint64 *fenceValueArray);
    void (APIENTRY *glWaitSemaphoreui64)(GLuint waitGpu,
                                         GLsizei fenceObjectCount,
                                         const GLuint *semaphoreArray,
                                         const GLuint64 *fenceValueArray);

    // Construction
    NVXProgressFence();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVXProgressFence

// GL_NV_alpha_to_coverage_dither_control extension API specification.
struct NVAlphaToCoverageDitherControl {
    // Public Members
    void (APIENTRY *glAlphaToCoverageDitherControl)(GLenum mode);

    // Construction
    NVAlphaToCoverageDitherControl();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVAlphaToCoverageDitherControl

// GL_NV_bindless_multi_draw_indirect extension API specification.
struct NVBindlessMultiDrawIndirect {
    // Public Members
    void (APIENTRY *glMultiDrawArraysIndirectBindless)(GLenum mode,
                                                       const void *indirect,
                                                       GLsizei drawCount,
                                                       GLsizei stride,
                                                       GLint vertexBufferCount);
    void (APIENTRY *glMultiDrawElementsIndirectBindless)(GLenum mode,
                                                         GLenum type,
                                                         const void *indirect,
                                                         GLsizei drawCount,
                                                         GLsizei stride,
                                                         GLint vertexBufferCount);

    // Construction
    NVBindlessMultiDrawIndirect();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVBindlessMultiDrawIndirect

// GL_NV_bindless_multi_draw_indirect_count extension API specification.
struct NVBindlessMultiDrawIndirectCount {
    // Public Members
    void (APIENTRY *glMultiDrawArraysIndirectBindlessCount)(GLenum mode,
                                                            const void *indirect,
                                                            GLsizei drawCount,
                                                            GLsizei maxDrawCount,
                                                            GLsizei stride,
                                                            GLint vertexBufferCount);
    void (APIENTRY *glMultiDrawElementsIndirectBindlessCount)(GLenum mode,
                                                              GLenum type,
                                                              const void *indirect,
                                                              GLsizei drawCount,
                                                              GLsizei maxDrawCount,
                                                              GLsizei stride,
                                                              GLint vertexBufferCount);

    // Construction
    NVBindlessMultiDrawIndirectCount();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVBindlessMultiDrawIndirectCount

// GL_NV_bindless_texture extension API specification.
struct NVBindlessTexture {
    // Public Members
    GLuint64 (APIENTRY *glGetImageHandle)(GLuint texture, GLint level,
                                          GLboolean layered, GLint layer,
                                          GLenum format);
    GLuint64 (APIENTRY *glGetTextureHandle)(GLuint texture);
    GLuint64 (APIENTRY *glGetTextureSamplerHandle)(GLuint texture,
                                                   GLuint sampler);
    GLboolean (APIENTRY *glIsImageHandleResident)(GLuint64 handle);
    GLboolean (APIENTRY *glIsTextureHandleResident)(GLuint64 handle);
    void (APIENTRY *glMakeImageHandleNonResident)(GLuint64 handle);
    void (APIENTRY *glMakeImageHandleResident)(GLuint64 handle, GLenum access);
    void (APIENTRY *glMakeTextureHandleNonResident)(GLuint64 handle);
    void (APIENTRY *glMakeTextureHandleResident)(GLuint64 handle);
    void (APIENTRY *glProgramUniformHandleui64)(GLuint program, GLint location,
                                                GLuint64 value);
    void (APIENTRY *glProgramUniformHandleui64v)(GLuint program, GLint location,
                                                 GLsizei count,
                                                 const GLuint64 *values);
    void (APIENTRY *glUniformHandleui64)(GLint location, GLuint64 value);
    void (APIENTRY *glUniformHandleui64v)(GLint location, GLsizei count,
                                          const GLuint64 *value);

    // Construction
    NVBindlessTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVBindlessTexture

// GL_NV_blend_equation_advanced extension API specification.
struct NVBlendEquationAdvanced {
    // Public Members
    void (APIENTRY *glBlendBarrier)();
    void (APIENTRY *glBlendParameteri)(GLenum pname, GLint value);

    // Construction
    NVBlendEquationAdvanced();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVBlendEquationAdvanced

// GL_NV_clip_space_w_scaling extension API specification.
struct NVClipSpaceWScaling {
    // Public Members
    void (APIENTRY *glViewportPositionWScale)(GLuint index, GLfloat xcoeff,
                                              GLfloat ycoeff);

    // Construction
    NVClipSpaceWScaling();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVClipSpaceWScaling

// GL_NV_command_list extension API specification.
struct NVCommandList {
    // Public Members
    void (APIENTRY *glCallCommandList)(GLuint list);
    void (APIENTRY *glCommandListSegments)(GLuint list, GLuint segments);
    void (APIENTRY *glCompileCommandList)(GLuint list);
    void (APIENTRY *glCreateCommandLists)(GLsizei n, GLuint *lists);
    void (APIENTRY *glCreateStates)(GLsizei n, GLuint *states);
    void (APIENTRY *glDeleteCommandLists)(GLsizei n, const GLuint *lists);
    void (APIENTRY *glDeleteStates)(GLsizei n, const GLuint *states);
    void (APIENTRY *glDrawCommandsAddress)(GLenum primitiveMode,
                                           const GLuint64 *indirects,
                                           const GLsizei *sizes, GLuint count);
    void (APIENTRY *glDrawCommands)(GLenum primitiveMode, GLuint buffer,
                                    const GLintptr *indirects,
                                    const GLsizei *sizes, GLuint count);
    void (APIENTRY *glDrawCommandsStatesAddress)(const GLuint64 *indirects,
                                                 const GLsizei *sizes,
                                                 const GLuint *states,
                                                 const GLuint *fbos,
                                                 GLuint count);
    void (APIENTRY *glDrawCommandsStates)(GLuint buffer,
                                          const GLintptr *indirects,
                                          const GLsizei *sizes,
                                          const GLuint *states,
                                          const GLuint *fbos, GLuint count);
    GLuint (APIENTRY *glGetCommandHeader)(GLenum tokenID, GLuint size);
    GLushort (APIENTRY *glGetStageIndex)(GLenum shadertype);
    GLboolean (APIENTRY *glIsCommandList)(GLuint list);
    GLboolean (APIENTRY *glIsState)(GLuint state);
    void (APIENTRY *glListDrawCommandsStatesClient)(GLuint list, GLuint segment,
                                                    const void **indirects,
                                                    const GLsizei *sizes,
                                                    const GLuint *states,
                                                    const GLuint *fbos,
                                                    GLuint count);
    void (APIENTRY *glStateCapture)(GLuint state, GLenum mode);

    // Construction
    NVCommandList();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVCommandList

// GL_NV_conditional_render extension API specification.
struct NVConditionalRender {
    // Public Members
    void (APIENTRY *glBeginConditionalRender)(GLuint id, GLenum mode);
    void (APIENTRY *glEndConditionalRender)();

    // Construction
    NVConditionalRender();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVConditionalRender

// GL_NV_conservative_raster extension API specification.
struct NVConservativeRaster {
    // Public Members
    void (APIENTRY *glSubpixelPrecisionBias)(GLuint xbits, GLuint ybits);

    // Construction
    NVConservativeRaster();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVConservativeRaster

// GL_NV_conservative_raster_dilate extension API specification.
struct NVConservativeRasterDilate {
    // Public Members
    void (APIENTRY *glConservativeRasterParameterf)(GLenum pname, GLfloat value);

    // Construction
    NVConservativeRasterDilate();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVConservativeRasterDilate

// GL_NV_conservative_raster_pre_snap_triangles extension API specification.
struct NVConservativeRasterPreSnapTriangles {
    // Public Members
    void (APIENTRY *glConservativeRasterParameteri)(GLenum pname, GLint param);

    // Construction
    NVConservativeRasterPreSnapTriangles();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVConservativeRasterPreSnapTriangles

// GL_NV_copy_image extension API specification.
struct NVCopyImage {
    // Public Members
    void (APIENTRY *glCopyImageSubData)(GLuint srcName, GLenum srcTarget,
                                        GLint srcLevel, GLint srcX, GLint srcY,
                                        GLint srcZ, GLuint dstName,
                                        GLenum dstTarget, GLint dstLevel,
                                        GLint dstX, GLint dstY, GLint dstZ,
                                        GLsizei width, GLsizei height,
                                        GLsizei depth);

    // Construction
    NVCopyImage();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVCopyImage

// GL_NV_depth_buffer_float extension API specification.
struct NVDepthBufferFloat {
    // Public Members
    void (APIENTRY *glClearDepthd)(GLdouble depth);
    void (APIENTRY *glDepthBoundsd)(GLdouble zmin, GLdouble zmax);
    void (APIENTRY *glDepthRanged)(GLdouble zNear, GLdouble zFar);

    // Construction
    NVDepthBufferFloat();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVDepthBufferFloat

// GL_NV_draw_texture extension API specification.
struct NVDrawTexture {
    // Public Members
    void (APIENTRY *glDrawTexture)(GLuint texture, GLuint sampler, GLfloat x0,
                                   GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z,
                                   GLfloat s0, GLfloat t0, GLfloat s1,
                                   GLfloat t1);

    // Construction
    NVDrawTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVDrawTexture

// GL_NV_draw_vulkan_image extension API specification.
struct NVDrawVulkanImage {
    // Public Members
    void (APIENTRY *glDrawVkImage)(GLuint64 vkImage, GLuint sampler, GLfloat x0,
                                   GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z,
                                   GLfloat s0, GLfloat t0, GLfloat s1,
                                   GLfloat t1);
    GLVULKANPROCNV (APIENTRY *glGetVkProcAddr)(const GLchar *name);
    void (APIENTRY *glSignalVkFence)(GLuint64 vkFence);
    void (APIENTRY *glSignalVkSemaphore)(GLuint64 vkSemaphore);
    void (APIENTRY *glWaitVkSemaphore)(GLuint64 vkSemaphore);

    // Construction
    NVDrawVulkanImage();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVDrawVulkanImage

// GL_NV_evaluators extension API specification.
struct NVEvaluators {
    // Public Members
    void (APIENTRY *glEvalMaps)(GLenum target, GLenum mode);
    void (APIENTRY *glGetMapAttribParameterfv)(GLenum target, GLuint index,
                                               GLenum pname, GLfloat *params);
    void (APIENTRY *glGetMapAttribParameteriv)(GLenum target, GLuint index,
                                               GLenum pname, GLint *params);
    void (APIENTRY *glGetMapControlPoints)(GLenum target, GLuint index,
                                           GLenum type, GLsizei ustride,
                                           GLsizei vstride, GLboolean packed,
                                           void *points);
    void (APIENTRY *glGetMapParameterfv)(GLenum target, GLenum pname,
                                         GLfloat *params);
    void (APIENTRY *glGetMapParameteriv)(GLenum target, GLenum pname,
                                         GLint *params);
    void (APIENTRY *glMapControlPoints)(GLenum target, GLuint index, GLenum type,
                                        GLsizei ustride, GLsizei vstride,
                                        GLint uorder, GLint vorder,
                                        GLboolean packed, const void *points);
    void (APIENTRY *glMapParameterfv)(GLenum target, GLenum pname,
                                      const GLfloat *params);
    void (APIENTRY *glMapParameteriv)(GLenum target, GLenum pname,
                                      const GLint *params);

    // Construction
    NVEvaluators();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVEvaluators

// GL_NV_explicit_multisample extension API specification.
struct NVExplicitMultisample {
    // Public Members
    void (APIENTRY *glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat *val);
    void (APIENTRY *glSampleMaskIndexed)(GLuint index, GLbitfield mask);
    void (APIENTRY *glTexRenderbuffer)(GLenum target, GLuint renderbuffer);

    // Construction
    NVExplicitMultisample();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVExplicitMultisample

// GL_NV_fence extension API specification.
struct NVFence {
    // Public Members
    void (APIENTRY *glDeleteFences)(GLsizei n, const GLuint *fences);
    void (APIENTRY *glFinishFence)(GLuint fence);
    void (APIENTRY *glGenFences)(GLsizei n, GLuint *fences);
    void (APIENTRY *glGetFenceiv)(GLuint fence, GLenum pname, GLint *params);
    GLboolean (APIENTRY *glIsFence)(GLuint fence);
    void (APIENTRY *glSetFence)(GLuint fence, GLenum condition);
    GLboolean (APIENTRY *glTestFence)(GLuint fence);

    // Construction
    NVFence();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVFence

// GL_NV_fragment_coverage_to_color extension API specification.
struct NVFragmentCoverageToColor {
    // Public Members
    void (APIENTRY *glFragmentCoverageColor)(GLuint color);

    // Construction
    NVFragmentCoverageToColor();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVFragmentCoverageToColor

// GL_NV_fragment_program extension API specification.
struct NVFragmentProgram {
    // Public Members
    void (APIENTRY *glGetProgramNamedParameterdv)(GLuint id, GLsizei len,
                                                  const GLubyte *name,
                                                  GLdouble *params);
    void (APIENTRY *glGetProgramNamedParameterfv)(GLuint id, GLsizei len,
                                                  const GLubyte *name,
                                                  GLfloat *params);
    void (APIENTRY *glProgramNamedParameter4d)(GLuint id, GLsizei len,
                                               const GLubyte *name, GLdouble x,
                                               GLdouble y, GLdouble z,
                                               GLdouble w);
    void (APIENTRY *glProgramNamedParameter4dv)(GLuint id, GLsizei len,
                                                const GLubyte *name,
                                                const GLdouble *v);
    void (APIENTRY *glProgramNamedParameter4f)(GLuint id, GLsizei len,
                                               const GLubyte *name, GLfloat x,
                                               GLfloat y, GLfloat z, GLfloat w);
    void (APIENTRY *glProgramNamedParameter4fv)(GLuint id, GLsizei len,
                                                const GLubyte *name,
                                                const GLfloat *v);

    // Construction
    NVFragmentProgram();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVFragmentProgram

// GL_NV_framebuffer_mixed_samples extension API specification.
struct NVFramebufferMixedSamples {
    // Public Members
    void (APIENTRY *glCoverageModulation)(GLenum components);
    void (APIENTRY *glCoverageModulationTable)(GLsizei n, const GLfloat *v);
    void (APIENTRY *glGetCoverageModulationTable)(GLsizei bufSize, GLfloat *v);

    // Construction
    NVFramebufferMixedSamples();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVFramebufferMixedSamples

// GL_NV_framebuffer_multisample_coverage extension API specification.
struct NVFramebufferMultisampleCoverage {
    // Public Members
    void (APIENTRY *glRenderbufferStorageMultisampleCoverage)(GLenum target,
                                                              GLsizei coverageSamples,
                                                              GLsizei colorSamples,
                                                              GLenum internalformat,
                                                              GLsizei width,
                                                              GLsizei height);

    // Construction
    NVFramebufferMultisampleCoverage();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVFramebufferMultisampleCoverage

// GL_NV_geometry_program4 extension API specification.
struct NVGeometryPROGRAM4 {
    // Public Members
    void (APIENTRY *glFramebufferTexture)(GLenum target, GLenum attachment,
                                          GLuint texture, GLint level);
    void (APIENTRY *glFramebufferTextureFace)(GLenum target, GLenum attachment,
                                              GLuint texture, GLint level,
                                              GLenum face);
    void (APIENTRY *glProgramVertexLimit)(GLenum target, GLint limit);

    // Construction
    NVGeometryPROGRAM4();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVGeometryPROGRAM4

// GL_NV_gpu_multicast extension API specification.
struct NVGPUMulticast {
    // Public Members
    void (APIENTRY *glMulticastBarrier)();
    void (APIENTRY *glMulticastBlitFramebuffer)(GLuint srcGpu, GLuint dstGpu,
                                                GLint srcX0, GLint srcY0,
                                                GLint srcX1, GLint srcY1,
                                                GLint dstX0, GLint dstY0,
                                                GLint dstX1, GLint dstY1,
                                                GLbitfield mask, GLenum filter);
    void (APIENTRY *glMulticastBufferSubData)(GLbitfield gpuMask, GLuint buffer,
                                              GLintptr offset, GLsizeiptr size,
                                              const void *data);
    void (APIENTRY *glMulticastCopyBufferSubData)(GLuint readGpu,
                                                  GLbitfield writeGpuMask,
                                                  GLuint readBuffer,
                                                  GLuint writeBuffer,
                                                  GLintptr readOffset,
                                                  GLintptr writeOffset,
                                                  GLsizeiptr size);
    void (APIENTRY *glMulticastCopyImageSubData)(GLuint srcGpu,
                                                 GLbitfield dstGpuMask,
                                                 GLuint srcName,
                                                 GLenum srcTarget,
                                                 GLint srcLevel, GLint srcX,
                                                 GLint srcY, GLint srcZ,
                                                 GLuint dstName,
                                                 GLenum dstTarget,
                                                 GLint dstLevel, GLint dstX,
                                                 GLint dstY, GLint dstZ,
                                                 GLsizei srcWidth,
                                                 GLsizei srcHeight,
                                                 GLsizei srcDepth);
    void (APIENTRY *glMulticastFramebufferSampleLocationsfv)(GLuint gpu,
                                                             GLuint framebuffer,
                                                             GLuint start,
                                                             GLsizei count,
                                                             const GLfloat *v);
    void (APIENTRY *glMulticastGetQueryObjecti64v)(GLuint gpu, GLuint id,
                                                   GLenum pname, GLint64 *params);
    void (APIENTRY *glMulticastGetQueryObjectiv)(GLuint gpu, GLuint id,
                                                 GLenum pname, GLint *params);
    void (APIENTRY *glMulticastGetQueryObjectui64v)(GLuint gpu, GLuint id,
                                                    GLenum pname,
                                                    GLuint64 *params);
    void (APIENTRY *glMulticastGetQueryObjectuiv)(GLuint gpu, GLuint id,
                                                  GLenum pname, GLuint *params);
    void (APIENTRY *glMulticastWaitSync)(GLuint signalGpu,
                                         GLbitfield waitGpuMask);
    void (APIENTRY *glRenderGpuMask)(GLbitfield mask);

    // Construction
    NVGPUMulticast();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVGPUMulticast

// GL_NV_gpu_program4 extension API specification.
struct NVGPUPROGRAM4 {
    // Public Members
    void (APIENTRY *glGetProgramEnvParameterIiv)(GLenum target, GLuint index,
                                                 GLint *params);
    void (APIENTRY *glGetProgramEnvParameterIuiv)(GLenum target, GLuint index,
                                                  GLuint *params);
    void (APIENTRY *glGetProgramLocalParameterIiv)(GLenum target, GLuint index,
                                                   GLint *params);
    void (APIENTRY *glGetProgramLocalParameterIuiv)(GLenum target, GLuint index,
                                                    GLuint *params);
    void (APIENTRY *glProgramEnvParameterI4i)(GLenum target, GLuint index,
                                              GLint x, GLint y, GLint z, GLint w);
    void (APIENTRY *glProgramEnvParameterI4iv)(GLenum target, GLuint index,
                                               const GLint *params);
    void (APIENTRY *glProgramEnvParameterI4ui)(GLenum target, GLuint index,
                                               GLuint x, GLuint y, GLuint z,
                                               GLuint w);
    void (APIENTRY *glProgramEnvParameterI4uiv)(GLenum target, GLuint index,
                                                const GLuint *params);
    void (APIENTRY *glProgramEnvParametersI4iv)(GLenum target, GLuint index,
                                                GLsizei count,
                                                const GLint *params);
    void (APIENTRY *glProgramEnvParametersI4uiv)(GLenum target, GLuint index,
                                                 GLsizei count,
                                                 const GLuint *params);
    void (APIENTRY *glProgramLocalParameterI4i)(GLenum target, GLuint index,
                                                GLint x, GLint y, GLint z,
                                                GLint w);
    void (APIENTRY *glProgramLocalParameterI4iv)(GLenum target, GLuint index,
                                                 const GLint *params);
    void (APIENTRY *glProgramLocalParameterI4ui)(GLenum target, GLuint index,
                                                 GLuint x, GLuint y, GLuint z,
                                                 GLuint w);
    void (APIENTRY *glProgramLocalParameterI4uiv)(GLenum target, GLuint index,
                                                  const GLuint *params);
    void (APIENTRY *glProgramLocalParametersI4iv)(GLenum target, GLuint index,
                                                  GLsizei count,
                                                  const GLint *params);
    void (APIENTRY *glProgramLocalParametersI4uiv)(GLenum target, GLuint index,
                                                   GLsizei count,
                                                   const GLuint *params);

    // Construction
    NVGPUPROGRAM4();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVGPUPROGRAM4

// GL_NV_gpu_program5 extension API specification.
struct NVGPUPROGRAM5 {
    // Public Members
    void (APIENTRY *glGetProgramSubroutineParameteruiv)(GLenum target,
                                                        GLuint index,
                                                        GLuint *param);
    void (APIENTRY *glProgramSubroutineParametersuiv)(GLenum target,
                                                      GLsizei count,
                                                      const GLuint *params);

    // Construction
    NVGPUPROGRAM5();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVGPUPROGRAM5

// GL_NV_half_float extension API specification.
struct NVHalfFloat {
    // Public Members
    void (APIENTRY *glColor3h)(GLhalfNV red, GLhalfNV green, GLhalfNV blue);
    void (APIENTRY *glColor3hv)(const GLhalfNV *v);
    void (APIENTRY *glColor4h)(GLhalfNV red, GLhalfNV green, GLhalfNV blue,
                               GLhalfNV alpha);
    void (APIENTRY *glColor4hv)(const GLhalfNV *v);
    void (APIENTRY *glFogCoordh)(GLhalfNV fog);
    void (APIENTRY *glFogCoordhv)(const GLhalfNV *fog);
    void (APIENTRY *glMultiTexCoord1h)(GLenum target, GLhalfNV s);
    void (APIENTRY *glMultiTexCoord1hv)(GLenum target, const GLhalfNV *v);
    void (APIENTRY *glMultiTexCoord2h)(GLenum target, GLhalfNV s, GLhalfNV t);
    void (APIENTRY *glMultiTexCoord2hv)(GLenum target, const GLhalfNV *v);
    void (APIENTRY *glMultiTexCoord3h)(GLenum target, GLhalfNV s, GLhalfNV t,
                                       GLhalfNV r);
    void (APIENTRY *glMultiTexCoord3hv)(GLenum target, const GLhalfNV *v);
    void (APIENTRY *glMultiTexCoord4h)(GLenum target, GLhalfNV s, GLhalfNV t,
                                       GLhalfNV r, GLhalfNV q);
    void (APIENTRY *glMultiTexCoord4hv)(GLenum target, const GLhalfNV *v);
    void (APIENTRY *glNormal3h)(GLhalfNV nx, GLhalfNV ny, GLhalfNV nz);
    void (APIENTRY *glNormal3hv)(const GLhalfNV *v);
    void (APIENTRY *glSecondaryColor3h)(GLhalfNV red, GLhalfNV green,
                                        GLhalfNV blue);
    void (APIENTRY *glSecondaryColor3hv)(const GLhalfNV *v);
    void (APIENTRY *glTexCoord1h)(GLhalfNV s);
    void (APIENTRY *glTexCoord1hv)(const GLhalfNV *v);
    void (APIENTRY *glTexCoord2h)(GLhalfNV s, GLhalfNV t);
    void (APIENTRY *glTexCoord2hv)(const GLhalfNV *v);
    void (APIENTRY *glTexCoord3h)(GLhalfNV s, GLhalfNV t, GLhalfNV r);
    void (APIENTRY *glTexCoord3hv)(const GLhalfNV *v);
    void (APIENTRY *glTexCoord4h)(GLhalfNV s, GLhalfNV t, GLhalfNV r, GLhalfNV q);
    void (APIENTRY *glTexCoord4hv)(const GLhalfNV *v);
    void (APIENTRY *glVertex2h)(GLhalfNV x, GLhalfNV y);
    void (APIENTRY *glVertex2hv)(const GLhalfNV *v);
    void (APIENTRY *glVertex3h)(GLhalfNV x, GLhalfNV y, GLhalfNV z);
    void (APIENTRY *glVertex3hv)(const GLhalfNV *v);
    void (APIENTRY *glVertex4h)(GLhalfNV x, GLhalfNV y, GLhalfNV z, GLhalfNV w);
    void (APIENTRY *glVertex4hv)(const GLhalfNV *v);
    void (APIENTRY *glVertexAttrib1h)(GLuint index, GLhalfNV x);
    void (APIENTRY *glVertexAttrib1hv)(GLuint index, const GLhalfNV *v);
    void (APIENTRY *glVertexAttrib2h)(GLuint index, GLhalfNV x, GLhalfNV y);
    void (APIENTRY *glVertexAttrib2hv)(GLuint index, const GLhalfNV *v);
    void (APIENTRY *glVertexAttrib3h)(GLuint index, GLhalfNV x, GLhalfNV y,
                                      GLhalfNV z);
    void (APIENTRY *glVertexAttrib3hv)(GLuint index, const GLhalfNV *v);
    void (APIENTRY *glVertexAttrib4h)(GLuint index, GLhalfNV x, GLhalfNV y,
                                      GLhalfNV z, GLhalfNV w);
    void (APIENTRY *glVertexAttrib4hv)(GLuint index, const GLhalfNV *v);
    void (APIENTRY *glVertexAttribs1hv)(GLuint index, GLsizei n,
                                        const GLhalfNV *v);
    void (APIENTRY *glVertexAttribs2hv)(GLuint index, GLsizei n,
                                        const GLhalfNV *v);
    void (APIENTRY *glVertexAttribs3hv)(GLuint index, GLsizei n,
                                        const GLhalfNV *v);
    void (APIENTRY *glVertexAttribs4hv)(GLuint index, GLsizei n,
                                        const GLhalfNV *v);
    void (APIENTRY *glVertexWeighth)(GLhalfNV weight);
    void (APIENTRY *glVertexWeighthv)(const GLhalfNV *weight);

    // Construction
    NVHalfFloat();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVHalfFloat

// GL_NV_internalformat_sample_query extension API specification.
struct NVInternalformatSampleQuery {
    // Public Members
    void (APIENTRY *glGetInternalformatSampleiv)(GLenum target,
                                                 GLenum internalformat,
                                                 GLsizei samples, GLenum pname,
                                                 GLsizei count, GLint *params);

    // Construction
    NVInternalformatSampleQuery();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVInternalformatSampleQuery

// GL_NV_memory_attachment extension API specification.
struct NVMemoryAttachment {
    // Public Members
    void (APIENTRY *glBufferAttachMemory)(GLenum target, GLuint memory,
                                          GLuint64 offset);
    void (APIENTRY *glGetMemoryObjectDetachedResourcesuiv)(GLuint memory,
                                                           GLenum pname,
                                                           GLint first,
                                                           GLsizei count,
                                                           GLuint *params);
    void (APIENTRY *glNamedBufferAttachMemory)(GLuint buffer, GLuint memory,
                                               GLuint64 offset);
    void (APIENTRY *glResetMemoryObjectParameter)(GLuint memory, GLenum pname);
    void (APIENTRY *glTexAttachMemory)(GLenum target, GLuint memory,
                                       GLuint64 offset);
    void (APIENTRY *glTextureAttachMemory)(GLuint texture, GLuint memory,
                                           GLuint64 offset);

    // Construction
    NVMemoryAttachment();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVMemoryAttachment

// GL_NV_memory_object_sparse extension API specification.
struct NVMemoryObjectSparse {
    // Public Members
    void (APIENTRY *glBufferPageCommitmentMem)(GLenum target, GLintptr offset,
                                               GLsizeiptr size, GLuint memory,
                                               GLuint64 memOffset,
                                               GLboolean commit);
    void (APIENTRY *glNamedBufferPageCommitmentMem)(GLuint buffer,
                                                    GLintptr offset,
                                                    GLsizeiptr size,
                                                    GLuint memory,
                                                    GLuint64 memOffset,
                                                    GLboolean commit);
    void (APIENTRY *glTexPageCommitmentMem)(GLenum target, GLint layer,
                                            GLint level, GLint xoffset,
                                            GLint yoffset, GLint zoffset,
                                            GLsizei width, GLsizei height,
                                            GLsizei depth, GLuint memory,
                                            GLuint64 offset, GLboolean commit);
    void (APIENTRY *glTexturePageCommitmentMem)(GLuint texture, GLint layer,
                                                GLint level, GLint xoffset,
                                                GLint yoffset, GLint zoffset,
                                                GLsizei width, GLsizei height,
                                                GLsizei depth, GLuint memory,
                                                GLuint64 offset,
                                                GLboolean commit);

    // Construction
    NVMemoryObjectSparse();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVMemoryObjectSparse

// GL_NV_mesh_shader extension API specification.
struct NVMeshShader {
    // Public Members
    void (APIENTRY *glDrawMeshTasksIndirect)(GLintptr indirect);
    void (APIENTRY *glDrawMeshTasks)(GLuint first, GLuint count);
    void (APIENTRY *glMultiDrawMeshTasksIndirectCount)(GLintptr indirect,
                                                       GLintptr drawcount,
                                                       GLsizei maxdrawcount,
                                                       GLsizei stride);
    void (APIENTRY *glMultiDrawMeshTasksIndirect)(GLintptr indirect,
                                                  GLsizei drawcount,
                                                  GLsizei stride);

    // Construction
    NVMeshShader();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVMeshShader

// GL_NV_occlusion_query extension API specification.
struct NVOcclusionQuery {
    // Public Members
    void (APIENTRY *glBeginOcclusionQuery)(GLuint id);
    void (APIENTRY *glDeleteOcclusionQueries)(GLsizei n, const GLuint *ids);
    void (APIENTRY *glEndOcclusionQuery)();
    void (APIENTRY *glGenOcclusionQueries)(GLsizei n, GLuint *ids);
    void (APIENTRY *glGetOcclusionQueryiv)(GLuint id, GLenum pname,
                                           GLint *params);
    void (APIENTRY *glGetOcclusionQueryuiv)(GLuint id, GLenum pname,
                                            GLuint *params);
    GLboolean (APIENTRY *glIsOcclusionQuery)(GLuint id);

    // Construction
    NVOcclusionQuery();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVOcclusionQuery

// GL_NV_parameter_buffer_object extension API specification.
struct NVParameterBufferObject {
    // Public Members
    void (APIENTRY *glProgramBufferParametersIiv)(GLenum target,
                                                  GLuint bindingIndex,
                                                  GLuint wordIndex,
                                                  GLsizei count,
                                                  const GLint *params);
    void (APIENTRY *glProgramBufferParametersIuiv)(GLenum target,
                                                   GLuint bindingIndex,
                                                   GLuint wordIndex,
                                                   GLsizei count,
                                                   const GLuint *params);
    void (APIENTRY *glProgramBufferParametersfv)(GLenum target,
                                                 GLuint bindingIndex,
                                                 GLuint wordIndex, GLsizei count,
                                                 const GLfloat *params);

    // Construction
    NVParameterBufferObject();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVParameterBufferObject

// GL_NV_path_rendering extension API specification.
struct NVPathRendering {
    // Public Members
    void (APIENTRY *glCopyPath)(GLuint resultPath, GLuint srcPath);
    void (APIENTRY *glCoverFillPathInstanced)(GLsizei numPaths,
                                              GLenum pathNameType,
                                              const void *paths, GLuint pathBase,
                                              GLenum coverMode,
                                              GLenum transformType,
                                              const GLfloat *transformValues);
    void (APIENTRY *glCoverFillPath)(GLuint path, GLenum coverMode);
    void (APIENTRY *glCoverStrokePathInstanced)(GLsizei numPaths,
                                                GLenum pathNameType,
                                                const void *paths,
                                                GLuint pathBase,
                                                GLenum coverMode,
                                                GLenum transformType,
                                                const GLfloat *transformValues);
    void (APIENTRY *glCoverStrokePath)(GLuint path, GLenum coverMode);
    void (APIENTRY *glDeletePaths)(GLuint path, GLsizei range);
    GLuint (APIENTRY *glGenPaths)(GLsizei range);
    void (APIENTRY *glGetPathCommands)(GLuint path, GLubyte *commands);
    void (APIENTRY *glGetPathCoords)(GLuint path, GLfloat *coords);
    void (APIENTRY *glGetPathDashArray)(GLuint path, GLfloat *dashArray);
    GLfloat (APIENTRY *glGetPathLength)(GLuint path, GLsizei startSegment,
                                        GLsizei numSegments);
    void (APIENTRY *glGetPathMetricRange)(GLbitfield metricQueryMask,
                                          GLuint firstPathName, GLsizei numPaths,
                                          GLsizei stride, GLfloat *metrics);
    void (APIENTRY *glGetPathMetrics)(GLbitfield metricQueryMask,
                                      GLsizei numPaths, GLenum pathNameType,
                                      const void *paths, GLuint pathBase,
                                      GLsizei stride, GLfloat *metrics);
    void (APIENTRY *glGetPathParameterfv)(GLuint path, GLenum pname,
                                          GLfloat *value);
    void (APIENTRY *glGetPathParameteriv)(GLuint path, GLenum pname,
                                          GLint *value);
    void (APIENTRY *glGetPathSpacing)(GLenum pathListMode, GLsizei numPaths,
                                      GLenum pathNameType, const void *paths,
                                      GLuint pathBase, GLfloat advanceScale,
                                      GLfloat kerningScale, GLenum transformType,
                                      GLfloat *returnedSpacing);
    void (APIENTRY *glGetProgramResourcefv)(GLuint program,
                                            GLenum programInterface,
                                            GLuint index, GLsizei propCount,
                                            const GLenum *props, GLsizei count,
                                            GLsizei *length, GLfloat *params);
    void (APIENTRY *glInterpolatePaths)(GLuint resultPath, GLuint pathA,
                                        GLuint pathB, GLfloat weight);
    GLboolean (APIENTRY *glIsPath)(GLuint path);
    GLboolean (APIENTRY *glIsPointInFillPath)(GLuint path, GLuint mask,
                                              GLfloat x, GLfloat y);
    GLboolean (APIENTRY *glIsPointInStrokePath)(GLuint path, GLfloat x,
                                                GLfloat y);
    void (APIENTRY *glMatrixLoad3x2f)(GLenum matrixMode, const GLfloat *m);
    void (APIENTRY *glMatrixLoad3x3f)(GLenum matrixMode, const GLfloat *m);
    void (APIENTRY *glMatrixLoadTranspose3x3f)(GLenum matrixMode,
                                               const GLfloat *m);
    void (APIENTRY *glMatrixMult3x2f)(GLenum matrixMode, const GLfloat *m);
    void (APIENTRY *glMatrixMult3x3f)(GLenum matrixMode, const GLfloat *m);
    void (APIENTRY *glMatrixMultTranspose3x3f)(GLenum matrixMode,
                                               const GLfloat *m);
    void (APIENTRY *glPathCommands)(GLuint path, GLsizei numCommands,
                                    const GLubyte *commands, GLsizei numCoords,
                                    GLenum coordType, const void *coords);
    void (APIENTRY *glPathCoords)(GLuint path, GLsizei numCoords,
                                  GLenum coordType, const void *coords);
    void (APIENTRY *glPathCoverDepthFunc)(GLenum func);
    void (APIENTRY *glPathDashArray)(GLuint path, GLsizei dashCount,
                                     const GLfloat *dashArray);
    GLenum (APIENTRY *glPathGlyphIndexArray)(GLuint firstPathName,
                                             GLenum fontTarget,
                                             const void *fontName,
                                             GLbitfield fontStyle,
                                             GLuint firstGlyphIndex,
                                             GLsizei numGlyphs,
                                             GLuint pathParameterTemplate,
                                             GLfloat emScale);
    GLenum (APIENTRY *glPathGlyphIndexRange)(GLenum fontTarget,
                                             const void *fontName,
                                             GLbitfield fontStyle,
                                             GLuint pathParameterTemplate,
                                             GLfloat emScale,
                                             GLuint *baseAndCount);
    void (APIENTRY *glPathGlyphRange)(GLuint firstPathName, GLenum fontTarget,
                                      const void *fontName, GLbitfield fontStyle,
                                      GLuint firstGlyph, GLsizei numGlyphs,
                                      GLenum handleMissingGlyphs,
                                      GLuint pathParameterTemplate,
                                      GLfloat emScale);
    void (APIENTRY *glPathGlyphs)(GLuint firstPathName, GLenum fontTarget,
                                  const void *fontName, GLbitfield fontStyle,
                                  GLsizei numGlyphs, GLenum type,
                                  const void *charcodes,
                                  GLenum handleMissingGlyphs,
                                  GLuint pathParameterTemplate, GLfloat emScale);
    GLenum (APIENTRY *glPathMemoryGlyphIndexArray)(GLuint firstPathName,
                                                   GLenum fontTarget,
                                                   GLsizeiptr fontSize,
                                                   const void *fontData,
                                                   GLsizei faceIndex,
                                                   GLuint firstGlyphIndex,
                                                   GLsizei numGlyphs,
                                                   GLuint pathParameterTemplate,
                                                   GLfloat emScale);
    void (APIENTRY *glPathParameterf)(GLuint path, GLenum pname, GLfloat value);
    void (APIENTRY *glPathParameterfv)(GLuint path, GLenum pname,
                                       const GLfloat *value);
    void (APIENTRY *glPathParameteri)(GLuint path, GLenum pname, GLint value);
    void (APIENTRY *glPathParameteriv)(GLuint path, GLenum pname,
                                       const GLint *value);
    void (APIENTRY *glPathStencilDepthOffset)(GLfloat factor, GLfloat units);
    void (APIENTRY *glPathStencilFunc)(GLenum func, GLint ref, GLuint mask);
    void (APIENTRY *glPathString)(GLuint path, GLenum format, GLsizei length,
                                  const void *pathString);
    void (APIENTRY *glPathSubCommands)(GLuint path, GLsizei commandStart,
                                       GLsizei commandsToDelete,
                                       GLsizei numCommands,
                                       const GLubyte *commands,
                                       GLsizei numCoords, GLenum coordType,
                                       const void *coords);
    void (APIENTRY *glPathSubCoords)(GLuint path, GLsizei coordStart,
                                     GLsizei numCoords, GLenum coordType,
                                     const void *coords);
    GLboolean (APIENTRY *glPointAlongPath)(GLuint path, GLsizei startSegment,
                                           GLsizei numSegments, GLfloat distance,
                                           GLfloat *x, GLfloat *y,
                                           GLfloat *tangentX, GLfloat *tangentY);
    void (APIENTRY *glProgramPathFragmentInputGen)(GLuint program,
                                                   GLint location,
                                                   GLenum genMode,
                                                   GLint components,
                                                   const GLfloat *coeffs);
    void (APIENTRY *glStencilFillPathInstanced)(GLsizei numPaths,
                                                GLenum pathNameType,
                                                const void *paths,
                                                GLuint pathBase, GLenum fillMode,
                                                GLuint mask,
                                                GLenum transformType,
                                                const GLfloat *transformValues);
    void (APIENTRY *glStencilFillPath)(GLuint path, GLenum fillMode, GLuint mask);
    void (APIENTRY *glStencilStrokePathInstanced)(GLsizei numPaths,
                                                  GLenum pathNameType,
                                                  const void *paths,
                                                  GLuint pathBase,
                                                  GLint reference, GLuint mask,
                                                  GLenum transformType,
                                                  const GLfloat *transformValues);
    void (APIENTRY *glStencilStrokePath)(GLuint path, GLint reference,
                                         GLuint mask);
    void (APIENTRY *glStencilThenCoverFillPathInstanced)(GLsizei numPaths,
                                                         GLenum pathNameType,
                                                         const void *paths,
                                                         GLuint pathBase,
                                                         GLenum fillMode,
                                                         GLuint mask,
                                                         GLenum coverMode,
                                                         GLenum transformType,
                                                         const GLfloat *transformValues);
    void (APIENTRY *glStencilThenCoverFillPath)(GLuint path, GLenum fillMode,
                                                GLuint mask, GLenum coverMode);
    void (APIENTRY *glStencilThenCoverStrokePathInstanced)(GLsizei numPaths,
                                                           GLenum pathNameType,
                                                           const void *paths,
                                                           GLuint pathBase,
                                                           GLint reference,
                                                           GLuint mask,
                                                           GLenum coverMode,
                                                           GLenum transformType,
                                                           const GLfloat *transformValues);
    void (APIENTRY *glStencilThenCoverStrokePath)(GLuint path, GLint reference,
                                                  GLuint mask, GLenum coverMode);
    void (APIENTRY *glTransformPath)(GLuint resultPath, GLuint srcPath,
                                     GLenum transformType,
                                     const GLfloat *transformValues);
    void (APIENTRY *glWeightPaths)(GLuint resultPath, GLsizei numPaths,
                                   const GLuint *paths, const GLfloat *weights);

    // Construction
    NVPathRendering();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVPathRendering

// GL_NV_pixel_data_range extension API specification.
struct NVPixelDataRange {
    // Public Members
    void (APIENTRY *glFlushPixelDataRange)(GLenum target);
    void (APIENTRY *glPixelDataRange)(GLenum target, GLsizei length,
                                      const void *pointer);

    // Construction
    NVPixelDataRange();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVPixelDataRange

// GL_NV_point_sprite extension API specification.
struct NVPointSprite {
    // Public Members
    void (APIENTRY *glPointParameteri)(GLenum pname, GLint param);
    void (APIENTRY *glPointParameteriv)(GLenum pname, const GLint *params);

    // Construction
    NVPointSprite();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVPointSprite

// GL_NV_present_video extension API specification.
struct NVPresentVideo {
    // Public Members
    void (APIENTRY *glGetVideoi64v)(GLuint video_slot, GLenum pname,
                                    GLint64EXT *params);
    void (APIENTRY *glGetVideoiv)(GLuint video_slot, GLenum pname, GLint *params);
    void (APIENTRY *glGetVideoui64v)(GLuint video_slot, GLenum pname,
                                     GLuint64EXT *params);
    void (APIENTRY *glGetVideouiv)(GLuint video_slot, GLenum pname,
                                   GLuint *params);
    void (APIENTRY *glPresentFrameDualFill)(GLuint video_slot,
                                            GLuint64EXT minPresentTime,
                                            GLuint beginPresentTimeId,
                                            GLuint presentDurationId,
                                            GLenum type, GLenum target0,
                                            GLuint fill0, GLenum target1,
                                            GLuint fill1, GLenum target2,
                                            GLuint fill2, GLenum target3,
                                            GLuint fill3);
    void (APIENTRY *glPresentFrameKeyed)(GLuint video_slot,
                                         GLuint64EXT minPresentTime,
                                         GLuint beginPresentTimeId,
                                         GLuint presentDurationId, GLenum type,
                                         GLenum target0, GLuint fill0,
                                         GLuint key0, GLenum target1,
                                         GLuint fill1, GLuint key1);

    // Construction
    NVPresentVideo();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVPresentVideo

// GL_NV_primitive_restart extension API specification.
struct NVPrimitiveRestart {
    // Public Members
    void (APIENTRY *glPrimitiveRestartIndex)(GLuint index);
    void (APIENTRY *glPrimitiveRestart)();

    // Construction
    NVPrimitiveRestart();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVPrimitiveRestart

// GL_NV_query_resource extension API specification.
struct NVQueryResource {
    // Public Members
    GLint (APIENTRY *glQueryResource)(GLenum queryType, GLint tagId,
                                      GLuint count, GLint *buffer);

    // Construction
    NVQueryResource();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVQueryResource

// GL_NV_query_resource_tag extension API specification.
struct NVQueryResourceTag {
    // Public Members
    void (APIENTRY *glDeleteQueryResourceTag)(GLsizei n, const GLint *tagIds);
    void (APIENTRY *glGenQueryResourceTag)(GLsizei n, GLint *tagIds);
    void (APIENTRY *glQueryResourceTag)(GLint tagId, const GLchar *tagString);

    // Construction
    NVQueryResourceTag();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVQueryResourceTag

// GL_NV_register_combiners extension API specification.
struct NVRegisterCombiners {
    // Public Members
    void (APIENTRY *glCombinerInput)(GLenum stage, GLenum portion,
                                     GLenum variable, GLenum input,
                                     GLenum mapping, GLenum componentUsage);
    void (APIENTRY *glCombinerOutput)(GLenum stage, GLenum portion,
                                      GLenum abOutput, GLenum cdOutput,
                                      GLenum sumOutput, GLenum scale,
                                      GLenum bias, GLboolean abDotProduct,
                                      GLboolean cdDotProduct, GLboolean muxSum);
    void (APIENTRY *glCombinerParameterf)(GLenum pname, GLfloat param);
    void (APIENTRY *glCombinerParameterfv)(GLenum pname, const GLfloat *params);
    void (APIENTRY *glCombinerParameteri)(GLenum pname, GLint param);
    void (APIENTRY *glCombinerParameteriv)(GLenum pname, const GLint *params);
    void (APIENTRY *glFinalCombinerInput)(GLenum variable, GLenum input,
                                          GLenum mapping, GLenum componentUsage);
    void (APIENTRY *glGetCombinerInputParameterfv)(GLenum stage, GLenum portion,
                                                   GLenum variable, GLenum pname,
                                                   GLfloat *params);
    void (APIENTRY *glGetCombinerInputParameteriv)(GLenum stage, GLenum portion,
                                                   GLenum variable, GLenum pname,
                                                   GLint *params);
    void (APIENTRY *glGetCombinerOutputParameterfv)(GLenum stage, GLenum portion,
                                                    GLenum pname,
                                                    GLfloat *params);
    void (APIENTRY *glGetCombinerOutputParameteriv)(GLenum stage, GLenum portion,
                                                    GLenum pname, GLint *params);
    void (APIENTRY *glGetFinalCombinerInputParameterfv)(GLenum variable,
                                                        GLenum pname,
                                                        GLfloat *params);
    void (APIENTRY *glGetFinalCombinerInputParameteriv)(GLenum variable,
                                                        GLenum pname,
                                                        GLint *params);

    // Construction
    NVRegisterCombiners();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVRegisterCombiners

// GL_NV_register_combiners2 extension API specification.
struct NVRegisterCOMBINERS2 {
    // Public Members
    void (APIENTRY *glCombinerStageParameterfv)(GLenum stage, GLenum pname,
                                                const GLfloat *params);
    void (APIENTRY *glGetCombinerStageParameterfv)(GLenum stage, GLenum pname,
                                                   GLfloat *params);

    // Construction
    NVRegisterCOMBINERS2();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVRegisterCOMBINERS2

// GL_NV_sample_locations extension API specification.
struct NVSampleLocations {
    // Public Members
    void (APIENTRY *glFramebufferSampleLocationsfv)(GLenum target, GLuint start,
                                                    GLsizei count,
                                                    const GLfloat *v);
    void (APIENTRY *glNamedFramebufferSampleLocationsfv)(GLuint framebuffer,
                                                         GLuint start,
                                                         GLsizei count,
                                                         const GLfloat *v);
    void (APIENTRY *glResolveDepthValues)();

    // Construction
    NVSampleLocations();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVSampleLocations

// GL_NV_scissor_exclusive extension API specification.
struct NVScissorExclusive {
    // Public Members
    void (APIENTRY *glScissorExclusiveArrayv)(GLuint first, GLsizei count,
                                              const GLint *v);
    void (APIENTRY *glScissorExclusive)(GLint x, GLint y, GLsizei width,
                                        GLsizei height);

    // Construction
    NVScissorExclusive();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVScissorExclusive

// GL_NV_shader_buffer_load extension API specification.
struct NVShaderBufferLoad {
    // Public Members
    void (APIENTRY *glGetBufferParameterui64v)(GLenum target, GLenum pname,
                                               GLuint64EXT *params);
    void (APIENTRY *glGetIntegerui64v)(GLenum value, GLuint64EXT *result);
    void (APIENTRY *glGetNamedBufferParameterui64v)(GLuint buffer, GLenum pname,
                                                    GLuint64EXT *params);
    GLboolean (APIENTRY *glIsBufferResident)(GLenum target);
    GLboolean (APIENTRY *glIsNamedBufferResident)(GLuint buffer);
    void (APIENTRY *glMakeBufferNonResident)(GLenum target);
    void (APIENTRY *glMakeBufferResident)(GLenum target, GLenum access);
    void (APIENTRY *glMakeNamedBufferNonResident)(GLuint buffer);
    void (APIENTRY *glMakeNamedBufferResident)(GLuint buffer, GLenum access);
    void (APIENTRY *glProgramUniformui64)(GLuint program, GLint location,
                                          GLuint64EXT value);
    void (APIENTRY *glProgramUniformui64v)(GLuint program, GLint location,
                                           GLsizei count,
                                           const GLuint64EXT *value);
    void (APIENTRY *glUniformui64)(GLint location, GLuint64EXT value);
    void (APIENTRY *glUniformui64v)(GLint location, GLsizei count,
                                    const GLuint64EXT *value);

    // Construction
    NVShaderBufferLoad();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVShaderBufferLoad

// GL_NV_shading_rate_image extension API specification.
struct NVShadingRateImage {
    // Public Members
    void (APIENTRY *glBindShadingRateImage)(GLuint texture);
    void (APIENTRY *glGetShadingRateImagePalette)(GLuint viewport, GLuint entry,
                                                  GLenum *rate);
    void (APIENTRY *glGetShadingRateSampleLocationiv)(GLenum rate,
                                                      GLuint samples,
                                                      GLuint index,
                                                      GLint *location);
    void (APIENTRY *glShadingRateImageBarrier)(GLboolean synchronize);
    void (APIENTRY *glShadingRateImagePalette)(GLuint viewport, GLuint first,
                                               GLsizei count,
                                               const GLenum *rates);
    void (APIENTRY *glShadingRateSampleOrderCustom)(GLenum rate, GLuint samples,
                                                    const GLint *locations);
    void (APIENTRY *glShadingRateSampleOrder)(GLenum order);

    // Construction
    NVShadingRateImage();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVShadingRateImage

// GL_NV_texture_barrier extension API specification.
struct NVTextureBarrier {
    // Public Members
    void (APIENTRY *glTextureBarrier)();

    // Construction
    NVTextureBarrier();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVTextureBarrier

// GL_NV_texture_multisample extension API specification.
struct NVTextureMultisample {
    // Public Members
    void (APIENTRY *glTexImage2DMultisampleCoverage)(GLenum target,
                                                     GLsizei coverageSamples,
                                                     GLsizei colorSamples,
                                                     GLint internalFormat,
                                                     GLsizei width,
                                                     GLsizei height,
                                                     GLboolean fixedSampleLocations);
    void (APIENTRY *glTexImage3DMultisampleCoverage)(GLenum target,
                                                     GLsizei coverageSamples,
                                                     GLsizei colorSamples,
                                                     GLint internalFormat,
                                                     GLsizei width,
                                                     GLsizei height,
                                                     GLsizei depth,
                                                     GLboolean fixedSampleLocations);
    void (APIENTRY *glTextureImage2DMultisampleCoverage)(GLuint texture,
                                                         GLenum target,
                                                         GLsizei coverageSamples,
                                                         GLsizei colorSamples,
                                                         GLint internalFormat,
                                                         GLsizei width,
                                                         GLsizei height,
                                                         GLboolean fixedSampleLocations);
    void (APIENTRY *glTextureImage2DMultisample)(GLuint texture, GLenum target,
                                                 GLsizei samples,
                                                 GLint internalFormat,
                                                 GLsizei width, GLsizei height,
                                                 GLboolean fixedSampleLocations);
    void (APIENTRY *glTextureImage3DMultisampleCoverage)(GLuint texture,
                                                         GLenum target,
                                                         GLsizei coverageSamples,
                                                         GLsizei colorSamples,
                                                         GLint internalFormat,
                                                         GLsizei width,
                                                         GLsizei height,
                                                         GLsizei depth,
                                                         GLboolean fixedSampleLocations);
    void (APIENTRY *glTextureImage3DMultisample)(GLuint texture, GLenum target,
                                                 GLsizei samples,
                                                 GLint internalFormat,
                                                 GLsizei width, GLsizei height,
                                                 GLsizei depth,
                                                 GLboolean fixedSampleLocations);

    // Construction
    NVTextureMultisample();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVTextureMultisample

// GL_NV_timeline_semaphore extension API specification.
struct NVTimelineSemaphore {
    // Public Members
    void (APIENTRY *glCreateSemaphores)(GLsizei n, GLuint *semaphores);
    void (APIENTRY *glGetSemaphoreParameteriv)(GLuint semaphore, GLenum pname,
                                               GLint *params);
    void (APIENTRY *glSemaphoreParameteriv)(GLuint semaphore, GLenum pname,
                                            const GLint *params);

    // Construction
    NVTimelineSemaphore();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVTimelineSemaphore

// GL_NV_transform_feedback extension API specification.
struct NVTransformFeedback {
    // Public Members
    void (APIENTRY *glActiveVarying)(GLuint program, const GLchar *name);
    void (APIENTRY *glBeginTransformFeedback)(GLenum primitiveMode);
    void (APIENTRY *glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
    void (APIENTRY *glBindBufferOffset)(GLenum target, GLuint index,
                                        GLuint buffer, GLintptr offset);
    void (APIENTRY *glBindBufferRange)(GLenum target, GLuint index,
                                       GLuint buffer, GLintptr offset,
                                       GLsizeiptr size);
    void (APIENTRY *glEndTransformFeedback)();
    void (APIENTRY *glGetActiveVarying)(GLuint program, GLuint index,
                                        GLsizei bufSize, GLsizei *length,
                                        GLsizei *size, GLenum *type,
                                        GLchar *name);
    void (APIENTRY *glGetTransformFeedbackVarying)(GLuint program, GLuint index,
                                                   GLint *location);
    GLint (APIENTRY *glGetVaryingLocation)(GLuint program, const GLchar *name);
    void (APIENTRY *glTransformFeedbackAttribs)(GLsizei count,
                                                const GLint *attribs,
                                                GLenum bufferMode);
    void (APIENTRY *glTransformFeedbackStreamAttribs)(GLsizei count,
                                                      const GLint *attribs,
                                                      GLsizei nbuffers,
                                                      const GLint *bufstreams,
                                                      GLenum bufferMode);
    void (APIENTRY *glTransformFeedbackVaryings)(GLuint program, GLsizei count,
                                                 const GLint *locations,
                                                 GLenum bufferMode);

    // Construction
    NVTransformFeedback();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVTransformFeedback

// GL_NV_transform_feedback2 extension API specification.
struct NVTransformFeedback2 {
    // Public Members
    void (APIENTRY *glBindTransformFeedback)(GLenum target, GLuint id);
    void (APIENTRY *glDeleteTransformFeedbacks)(GLsizei n, const GLuint *ids);
    void (APIENTRY *glDrawTransformFeedback)(GLenum mode, GLuint id);
    void (APIENTRY *glGenTransformFeedbacks)(GLsizei n, GLuint *ids);
    GLboolean (APIENTRY *glIsTransformFeedback)(GLuint id);
    void (APIENTRY *glPauseTransformFeedback)();
    void (APIENTRY *glResumeTransformFeedback)();

    // Construction
    NVTransformFeedback2();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVTransformFeedback2

// GL_NV_vdpau_interop extension API specification.
struct NVVdpauInterop {
    // Public Members
    void (APIENTRY *glVDPAUFini)();
    void (APIENTRY *glVDPAUGetSurfaceiv)(GLvdpauSurfaceNV surface, GLenum pname,
                                         GLsizei count, GLsizei *length,
                                         GLint *values);
    void (APIENTRY *glVDPAUInit)(const void *vdpDevice,
                                 const void *getProcAddress);
    GLboolean (APIENTRY *glVDPAUIsSurface)(GLvdpauSurfaceNV surface);
    void (APIENTRY *glVDPAUMapSurfaces)(GLsizei numSurfaces,
                                        const GLvdpauSurfaceNV *surfaces);
    GLvdpauSurfaceNV (APIENTRY *glVDPAURegisterOutputSurface)(const void *vdpSurface,
                                                              GLenum target,
                                                              GLsizei numTextureNames,
                                                              const GLuint *textureNames);
    GLvdpauSurfaceNV (APIENTRY *glVDPAURegisterVideoSurface)(const void *vdpSurface,
                                                             GLenum target,
                                                             GLsizei numTextureNames,
                                                             const GLuint *textureNames);
    void (APIENTRY *glVDPAUSurfaceAccess)(GLvdpauSurfaceNV surface,
                                          GLenum access);
    void (APIENTRY *glVDPAUUnmapSurfaces)(GLsizei numSurface,
                                          const GLvdpauSurfaceNV *surfaces);
    void (APIENTRY *glVDPAUUnregisterSurface)(GLvdpauSurfaceNV surface);

    // Construction
    NVVdpauInterop();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVVdpauInterop

// GL_NV_vdpau_interop2 extension API specification.
struct NVVdpauINTEROP2 {
    // Public Members
    GLvdpauSurfaceNV (APIENTRY *glVDPAURegisterVideoSurfaceWithPictureStructure)(const void *vdpSurface,
                                                                                 GLenum target,
                                                                                 GLsizei numTextureNames,
                                                                                 const GLuint *textureNames,
                                                                                 GLboolean isFrameStructure);

    // Construction
    NVVdpauINTEROP2();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVVdpauINTEROP2

// GL_NV_vertex_array_range extension API specification.
struct NVVertexArrayRange {
    // Public Members
    void (APIENTRY *glFlushVertexArrayRange)();
    void (APIENTRY *glVertexArrayRange)(GLsizei length, const void *pointer);

    // Construction
    NVVertexArrayRange();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVVertexArrayRange

// GL_NV_vertex_attrib_integer_64bit extension API specification.
struct NVVertexAttribInteger64BIT {
    // Public Members
    void (APIENTRY *glGetVertexAttribLi64v)(GLuint index, GLenum pname,
                                            GLint64EXT *params);
    void (APIENTRY *glGetVertexAttribLui64v)(GLuint index, GLenum pname,
                                             GLuint64EXT *params);
    void (APIENTRY *glVertexAttribL1i64)(GLuint index, GLint64EXT x);
    void (APIENTRY *glVertexAttribL1i64v)(GLuint index, const GLint64EXT *v);
    void (APIENTRY *glVertexAttribL1ui64)(GLuint index, GLuint64EXT x);
    void (APIENTRY *glVertexAttribL1ui64v)(GLuint index, const GLuint64EXT *v);
    void (APIENTRY *glVertexAttribL2i64)(GLuint index, GLint64EXT x,
                                         GLint64EXT y);
    void (APIENTRY *glVertexAttribL2i64v)(GLuint index, const GLint64EXT *v);
    void (APIENTRY *glVertexAttribL2ui64)(GLuint index, GLuint64EXT x,
                                          GLuint64EXT y);
    void (APIENTRY *glVertexAttribL2ui64v)(GLuint index, const GLuint64EXT *v);
    void (APIENTRY *glVertexAttribL3i64)(GLuint index, GLint64EXT x,
                                         GLint64EXT y, GLint64EXT z);
    void (APIENTRY *glVertexAttribL3i64v)(GLuint index, const GLint64EXT *v);
    void (APIENTRY *glVertexAttribL3ui64)(GLuint index, GLuint64EXT x,
                                          GLuint64EXT y, GLuint64EXT z);
    void (APIENTRY *glVertexAttribL3ui64v)(GLuint index, const GLuint64EXT *v);
    void (APIENTRY *glVertexAttribL4i64)(GLuint index, GLint64EXT x,
                                         GLint64EXT y, GLint64EXT z,
                                         GLint64EXT w);
    void (APIENTRY *glVertexAttribL4i64v)(GLuint index, const GLint64EXT *v);
    void (APIENTRY *glVertexAttribL4ui64)(GLuint index, GLuint64EXT x,
                                          GLuint64EXT y, GLuint64EXT z,
                                          GLuint64EXT w);
    void (APIENTRY *glVertexAttribL4ui64v)(GLuint index, const GLuint64EXT *v);
    void (APIENTRY *glVertexAttribLFormat)(GLuint index, GLint size, GLenum type,
                                           GLsizei stride);

    // Construction
    NVVertexAttribInteger64BIT();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVVertexAttribInteger64BIT

// GL_NV_vertex_buffer_unified_memory extension API specification.
struct NVVertexBufferUnifiedMemory {
    // Public Members
    void (APIENTRY *glBufferAddressRange)(GLenum pname, GLuint index,
                                          GLuint64EXT address, GLsizeiptr length);
    void (APIENTRY *glColorFormat)(GLint size, GLenum type, GLsizei stride);
    void (APIENTRY *glEdgeFlagFormat)(GLsizei stride);
    void (APIENTRY *glFogCoordFormat)(GLenum type, GLsizei stride);
    void (APIENTRY *glGetIntegerui64i_v)(GLenum value, GLuint index,
                                         GLuint64EXT *result);
    void (APIENTRY *glIndexFormat)(GLenum type, GLsizei stride);
    void (APIENTRY *glNormalFormat)(GLenum type, GLsizei stride);
    void (APIENTRY *glSecondaryColorFormat)(GLint size, GLenum type,
                                            GLsizei stride);
    void (APIENTRY *glTexCoordFormat)(GLint size, GLenum type, GLsizei stride);
    void (APIENTRY *glVertexAttribFormat)(GLuint index, GLint size, GLenum type,
                                          GLboolean normalized, GLsizei stride);
    void (APIENTRY *glVertexAttribIFormat)(GLuint index, GLint size, GLenum type,
                                           GLsizei stride);
    void (APIENTRY *glVertexFormat)(GLint size, GLenum type, GLsizei stride);

    // Construction
    NVVertexBufferUnifiedMemory();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVVertexBufferUnifiedMemory

// GL_NV_vertex_program extension API specification.
struct NVVertexProgram {
    // Public Members
    GLboolean (APIENTRY *glAreProgramsResident)(GLsizei n,
                                                const GLuint *programs,
                                                GLboolean *residences);
    void (APIENTRY *glBindProgram)(GLenum target, GLuint id);
    void (APIENTRY *glDeletePrograms)(GLsizei n, const GLuint *programs);
    void (APIENTRY *glExecuteProgram)(GLenum target, GLuint id,
                                      const GLfloat *params);
    void (APIENTRY *glGenPrograms)(GLsizei n, GLuint *programs);
    void (APIENTRY *glGetProgramParameterdv)(GLenum target, GLuint index,
                                             GLenum pname, GLdouble *params);
    void (APIENTRY *glGetProgramParameterfv)(GLenum target, GLuint index,
                                             GLenum pname, GLfloat *params);
    void (APIENTRY *glGetProgramString)(GLuint id, GLenum pname,
                                        GLubyte *program);
    void (APIENTRY *glGetProgramiv)(GLuint id, GLenum pname, GLint *params);
    void (APIENTRY *glGetTrackMatrixiv)(GLenum target, GLuint address,
                                        GLenum pname, GLint *params);
    void (APIENTRY *glGetVertexAttribPointerv)(GLuint index, GLenum pname,
                                               void **pointer);
    void (APIENTRY *glGetVertexAttribdv)(GLuint index, GLenum pname,
                                         GLdouble *params);
    void (APIENTRY *glGetVertexAttribfv)(GLuint index, GLenum pname,
                                         GLfloat *params);
    void (APIENTRY *glGetVertexAttribiv)(GLuint index, GLenum pname,
                                         GLint *params);
    GLboolean (APIENTRY *glIsProgram)(GLuint id);
    void (APIENTRY *glLoadProgram)(GLenum target, GLuint id, GLsizei len,
                                   const GLubyte *program);
    void (APIENTRY *glProgramParameter4d)(GLenum target, GLuint index,
                                          GLdouble x, GLdouble y, GLdouble z,
                                          GLdouble w);
    void (APIENTRY *glProgramParameter4dv)(GLenum target, GLuint index,
                                           const GLdouble *v);
    void (APIENTRY *glProgramParameter4f)(GLenum target, GLuint index, GLfloat x,
                                          GLfloat y, GLfloat z, GLfloat w);
    void (APIENTRY *glProgramParameter4fv)(GLenum target, GLuint index,
                                           const GLfloat *v);
    void (APIENTRY *glProgramParameters4dv)(GLenum target, GLuint index,
                                            GLsizei count, const GLdouble *v);
    void (APIENTRY *glProgramParameters4fv)(GLenum target, GLuint index,
                                            GLsizei count, const GLfloat *v);
    void (APIENTRY *glRequestResidentPrograms)(GLsizei n, const GLuint *programs);
    void (APIENTRY *glTrackMatrix)(GLenum target, GLuint address, GLenum matrix,
                                   GLenum transform);
    void (APIENTRY *glVertexAttrib1d)(GLuint index, GLdouble x);
    void (APIENTRY *glVertexAttrib1dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib1f)(GLuint index, GLfloat x);
    void (APIENTRY *glVertexAttrib1fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib1s)(GLuint index, GLshort x);
    void (APIENTRY *glVertexAttrib1sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
    void (APIENTRY *glVertexAttrib2dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
    void (APIENTRY *glVertexAttrib2fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib2s)(GLuint index, GLshort x, GLshort y);
    void (APIENTRY *glVertexAttrib2sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y,
                                      GLdouble z);
    void (APIENTRY *glVertexAttrib3dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y,
                                      GLfloat z);
    void (APIENTRY *glVertexAttrib3fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib3s)(GLuint index, GLshort x, GLshort y,
                                      GLshort z);
    void (APIENTRY *glVertexAttrib3sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y,
                                      GLdouble z, GLdouble w);
    void (APIENTRY *glVertexAttrib4dv)(GLuint index, const GLdouble *v);
    void (APIENTRY *glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y,
                                      GLfloat z, GLfloat w);
    void (APIENTRY *glVertexAttrib4fv)(GLuint index, const GLfloat *v);
    void (APIENTRY *glVertexAttrib4s)(GLuint index, GLshort x, GLshort y,
                                      GLshort z, GLshort w);
    void (APIENTRY *glVertexAttrib4sv)(GLuint index, const GLshort *v);
    void (APIENTRY *glVertexAttrib4ub)(GLuint index, GLubyte x, GLubyte y,
                                       GLubyte z, GLubyte w);
    void (APIENTRY *glVertexAttrib4ubv)(GLuint index, const GLubyte *v);
    void (APIENTRY *glVertexAttribPointer)(GLuint index, GLint fsize,
                                           GLenum type, GLsizei stride,
                                           const void *pointer);
    void (APIENTRY *glVertexAttribs1dv)(GLuint index, GLsizei count,
                                        const GLdouble *v);
    void (APIENTRY *glVertexAttribs1fv)(GLuint index, GLsizei count,
                                        const GLfloat *v);
    void (APIENTRY *glVertexAttribs1sv)(GLuint index, GLsizei count,
                                        const GLshort *v);
    void (APIENTRY *glVertexAttribs2dv)(GLuint index, GLsizei count,
                                        const GLdouble *v);
    void (APIENTRY *glVertexAttribs2fv)(GLuint index, GLsizei count,
                                        const GLfloat *v);
    void (APIENTRY *glVertexAttribs2sv)(GLuint index, GLsizei count,
                                        const GLshort *v);
    void (APIENTRY *glVertexAttribs3dv)(GLuint index, GLsizei count,
                                        const GLdouble *v);
    void (APIENTRY *glVertexAttribs3fv)(GLuint index, GLsizei count,
                                        const GLfloat *v);
    void (APIENTRY *glVertexAttribs3sv)(GLuint index, GLsizei count,
                                        const GLshort *v);
    void (APIENTRY *glVertexAttribs4dv)(GLuint index, GLsizei count,
                                        const GLdouble *v);
    void (APIENTRY *glVertexAttribs4fv)(GLuint index, GLsizei count,
                                        const GLfloat *v);
    void (APIENTRY *glVertexAttribs4sv)(GLuint index, GLsizei count,
                                        const GLshort *v);
    void (APIENTRY *glVertexAttribs4ubv)(GLuint index, GLsizei count,
                                         const GLubyte *v);

    // Construction
    NVVertexProgram();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVVertexProgram

// GL_NV_video_capture extension API specification.
struct NVVideoCapture {
    // Public Members
    void (APIENTRY *glBeginVideoCapture)(GLuint video_capture_slot);
    void (APIENTRY *glBindVideoCaptureStreamBuffer)(GLuint video_capture_slot,
                                                    GLuint stream,
                                                    GLenum frame_region,
                                                    GLintptrARB offset);
    void (APIENTRY *glBindVideoCaptureStreamTexture)(GLuint video_capture_slot,
                                                     GLuint stream,
                                                     GLenum frame_region,
                                                     GLenum target,
                                                     GLuint texture);
    void (APIENTRY *glEndVideoCapture)(GLuint video_capture_slot);
    void (APIENTRY *glGetVideoCaptureStreamdv)(GLuint video_capture_slot,
                                               GLuint stream, GLenum pname,
                                               GLdouble *params);
    void (APIENTRY *glGetVideoCaptureStreamfv)(GLuint video_capture_slot,
                                               GLuint stream, GLenum pname,
                                               GLfloat *params);
    void (APIENTRY *glGetVideoCaptureStreamiv)(GLuint video_capture_slot,
                                               GLuint stream, GLenum pname,
                                               GLint *params);
    void (APIENTRY *glGetVideoCaptureiv)(GLuint video_capture_slot, GLenum pname,
                                         GLint *params);
    GLenum (APIENTRY *glVideoCapture)(GLuint video_capture_slot,
                                      GLuint *sequence_num,
                                      GLuint64EXT *capture_time);
    void (APIENTRY *glVideoCaptureStreamParameterdv)(GLuint video_capture_slot,
                                                     GLuint stream, GLenum pname,
                                                     const GLdouble *params);
    void (APIENTRY *glVideoCaptureStreamParameterfv)(GLuint video_capture_slot,
                                                     GLuint stream, GLenum pname,
                                                     const GLfloat *params);
    void (APIENTRY *glVideoCaptureStreamParameteriv)(GLuint video_capture_slot,
                                                     GLuint stream, GLenum pname,
                                                     const GLint *params);

    // Construction
    NVVideoCapture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVVideoCapture

// GL_NV_viewport_swizzle extension API specification.
struct NVViewportSwizzle {
    // Public Members
    void (APIENTRY *glViewportSwizzle)(GLuint index, GLenum swizzlex,
                                       GLenum swizzley, GLenum swizzlez,
                                       GLenum swizzlew);

    // Construction
    NVViewportSwizzle();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct NVViewportSwizzle

// GL_OES_byte_coordinates extension API specification.
struct OESByteCoordinates {
    // Public Members
    void (APIENTRY *glMultiTexCoord1b)(GLenum texture, GLbyte s);
    void (APIENTRY *glMultiTexCoord1bv)(GLenum texture, const GLbyte *coords);
    void (APIENTRY *glMultiTexCoord2b)(GLenum texture, GLbyte s, GLbyte t);
    void (APIENTRY *glMultiTexCoord2bv)(GLenum texture, const GLbyte *coords);
    void (APIENTRY *glMultiTexCoord3b)(GLenum texture, GLbyte s, GLbyte t,
                                       GLbyte r);
    void (APIENTRY *glMultiTexCoord3bv)(GLenum texture, const GLbyte *coords);
    void (APIENTRY *glMultiTexCoord4b)(GLenum texture, GLbyte s, GLbyte t,
                                       GLbyte r, GLbyte q);
    void (APIENTRY *glMultiTexCoord4bv)(GLenum texture, const GLbyte *coords);
    void (APIENTRY *glTexCoord1b)(GLbyte s);
    void (APIENTRY *glTexCoord1bv)(const GLbyte *coords);
    void (APIENTRY *glTexCoord2b)(GLbyte s, GLbyte t);
    void (APIENTRY *glTexCoord2bv)(const GLbyte *coords);
    void (APIENTRY *glTexCoord3b)(GLbyte s, GLbyte t, GLbyte r);
    void (APIENTRY *glTexCoord3bv)(const GLbyte *coords);
    void (APIENTRY *glTexCoord4b)(GLbyte s, GLbyte t, GLbyte r, GLbyte q);
    void (APIENTRY *glTexCoord4bv)(const GLbyte *coords);
    void (APIENTRY *glVertex2b)(GLbyte x, GLbyte y);
    void (APIENTRY *glVertex2bv)(const GLbyte *coords);
    void (APIENTRY *glVertex3b)(GLbyte x, GLbyte y, GLbyte z);
    void (APIENTRY *glVertex3bv)(const GLbyte *coords);
    void (APIENTRY *glVertex4b)(GLbyte x, GLbyte y, GLbyte z, GLbyte w);
    void (APIENTRY *glVertex4bv)(const GLbyte *coords);

    // Construction
    OESByteCoordinates();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct OESByteCoordinates

// GL_OES_fixed_point extension API specification.
struct OESFixedPoint {
    // Public Members
    void (APIENTRY *glAccumx)(GLenum op, GLfixed value);
    void (APIENTRY *glAlphaFuncx)(GLenum func, GLfixed ref);
    void (APIENTRY *glBitmapx)(GLsizei width, GLsizei height, GLfixed xorig,
                               GLfixed yorig, GLfixed xmove, GLfixed ymove,
                               const GLubyte *bitmap);
    void (APIENTRY *glBlendColorx)(GLfixed red, GLfixed green, GLfixed blue,
                                   GLfixed alpha);
    void (APIENTRY *glClearAccumx)(GLfixed red, GLfixed green, GLfixed blue,
                                   GLfixed alpha);
    void (APIENTRY *glClearColorx)(GLfixed red, GLfixed green, GLfixed blue,
                                   GLfixed alpha);
    void (APIENTRY *glClearDepthx)(GLfixed depth);
    void (APIENTRY *glClipPlanex)(GLenum plane, const GLfixed *equation);
    void (APIENTRY *glColor3x)(GLfixed red, GLfixed green, GLfixed blue);
    void (APIENTRY *glColor3xv)(const GLfixed *components);
    void (APIENTRY *glColor4x)(GLfixed red, GLfixed green, GLfixed blue,
                               GLfixed alpha);
    void (APIENTRY *glColor4xv)(const GLfixed *components);
    void (APIENTRY *glConvolutionParameterx)(GLenum target, GLenum pname,
                                             GLfixed param);
    void (APIENTRY *glConvolutionParameterxv)(GLenum target, GLenum pname,
                                              const GLfixed *params);
    void (APIENTRY *glDepthRangex)(GLfixed n, GLfixed f);
    void (APIENTRY *glEvalCoord1x)(GLfixed u);
    void (APIENTRY *glEvalCoord1xv)(const GLfixed *coords);
    void (APIENTRY *glEvalCoord2x)(GLfixed u, GLfixed v);
    void (APIENTRY *glEvalCoord2xv)(const GLfixed *coords);
    void (APIENTRY *glFeedbackBufferx)(GLsizei n, GLenum type,
                                       const GLfixed *buffer);
    void (APIENTRY *glFogx)(GLenum pname, GLfixed param);
    void (APIENTRY *glFogxv)(GLenum pname, const GLfixed *param);
    void (APIENTRY *glFrustumx)(GLfixed l, GLfixed r, GLfixed b, GLfixed t,
                                GLfixed n, GLfixed f);
    void (APIENTRY *glGetClipPlanex)(GLenum plane, GLfixed *equation);
    void (APIENTRY *glGetConvolutionParameterxv)(GLenum target, GLenum pname,
                                                 GLfixed *params);
    void (APIENTRY *glGetFixedv)(GLenum pname, GLfixed *params);
    void (APIENTRY *glGetHistogramParameterxv)(GLenum target, GLenum pname,
                                               GLfixed *params);
    void (APIENTRY *glGetLightx)(GLenum light, GLenum pname, GLfixed *params);
    void (APIENTRY *glGetMapxv)(GLenum target, GLenum query, GLfixed *v);
    void (APIENTRY *glGetMaterialx)(GLenum face, GLenum pname, GLfixed param);
    void (APIENTRY *glGetPixelMapxv)(GLenum map, GLint size, GLfixed *values);
    void (APIENTRY *glGetTexEnvxv)(GLenum target, GLenum pname, GLfixed *params);
    void (APIENTRY *glGetTexGenxv)(GLenum coord, GLenum pname, GLfixed *params);
    void (APIENTRY *glGetTexLevelParameterxv)(GLenum target, GLint level,
                                              GLenum pname, GLfixed *params);
    void (APIENTRY *glGetTexParameterxv)(GLenum target, GLenum pname,
                                         GLfixed *params);
    void (APIENTRY *glIndexx)(GLfixed component);
    void (APIENTRY *glIndexxv)(const GLfixed *component);
    void (APIENTRY *glLightModelx)(GLenum pname, GLfixed param);
    void (APIENTRY *glLightModelxv)(GLenum pname, const GLfixed *param);
    void (APIENTRY *glLightx)(GLenum light, GLenum pname, GLfixed param);
    void (APIENTRY *glLightxv)(GLenum light, GLenum pname, const GLfixed *params);
    void (APIENTRY *glLineWidthx)(GLfixed width);
    void (APIENTRY *glLoadMatrixx)(const GLfixed *m);
    void (APIENTRY *glLoadTransposeMatrixx)(const GLfixed *m);
    void (APIENTRY *glMap1x)(GLenum target, GLfixed u1, GLfixed u2, GLint stride,
                             GLint order, GLfixed points);
    void (APIENTRY *glMap2x)(GLenum target, GLfixed u1, GLfixed u2,
                             GLint ustride, GLint uorder, GLfixed v1, GLfixed v2,
                             GLint vstride, GLint vorder, GLfixed points);
    void (APIENTRY *glMapGrid1x)(GLint n, GLfixed u1, GLfixed u2);
    void (APIENTRY *glMapGrid2x)(GLint n, GLfixed u1, GLfixed u2, GLfixed v1,
                                 GLfixed v2);
    void (APIENTRY *glMaterialx)(GLenum face, GLenum pname, GLfixed param);
    void (APIENTRY *glMaterialxv)(GLenum face, GLenum pname,
                                  const GLfixed *param);
    void (APIENTRY *glMultMatrixx)(const GLfixed *m);
    void (APIENTRY *glMultTransposeMatrixx)(const GLfixed *m);
    void (APIENTRY *glMultiTexCoord1x)(GLenum texture, GLfixed s);
    void (APIENTRY *glMultiTexCoord1xv)(GLenum texture, const GLfixed *coords);
    void (APIENTRY *glMultiTexCoord2x)(GLenum texture, GLfixed s, GLfixed t);
    void (APIENTRY *glMultiTexCoord2xv)(GLenum texture, const GLfixed *coords);
    void (APIENTRY *glMultiTexCoord3x)(GLenum texture, GLfixed s, GLfixed t,
                                       GLfixed r);
    void (APIENTRY *glMultiTexCoord3xv)(GLenum texture, const GLfixed *coords);
    void (APIENTRY *glMultiTexCoord4x)(GLenum texture, GLfixed s, GLfixed t,
                                       GLfixed r, GLfixed q);
    void (APIENTRY *glMultiTexCoord4xv)(GLenum texture, const GLfixed *coords);
    void (APIENTRY *glNormal3x)(GLfixed nx, GLfixed ny, GLfixed nz);
    void (APIENTRY *glNormal3xv)(const GLfixed *coords);
    void (APIENTRY *glOrthox)(GLfixed l, GLfixed r, GLfixed b, GLfixed t,
                              GLfixed n, GLfixed f);
    void (APIENTRY *glPassThroughx)(GLfixed token);
    void (APIENTRY *glPixelMapx)(GLenum map, GLint size, const GLfixed *values);
    void (APIENTRY *glPixelStorex)(GLenum pname, GLfixed param);
    void (APIENTRY *glPixelTransferx)(GLenum pname, GLfixed param);
    void (APIENTRY *glPixelZoomx)(GLfixed xfactor, GLfixed yfactor);
    void (APIENTRY *glPointParameterxv)(GLenum pname, const GLfixed *params);
    void (APIENTRY *glPointSizex)(GLfixed size);
    void (APIENTRY *glPolygonOffsetx)(GLfixed factor, GLfixed units);
    void (APIENTRY *glPrioritizeTexturesx)(GLsizei n, const GLuint *textures,
                                           const GLfixed *priorities);
    void (APIENTRY *glRasterPos2x)(GLfixed x, GLfixed y);
    void (APIENTRY *glRasterPos2xv)(const GLfixed *coords);
    void (APIENTRY *glRasterPos3x)(GLfixed x, GLfixed y, GLfixed z);
    void (APIENTRY *glRasterPos3xv)(const GLfixed *coords);
    void (APIENTRY *glRasterPos4x)(GLfixed x, GLfixed y, GLfixed z, GLfixed w);
    void (APIENTRY *glRasterPos4xv)(const GLfixed *coords);
    void (APIENTRY *glRectx)(GLfixed x1, GLfixed y1, GLfixed x2, GLfixed y2);
    void (APIENTRY *glRectxv)(const GLfixed *v1, const GLfixed *v2);
    void (APIENTRY *glRotatex)(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
    void (APIENTRY *glScalex)(GLfixed x, GLfixed y, GLfixed z);
    void (APIENTRY *glTexCoord1x)(GLfixed s);
    void (APIENTRY *glTexCoord1xv)(const GLfixed *coords);
    void (APIENTRY *glTexCoord2x)(GLfixed s, GLfixed t);
    void (APIENTRY *glTexCoord2xv)(const GLfixed *coords);
    void (APIENTRY *glTexCoord3x)(GLfixed s, GLfixed t, GLfixed r);
    void (APIENTRY *glTexCoord3xv)(const GLfixed *coords);
    void (APIENTRY *glTexCoord4x)(GLfixed s, GLfixed t, GLfixed r, GLfixed q);
    void (APIENTRY *glTexCoord4xv)(const GLfixed *coords);
    void (APIENTRY *glTexEnvx)(GLenum target, GLenum pname, GLfixed param);
    void (APIENTRY *glTexEnvxv)(GLenum target, GLenum pname,
                                const GLfixed *params);
    void (APIENTRY *glTexGenx)(GLenum coord, GLenum pname, GLfixed param);
    void (APIENTRY *glTexGenxv)(GLenum coord, GLenum pname,
                                const GLfixed *params);
    void (APIENTRY *glTexParameterx)(GLenum target, GLenum pname, GLfixed param);
    void (APIENTRY *glTexParameterxv)(GLenum target, GLenum pname,
                                      const GLfixed *params);
    void (APIENTRY *glTranslatex)(GLfixed x, GLfixed y, GLfixed z);
    void (APIENTRY *glVertex2x)(GLfixed x);
    void (APIENTRY *glVertex2xv)(const GLfixed *coords);
    void (APIENTRY *glVertex3x)(GLfixed x, GLfixed y);
    void (APIENTRY *glVertex3xv)(const GLfixed *coords);
    void (APIENTRY *glVertex4x)(GLfixed x, GLfixed y, GLfixed z);
    void (APIENTRY *glVertex4xv)(const GLfixed *coords);

    // Construction
    OESFixedPoint();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct OESFixedPoint

// GL_OES_query_matrix extension API specification.
struct OESQueryMatrix {
    // Public Members
    GLbitfield (APIENTRY *glQueryMatrixx)(GLfixed *mantissa, GLint *exponent);

    // Construction
    OESQueryMatrix();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct OESQueryMatrix

// GL_OES_single_precision extension API specification.
struct OESSinglePrecision {
    // Public Members
    void (APIENTRY *glClearDepthf)(GLclampf depth);
    void (APIENTRY *glClipPlanef)(GLenum plane, const GLfloat *equation);
    void (APIENTRY *glDepthRangef)(GLclampf n, GLclampf f);
    void (APIENTRY *glFrustumf)(GLfloat l, GLfloat r, GLfloat b, GLfloat t,
                                GLfloat n, GLfloat f);
    void (APIENTRY *glGetClipPlanef)(GLenum plane, GLfloat *equation);
    void (APIENTRY *glOrthof)(GLfloat l, GLfloat r, GLfloat b, GLfloat t,
                              GLfloat n, GLfloat f);

    // Construction
    OESSinglePrecision();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct OESSinglePrecision

// GL_OVR_multiview extension API specification.
struct OvrMultiview {
    // Public Members
    void (APIENTRY *glFramebufferTextureMultiviewOVR)(GLenum target,
                                                      GLenum attachment,
                                                      GLuint texture,
                                                      GLint level,
                                                      GLint baseViewIndex,
                                                      GLsizei numViews);

    // Construction
    OvrMultiview();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct OvrMultiview

// GL_PGI_misc_hints extension API specification.
struct PGIMiscHints {
    // Public Members
    void (APIENTRY *glHint)(GLenum target, GLint mode);

    // Construction
    PGIMiscHints();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct PGIMiscHints

// GL_SGIS_detail_texture extension API specification.
struct SGISDetailTexture {
    // Public Members
    void (APIENTRY *glDetailTexFunc)(GLenum target, GLsizei n,
                                     const GLfloat *points);
    void (APIENTRY *glGetDetailTexFunc)(GLenum target, GLfloat *points);

    // Construction
    SGISDetailTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISDetailTexture

// GL_SGIS_fog_function extension API specification.
struct SGISFogFunction {
    // Public Members
    void (APIENTRY *glFogFunc)(GLsizei n, const GLfloat *points);
    void (APIENTRY *glGetFogFunc)(GLfloat *points);

    // Construction
    SGISFogFunction();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISFogFunction

// GL_SGIS_multisample extension API specification.
struct SGISMultisample {
    // Public Members
    void (APIENTRY *glSampleMask)(GLclampf value, GLboolean invert);
    void (APIENTRY *glSamplePattern)(GLenum pattern);

    // Construction
    SGISMultisample();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISMultisample

// GL_SGIS_pixel_texture extension API specification.
struct SGISPixelTexture {
    // Public Members
    void (APIENTRY *glGetPixelTexGenParameterfv)(GLenum pname, GLfloat *params);
    void (APIENTRY *glGetPixelTexGenParameteriv)(GLenum pname, GLint *params);
    void (APIENTRY *glPixelTexGenParameterf)(GLenum pname, GLfloat param);
    void (APIENTRY *glPixelTexGenParameterfv)(GLenum pname,
                                              const GLfloat *params);
    void (APIENTRY *glPixelTexGenParameteri)(GLenum pname, GLint param);
    void (APIENTRY *glPixelTexGenParameteriv)(GLenum pname, const GLint *params);

    // Construction
    SGISPixelTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISPixelTexture

// GL_SGIS_point_parameters extension API specification.
struct SGISPointParameters {
    // Public Members
    void (APIENTRY *glPointParameterf)(GLenum pname, GLfloat param);
    void (APIENTRY *glPointParameterfv)(GLenum pname, const GLfloat *params);

    // Construction
    SGISPointParameters();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISPointParameters

// GL_SGIS_sharpen_texture extension API specification.
struct SGISSharpenTexture {
    // Public Members
    void (APIENTRY *glGetSharpenTexFunc)(GLenum target, GLfloat *points);
    void (APIENTRY *glSharpenTexFunc)(GLenum target, GLsizei n,
                                      const GLfloat *points);

    // Construction
    SGISSharpenTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISSharpenTexture

// GL_SGIS_texture4D extension API specification.
struct SGISTEXTURE4D {
    // Public Members
    void (APIENTRY *glTexImage4D)(GLenum target, GLint level,
                                  GLenum internalformat, GLsizei width,
                                  GLsizei height, GLsizei depth, GLsizei size4d,
                                  GLint border, GLenum format, GLenum type,
                                  const void *pixels);
    void (APIENTRY *glTexSubImage4D)(GLenum target, GLint level, GLint xoffset,
                                     GLint yoffset, GLint zoffset, GLint woffset,
                                     GLsizei width, GLsizei height,
                                     GLsizei depth, GLsizei size4d,
                                     GLenum format, GLenum type,
                                     const void *pixels);

    // Construction
    SGISTEXTURE4D();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISTEXTURE4D

// GL_SGIS_texture_color_mask extension API specification.
struct SGISTextureColorMask {
    // Public Members
    void (APIENTRY *glTextureColorMask)(GLboolean red, GLboolean green,
                                        GLboolean blue, GLboolean alpha);

    // Construction
    SGISTextureColorMask();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISTextureColorMask

// GL_SGIS_texture_filter4 extension API specification.
struct SGISTextureFILTER4 {
    // Public Members
    void (APIENTRY *glGetTexFilterFunc)(GLenum target, GLenum filter,
                                        GLfloat *weights);
    void (APIENTRY *glTexFilterFunc)(GLenum target, GLenum filter, GLsizei n,
                                     const GLfloat *weights);

    // Construction
    SGISTextureFILTER4();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGISTextureFILTER4

// GL_SGIX_async extension API specification.
struct SGIXAsync {
    // Public Members
    void (APIENTRY *glAsyncMarker)(GLuint marker);
    void (APIENTRY *glDeleteAsyncMarkers)(GLuint marker, GLsizei range);
    GLint (APIENTRY *glFinishAsync)(GLuint *markerp);
    GLuint (APIENTRY *glGenAsyncMarkers)(GLsizei range);
    GLboolean (APIENTRY *glIsAsyncMarker)(GLuint marker);
    GLint (APIENTRY *glPollAsync)(GLuint *markerp);

    // Construction
    SGIXAsync();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXAsync

// GL_SGIX_flush_raster extension API specification.
struct SGIXFlushRaster {
    // Public Members
    void (APIENTRY *glFlushRaster)();

    // Construction
    SGIXFlushRaster();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXFlushRaster

// GL_SGIX_fragment_lighting extension API specification.
// Incomplete extension
struct SGIXFragmentLighting {
    // Public Members
    void (APIENTRY *glFragmentColorMaterial)(GLenum face, GLenum mode);
    void (APIENTRY *glFragmentLightModelf)(GLenum pname, GLfloat param);
    void (APIENTRY *glFragmentLightModelfv)(GLenum pname, const GLfloat *params);
    void (APIENTRY *glFragmentLightModeli)(GLenum pname, GLint param);
    void (APIENTRY *glFragmentLightModeliv)(GLenum pname, const GLint *params);
    void (APIENTRY *glFragmentLightf)(GLenum light, GLenum pname, GLfloat param);
    void (APIENTRY *glFragmentLightfv)(GLenum light, GLenum pname,
                                       const GLfloat *params);
    void (APIENTRY *glFragmentLighti)(GLenum light, GLenum pname, GLint param);
    void (APIENTRY *glFragmentLightiv)(GLenum light, GLenum pname,
                                       const GLint *params);
    void (APIENTRY *glFragmentMaterialf)(GLenum face, GLenum pname,
                                         GLfloat param);
    void (APIENTRY *glFragmentMaterialfv)(GLenum face, GLenum pname,
                                          const GLfloat *params);
    void (APIENTRY *glFragmentMateriali)(GLenum face, GLenum pname, GLint param);
    void (APIENTRY *glFragmentMaterialiv)(GLenum face, GLenum pname,
                                          const GLint *params);
    void (APIENTRY *glGetFragmentLightfv)(GLenum light, GLenum pname,
                                          GLfloat *params);
    void (APIENTRY *glGetFragmentLightiv)(GLenum light, GLenum pname,
                                          GLint *params);
    void (APIENTRY *glGetFragmentMaterialfv)(GLenum face, GLenum pname,
                                             GLfloat *params);
    void (APIENTRY *glGetFragmentMaterialiv)(GLenum face, GLenum pname,
                                             GLint *params);
    void (APIENTRY *glLightEnvi)(GLenum pname, GLint param);

    // Construction
    SGIXFragmentLighting();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXFragmentLighting

// GL_SGIX_framezoom extension API specification.
struct SGIXFramezoom {
    // Public Members
    void (APIENTRY *glFrameZoom)(GLint factor);

    // Construction
    SGIXFramezoom();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXFramezoom

// GL_SGIX_igloo_interface extension API specification.
struct SGIXIglooInterface {
    // Public Members
    void (APIENTRY *glIglooInterface)(GLenum pname, const void *params);

    // Construction
    SGIXIglooInterface();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXIglooInterface

// GL_SGIX_instruments extension API specification.
struct SGIXInstrumentS {
    // Public Members
    GLint (APIENTRY *glGetInstruments)();
    void (APIENTRY *glInstrumentsBuffer)(GLsizei size, GLint *buffer);
    GLint (APIENTRY *glPollInstruments)(GLint *marker_p);
    void (APIENTRY *glReadInstruments)(GLint marker);
    void (APIENTRY *glStartInstruments)();
    void (APIENTRY *glStopInstruments)(GLint marker);

    // Construction
    SGIXInstrumentS();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXInstrumentS

// GL_SGIX_list_priority extension API specification.
struct SGIXListPriority {
    // Public Members
    void (APIENTRY *glGetListParameterfv)(GLuint list, GLenum pname,
                                          GLfloat *params);
    void (APIENTRY *glGetListParameteriv)(GLuint list, GLenum pname,
                                          GLint *params);
    void (APIENTRY *glListParameterf)(GLuint list, GLenum pname, GLfloat param);
    void (APIENTRY *glListParameterfv)(GLuint list, GLenum pname,
                                       const GLfloat *params);
    void (APIENTRY *glListParameteri)(GLuint list, GLenum pname, GLint param);
    void (APIENTRY *glListParameteriv)(GLuint list, GLenum pname,
                                       const GLint *params);

    // Construction
    SGIXListPriority();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXListPriority

// GL_SGIX_pixel_texture extension API specification.
struct SGIXPixelTexture {
    // Public Members
    void (APIENTRY *glPixelTexGen)(GLenum mode);

    // Construction
    SGIXPixelTexture();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXPixelTexture

// GL_SGIX_polynomial_ffd extension API specification.
struct SGIXPolynomialFfd {
    // Public Members
    void (APIENTRY *glDeform)(GLbitfield mask);
    void (APIENTRY *glDeformationMap3d)(GLenum target, GLdouble u1, GLdouble u2,
                                        GLint ustride, GLint uorder, GLdouble v1,
                                        GLdouble v2, GLint vstride, GLint vorder,
                                        GLdouble w1, GLdouble w2, GLint wstride,
                                        GLint worder, const GLdouble *points);
    void (APIENTRY *glDeformationMap3f)(GLenum target, GLfloat u1, GLfloat u2,
                                        GLint ustride, GLint uorder, GLfloat v1,
                                        GLfloat v2, GLint vstride, GLint vorder,
                                        GLfloat w1, GLfloat w2, GLint wstride,
                                        GLint worder, const GLfloat *points);
    void (APIENTRY *glLoadIdentityDeformationMap)(GLbitfield mask);

    // Construction
    SGIXPolynomialFfd();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXPolynomialFfd

// GL_SGIX_reference_plane extension API specification.
struct SGIXReferencePlane {
    // Public Members
    void (APIENTRY *glReferencePlane)(const GLdouble *equation);

    // Construction
    SGIXReferencePlane();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXReferencePlane

// GL_SGIX_sprite extension API specification.
struct SGIXSprite {
    // Public Members
    void (APIENTRY *glSpriteParameterf)(GLenum pname, GLfloat param);
    void (APIENTRY *glSpriteParameterfv)(GLenum pname, const GLfloat *params);
    void (APIENTRY *glSpriteParameteri)(GLenum pname, GLint param);
    void (APIENTRY *glSpriteParameteriv)(GLenum pname, const GLint *params);

    // Construction
    SGIXSprite();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXSprite

// GL_SGIX_tag_sample_buffer extension API specification.
struct SGIXTagSampleBuffer {
    // Public Members
    void (APIENTRY *glTagSampleBuffer)();

    // Construction
    SGIXTagSampleBuffer();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIXTagSampleBuffer

// GL_SGI_color_table extension API specification.
struct SGIColorTable {
    // Public Members
    void (APIENTRY *glColorTableParameterfv)(GLenum target, GLenum pname,
                                             const GLfloat *params);
    void (APIENTRY *glColorTableParameteriv)(GLenum target, GLenum pname,
                                             const GLint *params);
    void (APIENTRY *glColorTable)(GLenum target, GLenum internalformat,
                                  GLsizei width, GLenum format, GLenum type,
                                  const void *table);
    void (APIENTRY *glCopyColorTable)(GLenum target, GLenum internalformat,
                                      GLint x, GLint y, GLsizei width);
    void (APIENTRY *glGetColorTableParameterfv)(GLenum target, GLenum pname,
                                                GLfloat *params);
    void (APIENTRY *glGetColorTableParameteriv)(GLenum target, GLenum pname,
                                                GLint *params);
    void (APIENTRY *glGetColorTable)(GLenum target, GLenum format, GLenum type,
                                     void *table);

    // Construction
    SGIColorTable();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SGIColorTable

// GL_SUNX_constant_data extension API specification.
struct SUNXConstantData {
    // Public Members
    void (APIENTRY *glFinishTexture)();

    // Construction
    SUNXConstantData();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SUNXConstantData

// GL_SUN_global_alpha extension API specification.
struct SUNGlobalAlpha {
    // Public Members
    void (APIENTRY *glGlobalAlphaFactorb)(GLbyte factor);
    void (APIENTRY *glGlobalAlphaFactord)(GLdouble factor);
    void (APIENTRY *glGlobalAlphaFactorf)(GLfloat factor);
    void (APIENTRY *glGlobalAlphaFactori)(GLint factor);
    void (APIENTRY *glGlobalAlphaFactors)(GLshort factor);
    void (APIENTRY *glGlobalAlphaFactorub)(GLubyte factor);
    void (APIENTRY *glGlobalAlphaFactorui)(GLuint factor);
    void (APIENTRY *glGlobalAlphaFactorus)(GLushort factor);

    // Construction
    SUNGlobalAlpha();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SUNGlobalAlpha

// GL_SUN_mesh_array extension API specification.
struct SUNMeshArray {
    // Public Members
    void (APIENTRY *glDrawMeshArrays)(GLenum mode, GLint first, GLsizei count,
                                      GLsizei width);

    // Construction
    SUNMeshArray();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SUNMeshArray

// GL_SUN_triangle_list extension API specification.
struct SUNTriangleList {
    // Public Members
    void (APIENTRY *glReplacementCodePointer)(GLenum type, GLsizei stride,
                                              const void **pointer);
    void (APIENTRY *glReplacementCodeub)(GLubyte code);
    void (APIENTRY *glReplacementCodeubv)(const GLubyte *code);
    void (APIENTRY *glReplacementCodeui)(GLuint code);
    void (APIENTRY *glReplacementCodeuiv)(const GLuint *code);
    void (APIENTRY *glReplacementCodeus)(GLushort code);
    void (APIENTRY *glReplacementCodeusv)(const GLushort *code);

    // Construction
    SUNTriangleList();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SUNTriangleList

// GL_SUN_vertex extension API specification.
struct SUNVertex {
    // Public Members
    void (APIENTRY *glColor3fVertex3f)(GLfloat r, GLfloat g, GLfloat b,
                                       GLfloat x, GLfloat y, GLfloat z);
    void (APIENTRY *glColor3fVertex3fv)(const GLfloat *c, const GLfloat *v);
    void (APIENTRY *glColor4fNormal3fVertex3f)(GLfloat r, GLfloat g, GLfloat b,
                                               GLfloat a, GLfloat nx, GLfloat ny,
                                               GLfloat nz, GLfloat x, GLfloat y,
                                               GLfloat z);
    void (APIENTRY *glColor4fNormal3fVertex3fv)(const GLfloat *c,
                                                const GLfloat *n,
                                                const GLfloat *v);
    void (APIENTRY *glColor4ubVertex2f)(GLubyte r, GLubyte g, GLubyte b,
                                        GLubyte a, GLfloat x, GLfloat y);
    void (APIENTRY *glColor4ubVertex2fv)(const GLubyte *c, const GLfloat *v);
    void (APIENTRY *glColor4ubVertex3f)(GLubyte r, GLubyte g, GLubyte b,
                                        GLubyte a, GLfloat x, GLfloat y,
                                        GLfloat z);
    void (APIENTRY *glColor4ubVertex3fv)(const GLubyte *c, const GLfloat *v);
    void (APIENTRY *glNormal3fVertex3f)(GLfloat nx, GLfloat ny, GLfloat nz,
                                        GLfloat x, GLfloat y, GLfloat z);
    void (APIENTRY *glNormal3fVertex3fv)(const GLfloat *n, const GLfloat *v);
    void (APIENTRY *glReplacementCodeuiColor3fVertex3f)(GLuint rc, GLfloat r,
                                                        GLfloat g, GLfloat b,
                                                        GLfloat x, GLfloat y,
                                                        GLfloat z);
    void (APIENTRY *glReplacementCodeuiColor3fVertex3fv)(const GLuint *rc,
                                                         const GLfloat *c,
                                                         const GLfloat *v);
    void (APIENTRY *glReplacementCodeuiColor4fNormal3fVertex3f)(GLuint rc,
                                                                GLfloat r,
                                                                GLfloat g,
                                                                GLfloat b,
                                                                GLfloat a,
                                                                GLfloat nx,
                                                                GLfloat ny,
                                                                GLfloat nz,
                                                                GLfloat x,
                                                                GLfloat y,
                                                                GLfloat z);
    void (APIENTRY *glReplacementCodeuiColor4fNormal3fVertex3fv)(const GLuint *rc,
                                                                 const GLfloat *c,
                                                                 const GLfloat *n,
                                                                 const GLfloat *v);
    void (APIENTRY *glReplacementCodeuiColor4ubVertex3f)(GLuint rc, GLubyte r,
                                                         GLubyte g, GLubyte b,
                                                         GLubyte a, GLfloat x,
                                                         GLfloat y, GLfloat z);
    void (APIENTRY *glReplacementCodeuiColor4ubVertex3fv)(const GLuint *rc,
                                                          const GLubyte *c,
                                                          const GLfloat *v);
    void (APIENTRY *glReplacementCodeuiNormal3fVertex3f)(GLuint rc, GLfloat nx,
                                                         GLfloat ny, GLfloat nz,
                                                         GLfloat x, GLfloat y,
                                                         GLfloat z);
    void (APIENTRY *glReplacementCodeuiNormal3fVertex3fv)(const GLuint *rc,
                                                          const GLfloat *n,
                                                          const GLfloat *v);
    void (APIENTRY *glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3f)(GLuint rc,
                                                                          GLfloat s,
                                                                          GLfloat t,
                                                                          GLfloat r,
                                                                          GLfloat g,
                                                                          GLfloat b,
                                                                          GLfloat a,
                                                                          GLfloat nx,
                                                                          GLfloat ny,
                                                                          GLfloat nz,
                                                                          GLfloat x,
                                                                          GLfloat y,
                                                                          GLfloat z);
    void (APIENTRY *glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fv)(const GLuint *rc,
                                                                           const GLfloat *tc,
                                                                           const GLfloat *c,
                                                                           const GLfloat *n,
                                                                           const GLfloat *v);
    void (APIENTRY *glReplacementCodeuiTexCoord2fNormal3fVertex3f)(GLuint rc,
                                                                   GLfloat s,
                                                                   GLfloat t,
                                                                   GLfloat nx,
                                                                   GLfloat ny,
                                                                   GLfloat nz,
                                                                   GLfloat x,
                                                                   GLfloat y,
                                                                   GLfloat z);
    void (APIENTRY *glReplacementCodeuiTexCoord2fNormal3fVertex3fv)(const GLuint *rc,
                                                                    const GLfloat *tc,
                                                                    const GLfloat *n,
                                                                    const GLfloat *v);
    void (APIENTRY *glReplacementCodeuiTexCoord2fVertex3f)(GLuint rc, GLfloat s,
                                                           GLfloat t, GLfloat x,
                                                           GLfloat y, GLfloat z);
    void (APIENTRY *glReplacementCodeuiTexCoord2fVertex3fv)(const GLuint *rc,
                                                            const GLfloat *tc,
                                                            const GLfloat *v);
    void (APIENTRY *glReplacementCodeuiVertex3f)(GLuint rc, GLfloat x, GLfloat y,
                                                 GLfloat z);
    void (APIENTRY *glReplacementCodeuiVertex3fv)(const GLuint *rc,
                                                  const GLfloat *v);
    void (APIENTRY *glTexCoord2fColor3fVertex3f)(GLfloat s, GLfloat t, GLfloat r,
                                                 GLfloat g, GLfloat b, GLfloat x,
                                                 GLfloat y, GLfloat z);
    void (APIENTRY *glTexCoord2fColor3fVertex3fv)(const GLfloat *tc,
                                                  const GLfloat *c,
                                                  const GLfloat *v);
    void (APIENTRY *glTexCoord2fColor4fNormal3fVertex3f)(GLfloat s, GLfloat t,
                                                         GLfloat r, GLfloat g,
                                                         GLfloat b, GLfloat a,
                                                         GLfloat nx, GLfloat ny,
                                                         GLfloat nz, GLfloat x,
                                                         GLfloat y, GLfloat z);
    void (APIENTRY *glTexCoord2fColor4fNormal3fVertex3fv)(const GLfloat *tc,
                                                          const GLfloat *c,
                                                          const GLfloat *n,
                                                          const GLfloat *v);
    void (APIENTRY *glTexCoord2fColor4ubVertex3f)(GLfloat s, GLfloat t,
                                                  GLubyte r, GLubyte g,
                                                  GLubyte b, GLubyte a,
                                                  GLfloat x, GLfloat y,
                                                  GLfloat z);
    void (APIENTRY *glTexCoord2fColor4ubVertex3fv)(const GLfloat *tc,
                                                   const GLubyte *c,
                                                   const GLfloat *v);
    void (APIENTRY *glTexCoord2fNormal3fVertex3f)(GLfloat s, GLfloat t,
                                                  GLfloat nx, GLfloat ny,
                                                  GLfloat nz, GLfloat x,
                                                  GLfloat y, GLfloat z);
    void (APIENTRY *glTexCoord2fNormal3fVertex3fv)(const GLfloat *tc,
                                                   const GLfloat *n,
                                                   const GLfloat *v);
    void (APIENTRY *glTexCoord2fVertex3f)(GLfloat s, GLfloat t, GLfloat x,
                                          GLfloat y, GLfloat z);
    void (APIENTRY *glTexCoord2fVertex3fv)(const GLfloat *tc, const GLfloat *v);
    void (APIENTRY *glTexCoord4fColor4fNormal3fVertex4f)(GLfloat s, GLfloat t,
                                                         GLfloat p, GLfloat q,
                                                         GLfloat r, GLfloat g,
                                                         GLfloat b, GLfloat a,
                                                         GLfloat nx, GLfloat ny,
                                                         GLfloat nz, GLfloat x,
                                                         GLfloat y, GLfloat z,
                                                         GLfloat w);
    void (APIENTRY *glTexCoord4fColor4fNormal3fVertex4fv)(const GLfloat *tc,
                                                          const GLfloat *c,
                                                          const GLfloat *n,
                                                          const GLfloat *v);
    void (APIENTRY *glTexCoord4fVertex4f)(GLfloat s, GLfloat t, GLfloat p,
                                          GLfloat q, GLfloat x, GLfloat y,
                                          GLfloat z, GLfloat w);
    void (APIENTRY *glTexCoord4fVertex4fv)(const GLfloat *tc, const GLfloat *v);

    // Construction
    SUNVertex();

    // Operations
    bool isPresent(const APIResolver *resolver) const;
    void resolveEntryPoints(const APIResolver *resolver);
}; // struct SUNVertex


} // namespace gl

#endif // ifndef __061B23F4_7783_415E_B538_DD0F5B335134_INCLUDED__

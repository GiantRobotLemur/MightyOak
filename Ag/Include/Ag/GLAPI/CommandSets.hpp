//! @file Ag/GLAPI/CommandSets.hpp
//! @brief A file generated from the OpenGL API schema definition to create
//! C++ 17 language bindings.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __C887F2C6_25FE_46FA_A234_66CE1D192F9E_INCLUDED__
#define __C887F2C6_25FE_46FA_A234_66CE1D192F9E_INCLUDED__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Version.hpp"

#include "APIs.hpp"
#include "Identifiers.hpp"

namespace gl {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
// A base class for a wrapper to a Core API.
class BaseAPI
{
protected:
    // Construction
    BaseAPI();
public:
    virtual ~BaseAPI() = default;

    // Operations
    virtual void resolve(const APIResolver* resolver);

protected:
    // Internal Functions
    void beforeCommand(const char *commandName, bool isNonNull) const;
    void afterCommand(const char *commandName) const;
private:
    // Internal Fields
    unsigned int (APIENTRY *_getError)();
};

// A base class for a wrapper to an extension API.
class BaseExtension : public BaseAPI
{
protected:
    // Construction
    BaseExtension(const char *extName);

public:
    virtual ~BaseExtension() = default;

    // Accessors
    bool isPresent() const;

    // Operations
    virtual void resolve(const APIResolver* resolver) override;

private:
    // Internal Fields
    const char *_extName;
    bool _isPresent;
};

// Manages the entry points for the main API.
class GLAPI : public BaseAPI
{
public:
    // Construction
    GLAPI();

    // Accessors
    const GL &getRawAPI() const;
    const Ag::Version &getAPIVersion() const;

    // Operations
    void verifyAPIVersion(const Ag::Version &minRequired, const char *feature) const;

    // GL 1.0
    void blendFunc(BlendingFactor sfactor, BlendingFactor dfactor) const;
    void clear(GLbitfield  /* ClearBufferMask */ mask) const;
    void clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) const;
    void clearDepth(GLdouble depth) const;
    void clearStencil(GLint s) const;
    void colorMask(Boolean red, Boolean green, Boolean blue, Boolean alpha) const;
    void cullFace(CullFaceMode mode) const;
    void depthFunc(DepthFunction func) const;
    void depthMask(Boolean flag) const;
    void depthRange(GLdouble n, GLdouble f) const;
    void disable(EnableCap cap) const;
    void drawBuffer(DrawBufferMode buf) const;
    void enable(EnableCap cap) const;
    void finish() const;
    void flush() const;
    void frontFace(FrontFaceDirection mode) const;
    void getBooleanV(GetPName pname, Boolean *data) const;
    void getDoubleV(GetPName pname, GLdouble *data) const;
    ErrorCode getError() const;
    void getFloatV(GetPName pname, GLfloat *data) const;
    void getIntegerV(GetPName pname, GLint *data) const;
    const char *getString(StringName name) const;
    void getTexImage(TextureTarget target, GLint level, PixelFormat format,
                     PixelType type, void *pixels) const;
    void getTexLevelParameterFV(TextureTarget target, GLint level,
                                GetTextureParameter pname, GLfloat *params) const;
    void getTexLevelParameterIV(TextureTarget target, GLint level,
                                GetTextureParameter pname, GLint *params) const;
    void getTexParameterFV(TextureTarget target, GetTextureParameter pname,
                           GLfloat *params) const;
    void getTexParameterIV(TextureTarget target, GetTextureParameter pname,
                           GLint *params) const;
    void hint(HintTarget target, HintMode mode) const;
    Boolean isEnabled(EnableCap cap) const;
    void lineWidth(GLfloat width) const;
    void logicOp(LogicOp opcode) const;
    void pixelStoreF(PixelStoreParameter pname, GLfloat param) const;
    void pixelStoreI(PixelStoreParameter pname, GLint param) const;
    void pointSize(GLfloat size) const;
    void polygonMode(MaterialFace face, PolygonMode mode) const;
    void readBuffer(ReadBufferMode src) const;
    void readPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                    PixelFormat format, PixelType type, void *pixels) const;
    void scissor(GLint x, GLint y, GLsizei width, GLsizei height) const;
    void stencilFunc(StencilFunction func, GLint ref, GLuint mask) const;
    void stencilMask(GLuint mask) const;
    void stencilOp(StencilOp fail, StencilOp zfail, StencilOp zpass) const;
    void texImage1D(TextureTarget target, GLint level,
                    InternalFormat internalformat, GLsizei width, GLint border,
                    PixelFormat format, PixelType type, const void *pixels) const;
    void texImage2D(TextureTarget target, GLint level,
                    InternalFormat internalformat, GLsizei width, GLsizei height,
                    GLint border, PixelFormat format, PixelType type,
                    const void *pixels) const;
    void texParameterF(TextureTarget target, TextureParameterName pname,
                       GLfloat param) const;
    void texParameterFV(TextureTarget target, TextureParameterName pname,
                        const GLfloat *params) const;
    void texParameterI(TextureTarget target, TextureParameterName pname,
                       GLint param) const;
    void texParameterIV(TextureTarget target, TextureParameterName pname,
                        const GLint *params) const;
    void viewport(GLint x, GLint y, GLsizei width, GLsizei height) const;

    // GL 1.1
    void bindTexture(TextureTarget target, TextureName texture) const;
    void copyTexImage1D(TextureTarget target, GLint level,
                        InternalFormat internalformat, GLint x, GLint y,
                        GLsizei width, GLint border) const;
    void copyTexImage2D(TextureTarget target, GLint level,
                        InternalFormat internalformat, GLint x, GLint y,
                        GLsizei width, GLsizei height, GLint border) const;
    void copyTexSubImage1D(TextureTarget target, GLint level, GLint xoffset,
                           GLint x, GLint y, GLsizei width) const;
    void copyTexSubImage2D(TextureTarget target, GLint level, GLint xoffset,
                           GLint yoffset, GLint x, GLint y, GLsizei width,
                           GLsizei height) const;
    void deleteTextures(GLsizei n, const TextureName *textures) const;
    void drawArrays(PrimitiveType mode, GLint first, GLsizei count) const;
    void drawElements(PrimitiveType mode, GLsizei count, DrawElementsType type,
                      const void *indices) const;
    void genTextures(GLsizei n, TextureName *textures) const;
    Boolean isTexture(TextureName texture) const;
    void polygonOffset(GLfloat factor, GLfloat units) const;
    void texSubImage1D(TextureTarget target, GLint level, GLint xoffset,
                       GLsizei width, PixelFormat format, PixelType type,
                       const void *pixels) const;
    void texSubImage2D(TextureTarget target, GLint level, GLint xoffset,
                       GLint yoffset, GLsizei width, GLsizei height,
                       PixelFormat format, PixelType type, const void *pixels) const;

    // GL 1.2
    void copyTexSubImage3D(TextureTarget target, GLint level, GLint xoffset,
                           GLint yoffset, GLint zoffset, GLint x, GLint y,
                           GLsizei width, GLsizei height) const;
    void drawRangeElements(PrimitiveType mode, GLuint start, GLuint end,
                           GLsizei count, DrawElementsType type,
                           const void *indices) const;
    void texImage3D(TextureTarget target, GLint level,
                    InternalFormat internalformat, GLsizei width, GLsizei height,
                    GLsizei depth, GLint border, PixelFormat format,
                    PixelType type, const void *pixels) const;
    void texSubImage3D(TextureTarget target, GLint level, GLint xoffset,
                       GLint yoffset, GLint zoffset, GLsizei width,
                       GLsizei height, GLsizei depth, PixelFormat format,
                       PixelType type, const void *pixels) const;

    // GL 1.3
    void activeTexture(TextureUnit texture) const;
    void compressedTexImage1D(TextureTarget target, GLint level,
                              InternalFormat internalformat, GLsizei width,
                              GLint border, GLsizei imageSize, const void *data) const;
    void compressedTexImage2D(TextureTarget target, GLint level,
                              InternalFormat internalformat, GLsizei width,
                              GLsizei height, GLint border, GLsizei imageSize,
                              const void *data) const;
    void compressedTexImage3D(TextureTarget target, GLint level,
                              InternalFormat internalformat, GLsizei width,
                              GLsizei height, GLsizei depth, GLint border,
                              GLsizei imageSize, const void *data) const;
    void compressedTexSubImage1D(TextureTarget target, GLint level,
                                 GLint xoffset, GLsizei width,
                                 InternalFormat format, GLsizei imageSize,
                                 const void *data) const;
    void compressedTexSubImage2D(TextureTarget target, GLint level,
                                 GLint xoffset, GLint yoffset, GLsizei width,
                                 GLsizei height, InternalFormat format,
                                 GLsizei imageSize, const void *data) const;
    void compressedTexSubImage3D(TextureTarget target, GLint level,
                                 GLint xoffset, GLint yoffset, GLint zoffset,
                                 GLsizei width, GLsizei height, GLsizei depth,
                                 InternalFormat format, GLsizei imageSize,
                                 const void *data) const;
    void getCompressedTexImage(TextureTarget target, GLint level, void *img) const;
    void sampleCoverage(GLfloat value, Boolean invert) const;

    // GL 1.4
    void blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) const;
    void blendEquation(BlendEquationModeEXT mode) const;
    void blendFuncSeparate(BlendingFactor sfactorRGB, BlendingFactor dfactorRGB,
                           BlendingFactor sfactorAlpha,
                           BlendingFactor dfactorAlpha) const;
    void multiDrawArrays(PrimitiveType mode, const GLint *first,
                         const GLsizei *count, GLsizei drawcount) const;
    void multiDrawElements(PrimitiveType mode, const GLsizei *count,
                           DrawElementsType type, const void *const*indices,
                           GLsizei drawcount) const;
    void pointParameterF(PointParameterNameARB pname, GLfloat param) const;
    void pointParameterFV(PointParameterNameARB pname, const GLfloat *params) const;
    void pointParameterI(PointParameterNameARB pname, GLint param) const;
    void pointParameterIV(PointParameterNameARB pname, const GLint *params) const;

    // GL 1.5
    void beginQuery(QueryTarget target, QueryName id) const;
    void bindBuffer(BufferTarget target, BufferName buffer) const;
    void bufferData(BufferTarget target, GLsizeiptr size, const void *data,
                    BufferUsage usage) const;
    void bufferSubData(BufferTarget target, GLintptr offset, GLsizeiptr size,
                       const void *data) const;
    void deleteBuffers(GLsizei n, const BufferName *buffers) const;
    void deleteQueries(GLsizei n, const QueryName *ids) const;
    void endQuery(QueryTarget target) const;
    void genBuffers(GLsizei n, BufferName *buffers) const;
    void genQueries(GLsizei n, QueryName *ids) const;
    void getBufferParameterIV(BufferTarget target, BufferPNameARB pname,
                              GLint *params) const;
    void getBufferPointerV(BufferTarget target, BufferPointerNameARB pname,
                           void **params) const;
    void getBufferSubData(BufferTarget target, GLintptr offset,
                          GLsizeiptr size, void *data) const;
    void getQueryObjectIV(QueryName id, QueryObjectParameterName pname,
                          GLint *params) const;
    void getQueryObjectUIV(QueryName id, QueryObjectParameterName pname,
                           GLuint *params) const;
    void getQueryIV(QueryTarget target, QueryParameterName pname, GLint *params) const;
    Boolean isBuffer(BufferName buffer) const;
    Boolean isQuery(QueryName id) const;
    void *mapBuffer(BufferTarget target, BufferAccessARB access) const;
    Boolean unmapBuffer(BufferTarget target) const;

    // GL 2.0
    void attachShader(ProgramName program, ShaderName shader) const;
    void bindAttribLocation(ProgramName program, GLuint index,
                            const GLchar *name) const;
    void blendEquationSeparate(BlendEquationModeEXT modeRGB,
                               BlendEquationModeEXT modeAlpha) const;
    void compileShader(ShaderName shader) const;
    ProgramName createProgram() const;
    ShaderName createShader(ShaderType type) const;
    void deleteProgram(ProgramName program) const;
    void deleteShader(ShaderName shader) const;
    void detachShader(ProgramName program, ShaderName shader) const;
    void disableVertexAttribArray(GLuint index) const;
    void drawBuffers(GLsizei n, const DrawBufferMode *bufs) const;
    void enableVertexAttribArray(GLuint index) const;
    void getActiveAttrib(ProgramName program, GLuint index, GLsizei bufSize,
                         GLsizei *length, GLint *size, AttributeType *type,
                         GLchar *name) const;
    void getActiveUniform(ProgramName program, GLuint index, GLsizei bufSize,
                          GLsizei *length, GLint *size, UniformType *type,
                          GLchar *name) const;
    void getAttachedShaders(ProgramName program, GLsizei maxCount,
                            GLsizei *count, ShaderName *shaders) const;
    GLint getAttribLocation(ProgramName program, const GLchar *name) const;
    void getProgramInfoLog(ProgramName program, GLsizei bufSize, GLsizei *length,
                           GLchar *infoLog) const;
    void getProgramIV(ProgramName program, ProgramProperty pname,
                      GLint *params) const;
    void getShaderInfoLog(ShaderName shader, GLsizei bufSize, GLsizei *length,
                          GLchar *infoLog) const;
    void getShaderSource(ShaderName shader, GLsizei bufSize, GLsizei *length,
                         GLchar *source) const;
    void getShaderIV(ShaderName shader, ShaderParameterName pname, GLint *params) const;
    GLint getUniformLocation(ProgramName program, const GLchar *name) const;
    void getUniformFV(ProgramName program, GLint location, GLfloat *params) const;
    void getUniformIV(ProgramName program, GLint location, GLint *params) const;
    void getVertexAttribPointerV(GLuint index,
                                 VertexAttribPointerPropertyARB pname,
                                 void **pointer) const;
    void getVertexAttribDV(GLuint index, VertexAttribPropertyARB pname,
                           GLdouble *params) const;
    void getVertexAttribFV(GLuint index, VertexAttribPropertyARB pname,
                           GLfloat *params) const;
    void getVertexAttribIV(GLuint index, VertexAttribPropertyARB pname,
                           GLint *params) const;
    Boolean isProgram(ProgramName program) const;
    Boolean isShader(ShaderName shader) const;
    void linkProgram(ProgramName program) const;
    void shaderSource(ShaderName shader, GLsizei count,
                      const GLchar *const*string, const GLint *length) const;
    void stencilFuncSeparate(StencilFaceDirection face, StencilFunction func,
                             GLint ref, GLuint mask) const;
    void stencilMaskSeparate(StencilFaceDirection face, GLuint mask) const;
    void stencilOpSeparate(StencilFaceDirection face, StencilOp sfail,
                           StencilOp dpfail, StencilOp dppass) const;
    void uniform1F(GLint location, GLfloat v0) const;
    void uniform1FV(GLint location, GLsizei count, const GLfloat *value) const;
    void uniform1I(GLint location, GLint v0) const;
    void uniform1IV(GLint location, GLsizei count, const GLint *value) const;
    void uniform2F(GLint location, GLfloat v0, GLfloat v1) const;
    void uniform2FV(GLint location, GLsizei count, const GLfloat *value) const;
    void uniform2I(GLint location, GLint v0, GLint v1) const;
    void uniform2IV(GLint location, GLsizei count, const GLint *value) const;
    void uniform3F(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) const;
    void uniform3FV(GLint location, GLsizei count, const GLfloat *value) const;
    void uniform3I(GLint location, GLint v0, GLint v1, GLint v2) const;
    void uniform3IV(GLint location, GLsizei count, const GLint *value) const;
    void uniform4F(GLint location, GLfloat v0, GLfloat v1, GLfloat v2,
                   GLfloat v3) const;
    void uniform4FV(GLint location, GLsizei count, const GLfloat *value) const;
    void uniform4I(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) const;
    void uniform4IV(GLint location, GLsizei count, const GLint *value) const;
    void uniformMatrix2FV(GLint location, GLsizei count, Boolean transpose,
                          const GLfloat *value) const;
    void uniformMatrix3FV(GLint location, GLsizei count, Boolean transpose,
                          const GLfloat *value) const;
    void uniformMatrix4FV(GLint location, GLsizei count, Boolean transpose,
                          const GLfloat *value) const;
    void useProgram(ProgramName program) const;
    void validateProgram(ProgramName program) const;
    void vertexAttrib1D(GLuint index, GLdouble x) const;
    void vertexAttrib1DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib1F(GLuint index, GLfloat x) const;
    void vertexAttrib1FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib1S(GLuint index, GLshort x) const;
    void vertexAttrib1SV(GLuint index, const GLshort *v) const;
    void vertexAttrib2D(GLuint index, GLdouble x, GLdouble y) const;
    void vertexAttrib2DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib2F(GLuint index, GLfloat x, GLfloat y) const;
    void vertexAttrib2FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib2S(GLuint index, GLshort x, GLshort y) const;
    void vertexAttrib2SV(GLuint index, const GLshort *v) const;
    void vertexAttrib3D(GLuint index, GLdouble x, GLdouble y, GLdouble z) const;
    void vertexAttrib3DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib3F(GLuint index, GLfloat x, GLfloat y, GLfloat z) const;
    void vertexAttrib3FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib3S(GLuint index, GLshort x, GLshort y, GLshort z) const;
    void vertexAttrib3SV(GLuint index, const GLshort *v) const;
    void vertexAttrib4NBV(GLuint index, const GLbyte *v) const;
    void vertexAttrib4NIV(GLuint index, const GLint *v) const;
    void vertexAttrib4NSV(GLuint index, const GLshort *v) const;
    void vertexAttrib4NUB(GLuint index, GLubyte x, GLubyte y, GLubyte z,
                          GLubyte w) const;
    void vertexAttrib4NUBV(GLuint index, const GLubyte *v) const;
    void vertexAttrib4NUIV(GLuint index, const GLuint *v) const;
    void vertexAttrib4NUSV(GLuint index, const GLushort *v) const;
    void vertexAttrib4BV(GLuint index, const GLbyte *v) const;
    void vertexAttrib4D(GLuint index, GLdouble x, GLdouble y, GLdouble z,
                        GLdouble w) const;
    void vertexAttrib4DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib4F(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
    void vertexAttrib4FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib4IV(GLuint index, const GLint *v) const;
    void vertexAttrib4S(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w) const;
    void vertexAttrib4SV(GLuint index, const GLshort *v) const;
    void vertexAttrib4UBV(GLuint index, const GLubyte *v) const;
    void vertexAttrib4UIV(GLuint index, const GLuint *v) const;
    void vertexAttrib4USV(GLuint index, const GLushort *v) const;
    void vertexAttribPointer(GLuint index, GLint size,
                             VertexAttribPointerType type, Boolean normalized,
                             GLsizei stride, const void *pointer) const;

    // GL 2.1
    void uniformMatrix2x3FV(GLint location, GLsizei count, Boolean transpose,
                            const GLfloat *value) const;
    void uniformMatrix2x4FV(GLint location, GLsizei count, Boolean transpose,
                            const GLfloat *value) const;
    void uniformMatrix3x2FV(GLint location, GLsizei count, Boolean transpose,
                            const GLfloat *value) const;
    void uniformMatrix3x4FV(GLint location, GLsizei count, Boolean transpose,
                            const GLfloat *value) const;
    void uniformMatrix4x2FV(GLint location, GLsizei count, Boolean transpose,
                            const GLfloat *value) const;
    void uniformMatrix4x3FV(GLint location, GLsizei count, Boolean transpose,
                            const GLfloat *value) const;

    // GL 3.0
    void beginConditionalRender(GLuint id, ConditionalRenderMode mode) const;
    void beginTransformFeedback(PrimitiveType primitiveMode) const;
    void bindBufferBase(BufferTarget target, GLuint index, BufferName buffer) const;
    void bindBufferRange(BufferTarget target, GLuint index, BufferName buffer,
                         GLintptr offset, GLsizeiptr size) const;
    void bindFragDataLocation(ProgramName program, GLuint color,
                              const GLchar *name) const;
    void bindFramebuffer(FramebufferTarget target, FrameBufferName framebuffer) const;
    void bindRenderbuffer(RenderBufferTarget target,
                          RenderBufferName renderbuffer) const;
    void bindVertexArray(VertexArrayName array) const;
    void blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                         GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                         GLbitfield  /* ClearBufferMask */ mask,
                         BlitFramebufferFilter filter) const;
    FramebufferStatus checkFramebufferStatus(FramebufferTarget target) const;
    void clampColor(ClampColorTargetARB target, ClampColorModeARB clamp) const;
    void clearBufferFI(BufferEnum buffer, GLint drawbuffer, GLfloat depth,
                       GLint stencil) const;
    void clearBufferFV(BufferEnum buffer, GLint drawbuffer, const GLfloat *value) const;
    void clearBufferIV(BufferEnum buffer, GLint drawbuffer, const GLint *value) const;
    void clearBufferUIV(BufferEnum buffer, GLint drawbuffer, const GLuint *value) const;
    void colorMaskI(GLuint index, Boolean r, Boolean g, Boolean b, Boolean a) const;
    void deleteFramebuffers(GLsizei n, const FrameBufferName *framebuffers) const;
    void deleteRenderbuffers(GLsizei n, const RenderBufferName *renderbuffers) const;
    void deleteVertexArrays(GLsizei n, const VertexArrayName *arrays) const;
    void disableI(EnableCap target, GLuint index) const;
    void enableI(EnableCap target, GLuint index) const;
    void endConditionalRender() const;
    void endTransformFeedback() const;
    void flushMappedBufferRange(BufferTarget target, GLintptr offset,
                                GLsizeiptr length) const;
    void framebufferRenderbuffer(FramebufferTarget target,
                                 FramebufferAttachment attachment,
                                 RenderBufferTarget renderbuffertarget,
                                 RenderBufferName renderbuffer) const;
    void framebufferTexture1D(FramebufferTarget target,
                              FramebufferAttachment attachment,
                              TextureTarget textarget, TextureName texture,
                              GLint level) const;
    void framebufferTexture2D(FramebufferTarget target,
                              FramebufferAttachment attachment,
                              TextureTarget textarget, TextureName texture,
                              GLint level) const;
    void framebufferTexture3D(FramebufferTarget target,
                              FramebufferAttachment attachment,
                              TextureTarget textarget, TextureName texture,
                              GLint level, GLint zoffset) const;
    void framebufferTextureLayer(FramebufferTarget target,
                                 FramebufferAttachment attachment,
                                 TextureName texture, GLint level, GLint layer) const;
    void genFramebuffers(GLsizei n, FrameBufferName *framebuffers) const;
    void genRenderbuffers(GLsizei n, RenderBufferName *renderbuffers) const;
    void genVertexArrays(GLsizei n, VertexArrayName *arrays) const;
    void generateMipmap(TextureTarget target) const;
    void getBooleanIV(BufferTarget target, GLuint index, Boolean *data) const;
    GLint getFragDataLocation(ProgramName program, const GLchar *name) const;
    void getFramebufferAttachmentParameterIV(FramebufferTarget target,
                                             FramebufferAttachment attachment,
                                             FramebufferAttachmentParameterName pname,
                                             GLint *params) const;
    void getIntegerIV(GetPName target, GLuint index, GLint *data) const;
    void getRenderbufferParameterIV(RenderBufferTarget target,
                                    RenderBufferParameterName pname,
                                    GLint *params) const;
    const char *getStringI(StringName name, GLuint index) const;
    void getTexParameterIIV(TextureTarget target, GetTextureParameter pname,
                            GLint *params) const;
    void getTexParameterIUIV(TextureTarget target, GetTextureParameter pname,
                             GLuint *params) const;
    void getTransformFeedbackVarying(ProgramName program, GLuint index,
                                     GLsizei bufSize, GLsizei *length,
                                     GLsizei *size, AttributeType *type,
                                     GLchar *name) const;
    void getUniformUIV(ProgramName program, GLint location, GLuint *params) const;
    void getVertexAttribIIV(GLuint index, VertexAttribEnum pname, GLint *params) const;
    void getVertexAttribIUIV(GLuint index, VertexAttribEnum pname,
                             GLuint *params) const;
    Boolean isEnabledI(EnableCap target, GLuint index) const;
    Boolean isFramebuffer(FrameBufferName framebuffer) const;
    Boolean isRenderbuffer(RenderBufferName renderbuffer) const;
    Boolean isVertexArray(VertexArrayName array) const;
    void *mapBufferRange(BufferTarget target, GLintptr offset,
                         GLsizeiptr length,
                         GLbitfield  /* MapBufferAccessMask */ access) const;
    void renderbufferStorage(RenderBufferTarget target,
                             InternalFormat internalformat, GLsizei width,
                             GLsizei height) const;
    void renderbufferStorageMultisample(RenderBufferTarget target,
                                        GLsizei samples,
                                        InternalFormat internalformat,
                                        GLsizei width, GLsizei height) const;
    void texParameterIIV(TextureTarget target, TextureParameterName pname,
                         const GLint *params) const;
    void texParameterIUIV(TextureTarget target, TextureParameterName pname,
                          const GLuint *params) const;
    void transformFeedbackVaryings(ProgramName program, GLsizei count,
                                   const GLchar *const*varyings,
                                   TransformFeedbackBufferMode bufferMode) const;
    void uniform1UI(GLint location, GLuint v0) const;
    void uniform1UIV(GLint location, GLsizei count, const GLuint *value) const;
    void uniform2UI(GLint location, GLuint v0, GLuint v1) const;
    void uniform2UIV(GLint location, GLsizei count, const GLuint *value) const;
    void uniform3UI(GLint location, GLuint v0, GLuint v1, GLuint v2) const;
    void uniform3UIV(GLint location, GLsizei count, const GLuint *value) const;
    void uniform4UI(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) const;
    void uniform4UIV(GLint location, GLsizei count, const GLuint *value) const;
    void vertexAttribI1I(GLuint index, GLint x) const;
    void vertexAttribI1IV(GLuint index, const GLint *v) const;
    void vertexAttribI1UI(GLuint index, GLuint x) const;
    void vertexAttribI1UIV(GLuint index, const GLuint *v) const;
    void vertexAttribI2I(GLuint index, GLint x, GLint y) const;
    void vertexAttribI2IV(GLuint index, const GLint *v) const;
    void vertexAttribI2UI(GLuint index, GLuint x, GLuint y) const;
    void vertexAttribI2UIV(GLuint index, const GLuint *v) const;
    void vertexAttribI3I(GLuint index, GLint x, GLint y, GLint z) const;
    void vertexAttribI3IV(GLuint index, const GLint *v) const;
    void vertexAttribI3UI(GLuint index, GLuint x, GLuint y, GLuint z) const;
    void vertexAttribI3UIV(GLuint index, const GLuint *v) const;
    void vertexAttribI4BV(GLuint index, const GLbyte *v) const;
    void vertexAttribI4I(GLuint index, GLint x, GLint y, GLint z, GLint w) const;
    void vertexAttribI4IV(GLuint index, const GLint *v) const;
    void vertexAttribI4SV(GLuint index, const GLshort *v) const;
    void vertexAttribI4UBV(GLuint index, const GLubyte *v) const;
    void vertexAttribI4UI(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) const;
    void vertexAttribI4UIV(GLuint index, const GLuint *v) const;
    void vertexAttribI4USV(GLuint index, const GLushort *v) const;
    void vertexAttribIPointer(GLuint index, GLint size, VertexAttribIType type,
                              GLsizei stride, const void *pointer) const;

    // GL 3.1
    void copyBufferSubData(CopyBufferSubDataTarget readTarget,
                           CopyBufferSubDataTarget writeTarget,
                           GLintptr readOffset, GLintptr writeOffset,
                           GLsizeiptr size) const;
    void drawArraysInstanced(PrimitiveType mode, GLint first, GLsizei count,
                             GLsizei instancecount) const;
    void drawElementsInstanced(PrimitiveType mode, GLsizei count,
                               DrawElementsType type, const void *indices,
                               GLsizei instancecount) const;
    void getActiveUniformBlockName(ProgramName program, GLuint uniformBlockIndex,
                                   GLsizei bufSize, GLsizei *length,
                                   GLchar *uniformBlockName) const;
    void getActiveUniformBlockIV(ProgramName program, GLuint uniformBlockIndex,
                                 UniformBlockPName pname, GLint *params) const;
    void getActiveUniformName(ProgramName program, GLuint uniformIndex,
                              GLsizei bufSize, GLsizei *length,
                              GLchar *uniformName) const;
    void getActiveUniformsIV(ProgramName program, GLsizei uniformCount,
                             const GLuint *uniformIndices, UniformPName pname,
                             GLint *params) const;
    GLuint getUniformBlockIndex(ProgramName program,
                                const GLchar *uniformBlockName) const;
    void getUniformIndices(ProgramName program, GLsizei uniformCount,
                           const GLchar *const*uniformNames,
                           GLuint *uniformIndices) const;
    void primitiveRestartIndex(GLuint index) const;
    void texBuffer(TextureTarget target, SizedInternalFormat internalformat,
                   BufferName buffer) const;
    void uniformBlockBinding(ProgramName program, GLuint uniformBlockIndex,
                             GLuint uniformBlockBinding) const;

    // GL 3.2
    SyncStatus clientWaitSync(SyncName sync,
                              GLbitfield  /* SyncObjectMask */ flags,
                              GLuint64 timeout) const;
    void deleteSync(SyncName sync) const;
    void drawElementsBaseVertex(PrimitiveType mode, GLsizei count,
                                DrawElementsType type, const void *indices,
                                GLint basevertex) const;
    void drawElementsInstancedBaseVertex(PrimitiveType mode, GLsizei count,
                                         DrawElementsType type,
                                         const void *indices,
                                         GLsizei instancecount, GLint basevertex) const;
    void drawRangeElementsBaseVertex(PrimitiveType mode, GLuint start,
                                     GLuint end, GLsizei count,
                                     DrawElementsType type, const void *indices,
                                     GLint basevertex) const;
    SyncName fenceSync(SyncCondition condition, SyncBehaviorFlags flags) const;
    void framebufferTexture(FramebufferTarget target,
                            FramebufferAttachment attachment,
                            TextureName texture, GLint level) const;
    void getBufferParameterI64V(BufferTarget target, BufferPNameARB pname,
                                GLint64 *params) const;
    void getInteger64IV(GetPName target, GLuint index, GLint64 *data) const;
    void getInteger64V(GetPName pname, GLint64 *data) const;
    void getMultisampleFV(GetMultisamplePNameNV pname, GLuint index,
                          GLfloat *val) const;
    void getSyncIV(SyncName sync, SyncParameterName pname, GLsizei count,
                   GLsizei *length, GLint *values) const;
    Boolean isSync(SyncName sync) const;
    void multiDrawElementsBaseVertex(PrimitiveType mode, const GLsizei *count,
                                     DrawElementsType type,
                                     const void *const*indices,
                                     GLsizei drawcount, const GLint *basevertex) const;
    void provokingVertex(VertexProvokingMode mode) const;
    void sampleMaskI(GLuint maskNumber, GLbitfield mask) const;
    void texImage2DMultisample(TextureTarget target, GLsizei samples,
                               InternalFormat internalformat, GLsizei width,
                               GLsizei height, Boolean fixedsamplelocations) const;
    void texImage3DMultisample(TextureTarget target, GLsizei samples,
                               InternalFormat internalformat, GLsizei width,
                               GLsizei height, GLsizei depth,
                               Boolean fixedsamplelocations) const;
    void waitSync(SyncName sync, SyncBehaviorFlags flags, GLuint64 timeout) const;

    // GL 3.3
    void bindFragDataLocationIndexed(ProgramName program, GLuint colorNumber,
                                     GLuint index, const GLchar *name) const;
    void bindSampler(GLuint unit, SamplerName sampler) const;
    void deleteSamplers(GLsizei count, const SamplerName *samplers) const;
    void genSamplers(GLsizei count, SamplerName *samplers) const;
    GLint getFragDataIndex(ProgramName program, const GLchar *name) const;
    void getQueryObjectI64V(QueryName id, QueryObjectParameterName pname,
                            GLint64 *params) const;
    void getQueryObjectUI64(QueryName id, QueryObjectParameterName pname,
                            GLuint64 *params) const;
    void getSamplerParameterIIV(SamplerName sampler, SamplerParameterI pname,
                                GLint *params) const;
    void getSamplerParameterIUIV(SamplerName sampler, SamplerParameterI pname,
                                 GLuint *params) const;
    void getSamplerParameterFV(SamplerName sampler, SamplerParameterF pname,
                               GLfloat *params) const;
    void getSamplerParameterIV(SamplerName sampler, SamplerParameterI pname,
                               GLint *params) const;
    Boolean isSampler(SamplerName sampler) const;
    void queryCounter(QueryName id, QueryCounterTarget target) const;
    void samplerParameterIIV(SamplerName sampler, SamplerParameterI pname,
                             const GLint *param) const;
    void samplerParameterIUIV(SamplerName sampler, SamplerParameterI pname,
                              const GLuint *param) const;
    void samplerParameterF(SamplerName sampler, SamplerParameterF pname,
                           GLfloat param) const;
    void samplerParameterFV(SamplerName sampler, SamplerParameterF pname,
                            const GLfloat *param) const;
    void samplerParameterI(SamplerName sampler, SamplerParameterI pname,
                           GLint param) const;
    void samplerParameterIV(SamplerName sampler, SamplerParameterI pname,
                            const GLint *param) const;
    void vertexAttribDivisor(GLuint index, GLuint divisor) const;
    void vertexAttribP1UI(GLuint index, VertexAttribPointerType type,
                          Boolean normalized, GLuint value) const;
    void vertexAttribP1UIV(GLuint index, VertexAttribPointerType type,
                           Boolean normalized, const GLuint *value) const;
    void vertexAttribP2UI(GLuint index, VertexAttribPointerType type,
                          Boolean normalized, GLuint value) const;
    void vertexAttribP2UIV(GLuint index, VertexAttribPointerType type,
                           Boolean normalized, const GLuint *value) const;
    void vertexAttribP3UI(GLuint index, VertexAttribPointerType type,
                          Boolean normalized, GLuint value) const;
    void vertexAttribP3UIV(GLuint index, VertexAttribPointerType type,
                           Boolean normalized, const GLuint *value) const;
    void vertexAttribP4UI(GLuint index, VertexAttribPointerType type,
                          Boolean normalized, GLuint value) const;
    void vertexAttribP4UIV(GLuint index, VertexAttribPointerType type,
                           Boolean normalized, const GLuint *value) const;

    // GL 4.0
    void beginQueryIndexed(QueryTarget target, GLuint index, QueryName id) const;
    void bindTransformFeedback(BindTransformFeedbackTarget target,
                               TransformFeedbackName id) const;
    void blendEquationSeparateI(GLuint buf, BlendEquationModeEXT modeRGB,
                                BlendEquationModeEXT modeAlpha) const;
    void blendEquationI(GLuint buf, BlendEquationModeEXT mode) const;
    void blendFuncSeparateI(GLuint buf, BlendingFactor srcRGB,
                            BlendingFactor dstRGB, BlendingFactor srcAlpha,
                            BlendingFactor dstAlpha) const;
    void blendFuncI(GLuint buf, BlendingFactor src, BlendingFactor dst) const;
    void deleteTransformFeedbacks(GLsizei n, const TransformFeedbackName *ids) const;
    void drawArraysIndirect(PrimitiveType mode, const void *indirect) const;
    void drawElementsIndirect(PrimitiveType mode, DrawElementsType type,
                              const void *indirect) const;
    void drawTransformFeedback(PrimitiveType mode, TransformFeedbackName id) const;
    void drawTransformFeedbackStream(PrimitiveType mode,
                                     TransformFeedbackName id, GLuint stream) const;
    void endQueryIndexed(QueryTarget target, GLuint index) const;
    void genTransformFeedbacks(GLsizei n, TransformFeedbackName *ids) const;
    void getActiveSubroutineName(ProgramName program, ShaderType shadertype,
                                 GLuint index, GLsizei bufSize, GLsizei *length,
                                 GLchar *name) const;
    void getActiveSubroutineUniformName(ProgramName program,
                                        ShaderType shadertype, GLuint index,
                                        GLsizei bufSize, GLsizei *length,
                                        GLchar *name) const;
    void getActiveSubroutineUniformIV(ProgramName program, ShaderType shadertype,
                                      GLuint index,
                                      SubroutineParameterName pname,
                                      GLint *values) const;
    void getProgramStageIV(ProgramName program, ShaderType shadertype,
                           ProgramStagePName pname, GLint *values) const;
    void getQueryIndexedIV(QueryTarget target, GLuint index,
                           QueryParameterName pname, GLint *params) const;
    GLuint getSubroutineIndex(ProgramName program, ShaderType shadertype,
                              const GLchar *name) const;
    GLint getSubroutineUniformLocation(ProgramName program,
                                       ShaderType shadertype, const GLchar *name) const;
    void getUniformSubroutineUIV(ShaderType shadertype, GLint location,
                                 GLuint *params) const;
    void getUniformDV(ProgramName program, GLint location, GLdouble *params) const;
    Boolean isTransformFeedback(TransformFeedbackName id) const;
    void minSampleShading(GLfloat value) const;
    void patchParameterFV(PatchParameterName pname, const GLfloat *values) const;
    void patchParameterI(PatchParameterName pname, GLint value) const;
    void pauseTransformFeedback() const;
    void resumeTransformFeedback() const;
    void uniform1D(GLint location, GLdouble x) const;
    void uniform1DV(GLint location, GLsizei count, const GLdouble *value) const;
    void uniform2D(GLint location, GLdouble x, GLdouble y) const;
    void uniform2DV(GLint location, GLsizei count, const GLdouble *value) const;
    void uniform3D(GLint location, GLdouble x, GLdouble y, GLdouble z) const;
    void uniform3DV(GLint location, GLsizei count, const GLdouble *value) const;
    void uniform4D(GLint location, GLdouble x, GLdouble y, GLdouble z,
                   GLdouble w) const;
    void uniform4DV(GLint location, GLsizei count, const GLdouble *value) const;
    void uniformMatrix2DV(GLint location, GLsizei count, Boolean transpose,
                          const GLdouble *value) const;
    void uniformMatrix2x3DV(GLint location, GLsizei count, Boolean transpose,
                            const GLdouble *value) const;
    void uniformMatrix2x4DV(GLint location, GLsizei count, Boolean transpose,
                            const GLdouble *value) const;
    void uniformMatrix3DV(GLint location, GLsizei count, Boolean transpose,
                          const GLdouble *value) const;
    void uniformMatrix3x2DV(GLint location, GLsizei count, Boolean transpose,
                            const GLdouble *value) const;
    void uniformMatrix3x4DV(GLint location, GLsizei count, Boolean transpose,
                            const GLdouble *value) const;
    void uniformMatrix4DV(GLint location, GLsizei count, Boolean transpose,
                          const GLdouble *value) const;
    void uniformMatrix4x2DV(GLint location, GLsizei count, Boolean transpose,
                            const GLdouble *value) const;
    void uniformMatrix4x3DV(GLint location, GLsizei count, Boolean transpose,
                            const GLdouble *value) const;
    void uniformSubroutinesUIV(ShaderType shadertype, GLsizei count,
                               const GLuint *indices) const;

    // GL 4.1
    void activeShaderProgram(ProgramPipelineName pipeline, ProgramName program) const;
    void bindProgramPipeline(ProgramPipelineName pipeline) const;
    void clearDepthF(GLfloat d) const;
    ProgramName createShaderProgramV(ShaderType type, GLsizei count,
                                     const GLchar *const*strings) const;
    void deleteProgramPipelines(GLsizei n, const ProgramPipelineName *pipelines) const;
    void depthRangeArrayV(GLuint first, GLsizei count, const GLdouble *v) const;
    void depthRangeIndexed(GLuint index, GLdouble n, GLdouble f) const;
    void depthRangeF(GLfloat n, GLfloat f) const;
    void genProgramPipelines(GLsizei n, ProgramPipelineName *pipelines) const;
    void getDoubleIV(GetPName target, GLuint index, GLdouble *data) const;
    void getFloatIV(GetPName target, GLuint index, GLfloat *data) const;
    void getProgramBinary(ProgramName program, GLsizei bufSize, GLsizei *length,
                          GLenum *binaryFormat, void *binary) const;
    void getProgramPipelineInfoLog(ProgramPipelineName pipeline, GLsizei bufSize,
                                   GLsizei *length, GLchar *infoLog) const;
    void getProgramPipelineIV(ProgramPipelineName pipeline,
                              PipelineParameterName pname, GLint *params) const;
    void getShaderPrecisionFormat(ShaderType shadertype,
                                  PrecisionType precisiontype, GLint *range,
                                  GLint *precision) const;
    void getVertexAttribLDV(GLuint index, VertexAttribEnum pname,
                            GLdouble *params) const;
    Boolean isProgramPipeline(ProgramPipelineName pipeline) const;
    void programBinary(ProgramName program, GLenum binaryFormat,
                       const void *binary, GLsizei length) const;
    void programParameterI(ProgramName program, ProgramParameterPName pname,
                           GLint value) const;
    void programUniform1D(ProgramName program, GLint location, GLdouble v0) const;
    void programUniform1DV(ProgramName program, GLint location, GLsizei count,
                           const GLdouble *value) const;
    void programUniform1F(ProgramName program, GLint location, GLfloat v0) const;
    void programUniform1FV(ProgramName program, GLint location, GLsizei count,
                           const GLfloat *value) const;
    void programUniform1I(ProgramName program, GLint location, GLint v0) const;
    void programUniform1IV(ProgramName program, GLint location, GLsizei count,
                           const GLint *value) const;
    void programUniform1UI(ProgramName program, GLint location, GLuint v0) const;
    void programUniform1UIV(ProgramName program, GLint location, GLsizei count,
                            const GLuint *value) const;
    void programUniform2D(ProgramName program, GLint location, GLdouble v0,
                          GLdouble v1) const;
    void programUniform2DV(ProgramName program, GLint location, GLsizei count,
                           const GLdouble *value) const;
    void programUniform2F(ProgramName program, GLint location, GLfloat v0,
                          GLfloat v1) const;
    void programUniform2FV(ProgramName program, GLint location, GLsizei count,
                           const GLfloat *value) const;
    void programUniform2I(ProgramName program, GLint location, GLint v0,
                          GLint v1) const;
    void programUniform2IV(ProgramName program, GLint location, GLsizei count,
                           const GLint *value) const;
    void programUniform2UI(ProgramName program, GLint location, GLuint v0,
                           GLuint v1) const;
    void programUniform2UIV(ProgramName program, GLint location, GLsizei count,
                            const GLuint *value) const;
    void programUniform3D(ProgramName program, GLint location, GLdouble v0,
                          GLdouble v1, GLdouble v2) const;
    void programUniform3DV(ProgramName program, GLint location, GLsizei count,
                           const GLdouble *value) const;
    void programUniform3F(ProgramName program, GLint location, GLfloat v0,
                          GLfloat v1, GLfloat v2) const;
    void programUniform3FV(ProgramName program, GLint location, GLsizei count,
                           const GLfloat *value) const;
    void programUniform3I(ProgramName program, GLint location, GLint v0,
                          GLint v1, GLint v2) const;
    void programUniform3IV(ProgramName program, GLint location, GLsizei count,
                           const GLint *value) const;
    void programUniform3UI(ProgramName program, GLint location, GLuint v0,
                           GLuint v1, GLuint v2) const;
    void programUniform3UIV(ProgramName program, GLint location, GLsizei count,
                            const GLuint *value) const;
    void programUniform4D(ProgramName program, GLint location, GLdouble v0,
                          GLdouble v1, GLdouble v2, GLdouble v3) const;
    void programUniform4DV(ProgramName program, GLint location, GLsizei count,
                           const GLdouble *value) const;
    void programUniform4F(ProgramName program, GLint location, GLfloat v0,
                          GLfloat v1, GLfloat v2, GLfloat v3) const;
    void programUniform4FV(ProgramName program, GLint location, GLsizei count,
                           const GLfloat *value) const;
    void programUniform4I(ProgramName program, GLint location, GLint v0,
                          GLint v1, GLint v2, GLint v3) const;
    void programUniform4IV(ProgramName program, GLint location, GLsizei count,
                           const GLint *value) const;
    void programUniform4UI(ProgramName program, GLint location, GLuint v0,
                           GLuint v1, GLuint v2, GLuint v3) const;
    void programUniform4UIV(ProgramName program, GLint location, GLsizei count,
                            const GLuint *value) const;
    void programUniformMatrix2DV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLdouble *value) const;
    void programUniformMatrix2FV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLfloat *value) const;
    void programUniformMatrix2x3DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix2x3FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix2x4DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix2x4FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix3DV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLdouble *value) const;
    void programUniformMatrix3FV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLfloat *value) const;
    void programUniformMatrix3x2DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix3x2FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix3x4DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix3x4FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix4DV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLdouble *value) const;
    void programUniformMatrix4FV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLfloat *value) const;
    void programUniformMatrix4x2DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix4x2FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix4x3DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix4x3FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void releaseShaderCompiler() const;
    void scissorArrayV(GLuint first, GLsizei count, const GLint *v) const;
    void scissorIndexed(GLuint index, GLint left, GLint bottom, GLsizei width,
                        GLsizei height) const;
    void scissorIndexedv(GLuint index, const GLint *v) const;
    void shaderBinary(GLsizei count, const ShaderName *shaders,
                      ShaderBinaryFormat binaryFormat, const void *binary,
                      GLsizei length) const;
    void useProgramStages(ProgramPipelineName pipeline,
                          GLbitfield  /* UseProgramStageMask */ stages,
                          ProgramName program) const;
    void validateProgramPipeline(ProgramPipelineName pipeline) const;
    void vertexAttribL1D(GLuint index, GLdouble x) const;
    void vertexAttribL1DV(GLuint index, const GLdouble *v) const;
    void vertexAttribL2D(GLuint index, GLdouble x, GLdouble y) const;
    void vertexAttribL2DV(GLuint index, const GLdouble *v) const;
    void vertexAttribL3D(GLuint index, GLdouble x, GLdouble y, GLdouble z) const;
    void vertexAttribL3DV(GLuint index, const GLdouble *v) const;
    void vertexAttribL4D(GLuint index, GLdouble x, GLdouble y, GLdouble z,
                         GLdouble w) const;
    void vertexAttribL4DV(GLuint index, const GLdouble *v) const;
    void vertexAttribLPointer(GLuint index, GLint size, VertexAttribLType type,
                              GLsizei stride, const void *pointer) const;
    void viewportArrayV(GLuint first, GLsizei count, const GLfloat *v) const;
    void viewportIndexedF(GLuint index, GLfloat x, GLfloat y, GLfloat w,
                          GLfloat h) const;
    void viewportIndexedFV(GLuint index, const GLfloat *v) const;

    // GL 4.2
    void bindImageTexture(GLuint unit, TextureName texture, GLint level,
                          Boolean layered, GLint layer, BufferAccessARB access,
                          InternalFormat format) const;
    void drawArraysInstancedBaseInstance(PrimitiveType mode, GLint first,
                                         GLsizei count, GLsizei instancecount,
                                         GLuint baseinstance) const;
    void drawElementsInstancedBaseInstance(PrimitiveType mode, GLsizei count,
                                           PrimitiveType type,
                                           const void *indices,
                                           GLsizei instancecount,
                                           GLuint baseinstance) const;
    void drawElementsInstancedBaseVertexBaseInstance(PrimitiveType mode,
                                                     GLsizei count,
                                                     DrawElementsType type,
                                                     const void *indices,
                                                     GLsizei instancecount,
                                                     GLint basevertex,
                                                     GLuint baseinstance) const;
    void drawTransformFeedbackInstanced(PrimitiveType mode,
                                        TransformFeedbackName id,
                                        GLsizei instancecount) const;
    void drawTransformFeedbackStreamInstanced(PrimitiveType mode,
                                              TransformFeedbackName id,
                                              GLuint stream,
                                              GLsizei instancecount) const;
    void getActiveAtomicCounterBufferIV(ProgramName program, GLuint bufferIndex,
                                        AtomicCounterBufferPName pname,
                                        GLint *params) const;
    void getInternalformatIV(TextureTarget target, InternalFormat internalformat,
                             InternalFormatPName pname, GLsizei count,
                             GLint *params) const;
    void memoryBarrier(GLbitfield  /* MemoryBarrierMask */ barriers) const;
    void texStorage1D(TextureTarget target, GLsizei levels,
                      SizedInternalFormat internalformat, GLsizei width) const;
    void texStorage2D(TextureTarget target, GLsizei levels,
                      SizedInternalFormat internalformat, GLsizei width,
                      GLsizei height) const;
    void texStorage3D(TextureTarget target, GLsizei levels,
                      SizedInternalFormat internalformat, GLsizei width,
                      GLsizei height, GLsizei depth) const;

    // GL 4.3
    void bindVertexBuffer(GLuint bindingindex, BufferName buffer,
                          GLintptr offset, GLsizei stride) const;
    void clearBufferData(BufferStorageTarget target,
                         SizedInternalFormat internalformat, PixelFormat format,
                         PixelType type, const void *data) const;
    void clearBufferSubData(BufferTarget target,
                            SizedInternalFormat internalformat, GLintptr offset,
                            GLsizeiptr size, PixelFormat format, PixelType type,
                            const void *data) const;
    void copyImageSubData(GLuint srcName, CopyImageSubDataTarget srcTarget,
                          GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
                          GLuint dstName, CopyImageSubDataTarget dstTarget,
                          GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ,
                          GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth) const;
    void debugMessageCallback(GLDEBUGPROC callback, const void *userParam) const;
    void debugMessageControl(DebugSource source, DebugType type,
                             DebugSeverity severity, GLsizei count,
                             const GLuint *ids, Boolean enabled) const;
    void debugMessageInsert(DebugSource source, DebugType type, GLuint id,
                            DebugSeverity severity, GLsizei length,
                            const GLchar *buf) const;
    void dispatchCompute(GLuint num_groups_x, GLuint num_groups_y,
                         GLuint num_groups_z) const;
    void dispatchComputeIndirect(GLintptr indirect) const;
    void framebufferParameterI(FramebufferTarget target,
                               FramebufferParameterName pname, GLint param) const;
    GLuint getDebugMessageLog(GLuint count, GLsizei bufSize,
                              DebugSource *sources, DebugType *types,
                              GLuint *ids, DebugSeverity *severities,
                              GLsizei *lengths, GLchar *messageLog) const;
    void getFramebufferParameterIV(FramebufferTarget target,
                                   FramebufferAttachmentParameterName pname,
                                   GLint *params) const;
    void getInternalformatI64V(TextureTarget target,
                               InternalFormat internalformat,
                               InternalFormatPName pname, GLsizei count,
                               GLint64 *params) const;
    void getObjectLabel(ObjectIdentifier identifier, GLuint name,
                        GLsizei bufSize, GLsizei *length, GLchar *label) const;
    void getObjectPtrLabel(const void *ptr, GLsizei bufSize, GLsizei *length,
                           GLchar *label) const;
    void getPointerV(GetPointervPName pname, void **params) const;
    void getProgramInterfaceIV(ProgramName program,
                               ProgramInterface programInterface,
                               ProgramInterfacePName pname, GLint *params) const;
    GLuint getProgramResourceIndex(ProgramName program,
                                   ProgramInterface programInterface,
                                   const GLchar *name) const;
    GLint getProgramResourceLocation(ProgramName program,
                                     ProgramInterface programInterface,
                                     const GLchar *name) const;
    GLint getProgramResourceLocationIndex(ProgramName program,
                                          ProgramInterface programInterface,
                                          const GLchar *name) const;
    void getProgramResourceName(ProgramName program,
                                ProgramInterface programInterface, GLuint index,
                                GLsizei bufSize, GLsizei *length, GLchar *name) const;
    void getProgramResourceIV(ProgramName program,
                              ProgramInterface programInterface, GLuint index,
                              GLsizei propCount,
                              const ProgramResourceProperty *props,
                              GLsizei count, GLsizei *length, GLint *params) const;
    void invalidateBufferData(BufferName buffer) const;
    void invalidateBufferSubData(BufferName buffer, GLintptr offset,
                                 GLsizeiptr length) const;
    void invalidateFramebuffer(FramebufferTarget target, GLsizei numAttachments,
                               const InvalidateFramebufferAttachment *attachments) const;
    void invalidateSubFramebuffer(FramebufferTarget target,
                                  GLsizei numAttachments,
                                  const InvalidateFramebufferAttachment *attachments,
                                  GLint x, GLint y, GLsizei width,
                                  GLsizei height) const;
    void invalidateTexImage(TextureName texture, GLint level) const;
    void invalidateTexSubImage(TextureName texture, GLint level, GLint xoffset,
                               GLint yoffset, GLint zoffset, GLsizei width,
                               GLsizei height, GLsizei depth) const;
    void multiDrawArraysIndirect(PrimitiveType mode, const void *indirect,
                                 GLsizei drawcount, GLsizei stride) const;
    void multiDrawElementsIndirect(PrimitiveType mode, DrawElementsType type,
                                   const void *indirect, GLsizei drawcount,
                                   GLsizei stride) const;
    void objectLabel(ObjectIdentifier identifier, GLuint name, GLsizei length,
                     const GLchar *label) const;
    void objectPtrLabel(const void *ptr, GLsizei length, const GLchar *label) const;
    void popDebugGroup() const;
    void pushDebugGroup(DebugSource source, GLuint id, GLsizei length,
                        const GLchar *message) const;
    void shaderStorageBlockBinding(ProgramName program, GLuint storageBlockIndex,
                                   GLuint storageBlockBinding) const;
    void texBufferRange(TextureTarget target, SizedInternalFormat internalformat,
                        BufferName buffer, GLintptr offset, GLsizeiptr size) const;
    void texStorage2DMultisample(TextureTarget target, GLsizei samples,
                                 SizedInternalFormat internalformat,
                                 GLsizei width, GLsizei height,
                                 Boolean fixedsamplelocations) const;
    void texStorage3DMultisample(TextureTarget target, GLsizei samples,
                                 SizedInternalFormat internalformat,
                                 GLsizei width, GLsizei height, GLsizei depth,
                                 Boolean fixedsamplelocations) const;
    void textureView(TextureName texture, TextureTarget target,
                     TextureName origtexture, SizedInternalFormat internalformat,
                     GLuint minlevel, GLuint numlevels, GLuint minlayer,
                     GLuint numlayers) const;
    void vertexAttribBinding(GLuint attribindex, GLuint bindingindex) const;
    void vertexAttribFormat(GLuint attribindex, GLint size,
                            VertexAttribType type, Boolean normalized,
                            GLuint relativeoffset) const;
    void vertexAttribIFormat(GLuint attribindex, GLint size,
                             VertexAttribIType type, GLuint relativeoffset) const;
    void vertexAttribLFormat(GLuint attribindex, GLint size,
                             VertexAttribLType type, GLuint relativeoffset) const;
    void vertexBindingDivisor(GLuint bindingindex, GLuint divisor) const;

    // GL 4.4
    void bindBuffersBase(BufferTarget target, GLuint first, GLsizei count,
                         const BufferName *buffers) const;
    void bindBuffersRange(BufferTarget target, GLuint first, GLsizei count,
                          const BufferName *buffers, const GLintptr *offsets,
                          const GLsizeiptr *sizes) const;
    void bindImageTextures(GLuint first, GLsizei count,
                           const TextureName *textures) const;
    void bindSamplers(GLuint first, GLsizei count, const SamplerName *samplers) const;
    void bindTextures(GLuint first, GLsizei count, const TextureName *textures) const;
    void bindVertexBuffers(GLuint first, GLsizei count,
                           const BufferName *buffers, const GLintptr *offsets,
                           const GLsizei *strides) const;
    void bufferStorage(BufferStorageTarget target, GLsizeiptr size,
                       const void *data,
                       GLbitfield  /* BufferStorageMask */ flags) const;
    void clearTexImage(TextureName texture, GLint level, PixelFormat format,
                       PixelType type, const void *data) const;
    void clearTexSubImage(TextureName texture, GLint level, GLint xoffset,
                          GLint yoffset, GLint zoffset, GLsizei width,
                          GLsizei height, GLsizei depth, PixelFormat format,
                          PixelType type, const void *data) const;

    // GL 4.5
    void bindTextureUnit(GLuint unit, TextureName texture) const;
    void blitNamedFramebuffer(FrameBufferName readFramebuffer,
                              FrameBufferName drawFramebuffer, GLint srcX0,
                              GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0,
                              GLint dstY0, GLint dstX1, GLint dstY1,
                              GLbitfield  /* ClearBufferMask */ mask,
                              BlitFramebufferFilter filter) const;
    FramebufferStatus checkNamedFramebufferStatus(FrameBufferName framebuffer,
                                                  FramebufferTarget target) const;
    void clearNamedBufferData(BufferName buffer,
                              SizedInternalFormat internalformat,
                              PixelFormat format, PixelType type,
                              const void *data) const;
    void clearNamedBufferSubData(BufferName buffer,
                                 SizedInternalFormat internalformat,
                                 GLintptr offset, GLsizeiptr size,
                                 PixelFormat format, PixelType type,
                                 const void *data) const;
    void clearNamedFramebufferFI(FrameBufferName framebuffer, BufferEnum buffer,
                                 GLint drawbuffer, GLfloat depth, GLint stencil) const;
    void clearNamedFramebufferFV(FrameBufferName framebuffer, BufferEnum buffer,
                                 GLint drawbuffer, const GLfloat *value) const;
    void clearNamedFramebufferIV(FrameBufferName framebuffer, BufferEnum buffer,
                                 GLint drawbuffer, const GLint *value) const;
    void clearNamedFramebufferUIV(FrameBufferName framebuffer, BufferEnum buffer,
                                  GLint drawbuffer, const GLuint *value) const;
    void clipControl(ClipControlOrigin origin, ClipControlDepth depth) const;
    void compressedTextureSubImage1D(TextureName texture, GLint level,
                                     GLint xoffset, GLsizei width,
                                     InternalFormat format, GLsizei imageSize,
                                     const void *data) const;
    void compressedTextureSubImage2D(TextureName texture, GLint level,
                                     GLint xoffset, GLint yoffset, GLsizei width,
                                     GLsizei height, InternalFormat format,
                                     GLsizei imageSize, const void *data) const;
    void compressedTextureSubImage3D(TextureName texture, GLint level,
                                     GLint xoffset, GLint yoffset, GLint zoffset,
                                     GLsizei width, GLsizei height,
                                     GLsizei depth, InternalFormat format,
                                     GLsizei imageSize, const void *data) const;
    void copyNamedBufferSubData(BufferName readBuffer, BufferName writeBuffer,
                                GLintptr readOffset, GLintptr writeOffset,
                                GLsizeiptr size) const;
    void copyTextureSubImage1D(TextureName texture, GLint level, GLint xoffset,
                               GLint x, GLint y, GLsizei width) const;
    void copyTextureSubImage2D(TextureName texture, GLint level, GLint xoffset,
                               GLint yoffset, GLint x, GLint y, GLsizei width,
                               GLsizei height) const;
    void copyTextureSubImage3D(TextureName texture, GLint level, GLint xoffset,
                               GLint yoffset, GLint zoffset, GLint x, GLint y,
                               GLsizei width, GLsizei height) const;
    void createBuffers(GLsizei n, BufferName *buffers) const;
    void createFramebuffers(GLsizei n, FrameBufferName *framebuffers) const;
    void createProgramPipelines(GLsizei n, ProgramPipelineName *pipelines) const;
    void createQueries(QueryTarget target, GLsizei n, QueryName *ids) const;
    void createRenderbuffers(GLsizei n, RenderBufferName *renderbuffers) const;
    void createSamplers(GLsizei n, SamplerName *samplers) const;
    void createTextures(TextureTarget target, GLsizei n, TextureName *textures) const;
    void createTransformFeedbacks(GLsizei n, TransformFeedbackName *ids) const;
    void createVertexArrays(GLsizei n, VertexArrayName *arrays) const;
    void disableVertexArrayAttrib(VertexArrayName vaobj, GLuint index) const;
    void enableVertexArrayAttrib(VertexArrayName vaobj, GLuint index) const;
    void flushMappedNamedBufferRange(BufferName buffer, GLintptr offset,
                                     GLsizeiptr length) const;
    void generateTextureMipmap(TextureName texture) const;
    void getCompressedTextureImage(TextureName texture, GLint level,
                                   GLsizei bufSize, void *pixels) const;
    void getCompressedTextureSubImage(TextureName texture, GLint level,
                                      GLint xoffset, GLint yoffset,
                                      GLint zoffset, GLsizei width,
                                      GLsizei height, GLsizei depth,
                                      GLsizei bufSize, void *pixels) const;
    GraphicsResetStatus getGraphicsResetStatus() const;
    void getNamedBufferParameterI64V(BufferName buffer, BufferPNameARB pname,
                                     GLint64 *params) const;
    void getNamedBufferParameterIV(BufferName buffer, BufferPNameARB pname,
                                   GLint *params) const;
    void getNamedBufferPointerV(BufferName buffer, BufferPointerNameARB pname,
                                void **params) const;
    void getNamedBufferSubData(BufferName buffer, GLintptr offset,
                               GLsizeiptr size, void *data) const;
    void getNamedFramebufferAttachmentParameterIV(FrameBufferName framebuffer,
                                                  FramebufferAttachment attachment,
                                                  FramebufferAttachmentParameterName pname,
                                                  GLint *params) const;
    void getNamedFramebufferParameterIV(FrameBufferName framebuffer,
                                        GetFramebufferParameter pname,
                                        GLint *param) const;
    void getNamedRenderbufferParameterIV(RenderBufferName renderbuffer,
                                         RenderBufferParameterName pname,
                                         GLint *params) const;
    void getQueryBufferObjectI64V(QueryName id, BufferName buffer,
                                  QueryObjectParameterName pname,
                                  GLintptr offset) const;
    void getQueryBufferObjectIV(QueryName id, BufferName buffer,
                                QueryObjectParameterName pname, GLintptr offset) const;
    void getQueryBufferObjectUI64(QueryName id, BufferName buffer,
                                  QueryObjectParameterName pname,
                                  GLintptr offset) const;
    void getQueryBufferObjectUIV(QueryName id, BufferName buffer,
                                 QueryObjectParameterName pname, GLintptr offset) const;
    void getTextureImage(TextureName texture, GLint level, PixelFormat format,
                         PixelType type, GLsizei bufSize, void *pixels) const;
    void getTextureLevelParameterFV(TextureName texture, GLint level,
                                    GetTextureParameter pname, GLfloat *params) const;
    void getTextureLevelParameterIV(TextureName texture, GLint level,
                                    GetTextureParameter pname, GLint *params) const;
    void getTextureParameterIIV(TextureName texture, GetTextureParameter pname,
                                GLint *params) const;
    void getTextureParameterIUIV(TextureName texture, GetTextureParameter pname,
                                 GLuint *params) const;
    void getTextureParameterFV(TextureName texture, GetTextureParameter pname,
                               GLfloat *params) const;
    void getTextureParameterIV(TextureName texture, GetTextureParameter pname,
                               GLint *params) const;
    void getTextureSubImage(TextureName texture, GLint level, GLint xoffset,
                            GLint yoffset, GLint zoffset, GLsizei width,
                            GLsizei height, GLsizei depth, PixelFormat format,
                            PixelType type, GLsizei bufSize, void *pixels) const;
    void getTransformFeedbackI64(TransformFeedbackName xfb,
                                 TransformFeedbackPName pname, GLuint index,
                                 GLint64 *param) const;
    void getTransformFeedbackIV(TransformFeedbackName xfb,
                                TransformFeedbackPName pname, GLuint index,
                                GLint *param) const;
    void getTransformFeedbackIV(TransformFeedbackName xfb,
                                TransformFeedbackPName pname, GLint *param) const;
    void getVertexArrayIndexed64IV(VertexArrayName vaobj, GLuint index,
                                   VertexArrayPName pname, GLint64 *param) const;
    void getVertexArrayIndexedIV(VertexArrayName vaobj, GLuint index,
                                 VertexArrayPName pname, GLint *param) const;
    void getVertexArrayIV(VertexArrayName vaobj, VertexArrayPName pname,
                          GLint *param) const;
    void getnCompressedTexImage(TextureTarget target, GLint lod, GLsizei bufSize,
                                void *pixels) const;
    void getnTexImage(TextureTarget target, GLint level, PixelFormat format,
                      PixelType type, GLsizei bufSize, void *pixels) const;
    void getnUniformDV(ProgramName program, GLint location, GLsizei bufSize,
                       GLdouble *params) const;
    void getnUniformFV(ProgramName program, GLint location, GLsizei bufSize,
                       GLfloat *params) const;
    void getnUniformIV(ProgramName program, GLint location, GLsizei bufSize,
                       GLint *params) const;
    void getnUniformUIV(ProgramName program, GLint location, GLsizei bufSize,
                        GLuint *params) const;
    void invalidateNamedFramebufferData(FrameBufferName framebuffer,
                                        GLsizei numAttachments,
                                        const FramebufferAttachment *attachments) const;
    void invalidateNamedFramebufferSubData(FrameBufferName framebuffer,
                                           GLsizei numAttachments,
                                           const FramebufferAttachment *attachments,
                                           GLint x, GLint y, GLsizei width,
                                           GLsizei height) const;
    void *mapNamedBuffer(BufferName buffer, BufferAccessARB access) const;
    void *mapNamedBufferRange(BufferName buffer, GLintptr offset,
                              GLsizeiptr length,
                              GLbitfield  /* MapBufferAccessMask */ access) const;
    void memoryBarrierByRegion(GLbitfield  /* MemoryBarrierMask */ barriers) const;
    void namedBufferData(BufferName buffer, GLsizeiptr size, const void *data,
                         VertexBufferObjectUsage usage) const;
    void namedBufferStorage(BufferName buffer, GLsizeiptr size, const void *data,
                            GLbitfield  /* BufferStorageMask */ flags) const;
    void namedBufferSubData(BufferName buffer, GLintptr offset, GLsizeiptr size,
                            const void *data) const;
    void namedFramebufferDrawBuffer(FrameBufferName framebuffer, ColorBuffer buf) const;
    void namedFramebufferDrawBuffers(FrameBufferName framebuffer, GLsizei n,
                                     const ColorBuffer *bufs) const;
    void namedFramebufferParameterI(FrameBufferName framebuffer,
                                    FramebufferParameterName pname, GLint param) const;
    void namedFramebufferReadBuffer(FrameBufferName framebuffer, ColorBuffer src) const;
    void namedFramebufferRenderbuffer(FrameBufferName framebuffer,
                                      FramebufferAttachment attachment,
                                      RenderBufferTarget renderbuffertarget,
                                      RenderBufferName renderbuffer) const;
    void namedFramebufferTexture(FrameBufferName framebuffer,
                                 FramebufferAttachment attachment,
                                 TextureName texture, GLint level) const;
    void namedFramebufferTextureLayer(FrameBufferName framebuffer,
                                      FramebufferAttachment attachment,
                                      TextureName texture, GLint level,
                                      GLint layer) const;
    void namedRenderbufferStorage(RenderBufferName renderbuffer,
                                  InternalFormat internalformat, GLsizei width,
                                  GLsizei height) const;
    void namedRenderbufferStorageMultisample(RenderBufferName renderbuffer,
                                             GLsizei samples,
                                             InternalFormat internalformat,
                                             GLsizei width, GLsizei height) const;
    void readnPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                     PixelFormat format, PixelType type, GLsizei bufSize,
                     void *data) const;
    void textureBarrier() const;
    void textureBuffer(TextureName texture, SizedInternalFormat internalformat,
                       BufferName buffer) const;
    void textureBufferRange(TextureName texture,
                            SizedInternalFormat internalformat,
                            BufferName buffer, GLintptr offset, GLsizeiptr size) const;
    void textureParameterIIV(TextureName texture, TextureParameterName pname,
                             const GLint *params) const;
    void textureParameterIUIV(TextureName texture, TextureParameterName pname,
                              const GLuint *params) const;
    void textureParameterF(TextureName texture, TextureParameterName pname,
                           GLfloat param) const;
    void textureParameterFV(TextureName texture, TextureParameterName pname,
                            const GLfloat *param) const;
    void textureParameterI(TextureName texture, TextureParameterName pname,
                           GLint param) const;
    void textureParameterIV(TextureName texture, TextureParameterName pname,
                            const GLint *param) const;
    void textureStorage1D(TextureName texture, GLsizei levels,
                          SizedInternalFormat internalformat, GLsizei width) const;
    void textureStorage2D(TextureName texture, GLsizei levels,
                          SizedInternalFormat internalformat, GLsizei width,
                          GLsizei height) const;
    void textureStorage2DMultisample(TextureName texture, GLsizei samples,
                                     SizedInternalFormat internalformat,
                                     GLsizei width, GLsizei height,
                                     Boolean fixedsamplelocations) const;
    void textureStorage3D(TextureName texture, GLsizei levels,
                          SizedInternalFormat internalformat, GLsizei width,
                          GLsizei height, GLsizei depth) const;
    void textureStorage3DMultisample(TextureName texture, GLsizei samples,
                                     SizedInternalFormat internalformat,
                                     GLsizei width, GLsizei height,
                                     GLsizei depth, Boolean fixedsamplelocations) const;
    void textureSubImage1D(TextureName texture, GLint level, GLint xoffset,
                           GLsizei width, PixelFormat format, PixelType type,
                           const void *pixels) const;
    void textureSubImage2D(TextureName texture, GLint level, GLint xoffset,
                           GLint yoffset, GLsizei width, GLsizei height,
                           PixelFormat format, PixelType type,
                           const void *pixels) const;
    void textureSubImage3D(TextureName texture, GLint level, GLint xoffset,
                           GLint yoffset, GLint zoffset, GLsizei width,
                           GLsizei height, GLsizei depth, PixelFormat format,
                           PixelType type, const void *pixels) const;
    void transformFeedbackBufferBase(TransformFeedbackName xfb, GLuint index,
                                     BufferName buffer) const;
    void transformFeedbackBufferRange(TransformFeedbackName xfb, GLuint index,
                                      BufferName buffer, GLintptr offset,
                                      GLsizeiptr size) const;
    Boolean unmapNamedBuffer(BufferName buffer) const;
    void vertexArrayAttribBinding(VertexArrayName vaobj, GLuint attribindex,
                                  GLuint bindingindex) const;
    void vertexArrayAttribFormat(VertexArrayName vaobj, GLuint attribindex,
                                 GLint size, VertexAttribType type,
                                 Boolean normalized, GLuint relativeoffset) const;
    void vertexArrayAttribIFormat(VertexArrayName vaobj, GLuint attribindex,
                                  GLint size, VertexAttribIType type,
                                  GLuint relativeoffset) const;
    void vertexArrayAttribLFormat(VertexArrayName vaobj, GLuint attribindex,
                                  GLint size, VertexAttribLType type,
                                  GLuint relativeoffset) const;
    void vertexArrayBindingDivisor(VertexArrayName vaobj, GLuint bindingindex,
                                   GLuint divisor) const;
    void vertexArrayElementBuffer(VertexArrayName vaobj, BufferName buffer) const;
    void vertexArrayVertexBuffer(VertexArrayName vaobj, GLuint bindingindex,
                                 BufferName buffer, GLintptr offset,
                                 GLsizei stride) const;
    void vertexArrayVertexBuffers(VertexArrayName vaobj, GLuint first,
                                  GLsizei count, const BufferName *buffers,
                                  const GLintptr *offsets,
                                  const GLsizei *strides) const;

    // GL 4.6
    void multiDrawArraysIndirectCount(PrimitiveType mode, const void *indirect,
                                      GLintptr drawcount, GLsizei maxdrawcount,
                                      GLsizei stride) const;
    void multiDrawElementsIndirectCount(PrimitiveType mode,
                                        DrawElementsType type,
                                        const void *indirect, GLintptr drawcount,
                                        GLsizei maxdrawcount, GLsizei stride) const;
    void polygonOffsetClamp(GLfloat factor, GLfloat units, GLfloat clamp) const;
    void specializeShader(ShaderName shader, const GLchar *pEntryPoint,
                          GLuint numSpecializationConstants,
                          const GLuint *pConstantIndex,
                          const GLuint *pConstantValue) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    Ag::Version _version;
    GL _api;
};

// Manages the entry points for the GL_3DFX_tbuffer extension API.
class _3DFXTbufferAPI : public BaseExtension
{
public:
    // Construction
    _3DFXTbufferAPI();

    // Accessors
    const _3DFXTbuffer &getRawAPI() const;

    // Operations
    void tbufferMask(GLuint mask) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    _3DFXTbuffer _api;
};

// Manages the entry points for the GL_AMD_debug_output extension API.
class AMDDebugOutputAPI : public BaseExtension
{
public:
    // Construction
    AMDDebugOutputAPI();

    // Accessors
    const AMDDebugOutput &getRawAPI() const;

    // Operations
    void debugMessageCallback(GLDEBUGPROCAMD callback, void *userParam) const;
    void debugMessageEnable(GLenum category, DebugSeverity severity,
                            GLsizei count, const GLuint *ids, Boolean enabled) const;
    void debugMessageInsert(GLenum category, DebugSeverity severity, GLuint id,
                            GLsizei length, const GLchar *buf) const;
    GLuint getDebugMessageLog(GLuint count, GLsizei bufSize, GLenum *categories,
                              DebugSeverity *severities, GLuint *ids,
                              GLsizei *lengths, GLchar *message) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDDebugOutput _api;
};

// Manages the entry points for the GL_AMD_draw_buffers_blend extension API.
class AMDDrawBuffersBlendAPI : public BaseExtension
{
public:
    // Construction
    AMDDrawBuffersBlendAPI();

    // Accessors
    const AMDDrawBuffersBlend &getRawAPI() const;

    // Operations
    void blendEquationIndexed(GLuint buf, BlendEquationModeEXT mode) const;
    void blendEquationSeparateIndexed(GLuint buf, BlendEquationModeEXT modeRGB,
                                      BlendEquationModeEXT modeAlpha) const;
    void blendFuncIndexed(GLuint buf, GLenum src, GLenum dst) const;
    void blendFuncSeparateIndexed(GLuint buf, BlendingFactor srcRGB,
                                  BlendingFactor dstRGB, BlendingFactor srcAlpha,
                                  BlendingFactor dstAlpha) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDDrawBuffersBlend _api;
};

// Manages the entry points for the GL_AMD_framebuffer_multisample_advanced extension API.
class AMDFramebufferMultisampleAdvancedAPI : public BaseExtension
{
public:
    // Construction
    AMDFramebufferMultisampleAdvancedAPI();

    // Accessors
    const AMDFramebufferMultisampleAdvanced &getRawAPI() const;

    // Operations
    void namedRenderbufferStorageMultisampleAdvanced(RenderBufferName renderbuffer,
                                                     GLsizei samples,
                                                     GLsizei storageSamples,
                                                     InternalFormat internalformat,
                                                     GLsizei width,
                                                     GLsizei height) const;
    void renderbufferStorageMultisampleAdvanced(RenderBufferTarget target,
                                                GLsizei samples,
                                                GLsizei storageSamples,
                                                InternalFormat internalformat,
                                                GLsizei width, GLsizei height) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDFramebufferMultisampleAdvanced _api;
};

// Manages the entry points for the GL_AMD_framebuffer_sample_positions extension API.
class AMDFramebufferSamplePositionsAPI : public BaseExtension
{
public:
    // Construction
    AMDFramebufferSamplePositionsAPI();

    // Accessors
    const AMDFramebufferSamplePositions &getRawAPI() const;

    // Operations
    void framebufferSamplePositionsFV(FramebufferTarget target,
                                      GLuint numsamples, GLuint pixelindex,
                                      const GLfloat *values) const;
    void getFramebufferParameterFV(FramebufferTarget target,
                                   FramebufferAttachmentParameterName pname,
                                   GLuint numsamples, GLuint pixelindex,
                                   GLsizei size, GLfloat *values) const;
    void getNamedFramebufferParameterFV(FrameBufferName framebuffer,
                                        GLenum pname, GLuint numsamples,
                                        GLuint pixelindex, GLsizei size,
                                        GLfloat *values) const;
    void namedFramebufferSamplePositionsFV(FrameBufferName framebuffer,
                                           GLuint numsamples, GLuint pixelindex,
                                           const GLfloat *values) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDFramebufferSamplePositions _api;
};

// Manages the entry points for the GL_AMD_gpu_shader_int64 extension API.
class AMDGPUShaderInt64API : public BaseExtension
{
public:
    // Construction
    AMDGPUShaderInt64API();

    // Accessors
    const AMDGPUShaderInt64 &getRawAPI() const;

    // Operations
    void getUniformI64V(ProgramName program, GLint location, GLint64EXT *params) const;
    void getUniformUI64(ProgramName program, GLint location, GLuint64EXT *params) const;
    void programUniform1I64(ProgramName program, GLint location, GLint64EXT x) const;
    void programUniform1I64V(ProgramName program, GLint location, GLsizei count,
                             const GLint64EXT *value) const;
    void programUniform1UI64(ProgramName program, GLint location, GLuint64EXT x) const;
    void programUniform1UI64(ProgramName program, GLint location, GLsizei count,
                             const GLuint64EXT *value) const;
    void programUniform2I64(ProgramName program, GLint location, GLint64EXT x,
                            GLint64EXT y) const;
    void programUniform2I64V(ProgramName program, GLint location, GLsizei count,
                             const GLint64EXT *value) const;
    void programUniform2UI64(ProgramName program, GLint location, GLuint64EXT x,
                             GLuint64EXT y) const;
    void programUniform2UI64(ProgramName program, GLint location, GLsizei count,
                             const GLuint64EXT *value) const;
    void programUniform3I64(ProgramName program, GLint location, GLint64EXT x,
                            GLint64EXT y, GLint64EXT z) const;
    void programUniform3I64V(ProgramName program, GLint location, GLsizei count,
                             const GLint64EXT *value) const;
    void programUniform3UI64(ProgramName program, GLint location, GLuint64EXT x,
                             GLuint64EXT y, GLuint64EXT z) const;
    void programUniform3UI64(ProgramName program, GLint location, GLsizei count,
                             const GLuint64EXT *value) const;
    void programUniform4I64(ProgramName program, GLint location, GLint64EXT x,
                            GLint64EXT y, GLint64EXT z, GLint64EXT w) const;
    void programUniform4I64V(ProgramName program, GLint location, GLsizei count,
                             const GLint64EXT *value) const;
    void programUniform4UI64(ProgramName program, GLint location, GLuint64EXT x,
                             GLuint64EXT y, GLuint64EXT z, GLuint64EXT w) const;
    void programUniform4UI64(ProgramName program, GLint location, GLsizei count,
                             const GLuint64EXT *value) const;
    void uniform1I64(GLint location, GLint64EXT x) const;
    void uniform1I64V(GLint location, GLsizei count, const GLint64EXT *value) const;
    void uniform1UI64(GLint location, GLuint64EXT x) const;
    void uniform1UI64(GLint location, GLsizei count, const GLuint64EXT *value) const;
    void uniform2I64(GLint location, GLint64EXT x, GLint64EXT y) const;
    void uniform2I64V(GLint location, GLsizei count, const GLint64EXT *value) const;
    void uniform2UI64(GLint location, GLuint64EXT x, GLuint64EXT y) const;
    void uniform2UI64(GLint location, GLsizei count, const GLuint64EXT *value) const;
    void uniform3I64(GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z) const;
    void uniform3I64V(GLint location, GLsizei count, const GLint64EXT *value) const;
    void uniform3UI64(GLint location, GLuint64EXT x, GLuint64EXT y,
                      GLuint64EXT z) const;
    void uniform3UI64(GLint location, GLsizei count, const GLuint64EXT *value) const;
    void uniform4I64(GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z,
                     GLint64EXT w) const;
    void uniform4I64V(GLint location, GLsizei count, const GLint64EXT *value) const;
    void uniform4UI64(GLint location, GLuint64EXT x, GLuint64EXT y,
                      GLuint64EXT z, GLuint64EXT w) const;
    void uniform4UI64(GLint location, GLsizei count, const GLuint64EXT *value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDGPUShaderInt64 _api;
};

// Manages the entry points for the GL_AMD_interleaved_elements extension API.
class AMDInterleavedElementsAPI : public BaseExtension
{
public:
    // Construction
    AMDInterleavedElementsAPI();

    // Accessors
    const AMDInterleavedElements &getRawAPI() const;

    // Operations
    void vertexAttribParameterI(GLuint index, GLenum pname, GLint param) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDInterleavedElements _api;
};

// Manages the entry points for the GL_AMD_multi_draw_indirect extension API.
class AMDMultiDrawIndirectAPI : public BaseExtension
{
public:
    // Construction
    AMDMultiDrawIndirectAPI();

    // Accessors
    const AMDMultiDrawIndirect &getRawAPI() const;

    // Operations
    void multiDrawArraysIndirect(PrimitiveType mode, const void *indirect,
                                 GLsizei primcount, GLsizei stride) const;
    void multiDrawElementsIndirect(PrimitiveType mode, DrawElementsType type,
                                   const void *indirect, GLsizei primcount,
                                   GLsizei stride) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDMultiDrawIndirect _api;
};

// Manages the entry points for the GL_AMD_name_gen_delete extension API.
class AMDNameGenDeleteAPI : public BaseExtension
{
public:
    // Construction
    AMDNameGenDeleteAPI();

    // Accessors
    const AMDNameGenDelete &getRawAPI() const;

    // Operations
    void deleteNames(GLenum identifier, GLuint num, const GLuint *names) const;
    void genNames(GLenum identifier, GLuint num, GLuint *names) const;
    Boolean isName(GLenum identifier, GLuint name) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDNameGenDelete _api;
};

// Manages the entry points for the GL_AMD_occlusion_query_event extension API.
class AMDOcclusionQueryEventAPI : public BaseExtension
{
public:
    // Construction
    AMDOcclusionQueryEventAPI();

    // Accessors
    const AMDOcclusionQueryEvent &getRawAPI() const;

    // Operations
    void queryObjectParameterUI(QueryTarget target, QueryName id, GLenum pname,
                                GLuint  /* OcclusionQueryEventMaskAMD */ param) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDOcclusionQueryEvent _api;
};

// Manages the entry points for the GL_AMD_performance_monitor extension API.
class AMDPerformanceMonitorAPI : public BaseExtension
{
public:
    // Construction
    AMDPerformanceMonitorAPI();

    // Accessors
    const AMDPerformanceMonitor &getRawAPI() const;

    // Operations
    void beginPerfMonitor(GLuint monitor) const;
    void deletePerfMonitors(GLsizei n, GLuint *monitors) const;
    void endPerfMonitor(GLuint monitor) const;
    void genPerfMonitors(GLsizei n, GLuint *monitors) const;
    void getPerfMonitorCounterData(GLuint monitor, GLenum pname,
                                   GLsizei dataSize, GLuint *data,
                                   GLint *bytesWritten) const;
    void getPerfMonitorCounterInfo(GLuint group, GLuint counter, GLenum pname,
                                   void *data) const;
    void getPerfMonitorCounterString(GLuint group, GLuint counter,
                                     GLsizei bufSize, GLsizei *length,
                                     GLchar *counterString) const;
    void getPerfMonitorCounters(GLuint group, GLint *numCounters,
                                GLint *maxActiveCounters, GLsizei counterSize,
                                GLuint *counters) const;
    void getPerfMonitorGroupString(GLuint group, GLsizei bufSize,
                                   GLsizei *length, GLchar *groupString) const;
    void getPerfMonitorGroups(GLint *numGroups, GLsizei groupsSize,
                              GLuint *groups) const;
    void selectPerfMonitorCounters(GLuint monitor, Boolean enable, GLuint group,
                                   GLint numCounters, GLuint *counterList) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDPerformanceMonitor _api;
};

// Manages the entry points for the GL_AMD_sample_positions extension API.
class AMDSamplePositionsAPI : public BaseExtension
{
public:
    // Construction
    AMDSamplePositionsAPI();

    // Accessors
    const AMDSamplePositions &getRawAPI() const;

    // Operations
    void setMultisampleFV(GLenum pname, GLuint index, const GLfloat *val) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDSamplePositions _api;
};

// Manages the entry points for the GL_AMD_sparse_texture extension API.
class AMDSparseTextureAPI : public BaseExtension
{
public:
    // Construction
    AMDSparseTextureAPI();

    // Accessors
    const AMDSparseTexture &getRawAPI() const;

    // Operations
    void texStorageSparse(TextureTarget target,
                          SizedInternalFormat internalFormat, GLsizei width,
                          GLsizei height, GLsizei depth, GLsizei layers,
                          GLbitfield  /* TextureStorageMaskAMD */ flags) const;
    void textureStorageSparse(TextureName texture, GLenum target,
                              SizedInternalFormat internalFormat, GLsizei width,
                              GLsizei height, GLsizei depth, GLsizei layers,
                              GLbitfield  /* TextureStorageMaskAMD */ flags) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDSparseTexture _api;
};

// Manages the entry points for the GL_AMD_stencil_operation_extended extension API.
class AMDStencilOperationExtendedAPI : public BaseExtension
{
public:
    // Construction
    AMDStencilOperationExtendedAPI();

    // Accessors
    const AMDStencilOperationExtended &getRawAPI() const;

    // Operations
    void stencilOpValue(StencilFaceDirection face, GLuint value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDStencilOperationExtended _api;
};

// Manages the entry points for the GL_AMD_vertex_shader_tessellator extension API.
class AMDVertexShaderTessellatorAPI : public BaseExtension
{
public:
    // Construction
    AMDVertexShaderTessellatorAPI();

    // Accessors
    const AMDVertexShaderTessellator &getRawAPI() const;

    // Operations
    void tessellationFactor(GLfloat factor) const;
    void tessellationMode(GLenum mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    AMDVertexShaderTessellator _api;
};

// Manages the entry points for the GL_APPLE_element_array extension API.
class APPLEElementArrayAPI : public BaseExtension
{
public:
    // Construction
    APPLEElementArrayAPI();

    // Accessors
    const APPLEElementArray &getRawAPI() const;

    // Operations
    void drawElementArray(PrimitiveType mode, GLint first, GLsizei count) const;
    void drawRangeElementArray(PrimitiveType mode, GLuint start, GLuint end,
                               GLint first, GLsizei count) const;
    void elementPointer(ElementPointerTypeATI type, const void *pointer) const;
    void multiDrawElementArray(PrimitiveType mode, const GLint *first,
                               const GLsizei *count, GLsizei primcount) const;
    void multiDrawRangeElementArray(PrimitiveType mode, GLuint start, GLuint end,
                                    const GLint *first, const GLsizei *count,
                                    GLsizei primcount) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    APPLEElementArray _api;
};

// Manages the entry points for the GL_APPLE_fence extension API.
class APPLEFenceAPI : public BaseExtension
{
public:
    // Construction
    APPLEFenceAPI();

    // Accessors
    const APPLEFence &getRawAPI() const;

    // Operations
    void deleteFences(GLsizei n, const GLuint *fences) const;
    void finishFence(GLuint fence) const;
    void finishObject(ObjectTypeAPPLE object, GLint name) const;
    void genFences(GLsizei n, GLuint *fences) const;
    Boolean isFence(GLuint fence) const;
    void setFence(GLuint fence) const;
    Boolean testFence(GLuint fence) const;
    Boolean testObject(ObjectTypeAPPLE object, GLuint name) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    APPLEFence _api;
};

// Manages the entry points for the GL_APPLE_flush_buffer_range extension API.
class APPLEFlushBufferRangeAPI : public BaseExtension
{
public:
    // Construction
    APPLEFlushBufferRangeAPI();

    // Accessors
    const APPLEFlushBufferRange &getRawAPI() const;

    // Operations
    void bufferParameterI(GLenum target, GLenum pname, GLint param) const;
    void flushMappedBufferRange(BufferTarget target, GLintptr offset,
                                GLsizeiptr size) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    APPLEFlushBufferRange _api;
};

// Manages the entry points for the GL_APPLE_object_purgeable extension API.
class APPLEObjectPurgeableAPI : public BaseExtension
{
public:
    // Construction
    APPLEObjectPurgeableAPI();

    // Accessors
    const APPLEObjectPurgeable &getRawAPI() const;

    // Operations
    void getObjectParameterIV(GLenum objectType, GLuint name, GLenum pname,
                              GLint *params) const;
    GLenum objectPurgeable(GLenum objectType, GLuint name, GLenum option) const;
    GLenum objectUnpurgeable(GLenum objectType, GLuint name, GLenum option) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    APPLEObjectPurgeable _api;
};

// Manages the entry points for the GL_APPLE_texture_range extension API.
class APPLETextureRangeAPI : public BaseExtension
{
public:
    // Construction
    APPLETextureRangeAPI();

    // Accessors
    const APPLETextureRange &getRawAPI() const;

    // Operations
    void getTexParameterPointerV(GLenum target, GLenum pname, void **params) const;
    void textureRange(GLenum target, GLsizei length, const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    APPLETextureRange _api;
};

// Manages the entry points for the GL_APPLE_vertex_array_object extension API.
class APPLEVertexArrayObjectAPI : public BaseExtension
{
public:
    // Construction
    APPLEVertexArrayObjectAPI();

    // Accessors
    const APPLEVertexArrayObject &getRawAPI() const;

    // Operations
    void bindVertexArray(VertexArrayName array) const;
    void deleteVertexArrays(GLsizei n, const VertexArrayName *arrays) const;
    void genVertexArrays(GLsizei n, VertexArrayName *arrays) const;
    Boolean isVertexArray(VertexArrayName array) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    APPLEVertexArrayObject _api;
};

// Manages the entry points for the GL_APPLE_vertex_array_range extension API.
class APPLEVertexArrayRangeAPI : public BaseExtension
{
public:
    // Construction
    APPLEVertexArrayRangeAPI();

    // Accessors
    const APPLEVertexArrayRange &getRawAPI() const;

    // Operations
    void flushVertexArrayRange(GLsizei length, void *pointer) const;
    void vertexArrayParameterI(VertexArrayPNameAPPLE pname, GLint param) const;
    void vertexArrayRange(GLsizei length, void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    APPLEVertexArrayRange _api;
};

// Manages the entry points for the GL_APPLE_vertex_program_evaluators extension API.
class APPLEVertexProgramEvaluatorsAPI : public BaseExtension
{
public:
    // Construction
    APPLEVertexProgramEvaluatorsAPI();

    // Accessors
    const APPLEVertexProgramEvaluators &getRawAPI() const;

    // Operations
    void disableVertexAttrib(GLuint index, GLenum pname) const;
    void enableVertexAttrib(GLuint index, GLenum pname) const;
    Boolean isVertexAttribEnabled(GLuint index, GLenum pname) const;
    void mapVertexAttrib1D(GLuint index, GLuint size, GLdouble u1, GLdouble u2,
                           GLint stride, GLint order, const GLdouble *points) const;
    void mapVertexAttrib1F(GLuint index, GLuint size, GLfloat u1, GLfloat u2,
                           GLint stride, GLint order, const GLfloat *points) const;
    void mapVertexAttrib2D(GLuint index, GLuint size, GLdouble u1, GLdouble u2,
                           GLint ustride, GLint uorder, GLdouble v1, GLdouble v2,
                           GLint vstride, GLint vorder, const GLdouble *points) const;
    void mapVertexAttrib2F(GLuint index, GLuint size, GLfloat u1, GLfloat u2,
                           GLint ustride, GLint uorder, GLfloat v1, GLfloat v2,
                           GLint vstride, GLint vorder, const GLfloat *points) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    APPLEVertexProgramEvaluators _api;
};

// Manages the entry points for the GL_ARB_ES3_2_compatibility extension API.
class ARBES32CompatibilityAPI : public BaseExtension
{
public:
    // Construction
    ARBES32CompatibilityAPI();

    // Accessors
    const ARBES32Compatibility &getRawAPI() const;

    // Operations
    void primitiveBoundingBox(GLfloat minX, GLfloat minY, GLfloat minZ,
                              GLfloat minW, GLfloat maxX, GLfloat maxY,
                              GLfloat maxZ, GLfloat maxW) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBES32Compatibility _api;
};

// Manages the entry points for the GL_ARB_bindless_texture extension API.
class ARBBindlessTextureAPI : public BaseExtension
{
public:
    // Construction
    ARBBindlessTextureAPI();

    // Accessors
    const ARBBindlessTexture &getRawAPI() const;

    // Operations
    GLuint64 getImageHandle(TextureName texture, GLint level, Boolean layered,
                            GLint layer, PixelFormat format) const;
    GLuint64 getTextureHandle(TextureName texture) const;
    GLuint64 getTextureSamplerHandle(TextureName texture, SamplerName sampler) const;
    void getVertexAttribLUI64(GLuint index, VertexAttribEnum pname,
                              GLuint64EXT *params) const;
    Boolean isImageHandleResident(GLuint64 handle) const;
    Boolean isTextureHandleResident(GLuint64 handle) const;
    void makeImageHandleNonResident(GLuint64 handle) const;
    void makeImageHandleResident(GLuint64 handle, GLenum access) const;
    void makeTextureHandleNonResident(GLuint64 handle) const;
    void makeTextureHandleResident(GLuint64 handle) const;
    void programUniformHandleUI64(ProgramName program, GLint location,
                                  GLuint64 value) const;
    void programUniformHandleUI64(ProgramName program, GLint location,
                                  GLsizei count, const GLuint64 *values) const;
    void uniformHandleUI64(GLint location, GLuint64 value) const;
    void uniformHandleUI64(GLint location, GLsizei count, const GLuint64 *value) const;
    void vertexAttribL1UI64(GLuint index, GLuint64EXT x) const;
    void vertexAttribL1UI64(GLuint index, const GLuint64EXT *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBBindlessTexture _api;
};

// Manages the entry points for the GL_ARB_cl_event extension API.
class ARBClEventAPI : public BaseExtension
{
public:
    // Construction
    ARBClEventAPI();

    // Accessors
    const ARBClEvent &getRawAPI() const;

    // Operations
    SyncName createSyncFromCLevent(struct _cl_context *context,
                                   struct _cl_event *event, GLbitfield flags) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBClEvent _api;
};

// Manages the entry points for the GL_ARB_color_buffer_float extension API.
class ARBColorBufferFloatAPI : public BaseExtension
{
public:
    // Construction
    ARBColorBufferFloatAPI();

    // Accessors
    const ARBColorBufferFloat &getRawAPI() const;

    // Operations
    void clampColor(ClampColorTargetARB target, ClampColorModeARB clamp) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBColorBufferFloat _api;
};

// Manages the entry points for the GL_ARB_compute_variable_group_size extension API.
class ARBComputeVariableGroupSizeAPI : public BaseExtension
{
public:
    // Construction
    ARBComputeVariableGroupSizeAPI();

    // Accessors
    const ARBComputeVariableGroupSize &getRawAPI() const;

    // Operations
    void dispatchComputeGroupSize(GLuint num_groups_x, GLuint num_groups_y,
                                  GLuint num_groups_z, GLuint group_size_x,
                                  GLuint group_size_y, GLuint group_size_z) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBComputeVariableGroupSize _api;
};

// Manages the entry points for the GL_ARB_debug_output extension API.
class ARBDebugOutputAPI : public BaseExtension
{
public:
    // Construction
    ARBDebugOutputAPI();

    // Accessors
    const ARBDebugOutput &getRawAPI() const;

    // Operations
    void debugMessageCallback(GLDEBUGPROCARB callback, const void *userParam) const;
    void debugMessageControl(DebugSource source, DebugType type,
                             DebugSeverity severity, GLsizei count,
                             const GLuint *ids, Boolean enabled) const;
    void debugMessageInsert(DebugSource source, DebugType type, GLuint id,
                            DebugSeverity severity, GLsizei length,
                            const GLchar *buf) const;
    GLuint getDebugMessageLog(GLuint count, GLsizei bufSize,
                              DebugSource *sources, DebugType *types,
                              GLuint *ids, DebugSeverity *severities,
                              GLsizei *lengths, GLchar *messageLog) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBDebugOutput _api;
};

// Manages the entry points for the GL_ARB_draw_buffers extension API.
class ARBDrawBuffersAPI : public BaseExtension
{
public:
    // Construction
    ARBDrawBuffersAPI();

    // Accessors
    const ARBDrawBuffers &getRawAPI() const;

    // Operations
    void drawBuffers(GLsizei n, const DrawBufferMode *bufs) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBDrawBuffers _api;
};

// Manages the entry points for the GL_ARB_draw_buffers_blend extension API.
class ARBDrawBuffersBlendAPI : public BaseExtension
{
public:
    // Construction
    ARBDrawBuffersBlendAPI();

    // Accessors
    const ARBDrawBuffersBlend &getRawAPI() const;

    // Operations
    void blendEquationSeparateI(GLuint buf, BlendEquationModeEXT modeRGB,
                                BlendEquationModeEXT modeAlpha) const;
    void blendEquationI(GLuint buf, BlendEquationModeEXT mode) const;
    void blendFuncSeparateI(GLuint buf, BlendingFactor srcRGB,
                            BlendingFactor dstRGB, BlendingFactor srcAlpha,
                            BlendingFactor dstAlpha) const;
    void blendFuncI(GLuint buf, BlendingFactor src, BlendingFactor dst) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBDrawBuffersBlend _api;
};

// Manages the entry points for the GL_ARB_draw_instanced extension API.
class ARBDrawInstancedAPI : public BaseExtension
{
public:
    // Construction
    ARBDrawInstancedAPI();

    // Accessors
    const ARBDrawInstanced &getRawAPI() const;

    // Operations
    void drawArraysInstanced(PrimitiveType mode, GLint first, GLsizei count,
                             GLsizei primcount) const;
    void drawElementsInstanced(PrimitiveType mode, GLsizei count,
                               DrawElementsType type, const void *indices,
                               GLsizei primcount) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBDrawInstanced _api;
};

// Manages the entry points for the GL_ARB_fragment_program extension API.
class ARBFragmentProgramAPI : public BaseExtension
{
public:
    // Construction
    ARBFragmentProgramAPI();

    // Accessors
    const ARBFragmentProgram &getRawAPI() const;

    // Operations
    void bindProgram(ProgramTarget target, ProgramName program) const;
    void deletePrograms(GLsizei n, const ProgramName *programs) const;
    void genPrograms(GLsizei n, ProgramName *programs) const;
    void getProgramEnvParameterDV(ProgramTarget target, GLuint index,
                                  GLdouble *params) const;
    void getProgramEnvParameterFV(ProgramTarget target, GLuint index,
                                  GLfloat *params) const;
    void getProgramLocalParameterDV(ProgramTarget target, GLuint index,
                                    GLdouble *params) const;
    void getProgramLocalParameterFV(ProgramTarget target, GLuint index,
                                    GLfloat *params) const;
    void getProgramString(ProgramTarget target, ProgramStringProperty pname,
                          void *string) const;
    void getProgramIV(ProgramTarget target, ProgramProperty pname,
                      GLint *params) const;
    Boolean isProgram(ProgramName program) const;
    void programEnvParameter4D(ProgramTarget target, GLuint index, GLdouble x,
                               GLdouble y, GLdouble z, GLdouble w) const;
    void programEnvParameter4DV(ProgramTarget target, GLuint index,
                                const GLdouble *params) const;
    void programEnvParameter4F(ProgramTarget target, GLuint index, GLfloat x,
                               GLfloat y, GLfloat z, GLfloat w) const;
    void programEnvParameter4FV(ProgramTarget target, GLuint index,
                                const GLfloat *params) const;
    void programLocalParameter4D(ProgramTarget target, GLuint index, GLdouble x,
                                 GLdouble y, GLdouble z, GLdouble w) const;
    void programLocalParameter4DV(ProgramTarget target, GLuint index,
                                  const GLdouble *params) const;
    void programLocalParameter4F(ProgramTarget target, GLuint index, GLfloat x,
                                 GLfloat y, GLfloat z, GLfloat w) const;
    void programLocalParameter4FV(ProgramTarget target, GLuint index,
                                  const GLfloat *params) const;
    void programString(ProgramTarget target, ProgramFormat format, GLsizei len,
                       const void *string) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBFragmentProgram _api;
};

// Manages the entry points for the GL_ARB_geometry_shader4 extension API.
class ARBGeometrySHADER4API : public BaseExtension
{
public:
    // Construction
    ARBGeometrySHADER4API();

    // Accessors
    const ARBGeometrySHADER4 &getRawAPI() const;

    // Operations
    void framebufferTexture(FramebufferTarget target,
                            FramebufferAttachment attachment,
                            TextureName texture, GLint level) const;
    void framebufferTextureFace(FramebufferTarget target,
                                FramebufferAttachment attachment,
                                TextureName texture, GLint level,
                                TextureTarget face) const;
    void framebufferTextureLayer(FramebufferTarget target,
                                 FramebufferAttachment attachment,
                                 TextureName texture, GLint level, GLint layer) const;
    void programParameterI(ProgramName program, ProgramParameterPName pname,
                           GLint value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBGeometrySHADER4 _api;
};

// Manages the entry points for the GL_ARB_gl_spirv extension API.
class ARBGlSpirvAPI : public BaseExtension
{
public:
    // Construction
    ARBGlSpirvAPI();

    // Accessors
    const ARBGlSpirv &getRawAPI() const;

    // Operations
    void specializeShader(ShaderName shader, const GLchar *pEntryPoint,
                          GLuint numSpecializationConstants,
                          const GLuint *pConstantIndex,
                          const GLuint *pConstantValue) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBGlSpirv _api;
};

// Manages the entry points for the GL_ARB_gpu_shader_int64 extension API.
class ARBGPUShaderInt64API : public BaseExtension
{
public:
    // Construction
    ARBGPUShaderInt64API();

    // Accessors
    const ARBGPUShaderInt64 &getRawAPI() const;

    // Operations
    void getUniformI64V(ProgramName program, GLint location, GLint64 *params) const;
    void getUniformUI64(ProgramName program, GLint location, GLuint64 *params) const;
    void getnUniformI64V(ProgramName program, GLint location, GLsizei bufSize,
                         GLint64 *params) const;
    void getnUniformUI64(ProgramName program, GLint location, GLsizei bufSize,
                         GLuint64 *params) const;
    void programUniform1I64(ProgramName program, GLint location, GLint64 x) const;
    void programUniform1I64V(ProgramName program, GLint location, GLsizei count,
                             const GLint64 *value) const;
    void programUniform1UI64(ProgramName program, GLint location, GLuint64 x) const;
    void programUniform1UI64(ProgramName program, GLint location, GLsizei count,
                             const GLuint64 *value) const;
    void programUniform2I64(ProgramName program, GLint location, GLint64 x,
                            GLint64 y) const;
    void programUniform2I64V(ProgramName program, GLint location, GLsizei count,
                             const GLint64 *value) const;
    void programUniform2UI64(ProgramName program, GLint location, GLuint64 x,
                             GLuint64 y) const;
    void programUniform2UI64(ProgramName program, GLint location, GLsizei count,
                             const GLuint64 *value) const;
    void programUniform3I64(ProgramName program, GLint location, GLint64 x,
                            GLint64 y, GLint64 z) const;
    void programUniform3I64V(ProgramName program, GLint location, GLsizei count,
                             const GLint64 *value) const;
    void programUniform3UI64(ProgramName program, GLint location, GLuint64 x,
                             GLuint64 y, GLuint64 z) const;
    void programUniform3UI64(ProgramName program, GLint location, GLsizei count,
                             const GLuint64 *value) const;
    void programUniform4I64(ProgramName program, GLint location, GLint64 x,
                            GLint64 y, GLint64 z, GLint64 w) const;
    void programUniform4I64V(ProgramName program, GLint location, GLsizei count,
                             const GLint64 *value) const;
    void programUniform4UI64(ProgramName program, GLint location, GLuint64 x,
                             GLuint64 y, GLuint64 z, GLuint64 w) const;
    void programUniform4UI64(ProgramName program, GLint location, GLsizei count,
                             const GLuint64 *value) const;
    void uniform1I64(GLint location, GLint64 x) const;
    void uniform1I64V(GLint location, GLsizei count, const GLint64 *value) const;
    void uniform1UI64(GLint location, GLuint64 x) const;
    void uniform1UI64(GLint location, GLsizei count, const GLuint64 *value) const;
    void uniform2I64(GLint location, GLint64 x, GLint64 y) const;
    void uniform2I64V(GLint location, GLsizei count, const GLint64 *value) const;
    void uniform2UI64(GLint location, GLuint64 x, GLuint64 y) const;
    void uniform2UI64(GLint location, GLsizei count, const GLuint64 *value) const;
    void uniform3I64(GLint location, GLint64 x, GLint64 y, GLint64 z) const;
    void uniform3I64V(GLint location, GLsizei count, const GLint64 *value) const;
    void uniform3UI64(GLint location, GLuint64 x, GLuint64 y, GLuint64 z) const;
    void uniform3UI64(GLint location, GLsizei count, const GLuint64 *value) const;
    void uniform4I64(GLint location, GLint64 x, GLint64 y, GLint64 z, GLint64 w) const;
    void uniform4I64V(GLint location, GLsizei count, const GLint64 *value) const;
    void uniform4UI64(GLint location, GLuint64 x, GLuint64 y, GLuint64 z,
                      GLuint64 w) const;
    void uniform4UI64(GLint location, GLsizei count, const GLuint64 *value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBGPUShaderInt64 _api;
};

// Manages the entry points for the GL_ARB_indirect_parameters extension API.
class ARBIndirectParametersAPI : public BaseExtension
{
public:
    // Construction
    ARBIndirectParametersAPI();

    // Accessors
    const ARBIndirectParameters &getRawAPI() const;

    // Operations
    void multiDrawArraysIndirectCount(PrimitiveType mode, const void *indirect,
                                      GLintptr drawcount, GLsizei maxdrawcount,
                                      GLsizei stride) const;
    void multiDrawElementsIndirectCount(PrimitiveType mode,
                                        DrawElementsType type,
                                        const void *indirect, GLintptr drawcount,
                                        GLsizei maxdrawcount, GLsizei stride) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBIndirectParameters _api;
};

// Manages the entry points for the GL_ARB_instanced_arrays extension API.
class ARBInstancedArraysAPI : public BaseExtension
{
public:
    // Construction
    ARBInstancedArraysAPI();

    // Accessors
    const ARBInstancedArrays &getRawAPI() const;

    // Operations
    void vertexAttribDivisor(GLuint index, GLuint divisor) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBInstancedArrays _api;
};

// Manages the entry points for the GL_ARB_matrix_palette extension API.
class ARBMatrixPaletteAPI : public BaseExtension
{
public:
    // Construction
    ARBMatrixPaletteAPI();

    // Accessors
    const ARBMatrixPalette &getRawAPI() const;

    // Operations
    void currentPaletteMatrix(GLint index) const;
    void matrixIndexPointer(GLint size, MatrixIndexPointerTypeARB type,
                            GLsizei stride, const void *pointer) const;
    void matrixIndexUBV(GLint size, const GLubyte *indices) const;
    void matrixIndexUIV(GLint size, const GLuint *indices) const;
    void matrixIndexUSV(GLint size, const GLushort *indices) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBMatrixPalette _api;
};

// Manages the entry points for the GL_ARB_multisample extension API.
class ARBMultisampleAPI : public BaseExtension
{
public:
    // Construction
    ARBMultisampleAPI();

    // Accessors
    const ARBMultisample &getRawAPI() const;

    // Operations
    void sampleCoverage(GLfloat value, Boolean invert) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBMultisample _api;
};

// Manages the entry points for the GL_ARB_multitexture extension API.
class ARBMultitextureAPI : public BaseExtension
{
public:
    // Construction
    ARBMultitextureAPI();

    // Accessors
    const ARBMultitexture &getRawAPI() const;

    // Operations
    void activeTexture(TextureUnit texture) const;
    void clientActiveTexture(TextureUnit texture) const;
    void multiTexCoord1D(TextureUnit target, GLdouble s) const;
    void multiTexCoord1DV(TextureUnit target, const GLdouble *v) const;
    void multiTexCoord1F(TextureUnit target, GLfloat s) const;
    void multiTexCoord1FV(TextureUnit target, const GLfloat *v) const;
    void multiTexCoord1I(TextureUnit target, GLint s) const;
    void multiTexCoord1IV(TextureUnit target, const GLint *v) const;
    void multiTexCoord1S(TextureUnit target, GLshort s) const;
    void multiTexCoord1SV(TextureUnit target, const GLshort *v) const;
    void multiTexCoord2D(TextureUnit target, GLdouble s, GLdouble t) const;
    void multiTexCoord2DV(TextureUnit target, const GLdouble *v) const;
    void multiTexCoord2F(TextureUnit target, GLfloat s, GLfloat t) const;
    void multiTexCoord2FV(TextureUnit target, const GLfloat *v) const;
    void multiTexCoord2I(TextureUnit target, GLint s, GLint t) const;
    void multiTexCoord2IV(TextureUnit target, const GLint *v) const;
    void multiTexCoord2S(TextureUnit target, GLshort s, GLshort t) const;
    void multiTexCoord2SV(TextureUnit target, const GLshort *v) const;
    void multiTexCoord3D(TextureUnit target, GLdouble s, GLdouble t, GLdouble r) const;
    void multiTexCoord3DV(TextureUnit target, const GLdouble *v) const;
    void multiTexCoord3F(TextureUnit target, GLfloat s, GLfloat t, GLfloat r) const;
    void multiTexCoord3FV(TextureUnit target, const GLfloat *v) const;
    void multiTexCoord3I(TextureUnit target, GLint s, GLint t, GLint r) const;
    void multiTexCoord3IV(TextureUnit target, const GLint *v) const;
    void multiTexCoord3S(TextureUnit target, GLshort s, GLshort t, GLshort r) const;
    void multiTexCoord3SV(TextureUnit target, const GLshort *v) const;
    void multiTexCoord4D(TextureUnit target, GLdouble s, GLdouble t, GLdouble r,
                         GLdouble q) const;
    void multiTexCoord4DV(TextureUnit target, const GLdouble *v) const;
    void multiTexCoord4F(TextureUnit target, GLfloat s, GLfloat t, GLfloat r,
                         GLfloat q) const;
    void multiTexCoord4FV(TextureUnit target, const GLfloat *v) const;
    void multiTexCoord4I(TextureUnit target, GLint s, GLint t, GLint r, GLint q) const;
    void multiTexCoord4IV(TextureUnit target, const GLint *v) const;
    void multiTexCoord4S(TextureUnit target, GLshort s, GLshort t, GLshort r,
                         GLshort q) const;
    void multiTexCoord4SV(TextureUnit target, const GLshort *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBMultitexture _api;
};

// Manages the entry points for the GL_ARB_occlusion_query extension API.
class ARBOcclusionQueryAPI : public BaseExtension
{
public:
    // Construction
    ARBOcclusionQueryAPI();

    // Accessors
    const ARBOcclusionQuery &getRawAPI() const;

    // Operations
    void beginQuery(QueryTarget target, QueryName id) const;
    void deleteQueries(GLsizei n, const QueryName *ids) const;
    void endQuery(QueryTarget target) const;
    void genQueries(GLsizei n, QueryName *ids) const;
    void getQueryObjectIV(QueryName id, QueryObjectParameterName pname,
                          GLint *params) const;
    void getQueryObjectUIV(QueryName id, QueryObjectParameterName pname,
                           GLuint *params) const;
    void getQueryIV(QueryTarget target, QueryParameterName pname, GLint *params) const;
    Boolean isQuery(QueryName id) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBOcclusionQuery _api;
};

// Manages the entry points for the GL_ARB_parallel_shader_compile extension API.
class ARBParallelShaderCompileAPI : public BaseExtension
{
public:
    // Construction
    ARBParallelShaderCompileAPI();

    // Accessors
    const ARBParallelShaderCompile &getRawAPI() const;

    // Operations
    void maxShaderCompilerThreads(GLuint count) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBParallelShaderCompile _api;
};

// Manages the entry points for the GL_ARB_point_parameters extension API.
class ARBPointParametersAPI : public BaseExtension
{
public:
    // Construction
    ARBPointParametersAPI();

    // Accessors
    const ARBPointParameters &getRawAPI() const;

    // Operations
    void pointParameterF(PointParameterNameARB pname, GLfloat param) const;
    void pointParameterFV(PointParameterNameARB pname, const GLfloat *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBPointParameters _api;
};

// Manages the entry points for the GL_ARB_robustness extension API.
class ARBRobustnessAPI : public BaseExtension
{
public:
    // Construction
    ARBRobustnessAPI();

    // Accessors
    const ARBRobustness &getRawAPI() const;

    // Operations
    GraphicsResetStatus getGraphicsResetStatus() const;
    void getnCompressedTexImage(TextureTarget target, GLint lod, GLsizei bufSize,
                                void *img) const;
    void getnTexImage(TextureTarget target, GLint level, PixelFormat format,
                      PixelType type, GLsizei bufSize, void *img) const;
    void getnUniformDV(ProgramName program, GLint location, GLsizei bufSize,
                       GLdouble *params) const;
    void getnUniformFV(ProgramName program, GLint location, GLsizei bufSize,
                       GLfloat *params) const;
    void getnUniformIV(ProgramName program, GLint location, GLsizei bufSize,
                       GLint *params) const;
    void getnUniformUIV(ProgramName program, GLint location, GLsizei bufSize,
                        GLuint *params) const;
    void readnPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                     PixelFormat format, PixelType type, GLsizei bufSize,
                     void *data) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBRobustness _api;
};

// Manages the entry points for the GL_ARB_sample_locations extension API.
class ARBSampleLocationsAPI : public BaseExtension
{
public:
    // Construction
    ARBSampleLocationsAPI();

    // Accessors
    const ARBSampleLocations &getRawAPI() const;

    // Operations
    void evaluateDepthValues() const;
    void framebufferSampleLocationsFV(FramebufferTarget target, GLuint start,
                                      GLsizei count, const GLfloat *v) const;
    void namedFramebufferSampleLocationsFV(FrameBufferName framebuffer,
                                           GLuint start, GLsizei count,
                                           const GLfloat *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBSampleLocations _api;
};

// Manages the entry points for the GL_ARB_sample_shading extension API.
class ARBSampleShadingAPI : public BaseExtension
{
public:
    // Construction
    ARBSampleShadingAPI();

    // Accessors
    const ARBSampleShading &getRawAPI() const;

    // Operations
    void minSampleShading(GLfloat value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBSampleShading _api;
};

// Manages the entry points for the GL_ARB_shader_objects extension API.
class ARBShaderObjectsAPI : public BaseExtension
{
public:
    // Construction
    ARBShaderObjectsAPI();

    // Accessors
    const ARBShaderObjects &getRawAPI() const;

    // Operations
    void attachObject(GLhandleARB containerObj, GLhandleARB obj) const;
    void compileShader(GLhandleARB shaderObj) const;
    GLhandleARB createProgramObject() const;
    GLhandleARB createShaderObject(ShaderType shaderType) const;
    void deleteObject(GLhandleARB obj) const;
    void detachObject(GLhandleARB containerObj, GLhandleARB attachedObj) const;
    void getActiveUniform(GLhandleARB programObj, GLuint index,
                          GLsizei maxLength, GLsizei *length, GLint *size,
                          UniformType *type, GLcharARB *name) const;
    void getAttachedObjects(GLhandleARB containerObj, GLsizei maxCount,
                            GLsizei *count, GLhandleARB *obj) const;
    GLhandleARB getHandle(GLenum pname) const;
    void getInfoLog(GLhandleARB obj, GLsizei maxLength, GLsizei *length,
                    GLcharARB *infoLog) const;
    void getObjectParameterFV(GLhandleARB obj, GLenum pname, GLfloat *params) const;
    void getObjectParameterIV(GLhandleARB obj, GLenum pname, GLint *params) const;
    void getShaderSource(GLhandleARB obj, GLsizei maxLength, GLsizei *length,
                         GLcharARB *source) const;
    GLint getUniformLocation(GLhandleARB programObj, const GLcharARB *name) const;
    void getUniformFV(GLhandleARB programObj, GLint location, GLfloat *params) const;
    void getUniformIV(GLhandleARB programObj, GLint location, GLint *params) const;
    void linkProgram(GLhandleARB programObj) const;
    void shaderSource(GLhandleARB shaderObj, GLsizei count,
                      const GLcharARB **string, const GLint *length) const;
    void uniform1F(GLint location, GLfloat v0) const;
    void uniform1FV(GLint location, GLsizei count, const GLfloat *value) const;
    void uniform1I(GLint location, GLint v0) const;
    void uniform1IV(GLint location, GLsizei count, const GLint *value) const;
    void uniform2F(GLint location, GLfloat v0, GLfloat v1) const;
    void uniform2FV(GLint location, GLsizei count, const GLfloat *value) const;
    void uniform2I(GLint location, GLint v0, GLint v1) const;
    void uniform2IV(GLint location, GLsizei count, const GLint *value) const;
    void uniform3F(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) const;
    void uniform3FV(GLint location, GLsizei count, const GLfloat *value) const;
    void uniform3I(GLint location, GLint v0, GLint v1, GLint v2) const;
    void uniform3IV(GLint location, GLsizei count, const GLint *value) const;
    void uniform4F(GLint location, GLfloat v0, GLfloat v1, GLfloat v2,
                   GLfloat v3) const;
    void uniform4FV(GLint location, GLsizei count, const GLfloat *value) const;
    void uniform4I(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) const;
    void uniform4IV(GLint location, GLsizei count, const GLint *value) const;
    void uniformMatrix2FV(GLint location, GLsizei count, Boolean transpose,
                          const GLfloat *value) const;
    void uniformMatrix3FV(GLint location, GLsizei count, Boolean transpose,
                          const GLfloat *value) const;
    void uniformMatrix4FV(GLint location, GLsizei count, Boolean transpose,
                          const GLfloat *value) const;
    void useProgramObject(GLhandleARB programObj) const;
    void validateProgram(GLhandleARB programObj) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBShaderObjects _api;
};

// Manages the entry points for the GL_ARB_shading_language_include extension API.
class ARBShadingLanguageIncludeAPI : public BaseExtension
{
public:
    // Construction
    ARBShadingLanguageIncludeAPI();

    // Accessors
    const ARBShadingLanguageInclude &getRawAPI() const;

    // Operations
    void compileShaderInclude(ShaderName shader, GLsizei count,
                              const GLchar *const*path, const GLint *length) const;
    void deleteNamedString(GLint namelen, const GLchar *name) const;
    void getNamedString(GLint namelen, const GLchar *name, GLsizei bufSize,
                        GLint *stringlen, GLchar *string) const;
    void getNamedStringIV(GLint namelen, const GLchar *name, GLenum pname,
                          GLint *params) const;
    Boolean isNamedString(GLint namelen, const GLchar *name) const;
    void namedString(GLenum type, GLint namelen, const GLchar *name,
                     GLint stringlen, const GLchar *string) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBShadingLanguageInclude _api;
};

// Manages the entry points for the GL_ARB_sparse_buffer extension API.
class ARBSparseBufferAPI : public BaseExtension
{
public:
    // Construction
    ARBSparseBufferAPI();

    // Accessors
    const ARBSparseBuffer &getRawAPI() const;

    // Operations
    void bufferPageCommitment(GLenum target, GLintptr offset, GLsizeiptr size,
                              Boolean commit) const;
    void namedBufferPageCommitmentARB(BufferName buffer, GLintptr offset,
                                      GLsizeiptr size, Boolean commit) const;
    void namedBufferPageCommitmentEXT(BufferName buffer, GLintptr offset,
                                      GLsizeiptr size, Boolean commit) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBSparseBuffer _api;
};

// Manages the entry points for the GL_ARB_sparse_texture extension API.
class ARBSparseTextureAPI : public BaseExtension
{
public:
    // Construction
    ARBSparseTextureAPI();

    // Accessors
    const ARBSparseTexture &getRawAPI() const;

    // Operations
    void texPageCommitment(GLenum target, GLint level, GLint xoffset,
                           GLint yoffset, GLint zoffset, GLsizei width,
                           GLsizei height, GLsizei depth, Boolean commit) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBSparseTexture _api;
};

// Manages the entry points for the GL_ARB_texture_buffer_object extension API.
class ARBTextureBufferObjectAPI : public BaseExtension
{
public:
    // Construction
    ARBTextureBufferObjectAPI();

    // Accessors
    const ARBTextureBufferObject &getRawAPI() const;

    // Operations
    void texBuffer(TextureTarget target, SizedInternalFormat internalformat,
                   BufferName buffer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBTextureBufferObject _api;
};

// Manages the entry points for the GL_ARB_texture_compression extension API.
class ARBTextureCompressionAPI : public BaseExtension
{
public:
    // Construction
    ARBTextureCompressionAPI();

    // Accessors
    const ARBTextureCompression &getRawAPI() const;

    // Operations
    void compressedTexImage1D(TextureTarget target, GLint level,
                              InternalFormat internalformat, GLsizei width,
                              GLint border, GLsizei imageSize, const void *data) const;
    void compressedTexImage2D(TextureTarget target, GLint level,
                              InternalFormat internalformat, GLsizei width,
                              GLsizei height, GLint border, GLsizei imageSize,
                              const void *data) const;
    void compressedTexImage3D(TextureTarget target, GLint level,
                              InternalFormat internalformat, GLsizei width,
                              GLsizei height, GLsizei depth, GLint border,
                              GLsizei imageSize, const void *data) const;
    void compressedTexSubImage1D(TextureTarget target, GLint level,
                                 GLint xoffset, GLsizei width,
                                 InternalFormat format, GLsizei imageSize,
                                 const void *data) const;
    void compressedTexSubImage2D(TextureTarget target, GLint level,
                                 GLint xoffset, GLint yoffset, GLsizei width,
                                 GLsizei height, InternalFormat format,
                                 GLsizei imageSize, const void *data) const;
    void compressedTexSubImage3D(TextureTarget target, GLint level,
                                 GLint xoffset, GLint yoffset, GLint zoffset,
                                 GLsizei width, GLsizei height, GLsizei depth,
                                 InternalFormat format, GLsizei imageSize,
                                 const void *data) const;
    void getCompressedTexImage(TextureTarget target, GLint level, void *img) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBTextureCompression _api;
};

// Manages the entry points for the GL_ARB_transpose_matrix extension API.
class ARBTransposeMatrixAPI : public BaseExtension
{
public:
    // Construction
    ARBTransposeMatrixAPI();

    // Accessors
    const ARBTransposeMatrix &getRawAPI() const;

    // Operations
    void loadTransposeMatrixD(const GLdouble *m) const;
    void loadTransposeMatrixF(const GLfloat *m) const;
    void multTransposeMatrixD(const GLdouble *m) const;
    void multTransposeMatrixF(const GLfloat *m) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBTransposeMatrix _api;
};

// Manages the entry points for the GL_ARB_vertex_blend extension API.
class ARBVertexBlendAPI : public BaseExtension
{
public:
    // Construction
    ARBVertexBlendAPI();

    // Accessors
    const ARBVertexBlend &getRawAPI() const;

    // Operations
    void vertexBlend(GLint count) const;
    void weightPointer(GLint size, WeightPointerTypeARB type, GLsizei stride,
                       const void *pointer) const;
    void weightBV(GLint size, const GLbyte *weights) const;
    void weightDV(GLint size, const GLdouble *weights) const;
    void weightFV(GLint size, const GLfloat *weights) const;
    void weightIV(GLint size, const GLint *weights) const;
    void weightSV(GLint size, const GLshort *weights) const;
    void weightUBV(GLint size, const GLubyte *weights) const;
    void weightUIV(GLint size, const GLuint *weights) const;
    void weightUSV(GLint size, const GLushort *weights) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBVertexBlend _api;
};

// Manages the entry points for the GL_ARB_vertex_buffer_object extension API.
class ARBVertexBufferObjectAPI : public BaseExtension
{
public:
    // Construction
    ARBVertexBufferObjectAPI();

    // Accessors
    const ARBVertexBufferObject &getRawAPI() const;

    // Operations
    void bindBuffer(BufferTarget target, BufferName buffer) const;
    void bufferData(BufferTarget target, GLsizeiptrARB size, const void *data,
                    BufferUsage usage) const;
    void bufferSubData(BufferTarget target, GLintptrARB offset,
                       GLsizeiptrARB size, const void *data) const;
    void deleteBuffers(GLsizei n, const BufferName *buffers) const;
    void genBuffers(GLsizei n, BufferName *buffers) const;
    void getBufferParameterIV(BufferTarget target, BufferPNameARB pname,
                              GLint *params) const;
    void getBufferPointerV(BufferTarget target, BufferPointerNameARB pname,
                           void **params) const;
    void getBufferSubData(BufferTarget target, GLintptrARB offset,
                          GLsizeiptrARB size, void *data) const;
    Boolean isBuffer(BufferName buffer) const;
    void *mapBuffer(BufferTarget target, BufferAccessARB access) const;
    Boolean unmapBuffer(BufferTarget target) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBVertexBufferObject _api;
};

// Manages the entry points for the GL_ARB_vertex_program extension API.
class ARBVertexProgramAPI : public BaseExtension
{
public:
    // Construction
    ARBVertexProgramAPI();

    // Accessors
    const ARBVertexProgram &getRawAPI() const;

    // Operations
    void disableVertexAttribArray(GLuint index) const;
    void enableVertexAttribArray(GLuint index) const;
    void getVertexAttribPointerV(GLuint index,
                                 VertexAttribPointerPropertyARB pname,
                                 void **pointer) const;
    void getVertexAttribDV(GLuint index, VertexAttribPropertyARB pname,
                           GLdouble *params) const;
    void getVertexAttribFV(GLuint index, VertexAttribPropertyARB pname,
                           GLfloat *params) const;
    void getVertexAttribIV(GLuint index, VertexAttribPropertyARB pname,
                           GLint *params) const;
    void vertexAttrib1D(GLuint index, GLdouble x) const;
    void vertexAttrib1DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib1F(GLuint index, GLfloat x) const;
    void vertexAttrib1FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib1S(GLuint index, GLshort x) const;
    void vertexAttrib1SV(GLuint index, const GLshort *v) const;
    void vertexAttrib2D(GLuint index, GLdouble x, GLdouble y) const;
    void vertexAttrib2DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib2F(GLuint index, GLfloat x, GLfloat y) const;
    void vertexAttrib2FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib2S(GLuint index, GLshort x, GLshort y) const;
    void vertexAttrib2SV(GLuint index, const GLshort *v) const;
    void vertexAttrib3D(GLuint index, GLdouble x, GLdouble y, GLdouble z) const;
    void vertexAttrib3DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib3F(GLuint index, GLfloat x, GLfloat y, GLfloat z) const;
    void vertexAttrib3FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib3S(GLuint index, GLshort x, GLshort y, GLshort z) const;
    void vertexAttrib3SV(GLuint index, const GLshort *v) const;
    void vertexAttrib4NBV(GLuint index, const GLbyte *v) const;
    void vertexAttrib4NIV(GLuint index, const GLint *v) const;
    void vertexAttrib4NSV(GLuint index, const GLshort *v) const;
    void vertexAttrib4NUB(GLuint index, GLubyte x, GLubyte y, GLubyte z,
                          GLubyte w) const;
    void vertexAttrib4NUBV(GLuint index, const GLubyte *v) const;
    void vertexAttrib4NUIV(GLuint index, const GLuint *v) const;
    void vertexAttrib4NUSV(GLuint index, const GLushort *v) const;
    void vertexAttrib4BV(GLuint index, const GLbyte *v) const;
    void vertexAttrib4D(GLuint index, GLdouble x, GLdouble y, GLdouble z,
                        GLdouble w) const;
    void vertexAttrib4DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib4F(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
    void vertexAttrib4FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib4IV(GLuint index, const GLint *v) const;
    void vertexAttrib4S(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w) const;
    void vertexAttrib4SV(GLuint index, const GLshort *v) const;
    void vertexAttrib4UBV(GLuint index, const GLubyte *v) const;
    void vertexAttrib4UIV(GLuint index, const GLuint *v) const;
    void vertexAttrib4USV(GLuint index, const GLushort *v) const;
    void vertexAttribPointer(GLuint index, GLint size,
                             VertexAttribPointerType type, Boolean normalized,
                             GLsizei stride, const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBVertexProgram _api;
};

// Manages the entry points for the GL_ARB_vertex_shader extension API.
class ARBVertexShaderAPI : public BaseExtension
{
public:
    // Construction
    ARBVertexShaderAPI();

    // Accessors
    const ARBVertexShader &getRawAPI() const;

    // Operations
    void bindAttribLocation(GLhandleARB programObj, GLuint index,
                            const GLcharARB *name) const;
    void getActiveAttrib(GLhandleARB programObj, GLuint index, GLsizei maxLength,
                         GLsizei *length, GLint *size, AttributeType *type,
                         GLcharARB *name) const;
    GLint getAttribLocation(GLhandleARB programObj, const GLcharARB *name) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBVertexShader _api;
};

// Manages the entry points for the GL_ARB_viewport_array extension API.
class ARBViewportArrayAPI : public BaseExtension
{
public:
    // Construction
    ARBViewportArrayAPI();

    // Accessors
    const ARBViewportArray &getRawAPI() const;

    // Operations
    void depthRangeArrayDV(GLuint first, GLsizei count, const GLdouble *v) const;
    void depthRangeIndexedD(GLuint index, GLdouble n, GLdouble f) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBViewportArray _api;
};

// Manages the entry points for the GL_ARB_window_pos extension API.
class ARBWindowPosAPI : public BaseExtension
{
public:
    // Construction
    ARBWindowPosAPI();

    // Accessors
    const ARBWindowPos &getRawAPI() const;

    // Operations
    void windowPOS2D(GLdouble x, GLdouble y) const;
    void windowPOS2DV(const GLdouble *v) const;
    void windowPOS2F(GLfloat x, GLfloat y) const;
    void windowPOS2FV(const GLfloat *v) const;
    void windowPOS2I(GLint x, GLint y) const;
    void windowPOS2IV(const GLint *v) const;
    void windowPOS2S(GLshort x, GLshort y) const;
    void windowPOS2SV(const GLshort *v) const;
    void windowPOS3D(GLdouble x, GLdouble y, GLdouble z) const;
    void windowPOS3DV(const GLdouble *v) const;
    void windowPOS3F(GLfloat x, GLfloat y, GLfloat z) const;
    void windowPOS3FV(const GLfloat *v) const;
    void windowPOS3I(GLint x, GLint y, GLint z) const;
    void windowPOS3IV(const GLint *v) const;
    void windowPOS3S(GLshort x, GLshort y, GLshort z) const;
    void windowPOS3SV(const GLshort *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ARBWindowPos _api;
};

// Manages the entry points for the GL_ATI_draw_buffers extension API.
class ATIDrawBuffersAPI : public BaseExtension
{
public:
    // Construction
    ATIDrawBuffersAPI();

    // Accessors
    const ATIDrawBuffers &getRawAPI() const;

    // Operations
    void drawBuffers(GLsizei n, const DrawBufferMode *bufs) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIDrawBuffers _api;
};

// Manages the entry points for the GL_ATI_element_array extension API.
class ATIElementArrayAPI : public BaseExtension
{
public:
    // Construction
    ATIElementArrayAPI();

    // Accessors
    const ATIElementArray &getRawAPI() const;

    // Operations
    void drawElementArray(PrimitiveType mode, GLsizei count) const;
    void drawRangeElementArray(PrimitiveType mode, GLuint start, GLuint end,
                               GLsizei count) const;
    void elementPointer(ElementPointerTypeATI type, const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIElementArray _api;
};

// Manages the entry points for the GL_ATI_envmap_bumpmap extension API.
class ATIEnvmapBumpmapAPI : public BaseExtension
{
public:
    // Construction
    ATIEnvmapBumpmapAPI();

    // Accessors
    const ATIEnvmapBumpmap &getRawAPI() const;

    // Operations
    void getTexBumpParameterFV(GetTexBumpParameterATI pname, GLfloat *param) const;
    void getTexBumpParameterIV(GetTexBumpParameterATI pname, GLint *param) const;
    void texBumpParameterFV(TexBumpParameterATI pname, const GLfloat *param) const;
    void texBumpParameterIV(TexBumpParameterATI pname, const GLint *param) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIEnvmapBumpmap _api;
};

// Manages the entry points for the GL_ATI_fragment_shader extension API.
class ATIFragmentShaderAPI : public BaseExtension
{
public:
    // Construction
    ATIFragmentShaderAPI();

    // Accessors
    const ATIFragmentShader &getRawAPI() const;

    // Operations
    void alphaFragmentOp1(FragmentOpATI op, GLuint dst, GLuint dstMod,
                          GLuint arg1, GLuint arg1Rep, GLuint arg1Mod) const;
    void alphaFragmentOp2(FragmentOpATI op, GLuint dst, GLuint dstMod,
                          GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
                          GLuint arg2, GLuint arg2Rep, GLuint arg2Mod) const;
    void alphaFragmentOp3(FragmentOpATI op, GLuint dst, GLuint dstMod,
                          GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
                          GLuint arg2, GLuint arg2Rep, GLuint arg2Mod,
                          GLuint arg3, GLuint arg3Rep, GLuint arg3Mod) const;
    void beginFragmentShader() const;
    void bindFragmentShader(GLuint id) const;
    void colorFragmentOp1(FragmentOpATI op, GLuint dst, GLuint dstMask,
                          GLuint dstMod, GLuint arg1, GLuint arg1Rep,
                          GLuint arg1Mod) const;
    void colorFragmentOp2(FragmentOpATI op, GLuint dst, GLuint dstMask,
                          GLuint dstMod, GLuint arg1, GLuint arg1Rep,
                          GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
                          GLuint arg2Mod) const;
    void colorFragmentOp3(FragmentOpATI op, GLuint dst, GLuint dstMask,
                          GLuint dstMod, GLuint arg1, GLuint arg1Rep,
                          GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
                          GLuint arg2Mod, GLuint arg3, GLuint arg3Rep,
                          GLuint arg3Mod) const;
    void deleteFragmentShader(GLuint id) const;
    void endFragmentShader() const;
    GLuint genFragmentShaders(GLuint range) const;
    void passTexCoord(GLuint dst, GLuint coord, SwizzleOpATI swizzle) const;
    void sampleMap(GLuint dst, GLuint interp, SwizzleOpATI swizzle) const;
    void setFragmentShaderConstant(GLuint dst, const GLfloat *value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIFragmentShader _api;
};

// Manages the entry points for the GL_ATI_map_object_buffer extension API.
class ATIMapObjectBufferAPI : public BaseExtension
{
public:
    // Construction
    ATIMapObjectBufferAPI();

    // Accessors
    const ATIMapObjectBuffer &getRawAPI() const;

    // Operations
    void *mapObjectBuffer(BufferName buffer) const;
    void unmapObjectBuffer(BufferName buffer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIMapObjectBuffer _api;
};

// Manages the entry points for the GL_ATI_pn_triangles extension API.
class ATIPnTrianglesAPI : public BaseExtension
{
public:
    // Construction
    ATIPnTrianglesAPI();

    // Accessors
    const ATIPnTriangles &getRawAPI() const;

    // Operations
    void pnTrianglesF(PNTrianglesPNameATI pname, GLfloat param) const;
    void pnTrianglesI(PNTrianglesPNameATI pname, GLint param) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIPnTriangles _api;
};

// Manages the entry points for the GL_ATI_separate_stencil extension API.
class ATISeparateStencilAPI : public BaseExtension
{
public:
    // Construction
    ATISeparateStencilAPI();

    // Accessors
    const ATISeparateStencil &getRawAPI() const;

    // Operations
    void stencilFuncSeparate(StencilFunction frontfunc, StencilFunction backfunc,
                             GLint ref, GLuint mask) const;
    void stencilOpSeparate(StencilFaceDirection face, StencilOp sfail,
                           StencilOp dpfail, StencilOp dppass) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATISeparateStencil _api;
};

// Manages the entry points for the GL_ATI_vertex_array_object extension API.
class ATIVertexArrayObjectAPI : public BaseExtension
{
public:
    // Construction
    ATIVertexArrayObjectAPI();

    // Accessors
    const ATIVertexArrayObject &getRawAPI() const;

    // Operations
    void arrayObject(EnableCap array, GLint size, ScalarType type,
                     GLsizei stride, BufferName buffer, GLuint offset) const;
    void freeObjectBuffer(BufferName buffer) const;
    void getArrayObjectFV(EnableCap array, ArrayObjectPNameATI pname,
                          GLfloat *params) const;
    void getArrayObjectIV(EnableCap array, ArrayObjectPNameATI pname,
                          GLint *params) const;
    void getObjectBufferFV(BufferName buffer, ArrayObjectPNameATI pname,
                           GLfloat *params) const;
    void getObjectBufferIV(BufferName buffer, ArrayObjectPNameATI pname,
                           GLint *params) const;
    void getVariantArrayObjectFV(GLuint id, ArrayObjectPNameATI pname,
                                 GLfloat *params) const;
    void getVariantArrayObjectIV(GLuint id, ArrayObjectPNameATI pname,
                                 GLint *params) const;
    Boolean isObjectBuffer(BufferName buffer) const;
    GLuint newObjectBuffer(GLsizei size, const void *pointer,
                           ArrayObjectUsageATI usage) const;
    void updateObjectBuffer(BufferName buffer, GLuint offset, GLsizei size,
                            const void *pointer, PreserveModeATI preserve) const;
    void variantArrayObject(GLuint id, ScalarType type, GLsizei stride,
                            BufferName buffer, GLuint offset) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIVertexArrayObject _api;
};

// Manages the entry points for the GL_ATI_vertex_attrib_array_object extension API.
class ATIVertexAttribArrayObjectAPI : public BaseExtension
{
public:
    // Construction
    ATIVertexAttribArrayObjectAPI();

    // Accessors
    const ATIVertexAttribArrayObject &getRawAPI() const;

    // Operations
    void getVertexAttribArrayObjectFV(GLuint index, ArrayObjectPNameATI pname,
                                      GLfloat *params) const;
    void getVertexAttribArrayObjectIV(GLuint index, ArrayObjectPNameATI pname,
                                      GLint *params) const;
    void vertexAttribArrayObject(GLuint index, GLint size,
                                 VertexAttribPointerType type,
                                 Boolean normalized, GLsizei stride,
                                 BufferName buffer, GLuint offset) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIVertexAttribArrayObject _api;
};

// Manages the entry points for the GL_ATI_vertex_streams extension API.
class ATIVertexStreamsAPI : public BaseExtension
{
public:
    // Construction
    ATIVertexStreamsAPI();

    // Accessors
    const ATIVertexStreams &getRawAPI() const;

    // Operations
    void clientActiveVertexStream(VertexStreamATI stream) const;
    void normalStream3B(VertexStreamATI stream, GLbyte nx, GLbyte ny, GLbyte nz) const;
    void normalStream3BV(VertexStreamATI stream, const GLbyte *coords) const;
    void normalStream3D(VertexStreamATI stream, GLdouble nx, GLdouble ny,
                        GLdouble nz) const;
    void normalStream3DV(VertexStreamATI stream, const GLdouble *coords) const;
    void normalStream3F(VertexStreamATI stream, GLfloat nx, GLfloat ny,
                        GLfloat nz) const;
    void normalStream3FV(VertexStreamATI stream, const GLfloat *coords) const;
    void normalStream3I(VertexStreamATI stream, GLint nx, GLint ny, GLint nz) const;
    void normalStream3IV(VertexStreamATI stream, const GLint *coords) const;
    void normalStream3S(VertexStreamATI stream, GLshort nx, GLshort ny,
                        GLshort nz) const;
    void normalStream3SV(VertexStreamATI stream, const GLshort *coords) const;
    void vertexBlendEnvF(VertexStreamATI pname, GLfloat param) const;
    void vertexBlendEnvI(VertexStreamATI pname, GLint param) const;
    void vertexStream1D(VertexStreamATI stream, GLdouble x) const;
    void vertexStream1DV(VertexStreamATI stream, const GLdouble *coords) const;
    void vertexStream1F(VertexStreamATI stream, GLfloat x) const;
    void vertexStream1FV(VertexStreamATI stream, const GLfloat *coords) const;
    void vertexStream1I(VertexStreamATI stream, GLint x) const;
    void vertexStream1IV(VertexStreamATI stream, const GLint *coords) const;
    void vertexStream1S(VertexStreamATI stream, GLshort x) const;
    void vertexStream1SV(VertexStreamATI stream, const GLshort *coords) const;
    void vertexStream2D(VertexStreamATI stream, GLdouble x, GLdouble y) const;
    void vertexStream2DV(VertexStreamATI stream, const GLdouble *coords) const;
    void vertexStream2F(VertexStreamATI stream, GLfloat x, GLfloat y) const;
    void vertexStream2FV(VertexStreamATI stream, const GLfloat *coords) const;
    void vertexStream2I(VertexStreamATI stream, GLint x, GLint y) const;
    void vertexStream2IV(VertexStreamATI stream, const GLint *coords) const;
    void vertexStream2S(VertexStreamATI stream, GLshort x, GLshort y) const;
    void vertexStream2SV(VertexStreamATI stream, const GLshort *coords) const;
    void vertexStream3D(VertexStreamATI stream, GLdouble x, GLdouble y,
                        GLdouble z) const;
    void vertexStream3DV(VertexStreamATI stream, const GLdouble *coords) const;
    void vertexStream3F(VertexStreamATI stream, GLfloat x, GLfloat y, GLfloat z) const;
    void vertexStream3FV(VertexStreamATI stream, const GLfloat *coords) const;
    void vertexStream3I(VertexStreamATI stream, GLint x, GLint y, GLint z) const;
    void vertexStream3IV(VertexStreamATI stream, const GLint *coords) const;
    void vertexStream3S(VertexStreamATI stream, GLshort x, GLshort y, GLshort z) const;
    void vertexStream3SV(VertexStreamATI stream, const GLshort *coords) const;
    void vertexStream4D(VertexStreamATI stream, GLdouble x, GLdouble y,
                        GLdouble z, GLdouble w) const;
    void vertexStream4DV(VertexStreamATI stream, const GLdouble *coords) const;
    void vertexStream4F(VertexStreamATI stream, GLfloat x, GLfloat y, GLfloat z,
                        GLfloat w) const;
    void vertexStream4FV(VertexStreamATI stream, const GLfloat *coords) const;
    void vertexStream4I(VertexStreamATI stream, GLint x, GLint y, GLint z,
                        GLint w) const;
    void vertexStream4IV(VertexStreamATI stream, const GLint *coords) const;
    void vertexStream4S(VertexStreamATI stream, GLshort x, GLshort y, GLshort z,
                        GLshort w) const;
    void vertexStream4SV(VertexStreamATI stream, const GLshort *coords) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    ATIVertexStreams _api;
};

// Manages the entry points for the GL_EXT_EGL_image_storage extension API.
class EXTEglImageStorageAPI : public BaseExtension
{
public:
    // Construction
    EXTEglImageStorageAPI();

    // Accessors
    const EXTEglImageStorage &getRawAPI() const;

    // Operations
    void eglImageTargetTexStorage(GLenum target, GLeglImageOES image,
                                  const GLint*attrib_list) const;
    void eglImageTargetTextureStorage(TextureName texture, GLeglImageOES image,
                                      const GLint*attrib_list) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTEglImageStorage _api;
};

// Manages the entry points for the GL_EXT_bindable_uniform extension API.
class EXTBindableUniformAPI : public BaseExtension
{
public:
    // Construction
    EXTBindableUniformAPI();

    // Accessors
    const EXTBindableUniform &getRawAPI() const;

    // Operations
    GLint getUniformBufferSize(ProgramName program, GLint location) const;
    GLintptr getUniformOffset(ProgramName program, GLint location) const;
    void uniformBuffer(ProgramName program, GLint location, BufferName buffer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTBindableUniform _api;
};

// Manages the entry points for the GL_EXT_blend_color extension API.
class EXTBlendColorAPI : public BaseExtension
{
public:
    // Construction
    EXTBlendColorAPI();

    // Accessors
    const EXTBlendColor &getRawAPI() const;

    // Operations
    void blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTBlendColor _api;
};

// Manages the entry points for the GL_EXT_blend_equation_separate extension API.
class EXTBlendEquationSeparateAPI : public BaseExtension
{
public:
    // Construction
    EXTBlendEquationSeparateAPI();

    // Accessors
    const EXTBlendEquationSeparate &getRawAPI() const;

    // Operations
    void blendEquationSeparate(BlendEquationModeEXT modeRGB,
                               BlendEquationModeEXT modeAlpha) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTBlendEquationSeparate _api;
};

// Manages the entry points for the GL_EXT_blend_func_separate extension API.
class EXTBlendFuncSeparateAPI : public BaseExtension
{
public:
    // Construction
    EXTBlendFuncSeparateAPI();

    // Accessors
    const EXTBlendFuncSeparate &getRawAPI() const;

    // Operations
    void blendFuncSeparate(BlendingFactor sfactorRGB, BlendingFactor dfactorRGB,
                           BlendingFactor sfactorAlpha,
                           BlendingFactor dfactorAlpha) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTBlendFuncSeparate _api;
};

// Manages the entry points for the GL_EXT_blend_minmax extension API.
class EXTBlendMinmaxAPI : public BaseExtension
{
public:
    // Construction
    EXTBlendMinmaxAPI();

    // Accessors
    const EXTBlendMinmax &getRawAPI() const;

    // Operations
    void blendEquation(BlendEquationModeEXT mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTBlendMinmax _api;
};

// Manages the entry points for the GL_EXT_color_subtable extension API.
class EXTColorSubtableAPI : public BaseExtension
{
public:
    // Construction
    EXTColorSubtableAPI();

    // Accessors
    const EXTColorSubtable &getRawAPI() const;

    // Operations
    void colorSubTable(GLenum target, GLsizei start, GLsizei count,
                       PixelFormat format, PixelType type, const void *data) const;
    void copyColorSubTable(GLenum target, GLsizei start, GLint x, GLint y,
                           GLsizei width) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTColorSubtable _api;
};

// Manages the entry points for the GL_EXT_compiled_vertex_array extension API.
class EXTCompiledVertexArrayAPI : public BaseExtension
{
public:
    // Construction
    EXTCompiledVertexArrayAPI();

    // Accessors
    const EXTCompiledVertexArray &getRawAPI() const;

    // Operations
    void lockArrays(GLint first, GLsizei count) const;
    void unlockArrays() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTCompiledVertexArray _api;
};

// Manages the entry points for the GL_EXT_convolution extension API.
class EXTConvolutionAPI : public BaseExtension
{
public:
    // Construction
    EXTConvolutionAPI();

    // Accessors
    const EXTConvolution &getRawAPI() const;

    // Operations
    void convolutionFilter1D(ConvolutionTargetEXT target,
                             InternalFormat internalformat, GLsizei width,
                             PixelFormat format, PixelType type,
                             const void *image) const;
    void convolutionFilter2D(ConvolutionTargetEXT target,
                             InternalFormat internalformat, GLsizei width,
                             GLsizei height, PixelFormat format, PixelType type,
                             const void *image) const;
    void convolutionParameterF(ConvolutionTargetEXT target,
                               ConvolutionParameterEXT pname, GLfloat params) const;
    void convolutionParameterFV(ConvolutionTargetEXT target,
                                ConvolutionParameterEXT pname,
                                const GLfloat *params) const;
    void convolutionParameterI(ConvolutionTargetEXT target,
                               ConvolutionParameterEXT pname, GLint params) const;
    void convolutionParameterIV(ConvolutionTargetEXT target,
                                ConvolutionParameterEXT pname,
                                const GLint *params) const;
    void copyConvolutionFilter1D(ConvolutionTargetEXT target,
                                 InternalFormat internalformat, GLint x, GLint y,
                                 GLsizei width) const;
    void copyConvolutionFilter2D(ConvolutionTargetEXT target,
                                 InternalFormat internalformat, GLint x, GLint y,
                                 GLsizei width, GLsizei height) const;
    void getConvolutionFilter(ConvolutionTargetEXT target, PixelFormat format,
                              PixelType type, void *image) const;
    void getConvolutionParameterFV(ConvolutionTargetEXT target,
                                   ConvolutionParameterEXT pname,
                                   GLfloat *params) const;
    void getConvolutionParameterIV(ConvolutionTargetEXT target,
                                   ConvolutionParameterEXT pname, GLint *params) const;
    void getSeparableFilter(SeparableTargetEXT target, PixelFormat format,
                            PixelType type, void *row, void *column, void *span) const;
    void separableFilter2D(SeparableTargetEXT target,
                           InternalFormat internalformat, GLsizei width,
                           GLsizei height, PixelFormat format, PixelType type,
                           const void *row, const void *column) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTConvolution _api;
};

// Manages the entry points for the GL_EXT_coordinate_frame extension API.
class EXTCoordinateFrameAPI : public BaseExtension
{
public:
    // Construction
    EXTCoordinateFrameAPI();

    // Accessors
    const EXTCoordinateFrame &getRawAPI() const;

    // Operations
    void binormal3B(GLbyte bx, GLbyte by, GLbyte bz) const;
    void binormal3BV(const GLbyte *v) const;
    void binormal3D(GLdouble bx, GLdouble by, GLdouble bz) const;
    void binormal3DV(const GLdouble *v) const;
    void binormal3F(GLfloat bx, GLfloat by, GLfloat bz) const;
    void binormal3FV(const GLfloat *v) const;
    void binormal3I(GLint bx, GLint by, GLint bz) const;
    void binormal3IV(const GLint *v) const;
    void binormal3S(GLshort bx, GLshort by, GLshort bz) const;
    void binormal3SV(const GLshort *v) const;
    void binormalPointer(BinormalPointerTypeEXT type, GLsizei stride,
                         const void *pointer) const;
    void tangent3B(GLbyte tx, GLbyte ty, GLbyte tz) const;
    void tangent3BV(const GLbyte *v) const;
    void tangent3D(GLdouble tx, GLdouble ty, GLdouble tz) const;
    void tangent3DV(const GLdouble *v) const;
    void tangent3F(GLfloat tx, GLfloat ty, GLfloat tz) const;
    void tangent3FV(const GLfloat *v) const;
    void tangent3I(GLint tx, GLint ty, GLint tz) const;
    void tangent3IV(const GLint *v) const;
    void tangent3S(GLshort tx, GLshort ty, GLshort tz) const;
    void tangent3SV(const GLshort *v) const;
    void tangentPointer(TangentPointerTypeEXT type, GLsizei stride,
                        const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTCoordinateFrame _api;
};

// Manages the entry points for the GL_EXT_copy_texture extension API.
class EXTCopyTextureAPI : public BaseExtension
{
public:
    // Construction
    EXTCopyTextureAPI();

    // Accessors
    const EXTCopyTexture &getRawAPI() const;

    // Operations
    void copyTexImage1D(TextureTarget target, GLint level,
                        InternalFormat internalformat, GLint x, GLint y,
                        GLsizei width, GLint border) const;
    void copyTexImage2D(TextureTarget target, GLint level,
                        InternalFormat internalformat, GLint x, GLint y,
                        GLsizei width, GLsizei height, GLint border) const;
    void copyTexSubImage1D(TextureTarget target, GLint level, GLint xoffset,
                           GLint x, GLint y, GLsizei width) const;
    void copyTexSubImage2D(TextureTarget target, GLint level, GLint xoffset,
                           GLint yoffset, GLint x, GLint y, GLsizei width,
                           GLsizei height) const;
    void copyTexSubImage3D(TextureTarget target, GLint level, GLint xoffset,
                           GLint yoffset, GLint zoffset, GLint x, GLint y,
                           GLsizei width, GLsizei height) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTCopyTexture _api;
};

// Manages the entry points for the GL_EXT_cull_vertex extension API.
class EXTCullVertexAPI : public BaseExtension
{
public:
    // Construction
    EXTCullVertexAPI();

    // Accessors
    const EXTCullVertex &getRawAPI() const;

    // Operations
    void cullParameterDV(CullParameterEXT pname, GLdouble *params) const;
    void cullParameterFV(CullParameterEXT pname, GLfloat *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTCullVertex _api;
};

// Manages the entry points for the GL_EXT_debug_label extension API.
class EXTDebugLabelAPI : public BaseExtension
{
public:
    // Construction
    EXTDebugLabelAPI();

    // Accessors
    const EXTDebugLabel &getRawAPI() const;

    // Operations
    void getObjectLabel(GLenum type, GLuint object, GLsizei bufSize,
                        GLsizei *length, GLchar *label) const;
    void labelObject(GLenum type, GLuint object, GLsizei length,
                     const GLchar *label) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTDebugLabel _api;
};

// Manages the entry points for the GL_EXT_debug_marker extension API.
class EXTDebugMarkerAPI : public BaseExtension
{
public:
    // Construction
    EXTDebugMarkerAPI();

    // Accessors
    const EXTDebugMarker &getRawAPI() const;

    // Operations
    void insertEventMarker(GLsizei length, const GLchar *marker) const;
    void popGroupMarker() const;
    void pushGroupMarker(GLsizei length, const GLchar *marker) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTDebugMarker _api;
};

// Manages the entry points for the GL_EXT_depth_bounds_test extension API.
class EXTDepthBoundsTestAPI : public BaseExtension
{
public:
    // Construction
    EXTDepthBoundsTestAPI();

    // Accessors
    const EXTDepthBoundsTest &getRawAPI() const;

    // Operations
    void depthBounds(GLclampd zmin, GLclampd zmax) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTDepthBoundsTest _api;
};

// Manages the entry points for the GL_EXT_direct_state_access extension API.
class EXTDirectStateAccessAPI : public BaseExtension
{
public:
    // Construction
    EXTDirectStateAccessAPI();

    // Accessors
    const EXTDirectStateAccess &getRawAPI() const;

    // Operations
    void bindMultiTexture(TextureUnit texunit, TextureTarget target,
                          TextureName texture) const;
    FramebufferStatus checkNamedFramebufferStatus(FrameBufferName framebuffer,
                                                  FramebufferTarget target) const;
    void clearNamedBufferData(BufferName buffer,
                              SizedInternalFormat internalformat,
                              PixelFormat format, PixelType type,
                              const void *data) const;
    void clearNamedBufferSubData(BufferName buffer,
                                 SizedInternalFormat internalformat,
                                 GLsizeiptr offset, GLsizeiptr size,
                                 PixelFormat format, PixelType type,
                                 const void *data) const;
    void clientAttribDefault(GLbitfield mask) const;
    void compressedMultiTexImage1D(TextureUnit texunit, TextureTarget target,
                                   GLint level, InternalFormat internalformat,
                                   GLsizei width, GLint border,
                                   GLsizei imageSize, const void *bits) const;
    void compressedMultiTexImage2D(TextureUnit texunit, TextureTarget target,
                                   GLint level, InternalFormat internalformat,
                                   GLsizei width, GLsizei height, GLint border,
                                   GLsizei imageSize, const void *bits) const;
    void compressedMultiTexImage3D(TextureUnit texunit, TextureTarget target,
                                   GLint level, InternalFormat internalformat,
                                   GLsizei width, GLsizei height, GLsizei depth,
                                   GLint border, GLsizei imageSize,
                                   const void *bits) const;
    void compressedMultiTexSubImage1D(TextureUnit texunit, TextureTarget target,
                                      GLint level, GLint xoffset, GLsizei width,
                                      InternalFormat format, GLsizei imageSize,
                                      const void *bits) const;
    void compressedMultiTexSubImage2D(TextureUnit texunit, TextureTarget target,
                                      GLint level, GLint xoffset, GLint yoffset,
                                      GLsizei width, GLsizei height,
                                      InternalFormat format, GLsizei imageSize,
                                      const void *bits) const;
    void compressedMultiTexSubImage3D(TextureUnit texunit, TextureTarget target,
                                      GLint level, GLint xoffset, GLint yoffset,
                                      GLint zoffset, GLsizei width,
                                      GLsizei height, GLsizei depth,
                                      InternalFormat format, GLsizei imageSize,
                                      const void *bits) const;
    void compressedTextureImage1D(TextureName texture, TextureTarget target,
                                  GLint level, InternalFormat internalformat,
                                  GLsizei width, GLint border, GLsizei imageSize,
                                  const void *bits) const;
    void compressedTextureImage2D(TextureName texture, TextureTarget target,
                                  GLint level, InternalFormat internalformat,
                                  GLsizei width, GLsizei height, GLint border,
                                  GLsizei imageSize, const void *bits) const;
    void compressedTextureImage3D(TextureName texture, TextureTarget target,
                                  GLint level, InternalFormat internalformat,
                                  GLsizei width, GLsizei height, GLsizei depth,
                                  GLint border, GLsizei imageSize,
                                  const void *bits) const;
    void compressedTextureSubImage1D(TextureName texture, TextureTarget target,
                                     GLint level, GLint xoffset, GLsizei width,
                                     InternalFormat format, GLsizei imageSize,
                                     const void *bits) const;
    void compressedTextureSubImage2D(TextureName texture, TextureTarget target,
                                     GLint level, GLint xoffset, GLint yoffset,
                                     GLsizei width, GLsizei height,
                                     InternalFormat format, GLsizei imageSize,
                                     const void *bits) const;
    void compressedTextureSubImage3D(TextureName texture, TextureTarget target,
                                     GLint level, GLint xoffset, GLint yoffset,
                                     GLint zoffset, GLsizei width,
                                     GLsizei height, GLsizei depth,
                                     InternalFormat format, GLsizei imageSize,
                                     const void *bits) const;
    void copyMultiTexImage1D(TextureUnit texunit, TextureTarget target,
                             GLint level, InternalFormat internalformat, GLint x,
                             GLint y, GLsizei width, GLint border) const;
    void copyMultiTexImage2D(TextureUnit texunit, TextureTarget target,
                             GLint level, InternalFormat internalformat, GLint x,
                             GLint y, GLsizei width, GLsizei height,
                             GLint border) const;
    void copyMultiTexSubImage1D(TextureUnit texunit, TextureTarget target,
                                GLint level, GLint xoffset, GLint x, GLint y,
                                GLsizei width) const;
    void copyMultiTexSubImage2D(TextureUnit texunit, TextureTarget target,
                                GLint level, GLint xoffset, GLint yoffset,
                                GLint x, GLint y, GLsizei width, GLsizei height) const;
    void copyMultiTexSubImage3D(TextureUnit texunit, TextureTarget target,
                                GLint level, GLint xoffset, GLint yoffset,
                                GLint zoffset, GLint x, GLint y, GLsizei width,
                                GLsizei height) const;
    void copyTextureImage1D(TextureName texture, TextureTarget target,
                            GLint level, InternalFormat internalformat, GLint x,
                            GLint y, GLsizei width, GLint border) const;
    void copyTextureImage2D(TextureName texture, TextureTarget target,
                            GLint level, InternalFormat internalformat, GLint x,
                            GLint y, GLsizei width, GLsizei height, GLint border) const;
    void copyTextureSubImage1D(TextureName texture, TextureTarget target,
                               GLint level, GLint xoffset, GLint x, GLint y,
                               GLsizei width) const;
    void copyTextureSubImage2D(TextureName texture, TextureTarget target,
                               GLint level, GLint xoffset, GLint yoffset,
                               GLint x, GLint y, GLsizei width, GLsizei height) const;
    void copyTextureSubImage3D(TextureName texture, TextureTarget target,
                               GLint level, GLint xoffset, GLint yoffset,
                               GLint zoffset, GLint x, GLint y, GLsizei width,
                               GLsizei height) const;
    void disableClientStateIndexed(EnableCap array, GLuint index) const;
    void disableClientStateI(EnableCap array, GLuint index) const;
    void disableIndexed(EnableCap target, GLuint index) const;
    void disableVertexArrayAttrib(VertexArrayName vaobj, GLuint index) const;
    void disableVertexArray(VertexArrayName vaobj, EnableCap array) const;
    void enableClientStateIndexed(EnableCap array, GLuint index) const;
    void enableClientStateI(EnableCap array, GLuint index) const;
    void enableIndexed(EnableCap target, GLuint index) const;
    void enableVertexArrayAttrib(VertexArrayName vaobj, GLuint index) const;
    void enableVertexArray(VertexArrayName vaobj, EnableCap array) const;
    void flushMappedNamedBufferRange(BufferName buffer, GLintptr offset,
                                     GLsizeiptr length) const;
    void framebufferDrawBuffer(FrameBufferName framebuffer, DrawBufferMode mode) const;
    void framebufferDrawBuffers(FrameBufferName framebuffer, GLsizei n,
                                const DrawBufferMode *bufs) const;
    void framebufferReadBuffer(FrameBufferName framebuffer, ReadBufferMode mode) const;
    void generateMultiTexMipmap(TextureUnit texunit, TextureTarget target) const;
    void generateTextureMipmap(TextureName texture, TextureTarget target) const;
    void getBooleanIndexedv(BufferTarget target, GLuint index, Boolean *data) const;
    void getCompressedMultiTexImage(TextureUnit texunit, TextureTarget target,
                                    GLint lod, void *img) const;
    void getCompressedTextureImage(TextureName texture, TextureTarget target,
                                   GLint lod, void *img) const;
    void getDoubleIndexedv(GetPName target, GLuint index, GLdouble *data) const;
    void getDoubleIV(GetPName pname, GLuint index, GLdouble *params) const;
    void getFloatIndexedv(GetPName target, GLuint index, GLfloat *data) const;
    void getFloatIV(GetPName pname, GLuint index, GLfloat *params) const;
    void getFramebufferParameterIV(FrameBufferName framebuffer,
                                   GetFramebufferParameter pname, GLint *params) const;
    void getIntegerIndexedv(GetPName target, GLuint index, GLint *data) const;
    void getMultiTexEnvFV(TextureUnit texunit, GLenum target, GLenum pname,
                          GLfloat *params) const;
    void getMultiTexEnvIV(TextureUnit texunit, GLenum target, GLenum pname,
                          GLint *params) const;
    void getMultiTexGenDV(TextureUnit texunit, GLenum coord,
                          TextureGenParameter pname, GLdouble *params) const;
    void getMultiTexGenFV(TextureUnit texunit, GLenum coord,
                          TextureGenParameter pname, GLfloat *params) const;
    void getMultiTexGenIV(TextureUnit texunit, GLenum coord,
                          TextureGenParameter pname, GLint *params) const;
    void getMultiTexImage(TextureUnit texunit, TextureTarget target, GLint level,
                          PixelFormat format, PixelType type, void *pixels) const;
    void getMultiTexLevelParameterFV(TextureUnit texunit, TextureTarget target,
                                     GLint level, GetTextureParameter pname,
                                     GLfloat *params) const;
    void getMultiTexLevelParameterIV(TextureUnit texunit, TextureTarget target,
                                     GLint level, GetTextureParameter pname,
                                     GLint *params) const;
    void getMultiTexParameterIIV(TextureUnit texunit, TextureTarget target,
                                 GetTextureParameter pname, GLint *params) const;
    void getMultiTexParameterIUIV(TextureUnit texunit, TextureTarget target,
                                  GetTextureParameter pname, GLuint *params) const;
    void getMultiTexParameterFV(TextureUnit texunit, TextureTarget target,
                                GetTextureParameter pname, GLfloat *params) const;
    void getMultiTexParameterIV(TextureUnit texunit, TextureTarget target,
                                GetTextureParameter pname, GLint *params) const;
    void getNamedBufferParameterIV(BufferName buffer, BufferPNameARB pname,
                                   GLint *params) const;
    void getNamedBufferPointerV(BufferName buffer, BufferPointerNameARB pname,
                                void **params) const;
    void getNamedBufferSubData(BufferName buffer, GLintptr offset,
                               GLsizeiptr size, void *data) const;
    void getNamedFramebufferAttachmentParameterIV(FrameBufferName framebuffer,
                                                  FramebufferAttachment attachment,
                                                  FramebufferAttachmentParameterName pname,
                                                  GLint *params) const;
    void getNamedFramebufferParameterIV(FrameBufferName framebuffer,
                                        GetFramebufferParameter pname,
                                        GLint *params) const;
    void getNamedProgramLocalParameterIIV(ProgramName program,
                                          ProgramTarget target, GLuint index,
                                          GLint *params) const;
    void getNamedProgramLocalParameterIUIV(ProgramName program,
                                           ProgramTarget target, GLuint index,
                                           GLuint *params) const;
    void getNamedProgramLocalParameterDV(ProgramName program,
                                         ProgramTarget target, GLuint index,
                                         GLdouble *params) const;
    void getNamedProgramLocalParameterFV(ProgramName program,
                                         ProgramTarget target, GLuint index,
                                         GLfloat *params) const;
    void getNamedProgramString(ProgramName program, ProgramTarget target,
                               ProgramStringProperty pname, void *string) const;
    void getNamedProgramIV(ProgramName program, ProgramTarget target,
                           ProgramProperty pname, GLint *params) const;
    void getNamedRenderbufferParameterIV(RenderBufferName renderbuffer,
                                         RenderBufferParameterName pname,
                                         GLint *params) const;
    void getPointerIndexedv(GLenum target, GLuint index, void **data) const;
    void getPointerIV(GLenum pname, GLuint index, void **params) const;
    void getTextureImage(TextureName texture, TextureTarget target, GLint level,
                         PixelFormat format, PixelType type, void *pixels) const;
    void getTextureLevelParameterFV(TextureName texture, TextureTarget target,
                                    GLint level, GetTextureParameter pname,
                                    GLfloat *params) const;
    void getTextureLevelParameterIV(TextureName texture, TextureTarget target,
                                    GLint level, GetTextureParameter pname,
                                    GLint *params) const;
    void getTextureParameterIIV(TextureName texture, TextureTarget target,
                                GetTextureParameter pname, GLint *params) const;
    void getTextureParameterIUIV(TextureName texture, TextureTarget target,
                                 GetTextureParameter pname, GLuint *params) const;
    void getTextureParameterFV(TextureName texture, TextureTarget target,
                               GetTextureParameter pname, GLfloat *params) const;
    void getTextureParameterIV(TextureName texture, TextureTarget target,
                               GetTextureParameter pname, GLint *params) const;
    void getVertexArrayIntegerIV(VertexArrayName vaobj, GLuint index,
                                 VertexArrayPName pname, GLint *param) const;
    void getVertexArrayIntegerV(VertexArrayName vaobj, VertexArrayPName pname,
                                GLint *param) const;
    void getVertexArrayPointerIV(VertexArrayName vaobj, GLuint index,
                                 VertexArrayPName pname, void **param) const;
    void getVertexArrayPointerV(VertexArrayName vaobj, VertexArrayPName pname,
                                void **param) const;
    Boolean isEnabledIndexed(EnableCap target, GLuint index) const;
    void *mapNamedBuffer(BufferName buffer, BufferAccessARB access) const;
    void *mapNamedBufferRange(BufferName buffer, GLintptr offset,
                              GLsizeiptr length,
                              GLbitfield  /* MapBufferAccessMask */ access) const;
    void matrixFrustum(MatrixMode mode, GLdouble left, GLdouble right,
                       GLdouble bottom, GLdouble top, GLdouble zNear,
                       GLdouble zFar) const;
    void matrixLoadIdentity(MatrixMode mode) const;
    void matrixLoadTransposeD(MatrixMode mode, const GLdouble *m) const;
    void matrixLoadTransposeF(MatrixMode mode, const GLfloat *m) const;
    void matrixLoadD(MatrixMode mode, const GLdouble *m) const;
    void matrixLoadF(MatrixMode mode, const GLfloat *m) const;
    void matrixMultTransposeD(MatrixMode mode, const GLdouble *m) const;
    void matrixMultTransposeF(MatrixMode mode, const GLfloat *m) const;
    void matrixMultD(MatrixMode mode, const GLdouble *m) const;
    void matrixMultF(MatrixMode mode, const GLfloat *m) const;
    void matrixOrtho(MatrixMode mode, GLdouble left, GLdouble right,
                     GLdouble bottom, GLdouble top, GLdouble zNear,
                     GLdouble zFar) const;
    void matrixPop(MatrixMode mode) const;
    void matrixPush(MatrixMode mode) const;
    void matrixRotateD(MatrixMode mode, GLdouble angle, GLdouble x, GLdouble y,
                       GLdouble z) const;
    void matrixRotateF(MatrixMode mode, GLfloat angle, GLfloat x, GLfloat y,
                       GLfloat z) const;
    void matrixScaleD(MatrixMode mode, GLdouble x, GLdouble y, GLdouble z) const;
    void matrixScaleF(MatrixMode mode, GLfloat x, GLfloat y, GLfloat z) const;
    void matrixTranslateD(MatrixMode mode, GLdouble x, GLdouble y, GLdouble z) const;
    void matrixTranslateF(MatrixMode mode, GLfloat x, GLfloat y, GLfloat z) const;
    void multiTexBuffer(TextureUnit texunit, TextureTarget target,
                        InternalFormat internalformat, BufferName buffer) const;
    void multiTexCoordPointer(TextureUnit texunit, GLint size,
                              TexCoordPointerType type, GLsizei stride,
                              const void *pointer) const;
    void multiTexEnvF(TextureUnit texunit, GLenum target, GLenum pname,
                      GLfloat param) const;
    void multiTexEnvFV(TextureUnit texunit, GLenum target, GLenum pname,
                       const GLfloat *params) const;
    void multiTexEnvI(TextureUnit texunit, GLenum target, GLenum pname,
                      GLint param) const;
    void multiTexEnvIV(TextureUnit texunit, GLenum target, GLenum pname,
                       const GLint *params) const;
    void multiTexGenD(TextureUnit texunit, GLenum coord,
                      TextureGenParameter pname, GLdouble param) const;
    void multiTexGenDV(TextureUnit texunit, GLenum coord,
                       TextureGenParameter pname, const GLdouble *params) const;
    void multiTexGenF(TextureUnit texunit, GLenum coord,
                      TextureGenParameter pname, GLfloat param) const;
    void multiTexGenFV(TextureUnit texunit, GLenum coord,
                       TextureGenParameter pname, const GLfloat *params) const;
    void multiTexGenI(TextureUnit texunit, GLenum coord,
                      TextureGenParameter pname, GLint param) const;
    void multiTexGenIV(TextureUnit texunit, GLenum coord,
                       TextureGenParameter pname, const GLint *params) const;
    void multiTexImage1D(TextureUnit texunit, TextureTarget target, GLint level,
                         InternalFormat internalformat, GLsizei width,
                         GLint border, PixelFormat format, PixelType type,
                         const void *pixels) const;
    void multiTexImage2D(TextureUnit texunit, TextureTarget target, GLint level,
                         InternalFormat internalformat, GLsizei width,
                         GLsizei height, GLint border, PixelFormat format,
                         PixelType type, const void *pixels) const;
    void multiTexImage3D(TextureUnit texunit, TextureTarget target, GLint level,
                         InternalFormat internalformat, GLsizei width,
                         GLsizei height, GLsizei depth, GLint border,
                         PixelFormat format, PixelType type, const void *pixels) const;
    void multiTexParameterIIV(TextureUnit texunit, TextureTarget target,
                              TextureParameterName pname, const GLint *params) const;
    void multiTexParameterIUIV(TextureUnit texunit, TextureTarget target,
                               TextureParameterName pname, const GLuint *params) const;
    void multiTexParameterF(TextureUnit texunit, TextureTarget target,
                            TextureParameterName pname, GLfloat param) const;
    void multiTexParameterFV(TextureUnit texunit, TextureTarget target,
                             TextureParameterName pname, const GLfloat *params) const;
    void multiTexParameterI(TextureUnit texunit, TextureTarget target,
                            TextureParameterName pname, GLint param) const;
    void multiTexParameterIV(TextureUnit texunit, TextureTarget target,
                             TextureParameterName pname, const GLint *params) const;
    void multiTexRenderbuffer(TextureUnit texunit, TextureTarget target,
                              RenderBufferName renderbuffer) const;
    void multiTexSubImage1D(TextureUnit texunit, TextureTarget target,
                            GLint level, GLint xoffset, GLsizei width,
                            PixelFormat format, PixelType type,
                            const void *pixels) const;
    void multiTexSubImage2D(TextureUnit texunit, TextureTarget target,
                            GLint level, GLint xoffset, GLint yoffset,
                            GLsizei width, GLsizei height, PixelFormat format,
                            PixelType type, const void *pixels) const;
    void multiTexSubImage3D(TextureUnit texunit, TextureTarget target,
                            GLint level, GLint xoffset, GLint yoffset,
                            GLint zoffset, GLsizei width, GLsizei height,
                            GLsizei depth, PixelFormat format, PixelType type,
                            const void *pixels) const;
    void namedBufferData(BufferName buffer, GLsizeiptr size, const void *data,
                         VertexBufferObjectUsage usage) const;
    void namedBufferStorage(BufferName buffer, GLsizeiptr size, const void *data,
                            GLbitfield  /* BufferStorageMask */ flags) const;
    void namedBufferSubData(BufferName buffer, GLintptr offset, GLsizeiptr size,
                            const void *data) const;
    void namedCopyBufferSubData(BufferName readBuffer, BufferName writeBuffer,
                                GLintptr readOffset, GLintptr writeOffset,
                                GLsizeiptr size) const;
    void namedFramebufferParameterI(FrameBufferName framebuffer,
                                    FramebufferParameterName pname, GLint param) const;
    void namedFramebufferRenderbuffer(FrameBufferName framebuffer,
                                      FramebufferAttachment attachment,
                                      RenderBufferTarget renderbuffertarget,
                                      RenderBufferName renderbuffer) const;
    void namedFramebufferTexture1D(FrameBufferName framebuffer,
                                   FramebufferAttachment attachment,
                                   TextureTarget textarget, TextureName texture,
                                   GLint level) const;
    void namedFramebufferTexture2D(FrameBufferName framebuffer,
                                   FramebufferAttachment attachment,
                                   TextureTarget textarget, TextureName texture,
                                   GLint level) const;
    void namedFramebufferTexture3D(FrameBufferName framebuffer,
                                   FramebufferAttachment attachment,
                                   TextureTarget textarget, TextureName texture,
                                   GLint level, GLint zoffset) const;
    void namedFramebufferTexture(FrameBufferName framebuffer,
                                 FramebufferAttachment attachment,
                                 TextureName texture, GLint level) const;
    void namedFramebufferTextureFace(FrameBufferName framebuffer,
                                     FramebufferAttachment attachment,
                                     TextureName texture, GLint level,
                                     TextureTarget face) const;
    void namedFramebufferTextureLayer(FrameBufferName framebuffer,
                                      FramebufferAttachment attachment,
                                      TextureName texture, GLint level,
                                      GLint layer) const;
    void namedProgramLocalParameter4D(ProgramName program, ProgramTarget target,
                                      GLuint index, GLdouble x, GLdouble y,
                                      GLdouble z, GLdouble w) const;
    void namedProgramLocalParameter4DV(ProgramName program, ProgramTarget target,
                                       GLuint index, const GLdouble *params) const;
    void namedProgramLocalParameter4F(ProgramName program, ProgramTarget target,
                                      GLuint index, GLfloat x, GLfloat y,
                                      GLfloat z, GLfloat w) const;
    void namedProgramLocalParameter4FV(ProgramName program, ProgramTarget target,
                                       GLuint index, const GLfloat *params) const;
    void namedProgramLocalParameterI4I(ProgramName program, ProgramTarget target,
                                       GLuint index, GLint x, GLint y, GLint z,
                                       GLint w) const;
    void namedProgramLocalParameterI4IV(ProgramName program,
                                        ProgramTarget target, GLuint index,
                                        const GLint *params) const;
    void namedProgramLocalParameterI4UI(ProgramName program,
                                        ProgramTarget target, GLuint index,
                                        GLuint x, GLuint y, GLuint z, GLuint w) const;
    void namedProgramLocalParameterI4UIV(ProgramName program,
                                         ProgramTarget target, GLuint index,
                                         const GLuint *params) const;
    void namedProgramLocalParameters4FV(ProgramName program,
                                        ProgramTarget target, GLuint index,
                                        GLsizei count, const GLfloat *params) const;
    void namedProgramLocalParametersI4IV(ProgramName program,
                                         ProgramTarget target, GLuint index,
                                         GLsizei count, const GLint *params) const;
    void namedProgramLocalParametersI4UIV(ProgramName program,
                                          ProgramTarget target, GLuint index,
                                          GLsizei count, const GLuint *params) const;
    void namedProgramString(ProgramName program, ProgramTarget target,
                            ProgramFormat format, GLsizei len,
                            const void *string) const;
    void namedRenderbufferStorage(RenderBufferName renderbuffer,
                                  InternalFormat internalformat, GLsizei width,
                                  GLsizei height) const;
    void namedRenderbufferStorageMultisampleCoverage(RenderBufferName renderbuffer,
                                                     GLsizei coverageSamples,
                                                     GLsizei colorSamples,
                                                     InternalFormat internalformat,
                                                     GLsizei width,
                                                     GLsizei height) const;
    void namedRenderbufferStorageMultisample(RenderBufferName renderbuffer,
                                             GLsizei samples,
                                             InternalFormat internalformat,
                                             GLsizei width, GLsizei height) const;
    void programUniform1D(ProgramName program, GLint location, GLdouble x) const;
    void programUniform1DV(ProgramName program, GLint location, GLsizei count,
                           const GLdouble *value) const;
    void programUniform1F(ProgramName program, GLint location, GLfloat v0) const;
    void programUniform1FV(ProgramName program, GLint location, GLsizei count,
                           const GLfloat *value) const;
    void programUniform1I(ProgramName program, GLint location, GLint v0) const;
    void programUniform1IV(ProgramName program, GLint location, GLsizei count,
                           const GLint *value) const;
    void programUniform1UI(ProgramName program, GLint location, GLuint v0) const;
    void programUniform1UIV(ProgramName program, GLint location, GLsizei count,
                            const GLuint *value) const;
    void programUniform2D(ProgramName program, GLint location, GLdouble x,
                          GLdouble y) const;
    void programUniform2DV(ProgramName program, GLint location, GLsizei count,
                           const GLdouble *value) const;
    void programUniform2F(ProgramName program, GLint location, GLfloat v0,
                          GLfloat v1) const;
    void programUniform2FV(ProgramName program, GLint location, GLsizei count,
                           const GLfloat *value) const;
    void programUniform2I(ProgramName program, GLint location, GLint v0,
                          GLint v1) const;
    void programUniform2IV(ProgramName program, GLint location, GLsizei count,
                           const GLint *value) const;
    void programUniform2UI(ProgramName program, GLint location, GLuint v0,
                           GLuint v1) const;
    void programUniform2UIV(ProgramName program, GLint location, GLsizei count,
                            const GLuint *value) const;
    void programUniform3D(ProgramName program, GLint location, GLdouble x,
                          GLdouble y, GLdouble z) const;
    void programUniform3DV(ProgramName program, GLint location, GLsizei count,
                           const GLdouble *value) const;
    void programUniform3F(ProgramName program, GLint location, GLfloat v0,
                          GLfloat v1, GLfloat v2) const;
    void programUniform3FV(ProgramName program, GLint location, GLsizei count,
                           const GLfloat *value) const;
    void programUniform3I(ProgramName program, GLint location, GLint v0,
                          GLint v1, GLint v2) const;
    void programUniform3IV(ProgramName program, GLint location, GLsizei count,
                           const GLint *value) const;
    void programUniform3UI(ProgramName program, GLint location, GLuint v0,
                           GLuint v1, GLuint v2) const;
    void programUniform3UIV(ProgramName program, GLint location, GLsizei count,
                            const GLuint *value) const;
    void programUniform4D(ProgramName program, GLint location, GLdouble x,
                          GLdouble y, GLdouble z, GLdouble w) const;
    void programUniform4DV(ProgramName program, GLint location, GLsizei count,
                           const GLdouble *value) const;
    void programUniform4F(ProgramName program, GLint location, GLfloat v0,
                          GLfloat v1, GLfloat v2, GLfloat v3) const;
    void programUniform4FV(ProgramName program, GLint location, GLsizei count,
                           const GLfloat *value) const;
    void programUniform4I(ProgramName program, GLint location, GLint v0,
                          GLint v1, GLint v2, GLint v3) const;
    void programUniform4IV(ProgramName program, GLint location, GLsizei count,
                           const GLint *value) const;
    void programUniform4UI(ProgramName program, GLint location, GLuint v0,
                           GLuint v1, GLuint v2, GLuint v3) const;
    void programUniform4UIV(ProgramName program, GLint location, GLsizei count,
                            const GLuint *value) const;
    void programUniformMatrix2DV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLdouble *value) const;
    void programUniformMatrix2FV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLfloat *value) const;
    void programUniformMatrix2x3DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix2x3FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix2x4DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix2x4FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix3DV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLdouble *value) const;
    void programUniformMatrix3FV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLfloat *value) const;
    void programUniformMatrix3x2DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix3x2FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix3x4DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix3x4FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix4DV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLdouble *value) const;
    void programUniformMatrix4FV(ProgramName program, GLint location,
                                 GLsizei count, Boolean transpose,
                                 const GLfloat *value) const;
    void programUniformMatrix4x2DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix4x2FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void programUniformMatrix4x3DV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLdouble *value) const;
    void programUniformMatrix4x3FV(ProgramName program, GLint location,
                                   GLsizei count, Boolean transpose,
                                   const GLfloat *value) const;
    void pushClientAttribDefault(GLbitfield mask) const;
    void textureBuffer(TextureName texture, TextureTarget target,
                       SizedInternalFormat internalformat, BufferName buffer) const;
    void textureBufferRange(TextureName texture, TextureTarget target,
                            SizedInternalFormat internalformat,
                            BufferName buffer, GLintptr offset, GLsizeiptr size) const;
    void textureImage1D(TextureName texture, TextureTarget target, GLint level,
                        InternalFormat internalformat, GLsizei width,
                        GLint border, PixelFormat format, PixelType type,
                        const void *pixels) const;
    void textureImage2D(TextureName texture, TextureTarget target, GLint level,
                        InternalFormat internalformat, GLsizei width,
                        GLsizei height, GLint border, PixelFormat format,
                        PixelType type, const void *pixels) const;
    void textureImage3D(TextureName texture, TextureTarget target, GLint level,
                        InternalFormat internalformat, GLsizei width,
                        GLsizei height, GLsizei depth, GLint border,
                        PixelFormat format, PixelType type, const void *pixels) const;
    void texturePageCommitment(TextureName texture, GLint level, GLint xoffset,
                               GLint yoffset, GLint zoffset, GLsizei width,
                               GLsizei height, GLsizei depth, Boolean commit) const;
    void textureParameterIIV(TextureName texture, TextureTarget target,
                             TextureParameterName pname, const GLint *params) const;
    void textureParameterIUIV(TextureName texture, TextureTarget target,
                              TextureParameterName pname, const GLuint *params) const;
    void textureParameterF(TextureName texture, TextureTarget target,
                           TextureParameterName pname, GLfloat param) const;
    void textureParameterFV(TextureName texture, TextureTarget target,
                            TextureParameterName pname, const GLfloat *params) const;
    void textureParameterI(TextureName texture, TextureTarget target,
                           TextureParameterName pname, GLint param) const;
    void textureParameterIV(TextureName texture, TextureTarget target,
                            TextureParameterName pname, const GLint *params) const;
    void textureRenderbuffer(TextureName texture, TextureTarget target,
                             RenderBufferName renderbuffer) const;
    void textureStorage1D(TextureName texture, GLenum target, GLsizei levels,
                          SizedInternalFormat internalformat, GLsizei width) const;
    void textureStorage2D(TextureName texture, GLenum target, GLsizei levels,
                          SizedInternalFormat internalformat, GLsizei width,
                          GLsizei height) const;
    void textureStorage2DMultisample(TextureName texture, TextureTarget target,
                                     GLsizei samples,
                                     SizedInternalFormat internalformat,
                                     GLsizei width, GLsizei height,
                                     Boolean fixedsamplelocations) const;
    void textureStorage3D(TextureName texture, GLenum target, GLsizei levels,
                          SizedInternalFormat internalformat, GLsizei width,
                          GLsizei height, GLsizei depth) const;
    void textureStorage3DMultisample(TextureName texture, GLenum target,
                                     GLsizei samples,
                                     SizedInternalFormat internalformat,
                                     GLsizei width, GLsizei height,
                                     GLsizei depth, Boolean fixedsamplelocations) const;
    void textureSubImage1D(TextureName texture, TextureTarget target,
                           GLint level, GLint xoffset, GLsizei width,
                           PixelFormat format, PixelType type,
                           const void *pixels) const;
    void textureSubImage2D(TextureName texture, TextureTarget target,
                           GLint level, GLint xoffset, GLint yoffset,
                           GLsizei width, GLsizei height, PixelFormat format,
                           PixelType type, const void *pixels) const;
    void textureSubImage3D(TextureName texture, TextureTarget target,
                           GLint level, GLint xoffset, GLint yoffset,
                           GLint zoffset, GLsizei width, GLsizei height,
                           GLsizei depth, PixelFormat format, PixelType type,
                           const void *pixels) const;
    Boolean unmapNamedBuffer(BufferName buffer) const;
    void vertexArrayBindVertexBuffer(VertexArrayName vaobj, GLuint bindingindex,
                                     BufferName buffer, GLintptr offset,
                                     GLsizei stride) const;
    void vertexArrayColorOffset(VertexArrayName vaobj, BufferName buffer,
                                GLint size, ColorPointerType type,
                                GLsizei stride, GLintptr offset) const;
    void vertexArrayEdgeFlagOffset(VertexArrayName vaobj, BufferName buffer,
                                   GLsizei stride, GLintptr offset) const;
    void vertexArrayFogCoordOffset(VertexArrayName vaobj, BufferName buffer,
                                   FogCoordinatePointerType type, GLsizei stride,
                                   GLintptr offset) const;
    void vertexArrayIndexOffset(VertexArrayName vaobj, BufferName buffer,
                                IndexPointerType type, GLsizei stride,
                                GLintptr offset) const;
    void vertexArrayMultiTexCoordOffset(VertexArrayName vaobj, BufferName buffer,
                                        GLenum texunit, GLint size,
                                        TexCoordPointerType type, GLsizei stride,
                                        GLintptr offset) const;
    void vertexArrayNormalOffset(VertexArrayName vaobj, BufferName buffer,
                                 NormalPointerType type, GLsizei stride,
                                 GLintptr offset) const;
    void vertexArraySecondaryColorOffset(VertexArrayName vaobj,
                                         BufferName buffer, GLint size,
                                         ColorPointerType type, GLsizei stride,
                                         GLintptr offset) const;
    void vertexArrayTexCoordOffset(VertexArrayName vaobj, BufferName buffer,
                                   GLint size, TexCoordPointerType type,
                                   GLsizei stride, GLintptr offset) const;
    void vertexArrayVertexAttribBinding(VertexArrayName vaobj,
                                        GLuint attribindex, GLuint bindingindex) const;
    void vertexArrayVertexAttribDivisor(VertexArrayName vaobj, GLuint index,
                                        GLuint divisor) const;
    void vertexArrayVertexAttribFormat(VertexArrayName vaobj, GLuint attribindex,
                                       GLint size, VertexAttribType type,
                                       Boolean normalized, GLuint relativeoffset) const;
    void vertexArrayVertexAttribIFormat(VertexArrayName vaobj,
                                        GLuint attribindex, GLint size,
                                        VertexAttribIType type,
                                        GLuint relativeoffset) const;
    void vertexArrayVertexAttribIOffset(VertexArrayName vaobj, BufferName buffer,
                                        GLuint index, GLint size,
                                        VertexAttribType type, GLsizei stride,
                                        GLintptr offset) const;
    void vertexArrayVertexAttribLFormat(VertexArrayName vaobj,
                                        GLuint attribindex, GLint size,
                                        VertexAttribLType type,
                                        GLuint relativeoffset) const;
    void vertexArrayVertexAttribLOffset(VertexArrayName vaobj, BufferName buffer,
                                        GLuint index, GLint size,
                                        VertexAttribLType type, GLsizei stride,
                                        GLintptr offset) const;
    void vertexArrayVertexAttribOffset(VertexArrayName vaobj, BufferName buffer,
                                       GLuint index, GLint size,
                                       VertexAttribPointerType type,
                                       Boolean normalized, GLsizei stride,
                                       GLintptr offset) const;
    void vertexArrayVertexBindingDivisor(VertexArrayName vaobj,
                                         GLuint bindingindex, GLuint divisor) const;
    void vertexArrayVertexOffset(VertexArrayName vaobj, BufferName buffer,
                                 GLint size, VertexPointerType type,
                                 GLsizei stride, GLintptr offset) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTDirectStateAccess _api;
};

// Manages the entry points for the GL_EXT_draw_buffers2 extension API.
class EXTDrawBUFFERS2API : public BaseExtension
{
public:
    // Construction
    EXTDrawBUFFERS2API();

    // Accessors
    const EXTDrawBUFFERS2 &getRawAPI() const;

    // Operations
    void colorMaskIndexed(GLuint index, Boolean r, Boolean g, Boolean b,
                          Boolean a) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTDrawBUFFERS2 _api;
};

// Manages the entry points for the GL_EXT_draw_instanced extension API.
class EXTDrawInstancedAPI : public BaseExtension
{
public:
    // Construction
    EXTDrawInstancedAPI();

    // Accessors
    const EXTDrawInstanced &getRawAPI() const;

    // Operations
    void drawArraysInstanced(PrimitiveType mode, GLint start, GLsizei count,
                             GLsizei primcount) const;
    void drawElementsInstanced(PrimitiveType mode, GLsizei count,
                               DrawElementsType type, const void *indices,
                               GLsizei primcount) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTDrawInstanced _api;
};

// Manages the entry points for the GL_EXT_draw_range_elements extension API.
class EXTDrawRangeElementsAPI : public BaseExtension
{
public:
    // Construction
    EXTDrawRangeElementsAPI();

    // Accessors
    const EXTDrawRangeElements &getRawAPI() const;

    // Operations
    void drawRangeElements(PrimitiveType mode, GLuint start, GLuint end,
                           GLsizei count, DrawElementsType type,
                           const void *indices) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTDrawRangeElements _api;
};

// Manages the entry points for the GL_EXT_external_buffer extension API.
class EXTExternalBufferAPI : public BaseExtension
{
public:
    // Construction
    EXTExternalBufferAPI();

    // Accessors
    const EXTExternalBuffer &getRawAPI() const;

    // Operations
    void bufferStorageExternal(GLenum target, GLintptr offset, GLsizeiptr size,
                               GLeglClientBufferEXT clientBuffer,
                               GLbitfield  /* BufferStorageMask */ flags) const;
    void namedBufferStorageExternal(BufferName buffer, GLintptr offset,
                                    GLsizeiptr size,
                                    GLeglClientBufferEXT clientBuffer,
                                    GLbitfield  /* BufferStorageMask */ flags) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTExternalBuffer _api;
};

// Manages the entry points for the GL_EXT_fog_coord extension API.
class EXTFogCoordAPI : public BaseExtension
{
public:
    // Construction
    EXTFogCoordAPI();

    // Accessors
    const EXTFogCoord &getRawAPI() const;

    // Operations
    void fogCoordPointer(FogPointerTypeEXT type, GLsizei stride,
                         const void *pointer) const;
    void fogCoordD(GLdouble coord) const;
    void fogCoordDV(const GLdouble *coord) const;
    void fogCoordF(GLfloat coord) const;
    void fogCoordFV(const GLfloat *coord) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTFogCoord _api;
};

// Manages the entry points for the GL_EXT_framebuffer_blit extension API.
class EXTFramebufferBlitAPI : public BaseExtension
{
public:
    // Construction
    EXTFramebufferBlitAPI();

    // Accessors
    const EXTFramebufferBlit &getRawAPI() const;

    // Operations
    void blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                         GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                         GLbitfield  /* ClearBufferMask */ mask,
                         BlitFramebufferFilter filter) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTFramebufferBlit _api;
};

// Manages the entry points for the GL_EXT_framebuffer_multisample extension API.
class EXTFramebufferMultisampleAPI : public BaseExtension
{
public:
    // Construction
    EXTFramebufferMultisampleAPI();

    // Accessors
    const EXTFramebufferMultisample &getRawAPI() const;

    // Operations
    void renderbufferStorageMultisample(RenderBufferTarget target,
                                        GLsizei samples,
                                        InternalFormat internalformat,
                                        GLsizei width, GLsizei height) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTFramebufferMultisample _api;
};

// Manages the entry points for the GL_EXT_framebuffer_object extension API.
class EXTFramebufferObjectAPI : public BaseExtension
{
public:
    // Construction
    EXTFramebufferObjectAPI();

    // Accessors
    const EXTFramebufferObject &getRawAPI() const;

    // Operations
    void bindFramebuffer(FramebufferTarget target, FrameBufferName framebuffer) const;
    void bindRenderbuffer(RenderBufferTarget target,
                          RenderBufferName renderbuffer) const;
    FramebufferStatus checkFramebufferStatus(FramebufferTarget target) const;
    void deleteFramebuffers(GLsizei n, const FrameBufferName *framebuffers) const;
    void deleteRenderbuffers(GLsizei n, const RenderBufferName *renderbuffers) const;
    void framebufferRenderbuffer(FramebufferTarget target,
                                 FramebufferAttachment attachment,
                                 RenderBufferTarget renderbuffertarget,
                                 RenderBufferName renderbuffer) const;
    void framebufferTexture1D(FramebufferTarget target,
                              FramebufferAttachment attachment,
                              TextureTarget textarget, TextureName texture,
                              GLint level) const;
    void framebufferTexture2D(FramebufferTarget target,
                              FramebufferAttachment attachment,
                              TextureTarget textarget, TextureName texture,
                              GLint level) const;
    void framebufferTexture3D(FramebufferTarget target,
                              FramebufferAttachment attachment,
                              TextureTarget textarget, TextureName texture,
                              GLint level, GLint zoffset) const;
    void genFramebuffers(GLsizei n, FrameBufferName *framebuffers) const;
    void genRenderbuffers(GLsizei n, RenderBufferName *renderbuffers) const;
    void generateMipmap(TextureTarget target) const;
    void getFramebufferAttachmentParameterIV(FramebufferTarget target,
                                             FramebufferAttachment attachment,
                                             FramebufferAttachmentParameterName pname,
                                             GLint *params) const;
    void getRenderbufferParameterIV(RenderBufferTarget target,
                                    RenderBufferParameterName pname,
                                    GLint *params) const;
    Boolean isFramebuffer(FrameBufferName framebuffer) const;
    Boolean isRenderbuffer(RenderBufferName renderbuffer) const;
    void renderbufferStorage(RenderBufferTarget target,
                             InternalFormat internalformat, GLsizei width,
                             GLsizei height) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTFramebufferObject _api;
};

// Manages the entry points for the GL_EXT_geometry_shader4 extension API.
class EXTGeometrySHADER4API : public BaseExtension
{
public:
    // Construction
    EXTGeometrySHADER4API();

    // Accessors
    const EXTGeometrySHADER4 &getRawAPI() const;

    // Operations
    void programParameterI(ProgramName program, ProgramParameterPName pname,
                           GLint value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTGeometrySHADER4 _api;
};

// Manages the entry points for the GL_EXT_gpu_program_parameters extension API.
class EXTGPUProgramParametersAPI : public BaseExtension
{
public:
    // Construction
    EXTGPUProgramParametersAPI();

    // Accessors
    const EXTGPUProgramParameters &getRawAPI() const;

    // Operations
    void programEnvParameters4FV(ProgramTarget target, GLuint index,
                                 GLsizei count, const GLfloat *params) const;
    void programLocalParameters4FV(ProgramTarget target, GLuint index,
                                   GLsizei count, const GLfloat *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTGPUProgramParameters _api;
};

// Manages the entry points for the GL_EXT_gpu_shader4 extension API.
class EXTGPUSHADER4API : public BaseExtension
{
public:
    // Construction
    EXTGPUSHADER4API();

    // Accessors
    const EXTGPUSHADER4 &getRawAPI() const;

    // Operations
    void bindFragDataLocation(ProgramName program, GLuint color,
                              const GLchar *name) const;
    GLint getFragDataLocation(ProgramName program, const GLchar *name) const;
    void getUniformUIV(ProgramName program, GLint location, GLuint *params) const;
    void getVertexAttribIIV(GLuint index, VertexAttribEnum pname, GLint *params) const;
    void getVertexAttribIUIV(GLuint index, VertexAttribEnum pname,
                             GLuint *params) const;
    void uniform1UI(GLint location, GLuint v0) const;
    void uniform1UIV(GLint location, GLsizei count, const GLuint *value) const;
    void uniform2UI(GLint location, GLuint v0, GLuint v1) const;
    void uniform2UIV(GLint location, GLsizei count, const GLuint *value) const;
    void uniform3UI(GLint location, GLuint v0, GLuint v1, GLuint v2) const;
    void uniform3UIV(GLint location, GLsizei count, const GLuint *value) const;
    void uniform4UI(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) const;
    void uniform4UIV(GLint location, GLsizei count, const GLuint *value) const;
    void vertexAttribI1I(GLuint index, GLint x) const;
    void vertexAttribI1IV(GLuint index, const GLint *v) const;
    void vertexAttribI1UI(GLuint index, GLuint x) const;
    void vertexAttribI1UIV(GLuint index, const GLuint *v) const;
    void vertexAttribI2I(GLuint index, GLint x, GLint y) const;
    void vertexAttribI2IV(GLuint index, const GLint *v) const;
    void vertexAttribI2UI(GLuint index, GLuint x, GLuint y) const;
    void vertexAttribI2UIV(GLuint index, const GLuint *v) const;
    void vertexAttribI3I(GLuint index, GLint x, GLint y, GLint z) const;
    void vertexAttribI3IV(GLuint index, const GLint *v) const;
    void vertexAttribI3UI(GLuint index, GLuint x, GLuint y, GLuint z) const;
    void vertexAttribI3UIV(GLuint index, const GLuint *v) const;
    void vertexAttribI4BV(GLuint index, const GLbyte *v) const;
    void vertexAttribI4I(GLuint index, GLint x, GLint y, GLint z, GLint w) const;
    void vertexAttribI4IV(GLuint index, const GLint *v) const;
    void vertexAttribI4SV(GLuint index, const GLshort *v) const;
    void vertexAttribI4UBV(GLuint index, const GLubyte *v) const;
    void vertexAttribI4UI(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) const;
    void vertexAttribI4UIV(GLuint index, const GLuint *v) const;
    void vertexAttribI4USV(GLuint index, const GLushort *v) const;
    void vertexAttribIPointer(GLuint index, GLint size, VertexAttribIType type,
                              GLsizei stride, const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTGPUSHADER4 _api;
};

// Manages the entry points for the GL_EXT_histogram extension API.
class EXTHistogramAPI : public BaseExtension
{
public:
    // Construction
    EXTHistogramAPI();

    // Accessors
    const EXTHistogram &getRawAPI() const;

    // Operations
    void getHistogram(HistogramTargetEXT target, Boolean reset,
                      PixelFormat format, PixelType type, void *values) const;
    void getHistogramParameterFV(HistogramTargetEXT target,
                                 GetHistogramParameterPNameEXT pname,
                                 GLfloat *params) const;
    void getHistogramParameterIV(HistogramTargetEXT target,
                                 GetHistogramParameterPNameEXT pname,
                                 GLint *params) const;
    void getMinmax(MinmaxTargetEXT target, Boolean reset, PixelFormat format,
                   PixelType type, void *values) const;
    void getMinmaxParameterFV(MinmaxTargetEXT target,
                              GetMinmaxParameterPNameEXT pname, GLfloat *params) const;
    void getMinmaxParameterIV(MinmaxTargetEXT target,
                              GetMinmaxParameterPNameEXT pname, GLint *params) const;
    void histogram(HistogramTargetEXT target, GLsizei width,
                   InternalFormat internalformat, Boolean sink) const;
    void minmax(MinmaxTargetEXT target, InternalFormat internalformat,
                Boolean sink) const;
    void resetHistogram(HistogramTargetEXT target) const;
    void resetMinmax(MinmaxTargetEXT target) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTHistogram _api;
};

// Manages the entry points for the GL_EXT_index_func extension API.
class EXTIndexFuncAPI : public BaseExtension
{
public:
    // Construction
    EXTIndexFuncAPI();

    // Accessors
    const EXTIndexFunc &getRawAPI() const;

    // Operations
    void indexFunc(IndexFunctionEXT func, GLclampf ref) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTIndexFunc _api;
};

// Manages the entry points for the GL_EXT_index_material extension API.
class EXTIndexMaterialAPI : public BaseExtension
{
public:
    // Construction
    EXTIndexMaterialAPI();

    // Accessors
    const EXTIndexMaterial &getRawAPI() const;

    // Operations
    void indexMaterial(MaterialFace face, GLenum mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTIndexMaterial _api;
};

// Manages the entry points for the GL_EXT_light_texture extension API.
class EXTLightTextureAPI : public BaseExtension
{
public:
    // Construction
    EXTLightTextureAPI();

    // Accessors
    const EXTLightTexture &getRawAPI() const;

    // Operations
    void applyTexture(LightTextureModeEXT mode) const;
    void textureLight(LightTexturePNameEXT pname) const;
    void textureMaterial(MaterialFace face, GLenum mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTLightTexture _api;
};

// Manages the entry points for the GL_EXT_memory_object extension API.
class EXTMemoryObjectAPI : public BaseExtension
{
public:
    // Construction
    EXTMemoryObjectAPI();

    // Accessors
    const EXTMemoryObject &getRawAPI() const;

    // Operations
    void bufferStorageMem(BufferTarget target, GLsizeiptr size, GLuint memory,
                          GLuint64 offset) const;
    void createMemoryObjects(GLsizei n, GLuint *memoryObjects) const;
    void deleteMemoryObjects(GLsizei n, const GLuint *memoryObjects) const;
    void getMemoryObjectParameterIV(GLuint memoryObject,
                                    MemoryObjectParameterName pname,
                                    GLint *params) const;
    void getUnsignedByteIV(GLenum target, GLuint index, GLubyte *data) const;
    void getUnsignedByteV(GetPName pname, GLubyte *data) const;
    Boolean isMemoryObject(GLuint memoryObject) const;
    void memoryObjectParameterIV(GLuint memoryObject,
                                 MemoryObjectParameterName pname,
                                 const GLint *params) const;
    void namedBufferStorageMem(BufferName buffer, GLsizeiptr size, GLuint memory,
                               GLuint64 offset) const;
    void texStorageMem1D(TextureTarget target, GLsizei levels,
                         SizedInternalFormat internalFormat, GLsizei width,
                         GLuint memory, GLuint64 offset) const;
    void texStorageMem2D(TextureTarget target, GLsizei levels,
                         SizedInternalFormat internalFormat, GLsizei width,
                         GLsizei height, GLuint memory, GLuint64 offset) const;
    void texStorageMem2DMultisample(TextureTarget target, GLsizei samples,
                                    SizedInternalFormat internalFormat,
                                    GLsizei width, GLsizei height,
                                    Boolean fixedSampleLocations, GLuint memory,
                                    GLuint64 offset) const;
    void texStorageMem3D(TextureTarget target, GLsizei levels,
                         SizedInternalFormat internalFormat, GLsizei width,
                         GLsizei height, GLsizei depth, GLuint memory,
                         GLuint64 offset) const;
    void texStorageMem3DMultisample(TextureTarget target, GLsizei samples,
                                    SizedInternalFormat internalFormat,
                                    GLsizei width, GLsizei height, GLsizei depth,
                                    Boolean fixedSampleLocations, GLuint memory,
                                    GLuint64 offset) const;
    void textureStorageMem1D(TextureName texture, GLsizei levels,
                             SizedInternalFormat internalFormat, GLsizei width,
                             GLuint memory, GLuint64 offset) const;
    void textureStorageMem2D(TextureName texture, GLsizei levels,
                             SizedInternalFormat internalFormat, GLsizei width,
                             GLsizei height, GLuint memory, GLuint64 offset) const;
    void textureStorageMem2DMultisample(TextureName texture, GLsizei samples,
                                        SizedInternalFormat internalFormat,
                                        GLsizei width, GLsizei height,
                                        Boolean fixedSampleLocations,
                                        GLuint memory, GLuint64 offset) const;
    void textureStorageMem3D(TextureName texture, GLsizei levels,
                             SizedInternalFormat internalFormat, GLsizei width,
                             GLsizei height, GLsizei depth, GLuint memory,
                             GLuint64 offset) const;
    void textureStorageMem3DMultisample(TextureName texture, GLsizei samples,
                                        SizedInternalFormat internalFormat,
                                        GLsizei width, GLsizei height,
                                        GLsizei depth,
                                        Boolean fixedSampleLocations,
                                        GLuint memory, GLuint64 offset) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTMemoryObject _api;
};

// Manages the entry points for the GL_EXT_memory_object_fd extension API.
class EXTMemoryObjectFdAPI : public BaseExtension
{
public:
    // Construction
    EXTMemoryObjectFdAPI();

    // Accessors
    const EXTMemoryObjectFd &getRawAPI() const;

    // Operations
    void importMemoryFd(GLuint memory, GLuint64 size,
                        ExternalHandleType handleType, GLint fd) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTMemoryObjectFd _api;
};

// Manages the entry points for the GL_EXT_memory_object_win32 extension API.
class EXTMemoryObjectWIN32API : public BaseExtension
{
public:
    // Construction
    EXTMemoryObjectWIN32API();

    // Accessors
    const EXTMemoryObjectWIN32 &getRawAPI() const;

    // Operations
    void importMemoryWIN32Handle(GLuint memory, GLuint64 size,
                                 ExternalHandleType handleType, void *handle) const;
    void importMemoryWIN32Name(GLuint memory, GLuint64 size,
                               ExternalHandleType handleType, const void *name) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTMemoryObjectWIN32 _api;
};

// Manages the entry points for the GL_EXT_multi_draw_arrays extension API.
class EXTMultiDrawArraysAPI : public BaseExtension
{
public:
    // Construction
    EXTMultiDrawArraysAPI();

    // Accessors
    const EXTMultiDrawArrays &getRawAPI() const;

    // Operations
    void multiDrawArrays(PrimitiveType mode, const GLint *first,
                         const GLsizei *count, GLsizei primcount) const;
    void multiDrawElements(PrimitiveType mode, const GLsizei *count,
                           DrawElementsType type, const void *const*indices,
                           GLsizei primcount) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTMultiDrawArrays _api;
};

// Manages the entry points for the GL_EXT_multisample extension API.
class EXTMultisampleAPI : public BaseExtension
{
public:
    // Construction
    EXTMultisampleAPI();

    // Accessors
    const EXTMultisample &getRawAPI() const;

    // Operations
    void sampleMask(GLclampf value, Boolean invert) const;
    void samplePattern(SamplePatternEXT pattern) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTMultisample _api;
};

// Manages the entry points for the GL_EXT_paletted_texture extension API.
class EXTPalettedTextureAPI : public BaseExtension
{
public:
    // Construction
    EXTPalettedTextureAPI();

    // Accessors
    const EXTPalettedTexture &getRawAPI() const;

    // Operations
    void colorTable(GLenum target, InternalFormat internalFormat, GLsizei width,
                    PixelFormat format, PixelType type, const void *table) const;
    void getColorTable(GLenum target, PixelFormat format, PixelType type,
                       void *data) const;
    void getColorTableParameterFV(GLenum target,
                                  GetColorTableParameterPNameSGI pname,
                                  GLfloat *params) const;
    void getColorTableParameterIV(GLenum target,
                                  GetColorTableParameterPNameSGI pname,
                                  GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTPalettedTexture _api;
};

// Manages the entry points for the GL_EXT_pixel_transform extension API.
class EXTPixelTransformAPI : public BaseExtension
{
public:
    // Construction
    EXTPixelTransformAPI();

    // Accessors
    const EXTPixelTransform &getRawAPI() const;

    // Operations
    void getPixelTransformParameterFV(GLenum target, GLenum pname,
                                      GLfloat *params) const;
    void getPixelTransformParameterIV(GLenum target, GLenum pname, GLint *params) const;
    void pixelTransformParameterF(PixelTransformTargetEXT target,
                                  PixelTransformPNameEXT pname, GLfloat param) const;
    void pixelTransformParameterFV(PixelTransformTargetEXT target,
                                   PixelTransformPNameEXT pname,
                                   const GLfloat *params) const;
    void pixelTransformParameterI(PixelTransformTargetEXT target,
                                  PixelTransformPNameEXT pname, GLint param) const;
    void pixelTransformParameterIV(PixelTransformTargetEXT target,
                                   PixelTransformPNameEXT pname,
                                   const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTPixelTransform _api;
};

// Manages the entry points for the GL_EXT_point_parameters extension API.
class EXTPointParametersAPI : public BaseExtension
{
public:
    // Construction
    EXTPointParametersAPI();

    // Accessors
    const EXTPointParameters &getRawAPI() const;

    // Operations
    void pointParameterF(PointParameterNameARB pname, GLfloat param) const;
    void pointParameterFV(PointParameterNameARB pname, const GLfloat *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTPointParameters _api;
};

// Manages the entry points for the GL_EXT_polygon_offset extension API.
class EXTPolygonOffsetAPI : public BaseExtension
{
public:
    // Construction
    EXTPolygonOffsetAPI();

    // Accessors
    const EXTPolygonOffset &getRawAPI() const;

    // Operations
    void polygonOffset(GLfloat factor, GLfloat bias) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTPolygonOffset _api;
};

// Manages the entry points for the GL_EXT_polygon_offset_clamp extension API.
class EXTPolygonOffsetClampAPI : public BaseExtension
{
public:
    // Construction
    EXTPolygonOffsetClampAPI();

    // Accessors
    const EXTPolygonOffsetClamp &getRawAPI() const;

    // Operations
    void polygonOffsetClamp(GLfloat factor, GLfloat units, GLfloat clamp) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTPolygonOffsetClamp _api;
};

// Manages the entry points for the GL_EXT_provoking_vertex extension API.
class EXTProvokingVertexAPI : public BaseExtension
{
public:
    // Construction
    EXTProvokingVertexAPI();

    // Accessors
    const EXTProvokingVertex &getRawAPI() const;

    // Operations
    void provokingVertex(VertexProvokingMode mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTProvokingVertex _api;
};

// Manages the entry points for the GL_EXT_raster_multisample extension API.
class EXTRasterMultisampleAPI : public BaseExtension
{
public:
    // Construction
    EXTRasterMultisampleAPI();

    // Accessors
    const EXTRasterMultisample &getRawAPI() const;

    // Operations
    void rasterSamples(GLuint samples, Boolean fixedsamplelocations) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTRasterMultisample _api;
};

// Manages the entry points for the GL_EXT_secondary_color extension API.
class EXTSecondaryColorAPI : public BaseExtension
{
public:
    // Construction
    EXTSecondaryColorAPI();

    // Accessors
    const EXTSecondaryColor &getRawAPI() const;

    // Operations
    void secondaryColor3B(GLbyte red, GLbyte green, GLbyte blue) const;
    void secondaryColor3BV(const GLbyte *v) const;
    void secondaryColor3D(GLdouble red, GLdouble green, GLdouble blue) const;
    void secondaryColor3DV(const GLdouble *v) const;
    void secondaryColor3F(GLfloat red, GLfloat green, GLfloat blue) const;
    void secondaryColor3FV(const GLfloat *v) const;
    void secondaryColor3I(GLint red, GLint green, GLint blue) const;
    void secondaryColor3IV(const GLint *v) const;
    void secondaryColor3S(GLshort red, GLshort green, GLshort blue) const;
    void secondaryColor3SV(const GLshort *v) const;
    void secondaryColor3UB(GLubyte red, GLubyte green, GLubyte blue) const;
    void secondaryColor3UBV(const GLubyte *v) const;
    void secondaryColor3UI(GLuint red, GLuint green, GLuint blue) const;
    void secondaryColor3UIV(const GLuint *v) const;
    void secondaryColor3US(GLushort red, GLushort green, GLushort blue) const;
    void secondaryColor3USV(const GLushort *v) const;
    void secondaryColorPointer(GLint size, ColorPointerType type, GLsizei stride,
                               const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTSecondaryColor _api;
};

// Manages the entry points for the GL_EXT_semaphore extension API.
class EXTSemaphoreAPI : public BaseExtension
{
public:
    // Construction
    EXTSemaphoreAPI();

    // Accessors
    const EXTSemaphore &getRawAPI() const;

    // Operations
    void deleteSemaphores(GLsizei n, const GLuint *semaphores) const;
    void genSemaphores(GLsizei n, GLuint *semaphores) const;
    void getSemaphoreParameterUI64(GLuint semaphore,
                                   SemaphoreParameterName pname,
                                   GLuint64 *params) const;
    Boolean isSemaphore(GLuint semaphore) const;
    void semaphoreParameterUI64(GLuint semaphore, SemaphoreParameterName pname,
                                const GLuint64 *params) const;
    void signalSemaphore(GLuint semaphore, GLuint numBufferBarriers,
                         const BufferName *buffers, GLuint numTextureBarriers,
                         const TextureName *textures,
                         const TextureLayout *dstLayouts) const;
    void waitSemaphore(GLuint semaphore, GLuint numBufferBarriers,
                       const BufferName *buffers, GLuint numTextureBarriers,
                       const TextureName *textures,
                       const TextureLayout *srcLayouts) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTSemaphore _api;
};

// Manages the entry points for the GL_EXT_semaphore_fd extension API.
class EXTSemaphoreFdAPI : public BaseExtension
{
public:
    // Construction
    EXTSemaphoreFdAPI();

    // Accessors
    const EXTSemaphoreFd &getRawAPI() const;

    // Operations
    void importSemaphoreFd(GLuint semaphore, ExternalHandleType handleType,
                           GLint fd) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTSemaphoreFd _api;
};

// Manages the entry points for the GL_EXT_semaphore_win32 extension API.
class EXTSemaphoreWIN32API : public BaseExtension
{
public:
    // Construction
    EXTSemaphoreWIN32API();

    // Accessors
    const EXTSemaphoreWIN32 &getRawAPI() const;

    // Operations
    void importSemaphoreWIN32Handle(GLuint semaphore,
                                    ExternalHandleType handleType, void *handle) const;
    void importSemaphoreWIN32Name(GLuint semaphore,
                                  ExternalHandleType handleType,
                                  const void *name) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTSemaphoreWIN32 _api;
};

// Manages the entry points for the GL_EXT_separate_shader_objects extension API.
class EXTSeparateShaderObjectsAPI : public BaseExtension
{
public:
    // Construction
    EXTSeparateShaderObjectsAPI();

    // Accessors
    const EXTSeparateShaderObjects &getRawAPI() const;

    // Operations
    void activeProgram(ProgramName program) const;
    ProgramName createShaderProgram(ShaderType type, const GLchar *string) const;
    void useShaderProgram(GLenum type, ProgramName program) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTSeparateShaderObjects _api;
};

// Manages the entry points for the GL_EXT_shader_framebuffer_fetch_non_coherent extension API.
class EXTShaderFramebufferFetchNonCoherentAPI : public BaseExtension
{
public:
    // Construction
    EXTShaderFramebufferFetchNonCoherentAPI();

    // Accessors
    const EXTShaderFramebufferFetchNonCoherent &getRawAPI() const;

    // Operations
    void framebufferFetchBarrier() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTShaderFramebufferFetchNonCoherent _api;
};

// Manages the entry points for the GL_EXT_shader_image_load_store extension API.
class EXTShaderImageLoadStoreAPI : public BaseExtension
{
public:
    // Construction
    EXTShaderImageLoadStoreAPI();

    // Accessors
    const EXTShaderImageLoadStore &getRawAPI() const;

    // Operations
    void bindImageTexture(GLuint index, TextureName texture, GLint level,
                          Boolean layered, GLint layer, BufferAccessARB access,
                          GLint format) const;
    void memoryBarrier(GLbitfield  /* MemoryBarrierMask */ barriers) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTShaderImageLoadStore _api;
};

// Manages the entry points for the GL_EXT_stencil_clear_tag extension API.
class EXTStencilClearTagAPI : public BaseExtension
{
public:
    // Construction
    EXTStencilClearTagAPI();

    // Accessors
    const EXTStencilClearTag &getRawAPI() const;

    // Operations
    void stencilClearTag(GLsizei stencilTagBits, GLuint stencilClearTag) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTStencilClearTag _api;
};

// Manages the entry points for the GL_EXT_stencil_two_side extension API.
class EXTStencilTwoSideAPI : public BaseExtension
{
public:
    // Construction
    EXTStencilTwoSideAPI();

    // Accessors
    const EXTStencilTwoSide &getRawAPI() const;

    // Operations
    void activeStencilFace(StencilFaceDirection face) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTStencilTwoSide _api;
};

// Manages the entry points for the GL_EXT_subtexture extension API.
class EXTSubtextureAPI : public BaseExtension
{
public:
    // Construction
    EXTSubtextureAPI();

    // Accessors
    const EXTSubtexture &getRawAPI() const;

    // Operations
    void texSubImage1D(TextureTarget target, GLint level, GLint xoffset,
                       GLsizei width, PixelFormat format, PixelType type,
                       const void *pixels) const;
    void texSubImage2D(TextureTarget target, GLint level, GLint xoffset,
                       GLint yoffset, GLsizei width, GLsizei height,
                       PixelFormat format, PixelType type, const void *pixels) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTSubtexture _api;
};

// Manages the entry points for the GL_EXT_texture3D extension API.
class EXTTEXTURE3DAPI : public BaseExtension
{
public:
    // Construction
    EXTTEXTURE3DAPI();

    // Accessors
    const EXTTEXTURE3D &getRawAPI() const;

    // Operations
    void texImage3D(TextureTarget target, GLint level,
                    InternalFormat internalformat, GLsizei width, GLsizei height,
                    GLsizei depth, GLint border, PixelFormat format,
                    PixelType type, const void *pixels) const;
    void texSubImage3D(TextureTarget target, GLint level, GLint xoffset,
                       GLint yoffset, GLint zoffset, GLsizei width,
                       GLsizei height, GLsizei depth, PixelFormat format,
                       PixelType type, const void *pixels) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTEXTURE3D _api;
};

// Manages the entry points for the GL_EXT_texture_array extension API.
class EXTTextureArrayAPI : public BaseExtension
{
public:
    // Construction
    EXTTextureArrayAPI();

    // Accessors
    const EXTTextureArray &getRawAPI() const;

    // Operations
    void framebufferTextureLayer(FramebufferTarget target,
                                 FramebufferAttachment attachment,
                                 TextureName texture, GLint level, GLint layer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTextureArray _api;
};

// Manages the entry points for the GL_EXT_texture_buffer_object extension API.
class EXTTextureBufferObjectAPI : public BaseExtension
{
public:
    // Construction
    EXTTextureBufferObjectAPI();

    // Accessors
    const EXTTextureBufferObject &getRawAPI() const;

    // Operations
    void texBuffer(TextureTarget target, SizedInternalFormat internalformat,
                   BufferName buffer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTextureBufferObject _api;
};

// Manages the entry points for the GL_EXT_texture_integer extension API.
class EXTTextureIntegerAPI : public BaseExtension
{
public:
    // Construction
    EXTTextureIntegerAPI();

    // Accessors
    const EXTTextureInteger &getRawAPI() const;

    // Operations
    void clearColorII(GLint red, GLint green, GLint blue, GLint alpha) const;
    void clearColorIUI(GLuint red, GLuint green, GLuint blue, GLuint alpha) const;
    void getTexParameterIIV(TextureTarget target, GetTextureParameter pname,
                            GLint *params) const;
    void getTexParameterIUIV(TextureTarget target, GetTextureParameter pname,
                             GLuint *params) const;
    void texParameterIIV(TextureTarget target, TextureParameterName pname,
                         const GLint *params) const;
    void texParameterIUIV(TextureTarget target, TextureParameterName pname,
                          const GLuint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTextureInteger _api;
};

// Manages the entry points for the GL_EXT_texture_object extension API.
class EXTTextureObjectAPI : public BaseExtension
{
public:
    // Construction
    EXTTextureObjectAPI();

    // Accessors
    const EXTTextureObject &getRawAPI() const;

    // Operations
    Boolean areTexturesResident(GLsizei n, const TextureName *textures,
                                Boolean *residences) const;
    void bindTexture(TextureTarget target, TextureName texture) const;
    void deleteTextures(GLsizei n, const TextureName *textures) const;
    void genTextures(GLsizei n, TextureName *textures) const;
    Boolean isTexture(TextureName texture) const;
    void prioritizeTextures(GLsizei n, const TextureName *textures,
                            const GLclampf *priorities) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTextureObject _api;
};

// Manages the entry points for the GL_EXT_texture_perturb_normal extension API.
class EXTTexturePerturbNormalAPI : public BaseExtension
{
public:
    // Construction
    EXTTexturePerturbNormalAPI();

    // Accessors
    const EXTTexturePerturbNormal &getRawAPI() const;

    // Operations
    void textureNormal(TextureNormalModeEXT mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTexturePerturbNormal _api;
};

// Manages the entry points for the GL_EXT_texture_storage extension API.
class EXTTextureStorageAPI : public BaseExtension
{
public:
    // Construction
    EXTTextureStorageAPI();

    // Accessors
    const EXTTextureStorage &getRawAPI() const;

    // Operations
    void texStorage1D(TextureTarget target, GLsizei levels,
                      SizedInternalFormat internalformat, GLsizei width) const;
    void texStorage2D(TextureTarget target, GLsizei levels,
                      SizedInternalFormat internalformat, GLsizei width,
                      GLsizei height) const;
    void texStorage3D(TextureTarget target, GLsizei levels,
                      SizedInternalFormat internalformat, GLsizei width,
                      GLsizei height, GLsizei depth) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTextureStorage _api;
};

// Manages the entry points for the GL_EXT_timer_query extension API.
class EXTTimerQueryAPI : public BaseExtension
{
public:
    // Construction
    EXTTimerQueryAPI();

    // Accessors
    const EXTTimerQuery &getRawAPI() const;

    // Operations
    void getQueryObjectI64V(QueryName id, QueryObjectParameterName pname,
                            GLint64 *params) const;
    void getQueryObjectUI64(QueryName id, QueryObjectParameterName pname,
                            GLuint64 *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTimerQuery _api;
};

// Manages the entry points for the GL_EXT_transform_feedback extension API.
class EXTTransformFeedbackAPI : public BaseExtension
{
public:
    // Construction
    EXTTransformFeedbackAPI();

    // Accessors
    const EXTTransformFeedback &getRawAPI() const;

    // Operations
    void beginTransformFeedback(PrimitiveType primitiveMode) const;
    void bindBufferBase(BufferTarget target, GLuint index, BufferName buffer) const;
    void bindBufferOffset(BufferTarget target, GLuint index,
                          BufferName buffer, GLintptr offset) const;
    void bindBufferRange(BufferTarget target, GLuint index, BufferName buffer,
                         GLintptr offset, GLsizeiptr size) const;
    void endTransformFeedback() const;
    void getTransformFeedbackVarying(ProgramName program, GLuint index,
                                     GLsizei bufSize, GLsizei *length,
                                     GLsizei *size, AttributeType *type,
                                     GLchar *name) const;
    void transformFeedbackVaryings(ProgramName program, GLsizei count,
                                   const GLchar *const*varyings,
                                   GLenum bufferMode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTTransformFeedback _api;
};

// Manages the entry points for the GL_EXT_vertex_array extension API.
class EXTVertexArrayAPI : public BaseExtension
{
public:
    // Construction
    EXTVertexArrayAPI();

    // Accessors
    const EXTVertexArray &getRawAPI() const;

    // Operations
    void arrayElement(GLint i) const;
    void colorPointer(GLint size, ColorPointerType type, GLsizei stride,
                      GLsizei count, const void *pointer) const;
    void drawArrays(PrimitiveType mode, GLint first, GLsizei count) const;
    void edgeFlagPointer(GLsizei stride, GLsizei count, const Boolean *pointer) const;
    void getPointerV(GetPointervPName pname, void **params) const;
    void indexPointer(IndexPointerType type, GLsizei stride, GLsizei count,
                      const void *pointer) const;
    void normalPointer(NormalPointerType type, GLsizei stride, GLsizei count,
                       const void *pointer) const;
    void texCoordPointer(GLint size, TexCoordPointerType type, GLsizei stride,
                         GLsizei count, const void *pointer) const;
    void vertexPointer(GLint size, VertexPointerType type, GLsizei stride,
                       GLsizei count, const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTVertexArray _api;
};

// Manages the entry points for the GL_EXT_vertex_attrib_64bit extension API.
class EXTVertexAttrib64BITAPI : public BaseExtension
{
public:
    // Construction
    EXTVertexAttrib64BITAPI();

    // Accessors
    const EXTVertexAttrib64BIT &getRawAPI() const;

    // Operations
    void getVertexAttribLDV(GLuint index, VertexAttribEnum pname,
                            GLdouble *params) const;
    void vertexAttribL1D(GLuint index, GLdouble x) const;
    void vertexAttribL1DV(GLuint index, const GLdouble *v) const;
    void vertexAttribL2D(GLuint index, GLdouble x, GLdouble y) const;
    void vertexAttribL2DV(GLuint index, const GLdouble *v) const;
    void vertexAttribL3D(GLuint index, GLdouble x, GLdouble y, GLdouble z) const;
    void vertexAttribL3DV(GLuint index, const GLdouble *v) const;
    void vertexAttribL4D(GLuint index, GLdouble x, GLdouble y, GLdouble z,
                         GLdouble w) const;
    void vertexAttribL4DV(GLuint index, const GLdouble *v) const;
    void vertexAttribLPointer(GLuint index, GLint size, VertexAttribLType type,
                              GLsizei stride, const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTVertexAttrib64BIT _api;
};

// Manages the entry points for the GL_EXT_vertex_shader extension API.
class EXTVertexShaderAPI : public BaseExtension
{
public:
    // Construction
    EXTVertexShaderAPI();

    // Accessors
    const EXTVertexShader &getRawAPI() const;

    // Operations
    void beginVertexShader() const;
    GLuint bindLightParameter(LightName light, GLenum value) const;
    GLuint bindMaterialParameter(MaterialFace face, GLenum value) const;
    GLuint bindParameter(VertexShaderParameterEXT value) const;
    GLuint bindTexGenParameter(TextureUnit unit, GLenum coord,
                               TextureGenParameter value) const;
    GLuint bindTextureUnitParameter(TextureUnit unit, GLenum value) const;
    void bindVertexShader(GLuint id) const;
    void deleteVertexShader(GLuint id) const;
    void disableVariantClientState(GLuint id) const;
    void enableVariantClientState(GLuint id) const;
    void endVertexShader() const;
    void extractComponent(GLuint res, GLuint src, GLuint num) const;
    GLuint genSymbols(DataTypeEXT datatype,
                      VertexShaderStorageTypeEXT storagetype,
                      ParameterRangeEXT range, GLuint components) const;
    GLuint genVertexShaders(GLuint range) const;
    void getInvariantBooleanV(GLuint id, GetVariantValueEXT value, Boolean *data) const;
    void getInvariantFloatV(GLuint id, GetVariantValueEXT value, GLfloat *data) const;
    void getInvariantIntegerV(GLuint id, GetVariantValueEXT value, GLint *data) const;
    void getLocalConstantBooleanV(GLuint id, GetVariantValueEXT value,
                                  Boolean *data) const;
    void getLocalConstantFloatV(GLuint id, GetVariantValueEXT value,
                                GLfloat *data) const;
    void getLocalConstantIntegerV(GLuint id, GetVariantValueEXT value,
                                  GLint *data) const;
    void getVariantBooleanV(GLuint id, GetVariantValueEXT value, Boolean *data) const;
    void getVariantFloatV(GLuint id, GetVariantValueEXT value, GLfloat *data) const;
    void getVariantIntegerV(GLuint id, GetVariantValueEXT value, GLint *data) const;
    void getVariantPointerV(GLuint id, GetVariantValueEXT value, void **data) const;
    void insertComponent(GLuint res, GLuint src, GLuint num) const;
    Boolean isVariantEnabled(GLuint id, VariantCapEXT cap) const;
    void setInvariant(GLuint id, ScalarType type, const void *addr) const;
    void setLocalConstant(GLuint id, ScalarType type, const void *addr) const;
    void shaderOp1(VertexShaderOpEXT op, GLuint res, GLuint arg1) const;
    void shaderOp2(VertexShaderOpEXT op, GLuint res, GLuint arg1, GLuint arg2) const;
    void shaderOp3(VertexShaderOpEXT op, GLuint res, GLuint arg1, GLuint arg2,
                   GLuint arg3) const;
    void swizzle(GLuint res, GLuint in, VertexShaderCoordOutEXT outX,
                 VertexShaderCoordOutEXT outY, VertexShaderCoordOutEXT outZ,
                 VertexShaderCoordOutEXT outW) const;
    void variantPointer(GLuint id, ScalarType type, GLuint stride,
                        const void *addr) const;
    void variantBV(GLuint id, const GLbyte *addr) const;
    void variantDV(GLuint id, const GLdouble *addr) const;
    void variantFV(GLuint id, const GLfloat *addr) const;
    void variantIV(GLuint id, const GLint *addr) const;
    void variantSV(GLuint id, const GLshort *addr) const;
    void variantUBV(GLuint id, const GLubyte *addr) const;
    void variantUIV(GLuint id, const GLuint *addr) const;
    void variantUSV(GLuint id, const GLushort *addr) const;
    void writeMask(GLuint res, GLuint in, VertexShaderWriteMaskEXT outX,
                   VertexShaderWriteMaskEXT outY, VertexShaderWriteMaskEXT outZ,
                   VertexShaderWriteMaskEXT outW) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTVertexShader _api;
};

// Manages the entry points for the GL_EXT_vertex_weighting extension API.
class EXTVertexWeightingAPI : public BaseExtension
{
public:
    // Construction
    EXTVertexWeightingAPI();

    // Accessors
    const EXTVertexWeighting &getRawAPI() const;

    // Operations
    void vertexWeightPointer(GLint size, VertexWeightPointerTypeEXT type,
                             GLsizei stride, const void *pointer) const;
    void vertexWeightF(GLfloat weight) const;
    void vertexWeightFV(const GLfloat *weight) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTVertexWeighting _api;
};

// Manages the entry points for the GL_EXT_win32_keyed_mutex extension API.
class EXTWIN32KeyedMutexAPI : public BaseExtension
{
public:
    // Construction
    EXTWIN32KeyedMutexAPI();

    // Accessors
    const EXTWIN32KeyedMutex &getRawAPI() const;

    // Operations
    Boolean acquireKeyedMutexWIN32(GLuint memory, GLuint64 key, GLuint timeout) const;
    Boolean releaseKeyedMutexWIN32(GLuint memory, GLuint64 key) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTWIN32KeyedMutex _api;
};

// Manages the entry points for the GL_EXT_window_rectangles extension API.
class EXTWindowRectanglesAPI : public BaseExtension
{
public:
    // Construction
    EXTWindowRectanglesAPI();

    // Accessors
    const EXTWindowRectangles &getRawAPI() const;

    // Operations
    void windowRectangles(GLenum mode, GLsizei count, const GLint *box) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTWindowRectangles _api;
};

// Manages the entry points for the GL_EXT_x11_sync_object extension API.
class EXTX11SyncObjectAPI : public BaseExtension
{
public:
    // Construction
    EXTX11SyncObjectAPI();

    // Accessors
    const EXTX11SyncObject &getRawAPI() const;

    // Operations
    SyncName importSync(GLenum external_sync_type, GLintptr external_sync,
                        GLbitfield flags) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    EXTX11SyncObject _api;
};

// Manages the entry points for the GL_GREMEDY_frame_terminator extension API.
class GremedyFrameTerminatorAPI : public BaseExtension
{
public:
    // Construction
    GremedyFrameTerminatorAPI();

    // Accessors
    const GremedyFrameTerminator &getRawAPI() const;

    // Operations
    void frameTerminatorGremedy() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    GremedyFrameTerminator _api;
};

// Manages the entry points for the GL_GREMEDY_string_marker extension API.
class GremedyStringMarkerAPI : public BaseExtension
{
public:
    // Construction
    GremedyStringMarkerAPI();

    // Accessors
    const GremedyStringMarker &getRawAPI() const;

    // Operations
    void stringMarkerGremedy(GLsizei len, const void *string) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    GremedyStringMarker _api;
};

// Manages the entry points for the GL_HP_image_transform extension API.
class HPImageTransformAPI : public BaseExtension
{
public:
    // Construction
    HPImageTransformAPI();

    // Accessors
    const HPImageTransform &getRawAPI() const;

    // Operations
    void getImageTransformParameterFV(ImageTransformTargetHP target,
                                      ImageTransformPNameHP pname,
                                      GLfloat *params) const;
    void getImageTransformParameterIV(ImageTransformTargetHP target,
                                      ImageTransformPNameHP pname, GLint *params) const;
    void imageTransformParameterF(ImageTransformTargetHP target,
                                  ImageTransformPNameHP pname, GLfloat param) const;
    void imageTransformParameterFV(ImageTransformTargetHP target,
                                   ImageTransformPNameHP pname,
                                   const GLfloat *params) const;
    void imageTransformParameterI(ImageTransformTargetHP target,
                                  ImageTransformPNameHP pname, GLint param) const;
    void imageTransformParameterIV(ImageTransformTargetHP target,
                                   ImageTransformPNameHP pname,
                                   const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    HPImageTransform _api;
};

// Manages the entry points for the GL_IBM_multimode_draw_arrays extension API.
class IBMMultimodeDrawArraysAPI : public BaseExtension
{
public:
    // Construction
    IBMMultimodeDrawArraysAPI();

    // Accessors
    const IBMMultimodeDrawArrays &getRawAPI() const;

    // Operations
    void multiModeDrawArrays(const PrimitiveType *mode, const GLint *first,
                             const GLsizei *count, GLsizei primcount,
                             GLint modestride) const;
    void multiModeDrawElements(const PrimitiveType *mode, const GLsizei *count,
                               DrawElementsType type, const void *const*indices,
                               GLsizei primcount, GLint modestride) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    IBMMultimodeDrawArrays _api;
};

// Manages the entry points for the GL_IBM_static_data extension API.
class IBMStaticDataAPI : public BaseExtension
{
public:
    // Construction
    IBMStaticDataAPI();

    // Accessors
    const IBMStaticData &getRawAPI() const;

    // Operations
    void flushStaticData(GLenum target) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    IBMStaticData _api;
};

// Manages the entry points for the GL_IBM_vertex_array_lists extension API.
class IBMVertexArrayListsAPI : public BaseExtension
{
public:
    // Construction
    IBMVertexArrayListsAPI();

    // Accessors
    const IBMVertexArrayLists &getRawAPI() const;

    // Operations
    void colorPointerList(GLint size, ColorPointerType type, GLint stride,
                          const void **pointer, GLint ptrstride) const;
    void edgeFlagPointerList(GLint stride, const GLboolean **pointer,
                             GLint ptrstride) const;
    void fogCoordPointerList(FogPointerTypeIBM type, GLint stride,
                             const void **pointer, GLint ptrstride) const;
    void indexPointerList(IndexPointerType type, GLint stride,
                          const void **pointer, GLint ptrstride) const;
    void normalPointerList(NormalPointerType type, GLint stride,
                           const void **pointer, GLint ptrstride) const;
    void secondaryColorPointerList(GLint size, SecondaryColorPointerTypeIBM type,
                                   GLint stride, const void **pointer,
                                   GLint ptrstride) const;
    void texCoordPointerList(GLint size, TexCoordPointerType type, GLint stride,
                             const void **pointer, GLint ptrstride) const;
    void vertexPointerList(GLint size, VertexPointerType type, GLint stride,
                           const void **pointer, GLint ptrstride) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    IBMVertexArrayLists _api;
};

// Manages the entry points for the GL_INGR_blend_func_separate extension API.
class IngrBlendFuncSeparateAPI : public BaseExtension
{
public:
    // Construction
    IngrBlendFuncSeparateAPI();

    // Accessors
    const IngrBlendFuncSeparate &getRawAPI() const;

    // Operations
    void blendFuncSeparateIngr(BlendingFactor sfactorRGB,
                               BlendingFactor dfactorRGB,
                               BlendingFactor sfactorAlpha,
                               BlendingFactor dfactorAlpha) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    IngrBlendFuncSeparate _api;
};

// Manages the entry points for the GL_INTEL_framebuffer_CMAA extension API.
class INTELFramebufferCmaaAPI : public BaseExtension
{
public:
    // Construction
    INTELFramebufferCmaaAPI();

    // Accessors
    const INTELFramebufferCmaa &getRawAPI() const;

    // Operations
    void applyFramebufferAttachmentCmaa() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    INTELFramebufferCmaa _api;
};

// Manages the entry points for the GL_INTEL_map_texture extension API.
class INTELMapTextureAPI : public BaseExtension
{
public:
    // Construction
    INTELMapTextureAPI();

    // Accessors
    const INTELMapTexture &getRawAPI() const;

    // Operations
    void *mapTexture2D(TextureName texture, GLint level, GLbitfield access,
                       GLint *stride, GLenum *layout) const;
    void syncTexture(TextureName texture) const;
    void unmapTexture2D(TextureName texture, GLint level) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    INTELMapTexture _api;
};

// Manages the entry points for the GL_INTEL_parallel_arrays extension API.
class INTELParallelArraysAPI : public BaseExtension
{
public:
    // Construction
    INTELParallelArraysAPI();

    // Accessors
    const INTELParallelArrays &getRawAPI() const;

    // Operations
    void colorPointerV(GLint size, VertexPointerType type, const void **pointer) const;
    void normalPointerV(NormalPointerType type, const void **pointer) const;
    void texCoordPointerV(GLint size, VertexPointerType type,
                          const void **pointer) const;
    void vertexPointerV(GLint size, VertexPointerType type, const void **pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    INTELParallelArrays _api;
};

// Manages the entry points for the GL_INTEL_performance_query extension API.
class INTELPerformanceQueryAPI : public BaseExtension
{
public:
    // Construction
    INTELPerformanceQueryAPI();

    // Accessors
    const INTELPerformanceQuery &getRawAPI() const;

    // Operations
    void beginPerfQuery(GLuint queryHandle) const;
    void createPerfQuery(GLuint queryId, GLuint *queryHandle) const;
    void deletePerfQuery(GLuint queryHandle) const;
    void endPerfQuery(GLuint queryHandle) const;
    void getFirstPerfQueryID(GLuint *queryId) const;
    void getNextPerfQueryID(GLuint queryId, GLuint *nextQueryId) const;
    void getPerfCounterInfo(GLuint queryId, GLuint counterId,
                            GLuint counterNameLength, GLchar *counterName,
                            GLuint counterDescLength, GLchar *counterDesc,
                            GLuint *counterOffset, GLuint *counterDataSize,
                            GLuint *counterTypeEnum, GLuint *counterDataTypeEnum,
                            GLuint64 *rawCounterMaxValue) const;
    void getPerfQueryData(GLuint queryHandle, GLuint flags, GLsizei dataSize,
                          void *data, GLuint *bytesWritten) const;
    void getPerfQueryIDByName(GLchar *queryName, GLuint *queryId) const;
    void getPerfQueryInfo(GLuint queryId, GLuint queryNameLength,
                          GLchar *queryName, GLuint *dataSize,
                          GLuint *noCounters, GLuint *noInstances,
                          GLuint *capsMask) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    INTELPerformanceQuery _api;
};

// Manages the entry points for the GL_KHR_blend_equation_advanced extension API.
class KHRBlendEquationAdvancedAPI : public BaseExtension
{
public:
    // Construction
    KHRBlendEquationAdvancedAPI();

    // Accessors
    const KHRBlendEquationAdvanced &getRawAPI() const;

    // Operations
    void blendBarrier() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    KHRBlendEquationAdvanced _api;
};

// Manages the entry points for the GL_KHR_parallel_shader_compile extension API.
class KHRParallelShaderCompileAPI : public BaseExtension
{
public:
    // Construction
    KHRParallelShaderCompileAPI();

    // Accessors
    const KHRParallelShaderCompile &getRawAPI() const;

    // Operations
    void maxShaderCompilerThreads(GLuint count) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    KHRParallelShaderCompile _api;
};

// Manages the entry points for the GL_MESA_framebuffer_flip_y extension API.
class MESAFramebufferFlipYAPI : public BaseExtension
{
public:
    // Construction
    MESAFramebufferFlipYAPI();

    // Accessors
    const MESAFramebufferFlipY &getRawAPI() const;

    // Operations
    void framebufferParameterI(FramebufferTarget target,
                               FramebufferParameterName pname, GLint param) const;
    void getFramebufferParameterIV(FramebufferTarget target,
                                   FramebufferAttachmentParameterName pname,
                                   GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    MESAFramebufferFlipY _api;
};

// Manages the entry points for the GL_MESA_resize_buffers extension API.
class MESAResizeBuffersAPI : public BaseExtension
{
public:
    // Construction
    MESAResizeBuffersAPI();

    // Accessors
    const MESAResizeBuffers &getRawAPI() const;

    // Operations
    void resizeBuffers() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    MESAResizeBuffers _api;
};

// Manages the entry points for the GL_MESA_window_pos extension API.
class MESAWindowPosAPI : public BaseExtension
{
public:
    // Construction
    MESAWindowPosAPI();

    // Accessors
    const MESAWindowPos &getRawAPI() const;

    // Operations
    void windowPOS2D(GLdouble x, GLdouble y) const;
    void windowPOS2DV(const GLdouble *v) const;
    void windowPOS2F(GLfloat x, GLfloat y) const;
    void windowPOS2FV(const GLfloat *v) const;
    void windowPOS2I(GLint x, GLint y) const;
    void windowPOS2IV(const GLint *v) const;
    void windowPOS2S(GLshort x, GLshort y) const;
    void windowPOS2SV(const GLshort *v) const;
    void windowPOS3D(GLdouble x, GLdouble y, GLdouble z) const;
    void windowPOS3DV(const GLdouble *v) const;
    void windowPOS3F(GLfloat x, GLfloat y, GLfloat z) const;
    void windowPOS3FV(const GLfloat *v) const;
    void windowPOS3I(GLint x, GLint y, GLint z) const;
    void windowPOS3IV(const GLint *v) const;
    void windowPOS3S(GLshort x, GLshort y, GLshort z) const;
    void windowPOS3SV(const GLshort *v) const;
    void windowPOS4D(GLdouble x, GLdouble y, GLdouble z, GLdouble w) const;
    void windowPOS4DV(const GLdouble *v) const;
    void windowPOS4F(GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
    void windowPOS4FV(const GLfloat *v) const;
    void windowPOS4I(GLint x, GLint y, GLint z, GLint w) const;
    void windowPOS4IV(const GLint *v) const;
    void windowPOS4S(GLshort x, GLshort y, GLshort z, GLshort w) const;
    void windowPOS4SV(const GLshort *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    MESAWindowPos _api;
};

// Manages the entry points for the GL_NVX_conditional_render extension API.
class NVXConditionalRenderAPI : public BaseExtension
{
public:
    // Construction
    NVXConditionalRenderAPI();

    // Accessors
    const NVXConditionalRender &getRawAPI() const;

    // Operations
    void beginConditionalRender(GLuint id) const;
    void endConditionalRender() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVXConditionalRender _api;
};

// Manages the entry points for the GL_NVX_gpu_multicast2 extension API.
class NVXGPUMULTICAST2API : public BaseExtension
{
public:
    // Construction
    NVXGPUMULTICAST2API();

    // Accessors
    const NVXGPUMULTICAST2 &getRawAPI() const;

    // Operations
    GLuint asyncCopyBufferSubData(GLsizei waitSemaphoreCount,
                                  const GLuint *waitSemaphoreArray,
                                  const GLuint64 *fenceValueArray,
                                  GLuint readGpu, GLbitfield writeGpuMask,
                                  BufferName readBuffer, BufferName writeBuffer,
                                  GLintptr readOffset, GLintptr writeOffset,
                                  GLsizeiptr size, GLsizei signalSemaphoreCount,
                                  const GLuint *signalSemaphoreArray,
                                  const GLuint64 *signalValueArray) const;
    GLuint asyncCopyImageSubData(GLsizei waitSemaphoreCount,
                                 const GLuint *waitSemaphoreArray,
                                 const GLuint64 *waitValueArray, GLuint srcGpu,
                                 GLbitfield dstGpuMask, GLuint srcName,
                                 GLenum srcTarget, GLint srcLevel, GLint srcX,
                                 GLint srcY, GLint srcZ, GLuint dstName,
                                 GLenum dstTarget, GLint dstLevel, GLint dstX,
                                 GLint dstY, GLint dstZ, GLsizei srcWidth,
                                 GLsizei srcHeight, GLsizei srcDepth,
                                 GLsizei signalSemaphoreCount,
                                 const GLuint *signalSemaphoreArray,
                                 const GLuint64 *signalValueArray) const;
    void multicastScissorArrayV(GLuint gpu, GLuint first, GLsizei count,
                                const GLint *v) const;
    void multicastViewportArrayV(GLuint gpu, GLuint first, GLsizei count,
                                 const GLfloat *v) const;
    void multicastViewportPositionWScale(GLuint gpu, GLuint index,
                                         GLfloat xcoeff, GLfloat ycoeff) const;
    void uploadGpuMask(GLbitfield mask) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVXGPUMULTICAST2 _api;
};

// Manages the entry points for the GL_NVX_linked_gpu_multicast extension API.
class NVXLinkedGPUMulticastAPI : public BaseExtension
{
public:
    // Construction
    NVXLinkedGPUMulticastAPI();

    // Accessors
    const NVXLinkedGPUMulticast &getRawAPI() const;

    // Operations
    void lgpuCopyImageSubData(GLuint sourceGpu, GLbitfield destinationGpuMask,
                              GLuint srcName, GLenum srcTarget, GLint srcLevel,
                              GLint srcX, GLint srxY, GLint srcZ, GLuint dstName,
                              GLenum dstTarget, GLint dstLevel, GLint dstX,
                              GLint dstY, GLint dstZ, GLsizei width,
                              GLsizei height, GLsizei depth) const;
    void lgpuInterlock() const;
    void lgpuNamedBufferSubData(GLbitfield gpuMask, BufferName buffer,
                                GLintptr offset, GLsizeiptr size,
                                const void *data) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVXLinkedGPUMulticast _api;
};

// Manages the entry points for the GL_NVX_progress_fence extension API.
class NVXProgressFenceAPI : public BaseExtension
{
public:
    // Construction
    NVXProgressFenceAPI();

    // Accessors
    const NVXProgressFence &getRawAPI() const;

    // Operations
    void clientWaitSemaphoreUI64(GLsizei fenceObjectCount,
                                 const GLuint *semaphoreArray,
                                 const GLuint64 *fenceValueArray) const;
    GLuint createProgressFence() const;
    void signalSemaphoreUI64(GLuint signalGpu, GLsizei fenceObjectCount,
                             const GLuint *semaphoreArray,
                             const GLuint64 *fenceValueArray) const;
    void waitSemaphoreUI64(GLuint waitGpu, GLsizei fenceObjectCount,
                           const GLuint *semaphoreArray,
                           const GLuint64 *fenceValueArray) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVXProgressFence _api;
};

// Manages the entry points for the GL_NV_alpha_to_coverage_dither_control extension API.
class NVAlphaToCoverageDitherControlAPI : public BaseExtension
{
public:
    // Construction
    NVAlphaToCoverageDitherControlAPI();

    // Accessors
    const NVAlphaToCoverageDitherControl &getRawAPI() const;

    // Operations
    void alphaToCoverageDitherControl(GLenum mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVAlphaToCoverageDitherControl _api;
};

// Manages the entry points for the GL_NV_bindless_multi_draw_indirect extension API.
class NVBindlessMultiDrawIndirectAPI : public BaseExtension
{
public:
    // Construction
    NVBindlessMultiDrawIndirectAPI();

    // Accessors
    const NVBindlessMultiDrawIndirect &getRawAPI() const;

    // Operations
    void multiDrawArraysIndirectBindless(PrimitiveType mode,
                                         const void *indirect, GLsizei drawCount,
                                         GLsizei stride, GLint vertexBufferCount) const;
    void multiDrawElementsIndirectBindless(PrimitiveType mode,
                                           DrawElementsType type,
                                           const void *indirect,
                                           GLsizei drawCount, GLsizei stride,
                                           GLint vertexBufferCount) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVBindlessMultiDrawIndirect _api;
};

// Manages the entry points for the GL_NV_bindless_multi_draw_indirect_count extension API.
class NVBindlessMultiDrawIndirectCountAPI : public BaseExtension
{
public:
    // Construction
    NVBindlessMultiDrawIndirectCountAPI();

    // Accessors
    const NVBindlessMultiDrawIndirectCount &getRawAPI() const;

    // Operations
    void multiDrawArraysIndirectBindlessCount(PrimitiveType mode,
                                              const void *indirect,
                                              GLsizei drawCount,
                                              GLsizei maxDrawCount,
                                              GLsizei stride,
                                              GLint vertexBufferCount) const;
    void multiDrawElementsIndirectBindlessCount(PrimitiveType mode,
                                                DrawElementsType type,
                                                const void *indirect,
                                                GLsizei drawCount,
                                                GLsizei maxDrawCount,
                                                GLsizei stride,
                                                GLint vertexBufferCount) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVBindlessMultiDrawIndirectCount _api;
};

// Manages the entry points for the GL_NV_bindless_texture extension API.
class NVBindlessTextureAPI : public BaseExtension
{
public:
    // Construction
    NVBindlessTextureAPI();

    // Accessors
    const NVBindlessTexture &getRawAPI() const;

    // Operations
    GLuint64 getImageHandle(TextureName texture, GLint level, Boolean layered,
                            GLint layer, PixelFormat format) const;
    GLuint64 getTextureHandle(TextureName texture) const;
    GLuint64 getTextureSamplerHandle(TextureName texture, SamplerName sampler) const;
    Boolean isImageHandleResident(GLuint64 handle) const;
    Boolean isTextureHandleResident(GLuint64 handle) const;
    void makeImageHandleNonResident(GLuint64 handle) const;
    void makeImageHandleResident(GLuint64 handle, GLenum access) const;
    void makeTextureHandleNonResident(GLuint64 handle) const;
    void makeTextureHandleResident(GLuint64 handle) const;
    void programUniformHandleUI64(ProgramName program, GLint location,
                                  GLuint64 value) const;
    void programUniformHandleUI64(ProgramName program, GLint location,
                                  GLsizei count, const GLuint64 *values) const;
    void uniformHandleUI64(GLint location, GLuint64 value) const;
    void uniformHandleUI64(GLint location, GLsizei count, const GLuint64 *value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVBindlessTexture _api;
};

// Manages the entry points for the GL_NV_blend_equation_advanced extension API.
class NVBlendEquationAdvancedAPI : public BaseExtension
{
public:
    // Construction
    NVBlendEquationAdvancedAPI();

    // Accessors
    const NVBlendEquationAdvanced &getRawAPI() const;

    // Operations
    void blendBarrier() const;
    void blendParameterI(GLenum pname, GLint value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVBlendEquationAdvanced _api;
};

// Manages the entry points for the GL_NV_clip_space_w_scaling extension API.
class NVClipSpaceWScalingAPI : public BaseExtension
{
public:
    // Construction
    NVClipSpaceWScalingAPI();

    // Accessors
    const NVClipSpaceWScaling &getRawAPI() const;

    // Operations
    void viewportPositionWScale(GLuint index, GLfloat xcoeff, GLfloat ycoeff) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVClipSpaceWScaling _api;
};

// Manages the entry points for the GL_NV_command_list extension API.
class NVCommandListAPI : public BaseExtension
{
public:
    // Construction
    NVCommandListAPI();

    // Accessors
    const NVCommandList &getRawAPI() const;

    // Operations
    void callCommandList(GLuint list) const;
    void commandListSegments(GLuint list, GLuint segments) const;
    void compileCommandList(GLuint list) const;
    void createCommandLists(GLsizei n, GLuint *lists) const;
    void createStateS(GLsizei n, GLuint *states) const;
    void deleteCommandLists(GLsizei n, const GLuint *lists) const;
    void deleteStateS(GLsizei n, const GLuint *states) const;
    void drawCommandsAddress(GLenum primitiveMode, const GLuint64 *indirects,
                             const GLsizei *sizes, GLuint count) const;
    void drawCommands(GLenum primitiveMode, GLuint buffer,
                      const GLintptr *indirects, const GLsizei *sizes,
                      GLuint count) const;
    void drawCommandsStateSAddress(const GLuint64 *indirects,
                                   const GLsizei *sizes, const GLuint *states,
                                   const GLuint *fbos, GLuint count) const;
    void drawCommandsStateS(BufferName buffer, const GLintptr *indirects,
                            const GLsizei *sizes, const GLuint *states,
                            const GLuint *fbos, GLuint count) const;
    GLuint getCommandHeader(GLenum tokenID, GLuint size) const;
    GLushort getStageIndex(ShaderType shadertype) const;
    Boolean isCommandList(GLuint list) const;
    Boolean isState(GLuint state) const;
    void listDrawCommandsStateSClient(GLuint list, GLuint segment,
                                      const void **indirects,
                                      const GLsizei *sizes, const GLuint *states,
                                      const GLuint *fbos, GLuint count) const;
    void stateCapture(GLuint state, GLenum mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVCommandList _api;
};

// Manages the entry points for the GL_NV_conditional_render extension API.
class NVConditionalRenderAPI : public BaseExtension
{
public:
    // Construction
    NVConditionalRenderAPI();

    // Accessors
    const NVConditionalRender &getRawAPI() const;

    // Operations
    void beginConditionalRender(GLuint id, ConditionalRenderMode mode) const;
    void endConditionalRender() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVConditionalRender _api;
};

// Manages the entry points for the GL_NV_conservative_raster extension API.
class NVConservativeRasterAPI : public BaseExtension
{
public:
    // Construction
    NVConservativeRasterAPI();

    // Accessors
    const NVConservativeRaster &getRawAPI() const;

    // Operations
    void subpixelPrecisionBias(GLuint xbits, GLuint ybits) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVConservativeRaster _api;
};

// Manages the entry points for the GL_NV_conservative_raster_dilate extension API.
class NVConservativeRasterDilateAPI : public BaseExtension
{
public:
    // Construction
    NVConservativeRasterDilateAPI();

    // Accessors
    const NVConservativeRasterDilate &getRawAPI() const;

    // Operations
    void conservativeRasterParameterF(GLenum pname, GLfloat value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVConservativeRasterDilate _api;
};

// Manages the entry points for the GL_NV_conservative_raster_pre_snap_triangles extension API.
class NVConservativeRasterPreSnapTrianglesAPI : public BaseExtension
{
public:
    // Construction
    NVConservativeRasterPreSnapTrianglesAPI();

    // Accessors
    const NVConservativeRasterPreSnapTriangles &getRawAPI() const;

    // Operations
    void conservativeRasterParameterI(GLenum pname, GLint param) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVConservativeRasterPreSnapTriangles _api;
};

// Manages the entry points for the GL_NV_copy_image extension API.
class NVCopyImageAPI : public BaseExtension
{
public:
    // Construction
    NVCopyImageAPI();

    // Accessors
    const NVCopyImage &getRawAPI() const;

    // Operations
    void copyImageSubData(GLuint srcName, CopyBufferSubDataTarget srcTarget,
                          GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
                          GLuint dstName, CopyBufferSubDataTarget dstTarget,
                          GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ,
                          GLsizei width, GLsizei height, GLsizei depth) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVCopyImage _api;
};

// Manages the entry points for the GL_NV_depth_buffer_float extension API.
class NVDepthBufferFloatAPI : public BaseExtension
{
public:
    // Construction
    NVDepthBufferFloatAPI();

    // Accessors
    const NVDepthBufferFloat &getRawAPI() const;

    // Operations
    void clearDepthD(GLdouble depth) const;
    void depthBoundsD(GLdouble zmin, GLdouble zmax) const;
    void depthRangeD(GLdouble zNear, GLdouble zFar) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVDepthBufferFloat _api;
};

// Manages the entry points for the GL_NV_draw_texture extension API.
class NVDrawTextureAPI : public BaseExtension
{
public:
    // Construction
    NVDrawTextureAPI();

    // Accessors
    const NVDrawTexture &getRawAPI() const;

    // Operations
    void drawTexture(TextureName texture, SamplerName sampler, GLfloat x0,
                     GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z, GLfloat s0,
                     GLfloat t0, GLfloat s1, GLfloat t1) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVDrawTexture _api;
};

// Manages the entry points for the GL_NV_draw_vulkan_image extension API.
class NVDrawVulkanImageAPI : public BaseExtension
{
public:
    // Construction
    NVDrawVulkanImageAPI();

    // Accessors
    const NVDrawVulkanImage &getRawAPI() const;

    // Operations
    void drawVkImage(GLuint64 vkImage, SamplerName sampler, GLfloat x0,
                     GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z, GLfloat s0,
                     GLfloat t0, GLfloat s1, GLfloat t1) const;
    GLVULKANPROCNV getVkProcAddr(const GLchar *name) const;
    void signalVkFence(GLuint64 vkFence) const;
    void signalVkSemaphore(GLuint64 vkSemaphore) const;
    void waitVkSemaphore(GLuint64 vkSemaphore) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVDrawVulkanImage _api;
};

// Manages the entry points for the GL_NV_evaluators extension API.
class NVEvaluatorsAPI : public BaseExtension
{
public:
    // Construction
    NVEvaluatorsAPI();

    // Accessors
    const NVEvaluators &getRawAPI() const;

    // Operations
    void evalMaps(EvalTargetNV target, GLenum mode) const;
    void getMapAttribParameterFV(EvalTargetNV target, GLuint index,
                                 MapAttribParameterNV pname, GLfloat *params) const;
    void getMapAttribParameterIV(EvalTargetNV target, GLuint index,
                                 MapAttribParameterNV pname, GLint *params) const;
    void getMapControlPoints(EvalTargetNV target, GLuint index, MapTypeNV type,
                             GLsizei ustride, GLsizei vstride, Boolean packed,
                             void *points) const;
    void getMapParameterFV(EvalTargetNV target, MapParameterNV pname,
                           GLfloat *params) const;
    void getMapParameterIV(EvalTargetNV target, MapParameterNV pname,
                           GLint *params) const;
    void mapControlPoints(EvalTargetNV target, GLuint index, MapTypeNV type,
                          GLsizei ustride, GLsizei vstride, GLint uorder,
                          GLint vorder, Boolean packed, const void *points) const;
    void mapParameterFV(EvalTargetNV target, MapParameterNV pname,
                        const GLfloat *params) const;
    void mapParameterIV(EvalTargetNV target, MapParameterNV pname,
                        const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVEvaluators _api;
};

// Manages the entry points for the GL_NV_explicit_multisample extension API.
class NVExplicitMultisampleAPI : public BaseExtension
{
public:
    // Construction
    NVExplicitMultisampleAPI();

    // Accessors
    const NVExplicitMultisample &getRawAPI() const;

    // Operations
    void getMultisampleFV(GetMultisamplePNameNV pname, GLuint index,
                          GLfloat *val) const;
    void sampleMaskIndexed(GLuint index, GLbitfield mask) const;
    void texRenderbuffer(TextureTarget target, RenderBufferName renderbuffer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVExplicitMultisample _api;
};

// Manages the entry points for the GL_NV_fence extension API.
class NVFenceAPI : public BaseExtension
{
public:
    // Construction
    NVFenceAPI();

    // Accessors
    const NVFence &getRawAPI() const;

    // Operations
    void deleteFences(GLsizei n, const GLuint *fences) const;
    void finishFence(GLuint fence) const;
    void genFences(GLsizei n, GLuint *fences) const;
    void getFenceIV(GLuint fence, FenceParameterNameNV pname, GLint *params) const;
    Boolean isFence(GLuint fence) const;
    void setFence(GLuint fence, FenceConditionNV condition) const;
    Boolean testFence(GLuint fence) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVFence _api;
};

// Manages the entry points for the GL_NV_fragment_coverage_to_color extension API.
class NVFragmentCoverageToColorAPI : public BaseExtension
{
public:
    // Construction
    NVFragmentCoverageToColorAPI();

    // Accessors
    const NVFragmentCoverageToColor &getRawAPI() const;

    // Operations
    void fragmentCoverageColor(GLuint color) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVFragmentCoverageToColor _api;
};

// Manages the entry points for the GL_NV_fragment_program extension API.
class NVFragmentProgramAPI : public BaseExtension
{
public:
    // Construction
    NVFragmentProgramAPI();

    // Accessors
    const NVFragmentProgram &getRawAPI() const;

    // Operations
    void getProgramNamedParameterDV(ProgramName id, GLsizei len,
                                    const GLubyte *name, GLdouble *params) const;
    void getProgramNamedParameterFV(ProgramName id, GLsizei len,
                                    const GLubyte *name, GLfloat *params) const;
    void programNamedParameter4D(ProgramName id, GLsizei len,
                                 const GLubyte *name, GLdouble x, GLdouble y,
                                 GLdouble z, GLdouble w) const;
    void programNamedParameter4DV(ProgramName id, GLsizei len,
                                  const GLubyte *name, const GLdouble *v) const;
    void programNamedParameter4F(ProgramName id, GLsizei len,
                                 const GLubyte *name, GLfloat x, GLfloat y,
                                 GLfloat z, GLfloat w) const;
    void programNamedParameter4FV(ProgramName id, GLsizei len,
                                  const GLubyte *name, const GLfloat *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVFragmentProgram _api;
};

// Manages the entry points for the GL_NV_framebuffer_mixed_samples extension API.
class NVFramebufferMixedSamplesAPI : public BaseExtension
{
public:
    // Construction
    NVFramebufferMixedSamplesAPI();

    // Accessors
    const NVFramebufferMixedSamples &getRawAPI() const;

    // Operations
    void coverageModulation(GLenum components) const;
    void coverageModulationTable(GLsizei n, const GLfloat *v) const;
    void getCoverageModulationTable(GLsizei bufSize, GLfloat *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVFramebufferMixedSamples _api;
};

// Manages the entry points for the GL_NV_framebuffer_multisample_coverage extension API.
class NVFramebufferMultisampleCoverageAPI : public BaseExtension
{
public:
    // Construction
    NVFramebufferMultisampleCoverageAPI();

    // Accessors
    const NVFramebufferMultisampleCoverage &getRawAPI() const;

    // Operations
    void renderbufferStorageMultisampleCoverage(RenderBufferTarget target,
                                                GLsizei coverageSamples,
                                                GLsizei colorSamples,
                                                InternalFormat internalformat,
                                                GLsizei width, GLsizei height) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVFramebufferMultisampleCoverage _api;
};

// Manages the entry points for the GL_NV_geometry_program4 extension API.
class NVGeometryPROGRAM4API : public BaseExtension
{
public:
    // Construction
    NVGeometryPROGRAM4API();

    // Accessors
    const NVGeometryPROGRAM4 &getRawAPI() const;

    // Operations
    void framebufferTexture(FramebufferTarget target,
                            FramebufferAttachment attachment,
                            TextureName texture, GLint level) const;
    void framebufferTextureFace(FramebufferTarget target,
                                FramebufferAttachment attachment,
                                TextureName texture, GLint level,
                                TextureTarget face) const;
    void programVertexLimit(ProgramTarget target, GLint limit) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVGeometryPROGRAM4 _api;
};

// Manages the entry points for the GL_NV_gpu_multicast extension API.
class NVGPUMulticastAPI : public BaseExtension
{
public:
    // Construction
    NVGPUMulticastAPI();

    // Accessors
    const NVGPUMulticast &getRawAPI() const;

    // Operations
    void multicastBarrier() const;
    void multicastBlitFramebuffer(GLuint srcGpu, GLuint dstGpu, GLint srcX0,
                                  GLint srcY0, GLint srcX1, GLint srcY1,
                                  GLint dstX0, GLint dstY0, GLint dstX1,
                                  GLint dstY1,
                                  GLbitfield  /* ClearBufferMask */ mask,
                                  GLenum filter) const;
    void multicastBufferSubData(GLbitfield gpuMask, BufferName buffer,
                                GLintptr offset, GLsizeiptr size,
                                const void *data) const;
    void multicastCopyBufferSubData(GLuint readGpu, GLbitfield writeGpuMask,
                                    BufferName readBuffer,
                                    BufferName writeBuffer, GLintptr readOffset,
                                    GLintptr writeOffset, GLsizeiptr size) const;
    void multicastCopyImageSubData(GLuint srcGpu, GLbitfield dstGpuMask,
                                   GLuint srcName, GLenum srcTarget,
                                   GLint srcLevel, GLint srcX, GLint srcY,
                                   GLint srcZ, GLuint dstName, GLenum dstTarget,
                                   GLint dstLevel, GLint dstX, GLint dstY,
                                   GLint dstZ, GLsizei srcWidth,
                                   GLsizei srcHeight, GLsizei srcDepth) const;
    void multicastFramebufferSampleLocationsFV(GLuint gpu,
                                               FrameBufferName framebuffer,
                                               GLuint start, GLsizei count,
                                               const GLfloat *v) const;
    void multicastGetQueryObjectI64V(GLuint gpu, GLuint id, GLenum pname,
                                     GLint64 *params) const;
    void multicastGetQueryObjectIV(GLuint gpu, GLuint id, GLenum pname,
                                   GLint *params) const;
    void multicastGetQueryObjectUI64(GLuint gpu, GLuint id, GLenum pname,
                                     GLuint64 *params) const;
    void multicastGetQueryObjectUIV(GLuint gpu, GLuint id, GLenum pname,
                                    GLuint *params) const;
    void multicastWaitSync(GLuint signalGpu, GLbitfield waitGpuMask) const;
    void renderGpuMask(GLbitfield mask) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVGPUMulticast _api;
};

// Manages the entry points for the GL_NV_gpu_program4 extension API.
class NVGPUPROGRAM4API : public BaseExtension
{
public:
    // Construction
    NVGPUPROGRAM4API();

    // Accessors
    const NVGPUPROGRAM4 &getRawAPI() const;

    // Operations
    void getProgramEnvParameterIIV(ProgramTarget target, GLuint index,
                                   GLint *params) const;
    void getProgramEnvParameterIUIV(ProgramTarget target, GLuint index,
                                    GLuint *params) const;
    void getProgramLocalParameterIIV(ProgramTarget target, GLuint index,
                                     GLint *params) const;
    void getProgramLocalParameterIUIV(ProgramTarget target, GLuint index,
                                      GLuint *params) const;
    void programEnvParameterI4I(ProgramTarget target, GLuint index, GLint x,
                                GLint y, GLint z, GLint w) const;
    void programEnvParameterI4IV(ProgramTarget target, GLuint index,
                                 const GLint *params) const;
    void programEnvParameterI4UI(ProgramTarget target, GLuint index, GLuint x,
                                 GLuint y, GLuint z, GLuint w) const;
    void programEnvParameterI4UIV(ProgramTarget target, GLuint index,
                                  const GLuint *params) const;
    void programEnvParametersI4IV(ProgramTarget target, GLuint index,
                                  GLsizei count, const GLint *params) const;
    void programEnvParametersI4UIV(ProgramTarget target, GLuint index,
                                   GLsizei count, const GLuint *params) const;
    void programLocalParameterI4I(ProgramTarget target, GLuint index, GLint x,
                                  GLint y, GLint z, GLint w) const;
    void programLocalParameterI4IV(ProgramTarget target, GLuint index,
                                   const GLint *params) const;
    void programLocalParameterI4UI(ProgramTarget target, GLuint index, GLuint x,
                                   GLuint y, GLuint z, GLuint w) const;
    void programLocalParameterI4UIV(ProgramTarget target, GLuint index,
                                    const GLuint *params) const;
    void programLocalParametersI4IV(ProgramTarget target, GLuint index,
                                    GLsizei count, const GLint *params) const;
    void programLocalParametersI4UIV(ProgramTarget target, GLuint index,
                                     GLsizei count, const GLuint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVGPUPROGRAM4 _api;
};

// Manages the entry points for the GL_NV_gpu_program5 extension API.
class NVGPUPROGRAM5API : public BaseExtension
{
public:
    // Construction
    NVGPUPROGRAM5API();

    // Accessors
    const NVGPUPROGRAM5 &getRawAPI() const;

    // Operations
    void getProgramSubroutineParameterUIV(GLenum target, GLuint index,
                                          GLuint *param) const;
    void programSubroutineParametersUIV(GLenum target, GLsizei count,
                                        const GLuint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVGPUPROGRAM5 _api;
};

// Manages the entry points for the GL_NV_half_float extension API.
class NVHalfFloatAPI : public BaseExtension
{
public:
    // Construction
    NVHalfFloatAPI();

    // Accessors
    const NVHalfFloat &getRawAPI() const;

    // Operations
    void color3H(GLhalfNV red, GLhalfNV green, GLhalfNV blue) const;
    void color3HV(const GLhalfNV *v) const;
    void color4H(GLhalfNV red, GLhalfNV green, GLhalfNV blue, GLhalfNV alpha) const;
    void color4HV(const GLhalfNV *v) const;
    void fogCoordh(GLhalfNV fog) const;
    void fogCoordhV(const GLhalfNV *fog) const;
    void multiTexCoord1H(TextureUnit target, GLhalfNV s) const;
    void multiTexCoord1HV(TextureUnit target, const GLhalfNV *v) const;
    void multiTexCoord2H(TextureUnit target, GLhalfNV s, GLhalfNV t) const;
    void multiTexCoord2HV(TextureUnit target, const GLhalfNV *v) const;
    void multiTexCoord3H(TextureUnit target, GLhalfNV s, GLhalfNV t, GLhalfNV r) const;
    void multiTexCoord3HV(TextureUnit target, const GLhalfNV *v) const;
    void multiTexCoord4H(TextureUnit target, GLhalfNV s, GLhalfNV t, GLhalfNV r,
                         GLhalfNV q) const;
    void multiTexCoord4HV(TextureUnit target, const GLhalfNV *v) const;
    void normal3H(GLhalfNV nx, GLhalfNV ny, GLhalfNV nz) const;
    void normal3HV(const GLhalfNV *v) const;
    void secondaryColor3H(GLhalfNV red, GLhalfNV green, GLhalfNV blue) const;
    void secondaryColor3HV(const GLhalfNV *v) const;
    void texCoord1H(GLhalfNV s) const;
    void texCoord1HV(const GLhalfNV *v) const;
    void texCoord2H(GLhalfNV s, GLhalfNV t) const;
    void texCoord2HV(const GLhalfNV *v) const;
    void texCoord3H(GLhalfNV s, GLhalfNV t, GLhalfNV r) const;
    void texCoord3HV(const GLhalfNV *v) const;
    void texCoord4H(GLhalfNV s, GLhalfNV t, GLhalfNV r, GLhalfNV q) const;
    void texCoord4HV(const GLhalfNV *v) const;
    void vertex2H(GLhalfNV x, GLhalfNV y) const;
    void vertex2HV(const GLhalfNV *v) const;
    void vertex3H(GLhalfNV x, GLhalfNV y, GLhalfNV z) const;
    void vertex3HV(const GLhalfNV *v) const;
    void vertex4H(GLhalfNV x, GLhalfNV y, GLhalfNV z, GLhalfNV w) const;
    void vertex4HV(const GLhalfNV *v) const;
    void vertexAttrib1H(GLuint index, GLhalfNV x) const;
    void vertexAttrib1HV(GLuint index, const GLhalfNV *v) const;
    void vertexAttrib2H(GLuint index, GLhalfNV x, GLhalfNV y) const;
    void vertexAttrib2HV(GLuint index, const GLhalfNV *v) const;
    void vertexAttrib3H(GLuint index, GLhalfNV x, GLhalfNV y, GLhalfNV z) const;
    void vertexAttrib3HV(GLuint index, const GLhalfNV *v) const;
    void vertexAttrib4H(GLuint index, GLhalfNV x, GLhalfNV y, GLhalfNV z,
                        GLhalfNV w) const;
    void vertexAttrib4HV(GLuint index, const GLhalfNV *v) const;
    void vertexAttribs1HV(GLuint index, GLsizei n, const GLhalfNV *v) const;
    void vertexAttribs2HV(GLuint index, GLsizei n, const GLhalfNV *v) const;
    void vertexAttribs3HV(GLuint index, GLsizei n, const GLhalfNV *v) const;
    void vertexAttribs4HV(GLuint index, GLsizei n, const GLhalfNV *v) const;
    void vertexWeighth(GLhalfNV weight) const;
    void vertexWeighthV(const GLhalfNV *weight) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVHalfFloat _api;
};

// Manages the entry points for the GL_NV_internalformat_sample_query extension API.
class NVInternalformatSampleQueryAPI : public BaseExtension
{
public:
    // Construction
    NVInternalformatSampleQueryAPI();

    // Accessors
    const NVInternalformatSampleQuery &getRawAPI() const;

    // Operations
    void getInternalformatSampleIV(TextureTarget target,
                                   InternalFormat internalformat,
                                   GLsizei samples, InternalFormatPName pname,
                                   GLsizei count, GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVInternalformatSampleQuery _api;
};

// Manages the entry points for the GL_NV_memory_attachment extension API.
class NVMemoryAttachmentAPI : public BaseExtension
{
public:
    // Construction
    NVMemoryAttachmentAPI();

    // Accessors
    const NVMemoryAttachment &getRawAPI() const;

    // Operations
    void bufferAttachMemory(BufferTarget target, GLuint memory,
                            GLuint64 offset) const;
    void getMemoryObjectDetachedResourcesuiv(GLuint memory, GLenum pname,
                                             GLint first, GLsizei count,
                                             GLuint *params) const;
    void namedBufferAttachMemory(BufferName buffer, GLuint memory,
                                 GLuint64 offset) const;
    void resetMemoryObjectParameter(GLuint memory, GLenum pname) const;
    void texAttachMemory(TextureTarget target, GLuint memory, GLuint64 offset) const;
    void textureAttachMemory(TextureName texture, GLuint memory, GLuint64 offset) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVMemoryAttachment _api;
};

// Manages the entry points for the GL_NV_memory_object_sparse extension API.
class NVMemoryObjectSparseAPI : public BaseExtension
{
public:
    // Construction
    NVMemoryObjectSparseAPI();

    // Accessors
    const NVMemoryObjectSparse &getRawAPI() const;

    // Operations
    void bufferPageCommitmentMem(BufferStorageTarget target, GLintptr offset,
                                 GLsizeiptr size, GLuint memory,
                                 GLuint64 memOffset, Boolean commit) const;
    void namedBufferPageCommitmentMem(BufferName buffer, GLintptr offset,
                                      GLsizeiptr size, GLuint memory,
                                      GLuint64 memOffset, Boolean commit) const;
    void texPageCommitmentMem(TextureTarget target, GLint layer, GLint level,
                              GLint xoffset, GLint yoffset, GLint zoffset,
                              GLsizei width, GLsizei height, GLsizei depth,
                              GLuint memory, GLuint64 offset, Boolean commit) const;
    void texturePageCommitmentMem(TextureName texture, GLint layer, GLint level,
                                  GLint xoffset, GLint yoffset, GLint zoffset,
                                  GLsizei width, GLsizei height, GLsizei depth,
                                  GLuint memory, GLuint64 offset, Boolean commit) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVMemoryObjectSparse _api;
};

// Manages the entry points for the GL_NV_mesh_shader extension API.
class NVMeshShaderAPI : public BaseExtension
{
public:
    // Construction
    NVMeshShaderAPI();

    // Accessors
    const NVMeshShader &getRawAPI() const;

    // Operations
    void drawMeshTasksIndirect(GLintptr indirect) const;
    void drawMeshTasks(GLuint first, GLuint count) const;
    void multiDrawMeshTasksIndirectCount(GLintptr indirect, GLintptr drawcount,
                                         GLsizei maxdrawcount, GLsizei stride) const;
    void multiDrawMeshTasksIndirect(GLintptr indirect, GLsizei drawcount,
                                    GLsizei stride) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVMeshShader _api;
};

// Manages the entry points for the GL_NV_occlusion_query extension API.
class NVOcclusionQueryAPI : public BaseExtension
{
public:
    // Construction
    NVOcclusionQueryAPI();

    // Accessors
    const NVOcclusionQuery &getRawAPI() const;

    // Operations
    void beginOcclusionQuery(GLuint id) const;
    void deleteOcclusionQueries(GLsizei n, const GLuint *ids) const;
    void endOcclusionQuery() const;
    void genOcclusionQueries(GLsizei n, GLuint *ids) const;
    void getOcclusionQueryIV(GLuint id, OcclusionQueryParameterNameNV pname,
                             GLint *params) const;
    void getOcclusionQueryUIV(GLuint id, OcclusionQueryParameterNameNV pname,
                              GLuint *params) const;
    Boolean isOcclusionQuery(GLuint id) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVOcclusionQuery _api;
};

// Manages the entry points for the GL_NV_parameter_buffer_object extension API.
class NVParameterBufferObjectAPI : public BaseExtension
{
public:
    // Construction
    NVParameterBufferObjectAPI();

    // Accessors
    const NVParameterBufferObject &getRawAPI() const;

    // Operations
    void programBufferParametersIIV(ProgramTarget target, GLuint bindingIndex,
                                    GLuint wordIndex, GLsizei count,
                                    const GLint *params) const;
    void programBufferParametersIUIV(ProgramTarget target, GLuint bindingIndex,
                                     GLuint wordIndex, GLsizei count,
                                     const GLuint *params) const;
    void programBufferParametersFV(ProgramTarget target, GLuint bindingIndex,
                                   GLuint wordIndex, GLsizei count,
                                   const GLfloat *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVParameterBufferObject _api;
};

// Manages the entry points for the GL_NV_path_rendering extension API.
class NVPathRenderingAPI : public BaseExtension
{
public:
    // Construction
    NVPathRenderingAPI();

    // Accessors
    const NVPathRendering &getRawAPI() const;

    // Operations
    void copyPath(GLuint resultPath, GLuint srcPath) const;
    void coverFillPathInstanced(GLsizei numPaths, PathElementType pathNameType,
                                const void *paths, GLuint pathBase,
                                PathCoverMode coverMode,
                                PathTransformType transformType,
                                const GLfloat *transformValues) const;
    void coverFillPath(GLuint path, PathCoverMode coverMode) const;
    void coverStrokePathInstanced(GLsizei numPaths, PathElementType pathNameType,
                                  const void *paths, GLuint pathBase,
                                  PathCoverMode coverMode,
                                  PathTransformType transformType,
                                  const GLfloat *transformValues) const;
    void coverStrokePath(GLuint path, PathCoverMode coverMode) const;
    void deletePaths(GLuint path, GLsizei range) const;
    GLuint genPaths(GLsizei range) const;
    void getPathCommands(GLuint path, GLubyte *commands) const;
    void getPathCoords(GLuint path, GLfloat *coords) const;
    void getPathDashArray(GLuint path, GLfloat *dashArray) const;
    GLfloat getPathLength(GLuint path, GLsizei startSegment, GLsizei numSegments) const;
    void getPathMetricRange(GLbitfield  /* PathMetricMask */ metricQueryMask,
                            GLuint firstPathName, GLsizei numPaths,
                            GLsizei stride, GLfloat *metrics) const;
    void getPathMetrics(GLbitfield  /* PathMetricMask */ metricQueryMask,
                        GLsizei numPaths, PathElementType pathNameType,
                        const void *paths, GLuint pathBase, GLsizei stride,
                        GLfloat *metrics) const;
    void getPathParameterFV(GLuint path, PathParameter pname, GLfloat *value) const;
    void getPathParameterIV(GLuint path, PathParameter pname, GLint *value) const;
    void getPathSpacing(PathListMode pathListMode, GLsizei numPaths,
                        PathElementType pathNameType, const void *paths,
                        GLuint pathBase, GLfloat advanceScale,
                        GLfloat kerningScale, PathTransformType transformType,
                        GLfloat *returnedSpacing) const;
    void getProgramResourceFV(ProgramName program,
                              ProgramInterface programInterface, GLuint index,
                              GLsizei propCount, const GLenum *props,
                              GLsizei count, GLsizei *length, GLfloat *params) const;
    void interpolatePaths(GLuint resultPath, GLuint pathA, GLuint pathB,
                          GLfloat weight) const;
    Boolean isPath(GLuint path) const;
    Boolean isPointInFillPath(GLuint path, GLuint mask, GLfloat x, GLfloat y) const;
    Boolean isPointInStrokePath(GLuint path, GLfloat x, GLfloat y) const;
    void matrixLoad3x2F(GLenum matrixMode, const GLfloat *m) const;
    void matrixLoad3x3F(GLenum matrixMode, const GLfloat *m) const;
    void matrixLoadTranspose3x3F(GLenum matrixMode, const GLfloat *m) const;
    void matrixMult3x2F(GLenum matrixMode, const GLfloat *m) const;
    void matrixMult3x3F(GLenum matrixMode, const GLfloat *m) const;
    void matrixMultTranspose3x3F(GLenum matrixMode, const GLfloat *m) const;
    void pathCommands(GLuint path, GLsizei numCommands, const GLubyte *commands,
                      GLsizei numCoords, PathCoordType coordType,
                      const void *coords) const;
    void pathCoords(GLuint path, GLsizei numCoords, PathCoordType coordType,
                    const void *coords) const;
    void pathCoverDepthFunc(DepthFunction func) const;
    void pathDashArray(GLuint path, GLsizei dashCount, const GLfloat *dashArray) const;
    GLenum pathGlyphIndexArray(GLuint firstPathName, GLenum fontTarget,
                               const void *fontName,
                               GLbitfield  /* PathFontStyle */ fontStyle,
                               GLuint firstGlyphIndex, GLsizei numGlyphs,
                               GLuint pathParameterTemplate, GLfloat emScale) const;
    GLenum pathGlyphIndexRange(GLenum fontTarget, const void *fontName,
                               GLbitfield  /* PathFontStyle */ fontStyle,
                               GLuint pathParameterTemplate, GLfloat emScale,
                               GLuint *baseAndCount) const;
    void pathGlyphRange(GLuint firstPathName, PathFontTarget fontTarget,
                        const void *fontName,
                        GLbitfield  /* PathFontStyle */ fontStyle,
                        GLuint firstGlyph, GLsizei numGlyphs,
                        PathHandleMissingGlyphs handleMissingGlyphs,
                        GLuint pathParameterTemplate, GLfloat emScale) const;
    void pathGlyphs(GLuint firstPathName, PathFontTarget fontTarget,
                    const void *fontName,
                    GLbitfield  /* PathFontStyle */ fontStyle, GLsizei numGlyphs,
                    PathElementType type, const void *charcodes,
                    PathHandleMissingGlyphs handleMissingGlyphs,
                    GLuint pathParameterTemplate, GLfloat emScale) const;
    GLenum pathMemoryGlyphIndexArray(GLuint firstPathName, GLenum fontTarget,
                                     GLsizeiptr fontSize, const void *fontData,
                                     GLsizei faceIndex, GLuint firstGlyphIndex,
                                     GLsizei numGlyphs,
                                     GLuint pathParameterTemplate,
                                     GLfloat emScale) const;
    void pathParameterF(GLuint path, PathParameter pname, GLfloat value) const;
    void pathParameterFV(GLuint path, PathParameter pname, const GLfloat *value) const;
    void pathParameterI(GLuint path, PathParameter pname, GLint value) const;
    void pathParameterIV(GLuint path, PathParameter pname, const GLint *value) const;
    void pathStencilDepthOffset(GLfloat factor, GLfloat units) const;
    void pathStencilFunc(StencilFunction func, GLint ref, GLuint mask) const;
    void pathString(GLuint path, PathStringFormat format, GLsizei length,
                    const void *pathString) const;
    void pathSubCommands(GLuint path, GLsizei commandStart,
                         GLsizei commandsToDelete, GLsizei numCommands,
                         const GLubyte *commands, GLsizei numCoords,
                         PathCoordType coordType, const void *coords) const;
    void pathSubCoords(GLuint path, GLsizei coordStart, GLsizei numCoords,
                       PathCoordType coordType, const void *coords) const;
    Boolean pointAlongPath(GLuint path, GLsizei startSegment,
                           GLsizei numSegments, GLfloat distance, GLfloat *x,
                           GLfloat *y, GLfloat *tangentX, GLfloat *tangentY) const;
    void programPathFragmentInputGen(ProgramName program, GLint location,
                                     GLenum genMode, GLint components,
                                     const GLfloat *coeffs) const;
    void stencilFillPathInstanced(GLsizei numPaths, PathElementType pathNameType,
                                  const void *paths, GLuint pathBase,
                                  PathFillMode fillMode, GLuint mask,
                                  PathTransformType transformType,
                                  const GLfloat *transformValues) const;
    void stencilFillPath(GLuint path, PathFillMode fillMode, GLuint mask) const;
    void stencilStrokePathInstanced(GLsizei numPaths,
                                    PathElementType pathNameType,
                                    const void *paths, GLuint pathBase,
                                    GLint reference, GLuint mask,
                                    PathTransformType transformType,
                                    const GLfloat *transformValues) const;
    void stencilStrokePath(GLuint path, GLint reference, GLuint mask) const;
    void stencilThenCoverFillPathInstanced(GLsizei numPaths, GLenum pathNameType,
                                           const void *paths, GLuint pathBase,
                                           GLenum fillMode, GLuint mask,
                                           GLenum coverMode,
                                           GLenum transformType,
                                           const GLfloat *transformValues) const;
    void stencilThenCoverFillPath(GLuint path, GLenum fillMode, GLuint mask,
                                  GLenum coverMode) const;
    void stencilThenCoverStrokePathInstanced(GLsizei numPaths,
                                             GLenum pathNameType,
                                             const void *paths, GLuint pathBase,
                                             GLint reference, GLuint mask,
                                             GLenum coverMode,
                                             GLenum transformType,
                                             const GLfloat *transformValues) const;
    void stencilThenCoverStrokePath(GLuint path, GLint reference, GLuint mask,
                                    GLenum coverMode) const;
    void transformPath(GLuint resultPath, GLuint srcPath,
                       PathTransformType transformType,
                       const GLfloat *transformValues) const;
    void weightPaths(GLuint resultPath, GLsizei numPaths, const GLuint *paths,
                     const GLfloat *weights) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVPathRendering _api;
};

// Manages the entry points for the GL_NV_pixel_data_range extension API.
class NVPixelDataRangeAPI : public BaseExtension
{
public:
    // Construction
    NVPixelDataRangeAPI();

    // Accessors
    const NVPixelDataRange &getRawAPI() const;

    // Operations
    void flushPixelDataRange(PixelDataRangeTargetNV target) const;
    void pixelDataRange(PixelDataRangeTargetNV target, GLsizei length,
                        const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVPixelDataRange _api;
};

// Manages the entry points for the GL_NV_point_sprite extension API.
class NVPointSpriteAPI : public BaseExtension
{
public:
    // Construction
    NVPointSpriteAPI();

    // Accessors
    const NVPointSprite &getRawAPI() const;

    // Operations
    void pointParameterI(PointParameterNameARB pname, GLint param) const;
    void pointParameterIV(PointParameterNameARB pname, const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVPointSprite _api;
};

// Manages the entry points for the GL_NV_present_video extension API.
class NVPresentVideoAPI : public BaseExtension
{
public:
    // Construction
    NVPresentVideoAPI();

    // Accessors
    const NVPresentVideo &getRawAPI() const;

    // Operations
    void getVideoI64V(GLuint video_slot, GLenum pname, GLint64EXT *params) const;
    void getVideoIV(GLuint video_slot, GLenum pname, GLint *params) const;
    void getVideoUI64(GLuint video_slot, GLenum pname, GLuint64EXT *params) const;
    void getVideoUIV(GLuint video_slot, GLenum pname, GLuint *params) const;
    void presentFrameDualFill(GLuint video_slot, GLuint64EXT minPresentTime,
                              GLuint beginPresentTimeId,
                              GLuint presentDurationId, GLenum type,
                              GLenum target0, GLuint fill0, GLenum target1,
                              GLuint fill1, GLenum target2, GLuint fill2,
                              GLenum target3, GLuint fill3) const;
    void presentFrameKeyed(GLuint video_slot, GLuint64EXT minPresentTime,
                           GLuint beginPresentTimeId, GLuint presentDurationId,
                           GLenum type, GLenum target0, GLuint fill0,
                           GLuint key0, GLenum target1, GLuint fill1,
                           GLuint key1) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVPresentVideo _api;
};

// Manages the entry points for the GL_NV_primitive_restart extension API.
class NVPrimitiveRestartAPI : public BaseExtension
{
public:
    // Construction
    NVPrimitiveRestartAPI();

    // Accessors
    const NVPrimitiveRestart &getRawAPI() const;

    // Operations
    void primitiveRestartIndex(GLuint index) const;
    void primitiveRestart() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVPrimitiveRestart _api;
};

// Manages the entry points for the GL_NV_query_resource extension API.
class NVQueryResourceAPI : public BaseExtension
{
public:
    // Construction
    NVQueryResourceAPI();

    // Accessors
    const NVQueryResource &getRawAPI() const;

    // Operations
    GLint queryResource(GLenum queryType, GLint tagId, GLuint count,
                        GLint *buffer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVQueryResource _api;
};

// Manages the entry points for the GL_NV_query_resource_tag extension API.
class NVQueryResourceTagAPI : public BaseExtension
{
public:
    // Construction
    NVQueryResourceTagAPI();

    // Accessors
    const NVQueryResourceTag &getRawAPI() const;

    // Operations
    void deleteQueryResourceTag(GLsizei n, const GLint *tagIds) const;
    void genQueryResourceTag(GLsizei n, GLint *tagIds) const;
    void queryResourceTag(GLint tagId, const GLchar *tagString) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVQueryResourceTag _api;
};

// Manages the entry points for the GL_NV_register_combiners extension API.
class NVRegisterCombinersAPI : public BaseExtension
{
public:
    // Construction
    NVRegisterCombinersAPI();

    // Accessors
    const NVRegisterCombiners &getRawAPI() const;

    // Operations
    void combinerInput(CombinerStageNV stage, CombinerPortionNV portion,
                       CombinerVariableNV variable, CombinerRegisterNV input,
                       CombinerMappingNV mapping,
                       CombinerComponentUsageNV componentUsage) const;
    void combinerOutput(CombinerStageNV stage, CombinerPortionNV portion,
                        CombinerRegisterNV abOutput, CombinerRegisterNV cdOutput,
                        CombinerRegisterNV sumOutput, CombinerScaleNV scale,
                        CombinerBiasNV bias, Boolean abDotProduct,
                        Boolean cdDotProduct, Boolean muxSum) const;
    void combinerParameterF(CombinerParameterNV pname, GLfloat param) const;
    void combinerParameterFV(CombinerParameterNV pname, const GLfloat *params) const;
    void combinerParameterI(CombinerParameterNV pname, GLint param) const;
    void combinerParameterIV(CombinerParameterNV pname, const GLint *params) const;
    void finalCombinerInput(CombinerVariableNV variable,
                            CombinerRegisterNV input, CombinerMappingNV mapping,
                            CombinerComponentUsageNV componentUsage) const;
    void getCombinerInputParameterFV(CombinerStageNV stage,
                                     CombinerPortionNV portion,
                                     CombinerVariableNV variable,
                                     CombinerParameterNV pname, GLfloat *params) const;
    void getCombinerInputParameterIV(CombinerStageNV stage,
                                     CombinerPortionNV portion,
                                     CombinerVariableNV variable,
                                     CombinerParameterNV pname, GLint *params) const;
    void getCombinerOutputParameterFV(CombinerStageNV stage,
                                      CombinerPortionNV portion,
                                      CombinerParameterNV pname, GLfloat *params) const;
    void getCombinerOutputParameterIV(CombinerStageNV stage,
                                      CombinerPortionNV portion,
                                      CombinerParameterNV pname, GLint *params) const;
    void getFinalCombinerInputParameterFV(CombinerVariableNV variable,
                                          CombinerParameterNV pname,
                                          GLfloat *params) const;
    void getFinalCombinerInputParameterIV(CombinerVariableNV variable,
                                          CombinerParameterNV pname,
                                          GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVRegisterCombiners _api;
};

// Manages the entry points for the GL_NV_register_combiners2 extension API.
class NVRegisterCOMBINERS2API : public BaseExtension
{
public:
    // Construction
    NVRegisterCOMBINERS2API();

    // Accessors
    const NVRegisterCOMBINERS2 &getRawAPI() const;

    // Operations
    void combinerStageParameterFV(CombinerStageNV stage,
                                  CombinerParameterNV pname,
                                  const GLfloat *params) const;
    void getCombinerStageParameterFV(CombinerStageNV stage,
                                     CombinerParameterNV pname, GLfloat *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVRegisterCOMBINERS2 _api;
};

// Manages the entry points for the GL_NV_sample_locations extension API.
class NVSampleLocationsAPI : public BaseExtension
{
public:
    // Construction
    NVSampleLocationsAPI();

    // Accessors
    const NVSampleLocations &getRawAPI() const;

    // Operations
    void framebufferSampleLocationsFV(FramebufferTarget target, GLuint start,
                                      GLsizei count, const GLfloat *v) const;
    void namedFramebufferSampleLocationsFV(FrameBufferName framebuffer,
                                           GLuint start, GLsizei count,
                                           const GLfloat *v) const;
    void resolveDepthValues() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVSampleLocations _api;
};

// Manages the entry points for the GL_NV_scissor_exclusive extension API.
class NVScissorExclusiveAPI : public BaseExtension
{
public:
    // Construction
    NVScissorExclusiveAPI();

    // Accessors
    const NVScissorExclusive &getRawAPI() const;

    // Operations
    void scissorExclusiveArrayV(GLuint first, GLsizei count, const GLint *v) const;
    void scissorExclusive(GLint x, GLint y, GLsizei width, GLsizei height) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVScissorExclusive _api;
};

// Manages the entry points for the GL_NV_shader_buffer_load extension API.
class NVShaderBufferLoadAPI : public BaseExtension
{
public:
    // Construction
    NVShaderBufferLoadAPI();

    // Accessors
    const NVShaderBufferLoad &getRawAPI() const;

    // Operations
    void getBufferParameterUI64(BufferTarget target, GLenum pname,
                                GLuint64EXT *params) const;
    void getIntegerUI64(GLenum value, GLuint64EXT *result) const;
    void getNamedBufferParameterUI64(BufferName buffer, BufferPNameARB pname,
                                     GLuint64EXT *params) const;
    Boolean isBufferResident(GLenum target) const;
    Boolean isNamedBufferResident(BufferName buffer) const;
    void makeBufferNonResident(GLenum target) const;
    void makeBufferResident(GLenum target, GLenum access) const;
    void makeNamedBufferNonResident(BufferName buffer) const;
    void makeNamedBufferResident(BufferName buffer, GLenum access) const;
    void programUniformUI64(ProgramName program, GLint location,
                            GLuint64EXT value) const;
    void programUniformUI64(ProgramName program, GLint location, GLsizei count,
                            const GLuint64EXT *value) const;
    void uniformUI64(GLint location, GLuint64EXT value) const;
    void uniformUI64(GLint location, GLsizei count, const GLuint64EXT *value) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVShaderBufferLoad _api;
};

// Manages the entry points for the GL_NV_shading_rate_image extension API.
class NVShadingRateImageAPI : public BaseExtension
{
public:
    // Construction
    NVShadingRateImageAPI();

    // Accessors
    const NVShadingRateImage &getRawAPI() const;

    // Operations
    void bindShadingRateImage(TextureName texture) const;
    void getShadingRateImagePalette(GLuint viewport, GLuint entry, GLenum *rate) const;
    void getShadingRateSampleLocationIV(GLenum rate, GLuint samples,
                                        GLuint index, GLint *location) const;
    void shadingRateImageBarrier(Boolean synchronize) const;
    void shadingRateImagePalette(GLuint viewport, GLuint first, GLsizei count,
                                 const GLenum *rates) const;
    void shadingRateSampleOrderCustom(GLenum rate, GLuint samples,
                                      const GLint *locations) const;
    void shadingRateSampleOrder(GLenum order) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVShadingRateImage _api;
};

// Manages the entry points for the GL_NV_texture_barrier extension API.
class NVTextureBarrierAPI : public BaseExtension
{
public:
    // Construction
    NVTextureBarrierAPI();

    // Accessors
    const NVTextureBarrier &getRawAPI() const;

    // Operations
    void textureBarrier() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVTextureBarrier _api;
};

// Manages the entry points for the GL_NV_texture_multisample extension API.
class NVTextureMultisampleAPI : public BaseExtension
{
public:
    // Construction
    NVTextureMultisampleAPI();

    // Accessors
    const NVTextureMultisample &getRawAPI() const;

    // Operations
    void texImage2DMultisampleCoverage(TextureTarget target,
                                       GLsizei coverageSamples,
                                       GLsizei colorSamples,
                                       InternalFormat internalFormat,
                                       GLsizei width, GLsizei height,
                                       Boolean fixedSampleLocations) const;
    void texImage3DMultisampleCoverage(TextureTarget target,
                                       GLsizei coverageSamples,
                                       GLsizei colorSamples,
                                       InternalFormat internalFormat,
                                       GLsizei width, GLsizei height,
                                       GLsizei depth,
                                       Boolean fixedSampleLocations) const;
    void textureImage2DMultisampleCoverage(TextureName texture,
                                           TextureTarget target,
                                           GLsizei coverageSamples,
                                           GLsizei colorSamples,
                                           InternalFormat internalFormat,
                                           GLsizei width, GLsizei height,
                                           Boolean fixedSampleLocations) const;
    void textureImage2DMultisample(TextureName texture, TextureTarget target,
                                   GLsizei samples,
                                   InternalFormat internalFormat, GLsizei width,
                                   GLsizei height, Boolean fixedSampleLocations) const;
    void textureImage3DMultisampleCoverage(TextureName texture,
                                           TextureTarget target,
                                           GLsizei coverageSamples,
                                           GLsizei colorSamples,
                                           InternalFormat internalFormat,
                                           GLsizei width, GLsizei height,
                                           GLsizei depth,
                                           Boolean fixedSampleLocations) const;
    void textureImage3DMultisample(TextureName texture, TextureTarget target,
                                   GLsizei samples,
                                   InternalFormat internalFormat, GLsizei width,
                                   GLsizei height, GLsizei depth,
                                   Boolean fixedSampleLocations) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVTextureMultisample _api;
};

// Manages the entry points for the GL_NV_timeline_semaphore extension API.
class NVTimelineSemaphoreAPI : public BaseExtension
{
public:
    // Construction
    NVTimelineSemaphoreAPI();

    // Accessors
    const NVTimelineSemaphore &getRawAPI() const;

    // Operations
    void createSemaphores(GLsizei n, GLuint *semaphores) const;
    void getSemaphoreParameterIV(GLuint semaphore, SemaphoreParameterName pname,
                                 GLint *params) const;
    void semaphoreParameterIV(GLuint semaphore, SemaphoreParameterName pname,
                              const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVTimelineSemaphore _api;
};

// Manages the entry points for the GL_NV_transform_feedback extension API.
class NVTransformFeedbackAPI : public BaseExtension
{
public:
    // Construction
    NVTransformFeedbackAPI();

    // Accessors
    const NVTransformFeedback &getRawAPI() const;

    // Operations
    void activeVarying(ProgramName program, const GLchar *name) const;
    void beginTransformFeedback(PrimitiveType primitiveMode) const;
    void bindBufferBase(BufferTarget target, GLuint index, BufferName buffer) const;
    void bindBufferOffset(BufferTarget target, GLuint index,
                          BufferName buffer, GLintptr offset) const;
    void bindBufferRange(BufferTarget target, GLuint index, BufferName buffer,
                         GLintptr offset, GLsizeiptr size) const;
    void endTransformFeedback() const;
    void getActiveVarying(ProgramName program, GLuint index, GLsizei bufSize,
                          GLsizei *length, GLsizei *size, GLenum *type,
                          GLchar *name) const;
    void getTransformFeedbackVarying(ProgramName program, GLuint index,
                                     GLint *location) const;
    GLint getVaryingLocation(ProgramName program, const GLchar *name) const;
    void transformFeedbackAttribs(GLsizei count, const GLint *attribs,
                                  GLenum bufferMode) const;
    void transformFeedbackStreamAttribs(GLsizei count, const GLint *attribs,
                                        GLsizei nbuffers,
                                        const GLint *bufstreams,
                                        GLenum bufferMode) const;
    void transformFeedbackVaryings(ProgramName program, GLsizei count,
                                   const GLint *locations, GLenum bufferMode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVTransformFeedback _api;
};

// Manages the entry points for the GL_NV_transform_feedback2 extension API.
class NVTransformFeedback2API : public BaseExtension
{
public:
    // Construction
    NVTransformFeedback2API();

    // Accessors
    const NVTransformFeedback2 &getRawAPI() const;

    // Operations
    void bindTransformFeedback(BufferTarget target, TransformFeedbackName id) const;
    void deleteTransformFeedbacks(GLsizei n, const TransformFeedbackName *ids) const;
    void drawTransformFeedback(PrimitiveType mode, TransformFeedbackName id) const;
    void genTransformFeedbacks(GLsizei n, TransformFeedbackName *ids) const;
    Boolean isTransformFeedback(TransformFeedbackName id) const;
    void pauseTransformFeedback() const;
    void resumeTransformFeedback() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVTransformFeedback2 _api;
};

// Manages the entry points for the GL_NV_vdpau_interop extension API.
class NVVdpauInteropAPI : public BaseExtension
{
public:
    // Construction
    NVVdpauInteropAPI();

    // Accessors
    const NVVdpauInterop &getRawAPI() const;

    // Operations
    void vdpauFini() const;
    void vdpauGetSurfaceIV(GLvdpauSurfaceNV surface, GLenum pname, GLsizei count,
                           GLsizei *length, GLint *values) const;
    void vdpauInit(const void *vdpDevice, const void *getProcAddress) const;
    Boolean vdpauIsSurface(GLvdpauSurfaceNV surface) const;
    void vdpauMapSurfaces(GLsizei numSurfaces, const GLvdpauSurfaceNV *surfaces) const;
    GLvdpauSurfaceNV vdpauRegisterOutputSurface(const void *vdpSurface,
                                                GLenum target,
                                                GLsizei numTextureNames,
                                                const GLuint *textureNames) const;
    GLvdpauSurfaceNV vdpauRegisterVideoSurface(const void *vdpSurface,
                                               GLenum target,
                                               GLsizei numTextureNames,
                                               const GLuint *textureNames) const;
    void vdpauSurfaceAccess(GLvdpauSurfaceNV surface, GLenum access) const;
    void vdpauUnmapSurfaces(GLsizei numSurface, const GLvdpauSurfaceNV *surfaces) const;
    void vdpauUnregisterSurface(GLvdpauSurfaceNV surface) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVVdpauInterop _api;
};

// Manages the entry points for the GL_NV_vdpau_interop2 extension API.
class NVVdpauINTEROP2API : public BaseExtension
{
public:
    // Construction
    NVVdpauINTEROP2API();

    // Accessors
    const NVVdpauINTEROP2 &getRawAPI() const;

    // Operations
    GLvdpauSurfaceNV vdpauRegisterVideoSurfaceWithPictureStructure(const void *vdpSurface,
                                                                   GLenum target,
                                                                   GLsizei numTextureNames,
                                                                   const GLuint *textureNames,
                                                                   Boolean isFrameStructure) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVVdpauINTEROP2 _api;
};

// Manages the entry points for the GL_NV_vertex_array_range extension API.
class NVVertexArrayRangeAPI : public BaseExtension
{
public:
    // Construction
    NVVertexArrayRangeAPI();

    // Accessors
    const NVVertexArrayRange &getRawAPI() const;

    // Operations
    void flushVertexArrayRange() const;
    void vertexArrayRange(GLsizei length, const void *pointer) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVVertexArrayRange _api;
};

// Manages the entry points for the GL_NV_vertex_attrib_integer_64bit extension API.
class NVVertexAttribInteger64BITAPI : public BaseExtension
{
public:
    // Construction
    NVVertexAttribInteger64BITAPI();

    // Accessors
    const NVVertexAttribInteger64BIT &getRawAPI() const;

    // Operations
    void getVertexAttribLI64V(GLuint index, VertexAttribEnum pname,
                              GLint64EXT *params) const;
    void getVertexAttribLUI64(GLuint index, VertexAttribEnum pname,
                              GLuint64EXT *params) const;
    void vertexAttribL1I64(GLuint index, GLint64EXT x) const;
    void vertexAttribL1I64V(GLuint index, const GLint64EXT *v) const;
    void vertexAttribL1UI64(GLuint index, GLuint64EXT x) const;
    void vertexAttribL1UI64(GLuint index, const GLuint64EXT *v) const;
    void vertexAttribL2I64(GLuint index, GLint64EXT x, GLint64EXT y) const;
    void vertexAttribL2I64V(GLuint index, const GLint64EXT *v) const;
    void vertexAttribL2UI64(GLuint index, GLuint64EXT x, GLuint64EXT y) const;
    void vertexAttribL2UI64(GLuint index, const GLuint64EXT *v) const;
    void vertexAttribL3I64(GLuint index, GLint64EXT x, GLint64EXT y,
                           GLint64EXT z) const;
    void vertexAttribL3I64V(GLuint index, const GLint64EXT *v) const;
    void vertexAttribL3UI64(GLuint index, GLuint64EXT x, GLuint64EXT y,
                            GLuint64EXT z) const;
    void vertexAttribL3UI64(GLuint index, const GLuint64EXT *v) const;
    void vertexAttribL4I64(GLuint index, GLint64EXT x, GLint64EXT y,
                           GLint64EXT z, GLint64EXT w) const;
    void vertexAttribL4I64V(GLuint index, const GLint64EXT *v) const;
    void vertexAttribL4UI64(GLuint index, GLuint64EXT x, GLuint64EXT y,
                            GLuint64EXT z, GLuint64EXT w) const;
    void vertexAttribL4UI64(GLuint index, const GLuint64EXT *v) const;
    void vertexAttribLFormat(GLuint index, GLint size, VertexAttribLType type,
                             GLsizei stride) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVVertexAttribInteger64BIT _api;
};

// Manages the entry points for the GL_NV_vertex_buffer_unified_memory extension API.
class NVVertexBufferUnifiedMemoryAPI : public BaseExtension
{
public:
    // Construction
    NVVertexBufferUnifiedMemoryAPI();

    // Accessors
    const NVVertexBufferUnifiedMemory &getRawAPI() const;

    // Operations
    void bufferAddressRange(GLenum pname, GLuint index, GLuint64EXT address,
                            GLsizeiptr length) const;
    void colorFormat(GLint size, GLenum type, GLsizei stride) const;
    void edgeFlagFormat(GLsizei stride) const;
    void fogCoordFormat(GLenum type, GLsizei stride) const;
    void getIntegerUI64(GLenum value, GLuint index, GLuint64EXT *result) const;
    void indexFormat(GLenum type, GLsizei stride) const;
    void normalFormat(GLenum type, GLsizei stride) const;
    void secondaryColorFormat(GLint size, ColorPointerType type, GLsizei stride) const;
    void texCoordFormat(GLint size, GLenum type, GLsizei stride) const;
    void vertexAttribFormat(GLuint index, GLint size, VertexAttribType type,
                            Boolean normalized, GLsizei stride) const;
    void vertexAttribIFormat(GLuint index, GLint size, VertexAttribIType type,
                             GLsizei stride) const;
    void vertexFormat(GLint size, VertexPointerType type, GLsizei stride) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVVertexBufferUnifiedMemory _api;
};

// Manages the entry points for the GL_NV_vertex_program extension API.
class NVVertexProgramAPI : public BaseExtension
{
public:
    // Construction
    NVVertexProgramAPI();

    // Accessors
    const NVVertexProgram &getRawAPI() const;

    // Operations
    Boolean areProgramsResident(GLsizei n, const ProgramName *programs,
                                Boolean *residences) const;
    void bindProgram(VertexAttribEnumNV target, ProgramName id) const;
    void deletePrograms(GLsizei n, const ProgramName *programs) const;
    void executeProgram(VertexAttribEnumNV target, GLuint id,
                        const GLfloat *params) const;
    void genPrograms(GLsizei n, ProgramName *programs) const;
    void getProgramParameterDV(VertexAttribEnumNV target, GLuint index,
                               VertexAttribEnumNV pname, GLdouble *params) const;
    void getProgramParameterFV(VertexAttribEnumNV target, GLuint index,
                               VertexAttribEnumNV pname, GLfloat *params) const;
    void getProgramString(ProgramName id, VertexAttribEnumNV pname,
                          GLubyte *program) const;
    void getProgramIV(ProgramName id, VertexAttribEnumNV pname, GLint *params) const;
    void getTrackMatrixIV(VertexAttribEnumNV target, GLuint address,
                          VertexAttribEnumNV pname, GLint *params) const;
    void getVertexAttribPointerV(GLuint index, VertexAttribEnumNV pname,
                                 void **pointer) const;
    void getVertexAttribDV(GLuint index, VertexAttribEnumNV pname,
                           GLdouble *params) const;
    void getVertexAttribFV(GLuint index, VertexAttribEnumNV pname,
                           GLfloat *params) const;
    void getVertexAttribIV(GLuint index, VertexAttribEnumNV pname, GLint *params) const;
    Boolean isProgram(ProgramName id) const;
    void loadProgram(VertexAttribEnumNV target, GLuint id, GLsizei len,
                     const GLubyte *program) const;
    void programParameter4D(VertexAttribEnumNV target, GLuint index, GLdouble x,
                            GLdouble y, GLdouble z, GLdouble w) const;
    void programParameter4DV(VertexAttribEnumNV target, GLuint index,
                             const GLdouble *v) const;
    void programParameter4F(VertexAttribEnumNV target, GLuint index, GLfloat x,
                            GLfloat y, GLfloat z, GLfloat w) const;
    void programParameter4FV(VertexAttribEnumNV target, GLuint index,
                             const GLfloat *v) const;
    void programParameters4DV(VertexAttribEnumNV target, GLuint index,
                              GLsizei count, const GLdouble *v) const;
    void programParameters4FV(VertexAttribEnumNV target, GLuint index,
                              GLsizei count, const GLfloat *v) const;
    void requestResidentPrograms(GLsizei n, const ProgramName *programs) const;
    void trackMatrix(VertexAttribEnumNV target, GLuint address,
                     VertexAttribEnumNV matrix, VertexAttribEnumNV transform) const;
    void vertexAttrib1D(GLuint index, GLdouble x) const;
    void vertexAttrib1DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib1F(GLuint index, GLfloat x) const;
    void vertexAttrib1FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib1S(GLuint index, GLshort x) const;
    void vertexAttrib1SV(GLuint index, const GLshort *v) const;
    void vertexAttrib2D(GLuint index, GLdouble x, GLdouble y) const;
    void vertexAttrib2DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib2F(GLuint index, GLfloat x, GLfloat y) const;
    void vertexAttrib2FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib2S(GLuint index, GLshort x, GLshort y) const;
    void vertexAttrib2SV(GLuint index, const GLshort *v) const;
    void vertexAttrib3D(GLuint index, GLdouble x, GLdouble y, GLdouble z) const;
    void vertexAttrib3DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib3F(GLuint index, GLfloat x, GLfloat y, GLfloat z) const;
    void vertexAttrib3FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib3S(GLuint index, GLshort x, GLshort y, GLshort z) const;
    void vertexAttrib3SV(GLuint index, const GLshort *v) const;
    void vertexAttrib4D(GLuint index, GLdouble x, GLdouble y, GLdouble z,
                        GLdouble w) const;
    void vertexAttrib4DV(GLuint index, const GLdouble *v) const;
    void vertexAttrib4F(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
    void vertexAttrib4FV(GLuint index, const GLfloat *v) const;
    void vertexAttrib4S(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w) const;
    void vertexAttrib4SV(GLuint index, const GLshort *v) const;
    void vertexAttrib4UB(GLuint index, GLubyte x, GLubyte y, GLubyte z,
                         GLubyte w) const;
    void vertexAttrib4UBV(GLuint index, const GLubyte *v) const;
    void vertexAttribPointer(GLuint index, GLint fsize, VertexAttribEnumNV type,
                             GLsizei stride, const void *pointer) const;
    void vertexAttribs1DV(GLuint index, GLsizei count, const GLdouble *v) const;
    void vertexAttribs1FV(GLuint index, GLsizei count, const GLfloat *v) const;
    void vertexAttribs1SV(GLuint index, GLsizei count, const GLshort *v) const;
    void vertexAttribs2DV(GLuint index, GLsizei count, const GLdouble *v) const;
    void vertexAttribs2FV(GLuint index, GLsizei count, const GLfloat *v) const;
    void vertexAttribs2SV(GLuint index, GLsizei count, const GLshort *v) const;
    void vertexAttribs3DV(GLuint index, GLsizei count, const GLdouble *v) const;
    void vertexAttribs3FV(GLuint index, GLsizei count, const GLfloat *v) const;
    void vertexAttribs3SV(GLuint index, GLsizei count, const GLshort *v) const;
    void vertexAttribs4DV(GLuint index, GLsizei count, const GLdouble *v) const;
    void vertexAttribs4FV(GLuint index, GLsizei count, const GLfloat *v) const;
    void vertexAttribs4SV(GLuint index, GLsizei count, const GLshort *v) const;
    void vertexAttribs4UBV(GLuint index, GLsizei count, const GLubyte *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVVertexProgram _api;
};

// Manages the entry points for the GL_NV_video_capture extension API.
class NVVideoCaptureAPI : public BaseExtension
{
public:
    // Construction
    NVVideoCaptureAPI();

    // Accessors
    const NVVideoCapture &getRawAPI() const;

    // Operations
    void beginVideoCapture(GLuint video_capture_slot) const;
    void bindVideoCaptureStreamBuffer(GLuint video_capture_slot, GLuint stream,
                                      GLenum frame_region, GLintptrARB offset) const;
    void bindVideoCaptureStreamTexture(GLuint video_capture_slot, GLuint stream,
                                       GLenum frame_region, GLenum target,
                                       TextureName texture) const;
    void endVideoCapture(GLuint video_capture_slot) const;
    void getVideoCaptureStreamDV(GLuint video_capture_slot, GLuint stream,
                                 GLenum pname, GLdouble *params) const;
    void getVideoCaptureStreamFV(GLuint video_capture_slot, GLuint stream,
                                 GLenum pname, GLfloat *params) const;
    void getVideoCaptureStreamIV(GLuint video_capture_slot, GLuint stream,
                                 GLenum pname, GLint *params) const;
    void getVideoCaptureIV(GLuint video_capture_slot, GLenum pname,
                           GLint *params) const;
    GLenum videoCapture(GLuint video_capture_slot, GLuint *sequence_num,
                        GLuint64EXT *capture_time) const;
    void videoCaptureStreamParameterDV(GLuint video_capture_slot, GLuint stream,
                                       GLenum pname, const GLdouble *params) const;
    void videoCaptureStreamParameterFV(GLuint video_capture_slot, GLuint stream,
                                       GLenum pname, const GLfloat *params) const;
    void videoCaptureStreamParameterIV(GLuint video_capture_slot, GLuint stream,
                                       GLenum pname, const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVVideoCapture _api;
};

// Manages the entry points for the GL_NV_viewport_swizzle extension API.
class NVViewportSwizzleAPI : public BaseExtension
{
public:
    // Construction
    NVViewportSwizzleAPI();

    // Accessors
    const NVViewportSwizzle &getRawAPI() const;

    // Operations
    void viewportSwizzle(GLuint index, GLenum swizzlex, GLenum swizzley,
                         GLenum swizzlez, GLenum swizzlew) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    NVViewportSwizzle _api;
};

// Manages the entry points for the GL_OES_byte_coordinates extension API.
class OESByteCoordinatesAPI : public BaseExtension
{
public:
    // Construction
    OESByteCoordinatesAPI();

    // Accessors
    const OESByteCoordinates &getRawAPI() const;

    // Operations
    void multiTexCoord1B(TextureUnit texture, GLbyte s) const;
    void multiTexCoord1BV(TextureUnit texture, const GLbyte *coords) const;
    void multiTexCoord2B(TextureUnit texture, GLbyte s, GLbyte t) const;
    void multiTexCoord2BV(TextureUnit texture, const GLbyte *coords) const;
    void multiTexCoord3B(TextureUnit texture, GLbyte s, GLbyte t, GLbyte r) const;
    void multiTexCoord3BV(TextureUnit texture, const GLbyte *coords) const;
    void multiTexCoord4B(TextureUnit texture, GLbyte s, GLbyte t, GLbyte r,
                         GLbyte q) const;
    void multiTexCoord4BV(TextureUnit texture, const GLbyte *coords) const;
    void texCoord1B(GLbyte s) const;
    void texCoord1BV(const GLbyte *coords) const;
    void texCoord2B(GLbyte s, GLbyte t) const;
    void texCoord2BV(const GLbyte *coords) const;
    void texCoord3B(GLbyte s, GLbyte t, GLbyte r) const;
    void texCoord3BV(const GLbyte *coords) const;
    void texCoord4B(GLbyte s, GLbyte t, GLbyte r, GLbyte q) const;
    void texCoord4BV(const GLbyte *coords) const;
    void vertex2B(GLbyte x, GLbyte y) const;
    void vertex2BV(const GLbyte *coords) const;
    void vertex3B(GLbyte x, GLbyte y, GLbyte z) const;
    void vertex3BV(const GLbyte *coords) const;
    void vertex4B(GLbyte x, GLbyte y, GLbyte z, GLbyte w) const;
    void vertex4BV(const GLbyte *coords) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    OESByteCoordinates _api;
};

// Manages the entry points for the GL_OES_fixed_point extension API.
class OESFixedPointAPI : public BaseExtension
{
public:
    // Construction
    OESFixedPointAPI();

    // Accessors
    const OESFixedPoint &getRawAPI() const;

    // Operations
    void accumx(GLenum op, GLfixed value) const;
    void alphaFuncx(AlphaFunction func, GLfixed ref) const;
    void bitmapx(GLsizei width, GLsizei height, GLfixed xorig, GLfixed yorig,
                 GLfixed xmove, GLfixed ymove, const GLubyte *bitmap) const;
    void blendColorx(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) const;
    void clearAccumx(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) const;
    void clearColorx(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) const;
    void clearDepthx(GLfixed depth) const;
    void clipPlanex(ClipPlaneName plane, const GLfixed *equation) const;
    void color3X(GLfixed red, GLfixed green, GLfixed blue) const;
    void color3XV(const GLfixed *components) const;
    void color4X(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) const;
    void color4XV(const GLfixed *components) const;
    void convolutionParameterx(ConvolutionTargetEXT target,
                               ConvolutionParameterEXT pname, GLfixed param) const;
    void convolutionParameterxV(ConvolutionTargetEXT target,
                                ConvolutionParameterEXT pname,
                                const GLfixed *params) const;
    void depthRangex(GLfixed n, GLfixed f) const;
    void evalCoord1X(GLfixed u) const;
    void evalCoord1XV(const GLfixed *coords) const;
    void evalCoord2X(GLfixed u, GLfixed v) const;
    void evalCoord2XV(const GLfixed *coords) const;
    void feedbackBufferx(GLsizei n, GLenum type, const GLfixed *buffer) const;
    void fogx(GLenum pname, GLfixed param) const;
    void fogxV(GLenum pname, const GLfixed *param) const;
    void frustumx(GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n,
                  GLfixed f) const;
    void getClipPlanex(ClipPlaneName plane, GLfixed *equation) const;
    void getConvolutionParameterxV(GLenum target, GLenum pname, GLfixed *params) const;
    void getFixedv(GetPName pname, GLfixed *params) const;
    void getHistogramParameterxV(HistogramTargetEXT target,
                                 GetHistogramParameterPNameEXT pname,
                                 GLfixed *params) const;
    void getLightx(LightName light, GLenum pname, GLfixed *params) const;
    void getMapxV(MapTarget target, GLenum query, GLfixed *v) const;
    void getMaterialx(MaterialFace face, GLenum pname, GLfixed param) const;
    void getPixelMapxV(GLenum map, GLint size, GLfixed *values) const;
    void getTexEnvxV(GLenum target, GLenum pname, GLfixed *params) const;
    void getTexGenxV(GLenum coord, TextureGenParameter pname, GLfixed *params) const;
    void getTexLevelParameterxV(TextureTarget target, GLint level,
                                GetTextureParameter pname, GLfixed *params) const;
    void getTexParameterxV(TextureTarget target, GetTextureParameter pname,
                           GLfixed *params) const;
    void indexx(GLfixed component) const;
    void indexxV(const GLfixed *component) const;
    void lightModelx(LightModelParameter pname, GLfixed param) const;
    void lightModelxV(LightModelParameter pname, const GLfixed *param) const;
    void lightx(LightName light, GLenum pname, GLfixed param) const;
    void lightxV(LightName light, GLenum pname, const GLfixed *params) const;
    void lineWidthx(GLfixed width) const;
    void loadMatrixx(const GLfixed *m) const;
    void loadTransposeMatrixx(const GLfixed *m) const;
    void map1X(MapTarget target, GLfixed u1, GLfixed u2, GLint stride,
               GLint order, GLfixed points) const;
    void map2X(MapTarget target, GLfixed u1, GLfixed u2, GLint ustride,
               GLint uorder, GLfixed v1, GLfixed v2, GLint vstride, GLint vorder,
               GLfixed points) const;
    void mapGRID1X(GLint n, GLfixed u1, GLfixed u2) const;
    void mapGRID2X(GLint n, GLfixed u1, GLfixed u2, GLfixed v1, GLfixed v2) const;
    void materialx(MaterialFace face, GLenum pname, GLfixed param) const;
    void materialxV(MaterialFace face, GLenum pname, const GLfixed *param) const;
    void multMatrixx(const GLfixed *m) const;
    void multTransposeMatrixx(const GLfixed *m) const;
    void multiTexCoord1X(TextureUnit texture, GLfixed s) const;
    void multiTexCoord1XV(TextureUnit texture, const GLfixed *coords) const;
    void multiTexCoord2X(TextureUnit texture, GLfixed s, GLfixed t) const;
    void multiTexCoord2XV(TextureUnit texture, const GLfixed *coords) const;
    void multiTexCoord3X(TextureUnit texture, GLfixed s, GLfixed t, GLfixed r) const;
    void multiTexCoord3XV(TextureUnit texture, const GLfixed *coords) const;
    void multiTexCoord4X(TextureUnit texture, GLfixed s, GLfixed t, GLfixed r,
                         GLfixed q) const;
    void multiTexCoord4XV(TextureUnit texture, const GLfixed *coords) const;
    void normal3X(GLfixed nx, GLfixed ny, GLfixed nz) const;
    void normal3XV(const GLfixed *coords) const;
    void orthox(GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f) const;
    void passThroughx(GLfixed token) const;
    void pixelMapx(GLenum map, GLint size, const GLfixed *values) const;
    void pixelStorex(PixelStoreParameter pname, GLfixed param) const;
    void pixelTransferx(PixelTransferParameter pname, GLfixed param) const;
    void pixelZoomx(GLfixed xfactor, GLfixed yfactor) const;
    void pointParameterxV(PointParameterNameARB pname, const GLfixed *params) const;
    void pointSizex(GLfixed size) const;
    void polygonOffsetx(GLfixed factor, GLfixed units) const;
    void prioritizeTexturesx(GLsizei n, const TextureName *textures,
                             const GLfixed *priorities) const;
    void rasterPOS2X(GLfixed x, GLfixed y) const;
    void rasterPOS2XV(const GLfixed *coords) const;
    void rasterPOS3X(GLfixed x, GLfixed y, GLfixed z) const;
    void rasterPOS3XV(const GLfixed *coords) const;
    void rasterPOS4X(GLfixed x, GLfixed y, GLfixed z, GLfixed w) const;
    void rasterPOS4XV(const GLfixed *coords) const;
    void rectx(GLfixed x1, GLfixed y1, GLfixed x2, GLfixed y2) const;
    void rectxV(const GLfixed *v1, const GLfixed *v2) const;
    void rotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z) const;
    void scalex(GLfixed x, GLfixed y, GLfixed z) const;
    void texCoord1X(GLfixed s) const;
    void texCoord1XV(const GLfixed *coords) const;
    void texCoord2X(GLfixed s, GLfixed t) const;
    void texCoord2XV(const GLfixed *coords) const;
    void texCoord3X(GLfixed s, GLfixed t, GLfixed r) const;
    void texCoord3XV(const GLfixed *coords) const;
    void texCoord4X(GLfixed s, GLfixed t, GLfixed r, GLfixed q) const;
    void texCoord4XV(const GLfixed *coords) const;
    void texEnvx(GLenum target, GLenum pname, GLfixed param) const;
    void texEnvxV(GLenum target, GLenum pname, const GLfixed *params) const;
    void texGenx(GLenum coord, TextureGenParameter pname, GLfixed param) const;
    void texGenxV(GLenum coord, TextureGenParameter pname, const GLfixed *params) const;
    void texParameterx(TextureTarget target, GetTextureParameter pname,
                       GLfixed param) const;
    void texParameterxV(TextureTarget target, GetTextureParameter pname,
                        const GLfixed *params) const;
    void translatex(GLfixed x, GLfixed y, GLfixed z) const;
    void vertex2X(GLfixed x) const;
    void vertex2XV(const GLfixed *coords) const;
    void vertex3X(GLfixed x, GLfixed y) const;
    void vertex3XV(const GLfixed *coords) const;
    void vertex4X(GLfixed x, GLfixed y, GLfixed z) const;
    void vertex4XV(const GLfixed *coords) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    OESFixedPoint _api;
};

// Manages the entry points for the GL_OES_query_matrix extension API.
class OESQueryMatrixAPI : public BaseExtension
{
public:
    // Construction
    OESQueryMatrixAPI();

    // Accessors
    const OESQueryMatrix &getRawAPI() const;

    // Operations
    GLbitfield queryMatrixx(GLfixed *mantissa, GLint *exponent) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    OESQueryMatrix _api;
};

// Manages the entry points for the GL_OES_single_precision extension API.
class OESSinglePrecisionAPI : public BaseExtension
{
public:
    // Construction
    OESSinglePrecisionAPI();

    // Accessors
    const OESSinglePrecision &getRawAPI() const;

    // Operations
    void clearDepthF(GLclampf depth) const;
    void clipPlaneF(ClipPlaneName plane, const GLfloat *equation) const;
    void depthRangeF(GLclampf n, GLclampf f) const;
    void frustumF(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n,
                  GLfloat f) const;
    void getClipPlaneF(ClipPlaneName plane, GLfloat *equation) const;
    void orthoF(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    OESSinglePrecision _api;
};

// Manages the entry points for the GL_OVR_multiview extension API.
class OvrMultiviewAPI : public BaseExtension
{
public:
    // Construction
    OvrMultiviewAPI();

    // Accessors
    const OvrMultiview &getRawAPI() const;

    // Operations
    void framebufferTextureMultiviewOvr(FramebufferTarget target,
                                        FramebufferAttachment attachment,
                                        TextureName texture, GLint level,
                                        GLint baseViewIndex, GLsizei numViews) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    OvrMultiview _api;
};

// Manages the entry points for the GL_PGI_misc_hints extension API.
class PGIMiscHintsAPI : public BaseExtension
{
public:
    // Construction
    PGIMiscHintsAPI();

    // Accessors
    const PGIMiscHints &getRawAPI() const;

    // Operations
    void hint(HintTargetPGI target, GLint mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    PGIMiscHints _api;
};

// Manages the entry points for the GL_SGIS_detail_texture extension API.
class SGISDetailTextureAPI : public BaseExtension
{
public:
    // Construction
    SGISDetailTextureAPI();

    // Accessors
    const SGISDetailTexture &getRawAPI() const;

    // Operations
    void detailTexFunc(TextureTarget target, GLsizei n, const GLfloat *points) const;
    void getDetailTexFunc(TextureTarget target, GLfloat *points) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISDetailTexture _api;
};

// Manages the entry points for the GL_SGIS_fog_function extension API.
class SGISFogFunctionAPI : public BaseExtension
{
public:
    // Construction
    SGISFogFunctionAPI();

    // Accessors
    const SGISFogFunction &getRawAPI() const;

    // Operations
    void fogFunc(GLsizei n, const GLfloat *points) const;
    void getFogFunc(GLfloat *points) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISFogFunction _api;
};

// Manages the entry points for the GL_SGIS_multisample extension API.
class SGISMultisampleAPI : public BaseExtension
{
public:
    // Construction
    SGISMultisampleAPI();

    // Accessors
    const SGISMultisample &getRawAPI() const;

    // Operations
    void sampleMask(GLclampf value, Boolean invert) const;
    void samplePattern(SamplePatternSGIS pattern) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISMultisample _api;
};

// Manages the entry points for the GL_SGIS_pixel_texture extension API.
class SGISPixelTextureAPI : public BaseExtension
{
public:
    // Construction
    SGISPixelTextureAPI();

    // Accessors
    const SGISPixelTexture &getRawAPI() const;

    // Operations
    void getPixelTexGenParameterFV(PixelTexGenParameterNameSGIS pname,
                                   GLfloat *params) const;
    void getPixelTexGenParameterIV(PixelTexGenParameterNameSGIS pname,
                                   GLint *params) const;
    void pixelTexGenParameterF(PixelTexGenParameterNameSGIS pname, GLfloat param) const;
    void pixelTexGenParameterFV(PixelTexGenParameterNameSGIS pname,
                                const GLfloat *params) const;
    void pixelTexGenParameterI(PixelTexGenParameterNameSGIS pname, GLint param) const;
    void pixelTexGenParameterIV(PixelTexGenParameterNameSGIS pname,
                                const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISPixelTexture _api;
};

// Manages the entry points for the GL_SGIS_point_parameters extension API.
class SGISPointParametersAPI : public BaseExtension
{
public:
    // Construction
    SGISPointParametersAPI();

    // Accessors
    const SGISPointParameters &getRawAPI() const;

    // Operations
    void pointParameterF(PointParameterNameARB pname, GLfloat param) const;
    void pointParameterFV(PointParameterNameARB pname, const GLfloat *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISPointParameters _api;
};

// Manages the entry points for the GL_SGIS_sharpen_texture extension API.
class SGISSharpenTextureAPI : public BaseExtension
{
public:
    // Construction
    SGISSharpenTextureAPI();

    // Accessors
    const SGISSharpenTexture &getRawAPI() const;

    // Operations
    void getSharpenTexFunc(TextureTarget target, GLfloat *points) const;
    void sharpenTexFunc(TextureTarget target, GLsizei n, const GLfloat *points) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISSharpenTexture _api;
};

// Manages the entry points for the GL_SGIS_texture4D extension API.
class SGISTEXTURE4DAPI : public BaseExtension
{
public:
    // Construction
    SGISTEXTURE4DAPI();

    // Accessors
    const SGISTEXTURE4D &getRawAPI() const;

    // Operations
    void texImage4D(TextureTarget target, GLint level,
                    InternalFormat internalformat, GLsizei width, GLsizei height,
                    GLsizei depth, GLsizei size4d, GLint border,
                    PixelFormat format, PixelType type, const void *pixels) const;
    void texSubImage4D(TextureTarget target, GLint level, GLint xoffset,
                       GLint yoffset, GLint zoffset, GLint woffset,
                       GLsizei width, GLsizei height, GLsizei depth,
                       GLsizei size4d, PixelFormat format, PixelType type,
                       const void *pixels) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISTEXTURE4D _api;
};

// Manages the entry points for the GL_SGIS_texture_color_mask extension API.
class SGISTextureColorMaskAPI : public BaseExtension
{
public:
    // Construction
    SGISTextureColorMaskAPI();

    // Accessors
    const SGISTextureColorMask &getRawAPI() const;

    // Operations
    void textureColorMask(Boolean red, Boolean green, Boolean blue,
                          Boolean alpha) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISTextureColorMask _api;
};

// Manages the entry points for the GL_SGIS_texture_filter4 extension API.
class SGISTextureFILTER4API : public BaseExtension
{
public:
    // Construction
    SGISTextureFILTER4API();

    // Accessors
    const SGISTextureFILTER4 &getRawAPI() const;

    // Operations
    void getTexFilterFunc(TextureTarget target, TextureFilterSGIS filter,
                          GLfloat *weights) const;
    void texFilterFunc(TextureTarget target, TextureFilterSGIS filter, GLsizei n,
                       const GLfloat *weights) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGISTextureFILTER4 _api;
};

// Manages the entry points for the GL_SGIX_async extension API.
class SGIXAsyncAPI : public BaseExtension
{
public:
    // Construction
    SGIXAsyncAPI();

    // Accessors
    const SGIXAsync &getRawAPI() const;

    // Operations
    void asyncMarker(GLuint marker) const;
    void deleteAsyncMarkers(GLuint marker, GLsizei range) const;
    GLint finishAsync(GLuint *markerp) const;
    GLuint genAsyncMarkers(GLsizei range) const;
    Boolean isAsyncMarker(GLuint marker) const;
    GLint pollAsync(GLuint *markerp) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXAsync _api;
};

// Manages the entry points for the GL_SGIX_flush_raster extension API.
class SGIXFlushRasterAPI : public BaseExtension
{
public:
    // Construction
    SGIXFlushRasterAPI();

    // Accessors
    const SGIXFlushRaster &getRawAPI() const;

    // Operations
    void flushRaster() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXFlushRaster _api;
};

// Manages the entry points for the GL_SGIX_fragment_lighting extension API.
class SGIXFragmentLightingAPI : public BaseExtension
{
public:
    // Construction
    SGIXFragmentLightingAPI();

    // Accessors
    const SGIXFragmentLighting &getRawAPI() const;

    // Operations
    void fragmentColorMaterial(MaterialFace face, GLenum mode) const;
    void fragmentLightModelF(FragmentLightModelParameterSGIX pname,
                             GLfloat param) const;
    void fragmentLightModelFV(FragmentLightModelParameterSGIX pname,
                              const GLfloat *params) const;
    void fragmentLightModelI(FragmentLightModelParameterSGIX pname, GLint param) const;
    void fragmentLightModelIV(FragmentLightModelParameterSGIX pname,
                              const GLint *params) const;
    void fragmentLightF(FragmentLightNameSGIX light, GLenum pname, GLfloat param) const;
    void fragmentLightFV(FragmentLightNameSGIX light, GLenum pname,
                         const GLfloat *params) const;
    void fragmentLightI(FragmentLightNameSGIX light, GLenum pname, GLint param) const;
    void fragmentLightIV(FragmentLightNameSGIX light, GLenum pname,
                         const GLint *params) const;
    void fragmentMaterialF(MaterialFace face, GLenum pname, GLfloat param) const;
    void fragmentMaterialFV(MaterialFace face, GLenum pname,
                            const GLfloat *params) const;
    void fragmentMaterialI(MaterialFace face, GLenum pname, GLint param) const;
    void fragmentMaterialIV(MaterialFace face, GLenum pname, const GLint *params) const;
    void getFragmentLightFV(FragmentLightNameSGIX light, GLenum pname,
                            GLfloat *params) const;
    void getFragmentLightIV(FragmentLightNameSGIX light, GLenum pname,
                            GLint *params) const;
    void getFragmentMaterialFV(MaterialFace face, GLenum pname, GLfloat *params) const;
    void getFragmentMaterialIV(MaterialFace face, GLenum pname, GLint *params) const;
    void lightEnvI(LightEnvParameterSGIX pname, GLint param) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXFragmentLighting _api;
};

// Manages the entry points for the GL_SGIX_framezoom extension API.
class SGIXFramezoomAPI : public BaseExtension
{
public:
    // Construction
    SGIXFramezoomAPI();

    // Accessors
    const SGIXFramezoom &getRawAPI() const;

    // Operations
    void frameZoom(GLint factor) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXFramezoom _api;
};

// Manages the entry points for the GL_SGIX_igloo_interface extension API.
class SGIXIglooInterfaceAPI : public BaseExtension
{
public:
    // Construction
    SGIXIglooInterfaceAPI();

    // Accessors
    const SGIXIglooInterface &getRawAPI() const;

    // Operations
    void iglooInterface(GLenum pname, const void *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXIglooInterface _api;
};

// Manages the entry points for the GL_SGIX_instruments extension API.
class SGIXInstrumentSAPI : public BaseExtension
{
public:
    // Construction
    SGIXInstrumentSAPI();

    // Accessors
    const SGIXInstrumentS &getRawAPI() const;

    // Operations
    GLint getInstrumentS() const;
    void instrumentSBuffer(GLsizei size, GLint *buffer) const;
    GLint pollInstrumentS(GLint *marker_p) const;
    void readInstrumentS(GLint marker) const;
    void startInstrumentS() const;
    void stopInstrumentS(GLint marker) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXInstrumentS _api;
};

// Manages the entry points for the GL_SGIX_list_priority extension API.
class SGIXListPriorityAPI : public BaseExtension
{
public:
    // Construction
    SGIXListPriorityAPI();

    // Accessors
    const SGIXListPriority &getRawAPI() const;

    // Operations
    void getListParameterFV(GLuint list, ListParameterName pname,
                            GLfloat *params) const;
    void getListParameterIV(GLuint list, ListParameterName pname, GLint *params) const;
    void listParameterF(GLuint list, ListParameterName pname, GLfloat param) const;
    void listParameterFV(GLuint list, ListParameterName pname,
                         const GLfloat *params) const;
    void listParameterI(GLuint list, ListParameterName pname, GLint param) const;
    void listParameterIV(GLuint list, ListParameterName pname,
                         const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXListPriority _api;
};

// Manages the entry points for the GL_SGIX_pixel_texture extension API.
class SGIXPixelTextureAPI : public BaseExtension
{
public:
    // Construction
    SGIXPixelTextureAPI();

    // Accessors
    const SGIXPixelTexture &getRawAPI() const;

    // Operations
    void pixelTexGen(GLenum mode) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXPixelTexture _api;
};

// Manages the entry points for the GL_SGIX_polynomial_ffd extension API.
class SGIXPolynomialFfdAPI : public BaseExtension
{
public:
    // Construction
    SGIXPolynomialFfdAPI();

    // Accessors
    const SGIXPolynomialFfd &getRawAPI() const;

    // Operations
    void deform(GLbitfield  /* FfdMaskSGIX */ mask) const;
    void deformationMap3D(FfdTargetSGIX target, GLdouble u1, GLdouble u2,
                          GLint ustride, GLint uorder, GLdouble v1, GLdouble v2,
                          GLint vstride, GLint vorder, GLdouble w1, GLdouble w2,
                          GLint wstride, GLint worder, const GLdouble *points) const;
    void deformationMap3F(FfdTargetSGIX target, GLfloat u1, GLfloat u2,
                          GLint ustride, GLint uorder, GLfloat v1, GLfloat v2,
                          GLint vstride, GLint vorder, GLfloat w1, GLfloat w2,
                          GLint wstride, GLint worder, const GLfloat *points) const;
    void loadIdentityDeformationMap(GLbitfield  /* FfdMaskSGIX */ mask) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXPolynomialFfd _api;
};

// Manages the entry points for the GL_SGIX_reference_plane extension API.
class SGIXReferencePlaneAPI : public BaseExtension
{
public:
    // Construction
    SGIXReferencePlaneAPI();

    // Accessors
    const SGIXReferencePlane &getRawAPI() const;

    // Operations
    void referencePlane(const GLdouble *equation) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXReferencePlane _api;
};

// Manages the entry points for the GL_SGIX_sprite extension API.
class SGIXSpriteAPI : public BaseExtension
{
public:
    // Construction
    SGIXSpriteAPI();

    // Accessors
    const SGIXSprite &getRawAPI() const;

    // Operations
    void spriteParameterF(SpriteParameterNameSGIX pname, GLfloat param) const;
    void spriteParameterFV(SpriteParameterNameSGIX pname, const GLfloat *params) const;
    void spriteParameterI(SpriteParameterNameSGIX pname, GLint param) const;
    void spriteParameterIV(SpriteParameterNameSGIX pname, const GLint *params) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXSprite _api;
};

// Manages the entry points for the GL_SGIX_tag_sample_buffer extension API.
class SGIXTagSampleBufferAPI : public BaseExtension
{
public:
    // Construction
    SGIXTagSampleBufferAPI();

    // Accessors
    const SGIXTagSampleBuffer &getRawAPI() const;

    // Operations
    void tagSampleBuffer() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIXTagSampleBuffer _api;
};

// Manages the entry points for the GL_SGI_color_table extension API.
class SGIColorTableAPI : public BaseExtension
{
public:
    // Construction
    SGIColorTableAPI();

    // Accessors
    const SGIColorTable &getRawAPI() const;

    // Operations
    void colorTableParameterFV(ColorTableTargetSGI target,
                               ColorTableParameterPNameSGI pname,
                               const GLfloat *params) const;
    void colorTableParameterIV(ColorTableTargetSGI target,
                               ColorTableParameterPNameSGI pname,
                               const GLint *params) const;
    void colorTable(ColorTableTargetSGI target, InternalFormat internalformat,
                    GLsizei width, PixelFormat format, PixelType type,
                    const void *table) const;
    void copyColorTable(ColorTableTargetSGI target,
                        InternalFormat internalformat, GLint x, GLint y,
                        GLsizei width) const;
    void getColorTableParameterFV(ColorTableTargetSGI target,
                                  GetColorTableParameterPNameSGI pname,
                                  GLfloat *params) const;
    void getColorTableParameterIV(ColorTableTargetSGI target,
                                  GetColorTableParameterPNameSGI pname,
                                  GLint *params) const;
    void getColorTable(ColorTableTargetSGI target, PixelFormat format,
                       PixelType type, void *table) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SGIColorTable _api;
};

// Manages the entry points for the GL_SUNX_constant_data extension API.
class SUNXConstantDataAPI : public BaseExtension
{
public:
    // Construction
    SUNXConstantDataAPI();

    // Accessors
    const SUNXConstantData &getRawAPI() const;

    // Operations
    void finishTexture() const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SUNXConstantData _api;
};

// Manages the entry points for the GL_SUN_global_alpha extension API.
class SUNGlobalAlphaAPI : public BaseExtension
{
public:
    // Construction
    SUNGlobalAlphaAPI();

    // Accessors
    const SUNGlobalAlpha &getRawAPI() const;

    // Operations
    void globalAlphaFactorB(GLbyte factor) const;
    void globalAlphaFactorD(GLdouble factor) const;
    void globalAlphaFactorF(GLfloat factor) const;
    void globalAlphaFactorI(GLint factor) const;
    void globalAlphaFactors(GLshort factor) const;
    void globalAlphaFactorUB(GLubyte factor) const;
    void globalAlphaFactorUI(GLuint factor) const;
    void globalAlphaFactorUS(GLushort factor) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SUNGlobalAlpha _api;
};

// Manages the entry points for the GL_SUN_mesh_array extension API.
class SUNMeshArrayAPI : public BaseExtension
{
public:
    // Construction
    SUNMeshArrayAPI();

    // Accessors
    const SUNMeshArray &getRawAPI() const;

    // Operations
    void drawMeshArrays(PrimitiveType mode, GLint first, GLsizei count,
                        GLsizei width) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SUNMeshArray _api;
};

// Manages the entry points for the GL_SUN_triangle_list extension API.
class SUNTriangleListAPI : public BaseExtension
{
public:
    // Construction
    SUNTriangleListAPI();

    // Accessors
    const SUNTriangleList &getRawAPI() const;

    // Operations
    void replacementCodePointer(ReplacementCodeTypeSUN type, GLsizei stride,
                                const void **pointer) const;
    void replacementCodeUB(GLubyte code) const;
    void replacementCodeUBV(const GLubyte *code) const;
    void replacementCodeUI(GLuint code) const;
    void replacementCodeUIV(const GLuint *code) const;
    void replacementCodeUS(GLushort code) const;
    void replacementCodeUSV(const GLushort *code) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SUNTriangleList _api;
};

// Manages the entry points for the GL_SUN_vertex extension API.
class SUNVertexAPI : public BaseExtension
{
public:
    // Construction
    SUNVertexAPI();

    // Accessors
    const SUNVertex &getRawAPI() const;

    // Operations
    void color3FVertex3F(GLfloat r, GLfloat g, GLfloat b, GLfloat x, GLfloat y,
                         GLfloat z) const;
    void color3FVertex3FV(const GLfloat *c, const GLfloat *v) const;
    void color4FNormal3FVertex3F(GLfloat r, GLfloat g, GLfloat b, GLfloat a,
                                 GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x,
                                 GLfloat y, GLfloat z) const;
    void color4FNormal3FVertex3FV(const GLfloat *c, const GLfloat *n,
                                  const GLfloat *v) const;
    void color4UBVertex2F(GLubyte r, GLubyte g, GLubyte b, GLubyte a, GLfloat x,
                          GLfloat y) const;
    void color4UBVertex2FV(const GLubyte *c, const GLfloat *v) const;
    void color4UBVertex3F(GLubyte r, GLubyte g, GLubyte b, GLubyte a, GLfloat x,
                          GLfloat y, GLfloat z) const;
    void color4UBVertex3FV(const GLubyte *c, const GLfloat *v) const;
    void normal3FVertex3F(GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x,
                          GLfloat y, GLfloat z) const;
    void normal3FVertex3FV(const GLfloat *n, const GLfloat *v) const;
    void replacementCodeUIColor3FVertex3F(GLuint rc, GLfloat r, GLfloat g,
                                          GLfloat b, GLfloat x, GLfloat y,
                                          GLfloat z) const;
    void replacementCodeUIColor3FVertex3FV(const GLuint *rc, const GLfloat *c,
                                           const GLfloat *v) const;
    void replacementCodeUIColor4FNormal3FVertex3F(GLuint rc, GLfloat r,
                                                  GLfloat g, GLfloat b,
                                                  GLfloat a, GLfloat nx,
                                                  GLfloat ny, GLfloat nz,
                                                  GLfloat x, GLfloat y,
                                                  GLfloat z) const;
    void replacementCodeUIColor4FNormal3FVertex3FV(const GLuint *rc,
                                                   const GLfloat *c,
                                                   const GLfloat *n,
                                                   const GLfloat *v) const;
    void replacementCodeUIColor4UBVertex3F(GLuint rc, GLubyte r, GLubyte g,
                                           GLubyte b, GLubyte a, GLfloat x,
                                           GLfloat y, GLfloat z) const;
    void replacementCodeUIColor4UBVertex3FV(const GLuint *rc, const GLubyte *c,
                                            const GLfloat *v) const;
    void replacementCodeUINormal3FVertex3F(GLuint rc, GLfloat nx, GLfloat ny,
                                           GLfloat nz, GLfloat x, GLfloat y,
                                           GLfloat z) const;
    void replacementCodeUINormal3FVertex3FV(const GLuint *rc, const GLfloat *n,
                                            const GLfloat *v) const;
    void replacementCodeUITexCoord2FColor4FNormal3FVertex3F(GLuint rc, GLfloat s,
                                                            GLfloat t, GLfloat r,
                                                            GLfloat g, GLfloat b,
                                                            GLfloat a,
                                                            GLfloat nx,
                                                            GLfloat ny,
                                                            GLfloat nz,
                                                            GLfloat x, GLfloat y,
                                                            GLfloat z) const;
    void replacementCodeUITexCoord2FColor4FNormal3FVertex3FV(const GLuint *rc,
                                                             const GLfloat *tc,
                                                             const GLfloat *c,
                                                             const GLfloat *n,
                                                             const GLfloat *v) const;
    void replacementCodeUITexCoord2FNormal3FVertex3F(GLuint rc, GLfloat s,
                                                     GLfloat t, GLfloat nx,
                                                     GLfloat ny, GLfloat nz,
                                                     GLfloat x, GLfloat y,
                                                     GLfloat z) const;
    void replacementCodeUITexCoord2FNormal3FVertex3FV(const GLuint *rc,
                                                      const GLfloat *tc,
                                                      const GLfloat *n,
                                                      const GLfloat *v) const;
    void replacementCodeUITexCoord2FVertex3F(GLuint rc, GLfloat s, GLfloat t,
                                             GLfloat x, GLfloat y, GLfloat z) const;
    void replacementCodeUITexCoord2FVertex3FV(const GLuint *rc,
                                              const GLfloat *tc,
                                              const GLfloat *v) const;
    void replacementCodeUIVertex3F(GLuint rc, GLfloat x, GLfloat y, GLfloat z) const;
    void replacementCodeUIVertex3FV(const GLuint *rc, const GLfloat *v) const;
    void texCoord2FColor3FVertex3F(GLfloat s, GLfloat t, GLfloat r, GLfloat g,
                                   GLfloat b, GLfloat x, GLfloat y, GLfloat z) const;
    void texCoord2FColor3FVertex3FV(const GLfloat *tc, const GLfloat *c,
                                    const GLfloat *v) const;
    void texCoord2FColor4FNormal3FVertex3F(GLfloat s, GLfloat t, GLfloat r,
                                           GLfloat g, GLfloat b, GLfloat a,
                                           GLfloat nx, GLfloat ny, GLfloat nz,
                                           GLfloat x, GLfloat y, GLfloat z) const;
    void texCoord2FColor4FNormal3FVertex3FV(const GLfloat *tc, const GLfloat *c,
                                            const GLfloat *n, const GLfloat *v) const;
    void texCoord2FColor4UBVertex3F(GLfloat s, GLfloat t, GLubyte r, GLubyte g,
                                    GLubyte b, GLubyte a, GLfloat x, GLfloat y,
                                    GLfloat z) const;
    void texCoord2FColor4UBVertex3FV(const GLfloat *tc, const GLubyte *c,
                                     const GLfloat *v) const;
    void texCoord2FNormal3FVertex3F(GLfloat s, GLfloat t, GLfloat nx, GLfloat ny,
                                    GLfloat nz, GLfloat x, GLfloat y, GLfloat z) const;
    void texCoord2FNormal3FVertex3FV(const GLfloat *tc, const GLfloat *n,
                                     const GLfloat *v) const;
    void texCoord2FVertex3F(GLfloat s, GLfloat t, GLfloat x, GLfloat y,
                            GLfloat z) const;
    void texCoord2FVertex3FV(const GLfloat *tc, const GLfloat *v) const;
    void texCoord4FColor4FNormal3FVertex4F(GLfloat s, GLfloat t, GLfloat p,
                                           GLfloat q, GLfloat r, GLfloat g,
                                           GLfloat b, GLfloat a, GLfloat nx,
                                           GLfloat ny, GLfloat nz, GLfloat x,
                                           GLfloat y, GLfloat z, GLfloat w) const;
    void texCoord4FColor4FNormal3FVertex4FV(const GLfloat *tc, const GLfloat *c,
                                            const GLfloat *n, const GLfloat *v) const;
    void texCoord4FVertex4F(GLfloat s, GLfloat t, GLfloat p, GLfloat q,
                            GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
    void texCoord4FVertex4FV(const GLfloat *tc, const GLfloat *v) const;

    // Overrides
    virtual void resolve(const APIResolver* resolver) override;

private:
    SUNVertex _api;
};

} // namespace gl


#endif // ifndef __C887F2C6_25FE_46FA_A234_66CE1D192F9E_INCLUDED__

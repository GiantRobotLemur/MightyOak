//! @file ObjectGL/ResourceWrappers.cpp
//! @brief The definition of a number of internal classes which wrap OpenGL
//! resources.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"
#include "Ag/GLAPI/EnumTypes.hpp"
#include "ResourceWrappers.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// ShaderResource Member Definitions
////////////////////////////////////////////////////////////////////////////////
ShaderResource::ShaderResource(const DisplayContextPrivateSPtr &display,
                               const gl::ShaderName &name) :
    BaseResource(display, name)
{
}

AssignableShader::AssignableShader(const std::shared_ptr<ShaderResource> &resource) :
    Shader(resource)
{
}

////////////////////////////////////////////////////////////////////////////////
// ProgramResource Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a shareable object to represent a Program component.
//! @param[in] display A reference to the display the resource belongs to.
//! @param[in] name The identifier of the resource.
ProgramResource::ProgramResource(const DisplayContextPrivateSPtr &display,
                                 const gl::ProgramName &name) :
    BaseResource(display, name)
{
}

AssignableProgram::AssignableProgram(const std::shared_ptr<ProgramResource> &resource) :
                                     Program(resource)
{
}

////////////////////////////////////////////////////////////////////////////////
// BufferResource Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a shareable object to represent a Program component.
//! @param[in] display A reference to the display the resource belongs to.
//! @param[in] name The identifier of the resource.
BufferResource::BufferResource(const DisplayContextPrivateSPtr &display,
                               const gl::BufferName &name) :
    BaseResource(display, name)
{
}

AssignableBuffer::AssignableBuffer(const std::shared_ptr<BufferResource> &resource) :
        Buffer(resource)
{
}

////////////////////////////////////////////////////////////////////////////////
// IndexBufferResource Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a shareable object to represent a Program component.
//! @param[in] display A reference to the display the resource belongs to.
//! @param[in] name The identifier of the resource.
IndexBufferResource::IndexBufferResource(const DisplayContextPrivateSPtr &display,
                    const gl::BufferName &name) :
    BufferResource(display, name),
    _restartIndex(UINT32_MAX),
    _dataType(gl::DrawElementsType::UnsignedInt),
    _usesPrimitiveRestart(false)
{
}

//! @brief Gets the encoding of indices currently held in the buffer.
gl::DrawElementsType IndexBufferResource::getDataType() const
{
    return _dataType;
}

//! @brief Sets the encoding of indices currently held in the buffer.
//! @param[in] dataType The new index encoding.
void IndexBufferResource::setDataType(gl::DrawElementsType dataType)
{
    _dataType = dataType;
}

//! @brief Gets the index used to indicate the restart of a primitive.
uint32_t IndexBufferResource::getRestartIndex() const
{
    return _restartIndex;
}

//! @brief Sets the index used to indicate the restart of a primitive.
//! @param[in] index The new restart index, must be appropriate given the
//! data type used to encode indices.
void IndexBufferResource::setRestartIndex(uint32_t index)
{
    _restartIndex = index;
}

//! @brief Gets whether the buffer is encoded with markers to indicate the end
//! of a primitive.
//! @retval true The buffer is encoded with a restart index.
//! @retval false The buffer does not use primitive restart.
bool IndexBufferResource::usesPrimitiveRestart() const
{
    return _usesPrimitiveRestart;
}

//! @brief Sets whether the buffer is encoded with markers to indicate the end
//! of a primitive.
//! @param[in] isEnabled True to indicate that the buffer uses primitive restart,
//! false if not.
void IndexBufferResource::setUsePrimitiveRestart(bool isEnabled)
{
    _usesPrimitiveRestart = isEnabled;
}

//! @brief Constructs an IndexBuffer object and assigns it a resource.
//! @param[in] buffer The resource to assign.
AssignableIndexBuffer::AssignableIndexBuffer(const std::shared_ptr<IndexBufferResource> &buffer) :
    IndexBuffer(buffer)
{
}

////////////////////////////////////////////////////////////////////////////////
// VertexBufferResource Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing a buffer specifically intended to
//! contain vertex data.
//! @param[in] display A reference to the display the resource belongs to.
//! @param[in] name The identifier of the resource.
VertexBufferResource::VertexBufferResource(const DisplayContextPrivateSPtr &display,
                                           const gl::BufferName &name) :
    BufferResource(display, name),
    _schema(VertexAttribCollection(), 0)
{
}

//! @brief Gets the schema which describes the structure of the vertex data.
const VertexSchema &VertexBufferResource::getSchema() const { return _schema; }

//! @brief Sets the schema which describes the structure of the vertex data.
//! @param[in] schema The new schema which will annotate the buffer.
void VertexBufferResource::setSchema(const VertexSchema &schema)
{
    _schema = schema;
}

//! @brief Moves a schema describing the structure of the vertex data into the
//! object.
//! @param[in] schema The schema which will annotate the buffer.
void VertexBufferResource::setSchema(VertexSchema &&schema)
{
    _schema = schema;
}

//! @brief Constructs an VertexBuffer object and assigns it a resource.
//! @param[in] buffer The resource to assign.
AssignableVertexBuffer::AssignableVertexBuffer(const std::shared_ptr<VertexBufferResource> &buffer) :
    VertexBuffer(buffer)
{
}

////////////////////////////////////////////////////////////////////////////////
// VAOResource Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to manage the lifetime of an OpenGL Vertex
//! Array Object resource.
//! @param[in] display A reference to the display the resource belongs to.
//! @param[in] name The unique identifier of the resource.
VAOResource::VAOResource(const DisplayContextPrivateSPtr &display,
                         const gl::VertexArrayName &name) :
    BaseResource<gl::VertexArrayName>(display, name)
{
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


//! @file ObjectGL/ResourceWrappers.hpp
//! @brief The declaration of a number of internal classes which wrap OpenGL
//! resources.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_RESOURCE_WRAPPERS_HPP__
#define __AG_OBJECT_GL_RESOURCE_WRAPPERS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include "Ag/Core/Exception.hpp"
#include "Ag/GLAPI/Identifiers.hpp"

#include "Ag/ObjectGL/Buffer.hpp"
#include "Ag/ObjectGL/Program.hpp"
#include "Ag/ObjectGL/Shader.hpp"
#include "Ag/ObjectGL/IndexBuffer.hpp"
#include "Ag/ObjectGL/VertexSchema.hpp"
#include "Ag/ObjectGL/VertexBuffer.hpp"
#include "Ag/ObjectGL/VertexArrayObject.hpp"
#include "DisplayContextPrivate.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A template for a shareable object which wraps an OpenGL resource.
//! @tparam TName The data type of the object identifying the resource.
template<typename TName>
class BaseResource : public std::enable_shared_from_this<BaseResource<TName>>
{
public:
    // Construction/Destruction
    //! @brief Constructs a shareable object to represent a named resource.
    //! @param[in] display The display which owns the resource.
    //! @param[in] name The identifier of the resource to wrap.
    BaseResource(const DisplayContextPrivateSPtr &display, const TName &name) :
        _display(display),
        _name(name)
    {
        // Ensure the object has a valid state.
        if (name.ID == 0)
        {
            throw Ag::ArgumentException("name");
        }
        else if (!display)
        {
            throw Ag::ArgumentException("display");
        }
    }

    //! @brief Ensures that the resource is scheduled for destruction.
    ~BaseResource()
    {
        if (_display && (_name.ID != 0))
        {
            _display->dispose(_name);
            _name.ID = 0;
        }
    }

    // Accessors
    //! @brief Gets the identifier of the underlying resource.
    TName getName() const { return _name; }

    //! @brief Gets the display which owns the resource.
    const DisplayContextPrivateSPtr &getDisplay() const { return _display; }
private:
    // Internal Fields
    DisplayContextPrivateSPtr _display;
    TName _name;
};

//! @brief A shareable object wrapping an OpenGL shader resource.
class ShaderResource : public BaseResource<gl::ShaderName>
{
public:
    // Construction/Destruction
    ShaderResource(const DisplayContextPrivateSPtr &display,
                   const gl::ShaderName &name);
    ~ShaderResource() = default;
};

//! @brief A class derived from Shader which can be initialised
//! with a resource.
class AssignableShader : public Shader
{
public:
    AssignableShader(const std::shared_ptr<ShaderResource> &resource);
    ~AssignableShader() = default;
};

//! @brief A shareable object wrapping an OpenGL Program resource.
class ProgramResource : public BaseResource<gl::ProgramName>
{
public:
    // Construction/Destruction
    ProgramResource(const DisplayContextPrivateSPtr &display,
                    const gl::ProgramName &name);
    ~ProgramResource() = default;
};

//! @brief A class derived from Program which can be initialised
//! with a resource.
class AssignableProgram : public Program
{
public:
    AssignableProgram(const std::shared_ptr<ProgramResource> &resource);
    ~AssignableProgram() = default;
};

//! @brief A shareable object wrapping an OpenGL Buffer resource.
class BufferResource : public BaseResource<gl::BufferName>
{
public:
    // Construction/Destruction
    BufferResource(const DisplayContextPrivateSPtr &display,
                   const gl::BufferName &name);
    ~BufferResource() = default;
};

//! @brief A class derived from Buffer which can be initialised
//! with a resource.
class AssignableBuffer : public Buffer
{
public:
    AssignableBuffer(const std::shared_ptr<BufferResource> &resource);
    ~AssignableBuffer() = default;
};

//! @brief A buffer resource specifically intended to hold index data.
class IndexBufferResource : public BufferResource
{
public:
    IndexBufferResource(const DisplayContextPrivateSPtr &display,
                        const gl::BufferName &name);
    ~IndexBufferResource() = default;

    // Accessors
    gl::DrawElementsType getDataType() const;
    void setDataType(gl::DrawElementsType dataType);
    uint32_t getRestartIndex() const;
    void setRestartIndex(uint32_t index);
    bool usesPrimitiveRestart() const;
    void setUsePrimitiveRestart(bool isEnabled);

private:
    // Internal Fields
    uint32_t _restartIndex;
    gl::DrawElementsType _dataType;
    bool _usesPrimitiveRestart;
};

//! @brief A class derived from IndexBuffer which can be initialised
//! with a resource.
class AssignableIndexBuffer : public IndexBuffer
{
public:
    AssignableIndexBuffer(const std::shared_ptr<IndexBufferResource> &buffer);
    ~AssignableIndexBuffer() = default;
};

//! @brief A buffer resource specifically intended to hold index data.
class VertexBufferResource : public BufferResource
{
public:
    // Construction/Destruction
    VertexBufferResource(const DisplayContextPrivateSPtr &display,
                         const gl::BufferName &name);
    ~VertexBufferResource() = default;

    // Accessors
    const VertexSchema &getSchema() const;
    void setSchema(const VertexSchema &schema);
    void setSchema(VertexSchema &&schema);

private:
    // Internal Fields
    VertexSchema _schema;
};

//! @brief A class derived from VertexBuffer which can be initialised
//! with a resource.
class AssignableVertexBuffer : public VertexBuffer
{
public:
    AssignableVertexBuffer(const std::shared_ptr<VertexBufferResource> &buffer);
    ~AssignableVertexBuffer() = default;
};

//! @brief A class which manages a Vertex Array Object resource.
class VAOResource : public BaseResource<gl::VertexArrayName>
{
public:
    // Construction/Destruction
    VAOResource(const DisplayContextPrivateSPtr &display,
                const gl::VertexArrayName &name);
    ~VAOResource() = default;
};

//! @brief An object derived from VertexArrayObject which can be bound to
//! a VAO resource.
class AssignableVAO : public VertexArrayObject
{
public:
    AssignableVAO(const std::shared_ptr<VAOResource> &vao) :
        VertexArrayObject(vao)
    {
    }

    ~AssignableVAO() = default;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

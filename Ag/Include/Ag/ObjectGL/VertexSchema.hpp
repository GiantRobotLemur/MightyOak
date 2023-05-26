//! @file ObjectGL/VertexSchema.hpp
//! @brief The declaration of an object which describes the format of an
//! OpenGL vertex buffer resource.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_VERTEX_SCHEMA_HPP__
#define __AG_OBJECT_GL_VERTEX_SCHEMA_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "Ag/Core/LinearSortedMap.hpp"
#include "Ag/Core/String.hpp"

#include "Ag/GLAPI/EnumTypes.hpp"

////////////////////////////////////////////////////////////////////////////////
// Data Types
////////////////////////////////////////////////////////////////////////////////
namespace gl {
class GLAPI;
}

namespace Ag {
namespace GL {

//! @brief The allowed data types of elements of a vertex.
enum class VertexAttribType : uint8_t
{
    Int8,   // OpenGL 3.0+
    Int8Norm,
    Uint8,  // OpenGL 3.0+
    Uint8Norm,
    Int16,  // OpenGL 3.0+
    Int16Norm,
    Uint16, // OpenGL 3.0+
    Uint16Norm,
    Int32,  // OpenGL 3.0+
    Int32Norm,
    Uint32, // OpenGL 3.0+
    Uint32Norm,
    Float,
    HalfFloat,
    Double, // OpenGL 4.1+
    Fixed,
    IntTriple_2_10_10_10Norm,
    UintTriple_2_10_10_10Norm,
    UintTriple_10F_11F_11FNorm,

    Max,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
struct VertexAttribTypeInfo;

//! @brief An object which describes a field within an OpenGL vertex buffer.
class VertexAttrib
{
public:
    // Construction/Destruction
    VertexAttrib(const std::string_view &name, VertexAttribType dataType,
                 size_t offset, int elementCount = -1);

    // Accessors
    string_cref_t getName() const;
    VertexAttribType getDataType() const;
    uint32_t getOffset() const;
    uint32_t getTotalSize() const;
    uint8_t getElementCount() const;
    bool isNormalised() const;

    // Operations
    void define(const gl::GLAPI &api, uint32_t attributeId, size_t vertexSize) const;
private:
    // Internal Fields
    String _name;
    uint32_t _offset;
    VertexAttribType _dataType;
    const VertexAttribTypeInfo *_dataTypeInfo;
    uint8_t _elementCount;
};

using VertexAttribCollection = std::vector<VertexAttrib>;

//! @brief An alias for an optimised map of the index of a VertexAttribute
//! in a schema to its index as referenced in a vertex shader program.
using VertexAttribMapping = Ag::LinearSortedMap<size_t, uint32_t>;

//! @brief An object which describes the format of an OpenGL vertex buffer
//! resource.
class VertexSchema
{
public:
    // Construction/Destruction
    VertexSchema();
    VertexSchema(const VertexAttribCollection &attributes,
                 size_t preferredStride = 0);
    ~VertexSchema() = default;

    // Accessors
    bool tryFindAttributeByName(string_cref_t name, size_t &index) const;

    // Operations
    void define(const gl::GLAPI &api,
                const VertexAttribMapping &mappings) const;
private:
    // Internal Fields
    VertexAttribCollection _attributes;
    std::unordered_map<Ag::String, size_t> _nameIndex;
    size_t _stride;
};


//! @brief An object which constructs a description of the format of an
//! OpenGL buffer.
class VertexSchemaBuilder
{
public:
    // Construction/Destruction
    VertexSchemaBuilder();

    // Accessors
    void setVertexSize(size_t byteCount);

    // Operations
    void clear();
    void add2DPosition(const std::string_view &name, size_t offset);
    void add3DPosition(const std::string_view &name, size_t offset);
    void add4DPosition(const std::string_view &name, size_t offset);
    void addHDRColour(const std::string_view &name, size_t offset);
    void addHDRColourWithAlpha(const std::string_view &name, size_t offset);
    void addPackedIntegerColour(const std::string_view &name, size_t offset);
    void addAttribute(const std::string_view &name, VertexAttribType dataType,
                      size_t offset, int elementCount = -1);
    VertexSchema createSchema();
private:
    // Internal Fields
    VertexAttribCollection _attributes;
    size_t _preferredSize;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

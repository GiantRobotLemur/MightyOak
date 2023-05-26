//! @file Ag/GLAPI/CommandSets.hpp
//! @brief A file generated from the OpenGL API schema definition to create
//! C++ 17 language bindings.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __EFCB0C06_7A7A_46A0_B04E_C3925634D0FD_INCLUDED__
#define __EFCB0C06_7A7A_46A0_B04E_C3925634D0FD_INCLUDED__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "BaseTypes.hpp"

namespace gl {
////////////////////////////////////////////////////////////////////////////////
// Global Constants
////////////////////////////////////////////////////////////////////////////////
// Tokens whose numeric value is intrinsically meaningful
static constexpr unsigned long long TimeoutIgnored = 0xFFFFFFFFFFFFFFFFull; // GL_TIMEOUT_IGNORED - Tagged as uint64
static constexpr GLenum False                      =                     0; // GL_FALSE
static constexpr GLenum NoError                    =                     0; // GL_NO_ERROR
static constexpr GLenum None                       =                     0; // GL_NONE
static constexpr GLenum Zero                       =                     0; // GL_ZERO
static constexpr GLenum One                        =                     1; // GL_ONE
static constexpr GLenum True                       =                     1; // GL_TRUE
static constexpr GLenum LuidSize                   =                     8; // GL_LUID_SIZE_EXT
static constexpr GLenum UuidSize                   =                    16; // GL_UUID_SIZE_EXT
static constexpr GLenum AllPixels                  =            0xFFFFFFFF; // GL_ALL_PIXELS_AMD
static constexpr unsigned int InvalidIndex         =           0xFFFFFFFFu; // GL_INVALID_INDEX - Tagged as uint

////////////////////////////////////////////////////////////////////////////////
// Enumeration Types
////////////////////////////////////////////////////////////////////////////////
enum class AlphaFunction : GLenum {
    Never    = 0x0200, // GL_NEVER
    Less     = 0x0201, // GL_LESS
    Equal    = 0x0202, // GL_EQUAL
    Lequal   = 0x0203, // GL_LEQUAL
    Greater  = 0x0204, // GL_GREATER
    Notequal = 0x0205, // GL_NOTEQUAL
    Gequal   = 0x0206, // GL_GEQUAL
    Always   = 0x0207, // GL_ALWAYS
}; // enum class AlphaFunction

enum class ArrayObjectPNameATI : GLenum {
    ObjectBufferSize  = 0x8764, // GL_OBJECT_BUFFER_SIZE_ATI
    ObjectBufferUsage = 0x8765, // GL_OBJECT_BUFFER_USAGE_ATI
}; // enum class ArrayObjectPNameATI

enum class ArrayObjectUsageATI : GLenum {
    Static  = 0x8760, // GL_STATIC_ATI
    Dynamic = 0x8761, // GL_DYNAMIC_ATI
}; // enum class ArrayObjectUsageATI

enum class AtomicCounterBufferPName : GLenum {
    AtomicCounterBufferReferencedByComputeShader        = 0x90ED, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER
    AtomicCounterBufferBinding                          = 0x92C1, // GL_ATOMIC_COUNTER_BUFFER_BINDING
    AtomicCounterBufferDataSize                         = 0x92C4, // GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE
    AtomicCounterBufferActiveAtomicCounters             = 0x92C5, // GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS
    AtomicCounterBufferActiveAtomicCounterIndices       = 0x92C6, // GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES
    AtomicCounterBufferReferencedByVertexShader         = 0x92C7, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER
    AtomicCounterBufferReferencedByTessControlShader    = 0x92C8, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER
    AtomicCounterBufferReferencedByTessEvaluationShader = 0x92C9, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER
    AtomicCounterBufferReferencedByGeometryShader       = 0x92CA, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER
    AtomicCounterBufferReferencedByFragmentShader       = 0x92CB, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER
}; // enum class AtomicCounterBufferPName

struct AttribMask {
    static constexpr GLenum DepthBuffer        = 0x00000100; // GL_DEPTH_BUFFER_BIT
    static constexpr GLenum StencilBuffer      = 0x00000400; // GL_STENCIL_BUFFER_BIT
    static constexpr GLenum ColorBuffer        = 0x00004000; // GL_COLOR_BUFFER_BIT
    static constexpr GLenum ColourBuffer       = 0x00004000; // GL_COLOR_BUFFER_BIT
    static constexpr GLenum MultisampleBit3Dfx = 0x20000000; // GL_MULTISAMPLE_BIT_3DFX
    static constexpr GLenum Multisample        = 0x20000000; // GL_MULTISAMPLE_BIT_ARB
}; // struct AttribMask

enum class AttributeType : GLenum {
    Int                                  = 0x1404, // GL_INT
    UnsignedInt                          = 0x1405, // GL_UNSIGNED_INT
    Float                                = 0x1406, // GL_FLOAT
    Double                               = 0x140A, // GL_DOUBLE
    Int64                                = 0x140E, // GL_INT64_ARB
    UnsignedInt64                        = 0x140F, // GL_UNSIGNED_INT64_ARB
    FloatVec2                            = 0x8B50, // GL_FLOAT_VEC2
    FloatVec3                            = 0x8B51, // GL_FLOAT_VEC3
    FloatVec4                            = 0x8B52, // GL_FLOAT_VEC4
    IntVec2                              = 0x8B53, // GL_INT_VEC2
    IntVec3                              = 0x8B54, // GL_INT_VEC3
    IntVec4                              = 0x8B55, // GL_INT_VEC4
    Bool                                 = 0x8B56, // GL_BOOL
    BoolVec2                             = 0x8B57, // GL_BOOL_VEC2
    BoolVec3                             = 0x8B58, // GL_BOOL_VEC3
    BoolVec4                             = 0x8B59, // GL_BOOL_VEC4
    FloatMat2                            = 0x8B5A, // GL_FLOAT_MAT2
    FloatMat3                            = 0x8B5B, // GL_FLOAT_MAT3
    FloatMat4                            = 0x8B5C, // GL_FLOAT_MAT4
    Sampler1D                            = 0x8B5D, // GL_SAMPLER_1D
    Sampler2D                            = 0x8B5E, // GL_SAMPLER_2D
    Sampler3D                            = 0x8B5F, // GL_SAMPLER_3D
    SamplerCube                          = 0x8B60, // GL_SAMPLER_CUBE
    Sampler1DShadow                      = 0x8B61, // GL_SAMPLER_1D_SHADOW
    Sampler2DShadow                      = 0x8B62, // GL_SAMPLER_2D_SHADOW
    Sampler2DRect                        = 0x8B63, // GL_SAMPLER_2D_RECT
    Sampler2DRectShadow                  = 0x8B64, // GL_SAMPLER_2D_RECT_SHADOW
    FloatMat2x3                          = 0x8B65, // GL_FLOAT_MAT2x3
    FloatMat2x4                          = 0x8B66, // GL_FLOAT_MAT2x4
    FloatMat3x2                          = 0x8B67, // GL_FLOAT_MAT3x2
    FloatMat3x4                          = 0x8B68, // GL_FLOAT_MAT3x4
    FloatMat4x2                          = 0x8B69, // GL_FLOAT_MAT4x2
    FloatMat4x3                          = 0x8B6A, // GL_FLOAT_MAT4x3
    SamplerBuffer                        = 0x8DC2, // GL_SAMPLER_BUFFER
    Sampler1DArrayShadow                 = 0x8DC3, // GL_SAMPLER_1D_ARRAY_SHADOW
    Sampler2DArrayShadow                 = 0x8DC4, // GL_SAMPLER_2D_ARRAY_SHADOW
    SamplerCubeShadow                    = 0x8DC5, // GL_SAMPLER_CUBE_SHADOW
    UnsignedIntVec2                      = 0x8DC6, // GL_UNSIGNED_INT_VEC2
    UnsignedIntVec3                      = 0x8DC7, // GL_UNSIGNED_INT_VEC3
    UnsignedIntVec4                      = 0x8DC8, // GL_UNSIGNED_INT_VEC4
    IntSampler1D                         = 0x8DC9, // GL_INT_SAMPLER_1D
    IntSampler2D                         = 0x8DCA, // GL_INT_SAMPLER_2D
    IntSampler3D                         = 0x8DCB, // GL_INT_SAMPLER_3D
    IntSamplerCube                       = 0x8DCC, // GL_INT_SAMPLER_CUBE
    IntSampler2DRect                     = 0x8DCD, // GL_INT_SAMPLER_2D_RECT
    IntSampler1DArray                    = 0x8DCE, // GL_INT_SAMPLER_1D_ARRAY
    IntSampler2DArray                    = 0x8DCF, // GL_INT_SAMPLER_2D_ARRAY
    IntSamplerBuffer                     = 0x8DD0, // GL_INT_SAMPLER_BUFFER
    UnsignedIntSampler1D                 = 0x8DD1, // GL_UNSIGNED_INT_SAMPLER_1D
    UnsignedIntSampler2D                 = 0x8DD2, // GL_UNSIGNED_INT_SAMPLER_2D
    UnsignedIntSampler3D                 = 0x8DD3, // GL_UNSIGNED_INT_SAMPLER_3D
    UnsignedIntSamplerCube               = 0x8DD4, // GL_UNSIGNED_INT_SAMPLER_CUBE
    UnsignedIntSampler2DRect             = 0x8DD5, // GL_UNSIGNED_INT_SAMPLER_2D_RECT
    UnsignedIntSampler1DArray            = 0x8DD6, // GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
    UnsignedIntSampler2DArray            = 0x8DD7, // GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
    UnsignedIntSamplerBuffer             = 0x8DD8, // GL_UNSIGNED_INT_SAMPLER_BUFFER
    DoubleMat2                           = 0x8F46, // GL_DOUBLE_MAT2
    DoubleMat3                           = 0x8F47, // GL_DOUBLE_MAT3
    DoubleMat4                           = 0x8F48, // GL_DOUBLE_MAT4
    DoubleMat2x3                         = 0x8F49, // GL_DOUBLE_MAT2x3
    DoubleMat2x4                         = 0x8F4A, // GL_DOUBLE_MAT2x4
    DoubleMat3x2                         = 0x8F4B, // GL_DOUBLE_MAT3x2
    DoubleMat3x4                         = 0x8F4C, // GL_DOUBLE_MAT3x4
    DoubleMat4x2                         = 0x8F4D, // GL_DOUBLE_MAT4x2
    DoubleMat4x3                         = 0x8F4E, // GL_DOUBLE_MAT4x3
    Int64Vec2                            = 0x8FE9, // GL_INT64_VEC2_ARB
    Int64Vec3                            = 0x8FEA, // GL_INT64_VEC3_ARB
    Int64Vec4                            = 0x8FEB, // GL_INT64_VEC4_ARB
    UnsignedInt64Vec2                    = 0x8FF5, // GL_UNSIGNED_INT64_VEC2_ARB
    UnsignedInt64Vec3                    = 0x8FF6, // GL_UNSIGNED_INT64_VEC3_ARB
    UnsignedInt64Vec4                    = 0x8FF7, // GL_UNSIGNED_INT64_VEC4_ARB
    DoubleVec2                           = 0x8FFC, // GL_DOUBLE_VEC2
    DoubleVec3                           = 0x8FFD, // GL_DOUBLE_VEC3
    DoubleVec4                           = 0x8FFE, // GL_DOUBLE_VEC4
    SamplerCubeMapArray                  = 0x900C, // GL_SAMPLER_CUBE_MAP_ARRAY
    SamplerCubeMapArrayShadow            = 0x900D, // GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW
    IntSamplerCubeMapArray               = 0x900E, // GL_INT_SAMPLER_CUBE_MAP_ARRAY
    UnsignedIntSamplerCubeMapArray       = 0x900F, // GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY
    Image1D                              = 0x904C, // GL_IMAGE_1D
    Image2D                              = 0x904D, // GL_IMAGE_2D
    Image3D                              = 0x904E, // GL_IMAGE_3D
    Image2DRect                          = 0x904F, // GL_IMAGE_2D_RECT
    ImageCube                            = 0x9050, // GL_IMAGE_CUBE
    ImageBuffer                          = 0x9051, // GL_IMAGE_BUFFER
    Image1DArray                         = 0x9052, // GL_IMAGE_1D_ARRAY
    Image2DArray                         = 0x9053, // GL_IMAGE_2D_ARRAY
    ImageCubeMapArray                    = 0x9054, // GL_IMAGE_CUBE_MAP_ARRAY
    Image2DMultisample                   = 0x9055, // GL_IMAGE_2D_MULTISAMPLE
    Image2DMultisampleArray              = 0x9056, // GL_IMAGE_2D_MULTISAMPLE_ARRAY
    IntImage1D                           = 0x9057, // GL_INT_IMAGE_1D
    IntImage2D                           = 0x9058, // GL_INT_IMAGE_2D
    IntImage3D                           = 0x9059, // GL_INT_IMAGE_3D
    IntImage2DRect                       = 0x905A, // GL_INT_IMAGE_2D_RECT
    IntImageCube                         = 0x905B, // GL_INT_IMAGE_CUBE
    IntImageBuffer                       = 0x905C, // GL_INT_IMAGE_BUFFER
    IntImage1DArray                      = 0x905D, // GL_INT_IMAGE_1D_ARRAY
    IntImage2DArray                      = 0x905E, // GL_INT_IMAGE_2D_ARRAY
    IntImageCubeMapArray                 = 0x905F, // GL_INT_IMAGE_CUBE_MAP_ARRAY
    IntImage2DMultisample                = 0x9060, // GL_INT_IMAGE_2D_MULTISAMPLE
    IntImage2DMultisampleArray           = 0x9061, // GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY
    UnsignedIntImage1D                   = 0x9062, // GL_UNSIGNED_INT_IMAGE_1D
    UnsignedIntImage2D                   = 0x9063, // GL_UNSIGNED_INT_IMAGE_2D
    UnsignedIntImage3D                   = 0x9064, // GL_UNSIGNED_INT_IMAGE_3D
    UnsignedIntImage2DRect               = 0x9065, // GL_UNSIGNED_INT_IMAGE_2D_RECT
    UnsignedIntImageCube                 = 0x9066, // GL_UNSIGNED_INT_IMAGE_CUBE
    UnsignedIntImageBuffer               = 0x9067, // GL_UNSIGNED_INT_IMAGE_BUFFER
    UnsignedIntImage1DArray              = 0x9068, // GL_UNSIGNED_INT_IMAGE_1D_ARRAY
    UnsignedIntImage2DArray              = 0x9069, // GL_UNSIGNED_INT_IMAGE_2D_ARRAY
    UnsignedIntImageCubeMapArray         = 0x906A, // GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY
    UnsignedIntImage2DMultisample        = 0x906B, // GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE
    UnsignedIntImage2DMultisampleArray   = 0x906C, // GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY
    Sampler2DMultisample                 = 0x9108, // GL_SAMPLER_2D_MULTISAMPLE
    IntSampler2DMultisample              = 0x9109, // GL_INT_SAMPLER_2D_MULTISAMPLE
    UnsignedIntSampler2DMultisample      = 0x910A, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
    Sampler2DMultisampleArray            = 0x910B, // GL_SAMPLER_2D_MULTISAMPLE_ARRAY
    IntSampler2DMultisampleArray         = 0x910C, // GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    UnsignedIntSampler2DMultisampleArray = 0x910D, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
}; // enum class AttributeType

enum class BindTransformFeedbackTarget : GLenum {
    TransformFeedback = 0x8E22, // GL_TRANSFORM_FEEDBACK
}; // enum class BindTransformFeedbackTarget

enum class BinormalPointerTypeEXT : GLenum {
    Byte   = 0x1400, // GL_BYTE
    Short  = 0x1402, // GL_SHORT
    Int    = 0x1404, // GL_INT
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class BinormalPointerTypeEXT

enum class BlendEquationModeEXT : GLenum {
    FuncAdd             = 0x8006, // GL_FUNC_ADD
    Min                 = 0x8007, // GL_MIN
    Max                 = 0x8008, // GL_MAX
    FuncSubtract        = 0x800A, // GL_FUNC_SUBTRACT
    FuncReverseSubtract = 0x800B, // GL_FUNC_REVERSE_SUBTRACT
    AlphaMin            = 0x8320, // GL_ALPHA_MIN_SGIX
    AlphaMax            = 0x8321, // GL_ALPHA_MAX_SGIX
}; // enum class BlendEquationModeEXT

enum class BlendingFactor : GLenum {
    Zero                  =      0, // GL_ZERO
    One                   =      1, // GL_ONE
    SrcColor              = 0x0300, // GL_SRC_COLOR
    OneMinusSrcColor      = 0x0301, // GL_ONE_MINUS_SRC_COLOR
    SrcAlpha              = 0x0302, // GL_SRC_ALPHA
    OneMinusSrcAlpha      = 0x0303, // GL_ONE_MINUS_SRC_ALPHA
    DstAlpha              = 0x0304, // GL_DST_ALPHA
    OneMinusDstAlpha      = 0x0305, // GL_ONE_MINUS_DST_ALPHA
    DstColor              = 0x0306, // GL_DST_COLOR
    OneMinusDstColor      = 0x0307, // GL_ONE_MINUS_DST_COLOR
    SrcAlphaSaturate      = 0x0308, // GL_SRC_ALPHA_SATURATE
    ConstantColor         = 0x8001, // GL_CONSTANT_COLOR
    OneMinusConstantColor = 0x8002, // GL_ONE_MINUS_CONSTANT_COLOR
    ConstantAlpha         = 0x8003, // GL_CONSTANT_ALPHA
    OneMinusConstantAlpha = 0x8004, // GL_ONE_MINUS_CONSTANT_ALPHA
    Src1Alpha             = 0x8589, // GL_SRC1_ALPHA
    Src1Color             = 0x88F9, // GL_SRC1_COLOR
    OneMinusSrc1Color     = 0x88FA, // GL_ONE_MINUS_SRC1_COLOR
    OneMinusSrc1Alpha     = 0x88FB, // GL_ONE_MINUS_SRC1_ALPHA
}; // enum class BlendingFactor

enum class BlitFramebufferFilter : GLenum {
    Nearest = 0x2600, // GL_NEAREST
    Linear  = 0x2601, // GL_LINEAR
}; // enum class BlitFramebufferFilter

enum class Boolean : GLboolean {
    False = 0, // GL_FALSE
    True  = 1, // GL_TRUE
}; // enum class Boolean

enum class BufferEnum : GLenum {
    Color   = 0x1800, // GL_COLOR
    Depth   = 0x1801, // GL_DEPTH
    Stencil = 0x1802, // GL_STENCIL
}; // enum class BufferEnum

enum class BufferAccessARB : GLenum {
    ReadOnly  = 0x88B8, // GL_READ_ONLY
    WriteOnly = 0x88B9, // GL_WRITE_ONLY
    ReadWrite = 0x88BA, // GL_READ_WRITE
}; // enum class BufferAccessARB

enum class BufferPNameARB : GLenum {
    BufferImmutableStorage = 0x821F, // GL_BUFFER_IMMUTABLE_STORAGE
    BufferStorageFlags     = 0x8220, // GL_BUFFER_STORAGE_FLAGS
    BufferSize             = 0x8764, // GL_BUFFER_SIZE
    BufferUsage            = 0x8765, // GL_BUFFER_USAGE
    BufferAccess           = 0x88BB, // GL_BUFFER_ACCESS
    BufferMapped           = 0x88BC, // GL_BUFFER_MAPPED
    BufferAccessFlags      = 0x911F, // GL_BUFFER_ACCESS_FLAGS
    BufferMapLength        = 0x9120, // GL_BUFFER_MAP_LENGTH
    BufferMapOffset        = 0x9121, // GL_BUFFER_MAP_OFFSET
}; // enum class BufferPNameARB

enum class BufferPointerNameARB : GLenum {
    BufferMapPointer = 0x88BD, // GL_BUFFER_MAP_POINTER
}; // enum class BufferPointerNameARB

// GL_MAP_{COHERENT,PERSISTENT,READ,WRITE}_{BIT,BIT_EXT} also lie in this namespace
struct BufferStorageMask {
    static constexpr GLbitfield MapRead             = 0x0001; // GL_MAP_READ_BIT
    static constexpr GLbitfield MapWrite            = 0x0002; // GL_MAP_WRITE_BIT
    static constexpr GLbitfield MapPersistent       = 0x0040; // GL_MAP_PERSISTENT_BIT
    static constexpr GLbitfield MapCoherent         = 0x0080; // GL_MAP_COHERENT_BIT
    static constexpr GLbitfield DynamicStorage      = 0x0100; // GL_DYNAMIC_STORAGE_BIT
    static constexpr GLbitfield ClientStorage       = 0x0200; // GL_CLIENT_STORAGE_BIT
    static constexpr GLbitfield SparseStorage       = 0x0400; // GL_SPARSE_STORAGE_BIT_ARB
    static constexpr GLbitfield LGPUSeparateStorage = 0x0800; // GL_LGPU_SEPARATE_STORAGE_BIT_NVX
    static constexpr GLbitfield PerGPUStorage       = 0x0800; // GL_PER_GPU_STORAGE_BIT_NV
}; // struct BufferStorageMask

enum class BufferStorageTarget : GLenum {
    ArrayBuffer             = 0x8892, // GL_ARRAY_BUFFER
    ElementArrayBuffer      = 0x8893, // GL_ELEMENT_ARRAY_BUFFER
    PixelPackBuffer         = 0x88EB, // GL_PIXEL_PACK_BUFFER
    PixelUnpackBuffer       = 0x88EC, // GL_PIXEL_UNPACK_BUFFER
    UniformBuffer           = 0x8A11, // GL_UNIFORM_BUFFER
    TextureBuffer           = 0x8C2A, // GL_TEXTURE_BUFFER
    TransformFeedbackBuffer = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
    CopyReadBuffer          = 0x8F36, // GL_COPY_READ_BUFFER
    CopyWriteBuffer         = 0x8F37, // GL_COPY_WRITE_BUFFER
    DrawIndirectBuffer      = 0x8F3F, // GL_DRAW_INDIRECT_BUFFER
    ShaderStorageBuffer     = 0x90D2, // GL_SHADER_STORAGE_BUFFER
    DispatchIndirectBuffer  = 0x90EE, // GL_DISPATCH_INDIRECT_BUFFER
    QueryBuffer             = 0x9192, // GL_QUERY_BUFFER
    AtomicCounterBuffer     = 0x92C0, // GL_ATOMIC_COUNTER_BUFFER
}; // enum class BufferStorageTarget

enum class BufferTarget : GLenum {
    ParameterBuffer         = 0x80EE, // GL_PARAMETER_BUFFER
    ArrayBuffer             = 0x8892, // GL_ARRAY_BUFFER
    ElementArrayBuffer      = 0x8893, // GL_ELEMENT_ARRAY_BUFFER
    PixelPackBuffer         = 0x88EB, // GL_PIXEL_PACK_BUFFER
    PixelUnpackBuffer       = 0x88EC, // GL_PIXEL_UNPACK_BUFFER
    UniformBuffer           = 0x8A11, // GL_UNIFORM_BUFFER
    TextureBuffer           = 0x8C2A, // GL_TEXTURE_BUFFER
    TransformFeedbackBuffer = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
    CopyReadBuffer          = 0x8F36, // GL_COPY_READ_BUFFER
    CopyWriteBuffer         = 0x8F37, // GL_COPY_WRITE_BUFFER
    DrawIndirectBuffer      = 0x8F3F, // GL_DRAW_INDIRECT_BUFFER
    ShaderStorageBuffer     = 0x90D2, // GL_SHADER_STORAGE_BUFFER
    DispatchIndirectBuffer  = 0x90EE, // GL_DISPATCH_INDIRECT_BUFFER
    QueryBuffer             = 0x9192, // GL_QUERY_BUFFER
    AtomicCounterBuffer     = 0x92C0, // GL_ATOMIC_COUNTER_BUFFER
}; // enum class BufferTarget

enum class BufferUsage : GLenum {
    StreamDraw  = 0x88E0, // GL_STREAM_DRAW
    StreamRead  = 0x88E1, // GL_STREAM_READ
    StreamCopy  = 0x88E2, // GL_STREAM_COPY
    StaticDraw  = 0x88E4, // GL_STATIC_DRAW
    StaticRead  = 0x88E5, // GL_STATIC_READ
    StaticCopy  = 0x88E6, // GL_STATIC_COPY
    DynamicDraw = 0x88E8, // GL_DYNAMIC_DRAW
    DynamicRead = 0x88E9, // GL_DYNAMIC_READ
    DynamicCopy = 0x88EA, // GL_DYNAMIC_COPY
}; // enum class BufferUsage

enum class CheckFramebufferStatusTarget : GLenum {
    ReadFramebuffer = 0x8CA8, // GL_READ_FRAMEBUFFER
    DrawFramebuffer = 0x8CA9, // GL_DRAW_FRAMEBUFFER
    Framebuffer     = 0x8D40, // GL_FRAMEBUFFER
}; // enum class CheckFramebufferStatusTarget

enum class ClampColorModeARB : GLenum {
    False     =      0, // GL_FALSE
    True      =      1, // GL_TRUE
    FixedOnly = 0x891D, // GL_FIXED_ONLY
}; // enum class ClampColorModeARB

enum class ClampColorTargetARB : GLenum {
    ClampVertexColor   = 0x891A, // GL_CLAMP_VERTEX_COLOR_ARB
    ClampFragmentColor = 0x891B, // GL_CLAMP_FRAGMENT_COLOR_ARB
    ClampReadColor     = 0x891C, // GL_CLAMP_READ_COLOR
}; // enum class ClampColorTargetARB

// GL_{DEPTH,ACCUM,STENCIL,COLOR}_BUFFER_BIT also lie in this namespace
struct ClearBufferMask {
    static constexpr GLbitfield DepthBuffer   = 0x00000100; // GL_DEPTH_BUFFER_BIT
    static constexpr GLbitfield StencilBuffer = 0x00000400; // GL_STENCIL_BUFFER_BIT
    static constexpr GLbitfield ColorBuffer   = 0x00004000; // GL_COLOR_BUFFER_BIT
}; // struct ClearBufferMask

enum class ClipControlDepth : GLenum {
    NegativeOneToOne = 0x935E, // GL_NEGATIVE_ONE_TO_ONE
    ZeroToOne        = 0x935F, // GL_ZERO_TO_ONE
}; // enum class ClipControlDepth

enum class ClipControlOrigin : GLenum {
    LowerLeft = 0x8CA1, // GL_LOWER_LEFT
    UpperLeft = 0x8CA2, // GL_UPPER_LEFT
}; // enum class ClipControlOrigin

enum class ClipPlaneName : GLenum {
    ClipDistance0 = 0x3000, // GL_CLIP_DISTANCE0
    ClipDistance1 = 0x3001, // GL_CLIP_DISTANCE1
    ClipDistance2 = 0x3002, // GL_CLIP_DISTANCE2
    ClipDistance3 = 0x3003, // GL_CLIP_DISTANCE3
    ClipDistance4 = 0x3004, // GL_CLIP_DISTANCE4
    ClipDistance5 = 0x3005, // GL_CLIP_DISTANCE5
    ClipDistance6 = 0x3006, // GL_CLIP_DISTANCE6
    ClipDistance7 = 0x3007, // GL_CLIP_DISTANCE7
}; // enum class ClipPlaneName

enum class ColorBuffer : GLenum {
    None              =      0, // GL_NONE
    FrontLeft         = 0x0400, // GL_FRONT_LEFT
    FrontRight        = 0x0401, // GL_FRONT_RIGHT
    BackLeft          = 0x0402, // GL_BACK_LEFT
    BackRight         = 0x0403, // GL_BACK_RIGHT
    Front             = 0x0404, // GL_FRONT
    Back              = 0x0405, // GL_BACK
    Left              = 0x0406, // GL_LEFT
    Right             = 0x0407, // GL_RIGHT
    FrontAndBack      = 0x0408, // GL_FRONT_AND_BACK
    ColorAttachment0  = 0x8CE0, // GL_COLOR_ATTACHMENT0
    ColorAttachment1  = 0x8CE1, // GL_COLOR_ATTACHMENT1
    ColorAttachment2  = 0x8CE2, // GL_COLOR_ATTACHMENT2
    ColorAttachment3  = 0x8CE3, // GL_COLOR_ATTACHMENT3
    ColorAttachment4  = 0x8CE4, // GL_COLOR_ATTACHMENT4
    ColorAttachment5  = 0x8CE5, // GL_COLOR_ATTACHMENT5
    ColorAttachment6  = 0x8CE6, // GL_COLOR_ATTACHMENT6
    ColorAttachment7  = 0x8CE7, // GL_COLOR_ATTACHMENT7
    ColorAttachment8  = 0x8CE8, // GL_COLOR_ATTACHMENT8
    ColorAttachment9  = 0x8CE9, // GL_COLOR_ATTACHMENT9
    ColorAttachment10 = 0x8CEA, // GL_COLOR_ATTACHMENT10
    ColorAttachment11 = 0x8CEB, // GL_COLOR_ATTACHMENT11
    ColorAttachment12 = 0x8CEC, // GL_COLOR_ATTACHMENT12
    ColorAttachment13 = 0x8CED, // GL_COLOR_ATTACHMENT13
    ColorAttachment14 = 0x8CEE, // GL_COLOR_ATTACHMENT14
    ColorAttachment15 = 0x8CEF, // GL_COLOR_ATTACHMENT15
    ColorAttachment16 = 0x8CF0, // GL_COLOR_ATTACHMENT16
    ColorAttachment17 = 0x8CF1, // GL_COLOR_ATTACHMENT17
    ColorAttachment18 = 0x8CF2, // GL_COLOR_ATTACHMENT18
    ColorAttachment19 = 0x8CF3, // GL_COLOR_ATTACHMENT19
    ColorAttachment20 = 0x8CF4, // GL_COLOR_ATTACHMENT20
    ColorAttachment21 = 0x8CF5, // GL_COLOR_ATTACHMENT21
    ColorAttachment22 = 0x8CF6, // GL_COLOR_ATTACHMENT22
    ColorAttachment23 = 0x8CF7, // GL_COLOR_ATTACHMENT23
    ColorAttachment24 = 0x8CF8, // GL_COLOR_ATTACHMENT24
    ColorAttachment25 = 0x8CF9, // GL_COLOR_ATTACHMENT25
    ColorAttachment26 = 0x8CFA, // GL_COLOR_ATTACHMENT26
    ColorAttachment27 = 0x8CFB, // GL_COLOR_ATTACHMENT27
    ColorAttachment28 = 0x8CFC, // GL_COLOR_ATTACHMENT28
    ColorAttachment29 = 0x8CFD, // GL_COLOR_ATTACHMENT29
    ColorAttachment30 = 0x8CFE, // GL_COLOR_ATTACHMENT30
    ColorAttachment31 = 0x8CFF, // GL_COLOR_ATTACHMENT31
}; // enum class ColorBuffer

enum class ColorMaterialFace : GLenum {
    Front        = 0x0404, // GL_FRONT
    Back         = 0x0405, // GL_BACK
    FrontAndBack = 0x0408, // GL_FRONT_AND_BACK
}; // enum class ColorMaterialFace

enum class ColorPointerType : GLenum {
    Byte          = 0x1400, // GL_BYTE
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
}; // enum class ColorPointerType

enum class ColorTableParameterPNameSGI : GLenum {
    ColorTableScale         = 0x80D6, // GL_COLOR_TABLE_SCALE_SGI
    ColorTableBias          = 0x80D7, // GL_COLOR_TABLE_BIAS_SGI
    ColorTableFormat        = 0x80D8, // GL_COLOR_TABLE_FORMAT_SGI
    ColorTableWidth         = 0x80D9, // GL_COLOR_TABLE_WIDTH_SGI
    ColorTableRedSize       = 0x80DA, // GL_COLOR_TABLE_RED_SIZE_SGI
    ColorTableGreenSize     = 0x80DB, // GL_COLOR_TABLE_GREEN_SIZE_SGI
    ColorTableBlueSize      = 0x80DC, // GL_COLOR_TABLE_BLUE_SIZE_SGI
    ColorTableAlphaSize     = 0x80DD, // GL_COLOR_TABLE_ALPHA_SIZE_SGI
    ColorTableLuminanceSize = 0x80DE, // GL_COLOR_TABLE_LUMINANCE_SIZE_SGI
    ColorTableIntensitySize = 0x80DF, // GL_COLOR_TABLE_INTENSITY_SIZE_SGI
}; // enum class ColorTableParameterPNameSGI

enum class ColorTableTargetSGI : GLenum {
    TextureColorTable              = 0x80BC, // GL_TEXTURE_COLOR_TABLE_SGI
    ProxyTextureColorTable         = 0x80BD, // GL_PROXY_TEXTURE_COLOR_TABLE_SGI
    ColorTable                     = 0x80D0, // GL_COLOR_TABLE_SGI
    PostConvolutionColorTable      = 0x80D1, // GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    PostColorMatrixColorTable      = 0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    ProxyColorTable                = 0x80D3, // GL_PROXY_COLOR_TABLE_SGI
    ProxyPostConvolutionColorTable = 0x80D4, // GL_PROXY_POST_CONVOLUTION_COLOR_TABLE_SGI
    ProxyPostColorMatrixColorTable = 0x80D5, // GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE_SGI
}; // enum class ColorTableTargetSGI

enum class CombinerBiasNV : GLenum {
    None                  =      0, // GL_NONE
    BiasByNegativeOneHalf = 0x8541, // GL_BIAS_BY_NEGATIVE_ONE_HALF_NV
}; // enum class CombinerBiasNV

enum class CombinerComponentUsageNV : GLenum {
    Blue  = 0x1905, // GL_BLUE
    Alpha = 0x1906, // GL_ALPHA
    RGB   = 0x1907, // GL_RGB
}; // enum class CombinerComponentUsageNV

enum class CombinerMappingNV : GLenum {
    UnsignedIdentity = 0x8536, // GL_UNSIGNED_IDENTITY_NV
    UnsignedInvert   = 0x8537, // GL_UNSIGNED_INVERT_NV
    ExpandNormal     = 0x8538, // GL_EXPAND_NORMAL_NV
    ExpandNegate     = 0x8539, // GL_EXPAND_NEGATE_NV
    HalfBiasNormal   = 0x853A, // GL_HALF_BIAS_NORMAL_NV
    HalfBiasNegate   = 0x853B, // GL_HALF_BIAS_NEGATE_NV
    SignedIdentity   = 0x853C, // GL_SIGNED_IDENTITY_NV
    SignedNegate     = 0x853D, // GL_SIGNED_NEGATE_NV
}; // enum class CombinerMappingNV

enum class CombinerParameterNV : GLenum {
    CombinerInput          = 0x8542, // GL_COMBINER_INPUT_NV
    CombinerMapping        = 0x8543, // GL_COMBINER_MAPPING_NV
    CombinerComponentUsage = 0x8544, // GL_COMBINER_COMPONENT_USAGE_NV
}; // enum class CombinerParameterNV

enum class CombinerPortionNV : GLenum {
    Alpha = 0x1906, // GL_ALPHA
    RGB   = 0x1907, // GL_RGB
}; // enum class CombinerPortionNV

enum class CombinerRegisterNV : GLenum {
    Texture0       = 0x84C0, // GL_TEXTURE0_ARB
    Texture1       = 0x84C1, // GL_TEXTURE1_ARB
    PrimaryColor   = 0x852C, // GL_PRIMARY_COLOR_NV
    SecondaryColor = 0x852D, // GL_SECONDARY_COLOR_NV
    Spare0         = 0x852E, // GL_SPARE0_NV
    Spare1         = 0x852F, // GL_SPARE1_NV
    Discard        = 0x8530, // GL_DISCARD_NV
}; // enum class CombinerRegisterNV

enum class CombinerScaleNV : GLenum {
    None           =      0, // GL_NONE
    ScaleByTwo     = 0x853E, // GL_SCALE_BY_TWO_NV
    ScaleByFour    = 0x853F, // GL_SCALE_BY_FOUR_NV
    ScaleByOneHalf = 0x8540, // GL_SCALE_BY_ONE_HALF_NV
}; // enum class CombinerScaleNV

enum class CombinerStageNV : GLenum {
    Combiner0 = 0x8550, // GL_COMBINER0_NV
    Combiner1 = 0x8551, // GL_COMBINER1_NV
    Combiner2 = 0x8552, // GL_COMBINER2_NV
    Combiner3 = 0x8553, // GL_COMBINER3_NV
    Combiner4 = 0x8554, // GL_COMBINER4_NV
    Combiner5 = 0x8555, // GL_COMBINER5_NV
    Combiner6 = 0x8556, // GL_COMBINER6_NV
    Combiner7 = 0x8557, // GL_COMBINER7_NV
}; // enum class CombinerStageNV

enum class CombinerVariableNV : GLenum {
    VariableA = 0x8523, // GL_VARIABLE_A_NV
    VariableB = 0x8524, // GL_VARIABLE_B_NV
    VariableC = 0x8525, // GL_VARIABLE_C_NV
    VariableD = 0x8526, // GL_VARIABLE_D_NV
    VariableE = 0x8527, // GL_VARIABLE_E_NV
    VariableF = 0x8528, // GL_VARIABLE_F_NV
    VariableG = 0x8529, // GL_VARIABLE_G_NV
}; // enum class CombinerVariableNV

// For NV_command_list.
enum class CommandOpcodesNV : GLenum {
    TerminateSequenceCommand     = 0x0000, // GL_TERMINATE_SEQUENCE_COMMAND_NV
    NopCommand                   = 0x0001, // GL_NOP_COMMAND_NV
    DrawElementsCommand          = 0x0002, // GL_DRAW_ELEMENTS_COMMAND_NV
    DrawArraysCommand            = 0x0003, // GL_DRAW_ARRAYS_COMMAND_NV
    DrawElementsStripCommand     = 0x0004, // GL_DRAW_ELEMENTS_STRIP_COMMAND_NV
    DrawArraysStripCommand       = 0x0005, // GL_DRAW_ARRAYS_STRIP_COMMAND_NV
    DrawElementsInstancedCommand = 0x0006, // GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV
    DrawArraysInstancedCommand   = 0x0007, // GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV
    ElementAddressCommand        = 0x0008, // GL_ELEMENT_ADDRESS_COMMAND_NV
    AttributeAddressCommand      = 0x0009, // GL_ATTRIBUTE_ADDRESS_COMMAND_NV
    UniformAddressCommand        = 0x000A, // GL_UNIFORM_ADDRESS_COMMAND_NV
    BlendColorCommand            = 0x000B, // GL_BLEND_COLOR_COMMAND_NV
    StencilRefCommand            = 0x000C, // GL_STENCIL_REF_COMMAND_NV
    LineWidthCommand             = 0x000D, // GL_LINE_WIDTH_COMMAND_NV
    PolygonOffsetCommand         = 0x000E, // GL_POLYGON_OFFSET_COMMAND_NV
    AlphaRefCommand              = 0x000F, // GL_ALPHA_REF_COMMAND_NV
    ViewportCommand              = 0x0010, // GL_VIEWPORT_COMMAND_NV
    ScissorCommand               = 0x0011, // GL_SCISSOR_COMMAND_NV
    FrontFaceCommand             = 0x0012, // GL_FRONT_FACE_COMMAND_NV
}; // enum class CommandOpcodesNV

enum class ConditionalRenderMode : GLenum {
    QueryWait                   = 0x8E13, // GL_QUERY_WAIT
    QueryNoWait                 = 0x8E14, // GL_QUERY_NO_WAIT
    QueryByRegionWait           = 0x8E15, // GL_QUERY_BY_REGION_WAIT
    QueryByRegionNoWait         = 0x8E16, // GL_QUERY_BY_REGION_NO_WAIT
    QueryWaitInverted           = 0x8E17, // GL_QUERY_WAIT_INVERTED
    QueryNoWaitInverted         = 0x8E18, // GL_QUERY_NO_WAIT_INVERTED
    QueryByRegionWaitInverted   = 0x8E19, // GL_QUERY_BY_REGION_WAIT_INVERTED
    QueryByRegionNoWaitInverted = 0x8E1A, // GL_QUERY_BY_REGION_NO_WAIT_INVERTED
}; // enum class ConditionalRenderMode

enum class ContainerType : GLenum {
    ProgramObject = 0x8B40, // GL_PROGRAM_OBJECT_ARB
}; // enum class ContainerType

// Should be shared with WGL/GLX, but aren't since the FORWARD_COMPATIBLE and DEBUG values are swapped vs. WGL/GLX.
struct ContextFlagMask {
    static constexpr GLenum ContextFlagForwardCompatible = 0x00000001; // GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT
    static constexpr GLenum ContextFlagDebug             = 0x00000002; // GL_CONTEXT_FLAG_DEBUG_BIT
    static constexpr GLenum ContextFlagRobustAccess      = 0x00000004; // GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT
    static constexpr GLenum ContextFlagNoError           = 0x00000008; // GL_CONTEXT_FLAG_NO_ERROR_BIT
}; // struct ContextFlagMask

struct ContextProfileMask {
    static constexpr GLenum ContextCoreProfile          = 0x00000001; // GL_CONTEXT_CORE_PROFILE_BIT
    static constexpr GLenum ContextCompatibilityProfile = 0x00000002; // GL_CONTEXT_COMPATIBILITY_PROFILE_BIT
}; // struct ContextProfileMask

enum class ConvolutionBorderModeEXT : GLenum {
    Reduce = 0x8016, // GL_REDUCE_EXT
}; // enum class ConvolutionBorderModeEXT

enum class ConvolutionParameterEXT : GLenum {
    ConvolutionBorderMode  = 0x8013, // GL_CONVOLUTION_BORDER_MODE_EXT
    ConvolutionFilterScale = 0x8014, // GL_CONVOLUTION_FILTER_SCALE_EXT
    ConvolutionFilterBias  = 0x8015, // GL_CONVOLUTION_FILTER_BIAS_EXT
}; // enum class ConvolutionParameterEXT

enum class ConvolutionTargetEXT : GLenum {
    Convolution1D = 0x8010, // GL_CONVOLUTION_1D_EXT
    Convolution2D = 0x8011, // GL_CONVOLUTION_2D_EXT
}; // enum class ConvolutionTargetEXT

enum class CopyBufferSubDataTarget : GLenum {
    ArrayBuffer             = 0x8892, // GL_ARRAY_BUFFER
    ElementArrayBuffer      = 0x8893, // GL_ELEMENT_ARRAY_BUFFER
    PixelPackBuffer         = 0x88EB, // GL_PIXEL_PACK_BUFFER
    PixelUnpackBuffer       = 0x88EC, // GL_PIXEL_UNPACK_BUFFER
    UniformBuffer           = 0x8A11, // GL_UNIFORM_BUFFER
    TextureBuffer           = 0x8C2A, // GL_TEXTURE_BUFFER
    TransformFeedbackBuffer = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
    CopyReadBuffer          = 0x8F36, // GL_COPY_READ_BUFFER
    CopyWriteBuffer         = 0x8F37, // GL_COPY_WRITE_BUFFER
    DrawIndirectBuffer      = 0x8F3F, // GL_DRAW_INDIRECT_BUFFER
    ShaderStorageBuffer     = 0x90D2, // GL_SHADER_STORAGE_BUFFER
    DispatchIndirectBuffer  = 0x90EE, // GL_DISPATCH_INDIRECT_BUFFER
    QueryBuffer             = 0x9192, // GL_QUERY_BUFFER
    AtomicCounterBuffer     = 0x92C0, // GL_ATOMIC_COUNTER_BUFFER
}; // enum class CopyBufferSubDataTarget

enum class CopyImageSubDataTarget : GLenum {
    Texture1D                 = 0x0DE0, // GL_TEXTURE_1D
    Texture2D                 = 0x0DE1, // GL_TEXTURE_2D
    Texture3D                 = 0x806F, // GL_TEXTURE_3D
    TextureRectangle          = 0x84F5, // GL_TEXTURE_RECTANGLE
    TextureCubeMap            = 0x8513, // GL_TEXTURE_CUBE_MAP
    Texture1DArray            = 0x8C18, // GL_TEXTURE_1D_ARRAY
    Texture2DArray            = 0x8C1A, // GL_TEXTURE_2D_ARRAY
    Renderbuffer              = 0x8D41, // GL_RENDERBUFFER
    TextureCubeMapArray       = 0x9009, // GL_TEXTURE_CUBE_MAP_ARRAY
    Texture2DMultisample      = 0x9100, // GL_TEXTURE_2D_MULTISAMPLE
    Texture2DMultisampleArray = 0x9102, // GL_TEXTURE_2D_MULTISAMPLE_ARRAY
}; // enum class CopyImageSubDataTarget

enum class CullFaceMode : GLenum {
    Front        = 0x0404, // GL_FRONT
    Back         = 0x0405, // GL_BACK
    FrontAndBack = 0x0408, // GL_FRONT_AND_BACK
}; // enum class CullFaceMode

enum class CullParameterEXT : GLenum {
    CullVertexEyePosition    = 0x81AB, // GL_CULL_VERTEX_EYE_POSITION_EXT
    CullVertexObjectPosition = 0x81AC, // GL_CULL_VERTEX_OBJECT_POSITION_EXT
}; // enum class CullParameterEXT

enum class DataTypeEXT : GLenum {
    Scalar = 0x87BE, // GL_SCALAR_EXT
    Vector = 0x87BF, // GL_VECTOR_EXT
    Matrix = 0x87C0, // GL_MATRIX_EXT
}; // enum class DataTypeEXT

enum class DebugSeverity : GLenum {
    DontCare                  = 0x1100, // GL_DONT_CARE
    DebugSeverityNotification = 0x826B, // GL_DEBUG_SEVERITY_NOTIFICATION
    DebugSeverityHigh         = 0x9146, // GL_DEBUG_SEVERITY_HIGH
    DebugSeverityMedium       = 0x9147, // GL_DEBUG_SEVERITY_MEDIUM
    DebugSeverityLow          = 0x9148, // GL_DEBUG_SEVERITY_LOW
}; // enum class DebugSeverity

enum class DebugSource : GLenum {
    DontCare                  = 0x1100, // GL_DONT_CARE
    DebugSourceAPI            = 0x8246, // GL_DEBUG_SOURCE_API
    DebugSourceWindowSystem   = 0x8247, // GL_DEBUG_SOURCE_WINDOW_SYSTEM
    DebugSourceShaderCompiler = 0x8248, // GL_DEBUG_SOURCE_SHADER_COMPILER
    DebugSourceThirdParty     = 0x8249, // GL_DEBUG_SOURCE_THIRD_PARTY
    DebugSourceApplication    = 0x824A, // GL_DEBUG_SOURCE_APPLICATION
    DebugSourceOther          = 0x824B, // GL_DEBUG_SOURCE_OTHER
}; // enum class DebugSource

enum class DebugType : GLenum {
    DontCare                    = 0x1100, // GL_DONT_CARE
    DebugTypeError              = 0x824C, // GL_DEBUG_TYPE_ERROR
    DebugTypeDeprecatedBehavior = 0x824D, // GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
    DebugTypeUndefinedBehavior  = 0x824E, // GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
    DebugTypePortability        = 0x824F, // GL_DEBUG_TYPE_PORTABILITY
    DebugTypePerformance        = 0x8250, // GL_DEBUG_TYPE_PERFORMANCE
    DebugTypeOther              = 0x8251, // GL_DEBUG_TYPE_OTHER
    DebugTypeMarker             = 0x8268, // GL_DEBUG_TYPE_MARKER
    DebugTypePushGroup          = 0x8269, // GL_DEBUG_TYPE_PUSH_GROUP
    DebugTypePopGroup           = 0x826A, // GL_DEBUG_TYPE_POP_GROUP
}; // enum class DebugType

enum class DepthFunction : GLenum {
    Never    = 0x0200, // GL_NEVER
    Less     = 0x0201, // GL_LESS
    Equal    = 0x0202, // GL_EQUAL
    Lequal   = 0x0203, // GL_LEQUAL
    Greater  = 0x0204, // GL_GREATER
    Notequal = 0x0205, // GL_NOTEQUAL
    Gequal   = 0x0206, // GL_GEQUAL
    Always   = 0x0207, // GL_ALWAYS
}; // enum class DepthFunction

enum class DepthStencilTextureMode : GLenum {
    StencilIndex   = 0x1901, // GL_STENCIL_INDEX
    DepthComponent = 0x1902, // GL_DEPTH_COMPONENT
}; // enum class DepthStencilTextureMode

enum class DrawBufferMode : GLenum {
    None              =      0, // GL_NONE
    FrontLeft         = 0x0400, // GL_FRONT_LEFT
    FrontRight        = 0x0401, // GL_FRONT_RIGHT
    BackLeft          = 0x0402, // GL_BACK_LEFT
    BackRight         = 0x0403, // GL_BACK_RIGHT
    Front             = 0x0404, // GL_FRONT
    Back              = 0x0405, // GL_BACK
    Left              = 0x0406, // GL_LEFT
    Right             = 0x0407, // GL_RIGHT
    FrontAndBack      = 0x0408, // GL_FRONT_AND_BACK
    ColorAttachment0  = 0x8CE0, // GL_COLOR_ATTACHMENT0
    ColorAttachment1  = 0x8CE1, // GL_COLOR_ATTACHMENT1
    ColorAttachment2  = 0x8CE2, // GL_COLOR_ATTACHMENT2
    ColorAttachment3  = 0x8CE3, // GL_COLOR_ATTACHMENT3
    ColorAttachment4  = 0x8CE4, // GL_COLOR_ATTACHMENT4
    ColorAttachment5  = 0x8CE5, // GL_COLOR_ATTACHMENT5
    ColorAttachment6  = 0x8CE6, // GL_COLOR_ATTACHMENT6
    ColorAttachment7  = 0x8CE7, // GL_COLOR_ATTACHMENT7
    ColorAttachment8  = 0x8CE8, // GL_COLOR_ATTACHMENT8
    ColorAttachment9  = 0x8CE9, // GL_COLOR_ATTACHMENT9
    ColorAttachment10 = 0x8CEA, // GL_COLOR_ATTACHMENT10
    ColorAttachment11 = 0x8CEB, // GL_COLOR_ATTACHMENT11
    ColorAttachment12 = 0x8CEC, // GL_COLOR_ATTACHMENT12
    ColorAttachment13 = 0x8CED, // GL_COLOR_ATTACHMENT13
    ColorAttachment14 = 0x8CEE, // GL_COLOR_ATTACHMENT14
    ColorAttachment15 = 0x8CEF, // GL_COLOR_ATTACHMENT15
    ColorAttachment16 = 0x8CF0, // GL_COLOR_ATTACHMENT16
    ColorAttachment17 = 0x8CF1, // GL_COLOR_ATTACHMENT17
    ColorAttachment18 = 0x8CF2, // GL_COLOR_ATTACHMENT18
    ColorAttachment19 = 0x8CF3, // GL_COLOR_ATTACHMENT19
    ColorAttachment20 = 0x8CF4, // GL_COLOR_ATTACHMENT20
    ColorAttachment21 = 0x8CF5, // GL_COLOR_ATTACHMENT21
    ColorAttachment22 = 0x8CF6, // GL_COLOR_ATTACHMENT22
    ColorAttachment23 = 0x8CF7, // GL_COLOR_ATTACHMENT23
    ColorAttachment24 = 0x8CF8, // GL_COLOR_ATTACHMENT24
    ColorAttachment25 = 0x8CF9, // GL_COLOR_ATTACHMENT25
    ColorAttachment26 = 0x8CFA, // GL_COLOR_ATTACHMENT26
    ColorAttachment27 = 0x8CFB, // GL_COLOR_ATTACHMENT27
    ColorAttachment28 = 0x8CFC, // GL_COLOR_ATTACHMENT28
    ColorAttachment29 = 0x8CFD, // GL_COLOR_ATTACHMENT29
    ColorAttachment30 = 0x8CFE, // GL_COLOR_ATTACHMENT30
    ColorAttachment31 = 0x8CFF, // GL_COLOR_ATTACHMENT31
}; // enum class DrawBufferMode

enum class DrawElementsType : GLenum {
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
}; // enum class DrawElementsType

enum class ElementPointerTypeATI : GLenum {
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
}; // enum class ElementPointerTypeATI

enum class EnableCap : GLenum {
    LineSmooth                   = 0x0B20, // GL_LINE_SMOOTH
    PolygonSmooth                = 0x0B41, // GL_POLYGON_SMOOTH
    CullFace                     = 0x0B44, // GL_CULL_FACE
    Fog                          = 0x0B60, // GL_FOG
    DepthTest                    = 0x0B71, // GL_DEPTH_TEST
    StencilTest                  = 0x0B90, // GL_STENCIL_TEST
    Dither                       = 0x0BD0, // GL_DITHER
    Blend                        = 0x0BE2, // GL_BLEND
    ColorLogicOp                 = 0x0BF2, // GL_COLOR_LOGIC_OP
    ScissorTest                  = 0x0C11, // GL_SCISSOR_TEST
    Texture1D                    = 0x0DE0, // GL_TEXTURE_1D
    Texture2D                    = 0x0DE1, // GL_TEXTURE_2D
    PolygonOffsetPoint           = 0x2A01, // GL_POLYGON_OFFSET_POINT
    PolygonOffsetLine            = 0x2A02, // GL_POLYGON_OFFSET_LINE
    ClipDistance0                = 0x3000, // GL_CLIP_DISTANCE0
    ClipDistance1                = 0x3001, // GL_CLIP_DISTANCE1
    ClipDistance2                = 0x3002, // GL_CLIP_DISTANCE2
    ClipDistance3                = 0x3003, // GL_CLIP_DISTANCE3
    ClipDistance4                = 0x3004, // GL_CLIP_DISTANCE4
    ClipDistance5                = 0x3005, // GL_CLIP_DISTANCE5
    ClipDistance6                = 0x3006, // GL_CLIP_DISTANCE6
    ClipDistance7                = 0x3007, // GL_CLIP_DISTANCE7
    Convolution1D                = 0x8010, // GL_CONVOLUTION_1D_EXT
    Convolution2D                = 0x8011, // GL_CONVOLUTION_2D_EXT
    Separable2D                  = 0x8012, // GL_SEPARABLE_2D_EXT
    Histogram                    = 0x8024, // GL_HISTOGRAM_EXT
    Minmax                       = 0x802E, // GL_MINMAX_EXT
    PolygonOffsetFill            = 0x8037, // GL_POLYGON_OFFSET_FILL
    RescaleNormal                = 0x803A, // GL_RESCALE_NORMAL_EXT
    Texture3D                    = 0x806F, // GL_TEXTURE_3D_EXT
    VertexArray                  = 0x8074, // GL_VERTEX_ARRAY
    Interlace                    = 0x8094, // GL_INTERLACE_SGIX
    Multisample                  = 0x809D, // GL_MULTISAMPLE
    SampleAlphaToCoverage        = 0x809E, // GL_SAMPLE_ALPHA_TO_COVERAGE
    SampleAlphaToOne             = 0x809F, // GL_SAMPLE_ALPHA_TO_ONE
    SampleCoverage               = 0x80A0, // GL_SAMPLE_COVERAGE
    TextureColorTable            = 0x80BC, // GL_TEXTURE_COLOR_TABLE_SGI
    ColorTable                   = 0x80D0, // GL_COLOR_TABLE_SGI
    PostConvolutionColorTable    = 0x80D1, // GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    PostColorMatrixColorTable    = 0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    Texture4D                    = 0x8134, // GL_TEXTURE_4D_SGIS
    PixelTexGen                  = 0x8139, // GL_PIXEL_TEX_GEN_SGIX
    Sprite                       = 0x8148, // GL_SPRITE_SGIX
    ReferencePlane               = 0x817D, // GL_REFERENCE_PLANE_SGIX
    IrInstrument1                = 0x817F, // GL_IR_INSTRUMENT1_SGIX
    CalligraphicFragment         = 0x8183, // GL_CALLIGRAPHIC_FRAGMENT_SGIX
    Framezoom                    = 0x818B, // GL_FRAMEZOOM_SGIX
    FogOffset                    = 0x8198, // GL_FOG_OFFSET_SGIX
    SharedTexturePalette         = 0x81FB, // GL_SHARED_TEXTURE_PALETTE_EXT
    DebugOutputSynchronous       = 0x8242, // GL_DEBUG_OUTPUT_SYNCHRONOUS
    AsyncHistogram               = 0x832C, // GL_ASYNC_HISTOGRAM_SGIX
    PixelTexture                 = 0x8353, // GL_PIXEL_TEXTURE_SGIS
    AsyncTexImage                = 0x835C, // GL_ASYNC_TEX_IMAGE_SGIX
    AsyncDrawPixels              = 0x835D, // GL_ASYNC_DRAW_PIXELS_SGIX
    AsyncReadPixels              = 0x835E, // GL_ASYNC_READ_PIXELS_SGIX
    FragmentLighting             = 0x8400, // GL_FRAGMENT_LIGHTING_SGIX
    FragmentColorMaterial        = 0x8401, // GL_FRAGMENT_COLOR_MATERIAL_SGIX
    FragmentLight0               = 0x840C, // GL_FRAGMENT_LIGHT0_SGIX
    FragmentLight1               = 0x840D, // GL_FRAGMENT_LIGHT1_SGIX
    FragmentLight2               = 0x840E, // GL_FRAGMENT_LIGHT2_SGIX
    FragmentLight3               = 0x840F, // GL_FRAGMENT_LIGHT3_SGIX
    FragmentLight4               = 0x8410, // GL_FRAGMENT_LIGHT4_SGIX
    FragmentLight5               = 0x8411, // GL_FRAGMENT_LIGHT5_SGIX
    FragmentLight6               = 0x8412, // GL_FRAGMENT_LIGHT6_SGIX
    FragmentLight7               = 0x8413, // GL_FRAGMENT_LIGHT7_SGIX
    TextureRectangle             = 0x84F5, // GL_TEXTURE_RECTANGLE
    TextureCubeMap               = 0x8513, // GL_TEXTURE_CUBE_MAP
    ProgramPointSize             = 0x8642, // GL_PROGRAM_POINT_SIZE
    DepthClamp                   = 0x864F, // GL_DEPTH_CLAMP
    TextureCubeMapSeamless       = 0x884F, // GL_TEXTURE_CUBE_MAP_SEAMLESS
    SampleShading                = 0x8C36, // GL_SAMPLE_SHADING
    RasterizerDiscard            = 0x8C89, // GL_RASTERIZER_DISCARD
    PrimitiveRestartFixedIndex   = 0x8D69, // GL_PRIMITIVE_RESTART_FIXED_INDEX
    FramebufferSRGB              = 0x8DB9, // GL_FRAMEBUFFER_SRGB
    SampleMask                   = 0x8E51, // GL_SAMPLE_MASK
    PrimitiveRestart             = 0x8F9D, // GL_PRIMITIVE_RESTART
    DebugOutput                  = 0x92E0, // GL_DEBUG_OUTPUT
    ShadingRateImagePerPrimitive = 0x95B1, // GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV
}; // enum class EnableCap

enum class ErrorCode : GLenum {
    NoError                     =      0, // GL_NO_ERROR
    InvalidEnum                 = 0x0500, // GL_INVALID_ENUM
    InvalidValue                = 0x0501, // GL_INVALID_VALUE
    InvalidOperation            = 0x0502, // GL_INVALID_OPERATION
    StackOverflow               = 0x0503, // GL_STACK_OVERFLOW
    StackUnderflow              = 0x0504, // GL_STACK_UNDERFLOW
    OutOfMemory                 = 0x0505, // GL_OUT_OF_MEMORY
    InvalidFramebufferOperation = 0x0506, // GL_INVALID_FRAMEBUFFER_OPERATION
    TableTooLarge               = 0x8031, // GL_TABLE_TOO_LARGE_EXT
    TextureTooLarge             = 0x8065, // GL_TEXTURE_TOO_LARGE_EXT
}; // enum class ErrorCode

enum class EvalTargetNV : GLenum {
    Eval2D           = 0x86C0, // GL_EVAL_2D_NV
    EvalTriangular2D = 0x86C1, // GL_EVAL_TRIANGULAR_2D_NV
}; // enum class EvalTargetNV

enum class ExternalHandleType : GLenum {
    HandleTypeOpaqueFd       = 0x9586, // GL_HANDLE_TYPE_OPAQUE_FD_EXT
    HandleTypeOpaqueWIN32    = 0x9587, // GL_HANDLE_TYPE_OPAQUE_WIN32_EXT
    HandleTypeOpaqueWIN32Kmt = 0x9588, // GL_HANDLE_TYPE_OPAQUE_WIN32_KMT_EXT
    HandleTypeD3D12Tilepool  = 0x9589, // GL_HANDLE_TYPE_D3D12_TILEPOOL_EXT
    HandleTypeD3D12Resource  = 0x958A, // GL_HANDLE_TYPE_D3D12_RESOURCE_EXT
    HandleTypeD3D11Image     = 0x958B, // GL_HANDLE_TYPE_D3D11_IMAGE_EXT
    HandleTypeD3D11ImageKmt  = 0x958C, // GL_HANDLE_TYPE_D3D11_IMAGE_KMT_EXT
    HandleTypeD3D12Fence     = 0x9594, // GL_HANDLE_TYPE_D3D12_FENCE_EXT
}; // enum class ExternalHandleType

enum class FenceConditionNV : GLenum {
    AllCompleted = 0x84F2, // GL_ALL_COMPLETED_NV
}; // enum class FenceConditionNV

enum class FenceParameterNameNV : GLenum {
    FenceStatus    = 0x84F3, // GL_FENCE_STATUS_NV
    FenceCondition = 0x84F4, // GL_FENCE_CONDITION_NV
}; // enum class FenceParameterNameNV

struct FfdMaskSGIX {
    static constexpr GLbitfield TextureDeformation  = 0x00000001; // GL_TEXTURE_DEFORMATION_BIT_SGIX
    static constexpr GLbitfield GeometryDeformation = 0x00000002; // GL_GEOMETRY_DEFORMATION_BIT_SGIX
}; // struct FfdMaskSGIX

enum class FfdTargetSGIX : GLenum {
    GeometryDeformation = 0x8194, // GL_GEOMETRY_DEFORMATION_SGIX
    TextureDeformation  = 0x8195, // GL_TEXTURE_DEFORMATION_SGIX
}; // enum class FfdTargetSGIX

enum class FogCoordinatePointerType : GLenum {
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class FogCoordinatePointerType

enum class FogMode : GLenum {
    Linear  = 0x2601, // GL_LINEAR
    FogFunc = 0x812A, // GL_FOG_FUNC_SGIS
}; // enum class FogMode

enum class FogParameter : GLenum {
    FogOffsetValue = 0x8199, // GL_FOG_OFFSET_VALUE_SGIX
}; // enum class FogParameter

enum class FogPointerTypeEXT : GLenum {
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class FogPointerTypeEXT

enum class FogPointerTypeIBM : GLenum {
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class FogPointerTypeIBM

enum class FragmentLightModelParameterSGIX : GLenum {
    FragmentLightModelLocalViewer         = 0x8408, // GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX
    FragmentLightModelTwoSide             = 0x8409, // GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX
    FragmentLightModelAmbient             = 0x840A, // GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX
    FragmentLightModelNormalInterpolation = 0x840B, // GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX
}; // enum class FragmentLightModelParameterSGIX

enum class FragmentLightNameSGIX : GLenum {
    FragmentLight0 = 0x840C, // GL_FRAGMENT_LIGHT0_SGIX
    FragmentLight1 = 0x840D, // GL_FRAGMENT_LIGHT1_SGIX
    FragmentLight2 = 0x840E, // GL_FRAGMENT_LIGHT2_SGIX
    FragmentLight3 = 0x840F, // GL_FRAGMENT_LIGHT3_SGIX
    FragmentLight4 = 0x8410, // GL_FRAGMENT_LIGHT4_SGIX
    FragmentLight5 = 0x8411, // GL_FRAGMENT_LIGHT5_SGIX
    FragmentLight6 = 0x8412, // GL_FRAGMENT_LIGHT6_SGIX
    FragmentLight7 = 0x8413, // GL_FRAGMENT_LIGHT7_SGIX
}; // enum class FragmentLightNameSGIX

enum class FragmentOpATI : GLenum {
    Mov     = 0x8961, // GL_MOV_ATI
    Add     = 0x8963, // GL_ADD_ATI
    Mul     = 0x8964, // GL_MUL_ATI
    Sub     = 0x8965, // GL_SUB_ATI
    Dot3    = 0x8966, // GL_DOT3_ATI
    Dot4    = 0x8967, // GL_DOT4_ATI
    Mad     = 0x8968, // GL_MAD_ATI
    Lerp    = 0x8969, // GL_LERP_ATI
    Cnd     = 0x896A, // GL_CND_ATI
    Cnd0    = 0x896B, // GL_CND0_ATI
    Dot2Add = 0x896C, // GL_DOT2_ADD_ATI
}; // enum class FragmentOpATI

struct FragmentShaderColorModMaskATI {
    static constexpr GLenum Comp   = 0x00000002; // GL_COMP_BIT_ATI
    static constexpr GLenum Negate = 0x00000004; // GL_NEGATE_BIT_ATI
    static constexpr GLenum Bias   = 0x00000008; // GL_BIAS_BIT_ATI
}; // struct FragmentShaderColorModMaskATI

struct FragmentShaderDestMaskATI {
    static constexpr GLenum Red   = 0x00000001; // GL_RED_BIT_ATI
    static constexpr GLenum Green = 0x00000002; // GL_GREEN_BIT_ATI
    static constexpr GLenum Blue  = 0x00000004; // GL_BLUE_BIT_ATI
}; // struct FragmentShaderDestMaskATI

struct FragmentShaderDestModMaskATI {
    static constexpr GLenum _2X      = 0x00000001; // GL_2X_BIT_ATI
    static constexpr GLenum _4X      = 0x00000002; // GL_4X_BIT_ATI
    static constexpr GLenum _8X      = 0x00000004; // GL_8X_BIT_ATI
    static constexpr GLenum Half     = 0x00000008; // GL_HALF_BIT_ATI
    static constexpr GLenum Quarter  = 0x00000010; // GL_QUARTER_BIT_ATI
    static constexpr GLenum Eighth   = 0x00000020; // GL_EIGHTH_BIT_ATI
    static constexpr GLenum Saturate = 0x00000040; // GL_SATURATE_BIT_ATI
}; // struct FragmentShaderDestModMaskATI

enum class FramebufferAttachment : GLenum {
    DepthStencilAttachment = 0x821A, // GL_DEPTH_STENCIL_ATTACHMENT
    ColorAttachment0       = 0x8CE0, // GL_COLOR_ATTACHMENT0
    ColorAttachment1       = 0x8CE1, // GL_COLOR_ATTACHMENT1
    ColorAttachment2       = 0x8CE2, // GL_COLOR_ATTACHMENT2
    ColorAttachment3       = 0x8CE3, // GL_COLOR_ATTACHMENT3
    ColorAttachment4       = 0x8CE4, // GL_COLOR_ATTACHMENT4
    ColorAttachment5       = 0x8CE5, // GL_COLOR_ATTACHMENT5
    ColorAttachment6       = 0x8CE6, // GL_COLOR_ATTACHMENT6
    ColorAttachment7       = 0x8CE7, // GL_COLOR_ATTACHMENT7
    ColorAttachment8       = 0x8CE8, // GL_COLOR_ATTACHMENT8
    ColorAttachment9       = 0x8CE9, // GL_COLOR_ATTACHMENT9
    ColorAttachment10      = 0x8CEA, // GL_COLOR_ATTACHMENT10
    ColorAttachment11      = 0x8CEB, // GL_COLOR_ATTACHMENT11
    ColorAttachment12      = 0x8CEC, // GL_COLOR_ATTACHMENT12
    ColorAttachment13      = 0x8CED, // GL_COLOR_ATTACHMENT13
    ColorAttachment14      = 0x8CEE, // GL_COLOR_ATTACHMENT14
    ColorAttachment15      = 0x8CEF, // GL_COLOR_ATTACHMENT15
    ColorAttachment16      = 0x8CF0, // GL_COLOR_ATTACHMENT16
    ColorAttachment17      = 0x8CF1, // GL_COLOR_ATTACHMENT17
    ColorAttachment18      = 0x8CF2, // GL_COLOR_ATTACHMENT18
    ColorAttachment19      = 0x8CF3, // GL_COLOR_ATTACHMENT19
    ColorAttachment20      = 0x8CF4, // GL_COLOR_ATTACHMENT20
    ColorAttachment21      = 0x8CF5, // GL_COLOR_ATTACHMENT21
    ColorAttachment22      = 0x8CF6, // GL_COLOR_ATTACHMENT22
    ColorAttachment23      = 0x8CF7, // GL_COLOR_ATTACHMENT23
    ColorAttachment24      = 0x8CF8, // GL_COLOR_ATTACHMENT24
    ColorAttachment25      = 0x8CF9, // GL_COLOR_ATTACHMENT25
    ColorAttachment26      = 0x8CFA, // GL_COLOR_ATTACHMENT26
    ColorAttachment27      = 0x8CFB, // GL_COLOR_ATTACHMENT27
    ColorAttachment28      = 0x8CFC, // GL_COLOR_ATTACHMENT28
    ColorAttachment29      = 0x8CFD, // GL_COLOR_ATTACHMENT29
    ColorAttachment30      = 0x8CFE, // GL_COLOR_ATTACHMENT30
    ColorAttachment31      = 0x8CFF, // GL_COLOR_ATTACHMENT31
    DepthAttachment        = 0x8D00, // GL_DEPTH_ATTACHMENT
    StencilAttachment      = 0x8D20, // GL_STENCIL_ATTACHMENT
}; // enum class FramebufferAttachment

enum class FramebufferAttachmentParameterName : GLenum {
    FramebufferAttachmentColorEncoding           = 0x8210, // GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING
    FramebufferAttachmentComponentType           = 0x8211, // GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE
    FramebufferAttachmentRedSize                 = 0x8212, // GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE
    FramebufferAttachmentGreenSize               = 0x8213, // GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE
    FramebufferAttachmentBlueSize                = 0x8214, // GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE
    FramebufferAttachmentAlphaSize               = 0x8215, // GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE
    FramebufferAttachmentDepthSize               = 0x8216, // GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE
    FramebufferAttachmentStencilSize             = 0x8217, // GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE
    FramebufferAttachmentObjectType              = 0x8CD0, // GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
    FramebufferAttachmentObjectName              = 0x8CD1, // GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
    FramebufferAttachmentTextureLevel            = 0x8CD2, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL
    FramebufferAttachmentTextureCubeMapFace      = 0x8CD3, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE
    FramebufferAttachmentTexture3DZoffset        = 0x8CD4, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT
    FramebufferAttachmentLayered                 = 0x8DA7, // GL_FRAMEBUFFER_ATTACHMENT_LAYERED
    FramebufferAttachmentTextureNumViewsOvr      = 0x9630, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR
    FramebufferAttachmentTextureBaseViewIndexOvr = 0x9632, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR
}; // enum class FramebufferAttachmentParameterName

enum class FramebufferParameterName : GLenum {
    FramebufferDefaultWidth                = 0x9310, // GL_FRAMEBUFFER_DEFAULT_WIDTH
    FramebufferDefaultHeight               = 0x9311, // GL_FRAMEBUFFER_DEFAULT_HEIGHT
    FramebufferDefaultLayers               = 0x9312, // GL_FRAMEBUFFER_DEFAULT_LAYERS
    FramebufferDefaultSamples              = 0x9313, // GL_FRAMEBUFFER_DEFAULT_SAMPLES
    FramebufferDefaultFixedSampleLocations = 0x9314, // GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
}; // enum class FramebufferParameterName

enum class FramebufferStatus : GLenum {
    FramebufferUndefined                   = 0x8219, // GL_FRAMEBUFFER_UNDEFINED
    FramebufferComplete                    = 0x8CD5, // GL_FRAMEBUFFER_COMPLETE
    FramebufferIncompleteAttachment        = 0x8CD6, // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
    FramebufferIncompleteMissingAttachment = 0x8CD7, // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
    FramebufferIncompleteDrawBuffer        = 0x8CDB, // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
    FramebufferIncompleteReadBuffer        = 0x8CDC, // GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
    FramebufferUnsupported                 = 0x8CDD, // GL_FRAMEBUFFER_UNSUPPORTED
    FramebufferIncompleteMultisample       = 0x8D56, // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
    FramebufferIncompleteLayerTargets      = 0x8DA8, // GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
}; // enum class FramebufferStatus

enum class FramebufferTarget : GLenum {
    ReadFramebuffer = 0x8CA8, // GL_READ_FRAMEBUFFER
    DrawFramebuffer = 0x8CA9, // GL_DRAW_FRAMEBUFFER
    Framebuffer     = 0x8D40, // GL_FRAMEBUFFER
}; // enum class FramebufferTarget

enum class FrontFaceDirection : GLenum {
    CW  = 0x0900, // GL_CW
    CCW = 0x0901, // GL_CCW
}; // enum class FrontFaceDirection

enum class GetColorTableParameterPNameSGI : GLenum {
    ColorTableScale         = 0x80D6, // GL_COLOR_TABLE_SCALE_SGI
    ColorTableBias          = 0x80D7, // GL_COLOR_TABLE_BIAS_SGI
    ColorTableFormat        = 0x80D8, // GL_COLOR_TABLE_FORMAT_SGI
    ColorTableWidth         = 0x80D9, // GL_COLOR_TABLE_WIDTH_SGI
    ColorTableRedSize       = 0x80DA, // GL_COLOR_TABLE_RED_SIZE_SGI
    ColorTableGreenSize     = 0x80DB, // GL_COLOR_TABLE_GREEN_SIZE_SGI
    ColorTableBlueSize      = 0x80DC, // GL_COLOR_TABLE_BLUE_SIZE_SGI
    ColorTableAlphaSize     = 0x80DD, // GL_COLOR_TABLE_ALPHA_SIZE_SGI
    ColorTableLuminanceSize = 0x80DE, // GL_COLOR_TABLE_LUMINANCE_SIZE_SGI
    ColorTableIntensitySize = 0x80DF, // GL_COLOR_TABLE_INTENSITY_SIZE_SGI
}; // enum class GetColorTableParameterPNameSGI

enum class GetConvolutionParameter : GLenum {
    ConvolutionBorderMode  = 0x8013, // GL_CONVOLUTION_BORDER_MODE_EXT
    ConvolutionFilterScale = 0x8014, // GL_CONVOLUTION_FILTER_SCALE_EXT
    ConvolutionFilterBias  = 0x8015, // GL_CONVOLUTION_FILTER_BIAS_EXT
    ConvolutionFormat      = 0x8017, // GL_CONVOLUTION_FORMAT_EXT
    ConvolutionWidth       = 0x8018, // GL_CONVOLUTION_WIDTH_EXT
    ConvolutionHeight      = 0x8019, // GL_CONVOLUTION_HEIGHT_EXT
    MaxConvolutionWidth    = 0x801A, // GL_MAX_CONVOLUTION_WIDTH_EXT
    MaxConvolutionHeight   = 0x801B, // GL_MAX_CONVOLUTION_HEIGHT_EXT
}; // enum class GetConvolutionParameter

enum class GetFramebufferParameter : GLenum {
    Doublebuffer                           = 0x0C32, // GL_DOUBLEBUFFER
    Stereo                                 = 0x0C33, // GL_STEREO
    SampleBuffers                          = 0x80A8, // GL_SAMPLE_BUFFERS
    Samples                                = 0x80A9, // GL_SAMPLES
    ImplementationColorReadType            = 0x8B9A, // GL_IMPLEMENTATION_COLOR_READ_TYPE
    ImplementationColorReadFormat          = 0x8B9B, // GL_IMPLEMENTATION_COLOR_READ_FORMAT
    FramebufferDefaultWidth                = 0x9310, // GL_FRAMEBUFFER_DEFAULT_WIDTH
    FramebufferDefaultHeight               = 0x9311, // GL_FRAMEBUFFER_DEFAULT_HEIGHT
    FramebufferDefaultLayers               = 0x9312, // GL_FRAMEBUFFER_DEFAULT_LAYERS
    FramebufferDefaultSamples              = 0x9313, // GL_FRAMEBUFFER_DEFAULT_SAMPLES
    FramebufferDefaultFixedSampleLocations = 0x9314, // GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
}; // enum class GetFramebufferParameter

enum class GetHistogramParameterPNameEXT : GLenum {
    HistogramWidth         = 0x8026, // GL_HISTOGRAM_WIDTH_EXT
    HistogramFormat        = 0x8027, // GL_HISTOGRAM_FORMAT_EXT
    HistogramRedSize       = 0x8028, // GL_HISTOGRAM_RED_SIZE_EXT
    HistogramGreenSize     = 0x8029, // GL_HISTOGRAM_GREEN_SIZE_EXT
    HistogramBlueSize      = 0x802A, // GL_HISTOGRAM_BLUE_SIZE_EXT
    HistogramAlphaSize     = 0x802B, // GL_HISTOGRAM_ALPHA_SIZE_EXT
    HistogramLuminanceSize = 0x802C, // GL_HISTOGRAM_LUMINANCE_SIZE_EXT
    HistogramSink          = 0x802D, // GL_HISTOGRAM_SINK_EXT
}; // enum class GetHistogramParameterPNameEXT

enum class GetMinmaxParameterPNameEXT : GLenum {
    MinmaxFormat = 0x802F, // GL_MINMAX_FORMAT_EXT
    MinmaxSink   = 0x8030, // GL_MINMAX_SINK_EXT
}; // enum class GetMinmaxParameterPNameEXT

enum class GetMultisamplePNameNV : GLenum {
    SampleLocation             = 0x8E50, // GL_SAMPLE_LOCATION_ARB
    ProgrammableSampleLocation = 0x9341, // GL_PROGRAMMABLE_SAMPLE_LOCATION_ARB
}; // enum class GetMultisamplePNameNV

enum class GetPName : GLenum {
    PointSize                             = 0x0B11, // GL_POINT_SIZE
    PointSizeRange                        = 0x0B12, // GL_POINT_SIZE_RANGE
    PointSizeGranularity                  = 0x0B13, // GL_POINT_SIZE_GRANULARITY
    LineSmooth                            = 0x0B20, // GL_LINE_SMOOTH
    LineWidth                             = 0x0B21, // GL_LINE_WIDTH
    LineWidthRange                        = 0x0B22, // GL_LINE_WIDTH_RANGE
    LineWidthGranularity                  = 0x0B23, // GL_LINE_WIDTH_GRANULARITY
    PolygonMode                           = 0x0B40, // GL_POLYGON_MODE
    PolygonSmooth                         = 0x0B41, // GL_POLYGON_SMOOTH
    CullFace                              = 0x0B44, // GL_CULL_FACE
    CullFaceMode                          = 0x0B45, // GL_CULL_FACE_MODE
    FrontFace                             = 0x0B46, // GL_FRONT_FACE
    Fog                                   = 0x0B60, // GL_FOG
    DepthRange                            = 0x0B70, // GL_DEPTH_RANGE
    DepthTest                             = 0x0B71, // GL_DEPTH_TEST
    DepthWritemask                        = 0x0B72, // GL_DEPTH_WRITEMASK
    DepthClearValue                       = 0x0B73, // GL_DEPTH_CLEAR_VALUE
    DepthFunc                             = 0x0B74, // GL_DEPTH_FUNC
    StencilTest                           = 0x0B90, // GL_STENCIL_TEST
    StencilClearValue                     = 0x0B91, // GL_STENCIL_CLEAR_VALUE
    StencilFunc                           = 0x0B92, // GL_STENCIL_FUNC
    StencilValueMask                      = 0x0B93, // GL_STENCIL_VALUE_MASK
    StencilFail                           = 0x0B94, // GL_STENCIL_FAIL
    StencilPassDepthFail                  = 0x0B95, // GL_STENCIL_PASS_DEPTH_FAIL
    StencilPassDepthPass                  = 0x0B96, // GL_STENCIL_PASS_DEPTH_PASS
    StencilRef                            = 0x0B97, // GL_STENCIL_REF
    StencilWritemask                      = 0x0B98, // GL_STENCIL_WRITEMASK
    Viewport                              = 0x0BA2, // GL_VIEWPORT
    Modelview0StackDepth                  = 0x0BA3, // GL_MODELVIEW0_STACK_DEPTH_EXT
    Modelview0Matrix                      = 0x0BA6, // GL_MODELVIEW0_MATRIX_EXT
    Dither                                = 0x0BD0, // GL_DITHER
    BlendDst                              = 0x0BE0, // GL_BLEND_DST
    BlendSrc                              = 0x0BE1, // GL_BLEND_SRC
    Blend                                 = 0x0BE2, // GL_BLEND
    LogicOpMode                           = 0x0BF0, // GL_LOGIC_OP_MODE
    ColorLogicOp                          = 0x0BF2, // GL_COLOR_LOGIC_OP
    DrawBuffer                            = 0x0C01, // GL_DRAW_BUFFER
    ReadBuffer                            = 0x0C02, // GL_READ_BUFFER
    ScissorBox                            = 0x0C10, // GL_SCISSOR_BOX
    ScissorTest                           = 0x0C11, // GL_SCISSOR_TEST
    ColorClearValue                       = 0x0C22, // GL_COLOR_CLEAR_VALUE
    ColorWritemask                        = 0x0C23, // GL_COLOR_WRITEMASK
    Doublebuffer                          = 0x0C32, // GL_DOUBLEBUFFER
    Stereo                                = 0x0C33, // GL_STEREO
    LineSmoothHint                        = 0x0C52, // GL_LINE_SMOOTH_HINT
    PolygonSmoothHint                     = 0x0C53, // GL_POLYGON_SMOOTH_HINT
    UnpackSwapBytes                       = 0x0CF0, // GL_UNPACK_SWAP_BYTES
    UnpackLsbFirst                        = 0x0CF1, // GL_UNPACK_LSB_FIRST
    UnpackRowLength                       = 0x0CF2, // GL_UNPACK_ROW_LENGTH
    UnpackSkipRows                        = 0x0CF3, // GL_UNPACK_SKIP_ROWS
    UnpackSkipPixels                      = 0x0CF4, // GL_UNPACK_SKIP_PIXELS
    UnpackAlignment                       = 0x0CF5, // GL_UNPACK_ALIGNMENT
    PackSwapBytes                         = 0x0D00, // GL_PACK_SWAP_BYTES
    PackLsbFirst                          = 0x0D01, // GL_PACK_LSB_FIRST
    PackRowLength                         = 0x0D02, // GL_PACK_ROW_LENGTH
    PackSkipRows                          = 0x0D03, // GL_PACK_SKIP_ROWS
    PackSkipPixels                        = 0x0D04, // GL_PACK_SKIP_PIXELS
    PackAlignment                         = 0x0D05, // GL_PACK_ALIGNMENT
    MaxClipDistances                      = 0x0D32, // GL_MAX_CLIP_DISTANCES
    MaxTextureSize                        = 0x0D33, // GL_MAX_TEXTURE_SIZE
    MaxViewportDims                       = 0x0D3A, // GL_MAX_VIEWPORT_DIMS
    SubpixelBits                          = 0x0D50, // GL_SUBPIXEL_BITS
    Texture1D                             = 0x0DE0, // GL_TEXTURE_1D
    Texture2D                             = 0x0DE1, // GL_TEXTURE_2D
    PolygonOffsetUnits                    = 0x2A00, // GL_POLYGON_OFFSET_UNITS
    PolygonOffsetPoint                    = 0x2A01, // GL_POLYGON_OFFSET_POINT
    PolygonOffsetLine                     = 0x2A02, // GL_POLYGON_OFFSET_LINE
    BlendColor                            = 0x8005, // GL_BLEND_COLOR
    BlendEquation                         = 0x8009, // GL_BLEND_EQUATION_EXT
    PackCMYKHint                          = 0x800E, // GL_PACK_CMYK_HINT_EXT
    UnpackCMYKHint                        = 0x800F, // GL_UNPACK_CMYK_HINT_EXT
    Convolution1D                         = 0x8010, // GL_CONVOLUTION_1D_EXT
    Convolution2D                         = 0x8011, // GL_CONVOLUTION_2D_EXT
    Separable2D                           = 0x8012, // GL_SEPARABLE_2D_EXT
    PostConvolutionRedScale               = 0x801C, // GL_POST_CONVOLUTION_RED_SCALE_EXT
    PostConvolutionGreenScale             = 0x801D, // GL_POST_CONVOLUTION_GREEN_SCALE_EXT
    PostConvolutionBlueScale              = 0x801E, // GL_POST_CONVOLUTION_BLUE_SCALE_EXT
    PostConvolutionAlphaScale             = 0x801F, // GL_POST_CONVOLUTION_ALPHA_SCALE_EXT
    PostConvolutionRedBias                = 0x8020, // GL_POST_CONVOLUTION_RED_BIAS_EXT
    PostConvolutionGreenBias              = 0x8021, // GL_POST_CONVOLUTION_GREEN_BIAS_EXT
    PostConvolutionBlueBias               = 0x8022, // GL_POST_CONVOLUTION_BLUE_BIAS_EXT
    PostConvolutionAlphaBias              = 0x8023, // GL_POST_CONVOLUTION_ALPHA_BIAS_EXT
    Histogram                             = 0x8024, // GL_HISTOGRAM_EXT
    Minmax                                = 0x802E, // GL_MINMAX_EXT
    PolygonOffsetFill                     = 0x8037, // GL_POLYGON_OFFSET_FILL
    PolygonOffsetFactor                   = 0x8038, // GL_POLYGON_OFFSET_FACTOR
    PolygonOffsetBias                     = 0x8039, // GL_POLYGON_OFFSET_BIAS_EXT
    RescaleNormal                         = 0x803A, // GL_RESCALE_NORMAL_EXT
    TextureBinding1D                      = 0x8068, // GL_TEXTURE_BINDING_1D
    TextureBinding2D                      = 0x8069, // GL_TEXTURE_BINDING_2D
    Texture3DBinding                      = 0x806A, // GL_TEXTURE_3D_BINDING_EXT
    PackSkipImageS                        = 0x806B, // GL_PACK_SKIP_IMAGES
    PackImageHeight                       = 0x806C, // GL_PACK_IMAGE_HEIGHT
    UnpackSkipImageS                      = 0x806D, // GL_UNPACK_SKIP_IMAGES
    UnpackImageHeight                     = 0x806E, // GL_UNPACK_IMAGE_HEIGHT
    Texture3D                             = 0x806F, // GL_TEXTURE_3D_EXT
    Max3DTextureSize                      = 0x8073, // GL_MAX_3D_TEXTURE_SIZE
    VertexArray                           = 0x8074, // GL_VERTEX_ARRAY
    VertexArrayCount                      = 0x807D, // GL_VERTEX_ARRAY_COUNT_EXT
    NormalArrayCount                      = 0x8080, // GL_NORMAL_ARRAY_COUNT_EXT
    ColorArrayCount                       = 0x8084, // GL_COLOR_ARRAY_COUNT_EXT
    IndexArrayCount                       = 0x8087, // GL_INDEX_ARRAY_COUNT_EXT
    TextureCoordArrayCount                = 0x808B, // GL_TEXTURE_COORD_ARRAY_COUNT_EXT
    EdgeFlagArrayCount                    = 0x808D, // GL_EDGE_FLAG_ARRAY_COUNT_EXT
    Interlace                             = 0x8094, // GL_INTERLACE_SGIX
    DetailTexture2DBinding                = 0x8096, // GL_DETAIL_TEXTURE_2D_BINDING_SGIS
    Multisample                           = 0x809D, // GL_MULTISAMPLE_SGIS
    SampleAlphaToMask                     = 0x809E, // GL_SAMPLE_ALPHA_TO_MASK_SGIS
    SampleAlphaToOne                      = 0x809F, // GL_SAMPLE_ALPHA_TO_ONE_SGIS
    SampleMask                            = 0x80A0, // GL_SAMPLE_MASK_SGIS
    SampleBuffers                         = 0x80A8, // GL_SAMPLE_BUFFERS
    Samples                               = 0x80A9, // GL_SAMPLES
    SampleCoverageValue                   = 0x80AA, // GL_SAMPLE_COVERAGE_VALUE
    SampleCoverageInvert                  = 0x80AB, // GL_SAMPLE_COVERAGE_INVERT
    SamplePattern                         = 0x80AC, // GL_SAMPLE_PATTERN_SGIS
    ColorMatrix                           = 0x80B1, // GL_COLOR_MATRIX_SGI
    ColorMatrixStackDepth                 = 0x80B2, // GL_COLOR_MATRIX_STACK_DEPTH_SGI
    MaxColorMatrixStackDepth              = 0x80B3, // GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI
    PostColorMatrixRedScale               = 0x80B4, // GL_POST_COLOR_MATRIX_RED_SCALE_SGI
    PostColorMatrixGreenScale             = 0x80B5, // GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI
    PostColorMatrixBlueScale              = 0x80B6, // GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI
    PostColorMatrixAlphaScale             = 0x80B7, // GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI
    PostColorMatrixRedBias                = 0x80B8, // GL_POST_COLOR_MATRIX_RED_BIAS_SGI
    PostColorMatrixGreenBias              = 0x80B9, // GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI
    PostColorMatrixBlueBias               = 0x80BA, // GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI
    PostColorMatrixAlphaBias              = 0x80BB, // GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI
    TextureColorTable                     = 0x80BC, // GL_TEXTURE_COLOR_TABLE_SGI
    BlendDstRGB                           = 0x80C8, // GL_BLEND_DST_RGB
    BlendSrcRGB                           = 0x80C9, // GL_BLEND_SRC_RGB
    BlendDstAlpha                         = 0x80CA, // GL_BLEND_DST_ALPHA
    BlendSrcAlpha                         = 0x80CB, // GL_BLEND_SRC_ALPHA
    ColorTable                            = 0x80D0, // GL_COLOR_TABLE_SGI
    PostConvolutionColorTable             = 0x80D1, // GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    PostColorMatrixColorTable             = 0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    MaxElementsVertices                   = 0x80E8, // GL_MAX_ELEMENTS_VERTICES
    MaxElementsIndices                    = 0x80E9, // GL_MAX_ELEMENTS_INDICES
    PointSizeMin                          = 0x8126, // GL_POINT_SIZE_MIN_SGIS
    PointSizeMax                          = 0x8127, // GL_POINT_SIZE_MAX_SGIS
    PointFadeThresholdSize                = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE
    DistanceAttenuation                   = 0x8129, // GL_DISTANCE_ATTENUATION_SGIS
    FogFuncPoints                         = 0x812B, // GL_FOG_FUNC_POINTS_SGIS
    MaxFogFuncPoints                      = 0x812C, // GL_MAX_FOG_FUNC_POINTS_SGIS
    PackSkipVolumes                       = 0x8130, // GL_PACK_SKIP_VOLUMES_SGIS
    PackImageDepth                        = 0x8131, // GL_PACK_IMAGE_DEPTH_SGIS
    UnpackSkipVolumes                     = 0x8132, // GL_UNPACK_SKIP_VOLUMES_SGIS
    UnpackImageDepth                      = 0x8133, // GL_UNPACK_IMAGE_DEPTH_SGIS
    Texture4D                             = 0x8134, // GL_TEXTURE_4D_SGIS
    Max4DTextureSize                      = 0x8138, // GL_MAX_4D_TEXTURE_SIZE_SGIS
    PixelTexGen                           = 0x8139, // GL_PIXEL_TEX_GEN_SGIX
    PixelTileBestAlignment                = 0x813E, // GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX
    PixelTileCacheIncrement               = 0x813F, // GL_PIXEL_TILE_CACHE_INCREMENT_SGIX
    PixelTileWidth                        = 0x8140, // GL_PIXEL_TILE_WIDTH_SGIX
    PixelTileHeight                       = 0x8141, // GL_PIXEL_TILE_HEIGHT_SGIX
    PixelTileGridWidth                    = 0x8142, // GL_PIXEL_TILE_GRID_WIDTH_SGIX
    PixelTileGridHeight                   = 0x8143, // GL_PIXEL_TILE_GRID_HEIGHT_SGIX
    PixelTileGridDepth                    = 0x8144, // GL_PIXEL_TILE_GRID_DEPTH_SGIX
    PixelTileCacheSize                    = 0x8145, // GL_PIXEL_TILE_CACHE_SIZE_SGIX
    Sprite                                = 0x8148, // GL_SPRITE_SGIX
    SpriteMode                            = 0x8149, // GL_SPRITE_MODE_SGIX
    SpriteAxis                            = 0x814A, // GL_SPRITE_AXIS_SGIX
    SpriteTranslation                     = 0x814B, // GL_SPRITE_TRANSLATION_SGIX
    Texture4DBinding                      = 0x814F, // GL_TEXTURE_4D_BINDING_SGIS
    MaxClipmapDepth                       = 0x8177, // GL_MAX_CLIPMAP_DEPTH_SGIX
    MaxClipmapVirtualDepth                = 0x8178, // GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX
    PostTextureFilterBiasRange            = 0x817B, // GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX
    PostTextureFilterScaleRange           = 0x817C, // GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX
    ReferencePlane                        = 0x817D, // GL_REFERENCE_PLANE_SGIX
    ReferencePlaneEquation                = 0x817E, // GL_REFERENCE_PLANE_EQUATION_SGIX
    IrInstrument1                         = 0x817F, // GL_IR_INSTRUMENT1_SGIX
    InstrumentMeasurements                = 0x8181, // GL_INSTRUMENT_MEASUREMENTS_SGIX
    CalligraphicFragment                  = 0x8183, // GL_CALLIGRAPHIC_FRAGMENT_SGIX
    Framezoom                             = 0x818B, // GL_FRAMEZOOM_SGIX
    FramezoomFactor                       = 0x818C, // GL_FRAMEZOOM_FACTOR_SGIX
    MaxFramezoomFactor                    = 0x818D, // GL_MAX_FRAMEZOOM_FACTOR_SGIX
    GenerateMipmapHint                    = 0x8192, // GL_GENERATE_MIPMAP_HINT_SGIS
    DeformationSMask                      = 0x8196, // GL_DEFORMATIONS_MASK_SGIX
    FogOffset                             = 0x8198, // GL_FOG_OFFSET_SGIX
    FogOffsetValue                        = 0x8199, // GL_FOG_OFFSET_VALUE_SGIX
    SharedTexturePalette                  = 0x81FB, // GL_SHARED_TEXTURE_PALETTE_EXT
    MajorVersion                          = 0x821B, // GL_MAJOR_VERSION
    MinorVersion                          = 0x821C, // GL_MINOR_VERSION
    NumExtensions                         = 0x821D, // GL_NUM_EXTENSIONS
    ContextFlags                          = 0x821E, // GL_CONTEXT_FLAGS
    ProgramPipelineBinding                = 0x825A, // GL_PROGRAM_PIPELINE_BINDING
    MaxViewportS                          = 0x825B, // GL_MAX_VIEWPORTS
    ViewportSubpixelBits                  = 0x825C, // GL_VIEWPORT_SUBPIXEL_BITS
    ViewportBoundsRange                   = 0x825D, // GL_VIEWPORT_BOUNDS_RANGE
    LayerProvokingVertex                  = 0x825E, // GL_LAYER_PROVOKING_VERTEX
    ViewportIndexProvokingVertex          = 0x825F, // GL_VIEWPORT_INDEX_PROVOKING_VERTEX
    MaxComputeUniformComponents           = 0x8263, // GL_MAX_COMPUTE_UNIFORM_COMPONENTS
    MaxComputeAtomicCounterBuffers        = 0x8264, // GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS
    MaxComputeAtomicCounters              = 0x8265, // GL_MAX_COMPUTE_ATOMIC_COUNTERS
    MaxCombinedComputeUniformComponents   = 0x8266, // GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
    MaxDebugGroupStackDepth               = 0x826C, // GL_MAX_DEBUG_GROUP_STACK_DEPTH
    DebugGroupStackDepth                  = 0x826D, // GL_DEBUG_GROUP_STACK_DEPTH
    MaxUniformLocations                   = 0x826E, // GL_MAX_UNIFORM_LOCATIONS
    VertexBindingDivisor                  = 0x82D6, // GL_VERTEX_BINDING_DIVISOR
    VertexBindingOffset                   = 0x82D7, // GL_VERTEX_BINDING_OFFSET
    VertexBindingStride                   = 0x82D8, // GL_VERTEX_BINDING_STRIDE
    MaxVertexAttribRelativeOffset         = 0x82D9, // GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET
    MaxVertexAttribBindings               = 0x82DA, // GL_MAX_VERTEX_ATTRIB_BINDINGS
    MaxLabelLength                        = 0x82E8, // GL_MAX_LABEL_LENGTH
    ConvolutionHint                       = 0x8316, // GL_CONVOLUTION_HINT_SGIX
    AsyncMarker                           = 0x8329, // GL_ASYNC_MARKER_SGIX
    PixelTexGenMode                       = 0x832B, // GL_PIXEL_TEX_GEN_MODE_SGIX
    AsyncHistogram                        = 0x832C, // GL_ASYNC_HISTOGRAM_SGIX
    MaxAsyncHistogram                     = 0x832D, // GL_MAX_ASYNC_HISTOGRAM_SGIX
    PixelTexture                          = 0x8353, // GL_PIXEL_TEXTURE_SGIS
    AsyncTexImage                         = 0x835C, // GL_ASYNC_TEX_IMAGE_SGIX
    AsyncDrawPixels                       = 0x835D, // GL_ASYNC_DRAW_PIXELS_SGIX
    AsyncReadPixels                       = 0x835E, // GL_ASYNC_READ_PIXELS_SGIX
    MaxAsyncTexImage                      = 0x835F, // GL_MAX_ASYNC_TEX_IMAGE_SGIX
    MaxAsyncDrawPixels                    = 0x8360, // GL_MAX_ASYNC_DRAW_PIXELS_SGIX
    MaxAsyncReadPixels                    = 0x8361, // GL_MAX_ASYNC_READ_PIXELS_SGIX
    VertexPreclip                         = 0x83EE, // GL_VERTEX_PRECLIP_SGIX
    VertexPreclipHint                     = 0x83EF, // GL_VERTEX_PRECLIP_HINT_SGIX
    FragmentLighting                      = 0x8400, // GL_FRAGMENT_LIGHTING_SGIX
    FragmentColorMaterial                 = 0x8401, // GL_FRAGMENT_COLOR_MATERIAL_SGIX
    FragmentColorMaterialFace             = 0x8402, // GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX
    FragmentColorMaterialParameter        = 0x8403, // GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX
    MaxFragmentLightS                     = 0x8404, // GL_MAX_FRAGMENT_LIGHTS_SGIX
    MaxActiveLightS                       = 0x8405, // GL_MAX_ACTIVE_LIGHTS_SGIX
    LightEnvMode                          = 0x8407, // GL_LIGHT_ENV_MODE_SGIX
    FragmentLightModelLocalViewer         = 0x8408, // GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX
    FragmentLightModelTwoSide             = 0x8409, // GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX
    FragmentLightModelAmbient             = 0x840A, // GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX
    FragmentLightModelNormalInterpolation = 0x840B, // GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX
    FragmentLight0                        = 0x840C, // GL_FRAGMENT_LIGHT0_SGIX
    PackResample                          = 0x842E, // GL_PACK_RESAMPLE_SGIX - Formerly 0x842C in SGI specfile
    UnpackResample                        = 0x842F, // GL_UNPACK_RESAMPLE_SGIX - Formerly 0x842D in SGI specfile
    AliasedLineWidthRange                 = 0x846E, // GL_ALIASED_LINE_WIDTH_RANGE
    ActiveTexture                         = 0x84E0, // GL_ACTIVE_TEXTURE
    MaxRenderbufferSize                   = 0x84E8, // GL_MAX_RENDERBUFFER_SIZE
    TextureCompressionHint                = 0x84EF, // GL_TEXTURE_COMPRESSION_HINT
    TextureBindingRectangle               = 0x84F6, // GL_TEXTURE_BINDING_RECTANGLE
    MaxRectangleTextureSize               = 0x84F8, // GL_MAX_RECTANGLE_TEXTURE_SIZE
    MaxTextureLODBias                     = 0x84FD, // GL_MAX_TEXTURE_LOD_BIAS
    TextureBindingCubeMap                 = 0x8514, // GL_TEXTURE_BINDING_CUBE_MAP
    MaxCubeMapTextureSize                 = 0x851C, // GL_MAX_CUBE_MAP_TEXTURE_SIZE
    PackSubsampleRate                     = 0x85A0, // GL_PACK_SUBSAMPLE_RATE_SGIX
    UnpackSubsampleRate                   = 0x85A1, // GL_UNPACK_SUBSAMPLE_RATE_SGIX
    VertexArrayBinding                    = 0x85B5, // GL_VERTEX_ARRAY_BINDING
    ProgramPointSize                      = 0x8642, // GL_PROGRAM_POINT_SIZE
    NumCompressedTextureFormats           = 0x86A2, // GL_NUM_COMPRESSED_TEXTURE_FORMATS
    CompressedTextureFormats              = 0x86A3, // GL_COMPRESSED_TEXTURE_FORMATS
    NumProgramBinaryFormats               = 0x87FE, // GL_NUM_PROGRAM_BINARY_FORMATS
    ProgramBinaryFormats                  = 0x87FF, // GL_PROGRAM_BINARY_FORMATS
    StencilBackFunc                       = 0x8800, // GL_STENCIL_BACK_FUNC
    StencilBackFail                       = 0x8801, // GL_STENCIL_BACK_FAIL
    StencilBackPassDepthFail              = 0x8802, // GL_STENCIL_BACK_PASS_DEPTH_FAIL
    StencilBackPassDepthPass              = 0x8803, // GL_STENCIL_BACK_PASS_DEPTH_PASS
    MaxDrawBuffers                        = 0x8824, // GL_MAX_DRAW_BUFFERS
    BlendEquationAlpha                    = 0x883D, // GL_BLEND_EQUATION_ALPHA
    MaxVertexAttribs                      = 0x8869, // GL_MAX_VERTEX_ATTRIBS
    MaxTextureImageUnits                  = 0x8872, // GL_MAX_TEXTURE_IMAGE_UNITS
    ArrayBufferBinding                    = 0x8894, // GL_ARRAY_BUFFER_BINDING
    ElementArrayBufferBinding             = 0x8895, // GL_ELEMENT_ARRAY_BUFFER_BINDING
    PixelPackBufferBinding                = 0x88ED, // GL_PIXEL_PACK_BUFFER_BINDING
    PixelUnpackBufferBinding              = 0x88EF, // GL_PIXEL_UNPACK_BUFFER_BINDING
    MaxDualSourceDrawBuffers              = 0x88FC, // GL_MAX_DUAL_SOURCE_DRAW_BUFFERS
    MaxArrayTextureLayers                 = 0x88FF, // GL_MAX_ARRAY_TEXTURE_LAYERS
    MinProgramTexelOffset                 = 0x8904, // GL_MIN_PROGRAM_TEXEL_OFFSET
    MaxProgramTexelOffset                 = 0x8905, // GL_MAX_PROGRAM_TEXEL_OFFSET
    SamplerBinding                        = 0x8919, // GL_SAMPLER_BINDING
    UniformBufferBinding                  = 0x8A28, // GL_UNIFORM_BUFFER_BINDING
    UniformBufferStart                    = 0x8A29, // GL_UNIFORM_BUFFER_START
    UniformBufferSize                     = 0x8A2A, // GL_UNIFORM_BUFFER_SIZE
    MaxVertexUniformBlocks                = 0x8A2B, // GL_MAX_VERTEX_UNIFORM_BLOCKS
    MaxGeometryUniformBlocks              = 0x8A2C, // GL_MAX_GEOMETRY_UNIFORM_BLOCKS
    MaxFragmentUniformBlocks              = 0x8A2D, // GL_MAX_FRAGMENT_UNIFORM_BLOCKS
    MaxCombinedUniformBlocks              = 0x8A2E, // GL_MAX_COMBINED_UNIFORM_BLOCKS
    MaxUniformBufferBindings              = 0x8A2F, // GL_MAX_UNIFORM_BUFFER_BINDINGS
    MaxUniformBlockSize                   = 0x8A30, // GL_MAX_UNIFORM_BLOCK_SIZE
    MaxCombinedVertexUniformComponents    = 0x8A31, // GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
    MaxCombinedGeometryUniformComponents  = 0x8A32, // GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS
    MaxCombinedFragmentUniformComponents  = 0x8A33, // GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
    UniformBufferOffsetAlignment          = 0x8A34, // GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
    MaxFragmentUniformComponents          = 0x8B49, // GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
    MaxVertexUniformComponents            = 0x8B4A, // GL_MAX_VERTEX_UNIFORM_COMPONENTS
    MaxVaryingComponents                  = 0x8B4B, // GL_MAX_VARYING_COMPONENTS
    MaxVertexTextureImageUnits            = 0x8B4C, // GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
    MaxCombinedTextureImageUnits          = 0x8B4D, // GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    FragmentShaderDerivativeHint          = 0x8B8B, // GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    CurrentProgram                        = 0x8B8D, // GL_CURRENT_PROGRAM
    ImplementationColorReadType           = 0x8B9A, // GL_IMPLEMENTATION_COLOR_READ_TYPE
    ImplementationColorReadFormat         = 0x8B9B, // GL_IMPLEMENTATION_COLOR_READ_FORMAT
    TextureBinding1DArray                 = 0x8C1C, // GL_TEXTURE_BINDING_1D_ARRAY
    TextureBinding2DArray                 = 0x8C1D, // GL_TEXTURE_BINDING_2D_ARRAY
    MaxGeometryTextureImageUnits          = 0x8C29, // GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS
    MaxTextureBufferSize                  = 0x8C2B, // GL_MAX_TEXTURE_BUFFER_SIZE
    TextureBindingBuffer                  = 0x8C2C, // GL_TEXTURE_BINDING_BUFFER
    TransformFeedbackBufferStart          = 0x8C84, // GL_TRANSFORM_FEEDBACK_BUFFER_START
    TransformFeedbackBufferSize           = 0x8C85, // GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    TransformFeedbackBufferBinding        = 0x8C8F, // GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    StencilBackRef                        = 0x8CA3, // GL_STENCIL_BACK_REF
    StencilBackValueMask                  = 0x8CA4, // GL_STENCIL_BACK_VALUE_MASK
    StencilBackWritemask                  = 0x8CA5, // GL_STENCIL_BACK_WRITEMASK
    DrawFramebufferBinding                = 0x8CA6, // GL_DRAW_FRAMEBUFFER_BINDING
    RenderbufferBinding                   = 0x8CA7, // GL_RENDERBUFFER_BINDING
    ReadFramebufferBinding                = 0x8CAA, // GL_READ_FRAMEBUFFER_BINDING
    MaxColorAttachmentS                   = 0x8CDF, // GL_MAX_COLOR_ATTACHMENTS
    MaxElementIndex                       = 0x8D6B, // GL_MAX_ELEMENT_INDEX
    MaxGeometryUniformComponents          = 0x8DDF, // GL_MAX_GEOMETRY_UNIFORM_COMPONENTS
    ShaderBinaryFormats                   = 0x8DF8, // GL_SHADER_BINARY_FORMATS
    NumShaderBinaryFormats                = 0x8DF9, // GL_NUM_SHADER_BINARY_FORMATS
    ShaderCompiler                        = 0x8DFA, // GL_SHADER_COMPILER
    MaxVertexUniformVectors               = 0x8DFB, // GL_MAX_VERTEX_UNIFORM_VECTORS
    MaxVaryingVectors                     = 0x8DFC, // GL_MAX_VARYING_VECTORS
    MaxFragmentUniformVectors             = 0x8DFD, // GL_MAX_FRAGMENT_UNIFORM_VECTORS
    Timestamp                             = 0x8E28, // GL_TIMESTAMP
    ProvokingVertex                       = 0x8E4F, // GL_PROVOKING_VERTEX
    MaxSampleMaskWords                    = 0x8E59, // GL_MAX_SAMPLE_MASK_WORDS
    MaxTessControlUniformBlocks           = 0x8E89, // GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS
    MaxTessEvaluationUniformBlocks        = 0x8E8A, // GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS
    PrimitiveRestartIndex                 = 0x8F9E, // GL_PRIMITIVE_RESTART_INDEX
    MinMapBufferAlignment                 = 0x90BC, // GL_MIN_MAP_BUFFER_ALIGNMENT
    ShaderStorageBufferBinding            = 0x90D3, // GL_SHADER_STORAGE_BUFFER_BINDING
    ShaderStorageBufferStart              = 0x90D4, // GL_SHADER_STORAGE_BUFFER_START
    ShaderStorageBufferSize               = 0x90D5, // GL_SHADER_STORAGE_BUFFER_SIZE
    MaxVertexShaderStorageBlocks          = 0x90D6, // GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS
    MaxGeometryShaderStorageBlocks        = 0x90D7, // GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS
    MaxTessControlShaderStorageBlocks     = 0x90D8, // GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS
    MaxTessEvaluationShaderStorageBlocks  = 0x90D9, // GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS
    MaxFragmentShaderStorageBlocks        = 0x90DA, // GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS
    MaxComputeShaderStorageBlocks         = 0x90DB, // GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
    MaxCombinedShaderStorageBlocks        = 0x90DC, // GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS
    MaxShaderStorageBufferBindings        = 0x90DD, // GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
    ShaderStorageBufferOffsetAlignment    = 0x90DF, // GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT
    MaxComputeWorkGroupInvocations        = 0x90EB, // GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
    DispatchIndirectBufferBinding         = 0x90EF, // GL_DISPATCH_INDIRECT_BUFFER_BINDING
    TextureBinding2DMultisample           = 0x9104, // GL_TEXTURE_BINDING_2D_MULTISAMPLE
    TextureBinding2DMultisampleArray      = 0x9105, // GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY
    MaxColorTextureSamples                = 0x910E, // GL_MAX_COLOR_TEXTURE_SAMPLES
    MaxDepthTextureSamples                = 0x910F, // GL_MAX_DEPTH_TEXTURE_SAMPLES
    MaxIntegerSamples                     = 0x9110, // GL_MAX_INTEGER_SAMPLES
    MaxServerWaitTimeout                  = 0x9111, // GL_MAX_SERVER_WAIT_TIMEOUT
    MaxVertexOutputComponents             = 0x9122, // GL_MAX_VERTEX_OUTPUT_COMPONENTS
    MaxGeometryInputComponents            = 0x9123, // GL_MAX_GEOMETRY_INPUT_COMPONENTS
    MaxGeometryOutputComponents           = 0x9124, // GL_MAX_GEOMETRY_OUTPUT_COMPONENTS
    MaxFragmentInputComponents            = 0x9125, // GL_MAX_FRAGMENT_INPUT_COMPONENTS
    ContextProfileMask                    = 0x9126, // GL_CONTEXT_PROFILE_MASK
    TextureBufferOffsetAlignment          = 0x919F, // GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT
    MaxComputeUniformBlocks               = 0x91BB, // GL_MAX_COMPUTE_UNIFORM_BLOCKS
    MaxComputeTextureImageUnits           = 0x91BC, // GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS
    MaxComputeWorkGroupCount              = 0x91BE, // GL_MAX_COMPUTE_WORK_GROUP_COUNT
    MaxComputeWorkGroupSize               = 0x91BF, // GL_MAX_COMPUTE_WORK_GROUP_SIZE
    MaxVertexAtomicCounters               = 0x92D2, // GL_MAX_VERTEX_ATOMIC_COUNTERS
    MaxTessControlAtomicCounters          = 0x92D3, // GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS
    MaxTessEvaluationAtomicCounters       = 0x92D4, // GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
    MaxGeometryAtomicCounters             = 0x92D5, // GL_MAX_GEOMETRY_ATOMIC_COUNTERS
    MaxFragmentAtomicCounters             = 0x92D6, // GL_MAX_FRAGMENT_ATOMIC_COUNTERS
    MaxCombinedAtomicCounters             = 0x92D7, // GL_MAX_COMBINED_ATOMIC_COUNTERS
    MaxFramebufferWidth                   = 0x9315, // GL_MAX_FRAMEBUFFER_WIDTH
    MaxFramebufferHeight                  = 0x9316, // GL_MAX_FRAMEBUFFER_HEIGHT
    MaxFramebufferLayers                  = 0x9317, // GL_MAX_FRAMEBUFFER_LAYERS
    MaxFramebufferSamples                 = 0x9318, // GL_MAX_FRAMEBUFFER_SAMPLES
    NumDeviceUuids                        = 0x9596, // GL_NUM_DEVICE_UUIDS_EXT
    DeviceUuid                            = 0x9597, // GL_DEVICE_UUID_EXT
    DriverUuid                            = 0x9598, // GL_DRIVER_UUID_EXT
    DeviceLuid                            = 0x9599, // GL_DEVICE_LUID_EXT
    DeviceNodeMask                        = 0x959A, // GL_DEVICE_NODE_MASK_EXT
    ShadingRateImagePerPrimitive          = 0x95B1, // GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV
    ShadingRateImagePaletteCount          = 0x95B2, // GL_SHADING_RATE_IMAGE_PALETTE_COUNT_NV
    MaxTimelineSemaphoreValueDifference   = 0x95B6, // GL_MAX_TIMELINE_SEMAPHORE_VALUE_DIFFERENCE_NV
}; // enum class GetPName

enum class GetPointervPName : GLenum {
    VertexArrayPointer       = 0x808E, // GL_VERTEX_ARRAY_POINTER_EXT
    NormalArrayPointer       = 0x808F, // GL_NORMAL_ARRAY_POINTER_EXT
    ColorArrayPointer        = 0x8090, // GL_COLOR_ARRAY_POINTER_EXT
    IndexArrayPointer        = 0x8091, // GL_INDEX_ARRAY_POINTER_EXT
    TextureCoordArrayPointer = 0x8092, // GL_TEXTURE_COORD_ARRAY_POINTER_EXT
    EdgeFlagArrayPointer     = 0x8093, // GL_EDGE_FLAG_ARRAY_POINTER_EXT
    InstrumentBufferPointer  = 0x8180, // GL_INSTRUMENT_BUFFER_POINTER_SGIX
    DebugCallbackFunction    = 0x8244, // GL_DEBUG_CALLBACK_FUNCTION
    DebugCallbackUserParam   = 0x8245, // GL_DEBUG_CALLBACK_USER_PARAM
}; // enum class GetPointervPName

enum class GetTexBumpParameterATI : GLenum {
    BumpRotMatrix     = 0x8775, // GL_BUMP_ROT_MATRIX_ATI
    BumpRotMatrixSize = 0x8776, // GL_BUMP_ROT_MATRIX_SIZE_ATI
    BumpNumTexUnits   = 0x8777, // GL_BUMP_NUM_TEX_UNITS_ATI
    BumpTexUnits      = 0x8778, // GL_BUMP_TEX_UNITS_ATI
}; // enum class GetTexBumpParameterATI

enum class GetTextureParameter : GLenum {
    TextureWidth               = 0x1000, // GL_TEXTURE_WIDTH
    TextureHeight              = 0x1001, // GL_TEXTURE_HEIGHT
    TextureInternalFormat      = 0x1003, // GL_TEXTURE_INTERNAL_FORMAT
    TextureBorderColor         = 0x1004, // GL_TEXTURE_BORDER_COLOR
    TextureMagFilter           = 0x2800, // GL_TEXTURE_MAG_FILTER
    TextureMinFilter           = 0x2801, // GL_TEXTURE_MIN_FILTER
    TextureWrapS               = 0x2802, // GL_TEXTURE_WRAP_S
    TextureWrapT               = 0x2803, // GL_TEXTURE_WRAP_T
    TextureRedSize             = 0x805C, // GL_TEXTURE_RED_SIZE
    TextureGreenSize           = 0x805D, // GL_TEXTURE_GREEN_SIZE
    TextureBlueSize            = 0x805E, // GL_TEXTURE_BLUE_SIZE
    TextureAlphaSize           = 0x805F, // GL_TEXTURE_ALPHA_SIZE
    TextureDepth               = 0x8071, // GL_TEXTURE_DEPTH_EXT
    TextureWrapR               = 0x8072, // GL_TEXTURE_WRAP_R_EXT
    DetailTextureLevel         = 0x809A, // GL_DETAIL_TEXTURE_LEVEL_SGIS
    DetailTextureMode          = 0x809B, // GL_DETAIL_TEXTURE_MODE_SGIS
    DetailTextureFuncPoints    = 0x809C, // GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS
    SharpenTextureFuncPoints   = 0x80B0, // GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS
    ShadowAmbient              = 0x80BF, // GL_SHADOW_AMBIENT_SGIX
    DualTextureSelect          = 0x8124, // GL_DUAL_TEXTURE_SELECT_SGIS
    QuadTextureSelect          = 0x8125, // GL_QUAD_TEXTURE_SELECT_SGIS
    Texture4DSIZE              = 0x8136, // GL_TEXTURE_4DSIZE_SGIS
    TextureWrapQ               = 0x8137, // GL_TEXTURE_WRAP_Q_SGIS
    TextureMinLOD              = 0x813A, // GL_TEXTURE_MIN_LOD_SGIS
    TextureMaxLOD              = 0x813B, // GL_TEXTURE_MAX_LOD_SGIS
    TextureBaseLevel           = 0x813C, // GL_TEXTURE_BASE_LEVEL_SGIS
    TextureMaxLevel            = 0x813D, // GL_TEXTURE_MAX_LEVEL_SGIS
    TextureFilter4Size         = 0x8147, // GL_TEXTURE_FILTER4_SIZE_SGIS
    TextureClipmapCenter       = 0x8171, // GL_TEXTURE_CLIPMAP_CENTER_SGIX
    TextureClipmapFrame        = 0x8172, // GL_TEXTURE_CLIPMAP_FRAME_SGIX
    TextureClipmapOffset       = 0x8173, // GL_TEXTURE_CLIPMAP_OFFSET_SGIX
    TextureClipmapVirtualDepth = 0x8174, // GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX
    TextureClipmapLODOffset    = 0x8175, // GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX
    TextureClipmapDepth        = 0x8176, // GL_TEXTURE_CLIPMAP_DEPTH_SGIX
    PostTextureFilterBias      = 0x8179, // GL_POST_TEXTURE_FILTER_BIAS_SGIX
    PostTextureFilterScale     = 0x817A, // GL_POST_TEXTURE_FILTER_SCALE_SGIX
    TextureLODBiasS            = 0x818E, // GL_TEXTURE_LOD_BIAS_S_SGIX
    TextureLODBiasT            = 0x818F, // GL_TEXTURE_LOD_BIAS_T_SGIX
    TextureLODBiasR            = 0x8190, // GL_TEXTURE_LOD_BIAS_R_SGIX
    GenerateMipmap             = 0x8191, // GL_GENERATE_MIPMAP_SGIS
    TextureCompare             = 0x819A, // GL_TEXTURE_COMPARE_SGIX
    TextureCompareOperator     = 0x819B, // GL_TEXTURE_COMPARE_OPERATOR_SGIX
    TextureLequalR             = 0x819C, // GL_TEXTURE_LEQUAL_R_SGIX
    TextureGequalR             = 0x819D, // GL_TEXTURE_GEQUAL_R_SGIX
    TextureMaxClampS           = 0x8369, // GL_TEXTURE_MAX_CLAMP_S_SGIX
    TextureMaxClampT           = 0x836A, // GL_TEXTURE_MAX_CLAMP_T_SGIX
    TextureMaxClampR           = 0x836B, // GL_TEXTURE_MAX_CLAMP_R_SGIX
}; // enum class GetTextureParameter

enum class GetVariantValueEXT : GLenum {
    VariantValue       = 0x87E4, // GL_VARIANT_VALUE_EXT
    VariantDatatype    = 0x87E5, // GL_VARIANT_DATATYPE_EXT
    VariantArrayStride = 0x87E6, // GL_VARIANT_ARRAY_STRIDE_EXT
    VariantArrayType   = 0x87E7, // GL_VARIANT_ARRAY_TYPE_EXT
}; // enum class GetVariantValueEXT

// Evans and Sutherland is out of the graphics hardware business
enum class Global : GLenum {
    SharedEdge                                 =    0xC0, // GL_SHARED_EDGE_NV
    ContextLost                                =  0x0507, // GL_CONTEXT_LOST
    PathModelviewStackDepth                    =  0x0BA3, // GL_PATH_MODELVIEW_STACK_DEPTH_NV
    PathProjectionStackDepth                   =  0x0BA4, // GL_PATH_PROJECTION_STACK_DEPTH_NV
    PathModelviewMatrix                        =  0x0BA6, // GL_PATH_MODELVIEW_MATRIX_NV
    PathProjectionMatrix                       =  0x0BA7, // GL_PATH_PROJECTION_MATRIX_NV
    PathMaxModelviewStackDepth                 =  0x0D36, // GL_PATH_MAX_MODELVIEW_STACK_DEPTH_NV
    PathMaxProjectionStackDepth                =  0x0D38, // GL_PATH_MAX_PROJECTION_STACK_DEPTH_NV
    TextureTarget                              =  0x1006, // GL_TEXTURE_TARGET
    Half                                       =  0x140B, // GL_HALF_APPLE
    Fixed                                      =  0x140C, // GL_FIXED_OES
    Xor                                        =  0x1506, // GL_XOR_NV
    Modelview0                                 =  0x1700, // GL_MODELVIEW0_ARB
    PathProjection                             =  0x1701, // GL_PATH_PROJECTION_NV
    Red                                        =  0x1903, // GL_RED_NV
    Green                                      =  0x1904, // GL_GREEN_NV
    Blue                                       =  0x1905, // GL_BLUE_NV
    ConstantColor                              =  0x8001, // GL_CONSTANT_COLOR_EXT
    OneMinusConstantColor                      =  0x8002, // GL_ONE_MINUS_CONSTANT_COLOR_EXT
    ConstantAlpha                              =  0x8003, // GL_CONSTANT_ALPHA_EXT
    OneMinusConstantAlpha                      =  0x8004, // GL_ONE_MINUS_CONSTANT_ALPHA_EXT
    BlendEquation                              =  0x8009, // GL_BLEND_EQUATION
    PolygonOffset                              =  0x8037, // GL_POLYGON_OFFSET_EXT
    PolygonOffsetFactor                        =  0x8038, // GL_POLYGON_OFFSET_FACTOR_EXT
    Intensity                                  =  0x8049, // GL_INTENSITY_EXT
    TextureRedSize                             =  0x805C, // GL_TEXTURE_RED_SIZE_EXT
    TextureGreenSize                           =  0x805D, // GL_TEXTURE_GREEN_SIZE_EXT
    TextureBlueSize                            =  0x805E, // GL_TEXTURE_BLUE_SIZE_EXT
    TextureAlphaSize                           =  0x805F, // GL_TEXTURE_ALPHA_SIZE_EXT
    TextureLuminanceSize                       =  0x8060, // GL_TEXTURE_LUMINANCE_SIZE_EXT
    TextureIntensitySize                       =  0x8061, // GL_TEXTURE_INTENSITY_SIZE_EXT
    TextureResident                            =  0x8067, // GL_TEXTURE_RESIDENT_EXT
    Texture1DBinding                           =  0x8068, // GL_TEXTURE_1D_BINDING_EXT
    Texture2DBinding                           =  0x8069, // GL_TEXTURE_2D_BINDING_EXT
    TextureDepth                               =  0x8071, // GL_TEXTURE_DEPTH
    VertexArray                                =  0x8074, // GL_VERTEX_ARRAY_EXT
    NormalArray                                =  0x8075, // GL_NORMAL_ARRAY_EXT
    ColorArray                                 =  0x8076, // GL_COLOR_ARRAY_EXT
    IndexArray                                 =  0x8077, // GL_INDEX_ARRAY_EXT
    TextureCoordArray                          =  0x8078, // GL_TEXTURE_COORD_ARRAY_EXT
    EdgeFlagArray                              =  0x8079, // GL_EDGE_FLAG_ARRAY_EXT
    VertexArraySize                            =  0x807A, // GL_VERTEX_ARRAY_SIZE_EXT
    VertexArrayType                            =  0x807B, // GL_VERTEX_ARRAY_TYPE_EXT
    VertexArrayStride                          =  0x807C, // GL_VERTEX_ARRAY_STRIDE_EXT
    NormalArrayType                            =  0x807E, // GL_NORMAL_ARRAY_TYPE_EXT
    NormalArrayStride                          =  0x807F, // GL_NORMAL_ARRAY_STRIDE_EXT
    ColorArraySize                             =  0x8081, // GL_COLOR_ARRAY_SIZE_EXT
    ColorArrayType                             =  0x8082, // GL_COLOR_ARRAY_TYPE_EXT
    ColorArrayStride                           =  0x8083, // GL_COLOR_ARRAY_STRIDE_EXT
    IndexArrayType                             =  0x8085, // GL_INDEX_ARRAY_TYPE_EXT
    IndexArrayStride                           =  0x8086, // GL_INDEX_ARRAY_STRIDE_EXT
    TextureCoordArraySize                      =  0x8088, // GL_TEXTURE_COORD_ARRAY_SIZE_EXT
    TextureCoordArrayType                      =  0x8089, // GL_TEXTURE_COORD_ARRAY_TYPE_EXT
    TextureCoordArrayStride                    =  0x808A, // GL_TEXTURE_COORD_ARRAY_STRIDE_EXT
    EdgeFlagArrayStride                        =  0x808C, // GL_EDGE_FLAG_ARRAY_STRIDE_EXT
    Multisample                                =  0x809D, // GL_MULTISAMPLE_ARB
    SampleAlphaToCoverage                      =  0x809E, // GL_SAMPLE_ALPHA_TO_COVERAGE_ARB
    SampleAlphaToOne                           =  0x809F, // GL_SAMPLE_ALPHA_TO_ONE_ARB
    SampleCoverage                             =  0x80A0, // GL_SAMPLE_COVERAGE_ARB
    SampleBuffers                              =  0x80A8, // GL_SAMPLE_BUFFERS_ARB
    Samples                                    =  0x80A9, // GL_SAMPLES_ARB
    SampleCoverageValue                        =  0x80AA, // GL_SAMPLE_COVERAGE_VALUE_ARB
    SampleCoverageInvert                       =  0x80AB, // GL_SAMPLE_COVERAGE_INVERT_ARB
    SamplePattern                              =  0x80AC, // GL_SAMPLE_PATTERN_EXT
    TextureCompareFailValue                    =  0x80BF, // GL_TEXTURE_COMPARE_FAIL_VALUE_ARB
    BlendDstRGB                                =  0x80C8, // GL_BLEND_DST_RGB_EXT
    BlendSrcRGB                                =  0x80C9, // GL_BLEND_SRC_RGB_EXT
    BlendDstAlpha                              =  0x80CA, // GL_BLEND_DST_ALPHA_EXT
    BlendSrcAlpha                              =  0x80CB, // GL_BLEND_SRC_ALPHA_EXT
    _422                                       =  0x80CC, // GL_422_EXT
    _422Rev                                    =  0x80CD, // GL_422_REV_EXT
    _422Average                                =  0x80CE, // GL_422_AVERAGE_EXT
    _422RevAverage                             =  0x80CF, // GL_422_REV_AVERAGE_EXT
    BGR                                        =  0x80E0, // GL_BGR_EXT
    BGRA                                       =  0x80E1, // GL_BGRA_EXT
    ColorIndex1                                =  0x80E2, // GL_COLOR_INDEX1_EXT
    ColorIndex2                                =  0x80E3, // GL_COLOR_INDEX2_EXT
    ColorIndex4                                =  0x80E4, // GL_COLOR_INDEX4_EXT
    ColorIndex8                                =  0x80E5, // GL_COLOR_INDEX8_EXT
    ColorIndex12                               =  0x80E6, // GL_COLOR_INDEX12_EXT
    ColorIndex16                               =  0x80E7, // GL_COLOR_INDEX16_EXT
    MaxElementsVertices                        =  0x80E8, // GL_MAX_ELEMENTS_VERTICES_EXT
    MaxElementsIndices                         =  0x80E9, // GL_MAX_ELEMENTS_INDICES_EXT
    PhongWin                                   =  0x80EA, // GL_PHONG_WIN
    FogSpecularTextureWin                      =  0x80EC, // GL_FOG_SPECULAR_TEXTURE_WIN
    TextureIndexSize                           =  0x80ED, // GL_TEXTURE_INDEX_SIZE_EXT
    ParameterBuffer                            =  0x80EE, // GL_PARAMETER_BUFFER_ARB
    ParameterBufferBinding                     =  0x80EF, // GL_PARAMETER_BUFFER_BINDING
    SpriteAxial                                =  0x814C, // GL_SPRITE_AXIAL_SGIX
    SpriteObjectAligned                        =  0x814D, // GL_SPRITE_OBJECT_ALIGNED_SGIX
    SpriteEyeAligned                           =  0x814E, // GL_SPRITE_EYE_ALIGNED_SGIX
    IgnoreBorder                               =  0x8150, // GL_IGNORE_BORDER_HP
    ConstantBorder                             =  0x8151, // GL_CONSTANT_BORDER_HP
    ReplicateBorder                            =  0x8153, // GL_REPLICATE_BORDER_HP
    ConvolutionBorderColor                     =  0x8154, // GL_CONVOLUTION_BORDER_COLOR_HP
    CubicHP                                    =  0x815F, // GL_CUBIC_HP
    AverageHP                                  =  0x8160, // GL_AVERAGE_HP
    PostImageTransformColorTable               =  0x8162, // GL_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP
    ProxyPostImageTransformColorTable          =  0x8163, // GL_PROXY_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP
    OcclusionTest                              =  0x8165, // GL_OCCLUSION_TEST_HP
    OcclusionTestResult                        =  0x8166, // GL_OCCLUSION_TEST_RESULT_HP
    TextureLightingMode                        =  0x8167, // GL_TEXTURE_LIGHTING_MODE_HP
    TexturePostSpecular                        =  0x8168, // GL_TEXTURE_POST_SPECULAR_HP
    TexturePreSpecular                         =  0x8169, // GL_TEXTURE_PRE_SPECULAR_HP
    MaxDeformationOrder                        =  0x8197, // GL_MAX_DEFORMATION_ORDER_SGIX
    ArrayElementLockFirst                      =  0x81A8, // GL_ARRAY_ELEMENT_LOCK_FIRST_EXT
    ArrayElementLockCount                      =  0x81A9, // GL_ARRAY_ELEMENT_LOCK_COUNT_EXT
    CullVertexEXT                              =  0x81AA, // GL_CULL_VERTEX_EXT
    IUIV2F                                     =  0x81AD, // GL_IUI_V2F_EXT
    IUIV3F                                     =  0x81AE, // GL_IUI_V3F_EXT
    IUIN3FV2F                                  =  0x81AF, // GL_IUI_N3F_V2F_EXT
    IUIN3FV3F                                  =  0x81B0, // GL_IUI_N3F_V3F_EXT
    T2FIUIV2F                                  =  0x81B1, // GL_T2F_IUI_V2F_EXT
    T2FIUIV3F                                  =  0x81B2, // GL_T2F_IUI_V3F_EXT
    T2FIUIN3FV2F                               =  0x81B3, // GL_T2F_IUI_N3F_V2F_EXT
    T2FIUIN3FV3F                               =  0x81B4, // GL_T2F_IUI_N3F_V3F_EXT
    IndexTest                                  =  0x81B5, // GL_INDEX_TEST_EXT
    IndexTestFunc                              =  0x81B6, // GL_INDEX_TEST_FUNC_EXT
    IndexTestRef                               =  0x81B7, // GL_INDEX_TEST_REF_EXT
    IndexMaterial                              =  0x81B8, // GL_INDEX_MATERIAL_EXT
    IndexMaterialParameter                     =  0x81B9, // GL_INDEX_MATERIAL_PARAMETER_EXT
    IndexMaterialFace                          =  0x81BA, // GL_INDEX_MATERIAL_FACE_EXT
    WrapBorder                                 =  0x81D4, // GL_WRAP_BORDER_SUN
    UnpackConstantData                         =  0x81D5, // GL_UNPACK_CONSTANT_DATA_SUNX
    TextureConstantData                        =  0x81D6, // GL_TEXTURE_CONSTANT_DATA_SUNX
    TriangleList                               =  0x81D7, // GL_TRIANGLE_LIST_SUN
    ReplacementCode                            =  0x81D8, // GL_REPLACEMENT_CODE_SUN
    GlobalAlpha                                =  0x81D9, // GL_GLOBAL_ALPHA_SUN
    GlobalAlphaFactor                          =  0x81DA, // GL_GLOBAL_ALPHA_FACTOR_SUN
    TextureColorWritemask                      =  0x81EF, // GL_TEXTURE_COLOR_WRITEMASK_SGIS
    FramebufferDefault                         =  0x8218, // GL_FRAMEBUFFER_DEFAULT
    PrimitiveRestartForPatchesSupported        =  0x8221, // GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED
    SyncClEvent                                =  0x8240, // GL_SYNC_CL_EVENT_ARB
    SyncClEventComplete                        =  0x8241, // GL_SYNC_CL_EVENT_COMPLETE_ARB
    DebugOutputSynchronous                     =  0x8242, // GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB
    DebugNextLoggedMessageLength               =  0x8243, // GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH
    DebugCallbackFunction                      =  0x8244, // GL_DEBUG_CALLBACK_FUNCTION_ARB
    DebugCallbackUserParam                     =  0x8245, // GL_DEBUG_CALLBACK_USER_PARAM_ARB
    DebugSourceAPI                             =  0x8246, // GL_DEBUG_SOURCE_API_ARB
    DebugSourceWindowSystem                    =  0x8247, // GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB
    DebugSourceShaderCompiler                  =  0x8248, // GL_DEBUG_SOURCE_SHADER_COMPILER_ARB
    DebugSourceThirdParty                      =  0x8249, // GL_DEBUG_SOURCE_THIRD_PARTY_ARB
    DebugSourceApplication                     =  0x824A, // GL_DEBUG_SOURCE_APPLICATION_ARB
    DebugSourceOther                           =  0x824B, // GL_DEBUG_SOURCE_OTHER_ARB
    DebugTypeError                             =  0x824C, // GL_DEBUG_TYPE_ERROR_ARB
    DebugTypeDeprecatedBehavior                =  0x824D, // GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
    DebugTypeUndefinedBehavior                 =  0x824E, // GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
    DebugTypePortability                       =  0x824F, // GL_DEBUG_TYPE_PORTABILITY_ARB
    DebugTypePerformance                       =  0x8250, // GL_DEBUG_TYPE_PERFORMANCE_ARB
    DebugTypeOther                             =  0x8251, // GL_DEBUG_TYPE_OTHER_ARB
    LoseContextOnReset                         =  0x8252, // GL_LOSE_CONTEXT_ON_RESET
    GuiltyContextReset                         =  0x8253, // GL_GUILTY_CONTEXT_RESET_ARB
    InnocentContextReset                       =  0x8254, // GL_INNOCENT_CONTEXT_RESET_ARB
    UnknownContextReset                        =  0x8255, // GL_UNKNOWN_CONTEXT_RESET_ARB
    ResetNotificationStrategy                  =  0x8256, // GL_RESET_NOTIFICATION_STRATEGY
    UndefinedVertex                            =  0x8260, // GL_UNDEFINED_VERTEX
    NoResetNotification                        =  0x8261, // GL_NO_RESET_NOTIFICATION
    MaxComputeSharedMemorySize                 =  0x8262, // GL_MAX_COMPUTE_SHARED_MEMORY_SIZE
    MaxCombinedDimensions                      =  0x8282, // GL_MAX_COMBINED_DIMENSIONS
    DepthComponents                            =  0x8284, // GL_DEPTH_COMPONENTS
    StencilComponents                          =  0x8285, // GL_STENCIL_COMPONENTS
    ManualGenerateMipmap                       =  0x8294, // GL_MANUAL_GENERATE_MIPMAP
    SRGBDecode                                 =  0x8299, // GL_SRGB_DECODE_ARB
    FullSupport                                =  0x82B7, // GL_FULL_SUPPORT
    CaveatSupport                              =  0x82B8, // GL_CAVEAT_SUPPORT
    ImageClass4x32                             =  0x82B9, // GL_IMAGE_CLASS_4_X_32
    ImageClass2x32                             =  0x82BA, // GL_IMAGE_CLASS_2_X_32
    ImageClass1x32                             =  0x82BB, // GL_IMAGE_CLASS_1_X_32
    ImageClass4x16                             =  0x82BC, // GL_IMAGE_CLASS_4_X_16
    ImageClass2x16                             =  0x82BD, // GL_IMAGE_CLASS_2_X_16
    ImageClass1x16                             =  0x82BE, // GL_IMAGE_CLASS_1_X_16
    ImageClass4x8                              =  0x82BF, // GL_IMAGE_CLASS_4_X_8
    ImageClass2x8                              =  0x82C0, // GL_IMAGE_CLASS_2_X_8
    ImageClass1x8                              =  0x82C1, // GL_IMAGE_CLASS_1_X_8
    ImageClass11_11_10                         =  0x82C2, // GL_IMAGE_CLASS_11_11_10
    ImageClass10_10_10_2                       =  0x82C3, // GL_IMAGE_CLASS_10_10_10_2
    ViewClass128Bits                           =  0x82C4, // GL_VIEW_CLASS_128_BITS
    ViewClass96Bits                            =  0x82C5, // GL_VIEW_CLASS_96_BITS
    ViewClass64Bits                            =  0x82C6, // GL_VIEW_CLASS_64_BITS
    ViewClass48Bits                            =  0x82C7, // GL_VIEW_CLASS_48_BITS
    ViewClass32Bits                            =  0x82C8, // GL_VIEW_CLASS_32_BITS
    ViewClass24Bits                            =  0x82C9, // GL_VIEW_CLASS_24_BITS
    ViewClass16Bits                            =  0x82CA, // GL_VIEW_CLASS_16_BITS
    ViewClass8Bits                             =  0x82CB, // GL_VIEW_CLASS_8_BITS
    ViewClassS3TcDXT1RGB                       =  0x82CC, // GL_VIEW_CLASS_S3TC_DXT1_RGB
    ViewClassS3TcDXT1RGBA                      =  0x82CD, // GL_VIEW_CLASS_S3TC_DXT1_RGBA
    ViewClassS3TcDXT3RGBA                      =  0x82CE, // GL_VIEW_CLASS_S3TC_DXT3_RGBA
    ViewClassS3TcDXT5RGBA                      =  0x82CF, // GL_VIEW_CLASS_S3TC_DXT5_RGBA
    ViewClassRGTC1Red                          =  0x82D0, // GL_VIEW_CLASS_RGTC1_RED
    ViewClassRGTC2Rg                           =  0x82D1, // GL_VIEW_CLASS_RGTC2_RG
    ViewClassBPTCUnorm                         =  0x82D2, // GL_VIEW_CLASS_BPTC_UNORM
    ViewClassBPTCFloat                         =  0x82D3, // GL_VIEW_CLASS_BPTC_FLOAT
    TextureViewMinLevel                        =  0x82DB, // GL_TEXTURE_VIEW_MIN_LEVEL
    TextureViewNumLevels                       =  0x82DC, // GL_TEXTURE_VIEW_NUM_LEVELS
    TextureViewMinLayer                        =  0x82DD, // GL_TEXTURE_VIEW_MIN_LAYER
    TextureViewNumLayers                       =  0x82DE, // GL_TEXTURE_VIEW_NUM_LAYERS
    TextureImmutableLevels                     =  0x82DF, // GL_TEXTURE_IMMUTABLE_LEVELS
    MaxVertexAttribStride                      =  0x82E5, // GL_MAX_VERTEX_ATTRIB_STRIDE
    NumShadingLanguageVersions                 =  0x82E9, // GL_NUM_SHADING_LANGUAGE_VERSIONS
    TransformFeedbackOverflow                  =  0x82EC, // GL_TRANSFORM_FEEDBACK_OVERFLOW_ARB
    TransformFeedbackStreamOverflow            =  0x82ED, // GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW
    VerticesSubmitted                          =  0x82EE, // GL_VERTICES_SUBMITTED_ARB
    PrimitivesSubmitted                        =  0x82EF, // GL_PRIMITIVES_SUBMITTED_ARB
    VertexShaderInvocations                    =  0x82F0, // GL_VERTEX_SHADER_INVOCATIONS_ARB
    TessControlShaderPatches                   =  0x82F1, // GL_TESS_CONTROL_SHADER_PATCHES
    TessEvaluationShaderInvocations            =  0x82F2, // GL_TESS_EVALUATION_SHADER_INVOCATIONS
    GeometryShaderPrimitivesEmitted            =  0x82F3, // GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED
    FragmentShaderInvocations                  =  0x82F4, // GL_FRAGMENT_SHADER_INVOCATIONS
    ComputeShaderInvocations                   =  0x82F5, // GL_COMPUTE_SHADER_INVOCATIONS
    ClippingInputPrimitives                    =  0x82F6, // GL_CLIPPING_INPUT_PRIMITIVES
    ClippingOutputPrimitives                   =  0x82F7, // GL_CLIPPING_OUTPUT_PRIMITIVES
    SparseBufferPageSize                       =  0x82F8, // GL_SPARSE_BUFFER_PAGE_SIZE_ARB
    MaxCullDistances                           =  0x82F9, // GL_MAX_CULL_DISTANCES
    MaxCombinedClipAndCullDistances            =  0x82FA, // GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES
    ContextReleaseBehavior                     =  0x82FB, // GL_CONTEXT_RELEASE_BEHAVIOR
    ContextReleaseBehaviorFlush                =  0x82FC, // GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH
    Ycrcb                                      =  0x8318, // GL_YCRCB_SGIX
    Ycrcba                                     =  0x8319, // GL_YCRCBA_SGIX
    CubicEXT                                   =  0x8334, // GL_CUBIC_EXT
    AverageEXT                                 =  0x8335, // GL_AVERAGE_EXT
    PixelTransform2DStackDepth                 =  0x8336, // GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT
    MaxPixelTransform2DStackDepth              =  0x8337, // GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT
    PixelTransform2DMatrix                     =  0x8338, // GL_PIXEL_TRANSFORM_2D_MATRIX_EXT
    TextureApplicationMode                     =  0x834F, // GL_TEXTURE_APPLICATION_MODE_EXT
    TextureLight                               =  0x8350, // GL_TEXTURE_LIGHT_EXT
    TextureMaterialFace                        =  0x8351, // GL_TEXTURE_MATERIAL_FACE_EXT
    TextureMaterialParameter                   =  0x8352, // GL_TEXTURE_MATERIAL_PARAMETER_EXT
    PixelGroupColor                            =  0x8356, // GL_PIXEL_GROUP_COLOR_SGIS
    UnsignedByte2_3_3Rev                       =  0x8362, // GL_UNSIGNED_BYTE_2_3_3_REV
    UnsignedShort5_6_5                         =  0x8363, // GL_UNSIGNED_SHORT_5_6_5
    UnsignedShort5_6_5Rev                      =  0x8364, // GL_UNSIGNED_SHORT_5_6_5_REV
    UnsignedShort4_4_4_4Rev                    =  0x8365, // GL_UNSIGNED_SHORT_4_4_4_4_REV
    UnsignedShort1_5_5_5Rev                    =  0x8366, // GL_UNSIGNED_SHORT_1_5_5_5_REV
    UnsignedInt8_8_8_8Rev                      =  0x8367, // GL_UNSIGNED_INT_8_8_8_8_REV
    MirroredRepeat                             =  0x8370, // GL_MIRRORED_REPEAT_ARB
    RGBS3Tc                                    =  0x83A0, // GL_RGB_S3TC
    RGB4S3Tc                                   =  0x83A1, // GL_RGB4_S3TC
    RGBAS3Tc                                   =  0x83A2, // GL_RGBA_S3TC
    RGBA4S3Tc                                  =  0x83A3, // GL_RGBA4_S3TC
    RGBADXT5S3Tc                               =  0x83A4, // GL_RGBA_DXT5_S3TC
    RGBA4DXT5S3Tc                              =  0x83A5, // GL_RGBA4_DXT5_S3TC
    ParallelArrays                             =  0x83F4, // GL_PARALLEL_ARRAYS_INTEL
    VertexArrayParallelPointers                =  0x83F5, // GL_VERTEX_ARRAY_PARALLEL_POINTERS_INTEL
    NormalArrayParallelPointers                =  0x83F6, // GL_NORMAL_ARRAY_PARALLEL_POINTERS_INTEL
    ColorArrayParallelPointers                 =  0x83F7, // GL_COLOR_ARRAY_PARALLEL_POINTERS_INTEL
    TextureCoordArrayParallelPointers          =  0x83F8, // GL_TEXTURE_COORD_ARRAY_PARALLEL_POINTERS_INTEL
    PerfqueryDonotFlush                        =  0x83F9, // GL_PERFQUERY_DONOT_FLUSH_INTEL
    PerfqueryFlush                             =  0x83FA, // GL_PERFQUERY_FLUSH_INTEL
    PerfqueryWait                              =  0x83FB, // GL_PERFQUERY_WAIT_INTEL
    BlackholeRender                            =  0x83FC, // GL_BLACKHOLE_RENDER_INTEL
    ConservativeRasterizationINTEL             =  0x83FE, // GL_CONSERVATIVE_RASTERIZATION_INTEL
    TextureMemoryLayout                        =  0x83FF, // GL_TEXTURE_MEMORY_LAYOUT_INTEL
    CurrentRasterNormal                        =  0x8406, // GL_CURRENT_RASTER_NORMAL_SGIX
    TangentArray                               =  0x8439, // GL_TANGENT_ARRAY_EXT
    BinormalArray                              =  0x843A, // GL_BINORMAL_ARRAY_EXT
    CurrentTangent                             =  0x843B, // GL_CURRENT_TANGENT_EXT
    CurrentBinormal                            =  0x843C, // GL_CURRENT_BINORMAL_EXT
    TangentArrayType                           =  0x843E, // GL_TANGENT_ARRAY_TYPE_EXT
    TangentArrayStride                         =  0x843F, // GL_TANGENT_ARRAY_STRIDE_EXT
    BinormalArrayType                          =  0x8440, // GL_BINORMAL_ARRAY_TYPE_EXT
    BinormalArrayStride                        =  0x8441, // GL_BINORMAL_ARRAY_STRIDE_EXT
    TangentArrayPointer                        =  0x8442, // GL_TANGENT_ARRAY_POINTER_EXT
    BinormalArrayPointer                       =  0x8443, // GL_BINORMAL_ARRAY_POINTER_EXT
    Map1Tangent                                =  0x8444, // GL_MAP1_TANGENT_EXT
    Map2Tangent                                =  0x8445, // GL_MAP2_TANGENT_EXT
    Map1Binormal                               =  0x8446, // GL_MAP1_BINORMAL_EXT
    Map2Binormal                               =  0x8447, // GL_MAP2_BINORMAL_EXT
    FogCoordinateSource                        =  0x8450, // GL_FOG_COORDINATE_SOURCE_EXT
    FogCoordinate                              =  0x8451, // GL_FOG_COORDINATE_EXT
    CurrentFogCoordinate                       =  0x8453, // GL_CURRENT_FOG_COORDINATE_EXT
    FogCoordinateArrayType                     =  0x8454, // GL_FOG_COORDINATE_ARRAY_TYPE_EXT
    FogCoordinateArrayStride                   =  0x8455, // GL_FOG_COORDINATE_ARRAY_STRIDE_EXT
    FogCoordinateArrayPointer                  =  0x8456, // GL_FOG_COORDINATE_ARRAY_POINTER_EXT
    FogCoordinateArray                         =  0x8457, // GL_FOG_COORDINATE_ARRAY_EXT
    ColorSum                                   =  0x8458, // GL_COLOR_SUM_ARB
    CurrentSecondaryColor                      =  0x8459, // GL_CURRENT_SECONDARY_COLOR_EXT
    SecondaryColorArraySize                    =  0x845A, // GL_SECONDARY_COLOR_ARRAY_SIZE_EXT
    SecondaryColorArrayType                    =  0x845B, // GL_SECONDARY_COLOR_ARRAY_TYPE_EXT
    SecondaryColorArrayStride                  =  0x845C, // GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT
    SecondaryColorArrayPointer                 =  0x845D, // GL_SECONDARY_COLOR_ARRAY_POINTER_EXT
    SecondaryColorArray                        =  0x845E, // GL_SECONDARY_COLOR_ARRAY_EXT
    ScreenCoordinates                          =  0x8490, // GL_SCREEN_COORDINATES_REND
    InvertedScreenW                            =  0x8491, // GL_INVERTED_SCREEN_W_REND
    Texture2                                   =  0x84C2, // GL_TEXTURE2_ARB
    Texture3                                   =  0x84C3, // GL_TEXTURE3_ARB
    Texture4                                   =  0x84C4, // GL_TEXTURE4_ARB
    Texture5                                   =  0x84C5, // GL_TEXTURE5_ARB
    Texture6                                   =  0x84C6, // GL_TEXTURE6_ARB
    Texture7                                   =  0x84C7, // GL_TEXTURE7_ARB
    Texture8                                   =  0x84C8, // GL_TEXTURE8_ARB
    Texture9                                   =  0x84C9, // GL_TEXTURE9_ARB
    Texture10                                  =  0x84CA, // GL_TEXTURE10_ARB
    Texture11                                  =  0x84CB, // GL_TEXTURE11_ARB
    Texture12                                  =  0x84CC, // GL_TEXTURE12_ARB
    Texture13                                  =  0x84CD, // GL_TEXTURE13_ARB
    Texture14                                  =  0x84CE, // GL_TEXTURE14_ARB
    Texture15                                  =  0x84CF, // GL_TEXTURE15_ARB
    Texture16                                  =  0x84D0, // GL_TEXTURE16_ARB
    Texture17                                  =  0x84D1, // GL_TEXTURE17_ARB
    Texture18                                  =  0x84D2, // GL_TEXTURE18_ARB
    Texture19                                  =  0x84D3, // GL_TEXTURE19_ARB
    Texture20                                  =  0x84D4, // GL_TEXTURE20_ARB
    Texture21                                  =  0x84D5, // GL_TEXTURE21_ARB
    Texture22                                  =  0x84D6, // GL_TEXTURE22_ARB
    Texture23                                  =  0x84D7, // GL_TEXTURE23_ARB
    Texture24                                  =  0x84D8, // GL_TEXTURE24_ARB
    Texture25                                  =  0x84D9, // GL_TEXTURE25_ARB
    Texture26                                  =  0x84DA, // GL_TEXTURE26_ARB
    Texture27                                  =  0x84DB, // GL_TEXTURE27_ARB
    Texture28                                  =  0x84DC, // GL_TEXTURE28_ARB
    Texture29                                  =  0x84DD, // GL_TEXTURE29_ARB
    Texture30                                  =  0x84DE, // GL_TEXTURE30_ARB
    Texture31                                  =  0x84DF, // GL_TEXTURE31_ARB
    ActiveTexture                              =  0x84E0, // GL_ACTIVE_TEXTURE_ARB
    ClientActiveTexture                        =  0x84E1, // GL_CLIENT_ACTIVE_TEXTURE_ARB
    MaxTextureUnits                            =  0x84E2, // GL_MAX_TEXTURE_UNITS_ARB
    PathTransposeModelviewMatrix               =  0x84E3, // GL_PATH_TRANSPOSE_MODELVIEW_MATRIX_NV
    PathTransposeProjectionMatrix              =  0x84E4, // GL_PATH_TRANSPOSE_PROJECTION_MATRIX_NV
    TransposeTextureMatrix                     =  0x84E5, // GL_TRANSPOSE_TEXTURE_MATRIX_ARB
    TransposeColorMatrix                       =  0x84E6, // GL_TRANSPOSE_COLOR_MATRIX_ARB
    Subtract                                   =  0x84E7, // GL_SUBTRACT_ARB
    MaxRenderbufferSize                        =  0x84E8, // GL_MAX_RENDERBUFFER_SIZE_EXT
    CompressedAlpha                            =  0x84E9, // GL_COMPRESSED_ALPHA_ARB
    CompressedLuminance                        =  0x84EA, // GL_COMPRESSED_LUMINANCE_ARB
    CompressedLuminanceAlpha                   =  0x84EB, // GL_COMPRESSED_LUMINANCE_ALPHA_ARB
    CompressedIntensity                        =  0x84EC, // GL_COMPRESSED_INTENSITY_ARB
    CompressedRGB                              =  0x84ED, // GL_COMPRESSED_RGB_ARB
    CompressedRGBA                             =  0x84EE, // GL_COMPRESSED_RGBA_ARB
    MaxRectangleTextureSize                    =  0x84F8, // GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB
    UnsignedInt24_8                            =  0x84FA, // GL_UNSIGNED_INT_24_8
    MaxTextureLODBias                          =  0x84FD, // GL_MAX_TEXTURE_LOD_BIAS_EXT
    TextureMaxAnisotropy                       =  0x84FE, // GL_TEXTURE_MAX_ANISOTROPY_EXT
    MaxTextureMaxAnisotropy                    =  0x84FF, // GL_MAX_TEXTURE_MAX_ANISOTROPY
    TextureFilterControl                       =  0x8500, // GL_TEXTURE_FILTER_CONTROL_EXT
    TextureLODBias                             =  0x8501, // GL_TEXTURE_LOD_BIAS_EXT
    Modelview1StackDepth                       =  0x8502, // GL_MODELVIEW1_STACK_DEPTH_EXT
    COMBINE4                                   =  0x8503, // GL_COMBINE4_NV
    MaxShininess                               =  0x8504, // GL_MAX_SHININESS_NV
    MaxSpotExponent                            =  0x8505, // GL_MAX_SPOT_EXPONENT_NV
    Modelview1Matrix                           =  0x8506, // GL_MODELVIEW1_MATRIX_EXT
    IncrWrap                                   =  0x8507, // GL_INCR_WRAP_EXT
    DecrWrap                                   =  0x8508, // GL_DECR_WRAP_EXT
    VertexWeighting                            =  0x8509, // GL_VERTEX_WEIGHTING_EXT
    Modelview1                                 =  0x850A, // GL_MODELVIEW1_ARB
    CurrentVertexWeight                        =  0x850B, // GL_CURRENT_VERTEX_WEIGHT_EXT
    VertexWeightArray                          =  0x850C, // GL_VERTEX_WEIGHT_ARRAY_EXT
    VertexWeightArraySize                      =  0x850D, // GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT
    VertexWeightArrayType                      =  0x850E, // GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT
    VertexWeightArrayStride                    =  0x850F, // GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT
    VertexWeightArrayPointer                   =  0x8510, // GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT
    NormalMap                                  =  0x8511, // GL_NORMAL_MAP_ARB
    ReflectionMap                              =  0x8512, // GL_REFLECTION_MAP_ARB
    TextureCubeMapPositiveX                    =  0x8515, // GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB
    TextureCubeMapNegativeX                    =  0x8516, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB
    TextureCubeMapPositiveY                    =  0x8517, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB
    TextureCubeMapNegativeY                    =  0x8518, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB
    TextureCubeMapPositiveZ                    =  0x8519, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB
    TextureCubeMapNegativeZ                    =  0x851A, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
    MaxCubeMapTextureSize                      =  0x851C, // GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB
    VertexArrayRange                           =  0x851D, // GL_VERTEX_ARRAY_RANGE_APPLE
    VertexArrayRangeLength                     =  0x851E, // GL_VERTEX_ARRAY_RANGE_LENGTH_APPLE
    VertexArrayRangeValid                      =  0x851F, // GL_VERTEX_ARRAY_RANGE_VALID_NV
    MaxVertexArrayRangeElement                 =  0x8520, // GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV
    VertexArrayRangePointer                    =  0x8521, // GL_VERTEX_ARRAY_RANGE_POINTER_APPLE
    RegisterCombinerS                          =  0x8522, // GL_REGISTER_COMBINERS_NV
    ConstantColor0                             =  0x852A, // GL_CONSTANT_COLOR0_NV
    ConstantColor1                             =  0x852B, // GL_CONSTANT_COLOR1_NV
    ETimesF                                    =  0x8531, // GL_E_TIMES_F_NV
    Spare0PlusSecondaryColor                   =  0x8532, // GL_SPARE0_PLUS_SECONDARY_COLOR_NV
    VertexArrayRangeWithoutFlush               =  0x8533, // GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV
    PerStageConstants                          =  0x8535, // GL_PER_STAGE_CONSTANTS_NV
    CombinerAbDotProduct                       =  0x8545, // GL_COMBINER_AB_DOT_PRODUCT_NV
    CombinerCDDotProduct                       =  0x8546, // GL_COMBINER_CD_DOT_PRODUCT_NV
    CombinerMuxSum                             =  0x8547, // GL_COMBINER_MUX_SUM_NV
    CombinerScale                              =  0x8548, // GL_COMBINER_SCALE_NV
    CombinerBias                               =  0x8549, // GL_COMBINER_BIAS_NV
    CombinerAbOutput                           =  0x854A, // GL_COMBINER_AB_OUTPUT_NV
    CombinerCDOutput                           =  0x854B, // GL_COMBINER_CD_OUTPUT_NV
    CombinerSumOutput                          =  0x854C, // GL_COMBINER_SUM_OUTPUT_NV
    MaxGeneralCombinerS                        =  0x854D, // GL_MAX_GENERAL_COMBINERS_NV
    NumGeneralCombinerS                        =  0x854E, // GL_NUM_GENERAL_COMBINERS_NV
    ColorSumClamp                              =  0x854F, // GL_COLOR_SUM_CLAMP_NV
    PrimitiveRestart                           =  0x8558, // GL_PRIMITIVE_RESTART_NV
    PrimitiveRestartIndex                      =  0x8559, // GL_PRIMITIVE_RESTART_INDEX_NV
    FogDistanceMode                            =  0x855A, // GL_FOG_DISTANCE_MODE_NV
    EyeRadial                                  =  0x855B, // GL_EYE_RADIAL_NV
    EyePlaneAbsolute                           =  0x855C, // GL_EYE_PLANE_ABSOLUTE_NV
    EmbossLight                                =  0x855D, // GL_EMBOSS_LIGHT_NV
    EmbossConstant                             =  0x855E, // GL_EMBOSS_CONSTANT_NV
    EmbossMap                                  =  0x855F, // GL_EMBOSS_MAP_NV
    RedMinClampIngr                            =  0x8560, // GL_RED_MIN_CLAMP_INGR
    GreenMinClampIngr                          =  0x8561, // GL_GREEN_MIN_CLAMP_INGR
    BlueMinClampIngr                           =  0x8562, // GL_BLUE_MIN_CLAMP_INGR
    AlphaMinClampIngr                          =  0x8563, // GL_ALPHA_MIN_CLAMP_INGR
    RedMaxClampIngr                            =  0x8564, // GL_RED_MAX_CLAMP_INGR
    GreenMaxClampIngr                          =  0x8565, // GL_GREEN_MAX_CLAMP_INGR
    BlueMaxClampIngr                           =  0x8566, // GL_BLUE_MAX_CLAMP_INGR
    AlphaMaxClampIngr                          =  0x8567, // GL_ALPHA_MAX_CLAMP_INGR
    InterlaceReadIngr                          =  0x8568, // GL_INTERLACE_READ_INGR
    TextureNormal                              =  0x85AF, // GL_TEXTURE_NORMAL_EXT
    LightModelSpecularVector                   =  0x85B0, // GL_LIGHT_MODEL_SPECULAR_VECTOR_APPLE
    UnpackClientStorage                        =  0x85B2, // GL_UNPACK_CLIENT_STORAGE_APPLE
    BufferObjectAPPLE                          =  0x85B3, // GL_BUFFER_OBJECT_APPLE
    VertexArrayBinding                         =  0x85B5, // GL_VERTEX_ARRAY_BINDING_APPLE
    TextureRangeLength                         =  0x85B7, // GL_TEXTURE_RANGE_LENGTH_APPLE
    TextureRangePointer                        =  0x85B8, // GL_TEXTURE_RANGE_POINTER_APPLE
    Ycbcr422                                   =  0x85B9, // GL_YCBCR_422_APPLE
    UnsignedShort8_8                           =  0x85BA, // GL_UNSIGNED_SHORT_8_8_APPLE
    UnsignedShort8_8Rev                        =  0x85BB, // GL_UNSIGNED_SHORT_8_8_REV_APPLE
    StoragePrivate                             =  0x85BD, // GL_STORAGE_PRIVATE_APPLE
    ReplacementCodeArray                       =  0x85C0, // GL_REPLACEMENT_CODE_ARRAY_SUN
    ReplacementCodeArrayType                   =  0x85C1, // GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN
    ReplacementCodeArrayStride                 =  0x85C2, // GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN
    ReplacementCodeArrayPointer                =  0x85C3, // GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN
    R1UIV3F                                    =  0x85C4, // GL_R1UI_V3F_SUN
    R1UIC4UBV3F                                =  0x85C5, // GL_R1UI_C4UB_V3F_SUN
    R1UIC3FV3F                                 =  0x85C6, // GL_R1UI_C3F_V3F_SUN
    R1UIN3FV3F                                 =  0x85C7, // GL_R1UI_N3F_V3F_SUN
    R1UIC4FN3FV3F                              =  0x85C8, // GL_R1UI_C4F_N3F_V3F_SUN
    R1UIT2FV3F                                 =  0x85C9, // GL_R1UI_T2F_V3F_SUN
    R1UIT2FN3FV3F                              =  0x85CA, // GL_R1UI_T2F_N3F_V3F_SUN
    R1UIT2FC4FN3FV3F                           =  0x85CB, // GL_R1UI_T2F_C4F_N3F_V3F_SUN
    SliceAccum                                 =  0x85CC, // GL_SLICE_ACCUM_SUN
    QuadMesh                                   =  0x8614, // GL_QUAD_MESH_SUN
    TriangleMesh                               =  0x8615, // GL_TRIANGLE_MESH_SUN
    VertexProgram                              =  0x8620, // GL_VERTEX_PROGRAM_NV
    VertexStateProgram                         =  0x8621, // GL_VERTEX_STATE_PROGRAM_NV
    VertexAttribArrayEnabled                   =  0x8622, // GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB
    AttribArraySize                            =  0x8623, // GL_ATTRIB_ARRAY_SIZE_NV
    AttribArrayStride                          =  0x8624, // GL_ATTRIB_ARRAY_STRIDE_NV
    AttribArrayType                            =  0x8625, // GL_ATTRIB_ARRAY_TYPE_NV
    CurrentAttrib                              =  0x8626, // GL_CURRENT_ATTRIB_NV
    ProgramLength                              =  0x8627, // GL_PROGRAM_LENGTH_ARB
    ProgramString                              =  0x8628, // GL_PROGRAM_STRING_NV
    ModelviewProjection                        =  0x8629, // GL_MODELVIEW_PROJECTION_NV
    Identity                                   =  0x862A, // GL_IDENTITY_NV
    Inverse                                    =  0x862B, // GL_INVERSE_NV
    Transpose                                  =  0x862C, // GL_TRANSPOSE_NV
    InverseTranspose                           =  0x862D, // GL_INVERSE_TRANSPOSE_NV
    MaxProgramMatrixStackDepth                 =  0x862E, // GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB
    MaxProgramMatrices                         =  0x862F, // GL_MAX_PROGRAM_MATRICES_ARB
    Matrix0NV                                  =  0x8630, // GL_MATRIX0_NV
    Matrix1NV                                  =  0x8631, // GL_MATRIX1_NV
    Matrix2NV                                  =  0x8632, // GL_MATRIX2_NV
    Matrix3NV                                  =  0x8633, // GL_MATRIX3_NV
    Matrix4NV                                  =  0x8634, // GL_MATRIX4_NV
    Matrix5NV                                  =  0x8635, // GL_MATRIX5_NV
    Matrix6NV                                  =  0x8636, // GL_MATRIX6_NV
    Matrix7NV                                  =  0x8637, // GL_MATRIX7_NV
    CurrentMatrixStackDepth                    =  0x8640, // GL_CURRENT_MATRIX_STACK_DEPTH_ARB
    CurrentMatrix                              =  0x8641, // GL_CURRENT_MATRIX_ARB
    ProgramPointSize                           =  0x8642, // GL_PROGRAM_POINT_SIZE_ARB
    VertexProgramTwoSide                       =  0x8643, // GL_VERTEX_PROGRAM_TWO_SIDE_ARB
    AttribArrayPointer                         =  0x8645, // GL_ATTRIB_ARRAY_POINTER_NV
    ProgramTarget                              =  0x8646, // GL_PROGRAM_TARGET_NV
    ProgramResident                            =  0x8647, // GL_PROGRAM_RESIDENT_NV
    TrackMatrix                                =  0x8648, // GL_TRACK_MATRIX_NV
    TrackMatrixTransform                       =  0x8649, // GL_TRACK_MATRIX_TRANSFORM_NV
    VertexProgramBinding                       =  0x864A, // GL_VERTEX_PROGRAM_BINDING_NV
    ProgramErrorPosition                       =  0x864B, // GL_PROGRAM_ERROR_POSITION_ARB
    OffsetTextureRectangle                     =  0x864C, // GL_OFFSET_TEXTURE_RECTANGLE_NV
    OffsetTextureRectangleScale                =  0x864D, // GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV
    DotProductTextureRectangle                 =  0x864E, // GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV
    DepthClamp                                 =  0x864F, // GL_DEPTH_CLAMP_NV
    VertexAttribArray0                         =  0x8650, // GL_VERTEX_ATTRIB_ARRAY0_NV
    VertexAttribArray1                         =  0x8651, // GL_VERTEX_ATTRIB_ARRAY1_NV
    VertexAttribArray2                         =  0x8652, // GL_VERTEX_ATTRIB_ARRAY2_NV
    VertexAttribArray3                         =  0x8653, // GL_VERTEX_ATTRIB_ARRAY3_NV
    VertexAttribArray4                         =  0x8654, // GL_VERTEX_ATTRIB_ARRAY4_NV
    VertexAttribArray5                         =  0x8655, // GL_VERTEX_ATTRIB_ARRAY5_NV
    VertexAttribArray6                         =  0x8656, // GL_VERTEX_ATTRIB_ARRAY6_NV
    VertexAttribArray7                         =  0x8657, // GL_VERTEX_ATTRIB_ARRAY7_NV
    VertexAttribArray8                         =  0x8658, // GL_VERTEX_ATTRIB_ARRAY8_NV
    VertexAttribArray9                         =  0x8659, // GL_VERTEX_ATTRIB_ARRAY9_NV
    VertexAttribArray10                        =  0x865A, // GL_VERTEX_ATTRIB_ARRAY10_NV
    VertexAttribArray11                        =  0x865B, // GL_VERTEX_ATTRIB_ARRAY11_NV
    VertexAttribArray12                        =  0x865C, // GL_VERTEX_ATTRIB_ARRAY12_NV
    VertexAttribArray13                        =  0x865D, // GL_VERTEX_ATTRIB_ARRAY13_NV
    VertexAttribArray14                        =  0x865E, // GL_VERTEX_ATTRIB_ARRAY14_NV
    VertexAttribArray15                        =  0x865F, // GL_VERTEX_ATTRIB_ARRAY15_NV
    Map1VertexAttrib0_4                        =  0x8660, // GL_MAP1_VERTEX_ATTRIB0_4_NV
    Map1VertexAttrib1_4                        =  0x8661, // GL_MAP1_VERTEX_ATTRIB1_4_NV
    Map1VertexAttrib2_4                        =  0x8662, // GL_MAP1_VERTEX_ATTRIB2_4_NV
    Map1VertexAttrib3_4                        =  0x8663, // GL_MAP1_VERTEX_ATTRIB3_4_NV
    Map1VertexAttrib4_4                        =  0x8664, // GL_MAP1_VERTEX_ATTRIB4_4_NV
    Map1VertexAttrib5_4                        =  0x8665, // GL_MAP1_VERTEX_ATTRIB5_4_NV
    Map1VertexAttrib6_4                        =  0x8666, // GL_MAP1_VERTEX_ATTRIB6_4_NV
    Map1VertexAttrib7_4                        =  0x8667, // GL_MAP1_VERTEX_ATTRIB7_4_NV
    Map1VertexAttrib8_4                        =  0x8668, // GL_MAP1_VERTEX_ATTRIB8_4_NV
    Map1VertexAttrib9_4                        =  0x8669, // GL_MAP1_VERTEX_ATTRIB9_4_NV
    Map1VertexAttrib10_4                       =  0x866A, // GL_MAP1_VERTEX_ATTRIB10_4_NV
    Map1VertexAttrib11_4                       =  0x866B, // GL_MAP1_VERTEX_ATTRIB11_4_NV
    Map1VertexAttrib12_4                       =  0x866C, // GL_MAP1_VERTEX_ATTRIB12_4_NV
    Map1VertexAttrib13_4                       =  0x866D, // GL_MAP1_VERTEX_ATTRIB13_4_NV
    Map1VertexAttrib14_4                       =  0x866E, // GL_MAP1_VERTEX_ATTRIB14_4_NV
    Map1VertexAttrib15_4                       =  0x866F, // GL_MAP1_VERTEX_ATTRIB15_4_NV
    Map2VertexAttrib0_4                        =  0x8670, // GL_MAP2_VERTEX_ATTRIB0_4_NV
    Map2VertexAttrib1_4                        =  0x8671, // GL_MAP2_VERTEX_ATTRIB1_4_NV
    Map2VertexAttrib2_4                        =  0x8672, // GL_MAP2_VERTEX_ATTRIB2_4_NV
    Map2VertexAttrib3_4                        =  0x8673, // GL_MAP2_VERTEX_ATTRIB3_4_NV
    Map2VertexAttrib4_4                        =  0x8674, // GL_MAP2_VERTEX_ATTRIB4_4_NV
    Map2VertexAttrib5_4                        =  0x8675, // GL_MAP2_VERTEX_ATTRIB5_4_NV
    Map2VertexAttrib6_4                        =  0x8676, // GL_MAP2_VERTEX_ATTRIB6_4_NV
    Map2VertexAttrib7_4                        =  0x8677, // GL_MAP2_VERTEX_ATTRIB7_4_NV
    Map2VertexAttrib8_4                        =  0x8678, // GL_MAP2_VERTEX_ATTRIB8_4_NV
    Map2VertexAttrib9_4                        =  0x8679, // GL_MAP2_VERTEX_ATTRIB9_4_NV
    Map2VertexAttrib10_4                       =  0x867A, // GL_MAP2_VERTEX_ATTRIB10_4_NV
    Map2VertexAttrib11_4                       =  0x867B, // GL_MAP2_VERTEX_ATTRIB11_4_NV
    Map2VertexAttrib12_4                       =  0x867C, // GL_MAP2_VERTEX_ATTRIB12_4_NV
    Map2VertexAttrib13_4                       =  0x867D, // GL_MAP2_VERTEX_ATTRIB13_4_NV
    Map2VertexAttrib14_4                       =  0x867E, // GL_MAP2_VERTEX_ATTRIB14_4_NV
    Map2VertexAttrib15_4                       =  0x867F, // GL_MAP2_VERTEX_ATTRIB15_4_NV
    TextureCompressedImageSize                 =  0x86A0, // GL_TEXTURE_COMPRESSED_IMAGE_SIZE
    TextureCompressed                          =  0x86A1, // GL_TEXTURE_COMPRESSED_ARB
    NumCompressedTextureFormats                =  0x86A2, // GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB
    CompressedTextureFormats                   =  0x86A3, // GL_COMPRESSED_TEXTURE_FORMATS_ARB
    MaxVertexUnits                             =  0x86A4, // GL_MAX_VERTEX_UNITS_ARB
    ActiveVertexUnits                          =  0x86A5, // GL_ACTIVE_VERTEX_UNITS_ARB
    WeightSumUnity                             =  0x86A6, // GL_WEIGHT_SUM_UNITY_ARB
    VertexBlend                                =  0x86A7, // GL_VERTEX_BLEND_ARB
    CurrentWeight                              =  0x86A8, // GL_CURRENT_WEIGHT_ARB
    WeightArrayType                            =  0x86A9, // GL_WEIGHT_ARRAY_TYPE_ARB
    WeightArrayStride                          =  0x86AA, // GL_WEIGHT_ARRAY_STRIDE_ARB
    WeightArraySize                            =  0x86AB, // GL_WEIGHT_ARRAY_SIZE_ARB
    WeightArrayPointer                         =  0x86AC, // GL_WEIGHT_ARRAY_POINTER_ARB
    WeightArray                                =  0x86AD, // GL_WEIGHT_ARRAY_ARB
    Dot3RGBARB                                 =  0x86AE, // GL_DOT3_RGB_ARB
    Dot3RGBAARB                                =  0x86AF, // GL_DOT3_RGBA_ARB
    CompressedRGBFXT13Dfx                      =  0x86B0, // GL_COMPRESSED_RGB_FXT1_3DFX
    CompressedRGBAFXT13Dfx                     =  0x86B1, // GL_COMPRESSED_RGBA_FXT1_3DFX
    Multisample3Dfx                            =  0x86B2, // GL_MULTISAMPLE_3DFX
    SampleBuffers3Dfx                          =  0x86B3, // GL_SAMPLE_BUFFERS_3DFX
    Samples3Dfx                                =  0x86B4, // GL_SAMPLES_3DFX
    EvalFractionalTessellation                 =  0x86C5, // GL_EVAL_FRACTIONAL_TESSELLATION_NV
    EvalVertexAttrib0                          =  0x86C6, // GL_EVAL_VERTEX_ATTRIB0_NV
    EvalVertexAttrib1                          =  0x86C7, // GL_EVAL_VERTEX_ATTRIB1_NV
    EvalVertexAttrib2                          =  0x86C8, // GL_EVAL_VERTEX_ATTRIB2_NV
    EvalVertexAttrib3                          =  0x86C9, // GL_EVAL_VERTEX_ATTRIB3_NV
    EvalVertexAttrib4                          =  0x86CA, // GL_EVAL_VERTEX_ATTRIB4_NV
    EvalVertexAttrib5                          =  0x86CB, // GL_EVAL_VERTEX_ATTRIB5_NV
    EvalVertexAttrib6                          =  0x86CC, // GL_EVAL_VERTEX_ATTRIB6_NV
    EvalVertexAttrib7                          =  0x86CD, // GL_EVAL_VERTEX_ATTRIB7_NV
    EvalVertexAttrib8                          =  0x86CE, // GL_EVAL_VERTEX_ATTRIB8_NV
    EvalVertexAttrib9                          =  0x86CF, // GL_EVAL_VERTEX_ATTRIB9_NV
    EvalVertexAttrib10                         =  0x86D0, // GL_EVAL_VERTEX_ATTRIB10_NV
    EvalVertexAttrib11                         =  0x86D1, // GL_EVAL_VERTEX_ATTRIB11_NV
    EvalVertexAttrib12                         =  0x86D2, // GL_EVAL_VERTEX_ATTRIB12_NV
    EvalVertexAttrib13                         =  0x86D3, // GL_EVAL_VERTEX_ATTRIB13_NV
    EvalVertexAttrib14                         =  0x86D4, // GL_EVAL_VERTEX_ATTRIB14_NV
    EvalVertexAttrib15                         =  0x86D5, // GL_EVAL_VERTEX_ATTRIB15_NV
    MaxMapTessellation                         =  0x86D6, // GL_MAX_MAP_TESSELLATION_NV
    MaxRationalEvalOrder                       =  0x86D7, // GL_MAX_RATIONAL_EVAL_ORDER_NV
    MaxProgramPatchAttribs                     =  0x86D8, // GL_MAX_PROGRAM_PATCH_ATTRIBS_NV
    RGBAUnsignedDotProductMapping              =  0x86D9, // GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV
    UnsignedIntS8S88_8                         =  0x86DA, // GL_UNSIGNED_INT_S8_S8_8_8_NV
    UnsignedInt8_8S8S8Rev                      =  0x86DB, // GL_UNSIGNED_INT_8_8_S8_S8_REV_NV
    DsdtMagIntensity                           =  0x86DC, // GL_DSDT_MAG_INTENSITY_NV
    ShaderConsistent                           =  0x86DD, // GL_SHADER_CONSISTENT_NV
    TextureShader                              =  0x86DE, // GL_TEXTURE_SHADER_NV
    ShaderOperation                            =  0x86DF, // GL_SHADER_OPERATION_NV
    CullModes                                  =  0x86E0, // GL_CULL_MODES_NV
    OffsetTexture2DMatrix                      =  0x86E1, // GL_OFFSET_TEXTURE_2D_MATRIX_NV
    OffsetTexture2DScale                       =  0x86E2, // GL_OFFSET_TEXTURE_2D_SCALE_NV
    OffsetTexture2DBias                        =  0x86E3, // GL_OFFSET_TEXTURE_2D_BIAS_NV
    PreviousTextureInput                       =  0x86E4, // GL_PREVIOUS_TEXTURE_INPUT_NV
    ConstEye                                   =  0x86E5, // GL_CONST_EYE_NV
    PassThrough                                =  0x86E6, // GL_PASS_THROUGH_NV
    CullFragment                               =  0x86E7, // GL_CULL_FRAGMENT_NV
    OffsetTexture2D                            =  0x86E8, // GL_OFFSET_TEXTURE_2D_NV
    DependentArTexture2D                       =  0x86E9, // GL_DEPENDENT_AR_TEXTURE_2D_NV
    DependentGbTexture2D                       =  0x86EA, // GL_DEPENDENT_GB_TEXTURE_2D_NV
    SurfaceState                               =  0x86EB, // GL_SURFACE_STATE_NV
    DotProduct                                 =  0x86EC, // GL_DOT_PRODUCT_NV
    DotProductDepthReplace                     =  0x86ED, // GL_DOT_PRODUCT_DEPTH_REPLACE_NV
    DotProductTexture2D                        =  0x86EE, // GL_DOT_PRODUCT_TEXTURE_2D_NV
    DotProductTexture3D                        =  0x86EF, // GL_DOT_PRODUCT_TEXTURE_3D_NV
    DotProductTextureCubeMap                   =  0x86F0, // GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV
    DotProductDiffuseCubeMap                   =  0x86F1, // GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV
    DotProductReflectCubeMap                   =  0x86F2, // GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV
    DotProductConstEyeReflectCubeMap           =  0x86F3, // GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV
    Hilo                                       =  0x86F4, // GL_HILO_NV
    Dsdt                                       =  0x86F5, // GL_DSDT_NV
    DsdtMag                                    =  0x86F6, // GL_DSDT_MAG_NV
    DsdtMagVib                                 =  0x86F7, // GL_DSDT_MAG_VIB_NV
    HILO16                                     =  0x86F8, // GL_HILO16_NV
    SignedHilo                                 =  0x86F9, // GL_SIGNED_HILO_NV
    SignedHILO16                               =  0x86FA, // GL_SIGNED_HILO16_NV
    SignedRGBA                                 =  0x86FB, // GL_SIGNED_RGBA_NV
    SignedRGBA8                                =  0x86FC, // GL_SIGNED_RGBA8_NV
    SurfaceRegistered                          =  0x86FD, // GL_SURFACE_REGISTERED_NV
    SignedRGB                                  =  0x86FE, // GL_SIGNED_RGB_NV
    SignedRGB8                                 =  0x86FF, // GL_SIGNED_RGB8_NV
    SurfaceMapped                              =  0x8700, // GL_SURFACE_MAPPED_NV
    SignedLuminance                            =  0x8701, // GL_SIGNED_LUMINANCE_NV
    SignedLuminance8                           =  0x8702, // GL_SIGNED_LUMINANCE8_NV
    SignedLuminanceAlpha                       =  0x8703, // GL_SIGNED_LUMINANCE_ALPHA_NV
    SignedLuminance8Alpha8                     =  0x8704, // GL_SIGNED_LUMINANCE8_ALPHA8_NV
    SignedAlpha                                =  0x8705, // GL_SIGNED_ALPHA_NV
    SignedAlpha8                               =  0x8706, // GL_SIGNED_ALPHA8_NV
    SignedIntensity                            =  0x8707, // GL_SIGNED_INTENSITY_NV
    SignedIntensity8                           =  0x8708, // GL_SIGNED_INTENSITY8_NV
    DSDT8                                      =  0x8709, // GL_DSDT8_NV
    DSDT8MAG8                                  =  0x870A, // GL_DSDT8_MAG8_NV
    DSDT8MAG8Intensity8                        =  0x870B, // GL_DSDT8_MAG8_INTENSITY8_NV
    SignedRGBUnsignedAlpha                     =  0x870C, // GL_SIGNED_RGB_UNSIGNED_ALPHA_NV
    SignedRGB8UnsignedAlpha8                   =  0x870D, // GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV
    HIScale                                    =  0x870E, // GL_HI_SCALE_NV
    LoScale                                    =  0x870F, // GL_LO_SCALE_NV
    DsScale                                    =  0x8710, // GL_DS_SCALE_NV
    DtScale                                    =  0x8711, // GL_DT_SCALE_NV
    MagnitudeScale                             =  0x8712, // GL_MAGNITUDE_SCALE_NV
    VibranceScale                              =  0x8713, // GL_VIBRANCE_SCALE_NV
    HIBias                                     =  0x8714, // GL_HI_BIAS_NV
    LoBias                                     =  0x8715, // GL_LO_BIAS_NV
    DsBias                                     =  0x8716, // GL_DS_BIAS_NV
    DtBias                                     =  0x8717, // GL_DT_BIAS_NV
    MagnitudeBias                              =  0x8718, // GL_MAGNITUDE_BIAS_NV
    VibranceBias                               =  0x8719, // GL_VIBRANCE_BIAS_NV
    TextureBorderValues                        =  0x871A, // GL_TEXTURE_BORDER_VALUES_NV
    TextureHISize                              =  0x871B, // GL_TEXTURE_HI_SIZE_NV
    TextureLoSize                              =  0x871C, // GL_TEXTURE_LO_SIZE_NV
    TextureDsSize                              =  0x871D, // GL_TEXTURE_DS_SIZE_NV
    TextureDtSize                              =  0x871E, // GL_TEXTURE_DT_SIZE_NV
    TextureMagSize                             =  0x871F, // GL_TEXTURE_MAG_SIZE_NV
    Modelview2                                 =  0x8722, // GL_MODELVIEW2_ARB
    Modelview3                                 =  0x8723, // GL_MODELVIEW3_ARB
    Modelview4                                 =  0x8724, // GL_MODELVIEW4_ARB
    Modelview5                                 =  0x8725, // GL_MODELVIEW5_ARB
    Modelview6                                 =  0x8726, // GL_MODELVIEW6_ARB
    Modelview7                                 =  0x8727, // GL_MODELVIEW7_ARB
    Modelview8                                 =  0x8728, // GL_MODELVIEW8_ARB
    Modelview9                                 =  0x8729, // GL_MODELVIEW9_ARB
    Modelview10                                =  0x872A, // GL_MODELVIEW10_ARB
    Modelview11                                =  0x872B, // GL_MODELVIEW11_ARB
    Modelview12                                =  0x872C, // GL_MODELVIEW12_ARB
    Modelview13                                =  0x872D, // GL_MODELVIEW13_ARB
    Modelview14                                =  0x872E, // GL_MODELVIEW14_ARB
    Modelview15                                =  0x872F, // GL_MODELVIEW15_ARB
    Modelview16                                =  0x8730, // GL_MODELVIEW16_ARB
    Modelview17                                =  0x8731, // GL_MODELVIEW17_ARB
    Modelview18                                =  0x8732, // GL_MODELVIEW18_ARB
    Modelview19                                =  0x8733, // GL_MODELVIEW19_ARB
    Modelview20                                =  0x8734, // GL_MODELVIEW20_ARB
    Modelview21                                =  0x8735, // GL_MODELVIEW21_ARB
    Modelview22                                =  0x8736, // GL_MODELVIEW22_ARB
    Modelview23                                =  0x8737, // GL_MODELVIEW23_ARB
    Modelview24                                =  0x8738, // GL_MODELVIEW24_ARB
    Modelview25                                =  0x8739, // GL_MODELVIEW25_ARB
    Modelview26                                =  0x873A, // GL_MODELVIEW26_ARB
    Modelview27                                =  0x873B, // GL_MODELVIEW27_ARB
    Modelview28                                =  0x873C, // GL_MODELVIEW28_ARB
    Modelview29                                =  0x873D, // GL_MODELVIEW29_ARB
    Modelview30                                =  0x873E, // GL_MODELVIEW30_ARB
    Modelview31                                =  0x873F, // GL_MODELVIEW31_ARB
    Dot3RGBEXT                                 =  0x8740, // GL_DOT3_RGB_EXT
    Dot3RGBAEXT                                =  0x8741, // GL_DOT3_RGBA_EXT
    MirrorClamp                                =  0x8742, // GL_MIRROR_CLAMP_ATI
    MirrorClampToEdge                          =  0x8743, // GL_MIRROR_CLAMP_TO_EDGE
    ModulateAdd                                =  0x8744, // GL_MODULATE_ADD_ATI
    ModulateSignedAdd                          =  0x8745, // GL_MODULATE_SIGNED_ADD_ATI
    ModulateSubtract                           =  0x8746, // GL_MODULATE_SUBTRACT_ATI
    Set                                        =  0x874A, // GL_SET_AMD
    ReplaceValue                               =  0x874B, // GL_REPLACE_VALUE_AMD
    StencilOpValue                             =  0x874C, // GL_STENCIL_OP_VALUE_AMD
    StencilBackOpValue                         =  0x874D, // GL_STENCIL_BACK_OP_VALUE_AMD
    OcclusionQueryEventMask                    =  0x874F, // GL_OCCLUSION_QUERY_EVENT_MASK_AMD
    Ycbcr                                      =  0x8757, // GL_YCBCR_MESA
    PackInvert                                 =  0x8758, // GL_PACK_INVERT_MESA
    Texture1DStack                             =  0x8759, // GL_TEXTURE_1D_STACK_MESAX
    Texture2DStack                             =  0x875A, // GL_TEXTURE_2D_STACK_MESAX
    ProxyTexture1DStack                        =  0x875B, // GL_PROXY_TEXTURE_1D_STACK_MESAX
    ProxyTexture2DStack                        =  0x875C, // GL_PROXY_TEXTURE_2D_STACK_MESAX
    Texture1DStackBinding                      =  0x875D, // GL_TEXTURE_1D_STACK_BINDING_MESAX
    Texture2DStackBinding                      =  0x875E, // GL_TEXTURE_2D_STACK_BINDING_MESAX
    ProgramBinaryFormat                        =  0x875F, // GL_PROGRAM_BINARY_FORMAT_MESA
    ArrayObjectBuffer                          =  0x8766, // GL_ARRAY_OBJECT_BUFFER_ATI
    ArrayObjectOffset                          =  0x8767, // GL_ARRAY_OBJECT_OFFSET_ATI
    ElementArrayATI                            =  0x8768, // GL_ELEMENT_ARRAY_ATI
    ElementArrayTypeATI                        =  0x8769, // GL_ELEMENT_ARRAY_TYPE_ATI
    ElementArrayPointerATI                     =  0x876A, // GL_ELEMENT_ARRAY_POINTER_ATI
    MaxVertexStreamsATI                        =  0x876B, // GL_MAX_VERTEX_STREAMS_ATI
    VertexSource                               =  0x8774, // GL_VERTEX_SOURCE_ATI
    Dudv                                       =  0x8779, // GL_DUDV_ATI
    DU8DV8                                     =  0x877A, // GL_DU8DV8_ATI
    BumpEnvmap                                 =  0x877B, // GL_BUMP_ENVMAP_ATI
    BumpTarget                                 =  0x877C, // GL_BUMP_TARGET_ATI
    VertexShader                               =  0x8780, // GL_VERTEX_SHADER_EXT
    VertexShaderBinding                        =  0x8781, // GL_VERTEX_SHADER_BINDING_EXT
    OutputVertex                               =  0x879A, // GL_OUTPUT_VERTEX_EXT
    OutputColor0                               =  0x879B, // GL_OUTPUT_COLOR0_EXT
    OutputColor1                               =  0x879C, // GL_OUTPUT_COLOR1_EXT
    OutputTextureCoord0                        =  0x879D, // GL_OUTPUT_TEXTURE_COORD0_EXT
    OutputTextureCoord1                        =  0x879E, // GL_OUTPUT_TEXTURE_COORD1_EXT
    OutputTextureCoord2                        =  0x879F, // GL_OUTPUT_TEXTURE_COORD2_EXT
    OutputTextureCoord3                        =  0x87A0, // GL_OUTPUT_TEXTURE_COORD3_EXT
    OutputTextureCoord4                        =  0x87A1, // GL_OUTPUT_TEXTURE_COORD4_EXT
    OutputTextureCoord5                        =  0x87A2, // GL_OUTPUT_TEXTURE_COORD5_EXT
    OutputTextureCoord6                        =  0x87A3, // GL_OUTPUT_TEXTURE_COORD6_EXT
    OutputTextureCoord7                        =  0x87A4, // GL_OUTPUT_TEXTURE_COORD7_EXT
    OutputTextureCoord8                        =  0x87A5, // GL_OUTPUT_TEXTURE_COORD8_EXT
    OutputTextureCoord9                        =  0x87A6, // GL_OUTPUT_TEXTURE_COORD9_EXT
    OutputTextureCoord10                       =  0x87A7, // GL_OUTPUT_TEXTURE_COORD10_EXT
    OutputTextureCoord11                       =  0x87A8, // GL_OUTPUT_TEXTURE_COORD11_EXT
    OutputTextureCoord12                       =  0x87A9, // GL_OUTPUT_TEXTURE_COORD12_EXT
    OutputTextureCoord13                       =  0x87AA, // GL_OUTPUT_TEXTURE_COORD13_EXT
    OutputTextureCoord14                       =  0x87AB, // GL_OUTPUT_TEXTURE_COORD14_EXT
    OutputTextureCoord15                       =  0x87AC, // GL_OUTPUT_TEXTURE_COORD15_EXT
    OutputTextureCoord16                       =  0x87AD, // GL_OUTPUT_TEXTURE_COORD16_EXT
    OutputTextureCoord17                       =  0x87AE, // GL_OUTPUT_TEXTURE_COORD17_EXT
    OutputTextureCoord18                       =  0x87AF, // GL_OUTPUT_TEXTURE_COORD18_EXT
    OutputTextureCoord19                       =  0x87B0, // GL_OUTPUT_TEXTURE_COORD19_EXT
    OutputTextureCoord20                       =  0x87B1, // GL_OUTPUT_TEXTURE_COORD20_EXT
    OutputTextureCoord21                       =  0x87B2, // GL_OUTPUT_TEXTURE_COORD21_EXT
    OutputTextureCoord22                       =  0x87B3, // GL_OUTPUT_TEXTURE_COORD22_EXT
    OutputTextureCoord23                       =  0x87B4, // GL_OUTPUT_TEXTURE_COORD23_EXT
    OutputTextureCoord24                       =  0x87B5, // GL_OUTPUT_TEXTURE_COORD24_EXT
    OutputTextureCoord25                       =  0x87B6, // GL_OUTPUT_TEXTURE_COORD25_EXT
    OutputTextureCoord26                       =  0x87B7, // GL_OUTPUT_TEXTURE_COORD26_EXT
    OutputTextureCoord27                       =  0x87B8, // GL_OUTPUT_TEXTURE_COORD27_EXT
    OutputTextureCoord28                       =  0x87B9, // GL_OUTPUT_TEXTURE_COORD28_EXT
    OutputTextureCoord29                       =  0x87BA, // GL_OUTPUT_TEXTURE_COORD29_EXT
    OutputTextureCoord30                       =  0x87BB, // GL_OUTPUT_TEXTURE_COORD30_EXT
    OutputTextureCoord31                       =  0x87BC, // GL_OUTPUT_TEXTURE_COORD31_EXT
    OutputFog                                  =  0x87BD, // GL_OUTPUT_FOG_EXT
    MaxVertexShaderInstructions                =  0x87C5, // GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT
    MaxVertexShaderVariants                    =  0x87C6, // GL_MAX_VERTEX_SHADER_VARIANTS_EXT
    MaxVertexShaderInvariants                  =  0x87C7, // GL_MAX_VERTEX_SHADER_INVARIANTS_EXT
    MaxVertexShaderLocalConstants              =  0x87C8, // GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT
    MaxVertexShaderLocals                      =  0x87C9, // GL_MAX_VERTEX_SHADER_LOCALS_EXT
    MaxOptimizedVertexShaderInstructions       =  0x87CA, // GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT
    MaxOptimizedVertexShaderVariants           =  0x87CB, // GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT
    MaxOptimizedVertexShaderLocalConstants     =  0x87CC, // GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT
    MaxOptimizedVertexShaderInvariants         =  0x87CD, // GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT
    MaxOptimizedVertexShaderLocals             =  0x87CE, // GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT
    VertexShaderInstructions                   =  0x87CF, // GL_VERTEX_SHADER_INSTRUCTIONS_EXT
    VertexShaderVariants                       =  0x87D0, // GL_VERTEX_SHADER_VARIANTS_EXT
    VertexShaderInvariants                     =  0x87D1, // GL_VERTEX_SHADER_INVARIANTS_EXT
    VertexShaderLocalConstants                 =  0x87D2, // GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT
    VertexShaderLocals                         =  0x87D3, // GL_VERTEX_SHADER_LOCALS_EXT
    VertexShaderOptimized                      =  0x87D4, // GL_VERTEX_SHADER_OPTIMIZED_EXT
    VariantArrayPointer                        =  0x87E9, // GL_VARIANT_ARRAY_POINTER_EXT
    InvariantValue                             =  0x87EA, // GL_INVARIANT_VALUE_EXT
    InvariantDatatype                          =  0x87EB, // GL_INVARIANT_DATATYPE_EXT
    LocalConstantValue                         =  0x87EC, // GL_LOCAL_CONSTANT_VALUE_EXT
    LocalConstantDatatype                      =  0x87ED, // GL_LOCAL_CONSTANT_DATATYPE_EXT
    PnTriangles                                =  0x87F0, // GL_PN_TRIANGLES_ATI
    MaxPnTrianglesTesselationLevel             =  0x87F1, // GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI
    PnTrianglesPointModeLinear                 =  0x87F5, // GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI
    PnTrianglesPointModeCubic                  =  0x87F6, // GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI
    PnTrianglesNormalModeLinear                =  0x87F7, // GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI
    PnTrianglesNormalModeQuadratic             =  0x87F8, // GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI
    VboFreeMemory                              =  0x87FB, // GL_VBO_FREE_MEMORY_ATI
    TextureFreeMemory                          =  0x87FC, // GL_TEXTURE_FREE_MEMORY_ATI
    RenderbufferFreeMemory                     =  0x87FD, // GL_RENDERBUFFER_FREE_MEMORY_ATI
    StencilBackFunc                            =  0x8800, // GL_STENCIL_BACK_FUNC_ATI
    StencilBackFail                            =  0x8801, // GL_STENCIL_BACK_FAIL_ATI
    StencilBackPassDepthFail                   =  0x8802, // GL_STENCIL_BACK_PASS_DEPTH_FAIL_ATI
    StencilBackPassDepthPass                   =  0x8803, // GL_STENCIL_BACK_PASS_DEPTH_PASS_ATI
    ProgramAluInstructions                     =  0x8805, // GL_PROGRAM_ALU_INSTRUCTIONS_ARB
    ProgramTexInstructions                     =  0x8806, // GL_PROGRAM_TEX_INSTRUCTIONS_ARB
    ProgramTexIndirections                     =  0x8807, // GL_PROGRAM_TEX_INDIRECTIONS_ARB
    ProgramNativeAluInstructions               =  0x8808, // GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB
    ProgramNativeTexInstructions               =  0x8809, // GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB
    ProgramNativeTexIndirections               =  0x880A, // GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB
    MaxProgramAluInstructions                  =  0x880B, // GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB
    MaxProgramTexInstructions                  =  0x880C, // GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB
    MaxProgramTexIndirections                  =  0x880D, // GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB
    MaxProgramNativeAluInstructions            =  0x880E, // GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB
    MaxProgramNativeTexInstructions            =  0x880F, // GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB
    MaxProgramNativeTexIndirections            =  0x8810, // GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB
    RGBAFloat32                                =  0x8814, // GL_RGBA_FLOAT32_APPLE
    RGBFloat32                                 =  0x8815, // GL_RGB_FLOAT32_APPLE
    AlphaFloat32                               =  0x8816, // GL_ALPHA_FLOAT32_APPLE
    IntensityFloat32                           =  0x8817, // GL_INTENSITY_FLOAT32_APPLE
    LuminanceFloat32                           =  0x8818, // GL_LUMINANCE_FLOAT32_APPLE
    LuminanceAlphaFloat32                      =  0x8819, // GL_LUMINANCE_ALPHA_FLOAT32_APPLE
    RGBAFloat16                                =  0x881A, // GL_RGBA_FLOAT16_APPLE
    RGBFloat16                                 =  0x881B, // GL_RGB_FLOAT16_APPLE
    AlphaFloat16                               =  0x881C, // GL_ALPHA_FLOAT16_APPLE
    IntensityFloat16                           =  0x881D, // GL_INTENSITY_FLOAT16_APPLE
    LuminanceFloat16                           =  0x881E, // GL_LUMINANCE_FLOAT16_APPLE
    LuminanceAlphaFloat16                      =  0x881F, // GL_LUMINANCE_ALPHA_FLOAT16_APPLE
    RGBAFloatMode                              =  0x8820, // GL_RGBA_FLOAT_MODE_ARB
    MaxDrawBuffers                             =  0x8824, // GL_MAX_DRAW_BUFFERS_ARB
    DrawBuffer0                                =  0x8825, // GL_DRAW_BUFFER0
    DrawBuffer1                                =  0x8826, // GL_DRAW_BUFFER1
    DrawBuffer2                                =  0x8827, // GL_DRAW_BUFFER2
    DrawBuffer3                                =  0x8828, // GL_DRAW_BUFFER3
    DrawBuffer4                                =  0x8829, // GL_DRAW_BUFFER4
    DrawBuffer5                                =  0x882A, // GL_DRAW_BUFFER5
    DrawBuffer6                                =  0x882B, // GL_DRAW_BUFFER6
    DrawBuffer7                                =  0x882C, // GL_DRAW_BUFFER7
    DrawBuffer8                                =  0x882D, // GL_DRAW_BUFFER8
    DrawBuffer9                                =  0x882E, // GL_DRAW_BUFFER9
    DrawBuffer10                               =  0x882F, // GL_DRAW_BUFFER10
    DrawBuffer11                               =  0x8830, // GL_DRAW_BUFFER11
    DrawBuffer12                               =  0x8831, // GL_DRAW_BUFFER12
    DrawBuffer13                               =  0x8832, // GL_DRAW_BUFFER13
    DrawBuffer14                               =  0x8833, // GL_DRAW_BUFFER14
    DrawBuffer15                               =  0x8834, // GL_DRAW_BUFFER15
    ColorClearUnclampedValue                   =  0x8835, // GL_COLOR_CLEAR_UNCLAMPED_VALUE_ATI
    BlendEquationAlpha                         =  0x883D, // GL_BLEND_EQUATION_ALPHA_EXT
    SubsampleDistance                          =  0x883F, // GL_SUBSAMPLE_DISTANCE_AMD
    MatrixPalette                              =  0x8840, // GL_MATRIX_PALETTE_ARB
    MaxMatrixPaletteStackDepth                 =  0x8841, // GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB
    MaxPaletteMatrices                         =  0x8842, // GL_MAX_PALETTE_MATRICES_ARB
    CurrentPaletteMatrix                       =  0x8843, // GL_CURRENT_PALETTE_MATRIX_ARB
    MatrixIndexArray                           =  0x8844, // GL_MATRIX_INDEX_ARRAY_ARB
    CurrentMatrixIndex                         =  0x8845, // GL_CURRENT_MATRIX_INDEX_ARB
    MatrixIndexArraySize                       =  0x8846, // GL_MATRIX_INDEX_ARRAY_SIZE_ARB
    MatrixIndexArrayType                       =  0x8847, // GL_MATRIX_INDEX_ARRAY_TYPE_ARB
    MatrixIndexArrayStride                     =  0x8848, // GL_MATRIX_INDEX_ARRAY_STRIDE_ARB
    MatrixIndexArrayPointer                    =  0x8849, // GL_MATRIX_INDEX_ARRAY_POINTER_ARB
    TextureDepthSize                           =  0x884A, // GL_TEXTURE_DEPTH_SIZE
    DepthTextureMode                           =  0x884B, // GL_DEPTH_TEXTURE_MODE_ARB
    TextureCompareMode                         =  0x884C, // GL_TEXTURE_COMPARE_MODE_ARB
    TextureCompareFunc                         =  0x884D, // GL_TEXTURE_COMPARE_FUNC_ARB
    CompareRToTexture                          =  0x884E, // GL_COMPARE_R_TO_TEXTURE_ARB
    OffsetProjectiveTexture2D                  =  0x8850, // GL_OFFSET_PROJECTIVE_TEXTURE_2D_NV
    OffsetProjectiveTexture2DScale             =  0x8851, // GL_OFFSET_PROJECTIVE_TEXTURE_2D_SCALE_NV
    OffsetProjectiveTextureRectangle           =  0x8852, // GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_NV
    OffsetProjectiveTextureRectangleScale      =  0x8853, // GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_SCALE_NV
    OffsetHiloTexture2D                        =  0x8854, // GL_OFFSET_HILO_TEXTURE_2D_NV
    OffsetHiloTextureRectangle                 =  0x8855, // GL_OFFSET_HILO_TEXTURE_RECTANGLE_NV
    OffsetHiloProjectiveTexture2D              =  0x8856, // GL_OFFSET_HILO_PROJECTIVE_TEXTURE_2D_NV
    OffsetHiloProjectiveTextureRectangle       =  0x8857, // GL_OFFSET_HILO_PROJECTIVE_TEXTURE_RECTANGLE_NV
    DependentHiloTexture2D                     =  0x8858, // GL_DEPENDENT_HILO_TEXTURE_2D_NV
    DependentRGBTexture3D                      =  0x8859, // GL_DEPENDENT_RGB_TEXTURE_3D_NV
    DependentRGBTextureCubeMap                 =  0x885A, // GL_DEPENDENT_RGB_TEXTURE_CUBE_MAP_NV
    DotProductPassThrough                      =  0x885B, // GL_DOT_PRODUCT_PASS_THROUGH_NV
    DotProductTexture1D                        =  0x885C, // GL_DOT_PRODUCT_TEXTURE_1D_NV
    DotProductAffineDepthReplace               =  0x885D, // GL_DOT_PRODUCT_AFFINE_DEPTH_REPLACE_NV
    HILO8                                      =  0x885E, // GL_HILO8_NV
    SignedHILO8                                =  0x885F, // GL_SIGNED_HILO8_NV
    ForceBlueToOne                             =  0x8860, // GL_FORCE_BLUE_TO_ONE_NV
    PointSprite                                =  0x8861, // GL_POINT_SPRITE_ARB
    CoordReplace                               =  0x8862, // GL_COORD_REPLACE_ARB
    PointSpriteRMode                           =  0x8863, // GL_POINT_SPRITE_R_MODE_NV
    PixelCounterBits                           =  0x8864, // GL_PIXEL_COUNTER_BITS_NV
    CurrentOcclusionQueryID                    =  0x8865, // GL_CURRENT_OCCLUSION_QUERY_ID_NV
    QueryResult                                =  0x8866, // GL_QUERY_RESULT_ARB
    QueryResultAvailable                       =  0x8867, // GL_QUERY_RESULT_AVAILABLE_ARB
    MaxFragmentProgramLocalParameters          =  0x8868, // GL_MAX_FRAGMENT_PROGRAM_LOCAL_PARAMETERS_NV
    MaxVertexAttribs                           =  0x8869, // GL_MAX_VERTEX_ATTRIBS_ARB
    VertexAttribArrayNormalized                =  0x886A, // GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB
    MaxTessControlInputComponents              =  0x886C, // GL_MAX_TESS_CONTROL_INPUT_COMPONENTS
    MaxTessEvaluationInputComponents           =  0x886D, // GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS
    DepthStencilToRGBA                         =  0x886E, // GL_DEPTH_STENCIL_TO_RGBA_NV
    DepthStencilToBGRA                         =  0x886F, // GL_DEPTH_STENCIL_TO_BGRA_NV
    FragmentProgram                            =  0x8870, // GL_FRAGMENT_PROGRAM_NV
    MaxTextureCoords                           =  0x8871, // GL_MAX_TEXTURE_COORDS_ARB
    MaxTextureImageUnits                       =  0x8872, // GL_MAX_TEXTURE_IMAGE_UNITS_ARB
    FragmentProgramBinding                     =  0x8873, // GL_FRAGMENT_PROGRAM_BINDING_NV
    ProgramErrorString                         =  0x8874, // GL_PROGRAM_ERROR_STRING_ARB
    ProgramFormat                              =  0x8876, // GL_PROGRAM_FORMAT_ARB
    WritePixelDataRangeLength                  =  0x887A, // GL_WRITE_PIXEL_DATA_RANGE_LENGTH_NV
    ReadPixelDataRangeLength                   =  0x887B, // GL_READ_PIXEL_DATA_RANGE_LENGTH_NV
    WritePixelDataRangePointer                 =  0x887C, // GL_WRITE_PIXEL_DATA_RANGE_POINTER_NV
    ReadPixelDataRangePointer                  =  0x887D, // GL_READ_PIXEL_DATA_RANGE_POINTER_NV
    GeometryShaderInvocations                  =  0x887F, // GL_GEOMETRY_SHADER_INVOCATIONS
    FloatR                                     =  0x8880, // GL_FLOAT_R_NV
    FloatRg                                    =  0x8881, // GL_FLOAT_RG_NV
    FloatRGB                                   =  0x8882, // GL_FLOAT_RGB_NV
    FloatRGBA                                  =  0x8883, // GL_FLOAT_RGBA_NV
    FloatR16                                   =  0x8884, // GL_FLOAT_R16_NV
    FloatR32                                   =  0x8885, // GL_FLOAT_R32_NV
    FloatRG16                                  =  0x8886, // GL_FLOAT_RG16_NV
    FloatRG32                                  =  0x8887, // GL_FLOAT_RG32_NV
    FloatRGB16                                 =  0x8888, // GL_FLOAT_RGB16_NV
    FloatRGB32                                 =  0x8889, // GL_FLOAT_RGB32_NV
    FloatRGBA16                                =  0x888A, // GL_FLOAT_RGBA16_NV
    FloatRGBA32                                =  0x888B, // GL_FLOAT_RGBA32_NV
    TextureFloatComponents                     =  0x888C, // GL_TEXTURE_FLOAT_COMPONENTS_NV
    FloatClearColorValue                       =  0x888D, // GL_FLOAT_CLEAR_COLOR_VALUE_NV
    FloatRGBAMode                              =  0x888E, // GL_FLOAT_RGBA_MODE_NV
    TextureUnsignedRemapMode                   =  0x888F, // GL_TEXTURE_UNSIGNED_REMAP_MODE_NV
    DepthBoundsTest                            =  0x8890, // GL_DEPTH_BOUNDS_TEST_EXT
    DepthBounds                                =  0x8891, // GL_DEPTH_BOUNDS_EXT
    ArrayBuffer                                =  0x8892, // GL_ARRAY_BUFFER_ARB
    ElementArrayBuffer                         =  0x8893, // GL_ELEMENT_ARRAY_BUFFER_ARB
    ArrayBufferBinding                         =  0x8894, // GL_ARRAY_BUFFER_BINDING_ARB
    ElementArrayBufferBinding                  =  0x8895, // GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB
    VertexArrayBufferBinding                   =  0x8896, // GL_VERTEX_ARRAY_BUFFER_BINDING_ARB
    NormalArrayBufferBinding                   =  0x8897, // GL_NORMAL_ARRAY_BUFFER_BINDING_ARB
    ColorArrayBufferBinding                    =  0x8898, // GL_COLOR_ARRAY_BUFFER_BINDING_ARB
    IndexArrayBufferBinding                    =  0x8899, // GL_INDEX_ARRAY_BUFFER_BINDING_ARB
    TextureCoordArrayBufferBinding             =  0x889A, // GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB
    EdgeFlagArrayBufferBinding                 =  0x889B, // GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB
    SecondaryColorArrayBufferBinding           =  0x889C, // GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB
    FogCoordinateArrayBufferBinding            =  0x889D, // GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB
    WeightArrayBufferBinding                   =  0x889E, // GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB
    VertexAttribArrayBufferBinding             =  0x889F, // GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB
    ProgramInstructions                        =  0x88A0, // GL_PROGRAM_INSTRUCTIONS_ARB
    MaxProgramInstructions                     =  0x88A1, // GL_MAX_PROGRAM_INSTRUCTIONS_ARB
    ProgramNativeInstructions                  =  0x88A2, // GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB
    MaxProgramNativeInstructions               =  0x88A3, // GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB
    ProgramTemporaries                         =  0x88A4, // GL_PROGRAM_TEMPORARIES_ARB
    MaxProgramTemporaries                      =  0x88A5, // GL_MAX_PROGRAM_TEMPORARIES_ARB
    ProgramNativeTemporaries                   =  0x88A6, // GL_PROGRAM_NATIVE_TEMPORARIES_ARB
    MaxProgramNativeTemporaries                =  0x88A7, // GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB
    ProgramParameters                          =  0x88A8, // GL_PROGRAM_PARAMETERS_ARB
    MaxProgramParameters                       =  0x88A9, // GL_MAX_PROGRAM_PARAMETERS_ARB
    ProgramNativeParameters                    =  0x88AA, // GL_PROGRAM_NATIVE_PARAMETERS_ARB
    MaxProgramNativeParameters                 =  0x88AB, // GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB
    ProgramAttribs                             =  0x88AC, // GL_PROGRAM_ATTRIBS_ARB
    MaxProgramAttribs                          =  0x88AD, // GL_MAX_PROGRAM_ATTRIBS_ARB
    ProgramNativeAttribs                       =  0x88AE, // GL_PROGRAM_NATIVE_ATTRIBS_ARB
    MaxProgramNativeAttribs                    =  0x88AF, // GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB
    ProgramAddressRegisters                    =  0x88B0, // GL_PROGRAM_ADDRESS_REGISTERS_ARB
    MaxProgramAddressRegisters                 =  0x88B1, // GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB
    ProgramNativeAddressRegisters              =  0x88B2, // GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB
    MaxProgramNativeAddressRegisters           =  0x88B3, // GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB
    MaxProgramLocalParameters                  =  0x88B4, // GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB
    MaxProgramEnvParameters                    =  0x88B5, // GL_MAX_PROGRAM_ENV_PARAMETERS_ARB
    ProgramUnderNativeLimitS                   =  0x88B6, // GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB
    TransposeCurrentMatrix                     =  0x88B7, // GL_TRANSPOSE_CURRENT_MATRIX_ARB
    ReadOnly                                   =  0x88B8, // GL_READ_ONLY_ARB
    WriteOnly                                  =  0x88B9, // GL_WRITE_ONLY_ARB
    ReadWrite                                  =  0x88BA, // GL_READ_WRITE_ARB
    WriteDiscard                               =  0x88BE, // GL_WRITE_DISCARD_NV
    TimeElapsed                                =  0x88BF, // GL_TIME_ELAPSED_EXT
    Matrix0ARB                                 =  0x88C0, // GL_MATRIX0_ARB
    Matrix1ARB                                 =  0x88C1, // GL_MATRIX1_ARB
    Matrix2ARB                                 =  0x88C2, // GL_MATRIX2_ARB
    Matrix3ARB                                 =  0x88C3, // GL_MATRIX3_ARB
    Matrix4ARB                                 =  0x88C4, // GL_MATRIX4_ARB
    Matrix5ARB                                 =  0x88C5, // GL_MATRIX5_ARB
    Matrix6ARB                                 =  0x88C6, // GL_MATRIX6_ARB
    Matrix7ARB                                 =  0x88C7, // GL_MATRIX7_ARB
    Matrix8                                    =  0x88C8, // GL_MATRIX8_ARB
    Matrix9                                    =  0x88C9, // GL_MATRIX9_ARB
    Matrix10                                   =  0x88CA, // GL_MATRIX10_ARB
    Matrix11                                   =  0x88CB, // GL_MATRIX11_ARB
    Matrix12                                   =  0x88CC, // GL_MATRIX12_ARB
    Matrix13                                   =  0x88CD, // GL_MATRIX13_ARB
    Matrix14                                   =  0x88CE, // GL_MATRIX14_ARB
    Matrix15                                   =  0x88CF, // GL_MATRIX15_ARB
    Matrix16                                   =  0x88D0, // GL_MATRIX16_ARB
    Matrix17                                   =  0x88D1, // GL_MATRIX17_ARB
    Matrix18                                   =  0x88D2, // GL_MATRIX18_ARB
    Matrix19                                   =  0x88D3, // GL_MATRIX19_ARB
    Matrix20                                   =  0x88D4, // GL_MATRIX20_ARB
    Matrix21                                   =  0x88D5, // GL_MATRIX21_ARB
    Matrix22                                   =  0x88D6, // GL_MATRIX22_ARB
    Matrix23                                   =  0x88D7, // GL_MATRIX23_ARB
    Matrix24                                   =  0x88D8, // GL_MATRIX24_ARB
    Matrix25                                   =  0x88D9, // GL_MATRIX25_ARB
    Matrix26                                   =  0x88DA, // GL_MATRIX26_ARB
    Matrix27                                   =  0x88DB, // GL_MATRIX27_ARB
    Matrix28                                   =  0x88DC, // GL_MATRIX28_ARB
    Matrix29                                   =  0x88DD, // GL_MATRIX29_ARB
    Matrix30                                   =  0x88DE, // GL_MATRIX30_ARB
    Matrix31                                   =  0x88DF, // GL_MATRIX31_ARB
    StreamDraw                                 =  0x88E0, // GL_STREAM_DRAW_ARB
    StreamRead                                 =  0x88E1, // GL_STREAM_READ_ARB
    StreamCopy                                 =  0x88E2, // GL_STREAM_COPY_ARB
    StaticDraw                                 =  0x88E4, // GL_STATIC_DRAW_ARB
    StaticRead                                 =  0x88E5, // GL_STATIC_READ_ARB
    StaticCopy                                 =  0x88E6, // GL_STATIC_COPY_ARB
    DynamicDraw                                =  0x88E8, // GL_DYNAMIC_DRAW_ARB
    DynamicRead                                =  0x88E9, // GL_DYNAMIC_READ_ARB
    DynamicCopy                                =  0x88EA, // GL_DYNAMIC_COPY_ARB
    PixelPackBuffer                            =  0x88EB, // GL_PIXEL_PACK_BUFFER_ARB
    PixelUnpackBuffer                          =  0x88EC, // GL_PIXEL_UNPACK_BUFFER_ARB
    PixelPackBufferBinding                     =  0x88ED, // GL_PIXEL_PACK_BUFFER_BINDING_ARB
    PixelUnpackBufferBinding                   =  0x88EF, // GL_PIXEL_UNPACK_BUFFER_BINDING_ARB
    TextureStencilSize                         =  0x88F1, // GL_TEXTURE_STENCIL_SIZE
    StencilTagBits                             =  0x88F2, // GL_STENCIL_TAG_BITS_EXT
    StencilClearTagValue                       =  0x88F3, // GL_STENCIL_CLEAR_TAG_VALUE_EXT
    MaxProgramExecInstructions                 =  0x88F4, // GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV
    MaxProgramCallDepth                        =  0x88F5, // GL_MAX_PROGRAM_CALL_DEPTH_NV
    MaxProgramIFDepth                          =  0x88F6, // GL_MAX_PROGRAM_IF_DEPTH_NV
    MaxProgramLoopDepth                        =  0x88F7, // GL_MAX_PROGRAM_LOOP_DEPTH_NV
    MaxProgramLoopCount                        =  0x88F8, // GL_MAX_PROGRAM_LOOP_COUNT_NV
    VertexAttribArrayInteger                   =  0x88FD, // GL_VERTEX_ATTRIB_ARRAY_INTEGER_NV
    VertexAttribArrayDivisor                   =  0x88FE, // GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ARB
    MaxArrayTextureLayers                      =  0x88FF, // GL_MAX_ARRAY_TEXTURE_LAYERS_EXT
    MinProgramTexelOffset                      =  0x8904, // GL_MIN_PROGRAM_TEXEL_OFFSET_EXT
    MaxProgramTexelOffset                      =  0x8905, // GL_MAX_PROGRAM_TEXEL_OFFSET_EXT
    ProgramAttribComponents                    =  0x8906, // GL_PROGRAM_ATTRIB_COMPONENTS_NV
    ProgramResultComponents                    =  0x8907, // GL_PROGRAM_RESULT_COMPONENTS_NV
    MaxProgramAttribComponents                 =  0x8908, // GL_MAX_PROGRAM_ATTRIB_COMPONENTS_NV
    MaxProgramResultComponents                 =  0x8909, // GL_MAX_PROGRAM_RESULT_COMPONENTS_NV
    StencilTestTwoSide                         =  0x8910, // GL_STENCIL_TEST_TWO_SIDE_EXT
    ActiveStencilFace                          =  0x8911, // GL_ACTIVE_STENCIL_FACE_EXT
    MirrorClampToBorder                        =  0x8912, // GL_MIRROR_CLAMP_TO_BORDER_EXT
    SamplesPassed                              =  0x8914, // GL_SAMPLES_PASSED_ARB
    FragmentShader                             =  0x8920, // GL_FRAGMENT_SHADER_ATI
    Reg0                                       =  0x8921, // GL_REG_0_ATI
    Reg1                                       =  0x8922, // GL_REG_1_ATI
    Reg2                                       =  0x8923, // GL_REG_2_ATI
    Reg3                                       =  0x8924, // GL_REG_3_ATI
    Reg4                                       =  0x8925, // GL_REG_4_ATI
    Reg5                                       =  0x8926, // GL_REG_5_ATI
    Reg6                                       =  0x8927, // GL_REG_6_ATI
    Reg7                                       =  0x8928, // GL_REG_7_ATI
    Reg8                                       =  0x8929, // GL_REG_8_ATI
    Reg9                                       =  0x892A, // GL_REG_9_ATI
    Reg10                                      =  0x892B, // GL_REG_10_ATI
    Reg11                                      =  0x892C, // GL_REG_11_ATI
    Reg12                                      =  0x892D, // GL_REG_12_ATI
    Reg13                                      =  0x892E, // GL_REG_13_ATI
    Reg14                                      =  0x892F, // GL_REG_14_ATI
    Reg15                                      =  0x8930, // GL_REG_15_ATI
    Reg16                                      =  0x8931, // GL_REG_16_ATI
    Reg17                                      =  0x8932, // GL_REG_17_ATI
    Reg18                                      =  0x8933, // GL_REG_18_ATI
    Reg19                                      =  0x8934, // GL_REG_19_ATI
    Reg20                                      =  0x8935, // GL_REG_20_ATI
    Reg21                                      =  0x8936, // GL_REG_21_ATI
    Reg22                                      =  0x8937, // GL_REG_22_ATI
    Reg23                                      =  0x8938, // GL_REG_23_ATI
    Reg24                                      =  0x8939, // GL_REG_24_ATI
    Reg25                                      =  0x893A, // GL_REG_25_ATI
    Reg26                                      =  0x893B, // GL_REG_26_ATI
    Reg27                                      =  0x893C, // GL_REG_27_ATI
    Reg28                                      =  0x893D, // GL_REG_28_ATI
    Reg29                                      =  0x893E, // GL_REG_29_ATI
    Reg30                                      =  0x893F, // GL_REG_30_ATI
    Reg31                                      =  0x8940, // GL_REG_31_ATI
    Con0                                       =  0x8941, // GL_CON_0_ATI
    Con1                                       =  0x8942, // GL_CON_1_ATI
    Con2                                       =  0x8943, // GL_CON_2_ATI
    Con3                                       =  0x8944, // GL_CON_3_ATI
    Con4                                       =  0x8945, // GL_CON_4_ATI
    Con5                                       =  0x8946, // GL_CON_5_ATI
    Con6                                       =  0x8947, // GL_CON_6_ATI
    Con7                                       =  0x8948, // GL_CON_7_ATI
    Con8                                       =  0x8949, // GL_CON_8_ATI
    Con9                                       =  0x894A, // GL_CON_9_ATI
    Con10                                      =  0x894B, // GL_CON_10_ATI
    Con11                                      =  0x894C, // GL_CON_11_ATI
    Con12                                      =  0x894D, // GL_CON_12_ATI
    Con13                                      =  0x894E, // GL_CON_13_ATI
    Con14                                      =  0x894F, // GL_CON_14_ATI
    Con15                                      =  0x8950, // GL_CON_15_ATI
    Con16                                      =  0x8951, // GL_CON_16_ATI
    Con17                                      =  0x8952, // GL_CON_17_ATI
    Con18                                      =  0x8953, // GL_CON_18_ATI
    Con19                                      =  0x8954, // GL_CON_19_ATI
    Con20                                      =  0x8955, // GL_CON_20_ATI
    Con21                                      =  0x8956, // GL_CON_21_ATI
    Con22                                      =  0x8957, // GL_CON_22_ATI
    Con23                                      =  0x8958, // GL_CON_23_ATI
    Con24                                      =  0x8959, // GL_CON_24_ATI
    Con25                                      =  0x895A, // GL_CON_25_ATI
    Con26                                      =  0x895B, // GL_CON_26_ATI
    Con27                                      =  0x895C, // GL_CON_27_ATI
    Con28                                      =  0x895D, // GL_CON_28_ATI
    Con29                                      =  0x895E, // GL_CON_29_ATI
    Con30                                      =  0x895F, // GL_CON_30_ATI
    Con31                                      =  0x8960, // GL_CON_31_ATI
    SecondaryInterpolator                      =  0x896D, // GL_SECONDARY_INTERPOLATOR_ATI
    NumFragmentRegisters                       =  0x896E, // GL_NUM_FRAGMENT_REGISTERS_ATI
    NumFragmentConstants                       =  0x896F, // GL_NUM_FRAGMENT_CONSTANTS_ATI
    NumPasses                                  =  0x8970, // GL_NUM_PASSES_ATI
    NumInstructionsPerPass                     =  0x8971, // GL_NUM_INSTRUCTIONS_PER_PASS_ATI
    NumInstructionsTotal                       =  0x8972, // GL_NUM_INSTRUCTIONS_TOTAL_ATI
    NumInputInterpolatorComponents             =  0x8973, // GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI
    NumLoopbackComponents                      =  0x8974, // GL_NUM_LOOPBACK_COMPONENTS_ATI
    ColorAlphaPairing                          =  0x8975, // GL_COLOR_ALPHA_PAIRING_ATI
    SwizzleStrq                                =  0x897A, // GL_SWIZZLE_STRQ_ATI
    SwizzleStrqDq                              =  0x897B, // GL_SWIZZLE_STRQ_DQ_ATI
    Interlace                                  =  0x8980, // GL_INTERLACE_OML
    InterlaceRead                              =  0x8981, // GL_INTERLACE_READ_OML
    FormatSubsample24_24                       =  0x8982, // GL_FORMAT_SUBSAMPLE_24_24_OML
    FormatSubsample244_244                     =  0x8983, // GL_FORMAT_SUBSAMPLE_244_244_OML
    ResampleReplicate                          =  0x8986, // GL_RESAMPLE_REPLICATE_OML
    ResampleZeroFill                           =  0x8987, // GL_RESAMPLE_ZERO_FILL_OML
    ResampleAverage                            =  0x8988, // GL_RESAMPLE_AVERAGE_OML
    ResampleDecimate                           =  0x8989, // GL_RESAMPLE_DECIMATE_OML
    VertexAttribMap1                           =  0x8A00, // GL_VERTEX_ATTRIB_MAP1_APPLE
    VertexAttribMap2                           =  0x8A01, // GL_VERTEX_ATTRIB_MAP2_APPLE
    VertexAttribMap1Size                       =  0x8A02, // GL_VERTEX_ATTRIB_MAP1_SIZE_APPLE
    VertexAttribMap1Coeff                      =  0x8A03, // GL_VERTEX_ATTRIB_MAP1_COEFF_APPLE
    VertexAttribMap1Order                      =  0x8A04, // GL_VERTEX_ATTRIB_MAP1_ORDER_APPLE
    VertexAttribMap1Domain                     =  0x8A05, // GL_VERTEX_ATTRIB_MAP1_DOMAIN_APPLE
    VertexAttribMap2Size                       =  0x8A06, // GL_VERTEX_ATTRIB_MAP2_SIZE_APPLE
    VertexAttribMap2Coeff                      =  0x8A07, // GL_VERTEX_ATTRIB_MAP2_COEFF_APPLE
    VertexAttribMap2Order                      =  0x8A08, // GL_VERTEX_ATTRIB_MAP2_ORDER_APPLE
    VertexAttribMap2Domain                     =  0x8A09, // GL_VERTEX_ATTRIB_MAP2_DOMAIN_APPLE
    ElementArrayAPPLE                          =  0x8A0C, // GL_ELEMENT_ARRAY_APPLE
    ElementArrayTypeAPPLE                      =  0x8A0D, // GL_ELEMENT_ARRAY_TYPE_APPLE
    ElementArrayPointerAPPLE                   =  0x8A0E, // GL_ELEMENT_ARRAY_POINTER_APPLE
    ColorFloat                                 =  0x8A0F, // GL_COLOR_FLOAT_APPLE
    BufferSerializedModify                     =  0x8A12, // GL_BUFFER_SERIALIZED_MODIFY_APPLE
    BufferFlushingUnmap                        =  0x8A13, // GL_BUFFER_FLUSHING_UNMAP_APPLE
    AuxDepthStencil                            =  0x8A14, // GL_AUX_DEPTH_STENCIL_APPLE
    PackRowBytes                               =  0x8A15, // GL_PACK_ROW_BYTES_APPLE
    UnpackRowBytes                             =  0x8A16, // GL_UNPACK_ROW_BYTES_APPLE
    ReleaseD                                   =  0x8A19, // GL_RELEASED_APPLE
    Volatile                                   =  0x8A1A, // GL_VOLATILE_APPLE
    Retained                                   =  0x8A1B, // GL_RETAINED_APPLE
    Undefined                                  =  0x8A1C, // GL_UNDEFINED_APPLE
    Purgeable                                  =  0x8A1D, // GL_PURGEABLE_APPLE
    RGB422                                     =  0x8A1F, // GL_RGB_422_APPLE
    TextureSRGBDecode                          =  0x8A48, // GL_TEXTURE_SRGB_DECODE_EXT
    Decode                                     =  0x8A49, // GL_DECODE_EXT
    SkipDecode                                 =  0x8A4A, // GL_SKIP_DECODE_EXT
    ProgramPipelineObject                      =  0x8A4F, // GL_PROGRAM_PIPELINE_OBJECT_EXT
    RGBRaw422                                  =  0x8A51, // GL_RGB_RAW_422_APPLE
    FragmentShaderDiscardsSamples              =  0x8A52, // GL_FRAGMENT_SHADER_DISCARDS_SAMPLES_EXT
    ShaderObject                               =  0x8B48, // GL_SHADER_OBJECT_ARB
    MaxFragmentUniformComponents               =  0x8B49, // GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB
    MaxVertexUniformComponents                 =  0x8B4A, // GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB
    MaxVaryingComponents                       =  0x8B4B, // GL_MAX_VARYING_COMPONENTS_EXT
    MaxVertexTextureImageUnits                 =  0x8B4C, // GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB
    MaxCombinedTextureImageUnits               =  0x8B4D, // GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB
    ObjectType                                 =  0x8B4E, // GL_OBJECT_TYPE_ARB
    ObjectSubtype                              =  0x8B4F, // GL_OBJECT_SUBTYPE_ARB
    ObjectDeleteStatus                         =  0x8B80, // GL_OBJECT_DELETE_STATUS_ARB
    ObjectCompileStatus                        =  0x8B81, // GL_OBJECT_COMPILE_STATUS_ARB
    ObjectLinkStatus                           =  0x8B82, // GL_OBJECT_LINK_STATUS_ARB
    ObjectValidateStatus                       =  0x8B83, // GL_OBJECT_VALIDATE_STATUS_ARB
    ObjectInfoLogLength                        =  0x8B84, // GL_OBJECT_INFO_LOG_LENGTH_ARB
    ObjectAttachedObjects                      =  0x8B85, // GL_OBJECT_ATTACHED_OBJECTS_ARB
    ObjectActiveUniforms                       =  0x8B86, // GL_OBJECT_ACTIVE_UNIFORMS_ARB
    ObjectActiveUniformMaxLength               =  0x8B87, // GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB
    ObjectShaderSourceLength                   =  0x8B88, // GL_OBJECT_SHADER_SOURCE_LENGTH_ARB
    ObjectActiveAttributes                     =  0x8B89, // GL_OBJECT_ACTIVE_ATTRIBUTES_ARB
    ObjectActiveAttributeMaxLength             =  0x8B8A, // GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB
    ShadingLanguageVersion                     =  0x8B8C, // GL_SHADING_LANGUAGE_VERSION_ARB
    ActiveProgram                              =  0x8B8D, // GL_ACTIVE_PROGRAM_EXT - For the OpenGL version of EXT_separate_shader_objects
    Palette4RGB8                               =  0x8B90, // GL_PALETTE4_RGB8_OES
    Palette4RGBA8                              =  0x8B91, // GL_PALETTE4_RGBA8_OES
    Palette4R5G6B5                             =  0x8B92, // GL_PALETTE4_R5_G6_B5_OES
    Palette4RGBA4                              =  0x8B93, // GL_PALETTE4_RGBA4_OES
    Palette4RGB5A1                             =  0x8B94, // GL_PALETTE4_RGB5_A1_OES
    Palette8RGB8                               =  0x8B95, // GL_PALETTE8_RGB8_OES
    Palette8RGBA8                              =  0x8B96, // GL_PALETTE8_RGBA8_OES
    Palette8R5G6B5                             =  0x8B97, // GL_PALETTE8_R5_G6_B5_OES
    Palette8RGBA4                              =  0x8B98, // GL_PALETTE8_RGBA4_OES
    Palette8RGB5A1                             =  0x8B99, // GL_PALETTE8_RGB5_A1_OES
    ImplementationColorReadType                =  0x8B9A, // GL_IMPLEMENTATION_COLOR_READ_TYPE_OES
    ImplementationColorReadFormat              =  0x8B9B, // GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES
    TileRasterOrderFixed                       =  0x8BB8, // GL_TILE_RASTER_ORDER_FIXED_MESA
    TileRasterOrderIncreasingX                 =  0x8BB9, // GL_TILE_RASTER_ORDER_INCREASING_X_MESA
    TileRasterOrderIncreasingY                 =  0x8BBA, // GL_TILE_RASTER_ORDER_INCREASING_Y_MESA
    FramebufferFlipY                           =  0x8BBB, // GL_FRAMEBUFFER_FLIP_Y_MESA
    FramebufferFlipX                           =  0x8BBC, // GL_FRAMEBUFFER_FLIP_X_MESA
    FramebufferSwapXy                          =  0x8BBD, // GL_FRAMEBUFFER_SWAP_XY_MESA
    CounterType                                =  0x8BC0, // GL_COUNTER_TYPE_AMD
    CounterRange                               =  0x8BC1, // GL_COUNTER_RANGE_AMD
    UnsignedInt64                              =  0x8BC2, // GL_UNSIGNED_INT64_AMD
    Percentage                                 =  0x8BC3, // GL_PERCENTAGE_AMD
    PerfmonResultAvailable                     =  0x8BC4, // GL_PERFMON_RESULT_AVAILABLE_AMD
    PerfmonResultSize                          =  0x8BC5, // GL_PERFMON_RESULT_SIZE_AMD
    PerfmonResult                              =  0x8BC6, // GL_PERFMON_RESULT_AMD
    TextureRedType                             =  0x8C10, // GL_TEXTURE_RED_TYPE
    TextureGreenType                           =  0x8C11, // GL_TEXTURE_GREEN_TYPE
    TextureBlueType                            =  0x8C12, // GL_TEXTURE_BLUE_TYPE
    TextureAlphaType                           =  0x8C13, // GL_TEXTURE_ALPHA_TYPE
    TextureLuminanceType                       =  0x8C14, // GL_TEXTURE_LUMINANCE_TYPE_ARB
    TextureIntensityType                       =  0x8C15, // GL_TEXTURE_INTENSITY_TYPE_ARB
    TextureDepthType                           =  0x8C16, // GL_TEXTURE_DEPTH_TYPE
    UnsignedNormalized                         =  0x8C17, // GL_UNSIGNED_NORMALIZED
    Texture1DArray                             =  0x8C18, // GL_TEXTURE_1D_ARRAY_EXT
    Texture2DArray                             =  0x8C1A, // GL_TEXTURE_2D_ARRAY_EXT
    TextureBinding1DArray                      =  0x8C1C, // GL_TEXTURE_BINDING_1D_ARRAY_EXT
    TextureBinding2DArray                      =  0x8C1D, // GL_TEXTURE_BINDING_2D_ARRAY_EXT
    MaxProgramOutputVertices                   =  0x8C27, // GL_MAX_PROGRAM_OUTPUT_VERTICES_NV
    MaxProgramTotalOutputComponents            =  0x8C28, // GL_MAX_PROGRAM_TOTAL_OUTPUT_COMPONENTS_NV
    MaxGeometryTextureImageUnits               =  0x8C29, // GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_ARB
    TextureBuffer                              =  0x8C2A, // GL_TEXTURE_BUFFER_ARB
    MaxTextureBufferSize                       =  0x8C2B, // GL_MAX_TEXTURE_BUFFER_SIZE_ARB
    TextureBindingBuffer                       =  0x8C2C, // GL_TEXTURE_BINDING_BUFFER_ARB
    TextureBufferDataStoreBinding              =  0x8C2D, // GL_TEXTURE_BUFFER_DATA_STORE_BINDING
    TextureBufferFormat                        =  0x8C2E, // GL_TEXTURE_BUFFER_FORMAT_ARB
    SampleShading                              =  0x8C36, // GL_SAMPLE_SHADING_ARB
    MinSampleShadingValue                      =  0x8C37, // GL_MIN_SAMPLE_SHADING_VALUE
    UnsignedInt10F11F11FRev                    =  0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV_EXT
    RGBASignedComponents                       =  0x8C3C, // GL_RGBA_SIGNED_COMPONENTS_EXT
    UnsignedInt5_9_9_9Rev                      =  0x8C3E, // GL_UNSIGNED_INT_5_9_9_9_REV
    TextureSharedSize                          =  0x8C3F, // GL_TEXTURE_SHARED_SIZE
    SluminanceAlpha                            =  0x8C44, // GL_SLUMINANCE_ALPHA_EXT
    Sluminance8Alpha8                          =  0x8C45, // GL_SLUMINANCE8_ALPHA8_EXT
    Sluminance                                 =  0x8C46, // GL_SLUMINANCE_EXT
    Sluminance8                                =  0x8C47, // GL_SLUMINANCE8_EXT
    CompressedSRGB                             =  0x8C48, // GL_COMPRESSED_SRGB_EXT
    CompressedSRGBAlpha                        =  0x8C49, // GL_COMPRESSED_SRGB_ALPHA_EXT
    CompressedSluminance                       =  0x8C4A, // GL_COMPRESSED_SLUMINANCE_EXT
    CompressedSluminanceAlpha                  =  0x8C4B, // GL_COMPRESSED_SLUMINANCE_ALPHA_EXT
    CompressedLuminanceLATC1                   =  0x8C70, // GL_COMPRESSED_LUMINANCE_LATC1_EXT
    CompressedSignedLuminanceLATC1             =  0x8C71, // GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT
    CompressedLuminanceAlphaLATC2              =  0x8C72, // GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT
    CompressedSignedLuminanceAlphaLATC2        =  0x8C73, // GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT
    TessControlProgramParameterBuffer          =  0x8C74, // GL_TESS_CONTROL_PROGRAM_PARAMETER_BUFFER_NV
    TessEvaluationProgramParameterBuffer       =  0x8C75, // GL_TESS_EVALUATION_PROGRAM_PARAMETER_BUFFER_NV
    TransformFeedbackVaryingMaxLength          =  0x8C76, // GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH_EXT
    BackPrimaryColor                           =  0x8C77, // GL_BACK_PRIMARY_COLOR_NV
    BackSecondaryColor                         =  0x8C78, // GL_BACK_SECONDARY_COLOR_NV
    TextureCoord                               =  0x8C79, // GL_TEXTURE_COORD_NV
    ClipDistance                               =  0x8C7A, // GL_CLIP_DISTANCE_NV
    VertexID                                   =  0x8C7B, // GL_VERTEX_ID_NV
    PrimitiveID                                =  0x8C7C, // GL_PRIMITIVE_ID_NV
    GenericAttrib                              =  0x8C7D, // GL_GENERIC_ATTRIB_NV
    TransformFeedbackAttribs                   =  0x8C7E, // GL_TRANSFORM_FEEDBACK_ATTRIBS_NV
    TransformFeedbackBufferMode                =  0x8C7F, // GL_TRANSFORM_FEEDBACK_BUFFER_MODE_EXT
    MaxTransformFeedbackSeparateComponents     =  0x8C80, // GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS
    ActiveVaryings                             =  0x8C81, // GL_ACTIVE_VARYINGS_NV
    ActiveVaryingMaxLength                     =  0x8C82, // GL_ACTIVE_VARYING_MAX_LENGTH_NV
    TransformFeedbackVaryings                  =  0x8C83, // GL_TRANSFORM_FEEDBACK_VARYINGS_EXT
    TransformFeedbackBufferStart               =  0x8C84, // GL_TRANSFORM_FEEDBACK_BUFFER_START_EXT
    TransformFeedbackBufferSize                =  0x8C85, // GL_TRANSFORM_FEEDBACK_BUFFER_SIZE_EXT
    TransformFeedbackRecord                    =  0x8C86, // GL_TRANSFORM_FEEDBACK_RECORD_NV
    PrimitivesGenerated                        =  0x8C87, // GL_PRIMITIVES_GENERATED_EXT
    TransformFeedbackPrimitivesWritten         =  0x8C88, // GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_EXT
    RasterizerDiscard                          =  0x8C89, // GL_RASTERIZER_DISCARD_EXT
    MaxTransformFeedbackInterleavedComponents  =  0x8C8A, // GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS
    MaxTransformFeedbackSeparateAttribs        =  0x8C8B, // GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS
    InterleavedAttribs                         =  0x8C8C, // GL_INTERLEAVED_ATTRIBS_EXT
    SeparateAttribs                            =  0x8C8D, // GL_SEPARATE_ATTRIBS_EXT
    TransformFeedbackBuffer                    =  0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER_EXT
    TransformFeedbackBufferBinding             =  0x8C8F, // GL_TRANSFORM_FEEDBACK_BUFFER_BINDING_EXT
    PointSpriteCoordOrigin                     =  0x8CA0, // GL_POINT_SPRITE_COORD_ORIGIN
    DrawFramebufferBinding                     =  0x8CA6, // GL_DRAW_FRAMEBUFFER_BINDING_EXT
    RenderbufferBinding                        =  0x8CA7, // GL_RENDERBUFFER_BINDING_EXT
    ReadFramebuffer                            =  0x8CA8, // GL_READ_FRAMEBUFFER_EXT
    DrawFramebuffer                            =  0x8CA9, // GL_DRAW_FRAMEBUFFER_EXT
    ReadFramebufferBinding                     =  0x8CAA, // GL_READ_FRAMEBUFFER_BINDING_EXT
    FramebufferComplete                        =  0x8CD5, // GL_FRAMEBUFFER_COMPLETE_EXT
    FramebufferIncompleteAttachment            =  0x8CD6, // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
    FramebufferIncompleteMissingAttachment     =  0x8CD7, // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
    FramebufferIncompleteDimensions            =  0x8CD9, // GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
    FramebufferIncompleteFormats               =  0x8CDA, // GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
    FramebufferIncompleteDrawBuffer            =  0x8CDB, // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT
    FramebufferIncompleteReadBuffer            =  0x8CDC, // GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT
    FramebufferUnsupported                     =  0x8CDD, // GL_FRAMEBUFFER_UNSUPPORTED_EXT
    Framebuffer                                =  0x8D40, // GL_FRAMEBUFFER_EXT
    Renderbuffer                               =  0x8D41, // GL_RENDERBUFFER_EXT
    FramebufferIncompleteMultisample           =  0x8D56, // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT
    MaxSamples                                 =  0x8D57, // GL_MAX_SAMPLES
    RedInteger                                 =  0x8D94, // GL_RED_INTEGER_EXT
    GreenInteger                               =  0x8D95, // GL_GREEN_INTEGER_EXT
    BlueInteger                                =  0x8D96, // GL_BLUE_INTEGER_EXT
    AlphaInteger                               =  0x8D97, // GL_ALPHA_INTEGER_EXT
    RGBInteger                                 =  0x8D98, // GL_RGB_INTEGER_EXT
    RGBAInteger                                =  0x8D99, // GL_RGBA_INTEGER_EXT
    BGRInteger                                 =  0x8D9A, // GL_BGR_INTEGER_EXT
    BGRAInteger                                =  0x8D9B, // GL_BGRA_INTEGER_EXT
    LuminanceInteger                           =  0x8D9C, // GL_LUMINANCE_INTEGER_EXT
    LuminanceAlphaInteger                      =  0x8D9D, // GL_LUMINANCE_ALPHA_INTEGER_EXT
    RGBAIntegerMode                            =  0x8D9E, // GL_RGBA_INTEGER_MODE_EXT
    MaxProgramParameterBufferBindings          =  0x8DA0, // GL_MAX_PROGRAM_PARAMETER_BUFFER_BINDINGS_NV
    MaxProgramParameterBufferSize              =  0x8DA1, // GL_MAX_PROGRAM_PARAMETER_BUFFER_SIZE_NV
    VertexProgramParameterBuffer               =  0x8DA2, // GL_VERTEX_PROGRAM_PARAMETER_BUFFER_NV
    GeometryProgramParameterBuffer             =  0x8DA3, // GL_GEOMETRY_PROGRAM_PARAMETER_BUFFER_NV
    FragmentProgramParameterBuffer             =  0x8DA4, // GL_FRAGMENT_PROGRAM_PARAMETER_BUFFER_NV
    MaxProgramGenericAttribs                   =  0x8DA5, // GL_MAX_PROGRAM_GENERIC_ATTRIBS_NV
    MaxProgramGenericResults                   =  0x8DA6, // GL_MAX_PROGRAM_GENERIC_RESULTS_NV
    FramebufferIncompleteLayerTargets          =  0x8DA8, // GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB
    FramebufferIncompleteLayerCount            =  0x8DA9, // GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB
    Layer                                      =  0x8DAA, // GL_LAYER_NV
    Float32UnsignedInt24_8Rev                  =  0x8DAD, // GL_FLOAT_32_UNSIGNED_INT_24_8_REV
    ShaderInclude                              =  0x8DAE, // GL_SHADER_INCLUDE_ARB
    DepthBufferFloatMode                       =  0x8DAF, // GL_DEPTH_BUFFER_FLOAT_MODE_NV
    FramebufferSRGB                            =  0x8DB9, // GL_FRAMEBUFFER_SRGB_EXT
    FramebufferSRGBCapable                     =  0x8DBA, // GL_FRAMEBUFFER_SRGB_CAPABLE_EXT
    Sampler1DArray                             =  0x8DC0, // GL_SAMPLER_1D_ARRAY_EXT
    Sampler2DArray                             =  0x8DC1, // GL_SAMPLER_2D_ARRAY_EXT
    SamplerBufferEXT                           =  0x8DC2, // GL_SAMPLER_BUFFER_EXT
    Sampler1DArrayShadow                       =  0x8DC3, // GL_SAMPLER_1D_ARRAY_SHADOW_EXT
    Sampler2DArrayShadow                       =  0x8DC4, // GL_SAMPLER_2D_ARRAY_SHADOW_EXT
    SamplerCubeShadow                          =  0x8DC5, // GL_SAMPLER_CUBE_SHADOW_EXT
    UnsignedIntVec2                            =  0x8DC6, // GL_UNSIGNED_INT_VEC2_EXT
    UnsignedIntVec3                            =  0x8DC7, // GL_UNSIGNED_INT_VEC3_EXT
    UnsignedIntVec4                            =  0x8DC8, // GL_UNSIGNED_INT_VEC4_EXT
    IntSampler1D                               =  0x8DC9, // GL_INT_SAMPLER_1D_EXT
    IntSampler2D                               =  0x8DCA, // GL_INT_SAMPLER_2D_EXT
    IntSampler3D                               =  0x8DCB, // GL_INT_SAMPLER_3D_EXT
    IntSamplerCube                             =  0x8DCC, // GL_INT_SAMPLER_CUBE_EXT
    IntSampler2DRect                           =  0x8DCD, // GL_INT_SAMPLER_2D_RECT_EXT
    IntSampler1DArray                          =  0x8DCE, // GL_INT_SAMPLER_1D_ARRAY_EXT
    IntSampler2DArray                          =  0x8DCF, // GL_INT_SAMPLER_2D_ARRAY_EXT
    IntSamplerBufferEXT                        =  0x8DD0, // GL_INT_SAMPLER_BUFFER_EXT
    UnsignedIntSampler1D                       =  0x8DD1, // GL_UNSIGNED_INT_SAMPLER_1D_EXT
    UnsignedIntSampler2D                       =  0x8DD2, // GL_UNSIGNED_INT_SAMPLER_2D_EXT
    UnsignedIntSampler3D                       =  0x8DD3, // GL_UNSIGNED_INT_SAMPLER_3D_EXT
    UnsignedIntSamplerCube                     =  0x8DD4, // GL_UNSIGNED_INT_SAMPLER_CUBE_EXT
    UnsignedIntSampler2DRect                   =  0x8DD5, // GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT
    UnsignedIntSampler1DArray                  =  0x8DD6, // GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT
    UnsignedIntSampler2DArray                  =  0x8DD7, // GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT
    UnsignedIntSamplerBufferEXT                =  0x8DD8, // GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT
    GeometryShader                             =  0x8DD9, // GL_GEOMETRY_SHADER_ARB
    GeometryVerticesOut                        =  0x8DDA, // GL_GEOMETRY_VERTICES_OUT_ARB
    GeometryInputType                          =  0x8DDB, // GL_GEOMETRY_INPUT_TYPE_ARB
    GeometryOutputType                         =  0x8DDC, // GL_GEOMETRY_OUTPUT_TYPE_ARB
    MaxGeometryVaryingComponents               =  0x8DDD, // GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB
    MaxVertexVaryingComponents                 =  0x8DDE, // GL_MAX_VERTEX_VARYING_COMPONENTS_ARB
    MaxGeometryUniformComponents               =  0x8DDF, // GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB
    MaxGeometryOutputVertices                  =  0x8DE0, // GL_MAX_GEOMETRY_OUTPUT_VERTICES
    MaxGeometryTotalOutputComponents           =  0x8DE1, // GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS
    MaxVertexBindableUniforms                  =  0x8DE2, // GL_MAX_VERTEX_BINDABLE_UNIFORMS_EXT
    MaxFragmentBindableUniforms                =  0x8DE3, // GL_MAX_FRAGMENT_BINDABLE_UNIFORMS_EXT
    MaxGeometryBindableUniforms                =  0x8DE4, // GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT
    MaxSubroutines                             =  0x8DE7, // GL_MAX_SUBROUTINES
    MaxSubroutineUniformLocations              =  0x8DE8, // GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS
    NamedStringLength                          =  0x8DE9, // GL_NAMED_STRING_LENGTH_ARB
    NamedStringType                            =  0x8DEA, // GL_NAMED_STRING_TYPE_ARB
    MaxBindableUniformSize                     =  0x8DED, // GL_MAX_BINDABLE_UNIFORM_SIZE_EXT
    UniformBuffer                              =  0x8DEE, // GL_UNIFORM_BUFFER_EXT
    UniformBufferBinding                       =  0x8DEF, // GL_UNIFORM_BUFFER_BINDING_EXT
    MaxMultisampleCoverageModes                =  0x8E11, // GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV
    MultisampleCoverageModes                   =  0x8E12, // GL_MULTISAMPLE_COVERAGE_MODES_NV
    QueryWait                                  =  0x8E13, // GL_QUERY_WAIT_NV
    QueryNoWait                                =  0x8E14, // GL_QUERY_NO_WAIT_NV
    QueryByRegionWait                          =  0x8E15, // GL_QUERY_BY_REGION_WAIT_NV
    QueryByRegionNoWait                        =  0x8E16, // GL_QUERY_BY_REGION_NO_WAIT_NV
    PolygonOffsetClamp                         =  0x8E1B, // GL_POLYGON_OFFSET_CLAMP
    MaxCombinedTessControlUniformComponents    =  0x8E1E, // GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS
    MaxCombinedTessEvaluationUniformComponents =  0x8E1F, // GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS
    ColorSamples                               =  0x8E20, // GL_COLOR_SAMPLES_NV
    TransformFeedback                          =  0x8E22, // GL_TRANSFORM_FEEDBACK_NV
    TransformFeedbackBufferPaused              =  0x8E23, // GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED
    TransformFeedbackBufferActive              =  0x8E24, // GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE
    TransformFeedbackBinding                   =  0x8E25, // GL_TRANSFORM_FEEDBACK_BINDING
    Frame                                      =  0x8E26, // GL_FRAME_NV
    Fields                                     =  0x8E27, // GL_FIELDS_NV
    CurrentTime                                =  0x8E28, // GL_CURRENT_TIME_NV
    NumFillStreams                             =  0x8E29, // GL_NUM_FILL_STREAMS_NV
    PresentTime                                =  0x8E2A, // GL_PRESENT_TIME_NV
    PresentDuration                            =  0x8E2B, // GL_PRESENT_DURATION_NV
    ProgramMatrix                              =  0x8E2D, // GL_PROGRAM_MATRIX_EXT
    TransposeProgramMatrix                     =  0x8E2E, // GL_TRANSPOSE_PROGRAM_MATRIX_EXT
    ProgramMatrixStackDepth                    =  0x8E2F, // GL_PROGRAM_MATRIX_STACK_DEPTH_EXT
    TextureSwizzleR                            =  0x8E42, // GL_TEXTURE_SWIZZLE_R_EXT
    TextureSwizzleG                            =  0x8E43, // GL_TEXTURE_SWIZZLE_G_EXT
    TextureSwizzleB                            =  0x8E44, // GL_TEXTURE_SWIZZLE_B_EXT
    TextureSwizzleA                            =  0x8E45, // GL_TEXTURE_SWIZZLE_A_EXT
    TextureSwizzleRGBA                         =  0x8E46, // GL_TEXTURE_SWIZZLE_RGBA_EXT
    QuadsFollowProvokingVertexConvention       =  0x8E4C, // GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION
    FirstVertexConvention                      =  0x8E4D, // GL_FIRST_VERTEX_CONVENTION_EXT
    LastVertexConvention                       =  0x8E4E, // GL_LAST_VERTEX_CONVENTION_EXT
    ProvokingVertex                            =  0x8E4F, // GL_PROVOKING_VERTEX_EXT
    SampleLocation                             =  0x8E50, // GL_SAMPLE_LOCATION_NV
    SampleMask                                 =  0x8E51, // GL_SAMPLE_MASK_NV
    SampleMaskValue                            =  0x8E52, // GL_SAMPLE_MASK_VALUE
    TextureBindingRenderbuffer                 =  0x8E53, // GL_TEXTURE_BINDING_RENDERBUFFER_NV
    TextureRenderbufferDataStoreBinding        =  0x8E54, // GL_TEXTURE_RENDERBUFFER_DATA_STORE_BINDING_NV
    TextureRenderbuffer                        =  0x8E55, // GL_TEXTURE_RENDERBUFFER_NV
    SamplerRenderbuffer                        =  0x8E56, // GL_SAMPLER_RENDERBUFFER_NV
    IntSamplerRenderbuffer                     =  0x8E57, // GL_INT_SAMPLER_RENDERBUFFER_NV
    UnsignedIntSamplerRenderbuffer             =  0x8E58, // GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV
    MaxSampleMaskWords                         =  0x8E59, // GL_MAX_SAMPLE_MASK_WORDS_NV
    MaxGeometryProgramInvocations              =  0x8E5A, // GL_MAX_GEOMETRY_PROGRAM_INVOCATIONS_NV
    MinFragmentInterpolationOffset             =  0x8E5B, // GL_MIN_FRAGMENT_INTERPOLATION_OFFSET
    MaxFragmentInterpolationOffset             =  0x8E5C, // GL_MAX_FRAGMENT_INTERPOLATION_OFFSET
    FragmentInterpolationOffsetBits            =  0x8E5D, // GL_FRAGMENT_INTERPOLATION_OFFSET_BITS
    MinProgramTextureGatherOffset              =  0x8E5E, // GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET
    MaxProgramTextureGatherOffset              =  0x8E5F, // GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET
    MaxMeshUniformBlocks                       =  0x8E60, // GL_MAX_MESH_UNIFORM_BLOCKS_NV
    MaxMeshTextureImageUnits                   =  0x8E61, // GL_MAX_MESH_TEXTURE_IMAGE_UNITS_NV
    MaxMeshImageUniforms                       =  0x8E62, // GL_MAX_MESH_IMAGE_UNIFORMS_NV
    MaxMeshUniformComponents                   =  0x8E63, // GL_MAX_MESH_UNIFORM_COMPONENTS_NV
    MaxMeshAtomicCounterBuffers                =  0x8E64, // GL_MAX_MESH_ATOMIC_COUNTER_BUFFERS_NV
    MaxMeshAtomicCounters                      =  0x8E65, // GL_MAX_MESH_ATOMIC_COUNTERS_NV
    MaxMeshShaderStorageBlocks                 =  0x8E66, // GL_MAX_MESH_SHADER_STORAGE_BLOCKS_NV
    MaxCombinedMeshUniformComponents           =  0x8E67, // GL_MAX_COMBINED_MESH_UNIFORM_COMPONENTS_NV
    MaxTaskUniformBlocks                       =  0x8E68, // GL_MAX_TASK_UNIFORM_BLOCKS_NV
    MaxTaskTextureImageUnits                   =  0x8E69, // GL_MAX_TASK_TEXTURE_IMAGE_UNITS_NV
    MaxTaskImageUniforms                       =  0x8E6A, // GL_MAX_TASK_IMAGE_UNIFORMS_NV
    MaxTaskUniformComponents                   =  0x8E6B, // GL_MAX_TASK_UNIFORM_COMPONENTS_NV
    MaxTaskAtomicCounterBuffers                =  0x8E6C, // GL_MAX_TASK_ATOMIC_COUNTER_BUFFERS_NV
    MaxTaskAtomicCounters                      =  0x8E6D, // GL_MAX_TASK_ATOMIC_COUNTERS_NV
    MaxTaskShaderStorageBlocks                 =  0x8E6E, // GL_MAX_TASK_SHADER_STORAGE_BLOCKS_NV
    MaxCombinedTaskUniformComponents           =  0x8E6F, // GL_MAX_COMBINED_TASK_UNIFORM_COMPONENTS_NV
    MaxTransformFeedbackBuffers                =  0x8E70, // GL_MAX_TRANSFORM_FEEDBACK_BUFFERS
    MaxVertexStreams                           =  0x8E71, // GL_MAX_VERTEX_STREAMS
    TessControlOutputVertices                  =  0x8E75, // GL_TESS_CONTROL_OUTPUT_VERTICES
    TessGenMode                                =  0x8E76, // GL_TESS_GEN_MODE
    TessGenSpacing                             =  0x8E77, // GL_TESS_GEN_SPACING
    TessGenVertexOrder                         =  0x8E78, // GL_TESS_GEN_VERTEX_ORDER
    TessGenPointMode                           =  0x8E79, // GL_TESS_GEN_POINT_MODE
    Isolines                                   =  0x8E7A, // GL_ISOLINES
    FractionalOdd                              =  0x8E7B, // GL_FRACTIONAL_ODD
    FractionalEven                             =  0x8E7C, // GL_FRACTIONAL_EVEN
    MaxPatchVertices                           =  0x8E7D, // GL_MAX_PATCH_VERTICES
    MaxTessGenLevel                            =  0x8E7E, // GL_MAX_TESS_GEN_LEVEL
    MaxTessControlUniformComponents            =  0x8E7F, // GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS
    MaxTessEvaluationUniformComponents         =  0x8E80, // GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS
    MaxTessControlTextureImageUnits            =  0x8E81, // GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS
    MaxTessEvaluationTextureImageUnits         =  0x8E82, // GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS
    MaxTessControlOutputComponents             =  0x8E83, // GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS
    MaxTessPatchComponents                     =  0x8E84, // GL_MAX_TESS_PATCH_COMPONENTS
    MaxTessControlTotalOutputComponents        =  0x8E85, // GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS
    MaxTessEvaluationOutputComponents          =  0x8E86, // GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS
    Inclusive                                  =  0x8F10, // GL_INCLUSIVE_EXT
    Exclusive                                  =  0x8F11, // GL_EXCLUSIVE_EXT
    WindowRectangle                            =  0x8F12, // GL_WINDOW_RECTANGLE_EXT
    WindowRectangleMode                        =  0x8F13, // GL_WINDOW_RECTANGLE_MODE_EXT
    MaxWindowRectangles                        =  0x8F14, // GL_MAX_WINDOW_RECTANGLES_EXT
    NumWindowRectangles                        =  0x8F15, // GL_NUM_WINDOW_RECTANGLES_EXT
    BufferGPUAddress                           =  0x8F1D, // GL_BUFFER_GPU_ADDRESS_NV
    VertexAttribArrayUnified                   =  0x8F1E, // GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV
    ElementArrayUnified                        =  0x8F1F, // GL_ELEMENT_ARRAY_UNIFIED_NV
    VertexAttribArrayAddress                   =  0x8F20, // GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV
    VertexArrayAddress                         =  0x8F21, // GL_VERTEX_ARRAY_ADDRESS_NV
    NormalArrayAddress                         =  0x8F22, // GL_NORMAL_ARRAY_ADDRESS_NV
    ColorArrayAddress                          =  0x8F23, // GL_COLOR_ARRAY_ADDRESS_NV
    IndexArrayAddress                          =  0x8F24, // GL_INDEX_ARRAY_ADDRESS_NV
    TextureCoordArrayAddress                   =  0x8F25, // GL_TEXTURE_COORD_ARRAY_ADDRESS_NV
    EdgeFlagArrayAddress                       =  0x8F26, // GL_EDGE_FLAG_ARRAY_ADDRESS_NV
    SecondaryColorArrayAddress                 =  0x8F27, // GL_SECONDARY_COLOR_ARRAY_ADDRESS_NV
    FogCoordArrayAddress                       =  0x8F28, // GL_FOG_COORD_ARRAY_ADDRESS_NV
    ElementArrayAddress                        =  0x8F29, // GL_ELEMENT_ARRAY_ADDRESS_NV
    VertexAttribArrayLength                    =  0x8F2A, // GL_VERTEX_ATTRIB_ARRAY_LENGTH_NV
    VertexArrayLength                          =  0x8F2B, // GL_VERTEX_ARRAY_LENGTH_NV
    NormalArrayLength                          =  0x8F2C, // GL_NORMAL_ARRAY_LENGTH_NV
    ColorArrayLength                           =  0x8F2D, // GL_COLOR_ARRAY_LENGTH_NV
    IndexArrayLength                           =  0x8F2E, // GL_INDEX_ARRAY_LENGTH_NV
    TextureCoordArrayLength                    =  0x8F2F, // GL_TEXTURE_COORD_ARRAY_LENGTH_NV
    EdgeFlagArrayLength                        =  0x8F30, // GL_EDGE_FLAG_ARRAY_LENGTH_NV
    SecondaryColorArrayLength                  =  0x8F31, // GL_SECONDARY_COLOR_ARRAY_LENGTH_NV
    FogCoordArrayLength                        =  0x8F32, // GL_FOG_COORD_ARRAY_LENGTH_NV
    ElementArrayLength                         =  0x8F33, // GL_ELEMENT_ARRAY_LENGTH_NV
    GPUAddress                                 =  0x8F34, // GL_GPU_ADDRESS_NV
    MaxShaderBufferAddress                     =  0x8F35, // GL_MAX_SHADER_BUFFER_ADDRESS_NV
    CopyReadBufferBinding                      =  0x8F36, // GL_COPY_READ_BUFFER_BINDING
    CopyWriteBufferBinding                     =  0x8F37, // GL_COPY_WRITE_BUFFER_BINDING
    MaxImageUnits                              =  0x8F38, // GL_MAX_IMAGE_UNITS
    MaxCombinedImageUnitsAndFragmentOutputS    =  0x8F39, // GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS
    ImageBindingName                           =  0x8F3A, // GL_IMAGE_BINDING_NAME
    ImageBindingLevel                          =  0x8F3B, // GL_IMAGE_BINDING_LEVEL
    ImageBindingLayered                        =  0x8F3C, // GL_IMAGE_BINDING_LAYERED
    ImageBindingLayer                          =  0x8F3D, // GL_IMAGE_BINDING_LAYER
    ImageBindingAccess                         =  0x8F3E, // GL_IMAGE_BINDING_ACCESS
    DrawIndirectUnified                        =  0x8F40, // GL_DRAW_INDIRECT_UNIFIED_NV
    DrawIndirectAddress                        =  0x8F41, // GL_DRAW_INDIRECT_ADDRESS_NV
    DrawIndirectLength                         =  0x8F42, // GL_DRAW_INDIRECT_LENGTH_NV
    DrawIndirectBufferBinding                  =  0x8F43, // GL_DRAW_INDIRECT_BUFFER_BINDING
    MaxProgramSubroutineParameters             =  0x8F44, // GL_MAX_PROGRAM_SUBROUTINE_PARAMETERS_NV
    MaxProgramSubroutineNum                    =  0x8F45, // GL_MAX_PROGRAM_SUBROUTINE_NUM_NV
    DoubleMat2                                 =  0x8F46, // GL_DOUBLE_MAT2_EXT
    DoubleMat3                                 =  0x8F47, // GL_DOUBLE_MAT3_EXT
    DoubleMat4                                 =  0x8F48, // GL_DOUBLE_MAT4_EXT
    DoubleMat2x3                               =  0x8F49, // GL_DOUBLE_MAT2x3_EXT
    DoubleMat2x4                               =  0x8F4A, // GL_DOUBLE_MAT2x4_EXT
    DoubleMat3x2                               =  0x8F4B, // GL_DOUBLE_MAT3x2_EXT
    DoubleMat3x4                               =  0x8F4C, // GL_DOUBLE_MAT3x4_EXT
    DoubleMat4x2                               =  0x8F4D, // GL_DOUBLE_MAT4x2_EXT
    DoubleMat4x3                               =  0x8F4E, // GL_DOUBLE_MAT4x3_EXT
    VertexBindingBuffer                        =  0x8F4F, // GL_VERTEX_BINDING_BUFFER
    RedSnorm                                   =  0x8F90, // GL_RED_SNORM
    RgSnorm                                    =  0x8F91, // GL_RG_SNORM
    RGBSnorm                                   =  0x8F92, // GL_RGB_SNORM
    RGBASnorm                                  =  0x8F93, // GL_RGBA_SNORM
    SignedNormalized                           =  0x8F9C, // GL_SIGNED_NORMALIZED
    MaxProgramTextureGatherComponents          =  0x8F9F, // GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS_ARB
    Int8                                       =  0x8FE0, // GL_INT8_NV
    Int8Vec2                                   =  0x8FE1, // GL_INT8_VEC2_NV
    Int8Vec3                                   =  0x8FE2, // GL_INT8_VEC3_NV
    Int8Vec4                                   =  0x8FE3, // GL_INT8_VEC4_NV
    Int16                                      =  0x8FE4, // GL_INT16_NV
    Int16Vec2                                  =  0x8FE5, // GL_INT16_VEC2_NV
    Int16Vec3                                  =  0x8FE6, // GL_INT16_VEC3_NV
    Int16Vec4                                  =  0x8FE7, // GL_INT16_VEC4_NV
    Int64Vec2                                  =  0x8FE9, // GL_INT64_VEC2_NV
    Int64Vec3                                  =  0x8FEA, // GL_INT64_VEC3_NV
    Int64Vec4                                  =  0x8FEB, // GL_INT64_VEC4_NV
    UnsignedInt8                               =  0x8FEC, // GL_UNSIGNED_INT8_NV
    UnsignedInt8Vec2                           =  0x8FED, // GL_UNSIGNED_INT8_VEC2_NV
    UnsignedInt8Vec3                           =  0x8FEE, // GL_UNSIGNED_INT8_VEC3_NV
    UnsignedInt8Vec4                           =  0x8FEF, // GL_UNSIGNED_INT8_VEC4_NV
    UnsignedInt16                              =  0x8FF0, // GL_UNSIGNED_INT16_NV
    UnsignedInt16Vec2                          =  0x8FF1, // GL_UNSIGNED_INT16_VEC2_NV
    UnsignedInt16Vec3                          =  0x8FF2, // GL_UNSIGNED_INT16_VEC3_NV
    UnsignedInt16Vec4                          =  0x8FF3, // GL_UNSIGNED_INT16_VEC4_NV
    UnsignedInt64Vec2                          =  0x8FF5, // GL_UNSIGNED_INT64_VEC2_NV
    UnsignedInt64Vec3                          =  0x8FF6, // GL_UNSIGNED_INT64_VEC3_NV
    UnsignedInt64Vec4                          =  0x8FF7, // GL_UNSIGNED_INT64_VEC4_NV
    Float16                                    =  0x8FF8, // GL_FLOAT16_NV
    Float16Vec2                                =  0x8FF9, // GL_FLOAT16_VEC2_NV
    Float16Vec3                                =  0x8FFA, // GL_FLOAT16_VEC3_NV
    Float16Vec4                                =  0x8FFB, // GL_FLOAT16_VEC4_NV
    DoubleVec2                                 =  0x8FFC, // GL_DOUBLE_VEC2_EXT
    DoubleVec3                                 =  0x8FFD, // GL_DOUBLE_VEC3_EXT
    DoubleVec4                                 =  0x8FFE, // GL_DOUBLE_VEC4_EXT
    SamplerBufferAMD                           =  0x9001, // GL_SAMPLER_BUFFER_AMD
    IntSamplerBufferAMD                        =  0x9002, // GL_INT_SAMPLER_BUFFER_AMD
    UnsignedIntSamplerBufferAMD                =  0x9003, // GL_UNSIGNED_INT_SAMPLER_BUFFER_AMD
    TessellationMode                           =  0x9004, // GL_TESSELLATION_MODE_AMD
    TessellationFactor                         =  0x9005, // GL_TESSELLATION_FACTOR_AMD
    Discrete                                   =  0x9006, // GL_DISCRETE_AMD
    Continuous                                 =  0x9007, // GL_CONTINUOUS_AMD
    TextureBindingCubeMapArray                 =  0x900A, // GL_TEXTURE_BINDING_CUBE_MAP_ARRAY
    SamplerCubeMapArray                        =  0x900C, // GL_SAMPLER_CUBE_MAP_ARRAY_ARB
    SamplerCubeMapArrayShadow                  =  0x900D, // GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_ARB
    IntSamplerCubeMapArray                     =  0x900E, // GL_INT_SAMPLER_CUBE_MAP_ARRAY_ARB
    UnsignedIntSamplerCubeMapArray             =  0x900F, // GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_ARB
    AlphaSnorm                                 =  0x9010, // GL_ALPHA_SNORM
    LuminanceSnorm                             =  0x9011, // GL_LUMINANCE_SNORM
    LuminanceAlphaSnorm                        =  0x9012, // GL_LUMINANCE_ALPHA_SNORM
    IntensitySnorm                             =  0x9013, // GL_INTENSITY_SNORM
    Alpha8Snorm                                =  0x9014, // GL_ALPHA8_SNORM
    Luminance8Snorm                            =  0x9015, // GL_LUMINANCE8_SNORM
    Luminance8Alpha8Snorm                      =  0x9016, // GL_LUMINANCE8_ALPHA8_SNORM
    Intensity8Snorm                            =  0x9017, // GL_INTENSITY8_SNORM
    Alpha16Snorm                               =  0x9018, // GL_ALPHA16_SNORM
    Luminance16Snorm                           =  0x9019, // GL_LUMINANCE16_SNORM
    Luminance16Alpha16Snorm                    =  0x901A, // GL_LUMINANCE16_ALPHA16_SNORM
    Intensity16Snorm                           =  0x901B, // GL_INTENSITY16_SNORM
    FactorMin                                  =  0x901C, // GL_FACTOR_MIN_AMD
    FactorMax                                  =  0x901D, // GL_FACTOR_MAX_AMD
    DepthClampNear                             =  0x901E, // GL_DEPTH_CLAMP_NEAR_AMD
    DepthClampFar                              =  0x901F, // GL_DEPTH_CLAMP_FAR_AMD
    VideoBuffer                                =  0x9020, // GL_VIDEO_BUFFER_NV
    VideoBufferBinding                         =  0x9021, // GL_VIDEO_BUFFER_BINDING_NV
    FieldUpper                                 =  0x9022, // GL_FIELD_UPPER_NV
    FieldLower                                 =  0x9023, // GL_FIELD_LOWER_NV
    NumVideoCaptureStreams                     =  0x9024, // GL_NUM_VIDEO_CAPTURE_STREAMS_NV
    NextVideoCaptureBufferStatus               =  0x9025, // GL_NEXT_VIDEO_CAPTURE_BUFFER_STATUS_NV
    VideoCaptureTo422Supported                 =  0x9026, // GL_VIDEO_CAPTURE_TO_422_SUPPORTED_NV
    LastVideoCaptureStatus                     =  0x9027, // GL_LAST_VIDEO_CAPTURE_STATUS_NV
    VideoBufferPitch                           =  0x9028, // GL_VIDEO_BUFFER_PITCH_NV
    VideoColorConversionMatrix                 =  0x9029, // GL_VIDEO_COLOR_CONVERSION_MATRIX_NV
    VideoColorConversionMax                    =  0x902A, // GL_VIDEO_COLOR_CONVERSION_MAX_NV
    VideoColorConversionMin                    =  0x902B, // GL_VIDEO_COLOR_CONVERSION_MIN_NV
    VideoColorConversionOffset                 =  0x902C, // GL_VIDEO_COLOR_CONVERSION_OFFSET_NV
    VideoBufferInternalFormat                  =  0x902D, // GL_VIDEO_BUFFER_INTERNAL_FORMAT_NV
    PartialSuccess                             =  0x902E, // GL_PARTIAL_SUCCESS_NV
    Success                                    =  0x902F, // GL_SUCCESS_NV
    Failure                                    =  0x9030, // GL_FAILURE_NV
    YCBYCR8422                                 =  0x9031, // GL_YCBYCR8_422_NV
    YCBAYCR8A4224                              =  0x9032, // GL_YCBAYCR8A_4224_NV
    Z6Y10Z6CB10Z6Y10Z6CR10422                  =  0x9033, // GL_Z6Y10Z6CB10Z6Y10Z6CR10_422_NV
    Z6Y10Z6CB10Z6A10Z6Y10Z6CR10Z6A104224       =  0x9034, // GL_Z6Y10Z6CB10Z6A10Z6Y10Z6CR10Z6A10_4224_NV
    Z4Y12Z4CB12Z4Y12Z4CR12422                  =  0x9035, // GL_Z4Y12Z4CB12Z4Y12Z4CR12_422_NV
    Z4Y12Z4CB12Z4A12Z4Y12Z4CR12Z4A124224       =  0x9036, // GL_Z4Y12Z4CB12Z4A12Z4Y12Z4CR12Z4A12_4224_NV
    Z4Y12Z4CB12Z4CR12444                       =  0x9037, // GL_Z4Y12Z4CB12Z4CR12_444_NV
    VideoCaptureFrameWidth                     =  0x9038, // GL_VIDEO_CAPTURE_FRAME_WIDTH_NV
    VideoCaptureFrameHeight                    =  0x9039, // GL_VIDEO_CAPTURE_FRAME_HEIGHT_NV
    VideoCaptureFieldUpperHeight               =  0x903A, // GL_VIDEO_CAPTURE_FIELD_UPPER_HEIGHT_NV
    VideoCaptureFieldLowerHeight               =  0x903B, // GL_VIDEO_CAPTURE_FIELD_LOWER_HEIGHT_NV
    VideoCaptureSurfaceOrigin                  =  0x903C, // GL_VIDEO_CAPTURE_SURFACE_ORIGIN_NV
    TextureCoverageSamples                     =  0x9045, // GL_TEXTURE_COVERAGE_SAMPLES_NV
    TextureColorSamples                        =  0x9046, // GL_TEXTURE_COLOR_SAMPLES_NV
    GPUMemoryInfoDedicatedVidmem               =  0x9047, // GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX
    GPUMemoryInfoTotalAvailableMemory          =  0x9048, // GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
    GPUMemoryInfoCurrentAvailableVidmem        =  0x9049, // GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX
    GPUMemoryInfoEvictionCount                 =  0x904A, // GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX
    GPUMemoryInfoEvictedMemory                 =  0x904B, // GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX
    Image1D                                    =  0x904C, // GL_IMAGE_1D_EXT
    Image2D                                    =  0x904D, // GL_IMAGE_2D_EXT
    Image3D                                    =  0x904E, // GL_IMAGE_3D_EXT
    Image2DRect                                =  0x904F, // GL_IMAGE_2D_RECT_EXT
    ImageCube                                  =  0x9050, // GL_IMAGE_CUBE_EXT
    ImageBuffer                                =  0x9051, // GL_IMAGE_BUFFER_EXT
    Image1DArray                               =  0x9052, // GL_IMAGE_1D_ARRAY_EXT
    Image2DArray                               =  0x9053, // GL_IMAGE_2D_ARRAY_EXT
    ImageCubeMapArray                          =  0x9054, // GL_IMAGE_CUBE_MAP_ARRAY_EXT
    Image2DMultisample                         =  0x9055, // GL_IMAGE_2D_MULTISAMPLE_EXT
    Image2DMultisampleArray                    =  0x9056, // GL_IMAGE_2D_MULTISAMPLE_ARRAY_EXT
    IntImage1D                                 =  0x9057, // GL_INT_IMAGE_1D_EXT
    IntImage2D                                 =  0x9058, // GL_INT_IMAGE_2D_EXT
    IntImage3D                                 =  0x9059, // GL_INT_IMAGE_3D_EXT
    IntImage2DRect                             =  0x905A, // GL_INT_IMAGE_2D_RECT_EXT
    IntImageCube                               =  0x905B, // GL_INT_IMAGE_CUBE_EXT
    IntImageBuffer                             =  0x905C, // GL_INT_IMAGE_BUFFER_EXT
    IntImage1DArray                            =  0x905D, // GL_INT_IMAGE_1D_ARRAY_EXT
    IntImage2DArray                            =  0x905E, // GL_INT_IMAGE_2D_ARRAY_EXT
    IntImageCubeMapArray                       =  0x905F, // GL_INT_IMAGE_CUBE_MAP_ARRAY_EXT
    IntImage2DMultisample                      =  0x9060, // GL_INT_IMAGE_2D_MULTISAMPLE_EXT
    IntImage2DMultisampleArray                 =  0x9061, // GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY_EXT
    UnsignedIntImage1D                         =  0x9062, // GL_UNSIGNED_INT_IMAGE_1D_EXT
    UnsignedIntImage2D                         =  0x9063, // GL_UNSIGNED_INT_IMAGE_2D_EXT
    UnsignedIntImage3D                         =  0x9064, // GL_UNSIGNED_INT_IMAGE_3D_EXT
    UnsignedIntImage2DRect                     =  0x9065, // GL_UNSIGNED_INT_IMAGE_2D_RECT_EXT
    UnsignedIntImageCube                       =  0x9066, // GL_UNSIGNED_INT_IMAGE_CUBE_EXT
    UnsignedIntImageBuffer                     =  0x9067, // GL_UNSIGNED_INT_IMAGE_BUFFER_EXT
    UnsignedIntImage1DArray                    =  0x9068, // GL_UNSIGNED_INT_IMAGE_1D_ARRAY_EXT
    UnsignedIntImage2DArray                    =  0x9069, // GL_UNSIGNED_INT_IMAGE_2D_ARRAY_EXT
    UnsignedIntImageCubeMapArray               =  0x906A, // GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY_EXT
    UnsignedIntImage2DMultisample              =  0x906B, // GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_EXT
    UnsignedIntImage2DMultisampleArray         =  0x906C, // GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY_EXT
    MaxImageSamples                            =  0x906D, // GL_MAX_IMAGE_SAMPLES
    ImageBindingFormat                         =  0x906E, // GL_IMAGE_BINDING_FORMAT
    Square                                     =  0x90A3, // GL_SQUARE_NV
    Round                                      =  0x90A4, // GL_ROUND_NV
    Triangular                                 =  0x90A5, // GL_TRIANGULAR_NV
    Bevel                                      =  0x90A6, // GL_BEVEL_NV
    MiterRevert                                =  0x90A7, // GL_MITER_REVERT_NV
    MiterTruncate                              =  0x90A8, // GL_MITER_TRUNCATE_NV
    PathErrorPosition                          =  0x90AB, // GL_PATH_ERROR_POSITION_NV
    PathGenMode                                =  0x90B0, // GL_PATH_GEN_MODE_NV
    PathGenCoeff                               =  0x90B1, // GL_PATH_GEN_COEFF_NV
    PathGenComponents                          =  0x90B3, // GL_PATH_GEN_COMPONENTS_NV
    MoveToResets                               =  0x90B5, // GL_MOVE_TO_RESETS_NV
    MoveToContinues                            =  0x90B6, // GL_MOVE_TO_CONTINUES_NV
    PathStencilFunc                            =  0x90B7, // GL_PATH_STENCIL_FUNC_NV
    PathStencilRef                             =  0x90B8, // GL_PATH_STENCIL_REF_NV
    PathStencilValueMask                       =  0x90B9, // GL_PATH_STENCIL_VALUE_MASK_NV
    ScaleDResolveFastest                       =  0x90BA, // GL_SCALED_RESOLVE_FASTEST_EXT
    ScaleDResolveNicest                        =  0x90BB, // GL_SCALED_RESOLVE_NICEST_EXT
    PathStencilDepthOffsetFactor               =  0x90BD, // GL_PATH_STENCIL_DEPTH_OFFSET_FACTOR_NV
    PathStencilDepthOffsetUnits                =  0x90BE, // GL_PATH_STENCIL_DEPTH_OFFSET_UNITS_NV
    PathCoverDepthFunc                         =  0x90BF, // GL_PATH_COVER_DEPTH_FUNC_NV
    ImageFormatCompatibilityBySize             =  0x90C8, // GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE
    ImageFormatCompatibilityByClass            =  0x90C9, // GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS
    MaxVertexImageUniforms                     =  0x90CA, // GL_MAX_VERTEX_IMAGE_UNIFORMS
    MaxTessControlImageUniforms                =  0x90CB, // GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS
    MaxTessEvaluationImageUniforms             =  0x90CC, // GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS
    MaxGeometryImageUniforms                   =  0x90CD, // GL_MAX_GEOMETRY_IMAGE_UNIFORMS
    MaxFragmentImageUniforms                   =  0x90CE, // GL_MAX_FRAGMENT_IMAGE_UNIFORMS
    MaxCombinedImageUniforms                   =  0x90CF, // GL_MAX_COMBINED_IMAGE_UNIFORMS
    MaxDeep3DTextureWidthHeight                =  0x90D0, // GL_MAX_DEEP_3D_TEXTURE_WIDTH_HEIGHT_NV
    MaxDeep3DTextureDepth                      =  0x90D1, // GL_MAX_DEEP_3D_TEXTURE_DEPTH_NV
    MaxShaderStorageBlockSize                  =  0x90DE, // GL_MAX_SHADER_STORAGE_BLOCK_SIZE
    SyncX11Fence                               =  0x90E1, // GL_SYNC_X11_FENCE_EXT
    MaxComputeFixedGroupInvocations            =  0x90EB, // GL_MAX_COMPUTE_FIXED_GROUP_INVOCATIONS_ARB
    ContextRobustAccess                        =  0x90F3, // GL_CONTEXT_ROBUST_ACCESS
    ComputeProgramParameterBuffer              =  0x90FC, // GL_COMPUTE_PROGRAM_PARAMETER_BUFFER_NV
    TextureSamples                             =  0x9106, // GL_TEXTURE_SAMPLES
    TextureFixedSampleLocations                =  0x9107, // GL_TEXTURE_FIXED_SAMPLE_LOCATIONS
    SyncFence                                  =  0x9116, // GL_SYNC_FENCE
    Unsignaled                                 =  0x9118, // GL_UNSIGNALED
    Signaled                                   =  0x9119, // GL_SIGNALED
    UnpackCompressedBlockWidth                 =  0x9127, // GL_UNPACK_COMPRESSED_BLOCK_WIDTH
    UnpackCompressedBlockHeight                =  0x9128, // GL_UNPACK_COMPRESSED_BLOCK_HEIGHT
    UnpackCompressedBlockDepth                 =  0x9129, // GL_UNPACK_COMPRESSED_BLOCK_DEPTH
    UnpackCompressedBlockSize                  =  0x912A, // GL_UNPACK_COMPRESSED_BLOCK_SIZE
    PackCompressedBlockWidth                   =  0x912B, // GL_PACK_COMPRESSED_BLOCK_WIDTH
    PackCompressedBlockHeight                  =  0x912C, // GL_PACK_COMPRESSED_BLOCK_HEIGHT
    PackCompressedBlockDepth                   =  0x912D, // GL_PACK_COMPRESSED_BLOCK_DEPTH
    PackCompressedBlockSize                    =  0x912E, // GL_PACK_COMPRESSED_BLOCK_SIZE
    TextureImmutableFormat                     =  0x912F, // GL_TEXTURE_IMMUTABLE_FORMAT
    MaxDebugMessageLength                      =  0x9143, // GL_MAX_DEBUG_MESSAGE_LENGTH
    MaxDebugLoggedMessages                     =  0x9144, // GL_MAX_DEBUG_LOGGED_MESSAGES
    DebugLoggedMessages                        =  0x9145, // GL_DEBUG_LOGGED_MESSAGES
    DebugSeverityHigh                          =  0x9146, // GL_DEBUG_SEVERITY_HIGH_AMD
    DebugSeverityMedium                        =  0x9147, // GL_DEBUG_SEVERITY_MEDIUM_AMD
    DebugSeverityLow                           =  0x9148, // GL_DEBUG_SEVERITY_LOW_AMD
    DebugCategoryAPIError                      =  0x9149, // GL_DEBUG_CATEGORY_API_ERROR_AMD
    DebugCategoryWindowSystem                  =  0x914A, // GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD
    DebugCategoryDeprecation                   =  0x914B, // GL_DEBUG_CATEGORY_DEPRECATION_AMD
    DebugCategoryUndefinedBehavior             =  0x914C, // GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD
    DebugCategoryPerformance                   =  0x914D, // GL_DEBUG_CATEGORY_PERFORMANCE_AMD
    DebugCategoryShaderCompiler                =  0x914E, // GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD
    DebugCategoryApplication                   =  0x914F, // GL_DEBUG_CATEGORY_APPLICATION_AMD
    DebugCategoryOther                         =  0x9150, // GL_DEBUG_CATEGORY_OTHER_AMD
    BufferObjectEXT                            =  0x9151, // GL_BUFFER_OBJECT_EXT
    PerformanceMonitor                         =  0x9152, // GL_PERFORMANCE_MONITOR_AMD
    QueryObject                                =  0x9153, // GL_QUERY_OBJECT_AMD
    VertexArrayObject                          =  0x9154, // GL_VERTEX_ARRAY_OBJECT_AMD
    SamplerObject                              =  0x9155, // GL_SAMPLER_OBJECT_AMD
    ExternalVirtualMemoryBuffer                =  0x9160, // GL_EXTERNAL_VIRTUAL_MEMORY_BUFFER_AMD
    QueryBuffer                                =  0x9192, // GL_QUERY_BUFFER_AMD
    QueryBufferBinding                         =  0x9193, // GL_QUERY_BUFFER_BINDING
    QueryResultNoWait                          =  0x9194, // GL_QUERY_RESULT_NO_WAIT_AMD
    VirtualPageSizeX                           =  0x9195, // GL_VIRTUAL_PAGE_SIZE_X_AMD
    VirtualPageSizeY                           =  0x9196, // GL_VIRTUAL_PAGE_SIZE_Y_AMD
    VirtualPageSizeZ                           =  0x9197, // GL_VIRTUAL_PAGE_SIZE_Z_AMD
    MaxSparseTextureSize                       =  0x9198, // GL_MAX_SPARSE_TEXTURE_SIZE_AMD
    MaxSparse3DTextureSize                     =  0x9199, // GL_MAX_SPARSE_3D_TEXTURE_SIZE_AMD
    MaxSparseArrayTextureLayers                =  0x919A, // GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS
    MinSparseLevel                             =  0x919B, // GL_MIN_SPARSE_LEVEL_AMD
    MinLODWarning                              =  0x919C, // GL_MIN_LOD_WARNING_AMD
    TextureBufferOffset                        =  0x919D, // GL_TEXTURE_BUFFER_OFFSET
    TextureBufferSize                          =  0x919E, // GL_TEXTURE_BUFFER_SIZE
    StreamRasterization                        =  0x91A0, // GL_STREAM_RASTERIZATION_AMD
    VertexElementSwizzle                       =  0x91A4, // GL_VERTEX_ELEMENT_SWIZZLE_AMD
    VertexIDSwizzle                            =  0x91A5, // GL_VERTEX_ID_SWIZZLE_AMD
    TextureSparse                              =  0x91A6, // GL_TEXTURE_SPARSE_ARB
    VirtualPageSizeIndex                       =  0x91A7, // GL_VIRTUAL_PAGE_SIZE_INDEX_ARB
    NumVirtualPageSizes                        =  0x91A8, // GL_NUM_VIRTUAL_PAGE_SIZES_ARB
    SparseTextureFullArrayCubeMipmapS          =  0x91A9, // GL_SPARSE_TEXTURE_FULL_ARRAY_CUBE_MIPMAPS_ARB
    NumSparseLevels                            =  0x91AA, // GL_NUM_SPARSE_LEVELS_ARB
    PixelsPerSamplePatternX                    =  0x91AE, // GL_PIXELS_PER_SAMPLE_PATTERN_X_AMD
    PixelsPerSamplePatternY                    =  0x91AF, // GL_PIXELS_PER_SAMPLE_PATTERN_Y_AMD
    MaxShaderCompilerThreads                   =  0x91B0, // GL_MAX_SHADER_COMPILER_THREADS_ARB
    CompletionStatus                           =  0x91B1, // GL_COMPLETION_STATUS_ARB
    MaxColorFramebufferSamples                 =  0x91B3, // GL_MAX_COLOR_FRAMEBUFFER_SAMPLES_AMD
    MaxColorFramebufferStorageSamples          =  0x91B4, // GL_MAX_COLOR_FRAMEBUFFER_STORAGE_SAMPLES_AMD
    MaxDepthStencilFramebufferSamples          =  0x91B5, // GL_MAX_DEPTH_STENCIL_FRAMEBUFFER_SAMPLES_AMD
    NumSupportedMultisampleModes               =  0x91B6, // GL_NUM_SUPPORTED_MULTISAMPLE_MODES_AMD
    SupportedMultisampleModes                  =  0x91B7, // GL_SUPPORTED_MULTISAMPLE_MODES_AMD
    MaxComputeImageUniforms                    =  0x91BD, // GL_MAX_COMPUTE_IMAGE_UNIFORMS
    MaxComputeFixedGroupSize                   =  0x91BF, // GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB
    Float16Mat2                                =  0x91C5, // GL_FLOAT16_MAT2_AMD
    Float16Mat3                                =  0x91C6, // GL_FLOAT16_MAT3_AMD
    Float16Mat4                                =  0x91C7, // GL_FLOAT16_MAT4_AMD
    Float16Mat2x3                              =  0x91C8, // GL_FLOAT16_MAT2x3_AMD
    Float16Mat2x4                              =  0x91C9, // GL_FLOAT16_MAT2x4_AMD
    Float16Mat3x2                              =  0x91CA, // GL_FLOAT16_MAT3x2_AMD
    Float16Mat3x4                              =  0x91CB, // GL_FLOAT16_MAT3x4_AMD
    Float16Mat4x2                              =  0x91CC, // GL_FLOAT16_MAT4x2_AMD
    Float16Mat4x3                              =  0x91CD, // GL_FLOAT16_MAT4x3_AMD
    BlendPremultipliedSrc                      =  0x9280, // GL_BLEND_PREMULTIPLIED_SRC_NV
    BlendOverlap                               =  0x9281, // GL_BLEND_OVERLAP_NV
    Uncorrelated                               =  0x9282, // GL_UNCORRELATED_NV
    Disjoint                                   =  0x9283, // GL_DISJOINT_NV
    Conjoint                                   =  0x9284, // GL_CONJOINT_NV
    BlendAdvancedCoherent                      =  0x9285, // GL_BLEND_ADVANCED_COHERENT_KHR
    Src                                        =  0x9286, // GL_SRC_NV
    Dst                                        =  0x9287, // GL_DST_NV
    SrcOver                                    =  0x9288, // GL_SRC_OVER_NV
    DstOver                                    =  0x9289, // GL_DST_OVER_NV
    SrcIn                                      =  0x928A, // GL_SRC_IN_NV
    DstIn                                      =  0x928B, // GL_DST_IN_NV
    SrcOut                                     =  0x928C, // GL_SRC_OUT_NV
    DstOut                                     =  0x928D, // GL_DST_OUT_NV
    SrcAtop                                    =  0x928E, // GL_SRC_ATOP_NV
    DstAtop                                    =  0x928F, // GL_DST_ATOP_NV
    Plus                                       =  0x9291, // GL_PLUS_NV
    PlusDarker                                 =  0x9292, // GL_PLUS_DARKER_NV
    Multiply                                   =  0x9294, // GL_MULTIPLY_KHR
    Screen                                     =  0x9295, // GL_SCREEN_KHR
    Overlay                                    =  0x9296, // GL_OVERLAY_KHR
    Darken                                     =  0x9297, // GL_DARKEN_KHR
    Lighten                                    =  0x9298, // GL_LIGHTEN_KHR
    Colordodge                                 =  0x9299, // GL_COLORDODGE_KHR
    Colorburn                                  =  0x929A, // GL_COLORBURN_KHR
    Hardlight                                  =  0x929B, // GL_HARDLIGHT_KHR
    Softlight                                  =  0x929C, // GL_SOFTLIGHT_KHR
    Difference                                 =  0x929E, // GL_DIFFERENCE_KHR
    Minus                                      =  0x929F, // GL_MINUS_NV
    Exclusion                                  =  0x92A0, // GL_EXCLUSION_KHR
    Contrast                                   =  0x92A1, // GL_CONTRAST_NV
    InvertRGB                                  =  0x92A3, // GL_INVERT_RGB_NV
    Lineardodge                                =  0x92A4, // GL_LINEARDODGE_NV
    Linearburn                                 =  0x92A5, // GL_LINEARBURN_NV
    Vividlight                                 =  0x92A6, // GL_VIVIDLIGHT_NV
    Linearlight                                =  0x92A7, // GL_LINEARLIGHT_NV
    Pinlight                                   =  0x92A8, // GL_PINLIGHT_NV
    Hardmix                                    =  0x92A9, // GL_HARDMIX_NV
    HSLHue                                     =  0x92AD, // GL_HSL_HUE_KHR
    HSLSaturation                              =  0x92AE, // GL_HSL_SATURATION_KHR
    HSLColor                                   =  0x92AF, // GL_HSL_COLOR_KHR
    HSLLuminosity                              =  0x92B0, // GL_HSL_LUMINOSITY_KHR
    PlusClamped                                =  0x92B1, // GL_PLUS_CLAMPED_NV
    PlusClampedAlpha                           =  0x92B2, // GL_PLUS_CLAMPED_ALPHA_NV
    MinusClamped                               =  0x92B3, // GL_MINUS_CLAMPED_NV
    InvertOvg                                  =  0x92B4, // GL_INVERT_OVG_NV
    MaxLgpuGPUS                                =  0x92BA, // GL_MAX_LGPU_GPUS_NVX
    PurgedContextReset                         =  0x92BB, // GL_PURGED_CONTEXT_RESET_NV
    PrimitiveBoundingBox                       =  0x92BE, // GL_PRIMITIVE_BOUNDING_BOX_ARB
    AlphaToCoverageDitherMode                  =  0x92BF, // GL_ALPHA_TO_COVERAGE_DITHER_MODE_NV
    AtomicCounterBufferStart                   =  0x92C2, // GL_ATOMIC_COUNTER_BUFFER_START
    AtomicCounterBufferSize                    =  0x92C3, // GL_ATOMIC_COUNTER_BUFFER_SIZE
    MaxVertexAtomicCounterBuffers              =  0x92CC, // GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS
    MaxTessControlAtomicCounterBuffers         =  0x92CD, // GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS
    MaxTessEvaluationAtomicCounterBuffers      =  0x92CE, // GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS
    MaxGeometryAtomicCounterBuffers            =  0x92CF, // GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS
    MaxFragmentAtomicCounterBuffers            =  0x92D0, // GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS
    MaxCombinedAtomicCounterBuffers            =  0x92D1, // GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS
    MaxAtomicCounterBufferSize                 =  0x92D8, // GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE
    MaxAtomicCounterBufferBindings             =  0x92DC, // GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS
    FragmentCoverageToColor                    =  0x92DD, // GL_FRAGMENT_COVERAGE_TO_COLOR_NV
    FragmentCoverageColor                      =  0x92DE, // GL_FRAGMENT_COVERAGE_COLOR_NV
    MeshOutputPerVertexGranularity             =  0x92DF, // GL_MESH_OUTPUT_PER_VERTEX_GRANULARITY_NV
    RasterMultisample                          =  0x9327, // GL_RASTER_MULTISAMPLE_EXT
    RasterSamples                              =  0x9328, // GL_RASTER_SAMPLES_EXT
    MaxRasterSamples                           =  0x9329, // GL_MAX_RASTER_SAMPLES_EXT
    RasterFixedSampleLocations                 =  0x932A, // GL_RASTER_FIXED_SAMPLE_LOCATIONS_EXT
    MultisampleRasterizationAllowed            =  0x932B, // GL_MULTISAMPLE_RASTERIZATION_ALLOWED_EXT
    EffectiveRasterSamples                     =  0x932C, // GL_EFFECTIVE_RASTER_SAMPLES_EXT
    DepthSamples                               =  0x932D, // GL_DEPTH_SAMPLES_NV
    StencilSamples                             =  0x932E, // GL_STENCIL_SAMPLES_NV
    MixedDepthSamplesSupported                 =  0x932F, // GL_MIXED_DEPTH_SAMPLES_SUPPORTED_NV
    MixedStencilSamplesSupported               =  0x9330, // GL_MIXED_STENCIL_SAMPLES_SUPPORTED_NV
    CoverageModulationTable                    =  0x9331, // GL_COVERAGE_MODULATION_TABLE_NV
    CoverageModulation                         =  0x9332, // GL_COVERAGE_MODULATION_NV
    CoverageModulationTableSize                =  0x9333, // GL_COVERAGE_MODULATION_TABLE_SIZE_NV
    WarpSize                                   =  0x9339, // GL_WARP_SIZE_NV
    WarpsPerSm                                 =  0x933A, // GL_WARPS_PER_SM_NV
    SmCount                                    =  0x933B, // GL_SM_COUNT_NV
    FillRectangle                              =  0x933C, // GL_FILL_RECTANGLE_NV
    SampleLocationSubpixelBits                 =  0x933D, // GL_SAMPLE_LOCATION_SUBPIXEL_BITS_ARB
    SampleLocationPixelGridWidth               =  0x933E, // GL_SAMPLE_LOCATION_PIXEL_GRID_WIDTH_ARB
    SampleLocationPixelGridHeight              =  0x933F, // GL_SAMPLE_LOCATION_PIXEL_GRID_HEIGHT_ARB
    ProgrammableSampleLocationTableSize        =  0x9340, // GL_PROGRAMMABLE_SAMPLE_LOCATION_TABLE_SIZE_ARB
    ProgrammableSampleLocation                 =  0x9341, // GL_PROGRAMMABLE_SAMPLE_LOCATION_NV
    FramebufferProgrammableSampleLocations     =  0x9342, // GL_FRAMEBUFFER_PROGRAMMABLE_SAMPLE_LOCATIONS_ARB
    FramebufferSampleLocationPixelGrid         =  0x9343, // GL_FRAMEBUFFER_SAMPLE_LOCATION_PIXEL_GRID_ARB
    MaxComputeVariableGroupInvocations         =  0x9344, // GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB
    MaxComputeVariableGroupSize                =  0x9345, // GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB
    ConservativeRasterizationNV                =  0x9346, // GL_CONSERVATIVE_RASTERIZATION_NV
    SubpixelPrecisionBiasXBits                 =  0x9347, // GL_SUBPIXEL_PRECISION_BIAS_X_BITS_NV
    SubpixelPrecisionBiasYBits                 =  0x9348, // GL_SUBPIXEL_PRECISION_BIAS_Y_BITS_NV
    MaxSubpixelPrecisionBiasBits               =  0x9349, // GL_MAX_SUBPIXEL_PRECISION_BIAS_BITS_NV
    AlphaToCoverageDitherDefault               =  0x934D, // GL_ALPHA_TO_COVERAGE_DITHER_DEFAULT_NV
    AlphaToCoverageDitherEnable                =  0x934E, // GL_ALPHA_TO_COVERAGE_DITHER_ENABLE_NV
    AlphaToCoverageDitherDisable               =  0x934F, // GL_ALPHA_TO_COVERAGE_DITHER_DISABLE_NV
    ViewportSwizzlePositiveX                   =  0x9350, // GL_VIEWPORT_SWIZZLE_POSITIVE_X_NV
    ViewportSwizzleNegativeX                   =  0x9351, // GL_VIEWPORT_SWIZZLE_NEGATIVE_X_NV
    ViewportSwizzlePositiveY                   =  0x9352, // GL_VIEWPORT_SWIZZLE_POSITIVE_Y_NV
    ViewportSwizzleNegativeY                   =  0x9353, // GL_VIEWPORT_SWIZZLE_NEGATIVE_Y_NV
    ViewportSwizzlePositiveZ                   =  0x9354, // GL_VIEWPORT_SWIZZLE_POSITIVE_Z_NV
    ViewportSwizzleNegativeZ                   =  0x9355, // GL_VIEWPORT_SWIZZLE_NEGATIVE_Z_NV
    ViewportSwizzlePositiveW                   =  0x9356, // GL_VIEWPORT_SWIZZLE_POSITIVE_W_NV
    ViewportSwizzleNegativeW                   =  0x9357, // GL_VIEWPORT_SWIZZLE_NEGATIVE_W_NV
    ViewportSwizzleX                           =  0x9358, // GL_VIEWPORT_SWIZZLE_X_NV
    ViewportSwizzleY                           =  0x9359, // GL_VIEWPORT_SWIZZLE_Y_NV
    ViewportSwizzleZ                           =  0x935A, // GL_VIEWPORT_SWIZZLE_Z_NV
    ViewportSwizzleW                           =  0x935B, // GL_VIEWPORT_SWIZZLE_W_NV
    ClipOrigin                                 =  0x935C, // GL_CLIP_ORIGIN
    ClipDepthMode                              =  0x935D, // GL_CLIP_DEPTH_MODE
    TextureReductionMode                       =  0x9366, // GL_TEXTURE_REDUCTION_MODE_ARB
    WeightedAverage                            =  0x9367, // GL_WEIGHTED_AVERAGE_ARB
    FontGlyphsAvailable                        =  0x9368, // GL_FONT_GLYPHS_AVAILABLE_NV
    FontTargetUnavailable                      =  0x9369, // GL_FONT_TARGET_UNAVAILABLE_NV
    FontUnavailable                            =  0x936A, // GL_FONT_UNAVAILABLE_NV
    FontUnintelligible                         =  0x936B, // GL_FONT_UNINTELLIGIBLE_NV
    StandardFontFormat                         =  0x936C, // GL_STANDARD_FONT_FORMAT_NV
    FragmentInput                              =  0x936D, // GL_FRAGMENT_INPUT_NV
    UniformBufferUnified                       =  0x936E, // GL_UNIFORM_BUFFER_UNIFIED_NV
    UniformBufferAddress                       =  0x936F, // GL_UNIFORM_BUFFER_ADDRESS_NV
    UniformBufferLength                        =  0x9370, // GL_UNIFORM_BUFFER_LENGTH_NV
    MultisampleS                               =  0x9371, // GL_MULTISAMPLES_NV
    SupersampleScaleX                          =  0x9372, // GL_SUPERSAMPLE_SCALE_X_NV
    SupersampleScaleY                          =  0x9373, // GL_SUPERSAMPLE_SCALE_Y_NV
    Conformant                                 =  0x9374, // GL_CONFORMANT_NV
    ConservativeRasterDilate                   =  0x9379, // GL_CONSERVATIVE_RASTER_DILATE_NV
    ConservativeRasterDilateRange              =  0x937A, // GL_CONSERVATIVE_RASTER_DILATE_RANGE_NV
    ConservativeRasterDilateGranularity        =  0x937B, // GL_CONSERVATIVE_RASTER_DILATE_GRANULARITY_NV
    ViewportPositionWScale                     =  0x937C, // GL_VIEWPORT_POSITION_W_SCALE_NV
    ViewportPositionWScaleXCoeff               =  0x937D, // GL_VIEWPORT_POSITION_W_SCALE_X_COEFF_NV
    ViewportPositionWScaleYCoeff               =  0x937E, // GL_VIEWPORT_POSITION_W_SCALE_Y_COEFF_NV
    RepresentativeFragmentTest                 =  0x937F, // GL_REPRESENTATIVE_FRAGMENT_TEST_NV
    MultisampleLineWidthRange                  =  0x9381, // GL_MULTISAMPLE_LINE_WIDTH_RANGE_ARB
    MultisampleLineWidthGranularity            =  0x9382, // GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY_ARB
    ViewClassEACR11                            =  0x9383, // GL_VIEW_CLASS_EAC_R11
    ViewClassEACRG11                           =  0x9384, // GL_VIEW_CLASS_EAC_RG11
    ViewClassETC2RGB                           =  0x9385, // GL_VIEW_CLASS_ETC2_RGB
    ViewClassETC2RGBA                          =  0x9386, // GL_VIEW_CLASS_ETC2_RGBA
    ViewClassETC2EACRGBA                       =  0x9387, // GL_VIEW_CLASS_ETC2_EAC_RGBA
    ViewClassASTC4x4RGBA                       =  0x9388, // GL_VIEW_CLASS_ASTC_4x4_RGBA
    ViewClassASTC5x4RGBA                       =  0x9389, // GL_VIEW_CLASS_ASTC_5x4_RGBA
    ViewClassASTC5x5RGBA                       =  0x938A, // GL_VIEW_CLASS_ASTC_5x5_RGBA
    ViewClassASTC6x5RGBA                       =  0x938B, // GL_VIEW_CLASS_ASTC_6x5_RGBA
    ViewClassASTC6x6RGBA                       =  0x938C, // GL_VIEW_CLASS_ASTC_6x6_RGBA
    ViewClassASTC8x5RGBA                       =  0x938D, // GL_VIEW_CLASS_ASTC_8x5_RGBA
    ViewClassASTC8x6RGBA                       =  0x938E, // GL_VIEW_CLASS_ASTC_8x6_RGBA
    ViewClassASTC8x8RGBA                       =  0x938F, // GL_VIEW_CLASS_ASTC_8x8_RGBA
    ViewClassASTC10x5RGBA                      =  0x9390, // GL_VIEW_CLASS_ASTC_10x5_RGBA
    ViewClassASTC10x6RGBA                      =  0x9391, // GL_VIEW_CLASS_ASTC_10x6_RGBA
    ViewClassASTC10x8RGBA                      =  0x9392, // GL_VIEW_CLASS_ASTC_10x8_RGBA
    ViewClassASTC10x10RGBA                     =  0x9393, // GL_VIEW_CLASS_ASTC_10x10_RGBA
    ViewClassASTC12x10RGBA                     =  0x9394, // GL_VIEW_CLASS_ASTC_12x10_RGBA
    ViewClassASTC12x12RGBA                     =  0x9395, // GL_VIEW_CLASS_ASTC_12x12_RGBA
    BGRA8                                      =  0x93A1, // GL_BGRA8_EXT
    PerfqueryCounterEvent                      =  0x94F0, // GL_PERFQUERY_COUNTER_EVENT_INTEL
    PerfqueryCounterDurationNorm               =  0x94F1, // GL_PERFQUERY_COUNTER_DURATION_NORM_INTEL
    PerfqueryCounterDurationRaw                =  0x94F2, // GL_PERFQUERY_COUNTER_DURATION_RAW_INTEL
    PerfqueryCounterThroughput                 =  0x94F3, // GL_PERFQUERY_COUNTER_THROUGHPUT_INTEL
    PerfqueryCounterRaw                        =  0x94F4, // GL_PERFQUERY_COUNTER_RAW_INTEL
    PerfqueryCounterTimestamp                  =  0x94F5, // GL_PERFQUERY_COUNTER_TIMESTAMP_INTEL
    PerfqueryCounterDataUint32                 =  0x94F8, // GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL
    PerfqueryCounterDataUint64                 =  0x94F9, // GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL
    PerfqueryCounterDataFloat                  =  0x94FA, // GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL
    PerfqueryCounterDataDouble                 =  0x94FB, // GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL
    PerfqueryCounterDataBool32                 =  0x94FC, // GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL
    PerfqueryQueryNameLengthMax                =  0x94FD, // GL_PERFQUERY_QUERY_NAME_LENGTH_MAX_INTEL
    PerfqueryCounterNameLengthMax              =  0x94FE, // GL_PERFQUERY_COUNTER_NAME_LENGTH_MAX_INTEL
    PerfqueryCounterDescLengthMax              =  0x94FF, // GL_PERFQUERY_COUNTER_DESC_LENGTH_MAX_INTEL
    PerfqueryGpaExtendedCounters               =  0x9500, // GL_PERFQUERY_GPA_EXTENDED_COUNTERS_INTEL
    SubgroupSize                               =  0x9532, // GL_SUBGROUP_SIZE_KHR
    SubgroupSupportedStages                    =  0x9533, // GL_SUBGROUP_SUPPORTED_STAGES_KHR
    SubgroupSupportedFeatures                  =  0x9534, // GL_SUBGROUP_SUPPORTED_FEATURES_KHR
    SubgroupQuadAllStages                      =  0x9535, // GL_SUBGROUP_QUAD_ALL_STAGES_KHR
    MaxMeshTotalMemorySize                     =  0x9536, // GL_MAX_MESH_TOTAL_MEMORY_SIZE_NV
    MaxTaskTotalMemorySize                     =  0x9537, // GL_MAX_TASK_TOTAL_MEMORY_SIZE_NV
    MaxMeshOutputVertices                      =  0x9538, // GL_MAX_MESH_OUTPUT_VERTICES_NV
    MaxMeshOutputPrimitives                    =  0x9539, // GL_MAX_MESH_OUTPUT_PRIMITIVES_NV
    MaxTaskOutputCount                         =  0x953A, // GL_MAX_TASK_OUTPUT_COUNT_NV
    MaxMeshWorkGroupSize                       =  0x953B, // GL_MAX_MESH_WORK_GROUP_SIZE_NV
    MaxTaskWorkGroupSize                       =  0x953C, // GL_MAX_TASK_WORK_GROUP_SIZE_NV
    MaxDrawMeshTasksCount                      =  0x953D, // GL_MAX_DRAW_MESH_TASKS_COUNT_NV
    MeshWorkGroupSize                          =  0x953E, // GL_MESH_WORK_GROUP_SIZE_NV
    TaskWorkGroupSize                          =  0x953F, // GL_TASK_WORK_GROUP_SIZE_NV
    QueryResourceTypeVidmemAlloc               =  0x9540, // GL_QUERY_RESOURCE_TYPE_VIDMEM_ALLOC_NV
    QueryResourceMemtypeVidmem                 =  0x9542, // GL_QUERY_RESOURCE_MEMTYPE_VIDMEM_NV
    MeshOutputPerPrimitiveGranularity          =  0x9543, // GL_MESH_OUTPUT_PER_PRIMITIVE_GRANULARITY_NV
    QueryResourceSysReserved                   =  0x9544, // GL_QUERY_RESOURCE_SYS_RESERVED_NV
    QueryResourceTexture                       =  0x9545, // GL_QUERY_RESOURCE_TEXTURE_NV
    QueryResourceRenderbuffer                  =  0x9546, // GL_QUERY_RESOURCE_RENDERBUFFER_NV
    QueryResourceBufferobject                  =  0x9547, // GL_QUERY_RESOURCE_BUFFEROBJECT_NV
    PerGPUStorage                              =  0x9548, // GL_PER_GPU_STORAGE_NV
    MulticastProgrammableSampleLocation        =  0x9549, // GL_MULTICAST_PROGRAMMABLE_SAMPLE_LOCATION_NV
    UploadGPUMask                              =  0x954A, // GL_UPLOAD_GPU_MASK_NVX
    ConservativeRasterMode                     =  0x954D, // GL_CONSERVATIVE_RASTER_MODE_NV
    ConservativeRasterModePostSnap             =  0x954E, // GL_CONSERVATIVE_RASTER_MODE_POST_SNAP_NV
    ConservativeRasterModePreSnapTriangles     =  0x954F, // GL_CONSERVATIVE_RASTER_MODE_PRE_SNAP_TRIANGLES_NV
    ConservativeRasterModePreSnap              =  0x9550, // GL_CONSERVATIVE_RASTER_MODE_PRE_SNAP_NV
    ShaderBinaryFormatSpirV                    =  0x9551, // GL_SHADER_BINARY_FORMAT_SPIR_V_ARB
    SpirVBinary                                =  0x9552, // GL_SPIR_V_BINARY
    SpirVExtensions                            =  0x9553, // GL_SPIR_V_EXTENSIONS
    NumSpirVExtensions                         =  0x9554, // GL_NUM_SPIR_V_EXTENSIONS
    ScissorTestExclusive                       =  0x9555, // GL_SCISSOR_TEST_EXCLUSIVE_NV
    ScissorBoxExclusive                        =  0x9556, // GL_SCISSOR_BOX_EXCLUSIVE_NV
    MaxMeshViews                               =  0x9557, // GL_MAX_MESH_VIEWS_NV
    RenderGPUMask                              =  0x9558, // GL_RENDER_GPU_MASK_NV
    MeshShader                                 =  0x9559, // GL_MESH_SHADER_NV
    TaskShader                                 =  0x955A, // GL_TASK_SHADER_NV
    ShadingRateImageBinding                    =  0x955B, // GL_SHADING_RATE_IMAGE_BINDING_NV
    ShadingRateImageTexelWidth                 =  0x955C, // GL_SHADING_RATE_IMAGE_TEXEL_WIDTH_NV
    ShadingRateImageTexelHeight                =  0x955D, // GL_SHADING_RATE_IMAGE_TEXEL_HEIGHT_NV
    ShadingRateImagePaletteSize                =  0x955E, // GL_SHADING_RATE_IMAGE_PALETTE_SIZE_NV
    MaxCoarseFragmentSamples                   =  0x955F, // GL_MAX_COARSE_FRAGMENT_SAMPLES_NV
    ShadingRateImage                           =  0x9563, // GL_SHADING_RATE_IMAGE_NV
    ShadingRateNoInvocations                   =  0x9564, // GL_SHADING_RATE_NO_INVOCATIONS_NV
    ShadingRate1InvocationPerPixel             =  0x9565, // GL_SHADING_RATE_1_INVOCATION_PER_PIXEL_NV
    ShadingRate1InvocationPer1x2Pixels         =  0x9566, // GL_SHADING_RATE_1_INVOCATION_PER_1X2_PIXELS_NV
    ShadingRate1InvocationPer2x1Pixels         =  0x9567, // GL_SHADING_RATE_1_INVOCATION_PER_2X1_PIXELS_NV
    ShadingRate1InvocationPer2x2Pixels         =  0x9568, // GL_SHADING_RATE_1_INVOCATION_PER_2X2_PIXELS_NV
    ShadingRate1InvocationPer2x4Pixels         =  0x9569, // GL_SHADING_RATE_1_INVOCATION_PER_2X4_PIXELS_NV
    ShadingRate1InvocationPer4x2Pixels         =  0x956A, // GL_SHADING_RATE_1_INVOCATION_PER_4X2_PIXELS_NV
    ShadingRate1InvocationPer4x4Pixels         =  0x956B, // GL_SHADING_RATE_1_INVOCATION_PER_4X4_PIXELS_NV
    ShadingRate2InvocationsPerPixel            =  0x956C, // GL_SHADING_RATE_2_INVOCATIONS_PER_PIXEL_NV
    ShadingRate4InvocationsPerPixel            =  0x956D, // GL_SHADING_RATE_4_INVOCATIONS_PER_PIXEL_NV
    ShadingRate8InvocationsPerPixel            =  0x956E, // GL_SHADING_RATE_8_INVOCATIONS_PER_PIXEL_NV
    ShadingRate16InvocationsPerPixel           =  0x956F, // GL_SHADING_RATE_16_INVOCATIONS_PER_PIXEL_NV
    MeshVerticesOut                            =  0x9579, // GL_MESH_VERTICES_OUT_NV
    MeshPrimitivesOut                          =  0x957A, // GL_MESH_PRIMITIVES_OUT_NV
    MeshOutputType                             =  0x957B, // GL_MESH_OUTPUT_TYPE_NV
    MeshSubroutine                             =  0x957C, // GL_MESH_SUBROUTINE_NV
    TaskSubroutine                             =  0x957D, // GL_TASK_SUBROUTINE_NV
    MeshSubroutineUniform                      =  0x957E, // GL_MESH_SUBROUTINE_UNIFORM_NV
    TaskSubroutineUniform                      =  0x957F, // GL_TASK_SUBROUTINE_UNIFORM_NV
    NumTilingTypes                             =  0x9582, // GL_NUM_TILING_TYPES_EXT
    TilingTypes                                =  0x9583, // GL_TILING_TYPES_EXT
    OptimalTiling                              =  0x9584, // GL_OPTIMAL_TILING_EXT
    LinearTiling                               =  0x9585, // GL_LINEAR_TILING_EXT
    UniformBlockReferencedByMeshShader         =  0x959C, // GL_UNIFORM_BLOCK_REFERENCED_BY_MESH_SHADER_NV
    UniformBlockReferencedByTaskShader         =  0x959D, // GL_UNIFORM_BLOCK_REFERENCED_BY_TASK_SHADER_NV
    AtomicCounterBufferReferencedByMeshShader  =  0x959E, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_MESH_SHADER_NV
    AtomicCounterBufferReferencedByTaskShader  =  0x959F, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TASK_SHADER_NV
    ReferencedByMeshShader                     =  0x95A0, // GL_REFERENCED_BY_MESH_SHADER_NV
    ReferencedByTaskShader                     =  0x95A1, // GL_REFERENCED_BY_TASK_SHADER_NV
    MaxMeshWorkGroupInvocations                =  0x95A2, // GL_MAX_MESH_WORK_GROUP_INVOCATIONS_NV
    MaxTaskWorkGroupInvocations                =  0x95A3, // GL_MAX_TASK_WORK_GROUP_INVOCATIONS_NV
    AttachedMemoryObject                       =  0x95A4, // GL_ATTACHED_MEMORY_OBJECT_NV
    AttachedMemoryOffset                       =  0x95A5, // GL_ATTACHED_MEMORY_OFFSET_NV
    MemoryAttachableAlignment                  =  0x95A6, // GL_MEMORY_ATTACHABLE_ALIGNMENT_NV
    MemoryAttachableSize                       =  0x95A7, // GL_MEMORY_ATTACHABLE_SIZE_NV
    MemoryAttachable                           =  0x95A8, // GL_MEMORY_ATTACHABLE_NV
    DetachedMemoryIncarnation                  =  0x95A9, // GL_DETACHED_MEMORY_INCARNATION_NV
    DetachedTextures                           =  0x95AA, // GL_DETACHED_TEXTURES_NV
    DetachedBuffers                            =  0x95AB, // GL_DETACHED_BUFFERS_NV
    MaxDetachedTextures                        =  0x95AC, // GL_MAX_DETACHED_TEXTURES_NV
    MaxDetachedBuffers                         =  0x95AD, // GL_MAX_DETACHED_BUFFERS_NV
    ShadingRateSampleOrderDefault              =  0x95AE, // GL_SHADING_RATE_SAMPLE_ORDER_DEFAULT_NV
    ShadingRateSampleOrderPixelMajor           =  0x95AF, // GL_SHADING_RATE_SAMPLE_ORDER_PIXEL_MAJOR_NV
    ShadingRateSampleOrderSampleMajor          =  0x95B0, // GL_SHADING_RATE_SAMPLE_ORDER_SAMPLE_MAJOR_NV
    MaxViewsOvr                                =  0x9631, // GL_MAX_VIEWS_OVR
    FramebufferIncompleteViewTargetsOvr        =  0x9633, // GL_FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR
    RasterPositionUnclipped                    = 0x19262, // GL_RASTER_POSITION_UNCLIPPED_IBM
    CullVertexIBM                              =  103050, // GL_CULL_VERTEX_IBM
    AllStaticData                              =  103060, // GL_ALL_STATIC_DATA_IBM
    StaticVertexArray                          =  103061, // GL_STATIC_VERTEX_ARRAY_IBM
    VertexArrayList                            =  103070, // GL_VERTEX_ARRAY_LIST_IBM
    NormalArrayList                            =  103071, // GL_NORMAL_ARRAY_LIST_IBM
    ColorArrayList                             =  103072, // GL_COLOR_ARRAY_LIST_IBM
    IndexArrayList                             =  103073, // GL_INDEX_ARRAY_LIST_IBM
    TextureCoordArrayList                      =  103074, // GL_TEXTURE_COORD_ARRAY_LIST_IBM
    EdgeFlagArrayList                          =  103075, // GL_EDGE_FLAG_ARRAY_LIST_IBM
    FogCoordinateArrayList                     =  103076, // GL_FOG_COORDINATE_ARRAY_LIST_IBM
    SecondaryColorArrayList                    =  103077, // GL_SECONDARY_COLOR_ARRAY_LIST_IBM
    VertexArrayListStride                      =  103080, // GL_VERTEX_ARRAY_LIST_STRIDE_IBM
    NormalArrayListStride                      =  103081, // GL_NORMAL_ARRAY_LIST_STRIDE_IBM
    ColorArrayListStride                       =  103082, // GL_COLOR_ARRAY_LIST_STRIDE_IBM
    IndexArrayListStride                       =  103083, // GL_INDEX_ARRAY_LIST_STRIDE_IBM
    TextureCoordArrayListStride                =  103084, // GL_TEXTURE_COORD_ARRAY_LIST_STRIDE_IBM
    EdgeFlagArrayListStride                    =  103085, // GL_EDGE_FLAG_ARRAY_LIST_STRIDE_IBM
    FogCoordinateArrayListStride               =  103086, // GL_FOG_COORDINATE_ARRAY_LIST_STRIDE_IBM
    SecondaryColorArrayListStride              =  103087, // GL_SECONDARY_COLOR_ARRAY_LIST_STRIDE_IBM
    NativeGraphicsHandle                       = 0x1A202, // GL_NATIVE_GRAPHICS_HANDLE_PGI
}; // enum class Global

enum class GlslTypeToken : GLenum {
    Int                                  = 0x1404, // GL_INT
    UnsignedInt                          = 0x1405, // GL_UNSIGNED_INT
    Float                                = 0x1406, // GL_FLOAT
    Double                               = 0x140A, // GL_DOUBLE
    FloatVec2                            = 0x8B50, // GL_FLOAT_VEC2
    FloatVec3                            = 0x8B51, // GL_FLOAT_VEC3
    FloatVec4                            = 0x8B52, // GL_FLOAT_VEC4
    IntVec2                              = 0x8B53, // GL_INT_VEC2
    IntVec3                              = 0x8B54, // GL_INT_VEC3
    IntVec4                              = 0x8B55, // GL_INT_VEC4
    Bool                                 = 0x8B56, // GL_BOOL
    BoolVec2                             = 0x8B57, // GL_BOOL_VEC2
    BoolVec3                             = 0x8B58, // GL_BOOL_VEC3
    BoolVec4                             = 0x8B59, // GL_BOOL_VEC4
    FloatMat2                            = 0x8B5A, // GL_FLOAT_MAT2
    FloatMat3                            = 0x8B5B, // GL_FLOAT_MAT3
    FloatMat4                            = 0x8B5C, // GL_FLOAT_MAT4
    Sampler1D                            = 0x8B5D, // GL_SAMPLER_1D
    Sampler2D                            = 0x8B5E, // GL_SAMPLER_2D
    Sampler3D                            = 0x8B5F, // GL_SAMPLER_3D
    SamplerCube                          = 0x8B60, // GL_SAMPLER_CUBE
    Sampler1DShadow                      = 0x8B61, // GL_SAMPLER_1D_SHADOW
    Sampler2DShadow                      = 0x8B62, // GL_SAMPLER_2D_SHADOW
    Sampler2DRect                        = 0x8B63, // GL_SAMPLER_2D_RECT
    Sampler2DRectShadow                  = 0x8B64, // GL_SAMPLER_2D_RECT_SHADOW
    FloatMat2x3                          = 0x8B65, // GL_FLOAT_MAT2x3
    FloatMat2x4                          = 0x8B66, // GL_FLOAT_MAT2x4
    FloatMat3x2                          = 0x8B67, // GL_FLOAT_MAT3x2
    FloatMat3x4                          = 0x8B68, // GL_FLOAT_MAT3x4
    FloatMat4x2                          = 0x8B69, // GL_FLOAT_MAT4x2
    FloatMat4x3                          = 0x8B6A, // GL_FLOAT_MAT4x3
    Sampler1DArray                       = 0x8DC0, // GL_SAMPLER_1D_ARRAY
    Sampler2DArray                       = 0x8DC1, // GL_SAMPLER_2D_ARRAY
    SamplerBuffer                        = 0x8DC2, // GL_SAMPLER_BUFFER
    Sampler1DArrayShadow                 = 0x8DC3, // GL_SAMPLER_1D_ARRAY_SHADOW
    Sampler2DArrayShadow                 = 0x8DC4, // GL_SAMPLER_2D_ARRAY_SHADOW
    SamplerCubeShadow                    = 0x8DC5, // GL_SAMPLER_CUBE_SHADOW
    UnsignedIntVec2                      = 0x8DC6, // GL_UNSIGNED_INT_VEC2
    UnsignedIntVec3                      = 0x8DC7, // GL_UNSIGNED_INT_VEC3
    UnsignedIntVec4                      = 0x8DC8, // GL_UNSIGNED_INT_VEC4
    IntSampler1D                         = 0x8DC9, // GL_INT_SAMPLER_1D
    IntSampler2D                         = 0x8DCA, // GL_INT_SAMPLER_2D
    IntSampler3D                         = 0x8DCB, // GL_INT_SAMPLER_3D
    IntSamplerCube                       = 0x8DCC, // GL_INT_SAMPLER_CUBE
    IntSampler2DRect                     = 0x8DCD, // GL_INT_SAMPLER_2D_RECT
    IntSampler1DArray                    = 0x8DCE, // GL_INT_SAMPLER_1D_ARRAY
    IntSampler2DArray                    = 0x8DCF, // GL_INT_SAMPLER_2D_ARRAY
    IntSamplerBuffer                     = 0x8DD0, // GL_INT_SAMPLER_BUFFER
    UnsignedIntSampler1D                 = 0x8DD1, // GL_UNSIGNED_INT_SAMPLER_1D
    UnsignedIntSampler2D                 = 0x8DD2, // GL_UNSIGNED_INT_SAMPLER_2D
    UnsignedIntSampler3D                 = 0x8DD3, // GL_UNSIGNED_INT_SAMPLER_3D
    UnsignedIntSamplerCube               = 0x8DD4, // GL_UNSIGNED_INT_SAMPLER_CUBE
    UnsignedIntSampler2DRect             = 0x8DD5, // GL_UNSIGNED_INT_SAMPLER_2D_RECT
    UnsignedIntSampler1DArray            = 0x8DD6, // GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
    UnsignedIntSampler2DArray            = 0x8DD7, // GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
    UnsignedIntSamplerBuffer             = 0x8DD8, // GL_UNSIGNED_INT_SAMPLER_BUFFER
    DoubleMat2                           = 0x8F46, // GL_DOUBLE_MAT2
    DoubleMat3                           = 0x8F47, // GL_DOUBLE_MAT3
    DoubleMat4                           = 0x8F48, // GL_DOUBLE_MAT4
    DoubleVec2                           = 0x8FFC, // GL_DOUBLE_VEC2
    DoubleVec3                           = 0x8FFD, // GL_DOUBLE_VEC3
    DoubleVec4                           = 0x8FFE, // GL_DOUBLE_VEC4
    SamplerCubeMapArray                  = 0x900C, // GL_SAMPLER_CUBE_MAP_ARRAY
    SamplerCubeMapArrayShadow            = 0x900D, // GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW
    IntSamplerCubeMapArray               = 0x900E, // GL_INT_SAMPLER_CUBE_MAP_ARRAY
    UnsignedIntSamplerCubeMapArray       = 0x900F, // GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY
    Image1D                              = 0x904C, // GL_IMAGE_1D
    Image2D                              = 0x904D, // GL_IMAGE_2D
    Image3D                              = 0x904E, // GL_IMAGE_3D
    Image2DRect                          = 0x904F, // GL_IMAGE_2D_RECT
    ImageCube                            = 0x9050, // GL_IMAGE_CUBE
    ImageBuffer                          = 0x9051, // GL_IMAGE_BUFFER
    Image1DArray                         = 0x9052, // GL_IMAGE_1D_ARRAY
    Image2DArray                         = 0x9053, // GL_IMAGE_2D_ARRAY
    ImageCubeMapArray                    = 0x9054, // GL_IMAGE_CUBE_MAP_ARRAY
    Image2DMultisample                   = 0x9055, // GL_IMAGE_2D_MULTISAMPLE
    Image2DMultisampleArray              = 0x9056, // GL_IMAGE_2D_MULTISAMPLE_ARRAY
    IntImage1D                           = 0x9057, // GL_INT_IMAGE_1D
    IntImage2D                           = 0x9058, // GL_INT_IMAGE_2D
    IntImage3D                           = 0x9059, // GL_INT_IMAGE_3D
    IntImage2DRect                       = 0x905A, // GL_INT_IMAGE_2D_RECT
    IntImageCube                         = 0x905B, // GL_INT_IMAGE_CUBE
    IntImageBuffer                       = 0x905C, // GL_INT_IMAGE_BUFFER
    IntImage1DArray                      = 0x905D, // GL_INT_IMAGE_1D_ARRAY
    IntImage2DArray                      = 0x905E, // GL_INT_IMAGE_2D_ARRAY
    IntImageCubeMapArray                 = 0x905F, // GL_INT_IMAGE_CUBE_MAP_ARRAY
    IntImage2DMultisample                = 0x9060, // GL_INT_IMAGE_2D_MULTISAMPLE
    IntImage2DMultisampleArray           = 0x9061, // GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY
    UnsignedIntImage1D                   = 0x9062, // GL_UNSIGNED_INT_IMAGE_1D
    UnsignedIntImage2D                   = 0x9063, // GL_UNSIGNED_INT_IMAGE_2D
    UnsignedIntImage3D                   = 0x9064, // GL_UNSIGNED_INT_IMAGE_3D
    UnsignedIntImage2DRect               = 0x9065, // GL_UNSIGNED_INT_IMAGE_2D_RECT
    UnsignedIntImageCube                 = 0x9066, // GL_UNSIGNED_INT_IMAGE_CUBE
    UnsignedIntImageBuffer               = 0x9067, // GL_UNSIGNED_INT_IMAGE_BUFFER
    UnsignedIntImage1DArray              = 0x9068, // GL_UNSIGNED_INT_IMAGE_1D_ARRAY
    UnsignedIntImage2DArray              = 0x9069, // GL_UNSIGNED_INT_IMAGE_2D_ARRAY
    UnsignedIntImageCubeMapArray         = 0x906A, // GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY
    UnsignedIntImage2DMultisample        = 0x906B, // GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE
    UnsignedIntImage2DMultisampleArray   = 0x906C, // GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY
    Sampler2DMultisample                 = 0x9108, // GL_SAMPLER_2D_MULTISAMPLE
    IntSampler2DMultisample              = 0x9109, // GL_INT_SAMPLER_2D_MULTISAMPLE
    UnsignedIntSampler2DMultisample      = 0x910A, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
    Sampler2DMultisampleArray            = 0x910B, // GL_SAMPLER_2D_MULTISAMPLE_ARRAY
    IntSampler2DMultisampleArray         = 0x910C, // GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    UnsignedIntSampler2DMultisampleArray = 0x910D, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    UnsignedIntAtomicCounter             = 0x92DB, // GL_UNSIGNED_INT_ATOMIC_COUNTER
}; // enum class GlslTypeToken

enum class GraphicsResetStatus : GLenum {
    NoError              =      0, // GL_NO_ERROR
    GuiltyContextReset   = 0x8253, // GL_GUILTY_CONTEXT_RESET
    InnocentContextReset = 0x8254, // GL_INNOCENT_CONTEXT_RESET
    UnknownContextReset  = 0x8255, // GL_UNKNOWN_CONTEXT_RESET
}; // enum class GraphicsResetStatus

enum class HintMode : GLenum {
    DontCare = 0x1100, // GL_DONT_CARE
    Fastest  = 0x1101, // GL_FASTEST
    Nicest   = 0x1102, // GL_NICEST
}; // enum class HintMode

enum class HintTarget : GLenum {
    LineSmoothHint               =  0x0C52, // GL_LINE_SMOOTH_HINT
    PolygonSmoothHint            =  0x0C53, // GL_POLYGON_SMOOTH_HINT
    PackCMYKHint                 =  0x800E, // GL_PACK_CMYK_HINT_EXT
    UnpackCMYKHint               =  0x800F, // GL_UNPACK_CMYK_HINT_EXT
    PhongHintWin                 =  0x80EB, // GL_PHONG_HINT_WIN
    ClipVolumeClippingHint       =  0x80F0, // GL_CLIP_VOLUME_CLIPPING_HINT_EXT
    TextureMultiBufferHint       =  0x812E, // GL_TEXTURE_MULTI_BUFFER_HINT_SGIX
    GenerateMipmapHint           =  0x8192, // GL_GENERATE_MIPMAP_HINT_SGIS
    ProgramBinaryRetrievableHint =  0x8257, // GL_PROGRAM_BINARY_RETRIEVABLE_HINT
    ConvolutionHint              =  0x8316, // GL_CONVOLUTION_HINT_SGIX
    ScalebiasHint                =  0x8322, // GL_SCALEBIAS_HINT_SGIX
    VertexPreclip                =  0x83EE, // GL_VERTEX_PRECLIP_SGIX
    VertexPreclipHint            =  0x83EF, // GL_VERTEX_PRECLIP_HINT_SGIX
    TextureCompressionHint       =  0x84EF, // GL_TEXTURE_COMPRESSION_HINT
    VertexArrayStorageHint       =  0x851F, // GL_VERTEX_ARRAY_STORAGE_HINT_APPLE
    MultisampleFilterHint        =  0x8534, // GL_MULTISAMPLE_FILTER_HINT_NV
    TransformHint                =  0x85B1, // GL_TRANSFORM_HINT_APPLE
    TextureStorageHint           =  0x85BC, // GL_TEXTURE_STORAGE_HINT_APPLE
    FragmentShaderDerivativeHint =  0x8B8B, // GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    PreferDoublebufferHint       = 0x1A1F8, // GL_PREFER_DOUBLEBUFFER_HINT_PGI
    ConserveMemoryHint           = 0x1A1FD, // GL_CONSERVE_MEMORY_HINT_PGI
    ReclaimMemoryHint            = 0x1A1FE, // GL_RECLAIM_MEMORY_HINT_PGI
    NativeGraphicsBeginHint      = 0x1A203, // GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI
    NativeGraphicsEndHint        = 0x1A204, // GL_NATIVE_GRAPHICS_END_HINT_PGI
    AlwaysFastHint               = 0x1A20C, // GL_ALWAYS_FAST_HINT_PGI
    AlwaysSoftHint               = 0x1A20D, // GL_ALWAYS_SOFT_HINT_PGI
    AllowDrawObjHint             = 0x1A20E, // GL_ALLOW_DRAW_OBJ_HINT_PGI
    AllowDrawWinHint             = 0x1A20F, // GL_ALLOW_DRAW_WIN_HINT_PGI
    AllowDrawFrgHint             = 0x1A210, // GL_ALLOW_DRAW_FRG_HINT_PGI
    AllowDrawMemHint             = 0x1A211, // GL_ALLOW_DRAW_MEM_HINT_PGI
    StrictDepthfuncHint          = 0x1A216, // GL_STRICT_DEPTHFUNC_HINT_PGI
    StrictLightingHint           = 0x1A217, // GL_STRICT_LIGHTING_HINT_PGI
    StrictScissorHint            = 0x1A218, // GL_STRICT_SCISSOR_HINT_PGI
    FullStippleHint              = 0x1A219, // GL_FULL_STIPPLE_HINT_PGI
    ClipNearHint                 = 0x1A220, // GL_CLIP_NEAR_HINT_PGI
    ClipFarHint                  = 0x1A221, // GL_CLIP_FAR_HINT_PGI
    WideLineHint                 = 0x1A222, // GL_WIDE_LINE_HINT_PGI
    BackNormalSHint              = 0x1A223, // GL_BACK_NORMALS_HINT_PGI
    VertexDataHint               = 0x1A22A, // GL_VERTEX_DATA_HINT_PGI
    VertexConsistentHint         = 0x1A22B, // GL_VERTEX_CONSISTENT_HINT_PGI
    MaterialSideHint             = 0x1A22C, // GL_MATERIAL_SIDE_HINT_PGI
    MaxVertexHint                = 0x1A22D, // GL_MAX_VERTEX_HINT_PGI
}; // enum class HintTarget

enum class HintTargetPGI : GLenum {
    VertexDataHint       = 0x1A22A, // GL_VERTEX_DATA_HINT_PGI
    VertexConsistentHint = 0x1A22B, // GL_VERTEX_CONSISTENT_HINT_PGI
    MaterialSideHint     = 0x1A22C, // GL_MATERIAL_SIDE_HINT_PGI
    MaxVertexHint        = 0x1A22D, // GL_MAX_VERTEX_HINT_PGI
}; // enum class HintTargetPGI

enum class HistogramTargetEXT : GLenum {
    Histogram      = 0x8024, // GL_HISTOGRAM_EXT
    ProxyHistogram = 0x8025, // GL_PROXY_HISTOGRAM_EXT
}; // enum class HistogramTargetEXT

enum class ImageTransformPNameHP : GLenum {
    ImageScaleX        = 0x8155, // GL_IMAGE_SCALE_X_HP
    ImageScaleY        = 0x8156, // GL_IMAGE_SCALE_Y_HP
    ImageTranslateX    = 0x8157, // GL_IMAGE_TRANSLATE_X_HP
    ImageTranslateY    = 0x8158, // GL_IMAGE_TRANSLATE_Y_HP
    ImageRotateAngle   = 0x8159, // GL_IMAGE_ROTATE_ANGLE_HP
    ImageRotateOriginX = 0x815A, // GL_IMAGE_ROTATE_ORIGIN_X_HP
    ImageRotateOriginY = 0x815B, // GL_IMAGE_ROTATE_ORIGIN_Y_HP
    ImageMagFilter     = 0x815C, // GL_IMAGE_MAG_FILTER_HP
    ImageMinFilter     = 0x815D, // GL_IMAGE_MIN_FILTER_HP
    ImageCubicWeight   = 0x815E, // GL_IMAGE_CUBIC_WEIGHT_HP
}; // enum class ImageTransformPNameHP

enum class ImageTransformTargetHP : GLenum {
    ImageTransform2D = 0x8161, // GL_IMAGE_TRANSFORM_2D_HP
}; // enum class ImageTransformTargetHP

enum class IndexFunctionEXT : GLenum {
    Never    = 0x0200, // GL_NEVER
    Less     = 0x0201, // GL_LESS
    Equal    = 0x0202, // GL_EQUAL
    Lequal   = 0x0203, // GL_LEQUAL
    Greater  = 0x0204, // GL_GREATER
    Notequal = 0x0205, // GL_NOTEQUAL
    Gequal   = 0x0206, // GL_GEQUAL
    Always   = 0x0207, // GL_ALWAYS
}; // enum class IndexFunctionEXT

enum class IndexPointerType : GLenum {
    Short  = 0x1402, // GL_SHORT
    Int    = 0x1404, // GL_INT
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class IndexPointerType

enum class InternalFormat : GLint {
    StencilIndex                          = 0x1901, // GL_STENCIL_INDEX
    DepthComponent                        = 0x1902, // GL_DEPTH_COMPONENT
    Red                                   = 0x1903, // GL_RED
    RGB                                   = 0x1907, // GL_RGB
    RGBA                                  = 0x1908, // GL_RGBA
    R3G3B2                                = 0x2A10, // GL_R3_G3_B2
    Alpha4                                = 0x803B, // GL_ALPHA4_EXT
    Alpha8                                = 0x803C, // GL_ALPHA8_EXT
    Alpha12                               = 0x803D, // GL_ALPHA12_EXT
    Alpha16                               = 0x803E, // GL_ALPHA16_EXT
    Luminance4                            = 0x803F, // GL_LUMINANCE4_EXT
    Luminance8                            = 0x8040, // GL_LUMINANCE8_EXT
    Luminance12                           = 0x8041, // GL_LUMINANCE12_EXT
    Luminance16                           = 0x8042, // GL_LUMINANCE16_EXT
    Luminance4Alpha4                      = 0x8043, // GL_LUMINANCE4_ALPHA4_EXT
    Luminance6Alpha2                      = 0x8044, // GL_LUMINANCE6_ALPHA2_EXT
    Luminance8Alpha8                      = 0x8045, // GL_LUMINANCE8_ALPHA8_EXT
    Luminance12Alpha4                     = 0x8046, // GL_LUMINANCE12_ALPHA4_EXT
    Luminance12Alpha12                    = 0x8047, // GL_LUMINANCE12_ALPHA12_EXT
    Luminance16Alpha16                    = 0x8048, // GL_LUMINANCE16_ALPHA16_EXT
    Intensity4                            = 0x804A, // GL_INTENSITY4_EXT
    Intensity8                            = 0x804B, // GL_INTENSITY8_EXT
    Intensity12                           = 0x804C, // GL_INTENSITY12_EXT
    Intensity16                           = 0x804D, // GL_INTENSITY16_EXT
    RGB2                                  = 0x804E, // GL_RGB2_EXT
    RGB4                                  = 0x804F, // GL_RGB4
    RGB5                                  = 0x8050, // GL_RGB5
    RGB8                                  = 0x8051, // GL_RGB8
    RGB10                                 = 0x8052, // GL_RGB10
    RGB12                                 = 0x8053, // GL_RGB12
    RGB16                                 = 0x8054, // GL_RGB16
    RGBA2                                 = 0x8055, // GL_RGBA2
    RGBA4                                 = 0x8056, // GL_RGBA4
    RGB5A1                                = 0x8057, // GL_RGB5_A1
    RGBA8                                 = 0x8058, // GL_RGBA8
    RGB10A2                               = 0x8059, // GL_RGB10_A2
    RGBA12                                = 0x805A, // GL_RGBA12
    RGBA16                                = 0x805B, // GL_RGBA16
    DualAlpha4                            = 0x8110, // GL_DUAL_ALPHA4_SGIS
    DualAlpha8                            = 0x8111, // GL_DUAL_ALPHA8_SGIS
    DualAlpha12                           = 0x8112, // GL_DUAL_ALPHA12_SGIS
    DualAlpha16                           = 0x8113, // GL_DUAL_ALPHA16_SGIS
    DualLuminance4                        = 0x8114, // GL_DUAL_LUMINANCE4_SGIS
    DualLuminance8                        = 0x8115, // GL_DUAL_LUMINANCE8_SGIS
    DualLuminance12                       = 0x8116, // GL_DUAL_LUMINANCE12_SGIS
    DualLuminance16                       = 0x8117, // GL_DUAL_LUMINANCE16_SGIS
    DualIntensity4                        = 0x8118, // GL_DUAL_INTENSITY4_SGIS
    DualIntensity8                        = 0x8119, // GL_DUAL_INTENSITY8_SGIS
    DualIntensity12                       = 0x811A, // GL_DUAL_INTENSITY12_SGIS
    DualIntensity16                       = 0x811B, // GL_DUAL_INTENSITY16_SGIS
    DualLuminanceAlpha4                   = 0x811C, // GL_DUAL_LUMINANCE_ALPHA4_SGIS
    DualLuminanceAlpha8                   = 0x811D, // GL_DUAL_LUMINANCE_ALPHA8_SGIS
    QuadAlpha4                            = 0x811E, // GL_QUAD_ALPHA4_SGIS
    QuadAlpha8                            = 0x811F, // GL_QUAD_ALPHA8_SGIS
    QuadLuminance4                        = 0x8120, // GL_QUAD_LUMINANCE4_SGIS
    QuadLuminance8                        = 0x8121, // GL_QUAD_LUMINANCE8_SGIS
    QuadIntensity4                        = 0x8122, // GL_QUAD_INTENSITY4_SGIS
    QuadIntensity8                        = 0x8123, // GL_QUAD_INTENSITY8_SGIS
    DepthComponent16                      = 0x81A5, // GL_DEPTH_COMPONENT16
    DepthComponent24                      = 0x81A6, // GL_DEPTH_COMPONENT24
    DepthComponent32                      = 0x81A7, // GL_DEPTH_COMPONENT32
    CompressedRed                         = 0x8225, // GL_COMPRESSED_RED
    CompressedRg                          = 0x8226, // GL_COMPRESSED_RG
    Rg                                    = 0x8227, // GL_RG
    R8                                    = 0x8229, // GL_R8
    R16                                   = 0x822A, // GL_R16
    RG8                                   = 0x822B, // GL_RG8
    RG16                                  = 0x822C, // GL_RG16
    R16F                                  = 0x822D, // GL_R16F
    R32F                                  = 0x822E, // GL_R32F
    RG16F                                 = 0x822F, // GL_RG16F
    RG32F                                 = 0x8230, // GL_RG32F
    R8I                                   = 0x8231, // GL_R8I
    R8UI                                  = 0x8232, // GL_R8UI
    R16I                                  = 0x8233, // GL_R16I
    R16UI                                 = 0x8234, // GL_R16UI
    R32I                                  = 0x8235, // GL_R32I
    R32UI                                 = 0x8236, // GL_R32UI
    RG8I                                  = 0x8237, // GL_RG8I
    RG8UI                                 = 0x8238, // GL_RG8UI
    RG16I                                 = 0x8239, // GL_RG16I
    RG16UI                                = 0x823A, // GL_RG16UI
    RG32I                                 = 0x823B, // GL_RG32I
    RG32UI                                = 0x823C, // GL_RG32UI
    CompressedRGBS3TcDXT1                 = 0x83F0, // GL_COMPRESSED_RGB_S3TC_DXT1_EXT
    CompressedRGBAS3TcDXT1                = 0x83F1, // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    CompressedRGBAS3TcDXT3                = 0x83F2, // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    CompressedRGBAS3TcDXT5                = 0x83F3, // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    CompressedRGB                         = 0x84ED, // GL_COMPRESSED_RGB
    CompressedRGBA                        = 0x84EE, // GL_COMPRESSED_RGBA
    DepthStencil                          = 0x84F9, // GL_DEPTH_STENCIL
    RGBA32F                               = 0x8814, // GL_RGBA32F
    RGB32F                                = 0x8815, // GL_RGB32F
    RGBA16F                               = 0x881A, // GL_RGBA16F
    RGB16F                                = 0x881B, // GL_RGB16F
    Depth24Stencil8                       = 0x88F0, // GL_DEPTH24_STENCIL8
    R11FG11FB10F                          = 0x8C3A, // GL_R11F_G11F_B10F
    RGB9E5                                = 0x8C3D, // GL_RGB9_E5
    SRGB                                  = 0x8C40, // GL_SRGB
    SRGB8                                 = 0x8C41, // GL_SRGB8
    SRGBAlpha                             = 0x8C42, // GL_SRGB_ALPHA
    SRGB8Alpha8                           = 0x8C43, // GL_SRGB8_ALPHA8
    CompressedSRGB                        = 0x8C48, // GL_COMPRESSED_SRGB
    CompressedSRGBAlpha                   = 0x8C49, // GL_COMPRESSED_SRGB_ALPHA
    CompressedSRGBS3TcDXT1                = 0x8C4C, // GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
    CompressedSRGBAlphaS3TcDXT1           = 0x8C4D, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
    CompressedSRGBAlphaS3TcDXT3           = 0x8C4E, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
    CompressedSRGBAlphaS3TcDXT5           = 0x8C4F, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
    DepthComponent32F                     = 0x8CAC, // GL_DEPTH_COMPONENT32F
    Depth32FStencil8                      = 0x8CAD, // GL_DEPTH32F_STENCIL8
    StencilIndex1                         = 0x8D46, // GL_STENCIL_INDEX1
    StencilIndex4                         = 0x8D47, // GL_STENCIL_INDEX4
    StencilIndex8                         = 0x8D48, // GL_STENCIL_INDEX8
    StencilIndex16                        = 0x8D49, // GL_STENCIL_INDEX16
    RGB565                                = 0x8D62, // GL_RGB565
    RGBA32UI                              = 0x8D70, // GL_RGBA32UI
    RGB32UI                               = 0x8D71, // GL_RGB32UI
    Alpha32UI                             = 0x8D72, // GL_ALPHA32UI_EXT
    Intensity32UI                         = 0x8D73, // GL_INTENSITY32UI_EXT
    Luminance32UI                         = 0x8D74, // GL_LUMINANCE32UI_EXT
    LuminanceAlpha32UI                    = 0x8D75, // GL_LUMINANCE_ALPHA32UI_EXT
    RGBA16UI                              = 0x8D76, // GL_RGBA16UI
    RGB16UI                               = 0x8D77, // GL_RGB16UI
    Alpha16UI                             = 0x8D78, // GL_ALPHA16UI_EXT
    Intensity16UI                         = 0x8D79, // GL_INTENSITY16UI_EXT
    Luminance16UI                         = 0x8D7A, // GL_LUMINANCE16UI_EXT
    LuminanceAlpha16UI                    = 0x8D7B, // GL_LUMINANCE_ALPHA16UI_EXT
    RGBA8UI                               = 0x8D7C, // GL_RGBA8UI
    RGB8UI                                = 0x8D7D, // GL_RGB8UI
    Alpha8UI                              = 0x8D7E, // GL_ALPHA8UI_EXT
    Intensity8UI                          = 0x8D7F, // GL_INTENSITY8UI_EXT
    Luminance8UI                          = 0x8D80, // GL_LUMINANCE8UI_EXT
    LuminanceAlpha8UI                     = 0x8D81, // GL_LUMINANCE_ALPHA8UI_EXT
    RGBA32I                               = 0x8D82, // GL_RGBA32I
    RGB32I                                = 0x8D83, // GL_RGB32I
    Alpha32I                              = 0x8D84, // GL_ALPHA32I_EXT
    Intensity32I                          = 0x8D85, // GL_INTENSITY32I_EXT
    Luminance32I                          = 0x8D86, // GL_LUMINANCE32I_EXT
    LuminanceAlpha32I                     = 0x8D87, // GL_LUMINANCE_ALPHA32I_EXT
    RGBA16I                               = 0x8D88, // GL_RGBA16I
    RGB16I                                = 0x8D89, // GL_RGB16I
    Alpha16I                              = 0x8D8A, // GL_ALPHA16I_EXT
    Intensity16I                          = 0x8D8B, // GL_INTENSITY16I_EXT
    Luminance16I                          = 0x8D8C, // GL_LUMINANCE16I_EXT
    LuminanceAlpha16I                     = 0x8D8D, // GL_LUMINANCE_ALPHA16I_EXT
    RGBA8I                                = 0x8D8E, // GL_RGBA8I
    RGB8I                                 = 0x8D8F, // GL_RGB8I
    Alpha8I                               = 0x8D90, // GL_ALPHA8I_EXT
    Intensity8I                           = 0x8D91, // GL_INTENSITY8I_EXT
    Luminance8I                           = 0x8D92, // GL_LUMINANCE8I_EXT
    LuminanceAlpha8I                      = 0x8D93, // GL_LUMINANCE_ALPHA8I_EXT
    DepthComponent32FNV                   = 0x8DAB, // GL_DEPTH_COMPONENT32F_NV
    Depth32FStencil8NV                    = 0x8DAC, // GL_DEPTH32F_STENCIL8_NV
    CompressedRedRGTC1                    = 0x8DBB, // GL_COMPRESSED_RED_RGTC1
    CompressedSignedRedRGTC1              = 0x8DBC, // GL_COMPRESSED_SIGNED_RED_RGTC1
    CompressedRedGreenRGTC2               = 0x8DBD, // GL_COMPRESSED_RED_GREEN_RGTC2_EXT
    CompressedSignedRedGreenRGTC2         = 0x8DBE, // GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT
    CompressedRGBABPTCUnorm               = 0x8E8C, // GL_COMPRESSED_RGBA_BPTC_UNORM
    CompressedSRGBAlphaBPTCUnorm          = 0x8E8D, // GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM
    CompressedRGBBPTCSignedFloat          = 0x8E8E, // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT
    CompressedRGBBPTCUnsignedFloat        = 0x8E8F, // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
    R8Snorm                               = 0x8F94, // GL_R8_SNORM
    RG8Snorm                              = 0x8F95, // GL_RG8_SNORM
    RGB8Snorm                             = 0x8F96, // GL_RGB8_SNORM
    RGBA8Snorm                            = 0x8F97, // GL_RGBA8_SNORM
    R16Snorm                              = 0x8F98, // GL_R16_SNORM
    RG16Snorm                             = 0x8F99, // GL_RG16_SNORM
    RGB16Snorm                            = 0x8F9A, // GL_RGB16_SNORM
    RGBA16Snorm                           = 0x8F9B, // GL_RGBA16_SNORM
    SR8                                   = 0x8FBD, // GL_SR8_EXT
    SRG8                                  = 0x8FBE, // GL_SRG8_EXT
    RGB10A2UI                             = 0x906F, // GL_RGB10_A2UI
    CompressedR11EAC                      = 0x9270, // GL_COMPRESSED_R11_EAC
    CompressedSignedR11EAC                = 0x9271, // GL_COMPRESSED_SIGNED_R11_EAC
    CompressedRG11EAC                     = 0x9272, // GL_COMPRESSED_RG11_EAC
    CompressedSignedRG11EAC               = 0x9273, // GL_COMPRESSED_SIGNED_RG11_EAC
    CompressedRGB8ETC2                    = 0x9274, // GL_COMPRESSED_RGB8_ETC2
    CompressedSRGB8ETC2                   = 0x9275, // GL_COMPRESSED_SRGB8_ETC2
    CompressedRGB8PunchthroughAlpha1ETC2  = 0x9276, // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    CompressedSRGB8PunchthroughAlpha1ETC2 = 0x9277, // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    CompressedRGBA8ETC2EAC                = 0x9278, // GL_COMPRESSED_RGBA8_ETC2_EAC
    CompressedSRGB8Alpha8ETC2EAC          = 0x9279, // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    CompressedRGBAASTC4x4                 = 0x93B0, // GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    CompressedRGBAASTC5x4                 = 0x93B1, // GL_COMPRESSED_RGBA_ASTC_5x4_KHR
    CompressedRGBAASTC5x5                 = 0x93B2, // GL_COMPRESSED_RGBA_ASTC_5x5_KHR
    CompressedRGBAASTC6x5                 = 0x93B3, // GL_COMPRESSED_RGBA_ASTC_6x5_KHR
    CompressedRGBAASTC6x6                 = 0x93B4, // GL_COMPRESSED_RGBA_ASTC_6x6_KHR
    CompressedRGBAASTC8x5                 = 0x93B5, // GL_COMPRESSED_RGBA_ASTC_8x5_KHR
    CompressedRGBAASTC8x6                 = 0x93B6, // GL_COMPRESSED_RGBA_ASTC_8x6_KHR
    CompressedRGBAASTC8x8                 = 0x93B7, // GL_COMPRESSED_RGBA_ASTC_8x8_KHR
    CompressedRGBAASTC10x5                = 0x93B8, // GL_COMPRESSED_RGBA_ASTC_10x5_KHR
    CompressedRGBAASTC10x6                = 0x93B9, // GL_COMPRESSED_RGBA_ASTC_10x6_KHR
    CompressedRGBAASTC10x8                = 0x93BA, // GL_COMPRESSED_RGBA_ASTC_10x8_KHR
    CompressedRGBAASTC10x10               = 0x93BB, // GL_COMPRESSED_RGBA_ASTC_10x10_KHR
    CompressedRGBAASTC12x10               = 0x93BC, // GL_COMPRESSED_RGBA_ASTC_12x10_KHR
    CompressedRGBAASTC12x12               = 0x93BD, // GL_COMPRESSED_RGBA_ASTC_12x12_KHR
    CompressedSRGB8Alpha8ASTC4x4          = 0x93D0, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    CompressedSRGB8Alpha8ASTC5x4          = 0x93D1, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR
    CompressedSRGB8Alpha8ASTC5x5          = 0x93D2, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR
    CompressedSRGB8Alpha8ASTC6x5          = 0x93D3, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR
    CompressedSRGB8Alpha8ASTC6x6          = 0x93D4, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR
    CompressedSRGB8Alpha8ASTC8x5          = 0x93D5, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR
    CompressedSRGB8Alpha8ASTC8x6          = 0x93D6, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR
    CompressedSRGB8Alpha8ASTC8x8          = 0x93D7, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR
    CompressedSRGB8Alpha8ASTC10x5         = 0x93D8, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR
    CompressedSRGB8Alpha8ASTC10x6         = 0x93D9, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR
    CompressedSRGB8Alpha8ASTC10x8         = 0x93DA, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR
    CompressedSRGB8Alpha8ASTC10x10        = 0x93DB, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR
    CompressedSRGB8Alpha8ASTC12x10        = 0x93DC, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR
    CompressedSRGB8Alpha8ASTC12x12        = 0x93DD, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR
}; // enum class InternalFormat

enum class InternalFormatPName : GLenum {
    Samples                            = 0x80A9, // GL_SAMPLES
    InternalformatSupported            = 0x826F, // GL_INTERNALFORMAT_SUPPORTED
    InternalformatPreferred            = 0x8270, // GL_INTERNALFORMAT_PREFERRED
    InternalformatRedSize              = 0x8271, // GL_INTERNALFORMAT_RED_SIZE
    InternalformatGreenSize            = 0x8272, // GL_INTERNALFORMAT_GREEN_SIZE
    InternalformatBlueSize             = 0x8273, // GL_INTERNALFORMAT_BLUE_SIZE
    InternalformatAlphaSize            = 0x8274, // GL_INTERNALFORMAT_ALPHA_SIZE
    InternalformatDepthSize            = 0x8275, // GL_INTERNALFORMAT_DEPTH_SIZE
    InternalformatStencilSize          = 0x8276, // GL_INTERNALFORMAT_STENCIL_SIZE
    InternalformatSharedSize           = 0x8277, // GL_INTERNALFORMAT_SHARED_SIZE
    InternalformatRedType              = 0x8278, // GL_INTERNALFORMAT_RED_TYPE
    InternalformatGreenType            = 0x8279, // GL_INTERNALFORMAT_GREEN_TYPE
    InternalformatBlueType             = 0x827A, // GL_INTERNALFORMAT_BLUE_TYPE
    InternalformatAlphaType            = 0x827B, // GL_INTERNALFORMAT_ALPHA_TYPE
    InternalformatDepthType            = 0x827C, // GL_INTERNALFORMAT_DEPTH_TYPE
    InternalformatStencilType          = 0x827D, // GL_INTERNALFORMAT_STENCIL_TYPE
    MaxWidth                           = 0x827E, // GL_MAX_WIDTH
    MaxHeight                          = 0x827F, // GL_MAX_HEIGHT
    MaxDepth                           = 0x8280, // GL_MAX_DEPTH
    MaxLayers                          = 0x8281, // GL_MAX_LAYERS
    ColorComponents                    = 0x8283, // GL_COLOR_COMPONENTS
    ColorRenderable                    = 0x8286, // GL_COLOR_RENDERABLE
    DepthRenderable                    = 0x8287, // GL_DEPTH_RENDERABLE
    StencilRenderable                  = 0x8288, // GL_STENCIL_RENDERABLE
    FramebufferRenderable              = 0x8289, // GL_FRAMEBUFFER_RENDERABLE
    FramebufferRenderableLayered       = 0x828A, // GL_FRAMEBUFFER_RENDERABLE_LAYERED
    FramebufferBlend                   = 0x828B, // GL_FRAMEBUFFER_BLEND
    ReadPixels                         = 0x828C, // GL_READ_PIXELS
    ReadPixelsFormat                   = 0x828D, // GL_READ_PIXELS_FORMAT
    ReadPixelsType                     = 0x828E, // GL_READ_PIXELS_TYPE
    TextureImageFormat                 = 0x828F, // GL_TEXTURE_IMAGE_FORMAT
    TextureImageType                   = 0x8290, // GL_TEXTURE_IMAGE_TYPE
    GetTextureImageFormat              = 0x8291, // GL_GET_TEXTURE_IMAGE_FORMAT
    GetTextureImageType                = 0x8292, // GL_GET_TEXTURE_IMAGE_TYPE
    Mipmap                             = 0x8293, // GL_MIPMAP
    AutoGenerateMipmap                 = 0x8295, // GL_AUTO_GENERATE_MIPMAP - Should be deprecated
    ColorEncoding                      = 0x8296, // GL_COLOR_ENCODING
    SRGBRead                           = 0x8297, // GL_SRGB_READ
    SRGBWrite                          = 0x8298, // GL_SRGB_WRITE
    Filter                             = 0x829A, // GL_FILTER
    VertexTexture                      = 0x829B, // GL_VERTEX_TEXTURE
    TessControlTexture                 = 0x829C, // GL_TESS_CONTROL_TEXTURE
    TessEvaluationTexture              = 0x829D, // GL_TESS_EVALUATION_TEXTURE
    GeometryTexture                    = 0x829E, // GL_GEOMETRY_TEXTURE
    FragmentTexture                    = 0x829F, // GL_FRAGMENT_TEXTURE
    ComputeTexture                     = 0x82A0, // GL_COMPUTE_TEXTURE
    TextureShadow                      = 0x82A1, // GL_TEXTURE_SHADOW
    TextureGather                      = 0x82A2, // GL_TEXTURE_GATHER
    TextureGatherShadow                = 0x82A3, // GL_TEXTURE_GATHER_SHADOW
    ShaderImageLoad                    = 0x82A4, // GL_SHADER_IMAGE_LOAD
    ShaderImageStore                   = 0x82A5, // GL_SHADER_IMAGE_STORE
    ShaderImageAtomic                  = 0x82A6, // GL_SHADER_IMAGE_ATOMIC
    ImageTexelSize                     = 0x82A7, // GL_IMAGE_TEXEL_SIZE
    ImageCompatibilityClass            = 0x82A8, // GL_IMAGE_COMPATIBILITY_CLASS
    ImagePixelFormat                   = 0x82A9, // GL_IMAGE_PIXEL_FORMAT
    ImagePixelType                     = 0x82AA, // GL_IMAGE_PIXEL_TYPE
    SimultaneousTextureAndDepthTest    = 0x82AC, // GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST
    SimultaneousTextureAndStencilTest  = 0x82AD, // GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST
    SimultaneousTextureAndDepthWrite   = 0x82AE, // GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE
    SimultaneousTextureAndStencilWrite = 0x82AF, // GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE
    TextureCompressedBlockWidth        = 0x82B1, // GL_TEXTURE_COMPRESSED_BLOCK_WIDTH
    TextureCompressedBlockHeight       = 0x82B2, // GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT
    TextureCompressedBlockSize         = 0x82B3, // GL_TEXTURE_COMPRESSED_BLOCK_SIZE
    ClearBuffer                        = 0x82B4, // GL_CLEAR_BUFFER
    TextureView                        = 0x82B5, // GL_TEXTURE_VIEW
    ViewCompatibilityClass             = 0x82B6, // GL_VIEW_COMPATIBILITY_CLASS
    TextureCompressed                  = 0x86A1, // GL_TEXTURE_COMPRESSED
    ImageFormatCompatibilityType       = 0x90C7, // GL_IMAGE_FORMAT_COMPATIBILITY_TYPE
    ClearTexture                       = 0x9365, // GL_CLEAR_TEXTURE
    NumSampleCounts                    = 0x9380, // GL_NUM_SAMPLE_COUNTS
}; // enum class InternalFormatPName

enum class InvalidateFramebufferAttachment : GLenum {
    Color                  = 0x1800, // GL_COLOR
    Depth                  = 0x1801, // GL_DEPTH
    Stencil                = 0x1802, // GL_STENCIL
    DepthStencilAttachment = 0x821A, // GL_DEPTH_STENCIL_ATTACHMENT
    ColorAttachment0       = 0x8CE0, // GL_COLOR_ATTACHMENT0
    ColorAttachment1       = 0x8CE1, // GL_COLOR_ATTACHMENT1
    ColorAttachment2       = 0x8CE2, // GL_COLOR_ATTACHMENT2
    ColorAttachment3       = 0x8CE3, // GL_COLOR_ATTACHMENT3
    ColorAttachment4       = 0x8CE4, // GL_COLOR_ATTACHMENT4
    ColorAttachment5       = 0x8CE5, // GL_COLOR_ATTACHMENT5
    ColorAttachment6       = 0x8CE6, // GL_COLOR_ATTACHMENT6
    ColorAttachment7       = 0x8CE7, // GL_COLOR_ATTACHMENT7
    ColorAttachment8       = 0x8CE8, // GL_COLOR_ATTACHMENT8
    ColorAttachment9       = 0x8CE9, // GL_COLOR_ATTACHMENT9
    ColorAttachment10      = 0x8CEA, // GL_COLOR_ATTACHMENT10
    ColorAttachment11      = 0x8CEB, // GL_COLOR_ATTACHMENT11
    ColorAttachment12      = 0x8CEC, // GL_COLOR_ATTACHMENT12
    ColorAttachment13      = 0x8CED, // GL_COLOR_ATTACHMENT13
    ColorAttachment14      = 0x8CEE, // GL_COLOR_ATTACHMENT14
    ColorAttachment15      = 0x8CEF, // GL_COLOR_ATTACHMENT15
    ColorAttachment16      = 0x8CF0, // GL_COLOR_ATTACHMENT16
    ColorAttachment17      = 0x8CF1, // GL_COLOR_ATTACHMENT17
    ColorAttachment18      = 0x8CF2, // GL_COLOR_ATTACHMENT18
    ColorAttachment19      = 0x8CF3, // GL_COLOR_ATTACHMENT19
    ColorAttachment20      = 0x8CF4, // GL_COLOR_ATTACHMENT20
    ColorAttachment21      = 0x8CF5, // GL_COLOR_ATTACHMENT21
    ColorAttachment22      = 0x8CF6, // GL_COLOR_ATTACHMENT22
    ColorAttachment23      = 0x8CF7, // GL_COLOR_ATTACHMENT23
    ColorAttachment24      = 0x8CF8, // GL_COLOR_ATTACHMENT24
    ColorAttachment25      = 0x8CF9, // GL_COLOR_ATTACHMENT25
    ColorAttachment26      = 0x8CFA, // GL_COLOR_ATTACHMENT26
    ColorAttachment27      = 0x8CFB, // GL_COLOR_ATTACHMENT27
    ColorAttachment28      = 0x8CFC, // GL_COLOR_ATTACHMENT28
    ColorAttachment29      = 0x8CFD, // GL_COLOR_ATTACHMENT29
    ColorAttachment30      = 0x8CFE, // GL_COLOR_ATTACHMENT30
    ColorAttachment31      = 0x8CFF, // GL_COLOR_ATTACHMENT31
    DepthAttachment        = 0x8D00, // GL_DEPTH_ATTACHMENT
    StencilAttachment      = 0x8D20, // GL_STENCIL_ATTACHMENT_EXT
}; // enum class InvalidateFramebufferAttachment

enum class LightEnvModeSGIX : GLenum {
    Replace = 0x1E01, // GL_REPLACE
}; // enum class LightEnvModeSGIX

enum class LightEnvParameterSGIX : GLenum {
    LightEnvMode = 0x8407, // GL_LIGHT_ENV_MODE_SGIX
}; // enum class LightEnvParameterSGIX

enum class LightModelColorControl : GLenum {
    SingleColor           = 0x81F9, // GL_SINGLE_COLOR_EXT
    SeparateSpecularColor = 0x81FA, // GL_SEPARATE_SPECULAR_COLOR_EXT
}; // enum class LightModelColorControl

enum class LightModelParameter : GLenum {
    LightModelColorControl = 0x81F8, // GL_LIGHT_MODEL_COLOR_CONTROL_EXT
}; // enum class LightModelParameter

enum class LightName : GLenum {
    FragmentLight0 = 0x840C, // GL_FRAGMENT_LIGHT0_SGIX
    FragmentLight1 = 0x840D, // GL_FRAGMENT_LIGHT1_SGIX
    FragmentLight2 = 0x840E, // GL_FRAGMENT_LIGHT2_SGIX
    FragmentLight3 = 0x840F, // GL_FRAGMENT_LIGHT3_SGIX
    FragmentLight4 = 0x8410, // GL_FRAGMENT_LIGHT4_SGIX
    FragmentLight5 = 0x8411, // GL_FRAGMENT_LIGHT5_SGIX
    FragmentLight6 = 0x8412, // GL_FRAGMENT_LIGHT6_SGIX
    FragmentLight7 = 0x8413, // GL_FRAGMENT_LIGHT7_SGIX
}; // enum class LightName

enum class LightTextureModeEXT : GLenum {
    FragmentMaterial = 0x8349, // GL_FRAGMENT_MATERIAL_EXT
    FragmentNormal   = 0x834A, // GL_FRAGMENT_NORMAL_EXT
    FragmentColor    = 0x834C, // GL_FRAGMENT_COLOR_EXT
    FragmentDepth    = 0x8452, // GL_FRAGMENT_DEPTH_EXT
}; // enum class LightTextureModeEXT

enum class LightTexturePNameEXT : GLenum {
    Attenuation       = 0x834D, // GL_ATTENUATION_EXT
    ShadowAttenuation = 0x834E, // GL_SHADOW_ATTENUATION_EXT
}; // enum class LightTexturePNameEXT

enum class ListNameType : GLenum {
    Byte          = 0x1400, // GL_BYTE
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    Short         = 0x1402, // GL_SHORT
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    Int           = 0x1404, // GL_INT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
    Float         = 0x1406, // GL_FLOAT
}; // enum class ListNameType

enum class ListParameterName : GLenum {
    ListPriority = 0x8182, // GL_LIST_PRIORITY_SGIX
}; // enum class ListParameterName

enum class LogicOp : GLenum {
    Clear        = 0x1500, // GL_CLEAR
    And          = 0x1501, // GL_AND
    AndReverse   = 0x1502, // GL_AND_REVERSE
    Copy         = 0x1503, // GL_COPY
    AndInverted  = 0x1504, // GL_AND_INVERTED
    Noop         = 0x1505, // GL_NOOP
    Xor          = 0x1506, // GL_XOR
    Or           = 0x1507, // GL_OR
    Nor          = 0x1508, // GL_NOR
    Equiv        = 0x1509, // GL_EQUIV
    Invert       = 0x150A, // GL_INVERT
    OrReverse    = 0x150B, // GL_OR_REVERSE
    CopyInverted = 0x150C, // GL_COPY_INVERTED
    OrInverted   = 0x150D, // GL_OR_INVERTED
    Nand         = 0x150E, // GL_NAND
    Set          = 0x150F, // GL_SET
}; // enum class LogicOp

enum class MapAttribParameterNV : GLenum {
    MapAttribUOrder = 0x86C3, // GL_MAP_ATTRIB_U_ORDER_NV
    MapAttribVOrder = 0x86C4, // GL_MAP_ATTRIB_V_ORDER_NV
}; // enum class MapAttribParameterNV

struct MapBufferAccessMask {
    static constexpr GLbitfield MapRead             = 0x0001; // GL_MAP_READ_BIT
    static constexpr GLbitfield MapWrite            = 0x0002; // GL_MAP_WRITE_BIT
    static constexpr GLbitfield MapInvalidateRange  = 0x0004; // GL_MAP_INVALIDATE_RANGE_BIT
    static constexpr GLbitfield MapInvalidateBuffer = 0x0008; // GL_MAP_INVALIDATE_BUFFER_BIT
    static constexpr GLbitfield MapFlushExplicit    = 0x0010; // GL_MAP_FLUSH_EXPLICIT_BIT
    static constexpr GLbitfield MapUnsynchronized   = 0x0020; // GL_MAP_UNSYNCHRONIZED_BIT
    static constexpr GLbitfield MapPersistent       = 0x0040; // GL_MAP_PERSISTENT_BIT
    static constexpr GLbitfield MapCoherent         = 0x0080; // GL_MAP_COHERENT_BIT
}; // struct MapBufferAccessMask

enum class MapParameterNV : GLenum {
    MapTessellation = 0x86C2, // GL_MAP_TESSELLATION_NV
}; // enum class MapParameterNV

enum class MapTarget : GLenum {
    GeometryDeformation = 0x8194, // GL_GEOMETRY_DEFORMATION_SGIX
    TextureDeformation  = 0x8195, // GL_TEXTURE_DEFORMATION_SGIX
}; // enum class MapTarget

// Texture memory layouts for INTEL_map_texture
enum class MapTextureFormatINTEL : GLenum {
    LayoutDefault         = 0, // GL_LAYOUT_DEFAULT_INTEL
    LayoutLinear          = 1, // GL_LAYOUT_LINEAR_INTEL
    LayoutLinearCpuCached = 2, // GL_LAYOUT_LINEAR_CPU_CACHED_INTEL
}; // enum class MapTextureFormatINTEL

enum class MapTypeNV : GLenum {
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class MapTypeNV

enum class MaterialFace : GLenum {
    Front        = 0x0404, // GL_FRONT
    Back         = 0x0405, // GL_BACK
    FrontAndBack = 0x0408, // GL_FRONT_AND_BACK
}; // enum class MaterialFace

enum class MatrixIndexPointerTypeARB : GLenum {
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
}; // enum class MatrixIndexPointerTypeARB

enum class MatrixMode : GLenum {
    Modelview0 = 0x1700, // GL_MODELVIEW0_EXT
    Texture    = 0x1702, // GL_TEXTURE
}; // enum class MatrixMode

struct MemoryBarrierMask {
    static constexpr GLbitfield VertexAttribArrayBarrier  = 0x00000001; // GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    static constexpr GLbitfield ElementArrayBarrier       = 0x00000002; // GL_ELEMENT_ARRAY_BARRIER_BIT
    static constexpr GLbitfield UniformBarrier            = 0x00000004; // GL_UNIFORM_BARRIER_BIT
    static constexpr GLbitfield TextureFetchBarrier       = 0x00000008; // GL_TEXTURE_FETCH_BARRIER_BIT
    static constexpr GLbitfield ShaderGlobalAccessBarrier = 0x00000010; // GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV
    static constexpr GLbitfield ShaderImageAccessBarrier  = 0x00000020; // GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
    static constexpr GLbitfield CommandBarrier            = 0x00000040; // GL_COMMAND_BARRIER_BIT
    static constexpr GLbitfield PixelBufferBarrier        = 0x00000080; // GL_PIXEL_BUFFER_BARRIER_BIT
    static constexpr GLbitfield TextureUpdateBarrier      = 0x00000100; // GL_TEXTURE_UPDATE_BARRIER_BIT
    static constexpr GLbitfield BufferUpdateBarrier       = 0x00000200; // GL_BUFFER_UPDATE_BARRIER_BIT
    static constexpr GLbitfield FramebufferBarrier        = 0x00000400; // GL_FRAMEBUFFER_BARRIER_BIT
    static constexpr GLbitfield TransformFeedbackBarrier  = 0x00000800; // GL_TRANSFORM_FEEDBACK_BARRIER_BIT
    static constexpr GLbitfield AtomicCounterBarrier      = 0x00001000; // GL_ATOMIC_COUNTER_BARRIER_BIT
    static constexpr GLbitfield ShaderStorageBarrier      = 0x00002000; // GL_SHADER_STORAGE_BARRIER_BIT
    static constexpr GLbitfield ClientMappedBufferBarrier = 0x00004000; // GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
    static constexpr GLbitfield QueryBufferBarrier        = 0x00008000; // GL_QUERY_BUFFER_BARRIER_BIT
    static constexpr GLbitfield AllBarrierBits            = 0xFFFFFFFF; // GL_ALL_BARRIER_BITS
}; // struct MemoryBarrierMask

enum class MemoryObjectParameterName : GLenum {
    DedicatedMemoryObject = 0x9581, // GL_DEDICATED_MEMORY_OBJECT_EXT
    ProtectedMemoryObject = 0x959B, // GL_PROTECTED_MEMORY_OBJECT_EXT
}; // enum class MemoryObjectParameterName

enum class MeshMode1 : GLenum {
    Point = 0x1B00, // GL_POINT
    Line  = 0x1B01, // GL_LINE
}; // enum class MeshMode1

enum class MeshMode2 : GLenum {
    Point = 0x1B00, // GL_POINT
    Line  = 0x1B01, // GL_LINE
    Fill  = 0x1B02, // GL_FILL
}; // enum class MeshMode2

enum class MinmaxTargetEXT : GLenum {
    Minmax = 0x802E, // GL_MINMAX_EXT
}; // enum class MinmaxTargetEXT

enum class NormalPointerType : GLenum {
    Byte   = 0x1400, // GL_BYTE
    Short  = 0x1402, // GL_SHORT
    Int    = 0x1404, // GL_INT
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class NormalPointerType

enum class ObjectIdentifier : GLenum {
    Texture           = 0x1702, // GL_TEXTURE
    VertexArray       = 0x8074, // GL_VERTEX_ARRAY
    Buffer            = 0x82E0, // GL_BUFFER
    Shader            = 0x82E1, // GL_SHADER
    Program           = 0x82E2, // GL_PROGRAM
    Query             = 0x82E3, // GL_QUERY
    ProgramPipeline   = 0x82E4, // GL_PROGRAM_PIPELINE
    Sampler           = 0x82E6, // GL_SAMPLER
    Framebuffer       = 0x8D40, // GL_FRAMEBUFFER
    Renderbuffer      = 0x8D41, // GL_RENDERBUFFER
    TransformFeedback = 0x8E22, // GL_TRANSFORM_FEEDBACK
}; // enum class ObjectIdentifier

enum class ObjectTypeAPPLE : GLenum {
    DrawPixels = 0x8A0A, // GL_DRAW_PIXELS_APPLE
    Fence      = 0x8A0B, // GL_FENCE_APPLE
}; // enum class ObjectTypeAPPLE

struct OcclusionQueryEventMaskAMD {
    static constexpr GLuint QueryDepthPassEvent       = 0x00000001; // GL_QUERY_DEPTH_PASS_EVENT_BIT_AMD
    static constexpr GLuint QueryDepthFailEvent       = 0x00000002; // GL_QUERY_DEPTH_FAIL_EVENT_BIT_AMD
    static constexpr GLuint QueryStencilFailEvent     = 0x00000004; // GL_QUERY_STENCIL_FAIL_EVENT_BIT_AMD
    static constexpr GLuint QueryDepthBoundsFailEvent = 0x00000008; // GL_QUERY_DEPTH_BOUNDS_FAIL_EVENT_BIT_AMD
    static constexpr GLuint QueryAllEventBits         = 0xFFFFFFFF; // GL_QUERY_ALL_EVENT_BITS_AMD
}; // struct OcclusionQueryEventMaskAMD

enum class OcclusionQueryParameterNameNV : GLenum {
    PixelCount          = 0x8866, // GL_PIXEL_COUNT_NV
    PixelCountAvailable = 0x8867, // GL_PIXEL_COUNT_AVAILABLE_NV
}; // enum class OcclusionQueryParameterNameNV

enum class ParameterRangeEXT : GLenum {
    NormalizedRange = 0x87E0, // GL_NORMALIZED_RANGE_EXT
    FullRange       = 0x87E1, // GL_FULL_RANGE_EXT
}; // enum class ParameterRangeEXT

enum class PatchParameterName : GLenum {
    PatchVertices          = 0x8E72, // GL_PATCH_VERTICES
    PatchDefaultInnerLevel = 0x8E73, // GL_PATCH_DEFAULT_INNER_LEVEL
    PatchDefaultOuterLevel = 0x8E74, // GL_PATCH_DEFAULT_OUTER_LEVEL
}; // enum class PatchParameterName

enum class PathColor : GLenum {
    PrimaryColor   = 0x852C, // GL_PRIMARY_COLOR_NV
    SecondaryColor = 0x852D, // GL_SECONDARY_COLOR_NV
}; // enum class PathColor

enum class PathColorFormat : GLenum {
    None  =      0, // GL_NONE
    Alpha = 0x1906, // GL_ALPHA
    RGB   = 0x1907, // GL_RGB
    RGBA  = 0x1908, // GL_RGBA
}; // enum class PathColorFormat

enum class PathCoordType : GLenum {
    ClosePath                      = 0x00, // GL_CLOSE_PATH_NV
    MoveTo                         = 0x02, // GL_MOVE_TO_NV
    RelativeMoveTo                 = 0x03, // GL_RELATIVE_MOVE_TO_NV
    LineTo                         = 0x04, // GL_LINE_TO_NV
    RelativeLineTo                 = 0x05, // GL_RELATIVE_LINE_TO_NV
    HorizontalLineTo               = 0x06, // GL_HORIZONTAL_LINE_TO_NV
    RelativeHorizontalLineTo       = 0x07, // GL_RELATIVE_HORIZONTAL_LINE_TO_NV
    VerticalLineTo                 = 0x08, // GL_VERTICAL_LINE_TO_NV
    RelativeVerticalLineTo         = 0x09, // GL_RELATIVE_VERTICAL_LINE_TO_NV
    QuadraticCurveTo               = 0x0A, // GL_QUADRATIC_CURVE_TO_NV
    RelativeQuadraticCurveTo       = 0x0B, // GL_RELATIVE_QUADRATIC_CURVE_TO_NV
    CubicCurveTo                   = 0x0C, // GL_CUBIC_CURVE_TO_NV
    RelativeCubicCurveTo           = 0x0D, // GL_RELATIVE_CUBIC_CURVE_TO_NV
    SmoothQuadraticCurveTo         = 0x0E, // GL_SMOOTH_QUADRATIC_CURVE_TO_NV
    RelativeSmoothQuadraticCurveTo = 0x0F, // GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV
    SmoothCubicCurveTo             = 0x10, // GL_SMOOTH_CUBIC_CURVE_TO_NV
    RelativeSmoothCubicCurveTo     = 0x11, // GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV
    SmallCCWArcTo                  = 0x12, // GL_SMALL_CCW_ARC_TO_NV
    RelativeSmallCCWArcTo          = 0x13, // GL_RELATIVE_SMALL_CCW_ARC_TO_NV
    SmallCWArcTo                   = 0x14, // GL_SMALL_CW_ARC_TO_NV
    RelativeSmallCWArcTo           = 0x15, // GL_RELATIVE_SMALL_CW_ARC_TO_NV
    LargeCCWArcTo                  = 0x16, // GL_LARGE_CCW_ARC_TO_NV
    RelativeLargeCCWArcTo          = 0x17, // GL_RELATIVE_LARGE_CCW_ARC_TO_NV
    LargeCWArcTo                   = 0x18, // GL_LARGE_CW_ARC_TO_NV
    RelativeLargeCWArcTo           = 0x19, // GL_RELATIVE_LARGE_CW_ARC_TO_NV
    ConicCurveTo                   = 0x1A, // GL_CONIC_CURVE_TO_NV
    RelativeConicCurveTo           = 0x1B, // GL_RELATIVE_CONIC_CURVE_TO_NV
    RoundedRect                    = 0xE8, // GL_ROUNDED_RECT_NV
    RelativeRoundedRect            = 0xE9, // GL_RELATIVE_ROUNDED_RECT_NV
    RoundedRect2                   = 0xEA, // GL_ROUNDED_RECT2_NV
    RelativeRoundedRect2           = 0xEB, // GL_RELATIVE_ROUNDED_RECT2_NV
    RoundedRect4                   = 0xEC, // GL_ROUNDED_RECT4_NV
    RelativeRoundedRect4           = 0xED, // GL_RELATIVE_ROUNDED_RECT4_NV
    RoundedRect8                   = 0xEE, // GL_ROUNDED_RECT8_NV
    RelativeRoundedRect8           = 0xEF, // GL_RELATIVE_ROUNDED_RECT8_NV
    RestartPath                    = 0xF0, // GL_RESTART_PATH_NV
    DupFirstCubicCurveTo           = 0xF2, // GL_DUP_FIRST_CUBIC_CURVE_TO_NV
    DupLastCubicCurveTo            = 0xF4, // GL_DUP_LAST_CUBIC_CURVE_TO_NV
    Rect                           = 0xF6, // GL_RECT_NV
    RelativeRect                   = 0xF7, // GL_RELATIVE_RECT_NV
    CircularCCWArcTo               = 0xF8, // GL_CIRCULAR_CCW_ARC_TO_NV
    CircularCWArcTo                = 0xFA, // GL_CIRCULAR_CW_ARC_TO_NV
    CircularTangentArcTo           = 0xFC, // GL_CIRCULAR_TANGENT_ARC_TO_NV
    ArcTo                          = 0xFE, // GL_ARC_TO_NV
    RelativeArcTo                  = 0xFF, // GL_RELATIVE_ARC_TO_NV
}; // enum class PathCoordType

enum class PathCoverMode : GLenum {
    PathFillCoverMode          = 0x9082, // GL_PATH_FILL_COVER_MODE_NV
    ConvexHull                 = 0x908B, // GL_CONVEX_HULL_NV
    BoundingBox                = 0x908D, // GL_BOUNDING_BOX_NV
    BoundingBoxOfBoundingBoxes = 0x909C, // GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV
}; // enum class PathCoverMode

enum class PathElementType : GLenum {
    UTF8  = 0x909A, // GL_UTF8_NV
    UTF16 = 0x909B, // GL_UTF16_NV
}; // enum class PathElementType

enum class PathFillMode : GLenum {
    Invert       = 0x150A, // GL_INVERT
    PathFillMode = 0x9080, // GL_PATH_FILL_MODE_NV
    CountUp      = 0x9088, // GL_COUNT_UP_NV
    CountDown    = 0x9089, // GL_COUNT_DOWN_NV
}; // enum class PathFillMode

struct PathFontStyle {
    static constexpr GLbitfield None   =    0; // GL_NONE
    static constexpr GLbitfield Bold   = 0x01; // GL_BOLD_BIT_NV
    static constexpr GLbitfield Italic = 0x02; // GL_ITALIC_BIT_NV
}; // struct PathFontStyle

enum class PathFontTarget : GLenum {
    StandardFontName = 0x9072, // GL_STANDARD_FONT_NAME_NV
    SystemFontName   = 0x9073, // GL_SYSTEM_FONT_NAME_NV
    FileName         = 0x9074, // GL_FILE_NAME_NV
}; // enum class PathFontTarget

enum class PathGenMode : GLenum {
    None                  =      0, // GL_NONE
    PathObjectBoundingBox = 0x908A, // GL_PATH_OBJECT_BOUNDING_BOX_NV
}; // enum class PathGenMode

enum class PathHandleMissingGlyphs : GLenum {
    SkipMissingGlyph = 0x90A9, // GL_SKIP_MISSING_GLYPH_NV
    UseMissingGlyph  = 0x90AA, // GL_USE_MISSING_GLYPH_NV
}; // enum class PathHandleMissingGlyphs

enum class PathListMode : GLenum {
    AccumAdjacentPairs = 0x90AD, // GL_ACCUM_ADJACENT_PAIRS_NV
    AdjacentPairs      = 0x90AE, // GL_ADJACENT_PAIRS_NV
    FirstToRest        = 0x90AF, // GL_FIRST_TO_REST_NV
}; // enum class PathListMode

struct PathMetricMask {
    static constexpr GLbitfield GlyphWidth                    =       0x01; // GL_GLYPH_WIDTH_BIT_NV
    static constexpr GLbitfield GlyphHeight                   =       0x02; // GL_GLYPH_HEIGHT_BIT_NV
    static constexpr GLbitfield GlyphHorizontalBearingX       =       0x04; // GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV
    static constexpr GLbitfield GlyphHorizontalBearingY       =       0x08; // GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV
    static constexpr GLbitfield GlyphHorizontalBearingAdvance =       0x10; // GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV
    static constexpr GLbitfield GlyphVerticalBearingX         =       0x20; // GL_GLYPH_VERTICAL_BEARING_X_BIT_NV
    static constexpr GLbitfield GlyphVerticalBearingY         =       0x40; // GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV
    static constexpr GLbitfield GlyphVerticalBearingAdvance   =       0x80; // GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV
    static constexpr GLbitfield GlyphHasKerning               =      0x100; // GL_GLYPH_HAS_KERNING_BIT_NV
    static constexpr GLbitfield FontXMinBounds                = 0x00010000; // GL_FONT_X_MIN_BOUNDS_BIT_NV
    static constexpr GLbitfield FontYMinBounds                = 0x00020000; // GL_FONT_Y_MIN_BOUNDS_BIT_NV
    static constexpr GLbitfield FontXMaxBounds                = 0x00040000; // GL_FONT_X_MAX_BOUNDS_BIT_NV
    static constexpr GLbitfield FontYMaxBounds                = 0x00080000; // GL_FONT_Y_MAX_BOUNDS_BIT_NV
    static constexpr GLbitfield FontUnitsPerEm                = 0x00100000; // GL_FONT_UNITS_PER_EM_BIT_NV
    static constexpr GLbitfield FontAscender                  = 0x00200000; // GL_FONT_ASCENDER_BIT_NV
    static constexpr GLbitfield FontDescender                 = 0x00400000; // GL_FONT_DESCENDER_BIT_NV
    static constexpr GLbitfield FontHeight                    = 0x00800000; // GL_FONT_HEIGHT_BIT_NV
    static constexpr GLbitfield FontMaxAdvanceWidth           = 0x01000000; // GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV
    static constexpr GLbitfield FontMaxAdvanceHeight          = 0x02000000; // GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV
    static constexpr GLbitfield FontUnderlinePosition         = 0x04000000; // GL_FONT_UNDERLINE_POSITION_BIT_NV
    static constexpr GLbitfield FontUnderlineThickness        = 0x08000000; // GL_FONT_UNDERLINE_THICKNESS_BIT_NV
    static constexpr GLbitfield FontHasKerning                = 0x10000000; // GL_FONT_HAS_KERNING_BIT_NV
    static constexpr GLbitfield FontNumGlyphIndices           = 0x20000000; // GL_FONT_NUM_GLYPH_INDICES_BIT_NV
}; // struct PathMetricMask

enum class PathParameter : GLenum {
    PathStrokeWidth       = 0x9075, // GL_PATH_STROKE_WIDTH_NV
    PathEndCaps           = 0x9076, // GL_PATH_END_CAPS_NV
    PathInitialEndCap     = 0x9077, // GL_PATH_INITIAL_END_CAP_NV
    PathTerminalEndCap    = 0x9078, // GL_PATH_TERMINAL_END_CAP_NV
    PathJoinStyle         = 0x9079, // GL_PATH_JOIN_STYLE_NV
    PathMiterLimit        = 0x907A, // GL_PATH_MITER_LIMIT_NV
    PathDashCaps          = 0x907B, // GL_PATH_DASH_CAPS_NV
    PathInitialDashCap    = 0x907C, // GL_PATH_INITIAL_DASH_CAP_NV
    PathTerminalDashCap   = 0x907D, // GL_PATH_TERMINAL_DASH_CAP_NV
    PathDashOffset        = 0x907E, // GL_PATH_DASH_OFFSET_NV
    PathClientLength      = 0x907F, // GL_PATH_CLIENT_LENGTH_NV
    PathFillMode          = 0x9080, // GL_PATH_FILL_MODE_NV
    PathFillMask          = 0x9081, // GL_PATH_FILL_MASK_NV
    PathFillCoverMode     = 0x9082, // GL_PATH_FILL_COVER_MODE_NV
    PathStrokeCoverMode   = 0x9083, // GL_PATH_STROKE_COVER_MODE_NV
    PathStrokeMask        = 0x9084, // GL_PATH_STROKE_MASK_NV
    PathObjectBoundingBox = 0x908A, // GL_PATH_OBJECT_BOUNDING_BOX_NV
    PathCommandCount      = 0x909D, // GL_PATH_COMMAND_COUNT_NV
    PathCoordCount        = 0x909E, // GL_PATH_COORD_COUNT_NV
    PathDashArrayCount    = 0x909F, // GL_PATH_DASH_ARRAY_COUNT_NV
    PathComputeDLength    = 0x90A0, // GL_PATH_COMPUTED_LENGTH_NV
    PathFillBoundingBox   = 0x90A1, // GL_PATH_FILL_BOUNDING_BOX_NV
    PathStrokeBoundingBox = 0x90A2, // GL_PATH_STROKE_BOUNDING_BOX_NV
    PathDashOffsetReset   = 0x90B4, // GL_PATH_DASH_OFFSET_RESET_NV
}; // enum class PathParameter

enum class PathStringFormat : GLenum {
    PathFormatSvg = 0x9070, // GL_PATH_FORMAT_SVG_NV
    PathFormatPs  = 0x9071, // GL_PATH_FORMAT_PS_NV
}; // enum class PathStringFormat

enum class PathTransformType : GLenum {
    None              =      0, // GL_NONE
    TranslateX        = 0x908E, // GL_TRANSLATE_X_NV
    TranslateY        = 0x908F, // GL_TRANSLATE_Y_NV
    Translate2D       = 0x9090, // GL_TRANSLATE_2D_NV
    Translate3D       = 0x9091, // GL_TRANSLATE_3D_NV
    Affine2D          = 0x9092, // GL_AFFINE_2D_NV
    Affine3D          = 0x9094, // GL_AFFINE_3D_NV
    TransposeAffine2D = 0x9096, // GL_TRANSPOSE_AFFINE_2D_NV
    TransposeAffine3D = 0x9098, // GL_TRANSPOSE_AFFINE_3D_NV
}; // enum class PathTransformType

struct PerformanceQueryCapsMaskINTEL {
    static constexpr GLenum PerfquerySingleContext = 0x00000000; // GL_PERFQUERY_SINGLE_CONTEXT_INTEL
    static constexpr GLenum PerfqueryGlobalContext = 0x00000001; // GL_PERFQUERY_GLOBAL_CONTEXT_INTEL
}; // struct PerformanceQueryCapsMaskINTEL

enum class PipelineParameterName : GLenum {
    ActiveProgram        = 0x8259, // GL_ACTIVE_PROGRAM
    FragmentShader       = 0x8B30, // GL_FRAGMENT_SHADER
    VertexShader         = 0x8B31, // GL_VERTEX_SHADER
    InfoLogLength        = 0x8B84, // GL_INFO_LOG_LENGTH
    GeometryShader       = 0x8DD9, // GL_GEOMETRY_SHADER
    TessEvaluationShader = 0x8E87, // GL_TESS_EVALUATION_SHADER
    TessControlShader    = 0x8E88, // GL_TESS_CONTROL_SHADER
}; // enum class PipelineParameterName

enum class PixelCopyType : GLenum {
    Color   = 0x1800, // GL_COLOR
    Depth   = 0x1801, // GL_DEPTH
    Stencil = 0x1802, // GL_STENCIL
}; // enum class PixelCopyType

enum class PixelDataRangeTargetNV : GLenum {
    WritePixelDataRange = 0x8878, // GL_WRITE_PIXEL_DATA_RANGE_NV
    ReadPixelDataRange  = 0x8879, // GL_READ_PIXEL_DATA_RANGE_NV
}; // enum class PixelDataRangeTargetNV

enum class PixelFormat : GLenum {
    UnsignedShort  = 0x1403, // GL_UNSIGNED_SHORT
    UnsignedInt    = 0x1405, // GL_UNSIGNED_INT
    StencilIndex   = 0x1901, // GL_STENCIL_INDEX
    DepthComponent = 0x1902, // GL_DEPTH_COMPONENT
    Red            = 0x1903, // GL_RED
    Green          = 0x1904, // GL_GREEN
    Blue           = 0x1905, // GL_BLUE
    Alpha          = 0x1906, // GL_ALPHA
    RGB            = 0x1907, // GL_RGB
    RGBA           = 0x1908, // GL_RGBA
    ABGR           = 0x8000, // GL_ABGR_EXT
    CMYK           = 0x800C, // GL_CMYK_EXT
    CMYKA          = 0x800D, // GL_CMYKA_EXT
    BGR            = 0x80E0, // GL_BGR
    BGRA           = 0x80E1, // GL_BGRA
    Ycrcb422       = 0x81BB, // GL_YCRCB_422_SGIX
    Ycrcb444       = 0x81BC, // GL_YCRCB_444_SGIX
    Rg             = 0x8227, // GL_RG
    RgInteger      = 0x8228, // GL_RG_INTEGER
    DepthStencil   = 0x84F9, // GL_DEPTH_STENCIL
    RedInteger     = 0x8D94, // GL_RED_INTEGER
    GreenInteger   = 0x8D95, // GL_GREEN_INTEGER
    BlueInteger    = 0x8D96, // GL_BLUE_INTEGER
    RGBInteger     = 0x8D98, // GL_RGB_INTEGER
    RGBAInteger    = 0x8D99, // GL_RGBA_INTEGER
    BGRInteger     = 0x8D9A, // GL_BGR_INTEGER
    BGRAInteger    = 0x8D9B, // GL_BGRA_INTEGER
}; // enum class PixelFormat

enum class PixelStoreParameter : GLenum {
    UnpackSwapBytes     = 0x0CF0, // GL_UNPACK_SWAP_BYTES
    UnpackLsbFirst      = 0x0CF1, // GL_UNPACK_LSB_FIRST
    UnpackRowLength     = 0x0CF2, // GL_UNPACK_ROW_LENGTH
    UnpackSkipRows      = 0x0CF3, // GL_UNPACK_SKIP_ROWS
    UnpackSkipPixels    = 0x0CF4, // GL_UNPACK_SKIP_PIXELS
    UnpackAlignment     = 0x0CF5, // GL_UNPACK_ALIGNMENT
    PackSwapBytes       = 0x0D00, // GL_PACK_SWAP_BYTES
    PackLsbFirst        = 0x0D01, // GL_PACK_LSB_FIRST
    PackRowLength       = 0x0D02, // GL_PACK_ROW_LENGTH
    PackSkipRows        = 0x0D03, // GL_PACK_SKIP_ROWS
    PackSkipPixels      = 0x0D04, // GL_PACK_SKIP_PIXELS
    PackAlignment       = 0x0D05, // GL_PACK_ALIGNMENT
    PackSkipImageS      = 0x806B, // GL_PACK_SKIP_IMAGES
    PackImageHeight     = 0x806C, // GL_PACK_IMAGE_HEIGHT
    UnpackSkipImageS    = 0x806D, // GL_UNPACK_SKIP_IMAGES
    UnpackImageHeight   = 0x806E, // GL_UNPACK_IMAGE_HEIGHT
    PackSkipVolumes     = 0x8130, // GL_PACK_SKIP_VOLUMES_SGIS
    PackImageDepth      = 0x8131, // GL_PACK_IMAGE_DEPTH_SGIS
    UnpackSkipVolumes   = 0x8132, // GL_UNPACK_SKIP_VOLUMES_SGIS
    UnpackImageDepth    = 0x8133, // GL_UNPACK_IMAGE_DEPTH_SGIS
    PixelTileWidth      = 0x8140, // GL_PIXEL_TILE_WIDTH_SGIX
    PixelTileHeight     = 0x8141, // GL_PIXEL_TILE_HEIGHT_SGIX
    PixelTileGridWidth  = 0x8142, // GL_PIXEL_TILE_GRID_WIDTH_SGIX
    PixelTileGridHeight = 0x8143, // GL_PIXEL_TILE_GRID_HEIGHT_SGIX
    PixelTileGridDepth  = 0x8144, // GL_PIXEL_TILE_GRID_DEPTH_SGIX
    PixelTileCacheSize  = 0x8145, // GL_PIXEL_TILE_CACHE_SIZE_SGIX
    PackResampleSGIX    = 0x842E, // GL_PACK_RESAMPLE_SGIX - Formerly 0x842C in SGI specfile
    UnpackResampleSGIX  = 0x842F, // GL_UNPACK_RESAMPLE_SGIX - Formerly 0x842D in SGI specfile
    PackSubsampleRate   = 0x85A0, // GL_PACK_SUBSAMPLE_RATE_SGIX
    UnpackSubsampleRate = 0x85A1, // GL_UNPACK_SUBSAMPLE_RATE_SGIX
    PackResampleOML     = 0x8984, // GL_PACK_RESAMPLE_OML
    UnpackResampleOML   = 0x8985, // GL_UNPACK_RESAMPLE_OML
}; // enum class PixelStoreParameter

enum class PixelStoreResampleMode : GLenum {
    ResampleDecimate  = 0x8430, // GL_RESAMPLE_DECIMATE_SGIX - Formerly 0x8430 in SGI specfile
    ResampleReplicate = 0x8433, // GL_RESAMPLE_REPLICATE_SGIX - Formerly 0x842E in SGI specfile
    ResampleZeroFill  = 0x8434, // GL_RESAMPLE_ZERO_FILL_SGIX - Formerly 0x842F in SGI specfile
}; // enum class PixelStoreResampleMode

enum class PixelStoreSubsampleRate : GLenum {
    PixelSubsample4444 = 0x85A2, // GL_PIXEL_SUBSAMPLE_4444_SGIX
    PixelSubsample2424 = 0x85A3, // GL_PIXEL_SUBSAMPLE_2424_SGIX
    PixelSubsample4242 = 0x85A4, // GL_PIXEL_SUBSAMPLE_4242_SGIX
}; // enum class PixelStoreSubsampleRate

enum class PixelTexGenMode : GLenum {
    None =      0, // GL_NONE
    RGB  = 0x1907, // GL_RGB
    RGBA = 0x1908, // GL_RGBA
}; // enum class PixelTexGenMode

enum class PixelTexGenParameterNameSGIS : GLenum {
    PixelFragmentRGBSource   = 0x8354, // GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS
    PixelFragmentAlphaSource = 0x8355, // GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS
}; // enum class PixelTexGenParameterNameSGIS

enum class PixelTransferParameter : GLenum {
    PostConvolutionRedScale   = 0x801C, // GL_POST_CONVOLUTION_RED_SCALE_EXT
    PostConvolutionGreenScale = 0x801D, // GL_POST_CONVOLUTION_GREEN_SCALE_EXT
    PostConvolutionBlueScale  = 0x801E, // GL_POST_CONVOLUTION_BLUE_SCALE_EXT
    PostConvolutionAlphaScale = 0x801F, // GL_POST_CONVOLUTION_ALPHA_SCALE_EXT
    PostConvolutionRedBias    = 0x8020, // GL_POST_CONVOLUTION_RED_BIAS_EXT
    PostConvolutionGreenBias  = 0x8021, // GL_POST_CONVOLUTION_GREEN_BIAS_EXT
    PostConvolutionBlueBias   = 0x8022, // GL_POST_CONVOLUTION_BLUE_BIAS_EXT
    PostConvolutionAlphaBias  = 0x8023, // GL_POST_CONVOLUTION_ALPHA_BIAS_EXT
    PostColorMatrixRedScale   = 0x80B4, // GL_POST_COLOR_MATRIX_RED_SCALE_SGI
    PostColorMatrixGreenScale = 0x80B5, // GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI
    PostColorMatrixBlueScale  = 0x80B6, // GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI
    PostColorMatrixAlphaScale = 0x80B7, // GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI
    PostColorMatrixRedBias    = 0x80B8, // GL_POST_COLOR_MATRIX_RED_BIAS_SGI
    PostColorMatrixGreenBias  = 0x80B9, // GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI
    PostColorMatrixBlueBias   = 0x80BA, // GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI
    PostColorMatrixAlphaBias  = 0x80BB, // GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI
}; // enum class PixelTransferParameter

enum class PixelTransformPNameEXT : GLenum {
    PixelMagFilter   = 0x8331, // GL_PIXEL_MAG_FILTER_EXT
    PixelMinFilter   = 0x8332, // GL_PIXEL_MIN_FILTER_EXT
    PixelCubicWeight = 0x8333, // GL_PIXEL_CUBIC_WEIGHT_EXT
}; // enum class PixelTransformPNameEXT

enum class PixelTransformTargetEXT : GLenum {
    PixelTransform2D = 0x8330, // GL_PIXEL_TRANSFORM_2D_EXT
}; // enum class PixelTransformTargetEXT

enum class PixelType : GLenum {
    Byte                  = 0x1400, // GL_BYTE
    UnsignedByte          = 0x1401, // GL_UNSIGNED_BYTE
    Short                 = 0x1402, // GL_SHORT
    UnsignedShort         = 0x1403, // GL_UNSIGNED_SHORT
    Int                   = 0x1404, // GL_INT
    UnsignedInt           = 0x1405, // GL_UNSIGNED_INT
    Float                 = 0x1406, // GL_FLOAT
    UnsignedByte3_3_2     = 0x8032, // GL_UNSIGNED_BYTE_3_3_2
    UnsignedShort4_4_4_4  = 0x8033, // GL_UNSIGNED_SHORT_4_4_4_4
    UnsignedShort5_5_5_1  = 0x8034, // GL_UNSIGNED_SHORT_5_5_5_1
    UnsignedInt8_8_8_8    = 0x8035, // GL_UNSIGNED_INT_8_8_8_8
    UnsignedInt10_10_10_2 = 0x8036, // GL_UNSIGNED_INT_10_10_10_2
}; // enum class PixelType

enum class PNTrianglesPNameATI : GLenum {
    PnTrianglesPointMode        = 0x87F2, // GL_PN_TRIANGLES_POINT_MODE_ATI
    PnTrianglesNormalMode       = 0x87F3, // GL_PN_TRIANGLES_NORMAL_MODE_ATI
    PnTrianglesTesselationLevel = 0x87F4, // GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI
}; // enum class PNTrianglesPNameATI

enum class PointParameterNameARB : GLenum {
    PointSizeMin           = 0x8126, // GL_POINT_SIZE_MIN_EXT
    PointSizeMax           = 0x8127, // GL_POINT_SIZE_MAX_EXT
    PointFadeThresholdSize = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE
}; // enum class PointParameterNameARB

enum class PointParameterNameSGIS : GLenum {
    PointSizeMin           = 0x8126, // GL_POINT_SIZE_MIN_ARB
    PointSizeMax           = 0x8127, // GL_POINT_SIZE_MAX_ARB
    PointFadeThresholdSize = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE
    DistanceAttenuation    = 0x8129, // GL_DISTANCE_ATTENUATION_EXT
}; // enum class PointParameterNameSGIS

enum class PolygonMode : GLenum {
    Point = 0x1B00, // GL_POINT
    Line  = 0x1B01, // GL_LINE
    Fill  = 0x1B02, // GL_FILL
}; // enum class PolygonMode

enum class PrecisionType : GLenum {
    LowFloat    = 0x8DF0, // GL_LOW_FLOAT
    MediumFloat = 0x8DF1, // GL_MEDIUM_FLOAT
    HighFloat   = 0x8DF2, // GL_HIGH_FLOAT
    LowInt      = 0x8DF3, // GL_LOW_INT
    MediumInt   = 0x8DF4, // GL_MEDIUM_INT
    HighInt     = 0x8DF5, // GL_HIGH_INT
}; // enum class PrecisionType

enum class PreserveModeATI : GLenum {
    Preserve = 0x8762, // GL_PRESERVE_ATI
    Discard  = 0x8763, // GL_DISCARD_ATI
}; // enum class PreserveModeATI

enum class PrimitiveType : GLenum {
    Points                 = 0x0000, // GL_POINTS
    Lines                  = 0x0001, // GL_LINES
    LineLoop               = 0x0002, // GL_LINE_LOOP
    LineStrip              = 0x0003, // GL_LINE_STRIP
    Triangles              = 0x0004, // GL_TRIANGLES
    TriangleStrip          = 0x0005, // GL_TRIANGLE_STRIP
    TriangleFan            = 0x0006, // GL_TRIANGLE_FAN
    Quads                  = 0x0007, // GL_QUADS
    LinesAdjacency         = 0x000A, // GL_LINES_ADJACENCY
    LineStripAdjacency     = 0x000B, // GL_LINE_STRIP_ADJACENCY
    TrianglesAdjacency     = 0x000C, // GL_TRIANGLES_ADJACENCY
    TriangleStripAdjacency = 0x000D, // GL_TRIANGLE_STRIP_ADJACENCY
    Patches                = 0x000E, // GL_PATCHES
}; // enum class PrimitiveType

enum class ProgramFormat : GLenum {
    ProgramFormatASCII = 0x8875, // GL_PROGRAM_FORMAT_ASCII_ARB
}; // enum class ProgramFormat

enum class ProgramInterface : GLenum {
    TransformFeedbackBuffer         = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
    Uniform                         = 0x92E1, // GL_UNIFORM
    UniformBlock                    = 0x92E2, // GL_UNIFORM_BLOCK
    ProgramInput                    = 0x92E3, // GL_PROGRAM_INPUT
    ProgramOutput                   = 0x92E4, // GL_PROGRAM_OUTPUT
    BufferVariable                  = 0x92E5, // GL_BUFFER_VARIABLE
    ShaderStorageBlock              = 0x92E6, // GL_SHADER_STORAGE_BLOCK
    VertexSubroutine                = 0x92E8, // GL_VERTEX_SUBROUTINE
    TessControlSubroutine           = 0x92E9, // GL_TESS_CONTROL_SUBROUTINE
    TessEvaluationSubroutine        = 0x92EA, // GL_TESS_EVALUATION_SUBROUTINE
    GeometrySubroutine              = 0x92EB, // GL_GEOMETRY_SUBROUTINE
    FragmentSubroutine              = 0x92EC, // GL_FRAGMENT_SUBROUTINE
    ComputeSubroutine               = 0x92ED, // GL_COMPUTE_SUBROUTINE
    VertexSubroutineUniform         = 0x92EE, // GL_VERTEX_SUBROUTINE_UNIFORM
    TessControlSubroutineUniform    = 0x92EF, // GL_TESS_CONTROL_SUBROUTINE_UNIFORM
    TessEvaluationSubroutineUniform = 0x92F0, // GL_TESS_EVALUATION_SUBROUTINE_UNIFORM
    GeometrySubroutineUniform       = 0x92F1, // GL_GEOMETRY_SUBROUTINE_UNIFORM
    FragmentSubroutineUniform       = 0x92F2, // GL_FRAGMENT_SUBROUTINE_UNIFORM
    ComputeSubroutineUniform        = 0x92F3, // GL_COMPUTE_SUBROUTINE_UNIFORM
    TransformFeedbackVarying        = 0x92F4, // GL_TRANSFORM_FEEDBACK_VARYING
}; // enum class ProgramInterface

enum class ProgramInterfacePName : GLenum {
    ActiveResourceS             = 0x92F5, // GL_ACTIVE_RESOURCES
    MaxNameLength               = 0x92F6, // GL_MAX_NAME_LENGTH
    MaxNumActiveVariables       = 0x92F7, // GL_MAX_NUM_ACTIVE_VARIABLES
    MaxNumCompatibleSubroutines = 0x92F8, // GL_MAX_NUM_COMPATIBLE_SUBROUTINES
}; // enum class ProgramInterfacePName

enum class ProgramParameterPName : GLenum {
    ProgramBinaryRetrievableHint = 0x8257, // GL_PROGRAM_BINARY_RETRIEVABLE_HINT
    ProgramSeparable             = 0x8258, // GL_PROGRAM_SEPARABLE
}; // enum class ProgramParameterPName

enum class ProgramProperty : GLenum {
    ComputeWorkGroupSize              = 0x8267, // GL_COMPUTE_WORK_GROUP_SIZE
    ProgramBinaryLength               = 0x8741, // GL_PROGRAM_BINARY_LENGTH
    GeometryVerticesOut               = 0x8916, // GL_GEOMETRY_VERTICES_OUT
    GeometryInputType                 = 0x8917, // GL_GEOMETRY_INPUT_TYPE
    GeometryOutputType                = 0x8918, // GL_GEOMETRY_OUTPUT_TYPE
    ActiveUniformBlockMaxNameLength   = 0x8A35, // GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH
    ActiveUniformBlocks               = 0x8A36, // GL_ACTIVE_UNIFORM_BLOCKS
    DeleteStatus                      = 0x8B80, // GL_DELETE_STATUS
    LinkStatus                        = 0x8B82, // GL_LINK_STATUS
    ValidateStatus                    = 0x8B83, // GL_VALIDATE_STATUS
    InfoLogLength                     = 0x8B84, // GL_INFO_LOG_LENGTH
    AttachedShaders                   = 0x8B85, // GL_ATTACHED_SHADERS
    ActiveUniforms                    = 0x8B86, // GL_ACTIVE_UNIFORMS
    ActiveUniformMaxLength            = 0x8B87, // GL_ACTIVE_UNIFORM_MAX_LENGTH
    ActiveAttributes                  = 0x8B89, // GL_ACTIVE_ATTRIBUTES
    ActiveAttributeMaxLength          = 0x8B8A, // GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
    TransformFeedbackVaryingMaxLength = 0x8C76, // GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH
    TransformFeedbackBufferMode       = 0x8C7F, // GL_TRANSFORM_FEEDBACK_BUFFER_MODE
    TransformFeedbackVaryings         = 0x8C83, // GL_TRANSFORM_FEEDBACK_VARYINGS
    ActiveAtomicCounterBuffers        = 0x92D9, // GL_ACTIVE_ATOMIC_COUNTER_BUFFERS
}; // enum class ProgramProperty

enum class ProgramResourceProperty : GLenum {
    NumCompatibleSubroutines         = 0x8E4A, // GL_NUM_COMPATIBLE_SUBROUTINES
    CompatibleSubroutines            = 0x8E4B, // GL_COMPATIBLE_SUBROUTINES
    Uniform                          = 0x92E1, // GL_UNIFORM
    IsPerPatch                       = 0x92E7, // GL_IS_PER_PATCH
    NameLength                       = 0x92F9, // GL_NAME_LENGTH
    Type                             = 0x92FA, // GL_TYPE
    ArraySize                        = 0x92FB, // GL_ARRAY_SIZE
    Offset                           = 0x92FC, // GL_OFFSET
    BlockIndex                       = 0x92FD, // GL_BLOCK_INDEX
    ArrayStride                      = 0x92FE, // GL_ARRAY_STRIDE
    MatrixStride                     = 0x92FF, // GL_MATRIX_STRIDE
    IsRowMajor                       = 0x9300, // GL_IS_ROW_MAJOR
    AtomicCounterBufferIndex         = 0x9301, // GL_ATOMIC_COUNTER_BUFFER_INDEX
    BufferBinding                    = 0x9302, // GL_BUFFER_BINDING
    BufferDataSize                   = 0x9303, // GL_BUFFER_DATA_SIZE
    NumActiveVariables               = 0x9304, // GL_NUM_ACTIVE_VARIABLES
    ActiveVariables                  = 0x9305, // GL_ACTIVE_VARIABLES
    ReferencedByVertexShader         = 0x9306, // GL_REFERENCED_BY_VERTEX_SHADER
    ReferencedByTessControlShader    = 0x9307, // GL_REFERENCED_BY_TESS_CONTROL_SHADER
    ReferencedByTessEvaluationShader = 0x9308, // GL_REFERENCED_BY_TESS_EVALUATION_SHADER
    ReferencedByGeometryShader       = 0x9309, // GL_REFERENCED_BY_GEOMETRY_SHADER
    ReferencedByFragmentShader       = 0x930A, // GL_REFERENCED_BY_FRAGMENT_SHADER
    ReferencedByComputeShader        = 0x930B, // GL_REFERENCED_BY_COMPUTE_SHADER
    TopLevelArraySize                = 0x930C, // GL_TOP_LEVEL_ARRAY_SIZE
    TopLevelArrayStride              = 0x930D, // GL_TOP_LEVEL_ARRAY_STRIDE
    Location                         = 0x930E, // GL_LOCATION
    LocationIndex                    = 0x930F, // GL_LOCATION_INDEX
    LocationComponent                = 0x934A, // GL_LOCATION_COMPONENT
    TransformFeedbackBufferIndex     = 0x934B, // GL_TRANSFORM_FEEDBACK_BUFFER_INDEX
    TransformFeedbackBufferStride    = 0x934C, // GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE
}; // enum class ProgramResourceProperty

enum class ProgramStagePName : GLenum {
    ActiveSubroutines                = 0x8DE5, // GL_ACTIVE_SUBROUTINES
    ActiveSubroutineUniforms         = 0x8DE6, // GL_ACTIVE_SUBROUTINE_UNIFORMS
    ActiveSubroutineUniformLocations = 0x8E47, // GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS
    ActiveSubroutineMaxLength        = 0x8E48, // GL_ACTIVE_SUBROUTINE_MAX_LENGTH
    ActiveSubroutineUniformMaxLength = 0x8E49, // GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH
}; // enum class ProgramStagePName

enum class ProgramStringProperty : GLenum {
    ProgramString = 0x8628, // GL_PROGRAM_STRING_ARB
}; // enum class ProgramStringProperty

enum class ProgramTarget : GLenum {
    TextFragmentShader    = 0x8200, // GL_TEXT_FRAGMENT_SHADER_ATI
    VertexProgram         = 0x8620, // GL_VERTEX_PROGRAM_ARB
    FragmentProgram       = 0x8804, // GL_FRAGMENT_PROGRAM_ARB
    TessControlProgram    = 0x891E, // GL_TESS_CONTROL_PROGRAM_NV
    TessEvaluationProgram = 0x891F, // GL_TESS_EVALUATION_PROGRAM_NV
    GeometryProgram       = 0x8C26, // GL_GEOMETRY_PROGRAM_NV
    ComputeProgram        = 0x90FB, // GL_COMPUTE_PROGRAM_NV
}; // enum class ProgramTarget

enum class QueryCounterTarget : GLenum {
    Timestamp = 0x8E28, // GL_TIMESTAMP
}; // enum class QueryCounterTarget

enum class QueryObjectParameterName : GLenum {
    QueryTarget          = 0x82EA, // GL_QUERY_TARGET
    QueryResult          = 0x8866, // GL_QUERY_RESULT
    QueryResultAvailable = 0x8867, // GL_QUERY_RESULT_AVAILABLE
    QueryResultNoWait    = 0x9194, // GL_QUERY_RESULT_NO_WAIT
}; // enum class QueryObjectParameterName

enum class QueryParameterName : GLenum {
    QueryCounterBits = 0x8864, // GL_QUERY_COUNTER_BITS
    CurrentQuery     = 0x8865, // GL_CURRENT_QUERY
}; // enum class QueryParameterName

enum class QueryTarget : GLenum {
    TransformFeedbackOverflow          = 0x82EC, // GL_TRANSFORM_FEEDBACK_OVERFLOW
    VerticesSubmitted                  = 0x82EE, // GL_VERTICES_SUBMITTED
    PrimitivesSubmitted                = 0x82EF, // GL_PRIMITIVES_SUBMITTED
    VertexShaderInvocations            = 0x82F0, // GL_VERTEX_SHADER_INVOCATIONS
    TimeElapsed                        = 0x88BF, // GL_TIME_ELAPSED
    SamplesPassed                      = 0x8914, // GL_SAMPLES_PASSED
    AnySamplesPassed                   = 0x8C2F, // GL_ANY_SAMPLES_PASSED
    PrimitivesGenerated                = 0x8C87, // GL_PRIMITIVES_GENERATED
    TransformFeedbackPrimitivesWritten = 0x8C88, // GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
    AnySamplesPassedConservative       = 0x8D6A, // GL_ANY_SAMPLES_PASSED_CONSERVATIVE
}; // enum class QueryTarget

enum class ReadBufferMode : GLenum {
    None              =      0, // GL_NONE
    FrontLeft         = 0x0400, // GL_FRONT_LEFT
    FrontRight        = 0x0401, // GL_FRONT_RIGHT
    BackLeft          = 0x0402, // GL_BACK_LEFT
    BackRight         = 0x0403, // GL_BACK_RIGHT
    Front             = 0x0404, // GL_FRONT
    Back              = 0x0405, // GL_BACK
    Left              = 0x0406, // GL_LEFT
    Right             = 0x0407, // GL_RIGHT
    ColorAttachment0  = 0x8CE0, // GL_COLOR_ATTACHMENT0
    ColorAttachment1  = 0x8CE1, // GL_COLOR_ATTACHMENT1
    ColorAttachment2  = 0x8CE2, // GL_COLOR_ATTACHMENT2
    ColorAttachment3  = 0x8CE3, // GL_COLOR_ATTACHMENT3
    ColorAttachment4  = 0x8CE4, // GL_COLOR_ATTACHMENT4
    ColorAttachment5  = 0x8CE5, // GL_COLOR_ATTACHMENT5
    ColorAttachment6  = 0x8CE6, // GL_COLOR_ATTACHMENT6
    ColorAttachment7  = 0x8CE7, // GL_COLOR_ATTACHMENT7
    ColorAttachment8  = 0x8CE8, // GL_COLOR_ATTACHMENT8
    ColorAttachment9  = 0x8CE9, // GL_COLOR_ATTACHMENT9
    ColorAttachment10 = 0x8CEA, // GL_COLOR_ATTACHMENT10
    ColorAttachment11 = 0x8CEB, // GL_COLOR_ATTACHMENT11
    ColorAttachment12 = 0x8CEC, // GL_COLOR_ATTACHMENT12
    ColorAttachment13 = 0x8CED, // GL_COLOR_ATTACHMENT13
    ColorAttachment14 = 0x8CEE, // GL_COLOR_ATTACHMENT14
    ColorAttachment15 = 0x8CEF, // GL_COLOR_ATTACHMENT15
}; // enum class ReadBufferMode

enum class RegisterCombinerPName : GLenum {
    Combine       = 0x8570, // GL_COMBINE_ARB
    CombineRGB    = 0x8571, // GL_COMBINE_RGB_ARB
    CombineAlpha  = 0x8572, // GL_COMBINE_ALPHA_ARB
    RGBScale      = 0x8573, // GL_RGB_SCALE_ARB
    AddSigned     = 0x8574, // GL_ADD_SIGNED_ARB
    Interpolate   = 0x8575, // GL_INTERPOLATE_ARB
    Constant      = 0x8576, // GL_CONSTANT_ARB
    PrimaryColor  = 0x8577, // GL_PRIMARY_COLOR_ARB
    Previous      = 0x8578, // GL_PREVIOUS_ARB
    Source0RGB    = 0x8580, // GL_SOURCE0_RGB_ARB
    Source1RGB    = 0x8581, // GL_SOURCE1_RGB_ARB
    Source2RGB    = 0x8582, // GL_SOURCE2_RGB_ARB
    Source3RGB    = 0x8583, // GL_SOURCE3_RGB_NV
    Source0Alpha  = 0x8588, // GL_SOURCE0_ALPHA_ARB
    Source1Alpha  = 0x8589, // GL_SOURCE1_ALPHA_ARB
    Source2Alpha  = 0x858A, // GL_SOURCE2_ALPHA_ARB
    Source3Alpha  = 0x858B, // GL_SOURCE3_ALPHA_NV
    Operand0RGB   = 0x8590, // GL_OPERAND0_RGB_ARB
    Operand1RGB   = 0x8591, // GL_OPERAND1_RGB_ARB
    Operand2RGB   = 0x8592, // GL_OPERAND2_RGB_ARB
    Operand3RGB   = 0x8593, // GL_OPERAND3_RGB_NV
    Operand0Alpha = 0x8598, // GL_OPERAND0_ALPHA_ARB
    Operand1Alpha = 0x8599, // GL_OPERAND1_ALPHA_ARB
    Operand2Alpha = 0x859A, // GL_OPERAND2_ALPHA_ARB
    Operand3Alpha = 0x859B, // GL_OPERAND3_ALPHA_NV
}; // enum class RegisterCombinerPName

enum class RenderBufferParameterName : GLenum {
    RenderbufferCoverageSamples = 0x8CAB, // GL_RENDERBUFFER_COVERAGE_SAMPLES_NV
    RenderbufferWidth           = 0x8D42, // GL_RENDERBUFFER_WIDTH
    RenderbufferHeight          = 0x8D43, // GL_RENDERBUFFER_HEIGHT
    RenderbufferInternalFormat  = 0x8D44, // GL_RENDERBUFFER_INTERNAL_FORMAT
    RenderbufferRedSize         = 0x8D50, // GL_RENDERBUFFER_RED_SIZE
    RenderbufferGreenSize       = 0x8D51, // GL_RENDERBUFFER_GREEN_SIZE
    RenderbufferBlueSize        = 0x8D52, // GL_RENDERBUFFER_BLUE_SIZE
    RenderbufferAlphaSize       = 0x8D53, // GL_RENDERBUFFER_ALPHA_SIZE
    RenderbufferDepthSize       = 0x8D54, // GL_RENDERBUFFER_DEPTH_SIZE
    RenderbufferStencilSize     = 0x8D55, // GL_RENDERBUFFER_STENCIL_SIZE
    RenderbufferColorSamples    = 0x8E10, // GL_RENDERBUFFER_COLOR_SAMPLES_NV
    RenderbufferStorageSamples  = 0x91B2, // GL_RENDERBUFFER_STORAGE_SAMPLES_AMD
}; // enum class RenderBufferParameterName

enum class RenderBufferTarget : GLenum {
    Renderbuffer = 0x8D41, // GL_RENDERBUFFER
}; // enum class RenderBufferTarget

enum class ReplacementCodeTypeSUN : GLenum {
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
}; // enum class ReplacementCodeTypeSUN

enum class SamplePatternEXT : GLenum {
    _1Pass  = 0x80A1, // GL_1PASS_EXT
    _2Pass0 = 0x80A2, // GL_2PASS_0_EXT
    _2Pass1 = 0x80A3, // GL_2PASS_1_EXT
    _4Pass0 = 0x80A4, // GL_4PASS_0_EXT
    _4Pass1 = 0x80A5, // GL_4PASS_1_EXT
    _4Pass2 = 0x80A6, // GL_4PASS_2_EXT
    _4Pass3 = 0x80A7, // GL_4PASS_3_EXT
}; // enum class SamplePatternEXT

enum class SamplePatternSGIS : GLenum {
    _1Pass  = 0x80A1, // GL_1PASS_EXT
    _2Pass0 = 0x80A2, // GL_2PASS_0_EXT
    _2Pass1 = 0x80A3, // GL_2PASS_1_EXT
    _4Pass0 = 0x80A4, // GL_4PASS_0_EXT
    _4Pass1 = 0x80A5, // GL_4PASS_1_EXT
    _4Pass2 = 0x80A6, // GL_4PASS_2_EXT
    _4Pass3 = 0x80A7, // GL_4PASS_3_EXT
}; // enum class SamplePatternSGIS

enum class SamplerParameterF : GLenum {
    TextureBorderColor   = 0x1004, // GL_TEXTURE_BORDER_COLOR
    TextureMinLOD        = 0x813A, // GL_TEXTURE_MIN_LOD
    TextureMaxLOD        = 0x813B, // GL_TEXTURE_MAX_LOD
    TextureMaxAnisotropy = 0x84FE, // GL_TEXTURE_MAX_ANISOTROPY
    TextureLODBias       = 0x8501, // GL_TEXTURE_LOD_BIAS
}; // enum class SamplerParameterF

enum class SamplerParameterI : GLenum {
    TextureMagFilter   = 0x2800, // GL_TEXTURE_MAG_FILTER
    TextureMinFilter   = 0x2801, // GL_TEXTURE_MIN_FILTER
    TextureWrapS       = 0x2802, // GL_TEXTURE_WRAP_S
    TextureWrapT       = 0x2803, // GL_TEXTURE_WRAP_T
    TextureWrapR       = 0x8072, // GL_TEXTURE_WRAP_R
    TextureCompareMode = 0x884C, // GL_TEXTURE_COMPARE_MODE
    TextureCompareFunc = 0x884D, // GL_TEXTURE_COMPARE_FUNC
}; // enum class SamplerParameterI

enum class ScalarType : GLenum {
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
}; // enum class ScalarType

enum class SecondaryColorPointerTypeIBM : GLenum {
    Short  = 0x1402, // GL_SHORT
    Int    = 0x1404, // GL_INT
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class SecondaryColorPointerTypeIBM

enum class SemaphoreParameterName : GLenum {
    D3D12FenceValue       = 0x9595, // GL_D3D12_FENCE_VALUE_EXT
    SemaphoreType         = 0x95B3, // GL_SEMAPHORE_TYPE_NV
    SemaphoreTypeBinary   = 0x95B4, // GL_SEMAPHORE_TYPE_BINARY_NV
    SemaphoreTypeTimeline = 0x95B5, // GL_SEMAPHORE_TYPE_TIMELINE_NV
}; // enum class SemaphoreParameterName

enum class SeparableTargetEXT : GLenum {
    Separable2D = 0x8012, // GL_SEPARABLE_2D_EXT
}; // enum class SeparableTargetEXT

enum class ShaderBinaryFormat : GLenum {
    ShaderBinaryFormatSpirV = 0x9551, // GL_SHADER_BINARY_FORMAT_SPIR_V
}; // enum class ShaderBinaryFormat

enum class ShaderParameterName : GLenum {
    ShaderType         = 0x8B4F, // GL_SHADER_TYPE
    DeleteStatus       = 0x8B80, // GL_DELETE_STATUS
    CompileStatus      = 0x8B81, // GL_COMPILE_STATUS
    InfoLogLength      = 0x8B84, // GL_INFO_LOG_LENGTH
    ShaderSourceLength = 0x8B88, // GL_SHADER_SOURCE_LENGTH
}; // enum class ShaderParameterName

enum class ShaderType : GLenum {
    FragmentShader       = 0x8B30, // GL_FRAGMENT_SHADER
    VertexShader         = 0x8B31, // GL_VERTEX_SHADER
    GeometryShader       = 0x8DD9, // GL_GEOMETRY_SHADER
    TessEvaluationShader = 0x8E87, // GL_TESS_EVALUATION_SHADER
    TessControlShader    = 0x8E88, // GL_TESS_CONTROL_SHADER
    ComputeShader        = 0x91B9, // GL_COMPUTE_SHADER
}; // enum class ShaderType

enum class SizedInternalFormat : GLenum {
    R3G3B2                                = 0x2A10, // GL_R3_G3_B2
    Alpha4                                = 0x803B, // GL_ALPHA4_EXT
    Alpha8                                = 0x803C, // GL_ALPHA8_EXT
    Alpha12                               = 0x803D, // GL_ALPHA12_EXT
    Alpha16                               = 0x803E, // GL_ALPHA16_EXT
    Luminance4                            = 0x803F, // GL_LUMINANCE4_EXT
    Luminance8                            = 0x8040, // GL_LUMINANCE8_EXT
    Luminance12                           = 0x8041, // GL_LUMINANCE12_EXT
    Luminance16                           = 0x8042, // GL_LUMINANCE16_EXT
    Luminance4Alpha4                      = 0x8043, // GL_LUMINANCE4_ALPHA4_EXT
    Luminance6Alpha2                      = 0x8044, // GL_LUMINANCE6_ALPHA2_EXT
    Luminance8Alpha8                      = 0x8045, // GL_LUMINANCE8_ALPHA8_EXT
    Luminance12Alpha4                     = 0x8046, // GL_LUMINANCE12_ALPHA4_EXT
    Luminance12Alpha12                    = 0x8047, // GL_LUMINANCE12_ALPHA12_EXT
    Luminance16Alpha16                    = 0x8048, // GL_LUMINANCE16_ALPHA16_EXT
    Intensity4                            = 0x804A, // GL_INTENSITY4_EXT
    Intensity8                            = 0x804B, // GL_INTENSITY8_EXT
    Intensity12                           = 0x804C, // GL_INTENSITY12_EXT
    Intensity16                           = 0x804D, // GL_INTENSITY16_EXT
    RGB2                                  = 0x804E, // GL_RGB2_EXT
    RGB4                                  = 0x804F, // GL_RGB4
    RGB5                                  = 0x8050, // GL_RGB5
    RGB8                                  = 0x8051, // GL_RGB8
    RGB10                                 = 0x8052, // GL_RGB10
    RGB12                                 = 0x8053, // GL_RGB12
    RGB16                                 = 0x8054, // GL_RGB16
    RGBA2                                 = 0x8055, // GL_RGBA2
    RGBA4                                 = 0x8056, // GL_RGBA4
    RGB5A1                                = 0x8057, // GL_RGB5_A1
    RGBA8                                 = 0x8058, // GL_RGBA8
    RGB10A2                               = 0x8059, // GL_RGB10_A2
    RGBA12                                = 0x805A, // GL_RGBA12
    RGBA16                                = 0x805B, // GL_RGBA16
    DepthComponent16                      = 0x81A5, // GL_DEPTH_COMPONENT16
    DepthComponent24                      = 0x81A6, // GL_DEPTH_COMPONENT24
    DepthComponent32                      = 0x81A7, // GL_DEPTH_COMPONENT32
    R8                                    = 0x8229, // GL_R8
    R16                                   = 0x822A, // GL_R16
    RG8                                   = 0x822B, // GL_RG8
    RG16                                  = 0x822C, // GL_RG16
    R16F                                  = 0x822D, // GL_R16F
    R32F                                  = 0x822E, // GL_R32F
    RG16F                                 = 0x822F, // GL_RG16F
    RG32F                                 = 0x8230, // GL_RG32F
    R8I                                   = 0x8231, // GL_R8I
    R8UI                                  = 0x8232, // GL_R8UI
    R16I                                  = 0x8233, // GL_R16I
    R16UI                                 = 0x8234, // GL_R16UI
    R32I                                  = 0x8235, // GL_R32I
    R32UI                                 = 0x8236, // GL_R32UI
    RG8I                                  = 0x8237, // GL_RG8I
    RG8UI                                 = 0x8238, // GL_RG8UI
    RG16I                                 = 0x8239, // GL_RG16I
    RG16UI                                = 0x823A, // GL_RG16UI
    RG32I                                 = 0x823B, // GL_RG32I
    RG32UI                                = 0x823C, // GL_RG32UI
    CompressedRGBS3TcDXT1                 = 0x83F0, // GL_COMPRESSED_RGB_S3TC_DXT1_EXT
    CompressedRGBAS3TcDXT1                = 0x83F1, // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    CompressedRGBAS3TcDXT3                = 0x83F2, // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    CompressedRGBAS3TcDXT5                = 0x83F3, // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    RGBA32F                               = 0x8814, // GL_RGBA32F
    RGB32F                                = 0x8815, // GL_RGB32F
    RGBA16F                               = 0x881A, // GL_RGBA16F
    RGB16F                                = 0x881B, // GL_RGB16F
    Depth24Stencil8                       = 0x88F0, // GL_DEPTH24_STENCIL8
    R11FG11FB10F                          = 0x8C3A, // GL_R11F_G11F_B10F
    RGB9E5                                = 0x8C3D, // GL_RGB9_E5
    SRGB8                                 = 0x8C41, // GL_SRGB8
    SRGB8Alpha8                           = 0x8C43, // GL_SRGB8_ALPHA8
    CompressedSRGBS3TcDXT1                = 0x8C4C, // GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
    CompressedSRGBAlphaS3TcDXT1           = 0x8C4D, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
    CompressedSRGBAlphaS3TcDXT3           = 0x8C4E, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
    CompressedSRGBAlphaS3TcDXT5           = 0x8C4F, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
    DepthComponent32F                     = 0x8CAC, // GL_DEPTH_COMPONENT32F
    Depth32FStencil8                      = 0x8CAD, // GL_DEPTH32F_STENCIL8
    StencilIndex1                         = 0x8D46, // GL_STENCIL_INDEX1
    StencilIndex4                         = 0x8D47, // GL_STENCIL_INDEX4
    StencilIndex8                         = 0x8D48, // GL_STENCIL_INDEX8
    StencilIndex16                        = 0x8D49, // GL_STENCIL_INDEX16
    RGB565                                = 0x8D62, // GL_RGB565
    RGBA32UI                              = 0x8D70, // GL_RGBA32UI
    RGB32UI                               = 0x8D71, // GL_RGB32UI
    Alpha32UI                             = 0x8D72, // GL_ALPHA32UI_EXT
    Intensity32UI                         = 0x8D73, // GL_INTENSITY32UI_EXT
    Luminance32UI                         = 0x8D74, // GL_LUMINANCE32UI_EXT
    LuminanceAlpha32UI                    = 0x8D75, // GL_LUMINANCE_ALPHA32UI_EXT
    RGBA16UI                              = 0x8D76, // GL_RGBA16UI
    RGB16UI                               = 0x8D77, // GL_RGB16UI
    Alpha16UI                             = 0x8D78, // GL_ALPHA16UI_EXT
    Intensity16UI                         = 0x8D79, // GL_INTENSITY16UI_EXT
    Luminance16UI                         = 0x8D7A, // GL_LUMINANCE16UI_EXT
    LuminanceAlpha16UI                    = 0x8D7B, // GL_LUMINANCE_ALPHA16UI_EXT
    RGBA8UI                               = 0x8D7C, // GL_RGBA8UI
    RGB8UI                                = 0x8D7D, // GL_RGB8UI
    Alpha8UI                              = 0x8D7E, // GL_ALPHA8UI_EXT
    Intensity8UI                          = 0x8D7F, // GL_INTENSITY8UI_EXT
    Luminance8UI                          = 0x8D80, // GL_LUMINANCE8UI_EXT
    LuminanceAlpha8UI                     = 0x8D81, // GL_LUMINANCE_ALPHA8UI_EXT
    RGBA32I                               = 0x8D82, // GL_RGBA32I
    RGB32I                                = 0x8D83, // GL_RGB32I
    Alpha32I                              = 0x8D84, // GL_ALPHA32I_EXT
    Intensity32I                          = 0x8D85, // GL_INTENSITY32I_EXT
    Luminance32I                          = 0x8D86, // GL_LUMINANCE32I_EXT
    LuminanceAlpha32I                     = 0x8D87, // GL_LUMINANCE_ALPHA32I_EXT
    RGBA16I                               = 0x8D88, // GL_RGBA16I
    RGB16I                                = 0x8D89, // GL_RGB16I
    Alpha16I                              = 0x8D8A, // GL_ALPHA16I_EXT
    Intensity16I                          = 0x8D8B, // GL_INTENSITY16I_EXT
    Luminance16I                          = 0x8D8C, // GL_LUMINANCE16I_EXT
    LuminanceAlpha16I                     = 0x8D8D, // GL_LUMINANCE_ALPHA16I_EXT
    RGBA8I                                = 0x8D8E, // GL_RGBA8I
    RGB8I                                 = 0x8D8F, // GL_RGB8I
    Alpha8I                               = 0x8D90, // GL_ALPHA8I_EXT
    Intensity8I                           = 0x8D91, // GL_INTENSITY8I_EXT
    Luminance8I                           = 0x8D92, // GL_LUMINANCE8I_EXT
    LuminanceAlpha8I                      = 0x8D93, // GL_LUMINANCE_ALPHA8I_EXT
    DepthComponent32FNV                   = 0x8DAB, // GL_DEPTH_COMPONENT32F_NV
    Depth32FStencil8NV                    = 0x8DAC, // GL_DEPTH32F_STENCIL8_NV
    CompressedRedRGTC1                    = 0x8DBB, // GL_COMPRESSED_RED_RGTC1
    CompressedSignedRedRGTC1              = 0x8DBC, // GL_COMPRESSED_SIGNED_RED_RGTC1
    CompressedRedGreenRGTC2               = 0x8DBD, // GL_COMPRESSED_RED_GREEN_RGTC2_EXT
    CompressedSignedRedGreenRGTC2         = 0x8DBE, // GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT
    CompressedRGBABPTCUnorm               = 0x8E8C, // GL_COMPRESSED_RGBA_BPTC_UNORM
    CompressedSRGBAlphaBPTCUnorm          = 0x8E8D, // GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM
    CompressedRGBBPTCSignedFloat          = 0x8E8E, // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT
    CompressedRGBBPTCUnsignedFloat        = 0x8E8F, // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
    R8Snorm                               = 0x8F94, // GL_R8_SNORM
    RG8Snorm                              = 0x8F95, // GL_RG8_SNORM
    RGB8Snorm                             = 0x8F96, // GL_RGB8_SNORM
    RGBA8Snorm                            = 0x8F97, // GL_RGBA8_SNORM
    R16Snorm                              = 0x8F98, // GL_R16_SNORM
    RG16Snorm                             = 0x8F99, // GL_RG16_SNORM
    RGB16Snorm                            = 0x8F9A, // GL_RGB16_SNORM
    RGBA16Snorm                           = 0x8F9B, // GL_RGBA16_SNORM
    RGB10A2UI                             = 0x906F, // GL_RGB10_A2UI
    CompressedR11EAC                      = 0x9270, // GL_COMPRESSED_R11_EAC
    CompressedSignedR11EAC                = 0x9271, // GL_COMPRESSED_SIGNED_R11_EAC
    CompressedRG11EAC                     = 0x9272, // GL_COMPRESSED_RG11_EAC
    CompressedSignedRG11EAC               = 0x9273, // GL_COMPRESSED_SIGNED_RG11_EAC
    CompressedRGB8ETC2                    = 0x9274, // GL_COMPRESSED_RGB8_ETC2
    CompressedSRGB8ETC2                   = 0x9275, // GL_COMPRESSED_SRGB8_ETC2
    CompressedRGB8PunchthroughAlpha1ETC2  = 0x9276, // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    CompressedSRGB8PunchthroughAlpha1ETC2 = 0x9277, // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    CompressedRGBA8ETC2EAC                = 0x9278, // GL_COMPRESSED_RGBA8_ETC2_EAC
    CompressedSRGB8Alpha8ETC2EAC          = 0x9279, // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    CompressedRGBAASTC4x4                 = 0x93B0, // GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    CompressedRGBAASTC5x4                 = 0x93B1, // GL_COMPRESSED_RGBA_ASTC_5x4_KHR
    CompressedRGBAASTC5x5                 = 0x93B2, // GL_COMPRESSED_RGBA_ASTC_5x5_KHR
    CompressedRGBAASTC6x5                 = 0x93B3, // GL_COMPRESSED_RGBA_ASTC_6x5_KHR
    CompressedRGBAASTC6x6                 = 0x93B4, // GL_COMPRESSED_RGBA_ASTC_6x6_KHR
    CompressedRGBAASTC8x5                 = 0x93B5, // GL_COMPRESSED_RGBA_ASTC_8x5_KHR
    CompressedRGBAASTC8x6                 = 0x93B6, // GL_COMPRESSED_RGBA_ASTC_8x6_KHR
    CompressedRGBAASTC8x8                 = 0x93B7, // GL_COMPRESSED_RGBA_ASTC_8x8_KHR
    CompressedRGBAASTC10x5                = 0x93B8, // GL_COMPRESSED_RGBA_ASTC_10x5_KHR
    CompressedRGBAASTC10x6                = 0x93B9, // GL_COMPRESSED_RGBA_ASTC_10x6_KHR
    CompressedRGBAASTC10x8                = 0x93BA, // GL_COMPRESSED_RGBA_ASTC_10x8_KHR
    CompressedRGBAASTC10x10               = 0x93BB, // GL_COMPRESSED_RGBA_ASTC_10x10_KHR
    CompressedRGBAASTC12x10               = 0x93BC, // GL_COMPRESSED_RGBA_ASTC_12x10_KHR
    CompressedRGBAASTC12x12               = 0x93BD, // GL_COMPRESSED_RGBA_ASTC_12x12_KHR
    CompressedSRGB8Alpha8ASTC4x4          = 0x93D0, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    CompressedSRGB8Alpha8ASTC5x4          = 0x93D1, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR
    CompressedSRGB8Alpha8ASTC5x5          = 0x93D2, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR
    CompressedSRGB8Alpha8ASTC6x5          = 0x93D3, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR
    CompressedSRGB8Alpha8ASTC6x6          = 0x93D4, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR
    CompressedSRGB8Alpha8ASTC8x5          = 0x93D5, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR
    CompressedSRGB8Alpha8ASTC8x6          = 0x93D6, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR
    CompressedSRGB8Alpha8ASTC8x8          = 0x93D7, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR
    CompressedSRGB8Alpha8ASTC10x5         = 0x93D8, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR
    CompressedSRGB8Alpha8ASTC10x6         = 0x93D9, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR
    CompressedSRGB8Alpha8ASTC10x8         = 0x93DA, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR
    CompressedSRGB8Alpha8ASTC10x10        = 0x93DB, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR
    CompressedSRGB8Alpha8ASTC12x10        = 0x93DC, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR
    CompressedSRGB8Alpha8ASTC12x12        = 0x93DD, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR
}; // enum class SizedInternalFormat

enum class SpriteParameterNameSGIX : GLenum {
    SpriteMode = 0x8149, // GL_SPRITE_MODE_SGIX
}; // enum class SpriteParameterNameSGIX

enum class StencilFaceDirection : GLenum {
    Front        = 0x0404, // GL_FRONT
    Back         = 0x0405, // GL_BACK
    FrontAndBack = 0x0408, // GL_FRONT_AND_BACK
}; // enum class StencilFaceDirection

enum class StencilFunction : GLenum {
    Never    = 0x0200, // GL_NEVER
    Less     = 0x0201, // GL_LESS
    Equal    = 0x0202, // GL_EQUAL
    Lequal   = 0x0203, // GL_LEQUAL
    Greater  = 0x0204, // GL_GREATER
    Notequal = 0x0205, // GL_NOTEQUAL
    Gequal   = 0x0206, // GL_GEQUAL
    Always   = 0x0207, // GL_ALWAYS
}; // enum class StencilFunction

enum class StencilOp : GLenum {
    Zero     =      0, // GL_ZERO
    Invert   = 0x150A, // GL_INVERT
    Keep     = 0x1E00, // GL_KEEP
    Replace  = 0x1E01, // GL_REPLACE
    Incr     = 0x1E02, // GL_INCR
    Decr     = 0x1E03, // GL_DECR
    IncrWrap = 0x8507, // GL_INCR_WRAP
    DecrWrap = 0x8508, // GL_DECR_WRAP
}; // enum class StencilOp

enum class StringName : GLenum {
    Vendor                 = 0x1F00, // GL_VENDOR
    Renderer               = 0x1F01, // GL_RENDERER
    Version                = 0x1F02, // GL_VERSION
    Extensions             = 0x1F03, // GL_EXTENSIONS
    ShadingLanguageVersion = 0x8B8C, // GL_SHADING_LANGUAGE_VERSION
}; // enum class StringName

struct SubgroupSupportedFeatures {
    static constexpr GLenum SubgroupFeatureBasic           = 0x00000001; // GL_SUBGROUP_FEATURE_BASIC_BIT_KHR
    static constexpr GLenum SubgroupFeatureVote            = 0x00000002; // GL_SUBGROUP_FEATURE_VOTE_BIT_KHR
    static constexpr GLenum SubgroupFeatureArithmetic      = 0x00000004; // GL_SUBGROUP_FEATURE_ARITHMETIC_BIT_KHR
    static constexpr GLenum SubgroupFeatureBallot          = 0x00000008; // GL_SUBGROUP_FEATURE_BALLOT_BIT_KHR
    static constexpr GLenum SubgroupFeatureShuffle         = 0x00000010; // GL_SUBGROUP_FEATURE_SHUFFLE_BIT_KHR
    static constexpr GLenum SubgroupFeatureShuffleRelative = 0x00000020; // GL_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT_KHR
    static constexpr GLenum SubgroupFeatureClustered       = 0x00000040; // GL_SUBGROUP_FEATURE_CLUSTERED_BIT_KHR
    static constexpr GLenum SubgroupFeatureQuad            = 0x00000080; // GL_SUBGROUP_FEATURE_QUAD_BIT_KHR
    static constexpr GLenum SubgroupFeaturePartitioned     = 0x00000100; // GL_SUBGROUP_FEATURE_PARTITIONED_BIT_NV
}; // struct SubgroupSupportedFeatures

enum class SubroutineParameterName : GLenum {
    UniformSize              = 0x8A38, // GL_UNIFORM_SIZE
    UniformNameLength        = 0x8A39, // GL_UNIFORM_NAME_LENGTH
    NumCompatibleSubroutines = 0x8E4A, // GL_NUM_COMPATIBLE_SUBROUTINES
    CompatibleSubroutines    = 0x8E4B, // GL_COMPATIBLE_SUBROUTINES
}; // enum class SubroutineParameterName

enum class SwizzleOpATI : GLenum {
    SwizzleStr   = 0x8976, // GL_SWIZZLE_STR_ATI
    SwizzleStq   = 0x8977, // GL_SWIZZLE_STQ_ATI
    SwizzleStrDr = 0x8978, // GL_SWIZZLE_STR_DR_ATI
    SwizzleStqDq = 0x8979, // GL_SWIZZLE_STQ_DQ_ATI
}; // enum class SwizzleOpATI

enum class SyncBehaviorFlags : GLbitfield {
    None = 0, // GL_NONE
}; // enum class SyncBehaviorFlags

enum class SyncCondition : GLenum {
    SyncGPUCommandsComplete = 0x9117, // GL_SYNC_GPU_COMMANDS_COMPLETE
}; // enum class SyncCondition

struct SyncObjectMask {
    static constexpr GLbitfield SyncFlushCommands = 0x00000001; // GL_SYNC_FLUSH_COMMANDS_BIT
}; // struct SyncObjectMask

enum class SyncParameterName : GLenum {
    ObjectType    = 0x9112, // GL_OBJECT_TYPE
    SyncCondition = 0x9113, // GL_SYNC_CONDITION
    SyncStatus    = 0x9114, // GL_SYNC_STATUS
    SyncFlags     = 0x9115, // GL_SYNC_FLAGS
}; // enum class SyncParameterName

enum class SyncStatus : GLenum {
    AlreadySignaled    = 0x911A, // GL_ALREADY_SIGNALED
    TimeoutExpired     = 0x911B, // GL_TIMEOUT_EXPIRED
    ConditionSatisfied = 0x911C, // GL_CONDITION_SATISFIED
    WaitFailed         = 0x911D, // GL_WAIT_FAILED
}; // enum class SyncStatus

enum class TangentPointerTypeEXT : GLenum {
    Byte   = 0x1400, // GL_BYTE
    Short  = 0x1402, // GL_SHORT
    Int    = 0x1404, // GL_INT
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class TangentPointerTypeEXT

enum class TexBumpParameterATI : GLenum {
    BumpRotMatrix = 0x8775, // GL_BUMP_ROT_MATRIX_ATI
}; // enum class TexBumpParameterATI

enum class TexCoordPointerType : GLenum {
    Short  = 0x1402, // GL_SHORT
    Int    = 0x1404, // GL_INT
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class TexCoordPointerType

enum class TextureCompareMode : GLenum {
    None                =      0, // GL_NONE
    CompareRefToTexture = 0x884E, // GL_COMPARE_REF_TO_TEXTURE
}; // enum class TextureCompareMode

enum class TextureEnvMode : GLenum {
    Blend          = 0x0BE2, // GL_BLEND
    Replace        = 0x8062, // GL_REPLACE_EXT
    TextureEnvBias = 0x80BE, // GL_TEXTURE_ENV_BIAS_SGIX
}; // enum class TextureEnvMode

enum class TextureFilterFuncSGIS : GLenum {
    Filter4 = 0x8146, // GL_FILTER4_SGIS
}; // enum class TextureFilterFuncSGIS

enum class TextureFilterSGIS : GLenum {
    Filter4 = 0x8146, // GL_FILTER4_SGIS
}; // enum class TextureFilterSGIS

enum class TextureGenMode : GLenum {
    EyeDistanceToPoint    = 0x81F0, // GL_EYE_DISTANCE_TO_POINT_SGIS
    ObjectDistanceToPoint = 0x81F1, // GL_OBJECT_DISTANCE_TO_POINT_SGIS
    EyeDistanceToLine     = 0x81F2, // GL_EYE_DISTANCE_TO_LINE_SGIS
    ObjectDistanceToLine  = 0x81F3, // GL_OBJECT_DISTANCE_TO_LINE_SGIS
}; // enum class TextureGenMode

enum class TextureGenParameter : GLenum {
    EyePlane    = 0x2502, // GL_EYE_PLANE
    EyePoint    = 0x81F4, // GL_EYE_POINT_SGIS
    ObjectPoint = 0x81F5, // GL_OBJECT_POINT_SGIS
    EyeLine     = 0x81F6, // GL_EYE_LINE_SGIS
    ObjectLine  = 0x81F7, // GL_OBJECT_LINE_SGIS
}; // enum class TextureGenParameter

enum class TextureLayout : GLenum {
    LayoutDepthReadOnlyStencilAttachment = 0x9530, // GL_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_EXT
    LayoutDepthAttachmentStencilReadOnly = 0x9531, // GL_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_EXT
    LayoutGeneral                        = 0x958D, // GL_LAYOUT_GENERAL_EXT
    LayoutColorAttachment                = 0x958E, // GL_LAYOUT_COLOR_ATTACHMENT_EXT
    LayoutDepthStencilAttachment         = 0x958F, // GL_LAYOUT_DEPTH_STENCIL_ATTACHMENT_EXT
    LayoutDepthStencilReadOnly           = 0x9590, // GL_LAYOUT_DEPTH_STENCIL_READ_ONLY_EXT
    LayoutShaderReadOnly                 = 0x9591, // GL_LAYOUT_SHADER_READ_ONLY_EXT
    LayoutTransferSrc                    = 0x9592, // GL_LAYOUT_TRANSFER_SRC_EXT
    LayoutTransferDst                    = 0x9593, // GL_LAYOUT_TRANSFER_DST_EXT
}; // enum class TextureLayout

enum class TextureMagFilter : GLenum {
    Nearest            = 0x2600, // GL_NEAREST
    Linear             = 0x2601, // GL_LINEAR
    LinearDetail       = 0x8097, // GL_LINEAR_DETAIL_SGIS
    LinearDetailAlpha  = 0x8098, // GL_LINEAR_DETAIL_ALPHA_SGIS
    LinearDetailColor  = 0x8099, // GL_LINEAR_DETAIL_COLOR_SGIS
    LinearSharpen      = 0x80AD, // GL_LINEAR_SHARPEN_SGIS
    LinearSharpenAlpha = 0x80AE, // GL_LINEAR_SHARPEN_ALPHA_SGIS
    LinearSharpenColor = 0x80AF, // GL_LINEAR_SHARPEN_COLOR_SGIS
    Filter4            = 0x8146, // GL_FILTER4_SGIS
}; // enum class TextureMagFilter

enum class TextureMinFilter : GLenum {
    Nearest               = 0x2600, // GL_NEAREST
    Linear                = 0x2601, // GL_LINEAR
    NearestMipmapNearest  = 0x2700, // GL_NEAREST_MIPMAP_NEAREST
    LinearMipmapNearest   = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
    NearestMipmapLinear   = 0x2702, // GL_NEAREST_MIPMAP_LINEAR
    LinearMipmapLinear    = 0x2703, // GL_LINEAR_MIPMAP_LINEAR
    Filter4               = 0x8146, // GL_FILTER4_SGIS
    LinearClipmapLinear   = 0x8170, // GL_LINEAR_CLIPMAP_LINEAR_SGIX
    NearestClipmapNearest = 0x844D, // GL_NEAREST_CLIPMAP_NEAREST_SGIX
    NearestClipmapLinear  = 0x844E, // GL_NEAREST_CLIPMAP_LINEAR_SGIX
    LinearClipmapNearest  = 0x844F, // GL_LINEAR_CLIPMAP_NEAREST_SGIX
}; // enum class TextureMinFilter

enum class TextureNormalModeEXT : GLenum {
    Perturb = 0x85AE, // GL_PERTURB_EXT
}; // enum class TextureNormalModeEXT

enum class TextureParameterName : GLenum {
    TextureWidth               = 0x1000, // GL_TEXTURE_WIDTH
    TextureHeight              = 0x1001, // GL_TEXTURE_HEIGHT
    TextureInternalFormat      = 0x1003, // GL_TEXTURE_INTERNAL_FORMAT
    TextureBorderColor         = 0x1004, // GL_TEXTURE_BORDER_COLOR
    TextureMagFilter           = 0x2800, // GL_TEXTURE_MAG_FILTER
    TextureMinFilter           = 0x2801, // GL_TEXTURE_MIN_FILTER
    TextureWrapS               = 0x2802, // GL_TEXTURE_WRAP_S
    TextureWrapT               = 0x2803, // GL_TEXTURE_WRAP_T
    TextureRedSize             = 0x805C, // GL_TEXTURE_RED_SIZE
    TextureGreenSize           = 0x805D, // GL_TEXTURE_GREEN_SIZE
    TextureBlueSize            = 0x805E, // GL_TEXTURE_BLUE_SIZE
    TextureAlphaSize           = 0x805F, // GL_TEXTURE_ALPHA_SIZE
    TexturePriority            = 0x8066, // GL_TEXTURE_PRIORITY_EXT
    TextureDepth               = 0x8071, // GL_TEXTURE_DEPTH_EXT
    TextureWrapR               = 0x8072, // GL_TEXTURE_WRAP_R
    DetailTextureLevel         = 0x809A, // GL_DETAIL_TEXTURE_LEVEL_SGIS
    DetailTextureMode          = 0x809B, // GL_DETAIL_TEXTURE_MODE_SGIS
    DetailTextureFuncPoints    = 0x809C, // GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS
    SharpenTextureFuncPoints   = 0x80B0, // GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS
    ShadowAmbient              = 0x80BF, // GL_SHADOW_AMBIENT_SGIX
    DualTextureSelect          = 0x8124, // GL_DUAL_TEXTURE_SELECT_SGIS
    QuadTextureSelect          = 0x8125, // GL_QUAD_TEXTURE_SELECT_SGIS
    Texture4DSIZE              = 0x8136, // GL_TEXTURE_4DSIZE_SGIS
    TextureWrapQ               = 0x8137, // GL_TEXTURE_WRAP_Q_SGIS
    TextureMinLOD              = 0x813A, // GL_TEXTURE_MIN_LOD
    TextureMaxLOD              = 0x813B, // GL_TEXTURE_MAX_LOD
    TextureBaseLevel           = 0x813C, // GL_TEXTURE_BASE_LEVEL
    TextureMaxLevel            = 0x813D, // GL_TEXTURE_MAX_LEVEL
    TextureFilter4Size         = 0x8147, // GL_TEXTURE_FILTER4_SIZE_SGIS
    TextureClipmapCenter       = 0x8171, // GL_TEXTURE_CLIPMAP_CENTER_SGIX
    TextureClipmapFrame        = 0x8172, // GL_TEXTURE_CLIPMAP_FRAME_SGIX
    TextureClipmapOffset       = 0x8173, // GL_TEXTURE_CLIPMAP_OFFSET_SGIX
    TextureClipmapVirtualDepth = 0x8174, // GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX
    TextureClipmapLODOffset    = 0x8175, // GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX
    TextureClipmapDepth        = 0x8176, // GL_TEXTURE_CLIPMAP_DEPTH_SGIX
    PostTextureFilterBias      = 0x8179, // GL_POST_TEXTURE_FILTER_BIAS_SGIX
    PostTextureFilterScale     = 0x817A, // GL_POST_TEXTURE_FILTER_SCALE_SGIX
    TextureLODBiasS            = 0x818E, // GL_TEXTURE_LOD_BIAS_S_SGIX
    TextureLODBiasT            = 0x818F, // GL_TEXTURE_LOD_BIAS_T_SGIX
    TextureLODBiasR            = 0x8190, // GL_TEXTURE_LOD_BIAS_R_SGIX
    GenerateMipmap             = 0x8191, // GL_GENERATE_MIPMAP_SGIS
    TextureCompare             = 0x819A, // GL_TEXTURE_COMPARE_SGIX
    TextureCompareOperator     = 0x819B, // GL_TEXTURE_COMPARE_OPERATOR_SGIX
    TextureLequalR             = 0x819C, // GL_TEXTURE_LEQUAL_R_SGIX
    TextureGequalR             = 0x819D, // GL_TEXTURE_GEQUAL_R_SGIX
    TextureMaxClampS           = 0x8369, // GL_TEXTURE_MAX_CLAMP_S_SGIX
    TextureMaxClampT           = 0x836A, // GL_TEXTURE_MAX_CLAMP_T_SGIX
    TextureMaxClampR           = 0x836B, // GL_TEXTURE_MAX_CLAMP_R_SGIX
    TextureMaxAnisotropy       = 0x84FE, // GL_TEXTURE_MAX_ANISOTROPY
    TextureLODBias             = 0x8501, // GL_TEXTURE_LOD_BIAS
    TextureCompareMode         = 0x884C, // GL_TEXTURE_COMPARE_MODE
    TextureCompareFunc         = 0x884D, // GL_TEXTURE_COMPARE_FUNC
    TextureSwizzleR            = 0x8E42, // GL_TEXTURE_SWIZZLE_R
    TextureSwizzleG            = 0x8E43, // GL_TEXTURE_SWIZZLE_G
    TextureSwizzleB            = 0x8E44, // GL_TEXTURE_SWIZZLE_B
    TextureSwizzleA            = 0x8E45, // GL_TEXTURE_SWIZZLE_A
    TextureSwizzleRGBA         = 0x8E46, // GL_TEXTURE_SWIZZLE_RGBA
    DepthStencilTextureMode    = 0x90EA, // GL_DEPTH_STENCIL_TEXTURE_MODE
    TextureTiling              = 0x9580, // GL_TEXTURE_TILING_EXT
}; // enum class TextureParameterName

struct TextureStorageMaskAMD {
    static constexpr GLbitfield TextureStorageSparse = 0x00000001; // GL_TEXTURE_STORAGE_SPARSE_BIT_AMD
}; // struct TextureStorageMaskAMD

enum class TextureSwizzle : GLenum {
    Zero  =      0, // GL_ZERO
    One   =      1, // GL_ONE
    Red   = 0x1903, // GL_RED
    Green = 0x1904, // GL_GREEN
    Blue  = 0x1905, // GL_BLUE
    Alpha = 0x1906, // GL_ALPHA
}; // enum class TextureSwizzle

enum class TextureTarget : GLenum {
    Texture1D                      = 0x0DE0, // GL_TEXTURE_1D
    Texture2D                      = 0x0DE1, // GL_TEXTURE_2D
    ProxyTexture1D                 = 0x8063, // GL_PROXY_TEXTURE_1D
    ProxyTexture2D                 = 0x8064, // GL_PROXY_TEXTURE_2D
    Texture3D                      = 0x806F, // GL_TEXTURE_3D
    ProxyTexture3D                 = 0x8070, // GL_PROXY_TEXTURE_3D
    DetailTexture2D                = 0x8095, // GL_DETAIL_TEXTURE_2D_SGIS
    Texture4D                      = 0x8134, // GL_TEXTURE_4D_SGIS
    ProxyTexture4D                 = 0x8135, // GL_PROXY_TEXTURE_4D_SGIS
    TextureRectangle               = 0x84F5, // GL_TEXTURE_RECTANGLE
    ProxyTextureRectangle          = 0x84F7, // GL_PROXY_TEXTURE_RECTANGLE
    TextureCubeMap                 = 0x8513, // GL_TEXTURE_CUBE_MAP
    TextureCubeMapPositiveX        = 0x8515, // GL_TEXTURE_CUBE_MAP_POSITIVE_X
    TextureCubeMapNegativeX        = 0x8516, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    TextureCubeMapPositiveY        = 0x8517, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    TextureCubeMapNegativeY        = 0x8518, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    TextureCubeMapPositiveZ        = 0x8519, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    TextureCubeMapNegativeZ        = 0x851A, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    ProxyTextureCubeMap            = 0x851B, // GL_PROXY_TEXTURE_CUBE_MAP
    Texture1DArray                 = 0x8C18, // GL_TEXTURE_1D_ARRAY
    ProxyTexture1DArray            = 0x8C19, // GL_PROXY_TEXTURE_1D_ARRAY
    Texture2DArray                 = 0x8C1A, // GL_TEXTURE_2D_ARRAY
    ProxyTexture2DArray            = 0x8C1B, // GL_PROXY_TEXTURE_2D_ARRAY
    TextureBuffer                  = 0x8C2A, // GL_TEXTURE_BUFFER
    Renderbuffer                   = 0x8D41, // GL_RENDERBUFFER
    TextureCubeMapArray            = 0x9009, // GL_TEXTURE_CUBE_MAP_ARRAY
    ProxyTextureCubeMapArray       = 0x900B, // GL_PROXY_TEXTURE_CUBE_MAP_ARRAY
    Texture2DMultisample           = 0x9100, // GL_TEXTURE_2D_MULTISAMPLE
    ProxyTexture2DMultisample      = 0x9101, // GL_PROXY_TEXTURE_2D_MULTISAMPLE
    Texture2DMultisampleArray      = 0x9102, // GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    ProxyTexture2DMultisampleArray = 0x9103, // GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY
}; // enum class TextureTarget

enum class TextureUnit : GLenum {
    Texture0  = 0x84C0, // GL_TEXTURE0
    Texture1  = 0x84C1, // GL_TEXTURE1
    Texture2  = 0x84C2, // GL_TEXTURE2
    Texture3  = 0x84C3, // GL_TEXTURE3
    Texture4  = 0x84C4, // GL_TEXTURE4
    Texture5  = 0x84C5, // GL_TEXTURE5
    Texture6  = 0x84C6, // GL_TEXTURE6
    Texture7  = 0x84C7, // GL_TEXTURE7
    Texture8  = 0x84C8, // GL_TEXTURE8
    Texture9  = 0x84C9, // GL_TEXTURE9
    Texture10 = 0x84CA, // GL_TEXTURE10
    Texture11 = 0x84CB, // GL_TEXTURE11
    Texture12 = 0x84CC, // GL_TEXTURE12
    Texture13 = 0x84CD, // GL_TEXTURE13
    Texture14 = 0x84CE, // GL_TEXTURE14
    Texture15 = 0x84CF, // GL_TEXTURE15
    Texture16 = 0x84D0, // GL_TEXTURE16
    Texture17 = 0x84D1, // GL_TEXTURE17
    Texture18 = 0x84D2, // GL_TEXTURE18
    Texture19 = 0x84D3, // GL_TEXTURE19
    Texture20 = 0x84D4, // GL_TEXTURE20
    Texture21 = 0x84D5, // GL_TEXTURE21
    Texture22 = 0x84D6, // GL_TEXTURE22
    Texture23 = 0x84D7, // GL_TEXTURE23
    Texture24 = 0x84D8, // GL_TEXTURE24
    Texture25 = 0x84D9, // GL_TEXTURE25
    Texture26 = 0x84DA, // GL_TEXTURE26
    Texture27 = 0x84DB, // GL_TEXTURE27
    Texture28 = 0x84DC, // GL_TEXTURE28
    Texture29 = 0x84DD, // GL_TEXTURE29
    Texture30 = 0x84DE, // GL_TEXTURE30
    Texture31 = 0x84DF, // GL_TEXTURE31
}; // enum class TextureUnit

enum class TextureWrapMode : GLenum {
    LinearMipmapLinear = 0x2703, // GL_LINEAR_MIPMAP_LINEAR
    Repeat             = 0x2901, // GL_REPEAT
    ClampToBorder      = 0x812D, // GL_CLAMP_TO_BORDER
    ClampToEdge        = 0x812F, // GL_CLAMP_TO_EDGE
    MirroredRepeat     = 0x8370, // GL_MIRRORED_REPEAT
}; // enum class TextureWrapMode

enum class TransformFeedbackBufferMode : GLenum {
    InterleavedAttribs = 0x8C8C, // GL_INTERLEAVED_ATTRIBS
    SeparateAttribs    = 0x8C8D, // GL_SEPARATE_ATTRIBS
}; // enum class TransformFeedbackBufferMode

enum class TransformFeedbackPName : GLenum {
    TransformFeedbackBufferStart   = 0x8C84, // GL_TRANSFORM_FEEDBACK_BUFFER_START
    TransformFeedbackBufferSize    = 0x8C85, // GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    TransformFeedbackBufferBinding = 0x8C8F, // GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    TransformFeedbackPaused        = 0x8E23, // GL_TRANSFORM_FEEDBACK_PAUSED
    TransformFeedbackActive        = 0x8E24, // GL_TRANSFORM_FEEDBACK_ACTIVE
}; // enum class TransformFeedbackPName

// For NV_transform_feedback. No clue why small negative values are used
enum class TransformFeedbackTokenNV : GLenum {
    SkipComponents1 = static_cast<GLenum>(-6), // GL_SKIP_COMPONENTS1_NV
    SkipComponents2 = static_cast<GLenum>(-5), // GL_SKIP_COMPONENTS2_NV
    SkipComponents3 = static_cast<GLenum>(-4), // GL_SKIP_COMPONENTS3_NV
    SkipComponents4 = static_cast<GLenum>(-3), // GL_SKIP_COMPONENTS4_NV
    NextBuffer      = static_cast<GLenum>(-2), // GL_NEXT_BUFFER_NV
}; // enum class TransformFeedbackTokenNV

enum class TriangleListSUN : GLenum {
    Restart       = 0x0001, // GL_RESTART_SUN
    ReplaceMiddle = 0x0002, // GL_REPLACE_MIDDLE_SUN
    ReplaceOldest = 0x0003, // GL_REPLACE_OLDEST_SUN
}; // enum class TriangleListSUN

enum class UniformBlockPName : GLenum {
    UniformBlockReferencedByTessControlShader    = 0x84F0, // GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER
    UniformBlockReferencedByTessEvaluationShader = 0x84F1, // GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER
    UniformBlockBinding                          = 0x8A3F, // GL_UNIFORM_BLOCK_BINDING
    UniformBlockDataSize                         = 0x8A40, // GL_UNIFORM_BLOCK_DATA_SIZE
    UniformBlockNameLength                       = 0x8A41, // GL_UNIFORM_BLOCK_NAME_LENGTH
    UniformBlockActiveUniforms                   = 0x8A42, // GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS
    UniformBlockActiveUniformIndices             = 0x8A43, // GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES
    UniformBlockReferencedByVertexShader         = 0x8A44, // GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER
    UniformBlockReferencedByGeometryShader       = 0x8A45, // GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER
    UniformBlockReferencedByFragmentShader       = 0x8A46, // GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER
    UniformBlockReferencedByComputeShader        = 0x90EC, // GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER
}; // enum class UniformBlockPName

enum class UniformPName : GLenum {
    UniformType                     = 0x8A37, // GL_UNIFORM_TYPE
    UniformSize                     = 0x8A38, // GL_UNIFORM_SIZE
    UniformNameLength               = 0x8A39, // GL_UNIFORM_NAME_LENGTH
    UniformBlockIndex               = 0x8A3A, // GL_UNIFORM_BLOCK_INDEX
    UniformOffset                   = 0x8A3B, // GL_UNIFORM_OFFSET
    UniformArrayStride              = 0x8A3C, // GL_UNIFORM_ARRAY_STRIDE
    UniformMatrixStride             = 0x8A3D, // GL_UNIFORM_MATRIX_STRIDE
    UniformIsRowMajor               = 0x8A3E, // GL_UNIFORM_IS_ROW_MAJOR
    UniformAtomicCounterBufferIndex = 0x92DA, // GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX
}; // enum class UniformPName

enum class UniformType : GLenum {
    Int                                  = 0x1404, // GL_INT
    UnsignedInt                          = 0x1405, // GL_UNSIGNED_INT
    Float                                = 0x1406, // GL_FLOAT
    Double                               = 0x140A, // GL_DOUBLE
    FloatVec2                            = 0x8B50, // GL_FLOAT_VEC2
    FloatVec3                            = 0x8B51, // GL_FLOAT_VEC3
    FloatVec4                            = 0x8B52, // GL_FLOAT_VEC4
    IntVec2                              = 0x8B53, // GL_INT_VEC2
    IntVec3                              = 0x8B54, // GL_INT_VEC3
    IntVec4                              = 0x8B55, // GL_INT_VEC4
    Bool                                 = 0x8B56, // GL_BOOL
    BoolVec2                             = 0x8B57, // GL_BOOL_VEC2
    BoolVec3                             = 0x8B58, // GL_BOOL_VEC3
    BoolVec4                             = 0x8B59, // GL_BOOL_VEC4
    FloatMat2                            = 0x8B5A, // GL_FLOAT_MAT2
    FloatMat3                            = 0x8B5B, // GL_FLOAT_MAT3
    FloatMat4                            = 0x8B5C, // GL_FLOAT_MAT4
    Sampler1D                            = 0x8B5D, // GL_SAMPLER_1D
    Sampler2D                            = 0x8B5E, // GL_SAMPLER_2D
    Sampler3D                            = 0x8B5F, // GL_SAMPLER_3D
    SamplerCube                          = 0x8B60, // GL_SAMPLER_CUBE
    Sampler1DShadow                      = 0x8B61, // GL_SAMPLER_1D_SHADOW
    Sampler2DShadow                      = 0x8B62, // GL_SAMPLER_2D_SHADOW
    Sampler2DRect                        = 0x8B63, // GL_SAMPLER_2D_RECT
    Sampler2DRectShadow                  = 0x8B64, // GL_SAMPLER_2D_RECT_SHADOW
    FloatMat2x3                          = 0x8B65, // GL_FLOAT_MAT2x3
    FloatMat2x4                          = 0x8B66, // GL_FLOAT_MAT2x4
    FloatMat3x2                          = 0x8B67, // GL_FLOAT_MAT3x2
    FloatMat3x4                          = 0x8B68, // GL_FLOAT_MAT3x4
    FloatMat4x2                          = 0x8B69, // GL_FLOAT_MAT4x2
    FloatMat4x3                          = 0x8B6A, // GL_FLOAT_MAT4x3
    Sampler1DArray                       = 0x8DC0, // GL_SAMPLER_1D_ARRAY
    Sampler2DArray                       = 0x8DC1, // GL_SAMPLER_2D_ARRAY
    SamplerBuffer                        = 0x8DC2, // GL_SAMPLER_BUFFER
    Sampler1DArrayShadow                 = 0x8DC3, // GL_SAMPLER_1D_ARRAY_SHADOW
    Sampler2DArrayShadow                 = 0x8DC4, // GL_SAMPLER_2D_ARRAY_SHADOW
    SamplerCubeShadow                    = 0x8DC5, // GL_SAMPLER_CUBE_SHADOW
    UnsignedIntVec2                      = 0x8DC6, // GL_UNSIGNED_INT_VEC2
    UnsignedIntVec3                      = 0x8DC7, // GL_UNSIGNED_INT_VEC3
    UnsignedIntVec4                      = 0x8DC8, // GL_UNSIGNED_INT_VEC4
    IntSampler1D                         = 0x8DC9, // GL_INT_SAMPLER_1D
    IntSampler2D                         = 0x8DCA, // GL_INT_SAMPLER_2D
    IntSampler3D                         = 0x8DCB, // GL_INT_SAMPLER_3D
    IntSamplerCube                       = 0x8DCC, // GL_INT_SAMPLER_CUBE
    IntSampler2DRect                     = 0x8DCD, // GL_INT_SAMPLER_2D_RECT
    IntSampler1DArray                    = 0x8DCE, // GL_INT_SAMPLER_1D_ARRAY
    IntSampler2DArray                    = 0x8DCF, // GL_INT_SAMPLER_2D_ARRAY
    IntSamplerBuffer                     = 0x8DD0, // GL_INT_SAMPLER_BUFFER
    UnsignedIntSampler1D                 = 0x8DD1, // GL_UNSIGNED_INT_SAMPLER_1D
    UnsignedIntSampler2D                 = 0x8DD2, // GL_UNSIGNED_INT_SAMPLER_2D
    UnsignedIntSampler3D                 = 0x8DD3, // GL_UNSIGNED_INT_SAMPLER_3D
    UnsignedIntSamplerCube               = 0x8DD4, // GL_UNSIGNED_INT_SAMPLER_CUBE
    UnsignedIntSampler2DRect             = 0x8DD5, // GL_UNSIGNED_INT_SAMPLER_2D_RECT
    UnsignedIntSampler1DArray            = 0x8DD6, // GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
    UnsignedIntSampler2DArray            = 0x8DD7, // GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
    UnsignedIntSamplerBuffer             = 0x8DD8, // GL_UNSIGNED_INT_SAMPLER_BUFFER
    DoubleMat2                           = 0x8F46, // GL_DOUBLE_MAT2
    DoubleMat3                           = 0x8F47, // GL_DOUBLE_MAT3
    DoubleMat4                           = 0x8F48, // GL_DOUBLE_MAT4
    DoubleMat2x3                         = 0x8F49, // GL_DOUBLE_MAT2x3
    DoubleMat2x4                         = 0x8F4A, // GL_DOUBLE_MAT2x4
    DoubleMat3x2                         = 0x8F4B, // GL_DOUBLE_MAT3x2
    DoubleMat3x4                         = 0x8F4C, // GL_DOUBLE_MAT3x4
    DoubleMat4x2                         = 0x8F4D, // GL_DOUBLE_MAT4x2
    DoubleMat4x3                         = 0x8F4E, // GL_DOUBLE_MAT4x3
    DoubleVec2                           = 0x8FFC, // GL_DOUBLE_VEC2
    DoubleVec3                           = 0x8FFD, // GL_DOUBLE_VEC3
    DoubleVec4                           = 0x8FFE, // GL_DOUBLE_VEC4
    SamplerCubeMapArray                  = 0x900C, // GL_SAMPLER_CUBE_MAP_ARRAY
    SamplerCubeMapArrayShadow            = 0x900D, // GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW
    IntSamplerCubeMapArray               = 0x900E, // GL_INT_SAMPLER_CUBE_MAP_ARRAY
    UnsignedIntSamplerCubeMapArray       = 0x900F, // GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY
    Sampler2DMultisample                 = 0x9108, // GL_SAMPLER_2D_MULTISAMPLE
    IntSampler2DMultisample              = 0x9109, // GL_INT_SAMPLER_2D_MULTISAMPLE
    UnsignedIntSampler2DMultisample      = 0x910A, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
    Sampler2DMultisampleArray            = 0x910B, // GL_SAMPLER_2D_MULTISAMPLE_ARRAY
    IntSampler2DMultisampleArray         = 0x910C, // GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    UnsignedIntSampler2DMultisampleArray = 0x910D, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
}; // enum class UniformType

struct UseProgramStageMask {
    static constexpr GLbitfield VertexShader         = 0x00000001; // GL_VERTEX_SHADER_BIT
    static constexpr GLbitfield FragmentShader       = 0x00000002; // GL_FRAGMENT_SHADER_BIT
    static constexpr GLbitfield GeometryShader       = 0x00000004; // GL_GEOMETRY_SHADER_BIT
    static constexpr GLbitfield TessControlShader    = 0x00000008; // GL_TESS_CONTROL_SHADER_BIT
    static constexpr GLbitfield TessEvaluationShader = 0x00000010; // GL_TESS_EVALUATION_SHADER_BIT
    static constexpr GLbitfield ComputeShader        = 0x00000020; // GL_COMPUTE_SHADER_BIT
    static constexpr GLbitfield MeshShader           = 0x00000040; // GL_MESH_SHADER_BIT_NV
    static constexpr GLbitfield TaskShader           = 0x00000080; // GL_TASK_SHADER_BIT_NV
    static constexpr GLbitfield AllShaderBits        = 0xFFFFFFFF; // GL_ALL_SHADER_BITS
}; // struct UseProgramStageMask

enum class VariantCapEXT : GLenum {
    VariantArray = 0x87E8, // GL_VARIANT_ARRAY_EXT
}; // enum class VariantCapEXT

enum class VertexArrayPName : GLenum {
    VertexAttribRelativeOffset  = 0x82D5, // GL_VERTEX_ATTRIB_RELATIVE_OFFSET
    VertexAttribArrayEnabled    = 0x8622, // GL_VERTEX_ATTRIB_ARRAY_ENABLED
    VertexAttribArraySize       = 0x8623, // GL_VERTEX_ATTRIB_ARRAY_SIZE
    VertexAttribArrayStride     = 0x8624, // GL_VERTEX_ATTRIB_ARRAY_STRIDE
    VertexAttribArrayType       = 0x8625, // GL_VERTEX_ATTRIB_ARRAY_TYPE
    VertexAttribArrayLong       = 0x874E, // GL_VERTEX_ATTRIB_ARRAY_LONG
    VertexAttribArrayNormalized = 0x886A, // GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    VertexAttribArrayInteger    = 0x88FD, // GL_VERTEX_ATTRIB_ARRAY_INTEGER
    VertexAttribArrayDivisor    = 0x88FE, // GL_VERTEX_ATTRIB_ARRAY_DIVISOR
}; // enum class VertexArrayPName

enum class VertexArrayPNameAPPLE : GLenum {
    StorageClient = 0x85B4, // GL_STORAGE_CLIENT_APPLE
    StorageCached = 0x85BE, // GL_STORAGE_CACHED_APPLE
    StorageShared = 0x85BF, // GL_STORAGE_SHARED_APPLE
}; // enum class VertexArrayPNameAPPLE

enum class VertexAttribEnum : GLenum {
    VertexAttribArrayEnabled       = 0x8622, // GL_VERTEX_ATTRIB_ARRAY_ENABLED
    VertexAttribArraySize          = 0x8623, // GL_VERTEX_ATTRIB_ARRAY_SIZE
    VertexAttribArrayStride        = 0x8624, // GL_VERTEX_ATTRIB_ARRAY_STRIDE
    VertexAttribArrayType          = 0x8625, // GL_VERTEX_ATTRIB_ARRAY_TYPE
    CurrentVertexAttrib            = 0x8626, // GL_CURRENT_VERTEX_ATTRIB
    VertexAttribArrayNormalized    = 0x886A, // GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    VertexAttribArrayBufferBinding = 0x889F, // GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    VertexAttribArrayInteger       = 0x88FD, // GL_VERTEX_ATTRIB_ARRAY_INTEGER
    VertexAttribArrayDivisor       = 0x88FE, // GL_VERTEX_ATTRIB_ARRAY_DIVISOR
}; // enum class VertexAttribEnum

enum class VertexAttribEnumNV : GLenum {
    ProgramParameter = 0x8644, // GL_PROGRAM_PARAMETER_NV
}; // enum class VertexAttribEnumNV

enum class VertexAttribIType : GLenum {
    Byte          = 0x1400, // GL_BYTE
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    Short         = 0x1402, // GL_SHORT
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    Int           = 0x1404, // GL_INT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
}; // enum class VertexAttribIType

enum class VertexAttribLType : GLenum {
    Double = 0x140A, // GL_DOUBLE
}; // enum class VertexAttribLType

enum class VertexAttribPointerPropertyARB : GLenum {
    VertexAttribArrayPointer = 0x8645, // GL_VERTEX_ATTRIB_ARRAY_POINTER
}; // enum class VertexAttribPointerPropertyARB

enum class VertexAttribPointerType : GLenum {
    Byte                      = 0x1400, // GL_BYTE
    UnsignedByte              = 0x1401, // GL_UNSIGNED_BYTE
    Short                     = 0x1402, // GL_SHORT
    UnsignedShort             = 0x1403, // GL_UNSIGNED_SHORT
    Int                       = 0x1404, // GL_INT
    UnsignedInt               = 0x1405, // GL_UNSIGNED_INT
    Float                     = 0x1406, // GL_FLOAT
    Double                    = 0x140A, // GL_DOUBLE
    HalfFloat                 = 0x140B, // GL_HALF_FLOAT
    Fixed                     = 0x140C, // GL_FIXED
    Int64                     = 0x140E, // GL_INT64_ARB
    UnsignedInt64             = 0x140F, // GL_UNSIGNED_INT64_ARB
    UnsignedInt2_10_10_10Rev  = 0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV
    UnsignedInt10F_11F_11FRev = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
    Int2_10_10_10Rev          = 0x8D9F, // GL_INT_2_10_10_10_REV
}; // enum class VertexAttribPointerType

enum class VertexAttribPropertyARB : GLenum {
    VertexAttribBinding            = 0x82D4, // GL_VERTEX_ATTRIB_BINDING
    VertexAttribRelativeOffset     = 0x82D5, // GL_VERTEX_ATTRIB_RELATIVE_OFFSET
    VertexAttribArrayEnabled       = 0x8622, // GL_VERTEX_ATTRIB_ARRAY_ENABLED
    VertexAttribArraySize          = 0x8623, // GL_VERTEX_ATTRIB_ARRAY_SIZE
    VertexAttribArrayStride        = 0x8624, // GL_VERTEX_ATTRIB_ARRAY_STRIDE
    VertexAttribArrayType          = 0x8625, // GL_VERTEX_ATTRIB_ARRAY_TYPE
    CurrentVertexAttrib            = 0x8626, // GL_CURRENT_VERTEX_ATTRIB
    VertexAttribArrayLong          = 0x874E, // GL_VERTEX_ATTRIB_ARRAY_LONG
    VertexAttribArrayNormalized    = 0x886A, // GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    VertexAttribArrayBufferBinding = 0x889F, // GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    VertexAttribArrayInteger       = 0x88FD, // GL_VERTEX_ATTRIB_ARRAY_INTEGER
    VertexAttribArrayDivisor       = 0x88FE, // GL_VERTEX_ATTRIB_ARRAY_DIVISOR
}; // enum class VertexAttribPropertyARB

enum class VertexAttribType : GLenum {
    Byte                     = 0x1400, // GL_BYTE
    UnsignedByte             = 0x1401, // GL_UNSIGNED_BYTE
    Short                    = 0x1402, // GL_SHORT
    UnsignedShort            = 0x1403, // GL_UNSIGNED_SHORT
    Int                      = 0x1404, // GL_INT
    UnsignedInt              = 0x1405, // GL_UNSIGNED_INT
    Float                    = 0x1406, // GL_FLOAT
    Double                   = 0x140A, // GL_DOUBLE
    HalfFloat                = 0x140B, // GL_HALF_FLOAT
    Fixed                    = 0x140C, // GL_FIXED
    UnsignedInt2_10_10_10Rev = 0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV
    UnsignedInt10F11F11FRev  = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
    Int2_10_10_10Rev         = 0x8D9F, // GL_INT_2_10_10_10_REV
}; // enum class VertexAttribType

enum class VertexBufferObjectParameter : GLenum {
    BufferImmutableStorage = 0x821F, // GL_BUFFER_IMMUTABLE_STORAGE
    BufferStorageFlags     = 0x8220, // GL_BUFFER_STORAGE_FLAGS
    BufferSize             = 0x8764, // GL_BUFFER_SIZE
    BufferUsage            = 0x8765, // GL_BUFFER_USAGE
    BufferAccess           = 0x88BB, // GL_BUFFER_ACCESS
    BufferMapped           = 0x88BC, // GL_BUFFER_MAPPED
    BufferAccessFlags      = 0x911F, // GL_BUFFER_ACCESS_FLAGS
    BufferMapLength        = 0x9120, // GL_BUFFER_MAP_LENGTH
    BufferMapOffset        = 0x9121, // GL_BUFFER_MAP_OFFSET
}; // enum class VertexBufferObjectParameter

enum class VertexBufferObjectUsage : GLenum {
    StreamDraw  = 0x88E0, // GL_STREAM_DRAW
    StreamRead  = 0x88E1, // GL_STREAM_READ
    StreamCopy  = 0x88E2, // GL_STREAM_COPY
    StaticDraw  = 0x88E4, // GL_STATIC_DRAW
    StaticRead  = 0x88E5, // GL_STATIC_READ
    StaticCopy  = 0x88E6, // GL_STATIC_COPY
    DynamicDraw = 0x88E8, // GL_DYNAMIC_DRAW
    DynamicRead = 0x88E9, // GL_DYNAMIC_READ
    DynamicCopy = 0x88EA, // GL_DYNAMIC_COPY
}; // enum class VertexBufferObjectUsage

struct VertexHintsMaskPGI {
    static constexpr GLenum Vertex23             = 0x00000004; // GL_VERTEX23_BIT_PGI
    static constexpr GLenum Vertex4              = 0x00000008; // GL_VERTEX4_BIT_PGI
    static constexpr GLenum Color3               = 0x00010000; // GL_COLOR3_BIT_PGI
    static constexpr GLenum Color4               = 0x00020000; // GL_COLOR4_BIT_PGI
    static constexpr GLenum Edgeflag             = 0x00040000; // GL_EDGEFLAG_BIT_PGI
    static constexpr GLenum Index                = 0x00080000; // GL_INDEX_BIT_PGI
    static constexpr GLenum MatAmbient           = 0x00100000; // GL_MAT_AMBIENT_BIT_PGI
    static constexpr GLenum MatAmbientAndDiffuse = 0x00200000; // GL_MAT_AMBIENT_AND_DIFFUSE_BIT_PGI
    static constexpr GLenum MatDiffuse           = 0x00400000; // GL_MAT_DIFFUSE_BIT_PGI
    static constexpr GLenum MatEmission          = 0x00800000; // GL_MAT_EMISSION_BIT_PGI
    static constexpr GLenum MatColorIndexes      = 0x01000000; // GL_MAT_COLOR_INDEXES_BIT_PGI
    static constexpr GLenum MatShininess         = 0x02000000; // GL_MAT_SHININESS_BIT_PGI
    static constexpr GLenum MatSpecular          = 0x04000000; // GL_MAT_SPECULAR_BIT_PGI
    static constexpr GLenum Normal               = 0x08000000; // GL_NORMAL_BIT_PGI
    static constexpr GLenum Texcoord1            = 0x10000000; // GL_TEXCOORD1_BIT_PGI
    static constexpr GLenum Texcoord2            = 0x20000000; // GL_TEXCOORD2_BIT_PGI
    static constexpr GLenum Texcoord3            = 0x40000000; // GL_TEXCOORD3_BIT_PGI
    static constexpr GLenum Texcoord4            = 0x80000000; // GL_TEXCOORD4_BIT_PGI
}; // struct VertexHintsMaskPGI

enum class VertexPointerType : GLenum {
    Short  = 0x1402, // GL_SHORT
    Int    = 0x1404, // GL_INT
    Float  = 0x1406, // GL_FLOAT
    Double = 0x140A, // GL_DOUBLE
}; // enum class VertexPointerType

enum class VertexProvokingMode : GLenum {
    FirstVertexConvention = 0x8E4D, // GL_FIRST_VERTEX_CONVENTION
    LastVertexConvention  = 0x8E4E, // GL_LAST_VERTEX_CONVENTION
}; // enum class VertexProvokingMode

enum class VertexShaderCoordOutEXT : GLenum {
    X           = 0x87D5, // GL_X_EXT
    Y           = 0x87D6, // GL_Y_EXT
    Z           = 0x87D7, // GL_Z_EXT
    W           = 0x87D8, // GL_W_EXT
    NegativeX   = 0x87D9, // GL_NEGATIVE_X_EXT
    NegativeY   = 0x87DA, // GL_NEGATIVE_Y_EXT
    NegativeZ   = 0x87DB, // GL_NEGATIVE_Z_EXT
    NegativeW   = 0x87DC, // GL_NEGATIVE_W_EXT
    Zero        = 0x87DD, // GL_ZERO_EXT
    One         = 0x87DE, // GL_ONE_EXT
    NegativeOne = 0x87DF, // GL_NEGATIVE_ONE_EXT
}; // enum class VertexShaderCoordOutEXT

enum class VertexShaderOpEXT : GLenum {
    OpIndex          = 0x8782, // GL_OP_INDEX_EXT
    OpNegate         = 0x8783, // GL_OP_NEGATE_EXT
    OpDot3           = 0x8784, // GL_OP_DOT3_EXT
    OpDot4           = 0x8785, // GL_OP_DOT4_EXT
    OpMul            = 0x8786, // GL_OP_MUL_EXT
    OpAdd            = 0x8787, // GL_OP_ADD_EXT
    OpMadd           = 0x8788, // GL_OP_MADD_EXT
    OpFrac           = 0x8789, // GL_OP_FRAC_EXT
    OpMax            = 0x878A, // GL_OP_MAX_EXT
    OpMin            = 0x878B, // GL_OP_MIN_EXT
    OpSetGe          = 0x878C, // GL_OP_SET_GE_EXT
    OpSetLt          = 0x878D, // GL_OP_SET_LT_EXT
    OpClamp          = 0x878E, // GL_OP_CLAMP_EXT
    OpFloor          = 0x878F, // GL_OP_FLOOR_EXT
    OpRound          = 0x8790, // GL_OP_ROUND_EXT
    OpExpBase2       = 0x8791, // GL_OP_EXP_BASE_2_EXT
    OpLogBase2       = 0x8792, // GL_OP_LOG_BASE_2_EXT
    OpPower          = 0x8793, // GL_OP_POWER_EXT
    OpRecip          = 0x8794, // GL_OP_RECIP_EXT
    OpRecipSqrt      = 0x8795, // GL_OP_RECIP_SQRT_EXT
    OpSub            = 0x8796, // GL_OP_SUB_EXT
    OpCrossProduct   = 0x8797, // GL_OP_CROSS_PRODUCT_EXT
    OpMultiplyMatrix = 0x8798, // GL_OP_MULTIPLY_MATRIX_EXT
    OpMov            = 0x8799, // GL_OP_MOV_EXT
}; // enum class VertexShaderOpEXT

enum class VertexShaderParameterEXT : GLenum {
    CurrentVertex = 0x87E2, // GL_CURRENT_VERTEX_EXT
    MvpMatrix     = 0x87E3, // GL_MVP_MATRIX_EXT
}; // enum class VertexShaderParameterEXT

enum class VertexShaderStorageTypeEXT : GLenum {
    Variant       = 0x87C1, // GL_VARIANT_EXT
    Invariant     = 0x87C2, // GL_INVARIANT_EXT
    LocalConstant = 0x87C3, // GL_LOCAL_CONSTANT_EXT
    Local         = 0x87C4, // GL_LOCAL_EXT
}; // enum class VertexShaderStorageTypeEXT

enum class VertexShaderWriteMaskEXT : GLenum {
    False = 0, // GL_FALSE
    True  = 1, // GL_TRUE
}; // enum class VertexShaderWriteMaskEXT

enum class VertexStreamATI : GLenum {
    VertexStream0 = 0x876C, // GL_VERTEX_STREAM0_ATI
    VertexStream1 = 0x876D, // GL_VERTEX_STREAM1_ATI
    VertexStream2 = 0x876E, // GL_VERTEX_STREAM2_ATI
    VertexStream3 = 0x876F, // GL_VERTEX_STREAM3_ATI
    VertexStream4 = 0x8770, // GL_VERTEX_STREAM4_ATI
    VertexStream5 = 0x8771, // GL_VERTEX_STREAM5_ATI
    VertexStream6 = 0x8772, // GL_VERTEX_STREAM6_ATI
    VertexStream7 = 0x8773, // GL_VERTEX_STREAM7_ATI
}; // enum class VertexStreamATI

enum class VertexWeightPointerTypeEXT : GLenum {
    Float = 0x1406, // GL_FLOAT
}; // enum class VertexWeightPointerTypeEXT

enum class WeightPointerTypeARB : GLenum {
    Byte          = 0x1400, // GL_BYTE
    UnsignedByte  = 0x1401, // GL_UNSIGNED_BYTE
    Short         = 0x1402, // GL_SHORT
    UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
    Int           = 0x1404, // GL_INT
    UnsignedInt   = 0x1405, // GL_UNSIGNED_INT
    Float         = 0x1406, // GL_FLOAT
    Double        = 0x140A, // GL_DOUBLE
}; // enum class WeightPointerTypeARB


////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Converts a scalar type into an OpenGL compatible boolean.
//! @tparam T The data type of the input value.
//! @param[in] scalar The scalar to convert.
//! @return The value as an OpenGL compatible boolean.
template<typename T>
Boolean toBoolean(T scalar)
{
    static constexpr T TFalse = static_cast<T>(0);

    return (scalar == TFalse) ? Boolean::False : Boolean::True;
}

//! @brief Converts one strongly typed OpenGL enumeration into another.
//! @tparam TOutput The data type of the output enumeration.
//! @tparam TInput The data type of the input enumeration (inferred).
//! @param value The value to convert.
//! @return The converted result, probably with the same numeric value.
//! @note This operates on the principle that all enumeration values in OpenGL,
//! no matter the type, have unique values, but the same underlying integer
//! data type.
template<typename TOutput, typename TInput>
TOutput convertEnum(TInput value)
{
    using RawInput = std::underlying_type_t<TInput>;
    using RawOutput = std::underlying_type_t<TOutput>;

    return static_cast<TOutput>(static_cast<RawOutput>(static_cast<RawInput>(value)));
}

} // namespace gl

#endif // ifndef __EFCB0C06_7A7A_46A0_B04E_C3925634D0FD_INCLUDED__

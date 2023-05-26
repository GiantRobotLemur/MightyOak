//! @file ObjectGL/DisplayContextPrivate.hpp
//! @brief The declaration of the inner implementation of the Display Context
//! object.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_DISPLAY_CONTEXT_PRIVATE_HPP__
#define __AG_OBJECT_GL_DISPLAY_CONTEXT_PRIVATE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <mutex>
#include <vector>

#include "Ag/Core/Version.hpp"
#include "Ag/GLAPI/BaseTypes.hpp"
#include "Ag/GLAPI/CommandSets.hpp"
#include "Ag/ObjectGL/DisplayContext.hpp"

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
namespace gl {
struct APIResolver;
struct TextureName;
struct QueryName;
struct BufferName;
struct ProgramName;
struct ShaderName;
struct FramebufferName;
struct RenderbufferName;
struct VertexArrayName;
struct SamplerName;
struct TransformFeedbackName;
struct ProgramPipelineName;

} // namespace gl

namespace Ag {
namespace GL {

enum class ResourceType
{
    Texture,
    Query,
    Buffer,
    Program,
    Shader,
    FrameBuffer,
    RenderBuffer,
    VertexArray,
    Sampler,
    TransformFeedback,
    ProgramPipeline,
};

using TaggedResource = std::pair<ResourceType, GLuint>;

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class IDriver;
class ContextOptions;
class RenderContextPrivate;

//! @brief The inner implementation of the Display Context object.
class DisplayContextPrivate : public std::enable_shared_from_this<DisplayContextPrivate>
{
public:
    // Construction/Destruction
    DisplayContextPrivate();
    virtual ~DisplayContextPrivate() = default;

    // Accessors
    const gl::GLAPI &getAPI() const;
    const Version getMaxSupportedVersion() const;
    virtual const gl::APIResolver *getResolver() const = 0;

    // Operations
    void dispose(const gl::TextureName &resource);
    void dispose(const gl::QueryName &resource);
    void dispose(const gl::BufferName &resource);
    void dispose(const gl::ProgramName &resource);
    void dispose(const gl::ShaderName &resource);
    void dispose(const gl::FrameBufferName &resource);
    void dispose(const gl::RenderBufferName &resource);
    void dispose(const gl::VertexArrayName &resource);
    void dispose(const gl::SamplerName &resource);
    void dispose(const gl::TransformFeedbackName &resource);
    void dispose(const gl::ProgramPipelineName &resource);
    void flushResources();

    // Overrides
    //! @brief Creates an implementation of RenderContextPrivate compatible
    //! with the current display.
    //! @param[in] drawable A reference to an object, probably something visual
    //! required to create the OpenGL context against.
    //! @param[in] options Specifies attributes of the context to create.
    //! @note On failure, an Ag::Exception of some type will be thrown.
    virtual std::shared_ptr<RenderContextPrivate> createContext(uintptr_t drawable,
                                                                const ContextOptions &option) = 0;

protected:
    void initialiseAPI();

private:
    // Internal Types
    using TaggedResourceCollection = std::vector<TaggedResource>;
    using TaggedResourceIter = TaggedResourceCollection::iterator;

    // Internal Functions
    void dispose(ResourceType type, GLuint id);

    // Internal Fields
    std::mutex _disposalLock;
    TaggedResourceCollection _resourcesForDisposal;
    gl::GLAPI _api;
    Version _maxVersion;
};

typedef std::shared_ptr<DisplayContextPrivate> DisplayContextPrivateSPtr;

class AssignableDisplayContext : public DisplayContext
{
public:
    AssignableDisplayContext(const DisplayContextPrivateSPtr &display);
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

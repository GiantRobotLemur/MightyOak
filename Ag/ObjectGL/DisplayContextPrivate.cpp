//! @file ObjectGL/DisplayContextPrivate.cpp
//! @brief The definition of the inner implementation of the Display Context
//! object.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Ag/Core/Exception.hpp"
#include "Ag/Core/CollectionTools.hpp"
#include "Ag/GLAPI/Identifiers.hpp"
#include "DisplayContextPrivate.hpp"
#include "RenderContextPrivate.hpp"

namespace Ag {
namespace GL {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct ResourceComparer
{
    bool operator()(const TaggedResource &lhs, const TaggedResource &rhs) const
    {
        bool isLess = false;

        if (lhs.first == rhs.first)
        {
            isLess = (lhs.second < rhs.second);
        }
        else
        {
            isLess = lhs.first < rhs.first;
        }

        return isLess;
    }
};

struct GroupByType
{
    bool operator()(const TaggedResource &lhs, const TaggedResource &rhs) const
    {
        return lhs.first < rhs.first;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Extracts the identifiers of a run of tagged resources.
//! @tparam T The data type of the iterators used to define the run.
//! @param[in] begin The iterator pointing to the first tagged resource to
//! extract an identifier from.
//! @param[in] end The iterator pointing to just after the last tagged resource
//! to extract an identifier from.
//! @param[out] ids A collection to append the identifiers to.
template<typename T, typename U>
void extractResourceIDs(const Ag::IteratorRange<T> &range,
                        std::vector<U> &ids)
{
    if (range.isEmpty())
    {
        return;
    }

    // Minimise the amount of memory reallocation in the vector.
    size_t count = range.getCount();
    size_t totalSize = ids.size() + count;

    if (totalSize > ids.capacity())
    {
        ids.reserve(totalSize);
    }

    // Extract unique identifiers.
    unsigned int previousId = range.Begin->second;
    ids.emplace_back(previousId);

    for (const TaggedResource &resource : range)
    {
        // Filter out any duplicate IDs, which should be consecutive.
        if (resource.second != previousId)
        {
            previousId = resource.second;
            ids.emplace_back(previousId);
        }
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// DisplayContextPrivate Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which can manage OpenGL contexts and the
//! resources they share.
DisplayContextPrivate::DisplayContextPrivate()
{
    _resourcesForDisposal.reserve(32);
}

//! @brief Gets the API resolved through a root context.
const gl::GLAPI &DisplayContextPrivate::getAPI() const
{
    return _api;
}

//! @brief Gets the maximum supported version of the OpenGL API supported by
//! the current display.
const Version DisplayContextPrivate::getMaxSupportedVersion() const
{
    return _maxVersion;
}

//! @brief Disposes of a texture resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::TextureName &resource)
{
    dispose(ResourceType::Texture, resource.ID);
}

//! @brief Disposes of a query resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::QueryName &resource)
{
    dispose(ResourceType::Query, resource.ID);
}

//! @brief Disposes of a buffer resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::BufferName &resource)
{
    dispose(ResourceType::Buffer, resource.ID);
}

//! @brief Disposes of a program resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::ProgramName &resource)
{
    dispose(ResourceType::Program, resource.ID);
}

//! @brief Disposes of a shader resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::ShaderName &resource)
{
    dispose(ResourceType::Shader, resource.ID);
}

//! @brief Disposes of a frame buffer resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::FrameBufferName &resource)
{
    dispose(ResourceType::FrameBuffer, resource.ID);
}

//! @brief Disposes of a render buffer resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::RenderBufferName &resource)
{
    dispose(ResourceType::RenderBuffer, resource.ID);
}

//! @brief Disposes of a VAO resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::VertexArrayName &resource)
{
    dispose(ResourceType::VertexArray, resource.ID);
}

//! @brief Disposes of a sampler resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::SamplerName &resource)
{
    dispose(ResourceType::Sampler, resource.ID);
}

//! @brief Disposes of a transform feedback resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::TransformFeedbackName &resource)
{
    dispose(ResourceType::TransformFeedback, resource.ID);
}

//! @brief Disposes of a program pipeline resource created on the current display.
//! @param[in] resource The identifier of the resource to schedule for disposal.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(const gl::ProgramPipelineName &resource)
{
    dispose(ResourceType::ProgramPipeline, resource.ID);
}

//! @brief Disposes of all resources scheduled for disposal at a point when a
//! context associated with the current display is selected.
void DisplayContextPrivate::flushResources()
{
    if (_resourcesForDisposal.empty())
    {
        return;
    }

    std::lock_guard guard(_disposalLock);

    // Sort the resources by type.
    std::sort(_resourcesForDisposal.begin(), _resourcesForDisposal.end(),
              ResourceComparer());

    // Group the resources by type for disposal.
    GroupByType groupPredicate;
    IteratorRange<TaggedResourceIter> group;

    if (getFirstGroup(_resourcesForDisposal.begin(),
                      _resourcesForDisposal.end(),
                      group, groupPredicate))
    {
        do
        {
            switch (group.Begin->first)
            {
            case ResourceType::Texture: {
                std::vector<gl::TextureName> names;

                extractResourceIDs(group, names);
                _api.deleteTextures(static_cast<GLsizei>(names.size()),
                                    names.data());
            } break;

            case ResourceType::Query: {
                std::vector<gl::QueryName> names;

                extractResourceIDs(group, names);
                _api.deleteQueries(static_cast<GLsizei>(names.size()), names.data());
            } break;

            case ResourceType::Buffer: {
                std::vector<gl::BufferName> names;

                extractResourceIDs(group, names);
                _api.deleteBuffers(static_cast<GLsizei>(names.size()), names.data());
            } break;

            case ResourceType::Program:
                // The API for managing program resources doesn't work on groups :-(
                for (const auto &taggedProgram : group)
                {
                    _api.deleteProgram(taggedProgram.second);
                }
                break;

            case ResourceType::Shader:
                // The API for managing shader resources doesn't work on groups :-(
                for (const auto &shader : group)
                {
                    _api.deleteShader(shader.second);
                }
                break;

            case ResourceType::FrameBuffer: {
                std::vector<gl::FrameBufferName> names;

                extractResourceIDs(group, names);
                _api.deleteFramebuffers(static_cast<GLsizei>(names.size()), names.data());
            } break;

            case ResourceType::RenderBuffer: {
                std::vector<gl::RenderBufferName> names;

                extractResourceIDs(group, names);
                _api.deleteRenderbuffers(static_cast<GLsizei>(names.size()), names.data());
            } break;

            case ResourceType::VertexArray: {
                std::vector<gl::VertexArrayName> names;

                extractResourceIDs(group, names);
                _api.deleteVertexArrays(static_cast<GLsizei>(names.size()), names.data());
            } break;

            case ResourceType::Sampler: {
                std::vector<gl::SamplerName> names;

                extractResourceIDs(group, names);
                _api.deleteSamplers(static_cast<GLsizei>(names.size()), names.data());
            } break;

            case ResourceType::TransformFeedback: {
                std::vector<gl::TransformFeedbackName> names;

                extractResourceIDs(group, names);
                _api.deleteTransformFeedbacks(static_cast<GLsizei>(names.size()),
                                              names.data());
            } break;

            case ResourceType::ProgramPipeline: {
                std::vector<gl::ProgramPipelineName> names;

                extractResourceIDs(group, names);
                _api.deleteProgramPipelines(static_cast<GLsizei>(names.size()),
                                            names.data());
            } break;

            } // switch (ResourceType)
        } while (getNextGroup(_resourcesForDisposal.end(), group, groupPredicate));
    }
}

//! @brief Must be called by derived classes when the root context has been
//! created and selected into the current thread.
//! @note At the time of call, getResolver() must return a valid result.
void DisplayContextPrivate::initialiseAPI()
{
    _api.resolve(getResolver());

    _maxVersion.tryParse(_api.getString(gl::StringName::Version));
}

//! @brief Schedules a resource for disposal.
//! @param[in] type The type of resource to dispose of.
//! @param[in] id The identifier of the resource.
//! @note Disposal maybe deferred until an appropriate render context is
//! selected on a running thread.
void DisplayContextPrivate::dispose(ResourceType type, GLuint id)
{
    std::lock_guard guard(_disposalLock);

    _resourcesForDisposal.emplace_back(type, id);
}

////////////////////////////////////////////////////////////////////////////////
// AssignableDisplayContext Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief A public constructor which allows a DisplayContext to be created
//! which is in a bound state.
//! @param[in] display The inner display implementation to bind to.
AssignableDisplayContext::AssignableDisplayContext(const DisplayContextPrivateSPtr &display) :
    DisplayContext(display)
{
}

}} // namespace Ag::GL
////////////////////////////////////////////////////////////////////////////////


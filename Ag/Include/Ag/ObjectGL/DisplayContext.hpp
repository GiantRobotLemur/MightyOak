//! @file ObjectGL/DisplayContext.hpp
//! @brief The declaration of an object representing a context in which OpenGL
//! resources exist in order to be used by render contexts.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_DISPLAY_CONTEXT_HPP__
#define __AG_OBJECT_GL_DISPLAY_CONTEXT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <memory>

#include "Ag/Core/Version.hpp"
#include "RenderContext.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class DisplayContextPrivate;
class ContextOptions;

using DisplyContextPrivateSPtr = std::shared_ptr<DisplayContextPrivate>;

//! @brief An object representing a context in which OpenGL resources exist in
//! order to be used by render contexts.
class DisplayContext
{
protected:
    // Construction/Destruction
    DisplayContext(const std::shared_ptr<DisplayContextPrivate> &context);
public:
    DisplayContext() = default;
    ~DisplayContext() = default;

    // Accessors
    bool isBound() const;
    Version getMaxSupportedVersion() const;

    // Operations
    RenderContext createRenderer(uintptr_t drawable,
                                 const ContextOptions &options);
    void flushResources();
private:
    // Internal Fields
    DisplyContextPrivateSPtr _context;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file Ag/SdlTools/WrapperTypes.hpp
//! @brief The declaration of C++ wrapper types for C-based SDL structures.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SDL_TOOLS_WRAPPER_TYPES_HPP__
#define __AG_SDL_TOOLS_WRAPPER_TYPES_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <type_traits>

#include <SDL2/SDL.h>

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#define DECLARE_WRAPPER(name,sdlType) struct name ## Deleter { \
    void operator()(sdlType *obj) const; }; \
    using name ## UPtr = std::unique_ptr<sdlType, name ## Deleter>
#define DELCARE_OPAQUE_WRAPPER(name,sdlType) struct name ## Deleter { \
    void operator()(sdlType obj) const; }; \
    using name ## UPtr = std::unique_ptr<std::remove_pointer<sdlType>::type, name ## Deleter>

namespace Ag {
namespace SDL {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
DECLARE_WRAPPER(Window, SDL_Window);
DECLARE_WRAPPER(Renderer, SDL_Renderer);
DELCARE_OPAQUE_WRAPPER(GLContext, SDL_GLContext);


}} // namespace Ag::SDL

#undef DECLARE_WRAPPER
#undef DELCARE_OPAQUE_WRAPPER

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

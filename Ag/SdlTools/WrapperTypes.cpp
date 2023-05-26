//! @file SdlTools/WrapperTypes.cpp
//! @brief The definition of C++ wrapper types for C-based SDL structures.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <SDL2/SDL.h>

#include "Ag/SdlTools/WrapperTypes.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_WRAPPER(name,sdlType,destroyFnName) \
    void name ## Deleter::operator()(sdlType *obj) const \
    { if (obj != nullptr) destroyFnName(obj); }

#define IMPLEMENT_OPAQUE_WRAPPER(name,sdlType,destroyFnName) \
    void name ## Deleter::operator()(sdlType obj) const \
    { if (obj != nullptr) destroyFnName(obj); }

namespace Ag {
namespace SDL {

////////////////////////////////////////////////////////////////////////////////
// Data Type Definitions
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_WRAPPER(Window, SDL_Window, SDL_DestroyWindow);
IMPLEMENT_WRAPPER(Renderer, SDL_Renderer, SDL_DestroyRenderer);
IMPLEMENT_OPAQUE_WRAPPER(GLContext, SDL_GLContext, SDL_GL_DeleteContext);

}} // namespace Ag::SDL

#undef IMPLEMENT_WRAPPER
#undef IMPLEMENT_OPAQUE_WRAPPER

////////////////////////////////////////////////////////////////////////////////


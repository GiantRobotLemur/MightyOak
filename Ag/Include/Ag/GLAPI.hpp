//! @file Ag/GLAPI.hpp
//! @brief Imports all public header files for the GLAPI library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_GLAPI_HPP__
#define __AG_GLAPI_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "GLAPI/Platform.hpp"

// Header files generated from the Khronos OpenGL API registry.
#include "GLAPI/BaseTypes.hpp"
#include "GLAPI/EnumTypes.hpp"
#include "GLAPI/APIs.hpp"
#include "GLAPI/Identifiers.hpp"
#include "GLAPI/CommandSets.hpp"

#ifdef _WIN32
#include "GLAPI/WGL_API.hpp"
#include "GLAPI/WGL_CommandSets.hpp"
#endif // ifdef _WIN32

// Further headers which support the generated API.
#include "GLAPI/ExtensionSet.hpp"
#include "GLAPI/APIResolvers.hpp"
#include "GLAPI/OpenGLException.hpp"


#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

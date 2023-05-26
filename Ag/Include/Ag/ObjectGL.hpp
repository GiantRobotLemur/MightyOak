//! @file Ag/ObjectGL.hpp
//! @brief Imports all public header files for the ObjectGL library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_HPP__
#define __AG_OBJECT_GL_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
// Must be included before GLAPI headers.
#include "ObjectGL/Win32API.hpp"
#endif

#include "GLAPI.hpp"

#include "ObjectGL/VertexSchema.hpp"
#include "ObjectGL/Buffer.hpp"
#include "ObjectGL/IndexBuffer.hpp"
#include "ObjectGL/VertexBuffer.hpp"
#include "ObjectGL/VertexArrayObject.hpp"
#include "ObjectGL/ContextOptions.hpp"
#include "ObjectGL/DisplayContext.hpp"
#include "ObjectGL/DisplayFormat.hpp"
#include "ObjectGL/Driver.hpp"
#include "ObjectGL/Program.hpp"
#include "ObjectGL/ProgramBuilder.hpp"
#include "ObjectGL/RenderContext.hpp"
#include "ObjectGL/SDL2Driver.hpp"
#include "ObjectGL/Shader.hpp"

#ifdef _WIN32
#include "ObjectGL/WGL_Tools.hpp"
#include "ObjectGL/WGLDriver.hpp"
#endif // ifdef _WIN32

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

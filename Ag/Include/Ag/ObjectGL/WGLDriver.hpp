//! @file ObjectGL/WGLDriver.hpp
//! @brief The declaration of an object which uses direct calls to the WGL API
//! to create an OpenGL context.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_WGL_DRIVER_HPP__
#define __AG_OBJECT_GL_WGL_DRIVER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
// Must be included before GLAPI headers.
#include "Win32API.hpp"

#include "Driver.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which uses direct calls to the WGL API to create an
//! OpenGL context.
class WGLDriver : public IDriver
{
public:
    // Construction/Destruction
    virtual ~WGLDriver() = default;

    // Operations
    virtual DisplayContext createDisplayDevice(const DisplayFormat &format) const override;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

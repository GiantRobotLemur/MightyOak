//! @file ObjectGL/SDL2Driver.hpp
//! @brief The declaration of an implementation of an OpenGL driver which uses
//! libSDL2.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_SDL2_DRIVER_HPP__
#define __AG_OBJECT_GL_SDL2_DRIVER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/ObjectGL/Driver.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An implementation of an OpenGL driver which uses libSDL2.
class SDL2Driver : public IDriver
{
public:
    // Construction/Destruction
    SDL2Driver() = default;
    virtual ~SDL2Driver() = default;

    // Overrides
    virtual Version getMaxSupportedVersion(const ContextOptions &options,
                                           const DisplayFormat &format) const override;
    virtual DisplayContext createDisplayDevice(const DisplayFormat &format) const override;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

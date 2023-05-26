//! @file ObjectGL/Driver.hpp
//! @brief The declaration of the interface to an object which abstracts the
//! differences between different platforms underlying OpenGL implementations.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_DRIVER_HPP__
#define __AG_OBJECT_GL_DRIVER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/GLAPI/BaseTypes.hpp"
#include "Ag/ObjectGL/DisplayContext.hpp"

namespace Ag {
namespace GL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class DisplayFormat;

//! @brief The interface to a stateless factory object which abstracts the
//! differences between different platforms underlying OpenGL implementations
//! to allow the creation of OpenGL contexts.
class IDriver
{
public:
    // Construction/Destruction
    virtual ~IDriver() = default;

    // Operations
    virtual Version getMaxSupportedVersion(const ContextOptions &options,
                                           const DisplayFormat &format) const =0;
    virtual DisplayContext createDisplayDevice(const DisplayFormat &format) const = 0;
};

}} // namespace Ag::GL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file Ag/GLAPI/OpenGLException.hpp
//! @brief The declaration of an exception thrown when an OpenGL error condition
//! is detected.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_OBJECT_GL_OPEN_GL_EXCEPTION_HPP__
#define __AG_OBJECT_GL_OPEN_GL_EXCEPTION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"

namespace gl {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief an exception thrown when an OpenGL error condition is detected.
class OpenGLException : public Ag::Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction/Destruction
    OpenGLException(const char *fnName, unsigned int *errorCodes, size_t count);
};

} // namespace gl

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

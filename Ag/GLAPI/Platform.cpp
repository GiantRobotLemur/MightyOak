//! @file GLAPI/Platform.cpp
//! @brief The definition of functions useful to a specific platform or for
//! convenient merging into the generated OpenGL API implementation code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <string>

#include "Ag/Core/Exception.hpp"
#include "Ag/GLAPI/OpenGLException.hpp"
#include "Ag/GLAPI/Platform.hpp"

namespace gl {

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Throws an exception when an attempt is made to call the wrapper for
//! a function for which no entry point has been resolved.
//! @param[in] fnName The name of the function with no entry point.
//! @param[in] apiName The optional name of the API the function belonged to
//! such as OpenGL, WGL, GLX, etc.
//! @throws Ag::NotSupportedException Always after crafting an appropriate
//! message.
void reportMissingFunction(const char *fnName, const char *apiName /* = nullptr */)
{
    std::string feature;
    feature.assign("The '");
    feature.append(fnName);
    feature.append("()' function");

    if (apiName != nullptr)
    {
        feature.append(" of the ");
        feature.append(apiName);
        feature.append(" API");
    }

    throw Ag::NotSupportedException(feature.c_str());
}

//! @brief Gathers all error codes currently set and reports them by throwing
//! an OpenGLException.
//! @param[in] fnName The name of the function which produced the errors.
//! @param[in] firstErrorCode The first error code reported.
//! @param[in] errorReporterFn An optional pointer to a function which will report
//! further error codes.
//! @param[in] apiName The optional name of the API the function belongs to,
//! such as "OpenGL", "WGL" or "GLX".
void reportOpenGLError(const char *fnName,
                       unsigned int firstErrorCode,
                       unsigned int (APIENTRY *errorReporterFn)() /* = nullptr */,
                       const char *apiName /* = nullptr */)
{
    const size_t MaxErrors = 64;
    unsigned int errorCodes[MaxErrors];
    size_t errorCount = 0;

    if (errorReporterFn)
    {
        unsigned int prevErrorCode = 0;
        unsigned int errorCode = firstErrorCode;

        while ((errorCode != 0) && (errorCount < MaxErrors))
        {
            if (errorCode == prevErrorCode)
            {
                // When error codes repeat, something is wrong, so bail.
                errorCode = 0;
            }
            else
            {
                errorCodes[errorCount++] = errorCode;
                prevErrorCode = errorCode;

                // Get the next error.
                if (errorReporterFn == nullptr)
                {
                    errorCode = 0;
                }
                else
                {
                    errorCode = errorReporterFn();
                }
            }
        }
    }

    if (errorCount > 0)
    {
        if (apiName == nullptr)
        {
            throw OpenGLException(fnName, errorCodes, errorCount);
        }
        else
        {
            std::string qualifiedFnName;
            qualifiedFnName.append(apiName);
            qualifiedFnName.append(2, ':');
            qualifiedFnName.append(fnName);

            throw OpenGLException(qualifiedFnName.c_str(),
                                  errorCodes, errorCount);
        }
    }
}

} // namespace gl
////////////////////////////////////////////////////////////////////////////////


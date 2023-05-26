//! @file GLAPI/OpenGLException.cpp
//! @brief The definition of an exception thrown when an OpenGL error condition
//! occurs.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/LinearSortedMap.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/GLAPI/OpenGLException.hpp"
#include "Ag/GLAPI/EnumTypes.hpp"

namespace gl {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
struct ErrorInfo
{
    const char *Name;
    const char *Description;

    ErrorInfo(const char *name, const char *description) :
        Name(name),
        Description(description)
    {
    }
};

using ErrorInfoCPtr = const ErrorInfo *;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
bool tryGetErrorInfo(unsigned int errorCode, ErrorInfoCPtr &info)
{
    static Ag::LinearSortedMap<unsigned int, ErrorInfo> g_errorsByCode;

    if (g_errorsByCode.isEmpty())
    {
        // Define the error code mapping, once.
        Ag::LinearSortedMapIndexer indexer(g_errorsByCode);

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::NoError), {
                                  "No error",
                                  "No error has been recorded." });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::InvalidEnum), {
                                  "Invalid enumeration",
                                  "An unacceptable value is specified for an enumerated "
                                  "argument." });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::InvalidValue), {
                                  "Value out of range",
                                  "A numeric argument is out of range." });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::InvalidOperation), {
                                  "Operation not allowed",
                                  "The specified OpenGL operation is not allowed in the "
                                  "current state." });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::InvalidFramebufferOperation), {
                                  "Frame buffer object incomplete",
                                  "The frame buffer object is not complete." });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::OutOfMemory), {
                                  "Out of memory",
                                  "There is not enough memory left to execute the "
                                  "OpenGL command." });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::StackUnderflow), {
                                  "Stack underflow",
                                  "An attempt has been made to perform an operation that "
                                  "would cause an internal stack to underflow." });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::StackOverflow), {
                                  "Stack overflow",
                                  "An attempt has been made to perform an operation that "
                                  "would cause an internal stack to overflow." });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::TableTooLarge), {
                                  "Histogram table too large",
                                  "The requested width of the histogram table was larger "
                                  "than the maximum allowed histogram width" });

        g_errorsByCode.push_back(static_cast<unsigned int>(gl::ErrorCode::TextureTooLarge), {
                                  "Texture too large",
                                  "The specified texture is too large" });
    }

    // Look up the error.
    bool isFound = false;
    auto pos = g_errorsByCode.findIndexed(errorCode);

    if (pos == g_errorsByCode.end())
    {
        info = nullptr;
    }
    else
    {
        info = &pos->second;
        isFound = true;
    }

    return isFound;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// OpenGLException Member Definitions
////////////////////////////////////////////////////////////////////////////////
const char *OpenGLException::Domain = "OpenGLException";

//! @brief Constructs an exception detailing error conditions detected after an
//! OpenGL function has returned.
//! @param[in] fnName The name of the OpenGL function which produced the error.
//! @param[in] errorCodes The set of codes describing the error.
//! @param[in] count The count of elements in the errorCodes array.
OpenGLException::OpenGLException(const char *fnName, unsigned int *errorCodes,
                                 size_t count)
{
    std::string detail;

    detail.assign("The OpenGL function '");
    detail.append(fnName);
    detail.append("()' produced ");

    if (count == 0)
    {
        detail.append("no detailed error information.");
    }
    else if (count == 1)
    {
        detail.append("an error:");
    }
    else
    {
        detail.append("multiple errors:");
    }

    for (size_t i = 0; i < count; ++i)
    {
        ErrorInfoCPtr info = nullptr;

        if (tryGetErrorInfo(errorCodes[i], info))
        {
            detail.push_back('\n');
            detail.append(info->Name);
            detail.push_back(':');
            detail.push_back(' ');
            detail.append(info->Description);
        }
        else
        {
            const size_t BufferSize = 32;
            char buffer[BufferSize];
            snprintf(buffer, BufferSize, "%.4X", errorCodes[i]);

            detail.push_back('\n');
            detail.append("Unknown error code (0x");
            detail.append(buffer);
            detail.append(").");
        }
    }

    uintptr_t firstError = (count > 0) ? errorCodes[0] : 0;

    initialise(Domain,
               "The program detected an error condition from an OpenGL function.",
               detail, firstError);
}

} // namespace gl
////////////////////////////////////////////////////////////////////////////////


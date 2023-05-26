//! @file SdlTools/ApiException.cpp
//! @brief The definition of an exception documenting the unexpected failure
//! of an SDL function.
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

#include "Ag/Core/Format.hpp"
#include "Ag/SdlTools/ApiException.hpp"

namespace Ag {
namespace SDL {

////////////////////////////////////////////////////////////////////////////////
// ApiException Member Definitions
////////////////////////////////////////////////////////////////////////////////
const char *ApiException::Domain = "SDLException";

//! @brief Constructs an exception documenting an error which occurred during an
//! SDL API function.
//! @param[in] fnName The SDL function which failed including parenthesis and an
//! optional summary of the arguments.
//! @note The error detail will be captured using a call to SDL_GetError().
ApiException::ApiException(const char *fnName)
{
    // Capture the error message from the current thread state.
    std::string detail(SDL_GetError());
    std::string message;

    message.assign("The SDL function '");
    message.append(fnName);
    message.append("' returned an unexpected error condition.");

    initialise(Domain, message, detail, 0);
}

//! @brief Constructs an exception documenting an error which occurred during an
//! SDL API function.
//! @param[in] fnNameFormat The format specification used to create the name
//! of the SDL function which failed including parenthesis and an
//! optional summary of the arguments.
//! @param[in] args The arguments to be formatted into the function name string.
//! @note The error detail will be captured using a call to SDL_GetError().
ApiException::ApiException(const char *fnNameFormat, const Ag::VariantList &args)
{
    // Capture the error message from the current thread state.
    std::string detail(SDL_GetError());
    std::string message;

    message.assign("The SDL function '");
    appendFormat(fnNameFormat, message, args);
    message.append("' returned an unexpected error condition.");

    initialise(Domain, message, detail, 0);
}

}} // namespace Ag::SDL
////////////////////////////////////////////////////////////////////////////////


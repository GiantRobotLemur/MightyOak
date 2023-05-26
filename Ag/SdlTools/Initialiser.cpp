//! @file SdlTools/Initialiser.cpp
//! @brief The definition of an object which uses RAII to initialise and
//! properly shut down SDL sub-systems.
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

#include "Ag/SdlTools/ApiException.hpp"
#include "Ag/SdlTools/Initialiser.hpp"

namespace Ag {
namespace SDL {

////////////////////////////////////////////////////////////////////////////////
// Initialiser Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to initialise sub-systems of the SDL library.
//! @param[in] subSysFlags A bit field defining the sub-systems to initialise
//! such as SDL_INIT_VIDEO, SDL_INIT_EVENTS, etc.
Initialiser::Initialiser(uint32_t subSysFlags /*= SDL_INIT_VIDEO | SDL_INIT_EVENTS*/) :
    _subSystemFlags(subSysFlags),
    _isInitialised(false)
{
}

//! @brief Ensures that any initialised SDL sub-systems are properly shut down.
Initialiser::~Initialiser()
{
    shutdown();
}

//! @brief Determines if at least some SDL sub-systems are already initialised.
bool Initialiser::isInitialised() const
{
    return _isInitialised;
}

//! @brief Requests that additional sub-systems be initialised.
//! @param[in] subSystemFlags Flags defining the sub-systems to initialise
//! such as SDL_INIT_JOYSTICK or SDL_INIT_AUDIO.
//! @note Initialisation of the new sub-systems will be deferred until the next
//! call to initialise() if there hasn't already been one.
//! @throws ApiException If SDL was already initialised and the new sub-system
//! failed to initialise.
void Initialiser::addSubSystems(uint32_t subSystemFlags)
{
    if (_isInitialised)
    {
        // The new sub-systems need to be started up now.
        uint32_t newFlags = subSystemFlags & ~_subSystemFlags;

        if (newFlags != 0)
        {
            // There are new sub-systems to initialise.
            if (SDL_InitSubSystem(subSystemFlags) == 0)
            {
                _subSystemFlags |= subSystemFlags;
            }
            else
            {
                throw ApiException("SDL_InitSubSystem()");
            }
        }
    }
    else
    {
        _subSystemFlags |= subSystemFlags;
    }
}

//! @brief Initialises the pre-selected set of sub-systems.
//! @throw ApiException If initialisation fails.
void Initialiser::initialise()
{
    if (_isInitialised == false)
    {
        if (SDL_Init(_subSystemFlags) == 0)
        {
            _isInitialised = true;
        }
        else
        {
            throw ApiException("SDL_Init()");
        }
    }
}

//! @brief Explicitly shuts down all remaining SDL sub-systems.
void Initialiser::shutdown()
{
    if (_isInitialised)
    {
        // Prevent repeated shut-downs.
        _isInitialised = false;

        SDL_Quit();
    }
}

}} // namespace Ag::SDL
////////////////////////////////////////////////////////////////////////////////


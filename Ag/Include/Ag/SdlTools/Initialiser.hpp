//! @file Ag/SdlTools/Initialiser.hpp
//! @brief The declaration of an object which uses RAII to initialise and
//! properly shut down SDL sub-systems.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SDL_TOOLS_INITIALISER_HPP__
#define __AG_SDL_TOOLS_INITIALISER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <SDL2/SDL.h>

namespace Ag {
namespace SDL {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which uses RAII to initialise and properly shut down SDL
//! sub-systems.
class Initialiser
{
public:
    // Construction/Destruction
    Initialiser(uint32_t subSysFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    ~Initialiser();

    // Accessors
    bool isInitialised() const;

    // Operations
    void addSubSystems(uint32_t subSystemFlags);
    void initialise();
    void shutdown();
private:
    // Internal Fields
    uint32_t _subSystemFlags;
    bool _isInitialised;
};

}} // namespace Ag::SDL

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

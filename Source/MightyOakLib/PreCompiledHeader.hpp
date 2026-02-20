//! @file MightyOakLib/PreCompiledHeader.hpp
//! @brief Includes all system or library header files required internally by
//! the library so that they can be pre-compiled once.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MIGHTY_OAK_LIB_PCH_HPP__
#define __MIGHTY_OAK_LIB_PCH_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
// C/CRT Header Files

// C++/STL Header Files
#include <memory>

// 3rd Party Library Files
#include <SDL3/SDL.h>

// Upstream Library Files
#include <Ag/Core.hpp>
#include <Ag/Sdl3Tools.hpp>

// Application Library Files
#include "ArmEmu.hpp"

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

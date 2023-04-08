//! @file Core/Platform.hpp
//! @brief The includes platform specific headers hidden behind conditional
//! compilation macros.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_PLATFORM_HPP__
#define __AG_CORE_PLATFORM_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include "Win32API.hpp"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <execinfo.h>

#include "PosixAPI.hpp"
#endif

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
// Needed to implement the Win32 file system schema, even under Linux.
#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file MightyOakLib/EmulatorSession.cpp
//! @brief The definition of an object which holds the runtime state of the
//! system being emulated.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"

#include "MightyOakLib/EmulatorSession.hpp"
#include "ArmEmu/ArmSystemBuilder.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// EmulatorSession Member Definitions
////////////////////////////////////////////////////////////////////////////////
EmulatorSession::EmulatorSession(const Arm::Options &configuration) :
    _configuration(configuration)
{
}

//! @brief Returns the configuration of the system being emulated.
const Arm::Options &EmulatorSession::getConfiguration() const
{
    return _configuration;
}

//! @brief Gets the emulated ARM system, possibly nullptr if not yet created.
Arm::IArmSystem *EmulatorSession::getSystem()
{
    return _system.get();
}

//! @brief Gets the emulated ARM system, possibly nullptr if not yet created.
const Arm::IArmSystem *EmulatorSession::getSystem() const
{
    return _system.get();
}

//! @brief Creates the emulated ARM system based on the stored configuration.
//! @param[out] error Receives a description of any error that occurred.
//! @retval true The system was created successfully.
//! @retval false The system could not be created, error describes why.
bool EmulatorSession::createSystem(Ag::String &error)
{
    try
    {
        Arm::ArmSystemBuilder builder(_configuration);
        _system = builder.createSystem();
        return true;
    }
    catch (const Ag::Exception &ex)
    {
        error = ex.getMessage();
    }
    catch (const std::exception &ex)
    {
        error = ex.what();
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


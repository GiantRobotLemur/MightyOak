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

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An implementation of IHostConnection which connects to an emulator
//! session.
class SessionConnection : public Arm::IHostConnection
{
public:
    // Construction/Destruction
    SessionConnection(uint32_t guestEventMessageId);
    virtual ~SessionConnection() = default;

    // Overrides
    virtual void onGuestEvent(Arm::IArmSystem *instance, uint32_t id,
                              uintptr_t param1, uintptr_t param2) override;
private:
    // Internal Fields
    uint32_t _guestEventMessageId = 0;
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// EmulatorSession Member Definitions
////////////////////////////////////////////////////////////////////////////////
EmulatorSession::EmulatorSession(const Arm::Options &configuration,
                                 uint32_t guestEventId) :
    _configuration(configuration),
    _hostConnection(std::make_shared<SessionConnection>(guestEventId))
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
        builder.setHostConnection(_hostConnection);
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
// SessionConnection Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an IHostConnection implementation which passes guest
//! events on to the SDL input thread.
//! @param[in] guestEventMessageId The identifier allocated by SDL for encoding
//! guest events as SDL_Event values.
SessionConnection::SessionConnection(uint32_t guestEventMessageId) :
    _guestEventMessageId(guestEventMessageId)
{
}

// Inherited from IHostConnection.
void SessionConnection::onGuestEvent(Arm::IArmSystem */*instance*/, uint32_t id,
                                     uintptr_t param1, uintptr_t param2)
{
    if ((id >= Arm::HostMessageID::VSyncOccurred) &&
        (id < Arm::HostMessageID::LastHostMessage))
    {
        // Post the event to the input thread.
        SDL_Event guestEvent;
        Ag::zeroFill(guestEvent);

        guestEvent.type = _guestEventMessageId;
        guestEvent.user.code = static_cast<decltype(guestEvent.user.code)>(id);
        guestEvent.user.data1 = reinterpret_cast<void *>(param1);
        guestEvent.user.data2 = reinterpret_cast<void *>(param2);

        SDL_PushEvent(&guestEvent);
    }
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


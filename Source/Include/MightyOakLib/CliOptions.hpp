//! @file MightyOakLib/CliOptions.hpp
//! @brief The declaration of an object representing the options parsed from
//! command line arguments.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MIGHTY_OAK_LIB_CLI_OPTIONS_HPP__
#define __MIGHTY_OAK_LIB_CLI_OPTIONS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <type_traits>

#include "Ag/Sdl3Tools.hpp"
#include "ArmEmu/EmuOptions.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the commands which can be expressed to the application via
//! command line arguments.
enum class AppCommand : std::underlying_type_t<Ag::SDL3::SdlCommand>
{
    //! @brief Orders the application to start an emulator session.
    RunSession = Ag::SDL3::SdlCommand::LastSdlCommand,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing the options parsed from command line arguments.
class CliOptions : public Ag::SDL3::ProgramArguments
{
public:
    // Construction/Destruction
    CliOptions(const Ag::AppMetadata &appMetadata);
    virtual ~CliOptions() = default;

    // Accessors
    const Arm::Options &getEmulatedSystemConfig() const;

    // Overrides
protected:
    virtual bool processOption(uint32_t id, const Ag::String &original,
                               const Ag::String &value,
                               Ag::String &error) override;
    virtual bool validate(Ag::String &error) const override;
    virtual void postProcess() override;
private:
    // Internal Fields
    Arm::Options _emulatorOptions;
};

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

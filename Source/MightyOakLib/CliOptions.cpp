//! @file MightyOakLib/CliOptions.cpp
//! @brief The definition of an object representing the options parsed from
//! command line arguments.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "MightyOakLib/CliOptions.hpp"

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines identifiers for standard SDL command line options.
enum EmulatorOptions : uint32_t
{
    BaseSystem = Ag::SDL3::SdlOptions::SdlLastOption,
    Model,
    Processor,
    ProcessorSpeed,
    RamSize,
    VRamSize,
    Display,
    HDInterface,
    HDCount,
    FDCount,
    JoystickType,
    JoystickCount,
    RomSelection,
    CustomRomPath,
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Appends a list of valid command line option values based on metadata
//! associated with an enumeration type.
//! @tparam TEnumInfo The Ag::EnumInfo<>-derived data type used to describe
//! the enumeration type.
//! @param[in,out] description The description string to append to.
//! @param[in] metadata The enum type metadata used to enumerate valid values.
//template<typename TEnumInfo>
void defineValidValues(std::string &description, const Ag::IEnumInfo &metadata)
{
    description.append(" Valid values are: ");

    for (size_t i = 0, count = metadata.getSymbolCount(); i < count; ++i)
    {
        if (i > 0)
        {
            description.push_back(',');
            description.push_back(' ');
        }

        description.append(metadata.toStringFromIndex(i));
    }
}

//! @brief Attempts to parse a value associated with a command line option.
//! @tparam TEnumInfo The Ag::EnumInfo<>-derived data type used to describe
//! the enumeration type.
//! @tparam TEnum The enumeration type to parse a value for.
//! @param[in] option Text representing the command line option proceeding the value.
//! @param[in] valueText The text of the value to parse.
//! @param[out] error Receives a message detailing why parsing failed.
//! @param[in] metadata Provides metadata for the numeration type to parse a
//! value from.
//! @param[out] value The returned enumeration value.
//! @retval true The @p valueText was valid and returned in @p value.
//! @retval false The @p valueText was invalid.
template<typename TEnum, typename TEnumInfoSymbol>
bool tryParseOptionValue(Ag::utf8_cptr_t option, Ag::string_cref_t valueText,
                         Ag::string_ref_t error,
                         const Ag::EnumInfo<TEnum, TEnumInfoSymbol> &metadata,
                         TEnum &value)
{
    if (metadata.tryParse(valueText.toUtf8View(), value))
        return true;

    std::string message;

    message.assign("The value '");
    Ag::appendAgString(message, valueText);
    message.append("' is not valid of the ");
    message.append(option);
    message.append(" command line option.");

    error = message;

    return false;
}

//! @brief Attempts to parse a RAM size value from text.
//! @param[in] option The command line option preceding the value.
//! @param[in] valueText The text of the value to parse.
//! @param[out] error Receives a message if parsing failed.
//! @param[out] ramSize Receives the size in bytes if parsing succeeded.
//! @retval true Parsing was successful, @p ramSize holds the value.
//! @retval false Parsing failed, @p error describes why.
bool tryParseOptionRamSize(Ag::utf8_cptr_t option, Ag::string_cref_t valueText,
                           Ag::string_ref_t error, uint32_t &ramSize)
{
    Ag::ScalarParser parser(Ag::LocaleInfo::getNeutral());

    parser.enableExponent(false);
    parser.enableRadixPrefix(false);
    parser.enableSign(false);

    const auto &view = valueText.toUtf8View();
    size_t i;

    for (i = 0; i < view.length(); ++i)
    {
        if (parser.tryProcessCharacter(view.at(i)) == false)
        {
            break;
        }
    }

    double realValue;

    if (parser.tryGetValue(realValue))
    {
        double multiplier = 1 << 20;

        // Skip trailing white space.
        while (i < view.length() && std::isspace(view.at(i)))
            ++i;

        if (i < view.length())
        {
            auto suffix = std::toupper(view.at(i));

            if (suffix == 'K')
            {
                multiplier = 1 << 10;
            }
            else if (suffix == 'M')
            {
                multiplier = 1 << 20;
            }
            else
            {
                std::string suffixMessage;
                suffixMessage.assign("Unknown suffix '");
                suffixMessage.append(view.substr(i));
                suffixMessage.append("' on memory size following the '");
                suffixMessage.append(option);
                suffixMessage.append("' option.");

                error = suffixMessage;

                return false;
            }
        }

        ramSize = static_cast<uint32_t>(std::rint(realValue * multiplier));

        return true;
    }

    std::string message;
    message.assign("The value '");
    message.append(view);
    message.append("' following the '");
    message.append(option);
    message.append("' option is not a valid memory size.");

    error = message;

    return false;
}

//! @brief Attempts to parse a scalar value from text.
//! @param[in] option The command line option preceding the value.
//! @param[in] valueText The text of the value to parse.
//! @param[out] error Receives a message if parsing failed.
//! @param[in] multiplier The multiplier to scale the parsed value by before
//! returning it.
//! @param[out] scalar Receives the size in bytes if parsing succeeded.
//! @retval true Parsing was successful, @p ramSize holds the value.
//! @retval false Parsing failed, @p error describes why.
bool tryParseOptionScalar(Ag::utf8_cptr_t option, Ag::string_cref_t valueText,
                          Ag::string_ref_t error, uint32_t &scalar,
                          uint32_t multiplier = 1u)
{
    Ag::ScalarParser parser(Ag::LocaleInfo::getNeutral());
    parser.enableExponent(false);
    parser.enableSign(false);
    parser.enableRadixPrefix(false);
    parser.enableFraction(multiplier > 1);

    double realValue;

    if (parser.tryProcessString(valueText.toUtf8View()))
    {
        if (parser.allowFraction())
        {
            // We were expecting a real value.
            if (parser.tryGetValue(realValue))
            {
                const double factor = static_cast<double>(multiplier);

                scalar = static_cast<uint32_t>(std::rint(realValue * factor));

                return true;
            }
        }
        else if (parser.tryGetValue(scalar))
        {
            // We were expecting an integer.
            return true;
        }
    }

    std::string message;

    message.assign("The value '");
    Ag::appendAgString(message, valueText);
    message.append("' following the '");
    message.append(option);
    message.append("' option is not a valid value.");
    error = message;

    return false;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// CliOptions Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a description of valid command line options for the
//! emulator application.
CliOptions::CliOptions(const Ag::AppMetadata &appMetadata)
{
    using namespace Ag::Cli;

    SchemaBuilder optionSchema;
    optionSchema.setAppName(appMetadata.AppName);
    optionSchema.setDescription(appMetadata.Description);

    optionSchema.addShowHelpCommand();
    optionSchema.addShowVersionCommand(appMetadata.AppVersion);

    std::string descriptionBuilder;

    // Define the --model option. Expected values: A305, A310, A410, A420, etc.
    descriptionBuilder.assign("The basic model name.");
    defineValidValues(descriptionBuilder, Mo::Arm::getProductionModelType());

    optionSchema.defineOption(EmulatorOptions::BaseSystem,
                              descriptionBuilder.c_str(),
                              OptionValue::Mandatory,
                              "model name");
    optionSchema.defineAlias(EmulatorOptions::BaseSystem, "base", true);

    // Define the --model option. Valid values: TestBed, Archimedes, ASeries, RiscPC.
    descriptionBuilder.assign("The basic system type.");
    defineValidValues(descriptionBuilder, Mo::Arm::getSystemModelType());

    optionSchema.defineOption(EmulatorOptions::Model, descriptionBuilder.c_str(),
                              OptionValue::Mandatory, "model name");
    optionSchema.defineAlias(EmulatorOptions::Model, "system", true);

    // Define the --Processor option. ARM2, ARM250, ARM3, ARM3_FPA, etc.
    descriptionBuilder.assign("The CPU variant.");
    defineValidValues(descriptionBuilder, Mo::Arm::getProcessorModelType());

    optionSchema.defineOption(EmulatorOptions::Processor, descriptionBuilder.c_str(),
                              OptionValue::Mandatory, "variant");
    optionSchema.defineAlias(EmulatorOptions::Processor, "cpu", true);

    // Define the --ProcessorSpeed option. Expects a floating point value.
    optionSchema.defineOption(EmulatorOptions::ProcessorSpeed,
                              "The processor speed in MHz.",
                              OptionValue::Mandatory, "freq");
    optionSchema.defineAlias(EmulatorOptions::ProcessorSpeed, "speed", true);

    // Define the --RamSize option. Expects a real value, possibly suffixed
    // with K, KB, M or MB.
    optionSchema.defineOption(EmulatorOptions::RamSize,
                              "The amount of system RAM, e.g. 2Mb, 0.5M or 512K.",
                              OptionValue::Mandatory, "size");
    optionSchema.defineAlias(EmulatorOptions::RamSize, "ram", true);

    // Define the --VRamSize option. Expects a real value, possibly suffixed
    // with K, KB, M or MB.
    optionSchema.defineOption(EmulatorOptions::VRamSize,
                              "The amount of video RAM. Valid values 1Mb, 2Mb or 4Mb.",
                              OptionValue::Mandatory, "size");
    optionSchema.defineAlias(EmulatorOptions::VRamSize, "vram", true);

    // Define the --Display option. Expects one of NormalTV, HighResMono, MultiScan, etc.
    descriptionBuilder.assign("The connected display to emulate.");
    defineValidValues(descriptionBuilder, Mo::Arm::getDisplayInterfaceType());

    optionSchema.defineOption(EmulatorOptions::Display, descriptionBuilder.c_str(),
                              OptionValue::Mandatory, "type");
    optionSchema.defineAlias(EmulatorOptions::Display, "display", true);

    // Define the --HDInterface option. Expects one of ST506, IDE, or SCSI.
    descriptionBuilder.assign("The hard drive interface to emulate.");
    defineValidValues(descriptionBuilder, Mo::Arm::getHDInterfaceType());

    optionSchema.defineOption(EmulatorOptions::HDInterface, descriptionBuilder.c_str(),
                              OptionValue::Mandatory, "type");
    optionSchema.defineAlias(EmulatorOptions::HDInterface, "hd_type", true);

    // Define the --HDCount option. Expects an integer value.
    optionSchema.defineOption(EmulatorOptions::HDCount,
                              "The count of hard drives connected.",
                              OptionValue::Mandatory, "count");
    optionSchema.defineAlias(EmulatorOptions::HDCount, "hd_count", true);

    // Define the --FDCount option. Expects an integer value.
    optionSchema.defineOption(EmulatorOptions::FDCount,
                              "The count of floppy drives connected.",
                              OptionValue::Mandatory, "count");
    optionSchema.defineAlias(EmulatorOptions::FDCount, "fd_count", true);

    // Define the --JoystickType option. Valid values: None, Digital, Analogue.
    descriptionBuilder.assign("The joystick interface to emulate.");
    defineValidValues(descriptionBuilder, Mo::Arm::getJoystickInterfaceType());

    optionSchema.defineOption(EmulatorOptions::JoystickType, descriptionBuilder.c_str(),
                              OptionValue::Mandatory, "type");
    optionSchema.defineAlias(EmulatorOptions::JoystickType, "joystick_type", true);

    // Define the --JoystickCount option. Expects an integer value.
    optionSchema.defineOption(EmulatorOptions::JoystickCount,
                              "The count of joysticks connected.",
                              OptionValue::Mandatory, "count");
    optionSchema.defineAlias(EmulatorOptions::JoystickCount, "joy_count", true);

    // Define the --RomSelection option. Valid values: Custom, Arthur_0_30, RiscOS_3_11, etc.
    descriptionBuilder.assign("The preset system ROM to install.");
    defineValidValues(descriptionBuilder, Mo::Arm::getSystemROMPresetType());

    optionSchema.defineOption(EmulatorOptions::RomSelection, descriptionBuilder.c_str(),
                              OptionValue::Mandatory, "preset");
    optionSchema.defineAlias(EmulatorOptions::RomSelection, "rom", true);

    // Define the --CustomRomPath option. A path to a ROM image.
    descriptionBuilder.assign("The path to a custom ROM image to install.");
    defineValidValues(descriptionBuilder, Mo::Arm::getSystemROMPresetType());

    optionSchema.defineOption(EmulatorOptions::CustomRomPath,
                              "The path to a custom ROM image to install.",
                              OptionValue::Mandatory, "path");
    optionSchema.defineAlias(EmulatorOptions::CustomRomPath, "custom_rom", true);

    // Define SDL support commands.
    defineSDLSchema(optionSchema);

    setSchema(optionSchema);

    // Define default system to emulate.
    _emulatorOptions = Arm::Options::makeProductionModel(Arm::ProductionModel::A3020);
    _emulatorOptions.setRamSizeKb(4096);
}

//! @brief Gets a reference to the configuration of the desired emulated system.
const Arm::Options &CliOptions::getEmulatedSystemConfig() const
{
    return _emulatorOptions;
}

// Inherited from Ag::Cli::ProgramArguments.
bool CliOptions::processOption(uint32_t id, const Ag::String &original,
                               const Ag::String &value,
                               Ag::String &error)
{
    uint32_t scalar;
    Arm::ProductionModel productionModel;
    Arm::SystemModel model;
    Arm::ProcessorModel cpuVariant;
    Arm::DisplayInterface displayType;
    Arm::HardDiskInterface hdType;
    Arm::JoystickInterface joystickType;
    Arm::SystemROMPreset romPreset;
    bool isOK = false;

    switch (id)
    {
    case EmulatorOptions::BaseSystem:
        if (tryParseOptionValue("--base", value, error,
                                Arm::getProductionModelType(), productionModel))
        {
            // Overwrite any session options with a production model stereotype.
            _emulatorOptions = Arm::Options::makeProductionModel(productionModel);
            isOK = true;
        }
        break;

    case EmulatorOptions::Model:

        if (tryParseOptionValue("--model", value, error, Arm::getSystemModelType(), model))
        {
            _emulatorOptions.setHardwareArchitecture(model);
            isOK = true;
        }
        break;

    case EmulatorOptions::Processor:
        if (tryParseOptionValue("--cpu", value, error,
                                Arm::getProcessorModelType(), cpuVariant))
        {
            _emulatorOptions.setProcessorVariant(cpuVariant);
            isOK = true;
        }
        break;

    case EmulatorOptions::ProcessorSpeed:
        if (tryParseOptionScalar("--speed", value, error, scalar, 1000000))
        {
            // TODO: Allow processor speed to be set in Hz.
            _emulatorOptions.setProcessorSpeedMHz(static_cast<uint16_t>(scalar / 1000000));
            isOK = true;
        }
        break;

    case EmulatorOptions::RamSize:
        if (tryParseOptionRamSize("--ram", value, error, scalar))
        {
            _emulatorOptions.setRamSizeKb(scalar >> 10);
            isOK = true;
        }
        break;

    case EmulatorOptions::VRamSize:
        if (tryParseOptionRamSize("--vram", value, error, scalar))
        {
            _emulatorOptions.setVideoRamSizeKb(scalar >> 10);
            isOK = true;
        }
        break;

    case EmulatorOptions::Display:
        if (tryParseOptionValue("--display", value, error,
                                Arm::getDisplayInterfaceType(), displayType))
        {
            _emulatorOptions.setDisplayType(displayType);
            isOK = true;
        }
        break;

    case EmulatorOptions::HDInterface:
        if (tryParseOptionValue("--hd_type", value, error,
                                Arm::getHDInterfaceType(), hdType))
        {
            _emulatorOptions.setHardDiskTechnology(hdType);
            isOK = true;
        }
        break;

    case EmulatorOptions::HDCount:
        if (tryParseOptionScalar("--hd_count", value, error, scalar))
        {
            _emulatorOptions.setHardDriveCount(static_cast<uint8_t>(scalar));
            isOK = true;
        }
        break;

    case EmulatorOptions::FDCount:
        if (tryParseOptionScalar("--fd_count", value, error, scalar))
        {
            _emulatorOptions.setFloppyDiskCount(static_cast<uint8_t>(scalar));
            isOK = true;
        }
        break;

    case EmulatorOptions::JoystickType:
        if (tryParseOptionValue("--joystick_type", value, error,
                                Arm::getJoystickInterfaceType(),
                                joystickType))
        {
            _emulatorOptions.setJoystickType(joystickType);
            isOK = true;
        }
        break;

    case EmulatorOptions::JoystickCount:
        if (tryParseOptionScalar("--joy_count", value, error, scalar))
        {
            _emulatorOptions.setJoystickCount(static_cast<uint8_t>(scalar));
            isOK = true;
        }
        break;

    case EmulatorOptions::RomSelection:
        if (tryParseOptionValue("--rom", value, error,
                                Arm::getSystemROMPresetType(),
                                romPreset))
        {
            _emulatorOptions.setSystemRom(romPreset);
            isOK = true;
        }
        break;

    case EmulatorOptions::CustomRomPath: {
        // --custom_rom
        Ag::Fs::PathBuilder romPath;

        if (romPath.tryParse(value, error))
        {
            // Clean up the path.
            romPath.convertToAbsolute();
            romPath.makeCanonical();

            _emulatorOptions.setCustomRom(romPath);
            isOK = true;
        }
        else
        {
            std::string message;

            message.assign("The value following the '--custom_rom' option '");
            Ag::appendAgString(message, value);
            message.append("' is not a valid file path. ");
            Ag::appendAgString(message, error);

            error = message;
        }
    } break;

    default:
        // Allow the base class to try to process the option.
        isOK = Ag::SDL3::ProgramArguments::processOption(id, original, value, error);
        break;
    } // switch(id)

    return isOK;
}

// Inherited from Ag::Cli::ProgramArguments.
bool CliOptions::validate(Ag::String &error) const
{
    if (getCommand() == Ag::toScalar(AppCommand::RunSession))
    {
        // Validate the emulator configuration.
        return _emulatorOptions.validate(error);
    }

    return true;
}

// Inherited from Ag::Cli::ProgramArguments.
void CliOptions::postProcess()
{
    // Set the default application command to run an emulation session.
    if (getCommand() == Ag::Cli::StandardCommands::NoCommand)
        setCommand(Ag::toScalar(AppCommand::RunSession));
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


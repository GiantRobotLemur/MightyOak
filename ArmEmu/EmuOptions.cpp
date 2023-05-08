//! @file ArmEmu/EmuOptions.cpp
//! @brief The definition of an object defining the configuration of a system
//! to emulate.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/Variant.hpp"

#include "ArmEmu/EmuOptions.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Options Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates a default set of options representing as basic Acorn A3010
//! machine with 2MB of RAM.
Options::Options() :
    _model(SystemModel::ASeries),
    _processor(ProcessorModel::ARM2),
    _processorSpeedMHz(12),
    _ramSizeKb(2048),
    _videoRamSizeKb(0),
    _display(DisplayInterface::MultiScan),
    _hardDiskInterface(HardDiskInterface::IDE),
    _hardDriveCount(0),
    _floppyDriveCount(1),
    _joystickType(JoystickInterface::Digital),
    _joystickCount(2),
    _systemRom(SystemROMPreset::RiscOS_3_11)
{
}

//! @brief Gets the identifier of the underlying hardware architecture.
SystemModel Options::getHardwareArchitecture() const
{
    return _model;
}

//! @brief Sets the identifier of the underlying hardware architecture.
//! @param[in] model The new architecture.
void Options::setHardwareArchitecture(SystemModel model)
{
    _model = model;
}

//! @brief Gets the model and variant of the processor to emulate.
ProcessorModel Options::getProcessorVariant() const
{
    return _processor;
}

//! @brief Sets the model and variant of the processor to emulate.
//! @param[in] processor The new processor model.
void Options::setProcessorVariant(ProcessorModel processor)
{
    _processor = processor;
}

//! @brief Gets the speed of the processor to emulate in MHz.
uint16_t Options::getProcessorSpeedMHz() const
{
    return _processorSpeedMHz;
}

//! @brief Sets the speed of the processor to emulate.
//! @param[in] clockFreqMHz The emulated clock frequency in MHz.
void Options::setProcessorSpeedMHz(uint16_t clockFreqMHz)
{
    _processorSpeedMHz = clockFreqMHz;
}

//! @brief Gets the size of the dynamic RAM in the emulated system in KB.
uint32_t Options::getRamSizeKb() const
{
    return _ramSizeKb;
}

//! @brief Sets the size of the dynamic RAM in the emulated system in KB.
//! @param[in] ramSizeKb The new RAM size in kilobytes. Only certain sizes are
//! supported base on the architecture selected.
void Options::setRamSizeKb(uint32_t ramSizeKb)
{
    _ramSizeKb = ramSizeKb;
}

//! @brief Gets the amount of video RAM in the emulated system in KB.
uint32_t Options::getVideoRamSizeKb() const
{
    return _videoRamSizeKb;
}

//! @brief Sets the amount of dedicated video RAM in the emulated system in KB.
//! @param[in] vramSizeKb The new VRAM size. Only certain values are supported
//! based on the architecture selected.
void Options::setVideoRamSizeKb(uint32_t vramSizeKb)
{
    _videoRamSizeKb = vramSizeKb;
}

//! @brief Gets the type of monitor the emulated machine will appear to
//! be connected to.
DisplayInterface Options::getDisplayType() const
{
    return _display;
}

//! @brief Sets the type of monitor the emulated machine will appear to
//! be connected to.
//! @param[in] display The new display type. Only certain values are supported
//! based on the architecture selected.
void Options::setDisplayType(DisplayInterface display)
{
    _display = display;
}

//! @brief Gets the kind of hard drive interface which should appear on the
//! emulated system.
HardDiskInterface Options::getHardDiskTechnology() const
{
    return _hardDiskInterface;
}

//! @brief Sets the kind of hard drive interface which should appear on the
//! emulated system.
//! @param[in] hdTech The type of hard drive interface to emulate. Only certain
//! values are supported based on the architecture selected.
void Options::setHardDiskTechnology(HardDiskInterface hdTech)
{
    _hardDiskInterface = hdTech;
}

//! @brief Gets the count of hard disk drives attached to the
//! hard disk interface.
uint8_t Options::getHardDriveCount() const
{
    return _hardDriveCount;
}

//! @brief Sets the count of hard disk drives attached to the
//! hard disk interface.
//! @param[in] count The new drive count. Allowed values are 0-8 based on the
//! architecture and interface selected.
void Options::setHardDriveCount(uint8_t count)
{
    _hardDriveCount = count;
}

//! @brief Gets the count of floppy disk drives attached to the
//! emulated system.
uint8_t Options::getFloppyDiskCount() const
{
    return _floppyDriveCount;
}

//! @brief Sets the count of floppy disk drives attached to the
//! emulated system.
//! @param[in] count The count of drives. Valid values are 0-4 depending on the
//! architecture selected.
void Options::setFloppyDiskCount(uint8_t count)
{
    _floppyDriveCount = count;
}

//! @brief Gets the type of joystick technology to emulate.
JoystickInterface Options::getJoystickType() const
{
    return _joystickType;
}

//! @brief Sets the type of joystick technology to emulate.
//! @param[in] joystickTech The new joystick technology.
void Options::setJoystickType(JoystickInterface joystickTech)
{
    _joystickType = joystickTech;
}

//! @brief Gets the count of joysticks attached to the system.
uint8_t Options::getJoystickCount() const
{
    return _joystickCount;
}

//! @brief Sets the count of joysticks attached to the system.
//! @param[in] count The new joystick count.
void Options::setJoystickCount(uint8_t count)
{
    _joystickCount = count;
}

//! @brief Gets the identifier of the ROM to install in the memory map
//! of the emulated machine.
SystemROMPreset Options::getSystemRom() const
{
    return _systemRom;
}

//! @brief Sets the identifier of the ROM to install in the memory map
//! of the emulated machine.
//! @param[in] presetRom The new ROM preset. Valid values are limited based
//! on the architecture selected.
void Options::setSystemRom(SystemROMPreset presetRom)
{
    _systemRom = presetRom;
}

//! @brief Sets a custom ROM image to be installed in the memory map
//! of the emulated machine.
//! @param[in] romPath The path to the file containing the binary ROM image.
//! @note If the path is non-empty, the preset ROM will be set to a value
//! of Custom.
void Options::setCustomRom(const Ag::Fs::Path &romPath)
{
    _customRomPath = romPath;

    if (_customRomPath.isEmpty() == false)
    {
        _systemRom = SystemROMPreset::Custom;
    }
}

//! @brief Attempts to validate the combination of options currently set.
//! @param[out] error Receives details of the first error discovered.
//! @retval true The combination of options specified is valid.
//! @retval false The combination of options is invalid, error describes why.
bool Options::validate(Ag::String &error) const
{
    error = Ag::String::Empty;

    if (_model == SystemModel::TestBed)
    {
        // TODO: Expand the selection as support for new processors is added.
        if ((_processor != ProcessorModel::ARM2) && (_processor != ProcessorModel::ARM3))
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "An {0} processor is not currently supported in a {1} system.",
                                       { getProcessorModelType().toDisplayName(_processor),
                                         getSystemModelType().toDisplayName(_model) });
            return false;
        }

        if (_systemRom != SystemROMPreset::Custom)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A {0} system ROM is incompatible with an {1}-based system.",
                                       { getSystemROMPresetType().toDisplayName(_systemRom),
                                         getSystemModelType().toDisplayName(_model) });
            return false;
        }
    }
    else if ((_model == SystemModel::Archimedies) || (_model == SystemModel::ASeries))
    {
        if (_processor > ProcessorModel::ARM3_FPA)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "The {0} processor is not compatible with an {1}-based system.",
                                       { getProcessorModelType().toDisplayName(_processor),
                                         getSystemModelType().toDisplayName(_model) });
            return false;
        }

        if ((_processorSpeedMHz < 4) || (_processorSpeedMHz > 50))
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A processor speed of {0} MHz is not valid in an {1}-based system.",
                                       { _processorSpeedMHz, getSystemModelType().toDisplayName(_model) });
            return false;
        }

        bool isOK;

        switch (_ramSizeKb)
        {
        case 512:
            isOK = (_model == SystemModel::Archimedies);
            break;

        case 1024:
        case 2048:
        case 4096:
        case 8192:
        case 12288:
        case 16384:
            isOK = true;
            break;

        default:
            isOK = false;
            break;
        }

        if (isOK == false)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A RAM size of {0} KB valid in an {1}-based system.",
                                       { _ramSizeKb, getSystemModelType().toDisplayName(_model) });
            return false;
        }

        if (_videoRamSizeKb != 0)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "No dedicated VRAM can be fitted to an {0}-based system.",
                                       { getSystemModelType().toDisplayName(_model) });
            return false;
        }

        uint8_t maxFds = (_model == SystemModel::Archimedies) ? 4 : 2;

        if (_floppyDriveCount > maxFds)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A maximum of {0} drives can be fitted to an {1}-based system.",
                                       { _floppyDriveCount, getSystemModelType().toDisplayName(_model) });
            return false;
        }

        isOK = true;
        if (_model == SystemModel::Archimedies)
        {
            isOK = (_systemRom < SystemROMPreset::RiscOS_3_50);
        }
        else if (_model == SystemModel::ASeries)
        {
            isOK = (_systemRom == SystemROMPreset::Custom) ||
                   ((_systemRom >= SystemROMPreset::RiscOS_3_00) &&
                    (_systemRom < SystemROMPreset::RiscOS_3_50));
        }

        if (isOK == false)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A {0} system ROM is incompatible with an {1}-based system.",
                                       { getSystemROMPresetType().toDisplayName(_systemRom),
                                         getSystemModelType().toDisplayName(_model)});
            return false;
        }
    }
    else if (_model == SystemModel::RiscPC)
    {
        if (_processor < ProcessorModel::ARM610)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "The {0} processor is not compatible with an {1}-based system.",
                                       { getProcessorModelType().toDisplayName(_processor),
                                         getSystemModelType().toDisplayName(_model) });
            return false;
        }

        if (_processorSpeedMHz < 20)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A processor speed of {0} MHz is too slow for an {1}-based system.",
                                       { _processorSpeedMHz, getSystemModelType().toDisplayName(_model) });
            return false;
        }

        bool isOK = false;
        int msb, lsb;

        if ((Ag::Bin::popCount(_ramSizeKb) <= 4) &&
            Ag::Bin::bitScanForward(_ramSizeKb, lsb) &&
            Ag::Bin::bitScanReverse(_ramSizeKb, msb) &&
            (lsb >= 12) && (msb <= 18))
        {
            // The RiscPC can have up to 4 x SIMMs of between 4 and 64 MB.
            uint32_t size = _ramSizeKb;
            uint8_t simmCount = 0;

            while (Ag::Bin::bitScanReverse(size, lsb))
            {
                // Count 128MB and 256MB as 2 or 4 SIMMs respectively.
                simmCount += static_cast<uint8_t>(std::max<int>(lsb, 16) >> 16);

                // Clear the size bit.
                size ^= 1 << lsb;
            }

            if (simmCount > 4)
            {
                error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                           "A RAM size of {0}MB cannot be configured as up to 4 SIMMs"
                                           " of between 4MB and 64MB in a {1}-based system.",
                                           { _ramSizeKb >> 10, getSystemModelType().toDisplayName(_model) });
                return false;
            }
        }
        else
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A RAM size of {0}MB cannot be configured in a {1}-based system.",
                                       { _ramSizeKb >> 10, getSystemModelType().toDisplayName(_model) });
            return false;
        }

        uint32_t vramMB = _videoRamSizeKb >> 10;

        // Ensure VRAM is between 0-4MB in 1MB increments.
        if ((_videoRamSizeKb & 0x3FF) || (vramMB > 4))
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A video RAM size of {0}MB cannot be configured in a {1}-based system.",
                                       { _ramSizeKb >> 10, getSystemModelType().toDisplayName(_model) });
            return false;
        }

        if ((_display == DisplayInterface::NormalTV) ||
            (_display == DisplayInterface::HiResMono))
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A {0} display is incompatible with a {1}-based system.",
                                       { getDisplayInterfaceType().toDisplayName(_display),
                                         getSystemModelType().toDisplayName(_model) });
            return false;
        }

        if (_hardDiskInterface == HardDiskInterface::ST506)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "An {0} hard disk interface is incompatible with a {1}-based system.",
                                       { getHDInterfaceType().toDisplayName(_hardDiskInterface),
                                         getSystemModelType().toDisplayName(_model) });
            return false;
        }

        if (_floppyDriveCount > 4)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A maximum of {0} drives can be fitted to an {1}-based system.",
                                       { _floppyDriveCount, getSystemModelType().toDisplayName(_model) });
            return false;
        }

        isOK = false;

        if (_systemRom == SystemROMPreset::Custom)
        {
            // Works on any processor.
            isOK = true;
        }
        else if (_processor == ProcessorModel::ARM610)
        {
            isOK = (_systemRom >= SystemROMPreset::RiscOS_3_50);
        }
        else if (_processor <= ProcessorModel::ARM710_FPA)
        {
            // ARM710+ Needs RISC OS 3.60+
            isOK = (_systemRom >= SystemROMPreset::RiscOS_3_60);
        }
        else
        {
            // ARMv4 Needs RISC OS 3.70+
            isOK = (_systemRom >= SystemROMPreset::RiscOS_3_70);
        }

        if (isOK == false)
        {
            error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                       "A {0} system ROM is incompatible with an {1}/{2}-based system.",
                                       { getSystemROMPresetType().toDisplayName(_systemRom),
                                         getProcessorModelType().toDisplayName(_processor),
                                         getSystemModelType().toDisplayName(_model) });
            return false;
        }
    }

    uint8_t maxDrives = 0;

    switch (_hardDiskInterface)
    {
    case HardDiskInterface::None:
        if (_hardDriveCount > 0)
        {
            error = "There can be no hard drives when there is no hard disk interface.";
            return false;
        }
        break;

    case HardDiskInterface::ST506:
        maxDrives = 4;
        break;
        break;

    case HardDiskInterface::IDE:
        maxDrives = 2;
        break;

    case HardDiskInterface::SCSI:
        maxDrives = 8;
        break;
    }

    if (_hardDriveCount > maxDrives)
    {
        error = Ag::String::format(Ag::LocaleInfo::getDisplay(),
                                   "A maximum of {0} drives can be attached to an {1} hard disk interface.",
                                   { maxDrives, getHDInterfaceType().toDisplayName(_hardDiskInterface) });
        return false;
    }

    if (_joystickCount > 2)
    {
        error = "A maximum of 2 joysticks can be fitted.";
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Provides static metadata for the SystemModel enumeration type.
const SystemModelType &getSystemModelType()
{
    static const SystemModelType metadata({
        { SystemModel::TestBed, "TestBed", "Test bed", "A system only used in testing." },
        { SystemModel::Archimedies, "Archimedes", "Archimedes", "Original ARM2/3-based desktop systems from the A305 to the A540." },
        { SystemModel::ASeries, "ASeries", "A-Series", "The second generation ARM250/3-based desktop systems from the A3010 to the A5000." },
        { SystemModel::RiscPC, "RiscPC", "Risc PC", "ARMv3/v4-based desktop systems, including A7000 models." },
    });

    return metadata;
}

//! @brief Provides static metadata for the ProcessorModel enumeration type.
const ProcessorModelType &getProcessorModelType()
{
    static const ProcessorModelType metadata({
        { ProcessorModel::ARM2, "ARM2", "ARM 2", "Base on the original ARMv2 architecture." },
        { ProcessorModel::ARM3, "ARM3", "ARM 3", "The ARMv2 architecture with on-chip cache." },
        { ProcessorModel::ARM3_FPA, "ARM3_FPA", "ARM 3 + FPA", "The ARM 3 processor with a Floating Point Accelerator co-processor."},
        { ProcessorModel::ARM610, "ARM610", "ARM 610", "The first ARMv3 desktop processor." },
        { ProcessorModel::ARM710, "ARM710", "ARM 710", "The second ARMv3 desktop processor." },
        { ProcessorModel::ARM710_FPA, "ARM710_FPA", "ARM 710 + FPA", "An ARM700 core with a Floating Point Accelerator co-processor."},
        { ProcessorModel::ARM810, "ARM810", "ARM 810", "The experimental ARM-produced processor based on the ARMv4 architecture."},
        { ProcessorModel::ARM810_FPA, "ARM810_FPA", "ARM 810 + FPA", "A theoretical configuration combing the ARM 810 with a Floating Point Accelerator co-processor."},
        { ProcessorModel::StrongARM, "StrongARM", "Strong ARM", "The high performance DEC-produced processor based on the ARMv4 architecture."},
        { ProcessorModel::StrongARM_FPA, "StrongARM_FPA", "Strong ARM + FPA", "A theoretical configuration combing the Strong ARM with a Floating Point Accelerator co-processor." },
    });

    return metadata;
}

//! @brief Provides static metadata for the SystemROMPreset enumeration type.
const SystemROMPresetType &getSystemROMPresetType()
{
    static const SystemROMPresetType metadata({
        { SystemROMPreset::Custom, "Custom", "Custom ROM Image", "A custom ROM image provided by an external file." },
        { SystemROMPreset::Arthur_0_30, "Arthur_0_30", "Arthur 0.30", "The original Archimedes operating system heavily based on MOS." },
        { SystemROMPreset::Arthur_1_20, "Arthur_1_20", "Arthur 1.20", "The second iteration of the basic Archimedes operating system." },
        { SystemROMPreset::RiscOS_2_00, "RISCOS_2_00", "RISC OS 2.00", "A system with a graphically rich WIMP user interface." },
        { SystemROMPreset::RiscOS_2_01, "RISCOS_2_01", "RISC OS 2.01", "Contained minor bug fixes from RISC OS 2.00." },
        { SystemROMPreset::RiscOS_3_00, "RISCOS_3_00", "RISC OS 3.00", "Much more feature rich than RISC OS 2 with support for A-Series hardware." },
        { SystemROMPreset::RiscOS_3_10, "RISCOS_3_10", "RISC OS 3.10", "Adds bug fixes on top of RISC OS 3.00." },
        { SystemROMPreset::RiscOS_3_11, "RISCOS_3_11", "RISC OS 3.11", "Adds further minor bug fixes on RISC OS 3.10." },
        { SystemROMPreset::RiscOS_3_19, "RISCOS_3_19", "RISC OS 3.19", "A German language version of RISC OS 3.11." },
        { SystemROMPreset::RiscOS_3_50, "RISCOS_3_50", "RISC OS 3.50", "Provides support for RiscPC hardware and the ARM 610." },
        { SystemROMPreset::RiscOS_3_60, "RISCOS_3_60", "RISC OS 3.60", "Required to provide support for the ARM 710 processor." },
        { SystemROMPreset::RiscOS_3_70, "RISCOS_3_70", "RISC OS 3.70", "Required to provide support for the StrongARM processor." },
        { SystemROMPreset::RiscOS_3_71, "RISCOS_3_71", "RISC OS 3.71", "Contained additions for Java on a StrongARM RiscPC." },
    });

    return metadata;
}

//! @brief Provides static metadata for the DisplayInterface enumeration type.
const DisplayInterfaceType &getDisplayInterfaceType()
{
    static const DisplayInterfaceType metadata({
        { DisplayInterface::NormalTV, "NormalTV", "PAL TV", "A 50 Hz domestic PAL TV." },
        { DisplayInterface::HiResMono, "HiResMono", "High Resolution Mono", "A 2-colour mode created by VIDC hack." },
        { DisplayInterface::MultiScan, "MultiScan", "Multi-Scan", "A display which supports a vast variety of display modes." },
        { DisplayInterface::VGA, "VGA", "The PC-compatible 640 x 480 Visual Graphics Array display standard." },
        { DisplayInterface::SuperVGA, "SuperVGA", "An iteration of the VGA standard providing resolutions of upto 1600 x 1200." },
    });

    return metadata;
}

//! @brief Provides static metadata for the HardDiskInterface enumeration type.
const HDInterfaceType &getHDInterfaceType()
{
    static const HDInterfaceType metadata({
        { HardDiskInterface::None, "None", "None", "No hard disk interface fitted." },
        { HardDiskInterface::ST506, "ST506", "ST506", "1980s era MFM hard disk interface." },
        { HardDiskInterface::IDE, "IDE", "Integrated Drive Electronics. Supports up to 2 drives per interface." },
        { HardDiskInterface::SCSI, "SCSI", "Small Computer Systems Interface. Supports up to 8 devices per interface." },
    });

    return metadata;
}

//! @brief Provides static metadata for the JoystickInterface enumeration type.
const JoystickInterfaceType &getJoystickInterfaceType()
{
    static const JoystickInterfaceType metadata({
        { JoystickInterface::None, "None", "No Joysticks", "No joystick interface hardware fitted." },
        { JoystickInterface::Digital, "Digital", "Digital", "Atari/Amiga-style digital joysticks with 8 directions and 2 fire buttons."},
        { JoystickInterface::Analog, "Analogue", "Analogue", "PC-standard analogue joysticks with 2 axis and 2 fire buttons." },
    });

    return metadata;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


//! @file ArmEmu/EmuOptions.hpp
//! @brief The declaration of an object defining the configuration of a system
//! to emulate.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_OPTIONS_HPP__
#define __ARM_EMU_OPTIONS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "Ag/Core/EnumInfo.hpp"
#include "Ag/Core/FsPath.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the base hardware platform to be emulated.
enum class SystemModel : uint8_t
{
    //! @brief A system with a processor, 32KB RAM, 32KB ROM and no MMU or other
    //! hardware which is intended for testing purposes.
    TestBed,

    //! @brief The original Archimedes hardware using the ARM2/ARM3 with no
    //! fixed HD interface.
    Archimedies,

    //! @brief The A-Series hardware using the ARM250/ARM3 based on the
    //! 82c710/11 Universal Peripheral Controller II.
    ASeries,

    //! @brief The RiscPC hardware using the ARMv3 or ARMv4 based on the
    //! SMC FDC37C665GT Super I/O controller.
    RiscPC,
};

using SystemModelType = Ag::EnumInfo<SystemModel>;

//! @brief Defines the processor core to be emulated.
enum class ProcessorModel : uint8_t
{
    ARM2,
    ARM250,
    ARM3,
    ARM3_FPA,
    ARM610,
    ARM710,
    ARM710_FPA,
    ARM810,
    ARM810_FPA,
    StrongARM,
    StrongARM_FPA,
};

using ProcessorModelType = Ag::EnumInfo<ProcessorModel>;

//! @brief Identifies the preset ROM configured on the emulated system.
enum class SystemROMPreset : uint8_t
{
    Custom,
    Arthur_0_30,
    Arthur_1_20,
    RiscOS_2_00,
    RiscOS_2_01,
    RiscOS_3_00,
    RiscOS_3_10,
    RiscOS_3_11,
    RiscOS_3_19,
    RiscOS_3_50,
    RiscOS_3_60,
    RiscOS_3_70,
    RiscOS_3_71,
};

//! @brief An object which describes a symbol in the SystemROMPreset
//! enumeration type.
class SystemROMPresetSymbol : public Ag::EnumSymbol<SystemROMPreset>
{
private:
    // Internal Fields
    std::string_view _romImageFilename;

public:
    // Construction/Destruction
    SystemROMPresetSymbol(SystemROMPreset id);
    SystemROMPresetSymbol(SystemROMPreset id, Ag::utf8_cptr_t symbol,
                          Ag::utf8_cptr_t displayName,
                          Ag::utf8_cptr_t description,
                          Ag::utf8_cptr_t romImageFilename);
    ~SystemROMPresetSymbol() = default;

    // Accessors
    const std::string_view &getRomImageFilename() const;
};

using SystemROMPresetType = Ag::EnumInfo<SystemROMPreset, SystemROMPresetSymbol>;

//! @brief Defines the type of physical display device to assume is attached
//! to the emulated system.
enum class DisplayInterface : uint8_t
{
    NormalTV,
    HiResMono,
    MultiScan,
    VGA,
    SuperVGA,
};

using DisplayInterfaceType = Ag::EnumInfo<DisplayInterface>;

//! @brief Defines the type of hard disk interface which should appear attached
//! to the emulated system.
enum class HardDiskInterface : uint8_t
{
    None,
    ST506,
    IDE,
    SCSI,
};

using HDInterfaceType = Ag::EnumInfo<HardDiskInterface>;

//! @brief Defines the type of joystick controllers to be emulated.
enum class JoystickInterface : uint8_t
{
    None,
    Digital,
    Analog,
};

using JoystickInterfaceType = Ag::EnumInfo<JoystickInterface>;

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object defining the configuration of a system to emulate.
//! @details Any combination of values can be set, they will only be checked for
//! validity when the validate() member function is called.
class Options
{
public:
    // Construction/Destruction
    Options();
    ~Options() = default;

    // Accessors
    SystemModel getHardwareArchitecture() const;
    void setHardwareArchitecture(SystemModel model);
    ProcessorModel getProcessorVariant() const;
    void setProcessorVariant(ProcessorModel processor);
    uint16_t getProcessorSpeedMHz() const;
    void setProcessorSpeedMHz(uint16_t clockFreqMHz);
    uint32_t getRamSizeKb() const;
    void setRamSizeKb(uint32_t ramSizeKb);
    uint32_t getVideoRamSizeKb() const;
    void setVideoRamSizeKb(uint32_t vramSizeKb);
    DisplayInterface getDisplayType() const;
    void setDisplayType(DisplayInterface display);
    HardDiskInterface getHardDiskTechnology() const;
    void setHardDiskTechnology(HardDiskInterface hdTech);
    uint8_t getHardDriveCount() const;
    void setHardDriveCount(uint8_t count);
    uint8_t getFloppyDiskCount() const;
    void setFloppyDiskCount(uint8_t count);
    JoystickInterface getJoystickType() const;
    void setJoystickType(JoystickInterface joystickTech);
    uint8_t getJoystickCount() const;
    void setJoystickCount(uint8_t count);
    SystemROMPreset getSystemRom() const;
    void setSystemRom(SystemROMPreset presetRom);
    Ag::Fs::Path getRomPath() const;
    void setCustomRom(const Ag::Fs::Path &romPath);

    // Operations
    bool validate(Ag::String &error) const;

    static bool isValidMemcRAMSize(uint32_t ramSizeKb);
    static bool isValidRiscPCRAMSize(uint32_t ramSizeKb);
    static void setRomImageBasePath(const Ag::Fs::Path &basePath);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    Ag::Fs::Path _customRomPath;
    SystemModel _model;
    ProcessorModel _processor;
    uint16_t _processorSpeedMHz;
    uint32_t _ramSizeKb;
    uint32_t _videoRamSizeKb;
    DisplayInterface _display;
    HardDiskInterface _hardDiskInterface;
    uint8_t _hardDriveCount;
    uint8_t _floppyDriveCount;
    JoystickInterface _joystickType;
    uint8_t _joystickCount;
    SystemROMPreset _systemRom;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
const SystemModelType &getSystemModelType();
const ProcessorModelType &getProcessorModelType();
const SystemROMPresetType &getSystemROMPresetType();
const DisplayInterfaceType &getDisplayInterfaceType();
const HDInterfaceType &getHDInterfaceType();
const JoystickInterfaceType &getJoystickInterfaceType();

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

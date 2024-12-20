//! @file ArmEmu/ArmSystemBuilder.hpp
//! @brief The declaration of an object used to incrementally construct another
//! object representing an emulated ARM-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ARM_SYSTEM_BUILDER_HPP__
#define __ARM_EMU_ARM_SYSTEM_BUILDER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "EmuOptions.hpp"
#include "AddressMap.hpp"
#include "ArmSystem.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object used to incrementally construct an implementation of the
//! IArmSystem interface representing an emulated ARM-based system.
class ArmSystemBuilder
{
public:
    // Construction/Destruction
    ArmSystemBuilder();
    ArmSystemBuilder(const Options &baseOptions);
    ~ArmSystemBuilder() = default;

    // Accessors

    // Operations
    void addDevice(IHardwareDeviceUPtr &&device);
    void addMapping(IAddressRegionPtr region, uint32_t baseAddr, MemoryAccess access);
    void addMapping(IAddressRegionUPtr &&region, uint32_t baseAddr, MemoryAccess access);
    void reset(const Options &baseOptions);
    IArmSystemUPtr createSystem();
private:

    // Internal Fields
    Options _baseOptions;
    AddressMap _readMap;
    AddressMap _writeMap;
    HardwareDevicePool _devices;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

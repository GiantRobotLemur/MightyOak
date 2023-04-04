//! @file ArmEmu/SystemResources.hpp
//! @brief The declaration of an object which manages resources for an emulated
//! ARM-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_SYSTEM_RESOURCES_HPP__
#define __ARM_EMU_SYSTEM_RESOURCES_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <vector>

#include "IrqSink.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief an object which manages resources for an emulated
//! ARM-based system.
class SystemResources
{
public:
    // Construction/Destruction
    SystemResources(IIrqSink *irqSink);
    ~SystemResources() = default;

    // Accessors
    uint32_t getRamSize() const;
    uint8_t *getRam();
    const uint8_t *getRam() const;
    uint32_t getMainRomSize() const;
    const uint8_t *getMainRom() const;
    const GuestMMIO *queryReadAddressMap(uint32_t addr) const;
    const GuestMMIO *queryReadAddressMap(uint32_t addr, uint32_t &nextAddr) const;
    const GuestMMIO *queryWriteAddressMap(uint32_t addr) const;
    const GuestMMIO *queryWriteAddressMap(uint32_t addr, uint32_t &nextAddr) const;

    // Operations
    void loadMainRom(const void *romData, uint32_t byteCount);
    void addMemoryMapping(uint32_t baseAddr, uint32_t size,
                          void *hostBlock);
    void addMemoryMapping(uint32_t baseAddr, uint32_t size,
                          const void *hostBlock);
    void addMMIOMappping(uint32_t baseAddr, uint32_t size, uintptr_t context,
                         ReadMemFn read, WriteMemFn write = nullptr);
    void regenerateMemoryMap();
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    IIrqSink *_irqSink;
    std::vector<uint8_t> _ram;
    std::vector<uint8_t> _mainRom;
    std::vector<GuestMMIO> _mappedDevices;
    std::vector<GuestMMIO> _readMappings;
    std::vector<GuestMMIO> _writeMappings;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

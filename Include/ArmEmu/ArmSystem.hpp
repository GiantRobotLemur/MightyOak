//! @file ArmEmu/ArmSystem.hpp
//! @brief The declaration of an object representing an emulated ARM-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ARM_SYSTEM_HPP__
#define __ARM_EMU_ARM_SYSTEM_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <memory>

#include "Ag/Core/EnumInfo.hpp"
#include "ArmEmu/AddressMap.hpp"
#include "ArmEmu/ExecutionMetrics.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the operating modes of the ARM processor.
enum class ProcessorMode : uint8_t
{
    User26 = 0x00,
    FastIrq26 = 0x01,
    Irq26 = 0x02,
    Svc26 = 0x03,
    User32 = 0x10, // ARMv3+
    FastIrq32 = 0x11, // ARMv3+
    Irq32 = 0x12, // ARMv3+
    Svc32 = 0x13, // ARMv3+
    Abort = 0x17, // ARMv3+
    Undefined = 0x1B, // ARMv3+
    System = 0x1F, // ARMv4+

    PrivilegeMask = 0x0F, // If any bits are set, the most is privileged.
    Max,
};

//! @brief Defines the banks of registers which may exist in the
//! emulated processor.
enum class RegisterBank : uint8_t
{
    User,
    FastInterrupt,
    Interrupt,
    Supervisor,
    Abort,
    Undefined,
    System,

    Max,
};

//! @brief Identifies a register in the processor core.
enum class CoreRegister : uint8_t
{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    SPSR,
    CPSR,
    PC,

    Max,
};

//! @brief Used to define access to a range of mapped addresses in the
//! guest address space.
enum class MemoryAccess : uint8_t
{
    //! @brief The range cannot be read or written to (why?)
    None = 0x00,

    //! @brief The range can only be read.
    ReadOnly = 0x01,

    //! @brief The range can only be written to.
    WriteOnly = 0x02,

    //! @brief The range can be both read and written to.
    ReadWrite = 0x03,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An abstract interface to a component which emulates a 32-bit ARM
//! processor core and associated devices.
class IArmSystem
{
public:
    // Construction/Destruction
    virtual ~IArmSystem() = default;

    // Accessors
    virtual ProcessorMode getMode() const = 0;
    virtual uint32_t getCoreRegister(CoreRegister id) const = 0;
    virtual void setCoreRegister(CoreRegister id, uint32_t value) = 0;
    virtual const AddressMap &getReadAddresses() const = 0;
    virtual const AddressMap &getWriteAddresses() const = 0;
    virtual bool logicalToPhysicalAddress(uint32_t logicalAddr,
                                          uint32_t &physAddr) const = 0;

    // Operations
    virtual ExecutionMetrics run() = 0;
    virtual ExecutionMetrics runSingleStep() = 0;
};

//! @brief A custom deleter for IArmSystem implementations.
struct IArmSystemDeleter
{
    void operator()(IArmSystem *sys) const;
};

//! @brief An alias for a unique pointer to a simulated ARM system.
using IArmSystemUPtr = std::unique_ptr<IArmSystem, IArmSystemDeleter>;

//! @brief A customised EnumSymbol describing an ARM processor mode.
struct ProcessorModeInfo : public Ag::EnumSymbol<ProcessorMode>
{
private:
    uint8_t _minArchVersion;
public:
    ProcessorModeInfo(ProcessorMode id);
    ProcessorModeInfo(ProcessorMode id, const char *symbol, const char *displayName,
                      const char *description, uint8_t minArchVersion);

    bool is26Bit() const;
    uint8_t getMinimumArchitectureVersion() const;
};

using ProcessorModeEnumInfo = Ag::EnumInfo<ProcessorMode, ProcessorModeInfo>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
const ProcessorModeEnumInfo &getProcessorModeType();

uint32_t readFromPhysicalAddress(IArmSystem *sys, uint32_t physicalAddr,
                                 void *buffer, uint32_t length);
uint32_t writeToPhysicalAddress(IArmSystem *sys, uint32_t physicalAddr,
                                const void *buffer, uint32_t length,
                                bool useReadMap = false);
uint32_t readFromLogicalAddress(IArmSystem *sys, uint32_t logicalAddr,
                                void *buffer, uint32_t length);
uint32_t writeToLogicalAddress(IArmSystem *sys, uint32_t logicalAddr,
                               const void *buffer, uint32_t length,
                               bool useReadMap = false);

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

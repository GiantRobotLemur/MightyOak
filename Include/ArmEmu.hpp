//! @file ArmEmu.hpp
//! @brief The declaration of public elements of the ArmEmu library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_HPP__
#define __ARM_EMU_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
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
    virtual void destroy() = 0;

    // Accessors
    virtual ProcessorMode getMode() const = 0;
    virtual uint32_t getCoreRegister(CoreRegister id) const = 0;
    virtual uint32_t readFromLogicalAddress(uint32_t logicalAddr, uint32_t length,
                                            void *buffer) const = 0;

    // Operations
    virtual uint32_t run() =0;
    virtual uint32_t runSingleStep() = 0;
};

//! @brief A functor which calls the destroy() member function on an IProcessor
//! implementation as part of an std::unique_ptr specialisation.
struct IArmSystemDeleter
{
    void operator()(IArmSystem *processor) const;
};

using IArmSystemUPtr = std::unique_ptr<IArmSystem, IArmSystemDeleter>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
IArmSystemUPtr createUserModeTestSystem(const char *assembler);
const char *coreRegisterToString(CoreRegister regId);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/IrqSink.hpp
//! @brief The declaration of an interface to an object which can receive
//! emulated interrupt signals.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_IRQ_SINK_HPP__
#define __ARM_EMU_IRQ_SINK_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////

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
//! @brief The interface to an object which can receive emulated interrupt
//! signals.
class IIrqSink
{
public:
    // Construction/Destruction
    virtual ~IIrqSink() = default;

    // Accessors

    // Operations
    //! @brief Marks an interrupt as pending.
    virtual void raiseIrq() = 0;

    //! @brief Clears any pending interrupt.
    virtual void clearIrq() =0;

    //! @brief Marks a fast interrupt as pending.
    virtual void raiseFirq() =0;

    //! @brief Clears any pending fast interrupt.
    virtual void clearFirq() =0;
};

//! @brief Used to define host access to a range or mapped addresses in the
//! guest address space.
enum class MemoryAccess
{
    //! @brief The range cannot be read or written to (why?)
    None        = 0x00,

    //! @brief The range can only be read.
    ReadOnly    = 0x01,

    //! @brief The range can only be written to.
    WriteOnly   = 0x02,

    //! @brief The range can be both read and written to.
    ReadWrite   = 0x03,
};

//! @brief Describes a block of memory shared between the guest and host system
//! which can be writeable or read-only.
struct HostBlock
{
    //! @brief The offset of the first mapped byte in the guest memory map.
    uint32_t GuestBaseAddr;

    //! @brief The count of bytes in the range and of addresses mapped.
    //! @note This must be a whole multiple of 4.
    uint32_t Size;

    //! @brief A pointer to the block of host memory mapped into the guest
    //! address space.
    void *HostBaseAddr;
};

//! @brief A pointer to a function to read a location in a range of
//! guest addresses.
//! @param[in] context The context value associated with the MMIO address
//! range being read.
//! @param[in] offset The word-aligned offset of the byte, half-word or
//! word to read.
//! @return The word read from the specified location.
typedef uint32_t(*ReadMemFn)(uintptr_t context, uint32_t offset);


//! @brief A pointer to a function to write up to 32-bits to a location in
//! a range of guest addresses.
//! @param[in] context The context value associated with the MMIO address
//! range being written to.
//! @param[in] offset The word-aligned offset of the byte, half-word or 
//! word to write.
typedef void (*WriteMemFn)(uintptr_t context, uint32_t offset, uint32_t value);

//! @brief Describes a run of memory mapped I/O in the guest memory map.
struct GuestMMIO
{
    //! @brief The address of the first mapped word in the guest memory map.
    //! @note This must be 32-bit aligned.
    uint32_t GuestBaseAddr;

    //! @brief The count of bytes in the range and of addresses mapped.
    //! @note This must be a whole multiple of 4.
    uint32_t Size;

    //! @brief The context value to pass to the Read() and Write() entry points.
    uintptr_t Context;

    //! @brief A function to read a location in the range.
    ReadMemFn Read;

    //! @brief A function to write up to 32-bits to a location in the range.
    WriteMemFn Write;
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

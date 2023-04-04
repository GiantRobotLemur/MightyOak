//! @file ArmEmu/Hardware.hpp
//! @brief The declaration of a layer of ARM system emulation which encapsulates
//! RAM, ROM, memory-mapped I/O and interrupts.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_HARDWARE_HPP__
#define __ASM_EMU_HARDWARE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <memory>

#include "IrqSink.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An enumeration defining the mask of bits indicating which interrupt
//! types are active.
enum IrqStatus : uint8_t
{
    //! @brief Signals that a fast interrupt signal has been raised.
    IS_FirqPending      = 0x01,

    //! @brief Signals that a conventional interrupt signal has been raised.
    IS_IrqPending       = 0x02,

    //! @brief Signals that a debug signal has been raised via execution of
    //! the bkpt instruction.
    IS_DebugPending     = 0x04,

    //! @brief Signals that the emulator host has raise a signal to interrupt
    //! the session.
    IS_HostIrqPending   = 0x08,

    //! @brief A mask which covers maskable interrupt types.
    IS_IrqMask          = 0x03,

    //! @brief A mask which covers non-maskable interrupts which cause execution
    //! to cease.
    IS_HostMask         = 0x0C,

    //! @brief A mask which covers all interrupt types.
    IS_Mask             = 0x0F,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class SystemResources;

//! @brief The interface to the underlying system, RAM, ROM, MMIO and interrupts.
class Hardware : public IIrqSink
{
private:
    // Internal Fields
    SystemResources *_resources;
    uint8_t _irqState;
    uint8_t _irqMask;
    bool _isPrivileged;

public:
    // Public Constants
    static const uint32_t MainRomBase = 0x00000000; // 0KB
    static const uint32_t PhysicalRamBase = 0x00008000; // 32KB
    static const uint32_t PhysicalRamSize = 0x00001000; // 4KB
    static const uint32_t AddrSpaceMax = 0x04000000; // 64MB

    // Construction/Destruction
    Hardware();
    virtual ~Hardware() = default;

    // Accessors
    uint8_t getPendingIrqs() const;
    bool isPrivilegedMode() const;
    void setPrivilegedMode(bool isPrivileged);
    uint8_t getIrqMask() const;
    void setIrqMask(uint8_t mask);

    // Operations
    void initialiseSystemResources(SystemResources *resources);

    bool writeByte(uint32_t logicalAddr, uint8_t value);
    bool writeHalfWord(uint32_t logicalAddr, uint16_t value);
    bool writeWord(uint32_t logicalAddr, uint32_t value);
    bool writeWords(uint32_t logicalAddr, const uint32_t *values, uint8_t count);

    bool readByte(uint32_t logicalAddr, uint8_t &result);
    bool readHalfWord(uint32_t logicalAddr, uint16_t &result);
    bool readWord(uint32_t logicalAddr, uint32_t &result);
    bool readWords(uint32_t logicalAddr, uint32_t *results, uint8_t count);

    bool exchangeByte(uint32_t logicalAddr, uint8_t &value);
    bool exchangeWord(uint32_t logicalAddr, uint32_t &value);
    uint8_t fetchInstructions(uint32_t logicalAddr, uint32_t *pipeline);

    uint32_t rawReadLogicalMemory(uint32_t logicalAddr, void *data, uint32_t byteCount) const;

    void raiseDebugIrq();
    void clearDebugIrq();
    void raiseHostIrq();
    void clearHostIrq();

    // Overrides
    virtual void raiseIrq() override;
    virtual void clearIrq() override;
    virtual void raiseFirq() override;
    virtual void clearFirq() override;

protected:
    // Internal Functions
    void raiseIrqInternal();
    void clearIrqInternal();
    void raiseFirqInternal();
    void clearFirqInternal();
    static bool isValidAddress(uint32_t physicalAddr) noexcept;
private:
    // Internal Functions
    bool tryDecodeReadAddress(uint32_t addr, const uint8_t *&hostAddr, uint32_t &runSize) const;
    bool tryDecodeWriteAddress(uint32_t addr, uint8_t *&hostAddr, uint32_t &runSize) const;
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

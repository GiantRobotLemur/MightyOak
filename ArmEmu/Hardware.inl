//! @file ArmEmu/name_here.inl
//! @brief The declaration of an example of an implementation of a hardware layer underlying
//! register files and data transfer.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_HARDWARE_INL__
#define __ARM_EMU_HARDWARE_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <vector>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines bit values which define the hardware layer interrupt status
//! field.
struct IrqState
{
    //! @brief Signals that a fast interrupt signal has been raised.
    static constexpr uint8_t FastIrqPending = 0x01;

    //! @brief Signals that a conventional interrupt signal has been raised.
    static constexpr uint8_t IrqPending = 0x02;

    //! @brief Signals that a debug signal has been raised via execution of
    //! the bkpt instruction.
    static constexpr uint8_t DebugPending = 0x04;

    //! @brief Signals that the emulator host has raise a signal to interrupt
    //! the session.
    static constexpr uint8_t HostPending = 0x08;

    //! @brief A mask which covers maskable interrupt types.
    static constexpr uint8_t GuestIrqsMask = 0x03;

    //! @brief A mask which covers non-maskable interrupts which cause execution
    //! to cease.
    static constexpr uint8_t HostIrqsMask = 0x0C;

    //! @brief A mask which covers all interrupt types.
    static constexpr uint8_t AllIrqsMask = 0x0F;
};

//! @brief Describes a region of memory in the memory map of an emulated system
//! and how that maps to memory in the host system.
struct MemoryMapping
{
    // Public Fields
    void *HostAddress;
    uint32_t GuestAddress;
    uint32_t Size;

    MemoryMapping() :
        HostAddress(nullptr),
        GuestAddress(0),
        Size(0)
    {
    }

    MemoryMapping(void *hostAddr, uint32_t guestAddr, uint32_t byteCount) :
        HostAddress(hostAddr),
        GuestAddress(guestAddr),
        Size(byteCount)
    {
    }
};

//! @brief A structure which calculates an alignment mask for 32-bit
//! addresses at compile time.
//! @tparam T The data type being transferred.
template<typename T> struct AlignedAddr
{
    //! @brief Gives the bit mask required to truncate a 32-bit address
    //! base on the data type being transferred.
    static constexpr uint32_t mask = ~static_cast<uint32_t>(sizeof(T) - 1);
};

//! @brief An alias for a vector of bytes representing RAM or ROM in the
//! guest memory map backed by host RAM.
//! @todo Implement an allocator to ensure the pointer is at least 16-byte
//! aligned.
using HostBuffer = std::vector<uint8_t>;

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief An example of an implementation of a hardware layer underlying
//! register files and data transfer.
class GenericHardware
{
    // IRQ Management
    //! @brief Gets the bit field indicating which unmasked interrupts are
    //! pending, if any.
    //! @returns IrqsPending & ~IrqMask
    //! @note Bit patterns are described by the constants of the
    //! IrqState structure.
    uint8_t getIrqStatus() const noexcept;

    // Operations
    //! @brief Signals the effect of a system reset on the hardware, returning
    //! it to a known power-on state.
    void reset();

    //! @brief Updates the bits of the bits of the interrupt mask field.
    //! @param[in] mask The new pattern of bits to apply to the mask.
    //! @param[in] significantBits The bits describing which digits of mask
    //! parameter are significant.
    //! @note Bit patterns are described by the constants of the
    //! IrqState structure.
    void updateIrqMask(uint8_t mask, uint8_t significantBits) noexcept;

    //! @brief Determines whether the processor is operating in a privileged
    //! mode for the purposes of accessing memory.
    bool isPrivilegedMode() const noexcept;

    //! @brief Sets whether the processor is operating in a privileged mode
    //! for the purposes of accessing memory.
    void setPrivilegedMode(bool isPrivileged) noexcept;

    // Operations
    //! @brief Updates the pending interrupt state to indicate whether a debug
    //! interrupt is currently pending.
    //! @param[in] isRaised True to mark the interrupt as raised, false to mark
    //! it as handled.
    //! @note Debug interrupts are generally raised when the processor executes
    //! a BKPT instruction or is in single step mode.
    void setDebugIrq(bool isRaised) noexcept;

    //! @brief Updates the pending interrupt state to indicate whether a
    //! host-generated interrupt is currently pending.
    //! @param[in] isRaised True to mark the interrupt as raised, false to mark
    //! it as handled.
    //! @note Host interrupts are raised by the emulator application to
    //! interrupt the execution loop of the guest processor.
    void setHostIrq(bool isRaised) noexcept;

    //! @brief Updates the pending interrupt state to indicate whether a
    //! guest-generated interrupt is currently pending.
    //! @param[in] isRaised True to mark the interrupt as raised, false to mark
    //! it as handled.
    //! @note Guest interrupts are raised by emulated interrupt controller
    //! hardware which allows multiple (simulated) hardware devices to interrupt
    //! the processor.
    void setGuestIrq(bool isRaised) noexcept;

    //! @brief Updates the pending fast interrupt state to indicate whether a
    //! guest-generated fast interrupt is currently pending.
    //! @param[in] isRaised True to mark the interrupt as raised, false to mark
    //! it as handled.
    //! @note Guest fast interrupts are raised by emulated interrupt controller
    //! hardware which allows multiple (simulated) hardware devices to interrupt
    //! the processor.
    void setGuestFastIrq(bool isRaised) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    // Address Map Access
    ///////////////////////////////////////////////////////////////////////////

    //! @brief Writes an integral value to a location in the guest memory map.
    //! @tparam T The integral data type to write.
    //! @param[in] logicalAddr The logical address in guest memory to write to.
    //! @param[in] value The value to write.
    //! @retval true The value was successfully written.
    //! @retval false The address didn't point to a location backed by the host
    //! or it was protected and the processor wasn't running in a privileged
    //! mode.
    //! @note The address will be truncated to be a whole multiple of the
    //! size of the value being transferred.
    template<typename T> bool write(uint32_t logicalAddr, T value);

    //! @brief Writes a run of 32-bit words to the emulated memory map.
    //! @param[in] logicalAddr The logical address in the guest memory map to
    //! write to.
    //! @param[in] values A pointer to the array of words to write.
    //! @param[in] count The count of words pointed to by values.
    //! @retval true The first word was transferred successfully, any failures
    //! to transfer more were silently ignored.
    //! @retval false The transfer of the first word raised an abort signal.
    //! @note The address will be truncated to be a whole multiple of 4 bytes.
    bool writeWords(uint32_t logicalAddr, const uint32_t *values, uint8_t count);

    //! @brief Reads an integral value from the guest memory map.
    //! @tparam T The integral data type to read.
    //! @param[in] logicalAddr The logical address in the guest memory map to
    //! read from.
    //! @param[out] result Receives the value read from the memory map.
    //! @retval true The value was successfully read.
    //! @retval false The address didn't point to a location backed by the host
    //! or it was protected and the processor wasn't running in a privileged
    //! mode.
    //! @note The address will be truncated to be a whole multiple of the
    //! size of the value being transferred.
    template<typename T> bool read(uint32_t logicalAddr, T &result);

    //! @brief Reads a run of 32-bit words from the guest memory map.
    //! @param[in] logicalAddr The logical address in the guest memory map to
    //! read from.
    //! @param[out] results Receives the words read from the memory map.
    //! @param[in] count The count of words to read.
    //! @retval true The first word was read successfully, failure to read
    //! subsequent words was silently ignored.
    //! @retval false Reading the first word from memory triggered an abort signal.
    //! @note The address will be truncated to be a whole multiple of 4 bytes.
    bool readWords(uint32_t logicalAddr, uint32_t *results, uint8_t count);

    //! @brief Attempts to atomically exchange a value with guest memory.
    //! @tparam T The integral data type to read and write.
    //! @param[in] logicalAddr The logical address of the value to read and update.
    //! @param[in] writeValue Contains the value to write to the specified
    //! location in memory.
    //! @param[out] readValue Receives the value read from the target memory
    //! location before it was overwritten by writeValue.
    //! @retval true The value was successfully exchanged.
    //! @retval false The address didn't point to a location backed by the host
    //! or it was protected and the processor wasn't running in a privileged
    //! mode.
    //! @note The address will be truncated to be a whole multiple of the
    //! size of the value being transferred.
    template<typename T> bool exchange(uint32_t logicalAddr, T writeValue, T &readValue);

    ///////////////////////////////////////////////////////////////////////////
    // Address Map Inspection
    ///////////////////////////////////////////////////////////////////////////
    //! @brief Attempts to convert a logical address to a physical address
    //! given the current state of the system.
    //! @param[in] logicalAddr The logical address to translate.
    //! @param[out] physAddr The physical address produced by the conversion if
    //! one was possible. The next available logical address if conversion
    //! wasn't possible.
    //! @retval true The logical address had a corresponding physical address
    //! which was returned in the physAddr parameter.
    //! @retval false The logical address was not mapped to a physical address.
    //! The physAddr parameter is updated with the next logical address after
    //! logicalAddr which can be mapped, or 0.
    bool logicalToPhysicalAddress(uint32_t logicalAddr,
                                  uint32_t &physAddr) const;

    //! @brief Gets a map describing the entities read from indexed by physical
    //! address.
    const AddressMap &getReadAddressMap() const;

    //! @brief Gets a map describing the entities written to indexed by physical
    //! address.
    const AddressMap &getWriteAddressMap() const;
};

//! @brief An implementation of the common interrupt management requirements of
//! GenericHardware.
class BasicIrqManagerHardware
{
protected:
    // Internal Fields
    AddressMap _masterReadMap;
    AddressMap _masterWriteMap;

private:
    uint8_t _irqStatus;
    uint8_t _irqMask;
    bool _isPriviledged;

public:
    // Construction/Destruction
    BasicIrqManagerHardware() :
        _irqStatus(0),
        _irqMask(0),
        _isPriviledged(false)
    {
    }

    BasicIrqManagerHardware(const AddressMap &readMap,
                            const AddressMap writeMap) :
        _masterReadMap(readMap),
        _masterWriteMap(writeMap),
        _irqStatus(0),
        _irqMask(0),
        _isPriviledged(false)
    {
    }

    // Accessors
    uint8_t getIrqStatus() const noexcept { return _irqStatus & ~_irqMask; }

    // Operations
    void updateIrqMask(uint8_t mask, uint8_t significantBits) noexcept
    {
        _irqMask &= ~mask;
        _irqMask |= significantBits & mask;
    }

    constexpr bool isPrivilegedMode() const noexcept { return _isPriviledged; }

    void setPrivilegedMode(bool isPrivileged) noexcept
    {
        _isPriviledged = isPrivileged;
    }

    void setDebugIrq(bool isRaised) noexcept
    {
        _irqStatus = (_irqStatus & ~IrqState::DebugPending) |
                     (-static_cast<uint8_t>(isRaised) & IrqState::DebugPending);
    }

    void setHostIrq(bool isRaised) noexcept
    {
        _irqStatus = (_irqStatus & ~IrqState::HostPending) |
                     (-static_cast<uint8_t>(isRaised) & IrqState::HostPending);
    }

    void setGuestIrq(bool isRaised) noexcept
    {
        _irqStatus = (_irqStatus & ~IrqState::IrqPending) |
                     (-static_cast<uint8_t>(isRaised) & IrqState::IrqPending);
    }

    void setGuestFastIrq(bool isRaised) noexcept
    {
        _irqStatus = (_irqStatus & ~IrqState::FastIrqPending) |
                     (-static_cast<uint8_t>(isRaised) & IrqState::FastIrqPending);
    }

    const AddressMap &getReadAddressMap() const { return _masterReadMap; }

    const AddressMap &getWriteAddressMap() const { return _masterWriteMap; }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/Hardware.cpp
//! @brief The definition of a layer of ARM system emulation which encapsulates
//! RAM, ROM, memory-mapped I/O and interrupts.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>

#include <algorithm>

#include "Hardware.hpp"
#include "SystemResources.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// Hardware Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs the hardware level of an emulated ARM processor.
Hardware::Hardware() :
    _resources(nullptr),
    _irqState(0),
    _irqMask(IS_HostMask),
    _isPrivileged(true)
{
}

//! @brief Gets the status of pending interrupts apart from those
//! types which have been masked.
uint8_t Hardware::getPendingIrqs() const
{
    return _irqState & ~_irqMask;
}

//! @brief Determines if the processor is operating in a non-user mode.
//! @retval true The processor is operating in a non-user mode which allows
//! access to protected memory ranges.
//! @retval false The processor is operating in a user mode, access to
//! protected memory will fail.
bool Hardware::isPrivilegedMode() const { return _isPrivileged; }

//! @brief Sets whether the processor is in a non-user mode. This affects
//! the protection checks when accessing memory.
void Hardware::setPrivilegedMode(bool isPrivileged)
{
    _isPrivileged = isPrivileged;
}

//! @brief Gets the bits which mask pending interrupts.
uint8_t Hardware::getIrqMask() const { return _irqMask; }

//! @brief Updates the mask of pending interrupts.
//! @param[in] mask Set bits indicate masked interrupt types, e.g. IS_IrqPending.
//! @note Affects the value returned by getPendingIrqs().
//! @note Debug (breakpoint) and Host interrupts can never
//! be masked.
void Hardware::setIrqMask(uint8_t mask)
{
    // Only allow IRQs and FIRQs to be masked.
    _irqMask = mask & IS_IrqMask;
}

//! @brief Connects the emulated processor hardware to the system resources
//! it accesses via memory read and write operations.
//! @param[in] resources The system resources object to connect to.
void Hardware::initialiseSystemResources(SystemResources *resources)
{
    _resources = resources;
}

//! @brief Writes a byte to the emulated memory map.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! write to.
//! @param[in] value The byte value to write.
//! @retval true The byte was successfully written.
//! @retval false The address didn't make to a location backed by the host.
bool Hardware::writeByte(uint32_t logicalAddr, uint8_t value)
{
    bool isWritten = false;
    uint8_t *hostAddr;
    uint32_t hostSize;

    if (tryDecodeWriteAddress(logicalAddr, hostAddr, hostSize))
    {
        *hostAddr = value;
        isWritten = true;
    }
    else
    {
        uint32_t wordAddr = logicalAddr & ~3;
        const GuestMMIO *region = _resources->queryWriteAddressMap(wordAddr);

        if (region != nullptr)
        {
            // Replicate the bits to write across the entire bus.
            uint32_t wordValue = static_cast<uint32_t>(value);
            wordValue |= wordValue << 8;
            wordValue |= wordValue << 16;

            region->Write(region->Context,
                          wordAddr - region->GuestBaseAddr,
                          wordValue);

            isWritten = true;
        }
    }

    return isWritten;
}

//! @brief Writes a 16-bit half-word to the emulated memory map.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! write to.
//! @param[in] value The half-word value to write.
//! @retval true The half-word was successfully written.
//! @retval false The address didn't make to a location backed by the host.
bool Hardware::writeHalfWord(uint32_t logicalAddr, uint16_t value)
{
    bool isWritten = false;
    uint8_t *hostAddr;
    uint32_t hostSize;

    if (tryDecodeWriteAddress(logicalAddr & ~1, hostAddr, hostSize))
    {
        *reinterpret_cast<uint16_t *>(hostAddr) = value;
        isWritten = true;
    }
    else
    {
        uint32_t wordAddr = logicalAddr & ~3;
        const GuestMMIO *region = _resources->queryWriteAddressMap(wordAddr);

        if (region != nullptr)
        {
            // Replicate the bits to write across the entire bus.
            uint32_t wordValue = static_cast<uint32_t>(value) |
                                 (static_cast<uint32_t>(value) << 16);

            region->Write(region->Context,
                          wordAddr - region->GuestBaseAddr,
                          wordValue);

            isWritten = true;
        }
    }

    return isWritten;
}

//! @brief Writes a 32-bit word to the emulated memory map.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! write to.
//! @param[in] value The word value to write.
//! @retval true The word was successfully written.
//! @retval false The address didn't make to a location backed by the host.
bool Hardware::writeWord(uint32_t logicalAddr, uint32_t value)
{
    bool isWritten = false;
    uint8_t *hostAddr;
    uint32_t hostSize;
    uint32_t wordAddr = logicalAddr & ~3;

    if (tryDecodeWriteAddress(wordAddr, hostAddr, hostSize))
    {
        *reinterpret_cast<uint32_t *>(hostAddr) = value;
        isWritten = true;
    }
    else
    {
        const GuestMMIO *region = _resources->queryWriteAddressMap(wordAddr);

        if (region != nullptr)
        {
            region->Write(region->Context,
                          wordAddr - region->GuestBaseAddr,
                          value);

            isWritten = true;
        }
    }

    return isWritten;
}

//! @brief Writes a run of 32-bit words to the emulated memory map.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! write to.
//! @param[in] values A pointer to the array of words to write.
//! @param[in] count The count of words pointed to by values.
//! @retval true The first word was transferred successfully.
//! @retval false The transfer of the first word raised an abort signal.
bool Hardware::writeWords(uint32_t logicalAddr, const uint32_t *values,
                          uint8_t count)
{
    uint32_t guestAddr = logicalAddr & ~3;
    uint32_t wordsToWrite;
    uint8_t wordsWritten = 0;

    bool firstWordWritten = false;

    while (wordsWritten < count)
    {
        uint8_t *hostAddr;
        uint32_t hostSize;

        // TODO: Use memory protection to decide whether an abort signal
        // was asserted.
        bool isOK = true;

        if (tryDecodeWriteAddress(guestAddr, hostAddr, hostSize))
        {
            wordsToWrite = std::min(hostSize / 4,
                                    static_cast<uint32_t>(count - wordsWritten));

            // TODO: Use STL to copy the values, this maybe too slow in debug.
            std::copy_n(values + wordsWritten, wordsToWrite,
                        reinterpret_cast<uint32_t *>(hostAddr));
        }
        else
        {
            // Determine if the address is in memory mapped I/O.
            uint32_t nextAddr;
            const GuestMMIO *region = _resources->queryWriteAddressMap(guestAddr,
                                                                       nextAddr);

            if (region == nullptr)
            {
                // There is nothing mapped at the specified address.
                uint32_t gapSize = nextAddr - guestAddr;

                wordsToWrite = std::min(gapSize / 4,
                                       static_cast<uint32_t>(count - wordsWritten));
            }
            else
            {
                uint32_t offset = guestAddr - region->GuestBaseAddr;
                wordsToWrite = std::min((region->Size - offset) / 4,
                                       static_cast<uint32_t>(count - wordsWritten));

                for (uint32_t i = 0; i < wordsToWrite; ++i)
                {
                    region->Write(region->Context, offset + (i * 4),
                                  values[wordsWritten + i]);
                }
            }
        }

        if (wordsWritten == 0)
        {
            firstWordWritten = isOK;
        }

        wordsWritten += static_cast<uint8_t>(wordsToWrite);
        guestAddr += wordsToWrite * 4;
    }

    return firstWordWritten;
}

//! @brief Reads a byte from the guest memory map.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! read from.
//! @param[out] result Receives the byte read from the memory map.
//! @retval true The byte was successfully read.
//! @retval false The specified memory location was not backed by the host.
bool Hardware::readByte(uint32_t logicalAddr, uint8_t &result)
{
    bool isRead = false;
    const uint8_t *hostAddr;
    uint32_t hostSize;

    if (tryDecodeReadAddress(logicalAddr, hostAddr, hostSize))
    {
        result = *hostAddr;
        isRead = true;
    }
    else
    {
        uint32_t wordAddr = logicalAddr & ~3;
        const GuestMMIO *region = _resources->queryReadAddressMap(wordAddr);

        if (region != nullptr)
        {
            uint32_t word = region->Read(region->Context,
                                         wordAddr - region->GuestBaseAddr);

            word >>= (logicalAddr & 3) << 3; // Shift by 0, 8, 16 or 24 before casting.
            result = static_cast<uint8_t>(word);

            isRead = true;
        }
    }

    return isRead;
}

//! @brief Reads a 16-bit half-word from the guest memory map.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! read from.
//! @param[out] result Receives the half-word read from the memory map.
//! @retval true The half-word was successfully read.
//! @retval false The specified memory location was not backed by the host.
bool Hardware::readHalfWord(uint32_t logicalAddr, uint16_t &result)
{
    bool isRead = false;
    const uint8_t *hostAddr;
    uint32_t hostSize;

    if (tryDecodeReadAddress(logicalAddr & ~1, hostAddr, hostSize))
    {
        result = *reinterpret_cast<const uint16_t *>(hostAddr);
        isRead = true;
    }
    else
    {
        uint32_t wordAddr = logicalAddr & ~3;
        const GuestMMIO *region = _resources->queryReadAddressMap(wordAddr);

        if (region != nullptr)
        {
            uint32_t word = region->Read(region->Context,
                                         wordAddr - region->GuestBaseAddr);

            word >>= (logicalAddr & 2) << 3; // Shift by 16 or 0 before casting.
            result = static_cast<uint16_t>(word);

            isRead = true;
        }
    }

    return isRead;
}

//! @brief Reads a 32-bit word from the guest memory map.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! read from.
//! @param[out] result Receives the word read from the memory map.
//! @retval true The word was successfully read.
//! @retval false The specified memory location was not backed by the host.
bool Hardware::readWord(uint32_t logicalAddr, uint32_t &result)
{
    bool isRead = false;
    const uint8_t *hostAddr;
    uint32_t hostSize;
    uint32_t effectiveAddr = logicalAddr & ~3;

    if (tryDecodeReadAddress(effectiveAddr, hostAddr, hostSize))
    {
        result = *reinterpret_cast<const uint32_t *>(hostAddr);
        isRead = true;
    }
    else
    {
        const GuestMMIO *region = _resources->queryReadAddressMap(effectiveAddr);

        if (region != nullptr)
        {
            result = region->Read(region->Context,
                                  effectiveAddr - region->GuestBaseAddr);
            isRead = true;
        }
    }

    return isRead;
}

//! @brief Reads a run of 32-bit words from the guest memory map.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! read from.
//! @param[out] results Receives the words read from the memory map.
//! @param[in] count The count of words to read.
//! @retval true The first word was read successfully.
//! @retval false Reading the first word from memory triggered an abort signal.
bool Hardware::readWords(uint32_t logicalAddr, uint32_t *results, uint8_t count)
{
    const uint8_t *hostAddr;
    uint32_t hostSize;
    uint32_t guestAddr = logicalAddr & ~3;
    uint8_t wordsRead = 0;
    bool firstWordRead = false;

    while (wordsRead < count)
    {
        bool isOK = false;
        uint32_t wordsToRead;

        // TODO: Use memory protection to decide whether an abort signal
        // was asserted.
        isOK = true;

        if (tryDecodeReadAddress(guestAddr, hostAddr, hostSize))
        {
            wordsToRead = std::min(hostSize / 4,
                                   static_cast<uint32_t>(count - wordsRead));

            // TODO: Use of STL to copy the values, this maybe too slow in debug.
            std::copy_n(reinterpret_cast<const uint32_t *>(hostAddr),
                        wordsToRead, results + wordsRead);
        }
        else
        {
            // Determine if the address is in memory mapped I/O.
            uint32_t nextAddr;
            const GuestMMIO *region = _resources->queryReadAddressMap(guestAddr,
                                                                      nextAddr);

            if (region == nullptr)
            {
                // There is nothing mapped at the specified address.
                uint32_t gapSize = nextAddr - guestAddr;

                wordsToRead = std::min(gapSize / 4,
                                       static_cast<uint32_t>(count - wordsRead));

                // For testing purposes, fill un-read words with a fixed pattern.
                std::fill_n(results + wordsRead, wordsToRead, 0xDFDFDFDF);
            }
            else
            {
                uint32_t offset = guestAddr - region->GuestBaseAddr;
                wordsToRead = std::min((region->Size - offset) / 4,
                                       static_cast<uint32_t>(count - wordsRead));

                for (uint32_t i = 0; i < wordsToRead; ++i)
                {
                    results[wordsRead + i] = region->Read(region->Context,
                                                          offset + (i * 4));
                }
            }
        }

        if (wordsRead == 0)
        {
            firstWordRead = isOK;
        }

        wordsRead += static_cast<uint8_t>(wordsToRead);
        guestAddr += wordsToRead * 4;
    }

    return firstWordRead;
}

//! @brief Attempts to atomically exchange a byte with memory.
//! @param[in] logicalAddr The logical address of the byte to read and update.
//! @param[in,out] value Contains the byte to write and receives the byte
//! at the specified location in memory.
//! @retval true The byte was successfully exchanged.
//! @retval false An abort signal was raised.
bool Hardware::exchangeByte(uint32_t logicalAddr, uint8_t &value)
{
    uint8_t *hostAddr;
    uint32_t hostSize;

    // TODO: Set isExchanged based on memory protection.
    bool isExchanged = true;

    if (tryDecodeWriteAddress(logicalAddr, hostAddr, hostSize))
    {
        // TODO: Perform a truly atomic exchange?
        uint8_t readValue = *hostAddr;
        *hostAddr = value;

        value = readValue;
    }

    return isExchanged;
}

//! @brief Attempts to atomically exchange a 32-bit word with memory.
//! @param[in] logicalAddr The logical address of the byte to read and update.
//! @param[in,out] value Contains the word to write and receives the word
//! at the specified location in memory.
//! @retval true The word was successfully exchanged.
//! @retval false An abort signal was raised.
bool Hardware::exchangeWord(uint32_t logicalAddr, uint32_t &value)
{
    uint8_t *hostAddr;
    uint32_t hostSize;
    uint32_t effectiveAddr = logicalAddr & ~3u;

    // TODO: Set isExchanged based on memory protection.
    bool isExchanged = true;

    if (tryDecodeWriteAddress(effectiveAddr, hostAddr, hostSize))
    {
        // TODO: Perform a truly atomic exchange?
        uint32_t *target = reinterpret_cast<uint32_t *>(hostAddr);
        uint32_t readValue = *target;
        *target = value;

        value = readValue;
    }

    return isExchanged;
}

//! @brief Re-fills the instruction pipeline.
//! @param[in] logicalAddr The logical address in the guest memory map to
//! read the instructions from.
//! @param[out] pipeline Receives the instruction words read from the memory map.
//! @return Bits indicating which words were successfully fetched.
uint8_t Hardware::fetchInstructions(uint32_t logicalAddr, uint32_t *pipeline)
{
    static const uint8_t count = 2;
    const uint8_t *hostAddr;
    uint32_t hostSize;
    uint32_t guestAddr = logicalAddr & ~3;
    uint8_t wordsRead = 0;
    uint8_t resultMask = 0;

    while ((wordsRead < count) &&
           tryDecodeReadAddress(guestAddr, hostAddr, hostSize))
    {
        uint32_t wordsToRead = std::min(hostSize / 4,
                                        static_cast<uint32_t>(count - wordsRead));

        // TODO: Use STL to copy the values, this maybe too slow in debug.
        std::copy_n(reinterpret_cast<const uint32_t *>(hostAddr),
                    wordsToRead, pipeline + wordsRead);
        uint8_t mask = (1 << wordsToRead) - 1;

        resultMask |= mask << wordsRead;
        wordsRead += static_cast<uint8_t>(wordsToRead);
        guestAddr += wordsToRead * 4;
    }

    return resultMask;
}

//! @brief Copies bytes from a range of addresses in guest memory without
//! affecting the state of the guest machine.
//! @param[in] logicalAddr The logical address of the first byte in the guest
//! address space to copy.
//! @param[out] data The buffer to receive the copied data.
//! @param[in] byteCount The maximum number of bytes to copy.
//! @return The actual number of bytes copied.
uint32_t Hardware::rawReadLogicalMemory(uint32_t logicalAddr, void *data,
                                        uint32_t byteCount) const
{
    uint8_t *destination = reinterpret_cast<uint8_t *>(data);
    const uint8_t *hostAddr = nullptr;
    uint32_t runSize = 0;
    uint32_t bytesRead = 0;

    while ((bytesRead < byteCount) &&
           tryDecodeReadAddress(logicalAddr + bytesRead, hostAddr, runSize))
    {
        uint32_t bytesToRead = std::min(byteCount - bytesRead, byteCount);;
        std::memcpy(destination + bytesRead, hostAddr, bytesToRead);
        bytesRead += bytesToRead;
    }

    return bytesRead;
}

//! @brief Raises a debug interrupt in response to a bkpt instruction being executed.
void Hardware::raiseDebugIrq() { _irqState |= IS_DebugPending; }

//! @brief Clears a debug interrupt condition.
void Hardware::clearDebugIrq() { _irqState &= ~IS_DebugPending; }

//! @brief Raises a host interrupt in response to the emulator host wanting
//! execution to be interrupted.
void Hardware::raiseHostIrq() { _irqState |= IS_HostIrqPending; }

//! @brief Clears the host interrupt condition.
void Hardware::clearHostIrq() { _irqState &= ~IS_HostIrqPending; }

// Inherited from IIrqSink.
void Hardware::raiseIrq() { _irqState |= IS_IrqPending; }

// Inherited from IIrqSink.
void Hardware::clearIrq() { _irqState &= ~IS_IrqPending; }

// Inherited from IIrqSink.
void Hardware::raiseFirq() { _irqState |= IS_FirqPending; }

// Inherited from IIrqSink.
void Hardware::clearFirq() { _irqState &= ~IS_FirqPending; }

//! @brief Raises an interrupt without calling a virtual function.
void Hardware::raiseIrqInternal() { _irqState |= IS_IrqPending; }

//! @brief Mutes any pending interrupt without calling a virtual function.
void Hardware::clearIrqInternal() { _irqState &= ~IS_IrqPending; }

//! @brief Raises a fast interrupt without calling a virtual function.
void Hardware::raiseFirqInternal() { _irqState |= IS_FirqPending; }

//! @brief Mutes any pending fast interrupt without calling a virtual function.
void Hardware::clearFirqInternal() { _irqState &= ~IS_FirqPending; }

//! @brief Determines if a physical guest address is valid.
//! @param[in] physicalAddr The guest address to validate.
//! @retval true The address is valid.
//! @retval false A read or write to the specified address should raise
//! an address exception.
bool Hardware::isValidAddress(uint32_t physicalAddr) noexcept
{
    return static_cast<uint8_t>(physicalAddr >> 26) == 0;
}

//! @brief Calculates the host memory address from which data can be read which
//! corresponds to a specified guest memory location.
//! @param[in] addr The guest memory address to decode.
//! @param[out] hostAddr Receives the address of the corresponding location in
//! host memory.
//! @param[out] runSize Receives the maximum count of bytes which can be
//! read if mapped host memory was found.
//! @retval true The guest memory address maps to a valid host memory location.
//! @retval false The guest memory address doesn't map to any host memory.
bool Hardware::tryDecodeReadAddress(uint32_t addr, const uint8_t *&hostAddr,
                                    uint32_t &runSize) const
{
    bool canDecode = false;

    if (_resources)
    {
        if ((addr >= PhysicalRamBase) &&
            ((addr - PhysicalRamBase) < _resources->getRamSize()))
        {
            // The RAM is located somewhere in the middle of the physical
            // address map.
            uint32_t offset = addr - PhysicalRamBase;
            hostAddr = _resources->getRam() + offset;
            runSize = _resources->getRamSize() - offset;
            canDecode = true;
        }
        else if (addr < _resources->getMainRomSize())
        {
            // The ROM is at the bottom of the physical address map.
            uint32_t offset = addr;
            hostAddr = _resources->getMainRom() + offset;
            runSize = _resources->getMainRomSize() - offset;
            canDecode = true;
        }
        else
        {
            // Replicate the ROM at the top of the 26-bit address space.
            uint32_t highRomStart = 0x4000000 - _resources->getMainRomSize();

            if ((addr >= highRomStart) && (addr < 0x4000000))
            {
                uint32_t offset = addr - highRomStart;
                hostAddr = _resources->getMainRom() + offset;
                runSize = 0x4000000 - addr;
                canDecode = true;
            }
        }
    }

    return canDecode;
}

//! @brief Calculates the host memory address to which data can be written which
//! corresponds to a specified guest memory location.
//! @param[in] addr The guest memory address to decode.
//! @param[out] hostAddr Receives the address of the corresponding location in
//! host memory.
//! @param[out] runSize Receives the maximum count of bytes which can be
//! read if mapped host memory was found.
//! @retval true The guest memory address maps to a valid host memory location.
//! @retval false The guest memory address doesn't map to any host memory.
bool Hardware::tryDecodeWriteAddress(uint32_t addr, uint8_t *&hostAddr,
                                     uint32_t &runSize) const
{
    // Setup a 64 MB address space, even if most of it is unpopulated.
    bool canDecode = false;

    if (_resources)
    {
        if ((addr >= PhysicalRamBase) &&
            ((addr - PhysicalRamBase) < _resources->getRamSize()))
        {
            uint32_t offset = addr - PhysicalRamBase;
            hostAddr = _resources->getRam() + offset;
            runSize = _resources->getRamSize() - offset;
            canDecode = true;
        }
    }

    return canDecode;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


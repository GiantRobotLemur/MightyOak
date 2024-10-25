//! @file ArmEmu/ArmSystem.cpp
//! @brief The definition of an object representing an emulated ARM-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"

#include "ArmEmu/ArmSystem.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// IArmSystemDeleter Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Disposes of a dynamically allocated IArmSystem implementation.
//! @param[in] sys A pointer to the object to dispose of.
void IArmSystemDeleter::operator()(IArmSystem *sys) const
{
    Ag::safeDelete(sys);
}

////////////////////////////////////////////////////////////////////////////////
// ProcessorModeInfo Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to use as a look-up key.
//! @param[in] id The processor mode being looked up.
ProcessorModeInfo::ProcessorModeInfo(ProcessorMode id) :
    EnumSymbol<ProcessorMode>(id),
    _minArchVersion(0)
{
}

//! @brief Constructs a description of an ARM processor mode.
//! @param[in] id The identifier of the processor mode.
//! @param[in] symbol The internal symbol definition as text.
//! @param[in] displayName The symbol as text to be displayed to the user.//! @param[in] description A description of the meaning of the symbol
//! which can be displayed to the user.
//! @note All strings should be static and UTF-8 encoded.
ProcessorModeInfo::ProcessorModeInfo(ProcessorMode id, const char *symbol,
                                     const char *displayName,
                                     const char *description,
                                     uint8_t minArchVersion) :
    EnumSymbol<ProcessorMode>(id, symbol, displayName, description),
    _minArchVersion(minArchVersion)
{
}

//! @brief Determines if the mode operates with 26-bit addressing.
bool ProcessorModeInfo::is26Bit() const
{
    return _minArchVersion < 3;
}

//! @brief Gets the minimum ARM architecture in which the mode is valid.
uint8_t ProcessorModeInfo::getMinimumArchitectureVersion() const
{
    return _minArchVersion;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets static metadata for the ProcessorMode enumeration.
const ProcessorModeEnumInfo &getProcessorModeType()
{
    static const ProcessorModeEnumInfo instance({
        ProcessorModeInfo(ProcessorMode::User26, "USR26", "User Mode (26-bit)", nullptr, 2),
        ProcessorModeInfo(ProcessorMode::FastIrq26, "FIQ26", "Fast Interrupt Mode (26-bit)", nullptr, 2),
        ProcessorModeInfo(ProcessorMode::Irq26, "IRQ26", "Interrupt Mode (26-bit)", nullptr, 2),
        ProcessorModeInfo(ProcessorMode::Svc26, "SVC26", "Supervisor Mode (26-bit)", nullptr, 2),
        ProcessorModeInfo(ProcessorMode::User32, "USR32", "User Mode (32-bit)", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::FastIrq32, "FIQ32", "Fast Interrupt Mode (32-bit)", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::Irq32, "IRQ32", "Interrupt Mode (32-bit)", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::Svc32, "SVC32", "Supervisor Mode (32-bit)", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::Abort, "ABT", "Abort Mode", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::Undefined, "UND", "Undefined Mode", nullptr, 3), // ARMv3+
        ProcessorModeInfo(ProcessorMode::System, "SYS", "System Mode", nullptr, 4), // ARMv4+
    });

    return instance;
}

//! @brief Attempts to read an array of bytes from a set of physicalAddr addresses
//! within an emulated ARM-based system.
//! @param[in] sys The emulated system to write to.
//! @param[in] physicalAddr The physical address of the first bytes to read.
//! @param[in] buffer The buffer to receive the bytes read.
//! @param[in] length The count of bytes to read.
//! @return The count of bytes successfully read.
//! @note Reads from memory mapped I/O should be at 4-byte aligned addresses,
//! even when quantities smaller than 4 bytes are to be read.
uint32_t readFromPhysicalAddress(IArmSystem *sys, uint32_t physicalAddr,
                                 void *buffer, uint32_t length)
{
    const AddressMap &readMap = sys->getReadAddresses();
    uint8_t *target = reinterpret_cast<uint8_t *>(buffer);
    uint32_t bytesRead = 0;
    uint32_t physAddr = physicalAddr;

    while (bytesRead < length)
    {
        IAddressRegionPtr region;
        uint32_t baseAddr = physAddr & ~3;
        uint32_t addrOffset = physAddr & 3;
        uint32_t mappingOffset, mappingLength;

        if (readMap.tryFindRegion(baseAddr, region, mappingOffset, mappingLength))
        {
            auto regionType = region->getType();

            // Adjust for alignment.
            mappingLength -= addrOffset;
            mappingOffset += addrOffset;
            uint32_t bytesToRead = std::min(mappingLength, length - bytesRead);

            if (regionType == RegionType::HostBlock)
            {
                uint8_t *block = reinterpret_cast<uint8_t *>(static_cast<IHostBlockPtr>(region)->getHostAddress());

                std::memcpy(target + bytesRead, block + mappingOffset,
                            bytesToRead);

                bytesRead += bytesToRead;
            }
            else if (regionType == RegionType::MMIO)
            {
                if (mappingOffset & 3)
                {
                    throw Ag::OperationException("Reading from memory mapped I/O "
                                                 "at a non-aligned address.");
                }

                IMMIOBlockPtr mmio = static_cast<IMMIOBlockPtr>(region);
                uint32_t wordsToRead = bytesToRead >> 2;
                uint32_t *wordTarget = reinterpret_cast<uint32_t *>(target + bytesRead);

                for (uint32_t wordsRead = 0; wordsRead < wordsToRead; ++wordsRead)
                {
                    uint32_t word = mmio->read(mappingOffset + (wordsRead * 4));
                    wordTarget[wordsRead] = word;
                }

                uint32_t wordBytesRead = wordsToRead * 4;
                bytesRead += wordBytesRead;
                bytesToRead -= wordBytesRead;

                if (bytesToRead > 0)
                {
                    // Read a final word.
                    uint32_t word = mmio->read(mappingOffset + wordBytesRead);

                    // Copy the last 1-3 bytes.
                    std::copy_n(reinterpret_cast<uint8_t *>(&word),
                                bytesToRead, target + bytesRead);

                    bytesRead += bytesToRead;
                }
            }

            // Update the physical address so that we might read more.
            physAddr += bytesToRead;
        }
        else
        {
            // The address is not mapped.
            break;
        }
    }

    return bytesRead;
}

//! @brief Attempts to write bytes to a continuous run of physical addresses
//! within an emulated ARM-based system
//! @param[in] sys The emulated system to write to.
//! @param[in] physicalAddr The physical address of the first byte to write.
//! @param[in] buffer The buffer containing the bytes to write.
//! @param[in] length The count of bytes to be written.
//! @param[in] useReadMap True to look-up the address to write in the read
//! mapping, useful when writing to ROM.
//! @returns The count of bytes successfully written.
//! @note Writes to memory mapped I/O should be at 4-byte aligned addresses,
//! even when quantities smaller than 4 bytes are to be written.
uint32_t writeToPhysicalAddress(IArmSystem *sys, uint32_t physicalAddr,
                            const void *buffer, uint32_t length,
                            bool useReadMap /*= false*/)
{
    const AddressMap &writeMap = useReadMap ? sys->getReadAddresses() :
                                              sys->getWriteAddresses();
    const uint8_t *target = reinterpret_cast<const uint8_t *>(buffer);
    uint32_t bytesWritten = 0;
    uint32_t physAddr = physicalAddr;

    while (bytesWritten < length)
    {
        IAddressRegionPtr region;
        uint32_t baseAddr = physAddr & ~3;
        uint32_t addrOffset = physAddr & 3;
        uint32_t mappingOffset, mappingLength;

        if (writeMap.tryFindRegion(baseAddr, region, mappingOffset, mappingLength))
        {
            auto regionType = region->getType();

            // Adjust for alignment.
            mappingLength -= addrOffset;
            mappingOffset += addrOffset;
            uint32_t bytesToWrite = std::min(mappingLength, length - bytesWritten);

            if (regionType == RegionType::HostBlock)
            {
                uint8_t *block = reinterpret_cast<uint8_t *>(static_cast<IHostBlockPtr>(region)->getHostAddress());

                std::memcpy(block + mappingOffset, target + bytesWritten,
                            bytesToWrite);

                bytesWritten += bytesToWrite;
            }
            else if (regionType == RegionType::MMIO)
            {
                if (mappingOffset & 3)
                {
                    throw Ag::OperationException("Writing to memory mapped I/O "
                                                 "at a non-aligned address.");
                }

                IMMIOBlockPtr mmio = static_cast<IMMIOBlockPtr>(region);
                uint32_t wordsToWrite = bytesToWrite >> 2;
                const uint32_t *wordTarget = reinterpret_cast<const uint32_t *>(target + bytesWritten);

                for (uint32_t wordsWritten = 0; wordsWritten < wordsToWrite; ++wordsWritten)
                {
                    mmio->write(mappingOffset + (wordsWritten * 4),
                                wordTarget[wordsWritten]);
                }

                uint32_t wordBytesWritten = wordsToWrite * 4;
                bytesWritten += wordBytesWritten;
                bytesToWrite -= wordBytesWritten;

                if (bytesToWrite > 0)
                {
                    uint32_t lastWord = wordTarget[wordsToWrite];

                    // Mask superfluous bits.
                    uint32_t mask = (static_cast<uint32_t>(1) << bytesToWrite) - 1;

                    // Write a final word.
                    mmio->write(mappingOffset + wordBytesWritten, lastWord & mask);

                    bytesWritten += bytesToWrite;
                }
            }

            // Update the physical address so that we might write more.
            physAddr += bytesToWrite;
        }
        else
        {
            // The address is not mapped.
            break;
        }
    }

    return bytesWritten;
}

//! @brief Attempts to read an array of bytes from a set of logical addresses
//! within an emulated ARM-based system.
//! @param[in] sys The emulated system to write to.
//! @param[in] logicalAddr The logical address of the first bytes to read.
//! @param[in] buffer The buffer to receive the bytes read.
//! @param[in] length The count of bytes to read.
//! @return The count of bytes successfully read.
//! @note Reads from memory mapped I/O should be at 4-byte aligned addresses,
//! even when quantities smaller than 4 bytes are to be read.
uint32_t readFromLogicalAddress(IArmSystem *sys, uint32_t logicalAddr,
                                void *buffer, uint32_t length)
{
    uint8_t *target = reinterpret_cast<uint8_t *>(buffer);
    uint32_t bytesRead = 0;
    uint32_t logAddr = logicalAddr;

    while (bytesRead < length)
    {
        PageMapping mapping;

        sys->logicalToPhysicalAddress(logAddr, mapping);

        if (mapping.PageSize > 0)
        {
            uint32_t pageOffset = logAddr - mapping.VirtualBaseAddr;
            uint32_t bytesToRead = std::min(mapping.PageSize - pageOffset,
                                            length - bytesRead);

            if (mapping.Access & PageMapping::IsPresent)
            {
                // The page is present, copy the data.
                readFromPhysicalAddress(sys, mapping.PageBaseAddr + pageOffset,
                                        target + bytesRead, bytesToRead);
            }
            else
            {
                // The page is not present, fill with zeros.
                std::memset(target + bytesRead, 0, bytesToRead);
            }

            // Move past the block.
            bytesRead += bytesToRead;
            logAddr += bytesToRead;
        }
        else
        {
            break;
        }
    }

    return bytesRead;
}

//! @brief Attempts to write bytes to a continuous run of logical addresses
//! within an emulated ARM-based system
//! @param[in] sys The emulated system to write to.
//! @param[in] logicalAddr The logical address of the first byte to write.
//! @param[in] buffer The buffer containing the bytes to write.
//! @param[in] length The count of bytes to be written.
//! @param[in] useReadMap True to look-up the address to write in the read
//! mapping, useful when writing to ROM.
//! @returns The count of bytes successfully written.
//! @note Writes to memory mapped I/O should be at 4-byte aligned addresses,
//! even when quantities smaller than 4 bytes are to be written.
uint32_t writeToLogicalAddress(IArmSystem *sys, uint32_t logicalAddr,
                               const void *buffer, uint32_t length,
                               bool useReadMap /*= false*/)
{
    const uint8_t *target = reinterpret_cast<const uint8_t *>(buffer);
    uint32_t bytesWritten = 0;
    uint32_t logAddr = logicalAddr;

    while (bytesWritten < length)
    {
        PageMapping mapping;

        sys->logicalToPhysicalAddress(logAddr, mapping);

        if (mapping.PageSize > 0)
        {
            uint32_t pageOffset = logAddr - mapping.VirtualBaseAddr;
            uint32_t bytesToWrite = std::min(mapping.PageSize - pageOffset,
                                             length - bytesWritten);

            if (mapping.Access & PageMapping::IsPresent)
            {
                // The page is present, copy the data.
                writeToPhysicalAddress(sys, mapping.PageBaseAddr + pageOffset,
                                       target + bytesWritten, bytesToWrite,
                                       useReadMap);
            }

            // Move past the block.
            bytesWritten += bytesToWrite;
            logAddr += bytesToWrite;
        }
        else
        {
            break;
        }
    }

    return bytesWritten;
}

//! @brief Attempts to find a memory mapped device within an emulated system by
//! its name.
//! @param[in] sys A pointer to the system to find the device within.
//! @param[in] name The name of the device to find.
//! @param[out] device Receives a pointer to the device, if a matching one
//! was found.
//! @retval true A matching device was found and its pointer returned.
//! @retval false The system did not contain a device with the specified name.
bool tryFindDeviceByName(IArmSystem *sys, const std::string_view &name,
                         IMMIOBlockPtr &device)
{
    if ((sys != nullptr) && (name.empty() == false))
    {
        Ag::String key(name);

        // Use linear search as this function should not be called in
        // time-critical situations.
        for (uint8_t i = 0; i < 2; ++i)
        {
            const AddressMap &map = (i == 0) ? sys->getReadAddresses() :
                                               sys->getWriteAddresses();

            for (const auto &mapping : map.getMappings())
            {
                device = dynamic_cast<IMMIOBlockPtr>(mapping.Region);

                if ((device != nullptr) && (device->getName() == key))
                {
                    return true;
                }
            }
        }
    }

    device = nullptr;
    return false;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


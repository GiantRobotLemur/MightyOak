//! @file ArmEmu/ArmSystemBuilder.cpp
//! @brief The definition of an object used to incrementally construct another
//! object representing an emulated ARM-based system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/Variant.hpp"
#include "ArmEmu/ArmSystemBuilder.hpp"

#include "ArmSystem.inl"
#include "SystemConfigurations.inl"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
Ag::String makePlatformProcessorError(const Options &options)
{
    return Ag::String::format(Ag::FormatInfo::getDisplay(),
                              "The combination of an {0}-based platform and "
                              "a {1} processor is not supported.",
                              { getSystemModelType().toDisplayName(options.getHardwareArchitecture()),
                                getProcessorModelType().toDisplayName(options.getProcessorVariant()) });
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// ArmSystemBuilder Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object used to incrementally construct an IArmSystem
//! implementation.
ArmSystemBuilder::ArmSystemBuilder()
{
    reset(Options());
}

//! @brief Constructs an object used to incrementally construct an IArmSystem
//! implementation.
//! @param[in] baseOptions The basic system parameters used to select the correct
//! IArmSystem implementation to instantiate.
ArmSystemBuilder::ArmSystemBuilder(const Options &baseOptions)
{
    reset(baseOptions);
}

//! @brief Adds a device which the emulated system being built will take
//! ownership of.
//! @param[in] device A pointer to the device implementation to transfer
//! to the emulated system at creation.
void ArmSystemBuilder::addDevice(IHardwareDeviceUPtr &&device)
{
    if (device)
    {
    // See if we already have ownership of the device.
        auto pos = std::find(_devices.begin(), _devices.end(), device);

        if (pos == _devices.end())
        {
            // The device is not currently in the list, add it.
            _devices.emplace_back(std::move(device));
        }
    }
}

//! @brief Attempts to add a region into the physical memory map of the system
//! being constructed.
//! @param[in] region The object describing the memory region.
//! @param[in] baseAddr The base address of the region, must be 4-byte aligned.
//! @param[in] access The access the emulated system has to the mapped region.
//! @details It is expected that the emulated system already has ownership of the
//! device being mapped.
void ArmSystemBuilder::addMapping(IAddressRegionPtr region, uint32_t baseAddr,
                                  MemoryAccess access)
{
    if ((region == nullptr) || (region->getSize() & 3))
    {
        // The region description was invalid had a size which wasn't
        // a multiple of 4 bytes.
        throw Ag::ArgumentException("region");
    }
    else if (baseAddr & 3)
    {
        // The base address wasn't a whole multiple of 4 bytes.
        throw Ag::ArgumentException("baseAddr");
    }
    else if (access == MemoryAccess::None)
    {
        // The region could not be accessed.
        throw Ag::ArgumentException("access");
    }

    bool isReadable = (access == MemoryAccess::ReadOnly) ||
                      (access == MemoryAccess::ReadWrite);
    bool isWritable = (access == MemoryAccess::WriteOnly) ||
                      (access == MemoryAccess::ReadWrite);

    if (isReadable || isWritable)
    {
        if (isReadable &&
            (_readMap.tryInsert(baseAddr, region) == false))
        {
            throw Ag::OperationException(
                "The mapped readable memory region overlapped with "
                "previously registered regions.");
        }

        if (isWritable &&
            (_writeMap.tryInsert(baseAddr, region) == false))
        {
            throw Ag::OperationException(
                "The mapped writeable memory region overlapped with "
                "previously registered regions.");
        }
    }
}

//! @brief Attempts to add a region into the physical memory map of the system
//! being constructed and takes ownership of the underlying device.
//! @param[in] region The object describing the memory region.
//! @param[in] baseAddr The base address of the region, must be 4-byte aligned.
//! @param[in] access The access the emulated system has to the mapped region.
//! @details It is expected that the emulated system already has ownership of the
//! device being mapped.
void ArmSystemBuilder::addMapping(IAddressRegionUPtr &&region, uint32_t baseAddr,
                                  MemoryAccess access)
{
    // Add the device to the memory map.
    addMapping(region.get(), baseAddr, access);

    // If no exception was thrown, take ownership of the device.
    addDevice(std::move(region));
}

//! @brief Resets the state of the object back to an initial set of options.
//! @param[in] baseOptions The initial options used to instantiate the correct
//! IArmSystem implementation.
void ArmSystemBuilder::reset(const Options &baseOptions)
{
    _baseOptions = baseOptions;
    _readMap.clear();
    _writeMap.clear();
    _devices.clear();
}

//! @brief Instantiates an appropriate implementation of IArmSystem based on
//! the current state of the object.
//! @throws Ag::OperationException Thrown if the combination of parameters used
//! to define the system is invalid.
//! @throws Ag::NotSupportedException If an option was selected which isn't
//! currently supported by the existing IArmSystem implementations.
IArmSystemUPtr ArmSystemBuilder::createSystem()
{
    Ag::String error;
    IArmSystem *sys = nullptr;

    if (_baseOptions.validate(error))
    {
        switch (_baseOptions.getHardwareArchitecture())
        {
        case SystemModel::TestBed:
            if (_baseOptions.getProcessorVariant() == ProcessorModel::ARM2)
            {
                // A test system with an ARM 2 processor.
                sys = new ArmSystem<ArmV2TestSystemTraits>(_baseOptions,
                                                           std::move(_devices),
                                                           _readMap, _writeMap);
            }
            else if (_baseOptions.getProcessorVariant() == ProcessorModel::ARM3)
            {
                // A test system with an ARM 3 processor.
                sys = new ArmSystem<ArmV2aTestSystemTraits>(_baseOptions,
                                                            std::move(_devices),
                                                            _readMap, _writeMap);
            }
            else
            {
                error = makePlatformProcessorError(_baseOptions);
            }

            if (sys != nullptr)
            {
                // TODO: Load system ROM preset.
            }
            break;

        case SystemModel::Archimedies:
        case SystemModel::ASeries:
            if (_baseOptions.getProcessorVariant() == ProcessorModel::ARM2)
            {
                sys = new ArmSystem<ArmV2MemcSystemTraits>(_baseOptions,
                                                           std::move(_devices),
                                                           _readMap, _writeMap);
            }
            else if (_baseOptions.getProcessorVariant() == ProcessorModel::ARM3)
            {
                sys = new ArmSystem<ArmV2aMemcSystemTraits>(_baseOptions,
                                                            std::move(_devices),
                                                            _readMap, _writeMap);
            }
            else
            {
                error = makePlatformProcessorError(_baseOptions);
            }

            if (sys != nullptr)
            {
                // TODO: Load system ROM preset.
            }
            break;

        default:
            error = Ag::String::format(Ag::FormatInfo::getDisplay(),
                                       "A {0}-based system is not currently supported.",
                                       { getSystemModelType().toDisplayName(_baseOptions.getHardwareArchitecture()) });
            break;
        }
    }

    if (sys == nullptr)
    {
        if (error.isEmpty())
        {
            error = "The specified combination of configuration options cannot be emulated.";
        }

        throw Ag::NotSupportedException(error.getUtf8Bytes());
    }
    else
    {
        // Reset the object state.
        _readMap.clear();
        _writeMap.clear();

        return IArmSystemUPtr(sys);
    }
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


//! @file ArmEmu/Test/LoggerDevice.hpp
//! @brief The declaration of a device within an emulated system which can
//! log output to the host.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_LOGGER_DEVICE_HPP__
#define __ARM_EMU_LOGGER_DEVICE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/AddressMap.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief 
class LoggerDevice : public IMMIOBlock
{
public:
    // Construction/Destruction
    LoggerDevice();
    virtual ~LoggerDevice() = default;

    // Inherited from IAddressRegion
    virtual RegionType getType() const override;
    virtual Ag::string_cref_t getName() const override;
    virtual Ag::string_cref_t getDescription() const override;
    virtual uint32_t getSize() const override;

    // Inherited from IMMIOBlock.
    virtual uint32_t read(uint32_t offset) override;
    virtual void write(uint32_t offset, uint32_t value) override;
    virtual void connect(SystemContext &context) override;
private:

    // Internal Fields
    SystemContextPtr _parentContext;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/IOC.cpp
//! @brief The definition of an object which emulates the function of the
//! VL86C410 IOC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/IOC.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// IOC Member Definitions
////////////////////////////////////////////////////////////////////////////////
IOC::IOC(MemcHardware &parent) :
    _parent(parent),
    _context(nullptr)
{
}

// Inherited from IAddressRegion.
RegionType IOC::getType() const
{
    return RegionType::MMIO;
}

// Inherited from IAddressRegion.
Ag::string_cref_t IOC::getName() const
{
    static const Ag::String name("IOC");

    return name;
}

// Inherited from IAddressRegion.
Ag::string_cref_t IOC::getDescription() const
{
    static const Ag::String description("The VL86C410 RISC I/O Controller");

    return description;
}

// Inherited from IAddressRegion.
uint32_t IOC::getSize() const
{
    return 0x80;
}

// Inherited from IMMIOBlock.
uint32_t IOC::read(uint32_t /*offset*/)
{
    // TODO: Implement MMIO registers.
    return _context->getFuzz();
}

// Inherited from IMMIOBlock.
void IOC::write(uint32_t /*offset*/, uint32_t /*value*/)
{
    // TODO: Implement MMIO registers.
}

// Inherited from IMMIOBlock.
void IOC::connect(const ConnectionContext &context)
{
    // Connect to the rest of the emulated system.
    _context = context.getInteropContext();
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


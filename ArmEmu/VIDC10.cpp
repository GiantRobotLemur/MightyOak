//! @file ArmEmu/VIDC10.cpp
//! @brief The definition of an object which emulates the function of the
//! VL86C310 VIDC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/VIDC10.hpp"
#include "ArmEmu/SystemContext.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// VIDC10 Member Definitions
////////////////////////////////////////////////////////////////////////////////
VIDC10::VIDC10(MemcHardware &parent) :
    _parent(parent),
    _context(nullptr)
{
}

// Inherited from IAddressRegion.
RegionType VIDC10::getType() const
{
    return RegionType::MMIO;
}

// Inherited from IAddressRegion.
Ag::string_cref_t VIDC10::getName() const
{
    static const Ag::String name("VIDC10");

    return name;
}

// Inherited from IAddressRegion.
Ag::string_cref_t VIDC10::getDescription() const
{
    static const Ag::String description("The VL86C310 RISC Video Controller");

    return description;
}

// Inherited from IAddressRegion.
uint32_t VIDC10::getSize() const
{
    return 0x200000;
}

// Inherited from IMMIOBlock.
uint32_t VIDC10::read(uint32_t /*offset*/)
{
    // VIDC registers are write-only, so this should ever be called.
    return _context->getFuzz();
}

// Inherited from IMMIOBlock.
void VIDC10::write(uint32_t /*offset*/, uint32_t /*value*/)
{
    // TODO: Implement MMIO registers.
}

// Inherited from IMMIOBlock.
void VIDC10::connect(const ConnectionContext &context)
{
    // Connect to the rest of the emulated system.
    _context = context.getInteropContext();
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


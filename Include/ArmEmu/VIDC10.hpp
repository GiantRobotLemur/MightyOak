//! @file ArmEmu/name_here.hpp
//! @brief The declaration of an object which emulates the function of the
//! VL86C310 VIDC part.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_VIDC10_HPP__
#define __ARM_EMU_VIDC10_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AddressMap.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class MemcHardware;

//! @brief An object which emulates the function of the VL86C310 VIDC part.
class VIDC10 : public IMMIOBlock
{
public:
    // Construction/Destruction
    VIDC10(MemcHardware &parent);
    virtual ~VIDC10() = default;

    // Overrides
    virtual RegionType getType() const override;
    virtual Ag::string_cref_t getName() const override;
    virtual Ag::string_cref_t getDescription() const override;
    virtual uint32_t getSize() const override;

    virtual uint32_t read(uint32_t offset) override;
    virtual void write(uint32_t offset, uint32_t value) override;
    virtual void connect(const ConnectionContext &context) override;
private:
    // Internal Fields
    MemcHardware &_parent;
    SystemContext *_context;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

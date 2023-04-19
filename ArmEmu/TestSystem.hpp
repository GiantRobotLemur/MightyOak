//! @file ArmEmu/TestSystem.hpp
//! @brief The declaration of an implementation of IArmSystem used for testing.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_TEST_SYSTEM_HPP__
#define __ARM_EMU_TEST_SYSTEM_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu.hpp"

#include "InstructionPipeline.hpp"
#include "SystemResources.hpp"

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An implementation of IArmSystem used for testing.
class TestSystem : public IArmSystem
{
public:
    // Construction/Destruction
    TestSystem();
    virtual ~TestSystem() = default;

    // Accessors
    SystemResources &getResources();
    InstructionPipeline &getProcessor();

    // Operations

    // Overrides
    virtual ProcessorMode getMode() const override;
    virtual uint32_t getCoreRegister(CoreRegister id) const override;
    virtual void setCoreRegister(CoreRegister id, uint32_t value) override;
    virtual uint32_t readFromLogicalAddress(uint32_t logicalAddr, uint32_t length,
                                            void *buffer) const override;
    virtual void writeToLogicalAddress(uint32_t logicalAddr, uint32_t length,
                                       const void *buffer) override;

    virtual uint64_t run() override;
    virtual uint64_t runSingleStep() override;

private:
    // Internal Fields
    InstructionPipeline _processor;
    SystemResources _resources;
};

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

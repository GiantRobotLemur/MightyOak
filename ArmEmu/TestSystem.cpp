//! @file ArmEmu/TestSystem.cpp
//! @brief The definition of an implementation of IArmSystem used for testing.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "TestSystem.hpp"

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
// TestSystem Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an emulated system for testing purposes.
TestSystem::TestSystem() :
    _resources(&_processor)
{
    // Cross link the processor with its system resources.
    _processor.initialiseSystemResources(&_resources);
}

//! @brief Gets the resources associated with the emulated system.
SystemResources &TestSystem::getResources()
{
    return _resources;
}

//! @brief Gets the object representing the emulated ARM processor.
InstructionPipeline &TestSystem::getProcessor()
{
    return _processor;
}

// Inherited from IArmSystem.
void TestSystem::destroy()
{
    if (this != nullptr)
    {
        delete this;
    }
}

// Inherited from IArmSystem.
ProcessorMode TestSystem::getMode() const
{
    return _processor.getMode();
}

// Inherited from IArmSystem.
uint32_t TestSystem::getCoreRegister(CoreRegister id) const
{
    uint32_t value = 0;

    if (id <= CoreRegister::R15)
    {
        value = _processor.getRd(fromScalar<GeneralRegister>(toScalar(id)));
    }
    else if (id == CoreRegister::CPSR)
    {
        value = _processor.getPSR();
    }
    else if (id == CoreRegister::PC)
    {
        value = _processor.getPC();
    }

    return value;
}

// Inherited from IArmSystem.
uint32_t TestSystem::readFromLogicalAddress(uint32_t logicalAddr, uint32_t length,
                                            void *buffer) const
{
    return _processor.rawReadLogicalMemory(logicalAddr, buffer, length);
}

// Inherited from IArmSystem.
uint32_t TestSystem::run()
{
    return _processor.runPipeline(false);
}

// Inherited from IArmSystem.
uint32_t TestSystem::runSingleStep()
{
    return _processor.runPipeline(true);
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


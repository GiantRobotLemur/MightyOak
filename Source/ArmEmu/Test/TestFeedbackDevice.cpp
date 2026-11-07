//! @file ArmEmu/Test/TestFeedbackDevice.cpp
//! @brief The definition of an MMIO device allowing firmware running on an
//! emulated system to report test results back to the host test harness.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "TestFeedbackDevice.hpp"

#include "ArmEmu/SystemContext.hpp"
#include "ArmEmu/ArmSystem.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// TestFeedbackDevice Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a test feedback MMIO device with all registers zeroed.
TestFeedbackDevice::TestFeedbackDevice() :
    _parentContext(nullptr),
    _milestone(0),
    _result(0),
    _expected(0),
    _actual(0)
{
}

uint32_t TestFeedbackDevice::getMilestone() const { return _milestone; }
uint32_t TestFeedbackDevice::getResult() const { return _result; }
uint32_t TestFeedbackDevice::getExpected() const { return _expected; }
uint32_t TestFeedbackDevice::getActual() const { return _actual; }

//! @brief Resets all registers to zero.
void TestFeedbackDevice::reset()
{
    _milestone = 0;
    _result = 0;
    _expected = 0;
    _actual = 0;
}

// Inherited from IAddressRegion.
RegionType TestFeedbackDevice::getType() const { return RegionType::MMIO; }

// Inherited from IAddressRegion.
Ag::string_cref_t TestFeedbackDevice::getName() const
{
    static const Ag::String name("TestFeedbackDevice");

    return name;
}

// Inherited from IAddressRegion.
Ag::string_cref_t TestFeedbackDevice::getDescription() const
{
    static const Ag::String desc("MMIO device for firmware-to-harness test communication.");

    return desc;
}

// Inherited from IAddressRegion.
uint32_t TestFeedbackDevice::getSize() const
{
    return 16; // 4 registers x 4 bytes
}

// Inherited from IMMIOBlock.
uint32_t TestFeedbackDevice::read(uint32_t offset)
{
    switch (offset >> 2)
    {
    case 0: return _milestone;
    case 1: return _result;
    case 2: return _expected;
    case 3: return _actual;
    default: return 0;
    }
}

// Inherited from IMMIOBlock.
void TestFeedbackDevice::write(uint32_t offset, uint32_t value)
{
    switch (offset >> 2)
    {
    case 0: // Milestone
        _milestone = value;
        break;

    case 1: // Result — non-zero triggers host interrupt to halt emulation.
        _result = value;

        if (value != 0 && _parentContext != nullptr)
        {
            _parentContext->getSystem()->raiseHostInterrupt();
        }
        break;

    case 2: // Expected
        _expected = value;
        break;

    case 3: // Actual
        _actual = value;
        break;

    default:
        break;
    }
}

// Inherited from IHardwareDevice.
void TestFeedbackDevice::connect(SystemContext &context)
{
    _parentContext = &context;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

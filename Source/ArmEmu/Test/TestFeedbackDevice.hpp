//! @file ArmEmu/Test/TestFeedbackDevice.hpp
//! @brief The declaration of an MMIO device allowing firmware running on an
//! emulated system to report test results back to the host test harness.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_TEST_FEEDBACK_DEVICE_HPP__
#define __ARM_EMU_TEST_FEEDBACK_DEVICE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/AddressMap.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An MMIO device for firmware-to-harness communication during tests.
//! @details Mapped at a physical address in I/O space, this device provides
//! four 32-bit registers: Milestone, Result, Expected, and Actual.
//! Writing a non-zero value to Result triggers a host interrupt to halt
//! emulation, allowing the test harness to inspect failure details.
class TestFeedbackDevice : public IMMIOBlock
{
public:
    //! @brief The base physical address where this device should be mapped.
    static constexpr uint32_t BaseAddress = 0x3100000;

    // Construction/Destruction
    TestFeedbackDevice();
    virtual ~TestFeedbackDevice() = default;

    // Accessors
    uint32_t getMilestone() const;
    uint32_t getResult() const;
    uint32_t getExpected() const;
    uint32_t getActual() const;

    // Operations
    void reset();

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
    uint32_t _milestone;
    uint32_t _result;
    uint32_t _expected;
    uint32_t _actual;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

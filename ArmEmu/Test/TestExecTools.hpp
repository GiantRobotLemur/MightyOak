//! @file ArmEmu/TestExecTools.hpp
//! @brief The declaration of tools which allow execution of instructions on
//! various configurations of emulated system configurations for the purposes
//! of validation.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_TEST_EXEC_TOOLS_HPP__
#define __ARM_EMU_TEST_EXEC_TOOLS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "ArmEmu.hpp"

#include "ArmSystem.inl"
#include "SystemConfigurations.inl"
#include "TestConstraints.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
::testing::AssertionResult prepareTestSystem(IArmSystem *systemUnderTest,
                                             const std::string_view &source);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief A quick and dirty constraint adapter for the one system supported.
template<> struct ConstraintInterpretor<ArmV2TestSystemTraits>
{
    bool apply(ArmSystem<ArmV2TestSystemTraits> &target, const Constraint &constraint) const
    {
        bool isSet = false;
        uint32_t scratch;

        switch (constraint.Element)
        {
        case SystemElement::CoreRegister:
            if (constraint.ElementIndex < 16)
            {
                target.getRegisters().setRn(
                    Ag::forceFromScalar<GeneralRegister>(constraint.ElementIndex),
                    constraint.Value);
                isSet = true;
            }
            break;

        case SystemElement::PhysicalByte:
        case SystemElement::LogicalByte:
            isSet = target.getHardare().write(constraint.ElementIndex,
                                              static_cast<uint8_t>(constraint.Value));
            break;

        case SystemElement::PhysicalHalfWord:
        case SystemElement::LogicalHalfWord:
            isSet = target.getHardare().write(constraint.ElementIndex,
                                              static_cast<uint16_t>(constraint.Value));
            break;

        case SystemElement::PhysicalWord:
        case SystemElement::LogicalWord:
            isSet = target.getHardare().write(constraint.ElementIndex,
                                              constraint.Value);
            break;

        case SystemElement::SystemRegister:
            isSet = true;

            switch (Ag::fromScalar<SystemRegister>(constraint.ElementIndex))
            {
            case SystemRegister::PC:
                target.getRegisters().setPC(constraint.Value);
                break;

            case SystemRegister::CPSR:
                target.getRegisters().setPSR(constraint.Value);
                break;

            case SystemRegister::Status:
                scratch = target.getRegisters().getPSR();
                scratch &= ~PsrMask::Status;
                scratch |= constraint.Value << PsrShift::Status;
                target.getRegisters().setPSR(scratch);
                break;

            case SystemRegister::ProcessorMode:
                scratch = target.getRegisters().getPSR();
                scratch &= ~PsrMask26::ModeBits;
                scratch |= constraint.Value & PsrMask26::ModeBits;
                target.getRegisters().setPSR(scratch);
                break;

            case SystemRegister::IrqStatus:
            case SystemRegister::IrqMask:
            case SystemRegister::SPSR:
            default:
                isSet = false;
                break;
            }
            break;

        default:
            isSet = false;
            break;
        }

        return isSet;
    }

    bool extract(ArmSystem<ArmV2TestSystemTraits> &target, const Constraint &constraint, uint32_t &value)
    {
        bool isFound = false;
        uint8_t byteResult;
        uint16_t halfWordResult;
        value = 0;

        switch (constraint.Element)
        {
        case SystemElement::CoreRegister:
            if (constraint.ElementIndex < 16)
            {
                value = target.getRegisters().getRn(
                    Ag::forceFromScalar<GeneralRegister>(constraint.ElementIndex));
                isFound = true;
            }
            break;

        case SystemElement::PhysicalByte:
        case SystemElement::LogicalByte:
            if (target.getHardare().read(constraint.ElementIndex, byteResult))
            {
                value = byteResult;
                isFound = true;
            }
            break;

        case SystemElement::PhysicalHalfWord:
        case SystemElement::LogicalHalfWord:
            if (target.getHardare().read(constraint.ElementIndex, halfWordResult))
            {
                value = halfWordResult;
                isFound = true;
            }
            break;

        case SystemElement::PhysicalWord:
        case SystemElement::LogicalWord:
            isFound = target.getHardare().read(constraint.ElementIndex, value);
            break;

        case SystemElement::SystemRegister:
            isFound = true;

            switch (Ag::fromScalar<SystemRegister>(constraint.ElementIndex))
            {
            case SystemRegister::PC:
                value = target.getRegisters().getPC();
                break;

            case SystemRegister::CPSR:
                value = target.getRegisters().getPSR();
                break;

            case SystemRegister::Status:
                value = target.getRegisters().getPSR() >> PsrShift::Status;
                break;

            case SystemRegister::ProcessorMode:
                value = Ag::toScalar(target.getRegisters().getMode());
                break;

            case SystemRegister::IrqStatus:
            case SystemRegister::IrqMask:
            case SystemRegister::SPSR:
            default:
                isFound = false;
                break;
            }
            break;

        default:
            isFound = false;
            break;
        }

        return isFound;
    }
};

//! @brief A test fixture class which runs a test which creates an emulated
//! machine, sets an initial state, runs code and verifies the resultant state.
//! @tparam TSysTraits The data type of the traits describing the emulated
//! system to test.
template<typename TSysTraits>
class ExecTestFixture : public ::testing::Test
{
private:
    CoreTestParams _testPoint;

public:
    //! @brief Constructs an object to execute code and verify the result.
    //! @param[in] testPoint The conditions of the test to execute.
    explicit ExecTestFixture(const CoreTestParams &testPoint) :
        _testPoint(testPoint)
    {
    }

    void TestBody() override
    {
        ConstraintCollection initialState;
        ConstraintCollection expectedResultState;
        ArmSystem<TSysTraits> specimen;

        ASSERT_TRUE(prepareTestSystem(&specimen, _testPoint.Code));
        ASSERT_TRUE(applyConstraints(specimen, _testPoint.Location,
                                     _testPoint.Inputs));

        ExecutionMetrics metrics = specimen.run();

        EXPECT_GT(metrics.CycleCount, 0u);
        ASSERT_TRUE(verifyConstraints(specimen, _testPoint.Location,
                                      _testPoint.ExpectedOutputs));
    }
};

//! @brief Registers an array of ExecTestFixture-based unit tests.
//! @tparam TSysTraits The data type of the traits describing the emulated
//! system to instantiate in tests.
//! @param[in] sysName The name of the group of tests being registered.
//! @param[in] testPoints The array of test parameters describing the tests to
//! register.
//! @param[in] count The count of elements in the testPoints array.
template<typename TSysTraits>
void RegisterExecTests(const char *sysName, const CoreTestParams *testPoints, size_t count)
{
    struct Factory
    {
    private:
        CoreTestParams _testPoint;
    public:
        Factory(const CoreTestParams &testPoint) :
            _testPoint(testPoint)
        {
        }

        ExecTestFixture<TSysTraits> *operator()() const
        {
            return new ExecTestFixture<TSysTraits>(_testPoint);
        }
    };

    for (size_t i = 0; i < count; ++i)
    {
        const CoreTestParams &testPoint = testPoints[i];

        ::testing::RegisterTest(sysName,                                // Suite name
                                testPoint.Name.data(),                  // Test name
                                nullptr,                                // Type param description
                                nullptr,                                // Value param description
                                testPoint.Location.SourceFile.data(),   // Test source file
                                testPoint.Location.SourceLine,          // Test source line
                                Factory(testPoint));                    // Test factory
    }
}


}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

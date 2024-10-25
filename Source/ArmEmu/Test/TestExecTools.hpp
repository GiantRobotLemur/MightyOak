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
        Options opts;
        ConstraintCollection initialState;
        ConstraintCollection expectedResultState;
        ArmSystem<TSysTraits> specimen(opts);

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

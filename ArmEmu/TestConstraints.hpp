//! @file ArmEmu/TestConstraints.hpp
//! @brief The declaration of a system of objects used to set and verify the
//! state of an emulated system in tests.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_TEST_CONSTRAINTS_HPP__
#define __ARM_EMU_TEST_CONSTRAINTS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Format.hpp"


////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Captures the current position in source code.
#define TLOC TestLocation(__FILE__, __LINE__)

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Identifies elements of a processor which can be read or written to.
enum class SystemElement : uint8_t
{
    //! @brief Represents core registers R0-R15, including  A1-A4 and V1-V6.
    CoreRegister,

    //! @brief Represents co-processor identifiers C0-C15 or CP0-CP15 used to
    //! disambiguate co-processor register references.
    CoProcessor,

    //! @brief Represents a co-processor register CR0-CR15, although should be
    //! disambiguated by previously specifying a co-processor identifier.
    CoProcRegister,

    //! @brief A Floating Point Accelerator register F0-F7.
    FpaRegister,

    //! @brief A system register, identified by the SystemRegister enumeration.
    SystemRegister,

    //! @brief A byte located by its physical address.
    PhysicalByte,

    //! @brief A 16-bit half-word located by its physical address.
    PhysicalHalfWord,

    //! @brief A 32-bit word located by its physical address.
    PhysicalWord,

    //! @brief A byte located by its logical address based on current address
    //! translation settings.
    LogicalByte,

    //! @brief A 16-bit half-word located by its logical address based on
    //! current address translation settings.
    LogicalHalfWord,

    //! @brief A 32-bit word located by its logical address based on
    //! current address translation settings.
    LogicalWord,

    //! @brief The physical address which maps to the specified logical address
    //! based on current address translation settings.
    MappedPhysicalAddress,

    //! @brief A value used for bounds checking.
    Max,
};

//! @brief Identifies individual processor registers to be read or written by
//! constraints.
enum class SystemRegister : uint32_t
{
    PC,
    CPSR,
    SPSR,
    Status,
    ProcessorMode,
    IrqStatus,
    IrqMask,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Encapsulates a value to store in an emulated processor, or expected
//! set within an emulated processor.
//! @todo Support FPA real values.
struct Constraint
{
    //! @brief The value to store in the specified Element or compare with
    //! the value stored in that Element.
    uint32_t Value;

    //! @brief The Location index, i.e. the index of the register given a
    //! specific bank of registers, or a system register if Location is
    //! SystemRegister, or a memory address if Element is Memory.
    uint32_t ElementIndex;

    //! @brief Specifies the classification of location to query, either
    //! a register bank or memory.
    SystemElement Element;

    Constraint();
    Constraint(SystemElement location, uint32_t index, uint32_t value);

    bool operator==(const Constraint &rhs) const;
    bool operator<(const Constraint &rhs) const;

    std::string toString() const;
    std::string idToString() const;
};

using ConstraintCollection = std::vector<Constraint>;

//! @brief Describes the location of the definition of a set of test
//! parameters in the source code.
struct TestLocation
{
    std::string_view SourceFile;
    int SourceLine;

    // Construction
    TestLocation();
    TestLocation(const char *sourceFile, int sourceLine);

    // Accessors
    void appendToString(std::string &buffer) const;
};

//! @brief Defines the conditions of a test using constraints expressions.
struct CoreTestParams
{
    //! @brief The name of the test to report.
    std::string_view Name;

    //! @brief The constraints expression describing the initial processor state.
    std::string_view Inputs;

    //! @brief The constraints expression describing the expected processor state
    //! after the code has been executed.
    std::string_view ExpectedOutputs;

    //! @brief The assembly language source code to run.
    std::string_view Code;

    //! @brief The location in source code where the test parameters were defined.
    TestLocation Location;

    CoreTestParams() = default;
    CoreTestParams(const TestLocation &loc, const std::string_view &name,
                   const std::string_view &inputs, const std::string_view &outputs,
                   const std::string_view &assemblerCode);

    // Accessors
    void appendIdToString(std::string &buffer) const;
};

std::ostream &operator<<(std::ostream &os, const CoreTestParams &rhs);

struct CoreTestParamsName
{
    std::string operator()(const testing::TestParamInfo<typename CoreTestParams> &rhs) const;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
testing::AssertionResult parseConstraints(const TestLocation &loc,
                                          const std::string_view &text,
                                          ConstraintCollection &constraints);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief The specification for an object which can apply and extract constraint
//! values.
//! @tparam TTarget The data type of the object values are applied to or
//! extracted from.
template<typename TTarget>
struct ConstraintInterpretor
{
    //! @brief Attempts to set a constraint value on the target.
    //! @param[in] target The target to apply the constraint to.
    //! @param[in] constraint The constraint to apply.
    //! @retval true The constraint could be applied.
    //! @retval false The constraint is incompatible with the target.
    bool apply(TTarget &target, const Constraint &constraint) const
    {
        // The default implementation fails in all cases.
        return false;
    }

    //! @brief Attempts to get the value of a constraint from the target.
    //! @param[in] target The target object to query.
    //! @param[in] constraint The constraint to extract a value for.
    //! @param[out] value The extracted value.
    //! @retval true A value for the constraint was successfully extracted.
    //! @retval false The constraint was incompatible with the target.
    bool extract(TTarget &target, const Constraint &constraint,
                 uint32_t &value) const
    {
        // The default implementation fails in all cases.
        return false;
    }
};

//! @brief Applies a set of constraint value to a target object.
//! @tparam TTarget The data type of the target.
//! @tparam TInterpretor The data type of the object which can apply constraints.
//! @param[in] target The object to apply constraints to.
//! @param[in] loc The location of the test definition in source code.
//! @param[in] constraintsExpr The set of constraints to apply expressed as text.
//! @return An assertion result indicating if the constraints were
//! successfully applied.
template<typename TTarget, typename TInterpretor = ConstraintInterpretor<typename TTarget>>
testing::AssertionResult applyConstraints(TTarget &target, const TestLocation &loc,
                                          const std::string_view &constraintsExpr)
{
    ConstraintCollection items;
    testing::AssertionResult result = parseConstraints(loc, constraintsExpr, items);

    if (result)
    {
        TInterpretor interpretor;

        for (const Constraint &item : items)
        {
            if (interpretor.apply(target, item) == false)
            {
                result = testing::AssertionFailure();
                std::string message("Failed to apply constraint at: ");
                loc.appendToString(message);
                message.push_back('\n');
                message.append(item.toString());

                result << message;
                break;
            }
        }
    }

    return result;
}

//! @brief Attempts to verify if a number of constraints have expected values.
//! @tparam TTarget The data type of the target.
//! @tparam TInterpretor The data type of the object which can extract
//! constraint values.
//! @param[in] target The object to extract values from.
//! @param[in] loc The location of the test definition in source code.
//! @param[in] constraintsExpr A set of expected constraint values to validate.
//! @return An assertion result detailing whether all constraints were satisfied.
template<typename TTarget, typename TInterpretor = ConstraintInterpretor<typename TTarget>>
testing::AssertionResult verifyConstraints(TTarget &target, const TestLocation &loc,
                                           const std::string_view &constraintsExpr)
{
    ConstraintCollection items;
    testing::AssertionResult result = parseConstraints(loc, constraintsExpr, items);

    if (result)
    {
        TInterpretor interpretor;
        bool hasErrors = false;

        for (const Constraint &item : items)
        {
            uint32_t value = 0;;
            if (interpretor.extract(target, item, value))
            {
                // Compare the extracted value with the expected value.
                if (value != item.Value)
                {
                    if (hasErrors)
                    {
                        result << '\n';
                    }
                    else
                    {
                        result = testing::AssertionFailure();
                        std::string message("Constraints failure: ");
                        loc.appendToString(message);
                        message.push_back('\n');

                        result << message;
                        hasErrors = true;
                    }

                    result << item.idToString() << ": Expected ";

                    Ag::FormatInfo fmt;
                    uint32_t maxValue = std::max(value, item.Value);
                    std::string values;
                    values.reserve(32);

                    if (maxValue > 9)
                    {
                        // Use hex for larger values.
                        fmt.setRadix(16);
                        int msb;

                        if (Ag::Bin::bitScanReverse(maxValue, msb))
                        {
                            uint16_t binaryDigits = (static_cast<uint16_t>(msb) + 3) & ~3;

                            fmt.setMinimumWholeDigits(binaryDigits / 4);
                        }

                        values.append("0x");
                        Ag::appendValue(fmt, values, item.Value);
                        values.append(" != 0x");
                        Ag::appendValue(fmt, values, value);
                    }
                    else
                    {
                        // For small decimal values.
                        Ag::appendValue(fmt, values, item.Value);
                        values.append(" != ");
                        Ag::appendValue(fmt, values, value);
                    }

                    values.push_back('.');
                    result << values;
                    hasErrors = true;
                }
            }
            else
            {
                if (hasErrors)
                {
                    result << '\n';
                }
                else
                {
                    result = testing::AssertionFailure();

                    std::string message("Constraints failure: ");
                    loc.appendToString(message);
                    message.push_back('\n');

                    result << message;
                    hasErrors = true;
                }

                result << "Failed to extract a constraint value from " << item.idToString();
            }
        }
    }

    return result;
}

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

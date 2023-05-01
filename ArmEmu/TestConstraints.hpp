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

#include <vector>

#include <gtest/gtest.h>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Format.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Identifies elements of a processor which can be read or written to.
enum class SystemElement : uint8_t
{
    CoreRegister,
    CoProcessor,
    CoProcRegister,
    FpaRegister,
    SystemRegister,

    Max,
};

//! @brief Identifies individual processor registers to be read or written by
//! constraints.
enum class SystemRegister : uint8_t
{
    PC,
    CPSR,
    SPSR,
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
    uint32_t Value;
    SystemElement Location;
    uint8_t Index;
    uint8_t SubIndex;

    Constraint();
    Constraint(SystemElement location, uint8_t index, uint32_t value);

    bool operator==(const Constraint &rhs) const;
    bool operator<(const Constraint &rhs) const;

    std::string toString() const;
    std::string idToString() const;
};

using ConstraintCollection = std::vector<Constraint>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
testing::AssertionResult parseConstraints(const std::string_view &text,
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
    bool apply(TTarget &target, const Constraint &constraint) const;

    //! @brief Attempts to get the value of a constraint from the target.
    //! @param[in] target The target object to query.
    //! @param[in] constraint The constraint to extract a value for.
    //! @param[out] value The extracted value.
    //! @retval true A value for the constraint was successfully extracted.
    //! @retval false The constraint was incompatible with the target.
    bool extract(const TTarget &target, const Constraint &constraint, uint32_t &value) const;
};

//! @brief Applies a set of constraint value to a target object.
//! @tparam TTarget The data type of the target.
//! @tparam TInterpretor The data type of the object which can apply constraints.
//! @param[in] target The object to apply constraints to.
//! @param[in] constraintsExpr The set of constraints to apply expressed as text.
//! @return An assertion result indicating if the constraints were
//! successfully applied.
template<typename TTarget, typename TInterpretor>
testing::AssertionResult applyConstraints(TTarget &target, const std::string_view &constraintsExpr)
{
    ConstraintCollection items;
    testing::AssertionResult result = parseConstraints(constraintsExpr, items);

    if (result)
    {
        TInterpretor interpretor;

        for (const Constraint &item : items)
        {
            if (interpretor.apply(target, item) == false)
            {
                result = testing::AssertionFailure();

                result << "Failed to apply constraint " << item.toString();
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
//! @param[in] constraintsExpr A set of expected constraint values to validate.
//! @return An assertion result detailing whether all constraints were satisfied.
template<typename TTarget, typename TInterpretor>
testing::AssertionResult verifyConstraints(const TTarget &target,
                                           const std::string_view &constraintsExpr)
{
    ConstraintCollection items;
    testing::AssertionResult result = parseConstraints(constraintsExpr, items);

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
                    }

                    result << "Constraint not met. " << item.idToString() << ": ";

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
                }

                result << "Failed to extract a constraint value from " << item.idToString();
                hasErrors = true;
            }
        }
    }

    return result;
}

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

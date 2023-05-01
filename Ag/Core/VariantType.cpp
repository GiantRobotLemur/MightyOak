//! @file VariantType.cpp
//! @brief The definition of a value type which can hold one of many possible
//! data types.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <map>

#include "CoreInternal.hpp"
#include "Ag/Core/VariantType.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// VariantTypeMismatchException Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an exception due to a Variant having an unexpected data type.
//! @param[in] typeName The data type of the Variant, nullptr if it had none.
VariantTypeMismatchException::VariantTypeMismatchException(const char *typeName)
{
    std::string detail;

    if (typeName == nullptr)
    {
        detail.assign("The Variant had no assigned type.");
    }
    else
    {
        appendPrintf(detail,
                     "The Variant had an unexpected data type of '%s'.",
                     typeName);
    }

    initialise("VariantTypeMismatchException",
               "The program attempted to perform a data-type specific operation "
               "on a Variant and the Variant value was not of the expected type.",
               detail, 0);
}

////////////////////////////////////////////////////////////////////////////////
// VariantTypePair Member Definitions
////////////////////////////////////////////////////////////////////////////////
VariantTypePair::VariantTypePair(const VariantType *source,
                                 const VariantType *target) :
    Source(source),
    Target(target)
{
}

bool VariantTypePair::operator<(const VariantTypePair &rhs) const
{
    bool isLessThan = false;

    if (Source == rhs.Source)
    {
        isLessThan = Target < rhs.Target;
    }
    else
    {
        isLessThan = Source < rhs.Source;
    }

    return isLessThan;
}

////////////////////////////////////////////////////////////////////////////////
// VariantType Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Registers a function which can convert one variant type
//! to another.
//! @param[in] source The data type of the source variant data.
//! @param[in] target The data type produced by the conversion.
//! @param[in] fn A function to attempt to perform the conversion.
void VariantType::registerConversion(const VariantType *source,
                                     const VariantType *target,
                                     VariantConversionFn fn)
{
    VariantTypePair key(source, target);

    getConversionMap()[key] = fn;
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


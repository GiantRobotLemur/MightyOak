//! @file Core/CoreInternal.hpp
//! @brief The declaration of a set of definitions which are internal to the
//! Core module and should not be exposed in its public interface.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_INTERNAL_HPP__
#define __AG_CORE_INTERNAL_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Ag/Core/Utils.hpp"
#include "Ag/Core/VariantType.hpp"

#include "Win32API.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
struct StackTracePrivate;
struct ActivationRecord;

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
typedef std::pair<size_t, size_t> StringRange;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
// Implemented in StackTrace.cpp.
#ifdef _WIN32
size_t captureActivationRecords(const CONTEXT *context,
                                ActivationRecord *records,
                                size_t maxCount,
                                size_t skip);
#endif

size_t getStackTraceSize(const StackTracePrivate *info);
StackTracePrivate *cloneStackTrace(const StackTracePrivate *info);
void destroyStackTrace(StackTracePrivate *&info);

// Implemented in StringPrivate.cpp.
void appendPrintf(std::string &target, const char *format, ...);
bool tryReadLine(StdFilePtr &input, std::string &line);
void tokeniseLine(const std::string &line, std::vector<StringRange> &tokenRanges);
std::string_view getLineToken(const std::string &line, const StringRange &range);

// Implemented in VariantType.cpp.

//! @brief An object which represents a pair of variant types used to identify
//! a valid conversion.
struct VariantTypePair
{
    const VariantType *Source;
    const VariantType *Target;

    VariantTypePair(const VariantType *source, const VariantType *target);

    bool operator<(const VariantTypePair &rhs) const;
};

typedef std::map<VariantTypePair, VariantConversionFn> VariantConversionMap;

// Implemented in VariantTypes.cpp
VariantConversionMap &getConversionMap();

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

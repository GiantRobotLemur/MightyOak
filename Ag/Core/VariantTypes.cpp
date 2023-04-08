//! @file Core/VariantTypes.cpp
//! @brief The definition of various implementations of the VariantType class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <limits>

#include "CoreInternal.hpp"

#include "Ag/Core/Format.hpp"
#include "Ag/Core/ScalarParser.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/VariantType.hpp"
#include "Ag/Core/VariantTypes.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_SCALAR_VARIANT_TYPE(name, datatype) \
const name ## VariantType * name ## VariantType::getInstance() { \
    static name ## VariantType singleton; \
    return &singleton; } \
void name ## VariantType::create(VariantData &variant, datatype value) const { \
    variant. name = value; } \
datatype name ## VariantType::getValue(const VariantData &variant) const { \
    return variant. name; } \
datatype & name ## VariantType::getRef(VariantData &variant) const { \
    return variant. name; } \
const datatype & name ## VariantType::getRef(const VariantData &variant) const { \
    return variant. name; } \
datatype * name ## VariantType::getPtr(VariantData &variant) const { \
    return &variant. name; } \
const datatype * name ## VariantType::getPtr(const VariantData &variant) const { \
    return &variant. name; } \
const char * name ## VariantType::getName() const { return # name ; } \
void name ## VariantType::destroy(const VariantData &) const { } \
void name ## VariantType::copy(VariantData &destination, const VariantData &source) const { \
    destination. name = source. name ; } \
void name ## VariantType::move(VariantData &destination, VariantData &&source) const { \
    destination. name = source. name ; } \
bool name ## VariantType::read(IStream *input, VariantData &destination) const { \
    return input->tryRead(&destination. name, sizeof(destination. name )); } \
bool name ## VariantType::write(IStream *output, const VariantData &source) const { \
    return output->tryWrite(&source. name, sizeof(source. name)); } \
void name ## VariantType::toString(const FormatInfo &format, \
                               const VariantData &value, \
                               std::string &buffer) const { \
    appendValue(format, buffer, value. name); } \
bool name ## VariantType::tryParse(const LocaleInfo &format, utf8_cptr_t source, \
                               size_t &length, VariantData &value) const { \
    ScalarParser parser(format); \
    parseScalar< datatype >(parser, source, length); \
    return parser.tryGetValue(value. name); }

#define DECLARE_VARIANT_TYPE(name) \
const name ## VariantType *VariantTypes::name = name ## VariantType::getInstance()

#define DEFINE_TO_STRING_CONVERSION(source) \
bool convert ## source ## ToString(const VariantData &sourceValue, \
                                        VariantData &targetValue) {\
    std::string buffer; \
    scalarToString(sourceValue. source, buffer); \
    targetValue.Pointer = new Ag::String(buffer); \
    return true; }

#define DECLARE_TO_STRING_CONVERSION(source) \
    map[{ VariantTypes:: source, VariantTypes::String }] = \
        convert ## source ## ToString

#define DEFINE_SCALAR_CONVERSION(source, target) \
bool convert ## source ## To ## target (const VariantData &sourceValue, \
                                        VariantData &targetValue) {\
    return convertScalar(sourceValue. source, targetValue. target); }

#define DECLARE_SCALAR_CONVERSION(source, target) \
    map[VariantTypePair(VariantTypes::  source, VariantTypes::  target)] = \
        convert ## source ## To ## target


namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Global Data
////////////////////////////////////////////////////////////////////////////////

DECLARE_VARIANT_TYPE(Boolean);
DECLARE_VARIANT_TYPE(Character);
DECLARE_VARIANT_TYPE(Int8);
DECLARE_VARIANT_TYPE(Uint8);
DECLARE_VARIANT_TYPE(Int16);
DECLARE_VARIANT_TYPE(Uint16);
DECLARE_VARIANT_TYPE(Int32);
DECLARE_VARIANT_TYPE(Uint32);
DECLARE_VARIANT_TYPE(Int64);
DECLARE_VARIANT_TYPE(Uint64);
DECLARE_VARIANT_TYPE(Intptr);
DECLARE_VARIANT_TYPE(Uintptr);
DECLARE_VARIANT_TYPE(Pointer);
DECLARE_VARIANT_TYPE(Float);
DECLARE_VARIANT_TYPE(Double);
DECLARE_VARIANT_TYPE(String);

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Configures a scalar parser and processes characters in order to try
//! to parse a specified scalar data type.
//! @tparam[in] T The data type of the scalar to produce from the parsed digits.
//! @param[in,out] parser The object to configure and process characters.
//! @param[in] source The bounded array characters to parse.
//! @param[in,out] length The maximum count of characters in source, receives
//! the actual count of characters processed.
template<typename T>
void parseScalar(ScalarParser &parser, utf8_cptr_t source, size_t &length)
{
    using TypeInfo = std::numeric_limits<T>;
    parser.enableFraction(TypeInfo::is_exact == false);
    parser.enableExponent(TypeInfo::is_exact == false);
    parser.enableSign(TypeInfo::is_signed);

    bool isOK = true;

    for (size_t index = 0; isOK && (index < length);)
    {
        if (parser.tryProcessCharacter(source[index]))
        {
            ++index;
        }
        else
        {
            length = index;
            isOK = false;
        }
    }
}

template<typename TSource, typename TTarget>
bool convertScalar(const TSource &sourceValue, TTarget &targetValue)
{
    using SourceInfo = std::numeric_limits<TSource>;
    using TargetInfo = std::numeric_limits<TTarget>;
    bool canConvert = true;

    if constexpr (SourceInfo::is_signed == TargetInfo::is_signed)
    {
        if constexpr (SourceInfo::digits10 >= TargetInfo::digits10)
        {
            // Only check the value if the range of the source type
            // is greater than that of the target type.
            canConvert = (sourceValue >= static_cast<TSource>(TargetInfo::lowest())) &&
                (sourceValue <= static_cast<TSource>(TargetInfo::max()));
        }
    }
    else
    {
        // The signedness of the two data types does not match.
        if constexpr (SourceInfo::is_signed)
        {
            if constexpr (SourceInfo::digits10 >= TargetInfo::digits10)
            {
                // Only check the value if the range of the source type
                // is greater than that of the target type.
                canConvert = (sourceValue >= static_cast<TSource>(0)) &&
                    (sourceValue <= static_cast<TSource>(TargetInfo::max()));
            }
        }
        else
        {
            if constexpr (SourceInfo::digits10 >= TargetInfo::digits10)
            {
                // Only check the value if the range of the source type
                // is greater than that of the target type.
                canConvert = (sourceValue <= static_cast<TSource>(TargetInfo::max()));
            }
        }
    }

    if (canConvert)
    {
        targetValue = static_cast<TTarget>(sourceValue);
    }

    return canConvert;
}

//! @brief Converts a scalar value to a string using the neutral locale.
//! @tparam T The data type of the scalar to convert.
//! @param[in] value The value to convert to a string.
//! @param[out] buffer The STL string to receive the converted value.
template<typename T> void scalarToString(const T &value, std::string &buffer)
{
    FormatInfo format(LocaleInfo::getNeutral());

    appendValue(format, buffer, value);
}

// Define standard conversions.
DEFINE_SCALAR_CONVERSION(Int8, Uint8)
DEFINE_SCALAR_CONVERSION(Int8, Int16)
DEFINE_SCALAR_CONVERSION(Int8, Uint16)
DEFINE_SCALAR_CONVERSION(Int8, Int32)
DEFINE_SCALAR_CONVERSION(Int8, Uint32)
DEFINE_SCALAR_CONVERSION(Int8, Int64)
DEFINE_SCALAR_CONVERSION(Int8, Uint64)
DEFINE_SCALAR_CONVERSION(Int8, Intptr)
DEFINE_SCALAR_CONVERSION(Int8, Uintptr)
DEFINE_SCALAR_CONVERSION(Int8, Float)
DEFINE_SCALAR_CONVERSION(Int8, Double)
DEFINE_TO_STRING_CONVERSION(Int8)

DEFINE_SCALAR_CONVERSION(Uint8, Int8)
DEFINE_SCALAR_CONVERSION(Uint8, Int16)
DEFINE_SCALAR_CONVERSION(Uint8, Uint16)
DEFINE_SCALAR_CONVERSION(Uint8, Int32)
DEFINE_SCALAR_CONVERSION(Uint8, Uint32)
DEFINE_SCALAR_CONVERSION(Uint8, Int64)
DEFINE_SCALAR_CONVERSION(Uint8, Uint64)
DEFINE_SCALAR_CONVERSION(Uint8, Intptr)
DEFINE_SCALAR_CONVERSION(Uint8, Uintptr)
DEFINE_SCALAR_CONVERSION(Uint8, Float)
DEFINE_SCALAR_CONVERSION(Uint8, Double)
DEFINE_TO_STRING_CONVERSION(Uint8)

DEFINE_SCALAR_CONVERSION(Int16, Int8)
DEFINE_SCALAR_CONVERSION(Int16, Uint8)
DEFINE_SCALAR_CONVERSION(Int16, Uint16)
DEFINE_SCALAR_CONVERSION(Int16, Int32)
DEFINE_SCALAR_CONVERSION(Int16, Uint32)
DEFINE_SCALAR_CONVERSION(Int16, Int64)
DEFINE_SCALAR_CONVERSION(Int16, Uint64)
DEFINE_SCALAR_CONVERSION(Int16, Intptr)
DEFINE_SCALAR_CONVERSION(Int16, Uintptr)
DEFINE_SCALAR_CONVERSION(Int16, Float)
DEFINE_SCALAR_CONVERSION(Int16, Double)
DEFINE_TO_STRING_CONVERSION(Int16)

DEFINE_SCALAR_CONVERSION(Uint16, Int8)
DEFINE_SCALAR_CONVERSION(Uint16, Uint8)
DEFINE_SCALAR_CONVERSION(Uint16, Int16)
DEFINE_SCALAR_CONVERSION(Uint16, Int32)
DEFINE_SCALAR_CONVERSION(Uint16, Uint32)
DEFINE_SCALAR_CONVERSION(Uint16, Int64)
DEFINE_SCALAR_CONVERSION(Uint16, Uint64)
DEFINE_SCALAR_CONVERSION(Uint16, Intptr)
DEFINE_SCALAR_CONVERSION(Uint16, Uintptr)
DEFINE_SCALAR_CONVERSION(Uint16, Float)
DEFINE_SCALAR_CONVERSION(Uint16, Double)
DEFINE_TO_STRING_CONVERSION(Uint16)

DEFINE_SCALAR_CONVERSION(Int32, Int8)
DEFINE_SCALAR_CONVERSION(Int32, Uint8)
DEFINE_SCALAR_CONVERSION(Int32, Int16)
DEFINE_SCALAR_CONVERSION(Int32, Uint16)
DEFINE_SCALAR_CONVERSION(Int32, Uint32)
DEFINE_SCALAR_CONVERSION(Int32, Int64)
DEFINE_SCALAR_CONVERSION(Int32, Uint64)
DEFINE_SCALAR_CONVERSION(Int32, Intptr)
DEFINE_SCALAR_CONVERSION(Int32, Uintptr)
DEFINE_SCALAR_CONVERSION(Int32, Float)
DEFINE_SCALAR_CONVERSION(Int32, Double)
DEFINE_TO_STRING_CONVERSION(Int32)

DEFINE_SCALAR_CONVERSION(Uint32, Int8)
DEFINE_SCALAR_CONVERSION(Uint32, Uint8)
DEFINE_SCALAR_CONVERSION(Uint32, Int16)
DEFINE_SCALAR_CONVERSION(Uint32, Uint16)
DEFINE_SCALAR_CONVERSION(Uint32, Int32)
DEFINE_SCALAR_CONVERSION(Uint32, Int64)
DEFINE_SCALAR_CONVERSION(Uint32, Uint64)
DEFINE_SCALAR_CONVERSION(Uint32, Intptr)
DEFINE_SCALAR_CONVERSION(Uint32, Uintptr)
DEFINE_SCALAR_CONVERSION(Uint32, Float)
DEFINE_SCALAR_CONVERSION(Uint32, Double)
DEFINE_TO_STRING_CONVERSION(Uint32)

DEFINE_SCALAR_CONVERSION(Int64, Int8)
DEFINE_SCALAR_CONVERSION(Int64, Uint8)
DEFINE_SCALAR_CONVERSION(Int64, Int16)
DEFINE_SCALAR_CONVERSION(Int64, Uint16)
DEFINE_SCALAR_CONVERSION(Int64, Int32)
DEFINE_SCALAR_CONVERSION(Int64, Uint32)
DEFINE_SCALAR_CONVERSION(Int64, Uint64)
DEFINE_SCALAR_CONVERSION(Int64, Intptr)
DEFINE_SCALAR_CONVERSION(Int64, Uintptr)
DEFINE_SCALAR_CONVERSION(Int64, Float)
DEFINE_SCALAR_CONVERSION(Int64, Double)
DEFINE_TO_STRING_CONVERSION(Int64)

DEFINE_SCALAR_CONVERSION(Uint64, Int8)
DEFINE_SCALAR_CONVERSION(Uint64, Uint8)
DEFINE_SCALAR_CONVERSION(Uint64, Int16)
DEFINE_SCALAR_CONVERSION(Uint64, Uint16)
DEFINE_SCALAR_CONVERSION(Uint64, Int32)
DEFINE_SCALAR_CONVERSION(Uint64, Uint32)
DEFINE_SCALAR_CONVERSION(Uint64, Int64)
DEFINE_SCALAR_CONVERSION(Uint64, Intptr)
DEFINE_SCALAR_CONVERSION(Uint64, Uintptr)
DEFINE_SCALAR_CONVERSION(Uint64, Float)
DEFINE_SCALAR_CONVERSION(Uint64, Double)
DEFINE_TO_STRING_CONVERSION(Uint64)

DEFINE_SCALAR_CONVERSION(Intptr, Int8)
DEFINE_SCALAR_CONVERSION(Intptr, Uint8)
DEFINE_SCALAR_CONVERSION(Intptr, Int16)
DEFINE_SCALAR_CONVERSION(Intptr, Uint16)
DEFINE_SCALAR_CONVERSION(Intptr, Int32)
DEFINE_SCALAR_CONVERSION(Intptr, Uint32)
DEFINE_SCALAR_CONVERSION(Intptr, Int64)
DEFINE_SCALAR_CONVERSION(Intptr, Uint64)
DEFINE_SCALAR_CONVERSION(Intptr, Uintptr)
DEFINE_SCALAR_CONVERSION(Intptr, Float)
DEFINE_SCALAR_CONVERSION(Intptr, Double)
DEFINE_TO_STRING_CONVERSION(Intptr)

DEFINE_SCALAR_CONVERSION(Uintptr, Int8)
DEFINE_SCALAR_CONVERSION(Uintptr, Uint8)
DEFINE_SCALAR_CONVERSION(Uintptr, Int16)
DEFINE_SCALAR_CONVERSION(Uintptr, Uint16)
DEFINE_SCALAR_CONVERSION(Uintptr, Int32)
DEFINE_SCALAR_CONVERSION(Uintptr, Uint32)
DEFINE_SCALAR_CONVERSION(Uintptr, Int64)
DEFINE_SCALAR_CONVERSION(Uintptr, Uint64)
DEFINE_SCALAR_CONVERSION(Uintptr, Intptr)
// Exists for pointer conversion.
DEFINE_SCALAR_CONVERSION(Uintptr, Uintptr)
DEFINE_SCALAR_CONVERSION(Uintptr, Float)
DEFINE_SCALAR_CONVERSION(Uintptr, Double)
DEFINE_TO_STRING_CONVERSION(Uintptr)

DEFINE_SCALAR_CONVERSION(Float, Int8)
DEFINE_SCALAR_CONVERSION(Float, Uint8)
DEFINE_SCALAR_CONVERSION(Float, Int16)
DEFINE_SCALAR_CONVERSION(Float, Uint16)
DEFINE_SCALAR_CONVERSION(Float, Int32)
DEFINE_SCALAR_CONVERSION(Float, Uint32)
DEFINE_SCALAR_CONVERSION(Float, Int64)
DEFINE_SCALAR_CONVERSION(Float, Uint64)
DEFINE_SCALAR_CONVERSION(Float, Intptr)
DEFINE_SCALAR_CONVERSION(Float, Uintptr)
DEFINE_SCALAR_CONVERSION(Float, Double)
DEFINE_TO_STRING_CONVERSION(Float)

DEFINE_SCALAR_CONVERSION(Double, Int8)
DEFINE_SCALAR_CONVERSION(Double, Uint8)
DEFINE_SCALAR_CONVERSION(Double, Int16)
DEFINE_SCALAR_CONVERSION(Double, Uint16)
DEFINE_SCALAR_CONVERSION(Double, Int32)
DEFINE_SCALAR_CONVERSION(Double, Uint32)
DEFINE_SCALAR_CONVERSION(Double, Int64)
DEFINE_SCALAR_CONVERSION(Double, Uint64)
DEFINE_SCALAR_CONVERSION(Double, Intptr)
DEFINE_SCALAR_CONVERSION(Double, Uintptr)
DEFINE_SCALAR_CONVERSION(Double, Float)
DEFINE_TO_STRING_CONVERSION(Double)

//! @brief Registers conversion functions with the conversion map.
void initialiseStandardConversions(VariantConversionMap &map)
{
    // Register all standard conversions.
    DECLARE_SCALAR_CONVERSION(Int8, Uint8);
    DECLARE_SCALAR_CONVERSION(Int8, Int16);
    DECLARE_SCALAR_CONVERSION(Int8, Uint16);
    DECLARE_SCALAR_CONVERSION(Int8, Int32);
    DECLARE_SCALAR_CONVERSION(Int8, Uint32);
    DECLARE_SCALAR_CONVERSION(Int8, Int64);
    DECLARE_SCALAR_CONVERSION(Int8, Uint64);
    DECLARE_SCALAR_CONVERSION(Int8, Intptr);
    DECLARE_SCALAR_CONVERSION(Int8, Uintptr);
    DECLARE_SCALAR_CONVERSION(Int8, Float);
    DECLARE_SCALAR_CONVERSION(Int8, Double);
    DECLARE_TO_STRING_CONVERSION(Int8);

    DECLARE_SCALAR_CONVERSION(Uint8, Int8);
    DECLARE_SCALAR_CONVERSION(Uint8, Int16);
    DECLARE_SCALAR_CONVERSION(Uint8, Uint16);
    DECLARE_SCALAR_CONVERSION(Uint8, Int32);
    DECLARE_SCALAR_CONVERSION(Uint8, Uint32);
    DECLARE_SCALAR_CONVERSION(Uint8, Int64);
    DECLARE_SCALAR_CONVERSION(Uint8, Uint64);
    DECLARE_SCALAR_CONVERSION(Uint8, Intptr);
    DECLARE_SCALAR_CONVERSION(Uint8, Uintptr);
    DECLARE_SCALAR_CONVERSION(Uint8, Float);
    DECLARE_SCALAR_CONVERSION(Uint8, Double);
    DECLARE_TO_STRING_CONVERSION(Uint8);

    DECLARE_SCALAR_CONVERSION(Int16, Int8);
    DECLARE_SCALAR_CONVERSION(Int16, Uint8);
    DECLARE_SCALAR_CONVERSION(Int16, Uint16);
    DECLARE_SCALAR_CONVERSION(Int16, Int32);
    DECLARE_SCALAR_CONVERSION(Int16, Uint32);
    DECLARE_SCALAR_CONVERSION(Int16, Int64);
    DECLARE_SCALAR_CONVERSION(Int16, Uint64);
    DECLARE_SCALAR_CONVERSION(Int16, Intptr);
    DECLARE_SCALAR_CONVERSION(Int16, Uintptr);
    DECLARE_SCALAR_CONVERSION(Int16, Float);
    DECLARE_SCALAR_CONVERSION(Int16, Double);
    DECLARE_TO_STRING_CONVERSION(Int16);

    DECLARE_SCALAR_CONVERSION(Uint16, Int8);
    DECLARE_SCALAR_CONVERSION(Uint16, Uint8);
    DECLARE_SCALAR_CONVERSION(Uint16, Int16);
    DECLARE_SCALAR_CONVERSION(Uint16, Int32);
    DECLARE_SCALAR_CONVERSION(Uint16, Uint32);
    DECLARE_SCALAR_CONVERSION(Uint16, Int64);
    DECLARE_SCALAR_CONVERSION(Uint16, Uint64);
    DECLARE_SCALAR_CONVERSION(Uint16, Intptr);
    DECLARE_SCALAR_CONVERSION(Uint16, Uintptr);
    DECLARE_SCALAR_CONVERSION(Uint16, Float);
    DECLARE_SCALAR_CONVERSION(Uint16, Double);
    DECLARE_TO_STRING_CONVERSION(Uint16);

    DECLARE_SCALAR_CONVERSION(Int32, Int8);
    DECLARE_SCALAR_CONVERSION(Int32, Uint8);
    DECLARE_SCALAR_CONVERSION(Int32, Int16);
    DECLARE_SCALAR_CONVERSION(Int32, Uint16);
    DECLARE_SCALAR_CONVERSION(Int32, Uint32);
    DECLARE_SCALAR_CONVERSION(Int32, Int64);
    DECLARE_SCALAR_CONVERSION(Int32, Uint64);
    DECLARE_SCALAR_CONVERSION(Int32, Intptr);
    DECLARE_SCALAR_CONVERSION(Int32, Uintptr);
    DECLARE_SCALAR_CONVERSION(Int32, Float);
    DECLARE_SCALAR_CONVERSION(Int32, Double);
    DECLARE_TO_STRING_CONVERSION(Int32);

    DECLARE_SCALAR_CONVERSION(Uint32, Int8);
    DECLARE_SCALAR_CONVERSION(Uint32, Uint8);
    DECLARE_SCALAR_CONVERSION(Uint32, Int16);
    DECLARE_SCALAR_CONVERSION(Uint32, Uint16);
    DECLARE_SCALAR_CONVERSION(Uint32, Int32);
    DECLARE_SCALAR_CONVERSION(Uint32, Int64);
    DECLARE_SCALAR_CONVERSION(Uint32, Uint64);
    DECLARE_SCALAR_CONVERSION(Uint32, Intptr);
    DECLARE_SCALAR_CONVERSION(Uint32, Uintptr);
    DECLARE_SCALAR_CONVERSION(Uint32, Float);
    DECLARE_SCALAR_CONVERSION(Uint32, Double);
    DECLARE_TO_STRING_CONVERSION(Uint32);

    DECLARE_SCALAR_CONVERSION(Int64, Int8);
    DECLARE_SCALAR_CONVERSION(Int64, Uint8);
    DECLARE_SCALAR_CONVERSION(Int64, Int16);
    DECLARE_SCALAR_CONVERSION(Int64, Uint16);
    DECLARE_SCALAR_CONVERSION(Int64, Int32);
    DECLARE_SCALAR_CONVERSION(Int64, Uint32);
    DECLARE_SCALAR_CONVERSION(Int64, Uint64);
    DECLARE_SCALAR_CONVERSION(Int64, Intptr);
    DECLARE_SCALAR_CONVERSION(Int64, Uintptr);
    DECLARE_SCALAR_CONVERSION(Int64, Float);
    DECLARE_SCALAR_CONVERSION(Int64, Double);
    DECLARE_TO_STRING_CONVERSION(Int64);

    DECLARE_SCALAR_CONVERSION(Uint64, Int8);
    DECLARE_SCALAR_CONVERSION(Uint64, Uint8);
    DECLARE_SCALAR_CONVERSION(Uint64, Int16);
    DECLARE_SCALAR_CONVERSION(Uint64, Uint16);
    DECLARE_SCALAR_CONVERSION(Uint64, Int32);
    DECLARE_SCALAR_CONVERSION(Uint64, Uint32);
    DECLARE_SCALAR_CONVERSION(Uint64, Int64);
    DECLARE_SCALAR_CONVERSION(Uint64, Intptr);
    DECLARE_SCALAR_CONVERSION(Uint64, Uintptr);
    DECLARE_SCALAR_CONVERSION(Uint64, Float);
    DECLARE_SCALAR_CONVERSION(Uint64, Double);
    DECLARE_TO_STRING_CONVERSION(Uint64);

    DECLARE_SCALAR_CONVERSION(Intptr, Int8);
    DECLARE_SCALAR_CONVERSION(Intptr, Uint8);
    DECLARE_SCALAR_CONVERSION(Intptr, Int16);
    DECLARE_SCALAR_CONVERSION(Intptr, Uint16);
    DECLARE_SCALAR_CONVERSION(Intptr, Int32);
    DECLARE_SCALAR_CONVERSION(Intptr, Uint32);
    DECLARE_SCALAR_CONVERSION(Intptr, Int64);
    DECLARE_SCALAR_CONVERSION(Intptr, Uint64);
    DECLARE_SCALAR_CONVERSION(Intptr, Uintptr);
    DECLARE_SCALAR_CONVERSION(Intptr, Float);
    DECLARE_SCALAR_CONVERSION(Intptr, Double);
    DECLARE_TO_STRING_CONVERSION(Intptr);

    DECLARE_SCALAR_CONVERSION(Uintptr, Int8);
    DECLARE_SCALAR_CONVERSION(Uintptr, Uint8);
    DECLARE_SCALAR_CONVERSION(Uintptr, Int16);
    DECLARE_SCALAR_CONVERSION(Uintptr, Uint16);
    DECLARE_SCALAR_CONVERSION(Uintptr, Int32);
    DECLARE_SCALAR_CONVERSION(Uintptr, Uint32);
    DECLARE_SCALAR_CONVERSION(Uintptr, Int64);
    DECLARE_SCALAR_CONVERSION(Uintptr, Uint64);
    DECLARE_SCALAR_CONVERSION(Uintptr, Intptr);
    DECLARE_SCALAR_CONVERSION(Uintptr, Float);
    DECLARE_SCALAR_CONVERSION(Uintptr, Double);
    DECLARE_TO_STRING_CONVERSION(Uintptr);

    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Int8)] = convertUintptrToInt8;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Uint8)] = convertUintptrToUint8;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Int16)] = convertUintptrToInt16;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Uint16)] = convertUintptrToUint16;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Int32)] = convertUintptrToInt32;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Uint32)] = convertUintptrToUint32;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Int64)] = convertUintptrToInt64;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Uint64)] = convertUintptrToUint64;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Uintptr)] = convertUintptrToUintptr;
    map[VariantTypePair(VariantTypes::Pointer, VariantTypes::Intptr)] = convertUintptrToIntptr;

    DECLARE_SCALAR_CONVERSION(Float, Int8);
    DECLARE_SCALAR_CONVERSION(Float, Uint8);
    DECLARE_SCALAR_CONVERSION(Float, Int16);
    DECLARE_SCALAR_CONVERSION(Float, Uint16);
    DECLARE_SCALAR_CONVERSION(Float, Int32);
    DECLARE_SCALAR_CONVERSION(Float, Uint32);
    DECLARE_SCALAR_CONVERSION(Float, Int64);
    DECLARE_SCALAR_CONVERSION(Float, Uint64);
    DECLARE_SCALAR_CONVERSION(Float, Intptr);
    DECLARE_SCALAR_CONVERSION(Float, Uintptr);
    DECLARE_SCALAR_CONVERSION(Float, Double);
    DECLARE_TO_STRING_CONVERSION(Float);

    DECLARE_SCALAR_CONVERSION(Double, Int8);
    DECLARE_SCALAR_CONVERSION(Double, Uint8);
    DECLARE_SCALAR_CONVERSION(Double, Int16);
    DECLARE_SCALAR_CONVERSION(Double, Uint16);
    DECLARE_SCALAR_CONVERSION(Double, Int32);
    DECLARE_SCALAR_CONVERSION(Double, Uint32);
    DECLARE_SCALAR_CONVERSION(Double, Int64);
    DECLARE_SCALAR_CONVERSION(Double, Uint64);
    DECLARE_SCALAR_CONVERSION(Double, Intptr);
    DECLARE_SCALAR_CONVERSION(Double, Uintptr);
    DECLARE_SCALAR_CONVERSION(Double, Float);
    DECLARE_TO_STRING_CONVERSION(Double);
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SCALAR_VARIANT_TYPE(Int8, int8_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Uint8, uint8_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Int16, int16_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Uint16, uint16_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Int32, int32_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Uint32, uint32_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Int64, int64_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Uint64, uint64_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Intptr, intptr_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Uintptr, uintptr_t);
IMPLEMENT_SCALAR_VARIANT_TYPE(Float, float);
IMPLEMENT_SCALAR_VARIANT_TYPE(Double, double);

////////////////////////////////////////////////////////////////////////////////
// BooleanVariantType Member Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the singleton instance of the object describing the boolean
//! variant type.
const BooleanVariantType *BooleanVariantType::getInstance()
{
    static BooleanVariantType instance;

    return &instance;
}

const std::string_view BooleanVariantType::TrueText("True");
const std::string_view BooleanVariantType::FalseText("False");

//! @brief Supports the Variant initialiser template constructor.
void BooleanVariantType::create(VariantData &variant, bool value) const
{
    variant.Boolean = value;
}

//! @brief Supports the Variant::get() template function.
bool BooleanVariantType::getValue(const VariantData &variant) const
{
    return variant.Boolean;
}

//! @brief Supports the Variant::getPtr() template function.
bool *BooleanVariantType::getPtr(VariantData &variant) const
{
    return &variant.Boolean;
}

//! @brief Supports the Variant::getPtr() template function.
const bool *BooleanVariantType::getPtr(const VariantData &variant) const
{
    return &variant.Boolean;
}

//! @brief Supports the Variant::getRef() template function.
bool &BooleanVariantType::getRef(VariantData &variant) const
{
    return variant.Boolean;
}

//! @brief Supports the Variant::getRef() template function.
const bool &BooleanVariantType::getRef(const VariantData &variant) const
{
    return variant.Boolean;
}

// Inherited from VariantType.
const char *BooleanVariantType::getName() const { return "Boolean"; }

// Inherited from VariantType.
void BooleanVariantType::destroy(const VariantData &/* value */) const {}

// Inherited from VariantType.
void BooleanVariantType::copy(VariantData &destination, const VariantData &source) const
{
    destination.Boolean = source.Boolean;
}

// Inherited from VariantType.
void BooleanVariantType::move(VariantData &destination, VariantData &&source) const
{
    destination.Boolean = source.Boolean;
}

// Inherited from VariantType.
bool BooleanVariantType::read(IStream *input, VariantData &destination) const
{
    uint8_t scalar;
    bool isOK = false;

    if (input->tryRead(&scalar, sizeof(scalar)))
    {
        destination.Boolean = (scalar != 0);
        isOK = true;
    }

    return isOK;
}

// Inherited from VariantType.
bool BooleanVariantType::write(IStream *output, const VariantData &source) const
{
    uint8_t scalar = source.Boolean ? 0xFF : 0x00;

    return output->tryWrite(&scalar, sizeof(scalar));
}

// Inherited from VariantType.
void BooleanVariantType::toString(const FormatInfo &format,
                                  const VariantData &value,
                                  std::string &buffer) const
{
    const std::string_view &text = value.Boolean ? TrueText : FalseText;

    size_t fieldLength = std::max(toSize(format.getMinimumFieldWidth()),
                                  text.length());

    size_t padding = fieldLength = text.length();

    if (format.isRightAligned() && (padding > 0))
    {
        buffer.append(padding, ' ');
    }

    buffer.append(text);

    if ((format.isRightAligned() == false) && (padding > 0))
    {
        buffer.append(padding, ' ');
    }
}

// Inherited from VariantType.
bool BooleanVariantType::tryParse(const LocaleInfo & /* format */,
                                  utf8_cptr_t source, size_t &length,
                                  VariantData &value) const
{
    size_t index = 0;

    // Skip leading white space.
    while((index < length) && (std::isspace(source[index]) != 0))
    {
        ++index;
    }

    size_t charsLeft = length - index;
    bool hasValue = false;

    if ((charsLeft >= TrueText.length()) &&
        (compareBoundedStringsIgnoreCase(TrueText.data(), source + index,
                                         TrueText.length()) == 0))
    {
        hasValue = true;
        length = index + TrueText.length();
        value.Boolean = true;
    }
    else if ((charsLeft >= FalseText.length()) &&
             (compareBoundedStringsIgnoreCase(FalseText.data(), source + index,
                                              FalseText.length()) == 0))
    {
        hasValue = true;
        length = index + FalseText.length();
        value.Boolean = false;
    }

    return hasValue;
}

////////////////////////////////////////////////////////////////////////////////
// CharacterVariantType Member Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets a singleton instance of an object describing a character
//! variant type.
const CharacterVariantType *CharacterVariantType::getInstance()
{
    static CharacterVariantType instance;

    return &instance;
}

//! @brief Supports the Variant template initialiser constructor.
void CharacterVariantType::create(VariantData &variant, char32_t value) const
{
    variant.Character = value;
}

//! @brief Supports the template Variant::get() template function.
char32_t CharacterVariantType::getValue(const VariantData &variant) const
{
    return variant.Character;
}

//! @brief Supports the template Variant::getPtr() template function.
char32_t *CharacterVariantType::getPtr(VariantData &variant) const
{
    return &variant.Character;
}

//! @brief Supports the template Variant::getPtr() template function.
const char32_t *CharacterVariantType::getPtr(const VariantData &variant) const
{
    return &variant.Character;
}

//! @brief Supports the template Variant::getPtr() template function.
char32_t &CharacterVariantType::getRef(VariantData &variant) const
{
    return variant.Character;
}

//! @brief Supports the template Variant::getPtr() template function.
const char32_t &CharacterVariantType::getRef(const VariantData &variant) const
{
    return variant.Character;
}

// Inherited from VariantType.
const char *CharacterVariantType::getName() const { return "Character"; }

// Inherited from VariantType.
void CharacterVariantType::destroy(const VariantData & /* value */) const {}

// Inherited from VariantType.
void CharacterVariantType::copy(VariantData &destination, const VariantData &source) const
{
    destination.Character = source.Character;
}

// Inherited from VariantType.
void CharacterVariantType::move(VariantData &destination, VariantData &&source) const
{
    destination.Character = source.Character;
}

// Inherited from VariantType.
bool CharacterVariantType::read(IStream *input, VariantData &destination) const
{
    // Read in the character as UTF-8-encoded bytes.
    Utf::FromUtf8Converter converter;
    uint8_t currentByte;
    bool hasError = false;
    bool hasValue = false;

    while ((hasValue == false) && (hasError == false) &&
           input->tryRead(&currentByte, 1))
    {
        // Read the lead byte.
        hasValue = converter.tryConvert(currentByte,
                                        destination.Character,
                                        hasError);
    }

    return hasValue;
}

// Inherited from VariantType.
bool CharacterVariantType::write(IStream *output, const VariantData &source) const
{
    // Write the character encoded as UTF-8.
    Utf::ToUtf8Converter converter;
    const size_t BufferSize = 8;
    size_t length = converter.setCodePoint(source.Character);
    bool isOK = false;

    if (length < BufferSize)
    {
        uint8_t buffer[BufferSize];
        size_t index;

        for (index = 0; converter.tryGetNextByte(buffer[index]); ++index)
        {
            // Nothing to do.
        }

        isOK = output->tryWrite(buffer, index);
    }

    return isOK;
}

// Inherited from VariantType.
void CharacterVariantType::toString(const FormatInfo &format, const VariantData &value,
                                    std::string &buffer) const
{
    Utf::ToUtf8Converter converter;

    if (converter.setCodePoint(value.Character) > 0)
    {
        uint8_t nextByte;

        if (format.getMinimumFieldWidth() > 1)
        {
            size_t padding = toSize(format.getMinimumFieldWidth()) - 1;

            // Format with padding.
            if (format.isRightAligned())
            {
                buffer.append(padding, ' ');
            }

            while(converter.tryGetNextByte(nextByte))
            {
                buffer.push_back(static_cast<char>(nextByte));
            }

            if (format.isRightAligned() == false)
            {
                buffer.append(padding, ' ');
            }
        }
        else
        {
            while(converter.tryGetNextByte(nextByte))
            {
                buffer.push_back(static_cast<char>(nextByte));
            }
        }
    }
}

// Inherited from VariantType.
bool CharacterVariantType::tryParse(const LocaleInfo & /* format */,
                                    utf8_cptr_t source, size_t &length,
                                    VariantData &value) const
{
    Utf::FromUtf8Converter converter;

    size_t index;
    bool isOK = false;
    bool hasError = false;

    for (index = 0; (isOK == false) && (hasError == false) && (index < length); ++index)
    {
        isOK = converter.tryConvert(static_cast<uint8_t>(source[index]),
                                    value.Character, hasError);
    }

    if (isOK)
    {
        length = index;
    }

    return isOK;
}

////////////////////////////////////////////////////////////////////////////////
// PointerVariantType Member Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the singleton instance of the VariantType object representing
//! raw pointer objects.
const PointerVariantType *PointerVariantType::getInstance()
{
    static PointerVariantType instance;

    return &instance;
}

//! @brief Provides support for the Variant template initialiser constructor.
void PointerVariantType::create(VariantData &variant, const void *value) const
{
    variant.Uintptr = reinterpret_cast<uintptr_t>(value);
}

//! @brief Provides support for the Variant::get() template member function.
void *PointerVariantType::getValue(const VariantData &variant) const
{
    return variant.Pointer;
}

//! @brief Provides support for the Variant::getPtr() template member function.
void *PointerVariantType::getPtr(VariantData &variant) const
{
    return variant.Pointer;
}

//! @brief Provides support for the Variant::getPtr() template member function.
const void *PointerVariantType::getPtr(const VariantData &variant) const
{
    return variant.Pointer;
}

// Inherited from VariantType.
const char *PointerVariantType::getName() const { return "Pointer"; }

// Inherited from VariantType.
void PointerVariantType::destroy(const VariantData & /* value */) const { }

// Inherited from VariantType.
void PointerVariantType::copy(VariantData &destination, const VariantData &source) const
{
    destination.Pointer = source.Pointer;
}

// Inherited from VariantType.
void PointerVariantType::move(VariantData &destination, VariantData &&source) const
{
    destination.Pointer = source.Pointer;
    source.Pointer = nullptr;
}

// Inherited from VariantType.
bool PointerVariantType::read(IStream *input, VariantData &destination) const
{
    return input->tryRead(&destination.Pointer, sizeof(destination.Pointer));
}

// Inherited from VariantType.
bool PointerVariantType::write(IStream *output, const VariantData &source) const
{
    return output->tryWrite(&source.Pointer, sizeof(source.Pointer));
}

// Inherited from VariantType.
void PointerVariantType::toString(const FormatInfo &format, const VariantData &value,
                                  std::string &buffer) const
{
    FormatInfo fixedFormat(format);
    fixedFormat.setRadix(16);
    fixedFormat.enableForcedSign(false);
    fixedFormat.setMinimumWholeDigits(static_cast<uint16_t>(sizeof(void *) * 2));
    fixedFormat.setRequiredFractionDigits(0);
    fixedFormat.setRequiredSignificantFigures(0);
    fixedFormat.setThousandSeparator(String::Empty);

    buffer.push_back('0');
    buffer.push_back('x');

    appendValue(fixedFormat, buffer, value.Uintptr);
}

// Inherited from VariantType.
bool PointerVariantType::tryParse(const LocaleInfo &format, utf8_cptr_t source,
                                  size_t &length, VariantData &value) const
{
    ScalarParser parser(format);
    parser.enableRadixPrefix(true);
    parser.setPreferredRadix(16);
    parser.enableFraction(false);
    parser.enableExponent(false);
    parser.enableSign(false);

    bool isOK = true;

    for (size_t index = 0; isOK && (index < length); )
    {
        if (parser.tryProcessCharacter(source[index]))
        {
            ++index;
        }
        else
        {
            // We've reached the end of the value.
            length = index;
            isOK = false;
        }
    }

    return parser.tryGetValue(value.Uintptr);
}

////////////////////////////////////////////////////////////////////////////////
// StringVariantType Member Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the singleton object which describes Variant values with an
//! Ag::String type.
const StringVariantType *StringVariantType::getInstance()
{
    static StringVariantType instance;

    return &instance;
}

//! @brief Supports the Variant template initialiser constructor.
void StringVariantType::create(VariantData &variant, const Ag::String &value) const
{
    variant.Pointer = new Ag::String(value);
}

//! @brief Supports the Variant::get() member function.
Ag::String StringVariantType::getValue(const VariantData &variant) const
{
    return *static_cast<Ag::String *>(variant.Pointer);
}

//! @brief Supports the Variant::getPtr() member function.
Ag::String *StringVariantType::getPtr(VariantData &variant) const
{
    return static_cast<Ag::String *>(variant.Pointer);
}

//! @brief Supports the Variant::getPtr() member function.
const Ag::String *StringVariantType::getPtr(const VariantData &variant) const
{
    return static_cast<const Ag::String *>(variant.Pointer);
}

//! @brief Supports the Variant::getRef() member function.
Ag::String &StringVariantType::getRef(VariantData &variant) const
{
    return *static_cast<Ag::String *>(variant.Pointer);
}

//! @brief Supports the Variant::getRef() member function.
const Ag::String &StringVariantType::getRef(const VariantData &variant) const
{
    return *static_cast<const Ag::String *>(variant.Pointer);
}

// Inherited from VariantType.
const char *StringVariantType::getName() const { return "String"; }

// Inherited from VariantType.
void StringVariantType::destroy(const VariantData &value) const
{
    if (value.Pointer != nullptr)
    {
        Ag::String *str = static_cast<Ag::String *>(value.Pointer);

        delete str;
    }
}

// Inherited from VariantType.
void StringVariantType::copy(VariantData &destination, const VariantData &source) const
{
    if (source.Pointer != nullptr)
    {
        const Ag::String *str = static_cast<const Ag::String *>(source.Pointer);

        destination.Pointer = new Ag::String(*str);
    }
}

// Inherited from VariantType.
void StringVariantType::move(VariantData &destination, VariantData &&source) const
{
    destination.Pointer = source.Pointer;
    source.Pointer = nullptr;
}

// Inherited from VariantType.
bool StringVariantType::read(IStream *input, VariantData &destination) const
{
    size_t length;
    bool isOK = false;

    if (IStream::tryReadLength(input, length))
    {
        std::vector<char> buffer;

        if (length > 0)
        {
            buffer.resize(length, '\0');

            if (input->tryRead(buffer.data(), length))
            {
                destination.Pointer = new Ag::String(buffer.data(), length);
                isOK = true;
            }
        }
        else
        {
            // The string is empty.
            isOK = true;
            destination.Pointer = new Ag::String();
        }
    }

    return isOK;
}

// Inherited from VariantType.
bool StringVariantType::write(IStream *output, const VariantData &source) const
{
    const Ag::String *str = static_cast<const Ag::String *>(source.Pointer);

    size_t length = str->getUtf8Length();
    bool isOK = false;

    if (IStream::tryWriteLength(output, length))
    {
        if (length > 0)
        {
            // Write out the text as UTF-8 encoded bytes.
            isOK = output->tryWrite(str->getUtf8Bytes(), length);
        }
        else
        {
            // The string was empty.
            isOK = true;
        }
    }

    return isOK;
}

// Inherited from VariantType.
void StringVariantType::toString(const FormatInfo &format,
                                 const VariantData &value,
                                 std::string &buffer) const
{
    const Ag::String *str = static_cast<const Ag::String *>(value.Pointer);

    // TODO: If the Required Significant Digits field is set, truncate the
    // string to that many printable characters.

    if (format.getMinimumFieldWidth() > 0)
    {
        // NOTE: Calculate field width and padding based on the count of
        // printable characters in the string.
        size_t printableLength = str->getPrintLength();

        size_t fieldLength = std::max(toSize(format.getMinimumFieldWidth()),
                                      printableLength);

        size_t padding = fieldLength - printableLength;

        if (padding > 0)
        {
            if (format.isRightAligned())
            {
                buffer.append(padding, ' ');
            }

            buffer.append(str->getUtf8Bytes(), str->getUtf8Length());

            if (format.isRightAligned() == false)
            {
                buffer.append(padding, ' ');
            }
        }
        else
        {
            // Write the entire string.
            buffer.append(str->getUtf8Bytes(), str->getUtf8Length());
        }
    }
    else
    {
        // Write the entire string.
        buffer.append(str->getUtf8Bytes(), str->getUtf8Length());
    }
}

// Inherited from VariantType.
bool StringVariantType::tryParse(const LocaleInfo & /* format */,
                                 utf8_cptr_t source, size_t &length,
                                 VariantData &value) const
{
    value.Pointer = new Ag::String(source, length);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the map of functions which convert variants between types.
VariantConversionMap &getConversionMap()
{
    static std::map<VariantTypePair, VariantConversionFn> conversionsByTypes;

    if (conversionsByTypes.empty())
    {
        initialiseStandardConversions(conversionsByTypes);
    }

    return conversionsByTypes;
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


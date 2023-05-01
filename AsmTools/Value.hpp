//! @file Value.hpp
//! @brief The declaration of an object representing a value calculated by
//! evaluating an Expr.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_VALUE_HPP__
#define __ASM_TOOLS_VALUE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <Ag/Core/String.hpp>

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Expresses the data type of a value calculated by evaluating an Expr.
enum class DataType
{
    Unknown,
    Uint32,
    Int32,
    Uint64,
    Int64,
    Float,
    Double,
    ExtendedReal,
    Character,
    String,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing a value calculated by evaluating an Expr.
class Value
{
public:
    // Construction/Destruction
    Value();
    Value(int32_t value);
    Value(uint32_t value);
    Value(int64_t value);
    Value(uint64_t value);
    Value(char32_t value);
    Value(float value);
    Value(double value);
    Value(Ag::string_cref_t value);
    ~Value() = default;

    // Accessors
    bool isNull() const;
    DataType getDataType() const;
    int32_t asInt32() const;
    uint32_t asUint32() const;
    int64_t asInt64() const;
    uint64_t asUint64() const;
    float asFloat() const;
    double asDouble() const;
    char32_t asCharacter() const;
    Ag::string_cref_t asString() const;

    // Operations
    void setNull();
    bool tryConvert(DataType targetType, Value &result) const;
private:
    // Internal Types
    union Primitive
    {
        Primitive();

        int32_t Int32;
        uint32_t Uint32;
        int64_t Int64;
        uint64_t Uint64;
        float Float;
        double Double;
        uint32_t Extended[3];
        char32_t Character;
    };

    // Internal Functions

    // Internal Fields
    DataType _nativeType;
    Primitive _data;
    Ag::String _text;
};

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

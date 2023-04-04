//! @file Value.cpp
//! @brief The definition of an object representing a value calculated by
//! evaluating an Expr.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"
#include "Value.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#define VERIFY_TYPE(x) if (_nativeType != (DataType::x)) throw BadCastException(# x);

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// Value Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Initialises the object to a value of zero in all types.
Value::Primitive::Primitive()
{
    std::fill_n(Extended, arraySize(Extended), 0);
}

//! @brief Creates an empty value object.
Value::Value() :
    _nativeType(DataType::Unknown)
{
}

//! @brief Creates an initialised value object.
//! @param[in] value The concrete value to initialise the object with.
Value::Value(int32_t value) :
    _nativeType(DataType::Int32)
{
    _data.Int32 = value;
}

//! @brief Creates an initialised value object.
//! @param[in] value The concrete value to initialise the object with.
Value::Value(uint32_t value) :
    _nativeType(DataType::Uint32)
{
    _data.Uint32 = value;
}

//! @brief Creates an initialised value object.
//! @param[in] value The concrete value to initialise the object with.
Value::Value(int64_t value) :
    _nativeType(DataType::Int64)
{
    _data.Int64 = value;
}

//! @brief Creates an initialised value object.
//! @param[in] value The concrete value to initialise the object with.
Value::Value(uint64_t value) :
    _nativeType(DataType::Uint64)
{
    _data.Uint64 = value;
}

//! @brief Creates an initialised value object.
//! @param[in] value The concrete value to initialise the object with.
Value::Value(char32_t value) :
    _nativeType(DataType::Character)
{
    _data.Character = value;
}

//! @brief Creates an initialised value object.
//! @param[in] value The concrete value to initialise the object with.
Value::Value(float value) :
    _nativeType(DataType::Float)
{
    _data.Float = value;
}

//! @brief Creates an initialised value object.
//! @param[in] value The concrete value to initialise the object with.
Value::Value(double value) :
    _nativeType(DataType::Double)
{
    _data.Double = value;
}

//! @brief Creates an initialised value object.
//! @param[in] value The concrete value to initialise the object with.
Value::Value(string_cref_t value) :
    _nativeType(DataType::String),
    _text(value)
{
}

//! @brief Determines with the object has an assigned value.
//! @retval true The object has no assigned value.
//! @retval false The object has a concrete value assigned.
bool Value::isNull() const
{
    return _nativeType == DataType::Unknown;
}

//! @brief Gets the data type of the value stored within the object.
DataType Value::getDataType() const
{
    return _nativeType;
}

//! @brief Gets the 32-bit signed integer the object wraps with no conversion.
//! @throws BadCastException If the object doesn't contain a value of the
//! expected data type.
int32_t Value::asInt32() const
{
    VERIFY_TYPE(Int32);
    return _data.Int32;
}

//! @brief Gets the 32-bit unsigned integer the object wraps with no conversion.
//! @throws BadCastException If the object doesn't contain a value of the
//! expected data type.
uint32_t Value::asUint32() const
{
    VERIFY_TYPE(Uint32);
    return _data.Uint32;
}

//! @brief Gets the 64-bit signed integer the object wraps with no conversion.
//! @throws BadCastException If the object doesn't contain a value of the
//! expected data type.
int64_t Value::asInt64() const
{
    VERIFY_TYPE(Int64);
    return _data.Int64;
}

//! @brief Gets the 64-bit unsigned integer the object wraps with no conversion.
//! @throws BadCastException If the object doesn't contain a value of the
//! expected data type.
uint64_t Value::asUint64() const
{
    VERIFY_TYPE(Uint64);
    return _data.Uint64;
}

//! @brief Gets the 32-bit real value the object wraps with no conversion.
//! @throws BadCastException If the object doesn't contain a value of the
//! expected data type.
float Value::asFloat() const
{
    VERIFY_TYPE(Float);
    return _data.Float;
}

//! @brief Gets the 64-bit real value the object wraps with no conversion.
//! @throws BadCastException If the object doesn't contain a value of the
//! expected data type.
double Value::asDouble() const
{
    VERIFY_TYPE(Double);
    return _data.Double;
}

//! @brief Gets the character value the object wraps with no conversion.
//! @throws BadCastException If the object doesn't contain a value of the
//! expected data type.
char32_t Value::asCharacter() const
{
    VERIFY_TYPE(Character);
    return _data.Character;
}

//! @brief Gets the string value the object wraps with no conversion.
//! @throws BadCastException If the object doesn't contain a value of the
//! expected data type.
string_cref_t Value::asString() const
{
    VERIFY_TYPE(String);
    return _text;
}

//! @brief Resets the object to have no value.
void Value::setNull()
{
    _nativeType = DataType::Unknown;
    _data = Primitive();
    _text = String::Empty;
}

//! @brief Attempts to convert the current value to a value of a specified
//! data type.
//! @param[in] targetType The type to convert the current value to.
//! @param[out] result Receives the converted value if a conversion is possible.
//! @retval true The conversion was performed and result updated to an
//! equivalent value with the target data type.
//! @retval false The conversion was not possible given the current object's
//! value and the data types involved.
bool Value::tryConvert(DataType targetType, Value &result) const
{
    bool canConvert = true;

    if (_nativeType == targetType)
    {
        result = *this;
    }
    else
    {
        switch (_nativeType)
        {
        ///////////////////////////////////////////////////////////////////////
        // Int 32 Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::Int32:
            switch (targetType)
            {
            case DataType::Uint32:
                // Simply convert.
                result = Value(_data.Uint32);
                break;

            case DataType::Int64:
                result = Value(static_cast<int64_t>(_data.Int32)); break;

            case DataType::Uint64:
                // Sign extend and convert.
                result = Value(static_cast<uint64_t>(static_cast<int64_t>(_data.Int32)));
                break;

            case DataType::Float:
                result = Value(static_cast<float>(_data.Int32)); break;

            case DataType::Double:
                result = Value(static_cast<float>(_data.Int32)); break;

            case DataType::ExtendedReal:
                // TODO: Support 96-bit real values.
                canConvert = false;
                break;

            case DataType::Character:
                if ((_data.Int32 >= 0) && (_data.Int32 <= CodePointMax))
                {
                    result = _data.Character;
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::String: {
                std::string buffer;
                appendValue(LocaleInfo::getNeutral(), buffer, _data.Int32);
                result = Value(buffer);
            } break;

            default: canConvert = false; break;
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        // Uint 32 Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::Uint32:
            switch (targetType)
            {
            case DataType::Int32:
                // Simply convert.
                result = Value(_data.Int32);
                break;

            case DataType::Int64: result = Value(static_cast<int64_t>(_data.Uint32)); break;
            case DataType::Uint64: result = Value(static_cast<uint64_t>(_data.Uint32)); break;
            case DataType::Float: result = Value(static_cast<float>(_data.Uint32)); break;
            case DataType::Double: result = Value(static_cast<double>(_data.Uint32)); break;
            case DataType::ExtendedReal: canConvert = false; break;
            case DataType::Character:
                if (_data.Uint32 <= CodePointMax)
                {
                    result = Value(static_cast<char32_t>(_data.Uint32));
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::String:
            {
                std::string buffer;
                appendValue(LocaleInfo::getNeutral(), buffer, _data.Uint32);
                result = Value(buffer);
            } break;

            default: canConvert = false; break;
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        // Int 64 Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::Int64:
            switch (targetType)
            {
            case DataType::Int32:
                if ((_data.Int64 >= INT32_MIN) && (_data.Int64 <= INT32_MAX))
                {
                    result = Value(static_cast<int32_t>(_data.Int64));
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::Uint32:
                if (_data.Int64 <= UINT32_MAX)
                {
                    result = Value(static_cast<uint32_t>(_data.Int64));
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::Uint64:
                // Simply convert.
                result = Value(_data.Int64);
                break;

            case DataType::Float: result = Value(static_cast<float>(_data.Int64)); break;
            case DataType::Double: result = Value(static_cast<double>(_data.Int64)); break;
            case DataType::ExtendedReal: canConvert = false; break;
            case DataType::Character:
                if ((_data.Int64 >= 0) && (_data.Int64 <= CodePointMax))
                {
                    result = Value(static_cast<char32_t>(_data.Int64));
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::String:{
                std::string buffer;
                appendValue(LocaleInfo::getNeutral(), buffer, _data.Int64);
                result = Value(buffer);
            } break;

            default: canConvert = false; break;
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        // Uint 64 Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::Uint64:
            switch (targetType)
            {
            case DataType::Int32:
                if (_data.Uint64 <= INT32_MAX)
                {
                    result = Value(static_cast<int32_t>(_data.Uint64));
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::Uint32:
                if (_data.Uint64 <= UINT32_MAX)
                {
                    result = Value(static_cast<uint32_t>(_data.Uint64)); break;
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::Int64:
                // Simply convert.
                result = Value(_data.Int64);
                break;

            case DataType::Float: result = Value(static_cast<float>(_data.Uint64)); break;
            case DataType::Double: result = Value(static_cast<double>(_data.Uint64)); break;
            case DataType::ExtendedReal: canConvert = false; break;
            case DataType::Character:
                if (_data.Uint64 <= CodePointMax)
                {
                    result = Value(static_cast<char32_t>(_data.Uint64));
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::String:
            {
                std::string buffer;
                appendValue(LocaleInfo::getNeutral(), buffer, _data.Uint64);
                result = Value(buffer);
            } break;

            default: canConvert = false; break;
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        // Float Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::Float:
            switch (targetType)
            {
            case DataType::Int32: result = Value(static_cast<int32_t>(std::rint(_data.Float))); break;
            case DataType::Uint32: result = Value(static_cast<uint32_t>(std::rint(_data.Float))); break;
            case DataType::Int64: result = Value(static_cast<int64_t>(std::rint(_data.Float))); break;
            case DataType::Uint64: result = Value(static_cast<uint64_t>(std::rint(_data.Float))); break;
            case DataType::Double: result = Value(static_cast<double>(_data.Float)); break;

            case DataType::String:
            {
                std::string buffer;
                appendValue(LocaleInfo::getNeutral(), buffer, _data.Float);
                result = Value(buffer);
            } break;

            case DataType::ExtendedReal:
            case DataType::Character:
            default: canConvert = false; break;
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        // Double Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::Double:
            switch (targetType)
            {
            case DataType::Int32: result = Value(static_cast<int32_t>(std::rint(_data.Double))); break;
            case DataType::Uint32: result = Value(static_cast<uint32_t>(std::rint(_data.Double))); break;
            case DataType::Int64: result = Value(static_cast<int64_t>(std::rint(_data.Double))); break;
            case DataType::Uint64: result = Value(static_cast<uint64_t>(std::rint(_data.Double))); break;
            case DataType::Float: result = Value(static_cast<float>(_data.Double)); break;

            case DataType::String:
            {
                std::string buffer;
                appendValue(LocaleInfo::getNeutral(), buffer, _data.Float);
                result = Value(buffer);
            } break;

            case DataType::ExtendedReal:
            case DataType::Character:
            default: canConvert = false; break;
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        // Extended Real Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::ExtendedReal: canConvert = false; break;

        ///////////////////////////////////////////////////////////////////////
        // Character Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::Character:
            switch (targetType)
            {
            case DataType::Int32: result = Value(_data.Int32); break;
            case DataType::Uint32: result = Value(_data.Uint32); break;
            case DataType::Int64: result = Value(static_cast<int64_t>(_data.Character)); break;
            case DataType::Uint64: result = Value(static_cast<uint64_t>(_data.Character)); break;
            case DataType::Float: result = Value(static_cast<float>(_data.Character)); break;
            case DataType::Double: result = Value(static_cast<double>(_data.Character)); break;
            case DataType::String: result = Value(String(&_data.Character, 1)); break;

            case DataType::ExtendedReal:
            default: canConvert = false; break;
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        // String Conversions
        ///////////////////////////////////////////////////////////////////////
        case DataType::String:
            switch (targetType)
            {
            case DataType::Int32: {
                int32_t scalar;
                if (_text.tryParseScalar(scalar))
                {
                    result = Value(scalar);
                }
                else
                {
                    canConvert = false;
                }
            } break;

            case DataType::Uint32: {
                uint32_t scalar;
                if (_text.tryParseScalar(scalar))
                {
                    result = Value(scalar);
                }
                else
                {
                    canConvert = false;
                }
            } break;

            case DataType::Int64: {
                int64_t scalar;
                if (_text.tryParseScalar(scalar))
                {
                    result = Value(scalar);
                }
                else
                {
                    canConvert = false;
                }
            } break;

            case DataType::Uint64: {
                uint64_t scalar;
                if (_text.tryParseScalar(scalar))
                {
                    result = Value(scalar);
                }
                else
                {
                    canConvert = false;
                }
            } break;

            case DataType::Character:
                if (_text.getUtf32Length() == 1)
                {
                    result = Value(*_text.begin());
                }
                else
                {
                    canConvert = false;
                }
                break;

            case DataType::Float:
            case DataType::Double:
            case DataType::ExtendedReal:
            default: canConvert = false; break;
            }
            break;

        default: canConvert = false; break;
        }
    }

    return canConvert;
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file AsmTools/Expr.cpp
//! @brief The definition of a set of classes representing expressions which
//! can be evaluated to calculate concrete values.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cmath>

#include "Expr.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Variant.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Applies the unary negation operator to a sub-expression.
//! @param[in] input The value to apply the operator to.
//! @param[out] error Receives an error message if the application fails.
//! @return The result of applying the operator, or an empty value.
Value unaryNegate(const Value &input, Ag::string_ref_t error)
{
    Value result;

    switch (input.getDataType())
    {
        case DataType::Unknown: error = "Cannot operate on null value."; break;
        case DataType::Int32: result = -input.asInt32(); break;

        case DataType::Uint32: {
            Value signedValue;

            if (input.tryConvert(DataType::Int32, signedValue))
            {
                result = -signedValue.asInt32();
            }
            else
            {
                error = "Cannot convert to a signed data type.";
            }
        } break;

        case DataType::Int64: result = -input.asInt64(); break;

        case DataType::Uint64: {
            Value signedValue;

            if (input.tryConvert(DataType::Int64, signedValue))
            {
                result = -signedValue.asInt64();
            }
        } break;

        case DataType::Float: result = -input.asFloat(); break;
        case DataType::Double: result = -input.asDouble();  break;
        case DataType::ExtendedReal: error = "The extended real type is not supported."; break;

        case DataType::Character:
        case DataType::String:
        default:
            error = "The negation operator cannot be applied to character types.";
            break;
    }

    return result;
}

//! @brief Applies the unary absolute operator to a sub-expression.
//! @param[in] input The value to apply the operator to.
//! @param[out] error Receives an error message if the application fails.
//! @return The result of applying the operator, or an empty value.
Value unaryAbsolute(const Value &input, Ag::string_ref_t error)
{
    Value result;

    switch (input.getDataType())
    {
    case DataType::Unknown: error = "Cannot operate on null value."; break;
    case DataType::Int32: result = std::abs(input.asInt32()); break;
    case DataType::Uint32: result = input; break;
    case DataType::Int64: result = std::abs(input.asInt64()); break;
    case DataType::Uint64: result = input; break;
    case DataType::Float: result = std::abs(input.asFloat()); break;
    case DataType::Double: result = std::abs(input.asDouble());  break;
    case DataType::ExtendedReal: error = "The extended real type is not supported."; break;
    case DataType::Character:
    case DataType::String:
    default:
        error = "The absolute operator cannot be applied to character types.";
        break;
    }

    return result;
}


//! @brief Applies the unary not operator to a sub-expression.
//! @param[in] input The value to apply the operator to.
//! @param[out] error Receives an error message if the application fails.
//! @return The result of applying the operator, or an empty value.
Value unaryNot(const Value &input, Ag::string_ref_t error)
{
    Value result;

    switch (input.getDataType())
    {
    case DataType::Unknown: error = "Cannot operate on null value."; break;
    case DataType::Int32: result = ~input.asInt32(); break;
    case DataType::Uint32: result = ~input.asUint32(); break;
    case DataType::Int64: result = ~input.asInt64(); break;
    case DataType::Uint64: result = ~input.asUint64(); break;

    case DataType::Float:
    case DataType::Double:
    case DataType::ExtendedReal:
        error = "The logical NOT operator cannot be applied to real data types.";
        break;

    case DataType::Character:
    case DataType::String:
    default:
        error = "The absolute operator cannot be applied to character types.";
        break;
    }

    return result;
}

//! @brief Attempts to convert value data types so that they have a common
//! data type.
//! @param[in,out] lhs The left operand, possibly to be promoted.
//! @param[in,out] rhs The right operand, possibly to be promoted.
//! @return The data type shared by the returned lhs and rhs values, or
//! DataType::Unknown if a common type cannot be found.
DataType promoteToCompatibleTypes(Value &lhs, Value &rhs)
{
    DataType commonType = lhs.getDataType();

    if (lhs.getDataType() != rhs.getDataType())
    {
        Value promoted;

        if (lhs.getDataType() < rhs.getDataType())
        {
            commonType = rhs.getDataType();

            if (lhs.tryConvert(commonType, promoted))
            {
                lhs = promoted;
            }
            else
            {
                commonType = DataType::Unknown;
            }
        }
        else
        {
            commonType = lhs.getDataType();

            if (rhs.tryConvert(commonType, promoted))
            {
                rhs = promoted;
            }
            else
            {
                commonType = DataType::Unknown;
            }
        }
    }

    return commonType;
}

//! @brief Applies the addition operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value addition(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    DataType commonType = promoteToCompatibleTypes(lhs, rhs);
    Value result;

    switch (commonType)
    {
    case DataType::Int32: result = lhs.asInt32() + rhs.asInt32(); break;
    case DataType::Uint32: result = lhs.asUint32() + rhs.asUint32(); break;
    case DataType::Int64: result = lhs.asInt64() + rhs.asInt64(); break;
    case DataType::Uint64: result = lhs.asUint64() + rhs.asUint64(); break;
    case DataType::Float: result = lhs.asFloat() + rhs.asFloat(); break;
    case DataType::Double: result = lhs.asDouble() + rhs.asDouble();  break;
    case DataType::ExtendedReal: error = "The extended real type is not supported."; break;
    case DataType::Character: {
        char32_t buffer[2];
        buffer[0] = lhs.asCharacter();
        buffer[1] = rhs.asCharacter();

        result = Value(Ag::String(buffer, 2));
    } break;

    case DataType::String: {
        result = lhs.asString() + rhs.asString();
    } break;

    case DataType::Unknown:
    default:
        error = "Cannot apply the '+' operator to incompatible data types.";
        break;
    }

    return result;
}

//! @brief Applies the subtraction operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value subtraction(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    DataType commonType = promoteToCompatibleTypes(lhs, rhs);
    Value result;

    switch (commonType)
    {
    case DataType::Int32: result = lhs.asInt32() - rhs.asInt32(); break;
    case DataType::Uint32: result = lhs.asUint32() - rhs.asUint32(); break;
    case DataType::Int64: result = lhs.asInt64() - rhs.asInt64(); break;
    case DataType::Uint64: result = lhs.asUint64() - rhs.asUint64(); break;
    case DataType::Float: result = lhs.asFloat() - rhs.asFloat(); break;
    case DataType::Double: result = lhs.asDouble() - rhs.asDouble();  break;
    case DataType::ExtendedReal: error = "The extended real type is not supported."; break;

    case DataType::Character:
        error = "Cannot apply the '-' operator to character types.";
        break;

    case DataType::String:
        error = "Cannot apply the '-' operator to string types.";
        break;

    case DataType::Unknown:
    default:
        error = "Cannot apply the '-' operator to incompatible data types.";
        break;
    }

    return result;
}

//! @brief Applies the multiplication operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value multiplication(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    DataType commonType = promoteToCompatibleTypes(lhs, rhs);
    Value result;

    switch (commonType)
    {
    case DataType::Int32: result = lhs.asInt32() * rhs.asInt32(); break;
    case DataType::Uint32: result = lhs.asUint32() * rhs.asUint32(); break;
    case DataType::Int64: result = lhs.asInt64() * rhs.asInt64(); break;
    case DataType::Uint64: result = lhs.asUint64() * rhs.asUint64(); break;
    case DataType::Float: result = lhs.asFloat() * rhs.asFloat(); break;
    case DataType::Double: result = lhs.asDouble() * rhs.asDouble();  break;
    case DataType::ExtendedReal: error = "The extended real type is not supported."; break;

    case DataType::Character:
        error = "Cannot apply the '*' operator to character types.";
        break;

    case DataType::String:
        error = "Cannot apply the '*' operator to string types.";
        break;

    case DataType::Unknown:
    default:
        error = "Cannot apply the '*' operator to incompatible data types.";
        break;
    }

    return result;
}

//! @brief Applies the division operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value division(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    DataType commonType = promoteToCompatibleTypes(lhs, rhs);
    Value result;
    bool isDivByZero = true;

    switch (commonType)
    {
    case DataType::Int32:
        if (rhs.asInt32() != 0)
        {
            result = lhs.asInt32() / rhs.asInt32();
            isDivByZero = false;
        }
        break;

    case DataType::Uint32:
        if (rhs.asUint32() != 0)
        {
            result = lhs.asUint32() / rhs.asUint32();
            isDivByZero = false;
        }
        break;

    case DataType::Int64:
        if (rhs.asInt64() != 0)
        {
            result = lhs.asInt64() / rhs.asInt64();
            isDivByZero = false;
        }
        break;

    case DataType::Uint64:
        if (rhs.asUint64() != 0)
        {
            result = lhs.asUint64() / rhs.asUint64();
            isDivByZero = false;
        }
        break;

    case DataType::Float:
        if (rhs.asFloat() != 0.0f)
        {
            result = lhs.asFloat() / rhs.asFloat();
            isDivByZero = false;
        }
        break;

    case DataType::Double:
        if (rhs.asDouble() != 0.0)
        {
            result = lhs.asDouble() / rhs.asDouble();
            isDivByZero = false;
        }
        break;

    case DataType::ExtendedReal: error = "The extended real type is not supported."; break;

    case DataType::Character:
        error = "Cannot apply the '/' operator to character types.";
        isDivByZero = false;
        break;

    case DataType::String:
        error = "Cannot apply the '/' operator to string types.";
        isDivByZero = false;
        break;

    case DataType::Unknown:
    default:
        error = "Cannot apply the '/' operator to incompatible data types.";
        isDivByZero = false;
        break;
    }

    if (isDivByZero)
    {
        error = "Division by zero.";
    }

    return result;
}

//! @brief Applies the modulus operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value modulus(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    DataType commonType = promoteToCompatibleTypes(lhs, rhs);
    Value result;

    switch (commonType)
    {
    case DataType::Int32: result = lhs.asInt32() % rhs.asInt32(); break;
    case DataType::Uint32: result = lhs.asUint32() % rhs.asUint32(); break;
    case DataType::Int64: result = lhs.asInt64() % rhs.asInt64(); break;
    case DataType::Uint64: result = lhs.asUint64() % rhs.asUint64(); break;

    case DataType::Float:
    case DataType::Double:
    case DataType::ExtendedReal:
        error = "Cannot apply the 'MOD' operator to real numeric types.";
        break;

    case DataType::Character:
        error = "Cannot apply the 'MOD' operator to character types.";
        break;

    case DataType::String:
        error = "Cannot apply the 'MOD' operator to string types.";
        break;

    case DataType::Unknown:
    default:
        error = "Cannot apply the 'MOD' operator to incompatible data types.";
        break;
    }

    return result;
}

//! @brief Applies the logic left shift operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value logicShiftLeft(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    Value shiftValue;
    Value result;

    if (rhs.tryConvert(DataType::Int32, shiftValue))
    {
        DataType sourceType = lhs.getDataType();
        int32_t shift = shiftValue.asInt32();

        if ((sourceType == DataType::Int32) ||
            (sourceType == DataType::Uint32))
        {
            uint32_t value =
                (sourceType == DataType::Int32) ? static_cast<uint32_t>(lhs.asInt32()) :
                                                  lhs.asUint32();

            if (shift > 0)
            {
                value <<= shift;
            }
            else if (shift < 0)
            {
                value >>= -shift;
            }

            result = value;
        }
        else if ((sourceType == DataType::Int64) ||
                 (sourceType == DataType::Uint64))
        {
            uint64_t value =
                (sourceType == DataType::Int64) ? static_cast<uint64_t>(lhs.asInt64()) :
                                                  lhs.asUint64();

            if (shift > 0)
            {
                value <<= shift;
            }
            else if (shift < 0)
            {
                value >>= -shift;
            }

            result = value;
        }
        else
        {
            error = "Incompatible data type to apply the left shift operator to.";
        }
    }
    else
    {
        error = "Incompatible data type for a shift value.";
    }

    return result;
}

//! @brief Applies the logic right shift operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value logicShiftRight(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    Value shiftValue;
    Value result;

    if (rhs.tryConvert(DataType::Int32, shiftValue))
    {
        DataType sourceType = lhs.getDataType();
        int32_t shift = shiftValue.asInt32();

        if ((sourceType == DataType::Int32) ||
            (sourceType == DataType::Uint32))
        {
            uint32_t value =
                (sourceType == DataType::Int32) ? static_cast<uint32_t>(lhs.asInt32()) :
                                                  lhs.asUint32();

            if (shift > 0)
            {
                value >>= shift;
            }
            else if (shift < 0)
            {
                value <<= -shift;
            }

            result = value;
        }
        else if ((sourceType == DataType::Int64) ||
                 (sourceType == DataType::Uint64))
        {
            uint64_t value =
                (sourceType == DataType::Int64) ? static_cast<uint64_t>(lhs.asInt64()) :
                                                  lhs.asUint64();

            if (shift > 0)
            {
                value >>= shift;
            }
            else if (shift < 0)
            {
                value <<= -shift;
            }

            result = value;
        }
        else
        {
            error = "Incompatible data type to apply the right shift operator to.";
        }
    }
    else
    {
        error = "Incompatible data type for a shift value.";
    }

    return result;
}

//! @brief Applies the arithmetic right shift operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value arithmeticShiftRight(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    Value shiftValue;
    Value result;

    if (rhs.tryConvert(DataType::Int32, shiftValue))
    {
        DataType sourceType = lhs.getDataType();
        int32_t shift = shiftValue.asInt32();

        if ((sourceType == DataType::Int32) ||
            (sourceType == DataType::Uint32))
        {
            int32_t value =
                (sourceType == DataType::Uint32) ? static_cast<int32_t>(lhs.asUint32()) :
                                                  lhs.asInt32();

            if (shift > 0)
            {
                value >>= shift;
            }
            else if (shift < 0)
            {
                value <<= -shift;
            }

            result = value;
        }
        else if ((sourceType == DataType::Int64) ||
                 (sourceType == DataType::Uint64))
        {
            int64_t value =
                (sourceType == DataType::Uint64) ? static_cast<int64_t>(lhs.asUint64()) :
                                                   lhs.asInt64();

            if (shift > 0)
            {
                value >>= shift;
            }
            else if (shift < 0)
            {
                value <<= -shift;
            }

            result = value;
        }
        else
        {
            error = "Incompatible data type to apply the arithmetic right shift operator to.";
        }
    }
    else
    {
        error = "Incompatible data type for a shift value.";
    }

    return result;
}

//! @brief Applies the bitwise AND operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value bitwiseAnd(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    DataType commonType = promoteToCompatibleTypes(lhs, rhs);
    Value result;

    switch (commonType)
    {
    case DataType::Int32: result = lhs.asInt32() & rhs.asInt32(); break;
    case DataType::Uint32: result = lhs.asUint32() & rhs.asUint32(); break;
    case DataType::Int64: result = lhs.asInt64() & rhs.asInt64(); break;
    case DataType::Uint64: result = lhs.asUint64() & rhs.asUint64(); break;

    case DataType::Float:
    case DataType::Double:
    case DataType::ExtendedReal:
        error = "Cannot apply the 'AND' operator to real type.";
        break;

    case DataType::Character:
        error = "Cannot apply the 'AND' operator to character types.";
        break;

    case DataType::String:
        error = "Cannot apply the 'AND' operator to string types.";
        break;

    case DataType::Unknown:
    default:
        error = "Cannot apply the 'AND' operator to incompatible data types.";
        break;
    }

    return result;
}

//! @brief Applies the bitwise OR operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value bitwiseOr(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    DataType commonType = promoteToCompatibleTypes(lhs, rhs);
    Value result;

    switch (commonType)
    {
    case DataType::Int32: result = lhs.asInt32() | rhs.asInt32(); break;
    case DataType::Uint32: result = lhs.asUint32() | rhs.asUint32(); break;
    case DataType::Int64: result = lhs.asInt64() | rhs.asInt64(); break;
    case DataType::Uint64: result = lhs.asUint64() | rhs.asUint64(); break;

    case DataType::Float:
    case DataType::Double:
    case DataType::ExtendedReal:
        error = "Cannot apply the 'OR' operator to real type.";
        break;

    case DataType::Character:
        error = "Cannot apply the 'OR' operator to character types.";
        break;

    case DataType::String:
        error = "Cannot apply the 'OR' operator to string types.";
        break;

    case DataType::Unknown:
    default:
        error = "Cannot apply the 'OR' operator to incompatible data types.";
        break;
    }

    return result;
}

//! @brief Applies the bitwise XOR operator to two evaluated values.
//! @param[in] lhs The left evaluated operand value.
//! @param[in] rhs The right evaluated operand value.
//! @param[out] error Receives an error if the operator cannot be applied.
//! @return The result of applying the operator, empty if the operation failed.
Value bitwiseXor(Value &lhs, Value &rhs, Ag::string_ref_t error)
{
    DataType commonType = promoteToCompatibleTypes(lhs, rhs);
    Value result;

    switch (commonType)
    {
    case DataType::Int32: result = lhs.asInt32() ^ rhs.asInt32(); break;
    case DataType::Uint32: result = lhs.asUint32() ^ rhs.asUint32(); break;
    case DataType::Int64: result = lhs.asInt64() ^ rhs.asInt64(); break;
    case DataType::Uint64: result = lhs.asUint64() ^ rhs.asUint64(); break;

    case DataType::Float:
    case DataType::Double:
    case DataType::ExtendedReal:
        error = "Cannot apply the 'XOR' operator to real type.";
        break;

    case DataType::Character:
        error = "Cannot apply the 'XOR' operator to character types.";
        break;

    case DataType::String:
        error = "Cannot apply the 'XOR' operator to string types.";
        break;

    case DataType::Unknown:
    default:
        error = "Cannot apply the 'XOR' operator to incompatible data types.";
        break;
    }

    return result;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// BaseExpr Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node containing a constant value.
//! @param[in] at The location of the expression part in source code.
BaseExpr::BaseExpr(const Location &at) :
    _start(at)
{
}

// Inherited from IExpr.
const Location &BaseExpr::getLocation() const { return _start; }

////////////////////////////////////////////////////////////////////////////////
// ConstantExpr Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a node containing a constant value.
//! @param[in] at The location of the constant in source code.
//! @param[in] value The value to return when evaluated.
ConstantExpr::ConstantExpr(const Location &at, const Value &value) :
    BaseExpr(at),
    _value(value)
{
}

// Inherited from IExpr.
bool ConstantExpr::isConstant() const { return true; }

// Inherited from IExpr.
bool ConstantExpr::tryConstantEvaluate(Value &result, Ag::string_ref_t error) const
{
    result = _value;

    if (result.isNull())
    {
        error = "The expression could not be evaluated.";
    }

    return (result.isNull() == false);
}

// Inherited from IExpr.
bool ConstantExpr::tryEvaluate(const IEvalContext * /* context */,
                               Value &result, Ag::string_ref_t error) const
{
    return tryConstantEvaluate(result, error);
}

////////////////////////////////////////////////////////////////////////////////
// SymbolExpr Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a sub-expression which looks up a symbol.
//! @param[in] at The location of the symbol in source code.
//! @param[in] id The identifier of the symbol to look up.
SymbolExpr::SymbolExpr(const Location &at, Ag::string_cref_t id) :
    BaseExpr(at),
    _id(id)
{
}

// Inherited from IExpr.
bool SymbolExpr::isConstant() const { return false; }

// Inherited from IExpr.
bool SymbolExpr::tryConstantEvaluate(Value &result, Ag::string_ref_t error) const
{
    result = Value();
    error = "The a symbol can never be constant.";

    return false;
}

// Inherited from IExpr.
bool SymbolExpr::tryEvaluate(const IEvalContext *context,
                               Value &result, Ag::string_ref_t error) const
{
    if (context->tryLookupSymbol(_id, result))
    {
        return true;
    }
    else
    {
        error = Ag::String::format("Identifier '{0}' was not defined in the current context.",
                                   { _id });
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CurrentAddressExpr Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a sub-expression which returns the address the
//! instruction currently being assembled based on the case address of the
//! object code.
//! @param[in] at The location of the token in source code.
CurrentAddressExpr::CurrentAddressExpr(const Location &at) :
    BaseExpr(at)
{
}

// Inherited from IExpr.
bool CurrentAddressExpr::isConstant() const { return false; }

// Inherited from IExpr.
bool CurrentAddressExpr::tryConstantEvaluate(Value &result,
                                             Ag::string_ref_t error) const
{
    result = Value();
    error = "The a symbol can never be constant.";

    return false;
}

// Inherited from IExpr.
bool CurrentAddressExpr::tryEvaluate(const IEvalContext *context, Value &result,
                                     Ag::string_ref_t /* error */) const
{
    result = Value(context->getAssemblyAddress());

    // The current offset is always available.
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// UnaryOpExpr Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
UnaryOpExpr::UnaryOpExpr(const Location &at, UnaryOpExpr::Fn opFn, IExprPtr childExpr) :
    BaseExpr(at),
    _childExpr(constantOptimise(childExpr)),
    _operator(opFn)
{
}

//! @brief Constructs a sub-expression which applies the unary minus operator.
//! @param at The location of the operator in source code.
//! @param[in] childExpr The sub-expression to apply the operator to.
//! @return A newly constructed sub-expression.
UnaryOpExpr *UnaryOpExpr::createMinus(const Location &at, IExprPtr childExpr)
{
    return new UnaryOpExpr(at, unaryNegate, childExpr);
}

//! @brief Constructs a sub-expression which applies the absolute operator.
//! @param at The location of the operator in source code.
//! @param[in] childExpr The sub-expression to apply the operator to.
//! @return A newly constructed sub-expression.
UnaryOpExpr *UnaryOpExpr::createAbsolute(const Location &at, IExprPtr childExpr)
{
    return new UnaryOpExpr(at, unaryAbsolute, childExpr);
}

//! @brief Constructs a sub-expression which applies the NOT operator.
//! @param at The location of the operator in source code.
//! @param[in] childExpr The sub-expression to apply the operator to.
//! @return A newly constructed sub-expression.
UnaryOpExpr *UnaryOpExpr::createNot(const Location &at, IExprPtr childExpr)
{
    return new UnaryOpExpr(at, unaryNot, childExpr);
}

// Inherited from IExpr.
bool UnaryOpExpr::isConstant() const
{
    if (_childExpr)
    {
        return _childExpr->isConstant();
    }
    else
    {
        // An non-existent child expression will yield a null result every time.
        return true;
    }
}

// Inherited from IExpr.
bool UnaryOpExpr::tryConstantEvaluate(Value &result,
                                      Ag::string_ref_t error) const
{
    result.setNull();

    if (_childExpr)
    {
        Value childValue;

        if (_childExpr->tryConstantEvaluate(childValue, error))
        {
            result = _operator(childValue, error);
        }
    }

    return (result.isNull() == false);
}

// Inherited from IExpr.
bool UnaryOpExpr::tryEvaluate(const IEvalContext *context, Value &result,
                              Ag::string_ref_t error) const
{
    result.setNull();

    if (_childExpr)
    {
        Value childValue;

        if (_childExpr->tryEvaluate(context, childValue, error))
        {
            result = _operator(childValue, error);
        }
    }

    return (result.isNull() == false);
}

////////////////////////////////////////////////////////////////////////////////
// BinaryOpExpr Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
BinaryOpExpr::BinaryOpExpr(const Location &at, BinaryOpExpr::Fn opFn,
                           IExprPtr lhsChildExpr, IExprPtr rhsChildExpr) :
    BaseExpr(at),
    _lhsExpr(constantOptimise(lhsChildExpr)),
    _rhsExpr(constantOptimise(rhsChildExpr)),
    _operator(opFn)
{
}

//! @brief Constructs an object which applies the addition operator to a pair
//! of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createAddition(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, addition, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the subtraction operator to a pair
//! of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createSubtraction(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, subtraction, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the multiplication operator to a pair
//! of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createMultiplication(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, multiplication, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the division operator to a pair
//! of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createDivision(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, division, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the modulus operator to a pair
//! of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createModulus(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, modulus, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the logic left shift operator to
//! a pair of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createLsl(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, logicShiftLeft, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the logic right shift operator to
//! a pair of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createLsr(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, logicShiftRight, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the arithmetic right shift operator to
//! a pair of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createAsr(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, arithmeticShiftRight, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the bitwise-AND operator to a pair
//! of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createAnd(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, bitwiseAnd, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the bitwise-OR operator to a pair
//! of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createOr(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, bitwiseOr, lhsChildExpr, rhsChildExpr);
}

//! @brief Constructs an object which applies the bitwise-XOR operator to a pair
//! of sub-expressions.
//! @param at The location of the operator in source code.
//! @param lhsChildExpr The left operand sub-expression.
//! @param rhsChildExpr The right operand sub-expression.
//! @return A newly constructed binary operator expression.
BinaryOpExpr *BinaryOpExpr::createXor(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr)
{
    return new BinaryOpExpr(at, bitwiseXor, lhsChildExpr, rhsChildExpr);
}

// Inherited from IExpr.
bool BinaryOpExpr::isConstant() const
{
    if (_lhsExpr && _rhsExpr)
    {
        return _lhsExpr->isConstant() && _rhsExpr->isConstant();
    }
    else
    {
        // An non-existent child expression will yield a null result every time.
        return true;
    }
}

// Inherited from IExpr.
bool BinaryOpExpr::tryConstantEvaluate(Value &result,
                                       Ag::string_ref_t error) const
{
    result.setNull();

    if (_lhsExpr && _rhsExpr)
    {
        Value lhsValue, rhsValue;

        if (_lhsExpr->tryConstantEvaluate(lhsValue, error) &&
            _rhsExpr->tryConstantEvaluate(rhsValue, error))
        {
            result = _operator(lhsValue, rhsValue, error);
        }
    }

    return (result.isNull() == false);
}

// Inherited from IExpr.
bool BinaryOpExpr::tryEvaluate(const IEvalContext *context, Value &result,
                               Ag::string_ref_t error) const
{
    result.setNull();

    if (_lhsExpr && _rhsExpr)
    {
        Value lhsValue, rhsValue;

        if (_lhsExpr->tryEvaluate(context, lhsValue, error) &&
            _rhsExpr->tryEvaluate(context, rhsValue, error))
        {
            result = _operator(lhsValue, rhsValue, error);
        }
    }

    return (result.isNull() == false);
}

////////////////////////////////////////////////////////////////////////////////
// ExprToEvaluate Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
ExprToEvaluate::ExprToEvaluate(const IExprUPtr &expr, const char *name, uint32_t max) :
    Expr(expr),
    Name(name),
    Max(max)
{
}

bool ExprToEvaluate::tryEvaluate(IEvalContext *context, Messages &log,
                                 bool isFinalPass, uint32_t &result) const
{
    Mo::Asm::Value value;
    Ag::String error;
    bool isOK = false;
    result = 0;

    if (Expr->tryEvaluate(context, value, error))
    {
        Mo::Asm::Value ordinalValue;

        if (value.tryConvert(DataType::Uint32, ordinalValue))
        {
            if (ordinalValue.asUint32() <= Max)
            {
                result = ordinalValue.asUint32();
                isOK = true;
            }
            else if (isFinalPass)
            {
                log.appendError(Expr->getLocation(),
                                "The {0} evaluated to a value which was out of range.",
                                { Name });
            }
        }
        else if (isFinalPass)
        {
            log.appendError(Expr->getLocation(),
                            "The {0} expression did not evaluate to a value of a compatible type.",
                            { Name });
        }
    }
    else if (isFinalPass)
    {
        log.appendError(Expr->getLocation(), error);
    }

    return isOK;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to replace a sub-expression with a constant, if possible.
//! @param[in] expr The expression to optimise.
//! @return The possibly-optimised sub-expression.
IExprPtr constantOptimise(IExprPtr expr)
{
    if ((expr != nullptr) && expr->isConstant())
    {
        ConstantExpr *constant = nullptr;

        if (Ag::tryCast(expr, constant) == false)
        {
            // We have an expression which is constant, but could be optimised.
            Value constantResult;
            Ag::String error;

            if (expr->tryConstantEvaluate(constantResult, error))
            {
                // Replace the previous expression with a simple constant.
                IExprPtr constExpr = new ConstantExpr(expr->getLocation(), constantResult);
                delete expr;
                expr = constExpr;
            }
        }
    }

    return expr;
}

//! @brief Attempts to evaluate an expression as an unsigned ordinal value.
//! @param[in] context The context in which to evaluate the expression.
//! @param[in] expr The expression to evaluate.
//! @param[out] ordinal Receives the ordinal value, if evaluation was successful.
//! @param[out] error Receives a message if evaluation failed.
//! @retval true A value was successfully returned in ordinal.
//! @retval false Evaluation failed, error has been updated.
bool tryEvaluateOrdinal(IEvalContext *context, IExprPtr expr,
                        uint32_t &ordinal, Ag::string_ref_t error)
{
    bool hasValue = false;
    ordinal = 0;
    Value multiValue;

    if (expr)
    {
        if (expr->tryEvaluate(context, multiValue, error))
        {
            Value ordinalValue;

            if (multiValue.tryConvert(DataType::Uint32, ordinalValue))
            {
                ordinal = ordinalValue.asUint32();
                hasValue = true;
            }
            else
            {
                error = "Expression evaluates to an incompatible data type.";
            }
        }
    }
    else
    {
        error = "Empty expression.";
    }

    return hasValue;
}

//! @brief Attempts to evaluate an expression as an core register identifier.
//! @param[in] context The context in which to evaluate the expression.
//! @param[in] expr The expression to evaluate.
//! @param[out] reg Receives the register value, if evaluation was successful.
//! @param[out] error Receives a message if evaluation failed.
//! @retval true A value was successfully returned in reg.
//! @retval false Evaluation failed, error has been updated.
bool tryEvaluateCoreRegister(IEvalContext *context, IExprPtr expr,
                             CoreRegister &reg, Ag::string_ref_t error)
{
    uint32_t ordinal = 0;
    bool hasReg = false;

    if (tryEvaluateOrdinal(context, expr, ordinal, error))
    {
        if (ordinal < 16)
        {
            reg = Ag::forceFromScalar<CoreRegister>(ordinal);
            hasReg = true;
        }
        else
        {
            error = "The expression evaluated to a value beyond "
                "the range of allowed core register indices.";
        }
    }

    return hasReg;
}

//! @brief Attempts to evaluate an expression as an integer value.
//! @param[in] context The context in which to evaluate the expression.
//! @param[in] expr The expression to evaluate.
//! @param[out] value Receives the integer value, if evaluation was successful.
//! @param[out] error Receives a message if evaluation failed.
//! @retval true A value was successfully returned in ordinal.
//! @retval false Evaluation failed, error has been updated.
bool tryEvaluateInteger(IEvalContext *context, IExprPtr expr,
                        int32_t &value, Ag::string_ref_t error)
{
    bool hasValue = false;
    value = 0;
    Value multiValue;

    if (expr)
    {
        if (expr->tryEvaluate(context, multiValue, error))
        {
            hasValue = true;

            if (multiValue.getDataType() == DataType::Int32)
            {
                value = multiValue.asInt32();
            }
            else if (multiValue.getDataType() == DataType::Uint32)
            {
                value = static_cast<int32_t>(multiValue.asUint32());
            }
            else
            {
                error = "Expression evaluates to an incompatible data type.";
                hasValue = false;
            }
        }
    }
    else
    {
        error = "Empty expression.";
    }

    return hasValue;
}

//! @brief Attempt to evaluate a set of compiled expressions.
//! @param[in] exprsToEval The array of expressions to evaluate.
//! @param[out] values The array to receive the evaluated expression values.
//! @param[in] count The count of entries in the exprsToEval and values arrays.
//! @param[in] context The context in which expressions should be evaluated.
//! @param[in] log The log to receive error messages if failures occurred on
//! the final pass.
//! @param[in] isFinalPass True if failures should be logged as errors, false
//! if no error should be logged.
//! @retval true All expressions were successfully evaluated with values which
//! were within the required range.
//! @retval false At least one expression failed to evaluate.
bool tryEvaluateExpressions(const ExprToEvaluate *exprsToEval,
                            uint32_t *values, size_t count,
                            IEvalContext *context, Messages &log,
                            bool isFinalPass)
{
    bool isOK = true;

    for (size_t i = 0; isOK && (i < count); ++i)
    {
        isOK |= exprsToEval[i].tryEvaluate(context, log, isFinalPass, values[i]);
    }

    return isOK;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


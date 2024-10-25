//! @file Core/Variant.cpp
//! @brief The definition of an object representing a weakly typed value.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <climits>
#include <string>

#include "CoreInternal.hpp"

#include "Ag/Core/String.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/Variant.hpp"
#include "Ag/Core/VariantType.hpp"
#include "Ag/Core/VariantTypes.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Variant Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates a variant object with a null value.
Variant::Variant() :
    _dataType(nullptr)
{
    makeEmpty();
}

//! @brief Creates a copy of an existing variant value.
Variant::Variant(const Variant &rhs) :
    _dataType(rhs._dataType)
{
    makeEmpty();

    if (_dataType != nullptr)
    {
        _dataType->copy(_value, rhs._value);
    }
}

//! @brief Moves a variant value from one object into a new one.
Variant::Variant(Variant &&rhs) noexcept :
    _value(rhs._value),
    _dataType(rhs._dataType)
{
    // Remove the value from rhs.
    rhs._dataType = nullptr;
    rhs.makeEmpty();
}

//! @brief Constructs a variant object representing a boolean value.
//! @param[in] value The value to embed in the variant.
Variant::Variant(bool value) :
    _dataType(VariantTypes::Boolean)
{
    makeEmpty();
    _value.Boolean = value;
}

//! @brief Constructs a variant object representing a Unicode character value.
//! @param[in] value The value to embed in the variant.
Variant::Variant(char32_t value) :
    _dataType(VariantTypes::Character)
{
    makeEmpty();
    _value.Character = value;
}

//! @brief Constructs a variant object representing an 8-bit signed integer.
//! @param[in] value The value to embed in the variant.
Variant::Variant(int8_t value) :
    _dataType(VariantTypes::Int8)
{
    makeEmpty();
    _value.Int8 = value;
}

//! @brief Constructs a variant object representing  an 8-bit unsigned integer.
//! @param[in] value The value to embed in the variant.
Variant::Variant(uint8_t value) :
    _dataType(VariantTypes::Uint8)
{
    makeEmpty();
    _value.Uint8 = value;
}

//! @brief Constructs a variant object representing a 16-bit signed integer.
//! @param[in] value The value to embed in the variant.
Variant::Variant(int16_t value) :
    _dataType(VariantTypes::Int16)
{
    makeEmpty();
    _value.Int16 = value;
}

//! @brief Constructs a variant object representing a 16-bit unsigned integer.
//! @param[in] value The value to embed in the variant.
Variant::Variant(uint16_t value) :
    _dataType(VariantTypes::Uint16)
{
    makeEmpty();
    _value.Uint16 = value;
}

//! @brief Constructs a variant object representing a 32-bit signed integer.
//! @param[in] value The value to embed in the variant.
Variant::Variant(int32_t value) :
    _dataType(VariantTypes::Int32)
{
    makeEmpty();
    _value.Int32 = value;
}

//! @brief Constructs a variant object representing a 32-bit unsigned integer..
//! @param[in] value The value to embed in the variant.
Variant::Variant(uint32_t value) :
    _dataType(VariantTypes::Uint32)
{
    makeEmpty();
    _value.Uint32 = value;
}

//! @brief Constructs a variant object representing a 64-bit signed integer.
//! @param[in] value The value to embed in the variant.
Variant::Variant(int64_t value) :
    _dataType(VariantTypes::Int64)
{
    makeEmpty();
    _value.Int64 = value;
}

//! @brief Constructs a variant object representing a 64-bit unsigned integer.
//! @param[in] value The value to embed in the variant.
Variant::Variant(uint64_t value) :
    _dataType(VariantTypes::Uint64)
{
    makeEmpty();
    _value.Uint64 = value;
}

//! @brief Constructs a variant object representing a 32-bit floating point value.
//! @param[in] value The value to embed in the variant.
Variant::Variant(float value) :
    _dataType(VariantTypes::Float)
{
    makeEmpty();
    _value.Float = value;
}

//! @brief Constructs a variant object representing a 64-bit floating point value.
//! @param[in] value The value to embed in the variant.
Variant::Variant(double value) :
    _dataType(VariantTypes::Double)
{
    makeEmpty();
    _value.Double = value;
}

//! @brief Constructs a variant object representing a UTF-8 string value.
//! @param[in] value The value to embed in the variant.
Variant::Variant(const String &value) :
    _dataType(VariantTypes::String)
{
    makeEmpty();
    VariantTypes::String->create(_value, value);
}

//! @brief Constructs a variant object representing a UTF-8 string value.
//! @param[in] value The value to embed in the variant. A null pointer will
//! result in an empty string.
Variant::Variant(utf8_cptr_t value) :
    _dataType(VariantTypes::String)
{
    makeEmpty();

    if (value == nullptr)
    {
        VariantTypes::String->create(_value, String::Empty);
    }
    else
    {
        VariantTypes::String->create(_value, String(value));
    }
}

//! @brief Constructs a variant object representing a UTF-8 string value.
//! @param[in] value The value to embed in the variant.
Variant::Variant(const std::string_view &value) :
    _dataType(VariantTypes::String)
{
    makeEmpty();

    VariantTypes::String->create(_value, String(value.data(), value.length()));
}

//! @brief Performs type-specific disposal of variant data, if any value
//! is currently assigned.
Variant::~Variant()
{
    if (_dataType != nullptr)
    {
        // Dispose of the variant value.
        _dataType->destroy(_value);

        makeEmpty();
        _dataType = nullptr;
    }
}

//! @brief Determines if the object has a value assigned.
//! @retval true The object had no value.
//! @retval false The object has a value and data type assigned.
bool Variant::isEmpty() const { return _dataType == nullptr; }

//! @brief Gets the data type of the value currently assigned to the variant.
//! @returns A pointer to the type description or nullptr if the Variant
//! does not have a value.
const VariantType *Variant::getType() const { return _dataType; }

//! @brief Formats the value as text using the default Display format.
//! @returns A string representing the value as text.
String Variant::toString() const
{
    if (_dataType == nullptr)
    {
        return String::Empty;
    }
    else
    {
        std::string buffer;
        _dataType->toString(FormatInfo(LocaleInfo::getDisplay()),
                            _value, buffer);

        return String(buffer);
    }
}

//! @brief Formats the value as text using a specified format.
//! @param[in] format The format options used to render the value as text.
//! @returns A string representing the value as text.
String Variant::toString(const FormatInfo &format) const
{
    if (_dataType == nullptr)
    {
        return String::Empty;
    }
    else
    {
        std::string buffer;
        _dataType->toString(format, _value, buffer);

        return String(buffer);
    }
}

//! @brief Appends a textual representation of the value to a string.
//! @param[out] buffer The UTF-8-encoded STL string to append to.
void Variant::appendToString(std::string &buffer) const
{
    if (_dataType != nullptr)
    {
        FormatInfo formatOptions(LocaleInfo::getNeutral());

        _dataType->toString(formatOptions, _value, buffer);
    }
}

//! @brief Appends a textual representation of the value to a string.
//! @param[in] info Details of how values should be formatted.
//! @param[out] buffer The UTF-8-encoded STL string to append to.
void Variant::appendToString(const FormatInfo &info, std::string &buffer) const
{
    if (_dataType != nullptr)
    {
        _dataType->toString(info, _value, buffer);
    }
}

//! @brief Attempts to populate the object from a string using the default
//! display format.
//! @param[in] source The string to parse.
//! @param[in] dataType The data type of the value to interpret.
//! @retval true The text was in a valid format, the value has been successfully
//! interpreted and assigned.
//! @retval false The text was in an invalid format, the object is left in
//! an empty state.
bool Variant::tryParse(const String &source, const VariantType *dataType)
{
    clear();

    bool isOK = false;

    if (dataType != nullptr)
    {
        size_t length = source.getUtf8Length();

        if (dataType->tryParse(LocaleInfo::getDisplay(),
                               source.getUtf8Bytes(),
                               length, _value))
        {
            _dataType = dataType;
            isOK = true;
        }
    }

    return isOK;
}

//! @brief Attempts to populate the object from a string using a specified
//! display format.
//! @param[in] format The format the value is expected to be rendered in.
//! @param[in] source The string to parse.
//! @param[in] dataType The data type of the value to interpret.
//! @retval true The text was in a valid format, the value has been successfully
//! interpreted and assigned.
//! @retval false The text was in an invalid format, the object is left in
//! an empty state.
bool Variant::tryParse(const LocaleInfo &format, const String &source,
                       const VariantType *dataType)
{
    clear();

    bool isOK = false;

    if (dataType != nullptr)
    {
        size_t length = source.getUtf8Length();

        if (dataType->tryParse(format, source.getUtf8Bytes(),
                               length, _value))
        {
            _dataType = dataType;
            isOK = true;
        }
    }

    return isOK;
}


//! @brief Overwrites the contents of the variant with another.
//! @param[in] rhs The variant to create a copy of.
//! @returns A reference to the current variant.
Variant &Variant::operator=(const Variant &rhs)
{
    if (&rhs != this)
    {
        // The values are different.
        if (_dataType != nullptr)
        {
            _dataType->destroy(_value);
            makeEmpty();
        }

        // Make a copy of the value.
        _dataType = rhs._dataType;
        _dataType->copy(_value, rhs._value);
    }

    return *this;
}

//! @brief Overwrites the contents of the variant with another and clears
//! the original.
//! @param[in] rhs The variant to move data from.
//! @returns A reference to the current variant.
Variant &Variant::operator=(Variant &&rhs) noexcept
{
    if (&rhs != this)
    {
        if (_dataType != nullptr)
        {
            _dataType->destroy(_value);
            makeEmpty();
        }

        _dataType = rhs._dataType;
        _dataType->move(_value, std::move(rhs._value));

        rhs._dataType = nullptr;
        rhs.makeEmpty();
    }

    return *this;
}

//! @brief Zeros all storage in a variant and its data type.
void Variant::clear()
{
    if (_dataType != nullptr)
    {
        _dataType->destroy(_value);
    }

    _dataType = nullptr;
    makeEmpty();
}

//! @brief Determines if it is possible to convert the current variant type to
//! a specified type.
//! @param[in] targetDataType An object representing the data type to try to
//! convert to.
//! @retval true A conversion is theoretically possible, though may fail.
//! @retval false No conversion is possible.
bool Variant::canConvert(const VariantType *targetDataType) const
{
    VariantTypePair key(_dataType, targetDataType);
    auto conversionMap = getConversionMap();
    auto pos = conversionMap.find(key);

    return pos != conversionMap.end();
}

//! @brief Attempts to convert the current variant to a specified type.
//! @param[in] targetDataType An object representing the data type to try to
//! convert to.
//! @param[out] target An object to receive the result of the conversion.
//! @retval true The conversion succeeded.
//! @retval false The conversion failed.
bool Variant::tryConvert(const VariantType *targetDataType, Variant &target) const
{
    VariantTypePair key(_dataType, targetDataType);

    auto conversionMap = getConversionMap();
    auto pos = conversionMap.find(key);
    bool isConverted = false;

    if (pos != conversionMap.end())
    {
        target.clear();

        if (pos->second(_value, target._value))
        {
            target._dataType = targetDataType;
            isConverted = true;
        }
    }

    return isConverted;
}

//! @brief Fills the variant data with zeros.
void Variant::makeEmpty()
{
#ifdef IS_64_BIT
    std::fill_n(_value.Words64, std::size(_value.Words64), 0);
#else
    std::fill_n(_value.Words32, std::size(_value.Words32), 0);
#endif
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


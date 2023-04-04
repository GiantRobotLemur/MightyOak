//! @file VariantType.hpp
//! @brief The declaration of a value type which can hold one of many possible
//! data types.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_VARIANT_TYPE_HPP__
#define __AG_CORE_VARIANT_TYPE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "Exception.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

class FormatInfo;
class LocaleInfo;
class String;
class IStream;

//! @brief A union representing 128-bits of binary storage for a variant.
union VariantData
{
    bool Boolean;
    char32_t Character;
    int8_t Int8;
    uint8_t Uint8;
    int16_t Int16;
    uint16_t Uint16;
    int32_t Int32;
    uint32_t Uint32;
    int64_t Int64;
    uint64_t Uint64;
    float Float;
    double Double;
    uintptr_t Uintptr;
    intptr_t Intptr;
    void *Pointer;
    uint8_t Bytes[16];
    uint16_t Words16[8];
    uint32_t Words32[4];
    uint64_t Words64[2];
};

typedef bool (*VariantConversionFn)(const VariantData &sourceValue,
                                    VariantData &targetValue);

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An exception thrown when the program attempts to perform a
//! type-specific operation on a Variant and the data type is not what was
//! expected.
class VariantTypeMismatchException : public Exception
{
public:
    VariantTypeMismatchException(const char *typeName);
};

class BooleanVariantType;
class CharacterVariantType;
class Int8VariantType;
class Uint8VariantType;
class Int16VariantType;
class Uint16VariantType;
class Int32VariantType;
class Uint32VariantType;
class Int64VariantType;
class Uint64VariantType;
class IntptrVariantType;
class UintptrVariantType;
class PointerVariantType;
class FloatVariantType;
class DoubleVariantType;
class StringVariantType;

//! @brief An object which manages values of a specific type expressed as a variant.
class VariantType
{
public:
    // Construction/Destruction
    virtual ~VariantType() = default;

    // Accessors
    //! @brief Returns the static name of the data type.
    virtual const char *getName() const = 0;

    // Operations
    //! @brief Is called to dispose of resources owned by a variant value.
    //! @param[in] value The variant value to be disposed of. The value will be
    //! cleared immediately after.
    virtual void destroy(const VariantData &value) const = 0;

    //! @brief Creates an independent copy of a variant value of the described type.
    //! @param[in] destination Receives the copied value.
    //! @param[in] source The value data to copy.
    virtual void copy(VariantData &destination, const VariantData &source) const = 0;

    //! @brief Moves variant data from one object to another.
    //! @param[in] destination The object to receives the moved data.
    //! @param[in] source The rval reference to transfer from.
    virtual void move(VariantData &destination, VariantData &&source) const = 0;

    //! @brief Attempts to deserialize the variant value from a stream of bytes.
    //! @param[in] input The binary stream to read from.
    //! @param[out] destination The variant data to populate from the stream.
    //! @retval true A value was successfully read.
    //! @retval false The value could not be read from the stream.
    virtual bool read(IStream *input, VariantData &destination) const = 0;

    //! @brief Attempts to serialize the variant value as a stream of bytes.
    //! @param[in] output The stream to write bytes to.
    //! @param[in] source The variant value to be written to the stream.
    virtual bool write(IStream *output, const VariantData &source) const = 0;

    //! @brief Formats the object as text.
    //! @param[in] format Options relating to how values are formatted.
    //! @param[in] value The value to format.
    //! @param[out] buffer The STL string buffer to append the characters of
    //! the formatted value to.
    virtual void toString(const FormatInfo &format, const VariantData &value,
                          std::string &buffer) const = 0;

    //! @brief Attempts to translate a string into a value of the described type.
    //! @param[in] format Options which define how the value might be formatted.
    //! @param[in] source The array of UTF-8 encoded characters to interpret.
    //! @param[in,out] length The count of characters in source, optionally
    //! updated to reflect the number of characters consumed while parsing.
    //! @param[out] value Receives the data parsed from the text.
    //! @retval true The value was successfully parsed.
    //! @retval false A value of the described format could not be parsed from
    //! the characters available.
    virtual bool tryParse(const LocaleInfo &format, utf8_cptr_t source,
                          size_t &length, VariantData &value) const = 0;
protected:
    static void registerConversion(const VariantType *source,
                                   const VariantType *target,
                                   VariantConversionFn fn);
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file Ag/Core/VariantTypes.hpp
//! @brief The declaration of various implementations of the VariantType class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_VARIANT_TYPES_HPP__
#define __AG_CORE_VARIANT_TYPES_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Variant.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#define DECLARE_SCALAR_VARIANT_TYPE(name, datatype) \
class name ## VariantType : public VariantType { \
    name ## VariantType() = default; \
public: static const name ## VariantType *getInstance(); \
    void create(VariantData &variant, datatype value) const; \
    datatype getValue(const VariantData &variant) const; \
    datatype &getRef(VariantData &variant) const; \
    const datatype &getRef(const VariantData &variant) const; \
    datatype *getPtr(VariantData &variant) const; \
    const datatype *getPtr(const VariantData &variant) const; \
    virtual const char *getName() const override; \
    virtual void destroy(const VariantData &value) const override; \
    virtual void copy(VariantData &destination, const VariantData &source) const override; \
    virtual void move(VariantData &destination, VariantData &&source) const override; \
    virtual bool read(IStream *input, VariantData &destination) const override; \
    virtual bool write(IStream *output, const VariantData &source) const override; \
    virtual void toString(const FormatInfo &format, const VariantData &value, \
                          std::string &buffer) const override; \
    virtual bool tryParse(const LocaleInfo &format, utf8_cptr_t source, \
                          size_t &length, VariantData &value) const override; }

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
DECLARE_SCALAR_VARIANT_TYPE(Int8, int8_t);
DECLARE_SCALAR_VARIANT_TYPE(Uint8, uint8_t);
DECLARE_SCALAR_VARIANT_TYPE(Int16, int16_t);
DECLARE_SCALAR_VARIANT_TYPE(Uint16, uint16_t);
DECLARE_SCALAR_VARIANT_TYPE(Int32, int32_t);
DECLARE_SCALAR_VARIANT_TYPE(Uint32, uint32_t);
DECLARE_SCALAR_VARIANT_TYPE(Int64, int64_t);
DECLARE_SCALAR_VARIANT_TYPE(Uint64, uint64_t);
DECLARE_SCALAR_VARIANT_TYPE(Intptr, intptr_t);
DECLARE_SCALAR_VARIANT_TYPE(Uintptr, uintptr_t);
DECLARE_SCALAR_VARIANT_TYPE(Float, float);
DECLARE_SCALAR_VARIANT_TYPE(Double, double);

#undef DECLARE_SCALAR_VARIANT_TYPE

//! @brief A VariantType implementation which operates on boolean values.
class BooleanVariantType : public VariantType
{
    // Construction
    BooleanVariantType() = default;
public:
    static const BooleanVariantType *getInstance();
    static const std::string_view TrueText;
    static const std::string_view FalseText;

    // Accessors
    void create(VariantData &variant, bool value) const;
    bool getValue(const VariantData &variant) const;
    bool *getPtr(VariantData &variant) const;
    const bool *getPtr(const VariantData &variant) const;
    bool &getRef(VariantData &variant) const;
    const bool &getRef(const VariantData &variant) const;

    // Overrides
    virtual const char *getName() const override;
    virtual void destroy(const VariantData &value) const override;
    virtual void copy(VariantData &destination, const VariantData &source) const override;
    virtual void move(VariantData &destination, VariantData &&source) const override;
    virtual bool read(IStream *input, VariantData &destination) const override;
    virtual bool write(IStream *output, const VariantData &source) const override;
    virtual void toString(const FormatInfo &format, const VariantData &value,
                          std::string &buffer) const override;
    virtual bool tryParse(const LocaleInfo &format, utf8_cptr_t source,
                          size_t &length, VariantData &value) const override;
};

//! @brief A VariantType implementation which operates on Unicode code point values.
class CharacterVariantType : public VariantType
{
    // Construction
    CharacterVariantType() = default;
public:
    static const CharacterVariantType *getInstance();

    // Accessors
    void create(VariantData &variant, char32_t value) const;
    char32_t getValue(const VariantData &variant) const;
    char32_t *getPtr(VariantData &variant) const;
    const char32_t *getPtr(const VariantData &variant) const;
    char32_t &getRef(VariantData &variant) const;
    const char32_t &getRef(const VariantData &variant) const;

    // Overrides
    virtual const char *getName() const override;
    virtual void destroy(const VariantData &value) const override;
    virtual void copy(VariantData &destination, const VariantData &source) const override;
    virtual void move(VariantData &destination, VariantData &&source) const override;
    virtual bool read(IStream *input, VariantData &destination) const override;
    virtual bool write(IStream *output, const VariantData &source) const override;
    virtual void toString(const FormatInfo &format, const VariantData &value,
                          std::string &buffer) const override;
    virtual bool tryParse(const LocaleInfo &format, utf8_cptr_t source,
                          size_t &length, VariantData &value) const override;
};

//! @brief A VariantType implementation which operates on void pointers.
class PointerVariantType : public VariantType
{
    // Construction
    PointerVariantType() = default;
public:
    static const PointerVariantType *getInstance();

    // Accessors
    void create(VariantData &variant, const void *value) const;
    void *getValue(const VariantData &variant) const;
    void *getPtr(VariantData &variant) const;
    const void *getPtr(const VariantData &variant) const;

    // Overrides
    virtual const char *getName() const override;
    virtual void destroy(const VariantData &value) const override;
    virtual void copy(VariantData &destination, const VariantData &source) const override;
    virtual void move(VariantData &destination, VariantData &&source) const override;
    virtual bool read(IStream *input, VariantData &destination) const override;
    virtual bool write(IStream *output, const VariantData &source) const override;
    virtual void toString(const FormatInfo &format, const VariantData &value,
                          std::string &buffer) const override;
    virtual bool tryParse(const LocaleInfo &format, utf8_cptr_t source,
                          size_t &length, VariantData &value) const override;
};

//! @brief A VariantType implementation which operates on UTF-8 string values.
class StringVariantType : public VariantType
{
    // Construction
    StringVariantType() = default;
public:
    static const StringVariantType *getInstance();

    // Accessors
    void create(VariantData &variant, const Ag::String &value) const;
    Ag::String getValue(const VariantData &variant) const;
    Ag::String *getPtr(VariantData &variant) const;
    const Ag::String *getPtr(const VariantData &variant) const;
    Ag::String &getRef(VariantData &variant) const;
    const Ag::String &getRef(const VariantData &variant) const;

    // Overrides
    virtual const char *getName() const override;
    virtual void destroy(const VariantData &value) const override;
    virtual void copy(VariantData &destination, const VariantData &source) const override;
    virtual void move(VariantData &destination, VariantData &&source) const override;
    virtual bool read(IStream *input, VariantData &destination) const override;
    virtual bool write(IStream *output, const VariantData &source) const override;
    virtual void toString(const FormatInfo &format, const VariantData &value,
                          std::string &buffer) const override;
    virtual bool tryParse(const LocaleInfo &format, utf8_cptr_t source,
                          size_t &length, VariantData &value) const override;
};

//! @brief Defines VariantType implementations for built-in data types.
class VariantTypes
{
private:
    VariantTypes() = delete;
    ~VariantTypes() = delete;
public:
    //! @brief A VariantType which manages boolean values.
    static const BooleanVariantType *Boolean;

    //! @brief A variant type which operates on char32_t Unicode code points.
    static const CharacterVariantType *Character;

    //! @brief A variant type which operates on signed 8-bit integers.
    static const Int8VariantType *Int8;

    //! @brief A variant type which operates on unsigned 8-bit integers.
    static const Uint8VariantType *Uint8;

    //! @brief A variant type which operates on signed 16-bit integers.
    static const Int16VariantType *Int16;

    //! @brief A variant type which operates on unsigned 16-bit integers.
    static const Uint16VariantType *Uint16;

    //! @brief A variant type which operates on signed 32-bit integers.
    static const Int32VariantType *Int32;

    //! @brief A variant type which operates on unsigned 32-bit integers.
    static const Uint32VariantType *Uint32;

    //! @brief A variant type which operates on signed 64-bit integers.
    static const Int64VariantType *Int64;

    //! @brief A variant type which operates on unsigned 64-bit integers.
    static const Uint64VariantType *Uint64;

    //! @brief A variant type which operates on signed
    //! machine-word-sized integers.
    static const IntptrVariantType *Intptr;

    //! @brief A variant type which operates on unsigned
    //! machine-word-sized integers.
    static const UintptrVariantType *Uintptr;

    //! @brief A variant type which operates on pointer values.
    static const PointerVariantType *Pointer;

    //! @brief A variant type which operates on single precision 32-bit
    //! floating point values.
    static const FloatVariantType *Float;

    //! @brief A variant type which operates on double precision 64-bit
    //! floating point values.
    static const DoubleVariantType *Double;

    //! @brief A variant type which operates on immutable UTF-8 string values.
    static const StringVariantType *String;
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file Ag/Core/Variant.hpp
//! @brief The declaration of an object representing a weakly typed value.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_VARIANT_HPP__
#define __AG_CORE_VARIANT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <initializer_list>

#include "Exception.hpp"
#include "Stream.hpp"
#include "VariantType.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class VariantType;
class FormatInfo;
class LocaleInfo;

//! @brief An object representing a weakly typed value.
class Variant
{
public:
    // Construction/Destruction
    Variant();
    Variant(const Variant &rhs);
    Variant(Variant &&rhs) noexcept;
    Variant(bool value);
    Variant(char32_t value);
    Variant(int8_t value);
    Variant(uint8_t value);
    Variant(int16_t value);
    Variant(uint16_t value);
    Variant(int32_t value);
    Variant(uint32_t value);
    Variant(int64_t value);
    Variant(uint64_t value);
    Variant(float value);
    Variant(double value);
    Variant(const String &value);
    Variant(utf8_cptr_t value);
    Variant(const std::string_view &value);

    //! @brief Constructs a variant with a specific value.
    //! @tparam TDataType The VariantType-derived type which supports an
    //! appropriate create() function.
    //! @tparam TArgs The arguments to pass to the create() function after
    //! the reference to the variant value to initialise.
    //! @param[in] dataType The data type of the new variant.
    //! @param[in] args The arguments to pass to the creation function.
    template<typename TDataType, typename ... TArgs>
    Variant(const TDataType *dataType, TArgs && ... args) :
        _dataType(dataType)
    {
        makeEmpty();

        dataType->create(_value, std::forward<TArgs>(args)...);
    }

    // TODO: Constructors for fundamental types + string.
    ~Variant();

    // Accessors
    bool isEmpty() const;
    const VariantType *getType() const;
    String toString() const;
    String toString(const FormatInfo &format) const;
    void appendToString(std::string &buffer) const;
    void appendToString(const FormatInfo &info, std::string &buffer) const;
    bool tryParse(const String &source, const VariantType *dataType);
    bool tryParse(const LocaleInfo &format, const String &source,
                  const VariantType *dataType);

    //! @brief Attempts to get a copy of the value of the variant.
    //! @tparam TDataType The VariantType-derived type which supports an
    //! appropriate get() function which receives a VariantData reference and
    //! returns a value.
    //! @tparam TResultType The data type of the value returned by
    //! TDataType.get(const VariantData &data).
    //! @returns A copy of the value derived from the variant.
    //! @throws VariantTypeMismatchException If the variant doesn't have a
    //! value of the expected data type.
    template<typename TDataType, typename TResultType>
    TResultType get() const
    {
        const TDataType *resolvedType = dynamic_cast<const TDataType *>(_dataType);

        if (resolvedType == nullptr)
        {
            throw VariantTypeMismatchException((_dataType == nullptr) ? nullptr :
                                                                        _dataType->getName());
        }

        return resolvedType->getValue(_value);
    }

    //! @brief Gets a reference to the value of the variant.
    //! @tparam TDataType The VariantType-derived type which supports an
    //! appropriate getRef() function which receives a VariantData reference and
    //! returns a value.
    //! @tparam TResultType The data type of the reference returned by
    //! TDataType.getRef(const VariantData &data) const.
    //! @returns A const reference to the value of the variant.
    //! @throws VariantTypeMismatchException If the variant doesn't have a
    //! value of the expected data type.
    template<typename TDataType, typename TResultType>
    const TResultType &getRef() const
    {
        const TDataType *resolvedType = dynamic_cast<const TDataType *>(_dataType);

        if (resolvedType == nullptr)
        {
            throw VariantTypeMismatchException((_dataType == nullptr) ? nullptr :
                                               _dataType->getName());
        }

        // Be explicit about the return type.
        const TResultType &result = resolvedType->getRef(_value);

        return result;
    }

    //! @brief Gets a reference to the value of the variant.
    //! @tparam TDataType The VariantType-derived type which supports an
    //! appropriate getRef() function which receives a VariantData reference and
    //! returns a value.
    //! @tparam TResultType The data type of the reference returned by
    //! TDataType.getRef(const VariantData &data).
    //! @returns A reference to the value of the variant.
    //! @throws VariantTypeMismatchException If the variant doesn't have a
    //! value of the expected data type.
    template<typename TDataType, typename TResultType>
    TResultType &getRef()
    {
        const TDataType *resolvedType = dynamic_cast<const TDataType *>(_dataType);

        if (resolvedType == nullptr)
        {
            throw VariantTypeMismatchException((_dataType == nullptr) ? nullptr :
                                               _dataType->getName());
        }

        // Be explicit about the return type.
        TResultType &result = resolvedType->getRef(_value);

        return result;
    }

    //! @brief Gets a pointer to the value of the variant.
    //! @tparam TDataType The VariantType-derived type which supports an
    //! appropriate getPtr() function which receives a VariantData reference and
    //! returns a value.
    //! @tparam TResultType The data type of the pointer returned by
    //! TDataType.getPtr(const VariantData &data) const.
    //! @returns A const pointer to the value of the variant.
    //! @throws VariantTypeMismatchException If the variant doesn't have a
    //! value of the expected data type.
    template<typename TDataType, typename TResultType>
    const TResultType *getPtr() const
    {
        const TDataType *resolvedType = dynamic_cast<const TDataType *>(_dataType);

        if (resolvedType == nullptr)
        {
            throw VariantTypeMismatchException((_dataType == nullptr) ? nullptr :
                                               _dataType->getName());
        }

        // Be explicit about the return type.
        const TResultType *result = resolvedType->getPtr(_value);

        return result;
    }

    //! @brief Gets a pointer to the value of the variant.
    //! @tparam TDataType The VariantType-derived type which supports an
    //! appropriate getPtr() function which receives a VariantData reference and
    //! returns a value.
    //! @tparam TResultType The data type of the pointer returned by
    //! TDataType.getPtr(const VariantData &data) const.
    //! @returns A pointer to the value of the variant.
    //! @throws VariantTypeMismatchException If the variant doesn't have a
    //! value of the expected data type.
    template<typename TDataType, typename TResultType>
    TResultType *getPtr()
    {
        const TDataType *resolvedType = dynamic_cast<const TDataType *>(_dataType);

        if (resolvedType == nullptr)
        {
            throw VariantTypeMismatchException((_dataType == nullptr) ? nullptr :
                                               _dataType->getName());
        }

        // Be explicit about the return type.
        TResultType *result = resolvedType->getPtr(_value);

        return result;
    }

    // Operations
    Variant &operator=(const Variant &rhs);
    Variant &operator=(Variant &&rhs) noexcept;

    //! @brief Overwrites the value of a variant.
    //! @tparam TDataType The VariantType-derived type which supports an
    //! appropriate create(Variant::Data &, TArgs) function which receives a
    //! VariantData reference to initialise along with forwarded parameters.
    template<typename TDataType, typename ... TArgs>
    void assign(const TDataType *dataType, TArgs && ... args)
    {
        clear();

        dataType->create(_value, std::forward<TArgs>(args)...);
    }

    void clear();
    bool canConvert(const VariantType *targetDataType) const;
    bool tryConvert(const VariantType *targetDataType, Variant &target) const;

    // TODO: Assignment operators for fundamental types + string.

private:
    // Internal Functions
    void makeEmpty();

    // Internal Fields
    VariantData _value;
    const VariantType *_dataType;
};

//! @brief An alias for a variable list of values based on the Variant data type.
using VariantList = std::initializer_list<Variant>;

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file Ag/Core/Utils.hpp
//! @brief The declaration of useful stand-alone utility functions defined by
//! the core library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_UTILS_HPP__
#define __AG_CORE_UTILS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <cstdio>
#include <cstring>

#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "Configuration.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class String;

//! @brief An object which can automatically close an std::FILE as part of a
//! std::unique_ptr<>.
struct StdFileCloser
{
    //! @brief Closes a file if the pointer is valid.
    //! @param[in] file The file to close.
    void operator()(FILE *file) const
    {
        if (file != nullptr)
        {
            fclose(file);
        }
    }
};

//! @brief A unique pointer to an stdio-FILE object which calls fclose() on the
//! pointer at destruction.
typedef std::unique_ptr<FILE, StdFileCloser> StdFilePtr;

//! @brief An object defined in lexical scope which overwrites a property at
//! construction and restores the value at destruction.
template<typename TSource, typename TValue = TSource> class ValueScope
{
private:
    TSource &_value;
    TValue _oldValue;
public:
    //! @brief Overwrites a property with a value, but capturing the old value.
    //! @param[in] scopedValue A reference to the property to update.
    //! @param[in] value The value to write to scopedValue now.
    ValueScope(TSource &scopedValue, const TValue &value) :
        _value(scopedValue),
        _oldValue(scopedValue)
    {
        // Overwrite the value until this object is destroyed.
        _value = value;
    }

    //! @brief Overwrites the property with the value captured at construction.
    ~ValueScope()
    {
        // Restore the original value.
        _value = _oldValue;
    }

    ValueScope(const ValueScope &) = delete;
    ValueScope(ValueScope &&) = delete;
    ValueScope &operator=(const ValueScope &) = delete;
    ValueScope &operator=(ValueScope &&) = delete;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
// Implemented in StringPrivate.cpp.
int compareStringsIgnoreCase(utf8_cptr_t lhs, utf8_cptr_t rhs);
int compareBoundedStringsIgnoreCase(utf8_cptr_t lhs, utf8_cptr_t rhs, size_t length);

// Implemented in CoreInternal.cpp.
bool tryOpenFile(const String &fileName, const utf8_cptr_t mode,
                 FILE *&stream, String &error);

bool isStdoutEnabled();
void enableStdout();

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Calculates a pointer to an object offset from a base.
//! @tparam T The data type of the object to calculate a pointer to.
//! @param[in] base The base address to offset from.
//! @param[in] offset The count of bytes to offset from the base in order
//! to calculate the final pointer.
//! @return A type-safe pointer to the appropriate object.
template<typename T> T *offsetPtr(void *base, size_t offset)
{
    return reinterpret_cast<T *>(static_cast<uint8_t *>(base) + offset);
}

//! @brief Calculates a const pointer to an object offset from a base.
//! @tparam T The data type of the object to calculate a pointer to.
//! @param[in] base The base address to offset from.
//! @param[in] offset The count of bytes to offset from the base in order
//! to calculate the final pointer.
//! @return A type-safe pointer to the appropriate object.
template<typename T> const T *offsetPtr(const void *base, size_t offset)
{
    return reinterpret_cast<const T *>(static_cast<const uint8_t *>(base) + offset);
}

//! @brief Calculate the byte offset between two pointers.
//! @param[in] basePtr The pointer to calculate the offset from.
//! @param[in] offsetPtr The pointer to calculate the offset to from basePtr.
//! @return The signed offset added to basePtr to calculate offsetPtr.
inline ptrdiff_t ptrOffset(const void *basePtr, const void *offsetPtr)
{
    return static_cast<const uint8_t *>(offsetPtr) -
        static_cast<const uint8_t *>(basePtr);
}

//! @brief Safely disposes of an object allocated with the new operator.
//! @tparam T The data type of the object to dispose of.
//! @param[in,out] ptr The pointer to the object to dispose of and reset to nullptr.
//! @retval true The object was disposed of and ptr set to nullptr.
//! @retval false ptr was already set to nullptr, no action was taken.
//! @note This function should only be used when the lifetime of dynamically
//! allocated data is non-standard. For standard usage, consider using either
//! std::unique_ptr<T> or std::shared_ptr<T>.
template<typename T> bool safeDelete(T *&ptr)
{
    bool wasDeleted = false;

    if (ptr != nullptr)
    {
        // Ensure the pointer is reset to nullptr whether the deletion
        // succeeds or not.
        T *ptrToDelete = nullptr;
        wasDeleted = true;

        std::swap(ptr, ptrToDelete);

        delete ptrToDelete;
    }

    return wasDeleted;
}

//! @brief Safely disposes of an object allocated with the array new operator.
//! @tparam T The data type of the object to dispose of.
//! @param[in,out] ptr The pointer to the object to dispose of and reset to nullptr.
//! @retval true The object was disposed of and ptr set to nullptr.
//! @retval false ptr was already set to nullptr, no action was taken.
//! @note This function should only be used when the lifetime of dynamically
//! allocated data is non-standard. For standard usage, consider using either
//! std::unique_ptr<T> or std::shared_ptr<T>.
template<typename T> bool safeDeleteArray(T *&ptr)
{
    bool wasDeleted = false;

    if (ptr != nullptr)
    {
        // Ensure the pointer is reset to nullptr whether the deletion
        // succeeds or not.
        T *ptrToDelete = nullptr;
        wasDeleted = true;

        std::swap(ptr, ptrToDelete);

        delete[] ptrToDelete;
    }

    return wasDeleted;
}

//! @brief Converts a scalar value to a size_t, truncating to 0 if negative.
//! @tparam T The scalar data type to convert.
//! @param[in] value The value to convert to an unsigned machine word.
//! @return The value as an unsigned scalar, 0 if value was negative.
template<typename T> size_t toSize(T value)
{
    using TypeInfo = std::numeric_limits<T>;

    if constexpr (TypeInfo::is_signed)
    {
        return (value < static_cast<T>(0)) ? 0 : static_cast<size_t>(value);
    }
    else
    {
        return static_cast<size_t>(value);
    }
}

//! @brief Wraps a dynamic cast in something more convenient.
//! @tparam TBase The base class data type. This can be inferred from
//! the parameters.
//! @tparam TDerived The derived class type which should inherit from
//! TBase. This can be inferred from the parameters.
//! @param[in] obj A pointer to an object to attempt to cast.
//! @param[out] derived Receives a pointer to the converted object or nullptr.
//! @retval true The cast was successful, derived is a valid pointer.
//! @retval false The cast failed, derived is nullptr.
template<typename TDerived, typename TBase>
bool tryCast(TBase *obj, TDerived *&derived)
{
    derived = dynamic_cast<TDerived *>(obj);

    return derived != nullptr;
}

//! @brief Wraps a dynamic cast in something more convenient.
//! @tparam TBase The base class data type. This can be inferred from
//! the parameters.
//! @tparam TDerived The derived class type which should inherit from
//! TBase. This can be inferred from the parameters.
//! @param[in] obj A pointer to an object to attempt to cast.
//! @param[out] derived Receives a pointer to the converted object or nullptr.
//! @retval true The cast was successful, derived is a valid pointer.
//! @retval false The cast failed, derived is nullptr.
template<typename TDerived, typename TBase>
bool tryCast(const std::unique_ptr<TBase> &obj,
             TDerived *&derived) noexcept
{
    derived = dynamic_cast<TDerived *>(obj.get());

    return derived != nullptr;
}

//! @brief Wraps a dynamic cast in something more convenient.
//! @tparam TBase The base class data type. This can be inferred from
//! the parameters.
//! @tparam TDerived The derived class type which should inherit from
//! TBase. This can be inferred from the parameters.
//! @param[in] obj A pointer to an object to attempt to cast.
//! @param[out] derived Receives a pointer to the converted object or nullptr.
//! @retval true The cast was successful, derived is a valid pointer.
//! @retval false The cast failed, derived is nullptr.
template<typename TDerived, typename TBase>
bool tryCast(const std::shared_ptr<TBase> &obj,
             std::shared_ptr<TDerived> &derived) noexcept
{
    derived = std::dynamic_pointer_cast<TDerived, TBase>(obj);

    return derived;
}

//! @brief Wraps a dynamic cast in something more convenient.
//! @tparam TBase The base class data type. This can be inferred from
//! the parameters.
//! @tparam TDerived The derived class type which should inherit from
//! TBase. This can be inferred from the parameters.
//! @param[in] obj A pointer to an object to attempt to cast.
//! @param[out] derived Receives a pointer to the converted object or nullptr.
//! @retval true The cast was successful, derived is a valid pointer.
//! @retval false The cast failed, derived is nullptr.
template<typename TDerived, typename TBase>
bool tryCast(const std::shared_ptr<TBase> &obj,
             TDerived *&derived) noexcept
{
  derived = std::dynamic_pointer_cast<TDerived>(obj.get());

    return derived != nullptr;
}

//! @brief Wraps a dynamic cast in something more convenient.
//! @tparam TBase The base class data type. This can be inferred from
//! the parameters.
//! @tparam TDerived The derived class type which should inherit from
//! TBase. This can be inferred from the parameters.
//! @param[in] obj A pointer to an object to attempt to cast.
//! @param[out] derived Receives a pointer to the converted object or nullptr.
//! @retval true The cast was successful, derived is a valid pointer.
//! @retval false The cast failed, derived is nullptr.
template<typename TDerived, typename TBase>
bool tryCast(const TBase *obj, const TDerived *&derived)
{
    derived = dynamic_cast<const TDerived *>(obj);

    return derived != nullptr;
}

//! @brief Test to see if a pointer to an object is to a specified
//! derived data type.
//! @tparam T The data type to test for.
//! @tparam U The data type of the pointer to test.
//! @param[in] basePtr The pointer to test to see if it is derived from type T.
//! @retval true basePtr is derived from type T and can be safely cast.
//! @retval false Either basePtr was null or not derived from type T.
template<typename T, typename U>
bool isType(const U *basePtr)
{
    return dynamic_cast<const T *>(basePtr) != nullptr;
}

//! @brief Test to see if a shared pointer to an object is to a specified
//! derived data type.
//! @tparam T The data type to test for.
//! @tparam U The data type of the pointer to test.
//! @param[in] basePtr The pointer to test to see if it is derived from type T.
//! @retval true basePtr is derived from type T and can be safely cast.
//! @retval false Either basePtr was null or not derived from type T.
template<typename T, typename U>
bool isType(const std::shared_ptr<U> &basePtr)
{
    return dynamic_cast<const T *>(basePtr.get()) != nullptr;
}

//! @brief Overwrites a POD structure with zeros.
//! @tparam T The data type of the object.
//! @param[in] object %A reference to the object to fill.
template<typename T> void zeroFill(T &object)
{
    std::memset(static_cast<void *>(&object), 0, sizeof(T));
}

//! @brief Converts a scoped enumeration value to its underlying scalar type.
//! @tparam T The data type of the enumeration.
//! @param[in] value The enumeration value to cast.
//! @returns The scalar integer value corresponding to the enumeration value.
template<typename TEnum, std::enable_if_t<std::is_enum<TEnum>::value, bool> = true>
constexpr typename std::underlying_type<TEnum>::type toScalar(TEnum value) noexcept
{
    return static_cast<typename std::underlying_type<TEnum>::type>(value);
}

//! @brief Converts an integer value to a scoped enumeration type.
//! @tparam T The data type of the scoped enumeration to convert to.
//! @param[in] scalar The integer representation of the value.
//! @return The same value represented as a scoped enumeration.
template<typename TEnum, std::enable_if_t<std::is_enum<TEnum>::value, bool> = true>
constexpr TEnum fromScalar(typename std::underlying_type<TEnum>::type scalar) noexcept
{
    return static_cast<TEnum>(scalar);
}

//! @brief Force converts an integer value to a scoped enumeration type.
//! @tparam T The data type of the scoped enumeration to convert to.
//! @tparam U The data type of the scalar value to force convert.
//! @param[in] scalar The integer representation of the value.
//! @return The same value represented as a scoped enumeration.
template<typename TEnum, typename U,
         std::enable_if_t<std::is_enum<TEnum>::value, bool> = true>
constexpr TEnum forceFromScalar(U scalar) noexcept
{
    return static_cast<TEnum>(static_cast<typename std::underlying_type<TEnum>::type>(scalar));
}

//! @brief Rounds a real value to the nearest integer.
//! @tparam TDest The target integer scalar type.
//! @tparam TSrc The source real scalar type.
//! @param[in] realValue The real value to be rounded.
//! @return The realValue rounded and cast to an integer.
template<typename TDest, typename TSrc>
TDest roundNearest(TSrc realValue)
{
    return static_cast<TDest>(std::rint(realValue));
}

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

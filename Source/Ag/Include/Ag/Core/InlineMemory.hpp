//! @file Ag/Core/InlineMemory.hpp
//! @brief The declaration of objects which measure and initialise blocks of
//! memory packed with fields.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_INLINE_MEMORY_HPP__
#define __AG_CORE_INLINE_MEMORY_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <list>
#include <string>
#include <vector>

#include "Configuration.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class String;

//! @brief Encapsulates a field allocated in a buffer.
struct InlineField
{
    // Public Fields
    //! @brief The bytes offset of the field within the buffer.
    size_t Offset;

    //! @brief The size of the field in bytes or elements, whichever is appropriate.
    size_t Count;

    // Construction
    InlineField();
    InlineField(size_t offset, size_t byteCount);

    // Accessors
    bool isEmpty() const;
};

//! @brief An object which allocates space for fields within a non-existent
//! linear block of memory.
class InlineAllocator
{
public:
    // Construction
    InlineAllocator();
    InlineAllocator(size_t initialSize);

    // Accessors
    size_t getSize() const;

    // Operations
    void align(size_t size);
    void alignPow2(int32_t sizePow2);
    InlineField allocateRaw(size_t byteCount);
    InlineField allocate(utf8_cptr_t utf8Text);
    InlineField allocate(const std::string &utf8Text);
    InlineField allocate(const std::string_view &utf8Text);
    InlineField allocate(const String &utf8Text);
    InlineField allocateUTF8(const std::u16string &utf16Text);
    InlineField allocateUTF8(const utf16_cptr_t utf16Text);
    InlineField allocateUTF8(const std::u32string &utf32Text);
    InlineField allocateUTF8(const utf32_cptr_t &utf32Text);
    InlineField allocateUTF16(const std::string &utf8Text);
    InlineField allocateUTF16(const std::string_view &utf8Text);
    InlineField allocateUTF16(const String &utf8Text);
    InlineField allocateUTF16(const utf8_cptr_t utf8Text);
    InlineField allocateUTF16(const std::u32string &utf32Text);
    InlineField allocateUTF16(const utf32_cptr_t &utf32Text);
    InlineField allocateUTF32(const std::string_view &utf8Text);
    InlineField allocateUTF32(const String &utf8Text);
    InlineField allocateUTF32(const std::string &utf8Text);
    InlineField allocateUTF32(const utf8_cptr_t utf8Text);
    InlineField allocateUTF32(const std::u16string &utf16Text);
    InlineField allocateUTF32(const utf16_cptr_t &utf16Text);
    InlineField allocateArray(size_t elementCount, size_t elementSize);

    template<typename T> InlineField allocateArray(size_t elementCount)
    {
        // Ensure the offset returned has an appropriate alignment.
        align(sizeof(T));

        InlineField field(_totalSize, sizeof(T) * elementCount);

        _totalSize += field.Count;

        return field;
    }

    template<typename T, size_t U> InlineField allocateArray(const T (&values)[U])
    {
        // Ensure the offset returned has an appropriate alignment.
        align(sizeof(T));

        InlineField field(_totalSize, sizeof(T) * U);

        _totalSize += field.Count;

        return field;
    }

    template<typename T> InlineField allocateArray(const std::vector<T> &collection)
    {
        // Ensure the offset returned has an appropriate alignment.
        align(sizeof(T));

        InlineField field(_totalSize, sizeof(T) * collection.size());

        _totalSize += field.Count;

        return field;
    }

    template<typename T> InlineField allocateArray(const std::list<T> &collection)
    {
        // Ensure the offset returned has an appropriate alignment.
        align(sizeof(T));

        InlineField field(_totalSize, sizeof(T) * collection.size());

        _totalSize += field.Count;

        return field;
    }
private:
    // Internal Fields
    size_t _totalSize;
};

//! @brief An object which helps initialise a pre-allocated buffer of raw bytes.
class InlineInitialiser
{
public:
    // Construction
    InlineInitialiser(void *buffer, size_t byteCount);

    // Accessors
    size_t getSize() const;
    void *getFieldData(const InlineField &field) const;

    //! @brief Attempts to get a pointer to a typed field within the buffer.
    //! @tparam T The data type of the field to reference.
    //! @param[in] field An object describing the field location and size.
    //! @param[out] reference Receives a pointer to the field if successful.
    //! @param[out] safeCount Receives the maximum number of objects of type T
    //! which can be stored in the field.
    //! @retval true The field is within the buffer and can hold at least one
    //! element of type T. The reference and safeCount values are updated.
    //! @retval false The field is either beyond the end of the buffer or is
    //! too small to hold an element of type T.
    template<typename T> bool tryGetSafeFieldData(const InlineField &field,
                                                  T *&reference,
                                                  size_t &safeCount) const
    {
        bool isOK = true;

        if (field.Offset < _byteCount)
        {
            size_t maxFieldSize = std::min(_byteCount - field.Offset,
                                           field.Count);
            safeCount = maxFieldSize / sizeof(T);

            if (safeCount > 0)
            {
                // At least one element will fit in the buffer, return the
                // address of the field.
                reference = reinterpret_cast<T *>(_buffer + field.Offset);
            }
            else
            {
                // There is not enough space in the buffer for an object of the
                // requested size.
                reference = nullptr;
                isOK = false;
            }
        }
        else
        {
            // The field is beyond the end of the buffer.
            reference = nullptr;
            safeCount = 0;
            isOK = false;
        }

        return isOK;
    }

    // Operations
    void *initialiseRaw(const InlineField &field, const void *sourceData, size_t byteCount);
    utf8_ptr_t initialiseField(const InlineField &field, utf8_cptr_t utf8Text);
    utf8_ptr_t initialiseField(const InlineField &field, const std::string &utf8Text);
    utf8_ptr_t initialiseField(const InlineField &field, const std::string_view &utf8Text);
    utf8_ptr_t initialiseField(const InlineField &field, const String &utf8Text);
    utf8_ptr_t initialiseFieldUTF8(const InlineField &field, const std::u16string &utf16Text);
    utf8_ptr_t initialiseFieldUTF8(const InlineField &field, const utf16_cptr_t utf16Text);
    utf8_ptr_t initialiseFieldUTF8(const InlineField &field, const std::u32string &utf32Text);
    utf8_ptr_t initialiseFieldUTF8(const InlineField &field, const utf32_cptr_t utf32Text);
    utf16_ptr_t initialiseFieldUTF16(const InlineField &field, const std::string &utf8Text);
    utf16_ptr_t initialiseFieldUTF16(const InlineField &field, const std::string_view &utf8Text);
    utf16_ptr_t initialiseFieldUTF16(const InlineField &field, const String &utf8Text);
    utf16_ptr_t initialiseFieldUTF16(const InlineField &field, const utf8_cptr_t utf8Text);
    utf16_ptr_t initialiseFieldUTF16(const InlineField &field,
                                     const utf8_cptr_t utf8Text, size_t byteCount);
    utf16_ptr_t initialiseFieldUTF16(const InlineField &field, const std::u32string &utf32Text);
    utf16_ptr_t initialiseFieldUTF16(const InlineField &field, const utf32_cptr_t utf32Text);
    utf16_ptr_t initialiseFieldUTF16(const InlineField &field,
                                     const utf32_cptr_t utf32Text,
                                     size_t codePointCount);
    utf32_ptr_t initialiseFieldUTF32(const InlineField &field, const std::string_view &utf8Text);
    utf32_ptr_t initialiseFieldUTF32(const InlineField &field, const String &utf8Text);
    utf32_ptr_t initialiseFieldUTF32(const InlineField &field, const std::string &utf8Text);
    utf32_ptr_t initialiseFieldUTF32(const InlineField &field, const utf8_cptr_t utf8Text);
    utf32_ptr_t initialiseFieldUTF32(const InlineField &field,
                                     const utf8_cptr_t utf8Text,
                                     size_t byteCount);
    utf32_ptr_t initialiseFieldUTF32(const InlineField &field, const std::u16string &utf16Text);
    utf32_ptr_t initialiseFieldUTF32(const InlineField &field, const utf16_cptr_t utf16Text);
    utf32_ptr_t initialiseFieldUTF32(const InlineField &field,
                                     const utf16_cptr_t utf16Text,
                                     size_t wordCount);

    void *initialiseArray(const InlineField &field, const void *elements,
                          size_t elementCount, size_t elementSize);

    //! @brief Initialises an array of type elements in the buffer.
    //! @tparam T The data type of the elements being initialised.
    //! @param[in] field A description of the field pre-allocated in the buffer
    //! to copy the data to.
    //! @param[in] elements The array of values to copy.
    //! @param[in] elementCount The count of elements to copy.
    //! @return A pointer to the initialised field within the buffer, nullptr
    //! if the field was outside the buffer or too small.
    //! @details The maximum number of elements supported by the field allocated
    //! in the buffer will be copied, possibly none.
    template<typename T> T *initialiseArray(const InlineField &field,
                                            const T *elements, size_t elementCount)
    {
        T *data;
        size_t safeCount;

        if (tryGetSafeFieldData(field, data, safeCount))
        {
            std::uninitialized_copy_n(elements, std::min(safeCount, elementCount), data);
        }

        return data;
    }

    //! @brief Initialises an array of type elements in the buffer.
    //! @tparam T The data type of the elements being initialised.
    //! @tparam U The count of elements in the array to copy.
    //! @param[in] field A description of the field pre-allocated in the buffer
    //! to copy the data to.
    //! @param[in] elements The array of values to copy.
    //! @return A pointer to the initialised field within the buffer, nullptr
    //! if the field was outside the buffer or too small.
    //! @details The maximum number of elements supported by the field allocated
    //! in the buffer will be copied, possibly none.
    template<typename T, size_t U> T *initialiseArray(const InlineField &field,
                                                      const T (&elements)[U])
    {
        T *data;
        size_t safeCount;

        if (tryGetSafeFieldData(field, data, safeCount))
        {
            std::uninitialized_copy_n(elements, std::min(safeCount, U), data);
        }

        return data;
    }

    //! @brief Initialises an array of type elements in the buffer.
    //! @tparam T The data type of the elements being initialised.
    //! @tparam UAlloc The allocator type associated with the STL vector, this
    //! value can be deduced by the compiler.
    //! @param[in] field A description of the field pre-allocated in the buffer
    //! to copy the data to.
    //! @param[in] collection An STL vector of values to copy.
    //! @return A pointer to the initialised field within the buffer, nullptr
    //! if the field was outside the buffer or too small.
    //! @details The maximum number of elements supported by the field allocated
    //! in the buffer will be copied, possibly none.
    template<typename T, typename UAlloc>
    T *initialiseArray(const InlineField &field,
                       const std::vector<T, UAlloc> &collection)
    {
        T *data;
        size_t safeCount;

        if (tryGetSafeFieldData(field, data, safeCount))
        {
            std::uninitialized_copy_n(collection.begin(),
                                      std::min(safeCount, collection.size()),
                                      data);
        }

        return data;
    }

    //! @brief Initialises an array of type elements in the buffer.
    //! @tparam T The data type of the elements being initialised.
    //! @tparam UAlloc The allocator type associated with the STL list, this value
    //! can be deduced by the compiler.
    //! @param[in] field A description of the field pre-allocated in the buffer
    //! to copy the data to.
    //! @param[in] collection An STL list of values to copy.
    //! @return A pointer to the initialised field within the buffer, nullptr
    //! if the field was outside the buffer or too small.
    //! @details The maximum number of elements supported by the field allocated
    //! in the buffer will be copied, possibly none.
    template<typename T, typename UAlloc>
    T *initialiseArray(const InlineField &field,
                       const std::list<T, UAlloc> &collection)
    {
        T *data;
        size_t safeCount;

        if (tryGetSafeFieldData(field, data, safeCount))
        {
            std::uninitialized_copy_n(collection.begin(),
                                      std::min(safeCount, collection.size()),
                                      data);
        }

        return data;
    }

private:
    uint8_ptr_t _buffer;
    size_t _byteCount;
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

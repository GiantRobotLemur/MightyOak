//! @file Core/InlineMemory.cpp
//! @brief The definition of various functions and tools which operator on raw
//! memory.
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

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/InlineMemory.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty field description.
InlineField::InlineField() :
    Offset(0),
    Count(0)
{
}

//! @brief Constructs an initialised field description.
//! @param[in] offset The offset in bytes of the field from the beginning of
//! the block.
//! @param[in] byteCount The size of the field in bytes.
InlineField::InlineField(size_t offset, size_t byteCount) :
    Offset(offset),
    Count(byteCount)
{
}

//! @brief Determines whether the field is empty of data.
//! @retval true The field contains no data.
//! @retval false The field contains at least one byte of data.
bool InlineField::isEmpty() const
{
    return Count == 0;
}

//! @brief Creates an allocator with no initial size requirements.
InlineAllocator::InlineAllocator() :
    _totalSize(0)
{
}

//! @brief Create an allocator with an initial size requirement.
//! @param[in] initialSize The initial size requirement of the allocator.
InlineAllocator::InlineAllocator(size_t initialSize) :
    _totalSize(initialSize)
{
}

//! @brief Gets the current size requirement of all fields allocated.
size_t InlineAllocator::getSize() const
{
    return _totalSize;
}

//! @brief Ensures the next field is at an offset aligned for objects of a
//! specified size.
//! @param[in] size The alignment size requirement.
void InlineAllocator::align(size_t size)
{
    int pow2 = Bin::log2(size, true);

    alignPow2(pow2);
}

//! @brief Ensures the next field is at an offset aligned for objects of a
//! specified size.
//! @param[in] sizePow2 The alignment size requirement expressed as a power of 2.
void InlineAllocator::alignPow2(int32_t sizePow2)
{
    _totalSize = Bin::roundUpPow2(_totalSize, sizePow2);
}

//! @brief Allocates space for a field of a specified size.
//! @param[in] byteCount The size requirement of the field in bytes.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateRaw(size_t byteCount)
{
    InlineField field(_totalSize, byteCount);

    _totalSize += byteCount;

    return field;
}

//! @brief Allocates space for a field containing a null-terminated UTF-8
//! encoded character array.
//! @param[in] utf8Text The array of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocate(utf8_cptr_t utf8Text)
{
    size_t length = (utf8Text == nullptr) ? 0 : std::strlen(utf8Text);

    // Add space for the terminating null character.
    ++length;

    InlineField field(_totalSize, sizeof(char) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field containing an STL UTF-8 encoded
//! character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocate(const std::string &utf8Text)
{
    size_t length = utf8Text.length();

    // Add space for the terminating null character.
    ++length;

    InlineField field(_totalSize, sizeof(char) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field containing a view of a UTF-8 encoded
//! character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocate(const std::string_view &utf8Text)
{
    size_t length = utf8Text.length();

    // Add space for the terminating null character.
    ++length;

    InlineField field(_totalSize, sizeof(char) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field containing an immutable UTF-8 encoded
//! character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocate(const String &utf8Text)
{
    size_t length = utf8Text.getUtf8Length();

    // Add space for the terminating null character.
    ++length;

    InlineField field(_totalSize, sizeof(char) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-8
//! encoded character array initialised from an STL UTF-16 string.
//! @param[in] utf16Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF8(const std::u16string &utf16Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf16Text, Utf::Utf8) + 1;
    InlineField field(_totalSize, sizeof(char) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-8
//! encoded character array initialised from a null-terminated array of
//! UTF-16 characters.
//! @param[in] utf16Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF8(const utf16_cptr_t utf16Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf16Text, Utf::Utf8) + 1;

    InlineField field(_totalSize, sizeof(char) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-8
//! encoded character array initialised from a STL UTF-32 character string.
//! @param[in] utf32Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF8(const std::u32string &utf32Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf32Text, Utf::Utf8) + 1;

    InlineField field(_totalSize, sizeof(char) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-8
//! encoded character array initialised from a null-terminated array of
//! UTF-32 code points.
//! @param[in] utf32Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF8(const utf32_cptr_t &utf32Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf32Text, Utf::Utf8) + 1;

    InlineField field(_totalSize, sizeof(char) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-16
//! encoded character array initialised from a STL UTF-8 character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF16(const std::string &utf8Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf8Text, Utf::Utf16) + 1;

    InlineField field(_totalSize, sizeof(char16_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-16
//! encoded character array initialised from a view of a bounded UTF-8
//! character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF16(const std::string_view &utf8Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf8Text.data(), Utf::Utf16) + 1;

    InlineField field(_totalSize, sizeof(char16_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-16
//! encoded character array initialised from an immutable UTF-8
//! character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF16(const String &utf8Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = utf8Text.getUtf16Length() + 1;

    InlineField field(_totalSize, sizeof(char16_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-16
//! encoded character array initialised from a null terminated array of UTF-8
//! encoded bytes.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF16(const utf8_cptr_t utf8Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf8Text, Utf::Utf16) + 1;

    InlineField field(_totalSize, sizeof(char16_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-16
//! encoded character array initialised from an immutable UTF-32
//! character string.
//! @param[in] utf32Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF16(const std::u32string &utf32Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf32Text, Utf::Utf16) + 1;

    InlineField field(_totalSize, sizeof(char16_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-16
//! encoded character array initialised from a null terminated array of UTF-32
//! code points.
//! @param[in] utf32Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF16(const utf32_cptr_t &utf32Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length =Utf::calculateConvertedLength(utf32Text, Utf::Utf16) + 1;

    InlineField field(_totalSize, sizeof(char16_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-32
//! encoded character array initialised from a STL UTF-8 character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF32(const std::string &utf8Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf8Text, Utf::Utf32) + 1;

    InlineField field(_totalSize, sizeof(char32_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-32
//! encoded character array initialised from a view of a bounded UTF-8
//! character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF32(const std::string_view &utf8Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf8Text.data(), Utf::Utf32) + 1;

    InlineField field(_totalSize, sizeof(char32_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-32
//! encoded character array initialised from an immutable UTF-8
//! character string.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF32(const String &utf8Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = utf8Text.getUtf32Length() + 1;

    InlineField field(_totalSize, sizeof(char32_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-32
//! encoded character array initialised from a null terminated array of UTF-8
//! encoded bytes.
//! @param[in] utf8Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF32(const utf8_cptr_t utf8Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf8Text, Utf::Utf32) + 1;

    InlineField field(_totalSize, sizeof(char32_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-32
//! encoded character array initialised from an STL UTF-16
//! character string.
//! @param[in] utf16Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF32(const std::u16string &utf16Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf16Text, Utf::Utf32) + 1;

    InlineField field(_totalSize, sizeof(char32_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for a field encoded as a null terminated UTF-32
//! encoded character array initialised from a null terminated array of UTF-16
//! characters.
//! @param[in] utf16Text The string of characters to allocate space for,
//! including space for a terminating null character.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
InlineField InlineAllocator::allocateUTF32(const utf16_cptr_t &utf16Text)
{
    // Ensure there is at least enough space for a terminating null character.
    size_t length = Utf::calculateConvertedLength(utf16Text, Utf::Utf32) + 1;

    InlineField field(_totalSize, sizeof(char32_t) * length);

    _totalSize += field.Count;

    return field;
}

//! @brief Allocates space for an array of fixed sized elements.
//! @param[in] elementCount The count of elements to allocate space for.
//! @param[in] elementSize The size of each element.
//! @return An object describing the offset and size of the field in the buffer
//! to be allocated.
//! @details The elementSize value is used to ensure that the field is allocated
//! on an appropriately aligned address boundary.
InlineField InlineAllocator::allocateArray(size_t elementCount, size_t elementSize)
{
    align(elementSize);

    InlineField field(_totalSize, elementSize * elementCount);

    _totalSize += field.Count;

    return field;
}

//! @brief Constructs an object which can safely initialise pre-allocated fields
//! in a raw byte buffer.
//! @param[in] buffer A pointer to the buffer to initialise.
//! @param[in] byteCount The count of bytes in the buffer.
InlineInitialiser::InlineInitialiser(void *buffer, size_t byteCount) :
    _buffer(reinterpret_cast<uint8_t *>(buffer)),
    _byteCount(byteCount)
{
    if ((_buffer != nullptr) && (_byteCount > 0))
    {
        // Ensure the buffer is initialised to a known state before use.
        std::memset(_buffer, 0, _byteCount);
    }
}

//! @brief Gets the size of the buffer in bytes.
size_t InlineInitialiser::getSize() const
{
    return _byteCount;
}

//! @brief Gets a raw pointer to a field within the buffer without performing
//! any range checks.
//! @param[in] field A description of the field to get.
//! @return A pointer to the field within the buffer.
void *InlineInitialiser::getFieldData(const InlineField &field) const
{
    return _buffer + field.Offset;
}

//! @brief Copies an array of raw bytes to the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] sourceData A pointer to a raw array of bytes to copy.
//! @param[in] byteCount The array of bytes to copy.
//! @return A pointer to the field, if it was initialised.
void *InlineInitialiser::initialiseRaw(const InlineField &field,
                                 const void *sourceData, size_t byteCount)
{
    uint8_t *data = nullptr;
    size_t safeSize = 0;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        std::memcpy(data, sourceData, std::min(safeSize, byteCount));
    }

    return data;
}

//! @brief Copies a UTF-8 encoded string to a pre-allocated field in the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The null-terminated array of UTF-8 characters to copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf8_ptr_t InlineInitialiser::initialiseField(const InlineField &field,
                                              utf8_cptr_t utf8Text)
{
    char *data = nullptr;
    size_t safeSize = 0;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t length = std::min(Utf::calculateLength(utf8Text),
                                 safeSize - 1);
        std::memcpy(data, utf8Text, length);

        // Ensure the string is properly null terminated.
        data[length] = '\0';
    }

    return data;
}

//! @brief Copies a UTF-8 encoded string to a pre-allocated field in the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The STL UTF-8 character string to copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf8_ptr_t InlineInitialiser::initialiseField(const InlineField &field,
                                              const std::string &utf8Text)
{
    char *data = nullptr;
    size_t safeSize = 0;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t length = std::min(safeSize - 1, utf8Text.length());

        std::memcpy(data, utf8Text.c_str(), length);

        // Ensure the string is properly null terminated.
        data[length] = '\0';
    }

    return data;
}

//! @brief Copies a UTF-8 encoded string to a pre-allocated field in the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The view of a bounded UTF-8 character string to copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf8_ptr_t InlineInitialiser::initialiseField(const InlineField &field, const std::string_view &utf8Text)
{
    char *data = nullptr;
    size_t safeSize = 0;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t length = std::min(safeSize - 1, utf8Text.length());

        std::memcpy(data, utf8Text.data(), length);

        // Ensure the string is properly null terminated.
        data[length] = '\0';
    }

    return data;
}

//! @brief Copies a UTF-8 encoded string to a pre-allocated field in the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The immutable UTF-8 character string to copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf8_ptr_t InlineInitialiser::initialiseField(const InlineField &field,
                                              const String &utf8Text)
{
    char *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t length = std::min(safeSize - 1, utf8Text.getUtf8Length());

        std::memcpy(data, utf8Text.getUtf8Bytes(), length);

        // Ensure the string is properly null terminated.
        data[length] = '\0';
    }

    return data;
}

//! @brief Copies and converts an STL UTF-16 encoded string to a pre-allocated
//! UTF-8 field in the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf16Text The STL UTF-16 character string to copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf8_ptr_t InlineInitialiser::initialiseFieldUTF8(const InlineField &field,
                                                  const std::u16string &utf16Text)
{
    char *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t charCount = (safeSize / sizeof(char16_t)) - 1;
        size_t index = 0;
        Utf::FromUtf16Converter inputConverter;
        char32_t codePoint = U'\0';
        bool hasError = false;

        for (char16_t next : utf16Text)
        {
            if (inputConverter.tryConvert(next, codePoint, hasError))
            {
                inputConverter.reset();
                uint32_t byteCount;

                if (Utf::tryGetUTF8ByteCountFromCodePoint(codePoint, byteCount))
                {
                    if ((index + byteCount) <= charCount)
                    {
                        break;
                    }

                    Utf::ToUtf8Converter outputConverter;
                    outputConverter.setCodePoint(codePoint);
                    uint8_t nextByte = 0;

                    while (outputConverter.tryGetNextByte(nextByte))
                    {
                        data[index++] = static_cast<char>(nextByte);
                    }
                }
            }
            else if (hasError)
            {
                inputConverter.reset();
            }
        }

        // Ensure the buffer is properly null terminated.
        data[index] = '\0';
    }

    return data;
}

//! @brief Copies and converts an null terminated UTF-16 encoded string to a
//! pre-allocated UTF-8 field in the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf16Text The UTF-16 character string to copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf8_ptr_t InlineInitialiser::initialiseFieldUTF8(const InlineField &field, const utf16_cptr_t utf16Text)
{
    char *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t outputIndex = 0;
        size_t charCount = safeSize - 1;
        Utf::FromUtf16Converter inputConverter;
        char32_t codePoint = U'\0';
        bool hasError = false;

        for (size_t inputIndex = 0; utf16Text[inputIndex] != u'\0'; ++inputIndex)
        {
            if (inputConverter.tryConvert(utf16Text[inputIndex],
                                          codePoint, hasError))
            {
                inputConverter.reset();
                uint32_t byteCount;

                if (Utf::tryGetUTF8ByteCountFromCodePoint(codePoint, byteCount))
                {
                    if ((outputIndex + byteCount) <= charCount)
                    {
                        break;
                    }

                    Utf::ToUtf8Converter outputConverter;
                    outputConverter.setCodePoint(codePoint);
                    uint8_t nextByte = 0;

                    while (outputConverter.tryGetNextByte(nextByte))
                    {
                        data[outputIndex++] = static_cast<char>(nextByte);
                    }
                }
            }
            else if (hasError)
            {
                inputConverter.reset();
            }
        }

        // Ensure the buffer is properly null terminated.
        data[outputIndex] = '\0';
    }

    return data;
}

//! @brief Copies and converts an STL UTF-32 encoded string to a pre-allocated
//! UTF-8 field in the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf32Text The STL UTF-32 character string to copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf8_ptr_t InlineInitialiser::initialiseFieldUTF8(const InlineField &field,
                                            const std::u32string &utf32Text)
{
    char *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t charCount = (safeSize / sizeof(char32_t)) - 1;
        size_t index = 0;

        for (char32_t next : utf32Text)
        {
            uint32_t byteCount = 0;

            if (Utf::tryGetUTF8ByteCountFromCodePoint(next, byteCount))
            {
                if ((index + byteCount) <= charCount)
                {
                    break;
                }

                Utf::ToUtf8Converter outputConverter;
                outputConverter.setCodePoint(next);
                uint8_t nextByte = 0;

                while (outputConverter.tryGetNextByte(nextByte))
                {
                    data[index++] = static_cast<char>(nextByte);
                }
            }
        }

        // Ensure the buffer is properly null terminated.
        data[index] = '\0';
    }

    return data;
}

//! @brief Copies and converts an null terminated UTF-32 encoded string to a
//! pre-allocated UTF-8 field in the buffer.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf32Text The UTF-32 character string to copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf8_ptr_t InlineInitialiser::initialiseFieldUTF8(const InlineField &field,
                                                  const utf32_cptr_t utf32Text)
{
    char *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t outputIndex = 0;
        size_t charCount = safeSize - 1;

        for (size_t inputIndex = 0; utf32Text[inputIndex] != u'\0'; ++inputIndex)
        {
            char32_t codePoint = utf32Text[inputIndex];
            uint32_t byteCount = 0;

            if (Utf::tryGetUTF8ByteCountFromCodePoint(codePoint, byteCount))
            {
                if ((outputIndex + byteCount) <= charCount)
                {
                    break;
                }

                Utf::ToUtf8Converter outputConverter;
                outputConverter.setCodePoint(codePoint);
                uint8_t nextByte = 0;

                while (outputConverter.tryGetNextByte(nextByte))
                {
                    data[outputIndex++] = static_cast<char>(nextByte);
                }
            }
        }

        // Ensure the buffer is properly null terminated.
        data[outputIndex] = '\0';
    }

    return data;
}

//! @brief Initialises a pre-allocated UTF-16 encoded string field from an STL
//! UTF-8 encoded string.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf16_ptr_t InlineInitialiser::initialiseFieldUTF16(const InlineField &field,
                                              const std::string &utf8Text)
{
    return initialiseFieldUTF16(field, utf8Text.c_str(), utf8Text.length());
}

//! @brief Initialises a pre-allocated UTF-16 encoded string field from a
//! read-only bounded view of a UTF-8 encoded string.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The view of the string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf16_ptr_t InlineInitialiser::initialiseFieldUTF16(const InlineField &field,
                                                    const std::string_view &utf8Text)
{
    return initialiseFieldUTF16(field, utf8Text.data(),
                                utf8Text.length());
}

//! @brief Initialises a pre-allocated UTF-16 encoded string field from an
//! immutable UTF-8 encoded string object.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf16_ptr_t InlineInitialiser::initialiseFieldUTF16(const InlineField &field,
                                                    const String &utf8Text)
{
    return initialiseFieldUTF16(field, utf8Text.getUtf8Bytes(),
                                utf8Text.getUtf8Length());
}

//! @brief Initialises a pre-allocated UTF-16 encoded string field from a
//! null-terminated array of UTF-8 encoded bytes.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf16_ptr_t InlineInitialiser::initialiseFieldUTF16(const InlineField &field,
                                              const utf8_cptr_t utf8Text)
{
    size_t length = Utf::calculateLength(utf8Text);

    return initialiseFieldUTF16(field, utf8Text, length);
}

//! @brief Initialises a pre-allocated UTF-16 encoded string field from a
//! bounded array of UTF-8 encoded bytes.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @param[in] byteCount The count of bytes in utf8Text, not including the
//! terminating null character.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf16_ptr_t InlineInitialiser::initialiseFieldUTF16(const InlineField &field,
                                                    const utf8_cptr_t utf8Text,
                                                    size_t byteCount)
{
    char16_t *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t charCount = (safeSize / sizeof(char16_t)) - 1;
        size_t outputIndex = 0;
        Utf::FromUtf8Converter inputConverter;

        for (size_t inputIndex = 0;
             (inputIndex < byteCount) && (outputIndex < charCount);
             ++inputIndex)
        {
            char32_t codePoint = U'\0';
            bool hasError = false;

            if (inputConverter.tryConvert(utf8Text[inputIndex], codePoint,
                                          hasError))
            {
                uint32_t wordCount = 0;

                if (Utf::tryGetUTF16WordCountFromCodePoint(codePoint, wordCount))
                {
                    if ((outputIndex + wordCount) > charCount)
                    {
                        break;
                    }

                    Utf::ToUtf16Converter outputConverter;
                    outputConverter.setCodePoint(codePoint);
                    char16_t utf16Word = 0;

                    while (outputConverter.tryGetNextCharacter(utf16Word))
                    {
                        data[outputIndex++] = utf16Word;
                    }
                }
            }
            else if (hasError)
            {
                inputConverter.reset();
            }
        }

        // Ensure the string is properly null terminated.
        data[outputIndex] = u'\0';
    }

    return data;
}

//! @brief Initialises a pre-allocated UTF-16 encoded string field from an STL
//! UTF-32 encoded string.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf32Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf16_ptr_t InlineInitialiser::initialiseFieldUTF16(const InlineField &field,
                                              const std::u32string &utf32Text)
{
    return initialiseFieldUTF16(field, utf32Text.c_str(), utf32Text.length());
}

//! @brief Initialises a pre-allocated UTF-16 encoded string field from a
//! null-terminated array of UTF-32 code points.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf32Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf16_ptr_t InlineInitialiser::initialiseFieldUTF16(const InlineField &field, const utf32_cptr_t utf32Text)
{
    size_t length = Utf::calculateLength(utf32Text);

    return initialiseFieldUTF16(field, utf32Text, length);
}

//! @brief Initialises a pre-allocated UTF-16 encoded string field from a
//! bounded array of UTF-32 code points.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf32Text The string to convert and copy.
//! @param[in] codePointCount The count of Unicode code points in utf32Text,
//! not including the terminating null character.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf16_ptr_t InlineInitialiser::initialiseFieldUTF16(const InlineField &field,
                                                    const utf32_cptr_t utf32Text,
                                                    size_t codePointCount)
{
    char16_t *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t charCount = (safeSize / sizeof(char16_t)) - 1;
        size_t outputIndex = 0;
        Utf::FromUtf8Converter inputConverter;

        for (size_t inputIndex = 0;
             (inputIndex < codePointCount) && (outputIndex < charCount);
             ++inputIndex)
        {
            char32_t codePoint = utf32Text[inputIndex];
            uint32_t wordCount = 0;

            if (Utf::tryGetUTF16WordCountFromCodePoint(codePoint, wordCount))
            {
                if ((outputIndex + wordCount) > charCount)
                {
                    break;
                }

                Utf::ToUtf16Converter outputConverter;
                outputConverter.setCodePoint(codePoint);
                char16_t utf16Word = 0;

                while (outputConverter.tryGetNextCharacter(utf16Word))
                {
                    data[outputIndex++] = utf16Word;
                }
            }
        }

        // Ensure the string is properly null terminated.
        data[outputIndex] = u'\0';
    }

    return data;
}

//! @brief Initialises a pre-allocated UTF-32 encoded string field from a
//! UTF-8 encoded read-only view of a string.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf32_ptr_t InlineInitialiser::initialiseFieldUTF32(const InlineField &field,
                                              const std::string_view &utf8Text)
{
    return initialiseFieldUTF32(field, utf8Text.data(),
                                utf8Text.length());
}

//! @brief Initialises a pre-allocated UTF-32 encoded string field from a
//! UTF-8 encoded immutable string value.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf32_ptr_t InlineInitialiser::initialiseFieldUTF32(const InlineField &field,
                                                    const String &utf8Text)
{
    return initialiseFieldUTF32(field, utf8Text.getUtf8Bytes(),
                                utf8Text.getUtf8Length());
}

//! @brief Initialises a pre-allocated UTF-32 encoded string field from a
//! UTF-8 STL string object.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf32_ptr_t InlineInitialiser::initialiseFieldUTF32(const InlineField &field,
                                                    const std::string &utf8Text)
{
    return initialiseFieldUTF32(field, utf8Text.c_str(),
                                utf8Text.length());
}

//! @brief Initialises a pre-allocated UTF-32 encoded string field from a
//! null terminated array of UTF-8 encoded bytes.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf32_ptr_t InlineInitialiser::initialiseFieldUTF32(const InlineField &field, const utf8_cptr_t utf8Text)
{
    size_t length = Utf::calculateLength(utf8Text);

    return initialiseFieldUTF32(field, utf8Text, length);
}

//! @brief Initialises a pre-allocated UTF-32 encoded string field from a
//! bounded array of UTF-8 encoded bytes.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf8Text The string to convert and copy.
//! @param[in] byteCount The count of bytes in utf8Text, not including the
//! terminating null character.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf32_ptr_t InlineInitialiser::initialiseFieldUTF32(const InlineField &field,
                                              const utf8_cptr_t utf8Text,
                                              size_t byteCount)
{
    char32_t *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t charCount = (safeSize / sizeof(char32_t)) - 1;
        size_t outputIndex = 0;
        Utf::FromUtf8Converter inputConverter;

        for (size_t inputIndex = 0;
             (inputIndex < byteCount) && (outputIndex < charCount);
             ++inputIndex)
        {
            char32_t codePoint = U'\0';
            bool hasError = false;

            if (inputConverter.tryConvert(utf8Text[inputIndex], codePoint,
                                          hasError))
            {
                data[outputIndex++] = codePoint;
            }
            else if (hasError)
            {
                inputConverter.reset();
            }
        }

        // Ensure the string is properly null terminated.
        data[outputIndex] = U'\0';
    }

    return data;
}

//! @brief Initialises a pre-allocated UTF-32 encoded string field from a
//! UTF-16 encoded STL string object.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf16Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf32_ptr_t InlineInitialiser::initialiseFieldUTF32(const InlineField &field,
                                                    const std::u16string &utf16Text)
{
    return initialiseFieldUTF32(field, utf16Text.c_str(),
                                utf16Text.length());
}

//! @brief Initialises a pre-allocated UTF-32 encoded string field from a
//! null terminated UTF-16 encoded words.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf16Text The string to convert and copy.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf32_ptr_t InlineInitialiser::initialiseFieldUTF32(const InlineField &field,
                                                    const utf16_cptr_t utf16Text)
{
    size_t length = Utf::calculateLength(utf16Text);

    return initialiseFieldUTF32(field, utf16Text, length);
}

//! @brief Initialises a pre-allocated UTF-32 encoded string field from a
//! bounded array of UTF-16 encoded words.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] utf16Text The string to convert and copy.
//! @param[in] wordCount The count of bytes in utf8Text, not including the
//! terminating null character.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
utf32_ptr_t InlineInitialiser::initialiseFieldUTF32(const InlineField &field,
                                                    const utf16_cptr_t utf16Text,
                                                    size_t wordCount)
{
    char32_t *data;
    size_t safeSize;

    if (tryGetSafeFieldData(field, data, safeSize))
    {
        size_t charCount = (safeSize / sizeof(char32_t)) - 1;
        size_t outputIndex = 0;
        Utf::FromUtf16Converter inputConverter;
        char32_t codePoint = U'\0';
        bool hasError = false;

        for (size_t inputIndex = 0;
             (inputIndex < wordCount) && (outputIndex < charCount);
             ++inputIndex)
        {
            if (inputConverter.tryConvert(utf16Text[inputIndex], codePoint,
                                          hasError))
            {
                data[outputIndex++] = codePoint;
            }
            else if (hasError)
            {
                inputConverter.reset();
            }
        }

        // Ensure the string is properly null terminated.
        data[outputIndex] = U'\0';
    }

    return data;
}

//! @brief Initialises an array of POD data elements in a pre-allocated field.
//! @param[in] field A description of the field pre-allocated in the buffer to
//! copy the data to.
//! @param[in] elements The array of POD data elements to copy.
//! @param[in] elementCount The count of elements to copy.
//! @param[in] elementSize The size of each element being copied.
//! @return A pointer to the initialised field in the buffer, or nullptr if it
//! was beyond the buffer bounds.
void *InlineInitialiser::initialiseArray(const InlineField &field, const void *elements,
                                         size_t elementCount, size_t elementSize)
{
    void *data = nullptr;

    if (field.Offset < _byteCount)
    {
        size_t safeSize = std::min(field.Count, _byteCount - field.Offset);
        size_t safeCount = safeSize / elementSize;

        if (safeCount > 0)
        {
            data = offsetPtr<void>(_buffer, field.Offset);

            std::memcpy(data, elements, elementCount * elementSize);
        }
    }

    return data;
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


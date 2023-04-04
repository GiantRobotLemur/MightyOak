//! @file Core/StringPrivate.cpp
//! @brief The definition of the inner object of the Ag library string value type.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Utf.hpp"

#include "CoreInternal.hpp"
#include "StringPrivate.hpp"

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An exception thrown when invalid data is passed for Unicode
//! conversion.
class UnicodeConversionException : public Exception
{
public:
    //! @brief Constructs an exception documenting an invalid sequence of UTF-8
    //! bytes.
    //! @param[in] utf8ByteSequence An array of bytes which was invalid in the
    //! UTF-8 encoding.
    //! @param[in] byteCount The count of bytes in utf8ByteSequence.
    UnicodeConversionException(uint8_cptr_t utf8ByteSequence,
                               size_t byteCount)
    {
        std::string detail;

        detail.assign("The invalid byte sequence was { ");

        for (size_t index = 0; index < byteCount; ++index)
        {
            if (index > 0)
            {
                detail.push_back(',');
                detail.push_back(' ');
            }

            appendPrintf(detail, "0x%.2X", utf8ByteSequence[index]);
        }

        detail.append(" }.");

        initialise("UnicodeConversionException",
                   "The program attempted to convert an invalid sequence of "
                   "UTF-8 encoded bytes.", detail, 0);
    }

    //! @brief Constructs an exception documenting an invalid sequence of UTF-16
    //! characters.
    //! @param[in] utf16WordSequence An array of 16-bit characters which was
    //! invalid in the UTF-16 encoding.
    //! @param[in] byteCount The count of bytes in utf8ByteSequence.
    UnicodeConversionException(utf16_cptr_t utf16WordSequence,
                               size_t wordCount)
    {
        std::string detail;

        detail.assign("The invalid character sequence was { ");

        for (size_t index = 0; index < wordCount; ++index)
        {
            if (index > 0)
            {
                detail.push_back(',');
                detail.push_back(' ');
            }

            appendPrintf(detail, "0x%.4X", utf16WordSequence[index]);
        }

        detail.append(" }.");

        initialise("UnicodeConversionException",
                   "The program attempted to convert an invalid sequence of "
                   "UTF-16 encoded characters.", detail, 0);
    }

    //! @brief Constructs an exception documenting an attempt to encode an
    //! invalid Unicode code point.
    //! @param[in] codePoint The Unicode character which was invalid.
    UnicodeConversionException(char32_t codePoint)
    {
        std::string detail;

        appendPrintf(detail, "The invalid Unicode code point was: 0x%.6X.",
                     static_cast<uint32_t>(codePoint));

        initialise("UnicodeConversionException",
                   "The program attempted to convert an invalid sequence of "
                   "UTF-16 encoded characters.", detail, 0);
    }
};

} // TED

///////////////////////////////////////////////////////////////////////////////
// Member Functions
///////////////////////////////////////////////////////////////////////////////
//! Constructs an empty immutable string value.
StringPrivate::StringPrivate() :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
    // Copy the pointer after the STL string constructor has been called.
    _data = _localData.c_str();
}

//! @brief Constructs a string value which has been dynamically allocated to
//! allow the string data to be stored in-line.
//! @param[in] key The string value containing metadata and a pointer to the
//! non-local converted UTF-8 string data.
StringPrivate::StringPrivate(const StringPrivate &key) :
    std::enable_shared_from_this<StringPrivate>(key),
    _hashCode(key._hashCode),
    _utf8Length(key._utf8Length),
    _utf16Length(key._utf16Length),
    _utf32Length(key._utf32Length),
    _data(nullptr),
    _localData(key._data, key._utf8Length),
    _isDynamic(true)
{
    // Copy the pointer after the STL string constructor has been called.
    _data = _localData.c_str();
}

//! @brief Constructs a string value which has been allocated on the stack
//! from a null-terminated UTF-8 encoded array of bytes. This
//! storage will continue to be referenced for the lifetime of the object.
//! @param[in] nullTerminatedUtf8 The array of bytes to construct the key from.
StringPrivate::StringPrivate(utf8_cptr_t nullTerminatedUtf8) :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
    size_t length = Utf::calculateLength(nullTerminatedUtf8);

    initialiseUTF8(nullTerminatedUtf8, length);
}

//! @brief Constructs a string value which has been allocated on the stack
//! from a bounded array of UTF-8 encoded bytes.
//! @param[in] boundedUtf8 The array of bytes to construct the key from. This
//! storage will continue to be referenced for the lifetime of the object.
//! @param[in] byteCount The count of bytes in the array, not including any
//! terminating null bytes.
StringPrivate::StringPrivate(utf8_cptr_t boundedUtf8, size_t byteCount) :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
    initialiseUTF8(boundedUtf8, (boundedUtf8 == nullptr) ? 0 : byteCount);
}

//! @brief Constructs a string value which has been allocated on the stack
//! from a null-terminated array of UTF-16 characters.
//! @param[in] nullTerminatedUtf16 The array of characters to construct the
//! key from.
StringPrivate::StringPrivate(utf16_cptr_t nullTerminatedUtf16) :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
    size_t length = Utf::calculateLength(nullTerminatedUtf16);

    initialiseUTF16(nullTerminatedUtf16, length);
}

//! @brief Constructs a string value which has been allocated on the stack
//! from a bounded array of UTF-16 characters.
//! @param[in] boundedUtf16 The array of UTF-16 characters to construct the
//! key from.
//! @param[in] wordCount The count of 16-bit words in the array, not including any
//! terminating nulls.
StringPrivate::StringPrivate(utf16_cptr_t boundedUtf16, size_t wordCount) :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
    initialiseUTF16(boundedUtf16, (boundedUtf16 == nullptr) ? 0 : wordCount);
}

//! @brief Constructs a string value which has been allocated on the stack
//! from a null-terminated array of Unicode code points.
//! @param[in] nullTerminatedUtf32 The array of code points to construct the
//! key from.
StringPrivate::StringPrivate(utf32_cptr_t nullTerminatedUtf32) :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
    size_t length = Utf::calculateLength(nullTerminatedUtf32);

    initialiseUTF32(nullTerminatedUtf32, length);
}

//! @brief Constructs a string value which has been allocated on the stack
//! from a bounded array of Unicode code points.
//! @param[in] boundedUtf32 The array of bytes to construct the key from.
//! @param[in] codePointCount The count of code points in the array, not
//! including any terminating nulls.
StringPrivate::StringPrivate(utf32_cptr_t boundedUtf32, size_t codePointCount) :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
    initialiseUTF32(boundedUtf32, (boundedUtf32 == nullptr) ? 0 : codePointCount);
}

//! @brief Constructs a string value which has been allocated on the stack
//! from a null-terminated array of wide characters.
//! @param[in] nullTerminatedWide The array of wide characters to construct the
//! key from.
StringPrivate::StringPrivate(wchar_cptr_t nullTerminatedWide) :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
#ifdef WCHAR_IS_32BIT
    initialiseUTF32(reinterpret_cast<utf32_cptr_t>(nullTerminatedWide),
                    Utf::calculateLength(nullTerminatedWide));
#else
    initialiseUTF16(reinterpret_cast<utf16_cptr_t>(nullTerminatedWide),
                    Utf::calculateLength(nullTerminatedWide));
#endif
}

//! @brief Constructs a string value which has been allocated on the stack
//! from a bounded array of wide characters.
//! @param[in] boundedWide The array of wide characters to construct the key from.
//! @param[in] charCount The count of wide characters in the array, not including any
//! terminating nulls.
StringPrivate::StringPrivate(wchar_cptr_t boundedWide, size_t charCount) :
    _hashCode(0),
    _utf8Length(0),
    _utf16Length(0),
    _utf32Length(0),
    _data(nullptr),
    _isDynamic(false)
{
#ifdef WCHAR_IS_32BIT
    initialiseUTF32(reinterpret_cast<utf32_cptr_t>(boundedWide), charCount);
#else
    initialiseUTF16(reinterpret_cast<utf16_cptr_t>(boundedWide), charCount);
#endif
}

//! @brief Disposes of all resources owned by the object.
StringPrivate::~StringPrivate()
{
    if (_isDynamic)
    {
        disposeOfDynamicString(this);
    }
}

//! @brief Determines whether the string contains 0 characters.
bool StringPrivate::isEmpty() const
{
    return _utf8Length == 0;
}

//! @brief Gets the string data as UTF-8 encoded bytes.
const std::string &StringPrivate::getData() const
{
    return _localData;
}

//! @brief Gets a hash code calculated from the text data.
size_t StringPrivate::getHashCode() const
{
    return _hashCode;
}

//! @brief Gets the count of UTF-8 bytes which represent the string.
size_t StringPrivate::getUTF8Length() const
{
    return _utf8Length;
}

//! @brief Gets the count of UTF-16 16-bit words required to represent
//! the string.
size_t StringPrivate::getUTF16Length() const
{
    return _utf16Length;
}

//! @brief Gets the count of Unicode code points required to represent
//! the string.
size_t StringPrivate::getUTF32Length() const
{
    return _utf32Length;
}

//! @brief Determines if two string values are identical.
bool StringPrivate::operator==(const StringPrivate &rhs) const
{
    bool isEqual = false;

    if (&rhs == this)
    {
        // It's the same instance.
        isEqual = true;
    }
    else if ((_utf8Length == rhs._utf8Length) &&
             (_hashCode == rhs._hashCode))
    {
        isEqual = std::memcmp(_data, rhs._data, _utf8Length) == 0;
    }

    return isEqual;
}

//! @brief Performs shared initialisation of the object from a UTF-8
//! encoded array.
//! @param[in] boundedUtf8 The array of UTF-8 bytes to retain as the source
//! of the string.
//! @param[in] byteCount The count of bytes in the array.
void StringPrivate::initialiseUTF8(utf8_cptr_t boundedUtf8, size_t byteCount)
{
    _hashCode = 0;
    _utf8Length = byteCount;
    _utf16Length = 0;
    _utf32Length = 0;
    _data = boundedUtf8;
    _localData.clear();

    Utf::FromUtf8Converter converter;
    size_t codePointStartIndex = 0;

    for (size_t index = 0; index < byteCount; ++index)
    {
        bool hasError;
        char32_t codePoint;

        if (converter.tryConvert(boundedUtf8[index], codePoint, hasError))
        {
            _hashCode = Bin::rotateLeft(_hashCode, 7) ^ static_cast<size_t>(codePoint);

            codePointStartIndex = index + 1;

            // Update the sizes for various encodings.
            ++_utf32Length;

            uint wordCount = 0;

            if (Utf::tryGetUTF16WordCountFromCodePoint(codePoint, wordCount))
            {
                _utf16Length += wordCount;
            }
        }
        else if (hasError)
        {
            throw UnicodeConversionException(
                reinterpret_cast<uint8_cptr_t>(boundedUtf8 + codePointStartIndex),
                (index + 1) - codePointStartIndex);
        }
    }
}

//! @brief Performs shared initialisation of the object from a UTF-16
//! encoded array.
//! @param[in] boundedUtf16 The array of UTF-16 characters to convert into
//! UTF-8 as the source of the string.
//! @param[in] wordCount The count of 16-bit characters in the array.
void StringPrivate::initialiseUTF16(utf16_cptr_t boundedUtf16, size_t wordCount)
{
    _hashCode = 0;
    _utf8Length = 0;
    _utf16Length = 0;
    _utf32Length = 0;
    _data = nullptr;
    _localData.clear();

    Utf::FromUtf16Converter converter;
    Utf::ToUtf8Converter toUtf8;

    // "Guess" at probably UTF-8 length.
    _localData.reserve((wordCount * 110) / 100);

    size_t codePointStartIndex = 0;

    for (size_t index = 0; index < wordCount; ++index)
    {
        bool hasError = false;
        char32_t codePoint = 0;

        if (converter.tryConvert(boundedUtf16[index], codePoint, hasError))
        {
            _hashCode = Bin::rotateLeft(_hashCode, 7) ^ static_cast<size_t>(codePoint);

            _utf16Length += index + 1 - codePointStartIndex;
            ++_utf32Length;

            uint8_t nextByte;
            toUtf8.setCodePoint(codePoint);

            while (toUtf8.tryGetNextByte(nextByte))
            {
                _localData.push_back(static_cast<char>(nextByte));
            }

            codePointStartIndex = index + 1;
        }
        else if (hasError)
        {
            throw UnicodeConversionException(boundedUtf16 + codePointStartIndex,
                                             (index + 1) - codePointStartIndex);
        }
    }

    _utf8Length = _localData.length();
    _data = _localData.c_str();
}

//! @brief Performs shared initialisation of the object from an array of
//! Unicode code points.
//! @param[in] boundedUtf32 The array of code points to convert into
//! UTF-8 as the source of the string.
//! @param[in] codePointCount The count of Unicode characters in the array.
void StringPrivate::initialiseUTF32(utf32_cptr_t boundedUtf32, size_t codePointCount)
{
    _hashCode = 0;
    _utf8Length = 0;
    _utf16Length = 0;
    _utf32Length = 0;
    _data = nullptr;
    _localData.clear();

    Utf::ToUtf8Converter toUtf8;

    // "Guess" at the UTF-8 size of the string.
    _localData.reserve((codePointCount * 110) / 100);

    for (size_t index = 0; index < codePointCount; ++index)
    {
        char32_t codePoint = boundedUtf32[index];
        uint32_t codePointData = static_cast<uint32_t>(codePoint);

        if (codePointData < 0xD800)
        {
            _hashCode = Bin::rotateLeft(_hashCode, 7) ^ codePointData;
        }
        else if (codePointData < 0xE000)
        {
            // It's within the UTF-16 low/high surrogate range.
            throw UnicodeConversionException(codePoint);
        }
        else if (codePointData <= CodePointMax)
        {
            _hashCode = Bin::rotateLeft(_hashCode, 7) ^ codePointData;
        }
        else
        {
            // It's beyond the largest legal Unicode code point value.
            throw UnicodeConversionException(codePoint);
        }

        // Update various encoding lengths.
        ++_utf32Length;

        uint count = 0;

        if (Utf::tryGetUTF16WordCountFromCodePoint(codePoint, count))
        {
            _utf16Length += count;
        }

        toUtf8.setCodePoint(codePoint);
        uint8_t nextByte = 0;

        while(toUtf8.tryGetNextByte(nextByte))
        {
            _localData.push_back(static_cast<char>(nextByte));
        }
    }

    _utf8Length = _localData.length();
    _data = _localData.c_str();
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Compares two null-terminated strings without regard for case.
//! @param[in] lhs The first string to compare.
//! @param[in] rhs The second string to compare.
//! @retval <0 lhs has a lower value than rhs.
//! @retval 0 The value of lhs is equal to that of rhs.
//! @retval >0 lhs has a higher value than rhs.
int compareStringsIgnoreCase(utf8_cptr_t lhs, utf8_cptr_t rhs)
{
#ifdef _MSC_VER
    return _stricmp(lhs, rhs);
#else
    return strcasecmp(lhs, rhs);
#endif
}

//! @brief Compares two bounded strings without regard for case.
//! @param[in] lhs The first string to compare.
//! @param[in] rhs The second string to compare.
//! @param[in] length The maximum number of characters to compare.
//! @retval <0 lhs has a lower value than rhs.
//! @retval 0 The value of lhs is equal to that of rhs.
//! @retval >0 lhs has a higher value than rhs.
int compareBoundedStringsIgnoreCase(utf8_cptr_t lhs, utf8_cptr_t rhs, size_t length)
{
#ifdef _MSC_VER
    return _strnicmp(lhs, rhs, length);
#else
    return strncasecmp(lhs, rhs, length);
#endif
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


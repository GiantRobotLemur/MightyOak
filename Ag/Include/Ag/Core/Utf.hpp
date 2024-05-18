//! @file Ag/Core/Utf.hpp
//! @brief The declaration of various objects and functions for converting
//! between different encodings of Unicode characters.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_UTF_HPP__
#define __AG_CORE_UTF_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>

#include "Configuration.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief The maximum valid value of any Unicode code point.
//! @details Defined as a macro so that it can be used as an integer or character
//! type value.
#define CodePointMax 0x10FFFF

namespace Ag {

namespace Utf {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Expresses an encoding scheme for Unicode characters.
enum Encoding
{
    //! @brief The 8-bit encoding where a single Unicode code point can be
    //! encoded as 1 to 4 bytes.
    Utf8,

    //! @brief The 16-bit encoding where a single Unicode code point can be
    //! encoded as either 1 or 2 16-bit words.
    Utf16,

    //! @brief The 32-bit encoding where each 32-bit word represents a single
    //! Unicode code point.
    Utf32,

    //! @brief A platform/compiler specific encoding which is wider than 8-bits
    //! and is generally UTF-16 on Windows and UTF-32 on Linux.
    Wide,

    //! @brief A value only used for bounds checking.
    Encoding_Max,
};


////////////////////////////////////////////////////////////////////////////////
// Data Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief The maximum number of bytes required to encode a Unicode code point
//! using the UTF-8 encoding scheme.
const size_t MaxUTF8EncodingLength = 4;

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which converts a stream of UTF-8 encoded bytes into
//! Unicode code points.
class FromUtf8Converter
{
public:
    // Constructors/Destructors
    FromUtf8Converter();

    // Operations
    void reset();
    bool tryConvert(uint8_t nextByte, char32_t &result, bool &hasError);
private:
    // Internal Fields
    uint32_t _codePoint;
    uint32_t _bytesRead;
    uint32_t _byteCount;
};

//! @brief An object which converts Unicode code points into sequences of
//! UTF-8 encoded bytes.
class ToUtf8Converter
{
public:
    // Constructors/Destructors
    ToUtf8Converter();

    // Accessors
    uint32_t setCodePoint(char32_t codePoint);

    // Operations
    bool tryGetNextByte(uint8_t &next);
private:
    // Internal Fields
    uint32_t _codePoint;
    uint32_t _encodedSize;
    uint32_t _bytesRead;
    uint32_t _leadingBitCount;
    uint8_t _leadingByteMask;
};

//! @brief An object which converts a sequence of 16-bit UTF-16 characters into
//! Unicode code points.
class FromUtf16Converter
{
public:
    // Constructors/Destructors
    FromUtf16Converter();

    // Operations
    void reset();
    bool tryConvert(char16_t nextWord, char32_t &result, bool &hasError);
private:
    // Internal Fields
    uint32_t _highSurrogate;
    bool _needsLowSurrogate;
};

//! @brief An object which converts Unicode code points into sequences of
//! UTF-16 encoded bytes.
class ToUtf16Converter
{
public:
    // Constructors/Destructors
    ToUtf16Converter();

    // Accessors
    uint32_t setCodePoint(char32_t codePoint);

    // Operations
    bool tryGetNextCharacter(char16_t &next);
private:
    // Internal Fields
    char32_t _codePoint;
    uint32_t _wordsRead;
    bool _isSurrogatePair;
};

//! @brief An object which converts Unicode code points to wide characters.
class ToWideConverter
{
public:
    ToWideConverter();

    // Accessors
    uint32_t setCodePoint(char32_t codePoint);

    // Operations
    bool tryGetNextCharacter(wchar_t &next);
private:
#ifdef WCHAR_IS_32BIT
    char32_t _codePoint;
    bool _hasCodePoint;
#else
    ToUtf16Converter _innerConverter;
#endif
};

//! @brief An object which converts a sequence of wide characters into
//! Unicode code points.
class FromWideConverter
{
public:
    // Constructors/Destructors
    FromWideConverter() = default;

    // Operations
    void reset();
    bool tryConvert(wchar_t nextWord, char32_t &result, bool &hasError);
private:
    // Internal Fields
#ifdef WCHAR_IS_32BIT
    char32_t _codePoint;
    bool _hasCodePoint;
#else
    FromUtf16Converter _innerConverter;
#endif
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool tryGetUTF8ByteCountFromCodePoint(char32_t codePoint, uint32_t &byteCount);
bool tryGetUTF8ByteCountFromLeadingByte(uint8_t leadByte, uint32_t &byteCount);
bool tryGetUTF16WordCountFromCodePoint(char32_t codePoint, uint32_t &wordCount);
bool tryGetUTF16WordCountFromLeadingWord(char16_t leadingWord, uint32_t &wordCount);
size_t calculateConvertedLength(utf8_cptr_t utf8Text, Encoding targetEncoding);
size_t calculateConvertedLength(utf16_cptr_t utf16Text, Encoding targetEncoding);
size_t calculateConvertedLength(utf32_cptr_t utf32Text, Encoding targetEncoding);
size_t calculateConvertedLength(wchar_cptr_t wideText, Encoding targetEncoding);
size_t calculateConvertedLength(const std::string_view &utf8Text, Encoding targetEncoding);
size_t calculateConvertedLength(const std::u16string_view&utf16Text, Encoding targetEncoding);
size_t calculateConvertedLength(const std::u32string_view&utf32Text, Encoding targetEncoding);
size_t calculateConvertedLength(const std::wstring_view&utf32Text, Encoding targetEncoding);
size_t calculateLength(utf8_cptr_t utf8Text);
size_t calculateLength(utf16_cptr_t utf16Text);
size_t calculateLength(utf32_cptr_t utf32Text);
size_t calculateLength(wchar_cptr_t wideText);
void appendToUtf8(std::string &destination, utf32_cptr_t codePoints,
                  size_t codePointCount, size_t hintEncodedSize = 0);
void appendToUtf16(std::u16string &destination, utf8_cptr_t utf8Bytes,
                   size_t byteCount, size_t hintSize = 0);
void appendToUtf32(std::u32string &destination, utf8_cptr_t utf8Bytes,
                   size_t byteCount, size_t hintSize = 0);
void appendToWide(std::wstring &destination, utf8_cptr_t utf8Bytes,
                  size_t byteCount, size_t hintSize = 0);
void appendToWide(std::vector<wchar_t>& destination, utf8_cptr_t utf8Bytes,
                  size_t byteCount, size_t hintSize = 0);
void appendNative(std::wstring &destination, const char *nativeBytes);
void appendNative(std::wstring &destination, const char *nativeBytes,
                  size_t byteCount);
void appendWide(std::string &destination, wchar_cptr_t wideChars,
                size_t charCount);
bool appendCodePoint(std::string &destination, char32_t codePoint);
bool isValidCodePoint(char32_t codePoint);
bool isWhiteSpace(char32_t codePoint);
bool isNullOrEmpty(const utf8_cptr_t utf8Array);
const std::string_view &getEmpty();
utf8_cptr_t ensureNotNull(utf8_cptr_t utf8Text);

}} // namespace Ag::Utf

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

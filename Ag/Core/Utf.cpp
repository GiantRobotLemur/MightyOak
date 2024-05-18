//! @file Core/Utf.cpp
//! @brief The definition of various objects and functions used to convert
//! between the different encodings of Unicode characters.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <cwchar>
#include <vector>

#include "CoreInternal.hpp"
#include "Ag/Core/Utf.hpp"
#include "Win32API.hpp"

namespace Ag {

//! @brief Defines a number of functions and object which operate on character
//! data in various Unicode-compatible encodings.
namespace Utf {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to decode the first byte in a UTF-8 encoded sequence.
//! @param[in] leadingByte The byte to decode.
//! @param[out] bits The most significant bits of character data extracted
//! from the leading byte and shifted to start at bit 0.
//! @param[out] encodingSize The total count of bytes in the sequence required
//! to encode a whole Unicode code point.
//! @retval true The leadingByte was successfully decoded, bits and
//! encodingSize are returned with valid values.
//! @retval false The leadingByte is invalid, bits and encodingSize are zeroed.
bool tryDecodeLeadingUTF8Byte(uint8_t leadingByte, uint32_t &bits,
                              uint32_t &encodingSize)
{
    bool isOK = true;

    if (leadingByte < 0x80)
    {
        // Its a 1-byte encoded as a single wchar_t.
        bits = static_cast<wchar_t>(leadingByte);
        encodingSize = 1;
    }
    else if (leadingByte < 0xC0)
    {
        // Its an unexpected continuation byte.
        bits = 0;
        encodingSize = 0;
        isOK = false;
    }
    else if (leadingByte < 0xE0)
    {
        // Its a 2-byte encoding.
        encodingSize = 2;
        bits = static_cast<uint32_t>(leadingByte & 0x1F);
    }
    else if (leadingByte < 0xF0)
    {
        // Its a 3-byte encoding.
        encodingSize = 3;
        bits = static_cast<uint32_t>(leadingByte & 0x0F);
    }
    else if (leadingByte < 0xF8)
    {
        // Its a 4-byte encoding as a 2 16-bit wchar_t values.
        encodingSize = 4;
        bits = static_cast<uint32_t>(leadingByte & 0x07);
    }
    else
    {
        // The value is out of range as a leading byte.
        bits = 0;
        encodingSize = 0;
        isOK = false;
    }

    return isOK;
}

//! @brief Attempts to extract bits of character data from a UTF-8
//! continuation byte.
//! @param[in] continuationByte A byte in a UTF-8 encoded sequence.
//! @param[out] bits Receives the next 6 bits of code point data in the least
//! significant bits.
//! @retval true The continuationByte was valid, the data was returned in bits.
//! @retval false The continuationByte was invalid, bits is zeroed.
bool tryDecodeContinuationUTF8Byte(uint8_t continuationByte, uint32_t &bits)
{
    bool isOK = false;

    if (continuationByte < 0x80)
    {
        // Its an unexpected leading byte.
        bits = 0;
    }
    else if (continuationByte < 0xC0)
    {
        // Its a continuation byte, extract the bits of character data.
        bits = (continuationByte & 0x3F);
        isOK = true;
    }
    else
    {
        // The value is not that of a continuation byte.
        bits = 0;
    }

    return isOK;
}

//! @brief Attempts to decode a Unicode code point in order to encode it as
//! a sequence of UTF-8 bytes.
//! @param[in] codePoint The code point to decode.
//! @param[out] leadingBitCount Receives the count of code point bits to encode
//! in the leading UTF-8 byte of the corresponding sequence.
//! @param[out] leadingByteMask Receives the set of bits to set in the leading
//! UTF-8 byte of the corresponding sequence.
//! @param[out] encodingSize Receives the count if UTF-8 bytes required to
//! encode the code point.
//! @retval true The code point was value and the UTF-8 encoding parameters
//! returned.
//! @retval false The code point was invalid and the encoding parameters were
//! zeroed.
bool tryEncodeCodePointUTF8(char32_t codePoint, uint32_t &leadingBitCount,
                            uint8_t &leadingByteMask, uint32_t &encodingSize)
{
    bool isOK = true;
    uint32_t codePointData = static_cast<uint32_t>(codePoint);

    if (codePointData < 0x80)
    {
        // Its a 1-byte encoding.
        leadingBitCount = 7;
        leadingByteMask = 0x0;
        encodingSize = 1;
    }
    else if (codePointData < 0x800)
    {
        // Its a 2-byte encoding.
        leadingBitCount = 5;
        leadingByteMask = 0xC0;
        encodingSize = 2;
    }
    else if (codePointData < 0x10000)
    {
        // It's a 3-byte encoding.
        leadingBitCount = 4;
        leadingByteMask = 0xE0;
        encodingSize = 3;
    }
    else if (codePointData <= 0x10FFFF)
    {
        // It's a 4-byte encoding.
        leadingBitCount = 3;
        leadingByteMask = 0xF0;
        encodingSize = 4;
    }
    else
    {
        // It's an invalid code point.
        leadingBitCount = 0;
        leadingByteMask = 0;
        encodingSize = 0;
        isOK = false;
    }

    return isOK;
}

//! @brief Determines whether a Unicode code point must be encoded as a
//! surrogate pair when encoded as UTF-16.
//! @param[in] codePoint The Unicode code point to examine.
//! @param[out] isUtf16SurrogatePair True if 2 x 16-bit words are required to
//! encode the code point in UTF-16, false if a single word is sufficient.
//! @retval true The code point was valid and isUtf16SurrogatePair set.
//! @retval false The code point was invalid.
bool isCodePointSurrogatePair(char32_t codePoint, bool &isUtf16SurrogatePair)
{
    uint32_t codePointData = static_cast<uint32_t>(codePoint);
    bool isValid = true;

    if (codePointData < 0xD800)
    {
        isUtf16SurrogatePair = false;
    }
    else if (codePointData < 0xE000)
    {
        // This range is permanently reserved for the UTF-16 encoding.
        isValid = false;
        isUtf16SurrogatePair = false;
    }
    else if (codePointData < 0x10000)
    {
        isUtf16SurrogatePair = false;
    }
    else if (codePointData < 0x10FFFF)
    {
        isUtf16SurrogatePair = true;
    }
    else
    {
        // The code point is beyond the range of all Unicode code points.
        isValid = false;
        isUtf16SurrogatePair = false;
    }

    return isValid;
}

//! @brief Extracts the UTF-16 high surrogate from a Unicode code point.
//! @param[in] codePoint The code point to extract data from.
//! @return The top 10 bits of the code point encoded as a UTF-16 word.
char16_t extractHighSurrogate(char32_t codePoint)
{
    uint32_t codePointData = static_cast<uint32_t>(codePoint) - 0x10000;

    return static_cast<char16_t>(((codePointData >> 10) & 0x3FF) | 0xD800);
}

//! @brief Extracts the UTF-16 low surrogate from a Unicode code point.
//! @param[in] codePoint The code point to extract data from.
//! @return The bottom 10 bits of the code point encoded as a UTF-16 word.
char16_t extractLowSurrogate(char32_t codePoint)
{
    uint32_t codePointData = static_cast<uint32_t>(codePoint) - 0x10000;

    return static_cast<char16_t>((codePointData & 0x3FF) | 0xDC00);
}

//! @brief Calculates the length of a null-terminated UTF-16 word array when
//! encoded as a set of UTF-8 encoded bytes.
//! @param[in] utf16Text The array to calculate the encoded length of.
//! @return The count of UTF-8 bytes required to encode the string not including
//! any terminating null character.
size_t utf8Length(utf16_cptr_t utf16Text)
{
    size_t length = 0;

    if (utf16Text != nullptr)
    {
        FromUtf16Converter inputConverter;
        bool hasError = false;
        char32_t codePoint = U'\0';

        for (size_t index = 0; utf16Text[index] != u'\0'; ++index)
        {
            if (inputConverter.tryConvert(utf16Text[index], codePoint, hasError))
            {
                uint32_t byteCount = 0;

                if (tryGetUTF8ByteCountFromCodePoint(codePoint, byteCount))
                {
                    length += byteCount;
                }
            }
            else if (hasError)
            {
                // Ignore encoding errors.
                inputConverter.reset();
            }
        }
    }

    return length;
}

//! @brief Calculates the length of a UTF-16 string when encoded as a set of
//! UTF-8 encoded bytes.
//! @param[in] utf16Text The text to calculate the encoded length of.
//! @return The count of UTF-8 bytes required to encode the string not including
//! any terminating null character.
size_t utf8Length(const std::u16string_view &utf16Text)
{
    FromUtf16Converter inputConverter;
    bool hasError = false;
    char32_t codePoint = U'\0';
    size_t length = 0;

    for (char16_t next : utf16Text)
    {
        if (inputConverter.tryConvert(next, codePoint, hasError))
        {
            uint32_t byteCount = 0;

            if (tryGetUTF8ByteCountFromCodePoint(codePoint, byteCount))
            {
                length += byteCount;
            }
        }
        else if (hasError)
        {
            // Ignore encoding errors.
            inputConverter.reset();
        }
    }

    return length;
}

//! @brief Calculates the length of a null-terminated UTF-32 code point array when
//! encoded as a set of UTF-8 encoded bytes.
//! @param[in] utf32Text The array to calculate the encoded length of.
//! @return The count of UTF-8 bytes required to encode the string not including
//! any terminating null character.
size_t utf8Length(utf32_cptr_t utf32Text)
{
    size_t length = 0;

    if (utf32Text != nullptr)
    {
        uint32_t byteCount = 0;

        for (size_t index = 0; utf32Text[index] != u'\0'; ++index)
        {
            if (tryGetUTF8ByteCountFromCodePoint(utf32Text[index],
                                                 byteCount))
            {
                length += byteCount;
            }
        }
    }

    return length;
}

//! @brief Calculates the length of a UTF-32 string when encoded as a set of
//! UTF-8 encoded bytes.
//! @param[in] utf32Text The text to calculate the encoded length of.
//! @return The count of UTF-8 bytes required to encode the string not including
//! any terminating null character.
size_t utf8Length(const std::u32string_view &utf32Text)
{
    size_t length = 0;

    for (char32_t codePoint : utf32Text)
    {
        uint32_t byteCount = 0;

        if (tryGetUTF8ByteCountFromCodePoint(codePoint, byteCount))
        {
            length += byteCount;
        }
    }

    return length;
}

//! @brief Calculates the length of a null-terminated UTF-8 encoded character
//! string when encoded using UTF-16 words.
//! @param[in] utf8Text The null-terminated array of bytes to scan.
//! @return The count of UTF-16 words required to encode the string.
size_t utf16Length(utf8_cptr_t utf8Text)
{
    FromUtf8Converter inputConverter;
    size_t length = 0;
    bool hasError = false;
    char32_t codePoint = U'\0';

    for (utf8_cptr_t current = utf8Text; *current != '\0'; ++current)
    {
        if (inputConverter.tryConvert(*current, codePoint, hasError))
        {
            uint32_t wordCount = 0;

            if (tryGetUTF16WordCountFromCodePoint(codePoint, wordCount))
            {
                length += wordCount;
            }
        }
        else if (hasError)
        {
            // Ignore encoding errors.
            inputConverter.reset();
        }
    }

    return length;
}

//! @brief Calculates the length of a STL UTF-8 encoded character string when
//! encoded using UTF-16 words.
//! @param[in] utf8Text The string of characters to scan.
//! @return The count of UTF-16 words required to encode the string.
size_t utf16Length(const std::string_view &utf8Text)
{
    FromUtf8Converter inputConverter;
    size_t length = 0;
    bool hasError = false;
    char32_t codePoint = U'\0';

    for (char next : utf8Text)
    {
        if (inputConverter.tryConvert(next, codePoint, hasError))
        {
            uint32_t wordCount = 0;

            if (tryGetUTF16WordCountFromCodePoint(codePoint, wordCount))
            {
                length += wordCount;
            }
        }
        else if (hasError)
        {
            // Ignore encoding errors.
            inputConverter.reset();
        }
    }

    return length;
}

//! @brief Calculates the length of a STL UTF-32 encoded character string when
//! encoded using UTF-16 words
//! @param[in] utf32Text The string of characters to scan.
//! @return The count of UTF-16 words required to encode the string.
size_t utf16Length(const std::u32string_view &utf32Text)
{
    size_t length = 0;

    for (char32_t codePoint : utf32Text)
    {
        uint32_t wordCount = 0;

        if (tryGetUTF16WordCountFromCodePoint(codePoint, wordCount))
        {
            length += wordCount;
        }
    }

    return length;
}

//! @brief Calculates the length of a null-terminated UTF-8 encoded character
//! array when encoded using UTF-32 code points.
//! @param[in] utf8Text The null-terminated array of UTF-8 encoded bytes to scan.
//! @return The count of code points required to encode the string.
size_t utf32Length(utf8_cptr_t utf8Text)
{
    FromUtf8Converter inputConverter;
    size_t length = 0;
    bool hasError = false;
    char32_t codePoint = U'\0';

    for (utf8_cptr_t current = utf8Text; *current != '\0'; ++current)
    {
        if (inputConverter.tryConvert(*current, codePoint, hasError))
        {
            ++length;
        }
        else if (hasError)
        {
            // Ignore encoding errors.
            inputConverter.reset();
        }
    }

    return length;
}

//! @brief Calculates the length of a STL UTF-8 encoded character string when
//! encoded using UTF-32 code points.
//! @param[in] utf8Text The string of characters to scan.
//! @return The count of code points required to encode the string.
size_t utf32Length(const std::string_view &utf8Text)
{
    FromUtf8Converter inputConverter;
    size_t length = 0;
    bool hasError = false;
    char32_t codePoint = U'\0';

    for (char next : utf8Text)
    {
        if (inputConverter.tryConvert(next, codePoint, hasError))
        {
            ++length;
        }
        else if (hasError)
        {
            // Ignore encoding errors.
            inputConverter.reset();
        }
    }

    return length;
}

//! @brief Calculates the length of a STL UTF-16 encoded character string when
//! encoded using UTF-32 code points.
//! @param[in] utf16Text The string of characters to scan.
//! @return The count of code points required to encode the string.
size_t utf32Length(const std::u16string_view &utf16Text)
{
    FromUtf16Converter inputConverter;
    size_t length = 0;
    bool hasError = false;
    char32_t codePoint = U'\0';

    for (char16_t next : utf16Text)
    {
        if (inputConverter.tryConvert(next, codePoint, hasError))
        {
            ++length;
        }
        else if (hasError)
        {
            // Ignore encoding errors.
            inputConverter.reset();
        }
    }

    return length;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which converts a stream of bytes into Unicode
//! code points.
FromUtf8Converter::FromUtf8Converter() :
    _codePoint(0),
    _bytesRead(0),
    _byteCount(0)
{
}

//! @brief Resets the state of the converter ready to start a new stream.
void FromUtf8Converter::reset()
{
    _codePoint = 0;
    _bytesRead = 0;
    _byteCount = 0;
}

//! @brief Attempts to extract a Unicode code point by processing a byte
//! within a sequence.
//! @param[in] nextByte The UTF-8 encoded byte to process.
//! @param[out] result Receives the Unicode code point if enough data has
//! been processed in order to extract a whole one.
//! @param[out] hasError Receives true if nextByte was invalid, false if valid.
//! @retval true The byte was the last in the sequence which encoded a Unicode
//! code point, which was returned in result.
//! @retval false Either nextByte was invalid, in which case hasError will
//! return true, or more bytes are required to extract an entire code point.
bool FromUtf8Converter::tryConvert(uint8_t nextByte, char32_t &result,
                                   bool &hasError)
{
    bool isComplete = false;
    hasError = false;
    result = 0;

    if (_byteCount == 0)
    {
        // It's a leading byte.
        uint32_t codePoint, byteCount;

        if (tryDecodeLeadingUTF8Byte(nextByte, codePoint, byteCount))
        {
            if (byteCount == 1)
            {
                // We have our decoded code point.
                result = static_cast<char32_t>(codePoint);
                isComplete = true;
            }
            else
            {
                // We require more bytes to complete the code point, set the
                // state of the object to process further bytes.
                _codePoint = codePoint;
                _bytesRead = 1;
                _byteCount = byteCount;
            }
        }
        else
        {
            // The leading byte was invalid.
            hasError = true;
            isComplete = false;
        }
    }
    else
    {
        // It's a continuation byte.
        uint32_t codePointData;

        if (tryDecodeContinuationUTF8Byte(nextByte, codePointData))
        {
            // The continuation byte was valid.
            _codePoint = (_codePoint << 6) | codePointData;
            ++_bytesRead;

            if (_bytesRead >= _byteCount)
            {
                // We have completed the code point.
                result = static_cast<char32_t>(_codePoint);
                isComplete = true;

                // Reset in order to start a new sequence.
                _codePoint = 0;
                _bytesRead = 0;
                _byteCount = 0;
            }
        }
        else
        {
            // The continuation bytes was invalid.
            hasError = true;
        }
    }

    return isComplete;
}

//! @brief Constructs an object which produces a sequence of UTF-8 encoded
//! bytes from a Unicode code point.
ToUtf8Converter::ToUtf8Converter() :
    _codePoint(0),
    _encodedSize(0),
    _bytesRead(0),
    _leadingBitCount(0),
    _leadingByteMask(0)
{
}

//! @brief Sets the next Unicode code point to convert.
//! @param[in] codePoint The code point to convert.
//! @return The count of bytes required to encode the code point as UTF-8. A
//! value of 0 indicates the code point was invalid. Valid return values are
//! between 1 and 4.
uint32_t ToUtf8Converter::setCodePoint(char32_t codePoint)
{
    _bytesRead = 0;

    if (tryEncodeCodePointUTF8(codePoint, _leadingBitCount,
                               _leadingByteMask, _encodedSize))
    {
        // The code point is valid.
        _codePoint = static_cast<uint32_t>(codePoint);
    }
    else
    {
        // The code point is invalid.
        _codePoint = 0;
        _encodedSize = 0;
    }

    return _encodedSize;
}

//! @brief Attempts to get the next byte encoding the code point last passed
//! to SetCodePoint().
//! @param[out] next Receives the next byte in the UTF-8 sequence.
//! @retval true A UTF-8 bytes was returned.
//! @retval false No byte was returned, a new call to SetCodePoint() is
//! required before any more bytes can be produced.
bool ToUtf8Converter::tryGetNextByte(uint8_t &next)
{
    bool hasByte = true;

    if (_encodedSize > 0)
    {
        if (_bytesRead == 0)
        {
            // Return the leading byte.

            // Extract the most significant bits of the code point.
            uint8_t lead = static_cast<uint8_t>(_codePoint >> ((_encodedSize - 1) * 6));
            uint8_t leadMask = static_cast<uint8_t>((1U << _leadingBitCount) - 1);

            // Encode the leading byte.
            next = (lead & leadMask) | _leadingByteMask;
            _bytesRead = 1;
        }
        else if (_bytesRead < _encodedSize)
        {
            // Return one of the continuation bytes.

            // Extract the next 6 bits from the code point.
            uint8_t bits = static_cast<uint8_t>(_codePoint >> ((_encodedSize - _bytesRead - 1) * 6));

            // Encode the bits as a continuation byte.
            next = (bits & 0x3F) | 0x80;

            // Move on to the next byte.
            ++_bytesRead;
        }
        else
        {
            // All of the bytes have been read.
            hasByte = false;
            next = 0;
        }
    }
    else
    {
        hasByte = false;
        next = 0;
    }

    return hasByte;
}

//! @brief Constructs an object which converts UTF-16 characters to Unicode
//! code points.
FromUtf16Converter::FromUtf16Converter() :
    _highSurrogate(0),
    _needsLowSurrogate(false)
{
}

//! @brief Resets the state of the converter to start decoding a new sequence
//! of UTF-16 characters.
void FromUtf16Converter::reset()
{
    _highSurrogate = 0;
    _needsLowSurrogate = false;
}

//! @brief Attempts to obtain the next Unicode code point from a sequence of
//! UTF-16 characters.
//! @param[in] nextWord The next 16-bit UTF-16 character to decode.
//! @param[out] result Receives the decoded code point.
//! @param[out] hasError Receives true if nextWord was invalid.
//! @retval true The nextWord was valid and result was updated with the next
//! code point decoded from the sequence.
//! @retval false Not enough words have been processed to extract a code point,
//! or nextWord was invalid, in which case hasError will return true.
bool FromUtf16Converter::tryConvert(char16_t nextWord, char32_t &result,
                                    bool &hasError)
{
    uint32_t charData = static_cast<uint32_t>(nextWord);
    bool hasCodePoint = false;
    hasError = false;
    result = 0;

    if (_needsLowSurrogate)
    {
        if ((charData >= 0xDC00) && (charData < 0xE000))
        {
            // It's a valid low surrogate.
            uint32_t codePoint = ((charData & 0x3FF) | _highSurrogate) + 0x10000;

            result = static_cast<char32_t>(codePoint);
            hasCodePoint = true;

            // Reset ready for the next character.
            _needsLowSurrogate = false;
        }
        else
        {
            // It's a single word encoding or a high surrogate, not valid here.
            _needsLowSurrogate = false;
            _highSurrogate = 0;
            result = 0;
            hasError = true;
        }
    }
    else
    {
        // We're starting a new code point.
        uint32_t wordCount;

        if (tryGetUTF16WordCountFromLeadingWord(nextWord, wordCount))
        {
            if (wordCount == 1)
            {
                // The code point is encoded using a single 16-bit character.
                result = static_cast<char32_t>(nextWord);
                hasCodePoint = true;
            }
            else
            {
                // The code point is encoded as a surrogate pair.
                _highSurrogate = (charData & 0x3FF) << 10;
                _needsLowSurrogate = true;
            }
        }
        else
        {
            // The leading 16-bit character is invalid.
            hasError = true;
        }
    }

    return hasCodePoint;
}


//! @brief Constructs an object which converts Unicode code points into UTF-16
//! encoded words.
ToUtf16Converter::ToUtf16Converter() :
    _codePoint(0),
    _wordsRead(1),
    _isSurrogatePair(false)
{
}


//! @brief Sets the next Unicode code point to convert.
//! @param[in] codePoint The code point to convert.
//! @return The count of 16-bit words required to encode the code point as
//! UTF-16. A value of 0 indicates the code point was invalid. Valid return
//! values are 1 or 2.
uint32_t ToUtf16Converter::setCodePoint(char32_t codePoint)
{
    uint32_t wordCount;

    if (isCodePointSurrogatePair(codePoint, _isSurrogatePair))
    {
        // The code point is valid, prepare to extract the UTF-16 words.
        _codePoint = codePoint;
        _wordsRead = 0;
        wordCount = _isSurrogatePair ? 2 : 1;
    }
    else
    {
        // The code point is invalid.
        _codePoint = 0;
        _wordsRead = 1;
        _isSurrogatePair = false;
        wordCount = 0;
    }

    return wordCount;
}

//! @brief Attempts to get the next 16-bit word encoding the code point last
//! passed to SetCodePoint().
//! @param[out] next Receives the next character in the UTF-16 sequence.
//! @retval true A UTF-16 character was returned.
//! @retval false No data was returned, a new call to SetCodePoint() is
//! required before any more encoded data can be produced.
bool ToUtf16Converter::tryGetNextCharacter(char16_t &next)
{
    bool hasNext = true;

    if (_isSurrogatePair)
    {
        if (_wordsRead == 0)
        {
            next = extractHighSurrogate(_codePoint);
            _wordsRead = 1;
        }
        else if (_wordsRead == 1)
        {
            next = extractLowSurrogate(_codePoint);
            _wordsRead = 2;
        }
        else
        {
            // We have already read all of the words.
            hasNext = false;
        }
    }
    else if (_wordsRead == 0)
    {
        next = static_cast<char16_t>(_codePoint);
        _wordsRead = 1;
    }
    else
    {
        // We have already read all of the words.
        hasNext = false;
    }

    return hasNext;
}

//! @brief Constructs an object which converts a stream of wide characters into
//! Unicode code points.
ToWideConverter::ToWideConverter()
#ifdef WCHAR_IS_32BIT
    : _codePoint(0), _hasCodePoint(false)
#endif
{
}

//! @brief Sets the code point to convert.
//! @param[in] codePoint The next Unicode code point to convert.
//! @return The count of wide characters required to encode the code point,
//! 0 if it was invalid.
uint32_t ToWideConverter::setCodePoint(char32_t codePoint)
{
    uint32_t length = 0;

#ifdef WCHAR_IS_32BIT
    if (codePoint <= CodePointMax)
    {
        length = 1;
        _codePoint = codePoint;
        _hasCodePoint = true;
    }
#else
    length = _innerConverter.setCodePoint(codePoint);
#endif

    return length;
}

//! @brief Attempts to get the next wide character required to convert the
//! last Unicode code point passed to setCodePoint().
//! @param[out] next Receives the next wide character in the encoding.
//! @retval true Another wide character was returned in next.
//! @retval false No more wide characters are required to encode the last
//! code point set, or no valid code point was previously set.
bool ToWideConverter::tryGetNextCharacter(wchar_t &next)
{
    bool hasCharacter = false;

#ifdef WCHAR_IS_32BIT
    if (_hasCodePoint)
    {
        hasCharacter = true;
        next = static_cast<wchar_t>(_codePoint);
        _hasCodePoint = false;
    }
#else
    char16_t nextUtf16;

    if (_innerConverter.tryGetNextCharacter(nextUtf16))
    {
        hasCharacter = true;
        next = static_cast<wchar_t>(nextUtf16);
    }
#endif

    return hasCharacter;
}


//! @brief Resets the converter to the base state.
void FromWideConverter::reset()
{
#ifndef WCHAR_IS_32BIT
    _innerConverter.reset();
#endif
}

//! @brief Attempts to obtain the next Unicode code point from a sequence of
//! wide characters.
//! @param[in] nextWord The next wide character to decode.
//! @param[out] result Receives the decoded code point.
//! @param[out] hasError Receives true if nextWord was invalid.
//! @retval true The nextWord was valid and result was updated with the next
//! code point decoded from the sequence.
//! @retval false Not enough words have been processed to extract a code point,
//! or nextWord was invalid, in which case hasError will return true.
bool FromWideConverter::tryConvert(wchar_t nextWord, char32_t &result, bool &hasError)
{
#ifdef WCHAR_IS_32BIT
    if (nextWord <= CodePointMax)
    {
        result = static_cast<char32_t>(nextWord);

        hasError = false;
        return true;
    }
    else
    {
        result = 0;
        hasError = true;
        return false;
    }
#else
    return _innerConverter.tryConvert(static_cast<char16_t>(nextWord),
                                      result, hasError);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to calculate the number of UTF-8 encoded bytes required to
//! encode a Unicode code point.
//! @param[in] codePoint The code point to encode.
//! @param[out] byteCount Receives the count of bytes required to encode codePoint.
//! @retval true The code point was valid, byteCount is returned.
//! @retval false The code point was invalid, byteCount was zeroed.
bool tryGetUTF8ByteCountFromCodePoint(char32_t codePoint, uint32_t &byteCount)
{
    uint32_t leadingBitCount;
    uint8_t leadingByteMask;

    return tryEncodeCodePointUTF8(codePoint, leadingBitCount,
                                  leadingByteMask, byteCount);
}

//! @brief Attempts to calculate the number of UTF-8 bytes in a sequence
//! encoding a Unicode code point.
//! @param[in] leadByte The first byte in the sequence.
//! @param[out] byteCount Receives the count of bytes in the sequence,
//! including the leading byte.
//! @retval true The leadByte was valid, the total count of bytes in the
//! sequence (between 1 and 4) was written back to byteCount.
//! @retval false The leadByte was invalid.
bool tryGetUTF8ByteCountFromLeadingByte(uint8_t leadByte, uint32_t &byteCount)
{
    uint32_t leadingBits;

    return tryDecodeLeadingUTF8Byte(leadByte, leadingBits, byteCount);
}

//! @brief Attempts to calculate how many 16-bit characters are required to
//! encode a Unicode code point as UTF-16.
//! @param[in] codePoint The code point to encode.
//! @param[out] wordCount The receives count of UTF-16 characters required to
//! encode codePoint.
//! @retval true The code point was valid, wordCount was returned.
//! @retval false The code point was invalid, wordCount was zeroed.
bool tryGetUTF16WordCountFromCodePoint(char32_t codePoint, uint32_t &wordCount)
{
    bool isSurrogatePair;
    bool isValid;

    if (isCodePointSurrogatePair(codePoint, isSurrogatePair))
    {
        isValid = true;
        wordCount = isSurrogatePair ? 2 : 1;
    }
    else
    {
        isValid = false;
        wordCount = 0;
    }

    return isValid;
}

//! @brief Attempts to calculate the count of 16-bit UTF-16 characters encode
//! the next whole Unicode code point.
//! @param[in] leadingWord The first UTF-16 character in the sequence.
//! @param[out] wordCount Receives the count of 16-bit characters in the
//! sequence.
//! @retval true The leadingWord is valid and wordCount has been updated with
//! either 1 or 2.
//! @retval false The leadingWord is an unexpected low surrogate, wordCount
//! will be zeroed.
bool tryGetUTF16WordCountFromLeadingWord(char16_t leadingWord,
                                         uint32_t &wordCount)
{
    uint32_t charData = static_cast<uint32_t>(leadingWord);
    bool isValid = true;

    if (charData < 0xD800)
    {
        // Its a single word encoding.
        wordCount = 1;
    }
    else if (charData < 0xDC00)
    {
        // Its the high surrogate of a pair.
        wordCount = 2;
    }
    else if (charData < 0xE000)
    {
        // Its a low surrogate, which should not appear first.
        wordCount = 0;
        isValid = false;
    }
    else
    {
        // Its a single word encoding.
        wordCount = 1;
    }

    return isValid;
}

//! @brief Calculates the length of a null-terminated array of UTF-8 bytes when
//! converted to a different encoding.
//! @param[in] utf8Text The array of bytes to convert.
//! @param[in] targetEncoding Indicates the encoding the characters should be
//! converted to in order to calculate the length.
//! @returns The length of the string in the target encoding.
size_t calculateConvertedLength(utf8_cptr_t utf8Text, Encoding targetEncoding)
{
    size_t length = 0;

    if (utf8Text != nullptr)
    {
        switch (targetEncoding)
        {
        case Utf8: length = calculateLength(utf8Text); break;
        case Utf16: length = utf16Length(utf8Text); break;
        case Utf32: length = utf32Length(utf8Text); break;
        case Wide:
#ifdef WCHAR_IS_32BIT
            length = utf32Length(utf8Text);
#else
            length = utf16Length(utf8Text);
#endif
            break;
        case Encoding_Max:
        default:
            break;
        }
    }

    return length;
}

//! @brief Calculates the length of a null-terminated array of UTF-16 words when
//! converted to a different encoding.
//! @param[in] utf16Text The array of 16-bit words to convert.
//! @param[in] targetEncoding Indicates the encoding the characters should be
//! converted to in order to calculate the length.
//! @returns The length of the string in the target encoding.
size_t calculateConvertedLength(utf16_cptr_t utf16Text, Encoding targetEncoding)
{
    size_t length = 0;

    if (utf16Text != nullptr)
    {
        switch (targetEncoding)
        {
        case Utf8: length = utf8Length(utf16Text); break;
        case Utf16: length = calculateLength(utf16Text); break;
        case Utf32: length = utf32Length(utf16Text); break;
        case Wide:
#ifdef WCHAR_IS_32BIT
            length = utf32Length(utf16Text);
#else
            length = calculateLength(utf16Text);
#endif
            break;
        case Encoding_Max:
        default:
            break;
        }
    }

    return length;
}

//! @brief Calculates the length of a null-terminated array of Unicode code points
//! when converted to a different encoding.
//! @param[in] utf32Text The array of code points to convert.
//! @param[in] targetEncoding Indicates the encoding the characters should be
//! converted to in order to calculate the length.
//! @returns The length of the string in the target encoding.
size_t calculateConvertedLength(utf32_cptr_t utf32Text, Encoding targetEncoding)
{
    size_t length = 0;

    if (utf32Text != nullptr)
    {
        switch (targetEncoding)
        {
        case Utf8: length = utf8Length(utf32Text); break;
        case Utf16: length = utf16Length(utf32Text); break;
        case Utf32: length = calculateLength(utf32Text); break;
        case Wide:
#ifdef WCHAR_IS_32BIT
            length = calculateLength(utf32Text);
#else
            length = utf16Length(utf32Text);
#endif
            break;
        case Encoding_Max:
        default:
            break;
        }
    }

    return length;
}

//! @brief Calculates the length of a null-terminated array of wide characters
//! when converted to a different encoding.
//! @param[in] wideText The array of code points to convert.
//! @param[in] targetEncoding Indicates the encoding the characters should be
//! converted to in order to calculate the length.
//! @returns The length of the string in the target encoding.
size_t calculateConvertedLength(wchar_cptr_t wideText, Encoding targetEncoding)
{
    size_t length = 0;

    if (wideText != nullptr)
    {
        switch (targetEncoding)
        {
#ifdef WCHAR_IS_32BIT
        case Utf8: length = utf8Length(reinterpret_cast<utf32_cptr_t>(wideText)); break;
        case Utf16: length = utf16Length(reinterpret_cast<utf32_cptr_t>(wideText)); break;
        case Utf32: length = std::wcslen(wideText); break;
#else
        case Utf8: length = utf8Length(reinterpret_cast<utf16_cptr_t>(wideText)); break;
        case Utf16: length = std::wcslen(wideText); break;
        case Utf32: length = utf32Length(reinterpret_cast<utf16_cptr_t>(wideText)); break;
#endif
        case Wide: length = std::wcslen(wideText); break;
        case Encoding_Max:
        default:
            break;
        }
    }

    return length;
}

//! @brief Calculates the length of a UTF-8 encoded STL string when converted
//! to a different encoding.
//! @param[in] utf8Text The string to convert and measure.
//! @param[in] targetEncoding Indicates the encoding the characters should be
//! converted to in order to calculate the length.
//! @returns The length of the string in the target encoding.
size_t calculateConvertedLength(const std::string_view &utf8Text, Encoding targetEncoding)
{
    size_t length = 0;

    if (utf8Text.empty() == false)
    {
        switch (targetEncoding)
        {
        case Utf8: length = utf8Text.length(); break;
        case Utf16: length = utf16Length(utf8Text); break;
        case Utf32: length = utf32Length(utf8Text); break;
        case Wide:
#ifdef WCHAR_IS_32BIT
            length = utf32Length(utf8Text);
#else
            length = utf16Length(utf8Text);
#endif
            break;
        case Encoding_Max:
        default:
            break;
        }
    }

    return length;
}

//! @brief Calculates the length of a UTF-16 encoded STL string when converted
//! to a different encoding.
//! @param[in] utf16Text The string to convert and measure.
//! @param[in] targetEncoding Indicates the encoding the characters should be
//! converted to in order to calculate the length.
//! @returns The length of the string in the target encoding.
size_t calculateConvertedLength(const std::u16string_view &utf16Text, Encoding targetEncoding)
{
    size_t length = 0;

    if (utf16Text.empty() == false)
    {
        switch (targetEncoding)
        {
        case Utf8: length = utf8Length(utf16Text); break;
        case Utf16: length = utf16Text.length(); break;
        case Utf32: length = utf32Length(utf16Text); break;
        case Wide:
#ifdef WCHAR_IS_32BIT
            length = utf32Length(utf16Text);
#else
            length = utf16Text.length();
#endif
            break;
        case Encoding_Max:
        default:
            break;
        }
    }

    return length;
}

//! @brief Calculates the length of a UTF-32 encoded STL string when converted
//! to a different encoding.
//! @param[in] utf32Text The string to convert and measure.
//! @param[in] targetEncoding Indicates the encoding the characters should be
//! converted to in order to calculate the length.
//! @returns The length of the string in the target encoding.
size_t calculateConvertedLength(const std::u32string_view &utf32Text, Encoding targetEncoding)
{
    size_t length = 0;

    if (utf32Text.empty() == false)
    {
        switch (targetEncoding)
        {
        case Utf8: length = utf8Length(utf32Text); break;
        case Utf16: length = utf16Length(utf32Text); break;
        case Utf32: length = utf32Text.length(); break;
        case Wide:
#ifdef WCHAR_IS_32BIT
            length = utf32Text.length();
#else
            length = utf16Length(utf32Text);
#endif
            break;
        case Encoding_Max:
        default:
            break;
        }
    }

    return length;
}

//! @brief Calculates the length of a wide character STL string when converted
//! to a different encoding.
//! @param[in] wideText The string to convert and measure.
//! @param[in] targetEncoding Indicates the encoding the characters should be
//! converted to in order to calculate the length.
//! @returns The length of the string in the target encoding.
size_t calculateConvertedLength(const std::wstring_view &wideText, Encoding targetEncoding)
{
    size_t length = 0;

    if (wideText.empty() == false)
    {
        switch (targetEncoding)
        {
#ifdef WCHAR_IS_32BIT
        case Utf8: length = utf8Length(reinterpret_cast<utf32_cptr_t>(wideText.data())); break;
        case Utf16: length = utf16Length(reinterpret_cast<utf32_cptr_t>(wideText.data())); break;
        case Utf32: length = std::wcslen(wideText.c_str()); break;
#else
        case Utf8: length = utf8Length(reinterpret_cast<utf16_cptr_t>(wideText.data())); break;
        case Utf16: length = wideText.length(); break;
        case Utf32: length = utf32Length(reinterpret_cast<utf16_cptr_t>(wideText.data())); break;
#endif
        case Wide: length = wideText.length(); break;
        case Encoding_Max:
        default:
            break;
        }
    }

    return length;
}

//! @brief Calculates the length of a null-terminated array of UTF-8 encoded
//! bytes.
//! @param[in] utf8Text The array of characters to scan.
//! @retval 0 utf8Text was nullptr.
//! @retval length The count of bytes before the terminating null.
//! @details Although this function appears functionally identical to strlen(),
//! it will return a length of 0 for an input of nullptr where the behaviour of
//! strlen() is undefined.
size_t calculateLength(utf8_cptr_t utf8Text)
{
    size_t length = 0;

    if (utf8Text != nullptr)
    {
        length = std::strlen(utf8Text);
    }

    return length;
}

//! @brief Gets the count of 16-bit characters in an array before a null terminator.
//! @param[in] utf16Text The array of characters, nullptr will have a length of 0.
//! @retval 0 The input address was nullptr.
//! @retval length The input address was valid, the count of UTF-16 words before
//! a null terminator was encountered.
size_t calculateLength(utf16_cptr_t utf16Text)
{
    size_t characterCount = 0;

    if (utf16Text != nullptr)
    {
#ifdef WCHAR_IS_32BIT
        for (; utf16Text[characterCount] != 0; ++characterCount)
        {
            // Nothing to do.
        }
#else
        characterCount = std::wcslen(reinterpret_cast<wchar_cptr_t>(utf16Text));
#endif
    }

    return characterCount;
}

//! @brief Gets the count of 32-bit characters in an array before a null terminator.
//! @param[in] utf32Text The array of characters, nullptr will have a length of 0.
//! @retval 0 The input address was nullptr.
//! @retval length The input address was valid, the count of UTF-32 code points
//! before a null terminator was encountered.
size_t calculateLength(utf32_cptr_t utf32Text)
{
    size_t characterCount = 0;

    if (utf32Text != nullptr)
    {
#ifdef WCHAR_IS_32BIT
        characterCount = std::wcslen(reinterpret_cast<wchar_cptr_t>(utf32Text));
#else
        for (; utf32Text[characterCount] != 0; ++characterCount)
        {
            // Nothing to do.
        }
#endif
    }

    return characterCount;
}

//! @brief Gets the count of wide characters in an array before a null terminator.
//! @param[in] wideText The array of wide characters, nullptr will have a length of 0.
//! @retval 0 The input address was nullptr.
//! @retval length The input address was valid, the count of wide characters
//! before a null terminator was encountered.
size_t calculateLength(wchar_cptr_t wideText)
{
    size_t characterCount = 0;

    // wcslen() is undefined when passed a nullptr.
    if (wideText != nullptr)
    {
        characterCount = std::wcslen(wideText);
    }

    return characterCount;
}

//! @brief Converts a bounded array UTF-32 code points to UTF-8 encode bytes.
//! @param[out] destination The UTF-8 string to append to.
//! @param[in] codePoints The array of Unicode characters to append.
//! @param[in] codePointCount The count of elements in codePoints.
//! @param[in] hintEncodedSize An optional hint about the count of bytes
//! required to encode the code points, 0 if not set.
void appendToUtf8(std::string &destination, utf32_cptr_t codePoints,
                  size_t codePointCount, size_t hintEncodedSize /*= 0*/)
{
    if (hintEncodedSize > 0)
    {
        // Ensure the string can take the new characters without having
        // to perform multiple re-allocations.
        size_t requiredSize = destination.size() + hintEncodedSize;

        if (requiredSize > destination.capacity())
        {
            destination.reserve(requiredSize);
        }
    }

    ToUtf8Converter converter;

    for (size_t i = 0; i < codePointCount; ++i)
    {
        uint32_t byteCount = converter.setCodePoint(codePoints[i]);

        if (byteCount == 0)
        {
            // The code point was invalid.
            continue;
        }
        else
        {
            // Extract the encoded bytes.
            uint8_t nextByte;

            while (converter.tryGetNextByte(nextByte))
            {
                destination.push_back(nextByte);
            }
        }
    }
}

//! @brief Converts a bounded array of UTF-8 encode bytes to UTF-16 and appends
//! the results to an STL string.
//! @param[out] destination The STL string to receive the converted characters.
//! @param[in] utf8Bytes The UTF-8 encoded bytes to convert.
//! @param[in] byteCount The count of elements in utf8Bytes.
//! @param[in] hintSize The optional hint as to how many UTF-16 characters are
//! likely to be produced from the conversion.
//! @return An STL string containing the converted text.
void appendToUtf16(std::u16string &destination, utf8_cptr_t utf8Bytes,
                   size_t byteCount, size_t hintSize /* = 0 */)
{
    FromUtf8Converter inputConverter;
    ToUtf16Converter outputConverter;
    uint8_cptr_t bytes = reinterpret_cast<uint8_cptr_t>(utf8Bytes);
    bool hasError = false;
    char32_t codePoint = U'\0';

    if (hintSize > 0)
    {
        destination.reserve(hintSize + destination.length());
    }
    else
    {
        destination.reserve(byteCount+ destination.length());
    }

    for (size_t index = 0; index < byteCount; ++index)
    {
        if (inputConverter.tryConvert(bytes[index], codePoint, hasError))
        {
            outputConverter.setCodePoint(codePoint);
            char16_t utf16word = u'\0';

            while (outputConverter.tryGetNextCharacter(utf16word))
            {
                destination.push_back(utf16word);
            }
        }
        else if (hasError)
        {
            inputConverter.reset();
        }
    }
}

//! @brief Converts a bounded array of UTF-8 encode bytes to UTF-32 and appends
//! the results to an STL string.
//! @param[out] destination The STL string to receive the converted characters.
//! @param[in] utf8Bytes The UTF-8 encoded bytes to convert.
//! @param[in] byteCount The count of elements in utf8Bytes.
//! @param[in] hintSize The optional hint as to how many UTF-32 characters are
//! likely to be produced from the conversion.
//! @return An STL string containing the converted text.
void appendToUtf32(std::u32string &destination, utf8_cptr_t utf8Bytes,
                   size_t byteCount, size_t hintSize /* = 0 */)
{
    FromUtf8Converter inputConverter;
    uint8_cptr_t bytes = reinterpret_cast<uint8_cptr_t>(utf8Bytes);
    bool hasError = false;
    char32_t codePoint = U'\0';

    if (hintSize > 0)
    {
        destination.reserve(hintSize + destination.size());
    }
    else
    {
        destination.reserve(byteCount + destination.size());
    }

    for (size_t index = 0; index < byteCount; ++index)
    {
        if (inputConverter.tryConvert(bytes[index], codePoint, hasError))
        {
            destination.push_back(codePoint);
        }
        else if (hasError)
        {
            inputConverter.reset();
        }
    }
}

template<typename TBuffer>
void appendToWideInternal(TBuffer& destination, utf8_cptr_t utf8Bytes,
                          size_t byteCount, size_t hintSize)
{
#ifndef WCHAR_IS_32BIT
    ToUtf16Converter outputConverter;
#endif

    FromUtf8Converter inputConverter;
    uint8_cptr_t bytes = reinterpret_cast<uint8_cptr_t>(utf8Bytes);
    bool hasError = false;
    char32_t codePoint = U'\0';

    size_t reserveSize = (hintSize > 0) ? hintSize :
        calculateConvertedLength(std::string_view(utf8Bytes, byteCount),
            Encoding::Wide);

    if (destination.capacity() < reserveSize)
    {
        destination.reserve(reserveSize);
    }

    for (size_t index = 0; index < byteCount; ++index)
    {
        // Append the converted code points raw.
        if (inputConverter.tryConvert(bytes[index], codePoint, hasError))
        {
#ifdef WCHAR_IS_32BIT
            destination.push_back(codePoint);
#else
            // Convert to code points and then UTF-16.
            outputConverter.setCodePoint(codePoint);
            char16_t utf16word = u'\0';

            while (outputConverter.tryGetNextCharacter(utf16word))
            {
                destination.push_back(utf16word);
            }
#endif
        }
        else if (hasError)
        {
            inputConverter.reset();
        }
    }
}

//! @brief Converts a bounded array of UTF-8 encode bytes to wide characters
//! and appends the results to an STL string.
//! @param[out] destination The STL string to receive the converted characters.
//! @param[in] utf8Bytes The UTF-8 encoded bytes to convert.
//! @param[in] byteCount The count of elements in utf8Bytes.
//! @param[in] hintSize The optional hint as to how many wide characters are
//! likely to be produced from the conversion.
void appendToWide(std::wstring &destination, utf8_cptr_t utf8Bytes,
                  size_t byteCount, size_t hintSize /* = 0 */)
{
    appendToWideInternal(destination, utf8Bytes, byteCount, hintSize);
}

//! @brief Converts a bounded array of UTF-8 encode bytes to wide characters
//! and appends the results to an STL string.
//! @param[out] destination The STL wide character vector to receive the converted
//! characters but no terminating null.
//! @param[in] utf8Bytes The UTF-8 encoded bytes to convert.
//! @param[in] byteCount The count of elements in utf8Bytes.
//! @param[in] hintSize The optional hint as to how many wide characters are
//! likely to be produced from the conversion.
//! @note No terminating null character is appended.
void appendToWide(std::vector<wchar_t>& destination, utf8_cptr_t utf8Bytes,
                  size_t byteCount, size_t hintSize /* = 0 */)
{
    appendToWideInternal(destination, utf8Bytes, byteCount, hintSize);
}

//! @brief Converts a null-terminated array of characters in the native code
//! page to wide characters and appends them to an STL string.
//! @param[out] destination The wide string to append to.
//! @param[in] nativeBytes The null-terminated array of characters to append.
//! A null pointer will be treated as an empty string.
void appendNative(std::wstring &destination, const char *nativeBytes)
{
    if (nativeBytes != nullptr)
    {
        size_t length = std::strlen(nativeBytes);

        appendNative(destination, nativeBytes, length);
    }
}

//! @brief Converts a bounded array of characters in the native code
//! page to wide characters and appends them to an STL string.
//! @param[out] destination The wide string to append to.
//! @param[in] nativeBytes The array of characters to append. A null pointer
//! is invalid.
//! @param[in] byteCount The count of bytes in nativeBytes to process.
void appendNative(std::wstring &destination, const char *nativeBytes,
                  size_t byteCount)
{
    if (byteCount > 0)
    {
#ifdef _WIN32
        // Calculate how many wide characters the conversion is likely to
        // produce.
        int inputCount = static_cast<int>(byteCount);
        int requiredSize = ::MultiByteToWideChar(CP_THREAD_ACP, 0,
                                                    nativeBytes, inputCount,
                                                    nullptr, 0);

        if (requiredSize > 0)
        {
            // Allocate a temporary buffer to hold the converted characters.
            std::vector<wchar_t> buffer;
            buffer.resize(static_cast<size_t>(requiredSize), L'\0');

            // Convert characters into the temporary buffer.
            requiredSize = ::MultiByteToWideChar(CP_THREAD_ACP, 0,
                                                 nativeBytes, inputCount,
                                                 buffer.data(), requiredSize);

            if (requiredSize > 0)
            {
                // Append the buffer contents to the string.
                destination.append(buffer.data(), static_cast<size_t>(requiredSize));
            }
        }
#else
        mbstate_t state;
        zeroFill(state);

        const size_t notEnoughBytes = static_cast<size_t>(-2);
        const size_t badSequence = static_cast<size_t>(-1);

        for (size_t index = 0; index < byteCount; )
        {
            wchar_t wideChar;
            size_t bytesLeft = byteCount - index;
            size_t result = mbrtowc(&wideChar, nativeBytes + index,
                                    bytesLeft, &state);

            if (result == 0)
            {
                destination.push_back(L'\0');
                ++index;
            }
            else if (result == badSequence)
            {
                // Reset the state and try at the next byte.
                ++index;
                zeroFill(state);
            }
            else if (result == notEnoughBytes)
            {
                // We've run out of bytes, exit the loop.
                index = byteCount;
            }
            else
            {
                destination.push_back(wideChar);
                index += result;
            }
        }
#endif
    }
}

//! @brief Appends a wide character string to a UTF-8 encoded STL string.
//! @param[out] destination The UTF-8 string to append to.
//! @param[in] wideChars The array of wide characters to convert.
//! @param[in] charCount The count of elements in the wideChars array.
void appendWide(std::string &destination, wchar_cptr_t wideChars,
                size_t charCount)
{
    ToUtf8Converter outputConverter;
#ifdef WCHAR_IS_32BIT
    for (size_t sourceIndex = 0; sourceIndex < charCount; ++sourceIndex)
    {
        // No need for conversion from wide characters to code points.
        if (outputConverter.setCodePoint(static_cast<char32_t>(wideChars[sourceIndex])) > 0)
        {
            uint8_t utf8Byte;

            while (outputConverter.tryGetNextByte(utf8Byte))
            {
                destination.push_back(static_cast<char>(utf8Byte));
            }
        }
    }
#else
    FromWideConverter inputConverter;
    char32_t codePoint;
    bool hasError;

    for (size_t sourceIndex = 0; sourceIndex < charCount; ++sourceIndex)
    {
        // Convert one or more characters to a single code point.
        if (inputConverter.tryConvert(wideChars[sourceIndex],
                                      codePoint, hasError))
        {
            // Encode the code point as UTF-8.
            if (outputConverter.setCodePoint(codePoint) > 0)
            {
                uint8_t utf8Byte;

                while (outputConverter.tryGetNextByte(utf8Byte))
                {
                    destination.push_back(static_cast<char>(utf8Byte));
                }
            }
        }
        else if (hasError)
        {
            inputConverter.reset();
        }
    }
#endif
}

//! @brief Appends a Unicode code point to a UTF-8 encoded STL string.
//! @param[in] destination The UTF-8 encoded STL string to append to.
//! @param[in] codePoint The Unicode code point to append.
//! @retval true The code point was valid and its UTF-8 encoding was successfully
//! appended to the string.
//! @retval false The code point was invalid.
bool appendCodePoint(std::string &destination, char32_t codePoint)
{
    ToUtf8Converter converter;
    bool wasAppended = false;

    if (converter.setCodePoint(codePoint) > 0)
    {
        uint8_t next = 0;

        while (converter.tryGetNextByte(next))
        {
            destination.push_back(static_cast<char>(next));
            wasAppended = true;
        }
    }

    return wasAppended;
}

//! @brief Determines whether a Unicode code point falls in the range of
//! valid values.
//! @param[in] codePoint The Unicode character value to check.
//! @retval true The character value is valid.
//! @retval false The character value falls into a reserved range or is beyond
//! the maximum character value.
bool isValidCodePoint(char32_t codePoint)
{
    uint32_t codePointData = static_cast<uint32_t>(codePoint);
    bool isValid;

    if (codePointData < 0xD800)
    {
        isValid = true;
    }
    else if (codePointData < 0xE000)
    {
        isValid = false;
    }
    else if (codePointData <= 0x10FFFF)
    {
        isValid = true;
    }
    else
    {
        isValid = false;
    }

    return isValid;
}

//! @brief Determines if a Unicode character represents a white space.
//! @param[in] codePoint The Unicode code point to check.
//! @retval true The code point represents a gap between words.
//! @retval false The code point does not represent white space.
bool isWhiteSpace(char32_t codePoint)
{
    return codePoint <= U' ';
}

//! @brief Determines if a null-terminated array of UTF-8 characters is empty.
//! @param[in] utf8Array A pointer to the array of UTF-8 encoded bytes.
//! @retval true Either utf8Array is nullptr or points directly to a null byte.
//! @retval false There is at least one character byte pointed to by utf8Array.
bool isNullOrEmpty(const utf8_cptr_t utf8Array)
{
    return (utf8Array == nullptr) || (*utf8Array == '\0');
}

//! @brief Gets a view of an empty string.
const std::string_view &getEmpty()
{
    static char empty = '\0';
    static std::string_view emptyView(&empty, 0);

    return emptyView;
}

//! @brief Ensures that a pointer to a UTF-8 encoded character array is
//! always valid, albeit pointing directly to a null terminator.
//! @param[in] utf8Text The possibly-null pointer to a UTF-8 byte array.
//! @return A non-null pointer to a UTF-8 encoded byte array.
utf8_cptr_t ensureNotNull(utf8_cptr_t utf8Text)
{
    if (utf8Text == nullptr)
    {
        utf8Text = getEmpty().data();
    }

    return utf8Text;
}

}} // namespace Ag::Utf
////////////////////////////////////////////////////////////////////////////////


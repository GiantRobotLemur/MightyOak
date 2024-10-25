//! @file Core/String.cpp
//! @brief The definition of an immutable UTF-8-encoded string value data type.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <unordered_set>

#include "StringPrivate.hpp"
#include "Win32API.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/ScalarParser.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Variant.hpp"

namespace Ag {

namespace {
///////////////////////////////////////////////////////////////////////////////
// Local Data Types
///////////////////////////////////////////////////////////////////////////////
typedef StringPrivate *StringPrivatePtr;
typedef const StringPrivate *StringPrivateCPtr;
typedef std::shared_ptr<StringPrivate> StringPrivateSPtr;

//! @brief An object holding the UTF-8 encoding of a single Unicode code point.
class UTF8CodePoint
{
private:
    // Internal Fields
    char _encoded[Utf::MaxUTF8EncodingLength + 1];
    uint32_t _encodingLength;
public:
    // Construction
    //! @brief Constructs an object which holds the encoded code point data.
    //! @param[in] codePoint The Unicode code point to convert to UTF-8.
    UTF8CodePoint(char32_t codePoint) :
        _encodingLength(0)
    {
        Utf::ToUtf8Converter converter;

        _encodingLength = converter.setCodePoint(codePoint);

        for (uint32_t length = 0; length < _encodingLength; ++length)
        {
            uint8_t next;

            if (converter.tryGetNextByte(next))
            {
                _encoded[length] = static_cast<char>(next);
            }
            else
            {
                break;
            }
        }

        _encoded[_encodingLength] = '\0';
    }

    //! @brief Determines if the code point specified at construction was valid.
    //! @retval true The initial code point was valid.
    //! @retval false The initial code point was invalid.
    bool isValid() const { return _encodingLength > 0; }

    //! @brief Gets the count of bytes used to encode the code point.
    uint32_t getLength() const { return _encodingLength; }

    //! @brief Gets the null-terminated UTF-8 encoding of the code point.
    utf8_cptr_t getEncoding() const { return _encoded; }
};

//! @brief Holds all unique instances of any string value in use.
class StringPool
{
private:
    // Internal Data Types
    typedef std::lock_guard<std::mutex> MutexGuard;

    //! @brief A hash provider which merely passes the value back from the
    //! underlying StringPrivate object.
    struct StringPrivateHashProvider
    {
        size_t operator()(StringPrivateCPtr str) const
        {
            return (str == nullptr) ? 0 : str->getHashCode();
        }
    };

    //! @brief A StringPrivate equal to provider which compares hash codes
    //! and then string contents.
    struct StringPrivateEqualTo
    {
        bool operator()(StringPrivateCPtr lhs, StringPrivateCPtr rhs) const
        {
            if (lhs == nullptr)
            {
                return rhs == nullptr;
            }
            else if (rhs == nullptr)
            {
                return false;
            }
            else if (lhs->getHashCode() == rhs->getHashCode())
            {
                return (*lhs == *rhs);
            }
            else
            {
                return false;
            }
        }
    };

    //! @brief A hash set of pointers to StringInternal objects.
    typedef std::unordered_set<StringPrivateCPtr,
        StringPrivateHashProvider,
        StringPrivateEqualTo> StringPrivateSet;

    // Internal Fields
    StringPrivateSet _allStrings;
    StringPrivateSPtr _emptyStringPtr;
    std::mutex _globalLock;

public:
    // Construction/Destruction
    //! @brief Constructs an empty pool or reference counted strings.
    StringPool() :
        _emptyStringPtr(std::make_shared<StringPrivate>())
    {
    }

    StringPool(const StringPool &) = delete;
    StringPool(StringPool &&) = delete;
    virtual ~StringPool() = default;

    // Accessors
    StringPrivateSPtr getEmptyString() { return _emptyStringPtr; }

    // Operations
    StringPool &operator=(StringPool &&) = delete;
    StringPool &operator=(const StringPool &) = delete;

    //! @brief Removes a string from the pool when a call to release() returns
    //! true, indicating the reference count has reached 0.
    //! @param[in] str The string to dispose of.
    void destroyString(StringPrivatePtr str)
    {
        if ((str != nullptr) && (str != _emptyStringPtr.get()))
        {
            // Claim the lock.
            MutexGuard guard(_globalLock);

            auto pos = _allStrings.find(str);

            if (pos != _allStrings.end())
            {
                _allStrings.erase(pos);
            }
        }
    }

    //! @brief Gets the shared value of an immutable string identified by a key.
    //! @param[in] key The key giving the UTF-8 value of the immutable string
    //! to obtain.
    //! @return A shared pointer to a string value in the global pool.
    StringPrivateSPtr getString(const StringPrivate &key)
    {
        StringPrivateSPtr str;

        if (key.isEmpty())
        {
            str = _emptyStringPtr;
        }
        else
        {
            // Claim the lock.
            MutexGuard guard(_globalLock);

            auto pos = _allStrings.find(&key);

            if (pos == _allStrings.end())
            {
                // Dynamically allocate a shared string value object.
                str = std::make_shared<StringPrivate>(key);

                // Keep a copy in the index. It's lifetime will be governed by
                // the use of the shared pointer out in the wild.
                _allStrings.insert(str.get());
            }
            else
            {
                // Replicate the shared pointer already out in the wild.
                str = const_cast<StringPrivatePtr>(*pos)->shared_from_this();
            }
        }

        return str;
    }
};

void processScalarCharacters(ScalarParser &parser, const std::string &source)
{
    bool isLeading = true;

    for (char next : source)
    {
        // Skip leading white space.
        if (isLeading)
        {
            if (std::isspace(next) == 0)
            {
                isLeading = false;
            }
            else
            {
                continue;
            }
        }

        if (parser.tryProcessCharacter(next) == false)
        {
            break;
        }
    }
}

//! @brief Converts a scalar to a printable hexadecimal digit.
//! @param[in] value The scalar to convert (0-15).
//! @return The value rendered as a single hex digit.
char toHexDigit(uint32_t value)
{
    static const char digits[] = "0123456789ABCDEF";

    return digits[value & 0x0F];
}

} // Anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// Local Data
///////////////////////////////////////////////////////////////////////////////
const String String::Empty;

namespace {

///////////////////////////////////////////////////////////////////////////////
// Local Functions
///////////////////////////////////////////////////////////////////////////////
//! @brief Gets the singleton global string value pool.
StringPool &getGlobalPool()
{
    static StringPool *globalPool = nullptr;

    if (globalPool == nullptr)
    {
        // Ensure that the string pool is initialised, but never destroyed so
        // that even static finalization can rely on the object existing, not
        // matter what order things are disposed of in.
        globalPool = new StringPool();
    }

    return *globalPool;
}

//! @brief Transforms a string using a wide character conversion function.
//! @param[in] text The string to convert.
//! @param[in] transformFn A pointer to the function used to transform each
//! wide character.
//! @return The transformed string, possibly the original if no characters were
//! actually changed.
String transform(const String &text, wint_t (*transformFn)(wint_t))
{
    if (text.isEmpty())
    {
        return text;
    }
    else
    {
        std::wstring buffer;
        buffer.reserve(text.getWideLength());

        utf8_cptr_t source = text.getUtf8Bytes();
        Utf::FromUtf8Converter fromConverter;
        Utf::ToWideConverter toConverter;
        bool isDifferent = false, hasError = false;
        char32_t codePoint;
        wchar_t nextWide;

        for (size_t index = 0, count = text.getUtf8Length(); index < count; ++index)
        {
            if (fromConverter.tryConvert(static_cast<uint8_t>(source[index]),
                                         codePoint, hasError))
            {
                toConverter.setCodePoint(codePoint);

                while (toConverter.tryGetNextCharacter(nextWide))
                {
                    wchar_t transformed = transformFn(nextWide);

                    if (transformed != nextWide)
                    {
                        isDifferent = true;
                    }

                    buffer.push_back(transformed);
                }
            }
            else if (hasError)
            {
                fromConverter.reset();
                isDifferent = true;
            }
        }

        if (isDifferent)
        {
            return String(buffer);
        }
        else
        {
            return text;
        }
    }
}

} // Anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// Member Functions
///////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an iterator representing a position within a string.
//! @param[in] source The inner string data.
//! @param[in] offset The offset in bytes into the string of the beginning of
//! the next code point.
String::iterator::iterator(const std::shared_ptr<StringPrivate> &source,
                             size_t offset) :
    _source(source),
    _offset(offset),
    _cachedEncodedLength(0),
    _cachedValue(0),
    _hasCachedValue(false)
{
}

//! @brief Copies an iterator without going to the effort of copying its
//! internal cached values.
//! @param[in] rhs The iterator to copy.
String::iterator::iterator(const String::iterator &rhs) :
    _source(rhs._source),
    _offset(rhs._offset),
    _cachedEncodedLength(0),
    _cachedValue(0),
    _hasCachedValue(false)
{
}

//! @brief Copies an iterator without copying the cached converted character.
//! @param[in] rhs The iterator to copy.
//! @returns A reference to the current iterator.
String::iterator &String::iterator::operator=(const String::iterator &rhs)
{
    _source = rhs._source;
    _offset = rhs._offset;
    _cachedEncodedLength = 0;
    _cachedValue = 0;
    _hasCachedValue = false;

    return *this;
}

//! @brief Gets a read-only reference to the code point the iterator
//! current indexes.
const char32_t &String::iterator::operator*() const
{
    ensureCacheValid();

    return _cachedValue;
}

//! @brief Gets a read-only pointer to the code point the iterator
//! currently indexes.
const char32_t *String::iterator::operator->() const
{
    ensureCacheValid();

    return &_cachedValue;
}

//! @brief Advances the iterator to point to the next complete Unicode code point.
//! @return A reference to the current iterator.
String::iterator &String::iterator::operator++()
{
    _hasCachedValue = false;

    if (_hasCachedValue)
    {
        // Use the already calculated length to advance the offset.
        _offset += _cachedEncodedLength;
    }
    else
    {
        size_t pos = _offset;

        if (tryAdvance(pos))
        {
            _offset = pos;
        }
        else
        {
            // Set to the end of the string.
            _offset = _source->getUTF8Length();
        }
    }

    return *this;
}

//! @brief Advances the iterator to point to the next complete Unicode code point.
//! @return A copy of the iterator before it was advanced.
String::iterator String::iterator::operator++(int)
{
    // Copy the current position.
    String::iterator currentPos(*this);

    _hasCachedValue = false;

    if (_hasCachedValue)
    {
        // Use the already calculated length to advance the offset.
        _offset += _cachedEncodedLength;
    }
    else
    {
        size_t pos = _offset;

        if (tryAdvance(pos))
        {
            _offset = pos;
        }
        else
        {
            // Set to the end of the string.
            _offset = _source->getUTF8Length();
        }
    }

    // Return the copy of the iterator before it was advanced.
    return currentPos;
}

//! @brief Retreats the iterator to the previous complete Unicode code point.
//! @return A reference to the current iterator.
String::iterator &String::iterator::operator--()
{
    _hasCachedValue = false;
    size_t pos = _offset;

    if (tryRetreat(pos))
    {
        _offset = pos;
    }
    else
    {
        // Set to the start of the string.
        _offset = 0;
    }

    return *this;
}

//! @brief Retreats the iterator to the previous complete Unicode code point.
//! @return An iterator giving the copy of the position before it was retreated.
String::iterator String::iterator::operator--(int)
{
    iterator currentPos(*this);
    _hasCachedValue = false;

    size_t pos = _offset;

    if (tryRetreat(pos))
    {
        _offset = pos;
    }
    else
    {
        // Set to the start of the string.
        _offset = 0;
    }

    return currentPos;
}

//! @brief Determines whether an iterator indexes the same code point in the
//! same string as the current one.
//! @param[in] rhs The iterator to test against.
//! @retval true The iterators identify the same code point in the same string.
//! @retval false The iterators identify different code points, possibly in
//! different strings.
bool String::iterator::operator==(const String::iterator &rhs) const
{
    return (rhs._offset == _offset) && (rhs._source.get() == rhs._source.get());
}

//! @brief Determines whether an iterator identifies a different code point
//! possibly in a different string to the current one.
//! @param[in] rhs The iterator to test against.
//! @retval true The iterators identify different code points, possibly in
//! @retval false The iterators identify the same code point in the same string.
//! different strings.
bool String::iterator::operator!=(const String::iterator &rhs) const
{
    return (rhs._offset != _offset) || (rhs._source.get() != rhs._source.get());
}

//! @brief Returns the byte offset into the UTF-8 string of the first
//! byte of the Unicode code point the iterator points to.
size_t String::iterator::getOffset() const
{
    return _offset;
}

//! @brief Determines if the iterator represents a position past the end
//! of the string.
bool String::iterator::isPastEnd() const
{
    return _offset >= _source->getUTF8Length();
}

//! @brief Ensures the cached Unicode code point information relating to the byte
//! at the current position, is valid.
void String::iterator::ensureCacheValid() const
{
    if (_hasCachedValue == false)
    {
        _cachedValue = 0;
        _cachedEncodedLength = 0;
        const std::string &data = _source->getData();
        size_t max = data.length();

        if (_offset < data.length())
        {
            Utf::FromUtf8Converter  converter;
            bool hasError = false;
            uint8_cptr_t dataBytes = reinterpret_cast<uint8_cptr_t>(data.c_str());

            for (size_t pos = _offset; (hasError == false) && (pos < max); ++pos)
            {
                if (converter.tryConvert(dataBytes[pos], _cachedValue, hasError))
                {
                    _cachedEncodedLength = pos + 1 - _offset;
                    break;
                }
            }
        }

        // Mark the cache as valid, whether we decoded a valid code point or not.
        _hasCachedValue = true;
    }
}

//! @brief Attempts to move an offset to the beginning of the encoding of the
//! next Unicode code point.
//! @param[in,out] nextOffset The offset to start from and receive the offset
//! of the first byte of the encoding of the next code point.
//! @retval true Another code point was found, nextOffset points to it.
//! @retval false The end of the string was reached or an error encountered.
bool String::iterator::tryAdvance(size_t &nextOffset) const
{
    Utf::FromUtf8Converter  converter;
    size_t max = _source->getData().length();
    bool hasError = false;
    bool hasNext = false;
    uint8_cptr_t data = reinterpret_cast<uint8_cptr_t>(_source->getData().c_str());

    for (size_t pos = nextOffset; (hasError == false) && (pos < max); ++pos)
    {
        if (converter.tryConvert(data[pos], _cachedValue, hasError))
        {
            nextOffset = pos + 1;
            hasNext = true;
            break;
        }
    }

    return hasNext;
}

//! @brief Attempts to decrement an offset so that it points to the first byte
//! of the previous whole Unicode code point.
//! @param[in,out] previousOffset The offset of the code point to work backward
//! from.
//! @retval true The previous code point was found.
//! @retval false The beginning of the string was encountered.
bool String::iterator::tryRetreat(size_t &previousOffset) const
{
    bool hasPrevious = false;

    uint8_cptr_t data = reinterpret_cast<uint8_cptr_t>(_source->getData().c_str());

    for (size_t pos = previousOffset; pos > 0; --pos)
    {
        uint8_t previousByte = data[pos - 1];

        if ((previousByte < 0x80) || (previousByte >= 0xC0))
        {
            // We've found a lead byte.
            previousOffset = pos - 1;
            hasPrevious = true;
            break;
        }
    }

    return hasPrevious;
}

//! @brief Creates an empty string value.
String::String() :
    _str(getGlobalPool().getEmptyString())
{
}

//! @brief Constructs a string value from a null-terminated array of UTF-8
//! bytes.
//! @param[in] nullTerminatedUtf8 The array of bytes which form the string.
String::String(utf8_cptr_t nullTerminatedUtf8)
{
    StringPrivate key(nullTerminatedUtf8);

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs a string value from a bounded array of UTF-8 encoded bytes.
//! @param[in] boundedUtf8 The array of bytes representing the string.
//! @param[in] byteCount The count of bytes in boundedUtf8 not including any
//! null terminator.
String::String(utf8_cptr_t boundedUtf8, size_t byteCount)
{
    StringPrivate key(boundedUtf8, byteCount);

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs a string value from a null-terminated array of UTF-16
//! 16-bit words.
//! @param[in] nullTerminatedUtf16 The array of 16-bit words which form the string.
String::String(utf16_cptr_t nullTerminatedUtf16)
{
    StringPrivate key(nullTerminatedUtf16);

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs a string value from a bounded array of UTF-16 encoded words.
//! @param[in] boundedUtf16 The array of words representing the string.
//! @param[in] wordCount The count of 16-bit words in boundedUtf16 not including any
//! null terminator.
String::String(utf16_cptr_t boundedUtf16, size_t wordCount)
{
    StringPrivate key(boundedUtf16, wordCount);

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs a string value from a null-terminated array of UTF-32
//! code points.
//! @param[in] nullTerminatedUtf32 The array of Unicode code points which form
//! the string.
String::String(utf32_cptr_t nullTerminatedUtf32)
{
    StringPrivate key(nullTerminatedUtf32);

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs a string value from a bounded array of Unicode code points.
//! @param[in] boundedUtf32 The array of Unicode code points representing the string.
//! @param[in] codePointCount The count of code points in boundedUtf32 not including any
//! null terminator.
String::String(utf32_cptr_t boundedUtf32, size_t codePointCount)
{
    StringPrivate key(boundedUtf32, codePointCount);

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs a string value from a null-terminated array of
//! wide characters.
//! @param[in] nullTerminatedWide The array of wide characters which form
//! the string.
String::String(wchar_cptr_t nullTerminatedWide)
{
    StringPrivate key(nullTerminatedWide);

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs a string value from a bounded array of wide characters.
//! @param[in] boundedWide The array of wide characters representing the string.
//! @param[in] charCount The count of wide characters in boundedWide not including any
//! null terminator.
String::String(wchar_cptr_t boundedWide, size_t charCount)
{
    StringPrivate key(boundedWide, charCount);

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs an immutable UTF-8 encoded string value from a mutable
//! UTF-8 encoded STL string view.
//! @param[in] stlUtf8StringView The view of the string to copy to the object.
String::String(const std::string_view &stlUtf8StringView)
{
    StringPrivate key(stlUtf8StringView.data(),
                      stlUtf8StringView.length());

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs an immutable UTF-8 encoded string value from a mutable
//! UTF-8 encoded STL string.
//! @param[in] stlUtf8String The string to copy to the object.
String::String(const std::string &stlUtf8String)
{
    StringPrivate key(stlUtf8String.c_str(), stlUtf8String.length());

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs an immutable UTF-8 encoded string value from a mutable
//! UTF-16 encoded STL string view.
//! @param[in] stlUtf16View The string to convert and copy to the object.
String::String(const std::u16string_view &stlUtf16View)
{
    StringPrivate key(stlUtf16View.data(), stlUtf16View.length());

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs an immutable UTF-8 encoded string value from a mutable
//! UTF-16 encoded STL string.
//! @param[in] stlUtf16String The string to convert and copy to the object.
String::String(const std::u16string &stlUtf16String)
{
    StringPrivate key(stlUtf16String.c_str(), stlUtf16String.length());

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs an immutable UTF-8 encoded string value from a mutable
//! Unicode STL string view.
//! @param[in] stlUtf32View The string to convert and copy to the object.
String::String(const std::u32string_view &stlUtf32View)
{
    StringPrivate key(stlUtf32View.data(), stlUtf32View.length());

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs an immutable UTF-8 encoded string value from a mutable
//! Unicode STL string.
//! @param[in] stlUtf32String The string to convert and copy to the object.
String::String(const std::u32string &stlUtf32String)
{
    StringPrivate key(stlUtf32String.c_str(), stlUtf32String.length());

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs an immutable UTF-8 encoded string value from a mutable
//! wide character STL string view.
//! @param[in] stlWideView The string to convert and copy to the object.
String::String(const std::wstring_view &stlWideView)
{
    StringPrivate key(stlWideView.data(), stlWideView.length());

    _str = getGlobalPool().getString(key);
}

//! @brief Constructs an immutable UTF-8 encoded string value from a mutable
//! wide character STL string.
//! @param[in] stlWideString The string to convert and copy to the object.
String::String(const std::wstring &stlWideString)
{
    StringPrivate key(stlWideString.c_str(), stlWideString.length());

    _str = getGlobalPool().getString(key);
}

//! @brief Indicates whether the string contains no characters.
//! @retval true The string contains no characters.
//! @retval false The string contains at least one character.
bool String::isEmpty() const
{
    return _str->isEmpty();
}

//! @brief Gets the hash code calculated from the value of the string.
size_t String::getHashCode() const
{
    return _str->getHashCode();
}

//! @brief Gets the length of the string in it's native UTF-8 encoding in bytes.
size_t String::getUtf8Length() const
{
    return _str->getUTF8Length();
}

//! @brief Gets count of 16-bit UTF-16 words required to represent the string.
size_t String::getUtf16Length() const
{
    return _str->getUTF16Length();
}

//! @brief Gets the count of Unicode code points which the string represents.
size_t String::getUtf32Length() const
{
    return _str->getUTF32Length();
}

//! @brief Gets the count of wide characters which is required to represent
//! the value of the string.
size_t String::getWideLength() const
{
#ifdef WCHAR_IS_32BIT
    return _str->getUTF32Length();
#else
    return _str->getUTF16Length();
#endif
}

//! @brief Gets the count of characters if the string was printed.
//! @returns The count of printable characters in the string.
//! @note Unlike other length value, this value is calculated, not cached.
size_t String::getPrintLength() const
{
    uint8_cptr_t text = reinterpret_cast<uint8_cptr_t>(_str->getData().c_str());
    Utf::FromUtf8Converter fromUtf8;
    Utf::ToWideConverter toWide;
    size_t printable = 0;
    char32_t codePoint = 0;
    wchar_t wideChar = 0;
    bool hasErrors = false;

    for (size_t index = 0, count = _str->getData().length(); index < count; ++index)
    {
        if (fromUtf8.tryConvert(text[index], codePoint, hasErrors))
        {
            toWide.setCodePoint(codePoint);

            while (toWide.tryGetNextCharacter(wideChar))
            {
                if (iswprint(wideChar))
                {
                    ++printable;
                }
            }
        }
        else if (hasErrors)
        {
            fromUtf8.reset();
        }
    }

    return printable;
}

//! @brief Gets the value of the string as a null-terminated array of
//! UTF-8 encoded bytes.
utf8_cptr_t String::getUtf8Bytes() const
{
    return _str->getData().c_str();
}

//! @brief Gets the reference count of the shared string value.
//! @note This member function is for testing purposes only!
long String::getReferenceCount() const
{
    return _str.use_count();
}

//! @brief Determines if the current string contains another as some or all of
//! its value.
//! @param[in] rhs The null-terminated array of UTF-8 encoded bytes specifying
//! the sub-string to determine the presence of.
//! @retval true The string rhs forms part of all of the current string.
//! @retval false The string rhs does not appear in the current string.
bool String::contains(utf8_cptr_t rhsUtf8) const
{
    bool isRhsEmpty = Utf::isNullOrEmpty(rhsUtf8);

    if (_str->isEmpty() || isRhsEmpty)
    {
        return true;
    }
    else
    {
        return _str->getData().find(rhsUtf8) != std::string::npos;
    }
}

//! @brief Determines if the current string contains another as some or all of
//! its value.
//! @param[in] rhs The sub-string to determine the presence of.
//! @retval true The string rhs forms part of all of the current string.
//! @retval false The string rhs does not appear in the current string.
bool String::contains(const std::string_view &rhsUtf8) const
{
    if (_str->isEmpty())
    {
        return rhsUtf8.empty();
    }
    else if (rhsUtf8.empty())
    {
        return true;
    }
    else
    {
        return _str->getData().find(rhsUtf8) != std::string::npos;
    }
}

//! @brief Determines if the current string contains another as some or all of
//! its value.
//! @param[in] rhs The sub-string to determine the presence of.
//! @retval true The string rhs forms part of all of the current string.
//! @retval false The string rhs does not appear in the current string.
bool String::contains(const String &rhs) const
{
    if (_str->isEmpty())
    {
        return rhs._str->isEmpty();
    }
    else if (rhs._str->isEmpty())
    {
        return true;
    }
    else
    {
        return _str->getData().find(rhs._str->getData()) != std::string::npos;
    }
}

//! @brief Gets the iterator representing the position of the first
//! char32_t code point in the string.
String::iterator String::begin() const
{
    return iterator(_str, 0);
}

//! @brief Gets the iterator representing the position just past the last
//! char32_t code point in the string.
String::iterator String::end() const
{
    return iterator(_str, _str->getUTF8Length());
}

//! @brief Searches for the first occurrence of a specified Unicode code point.
//! @param[in] character The code point to search for.
//! @return The position of the first matching character or String::end().
String::iterator String::find(char32_t character) const
{
    if (Utf::isValidCodePoint(character) == false)
    {
        //throw UnicodeConversionException(GET_CURRENT_LOCATION(), character);
    }

    // Create a UTF-8 encoding of the code point.
    UTF8CodePoint codePoint(character);

    // Search for the encoded sequence of bytes.
    const std::string &source = _str->getData();

    size_t offset = source.find(codePoint.getEncoding(), 0,
                                codePoint.getLength());

    return iterator(_str, (offset == std::string::npos) ? source.length() : offset);
}

//! @brief Searches for the next occurrence of a specified Unicode code point
//! starting at a specified position.
//! @param[in] from The position of the first character to search.
//! @param[in] character The code point to search for.
//! @return The position of the first matching character or String::end().
String::iterator String::find(const String::iterator from,
                                  char32_t character) const
{
    if (Utf::isValidCodePoint(character) == false)
    {
        //throw UnicodeConversionException(GET_CURRENT_LOCATION(), character);
    }
    else if (from._source != _str)
    {
        //throw PrimitiveArgumentException(GET_CURRENT_LOCATION(), "from");
    }

    // Create a UTF-8 encoding of the code point.
    UTF8CodePoint codePoint(character);

    // Search for the encoded sequence of bytes.
    const std::string &source = _str->getData();

    size_t offset = source.find(codePoint.getEncoding(), from._offset,
                                codePoint.getLength());

    return iterator(_str, (offset == std::string::npos) ? source.length() : offset);
}

//! @brief Searches for the last occurrence of a specified Unicode code point.
//! @param[in] character The code point to search for.
//! @return The position of the last matching character or String::end().
String::iterator String::reverseFind(char32_t character) const
{
    if (Utf::isValidCodePoint(character) == false)
    {
        //throw UnicodeConversionException(GET_CURRENT_LOCATION(), character);
    }

    // Create a UTF-8 encoding of the code point.
    UTF8CodePoint codePoint(character);

    // Search backward for the encoded sequence of bytes.
    const std::string &source = _str->getData();
    size_t offset = std::string::npos;

    if (codePoint.getLength() == 1)
    {
        // Quickly search for a single byte.
        offset = source.rfind(*codePoint.getEncoding(), std::string::npos);
    }
    else
    {
        // Search more slowly for an encoded sub-string.
        offset = source.rfind(codePoint.getEncoding(), std::string::npos,
                              codePoint.getLength());
    }

    return iterator(_str, (offset == std::string::npos) ? source.length() : offset);
}

//! @brief Searches backward for the previous occurrence of a specified Unicode
//! code point starting at a specified position.
//! @param[in] from The position of the character to search backward from.
//! @param[in] character The code point to search for.
//! @return The position of the last matching character or String::end().
String::iterator String::reverseFind(const String::iterator from,
                                     char32_t character) const
{
    if (Utf::isValidCodePoint(character) == false)
    {
        // throw UnicodeConversionException(GET_CURRENT_LOCATION(), character);
    }
    else if (from._source != _str)
    {
        // throw PrimitiveArgumentException(GET_CURRENT_LOCATION(), "from");
    }

    // Create a UTF-8 encoding of the code point.
    UTF8CodePoint codePoint(character);

    // Search backward for the encoded sequence of bytes.
    const std::string &source = _str->getData();

    size_t offset = source.rfind(codePoint.getEncoding(), from._offset,
                                 codePoint.getLength());

    return iterator(_str, (offset == std::string::npos) ? source.length() : offset);
}

//! @brief Creates a UTF-8 string value by converting a string from the current
//! code page to Unicode.
//! @param[in] nativeString A null-terminated string encoded using the native
//! code page.
String String::fromNativeCodePage(const char *nativeString)
{
    std::wstring buffer;

    Utf::appendNative(buffer, nativeString);

    return String(buffer);
}

//! @brief Processes a format specification in order to create a string
//! containing formatted values.
//! @param[in] spec The null-terminated UTF-8 encoded format specification
//! used as a template for the string to create.
//! @param[in] params The set of values to be substituted into the string.
//! @note By default, this form of the function uses formatting options
//! derived from the neutral locale.
String String::format(utf8_cptr_t spec,
                      const std::initializer_list<Variant> &params)
{
    FormatInfo neutralFormat(LocaleInfo::getNeutral());
    std::string_view specView(spec);
    std::string buffer;

    appendFormat(neutralFormat, specView, buffer, params);

    return String(buffer);
}

//! @brief Processes a format specification in order to create a string
//! containing formatted values.
//! @param[in] format Defines how values should be formatted as text.
//! @param[in] spec The null-terminated UTF-8 encoded format specification
//! used as a template for the string to create.
//! @param[in] params The set of values to be substituted into the string.
String String::format(const FormatInfo &format, utf8_cptr_t spec,
                      const std::initializer_list<Variant> &params)
{
    std::string_view specView(spec);
    std::string buffer;

    appendFormat(format, specView, buffer, params);

    return String(buffer);
}

//! @brief Processes a format specification in order to create a string
//! containing formatted values.
//! @param[in] spec The format specification used as a template for the string
//! to create.
//! @param[in] params The set of values to be substituted into the string.
//! @note By default, this form of the function uses formatting options
//! derived from the neutral locale.
String String::format(const String &spec,
                      const std::initializer_list<Variant> &params)
{
    FormatInfo neutralFormat(LocaleInfo::getNeutral());
    std::string_view specView(spec.getUtf8Bytes(), spec.getUtf8Length());
    std::string buffer;

    appendFormat(neutralFormat, specView, buffer, params);

    return String(buffer);
}

//! @brief Processes a format specification in order to create a string
//! containing formatted values.
//! @param[in] format Defines how values should be formatted as text.
//! @param[in] spec The format specification used as a template for the string
//! to create.
//! @param[in] params The set of values to be substituted into the string.
String String::format(const FormatInfo &format, const String &spec,
                      const std::initializer_list<Variant> &params)
{
    std::string_view specView(spec.getUtf8Bytes(), spec.getUtf8Length());
    std::string buffer;

    appendFormat(format, specView, buffer, params);

    return String(buffer);
}

//! @brief Converts an Unicode character into a printable representation,
//! possibly an escaped hexadecimal value.
//! @param[in] ch The ASCII character to convert.
//! @return A string representing the character in a printable form.
String String::formatUcsNonPrintable(char32_t ch)
{
    uint32_t code = static_cast<uint32_t>(ch);

#ifdef WCHAR_IS_32BIT
    if (iswprint(static_cast<wchar_t>(ch) == 0))
#else
    if ((code < 32) ||
        ((code >= 0x80) && (code < 0xA0)) ||
        (code > 0x10FFFF))
#endif
    {
        std::string buffer;
        buffer.reserve(10);

        // Convert the value to hex, in reverse.
        while (code != 0)
        {
            buffer.push_back(toHexDigit(code & 0x0F));
            code >>= 4;
        }

        // Pad to 8 digits.
        buffer.append(8 - buffer.size(), '0');

        // Add the Unicode prefix, in reverse.
        buffer.push_back('u');
        buffer.push_back('\\');

        // Reverse the entire string.
        std::reverse(buffer.begin(), buffer.end());

        return String(buffer);
    }
    else
    {
        return String(&ch, 1);
    }
}

//! @brief Converts an ASCII character into a printable representation,
//! possibly an escaped hexadecimal value.
//! @param[in] ch The ASCII character to convert.
//! @return A string representing the character in a printable form.
String String::formatAsciiNonPrintable(char ch)
{
    std::string buffer;

    if (isprint(ch) && (static_cast<uint8_t>(ch) < 128))
    {
        buffer.push_back(ch);
    }
    else
    {
        buffer.reserve(4);
        buffer.push_back('\\');
        buffer.push_back('x');
        buffer.push_back(toHexDigit(static_cast<uint8_t>(ch) >> 4));
        buffer.push_back(toHexDigit(static_cast<uint8_t>(ch) & 0x0F));
    }

    return String(buffer);
}


//! @brief Determines if two string have the same value.
//! @param[in] rhs The string to compare the current one with.
//! @retval true The rhs string is identical to the current one.
//! @retval false The rhs string differs from the current one.
bool String::operator==(const String &rhs) const
{
    // If the inner pointers are identical, the String instances reference the
    // same globally unique inner string.
    return _str.get() == rhs._str.get();
}

//! @brief Determines if two string have differing values.
//! @param[in] rhs The string to compare the current one with.
//! @retval true The rhs string differs from the current one.
//! @retval false The rhs string is identical to the current one.
bool String::operator!=(const String &rhs) const
{
    // If the inner pointers are identical, the String instances reference the
    // same globally unique inner string.
    return _str.get() != rhs._str.get();
}

//! @brief Performs a less-than comparison between the current and another string.
//! @param[in] rhs The string to compare with.
//! @retval true The current string has a lower value than rhs.
//! @retval false The current string has an equal or greater value than rhs.
bool String::operator<(const String &rhs) const
{
    bool isLessThan = false;

    int diff = 0;

    size_t minSize = std::min(_str->getUTF8Length(), rhs._str->getUTF8Length());

    if (minSize > 0)
    {
        diff = std::memcmp(_str->getData().c_str(),
                           rhs._str->getData().c_str(), minSize);
    }

    if (diff == 0)
    {
        // The shortest string has the lower value.
        isLessThan = _str->getUTF8Length() < rhs._str->getUTF8Length();
    }
    else
    {
        isLessThan = diff < 0;
    }

    return isLessThan;
}

//! @brief Concatenates a string to the current one to create a new string.
//! @param[in] rhs The string to concatenate with the current one.
//! @return The sub of the two strings.
String String::operator+(const String &rhs) const
{
    if (_str->isEmpty())
    {
        return rhs;
    }
    else if (rhs._str->isEmpty())
    {
        return *this;
    }
    else
    {
        std::string buffer;
        const std::string &lhsValue = _str->getData();
        const std::string &rhsValue = rhs._str->getData();

        buffer.reserve(lhsValue.length() + rhsValue.length());
        buffer.assign(lhsValue);
        buffer.append(rhsValue);

        return String(buffer);
    }
}

//! @brief Overwrites the string with a null-terminated array of UTF-8 bytes.
//! @param[in] nullTerminatedUtf8 The array of bytes representing the string to
//! assign.
//! @return A reference to the current string.
String String::operator=(utf8_cptr_t nullTerminatedUtf8)
{
    StringPrivate key(nullTerminatedUtf8);

    _str = getGlobalPool().getString(key);

    return *this;
}

//! @brief Overwrites the string with a null-terminated array of UTF-16
//! characters.
//! @param[in] nullTerminatedUtf16 The array of bytes representing the string to
//! assign.
//! @return A reference to the current string.
String String::operator=(utf16_cptr_t nullTerminatedUtf16)
{
    StringPrivate key(nullTerminatedUtf16);

    _str = getGlobalPool().getString(key);

    return *this;
}

//! @brief Overwrites the string with a null-terminated array of UTF-32
//! code points.
//! @param[in] nullTerminatedUtf32 The array of code points representing the
//! string to assign.
//! @return A reference to the current string.
String String::operator=(utf32_cptr_t nullTerminatedUtf32)
{
    StringPrivate key(nullTerminatedUtf32);

    _str = getGlobalPool().getString(key);

    return *this;
}

//! @brief Overwrites the string with a null-terminated array of wide characters.
//! @param[in] nullTerminatedWide The array of wide characters representing the
//! string to assign.
//! @return A reference to the current string.
String String::operator=(wchar_cptr_t nullTerminatedWide)
{
    StringPrivate key(nullTerminatedWide);

    _str = getGlobalPool().getString(key);

    return *this;
}


//! @brief Overwrites the current string value with the contents of a UTF-8
//! encoded STL string.
//! @param[in] stlUtf8String The string value to assign.
//! @return A reference to the current object.
String String::operator=(const std::string &stlUtf8String)
{
    StringPrivate key(stlUtf8String.c_str(), stlUtf8String.length());

    _str = getGlobalPool().getString(key);

    return *this;
}

//! @brief Overwrites the current string value with the contents of a UTF-16
//! encoded STL string.
//! @param[in] stlUtf16String The string value to convert and assign.
//! @return A reference to the current object.
String String::operator=(const std::u16string &stlUtf16String)
{
    StringPrivate key(stlUtf16String.c_str(), stlUtf16String.length());

    _str = getGlobalPool().getString(key);

    return *this;
}

//! @brief Overwrites the current string value with the contents of a Unicode
//! STL string.
//! @param[in] stlUtf32String The string value to convert and assign.
//! @return A reference to the current object.
String String::operator=(const std::u32string &stlUtf32String)
{
    StringPrivate key(stlUtf32String.c_str(), stlUtf32String.length());

    _str = getGlobalPool().getString(key);

    return *this;
}

//! @brief Overwrites the current string value with the contents of a wide
//! STL string.
//! @param[in] wideString The string value to convert and assign.
//! @return A reference to the current object.
String String::operator=(const std::wstring &wideString)
{
    StringPrivate key(wideString.c_str(), wideString.length());

    _str = getGlobalPool().getString(key);

    return *this;
}

//! @brief Creates a version of the string where all letters are forced to
//! upper case.
String String::toUpper() const
{
    return transform(*this, std::towupper);
}

//! @brief Creates a version of the string where all letters are forced to
//! lower case.
String String::toLower() const
{
    return transform(*this, std::towlower);
}

//! @brief Gets a run of characters from the string.
//! @param[in] start The position of the first string to include.
//! @param[in] end The position of the character after the last one to include.
//! @return A string containing the run of characters specified.
String String::substring(const String::iterator &start,
                         const String::iterator &end) const
{
    if ((start._source != _str) || (start._offset > end._offset))
    {
        throw ArgumentException("start");
    }
    else if ((end._source != _str) || (end._offset > _str->getUTF8Length()))
    {
        throw ArgumentException("end");
    }

    String result;

    size_t length = end._offset - start._offset;

    if (length > 0)
    {
        if (start._offset == 0)
        {
            if (length >= _str->getUTF8Length())
            {
                // Simply copy the current string.
                result = *this;
            }
            else
            {
                // Create a value from a leading section.
                result = String(_str->getData().c_str(), length);
            }
        }
        else
        {
            // Create a value from a mid-section.
            result = String(_str->getData().c_str() + start._offset, length);
        }
    }

    return result;
}

//! @brief Gets the text as a UTF-8 encoded STL string.
std::string String::toUtf8() const
{
    return _str->getData();
}

//! @brief Returns a view of the raw UTF-8 encoded string data.
std::string_view String::toUtf8View() const
{
    const std::string &data = _str->getData();

    return std::string_view(data.c_str(), data.length());
}

//! @brief Gets the text as a UTF-16 encoded STL string.
std::u16string String::toUtf16() const
{
    std::u16string text;
    StringPrivatePtr str = _str.get();

    Utf::appendToUtf16(text, str->getData().c_str(),
                       str->getUTF8Length(), str->getUTF16Length());

    return text;
}

//! @brief Gets the text as a UTF-32 encoded STL string.
std::u32string String::toUtf32() const
{
    std::u32string text;
    StringPrivatePtr str = _str.get();

    Utf::appendToUtf32(text, str->getData().c_str(),
                       str->getUTF8Length(), str->getUTF32Length());

    return text;
}

//! @brief Converts the string to an STL wide character string.
std::wstring String::toWide() const
{
    std::wstring wideText;

    StringPrivatePtr str = _str.get();
    Utf::appendToWide(wideText, str->getData().c_str(), str->getData().length(),
#ifdef WCHAR_IS_32BIT
                      str->getUTF32Length());
#else
                      str->getUTF16Length());
#endif

    return wideText;
}

//! @brief Converts the contents of the string to wide characters and appends
//! them to a buffer.
//! @param[out] buffer The buffer to receive the converted characters.
//! @note Only the string characters are appended, not any terminating null
//! character.
void String::appendToWideBuffer(std::vector<wchar_t> &buffer) const
{
    StringPrivatePtr str = _str.get();

    if (str->isEmpty())
        return;

    size_t wideLength;

#ifdef WCHAR_IS_32BIT
    wideLength = str->getUTF32Length();
#else
    wideLength = str->getUTF16Length();
    Utf::ToUtf16Converter outputConverter;
#endif

    // Ensure the buffer has enough space for the new characters.
    size_t requiredSize = buffer.size() + wideLength + 1;

    if (requiredSize > buffer.capacity())
    {
        buffer.reserve(requiredSize);
    }

    Utf::FromUtf8Converter inputConverter;
    uint8_cptr_t bytes = reinterpret_cast<uint8_cptr_t>(str->getData().c_str());
    size_t byteCount = str->getData().length();

    bool hasError = false;
    char32_t codePoint = U'\0';

    for (size_t index = 0; index < byteCount; ++index)
    {
        // Append the converted code points raw.
        if (inputConverter.tryConvert(bytes[index], codePoint, hasError))
        {
#ifdef WCHAR_IS_32BIT
            buffer.push_back(codePoint);
#else
            // Convert to code points and then UTF-16.
            outputConverter.setCodePoint(codePoint);
            char16_t utf16word = u'\0';

            while (outputConverter.tryGetNextCharacter(utf16word))
            {
                buffer.push_back(utf16word);
            }
#endif
        }
        else if (hasError)
        {
            inputConverter.reset();
        }
    }
}

//! @brief Performs a per-byte comparison of two string.
//! @param[in] rhs The string to compare against the current one.
//! @retval <0 The current string has a lower value than rhs.
//! @retval 0 The values are identical.
//! @retval >0 The current string has a higher value than rhs.
int String::compare(const String &rhs) const
{
    int diff = 0;

    if (rhs._str.get() != _str.get())
    {
        utf8_cptr_t lhsText = _str->getData().c_str();
        size_t lhsLength = _str->getData().length();
        utf8_cptr_t rhsText = rhs._str->getData().c_str();
        size_t rhsLength = rhs._str->getData().length();

        size_t commonSize = std::min(lhsLength, rhsLength);

        diff = std::memcmp(lhsText, rhsText, commonSize);

        if ((diff == 0) && (lhsLength != rhsLength))
        {
            // The shortest string has the lowest value.
            diff = (lhsLength < rhsLength) ? -1 : 1;
        }
    }

    return diff;
}

//! @brief Performs a per-character comparison of two string ignoring
//! difference between upper and lower case.
//! @param[in] rhs The string to compare against the current one.
//! @retval <0 The current string has a lower value than rhs.
//! @retval 0 The values are identical.
//! @retval >0 The current string has a higher value than rhs.
int String::compareIgnoreCase(const String &rhs) const
{
    int diff = 0;

    if (rhs._str.get() != _str.get())
    {
        utf8_cptr_t lhsText = _str->getData().c_str();
        size_t lhsLength = _str->getData().length();
        utf8_cptr_t rhsText = rhs._str->getData().c_str();
        size_t rhsLength = rhs._str->getData().length();

        size_t commonSize = std::min(lhsLength, rhsLength);

        diff = compareBoundedStringsIgnoreCase(lhsText, rhsText,
                                               commonSize);

        if ((diff == 0) && (lhsLength != rhsLength))
        {
            // The shortest string has the lowest value.
            diff = (lhsLength < rhsLength) ? -1 : 1;
        }
    }

    return diff;
}

//! @brief Tests whether the string has a specified prefix.
//! @param[in] prefix The prefix to check for.
//! @retval true The string has at least the prefix characters at the beginning.
//! @retval false The string is too short or doesn't start with the specified
//! prefix.
bool String::startsWith(const std::string_view &prefix) const
{
    const std::string &str = _str->getData();
    bool hasPrefix = false;

    if (prefix.length() <= str.length())
    {
        hasPrefix = std::memcmp(prefix.data(), str.c_str(),
                                prefix.length()) == 0;
    }

    return hasPrefix;
}

//! @brief Attempts to parse a scalar integer from the contents of the string.
//! @param[out] scalar Receives a 32-bit signed integer if the leading
//! contents of the string could be interpreted as such.
//! @param[in] radix The radix value used to convert the value.
//! @retval true An integer was successfully extracted from the string.
//! @retval false The string could not be interpreted as an integer, or if it
//! could, its value overflowed.
bool String::tryParseScalar(int32_t &scalar, int radix /* = 10 */) const
{
    ScalarParser parser;
    parser.enableSign(true);
    parser.enableExponent(false);
    parser.enableFraction(false);
    parser.enableRadixPrefix(false);
    parser.setPreferredRadix(radix);

    processScalarCharacters(parser, _str->getData());

    return parser.tryGetValue(scalar);
}

//! @brief Attempts to parse a scalar integer from the contents of the string.
//! @param[out] scalar Receives a 32-bit unsigned integer if the leading
//! contents of the string could be interpreted as such.
//! @param[in] radix The radix value used to convert the value.
//! @retval true An integer was successfully extracted from the string.
//! @retval false The string could not be interpreted as an integer, or if it
//! could, its value overflowed.
bool String::tryParseScalar(uint32_t &scalar, int radix /* = 10 */) const
{
    ScalarParser parser;
    parser.enableSign(false);
    parser.enableExponent(false);
    parser.enableFraction(false);
    parser.enableRadixPrefix(false);
    parser.setPreferredRadix(radix);

    processScalarCharacters(parser, _str->getData());

    return parser.tryGetValue(scalar);
}

//! @brief Attempts to parse a scalar integer from the contents of the string.
//! @param[out] scalar Receives a 64-bit signed integer if the leading
//! contents of the string could be interpreted as such.
//! @param[in] radix The radix value used to convert the value.
//! @retval true An integer was successfully extracted from the string.
//! @retval false The string could not be interpreted as an integer, or if it
//! could, its value overflowed.
bool String::tryParseScalar(int64_t &scalar, int radix /* = 10 */) const
{
    ScalarParser parser;
    parser.enableSign(true);
    parser.enableExponent(false);
    parser.enableFraction(false);
    parser.enableRadixPrefix(false);
    parser.setPreferredRadix(radix);

    processScalarCharacters(parser, _str->getData());

    return parser.tryGetValue(scalar);
}

//! @brief Attempts to parse a scalar integer from the contents of the string.
//! @param[out] scalar Receives a 64-bit unsigned integer if the leading
//! contents of the string could be interpreted as such.
//! @param[in] radix The radix value used to convert the value.
//! @retval true An integer was successfully extracted from the string.
//! @retval false The string could not be interpreted as an integer, or if it
//! could, its value overflowed.
bool String::tryParseScalar(uint64_t &scalar, int radix /* = 10 */) const
{
    ScalarParser parser;
    parser.enableSign(false);
    parser.enableExponent(false);
    parser.enableFraction(false);
    parser.enableRadixPrefix(false);
    parser.setPreferredRadix(radix);

    processScalarCharacters(parser, _str->getData());

    return parser.tryGetValue(scalar);
}


///////////////////////////////////////////////////////////////////////////////
// Stand Alone Functions
///////////////////////////////////////////////////////////////////////////////
//! @brief An internal function which allows dynamically allocated
//! StringPrivate objects to be removed from the global pool.
//! @param[in] str A pointer to the string object to remove.
void disposeOfDynamicString(StringPrivate *str)
{
    if (str != nullptr)
    {
        getGlobalPool().destroyString(str);
    }
}

//! @brief Attempts to look up the error message associated with a Runtime
//! library error code.
//! @param[in] errorCode The code to look up, perhaps from errno.
//! @return A string containing the message or empty if look-up failed.
String getRuntimeLibraryErrorMessage(int errorCode)
{
    size_t bufferSize = 256;

#ifdef _MSC_VER
    std::vector<wchar_t> buffer;
    buffer.resize(bufferSize);

    if (_wcserror_s(buffer.data(), bufferSize, errorCode) == 0)
    {
        return String::Empty;
    }
    else
    {
        size_t length = std::wcslen(buffer.data());

        while (length == (bufferSize - 1))
        {
            // Make the buffer bigger and try again.
            bufferSize *= 2;
            buffer.resize(bufferSize);

            _wcserror_s(buffer.data(), bufferSize, errorCode);
            length = std::wcslen(buffer.data());
        }

        return String(buffer.data(), length);
    }
#else
    std::vector<char> buffer;
    buffer.resize(bufferSize);
    char *result = strerror_r(errorCode, buffer.data(), bufferSize);

    if (result == nullptr)
    {
        return String::Empty;
    }
    else
    {
        size_t length = strlen(buffer.data());

        while (length == (bufferSize - 1))
        {
            // Make the buffer bigger and try again.
            bufferSize *= 2;
            buffer.resize(bufferSize);

            result = strerror_r(errorCode, buffer.data(), bufferSize);
            length = strlen(buffer.data());
        }

        return String(result);
    }
#endif
}

//! @brief Appends an immutable UTF-8 string to a mutable STL string.
//! @param[in] destination The mutable string to append to, assumed to have
//! a UTF-8 encoding.
//! @param[in] str The immutable string to append.
//! @returns A reference to destination for convenience.
std::string &appendAgString(std::string &destination, string_cref_t &str)
{
    if (str.isEmpty() == false)
    {
        destination.append(str.getUtf8Bytes(), str.getUtf8Length());
    }

    return destination;
}

#ifdef _WIN32
//! @brief Gets the message associated with a Win32 error code.
//! @param[in] errorCode The error code to look up, possibly from the
//! GetLastError() API function.
//! @returns The message associated with the error code or an empty string.
String getWin32ErrorMessage(uint32_t errorCode)
{
    LPWSTR wideMessageBuffer;
    String message;

    uint32_t length = ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                       FORMAT_MESSAGE_FROM_SYSTEM |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                       NULL, errorCode,
                                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                       reinterpret_cast<LPWSTR>(&wideMessageBuffer),
                                       0, NULL);

    if (length > 0)
    {
        message = String(wideMessageBuffer, length);

        ::LocalFree(wideMessageBuffer);
    }

    return message;
}

//! @brief Gets the text from a window title or content, possibly in another process.
//! @param[in] windowHandle The handle of the window to extract text from.
//! @returns A string value containing the text or an empty string on failure.
String getWindowText(uintptr_t windowHandle)
{
    int length = ::GetWindowTextLengthW(reinterpret_cast<HWND>(windowHandle));

    if (length > 0)
    {
        size_t charCount = toSize(length);

        std::vector<wchar_t> buffer;
        buffer.resize(charCount + 1);

        length = ::GetWindowTextW(reinterpret_cast<HWND>(windowHandle),
                                  buffer.data(), length + 1);

        if (length > 0)
        {
            return String(buffer.data(), toSize(length));
        }
    }

    return String::Empty;
}
#endif

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


//! @file Ag/Core/String.hpp
//! @brief The declaration of an immutable UTF-8-encoded string value data type.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_STRING_HPP__
#define __AG_CORE_STRING_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_set>

#include "Configuration.hpp"
#include "String.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
class StringPrivate;
class FormatInfo;
class Variant;

//! @brief An object representing a UTF-8 encoded immutable string.
class String
{
public:
    // Public Types
    //! @brief An iterator data type which allows a caller to iterate through
    //! the Unicode code points of the string by performing implicit conversion.
    class iterator
    {
        friend class String;
    public:
        // Construction
        iterator(const std::shared_ptr<StringPrivate> &source, size_t offset);
        iterator(const iterator &rhs);
        ~iterator() = default;

        // Required Bi-directional Iterator Members

        // Public Types
        //! @brief The standard iterator typedef for the type of object the
        //! iterator dereferences into.
        typedef char32_t value_type;

        //! @brief The standard iterator typedef for the type of reference
        // which exposes the item the iterator represents.
        typedef char32_t &reference;

        //! @brief The standard iterator typedef for the type of pointer
        // which exposes the item the iterator represents.
        typedef  char32_t *pointer;

        //! @brief The standard iterator typedef for the category the iterator
        //! belongs to.
        typedef std::bidirectional_iterator_tag iterator_category;

        //! @brief The standard iterator typedef for the type returned when
        //! one iterator instance is subtracted from another.
        typedef std::ptrdiff_t difference_type;

        // Required Member Functions
        iterator &operator=(const iterator &rhs);
        const char32_t &operator*() const;
        const char32_t *operator->() const;
        iterator &operator++();
        iterator operator++(int);
        iterator &operator--();
        iterator operator--(int);
        bool operator==(const iterator &rhs) const;
        bool operator!=(const iterator &rhs) const;

        // Accessors
        size_t getOffset() const;
    private:
        // Internal Functions
        bool isPastEnd() const;
        void ensureCacheValid() const;
        bool tryAdvance(size_t &nextOffset) const;
        bool tryRetreat(size_t &previousOffset) const;

        // Internal Data
        std::shared_ptr<StringPrivate> _source;
        size_t _offset;
        mutable size_t _cachedEncodedLength;
        mutable char32_t _cachedValue;
        mutable bool _hasCachedValue;
    };

    // Public Data
    //! @brief An empty string value.
    static const String Empty;

    // Constructors/Destructors
    String();
    String(utf8_cptr_t nullTerminatedUtf8);
    String(utf8_cptr_t boundedUtf8, size_t byteCount);
    String(utf16_cptr_t nullTerminatedUtf16);
    String(utf16_cptr_t boundedUtf16, size_t wordCount);
    String(utf32_cptr_t nullTerminatedUtf32);
    String(utf32_cptr_t boundedUtf32, size_t codePointCount);
    String(wchar_cptr_t nullTerminatedWide);
    String(wchar_cptr_t boundedWide, size_t charCount);
    String(const std::string_view &stlUtf8StringView);
    String(const std::string &stlUtf8String);
    String(const std::u16string_view &stlUtf16View);
    String(const std::u16string &stlUtf16String);
    String(const std::u32string_view &stlUtf32View);
    String(const std::u32string &stlUtf32String);
    String(const std::wstring_view &stlWideView);
    String(const std::wstring &stlWideString);

    // Accessors
    bool isEmpty() const;
    size_t getHashCode() const;
    size_t getUtf8Length() const;
    size_t getUtf16Length() const;
    size_t getUtf32Length() const;
    size_t getWideLength() const;
    size_t getPrintLength() const;
    utf8_cptr_t getUtf8Bytes() const;
    long getReferenceCount() const;
    bool contains(utf8_cptr_t rhsUtf8) const;
    bool contains(const std::string_view &rhsUtf8) const;
    bool contains(const String &rhs) const;
    iterator begin() const;
    iterator end() const;
    iterator find(char32_t character) const;
    iterator find(const iterator from, char32_t character) const;
    iterator reverseFind(char32_t character) const;
    iterator reverseFind(const iterator from, char32_t character) const;

    // Operations
    static String fromNativeCodePage(const char *nativeString);
    static String format(utf8_cptr_t spec, const std::initializer_list<Variant> &params);
    static String format(const FormatInfo &format, utf8_cptr_t spec,
                         const std::initializer_list<Variant> &params);
    static String format(const String &spec, const std::initializer_list<Variant> &params);
    static String format(const FormatInfo &format, const String &spec,
                         const std::initializer_list<Variant> &params);
    static String formatUcsNonPrintable(char32_t ch);
    static String formatAsciiNonPrintable(char ch);

    bool operator==(const String &rhs) const;
    bool operator!=(const String &rhs) const;
    bool operator<(const String &rhs) const;
    String operator+(const String &rhs) const;
    String operator=(utf8_cptr_t nullTerminatedUtf8);
    String operator=(utf16_cptr_t nullTerminatedUtf16);
    String operator=(utf32_cptr_t nullTerminatedUtf32);
    String operator=(wchar_cptr_t nullTerminatedWide);
    String operator=(const std::string &stlUtf8String);
    String operator=(const std::u16string &stlUtf16String);
    String operator=(const std::u32string &stlUtf32String);
    String operator=(const std::wstring &wideString);

    String toUpper() const;
    String toLower() const;
    String substring(const iterator &start, const iterator &end) const;
    std::string toUtf8() const;
    std::string_view toUtf8View() const;
    std::u16string toUtf16() const;
    std::u32string toUtf32() const;
    std::wstring toWide() const;
    int compare(const String &rhs) const;
    int compareIgnoreCase(const String &rhs) const;
    bool startsWith(const std::string_view &prefix) const;
    bool tryParseScalar(int32_t &scalar, int radix = 10) const;
    bool tryParseScalar(uint32_t &scalar, int radix = 10) const;
    bool tryParseScalar(int64_t &scalar, int radix = 10) const;
    bool tryParseScalar(uint64_t &scalar, int radix = 10) const;
private:
    // Internal Fields
    std::shared_ptr<StringPrivate> _str;
};

//! @brief An alias type used to receive a string value from a function parameter.
using string_ref_t = String &;

//! @brief An alias type used to pass a string value as a function parameter.
using string_cref_t = const String &;

//! @brief An alias for an STL vector of String values.
using StringCollection = std::vector<String>;

} // namespace Ag

//! @brief Contains specialisations of templates define in the STL
//! home namespace to allow Ag objects to integrate with STL collections
//! and algorithms.
namespace std {

//! @brief A specialisation of the std::hash<> template which makes Ag::String
//! values compatible with STL hash-based data structures.
template<> struct hash<Ag::String>
{
    //! @brief Obtains the hash of the contents of a string value.
    //! @param[in] rhs The string value to get a hash code for.
    //! @return The cached hash code of the string.
    size_t operator()(const Ag::String &rhs) const
    {
        return rhs.getHashCode();
    }
};

//! @brief A specialisation of the std::less<> template which makes Ag::String
//! values compatible with STL sorting and searching algorithms.
template<> struct less<Ag::String>
{
    //! @brief Compares the value of two Ag::Strings.
    //! @param[in] lhs The left string to compare.
    //! @param[in] rhs The right string to compare.
    //! @retval true The lhs string has a lower value than rhs.
    //! @retval false The lhs string has an equal or higher value than rhs.
    bool operator()(const Ag::String &lhs, const Ag::String &rhs) const
    {
        return lhs.compare(rhs) < 0;
    }
};

} // namespace std.

namespace Ag {

//! @brief An alias for a hash set of strings.
using StringSet = std::unordered_set<String>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
String getRuntimeLibraryErrorMessage(int errorCode);
std::string &appendAgString(std::string &destination, string_cref_t &str);

#ifdef _WIN32
String getWin32ErrorMessage(uint32_t errorCode);
String getWindowText(uintptr_t windowHandle);
#endif

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

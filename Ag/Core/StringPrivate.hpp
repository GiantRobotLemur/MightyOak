//! @file Core/StringPrivate.hpp
//! @brief The declaration of the inner object of the Ag library string value type.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_STRING_PRIVATE_HPP__
#define __AG_CORE_STRING_PRIVATE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <string>

#include "Ag/Core/Configuration.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An internal object representing an immutable UTF-8 encoded Unicode
//! string.
class StringPrivate : public std::enable_shared_from_this<StringPrivate>
{
public:
    // Constructors/Destructors
    StringPrivate();
    StringPrivate(const StringPrivate &key);
    StringPrivate(StringPrivate &&rhs) = delete;
    StringPrivate(utf8_cptr_t nullTerminatedUtf8);
    StringPrivate(utf8_cptr_t boundedUtf8, size_t byteCount);
    StringPrivate(utf16_cptr_t nullTerminatedUtf16);
    StringPrivate(utf16_cptr_t boundedUtf16, size_t wordCount);
    StringPrivate(utf32_cptr_t nullTerminatedUtf32);
    StringPrivate(utf32_cptr_t boundedUtf32, size_t codePointCount);
    StringPrivate(wchar_cptr_t nullTerminatedWide);
    StringPrivate(wchar_cptr_t boundedWide, size_t charCount);
    ~StringPrivate();

    // Accessors
    bool isEmpty() const;
    const std::string &getData() const;
    size_t getHashCode() const;
    size_t getUTF8Length() const;
    size_t getUTF16Length() const;
    size_t getUTF32Length() const;

    // Operations
    bool operator==(const StringPrivate &rhs) const;
    StringPrivate &operator=(const StringPrivate &rhs) = delete;
    StringPrivate &operator=(StringPrivate &&rhs) = delete;
private:
    // Internal Functions
    void initialiseUTF8(utf8_cptr_t boundedUtf8, size_t byteCount);
    void initialiseUTF16(utf16_cptr_t boundedUtf16, size_t wordCount);
    void initialiseUTF32(utf32_cptr_t boundedUtf32, size_t codePointCount);

    // Internal Fields
    size_t _hashCode;
    size_t _utf8Length;
    size_t _utf16Length;
    size_t _utf32Length;
    utf8_cptr_t _data;
    std::string _localData;
    bool _isDynamic;
};

///////////////////////////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////////////////////////
void disposeOfDynamicString(StringPrivate *str);

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file QtInterop/Conversion.cpp
//! @brief The definition of various functions to convert betweem Ag and Qt
//! data types.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/QtInterop/Conversion.hpp"

#include "Ag/Core/Utf.hpp"

namespace Ag {
namespace Qt {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void appendUtf8(QString &target, const std::string_view &utf8Text)
{
    Utf::FromUtf8Converter fromUtf8;
    Utf::ToUtf16Converter toUtf16;

    size_t length = utf8Text.length();
    uint8_cptr_t bytes = reinterpret_cast<uint8_cptr_t>(utf8Text.data());
    char32_t codePoint;
    bool hasError = false;

    for (size_t i = 0; i < length; ++i)
    {
        if (fromUtf8.tryConvert(bytes[i], codePoint, hasError))
        {
            // We have a whole code point.
            toUtf16.setCodePoint(codePoint);

            char16_t next;
            while (toUtf16.tryGetNextCharacter(next))
            {
                target.append(QChar(next));
            }
        }
        else if (hasError)
        {
            fromUtf8.reset();
        }
    }
}


} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Converts an immutable UTF-8 Ag string into a UTF-16 QString.
//! @param[in] agString The Ag string to convert.
//! @returns The converted QString.
QString toQString(Ag::string_cref_t agString)
{
    QString buffer;

    if (agString.isEmpty() == false)
    {
        buffer.reserve(agString.getUtf16Length());

        Utf::FromUtf8Converter fromUtf8;
        Utf::ToUtf16Converter toUtf16;

        size_t length = agString.getUtf8Length();
        uint8_cptr_t bytes = reinterpret_cast<uint8_cptr_t>(agString.getUtf8Bytes());
        char32_t codePoint;
        bool hasError = false;

        for (size_t i = 0; i < length; ++i)
        {
            if (fromUtf8.tryConvert(bytes[i], codePoint, hasError))
            {
                // We have a whole code point.
                toUtf16.setCodePoint(codePoint);

                char16_t next;
                while (toUtf16.tryGetNextCharacter(next))
                {
                    buffer.append(QChar(next));
                }
            }
            else if (hasError)
            {
                fromUtf8.reset();
            }
        }
    }

    return buffer;
}

//! @brief Appends UTF-8 encoded characters to a QString.
//! @param[in] target The QString to append to.
//! @param[in] utf8Text A view of the UTF-8-encoded bytes to append.
void appendUtf8ToQString(QString &target, const std::string_view &utf8Text)
{
    size_t utf16Length = Ag::Utf::calculateConvertedLength(utf8Text, Utf::Utf16);

    if (utf16Length > 0)
    {
        // Ensure the target string has enough capacity without having
        // to re-allocate over and over again.
        qsizetype requiredCapacity = target.length() + utf16Length;

        if (target.capacity() < requiredCapacity)
        {
            target.reserve(requiredCapacity);
        }

        appendUtf8(target, utf8Text);
    }
}

//! @brief Appends UTF-8 encoded characters to a QString.
//! @param[in] target The QString to append to.
//! @param[in] utf8Text An immutable UTF-8 string to append.
void appendUtf8ToQString(QString &target, Ag::string_cref_t utf8Text)
{
    if (utf8Text.isEmpty() == false)
    {
        qsizetype requiredCapacity = target.length() + utf8Text.getUtf16Length();

        if (target.capacity() < requiredCapacity)
        {
            target.reserve(requiredCapacity);
        }

        appendUtf8(target, utf8Text.toUtf8View());
    }
}

//! @brief Converts an immutable Ag file path into a UTF-16 QString.
//! @param[in] path The Ag file path value to convert.
//! @returns The converted QString.
QString toQString(const Ag::Fs::Path &path)
{
    String utf8Text = path.toString(Fs::PathUsage::Display);

    return toQString(utf8Text);
}

//! @brief Converts a UTF-16-based QString to an immutable UTF-8-based
//! Ag string.
//! @param[in] qString The Qt string to convert.
//! @return The Qt string as an immutable UTF-8 string.
String fromQString(const QStringView &qString)
{
    String result;

    if (qString.isEmpty() == false)
    {
        auto utf8Buffer = qString.toUtf8();

        result = String(utf8Buffer.constData(), utf8Buffer.size());
    }

    return result;
}

//! @brief Attempts to convert a QString to an immutable Ag file path.
//! @param[in] qString The QString to convert.
//! @return The Ag Path, populated if qString was valid, otherwise in an
//! empty state.
Ag::Fs::Path pathFromQString(const QStringView &qString)
{
    Ag::Fs::Path result;

    if (qString.isEmpty() == false)
    {
        String utf8Text = fromQString(qString);
        Fs::Path parsedPath;

        if (Fs::Path::tryParse(utf8Text, parsedPath))
        {
            result = std::move(parsedPath);
        }
    }

    return result;
}

}} // namespace Ag::Qt
////////////////////////////////////////////////////////////////////////////////


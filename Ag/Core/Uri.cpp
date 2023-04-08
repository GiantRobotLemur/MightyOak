//! @file Core/Uri.cpp
//! @brief The definition of  classes to manage Universal Resource Identifiers.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iterator>
#include <string_view>

#include "Ag/Core/Format.hpp"
#include "Ag/Core/Variant.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/Uri.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief Represents a classified run of UTF-8 encoded characters in a URI
//! being parsed.
struct CharacterRun
{
    size_t Start;
    size_t Length;

    CharacterRun() :
        Start(0),
        Length(0)
    {
    }

    CharacterRun(size_t start, size_t length) :
        Start(start),
        Length(length)
    {
    }

    ~CharacterRun() = default;

    bool isValid() const { return Length > 0; }

    std::string_view get(string_cref_t source) const
    {
        return std::string_view(source.getUtf8Bytes() + Start, Length);
    }

    void markEnd(size_t endOffset)
    {
        Length = (endOffset >= Start) ? endOffset - Start : 0u;
    }

    void clear()
    {
        Start = Length = 0;
    }
};

//! @brief References the components of a URI within its source string.
struct UriStructure
{
    CharacterRun Scheme;
    CharacterRun UserInfo;
    CharacterRun Host;
    CharacterRun Path;
    CharacterRun Query;
    CharacterRun Fragment;
    int Port;

    UriStructure() :
        Port(-1)
    {
    }

    void clear()
    {
        Scheme.clear();
        UserInfo.clear();
        Host.clear();
        Path.clear();
        Query.clear();
        Fragment.clear();
        Port = -1;
    }
};

template<typename T>
struct GenerateScalars
{
    T _next;

    GenerateScalars(T first) :
        _next(first)
    {
    }

    T operator()()
    {
        return _next++;
    }
};

using AsciiCharSet = std::vector<char>;
using AsciiCharSetRef = const AsciiCharSet &;

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Adds a range of characters to a set.
//! @param[in] charSet The set to add to.
//! @param[in] first The first character of the range to add.
//! @param[in] last The last character of the range to add.
template<typename T>
void appendRange(std::vector<T> &charSet, T first, T last)
{
    auto rangePair = std::minmax(first, last);
    size_t count = static_cast<size_t>(rangePair.second - rangePair.first) + 1;

    std::generate_n(std::back_inserter(charSet), count,
                    GenerateScalars<T>(rangePair.first));
}

void appendSet(AsciiCharSet &target, AsciiCharSetRef source)
{
    std::copy(source.begin(), source.end(),
              std::back_inserter(target));
}

void compileSet(AsciiCharSet &charSet)
{
    std::sort(charSet.begin(), charSet.end());
    auto last = std::unique(charSet.begin(), charSet.end());
    charSet.erase(last, charSet.end());

    // Try to zero-terminate the collection to aid debugging.
    charSet.push_back(0);
    charSet.shrink_to_fit();
    charSet.pop_back();
}

//! @brief Determines if a character belongs in a scheme.
//! @param[in] ch The Unicode character to query.
//! @retval true If the character is valid in a scheme identifier.
//! @retval false If the character is not valid in a scheme.
bool isSchemeCharacter(char32_t ch)
{
    static std::vector<char32_t> charSet;

    if (charSet.empty())
    {
        appendRange(charSet, U'A', U'Z');
        appendRange(charSet, U'a', U'z');
        appendRange(charSet, U'0', U'9');
        charSet.push_back(U'-');
        charSet.push_back(U'.');
        charSet.push_back(U'+');
        charSet.shrink_to_fit();

        std::sort(charSet.begin(), charSet.end());
    }

    return std::binary_search(charSet.begin(), charSet.end(), ch);
}

//! @brief Determines if a character is a valid hexadecimal digit.
//! @param[in] ch The Unicode character to query.
//! @retval true If the character is a valid digit.
//! @retval false If the character is not valid digit.
bool isHexDigit(char32_t ch)
{
    return ((ch >= U'0') && (ch <= U'9')) ||
        ((ch >= U'A') && (ch <= U'F')) ||
        ((ch >= U'a') && (ch <= U'f'));
}


//! @brief Determines if a character is a valid hexadecimal digit.
//! @param[in] ch The Unicode character to query.
//! @retval true If the character is a valid digit.
//! @retval false If the character is not valid digit.
bool isAsciiHexDigit(char32_t ch)
{
    return ((ch >= '0') && (ch <= '9')) ||
        ((ch >= 'A') && (ch <= 'F')) ||
        ((ch >= 'a') && (ch <= 'f'));
}

//! @brief Converts an ASCII character representing a hexadecimal digit into
//! a scalar integer.
//! @param[in] hexChar The character to convert.
//! @return The scalar the character represents, or 0.
uint8_t hexCharToScalar(char hexChar)
{
    uint8_t value = 0;

    if ((hexChar >= '0') && (hexChar <= '9'))
    {
        value = static_cast<uint8_t>(hexChar - '0');
    }
    else if ((hexChar >= 'A') && (hexChar <= 'F'))
    {
        value = static_cast<uint8_t>(hexChar - 'A') + 10;
    }
    else if ((hexChar >= 'a') && (hexChar <= 'f'))
    {
        value = static_cast<uint8_t>(hexChar - 'a') + 10;
    }

    return value;
}

//! @brief Attempts to recognised the components of a URI within a source string.
//! @param[in] source The URI text to analyse.
//! @param[out] components The structure to receive descriptions of the various
//! components of the URI.
//! @param[out] error Receives a description of why the URI is invalid if the
//! parsing process fails.
//! @retval true The source string represented a valid URI and components was
//! filled in with details of its components.
//! @retval false The source string is an invalid URI, error is updated to
//! describe why.
bool tryParseUri(const string_cref_t source, UriStructure &components,
                 string_ref_t error)
{
    components.clear();
    error = String::Empty;

    enum class State
    {
        Start,
        SchemeOrPath,
        LeadingPath,
        AfterFirstSlash,
        AfterScheme,
        PostSchemeSlash,
        BeforeAuthority,
        AuthUserInfo,
        BeforeAuthHost,
        AuthHost,
        BeforeAuthPort,
        AuthPort,
        Path,

        Query,
        InQuery,
        Fragment,
        InFragment,

        BeforeEscaped,
        InEscaped,
        Error,
    };

    State state = State::Start;
    State oldState = State::Start;

    for (auto pos = source.begin(); pos != source.end(); ++pos)
    {
        char32_t next = *pos;

        switch (state)
        {
        case State::Start:
            // Could be:
            //  Path-only char  -> rootless path
            //  Scheme/Path char
            //  Escape Sequence -> rootless path?
            //  Slash -> rooted path or authority
            //  Query
            //  Fragment
            if (next == U':')
            {
                error = "The ':' character is invalid at the beginning of a URI.";
                state = State::Error;
            }
            else if (isSchemeCharacter(next))
            {
                state = State::SchemeOrPath;
            }
            else if (next == U'%')
            {
                oldState = State::LeadingPath;
                state = State::BeforeEscaped;
            }
            else if (next == U'/')
            {
                components.Path.Start = pos.getOffset();
                state = State::AfterFirstSlash;
            }
            else if (next == U'?')
            {
                state = State::Query;
            }
            else if (next == U'#')
            {
                state = State::Fragment;
            }
            else // if (isPathNotSchemeCharacter(next))
            {
                // Assume anything else is part of a path element.
                state = State::LeadingPath;
            }
            break;

        case State::SchemeOrPath:
            // Could be:
            //  Colon -> end of scheme
            //  Scheme/Path char
            //  Path-only char
            //  Scheme-only char
            //  Escape sequence
            //  Slash -> rootless path
            //  Query
            //  Fragment
            if (next == U':')
            {
                // It's the end of the scheme.
                components.Scheme.Length = pos.getOffset();
                state = State::AfterScheme;
            }
            else if (next == U'/')
            {
                // We've found the end of the leading path element.
                state = State::Path;
            }
            else if (next == U'%')
            {
                // An escaped character indicates a path.
                oldState = State::LeadingPath;
                state = State::BeforeEscaped;
            }
            else if (next == U'?')
            {
                // Assume it was a rootless path element.
                components.Path.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a rootless path element.
                components.Path.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            else if (!isSchemeCharacter(next))
            {
                // Assume anything else is part of a path element.
                state = State::LeadingPath;
            }
            break;

        case State::LeadingPath:
            // Could be:
            //  Path-char - no colons
            //  Escape sequence
            //  Slash -> next path element
            //  Query
            //  Fragment
            if (next == U':')
            {
                // It's the end of the scheme.
                error = "A rootless path element cannot contain a ':' character.";
                state = State::Error;
            }
            else if (next == U'/')
            {
                // We've found the end of the leading path element.
                state = State::Path;
            }
            else if (next == U'%')
            {
                // An escaped character indicates a path.
                oldState = State::LeadingPath;
                state = State::BeforeEscaped;
            }
            else if (next == U'?')
            {
                // Assume it was a rootless path element.
                components.Path.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a rootless path element.
                components.Path.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            // else continue accumulating path characters.
            break;

        case State::AfterFirstSlash:
            // Could be:
            //  Slash -> Authority
            //  Path-char -> rooted path
            //  Escape sequence ->rooted path
            //  Query
            //  Fragment
            if (next == U'/')
            {
                state = State::BeforeAuthority;
            }
            else if (next == U'%')
            {
                // An escaped character indicates a path.
                oldState = State::Path;
                state = State::BeforeEscaped;
            }
            else if (next == U'?')
            {
                // Assume it was a rooted path element.
                components.Path.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a rooted path element.
                components.Path.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            else // a path character?
            {
                // Assume its a rooted path element.
                state = State::Path;
            }
            break;

        case State::AfterScheme:
            // Could be:
            //  Slash -> Authority or path
            //  Path char -> rootless path
            //  Escape sequence - rootless path
            //  Query
            //  Fragment
            if (next == U'/')
            {
                // Mark the start of the path and the authority.
                size_t offset = pos.getOffset();
                components.Path.Start = offset;
                state = State::PostSchemeSlash;
            }
            else if (next == U'%')
            {
                // An escaped character indicates a path.
                oldState = State::Path;
                state = State::BeforeEscaped;
            }
            else if (next == U'?')
            {
                // Assume it was a rooted path element.
                components.Path.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a rooted path element.
                components.Path.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            else // a path character?
            {
                // Assume its a rooted path element.
                state = State::Path;
            }
            break;

        case State::PostSchemeSlash:
            // Could be:
            //  Slash -> Authority
            //  Path char -> A rooted path
            //  Escape sequence - rootless path
            //  Query
            //  Fragment
            if (next == U'/')
            {
                // Mark the start of the path.
                state = State::BeforeAuthority;
            }
            else if (next == U'%')
            {
                // An escaped character indicates a path.
                oldState = State::Path;
                state = State::BeforeEscaped;
            }
            else if (next == U'?')
            {
                // Assume it was a rooted path element.
                components.Path.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a rooted path element.
                components.Path.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            else // a path character?
            {
                // Assume its a rooted path element.
                state = State::Path;
            }
            break;

        case State::BeforeAuthority:
            // Could be:
            //  Authority char -> user-info or host.
            //  Slash -> end of authority
            //  Escape sequence
            //  Query
            //  Fragment
            if (next == U'/')
            {
                // A third slash, the authority is empty, we're starting a path.
                //components.Host.markEnd(pos.getOffset());
                components.Path.Start = pos.getOffset();
                state = State::Path;
            }
            else if (next == U'%')
            {
                // An escaped character goes in the authority.
                components.UserInfo.Start = components.Host.Start = pos.getOffset();
                oldState = State::AuthUserInfo;
                state = State::BeforeEscaped;
            }
            else if (next == U'?')
            {
                // Assume it was a rooted path element.
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a rooted path element.
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            else if (next == U'@')
            {
                // We have an empty user info field.
                components.UserInfo.markEnd(pos.getOffset());
                state = State::BeforeAuthHost;
            }
            else if (next == U':')
            {
                // You cannot specify a host without a port.
                error = "Empty host specification.";
                state = State::Error;
            }
            else
            {
                // Assume it's the first character of the user name or host.
                components.UserInfo.Start = components.Host.Start = pos.getOffset();
                state = State::AuthUserInfo;
            }
            break;

        case State::AuthUserInfo:
            // Could be:
            //  Authority char -> user-info or host.
            //  Slash -> end of authority, making this the host.
            //  At -> end of user info
            //  Colon -> end of host, start of port.
            //  Escape sequence
            //  Query
            //  Fragment
            if (next == U'/')
            {
                // The authority is complete, the previous characters
                // must have been a host name/specification.
                components.Host.markEnd(pos.getOffset());
                components.Path.Start = pos.getOffset();
                state = State::Path;
            }
            else if (next == U'%')
            {
                // An escaped character goes in the authority.
                oldState = state;
                state = State::BeforeEscaped;
            }
            else if (next == U':')
            {
                components.Host.markEnd(pos.getOffset());
                state = State::BeforeAuthPort;
            }
            else if (next == U'?')
            {
                // Assume it was a rooted path element.
                components.Host.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a rooted path element.
                components.Host.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            else if (next == U'@')
            {
                // We finished user info field, host next.
                components.UserInfo.markEnd(pos.getOffset());
                state = State::BeforeAuthHost;
            }
            // else It's a character of the user-info or host name.
            break;

        case State::BeforeAuthHost:
            // Could be:
            //  Host char.
            //  Slash -> end of host name, start of path.
            //  Colon -> end of host, start of port.
            //  Escape sequence -> host char.
            //  Query
            //  Fragment
            if (next == U'/')
            {
                // The authority is complete, the previous characters
                // must have been a host name/specification.
                components.Host.markEnd(pos.getOffset());
                components.Path.Start = pos.getOffset();
                state = State::Path;
            }
            else if (next == U'%')
            {
                // An escaped character goes in the host.
                oldState = State::AuthHost;
                state = State::BeforeEscaped;
            }
            else if (next == U':')
            {
                // Assume it was a host name.
                components.Host.markEnd(pos.getOffset());
                state = State::BeforeAuthPort;
            }
            else if (next == U'?')
            {
                // Assume it was a host name.
                components.Host.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a host name.
                components.Host.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            else
            {
                // It's the first character of the host name.
                components.Host.Start = pos.getOffset();
                state = State::AuthHost;
            }
            break;

        case State::AuthHost:
            // Could be:
            //  Host char.
            //  Slash -> end of host name, start of path
            //  Colon -> end of host, start of port.
            //  Escape sequence -> host char.
            //  Query
            //  Fragment
            if (next == U'/')
            {
                // The authority is complete, the previous characters
                // must have been a host name/specification.
                components.Host.markEnd(pos.getOffset());
                components.Path.Start = pos.getOffset();
                state = State::Path;
            }
            else if (next == U'%')
            {
                // An escaped character goes in the authority.
                oldState = state;
                state = State::BeforeEscaped;
            }
            else if (next == U':')
            {
                // Assume it was a host name.
                components.Host.markEnd(pos.getOffset());
                state = State::BeforeAuthPort;
            }
            else if (next == U'?')
            {
                // Assume it was a host name.
                components.Host.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a host name.
                components.Host.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            // else It's a host character.
            break;

        case State::BeforeAuthPort:
            // Could be:
            //  Port digit char.
            //  Slash -> empty port?, start of path
            //  Escape sequence -> invalid in port spec.
            //  Query
            //  Fragment
            if ((next == U'/') || (next == U'?') || (next == U'#'))
            {
                error = "The port specification was empty.";
                state = State::Error;
            }
            else if (next == U'%')
            {
                error = "Escape sequences are invalid in a port specification.";
                state = State::Error;
            }
            else if ((next >= U'0') && (next <= U'9'))
            {
                // It's the first character of the port number.
                components.Port = static_cast<uint32_t>(next - U'0');
                state = State::AuthPort;
            }
            else
            {
                error = String::format("Unexpected character '{0}' in port specification.",
                                       { String::formatUcsNonPrintable(next) });
                state = State::Error;
            }
            break;

        case State::AuthPort:
            // Could be:
            //  Port digit char.
            //  Slash -> end of port, start of path
            //  Escape sequence -> invalid in port spec.
            //  Query
            //  Fragment
            if (next == U'%')
            {
                error = "Escape sequences are invalid in a port specification.";
                state = State::Error;
            }
            else if ((next >= U'0') && (next <= U'9'))
            {
                // It's the first character of the port number.
                components.Port *= 10;
                components.Port += static_cast<uint32_t>(next - U'0');
            }
            else if (next == U'/')
            {
                // The authority is complete, the path begins here.
                components.Path.Start = pos.getOffset();
                state = State::Path;
            }
            else if (next == U'?')
            {
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            else
            {
                error = String::format("Unexpected character '{0}' in port specification.",
                                       { String::formatUcsNonPrintable(next) });
                state = State::Error;
            }
            break;

        case State::Path:
            // Could be:
            //  Path char
            //  Escape sequence
            //  Query
            //  Fragment
            if (next == U'%')
            {
                oldState = State::Path;
                state = State::BeforeEscaped;
            }
            else if (next == U'?')
            {
                // Assume it was a rootless path element.
                components.Path.markEnd(pos.getOffset());
                components.Query.Start = pos.getOffset();
                state = State::Query;
            }
            else if (next == U'#')
            {
                // Assume it was a rootless path element.
                components.Path.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            // else continue accumulating path characters.
            break;

        case State::Query:
            // Could be:
            //  Query char
            //  Escape sequence
            //  Fragment
            components.Query.Start = pos.getOffset();

            if (next == U'%')
            {
                oldState = State::InQuery;
                state = State::BeforeEscaped;
            }
            else if (next == U'#')
            {
                // Assume it was a rootless path element.
                state = State::Fragment;
            }
            else
            {
                // It's the first character of the query.
                state = State::InQuery;
            }
            break;

        case State::InQuery:
            // Could be:
            //  Query char
            //  Escape sequence
            //  Fragment
            if (next == U'%')
            {
                oldState = state;
                state = State::BeforeEscaped;
            }
            else if (next == U'#')
            {
                // Assume it was a rootless path element.
                components.Query.markEnd(pos.getOffset());
                components.Fragment.Start = pos.getOffset();
                state = State::Fragment;
            }
            // else continue accumulating query characters
            break;

        case State::Fragment:
            // Could be:
            //  Fragment char
            //  Escape sequence
            components.Fragment.Start = pos.getOffset();

            if (next == U'%')
            {
                oldState = State::InFragment;
                state = State::BeforeEscaped;
            }
            else
            {
                // It's the first character of the fragment.
                state = State::InFragment;
            }
            break;

        case State::InFragment:
            // Could be:
            //  Fragment char
            //  Escape sequence
            if (next == U'%')
            {
                oldState = state;
                state = State::BeforeEscaped;
            }
            // else continue accumulating fragment characters
            break;

        case State::BeforeEscaped:
            if (isHexDigit(next))
            {
                state = State::InEscaped;
            }
            else
            {
                error = String::format("Unexpected character '{0}' in escape sequence.",
                                       { String::formatUcsNonPrintable(next) });
                state = State::Error;
            }
            break;

        case State::InEscaped:
            if (isHexDigit(next))
            {
                // Restore the old state now that we have successfully parsed a
                // valid escape sequence.
                state = oldState;
            }
            else
            {
                error = String::format("Unexpected character '{0}' in escape sequence.",
                                       { String::formatUcsNonPrintable(next) });
                state = State::Error;
            }
            break;

        case State::Error:
            if (error.isEmpty())
            {
                auto prevPos = pos;
                --prevPos;

                error = String::format("Unexpected character '{0}' encountered.",
                                       { String::formatUcsNonPrintable(*prevPos) });
            }

            pos = source.end();
            break;
        }
    }

    // Use the final state to determine if the URI was valid and to capture
    // the last component.
    bool isValid = true;

    switch (state)
    {
    case State::Start:
        error = "Empty URI.";
        isValid = false;
        break;

    case State::AuthUserInfo:
    case State::AuthHost:
        components.Host.markEnd(source.getUtf8Length());
        break;

    case State::BeforeAuthHost:
        error = "Empty host component.";
        isValid = false;
        break;

    case State::BeforeAuthPort:
        error = "Empty port specification.";
        isValid = false;
        break;

    case State::AuthPort:
        break;

    case State::BeforeAuthority:
        error = "Authority component empty.";
        isValid = false;
        break;

    case State::SchemeOrPath:
    case State::LeadingPath:
    case State::AfterFirstSlash:
    case State::PostSchemeSlash:
    case State::Path:
        components.Path.markEnd(source.getUtf8Length());
        break;

    case State::AfterScheme:
    case State::Query:      // Finished in an empty query.
    case State::Fragment:   // Finished in an empty fragment.
        break;

    case State::InQuery:
        components.Query.markEnd(source.getUtf8Length());
        break;

    case State::InFragment:
        components.Fragment.markEnd(source.getUtf8Length());
        break;

    case State::BeforeEscaped:
    case State::InEscaped:
        error = "Incomplete escape sequence at the end of URI.";
        isValid = false;
        break;

    case State::Error:
    default:
        isValid = false;
        break;
    }

    return isValid;
}


//! @brief Gets the sorted set of unreserved URI ASCII characters.
AsciiCharSetRef getUnreservedCharSet()
{
    static AsciiCharSet allowed;

    if (allowed.empty())
    {
        appendRange(allowed, 'A', 'Z');
        appendRange(allowed, 'a', 'z');
        appendRange(allowed, '0', '9');
        allowed.push_back('-');
        allowed.push_back('.');
        allowed.push_back('_');
        allowed.push_back('~');
        allowed.shrink_to_fit();

        std::sort(allowed.begin(), allowed.end());
    }

    return allowed;
}

//! @brief Gets the sorted set of sub-delimiter URI ASCII characters.
AsciiCharSetRef getSubDelimitersCharSet()
{
    static AsciiCharSet allowed;

    if (allowed.empty())
    {
        allowed.push_back('!');
        allowed.push_back('$');
        allowed.push_back('&');
        allowed.push_back('\'');
        allowed.push_back('(');
        allowed.push_back(')');
        allowed.push_back('*');
        allowed.push_back('+');
        allowed.push_back(',');
        allowed.push_back(';');
        allowed.push_back('=');
        allowed.shrink_to_fit();

        std::sort(allowed.begin(), allowed.end());
    }

    return allowed;
}

//! @brief Gets the sorted set of allowed user info ASCII characters.
AsciiCharSetRef getUserInfoCharSet()
{
    static AsciiCharSet allowed;

    if (allowed.empty())
    {
        appendSet(allowed, getUnreservedCharSet());
        appendSet(allowed, getSubDelimitersCharSet());
        allowed.push_back('%');

        compileSet(allowed);
    }

    return allowed;
}

//! @brief Gets the sorted set of allowed host ASCII characters.
AsciiCharSetRef getHostCharSet()
{
    static AsciiCharSet allowed;

    if (allowed.empty())
    {
        appendSet(allowed, getUnreservedCharSet());
        appendSet(allowed, getSubDelimitersCharSet());
        allowed.push_back('%');

        // TODO: Properly deal with IPv6/IPvFuture addresses.
        allowed.push_back('[');
        allowed.push_back(']');

        compileSet(allowed);
    }

    return allowed;
}

//! @brief Gets the sorted set of allowed URI path element ASCII characters.
AsciiCharSetRef getPathCharSet()
{
    static AsciiCharSet allowed;

    if (allowed.empty())
    {
        appendSet(allowed, getUnreservedCharSet());
        appendSet(allowed, getSubDelimitersCharSet());
        allowed.push_back('%');
        allowed.push_back('@');

        compileSet(allowed);
    }

    return allowed;
}


//! @brief Gets the sorted set of allowed URI path ASCII characters.
AsciiCharSetRef getFullPathCharSet()
{
    static AsciiCharSet allowed;

    if (allowed.empty())
    {
        appendSet(allowed, getPathCharSet());
        allowed.push_back('/');

        compileSet(allowed);
    }

    return allowed;
}

//! @brief Gets the sorted set of allowed URI query parameter or fragment
//! ASCII characters.
AsciiCharSetRef getQueryFragmentCharSet()
{
    static AsciiCharSet allowed;

    if (allowed.empty())
    {
        appendSet(allowed, getPathCharSet());
        allowed.push_back('%');
        allowed.push_back(':');
        allowed.push_back('@');
        allowed.push_back('/');
        allowed.push_back('?');

        compileSet(allowed);
    }

    return allowed;
}

//! @brief Gets the sorted set of allowed URI query specification or fragment
//! ASCII characters.
AsciiCharSetRef getFullQueryFragmentCharSet()
{
    static AsciiCharSet allowed;

    if (allowed.empty())
    {
        appendSet(allowed, getQueryFragmentCharSet());
        allowed.push_back('&');

        compileSet(allowed);
    }

    return allowed;
}

void appendEscaped(std::string &buffer, char ch)
{
    static const char digits[] = "0123456789ABCDEF";

    uint8_t value = static_cast<uint8_t>(ch);

    buffer.push_back('%');
    buffer.push_back(digits[value >> 4]);
    buffer.push_back(digits[value & 0x0F]);
}

void appendComponent(std::string &buffer, UriUsage usage,
                     AsciiCharSetRef allowedChars,
                     const std::string_view &source)
{
    switch (usage)
    {
    case UriUsage::Escaped:
        // Escape bytes which are invalid.
        for (char next : source)
        {
            if (std::binary_search(allowedChars.begin(), allowedChars.end(), next))
            {
                buffer.push_back(next);
            }
            else
            {
                appendEscaped(buffer, next);
            }
        }
        break;

    case UriUsage::Display:
        // Translate escape sequences into actual bytes.
        for (size_t i = 0, c = source.length(); i < c; ++i)
        {
            char next = source[i];

            if (next == '%')
            {
                size_t bytesLeft = c - i;

                if ((bytesLeft > 2) &&
                    isAsciiHexDigit(source[i + 1]) &&
                    isAsciiHexDigit(source[i + 2]))
                {
                    uint8_t value = (hexCharToScalar(source[i + 1]) << 4) |
                                    hexCharToScalar(source[i + 2]);

                    buffer.push_back(static_cast<char>(value));

                    // Skip the escaped hex digits.
                    i += 2;
                }
            }
            else
            {
                buffer.push_back(next);
            }
        }
        break;

    case UriUsage::AsSpecified:
    default:
        buffer.append(source.data(), source.length());
        break;
    }
}

//! @brief Processes an immutable UTF-8 string to escape any characters
//! which are not valid.
//! @param[in] allowedChars A set defining valid characters.
//! @param[in] source The string of characters to escape.
//! @return The escaped string, possibly identical to the original.
String escapeString(AsciiCharSetRef allowedChars, string_cref_t source)
{
    size_t i, c;
    utf8_cptr_t sourceChars = source.getUtf8Bytes();

    for (i = 0, c = source.getUtf8Length(); i < c; ++i)
    {
        if (std::binary_search(allowedChars.begin(), allowedChars.end(),
                               sourceChars[i]) == false)
        {
            break;
        }
    }

    if (i < c)
    {
        // We found at least one character in need of escaping.
        size_t charsLeft = c - i;
        std::string buffer;
        buffer.reserve(i + (charsLeft * 2));
        buffer.assign(sourceChars, i);

        appendComponent(buffer, UriUsage::Escaped, allowedChars,
                        std::string_view(sourceChars + i, charsLeft));

        return String(buffer);
    }
    else
    {
        // No modification required.
        return source;
    }
}

//! @brief Decodes any escape sequences in a string to produce a new value.
//! @param[in] source The URI component string to decode.
//! @return The un-escaped string, possibly identical to the original.
String unescapeString(string_cref_t source)
{
    std::string_view view = source.toUtf8View();
    size_t next = view.find('%');

    if (next == std::string_view::npos)
    {
        // There are no escape sequences, return the original value.
        return source;
    }
    else
    {
        std::string buffer;
        buffer.reserve(view.length());
        size_t start = 0;

        do
        {
            if (next == std::string_view::npos)
            {
                next = view.length();
            }

            // Append any characters up to the next escape sequence.
            buffer.append(view.data() + start, next - start);

            size_t charsLeft = view.length() - next;

            if ((charsLeft > 2) &&
                isAsciiHexDigit(view[next + 1]) &&
                isAsciiHexDigit(view[next + 2]))
            {
                // It's a valid escape sequence, decode it.
                uint8_t value = hexCharToScalar(view[next + 1]);
                value <<= 4;
                value |= hexCharToScalar(view[next + 2]);

                buffer.push_back(static_cast<char>(value));
                start = next + 3;
            }
            else if (charsLeft > 0)
            {
                // The escape sequence is invalid, append it as-is.
                buffer.push_back('%');
                start = next + 1;
            }
            else
            {
                start = view.length();
            }

            // Look for the next escape sequence.
            next = view.find('%', start);
        } while (start < view.length());

        return String(buffer);
    }
}

} // Anonymous namespace


////////////////////////////////////////////////////////////////////////////////
// UriQueryParam Member Definitions
////////////////////////////////////////////////////////////////////////////////
UriQueryParam::UriQueryParam(string_cref_t value) : Value(value) {}

UriQueryParam::UriQueryParam(string_cref_t name, string_cref_t value) :
    Name(name),
    Value(value)
{
}

////////////////////////////////////////////////////////////////////////////////
// UriParseException Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an exception describing a failure to parse a URI.
//! @param[in] uriText The text of the URI the program attempted to parse,
//! possibly blank.
//! @param[in] error Text describing why the parsing process failed.
UriParseException::UriParseException(string_cref_t &uriText, string_cref_t error)
{
    static const std::string_view messagePrefix = "Failed to parse URI";
    static const std::string_view domain = "UriParseException";

    if (uriText.isEmpty())
    {
        initialise(domain, messagePrefix, error.toUtf8View());
    }
    else
    {
        std::string message;
        message.reserve(messagePrefix.length() + uriText.getUtf8Length() + 4);
        message.assign(messagePrefix);
        message.push_back(' ');
        message.push_back('\'');
        message.append(uriText.getUtf8Bytes());
        message.push_back('\'');
        message.push_back('.');

        initialise(domain, message, error.toUtf8View());
    }
}

////////////////////////////////////////////////////////////////////////////////
// UriBuilder Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to build URIs which starts in an empty state.
UriBuilder::UriBuilder() :
    _port(-1),
    _isRooted(false)
{
}

//! @brief Constructs an object to build URIs initialised by parsing a URI string.
//! @param[in] uriText The text of the URI to parse. Unlike other calls to the
//! parse member functions, this text can be empty.
//! @throws UriParseException If the text represented an invalid URI.
UriBuilder::UriBuilder(string_cref_t &uriText) :
    _port(-1),
    _isRooted(false)
{
    if (uriText.isEmpty() == false)
    {
        parse(uriText);
    }
}

//! @brief Determines if the object is in an empty state.
//! @retval true The toString() member function will return an empty string.
//! @retval false The toString() member function will return at least one
//! component of a URI.
bool UriBuilder::isEmpty() const
{
    return _scheme.isEmpty() &&
        _host.isEmpty() &&
        _path.empty() &&
        _queryParams.empty() &&
        _fragment.isEmpty();
}

//! @brief Gets the scheme portion of the URI.
string_cref_t UriBuilder::getScheme() const { return _scheme; }

//! @brief Sets the scheme portion of the URI.
//! @param[in] scheme The new scheme, possibly empty.
void UriBuilder::setScheme(const string_cref_t scheme) { _scheme = scheme; }

//! @brief Gets the user information embedded in the authority component.
string_cref_t UriBuilder::getUserInfo() const { return _userInfo; }

//! @brief Sets the user information embedded in the authority component.
//! @param[in] userInfo The new user information, possibly empty.
void UriBuilder::setUserInfo(const string_cref_t userInfo) { _userInfo = userInfo; }

//! @brief Sets the user information field to an empty state.
void UriBuilder::clearUserInfo() { _userInfo = String::Empty; }

//! @brief Gets the specification of the host.
string_cref_t UriBuilder::getHost() const { return _host; }

//! @brief Sets the specification of the host.
//! @param[in] host The new host name or address, possibly empty.
void UriBuilder::setHost(const string_cref_t host) { _host = host; }

//! @brief Sets the host specification to an empty state.
void UriBuilder::clearHost() { _host = String::Empty; }

//! @brief Indicates if a port number has been assigned.
//! @retval true A port number has been assigned.
//! @retval false No port number has been assigned, calls to getPort() return
//! a meaningless result.
bool UriBuilder::hasPort() const { return (_port >= 0) && (_port <= 0xFFFF); }

//! @brief Gets the optional port number associated with the host.
//! @returns The port number or 0 if none was specified.
uint16_t UriBuilder::getPort() const
{
    return ((_port < 0) || (_port > 0xFFFF)) ? 0 : static_cast<uint16_t>(_port);
}

//! @brief Sets the port number associated with the host.
//! @param[in] port The new port number.
void UriBuilder::setPort(uint16_t port) { _port = port; }

//! @brief Sets the port number field to an empty state.
void UriBuilder::clearPort() { _port = -1; }

//! @brief Gets whether the path component is rooted rather than relative.
//! @retval true The path is rooted and is expressed with a proceeding slash.
//! @retval false The path is relative, there is no proceeding slash.
bool UriBuilder::isRootedPath() const { return _isRooted; }

//! @brief Sets whether the path component is rooted.
//! @param[in] rooted True if the path is rooted, false if it is relative.
void UriBuilder::setRootedPath(bool rooted) { _isRooted = rooted; }

//! @brief Gets a reference to the collection of elements which defines the
//! path component.
StringCollection &UriBuilder::getPathElements() { return _path; }

//! @brief Gets a read-only reference to the collection of elements which
//! defines the path component.
const StringCollection &UriBuilder::getPathElements() const { return _path; }

//! @brief Gets a reference to the collection defining the query parameters.
UriParamCollection &UriBuilder::getQueryParameters() { return _queryParams; }

//! @brief Gets a read-only reference to the collection defining the query parameters.
const UriParamCollection &UriBuilder::getQueryParameters() const { return _queryParams; }

//! @brief Gets the optional fragment component of the URI.
string_cref_t UriBuilder::getFragment() const { return _fragment; }

//! @brief Sets the optional fragment component of the URI.
//! @param[in] fragment The new fragment component.
void UriBuilder::setFragment(const string_cref_t fragment) { _fragment = fragment; }

//! @brief Sets the optional fragment component of the URI to an empty state.
void UriBuilder::clearFragment() { _fragment = String::Empty; }

//! @brief Expresses the URI as a single string.
//! @param[in] usage Expresses how the URI is to be used.
//! @returns A string representation of the URI.
String UriBuilder::toString(UriUsage usage /*= UriUsage::AsSpecified*/) const
{
    std::string buffer;

    if (_scheme.isEmpty() == false)
    {
        appendAgString(buffer, _scheme);
        buffer.push_back(':');
    }

    if (_host.isEmpty() == false)
    {
        buffer.append(2, '/');

        if (_userInfo.isEmpty() == false)
        {
            appendComponent(buffer, usage, getUserInfoCharSet(),
                            _userInfo.toUtf8View());

            buffer.push_back('@');
        }

        appendComponent(buffer, usage, getHostCharSet(),
                        _host.toUtf8View());

        if (hasPort())
        {
            FormatInfo format(LocaleInfo::getNeutral());
            buffer.push_back(':');
            appendValue(format, buffer, _port);
        }
    }

    if (_isRooted)
    {
        buffer.push_back('/');
    }

    bool isFirst = true;

    for (string_cref_t element : _path)
    {
        if (isFirst)
        {
            isFirst = false;
        }
        else
        {
            buffer.push_back('/');
        }

        appendComponent(buffer, usage, getPathCharSet(),
                        element.toUtf8View());
    }

    if (_queryParams.empty() == false)
    {
        isFirst = true;
        buffer.push_back('?');

        for (const UriQueryParam &param : _queryParams)
        {
            if (param.Name.isEmpty() && param.Value.isEmpty())
                continue;

            if (isFirst)
            {
                isFirst = false;
            }
            else
            {
                buffer.push_back('&');
            }

            if (param.Name.isEmpty())
            {
                // It's just a value.
                appendComponent(buffer, usage,
                                getQueryFragmentCharSet(),
                                param.Value.toUtf8View());
            }
            else if (param.Value.isEmpty())
            {
                // Just output the name.
                appendComponent(buffer, usage,
                                getQueryFragmentCharSet(),
                                param.Name.toUtf8View());
            }
            else
            {
                // It's a key=value pair.
                appendComponent(buffer, usage,
                                getQueryFragmentCharSet(),
                                param.Name.toUtf8View());

                buffer.push_back('=');

                appendComponent(buffer, usage,
                                getQueryFragmentCharSet(),
                                param.Value.toUtf8View());
            }
        }
    }

    if (_fragment.isEmpty() == false)
    {
        buffer.push_back('#');

        appendComponent(buffer, usage,
                        getQueryFragmentCharSet(),
                        _fragment.toUtf8View());
    }

    return String(buffer);
}

//! @brief Sets all components of the URI to an empty state.
void UriBuilder::clear()
{
    _scheme = String::Empty;
    _userInfo = String::Empty;
    _host = String::Empty;
    _port = -1;
    _path.clear();
    _queryParams.clear();
    _fragment = String::Empty;
}

//! @brief Populates the object by parsing the contents of a UTF-8 string.
//! @param[in] source The URI expressed as text.
//! @param[in] source The UTF-8 encoded string to parse.
//! @throws UriParseException If the source text represented an invalid URI.
void UriBuilder::parse(string_cref_t source)
{
    String error;

    if (tryParse(source, error) == false)
    {
        throw UriParseException(source, error);
    }
}

//! @brief Attempts to populate the object by parsing the contents of
//! a UTF-8 string.
//! @param[in] source The UTF-8 encoded string to parse.
//! @retval true The text represented a valid URI and the object was
//! populated from it.
//! @retval false The text was not a valid URI, the object is unmodified.
bool UriBuilder::tryParse(string_cref_t source)
{
    String error;

    return tryParse(source, error);
}

//! @brief Attempts to populate the object by parsing the contents of
//! a UTF-8 string.
//! @param[in] source The UTF-8 encoded string to parse.
//! @param[out] error Updated with text describing why the string was not a
//! valid URI.
//! @retval true The text represented a valid URI and the object was
//! populated from it.
//! @retval false The text was not a valid URI, the object is unmodified,
//! but the error text should be populated with an error message.
bool UriBuilder::tryParse(string_cref_t source, string_ref_t error)
{
    UriStructure components;
    bool isOK = false;

    if (tryParseUri(source, components, error))
    {
        // Extract the components from the original string.
        _scheme = components.Scheme.get(source);
        _userInfo = components.UserInfo.get(source);
        _host = components.Host.get(source);
        _port = components.Port;
        std::string_view path = components.Path.get(source);
        std::string_view query = components.Query.get(source);
        _fragment = components.Fragment.get(source);

        _path.clear();
        _queryParams.clear();

        if (path.empty())
        {
            _isRooted = false;
        }
        else
        {
            // Split the path into its consistent elements, consolidating multiple
            // path element separators.
            _isRooted = (path.front() == '/');
            bool isInSeparator = _isRooted;
            size_t start = 0;

            for (size_t i = 0, c = path.length(); i <= c; ++i)
            {
                // Add a separator to the end to force the last path element
                // to be flushed.
                char next = (i == c) ? '/' : path[i];

                if (next == '/')
                {
                    if (isInSeparator == false)
                    {
                        // Flush the previous item.
                        _path.emplace_back(path.data() + start, i - start);
                        isInSeparator = true;
                    }
                    // else - It's another separator, no change.
                }
                else if (isInSeparator)
                {
                    // The first character of a new path element.
                    isInSeparator = false;
                    start = i;
                }
                // else - It's another path character, no change.
            }
        }

        if (query.empty() == false)
        {
            // Split by '&' then '=' character.
            size_t start = 0;
            size_t nextEnd = query.find('&');

            do
            {
                if (nextEnd == std::string_view::npos)
                {
                    nextEnd = query.length();
                }

                // Create a view of the specific parameter.
                std::string_view paramView(query.data() + start, nextEnd - start);
                size_t equalPos = paramView.find('=');

                if (equalPos == std::string_view::npos)
                {
                    // It's just a value.
                    _queryParams.emplace_back(paramView);
                }
                else
                {
                    // It's a name value pair.
                    String key(paramView.data(), equalPos);
                    String value(paramView.data() + equalPos + 1,
                                 paramView.length() - equalPos - 1);

                    _queryParams.emplace_back(key, value);
                }

                // Search for the next parameter break.
                start = nextEnd + 1;
                nextEnd = query.find('&', start);

            } while (start < query.length());
        }

        isOK = true;
    }

    return isOK;
}

//! @brief Converts all components to an escaped form where necessary.
void UriBuilder::escape()
{
    // NOTE: Escape sequences aren't allowed in the scheme.
    _userInfo = escapeString(getUserInfoCharSet(), _userInfo);

    // TODO: Properly deal with IPv6/IPvFuture addresses.
    _host = escapeString(getHostCharSet(), _host);

    for (string_ref_t pathElement : _path)
    {
        pathElement = escapeString(getPathCharSet(), pathElement);
    }

    for (UriQueryParam &param : _queryParams)
    {
        param.Name = escapeString(getQueryFragmentCharSet(), param.Name);
        param.Value = escapeString(getQueryFragmentCharSet(), param.Value);
    }

    _fragment = escapeString(getQueryFragmentCharSet(), _fragment);
}

//! @brief Converts all escape sequences in the URI components to Unicode
//! characters.
void UriBuilder::unescape()
{
    _userInfo = unescapeString(_userInfo);
    _host = unescapeString(_host);

    for (string_ref_t pathElement : _path)
    {
        pathElement = unescapeString(pathElement);
    }

    for (UriQueryParam &param : _queryParams)
    {
        param.Name = unescapeString(param.Name);
        param.Value = unescapeString(param.Value);
    }

    _fragment = unescapeString(_fragment);
}

////////////////////////////////////////////////////////////////////////////////
// Uri Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a copy of a URI, possibly converted to a different usage.
//! @param[in] rhs The URI to copy.
//! @param[in] usage The usage of the URI to construct, possibly performing
//! a conversion from rhs.
Uri::Uri(const Uri &rhs, UriUsage usage) :
    _port(rhs._port),
    _usage(usage)
{
    if (rhs.isEmpty())
    {
        // Copy an empty URI with no other semantics.
        _port = -1;
        _usage = UriUsage::AsSpecified;
    }
    else if ((usage == UriUsage::AsSpecified) || (usage == rhs._usage))
    {
        // No conversion is required, simply copy the data.
        //
        // NOTE: Copies of the string_views are still valid because they refer
        // to the same immutable UTF-8 string.
        _source = rhs._source;
        _scheme = rhs._scheme;
        _userInfo = rhs._userInfo;
        _host = rhs._host;
        _port = rhs._port;
        _path = rhs._path;
        _queryParams = rhs._queryParams;
        _fragment = rhs._fragment;
    }
    else
    {
        // Construct a URI string with converted usage.
        std::string buffer;
        buffer.reserve(rhs._source.getUtf8Length() * 3 / 2);

        UriStructure components;
        if (rhs._scheme.empty() == false)
        {
            buffer.append(rhs._scheme);
            components.Scheme.markEnd(buffer.size());
            buffer.push_back(':');
        }

        if (rhs._host.empty() == false)
        {
            buffer.append(2, '/');

            if (rhs.getUserInfo().isEmpty() == false)
            {
                components.UserInfo.Start = buffer.size();
                appendComponent(buffer, usage, getUserInfoCharSet(),
                                rhs._userInfo);
                components.UserInfo.markEnd(buffer.size());
                buffer.push_back('@');
            }

            components.Host.Start = buffer.size();
            appendComponent(buffer, usage, getHostCharSet(),
                            rhs._host);
            components.Host.markEnd(buffer.size());

            if (rhs.hasPort())
            {
                buffer.push_back(':');
                FormatInfo format(LocaleInfo::getNeutral());
                appendValue(format, buffer, rhs._port);
            }
        }

        if (rhs._path.empty() == false)
        {
            components.Path.Start = buffer.size();
            appendComponent(buffer, usage, getFullPathCharSet(), rhs._path);
            components.Path.markEnd(buffer.size());
        }

        if (rhs._queryParams.empty() == false)
        {
            buffer.push_back('?');
            components.Query.Start = buffer.size();
            appendComponent(buffer, usage, getFullQueryFragmentCharSet(),
                            rhs._queryParams);
            components.Query.markEnd(buffer.size());
        }

        if (rhs._fragment.empty() == false)
        {
            buffer.push_back('#');
            components.Fragment.Start = buffer.size();
            appendComponent(buffer, usage, getFullQueryFragmentCharSet(),
                            rhs._fragment);
            components.Fragment.markEnd(buffer.size());
        }

        // Construct the immutable UTF-8 string which view of components
        // should reference
        _source = buffer;
        _usage = usage;

        // Resolve references.
        _scheme = components.Scheme.get(_source);
        _userInfo = components.UserInfo.get(_source);
        _host = components.Host.get(_source);
        _path = components.Path.get(_source);
        _queryParams = components.Query.get(_source);
        _fragment = components.Fragment.get(_source);
    }
}

//! @brief Constructs an immutable URI object in an empty state.
Uri::Uri() :
    _port(-1),
    _usage(UriUsage::AsSpecified)
{
}

//! @brief Constructs an immutable URI object by parsing a string.
//! @param[in] source The string encoding the URI.
//! @param[in] usage The preferred usage of the URI once constructed.
//! @throws UriParseException Thrown if source represents an invalid URI.
Uri::Uri(string_cref_t source, UriUsage usage /*= UriUsage::AsSpecified*/) :
    _port(-1),
    _usage(UriUsage::AsSpecified)
{
    String error;

    if (tryParse(source, error, usage))
    {
        throw UriParseException(source, error);
    }
}

//! @brief Constructs an immutable URI object from a mutable URI object.
//! @param[in] builder The mutable URI object containing the components
//! to combine.
//! @param[in] usage The preferred usage of the URI once constructed.
Uri::Uri(const UriBuilder &builder,
         UriUsage usage /*= UriUsage::AsSpecified*/) :
    _port(-1),
    _usage(UriUsage::AsSpecified)
{
    if (builder.isEmpty())
    {
        // No sense populating any fields
        return;
    }

    // Create URI text with appropriate offsets.
    std::string buffer;
    UriStructure components;

    if (builder.getScheme().isEmpty() == false)
    {
        appendAgString(buffer, builder.getScheme());
        components.Scheme.markEnd(buffer.size());
        buffer.push_back(':');
    }

    if (builder.getHost().isEmpty() == false)
    {
        buffer.append(2, '/');

        if (builder.getUserInfo().isEmpty() == false)
        {
            components.UserInfo.Start = buffer.size();
            appendComponent(buffer, usage, getUserInfoCharSet(),
                            builder.getUserInfo().toUtf8View());
            components.UserInfo.markEnd(buffer.size());
            buffer.push_back('@');
        }

        components.Host.Start = buffer.size();
        appendComponent(buffer, usage, getHostCharSet(),
                        builder.getHost().toUtf8View());
        components.Host.markEnd(buffer.size());

        if (builder.hasPort())
        {
            _port = builder.getPort();
            buffer.push_back(':');
            FormatInfo format(LocaleInfo::getNeutral());
            appendValue(format, buffer, builder.getPort());
        }
    }

    if (builder.getPathElements().empty() == false)
    {
        components.Path.Start = buffer.size();

        if (builder.isRootedPath())
        {
            buffer.push_back('/');
        }

        bool isFirst = true;

        for (string_cref_t element : builder.getPathElements())
        {
            if (isFirst)
            {
                isFirst = false;
            }
            else
            {
                buffer.push_back('/');
            }

            appendComponent(buffer, usage, getPathCharSet(),
                            element.toUtf8View());
        }

        components.Path.markEnd(buffer.size());
    }

    if (builder.getQueryParameters().empty() == false)
    {
        buffer.push_back('?');
        components.Query.Start = buffer.size();

        bool isFirst = true;

        for (const UriQueryParam &param : builder.getQueryParameters())
        {
            if (param.Name.isEmpty() && param.Value.isEmpty())
                continue;

            if (isFirst)
            {
                isFirst = false;
            }
            else
            {
                buffer.push_back('&');
            }

            if (param.Name.isEmpty())
            {
                // Its just a value.
                appendComponent(buffer, usage, getQueryFragmentCharSet(),
                                param.Value.toUtf8View());
            }
            else if (param.Value.isEmpty())
            {
                // It's just a name.
                appendComponent(buffer, usage, getQueryFragmentCharSet(),
                                param.Name.toUtf8View());
            }
            else
            {
                // It's a name=value pair.
                appendComponent(buffer, usage, getQueryFragmentCharSet(),
                                param.Name.toUtf8View());
                buffer.push_back('=');
                appendComponent(buffer, usage, getQueryFragmentCharSet(),
                                param.Value.toUtf8View());
            }
        }

        components.Query.markEnd(buffer.size());
    }

    if (components.Fragment.isValid())
    {
        buffer.push_back('#');
        components.Fragment.Start = buffer.size();
        appendComponent(buffer, usage, getFullQueryFragmentCharSet(),
                        builder.getFragment().toUtf8View());
        components.Fragment.markEnd(buffer.size());
    }

    // Construct the immutable UTF-8 string which view of components
    // should reference
    _source = buffer;
    _usage = usage;

    // Resolve references.
    _scheme = components.Scheme.get(_source);
    _userInfo = components.UserInfo.get(_source);
    _host = components.Host.get(_source);
    _path = components.Path.get(_source);
    _queryParams = components.Query.get(_source);
    _fragment = components.Fragment.get(_source);
}

//! @brief Determines whether the URI is in an empty state.
//! @retval true The toString() member function would produce an empty string.
//! @retval false The toString() member function would produce at least one
//! URI component.
bool Uri::isEmpty() const { return _source.isEmpty(); }

//! @brief Gets the scheme component of the URI, if defined.
//! @note No usage is specified because the scheme component cannot be escaped.
String Uri::getScheme() const
{
    return String(_scheme);
}

//! @brief Gets the user info component in the specified usage, possibly blank.
//! @param[in] usage The usage to which the component should be converted
//! before being returned.
String Uri::getUserInfo(UriUsage usage /*= UriUsage::AsSpecified*/) const
{
    return getComponent(_userInfo, usage, getUserInfoCharSet());
}

//! @brief Gets the host component in the specified usage, possibly blank.
//! @param[in] usage The usage to which the component should be converted
//! before being returned.
String Uri::getHost(UriUsage usage /*= UriUsage::AsSpecified*/) const
{
    return getComponent(_host, usage, getHostCharSet());
}

//! @brief Indicates if a port number has been assigned.
//! @retval true A port number has been assigned.
//! @retval false No port number has been assigned, calls to getPort() return
//! a meaningless result.
bool Uri::hasPort() const { return (_port >= 0) && (_port <= 0xFFFF); }

//! @brief Gets the port number defined in the authority component or 0.
uint16_t Uri::getPort() const
{
    return ((_port >= 0) && (_port <= 0xFFFF)) ? static_cast<uint16_t>(_port) : 0u;
}

//! @brief Gets whether the path component is rooted rather than relative.
//! @retval true The path is rooted and is expressed with a proceeding slash.
//! @retval false The path is relative, there is no proceeding slash.
bool Uri::isRootedPath() const
{
    return (_path.empty() == false) && (_path.front() == '/');
}

//! @brief Gets the path component of the URI as a single string, if any
//! elements were defined.
//! @param[in] usage The usage to which the component should be converted
//! before being returned.
String Uri::getPathElements(UriUsage usage /*= UriUsage::AsSpecified*/) const
{
    return getComponent(_path, usage, getFullPathCharSet());
}

//! @brief Gets the query parameters of the URI as a single string, if any
//! were defined.
//! @param[in] usage The usage to which the component should be converted
//! before being returned.
String Uri::getQueryParameters(UriUsage usage /*= UriUsage::AsSpecified*/) const
{
    return getComponent(_queryParams, usage, getFullQueryFragmentCharSet());
}

//! @brief Gets the fragment component of the URI, if defined.
//! @param[in] usage The usage to which the component should be converted
//! before being returned.
String Uri::getFragment(UriUsage usage /*= UriUsage::AsSpecified*/) const
{
    return getComponent(_fragment, usage, getFullQueryFragmentCharSet());
}

//! @brief Gets the entire URI as a single immutable UTF-8 string.
//! @param[in] usage The usage to which the URI should be converted
//! before being returned.
String Uri::toString(UriUsage usage /*= UriUsage::AsSpecified*/) const
{
    if ((usage == UriUsage::AsSpecified) ||
        (usage == _usage) ||
        _source.isEmpty())
    {
        return _source;
    }
    else
    {
        // Re-construct the entire URI according to the required usage.
        std::string buffer;
        buffer.reserve(_source.getUtf8Length() * 3 / 2);

        if (_scheme.empty() == false)
        {
            appendAgString(buffer, _scheme);
            buffer.push_back(':');
        }

        if (_host.empty() == false)
        {
            buffer.append(2, '/');

            if (_userInfo.empty() == false)
            {
                appendComponent(buffer, usage, getUserInfoCharSet(), _userInfo);
                buffer.push_back('@');
            }

            appendComponent(buffer, usage, getHostCharSet(), _host);

            if (hasPort())
            {
                buffer.push_back(':');

                FormatInfo format(LocaleInfo::getNeutral());
                appendValue(format, buffer, _port);
            }
        }

        if (_path.empty() == false)
        {
            appendComponent(buffer, usage, getFullPathCharSet(), _path);
        }

        if (_queryParams.empty() == false)
        {
            buffer.push_back('?');
            appendComponent(buffer, usage, getFullQueryFragmentCharSet(),
                            _queryParams);
        }

        if (_fragment.empty() == false)
        {
            buffer.push_back('#');
            appendComponent(buffer, usage, getFullQueryFragmentCharSet(),
                            _fragment);
        }

        return String(buffer);
    }
}

//! @brief Resets the URI object to an empty state.
void Uri::clear()
{
    _scheme = _userInfo = _host = _path = _queryParams = _fragment = std::string_view();
    _source = String::Empty;
    _port = -1;
    _usage = UriUsage::AsSpecified;
}

//! @brief Initialises the URI object by parsing a string.
//! @param[in] source The string encoding the URI.
//! @param[in] usage The preferred usage of the URI once parsed.
//! @throws UriParseException Thrown if source represents an invalid URI.
void Uri::parse(string_cref_t source, UriUsage usage /*= UriUsage::AsSpecified*/)
{
    String error;

    if (tryParse(source, error, usage) == false)
    {
        throw UriParseException(source, error);
    }
}

//! @brief Attempts to initialise the URI object by parsing a string.
//! @param[in] source The string encoding the URI.
//! @param[in] usage The preferred usage of the URI once parsed.
//! @retval true The string was a valid URI and the object has been populated.
//! @retval false The string did not represent a valid URI, the object is
//! unmodified.
bool Uri::tryParse(string_cref_t source,
                   UriUsage usage /*= UriUsage::AsSpecified*/)
{
    String error;

    return tryParse(source, error, usage);
}


//! @brief Attempts to initialise the URI object by parsing a string.
//! @param[in] source The string encoding the URI.
//! @param[in] usage The preferred usage of the URI once parsed.
//! @param[out] error Receives a message describing why the URI text
//! was invalid.
//! @retval true The string was a valid URI and the object has been populated.
//! @retval false The string did not represent a valid URI, the object is
//! unmodified.
bool Uri::tryParse(string_cref_t source, string_ref_t error,
                   UriUsage usage /*= UriUsage::AsSpecified*/)
{
    UriStructure components;
    bool isOK = false;

    if (tryParseUri(source, components, error))
    {
        std::string buffer;
        buffer.reserve(source.getUtf8Length() * 3 / 2);

        // Store the positions of the components in their preferred usage
        // in the source URI string we construct.
        UriStructure usageComponents;

        if (components.Scheme.isValid())
        {
            buffer.append(components.Scheme.get(source));
            usageComponents.Scheme.markEnd(buffer.size());
            buffer.push_back(':');
        }

        if (components.Host.isValid())
        {
            buffer.append(2, '/');

            if (components.UserInfo.isValid())
            {
                usageComponents.UserInfo.Start = buffer.size();
                appendComponent(buffer, usage, getUserInfoCharSet(),
                                components.UserInfo.get(source));
                usageComponents.UserInfo.markEnd(buffer.size());
                buffer.push_back('@');
            }

            usageComponents.Host.Start = buffer.size();
            appendComponent(buffer, usage, getHostCharSet(),
                            components.Host.get(source));
            usageComponents.Host.markEnd(buffer.size());

            if ((components.Port >= 0) && (components.Port <= 0xFFFF))
            {
                buffer.push_back(':');
                FormatInfo format(LocaleInfo::getNeutral());
                appendValue(format, buffer, components.Port);
            }
        }

        if (components.Path.isValid())
        {
            usageComponents.Path.Start = buffer.size();

            std::string_view pathView = components.Path.get(source);
            size_t i = 0;
            bool isInSeparator = false;
            bool needsSeparator = false;

            if (pathView.front() == '/')
            {
                isInSeparator = true;
                needsSeparator = true;
                ++i;
            }

            for (size_t start = i; i <= pathView.length(); ++i)
            {
                char next = (i == pathView.length()) ? '/' : pathView[i];

                if (isInSeparator)
                {
                    if (next != '/')
                    {
                        // It's the start of a new path element.
                        start = i;
                        isInSeparator = false;
                    }
                }
                else if (next == '/')
                {
                    // Its the separator at the end of a path element.
                    isInSeparator = true;

                    if (needsSeparator)
                    {
                        buffer.push_back('/');
                    }
                    else
                    {
                        // Ensure the next element is proceeded by a
                        // separator.
                        needsSeparator = true;
                    }

                    // Append and reformat the path element.
                    appendComponent(buffer, usage, getPathCharSet(),
                                    pathView.substr(start, i - start));
                }
            }

            usageComponents.Path.markEnd(buffer.size());
        }

        if (components.Query.isValid())
        {
            buffer.push_back('?');
            usageComponents.Query.Start = buffer.size();
            appendComponent(buffer, usage, getFullQueryFragmentCharSet(),
                            components.Query.get(source));
            usageComponents.Query.markEnd(buffer.size());
        }

        if (components.Fragment.isValid())
        {
            buffer.push_back('#');
            usageComponents.Fragment.Start = buffer.size();
            appendComponent(buffer, usage, getFullQueryFragmentCharSet(),
                            components.Fragment.get(source));
            usageComponents.Fragment.markEnd(buffer.size());
        }

        // Construct the immutable UTF-8 string which view of components
        // should reference
        _source = buffer;
        _usage = usage;

        // Resolve references.
        _scheme = usageComponents.Scheme.get(_source);
        _userInfo = usageComponents.UserInfo.get(_source);
        _host = usageComponents.Host.get(_source);
        _port = components.Port;
        _path = usageComponents.Path.get(_source);
        _queryParams = usageComponents.Query.get(_source);
        _fragment = usageComponents.Fragment.get(_source);
        isOK = true;
    }

    return isOK;
}

//! @brief Returns a copy of the URI where all invalid characters are escaped.
Uri Uri::escaped() const
{
    return Uri(*this, UriUsage::Escaped);
}

//! @brief Returns a copy of the URI where all escape sequences have been
//! translated into characters.
Uri Uri::unescaped() const
{
    return Uri(*this, UriUsage::Display);
}

//! @brief Gets a string representing a component of the URI.
//! @param[in] segment The view of the component UTF-8 characters.
//! @param[in] usage The preferred format of the component.
//! @param[in] allowedChars The sorted set of characters allowed in the
//! component being extracted when escaped.
//! @return A string representing the component formatted for the desired usage.
String Uri::getComponent(const std::string_view &segment, UriUsage usage,
                         const std::vector<char> &allowedChars) const
{
    if (segment.empty())
    {
        return String::Empty;
    }
    else if ((usage == UriUsage::AsSpecified) || (usage == _usage))
    {
        // Return the segment in its original state.
        return String(segment);
    }
    else
    {
        std::string buffer;
        buffer.reserve(segment.length() * 3 / 2);

        appendComponent(buffer, usage, allowedChars, segment);

        return String(buffer);
    }
}




////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


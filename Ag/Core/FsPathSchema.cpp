//! @file Core/FsPathSchema.cpp
//! @brief The definition of an object which abstracts the differences between
//! file path formats.
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

#include "Ag/Core/String.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Variant.hpp"

#include "FsPathSchema.hpp"

#ifdef _WIN32
#include "Win32API.hpp"
#else
#include "PosixAPI.hpp"
#endif

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Fs {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
class Win32PathSchema : public PathSchema
{
private:
    std::vector<char32_t> _invalidChars;

    // Internal Functions
public:
    Win32PathSchema() :
        _invalidChars({ U'<', U'>', U':', U'"', U'/', U'\\', U'|', U'?', U'*' })
    {
        // Index the set of invalid path element characters.
        std::sort(_invalidChars.begin(), _invalidChars.end());
    }

    virtual ~Win32PathSchema() = default;

    // Overrides
    virtual bool isCaseSensitive() const override { return false; }
    virtual char getPathSeparator() const override { return ';'; }
    virtual wchar_t getWidePathSeparator() const override { return L';'; }
    virtual char getElementSeparator() const override { return '\\'; }
    virtual wchar_t getWideElementSeparator() const override { return L'\\'; }

    virtual bool isValidElementSeparator(char32_t character) const override
    {
        return (character == U'\\') || (character == U'/');
    }

    virtual bool isValidElementCharacter(char32_t ch) const override
    {
        bool isValid = false;

        if (ch > 31)
        {
            isValid = std::binary_search(_invalidChars.begin(),
                                         _invalidChars.end(),
                                         ch) == false;
        }

        return isValid;
    }

    virtual bool tryParsePathRoot(String::iterator &pos,
                                  const String::iterator &end,
                                  std::u32string &root, string_ref_t error,
                                  PathRootType &rootType) const override
    {
        // Trying to parse one of the following:
        // \\?\UNC\Host\Volume\  (Win32 File Namespace - UNC form)
        // \\?\C:\               (Win32 File Namespace - DOS drive form)
        // \\Host\Volume\        (UNC name)
        // C:\                   (DOS drive name)
        // \                     (Root of current drive)
        // The trailing slash is optional in all but current drive.
        enum class State
        {
            Start,
            AfterFirstSlash,
            AfterSecondSlash,
            AfterWin32FileQuestionMark,
            AfterWin32FileThirdSlash,
            AfterWin32FileCurrentDrive,
            AfterWin32FileDriveLetter,
            AfterWin32FileU,
            AfterWin32FileN,
            AfterWin32FileC,
            AfterFirstUncSlash,
            AfterFirstHostCharacter,
            AfterHostSlash,
            AfterFirstVolumeCharacter,
            AfterDriveLetter,
            AfterDriveColon,
            AfterTailingSlash,
            Complete,
        };

        bool hasRoot = false;
        bool keepParsing = true;
        bool hasTrailingSeparator = false;
        rootType = PathRootType::None;
        error = String::Empty;
        State state = State::Start;
        root.clear();
        root.reserve(32);
        auto current = pos;

        while (keepParsing && (current != end))
        {
            char32_t next = *current;

            switch (state)
            {
            case State::Start:
                if (isValidElementSeparator(next))
                {
                    state = State::AfterFirstSlash;
                    ++current;
                }
                else if (((next >= U'A') && (next <= 'Z')) ||
                         ((next >= U'a') && (next <= 'z')))
                {
                    // Looks like a drive letter, but could be a relative path.
                    state = State::AfterDriveLetter;
                    root.push_back(next);
                    ++current;
                }
                else if (isValidElementCharacter(next))
                {
                    // It looks like a relative path.
                    keepParsing = false;
                    current = pos;
                }
                else
                {
                    // Something weird.
                    error = String::format("Unexpected path character '{0}'.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterFirstSlash:
                if (isValidElementSeparator(next))
                {
                    state = State::AfterSecondSlash;
                    ++current;
                }
                else
                {
                    // The leading slash denotes the current drive.
                    root.push_back(U'\\');
                    rootType = PathRootType::CurrentDrive;
                    hasTrailingSeparator = true;
                    pos = current;
                    keepParsing = false;
                    hasRoot = true;
                }
                break;

            case State::AfterSecondSlash:
                if (next == U'?')
                {
                    // It's a Win32 file namespace path.
                    state = State::AfterWin32FileQuestionMark;
                    ++current;
                }
                else if (isValidElementCharacter(next))
                {
                    // It looks like the host portion of a UNC name.
                    state = State::AfterFirstHostCharacter;
                    root.push_back(U'\\');
                    root.push_back(U'\\');
                    root.push_back(next);
                    ++current;
                }
                else if (isValidElementSeparator(next))
                {
                    // Skip further superfluous slashes.
                    ++current;
                }
                else
                {
                    error = String::format("Unexpected character '{0}' "
                                           "in path root.", { next });
                    keepParsing = false;
                }
                break;

            case State::AfterWin32FileQuestionMark:
                if (isValidElementSeparator(next))
                {
                    state = State::AfterWin32FileThirdSlash;
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' in "
                                           "Win32 File System namespace prefix.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterWin32FileThirdSlash:
                if (next == U'U')
                {
                    // It looks like \\?\UNC\... or \\?\U:\

                    state = State::AfterWin32FileU;
                    ++current;
                }
                else if (((next >= U'A') && (next <= 'Z')) ||
                         ((next >= U'a') && (next <= 'z')))
                {
                    // Looks like a drive letter, but could also be the first path
                    // element on the current drive.
                    state = State::AfterWin32FileDriveLetter;
                    root.push_back(next);
                    pos = current;
                    ++current;
                }
                else if (isValidElementSeparator(next))
                {
                    // It could be a Win32 Namespace path which is relative to the
                    // current drive. I.e.: \\?\ followed by \Dir\File.ext
                    state = State::AfterWin32FileCurrentDrive;
                    rootType = PathRootType::CurrentDrive;
                    root.push_back(U'\\');
                    hasTrailingSeparator = true;
                    hasRoot = true;
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' after "
                                           "Win32 File System namespace prefix.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterWin32FileCurrentDrive:
                if (isValidElementCharacter(next))
                {
                    // We've found the start of the first path element.
                    pos = current;
                    keepParsing = false;
                }
                else if (isValidElementSeparator(next))
                {
                    // Eat any further separators.
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' after "
                                           "Win32 File System namespace prefix.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterWin32FileDriveLetter:
                if (next == U':')
                {
                    // It's a DOS drive letter.
                    state = State::AfterDriveColon;
                    root.push_back(next);
                    rootType = PathRootType::DosDrive;
                    hasRoot = true;
                    ++current;
                }
                else if (isValidElementSeparator(next) ||
                         isValidElementCharacter(next))
                {
                    // Its \\?\Xxxx

                    // It's likely there is no root and we are parsing a
                    // relative path.
                    root.clear();
                    current = pos;
                    keepParsing = false;
                }
                else
                {
                    error = String::format("Invalid character '{0}' in path root.",
                                           { next });
                    keepParsing = false;
                }
                break;
                break;

            case State::AfterWin32FileU:
                if (next == U'N')
                {
                    // It still looks like \\?\UNC\...
                    state = State::AfterWin32FileN;
                    ++current;
                }
                else if (next == U':')
                {
                    // It looks more like a dos drive letter after a Win32
                    // file system prefix: \\?\U:\

                    root.push_back(U'U');
                    root.push_back(U':');
                    state = State::AfterDriveColon;
                    hasRoot = true;
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' in "
                                           "Win32 File System namespace prefix.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterWin32FileN:
                if (next == U'C')
                {
                    // It still looks like \\?\UNC\...
                    state = State::AfterWin32FileC;
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' in "
                                           "Win32 File System namespace prefix.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterWin32FileC:
                if (isValidElementSeparator(next))
                {
                    // It still looks like \\?\UNC\Host\Volume which
                    // we will report as \\Host\Volume
                    state = State::AfterFirstUncSlash;
                    root.append(2, U'\\');
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' after "
                                           "Win32 File System namespace prefix.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterFirstUncSlash:
                if (isValidElementCharacter(next))
                {
                    // It's a Win32 file namespace UNC name: \\?\UNC\X...
                    state = State::AfterFirstHostCharacter;
                    root.push_back(next);
                    ++current;
                }
                else if (isValidElementSeparator(next))
                {
                    // We've got enough slashes, ignore this one.
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' after "
                                           "Win32 File System namespace prefix.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterFirstHostCharacter:
                if (isValidElementCharacter(next))
                {
                    // Accumulate the rest of the host name.
                    root.push_back(next);
                    ++current;
                }
                else if (isValidElementSeparator(next))
                {
                    // Move on to the volume name.
                    state = State::AfterHostSlash;
                    root.push_back(U'\\');
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' in UNC "
                                           "host name.", { next });
                    keepParsing = false;
                }
                break;

            case State::AfterHostSlash:
                if (isValidElementCharacter(next))
                {
                    state = State::AfterFirstVolumeCharacter;
                    rootType = PathRootType::UncName;
                    hasRoot = true;
                    root.push_back(next);
                    ++current;
                }
                else if (isValidElementSeparator(next))
                {
                    // Skip subsequent slashes.
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' in UNC host name.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterFirstVolumeCharacter:
                if (isValidElementCharacter(next))
                {
                    // Accumulate the rest of the volume name.
                    root.push_back(next);
                    ++current;
                }
                else if (isValidElementSeparator(next))
                {
                    state = State::AfterTailingSlash;
                    root.push_back(U'\\');
                    hasRoot = true;
                    rootType = PathRootType::UncName;
                    hasTrailingSeparator = true;
                    ++current;
                }
                else
                {
                    error = String::format("Invalid character '{0}' in UNC "
                                           "volume name.", { next });
                    keepParsing = false;
                }
                break;

            case State::AfterDriveLetter:
                if (next == U':')
                {
                    // It's a DOS drive letter.
                    state = State::AfterDriveColon;
                    root.push_back(next);
                    rootType = PathRootType::DosDrive;
                    hasRoot = true;
                    ++current;
                }
                else if (isValidElementSeparator(next) ||
                         isValidElementCharacter(next))
                {
                    // It's likely there is no root and we are parsing a
                    // relative path.
                    root.clear();
                    current = pos;
                    keepParsing = false;
                }
                else
                {
                    error = String::format("Invalid character '{0}' in path root.",
                                           { next });
                    keepParsing = false;
                }
                break;

            case State::AfterDriveColon:
                if (isValidElementSeparator(next))
                {
                    // Ignore any further slashes.
                    state = State::AfterTailingSlash;
                    root.push_back(U'\\');
                    hasTrailingSeparator = true;
                    ++current;
                    pos = current;
                }
                else
                {
                    error = String::format("Unexpected character '{0}' after drive "
                                           "specification.", { next });
                    keepParsing = false;
                }
                break;

            case State::AfterTailingSlash:
                if (isValidElementSeparator(next))
                {
                    // Consume trailing slashes without storing them.
                    ++current;
                }
                else
                {
                    // We've reached the end of the path root.
                    state = State::Complete;
                    keepParsing = false;
                    pos = current;
                }
                break;

            case State::Complete:
            default:
                keepParsing = false;
                current = end;
                break;
            }
        }

        if (current == end)
        {
            switch (state)
            {
            case State::AfterFirstSlash:
            case State::AfterSecondSlash:
                root.push_back(U'\\');
                rootType = PathRootType::CurrentDrive;
                hasTrailingSeparator = true;
                hasRoot = true;
                break;

            case State::AfterFirstVolumeCharacter:
            case State::AfterWin32FileCurrentDrive:
            case State::AfterDriveColon:
            case State::AfterTailingSlash:
                hasRoot = true;
                break;

            default:
                if (error.isEmpty())
                {
                    error = "Unexpected end to path root.";
                }

                hasRoot = false;
                break;
            }

            // Ensure no more of the path is parsed.
            pos = end;

            if (hasRoot && (hasTrailingSeparator == false))
            {
                // Ensure there is a trailing separator.
                root.push_back(U'\\');
            }
        }

        return hasRoot;
    }

    virtual std::string rootToString(PathRootType rootType,
                                     string_cref_t rootText,
                                     PathUsage usage) const override
    {
        std::string path;
        path.reserve(256);

        if (usage == PathUsage::Kernel)
        {
            if (rootType == PathRootType::UncName)
            {
                // Add enough characters to append '\\?\UNC\' but ignore the first
                // leading slash of the root.
                path.append("\\\\?\\UNC");

                // Append all of the root except the first slash.
                path.append(rootText.getUtf8Bytes() + 1, rootText.getUtf8Length() - 1);
            }
            else
            {
                // Add enough characters to append '\\?\'
                path.append("\\\\?\\");
                path.append(rootText.getUtf8Bytes(), rootText.getUtf8Length());
            }
        }
        else
        {
            // Use the path root as is.
            path.append(rootText.getUtf8Bytes(), rootText.getUtf8Length());
        }

        return path;
    }

    virtual std::wstring rootToWideString(PathRootType rootType,
                                          string_cref_t rootText,
                                          PathUsage usage) const override
    {
        std::wstring widePath;
        widePath.reserve(256);

        if (usage == PathUsage::Kernel)
        {
            if (rootType == PathRootType::UncName)
            {
                // Add enough characters to append '\\?\UNC\' but ignore the first
                // leading slash of the root.
                widePath.append(L"\\\\?\\UNC");

                // Append all of the root except the first slash.
                Utf::appendToWide(widePath, rootText.getUtf8Bytes() + 1,
                                  rootText.getUtf8Length() - 1,
                                  rootText.getUtf16Length() - 1);
            }
            else
            {
                // Add enough characters to append '\\?\'
                widePath.append(L"\\\\?\\");
                Utf::appendToWide(widePath, rootText.getUtf8Bytes(),
                                  rootText.getUtf8Length(),
                                  rootText.getUtf16Length());
            }
        }
        else
        {
            // Use the path root as is.
            Utf::appendToWide(widePath, rootText.getUtf8Bytes(),
                              rootText.getUtf8Length(),
                              rootText.getUtf16Length());
        }

        return widePath;
    }

    virtual String pathToString(PathUsage usage, PathRootType rootType,
                                size_t /*rootLength*/,
                                string_cref_t path) const override
    {
        if ((usage == PathUsage::Kernel) && (path.getUtf8Length() > _MAX_PATH))
        {
            // We need to customise the path with the Win32 file namespace prefix.
            std::string buffer;
            buffer.reserve(path.getUtf8Length() + 8);

            if (rootType == PathRootType::UncName)
            {
                // We need to skip the first character of the source path.
                buffer.append("\\\\?\\UNC");
                buffer.append(path.getUtf8Bytes() + 1,
                              path.getUtf8Length() - 1);
            }
            else
            {
                buffer.append("\\\\?\\");
                buffer.append(path.getUtf8Bytes(), path.getUtf8Length());
            }

            return String(buffer);
        }
        else if ((usage == PathUsage::Shell) && (path.getUtf8Length() > _MAX_PATH))
        {
            throw PathTooLongException(path, _MAX_PATH);
        }
        else
        {
            // The path is good enough as it is.
            return path;
        }
    }

    virtual std::wstring pathToWideString(PathUsage usage,
                                          PathRootType rootType,
                                          size_t /*rootLength*/,
                                          string_cref_t path) const override
    {
        std::wstring wideBuffer;

        if ((usage == PathUsage::Kernel) && (path.getUtf8Length() > _MAX_PATH))
        {
            // We need to customise the path with the Win32 file namespace prefix.
            wideBuffer.reserve(path.getWideLength() + 8);

            if (rootType == PathRootType::UncName)
            {
                // We need to skip the first character of the source path.
                wideBuffer.append(L"\\\\?\\UNC");
                Utf::appendToWide(wideBuffer,
                                  path.getUtf8Bytes() + 1,
                                  path.getUtf8Length() - 1);
            }
            else
            {
                wideBuffer.append(L"\\\\?\\");

                Utf::appendToWide(wideBuffer,
                                  path.getUtf8Bytes(),
                                  path.getUtf8Length());
            }
        }
        else if ((usage == PathUsage::Shell) && (path.getUtf8Length() > _MAX_PATH))
        {
            throw PathTooLongException(path, _MAX_PATH);
        }
        else
        {
            // The path is good enough as it is.
            Utf::appendToWide(wideBuffer, path.getUtf8Bytes(),
                              path.getUtf8Length(), path.getWideLength());
        }

        return wideBuffer;
    }
};

class PosixPathSchema : public PathSchema
{
public:
    PosixPathSchema() = default;
    virtual ~PosixPathSchema() = default;

    // Overrides
    virtual bool isCaseSensitive() const override { return true; }
    virtual char getPathSeparator() const override { return ':'; }
    virtual wchar_t getWidePathSeparator() const override { return L':'; }
    virtual char getElementSeparator() const override { return '/'; }
    virtual wchar_t getWideElementSeparator() const override { return L'/'; }

    virtual bool isValidElementSeparator(char32_t character) const override
    {
        return (character == U'/');
    }

    virtual bool isValidElementCharacter(char32_t ch) const override
    {
        return ch != U'/';
    }

    virtual bool tryParsePathRoot(String::iterator &pos,
                                  const String::iterator &end,
                                  std::u32string &root, string_ref_t error,
                                  PathRootType &rootType) const override
    {
        // Trying to parse one of the following:
        // /...     (file system root)
        // ~        (users home directory with no path elements)
        // ~/...    (users home directory, path elements optional)
        enum class State
        {
            Start,
            AfterSlash,
            AfterTilda,
            AfterTildaSlash,
        };

        bool hasRoot = false;
        bool keepParsing = true;
        rootType = PathRootType::None;
        error = String::Empty;
        State state = State::Start;
        root.clear();
        root.reserve(4);
        auto original = pos;
        auto current = pos;

        while (keepParsing && (current != end))
        {
            char32_t next = *current;

            switch (state)
            {
            case State::Start:
                if (next == U'/')
                {
                    hasRoot = true;
                    state = State::AfterSlash;
                    rootType = PathRootType::SysRoot;
                    root.push_back(U'/');
                    pos = ++current;
                }
                else if (next == U'~')
                {
                    // Assume it's a valid home directory reference until
                    // proved otherwise.
                    hasRoot = true;
                    state = State::AfterTilda;
                    rootType = PathRootType::UserHome;
                    root.push_back(U'~');
                    root.push_back(U'/');
                    ++current;
                    pos = current;
                }
                else
                {
                    // It looks like a relative path.
                    keepParsing = false;
                    current = pos;
                }
                break;

            case State::AfterSlash:
                if (next == U'/')
                {
                    // Allow as many subsequent slashes as required.
                    pos = ++current;
                }
                else
                {
                    // Looks like we've reached the first path element.
                    keepParsing = false;
                }
                break;

            case State::AfterTilda:
                if (next == U'/')
                {
                    // It's definitely the user home directory.
                    state = State::AfterTildaSlash;
                    pos = ++current;
                }
                else
                {
                    // It was '~' followed by path element characters, which
                    // is not a valid root. So "~..." must be a path element.
                    pos = original;
                    rootType = PathRootType::None;
                    root.clear();
                    hasRoot = false;
                    keepParsing = false;
                }
                break;

            case State::AfterTildaSlash:
                if (next == U'/')
                {
                    // Consume any additional element separators.
                    pos = ++current;
                }
                else
                {
                    // We've reached the characters of the first path element.
                    keepParsing = false;
                }
                break;
            }
        }

        return true;
    }

    virtual std::string rootToString(PathRootType rootType,
                                     string_cref_t rootText,
                                     PathUsage usage) const override
    {
        std::string path;
        path.reserve(256);

        if ((usage == PathUsage::Kernel) &&
            (rootType == PathRootType::UserHome))
        {
            // Substitute the root with the home directory.
            String homePath = getHomeDirectory();
            path.append(homePath.getUtf8Bytes(),
                        homePath.getUtf8Length());
            path.push_back('/');
        }
        else
        {
            // Otherwise, use the root as-is.
            path.append(rootText.getUtf8Bytes(),
                        rootText.getUtf8Length());
        }

        return path;
    }

    virtual std::wstring rootToWideString(PathRootType rootType,
                                          string_cref_t rootText,
                                          PathUsage usage) const override
    {
        std::wstring widePath;
        widePath.reserve(256);

        if ((usage == PathUsage::Kernel) &&
            (rootType == PathRootType::UserHome))
        {
            // Substitute the root with the home directory.
            String homePath = getHomeDirectory();
            Utf::appendToWide(widePath, homePath.getUtf8Bytes(),
                              homePath.getUtf8Length(),
                              homePath.getWideLength());
        }
        else
        {
            // Otherwise, use the root as-is.
            Utf::appendToWide(widePath, rootText.getUtf8Bytes(),
                              rootText.getUtf8Length(),
                              rootText.getWideLength());
        }

        return widePath;
    }

    virtual String pathToString(PathUsage usage, PathRootType rootType,
                                size_t rootLength,
                                string_cref_t path) const override
    {
        if ((usage == PathUsage::Kernel) && (rootType == PathRootType::UserHome))
        {
            String homeDir = getHomeDirectory();

            std::string result;
            result.reserve(homeDir.getUtf8Length() + path.getUtf8Length() - rootLength);

            appendAgString(result, homeDir);
            result.push_back('/');
            result.append(path.getUtf8Bytes() + rootLength,
                          path.getUtf8Length() - rootLength);

            return String(result);
        }
        else
        {
            // The path is good enough as it is.
            return path;
        }
    }


    virtual std::wstring pathToWideString(PathUsage usage,
                                          PathRootType rootType,
                                          size_t rootLength,
                                          string_cref_t path) const override
    {
        std::wstring buffer;

        if ((usage == PathUsage::Kernel) && (rootType == PathRootType::UserHome))
        {
            String homeDir = getHomeDirectory();

            buffer.reserve(homeDir.getWideLength() + path.getWideLength());

            Utf::appendToWide(buffer, homeDir.getUtf8Bytes(),
                              homeDir.getUtf8Length(),
                              homeDir.getWideLength());

            buffer.push_back(L'/');

            Utf::appendToWide(buffer, path.getUtf8Bytes() + rootLength,
                              path.getUtf8Length() - rootLength);
        }
        else
        {
            // The path is good enough as it is.
            Utf::appendToWide(buffer, path.getUtf8Bytes(),
                              path.getUtf8Length(), path.getWideLength());
        }

        return buffer;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the file path schema native to the host platform.
PathSchemaID getNativeSchema()
{
    static const PathSchemaID schema =
#ifdef _WIN32
        getWin32Schema();
#else
        getPosixSchema();
#endif

    return schema;
}

//! @brief Gets the file path schema for the Win32 platform.
PathSchemaID getWin32Schema()
{
    static const Win32PathSchema schema;

    return &schema;
}

//! @brief Gets the file path schema for a POSIX-based platform.
PathSchemaID getPosixSchema()
{
    static const PosixPathSchema schema;

    return &schema;
}

}} // namespace Ag::Fs
////////////////////////////////////////////////////////////////////////////////


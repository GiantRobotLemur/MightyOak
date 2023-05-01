//! @file Core/CoreInternal.cpp
//! @brief The definition of stand-alone functions which are internal to the
//! Core library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>

#include "Ag/Core/Configuration.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Utf.hpp"

#include "CoreInternal.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief A function which uses vsnprintf() to format values as text.
//! @param[in] target The STL string to append the formatted character data to.
//! @param[in] format The format specification.
//! @param[in] ... The values to format as text according to the specification.
//! @note This function is only for internal use where exception safety is
//! paramount.
void appendPrintf(std::string &target, const char *format, ...)
{
    va_list args, sizeArgs;
    va_start(args, format);
    va_copy(sizeArgs, args);

    // Calculate the size of the buffer needed to format the text.
    int length = vsnprintf(nullptr, 0, format, sizeArgs);
    va_end(sizeArgs);

    if (length > 0)
    {
        // Create a temporary buffer to hold the data.
        size_t requiredSize = static_cast<size_t>(length) + 1;

        std::vector<char> buffer;
        buffer.resize(requiredSize);

        // Format text into the buffer.
        vsnprintf(buffer.data(), requiredSize, format, args);

        // Copy the bounded string into the buffer.
        target.append(buffer.data(), static_cast<size_t>(length));
    }

    va_end(args);
}

//! @brief Attempts to read the next line of text from an STD stream.
//! @param[in] input The stream to read from.
//! @param[out] line The line to receive the string.
//! @retval true A line was read, though it might be empty.
//! @retval false The end of the stream was reached.
bool tryReadLine(StdFilePtr &input, std::string &line)
{
    bool hasMoreData = false;
    line.clear();

    if (input)
    {
        FILE *stream = input.get();

        int next = fgetc(stream);

        if (next != EOF)
        {
            hasMoreData = true;

            while ((next != EOF) && (next != '\n'))
            {
                line.push_back(static_cast<char>(next));

                next = fgetc(stream);
            }
        }
    }

    return hasMoreData;
}

//! @brief Splits a line of text into tokens separated by spaces.
//! @param[in] line The line of text to tokenise.
//! @param[out] tokenRanges Receives start/length pairs representing the runs of
//! characters in line which represent tokens.
void tokeniseLine(const std::string &line, std::vector<StringRange> &tokenRanges)
{
    tokenRanges.clear();

    size_t tokenStart = 0;
    bool isToken = true;

    for (size_t index = 0, count = line.length(); index < count; ++index)
    {
        char next = line[index];

        if (std::isspace(next))
        {
            if (isToken)
            {
                // Complete the token.
                size_t length = index - tokenStart;

                if (length > 0)
                {
                    tokenRanges.emplace_back(tokenStart, length);
                }

                isToken = false;
            }
        }
        else if (isToken == false)
        {
            // Start a new token.
            tokenStart = index;
            isToken = true;
        }
    }

    if (isToken)
    {
        // Finish the last token.
        size_t length = line.length() - tokenStart;

        if (length > 0)
        {
            tokenRanges.emplace_back(tokenStart, length);
        }
    }
}

//! @brief Gets a view of a token in a tokenised line.
//! @param[in] line The line containing the tokens.
//! @param[in] range The start offset and length of the token to obtain.
//! @return A view of the string.
std::string_view getLineToken(const std::string &line, const StringRange &range)
{
    size_t start = range.first;
    size_t end = range.first + range.second;
    size_t length = std::min(end, line.length());

    return std::string_view(line.c_str() + std::min(start, line.length()), length);
}

//! @brief Attempts to open an stdio file.
//! @param[in] fileName The name of the file to open.
//! @param[in] mode The mode used to open or create the file, as would be
//! passed to fopen() or the like.
//! @param[out] stream Receives the handle to the opened file on success.
//! @param[out] error Receives an error message if opening fails.
//! @retval true The file was successfully opened, stream is populated,
//! error is empty.
//! @retval false The file could not be opened. Stream is nullptr and
//! error is populated with an appropriate error message.
bool tryOpenFile(const String &fileName, const utf8_cptr_t mode,
                 FILE *&stream, String &error)
{
    stream = nullptr;
    bool isOK = false;

#ifdef _MSC_VER
    // The Windows version where file names are expressed as wide characters.
    std::wstring wideFileName = fileName.toWide(), wideMode;
    Utf::appendToWide(wideMode, mode, std::strlen(mode));

    int errorCode = _wfopen_s(&stream, wideFileName.c_str(), wideMode.c_str());

    if (errorCode == 0)
    {
        error = String::Empty;
        isOK = true;
    }
    else
    {
        error = getRuntimeLibraryErrorMessage(errorCode);
    }
#else
    // The POSIX version, where system calls interpret char as UTF-8.
    stream = fopen(fileName.getUtf8Bytes(), mode);

    if (stream == nullptr)
    {
        error = getRuntimeLibraryErrorMessage(errno);
    }
    else
    {
        isOK = true;
        error = String::Empty;
    }
#endif

    return isOK;
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


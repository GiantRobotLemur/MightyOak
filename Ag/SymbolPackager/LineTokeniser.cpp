//! @file LineTokeniser.cpp
//! @brief The definition of an object which splits a line of text into white
//! space separated tokens.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>

#include "LineTokeniser.hpp"

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing the tokens which define a line of
//! text to be recognised.
//! @param[in] elements The tokens to be recognised.
LineSignature::LineSignature(const std::initializer_list<BoundedString> &elements) :
    _elements(elements)
{
}

//! @brief Gets the count of tokens to match.
size_t LineSignature::getElementCount() const
{
    return _elements.size();
}

//! @brief Gets a token to match.
const BoundedString &LineSignature::getElement(size_t index) const
{
    return _elements.at(index);
}

//! @brief Creates an object with initially no tokens.
LineTokeniser::LineTokeniser()
{
}

//! @brief Gets the source tokenised line of text.
const std::string &LineTokeniser::getLine() const
{
    return _sourceLine;
}

//! @brief Gets the count of tokens in the last line parsed.
size_t LineTokeniser::getTokenCount()
{
    return _tokens.size();
}

//! @brief Gets the a token from the last line read.
const BoundedString &LineTokeniser::getToken(size_t index) const
{
    return _tokens.at(index);
}

//! @brief Gets all characters in the parsed line starting at a specified token.
//! @param[in] index The 0-based index of the token marking the start of text
//! to retrieve.
//! @return A bounded string representing the suffix of the input line.
BoundedString LineTokeniser::getTail(size_t index) const
{
    const char *start = _sourceLine.c_str() + _sourceLine.size();
    size_t length = 0;

    if (index < _tokens.size())
    {
        start = _tokens[index].Text;

        size_t prefixLength = _tokens[index].Text - _sourceLine.c_str();
        length = _sourceLine.size() - prefixLength;
    }

    return BoundedString(start, length);
}

//! @brief Determines if the line has at least one token and that the first
//! token begins with a specified prefix.
//! @param[in] prefix The prefix to check for.
//! @retval true The line has the specified prefix.
//! @retval false The line is blank of has a different prefix.
bool LineTokeniser::startsWith(const BoundedString &prefix) const
{
    return (_tokens.empty() == false) && _tokens[0].startsWith(prefix);
}

//! @brief Determines if the initial tokens of the line match a specific
//! signature.
//! @param[in] signature The prefix signature to match.
//! @retval true The initial tokens match the signature.
//! @retval false The line does not have enough tokens or the tokens it does
//! have don't match the signature.
bool LineTokeniser::matches(const LineSignature &signature) const
{
    bool isMatching = false;
    size_t elementCount = signature.getElementCount();

    if (elementCount <= _tokens.size())
    {
        isMatching = true;

        for (size_t index = 0; index < elementCount; ++index)
        {
            if (_tokens[index] != signature.getElement(index))
            {
                // The tokens are different, stop comparing.
                isMatching = false;
                index = elementCount;
            }
        }
    }

    return isMatching;
}

//! @brief Searches for a sub-string within the buffered line.
//! @param[in] subString The text to find.
//! @param[in] startToken The 0-based index of the token to start searching from.
//! @param[out] pos Receives the 0-based index of the first character of the
//! sub-string as it appears in the buffered line.
//! @retval true An occurrence of the substring was found.
//! @retval false The substring did not appear in the buffered line.
bool LineTokeniser::find(const BoundedString &subString, size_t startToken, size_t &pos) const
{
    bool hasSubString = false;

    if ((subString.Length <= _sourceLine.length()) &&
        (startToken < _tokens.size()))
    {
        size_t start = _tokens[startToken].Text - _sourceLine.c_str();

        pos = _sourceLine.find(std::string_view(subString.Text, subString.Length),
                               start);

        hasSubString = pos != std::string::npos;
    }

    return hasSubString;
}

//! @brief Reads and tokenises the next line from an input stream.
//! @param[in] inputStream The stream to read characters from.
//! @retval true A line was read, though it possibly had no tokens.
//! @retval false No tokens were read and the end of the stream was reached.
bool LineTokeniser::tryReadLine(FILE *inputStream)
{
    _tokens.clear();
    _sourceLine.clear();

    int next = std::fgetc(inputStream);

    size_t tokenStart = 0;
    size_t tokenLength = 0;

    while ((next != EOF) && (next != '\n'))
    {
        _sourceLine.push_back(static_cast<char>(next));

        // Get the next character.
        next = std::fgetc(inputStream);
    }

    // Now that we have the entire line, we can tokenise it.
    for (char ch : _sourceLine)
    {
        if (std::isspace(ch))
        {
            if (tokenLength > 0)
            {
                _tokens.emplace_back(_sourceLine.c_str() + tokenStart,
                                     tokenLength);

                // We need to point to the character after the space.
                tokenStart += tokenLength + 1;
                tokenLength = 0;
            }
            else
            {
                ++tokenStart;
            }
        }
        else
        {
            ++tokenLength;
        }
    }

    if (tokenLength > 0)
    {
        _tokens.emplace_back(_sourceLine.c_str() + tokenStart,
                             tokenLength);
    }

    return (_tokens.empty() == false) || (next != EOF);
}

////////////////////////////////////////////////////////////////////////////////


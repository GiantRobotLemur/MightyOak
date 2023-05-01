//! @file InputContext.cpp
//! @brief The definition of an object handling input from a specific source.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "InputContext.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// InputContext Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to manage input from a specified source.
//! @param[in] source The input source to take ownership of.
//! @param[in] sourceReference The location of the reference to the current source
//! on other source code.
//! @param[in] sourceId The text used to identify the source in messages, such
//! as the full path to the file resolved from the initial source name.
//! @param[in] lexerStackLevel The count of elements in the lexical context
//! stack when as the context is created.
InputContext::InputContext(IInputSourcePtr &source, const Location &sourceReference,
                           const Ag::String &sourceId, size_t lexerStackLevel) :
    _source(std::move(source)),
    _reference(sourceReference),
    _position(sourceId),
    _bufferMin(0),
    _bufferPos(0),
    _lexerStackLevel(lexerStackLevel)
{
    _buffer.reserve(256);
}

//! @brief Gets the count of elements in the lexical context stack when
//! as the context is created.
size_t InputContext::getBaseLexerStackLevel() const
{
    return _lexerStackLevel;
}

//! @brief Gets the position of the last character obtained from the
//! input stream.
const Location &InputContext::getCurrentLocation() const
{
    return _position;
}

//! @brief Attempts to get the next character from the input source.
//! @param[out] next Receives the next Unicode character.
//! @retval true A new character was returned.
//! @retval false No more characters are available from this source.
bool InputContext::tryGetNextCharacter(char32_t &next)
{
    bool hasCharacter = false;

    if (_bufferPos < _buffer.size())
    {
        next = _buffer[_bufferPos++];
        hasCharacter = true;
    }
    else if (tryRefillBuffer() && (_bufferPos < _buffer.size()))
    {
        next = _buffer[_bufferPos++];
        hasCharacter = true;
    }

    if (hasCharacter)
    {
        // Keep track of the source position.
        if (next == U'\n')
        {
            ++_position.LineNo;
            _position.Offset = 0;
        }
        else
        {
            ++_position.Offset;
        }
    }

    return hasCharacter;
}

//! @brief Determines if the next character, if there is one, is a
//! specified value.
//! @param[in] expected The character to check for.
//! @retval true The next character was equal to expected.
//! @retval false The next character was not expected so it was returned to the
//! input stream, or there were no more characters in the input stream.
bool InputContext::ensureNextCharacter(char32_t expected)
{
    char32_t next;
    bool wasExpected = false;

    if (tryGetNextCharacter(next))
    {
        if (next == expected)
        {
            wasExpected = true;
        }
        else
        {
            // Put it back in the input stream.
            ungetCharacter();
        }
    }

    return wasExpected;
}

//! @brief Move the input pointer back one character.
//! @throw OperationException If an attempt is made to unget characters to
//! a point before the last stream break.
void InputContext::ungetCharacter()
{
    if (_bufferPos > 0)
    {
        --_bufferPos;
        --_position.Offset;

        if (_buffer[_bufferPos] == U'\n')
        {
            // We un-got a new line character. We can fix up the line
            // position, but not the offset as we don't know how long the
            // last line was. It probably shouldn't matter though.
            --_position.LineNo;
        }

        _bufferMin = std::min(_bufferPos, _bufferMin);
    }
    else
    {
        throw Ag::OperationException("Cannot unget a character past the "
                                     "last input stream break.");
    }
}

//! @brief Marks all previous characters in the input stream as no longer
//! required, i.e. the should not be readable via ungetCharacter() followed
//! by tryGetCharacter().
void InputContext::breakInputStream()
{
    _bufferMin = _bufferPos;
}

//! @brief Attempt to read new characters from the input stream.
//! @retval true More characters were read.
//! @retval false No more characters could be read from the input stream.
bool InputContext::tryRefillBuffer()
{
    const size_t MinReadSize = 32;
    size_t validCharacters = _buffer.size() - _bufferMin;
    bool hasCharacters = false;

    if (validCharacters == 0)
    {
        // The buffer can be completely overwritten.
        _buffer.resize(_buffer.capacity());

        // Read the next set of characters.
        size_t charactersRead = _source->readCharacters(_buffer.data(), _buffer.size());

        // Cut the unused part of the buffer off.
        _buffer.resize(charactersRead);

        hasCharacters = charactersRead > 0;

        // Reset the buffer pointers.
        _bufferMin = 0;
        _bufferPos = 0;
    }
    else
    {
        if (_bufferMin >= MinReadSize)
        {
            // Shuffle the characters down to make some more space.
            char32_t *buffer = _buffer.data();

            for (size_t index = 0; index < validCharacters; ++index)
            {
                buffer[index] = buffer[index + _bufferMin];
            }

            _buffer.resize(validCharacters);
            _bufferPos -= _bufferMin;
            _bufferMin = 0;
        }

        size_t availableSpace = std::max(_buffer.capacity() - _buffer.size(),
                                         MinReadSize);
        size_t currentSize = _buffer.size();

        _buffer.resize(currentSize + availableSpace);

        size_t charactersRead = _source->readCharacters(_buffer.data() + currentSize,
                                                        availableSpace);

        _buffer.resize(currentSize + charactersRead);
        hasCharacters = charactersRead > 0;
    }

    return hasCharacters;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


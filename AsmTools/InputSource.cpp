//! @file InputSource.cpp
//! @brief The definition of an object which produces source code text to be
//! interpreted.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "InputSource.hpp"

namespace Mo {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An implementation of IInputSource backed by a text file.
class FileInputSource : public IInputSource
{
private:
    Ag::StdFilePtr _input;
public:
    FileInputSource(FILE *input) :
        _input(input)
    {
    }

    virtual ~FileInputSource() = default;

    // Inherited from IInputSource.
    virtual size_t readCharacters(Ag::utf32_ptr_t buffer, size_t bufferSize) override
    {
        size_t bufferUsed = 0;

        // Read characters until the buffer is full or the stream ended.
        while (_input && (bufferUsed < bufferSize))
        {
            Ag::Utf::FromWideConverter converter;
            char32_t result;
            bool hasError = false;

            while (bufferUsed < bufferSize)
            {
                // Get the next character.
                wint_t next = fgetwc(_input.get());

                if (next == WEOF)
                {
                    // Close the file to ensure no further characters are read.
                    _input.reset();
                    break;
                }
                else if (converter.tryConvert(static_cast<wchar_t>(next),
                                                result, hasError))
                {
                    buffer[bufferUsed++] = result;
                }
                else if (hasError)
                {
                    converter.reset();
                }
            }
        }

        return bufferUsed;
    }
};

//! @brief An implementation of IInputSource driven from a UTF-8 string.
class StringInputSource : public IInputSource
{
private:
    Ag::String _source;
    size_t _offset;
public:
    StringInputSource(Ag::string_cref_t source) :
        _source(source),
        _offset(0)
    {
    }

    virtual ~StringInputSource() = default;

    // Overrides
    virtual size_t readCharacters(Ag::utf32_ptr_t buffer,
                                  size_t bufferSize) override
    {
        size_t bufferUsed = 0;
        size_t sourceMax = _source.getUtf8Length();

        if (_offset < sourceMax)
        {
            Ag::Utf::FromUtf8Converter converter;
            char32_t next = 0;
            bool hasError = false;

            while (bufferUsed < bufferSize)
            {
                size_t index = 0;
                size_t indexMax = sourceMax - _offset;
                Ag::uint8_cptr_t encoded = reinterpret_cast<Ag::uint8_cptr_t>(_source.getUtf8Bytes()) + _offset;
                converter.reset();

                while (index < indexMax)
                {
                    if (converter.tryConvert(encoded[index++], next, hasError))
                    {
                        buffer[bufferUsed++] = next;
                        break;
                    }
                    else if (hasError)
                    {
                        converter.reset();
                    }
                }

                _offset += index;

                if (_offset >= sourceMax)
                {
                    // We have no more characters to give.
                    break;
                }
            }
        }

        return bufferUsed;
    }
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to create a file source object by opening a text file.
//! @param[in] fileName The name of the file to read.
//! @param[out] ptr Receives the file pointer, if the file was successfully opened.
//! @param[out] error Receives an error message if the file could not be opened.
//! @retval true The file was successfully opened and returned in ptr.
//! @retval false The file could not be opened, error describes why.
bool tryCreateFileInputSource(const Ag::Fs::Path &fileName, IInputSourcePtr &ptr,
                              Ag::string_ref_t error)
{
    FILE *stream = nullptr;
    bool isOK = false;

    Ag::String path = fileName.toString(Ag::Fs::PathUsage::Kernel);

    if (tryOpenFile(path, "r", stream, error))
    {
        ptr = std::make_unique<FileInputSource>(stream);
        isOK = true;
    }

    return isOK;
}

//! @brief Creates an input source which dispenses the contents of a text string.
//! @param[in] sourceText The text to be produced by the input source object.
//! @return A newly created IInputSource object.
IInputSourcePtr createBufferInputSource(Ag::string_cref_t sourceText)
{
    return std::make_unique<StringInputSource>(sourceText);
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


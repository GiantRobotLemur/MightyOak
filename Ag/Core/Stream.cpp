//! @file Stream.cpp
//! @brief The definition of a simple interface for reading and writing
//! binary data.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Stream.hpp"
#include "Ag/Core/Utils.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// IStream Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to read and decode a byte count value from a stream.
//! @param[in] input The stream to read from.
//! @param[out] length The byte count value read from the stream.
//! @retval true A value was successfully read.
//! @retval false The value could not be read.
bool IStream::tryReadLength(IStream *input, size_t &length)
{
    bool isRead = false;
    uint8_t nextByte;

    if ((input != nullptr) && input->tryRead(&nextByte, 1))
    {
        uint64_t value = 0;

        if (nextByte < 0xFA)
        {
            // The length is encoded in the first byte.
            length = static_cast<size_t>(nextByte);
            isRead = true;
        }
        else
        {
            // The length is encoded in the bytes which follow.
            size_t valueSize = nextByte - 0xF8;

            if (input->tryRead(&value, valueSize))
            {
                isRead = true;

                if (valueSize > SIZE_MAX)
                {
                    length = SIZE_MAX;
                }
                else
                {
                    length = static_cast<size_t>(value);
                }
            }
        }
    }

    return isRead;
}

//! @brief Attempts to encode and write a byte count value to a stream.
//! @param[in] output The stream to write to.
//! @param[in] length The byte count to encode.
//! @retval true The value was successfully written.
//! @retval false The value could not be written.
bool IStream::tryWriteLength(IStream *output, size_t length)
{
    // Calculate how many bytes are required to represent the size value.
    int bitCount = Bin::log2(length, false);
    int byteCount = (bitCount + 7) / 8;
    bool isOK = false;

    if (byteCount < 2)
    {
        // Encode the size in a single byte.
        isOK = output->tryWrite(&length, 1);
    }
    else
    {
        // Encode a header byte and the fewest bytes necessary to define
        // the value.
        uint8_t header = static_cast<uint8_t>(byteCount + 0xF8);

        isOK = output->tryWrite(&header, 1) &&
            output->tryWrite(&length, toSize(byteCount));
    }

    return isOK;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


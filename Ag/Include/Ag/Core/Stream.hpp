//! @file Stream.hpp
//! @brief The declaration of a simple interface for reading and writing
//! binary data.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_STREAM_HPP__
#define __AG_CORE_STREAM_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A simple interface for reading and writing binary data.
class IStream
{
public:
    virtual ~IStream() = default;

    // Operations
    static bool tryReadLength(IStream *input, size_t &length);
    static bool tryWriteLength(IStream *output, size_t length);

    virtual bool tryRead(void *targetBuffer, size_t requiredByteCount) = 0;
    virtual bool tryWrite(const void *sourceBuffer, size_t sourceByteCount) = 0;
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

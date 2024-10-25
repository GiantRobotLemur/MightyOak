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

#include <memory>

#include "Binary.hpp"
#include "FsPath.hpp"

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
    // TODO: These are related to binary serialization of Variants and should
    // perhaps be elsewhere.
    static bool tryReadLength(IStream *input, size_t &length);
    static bool tryWriteLength(IStream *output, size_t length);

    //! @brief Reads bytes from the stream.
    //! @param[in] targetBuffer The buffer to receive the bytes read.
    //! @param[in] requiredByteCount The maximum number of bytes to read.
    //! @return The actual number of bytes read.
    //! @throws Ag::Exception If an error occurs during the read.
    virtual size_t read(void *targetBuffer, size_t requiredByteCount) = 0;

    //! @brief Writes bytes to the stream.
    //! @param[in] sourceBuffer The bytes to write.
    //! @param[in] sourceByteCount The maximum count of bytes to write.
    //! @return The actual number of bytes written.
    //! @throws Ag::Exception If an error occurs during the write.
    virtual size_t write(const void *sourceBuffer, size_t sourceByteCount) = 0;

    //! @brief Attempts to read bytes from the stream.
    //! @param[in] sourceBuffer Receives the bytes read from the stream.
    //! @param[in] sourceByteCount The maximum count of bytes to read.
    //! @retval true The required count of bytes was read.
    //! @retval false An error occurred or not all bytes were read.
    virtual bool tryRead(void *targetBuffer, size_t requiredByteCount) = 0;

    //! @brief Attempts to write bytes to the stream.
    //! @param[in] sourceBuffer The bytes to write.
    //! @param[in] sourceByteCount The count of bytes to write.
    //! @retval true All bytes were written to the stream.
    //! @retval false Either an error occurred, or not all bytes were written.
    virtual bool tryWrite(const void *sourceBuffer, size_t sourceByteCount) = 0;
};

//! @brief An object which deletes implementations of IStream as part of a
//! unique_ptr to a stream implementation.
struct IStreamDeleter
{
    void operator()(IStream *stream) const;
};

//! @brief An alias for a unique pointer to a stream.
using IStreamUPtr = std::unique_ptr<IStream, IStreamDeleter>;

//! @brief An alias for a bit field used to define access to a file.
using FileAccessBits = uint8_t;

//! @brief Defines bit patterns used when opening file streams.
struct FileAccess
{
    static constexpr FileAccessBits Read = 0x01;
    static constexpr FileAccessBits Write = 0x02;
    static constexpr FileAccessBits CreateNew = 0x04;
    static constexpr FileAccessBits CreateAlways = 0x04;
    static constexpr FileAccessBits OpenExisting = 0x08;
};

//! @brief An IStream which represents the contents of a file.
class IFileStream : public IStream
{
public:
    // Public Types
    //! @brief An object which can delete an IFileStream implementation as part
    //! of a unique_ptr type.
    struct Deleter
    {
        void operator()(IFileStream *stream) const;
    };

    //! @brief An alias for a unique_ptr to an IFileStream implementation.
    using UPtr = std::unique_ptr<IFileStream, Deleter>;

    // Construction/Destruction
    static UPtr open(const Fs::Path &at, FileAccessBits access);
    virtual ~IFileStream() = default;

    // Accessors
    //! @brief Gets the path defining the file the stream accesses.
    virtual const Fs::Path &getPath() const =0;

    // Operations
    static void readWholeFile(const Fs::Path &fileName, ByteBlock &fileData);

    // Overrides
    virtual size_t read(void *targetBuffer, size_t requiredByteCount) = 0;
    virtual size_t write(const void *sourceBuffer, size_t sourceByteCount) = 0;
    virtual bool tryRead(void *targetBuffer, size_t requiredByteCount) = 0;
    virtual bool tryWrite(const void *sourceBuffer, size_t sourceByteCount) = 0;
};

//! @brief An alias for a unique_ptr to an IFileStream implementation.
using IFileStreamUPtr = IFileStream::UPtr;

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

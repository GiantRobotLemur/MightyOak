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
#include "Platform.hpp"
#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/FsDirectory.hpp"
#include "Ag/Core/Stream.hpp"
#include "Ag/Core/Utils.hpp"

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

//! @brief A structure which defines generic C-style file operations.
struct FileTraits
{
    using FileDescriptor = HANDLE;
    using ErrorCode = DWORD;
    static constexpr FileDescriptor BadFile = INVALID_HANDLE_VALUE;

    static Exception createError(const std::string_view &fnName,
                                 ErrorCode errorCode)
    {
        return Win32Exception(fnName.data(), errorCode);
    }

    static bool isValidFd(FileDescriptor fd)
    {
        return (fd != NULL) && (fd != INVALID_HANDLE_VALUE);
    }

    static size_t read(FileDescriptor fd, void *buffer,
                       size_t byteCount, ErrorCode &errorCode)
    {
        size_t bytesRead = 0;
        errorCode = ERROR_SUCCESS;

        do
        {
            DWORD bytesToRead = static_cast<DWORD>(std::min<size_t>(UINT32_MAX,
                                                                    byteCount - bytesRead));
            DWORD actuallyRead = 0;

            if (::ReadFile(fd, buffer, bytesToRead, &actuallyRead, nullptr))
            {
                bytesRead += actuallyRead;

                if (actuallyRead < bytesToRead)
                {
                    // We got as much as we could.
                    break;
                }
            }
            else
            {
                errorCode = ::GetLastError();
                break;
            }
        } while (bytesRead < byteCount);

        return bytesRead;
    }

    static size_t write(FileDescriptor fd, const void *buffer,
                        size_t byteCount, ErrorCode &errorCode)
    {
        size_t bytesWritten = 0;
        errorCode = ERROR_SUCCESS;

        const uint8_t *source = reinterpret_cast<const uint8_t *>(buffer);

        while (bytesWritten < byteCount)
        {
            DWORD bytesToWrite = static_cast<DWORD>(std::min<size_t>(UINT32_MAX,
                                                                     byteCount - bytesWritten));
            DWORD actuallyWritten;

            if (::WriteFile(fd, source + bytesWritten, bytesToWrite, &actuallyWritten, nullptr))
            {
                bytesWritten += actuallyWritten;

                if (actuallyWritten < bytesToWrite)
                {
                    // We didn't manage to write it all, so stop trying.
                    break;
                }
            }
            else
            {
                errorCode = ::GetLastError();
                break;
            }
        }

        return bytesWritten;
    }

    static FileDescriptor open(const Fs::Path &path, FileAccessBits access)
    {
        std::wstring filename = path.toWideString(Fs::PathUsage::Kernel);

        DWORD handleAccess = 0;
        DWORD shareMode = FILE_SHARE_READ;
        DWORD createMode = 0;

        if (access & FileAccess::Read)
        {
            handleAccess = GENERIC_READ;
        }

        if (access & FileAccess::Write)
        {
            handleAccess = GENERIC_WRITE;
        }

        if (access & FileAccess::CreateAlways)
        {
            createMode = CREATE_ALWAYS;
        }
        else if (access & FileAccess::CreateNew)
        {
            createMode = CREATE_NEW;
        }
        else if (access & FileAccess::OpenExisting)
        {
            createMode = OPEN_EXISTING;
        }
        else
        {
            createMode = OPEN_ALWAYS;
        }

        HANDLE fileHandle = ::CreateFileW(filename.c_str(), handleAccess,
                                          shareMode, nullptr, createMode,
                                          0, nullptr);

        if (fileHandle == INVALID_HANDLE_VALUE)
        {
            DWORD errorCode = ::GetLastError();
            std::string fnName("CreateFile('");
            appendAgString(fnName, path.toString(Fs::PathUsage::Kernel));
            fnName.append("')");

            new Win32Exception(fnName.c_str(), errorCode);
        }

        return fileHandle;
    }

    static bool close(FileDescriptor fd)
    {
        if (fd != BadFile)
        {
            return ::CloseHandle(fd) != FALSE;
        }

        return true;
    }
};
#else // if POSIX

//! @brief A structure which defines generic C-style file operations.
struct FileTraits
{
    using FileDescriptor = int;
    using ErrorCode = int;
    static constexpr FileDescriptor BadFile = -1;

    static Exception createError(const std::string_view &fnName,
                                 ErrorCode errorCode)
    {
        return RuntimeLibraryException(fnName.data(), errorCode);
    }

    static bool isValidFd(FileDescriptor fd)
    {
        return fd >= 0;
    }

    static size_t read(FileDescriptor fd, void *buffer,
                       size_t byteCount, ErrorCode &errorCode)
    {
        auto bytesRead = ::read(fd, buffer, byteCount);

        if (bytesRead < 0)
        {
            errorCode = errno;
            return 0;
        }
        else
        {
            errorCode = 0;
            return static_cast<size_t>(bytesRead);
        }
    }

    static size_t write(FileDescriptor fd, const void *buffer,
                        size_t byteCount, ErrorCode &errorCode)
    {
        auto bytesWritten = ::write(fd, buffer, byteCount);

        if (bytesWritten < 0)
        {
            errorCode = errno;
            return 0;
        }
        else
        {
            errorCode = 0;
            return static_cast<size_t>(bytesWritten);
        }
    }

    static FileDescriptor open(const Fs::Path &path, FileAccessBits access)
    {
        String pathText = path.toString(Fs::PathUsage::Kernel);
        int flags = 0;
        int mode = S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP;

        static constexpr FileAccessBits ReadWrite = FileAccess::Read | FileAccess::Write;
        static constexpr FileAccessBits Create = FileAccess::CreateNew | FileAccess::CreateAlways;

        if ((access & ReadWrite) == ReadWrite)
        {
            flags = O_RDWR;
        }
        else if (access & FileAccess::Write)
        {
            flags = O_WRONLY;
        }
        else
        {
            flags = O_RDONLY;
        }

        FileDescriptor fd;

        if (access & Create)
        {
            fd = ::open64(pathText.getUtf8Bytes(), flags | O_EXCL | O_CREAT, mode);

            if ((fd >= 0) && (access & FileAccess::CreateAlways))
            {
                // Truncate the file to zero length.
                if (ftruncate(fd, 0) < 0)
                {
                    std::string fnName("ftruncate('");
                    appendAgString(fnName, path.toString(Fs::PathUsage::Kernel));
                    fnName.append("')");

                    throw RuntimeLibraryException(fnName.c_str(), errorCode);
                }
            }
        }
        else
        {
            fd = ::open64(pathText.getUtf8Bytes(), flags, mode);
        }

        int errorCode = errno;

        if (fd < 0)
        {
            std::string fnName("open64('");
            appendAgString(fnName, path.toString(Fs::PathUsage::Kernel));
            fnName.append("')");

            throw RuntimeLibraryException(fnName.c_str(), errorCode);
        }

        return fd;
    }

    static bool close(FileDescriptor fd)
    {
        if (fd < 0)
        {
            return true;
        }
        else
        {
            return ::close(fd) >= 0;
        }
    }
};
#endif

//! @brief A concrete implementation of IFileStream.
class FileStream : public IFileStream
{
private:
    // Internal Fields
    Fs::Path _location;
    FileTraits::FileDescriptor _fd;

public:
    // Construction/Destruction
    FileStream(const Fs::Path &path, FileTraits::FileDescriptor fd) :
        _location(path),
        _fd(fd)
    {
    }

    virtual ~FileStream()
    {
        FileTraits::close(_fd);
        _fd = FileTraits::BadFile;
    }

    // Inherited from IStream.
    virtual size_t read(void *targetBuffer, size_t requiredByteCount) override
    {
        if (_fd == FileTraits::BadFile)
        {
            throw OperationException("Reading from a file which isn't open.");
        }

        FileTraits::ErrorCode errorCode;
        size_t bytesRead = FileTraits::read(_fd, targetBuffer,
                                            requiredByteCount,
                                            errorCode);

        if (errorCode != 0)
        {
            std::string fnName;
            fnName.assign("file.read('");
            appendAgString(fnName, _location.toString(Fs::PathUsage::Kernel));
            fnName.append("', ");
            appendFileSize(FormatInfo::getDisplay(), fnName, requiredByteCount);
            fnName.push_back(')');

            throw FileTraits::createError(fnName, errorCode);
        }

        return bytesRead;
    }

    // Inherited from IStream.
    virtual size_t write(const void *sourceBuffer, size_t sourceByteCount) override
    {
        if (_fd == FileTraits::BadFile)
        {
            throw OperationException("Writing to a file which isn't open.");
        }

        FileTraits::ErrorCode errorCode;
        size_t bytesWritten = FileTraits::write(_fd, sourceBuffer,
                                                sourceByteCount,
                                                errorCode);

        if (errorCode != 0)
        {
            std::string fnName;
            fnName.assign("file.write('");
            appendAgString(fnName, _location.toString(Fs::PathUsage::Kernel));
            fnName.append("', ");
            appendFileSize(FormatInfo::getDisplay(), fnName, sourceByteCount);
            fnName.push_back(')');

            throw FileTraits::createError(fnName, errorCode);
        }

        return bytesWritten;
    }

    // Inherited from IStream.
    virtual bool tryRead(void *targetBuffer, size_t requiredByteCount) override
    {
        bool isOK = false;

        if (_fd != FileTraits::BadFile)
        {
            FileTraits::ErrorCode errorCode;

            size_t bytesRead = FileTraits::read(_fd, targetBuffer,
                                                requiredByteCount,
                                                errorCode);

            isOK = (bytesRead == requiredByteCount);
        }

        return isOK;
    }

    // Inherited from IStream.
    virtual bool tryWrite(const void *sourceBuffer, size_t sourceByteCount) override
    {
        bool isOK = false;

        if (_fd != FileTraits::BadFile)
        {
            FileTraits::ErrorCode errorCode;

            size_t bytesWritten = FileTraits::write(_fd, sourceBuffer,
                                                    sourceByteCount,
                                                    errorCode);

            isOK = (bytesWritten == sourceByteCount);
        }

        return isOK;
    }

    // Inherited from IFileStream.
    virtual const Fs::Path &getPath() const { return _location; }
};

} // Anonymous namespace

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
// IStreamDeleter Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Deletes an IStream implementation embedded in an IStreamUPtr.
//! @param[in] stream The implementation to delete.
void IStreamDeleter::operator()(IStream *stream) const
{
    safeDelete(stream);
}

////////////////////////////////////////////////////////////////////////////////
// IFileStream Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Deletes an IFileStream implementation embedded in an IFileStreamUPtr.
//! @param[in] stream The implementation to delete.
void IFileStream::Deleter::operator()(IFileStream *stream) const
{
    safeDelete(stream);
}

//! @brief Creates a new instance of an IFileStream by opening a file.
//! @param[in] at The location of the file to open.
//! @param[in] access A flags field defined by FileAccess specifying how the file
//! should be opened and managed.
//! @return A unique pointer to an implementation of IFileStream which can be
//! used to access the named file.
IFileStream::UPtr IFileStream::open(const Fs::Path &at, FileAccessBits access)
{
    // Create an absolute path from the outset.
    Fs::Path fullPath = at.convertToAbsolute();

    FileTraits::FileDescriptor fd = FileTraits::open(fullPath, access);

    return IFileStream::UPtr(new FileStream(fullPath, fd));
}

//! @brief Reads the entire contents of a file.
//! @param[in] fileName The path to the file to read.
//! @param[out] fileData Receives the data read from the file.
//! @throws OperationException If the file is larger than 256 MB.
//! @throws Fs::FileNotFoundException If the file cannot be read.
void IFileStream::readWholeFile(const Fs::Path &fileName, ByteBlock &fileData)
{
    fileData.clear();

    Fs::Entry fileInfo(fileName);

    if (fileInfo.exists() == false)
    {
        throw Fs::FileNotFoundException(fileInfo.getPath());
    }

    int64_t size = fileInfo.getSize();

    // Set sensible a maximum size of 256 MB.
    static constexpr int64_t MaxSize = static_cast<int64_t>(256) << 20;

   if ((size < 0) || (size > MaxSize))
    {
        std::string message;
        message.assign("File '");
        appendAgString(message, fileInfo.getPath().toString(Fs::PathUsage::Display));
        message.append("' too large to read into memory all at once.");

        throw OperationException(std::string_view(message));
    }

   fileData.resize(static_cast<size_t>(size));

   FileTraits::FileDescriptor fd = FileTraits::open(fileInfo.getPath(),
                                                    FileAccess::OpenExisting | FileAccess::Read);
   FileStream stream(fileInfo.getPath(), fd);

   size_t actualSize = stream.read(fileData.data(), fileData.size());
   fileData.resize(actualSize);
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


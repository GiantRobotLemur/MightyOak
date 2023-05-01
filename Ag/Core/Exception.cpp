//! @file Core/Exception.cpp
//! @brief The definition of the base exception class used throughout the
//! Ag libraries.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <deque>
#include <set>
#include <vector>

#include "CoreInternal.hpp"
#include "Platform.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/InlineMemory.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"

namespace Ag {

#ifdef _WIN32
//! @brief Attempts to find the full path to a binary module loaded into the
//! current process as a UTF-8 string.
//! @param[in] baseAddr The base address of the module to query.
//! @param[out] filePath Receives the full path to the file.
//! @retval true A valid path was found and converted to UTF-8 characters.
//! @retval false No path could be resolved for the module.
bool tryGetModuleFileName(uintptr_t baseAddr, std::string &filePath)
{
    bool hasPath = false;
    filePath.clear();

    // Try getting the name with a fixed size buffer.
    const DWORD InitBufferSize = 256;
    WCHAR initBuffer[InitBufferSize];

    DWORD result = ::GetModuleFileNameW(reinterpret_cast<HMODULE>(baseAddr),
                                        initBuffer, InitBufferSize);
    if (result > 0)
    {
        if (result == InitBufferSize)
        {
            // Use an expanding buffer until we capture the whole string.
            std::vector<wchar_t> buffer;
            DWORD currentSize = InitBufferSize;

            while (currentSize == result)
            {
                currentSize *= 2;
                buffer.resize(static_cast<size_t>(currentSize), L'\0');

                result = ::GetModuleFileNameW(reinterpret_cast<HMODULE>(baseAddr),
                                              buffer.data(), currentSize);
            }

            Utf::appendWide(filePath, buffer.data(), static_cast<size_t>(result));
        }
        else
        {
            Utf::appendWide(filePath, initBuffer, static_cast<size_t>(result));
        }

        hasPath = true;
    }

    return hasPath;
}
#endif

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A reference counted immutable object containing all information about
//! an exception occurrence with no external dependencies.
class ExceptionPrivate : public std::enable_shared_from_this<ExceptionPrivate>
{
private:
    // Internal Fields
    std::string_view _domain;
    std::string_view _message;
    std::string_view _detail;
    std::string_view _what;
    StackTrace _stackTrace;
    Exception _innerException;
    uintptr_t _errorCode;
    uint8_ptr_t _data;
    bool _isFatal;

public:
    // Construction/Destruction
    //! @brief Constructs an object representing the reference counted set of
    //! self-contained information about a single exception instance.
    //! @param[in] stackTrace An object describing the call hierarchy when the
    //! exception was detected.
    //! @param[in] domain A symbol identifier defining the type of the exception.
    //! @param[in] message A string detailing the nature of the exception type.
    //! @param[in] detail A string providing information specific to this exception
    //! instance.
    //! @param[in] errorCode An optional error code specific to the exception
    //! instance, if one was relevant, otherwise 0.
    ExceptionPrivate(const StackTrace &stackTrace, const std::string_view &domain,
                     const std::string_view &message, const std::string_view &detail,
                     uintptr_t errorCode /* =0 */) :
        _errorCode(errorCode),
        _data(nullptr),
        _isFatal(false)
    {
        InlineAllocator allocator;
        std::string what;

        what.append(domain);
        what.push_back(':');
        what.push_back(' ');
        what.append(message);

        if (detail.empty() == false)
        {
            what.push_back('\n');
            what.append(detail);
        }

        // Allocate space for copies of the string fields.
        InlineField domainField = allocator.allocate(domain);
        InlineField messageField = allocator.allocate(message);
        InlineField detailField = allocator.allocate(detail);
        InlineField whatField = allocator.allocate(what);
        InlineField stackField = allocator.allocateRaw(getStackTraceSize(stackTrace.getData()));

        // Allocate space for all of the data at once.
        _data = reinterpret_cast<uint8_ptr_t>(std::malloc(allocator.getSize()));

        // Copy the data to the memory block.
        InlineInitialiser initialiser(_data, allocator.getSize());

        _domain = std::string_view(initialiser.initialiseField(domainField, domain),
                                   domainField.Count - 1);
        _message = std::string_view(initialiser.initialiseField(messageField, message),
                                    messageField.Count - 1);
        _detail = std::string_view(initialiser.initialiseField(detailField, detail),
                                   detailField.Count - 1);
        _what = std::string_view(initialiser.initialiseField(whatField, what),
                                 whatField.Count - 1);

        // The stack trace data is self-contained, so we can simply perform
        // a binary copy.
        StackTracePrivate *cloneTrace = static_cast<StackTracePrivate *>(initialiser.getFieldData(stackField));

        std::memcpy(cloneTrace, stackTrace.getData(), stackField.Count);

        // By assigning StackTrace with a non-const pointer, the object will take
        // ownership and mark the pointer as not to be deleted at destruction.
        _stackTrace = cloneTrace;
    }

    //! @brief Disposes of the block of memory containing all exception field data.
    ~ExceptionPrivate()
    {
        // Dispose of the blob of exception data.
        if (_data != nullptr)
        {
            std::free(_data);
            _data = nullptr;
        }
    }

    // Accessors
    const std::string_view &getDomain() const { return _domain; }
    const std::string_view &getMessage() const { return _message; }
    const std::string_view &getDetail() const { return _detail; }
    const std::string_view &what() const { return _what; }
    const Exception &getInnerException() const { return _innerException; }
    void setInnerException(const Exception &inner) { _innerException = inner; }
    const StackTrace &getStackTrace() const { return _stackTrace; }
    uintptr_t getErrorCode() const { return _errorCode; }
    uint8_cptr_t getData() const { return _data; }
    bool isFatal() const { return _isFatal; }
    void setFatal() { _isFatal = true; }
};

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
const StackTrace &getEmptyStackTrace()
{
    static const StackTrace emptyTrace;

    return emptyTrace;
}

const Exception &getEmptyException()
{
    static const Exception empty;

    return empty;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets whether the exception object is bound to any underlying data.
//! @retval true The object is bound to exception data. The accessor functions
//! will return useful results.
//! @retval false The object is unbound, the accessor functions will return
//! empty or benign values.
bool Exception::isEmpty() const
{
    return _data.operator bool() == false;
}

//! @brief Gets whether the error is considered recoverable.
//! @retval true The exception could be recovered from.
//! @retval false The exception is definitely fatal to the application.
bool Exception::isFatal() const
{
    return (_data) ? _data->isFatal() : false;
}

//! @brief Gets the error code recorded when an exception was thrown, 0 if there
//! was no relevant error code.
uintptr_t Exception::getErrorCode() const
{
    return (_data) ? _data->getErrorCode() : 0;
}

//! @brief Gets the symbolic type of the exception.
const std::string_view &Exception::getDomain() const
{
    return (_data) ? _data->getDomain() : Utf::getEmpty();
}

//! @brief Gets display text describing the exception type.
const std::string_view &Exception::getMessage() const
{
    return (_data) ? _data->getMessage() : Utf::getEmpty();
}

//! @brief Gets display text giving details of this specific exception occurrence.
const std::string_view &Exception::getDetail() const
{
    return (_data) ? _data->getDetail() : Utf::getEmpty();
}

//! @brief Gets the object describing the call stack when the exception
//! was thrown.
const StackTrace &Exception::getStackTrace() const
{
    return (_data) ? _data->getStackTrace() : getEmptyStackTrace();
}

//! @brief Gets the exception which caused the current one.
//! @returns An exception object which maybe empty.
const Exception &Exception::getInnerException() const
{
    return getEmptyException();
}

//! @brief A member function which renders exceptions compatible with STL
//! exceptions by producing a summary of the information contained.
const char *Exception::what() const noexcept
{
    static const char dummy = '\0';
    return _data ? _data->what().data() : &dummy;
}

//! @brief Copies and packages information about the exception.
//! @param[in] domain The symbolic type of the exception.
//! @param[in] message Display text detailing the exception type.
//! @param[in] detail Display text providing information specific to the current
//! occurrence of this exception type.
//! @param[in] errorCode An optional numeric code associated with the error
//! which caused the exception to be thrown.
//! @details Every type derived from Exception should call this function to
//! embed the exception data in a self-contained and shareable form. A stack trace
//! will be generated assuming the current function was called from an exception
//! constructor.
void Exception::initialise(const std::string_view &domain,
                           const std::string_view &message,
                           const std::string_view &detail,
                           uintptr_t errorCode /* =0 */)
{
    StackTrace stackTrace;
    stackTrace.captureCurrentThread(2);

    _data = std::make_shared<ExceptionPrivate>(stackTrace, domain, message,
                                               detail, errorCode);
}

//! @brief Copies and packages information about the exception.
//! @param[in] callStack The externally captured raw trace of the call stack
//! used to create the stack trace which annotates the exception.
//! @param[in] callStackSize The count of elements in callStack.
//! @param[in] domain The symbolic type of the exception.
//! @param[in] message Display text detailing the exception type.
//! @param[in] detail Display text providing information specific to the current
//! occurrence of this exception type.
//! @param[in] errorCode An optional numeric code associated with the error
//! which caused the exception to be thrown.
//! @details Only hardware exceptions where the stack was captured separately
//! should call this function to embed the exception data in a self-contained and
//! shareable form.
void Exception::initialise(const ActivationRecord *callStack,
                           size_t callStackSize,
                           const std::string_view &domain,
                           const std::string_view &message,
                           const std::string_view &detail,
                           uintptr_t errorCode )
{
    StackTrace stackTrace;
    stackTrace.capture(callStack, callStackSize);

    _data = std::make_shared<ExceptionPrivate>(stackTrace, domain, message,
                                               detail, errorCode);
}

//! @brief Annotates an initialised exception with another exception which directly
//! caused it.
//! @note This member function has no effect is called before initialise().
void Exception::setInnerException(const Exception &inner)
{
    if (_data && (inner._data.get() != _data.get()))
    {
        _data->setInnerException(inner);
    }
}

//! @brief Makes an initialised exception as non-recoverable.
//! @note This member function has no effect is called before initialise().
void Exception::setFatal()
{
    if (_data)
    {
        _data->setFatal();
    }
}

////////////////////////////////////////////////////////////////////////////////
// CustomException Members
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a completely customisable exception type.
//! @param[in] domain The symbolic type of the exception.
//! @param[in] message Display text detailing the exception type.
//! @param[in] detail Display text providing information specific to the current
//! occurrence of this exception type.
//! @param[in] errorCode An optional numeric code associated with the error
//! which caused the exception to be thrown.
CustomException::CustomException(const std::string_view &domain,
                                 const std::string_view &message,
                                 const std::string_view &detail,
                                 uintptr_t errorCode /* = 0 */)
{
    initialise(domain, message, detail, errorCode);
}

////////////////////////////////////////////////////////////////////////////////
// IndexOutOfRangeException Members
////////////////////////////////////////////////////////////////////////////////
const char *IndexOutOfRangeException::Domain = "IndexOutOfRangeException";
static const char *indexOutOfRangeMessage =
    "The program attempted to access an element in a linear collection which "
    "was beyond its bounds.";

//! @brief Constructs an exception documenting failed access to an indexed collection.
//! @param[in] index The index used to access the collection.
//! @param[in] range The count of elements in the collection being accessed.
IndexOutOfRangeException::IndexOutOfRangeException(intptr_t index, intptr_t range)
{
    std::string detail;
    appendPrintf(detail, "The program attempted to access index %jd in a "
                 "collection of %jd items.", index, range);
    initialise(Domain, indexOutOfRangeMessage, detail);
}

//! @brief Constructs an exception documenting failed access to an indexed collection.
//! @param[in] index The index used to access the collection.
//! @param[in] range The count of elements in the collection being accessed.
IndexOutOfRangeException::IndexOutOfRangeException(uintptr_t index, intptr_t range)
{
    std::string detail;
    appendPrintf(detail, "The program attempted to access index %ju in a "
                 "collection of %jd items.", index, range);
    initialise(Domain, indexOutOfRangeMessage, detail);
}

//! @brief Constructs an exception documenting failed access to an indexed collection.
//! @param[in] index The index used to access the collection.
//! @param[in] range The count of elements in the collection being accessed.
IndexOutOfRangeException::IndexOutOfRangeException(intptr_t index, uintptr_t range)
{
    std::string detail;
    appendPrintf(detail, "The program attempted to access index %jd in a "
                 "collection of %ju items.", index, range);
    initialise(Domain, indexOutOfRangeMessage, detail);
}

//! @brief Constructs an exception documenting failed access to an indexed collection.
//! @param[in] index The index used to access the collection.
//! @param[in] range The count of elements in the collection being accessed.
IndexOutOfRangeException::IndexOutOfRangeException(uintptr_t index, uintptr_t range)
{
    std::string detail;
    appendPrintf(detail, "The program attempted to access index %ju in a "
                 "collection of %ju items.", index, range);
    initialise(Domain, indexOutOfRangeMessage, detail);
}

////////////////////////////////////////////////////////////////////////////////
// DivisionByZeroException Members
////////////////////////////////////////////////////////////////////////////////
const char *DivisionByZeroException::Domain = "DivisionByZeroException";
static const char *divisionByZeroMessage =
    "The program detected an attempt to perform a division with a "
    "denominator of zero.";

//! @brief A constructor used explicitly when logic within the program detects
//! that it is about to try to perform a division with a denominator of zero or
//! a value with a magnitude which is very small.
//! @param[in] detail Provides some context for the operation which was being
//! checked.
DivisionByZeroException::DivisionByZeroException(const std::string_view &detail)
{
    initialise(Domain, divisionByZeroMessage, detail);
}

////////////////////////////////////////////////////////////////////////////////
// ArithmeticException Members
////////////////////////////////////////////////////////////////////////////////
const char *ArithmeticException::Domain = "ArithmeticException";
static const char *arithmeticMessage =
    "The program detected an attempt to perform an invalid numeric operation.";

//! @brief A constructor used explicitly when logic within the program detects
//! that it is about to try to perform an erroneous numeric operation.
//! @param[in] detail Provides some context for the operation which was being
//! checked.
ArithmeticException::ArithmeticException(const std::string_view &detail)
{
    initialise(Domain, arithmeticMessage, detail);
}

////////////////////////////////////////////////////////////////////////////////
// OperationException Members
////////////////////////////////////////////////////////////////////////////////
const char *OperationException::Domain = "OperationException";
static const char *operationMessage =
"The program detected an attempt to perform an operation which was invalid.";

//! @brief A constructor used explicitly when logic within the program detects
//! that it is about to try to perform an invalid operation.
//! @param[in] detail Provides some context for the operation which was being
//! checked.
OperationException::OperationException(utf8_cptr_t detail)
{
    initialise(Domain, operationMessage, detail);
}

//! @brief Constructs an exception to wrap another indicating that an operation
//! was invalid.
//! @param[in] detail Provides some context for the operation which failed.
//! @param[in] inner The lower-level failure which indicated the operation
//! was invalid.
OperationException::OperationException(utf8_cptr_t detail, const Exception &inner)
{
    initialise(Domain, operationMessage, detail);

    setInnerException(inner);
}


//! @brief A constructor used explicitly when logic within the program detects
//! that it is about to try to perform an invalid operation.
//! @param[in] detail Provides some context for the operation which was being
//! checked.
OperationException::OperationException(const std::string_view &detail)
{
    initialise(Domain, operationMessage, detail);
}

//! @brief Constructs an exception to wrap another indicating that an operation
//! was invalid.
//! @param[in] detail Provides some context for the operation which failed.
//! @param[in] inner The lower-level failure which indicated the operation
//! was invalid.
OperationException::OperationException(const std::string_view &detail,
                                       const Exception &inner)
{
    initialise(Domain, operationMessage, detail);

    setInnerException(inner);
}

//! @brief A constructor used explicitly when logic within the program detects
//! that it is about to try to perform an invalid operation.
//! @param[in] detail Provides some context for the operation which was being
//! checked.
OperationException::OperationException(const String &detail)
{
    initialise(Domain, operationMessage,
               std::string_view(detail.getUtf8Bytes(),
                                detail.getUtf8Length()));
}

//! @brief Constructs an exception to wrap another indicating that an operation
//! was invalid.
//! @param[in] detail Provides some context for the operation which failed.
//! @param[in] inner The lower-level failure which indicated the operation
//! was invalid.
OperationException::OperationException(const String &detail,
                                       const Exception &inner)
{
    initialise(Domain, operationMessage,
               std::string_view(detail.getUtf8Bytes(),
                                detail.getUtf8Length()));

    setInnerException(inner);
}

////////////////////////////////////////////////////////////////////////////////
// ArgumentException Members
////////////////////////////////////////////////////////////////////////////////
const char *ArgumentException::Domain = "ArgumentException";
static const char *argumentMessage =
"A function was passed an argument with an unexpected value.";

//! @brief A constructor used explicitly when logic within the program detects
//! that the argument to a function is invalid.
//! @param[in] argName The name of the argument with the invalid value.
ArgumentException::ArgumentException(const char *argName)
{
    std::string detail;
    appendPrintf(detail, "The function argument '%s' had an invalid value.",
                 argName);

    initialise(Domain, argumentMessage, detail);
}

////////////////////////////////////////////////////////////////////////////////
// BadCastException Members
////////////////////////////////////////////////////////////////////////////////
const char *BadCastException::Domain = "BadCastException";
static const char *badCastMessage =
"The program attempted to cast a base type to a derived type which it did not instantiate.";

//! @brief A constructor used explicitly when a dynamic cast fails.
//! @param[in] targetTypeName The name of the target data type the failed cast
//! was attempting to convert to.
BadCastException::BadCastException(const char *targetTypeName)
{
    std::string detail;
    appendPrintf(detail, "The source instance did not derive from the '%s' data type.",
                 targetTypeName);

    initialise(Domain, badCastMessage, detail);
}

////////////////////////////////////////////////////////////////////////////////
// ObjectNotBoundException Members
////////////////////////////////////////////////////////////////////////////////
const char *ObjectNotBoundException::Domain = "ObjectNotBoundException";
static const char *objNotBoundMessage =
"The program perform an operation on an object not bound to a valid underlying resource.";

//! @brief Constructs an exception throw when an attempt is made use a resource
//! wrapper object not bound to an underlying resource.
//! @param[in] objectTypeName The type of resource the object should be bound to.
//! @param[in] operation The operation which the program attempted to perform.
ObjectNotBoundException::ObjectNotBoundException(const char *objectTypeName,
                                                 const char *operation)
{
    std::string detail;
    appendPrintf(detail, "An '%s' object could not perform the '%s' function"
                 "as it was in an unbound state.",
                 objectTypeName, operation);


    initialise(Domain, objNotBoundMessage, detail);
}

////////////////////////////////////////////////////////////////////////////////
// NotSupportedException Members
////////////////////////////////////////////////////////////////////////////////
const char *NotSupportedException::Domain = "NotSupportedException";
static const char *notSupportedMessage =
"The program attempted to use a feature which was not supported in the current context.";
static const char *notSupportedDetail = "%s is not supported.";

//! @brief A constructor used to indicate a feature cannot be used.
//! @param[in] feature The name of the feature.
NotSupportedException::NotSupportedException(const char *feature)
{
    std::string detail;
    appendPrintf(detail, notSupportedDetail, feature);

    initialise(Domain, notSupportedMessage, detail);
}

//! @brief A constructor used to indicate a feature cannot be used.
//! @param[in] feature The name of the feature.
//! @param[in] inner The inner exception which indicated the feature was not present.
NotSupportedException::NotSupportedException(const char *feature, const Exception &inner)
{
    std::string detail;
    appendPrintf(detail, notSupportedDetail, feature);

    initialise(Domain, notSupportedMessage, detail);

    setInnerException(inner);
}

#ifdef _WIN32
////////////////////////////////////////////////////////////////////////////////
// Win32Exception Members
////////////////////////////////////////////////////////////////////////////////
const char *Win32Exception::Domain = "Win32Exception";
static const char *win32Message =
"A Win32 API function returned an unexpected error condition.";

//! @brief Constructs an exception object documenting the failure of a function
//! from the Win32 API.
//! @param[in] fnName The name of the function including parenthesis and
//! optionally a summary of pertinent parameters.
//! @param[in] win32ErrorCode The error code returned by GetLastError().
Win32Exception::Win32Exception(const char *fnName, uint32_t win32ErrorCode)
{
    LPWSTR messageBuffer = nullptr;

    uint32_t length = ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                       FORMAT_MESSAGE_FROM_SYSTEM |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                       nullptr, win32ErrorCode,
                                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                       reinterpret_cast<LPWSTR>(&messageBuffer),
                                       0, nullptr);

    std::string detail;
    appendPrintf(detail, "The function '%s' returned an error", fnName);

    if (length > 0)
    {
        detail.push_back(':');
        detail.push_back(' ');
        Utf::appendWide(detail, messageBuffer, length);

        ::LocalFree(messageBuffer);
        messageBuffer = nullptr;
    }
    else
    {
        detail.push_back('.');
    }

    initialise(Domain, win32Message, detail);
}
#endif // ifdef _WIN32

////////////////////////////////////////////////////////////////////////////////
// Win32Exception Members
////////////////////////////////////////////////////////////////////////////////
const char *RuntimeLibraryException::Domain = "RuntimeLibraryException";
static const char *rtErrorMessage =
"A C runtime library function returned an unexpected error condition.";

//! @brief Constructs an exception object documenting the failure of a function
//! from the platform C runtime library.
//! @param[in] fnName The name of the function including parenthesis and
//! optionally a summary of pertinent parameters.
//! @param[in] runtimeErrorCode The error code returned by the function or
//! extracted via errno.
RuntimeLibraryException::RuntimeLibraryException(const char *fnName,
                                                 RuntimeLibraryException::ErrorCode runtimeErrorCode)
{
    std::string detail;
    appendPrintf(detail, "The function '%s' returned an error", fnName);

#ifdef _MSC_VER
    std::vector<wchar_t> buffer;
    buffer.resize(256, L'\0');

    if (_wcserror_s(buffer.data(), buffer.size(), runtimeErrorCode) == 0)
    {
        // Append the error message text encoded as UTF-8 bytes.
        detail.push_back(':');
        detail.push_back(' ');
        Utf::appendWide(detail, buffer.data(), wcslen(buffer.data()));
    }
#else
    // Create a temporary buffer to hold a thread-safe copy of the system
    // error message.
    std::vector<char> buffer;
    buffer.resize(256, '\0');

    char *result = strerror_r(runtimeErrorCode, buffer.data(), buffer.size());

    if (result != nullptr)
    {
        detail.push_back(':');
        detail.push_back(' ');
        detail.append(result);
    }
#endif
    else
    {
        detail.push_back('.');
    }

    initialise(Domain, rtErrorMessage, detail);
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


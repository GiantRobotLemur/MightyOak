//! @file Ag/Core/Exception.hpp
//! @brief The declaration of basic Exception types used throughout the library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details..
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_EXCEPTION_HPP__
#define __AG_CORE_EXCEPTION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <exception>
#include <memory>
#include <string_view>

#include "Configuration.hpp"
#include "StackTrace.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ExceptionPrivate;
class String;

//! @defgroup CoreExceptions Core Exceptions
//! @brief All exceptions in the Silver suite of libraries are derived from
//! Ag::Exception.
//! @details
//! All exceptions are based on the Ag::Exception class which is a shared
//! reference to inner immutable exception data created during construction.
//! Each exception has a domain (essentially a text type identifier) a message,
//! detail text, a stack trace giving the location it was thrown from and an
//! optional error code.

//! @addtogroup CoreExceptions
//! @{

//! @brief The base class for all exceptions thrown by the Ag libraries.
class Exception : public std::exception
{
public:
    // Construction/Destruction
    Exception() = default;
    ~Exception() = default;

    // Accessors
    bool isEmpty() const;
    bool isFatal() const;
    uintptr_t getErrorCode() const;
    const std::string_view &getDomain() const;
    const std::string_view &getMessage() const;
    const std::string_view &getDetail() const;
    const StackTrace &getStackTrace() const;
    const Exception &getInnerException() const;

    // Overrides
    virtual const char *what() const noexcept override;
protected:
    // Operations
    void initialise(const std::string_view &domain, const std::string_view &message,
                    const std::string_view &detail, uintptr_t errorCode = 0);
    void initialise(const ActivationRecord *callStack, size_t callStackSize,
                    const std::string_view &domain, const std::string_view &message,
                    const std::string_view &detail, uintptr_t errorCode);
    void setInnerException(const Exception &inner);
    void setFatal();

private:
    // Internal Fields
    std::shared_ptr<ExceptionPrivate> _data;
};

//! @brief An exception thrown in unique situations were a fixed exception type
//! is not required.
class CustomException : public Exception
{
public:
    CustomException(const std::string_view &domain, const std::string_view &message,
                    const std::string_view &detail, uintptr_t errorCode = 0);
};

//! @brief An exception documenting failed access to an indexed collection.
class IndexOutOfRangeException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    IndexOutOfRangeException(intptr_t index, intptr_t range);
    IndexOutOfRangeException(uintptr_t index, intptr_t range);
    IndexOutOfRangeException(intptr_t index, uintptr_t range);
    IndexOutOfRangeException(uintptr_t index, uintptr_t range);
    ~IndexOutOfRangeException() = default;
};

//! @brief An exception documenting an attempt to divide by zero.
class DivisionByZeroException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    DivisionByZeroException(const std::string_view &detail);
    ~DivisionByZeroException() = default;
};

//! @brief An exception documenting a general arithmetic error.
class ArithmeticException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    ArithmeticException(const std::string_view &detail);
    ~ArithmeticException() = default;
};

//! @brief An exception documenting attempted use of an operation which
//! was invalid.
class OperationException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    OperationException(utf8_cptr_t detail);
    OperationException(utf8_cptr_t detail, const Exception &inner);
    OperationException(const std::string_view &detail);
    OperationException(const std::string_view &detail, const Exception &inner);
    OperationException(const String &detail);
    OperationException(const String &detail, const Exception &inner);
    ~OperationException() = default;
};

//! @brief An exception thrown when a function argument has an invalid value.
class ArgumentException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    ArgumentException(const char *argName);
    ~ArgumentException() = default;
};

//! @brief An exception thrown when an attempt is made to cast a base type to
//! a derived type which it does not instantiate.
class BadCastException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    BadCastException(const char *targetTypeName);
    ~BadCastException() = default;
};

//! @brief An exception thrown when an attempt is made perform an operation on
//! a wrapper object not bound to an underlying resource.
class ObjectNotBoundException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    ObjectNotBoundException(const char *objectTypeName, const char *operation);
    ~ObjectNotBoundException() = default;
};

//! @brief An exception thrown when an attempt is made to use functionality
//! which is not supported in the current context.
class NotSupportedException : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    NotSupportedException(const char *feature);
    NotSupportedException(const char *feature, const Exception &inner);
    ~NotSupportedException() = default;
};

#ifdef _WIN32
//! @brief An exception thrown when a Win32 API function returns a failure
//! condition.
class Win32Exception : public Exception
{
public:
    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    Win32Exception(const char *fnName, uint32_t win32ErrorCode);
    ~Win32Exception() = default;
};
#endif // ifdef _WIN32

//! @brief An exception describing an unexpcted result from the platform C
//! runtime library (glibc on POSIX systems or MSVCRT on Windows/MSVC).
class RuntimeLibraryException : public Exception
{
public:
    // Public Types
#ifdef _MSC_VER
    using ErrorCode = errno_t;
#else
    using ErrorCode = int;
#endif

    // Public Data
    //! @brief The domain used to annotate exceptions of this type.
    static const char *Domain;

    // Construction
    RuntimeLibraryException(const char *fnName, ErrorCode runtimeErrorCode);
    ~RuntimeLibraryException() = default;
};

//! @}

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

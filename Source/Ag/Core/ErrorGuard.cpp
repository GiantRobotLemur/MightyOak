//! @file Core/ErrorGuard.cpp
//! @brief The definition of an object which acts as a catch block for both
//! C++ and hardware exceptions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#ifdef _WIN32
#include "Win32API.hpp"
#else
#include <csetjmp>
#include <csignal>
#include <execinfo.h>
#endif

#include "Ag/Core/ErrorGuard.hpp"
#include "Ag/Core/StackTrace.hpp"
#include "Ag/Core/Utils.hpp"
#include "CoreInternal.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
namespace {

#ifdef _WIN32
//! @brief An object which adapts Win32-specific SEH exception information
//! into an Ag::Exception object.
class Win32StructuredException : public Exception
{
public:
    //! @brief Constructs an Ag::Exception from Win32 SEH exception information.
    //! @param[in] processorState The captured state or the processor when the
    //! exception was trapped.
    //! @param[in] info Details of the trapped SEH fault.
    //! @param[in] callStack The function activation records captured from the
    //! state of the processor when the exception was trapped.
    //! @param[in] callStackSize The count of elements in callStack.
    Win32StructuredException(const CONTEXT & /* processorState */,
                             const EXCEPTION_RECORD &info,
                             const  ActivationRecord *callStack,
                             size_t callStackSize)
    {
        const char *domain = "HardwareException";
        const char *message = "An unknown processor exception was detected.";
        std::string detail;
        bool isFatal = (info.ExceptionFlags & EXCEPTION_NONCONTINUABLE) != 0;
        const int addrHexDigits = static_cast<int>(sizeof(uintptr_t) * 2);

        switch (info.ExceptionCode)
        {
        case EXCEPTION_ACCESS_VIOLATION:
            domain = "AddressException";
            message = "The program attempted to access an invalid memory location.";

            switch (info.ExceptionInformation[0])
            {
            case 0:
                detail.assign("Memory was read from");
                break;

            case 1:
                detail.assign("Memory was written to");
                break;

            case 8:
                detail.assign("Code was executed from");
                break;

            default:
                detail.assign("Memory was accessed at");
                break;
            }

            appendPrintf(detail, " 0x%.*zX.", addrHexDigits, info.ExceptionInformation[1]);
            break;

        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            domain = IndexOutOfRangeException::Domain;
            message = "The program tried to access an array element that is out of bounds "
                "and the underlying hardware supports bounds checking.";
            appendPrintf(detail, "The instruction which accessed the array was at 0x%.*zX.",
                         addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_BREAKPOINT:
            domain = "DebugException";
            message = "A breakpoint was encountered.";
            appendPrintf(detail, "The breakpoint was at 0x%.*zX.",
                         addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_DATATYPE_MISALIGNMENT:
            domain = "AddressException";
            message = "The program tried to read or write data that is misaligned "
                "on hardware that does not provide alignment.";

            appendPrintf(detail, "The instruction which accessed the data was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_FLT_DENORMAL_OPERAND:
            domain = ArithmeticException::Domain;
            message = "One of the operands in a floating-point operation is "
                "denormal (too small to represent as a standard "
                "floating-point value).";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            domain = DivisionByZeroException::Domain;
            message = "The program tried to divide a floating-point value by "
                "a divisor of zero.";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_FLT_INEXACT_RESULT:
            domain = ArithmeticException::Domain;
            message = "The result of a floating-point operation cannot be "
                "represented exactly as a decimal fraction.";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_FLT_INVALID_OPERATION:
            domain = ArithmeticException::Domain;
            message = "The floating-point processor attempted to perform an "
                "invalid operation.";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_FLT_OVERFLOW:
            domain = ArithmeticException::Domain;
            message = "The exponent of a floating-point operation was greater "
                "than the magnitude allowed by the corresponding type.";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_FLT_STACK_CHECK:
            domain = ArithmeticException::Domain;
            message = "The register stack overflowed or underflowed as the "
                "result of a floating-point operation.";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_FLT_UNDERFLOW:
            domain = ArithmeticException::Domain;
            message = "The exponent of a floating-point operation is less than "
                "the magnitude allowed by the corresponding type.";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_ILLEGAL_INSTRUCTION:
            domain = OperationException::Domain;
            message = "The program tried to execute an invalid instruction.";

            appendPrintf(detail, "The instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_IN_PAGE_ERROR:
            domain = "AddressException";
            message = "The program attempted to access a memory page which was "
                "not present and the system was unable to load that page.";
            detail.assign("The program was trying to ");

            switch (info.ExceptionInformation[0])
            {
            case 0: detail.append("read data from"); break;
            case 1: detail.append("write data to"); break;
            case 8: detail.append("execute code at"); break;
            }

            appendPrintf(detail, " 0x%.*zX.", addrHexDigits,
                         info.ExceptionInformation[1]);
            break;

        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            domain = DivisionByZeroException::Domain;
            message = "The program tried to divide an integer value by an "
                "integer divisor of zero.";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_INT_OVERFLOW:
            domain = ArithmeticException::Domain;
            message = "The result of an integer operation caused a carry out "
                "of the most significant bit of the result.";

            appendPrintf(detail, "The arithmetic instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_INVALID_DISPOSITION:
            domain = OperationException::Domain;
            message = "An exception handler returned an invalid disposition "
                "to the exception dispatcher.";

            appendPrintf(detail, "The instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_PRIV_INSTRUCTION:
            domain = OperationException::Domain;
            message = "The program tried to execute an instruction whose "
                "operation is not allowed in an unprivileged processor mode.";

            appendPrintf(detail, "The instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_SINGLE_STEP:
            domain = "DebugException";
            message = "A trace trap or other single-instruction mechanism "
                "signalled that one instruction has been executed.";
            appendPrintf(detail, "The following instruction was at 0x%.*zX.",
                         addrHexDigits, info.ExceptionAddress);
            break;

        case EXCEPTION_STACK_OVERFLOW:
            domain = OperationException::Domain;
            message = "A thread within the program used up its stack.";

            appendPrintf(detail, "The instruction at fault was "
                         "at 0x%.*zX.", addrHexDigits, info.ExceptionAddress);
            break;

        default:
            appendPrintf(detail, "An SEH Exception Code: 0x%.8X.",
                         info.ExceptionCode);
            break;
        }

        // Populate the exception with data.
        initialise(callStack, callStackSize, domain, message,
                   detail, info.ExceptionCode);

        if (isFatal)
        {
            // Mark the error as non-recoverable.
            setFatal();
        }
    }
};

//! @brief The Win32-specific implementation of IErrorGuardContext.
class ErrorGuardContext : public IErrorGuardContext
{
private:
    static const size_t MaxCallStack = 256;

    CONTEXT _processorState;
    EXCEPTION_RECORD _exceptionInfo;
    ActivationRecord _callStack[MaxCallStack];
    size_t _callStackSize;
    bool _exceptionPending;
public:
    ErrorGuardContext() :
        _callStackSize(0),
        _exceptionPending(false)
    {
        zeroFill(_processorState);
        zeroFill(_exceptionInfo);
        zeroFill(_callStack);
    }

    virtual ~ErrorGuardContext() = default;

    // Accessors
    //! @brief Determines if we should be trying to handle the specified
    //! exception type.
    //! @param[in] sehType The type of structured exception caught.
    //! @retval true The exception should be handled.
    //! @retval false The exception cannot be handled.
    static bool isCatchableSEH(uint32_t sehType)
    {
        static bool isInitialised = false;
        static uint32_t allowedTypes[] = {
            EXCEPTION_ACCESS_VIOLATION,
            EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
            EXCEPTION_BREAKPOINT,
            EXCEPTION_DATATYPE_MISALIGNMENT,
            EXCEPTION_FLT_DENORMAL_OPERAND,
            EXCEPTION_FLT_DIVIDE_BY_ZERO,
            EXCEPTION_FLT_INEXACT_RESULT,
            EXCEPTION_FLT_INVALID_OPERATION,
            EXCEPTION_FLT_OVERFLOW,
            EXCEPTION_FLT_STACK_CHECK,
            EXCEPTION_FLT_UNDERFLOW,
            EXCEPTION_ILLEGAL_INSTRUCTION,
            EXCEPTION_IN_PAGE_ERROR,
            EXCEPTION_INT_DIVIDE_BY_ZERO,
            EXCEPTION_INT_OVERFLOW,
            EXCEPTION_INVALID_DISPOSITION,
            EXCEPTION_PRIV_INSTRUCTION,
            EXCEPTION_SINGLE_STEP,
            EXCEPTION_STACK_OVERFLOW,
        };

        // Calculate the end iterator for the static array.
        constexpr size_t count = std::size(allowedTypes);
        uint32_t *allowedTypesEnd = allowedTypes + count;

        if (isInitialised == false)
        {
            // Ensure the values are sorted so that we can use binary search in all
            // subsequent calls.
            isInitialised = true;
            std::sort(allowedTypes, allowedTypesEnd);
        }

        // See if the exception type is in the sorted set.
        return std::binary_search(allowedTypes, allowedTypesEnd, sehType);
    }

    // Operations
    //! @brief Captures information and state when a Structured Exception
    //! is detected.
    //! @param[in] exceptionPointers The processor state and information
    //! about this specific exception instance.
    void captureSEH(const EXCEPTION_POINTERS *exceptionPointers)
    {
        if (exceptionPointers != nullptr)
        {
            // Capture the exception data, processor state and call stack.
            _processorState = *exceptionPointers->ContextRecord;
            _exceptionInfo = *exceptionPointers->ExceptionRecord;

            // Break any chain of exceptions which may have existed.
            _exceptionInfo.ExceptionRecord = nullptr;

            // Capture the call stack using the processor state.
            captureActivationRecords(exceptionPointers->ContextRecord,
                                     _callStack, MaxCallStack, 0);

            // Flag that we have information pending which needs to be converted
            // into an Ag::Exception-derived object.
            _exceptionPending = true;
        }
    }

    // Overrides
    // Inherited from IErrorGuardContext.
    virtual void beforeExec() override
    {
        // Reset in preparation to catch the next hardware exception.
        _exceptionPending = false;
    }

    // Inherited from IErrorGuardContext.
    virtual void afterExec(Exception &error) override
    {
        if (_exceptionPending)
        {
            // Convert the raw SEH data to an Ag::Exception-derived object.
            error = Win32StructuredException(_processorState,
                                             _exceptionInfo,
                                             _callStack, _callStackSize);
        }
    }
};
#else
class POSIXHardwareException : public Exception
{
public:
    POSIXHardwareException(const ActivationRecord *callStack, size_t callStackSize,
                           int signalId, const siginfo_t &signalInfo)
    {
        // Populate the exception with data.
        const int addrHexDigits = sizeof(void *) * 2;
        const char *domain = "HardwareException";
        const char *message = "A low-level exception was trapped.";
        std::string detail;
        bool isFatal = false;

        switch(signalId)
        {
        case SIGILL: // Illegal instruction.
            domain = OperationException::Domain;
            message = "The program tried to execute an illegal instruction.";

            switch (signalInfo.si_code)
            {
            case ILL_ILLOPC:
                detail.assign("The instruction had an illegal op code");
                break;

            case ILL_ILLOPN:
                detail.assign("The instruction had an illegal operand");
                break;

            case ILL_ILLADR:
                detail.assign("The instruction had an illegal addressing mode");
                break;

            case ILL_ILLTRP:
                detail.assign("The instruction triggered an illegal trap");
                break;

            case ILL_PRVOPC:
                detail.assign("The instruction used a privilaged op code");
                break;

            case ILL_PRVREG:
                detail.assign("The instruction accessed a privilaged register");
                break;

            case ILL_COPROC:
                detail.assign("The instruction caused an error in a co-processor");
                break;

            case ILL_BADSTK:
                detail.assign("The instruction caused an internal stack error");
                break;

            default:
                detail.assign("The instruction which failed was");
            }

            appendPrintf(detail, " at 0x%.*zX.", addrHexDigits,
                         signalInfo.si_addr);
            break;

        case SIGFPE: // Floating point exception.
            switch (signalInfo.si_code)
            {
            case FPE_INTDIV:
                domain = DivisionByZeroException::Domain;
                message = "The program tried to divide a floating-point value by "
                          "a divisor of zero.";
                break;

            case FPE_INTOVF:
                domain = ArithmeticException::Domain;
                message = "The result of an integer operation caused a carry out "
                          "of the most significant bit of the result.";
                break;

            case FPE_FLTDIV:
                domain = DivisionByZeroException::Domain;
                message = "The program tried to divide a floating-point value by "
                          "a divisor of zero.";
                break;

            case FPE_FLTOVF:
                domain = ArithmeticException::Domain;
                message = "The exponent of a floating-point operation was greater "
                          "than the magnitude allowed by the corresponding type.";
                break;

            case FPE_FLTUND:
                domain = ArithmeticException::Domain;
                message = "The exponent of a floating-point operation is less than "
                          "the magnitude allowed by the corresponding type.";
                break;

            case FPE_FLTRES:
                domain = ArithmeticException::Domain;
                message = "The result of a floating-point operation cannot be "
                          "represented exactly as a decimal fraction.";
                break;

            case FPE_FLTSUB:
                domain = ArithmeticException::Domain;
                message = "The floating-point processor attempted to access a "
                          "subscript which was out of range.";
                break;

            case FPE_FLTINV:
            default:
                domain = ArithmeticException::Domain;
                message = "The floating-point processor attempted to perform an "
                          "invalid operation.";
                break;
            }

            appendPrintf(detail, "The arithmetic instruction at fault was "
                                 "at 0x%.*zX.", addrHexDigits,
                         signalInfo.si_addr);
            break;

        case SIGSEGV: // Invalid memory reference
            domain = "AddressException";

            switch (signalInfo.si_code)
            {
            case SEGV_MAPERR:
                message = "The program attempted to access and address which "
                          "was not mapped to an object.";
                break;

            case SEGV_ACCERR:
                message = "The program did not have the appropriate permissions "
                          "to access an address.";
                break;

            case SEGV_BNDERR:
                message = "The program tried to access an array element that is "
                          "out of bounds and the underlying hardware supports "
                          "bounds checking.";
                break;

            case SEGV_PKUERR:
                message = "The program was denied access to an address by "
                          "memory protection keys.";
                break;

            default:
                message = "The program attempted to access an invalid memory location.";
                break;
            }

            appendPrintf(detail, "Invalid memory access at: 0x%.*zX.",
                         addrHexDigits, signalInfo.si_addr);
            break;

        case SIGBUS: // Bus error (bad memory access).
            domain = "AddressException";

            switch (signalInfo.si_code)
            {
            case BUS_ADRALN:
                message = "The program attempted to access a value in memory "
                          "stored on an invalid address boundary.";
                break;

            case BUS_ADRERR:
                message = "The program attempted to access a virtual address "
                          "which was mapped to a non-existent physical address.";
                break;

            case BUS_OBJERR:
                message = "An object-specific hardware bus error occurred.";
                break;

            case BUS_MCEERR_AR:
                message = "A hardware memory error was detected on machine check.";
                break;

            case BUS_MCEERR_AO:
                message = "A hardware memory error was detected by the progress.";
                break;

            default:
                message = "A hardware-related memory error occurred.";
                break;
            }

            appendPrintf(detail, "The error occurred at: 0x%.*zX.",
                         addrHexDigits, signalInfo.si_addr);
            break;

        case SIGABRT: // abort() called.
            domain = OperationException::Domain;
            message = "The program called for an abnormal termination.";
            isFatal = true;
            break;

        default:
            domain = OperationException::Domain;
            message = "The program detected an unknown error.";

            appendPrintf(detail, "The signal raised was: %d.", signalId);
            isFatal = true;
            break;
        }

        initialise(callStack, callStackSize, domain, message,
                   detail, signalId);

        if (isFatal)
        {
            // Mark the error as non-recoverable.
            setFatal();
        }
    }
};

typedef void (*SignalHandlerFn)(int, siginfo_t *, void *);

//! @brief The POSIX-specific implementation of IErrorGuardContext.
class ErrorGuardContext : public IErrorGuardContext
{
private:
    // Internal Fields
    static const size_t MaxCallStack = 256;
    static const size_t MaxSignalCount = 8;
    static constexpr const int SignalIds[] =
    {
        SIGILL, // Illegal instruction.
        SIGFPE, // Floating point exception.
        SIGSEGV, // Invalid memory reference
        SIGBUS, // Bus error (bad memory access).
        SIGABRT, // abort() called.
    };
    struct sigaction _savedSignals[MaxSignalCount];
    int _savedSignalsOverride[MaxSignalCount];
    void *_callStackBuffer[MaxCallStack];
    siginfo_t _capturedSignal;
    size_t _callStackSize;
    int _capturedSignalId;
    bool _signalHandled;

    // Internal Functions
public:
    ErrorGuardContext(SignalHandlerFn errorGuardSignalHandler) :
        _callStackSize(0),
        _capturedSignalId(0),
        _signalHandled(false)
    {
        static_assert(std::size(SignalIds) <= MaxSignalCount,
                      "ErrorGuardContext::MaxSignalCount needs to be increased.");

        zeroFill(_savedSignals);
        zeroFill(_savedSignalsOverride);
        zeroFill(_callStackBuffer);
        zeroFill(_capturedSignal);

        // Setup handlers for key hardware signals, saving the state of the
        // previous handlers.
        struct sigaction signalConfig;
        zeroFill(signalConfig);
        signalConfig.sa_sigaction = errorGuardSignalHandler;
        sigemptyset(&signalConfig.sa_mask);
        signalConfig.sa_flags = SA_NODEFER | SA_SIGINFO;

        for (size_t index = 0, count = std::size(SignalIds); index < count; ++index)
        {
            _savedSignalsOverride[index] = sigaction(SignalIds[index],
                                                     &signalConfig,
                                                     _savedSignals + index);
        }
    }

    virtual ~ErrorGuardContext()
    {
        // Restore the states if claimed signals.
        for (size_t index = 0, count = std::size(SignalIds); index < count; ++index)
        {
            if (_savedSignalsOverride[index] == 0)
            {
                sigaction(SignalIds[index], _savedSignals + index, nullptr);
                _savedSignalsOverride[index] = -1;
            }
        }
    }

    void recordSignal(int sigNum, const siginfo_t *action)
    {
        if (action != nullptr)
        {
            // Capture the call stack.
            _callStackSize = backtrace(_callStackBuffer, MaxCallStack);

            _capturedSignal = *action;
            _capturedSignalId = sigNum;
            _signalHandled = true;
        }
    }

    // Inherited from IErrorGuardContext.
    virtual void beforeExec() override
    {
        _signalHandled = false;
    }

    // Inherited from IErrorGuardContext.
    virtual void afterExec(Exception &error) override
    {
        if (_signalHandled)
        {
            // Convert the recorded call stack into something compatible with
            // the StackTrace class.
            const size_t pruneCount = 1;
            std::vector<ActivationRecord> stackRecords;

            stackRecords.reserve(std::max(_callStackSize, pruneCount) - pruneCount);
            ActivationRecord record;
            record.ModuleBase = 0;
            record.Offset = 0;

            for (size_t index = pruneCount; index < _callStackSize; ++index)
            {
                record.Offset = reinterpret_cast<uintptr_t>(_callStackBuffer[index]);

                stackRecords.push_back(record);
            }

            // Construct an exception from the captured signal and stack data.
            error = POSIXHardwareException(stackRecords.data(), stackRecords.size(),
                                           _capturedSignalId, _capturedSignal);
        }
    }
};

#endif

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Data
////////////////////////////////////////////////////////////////////////////////
thread_local ErrorGuard *ErrorGuard::_currentGuard = nullptr;

//! @brief The domain used to annotate hardware-related errors detected by an
//! ErrorGuard during execution.
const char *ErrorGuard::HardwareDomain = "HardwareException";

//! @brief The domain used to annotate memory access-related errors detected
//! by an ErrorGuard during execution.
const char *ErrorGuard::AddressDomain = "AddressException";

//! @brief The domain used to annotate debug-related errors detected by an
//! ErrorGuard during execution.
const char *ErrorGuard::DebugDomain = "DebugException";

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an error guard ready to record errors on the current
//! thread.
ErrorGuard::ErrorGuard() :
    _previousGuard(_currentGuard),
    _rootContext(nullptr)
{
    if (_currentGuard == nullptr)
    {
        // Create a context in the least nested guard which is shared between
        // all guards on the same thread.
#ifdef _WIN32
        _rootContext = new ErrorGuardContext();
#else
        _rootContext = new ErrorGuardContext(onLowLevelExceptionTrap);
#endif

    }
    else
    {
        // Inherit the context from the parent guard.
        _rootContext = _currentGuard->_rootContext;
    }

    _currentGuard = this;
}

//! @brief De-registers the object as the guard on the current thread.
ErrorGuard::~ErrorGuard()
{
    _currentGuard = _previousGuard;

    if (_previousGuard == nullptr)
    {
        // This is the least nested guard on the current thread, it owns the
        // _rootContext and should dispose of it.
        safeDelete(_rootContext);
    }

    _previousGuard = nullptr;
}

//! @brief Determines if an error was registered with the guard.
//! @retval true An error was registered with the object.
//! @retval false The object is not aware of any error.
bool ErrorGuard::hasError() const { return _lastError.isEmpty() == false; }

//! @brief Gets the last error registered with the guard.
const Exception &ErrorGuard::getError() const { return _lastError; }

//! @brief Registers an error with the guard.
//! @param[in] error The error to register.
void ErrorGuard::setError(const Exception &error)
{
    _lastError = error;
}

//! @brief Gets a pointer to the most deeply nested ErrorGuard object on the
//! current thread.
//! @returns Either a pointer to an ErrorGuard instance or nullptr.
ErrorGuard *ErrorGuard::getCurrentGuard() { return _currentGuard; }

//! @brief Removes any error previously registered with the guard.
void ErrorGuard::clearLastError()
{
    if (_lastError.isEmpty() == false)
    {
        _lastError = Exception();
    }
}

#ifdef _WIN32
//! @brief An SEH exception filter used in the Win32-specific __try/__except
//! construct.
//! @param[in] exceptionType The type of exception which is occurring.
//! @param[in] exceptionPointers Pointers to objects describing the exception.
//! @retval EXCEPTION_CONTINUE_SEARCH A catch block further up the stack should
//! be sought.
//! @retval EXCEPTION_EXECUTE_HANDLER The exception has been handled.
int ErrorGuard::sehFilter(uint32_t exceptionType,
                          const EXCEPTION_POINTERS *exceptionPointers)
{
    int handlerAction = EXCEPTION_CONTINUE_SEARCH;

    // NOTE: Normal C++ exceptions will have an unknown exceptionType value.
    // We are only interested in specific SEH hardware exceptions, so ensure
    // the exception is of a type we want to catch.
    if ((_currentGuard != nullptr) &&
        ErrorGuardContext::isCatchableSEH(exceptionType))
    {
        handlerAction = EXCEPTION_EXECUTE_HANDLER;

        // Record the exception information, processor and thread state.
        static_cast<ErrorGuardContext *>(_currentGuard->_rootContext)->captureSEH(exceptionPointers);
    }

    return handlerAction;
}
#else
//! @brief A POSIX handler for signals generated by trapped hardware exceptions.
//! @param[in] sigNum The identifier of the signal raised.
//! @param[in] action Details of the signal raised.
//! @param[in] context The state of the system when the signal was raised (ignored).
void ErrorGuard::onLowLevelExceptionTrap(int sigNum, siginfo_t *action,
                                         void * /* context */)
{
    if ((_currentGuard != nullptr) && (_currentGuard->_guardExit != nullptr))
    {
        // Capture signal information.
        ErrorGuardContext *guardContext = static_cast<ErrorGuardContext *>(_currentGuard->_rootContext);

        guardContext->recordSignal(sigNum, action);

        // Jump back to the guard wrapping the code which raised the exception.
        longjmp(*_currentGuard->_guardExit, 1);
    }
}
#endif

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


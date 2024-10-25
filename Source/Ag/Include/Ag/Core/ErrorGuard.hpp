//! @file Ag/Core/ErrorGuard.hpp
//! @brief The declaration of an object which acts as a catch block for both
//! C++ and hardware exceptions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_ERROR_GUARD_HPP__
#define __AG_CORE_ERROR_GUARD_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
#define NOMINMAX          // Macros min(a,b) and max(a,b)
#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions
#include <Windows.h>
#else
#include <csetjmp>
#include <signal.h>
#endif

#include "Exception.hpp"

namespace Ag {
////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which provides a per-thread context for catching low level
//! hardware exceptions.
class IErrorGuardContext
{
public:
    virtual ~IErrorGuardContext() = default;

    //! @brief Sets the environment before a call is made to guarded code.
    virtual void beforeExec() =0;

    //! @brief Detects whether a non-C++ exception was caught and
    //! packages it as an Ag::Exception-derived object.
    //! @param[out] error Receives the packaged exception, if one was detected.
    virtual void afterExec(Exception &error) = 0;
};

//! @brief An object which acts as a catch block for both C++ and hardware
//! exceptions.
class ErrorGuard
{
private:
    // Internal Types
    //! @brief An internal data type representing a call to a function with
    //! no return value.
    //! @tparam TProc The function pointer type of the procedure to call.
    //! @tparam UArgs The types of parameters the wrapped function takes.
    template<typename TProc, typename ... UArgs>
    struct ScalarProcedure
    {
        TProc _proc;

        ScalarProcedure(TProc proc) : _proc(proc) { }

        void call(UArgs && ... args) const
        {
            (_proc)(std::forward<UArgs>(args)...);
        }
    };

    //! @brief An internal data type representing a call to a member
    //! function with no return value.
    //! @tparam TProc The pointer to member function type of the procedure to call.
    //! @tparam UArgs The types of parameters the wrapped function takes.
    template<typename TObj, typename UProc, typename ... VArgs>
    struct MemberProcedure
    {
        TObj *_obj;
        UProc _proc;

        MemberProcedure(TObj *obj, UProc proc) : _obj(obj), _proc(proc) { }

        void call(VArgs && ... args)
        {
            (_obj->*_proc)(std::forward<VArgs>(args)...);
        }
    };

    //! @brief An internal data type representing a call to a function with
    //! a return value.
    //! @tparam TProc The pointer type of the function to call.
    //! @tparam UArgs The types of parameters the wrapped function takes.
    template<typename URet, typename ... VArgs>
    struct ScalarFunction
    {
        typedef URet (*FnType)(VArgs...);

        FnType _fn;
        URet &_result;

        ScalarFunction(FnType fn, URet &result) : _fn(fn), _result(result) { }

        void call(VArgs && ... args) const
        {
            _result = _fn(std::forward<VArgs>(args)...);
        }
    };


    //! @brief An internal data type representing a call to a non-const member
    //! function with a return value.
    //! @tparam TProc The pointer to member type of the function to call.
    //! @tparam UArgs The types of parameters the wrapped function takes.
    template<typename TObj, typename TRet, typename ... TArgs>
    struct MemberFunction
    {
        typedef TRet (TObj::*FnType)(TArgs...);

        TObj *_obj;
        FnType _fn;
        TRet &_result;

        MemberFunction(TObj *obj, FnType fn, TRet &result) :
            _obj(obj),
            _fn(fn),
            _result(result)
        { }

        void call(TArgs && ... args) const
        {
            _result = (_obj->*_fn)(std::forward<TArgs>(args)...);
        }
    };

    //! @brief An internal data type representing a call to a const member
    //! function with a return value.
    //! @tparam TProc The pointer to member type of the function to call.
    //! @tparam UArgs The types of parameters the wrapped function takes.
    template<typename TObj, typename TRet, typename ... TArgs>
    struct ConstMemberFunction
    {
        typedef TRet (TObj::*FnType)(TArgs...) const;

        const TObj *_obj;
        FnType _fn;
        TRet &_result;

        ConstMemberFunction(const TObj *obj, FnType fn, TRet &result) :
            _obj(obj),
            _fn(fn),
            _result(result)
        { }

        void call(TArgs && ... args) const
        {
            _result = (_obj->*_fn)(std::forward<TArgs>(args)...);
        }
    };
public:
    // Public Data
    static const char *HardwareDomain;
    static const char *AddressDomain;
    static const char *DebugDomain;

    // Construction/Destruction
    ErrorGuard();
    ~ErrorGuard();

    // Accessors
    bool hasError() const;
    const Exception &getError() const;
    void setError(const Exception &error);

    static ErrorGuard *getCurrentGuard();

    // Operations
    void clearLastError();

    //! @brief Executes a function with a void return in a context which will
    //! catch useful exception types.
    //! @tparam TProc The data type of the procedure subroutine to call.
    //! @tparam TArgs The arguments to the procedure.
    //! @param[in] proc The procedure to call.
    //! @param[in] args The arguments to pass to the procedure.
    //! @retval true The procedure exited normally.
    //! @retval false An exception was captured by the guard object.
    template<typename TProc, typename ... TArgs>
    bool tryExecProcedure(TProc proc, TArgs && ... args)
    {
        ScalarProcedure<TProc, TArgs...> scalarProc(proc);

        return execProc<decltype((scalarProc)), TArgs...>(scalarProc,
                                                          std::forward<TArgs>(args)...);
    }

    //! @brief Executes a member function with a void return in a context
    //! which will catch useful exception types.
    //! @tparam TObj The data type of the object to call the member function on.
    //! @tparam TProc The data type of the member function to call.
    //! @tparam TArgs The arguments to the member function
    //! @param[in] obj The pointer to the object to call the member function on.
    //! @param[in] proc A pointer to the member function to call.
    //! @param[in] args The arguments to pass to the member function.
    //! @retval true The subroutine exited normally.
    //! @retval false An exception was captured by the guard object.
    template<typename TObj, typename TProc, typename ... TArgs>
    bool tryExecMemberProcedure(TObj *obj, TProc proc, TArgs && ... args)
    {
        MemberProcedure<TObj, TProc, TArgs...> memberProc(obj, proc);

        return execProc<decltype((memberProc)), TArgs...>(memberProc,
                                                          std::forward<TArgs>(args)...);
    }

    //! @brief Executes a function with a return value in a context which will
    //! catch useful exception types.
    //! @tparam TRet The data type of the return value.
    //! @tparam TArgs The arguments to the function.
    //! @param[in] fn The function to call.
    //! @param[out] result Receives the result of calling the guarded function.
    //! @param[in] args The arguments to pass to the function.
    //! @retval true The function exited normally.
    //! @retval false An exception was captured by the guard object.
    template<typename TRet, typename ... TArgs>
    bool tryExecFunction(TRet (*fn)(TArgs ...), TRet &result, TArgs && ... args)
    {
        ScalarFunction<TRet, TArgs...> scalarFn(fn, result);

        return execProc<decltype((scalarFn)), TArgs...>(scalarFn,
                                                        std::forward<TArgs>(args)...);
    }

    //! @brief Executes a member function with a return value in a context
    //! which will catch useful exception types.
    //! @tparam TObj A pointer to the object to call the member function on.
    //! @tparam TRet The data type of the return value.
    //! @tparam TArgs The arguments to the function.
    //! @param[in] obj A non-const pointer to the object to call the member
    //! function on.
    //! @param[in] fn A pointer to the member function to call.
    //! @param[out] result Receives the result of calling the guarded function.
    //! @param[in] args The arguments to pass to the member function.
    //! @retval true The member function exited normally.
    //! @retval false An exception was captured by the guard object.
    template<typename TObj, typename TRet, typename ... TArgs>
    bool tryExecMemberFunction(TObj *obj, TRet (TObj::*fn)(TArgs ...),
                               TRet &result, TArgs && ... args)
    {
        MemberFunction<TObj, TRet, TArgs...> memberFn(obj, fn, result);

        return execProc<decltype((memberFn)), TArgs...>(memberFn,
                                                        std::forward<TArgs>(args)...);
    }

    //! @brief Executes a const member function with a return value in a context
    //! which will catch useful exception types.
    //! @tparam TObj A pointer to the object to call the member function on.
    //! @tparam TRet The data type of the return value.
    //! @tparam TArgs The arguments to the function.
    //! @param[in] obj A const pointer to the object to call the member
    //! function on.
    //! @param[in] fn A pointer to the member function to call.
    //! @param[out] result Receives the result of the guarded function.
    //! @param[in] args The arguments to pass to the member function.
    //! @retval true The member function exited normally.
    //! @retval false An exception was captured by the guard object.
    template<typename TObj, typename TRet, typename ... TArgs>
    bool tryExecMemberFunction(const TObj *obj, TRet (TObj::*fn)(TArgs ...) const,
                               TRet &result, TArgs && ... args)
    {
        ConstMemberFunction<TObj, TRet, TArgs...> memberFn(obj, fn, result);

        return execProc<decltype((memberFn)), TArgs...>(memberFn,
                                                        std::forward<TArgs>(args)...);
    }
private:
    // Internal Types

    // Internal Functions
#ifdef _WIN32
    //! @brief A Win32-specific functions specifically for use in a __try/__except
    //! construct to determine if an SEH exception should be handled.
    //! @param[in] exceptionType The type of SEH exception detected.
    //! @param[in] exceptionPointers Processor state captured when the SEH
    //! exception was detected and detailed exception information.
    static int sehFilter(uint32_t exceptionType,
                         const _EXCEPTION_POINTERS *exceptionPointers);

    template<typename TProc, typename ...TArgs>
    bool execWin32Proc(const TProc &proc, TArgs && ... args)
    {
        bool isOK = true;

        _rootContext->beforeExec();

        __try
        {
            // Call the code under guard.
            proc.call(std::forward<TArgs>(args)...);
        }
        __except (sehFilter(GetExceptionCode(), GetExceptionInformation()))
        {
            // The raw exception data will have been saved to _rootContext by
            // sehFilter(), convert that to an Ag::Exception.
            _rootContext->afterExec(_lastError);
            isOK = false;
        }

        return isOK;
    }

    //! @brief Executes a subroutine in a hardware-exception guarded context.
    //! @tparam TProc The data type of an object which contains a call() function
    //! that accepts the appropriate arguments.
    //! @tparam TArgs The arguments to be forwarded to the code under guard.
    //! @param[in] proc The object defining the code to execute.
    //! @param[in] args The arguments to pass to the code for execution.
    //! @retval true The code executed and no exception was detected.
    //! @retval false The code produced an exception which was logged with the
    //! current guard object.
    template<typename TProc, typename ...TArgs>
    bool execProc(const TProc &proc, TArgs && ... args)
    {
        // Reset the currently captured error.
        clearLastError();

        bool isOK = true;

        try
        {
            // try/catch and __try/__except cannot appear in the same subroutine,
            // so another function call layer is required under Windows.
            isOK = execWin32Proc<TProc, TArgs...>(proc, std::forward<TArgs>(args)...);
        }
        catch (const Exception &ex)
        {
            _lastError = ex;
            isOK = false;
        }
        catch (const std::exception &stlError)
        {
            // Convert the STL exception into an Ag-compatible Exception.
            _lastError = CustomException("STL", stlError.what(), std::string_view());
            isOK = false;
        }

        return isOK;
    }
#else
    //! @brief A signal handler which internal access to ErrorGuard called by
    //! the POSIX hardware exception handling implementation.
    //! @param[in] sigNum The identifier of the signal which was raised.
    //! @param[in] action Details about the signal raised.
    //! @param[in] context The state of the processor when the signal was raised.
    static void onLowLevelExceptionTrap(int sigNum, siginfo_t *action, void *context);

    //! @brief Executes a subroutine in a hardware-exception guarded context.
    //! @tparam TProc The data type of an object which contains a call() function
    //! that accepts the appropriate arguments.
    //! @tparam TArgs The arguments to be forwarded to the code under guard.
    //! @param[in] proc The object defining the code to execute.
    //! @param[in] args The arguments to pass to the code for execution.
    //! @retval true The code executed and no exception was detected.
    //! @retval false The code produced an exception which was logged with the
    //! current guard object.
    template<typename TProc, typename ...TArgs>
    bool execProc(const TProc &proc, TArgs && ... args)
    {
        // Reset the currently captured error.
        clearLastError();

        bool isOK = true;
        jmp_buf catchLocation;
        _guardExit = &catchLocation;
        _rootContext->beforeExec();

        if (setjmp(catchLocation) == 0)
        {
            try
            {
                proc.call(std::forward<TArgs>(args)...);
            }
            catch (const Exception &ex)
            {
                _lastError = ex;
                isOK = false;
            }
            catch (const std::exception &stlError)
            {
                // Convert the STL exception into an Ag-compatible Exception.
                _lastError = CustomException("STL", stlError.what(), std::string_view());
                isOK = false;
            }
        }
        else
        {
            // We long jumped to here from a signal handler.
            _rootContext->afterExec(_lastError);
            isOK = false;
        }

        _guardExit = nullptr;

        return isOK;
    }

    // Internal Fields
    std::jmp_buf *_guardExit;
#endif
    Exception _lastError;
    ErrorGuard *_previousGuard;
    IErrorGuardContext *_rootContext;
    static thread_local ErrorGuard *_currentGuard;
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

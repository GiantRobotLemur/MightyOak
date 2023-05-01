//! @file Ag/Core/StackTrace.hpp
//! @brief The declaration of a class used to gather a stack trace.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_STACK_TRACE_HPP__
#define __AG_CORE_STACK_TRACE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
struct _CONTEXT;
#endif

namespace Ag {
////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
struct StackTracePrivate;

struct StackTraceModule
{
    std::string_view FilePath;
    std::string_view FileName;
    uintptr_t BaseAddress;
};

struct StackTraceEntry
{
    std::string_view Symbol;
    uintptr_t Offset;
    size_t ModuleId;
};

struct FullStackTraceEntry
{
    std::string_view FilePath;
    std::string_view FileName;
    std::string_view Symbol;
    uintptr_t ModuleBaseAddress;
    size_t ModuleId;
    uintptr_t Offset;
};


//! @brief A raw function activation record captured from a call stack.
//! @details Initially under Linux the ModuleBase will be 0 and the Offset will
//! contain the absolute address of the function call.
struct ActivationRecord
{
    //! @brief The base address of the module the function call belongs to.
    uintptr_t ModuleBase;

    //! @brief The offset of the function call within the owning module.
    uintptr_t Offset;
};

//! @brief An object used to gather a stack trace.
class StackTrace
{
public:
    // Construction/Destruction
    StackTrace();
    StackTrace(const StackTracePrivate *info);
    StackTrace(const StackTrace &rhs);
    StackTrace(StackTrace &&rhs) noexcept;
    ~StackTrace();

    // Accessors
    bool isEmpty() const;

    size_t getModuleCount() const;
    void getModule(size_t moduleIndex, StackTraceModule &moduleDefinition) const;
    std::string_view getModulePath(size_t moduleIndex) const;
    std::string_view getModuleFileName(size_t moduleIndex) const;
    uintptr_t getModuleBase(size_t moduleIndex) const;

    size_t getEntryCount() const;
    void getEntry(size_t entryIndex, StackTraceEntry &entry) const;
    void getFullEntry(size_t entryIndex, FullStackTraceEntry &entry) const;
    uintptr_t getEntryAddress(size_t entryIndex) const;
    uintptr_t getEntryOffset(size_t entryIndex) const;
    size_t getEntryModule(size_t entryIndex) const;
    std::string_view getEntrySymbol(size_t entryIndex) const;

    const StackTracePrivate *getData() const;

    // Operations
    void appendToString(std::string &destination, bool includeModules = true) const;
    void captureCurrentThread(size_t pruneEntries = 0);
    void capture(const ActivationRecord *stackRecords, size_t count);
#ifdef _WIN32
    void capture(const _CONTEXT *context, size_t pruneEntries = 0);
#endif
    StackTrace &operator=(StackTracePrivate *rhs);
    StackTrace &operator=(const StackTracePrivate *rhs);
    StackTrace &operator=(const StackTrace &rhs);
    StackTrace &operator=(StackTrace &&rhs) noexcept;
private:
    // Internal Functions
    void dispose();

    // Internal Fields
    StackTracePrivate *_info;
    bool _destroyOnDelete;
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

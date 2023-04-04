//! @file Core/Test_Exception.cpp
//! @brief The definition of unit tests of the Exception class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <gtest/gtest.h>

#include "Ag/Core/Exception.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

// Note no anonymous namespace so that the appropriate symbols definitely
// appear in the stack trace.

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
class MyCustomException : public Exception
{
public:
    MyCustomException(const char *message, const char *detail, uintptr_t errorCode)
    {
        initialise("Custom", message, detail, errorCode);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(Exception, CreateSimple)
{
    const char *message("Hello World!");
    const char *detail("Something went wrong!");
    const char *what("Custom: Hello World!\nSomething went wrong!");
    uintptr_t errorCode = 42;

    MyCustomException specimen(message, detail, errorCode);

    EXPECT_FALSE(specimen.isEmpty());
    EXPECT_STREQ(specimen.getDomain().data(), "Custom");
    EXPECT_STREQ(specimen.getMessage().data(), message);
    EXPECT_NE(specimen.getMessage().data(), message);
    EXPECT_STREQ(specimen.getDetail().data(), detail);
    EXPECT_NE(specimen.getDetail().data(), detail);
    EXPECT_NE(specimen.what(), what);
    EXPECT_EQ(specimen.getErrorCode(), errorCode);

    const StackTrace &trace = specimen.getStackTrace();
    ASSERT_FALSE(trace.isEmpty());

    // Ensure we have captured at least the current function, the some test
    // runner code and main().
    EXPECT_GE(trace.getEntryCount(), 3);

    // Ensure the top entry in the stack trace points to this function.
    std::vector<FullStackTraceEntry> calls;
    FullStackTraceEntry empty;

    for (size_t index = 0, count = trace.getEntryCount(); index < count; ++index)
    {
        calls.emplace_back();
        trace.getFullEntry(index, calls.back());
    }

    const FullStackTraceEntry &stackTop = calls.front();

    EXPECT_NE(stackTop.Symbol.find(__FUNCTION__), std::string_view::npos);
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


//! @file Core/Test_ErrorGuard.cpp
//! @brief The definition of the unit tests for the ErrorGuard class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <gtest/gtest.h>

#include "Ag/GTest_Core.hpp"
#include "Ag/Core/ErrorGuard.hpp"

#ifdef __GNUC__
// Disable warnings for code which is intended to induce runtime errors.
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
class MyCustomException : public Exception
{
public:
    MyCustomException(uintptr_t errorCode)
    {
        initialise("Custom", "Something went wrong.",
                   "We only have an error code", errorCode);
    }
};

class Thrower
{
public:
    void derefMemoryAddress(const size_t *ptr)
    {
        size_t value = *ptr;

        value = 0;
    }

    void integerDivide(int denominator)
    {
        int result = 424242 / denominator;

        result = 0;

        denominator /= result;
    }

    void floatingPointDivide(double denominator)
    {
        double result = 4242.42 / denominator;

        result = 0.0;
    }

    void throwProcedure(uintptr_t errorCode)
    {
        throw MyCustomException(errorCode);
    }

    void noThrowProcedure(uintptr_t errorCode)
    {
        if (errorCode < 0)
        {
            throw MyCustomException(errorCode);
        }
    }

    uintptr_t throwFunction(uintptr_t errorCode)
    {
        if (errorCode >= 0)
        {
            throw MyCustomException(errorCode);
        }

        return errorCode;
    }

    uintptr_t noThrowFunction(uintptr_t errorCode)
    {
        if (errorCode < 0)
        {
            throw MyCustomException(errorCode);
        }

        return errorCode;
    }

    void constThrowProcedure(uintptr_t errorCode) const
    {
        throw MyCustomException(errorCode);
    }

    void constNoThrowProcedure(uintptr_t errorCode) const
    {
        if (errorCode < 0)
        {
            throw MyCustomException(errorCode);
        }
    }

    uintptr_t constThrowFunction(uintptr_t errorCode) const
    {
        if (errorCode >= 0)
        {
            throw MyCustomException(errorCode);
        }

        return errorCode;
    }

    uintptr_t constNoThrowFunction(uintptr_t errorCode) const
    {
        if (errorCode < 0)
        {
            throw MyCustomException(errorCode);
        }

        return errorCode;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
// Disable optimization so that this function actually performs a deliberate
// memory access violation.
NO_OPTIMIZE_FN_BEGIN
void derefMemoryAddress(const size_t *ptr)
{
    size_t value = *ptr;

    value = 0;
}
NO_OPTIMIZE_FN_END

// Disable optimization so that this function actually performs a deliberate
// division by zero.
NO_OPTIMIZE_FN_BEGIN
void integerDivide(int denominator)
{
    size_t result = 424242 / denominator;

    result = 0;
}
NO_OPTIMIZE_FN_END

// void floatingPointDivide(double denominator)
// {
//     double result = 4242.42 / denominator;

//     result += 1.0;
// }

void throwProcedure(uintptr_t errorCode)
{
    throw MyCustomException(errorCode);
}

void noThrowProcedure(uintptr_t errorCode)
{
    if (errorCode < 0)
    {
        throw MyCustomException(errorCode);
    }
}

uintptr_t throwFunction(uintptr_t errorCode)
{
    if (errorCode >= 0)
    {
        throw MyCustomException(errorCode);
    }

    return errorCode;
}

uintptr_t noThrowFunction(uintptr_t errorCode)
{
    if (errorCode < 0)
    {
        throw MyCustomException(errorCode);
    }

    return errorCode;
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(ErrorGuard, DefaultConstruct)
{
    ErrorGuard guard;

    EXPECT_FALSE(guard.hasError());
    EXPECT_TRUE(guard.getError().isEmpty());
    EXPECT_EQ(ErrorGuard::getCurrentGuard(), &guard);
}

GTEST_TEST(ErrorGuard, NestedGuard)
{
    ErrorGuard guard;

    ASSERT_FALSE(guard.hasError());
    EXPECT_TRUE(guard.getError().isEmpty());
    EXPECT_EQ(ErrorGuard::getCurrentGuard(), &guard);

    if (guard.hasError() == false)
    {
        ErrorGuard specimen;
        EXPECT_FALSE(specimen.hasError());
        EXPECT_TRUE(specimen.getError().isEmpty());
        EXPECT_EQ(ErrorGuard::getCurrentGuard(), &specimen);
    }

    EXPECT_EQ(ErrorGuard::getCurrentGuard(), &guard);
}

GTEST_TEST(ErrorGuard, CatchScalarProcedure)
{
    ErrorGuard guard;

    EXPECT_FALSE(guard.hasError());
    EXPECT_TRUE(guard.getError().isEmpty());
    EXPECT_EQ(ErrorGuard::getCurrentGuard(), &guard);

    EXPECT_FALSE(guard.tryExecProcedure(throwProcedure, 42));
    EXPECT_TRUE(guard.hasError());
    EXPECT_EQ(guard.getError().getErrorCode(), 42);

    EXPECT_TRUE(guard.tryExecProcedure(noThrowProcedure, 54));
    EXPECT_FALSE(guard.hasError());
}


GTEST_TEST(ErrorGuard, CatchScalarFunction)
{
    ErrorGuard guard;

    EXPECT_FALSE(guard.hasError());
    EXPECT_TRUE(guard.getError().isEmpty());
    EXPECT_EQ(ErrorGuard::getCurrentGuard(), &guard);

    uintptr_t result = 0;
    EXPECT_FALSE(guard.tryExecFunction(throwFunction, result,
                                       static_cast<uintptr_t>(69)));
    EXPECT_TRUE(guard.hasError());
    EXPECT_EQ(result, 0);
    EXPECT_EQ(guard.getError().getErrorCode(), 69);

    result = 0;
    EXPECT_TRUE(guard.tryExecFunction(noThrowFunction, result,
                                      static_cast<uintptr_t>(54)));
    EXPECT_FALSE(guard.hasError());
    EXPECT_EQ(result, 54);
}

GTEST_TEST(ErrorGuard, CatchMemberProcedure)
{
    ErrorGuard guard;
    Thrower specimen;
    const Thrower *targetPtr = &specimen;

    // Confirm base state.
    EXPECT_FALSE(guard.hasError());
    EXPECT_TRUE(guard.getError().isEmpty());
    EXPECT_EQ(ErrorGuard::getCurrentGuard(), &guard);

    // Try error path.
    EXPECT_FALSE(guard.tryExecMemberProcedure(&specimen, &Thrower::throwProcedure, 42));
    EXPECT_TRUE(guard.hasError());
    EXPECT_EQ(guard.getError().getErrorCode(), 42);

    // Try normal path.
    EXPECT_TRUE(guard.tryExecMemberProcedure(&specimen, &Thrower::noThrowProcedure, 54));
    EXPECT_FALSE(guard.hasError());

    // Try error path with const member function.
    EXPECT_FALSE(guard.tryExecMemberProcedure(targetPtr, &Thrower::constThrowProcedure, 35));
    EXPECT_TRUE(guard.hasError());
    EXPECT_EQ(guard.getError().getErrorCode(), 35);

    // Try normal path with const member function.
    EXPECT_TRUE(guard.tryExecMemberProcedure(targetPtr, &Thrower::constNoThrowProcedure, 11));
    EXPECT_FALSE(guard.hasError());
}

GTEST_TEST(ErrorGuard, CatchMemberFunction)
{
    ErrorGuard guard;
    Thrower specimen;
    const Thrower *targetPtr = &specimen;

    // Confirm base state.
    EXPECT_FALSE(guard.hasError());
    EXPECT_TRUE(guard.getError().isEmpty());
    EXPECT_EQ(ErrorGuard::getCurrentGuard(), &guard);

    // Try error path.
    uintptr_t result = 0;
    EXPECT_FALSE(guard.tryExecMemberFunction(&specimen, &Thrower::throwFunction,
                                             result, static_cast<uintptr_t>(69)));
    EXPECT_TRUE(guard.hasError());
    EXPECT_EQ(result, 0);
    EXPECT_EQ(guard.getError().getErrorCode(), 69);

    // Try normal path.
    result = 0;
    EXPECT_TRUE(guard.tryExecMemberFunction(&specimen, &Thrower::noThrowFunction,
                                            result, static_cast<uintptr_t>(54)));
    EXPECT_FALSE(guard.hasError());
    EXPECT_EQ(result, 54);

    // Try error path with const member function.
    result = 0;
    EXPECT_FALSE(guard.tryExecMemberFunction(targetPtr, &Thrower::constThrowFunction,
                                             result, static_cast<uintptr_t>(11)));
    EXPECT_TRUE(guard.hasError());
    EXPECT_EQ(result, 0);
    EXPECT_EQ(guard.getError().getErrorCode(), 11);

    // Try normal path with const member function.
    result = 0;
    EXPECT_TRUE(guard.tryExecMemberFunction(targetPtr, &Thrower::constNoThrowFunction,
                                            result, static_cast<uintptr_t>(12)));
    EXPECT_FALSE(guard.hasError());
    EXPECT_EQ(result, 12);
}

GTEST_TEST(ErrorGuard, CatchLambdaProcedure)
{
    ErrorGuard guard;

    // Test error path.
    EXPECT_FALSE(guard.tryExecProcedure([](uintptr_t errorCode)
                                        { throw MyCustomException(errorCode); },
                                        42));
    EXPECT_TRUE(guard.hasError());
    EXPECT_EQ(guard.getError().getErrorCode(), 42);

    // Test no-throw path.
    EXPECT_TRUE(guard.tryExecProcedure([](uintptr_t errorCode)
                                       { errorCode += 4; },
                                       42));
    EXPECT_FALSE(guard.hasError());
}

// Struggling to get this to work:
//GTEST_TEST(ErrorGuard, CatchLambdaFunction)
//{
//    ErrorGuard guard;
//
//    // Test error path.
//    uintptr_t result = 0;
//    constexpr auto throwFn = [](uintptr_t errorCode) -> uintptr_t
//    {
//        if (errorCode >= 0) throw CustomException(errorCode);
//        return errorCode;
//    };
//
//    EXPECT_FALSE(guard.tryExecFunction<uintptr_t, uintptr_t>(throwFn, result, 42));
//    EXPECT_TRUE(guard.hasError());
//    EXPECT_EQ(guard.getError().getErrorCode(), 42);
//    EXPECT_EQ(result, 0);
//
//    // Test no-throw path.
//    result = 0;
//
//    const auto noThrowFn = [](uintptr_t errorCode) -> uintptr_t
//    {
//        if (errorCode < 0) throw CustomException(errorCode);
//        return errorCode;
//    };
//
//    EXPECT_TRUE(guard.tryExecFunction<uintptr_t, uintptr_t>(noThrowFn, result, 69));
//    EXPECT_FALSE(guard.hasError());
//    EXPECT_EQ(result, 69);
//}

GTEST_TEST(ErrorGuard, CatchMemoryException)
{
    ErrorGuard guard;

    // Test error path.
    EXPECT_FALSE(guard.tryExecProcedure(derefMemoryAddress, nullptr));
    EXPECT_TRUE(guard.hasError());
    EXPECT_STREQ(guard.getError().getDomain().data(), ErrorGuard::AddressDomain);
}


GTEST_TEST(ErrorGuard, CatchIntDivisionByZeroException)
{
    ErrorGuard guard;

    // Test error path.
    EXPECT_FALSE(guard.tryExecProcedure(integerDivide, 0));
    EXPECT_TRUE(guard.hasError());
    EXPECT_STREQ(guard.getError().getDomain().data(),
                 DivisionByZeroException::Domain);
}

// On x64 Windows and Linux, the double division by zero produces an inf result.
// GTEST_TEST(ErrorGuard, CatchFloatDivisionByZeroException)
// {
//    ErrorGuard guard;

//    // Test error path.
//    EXPECT_FALSE(guard.tryExecProcedure(floatingPointDivide, 0.0));
//    EXPECT_TRUE(guard.hasError());
//    EXPECT_STREQ(guard.getError().getDomain().data(),
//                 DivisionByZeroException::Domain);
// }

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


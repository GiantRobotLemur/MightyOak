//! @file GTest_Core.hpp
//! @brief The declaration of header-only elements useful in Google Test-based
//! unit test subroutines.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

// Only define elements if <gtest/gtest.h> has been previously included.
#if defined(GTEST_TEST)

#ifndef __AG_CORE_GTEST_CORE_HPP__
#define __AG_CORE_GTEST_CORE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#define EXPECT_STRINGEQ(x, y) EXPECT_STREQ((x).getUtf8Bytes(), (y).getUtf8Bytes())

#define EXPECT_STRINGEQC(x, y) EXPECT_STREQ((x).getUtf8Bytes(), (y))

#define EXPECT_NEAREQ(x, y, z) EXPECT_LT(std::abs((x) - (y)), (z))

#ifdef __GNUC__
#define NO_OPTIMIZE_FN_ATTRIB __attribute__((optimize("-O0")));
#else
#define NO_OPTIMIZE_FN_ATTRIB
#endif

#ifdef _MSC_VER
#ifdef __INTELLISENSE__
#define NO_OPTIMIZE_FN
#else
#define NO_OPTIMIZE_FN _Pragma("optimize(\"gt\", off)")
#endif // ifdef __INTELLISENSE__
#else // ifndef _MSC_VER
#define NO_OPTIMIZE_FN
#endif

#endif // Header guard

#endif // GTEST include guard
////////////////////////////////////////////////////////////////////////////////

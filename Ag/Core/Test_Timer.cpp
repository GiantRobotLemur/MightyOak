//! @file Core/Test_Timer.cpp
//! @brief The definition of unit tests for high-resolution timing functions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/Core/Timer.hpp"

namespace Ag {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t NoticablePause = 125;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void busyWait(uint32_t cycles)
{
    srand(123456);

    for (uint32_t cycle = 0; cycle < cycles; ++cycle)
    {
        const size_t Repeats = 1024 * 1024;
        int value = 0;

        for (size_t i = 0; i < Repeats; ++i)
        {
            value = rand();
        }

        value = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(HighResTimer, HasConstantFrequency)
{
    MonotonicTicks baseFreq = HighResMonotonicTimer::getFrequency();

    busyWait(NoticablePause);

    MonotonicTicks newFreq = HighResMonotonicTimer::getFrequency();

    ASSERT_EQ(baseFreq, newFreq);
    const char *unit = "Hz";
    double divisor = 1.0;

    if (baseFreq > (static_cast<MonotonicTicks>(5) * 1000 * 1000 * 1000))
    {
        unit = "GHz";
        divisor = 1e9;
    }
    else if (baseFreq > (static_cast<MonotonicTicks>(5) * 1000 * 1000))
    {
        unit = "MHz";
        divisor = 1e6;
    }
    else if (baseFreq > (static_cast<MonotonicTicks>(5) * 1000))
    {
        unit = "KHz";
        divisor = 1e3;
    }

    printf("High Resolution Timer Frequency steady at %.2f %s\n",
           newFreq / divisor, unit);
}

GTEST_TEST(HighResTimer, CaptureDuration)
{
    MonotonicTicks start = HighResMonotonicTimer::getTime();

    busyWait(NoticablePause);

    MonotonicTicks duration = HighResMonotonicTimer::getDuration(start);

    EXPECT_GT(duration, 0);
}

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


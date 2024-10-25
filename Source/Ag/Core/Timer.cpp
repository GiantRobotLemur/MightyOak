//! @file Core/Timer.cpp
//! @brief The declaration of various types of timer component.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Timer.hpp"

#include "Platform.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
namespace HighResMonotonicTimer
{

//! @brief Gets the count of high-resolution timer ticks per second.
MonotonicTicks getFrequency()
{
#ifdef _WIN32
    LARGE_INTEGER ticks;
    if (::QueryPerformanceFrequency(&ticks))
    {
        return ticks.QuadPart;
    }
    else
    {
        throw Win32Exception("QueryPerformanceFrequency()", ::GetLastError());
    }
#else
    // The timer resolution is 1 nano-second, but the precision probably
    // far lower.
    return 1e9;
#endif
}

//! @brief Gets the count of ticks on the system high resolution timer since an
//! arbitrary epoch.
MonotonicTicks getTime()
{
#ifdef _WIN32
    LARGE_INTEGER ticks;
    if (::QueryPerformanceCounter(&ticks))
    {
        return ticks.QuadPart;
    }
    else
    {
        throw Win32Exception("QueryPerformanceCounter()", ::GetLastError());
    }
#else
    struct timespec nativeFreq;

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &nativeFreq) == 0)
    {
        MonotonicTicks tickCount = static_cast<MonotonicTicks>(nativeFreq.tv_sec) * 1e9;
        tickCount += static_cast<MonotonicTicks>(nativeFreq.tv_nsec);

        return tickCount;
    }
    else
    {
        throw RuntimeLibraryException("clock_getres(CLOCK_MONOTONIC_RAW,...)",
                                      errno);
    }
#endif
}

//! @brief Gets the duration since a previously captured time stamp
//! @param[in] start The tick count returned by getTime() captured at the
//! beginning of the span of time being measured.
//! @return The count of ticks elapsed since the beginning of the time span.
MonotonicTicks getDuration(const MonotonicTicks &start)
{
    return getTime() - start;
}

//! @brief Calculates the span of time represented by a tick count, in
//! fractional seconds.
//! @param[in] tickCount The count of timer ticks as might be returned by
//! getDuration().
//! @return The span of time represented by tickCount as a count of fractional
//! seconds.
double getTimeSpan(MonotonicTicks tickCount)
{
    double timeSpanInSeconds = 0.0;

    if (tickCount != 0)
    {
        MonotonicTicks ticksPerSecond = getFrequency();

        if (ticksPerSecond > 0)
        {
            timeSpanInSeconds = tickCount / static_cast<double>(ticksPerSecond);
        }
    }

    return timeSpanInSeconds;
}

} // namespace HighResMonotonicTimer

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


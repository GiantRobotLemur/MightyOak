//! @file Test_GuestEventQueue.cpp
//! @brief The definition of unit tests of the GuestEventQueue object.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <chrono>
#include <future>
#include <thread>

#include <gtest/gtest.h>

#include "ArmEmu/GuestEventQueue.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct ListenerData : public std::enable_shared_from_this<ListenerData>
{
    std::vector<GuestEvent> Received;
    GuestEventQueue Specimen;

    ListenerData(uintptr_t id) :
        Received(),
        Specimen(id)
    {
        Received.reserve(16);
    }
};

struct Listener
{
    size_t operator()(std::shared_ptr<ListenerData> shared)
    {
        bool keepGoing = true;
        GuestEvent nextEvent;

        do
        {
            if (shared->Specimen.tryDeque(nextEvent))
            {
                shared->Received.push_back(nextEvent);

                keepGoing = (nextEvent.Type != 0);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

        } while (keepGoing);

        return shared->Received.size();
    }
};

using ListenerDataSPtr = std::shared_ptr<ListenerData>;

void compareEvents(size_t index, const GuestEvent &expected, const GuestEvent &actual)
{
    std::string trace = "Testing message " + std::to_string(index);
    SCOPED_TRACE(trace.c_str());

    EXPECT_EQ(expected.SourceID, actual.SourceID);
    EXPECT_EQ(expected.Type, actual.Type);
    EXPECT_EQ(expected.Data1, actual.Data1);
    EXPECT_EQ(expected.Data2, actual.Data2);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CoreLogic, CrossThreadMessages)
{
    ListenerDataSPtr specimenWrapper = std::make_shared<ListenerData>(0xDEADBEEF);

    auto asyncResult = std::async(Listener(), specimenWrapper);

    std::this_thread::sleep_for(std::chrono::microseconds(150));
    specimenWrapper->Specimen.enque(42, 9, 12);

    std::this_thread::sleep_for(std::chrono::microseconds(250));
    specimenWrapper->Specimen.enque(69, 11, 31);

    std::this_thread::sleep_for(std::chrono::microseconds(75));
    specimenWrapper->Specimen.enque(1, 2, 3);
    specimenWrapper->Specimen.enque(4, 5, 6);
    specimenWrapper->Specimen.enque(7, 8, 9);

    std::this_thread::sleep_for(std::chrono::microseconds(250));
    specimenWrapper->Specimen.enque(0, 0, 0);

    auto status = asyncResult.wait_for(std::chrono::milliseconds(500));

    ASSERT_EQ(status, std::future_status::ready);
    size_t receivedCount = asyncResult.get();

    EXPECT_EQ(receivedCount, 6u);
    ASSERT_EQ(specimenWrapper->Received.size(), 6u);

    compareEvents(0, GuestEvent(0xDEADBEEF, 42, 9, 12), specimenWrapper->Received[0]);
    compareEvents(0, GuestEvent(0xDEADBEEF, 69, 11, 31), specimenWrapper->Received[1]);
    compareEvents(0, GuestEvent(0xDEADBEEF, 1, 2, 3), specimenWrapper->Received[2]);
    compareEvents(0, GuestEvent(0xDEADBEEF, 4, 5, 6), specimenWrapper->Received[3]);
    compareEvents(0, GuestEvent(0xDEADBEEF, 7, 8, 9), specimenWrapper->Received[4]);
    compareEvents(0, GuestEvent(0xDEADBEEF, 0, 0, 0), specimenWrapper->Received[5]);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


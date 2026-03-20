//! @file ArmEmu/Test/Test_WatchpointManager.cpp
//! @brief The definition of unit tests for the WatchpointManager class.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "ArmEmu/WatchpointManager.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Memory Watchpoint Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(WatchpointManagerTest, AddMemoryWatchpoint)
{
    WatchpointManager mgr;

    EXPECT_FALSE(mgr.hasMemoryWatchpoints());

    uint32_t id = mgr.addMemoryWatchpoint(0x1000, WatchpointType::Write);

    EXPECT_TRUE(mgr.hasMemoryWatchpoints());
    ASSERT_EQ(mgr.getMemoryWatchpoints().size(), 1u);
    EXPECT_EQ(mgr.getMemoryWatchpoints()[0].Address, 0x1000u);
    EXPECT_EQ(mgr.getMemoryWatchpoints()[0].Id, id);
    EXPECT_EQ(mgr.getMemoryWatchpoints()[0].Type, WatchpointType::Write);
}

GTEST_TEST(WatchpointManagerTest, WriteWatchpointTriggersOnWrite)
{
    WatchpointManager mgr;
    mgr.addMemoryWatchpoint(0x1000, WatchpointType::Write);

    EXPECT_TRUE(mgr.checkMemoryWrite(0x1000, 4, 0x42));

    auto &hit = mgr.getLastHit();
    EXPECT_TRUE(hit.IsValid);
    EXPECT_TRUE(hit.IsWrite);
    EXPECT_FALSE(hit.IsRegister);
    EXPECT_EQ(hit.Address, 0x1000u);
    EXPECT_EQ(hit.NewValue, 0x42u);
}

GTEST_TEST(WatchpointManagerTest, WriteWatchpointIgnoresRead)
{
    WatchpointManager mgr;
    mgr.addMemoryWatchpoint(0x1000, WatchpointType::Write);

    EXPECT_FALSE(mgr.checkMemoryRead(0x1000, 4, 0x42));
}

GTEST_TEST(WatchpointManagerTest, ReadWatchpointTriggersOnRead)
{
    WatchpointManager mgr;
    mgr.addMemoryWatchpoint(0x1000, WatchpointType::Read);

    EXPECT_TRUE(mgr.checkMemoryRead(0x1000, 4, 0x42));
    EXPECT_FALSE(mgr.checkMemoryWrite(0x1000, 4, 0x42));
}

GTEST_TEST(WatchpointManagerTest, BothWatchpointTriggersOnBoth)
{
    WatchpointManager mgr;
    mgr.addMemoryWatchpoint(0x1000, WatchpointType::Both);

    EXPECT_TRUE(mgr.checkMemoryRead(0x1000, 4, 0x42));
    EXPECT_TRUE(mgr.checkMemoryWrite(0x1000, 4, 0x42));
}

GTEST_TEST(WatchpointManagerTest, WatchpointMissesNonOverlappingAddress)
{
    WatchpointManager mgr;
    mgr.addMemoryWatchpoint(0x1000, WatchpointType::Write);

    EXPECT_FALSE(mgr.checkMemoryWrite(0x2000, 4, 0x42));
}

GTEST_TEST(WatchpointManagerTest, WatchpointDetectsOverlappingAccess)
{
    WatchpointManager mgr;
    mgr.addMemoryWatchpoint(0x1002, WatchpointType::Write);

    // A 4-byte write starting at 0x1000 overlaps 0x1002.
    EXPECT_TRUE(mgr.checkMemoryWrite(0x1000, 4, 0x42));
}

GTEST_TEST(WatchpointManagerTest, WatchpointMissesAdjacentAccess)
{
    WatchpointManager mgr;
    mgr.addMemoryWatchpoint(0x1000, WatchpointType::Write);

    // A 4-byte write starting at 0x1004 does not overlap [0x1000, 0x1004).
    EXPECT_FALSE(mgr.checkMemoryWrite(0x1004, 4, 0x42));
}

////////////////////////////////////////////////////////////////////////////////
// Register Watchpoint Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(WatchpointManagerTest, AddRegisterWatchpoint)
{
    WatchpointManager mgr;

    EXPECT_FALSE(mgr.hasRegisterWatchpoints());

    uint32_t id = mgr.addRegisterWatchpoint(12, 0, false);

    EXPECT_TRUE(mgr.hasRegisterWatchpoints());
    EXPECT_EQ(mgr.getRegisterWatchpoints().size(), 1u);
    EXPECT_EQ(mgr.getRegisterWatchpoints()[0].RegisterId, 12u);
    EXPECT_EQ(mgr.getRegisterWatchpoints()[0].Id, id);
    EXPECT_FALSE(mgr.getRegisterWatchpoints()[0].MatchSpecific);
}

GTEST_TEST(WatchpointManagerTest, RegisterWatchpointAnyChange)
{
    WatchpointManager mgr;
    mgr.addRegisterWatchpoint(5, 0, false);

    // First call sets shadow, no trigger (value doesn't change from 0).
    EXPECT_FALSE(mgr.checkRegister(5, 0));

    // Value changes from 0 to 42, should trigger.
    EXPECT_TRUE(mgr.checkRegister(5, 42));

    auto &hit = mgr.getLastHit();
    EXPECT_TRUE(hit.IsValid);
    EXPECT_TRUE(hit.IsRegister);
    EXPECT_EQ(hit.Address, 5u);
    EXPECT_EQ(hit.OldValue, 0u);
    EXPECT_EQ(hit.NewValue, 42u);
}

GTEST_TEST(WatchpointManagerTest, RegisterWatchpointNoChangeNoTrigger)
{
    WatchpointManager mgr;
    mgr.addRegisterWatchpoint(3, 0, false);

    // Set initial shadow.
    mgr.checkRegister(3, 100);

    // Same value again — no change, no trigger.
    EXPECT_FALSE(mgr.checkRegister(3, 100));
}

GTEST_TEST(WatchpointManagerTest, RegisterWatchpointMatchSpecific)
{
    WatchpointManager mgr;
    mgr.addRegisterWatchpoint(12, 0x0380A4EC, true);

    // Change to some other value — doesn't match.
    EXPECT_FALSE(mgr.checkRegister(12, 0x12345678));

    // Change to the match value — triggers.
    EXPECT_TRUE(mgr.checkRegister(12, 0x0380A4EC));

    auto &hit = mgr.getLastHit();
    EXPECT_TRUE(hit.IsValid);
    EXPECT_TRUE(hit.IsRegister);
    EXPECT_EQ(hit.NewValue, 0x0380A4ECu);
}

GTEST_TEST(WatchpointManagerTest, RegisterWatchpointIgnoresOtherRegisters)
{
    WatchpointManager mgr;
    mgr.addRegisterWatchpoint(5, 0, false);

    // Change to register 3 — should not trigger.
    EXPECT_FALSE(mgr.checkRegister(3, 42));
}

GTEST_TEST(WatchpointManagerTest, RegisterOutOfRange)
{
    WatchpointManager mgr;
    mgr.addRegisterWatchpoint(5, 0, false);

    // Register 16 is out of range.
    EXPECT_FALSE(mgr.checkRegister(16, 42));
}

////////////////////////////////////////////////////////////////////////////////
// Remove and Clear Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(WatchpointManagerTest, RemoveMemoryWatchpoint)
{
    WatchpointManager mgr;
    uint32_t id = mgr.addMemoryWatchpoint(0x1000, WatchpointType::Write);

    EXPECT_TRUE(mgr.removeWatchpoint(id));
    EXPECT_FALSE(mgr.hasMemoryWatchpoints());
}

GTEST_TEST(WatchpointManagerTest, RemoveRegisterWatchpoint)
{
    WatchpointManager mgr;
    uint32_t id = mgr.addRegisterWatchpoint(5, 0, false);

    EXPECT_TRUE(mgr.removeWatchpoint(id));
    EXPECT_FALSE(mgr.hasRegisterWatchpoints());
}

GTEST_TEST(WatchpointManagerTest, RemoveNonExistent)
{
    WatchpointManager mgr;
    EXPECT_FALSE(mgr.removeWatchpoint(999));
}

GTEST_TEST(WatchpointManagerTest, ClearAll)
{
    WatchpointManager mgr;
    mgr.addMemoryWatchpoint(0x1000, WatchpointType::Write);
    mgr.addMemoryWatchpoint(0x2000, WatchpointType::Read);
    mgr.addRegisterWatchpoint(5, 0, false);

    mgr.clearAll();

    EXPECT_FALSE(mgr.hasMemoryWatchpoints());
    EXPECT_FALSE(mgr.hasRegisterWatchpoints());
}

GTEST_TEST(WatchpointManagerTest, UniqueIds)
{
    WatchpointManager mgr;
    uint32_t id1 = mgr.addMemoryWatchpoint(0x1000, WatchpointType::Write);
    uint32_t id2 = mgr.addMemoryWatchpoint(0x2000, WatchpointType::Read);
    uint32_t id3 = mgr.addRegisterWatchpoint(5, 0, false);

    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}

} // Anonymous namespace

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

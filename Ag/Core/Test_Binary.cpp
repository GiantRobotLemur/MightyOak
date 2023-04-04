//! @file Core/Test_Binary.cpp
//! @brief The definition of
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/Core/Binary.hpp"

namespace Ag {
namespace Bin {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
template<typename T>
uint8_t manualPopCount(T value)
{
    uint8_t count = 0;

    for ( ; value != 0; value >>= 1)
    {
        if (value & 1)
        {
            ++count;
        }
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(BinaryOperations, RotateLeft32)
{
    uint32_t testValue = 0x0000FF00;

    EXPECT_EQ(rotateLeft(testValue, 8),  0x00FF0000u);
    EXPECT_EQ(rotateLeft(testValue, 16), 0xFF000000u);
    EXPECT_EQ(rotateLeft(testValue, 20), 0xF000000Fu);
    EXPECT_EQ(rotateLeft(testValue, 24), 0x000000FFu);
    EXPECT_EQ(rotateLeft(testValue, 32), 0x0000FF00u);
}

GTEST_TEST(BinaryOperations, RotateLeft64)
{
    uint64_t testValue = 0x00FF00;

    EXPECT_EQ(rotateLeft(testValue, 8),  0x00FF0000ull);
    EXPECT_EQ(rotateLeft(testValue, 16), 0xFF000000ull);
    EXPECT_EQ(rotateLeft(testValue, 24), 0xFF00000000ull);
    EXPECT_EQ(rotateLeft(testValue, 32), 0xFF0000000000ull);
    EXPECT_EQ(rotateLeft(testValue, 40), 0xFF000000000000ull);
    EXPECT_EQ(rotateLeft(testValue, 48), 0xFF00000000000000ull);
    EXPECT_EQ(rotateLeft(testValue, 52), 0xF00000000000000Full);
    EXPECT_EQ(rotateLeft(testValue, 56), 0x00000000000000FFull);
    EXPECT_EQ(rotateLeft(testValue, 64), 0x000000000000FF00ull);
}

GTEST_TEST(BinaryOperations, RotateRight32)
{
    uint32_t testValue = 0x0000FF00;

    EXPECT_EQ(rotateRight(testValue, 8),  0x000000FFu);
    EXPECT_EQ(rotateRight(testValue, 12), 0xF000000Fu);
    EXPECT_EQ(rotateRight(testValue, 16), 0xFF000000u);
    EXPECT_EQ(rotateRight(testValue, 24), 0x00FF0000u);
    EXPECT_EQ(rotateRight(testValue, 32), 0x0000FF00u);
}

GTEST_TEST(BinaryOperations, RotateRight64)
{
    uint64_t testValue = 0x00FF00;

    EXPECT_EQ(rotateRight(testValue, 8),  0x000000FFull);
    EXPECT_EQ(rotateRight(testValue, 12), 0xF00000000000000Full);
    EXPECT_EQ(rotateRight(testValue, 16), 0xFF00000000000000ull);
    EXPECT_EQ(rotateRight(testValue, 24), 0x00FF000000000000ull);
    EXPECT_EQ(rotateRight(testValue, 32), 0x0000FF0000000000ull);
    EXPECT_EQ(rotateRight(testValue, 40), 0x000000FF00000000ull);
    EXPECT_EQ(rotateRight(testValue, 48), 0x00000000FF000000ull);
    EXPECT_EQ(rotateRight(testValue, 56), 0x0000000000FF0000ull);
    EXPECT_EQ(rotateRight(testValue, 64), 0x000000000000FF00ull);
}

GTEST_TEST(BinaryOperations, TestBitScanForward32)
{
    int32_t msb = 0;

    EXPECT_FALSE(bitScanForward(static_cast<uint32_t>(0), msb));
    EXPECT_FALSE(bitScanForward(static_cast<uint64_t>(0), msb));

    EXPECT_TRUE(bitScanForward(1U, msb));
    EXPECT_EQ(msb, 0);

    EXPECT_TRUE(bitScanForward(2U, msb));
    EXPECT_EQ(msb, 1);

    EXPECT_TRUE(bitScanForward(8U, msb));
    EXPECT_EQ(msb, 3);

    EXPECT_TRUE(bitScanForward(15U, msb));
    EXPECT_EQ(msb, 0);

    EXPECT_TRUE(bitScanForward(0x80000002U, msb));
    EXPECT_EQ(msb, 1);

    EXPECT_TRUE(bitScanForward(0xFFFFFFFFU, msb));
    EXPECT_EQ(msb, 0);
}

GTEST_TEST(BinaryOperations, TestBitScanForward64)
{
    int32_t msb = 0;
    EXPECT_TRUE(bitScanForward(static_cast<uint64_t>(0x3A0000100ULL), msb));
    EXPECT_EQ(msb, 8);

    EXPECT_TRUE(bitScanForward(static_cast<uint64_t>(0x80060000A0000000ULL), msb));
    EXPECT_EQ(msb, 29);
}

GTEST_TEST(BinaryOperations, TestBitScanReverse32)
{
    int32_t msb = 0;

    EXPECT_FALSE(bitScanReverse(static_cast<uint32_t>(0U), msb));
    EXPECT_FALSE(bitScanReverse(static_cast<uint64_t>(0ULL), msb));

    EXPECT_TRUE(bitScanReverse(1U, msb));
    EXPECT_EQ(msb, 0);

    EXPECT_TRUE(bitScanReverse(2U, msb));
    EXPECT_EQ(msb, 1);

    EXPECT_TRUE(bitScanReverse(8U, msb));
    EXPECT_EQ(msb, 3);

    EXPECT_TRUE(bitScanReverse(15U, msb));
    EXPECT_EQ(msb, 3);

    EXPECT_TRUE(bitScanReverse(0x80000002U, msb));
    EXPECT_EQ(msb, 31);

    EXPECT_TRUE(bitScanReverse(0xFFFFFFFFU, msb));
    EXPECT_EQ(msb, 31);
}

GTEST_TEST(BinaryOperations, TestBitScanReverse64)
{
    int32_t msb = 0;
    EXPECT_TRUE(bitScanReverse(static_cast<uint64_t>(0xCA0000), msb));
    EXPECT_EQ(msb, 23);

    EXPECT_TRUE(bitScanReverse(static_cast<uint64_t>(0x3A0000100), msb));
    EXPECT_EQ(msb, 33);

    EXPECT_TRUE(bitScanReverse(static_cast<uint64_t>(0x80060000A0000100), msb));
    EXPECT_EQ(msb, 63);
}

GTEST_TEST(PopCount, Verify16Bit)
{
    uint16_t i = 0;

    do
    {
        uint8_t count = popCount(i);
        uint8_t verifiedCount = manualPopCount(i);

        ASSERT_EQ(count, verifiedCount);

        ++i;
    } while (i != 0);
}

// Takes 7 minutes to run.
//GTEST_TEST(PopCount, Verify32Bit)
//{
//    uint32_t i = 0;
//
//    do
//    {
//        uint8_t count = popCount(i);
//        uint8_t verifiedCount = manualPopCount(i);
//
//        ASSERT_EQ(count, verifiedCount);
//
//        ++i;
//    } while (i != 0);
//}

} // TED

}} // namespace Ag::Bin
////////////////////////////////////////////////////////////////////////////////


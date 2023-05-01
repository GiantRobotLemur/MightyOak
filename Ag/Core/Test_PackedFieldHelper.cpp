//! @file Core/Test_PackedFieldHelper.cpp
//! @brief The unit tests for the PackedFieldHelper class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <ios>
#include <iomanip>

#include <gtest/gtest.h>

#include "Ag/Private/SymbolEncoding.hpp"

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
::testing::AssertionResult areBuffersEqual(const PackedFieldHelper &lhs,
                                           const std::initializer_list<uint8_t> &rhs)
{
    ::testing::AssertionResult result = ::testing::AssertionSuccess();

    if (lhs.getBufferSize() != rhs.size())
    {
        result = ::testing::AssertionFailure();
        
        result << "The buffer is " << lhs.getBufferSize() <<
            " bytes long but the expected result is " <<
            rhs.size() << " bytes.";
    }
    else
    {
        size_t index = 0;
        const uint8_t *buffer = lhs.getFieldBuffer();

        for (uint8_t nextByte : rhs)
        {
            if (buffer[index] != nextByte)
            {
                ::testing::Message msg;
                
                msg << "Byte " << index << " should be 0x";
                msg << std::setfill('0') << std::setw(2) << std::right <<
                    std::hex << std::uppercase;
                msg << static_cast<uint32_t>(nextByte) <<
                    " but is 0x" << std::setw(2) <<
                    static_cast<uint32_t>(buffer[index]) << ".";

                result = ::testing::AssertionFailure(msg);
                break;
            }
            else
            {
                ++index;
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(PackedFieldHelper, TwoFields)
{
    PackedFieldHelper specimen({ 3, 4 });

    EXPECT_EQ(specimen.getBufferSize(), 1u);
    EXPECT_NE(specimen.getFieldBuffer(), nullptr);
    EXPECT_EQ(specimen.getField<uint8_t>(0), 0u);
    EXPECT_EQ(specimen.getField<uint8_t>(1), 0u);

    // Test reading out of range.
    EXPECT_EQ(specimen.getField<uint8_t>(2), 0u);

    EXPECT_TRUE(areBuffersEqual(specimen, { 0 }));

    // Try setting some fields.
    specimen.setField(1, 0xF);
    EXPECT_TRUE(areBuffersEqual(specimen, { 0x78 }));
    EXPECT_EQ(specimen.getField<int>(0), 0);
    EXPECT_EQ(specimen.getField<int>(1), 0x0F);

    specimen.setField(0, 0x3);
    EXPECT_TRUE(areBuffersEqual(specimen, { 0x7B }));
    EXPECT_EQ(specimen.getField<int>(0), 0x03);
    EXPECT_EQ(specimen.getField<int>(1), 0x0F);

    specimen.setField(1, 0xA5);
    EXPECT_TRUE(areBuffersEqual(specimen, { 0x2B }));
    EXPECT_EQ(specimen.getField<int>(0), 0x03);
    EXPECT_EQ(specimen.getField<int>(1), 0x05);
}

GTEST_TEST(PackedFieldHelper, ThreeFields)
{
    PackedFieldHelper specimen({ 3, 5, 9 });

    EXPECT_EQ(specimen.getBufferSize(), 3u);
    EXPECT_NE(specimen.getFieldBuffer(), nullptr);
    EXPECT_EQ(specimen.getField<uint8_t>(0), 0u);
    EXPECT_EQ(specimen.getField<uint8_t>(1), 0u);
    EXPECT_EQ(specimen.getField<uint8_t>(2), 0u);

    // Test reading out of range.
    EXPECT_EQ(specimen.getField<uint8_t>(5), 0u);

    // Try setting some fields.
    specimen.setField(1, 0x12);
    EXPECT_TRUE(areBuffersEqual(specimen, { 0x90, 0x00, 0x00 }));
    EXPECT_EQ(specimen.getField<int>(0), 0);
    EXPECT_EQ(specimen.getField<int>(1), 0x12);
    EXPECT_EQ(specimen.getField<int>(2), 0);

    specimen.setField(2, 0xF1);
    EXPECT_TRUE(areBuffersEqual(specimen, { 0x90, 0xF1, 0x00 }));
    EXPECT_EQ(specimen.getField<int>(0), 0);
    EXPECT_EQ(specimen.getField<int>(1), 0x12);
    EXPECT_EQ(specimen.getField<int>(2), 0xF1);

    specimen.setField(0, 0x2);
    EXPECT_TRUE(areBuffersEqual(specimen, { 0x92, 0xF1, 0x00 }));
    EXPECT_EQ(specimen.getField<int>(0), 2);
    EXPECT_EQ(specimen.getField<int>(1), 0x12);
    EXPECT_EQ(specimen.getField<int>(2), 0xF1);
}

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


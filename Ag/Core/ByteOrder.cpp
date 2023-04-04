//! @file ByteOrder.cpp
//! @brief The definition of an interface to an object which swaps bytes
//! between particular byte orders.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Binary.hpp"
#include "Ag/Core/ByteOrder.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Bin {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An implementation of ByteOrder which swaps bytes.
struct SwapByteOrder : public ByteOrder
{
public:
    // Construction/Destruction
    SwapByteOrder() = default;
    virtual ~SwapByteOrder() = default;

    // Overrides
    virtual bool requiresSwap() const override { return true; }
    virtual uint16_t toHost(uint16_t bits) const { return byteSwap(bits); }
    virtual uint32_t toHost(uint32_t bits) const { return byteSwap(bits); }
    virtual uint64_t toHost(uint64_t bits) const { return byteSwap(bits); }

    virtual uint16_t toTarget(uint16_t bits) const { return byteSwap(bits); }
    virtual uint32_t toTarget(uint32_t bits) const { return byteSwap(bits); }
    virtual uint64_t toTarget(uint64_t bits) const { return byteSwap(bits); }
};

//! @brief An implementation of ByteOrder where the host and target byte order
//! are identical.
struct NoSwapByteOrder : public ByteOrder
{
public:
    // Construction/Destruction
    NoSwapByteOrder() = default;
    virtual ~NoSwapByteOrder() = default;

    // Overrides
    virtual bool requiresSwap() const override { return false; }
    virtual uint16_t toHost(uint16_t bits) const { return bits; }
    virtual uint32_t toHost(uint32_t bits) const { return bits; }
    virtual uint64_t toHost(uint64_t bits) const { return bits; }

    virtual uint16_t toTarget(uint16_t bits) const { return bits; }
    virtual uint32_t toTarget(uint32_t bits) const { return bits; }
    virtual uint64_t toTarget(uint64_t bits) const { return bits; }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
bool isHostLittleEndian()
{
    const uint16_t myValue = 0xFF00;

    return *reinterpret_cast<const uint8_t *>(&myValue) == 0x00;
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// ByteOrder Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets an object to convert byte order to/from little endian encoding.
const ByteOrder *ByteOrder::getLittleEndian()
{
    static const SwapByteOrder swapping;
    static const NoSwapByteOrder noSwapping;

    return isHostLittleEndian() ? static_cast<const ByteOrder *>(&noSwapping) :
        static_cast<const ByteOrder *>(&swapping);
}

//! @brief Gets an object to convert byte order to/from big endian encoding.
const ByteOrder *ByteOrder::getBigEndian()
{
    static const SwapByteOrder swapping;
    static const NoSwapByteOrder noSwapping;

    return isHostLittleEndian() ? static_cast<const ByteOrder *>(&swapping) :
        static_cast<const ByteOrder *>(&noSwapping);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Bin
////////////////////////////////////////////////////////////////////////////////


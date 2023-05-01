//! @file Ag/Core/ByteOrder.hpp
//! @brief The declaration of an interface to an object which swaps bytes
//! between particular byte orders.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_BYTE_ORDER_HPP__
#define __AG_CORE_BYTE_ORDER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

namespace Ag {
namespace Bin {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Implements functions which swap bytes between a particular
//! byte order and the host byte order.
struct ByteOrder
{
protected:
    // Construction/Destruction
    ByteOrder() = default;
    virtual ~ByteOrder() = default;

public:
    // Accessors
    static const ByteOrder *getLittleEndian();
    static const ByteOrder *getBigEndian();

    // Operations
    //! @brief Determines whether bytes need to be swapped to convert from
    //! native byte order to the described byte order.
    //! @retval true The order of bytes needs to be reversed to convert.
    //! @retval false The byte order represents that of the host architecture.
    virtual bool requiresSwap() const = 0;

    //! @brief Converts a 16-bit word from the described byte order to the
    //! native encoding of the host architecture.
    //! @param[in] bits The word encoded in the described byte order.
    //! @return The word encoded in the host byte order.
    virtual uint16_t toHost(uint16_t bits) const = 0;

    //! @brief Converts a 32-bit word from the described byte order to the
    //! native encoding of the host architecture.
    //! @param[in] bits The word encoded in the described byte order.
    //! @return The word encoded in the host byte order.
    virtual uint32_t toHost(uint32_t bits) const = 0;

    //! @brief Converts a 64-bit word from the described byte order to the
    //! native encoding of the host architecture.
    //! @param[in] bits The word encoded in the described byte order.
    //! @return The word encoded in the host byte order.
    virtual uint64_t toHost(uint64_t bits) const = 0;

    //! @brief Converts a 16-bit word from the host byte order to the byte order
    //! described by the current object.
    //! @param[in] bits The word encoded in host byte order to convert.
    //! @return The word in the described byte order.
    virtual uint16_t toTarget(uint16_t bits) const = 0;

    //! @brief Converts a 32-bit word from the host byte order to the byte order
    //! described by the current object.
    //! @param[in] bits The word encoded in host byte order to convert.
    //! @return The word in the described byte order.
    virtual uint32_t toTarget(uint32_t bits) const = 0;

    //! @brief Converts a 64-bit word from the host byte order to the byte order
    //! described by the current object.
    //! @param[in] bits The word encoded in host byte order to convert.
    //! @return The word in the described byte order.
    virtual uint64_t toTarget(uint64_t bits) const = 0;
};

}} // namespace Ag::Bin

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

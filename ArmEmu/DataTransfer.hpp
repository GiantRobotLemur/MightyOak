//! @file ArmEmu/DataTransfer.hpp
//! @brief The declaration of the layer of emulation which provides data
//! transfer functions for an ARM processor core.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_DATA_TRANSFER_HPP__
#define __ARM_EMU_DATA_TRANSFER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArithmeticLogicUnit.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Provides the layer of emulation which provides data transfer
//! functions for an ARM processor core.
class DataTransfer : public ArithmeticLogicUnit
{
private:
    // Internal Functions
    //! @brief Converts a integer bit position to a general purpose register
    //! identifier.
    //! @param[in] id The identifier of a register found by scanning the lower
    //! 16-bits of an LDM or STM instruction.
    //! @return The value as a general register.
    //! @note No range checking is performed.
    static constexpr GeneralRegister toRegisterId(int32_t id) noexcept
    {
        return static_cast<GeneralRegister>(static_cast<std::underlying_type<GeneralRegister>::type>(id));
    }
public:
    // Construction/Destruction
    DataTransfer() = default;
    virtual ~DataTransfer() = default;

    // Operations
    uint32_t execLoad(uint32_t instruction, uint32_t baseAddr,
                      uint32_t offset);
    uint32_t execStore(uint32_t instruction, uint32_t baseAddr,
                       uint32_t offset);
    uint32_t execLoadMultiple(uint32_t instruction, uint32_t baseAddr);
    uint32_t execStoreMultiple(uint32_t instruction, uint32_t baseAddr);
    uint32_t execSwap(uint32_t instruction);
};

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

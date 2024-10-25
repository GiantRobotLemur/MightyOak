//! @file ArmEmu/AluOperations_NoArch.cpp
//! @brief The definition of inner ALU operations which return status flags
//! implemented in platform-agnostic C++ code.
//! @author Nick Arkell
//! @copyright (c) 2023 Nick Arkell : Software Engineer
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "AluOperations.h"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////


namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Calculates the status flags after a logical data processing
//! operation.
//! @param[in] result The result of the operation.
//! @param[in] previousFlags The carry and overflow state produced by the barrel
//! shifter or preserved from a previous operation.
//! @return The new PSR status flag bits.
constexpr uint8_t logicResultStatus(uint32_t result, uint8_t previousFlags) noexcept
{
    // Preserve the overflow and carry flags.
    uint32_t status = previousFlags & 0x03;
    status |= (result == 0) ? StatusFlag_Z : 0;
    status |= (result & 0x80000000) >> 28;

    return static_cast<uint8_t>(status);
}

//! @brief Calculates the status flags after a 64-bit logical data processing
//! operation.
//! @param[in] result The 64-bit result of the operation.
//! @param[in] previousFlags The carry and overflow state produced by the barrel
//! shifter or preserved from a previous operation.
//! @return The new PSR status flag bits.
constexpr uint8_t logicResultStatus(uint64_t result, uint8_t previousFlags) noexcept
{
    // Preserve the overflow and carry flags.
    uint32_t status = previousFlags & 0x03;
    status |= (result == 0) ? StatusFlag_Z : 0;
    status |= (result & 0x8000000000000000ULL) >> 60;

    return static_cast<uint8_t>(status);
}

//! @brief Calculates the status flags after an add operation.
//! @param[in] op1 The first operand to the add operation.
//! @param[in] op2 The second operand to the add operation.
//! @param[in] result The result of the operation.
//! @return The new status flag bits.
constexpr uint8_t addResultStatus(uint32_t op1, uint32_t op2, uint32_t result) noexcept
{
    uint32_t flags = (~(op1 ^ op2) & (op2 ^ result) & 0x80000000) >> 31;
    flags |= (result < op1) ? StatusFlag_C : 0;
    flags |= (result == 0) ? StatusFlag_Z : 0;
    flags |= (result & 0x80000000) >> 28;

    return static_cast<uint8_t>(flags);
}

//! @brief Calculates the status flags after an subtract operation.
//! @param[in] op1 The first operand to the subtract operation.
//! @param[in] op2 The second operand to the subtract operation.
//! @param[in] result The result of the operation.
//! @return The new status flag bit
constexpr uint8_t subResultStatus(uint32_t op1, uint32_t op2, uint32_t result) noexcept
{
    uint32_t flags = ((op1 ^ op2) & (op1 ^ result) & 0x80000000) >> 31;
    flags |= (op1 < op2) ? StatusFlag_C : 0;
    flags |= (result == 0) ? StatusFlag_Z : 0;
    flags |= (result & 0x80000000) >> 28;

    return static_cast<uint8_t>(flags);
}

} // TED


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
extern "C" uint32_t ALU_Add(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    uint32_t result = op1 + op2;
    statusFlags = addResultStatus(op1, op2, result);

    return result;
}

extern "C" uint32_t ALU_Sub(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    uint32_t result = op1 - op2;
    statusFlags = subResultStatus(op1, op2, result);

    return result;
}

extern "C" uint32_t ALU_Adc(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    op2 += (statusFlags >> 1) & 1;
    uint32_t result = op1 + op2;
    statusFlags = addResultStatus(op1, op2, result);

    return result;
}

extern "C" uint32_t ALU_Sbc(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    op2 += (statusFlags >> 1) & 1;
    uint32_t result = op1 - op2;
    statusFlags = subResultStatus(op1, op2, result);

    return result;
}

extern "C" uint32_t ALU_Rsc(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    op1 += (statusFlags >> 1) & 1;
    uint32_t result = op2 - op1;
    statusFlags = subResultStatus(op2, op1, result);

    return result;
}

extern "C" uint32_t ALU_And(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    uint32_t result = op1 & op2;
    statusFlags = logicResultStatus(result, statusFlags);

    return result;
}

extern "C" uint32_t ALU_Or(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    uint32_t result = op1 | op2;
    statusFlags = logicResultStatus(result, statusFlags);

    return result;
}

extern "C" uint32_t ALU_Xor(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    uint32_t result = op1 ^ op2;
    statusFlags = logicResultStatus(result, statusFlags);

    return result;
}

extern "C" uint32_t ALU_Bic(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    uint32_t result = op1 & ~op2;
    statusFlags = logicResultStatus(result, statusFlags);

    return result;
}

extern "C" uint8_t ALU_Logic_Flags(uint32_t result, uint8_t statusFlags)
{
    // Preserve the Carry and oVerflow flag states.
    uint8_t flags = statusFlags & (StatusFlag_C | StatusFlag_V);

    flags |= static_cast<uint8_t>(result >> 28) & StatusFlag_N;
    flags |= result ? StatusFlag_None : StatusFlag_Z;

    return flags;
}

extern "C" uint32_t ALU_Mul(uint32_t op1, uint32_t op2, uint8_t &statusFlags)
{
    uint32_t result = op1 * op2;
    statusFlags = logicResultStatus(result, statusFlags);

    return result;
}

extern "C" uint32_t ALU_Mla(uint32_t op1, uint32_t op2, uint32_t op3,
                            uint8_t &statusFlags)
{
    uint32_t result = (op1 * op2) + op3;
    statusFlags = logicResultStatus(result, statusFlags);

    return result;
}

extern "C" uint8_t ALU_Umull(LongWord &rd, uint32_t rs, uint32_t rm,
                             uint8_t statusFlags)
{
    rd.Scalar = static_cast<uint64_t>(rm) * static_cast<uint64_t>(rs);

    return logicResultStatus(rd.Scalar, statusFlags);
}

extern "C" uint8_t ALU_Umlal(LongWord &rd, uint32_t rs, uint32_t rm,
                             uint8_t statusFlags)
{
    rd.Scalar += static_cast<uint64_t>(rm) * static_cast<uint64_t>(rs);

    return logicResultStatus(rd.Scalar, statusFlags);
}

extern "C" uint8_t ALU_Smull(LongWord &rd, uint32_t rs, uint32_t rm,
                             uint8_t statusFlags)
{
    rd.Scalar = static_cast<uint64_t>(static_cast<int64_t>(rm) * static_cast<int64_t>(rs));

    return logicResultStatus(rd.Scalar, statusFlags);
}

extern "C" uint8_t ALU_Smlal(LongWord &rd, uint32_t rs, uint32_t rm,
                             uint8_t statusFlags)
{
    rd.Scalar += static_cast<uint64_t>(static_cast<int64_t>(rm) * static_cast<int64_t>(rs));

    return logicResultStatus(rd.Scalar, statusFlags);
}

////////////////////////////////////////////////////////////////////////////////


//! @file ArmEmu/AluOperations.h
//! @brief The declaration of optimised functions which perform ALU operations
//! and return status flags.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ALU_OPERATIONS_HPP__
#define __ARM_EMU_ALU_OPERATIONS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "stdint.h"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
enum StatusFlags : uint8_t
{
    StatusFlag_None     = 0x00,
    StatusFlag_V        = 0x01,
    StatusFlag_C        = 0x02,
    StatusFlag_Z        = 0x04,
    StatusFlag_N        = 0x08,
    StatusFlag_Mask     = 0x0F,
    StatusFlag_Invalid  = 0xF0,
};

struct LongWord32
{
    uint32_t LoWord;
    uint32_t HiWord;
};

union LongWord
{
    LongWord32 Components;
    uint64_t Scalar;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Performs an add operation and produces ARM-compatible status flags
//! from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[out] statusFlags Receives the status flag values based on the result
//! of the operation.
//! @returns The result of the operation.
uint32_t ALU_Add(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs an subtract operation and produces ARM-compatible status flags
//! from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[out] statusFlags Receives the status flag values based on the result
//! of the operation.
//! @returns The result of the operation.
uint32_t ALU_Sub(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs an add-with-carry operation and produces ARM-compatible
//! status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in,out] statusFlags Contains the initial state of the carry flag and
//! receives the status flag values based on the operation result.
//! @returns The result of the operation.
uint32_t ALU_Adc(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs an subtract-with-carry operation and produces ARM-compatible
//! status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in,out] statusFlags Contains the initial state of the carry flag and
//! receives the status flag values based on the operation result.
//! @returns The result of the operation.
uint32_t ALU_Sbc(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs an reverse-subtract-with-carry operation and produces
//! ARM-compatible status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in,out] statusFlags Contains the initial state of the carry flag and
//! receives the status flag values based on the operation result.
//! @returns The result of the operation.
uint32_t ALU_Rsc(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs a logical AND operation and produces ARM-compatible
//! status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in,out] statusFlags Contains the states of the carry and
//! overflow flags to inherit and receives the status flag values based on the
//! result of the operation.
//! @returns The result of the operation.
uint32_t ALU_And(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs a logical OR operation and produces ARM-compatible
//! status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in,out] statusFlags Contains the states of the carry and
//! overflow flags to inherit and receives the status flag values based on the
//! result of the operation.
//! @returns The result of the operation.
uint32_t ALU_Or(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs a logical eXclusive OR operation and produces ARM-compatible
//! status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in,out] statusFlags Contains the states of the carry and
//! overflow flags to inherit and receives the status flag values based on the
//! result of the operation.
//! @returns The result of the operation.
uint32_t ALU_Xor(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs a bit clear operation (AND/NOT) and produces ARM-compatible
//! status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in,out] statusFlags Contains the states of the carry and
//! overflow flags to inherit and receives the status flag values based on the
//! result of the operation.
//! @returns The result of the operation.
uint32_t ALU_Bic(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs ARM-compatible status flags based on the result of a
//! logical operation, such as MOV.
//! @param[in] op1 The operand which is copied and sets status flags.
//! @param[in] statusFlags Contains the states of the carry and
//! overflow flags to inherit.
//! @returns The status flags based on inherited values and result.
uint8_t ALU_Logic_Flags(uint32_t result, uint8_t statusFlags);

//! @brief Performs an unsigned 32-bit multiply operation and produces
//! ARM-compatible status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in,out] statusFlags Contains the states of the overflow flag to
//! inherit and receives the status flag values based on the result of the
//! operation.
//! @returns The result of the operation.
uint32_t ALU_Mul(uint32_t op1, uint32_t op2, uint8_t &statusFlags);

//! @brief Performs an unsigned 32-bit multiply accumulate operation and
//! produces ARM-compatible status flags from the result.
//! @param[in] op1 The first operand.
//! @param[in] op2 The second operand.
//! @param[in] op2 The third operand, which is added to the product of the
//! first and second.
//! @param[in,out] statusFlags Contains the states of the overflow flag to
//! inherit and receives the status flag values based on the result of the
//! operation.
//! @returns The result of the operation.
uint32_t ALU_Mla(uint32_t op1, uint32_t op2, uint32_t op3, uint8_t &statusFlags);

//! @brief Performs an unsigned 64-bit multiply operation.
//! @param[out] rd Receives the components of the 64-bit result.
//! @param[in] rs The first multiplicand.
//! @param[in] rm The second multiplicand.
//! @param[in] statusFlags The initial values of the C and V status flags.
//! @return The status flags based on the result.
uint8_t ALU_Umull(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags);

//! @brief Performs an unsigned 64-bit multiply accumulate operation.
//! @param[in,out] rd Contains the components of the 64-bit scalar to
//! add and receives the 64-bit result.
//! @param[in] rs The first multiplicand.
//! @param[in] rm The second multiplicand.
//! @param[in] statusFlags The initial values of the C and V status flags.
//! @return The status flags based on the result.
uint8_t ALU_Umlal(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags);

//! @brief Performs a signed 64-bit multiply operation.
//! @param[out] rd Receives the components of the 64-bit result.
//! @param[in] rs The first multiplicand.
//! @param[in] rm The second multiplicand.
//! @param[in] statusFlags The initial values of the C and V status flags.
//! @return The status flags based on the result.
uint8_t ALU_Smull(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags);

//! @brief Performs a signed 64-bit multiply accumulate operation.
//! @param[in,out] rd Contains the components of the 64-bit scalar to
//! add and receives the 64-bit result.
//! @param[in] rs The first multiplicand.
//! @param[in] rm The second multiplicand.
//! @param[in] statusFlags The initial values of the C and V status flags.
//! @return The status flags based on the result.
uint8_t ALU_Smlal(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags);

#ifdef __cplusplus
} // extern "C"
#endif


#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

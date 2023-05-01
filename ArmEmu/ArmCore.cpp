//! @file ArmEmu/ArmCore.cpp
//! @brief The definition of functionality pervasive across all emulated
//! ARM core configurations.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <algorithm>

#include "ArmCore.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Initialises a matrix of conditional execution decoding flags.
//! @param[out] conditionMatrix The array of condition codes vs PSR flags used
//! to determine if an ARM instruction can be executed.
void initialiseConditionMatrix(uint16_t(&conditionMatrix)[16]) noexcept
{
    // Define shifted local copies of the status flags.
    static const uint8_t V = 0x01; // oVerflow
    static const uint8_t C = 0x02; // Carry
    static const uint8_t Z = 0x04; // Zero
    static const uint8_t N = 0x08; // Negative
    static const uint8_t NV = N | V;
    static const uint8_t ZNV = Z | N | V;

    // Zero the bit set.
    std::fill_n(conditionMatrix, 16, static_cast<uint16_t>(0));

    // Enumerate all combinations of status flags.
    for (uint8_t statusFlagState = 0; statusFlagState < 16; ++statusFlagState)
    {
        // Determine whether execution can occur for each condition code.
        uint16_t state = 0;

        // Condition EQ (0).
        if (statusFlagState & Z)
        {
            state |= 0x0001;
        }

        // Condition NE (1).
        if ((statusFlagState & Z) == 0)
        {
            state |= 0x0002;
        }

        // Condition CS/HS (2).
        if (statusFlagState & C)
        {
            state |= 0x0004;
        }

        // Condition CC/LO (3).
        if ((statusFlagState & C) == 0)
        {
            state |= 0x0008;
        }

        // Condition MI (4).
        if (statusFlagState & N)
        {
            state |= 0x0010;
        }

        // Condition PL (5).
        if ((statusFlagState & N) == 0)
        {
            state |= 0x0020;
        }

        // Condition VS (6).
        if (statusFlagState & V)
        {
            state |= 0x0040;
        }

        // Condition VC (7).
        if ((statusFlagState & V) == 0)
        {
            state |= 0x0080;
        }

        // Condition HI (8).
        if ((statusFlagState & C) &&
            ((statusFlagState & Z) == 0))
        {
            state |= 0x0100;
        }

        // Condition LS (9).
        if (((statusFlagState & C) == 0) ||
            (statusFlagState & Z))
        {
            state |= 0x0200;
        }

        // Condition GE (10).
        if (((statusFlagState & NV) == NV) ||
            ((statusFlagState & NV) == 0))
        {
            state |= 0x0400;
        }

        // Condition LT (11).
        if (((statusFlagState & NV) == V) ||
            ((statusFlagState & NV) == N))
        {
            state |= 0x0800;
        }

        // Condition GT (12).
        if (((statusFlagState & ZNV) == N) ||
            ((statusFlagState & ZNV) == V) ||
            ((statusFlagState & NV) == 0))
        {
            state |= 0x1000;
        }

        // Condition LE (13).
        if ((statusFlagState & Z) ||
            ((statusFlagState & NV) == N) ||
            ((statusFlagState & NV) == V))
        {
            state |= 0x2000;
        }

        // Condition AL (14).
        state |= 0x4000;

        // Condition NV (15).
        // Never execute, always clear.

        // Write the conditions back.
        conditionMatrix[statusFlagState] = state;
    }
}

//! @brief Determines if an instruction should be executed given the current
//! state of the PSR status flags.
//! @param[in] instruction The instruction bit-field to examine.
//! @param[in] statusFlags The current state of the status flags taken from
//! the most significant nubble of the PSR.
//! @retval true The instruction should be executed.
//! @retval false The instruction should not be executed.
bool canExecuteInstruction(uint32_t instruction, uint8_t statusFlags) noexcept
{
    // A set of flags indexed by instruction condition code with bits
    // set or cleared by the top nibble of the PSR.
    // NOTE The following was generated using initialiseConditionMatrix()
    // defined above and output as part of unit tests co be copied here.
    static const uint16_t ConditionMatrix[] = {
        0x56AA,
        0x7A6A,
        0x55A6,
        0x7966,
        0x76A9,
        0x6A69,
        0x76A5,
        0x6A65,
        0x7A9A,
        0x465A,
        0x7996,
        0x4556,
        0x6A99,
        0x6659,
        0x6A95,
        0x6655,
    };

    // The word is addressed by the current status flags value.
    // The bit is addressed by the condition encoded in the instruction.
    uint16_t bits = ConditionMatrix[statusFlags & 0x0F];

    return (bits & (1 << static_cast<uint8_t>(instruction >> 28))) != 0;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


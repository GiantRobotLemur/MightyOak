//! @file ArmDbg/StateFormatter.hpp
//! @brief The declaration of formatting utilities for debugger output.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DBG_STATE_FORMATTER_HPP__
#define __ARM_DBG_STATE_FORMATTER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <ostream>

#include "ArmEmu/ArmSystem.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class RingBufferTrace;

//! @brief Stateless formatting utilities for debugger output.
class StateFormatter
{
public:
    //! @brief Writes a full register dump to the stream.
    static void formatRegisters(std::ostream &out,
                                const uint32_t *regValues,
                                uint32_t cpsr,
                                ProcessorMode mode);

    //! @brief Writes a hex dump of a memory region.
    static void formatMemoryDump(std::ostream &out,
                                 uint32_t baseAddress,
                                 const uint8_t *data,
                                 uint32_t length);

    //! @brief Writes disassembled instructions.
    static void formatDisassembly(std::ostream &out,
                                  uint32_t baseAddress,
                                  const uint32_t *words,
                                  uint32_t count,
                                  uint32_t highlightAddr = UINT32_MAX);

    //! @brief Writes interrupt state.
    static void formatIrqState(std::ostream &out,
                               uint16_t irqStatus, uint16_t irqMask,
                               uint8_t firqStatus, uint8_t firqMask);

    //! @brief Writes the last N instruction trace entries.
    static void formatTrace(std::ostream &out,
                            const RingBufferTrace &trace,
                            size_t count);

    //! @brief Writes PC context with surrounding disassembly.
    static void formatPcContext(std::ostream &out,
                                uint32_t pc,
                                const uint32_t *surroundingWords,
                                uint32_t baseAddr,
                                uint32_t count);
};

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

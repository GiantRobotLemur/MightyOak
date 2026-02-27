//! @file ArmDbg/StateFormatter.cpp
//! @brief The definition of formatting utilities for debugger output.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <iomanip>

#include "AsmTools.hpp"
#include "ArmEmu/RingBufferTrace.hpp"

#include "StateFormatter.hpp"

namespace Mo {
namespace Arm {

namespace {

//! @brief Returns the display name for a processor mode.
const char *getModeName(ProcessorMode mode)
{
    switch (mode)
    {
    case ProcessorMode::User26:    return "USR26";
    case ProcessorMode::FastIrq26: return "FIQ26";
    case ProcessorMode::Irq26:     return "IRQ26";
    case ProcessorMode::Svc26:     return "SVC26";
    case ProcessorMode::User32:    return "USR32";
    case ProcessorMode::FastIrq32: return "FIQ32";
    case ProcessorMode::Irq32:     return "IRQ32";
    case ProcessorMode::Svc32:     return "SVC32";
    case ProcessorMode::Abort:     return "ABT32";
    case ProcessorMode::Undefined: return "UND32";
    case ProcessorMode::System:    return "SYS";
    default:                       return "???";
    }
}

//! @brief Disassembles a single instruction to a string.
void disasmToStream(std::ostream &out, uint32_t addr, uint32_t opcode)
{
    Asm::InstructionInfo info;

    if (info.disassemble(opcode, addr))
    {
        Ag::String text = info.toString();
        out << text.getUtf8Bytes();
    }
    else
    {
        out << "DCD 0x" << std::hex << std::setfill('0')
            << std::setw(8) << opcode << std::dec << std::setfill(' ');
    }
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// StateFormatter Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Writes a full register dump to the stream.
void StateFormatter::formatRegisters(std::ostream &out,
                                     const uint32_t *regValues,
                                     uint32_t cpsr,
                                     ProcessorMode mode)
{
    out << "=== Registers ===\n";

    for (int i = 0; i < 16; i += 4)
    {
        for (int j = 0; j < 4; ++j)
        {
            int regNum = i + j;
            const char *name;
            char nameBuf[8];

            switch (regNum)
            {
            case 13: name = "SP"; break;
            case 14: name = "LR"; break;
            case 15: name = "PC"; break;
            default:
                std::snprintf(nameBuf, sizeof(nameBuf), "R%d", regNum);
                name = nameBuf;
                break;
            }

            out << "  " << std::setw(3) << std::left << name << std::right
                << " = 0x" << std::hex << std::setfill('0') << std::setw(8)
                << regValues[regNum] << std::dec << std::setfill(' ');
        }

        out << "\n";
    }

    out << "  CPSR = 0x" << std::hex << std::setfill('0') << std::setw(8)
        << cpsr << std::dec << std::setfill(' ')
        << "  Mode: " << getModeName(mode);

    // Flag bits.
    out << "  [";
    out << ((cpsr & 0x80000000) ? 'N' : '-');
    out << ((cpsr & 0x40000000) ? 'Z' : '-');
    out << ((cpsr & 0x20000000) ? 'C' : '-');
    out << ((cpsr & 0x10000000) ? 'V' : '-');
    out << ((cpsr & 0x08000000) ? 'I' : '-');
    out << ((cpsr & 0x04000000) ? 'F' : '-');
    out << "]\n";
}

//! @brief Writes a hex dump of a memory region.
void StateFormatter::formatMemoryDump(std::ostream &out,
                                      uint32_t baseAddress,
                                      const uint8_t *data,
                                      uint32_t length)
{
    for (uint32_t offset = 0; offset < length; offset += 16)
    {
        // Address column.
        out << "  0x" << std::hex << std::setfill('0') << std::setw(8)
            << (baseAddress + offset) << ": " << std::dec << std::setfill(' ');

        // Hex bytes.
        uint32_t lineLen = std::min(uint32_t(16), length - offset);

        for (uint32_t i = 0; i < 16; ++i)
        {
            if (i < lineLen)
            {
                out << std::hex << std::setfill('0') << std::setw(2)
                    << static_cast<unsigned>(data[offset + i])
                    << std::dec << std::setfill(' ') << ' ';
            }
            else
            {
                out << "   ";
            }

            if (i == 7) out << ' ';
        }

        // ASCII sidebar.
        out << " |";

        for (uint32_t i = 0; i < lineLen; ++i)
        {
            char c = static_cast<char>(data[offset + i]);
            out << (std::isprint(static_cast<unsigned char>(c)) ? c : '.');
        }

        out << "|\n";
    }
}

//! @brief Writes disassembled instructions.
void StateFormatter::formatDisassembly(std::ostream &out,
                                       uint32_t baseAddress,
                                       const uint32_t *words,
                                       uint32_t count,
                                       uint32_t highlightAddr)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t addr = baseAddress + i * 4;
        const char *marker = (addr == highlightAddr) ? ">>> " : "    ";

        out << marker << "0x" << std::hex << std::setfill('0') << std::setw(8)
            << addr << ": " << std::setw(8) << words[i]
            << std::dec << std::setfill(' ') << "  ";

        disasmToStream(out, addr, words[i]);
        out << "\n";
    }
}

//! @brief Writes interrupt state.
void StateFormatter::formatIrqState(std::ostream &out,
                                    uint16_t irqStatus, uint16_t irqMask,
                                    uint8_t firqStatus, uint8_t firqMask)
{
    static const char *irqANames[] = {
        "POR",     "IFlBnk", "RII",    "TFlight",
        "POnRes",  "Timer0", "Timer1", "FhSet"
    };

    static const char *irqBNames[] = {
        "PodFIQ", "SndBuf", "Serial", "WiniIRQ",
        "DiscCh", "PodIRQ", "KartTx", "KartRx"
    };

    out << "=== IRQ State ===\n";
    out << "  IRQ Status A: 0x" << std::hex << std::setfill('0')
        << std::setw(2) << (irqStatus & 0xFF)
        << "  Mask A: 0x" << std::setw(2) << (irqMask & 0xFF)
        << std::dec << std::setfill(' ') << "\n";

    // Show individual IRQ A bits.
    for (int i = 0; i < 8; ++i)
    {
        bool active = (irqStatus >> i) & 1;
        bool masked = (irqMask >> i) & 1;

        if (active || masked)
        {
            out << "    [" << i << "] " << std::setw(8) << std::left
                << irqANames[i] << std::right
                << " Status=" << (active ? "SET" : "clr")
                << " Mask=" << (masked ? "EN" : "dis") << "\n";
        }
    }

    out << "  IRQ Status B: 0x" << std::hex << std::setfill('0')
        << std::setw(2) << ((irqStatus >> 8) & 0xFF)
        << "  Mask B: 0x" << std::setw(2) << ((irqMask >> 8) & 0xFF)
        << std::dec << std::setfill(' ') << "\n";

    for (int i = 0; i < 8; ++i)
    {
        bool active = (irqStatus >> (i + 8)) & 1;
        bool masked = (irqMask >> (i + 8)) & 1;

        if (active || masked)
        {
            out << "    [" << i << "] " << std::setw(8) << std::left
                << irqBNames[i] << std::right
                << " Status=" << (active ? "SET" : "clr")
                << " Mask=" << (masked ? "EN" : "dis") << "\n";
        }
    }

    out << "  FIRQ Status: 0x" << std::hex << std::setfill('0')
        << std::setw(2) << static_cast<unsigned>(firqStatus)
        << "  FIRQ Mask: 0x" << std::setw(2) << static_cast<unsigned>(firqMask)
        << std::dec << std::setfill(' ') << "\n";
}

//! @brief Writes the last N instruction trace entries.
void StateFormatter::formatTrace(std::ostream &out,
                                 const RingBufferTrace &trace,
                                 size_t count)
{
    size_t available = trace.getInstructionCount();
    size_t start = (available > count) ? (available - count) : 0;
    size_t display = available - start;

    out << "=== Trace (last " << display << " of " << available
        << " instructions) ===\n";

    for (size_t i = start; i < available; ++i)
    {
        const auto &entry = trace.getInstruction(i);
        uint32_t pc = entry.PC & 0x03FFFFFC;

        out << "  [" << std::setw(4) << (i - start) << "] 0x"
            << std::hex << std::setfill('0') << std::setw(8) << pc
            << ": " << std::setw(8) << entry.Opcode
            << std::dec << std::setfill(' ') << "  ";

        disasmToStream(out, pc, entry.Opcode);
        out << "\n";
    }
}

//! @brief Writes PC context with surrounding disassembly.
void StateFormatter::formatPcContext(std::ostream &out,
                                     uint32_t pc,
                                     const uint32_t *surroundingWords,
                                     uint32_t baseAddr,
                                     uint32_t count)
{
    out << "=== PC Context ===\n"
        << "  PC = 0x" << std::hex << std::setfill('0') << std::setw(8)
        << pc << std::dec << std::setfill(' ') << "\n\n";

    formatDisassembly(out, baseAddr, surroundingWords, count, pc);
}

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

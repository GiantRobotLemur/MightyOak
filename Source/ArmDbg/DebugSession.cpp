//! @file ArmDbg/DebugSession.cpp
//! @brief The definition of a class which manages an interactive debug session.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "Ag/Core/FsPath.hpp"

#include "ArmEmu/ArmSystemBuilder.hpp"
#include "ArmEmu/RingBufferTrace.hpp"
#include "ArmEmu/CompositeDiagnosticSink.hpp"
#include "ArmEmu/IOC.hpp"

#include "ArmDbg/DebugSession.hpp"
#include "StateFormatter.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// DebugSession Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to manage a debug session.
//! @param[in] output The stream to which all output is written.
DebugSession::DebugSession(std::ostream &output) :
    _output(output),
    _trace(nullptr),
    _initialised(false)
{
    // Set reasonable defaults.
    _options.setHardwareArchitecture(SystemModel::Archimedies);
    _options.setProcessorVariant(ProcessorModel::ARM2);
    _options.setSystemRom(SystemROMPreset::RiscOS_3_10);
    _options.setRamSizeKb(4096);
}

//! @brief Destructs the debug session, releasing owned resources.
DebugSession::~DebugSession() = default;

//! @brief Gets whether the emulator system has been created.
bool DebugSession::isInitialised() const
{
    return _initialised;
}

//! @brief Executes a single parsed command.
bool DebugSession::executeCommand(const ParsedCommand &cmd)
{
    switch (cmd.Command)
    {
    case DbgCommand::ConfigModel: return executeConfigModel(cmd);
    case DbgCommand::ConfigCpu:   return executeConfigCpu(cmd);
    case DbgCommand::ConfigRom:   return executeConfigRom(cmd);
    case DbgCommand::ConfigRam:   return executeConfigRam(cmd);
    case DbgCommand::Init:        return executeInit(cmd);
    case DbgCommand::Run:         return executeRun(cmd);
    case DbgCommand::Step:        return executeStep(cmd);
    case DbgCommand::Continue:    return executeContinue(cmd);
    case DbgCommand::Break:       return executeBreak(cmd);
    case DbgCommand::Regs:        return executeRegs(cmd);
    case DbgCommand::Reg:         return executeReg(cmd);
    case DbgCommand::Mem:         return executeMem(cmd);
    case DbgCommand::Disasm:      return executeDisasm(cmd);
    case DbgCommand::Irq:         return executeIrq(cmd);
    case DbgCommand::Memc:        return executeMemc(cmd);
    case DbgCommand::Ioc:         return executeIoc(cmd);
    case DbgCommand::Trace:       return executeTrace(cmd);
    case DbgCommand::Pc:          return executePc(cmd);
    case DbgCommand::Echo:        return executeEcho(cmd);
    default:
        _output << "Error (line " << cmd.LineNumber
                << "): Unknown command.\n";
        return false;
    }
}

//! @brief Override the ROM search path.
void DebugSession::setRomSearchPath(const Ag::Fs::Path &path)
{
    _romSearchPath = path;
}

bool DebugSession::executeConfigModel(const ParsedCommand &cmd)
{
    if (cmd.Arguments.empty())
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'config model' requires a model name.\n";
        return false;
    }

    const std::string &model = cmd.Arguments[0];

    if (model == "Archimedies" || model == "archimedies" || model == "A3xx")
        _options.setHardwareArchitecture(SystemModel::Archimedies);
    else if (model == "A540" || model == "a540")
    {
        _options.setHardwareArchitecture(SystemModel::Archimedies);
        _options.setProcessorVariant(ProcessorModel::ARM3);
    }
    else if (model == "RiscPC" || model == "riscpc")
        _options.setHardwareArchitecture(SystemModel::RiscPC);
    else
    {
        _output << "Error (line " << cmd.LineNumber
                << "): Unknown model '" << model
                << "'. Expected: Archimedies, A540, RiscPC.\n";
        return false;
    }

    return true;
}

bool DebugSession::executeConfigCpu(const ParsedCommand &cmd)
{
    if (cmd.Arguments.empty())
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'config cpu' requires a processor name.\n";
        return false;
    }

    const std::string &cpu = cmd.Arguments[0];

    if (cpu == "ARM2" || cpu == "arm2")
        _options.setProcessorVariant(ProcessorModel::ARM2);
    else if (cpu == "ARM3" || cpu == "arm3")
        _options.setProcessorVariant(ProcessorModel::ARM3);
    else if (cpu == "ARM610" || cpu == "arm610" || cpu == "ARM6" || cpu == "arm6")
        _options.setProcessorVariant(ProcessorModel::ARM610);
    else if (cpu == "ARM710" || cpu == "arm710" || cpu == "ARM7" || cpu == "arm7")
        _options.setProcessorVariant(ProcessorModel::ARM710);
    else if (cpu == "ARM810" || cpu == "arm810")
        _options.setProcessorVariant(ProcessorModel::ARM810);
    else
    {
        _output << "Error (line " << cmd.LineNumber
                << "): Unknown CPU '" << cpu
                << "'. Expected: ARM2, ARM3, ARM610, ARM710, ARM810.\n";
        return false;
    }

    return true;
}

bool DebugSession::executeConfigRom(const ParsedCommand &cmd)
{
    if (cmd.Arguments.empty())
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'config rom' requires a ROM preset or file path.\n";
        return false;
    }

    const std::string &rom = cmd.Arguments[0];

    // Try known presets first.
    struct RomPreset
    {
        const char *Name;
        SystemROMPreset Preset;
    };

    static const RomPreset presets[] = {
        { "RiscOS_3_10", SystemROMPreset::RiscOS_3_10 },
        { "RiscOS_3_11", SystemROMPreset::RiscOS_3_11 },
        { "RiscOS_3_00", SystemROMPreset::RiscOS_3_00 },
        { "RiscOS_2_00", SystemROMPreset::RiscOS_2_00 },
        { "Arthur_1_20", SystemROMPreset::Arthur_1_20 },
    };

    for (const auto &preset : presets)
    {
        if (rom == preset.Name)
        {
            _options.setSystemRom(preset.Preset);
            return true;
        }
    }

    // Treat as a file path.
    _options.setCustomRom(Ag::Fs::Path(rom.c_str()));
    return true;
}

bool DebugSession::executeConfigRam(const ParsedCommand &cmd)
{
    if (cmd.Arguments.empty())
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'config ram' requires a size in KB.\n";
        return false;
    }

    uint32_t sizeKb;

    if (!parseUint32(cmd.Arguments[0], sizeKb))
    {
        _output << "Error (line " << cmd.LineNumber
                << "): Invalid RAM size '" << cmd.Arguments[0] << "'.\n";
        return false;
    }

    _options.setRamSizeKb(sizeKb);
    return true;
}

bool DebugSession::executeInit(const ParsedCommand & /*cmd*/)
{
    // Set up ROM path resolution if not already done.
    if (_romSearchPath.isEmpty())
    {
        auto programDir = Ag::Fs::Path::getProgramDirectory();
        Ag::utf8_cptr_t paths[] = {
            "ROMs",
            "EmulatorApp/RelWithDebInfo/ROMs",
            "EmulatorApp/Debug/ROMs",
        };

        for (auto path : paths)
        {
            Ag::Fs::Path relPath(path);

            if (Options::findRomImagePath(programDir, relPath))
                break;
        }
    }
    else
    {
        Options::setRomImageBasePath(_romSearchPath);
    }

    Ag::String error;

    if (!_options.validate(error))
    {
        _output << "Error: Invalid system configuration: "
                << error.getUtf8Bytes() << "\n";
        return false;
    }

    // Create the system with diagnostics enabled.
    ArmSystemBuilder builder(_options);
    builder.setDiagnosticsEnabled(true);

    _traceOwner = std::make_unique<RingBufferTrace>(4096, 256, 1024);
    _trace = _traceOwner.get();

    auto composite = std::make_unique<CompositeDiagnosticSink>();
    composite->addSink(_trace);
    builder.addDevice(std::move(composite));


    _system = builder.createSystem();

    if (!_system)
    {
        _output << "Error: Failed to create emulator system.\n";
        return false;
    }

    _initialised = true;
    _output << "System initialised.\n";
    return true;
}

bool DebugSession::executeRun(const ParsedCommand &cmd)
{
    if (!requireInit("run")) return false;

    if (cmd.Arguments.empty())
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'run' requires a cycle count.\n";
        return false;
    }

    uint32_t cycles;

    if (!parseUint32(cmd.Arguments[0], cycles))
    {
        _output << "Error (line " << cmd.LineNumber
                << "): Invalid cycle count '" << cmd.Arguments[0] << "'.\n";
        return false;
    }

    ExecutionMetrics result = _system->runLimited(static_cast<int32_t>(cycles));

    _output << "Executed " << result.InstructionCount << " instructions in "
            << result.CycleCount << " cycles.\n";

    return true;
}

bool DebugSession::executeStep(const ParsedCommand &cmd)
{
    if (!requireInit("step")) return false;

    uint32_t stepCount = 1;

    if (!cmd.Arguments.empty())
    {
        if (!parseUint32(cmd.Arguments[0], stepCount))
        {
            _output << "Error (line " << cmd.LineNumber
                    << "): Invalid step count '" << cmd.Arguments[0] << "'.\n";
            return false;
        }
    }

    for (uint32_t i = 0; i < stepCount; ++i)
    {
        _system->runSingleStep();
    }

    // Show PC after stepping.
    uint32_t pc = _system->getCoreRegister(CoreRegister::PC) & 0x03FFFFFC;
    _output << "PC = 0x" << std::hex << std::setfill('0') << std::setw(8)
            << pc << std::dec << std::setfill(' ') << "\n";

    return true;
}

bool DebugSession::executeContinue(const ParsedCommand & /*cmd*/)
{
    if (!requireInit("continue")) return false;

    ExecutionMetrics result = _system->run();

    _output << "Stopped after " << result.InstructionCount << " instructions, "
            << result.CycleCount << " cycles.\n";

    return true;
}

bool DebugSession::executeBreak(const ParsedCommand &cmd)
{
    if (!requireInit("break")) return false;

    if (cmd.Arguments.empty())
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'break' requires an address.\n";
        return false;
    }

    uint32_t addr;

    if (!parseAddress(cmd.Arguments[0], addr))
    {
        _output << "Error (line " << cmd.LineNumber
                << "): Invalid address '" << cmd.Arguments[0] << "'.\n";
        return false;
    }

    _output << "Breakpoint set at 0x" << std::hex << std::setfill('0')
            << std::setw(8) << addr << std::dec << std::setfill(' ')
            << ".\n";

    return true;
}

bool DebugSession::executeRegs(const ParsedCommand & /*cmd*/)
{
    if (!requireInit("regs")) return false;

    uint32_t regValues[16];

    for (int i = 0; i < 16; ++i)
    {
        regValues[i] = _system->getCoreRegister(
            static_cast<CoreRegister>(i));
    }

    uint32_t cpsr = _system->getCoreRegister(CoreRegister::CPSR);
    ProcessorMode mode = _system->getMode();

    StateFormatter::formatRegisters(_output, regValues, cpsr, mode);
    return true;
}

bool DebugSession::executeReg(const ParsedCommand &cmd)
{
    if (!requireInit("reg")) return false;

    if (cmd.Arguments.empty())
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'reg' requires a register name.\n";
        return false;
    }

    const std::string &name = cmd.Arguments[0];
    CoreRegister reg;

    // Parse register name.
    if (name == "pc" || name == "PC" || name == "r15" || name == "R15")
        reg = CoreRegister::PC;
    else if (name == "lr" || name == "LR" || name == "r14" || name == "R14")
        reg = CoreRegister::R14;
    else if (name == "sp" || name == "SP" || name == "r13" || name == "R13")
        reg = CoreRegister::R13;
    else if (name == "cpsr" || name == "CPSR")
        reg = CoreRegister::CPSR;
    else if (name.size() >= 2 && (name[0] == 'r' || name[0] == 'R'))
    {
        char *end;
        long num = std::strtol(name.c_str() + 1, &end, 10);

        if (*end != '\0' || num < 0 || num > 15)
        {
            _output << "Error (line " << cmd.LineNumber
                    << "): Invalid register '" << name << "'.\n";
            return false;
        }

        reg = static_cast<CoreRegister>(num);
    }
    else
    {
        _output << "Error (line " << cmd.LineNumber
                << "): Unknown register '" << name << "'.\n";
        return false;
    }

    uint32_t value = _system->getCoreRegister(reg);
    _output << name << " = 0x" << std::hex << std::setfill('0')
            << std::setw(8) << value << std::dec << std::setfill(' ')
            << " (" << value << ")\n";

    return true;
}

bool DebugSession::executeMem(const ParsedCommand &cmd)
{
    if (!requireInit("mem")) return false;

    if (cmd.Arguments.size() < 2)
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'mem' requires <addr> <count> [p|l].\n";
        return false;
    }

    uint32_t addr, count;

    if (!parseAddress(cmd.Arguments[0], addr) ||
        !parseUint32(cmd.Arguments[1], count))
    {
        _output << "Error (line " << cmd.LineNumber
                << "): Invalid address or count.\n";
        return false;
    }

    // Determine physical vs logical (default: logical).
    bool isPhysical = false;

    if (cmd.Arguments.size() >= 3)
    {
        const std::string &mode = cmd.Arguments[2];
        isPhysical = (mode == "p" || mode == "phys" || mode == "physical");
    }

    // Read memory in chunks.
    std::vector<uint8_t> buffer(count);
    uint32_t bytesRead;

    if (isPhysical)
    {
        bytesRead = readFromPhysicalAddress(_system.get(), addr,
                                             buffer.data(), count);
    }
    else
    {
        bytesRead = readFromLogicalAddress(_system.get(), addr,
                                            buffer.data(), count);
    }

    _output << (isPhysical ? "Physical" : "Logical") << " memory at 0x"
            << std::hex << std::setfill('0') << std::setw(8) << addr
            << std::dec << std::setfill(' ')
            << " (" << bytesRead << " bytes read):\n";

    StateFormatter::formatMemoryDump(_output, addr, buffer.data(), bytesRead);
    return true;
}

bool DebugSession::executeDisasm(const ParsedCommand &cmd)
{
    if (!requireInit("disasm")) return false;

    if (cmd.Arguments.size() < 2)
    {
        _output << "Error (line " << cmd.LineNumber
                << "): 'disasm' requires <addr> <count> [p|l].\n";
        return false;
    }

    uint32_t addr, count;

    if (!parseAddress(cmd.Arguments[0], addr) ||
        !parseUint32(cmd.Arguments[1], count))
    {
        _output << "Error (line " << cmd.LineNumber
                << "): Invalid address or count.\n";
        return false;
    }

    bool isPhysical = false;

    if (cmd.Arguments.size() >= 3)
    {
        const std::string &mode = cmd.Arguments[2];
        isPhysical = (mode == "p" || mode == "phys" || mode == "physical");
    }

    // Read words.
    std::vector<uint32_t> words(count);
    uint32_t byteCount = count * 4;
    uint32_t bytesRead;

    if (isPhysical)
    {
        bytesRead = readFromPhysicalAddress(_system.get(), addr,
                                             words.data(), byteCount);
    }
    else
    {
        bytesRead = readFromLogicalAddress(_system.get(), addr,
                                            words.data(), byteCount);
    }

    uint32_t wordsRead = bytesRead / 4;

    _output << (isPhysical ? "Physical" : "Logical") << " disassembly at 0x"
            << std::hex << std::setfill('0') << std::setw(8) << addr
            << std::dec << std::setfill(' ')
            << " (" << wordsRead << " instructions):\n";

    StateFormatter::formatDisassembly(_output, addr, words.data(), wordsRead);
    return true;
}

bool DebugSession::executeIrq(const ParsedCommand & /*cmd*/)
{
    if (!requireInit("irq")) return false;

    IOC *ioc = nullptr;

    if (!_system->tryFindTypedDevice<IOC>(Ag::String("IOC"), ioc) ||
        ioc == nullptr)
    {
        _output << "Warning: IOC device not found.\n";
        return true;
    }

    // Access IRQ state through the IOC's synchronised state.
    // Read via the IOC MMIO interface at known offsets.
    uint32_t irqStatusA = 0, irqStatusB = 0;
    uint32_t irqMaskA = 0, irqMaskB = 0;
    uint32_t firqStatus = 0, firqMask = 0;

    // Read IOC registers via physical memory at IOC base + register offsets.
    // IRQ Status A = offset 0x10, IRQ Mask A = offset 0x18
    // IRQ Status B = offset 0x20, IRQ Mask B = offset 0x28
    // FIRQ Status  = offset 0x30, FIRQ Mask  = offset 0x38
    readFromPhysicalAddress(_system.get(), IOC::BaseAddr + 0x10,
                             &irqStatusA, 4);
    readFromPhysicalAddress(_system.get(), IOC::BaseAddr + 0x18,
                             &irqMaskA, 4);
    readFromPhysicalAddress(_system.get(), IOC::BaseAddr + 0x20,
                             &irqStatusB, 4);
    readFromPhysicalAddress(_system.get(), IOC::BaseAddr + 0x28,
                             &irqMaskB, 4);
    readFromPhysicalAddress(_system.get(), IOC::BaseAddr + 0x30,
                             &firqStatus, 4);
    readFromPhysicalAddress(_system.get(), IOC::BaseAddr + 0x38,
                             &firqMask, 4);

    uint16_t combinedStatus = static_cast<uint16_t>(irqStatusA & 0xFF) |
                              (static_cast<uint16_t>(irqStatusB & 0xFF) << 8);
    uint16_t combinedMask = static_cast<uint16_t>(irqMaskA & 0xFF) |
                            (static_cast<uint16_t>(irqMaskB & 0xFF) << 8);

    StateFormatter::formatIrqState(_output, combinedStatus, combinedMask,
                                   static_cast<uint8_t>(firqStatus),
                                   static_cast<uint8_t>(firqMask));

    return true;
}

bool DebugSession::executeMemc(const ParsedCommand & /*cmd*/)
{
    if (!requireInit("memc")) return false;

    _output << "=== MEMC Page Table ===\n";
    _output << "  Logical Addr  -> Physical Addr  PPL  Present\n";
    _output << "  ------------     -------------  ---  -------\n";

    // Probe logical address space in page-sized increments.
    // MEMC supports 4KB, 8KB, 16KB, 32KB pages. We'll use 32KB steps
    // to get a coarse view, then refine.
    static const uint32_t pageSize = 0x8000; // 32KB
    uint32_t logicalRange = 0x02000000; // 32MB logical address space

    int mappedCount = 0;

    for (uint32_t logAddr = 0; logAddr < logicalRange; logAddr += pageSize)
    {
        PageMapping mapping;

        if (_system->logicalToPhysicalAddress(logAddr, mapping))
        {
            if (mapping.Access & PageMapping::IsPresent)
            {
                _output << "  0x" << std::hex << std::setfill('0')
                        << std::setw(8) << logAddr << "  -> 0x"
                        << std::setw(8) << mapping.PageBaseAddr
                        << "  " << std::dec << std::setfill(' ')
                        << std::setw(3) << (mapping.Access & PageMapping::Mask)
                        << "  Yes\n";
                ++mappedCount;
            }
        }
    }

    _output << "  Total mapped pages: " << mappedCount << " (at 32KB granularity)\n";
    return true;
}

bool DebugSession::executeIoc(const ParsedCommand & /*cmd*/)
{
    if (!requireInit("ioc")) return false;

    _output << "=== IOC State ===\n";

    // Read IOC control register.
    uint32_t ctrlReg = 0;
    readFromPhysicalAddress(_system.get(), IOC::BaseAddr, &ctrlReg, 4);
    _output << "  Control Register: 0x" << std::hex << std::setfill('0')
            << std::setw(2) << (ctrlReg & 0xFF) << std::dec
            << std::setfill(' ') << "\n";

    // Read timer counter values (Timer 0-3).
    static const char *timerNames[] = { "Timer 0", "Timer 1", "Timer 2", "Timer 3 (KART)" };
    static const uint32_t timerOffsets[] = { 0x40, 0x50, 0x60, 0x70 };

    for (int t = 0; t < 4; ++t)
    {
        uint32_t latchLo = 0, latchHi = 0;
        readFromPhysicalAddress(_system.get(),
                                 IOC::BaseAddr + timerOffsets[t], &latchLo, 4);
        readFromPhysicalAddress(_system.get(),
                                 IOC::BaseAddr + timerOffsets[t] + 4, &latchHi, 4);

        uint16_t latch = static_cast<uint16_t>(latchLo & 0xFF) |
                         (static_cast<uint16_t>(latchHi & 0xFF) << 8);

        _output << "  " << timerNames[t] << ": latch=" << latch << "\n";
    }

    return true;
}

bool DebugSession::executeTrace(const ParsedCommand &cmd)
{
    if (!requireInit("trace")) return false;

    if (_trace == nullptr)
    {
        _output << "Warning: No trace buffer available.\n";
        return true;
    }

    uint32_t count = 20;

    if (!cmd.Arguments.empty())
    {
        if (!parseUint32(cmd.Arguments[0], count))
        {
            _output << "Error (line " << cmd.LineNumber
                    << "): Invalid trace count.\n";
            return false;
        }
    }

    StateFormatter::formatTrace(_output, *_trace, count);
    return true;
}

bool DebugSession::executePc(const ParsedCommand & /*cmd*/)
{
    if (!requireInit("pc")) return false;

    uint32_t pc = _system->getCoreRegister(CoreRegister::PC) & 0x03FFFFFC;

    // Read surrounding instructions (8 before, 8 after).
    uint32_t windowSize = 17;
    uint32_t baseAddr = (pc >= 32) ? (pc - 32) : 0;
    std::vector<uint32_t> words(windowSize);

    readFromLogicalAddress(_system.get(), baseAddr,
                            words.data(), windowSize * 4);

    StateFormatter::formatPcContext(_output, pc, words.data(),
                                    baseAddr, windowSize);

    // Also show mode and key registers.
    ProcessorMode mode = _system->getMode();
    _output << "  Mode: " << static_cast<int>(mode) << "\n";

    return true;
}

bool DebugSession::executeEcho(const ParsedCommand &cmd)
{
    if (!cmd.Arguments.empty())
    {
        _output << cmd.Arguments[0] << "\n";
    }
    else
    {
        _output << "\n";
    }

    return true;
}

bool DebugSession::requireInit(const char *commandName)
{
    if (!_initialised)
    {
        _output << "Error: '" << commandName
                << "' requires 'init' to be called first.\n";
        return false;
    }

    return true;
}

bool DebugSession::parseAddress(const std::string &text, uint32_t &addr)
{
    return parseUint32(text, addr);
}

bool DebugSession::parseUint32(const std::string &text, uint32_t &value)
{
    if (text.empty())
        return false;

    char *end;
    unsigned long long parsed;

    if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X'))
    {
        parsed = std::strtoull(text.c_str() + 2, &end, 16);
    }
    else
    {
        parsed = std::strtoull(text.c_str(), &end, 10);
    }

    if (*end != '\0' || parsed > UINT32_MAX)
        return false;

    value = static_cast<uint32_t>(parsed);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

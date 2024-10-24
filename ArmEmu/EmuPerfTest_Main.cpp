//! @file EmuPerfTest_Main.cpp
//! @brief The definition of the entry point for the EmuPerfTest CLI performance
//! measurement tool.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core.hpp"
#include "AsmTools.hpp"

#include "ArmEmu.hpp"
#include "DhrystoneProgram.hpp"
#include "ArmEmu/ArmSystemBuilder.hpp"
#include "TestBedHardware.inl"

// A bit lazy, but it's only one file.
// TODO: If EmuPerfTest is expanded to more than a single file, resolve the Ag
// namespaced element references.
using namespace Ag;

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum class EmuPerfTestCommand
{
    Auto,
    ShowHelp,
    ListConfigs,
    RunTest,
};

//! @brief Defines the emulator configurations which can be selected for test.
enum Configuration
{
    None,
    ArmV2_Test,
    ArmV2a_Test,
    ArmV2a_FPA_Test,
    ArmV3_Test,
    ArmV3_FPA_Test,
    ArmV4_Test,
    ArmV4_FPA_Test,
};

const EnumInfo<Configuration> &getConfigMetadata()
{
    static const EnumInfo<Configuration> instance({
        { Configuration::ArmV2_Test, "ARMv2-Test" },
        { Configuration::ArmV2a_Test, "ARMv2a-Test" },
        { Configuration::ArmV2a_FPA_Test, "ARMv2a-FPA-Test" },
        { Configuration::ArmV3_Test, "ARMv3-Test" },
        { Configuration::ArmV3_FPA_Test, "ARMv3-FPA-Test" },
        { Configuration::ArmV4_Test, "ARMv4-Test" },
        { Configuration::ArmV4_FPA_Test, "ARMv4-FPA-Test" },
    });

    return instance;
}

//! @brief Defines command line arguments for the EmuPerfTest tools.
class EmuPerfTestArgs : public Cli::ProgramArguments
{
private:
    // Internal Types
    enum Option
    {
        ShowHelp,
        CycleCount,
    };

    // Internal Fields
    EmuPerfTestCommand _command;
    Configuration _config;
    uint32_t _cycleCount;

    // Internal Functions
public:
    static Cli::Schema createSchema()
    {
        Cli::SchemaBuilder builder;
        builder.setDescription("An ARM emulation performance measurement tool.");
        builder.defineValueArgument("config name", Cli::UpToOne);

        builder.defineOption(Option::ShowHelp,
                             "Display command line help. Specify 'configs' to "
                             "list valid test configurations.",
                             Cli::OptionValue::Optional, "topic name");
        builder.defineAlias(Option::ShowHelp, U'?');
        builder.defineAlias(Option::ShowHelp, "help");

        builder.defineOption(Option::CycleCount,
                             "Specifies the number of Dhrystone cycles to execute.",
                             Cli::OptionValue::Mandatory, "cycle count");
        builder.defineAlias(Option::CycleCount, U'c');
        builder.defineAlias(Option::CycleCount, "cycles");

        return builder.createSchema();
    }

    // Construction/Destruction
    EmuPerfTestArgs() :
        Cli::ProgramArguments(createSchema()),
        _command(EmuPerfTestCommand::Auto),
        _config(Configuration::None),
        _cycleCount(0)
    {
    }

    virtual ~EmuPerfTestArgs() = default;

    // Accessors
    EmuPerfTestCommand getCommand() const { return _command; }
    Configuration getConfiguration() const { return _config; }
    uint32_t getCycleCount() const { return _cycleCount; }

protected:
    // Overrides
    // Inherited from Cli::ProgramArguments.
    virtual bool processOption(uint32_t id, const String &value, String &error) override
    {
        bool isOK = true;

        switch (id)
        {
        case ShowHelp:
            if (value.isEmpty())
            {
                _command = EmuPerfTestCommand::ShowHelp;
            }
            else if (value.compareIgnoreCase("CONFIGS") == 0)
            {
                _command = EmuPerfTestCommand::ListConfigs;
            }
            else
            {
                isOK = false;
                error = String::format(FormatInfo::getDisplay(),
                                       "Unknown help topic '{0}'.",
                                       { value });
            }
            break;

        case CycleCount:
            if (value.tryParseScalar(_cycleCount) == false)
            {
                error = String::format(FormatInfo::getDisplay(),
                                       "Invalid cycle count '{0}' specified.",
                                       { value });
                isOK = false;
            }
            break;

        default:
            isOK = false;
            break;
        }

        return isOK;
    }

    // Inherited from Cli::ProgramArguments.
    virtual bool processArgument(const String &argument, String &error) override
    {
        bool isOK = false;

        if (_config == Configuration::None)
        {
            const auto &configType = getConfigMetadata();
            Configuration selectedConfig;

            if (configType.tryParse(argument.toUtf8View(), selectedConfig))
            {
                _config = selectedConfig;
                isOK = true;
            }
            else
            {
                error = String::format("Unknown test configuration '{0}'.",
                                       { argument });
            }
        }
        else
        {
            error = "Only one test configuration can be specified.";
        }

        return isOK;
    }

    // Inherited from Cli::ProgramArguments.
    virtual void postProcess() override
    {
        if (_command == EmuPerfTestCommand::Auto)
        {
            _command = EmuPerfTestCommand::RunTest;
        }

        if (_command == EmuPerfTestCommand::RunTest)
        {
            if (_config == Configuration::None)
            {
                _config = Configuration::ArmV2_Test;
            }

            if (_cycleCount == 0)
            {
                // Set the default count of loops to execute.
#ifdef _DEBUG
                _cycleCount = 500000;
#else
                _cycleCount = 3000000;
#endif
            }
        }
    }
};

struct GenerateBreakPoint
{
private:
    Asm::InstructionInfo _instruction;
    uint16_t _index;
public:
    GenerateBreakPoint() :
        _index(0)
    {
        _instruction.reset(Asm::InstructionMnemonic::Bkpt,
                           Asm::OperationClass::Breakpoint);
    }

    uint32_t operator()()
    {
        _instruction.getBreakpointParameters().Comment = _index++;

        String error;
        uint32_t instruction = 0;
        if (_instruction.assemble(instruction, 0x0000, error) == false)
        {
            throw Ag::OperationException("Could not assemble numbered breakpoint.");
        }

        return instruction;
    }
};

//! @brief The object representing the root application object.
class EmuPerfTestApp : public App
{
private:
    // Internal Fields
    EmuPerfTestCommand _command;
    Configuration _config;
    uint32_t _cycleCount;

    // Internal Functions
    IArmSystemUPtr initialiseEmbeddedTestSystem(Options &systemOptions) const
    {
        IArmSystemUPtr testSystem;
        systemOptions.setSystemRom(SystemROMPreset::Custom);
        systemOptions.setHardwareArchitecture(SystemModel::TestBed);

        switch (_config)
        {
        case ArmV2_Test:
            systemOptions.setProcessorVariant(ProcessorModel::ARM2);
            break;

        case ArmV2a_Test:
            systemOptions.setProcessorVariant(ProcessorModel::ARM3);
            break;

        case ArmV2a_FPA_Test:
            systemOptions.setProcessorVariant(ProcessorModel::ARM3_FPA);
            break;

        case ArmV3_Test:
            systemOptions.setProcessorVariant(ProcessorModel::ARM610);
            break;

        case ArmV3_FPA_Test:
            systemOptions.setProcessorVariant(ProcessorModel::ARM710_FPA);
            break;

        case ArmV4_Test:
            systemOptions.setProcessorVariant(ProcessorModel::ARM810);
            break;

        case ArmV4_FPA_Test:
            systemOptions.setProcessorVariant(ProcessorModel::ARM810_FPA);
            break;

        default:
            puts("Error: Emulated system configuration not supported.");
            return testSystem;
        }

        Ag::String error;

        if (systemOptions.validate(error))
        {
            // Create the emulated system based on the settings provided.
            ArmSystemBuilder builder(systemOptions);
            testSystem = builder.createSystem();

            // Create a ROM image filled with breakpoints.
            std::vector<uint32_t> rom;

            std::generate_n(std::back_inserter(rom),
                            TestBedHardware::RomSize / 4,
                            GenerateBreakPoint());

            // Create an instruction which at the hardware reset vector which
            // branches to the first work in memory.
            Asm::InstructionInfo resetBranch(Asm::InstructionMnemonic::B,
                                             Asm::OperationClass::Branch);
            resetBranch.getBranchParameters().Address = TestBedHardware::RamBase;

            if (resetBranch.assemble(rom.front(), TestBedHardware::RomBase, error) == false)
            {
                throw Ag::OperationException("Could not assemble reset vector.");
            }

            // Fill the ROM with breakpoints and a branch to RAM on reset.
            writeToLogicalAddress(testSystem.get(), TestBedHardware::RomBase,
                                  rom.data(), static_cast<uint32_t>(rom.size() * 4),
                                  true);

            // Copy the assembled code into RAM.
            size_t byteCount;
            const void *program = getDhrystoneData(byteCount);

            writeToLogicalAddress(testSystem.get(), TestBedHardware::RamBase,
                                  program, static_cast<uint32_t>(byteCount));

            // Setup a full-descending stack in R13 after the reset.
            uint32_t ramEnd = TestBedHardware::RamEnd - 4;
            testSystem->setCoreRegister(CoreRegister::R13, ramEnd);
        }
        else
        {
            // Option validation failed.
            printf("Error: %s\n", error.getUtf8Bytes());
        }

        return testSystem;
    }

    void displayConfigs() const
    {
        std::string buffer;
        buffer.assign("Configurations: ");
        Cli::appendValidValues(buffer, getConfigMetadata());

        puts(buffer.c_str());
    }

    bool runTest() const
    {
        Options testSystemOptions;
        IArmSystemUPtr testSystem = initialiseEmbeddedTestSystem(testSystemOptions);

        if (!testSystem)
            return false;

        // Pass the look count to the program.
        testSystem->setCoreRegister(CoreRegister::R0, _cycleCount);

        std::string output;
        appendFormat(FormatInfo::getDisplay(),
                     "Selected {0} processor.\n"
                     "Running {1} loops of the Dhrystone 2.1 benchmark...", output,
                     { getProcessorModelType().toDisplayName(testSystemOptions.getProcessorVariant()),
                       _cycleCount });

        puts(output.c_str());
        ExecutionMetrics metrics = testSystem->run();

        uint32_t endPC = testSystem->getCoreRegister(CoreRegister::PC) - 12;

        if (endPC < 0x20)
        {
            const char *reason = "Unknown";

            switch (endPC >> 2)
            {
            case 0x00: reason = "Reset"; break;
            case 0x01: reason = "Unidentified instruction"; break;
            case 0x02: reason = "Software interrupt"; break;
            case 0x03: reason = "Pre-fetch abort"; break;
            case 0x04: reason = "Data abort"; break;
            case 0x05: reason = "Address exception"; break;
            case 0x06: reason = "Interrupt request"; break;
            case 0x07: reason = "Fast interrupt request"; break;
            }

            printf("Program crashed: %s\nRegisters:\n", reason);
            for (uint8_t i = 0; i < 16; i += 2)
            {
                printf("\tR%u = 0x%.8X, R%u = 0x%.8X\n",
                       i, testSystem->getCoreRegister(fromScalar<CoreRegister>(i)),
                       i + 1, testSystem->getCoreRegister(fromScalar<CoreRegister>(i + 1)));
            }
        }

        double durationInSeconds = HighResMonotonicTimer::getTimeSpan(metrics.ElapsedTime);
        double clockSpeedHz = metrics.calculateClockFrequency();

        output.clear();
        appendFormat(FormatInfo::getDisplay(),
                     "Executed {0} CPU cycles in {1:F2} seconds (~{2:F0} Dhrystone's per second).\n"
                     "Simulated clock speed: {3:F2} MHz\n"
                     "Simulated performance: {4:F2} MIPS\n", output,
                     { metrics.CycleCount, durationInSeconds,
                       std::floor(_cycleCount / durationInSeconds),
                       clockSpeedHz / 1.0e6,
                       metrics.calculateSpeedInMIPS() });

        puts(output.c_str());
        return true;
    }
public:
    // Construction/Destruction
    EmuPerfTestApp() :
        _command(EmuPerfTestCommand::Auto),
        _config(Configuration::None),
        _cycleCount(100)
    {
    }

    virtual ~EmuPerfTestApp() = default;

protected:
    // Overrides
    // Inherited from App.
    virtual CommandLineUPtr createCommandLineArguments() const override
    {
        return std::make_unique<EmuPerfTestArgs>();
    }

    // Inherited from App.
    virtual bool initialise(const Cli::ProgramArguments *args)
    {
        bool isOK = false;

        if (const EmuPerfTestArgs *testArgs = dynamic_cast<const EmuPerfTestArgs *>(args))
        {
            _command = testArgs->getCommand();
            isOK = true;

            if (testArgs->getCommand() == EmuPerfTestCommand::RunTest)
            {
                // Extract the options we need.
                _config = testArgs->getConfiguration();
                _cycleCount = testArgs->getCycleCount();
            }
            else if (_command == EmuPerfTestCommand::Auto)
            {
                puts("Unknown command.");
                isOK = false;
            }
        }

        return isOK;
    }

    // Inherited from App.
    virtual int run()
    {
        int processResult = 0;

        switch (_command)
        {
        case EmuPerfTestCommand::ShowHelp:
            // Display command line help.
            puts(EmuPerfTestArgs::createSchema().getHelpText(100).getUtf8Bytes());
            break;

        case EmuPerfTestCommand::ListConfigs:
            displayConfigs();
            break;

        case EmuPerfTestCommand::RunTest:
            processResult = runTest() ? 0 : 1;
            break;

        default:
            processResult = 1;
            puts("Unknown command.");
            break;
        }

        return processResult;
    }
};

} // Anonymous namespace

}} // namespace Mo::Arm

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_MAIN(Mo::Arm::EmuPerfTestApp);

////////////////////////////////////////////////////////////////////////////////


//! @file EmuPerfTest_Main.cpp
//! @brief The definition of the entry point for the EmuPerfTest CLI performance
//! measurement tool.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
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

#include "ArmSystem.inl"
#include "SystemConfigurations.inl"

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
};

const EnumInfo<Configuration> &getConfigMetadata()
{
    static const EnumInfo<Configuration> instance({
        { Configuration::ArmV2_Test, "ARMv2-Test" },
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

        return builder.createSchema();
    }

    // Construction/Destruction
    EmuPerfTestArgs() :
        Cli::ProgramArguments(createSchema()),
        _command(EmuPerfTestCommand::Auto),
        _config(Configuration::None)
    {
    }

    virtual ~EmuPerfTestArgs() = default;

    // Accessors
    EmuPerfTestCommand getCommand() const { return _command; }
    Configuration getConfiguration() const { return _config; }

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
                error = String::format("Unknown help topic '{0}'.",
                                       { value });
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

        if ((_command == EmuPerfTestCommand::RunTest) &&
            (_config == Configuration::None))
        {
            _config = Configuration::ArmV2_Test;
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

    // Internal Functions
    static void initialiseEmbeddedTestSystem(IArmSystem *testSystem)
    {
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

        String error;
        if (resetBranch.assemble(rom.front(), TestBedHardware::RomBase, error) == false)
        {
            throw Ag::OperationException("Could not assemble reset vector.");
        }

        //auto *testSystem = new ArmSystem<ArmV2TestSystemTraits>();

        // Fill the ROM with breakpoints and a branch to RAM on reset.
        testSystem->writeToLogicalAddress(TestBedHardware::RomBase,
                                          static_cast<uint32_t>(rom.size() * 4),
                                          rom.data());

        // Copy the assembled code into RAM.
        size_t byteCount;
        const void *program = getDhrystoneData(byteCount);

        testSystem->writeToLogicalAddress(TestBedHardware::RamBase,
                                          static_cast<uint32_t>(byteCount),
                                          program);

        // Assemble a breakpoint at the end of the program.
        resetBranch.reset(Asm::InstructionMnemonic::Bkpt,
                          Asm::OperationClass::Breakpoint);
        resetBranch.getBreakpointParameters().Comment = 0xFFFF;

        uint32_t bkptInstruction;
        if (resetBranch.assemble(bkptInstruction, 0x0000, error) == false)
        {
            throw Ag::OperationException("Could not assemble final break point.");
        }

        // Write the final breakpoint to RAM.
        testSystem->writeToLogicalAddress(static_cast<uint32_t>(TestBedHardware::RamBase + byteCount),
                                          4, &bkptInstruction);

        // Setup a full-descending stack in R13 after the reset.
        uint32_t ramEnd = TestBedHardware::RamEnd - 4;
        testSystem->setCoreRegister(CoreRegister::R13, ramEnd);
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
        ArmSystem<ArmV2TestSystemTraits> testSystem;
        initialiseEmbeddedTestSystem(&testSystem);

        // Set count of loops to execute.
#ifdef _DEBUG
        constexpr uint32_t loops = 500000;
#else
        constexpr uint32_t loops = 3000000;
#endif

        // Pass the look count to the program.
        testSystem.setCoreRegister(CoreRegister::R0, loops);

        std::string output;
        appendFormat(FormatInfo::getDisplay(),
                     "Running {0} loops of the Dhrystone 2.1 benchmark...",
                     output, { loops });

        puts(output.c_str());
        ExecutionMetrics metrics = testSystem.run();

        uint32_t endPC = testSystem.getCoreRegister(CoreRegister::PC) - 12;

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
                       i, testSystem.getCoreRegister(fromScalar<CoreRegister>(i)),
                       i + 1, testSystem.getCoreRegister(fromScalar<CoreRegister>(i + 1)));
            }
        }

        double durationInSeconds = HighResMonotonicTimer::getTimeSpan(metrics.ElapsedTime);
        double clockSpeedHz = metrics.calculateClockFrequency();

        output.clear();
        appendFormat(FormatInfo::getDisplay(),
                     "Executed {0} cycles in {1:F2} seconds.\n"
                     "Simulated clock speed: {2:F2} MHz\n"
                     "Simulated performance: {3:F2} MIPS\n", output,
                     { metrics.CycleCount, durationInSeconds,
                       clockSpeedHz / 1.0e6,
                       metrics.calculateSpeedInMIPS() });

        puts(output.c_str());
        return true;
    }
public:
    // Construction/Destruction
    EmuPerfTestApp() :
        _command(EmuPerfTestCommand::Auto),
        _config(Configuration::None)
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


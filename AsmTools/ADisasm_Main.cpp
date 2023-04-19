//! @file ADisasm_Main.cpp
//! @brief The definition of the entry point for the ADisasm CLI assembler tool.
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

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum class ADisasmCommand
{
    Auto,
    ShowHelp,
    Disassemble,
};

//! @brief Defines command line arguments for the ADisasm tools.
class ADisasmArgs : public Cli::ProgramArguments
{
private:
    // Internal Types
    enum Option
    {
        ShowHelp,
        OutputFile,
        TargetInstructionSet,
        Extension,
        LoadAddress,
    };

    // Internal Fields
    String _inputFile;
    String _outputFile;
    InstructionSet _instructionSet;
    uint32_t _disasmFlags;
    uint32_t _loadAddress;
    ADisasmCommand _command;
    bool _usePIC;

    // Internal Functions
    static Cli::Schema createSchema()
    {
        Cli::SchemaBuilder builder;
        builder.setDescription("A 32-bit ARM disassembler.");
        builder.defineValueArgument("object code file", Cli::UpToOne);

        builder.defineOption(Option::ShowHelp, "Display command line help.",
                             Cli::OptionValue::None);
        builder.defineAlias(Option::ShowHelp, U'?');
        builder.defineAlias(Option::ShowHelp, "help");

        builder.defineOption(Option::OutputFile, "Specifies the output text file.",
                             Cli::OptionValue::Mandatory, "output file");
        builder.defineAlias(Option::OutputFile, U'o');
        builder.defineAlias(Option::OutputFile, "output");

        std::string description;
        description.assign("Specifies the instruction set the object code uses. ");
        Cli::appendValidValues(description, getInstructionSetType());

        builder.defineOption(Option::TargetInstructionSet,
                             description.c_str(),
                             Cli::OptionValue::Mandatory, "instruction set");
        builder.defineAlias(Option::TargetInstructionSet, U's');
        builder.defineAlias(Option::TargetInstructionSet, "instructionset");

        builder.defineOption(Option::Extension,
                             "Interpret extension instruction sets. Valid values are FPA and Thumb.",
                             Cli::OptionValue::Mandatory, "extension");
        builder.defineAlias(Option::Extension, U'x');
        builder.defineAlias(Option::Extension, "extension");

        builder.defineOption(Option::LoadAddress, "Specifies the base address of the object code.",
                             Cli::OptionValue::Mandatory, "base address");
        builder.defineAlias(Option::LoadAddress, U'b');
        builder.defineAlias(Option::LoadAddress, "base");

        return builder.createSchema();
    }
public:
    // Construction/Destruction
    ADisasmArgs() :
        Cli::ProgramArguments(createSchema()),
        _instructionSet(InstructionSet::Max),
        _disasmFlags(InstructionInfo::UseStackModesOnR13),
        _loadAddress(0xFFFFFFFF),
        _command(ADisasmCommand::Auto),
        _usePIC(true)
    {
    }

    virtual ~ADisasmArgs() = default;

    // Accessors
    ADisasmCommand getCommand() const { return _command; }

    uint32_t getDisassemblyOptions() const
    {
        uint32_t flags = _disasmFlags;

        switch (_instructionSet)
        {
        case InstructionSet::ArmV2: flags |= InstructionInfo::ARMv2; break;
        case InstructionSet::ArmV2a: flags |= InstructionInfo::ARMv2a; break;
        case InstructionSet::ArmV3: flags |= InstructionInfo::ARMv3; break;
        case InstructionSet::ArmV4: flags |= InstructionInfo::ARMv4; break;
        case InstructionSet::ArmV5: flags |= InstructionInfo::ARMv5; break;
        case InstructionSet::ArmV6: flags |= InstructionInfo::ARMv6; break;
        case InstructionSet::ArmV7:
        default: flags |= InstructionInfo::ModelMask; break;
        }

        return flags;
    }

    uint32_t getLoadAddress() const { return _loadAddress; }
    bool hasLoadAddress() const { return _loadAddress != 0xFFFFFFFF; }
    string_cref_t getInputFile() const { return _inputFile; }
    string_cref_t getOutputFile() const { return _outputFile; }

protected:
    // Overrides
    // Inherited from Cli::ProgramArguments.
    virtual bool processOption(uint32_t id, const String &value, String &error) override
    {
        InstructionSet instructionSet = InstructionSet::Max;
        bool isOK = true;

        switch (id)
        {
        case ShowHelp:
            _command = ADisasmCommand::ShowHelp;
            break;

        case OutputFile:
            _outputFile = value;
            break;

        case TargetInstructionSet:
            if (getInstructionSetType().tryParse(value.toUtf8View(),
                                                 instructionSet))
            {
                if (_instructionSet == InstructionSet::Max)
                {
                    _instructionSet = instructionSet;
                }
                else
                {
                    error = "Multiple instructions sets cannot be specified.";
                    isOK = false;
                }
            }
            else
            {
                error = String::format("'{0}' is an invalid instruction set specification.",
                                       { value });
                isOK = false;
            }
            break;

        case Extension:
            if (value.compareIgnoreCase("FPA") == 0)
            {
                _disasmFlags |= InstructionInfo::AllowFPA;
            }
            else if (value.compareIgnoreCase("THUMB"))
            {
                _disasmFlags |= InstructionInfo::AllowThumb;
            }
            else
            {
                error = String::format("'{0}' is an invalid instruction set extension.",
                                       { value });
                isOK = false;
            }
            break;

        case LoadAddress: {
            ScalarParser parser(LocaleInfo::getNeutral());
            uint32_t address = 0;
            parser.setPreferredRadix(16);
            parser.enableExponent(false);
            parser.enableFraction(false);
            parser.enableRadixPrefix(true);
            parser.enableSign(false);

            if (parser.tryProcessString(value.toUtf8View()) &&
                parser.tryGetValue(address))
            {
                if (_loadAddress == 0xFFFFFFFF)
                {
                    _loadAddress = address;
                }
                else
                {
                    error = "The base address has already been specified.";
                    isOK = false;
                }
            }
            else
            {
                error = String::format("'{0}' is not a valid base address specification.",
                                       { value });
                isOK = false;
            }

        } break;

        default:
            isOK = false;
            break;
        }

        return isOK;
    }

    // Inherited from Cli::ProgramArguments.
    virtual bool processArgument(const String &argument, String &error) override
    {
        bool isOK = true;

        if (_inputFile.isEmpty())
        {
            _inputFile = argument;
        }
        else
        {
            error = "Only one source file can be specified.";
            isOK = false;
        }

        return isOK;
    }

    // Inherited from Cli::ProgramArguments.
    virtual bool validate(String &error) const override
    {
        bool isOK = false;

        if (_command == ADisasmCommand::Disassemble)
        {
            if (_inputFile.isEmpty())
            {
                error = "An input file must be specified.";
            }
            else
            {
                isOK = true;
            }
        }
        else
        {
            isOK = true;
        }

        return isOK;
    }

    // Inherited from Cli::ProgramArguments.
    virtual void postProcess() override
    {
        if (_command == ADisasmCommand::Auto)
        {
            _command = ADisasmCommand::Disassemble;
        }
    }
};

//! @brief The object representing the root application object.
class ADisasmApp : public App
{
private:
    // Internal Fields
    String _inputFile;
    String _outputFile;
    uint32_t _baseAddr;
    uint32_t _disasmFlags;
    bool _isPIC;

    // Internal Functions
    static void flushBytesToOutput(std::vector<uint8_t> &bytes, FILE *output)
    {
        const size_t MaxBytesPerLine = 20;
        size_t bytesInLine = 0;
        bool isFirstInLine = true;

        for (uint8_t next : bytes)
        {
            if (isFirstInLine)
            {
                fprintf(output, "EQUB 0x%.2X", next);
                isFirstInLine = false;
            }
            else
            {
                fprintf(output, ", 0x%.2X", next);
            }

            ++bytesInLine;

            if (bytesInLine >= MaxBytesPerLine)
            {
                // Start a new line after this.
                fputc('\n', output);
                bytesInLine = 0;
                isFirstInLine = true;
            }
        }

        bytes.clear();
    }

    int disassemble(FILE *input, FILE *output) const
    {
        InstructionInfo instruction;
        int processResult = 0;
        uint32_t objAddr = _isPIC ? 0x00000000 : _baseAddr;
        FormatterOptions formatter(objAddr, _disasmFlags);
        std::vector<uint8_t> buffer;

        while (!feof(input))
        {
            if (objAddr & 3)
            {
                uint8_t alignBuffer[4];
                size_t bytesToRead = 4 - (objAddr & 3);
                size_t bytesRead = fread(alignBuffer, 1, bytesToRead, input);

                std::copy_n(alignBuffer, bytesRead, std::back_inserter(buffer));
                objAddr += static_cast<uint32_t>(bytesRead);
            }
            else
            {
                // The address is word aligned, try to disassemble an instruction.
                uint32_t instructionWord;
                size_t bytesRead = fread(&instructionWord, 1, 4, input);

                if ((bytesRead == 4) &&
                    instruction.disassemble(instructionWord, objAddr, _disasmFlags))
                {
                    // Write any previous un-decoded bytes.
                    flushBytesToOutput(buffer, output);

                    // The instruction was successfully disassembled.
                    formatter.setInstructionAddress(objAddr);
                    String statement = instruction.toString(&formatter);

                    fprintf(output, "0x%.6X: %s\n", objAddr, statement.getUtf8Bytes());

                    objAddr += 4;
                }
                else
                {
                    // Accumulate un-decoded bytes.
                    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&instructionWord);

                    std::copy_n(bytes, bytesRead, std::back_inserter(buffer));
                    objAddr += static_cast<uint32_t>(bytesRead);
                }
            }
        }

        // Write last un-decode bytes, if any.
        flushBytesToOutput(buffer, output);

        // Add a trailing line break.
        fputc('\n', output);

        return processResult;
    }
public:
    // Construction/Destruction
    ADisasmApp() :
        _baseAddr(0),
        _disasmFlags(0),
        _isPIC(true)
    {
    }

    virtual ~ADisasmApp() = default;

protected:
    // Overrides
    // Inherited from App.
    virtual CommandLineUPtr createCommandLineArguments() const override
    {
        return std::make_unique<ADisasmArgs>();
    }

    // Inherited from App.
    virtual bool initialise(const Cli::ProgramArguments *args)
    {
        bool isOK = false;

        if (const ADisasmArgs *disasmArgs = dynamic_cast<const ADisasmArgs *>(args))
        {
            if (disasmArgs->getCommand() == ADisasmCommand::Disassemble)
            {
                // Extract the options we need.
                _inputFile = disasmArgs->getInputFile();
                _outputFile = disasmArgs->getOutputFile();
                _disasmFlags = disasmArgs->getDisassemblyOptions();

                if (disasmArgs->hasLoadAddress())
                {
                    _baseAddr = disasmArgs->getLoadAddress();
                    _isPIC = false;
                }
                else
                {
                    _isPIC = true;
                }

                isOK = true;
            }
            else
            {
                // Display command line help.
                puts(disasmArgs->getSchema().getHelpText(100).getUtf8Bytes());

                isOK = true;
            }
        }

        return isOK;
    }

    // Inherited from App.
    virtual int run()
    {
        int processResult = 0;

        if (_inputFile.isEmpty() == false)
        {
            String error;
            FILE *objectCode = nullptr;

            if (tryOpenFile(_inputFile.getUtf8Bytes(), "rb", objectCode, error))
            {
                // Ensure the input file is closed at completion.
                StdFilePtr objCodeManager(objectCode);
                StdFilePtr outputManager;

                FILE *outputStream = nullptr;

                if (_outputFile.isEmpty())
                {
                    // Output to stdout.
                    outputStream = stdout;
                }
                else if (tryOpenFile(_outputFile.getUtf8Bytes(), "w",
                                     outputStream, error))
                {
                    // Ensure the stream is closed at completion.
                    outputManager.reset(outputStream);
                }
                else
                {
                    fprintf(stderr, "Error: Could not open output file '%s' for writing.",
                            _outputFile.getUtf8Bytes());
                    processResult = 1;
                }

                if (processResult == 0)
                {
                    processResult = disassemble(objectCode, outputStream);
                }
            }
            else
            {
                fprintf(stderr, "Error: Could not open input file '%s'.\n",
                        _inputFile.getUtf8Bytes());
                processResult = 1;
            }
        }

        return processResult;
    }
};

} // Anonymous namespace

}} // namespace Ag::Asm

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_MAIN(Ag::Asm::ADisasmApp);

////////////////////////////////////////////////////////////////////////////////


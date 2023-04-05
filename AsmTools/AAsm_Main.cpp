//! @file AAsm_Main.cpp
//! @brief The definition of the entry point for the AAsm CLI assembler tool.
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
enum class AAsmCommand
{
    Auto,
    ShowHelp,
    Assemble,
};

//! @brief Defines command line arguments for the AAsm tools.
class AAsmArgs : public Cli::ProgramArguments
{
private:
    // Internal Types
    enum Option
    {
        ShowHelp,
        OutputFile,
        IncludePath,
        InitialInstructionSet,
        Extension,
        LoadAddress,
    };

    // Internal Fields
    String _inputFile;
    String _outputFile;
    StringCollection _includePaths;
    InstructionSet _instructionSet;
    uint32_t _extensions;
    uint32_t _loadAddress;
    AAsmCommand _command;
    bool _usePIC;

    // Internal Functions
    static Cli::Schema createSchema()
    {
        Cli::SchemaBuilder builder;
        builder.setDescription("A 32-bit ARM assembler.");
        builder.defineValueArgument("source file", Cli::UpToOne);

        builder.defineOption(Option::ShowHelp, "Display command line help.",
                             Cli::OptionValue::None);
        builder.defineAlias(Option::ShowHelp, U'?');
        builder.defineAlias(Option::ShowHelp, "help");

        builder.defineOption(Option::OutputFile, "Specifies the target output file.",
                             Cli::OptionValue::Mandatory, "output file");
        builder.defineAlias(Option::OutputFile, U'o');
        builder.defineAlias(Option::OutputFile, "output");

        builder.defineOption(Option::IncludePath,
                             "Specifies a folder to search for included source files.",
                             Cli::OptionValue::Mandatory, "include dir");
        builder.defineAlias(Option::IncludePath, U'i');

        std::string description;
        description.assign("Specifies the initial instruction set. ");
        Cli::appendValidValues(description, getInstructionSetType());

        builder.defineOption(Option::InitialInstructionSet,
                             description.c_str(),
                             Cli::OptionValue::Mandatory, "instruction set");
        builder.defineAlias(Option::InitialInstructionSet, U's');
        builder.defineAlias(Option::InitialInstructionSet, "instructionset");

        description.clear();
        description.assign("Specifies an extension instruction set to enable. ");
        Cli::appendValidValues(description, getArchExtensionsType());

        builder.defineOption(Option::Extension,
                             description.c_str(),
                             Cli::OptionValue::Mandatory, "extension");
        builder.defineAlias(Option::Extension, U'x');
        builder.defineAlias(Option::Extension, "extension");

        builder.defineOption(Option::LoadAddress, "Specifies the load address of the object code.",
                             Cli::OptionValue::Mandatory, "base address");
        builder.defineAlias(Option::LoadAddress, U'b');
        builder.defineAlias(Option::LoadAddress, "base");

        return builder.createSchema();
    }
public:
    // Construction/Destruction
    AAsmArgs() :
        Cli::ProgramArguments(createSchema()), 
        _instructionSet(InstructionSet::Max),
        _extensions(0),
        _loadAddress(0xFFFFFFFF),
        _command(AAsmCommand::Auto),
        _usePIC(true)
    {
    }

    virtual ~AAsmArgs() = default;

    // Accessors
    AAsmCommand getCommand() const { return _command; }
    InstructionSet getInstructionSet() const { return _instructionSet; }
    uint32_t getExtensions() const { return _extensions; }
    uint32_t getLoadAddress() const { return _loadAddress; }
    bool hasLoadAddress() const { return _loadAddress != 0xFFFFFFFF; }
    string_cref_t getInputFile() const { return _inputFile; }
    string_cref_t getOutputFile() const { return _outputFile; }
    const StringCollection &getIncludePaths() const { return _includePaths; }

protected:
    // Overrides
    virtual bool processOption(uint32_t id, const String &value, String &error) override
    {
        InstructionSet instructionSet = InstructionSet::Max;
        ArchExtensionEnum extension = ArchExtensionEnum::None;
        bool isOK = true;

        switch (id)
        {
        case ShowHelp:
            _command = AAsmCommand::ShowHelp;
            break;

        case OutputFile:
            _outputFile = value;
            break;

        case IncludePath:
            _includePaths.push_back(value);
            break;

        case InitialInstructionSet:
            if (getInstructionSetType().tryParse(value.toUtf8View(),
                                                 instructionSet))
            {
                if (_instructionSet == InstructionSet::Max)
                {
                    _instructionSet = instructionSet;
                }
                else
                {
                    error = "Multiple instructions sets specified.";
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
            if (getArchExtensionsType().tryParse(value.toUtf8View(),
                                                 extension))
            {
                _extensions |= toScalar(extension);
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

    virtual bool validate(String &error) const override
    {
        bool isOK = false;

        if (_command == AAsmCommand::Assemble)
        {
            if (_inputFile.isEmpty())
            {
                error = "An input file must be specified.";
            }
            else if (_outputFile.isEmpty())
            {
                error = "An output file must be specified.";
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

    virtual void postProcess() override
    {
        if (_command == AAsmCommand::Auto)
        {
            _command = AAsmCommand::Assemble;
        }

        if ((_command == AAsmCommand::Assemble) &&
            (_inputFile.isEmpty() == false) &&
            _outputFile.isEmpty())
        {
            // Set a default output file name.
            Fs::PathBuilder outFilePath;

            if (outFilePath.tryParse(_inputFile) &&
                outFilePath.hasElements())
            {
                outFilePath.convertToAbsolute();
                outFilePath.setFileExtension("bin");
                _outputFile = outFilePath.toString(Fs::PathUsage::Kernel);
            }
        }
    }
};

//! @brief The object representing the root application object.
class AAsmApp : public App
{
private:
    // Internal Fields
    Options _options;
    String _inputFile;
    String _outputFile;

    // Internal Functions
    bool configureAssembly(const AAsmArgs &args)
    {
        bool isOK = false;

        if ((args.getInputFile().isEmpty() == false) &&
            (args.getOutputFile().isEmpty() == false))
        {
            isOK = true;
            _inputFile = args.getInputFile();
            _outputFile = args.getOutputFile();

            if (args.getInstructionSet() != InstructionSet::Max)
            {
                _options.setInstructionSet(args.getInstructionSet());
            }

            if (args.hasLoadAddress())
            {
                _options.setLoadAddress(args.getLoadAddress());
            }
            else
            {
                _options.setFlag(AssemblyFlagsEnum::PositionIndependentCode);
            }

            // Configure extensions.
            uint32_t extensions = args.getExtensions();
            int32_t msb;

            while (Bin::bitScanForward(extensions, msb))
            {
                uint32_t mask = 1u << msb;
                ArchExtensionEnum ext = forceFromScalar<ArchExtensionEnum>(mask);

                // Mask out the extension bit.
                extensions &= ~mask;

                if (ext < ArchExtensionEnum::Max)
                {
                    _options.addArchitecureExtension(ext);
                }
            }

            // Resolve search paths into real folder.
            for (string_cref_t searchPath : args.getIncludePaths())
            {
                Fs::PathBuilder path;
                String error;

                if (path.tryParse(searchPath, error))
                {
                    path.convertToAbsolute();
                    _options.addIncludePath(path.toString(Fs::PathUsage::Kernel));
                }
                else
                {
                    printf("Error: The include path '%s' is not valid\n",
                           searchPath.getUtf8Bytes());
                    isOK = false;
                    break;
                }
            }
        }

        return isOK;
    }
public:
    // Construction/Destruction
    AAsmApp() = default;
    virtual ~AAsmApp() = default;


protected:
    // Overrides
    // Inherited from App.
    virtual CommandLineUPtr createCommandLineArguments() const override
    {
        return std::make_unique<AAsmArgs>();
    }

    // Inherited from App.
    virtual bool initialise(const Cli::ProgramArguments *args)
    {
        bool isOK = false;

        if (const AAsmArgs *asmArgs = dynamic_cast<const AAsmArgs *>(args))
        {
            if (asmArgs->getCommand() == AAsmCommand::Assemble)
            {
                isOK = configureAssembly(*asmArgs);
            }
            else
            {
                // Display command line help.
                puts(asmArgs->getSchema().getHelpText(100).getUtf8Bytes());

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
            Messages log;
            ObjectCode code = assembleFile(_inputFile, _options, log);

            for (const Message &message : log.getMessages())
            {
                String text = message.toString();
                puts(text.getUtf8Bytes());
            }

            if (log.hasErrors())
            {
                processResult = 1;
            }
            else
            {
                String error;
                FILE *outputStream = nullptr;

                if (tryOpenFile(_outputFile, "wb", outputStream, error))
                {
                    // Write the object code bytes.
                    fwrite(code.getCode(), 1, code.getCodeSize(), outputStream);
                    fclose(outputStream);
                    outputStream = nullptr;
                }
                else
                {
                    // Failed to open the file.
                    printf("Error: Failed to write output file '%s': %s\n",
                           _outputFile.getUtf8Bytes(), error.getUtf8Bytes());
                    processResult = 1;
                }
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
IMPLEMENT_MAIN(Ag::Asm::AAsmApp);

////////////////////////////////////////////////////////////////////////////////


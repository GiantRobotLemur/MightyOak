//! @file CommandLine.cpp
//! @brief The definition of an object which manages command line arguments.
//! @date 2021-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <cstring>
#include <string>
#include <vector>

#include "CommandLine.hpp"
#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum Format
{
    Format_Symbols,
    Format_MicrosftMap,
    Format_MicrosoftPdb,
    Format_GnuMap,
    Format_GnuNm,
    Format_Unknown,
    Format_Max
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which iterates through the options specified in a single
//! command line argument.
class Option
{
private:
    const char *_source;
    size_t _index;
    size_t _lastIndex;
    bool _isLong;
public:
    Option(const char *source) :
        _source(source),
        _index(0),
        _lastIndex(0),
        _isLong(false)
    {
        bool isOption = false;

        if (_source != nullptr)
        {
            if (*_source == '-')
            {
                isOption = true;
                ++_source;

                if (*_source == '-')
                {
                    ++_source;
                    _isLong = true;
                }
            }
#ifdef _WIN32
            else if (*_source == '/')
            {
                ++_source;
                _isLong = true;
                isOption = true;
            }
#endif
        }

        if (isOption)
        {
            const char *equals = std::strchr(_source, '=');

            if (equals == nullptr)
            {
                _lastIndex = std::strlen(_source);
            }
            else
            {
                _lastIndex = equals - _source;
            }
        }
    }

    // Operations
    bool tryGetNext(std::string &option, std::string &value, bool &valueBound)
    {
        bool hasOption = false;

        if (_index < _lastIndex)
        {
            hasOption = true;
            bool lastOption = false;

            if (_isLong)
            {
                option.assign(_source, _lastIndex);
                _index = _lastIndex;
                lastOption = true;
            }
            else
            {
                option.assign(1, _source[_index++]);
                lastOption = _index == _lastIndex;
            }

            if (lastOption && (_source[_lastIndex] != '\0'))
            {
                // The option was of the form --opt=value.
                value.assign(_source + _lastIndex + 1);
                valueBound = true;
            }
            else
            {
                value.clear();
                valueBound = false;
            }
        }
        else
        {
            option.clear();
            value.clear();
            valueBound = false;
        }

        return hasOption;
    }

};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
bool tryGetOptionValue(int argc, const char *argv[], int &currentArg,
                       bool hasValue, std::string &value)
{
    if (hasValue == false)
    {
        int lastArg = argc - 1;

        if (currentArg < lastArg)
        {
            hasValue = true;
            ++currentArg;

            value.assign(argv[currentArg]);
        }
    }

    return hasValue;
}

bool tryParseFormat(const std::string &value, Format &fmt)
{
    bool isOK = true;

    if (isEqualIgnoreCase(value, "MSMAP"))
    {
        fmt = Format_MicrosftMap;
    }
    else if (isEqualIgnoreCase(value, "GNUMAP"))
    {
        fmt = Format_GnuMap;
    }
    else if (isEqualIgnoreCase(value, "GNUNM"))
    {
        fmt = Format_GnuNm;
    }
    else if (isEqualIgnoreCase(value, "PDB"))
    {
        fmt = Format_MicrosoftPdb;
    }
    else if (isEqualIgnoreCase(value, "SYMBOL"))
    {
        fmt = Format_Symbols;
    }
    else
    {
        fmt = Format_Unknown;
        isOK = false;
    }

    return isOK;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which manages the command line options.
CommandLine::CommandLine() :
    _command(Command_Max)
{
    std::vector<char> buffer;
    int bufferSize = 128;

    do
    {
        bufferSize *= 2;

        buffer.resize(static_cast<int>(bufferSize), '\0');
#ifdef _MSC_VER
    } while (_getcwd(buffer.data(), bufferSize) == nullptr);
#else
    } while (getcwd(buffer.data(), bufferSize) == nullptr);
#endif

    _workingFolder.assign(buffer.data());
}

//! @brief Gets the command the application should perform determined from the
//! last command line parsed.
Command CommandLine::getCommand() const
{
    return _command;
}

//! @brief Gets the primary input file specified the last time a command line
//! was parsed.
const std::string &CommandLine::getInputFile() const
{
    return _inputFile;
}

//! @brief Gets the PE binary which accompanies a PDB file.
const std::string &CommandLine::getExecutableFile() const
{
    return _exeFile;
}

//! @brief Gets the output file path specified the last time the command line
//! was parsed.
const std::string &CommandLine::getOutputFile() const
{
    return _outputFile;
}

//! @brief Gets the current working folder captured when the object was
//! constructed.
const std::string &CommandLine::getWorkingFolder() const
{
    return _workingFolder;
}

//! @brief Writes a summary of the available command line options to the console.
void CommandLine::displayHelp()
{
    puts(
"SymbolPackager [options] <input file>\n"
"Options:\n"
"  -?/h              Displays this usage summary.\n"
"  --help\n"
"  -o <file>         Specifies the name of the symbol file to write.\n"
"  --output <file>   \n"
"  -f <format>       Specifies the format of the input file. Valid values are:\n"
#ifdef _WIN32
"  --format <format> { Symbol, MSMap, GNUMap, PDB, GNUNm }. The format will be\n"
#else
"  --format <format> { Symbol, MSMap, GNUMap, GNUNm }. The format will be\n"
#endif
"                    guessed from the input file extension if omitted.\n"
#ifdef _WIN32
"  --exe <file>      Specifies the PE binary which accompanies a .PDB input file.\n"
#endif
"<input file>        The name of the file to extract symbols from which can be\n"
#ifdef _WIN32
"                    a linker generated map file, previously generated symbol\n"
"                    file, output from the nm tool or a PDB file.\n"
#else
"                    a map file generated by the linker, then nm tool or a \n"
"                    previously generated symbol file.\n"
#endif
    );
}

//! @brief Attempts to parse the command line argument tokens.
//! @param[in] argc The count of arguments.
//! @param[in] argv The array of argument tokens, the first being the name of
//! the program.
//! @param[out] error Receives an error message if parsing fails.
//! @retval true The command line was valid.
//! @retval false The command line was invalid, error describes why.
bool CommandLine::tryParse(int argc, const char *argv[], std::string &error)
{
    Format inputFormat = Format_Unknown;

    error.clear();
    _command = Command_None;

    const BoundedString helpOptions("HELP");
    const BoundedString outputOption("OUTPUT");
    const BoundedString formatOption("FORMAT");
    const BoundedString exeOption("EXE");

    int lastIndex = argc - 1;
    std::string option, value;

    for (int argi = 1; argi <= lastIndex; ++argi)
    {
        Option optArg(argv[argi]);
        bool isOption = false;
        bool hasValue;

        while(optArg.tryGetNext(option, value, hasValue))
        {
            // Process the option argument.
            isOption = true;

            if (option.length() == 1)
            {
                char optionChar = option.at(0);

                switch (optionChar)
                {
                case 'o':
                case 'O':
                    if (tryGetOptionValue(argc, argv, argi, hasValue, value))
                    {
                        _outputFile = makeFullPath(_workingFolder, value);
                    }
                    else
                    {
                        appendFormat(error,
                                     "Argument -%c should be followed by an "
                                     "output file specification.", optionChar);
                    }
                    break;

                case 'F':
                case 'f':
                    if (tryGetOptionValue(argc, argv, argi, hasValue, value))
                    {
                        if (tryParseFormat(value, inputFormat) == false)
                        {
                            appendFormat(error, "The input format '%s' is invalid.",
                                         value.c_str());
                        }
                    }
                    else
                    {
                        appendFormat(error,
                                     "Argument -%c should be followed by an "
                                     "input file format specification.", optionChar);
                    }
                    break;

                case '?':
                case 'H':
                case 'h':
                    _command = Command_DisplayHelp;
                    break;

                default:
                    appendFormat(error, "Unknown command line option '-%c'\n",
                                 optionChar);
                    break;
                }
            }
            else
            {
                BoundedString optionToken(option);

                if (helpOptions.isEqualIgnoreCase(optionToken))
                {
                    _command = Command_DisplayHelp;
                }
                else if (outputOption.isEqualIgnoreCase(optionToken))
                {
                    const char *arg = argv[argi];

                    if (tryGetOptionValue(argc, argv, argi, hasValue, value))
                    {
                        _outputFile = makeFullPath(_workingFolder, value);
                    }
                    else
                    {
                        appendFormat(error,
                                     "Argument %s should be followed by an output "
                                     "file specification.", arg);
                    }
                }
                else if (exeOption.isEqualIgnoreCase(optionToken))
                {
                    const char *arg = argv[argi];

                    if (tryGetOptionValue(argc, argv, argi, hasValue, value))
                    {
                        _exeFile = makeFullPath(_workingFolder, value);
                    }
                    else
                    {
                        appendFormat(error,
                                     "Argument %s should be followed by a .exe/.dll "
                                     "file specification.", arg);
                    }
                }
                else if (formatOption.isEqualIgnoreCase(optionToken))
                {
                    const char *arg = argv[argi];

                    if (tryGetOptionValue(argc, argv, argi, hasValue, value))
                    {
                        if (tryParseFormat(value, inputFormat) == false)
                        {
                            appendFormat(error, "The input format '%s' is invalid.",
                                         value.c_str());
                        }
                    }
                    else
                    {
                        appendFormat(error,
                                     "Argument %s should be followed by an "
                                     "input file format specification.", arg);
                    }
                }
                else
                {
                    appendFormat(error,
                                 "Unknown command line option '--%s'.",
                                 option.c_str());
                }
            }
        }

        if (isOption == false)
        {
            // Process the non-option argument.
            std::string inputPath(argv[argi]);

            _inputFile = makeFullPath(_workingFolder, inputPath);
        }
    }

    if (error.empty())
    {
        if (_command == Command_None)
        {
            // Validate and default the provided options.
            if (_inputFile.empty())
            {
                error.assign("No input file specified.");
            }
            else if (inputFormat == Format_Unknown)
            {
                size_t lastDot = _inputFile.rfind('.');

                if (lastDot != std::string::npos)
                {
                    BoundedString mapExt(".MAP");
                    BoundedString nmExt(".NM");
                    BoundedString pdbExt(".PDB");
                    BoundedString symExt(".SYM");
                    BoundedString ext(_inputFile.c_str() + lastDot);

                    if (ext.isEqualIgnoreCase(mapExt))
                    {
#ifdef _WIN32
                        _command = Command_PackageMsMapFile;
#else
                        _command = Command_PackageGnuMapFile;
#endif
                    }
#ifdef _WIN32
                    else if (ext.isEqualIgnoreCase(pdbExt))
                    {
                        _command = Command_PackagePdbFile;
                    }
#endif
                    else if (ext.isEqualIgnoreCase(nmExt))
                    {
                        _command = Command_PackageGnuNmFile;
                    }
                    else if (ext.isEqualIgnoreCase(symExt))
                    {
                        _command = Command_ExtractSymFile;
                    }
                    else
                    {
                        error.assign("Not sure what format file '");
                        error.append(_inputFile);
                        error.append("' is in.");
                    }

                    if (_outputFile.empty() &&
                        (_command != Command_ExtractSymFile))
                    {
                        _outputFile.assign(_inputFile, 0, lastDot);
                        _outputFile.append(".sym");
                    }
                }
            }
            else
            {
                switch(inputFormat)
                {
                case Format_Symbols:
                    _command = Command_ExtractSymFile;
                    break;

                case Format_MicrosftMap:
                    _command = Command_PackageMsMapFile;
                    break;

                case Format_MicrosoftPdb:
                    _command = Command_PackagePdbFile;
                    break;

                case Format_GnuMap:
                    _command = Command_PackageGnuMapFile;
                    break;

                case Format_GnuNm:
                    _command = Command_PackageGnuNmFile;
                    break;

                default:
                    _command = Command_DisplayHelp;
                    break;
                }
            }
        }

        if ((_command == Command_PackagePdbFile) && _exeFile.empty() && !_inputFile.empty())
        {
            size_t lastDot = _inputFile.rfind('.');
            std::string exeFile(_inputFile, 0, lastDot);

            // Try to see if there is an EXE with the same base name.
            exeFile.append(".exe");

// Prevent MSVCRT warning about access()/_access() naming.
#ifdef _MSC_VER
#define FILE_ACCESS _access
#else
#define FILE_ACCESS access
#endif

            if (FILE_ACCESS(exeFile.c_str(), 0) < 0)
            {
                // Try for a DLL.
                exeFile.erase(exeFile.begin() + lastDot, exeFile.end());
                exeFile.append(".dll");

                if (FILE_ACCESS(exeFile.c_str(), 0) >= 0)
                {
                    _exeFile = std::move(exeFile);
                }
            }
            else
            {
                _exeFile = std::move(exeFile);
            }
#undef FILE_ACCESS
        }

        if ((_command == Command_PackagePdbFile) && _exeFile.empty())
        {
            error.assign("A PDB file must be accompanied by an "
                "executable file (.exe, .dll, etc.) "
                "specified using the --exe option in "
                "order to extract symbols.");
        }
    }

    return error.empty();
}

////////////////////////////////////////////////////////////////////////////////


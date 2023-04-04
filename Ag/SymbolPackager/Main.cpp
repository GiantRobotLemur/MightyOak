//! @file SymbolPackager.cpp
//! @brief The definition of the entry point for a command line tool which
//! extracts and packages up the symbols for a binary module in order to support
//! symbolic stack traces.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdio>

#include "CommandLine.hpp"
#include "MsMapFileReader.hpp"
#include "GnuMapFileReader.hpp"
#include "GnuNmFileReader.hpp"

#ifdef _WIN32
// PDBs only supported under Windows.
#include "PdbFileReader.hpp"
#endif

#include "SymbolDb.hpp"
#include "SymbolFileReader.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief A template function which uses a specific file reader implementation
//! to read program symbols.
//! @tparam TReader The data type of the class to read the symbolic information.
//! @param[in] args The parameters parsed from the command line.
//! @param[out] symbols An object to receive the symbolic information read.
//! @param[out] error Receives a string defining why the operation failed.
//! @retval true The symbols were successfully read.
//! @retval false The symbols could not be read, the text in error describes
//! why.
template<typename TReader>
bool readSymbols(const CommandLine &args, SymbolDb &symbols, std::string &error)
{
    TReader reader(args);
    error.clear();
    symbols.clear();

    reader.readSymbols(symbols, error);
    symbols.compile();

    return error.empty();
}

//! @brief Writes the symbol data as a compressed binary .sym file.
//! @param[in] args The command line arguments defining the name of the file
//! to write.
//! @param[in] symbols The database of symbols to write.
//! @param[out] error A string to receive an error message if the operation fails.
bool writeSymbols(const CommandLine &args, const SymbolDb &symbols,
                  std::string &error)
{
    StdFilePtr output;
    bool isOK = false;
    error.clear();

    if (tryOpenFile(args.getOutputFile().c_str(), "wb", output))
    {
        if (symbols.writeSymbolFile(output.get()))
        {
            isOK = true;
        }
        else
        {
            appendFormat(error, "Failed to write symbol file '%s'.",
                         args.getOutputFile().c_str());
        }
    }
    else
    {
        appendFormat(error, "Failed to create symbol file '%s'.",
                     args.getOutputFile().c_str());
    }

    return isOK;
}

//! @brief Writes the symbol data as text.
//! @param[in] args The command line arguments defining the name of the file
//! to write, or stdout if none is specified.
//! @param[in] symbols The database of symbols to write.
//! @param[out] error A string to receive an error message if the operation fails.
bool writeText(const CommandLine &args, const SymbolDb &symbols,
               std::string &error)
{
    StdFilePtr outputFile;
    FILE *output = nullptr;
    error.clear();

    if (args.getOutputFile().empty())
    {
        // Output to the console.
        output = stdout;
    }
    else
    {
        if (tryOpenFile(args.getOutputFile().c_str(), "w", outputFile))
        {
            output = outputFile.get();
        }
        else
        {
            appendFormat(error, "Unable to create the text file '%s' to receive "
                                "symbol data rendered as text.");
            output = nullptr;
        }
    }

    if (output != nullptr)
    {
        if (symbols.writeText(output) == false)
        {
            appendFormat(error, "Failed to write symbol data to the "
                                "text file '%s'.");
        }
    }

    return error.empty();
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char *argv[])
{
    std::string error;
    CommandLine args;
    int processResult = 0;

    if (args.tryParse(argc, argv, error))
    {
        SymbolDb symbols;

        switch (args.getCommand())
        {
        case Command_PackageMsMapFile:
            readSymbols<MsMapFileReader>(args, symbols, error);

            if (error.empty())
            {
                writeSymbols(args, symbols, error);
            }
            break;

        case Command_PackageGnuMapFile:
            readSymbols<GnuMapFileReader>(args, symbols, error);

            if (error.empty())
            {
                writeSymbols(args, symbols, error);
            }
            break;

        case Command_PackageGnuNmFile:
            readSymbols<GnuNmFileReader>(args, symbols, error);

            if (error.empty())
            {
                writeSymbols(args, symbols, error);
            }
            break;

        case Command_PackagePdbFile:
#ifdef _WIN32
            readSymbols<PdbFileReader>(args, symbols, error);
#else
            error.assign("Reading PDBs is not supported on platforms other than "
                         "Microsoft Windows.");
#endif

            if (error.empty())
            {
                writeSymbols(args, symbols, error);
            }
            break;

        case Command_ExtractSymFile:
            readSymbols<SymbolFileReader>(args, symbols, error);

            if (error.empty())
            {
                writeText(args, symbols, error);
            }
            break;

        default:
            // Display command line help
            CommandLine::displayHelp();
            break;
        }

        if (error.empty() == false)
        {
            printf("Error: %s\n", error.c_str());
        }
    }
    else
    {
        printf("Command line error: %s\n", error.c_str());
        processResult = 1;
    }

    return processResult;
}

////////////////////////////////////////////////////////////////////////////////


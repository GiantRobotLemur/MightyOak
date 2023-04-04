//! @file GnuMapFileReader.cpp
//! @brief The definition of an object which can read map files produced by the
//! GNU ld linker.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <string>

#include "CommandLine.hpp"
#include "GnuMapFileReader.hpp"
#include "LineTokeniser.hpp"
#include "SymbolDb.hpp"
#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum ParseState
{
    Parse_Start,
    Parse_ScriptAndMemMap,
    Parse_SymbolsSection,
    Parse_CodeSymbols,
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void parseFile(FILE *input, SymbolDb &symbols)
{
    LineTokeniser line;
    ParseState state = Parse_Start;

    const LineSignature scriptAndMemMapSignature({ "Linker", "script", "and",
                                                   "memory", "map"});
    const BoundedString textSectionPrefix(".text");
    const BoundedString sectionPrefix(".");
    const LineSignature textSectionSignature({ textSectionPrefix });
    const BoundedString hexPrefix("0x");
    const BoundedString baseAddrSignature("SEGMENT_START (\"text-segment\", ");

    uint64_t baseAddr = UINT64_MAX;
    uint64_t prevAddr = UINT64_MAX;
    size_t pos = 0;

    while (line.tryReadLine(input))
    {
        switch(state)
        {
        case Parse_Start:
            if (line.matches(scriptAndMemMapSignature))
            {
                state = Parse_ScriptAndMemMap;
            }
            break;

        case Parse_ScriptAndMemMap:
            if (line.matches(textSectionSignature))
            {
                state = Parse_SymbolsSection;
            }
            else if ((baseAddr == UINT64_MAX) &&
                     line.startsWith(hexPrefix) &&
                     line.find(baseAddrSignature, 3, pos))
            {
                baseAddr = std::strtoull(line.getLine().c_str() + pos +
                                         baseAddrSignature.Length,
                                         nullptr, 0);
            }
            break;

        case Parse_SymbolsSection:
            if (line.startsWith(textSectionPrefix))
            {
                state = Parse_CodeSymbols;
            }
            break;

        case Parse_CodeSymbols:
            if (line.startsWith(sectionPrefix) &&
                (line.startsWith(textSectionPrefix) == false))
            {
                state = Parse_SymbolsSection;
                prevAddr = UINT64_MAX;
            }
            else if ((line.getTokenCount() > 1) &&
                     line.startsWith(hexPrefix) &&
                     (line.getToken(1).startsWith(hexPrefix) == false))
            {
                // Harvest the symbol.
                uint64_t address = std::strtoull(line.getToken(0).Text, nullptr, 0);

                if ((address != 0) &&
                    (address != ULLONG_MAX) &&
                    (address != prevAddr))
                {
                    prevAddr = address;
                    symbols.addSymbol(address - baseAddr, line.getTail(1));
                }
            }
            break;
        }
    }
}


} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which can read symbol information from a GNU
//! ld-generated map file.
GnuMapFileReader::GnuMapFileReader(const CommandLine &args) :
    _inputFile(args.getInputFile())
{
}

//! @brief Reads the symbols into a symbol database.
//! @param[out] symbols The database to receive the symbols read.
//! @param[out] error A string to receive an error is any occurred.
void GnuMapFileReader::readSymbols(SymbolDb &symbols, std::string &error)
{
    StdFilePtr mapFile;
    error.clear();

    if (tryOpenFile(_inputFile.c_str(), "r", mapFile))
    {
        parseFile(mapFile.get(), symbols);
    }
    else
    {
        appendFormat(error, "Failed to open map file '%s'.",
                     _inputFile.c_str());
    }
}
////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////


//! @file GnuNmFileReader.cpp
//! @brief The definition of an object which reads symbol output from the GNU
//! nm utility.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <vector>

#include "GnuNmFileReader.hpp"
#include "CommandLine.hpp"
#include "LineTokeniser.hpp"
#include "SymbolDb.hpp"
#include "Utils.hpp"

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void parse(FILE *input, SymbolDb &symbols)
{
    LineTokeniser lineTokens;

    uint64_t lastAddress = 0;
    char lastType = '\0';
    std::string lastSymbol;
    const constexpr size_t minAddrDigits = (sizeof(uint32_t) * 2);

    while(lineTokens.tryReadLine(input))
    {
        size_t nextToken = 0;
        uint64_t address = 0;
        char symbolType = '\0';

        if ((lineTokens.getTokenCount() > 1) &&
            (lineTokens.getToken(0).Length >= minAddrDigits))
        {
            address = std::strtoull(lineTokens.getToken(0).Text, nullptr, 16);

            if (address == ULLONG_MAX)
            {
                address = lastAddress;
            }
            else
            {
                if (address != lastAddress)
                {
                    // We've found a new address, store the previous symbol if
                    // it was relevant.
                    if (lastSymbol.empty() == false)
                    {
                        symbols.addSymbol(lastAddress, lastSymbol);
                    }

                    lastAddress = address;
                    lastType = '\0';
                    lastSymbol.clear();
                }
                ++nextToken;
            }
        }
        else
        {
            // The address is as previously stated.
            address = lastAddress;
        }

        if (lineTokens.getTokenCount() >= (1 + nextToken))
        {
            const BoundedString &typeToken = lineTokens.getToken(nextToken);

            if (typeToken.Length == 1)
            {
                symbolType = *typeToken.Text;
            }

            char upperType = static_cast<char>(std::toupper(symbolType));

            if ((upperType == 'T') || (upperType == 'W'))
            {
                // We have a symbol in the .text section, i.e. code, probably
                // a function call.
                if (std::isupper(lastType) == 0)
                {
                    // The new symbol should overwrite the previous one
                    // with the same address.
                    lastType = symbolType;
                    const BoundedString &symbol = lineTokens.getTail(nextToken + 1);
                    lastSymbol.assign(symbol.Text, symbol.Length);
                }
            }
        }
    }

    if (lastSymbol.empty() == false)
    {
        symbols.addSymbol(lastAddress, lastSymbol);
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to read output from the GNU nm utility.
//! @param[in] args The parsed command line arguments.
GnuNmFileReader::GnuNmFileReader(const CommandLine &args) :
    _inputFile(args.getInputFile())
{
}

//! @brief Reads symbols and adds them to a database.
//! @param[out] symbols The database to add symbols to.
//! @param[out] error A string to write an error message to if the operation
//! fails.
void GnuNmFileReader::readSymbols(SymbolDb &symbols, std::string &error)
{
    StdFilePtr nmFile;
    error.clear();

    if (tryOpenFile(_inputFile.c_str(), "r", nmFile))
    {
        parse(nmFile.get(), symbols);
    }
    else
    {
        appendFormat(error, "Failed to open nm output file '%s'.",
                     _inputFile.c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////


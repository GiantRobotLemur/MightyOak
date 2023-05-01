//! @file SymbolFileReader.cpp
//! @brief The definition of an object which reads symbols pre-packaged in
//! a file.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "CommandLine.hpp"
#include "SymbolDb.hpp"
#include "Ag/Private/SymbolEncoding.hpp"
#include "SymbolFileReader.hpp"
#include "Utils.hpp"

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
typedef std::pair<uint64_t, size_t> RawSymbol;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Reads the symbol table from a symbol file.
//! @param[in] input The input stream to read from.
//! @param[in] symbolCount The number of table rows to read.
//! @param[in] fieldUnpacker An object to read the packed symbol table fields.
//! @param[in] initialOffset The offset of the first symbol table entry.
//! @param[out] rawSymbols A collection to receive the raw symbols read.
//! @retval true The table was successfully read.
//! @retval false The end of the input stream was unexpectedly encountered.
bool readSymbolTable(FILE *input, size_t symbolCount,
                     Ag::PackedFieldHelper &fieldUnpacker, uint64_t initialOffset,
                     std::vector<RawSymbol> &rawSymbols)
{
    bool isOK = true;
    uint64_t currentOffset = initialOffset;
    rawSymbols.clear();
    rawSymbols.reserve(symbolCount);

    for (size_t index = 0; isOK && (index < symbolCount); ++index)
    {
        if (fieldUnpacker.read(input))
        {
            // Calculate the symbol offset from the read delta and the
            // previous value.
            currentOffset = fieldUnpacker.getField<uint64_t>(0) + currentOffset;

            rawSymbols.emplace_back(currentOffset,
                                    fieldUnpacker.getField<size_t>(1));
        }
        else
        {
            isOK = false;
        }
    }

    return isOK;
}

//! @brief Reads the string table from a symbol file.
//! @param[in] input The input stream to read from.
//! @param[in] stringCount The number of table rows to read.
//! @param[in] maxLength The maximum length of any symbol.
//! @param[in] stringFields An object to read the packed string table fields.
//! @param[out] stringTable A collection to receive the strings read.
//! @retval true The table was successfully read.
//! @retval false The end of the input stream was unexpectedly encountered.
bool readStringTable(FILE *input, size_t stringCount, size_t maxLength,
                     Ag::PackedFieldHelper &stringFields,
                     std::vector<std::string> &stringTable)
{
    stringTable.clear();
    stringTable.reserve(stringCount);

    std::vector<char> buffer;
    buffer.reserve(maxLength + 1);
    bool isOK = true;

    for (size_t index = 0; isOK && (index < stringCount); ++index)
    {
        isOK = false;

        // Read the packed fields.
        if (stringFields.read(input))
        {
            size_t prefixSize = stringFields.getField<size_t>(0);
            size_t suffixSize = stringFields.getField<size_t>(1);

            buffer.resize(prefixSize + suffixSize, '\0');

            // Read the additional characters required to form the string.
            if (tryRead(input, buffer.data() + prefixSize, suffixSize))
            {
                stringTable.emplace_back(buffer.data(),
                                         prefixSize + suffixSize);
                isOK = true;
            }
        }
    }

    return isOK;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which can read data from a pre-packaged symbol
//! file.
//! @param[in] args The command line options specifying the symbol file to read.
SymbolFileReader::SymbolFileReader(const CommandLine &args) :
    _inputFile(args.getInputFile())
{
}

//! @brief Reads the symbols from a pre-packaged file into a database.
//! @param[out] symbol An object to receive the symbol data.
//! @param[out] error Receives an error message if the operation failed.
void SymbolFileReader::readSymbols(SymbolDb &symbols, std::string &error)
{
    StdFilePtr input;
    error.clear();

    if (tryOpenFile(_inputFile.c_str(), "rb", input))
    {
        Ag::SymbolHeader fileData;

        if (tryRead(input, &fileData, sizeof(fileData)) == false)
        {
            appendFormat(error, "Failed to read header from symbol file '%s'.",
                         _inputFile.c_str());
        }
        else if (std::memcmp(fileData.Header.Signature,
                             SYMBOL_SIGNATURE,
                             sizeof(fileData.Header.Signature)) != 0)
        {
            appendFormat(error, "The signature of symbol file '%s' did "
                         "not have the expected value.",
                         _inputFile.c_str());
        }
        else if ((fileData.Header.Version[0] != 1) ||
                 (fileData.Header.Version[1] != 0) ||
                 (fileData.Header.Version[2] != 0) ||
                 (fileData.Header.Version[3] != 0))
        {
            appendFormat(error, "The symbol file '%s' was encoded using "
                         "a format newer than that which the program supports.",
                         _inputFile.c_str());
        }
        else if (fileData.SymbolCount > 0)
        {
            Ag::PackedFieldHelper symbolFields({ fileData.SymbolOffsetBitCount,
                                                 fileData.SymbolOrdinalBitCount });
            Ag::PackedFieldHelper stringFields({ fileData.StringPrefixBitCount,
                                                 fileData.StringSuffixBitCount });

            std::vector<RawSymbol> symbolTable;
            std::vector<std::string> stringTable;

            if (readSymbolTable(input.get(), fileData.SymbolCount, symbolFields,
                                fileData.InitialOffset, symbolTable) == false)
            {
                appendFormat(error, "Failed to read the symbol table from the "
                             "file '%s'.", _inputFile.c_str());
            }
            else if (readStringTable(input.get(), fileData.SymbolCount, fileData.MaxStringLength,
                                     stringFields, stringTable) == false)
            {
                appendFormat(error, "Failed to read the string table from the "
                             "file '%s'.", _inputFile.c_str());
            }
            else
            {
                // Combine the symbol a string tables.
                for (const RawSymbol &symbol : symbolTable)
                {
                    const std::string &fnName = stringTable.at(symbol.second);

                    symbols.addSymbol(symbol.first, fnName);
                }
            }
        }
    }
    else
    {
        appendFormat(error, "Failed to open symbol file '%s'.",
                     _inputFile.c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////


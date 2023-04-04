//! @file SymbolDb.cpp
//! @brief The definition of an object which can store symbolic information to
//! be packaged as a file.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "SymbolDb.hpp"
#include "Ag/Private/SymbolEncoding.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct CompareOffset
{
    bool operator()(const SymbolEntry &lhs, const SymbolEntry &rhs) const
    {
        return lhs.Offset < rhs.Offset;
    }
};

struct CompareSymbol
{
    bool operator()(const SymbolEntry *lhs, const SymbolEntry *rhs) const
    {
        return lhs->Symbol < rhs->Symbol;
    }
};


////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Calculates the number of significant bits in a binary scalar.
//! @param[in] value The value to analyse.
//! @returns The minimum count of bits required to represent the value.
uint32_t countSignificantBits(uint64_t value)
{
    uint32_t bits = 0;

    for (uint64_t current = value; current != 0; current >>= 1)
    {
        ++bits;
    }

    return bits;
}

//! @brief Calculates the number of initial common characters two strings share.
//! @param[in] lhs The first string to compare.
//! @param[in] rhs The second string to compare.
//! @returns The count of leading characters which are identical between
//! both strings.
size_t calcPrefixLength(const std::string &lhs, const std::string &rhs)
{
    size_t prefixLength;
    size_t commonLength = std::min(lhs.length(), rhs.length());

    for (prefixLength = 0; prefixLength < commonLength; ++prefixLength)
    {
        if (lhs[prefixLength] != rhs[prefixLength])
        {
            break;
        }
    }

    return prefixLength;
}

//! @brief Writes the packed symbol table entries.
//! @param[in] output The output stream to write the table to.
//! @param[in] fieldPacker An object used to pack the fields in each row.
//! @param[in] entries The collection of entries to write ordered by Offset.
//! @return A boolean value indicating whether the table was successfully written.
bool writeSymbols(FILE *output, Ag::PackedFieldHelper &fieldPacker,
                  const std::vector<SymbolEntry> &entries)
{
    bool isOK = true;
    bool isFirst = true;
    uint64_t prevOffset = entries.front().Offset;

    for (const SymbolEntry &entry : entries)
    {
        // Pack the offset delta and string ID fields.
        if (isFirst)
        {
            fieldPacker.setField(0, 0);
            isFirst = false;
        }
        else
        {
            // Write the delta from the last offset.
            fieldPacker.setField(0, entry.Offset - prevOffset);
            prevOffset = entry.Offset;
        }

        fieldPacker.setField(1, entry.SymbolId);

        // Write the fields.
        if (fieldPacker.write(output) == false)
        {
            isOK = false;
            break;
        }
    }

    return isOK;
}

//! @brief Writes a compressed string table to a stream.
//! @param[in] output The output stream to write the table to.
//! @param[in] fieldPacker An object which will pack the fields of the string
//! table.
//! @param[in] entries A collection of symbol entries lexically ordered.
//! @return A boolean value indicating whether the table was successfully written.
bool writeStrings(FILE *output, Ag::PackedFieldHelper &fieldPacker,
                  const std::vector<SymbolEntry *> &entries)
{
    bool isOK = true;
    bool isFirst = true;
    const SymbolEntry *prevEntry = entries.front();

    for (const SymbolEntry *entry : entries)
    {
        size_t prefix, suffix;

        if (isFirst)
        {
            isFirst = false;
            prefix = 0;
            suffix = entry->Symbol.length();
        }
        else
        {
            prefix = calcPrefixLength(prevEntry->Symbol, entry->Symbol);
            suffix = entry->Symbol.length() - prefix;
            prevEntry = entry;
        }

        // Pack the prefix and suffix length fields.
        fieldPacker.setField(0, prefix);
        fieldPacker.setField(1, suffix);

        // Write the fields and the suffix characters.
        if ((fieldPacker.write(output) == false) ||
            (fwrite(entry->Symbol.c_str() + prefix, 1, suffix, output) != suffix))
        {
            isOK = false;
            break;
        }
    }

    return isOK;
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Construct an object representing a symbol within a binary.
//! @param[in] offset The offset of the symbol within the binary.
//! @param[in] symbol The identifier associated with the offset.
SymbolEntry::SymbolEntry(uint64_t offset, const std::string &symbol) :
    Offset(offset),
    SymbolId(0),
    Symbol(symbol)
{
}

//! @brief Construct an object representing a symbol within a binary.
//! @param[in] offset The offset of the symbol within the binary.
//! @param[in] symbol The identifier associated with the offset.
SymbolEntry::SymbolEntry(uint64_t offset, const BoundedString &symbol) :
    Offset(offset),
    SymbolId(0),
    Symbol(symbol.Text, symbol.Length)
{
}

//! @brief Constructs an empty symbol database.
SymbolDb::SymbolDb() :
    _maxSymbolLength(0),
    _offsetBits(0),
    _prefixBits(0),
    _suffixBits(0)
{
}

//! @brief Removes all symbols from the database.
void SymbolDb::clear()
{
    // Reset the compilation state.
    reset();

    // Remove all previously defined symbols.
    _symbolTable.clear();
}

//! @brief Prepares the data for writing.
void SymbolDb::compile()
{
    // Reset the compilation state.
    _symbolIndex.clear();

    _maxSymbolLength = 0;
    _offsetBits = 0;
    _prefixBits = 0;
    _suffixBits = 0;

    if (_symbolTable.empty() == false)
    {
        // Sort the table by the address of each symbol within the binary.
        CompareOffset compareByOffset;

        std::sort(_symbolTable.begin(), _symbolTable.end(), compareByOffset);

        // Ensure any symbols for duplicate offsets are removed.
        //auto pos = std::unique(_symbolTable.begin(), _symbolTable.end(),
        //                       compareByOffset);
        //_symbolTable.erase(pos, _symbolTable.end());

        // Calculate the sizes of fields needed to encode the symbols.
        uint64_t maxDelta = 0;
        uint64_t prevOffset = _symbolTable.front().Offset;
        size_t entryCount = _symbolTable.size();
        size_t maxLength = 0;
        _symbolIndex.reserve(entryCount);

        for (size_t index = 0; index < entryCount; ++index)
        {
            const SymbolEntry &entry = _symbolTable[index];
            uint64_t delta = entry.Offset - prevOffset;

            maxDelta = std::max(maxDelta, delta);
            maxLength = std::max(maxLength, entry.Symbol.length());
                _symbolIndex.push_back(_symbolTable.data() + index);

            prevOffset = entry.Offset;
        }

        _maxSymbolLength = static_cast<uint32_t>(maxLength);
        _offsetBits = countSignificantBits(maxDelta);

        // Calculate the sizes of fields needed to encode the strings.
        std::sort(_symbolIndex.begin(), _symbolIndex.end(), CompareSymbol());

        const SymbolEntry *prevEntry = *_symbolIndex.begin();
        size_t maxPrefixSize = 0;
        size_t maxSuffixSize = prevEntry->Symbol.length();
        size_t stringId = 0;

        // We know the strings are in lexical order in the map.
        for (SymbolEntry *entry : _symbolIndex)
        {
            // Create a mapping from string ordinal to ID.
            entry->SymbolId = stringId++;

            size_t prefix = calcPrefixLength(prevEntry->Symbol, entry->Symbol);
            size_t suffix = entry->Symbol.length() - prefix;

            maxPrefixSize = std::max(maxPrefixSize, prefix);
            maxSuffixSize = std::max(maxSuffixSize, suffix);

            // Move on to the next string.
            prevEntry = entry;
        }

        _prefixBits = countSignificantBits(maxPrefixSize);
        _suffixBits = countSignificantBits(maxSuffixSize);
    }
}

//! @brief Adds a symbol definition to the database.
//! @param[in] symbol The complete symbol to add.
void SymbolDb::addSymbol(const SymbolEntry &symbol)
{
    _symbolTable.push_back(symbol);
    _symbolTable.back().SymbolId = 0;

    // Reset the compilation state.
    reset();
}

//! @brief Adds a symbol definition to the database.
//! @param[in] offset The offset of the symbol within the binary.
//! @param[in] symbol The string associated with the symbol.
void SymbolDb::addSymbol(uint64_t offset, const std::string &symbol)
{
    _symbolTable.emplace_back(offset, symbol);

    // Reset the compilation state.
    reset();
}

//! @brief Adds a symbol definition to the database.
//! @param[in] offset The offset of the symbol within the binary.
//! @param[in] symbol The string associated with the symbol.
void SymbolDb::addSymbol(uint64_t offset, const BoundedString &symbol)
{
    _symbolTable.emplace_back(offset, symbol);

    // Reset the compilation state.
    reset();
}

//! @brief Writes the contents of the database to an output stream.
bool SymbolDb::writeSymbolFile(FILE *outputStream) const
{
    if ((_symbolTable.empty() == false) && _symbolIndex.empty())
    {
        // The table has not been compiled since the last symbol was
        // added to it.
        return false;
    }

    // Initialise the file header.
    Ag::SymbolHeader fileData;

    std::memset(&fileData, 0, sizeof(fileData));
    std::memcpy(fileData.Header.Signature, SYMBOL_SIGNATURE,
                sizeof(fileData.Header.Signature));

    uint32_t ordinalBits = countSignificantBits(_symbolTable.size());

    fileData.Header.Version[0] = 1;
    fileData.SymbolOffsetBitCount = static_cast<uint8_t>(_offsetBits);
    fileData.SymbolOrdinalBitCount = static_cast<uint8_t>(ordinalBits);
    fileData.StringPrefixBitCount = static_cast<uint8_t>(_prefixBits);
    fileData.StringSuffixBitCount = static_cast<uint8_t>(_suffixBits);
    fileData.SymbolCount = static_cast<uint32_t>(_symbolTable.size());
    fileData.MaxStringLength = _maxSymbolLength;

    if (_symbolTable.empty() == false)
    {
        fileData.InitialOffset = _symbolTable.front().Offset;
    }

    // Write the file header.
    bool isOK =
        (fwrite(&fileData, 1, sizeof(fileData), outputStream) == sizeof(fileData));

    if (isOK && (_symbolTable.empty() == false))
    {
        // Write the compressed symbol table.
        Ag::PackedFieldHelper symbolFields({ _offsetBits, ordinalBits });

        isOK = writeSymbols(outputStream, symbolFields, _symbolTable);

        if (isOK)
        {
            // Write the compressed string table.
            Ag::PackedFieldHelper stringFields({ _prefixBits, _suffixBits });

            isOK = writeStrings(outputStream, stringFields, _symbolIndex);
        }
    }

    return isOK;
}

//! @brief Writes out the contents of the database as text.
bool SymbolDb::writeText(FILE *outputStream) const
{
    bool isOK = false;

    if (_symbolTable.empty())
    {
        isOK = true;
    }
    else if (_symbolIndex.empty() == false)
    {
        // Output the table header.
        uint32_t bits = countSignificantBits(_symbolTable.back().Offset);
        uint32_t hexDigits = (bits + 3) / 4;

        const char *offsetTitle = "Offset";
        size_t offsetLength = std::strlen(offsetTitle);
        hexDigits = std::max(static_cast<uint32_t>(offsetLength - 2),
                             hexDigits);

        fputs(offsetTitle, outputStream);

        size_t titleLength = std::max(static_cast<size_t>(hexDigits) + 2,
                                      offsetLength) + 1;
        size_t padding = titleLength - offsetLength;

        for (size_t index = 0; index < padding; ++index)
        {
            fputc(' ', outputStream);
        }

        fputs("Symbol\n", outputStream);

        // Write the records.
        size_t symbolCount = _symbolTable.size();
        for (size_t index = 0; index < symbolCount; ++index)
        {
            const SymbolEntry &entry = _symbolTable[index];

            // NOTE: use of variable precision in the first format value.
            fprintf(outputStream, "0x%.*llX %s\n", hexDigits,
                    static_cast<unsigned long long>(entry.Offset),
                    entry.Symbol.c_str());
        }

        isOK = true;
    }

    return isOK;
}

//! @brief Resets the compilation state of the database.
void SymbolDb::reset()
{
    _symbolIndex.clear();

    _maxSymbolLength = 0;
    _offsetBits = 0;
    _prefixBits = 0;
    _suffixBits = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////


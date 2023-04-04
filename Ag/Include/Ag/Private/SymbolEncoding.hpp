//! @file SymbolEncoding.hpp
//! @brief The declaration of data types shared between the Symbol Packager tool
//! and the Core library which reads the symbol data.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SYMBOL_PACKAGER_SYMBOL_ENCODING_HPP__
#define __AG_SYMBOL_PACKAGER_SYMBOL_ENCODING_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <cstring>

#include <algorithm>
#include <initializer_list>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#define SYMBOL_SIGNATURE "Symbolic"


namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief The header for a file containing binary symbol data.
struct SymbolFileHeader
{
    //! @brief A 64-bit signature.
    char Signature[8];

    //! @brief A 32-bit version number consisting of major, minor, revision and
    //! patch number components.
    uint8_t Version[4];
};

//! @brief The version 1 structure of a binary symbol file.
struct SymbolHeaderV1
{
    //! @brief The header identifying the file type and version.
    SymbolFileHeader Header;

    //! @brief The count of bits in the offset field of symbol table entries.
    uint8_t SymbolOffsetBitCount;

    //! @brief The count of bit in the ordinal field of symbol table entries.
    uint8_t SymbolOrdinalBitCount;

    //! @brief The count of bits in the prefix field of the string table.
    uint8_t StringPrefixBitCount;

    //! @brief The count of bits in the suffix field of the string table.
    uint8_t StringSuffixBitCount;

    //! @brief The absolute offset value of the first entry in the symbol table.
    uint64_t InitialOffset;

    //! @brief The count if records in the symbol table.
    uint32_t SymbolCount;

    //! @brief The maximum length of all symbols in the string table when
    //! uncompressed.
    uint32_t MaxStringLength;

    // The symbol stable immediately follows the header.
    // struct {
    //     Offset : SymbolOffsetBitCount;
    //     Ordinal : SymbolOrdinalBitCount;
    // } SymbolTable[SymbolCount];
    //
    // The string table follows the symbol table.
    // struct {
    //     PrefixSize : StringPrefixBitCount;
    //     SuffixSize : StringSuffixBitCount;
    //     char SuffixCharacters[SuffixSize];
    // } StringTable[SymbolCount];
};

//! @brief An alias of the current version of the file structure.
typedef SymbolHeaderV1 SymbolHeader;

//! @brief An object which packs multiple scalar fields into a run of bytes.
class PackedFieldHelper
{
private:
    // Internal Types
    //! @brief Defines a sub-field to be packed.
    struct SubField
    {
        //! @brief The offset of the first bit within the packed field.
        uint32_t Offset;

        //! @brief The count of bits in this sub-field.
        uint32_t Count;

        //! @brief Constructs a new sub-field definition.
        //! @param[in] offset The global offset of the sub-field within the
        //! packed field.
        //! @param[in] count The count of bits in this sub-field.
        SubField(uint32_t offset, uint32_t count) :
            Offset(offset),
            Count(count)
        {
        }
    };

    // Internal Fields
    std::vector<SubField> _fields;
    std::vector<uint8_t> _buffer;

    // Internal Functions
    //! @brief Reads the contents of a specific field.
    //! @param[in] fieldIndex The 0-based index of the sub-field to read.
    //! @return The sub-field data read from the current contents of the field
    //! buffer.
    uint64_t readBits(size_t fieldIndex) const
    {
        uint64_t value = 0;

        if (fieldIndex < _fields.size())
        {
            const SubField &field = _fields[fieldIndex];
            uint32_t bitsRead = 0;

            while (bitsRead < field.Count)
            {
                uint32_t bitOffset = field.Offset + bitsRead;
                uint32_t prefix = bitOffset & 0x07;
                uint32_t significantBits = std::min(8 - prefix,
                                                    field.Count - bitsRead);

                uint8_t source = _buffer.at(bitOffset / 8) >> prefix;
                uint64_t mask = (1ull << significantBits) - 1;

                // Merge the bits into the value at the correct position.
                value |= (source & mask) << bitsRead;

                // Move on to read from the next byte.
                bitsRead += significantBits;
            }
        }

        return value;
    }

    template<typename TIter>
    void initialise(TIter begin, TIter end)
    {
        uint32_t totalBits = 0;

        _fields.reserve(std::distance(begin, end));

        for (TIter current = begin; current != end; ++current)
        {
            uint32_t bitCount = static_cast<uint32_t>(*current);

            _fields.emplace_back(totalBits, bitCount);
            totalBits += bitCount;
        }

        if (_fields.empty() == false)
        {
            // Pad the last sub-field in order to create a packed field which
            // takes up a whole number of bytes.
            uint32_t roundedBits = (totalBits + 7) & ~0x07;

            //SubField &last = _fields.back();
            //uint32_t endBit = last.Offset + last.Count;
            //uint32_t paddingBits = roundedBits - endBit;

            //last.Count += paddingBits;

            // Create a buffer of an appropriate size.
            _buffer.resize(roundedBits / 8, 0);
        }

    }
public:
    // Construction/Destruction
    //! @brief Constructs an object which can pack and unpack a composite
    //! field with a specified schema.
    //! @param[in] bitCounts A collection defining the number of bits in each
    //! sub-field.
    PackedFieldHelper(const std::initializer_list<uint32_t> &bitCounts)
    {
        initialise(bitCounts.begin(), bitCounts.end());
    }

    // Accessors
    size_t getBufferSize() const { return _buffer.size(); }

    //! @brief Gets read-only access to the buffer holding packed field
    //! values.
    const uint8_t *getFieldBuffer() const { return _buffer.data(); }

    // Operations
    //! @brief Resets all fields to zeros.
    void clear()
    {
        std::memset(_buffer.data(), 0, _buffer.size());
    }

    //! @brief Sets the value of a specified sub-field in the current buffer.
    //! @param[in] index The 0-based index of the field to write.
    //! @param[in] value The value to write to the sub-field.
    void setField(size_t index, uint64_t value)
    {
        if (index < _fields.size())
        {
            const SubField &field = _fields[index];

            uint32_t bitsWritten = 0;
            uint64_t current = value;

            while (bitsWritten < field.Count)
            {
                uint32_t bitOffset = field.Offset + bitsWritten;
                uint32_t prefix = bitOffset & 0x07;
                uint32_t significantBits = std::min(8 - prefix,
                                                    field.Count - bitsWritten);
                size_t byteOffset = bitOffset / 8;

                uint64_t mask = (1ull << significantBits) - 1;
                uint8_t bits = static_cast<uint8_t>(current & mask);

                if (significantBits == 8)
                {
                    // Just overwrite the byte.
                    _buffer[byteOffset] = bits;
                }
                else
                {
                    // Merge the bits into the byte.
                    uint8_t targetBits = _buffer[byteOffset] & ~(mask << prefix);
                    targetBits |= bits << prefix;

                    _buffer[byteOffset] = targetBits;
                }

                // Move on to write the next byte.
                bitsWritten += significantBits;
                current >>= significantBits;
            }
        }
    }

    //! @brief Reads the value of a sub-field from the current buffer contents.
    //! @tparam The data type of the scalar to read.
    //! @param[in] index The 0-based index of the sub-field to read.
    template<typename T> T getField(size_t index) const
    {
        return static_cast<T>(readBits(index));
    }

    //! @brief Writes the contents of the current field buffer to an output stream.
    //! @param[in] outputStream The stream to write the encoded field to.
    //! @retval true The entire field was successfully written.
    //! @retval false The data could not be written to the stream.
    bool write(FILE *outputStream) const
    {
        size_t bytesWritten = fwrite(_buffer.data(), 1,
                                     _buffer.size(), outputStream);

        return bytesWritten == _buffer.size();
    }

    //! @brief Updates the current field buffer from an input stream.
    //! @param[in] inputStream The stream to read bytes from.
    //! @retval true The data was successfully read.
    //! @retval false The data could not be read from the input stream.
    bool read(FILE *inputStream)
    {
        size_t bytesRead = fread(_buffer.data(), 1,
                                 _buffer.size(), inputStream);

        return bytesRead == _buffer.size();
    }
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

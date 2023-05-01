//! @file ObjectCodeBuilder.cpp
//! @brief The definition of an object to which object code can be written.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <iterator>

#include "AsmTools/Messages.hpp"

#include "ExprContexts.hpp"
#include "ObjectCodeBuilder.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// ObjectCodeBuilder Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to receive assembled object code or data.
//! @param[in] messages A collection to receive messages from instructions
//! being assembled.
//! @param[in] baseAddr The base address of the first instruction assembled,
//! if known, otherwise 0x0000.
//! @param[in] initialOffset The offset of the first byte of object code
//! from baseAddr represented by the current object.
ObjectCodeBuilder::ObjectCodeBuilder(Messages &messages, uint32_t baseAddr,
                                     uint32_t initialOffset) :
    _output(messages),
    _encoder(Ag::Bin::ByteOrder::getLittleEndian()),
    _baseAddress(baseAddr),
    _initialOffset(initialOffset),
    _finalPass(false)
{
}

//! @brief Determines whether the object contains any object code.
//! @retval true The object contains no object code.
//! @retval false The object contains at least one byte of object code.
bool ObjectCodeBuilder::isEmpty() const { return _code.empty(); }

//! @brief Determines if the assembler is on its final pass in which object
//! code must be generated or an error produced.
//! @retval true The process is in its final pass, all symbols should be defined.
//! @retval false The process is in its initial pass, symbols which appear later
//! in the source code will not be defined yet.
bool ObjectCodeBuilder::isFinalPass() const
{
    return _finalPass;
}

//! @brief Gets the collection of assembly messages in order to add to it.
Messages &ObjectCodeBuilder::getOutput() const { return _output; }

//! @brief Gets the object code assembled since construction or the last call
//! to clear().
const uint8_t *ObjectCodeBuilder::getCode() const { return _code.data(); }

//! @brief Gets the count of object code bytes written since construction or
//! the last call to clear.
size_t ObjectCodeBuilder::getSize() const { return _code.size(); }

//! @brief Gets the bytes offset of the start of this block within the overall
//! object code.
uint32_t ObjectCodeBuilder::getInitialOffset() const
{
    return _initialOffset;
}

//! @brief Gets the object code address at which the next instruction will be
//! assembled.
//! @return A byte offset.
uint32_t ObjectCodeBuilder::getCurrentOffset() const
{
    return static_cast<uint32_t>(_code.size() + _initialOffset);
}

//! @brief Gets the theoretical address at which the next instruction assembled
//! will run at.
//! @return A byte offset from the base of runtime memory.
uint32_t ObjectCodeBuilder::getCurrentAddress() const
{
    return static_cast<uint32_t>(_code.size() + _baseAddress + _initialOffset);
}


//! @brief Removes all previously written object code.
void ObjectCodeBuilder::clear()
{
    _code.clear();
}

//! @brief Provides a hint as to the expected size of the object code written
//! to the current object.
//! @param[in] hintSize A prediction of the required size in bytes.
void ObjectCodeBuilder::reserve(size_t hintSize)
{
    if (_code.capacity() < hintSize)
    {
        _code.reserve(hintSize);
    }
}

//! @brief Writes a byte to the object code being assembled.
//! @param[in] value The byte to write.
void ObjectCodeBuilder::writeByte(uint8_t value)
{
    _code.push_back(value);
}

//! @brief Writes a run of bytes to the object code being assembled without
//! any byte order translation.
//! @param[in] values A pointer to the array of bytes to write.
//! @param[in] count The count of bytes in values.
void ObjectCodeBuilder::writeBytes(const void *values, size_t count)
{
    size_t currentSize = _code.capacity();
    size_t requiredSize = _code.size() + count;

    if (requiredSize > currentSize)
    {
        _code.reserve((requiredSize * 110) / 100);
    }

    std::copy_n(reinterpret_cast<const uint8_t *>(values), count,
                std::back_inserter(_code));
}

//! @brief Converts a 16-bit word to the target byte order and writes it to
//! the object code being assembled.
//! @param[in] value The value to write.
void ObjectCodeBuilder::writeHalfWord(uint16_t value)
{
    uint16_t encoded = _encoder->toTarget(value);

    writeBytes(&encoded, sizeof(encoded));
}

//! @brief Converts a 32-bit word to the target byte order and writes it to
//! the object code being assembled.
//! @param[in] value The value to write.
void ObjectCodeBuilder::writeWord(uint32_t value)
{
    uint32_t encoded = _encoder->toTarget(value);

    writeBytes(&encoded, sizeof(encoded));
}

//! @brief Converts a 64-bit word to the target byte order and writes it to
//! the object code being assembled.
//! @param[in] value The value to write.
void ObjectCodeBuilder::writeLongWord(uint64_t value)
{
    uint64_t encoded = _encoder->toTarget(value);

    writeBytes(&encoded, sizeof(encoded));
}

//! @brief Stuffs the object code buffer with zero bytes.
//! @param[in] byteCount The count of bytes to write.
void ObjectCodeBuilder::writeZeros(size_t byteCount)
{
    if (byteCount > 0)
    {
        std::fill_n(std::back_inserter(_code), byteCount,
                    static_cast<uint8_t>(0));
    }
}

//! @brief Changes the state of the object to indicate that assembly must take
//! place or an error issued, rather than silently failing due to undefined
//! symbols.
void ObjectCodeBuilder::beginFinalPass()
{
    _finalPass = true;
}

//! @brief Constructs an immutable object code object and resets the current
//! object back to an empty state.
ObjectCode ObjectCodeBuilder::createObjectCode()
{
    _finalPass = false;
    SymbolMap emptySymbols;

    return ObjectCode(std::move(_code), std::move(emptySymbols), _baseAddress);
}

//! @brief Constructs an immutable object code object annotated with symbols
//! and resets the current object back to an empty state.
//! @param[in] topLevelSymbols The table of symbols to annotate the object
//! code with.
ObjectCode ObjectCodeBuilder::createObjectCode(const SymbolTable &topLevelSymbols)
{
    _finalPass = false;
    SymbolMap symbols;

    for (const auto &symbol : topLevelSymbols.getAllSymbols())
    {
        if (symbol.isAddress() && (symbol.getValue().getDataType() == DataType::Uint32))
        {
            symbols.emplace(symbol.getId(), symbol.getValue().asUint32());
        }
    }

    return ObjectCode(std::move(_code), std::move(symbols), _baseAddress);
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


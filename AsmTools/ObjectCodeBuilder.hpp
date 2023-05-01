//! @file ObjectCodeBuilder.hpp
//! @brief The declaration of an object to which object code can be written.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_OBJECT_CODE_BUILDER_HPP__
#define __ASM_TOOLS_OBJECT_CODE_BUILDER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "Ag/Core/ByteOrder.hpp"
#include "AsmTools/ObjectCode.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class Messages;
class SymbolTable;

//! @brief An object to which object code can be written.
class ObjectCodeBuilder
{
public:
    // Construction/Destruction
    ObjectCodeBuilder() = delete;
    ObjectCodeBuilder(const ObjectCodeBuilder &) = delete;
    ObjectCodeBuilder(ObjectCodeBuilder &&) = delete;
    ObjectCodeBuilder &operator=(const ObjectCodeBuilder &) = delete;
    ObjectCodeBuilder &operator=(ObjectCodeBuilder &&) = delete;

    ObjectCodeBuilder(Messages &messages, uint32_t baseAddr, uint32_t initialOffset);
    ~ObjectCodeBuilder() = default;

    // Accessors
    bool isEmpty() const;
    bool isFinalPass() const;
    Messages &getOutput() const;
    const uint8_t *getCode() const;
    size_t getSize() const;
    uint32_t getInitialOffset() const;
    uint32_t getCurrentOffset() const;
    uint32_t getCurrentAddress() const;

    // Operations
    void clear();
    void reserve(size_t hintSize);

    void writeByte(uint8_t value);
    void writeBytes(const void *values, size_t count);
    void writeHalfWord(uint16_t value);
    void writeWord(uint32_t value);
    void writeLongWord(uint64_t value);
    void writeZeros(size_t byteCount);
    void beginFinalPass();

    ObjectCode createObjectCode();
    ObjectCode createObjectCode(const SymbolTable &symbols);
private:
    // Internal Fields
    Messages &_output;
    const Ag::Bin::ByteOrder *_encoder;
    std::vector<uint8_t> _code;
    uint32_t _baseAddress;
    uint32_t _initialOffset;
    bool _finalPass;
};

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

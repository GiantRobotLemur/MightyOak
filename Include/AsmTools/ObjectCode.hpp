//! @file ObjectCode.hpp
//! @brief The declaration of an object holding assembled object machine code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_OBJECT_CODE_HPP__
#define __ASM_TOOLS_OBJECT_CODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <unordered_map>
#include <vector>

#include "Ag/Core/String.hpp"

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
using SymbolMap = std::unordered_map<String, uint32_t>;

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object holding assembled object machine code.
class ObjectCode
{
public:
    // Construction/Destruction
    ObjectCode();
    ObjectCode(std::vector<uint8_t> &&machineCode, SymbolMap &&symbolMap,
               uint32_t loadAddress);
    ~ObjectCode();

    // Accessors
    bool isEmpty() const;
    const void *getCode() const;
    size_t getCodeSize() const;
    uint32_t getLoadAddress() const;
    const SymbolMap &getSymbols() const;
private:
    // Internal Functions
    void clear();

    // Internal Fields
    std::vector<uint8_t> _code;
    SymbolMap _symbols;
    uint32_t _loadAddress;
};

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

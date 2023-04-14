//! @file ObjectCode.cpp
//! @brief The definition of an object holding assembled object machine code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <cstring>
#include "AsmTools/ObjectCode.hpp"

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty block of object code.
ObjectCode::ObjectCode() :
    _loadAddress(0)
{
}

//! @brief Constructs an object to hold assembled machine code.
//! @param[in] machineCode A vector of object code bytes to take ownership of.
//! @param[in] symbolMap A map of symbols defined at global scope in the
//! assembled code.
//! @param[in] loadAddress The 32-bit address at which the code is expected to run.
ObjectCode::ObjectCode(std::vector<uint8_t> &&machineCode,
                       SymbolMap &&symbolMap, uint32_t loadAddress) :
    _code(machineCode),
    _symbols(symbolMap),
    _loadAddress(loadAddress)
{
}

//! @brief Disposes of the object machine code.
ObjectCode::~ObjectCode()
{
    clear();
}

//! @brief Determines whether the object contains no assembled machine code.
//! @retval true The object contains no object code.
//! @retval false The object contains at least one byte of object code.
bool ObjectCode::isEmpty() const
{
    return _code.empty();
}

//! @brief Gets a pointer to the assembled code.
const void *ObjectCode::getCode() const
{
    return _code.data();
}

//! @brief Gets the count of bytes of machine code the object holds.
size_t ObjectCode::getCodeSize() const
{
    return _code.size();
}

//! @brief Gets the 32-bit address at which the code is expected to run.
uint32_t ObjectCode::getLoadAddress() const
{
    return _loadAddress;
}

//! @brief Gets symbols representing addresses in the object code.
const SymbolMap &ObjectCode::getSymbols() const
{
    return _symbols;
}

//! @brief Frees any object code and resets the object to an empty state.
void ObjectCode::clear()
{
    _code.clear();
    _symbols.clear();
}

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


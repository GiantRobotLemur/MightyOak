//! @file AsmTools/SymbolTable.cpp
//! @brief The definition of an object representing an indexed set of symbols.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/CodePoint.hpp"

#include "SymbolTable.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// SymbolDefinition Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a symbol definition to be used as a lookup key.
//! @param[in] id The value used to identify the symbol.
SymbolDefinition::SymbolDefinition(Ag::string_cref_t id) :
    _id(id),
    _isAddress(false)
{
}

//! @brief Constructs an object declaring the existence of a symbol but not
//! its value.
//! @param[in] id The value used to identify the symbol.
//! @param[in] source The location at which the symbol was defined.
SymbolDefinition::SymbolDefinition(Ag::string_cref_t id, const Location &source) :
    _id(id),
    _source(source),
    _isAddress(false)
{
}

//! @brief Constructs an object defining a symbol with a value.
//! @param[in] id The value used to identify the symbol.
//! @param[in] source The location at which the symbol was defined.
//! @param[in] value The initial value of the symbol.
//! @param[in] isAddress True if the symbol value represents a position in
//! code, false if it represent an arbitrary value.
SymbolDefinition::SymbolDefinition(Ag::string_cref_t id, const Location &source,
                                   const Value &value, bool isAddress) :
    _id(id),
    _source(source),
    _definition(value),
    _isAddress(isAddress)
{
}

//! @brief Gets the value used to identify the symbol in source code.
Ag::string_cref_t SymbolDefinition::getId() const { return _id; }

//! @brief Gets the location at which the symbol was defined.
const Location &SymbolDefinition::getSource() const { return _source; }

//! @brief Determines if the symbol has been assigned a value.
//! @retval true The symbol has a non-null value;
//! @retval false No value has been assigned to the symbol.
bool SymbolDefinition::hasValue() const { return _definition.isNull(); }

//! @brief Gets the value assigned to the symbol, possibly null.
const Value &SymbolDefinition::getValue() const { return _definition; }

//! @brief Assigns a value to the symbol.
//! @param[in] value The new symbol value, possibly null.
void SymbolDefinition::setValue(const Value &value) { _definition = value; }

//! @brief Gets whether the label marks a position in code.
//! @retval true The label marks a position in code.
//! @retval false The label was given an explicit value, possibly derived
//! from other labels defining a position in code.
bool SymbolDefinition::isAddress() const
{
    return _isAddress;
}

//! @brief Annotates the label as one created from an address in code.
//! @param isAddress True to make the label as an address, false to
//! mark it as arbitrary data.
void SymbolDefinition::setIsAddress(bool isAddress)
{
    _isAddress = isAddress;
}

//! @brief Determines if the identifier for the current symbol is the same as
//! the identifier for another.
//! @param[in] rhs The symbol to compare against.
//! @retval true The symbols have the same identifier.
//! @retval false The symbols have different identifiers.
bool SymbolDefinition::operator==(const SymbolDefinition &rhs) const
{
    return _id == rhs._id;
}

//! @brief Determines if the identifier for the current symbol is the different
//! from the identifier for another.
//! @param[in] rhs The symbol to compare against.
//! @retval true The symbols have the same identifier.
//! @retval false The symbols have different identifiers.
bool SymbolDefinition::operator!=(const SymbolDefinition &rhs) const
{
    return _id != rhs._id;
}

//! @brief Calculates the hash code of a symbol identifier.
//! @param[in] sym The symbol to analyse.
//! @return The hash code of the symbol.
size_t SymbolDefinitionIDHash::operator()(const Mo::Asm::SymbolDefinition &sym) const
{
    return sym.getId().getHashCode();
}

//! @brief Determines whether two symbols have the same identifier.
//! @param[in] lhs The first symbol to analyse.
//! @param[in] rhs The second symbol to analyse.
//! @retval true The two symbols have the same identifier.
//! @retval false The two symbols have different identifiers.
bool SymbolDefinitionIDEqual::operator()(const Mo::Asm::SymbolDefinition &lhs,
                                         const Mo::Asm::SymbolDefinition &rhs) const
{
    return lhs.getId() == rhs.getId();
}

////////////////////////////////////////////////////////////////////////////////
// SymbolTable Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the set of all symbols defined in the table.
const SymbolTable::Symbols &SymbolTable::getAllSymbols() const
{
    return _symbols;
}

//! @brief Determines if the symbol table contains a definition for a named symbol.
//! @param[in] id The case-sensitive identifier of the symbol to query.
//! @param[out] at Received the source code location of the definition of the
//! symbol, if one was found.
//! @retval true The table contains a definition for the named symbol.
//! @retval false The table does not contain a matching definition.
bool SymbolTable::contains(Ag::string_cref_t id, Location &at) const
{
    SymbolDefinition key(id);
    auto pos = _symbols.find(key);
    bool hasMatch = false;

    if (pos != _symbols.end())
    {
        hasMatch = true;
        at = pos->getSource();
    }

    return hasMatch;
}

//! @brief Attempts to find the definition of a named symbol.
//! @param[in] id The case-sensitive identifier of the symbol to query.
//! @param[out] value Receives the value of the symbol if one was found.
//! @retval true The table contains a definition for the named symbol and its
//! value was returned, although his maybe null.
//! @retval false The table does not contain a matching definition.
bool SymbolTable::tryLookupValue(Ag::string_cref_t id, Value &value) const
{
    SymbolDefinition key(id);
    auto pos = _symbols.find(id);
    bool hasMatch = false;

    if (pos == _symbols.end())
    {
        value.setNull();
    }
    else
    {
        value = pos->getValue();
        hasMatch = true;
    }

    return hasMatch;
}

//! @brief Declares a named symbol with no value.
//! @param[in] id The identifier of the symbol to declare.
//! @param[in] source The source code location of the symbol.
//! @retval true The symbol unique and thus added to the table.
//! @retval false The symbol was a duplicate and not added.
bool SymbolTable::declareSymbol(Ag::string_cref_t id, const Location &source)
{
    auto insertPair = _symbols.emplace(id, source);

    return insertPair.second;
}

//! @brief Defines a named symbol with an initial value.
//! @param[in] id The identifier of the symbol to define.
//! @param[in] source The source code location of the symbol.
//! @param[in] value The value to associate with the symbol.
//! @retval true The symbol unique and thus added to the table.
//! @retval false The symbol was a duplicate and not added.
//! @param[in] isAddress True if the symbol value represents a position in
//! code, false if it represent an arbitrary value.
bool SymbolTable::defineSymbol(Ag::string_cref_t id, const Location &source,
                               const Value &value, bool isAddress)
{
    auto insertPair = _symbols.emplace(id, source, value, isAddress);

    return insertPair.second;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


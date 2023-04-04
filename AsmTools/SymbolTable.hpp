//! @file SymbolTable.hpp
//! @brief The declaration of an object representing an indexed set of symbols.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_SYMBOL_TABLE_HPP__
#define __ASM_TOOLS_SYMBOL_TABLE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <unordered_set>

#include "Ag/Core/String.hpp"
#include "AsmTools/Messages.hpp"
#include "Value.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class SymbolDefinition
{
public:
    // Construction/Destruction
    SymbolDefinition(string_cref_t id);
    SymbolDefinition(string_cref_t id, const Location &source);
    SymbolDefinition(string_cref_t id, const Location &source, const Value &value);

    // Accessors
    string_cref_t getId() const;
    const Location &getSource() const;
    bool hasValue() const;
    const Value &getValue() const;
    void setValue(const Value &value);

    // Operations
    bool operator==(const SymbolDefinition &rhs) const;
    bool operator!=(const SymbolDefinition &rhs) const;
private:
    String _id;
    Location _source;
    Value _definition;
};

//! @brief A functor which produces a hash of a symbol definition identifier.
struct SymbolDefinitionIDHash
{
    size_t operator()(const Ag::Asm::SymbolDefinition &sym) const;
};

//! @brief A functor which compares the identifies of symbol definitions.
struct SymbolDefinitionIDEqual
{
    bool operator()(const Ag::Asm::SymbolDefinition &lhs,
                    const Ag::Asm::SymbolDefinition &rhs) const;
};

//! @brief An object representing an indexed set of symbols.
class SymbolTable
{
public:
    // Construction/Destruction
    SymbolTable() = default;
    ~SymbolTable() = default;

    // Accessors
    bool contains(string_cref_t id, Location &at) const;
    bool tryLookupValue(string_cref_t id, Value &value) const;

    // Operations
    bool declareSymbol(string_cref_t id, const Location &source);
    bool defineSymbol(string_cref_t id, const Location &source, const Value &value);
private:
    // Internal Types
    using SymbolHashTable = std::unordered_set<SymbolDefinition,
                                               SymbolDefinitionIDHash,
                                               SymbolDefinitionIDEqual>;

    // Internal Functions

    // Internal Fields
    SymbolHashTable _symbols;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

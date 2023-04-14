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

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Captures the definition of a symbol defined in source code.
class SymbolDefinition
{
public:
    // Construction/Destruction
    SymbolDefinition(string_cref_t id);
    SymbolDefinition(string_cref_t id, const Location &source);
    SymbolDefinition(string_cref_t id, const Location &source,
                     const Value &value, bool isAddress);

    // Accessors
    string_cref_t getId() const;
    const Location &getSource() const;
    bool hasValue() const;
    const Value &getValue() const;
    void setValue(const Value &value);
    bool isAddress() const;
    void setIsAddress(bool isAddress);

    // Operations
    bool operator==(const SymbolDefinition &rhs) const;
    bool operator!=(const SymbolDefinition &rhs) const;
private:
    String _id;
    Location _source;
    Value _definition;
    bool _isAddress;
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
    // Public Types
    using Symbols = std::unordered_set<SymbolDefinition,
        SymbolDefinitionIDHash,
        SymbolDefinitionIDEqual>;

    // Construction/Destruction
    SymbolTable() = default;
    ~SymbolTable() = default;

    // Accessors
    const Symbols &getAllSymbols() const;
    bool contains(string_cref_t id, Location &at) const;
    bool tryLookupValue(string_cref_t id, Value &value) const;

    // Operations
    bool declareSymbol(string_cref_t id, const Location &source);
    bool defineSymbol(string_cref_t id, const Location &source,
                      const Value &value, bool isAddress);
private:
    // Internal Fields
    Symbols _symbols;
};

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

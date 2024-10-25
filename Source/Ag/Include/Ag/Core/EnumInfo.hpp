//! @file Core/EnumInfo.hpp
//! @brief The declaration of a template class which provides metadata for an
//! enumeration type.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_ENUM_INFO_HPP__
#define __AG_CORE_ENUM_INFO_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <initializer_list>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Exception.hpp"
#include "Utf.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////
#define STATIC_SCALAR_DEF(t, x) Ag::EnumSymbol<t>(x, # x)
#define STATIC_SCALAR_DEF2(t, x, y) Ag::EnumSymbol<t>(x, # x, y)
#define STATIC_SCALAR_DEF3(t, x, y, z) Ag::EnumSymbol<t>(x, # x, y, z)
#define STATIC_ENUM_DEF(x) Ag::EnumSymbol<decltype(x)>(x, # x)

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class HashedStringView : public std::string_view
{
private:
    size_t _hash;
public:
    HashedStringView() :
        std::string_view(),
        _hash(std::hash<std::string_view>()(*this))
    {
    }

    HashedStringView(const char *text) :
        std::string_view(Utf::ensureNotNull(text)),
        _hash(std::hash<std::string_view>()(*this))
    {
    }

    HashedStringView(const std::string_view &view) :
        std::string_view(view),
        _hash(std::hash<std::string_view>()(view))
    {
    }

    size_t hash() const { return _hash; }
};

struct CachedHash
{
    size_t operator()(const HashedStringView &rhs) const
    {
        return rhs.hash();
    }
};

using HashedStringMap = std::unordered_map<HashedStringView, size_t, CachedHash>;

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Represents a symbol in an enumeration type described using
//! static strings.
//! @tparam TEnum The data type of the enum class being documented.
template<typename TEnum>
struct EnumSymbol
{
private:
    // Public Fields
    HashedStringView _symbol;
    std::string_view _displayName;
    std::string_view _description;
    TEnum _id;

public:
    // Construction
    //! @brief Constructs a symbol to be used solely as a search key.
    //! @param[in] id The binary value of the symbol.
    EnumSymbol(TEnum id) :
        _id(id)
    {
    }

    //! @brief Constructs an object representing a symbol in an enumeration
    //! class.
    //! @param[in] id The binary value of the symbol.
    //! @param[in] symbol The internal symbol definition as text.
    //! @param[in] displayName The symbol as text to be displayed to the user.
    //! @param[in] description A description of the meaning of the symbol
    //! which can be displayed to the user.
    //! @note All strings should be static and UTF-8 encoded.
    EnumSymbol(TEnum id, const char *symbol, const char *displayName = nullptr,
               const char *description = nullptr) :
        _symbol(Utf::ensureNotNull(symbol)),
        _displayName(Utf::ensureNotNull(displayName)),
        _description(Utf::ensureNotNull(description)),
        _id(id)
    {
        if (_displayName.empty())
        {
            _displayName = _symbol;
        }
    }

    // Accessors
    //! @brief Gets the binary value of the documented symbol.
    TEnum getId() const { return _id; }

    //! @brief Gets the internal symbol definition as text.
    const HashedStringView &getSymbol() const { return _symbol; }

    //! @brief Gets the symbol as text to be displayed to the user.
    const std::string_view &getDisplayName() const { return _displayName; }

    //! @brief Gets a description of the meaning of the symbol
    //! which can be displayed to the user.
    const std::string_view &getDescription() const { return _description; }

    //! @brief Compares objects by their ID field.
    //! @param[in] rhs The symbol to compare against.
    //! @retval true If the current Key is less than rhs.Key.
    bool operator<(const EnumSymbol &rhs) const
    {
        return _id < rhs._id;
    }

    //! @brief Tests objects for equality by their ID field.
    bool operator==(const EnumSymbol &rhs) const
    {
        return _id == rhs._id;
    }
};

//! @brief A template class which provides metadata for an enumeration type.
//! @tparam TEnum The enumeration type being described.
//! @tparam TEnumSym The structure which describes each enumeration symbol.
template<typename TEnum, typename TEnumSym = EnumSymbol<TEnum> >
class EnumInfo
{
public:
    // Public Types
    using SymbolInfo = TEnumSym;
    using SymbolCollection = std::vector<TEnumSym>;

private:
    // Internal Types
    using IndexCollection = std::vector<size_t>;

    //! @brief Compares indexes into a SymbolCollection by the Symbol field.
    struct CompareSymbolIndex
    {
    private:
        const SymbolCollection &_symbols;
    public:
        CompareSymbolIndex(const SymbolCollection &symbols) :
            _symbols(symbols)
        {
        }

        bool operator()(size_t lhs, size_t rhs) const
        {
            const TEnumSym &lhsValue = _symbols[lhs];
            const TEnumSym &rhsValue = _symbols[rhs];

            return lhsValue.getSymbol().compare(rhsValue.getSymbol()) < 0;
        }
    };

    // Internal Fields
    SymbolCollection _symbols;
    std::vector<std::string> _upperCaseKeys;
    HashedStringMap _indexesBySymbol;
    HashedStringMap _indexesByUpperCaseSymbol;
public:
    // Construction/Destruction
    //! @brief Constructs an object describing an enumeration type.
    //! @param[in] symbols The set of symbol descriptions which describe the
    //! enumeration type.
    EnumInfo(const std::initializer_list<TEnumSym> &symbols) :
        _symbols(symbols)
    {
        if (_symbols.empty())
        {
            throw ArgumentException("symbols");
        }

        std::sort(_symbols.begin(), _symbols.end());

        // Ensure there are no duplicate definitions.
        auto pos = std::unique(_symbols.begin(), _symbols.end());

        if (pos != _symbols.end())
        {
            throw Ag::OperationException("Duplicate enumeration symbol values defined.");
        }

        // Ensure the collection of upper-case keys is not re-allocated.
        _upperCaseKeys.reserve(_symbols.size());

        // Create an index of symbol names.
        for (size_t i = 0, c = _symbols.size(); i < c; ++i)
        {
            const TEnumSym &item = _symbols.at(i);
            HashedStringView key(item.getSymbol());

            auto insertResult = _indexesBySymbol.emplace(item.getSymbol(), i);

            if (insertResult.second == false)
            {
                // The symbol wasn't inserted because it was already defined,
                // that's bad.
                throw Ag::OperationException("Duplicate enumeration symbol strings defined.");
            }

            // Create an upper-case version of the symbol.
            std::string &upperCaseKey = _upperCaseKeys.emplace_back();
            upperCaseKey.reserve(item.getSymbol().length());

            for (char ch : item.getSymbol())
            {
                upperCaseKey.push_back(static_cast<char>(toupper(ch)));
            }

            HashedStringView upperCase(upperCaseKey);
            _indexesByUpperCaseSymbol.emplace(upperCase, i);
        }
    }

    // Accessors
    //! @brief Gets the collection of all symbols defined order by the base
    //! enumeration type.
    const SymbolCollection &getSymbols() const { return _symbols; }

    //! @brief Attempts to find the index of an entry describing a specific
    //! symbol.
    //! @param[in] id The symbol value to look up.
    //! @param[out] index Receives the index of the entry describing the symbol.
    //! @retval True The symbol was found, it's index in the symbols collection
    //! was returned in index.
    //! @retval False The symbol was not defined. Index is initialised to a
    //! invalid value.
    bool tryFindSymbolIndex(TEnum id, size_t &index) const
    {
        TEnumSym key(id);

        auto pos = std::lower_bound(_symbols.begin(), _symbols.end(), key);

        bool hasValue = false;

        if ((pos == _symbols.end()) || (pos->getId() != id))
        {
            index = _symbols.size();
        }
        else
        {
            index = static_cast<size_t>(std::distance(_symbols.begin(), pos));
            hasValue = true;
        }

        return hasValue;
    }

    //! @brief Attempts to find the index of an entry describing a specific
    //! symbol from its textual representation.
    //! @param[in] symbol The UTF-8 encoded text of the symbol value to look up.
    //! @param[out] index Receives the index of the entry describing the symbol.
    //! @retval True The symbol was found, it's index in the symbols collection
    //! was returned in index.
    //! @retval False The symbol was not defined. Index is initialised to a
    //! invalid value.
    bool tryFindSymbolIndex(const std::string_view &symbol, size_t &index) const
    {
        HashedStringView key(symbol);

        auto pos = _indexesBySymbol.find(key);
        bool hasValue = false;

        if (pos == _indexesBySymbol.end())
        {
            // Create an upper-case rendering of the symbol to search for.
            std::string upperCase;
            upperCase.reserve(key.length());

            for (char ch : symbol)
            {
                upperCase.push_back(static_cast<char>(toupper(ch)));
            }

            // Try looking up the upper-case version of the key.
            HashedStringView upperCaseKey(upperCase);
            pos = _indexesByUpperCaseSymbol.find(upperCaseKey);

            if (pos == _indexesByUpperCaseSymbol.end())
            {
                index = _indexesBySymbol.size();
            }
            else
            {
                index = pos->second;
                hasValue = true;
            }
        }
        else
        {
            index = pos->second;
            hasValue = true;
        }

        return hasValue;
    }

    //! @brief Attempts to parse a text representation of a symbol.
    //! @param[in] symbol A locale-neutral UTF-8 encoded representation of the
    //! symbol to look-up.
    //! @param[out] value Receives the symbol identifier matched from the text.
    //! @retval true A matching symbol was found and returned.
    //! @retval false No symbol matched the specified text.
    bool tryParse(const std::string_view &symbol, TEnum &value) const
    {
        size_t index;
        bool isFound = false;
        value = _symbols.front().getId();

        if (tryFindSymbolIndex(symbol, index))
        {
            value = _symbols[index].getId();
            isFound = true;
        }

        return isFound;
    }

    //! @brief Parses a text representation of a symbol.
    //! @param[in] symbol A locale-neutral UTF-8 encoded representation of the
    //! symbol to look-up.
    //! @param[in] defaultValue The symbol identifier to return if the text
    //! was not recognised.
    //! @return Either the symbol matching the text or defaultValue.
    TEnum parse(const std::string_view &symbol, TEnum defaultValue) const
    {
        size_t index;

        return tryFindSymbolIndex(symbol, index) ? _symbols[index].getId() :
                                                   defaultValue;
    }

    //! @brief Gets information about an enumeration symbol based on its index.
    //! @param[in] index The index of the symbol to obtain.
    //! @return A reference to an object describing the symbol.
    const SymbolInfo &getSymbolByIndex(size_t index) const { return _symbols.at(index); }

    //! @brief Gets information about an enumeration symbol based on its identifier.
    //! @param[in] id The identifier of the symbol to obtain.
    //! @return A reference to an object describing the symbol.
    //! @throws ArgumentException Thrown if the identifier does not have a
    //! corresponding description.
    const SymbolInfo &getSymbolById(TEnum id) const
    {
        size_t index;

        if (tryFindSymbolIndex(id, index))
        {
            return _symbols.at(index);
        }
        else
        {
            throw ArgumentException("id");
        }
    }

    //! @brief Looks up the locale-neutral textual representation of a symbol.
    //! @param[in] symbol The symbol value to look up.
    //! @return A locale-neutral string representation of the symbol,
    //! empty if not found.
    std::string_view toString(TEnum symbol) const
    {
        size_t index;

        if (tryFindSymbolIndex(symbol, index))
        {
            return _symbols[index].getSymbol();
        }
        else
        {
            return std::string_view();
        }
    }

    //! @brief Looks up the display-compatible textual representation
    //! of a symbol.
    //! @param[in] symbol The symbol value to look up.
    //! @return A display-compatible string representation the symbol,
    //! empty if not found.
    std::string_view toDisplayName(TEnum symbol) const
    {
        size_t index;

        if (tryFindSymbolIndex(symbol, index))
        {
            return _symbols[index].getDisplayName();
        }
        else
        {
            return std::string_view();
        }
    }

    //! @brief Gets a display-compatible description of a symbol.
    //! @param[in] symbol The symbol value to look up.
    //! @return A display-compatible string describing the symbol,
    //! empty if not found.
    std::string_view getDescription(TEnum symbol) const
    {
        size_t index;

        if (tryFindSymbolIndex(symbol, index))
        {
            return _symbols[index].getDescription();
        }
        else
        {
            return std::string_view();
        }
    }
};

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

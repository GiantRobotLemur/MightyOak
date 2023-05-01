//! @file ConstantSet.cpp
//! @brief The definition of an object representing table of case-insensitive
//! constants to be recognised in source code.
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

#include "ConstantSet.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to split the symbol into a prefix and numeric suffix in
//! order to look up a based constant.
//! @param[in] id The identifier to interpret.
//! @param[out] prefix Receives The based value prefix converted to upper case
//! letters.
//! @param[out] ordinal Receives the value specified in the digits which followed
//! the prefix and made up the remainder of the symbol.
//! @retval true The identifier was in the form of a based symbol and its prefix
//! and ordinal were returned.
//! @retval false The identifier was not a valid based symbol.
bool tryParseBasedSymbol(Ag::string_cref_t id, Ag::string_ref_t prefix,
                         uint32_t &ordinal)
{
    auto firstDigit = id.end();
    size_t digitCount = 0;
    bool hasDigits = false;

    for (auto pos = id.begin(); pos != id.end(); ++pos)
    {
        if (hasDigits)
        {
            if (Ag::CodePoint::isNumeric(*pos))
            {
                // Keep track of how many digits in the ordinal.
                ++digitCount;
            }
            else
            {
                // We've found a non-digit character after the run
                // of digits.
                hasDigits = false;
                break;
            }
        }
        else if (Ag::CodePoint::isNumeric(*pos))
        {
            prefix = id.substring(id.begin(), pos).toUpper();
            firstDigit = pos;
            digitCount = 1;
            hasDigits = true;
        }
    }

    // Only assume digits are an ordinal if there are no unnecessary
    // leading zeros.
    if (hasDigits && ((*firstDigit != U'0') || (digitCount == 1)))
    {
        Ag::String digits = id.substring(firstDigit, id.end());

        if (digits.tryParseScalar(ordinal, 10) == false)
        {
            prefix = Ag::String::Empty;
            ordinal = 0;
            hasDigits = false;
        }
    }
    else
    {
        prefix = Ag::String::Empty;
        ordinal = 0;
    }

    return hasDigits;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// ConstantSet Member Definitions
////////////////////////////////////////////////////////////////////////////////
const ConstantSet ConstantSet::Empty;

//! @brief Determines if the set is in an empty state.
//! @retval true No constants of any kind are defined in the set.
//! @retval false At least one based or fixed constant is defined.
bool ConstantSet::isEmpty() const
{
    return _basedConstants.empty() && _fixedConstants.empty();
}

//! @brief Determines if a constant is defined in the set without getting its value.
//! @param[in] id The identifier of the constant to query the existence of.
//! @retval true The constant is defined, possibly as a prefix followed by digits.
//! @retval false The constant is not defined in the current set.
bool ConstantSet::contains(Ag::string_cref_t id) const
{
    Ag::String prefix;
    uint32_t ordinal;
    bool hasMatch = false;

    if (tryParseBasedSymbol(id, prefix, ordinal))
    {
        auto pos = _basedConstants.find(prefix);
        hasMatch = (pos != _basedConstants.end()) &&
                   (ordinal >= pos->second.MinOrdinal) &&
                   (ordinal <= pos->second.MaxOrdinal);
    }
    else
    {
        hasMatch = (_fixedConstants.find(id.toUpper()) != _fixedConstants.end());
    }

    return hasMatch;
}

//! @brief Attempts to look up a constant symbol.
//! @param[in] id The case-insensitive identifier to look up.
//! @param[out] value The value of the constant if a match was found.
//! @retval true A matching constant definition was found.
//! @retval false No matches were found.
bool ConstantSet::tryLookupValue(Ag::string_cref_t id, Value &value) const
{
    Ag::String prefix;
    uint32_t ordinal;
    bool hasMatch = false;

    if (tryParseBasedSymbol(id, prefix, ordinal))
    {
        auto pos = _basedConstants.find(prefix);

        if ((pos != _basedConstants.end()) &&
            (ordinal >= pos->second.MinOrdinal) &&
            (ordinal <= pos->second.MaxOrdinal))
        {
            value = Value(pos->second.Offset + static_cast<int32_t>(ordinal));
            hasMatch = true;
        }
    }
    else
    {
        auto pos = _fixedConstants.find(id.toUpper());

        if (pos != _fixedConstants.end())
        {
            hasMatch = true;
            value = pos->second;
        }
    }

    return hasMatch;
}

//! @brief Adds the definition of a constant with a fixed name.
//! @param[in] id The case-insensitive name of the constant.
//! @param[in] value The value the constant represents.
void ConstantSet::addConstant(Ag::string_cref_t id, const Value &value)
{
    _fixedConstants[id.toUpper()] = value;
}

//! @brief Adds a definition for a constant symbol which will be followed by
//! digits when referenced.
//! @param[in] id The case-insensitive prefix of the set of symbols to define.
//! @param[in] minOrdinal The minimum ordinal value specified with the prefix.
//! @param[in] maxOrdinal The maximum ordinal value specified with the prefix.
//! @param[in] valueOffset Value to offset the ordinal by to produce the
//! final value.
void ConstantSet::addBasedConstant(Ag::string_cref_t id, uint32_t minOrdinal,
                                   uint32_t maxOrdinal, int32_t valueOffset /* = 0 */)
{
    _basedConstants[id.toUpper()] = BaseConstant(valueOffset, minOrdinal, maxOrdinal);
}

//! @brief Constructs an empty description of a constant symbol with an
//! appended ordinal value.
ConstantSet::BaseConstant::BaseConstant() :
    Offset(0),
    MinOrdinal(0),
    MaxOrdinal(0)
{
}

//! @brief Constructs a description of a constant symbol with an appended
//! ordinal value.
//! @param[in] offset The value to offset the ordinal by to produce the
//! final value.
//! @param[in] minOrdinal The minimum ordinal value specified with the prefix.
//! @param[in] maxOrdinal The maximum ordinal value specified with the prefix.
ConstantSet::BaseConstant::BaseConstant(int32_t offset, uint32_t minOrdinal,
                                        uint32_t maxOrdinal) :
    Offset(offset),
    MinOrdinal(minOrdinal),
    MaxOrdinal(maxOrdinal)
{
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets the set of constants which recognise the symbols used to refer
//! to core registers.
const ConstantSet &getCoreRegSymbols()
{
    static ConstantSet regs;

    if (regs.isEmpty())
    {
        // Initialise the set.
        regs.addBasedConstant("R", 0, 15); // R0 - R15
        regs.addBasedConstant("A", 1, 4, -1); // A1 - A4 as R0 - R3
        regs.addBasedConstant("V", 1, 6, 3); // V1 - V6 as R4 - R9
        regs.addConstant("SL", 10u);
        regs.addConstant("FP", 11u);
        regs.addConstant("IP", 12u);
        regs.addConstant("SP", 13u);
        regs.addConstant("LK", 14u);
        regs.addConstant("LINK", 14u);
        regs.addConstant("PC", 15u);
    }

    return regs;
}

//! @brief Gets the set of constants which recognise the symbols used to refer
//! to co-processor identifiers.
const ConstantSet &getCoProcIDSymbols()
{
    static ConstantSet coprocIds;

    if (coprocIds.isEmpty())
    {
        coprocIds.addBasedConstant("C", 0, 15);  // C0 - C15
        coprocIds.addBasedConstant("CP", 0, 15); // CP0 - CP15
        coprocIds.addBasedConstant("P", 0, 15);  // P0 - P15
    }

    return coprocIds;
}

//! @brief Gets the set of constants which recognise the symbols used to refer
//! to co-processor registers.
const ConstantSet &getCoProcRegSymbols()
{
    static ConstantSet coprocIds;

    if (coprocIds.isEmpty())
    {
        coprocIds.addBasedConstant("CR", 0, 15);  // CR0 - CR15
    }

    return coprocIds;
}

//! @brief Gets the set of constants which recognise the symbols used to refer
//! to FPA registers.
const ConstantSet &getFPARegSymbols()
{
    static ConstantSet fpaRegs;

    if (fpaRegs.isEmpty())
    {
        fpaRegs.addBasedConstant("F", 0, 7); // F0- F7
    }

    return fpaRegs;
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


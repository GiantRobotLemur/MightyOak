//! @file ConstantSet.hpp
//! @brief The declaration of an object representing table of case-insensitive
//! constants to be recognised in source code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_CONSTANT_SET_HPP__
#define __ASM_TOOLS_CONSTANT_SET_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <unordered_map>

#include "Ag/Core/String.hpp"
#include "Value.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing table of case-insensitive constants to be
//! recognised in source code.
class ConstantSet
{
public:
    // Public Members
    //! @brief An empty set of constants.
    static const ConstantSet Empty;

    // Construction/Destruction
    ConstantSet() = default;
    ~ConstantSet() = default;

    // Accessors
    bool isEmpty() const;
    bool contains(Ag::string_cref_t id) const;
    bool tryLookupValue(Ag::string_cref_t id, Value &value) const;

    // Operations
    void addConstant(Ag::string_cref_t id, const Value &value);
    void addBasedConstant(Ag::string_cref_t id, uint32_t minOrdinal,
                          uint32_t maxOrdinal, int32_t valueOffset = 0);
private:
    struct BaseConstant
    {
        int32_t Offset;
        uint32_t MinOrdinal;
        uint32_t MaxOrdinal;

        BaseConstant();
        BaseConstant(int32_t offset, uint32_t minOrdinal, uint32_t maxOrdinal);
    };

    // Internal Fields
    std::unordered_map<Ag::String, Value> _fixedConstants;
    std::unordered_map<Ag::String, BaseConstant> _basedConstants;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
const ConstantSet &getCoreRegSymbols();
const ConstantSet &getCoProcIDSymbols();
const ConstantSet &getCoProcRegSymbols();
const ConstantSet &getFPARegSymbols();

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

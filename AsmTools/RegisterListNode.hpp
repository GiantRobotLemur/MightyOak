//! @file RegisterListNode.hpp
//! @brief The declaration of a syntax node which represents a specification of
//! a set of core registers.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_REGISTER_LIST_NODE_HPP__
#define __ASM_TOOLS_REGISTER_LIST_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <utility>
#include <vector>

#include "BaseSyntaxNode.hpp"
#include "ExpressionNode.hpp"

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
//! @brief A syntax node which represents a specification of a set of
//! core registers.
class RegisterListNode : public BaseSyntaxNode
{
public:
    // Public Types
    //! @brief Represents a range of symbols in a register list.
    struct RegisterRange
    {
        Location At;
        String First;
        String Last;

        RegisterRange(const Location &at, string_cref_t symbol);
    };

    using SymbolRangeCollection = std::vector<RegisterRange>;

    // Construction/Destruction
    RegisterListNode(ParseContext &context);
    virtual ~RegisterListNode() = default;

    // Accessors
    const SymbolRangeCollection &getRanges() const;

    // Operations

    // Overrides
     virtual bool isComplete() const override;
     virtual bool isValid() const override;
     virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
     virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
private:
    // Internal Types
    enum class State
    {
        BeforeOpenBrace,
        BeforeFirstReg,
        AfterFirstReg,
        AfterDash,
        AfterSecondReg,
        AfterSeparator,

        Complete,
    };

    // Internal Functions

    // Internal Fields
    SymbolRangeCollection _ranges;
    State _state;
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

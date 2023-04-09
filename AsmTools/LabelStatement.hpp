//! @file AsmTools/LabelStatement.hpp
//! @brief The declaration of structures required to parse and process an
//! assembly language statement defining a label.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_LABEL_STATEMENT_HPP__
#define __ASM_TOOLS_LABEL_STATEMENT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/String.hpp"
#include "AsmTools/Messages.hpp"

#include "StatementListNode.hpp"

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ParseContext;

//! @brief An object represent a partially parsed statement defining an assembly
//! language label.
class LabelNode : public StatementNode
{
public:
    // Construction/Destruction
    LabelNode(ParseContext &context, const Token &labelToken);
    virtual ~LabelNode() = default;

    // Accessors

    // Operations

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node)  override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterSymbol,
        Complete,
    };

    // Internal Fields
    String _id;
    State _currentState;
};


//! @brief An object represent a statement defining an assembly language label.
class LabelStatement : public Statement
{
public:
    // Construction/Destruction
    LabelStatement(string_cref_t id, const Location &at);
    virtual ~LabelStatement() = default;

    // Accessors
    string_cref_t getID() const;
    const Location &getSourcePosition() const;

private:
    // Internal Fields
    Location _at;
    String _id;
};

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

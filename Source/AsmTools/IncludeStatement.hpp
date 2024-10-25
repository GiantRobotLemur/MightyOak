//! @file IncludeStatement.hpp
//! @brief The declaration of objects representing the use of the include
//! directive in assembly language source code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_INCLUDE_STATEMENT_HPP__
#define __ASM_TOOLS_INCLUDE_STATEMENT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/Messages.hpp"

#include "Statement.hpp"
#include "StatementListNode.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing a partially parsed include directive.
class IncludeNode : public StatementNode
{
public:
    // Construction/Destruction
    IncludeNode(ParseContext &context, const Location &at);
    virtual ~IncludeNode() = default;

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
        AfterKeyword,
        AfterFileId,
        Complete,
    };

    // Internal Fields
    Ag::String _id;
    State _currentState;
};

//! @brief An object represent a statement defining an assembly language include directive.
class IncludeStatement : public Statement
{
public:
    // Construction/Destruction
    IncludeStatement(Ag::string_cref_t id, const Location &at);
    virtual ~IncludeStatement() = default;

    // Accessors
    const Location &getLocation() const;
    Ag::string_cref_t getFileReference() const;

    // Overrides
    virtual uint32_t calculateObjectCodeSize(IEvalContext *context) const override;
    virtual bool assemble(const AssemblyState &state, IEvalContext *context,
                          ObjectCodeBuilder &output) const override;
private:
    // Internal Fields
    Location _at;
    Ag::String _id;
};

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file AsmTools/ProcStatments.hpp
//! @brief The declaration of objects related to the parsing or procedure-related
//! keywords.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_PROC_STATEMENTS_HPP__
#define __ASM_TOOLS_PROC_STATEMENTS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Expr.hpp"
#include "ExpressionNode.hpp"
#include "StatementListNode.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing an assembly language statement which defines
//! the beginning of a subroutine.
class ProcStatementNode : public StatementNode
{
public:
    // Construction/Destruction
    ProcStatementNode(ParseContext &context, const Location &start);
    virtual ~ProcStatementNode() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;

    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterKeyword,
        Complete
    };

    // Internal Fields
    State _state;
    Ag::String _id;
};

//! @brief An object representing an assembly language statement which defines
//! the beginning of a subroutine.
class ProcStatement : public Statement
{
public:
    // Construction/Destruction
    ProcStatement(const Location &at, Ag::string_cref_t id);
    virtual ~ProcStatement() = default;

    // Accessors
    Ag::string_cref_t getId() const;
    const Location &getLocation() const;

    // Overrides
    virtual bool assemble(const AssemblyState &/*state*/,
                          IEvalContext */*context*/,
                          ObjectCodeBuilder &/*output*/) const override;
private:
    // Internal Fields
    Location _at;
    Ag::String _id;
};

//! @brief An object representing an assembly language statement which defines
//! the end of a subroutine.
class EndProcStatementNode : public StatementNode
{
public:
    // Construction/Destruction
    EndProcStatementNode(ParseContext &context, const Location &start);
    virtual ~EndProcStatementNode() = default;

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context,
                                    const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context,
                                   ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;

    virtual Statement *compile(Messages &output) const override;
};

//! @brief An object representing an assembly language statement which defines
//! the beginning of a subroutine.
class EndProcStatement : public Statement
{
public:
    // Construction/Destruction
    EndProcStatement(const Location &at);
    virtual ~EndProcStatement() = default;

    // Accessors
    const Location &getLocation() const;

    // Overrides
    virtual bool assemble(const AssemblyState &/*state*/,
                          IEvalContext */*context*/,
                          ObjectCodeBuilder &/*output*/) const override;
private:
    // Internal Fields
    Location _at;
};

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

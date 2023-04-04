//! @file Statement.hpp
//! @brief The declaration of an object created to represent a parsed assembly
//! language statement.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_STATEMENT_HPP__
#define __ASM_TOOLS_STATEMENT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <memory>
#include <vector>

#include "AsmTools/Messages.hpp"
#include "AsmTools/Options.hpp"

#include "BaseSyntaxNode.hpp"


////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

namespace Asm {

//! @brief Expresses the type of a Statement-derived object.
enum class StatementType
{
    Empty,
    PreAssembledData,
    Instruction,
    AssemblyDirective,
    DataDirective,
    Label,
    Include,
    MacroInvocation,
    MacroStart,
    MacroEnd,
    ProcedureStart,
    ProcedureEnd,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class AssemblyState;
class Messages;
class IEvalContext;
class ObjectCodeBuilder;
class Statement;

//! @brief An ISyntaxNode implementation which represents a single assembly
//! language statement.
class StatementNode : public BaseSyntaxNode
{
public:
    // Construction/Destruction
    StatementNode(const ParseContext &context, const Location &start);
    StatementNode(const ParseContext &context, const Token &startToken);
    virtual ~StatementNode() = default;

    // Overrides
    // Inherited from ISyntaxNode
    // virtual bool isComplete() const = 0;
    // virtual bool isValid() const = 0;
    // virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) = 0;
    // virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) = 0;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;

    virtual Statement *compile(Messages &output) const = 0;

private:
    // Internal Fields
};

//! @brief An object created to represent a parsed assembly language statement.
class Statement
{
protected:
    // Construction/Destruction
    Statement(StatementType type);
public:
    virtual ~Statement() = default;

    // Accessors
    StatementType getType() const;

    // Operations
    virtual bool updateAssemblyState(AssemblyState &state) const;
    virtual uint32_t calculateObjectCodeSize(IEvalContext *context) const;
    virtual bool assemble(const AssemblyState &state, IEvalContext *context,
                          ObjectCodeBuilder &output) const;
private:
    // Internal Fields
    StatementType _type;
};

//! @brief An alias for unique pointers to Statement-derived objects.
using StatementUPtr = std::unique_ptr<Statement>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

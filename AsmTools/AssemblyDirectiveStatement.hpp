//! @file AssemblyDirectiveStatement.hpp
//! @brief The declaration of an object representing a statement containing an
//! assembly directive.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_ASSEMBLY_DIRECTIVE_STATEMENT_HPP__
#define __ASM_TOOLS_ASSEMBLY_DIRECTIVE_STATEMENT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <vector>

//#include "AsmEnums.hpp"
#include "Expr.hpp"
#include "ExpressionNode.hpp"
#include "StatementListNode.hpp"

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
//! @brief An object representing a statement containing an assembly directive.
class AssemblyDirectiveNode : public StatementNode
{
public:
    // Construction/Destruction
    AssemblyDirectiveNode(ParseContext &context, const Token &directive);
    virtual ~AssemblyDirectiveNode() = default;

    // Accessors

    // Operations

    // Overrides
    // Inherited from ISyntaxNode
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    AssemblyDirectiveType _type;
    InstructionSet _instructionSet;
    ArchExtensionEnum _extension;
    ProcessorMode _processorMode;
    AddressMode _addressMode;
    bool _isComplete;
};


//! @brief An object representing a statement an ALIGN directive.
class AlignDirectiveNode : public StatementNode
{
public:
    // Construction/Destruction
    AlignDirectiveNode(ParseContext &context, const Token &directive);
    virtual ~AlignDirectiveNode() = default;

    // Accessors

    // Operations

    // Overrides
    // Inherited from ISyntaxNode
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node) override;
    virtual Statement *compile(Messages &output) const override;
private:
    // Internal Types
    enum class State
    {
        AfterKeyword,
        AfterValue,
        Complete,
    };

    // Internal Functions

    // Internal Fields
    size_t _oldLexerState;
    uint32_t _value;
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

//! @file DataDirectiveStatement.hpp
//! @brief The declaration of an object representing an assembly language
//! statement which defines one or more data elements to embed in code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_DATA_DIRECTIVE_STATEMENT_HPP__
#define __ASM_TOOLS_DATA_DIRECTIVE_STATEMENT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
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

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing an assembly language
//! statement which defines one or more data elements to embed in code.
class DataDirectiveNode : public StatementNode
{
public:
    // Construction/Destruction
    DataDirectiveNode(ParseContext &context, DirectiveDataType dataType,
                      const Location &start);
    virtual ~DataDirectiveNode() = default;

    // Accessors

    // Operations

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
        AfterMnemonic,
        BeforeSeparator,
        AfterSeparator,
        Complete
    };

    // Internal Functions

    // Internal Fields
    State _state;
    DirectiveDataType _dataType;
    std::vector<ExpressionNodeUPtr> _values;
    size_t _lexicalStackBase;
};

//! @brief An object representing an assembly language
//! statement which defines one or more data elements to embed in code.
class DataDirectiveStatement : public Statement
{
public:
    // Construction/Destruction
    DataDirectiveStatement(const Location &at,
                           IExprUPtrCollection &&expressions);
    virtual ~DataDirectiveStatement() = default;

    // Accessors

    // Overrides
    virtual bool assemble(const AssemblyState &state, IEvalContext *context,
                          ObjectCodeBuilder &output) const override;

protected:
    const Location &getLocation() const;
    const IExprUPtrCollection &getValueExpressions() const;

    //! @brief Assembles a single value specified with the data directive.
    //! @param[in] value The resolved value to assemble into object code.
    //! @param[in] builder The object to receive the assembled object code.
    //! @retval true The value was correctly encoded in the object code.
    //! @retval false An error occurred, a message was logged.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const = 0;

private:
    // Internal Fields
    Location _at;
    IExprUPtrCollection _values;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

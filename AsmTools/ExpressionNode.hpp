//! @file ExpressionNode.hpp
//! @brief The declaration of a number of objects representing nested elements
//! of expressions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_EXPRESSION_NODE_HPP__
#define __ASM_TOOLS_EXPRESSION_NODE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/Messages.hpp"
#include "Expr.hpp"
#include "SyntaxNode.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Expresses the data type of a nested expression to be evaluated.
enum class ExpressionDataType
{
    Integer,
    Real,
    String,
};

//! @brief Expresses the type of a syntax node within a nested expression.
enum class ExpressionType
{
    IntegerLiteral,
    RealLiteral,
    StringLiteral,
    Symbol,
    CurrentOffset,

    Parenthesis,
    UnaryOperator,
    BinaryOperator,
};

//! @brief Expresses the operation applied by a unary operator.
enum class UnaryOperatorType
{
    Minus,
    Absolute,
    LogicalNot,

    Max,
};

//! @brief Expresses the operation applied by a binary operator.
enum class BinaryOperatorType
{
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Modulus,
    Lsl,
    Lsr,
    Asr,
    And,
    Or,
    Xor,

    Max,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class IExpr;
class ConstantSet;

//! @brief The base class for all syntax nodes involved in nested expressions.
class ExpressionNode : public ISyntaxNode
{
public:
    // Construction/Destruction
    ExpressionNode(const Location &at, ExpressionType exprType);
    virtual ~ExpressionNode() = default;

    // Accessors
    const Location &getPosition() const;
    ExpressionType getExprType() const;

    // Operations
    //! @brief Produces an IExpr-derived object which represents the
    //! sub-expression so that it can be evaluated.
    //! @param[in] constants The set of constants defined for the parsed
    //! expression being converted.
    //! @returns A pointer to an IExpr-derived object which the caller
    //! is responsible for disposing of.
    virtual IExpr *compile(const ConstantSet &constants) const =0;
private:
    // Internal Fields
    Location _position;
    ExpressionType _type;
};

//! @brief An alias for a pointer to a syntax node in an expression hierarchy.
using ExpressionNodePtr = ExpressionNode *;

//! @brief An alias for a read-only pointer to an expression syntax node.
using ExpressionNodeCPtr = const ExpressionNode *;

//! @brief An alias for a managed pointer to an expression syntax node.
using ExpressionNodeUPtr = std::unique_ptr<ExpressionNode>;

//! @brief The base class for expression nodes which do not allow any nested
//! expressions.
class TerminalExpressionNode : public ExpressionNode
{
public:
    // Construction/Destruction
    TerminalExpressionNode(const Location &at, ExpressionType exprType);
    virtual ~TerminalExpressionNode() = default;

    // Accessors

    // Operations

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node)  override;
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
};

//! @brief Represents a literal integer.
class IntegerLiteralNode : public TerminalExpressionNode
{
public:
    // Construction/Destruction
    IntegerLiteralNode(const Location &at, string_cref_t value, int radix);
    virtual ~IntegerLiteralNode() = default;

    // Accessors
    int getRadix() const;
    string_cref_t getValue() const;

    // Overrides
    virtual IExpr *compile(const ConstantSet &constants) const override;
private:
    // Internal Fields
    String _value;
    int _radix;
};

//! @brief Represents a literal floating point value.
class RealLiteralNode : public TerminalExpressionNode
{
public:
    // Construction/Destruction
    RealLiteralNode(const Location &at, string_cref_t value);
    virtual ~RealLiteralNode() = default;

    string_cref_t getValue() const;

    // Overrides
    virtual IExpr *compile(const ConstantSet &constants) const override;
private:
    // Internal Fields
    String _value;
};

//! @brief Represents a literal string value, possibly only a single character.
class StringLiteralNode : public TerminalExpressionNode
{
public:
    // Construction/Destruction
    StringLiteralNode(const Location &at, string_cref_t value, bool isUnterminted);
    virtual ~StringLiteralNode() = default;

    // Accessors
    string_cref_t getValue() const;

    // Overrides
    virtual IExpr *compile(const ConstantSet &constants) const override;
private:
    // Internal Fields
    String _value;
    bool _isUnterminted;
};

//! @brief Represents a reference to a named variable.
class SymbolNode : public TerminalExpressionNode
{
public:
    // Construction/Destruction
    SymbolNode(const Location &at, string_cref_t id);
    virtual ~SymbolNode() = default;

    // Accessors
    string_cref_t getId() const;

    // Overrides
    virtual IExpr *compile(const ConstantSet &constants) const override;
private:
    // Internal Fields
    String _id;
};

//! @brief Represents the use of the '$' symbol meaning the current
//! assembly address.
class CurrentAddressNode : public TerminalExpressionNode
{
public:
    // Construction/Destruction
    CurrentAddressNode(const Location &at);
    virtual ~CurrentAddressNode() = default;

    // Overrides
    virtual IExpr *compile(const ConstantSet & constants) const override;
};

//! @brief An expression node representing matching parenthesis.
class ParenthesisNode : public ExpressionNode
{
public:
    // Construction/Destruction
    ParenthesisNode(const Location &at);
    virtual ~ParenthesisNode() = default;

    // Accessors
    ExpressionNodeCPtr getChildExpr() const;

    // Operations

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node)  override;
    virtual IExpr *compile(const ConstantSet &constants) const override;
private:
    // Internal Types
    enum class State
    {
        BeforeExpr,
        AfterExpr,
        Complete,
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _child;
    State _state;
    bool _isValid;
};

//! @brief An expression node forming a base class for all operators.
class OperatorNode : public ExpressionNode
{
public:
    // Construction/Destruction
    OperatorNode(const Location &at, ExpressionType exprType, int precedence);

    // Accessors
    int getPresedence() const;

private:
    // Internal Fields
    int _precedence;
};

//! @brief An expression node representing the application of an
//! operator to a single sub-expression.
class UnaryOperatorNode : public OperatorNode
{
public:
    // Construction/Destruction
    UnaryOperatorNode(const Location &at, UnaryOperatorType opType);
    virtual ~UnaryOperatorNode() = default;

    // Accessors
    UnaryOperatorType getOperatorType() const;
    ExpressionNodeCPtr getChildExpr() const;

    // Operations

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node)  override;
    virtual IExpr *compile(const ConstantSet &constants) const override;
private:
    // Internal Types
    enum class State
    {
        BeforeExpr,
        Complete,
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _child;
    UnaryOperatorType _operatorType;
    State _state;
    bool _isValid;
};

//! @brief An expression node representing the application of an
//! operator to a pair of sub-expressions.
class BinaryOperatorNode : public OperatorNode
{
public:
    // Construction/Destruction
    BinaryOperatorNode(const Location &at, ExpressionNode *lhsExpr, BinaryOperatorType opType);
    virtual ~BinaryOperatorNode() = default;

    // Accessors
    BinaryOperatorType getOperatorType() const;
    ExpressionNodeCPtr getLeftChild() const;
    ExpressionNodeCPtr getRightChild() const;

    // Operations
    ExpressionNodePtr swapLeftChild(ExpressionNodePtr newLeft);

    // Overrides
    virtual bool isComplete() const override;
    virtual bool isValid() const override;
    virtual ISyntaxNode *applyToken(ParseContext &context, const Token &token) override;
    virtual ISyntaxNode *applyNode(ParseContext &context, ISyntaxNode *childNode) override;
    virtual void recover(ParseContext &context, ISyntaxNode *node)  override;
    virtual IExpr *compile(const ConstantSet &constants) const override;
private:
    // Internal Types
    enum class State
    {
        BeforeRhsExpr,
        Complete,
    };

    // Internal Functions

    // Internal Fields
    ExpressionNodeUPtr _lhsChild;
    ExpressionNodeUPtr _rhsChild;
    BinaryOperatorType _operatorType;
    State _state;
    bool _isValid;
};

//! @brief Defines an expression to be compiled after it has been parsed.
struct ExprToCompile
{
    // Public Fields
    ExpressionNodePtr Expr;
    const char *Name;
    const ConstantSet &FixedSymbols;

    // Construction/Destruction
    ExprToCompile(const ExpressionNodeUPtr &expr, const char *name);
    ExprToCompile(const ExpressionNodeUPtr &expr, const char *name,
                  const ConstantSet &fixedSymbols);
    ~ExprToCompile() = default;

    // Operations
    bool tryCompile(IExprUPtr &expr, Messages &log) const;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool compileExpressionNodes(ExprToCompile *exprNodes, IExprUPtr *exprs,
                            size_t count, Messages &log);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Compiles an array of expression syntax trees into an array of evaluable
//! expression objects.
//! @tparam NodeCount The count of nodes to compile.
//! @tparam ExprCount The count of elements in the output array.
//! @param[in] nodes The array of nodes to compile.
//! @param[out] exprs The array to receive the compiled expressions.
//! @param[in] log The log to receive errors if any expression failed to compile.
//! @retval true All expression nodes were successfully compiled.
//! @retval false At least one expression failed to compile, all previously
//! compiled expressions were disposed of before the function returned.
template<size_t NodeCount, size_t ExprCount>
bool compileExpressionArray(ExprToCompile(&nodes)[NodeCount],
                            IExprUPtr (&exprs)[ExprCount], Messages &log)
{
    static_assert(NodeCount <= ExprCount, "The result array size is too small.");

    if constexpr (NodeCount < ExprCount)
    {
        std::fill_n(exprs + NodeCount, ExprCount - NodeCount, nullptr);
    }

    return compileExpressionNodes(nodes, exprs, NodeCount, log);
}


}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

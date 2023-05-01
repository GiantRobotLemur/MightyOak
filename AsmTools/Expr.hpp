//! @file Expr.hpp
//! @brief The declaration of a set of classes representing expressions which
//! can be evaluated to calculate concrete values.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_EXPR_HPP__
#define __ASM_TOOLS_EXPR_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <vector>

#include "Ag/Core/String.hpp"
#include "AsmTools/InstructionInfo.hpp"
#include "AsmTools/Messages.hpp"

#include "Value.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief The interface to an object which provides contextual information to
//! an Expr being evaluated.
class IEvalContext : public std::enable_shared_from_this<IEvalContext>
{
public:
    // Construction/Destruction
    virtual ~IEvalContext() = default;

    // Accessors
    //! @brief Attempts to look up the value of a specified symbol.
    //! @param[in] id The case-sensitive symbol to look up.
    //! @param[out] value Receives the value of the symbol if one was found.
    //! @retval true A value was defined for the named symbol.
    //! @retval false No value was defined for the named symbol.
    virtual bool tryLookupSymbol(Ag::string_cref_t &id, Value &value) const = 0;

    //! @brief Gets the offset of the instruction currently being assembled
    //! from the beginning of the object code.
    virtual uint32_t getAssemblyOffset() const = 0;

    //! @brief Gets the address of the instruction currently being assembled
    //! given the expected address at which the object code will be loaded.
    virtual uint32_t getAssemblyAddress() const = 0;
};

//! @brief A base class representing expressions which can be evaluated to
//! calculate concrete values.
class IExpr
{
public:
    // Construction/Destruction
    virtual ~IExpr() = default;

    // Accessors
    //! @brief Gets the location of the start of the sub-expression represented.
    virtual const Location &getLocation() const =0;

    // Operations
    //! @brief Determines whether the expression evaluates to a context-free
    //! constant value.
    //! @retval true The expression fragment always evaluates to the same
    //! value.
    //! @retval false The expression may evaluate to different values depending
    //! on the context in which it is evaluated.
    virtual bool isConstant() const = 0;

    //! @brief Attempts to evaluate the expression fragment with no context.
    //! @param[out] result Receives the evaluated value of the expression.
    //! @param[out] error Receives a message if the value could not be evaluated.
    //! @retval true The constant value was successfully evaluated.
    //! @retval false An error occurred during evaluation, a message was returned.
    virtual bool tryConstantEvaluate(Value &result,
                                     Ag::string_ref_t error) const = 0;

    //! @brief Attempts to evaluate the expression fragment in a given context.
    //! @param[in] context The context in which the expression should be evaluated.
    //! @param[out] result Receives the evaluated value of the expression.
    //! @param[out] error Receives a message if the value could not be evaluated.
    //! @retval true The expression was successfully evaluated.
    //! @retval false An error occurred during evaluation, a message was returned.
    virtual bool tryEvaluate(const IEvalContext *context, Value &result,
                             Ag::string_ref_t error) const = 0;
};

using IExprPtr = IExpr *;
using IExprCPtr = const IExpr *;
using IExprUPtr = std::unique_ptr<IExpr>;
using IExprUPtrCollection = std::vector<IExprUPtr>;

//! @brief A base class for IExpr implementations which takes care of tracking
//! the source location.
class BaseExpr : public IExpr
{
public:
    // Construction/Destruction
    BaseExpr(const Location &start);
    virtual ~BaseExpr() = default;

    // Overrides
    virtual const Location &getLocation() const override;
private:
    // Internal Fields
    Location _start;
};

//! @brief Represents a fixed value.
class ConstantExpr : public BaseExpr
{
public:
    // Construction/Destruction
    ConstantExpr(const Location &at, const Value &value);
    virtual ~ConstantExpr() = default;

    // Overrides
    virtual bool isConstant() const override;
    virtual bool tryConstantEvaluate(Value &result,
                                     Ag::string_ref_t error) const override;
    virtual bool tryEvaluate(const IEvalContext *context, Value &result,
                             Ag::string_ref_t error) const override;
private:
    // Internal Fields
    Value _value;
};

//! @brief Represents a variable value.
class SymbolExpr : public BaseExpr
{
public:
    SymbolExpr(const Location &at, Ag::string_cref_t id);
    virtual ~SymbolExpr() = default;

    // Overrides
    virtual bool isConstant() const override;
    virtual bool tryConstantEvaluate(Value &result,
                                     Ag::string_ref_t error) const override;
    virtual bool tryEvaluate(const IEvalContext *context, Value &result,
                             Ag::string_ref_t error) const override;
private:
    // Internal Fields
    Ag::String _id;
};

//! @brief Represents the current assembly address in an expression.
class CurrentAddressExpr : public BaseExpr
{
public:
    CurrentAddressExpr(const Location &at);
    virtual ~CurrentAddressExpr() = default;

    // Overrides
    virtual bool isConstant() const override;
    virtual bool tryConstantEvaluate(Value &result,
                                     Ag::string_ref_t error) const override;
    virtual bool tryEvaluate(const IEvalContext *context, Value &result,
                             Ag::string_ref_t error) const override;
};

//! @brief Represents the application of a unary operator to a sub-expression.
class UnaryOpExpr : public BaseExpr
{
public:
    // Public Types
    typedef Value (*Fn)(const Value &operand, Ag::string_ref_t error);

private:
    // Construction/Destruction
    UnaryOpExpr(const Location &at, Fn opFn, IExprPtr childExpr);

public:
    static UnaryOpExpr *createMinus(const Location &at, IExprPtr childExpr);
    static UnaryOpExpr *createAbsolute(const Location &at, IExprPtr childExpr);
    static UnaryOpExpr *createNot(const Location &at, IExprPtr childExpr);
    virtual ~UnaryOpExpr() = default;

    // Overrides
    virtual bool isConstant() const override;
    virtual bool tryConstantEvaluate(Value &result,
                                     Ag::string_ref_t error) const override;
    virtual bool tryEvaluate(const IEvalContext *context, Value &result,
                             Ag::string_ref_t error) const override;
private:
    // Internal Fields
    IExprUPtr _childExpr;
    Fn _operator;
};

//! @brief Represents the application of a binary operator to a pair of
//! sub-expressions.
class BinaryOpExpr : public BaseExpr
{
public:
    // Public Types
    typedef Value (*Fn)(Value &lhsOperand, Value &rhsOperand,
                        Ag::string_ref_t error);
private:
    // Construction/Destruction
    BinaryOpExpr(const Location &at, Fn opFn, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
public:
    static BinaryOpExpr *createAddition(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createSubtraction(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createMultiplication(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createDivision(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createModulus(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createLsl(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createLsr(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createAsr(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createAnd(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createOr(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    static BinaryOpExpr *createXor(const Location &at, IExprPtr lhsChildExpr, IExprPtr rhsChildExpr);
    virtual ~BinaryOpExpr() = default;

    // Overrides
    virtual bool isConstant() const override;
    virtual bool tryConstantEvaluate(Value &result,
                                     Ag::string_ref_t error) const override;
    virtual bool tryEvaluate(const IEvalContext *context, Value &result,
                             Ag::string_ref_t error) const override;
private:
    // Internal Fields
    IExprUPtr _lhsExpr;
    IExprUPtr _rhsExpr;
    Fn _operator;
};

//! @brief Represents an expression to be evaluated to a scalar value.
struct ExprToEvaluate
{
    // Public Fields
    const IExprUPtr &Expr;
    const char *Name;
    uint32_t Max;

    // Construction/Destruction
    ExprToEvaluate(const IExprUPtr &expr, const char *name, uint32_t max);
    ~ExprToEvaluate() = default;

    // Operations
    bool tryEvaluate(IEvalContext *context, Messages &log,
                     bool isFinalPass, uint32_t &result) const;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
IExprPtr constantOptimise(IExprPtr expr);
bool tryEvaluateOrdinal(IEvalContext *context, IExprPtr expr,
                        uint32_t &ordinal, Ag::string_ref_t error);
bool tryEvaluateCoreRegister(IEvalContext *context, IExprPtr expr,
                             CoreRegister &reg, Ag::string_ref_t error);
bool tryEvaluateInteger(IEvalContext *context, IExprPtr expr,
                        int32_t &value, Ag::string_ref_t error);
bool tryEvaluateExpressions(const ExprToEvaluate *exprsToEval,
                            uint32_t *values, size_t count, IEvalContext *context,
                            Messages &log, bool isFinalPass);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
template<size_t ExprCount, size_t ValueCount>
bool tryEvaluateExpressionArray(const ExprToEvaluate(&exprsToEval)[ExprCount],
                                uint32_t(&values)[ValueCount], IEvalContext *context,
                                Messages &log, bool isFinalPass)
{
    static_assert(ExprCount <= ValueCount, "The result array is too small.");

    bool isOK = true;

    for (size_t i = 0; i < ExprCount; ++i)
    {
        isOK &= exprsToEval[i].tryEvaluate(context, log, isFinalPass, values[i]);
    }

    return isOK;
}

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

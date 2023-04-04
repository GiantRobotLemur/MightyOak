//! @file ExprContexts.hpp
//! @brief The declaration of various implementations of the IEvalContext
//! interface.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_EXPR_CONTEXTS_HPP__
#define __ASM_TOOLS_EXPR_CONTEXTS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Expr.hpp"
#include "SymbolTable.hpp"

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
struct Location;

//! @brief An interface to a an expression evaluation context which can have
//! properties dynamically set.
class IScopedContext : public IEvalContext
{
public:
    // Construction/Destruction
    virtual ~IScopedContext() = default;

    // Accessors
    //! @brief Gets the name of the current scope.
    virtual string_cref_t getScopeName() const = 0;

    //! @brief Gets the address at which the object code is expected to be loaded.
    virtual uint32_t getBaseAddress() const = 0;

    //! @brief Determines if a symbol is defined, and if so, where.
    //! @param[in] id The identifier of the symbol to look up.
    //! @param[in] scope Receives the name of the scope containing the
    //! existing symbol definition.
    //! @param[in] source Receives the source location of the existing
    //! symbol definition.
    //! @retval true The symbol is defined, scope and source are updated to
    //! describe where.
    //! @retval false The symbol is not defined in the current or any
    //! ancestor scope.
    virtual bool isSymbolDefined(string_cref_t id, string_ref_t scope,
                                 Location &source) const = 0;

    //! @brief Sets the offset of the instruction currently being assembled.
    //! @param[in] offset The new offset from the beginning of the object code.
    virtual void setAssemblyOffset(uint32_t offset) = 0;

    // Operations
    //! @brief Allows a value to be associated with a symbol name.
    //! @param[in] id The identifier to define.
    //! @param[in] source The location in source code of the definition.
    //! @param[in] value The value to associate with the symbol.
    virtual void defineSymbol(string_cref_t id, const Location &source,
                              const Value &value) = 0;
};

//! @brief An alias for a shared pointer to a scoped expression evaluation context.
using IScopedContextSPtr = std::shared_ptr<IScopedContext>;

//! @brief An object representing the top level expression evaluation context.
class RootEvalContext : public IScopedContext
{
public:
    // Construction/Destruction
    RootEvalContext(uint32_t baseAddress);
    virtual ~RootEvalContext() = default;

    // Accessors

    // Overrides
    virtual bool tryLookupSymbol(string_cref_t &id, Value &value) const override;
    virtual uint32_t getAssemblyOffset() const override;
    virtual uint32_t getAssemblyAddress() const override;

    virtual string_cref_t getScopeName() const override;
    virtual uint32_t getBaseAddress() const override;
    virtual bool isSymbolDefined(string_cref_t id, string_ref_t scope,
                                 Location &source) const override;
    virtual void setAssemblyOffset(uint32_t offset) override;
    virtual void defineSymbol(string_cref_t id, const Location &source,
                              const Value &value) override;
private:
    // Internal Fields
    SymbolTable _globalSymbols;
    uint32_t _assemblyOffset;
    uint32_t _baseAddress;
};

//! @brief An object representing an expression evaluation context which relies
//! on ancestors for additional symbol definitions.
class InnerEvalContext : public IScopedContext
{
public:
    // Construction/Destruction
    InnerEvalContext(IScopedContext *parentContext, string_cref_t name);
    virtual ~InnerEvalContext() = default;

    // Accessors

    // Overrides
    virtual bool tryLookupSymbol(string_cref_t &id, Value &value) const override;
    virtual uint32_t getAssemblyOffset() const override;
    virtual uint32_t getAssemblyAddress() const override;

    virtual string_cref_t getScopeName() const override;
    virtual uint32_t getBaseAddress() const override;
    virtual bool isSymbolDefined(string_cref_t id, string_ref_t scope,
                                 Location &source) const override;
    virtual void setAssemblyOffset(uint32_t offset) override;
    virtual void defineSymbol(string_cref_t id, const Location &source,
                              const Value &value) override;
private:
    // Internal Fields
    IScopedContext *_parentContext;
    String _name;
    SymbolTable _localSymbols;
};

class ConstantSet;

//! @brief An object which wraps an IEvalContext in order to insert a set of
//! fixed constant symbols at the inner-most scope.
class ConstantWrapperEvalContext : public IEvalContext
{
public:
    // Construction/Destruction
    ConstantWrapperEvalContext(IEvalContext *innerContext, const ConstantSet &constants);
    virtual ~ConstantWrapperEvalContext() = default;

    // Overrides
    virtual bool tryLookupSymbol(string_cref_t &id, Value &value) const override;
    virtual uint32_t getAssemblyOffset() const override;
private:
    // Internal Fields
    IEvalContext *_innerContext;
    const ConstantSet &_constants;
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

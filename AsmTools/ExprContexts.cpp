//! @file ExprContexts.cpp
//! @brief The definition of various implementations of the IEvalContext
//! interface.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Exception.hpp"

#include "ConstantSet.hpp"
#include "ExprContexts.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// RootEvalContext Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a context containing global variables which defines an
//! initial assembly offset of 0x0000.
RootEvalContext::RootEvalContext(uint32_t baseAddress) :
    _assemblyOffset(0),
    _baseAddress(baseAddress)
{
}

// Inherited from IEvalContext.
bool RootEvalContext::tryLookupSymbol(string_cref_t &id, Value &value) const
{
    return _globalSymbols.tryLookupValue(id, value);
}

// Inherited from IEvalContext.
uint32_t RootEvalContext::getAssemblyOffset() const
{
    return _assemblyOffset;
}

// Inherited from IEvalContext.
uint32_t RootEvalContext::getAssemblyAddress() const
{
    return _baseAddress + _assemblyOffset;
}

// Inherited from IScopedContext.
string_cref_t RootEvalContext::getScopeName() const
{
    static const String name("global scope");

    return name;
}

// Inherited from IScopedContext.
uint32_t RootEvalContext::getBaseAddress() const
{
    return _baseAddress;
}

// Inherited from IScopedContext.
bool RootEvalContext::isSymbolDefined(string_cref_t id, string_ref_t scope,
                                      Location &source) const
{
    bool isDefined = false;

    if (_globalSymbols.contains(id, source))
    {
        scope = getScopeName();
        isDefined = true;
    }

    return isDefined;
}

// Inherited from IScopedContext.
void RootEvalContext::setAssemblyOffset(uint32_t offset)
{
    _assemblyOffset = offset;
}

// Inherited from IScopedContext.
void RootEvalContext::defineSymbol(string_cref_t id, const Location &source,
                                   const Value &value)
{
    _globalSymbols.defineSymbol(id, source, value);
}

////////////////////////////////////////////////////////////////////////////////
// InnerEvalContext Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a context which inherits from a parent.
InnerEvalContext::InnerEvalContext(IScopedContext *parentContext,
                                   string_cref_t name) :
    _parentContext(parentContext),
    _name(name)
{
    if (_parentContext == nullptr)
    {
        throw ArgumentException("parentContext");
    }
    else if (_name.isEmpty())
    {
        throw ArgumentException("name");
    }
}

// Inherited from IEvalContext.
bool InnerEvalContext::tryLookupSymbol(string_cref_t &id, Value &value) const
{
    bool hasSymbol = false;

    if (_localSymbols.tryLookupValue(id, value))
    {
        // The symbol is defined in the local scope.
        hasSymbol = true;
    }
    else
    {
        // See if the symbol is defined in an ancestor scope.
        hasSymbol = _parentContext->tryLookupSymbol(id, value);
    }

    return hasSymbol;
}

// Inherited from IEvalContext.
uint32_t InnerEvalContext::getAssemblyOffset() const
{
    return _parentContext->getAssemblyOffset();
}

// Inherited from IEvalContext.
uint32_t InnerEvalContext::getAssemblyAddress() const
{
    return _parentContext->getAssemblyAddress();
}

// Inherited from IScopedContext.
string_cref_t InnerEvalContext::getScopeName() const
{
    return _name;
}

// Inherited from IScopedContext.
uint32_t InnerEvalContext::getBaseAddress() const
{
    return _parentContext->getBaseAddress();
}

// Inherited from IScopedContext.
bool InnerEvalContext::isSymbolDefined(string_cref_t id, string_ref_t scope,
                                       Location &source) const
{
    bool isDefined = false;

    if (_localSymbols.contains(id, source))
    {
        // The symbol is defined in the local scope.
        scope = _name;
        isDefined = true;
    }
    else
    {
        // Check ancestor scopes for a definition.
        isDefined = _parentContext->isSymbolDefined(id, scope, source);
    }

    return isDefined;
}

// Inherited from IScopedContext.
void InnerEvalContext::setAssemblyOffset(uint32_t offset)
{
    _parentContext->setAssemblyOffset(offset);
}

// Inherited from IScopedContext.
void InnerEvalContext::defineSymbol(string_cref_t id, const Location &source,
                                    const Value &value)
{
    _localSymbols.defineSymbol(id, source, value);
}


////////////////////////////////////////////////////////////////////////////////
// ConstantWrapperEvalContext Member Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a wrapper around an IEvalContext implementation which
//! inserts a set of constant symbols at the inner-most scope.
//! @param[in] innerContext The context to wrap.
//! @param[in] constants The set of constants to recognise above all other symbols.
ConstantWrapperEvalContext::ConstantWrapperEvalContext(IEvalContext *innerContext,
                                                       const ConstantSet &constants) :
    _innerContext(innerContext),
    _constants(constants)
{
}

// Inherited from IEvalContext.
bool ConstantWrapperEvalContext::tryLookupSymbol(string_cref_t &id, Value &value) const
{
    // See if the symbol is one of the constants.
    bool hasValue = _constants.tryLookupValue(id, value);

    if (hasValue == false)
    {
        // The symbol is not a constant, look it up in the wrapped scope.
        hasValue = _innerContext->tryLookupSymbol(id, value);
    }

    return hasValue;
}

// Inherited from IEvalContext.
uint32_t ConstantWrapperEvalContext::getAssemblyOffset() const
{
    return _innerContext->getAssemblyOffset();
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////

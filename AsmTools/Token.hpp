//! @file AsmTools/Token.hpp
//! @brief The declaration of an object representing a set of characters
//! recognised with a specific classification.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_TOKEN_HPP__
#define __ASM_TOOLS_TOKEN_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <type_traits>
#include <utility>

#include "Ag/Core.hpp"
#include "AsmTools/Messages.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Expresses the identifiers of properties which can be used to annotate
//! a token.
enum class TokenProperty : uint8_t
{
    Null,
    DirectiveType,
    InstructionSet,
    ProcessorExtension,
    ProcessorMode,
    AddressMode,
    IntRadix,
    DataType,
    RegisterIndex,
    Mnemonic,
    ConditionCode,
    PsrComponent,
    UpdatePsr,
    OverwritePsr,
    UserPrivilage,
    TransferDataType,
    MultiTransferMode,
    SequenceEncoding,
    LongMode,
    FpaPrecision,
    FpaRoundMode,
    ShiftType,
    UnterminatedString,

    Max,
};

//! @brief Defines the allowed classifications of recognised tokens.
enum class TokenClass
{
    Empty,

    Bang,
    Hash,
    Hat,
    Comma,
    Plus,
    Minus,

    LogicShiftLeft,
    LogicShiftRight,
    ArithmeticShiftRight,
    RotateRightShift,
    RotateRightWithExtendShift,

    OpenSquare,
    CloseSquare,
    OpenBrace,
    CloseBrace,
    OpenParenthesis,
    CloseParenthesis,
    Dollar,
    Asterisk,
    Slash,
    Symbol,
    KeywordNot,
    KeywordAnd,
    KeywordOr,
    KeywordXor,
    KeywordMod,
    IntegerLiteral,
    RealLiteral,
    StringLiteral,

    StatementTerminator,
    Label,
    AssemblyDirective,
    DataDirective,
    AlignDirective,
    RegisterCore,
    RegisterFpa,
    RegisterStatus,
    MnemonicShift,
    MnemonicAluOp,
    MnemonicMul3,
    MnemonicMul4,
    MnemonicAdr,
    MnemonicBranch,
    MnemonicBranchExchange,
    MnemonicSwi,
    MnemonicMrs,
    MnemonicMsr,
    MnemonicSingleDataTransfer,
    MnemonicMultiDataTransfer,
    MnemonicSwap,
    MnemonicBreakPt,
    MnemonicCoProcDataOp,
    MnemonicCoProcRegTransfer,
    MnemonicCoProcDataTransfer,
    MnemonicFpaDataTransfer,
    MnemonicFpaMultiTransfer,
    MnemonicFpaDyadicOp,
    MnemonicFpaMonadicOp,
    MnemonicFpaComparisonOp,
    MnemonicFpaLoadRegOp,
    MnemonicFpaStoreRegOp,
    MnemonicFpaCoreRegOp,

    //! @brief A warning passed from the lexical analyser to be converted into
    //! a warning message by the parser and then ignored.
    Warning,
    Error,
    Class_Max,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing a set of characters recognised with a specific
//! classification.
class Token
{
public:
    // Construction/Destruction
    Token();
    Token(const Location &at, Token &original);
    Token(const Location &at, TokenClass classification);
    Token(const Location &at, TokenClass classification, const String &value);
    ~Token() = default;

    // Accessors
    TokenClass getClass() const;
    const String &getValue() const;
    const Location &getLocation() const;
    bool tryGetScalarProperty(TokenProperty propId, uint8_t &value) const;
    void addScalarProperty(TokenProperty propId, uint8_t value);

    // Operations
    void clear();
    void clearProperties();

    void reset(const Location &at, TokenClass classification);
    void reset(const Location &at, TokenClass classification, const String &value);

private:
    // Internal Constants
    static const size_t MaxPropCount = 4;

    // Internal Types
    using PropValue = std::pair<uint8_t, uint8_t>;

    // Internal Fields
    Location _location;
    String _value;
    TokenClass _classification;
    PropValue _properties[MaxPropCount];
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
// NOTE: The following would be member function specialisations of Token, but
// gcc has a bug where it doesn't like them, so global function templates here:
bool getTokenFlag(const Token &token, TokenProperty propId, bool defaultValue);
void addTokenFlag(Token &token, TokenProperty propId, bool value);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to get an enumeration property from a token.
//! @tparam TEnum The data type of the property to get, must be an enumeration.
//! @param[in] token The token to query.
//! @param[in] propId The identifier of the property to get the value of.
//! @param[out] value Receives the value of the property if defined.
//! @retval true The property was defined and the value returned.
//! @retval false The property was not defined in the token.
template<typename TEnum, std::enable_if_t<std::is_enum<TEnum>::value, bool> = true>
bool tryGetTokenEnum(const Token &token, TokenProperty propId, TEnum &value)
{
    uint8_t rawValue;

    if (token.tryGetScalarProperty(propId, rawValue))
    {
        value = static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(rawValue));
        return true;
    }
    else
    {
        return false;
    }
}

//! @brief Gets an enumeration property from a token.
//! @tparam TEnum The data type of the property to get, must be an enumeration.
//! @param[in] token The token to query.
//! @param[in] propId The identifier of the property to get the value of.
//! @param[in] defaultValue The value to return if the property is not defined.
//! @returns Either the value embedded in the token or defaultValue.
template<typename TEnum, std::enable_if_t<std::is_enum<TEnum>::value, bool> = true>
TEnum getTokenEnum(const Token &token, TokenProperty propId, TEnum defaultValue)
{
    uint8_t rawValue;

    return token.tryGetScalarProperty(propId, rawValue) ?
        static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(rawValue)) :
        defaultValue;
}

//! @brief Gets an enumeration property from a token.
//! @tparam TEnum The data type of the property to get, must be an enumeration.
//! @param[in] token The token to query.
//! @param[in] propId The identifier of the property to get the value of.
//! @param[in] defaultValue The value to return if the property is not defined.
//! @returns Either the value embedded in the token or defaultValue.
template<typename TScalar, std::enable_if_t<std::is_scalar<TScalar>::value, bool> = true >
TScalar getTokenScalar(const Token &token, TokenProperty propId, TScalar defaultValue)
{
    uint8_t rawValue;

    return token.tryGetScalarProperty(propId, rawValue) ?
        static_cast<TScalar>(rawValue) :
        defaultValue;
}

//! @brief Annotates a token with a scalar property value.
//! @tparam TScalar The data type of the scalar, should be castable uint8_t.
//! @param[in] token The token to annotate.
//! @param[in] propId The identifier of the property to set.
//! @param[in] value The new property value.
template<typename TScalar, std::enable_if_t<std::is_scalar<TScalar>::value, bool> = true >
void addTokenScalar(Token &token, TokenProperty propId, TScalar value)
{
    token.addScalarProperty(propId, static_cast<uint8_t>(value));
}

//! @brief Annotates a token with an enumeration value.
//! @tparam TEnum The data type of the value to set, must be an enumeration.
//! @param[in] token The token to annotate.
//! @param[in] propId The identifier of the property to set.
//! @param[in] value The new property value.
template<typename TEnum, std::enable_if_t<std::is_enum<TEnum>::value, bool> = true>
void addTokenEnum(Token &token, TokenProperty propId, TEnum value)
{
    token.addScalarProperty(propId,
                            static_cast<uint8_t>(static_cast<std::underlying_type_t<TEnum>>(value)));
}

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

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

    template<typename T> bool tryGetProperty(TokenProperty prop, T &value) const
    {
        uint8_t rawValue;
        bool hasValue = false;

        if (tryGetScalarProperty(prop, rawValue))
        {
            value = static_cast<T>(rawValue);
            hasValue = true;
        }

        return hasValue;
    }

    template<typename T> T getProperty(TokenProperty prop, T defaultValue) const
    {
        uint8_t rawValue;
        T result = defaultValue;

        if (tryGetScalarProperty(prop, rawValue))
        {
            result = static_cast<T>(rawValue);
        }

        return result;
    }

    template<> bool getProperty(TokenProperty prop, bool defaultValue) const
    {
        uint8_t rawValue;
        bool result = defaultValue;

        if (tryGetScalarProperty(prop, rawValue))
        {
            result = rawValue != 0;
        }

        return result;
    }

    // Operations
    void clear();
    void clearProperties();

    void reset(const Location &at, TokenClass classification);
    void reset(const Location &at, TokenClass classification, const String &value);

    template<typename T> void addProperty(TokenProperty prop, T value)
    {
        typedef typename std::underlying_type<T>::type RawType;

        addScalarProperty(prop, static_cast<uint8_t>(static_cast<RawType>(value)));
    }

    template<> void addProperty(TokenProperty prop, int value)
    {
        addScalarProperty(prop, static_cast<uint8_t>(value));
    }

    template<> void addProperty(TokenProperty prop, bool value)
    {
        addScalarProperty(prop, value ? 0xFF : 0x00);
    }

private:
    // Internal Constants
    static const size_t MaxPropCount = 4;

    // Internal Types
    using PropValue = std::pair<uint8_t, uint8_t>;

    // Internal Functions

    // Internal Fields
    Location _location;
    String _value;
    TokenClass _classification;
    PropValue _properties[MaxPropCount];
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

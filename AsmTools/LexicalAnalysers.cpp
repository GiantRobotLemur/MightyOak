//! @file LexicalAnalysers.cpp
//! @brief The definition of various ILexicalContext implementations.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cwchar>
#include <string>
#include <unordered_map>

#include "Ag/Core/LinearSortedMap.hpp"

#include "AddressDirectiveNode.hpp"
#include "AluInstructionNode.hpp"
#include "AssemblyDirectiveStatement.hpp"
#include "AtomicSwapInstructionNode.hpp"
#include "CoProcInstructionStatements.hpp"
#include "DataDirectiveStatement.hpp"
#include "DataTransferInstructionNode.hpp"
#include "ExpressionNode.hpp"
#include "IncludeStatement.hpp"
#include "LabelStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "LexicalContext.hpp"
#include "MultiTransferInstructionNode.hpp"
#include "RegisterListNode.hpp"
#include "SimpleInstructionStatements.hpp"
#include "StatementListNode.hpp"

#include "AsmTools/InstructionInfo.hpp"
#include "AsmTools/Options.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Skips white space characters in an input stream which don't represent
//! a line break.
//! @param[in] input The input stream to read characters from.
void skipNoneNewlineWhiteSpace(InputContext &input)
{
    char32_t next;

    while (input.tryGetNextCharacter(next))
    {
        if ((next == U'\n') || (CodePoint::isWhiteSpace(next) == false))
        {
            input.ungetCharacter();
            break;
        }
    }
}

//! @brief Creates an upper case UTF-8 string from a UTF-32 STL string.
//! @param[in] text The STL string to process.
//! @return A string containing the transformed and converted characters.
String makeKey(const std::u32string &text, size_t length)
{
    std::string buffer;
    size_t hintLength = std::min(length, text.length());
    buffer.reserve((hintLength * 110) / 100);

    Utf::ToUtf8Converter converter;

    for (size_t index = 0; index < hintLength; ++index)
    {
        converter.setCodePoint(CodePoint::toUpper(text[index]));
        uint8_t next;

        while (converter.tryGetNextByte(next))
        {
            buffer.push_back(static_cast<char>(next));
        }
    }

    return String(buffer);
}

//! @brief Determines if a raw UTF-32 string begins with a static prefix.
//! @param[in] text The text to analyse.
//! @param[in] prefix The bounded prefix string to test for.
//! @retval true Text starts with the characters in prefix.
//! @retval false Text is too short of starts with different characters.
bool startsWith(const std::u32string &text, const std::u32string_view &prefix)
{
    bool hasPrefix = false;

    if (text.length() >= prefix.length())
    {
        hasPrefix = true;

        for (size_t i = 0, c = prefix.length(); hasPrefix && (i < c); ++i)
        {
            hasPrefix = text[i] == prefix[i];
        }
    }

    return hasPrefix;
}

//! @brief Harvests alpha numeric characters and adds them to a buffer.
//! @param[in] input The input stream to read.
//! @param[in] buffer The buffer to add characters to until an invalid
//! character or the end of input is reached.
void accumulateAlphaNumeric(InputContext &input, std::u32string &buffer)
{
    char32_t next;

    while (input.tryGetNextCharacter(next))
    {
        if (CodePoint::isAlphaNumeric(next))
        {
            buffer.push_back(next);
        }
        else
        {
            input.ungetCharacter();
            break;
        }
    }
}

//! @brief Harvests symbol characters and adds them to a buffer.
//! @param[in] input The input stream to read.
//! @param[in] buffer The buffer to add characters to until an invalid
//! character or the end of input is reached.
void accumulateSymbol(InputContext &input, std::u32string &buffer)
{
    char32_t next;

    while (input.tryGetNextCharacter(next))
    {
        if ((next == U'_') || CodePoint::isAlphaNumeric(next))
        {
            buffer.push_back(next);
        }
        else
        {
            input.ungetCharacter();
            break;
        }
    }
}

struct MnemonicContext;

//! @brief Describes a data used to interpret a specific instruction mnemonic.
struct MnemonicMapping
{
    typedef Token(*Fn)(MnemonicContext &context);
    Fn Interpret;
    TokenClass Classifiation;
    InstructionMnemonic Mnemonic;
    DirectiveDataType DataType;

    MnemonicMapping() :
        Interpret(nullptr),
        Classifiation(TokenClass::Empty),
        Mnemonic(InstructionMnemonic::MaxMnemonic),
        DataType(DirectiveDataType::Word)
    {
    }

    MnemonicMapping(TokenClass tokenClass) :
        Interpret(nullptr),
        Classifiation(tokenClass),
        Mnemonic(InstructionMnemonic::MaxMnemonic),
        DataType(DirectiveDataType::Word)
    {
    }

    MnemonicMapping(Fn fn, TokenClass tokenClass) :
        Interpret(fn),
        Classifiation(tokenClass),
        Mnemonic(InstructionMnemonic::MaxMnemonic),
        DataType(DirectiveDataType::Word)
    {
    }

    MnemonicMapping(Fn fn, TokenClass tokenClass, InstructionMnemonic mnemonic) :
        Interpret(fn),
        Classifiation(tokenClass),
        Mnemonic(mnemonic),
        DataType(DirectiveDataType::Word)
    {
    }

    MnemonicMapping(Fn fn, DirectiveDataType dataType) :
        Interpret(fn),
        Classifiation(TokenClass::DataDirective),
        Mnemonic(InstructionMnemonic::MaxMnemonic),
        DataType(dataType)
    {
    }
};

//! @brief A context used when interpreting an instruction mnemonic.
struct MnemonicContext
{
    const Location &Position;
    const std::u32string &Text;
    size_t Index;
    TokenClass Class;
    InstructionMnemonic Mnemonic;
    DirectiveDataType DataType;

    MnemonicContext(const Location &position, const std::u32string &text,
                    const MnemonicMapping &mapping) :
        Position(position),
        Text(text),
        Index(0),
        Class(mapping.Classifiation),
        Mnemonic(mapping.Mnemonic),
        DataType(mapping.DataType)
    {
    }

    // Accessors
    //! @brief Attempts to get an unparsed character.
    //! @param[in] offset The 0-based offset from Index of the character to
    //! attempt to access.
    //! @param[out] ch Receives the upper case character, if it could be accessed.
    //! @retval true The character was accessible and returned.
    //! @retval false The character was beyond the end of the mnemonic, ch is zeroed.
    bool tryGetChar(size_t offset, char32_t &ch) const
    {
        size_t index = Index + offset;
        bool hasCharacter = false;
        ch = U'\0';

        if (index < Text.length())
        {
            ch = CodePoint::toUpper(Text[index]);
            hasCharacter = true;
        }

        return hasCharacter;
    }

    // Operations
    //! @brief Interprets the condition code after the characters of an
    //! instruction mnemonic.
    //! @param[in] insturctionToken The token to annotate with the condition code.
    void parseConditionCode(Token &instructionToken)
    {
        size_t length = Text.length() - Index;

        if (length > 1)
        {
            char32_t first = CodePoint::toUpper(Text[Index]);
            char32_t second = CodePoint::toUpper(Text[Index + 1]);
            bool hasCondition = true;
            ConditionCode code = ConditionCode::Al;

            switch (first)
            {
            case U'A':
                if (second == U'L')
                {
                    code = ConditionCode::Al;
                }
                else
                {
                    hasCondition = false;
                }
                break;

            case U'C':
                switch (second)
                {
                case U'C':
                    code = ConditionCode::Cc;
                    break;

                case U'S':
                    code = ConditionCode::Cs;
                    break;

                default:
                    hasCondition = false;
                    break;
                }
                break;

            case U'E':
                if (second == U'Q')
                {
                    code = ConditionCode::Eq;
                }
                else
                {
                    hasCondition = false;
                }
                break;

            case U'G':
                switch (second)
                {
                case U'E':
                    code = ConditionCode::Ge;
                    break;

                case U'T':
                    code = ConditionCode::Gt;
                    break;

                default:
                    hasCondition = false;
                    break;
                }
                break;

            case U'H':
                switch (second)
                {
                case U'I':
                    code = ConditionCode::Hi;
                    break;

                case U'S':
                    code = ConditionCode::Hs;
                    break;

                default:
                    hasCondition = false;
                    break;
                }
                break;

            case U'L':
                switch (second)
                {
                case U'E':
                    code = ConditionCode::Le;
                    break;

                case U'O':
                    code = ConditionCode::Lo;
                    break;

                case U'S':
                    code = ConditionCode::Ls;
                    break;

                case U'T':
                    code = ConditionCode::Lt;
                    break;

                default:
                    hasCondition = false;
                    break;
                }
                break;

            case U'M':
                if (second == U'I')
                {
                    code = ConditionCode::Mi;
                }
                else
                {
                    hasCondition = false;
                }
                break;

            case U'N':
                switch (second)
                {
                case U'E':
                    code = ConditionCode::Ne;
                    break;

                case U'V':
                    code = ConditionCode::Nv;
                    break;

                default:
                    hasCondition = false;
                    break;
                }
                break;

            case U'P':
                if (second == U'L')
                {
                    code = ConditionCode::Pl;
                }
                else
                {
                    hasCondition = false;
                }
                break;

            case U'V':
                switch (second)
                {
                case U'C':
                    code = ConditionCode::Vc;
                    break;

                case U'S':
                    code = ConditionCode::Vs;
                    break;

                default:
                    hasCondition = false;
                }
                break;

            default:
                hasCondition = false;
                break;
            }

            if (hasCondition)
            {
                // Annotate the token with the condition code and move past it.
                Index += 2;
                instructionToken.addProperty(TokenProperty::ConditionCode, code);
            }
        }
    }

    //! @brief Interprets a single character suffix after the characters of an
    //! instruction mnemonic.
    //! @param[in] insturctionToken The token to annotate with the suffix.
    //! @param[in] propId The boolean token property to set if the suffix is recognised.
    //! @param[in] suffixChar The single character suffix to check for.
    //! @retval true The suffix was applied.
    //! @retval false No suffix was found.
    bool parseSuffix(Token &instructionToken, TokenProperty propId,
                     char32_t suffixChar)
    {
        size_t length = Text.length() - Index;
        bool suffixFound = false;

        if (length > 0)
        {
            char32_t suffix = CodePoint::toUpper(Text[Index]);

            if (suffix == suffixChar)
            {
                instructionToken.addProperty(propId, true);
                ++Index;
                suffixFound = true;
            }
        }

        return suffixFound;
    }

    //! @brief Parses the FPA precision mode suffix in order to annotate an FPA
    //! instruction mnemonic token.
    //! @param[in] instruction The token to annotate.
    //! @param[in] allowPackedBCD True to allow the 'P' suffix, false to ignore it.
    //! @retval true The suffix was recognised and the token annotated.
    //! @retval false The suffix was not recognised.
    bool tryParseFpaPrecision(Token &instruction, bool allowPackedBCD)
    {
        bool isValid = false;
        char32_t precisionChar;

        if (tryGetChar(0, precisionChar))
        {
            FpaPrecision precision = FpaPrecision::Single;
            isValid = true;

            switch (precisionChar)
            {
            case U'S':
                precision = FpaPrecision::Single;
                break;

            case U'D':
                precision = FpaPrecision::Double;
                break;

            case U'E':
                precision = FpaPrecision::Extended;
                break;

            case U'P':
                if (allowPackedBCD)
                {
                    precision = FpaPrecision::Packed;
                }
                else
                {
                    isValid = false;
                }
                break;

            default:
                isValid = false;
                break;
            }

            if (isValid)
            {
                ++Index;

                instruction.addProperty(TokenProperty::FpaPrecision, precision);
            }
        }

        return isValid;
    }

    //! @brief Parses the FPA rounding mode suffix in order to annotate an FPA
    //! instruction mnemonic token.
    //! @param[in] instruction The token to annotate.
    //! @retval true The suffix was recognised and the token annotated.
    //! @retval false The suffix was not recognised.
    bool tryParseFpaRoundMode(Token &instruction)
    {
        bool isValid = true;
        char32_t roundChar;

        if (tryGetChar(0, roundChar))
        {
            FpaRoundMode roundMode = FpaRoundMode::Nearest;

            switch (roundChar)
            {
            case U'P':
                roundMode = FpaRoundMode::Plus;
                break;

            case U'M':
                roundMode = FpaRoundMode::Minus;
                break;

            case U'Z':
                roundMode = FpaRoundMode::Zero;
                break;

            default:
                isValid = false;
                break;
            }

            if (isValid)
            {
                ++Index;
                instruction.addProperty(TokenProperty::FpaRoundMode, roundMode);
            }
        }

        return isValid;
    }

    //! @brief Ensures that the next characters have specific values.
    //! @param[in] suffix The suffix to match.
    //! @param[in] length The length of the prefix.
    //! @retval true The suffix matches and the index moved on.
    //! @retval false The suffix doesn't match.
    bool ensureSuffix(const utf32_cptr_t suffix, size_t length)
    {
        size_t charsLeft = Text.length() - Index;
        bool hasSuffix = false;

        if (length <= charsLeft)
        {
            hasSuffix = true;

            for (size_t offset = 0; hasSuffix && (offset < length); ++offset)
            {
                hasSuffix = CodePoint::toUpper(Text[Index + offset]) == suffix[offset];
            }

            if (hasSuffix)
            {
                Index += length;
            }
        }

        return hasSuffix;
    }

    //! @brief Ensures that the next characters have specific values.
    //! @tparam TLen The length of the array of character to compare against.
    //! @param[in] suffix The suffix to match.
    //! @retval true The suffix matches and the index moved on.
    //! @retval false The suffix doesn't match.
    template<size_t TLen>
    bool ensureSuffix(const char32_t (&suffix)[TLen])
    {
        return ensureSuffix(suffix, arraySize(suffix) - 1);
    }

    //! @brief Ensures there are no characters after the end of the recognised
    //! instruction mnemonic and creates an error token if there are.
    //! @param[in] mnemonic The token to update on error.
    void verifyComplete(Token &mnemonic) const
    {
        if (Index < Text.length())
        {
            // There are unrecognised characters after those which were recognised.
            String instruction(Text);
            String message = String::format("The instruction mnemonic '{0}' is invalid.",
                                            { instruction });

            Location oldLocation = mnemonic.getLocation();
            mnemonic.reset(oldLocation, TokenClass::Error, message);
        }
    }
};

//! @brief Creates a token from a recognised ALU instruction mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseAluOp(MnemonicContext &context)
{
    Token result(context.Position, context.Class);
    result.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    // Parse condition code.
    context.parseConditionCode(result);

    // Parse the 'S' suffix.
    context.parseSuffix(result, TokenProperty::UpdatePsr, U'S');

    return result;
}

//! @brief Creates a token from a recognised ALU comparison instruction mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseAluCmpOp(MnemonicContext &context)
{
    Token result(context.Position, context.Class);
    result.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    // Parse condition code.
    context.parseConditionCode(result);

    // Parse the 'P' suffix.
    context.parseSuffix(result, TokenProperty::OverwritePsr, U'P');

    return result;
}

//! @brief Creates a token from a recognised long multiply instruction mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseLongMulOp(MnemonicContext &context)
{
    Token result(context.Position, context.Class);

    if (context.Text.length() >= 5)
    {
        result.addProperty(TokenProperty::Mnemonic, context.Mnemonic);
        char32_t third = CodePoint::toUpper(context.Text[2]);

        if (third == U'U')
        {
            // It's UMULL or SMULL.
            if (context.ensureSuffix(U"LL"))
            {
                context.parseConditionCode(result);
                context.parseSuffix(result, TokenProperty::UpdatePsr, U'S');
            }
            else
            {
                // The mnemonic is invalid.
                context.Index = 0;
            }
        }
        else if (third == U'L')
        {
            // It's UMLAL or SMLAL.
            if (context.ensureSuffix(U"AL"))
            {
                context.parseConditionCode(result);
                context.parseSuffix(result, TokenProperty::UpdatePsr, U'S');
            }
            else
            {
                // The mnemonic is invalid.
                context.Index = 0;
            }
        }
    }

    return result;
}

//! @brief Parses a breakpoint instruction.
Token parseBreakPoint(MnemonicContext &context)
{
    if (context.ensureSuffix(U"T")) // Verify BKPT mnemonic.
    {
        // Its a BKPT instruction, which never uses a condition code.
        Token instruction(context.Position, TokenClass::MnemonicBreakPt);

        instruction.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::Bkpt);

        // Theoretically a breakpoint instruction is not conditional, it is 
        // encoded with the AL (always) condition code. Let's allow that, but
        // penalise use of any other condition codes later.
        context.parseConditionCode(instruction);

        return instruction;
    }
    else
    {
        context.Index = 0;
        return Token();
    }
}

//! @brief Creates a token from a recognised branch instruction mnemonic with a
//! condition code.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic or an error.
Token parseConditionalBranch(MnemonicContext &context)
{
    // We can assume the first character is 'B'.
    context.Index = 1;

    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::B);

    context.parseConditionCode(instruction);

    return instruction;
}

//! @brief Creates a token from a recognised BL instruction mnemonic but
//! it could be a B mnemonic with a condition code of a BL mnemonic with
//! a condition code.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic or an error.
Token parseAmbiguousBranch(MnemonicContext &context)
{
    Token instruction;

    if (context.Text.length() == 3)
    {
        // It's a branch instruction with a condition code.
        instruction.reset(context.Position, TokenClass::MnemonicBranch);
        instruction.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::B);

        context.Index = 1;

        // TODO: What if the condition code is invalid?
        context.parseConditionCode(instruction);
    }
    else if (context.Text.length() == 4)
    {
        // It should be a BL instruction with a condition code.
        instruction.reset(context.Position, TokenClass::MnemonicBranch);
        instruction.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::Bl);

        context.Index = 2;

        // TODO: What if the condition code is invalid?
        context.parseConditionCode(instruction);
    }

    return instruction;
}

//! @brief Parses a single value data transfer instruction mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseDataTransfer(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    context.parseConditionCode(instruction);

    char32_t first;

    if (context.tryGetChar(0, first))
    {
        char32_t second = U'\0';

        switch (first)
        {
        case U'B':
            instruction.addProperty(TokenProperty::TransferDataType,
                                    TransferDataType::UnsignedByte);
            ++context.Index;

            if (context.tryGetChar(0, second) && (second == U'T'))
            {
                instruction.addProperty(TokenProperty::UserPrivilage, true);
                ++context.Index;
            }
            break;

        case U'T':
            instruction.addProperty(TokenProperty::UserPrivilage, true);
            ++context.Index;
            break;

        case U'H':
            instruction.addProperty(TokenProperty::TransferDataType,
                                    TransferDataType::UnsignedHalfWord);
            ++context.Index;
            break;

        case U'S':
            if (context.tryGetChar(1, second))
            {
                if (second == U'B')
                {
                    instruction.addProperty(TokenProperty::TransferDataType,
                                            TransferDataType::SignedByte);
                    context.Index += 2;
                }
                else if (second == U'H')
                {
                    instruction.addProperty(TokenProperty::TransferDataType,
                                            TransferDataType::SignedHalfWord);
                    context.Index += 2;
                }
            }
            break;
        }
    }

    return instruction;
}

//! @brief Parses the mnemonic for a multi-word data transfer instruction (LDM/STM).
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseMultiDataTransfer(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    context.parseConditionCode(instruction);

    char32_t first, second;

    if (context.tryGetChar(0, first) && context.tryGetChar(1, second))
    {
        MultiTransferMode mode = MultiTransferMode::DecrementAfter;
        bool isValid = true;

        switch (first)
        {
        case U'D':
            switch (second)
            {
            case U'A': mode = MultiTransferMode::DecrementAfter; break;
            case U'B': mode = MultiTransferMode::DecrementBefore; break;
            default: isValid = false; break;
            }
            break;

        case U'E':
            switch (second)
            {
            case U'A': mode = MultiTransferMode::EmptyAscending; break;
            case U'D': mode = MultiTransferMode::EmptyDescending; break;
            default: isValid = false; break;
            }
            break;

        case U'F':
            switch (second)
            {
            case U'A': mode = MultiTransferMode::FullAscending; break;
            case U'D': mode = MultiTransferMode::FullDescending; break;
            default: isValid = false; break;
            }
            break;

        case U'I':
            switch (second)
            {
            case U'A': mode = MultiTransferMode::IncrementAfter; break;
            case U'B': mode = MultiTransferMode::IncrementBefore; break;
            default: isValid = false; break;
            }
            break;

        default:
            isValid = false;
            break;
        }

        if (isValid)
        {
            instruction.addProperty(TokenProperty::MultiTransferMode, mode);
            context.Index += 2;
        }
        else
        {
            // Define a custom error.
            String mnemonic(context.Text);
            String message = String::format("Invalid suffix on multi-data transfer instruction '{0}'.",
                                            { mnemonic });

            instruction.reset(context.Position, TokenClass::Error, message);
            context.Index = context.Text.length();
        }
    }
    else
    {
        // Define a custom error.
        String mnemonic(context.Text);
        String message = String::format("Multi-data transfer instruction '{0}' missing transfer mode suffix.",
                                        { mnemonic });

        instruction.reset(context.Position, TokenClass::Error, message);
        context.Index = context.Text.length();
    }

    return instruction;
}

//! @brief Parses a generic instruction with no suffixes other than the
//! condition code.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseGenericInstruction(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    context.parseConditionCode(instruction);

    return instruction;
}

//! @brief Parses a swap instruction.
//! @param[in] position The position in source code of the first character
//! of the mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseSwapInstruction(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);

    context.parseConditionCode(instruction);

    char32_t suffix;
    
    if (context.tryGetChar(0, suffix) &&
        (suffix == U'B'))
    {
        instruction.addProperty(TokenProperty::TransferDataType, TransferDataType::UnsignedByte);
        ++context.Index;
    }

    return instruction;
}

//! @brief Parses a generic instruction which accepts a condition
//! code and an optional 'L' suffix.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseGenericWithLongSuffix(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    context.parseConditionCode(instruction);

    // Check for an operand size suffix.
    context.parseSuffix(instruction, TokenProperty::LongMode, U'L');

    return instruction;
}

//! @brief Creates a token from a recognised FPA data processing
//! instruction mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseFpaDataOp(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    // Parse condition code
    context.parseConditionCode(instruction);

    // The precision mode is mandatory.
    if (context.tryParseFpaPrecision(instruction, false))
    {
        // The rounding mode is optional.
        context.tryParseFpaRoundMode(instruction);
    }
    else
    {
        // Create a custom error message for a missing suffix.
        String mnemonic(context.Text);
        String message = String::format("Missing precision suffix on FPA instruction mnemonic '{0}'.",
                                        { mnemonic });

        instruction.reset(context.Position, TokenClass::Error, message);

        context.Index = context.Text.length();
    }

    return instruction;
}

//! @brief Creates a token from a recognised FPA comparison instruction mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseFpaCmpOp(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);

    size_t excess = context.Text.length() - context.Index;

    if (excess == 0)
    {
        instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);
    }
    else if ((excess == 1) || (excess == 3))
    {
        if (CodePoint::toUpper(context.Text[context.Index]) == U'E')
        {
            if (context.Mnemonic == InstructionMnemonic::Cmf)
            {
                ++context.Index;
                instruction.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::Cmfe);
            }
            else if (context.Mnemonic == InstructionMnemonic::Cnf)
            {
                ++context.Index;
                instruction.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::Cnfe);
            }
        }

        context.parseConditionCode(instruction);
    }
    else if (excess == 2)
    {
        // Enough space for just a condition code.
        context.parseConditionCode(instruction);
    }
    else
    {
        // Mark the mnemonic as invalid.
        context.Index = 0;
    }

    return instruction;
}

//! @brief Creates a token from a recognised FPA FIX.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseFpaStoreRegisterMnemonic(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    // Parse condition code
    context.parseConditionCode(instruction);

    // Parse the optional rounding mode.
    context.tryParseFpaRoundMode(instruction);

    return instruction;
}

//! @brief Creates a token from a recognised FPA data processing
//! instruction mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseFpaDataTransfer(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    // Parse condition code
    context.parseConditionCode(instruction);

    // Parse the mandatory precision mode.
    if (context.tryParseFpaPrecision(instruction, true) == false)
    {
        // Mark the mnemonic as invalid.
        context.Index = 0;
    }

    return instruction;
}

//! @brief Creates a token from a recognised FPA multi-data processing
//! instruction mnemonic.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseFpaMultiTransfer(MnemonicContext &context)
{
    Token instruction(context.Position, context.Class);
    instruction.addProperty(TokenProperty::Mnemonic, context.Mnemonic);

    // Parse condition code.
    context.parseConditionCode(instruction);

    // Parse the optional transfer mode.
    char32_t first, second;

    if (context.tryGetChar(0, first) && context.tryGetChar(1, second))
    {
        if ((first == U'E') && (second == U'A'))
        {
            context.Index += 2;
            instruction.addProperty(TokenProperty::MultiTransferMode,
                                    MultiTransferMode::EmptyAscending);
        }
        else if ((first == U'F') && (second == U'D'))
        {
            context.Index += 2;
            instruction.addProperty(TokenProperty::MultiTransferMode,
                                    MultiTransferMode::FullDescending);
        }
    }

    return instruction;
}

//! @brief Creates a token from a recognised data directive with an EQU prefix.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseEquDataDirective(MnemonicContext &context)
{
    Token directive(context.Position, context.Class);

    char32_t next;

    if (context.tryGetChar(0, next))
    {
        switch (next)
        {
        case U'B':
            directive.addProperty(TokenProperty::DataType,
                                  DirectiveDataType::Byte);
            ++context.Index;
            break;

        case U'W':
            directive.addProperty(TokenProperty::DataType,
                                  DirectiveDataType::HalfWord);
            ++context.Index;
            break;

        case U'D':
            directive.addProperty(TokenProperty::DataType,
                                  DirectiveDataType::Word);
            ++context.Index;
            break;

        case U'Q':
            directive.addProperty(TokenProperty::DataType,
                                  DirectiveDataType::LongWord);
            ++context.Index;
            break;

        case U'S':
            ++context.Index;

            if (context.tryGetChar(0, next) && (next == U'U'))
            {
                // It's a possibly a Unicode string.
                size_t left = context.Text.length() - context.Index;

                if ((left == 2) &&
                    (context.Text[context.Index + 1] == U'8'))
                {
                    directive.addProperty(TokenProperty::DataType,
                                          DirectiveDataType::Utf8String);
                    context.Index += 2;
                }
                else if ((left == 3) &&
                         (context.Text[context.Index + 1] == U'1') &&
                         (context.Text[context.Index + 2] == U'6'))
                {
                    directive.addProperty(TokenProperty::DataType,
                                          DirectiveDataType::Utf16String);
                    context.Index += 2;
                }
                else if ((left == 3) &&
                         (context.Text[context.Index + 1] == U'3') &&
                         (context.Text[context.Index + 2] == U'2'))
                {
                    directive.addProperty(TokenProperty::DataType,
                                          DirectiveDataType::Utf32String);
                    context.Index += 2;
                }
                else
                {
                    // Mark the token as invalid.
                    context.Index = 0;
                }
            }
            else
            {
                directive.addProperty(TokenProperty::DataType,
                                      DirectiveDataType::NativeString);
            }
            break;

        case U'F':
            directive.addProperty(TokenProperty::DataType,
                                  DirectiveDataType::Real32);
            ++context.Index;
            break;

        case U'R':
            directive.addProperty(TokenProperty::DataType,
                                  DirectiveDataType::Real64);
            ++context.Index;
            break;

        case U'E':
            directive.addProperty(TokenProperty::DataType,
                                  DirectiveDataType::Real96);
            ++context.Index;
            break;

        default:
            // Make the mnemonic as invalid.
            context.Index = 0;
            break;
        }
    }

    return directive;
}

//! @brief Creates a token from a recognised data directive with a known data type.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseDataDirective(MnemonicContext &context)
{
    Token directive(context.Position, context.Class);
    directive.addProperty(TokenProperty::DataType, context.DataType);

    return directive;
}

//! @brief Creates a token if the align directive is recognised.
//! @param[in] context The context to interpret to create the token.
//! @returns A token object representing an instruction mnemonic.
Token parseAlign(MnemonicContext &context)
{
    Token directive(context.Position, context.Class);

    char32_t first, second;

    if (context.tryGetChar(0, first) &&
        context.tryGetChar(1, second) &&
        (first == U'G') && (second == U'N'))
    {
        context.Index += 2;
    }
    else
    {
        // Ensure the token is marked as erroneous.
        context.Index = 0;
    }

    return directive;
}

//! @brief Interprets a possible label definition.
//! @param[in] position The position in source code of the first character
//! of the perspective token.
//! @param[in] buffer The characters of the label after the leading '.'.
//! @returns The token created from the characters.
Token interpretLabelDefinition(const Location &position, std::u32string &buffer)
{
    TokenClass classification = TokenClass::Error;
    String value;

    if (buffer.empty())
    {
        value = "Empty label definition.";
    }
    else
    {
        classification = TokenClass::Label;
        value = String(buffer.c_str(), buffer.length());
    }

    return Token(position, classification, value);
}

//! @brief Interprets a possible assembly directive following the '%' character.
//! @param[in] position The position in source code of the first character
//! of the perspective token.
//! @param[in] buffer The characters of the directive after the leading '%'.
//! @returns The token created from the characters.
Token interpretDirective(const Location &position, std::u32string &buffer)
{
    Token result;

    if (buffer.empty())
    {
        result = Token(position, TokenClass::Error, "Empty assembly directive.");
    }
    else
    {
        // Store the possible matches in a hash table for fast lookup.
        static std::unordered_map<String, Token> directiveById;

        if (directiveById.empty())
        {
            Location empty;
            Token temp(empty, TokenClass::AssemblyDirective);
            temp.addProperty(TokenProperty::DirectiveType, AssemblyDirectiveType::Include);

            directiveById["INCLUDE"] = temp;

            // Create templates for the processor model directives.
            temp.clearProperties();
            temp.addProperty(TokenProperty::DirectiveType, AssemblyDirectiveType::InstructionSet);
            temp.addProperty(TokenProperty::InstructionSet, InstructionSet::ArmV2);
            directiveById["ARMV2"] = temp;

            temp.addProperty(TokenProperty::InstructionSet, InstructionSet::ArmV2a);
            directiveById["ARMV2A"] = temp;

            temp.addProperty(TokenProperty::InstructionSet, InstructionSet::ArmV3);
            directiveById["ARMV3"] = temp;

            temp.addProperty(TokenProperty::InstructionSet, InstructionSet::ArmV4);
            directiveById["ARMV4"] = temp;

            // Create templates for the processor extensions directives.
            temp.clearProperties();
            temp.addProperty(TokenProperty::DirectiveType, AssemblyDirectiveType::ProcessorExtension);
            temp.addProperty(TokenProperty::ProcessorExtension, ArchExtensionEnum::Fpa);
            directiveById["FPA"] = temp;

            temp.addProperty(TokenProperty::ProcessorExtension, ArchExtensionEnum::VfpV1);
            directiveById["VFPV1"] = temp;

            // Create templates for the processor mode directives.
            temp.clearProperties();
            temp.addProperty(TokenProperty::DirectiveType, AssemblyDirectiveType::ProcessorMode);
            temp.addProperty(TokenProperty::ProcessorMode, ProcessorMode::Arm);
            directiveById["ARM"] = temp;

            temp.addProperty(TokenProperty::ProcessorMode, ProcessorMode::Thumb);
            directiveById["THUMB"] = temp;

            // Create templates for the assembly mode directives.
            temp.clearProperties();
            temp.addProperty(TokenProperty::DirectiveType, AssemblyDirectiveType::AddressMode);
            temp.addProperty(TokenProperty::AddressMode, AddressMode::Bits26);
            directiveById["26BIT"] = temp;

            temp.addProperty(TokenProperty::AddressMode, AddressMode::Bits32);
            directiveById["32BIT"] = temp;
        }

        String key = makeKey(buffer, buffer.length());
        auto match = directiveById.find(key);

        if (match == directiveById.end())
        {
            // The directive was not recognised, return an error.
            String directiveText(buffer);
            String message = String::format("Unknown assembly directive '%{0}'.",
                                            { directiveText });

            result = Token(position, TokenClass::Error, message);
        }
        else
        {
            // Create a token from the template in the hash map.
            result = Token(position, match->second);
        }
    }

    return result;
}

Token interpretMnemonic(const Location &position, std::u32string &buffer)
{
    Token result;

    if (buffer.length() < 3)
    {
        char32_t first = buffer.empty() ? U'\0' : CodePoint::toUpper(buffer[0]);
        char32_t second = (buffer.size() < 2) ? U'\0' : CodePoint::toUpper(buffer[1]);

        if (first == U'B')
        {
            if (second == U'\0')
            {
                result.reset(position, TokenClass::MnemonicBranch);
                result.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::B);
            }
            else if (second == U'L')
            {
                result.reset(position, TokenClass::MnemonicBranch);
                result.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::Bl);
            }
            else if (second == U'X')
            {
                result.reset(position, TokenClass::MnemonicBranchExchange);
                result.addProperty(TokenProperty::Mnemonic, InstructionMnemonic::Bx);
            }
        }
    }
    else if (buffer.length() > 2)
    {
        // Somewhat harder now: use the first three characters as a key
        // into a hash map.
        static std::unordered_map<String, MnemonicMapping> mnemonicParsersById;

        if (mnemonicParsersById.empty())
        {
            mnemonicParsersById["ADD"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Add);
            mnemonicParsersById["SUB"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Sub);
            mnemonicParsersById["ADC"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Adc);
            mnemonicParsersById["SBC"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Sbc);
            mnemonicParsersById["RSB"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Rsb);
            mnemonicParsersById["RSC"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Rsc);
            mnemonicParsersById["AND"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::And);
            mnemonicParsersById["ORR"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Orr);
            mnemonicParsersById["EOR"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Eor);
            mnemonicParsersById["BIC"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Bic);
            mnemonicParsersById["MOV"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Mov);
            mnemonicParsersById["MVN"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Mvn);
            mnemonicParsersById["CMP"] = MnemonicMapping(parseAluCmpOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Cmp);
            mnemonicParsersById["CMN"] = MnemonicMapping(parseAluCmpOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Cmn);
            mnemonicParsersById["TST"] = MnemonicMapping(parseAluCmpOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Tst);
            mnemonicParsersById["TEQ"] = MnemonicMapping(parseAluCmpOp, TokenClass::MnemonicAluOp, InstructionMnemonic::Teq);
            mnemonicParsersById["MUL"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicMul3, InstructionMnemonic::Mul);
            mnemonicParsersById["MLA"] = MnemonicMapping(parseAluOp, TokenClass::MnemonicMul4, InstructionMnemonic::Mla);

            mnemonicParsersById["UMU"] = MnemonicMapping(parseLongMulOp, TokenClass::MnemonicMul4, InstructionMnemonic::Umull);
            mnemonicParsersById["UML"] = MnemonicMapping(parseLongMulOp, TokenClass::MnemonicMul4, InstructionMnemonic::Umlal);
            mnemonicParsersById["SMU"] = MnemonicMapping(parseLongMulOp, TokenClass::MnemonicMul4, InstructionMnemonic::Smull);
            mnemonicParsersById["SML"] = MnemonicMapping(parseLongMulOp, TokenClass::MnemonicMul4, InstructionMnemonic::Smlal);

            mnemonicParsersById["LDR"] = MnemonicMapping(parseDataTransfer, TokenClass::MnemonicSingleDataTransfer, InstructionMnemonic::Ldr);
            mnemonicParsersById["STR"] = MnemonicMapping(parseDataTransfer, TokenClass::MnemonicSingleDataTransfer, InstructionMnemonic::Str);
            mnemonicParsersById["LDM"] = MnemonicMapping(parseMultiDataTransfer, TokenClass::MnemonicMultiDataTransfer, InstructionMnemonic::Ldm);
            mnemonicParsersById["STM"] = MnemonicMapping(parseMultiDataTransfer, TokenClass::MnemonicMultiDataTransfer, InstructionMnemonic::Stm);

            mnemonicParsersById["MRS"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicMrs, InstructionMnemonic::Mrs);
            mnemonicParsersById["MSR"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicMsr, InstructionMnemonic::Msr);

            // Ambiguous combinations of B/BL instructions and condition codes.
            mnemonicParsersById["BEQ"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BNE"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BCS"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BHS"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BCC"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BLO"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BMI"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BPL"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BVS"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BVC"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BHI"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BLS"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BGE"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BLT"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BGT"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BLE"] = MnemonicMapping(parseAmbiguousBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BAL"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BNV"] = MnemonicMapping(parseConditionalBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);
            mnemonicParsersById["BLL"] = MnemonicMapping(parseAmbiguousBranch, TokenClass::MnemonicBranch, InstructionMnemonic::B);

            mnemonicParsersById["SWI"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicSwi, InstructionMnemonic::Swi);
            mnemonicParsersById["SWP"] = MnemonicMapping(parseSwapInstruction, TokenClass::MnemonicSwap, InstructionMnemonic::Swp);

            // Generic co-processor instructions.
            mnemonicParsersById["MCR"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicCoProcRegTransfer, InstructionMnemonic::Mcr);
            mnemonicParsersById["MRC"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicCoProcRegTransfer, InstructionMnemonic::Mrc);
            mnemonicParsersById["CDP"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicCoProcDataOp, InstructionMnemonic::Cdp);
            mnemonicParsersById["LDC"] = MnemonicMapping(parseGenericWithLongSuffix, TokenClass::MnemonicCoProcDataTransfer, InstructionMnemonic::Ldc);
            mnemonicParsersById["STC"] = MnemonicMapping(parseGenericWithLongSuffix, TokenClass::MnemonicCoProcDataTransfer, InstructionMnemonic::Stc);

            // FPA co-processor instructions.
            mnemonicParsersById["ADF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Adf);
            mnemonicParsersById["SUF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Suf);
            mnemonicParsersById["RSF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Rsf);
            mnemonicParsersById["MUF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Muf);
            mnemonicParsersById["DVF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Dvf);
            mnemonicParsersById["RDF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Rdf);
            mnemonicParsersById["FML"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Fml);
            mnemonicParsersById["FDV"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Fdv);
            mnemonicParsersById["FRD"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Frd);
            mnemonicParsersById["RMF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Rmf);
            mnemonicParsersById["POW"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Pow);
            mnemonicParsersById["RPW"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Rpw);
            mnemonicParsersById["POL"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaDyadicOp, InstructionMnemonic::Pol);

            mnemonicParsersById["ABS"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Abs);
            mnemonicParsersById["URD"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Urd);
            mnemonicParsersById["NRM"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Nrm);
            mnemonicParsersById["MVF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Mvf);
            mnemonicParsersById["MNF"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Mnf);
            mnemonicParsersById["SQT"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Sqt);
            mnemonicParsersById["RND"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Rnd);
            mnemonicParsersById["LOG"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Abs);
            mnemonicParsersById["LGN"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Urd);
            mnemonicParsersById["EXP"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Nrm);
            mnemonicParsersById["SIN"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Mvf);
            mnemonicParsersById["COS"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Mnf);
            mnemonicParsersById["TAN"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Sqt);
            mnemonicParsersById["ASN"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Rnd);
            mnemonicParsersById["ACS"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Mnf);
            mnemonicParsersById["ATN"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaMonadicOp, InstructionMnemonic::Sqt);

            mnemonicParsersById["CMF"] = MnemonicMapping(parseFpaCmpOp, TokenClass::MnemonicFpaComparisonOp, InstructionMnemonic::Cmf);
            mnemonicParsersById["CNF"] = MnemonicMapping(parseFpaCmpOp, TokenClass::MnemonicFpaComparisonOp, InstructionMnemonic::Cnf);

            mnemonicParsersById["FLT"] = MnemonicMapping(parseFpaDataOp, TokenClass::MnemonicFpaLoadRegOp, InstructionMnemonic::Flt);
            mnemonicParsersById["FIX"] = MnemonicMapping(parseFpaStoreRegisterMnemonic, TokenClass::MnemonicFpaStoreRegOp, InstructionMnemonic::Fix);
            mnemonicParsersById["WFS"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicFpaCoreRegOp, InstructionMnemonic::Wfs);
            mnemonicParsersById["RFS"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicFpaCoreRegOp, InstructionMnemonic::Rfs);
            mnemonicParsersById["WFC"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicFpaCoreRegOp, InstructionMnemonic::Wfc);
            mnemonicParsersById["RFC"] = MnemonicMapping(parseGenericInstruction, TokenClass::MnemonicFpaCoreRegOp, InstructionMnemonic::Rfc);

            mnemonicParsersById["LDF"] = MnemonicMapping(parseFpaDataTransfer, TokenClass::MnemonicFpaDataTransfer, InstructionMnemonic::Ldf);
            mnemonicParsersById["STF"] = MnemonicMapping(parseFpaDataTransfer, TokenClass::MnemonicFpaDataTransfer, InstructionMnemonic::Stf);
            mnemonicParsersById["LFM"] = MnemonicMapping(parseFpaMultiTransfer, TokenClass::MnemonicFpaMultiTransfer, InstructionMnemonic::Lfm);
            mnemonicParsersById["SFM"] = MnemonicMapping(parseFpaMultiTransfer, TokenClass::MnemonicFpaMultiTransfer, InstructionMnemonic::Sfm);

            mnemonicParsersById["ADR"] = MnemonicMapping(parseGenericWithLongSuffix, TokenClass::MnemonicAdr, InstructionMnemonic::Adr);

            mnemonicParsersById["EQU"] = MnemonicMapping(parseEquDataDirective, TokenClass::DataDirective);
            mnemonicParsersById["DCB"] = MnemonicMapping(parseDataDirective, DirectiveDataType::Byte);
            mnemonicParsersById["DCW"] = MnemonicMapping(parseDataDirective, DirectiveDataType::HalfWord);
            mnemonicParsersById["DCD"] = MnemonicMapping(parseDataDirective, DirectiveDataType::Word);
            mnemonicParsersById["DCS"] = MnemonicMapping(parseDataDirective, DirectiveDataType::NativeString);

            mnemonicParsersById["ALI"] = MnemonicMapping(parseAlign, TokenClass::AlignDirective, InstructionMnemonic::Align); // ALIGN
            mnemonicParsersById["BKP"] = MnemonicMapping(parseBreakPoint, TokenClass::MnemonicBreakPt, InstructionMnemonic::Bkpt);

            // Shifts
            mnemonicParsersById["LSL"] = MnemonicMapping(TokenClass::LogicShiftLeft);
            mnemonicParsersById["LSR"] = MnemonicMapping(TokenClass::LogicShiftRight);
            mnemonicParsersById["ASR"] = MnemonicMapping(TokenClass::ArithmeticShiftRight);
            mnemonicParsersById["ROR"] = MnemonicMapping(TokenClass::RotateRightShift);
            mnemonicParsersById["RRX"] = MnemonicMapping(TokenClass::RotateRightWithExtendShift);
        }

        String key = makeKey(buffer, 3);
        auto match = mnemonicParsersById.find(key);

        if (match != mnemonicParsersById.end())
        {
            const auto &mapping = match->second;
            MnemonicContext context(position, buffer, match->second);
            context.Index = 3;

            if (mapping.Interpret == nullptr)
            {
                // Use the default token classification.
                result.reset(position, mapping.Classifiation);
            }
            else
            {
                // Call function mapped to the mnemonic.
                result = match->second.Interpret(context);
            }

            // Ensure there are no characters after the end of the
            // recognised instruction mnemonic.
            context.verifyComplete(result);
        }
    }

    if (result.getClass() == TokenClass::Empty)
    {
        String mnemonic(buffer);
        String message = String::format("Unknown mnemonic '{0}' at the beginning of a statement.",
                                        { mnemonic });

        result = Token(position, TokenClass::Error, message);
    }

    return result;
}

//! @brief Processes characters after a statement continuation marker, '\'.
//! @param[in] input The input stream to extract further characters from.
//! @param[out] token The token to update with a warning if unexpected
//! characters are found before the end of the line.
//! @retval true A warning token was created due to unexpected characters.
//! @retval false The newline was encountered with no further token
//! reported.
bool processContinuationMarker(InputContext &input, Token &token)
{
    // We want to continue with this statement onto the next line
    // without returning a StatementTerminator when the new line
    // character is encountered.
    bool hasGarbage = false;
    char32_t next = U'\0';
    token.clear();

    while (input.tryGetNextCharacter(next) && (next != '\n'))
    {
        if ((hasGarbage == false) &&
            (CodePoint::isWhiteSpace(next) == false))
        {
            // We've found characters after the statement
            // continuation marker. Unless it's a comment
            // issue a warning.
            hasGarbage = true;

            if (next != U';')
            {
                token.reset(input.getCurrentLocation(),
                            TokenClass::Warning,
                            "Unexpected characters after statement continuation marker.");
            }
        }
    }

    // Unless there were garbage characters
    skipNoneNewlineWhiteSpace(input);
    return hasGarbage && (token.getClass() != TokenClass::Empty);
}

//! @brief Skips all further input characters until the end of the statement.
//! @param[in] input The input stream to process.
void recover(InputContext &input)
{
    char32_t next;

    while (input.tryGetNextCharacter(next))
    {
        if ((next == U':') || (next == '\n') || (next == U';'))
        {
            input.ungetCharacter();
            input.breakInputStream();
            break;
        }
    }
}

//! @brief Creates an error token based on an unexpected character.
//! @param[in] location The location of the character in source code.
//! @param[in] ch The offending character.
//! @returns An error token with an appropriate message.
Token makeUnexpectedInputToken(const Location &location, char32_t ch)
{
    String message = String::format("Unexpected character '{0}' in the input stream.",
                                    { ch });

    return Token(location, TokenClass::Error, message);
}

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which processes characters into tokens.
class StatementLexer : public ILexicalContext
{
private:
    // Internal Functions

    // Construction/Destruction
    StatementLexer() = default;
public:
    virtual ~StatementLexer() = default;

    //! @brief Gets a singleton instance of the lexical analyser.
    static StatementLexer *get()
    {
        static StatementLexer instance;

        return &instance;
    }

    // Overrides
    // Inherited from ILexicalContext.
    virtual bool tryGetNextToken(InputContext &input, Token &token) const override
    {
        bool hasToken = false;
        char32_t next = 0;
        token.clear();

        skipNoneNewlineWhiteSpace(input);

        while ((hasToken == false) && input.tryGetNextCharacter(next))
        {
            // Copy the location of the start of the token.
            Location position = input.getCurrentLocation();
            hasToken = true;

            static LinearSortedMap<char32_t, TokenClass> tokenIdByCharacter;

            if (tokenIdByCharacter.isEmpty())
            {
                LinearSortedMapIndexer indexer(tokenIdByCharacter);
                tokenIdByCharacter.push_back(U'\n', TokenClass::StatementTerminator);
                tokenIdByCharacter.push_back(U':', TokenClass::StatementTerminator);
                tokenIdByCharacter.push_back(U'!', TokenClass::Bang);
                tokenIdByCharacter.push_back(U'{', TokenClass::OpenBrace);
                tokenIdByCharacter.push_back(U'}', TokenClass::CloseBrace);
                tokenIdByCharacter.push_back(U'[', TokenClass::OpenSquare);
                tokenIdByCharacter.push_back(U']', TokenClass::CloseSquare);
                tokenIdByCharacter.push_back(U',', TokenClass::Comma);
                tokenIdByCharacter.push_back(U'#', TokenClass::Hash);
                tokenIdByCharacter.push_back(U'^', TokenClass::Hat);
            }

            TokenClass tokenId;

            if (tokenIdByCharacter.tryFind(next, tokenId))
            {
                token.reset(position, tokenId);
            }
            else if (next == U'\\')
            {
                hasToken = processContinuationMarker(input, token);
            }
            else if (next == U';') // Line comments begin with ';'
            {
                while (input.tryGetNextCharacter(next))
                {
                    if (next == U'\n')
                    {
                        token = Token(input.getCurrentLocation(),
                                      TokenClass::StatementTerminator);
                        break;
                    }
                }

                hasToken = (token.getClass() != TokenClass::Empty);
            }
            else if (next == U'%') // Directives start with '%'.
            {
                std::u32string buffer;
                buffer.reserve(32);

                accumulateSymbol(input, buffer);

                token = interpretDirective(position, buffer);
            }
            else if (next == U'.') // Labels start with '.'.
            {
                std::u32string buffer;
                buffer.reserve(32);

                accumulateSymbol(input, buffer);

                token = interpretLabelDefinition(position, buffer);
            }
            else if (CodePoint::isLetter(next))
            {
                // It could be an instruction mnemonic or keyword of
                // some kind?
                std::u32string buffer;
                buffer.reserve(32);

                buffer.push_back(next);
                accumulateSymbol(input, buffer);

                token = interpretMnemonic(position, buffer);
            }
            else
            {
                // Accumulate characters until the end of the statement or input
                // an return an error token.
                std::u32string buffer;
                buffer.reserve(32);

                buffer.push_back(next);

                while (input.tryGetNextCharacter(next))
                {
                    if ((next == U'\n') || (next == U':') || (next == U';'))
                    {
                        input.ungetCharacter();
                        break;
                    }
                    else
                    {
                        buffer.push_back(next);
                    }
                }

                String message = String::format("Unknown statement text '{0}'.",
                                                { String(buffer) });

                token.reset(position, TokenClass::Error, message);
            }
        }

        if (hasToken)
        {
            // Having recognised the token, we aren't going back.
            input.breakInputStream();
        }

        return hasToken;
    }

    // Inherited from ILexicalContext.
    virtual bool tryCreateNodeFromStartToken(ParseContext &context,
                                             const Token &token,
                                             ISyntaxNode *&node) const override
    {
        node = nullptr;

        switch (token.getClass())
        {
        case TokenClass::DataDirective:
            node = new DataDirectiveNode(context,
                                         token.getProperty(TokenProperty::DataType,
                                                           DirectiveDataType::Byte),
                                         token.getLocation());
            break;

        case TokenClass::AssemblyDirective: {
            AssemblyDirectiveType directiveType;

            if (token.tryGetProperty(TokenProperty::DirectiveType, directiveType))
            {
                switch (directiveType)
                {
                case AssemblyDirectiveType::Include:
                    node = new IncludeNode(context, token.getLocation());
                    break;

                case AssemblyDirectiveType::InstructionSet:
                case AssemblyDirectiveType::ProcessorExtension:
                case AssemblyDirectiveType::ProcessorMode:
                case AssemblyDirectiveType::AddressMode:
                    node = new AssemblyDirectiveNode(context, token);
                    break;
                }
            }
        } break;

        case TokenClass::AlignDirective:
            node = new AlignDirectiveNode(context, token);
            break;

        case TokenClass::MnemonicAluOp:
            node = new AluInstructionNode(context, token);
            break;

        case TokenClass::MnemonicAdr:
            node = new AddressDirectiveNode(context, token);
            break;

        case TokenClass::MnemonicMul3:
        case TokenClass::MnemonicMul4:
            node = new MultiplyInstructionNode(context, token);
            break;

        case TokenClass::MnemonicBranch:
            node = new BranchInstructionNode(context, token);
            break;

        case TokenClass::MnemonicSwi:
            node = new SwiInstructionNode(context, token);
            break;

        case TokenClass::MnemonicMrs:
            node = new MoveFromStatusRegNode(context, token);
            break;

        case TokenClass::MnemonicMsr:
            node = new MoveToStatusRegNode(context, token);
            break;

        case TokenClass::MnemonicBreakPt:
            node = new BkptInstructionNode(context, token);
            break;

        case TokenClass::MnemonicSingleDataTransfer:
            node = new DataTransferInstructionNode(context, token);
            break;

        case TokenClass::MnemonicMultiDataTransfer:
            node = new MultiTransferInstructionNode(context, token);
            break;

        case TokenClass::MnemonicSwap:
            node = new AtomicSwapInstructionNode(context, token);
            break;

        case TokenClass::Label:
            node = new LabelNode(context, token);
            break;

        case TokenClass::StatementTerminator:
            node = new EmptyStatement(context, token.getLocation());
            break;

        case TokenClass::MnemonicCoProcDataOp:
        case TokenClass::MnemonicCoProcRegTransfer:
            // The CDP, MCR and MRC instructions all have pretty much the
            // same format, so a single syntax node type is used for all.
            node = new CoProcDataProcInstructionNode(context, token);
            break;

        case TokenClass::MnemonicCoProcDataTransfer:
            node = new CoProcDataTransferInstructionNode(context, token);
            break;

        default:
            break;
        }

        return node != nullptr;
    }
};

//! @brief Recognises tokens from expressions.
class ExpressionLexer : public ILexicalContext
{
private:
    //! @brief Classifies a sequence of alphanumeric characters.
    //! @param[in] position The source code position of the first character.
    //! @param[in] text The characters of the sequence.
    //! @return A token created from the specified characters.
    static Token parseSymbol(const Location &position, std::u32string &text)
    {
        String key = makeKey(text, text.length());

        static std::unordered_map<String, TokenClass> keywordClassesById;

        if (keywordClassesById.empty())
        {
            keywordClassesById["NOT"] = TokenClass::KeywordNot;
            keywordClassesById["AND"] = TokenClass::KeywordAnd;
            keywordClassesById["OR"] = TokenClass::KeywordOr;
            keywordClassesById["XOR"] = TokenClass::KeywordXor;
            keywordClassesById["EOR"] = TokenClass::KeywordXor;
            keywordClassesById["MOD"] = TokenClass::KeywordMod;
        }

        auto pos = keywordClassesById.find(key);
        Token token;

        if (pos == keywordClassesById.end())
        {
            token.reset(position, TokenClass::Symbol, String(text));
        }
        else
        {
            token.reset(position, pos->second);
        }

        return token;
    }

    //! @brief Attempts to parse the characters of a binary literal following a
    //! '%' character.
    //! @param[in] position The location of the first character of the literal
    //! in source code.
    //! @param[in] input The input stream to read the digits from.
    //! @returns A token representing a binary literal or an error.
    static Token parseBinaryLiteral(const Location &position, InputContext &input)
    {
        Token token;
        char32_t next;

        // Ensure we have at least one binary digit.
        if (input.tryGetNextCharacter(next))
        {
            std::string buffer;
            buffer.reserve(64);

            if ((next == U'0') || (next == U'1'))
            {
                // Consume all subsequent binary digits.
                buffer.push_back(static_cast<char>(next));

                while (input.tryGetNextCharacter(next))
                {
                    if (next == U'0')
                    {
                        buffer.push_back(U'0');
                    }
                    else if (next == U'1')
                    {
                        buffer.push_back(U'1');
                    }
                    else
                    {
                        input.ungetCharacter();
                        break;
                    }
                }

                // Construct a token from the digits.
                token.reset(position, TokenClass::IntegerLiteral, buffer);
                token.addProperty(TokenProperty::IntRadix, 2);
            }
            else
            {
                // Construct an error.
                token.reset(position, TokenClass::Error, "Empty binary literal.");

                input.ungetCharacter();
                recover(input);
            }
        }
        else
        {
            // Construct an error.
            token.reset(position, TokenClass::Error, "Empty binary literal.");
        }

        return token;
    }

    //! @brief Attempts to parse the characters of a hexadecimal literal following a
    //! '&' character.
    //! @param[in] position The location of the first character of the literal
    //! in source code.
    //! @param[in] input The input stream to read the digits from.
    //! @returns A token representing a hexadecimal literal or an error.
    static Token parseHexLiteral(const Location &position, InputContext &input)
    {
        Token token;
        char32_t next;

        // Ensure we have at least one hex digit.
        if (input.tryGetNextCharacter(next))
        {
            if (CodePoint::isHexDigit(next))
            {
                // Consume all subsequent hex digits.
                std::u32string buffer;
                buffer.reserve(16);

                buffer.push_back(next);

                while (input.tryGetNextCharacter(next))
                {
                    if (CodePoint::isHexDigit(next))
                    {
                        buffer.push_back(next);
                    }
                    else
                    {
                        input.ungetCharacter();
                        break;
                    }
                }

                // Construct a token.
                token.reset(position, TokenClass::IntegerLiteral, buffer);
                token.addProperty(TokenProperty::IntRadix, 16);
            }
            else
            {
                // Construct an error.
                input.ungetCharacter();
                recover(input);

                token.reset(position, TokenClass::Error, "Empty hexadecimal literal.");
            }
        }
        else
        {
            // Construct an error.
            token.reset(position, TokenClass::Error, "Empty hexadecimal literal.");
        }

        return token;
    }

    //! @brief Attempts to parse the characters of a numeric literal following a
    //! digit character.
    //! @param[in] position The location of the first character of the literal
    //! in source code.
    //! @param[in] input The input stream to read the digits from.
    //! @returns A token representing a numeric literal or an error.
    static Token parseDecimalLiteral(const Location &position,
                                     InputContext &input, char32_t first)
    {
        std::string buffer;
        char32_t next;
        buffer.reserve(32);

        // We already have at least one decimal digit.
        buffer.push_back(static_cast<char>(first));

        TokenClass tokenClass = TokenClass::IntegerLiteral;
        bool hasDecimalPoint = false;
        bool hasExponent = false;

        // Consume any further decimal digits.
        while (input.tryGetNextCharacter(next))
        {
            if (CodePoint::isNumeric(next))
            {
                buffer.push_back(static_cast<char>(next));
            }
            else if (next == U'.')
            {
                tokenClass = TokenClass::RealLiteral;
                buffer.push_back('.');
                hasDecimalPoint = true;
                break;
            }
            else if ((next == U'E') || (next == U'e'))
            {
                // Skip the fraction and go straight on to the exponent.
                tokenClass = TokenClass::RealLiteral;
                buffer.push_back(static_cast<char>(next));
                hasExponent = true;
                break;
            }
            else
            {
                input.ungetCharacter();
                break;
            }
        }

        if (tokenClass == TokenClass::RealLiteral)
        {
            bool isValid = true;

            if (hasDecimalPoint)
            {
                // Ensure there is at least one fraction digit.
                if (input.tryGetNextCharacter(next))
                {
                    if (CodePoint::isNumeric(next))
                    {
                        // Consume all further fraction digits.
                        buffer.push_back(static_cast<char>(next));

                        while (input.tryGetNextCharacter(next))
                        {
                            if (CodePoint::isNumeric(next))
                            {
                                buffer.push_back(static_cast<char>(next));
                            }
                            else if ((next == U'E') || (next == U'e'))
                            {
                                buffer.push_back(static_cast<char>(next));
                                hasExponent = true;
                                break;
                            }
                            else
                            {
                                input.ungetCharacter();
                                break;
                            }
                        }
                    }
                    else
                    {
                        // No digits after the decimal point.
                        input.ungetCharacter();
                        isValid = false;
                    }
                }
                else
                {
                    // No digits after the decimal point.
                    isValid = false;
                }
            }

            if (isValid && hasExponent)
            {
                if (input.tryGetNextCharacter(next))
                {
                    if ((next == U'-') || (next == U'+'))
                    {
                        // Its the exponent sign.
                        buffer.push_back(static_cast<char>(next));
                    }
                    else
                    {
                        // If it's a digit or anything else, deal with it next.
                        input.ungetCharacter();
                    }
                }

                // Ensure we have at least one exponent digit.
                if (input.tryGetNextCharacter(next))
                {
                    if (CodePoint::isNumeric(next))
                    {
                        // Consume all further exponent digits.
                        buffer.push_back(static_cast<char>(next));

                        while (input.tryGetNextCharacter(next))
                        {
                            if (CodePoint::isNumeric(next))
                            {
                                buffer.push_back(static_cast<char>(next));
                            }
                            else
                            {
                                input.ungetCharacter();
                                break;
                            }
                        }
                    }
                    else
                    {
                        // No exponent digits.
                        input.ungetCharacter();
                        isValid = false;
                    }
                }
                else
                {
                    // No exponent digits.
                    isValid = false;
                }
            }

            if (isValid == false)
            {
                // Create an error message in the buffer.
                String digits(buffer);
                buffer.clear();

                FormatInfo format(LocaleInfo::getDisplay());
                appendFormat(format, "Invalid floating point literal '{0}'.",
                             buffer, { digits });

                tokenClass = TokenClass::Error;
                recover(input);
            }
        }

        Token literal(position, tokenClass, buffer);

        if (tokenClass == TokenClass::IntegerLiteral)
        {
            literal.addProperty(TokenProperty::IntRadix, 10);
        }

        return literal;
    }

    //! @brief Attempts to parse the characters of a numeric literal following a
    //! digit character.
    //! @param[in] position The location of the first character of the literal
    //! in source code.
    //! @param[in] input The input stream to read the digits from.
    //! @returns A token representing a numeric literal or an error.
    static Token parseNumericLiteral(const Location &position,
                                     InputContext &input, char32_t first)
    {
        if (first == U'0')
        {
            char32_t second;

            if (input.tryGetNextCharacter(second))
            {
                char32_t key = CodePoint::toUpper(second);

                if (CodePoint::isNumeric(second) || (second == '.') ||
                    (second == U'E') || (second == U'e'))
                {
                    input.ungetCharacter();

                    return parseDecimalLiteral(position, input, first);
                }
                else if (key == U'X')
                {
                    // It's the prefix for a hex literal.
                    return parseHexLiteral(position, input);
                }
                else if (key == U'B')
                {
                    // It's the prefix for a binary literal.
                    return parseBinaryLiteral(position, input);
                }
                else
                {
                    // The next character wasn't a digit.
                    input.ungetCharacter();

                    Token zero(position, TokenClass::IntegerLiteral, "0");
                    zero.addProperty(TokenProperty::IntRadix, 10);

                    return zero;
                }
            }
            else
            {
                // The zero was at the end of the input stream.
                Token zero(position, TokenClass::IntegerLiteral, "0");
                zero.addProperty(TokenProperty::IntRadix, 10);

                return zero;
            }
        }
        else
        {
            return parseDecimalLiteral(position, input, first);
        }
    }

    //! @brief Consume input characters from within a string literal until
    //! the end of the line or string.
    //! @param[in] input The input stream.
    //! @param[in] closingQuote The character expected at the end of the
    //! string literal.
    static void recoverFromStringLiteral(InputContext &input,
                                         char32_t closingQuote)
    {
        char32_t next;

        while (input.tryGetNextCharacter(next))
        {
            if (next == closingQuote)
            {
                break;
            }
            else if (next == U'\n')
            {
                input.ungetCharacter();
                break;
            }
        }
    }

    //! @brief Attempts to consume hex digits from the input stream
    //! representing a Unicode character code.
    //! @param[in] input The input stream to consume characters from.
    //! @param[in] sequence The buffer to add the processed characters to.
    //! @param[in] digitCount The count of expected hex digits to read.
    //! @param[in] closingQuote The character marking the end of the
    //! string literal.
    //! @param[out] result Receives the character code parsed from the
    //! hex digits.
    //! @retval true The character code was successfully parsed.
    //! @retval false The escape sequence was invalid.
    static bool tryConsumeHexEscapeSequence(InputContext &input,
                                            std::u32string &sequence,
                                            size_t digitCount,
                                            char32_t closingQuote,
                                            char32_t &result)
    {
        size_t sequenceStart = sequence.size();
        result = U'\0';

        for (size_t digitIndex = 0; digitIndex < digitCount; ++digitIndex)
        {
            char32_t next;

            if (input.tryGetNextCharacter(next))
            {
                if (CodePoint::isHexDigit(next))
                {
                    sequence.push_back(next);
                }
                else
                {
                    input.ungetCharacter();
                    recoverFromStringLiteral(input, closingQuote);

                    break;
                }
            }
            else
            {
                // Run out of characters, no need to recover.
                break;
            }
        }

        if ((sequence.size() - sequenceStart) == digitCount)
        {
            String digits(sequence.c_str() + sequenceStart, digitCount);

            uint32_t codePoint;

            if (digits.tryParseScalar(codePoint, 16))
            {
                result = static_cast<char32_t>(codePoint);

                return true;
            }
        }

        return false;
    }
    
    //! @brief Attempts to parse a quoted string literal.
    //! @param[in] position The position of the opening quote.
    //! @param[in] input The input stream to read characters from.
    //! @param[in] openingQuote The character which is used to delimit the
    //! string literal.
    //! @returns A token representing the literal or an error.
    static Token parseStringLiteral(const Location &position,
                                    InputContext &input, char32_t openingQuote)
    {
        std::u32string buffer;
        buffer.reserve(32);

        std::u32string escapeSequence;
        escapeSequence.reserve(16);

        bool isEscaped = false;
        bool isClosed = false;

        Location currentEscape = position;

        char32_t next;

        while (input.tryGetNextCharacter(next))
        {
            if (isEscaped)
            {
                static LinearSortedMap<char32_t, char32_t> singleCharEscape;

                if (singleCharEscape.isEmpty())
                {
                    LinearSortedMapIndexer reindex(singleCharEscape);
                    singleCharEscape.push_back(U'\\', U'\\');
                    singleCharEscape.push_back(U'0', U'\0');
                    singleCharEscape.push_back(U't', U'\t');
                    singleCharEscape.push_back(U'b', U'\b');
                    singleCharEscape.push_back(U'r', U'\r');
                    singleCharEscape.push_back(U'n', U'\n');
                }

                char32_t escaped;
                bool isValid = true;

                if (singleCharEscape.tryFind(next, escaped))
                {
                    buffer.push_back(escaped);
                    isEscaped = false;
                }
                else
                {
                    // It could be some kind of character code escape sequence.
                    escapeSequence.clear();
                    escapeSequence.push_back(U'\\');
                    escapeSequence.push_back(next);

                    size_t digitCount;

                    switch (next)
                    {
                    case U'x':
                    case U'X':
                        digitCount = 2;
                        break;

                    case U'u':
                        digitCount = 4;
                        break;

                    case U'U':
                        digitCount = 8;
                        break;

                    default:
                        digitCount = 0;
                        break;
                    }

                    if ((digitCount > 0) &&
                        tryConsumeHexEscapeSequence(input, escapeSequence,
                                                    digitCount, openingQuote,
                                                    escaped))
                    {
                        buffer.push_back(escaped);
                        isEscaped = false;
                    }
                    else
                    {
                        isValid = false;
                    }
                }

                if (isValid == false)
                {
                    String message =
                        String::format("Invalid escape sequence '{0}'.",
                                       { String(escapeSequence) });

                    Token error(currentEscape, TokenClass::Error, message);
                    recoverFromStringLiteral(input, openingQuote);

                    return error;
                }
            }
            else if (next == U'\\')
            {
                isEscaped = true;
                currentEscape = input.getCurrentLocation();
                escapeSequence.clear();
                escapeSequence.push_back(next);
            }
            else if (next == openingQuote)
            {
                isClosed = true;
                break;
            }
            else if (next == U'\n')
            {
                input.ungetCharacter();
                break;
            }
            else
            {
                buffer.push_back(next);
            }
        }

        Token stringLiteral(position, TokenClass::StringLiteral, buffer);

        if (isClosed == false)
        {
            stringLiteral.addProperty(TokenProperty::UnterminatedString, true);
        }

        return stringLiteral;
    }

    ExpressionLexer() = default;
public:
    static ExpressionLexer *get()
    {
        static ExpressionLexer instance;

        return &instance;
    }

    virtual ~ExpressionLexer() = default;

    // Inherited from ILexicalContext.
    virtual bool tryGetNextToken(InputContext &input, Token &token) const override
    {
        bool hasToken = false;
        char32_t next = 0;
        token.clear();

        skipNoneNewlineWhiteSpace(input);

        while ((hasToken == false) && input.tryGetNextCharacter(next))
        {
            // Save the position of the first character in the token.
            Location position = input.getCurrentLocation();
            hasToken = true;

            static LinearSortedMap<char32_t, TokenClass> tokenClassesByCharacter;

            if (tokenClassesByCharacter.isEmpty())
            {
                LinearSortedMapIndexer reindex(tokenClassesByCharacter);
                tokenClassesByCharacter.push_back(U'\n', TokenClass::StatementTerminator);
                tokenClassesByCharacter.push_back(U':', TokenClass::StatementTerminator);
                tokenClassesByCharacter.push_back(U'+', TokenClass::Plus);
                tokenClassesByCharacter.push_back(U'-', TokenClass::Minus);
                tokenClassesByCharacter.push_back(U',', TokenClass::Comma);
                tokenClassesByCharacter.push_back(U'!', TokenClass::Bang);
                tokenClassesByCharacter.push_back(U'$', TokenClass::Dollar);
                tokenClassesByCharacter.push_back(U'*', TokenClass::Asterisk);
                tokenClassesByCharacter.push_back(U'/', TokenClass::Slash);
                tokenClassesByCharacter.push_back(U'(', TokenClass::OpenParenthesis);
                tokenClassesByCharacter.push_back(U')', TokenClass::CloseParenthesis);
                tokenClassesByCharacter.push_back(U'[', TokenClass::OpenSquare);
                tokenClassesByCharacter.push_back(U']', TokenClass::CloseSquare);
                tokenClassesByCharacter.push_back(U'{', TokenClass::OpenBrace);
                tokenClassesByCharacter.push_back(U'}', TokenClass::CloseBrace);
                tokenClassesByCharacter.push_back(U'#', TokenClass::Hash);
                tokenClassesByCharacter.push_back(U'^', TokenClass::Hat);
            }

            TokenClass tokenClass;

            if (tokenClassesByCharacter.tryFind(next, tokenClass))
            {
                token.reset(position, tokenClass);
            }
            else if (next == U'\\')
            {
                hasToken = processContinuationMarker(input, token);
            }
            else if ((next == U'"') || (next == U'\''))
            {
                token = parseStringLiteral(position, input, next);
            }
            else if (next == U';')
            {
                // It's a line comment.
                while (input.tryGetNextCharacter(next))
                {
                    if (next == U'\n')
                    {
                        token.reset(input.getCurrentLocation(),
                                    TokenClass::StatementTerminator);
                        break;
                    }
                }

                hasToken = (token.getClass() != TokenClass::Empty);
            }
            else if (next == U'%')
            {
                // A binary literal.
                token = parseBinaryLiteral(position, input);
            }
            else if (next == U'&')
            {
                // A hexadecimal literal.
                token = parseHexLiteral(position, input);
            }
            else if (next == U'<')
            {
                // It could be a left shift.
                if (input.ensureNextCharacter(U'<'))
                {
                    token.reset(position, TokenClass::LogicShiftLeft);
                }
                else
                {
                    token = makeUnexpectedInputToken(position, next);

                    recover(input);
                }
            }
            else if (next == U'>')
            {
                // It could be a right shift.
                if (input.ensureNextCharacter(U'>'))
                {
                    if (input.ensureNextCharacter(U'>'))
                    {
                        // Its a right shift with sign extension.
                        token.reset(position, TokenClass::ArithmeticShiftRight);
                    }
                    else
                    {
                        // Its an unsigned right shift.
                        token.reset(position, TokenClass::LogicShiftRight);
                    }
                }
                else
                {
                    // The character is at the end of the input stream.
                    token = makeUnexpectedInputToken(position, next);

                    recover(input);
                }
            }
            else if (CodePoint::isNumeric(next))
            {
                // A numeric literal.
                token = parseNumericLiteral(position, input, next);
            }
            else if ((next == U'_') || CodePoint::isLetter(next))
            {
                // A symbol or keyword.
                std::u32string buffer;
                buffer.reserve(32);

                buffer.push_back(next);
                accumulateSymbol(input, buffer);

                token = parseSymbol(position, buffer);
            }
            else
            {
                // An error.
                token = makeUnexpectedInputToken(position, next);

                recover(input);
            }

            input.breakInputStream();
        }

        return hasToken;
    }

    // Inherited from ILexicalContext.
    virtual bool tryCreateNodeFromStartToken(ParseContext & /* context */,
                                             const Token &token,
                                             ISyntaxNode *&node) const
    {
        node = nullptr;

        switch (token.getClass())
        {
        case TokenClass::IntegerLiteral:
            node = new IntegerLiteralNode(token.getLocation(), token.getValue(),
                                          token.getProperty(TokenProperty::IntRadix, 10));
            break;

        case TokenClass::RealLiteral:
            node = new RealLiteralNode(token.getLocation(), token.getValue());
            break;

        case TokenClass::StringLiteral:
            node = new StringLiteralNode(token.getLocation(), token.getValue(),
                                         token.getProperty(TokenProperty::UnterminatedString, false));
            break;

        case TokenClass::Dollar:
            node = new CurrentAddressNode(token.getLocation());
            break;

        case TokenClass::Symbol:
            node = new SymbolNode(token.getLocation(), token.getValue());
            break;

        case TokenClass::OpenParenthesis:
            node = new ParenthesisNode(token.getLocation());
            break;

        case TokenClass::Minus:
            node = new UnaryOperatorNode(token.getLocation(),
                                         UnaryOperatorType::Minus);
            break;

        case TokenClass::Plus:
            node = new UnaryOperatorNode(token.getLocation(),
                                         UnaryOperatorType::Absolute);
            break;

        case TokenClass::KeywordNot:
            node = new UnaryOperatorNode(token.getLocation(),
                                         UnaryOperatorType::LogicalNot);
            break;

        default:
            break;
        }

        return node != nullptr;
    }
};

class CoreRegListLexer : public ILexicalContext
{
private:
    CoreRegListLexer() = default;
public:
    virtual ~CoreRegListLexer() = default;
    static ILexicalContext *get()
    {
        static CoreRegListLexer instance;

        return &instance;
    }

    // Inherited from ILexicalContext.
    virtual bool tryGetNextToken(InputContext &input, Token &token) const override
    {
        bool hasToken = false;
        char32_t next = 0;
        token.clear();

        skipNoneNewlineWhiteSpace(input);

        while ((hasToken == false) && input.tryGetNextCharacter(next))
        {
            // Save the position of the first character in the token.
            Location position = input.getCurrentLocation();
            hasToken = true;

            static LinearSortedMap<char32_t, TokenClass> tokenClassesByCharacter;

            if (tokenClassesByCharacter.isEmpty())
            {
                LinearSortedMapIndexer reindex(tokenClassesByCharacter);
                tokenClassesByCharacter.push_back(U'\n', TokenClass::StatementTerminator);
                tokenClassesByCharacter.push_back(U':', TokenClass::StatementTerminator);
                tokenClassesByCharacter.push_back(U'-', TokenClass::Minus);
                tokenClassesByCharacter.push_back(U',', TokenClass::Comma);
                tokenClassesByCharacter.push_back(U'{', TokenClass::OpenBrace);
                tokenClassesByCharacter.push_back(U'}', TokenClass::CloseBrace);
            }

            TokenClass tokenClass;

            if (tokenClassesByCharacter.tryFind(next, tokenClass))
            {
                token.reset(position, tokenClass);
            }
            else if (next == U'\\')
            {
                hasToken = processContinuationMarker(input, token);
            }
            else if (next == U';')
            {
                // It's a line comment.
                while (input.tryGetNextCharacter(next))
                {
                    if (next == U'\n')
                    {
                        token.reset(input.getCurrentLocation(),
                                    TokenClass::StatementTerminator);
                        break;
                    }
                }

                hasToken = (token.getClass() != TokenClass::Empty);
            }
            else if (CodePoint::isLetter(next))
            {
                // A symbol or keyword.
                std::u32string buffer;
                buffer.reserve(32);

                buffer.push_back(next);
                accumulateAlphaNumeric(input, buffer);

                token.reset(position, TokenClass::Symbol, String(buffer));
            }
            else
            {
                // An error.
                token = makeUnexpectedInputToken(position, next);

                recover(input);
            }

            input.breakInputStream();
        }

        return hasToken;
    }

    // Inherited from ILexicalContext.
    virtual bool tryCreateNodeFromStartToken(ParseContext &/*context*/,
                                             const Token &/*token*/,
                                             ISyntaxNode *&node) const override
    {
        node = nullptr;

        return false;
    }
};

class PsrNameLexer : public ILexicalContext
{
private:
    // Internal Fields
    bool _allowComponents;

    // Internal Functions
    bool tryParseSuffix(const std::u32string &source, uint8_t &components) const
    {
        bool isSuffixValid = true;
        components = toScalar(PsrComponent::All);

        if ((source.length() > 4) && (source[4] == U'_'))
        {
            components = 0;
            isSuffixValid = false;

            if (source.length() == 8)
            {
                static const std::u32string_view allSuffix(U"ALL");
                static const std::u32string_view flgSuffix(U"FLG");
                static const std::u32string_view ctlSuffix(U"CTL");

                std::u32string_view suffix(source.c_str() + 5, 3);
                isSuffixValid = true;

                if (suffix == allSuffix)
                {
                    components = toScalar(PsrComponent::All);
                }
                else if (suffix == flgSuffix)
                {
                    components = toScalar(PsrComponent::Flags);
                }
                else if (suffix == ctlSuffix)
                {
                    components = toScalar(PsrComponent::Control);
                }
                else
                {
                    isSuffixValid = false;
                }
            }

            if ((isSuffixValid == false) && (source.length() > 5))
            {
                // Process the suffix characters individually.
                isSuffixValid = true;

                for (size_t i = 5, c = source.length(); isSuffixValid && (i < c); ++i)
                {
                    PsrComponent component = PsrComponent::None;

                    switch (source[i])
                    {
                    case U'C': component = PsrComponent::Control; break;
                    case U'X': component = PsrComponent::Extension; break;
                    case U'S': component = PsrComponent::Status; break;
                    case U'F': component = PsrComponent::Flags; break;
                    default:
                        isSuffixValid = false;
                        break;
                    }

                    uint8_t rawComponent = toScalar(component);

                    if (components & rawComponent)
                    {
                        // A component has been specified more than once.
                        isSuffixValid = false;
                    }
                    else
                    {
                        // Apply the component.
                        components |= rawComponent;
                    }
                }
            }
        }

        return isSuffixValid;
    }
public:
    PsrNameLexer(bool allowComponents) :
        _allowComponents(allowComponents)
    {
    }

    virtual ~PsrNameLexer() = default;

    // Overrides
    // Inherited from ILexicalContext.
    virtual bool tryGetNextToken(InputContext &input, Token &token) const override
    {
        bool hasToken = false;
        char32_t next = 0;
        token.clear();

        skipNoneNewlineWhiteSpace(input);

        while ((hasToken == false) && input.tryGetNextCharacter(next))
        {
            // Save the position of the first character in the token.
            Location position = input.getCurrentLocation();
            hasToken = true;

            if (next == U'\\')
            {
                hasToken = processContinuationMarker(input, token);
            }
            else if (next == U';')
            {
                // It's a line comment.
                while (input.tryGetNextCharacter(next))
                {
                    if (next == U'\n')
                    {
                        token.reset(input.getCurrentLocation(),
                                    TokenClass::StatementTerminator);
                        break;
                    }
                }

                hasToken = (token.getClass() != TokenClass::Empty);
            }
            else if (CodePoint::isLetter(next))
            {
                // A symbol or keyword.
                std::u32string buffer;
                buffer.reserve(32);
                buffer.push_back(next);
                accumulateSymbol(input, buffer);

                hasToken = false;

                if (buffer.length() >= 4)
                {
                    // Create an upper-case version of the symbol.
                    static const std::u32string_view cpsr(U"CPSR");
                    static const std::u32string_view spsr(U"SPSR");

                    std::u32string key;
                    key.reserve(buffer.size());

                    std::transform(buffer.begin(), buffer.end(),
                                   std::back_inserter(key), CodePoint::toUpper);

                    if (startsWith(key, cpsr))
                    {
                        token.reset(position, TokenClass::RegisterStatus);
                        token.addProperty(TokenProperty::RegisterIndex,
                                          CoreRegister::CPSR);
                        hasToken = true;
                    }
                    else if (startsWith(key, spsr))
                    {
                        token.reset(position, TokenClass::RegisterStatus);
                        token.addProperty(TokenProperty::RegisterIndex,
                                          CoreRegister::SPSR);
                        hasToken = true;
                    }

                    uint8_t components;

                    if (tryParseSuffix(key, components))
                    {
                        if (_allowComponents)
                        {
                            // Apply the PSR component detail to the token.
                            token.addScalarProperty(TokenProperty::PsrComponent, components);
                        }
                        else if (components != toScalar(PsrComponent::All))
                        {
                            token.reset(position, TokenClass::Error,
                                        "The status register cannot specify a sub-component.");
                        }
                    }
                    else
                    {
                        String value(buffer);
                        String message = String::format("Status register specification '{0}' has an invalid suffix.",
                                                        { value });

                        token.reset(position, TokenClass::Error, message);
                        hasToken = true;
                    }
                }

                if (hasToken == false)
                {
                    String value(buffer);
                    String message = String::format("Token '{0}' is not a valid status register specification.",
                                                    { value });

                    token.reset(position, TokenClass::Error, message);
                    hasToken = true;
                }
            }
            else
            {
                // An error.
                token = makeUnexpectedInputToken(position, next);

                recover(input);
            }

            input.breakInputStream();
        }

        return hasToken;
    }

    // Inherited from ILexicalContext.
    virtual bool tryCreateNodeFromStartToken(ParseContext &/*context*/,
                                             const Token &/*token*/,
                                             ISyntaxNode *&node) const override
    {
        // Psr name tokens aren't the root of larger syntax trees.
        node = nullptr;

        return false;
    }
};

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets a lexical analyser which operates at the statement level.
ILexicalContext *getStatementLexer() { return StatementLexer::get(); }

//! @brief Gets a lexical analyser which operates at the expression level.
ILexicalContext *getExpressionLexer() { return ExpressionLexer::get(); }

//! @brief Gets a lexical analyser which can recognise the tokens of a
//! register list used in LDM/STM instructions.
ILexicalContext *getCoreRegListLexer() { return CoreRegListLexer::get(); }

//! @brief Gets a lexical analyser which can recognise a status register name.
ILexicalContext *getPsrNameLexer()
{
    static PsrNameLexer instance(false);

    return &instance;
}

//! @brief Gets a lexical analyser which can recognise a status register name and
//! optional component suffixes.
ILexicalContext *getPsrComponentLexer()
{
    static PsrNameLexer instance(true);

    return &instance;
}


}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file AsmTools/FormatInstruction.hpp
//! @brief The declaration of an entry point to a function which will format a
//! 32-bit ARM machine code instruction as text.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_FORMAT_INSTRUCTION_HPP__
#define __ASM_TOOLS_FORMAT_INSTRUCTION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"
#include "FormatInstruction.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! An object which builds up a text representation of an assembly
//! language instruction.
class InstructionBuilder
{
public:
    // Construction/Destruction
    InstructionBuilder();
    InstructionBuilder(bool isFormatting);
    ~InstructionBuilder() = default;

    // Accessors
    bool isEmpty() const;
    Ag::String toString() const;
    FormattedInstruction format();
    uint32_t getPendingTokenData() const;
    void setPendingTokenData(uint32_t data);

    // Operations
    void beginToken(FormattedInstruction::TokenType tokenType,
                    uint32_t data = 0);
    void endToken();
    void extendToken();
    void appendToken(char ch, FormattedInstruction::TokenType tokenType);
    void appendOptionalToken(char ch, FormattedInstruction::TokenType tokenType,
                             bool isAppended);

    void appendSeparator();
    void appendMnemonic(InstructionMnemonic mnemonic);
    void appendConditionCode(ConditionCode condition);
    void appendOffset(int32_t offset, uint32_t formatFlags);
    void appendAddress(uint32_t address, const FormatterOptions &options);
    void appendSwiNumber(uint32_t swiNo, const FormatterOptions &options);
    void appendComment(uint32_t value, uint32_t formatFlags);
    void appendSuffix(char suffix, bool hasSuffix);
    void appendRegisterList(uint16_t regMask, uint32_t formatFlags);
    void appendHexImmediate(uint32_t value, uint8_t minDigits,
                            const FormatterOptions &options);
    void appendHexValue(uint32_t value, uint8_t minDigits);
    void append(CoreRegister reg, uint32_t formatFlags, bool negated = false);
    void append(ShiftType shift);
    void append(CoProcId cpId);
    void append(CoProcRegister reg);
    void append(char ch);
    void append(int32_t value);
    void append(uint32_t value);
    void append(Ag::utf8_cptr_t text);
private:
    // Internal Fields
    FormattedInstruction::TokenCollection _tokens;
    std::string _buffer;
    size_t _pendingTokenStart;
    uint32_t _pendingTokenData;
    FormattedInstruction::TokenType _pendingTokenType;
    bool _isFormatting;
};

//! @brief A structure used to pass parameters which allow a 32-bit ARM
//! machine code instruction to be formatted as text.
struct FormatParams
{
    // Public Fields
    const FormatterOptions *Options;
    const InstructionParams *Params;
    InstructionBuilder Builder;
    OperationClass OpClass;
    InstructionMnemonic Mnemonic;
    ConditionCode Condition;

    // Construction/Destruction
    FormatParams(const FormatterOptions *options,
                 const InstructionParams *params,
                 OperationClass opClass,
                 InstructionMnemonic mnemonic,
                 ConditionCode condition,
                 bool isFormatting);

    // Accessors
    bool hasOption(FormatterOptions::Flags flag) const;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool formatInstruction(FormatParams &params);

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

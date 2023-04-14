//! @file Test_StatementLexer.cpp
//! @brief The definition of unit tests for the statement lexical analyser.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/GTest_Core.hpp"
#include "AsmTools/InstructionInfo.hpp"
#include "AsmTools/Options.hpp"

#include "AsmEnums.hpp"
#include "InputContext.hpp"
#include "LexicalAnalysers.hpp"
#include "LexicalContext.hpp"

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
InputContext createInput(const char *sourceCode)
{
    IInputSourcePtr source = createBufferInputSource(String(sourceCode));
    Location position("MyProject/Libs/Here.asm");
    String sourceId("MyProject/Libs/Lib.asm");

    return InputContext(source, position, sourceId, 2);
}

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(StatementLexer, NoTextReturnsNoToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, WhiteSpaceReturnsNoToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("   \t\r  ");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, CommentReturnsNoToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("  ; This is a line comment.");

    Token next;

    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseLabelToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(".Hello");

    Token next;

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Label);

    const Location &loc = next.getLocation();

    EXPECT_FALSE(loc.FileName.isEmpty());
    EXPECT_EQ(loc.LineNo, 1);
    EXPECT_EQ(loc.Offset, 1);
    EXPECT_STRINGEQC(next.getValue(), "Hello");
}

GTEST_TEST(StatementLexer, RecogniseLabelTokenWithWhiteSpace)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(" \t.Hello  ");

    Token next;

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Label);

    const Location &loc = next.getLocation();

    EXPECT_FALSE(loc.FileName.isEmpty());
    EXPECT_EQ(loc.LineNo, 1);
    EXPECT_EQ(loc.Offset, 3);
    EXPECT_STRINGEQC(next.getValue(), "Hello");

    // Verify there are no more tokens.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Empty);
}

GTEST_TEST(StatementLexer, RecogniseLabelTokenWithLineComment)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "\t.myLabel  ; Comment on the same line as the label.");

    Token next;

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Label);

    const Location &loc = next.getLocation();

    EXPECT_FALSE(loc.FileName.isEmpty());
    EXPECT_EQ(loc.LineNo, 1);
    EXPECT_EQ(loc.Offset, 2);
    EXPECT_STRINGEQC(next.getValue(), "myLabel");

    // Verify there are no more tokens.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Empty);
}

GTEST_TEST(StatementLexer, RecogniseEmptyLabelToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(".; Commented out");

    Token next;

    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Error);

    const Location &loc = next.getLocation();

    EXPECT_FALSE(loc.FileName.isEmpty());
    EXPECT_EQ(loc.LineNo, 1);
    EXPECT_EQ(loc.Offset, 1);
    EXPECT_FALSE(next.getValue().isEmpty());

    // Verify there are no more tokens.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Empty);
}


GTEST_TEST(StatementLexer, ContinuationReturnsNoToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("  \\  \n ] ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::CloseSquare);
}

GTEST_TEST(StatementLexer, ContinuationWarnsOfTrailingCharacters)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("  \\ , \n { ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::Warning);
    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::OpenBrace);
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(StatementLexer, ContinuationIgnoresComments)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("  \\ ; Hello \n { ");

    Token token;

    EXPECT_TRUE(specimen->tryGetNextToken(input, token));
    EXPECT_EQ(token.getClass(), TokenClass::OpenBrace);
    EXPECT_FALSE(specimen->tryGetNextToken(input, token));
}

GTEST_TEST(StatementLexer, RecogniseNewLineAsStatementTerminatorToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("; header comment.\n.label");

    Token next;

    // Get the new line
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    EXPECT_EQ(next.getLocation().LineNo, 2);
    EXPECT_EQ(next.getLocation().Offset, 0);
    EXPECT_TRUE(next.getValue().isEmpty());

    // Get the label on the next line.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Label);
    EXPECT_EQ(next.getLocation().LineNo, 2);
    EXPECT_EQ(next.getLocation().Offset, 1);
    EXPECT_STRINGEQC(next.getValue(), "label");

    // Ensure that is the end of the stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseColonAsStatementTerminatorToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(".label:.otherLabel\n");

    Token next;

    // Get the label statement token.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Label);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 1);
    EXPECT_STRINGEQC(next.getValue(), "label");

    // Recognise the colon.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 7);
    EXPECT_TRUE(next.getValue().isEmpty());

    // Recognise the second label statement.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Label);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 8);
    EXPECT_STRINGEQC(next.getValue(), "otherLabel");

    // Recognised the new line as a statement terminator.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    EXPECT_EQ(next.getLocation().LineNo, 2);
    EXPECT_EQ(next.getLocation().Offset, 0);
    EXPECT_TRUE(next.getValue().isEmpty());

    // Ensure that is the end of the stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseGarbageAsErrorToken)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("  @#!.hello\n");

    Token next;

    // Get the label statement token.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Error);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 3);
    EXPECT_FALSE(next.getValue().isEmpty());

    // Ensure statement terminator found after garbage.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseCharacterTokens)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(": ! { } [ ] , # ^\n");

    TokenClass expectedTokens[] =
    {
        TokenClass::StatementTerminator,
        TokenClass::Bang,
        TokenClass::OpenBrace,
        TokenClass::CloseBrace,
        TokenClass::OpenSquare,
        TokenClass::CloseSquare,
        TokenClass::Comma,
        TokenClass::Hash,
        TokenClass::Hat,
        TokenClass::StatementTerminator
    };

    Token token;

    for (TokenClass expectedToken : expectedTokens)
    {
        ASSERT_TRUE(specimen->tryGetNextToken(input, token));
        EXPECT_EQ(token.getClass(), expectedToken);
    }
}

GTEST_TEST(StatementLexer, RecogniseIncludeDirective)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("  %iNcLuDe ; The include directive. ");

    Token next;

    // Get the label statement token.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::AssemblyDirective);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 3);
    AssemblyDirectiveType directiveType;

    ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::DirectiveType, directiveType));
    EXPECT_EQ(directiveType, AssemblyDirectiveType::Include);

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseCpuModelDirective)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "%ARMv2\n"
        "%ARMv2a: %ArmV3\n"
        "  %armV4 ; StrongARM/ARM810\n");

    InstructionSet models[] = {
        InstructionSet::ArmV2,
        InstructionSet::ArmV2a,
        InstructionSet::ArmV3,
        InstructionSet::ArmV4,
    };

    Token next;
    InstructionSet model;
    AssemblyDirectiveType directiveType;

    for (InstructionSet expectedModel : models)
    {
        // Get the processor model directive.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::AssemblyDirective);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::DirectiveType, directiveType));
        EXPECT_EQ(directiveType, AssemblyDirectiveType::InstructionSet);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::InstructionSet, model));
        EXPECT_EQ(model, expectedModel);

        // Get the statement terminator.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    }

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, EnsureBadDirectiveIsError)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput("%ARMv2f  ; ARM 2 with FPA?");

    Token next;

    // Get the label statement token.
    EXPECT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::Error);
    EXPECT_EQ(next.getLocation().LineNo, 1);
    EXPECT_EQ(next.getLocation().Offset, 1);
    EXPECT_FALSE(next.getValue().isEmpty());

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseCpuExtensionDirective)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "%FPA\n"
        "%vfpV1\n");

    ArchExtensionEnum extensions[] = {
        ArchExtensionEnum::Fpa,
        ArchExtensionEnum::VfpV1,
    };

    Token next;
    ArchExtensionEnum extension;
    AssemblyDirectiveType directiveType;

    for (ArchExtensionEnum expectedExtenstion : extensions)
    {
        // Get the processor extension directive.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::AssemblyDirective);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::DirectiveType, directiveType));
        EXPECT_EQ(directiveType, AssemblyDirectiveType::ProcessorExtension);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::ProcessorExtension, extension));
        EXPECT_EQ(extension, expectedExtenstion);

        // Get the statement terminator.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    }

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseCpuModeDirective)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "%arm\n"
        "%Thumb\n");

    ProcessorMode modes[] = {
        ProcessorMode::Arm,
        ProcessorMode::Thumb,
    };

    Token next;
    ProcessorMode mode;
    AssemblyDirectiveType directiveType;

    for (ProcessorMode expectedMode : modes)
    {
        // Get the processor mode directive.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::AssemblyDirective);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::DirectiveType, directiveType));
        EXPECT_EQ(directiveType, AssemblyDirectiveType::ProcessorMode);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::ProcessorMode, mode));
        EXPECT_EQ(mode, expectedMode);

        // Get the statement terminator.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    }

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseAssemblyModeDirective)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "%26bit\n"
        "%32bit\n");

    AddressMode modes[] = {
        AddressMode::Bits26,
        AddressMode::Bits32,
    };

    Token next;
    AddressMode mode;
    AssemblyDirectiveType directiveType;

    for (AddressMode expectedMode : modes)
    {
        // Get the processor mode directive.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::AssemblyDirective);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::DirectiveType, directiveType));
        EXPECT_EQ(directiveType, AssemblyDirectiveType::AddressMode);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::AddressMode, mode));
        EXPECT_EQ(mode, expectedMode);

        // Get the statement terminator.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    }

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}


GTEST_TEST(StatementLexer, RecogniseAluInstructions)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "And: ORR: eOr: BIC ; Logical ops\n"
        "add:adc:sub:sbc:rsb:rsc ; addition and subtraction\n"
        "cmp: cmn: tst : teq ; comparison\n"
        "MOV : MVN ; Register transfer\n"
        "Mul :mlA ; Multiplication \n"
    );

    std::pair<TokenClass, InstructionMnemonic> mnemonics[] = {
        { TokenClass::MnemonicAluOp, InstructionMnemonic::And },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Orr },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Eor },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Bic },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Add },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Adc },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Sub },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Sbc },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Rsb },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Rsc },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Cmp },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Cmn },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Tst },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Teq },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Mov },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Mvn },
        { TokenClass::MnemonicMul3, InstructionMnemonic::Mul },
        { TokenClass::MnemonicMul4 , InstructionMnemonic::Mla },
    };

    Token next;
    InstructionMnemonic mnemonic;

    for (const auto &expectedInstruction : mnemonics)
    {
        String instruction = String::format("InstructionMnemonic: {0}",
                                            { static_cast<uint32_t>(expectedInstruction.second) });
        SCOPED_TRACE(instruction.getUtf8Bytes());

        // Get the processor mode directive.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), expectedInstruction.first);

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::Mnemonic, mnemonic));
        EXPECT_EQ(expectedInstruction.second, mnemonic);

        // Get the statement terminator.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    }

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseBranchInstructions)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "B: BL: BLT: BX ; Basic Branches\n"
        "BLE : BLLE : BICEQ ; Weird condition code combos.\n"
        "BLLT ; Another confusing combo.\n"
    );

    std::tuple<TokenClass, InstructionMnemonic, ConditionCode> mnemonics[] = {
        { TokenClass::MnemonicBranch, InstructionMnemonic::B, ConditionCode::Nv },
        { TokenClass::MnemonicBranch, InstructionMnemonic::Bl, ConditionCode::Nv },
        { TokenClass::MnemonicBranch, InstructionMnemonic::B, ConditionCode::Lt },
        { TokenClass::MnemonicBranchExchange, InstructionMnemonic::Bx, ConditionCode::Nv },
        { TokenClass::MnemonicBranch, InstructionMnemonic::B, ConditionCode::Le },
        { TokenClass::MnemonicBranch, InstructionMnemonic::Bl, ConditionCode::Le },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Bic, ConditionCode::Eq },
        { TokenClass::MnemonicBranch, InstructionMnemonic::Bl, ConditionCode::Lt },
    };

    Token next;
    InstructionMnemonic mnemonic;
    ConditionCode cond;

    for (const auto &expected : mnemonics)
    {
        String instruction = String::format("InstructionMnemonic: {0}, Cond: {1}",
                                            { static_cast<uint32_t>(std::get<1>(expected)),
                                              static_cast<uint32_t>(std::get<2>(expected)) });
        SCOPED_TRACE(instruction.getUtf8Bytes());

        // Get the processor mode directive.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), std::get<0>(expected));

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::Mnemonic, mnemonic));
        EXPECT_EQ(std::get<1>(expected), mnemonic);

        if (tryGetTokenEnum(next, TokenProperty::ConditionCode, cond))
        {
            EXPECT_EQ(cond, std::get<2>(expected));
        }
        else
        {
            EXPECT_EQ(std::get<2>(expected), ConditionCode::Nv);
        }

        // Get the statement terminator.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    }

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}


GTEST_TEST(StatementLexer, RecogniseConditionCodes)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "MovEq: AndNe : ORRCS: BICHs\n"
        "STCCc: MULLo: MlAMi: ADdPl\n"
        "aDcVs:  SbcVc: RSCHi: rsbLs \n"
        "TSTGe: teQLt: cMpGt: CMNLe ; Random line comment.\n"
        " LdrAl: ADRNv: ; Nothing more here."
    );

    std::tuple<TokenClass, InstructionMnemonic, ConditionCode> mnemonics[] = {
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Mov, ConditionCode::Eq },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::And, ConditionCode::Ne },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Orr, ConditionCode::Cs },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Bic, ConditionCode::Hs },

        { TokenClass::MnemonicCoProcDataTransfer, InstructionMnemonic::Stc, ConditionCode::Cc },
        { TokenClass::MnemonicMul3, InstructionMnemonic::Mul, ConditionCode::Lo },
        { TokenClass::MnemonicMul4, InstructionMnemonic::Mla, ConditionCode::Mi },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Add, ConditionCode::Pl },

        { TokenClass::MnemonicAluOp, InstructionMnemonic::Adc, ConditionCode::Vs },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Sbc, ConditionCode::Vc },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Rsc, ConditionCode::Hi },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Rsb, ConditionCode::Ls },

        { TokenClass::MnemonicAluOp, InstructionMnemonic::Tst, ConditionCode::Ge },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Teq, ConditionCode::Lt },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Cmp, ConditionCode::Gt },
        { TokenClass::MnemonicAluOp, InstructionMnemonic::Cmn, ConditionCode::Le },

        { TokenClass::MnemonicSingleDataTransfer, InstructionMnemonic::Ldr, ConditionCode::Al },
        { TokenClass::MnemonicAdr, InstructionMnemonic::Adr, ConditionCode::Nv },
    };

    Token next;
    InstructionMnemonic mnemonic;
    ConditionCode cond;

    for (const auto &expected : mnemonics)
    {
        String instruction = String::format("InstructionMnemonic: {0}, Cond: {1}",
                                            { static_cast<uint32_t>(std::get<1>(expected)),
                                            static_cast<uint32_t>(std::get<2>(expected)) });
        SCOPED_TRACE(instruction.getUtf8Bytes());

        // Get the processor mode directive.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), std::get<0>(expected));

        ASSERT_TRUE(tryGetTokenEnum(next, TokenProperty::Mnemonic, mnemonic));
        EXPECT_EQ(std::get<1>(expected), mnemonic);

        if (tryGetTokenEnum(next, TokenProperty::ConditionCode, cond))
        {
            EXPECT_EQ(cond, std::get<2>(expected));
        }
        else
        {
            EXPECT_EQ(std::get<2>(expected), ConditionCode::Nv);
        }

        // Get the statement terminator.
        ASSERT_TRUE(specimen->tryGetNextToken(input, next));
        EXPECT_EQ(next.getClass(), TokenClass::StatementTerminator);
    }

    // Verify end of stream.
    EXPECT_FALSE(specimen->tryGetNextToken(input, next));
}

GTEST_TEST(StatementLexer, RecogniseAluInstructionSuffixes)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "MovEq AndS  ORRCSS TSTP"
    );

    Token next;

    // Recognise MOV.
    ASSERT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::MnemonicAluOp);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::Mnemonic, InstructionMnemonic::MaxMnemonic),
              InstructionMnemonic::Mov);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::ConditionCode, ConditionCode::Al),
              ConditionCode::Eq);
    EXPECT_EQ(getTokenFlag(next, TokenProperty::UpdatePsr, false), false);

    // Recognise AND.
    ASSERT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::MnemonicAluOp);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::Mnemonic, InstructionMnemonic::MaxMnemonic),
              InstructionMnemonic::And);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::ConditionCode, ConditionCode::Al),
              ConditionCode::Al);
    EXPECT_EQ(getTokenFlag(next, TokenProperty::UpdatePsr, false), true);

    // Recognise ORR.
    ASSERT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::MnemonicAluOp);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::Mnemonic, InstructionMnemonic::MaxMnemonic),
              InstructionMnemonic::Orr);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::ConditionCode, ConditionCode::Al),
              ConditionCode::Cs);
    EXPECT_EQ(getTokenFlag(next, TokenProperty::UpdatePsr, false), true);

    // Recognise TST.
    ASSERT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::MnemonicAluOp);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::Mnemonic, InstructionMnemonic::MaxMnemonic),
              InstructionMnemonic::Tst);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::ConditionCode, ConditionCode::Al),
              ConditionCode::Al);
    EXPECT_EQ(getTokenFlag(next, TokenProperty::UpdatePsr, false), false);
    EXPECT_EQ(getTokenFlag(next, TokenProperty::OverwritePsr, false), true);
}

GTEST_TEST(StatementLexer, RecogniseAdrDirective)
{
    ILexicalContext *specimen = getStatementLexer();
    InputContext input = createInput(
        "adrvs ADRpll ADReqe"
    );

    Token next;

    // Recognise ADR.
    ASSERT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::MnemonicAdr);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::Mnemonic, InstructionMnemonic::MaxMnemonic),
              InstructionMnemonic::Adr);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::ConditionCode, ConditionCode::Al),
              ConditionCode::Vs);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::SequenceEncoding,
                           MultiWordEncoding::Single), MultiWordEncoding::Single);

    // Recognise ADRL.
    ASSERT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::MnemonicAdr);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::Mnemonic, InstructionMnemonic::MaxMnemonic),
              InstructionMnemonic::Adr);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::ConditionCode, ConditionCode::Al),
              ConditionCode::Pl);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::SequenceEncoding,
                           MultiWordEncoding::Extended), MultiWordEncoding::Long);

    // Recognise ADRE.
    ASSERT_TRUE(specimen->tryGetNextToken(input, next));
    EXPECT_EQ(next.getClass(), TokenClass::MnemonicAdr);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::Mnemonic, InstructionMnemonic::MaxMnemonic),
              InstructionMnemonic::Adr);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::ConditionCode, ConditionCode::Al),
              ConditionCode::Eq);
    EXPECT_EQ(getTokenEnum(next, TokenProperty::SequenceEncoding,
                           MultiWordEncoding::Single), MultiWordEncoding::Extended);

    ASSERT_FALSE(specimen->tryGetNextToken(input, next));
}

} // Anonymous namespace

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file Core/Test_CommandLine.cpp
//! @brief The definition of unit tests for the CommandLine class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <initializer_list>

#include <gtest/gtest.h>

#include "Ag/Core/ProgramArguments.hpp"
#include "Ag/Core/Utils.hpp"

namespace Ag {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum Options
{
    Opt_Output,
    Opt_Input,
    Opt_Verbose,
    Opt_Quiet,
    Opt_ShowHelp,

    Opt_Max,
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
static Cli::Schema createSchema()
{
    Cli::SchemaBuilder builder;

    builder.defineOption(Opt_Output,
                         "Specifies the output file.",
                         Cli::OptionValue::Mandatory,
                         "OutputFile");
    builder.defineOption(Opt_Input,
                         "Specifies the input file (but ignores it really).",
                         Cli::OptionValue::Mandatory,
                         "InputFile");
    builder.defineOption(Opt_Verbose,
                         "Enables verbose output");
    builder.defineOption(Opt_Quiet,
                         "Suppresses output");
    builder.defineOption(Opt_ShowHelp,
                         "Shows help, optionally on a specific keyword area.",
                         Cli::OptionValue::Optional, "HelpTopic");

    builder.defineAlias(Opt_Output, "output", true);
    builder.defineAlias(Opt_Output, U'o', true);
    builder.defineAlias(Opt_Input, "input", true);
    builder.defineAlias(Opt_Input, U'i', false);
    builder.defineAlias(Opt_Verbose, "verbose", false);
    builder.defineAlias(Opt_Verbose, U'v', true);
    builder.defineAlias(Opt_Quiet, U'Q', true);
    builder.defineAlias(Opt_Quiet, "QUIET", true);
    builder.defineAlias(Opt_ShowHelp, U'?', true);
    builder.defineAlias(Opt_ShowHelp, U'h', false);
    builder.defineAlias(Opt_ShowHelp, "help", false);

    return builder.createSchema();
}

class TestCommandLine : public Cli::ProgramArguments
{
private:
    bool _processValues;
public:
    TestCommandLine() :
        Cli::ProgramArguments(createSchema()),
        _processValues(false)
    {
    }

    using ArgPair = std::pair<uint32_t, String>;
    std::vector<ArgPair> Args;

    // Operations
    void enableProcessing() { _processValues = true; }

    ::testing::AssertionResult compareArgs(const std::initializer_list<ArgPair> &args) const
    {
        if (Args.size() != args.size())
        {
            return ::testing::AssertionFailure() <<
                "The incorrect number of arguments were processed (" <<
                Args.size() << " vs " << args.size() << ").";
        }

        auto src = Args.begin();
        auto target = args.begin();
        int item = 1;

        while (src != Args.end())
        {
            if (src->first != target->first)
            {
                return ::testing::AssertionFailure() <<
                    "Argument " << item << " does not have the expected identifier (" <<
                    src->first << " vs " << target->first << ").";

            }
            else if (src->second != target->second)
            {
                return ::testing::AssertionFailure() <<
                    "Argument " << item << " does not have the expected value ('" <<
                    src->second.getUtf8Bytes() << "' vs '" << target->second.getUtf8Bytes() <<
                    "').";
            }
            else
            {
                ++src;
                ++target;
            }
        }

        return ::testing::AssertionSuccess();
    }

    // Overrides
protected:
    virtual bool processOption(uint32_t id, const String &value,
                               String & /* error */) override
    {
        Args.emplace_back(id, value);

        return true;
    }

    virtual bool processArgument(const String &argument,
                                 String & /* error */) override
    {
        Args.emplace_back(~0u, argument);

        return true;
    }

    virtual bool validate(String & /* error */) const override
    {
        return true;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(CommandLine, LookupShortForm)
{
    Cli::Schema specimen = createSchema();
    size_t index;

    // Check a case sensitive option.
    ASSERT_TRUE(specimen.tryFindShortOption(U'o', index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Output));
    EXPECT_FALSE(specimen.tryFindShortOption(U'O', index));

    // Check a case insensitive option.
    ASSERT_TRUE(specimen.tryFindShortOption(U'I', index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Input));
    ASSERT_TRUE(specimen.tryFindShortOption(U'i', index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Input));

    // Check another case sensitive option.
    ASSERT_TRUE(specimen.tryFindShortOption(U'v', index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Verbose));
    EXPECT_FALSE(specimen.tryFindShortOption(U'V', index));

    ASSERT_TRUE(specimen.tryFindShortOption(U'Q', index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Quiet));
    EXPECT_FALSE(specimen.tryFindShortOption(U'q', index));

    // Check an option is more than two definitions.
    ASSERT_TRUE(specimen.tryFindShortOption(U'?', index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_ShowHelp));
    ASSERT_TRUE(specimen.tryFindShortOption(U'h', index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_ShowHelp));
    ASSERT_TRUE(specimen.tryFindShortOption(U'H', index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_ShowHelp));

    // Check a random option.
    EXPECT_FALSE(specimen.tryFindShortOption(U'@', index));
}

GTEST_TEST(CommandLine, LookupLongForm)
{
    Cli::Schema specimen = createSchema();
    size_t index;

    // Check a case sensitive option.
    ASSERT_TRUE(specimen.tryFindLongOption("output", index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Output));
    EXPECT_FALSE(specimen.tryFindLongOption("Output", index));

    // Check another case sensitive option.
    ASSERT_TRUE(specimen.tryFindLongOption("input", index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Input));
    EXPECT_FALSE(specimen.tryFindLongOption("inPUT", index));

    // Check a case insensitive option
    ASSERT_TRUE(specimen.tryFindLongOption("verbose", index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Verbose));
    ASSERT_TRUE(specimen.tryFindLongOption("VERBOSE", index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Verbose));
    ASSERT_TRUE(specimen.tryFindLongOption("vErBoSe", index));
    EXPECT_EQ(specimen.getOptionDefinition(index).getId(), static_cast<uint32_t>(Opt_Verbose));
}

GTEST_TEST(CommandLine, ParseWin32CommandLineSuccess)
{
    TestCommandLine specimen;
    String error;
#ifdef _WIN32
    wchar_cptr_t input = L"-o=Log.txt --help /i "
        "\"C:\\Users\\My Documents\\Table.csv\" /? Options";
#else
    wchar_cptr_t input = L"-o=Log.txt --help -i "
        "\"C:\\Users\\My Documents\\Table.csv\" -? Options";
#endif

    EXPECT_TRUE(specimen.tryParse(input, error));
    EXPECT_FALSE(specimen.getProgramFile().isEmpty());
    EXPECT_TRUE(error.isEmpty());
    EXPECT_TRUE(specimen.compareArgs({
        { Opt_Output, "Log.txt" },
        { Opt_ShowHelp, String::Empty },
        { Opt_Input, "C:\\Users\\My Documents\\Table.csv" },
        { Opt_ShowHelp, "Options" } }));
}

GTEST_TEST(CommandLine, ParsePOSIXCommandLineSuccess)
{
    TestCommandLine specimen;
    String error;
    const char *argv[] = {
#ifdef _WIN32
        "C:\\Program Files\\Ag\\Test_AgCore.exe",
        "-o=Log.txt",
        "--help",
        "/i",
        "C:\\Users\\My Documents\\Table.csv",
        "/?",
        "Options" };
#else
        "/usr/bin/Test_AgCore",
        "-o=Log.txt",
        "--help",
        "-i",
        "C:\\Users\\My Documents\\Table.csv",
        "-?",
        "Options" };
#endif
    int argc = static_cast<int>(arraySize(argv));

    EXPECT_TRUE(specimen.tryParse(argc, argv, error));
    EXPECT_FALSE(specimen.getProgramFile().isEmpty());
    EXPECT_TRUE(error.isEmpty());
    EXPECT_TRUE(specimen.compareArgs({
        { Opt_Output, "Log.txt" },
        { Opt_ShowHelp, String::Empty },
        { Opt_Input, "C:\\Users\\My Documents\\Table.csv" },
        { Opt_ShowHelp, "Options" } }));
}

GTEST_TEST(CommandLine, ParseMultipleShortOptions)
{
    TestCommandLine specimen;
    String error;
    const char *argv[] = {
        "C:\\Program Files\\Ag\\Test_AgCore.exe",
        "-?Qo=Log.txt",
        "Input.doc",
        "-hvi",
        "C:\\Users\\My Documents\\Table.csv" };
    int argc = static_cast<int>(arraySize(argv));

    EXPECT_TRUE(specimen.tryParse(argc, argv, error));
    EXPECT_TRUE(error.isEmpty());
    EXPECT_TRUE(specimen.compareArgs({
        { Opt_ShowHelp, String::Empty },
        { Opt_Quiet, String::Empty },
        { Opt_Output, "Log.txt" },
        { ~0u, "Input.doc" },
        { Opt_ShowHelp, String::Empty },
        { Opt_Verbose, String::Empty },
        { Opt_Input, "C:\\Users\\My Documents\\Table.csv" }
    }));
}

GTEST_TEST(CommandLine, MissingRequiredValue)
{
    TestCommandLine specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse(L"-i Script.txt --output --help", error));
    EXPECT_FALSE(error.isEmpty());
}

GTEST_TEST(CommandLine, MissingClosingQuote)
{
    TestCommandLine specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse(L"-i Script.txt --output \"Output.log", error));
    EXPECT_FALSE(error.isEmpty());
}

GTEST_TEST(CommandLine, MissingRequiredValueAtEnd)
{
    TestCommandLine specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse(L"-i Script.txt --output", error));
    EXPECT_FALSE(error.isEmpty());
}

GTEST_TEST(CommandLine, MalFormedShortOption)
{
    TestCommandLine specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse(L"--help - Script.txt", error));
    EXPECT_FALSE(error.isEmpty());
}

GTEST_TEST(CommandLine, MalFormedLongOption)
{
    TestCommandLine specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse(L"--help -- Script.txt", error));
    EXPECT_FALSE(error.isEmpty());
}

#ifdef _WIN32
GTEST_TEST(CommandLine, MalFormedWin32Option)
{
    TestCommandLine specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse(L"--help / Script.txt", error));
    EXPECT_FALSE(error.isEmpty());
}
#endif

GTEST_TEST(CommandLine, MalFormedOptionWithValue)
{
    TestCommandLine specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse(L"--help -=Script.txt", error));
    EXPECT_FALSE(error.isEmpty());
}

} // TED

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


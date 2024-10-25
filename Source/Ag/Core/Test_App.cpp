//! @file Core/Test_App.cpp
//! @brief The definition of unit tests for the App class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>
#include <memory>
#include <vector>
#include <string_view>
#include <gtest/gtest.h>

#include "Ag/Core/App.hpp"
#include "Ag/Core/ProgramArguments.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/String.hpp"
#include "Ag/Core/Utils.hpp"

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum FailurePhase
{
    Fail_None,
    Fail_CommandLineOption,
    Fail_CommandLineArgument,
    Fail_CommandLineValidate,
    Fail_Initialise,
    Fail_InitialiseThrow,
    Fail_Run,
    Fail_RunThrow,
    Fail_ShutdownThrow,
};

enum CliOption
{
    Opt_ShowHelp,
    Opt_Input,
    Opt_PossibleFail,
};

class TestCommandLine : public Cli::ProgramArguments
{
private:
    String _errorMessage;
    FailurePhase _phase;

    static Cli::Schema createSchema()
    {
        Cli::SchemaBuilder builder;
        builder.defineOption(Opt_ShowHelp, "Shows help");
        builder.defineOption(Opt_Input, "Sets input", Cli::OptionValue::Mandatory);
        builder.defineOption(Opt_PossibleFail, "Might fail", Cli::OptionValue::Optional);
        builder.defineAlias(Opt_ShowHelp, U'?');
        builder.defineAlias(Opt_ShowHelp, U'h');
        builder.defineAlias(Opt_ShowHelp, "help");
        builder.defineAlias(Opt_Input, U'i');
        builder.defineAlias(Opt_Input, "input");
        builder.defineAlias(Opt_PossibleFail, U'f');
        builder.defineAlias(Opt_PossibleFail, "fail");

        return builder.createSchema();
    }
public:
    TestCommandLine(FailurePhase phase, const String &failMessage) :
        Cli::ProgramArguments(createSchema()),
        _errorMessage(failMessage),
        _phase(phase)
    {
    }

    // Overrides
protected:
    virtual bool processOption(uint32_t id, const String &value, String &error) override
    {
        if ((id == Opt_PossibleFail) && (_phase == Fail_CommandLineOption))
        {
            error = _errorMessage;
            return false;
        }
        else
        {
            return Cli::ProgramArguments::processOption(id, value, error);
        }
    }

    virtual bool processArgument(const String &argument, String &error)
    {
        if (_phase == Fail_CommandLineArgument)
        {
            error = _errorMessage;
            return false;
        }
        else
        {
            return Cli::ProgramArguments::processArgument(argument, error);
        }
    }
    
    virtual bool validate(String &error) const
    {
        if (_phase == Fail_CommandLineValidate)
        {
            error = _errorMessage;
            return false;
        }
        else
        {
            return Cli::ProgramArguments::validate(error);
        }
    }
};

class TestApp : public App
{
    std::vector<String> _errors;
    String _failureMessage;
    FailurePhase _phase;
    bool _exceptionCaught;
    bool _runCalled;

public:
    TestApp(FailurePhase phase, const String &failMessage) :
        _failureMessage(failMessage),
        _phase(phase),
        _exceptionCaught(false),
        _runCalled(false)
    {
    }

    // Accessors
    bool didExecuteRun() const { return _runCalled; }
    bool isExceptionCaught() const { return _exceptionCaught; }
    bool hasErrors() const { return _errors.empty() == false; }
    bool hasMatchingError() const
    {
        for (const String &error : _errors)
        {
            if (strstr(error.getUtf8Bytes(), _failureMessage.getUtf8Bytes()) != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    const std::vector<String> getErrors() const { return _errors; }

    // Overrides
    virtual CommandLineUPtr createCommandLineArguments() const
    {
        return std::make_unique<TestCommandLine>(_phase, _failureMessage);
    }

    virtual bool initialise(const Cli::ProgramArguments * /* args */)
    {
        if (_phase == Fail_Initialise)
        {
            return false;
        }
        else if (_phase == Fail_InitialiseThrow)
        {
            throw OperationException(_failureMessage);
        }
        else
        {
            return true;
        }
    }
    
    virtual int run()
    {
        // The application tried to perform it's function.
        _runCalled = true;

        if (_phase == Fail_Run)
        {
            return 1;
        }
        else if (_phase == Fail_RunThrow)
        {
            throw OperationException(_failureMessage);
        }
        else
        {
            return 0;
        }
    }

    virtual void shutdown()
    {
        if (_phase == Fail_ShutdownThrow)
        {
            throw OperationException(_failureMessage);
        }
    }

    virtual void reportException(const std::exception &error) override
    {
        const Ag::Exception *agError;

        if (tryCast(&error, agError))
        {
            const std::string_view failure(_failureMessage.getUtf8Bytes(),
                                           _failureMessage.getUtf8Length());

            if (agError->getDetail().find(failure) != std::string_view::npos)
            {
                _exceptionCaught = true;
            }

            App::reportException(error);
        }
    }
    
    virtual void reportError(utf8_cptr_t errorText) override
    {
        _errors.emplace_back(errorText);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(App, SuccessfulRun)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_None, "{886E480F-8AAA-4D49-9086-10808BAF2BE5}");

    EXPECT_EQ(specimen.exec(L"-? --input=SomeFile.txt"), 0);
    EXPECT_FALSE(specimen.hasErrors());
    EXPECT_FALSE(specimen.isExceptionCaught());
    EXPECT_TRUE(specimen.didExecuteRun());
}

GTEST_TEST(App, FailProcessingCommandLineOption)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_CommandLineOption, "{C136B1EA-D51E-4DA5-B4F4-D3F7AB47FAF3}");

    EXPECT_NE(specimen.exec(L"-? --fail --input=SomeFile.txt"), 0);
    EXPECT_TRUE(specimen.hasErrors());
    EXPECT_TRUE(specimen.hasMatchingError());
    EXPECT_FALSE(specimen.isExceptionCaught());
    EXPECT_FALSE(specimen.didExecuteRun());
}

GTEST_TEST(App, FailProcessingCommandLineArgument)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_CommandLineArgument, "{C73B570A-6A78-422B-9DAA-9E84B5B17360}");

    EXPECT_NE(specimen.exec(L"-? ThiFile.log --input SomeFile.txt"), 0);
    EXPECT_TRUE(specimen.hasErrors());
    EXPECT_TRUE(specimen.hasMatchingError());
    EXPECT_FALSE(specimen.isExceptionCaught());
    EXPECT_FALSE(specimen.didExecuteRun());
}

GTEST_TEST(App, FailCommandLineValidation)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_CommandLineValidate, "{CC7390B7-0C29-430B-8EE4-882A289BA6A6}");

    EXPECT_NE(specimen.exec(L"-? --input SomeFile.txt"), 0);
    EXPECT_TRUE(specimen.hasErrors());
    EXPECT_TRUE(specimen.hasMatchingError());
    EXPECT_FALSE(specimen.isExceptionCaught());
    EXPECT_FALSE(specimen.didExecuteRun());
}

GTEST_TEST(App, InitialisationFailure)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_Initialise, "{C71CFAB0-03F5-4C27-BCC9-3017D115E316}");

    EXPECT_NE(specimen.exec(L"-? --input=SomeFile.txt"), 0);
    EXPECT_FALSE(specimen.hasErrors());
    EXPECT_FALSE(specimen.isExceptionCaught());
    EXPECT_FALSE(specimen.didExecuteRun());
}

GTEST_TEST(App, InitialisationException)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_InitialiseThrow, "{865F6D01-6845-46F5-8CC5-29860A8281C2}");

    EXPECT_NE(specimen.exec(L"-? --input=SomeFile.txt"), 0);
    EXPECT_TRUE(specimen.hasErrors());
    EXPECT_TRUE(specimen.hasMatchingError());
    EXPECT_FALSE(specimen.didExecuteRun());

    // Exceptions caught during initialisation don't go to onUnhandledException().
    EXPECT_FALSE(specimen.isExceptionCaught());
}

GTEST_TEST(App, RuntimeFailure)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_Run, "{7D7EC2B5-592E-4147-9CDE-A048CC5FCF6A}");

    EXPECT_NE(specimen.exec(L"-? --input=SomeFile.txt"), 0);

    // No exception thrown or error reported for run() != 0.
    EXPECT_FALSE(specimen.hasErrors());
    EXPECT_FALSE(specimen.hasMatchingError());
    EXPECT_FALSE(specimen.isExceptionCaught());
    EXPECT_TRUE(specimen.didExecuteRun());
}

GTEST_TEST(App, RuntimeException)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_RunThrow, "{04167A93-4964-4EA1-85ED-DF3DE2CF73C4}");

    EXPECT_NE(specimen.exec(L"-? --input=SomeFile.txt"), 0);
    EXPECT_TRUE(specimen.hasErrors());
    EXPECT_TRUE(specimen.hasMatchingError());
    EXPECT_TRUE(specimen.isExceptionCaught());
    EXPECT_TRUE(specimen.didExecuteRun());
}

GTEST_TEST(App, ShutdownException)
{
    // Use a unique string for the error message so that there is no confusion.
    TestApp specimen(Fail_ShutdownThrow, "{7ABDEC4E-6A50-4EFF-AFCA-424C08344DFA}");

    EXPECT_NE(specimen.exec(L"-? --input=SomeFile.txt"), 0);

    // Exceptions thrown on shutdown are not reported, but do change the process result.
    EXPECT_FALSE(specimen.hasErrors());
    EXPECT_FALSE(specimen.hasMatchingError());
    EXPECT_FALSE(specimen.isExceptionCaught());
    EXPECT_TRUE(specimen.didExecuteRun());
}

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


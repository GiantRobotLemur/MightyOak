//! @file Ag/Core/ProgarmArguments.hpp
//! @brief The declaration of an object which manages command line argument
//! parsing, processing and verification.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_PROGRAM_ARGUMENTS_HPP__
#define __AG_CORE_PROGRAM_ARGUMENTS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "CommandLineSchema.hpp"

namespace Ag {

namespace Cli {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class ArgumentCollection;

//! @brief An object used to process command line arguments and store
//! information relevant to the application.
class ProgramArguments
{
public:
    // Construction/Destruction
    ProgramArguments(const Schema &schema);
    ~ProgramArguments() = default;

    // Accessors
    const String &getProgramFile() const;
    const Schema &getSchema() const;

    // Operations
    bool tryParse(wchar_cptr_t win32CommandLine, String &error);
    bool tryParse(int argc, const char **argv, String &error);
    bool tryParse(int argc, const wchar_t * const *argv, String &error);

    // Overrides
protected:
    virtual bool processOption(uint32_t id, const String &value, String &error);
    virtual bool processArgument(const String &argument, String &error);
    virtual bool validate(String &error) const;
    virtual void postProcess();
private:
    // Internal Functions
    bool processArgumentTokens(const ArgumentCollection &tokens, String &error);

    // Internal Fields
    Schema _schema;
    String _programFile;
};

}} // namespace Ag::Cli

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

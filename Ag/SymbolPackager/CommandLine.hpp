//! @file CommandLine.hpp
//! @brief The declaration of an object which manages command line arguments.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SYMPKG_COMMAND_LINE_HPP__
#define __AG_SYMPKG_COMMAND_LINE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
enum Command
{
    Command_None,
    Command_DisplayHelp,
    Command_PackageMsMapFile,
    Command_PackageGnuMapFile,
    Command_PackageGnuNmFile,
    Command_PackagePdbFile,
    Command_ExtractSymFile,
    Command_Max,
};


////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which parses the command line tokens into parameters for
//! command execution.
class CommandLine
{
public:
    // Construction/Destruction
    CommandLine();

    // Accessors
    Command getCommand() const;
    const std::string &getInputFile() const;
    const std::string &getExecutableFile() const;
    const std::string &getOutputFile() const;
    const std::string &getWorkingFolder() const;

    // Operations
    static void displayHelp();
    bool tryParse(int argc, const char *argv[], std::string &error);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    std::string _outputFile;
    std::string _inputFile;
    std::string _exeFile;
    std::string _workingFolder;
    Command _command;
};

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

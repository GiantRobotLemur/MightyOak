//! @file PdbFileReader.hpp
//! @brief The declaration of an object which can read symbols from a Microsoft
//! Program Database file.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SYMBOL_PACKAGER_PDB_FILE_READER_HPP__
#define __AG_SYMBOL_PACKAGER_PDB_FILE_READER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class CommandLine;
class SymbolDb;

//! @brief An object which can read symbols from a Microsoft Program Database
//! (.PDB) file.
class PdbFileReader
{
public:
    // Construction/Destruction
    PdbFileReader(const CommandLine &args);

    // Accessors

    // Operations
    void readSymbols(SymbolDb &symbols, std::string &error);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    std::string _inputFile;
    std::string _exeFile;
};

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file GnuNmFileReader.hpp
//! @brief The declaration of an object which reads symbol information from the
//! output of the GNU bin utils command nm.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SYMBOL_PACKAGER_GNU_NM_FILE_READER_HPP__
#define __AG_SYMBOL_PACKAGER_GNU_NM_FILE_READER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class CommandLine;
class SymbolDb;

//! @brief An object which can read output produced by the GNU nm utility.
class GnuNmFileReader
{
public:
    // Construction/Destruction
    GnuNmFileReader(const CommandLine &args);

    // Operations
    void readSymbols(SymbolDb &symbols, std::string &error);
private:
    // Internal Fields
    std::string _inputFile;
};

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

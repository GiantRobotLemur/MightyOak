//! @file MsMapFileReader.hpp
//! @brief The declaration of an object which can read map files produced by the
//! Microsoft linker.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SYMBOL_PACKAGER_MS_MAP_FILE_READER_HPP__
#define __AG_SYMBOL_PACKAGER_MS_MAP_FILE_READER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class CommandLine;
class SymbolDb;

//! @brief An object which can read map files produced by the Microsoft linker.
class MsMapFileReader
{
public:
    // Construction/Destruction
    MsMapFileReader(const CommandLine &args);

    // Operations
    void readSymbols(SymbolDb &symbols, std::string &error);
private:
    // Internal Fields
    std::string _mapFilePath;
};

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

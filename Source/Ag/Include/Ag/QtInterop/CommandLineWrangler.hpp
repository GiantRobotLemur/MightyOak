//! @file Ag/QtInterop/CommandLineWrangler.hpp
//! @brief The declaration of a class which prevents character set conversion
//! errors in Qt command line argument processing.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __QT_INTEROP_COMMAND_LINE_WRANGLER_HPP__
#define __QT_INTEROP_COMMAND_LINE_WRANGLER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include <map>
#include <vector>

#include <QStringList>

namespace Ag {
namespace Qt {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A class which prevents character set conversion errors in Qt command
//! line argument processing.
class CommandLineWrangler
{
public:
    // Construction/Destruction
    CommandLineWrangler() = delete;
    CommandLineWrangler(int argc, char **argv);
    ~CommandLineWrangler() = default;

    // Accessors
    int &getNarrowArgumentCount();
    char **getNarrowArguments() const;
    QStringList getWideArguments() const;

    // Operations
    void clear();

private:
    // Internal Fields
#ifdef _WIN32
    std::vector<char> _narrowBuffer;
    std::vector<char *> _narrowArgVector;
    std::map<char *, QString> _wideArgsByNarrowPtr;
#endif // ifdef _WIN32

    int _narrowArgCount;
    char **_narrowArgs;
};

}} // namespace Ag::Qt

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

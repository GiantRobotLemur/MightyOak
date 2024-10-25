//! @file InputContext.hpp
//! @brief The declaration of an object handling input from a specific source.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_INPUT_CONTEXT_HPP__
#define __ASM_TOOLS_INPUT_CONTEXT_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "InputSet.hpp"
#include "AsmTools/Messages.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object handling input from a specific source.
class InputContext
{
public:
    // Construction/Destruction
    InputContext(IInputSourcePtr &source, const Location &sourceReference,
                 const Ag::String &sourceId, size_t lexerStackLevel);

    // Accessors
    size_t getBaseLexerStackLevel() const;
    const Location &getCurrentLocation() const;

    // Operations
    bool tryGetNextCharacter(char32_t &next);
    bool ensureNextCharacter(char32_t expected);
    void ungetCharacter();
    void breakInputStream();
private:
    // Internal Functions
    bool tryRefillBuffer();

    // Internal Fields
    IInputSourcePtr _source;
    Location _reference;
    Location _position;
    std::vector<char32_t> _buffer;
    size_t _bufferMin;
    size_t _bufferPos;
    size_t _lexerStackLevel;
};

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

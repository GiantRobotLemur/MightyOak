//! @file LineTokeniser.hpp
//! @brief The declaration of an object which splits a line of text into bounded
//! tokens.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_SYMPKG_LINE_TOKENISER_HPP__
#define __AG_SYMPKG_LINE_TOKENISER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include <initializer_list>
#include <vector>

#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class LineSignature
{
public:
    // Construction/Destruction
    LineSignature(const std::initializer_list<BoundedString> &elements);

    // Accessors
    size_t getElementCount() const;
    const BoundedString &getElement(size_t index) const;
private:
    // Internal Fields
    std::vector<BoundedString> _elements;
};


//! @brief An object which splits a line of text into bounded tokens.
class LineTokeniser
{
public:
    // Construction/Destruction
    LineTokeniser();

    // Accessors
    const std::string &getLine() const;
    size_t getTokenCount();
    const BoundedString &getToken(size_t index) const;
    BoundedString getTail(size_t index) const;
    bool startsWith(const BoundedString &prefix) const;
    bool matches(const LineSignature &signature) const;
    bool find(const BoundedString &subString, size_t startToken, size_t &pos) const;

    // Operations
    bool tryReadLine(FILE *inputStream);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    std::string _sourceLine;
    std::vector<BoundedString> _tokens;
};

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

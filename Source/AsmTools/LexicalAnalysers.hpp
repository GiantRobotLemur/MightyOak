//! @file LexicalAnalysers.hpp
//! @brief The declaration of functions to get various ILexicalContext
//! implementations.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_LEXICAL_ANALYSERS_HPP__
#define __ASM_TOOLS_LEXICAL_ANALYSERS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ParseContext.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
ILexicalContext *getStatementLexer();
ILexicalContext *getExpressionLexer();
ILexicalContext *getCoreRegListLexer();
ILexicalContext *getPsrNameLexer();
ILexicalContext *getPsrComponentLexer();

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

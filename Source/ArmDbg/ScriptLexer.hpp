//! @file ArmDbg/ScriptLexer.hpp
//! @brief The declaration of a tokenizer for debugger script lines.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DBG_SCRIPT_LEXER_HPP__
#define __ARM_DBG_SCRIPT_LEXER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <string_view>
#include <vector>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Represents a single token extracted from a script line.
struct ScriptToken
{
    std::string_view Text;   //!< The token text (view into original line).
    size_t Column;           //!< 0-based column position in the line.
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Tokenizes a single script line into whitespace-separated tokens.
class ScriptLexer
{
public:
    //! @brief Tokenizes a single line of script text.
    //! @param[in] line The line text to tokenize.
    //! @param[out] tokensOut Receives the extracted tokens.
    //! @retval true The line contained tokens.
    //! @retval false The line was blank or a comment.
    static bool tokenizeLine(const std::string_view &line,
                             std::vector<ScriptToken> &tokensOut);
};

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

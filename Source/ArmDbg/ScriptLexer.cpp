//! @file ArmDbg/ScriptLexer.cpp
//! @brief The definition of a tokenizer for debugger script lines.
//! @author GiantRobotLemur@na-se.co.uk and Claude Code
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ScriptLexer.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// ScriptLexer Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Tokenizes a single line of script text.
//! @param[in] line The line text to tokenize.
//! @param[out] tokensOut Receives the extracted tokens.
//! @retval true The line contained tokens.
//! @retval false The line was blank or a comment.
bool ScriptLexer::tokenizeLine(const std::string_view &line,
                               std::vector<ScriptToken> &tokensOut)
{
    tokensOut.clear();

    // Skip leading whitespace.
    size_t pos = 0;
    size_t len = line.size();

    while (pos < len && (line[pos] == ' ' || line[pos] == '\t'))
        ++pos;

    // Empty or comment line.
    if (pos >= len || line[pos] == '#')
        return false;

    // Tokenize by whitespace.
    while (pos < len)
    {
        // Skip whitespace between tokens.
        while (pos < len && (line[pos] == ' ' || line[pos] == '\t'))
            ++pos;

        if (pos >= len || line[pos] == '#')
            break;

        size_t start = pos;

        // Check if this is the "echo" command — everything after "echo "
        // becomes a single token.
        if (tokensOut.size() == 1)
        {
            // Case-insensitive check for "echo" as the first token.
            const auto &firstToken = tokensOut[0].Text;

            if (firstToken.size() == 4 &&
                (firstToken[0] == 'e' || firstToken[0] == 'E') &&
                (firstToken[1] == 'c' || firstToken[1] == 'C') &&
                (firstToken[2] == 'h' || firstToken[2] == 'H') &&
                (firstToken[3] == 'o' || firstToken[3] == 'O'))
            {
                // Everything remaining is the echo message.
                tokensOut.push_back({ line.substr(start), start });
                return true;
            }
        }

        // Scan to end of token.
        while (pos < len && line[pos] != ' ' && line[pos] != '\t' &&
               line[pos] != '#')
        {
            ++pos;
        }

        tokensOut.push_back({ line.substr(start, pos - start), start });
    }

    return !tokensOut.empty();
}

////////////////////////////////////////////////////////////////////////////////
}} // namespace Mo::Arm

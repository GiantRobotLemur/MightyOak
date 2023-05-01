//! @file MsMapFileReader.cpp
//! @brief The definition of an object which can read map files produced by the
//! Microsoft linker.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <climits>
#include <cstdlib>

#include <algorithm>
#include <set>
#include <string>
#include <utility>

#include "CommandLine.hpp"
#include "LineTokeniser.hpp"
#include "MsMapFileReader.hpp"
#include "SymbolDb.hpp"
#include "Utils.hpp"

#ifdef _MSC_VER
// Define macros to minimise the definitions introduced by including windows.h.
#define WIN32_MEAN_AND_LEAN
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
#define NOMINMAX          // Macros min(a,b) and max(a,b)
#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions
#include <Windows.h>
#include <DbgHelp.h>

//! @brief Undecorates a C++ symbol.
//! @param[in] outputString An optional buffer to receive the undecorated result.
//! @param[in] name The symbol to undecorate.
//! @param[in] maxStringLength The size of the outputString buffer.
//! @param[in] pAlloc A pointer to a malloc()-like function which can allocate
//! a buffer for the result if outputString is nullptr.
//! @param[in] pFree A pointer to a free()-like function which can release memory
//! allocated by the function passed to pAlloc.
//! @param[in] disableFlags The UNDNAME_* flags compatible with the
//! UnDecorateSymbolName() function exported by DbgHelp.dll.
//! @note The function is an undocumented and exists within the MSVCRT.
extern "C"
char * __unDName(char * outputString,
                 const char * name,
                 int maxStringLength,
                 void * (* pAlloc)(size_t),
                 void (* pFree )(void *),
                 unsigned short disableFlags);

void undecorateSymbol(SymbolEntry &entry)
{
    const unsigned short flags = UNDNAME_NO_ACCESS_SPECIFIERS |
                                 UNDNAME_NO_MS_KEYWORDS |
#if SIZE_MAX <= 0xFFFFFFFF
                                 UNDNAME_32_BIT_DECODE |
#endif
                                 UNDNAME_NO_MEMBER_TYPE;

    char *undecorated = __unDName(nullptr, entry.Symbol.c_str(), 0,
                                  &std::malloc, &std::free,
                                  flags);

    if (undecorated != nullptr)
    {
        entry.Symbol.assign(undecorated);
        std::free(undecorated);
    }

    if (entry.Symbol.find('(') == std::string::npos)
    {
        // The symbol has no parameter list parenthesis, add them.
        entry.Symbol.push_back('(');
        entry.Symbol.push_back(')');
    }
}
#else
void undecorateSymbol(SymbolEntry & /* entry */) {}
#endif

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
enum ParseState
{
    Parse_Start,
    Parse_AfterTimestamp,
    Parse_AfterPreferredLoadAddress,
    Parse_SectionTable,
    Parse_SymbolTable,
    Parse_Complete,
};


////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
bool tryParseAddress(const BoundedString &token, uint16_t &sectionId,
                     uint32_t &offset)
{
    char *endPos = nullptr;
    auto value = std::strtoul(token.Text, &endPos, 16);
    bool hasValue = false;

    if (value <= 0xFFFF)
    {
        size_t currentOffset = endPos - token.Text;
        sectionId = static_cast<uint16_t>(value);

        for (; currentOffset < token.Length; ++currentOffset)
        {
            if (std::isxdigit(token.Text[currentOffset]))
            {
                // It's a hex digit, try to convert the value.
                value = std::strtoul(token.Text + currentOffset,
                                     nullptr, 16);

                if (value == ULONG_MAX)
                {
                    // The value was out of range.
                    offset = 0;
                }
                else
                {
                    // The value was (probably) valid.
                    offset = static_cast<uint32_t>(value);
                    hasValue = true;
                }

                // Terminate the loop.
                currentOffset = token.Length;
            }
        }
    }
    else
    {
        sectionId = 0;
        offset = 0;
    }

    return hasValue;
}

//! @brief Parses the contents of a map file created by the MSVC linker.
//! @param[in] input The text input stream to read.
//! @param[out] result The symbol database to be updated from the contents of
//! the map file.
void parseFile(FILE *input, SymbolDb &result)
{
    const LineSignature timestampSig = { "Timestamp", "is" };
    const LineSignature loadAddrSig = { "Preferred", "load", "address", "is" };
    const LineSignature sectionTableSig = { "Start", "Length", "Name", "Class" };
    const LineSignature symbolTableSig = { "Address", "Publics", "by", "Value",
                                           "Rva+Base", "Lib:Object" };
    const LineSignature staticSymbolSig = { "Static", "symbols" };
    const BoundedString codeSectionSig("CODE");

    LineTokeniser line;
    ParseState state = Parse_Start;
    std::vector<uint16_t> codeSectionIds;
    uint64_t preferredBase = 0;
    uint16_t sectionId;
    uint32_t offset;

    while ((state != Parse_Complete) && line.tryReadLine(input))
    {
        switch (state)
        {
        case Parse_Start:
            if (line.matches(timestampSig))
            {
                state = Parse_AfterTimestamp;
            }
            break;

        case Parse_AfterTimestamp:
            if (line.matches(loadAddrSig))
            {
                state = Parse_AfterPreferredLoadAddress;

                if (tryParseHex(line.getToken(4).Text, preferredBase) == false)
                {
                    preferredBase = 0;
                }
            }
            break;

        case Parse_AfterPreferredLoadAddress:
            if (line.matches(sectionTableSig))
            {
                state = Parse_SectionTable;
            }
            break;

        case Parse_SectionTable:
            if (line.matches(symbolTableSig))
            {
                state = Parse_SymbolTable;

                // Compact the table of section IDs which refer to code.
                std::sort(codeSectionIds.begin(), codeSectionIds.end());
                auto endId = std::unique(codeSectionIds.begin(),
                                         codeSectionIds.end());
                codeSectionIds.erase(endId, codeSectionIds.end());
            }
            else if ((line.getTokenCount() > 3) &&
                     tryParseAddress(line.getToken(0), sectionId, offset))
            {
                if (line.getToken(3) == codeSectionSig)
                {
                    codeSectionIds.push_back(sectionId);
                }
            }
            break;

        case Parse_SymbolTable:
            if (line.matches(staticSymbolSig))
            {
                // We have finished.
                state = Parse_Complete;
            }
            else if ((line.getTokenCount() > 3) &&
                     tryParseAddress(line.getToken(0), sectionId, offset) &&
                     std::binary_search(codeSectionIds.begin(),
                                        codeSectionIds.end(), sectionId))
            {
                // Its a symbol entry.
                uint64_t address;
                if (tryParseHex(line.getToken(2).Text, address))
                {
                    SymbolEntry symbol(address - preferredBase, line.getToken(1));

                    undecorateSymbol(symbol);

                    result.addSymbol(symbol);
                }
            }
            break;

        case Parse_Complete:
        default:
            break;
        }
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which will read symbols from a Microsoft map file.
//! @param[in] args The arguments passed to the tool on the command line.
MsMapFileReader::MsMapFileReader(const CommandLine &args) :
    _mapFilePath(args.getInputFile())
{
}

//! @brief Reads the symbols from the map file specified on the command line.
//! @param[out] symbols The object to receive the symbol data.
//! @param[out] error Receives details of any failure which occurred, it empty
//! if the operation was successful.
void MsMapFileReader::readSymbols(SymbolDb &symbols, std::string &error)
{
    StdFilePtr mapFile;
    error.clear();

    if (tryOpenFile(_mapFilePath.c_str(), "r", mapFile))
    {
        parseFile(mapFile.get(), symbols);
    }
    else
    {
        appendFormat(error, "Failed to open map file '%s'.",
                     _mapFilePath.c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////


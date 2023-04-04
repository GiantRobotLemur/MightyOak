//! @file PdbFileReader.cpp
//! @brief The definition of an object which can read symbols from a Microsoft
//! Program Database file.
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
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

#define _NO_CVCONST_H     // Ensure the SymTagEnum numeration value in dbhhelp.h
                          // are defined.

#include <Windows.h>
#include <DbgHelp.h>

// Link to dbghelp.dll.
#pragma comment(lib, "Dbghelp.lib")

//#include <dia2.h>
//
//#pragma comment(lib, "diaguids.lib")

#include "CommandLine.hpp"
#include "PdbFileReader.hpp"
#include "SymbolDb.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Looks up a Win32 error code and appends it to a UTF-8 string.
//! @param[out] destination The STL string to append to.
//! @param[in] errorCode The error code to look up, possibly from GetLastError().
void appendWin32Error(std::string &destination, DWORD errorCode)
{
    char *messageBuffer = nullptr;

    DWORD length = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                                    FORMAT_MESSAGE_FROM_SYSTEM |
                                    FORMAT_MESSAGE_IGNORE_INSERTS,
                                    NULL, errorCode,
                                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                    reinterpret_cast<LPSTR>(&messageBuffer),
                                    0, NULL);

    if (length > 0)
    {
        destination.append(messageBuffer, length);

        ::LocalFree(messageBuffer);
    }
}

//! @brief Formats an error message based on the failure of a Win32 function.
//! @param[out] destination The string to receive the error message.
//! @param[in] message The text to appear before the derived error message.
//! @param[in] errorCode The value used to derived an error message. If
//! ERROR_SUCCESS (0) is specified, GetLastError() will be called internally.
void win32Error(std::string &destination, const char *message, DWORD errorCode = 0)
{
    if (errorCode == 0)
    {
        errorCode = ::GetLastError();
    }

    destination.append(message);
    destination.push_back(':');
    destination.push_back(' ');
    appendWin32Error(destination, errorCode);
}

//! @brief Strips the file name part of a path to leave the directory path.
//! @param[in] source The path to analyse.
//! @param[out] path Receives the source path with the leaf element removed.
void getDirectoryPath(const std::string &source, std::string &path)
{
    size_t separatorPos = source.find_last_of("\\/");

    if (separatorPos == std::string::npos)
    {
        path.assign(".");
    }
    else
    {
        path.assign(source, 0, separatorPos);
    }
}

//! @brief Attempts to initialise the DbgHelp.dll library.
//! @param[out] hProcess Receives the handle to the process passed to the
//! initialisation functions.
//! @param[in] exePath The full path to the PE executable to get symbols for.
//! @param[in] pdbPath The full path to the PDB file corresponding to the
//! executable.
//! @param[out] error Receives an error message if the operation failed.
//! @retval true DbgHelp.dll was successfully initialised.
//! @retval false The operation failed, error was updated with details.
bool initialiseDebugTools(HANDLE &hProcess, const std::string &exePath,
                          const std::string &pdbPath, std::string &error)
{
    bool isOK = true;
    ::SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS); // SYMOPT_ALLOW_ZERO_ADDRESS
    hProcess = ::GetCurrentProcess();

    std::string exeDir, pdbDir;
    const char *searchPaths = nullptr;

    getDirectoryPath(exePath, exeDir);
    getDirectoryPath(pdbPath, pdbDir);

    if (_stricmp(exeDir.c_str(), pdbDir.c_str()) != 0)
    {
        searchPaths = pdbDir.c_str();
    }

    if (::SymInitialize(hProcess, searchPaths, FALSE) == FALSE)
    {
        // SymInitialize failed.
        hProcess = NULL;
        win32Error(error, "Failed to initialise DbgHelp.dll");
        isOK = false;
    }

    return isOK;
}

void replaceAllOccurrances(std::string &target, const char *oldStr, const char *newStr)
{
    size_t pos = target.find(oldStr);

    if (pos != std::string::npos)
    {
        std::string result;
        result.reserve(target.length() * 110 / 100);
        size_t oldStrLen = std::strlen(oldStr);
        size_t start = 0;

        while (pos != std::string::npos)
        {
            // Append text up to the recognised substring.
            result.append(target, start, pos - start);

            // Append the new text.
            result.append(newStr);

            // Prepare to find the next occurrence.
            start = pos + oldStrLen;
            pos = target.find(oldStr, start);
        }

        // Append the last of the unmodified text.
        result.append(target, start);

        // Assign the return value.
        target = result;
    }
}

//! @brief A callback function used to enumerate symbols in a PDB.
//! @param[in] symInfo The symbol being enumerated.
//! @param[in] userContext A pointer to a SymbolDb object.
//! @retval TRUE To continue enumerating.
//! @retval FALSE To stop any further enumeration.
BOOL enumeratePdbSymbols(PSYMBOL_INFO symInfo,
                         ULONG /* symbolSize */,
                         PVOID userContext)
{
    if (symInfo->Tag == SymTagFunction)
    {
        SymbolDb *db = static_cast<SymbolDb *>(userContext);

        std::string symbolName(symInfo->Name, symInfo->NameLen);

        // Fix up the symbol so that it matches __FUNCTION__, this helps
        // with unit testing of the stack trace functionality.
        replaceAllOccurrances(symbolName, "`anonymous namespace'",
                              "`anonymous-namespace'");

        if (symbolName.find('(') == std::string::npos)
        {
            symbolName.push_back('(');
            symbolName.push_back(')');
        }

        db->addSymbol(symInfo->Address - symInfo->ModBase, symbolName);
    }

    return TRUE;
}



} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object to extract symbols from a PDB file.
//! @param[in] args The command line arguments defining which file to read.
PdbFileReader::PdbFileReader(const CommandLine &args) :
    _inputFile(args.getInputFile()),
    _exeFile(args.getExecutableFile())
{
}

//! @brief Reads the symbols from the pre-defined PDF file and stores the result
//! in a symbol database.
//! @param[out] symbols The object to receive the symbol values.
//! @param[out] error A string to receive an error message if the operation fails.
void PdbFileReader::readSymbols(SymbolDb &symbols, std::string &error)
{
    HANDLE thisProcess;

    if (initialiseDebugTools(thisProcess, _exeFile, _inputFile, error) == false)
    {
        return;
    }

    // Load the PDB associated with the PE binary.
    auto baseAddr = ::SymLoadModuleEx(thisProcess, NULL, _exeFile.c_str(),
                                      NULL, 0, 0, NULL, 0);

    if (baseAddr == 0)
    {
        DWORD errorCode = ::GetLastError();
        std::string message;
        message.assign("Failed to load PDB file for '");
        message.append(_exeFile);
        message.push_back('\'');

        win32Error(error, message.c_str(), errorCode);

        ::SymCleanup(thisProcess);
        return;
    }

    if (::SymEnumSymbols(thisProcess, baseAddr, "*",
                         enumeratePdbSymbols, &symbols) == FALSE)
    {
        win32Error(error, "Failed to enumerate PDB symbols");
    }


    // Un-load the library before exit.
    ::SymUnloadModule64(thisProcess, baseAddr);
}
////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

#endif // ifdef _WIN32
////////////////////////////////////////////////////////////////////////////////


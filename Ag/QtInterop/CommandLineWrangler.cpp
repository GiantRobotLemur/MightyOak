//! @file Ag/QtInterop/CommandLineWrangler.cpp
//! @brief The definition of a class which prevents character set conversion
//! errors in Qt command line argument processing.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2024
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
#ifndef UNICODE
#define UNICODE
#endif // ifndef UNICODE

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
//#define NOKERNEL          // All KERNEL defines and routines
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

#include <algorithm>
#include <Windows.h>
#endif

#include "Ag/QtInterop/CommandLineWrangler.hpp"

namespace Ag {
namespace Qt {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

//! @brief Obtains the original Unicode command line text and breaks it into
//! tokens.
//! @return The Unicode tokens derived from the application command line.
QStringList getWideCommandLineArgs()
{
    auto wideCommandLine = ::GetCommandLineW();

    QStringList args;
    QString currentArg;
    bool isQuoted = false;
    size_t index = 0;

    do
    {
        bool flush = false;

        QChar next(wideCommandLine[index++]);

        if (next.isNull())
        {
            flush = true;
        }
        else
        {
            // Process the character.
            if (isQuoted)
            {
                if (next == L'"')
                {
                    flush = true;
                    isQuoted = false;
                }
                else
                {
                    currentArg.append(next);
                }
            }
            else if (next == L'"')
            {
                isQuoted = true;
                flush = true;
            }
            else if (next.isSpace())
            {
                flush = true;
            }
            else
            {
                currentArg.append(next);
            }
        }

        if (flush && (currentArg.isEmpty() == false))
        {
            args.append(currentArg);
            currentArg.clear();
        }

    } while (wideCommandLine[index - 1] != L'\0');

    return args;
}

#endif // ifdef _WIN32

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// CommandLineWrangler Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates an object to manage manipulation and conversion of the
//! command line arguments passed to main() and the constructor of
//! QCoreApplication.
//! @param[in] argc The count of command line argument tokens passed to main().
//! @param[in] argv The array of command line argument tokens passed to main().
//! @note Under Windows, the argc/argv values will be discarded in favour of
//! values derived from GetCommandLineW().
CommandLineWrangler::CommandLineWrangler(int argc, char **argv) :
    _narrowArgCount(argc),
    _narrowArgs(argv)
{
#ifdef _WIN32
    // Under Windows, get the original Unicode command line and make narrow
    // versions of each argument.
    QStringList wideArgs = getWideCommandLineArgs();
    std::vector<size_t> narrowOffsets;
    narrowOffsets.reserve(wideArgs.size());

    for (const QString &wideArg : wideArgs)
    {
        QByteArray narrowArg = wideArg.toUtf8();
        narrowOffsets.push_back(_narrowBuffer.size());

        // Copy the narrow encoding of the argument.
        std::copy(narrowArg.cbegin(), narrowArg.cend(),
                  std::back_inserter(_narrowBuffer));

        // Ensure it is null-terminated.
        _narrowBuffer.push_back('\0');
    }

    // Create a mapping of narrow to wide arguments.
    for (size_t i = 0; i < narrowOffsets.size(); ++i)
    {
        size_t narrowOffset = narrowOffsets[i];
        char *narrowEncoding = _narrowBuffer.data() + narrowOffset;
        const QString &wideEncoding = wideArgs[i];

        _narrowArgVector.push_back(narrowEncoding);
        _wideArgsByNarrowPtr.insert({ narrowEncoding, wideEncoding });
    }

    // Set the argc/argv values which the QCoreApplication constructor
    // can modify.
    _narrowArgCount = static_cast<int>(_narrowArgVector.size());
    _narrowArgs = _narrowArgVector.data();
#endif // ifdef _WIN32
}

//! @brief Gets a reference to the count of narrow command line argument
//! which can be passed to the constructor of QCoreApplication to modify.
int &CommandLineWrangler::getNarrowArgumentCount()
{
    return _narrowArgCount;
}

//! @brief Gets the array of narrow command line arguments which can be passed
//! to the constructor of QCoreApplication to modify.
char **CommandLineWrangler::getNarrowArguments() const
{
    return _narrowArgs;
}

//! @brief Gets the Unicode encodings of the command line arguments left after
//! modification via values returned by getNarrowArgumentCount() and
//! getNarrowArguments().
QStringList CommandLineWrangler::getWideArguments() const
{
    QStringList wideArgs;
    wideArgs.reserve(static_cast<qsizetype>(_narrowArgCount));

    for (int argi = 0; argi < _narrowArgCount; ++argi)
    {
        char *narrowArg = _narrowArgs[argi];

#ifdef _WIN32
        auto wideMapping = _wideArgsByNarrowPtr.find(narrowArg);

        if (wideMapping != _wideArgsByNarrowPtr.end())
        {
            wideArgs.append(wideMapping->second);
        }
#else
        wideArgs.append(QString::fromUtf8(narrowArg));
#endif
    }

    return wideArgs;
}

//! @brief Frees resources used command line arguments.
void CommandLineWrangler::clear()
{
#ifdef _WIN32
    _wideArgsByNarrowPtr.clear();
    _narrowArgVector.clear();
    _narrowBuffer.clear();
#endif // ifdef _WIN32

    _narrowArgCount = 0;
    _narrowArgs = nullptr;
}

}} // namespace Ag::Qt
////////////////////////////////////////////////////////////////////////////////

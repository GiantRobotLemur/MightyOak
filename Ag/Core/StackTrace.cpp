//! @file Core/StackTrace.cpp
//! @brief The definition of an object used to gather a stack trace.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <cstdio>

#include <algorithm>
#include <vector>

#include "Ag/Core/InlineMemory.hpp"
#include "Ag/Core/StackTrace.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Private/SymbolEncoding.hpp"
#include "CoreInternal.hpp"
#include "Platform.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A structure representing a module used when a stack trace is packed.
struct TraceModule
{
    //! @brief The offset and size of the string representing the full path to
    //! the module binary file.
    InlineField FilePath;

    //! @brief The base address of the module as it is loaded into the process.
    uintptr_t BaseAddress;

    //! @brief The offset into the full path string of the module file name.
    size_t FileNameOffset;
};

//! @brief A structure representing a stack activation record used when a stack
//! trace is packed.
struct TraceRecord
{
    //! @brief The offset and size of the string representing the function name.
    InlineField FunctionName;

    //! @brief The offset of the function call within the target module.
    uintptr_t Offset;

    //! @brief The 0-based index of the module containing the function call.
    size_t ModuleOrdinal;
};

//! @brief A structure which is at the head of a self-contained block of memory
//! defining a stack trace.
struct StackTracePrivate
{
    //! @brief The total size of the data structure and all in-line data in bytes.
    size_t TotalSize;

    //! @brief The offset and count of modules referenced in the stack trace.
    InlineField Modules;

    //! @brief The offset and count of function activation record elements in the
    //! stack trace.
    InlineField Entries;

    // TraceModule Modules[ModuleCount.Count];
    // TraceRecord Entries[EntryCount.Count];
    // Strings follow which are referenced in the Modules and Entries array by
    // offset from the beginning of the data structure.

    // Accessors

    //! @brief Gets a module definition stored in-line with the current object.
    //! @param[in] index The 0-based index of the module.
    //! @note No bounds checking is performed.
    const TraceModule *getModule(size_t index) const
    {
        return offsetPtr<TraceModule>(this, Modules.Offset + (sizeof(TraceModule) * index));
    }

    //! @brief Gets a stack trace record stored in-line with the current object.
    //! @param[in] index The 0-based index of the record.
    //! @note No bounds checking is performed.
    const TraceRecord *getEntry(size_t index) const
    {
        return offsetPtr<TraceRecord>(this, Entries.Offset + (sizeof(TraceRecord) * index));
    }

    //! @brief Gets a string stored in-line with the current object.
    //! @param[in] field The object describing the position and size of the string.
    //! @note No bounds checking is performed.
    std::string_view getString(const InlineField &field) const
    {
        return std::string_view(offsetPtr<char>(this, field.Offset), field.Count);
    }
};

namespace {

//! @brief Represents a stack trace element used for the purpose of resolving
//! module and function symbols.
struct TraceElement
{
    //! @brief The original activation record captured from the call stack.
    ActivationRecord Record;

    //! @brief The 0-based index of the element as it was captured from the call stack.
    size_t SequenceNo;

    //! @brief The ordinal of the string representing the function name.
    size_t SymbolOrdinal;

    //! @brief The 0-based index of the module which contained the active function.
    size_t ModuleOrdinal;

    TraceElement(const ActivationRecord &record, size_t sequence) :
        Record(record),
        SequenceNo(sequence),
        SymbolOrdinal(0),
        ModuleOrdinal(0)
    {
    }
};

typedef std::vector<TraceElement> TraceElements;

//! @brief An object representing a module referenced in a stack trace.
struct ModuleElement
{
    //! @brief The base address of the module in the current process.
    uintptr_t ModuleBase;

    //! @brief The ordinal of the string defining the full path to the module file.
    size_t FilePathOrdinal;

    //! @brief The offset within the path string to the module file name.
    size_t FileNameOffset;

    //! @brief Constructs a module with just a base address.
    ModuleElement(uintptr_t moduleBase) :
        ModuleBase(moduleBase),
        FilePathOrdinal(~0u),
        FileNameOffset(0)
    {
    }

    //! @brief Constructs a module with a base address and file path.
    ModuleElement(uintptr_t moduleBase,
                  size_t pathOrdinal,
                  size_t nameOffset) :
        ModuleBase(moduleBase),
        FilePathOrdinal(pathOrdinal),
        FileNameOffset(nameOffset)
    {
    }
};

typedef std::vector<ModuleElement> ModuleElements;

//! @brief A structure representing a string referenced in a stack trace.
struct StringElement
{
    //! @brief The text data.
    std::string Text;

    //! @brief The byte offset to the text data from the beginning of the packed
    //! string table.
    size_t Offset;
};

typedef std::vector<StringElement> StringElements;

//! @brief An object which can compare one or more size_t fields within a
//! data structure.
//! @tparam T The data structure to compare.
template<typename T>
struct CompareByWord
{
private:
    static const size_t MaxFields = 4;
    size_t _offsets[MaxFields];
    size_t _fieldCount;
public:
    //! @brief Constructs an object which compares a single field.
    //! @param[in] offsetToWord The byte offset of the word to compare from the
    //! beginning of the data structure, generally obtained with the
    //! offsetof() macro.
    CompareByWord(size_t offsetToWord) :
        _fieldCount(1)
    {
        _offsets[0] = offsetToWord;

        std::fill(_offsets + _fieldCount, _offsets + MaxFields, 0);
    }

    //! @brief Constructs an object which compares a pair of fields.
    //! @param[in] offsetToWord1 The byte offset of the first word to compare
    //! from the beginning of the data structure, generally obtained with the
    //! offsetof() macro.
    //! @param[in] offsetToWord2 The byte offset of the second word to compare.
    CompareByWord(size_t offsetToWord1, size_t offsetToWord2) :
        _fieldCount(2)
    {
        _offsets[0] = offsetToWord1;
        _offsets[1] = offsetToWord2;

        std::fill(_offsets + _fieldCount, _offsets + MaxFields, 0);
    }

    bool operator()(const T &lhs, const T &rhs) const
    {
        bool isLessThan = false;

        for (size_t index = 0; index < _fieldCount; ++index)
        {
            const size_t *lhsValue = offsetPtr<size_t>(&lhs, _offsets[index]);
            const size_t *rhsValue = offsetPtr<size_t>(&rhs, _offsets[index]);

            if (*lhsValue != *rhsValue)
            {
                // We have found a difference. Perform the comparison and
                // terminate the loop.
                isLessThan = *lhsValue < *rhsValue;
                index = _fieldCount;
            }
        }

        return isLessThan;
    }
};

//! @brief An object which compares pointers to TraceElement objects by their
//! SymbolOrdinal field.
struct CompareBySymbolOrdinal
{
    bool operator()(const TraceElement *lhs, const TraceElement *rhs) const
    {
        bool isLess = false;

        if (lhs == nullptr)
        {
            isLess = (rhs != nullptr);
        }
        else if (rhs != nullptr)
        {
            isLess = lhs->SymbolOrdinal < rhs->SymbolOrdinal;
        }

        return isLess;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to read a number of bytes from an stdio stream.
//! @param[in] ptr The unique pointer to the open stream.
//! @param[out] buffer The buffer to read bytes into.
//! @param[in] byteCount The count of bytes to read.
//! @retval All the required bytes were successfully read.
//! @retval The read failed or the end of the stream was unexpectedly encountered.
bool tryRead(StdFilePtr &ptr, void *buffer, size_t byteCount)
{
    bool isRead = false;
    if (ptr)
    {
        size_t bytesRead = fread(buffer, 1, byteCount, ptr.get());

        isRead = (bytesRead == byteCount);
    }

    return isRead;
}

//! @brief Adds an empty string to a string table.
//! @param[in] strings The string table to add to.
//! @param[out] ordinal Receives the ordinal of the newly added string.
//! @returns A reference to the newly added string.
std::string &addString(StringElements &strings, size_t &ordinal)
{
    ordinal = strings.size();

    size_t offset = 0;

    if (strings.empty() == false)
    {
        offset = strings.back().Offset + strings.back().Text.length() + 1;
    }

    strings.emplace_back();
    strings.back().Offset = offset;

    return strings.back().Text;
}

#ifdef _WIN32
//! @brief Resolves the set of modules referenced in a stack trace.
//! @param[in,out] traces The function activation records which reference the
//! modules to resolve, updated with ordinals to modules created.
//! @param[out] modules A collection to be populated with module definitions.
//! @param[out] strings A string table to be populated with the text data the
//! new module definitions require.
void resolveModules(TraceElements &traces, ModuleElements &modules,
                    StringElements &strings)
{
    std::sort(traces.begin(), traces.end(),
              CompareByWord<TraceElement>(offsetof(TraceElement, Record.ModuleBase),
                                          offsetof(TraceElement, Record.Offset)));

    // Count the modules referenced by the stack trace.
    size_t moduleCount = 0;
    uintptr_t lastBase = 0;

    for (TraceElement &element : traces)
    {
        if (element.Record.ModuleBase != lastBase)
        {
            ++moduleCount;
            lastBase = element.Record.ModuleBase;
        }
    }

    modules.reserve(moduleCount);
    std::vector<wchar_t> wideBuffer;
    wideBuffer.resize(256, L'\0');
    size_t lastModule = ~0u;
    lastBase = ~0u;

    for (TraceElement &element : traces)
    {
        if (element.Record.ModuleBase != lastBase)
        {
            // Attempt to resolve the module.
            lastBase = element.Record.ModuleBase;

            // Attempt to get the module file name as Unicode text into the
            // existing buffer.
            size_t length = ::GetModuleFileNameW(reinterpret_cast<HMODULE>(lastBase),
                                                 wideBuffer.data(),
                                                 static_cast<DWORD>(wideBuffer.size()));

            if (length == 0)
            {
                // The file name cannot be resolved.
                modules.emplace_back(lastBase);
                lastModule = ~0u;
            }
            else
            {
                lastModule = modules.size();

                while (length == wideBuffer.size())
                {
                    // The buffer wasn't large enough. Resize and try again.
                    wideBuffer.resize(wideBuffer.size() * 2, L'\0');

                    length = ::GetModuleFileNameW(reinterpret_cast<HMODULE>(lastBase),
                                                  wideBuffer.data(),
                                                  static_cast<DWORD>(wideBuffer.size()));
                }

                // Convert the wide character string to UTF-8, note the ANSI version
                // GetModuleFileNameA(), would not have encoded the path name as UTF-8.
                size_t ordinal;
                std::string &filePath = addString(strings, ordinal);

                Utf::appendWide(filePath, wideBuffer.data(), length);

                // Find the leaf file name within the path string.
                size_t offset = filePath.find_last_of("\\/");

                if (offset == std::string::npos)
                {
                    // There were no directories in the path string.
                    offset = 0;
                }
                else
                {
                    // Move past the directory separator.
                    ++offset;
                }

                // Add the module definition for re-use by all following symbols
                // which reference the same module base address.
                modules.emplace_back(lastBase, ordinal, offset);
            }
        }

        // Assign a module to the stack trace element.
        element.ModuleOrdinal = lastModule;
    }
}

//! @brief Attempts to find and open the file defining function symbols associated
//! with a specific binary module.
//! @param[in] moduleFilePath The full path to the binary module.
//! @returns Either a valid pointer to an open .sym file or nullptr if no symbols
//! associated with the module could be found.
FILE *findSymbolFile(const std::string &moduleFilePath)
{
    // Search appropriate folders for the .sym file and open it.
    // Perform the search in Unicode as the ANSI version of functions will not treat
    // 8-bit character strings as UTF-8.
    std::wstring symbolFile;
    symbolFile.reserve(moduleFilePath.length() + 4);

    size_t dotPos = moduleFilePath.rfind('.');

    if (dotPos == std::string::npos)
    {
        // Copy the entire file path.
        Utf::appendToWide(symbolFile, moduleFilePath.c_str(),
                          moduleFilePath.length());
    }
    else
    {
        // Copy the module file path up to the extension.
        Utf::appendToWide(symbolFile, moduleFilePath.c_str(), dotPos);
    }

    // Change the extension.
    symbolFile.append(L".sym");

    FILE *fp = nullptr;

    if (_wfopen_s(&fp, symbolFile.c_str(), L"rb") != 0)
    {
        fp = nullptr;
    }

    return fp;
}

#else
// The POSIX implementation of resolveModules() and findSymbolFile().

//! @brief Resolves the set of modules referenced in a stack trace.
//! @param[in,out] traces The function activation records which reference the
//! modules to resolve, updated with ordinals to modules created.
//! @param[out] modules A collection to be populated with module definitions.
//! @param[out] strings A string table to be populated with the text data the
//! new module definitions require.
void resolveModules(TraceElements &traces, ModuleElements &modules,
                    StringElements &strings)
{
    // Sort the stack trace elements by their absolute address. Under Linux
    // this is held in the Record.Offset field until we have grouped entries
    // into modules and separated Module Base Address and Offset.
    std::sort(traces.begin(), traces.end(),
              CompareByWord<TraceElement>(offsetof(TraceElement, Record.Offset)));

    const size_t BufferSize = 32;
    char fileNameBuffer[BufferSize];
    snprintf(fileNameBuffer, BufferSize, "/proc/%u/maps", getpid());

    // Read /proc/<pid>/maps to get a table of binaries loaded into the process.
    StdFilePtr memoryMaps(fopen(fileNameBuffer, "r"));

    if (memoryMaps)
    {
        std::string lineBuffer;
        std::vector<StringRange> tokens;
        auto element = traces.begin();
        auto elementEnd = traces.end();

        // Read the file line by line.
        while ((element != elementEnd) &&
               tryReadLine(memoryMaps, lineBuffer))
        {
            // Split the line into tokens.
            tokeniseLine(lineBuffer, tokens);

            if (tokens.size() > 5)
            {
                // The schema should be:
                // Address Range, Permissions, device, inode, file path.
                std::string_view perms = getLineToken(lineBuffer, tokens[1]);

                if (perms.find('x') != std::string_view::npos)
                {
                    // The memory map has executable permissions.
                    std::string_view addrRange = getLineToken(lineBuffer,
                                                              tokens[0]);
                    char *nextToken;
                    uint64_t start = std::strtoull(addrRange.data(),
                                                   &nextToken, 16);
                    uint64_t end = start;

                    if (nextToken != nullptr)
                    {
                        ++nextToken;

                        end = std::strtoull(nextToken, nullptr, 16);
                    }

                    // Read the offset of the section into the module as all
                    // symbols are stored relative to the .text section of
                    // the EXE.
                    std::string_view offsetToken = getLineToken(lineBuffer,
                                                                tokens[2]);
                    uint64_t sectionOffset = std::strtoull(offsetToken.data(),
                                                           nullptr, 16);

                    if ((sectionOffset != ULLONG_MAX) && (start != ULLONG_MAX) &&
                        (end != ULLONG_MAX) && (start < end) &&
                        (element->Record.Offset < end))
                    {
                        // Create a module and use it to annotate all
                        // symbols within it.
                        size_t moduleId = modules.size();
                        size_t moduleNameId = 0;
                        size_t nameOffset = 0;

                        std::string &moduleFilePath = addString(strings, moduleNameId);
                        moduleFilePath.assign(lineBuffer, tokens[5].first);

                        nameOffset = moduleFilePath.rfind('/');
                        if (nameOffset == std::string::npos)
                        {
                            nameOffset = 0;
                        }
                        else
                        {
                            ++nameOffset;
                        }

                        modules.emplace_back(start, moduleNameId, nameOffset);

                        while ((element != elementEnd) && (element->Record.Offset < end))
                        {
                            // Convert the absolute address into an offset into a module.
                            element->Record.ModuleBase = start - sectionOffset;
                            element->Record.Offset -= element->Record.ModuleBase;
                            // element->Record.Offset += sectionOffset;
                            element->ModuleOrdinal = moduleId;

                            // Move on to the next element.
                            ++element;
                        }
                    }
                }
            }
        }

        // Resolve the symbols we couldn't assign to a module.
        while (element != elementEnd)
        {
            element->ModuleOrdinal = ~0u;
        }
    }
}

//! @brief Attempts to find and open the file defining function symbols associated
//! with a specific binary module.
//! @param[in] moduleFilePath The full path to the binary module.
//! @returns Either a valid pointer to an open .sym file or nullptr if no symbols
//! associated with the module could be found.
FILE *findSymbolFile(const std::string &moduleFilePath)
{
    // Search appropriate folders for the .sym file and open it.
    std::string symbolFile;
    symbolFile.reserve(moduleFilePath.length() + 4);

    size_t dotPos = moduleFilePath.rfind('.');

    if (dotPos == std::string::npos)
    {
        // Copy the entire file path.
        symbolFile.assign(moduleFilePath);
    }
    else
    {
        // Copy the module file path up to the extension.
        symbolFile.assign(moduleFilePath, 0, dotPos);
    }

    // Change the extension.
    symbolFile.append(".sym");

    FILE *fp = fopen(symbolFile.c_str(), "rb");

    return fp;
}
#endif

//! @brief Resolve the symbols in the stack trace which reference a specific module.
//! @param[in] moduleFilePath The path to the module containing the symbols to resolve.
//! @param[in] begin A pointer to the first stack trace element which references
//! the module.
//! @param[in] end A pointer to the stack trace element after the last one which
//! references the module.
//! @note stack trace elements are ordered by their offset within the module.
void resolveSymbols(const std::string &moduleFilePath, StringElements &stringTable,
                    TraceElements::iterator begin, TraceElements::iterator end)
{
    StdFilePtr symbolFile(findSymbolFile(moduleFilePath));

    if (symbolFile)
    {
        SymbolHeader fileData;

        if (tryRead(symbolFile, &fileData, sizeof(fileData)) &&
            (std::memcmp(fileData.Header.Signature, SYMBOL_SIGNATURE,
                         sizeof(fileData.Header.Signature)) == 0) &&
            (fileData.Header.Version[0] == 1) &&
            (fileData.Header.Version[1] == 0) &&
            (fileData.Header.Version[2] == 0) &&
            (fileData.Header.Version[3] == 0))
        {
            // The file is valid.
            PackedFieldHelper symbolFields({ fileData.SymbolOffsetBitCount,
                                             fileData.SymbolOrdinalBitCount });
            std::vector<TraceElement *> elementsBySymbol;
            elementsBySymbol.reserve(std::distance(begin, end));

            TraceElements::iterator current = begin;
            uintptr_t currentOffset = static_cast<uintptr_t>(fileData.InitialOffset);
            size_t prevOrdinal = ~0u;
            bool isOK = true;

            // Stream through the contents only retaining the data we need.
            for (uint32_t index = 0; isOK && (index < fileData.SymbolCount); ++index)
            {
                if (symbolFields.read(symbolFile.get()))
                {
                    currentOffset += symbolFields.getField<uintptr_t>(0);
                    size_t ordinal = symbolFields.getField<size_t>(1);

                    while ((current != end) && (current->Record.Offset < currentOffset))
                    {
                        // The entry is beyond the one we were looking for,
                        // assign the previous one.
                        current->SymbolOrdinal = prevOrdinal;

                        elementsBySymbol.push_back(&(*current));

                        ++current;
                    }

                    prevOrdinal = ordinal;
                }
                else
                {
                    isOK = false;
                }
            }

            // Assign all of the entries left to the last symbol read.
            while (current != end)
            {
                // The entry is beyond the one we were looking for,
                // assign the previous one.
                current->SymbolOrdinal = prevOrdinal;

                elementsBySymbol.push_back(&(*current));

                ++current;
            }

            if (isOK)
            {
                // Order the elements by the ordinal of the symbol within
                // the file so that we can apply the symbols as they are read in.
                std::sort(elementsBySymbol.begin(), elementsBySymbol.end(),
                          CompareBySymbolOrdinal());

                auto element = elementsBySymbol.begin();

                // Read the string data.
                PackedFieldHelper stringFields({ fileData.StringPrefixBitCount,
                                                 fileData.StringSuffixBitCount });
                std::vector<char> buffer;
                buffer.reserve(static_cast<size_t>(fileData.MaxStringLength) + 1);

                for (size_t ordinal = 0;
                     isOK && (ordinal < fileData.SymbolCount) && (element != elementsBySymbol.end());
                     ++ordinal)
                {
                    if (stringFields.read(symbolFile.get()))
                    {
                        size_t prefixSize = stringFields.getField<size_t>(0);
                        size_t suffixSize = stringFields.getField<size_t>(1);

                        buffer.resize(prefixSize + suffixSize);

                        if (tryRead(symbolFile, buffer.data() + prefixSize,
                                    suffixSize))
                        {
                            // We have the complete string.
                            if (((element != elementsBySymbol.end()) &&
                                 ((*element)->SymbolOrdinal == ordinal)))
                            {
                                // Create a symbol from the current contents
                                // of the buffer.
                                size_t symbolOrdinal = ~0u;

                                std::string &symbol = addString(stringTable,
                                                                symbolOrdinal);

                                symbol.assign(buffer.data(), prefixSize + suffixSize);

                                // Apply the symbol to all affected elements.
                                while ((element != elementsBySymbol.end()) &&
                                       ((*element)->SymbolOrdinal == ordinal))
                                {
                                    (*element)->SymbolOrdinal = symbolOrdinal;
                                    ++element;
                                }
                            }
                        }
                        else
                        {
                            isOK = false;
                        }
                    }
                    else
                    {
                        isOK = false;
                    }
                }
            }
        }
    }
    else
    {
        // Set all symbols to be empty.
        for (auto current = begin; current != end; ++current)
        {
            current->SymbolOrdinal = ~0u;
        }
    }
}

//! @brief Packs the stack trace data into a self-contained block of memory.
//! @param[in] element The resolved activation records to encode.
//! @param[in] modules The resolved module definitions to encode.
//! @param[in] stringTable The table of strings referenced by elements and modules.
//! @returns An object which the caller is responsible for disposing of with a
//! call to the destroyStackTrace() function.
StackTracePrivate *packStackTrace(const TraceElements &elements,
                                  const ModuleElements &modules,
                                  const StringElements &stringTable)
{
    StackTracePrivate *trace = nullptr;

    InlineAllocator allocator(sizeof(StackTracePrivate));

    InlineField moduleArray = allocator.allocateArray<TraceModule>(modules.size());
    InlineField entryArray = allocator.allocateArray<TraceRecord>(elements.size());
    InlineField stringArray;

    if (stringTable.empty() == false)
    {
        size_t stringTableSize = stringTable.back().Offset +
                                 stringTable.back().Text.length() + 1;

        stringArray = allocator.allocateRaw(stringTableSize);
    }

    void *block = std::malloc(allocator.getSize());
    trace = reinterpret_cast<StackTracePrivate *>(block);

    InlineInitialiser initialiser(block, allocator.getSize());

    // Initialise the header.
    trace->TotalSize = allocator.getSize();
    trace->Modules = moduleArray;
    trace->Entries = entryArray;

    size_t count = 0;
    TraceModule *packedModules = nullptr;

    // Initialise the packed array of modules.
    if (initialiser.tryGetSafeFieldData(moduleArray, packedModules, count))
    {
        // Replace the byte count with an element count.
        trace->Modules.Count = count;

        size_t maxCount = std::min(count, modules.size());

        for (size_t index = 0; index < maxCount; ++index)
        {
            const ModuleElement &module = modules[index];
            TraceModule &packedModule = packedModules[index];

            packedModule.BaseAddress = module.ModuleBase;

            if (module.FilePathOrdinal < stringTable.size())
            {
                const StringElement &filePath = stringTable[module.FilePathOrdinal];
                packedModule.FilePath.Offset = stringArray.Offset + filePath.Offset;
                packedModule.FilePath.Count = filePath.Text.length();
                packedModule.FileNameOffset = module.FileNameOffset;
            }
        }
    }

    TraceRecord *entries;

    if (initialiser.tryGetSafeFieldData(entryArray, entries, count))
    {
        // Replace the byte count with an element count.
        trace->Entries.Count = count;

        size_t maxCount = std::min(count, elements.size());

        for (size_t index = 0; index < maxCount; ++index)
        {
            const TraceElement &element = elements[index];
            TraceRecord &record = entries[index];

            if (element.SymbolOrdinal < stringTable.size())
            {
                const StringElement &fnName = stringTable[element.SymbolOrdinal];
                record.FunctionName.Offset = fnName.Offset + stringArray.Offset;
                record.FunctionName.Count = fnName.Text.length();
            }

            record.Offset = element.Record.Offset;
            record.ModuleOrdinal = element.ModuleOrdinal;
        }
    }

    // Initialise strings.
    count = stringTable.size();

    for (size_t index = 0; index < count; ++index)
    {
        const StringElement &str = stringTable[index];

        // Copy the string and null terminator into the block.
        initialiser.initialiseField(stringArray, str.Text);

        // Increase the offset/reduce the size of the string table block.
        size_t size = std::min(str.Text.length() + 1, stringArray.Count);
        stringArray.Offset += size;
        stringArray.Count -= size;
    }

    return trace;
}

//! @brief Constructs a self-contained stack trace blob from a set of
//! activation records.
//! @param[in] records The array of activation records captured from the call stack.
//! @param[in] count The number of elements in the records array.
//! @returns An object which the caller is responsible for deleting using the
//! destroyStackTrace() function.
StackTracePrivate *resolveStackTrace(const ActivationRecord *records, size_t count)
{
    const size_t InitialStringCount = 64;
    ModuleElements modules;
    StringElements stringTable;
    TraceElements elements;
    elements.reserve(count);
    stringTable.reserve(InitialStringCount);

    // Initialise the stack trace elements from activation records.
    for (size_t index = 0; index < count; ++index)
    {
        elements.emplace_back(records[index], index);
    }

    // Resolve the modules referenced by the stack trace.
    resolveModules(elements, modules, stringTable);

    CompareByWord<TraceElement> groupByModule(offsetof(TraceElement, Record.ModuleBase));

    auto start = elements.begin();

    while (start != elements.end())
    {
        // Find the extent of the next set of symbols which reference a
        // specific module.
        auto end = std::upper_bound(start, elements.end(),
                                    *start, groupByModule);

        if (start->ModuleOrdinal < modules.size())
        {
            ModuleElement &module = modules[start->ModuleOrdinal];

            resolveSymbols(stringTable[module.FilePathOrdinal].Text,
                           stringTable, start, end);
        }

        // Move on to the next group.
        start = end;
    }

    // Sort the elements back into the order in which they appear in the
    // captured call stack.
    std::sort(elements.begin(), elements.end(),
              CompareByWord<TraceElement>(offsetof(TraceElement, SequenceNo)));

    return packStackTrace(elements, modules, stringTable);
}

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object which can capture and hold a stack trace.
StackTrace::StackTrace() :
    _info(nullptr),
    _destroyOnDelete(false)
{}

//! @brief Copies a stack trace description.
//! @param[in] info The stack trace data to copy.
StackTrace::StackTrace(const StackTracePrivate *info) :
    _info(cloneStackTrace(info)),
    _destroyOnDelete(true)
{
}

//! @brief Create an independent copy of a stack trace.
//! @param[in] rhs The stack trace to copy.
StackTrace::StackTrace(const StackTrace &rhs) :
    _info(cloneStackTrace(rhs._info)),
    _destroyOnDelete(true)
{
}

//! @brief Moves a stack trace from one object to another.
StackTrace::StackTrace(StackTrace &&rhs) noexcept :
    _info(rhs._info),
    _destroyOnDelete(rhs._destroyOnDelete)
{
    rhs._info = nullptr;
}

//! @brief Disposes of all resources owned by the object.
StackTrace::~StackTrace()
{
    dispose();
}

//! @brief Determines whether the stack trace contains any entries.
//! @retval true The stack trace does not contain any entries.
//! @retval false The stack trace contains at least one entry.
bool StackTrace::isEmpty() const
{
    return (_info == nullptr) || (_info->Entries.Count < 1);
}

//! @brief Gets the count of modules referenced by the stack trace.
size_t StackTrace::getModuleCount() const
{
    return (_info == nullptr) ? 0 : _info->Modules.Count;
}

//! @brief Fills in an object defining a module referenced in the stack trace.
//! @param[in] moduleIndex The 0-based index of the module to get.
//! @param[out] moduleDefinition A structure to be filled in with the definition
//! of the module.
//! @details If entryIndex is out of range, the structure will be initialised
//! with empty values.
void StackTrace::getModule(size_t moduleIndex, StackTraceModule &moduleDefinition) const
{
    if ((_info != nullptr) && (moduleIndex < _info->Modules.Count))
    {
        const TraceModule *sourceModule = _info->getModule(moduleIndex);

        moduleDefinition.FilePath = _info->getString(sourceModule->FilePath);
        InlineField fileName = sourceModule->FilePath;
        fileName.Offset += sourceModule->FileNameOffset;
        fileName.Count -= sourceModule->FileNameOffset;

        moduleDefinition.FileName = _info->getString(fileName);
        moduleDefinition.BaseAddress = sourceModule->BaseAddress;
    }
    else
    {
        moduleDefinition.FilePath = Utf::getEmpty();
        moduleDefinition.FileName = Utf::getEmpty();
        moduleDefinition.BaseAddress = 0;
    }
}

//! @brief Gets the full path to a module referenced in the stack trace.
//! @param[in] moduleIndex The 0-based index of the module.
std::string_view StackTrace::getModulePath(size_t moduleIndex) const
{
    if ((_info == nullptr) ||
        (moduleIndex >= _info->Modules.Count))
    {
        return Utf::getEmpty();
    }
    else
    {
        const TraceModule *module = _info->getModule(moduleIndex);

        return _info->getString(module->FilePath);
    }
}

//! @brief Gets the file name of a module referenced in the stack trace.
//! @param[in] moduleIndex The 0-based index of the module.
std::string_view StackTrace::getModuleFileName(size_t moduleIndex) const
{
    if ((_info == nullptr) ||
        (moduleIndex >= _info->Modules.Count))
    {
        return Utf::getEmpty();
    }
    else
    {
        const TraceModule *module = _info->getModule(moduleIndex);
        InlineField fileName = module->FilePath;
        fileName.Offset += module->FileNameOffset;
        fileName.Count -= module->FileNameOffset;

        return _info->getString(fileName);
    }
}

//! @brief Gets the base address of a module referenced in the stack trace.
//! @param[in] moduleIndex The 0-based index of the module.
uintptr_t StackTrace::getModuleBase(size_t moduleIndex) const
{
    uintptr_t baseAddr = 0;

    if ((_info != nullptr) && (moduleIndex < _info->Modules.Count))
    {
        baseAddr = _info->getModule(moduleIndex)->BaseAddress;
    }

    return baseAddr;
}

//! @brief Gets the count of function activation records in the stack trace.
size_t StackTrace::getEntryCount() const
{
    return (_info == nullptr) ? 0 : _info->Entries.Count;
}

//! @brief Fills in a structure describing a stack trace entry.
//! @param[in] entryIndex The 0-based index of the entry to retrieve.
//! @param[out] entry The structure to fill in with information about the stack
//! trace entry.
//! @details If entryIndex is out of range, the structure will be initialised
//! with empty values.
void StackTrace::getEntry(size_t entryIndex, StackTraceEntry &entry) const
{
    if ((_info != nullptr) && (entryIndex < _info->Entries.Count))
    {
        const TraceRecord *record = _info->getEntry(entryIndex);

        entry.Symbol = _info->getString(record->FunctionName);
        entry.Offset = record->Offset;
        entry.ModuleId = record->ModuleOrdinal;
    }
    else
    {
        entry.Symbol = Utf::getEmpty();
        entry.Offset = 0;
        entry.ModuleId = 0;
    }
}

//! @brief Fills in a structure fully describing a stack trace entry and the
//! module it references
//! @param[in] entryIndex The 0-based index of the entry to retrieve.
//! @param[out] entry The structure to fill in with information about the stack
//! trace entry.
//! @details If entryIndex is out of range, the structure will be initialised
//! with empty values.
void StackTrace::getFullEntry(size_t entryIndex, FullStackTraceEntry &entry) const
{
    if ((_info != nullptr) && (entryIndex < _info->Entries.Count))
    {
        const TraceRecord *record = _info->getEntry(entryIndex);
        entry.ModuleId = record->ModuleOrdinal;

        if (record->ModuleOrdinal < _info->Modules.Count)
        {
            const TraceModule *module = _info->getModule(record->ModuleOrdinal);
            InlineField fileName = module->FilePath;
            fileName.Offset += module->FileNameOffset;
            fileName.Count -= module->FileNameOffset;

            entry.FilePath = _info->getString(module->FilePath);
            entry.FileName = _info->getString(fileName);
            entry.ModuleBaseAddress = module->BaseAddress;
        }
        else
        {
            entry.FilePath = Utf::getEmpty();
            entry.FileName = Utf::getEmpty();
            entry.ModuleBaseAddress = 0;
        }

        entry.Symbol = _info->getString(record->FunctionName);
        entry.Offset = record->Offset;
    }
    else
    {
        entry.Symbol = Utf::getEmpty();
        entry.Offset = 0;
        entry.ModuleId = 0;
    }
}

//! @brief Gets the address of a function activation record in the stack trace.
//! @param[in] entryIndex The 0-based index of the activation record to obtain.
uintptr_t StackTrace::getEntryAddress(size_t entryIndex) const
{
    uintptr_t addr = 0;

    if ((_info != nullptr) && (entryIndex < _info->Entries.Count))
    {
        const TraceRecord *entry = _info->getEntry(entryIndex);
        const TraceModule *module = _info->getModule(entry->ModuleOrdinal);

        addr = module->BaseAddress + entry->Offset;
    }

    return addr;
}

//! @brief Gets the offset of a function activation record within its owning module.
//! @param[in] entryIndex The 0-based index of the activation record to obtain.
uintptr_t StackTrace::getEntryOffset(size_t entryIndex) const
{
    uintptr_t offset = 0;

    if ((_info != nullptr) && (entryIndex < _info->Entries.Count))
    {
        offset = _info->getEntry(entryIndex)->Offset;
    }

    return offset;
}

//! @brief Gets the index of the module associated with a function activation record.
//! @param[in] entryIndex The 0-based index of the activation record to obtain.
//! @return A value less than that which is returned by getModuleCount() if the
//! associated module is valid, or a value greater than or equal if there is no
//! associated module, such as when the code was dynamically generated.
size_t StackTrace::getEntryModule(size_t entryIndex) const
{
    uintptr_t moduleId = 0;

    if ((_info != nullptr) && (entryIndex < _info->Entries.Count))
    {
        moduleId = _info->getEntry(entryIndex)->ModuleOrdinal;
    }

    return moduleId;
}

//! @brief Gets the name of the C++ function associated with the activation record.
//! @param[in] entryIndex The 0-based index of the activation record to obtain.
std::string_view StackTrace::getEntrySymbol(size_t entryIndex) const
{
    if ((_info != nullptr) && (entryIndex < _info->Entries.Count))
    {
        return _info->getString(_info->getEntry(entryIndex)->FunctionName);
    }
    else
    {
        return Utf::getEmpty();
    }
}

//! @brief Gets a pointer to the raw stack trace data.
//! @note For internal use only!
const StackTracePrivate *StackTrace::getData() const
{
    return _info;
}

//! @brief Appends a summary of the stack trace to an STL string.
//! @param[out] destination The STL string to receive the text summary.
//! @param[in] includeModules True to include a summary of modules
//! referenced in the trace, false to omit additional module data.
void StackTrace::appendToString(std::string &destination,
                                bool includeModules /* = true */) const
{
    if (_info != nullptr)
    {
        const int addressPrecision = static_cast<int>(sizeof(uintptr_t) * 2);
        FullStackTraceEntry fullEntry;
        int entryPrecision = static_cast<int>(ceil(log10(_info->Entries.Count + 1)));

        for (size_t index = 0; index < _info->Entries.Count; ++index)
        {
            getFullEntry(index, fullEntry);

            // Print the entry index.
            appendPrintf(destination, "%*zu: ", entryPrecision, index + 1);

            if (fullEntry.FilePath.empty())
            {
                destination.append("<Dynamic module>");
            }
            else
            {
                destination.append(fullEntry.FileName);
            }

            if (includeModules)
            {
                // Print the module reference in case several modules of the
                // same name (but different paths) are loaded into the process.
                appendPrintf(destination, " [%zu]",
                             fullEntry.ModuleId + 1);
            }

            if (fullEntry.Symbol.empty())
            {
                appendPrintf(destination, "!0x%.*zX\n",
                             addressPrecision,
                             fullEntry.ModuleBaseAddress + fullEntry.Offset,
                             fullEntry.ModuleId + 1);
            }
            else
            {
                appendPrintf(destination, "!%s\n",
                             fullEntry.Symbol.data(),
                             fullEntry.ModuleId + 1);
            }
        }

        if (includeModules)
        {
            destination.append("\nModules:\n");

            for (size_t index = 0; index < _info->Modules.Count; ++index)
            {
                const TraceModule *moduleData = _info->getModule(index);
                std::string_view filePath = _info->getString(moduleData->FilePath);

                if (filePath.empty())
                {
                    appendPrintf(destination, "[%zu] Base: 0x%.*X: <Dynamic module>\n",
                                 index + 1, addressPrecision, moduleData->BaseAddress);
                }
                else
                {
                    appendPrintf(destination, "[%zu] Base: 0x%.*X: %s\n",
                                 index + 1, addressPrecision,
                                 moduleData->BaseAddress, filePath.data());
                }
            }
        }
    }
}

//! @brief Captures a trace of the stack on the current thread.
//! @param[in] pruneEntries The count of activation records to prune from the top
//! of the stack before resolving the entries.
void StackTrace::captureCurrentThread(size_t pruneEntries /* = 0 */)
{
#ifdef _WIN32
    CONTEXT cpuState;
    ::RtlCaptureContext(&cpuState);

    // Ensure the capture omits the current function.
    capture(&cpuState, pruneEntries + 1);
#else
    std::vector<void *> callStack;
    callStack.resize(256);

    // Use the gcc compiler function to get a raw stack trace.
    size_t callStackSize = backtrace(callStack.data(), callStack.size());

    while (callStackSize == callStack.size())
    {
        // The buffer wasn't big enough, make it bigger and try again.
        callStack.resize(callStack.size() * 2);

        callStackSize = backtrace(callStack.data(), callStack.size());
    }

    // Convert the raw pointers in ActivationRecord structures.
    std::vector<ActivationRecord> records;
    records.reserve(callStackSize - pruneEntries);
    ActivationRecord record;

    for (size_t index = pruneEntries + 1; index < callStackSize; ++index)
    {
        // The addresses will be split into module/offset at a lower level.
        record.ModuleBase = 0;
        record.Offset = reinterpret_cast<uintptr_t>(callStack[index]);

        records.push_back(record);
    }

    // Process the activation records into a symbolic stack trace.
    capture(records.data(), records.size());
#endif
}

//! @brief Resolves a set of function activation records into a symbolic
//! stack trace.
//! @param[in] stackRecords The activation records to resolve.
//! @param[in] count The number of elements in stackRecords.
void StackTrace::capture(const ActivationRecord *stackRecords, size_t count)
{
    destroyStackTrace(_info);

    // Process the trace.
    _info = resolveStackTrace(stackRecords, count);
}

#ifdef _WIN32
//! @brief Captures function activation records from a Win32 processor context.
//! @param[in] context The state of the processor from which the stack should
//! be captured.
//! @param[in] pruneEntries The count of entries from the top of the stack to
//! ignore.
void StackTrace::capture(const _CONTEXT *context, size_t pruneEntries /* = 0 */)
{
    destroyStackTrace(_info);

    std::vector<ActivationRecord> activationRecords;

    activationRecords.resize(256);

    size_t count = captureActivationRecords(context, activationRecords.data(),
                                            activationRecords.size(),
                                            pruneEntries);

    while (count == activationRecords.size())
    {
        // The buffer was too small, resize and try again.
        activationRecords.resize(activationRecords.size() * 2);

        count = captureActivationRecords(context, activationRecords.data(),
                                         activationRecords.size(),
                                         pruneEntries);
    }

    // Process the trace.
    _info = resolveStackTrace(activationRecords.data(), count);
}
#endif

//! @brief Assigns a call stack to the object and disables destroy
//! semantics on it.
//! @returns A reference to the current object.
//! @note This mechanism is for internal use only.
StackTrace &StackTrace::operator=(StackTracePrivate *rhs)
{
    if (_info != rhs)
    {
        dispose();

        _info = rhs;
        _destroyOnDelete = false;
    }

    return *this;
}

//! @brief Creates a copy of stack data within the object.
//! @param[in] rhs A pointer to the raw stack trace data to copy.
//! @returns A reference to the current object.
StackTrace &StackTrace::operator=(const StackTracePrivate *rhs)
{
    if (_info != rhs)
    {
        dispose();

        _destroyOnDelete = true;
        _info = cloneStackTrace(rhs);
    }

    return *this;
}

//! @brief Creates a copy of a stack trace.
//! @param[in] rhs The stack trace to copy.
//! @returns A reference to the current object.
StackTrace &StackTrace::operator=(const StackTrace &rhs)
{
    if (_info != rhs._info)
    {
        dispose();

        _destroyOnDelete = true;
        _info = cloneStackTrace(rhs._info);
    }

    return *this;
}

//! @brief Moves the stack trace data from one object to this one.
//! @param[in] rhs The stack trace object to transfer data from.
//! @returns A reference to the current object.
StackTrace &StackTrace::operator=(StackTrace &&rhs) noexcept
{
    if (_info != rhs._info)
    {
        dispose();

        _info = rhs._info;
        _destroyOnDelete = rhs._destroyOnDelete;
        rhs._info = nullptr;
    }

    return *this;
}

//! @brief Disposes of the stack trace data, possibly freeing it.
void StackTrace::dispose()
{
    if (_destroyOnDelete)
    {
        destroyStackTrace(_info);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
//! @brief Captures the activation records of a processor stack described
//! by a captured processor state.
//! @param[in] context A pointer to the processor state defining the top of
//! the stack.
//! @param[out] records The buffer to receive the captured activation records.
//! @param[in] maxCount The maximum number of entries in the records array.
//! @param[in] skip The count of activation records to skip at the top of the
//! stack.
//! @returns The count of elements written to the records array.
size_t captureActivationRecords(const CONTEXT *context,
                                ActivationRecord *records,
                                size_t maxCount,
                                size_t skip)
{
    // Copy the processor state so that we can simulate function returns all
    // the way up the stack.
    CONTEXT simulatedState = *context;

    // Use the copy of the state to simulate returning from successive
    // function calls.
    const size_t wordSize = sizeof(void *);
    UNWIND_HISTORY_TABLE unwindCache;
    zeroFill(unwindCache);

    int entryCount;

    // Increase the limit by the number of elements we ignore.
    maxCount += skip;

    for (entryCount = 0;
         (entryCount < maxCount) && (simulatedState.Rip != 0);
         ++entryCount)
    {
        size_t functionImageBase;
        PRUNTIME_FUNCTION functionInfo = RtlLookupFunctionEntry(simulatedState.Rip,
                                                                &functionImageBase,
                                                                &unwindCache);

        if (functionInfo == nullptr)
        {
            // The function was trivial, the stack pointer holds the return
            // address.

            // Simulate a simple function return.
            simulatedState.Rip = *reinterpret_cast<const DWORD64 *>(simulatedState.Rsp);
            simulatedState.Rsp += wordSize;
        }
        else
        {
            if (entryCount >= skip)
            {
                // Store an entry defining the current location in the simulated code.
                ActivationRecord &entry = records[entryCount - skip];
                entry.ModuleBase = functionImageBase;
                entry.Offset = simulatedState.Rip - functionImageBase;
            }

            // Simulate the effects of a return form the selected function on the
            // copy of the processor state.
            size_t establisherFrame;
            RtlVirtualUnwind(UNW_FLAG_NHANDLER, functionImageBase, simulatedState.Rip,
                             functionInfo, &simulatedState, nullptr,
                             &establisherFrame, nullptr);
        }
    }

    return entryCount - skip;
}
#endif

//! @brief Gets the size of a block of self-contained stack trace data.
//! @param[in] info A pointer to the opaque stack trace data.
//! @returns The size of the block in bytes.
size_t getStackTraceSize(const StackTracePrivate *info)
{
    size_t infoSize = 0;

    if (info != nullptr)
    {
        infoSize = info->TotalSize;
    }

    return infoSize;
}

//! @brief Creates an independent copy of a captured stack trace.
//! @param[in] info The stack trace data to copy.
//! @return A copy of the stack trace data which the caller is responsible for
//! disposing of with the destroyStackTrace() function.
StackTracePrivate *cloneStackTrace(const StackTracePrivate *info)
{
    StackTracePrivate *clone = nullptr;

    if (info != nullptr)
    {
        clone = reinterpret_cast<StackTracePrivate *>(std::malloc(info->TotalSize));

        std::memcpy(clone, info, info->TotalSize);
    }

    return clone;
}

//! @brief Disposes of a stack trace object.
//! @param[in,out] info The pointer to the stack trace data to release and set
//! to nullptr if it hasn't already.
void destroyStackTrace(StackTracePrivate *&info)
{
    if (info != nullptr)
    {
        std::free(info);
        info = nullptr;
    }
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


//! @file InputSet.cpp
//! @brief The definition of an object which defines a set of sources of
//! source code text to be interpreted.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <unordered_set>

#include "InputSet.hpp"
#include "InputSource.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An implementation of IInputSet which accesses a file and possibly
//! additional files in the same or related folders.
class FileInputSet : public IInputSet
{
private:
    // Internal Fields
    Fs::Path _primaryFile;
    std::vector<Fs::Path> _searchPaths;

    // Internal Functions
    //! @brief Determines if a file path points to a valid include file.
    //! @param[in] includeFilePath The path to the file to query.
    //! @retval true The file exists and appears valid.
    //! @retval false The file doesn't exist or is a directory.
    static bool isValidFile(const Fs::Path &includeFilePath)
    {
        Fs::Entry fileEntry(includeFilePath);

        return fileEntry.exists() && (fileEntry.isDirectory() == false);
    }

    //! @brief Attempts to find a file from a path specification.
    //! @param[in] id The parsed path specification.
    //! @param[out] match The full path to the matching file, if one was found.
    //! @retval true A file matching the specification was found.
    //! @retval false No file on any search path matched the specification.
    bool tryFindFile(const Fs::Path &id, Fs::Path &match) const
    {
        bool hasMatch = false;

        if (id.hasRoot())
        {
            // Don't bother with search paths, we only have one choice.
            if (isValidFile(id))
            {
                match = id;
                hasMatch = true;
            }
        }
        else
        {
            // Search the folder containing the current file first.
            Fs::Path currentDir = _primaryFile.getDirectoryPath();
            Fs::Path absIdPath = id.convertToAbsolute(currentDir);

            if (isValidFile(absIdPath))
            {
                match = absIdPath;
                hasMatch = true;
            }
            else
            {
                // Use the search paths.
                for (const Fs::Path &searchDir : _searchPaths)
                {
                    absIdPath = id.convertToAbsolute(searchDir);

                    if (isValidFile(absIdPath))
                    {
                        match = absIdPath;
                        hasMatch = true;
                        break;
                    }
                }
            }
        }

        return hasMatch;
    }
public:
    // Construction/Destruction
    //! @brief Constructs a new input set.
    //! @param[in] primaryFile The path to the primary source code file.
    //! @param[in] searchPaths The ordered set of paths to folders to search for
    //! included source files.
    FileInputSet(const Fs::Path &primaryFile, const std::vector<Fs::Path> &searchPaths) :
        _primaryFile(primaryFile.convertToAbsolute()),
        _searchPaths(searchPaths)
    {
    }

    // Overrides
    // Inherited from IInputSet.
    virtual bool tryGetPrimaryInputSource(IInputSourcePtr &source,
                                          String &errorOrId) override
    {
        bool hasSource = false;

        if (tryCreateFileInputSource(_primaryFile, source, errorOrId))
        {
            errorOrId = _primaryFile.toString(Fs::PathUsage::Display);
            hasSource = true;
        }

        return hasSource;
    }

    // Inherited from IInputSet.
    virtual bool tryGetInputSource(const String &identifier,
                                   IInputSetUPtr &source,
                                   String &errorOrId) override
    {
        Fs::Path idPath;

        bool isOK = false;

        if (Fs::Path::tryParse(identifier, idPath, errorOrId))
        {
            Fs::Path target;

            if (tryFindFile(idPath, target))
            {
                // Create a new input set based on the file which was found.
                source.reset(new FileInputSet(target, _searchPaths));
                isOK = true;
            }
            else
            {
                errorOrId = String::format("Cannot find included file '{0}' on any search path.",
                                           { identifier });
            }
        }
        else
        {
            errorOrId = String::format("Include file path '{0}' is invalid.",
                                       { identifier });
        }

        return isOK;
    }
};


//! @brief An implementation of IInputSet which accesses a string.
class BufferInputSet : public IInputSet
{
private:
    // Internal Fields
    String _id;
    String _primaryData;

public:
    // Construction/Destruction
    //! @brief Constructs a new input set.
    //! @param[in] id The identifier of the primary input source.
    //! @param[in] primaryData The text in the primary input source.
    BufferInputSet(const String &id, const String &primaryData) :
        _id(id),
        _primaryData(primaryData)
    {
    }

    // Overrides
    // Inherited from IInputSet.
    virtual bool tryGetPrimaryInputSource(IInputSourcePtr &source,
                                          String &errorOrId) override
    {
        // Always return source providing the primary data.
        source = createBufferInputSource(_primaryData);
        errorOrId = _id;
        return true;
    }

    // Inherited from IInputSet.
    virtual bool tryGetInputSource(const String & /* identifier */,
                                   IInputSetUPtr & /* source */,
                                   String &errorOrId) override
    {
        // No secondary sources currently supported.
        errorOrId = "Data not found.";
        return false;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED

////////////////////////////////////////////////////////////////////////////////
// Class Method Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an input set based on a single source file.
//! @param[in] rootFile The name and path of the primary source file.
//! @param[in] searchPaths The paths to folders to search for any included
//! source files.
//! @return An input set which will be automatically disposed of.
IInputSetUPtr createFileInputSet(const Fs::Path &rootFile,
                                 const std::vector<Fs::Path> &searchPaths)
{
    std::vector<Fs::Path> resolvedPaths;

    if (searchPaths.empty() == false)
    {
        // Resolve the set of search paths to remove invalid entries and repeats.
        resolvedPaths.reserve(searchPaths.size() + 1);

        Fs::PathBuilder builder;
        std::unordered_set<Fs::Path> visitedPaths;

        // Add the search paths in order ensuring there are no duplicates and
        // any search paths actually exist and point to a folder.
        for (const Fs::Path &searchPath : searchPaths)
        {
            builder = searchPath;
            builder.convertToAbsolute();
            builder.makeCanonical();

            Fs::Path resolvedPath(builder);

            if (visitedPaths.find(resolvedPath) == visitedPaths.end())
            {
                // The path is not a repeat of another.
                Fs::Entry targetDir(resolvedPath);

                if (targetDir.exists() && targetDir.isDirectory())
                {
                    // The path points to an existing directory.
                    resolvedPaths.push_back(resolvedPath);
                    visitedPaths.insert(resolvedPath);
                }
            }
        }
    }

    return std::make_unique<FileInputSet>(rootFile, resolvedPaths);
}

//! @brief Constructs an input set which reproduces the contents of an
//! in-memory string.
//! @param[in] identifier The text used to identify the primary source in
//! messages.
//! @param[in] source The text of the primary input source.
//! @return An input set which will be automatically disposed of.
IInputSetUPtr createStringInputSet(const String &identifier, const String &source)
{
    return std::make_unique<BufferInputSet>(identifier, source);
}



}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


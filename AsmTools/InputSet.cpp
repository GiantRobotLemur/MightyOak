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

namespace Mo {
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
    Ag::Fs::Path _primaryFile;
    std::vector<Ag::Fs::Path> _searchPaths;

    // Internal Functions
    //! @brief Determines if a file path points to a valid include file.
    //! @param[in] includeFilePath The path to the file to query.
    //! @retval true The file exists and appears valid.
    //! @retval false The file doesn't exist or is a directory.
    static bool isValidFile(const Ag::Fs::Path &includeFilePath)
    {
        Ag::Fs::Entry fileEntry(includeFilePath);

        return fileEntry.exists() && (fileEntry.isDirectory() == false);
    }

    //! @brief Attempts to find a file from a path specification.
    //! @param[in] id The parsed path specification.
    //! @param[out] match The full path to the matching file, if one was found.
    //! @retval true A file matching the specification was found.
    //! @retval false No file on any search path matched the specification.
    bool tryFindFile(const Ag::Fs::Path &id, Ag::Fs::Path &match) const
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
            Ag::Fs::Path currentDir = _primaryFile.getDirectoryPath();
            Ag::Fs::Path absIdPath = id.convertToAbsolute(currentDir);

            if (isValidFile(absIdPath))
            {
                match = absIdPath;
                hasMatch = true;
            }
            else
            {
                // Use the search paths.
                for (const Ag::Fs::Path &searchDir : _searchPaths)
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
    FileInputSet(const Ag::Fs::Path &primaryFile,
                 const std::vector<Ag::Fs::Path> &searchPaths) :
        _primaryFile(primaryFile.convertToAbsolute()),
        _searchPaths(searchPaths)
    {
    }

    // Overrides
    // Inherited from IInputSet.
    virtual bool tryGetPrimaryInputSource(IInputSourcePtr &source,
                                          Ag::string_ref_t errorOrId) override
    {
        bool hasSource = false;

        if (tryCreateFileInputSource(_primaryFile, source, errorOrId))
        {
            errorOrId = _primaryFile.toString(Ag::Fs::PathUsage::Display);
            hasSource = true;
        }

        return hasSource;
    }

    // Inherited from IInputSet.
    virtual bool tryGetInputSource(Ag::string_cref_t identifier,
                                   IInputSetUPtr &source,
                                   Ag::string_ref_t errorOrId) override
    {
        Ag::Fs::Path idPath;

        bool isOK = false;

        if (Ag::Fs::Path::tryParse(identifier, idPath, errorOrId))
        {
            Ag::Fs::Path target;

            if (tryFindFile(idPath, target))
            {
                // Create a new input set based on the file which was found.
                source.reset(new FileInputSet(target, _searchPaths));
                isOK = true;
            }
            else
            {
                errorOrId = Ag::String::format("Cannot find included file '{0}'"
                                               " on any search path.",
                                               { identifier });
            }
        }
        else
        {
            errorOrId = Ag::String::format("Include file path '{0}' is invalid.",
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
    Ag::String _id;
    Ag::String _primaryData;

public:
    // Construction/Destruction
    //! @brief Constructs a new input set.
    //! @param[in] id The identifier of the primary input source.
    //! @param[in] primaryData The text in the primary input source.
    BufferInputSet(Ag::string_cref_t id, Ag::string_cref_t primaryData) :
        _id(id),
        _primaryData(primaryData)
    {
    }

    // Overrides
    // Inherited from IInputSet.
    virtual bool tryGetPrimaryInputSource(IInputSourcePtr &source,
                                          Ag::string_ref_t errorOrId) override
    {
        // Always return source providing the primary data.
        source = createBufferInputSource(_primaryData);
        errorOrId = _id;
        return true;
    }

    // Inherited from IInputSet.
    virtual bool tryGetInputSource(Ag::string_cref_t /* identifier */,
                                   IInputSetUPtr & /* source */,
                                   Ag::string_ref_t errorOrId) override
    {
        // No secondary sources currently supported.
        errorOrId = "Data not found.";
        return false;
    }
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an input set based on a single source file.
//! @param[in] rootFile The name and path of the primary source file.
//! @param[in] searchPaths The paths to folders to search for any included
//! source files.
//! @return An input set which will be automatically disposed of.
IInputSetUPtr createFileInputSet(const Ag::Fs::Path &rootFile,
                                 const std::vector<Ag::Fs::Path> &searchPaths)
{
    std::vector<Ag::Fs::Path> resolvedPaths;

    if (searchPaths.empty() == false)
    {
        // Resolve the set of search paths to remove invalid entries and repeats.
        resolvedPaths.reserve(searchPaths.size() + 1);

        Ag::Fs::PathBuilder builder;
        std::unordered_set<Ag::Fs::Path> visitedPaths;

        // Add the search paths in order ensuring there are no duplicates and
        // any search paths actually exist and point to a folder.
        for (const Ag::Fs::Path &searchPath : searchPaths)
        {
            builder = searchPath;
            builder.convertToAbsolute();
            builder.makeCanonical();

            Ag::Fs::Path resolvedPath(builder);

            if (visitedPaths.find(resolvedPath) == visitedPaths.end())
            {
                // The path is not a repeat of another.
                Ag::Fs::Entry targetDir(resolvedPath);

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
IInputSetUPtr createStringInputSet(Ag::string_cref_t identifier,
                                   Ag::string_cref_t source)
{
    return std::make_unique<BufferInputSet>(identifier, source);
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////


//! @file InputSet.hpp
//! @brief The declaration of an object which defines a set of sources of
//! source code text to be interpreted.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_INPUT_SET_HPP__
#define __ASM_TOOLS_INPUT_SET_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////

#include "InputSource.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which defines a set of sources of source code text to be
//! interpreted.
class IInputSet
{
public:
    // Destruction
    virtual ~IInputSet() = default;

    // Operations
    //! @brief Attempts to get the primary input source of the set.
    //! @param[out] source Receives the new input source.
    //! @param[out] errorOrId On success, receives the identifier of the source
    //! which should appear in messages related to it. On failure, receives a
    //! message detailing why the function failed.
    //! @retval true source was populated with an new input source and
    //! errorOrId with its identifier.
    //! @retval false source was not overwritten and errorOrId receives text
    //! detailing why.
    virtual bool tryGetPrimaryInputSource(IInputSourcePtr &source,
                                          String &errorOrId) = 0;

    //! @brief Attempts to get a secondary input set using an identifier which
    //! it to be interpreted relative to that of the current input set.
    //! @param[in] identifier The identifier of the input source used to find
    //! the data to return.
    //! @param[out] input Receives the new input set.
    //! @param[out] errorOrId On success, receives the identifier of the source
    //! which should appear in messages related to it. On failure, receives a
    //! message detailing why the function failed.
    //! @retval true source was populated with an new input source and
    //! errorOrId with its identifier.
    //! @retval false source was not overwritten and errorOrId receives text
    //! detailing why.
    virtual bool tryGetInputSource(const String &identifier,
                                   std::unique_ptr<IInputSet> &input,
                                   String &errorOrId) = 0;
};

using IInputSetUPtr = std::unique_ptr<IInputSet>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
IInputSetUPtr createFileInputSet(const Fs::Path &rootFile,
                                 const std::vector<Fs::Path> &searchPaths);
IInputSetUPtr createStringInputSet(const String &identifier, const String &source);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

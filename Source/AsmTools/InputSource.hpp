//! @file InputSource.hpp
//! @brief The declaration of an object which produces source code text to be
//! interpreted.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_INPUT_SOURCE_HPP__
#define __ASM_TOOLS_INPUT_SOURCE_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include <Ag/Core.hpp>

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which produces source code text to be interpreted.
class IInputSource
{
public:
    // Construction/Destruction
    virtual ~IInputSource() = default;

    // Operations
    //! @brief Reads the next block of wide characters from the input.
    //! @param[in] buffer The buffer to receive the Unicode characters read from
    //! the input.
    //! @param[in] bufferSize The maximum number of characters buffer can
    //! accommodate.
    //! @returns The count of characters written to the buffer. The value will
    //! be no more than bufferSize. If the value is 0, the input source will
    //! not return any more characters from any subsequent calls.
    virtual size_t readCharacters(Ag::utf32_ptr_t buffer, size_t bufferSize) =0;
};

using IInputSourcePtr = std::unique_ptr<IInputSource>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool tryCreateFileInputSource(const Ag::Fs::Path &fileName, IInputSourcePtr &ptr,
                              Ag::String &error);
IInputSourcePtr createBufferInputSource(const Ag::String &sourceText);

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

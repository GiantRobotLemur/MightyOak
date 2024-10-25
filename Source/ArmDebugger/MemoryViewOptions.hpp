//! @file ArmDebugger/MemoryViewOptions.hpp
//! @brief The declaration of an object describing how memory contents should
//! be displayed to the user.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_MEMORY_VIEW_OPTIONS_HPP__
#define __ARM_DEBUGGER_MEMORY_VIEW_OPTIONS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QJsonObject>

#include "AsmTools/Options.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief an object describing how memory contents should be displayed to
//! the user.
class MemoryViewOptions
{
public:
    // Public Types
    enum Flags : uint32_t
    {
        //! @brief Indicates that the raw memory contents should be displayed along
        //! with the interpreted contents.
        ShowRawData = 0x01,

        //! @brief Indicates that text annotations should be displayed with memory
        //! contents.
        ShowComments,

        //! @brief Indicates that text data should be wrapped on word breaks where
        //! possible, rather than at arbitrary positions.
        WrapStringDataOnWordBreaks,

        //! @brief Indicates that the disassembly processor variant should be ignored
        //! in favour of the processor variant configured for emulation.
        DisassembleCurrentArchOnly,
    };

    // Construction/Destruction
    MemoryViewOptions();
    ~MemoryViewOptions() = default;

    // Accessors
    uint32_t getAssemblyFormatFlags() const;
    void setAssemblyFormatFlags(uint32_t flags);
    uint32_t getDissemblyFlags() const;
    void setDissemblyFlags(uint32_t flags);
    uint32_t getDisplayFlags() const;
    void setDisplayFlags(uint32_t flags);
    Asm::DirectiveDataType getReptitiveDataType() const;
    void setRepetitiveDataType(Asm::DirectiveDataType dataType);

    // Operations
    void reset();
    QJsonObject write() const;
    void read(const QJsonObject &jsonObj);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    uint32_t _asmFormatFlags;   // Asm::FormatterOptions::Flags
    uint32_t _disassemblyFlags; // Asm::InstructionInfo::DisasmBits
    uint32_t _displayFlags;     // MemoryDisplayFlags
    Asm::DirectiveDataType _repetitiveDataType;

};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

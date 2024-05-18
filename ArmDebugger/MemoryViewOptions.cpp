//! @file ArmDebugger/MemoryViewOptions.cpp
//! @brief The definition of an object describing how memory contents should
//! be displayed to the user.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AsmTools/InstructionInfo.hpp"

#include "MemoryViewOptions.hpp"
#include "Tools.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

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

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// MemoryViewOptions Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
MemoryViewOptions::MemoryViewOptions() :
    _asmFormatFlags(0),
    _disassemblyFlags(0),
    _displayFlags(0)

{
    reset();
}

uint32_t MemoryViewOptions::getAssemblyFormatFlags() const { return _asmFormatFlags; }
void MemoryViewOptions::setAssemblyFormatFlags(uint32_t flags) { _asmFormatFlags = flags; }
uint32_t MemoryViewOptions::getDissemblyFlags() const { return _disassemblyFlags; }
void MemoryViewOptions::setDissemblyFlags(uint32_t flags) { _disassemblyFlags = flags; }
uint32_t MemoryViewOptions::getDisplayFlags() const { return _displayFlags; }
void MemoryViewOptions::setDisplayFlags(uint32_t flags) { _displayFlags = flags; }
Asm::DirectiveDataType MemoryViewOptions::getReptitiveDataType() const { return _repetitiveDataType; }
void MemoryViewOptions::setRepetitiveDataType(Asm::DirectiveDataType dataType)
{
    _repetitiveDataType = dataType;
}

void MemoryViewOptions::reset()
{
    _asmFormatFlags = Asm::FormatterOptions::UseCoreRegAliases;

    _disassemblyFlags = Asm::InstructionInfo::ARMv6 |
        Asm::InstructionInfo::AllowFPA |
        Asm::InstructionInfo::UseStackModesOnR13;

    _displayFlags = MemoryViewOptions::ShowRawData |
        MemoryViewOptions::WrapStringDataOnWordBreaks;
}

QJsonObject MemoryViewOptions::write() const
{
    QJsonObject jsonObj;

    setJsonValue(jsonObj, "FormatFlags", _asmFormatFlags);
    setJsonValue(jsonObj, "DisasmFlags", _disassemblyFlags);
    setJsonValue(jsonObj, "DisplayFlags", _displayFlags);

    return jsonObj;
}

void MemoryViewOptions::read(const QJsonObject &jsonObj)
{
    reset();

    uint32_t value;

    if (tryGetJsonValue(jsonObj, "FormatFlags", value))
    {
        _asmFormatFlags = value;
    }

    if (tryGetJsonValue(jsonObj, "DisasmFlags", value))
    {
        _disassemblyFlags = value;
    }

    if (tryGetJsonValue(jsonObj, "DisplayFlags", value))
    {
        _displayFlags = value;
    }
}
////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


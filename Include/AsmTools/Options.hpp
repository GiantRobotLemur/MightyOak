//! @file AsmTools/Options.hpp
//! @brief The declaration of a set of properties configuring the assembly
//! process.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_OPTIONS_HPP__
#define __ASM_TOOLS_OPTIONS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <type_traits>

#include <Ag/Core.hpp>

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Describes the default instruction set available.
enum class InstructionSet : uint8_t {
    ArmV2,
    ArmV2a,
    ArmV3,
    ArmV4,
    ArmV5,
    ArmV6,
    ArmV7,

    Max,
};

//! @brief Used to define a bit mask of extensions available.
enum class ArchExtensionEnum : uint16_t
{
    None     = 0x00,
    Fpa      = 0x01,
    VfpV1    = 0x02,
    VfpV2    = 0x04,
    VfpV3    = 0x08,
    VfpV4    = 0x10,
    Neon     = 0x20,
    ThumbV1  = 0x40,
    ThumbV2  = 0x80,
    BreakPt  = 0x100,

    Mask     = 0x1FF,
    Max      = 9, // Bits used
};

using ArchExtensions = std::underlying_type<ArchExtensionEnum>::type;

//! @brief Defines the operating mode targeted by the assembler.
enum class ProcessorMode : uint8_t
{
    Arm,
    Thumb,
    Jazelle,

    Max
};

//! @brief Defines boolean options relating to the assembly process.
enum class AssemblyFlagsEnum : uint16_t
{
    None                     = 0x0000,
    PositionIndependentCode  = 0x0001,

    Mask = 0x0001,
};

using AssemblyFlags = std::underlying_type<AssemblyFlagsEnum>::type;

//! @brief Expresses the data types expected in data directives.
enum class DirectiveDataType : uint8_t
{
    Byte,
    HalfWord,
    Word,
    LongWord,
    NativeString,
    Utf8String,
    Utf16String,
    Utf32String,
    Real32,
    Real64,
    Real96,

    Max,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A set of properties configuring the assembly process.
class Options
{
public:
    // Construction/Destruction
    Options();
    ~Options() = default;

    // Accessors
    InstructionSet getInstructionSet() const;
    void setInstructionSet(InstructionSet instructionSet);
    ArchExtensions getArchitectureExtensions() const;
    void addArchitecureExtension(ArchExtensionEnum extension);
    bool hasArchitectureExtension(ArchExtensionEnum extension) const;
    uint32_t getLoadAddress() const;
    void setLoadAddress(uint32_t loadAddr);
    AssemblyFlags getFlags() const;
    bool hasFlag(AssemblyFlagsEnum flag) const;
    void setFlag(AssemblyFlagsEnum flag);
    const std::vector<Ag::Fs::Path> &getIncludePaths() const;

    // Operations
    void addIncludePath(Ag::string_cref_t path);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    std::vector<Ag::Fs::Path> _includePaths;
    uint32_t _loadAddress;
    InstructionSet _instructionSet;
    ArchExtensions _extensions;
    AssemblyFlags _flags;
};

class DirectiveDataTypeSymbol : public Ag::EnumSymbol<DirectiveDataType>
{
public:
    // Construction/Destruction
    DirectiveDataTypeSymbol(DirectiveDataType id);
    DirectiveDataTypeSymbol(DirectiveDataType id, const char *symbol,
                            const char *displayName,
                            const char *description,
                            uint8_t unitSize);

    // Accessors
    uint8_t getUnitSize() const;
private:
    // Internal Fields
    uint8_t _unitSize;
};

using DirectiveDataTypeInfo = Ag::EnumInfo<DirectiveDataType, DirectiveDataTypeSymbol>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
const Ag::EnumInfo<InstructionSet> &getInstructionSetType();
const Ag::EnumInfo<ArchExtensionEnum> &getArchExtensionsType();
const DirectiveDataTypeInfo &getDirectiveDataTypeInfo();

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

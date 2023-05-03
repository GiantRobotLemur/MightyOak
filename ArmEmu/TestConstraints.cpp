//! @file ArmEmu/name_here.cpp
//! @brief The definition of a system of objects used to set and verify the
//! state of an emulated system in tests.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cctype>

#include <algorithm>
#include <unordered_map>

#include "Ag/Core/Format.hpp"
#include "Ag/Core/LinearSortedMap.hpp"
#include "Ag/Core/VariantTypes.hpp"
#include "Ag/Core/Utils.hpp"
#include "ArmEmu.hpp"
#include "RegisterFile.inl"

#include "TestConstraints.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct ParsedConstraint
{
    std::string Id;
    std::string SymbolValue;
    uint32_t ScalarValue;
    uint32_t IdOrdinal;
    bool IsScalar;
    bool IsAddress;

    ParsedConstraint()
    {
        reset();
    }

    void reset()
    {
        Id.clear();
        SymbolValue.clear();
        ScalarValue = 0;
        IdOrdinal = 0;
        IsScalar = true;
        IsAddress = false;
    }
};

struct ElementInfo
{
    SystemElement Element;
    uint32_t OrdinalMin;
    uint32_t OrdinalMax;
    uint32_t OrdinalOffset;

    ElementInfo() :
        Element(SystemElement::Max),
        OrdinalMin(0),
        OrdinalMax(0),
        OrdinalOffset(0)
    {
    }

    ElementInfo(SystemElement element, uint32_t maxOrdinal,
                uint32_t minOrdinal = 0, uint32_t offset = 0) :
        Element(element),
        OrdinalMin(minOrdinal),
        OrdinalMax(maxOrdinal),
        OrdinalOffset(offset)
    {
    }

    ElementInfo(SystemElement element, SystemRegister reg) :
        Element(element),
        OrdinalMin(Ag::toScalar(reg)),
        OrdinalMax(Ag::toScalar(reg)),
        OrdinalOffset(Ag::toScalar(reg))
    {
    }
};

using ElementInfoIndex = std::unordered_map<std::string_view, ElementInfo>;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
const Ag::EnumInfo<SystemRegister> &getSystemRegisterType()
{
    using Item = Ag::EnumSymbol<SystemRegister>;

    static Ag::EnumInfo<SystemRegister> instance({
        Item(SystemRegister::PC, "PC", "Program Counter"),
        Item(SystemRegister::CPSR, "CPSR", "Current Program Status Register"),
        Item(SystemRegister::SPSR, "SPSR", "Saved Program Status Register"),
        Item(SystemRegister::Status, "Status", "Status Flags"),
        Item(SystemRegister::ProcessorMode, "Mode", "Processor Mode"),
        Item(SystemRegister::IrqStatus, "IrqStatus", "Interrupt Status"),
        Item(SystemRegister::IrqMask, "IrqMask", "Interrupt Mask")
    });

    return instance;
}

//! @brief Attempts to parse a string containing characters representing ARM
//! processor status flags.
//! @param[in] text The symbolic status value to parse.
//! @param[out] value Receives the value interpreted from the text.
//! @param[out] error Receives an error message if parsing failed.
//! @return A boolean value indicating if parsing was successful.
bool tryParseStatus(const std::string &text, uint32_t &value, std::string &error)
{
    bool isOK = true;
    value = 0;

    for (char bitChar : text)
    {
        switch (std::toupper(bitChar))
        {
        case 'V': value |= PsrMask::LowOverflow; break;
        case 'C': value |= PsrMask::LowCarry; break;
        case 'Z': value |= PsrMask::LowZero; break;
        case 'N': value |= PsrMask::LowNegative; break;
        default:
            if (isOK)
            {
                isOK = false;
                error.assign("Unexpected status flag character '");
                error.push_back(bitChar);
                error.append("'.");
            }
            break;
        }

        if (isOK == false)
        {
            break;
        }
    }

    return isOK;
}

bool tryProcessConstraint(ConstraintCollection &collection,
                          const ParsedConstraint &parsed,
                          std::string &error)
{
    std::string key;
    key.reserve(parsed.Id.size());
    std::transform(parsed.Id.begin(), parsed.Id.end(),
                   std::back_inserter(key),
                   [](char ch) { return static_cast<char>(std::toupper(ch)); });

    static ElementInfoIndex elementIndex;

    if (elementIndex.empty())
    {
        elementIndex.try_emplace("R", ElementInfo(SystemElement::CoreRegister, 15));
        elementIndex.try_emplace("A", ElementInfo(SystemElement::CoreRegister, 4, 1));
        elementIndex.try_emplace("V", ElementInfo(SystemElement::CoreRegister, 6, 1, 4));
        elementIndex.try_emplace("C", ElementInfo(SystemElement::CoProcessor, 15));
        elementIndex.try_emplace("CP", ElementInfo(SystemElement::CoProcessor, 15));
        elementIndex.try_emplace("CR", ElementInfo(SystemElement::CoProcRegister, 15));
        elementIndex.try_emplace("F", ElementInfo(SystemElement::FpaRegister, 7));
        elementIndex.try_emplace("PBYTE", ElementInfo(SystemElement::PhysicalByte, 0xFFFFFFFF, 0));
        elementIndex.try_emplace("PHWORD", ElementInfo(SystemElement::PhysicalHalfWord, 0xFFFFFFFF, 0));
        elementIndex.try_emplace("PWORD", ElementInfo(SystemElement::PhysicalWord, 0xFFFFFFFF, 0));
        elementIndex.try_emplace("LBYTE", ElementInfo(SystemElement::LogicalByte, 0xFFFFFFFF, 0));
        elementIndex.try_emplace("LHWORD", ElementInfo(SystemElement::LogicalHalfWord, 0xFFFFFFFF, 0));
        elementIndex.try_emplace("LWORD", ElementInfo(SystemElement::LogicalWord, 0xFFFFFFFF, 0));
        elementIndex.try_emplace("MAP", ElementInfo(SystemElement::MappedPhysicalAddress, 0xFFFFFFFF, 0));
    }

    SystemElement element = SystemElement::Max;
    uint32_t elementOrdinal = 0;

    auto pos = elementIndex.find(key);

    if (pos == elementIndex.end())
    {
        if (parsed.IdOrdinal == 0)
        {
            const auto &sysRegType = getSystemRegisterType();
            SystemRegister reg;

            if (sysRegType.tryParse(key, reg))
            {
                element = SystemElement::SystemRegister;
                elementOrdinal = Ag::toScalar(reg);
            }
        }
    }
    else
    {
        const ElementInfo &info = pos->second;

        if ((parsed.IdOrdinal < info.OrdinalMin) ||
            (parsed.IdOrdinal > info.OrdinalMax))
        {
            error.push_back('\'');
            error.append(parsed.Id);
            error.append("' element ordinal out of range.");
            return false;
        }

        element = pos->second.Element;
        elementOrdinal = parsed.IdOrdinal - info.OrdinalMin + info.OrdinalOffset;
    }

    switch (element)
    {
    case SystemElement::PhysicalByte:
    case SystemElement::PhysicalHalfWord:
    case SystemElement::PhysicalWord:
    case SystemElement::LogicalByte:
    case SystemElement::LogicalHalfWord:
    case SystemElement::LogicalWord:
    case SystemElement::MappedPhysicalAddress:
        if (parsed.IsAddress == false)
        {
            error.assign("Memory elements must specify an address.");
            return false;
        }
        break;

    case SystemElement::Max:
        error.assign("Unknown system component identifier '");
        error.append(parsed.Id);
        error.push_back('\'');
        error.push_back('.');
        return false;

    default:
        if (parsed.IsAddress)
        {
            error.assign("Register elements cannot be specified as an address.");
            return false;
        }
        break;
    }

    uint32_t elementValue = 0;

    if (element == SystemElement::SystemRegister)
    {
        SystemRegister sysReg = Ag::fromScalar<SystemRegister>(elementOrdinal);

        if (sysReg == SystemRegister::ProcessorMode)
        {
            if (parsed.IsScalar)
            {
                error.assign("A processor mode must be specified with a symbol.");
                return false;
            }
            else
            {
                const ProcessorModeEnumInfo &modeInfo = getProcessorModeType();
                size_t index;

                if (modeInfo.tryFindSymbolIndex(parsed.SymbolValue, index))
                {
                    elementValue = Ag::toScalar(modeInfo.getSymbols().at(index).getId());
                }
                else
                {
                    error.assign("Unknown processor mode '");
                    error.append(parsed.SymbolValue);
                    error.push_back('\'');
                    error.push_back('.');

                    return false;
                }
            }
        }
        else if (sysReg == SystemRegister::Status)
        {
            if (parsed.IsScalar)
            {
                if (parsed.ScalarValue > 15)
                {
                    error.assign("A scalar status value must be in the range 0-15.");
                    return false;
                }
                else
                {
                    elementValue = parsed.ScalarValue;
                }
            }
            else if (tryParseStatus(parsed.SymbolValue, elementValue, error) == false)
            {
                return false;
            }
        }
        else if (parsed.IsScalar)
        {
            elementValue = parsed.ScalarValue;
        }
        else
        {
            error.assign("A value for system register ");
            error.append(getSystemRegisterType().toDisplayName(sysReg));
            error.append(" must be bound have a scalar type.");
            return false;
        }
    }
    else if ((element != SystemElement::CoProcessor) && (parsed.IsScalar == false))
    {
        Constraint id(element, elementOrdinal, 0);
        error.assign(id.idToString());
        error.assign(" should be bound to a scalar value.");

        return false;
    }
    else
    {
        elementValue = parsed.ScalarValue;
    }

    collection.emplace_back(element, elementOrdinal, elementValue);

    return true;
}

bool compileConstraints(ConstraintCollection &constraints,
                        std::string &error)
{
    uint32_t cpid = 0xFF;

    for (Constraint &item : constraints)
    {
        if (item.Element == SystemElement::CoProcessor)
        {
            cpid = item.ElementIndex;
        }
        else if (item.Element == SystemElement::CoProcRegister)
        {
            if (cpid == 0xFF)
            {
                error.assign("Co-processor register specified without "
                             "previously defining the owning co-processor.");
                return false;
            }
            else
            {
                // Assign the identifier of the owning co-processor.
                item.ElementIndex |= cpid << 4;
            }
        }
    }

    // Remove co-processor references now that registers have been assigned.
    auto last = std::remove_if(constraints.begin(), constraints.end(),
                               [](const Constraint &rhs) { return rhs.Element == SystemElement::CoProcessor; });
    constraints.erase(last, constraints.end());

    // Sort constraints into a fixed order.
    std::sort(constraints.begin(), constraints.end());

    // Ensure there are no duplicates.
    last = std::unique(constraints.begin(), constraints.end());

    if (last == constraints.end())
    {
        // All elements are unique.
        return true;
    }
    else
    {
        error.assign("Duplicate constraints defined.");
        return false;
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Constraint Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty constraint.
Constraint::Constraint() :
    Value(0),
    Element(SystemElement::Max),
    ElementIndex(0)
{
}

//! @brief Constructs an initialised constraint.
//! @param[in] location The location within the processor the value is located.
//! @param[in] index The 0-based register index to access.
//! @param[in] value The value to load, store or be expected.
Constraint::Constraint(SystemElement location, uint32_t index, uint32_t value) :
    Value(value),
    Element(location),
    ElementIndex(index)
{
}

//! @brief Compares constraints based on the location they reference.
//! @param[in] rhs The constraint to compare against.
//! @return True if the current value references the same location as rhs.
bool Constraint::operator==(const Constraint &rhs) const
{
    return (Element == rhs.Element) &&
           (ElementIndex == rhs.ElementIndex);
}

//! @brief Compares constraints so that they are ordered by location and index.
//! @param[in] rhs The constraint to compare against.
//! @return True if the current value is less than rhs.
bool Constraint::operator<(const Constraint &rhs) const
{
    bool isLess = false;

    if (Element == rhs.Element)
    {
        isLess = ElementIndex < rhs.ElementIndex;
    }
    else
    {
        isLess = Element < rhs.Element;
    }

    return isLess;
}

//! @brief Produces a text string describing the constraint.
//! @return A string in a Key=Value pair format.
std::string Constraint::toString() const
{
    std::string text = idToString();

    text.push_back('=');
    bool hasText = false;

    if (Element == SystemElement::SystemRegister)
    {
        SystemRegister sysReg = Ag::fromScalar<SystemRegister>(ElementIndex);

        if (sysReg == SystemRegister::ProcessorMode)
        {
            const auto &modeType = getProcessorModeType();

            text.append(modeType.toString(Ag::forceFromScalar<ProcessorMode>(Value)));
        }
        else if (sysReg == SystemRegister::Status)
        {
            if (Value & PsrMask::LowStatus)
            {
                if (Value & PsrMask::LowOverflow)
                    text.push_back('V');

                if (Value & PsrMask::LowCarry)
                    text.push_back('C');

                if (Value & PsrMask::LowZero)
                    text.push_back('Z');

                if (Value & PsrMask::LowNegative)
                    text.push_back('N');
            }
            else
            {
                text.push_back('0');
            }
        }
    }

    if (hasText == false)
    {
        Ag::FormatInfo fmt = Ag::FormatInfo::getNeutral();

        if (Value > 9)
        {
            // Show larger values as hex.
            fmt.setRadix(16);
            text.append("0x");
        }

        Ag::appendValue(fmt, text, Value);
    }

    return text;
}

//! @brief Produces a string which produces the identifier of the processor
//! element the constraint refers to.
std::string Constraint::idToString() const
{
    std::string text;
    size_t index;

    using ElementMap = Ag::LinearSortedMap<SystemElement, std::string_view>;
    using ElementIndexer = Ag::LinearSortedMapIndexer<SystemElement, std::string_view>;
    ElementMap elementPrefixes;

    if (elementPrefixes.isEmpty())
    {
        ElementIndexer indexer(elementPrefixes);

        elementPrefixes.push_back(SystemElement::CoreRegister, "R");
        elementPrefixes.push_back(SystemElement::CoProcessor, "CP");
        elementPrefixes.push_back(SystemElement::CoProcRegister, "CP");
        elementPrefixes.push_back(SystemElement::FpaRegister, "F");
        elementPrefixes.push_back(SystemElement::PhysicalByte, "PBYTE");
        elementPrefixes.push_back(SystemElement::PhysicalHalfWord, "PHWORD");
        elementPrefixes.push_back(SystemElement::PhysicalWord, "PWORD");
        elementPrefixes.push_back(SystemElement::LogicalByte, "LBYTE");
        elementPrefixes.push_back(SystemElement::LogicalHalfWord, "LHWORD");
        elementPrefixes.push_back(SystemElement::LogicalWord, "LWORD");
        elementPrefixes.push_back(SystemElement::MappedPhysicalAddress, "MAP");
    }

    text.reserve(32);
    std::string_view prefix;

    if (elementPrefixes.tryFind(Element, prefix))
    {
        text.assign(prefix);
    }

    switch (Element)
    {
    case SystemElement::CoreRegister:
    case SystemElement::CoProcessor:
    case SystemElement::FpaRegister:
        Ag::appendValue(Ag::FormatInfo::getNeutral(), text, ElementIndex);
        break;

    case SystemElement::CoProcRegister:
        Ag::appendValue(Ag::FormatInfo::getNeutral(), text, ElementIndex >> 4);
        text.append(".CR");
        Ag::appendValue(Ag::FormatInfo::getNeutral(), text, ElementIndex & 0x0F);
        break;

    case SystemElement::PhysicalByte:
    case SystemElement::PhysicalHalfWord:
    case SystemElement::PhysicalWord:
    case SystemElement::LogicalByte:
    case SystemElement::LogicalHalfWord:
    case SystemElement::LogicalWord:
    case SystemElement::MappedPhysicalAddress: {
        Ag::FormatInfo fmt = Ag::FormatInfo::getNeutral();
        fmt.setRadix(16);

        text.push_back('[');
        Ag::appendValue(fmt, text, ElementIndex);
        text.push_back(']');
    } break;

    case SystemElement::SystemRegister:
        if (getSystemRegisterType().tryFindSymbolIndex(Ag::fromScalar<SystemRegister>(ElementIndex), index))
        {
            const auto &symbol = getSystemRegisterType().getSymbols().at(index);

            text.assign(symbol.getSymbol());
        }
        else
        {
            text.assign("(unknown system register)");
        }
        break;

    case SystemElement::Max:
    default:
        text.assign("(unknown system element)");
        break;
    }

    return text;
}

////////////////////////////////////////////////////////////////////////////////
// TestLocation Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty source code location.
TestLocation::TestLocation() :
    SourceLine(0)
{
}

//! @brief Constructs an object defining a location in source code.
//! @param[in] sourceFile The name of the source file, probably from the
//! __FILE__ pre-processor macro.
//! @param[in] sourceLine The 1-based index of the source line, probably from
//! the __LINE__ pre-processor macro.
TestLocation::TestLocation(const char *sourceFile, int sourceLine) :
    SourceFile(sourceFile),
    SourceLine(sourceLine)
{
}

//! @brief Appends a rendering of the test location to a text buffer.
//! @param[out] buffer The buffer to append to.
void TestLocation::appendToString(std::string &buffer) const
{
    Ag::appendFormat(Ag::FormatInfo::getNeutral(), "{0}({1})",
                     buffer, { SourceFile, SourceLine });
}

////////////////////////////////////////////////////////////////////////////////
// CoreTestParams Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object describing the conditions of a processor test.
//! @param[in] loc The location in source code at which the test was defined.
//! @param[in] name The symbolic name of the test point.
//! @param[in] inputs The constraints expression describing the state of the
//! system under test before performing the test.
//! @param[in] outputs The constraints expression describing the expected state
//! of the system under test after performing the test.
//! @param[in] assemblerCode The assembly language source code to assemble and
//! execute on the system under test.
CoreTestParams::CoreTestParams(const TestLocation &loc,
                               const std::string_view &name,
                               const std::string_view &inputs,
                               const std::string_view &outputs,
                               const std::string_view &assemblerCode) :
    Name(name),
    Inputs(inputs),
    ExpectedOutputs(outputs),
    Code(assemblerCode),
    Location(loc)
{
}

//! @brief Appends a description of the test point to a buffer.
//! @param[out] buffer The buffer to append the description to.
void CoreTestParams::appendIdToString(std::string &buffer) const
{
    Location.appendToString(buffer);
    buffer.push_back(':');
    buffer.push_back(' ');
    buffer.append(Name);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to parse a constraint string into a collection of constraint
//! objects for use in unit tests.
//! @param[in] loc The location of the test definition in source code.
//! @param[in] text The constraint expression to parse.
//! @param[out] constraints Receives the parsed constraint object.
//! @returns A test result detailing the failure if the constraint string was
//! found to be invalid.
testing::AssertionResult parseConstraints(const TestLocation &loc,
                                          const std::string_view &text,
                                          ConstraintCollection &constraints)
{
    // ID '=' value (, ID '=' value)*
    // ([A-Za-z_]+([0-9]*|'['[0-9A-Fa-f]+']') '=' ((0[Xx][0-9A-Fa-f]+) | ([0-9]+) | (0[Bb][0-1]+) | [A-Za-z_]+) ',' ...
    enum class State
    {
        BeforeIdPrefix,
        InIdPrefix,
        InIdDigits,

        AfterAddrBracket,
        InAddrDigits,
        AfterAddrDigits,

        BeforeEquals,
        AfterEquals,
        AfterLeadingZero,
        BeforeFirstHexDigit,
        InHexDigits,
        BeforeFirstBinaryDigit,
        InBinaryDigits,
        InDecimalDigits,
        InSymbol,
        BeforeSeparator,
        AfterSeparator,
        Error,
    };

    ParsedConstraint context;
    std::string error;
    State state = State::BeforeIdPrefix;

    context.reset();
    constraints.clear();

    for (size_t i = 0; (state != State::Error) && (i < text.length()); ++i)
    {
        char next = text[i];
        bool itemComplete = false;

        switch (state)
        {
        case State::BeforeIdPrefix:
            if (std::isalpha(next))
            {
                context.Id.push_back(next);
                state = State::InIdPrefix;
            }
            else if (std::isspace(next) == 0)
            {
                error.assign("Unknown character '");
                error.push_back(next);
                error.append("' before element ID.");
                state = State::Error;
            }
            break;

        case State::InIdPrefix:
            if ((next == '_') || (next == '.') || std::isalpha(next))
            {
                context.Id.push_back(next);
            }
            else if (next == '[')
            {
                state = State::AfterAddrBracket;
                context.IsAddress = true;
            }
            else if (std::isdigit(next))
            {
                context.IdOrdinal = static_cast<uint32_t>(next - '0');
                state = State::InIdDigits;
            }
            else if (next == '=')
            {
                // No ID ordinal.
                state = State::AfterEquals;
            }
            else if ((next == '\n') || (next == ','))
            {
                // The prefix ended with no value.
                context.IsScalar = false;
                itemComplete = true;
                state = State::AfterSeparator;
            }
            else if (std::isspace(next))
            {
                state = State::BeforeEquals;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after element identifier.");
                state = State::Error;
            }
            break;

        case State::InIdDigits:
            if (std::isdigit(next))
            {
                context.IdOrdinal *= 10;
                context.IdOrdinal += static_cast<uint32_t>(next - '0');
            }
            else if (next == '=')
            {
                state = State::AfterEquals;
            }
            else if ((next == '\n') || (next == ','))
            {
                // The ID ended with no value.
                context.IsScalar = false;
                itemComplete = true;
                state = State::AfterSeparator;
            }
            else if (std::isspace(next))
            {
                state = State::BeforeEquals;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after element identifier.");
                state = State::Error;
            }
            break;

        case State::AfterAddrBracket:
            if (next == ']')
            {
                error.assign("Empty address specification");
                state = State::Error;
            }
            else if ((next >= '0') && (next <= '9'))
            {
                context.IdOrdinal = static_cast<uint32_t>(next - '0');
                state = State::InAddrDigits;
            }
            else if ((next >= 'A') && (next <= 'F'))
            {
                context.IdOrdinal = static_cast<uint32_t>(next - 'A') + 10;
                state = State::InAddrDigits;
            }
            else if ((next >= 'a') && (next <= 'f'))
            {
                context.IdOrdinal = static_cast<uint32_t>(next - 'a') + 10;
                state = State::InAddrDigits;
            }
            else if (std::isspace(next) == 0)
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' in element address specification.");
                state = State::Error;
            }
            break;

        case State::InAddrDigits:
            if (next == ']')
            {
                state = State::BeforeEquals;
            }
            else if ((next >= '0') && (next <= '9'))
            {
                context.IdOrdinal <<= 4;
                context.IdOrdinal += static_cast<uint32_t>(next - '0');
                state = State::InAddrDigits;
            }
            else if ((next >= 'A') && (next <= 'F'))
            {
                context.IdOrdinal <<= 4;
                context.IdOrdinal += static_cast<uint32_t>(next - 'A') + 10;
                state = State::InAddrDigits;
            }
            else if ((next >= 'a') && (next <= 'f'))
            {
                context.IdOrdinal <<= 4;
                context.IdOrdinal += static_cast<uint32_t>(next - 'a') + 10;
                state = State::InAddrDigits;
            }
            else if (std::isspace(next))
            {
                state = State::AfterAddrDigits;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' in element address specification.");
                state = State::Error;
            }
            break;

        case State::AfterAddrDigits:
            if (next == ']')
            {
                state = State::BeforeEquals;
            }
            else if (std::isspace(next) == 0)
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after element address specification.");
                state = State::Error;
            }
            break;

        case State::BeforeEquals:
            if (next == '=')
            {
                state = State::AfterEquals;
            }
            else if ((next == '\n') || (next == ','))
            {
                // The ID ended with no value.
                context.IsScalar = false;
                itemComplete = true;
                state = State::AfterSeparator;
            }
            else if (std::isspace(next) == 0)
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after element identifier.");
                state = State::Error;
            }
            break;

        case State::AfterEquals:
            if (next == '0')
            {
                context.IsScalar = true;
                state = State::AfterLeadingZero;
            }
            else if (std::isdigit(next))
            {
                context.IsScalar = true;
                context.ScalarValue = static_cast<uint32_t>(next - '0');
                state = State::InDecimalDigits;
            }
            else if ((next == '_') || (next == '.') || std::isalpha(next))
            {
                context.IsScalar = false;
                context.SymbolValue.push_back(next);
                state = State::InSymbol;
            }
            else if (std::isspace(next) == 0)
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' before value.");
                state = State::Error;
            }
            break;

        case State::AfterLeadingZero:
            if ((next == 'X') || (next == 'x'))
            {
                state = State::BeforeFirstHexDigit;
            }
            else if ((next == 'B') || (next == 'b'))
            {
                state = State::BeforeFirstBinaryDigit;
            }
            else if (std::isdigit(next))
            {
                context.ScalarValue = static_cast<uint32_t>(next - '0');
                state = State::InDecimalDigits;
            }
            else if ((next == ',') || (next == '\n'))
            {
                // Process completed token.
                state = State::AfterSeparator;
                itemComplete = true;
            }
            else if (std::isspace(next))
            {
                // The scalar value was 0.
                state = State::BeforeSeparator;
                itemComplete = true;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after value.");
                state = State::Error;
            }
            break;

        case State::BeforeFirstHexDigit:
            if ((next >= 'A') && (next <= 'F'))
            {
                context.ScalarValue = (next - 'A' + 10);
                state = State::InHexDigits;
            }
            else if ((next >= 'a') && (next <= 'a'))
            {
                context.ScalarValue = (next - 'a' + 10);
                state = State::InHexDigits;
            }
            else if (std::isdigit(next))
            {
                context.ScalarValue = (next - '0');
                state = State::InHexDigits;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after hexadecimal prefix.");
                state = State::Error;
            }
            break;

        case State::InHexDigits:
            if ((next >= 'A') && (next <= 'F'))
            {
                context.ScalarValue = (context.ScalarValue << 4) + (next - 'A' + 10);
            }
            else if ((next >= 'a') && (next <= 'a'))
            {
                context.ScalarValue = (context.ScalarValue << 4) + (next - 'a' + 10);
            }
            else if (std::isdigit(next))
            {
                context.ScalarValue = (context.ScalarValue << 4) + (next - '0');
            }
            else if ((next == ',') || (next == '\n'))
            {
                // Process completed token.
                state = State::AfterSeparator;
                itemComplete = true;
            }
            else if (std::isspace(next))
            {
                // The scalar value was 0.
                state = State::BeforeSeparator;
                itemComplete = true;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after hexadecimal value.");
                state = State::Error;
            }
            break;

        case State::BeforeFirstBinaryDigit:
            if ((next == '0') || (next == '1'))
            {
                context.ScalarValue = static_cast<uint32_t>(next - '0');
                state = State::InBinaryDigits;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after binary prefix.");
                state = State::Error;
            }
            break;

        case State::InBinaryDigits:
            if ((next == '0') || (next == '1'))
            {
                context.ScalarValue = (context.ScalarValue << 1) | static_cast<uint32_t>(next - '0');
            }
            else if ((next == ',') || (next == '\n'))
            {
                // Process completed token.
                state = State::AfterSeparator;
                itemComplete = true;
            }
            else if (std::isspace(next))
            {
                // The scalar value was 0.
                state = State::BeforeSeparator;
                itemComplete = true;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after binary value.");
                state = State::Error;
            }
            break;

        case State::InDecimalDigits:
            if ((next >= '0') && (next <= '9'))
            {
                context.ScalarValue = (context.ScalarValue * 10) + static_cast<uint32_t>(next - '0');
            }
            else if ((next == ',') || (next == '\n'))
            {
                // Process completed token.
                state = State::AfterSeparator;
                itemComplete = true;
            }
            else if (std::isspace(next))
            {
                state = State::BeforeSeparator;
                itemComplete = true;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after decimal.");
                state = State::Error;
            }
            break;

        case State::InSymbol:
            if ((next == '_') || (next == '.') || std::isalnum(next))
            {
                context.SymbolValue.push_back(next);
            }
            else if ((next == ',') || (next == '\n'))
            {
                // Process completed token.
                state = State::AfterSeparator;
                itemComplete = true;
            }
            else if (std::isspace(next))
            {
                // The scalar value was 0.
                state = State::BeforeSeparator;
                itemComplete = true;
            }
            else
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' after decimal.");
                state = State::Error;
            }
            break;

        case State::BeforeSeparator:
            if ((next == ',') || (next == '\n'))
            {
                // Process completed token.
                state = State::AfterSeparator;
            }
            else if (std::isspace(next) == 0)
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' between constraints.");
                state = State::Error;
            }
            break;

        case State::AfterSeparator:
            if (std::isalpha(next))
            {
                context.reset();
                context.Id.push_back(next);
                state = State::InIdPrefix;
            }
            else if (std::isspace(next) == 0)
            {
                error.assign("Unexpected character '");
                error.push_back(next);
                error.append("' before constraint.");
                state = State::Error;
            }
            break;

        case State::Error:
        default:
            break;
        }

        if (itemComplete)
        {
            if (tryProcessConstraint(constraints, context, error))
            {
                context.reset();
            }
            else
            {
                // Processing failed.
                state = State::Error;
            }
        }
    }

    switch (state)
    {
    case State::BeforeIdPrefix:
    case State::AfterAddrBracket:
    case State::InAddrDigits:
    case State::AfterAddrDigits:
    case State::AfterEquals:
    case State::BeforeFirstHexDigit:
    case State::BeforeFirstBinaryDigit:
    case State::AfterSeparator:
    case State::Error:
    default:
        break;

    // All states where we can legitimately run out of characters and
    // still have a fully parsed constraint.
    case State::InIdPrefix:
    case State::InIdDigits:
    case State::BeforeEquals:
    case State::AfterLeadingZero:
    case State::InHexDigits:
    case State::InBinaryDigits:
    case State::InDecimalDigits:
    case State::InSymbol:
    case State::BeforeSeparator:
        if (tryProcessConstraint(constraints, context, error) == false)
        {
            // Processing failed.
            state = State::Error;
        }
        break;
    }

    if ((state == State::Error) ||
        (compileConstraints(constraints, error) == false))
    {
        testing::AssertionResult result = testing::AssertionFailure();

        std::string message("Failed to parse constraint at: ");
        loc.appendToString(message);
        message.push_back('\n');
        message.append(error);

        result << message;

        return result;
    }
    else
    {
        return testing::AssertionSuccess();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Outputs a summary of a set of test parameters which appear in the
//! VisualStudio Test Explorer panel.
//! @param[in] os The stream to write the text to.
//! @param[in] rhs The test parameters to render as text.
std::ostream &operator<<(std::ostream &os, const CoreTestParams &rhs)
{
    std::string output;
    rhs.Location.appendToString(output);
    output.push_back(':');
    output.push_back(' ');
    output.append(rhs.Name);
    return os << output;
}

//! @brief Extracts the name of a set of execution test parameters.
//! @param[in] rhs The test parameters to render as text.
//! @return The name of the test.
std::string CoreTestParamsName::operator()(const testing::TestParamInfo<typename CoreTestParams> &rhs) const
{
    return std::string(rhs.param.Name);
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


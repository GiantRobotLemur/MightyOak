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
#include "Ag/Core/Utils.hpp"
#include "ArmEmu.hpp"

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
    uint8_t IdOrdinal;
    bool IsScalar;

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
    }
};

struct ElementInfo
{
    SystemElement Element;
    uint8_t OrdinalMin;
    uint8_t OrdinalMax;
    uint8_t OrdinalOffset;

    ElementInfo() :
        Element(SystemElement::Max),
        OrdinalMin(0),
        OrdinalMax(0),
        OrdinalOffset(0)
    {
    }

    ElementInfo(SystemElement element, uint8_t maxOrdinal,
                uint8_t minOrdinal = 0, uint8_t offset = 0) :
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
        Item(SystemRegister::ProcessorMode, "Mode", "Processor Mode"),
        Item(SystemRegister::IrqStatus, "IrqStatus", "Interrupt Status"),
        Item(SystemRegister::IrqMask, "IrqMask", "Interrupt Mask")
    });

    return instance;
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
    }

    SystemElement element = SystemElement::Max;
    uint8_t elementOrdinal = 0;

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

    if (element == SystemElement::Max)
    {
        error.assign("Unknown system component identifier '");
        error.append(parsed.Id);
        error.push_back('\'');
        error.push_back('.');
        return false;
    }

    uint32_t elementValue = 0;

    if ((element == SystemElement::SystemRegister) &&
        (elementOrdinal == Ag::toScalar(SystemRegister::ProcessorMode)))
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
    uint8_t cpid = 0xFF;

    for (Constraint &item : constraints)
    {
        if (item.Location == SystemElement::CoProcessor)
        {
            cpid = item.Index;
        }
        else if (item.Location == SystemElement::CoProcRegister)
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
                item.SubIndex = item.Index;
                item.Index = cpid;
            }
        }
    }

    // Remove co-processor references now that registers have been assigned.
    auto last = std::remove_if(constraints.begin(), constraints.end(),
                               [](const Constraint &rhs) { return rhs.Location == SystemElement::CoProcessor; });
    constraints.erase(last, constraints.end());

    // Sort constraints into a fixed order.
    std::sort(constraints.begin(), constraints.end());

    // Ensure there are no duplicates.
    last = std::unique(constraints.begin(), constraints.end());

    if (last == constraints.end())
    {
        // Ensure that all co-processor register references inherited a
        // co-processor identifier.
        for (auto &constraint : constraints)
        {
            // Reset the sub-index constraints.
            if (constraint.SubIndex == 0xFF)
            {
                if (constraint.Location == SystemElement::CoProcRegister)
                {
                    error.assign(constraint.idToString());
                    error.append(" was not assigned to a parent co-processor.");
                    return false;
                }
                else
                {
                    constraint.SubIndex = 0;
                }
            }
        }
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
    Location(SystemElement::Max),
    Index(0),
    SubIndex(0)
{
}

//! @brief Constructs an initialised constraint.
//! @param[in] location The location within the processor the value is located.
//! @param[in] index The 0-based register index to access.
//! @param[in] value The value to load, store or be expected.
Constraint::Constraint(SystemElement location, uint8_t index, uint32_t value) :
    Value(value),
    Location(location),
    Index(index),
    SubIndex(0xFF)
{
}

//! @brief Compares constraints based on the location they reference.
//! @param[in] rhs The constraint to compare against.
//! @return True if the current value references the same location as rhs.
bool Constraint::operator==(const Constraint &rhs) const
{
    return (Location == rhs.Location) &&
           (Index == rhs.Index) &&
           (SubIndex == rhs.SubIndex);
}

//! @brief Compares constraints so that they are ordered by location and index.
//! @param[in] rhs The constraint to compare against.
//! @return True if the current value is less than rhs.
bool Constraint::operator<(const Constraint &rhs) const
{
    bool isLess = false;

    if (Location == rhs.Location)
    {
        if (Index == rhs.Index)
        {
            isLess = SubIndex < rhs.SubIndex;
        }
        else
        {
            isLess = Index < rhs.Index;
        }
    }
    else
    {
        isLess = Location < rhs.Location;
    }

    return isLess;
}

//! @brief Produces a text string describing the constraint.
//! @return A string in a Key=Value pair format.
std::string Constraint::toString() const
{
    std::string text = idToString();

    text.push_back('=');

    if ((Location == SystemElement::SystemRegister) &&
        (Ag::fromScalar<SystemRegister>(Index) == SystemRegister::ProcessorMode))
    {
        const auto &modeType = getProcessorModeType();

        text.append(modeType.toString(Ag::forceFromScalar<ProcessorMode>(Value)));
    }
    else
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

    text.reserve(32);

    switch (Location)
    {
    case SystemElement::CoreRegister:
        text.push_back('R');
        Ag::appendValue(Ag::FormatInfo::getNeutral(), text, Index);
        break;

    case SystemElement::CoProcessor:
        text.append("CP");
        Ag::appendValue(Ag::FormatInfo::getNeutral(), text, Index);
        break;

    case SystemElement::CoProcRegister:
        text.append("CP");
        Ag::appendValue(Ag::FormatInfo::getNeutral(), text, Index);
        text.append(".CR");
        Ag::appendValue(Ag::FormatInfo::getNeutral(), text, SubIndex);
        break;

    case SystemElement::FpaRegister:
        text.push_back('F');
        Ag::appendValue(Ag::FormatInfo::getNeutral(), text, Index);
        break;

    case SystemElement::SystemRegister:
        if (getSystemRegisterType().tryFindSymbolIndex(Ag::fromScalar<SystemRegister>(Index), index))
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
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Attempts to parse a constraint string into a collection of constraint
//! objects for use in unit tests.
//! @param[in] text The constraint expression to parse.
//! @param[out] constraints Receives the parsed constraint object.
//! @returns A test result detailing the failure if the constraint string was
//! found to be invalid.
testing::AssertionResult parseConstraints(const std::string_view &text,
                                          ConstraintCollection &constraints)
{
    // ID '=' value (, ID '=' value)*
    // ([A-Za-z_]+[0-9]* '=' ((0[Xx][0-9A-Fa-f]+) | ([0-9]+) | (0[Bb][0-1]+) | [A-Za-z_]+) ',' ...
    enum class State
    {
        BeforeIdPrefix,
        InIdPrefix,
        InIdDigits,
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
            else if (std::isdigit(next))
            {
                context.IdOrdinal = static_cast<uint8_t>(next - '0');
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
                context.IdOrdinal = (context.IdOrdinal * 10) + static_cast<uint8_t>(next - '0');
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

        result << "Failed to parse constraint: " << error;

        return result;
    }
    else
    {
        return testing::AssertionSuccess();
    }
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


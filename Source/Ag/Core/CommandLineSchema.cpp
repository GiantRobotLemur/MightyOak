//! @file CommandLineSchema.cpp
//! @brief The definition of an object which defines which command line options
//! are valid.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cwctype>

#include <algorithm>
#include <map>
#include <unordered_map>

#include "Platform.hpp"
#include "Ag/Core/CodePoint.hpp"
#include "Ag/Core/CommandLineSchema.hpp"
#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Format.hpp"
#include "Ag/Core/FsPath.hpp"
#include "Ag/Core/Utf.hpp"
#include "Ag/Core/LinearSortedMap.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/Variant.hpp"


namespace Ag {
namespace Cli {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
using OptionDefinitionCollection = std::vector<OptionDefinition>;

//! @brief A shareable object used to incrementally construct the definition of
//! a set of valid command line options.
class SchemaBuilderPrivate :
    public std::enable_shared_from_this<SchemaBuilderPrivate>
{
public:
    // Public Types
    using StringToIndexMap = std::unordered_map<String, size_t>;
    using CharToIndexMap = std::map<char32_t, size_t>;
    using IdToIndexMap = std::map<uint32_t, size_t>;

private:
    // Internal Fields
    CharToIndexMap _optionIndexByShortOption;
    StringToIndexMap _optionIndexByLongOption;
    StringToIndexMap _optionIndexByLongOptionIgnoreCase;
    IdToIndexMap _optionIndexById;
    OptionDefinitionCollection _options;
    String _appName;
    String _appDescription;
    String _valueArgName;
    ValueMultiplicity _valueArgMultiplicity;

    // Internal Functions
    //! @brief Ensures that a definition exists for a command line option with
    //! the specified numeric identifier.
    //! @param[in] id The identifier to check for.
    //! @returns The index of the matching option definition.
    //! @throws OperationException If id does not have a corresponding definition.
    size_t verifyOptionId(uint32_t id)
    {
        auto mapping = _optionIndexById.find(id);
        if (mapping == _optionIndexById.end())
        {
            String detail = String::format("A command line option ID=#{0} has "
                                           "not yet been defined.", { id });
            throw OperationException(detail);
        }
        else
        {
            return mapping->second;
        }
    }
public:
    // Construction
    SchemaBuilderPrivate() :
        _valueArgMultiplicity(ValueMultiplicity::Many)
    {
    }

    // Accessors
    string_cref_t getAppName() const { return _appName; }

    string_cref_t getAppDescription() const { return _appDescription; }

    ValueMultiplicity getValueArgumentCount() const { return _valueArgMultiplicity; }

    string_cref_t getValueArgumentName() const { return _valueArgName; }

    const  CharToIndexMap &getShortFormIndex() const
    {
        return _optionIndexByShortOption;
    }

    const StringToIndexMap &getLongFormIndex() const
    {
        return _optionIndexByLongOption;
    }

    const StringToIndexMap &getLongFormIgnoreCaseIndex() const
    {
        return _optionIndexByLongOptionIgnoreCase;
    }

    const OptionDefinitionCollection &getOptionDefinitions() const
    {
        return _options;
    }

    // Operations
    void setAppName(string_cref_t name)
    {
        _appName = name;
    }

    void setAppDescription(string_cref_t description)
    {
        _appDescription = description;
    }

    //! @brief Defines the description of non-option arguments.
    //! @param[in] name The name to use for value arguments when describing the
    //! CLI interface.
    //! @param[in] multiplicity The number of value arguments allowed.
    void defineValueArgument(utf8_cptr_t name, ValueMultiplicity multiplicity)
    {
        _valueArgName = name;
        _valueArgMultiplicity = multiplicity;
    }

    //! @brief Defines a command line option.
    //! @param[in] option The option definition to add to the schema.
    void addOption(const OptionDefinition &option)
    {
        auto mapping = _optionIndexById.find(option.getId());

        if (mapping == _optionIndexById.end())
        {
            _optionIndexById[option.getId()] = _options.size();
            _options.push_back(option);
        }
        else
        {
            String detail = String::format("A command line option ID=#{0} "
                                           "has already been defined.",
                                           { option.getId() });
            throw OperationException(detail);
        }
    }

    //! @brief Associates a token with a command line option definition.
    //! @param[in] id The unique identifier of the previously defined option.
    //! @param[in] shortOption The character used to express the option.
    void defineAlias(uint32_t id, char32_t shortOption)
    {
        size_t index = verifyOptionId(id);

        auto pos = _optionIndexByShortOption.find(shortOption);

        if (pos == _optionIndexByShortOption.end())
        {
            // The option alias doesn't exist, add it.
            _optionIndexByShortOption[shortOption] = index;
        }
        else if (pos->second != id)
        {
            // The option alias does exist and is associated with a different
            // option.
            String detail = String::format("A command line option '-{0}' is "
                                           "already associated with option ID=#{1}.",
                                           { shortOption, id });
            throw OperationException(detail);
        }
    }

    //! @brief Associates a token with a command line option definition.
    //! @param[in] id The unique identifier of the previously defined option.
    //! @param[in] longOption The token used to express the option.
    //! @param[in] isCaseSensitive True to indicate that the token should be
    //! recognised as-is, false to recognise it without regard for case.
    void defineAlias(uint32_t id, const String &longOption, bool isCaseSensitive)
    {
        size_t index = verifyOptionId(id);

        StringToIndexMap &map = isCaseSensitive ? _optionIndexByLongOption :
                                                  _optionIndexByLongOptionIgnoreCase;
        String key = isCaseSensitive ? longOption : longOption.toUpper();

        auto pos = map.find(key);

        if (pos == map.end())
        {
            // The option alias doesn't exist, add it.
            map[key] = index;
        }
        else if (pos->second != index)
        {
            // The option alias does exist and is associated with a different
            // option.
            String detail = String::format("A command line option '--{0}' is already "
                                           "associated with option ID=#{1}.",
                                           { longOption, id });
            throw OperationException(detail);
        }
    }
};

//! @brief A shareable object defining a command line option schema.
class SchemaPrivate :
    public std::enable_shared_from_this<SchemaPrivate>
{
private:
    // Internal Fields
    std::vector<OptionDefinition> _definitions;
    LinearSortedMap<char32_t, size_t> _shortForms;
    LinearSortedMap<String, size_t> _longForms;
    LinearSortedMap<String, size_t> _longFormsIgnoreCase;
    String _appName;
    String _appDescription;
    String _valueArgName;
    ValueMultiplicity _valueArgMultiplicity;

    // Internal Functions
    size_t calculateOptionLength(size_t optionIndex,
                                 LinearSortedMap<size_t, char32_t> shortOptionsById,
                                 LinearSortedMap<size_t, String> longOptionsById,
                                 size_t &mappingCount) const
    {
        size_t maxLength = 0;
        mappingCount = 0;

        const OptionDefinition &opt = _definitions[optionIndex];

        if (opt.getDescription().isEmpty() == false)
        {
            // The option is documented.

            // Calculate the length of the bound value.
            size_t valueLength = 0;

            if (opt.getValueRequired() == OptionValue::Optional)
            {
                // Value: " [<%>]"
                valueLength = opt.getValueName().getPrintLength() + 5;
            }
            else if (opt.getValueRequired() == OptionValue::Mandatory)
            {
                // Value: " <%>"
                valueLength = opt.getValueName().getPrintLength() + 3;
            }

            auto shortMappings = shortOptionsById.findRange(optionIndex);
            auto longMappings = longOptionsById.findRange(optionIndex);
            mappingCount = shortMappings.getCount() + longMappings.getCount();

            // We don't need to calculate the length of each short mapping as
            // they are fixed. Just calculate a length if one exists.
            size_t lastLength = 0;
            size_t newlinePremium = (mappingCount > 1) ? 2 : 0;

            if (shortMappings.isEmpty() == false)
            {
                // Calculate the size of "-% <value>".
                lastLength = 2 + valueLength + newlinePremium;
                maxLength = std::max(maxLength, lastLength);
            }

            for (const auto &longMapping : longMappings)
            {
                // Calculate the size of "--% <value>".
                lastLength = longMapping.second.getPrintLength() + 2;

                maxLength = std::max(maxLength, lastLength);
            }

            // Assume the last element will be on a line with the value.
            maxLength = std::max(maxLength,
                                 lastLength + valueLength + newlinePremium);
        }

        return maxLength;
    }
public:
    // Construction
    //! @brief Constructs an immutable schema from an incrementally defined one.
    //! @param[in] builder The incrementally defined schema to copy.
    SchemaPrivate(const SchemaBuilderPrivate &builder) :
        _definitions(builder.getOptionDefinitions()),
        _shortForms(builder.getShortFormIndex()),
        _longForms(builder.getLongFormIndex()),
        _longFormsIgnoreCase(builder.getLongFormIgnoreCaseIndex()),
        _appName(builder.getAppName()),
        _appDescription(builder.getAppDescription()),
        _valueArgName(builder.getValueArgumentName()),
        _valueArgMultiplicity(builder.getValueArgumentCount())
    {
        if (_appName.isEmpty())
        {
            // Use the base file name as the default application name.
            Fs::Path filePath(getProgramFileName());

            _appName = filePath.getFileBaseName();
        }

        if (_valueArgName.isEmpty())
        {
            _valueArgName = "value";
        }
    }

    ~SchemaPrivate() = default;

    // Accessors
    //! @brief Gets the number of non-option arguments allowed by the schema.
    ValueMultiplicity getValueMultiplicity() const
    {
        return _valueArgMultiplicity;
    }

    // Operations
    //! @brief Searches for an option expressed in the short form.
    //! @param[in] shortForm The character used to express the option.
    //! @param[out] index Receives the 0-based index of the option, if one
    //! was matched.
    //! @retval true An option was found which could be expressed with the
    //! specified character, index defines which.
    //! @retval false No option matches the specified character.
    bool tryFindOption(char32_t shortForm, size_t &index) const
    {
        return _shortForms.tryFind(shortForm, index);
    }

    //! @brief Searches for an option expressed in the short form.
    //! @param[in] longForm The token used to express the option.
    //! @param[out] index Receives the 0-based index of the option, if one
    //! was matched.
    //! @retval true An option was found which could be expressed with the
    //! specified token, index defines which.
    //! @retval false No option matches the specified token.
    bool tryFindOption(const String &longForm, size_t &index) const
    {
        bool isFound = false;
        index = 0;

        if (_longForms.tryFind(longForm, index))
        {
            isFound = true;
        }
        else
        {
            String key = longForm.toUpper();

            isFound = _longFormsIgnoreCase.tryFind(key, index);
        }

        return isFound;
    }

    //! @brief Gets the definition of a command line option previously
    //! identified using tryFindOption().
    //! @param[in] index The index of the option.
    //! @return A reference to the option definition.
    //! @throw ArgumentException If id is not a valid identifier.
    const OptionDefinition &getDefinition(size_t index) const
    {
        if (index >= _definitions.size())
        {
            throw ArgumentException("index");
        }
        else
        {
            return _definitions.at(index);
        }
    }

    //! @brief Uses information in the schema to format help on how to format
    //! command line arguments.
    //! @param[in] maxWidth The optional maximum width of text lines before they
    //! are wrapped. A value of 0 or less implies no wrapping.
    //! @return A string containing multi-line help text describing allowed
    //! combinations of command line options.
    String getHelpText(int maxWidth) const
    {
        std::string buffer;

        if (_appDescription.isEmpty() == false)
        {
            appendAgString(buffer, _appName);
            buffer.push_back(':');
            buffer.push_back(' ');
            appendAgString(buffer, _appDescription);
            buffer.push_back('\n');
        }

        if ((_valueArgMultiplicity != ValueMultiplicity::None) ||
            (_definitions.empty() == false))
        {
            // There should be some arguments to describe.
            if (buffer.empty() == false)
            {
                buffer.push_back('\n');
            }

            buffer.append("Usage: ");
            Fs::Path progPath = getProgramFileName();

            appendAgString(buffer, progPath.getFileBaseName());

            if (_definitions.empty() == false)
            {
                buffer.append(" [options]");
            }

            switch (_valueArgMultiplicity)
            {
            case ValueMultiplicity::None:
            default:
                break;

            case ValueMultiplicity::UpToOne:
                buffer.append(" [<");
                appendAgString(buffer, _valueArgName);
                buffer.append(">]");
                break;

            case ValueMultiplicity::ExactlyOne:
                buffer.append(" <");
                appendAgString(buffer, _valueArgName);
                buffer.push_back('>');
                break;

            case ValueMultiplicity::AtLeastOne:
                buffer.append(" <");
                appendAgString(buffer, _valueArgName);
                buffer.append("> [<");
                appendAgString(buffer, _valueArgName);
                buffer.append("> ...]");
                break;

            case ValueMultiplicity::Many:
                buffer.append(" [<");
                appendAgString(buffer, _valueArgName);
                buffer.append("> [<");
                appendAgString(buffer, _valueArgName);
                buffer.append("> ...]]");
                break;
            }

            buffer.push_back('\n');
        }

        if (_definitions.empty() == false)
        {
            // Create reverse mappings for all options.
            LinearSortedMap<size_t, char32_t> shortOptionsById;
            LinearSortedMap<size_t, String> longOptionsById;

            size_t maxLineLength = (maxWidth < 0) ? std::numeric_limits<size_t>::max() :
                                                    toSize(maxWidth);

            for (const auto &shortMapping : _shortForms)
            {
                shortOptionsById.push_back(shortMapping.second, shortMapping.first);
            }

            shortOptionsById.reindex();

            for (const auto &longMapping : _longForms)
            {
                longOptionsById.push_back(longMapping.second, longMapping.first);
            }

            longOptionsById.reindex();

            // Output option information.
            buffer.append("Options:\n");

            // Calculate the maximum length of any option.
            size_t maxOptionLength = 0;
            size_t maxOptionCount = 0;

            for (size_t i = 0, c = _definitions.size(); i < c; ++i)
            {
                size_t optionCount;
                size_t optLength = calculateOptionLength(i, shortOptionsById,
                                                         longOptionsById, optionCount);

                maxOptionLength = std::max(maxOptionLength, optLength);
                maxOptionCount = std::max(maxOptionCount, optionCount);
            }

            // Ensure there is enough space for options and some description.
            maxLineLength = std::max(maxOptionLength + 20, maxLineLength);

            // Output the options, formatting as required.
            StringCollection tokens;
            std::string lineBuffer;
            tokens.reserve(maxOptionCount);

            for (size_t i = 0, c = _definitions.size(); i < c; ++i)
            {
                const OptionDefinition &opt = _definitions[i];

                if (opt.getDescription().isEmpty())
                    continue;

                tokens.clear();
                auto shortOptRange = shortOptionsById.findRange(opt.getId());
                auto longOptRange = longOptionsById.findRange(opt.getId());

                // Accumulate the tokens to appear on each line.
                for (const auto &shortMapping : shortOptRange)
                {
                    lineBuffer.clear();

                    lineBuffer.push_back('-');
                    Utf::appendCodePoint(lineBuffer, shortMapping.second);
                    lineBuffer.push_back(',');

                    tokens.emplace_back(lineBuffer);
                }

                for (const auto &longMapping : longOptRange)
                {
                    lineBuffer.clear();
                    lineBuffer.push_back('-');
                    lineBuffer.push_back('-');
                    appendAgString(lineBuffer, longMapping.second);
                    lineBuffer.push_back(',');

                    tokens.emplace_back(lineBuffer);
                }

                if (tokens.empty() == false)
                {
                    // Doctor the last item to remove the trailing separator
                    // and possibly add a bound value.
                    lineBuffer.clear();
                    appendAgString(lineBuffer, tokens.back());
                    tokens.pop_back();
                    lineBuffer.pop_back();

                    if (opt.getValueRequired() == OptionValue::Optional)
                    {
                        lineBuffer.append(" [<");
                        appendAgString(lineBuffer, opt.getValueName());
                        lineBuffer.append(">]");
                    }
                    else if (opt.getValueRequired() == OptionValue::Mandatory)
                    {
                        lineBuffer.push_back(' ');
                        lineBuffer.push_back('<');
                        appendAgString(lineBuffer, opt.getValueName());
                        lineBuffer.push_back('>');
                    }

                    tokens.emplace_back(lineBuffer);
                }

                // Output the options and description in two columns.
                size_t token = 0;
                const size_t optColWidth = maxOptionLength + 1;
                std::u32string description = opt.getDescription().toUtf32();
                size_t descOffset = 0;
                bool isFirstLine = true;

                while ((token < tokens.size()) ||
                       (descOffset != description.size()))
                {
                    size_t charsWritten = 0;
                    bool indent;

                    if (isFirstLine)
                    {
                        isFirstLine = false;
                        indent = false;
                    }
                    else
                    {
                        indent = true;
                        buffer.append(2, ' ');
                        charsWritten = 2;
                    }

                    // Write what option tokens we can fit into the column.
                    while (token < tokens.size())
                    {
                        string_cref_t nextToken = tokens[token];
                        size_t printLength = nextToken.getPrintLength();

                        if ((charsWritten < 1) ||
                            ((charsWritten + printLength) <= maxOptionLength))
                        {
                            // There is enough space to write the token.
                            appendAgString(buffer, nextToken);
                            charsWritten += printLength;
                            ++token;
                        }
                        else
                        {
                            break;
                        }
                    }

                    // Pad to the description column.
                    if (charsWritten < optColWidth)
                    {
                        buffer.append(optColWidth - charsWritten, ' ');

                        charsWritten = optColWidth;
                    }

                    // Now write what we can of the description.

                    // Skip leading white space.
                    while (descOffset < description.size())
                    {
                        if (CodePoint::isWhiteSpace(description[descOffset]))
                        {
                            ++descOffset;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (indent)
                    {
                        // Indent continuation lines.
                        buffer.append(2, ' ');
                        charsWritten += 2;
                    }

                    // Output what words we can, wrapping at a word break.
                    size_t descStart = descOffset;
                    size_t endOffset = descOffset;
                    bool isInWord = true;

                    while ((descOffset < description.size()) &&
                           (charsWritten < maxLineLength))
                    {
                        char32_t codePoint = description[descOffset];

                        if (CodePoint::isWhiteSpace(codePoint))
                        {
                            if (isInWord)
                            {
                                // Its the end of a word, mark it.
                                endOffset = descOffset;
                                isInWord = false;
                            }

                            // All spaces are printable(?).
                            ++charsWritten;
                        }
                        else if (isInWord == false)
                        {
                            // It's the beginning of a new word.
                            isInWord = true;

                            // Only measure printable characters.
                            if (CodePoint::isPrintable(codePoint))
                            {
                                ++charsWritten;
                            }
                        }
                        else if (CodePoint::isPrintable(codePoint))
                        {
                            ++charsWritten;
                        }

                        ++descOffset;
                    }

                    if (charsWritten < maxLineLength)
                    {
                        // We can output the rest of the line.
                        endOffset = descOffset;
                    }

                    if (endOffset > descStart)
                    {
                        // At least one word break was found.
                        Utf::appendToUtf8(buffer,
                                          description.c_str() + descStart,
                                          endOffset - descStart);
                        descOffset = endOffset;
                    }
                    else
                    {
                        // There was no line break, just output what we can.
                        Utf::appendToUtf8(buffer,
                                          description.c_str() + descStart,
                                          descOffset - descStart);
                    }

                    buffer.push_back('\n');
                }
            }

        }

        return String(buffer);
    }
};

////////////////////////////////////////////////////////////////////////////////
// OptionDefinition Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty definition of a command line option.
OptionDefinition::OptionDefinition() :
    _id(~0u),
    _valueRequirement(OptionValue::None)
{
}

//! @brief Constructs an object defining a command line option.
//! @param[in] id The user supplied numeric identifier of the option.
//! @param[in] description The display text description of the option.
//! @param[in] valueName The name of any value bound to the option to appear in
//! help text.
//! @param[in] valueRequired A value determining if the option needs to be
//! followed by an associated value.
OptionDefinition::OptionDefinition(uint32_t id, string_cref_t description,
                                   string_cref_t valueName /*= String::Empty*/,
                                   OptionValue valueRequired /*= OptionValue::None*/) :
    _id(id),
    _valueRequirement(valueRequired),
    _description(description),
    _valueName(valueName)
{
    if ((_valueRequirement != OptionValue::None) &&
        _valueName.isEmpty() && (_description.isEmpty() == false))
    {
        // Use a default value name when describing the option.
        _valueName = "value";
    }
}

//! @brief Gets the user supplied numeric identifier of the option.
uint32_t OptionDefinition::getId() const
{
    return _id;
}

//! @brief Gets the display text description of the option.
string_cref_t OptionDefinition::getDescription() const
{
    return _description;
}

//! @brief Gets a value determining if the option needs to be followed by an
//! associated value.
OptionValue OptionDefinition::getValueRequired() const
{
    return _valueRequirement;
}

//! @brief Gets the display name associated with the value bound to the option.
string_cref_t OptionDefinition::getValueName() const
{
    return _valueName;
}

////////////////////////////////////////////////////////////////////////////////
// Schema Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an immutable schema of command line options.
//! @param[in] builder An object used to incrementally construct the schema.
Schema::Schema(const SchemaBuilderPrivate &builder) :
    _schema(std::make_shared<SchemaPrivate>(builder))
{
}

//! @brief Gets the number of non-option arguments allowed by the schema.
ValueMultiplicity Schema::getValueMultiplicity() const
{
    return _schema->getValueMultiplicity();
}

//! @brief Attempts to find the definition of a command line option associated
//! with a specified short option token.
//! @param[in] shortOption The character representing the option token.
//! @param[out] index Receives the index of the command line option associated
//! with the character.
//! @retval true A matching command line option definition was found.
//! @retval false No matching definition was found.
bool Schema::tryFindShortOption(char32_t shortOption, size_t &index) const
{
    return _schema->tryFindOption(shortOption, index);
}

//! @brief Attempts to find the identifier of a command line option associated
//! with a specified long option token.
//! @param[in] option The token representing the option.
//! @param[out] index Receives the definition of the command line option associated
//! with the character.
//! @retval true A matching command line option definition was found.
//! @retval false No matching definition was found.
bool Schema::tryFindLongOption(const String &option, size_t &index) const
{
    return _schema->tryFindOption(option, index);
}

//! @brief Gets the definition of a command line option.
//! @param[in] index The index of the option definition to obtain.
//! @return A reference to the option definition.
//! @throw An ArgumentException if the identifier does not correspond to a valid
//! option definition.
const OptionDefinition &Schema::getOptionDefinition(size_t index) const
{
    return _schema->getDefinition(index);
}

String Schema::getHelpText(int maxWidth) const
{
    return _schema->getHelpText(maxWidth);
}

////////////////////////////////////////////////////////////////////////////////
// SchemaBuilder Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object ready to incrementally build a description of a
//! valid set of command line options.
SchemaBuilder::SchemaBuilder() :
    _builder(std::make_shared<SchemaBuilderPrivate>())
{
}

void SchemaBuilder::setName(const std::string_view &name)
{
    _builder->setAppName(name);
}

void SchemaBuilder::setDescription(const std::string_view &description)
{
    _builder->setAppDescription(description);
}

//! @brief Describes non-option arguments.
//! @param[in] name The name given to non-option arguments in command line help.
//! @param[in] multiplicity The count of non-option arguments which is valid.
void SchemaBuilder::defineValueArgument(utf8_cptr_t name,
                                        ValueMultiplicity multiplicity)
{
    _builder->defineValueArgument(name, multiplicity);
}

//! @brief Defines a command line option to be accessed via one or more aliases.
//! @param[in] id The numeric identifier of the command line options.
//! @param[in] description A string describing the option for use in the
//! generated command line help text.
//! @param[in] value Indicates whether a value should be associated with the option.
//! @param[in] valueName An optional name attributed to any bound value used
//! when generating the command line help text.
void SchemaBuilder::defineOption(uint32_t id, utf8_cptr_t description,
                                 OptionValue value /*= OptionValue::None*/,
                                 utf8_cptr_t valueName /*= nullptr*/)
{
    OptionDefinition definition(id, String(description),
                                String(valueName), value);

    _builder->addOption(definition);
}

//! @brief Defines an alias used to refer to an option on the command line.
//! @param[in] id The numeric identifier of an option previously registered with
//! defineOption().
//! @param[in] shortForm The single character which will be used on the command
//! line to refer to the specified option.
//! @param[in] isCaseSensitive True to register the character as is, false to
//! register both upper and lower-case forms of the character, if they exist.
void SchemaBuilder::defineAlias(uint32_t id, char32_t shortForm,
                                bool isCaseSensitive /* = true */)
{
    if (isCaseSensitive)
    {
        _builder->defineAlias(id, shortForm);
    }
    else
    {
        Utf::ToWideConverter converter;
        converter.setCodePoint(shortForm);

        wchar_t wc;

        if (converter.tryGetNextCharacter(wc))
        {
            // We only support code points which can be expressed as a single
            // wide character to change case.
            wchar_t upper = static_cast<wchar_t>(towupper(wc));
            wchar_t lower = static_cast<wchar_t>(towlower(wc));

            if (upper == lower)
            {
                _builder->defineAlias(id, shortForm);
            }
            else
            {
                // Again, we only support single wide characters which translate
                // to whole Unicode code points.
                Utf::FromWideConverter toCodePoint;
                char32_t codePoint;
                bool hasError;

                if (toCodePoint.tryConvert(upper, codePoint, hasError))
                {
                    _builder->defineAlias(id, codePoint);
                }

                toCodePoint.reset();

                if (toCodePoint.tryConvert(lower, codePoint, hasError))
                {
                    _builder->defineAlias(id, codePoint);
                }
            }
        }
    }
}

//! @brief Defines an alias used to refer to an option on the command line.
//! @param[in] id The numeric identifier of an option previously registered with
//! defineOption().
//! @param[in] longForm The token which will be used on the command line to
//! refer to the specified option.
//! @param[in] isCaseSensitive True to register the token as is, false to
//! register it as a case-insensitive alias.
void SchemaBuilder::defineAlias(uint32_t id, utf8_cptr_t longForm,
                                           bool isCaseSensitive /* = true */)
{
    if (Utf::isNullOrEmpty(longForm))
    {
        throw ArgumentException("longForm");
    }

    _builder->defineAlias(id, String(longForm), isCaseSensitive);
}

//! @brief Creates a shared, immutable CommandLineSchema object from the current
//! set of command line option definitions.
Schema SchemaBuilder::createSchema() const
{
    return Schema(*_builder.get());
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Cli
////////////////////////////////////////////////////////////////////////////////


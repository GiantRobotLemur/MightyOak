//! @file Messages.cpp
//! @brief The definition of an object which manages messages generated during
//! assembly.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Format.hpp"
#include "AsmTools/Messages.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

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

} // TED

////////////////////////////////////////////////////////////////////////////////
// Location Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a location representing nothing.
Location::Location() :
    LineNo(-1),
    Offset(-1)
{
}

//! @brief Constructs a location representing the beginning of a file.
Location::Location(const String &fileName) :
    FileName(fileName),
    LineNo(1),
    Offset(0)
{
}

//! @brief Determines whether the object represents a valid source code location.
//! @retval true The location is valid.
//! @retval false The object represents a null location.
bool Location::isValid() const
{
    return (LineNo > 0) && (Offset >= 0) && (FileName.isEmpty() == false);
}

////////////////////////////////////////////////////////////////////////////////
// Message Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a message relating to the assembly process.
//! @param[in] severity The nature of the message.
//! @param[in] location The position of the first character in source code
//! which is associated with the message.
//! @param[in] message The message text.
Message::Message(MessageSeverity severity, const Location &location,
                 const String &message) :
    _message(message),
    _location(location),
    _severity(severity)
{
}

//! @brief Gets the nature of the message.
MessageSeverity Message::getSeverity() const
{
    return _severity;
}

//! @brief Gets the position of the first character in source code
//! which is associated with the message.
const Location &Message::getLocation() const
{
    return _location;
}

//! @brief Gets the message text.
const String &Message::getMessage() const
{
    return _message;
}

//! @brief Summarises the message object as a single string.
//! @return A single line summary of the message.
String Message::toString() const
{
    std::string builder;

    switch (_severity)
    {
    case MessageSeverity::Info:
        builder.append("Info: ");
        break;

    case MessageSeverity::Warning:
        builder.append("Warning: ");
        break;

    case MessageSeverity::Error:
        builder.append("Error: ");
        break;

    case MessageSeverity::Fatal:
        builder.append("Fatal: ");
        break;

    default:
        break;
    }

    builder.append(_location.FileName.getUtf8Bytes(),
                   _location.FileName.getUtf8Length());

    FormatInfo defaultFormat;
    appendFormat("({0}+{1}): ", builder, { _location.LineNo, _location.Offset });

    builder.append(_message.getUtf8Bytes(),
                   _message.getUtf8Length());

    return String(builder);
}

////////////////////////////////////////////////////////////////////////////////
// Messages Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an empty set of messages.
Messages::Messages() :
    _hasErrors(false)
{
}

//! @brief Determines whether there are no messages in the collection.
//! @retval true There are no messages in the collection of any type.
//! @retval false There is at least one message in the collection, although
//! possibly not an error.
bool Messages::isEmpty() const
{
    return _messages.empty();
}

//! @brief Indicates whether any errors, fatal or otherwise, are in the collection.
//! @retval true At least one error was logged.
//! @retval false No errors were logged.
bool Messages::hasErrors() const
{
    return _hasErrors;
}

//! @brief Gets the collection of all messages logged.
const Messages::MessageCollection &Messages::getMessages() const
{
    return _messages;
}

//! @brief Appends a static informational message to the collection.
//! @param[in] location The location in source code associated with the message.
//! @param[in] message The message text.
void Messages::appendInfo(const Location &location, const String &message)
{
    _messages.emplace_back(MessageSeverity::Info, location, message);
}

//! @brief Appends a formatted informational message to the collection.
//! @param[in] location The location in source code associated with the message.
//! @param[in] formatSpec The format specification to process.
//! @param[in] values The values to be formatted into the message text.
void Messages::appendInfo(const Location &location, utf8_cptr_t formatSpec,
                          const VariantList &values)
{
    appendFormatted(MessageSeverity::Info, location, formatSpec, values);
}

//! @brief Appends a static warning message to the collection.
//! @param[in] location The location in source code associated with the message.
//! @param[in] message The message text.
void Messages::appendWarning(const Location &location, const String &message)
{
    _messages.emplace_back(MessageSeverity::Warning, location, message);
}

//! @brief Appends a formatted warning message to the collection.
//! @param[in] location The location in source code associated with the message.
//! @param[in] formatSpec The format specification to process.
//! @param[in] values The values to be formatted into the message text.
void Messages::appendWarning(const Location &location, utf8_cptr_t formatSpec,
                             const VariantList &values)
{
    appendFormatted(MessageSeverity::Warning, location, formatSpec, values);
}

//! @brief Appends a static error message to the collection.
//! @param[in] location The location in source code associated with the message.
//! @param[in] message The message text.
void Messages::appendError(const Location &location, const String &message)
{
    _messages.emplace_back(MessageSeverity::Error, location, message);
    _hasErrors = true;
}

//! @brief Appends a formatted error message to the collection.
//! @param[in] location The location in source code associated with the message.
//! @param[in] formatSpec The format specification to process.
//! @param[in] values The values to be formatted into the message text.
void Messages::appendError(const Location &location, utf8_cptr_t formatSpec,
                           const VariantList &values)
{
    appendFormatted(MessageSeverity::Error, location, formatSpec, values);
    _hasErrors = true;
}

//! @brief Appends a static fatal error message to the collection.
//! @param[in] location The location in source code associated with the message.
//! @param[in] message The message text.
void Messages::appendFatal(const Location &location, const String &message)
{
    _messages.emplace_back(MessageSeverity::Fatal, location, message);
    _hasErrors = true;
}

//! @brief Appends a formatted fatal error message to the collection.
//! @param[in] location The location in source code associated with the message.
//! @param[in] formatSpec The format specification to process.
//! @param[in] values The values to be formatted into the message text.
void Messages::appendFatal(const Location &location, utf8_cptr_t formatSpec,
                           const VariantList &values)
{
    appendFormatted(MessageSeverity::Fatal, location, formatSpec, values);
    _hasErrors = true;
}

//! @brief Appends a formatted message to the collection.
//! @param[in] severity The severity to tag the message with.
//! @param[in] location The location in source code the message relates to.
//! @param[in] formatSpec The specification of the message to format.
//! @param[in] values A set of values to be formatted into the final message.
void Messages::appendFormatted(MessageSeverity severity,
                               const Location &location,
                               utf8_cptr_t formatSpec,
                               const VariantList &values)
{
    FormatInfo formatInfo(LocaleInfo::getDisplay());

    String message = String::format(LocaleInfo::getDisplay(), formatSpec, values);

    _messages.emplace_back(severity, location, message);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


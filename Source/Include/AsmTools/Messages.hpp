//! @file Messages.hpp
//! @brief The declaration of an object which manages messages generated during
//! assembly.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_MESSAGES_HPP__
#define __ASM_TOOLS_MESSAGES_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <deque>

#include "Ag/Core.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object describing the position of a character in assembled source code.
struct Location
{
    //! @brief The name of the file being assembled.
    Ag::String FileName;

    //! @brief The 1-based index of the line containing the located character.
    int LineNo;

    //! @brief The 1-based index of the located character within the source line.
    int Offset;

    // Construction
    Location();
    Location(const Ag::String &fileName);

    // Accessors
    bool isValid() const;
};

//! @brief Describes the severity of the issue described by the message.
enum class MessageSeverity
{
    //! @brief A message which merely conveys information.
    Info,

    //! @brief A message which describes a situation which could be erroneous.
    Warning,

    //! @brief A message which describes a recoverable error.
    Error,

    //! @brief A message which describes a non-recoverable error.
    Fatal,

    //! @brief A value only used for bounds checking.
    Max,
};

//! @brief An object defining an individual message generated at assembly time.
class Message
{
public:
    // Construction/Destruction
    Message(MessageSeverity severity, const Location &location,
            const Ag::String &message, size_t ordinal);

    // Accessors
    size_t getOrdinal() const;
    MessageSeverity getSeverity() const;
    const Location &getLocation() const;
    const Ag::String &getMessage() const;

    // Operations
    Ag::String toString() const;
private:
    // Internal Fields
    Ag::String _message;
    Location _location;
    size_t _ordinal;
    MessageSeverity _severity;
};

//! @brief An object which holds all messages generated by the assembly process.
class Messages
{
public:
    // Public Types
    using MessageCollection = std::deque<Message>;

    // Construction/Destruction
    Messages();

    // Accessors
    bool isEmpty() const;
    bool hasErrors() const;
    const MessageCollection &getMessages() const;

    // Operations
    void appendInfo(const Location &location, const Ag::String &message);
    void appendInfo(const Location &location, Ag::utf8_cptr_t formatSpec,
                    const Ag::VariantList &values);
    void appendWarning(const Location &location, const Ag::String &message);
    void appendWarning(const Location &location, Ag::utf8_cptr_t formatSpec,
                       const Ag::VariantList &values);
    void appendError(const Location &location, const Ag::String &message);
    void appendError(const Location &location, Ag::utf8_cptr_t formatSpec,
                     const Ag::VariantList &values);
    void appendFatal(const Location &location, const Ag::String &message);
    void appendFatal(const Location &location, Ag::utf8_cptr_t formatSpec,
                     const Ag::VariantList &values);

    void sort();
private:
    // Internal Functions
    void appendFormatted(MessageSeverity severity, const Location &location,
                         Ag::utf8_cptr_t formatSpec, const Ag::VariantList &values);

    // Internal Fields
    MessageCollection _messages;
    bool _hasErrors;
};

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
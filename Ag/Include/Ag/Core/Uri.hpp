//! @file Ag/Core/Uri.hpp
//! @brief The declaration of classes to manage Universal Resource Identifiers
//! based on https://www.rfc-editor.org/rfc/rfc3986.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_URI_HPP__
#define __AG_CORE_URI_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Exception.hpp"
#include "String.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
struct UriQueryParam
{
    String Name;
    String Value;

    UriQueryParam() = default;
    UriQueryParam(string_cref_t value);
    UriQueryParam(string_cref_t name, string_cref_t value);
    ~UriQueryParam() = default;
};

using UriParamCollection = std::vector<UriQueryParam>;

//! @brief An exception throw describing a failure to parse a URI string.
class UriParseException : public Exception
{
public:
    UriParseException(string_cref_t &uriText, string_cref_t error);
};

//! @brief Expresses how a URI should be expressed as a string.
enum class UriUsage
{
    Display,
    Escaped,
    AsSpecified,
};

//! @brief A class which allows the components of a URI to be manipulated.
class UriBuilder
{
public:
    // Construction/Destruction
    UriBuilder();
    UriBuilder(string_cref_t &uriText);
    ~UriBuilder() = default;

    // Accessors
    bool isEmpty() const;
    string_cref_t getScheme() const;
    void setScheme(const string_cref_t scheme);
    string_cref_t getUserInfo() const;
    void setUserInfo(const string_cref_t userInfo);
    void clearUserInfo();
    string_cref_t getHost() const;
    void setHost(const string_cref_t host);
    void clearHost();
    bool hasPort() const;
    uint16_t getPort() const;
    void setPort(uint16_t port);
    void clearPort();
    bool isRootedPath() const;
    void setRootedPath(bool rooted);
    StringCollection &getPathElements();
    const StringCollection &getPathElements() const;
    UriParamCollection &getQueryParameters();
    const UriParamCollection &getQueryParameters() const;
    string_cref_t getFragment() const;
    void setFragment(const string_cref_t fragment);
    void clearFragment();
    String toString(UriUsage usage = UriUsage::AsSpecified) const;

    // Operations
    void clear();
    void parse(string_cref_t source);
    bool tryParse(string_cref_t source);
    bool tryParse(string_cref_t source, string_ref_t error);
    void escape();
    void unescape();
private:
    // Internal Fields
    String _scheme;
    String _userInfo;
    String _host;
    int _port;
    bool _isRooted;
    StringCollection _path;
    UriParamCollection _queryParams;
    String _fragment;
};

//! @brief An object representing an immutable Universal Resource Identifier.
class Uri
{
    // Construction/Destruction
private:
    Uri(const Uri &rhs, UriUsage usage);
public:
    Uri();
    Uri(string_cref_t source, UriUsage usage = UriUsage::AsSpecified);
    Uri(const UriBuilder &builder, UriUsage usage = UriUsage::AsSpecified);
    ~Uri() = default;

    // Accessors
    bool isEmpty() const;
    String getScheme() const;
    String getUserInfo(UriUsage usage = UriUsage::AsSpecified) const;
    String getHost(UriUsage usage = UriUsage::AsSpecified) const;
    bool hasPort() const;
    uint16_t getPort() const;
    bool isRootedPath() const;
    String getPathElements(UriUsage usage = UriUsage::AsSpecified) const;
    String getQueryParameters(UriUsage usage = UriUsage::AsSpecified) const;
    String getFragment(UriUsage usage = UriUsage::AsSpecified) const;
    String toString(UriUsage usage = UriUsage::AsSpecified) const;

    // Operations
    void clear();
    void parse(string_cref_t source, UriUsage usage = UriUsage::AsSpecified);
    bool tryParse(string_cref_t source, UriUsage usage = UriUsage::AsSpecified);
    bool tryParse(string_cref_t source, string_ref_t error, UriUsage usage = UriUsage::AsSpecified);
    Uri escaped() const;
    Uri unescaped() const;

private:
    // Internal Functions
    String getComponent(const std::string_view &segment, UriUsage usage,
                        const std::vector<char> &allowedChars) const;

    // Internal Fields
    String _source;
    std::string_view _scheme;
    std::string_view _userInfo;
    std::string_view _host;
    std::string_view _path;
    std::string_view _queryParams;
    std::string_view _fragment;
    int _port;
    UriUsage _usage;
};
////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

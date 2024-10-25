//! @file Core/Test_Uri.cpp
//! @brief The definition of unit tests for the Uri and UriBuilder classes.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/GTest_Core.hpp"
#include "Ag/Core/Uri.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(UriBuilder, DefaultConstruct)
{
    UriBuilder specimen;

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(UriBuilder, SchemeOnly)
{
    UriBuilder specimen;

    specimen.setScheme("https");

    ASSERT_FALSE(specimen.getScheme().isEmpty());
    EXPECT_STRINGEQC(specimen.getScheme(), "https");
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "https:");
}

GTEST_TEST(UriBuilder, UserInfoOnly)
{
    UriBuilder specimen;
    specimen.setUserInfo("root");

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_FALSE(specimen.getUserInfo().isEmpty());
    EXPECT_STRINGEQC(specimen.getUserInfo(), "root");
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(UriBuilder, HostOnly)
{
    UriBuilder specimen;

    specimen.setHost("www.myhost.com");

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "//www.myhost.com");
}

GTEST_TEST(UriBuilder, PortOnly)
{
    UriBuilder specimen;
    specimen.setPort(55565);

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_TRUE(specimen.hasPort());
    EXPECT_EQ(specimen.getPort(), 55565);
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(UriBuilder, RootlessPathOnly)
{
    UriBuilder specimen;
    specimen.getPathElements().push_back("shop");
    specimen.getPathElements().push_back("products");
    specimen.getPathElements().push_back("en-GB");

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_FALSE(specimen.getPathElements().empty());
    EXPECT_EQ(specimen.getPathElements().size(), 3u);
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "shop/products/en-GB");
}

GTEST_TEST(UriBuilder, RootedPathOnly)
{
    UriBuilder specimen;
    specimen.setRootedPath(true);
    specimen.getPathElements().push_back("shop");
    specimen.getPathElements().push_back("products");
    specimen.getPathElements().push_back("en-GB");

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_TRUE(specimen.isRootedPath());
    EXPECT_FALSE(specimen.getPathElements().empty());
    EXPECT_EQ(specimen.getPathElements().size(), 3u);
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "/shop/products/en-GB");
}

GTEST_TEST(UriBuilder, QueryOnly)
{
    UriBuilder specimen;
    specimen.getQueryParameters().emplace_back("MyKey", "MyValue");
    specimen.getQueryParameters().emplace_back("NoValue", String::Empty);
    specimen.getQueryParameters().emplace_back(String::Empty, "NoKey");

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_FALSE(specimen.getQueryParameters().empty());
    EXPECT_EQ(specimen.getQueryParameters().size(), 3u);
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "?MyKey=MyValue&NoValue&NoKey");
}

GTEST_TEST(UriBuilder, FragmentOnly)
{
    UriBuilder specimen;
    specimen.setFragment("Chapter4");

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_FALSE(specimen.getFragment().isEmpty());
    EXPECT_STRINGEQC(specimen.getFragment(), "Chapter4");

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "#Chapter4");
}

GTEST_TEST(UriBuilder, ParseEmptyFails)
{
    UriBuilder specimen;

    EXPECT_FALSE(specimen.tryParse(String::Empty));

    // Verify state unchanged.
    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(UriBuilder, ParseSchemeOnly)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("https:", error));
    EXPECT_TRUE(error.isEmpty());

    ASSERT_FALSE(specimen.getScheme().isEmpty());
    EXPECT_STRINGEQC(specimen.getScheme(), "https");
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "https:");
}

GTEST_TEST(UriBuilder, FailToParseSchemeWithEscapes)
{
    UriBuilder specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse("http%20s:", error));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(UriBuilder, FailToParseUserInfoOnly)
{
    UriBuilder specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse("//samba-users@", error));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(UriBuilder, ParseHostOnly)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("//www.myhost.com", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "//www.myhost.com");

    EXPECT_TRUE(specimen.tryParse("//www.my%20host.com", error));
    EXPECT_TRUE(error.isEmpty());
}

GTEST_TEST(UriBuilder, FailToParsePortOnly)
{
    UriBuilder specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse("//:55565", error));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(UriBuilder, ParseHostWithUserInfo)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("//all-users@www.myhost.com", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_FALSE(specimen.getUserInfo().isEmpty());
    EXPECT_STRINGEQC(specimen.getUserInfo(), "all-users");
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "//all-users@www.myhost.com");

    EXPECT_TRUE(specimen.tryParse("//root@www.my%20host.com", error));
    EXPECT_TRUE(error.isEmpty());
}

GTEST_TEST(UriBuilder, ParseHostWithPort)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("//www.myhost.com:8080", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_TRUE(specimen.hasPort());
    EXPECT_EQ(specimen.getPort(), 8080u);
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "//www.myhost.com:8080");

    EXPECT_TRUE(specimen.tryParse("//www.my%20host.com:69", error));
    EXPECT_TRUE(error.isEmpty());
}

GTEST_TEST(UriBuilder, ParseHostWithScheme)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("ftp://www.myhost.com", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_FALSE(specimen.getScheme().isEmpty());
    EXPECT_STRINGEQC(specimen.getScheme(), "ftp");
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "ftp://www.myhost.com");

    EXPECT_TRUE(specimen.tryParse("samba://www.my%20host.com", error));
    EXPECT_TRUE(error.isEmpty());
}

GTEST_TEST(UriBuilder, ParseRootlessPathOnly)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("shop//products///en-GB/", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_FALSE(specimen.getPathElements().empty());
    EXPECT_EQ(specimen.getPathElements().size(), 3u);
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "shop/products/en-GB");
}

GTEST_TEST(UriBuilder, ParseRootedPathOnly)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("/shop/products///en-GB", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_TRUE(specimen.isRootedPath());
    EXPECT_FALSE(specimen.getPathElements().empty());
    EXPECT_EQ(specimen.getPathElements().size(), 3u);
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "/shop/products/en-GB");
}

GTEST_TEST(UriBuilder, ParseQueryOnly)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("?MyKey=MyValue&NoValue=&=NoKey", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_FALSE(specimen.getQueryParameters().empty());
    EXPECT_EQ(specimen.getQueryParameters().size(), 3u);
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "?MyKey=MyValue&NoValue&NoKey");
}

GTEST_TEST(UriBuilder, ParseFragmentOnly)
{
    UriBuilder specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("#Chapter4", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().empty());
    EXPECT_TRUE(specimen.getQueryParameters().empty());
    EXPECT_FALSE(specimen.getFragment().isEmpty());
    EXPECT_STRINGEQC(specimen.getFragment(), "Chapter4");

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "#Chapter4");
}

GTEST_TEST(UriBuilder, Escape)
{
    UriBuilder specimen;

    specimen.setScheme("ht%74p");
    specimen.setUserInfo("loadsa%C2%A3"); // Pounds sterling: \u00A3
    specimen.setHost("my%20face"); // Smily face 0x263A;
    specimen.setPort(11);
    specimen.setRootedPath(true);
    specimen.getPathElements().emplace_back("event-more-%C2%A3");
    specimen.getQueryParameters().emplace_back("100%3D",// \u003D is '='
                                               "at%20the%20%7C"); // \u007C is '|'
    specimen.setFragment("use%20of%20a%20%3B"); // \u003B is ';'

    EXPECT_STRINGEQC(specimen.toString(UriUsage::Display),
                     "ht%74p://loadsa\xC2\xA3@my face:11/event-more-\xC2\xA3?100==at the |#use of a ;");

    EXPECT_STRINGEQC(specimen.toString(UriUsage::Escaped),
                     "ht%74p://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100%3D=at%20the%20%7C#use%20of%20a%20%3B");

    EXPECT_STRINGEQC(specimen.toString(UriUsage::AsSpecified),
                     "ht%74p://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100%3D=at%20the%20%7C#use%20of%20a%20%3B");

    specimen.unescape();

    EXPECT_STRINGEQC(specimen.toString(UriUsage::AsSpecified),
                     "ht%74p://loadsa\xC2\xA3@my face:11/event-more-\xC2\xA3?100==at the |#use of a ;");

    EXPECT_STRINGEQC(specimen.toString(UriUsage::Display),
                     "ht%74p://loadsa\xC2\xA3@my face:11/event-more-\xC2\xA3?100==at the |#use of a ;");

    EXPECT_STRINGEQC(specimen.toString(UriUsage::Escaped),
                     "ht%74p://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100==at%20the%20%7C#use%20of%20a%20;");

    specimen.escape();

    EXPECT_STRINGEQC(specimen.toString(UriUsage::AsSpecified),
                     "ht%74p://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100==at%20the%20%7C#use%20of%20a%20;");

    EXPECT_STRINGEQC(specimen.toString(UriUsage::Display),
                     "ht%74p://loadsa\xC2\xA3@my face:11/event-more-\xC2\xA3?100==at the |#use of a ;");

    EXPECT_STRINGEQC(specimen.toString(UriUsage::Escaped),
                     "ht%74p://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100==at%20the%20%7C#use%20of%20a%20;");
}

















GTEST_TEST(Uri, DefaultConstruct)
{
    Uri specimen;

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(Uri, ParseEmptyFails)
{
    Uri specimen;

    EXPECT_FALSE(specimen.tryParse(String::Empty));

    // Verify state unchanged.
    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(Uri, ParseSchemeOnly)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("https:", error));
    EXPECT_TRUE(error.isEmpty());

    ASSERT_FALSE(specimen.getScheme().isEmpty());
    EXPECT_STRINGEQC(specimen.getScheme(), "https");
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "https:");
}

GTEST_TEST(Uri, FailToParseSchemeWithEscapes)
{
    Uri specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse("http%20s:", error));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(Uri, FailToParseUserInfoOnly)
{
    Uri specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse("//samba-users@", error));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(Uri, ParseHostOnly)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("//www.myhost.com", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "//www.myhost.com");

    EXPECT_TRUE(specimen.tryParse("//www.my%20host.com", error));
    EXPECT_TRUE(error.isEmpty());
}

GTEST_TEST(Uri, FailToParsePortOnly)
{
    Uri specimen;
    String error;

    EXPECT_FALSE(specimen.tryParse("//:55565", error));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());
    EXPECT_TRUE(specimen.toString().isEmpty());
}

GTEST_TEST(Uri, ParseHostWithUserInfo)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("//all-users@www.myhost.com", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_FALSE(specimen.getUserInfo().isEmpty());
    EXPECT_STRINGEQC(specimen.getUserInfo(), "all-users");
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "//all-users@www.myhost.com");

    EXPECT_TRUE(specimen.tryParse("//root@www.my%20host.com", error));
    EXPECT_TRUE(error.isEmpty());
}

GTEST_TEST(Uri, ParseHostWithPort)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("//www.myhost.com:8080", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_TRUE(specimen.hasPort());
    EXPECT_EQ(specimen.getPort(), 8080u);
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "//www.myhost.com:8080");

    EXPECT_TRUE(specimen.tryParse("//www.my%20host.com:69", error));
    EXPECT_TRUE(error.isEmpty());
}

GTEST_TEST(Uri, ParseHostWithScheme)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("ftp://www.myhost.com", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_FALSE(specimen.getScheme().isEmpty());
    EXPECT_STRINGEQC(specimen.getScheme(), "ftp");
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_FALSE(specimen.getHost().isEmpty());
    EXPECT_STRINGEQC(specimen.getHost(), "www.myhost.com");
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    ASSERT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "ftp://www.myhost.com");

    EXPECT_TRUE(specimen.tryParse("samba://www.my%20host.com", error));
    EXPECT_TRUE(error.isEmpty());
}

GTEST_TEST(Uri, ParseRootlessPathOnly)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("shop//products///en-GB/", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_FALSE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "shop/products/en-GB");
}

GTEST_TEST(Uri, ParseRootedPathOnly)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("/shop/products///en-GB", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_TRUE(specimen.isRootedPath());
    EXPECT_FALSE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "/shop/products/en-GB");
}

GTEST_TEST(Uri, ParseQueryOnly)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("?MyKey=MyValue&NoValue=&=NoKey", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_FALSE(specimen.getQueryParameters().isEmpty());
    EXPECT_TRUE(specimen.getFragment().isEmpty());

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "?MyKey=MyValue&NoValue=&=NoKey");
}

GTEST_TEST(Uri, ParseFragmentOnly)
{
    Uri specimen;
    String error;

    EXPECT_TRUE(specimen.tryParse("#Chapter4", error));
    EXPECT_TRUE(error.isEmpty());

    EXPECT_TRUE(specimen.getScheme().isEmpty());
    EXPECT_TRUE(specimen.getUserInfo().isEmpty());
    EXPECT_TRUE(specimen.getHost().isEmpty());
    EXPECT_FALSE(specimen.hasPort());
    EXPECT_FALSE(specimen.isRootedPath());
    EXPECT_TRUE(specimen.getPathElements().isEmpty());
    EXPECT_TRUE(specimen.getQueryParameters().isEmpty());
    EXPECT_FALSE(specimen.getFragment().isEmpty());
    EXPECT_STRINGEQC(specimen.getFragment(), "Chapter4");

    EXPECT_FALSE(specimen.toString().isEmpty());
    EXPECT_STRINGEQC(specimen.toString(), "#Chapter4");
}

GTEST_TEST(Uri, Escape)
{
    Uri specimen;

    String error;
    EXPECT_TRUE(specimen.tryParse("http://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100%3D=at%20the%20%7C#use%20of%20a%20%3B", error));
    ASSERT_TRUE(error.isEmpty());

    EXPECT_STRINGEQC(specimen.toString(UriUsage::Display),
                     "http://loadsa\xC2\xA3@my face:11/event-more-\xC2\xA3?100==at the |#use of a ;");

    EXPECT_STRINGEQC(specimen.toString(UriUsage::Escaped),
                     "http://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100%3D=at%20the%20%7C#use%20of%20a%20%3B");

    EXPECT_STRINGEQC(specimen.toString(UriUsage::AsSpecified),
                     "http://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100%3D=at%20the%20%7C#use%20of%20a%20%3B");

    Uri unescaped = specimen.unescaped();

    EXPECT_STRINGEQC(unescaped.toString(UriUsage::AsSpecified),
                     "http://loadsa\xC2\xA3@my face:11/event-more-\xC2\xA3?100==at the |#use of a ;");

    EXPECT_STRINGEQC(unescaped.toString(UriUsage::Display),
                     "http://loadsa\xC2\xA3@my face:11/event-more-\xC2\xA3?100==at the |#use of a ;");

    EXPECT_STRINGEQC(unescaped.toString(UriUsage::Escaped),
                     "http://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100==at%20the%20%7C#use%20of%20a%20;");

    Uri escaped = unescaped.escaped();

    EXPECT_STRINGEQC(escaped.toString(UriUsage::AsSpecified),
                     "http://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100==at%20the%20%7C#use%20of%20a%20;");

    EXPECT_STRINGEQC(escaped.toString(UriUsage::Display),
                     "http://loadsa\xC2\xA3@my face:11/event-more-\xC2\xA3?100==at the |#use of a ;");

    EXPECT_STRINGEQC(escaped.toString(UriUsage::Escaped),
                     "http://loadsa%C2%A3@my%20face:11/event-more-%C2%A3?100==at%20the%20%7C#use%20of%20a%20;");
}

} // Anonymous namespace

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////


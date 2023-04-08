//! @file Test_FileSystem.cpp
//! @brief The definition of unit tests for file system related classes.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include <Ag/GTest_Core.hpp>

#include "Ag/Core/FsDirectory.hpp"
#include "Ag/Core/FsPath.hpp"
#include "Ag/Core/FsSearchPathList.hpp"
#include "Platform.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Fs {

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
GTEST_TEST(FsPathBuilder, DefaultConstruct)
{
    PathBuilder specimen;

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_TRUE(specimen.getRoot().isEmpty());
    EXPECT_EQ(specimen.getElementCount(), 0u);
    EXPECT_EQ(specimen.getSchema(), getNativeSchema());
}

GTEST_TEST(FsPathBuilder, TryParseDriveLetterPath)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("C:\\My Documents/Read Me.txt"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "C:\\");
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "My Documents");
    EXPECT_STRINGEQC(specimen.getElement(1), "Read Me.txt");
}

GTEST_TEST(FsPathBuilder, TryParseWin32NSDriveLetterPath)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\\\?/C:///WinNT/System32\\OpenGL32.dll"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "C:\\");
    ASSERT_EQ(specimen.getElementCount(), 3u);
    EXPECT_STRINGEQC(specimen.getElement(0), "WinNT");
    EXPECT_STRINGEQC(specimen.getElement(1), "System32");
    EXPECT_STRINGEQC(specimen.getElement(2), "OpenGL32.dll");
}

GTEST_TEST(FsPathBuilder, TryParseUNCPath)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("//FileStore.lan\\Movies/StarTrek\\..\\StarWars\\.\\"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\FileStore.lan\\Movies\\");
    ASSERT_EQ(specimen.getElementCount(), 4u);
    EXPECT_STRINGEQC(specimen.getElement(0), "StarTrek");
    EXPECT_STRINGEQC(specimen.getElement(1), "..");
    EXPECT_STRINGEQC(specimen.getElement(2), "StarWars");
    EXPECT_STRINGEQC(specimen.getElement(3), ".");
}

GTEST_TEST(FsPathBuilder, TryParseWin32NSUNCPath)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\/?\\UNC/\\NasStorge.lan//\\Music\\Yes\\/Owner of a Lonely Heart.mp3"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\NasStorge.lan\\Music\\");
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Yes");
    EXPECT_STRINGEQC(specimen.getElement(1), "Owner of a Lonely Heart.mp3");
}

GTEST_TEST(FsPathBuilder, TryParseCurrentDrivePath)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\Doc\\User\\Report.pdf"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    ASSERT_EQ(specimen.getElementCount(), 3u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Doc");
    EXPECT_STRINGEQC(specimen.getElement(1), "User");
    EXPECT_STRINGEQC(specimen.getElement(2), "Report.pdf");
}

GTEST_TEST(FsPathBuilder, TryParseWin32NSCurrentDrivePath)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\\\?\\/Local\\Files\\."));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    ASSERT_EQ(specimen.getElementCount(), 3u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Local");
    EXPECT_STRINGEQC(specimen.getElement(1), "Files");
    EXPECT_STRINGEQC(specimen.getElement(2), ".");
}

GTEST_TEST(FsPathBuilder, TryParsePosixSysRoot)
{
    PathBuilder specimen(getPosixSchema());

    ASSERT_TRUE(specimen.tryParse("/usr/local/share/myStuff"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::SysRoot);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "/");
    ASSERT_EQ(specimen.getElementCount(), 4u);
    EXPECT_STRINGEQC(specimen.getElement(0), "usr");
    EXPECT_STRINGEQC(specimen.getElement(1), "local");
    EXPECT_STRINGEQC(specimen.getElement(2), "share");
    EXPECT_STRINGEQC(specimen.getElement(3), "myStuff");

    // Try parsing with multiple slashes.
    ASSERT_TRUE(specimen.tryParse("////var/log/daemons"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::SysRoot);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "/");
    ASSERT_EQ(specimen.getElementCount(), 3u);
    EXPECT_STRINGEQC(specimen.getElement(0), "var");
    EXPECT_STRINGEQC(specimen.getElement(1), "log");
    EXPECT_STRINGEQC(specimen.getElement(2), "daemons");
}

GTEST_TEST(FsPathBuilder, TryParsePosixUserRoot)
{
    PathBuilder specimen(getPosixSchema());

    ASSERT_TRUE(specimen.tryParse("~/Documents/jobs/CV.tex"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UserHome);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "~/");
    ASSERT_EQ(specimen.getElementCount(), 3u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Documents");
    EXPECT_STRINGEQC(specimen.getElement(1), "jobs");
    EXPECT_STRINGEQC(specimen.getElement(2), "CV.tex");

    // Try parsing with multiple slashes.
    ASSERT_TRUE(specimen.tryParse("~////Video/StarTrek///Clips"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UserHome);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "~/");
    ASSERT_EQ(specimen.getElementCount(), 3u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Video");
    EXPECT_STRINGEQC(specimen.getElement(1), "StarTrek");
    EXPECT_STRINGEQC(specimen.getElement(2), "Clips");
}

GTEST_TEST(FsPathBuilder, TryParseDriveLetterOnly)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("Q:"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "Q:\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try with trailing separators.
    ASSERT_TRUE(specimen.tryParse("x:\\\\"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "x:\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);
}

GTEST_TEST(FsPathBuilder, TryParseWin32NSDriveLetterOnly)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\\\?\\a:"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "a:\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try with trailing separators.
    ASSERT_TRUE(specimen.tryParse("//?\\K://"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "K:\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);
}

GTEST_TEST(FsPathBuilder, TryParseCurrentDriveOnly)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try with trailing separators.
    ASSERT_TRUE(specimen.tryParse("//\\"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);
}

GTEST_TEST(FsPathBuilder, TryParseWin32NSCurrentDriveOnly)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\\\?\\\\"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try with trailing separators.
    ASSERT_TRUE(specimen.tryParse("//?//\\"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);
}

GTEST_TEST(FsPathBuilder, TryParseUNCNameOnly)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\/Mail/Attachments"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\Mail\\Attachments\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try with trailing separators.
    ASSERT_TRUE(specimen.tryParse("\\\\News\\/Downloads/\\"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\News\\Downloads\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);
}

GTEST_TEST(FsPathBuilder, TryParseWin32NSUNCNameOnly)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\\\?\\UNC/Vpn\\Updates"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\Vpn\\Updates\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try with trailing separators.
    ASSERT_TRUE(specimen.tryParse("\\\\?\\UNC\\CrmHub/Tickets\\\\"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\CrmHub\\Tickets\\");
    EXPECT_EQ(specimen.getElementCount(), 0u);
}

GTEST_TEST(FsPathBuilder, TryParsePosixSysRootOnly)
{
    PathBuilder specimen(getPosixSchema());

    ASSERT_TRUE(specimen.tryParse("/"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::SysRoot);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "/");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try parsing with multiple slashes.
    ASSERT_TRUE(specimen.tryParse("////"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::SysRoot);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "/");
    EXPECT_EQ(specimen.getElementCount(), 0u);
}

GTEST_TEST(FsPathBuilder, TryParsePosixUserRootOnly)
{
    PathBuilder specimen(getPosixSchema());

    ASSERT_TRUE(specimen.tryParse("~/"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UserHome);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "~/");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try parsing with multiple slashes.
    ASSERT_TRUE(specimen.tryParse("~////"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UserHome);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "~/");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try parsing with no slashes.
    ASSERT_TRUE(specimen.tryParse("~"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UserHome);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "~/");
    EXPECT_EQ(specimen.getElementCount(), 0u);

    // Try parsing with no slashes not intended to be a root.
    ASSERT_TRUE(specimen.tryParse("~backup"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    ASSERT_EQ(specimen.getElementCount(), 1u);
    EXPECT_STRINGEQC(specimen.getElement(0), "~backup");
}

GTEST_TEST(FsPathBuilder, TryParseNoRootWin32)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("."));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    ASSERT_EQ(specimen.getElementCount(), 1u);
    EXPECT_STRINGEQC(specimen.getElement(0), ".");

    ASSERT_TRUE(specimen.tryParse("Documents/.."));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Documents");
    EXPECT_STRINGEQC(specimen.getElement(1), "..");
}

GTEST_TEST(FsPathBuilder, TryParseNoRootPosix)
{
    PathBuilder specimen(getPosixSchema());

    ASSERT_TRUE(specimen.tryParse("."));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    ASSERT_EQ(specimen.getElementCount(), 1u);
    EXPECT_STRINGEQC(specimen.getElement(0), ".");

    ASSERT_TRUE(specimen.tryParse("Documents/.."));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Documents");
    EXPECT_STRINGEQC(specimen.getElement(1), "..");
}

GTEST_TEST(FsPathBuilder, TryParseWin32FileNoRoot)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("\\\\?\\Build//x64/lib/libAg.a"));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    ASSERT_EQ(specimen.getElementCount(), 4u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Build");
    EXPECT_STRINGEQC(specimen.getElement(1), "x64");
    EXPECT_STRINGEQC(specimen.getElement(2), "lib");
    EXPECT_STRINGEQC(specimen.getElement(3), "libAg.a");

    ASSERT_TRUE(specimen.tryParse("//?/Documents/.."));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Documents");
    EXPECT_STRINGEQC(specimen.getElement(1), "..");
}

GTEST_TEST(FsPathBuilder, TryParseInvalidFailsWin32)
{
    PathBuilder specimen(getWin32Schema());

    String error;
    EXPECT_FALSE(specimen.tryParse(":\\Here", error));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_FALSE(specimen.tryParse("R:\\Over There ->\\Here I Am", error));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_FALSE(specimen.tryParse("$:\\DriveFail", error));
    EXPECT_FALSE(error.isEmpty());
}

GTEST_TEST(FsPathBuilder, ToDisplayStringWin32)
{
    PathBuilder specimen(getWin32Schema());

    ASSERT_TRUE(specimen.tryParse("C://My Documents\\\\Errors.txt/"));
    String result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "C:\\My Documents\\Errors.txt");

    ASSERT_TRUE(specimen.tryParse("//Main\\Files\\AllUsers/Documents\\..\\/Rules.txt"));
    result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "\\\\Main\\Files\\AllUsers\\Documents\\..\\Rules.txt");

    ASSERT_TRUE(specimen.tryParse("\\\\?\\Build//x64/lib/libAg.a"));
    result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "Build\\x64\\lib\\libAg.a");
}

GTEST_TEST(FsPathBuilder, ToStringPosix)
{
    PathBuilder specimen(getPosixSchema());

    ASSERT_TRUE(specimen.tryParse("//My Documents////Errors.txt/"));
    String result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "/My Documents/Errors.txt");

    ASSERT_TRUE(specimen.tryParse("~/Documents///..//Rules.txt"));
    result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "~/Documents/../Rules.txt");

    result = specimen.toString(PathUsage::Shell);
    EXPECT_STRINGEQC(result, "~/Documents/../Rules.txt");

    // Expect ~/ to be replaced with something longer, like '/home/myuser/'
    String tail("/Documents/../Rules.txt");
    result = specimen.toString(PathUsage::Kernel);
    EXPECT_FALSE(result.isEmpty());
    ASSERT_GT(result.getUtf32Length(), tail.getUtf32Length());
    size_t extra = result.getUtf32Length() - tail.getUtf32Length();
    EXPECT_GT(extra, 1u);
}

GTEST_TEST(FsPathBuilder, LongPathToString)
{
    PathBuilder specimen(getWin32Schema());

    // Ensure a path > 260 characters receives the Win32 File namespace prefix.
    std::string longPathSrc;
    longPathSrc.assign("C:");

    while (longPathSrc.length() <= _MAX_PATH)
    {
        longPathSrc.append("\\MakePathEvenLonger");
    }

    String longPath(longPathSrc);

    ASSERT_TRUE(specimen.tryParse(longPath));

    String result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQ(result, longPath);

    std::string longPathPrefixSrc;
    longPathPrefixSrc.assign("\\\\?\\");
    longPathPrefixSrc.append(longPathSrc);
    String longPathPrefix(longPathPrefixSrc);

    result = specimen.toString(PathUsage::Kernel);
    EXPECT_STRINGEQ(result, longPathPrefix);

    EXPECT_THROW({ result = specimen.toString(PathUsage::Shell); }, PathTooLongException);
}

GTEST_TEST(FsPathBuilder, AssignProgramFilePath)
{
    PathBuilder specimen(getNativeSchema());

    specimen.assignProgramFile();

    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
}

GTEST_TEST(FsPathBuilder, AssignProgramDir)
{
    PathBuilder specimen(getNativeSchema());

    specimen.assignProgramDirectory();

    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
}

GTEST_TEST(FsPathBuilder, AssignHomeDir)
{
    PathBuilder specimen(getNativeSchema());

    specimen.assignHomeDirectory();

    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
}

GTEST_TEST(FsPathBuilder, AssignWorkingDir)
{
    PathBuilder specimen(getNativeSchema());

    specimen.assignWorkingDirectory();

    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
}

GTEST_TEST(FsPathBuilder, SetFileNameWin32)
{
    // Start with no path elements.
    PathBuilder specimen(getWin32Schema());

    EXPECT_FALSE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 0u);
    EXPECT_TRUE(specimen.getFileName().isEmpty());

    specimen.setFileName("MyStuff");
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 1u);
    EXPECT_STRINGEQC(specimen.getFileName(), "MyStuff");

    specimen.setFileName("Hello");
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 1u);
    EXPECT_STRINGEQC(specimen.getFileName(), "Hello");

    specimen.pushElement("Goodbye");
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Hello");
    EXPECT_STRINGEQC(specimen.getElement(1), "Goodbye");
    EXPECT_STRINGEQC(specimen.getFileName(), "Goodbye");

    specimen.setFileName("There");
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Hello");
    EXPECT_STRINGEQC(specimen.getElement(1), "There");
    EXPECT_STRINGEQC(specimen.getFileName(), "There");

    EXPECT_THROW({ specimen.setFileName(String::Empty); }, ArgumentException);
}

GTEST_TEST(FsPathBuilder, SetFileNamePosix)
{
    // Start with no path elements.
    PathBuilder specimen(getPosixSchema());

    EXPECT_FALSE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 0u);
    EXPECT_TRUE(specimen.getFileName().isEmpty());

    specimen.setFileName("MyStuff");
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 1u);
    EXPECT_STRINGEQC(specimen.getFileName(), "MyStuff");

    specimen.setFileName("Hello");
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 1u);
    EXPECT_STRINGEQC(specimen.getFileName(), "Hello");

    specimen.pushElement("Goodbye");
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Hello");
    EXPECT_STRINGEQC(specimen.getElement(1), "Goodbye");
    EXPECT_STRINGEQC(specimen.getFileName(), "Goodbye");

    specimen.setFileName("There");
    ASSERT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getElement(0), "Hello");
    EXPECT_STRINGEQC(specimen.getElement(1), "There");
    EXPECT_STRINGEQC(specimen.getFileName(), "There");

    EXPECT_THROW({ specimen.setFileName(String::Empty); }, ArgumentException);
}

GTEST_TEST(FsPathBuilder, GetDirectoryWin32)
{
    // Start with no path elements.
    PathBuilder specimen(getWin32Schema());

    EXPECT_FALSE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 0u);
    EXPECT_TRUE(specimen.getDirectory().isEmpty());

    specimen.pushElement("MyStuff");
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 1u);
    EXPECT_TRUE(specimen.getDirectory().isEmpty());

    specimen.pushElement("Hello");
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getDirectory(), "MyStuff");

    specimen.pushElement("Goodbye");
    ASSERT_EQ(specimen.getElementCount(), 3u);
    EXPECT_STRINGEQC(specimen.getDirectory(), "MyStuff\\Hello");

    specimen.setRoot("C:///\\");
    EXPECT_STRINGEQC(specimen.getDirectory(), "C:\\MyStuff\\Hello");
}

GTEST_TEST(FsPathBuilder, GetDirectoryPosix)
{
    // Start with no path elements.
    PathBuilder specimen(getPosixSchema());

    EXPECT_FALSE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 0u);
    EXPECT_TRUE(specimen.getDirectory().isEmpty());

    specimen.pushElement("MyStuff");
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 1u);
    EXPECT_TRUE(specimen.getDirectory().isEmpty());

    specimen.pushElement("Hello");
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getElementCount(), 2u);
    EXPECT_STRINGEQC(specimen.getDirectory(), "MyStuff");

    specimen.pushElement("Goodbye");
    ASSERT_EQ(specimen.getElementCount(), 3u);
    EXPECT_STRINGEQC(specimen.getDirectory(), "MyStuff/Hello");

    specimen.setRoot("~/");
    EXPECT_STRINGEQC(specimen.getDirectory(), "~/MyStuff/Hello");
}

GTEST_TEST(FsPathBuilder, SetFileExtensionWin32)
{
    // Start with no extension.
    PathBuilder specimen("/Document/Hello", getWin32Schema());

    EXPECT_TRUE(specimen.getFileExtension().isEmpty());

    // Try to add one without specifying a leading period.
    specimen.setFileExtension("doc");
    EXPECT_STRINGEQC(specimen.getFileExtension(), "doc");

    // Try specifying only a leading period.
    specimen.setFileExtension(".");
    EXPECT_TRUE(specimen.getFileExtension().isEmpty());

    // Try with one leading period.
    specimen.setFileExtension(".tex");
    EXPECT_STRINGEQC(specimen.getFileExtension(), "tex");

    // Try replacing the extension with multiple leading periods.
    specimen.setFileExtension("..txt");
    EXPECT_STRINGEQC(specimen.getFileExtension(), "txt");
}

GTEST_TEST(FsPathBuilder, SetFileExtensionPosix)
{
    // Start with no extension.
    PathBuilder specimen("/Document/Hello", getPosixSchema());

    EXPECT_TRUE(specimen.getFileExtension().isEmpty());

    // Try to add one without specifying a leading period.
    specimen.setFileExtension("doc");
    EXPECT_STRINGEQC(specimen.getFileExtension(), "doc");

    // Try specifying only a leading period.
    specimen.setFileExtension(".");
    EXPECT_TRUE(specimen.getFileExtension().isEmpty());

    // Try with one leading period.
    specimen.setFileExtension(".tex");
    EXPECT_STRINGEQC(specimen.getFileExtension(), "tex");

    // Try replacing the extension with multiple leading periods.
    specimen.setFileExtension("..txt");
    EXPECT_STRINGEQC(specimen.getFileExtension(), "txt");
}

GTEST_TEST(FsPath, DefaultConstruct)
{
    Path specimen;

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_TRUE(specimen.getRoot().isEmpty());
    EXPECT_TRUE(specimen.getFileName().isEmpty());
    EXPECT_TRUE(specimen.getFileBaseName().isEmpty());
    EXPECT_TRUE(specimen.getFileExtension().isEmpty());
    EXPECT_EQ(specimen.getSchema(), getNativeSchema());
}

GTEST_TEST(FsPath, TryParseGenericPosixPath)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("/home/MyUser/Documents/MyStuff.tar.gz",
                               specimen, getPosixSchema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::SysRoot);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "/");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "/home/MyUser/Documents/MyStuff.tar.gz");
    EXPECT_STRINGEQC(specimen.getFileName(), "MyStuff.tar.gz");
    EXPECT_STRINGEQC(specimen.getFileBaseName(), "MyStuff");
    EXPECT_STRINGEQC(specimen.getFileExtension(), "tar.gz");
    EXPECT_STRINGEQC(specimen.getLastExtension(), "gz");
}

GTEST_TEST(FsPath, TryParseDriverLetterPath)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("C:\\My Documents/Read Me.txt.zip",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "C:\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "C:\\My Documents\\Read Me.txt.zip");
    EXPECT_STRINGEQC(specimen.getFileName(), "Read Me.txt.zip");
    EXPECT_STRINGEQC(specimen.getFileBaseName(), "Read Me");
    EXPECT_STRINGEQC(specimen.getFileExtension(), "txt.zip");
    EXPECT_STRINGEQC(specimen.getLastExtension(), "zip");
}

GTEST_TEST(FsPath, TryParseWin32NSDriverLetterPath)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\\\?/C:///WinNT/System32\\OpenGL32.dll",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "C:\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "C:\\WinNT\\System32\\OpenGL32.dll");
}

GTEST_TEST(FsPath, TryParseUNCPath)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("//FileStore.lan\\Movies/StarTrek\\..\\StarWars\\.\\",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\FileStore.lan\\Movies\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "\\\\FileStore.lan\\Movies\\StarTrek\\..\\StarWars\\.");
}

GTEST_TEST(FsPath, TryParseWin32NSUNCPath)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\/?\\UNC/\\NasStorge.lan//\\Music\\Yes\\/Owner of a Lonely Heart.mp3",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\NasStorge.lan\\Music\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "\\\\NasStorge.lan\\Music\\Yes\\Owner of a Lonely Heart.mp3");
}

GTEST_TEST(FsPath, TryParseCurrentDrivePath)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\Doc\\User\\Report.pdf",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "\\Doc\\User\\Report.pdf");
}

GTEST_TEST(FsPath, TryParseWin32NSCurrentDrivePath)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\\\?\\/Local\\Files\\.",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "\\Local\\Files\\.");
}

GTEST_TEST(FsPath, TryParsePosixSysRoot)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("///Local/Files///.",
                               specimen, getPosixSchema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::SysRoot);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "/");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "/Local/Files/.");
}

GTEST_TEST(FsPath, TryParsePosixUserRoot)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("~///Local/Files///.",
                               specimen, getPosixSchema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UserHome);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "~/");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "~/Local/Files/.");
}

GTEST_TEST(FsPath, TryParseDriveLetterOnly)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("Q:", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "Q:\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "Q:\\");

    // Try with trailing separators.
    ASSERT_TRUE(Path::tryParse("x:\\\\", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "x:\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "x:\\");
}

GTEST_TEST(FsPath, TryParseWin32NSDriveLetterOnly)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\\\?\\a:", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "a:\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "a:\\");

    // Try with trailing separators.
    ASSERT_TRUE(Path::tryParse("//?\\K://", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::DosDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "K:\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "K:\\");
}

GTEST_TEST(FsPath, TryParseCurrentDriveOnly)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "\\");

    // Try with trailing separators.
    ASSERT_TRUE(Path::tryParse("//\\", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "\\");
}

GTEST_TEST(FsPath, TryParseWin32NSCurrentDriveOnly)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\\\?\\\\", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "\\");

    // Try with trailing separators.
    ASSERT_TRUE(Path::tryParse("//?//\\", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::CurrentDrive);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "\\");
}

GTEST_TEST(FsPath, TryParseUNCNameOnly)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\/Mail/Attachments",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\Mail\\Attachments\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "\\\\Mail\\Attachments\\");

    // Try with trailing separators.
    ASSERT_TRUE(Path::tryParse("\\\\News\\/Downloads/\\",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\News\\Downloads\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "\\\\News\\Downloads\\");
}

GTEST_TEST(FsPath, TryParseWin32NSUNCNameOnly)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\\\?\\UNC/Vpn\\Updates",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\Vpn\\Updates\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "\\\\Vpn\\Updates\\");

    // Try with trailing separators.
    ASSERT_TRUE(Path::tryParse("\\\\?\\UNC\\CrmHub/Tickets\\\\",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UncName);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "\\\\CrmHub\\Tickets\\");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "\\\\CrmHub\\Tickets\\");
}

GTEST_TEST(FsPath, TryParsePosixSysRootOnly)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("///",
                               specimen, getPosixSchema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::SysRoot);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "/");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "/");
}

GTEST_TEST(FsPath, TryParsePosixUserRootOnly)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("~//",
                               specimen, getPosixSchema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::UserHome);
    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getRoot(), "~/");
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "~/");
}

GTEST_TEST(FsPath, TryParseNoRootWin32)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse(".", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), ".");

    ASSERT_TRUE(Path::tryParse("Documents/..", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "Documents\\..");
}

GTEST_TEST(FsPath, TryParseNoRootPosix)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse(".", specimen, getPosixSchema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), ".");

    ASSERT_TRUE(Path::tryParse("Documents//..", specimen, getPosixSchema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "Documents/..");
}

GTEST_TEST(FsPath, TryParseWin32FileNoRoot)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("\\\\?\\Build//x64/lib/libAg.a",
                               specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "Build\\x64\\lib\\libAg.a");

    ASSERT_TRUE(Path::tryParse("//?/Documents/..", specimen, getWin32Schema()));

    EXPECT_EQ(specimen.getRootType(), PathRootType::None);
    EXPECT_FALSE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "Documents\\..");
}

GTEST_TEST(FsPath, TryParseInvalidFails)
{
    Path specimen;

    String error;
    EXPECT_FALSE(Path::tryParse(":\\Here", specimen,
                                error, getWin32Schema()));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_FALSE(Path::tryParse("R:\\Over There ->\\Here I Am", specimen,
                                error, getWin32Schema()));
    EXPECT_FALSE(error.isEmpty());

    EXPECT_FALSE(Path::tryParse("$:\\DriveFail", specimen,
                                error, getWin32Schema()));
    EXPECT_FALSE(error.isEmpty());
}

GTEST_TEST(FsPath, ToDisplayString)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("C://My Documents\\\\Errors.txt/",
                               specimen, getWin32Schema()));
    String result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "C:\\My Documents\\Errors.txt");

    ASSERT_TRUE(Path::tryParse("//Main\\Files\\AllUsers/Documents\\..\\/Rules.txt",
                               specimen, getWin32Schema()));
    result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "\\\\Main\\Files\\AllUsers\\Documents\\..\\Rules.txt");

    ASSERT_TRUE(Path::tryParse("\\\\?\\Build//x64/lib/libAg.a",
                               specimen, getWin32Schema()));
    result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "Build\\x64\\lib\\libAg.a");
}

GTEST_TEST(FsPath, ToStringPosix)
{
    Path specimen;

    ASSERT_TRUE(specimen.tryParse("//My Documents////Errors.txt/",
                                  specimen, getPosixSchema()));
    String result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "/My Documents/Errors.txt");

    ASSERT_TRUE(specimen.tryParse("~/Documents///..//Rules.txt",
                                  specimen, getPosixSchema()));
    result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQC(result, "~/Documents/../Rules.txt");

    result = specimen.toString(PathUsage::Shell);
    EXPECT_STRINGEQC(result, "~/Documents/../Rules.txt");

    // Expect ~/ to be replaced with something longer, like '/home/myuser/'
    String tail("/Documents/../Rules.txt");
    result = specimen.toString(PathUsage::Kernel);
    EXPECT_FALSE(result.isEmpty());
    ASSERT_GT(result.getUtf32Length(), tail.getUtf32Length());
    size_t extra = result.getUtf32Length() - tail.getUtf32Length();
    EXPECT_GT(extra, 1u);
}

GTEST_TEST(FsPath, LongPathToString)
{
    Path specimen;

    // Ensure a path > 260 characters receives the Win32 File namespace prefix.
    std::string longPathSrc;
    longPathSrc.assign("C:");

    while (longPathSrc.length() <= _MAX_PATH)
    {
        longPathSrc.append("\\MakePathEvenLonger");
    }

    String longPath(longPathSrc);

    ASSERT_TRUE(Path::tryParse(longPath, specimen, getWin32Schema()));

    String result = specimen.toString(PathUsage::Display);
    EXPECT_STRINGEQ(result, longPath);

    std::string longPathPrefixSrc;
    longPathPrefixSrc.assign("\\\\?\\");
    longPathPrefixSrc.append(longPathSrc);
    String longPathPrefix(longPathPrefixSrc);

    result = specimen.toString(PathUsage::Kernel);
    EXPECT_STRINGEQ(result, longPathPrefix);

    EXPECT_THROW({ result = specimen.toString(PathUsage::Shell); }, PathTooLongException);
}

GTEST_TEST(FsPath, AssignProgramFilePath)
{
    Path specimen = Path::getProgramFile();

    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getSchema(), getNativeSchema());
}

GTEST_TEST(FsPath, AssignProgramDir)
{
    Path specimen = Path::getProgramDirectory();

    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getSchema(), getNativeSchema());
}

GTEST_TEST(FsPath, AssignHomeDir)
{
    Path specimen = Path::getHomeDirectory();

    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getSchema(), getNativeSchema());
}

GTEST_TEST(FsPath, AssignWorkingDir)
{
    Path specimen = Path::getWorkingDirectory();

    EXPECT_TRUE(specimen.hasRoot());
    EXPECT_TRUE(specimen.hasElements());
    EXPECT_EQ(specimen.getSchema(), getNativeSchema());
}

GTEST_TEST(FsPath, GetFileNameWin32)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("MyStuff\\Hello\\There",
                               specimen, getWin32Schema()));

    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getFileName(), "There");
}

GTEST_TEST(FsPath, GetFileNamePosix)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("MyStuff/Hello/There",
                               specimen, getPosixSchema()));

    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getFileName(), "There");
}

GTEST_TEST(FsPath, GetDirectoryWin32)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("W:\\MyStuff\\Hello\\There",
                               specimen, getWin32Schema()));

    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getDirectory(), "W:\\MyStuff\\Hello");

    // Try with just a root.
    ASSERT_TRUE(Path::tryParse("W:", specimen, getWin32Schema()));

    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getDirectory(), "W:\\");
}

GTEST_TEST(FsPath, GetDirectoryPosix)
{
    Path specimen;

    ASSERT_TRUE(Path::tryParse("/MyStuff/Hello/There",
                               specimen, getPosixSchema()));

    EXPECT_TRUE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getDirectory(), "/MyStuff/Hello");

    // Try with just a root.
    ASSERT_TRUE(Path::tryParse("~", specimen, getPosixSchema()));

    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.getDirectory(), "~/");
}

GTEST_TEST(FsPath, GetDirectoryPathWin32)
{
    Path sample;

    ASSERT_TRUE(Path::tryParse("W:\\MyStuff\\Hello\\There",
                               sample, getWin32Schema()));

    EXPECT_TRUE(sample.hasElements());
    EXPECT_STRINGEQC(sample.getDirectory(), "W:\\MyStuff\\Hello");

    Path specimen = sample.getDirectoryPath();
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "W:\\MyStuff\\Hello");
    EXPECT_STRINGEQC(specimen.getFileName(), "Hello");
    EXPECT_TRUE(specimen.getFileExtension().isEmpty());

    // Try with just a root.
    ASSERT_TRUE(Path::tryParse("W:", sample, getWin32Schema()));

    EXPECT_FALSE(sample.hasElements());
    EXPECT_STRINGEQC(sample.getDirectory(), "W:\\");

    specimen = sample.getDirectoryPath();
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.toString(), "W:\\");
}

GTEST_TEST(FsPath, GetDirectoryPathPosix)
{
    Path sample;

    ASSERT_TRUE(Path::tryParse("/MyStuff/Hello/There",
                               sample, getPosixSchema()));

    EXPECT_TRUE(sample.hasElements());
    EXPECT_STRINGEQC(sample.getDirectory(), "/MyStuff/Hello");

    Path specimen = sample.getDirectoryPath();
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "/MyStuff/Hello");
    EXPECT_STRINGEQC(specimen.getFileName(), "Hello");
    EXPECT_TRUE(specimen.getFileExtension().isEmpty());

    // Try with just a root.
    ASSERT_TRUE(Path::tryParse("~", sample, getPosixSchema()));

    EXPECT_FALSE(sample.hasElements());
    EXPECT_STRINGEQC(sample.getDirectory(), "~/");

    specimen = sample.getDirectoryPath();
    EXPECT_FALSE(specimen.hasElements());
    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display), "~/");
}

GTEST_TEST(FsPath, GetFileExtensionWin32)
{
    // Start with no extension.
    Path specimen("/Document/Hello", getWin32Schema());

    EXPECT_TRUE(specimen.getFileExtension().isEmpty());

    specimen = "A:\\Docs\\Manual.pdf";

    EXPECT_STRINGEQC(specimen.getFileExtension(), "pdf");
}

GTEST_TEST(FsPath, GetFileExtensionPosix)
{
    // Start with no extension.
    Path specimen("/Document/Hello", getPosixSchema());

    EXPECT_TRUE(specimen.getFileExtension().isEmpty());

    specimen = "~/Docs/Manual.pdf";

    EXPECT_STRINGEQC(specimen.getFileExtension(), "pdf");
}

GTEST_TEST(FsPath, AppendWin32)
{
    Path specimen("C:/Document/Hello", getWin32Schema());

    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "C:\\Document\\Hello");

    Path expanded(specimen, "World");

    EXPECT_STRINGEQC(expanded.toString(PathUsage::Display),
                     "C:\\Document\\Hello\\World");

    specimen = expanded.append("ReadMe.txt");

    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "C:\\Document\\Hello\\World\\ReadMe.txt");
}

GTEST_TEST(FsPath, AppendPosix)
{
    Path specimen("//Document/Hello", getPosixSchema());

    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "/Document/Hello");

    Path expanded(specimen, "World");

    EXPECT_STRINGEQC(expanded.toString(PathUsage::Display),
                     "/Document/Hello/World");

    specimen = expanded.append("ReadMe.txt");

    EXPECT_STRINGEQC(specimen.toString(PathUsage::Display),
                     "/Document/Hello/World/ReadMe.txt");
}

GTEST_TEST(FsEntry, ConstructFromFile)
{
    Path programFilePath = Path::getProgramFile();

    Entry specimen(programFilePath);

    EXPECT_TRUE(specimen.exists());
    EXPECT_TRUE(specimen.isFile());
    EXPECT_FALSE(specimen.isDirectory());
    EXPECT_GT(specimen.getSize(), 4096);
    EXPECT_STRINGEQ(programFilePath.toString(), specimen.getPath().toString());
}

GTEST_TEST(FsEntry, ConstructFromDirectory)
{
    Path programDirPath = Path::getProgramDirectory();

    Entry specimen(programDirPath);

    EXPECT_TRUE(specimen.exists());
    EXPECT_FALSE(specimen.isFile());
    EXPECT_TRUE(specimen.isDirectory());
    EXPECT_LE(specimen.getSize(), 0);
    EXPECT_STRINGEQ(programDirPath.toString(), specimen.getPath().toString());
}

GTEST_TEST(FsEntry, ConstructFromMissing)
{
    Path specimenPath("{ADA4DCC8-6C43-4B59-9F9D-1865067619E7}.dat");

    Entry specimen(specimenPath);

    EXPECT_FALSE(specimen.exists());
    EXPECT_FALSE(specimen.isFile());
    EXPECT_FALSE(specimen.isDirectory());
    EXPECT_LE(specimen.getSize(), 0);
    EXPECT_STRINGEQ(specimenPath.toString(), specimen.getPath().getFileName());
}

GTEST_TEST(FsDirectory, BaseState)
{
    Path specimenPath = Path::getProgramDirectory();
    String programFileName = Path::getProgramFile().getFileName();
    String programFilePath = Path::getProgramFile().toString();

    Directory specimen(specimenPath);

    EXPECT_TRUE(specimen.exists());
    EXPECT_STRINGEQ(specimen.getName(), specimenPath.getFileName());
    EXPECT_STRINGEQ(specimen.getPath().toString(), specimen.getPath().toString());

    StringCollection names = specimen.getNames();

    ASSERT_GT(names.size(), 0u);

    String dot(".");
    String dotdot("..");

    bool hasProgramFile = false;
    bool containsDot = false;
    bool containsDotDot = false;

    for (string_cref_t fileName : names)
    {
        if (fileName == programFileName)
        {
            hasProgramFile = true;
        }
        else if (fileName == dot)
        {
            containsDot = true;
        }
        else if (fileName == dotdot)
        {
            containsDotDot = true;
        }
    }

    EXPECT_TRUE(hasProgramFile);
    EXPECT_FALSE(containsDot);
    EXPECT_FALSE(containsDotDot);

    hasProgramFile = false;

    EntryVector entries = specimen.getEntries();

    for (const Entry &entry : entries)
    {
        EXPECT_TRUE(entry.exists());

        if (entry.getName() == programFileName)
        {
            hasProgramFile = true;
            EXPECT_TRUE(entry.isFile());
            EXPECT_FALSE(entry.isDirectory());
            EXPECT_GT(entry.getSize(), 4096);
            EXPECT_STRINGEQ(entry.getPath().toString(), programFilePath);
            break;
        }
    }

    EXPECT_TRUE(hasProgramFile);
}

GTEST_TEST(FsSearchPathList, DefaultConstruct)
{
    SearchPathList specimen;

    EXPECT_TRUE(specimen.isEmpty());
    EXPECT_EQ(specimen.begin(), specimen.end());
    EXPECT_EQ(specimen.getCount(), 0u);
    EXPECT_EQ(specimen.getUniqueCount(), 0u);
}

GTEST_TEST(FsSearchPathList, TryFindAbsolute)
{
    SearchPathList specimen;

    Path programPath = Path::getProgramFile();

    Entry match;
    ASSERT_TRUE(specimen.tryFind(programPath, match));
    EXPECT_TRUE(match.exists());
    EXPECT_TRUE(match.isFile());
}

GTEST_TEST(FsSearchPathList, TryFindRelative)
{
    SearchPathList specimen;

    specimen.append(Path::getProgramDirectory());

    Path programPath = Path::getProgramFile();
    Entry match;

    ASSERT_TRUE(specimen.tryFind(programPath.getFileName(), match));
    EXPECT_TRUE(match.exists());
    EXPECT_TRUE(match.isFile());
}

GTEST_TEST(FsSearchPathList, Prepend)
{
    Path progDir = Path::getProgramDirectory();
    Path homeDir = Path::getHomeDirectory();
    Path currentDir = Path::getWorkingDirectory();

    if (progDir == currentDir)
    {
        currentDir = currentDir.append("MyStuff");
    }

    SearchPathList specimen;

    specimen.prepend(progDir);
    specimen.prepend(homeDir);
    specimen.prepend(currentDir);
    specimen.prepend(homeDir);

    ASSERT_FALSE(specimen.isEmpty());

    std::vector<Path> searchPaths(specimen.begin(), specimen.end());

    ASSERT_EQ(searchPaths.size(), 3u);
    EXPECT_EQ(searchPaths[0], homeDir);
    EXPECT_EQ(searchPaths[1], currentDir);
    EXPECT_EQ(searchPaths[2], progDir);
}


GTEST_TEST(FsSearchPathList, Append)
{
    Path progDir = Path::getProgramDirectory();
    Path homeDir = Path::getHomeDirectory();
    Path currentDir = Path::getWorkingDirectory();

    if (progDir == currentDir)
    {
        currentDir = currentDir.append("MyStuff");
    }

    SearchPathList specimen;

    specimen.append(homeDir);
    specimen.append(currentDir);
    specimen.append(homeDir);
    specimen.append(progDir);

    ASSERT_FALSE(specimen.isEmpty());

    std::vector<Path> searchPaths(specimen.begin(), specimen.end());

    ASSERT_EQ(searchPaths.size(), 3u);
    EXPECT_EQ(searchPaths[0], homeDir);
    EXPECT_EQ(searchPaths[1], currentDir);
    EXPECT_EQ(searchPaths[2], progDir);
}


} // TED

}} // namespace Ag::Fs
////////////////////////////////////////////////////////////////////////////////


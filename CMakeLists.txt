# Root CMakeLists for the MightyOak emulator built on the Silver cross-platform runtime library.
cmake_minimum_required(VERSION 3.21)

set(MO_VERSION "0.0.1.0")

# Use to determine whether to use ASM for small sections of code or
# the C++, platform agnostic (slower) alternative.
set(USE_ASM 1 CACHE BOOL "Sets whether assembly language will be used to directly emulate some instructions.")
set(PROJ_LANGUAGES CXX)

if (${USE_ASM})
    if (DEFINED CMAKE_HOST_WIN32 AND "$ENV{PROCESSOR_ARCHITECTURE}" STREQUAL "AMD64")
        # HACK: We need to adapt this for different assembler types.
        list(APPEND PROJ_LANGUAGES ASM_MASM)
    else()
        # We're not ready for assembler in the current environment.
        set(USE_ASM 0 CACHE FORCE)
    endif()
endif()

project(MightyOak
        VERSION "${MO_VERSION}"
        DESCRIPTION "An Acorn Archimedes and RiscPC emulator supported by a custom cross platform runtime (Ag)."
        HOMEPAGE_URL "https://github.com/GiantRobotLemur/MightyOak"
        LANGUAGES ${PROJ_LANGUAGES})

# Configure required external packages.
#include(CheckLanguage)


find_package(Doxygen 1.8 OPTIONAL_COMPONENTS dot)
find_package(Qt6 QUIET COMPONENTS Core Widgets Concurrent)

if (${Qt6_FOUND})
    message(STATUS "Found Qt6 suite ${Qt6_VERSION}.")
    qt_standard_project_setup()
endif()

# Needs to be called from the top level CMake file.
include("Source/Ag/cmake/ConfigureBuild.cmake")
ag_configure_build()

set(ExtFolderName "3rdParty")

message(STATUS "Obtaining Google Test...")
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
set_target_properties(gtest PROPERTIES FOLDER "${ExtFolderName}")
set_target_properties(gtest_main PROPERTIES FOLDER "${ExtFolderName}")
set_target_properties(gmock PROPERTIES FOLDER "${ExtFolderName}")
set_target_properties(gmock_main PROPERTIES FOLDER "${ExtFolderName}")

add_subdirectory(Source)
add_subdirectory(Doc)

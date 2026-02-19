# Root CMakeLists for the MightyOak emulator built on the Silver cross-platform runtime library.
cmake_minimum_required(VERSION 3.21)

set(MO_VERSION "0.0.1.0")
set(MO_AUTHOR "GiantRobotLemur@na-se.co.uk")
string(TIMESTAMP MO_COPYRIGHT "Copyright (c) 2021-%Y ${MO_AUTHOR}. All rights reserved.")

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
        set(USE_ASM 0)
    endif()
endif()

project(MightyOak
        VERSION "${MO_VERSION}"
        DESCRIPTION "An Acorn Archimedes and RiscPC emulator supported by a custom cross platform runtime (Ag)."
        HOMEPAGE_URL "https://github.com/GiantRobotLemur/MightyOak"
        LANGUAGES ${PROJ_LANGUAGES})

find_package(Doxygen 1.8 OPTIONAL_COMPONENTS dot)
find_package(Qt6 QUIET COMPONENTS Core Widgets Concurrent)

if (${Qt6_FOUND})
    set(AG_CONFIG_QT ON CACHE "BOOL" "Use Qt" FORCE)
    message(STATUS "Qt6: Found (Version ${Qt6_VERSION})")
else()
    set(AG_CONFIG_QT OFF CACHE "BOOL" "Use Qt" FORCE)
    message(STATUS "Qt6: Not Found (Add Qt6 path to CMAKE_PREFIX_PATH)")
endif()

# Set options consumed by the Ag libraries before they are configured.
set(AG_STATIC_RUNTIME ON CACHE "BOOL" "Static link" FORCE)

set(AG_CONFIG_GFX OFF CACHE "BOOL" "Use Gfx" FORCE)
set(AG_CONFIG_OPEN_GL OFF CACHE "BOOL" "Use GL" FORCE)
set(AG_CONFIG_SDL3 ON CACHE "BOOL" "Use SDL3" FORCE)

set(AG_ENABLE_X64_V1 ON CACHE "BOOL" "Uses the capabilities of the x86-64 v1 architecture - SSE2" FORCE)
set(AG_ENABLE_X64_V2 ON CACHE "BOOL" "Uses the capabilities of the x86-64 v2 architecture - SSE4, POPCNT, CMPXCHG16B" FORCE)
set(AG_ENABLE_X64_V3 ON CACHE "BOOL" "Uses the capabilities of the x86-64 v3 architecture - AVX, AVX2, FMA, LZCNT" FORCE)
set(AG_ENABLE_X64_V4 OFF CACHE "BOOL" "Uses the capabilities of the x86-64 v4 architecture - AVX512F/BW/CD/DQ/VL" FORCE)

# Needs to be called from the top level CMake file.
include("Source/Ag/cmake/ConfigureBuild.cmake")

# Apply default build configuration compatible with Ag.
ag_configure_build()

# A third party header-only library for a synchronised queue.
message(STATUS "Obtaining Moody Camel Reader/Writer Queue...")
FetchContent_Declare(readerwriterqueue
                     GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue.git
                     GIT_TAG v1.0.6)
FetchContent_MakeAvailable(readerwriterqueue)

# Download and build Google Test.
ag_configure_gtest()

# Download and build GLM
# ag_configure_glm()

# Download and configure libSDL3
ag_configure_sdl3()

if (${Qt6_FOUND})
    # Perform Qt initialisation once other 3rd party libraries are configured.
    qt_standard_project_setup()
endif()

# Gather our source code and targets.
add_subdirectory(Source)

# Create documentation targets.
add_subdirectory(Doc)

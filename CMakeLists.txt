# Root CMakeLists for the MightyOak emulator built on the Silver cross-platform runtime library.
cmake_minimum_required(VERSION 3.21)
enable_testing()

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        ON)

# Use to determine whether to use ASM for small sections of code or
# the C++, platform agnostic (slower) alternative.
set(USE_ASM 1)
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
        VERSION 0.1.0.0
        DESCRIPTION "An Acorn Archimedes and RiscPC emulator supported by a custom cross platform runtime (Ag)."
        HOMEPAGE_URL "https://github.com/GiantRobotLemur/MightyOak"
        LANGUAGES ${PROJ_LANGUAGES})

# Set the use of IDE folders before anything else.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configure required external packages.
include(CheckLanguage)
find_package(GTest 1.10 REQUIRED)
find_package(Doxygen 1.8 OPTIONAL_COMPONENTS dot)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED) # For GLU
find_package(glm REQUIRED)

find_package(Qt6 QUIET COMPONENTS Core Widgets Concurrent)

if (${SDL2_FOUND})
    message(STATUS "Found SDL library ${SDL2_VERSION}.")
endif()

if (${OPENGL_GLU_FOUND})
    message(STATUS "Found GLU library.")
endif()

if (${glm_FOUND})
    message(STATUS "Found GLM library ${glm_VERSION}.")
endif()

if (${Qt6_FOUND})
    message(STATUS "Found Qt6 suite ${Qt6_VERSION}.")
    qt_standard_project_setup()
endif()

# Limit configurations
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(isMultiConfig)
    # Limit build configurations to Release with Debug Info and Debug.
    set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo")
endif()

# Configure global compiler options.
if (DEFINED MSVC)
    add_compile_options("/W4")
else()
    add_compile_options(-Wall -Wextra -pedantic -Wshadow)
    
    # Ensure _DEBUG is defined for debug builds, this is done automatically
    # for MSVC.
    add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/cmake/Utilities.cmake")

# Add the location of public header files.
set(AG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Ag/Include")
set(MO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")
include_directories("${AG_INCLUDE_DIR}" "${MO_INCLUDE_DIR}")

# A third party header-only library for a synchronised queue.
add_subdirectory("Ag/Include/ReaderWriterQueue")

# Underlying runtime components.
add_subdirectory(Ag/SymbolPackager)
add_subdirectory(Ag/Core)
add_subdirectory(Ag/SdlTools)
add_subdirectory(Ag/GLAPI)
add_subdirectory(Ag/ObjectGL)
#add_subdirectory(Ag/Geometry)
#add_subdirectory(Ag/Gfx)

if ("${Qt6_FOUND}")
    add_subdirectory(Ag/QtInterop)
endif()

# ARM Emulator Modules
add_subdirectory(AsmTools)
add_subdirectory(ArmEmu)

if ("${Qt6_FOUND}")
    message("-- [Optional] Configuring ARM Debugger project")
    add_subdirectory(ArmDebugger)
endif()

#add_subdirectory(EmuPlatform)
add_subdirectory(EmuUI)
add_subdirectory(EmulatorApp)

# Generate Documentation from code.
if (DEFINED DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)

    # HTML Help CHM file generation options.
    list(APPEND DOXYGEN_PREDEFINED DO_NOT_DOCUMENT "STRUCT_ALIGN_8="
                                   "STRUCT_ALIGN_16=" "STRUCT_ALIGN_32=")
    list(APPEND DOXYGEN_EXCLUDE_PATTERNS "*/Template.cpp"
                                         "*/Template.hpp"
                                         "*/Test_Template.cpp")

    set(DOXYGEN_WARN_IF_DOC_ERROR YES)
    set(DOXYGEN_INHERIT_DOCS YES)
    set(DOXYGEN_HIDE_UNDOC_CLASSES YES)
    set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
    set(DOXYGEN_SORT_BRIEF_DOCS YES)
    set(DOXYGEN_SORT_MEMBER_DOCS YES)
    set(DOXYGEN_SORT_MEMBERS_CTORS_1ST YES)
    set(DOXYGEN_GENERATE_TODOLIST YES)
    set(DOXYGEN_ENABLE_PREPROCESSING YES)
    set(DOXYGEN_EXPAND_ONLY_PREDEF YES)

    # Find HTML Help Compiler to produce .CHM files under Windows.
    include(FindHTMLHelp)

    if(DEFINED HTML_HELP_COMPILER)
       set(DOXYGEN_GENERATE_HTMLHELP YES)
       set(DOXYGEN_SEARCHENGINE NO)
       set(DOXYGEN_CHM_FILE "MightyOak.chm")
       set(DOXYGEN_HHC_LOCATION "${HTML_HELP_COMPILER}")
    else()
        # If HTML Help not available, consider:
        set(DOXYGEN_GENERATE_TREEVIEW YES)
        set(DOXYGEN_SEARCHENGINE YES)
    endif()

    list(APPEND DOC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Ag/Core" 
                         "${AG_INCLUDE_DIR}/Ag/Core.hpp"
                         "${AG_INCLUDE_DIR}/Ag/Core")
    list(APPEND DOC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Ag/SdlTools" 
                         "${AG_INCLUDE_DIR}/Ag/SdlTools.hpp"
                         "${AG_INCLUDE_DIR}/Ag/SdlTools")
    list(APPEND DOC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/AsmTools"
                         "${MO_INCLUDE_DIR}/AsmTools.hpp"
                         "${MO_INCLUDE_DIR}/AsmTools")
    list(APPEND DOC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/ArmEmu"
                         "${MO_INCLUDE_DIR}/ArmEmu.hpp"
                         "${MO_INCLUDE_DIR}/ArmEmu")

if (DEFINED WIN32)
    list(APPEND DOC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Ag/Win32Base"
                         "${AG_INCLUDE_DIR}/Ag/Win32Base.hpp"
                         "${AG_INCLUDE_DIR}/Ag/Win32Base"
                         "${CMAKE_CURRENT_SOURCE_DIR}/Ag/Win32UI"
                         "${AG_INCLUDE_DIR}/Ag/Win32UI.hpp"
                         "${AG_INCLUDE_DIR}/Ag/Win32UI")
endif()


    doxygen_add_docs(Documentation ${DOC_SRCS}
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     COMMENT "Generating documentation...")
endif()

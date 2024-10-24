# CMakeLists file for the SymbolPackager tool.

# Fix up difference between CMAKE system ID and generator platform.
if ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
    set (TARGET_ARCH "AMD64")
else()
    set(TARGET_ARCH "${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

# message(STATUS "VS Platform Name: ${CMAKE_VS_PLATFORM_NAME}")
# message(STATUS "Generator Platform: ${CMAKE_GENERATOR_PLATFORM}")
# message(STATUS "Host Processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
# message(STATUS "Target Processor: ${CMAKE_SYSTEM_PROCESSOR}")
# message(STATUS "Target Architecture: ${TARGET_ARCH}")

# This might be important when building for ARM64 on x64.
set(AG_EXT_SYMBOL_PACKAGER "" CACHE FILEPATH "The path to an external Symbol Packager tool binary.")

if ("${TARGET_ARCH}" STREQUAL "${CMAKE_HOST_SYSTEM_PROCESSOR}")
    # Only build the symbol packager if it can be run on the host platform.
    add_executable(SymbolPackager
                   Main.cpp
                   Utils.hpp
                   Utils.cpp
                   LineTokeniser.cpp
                   LineTokeniser.hpp
                   CommandLine.hpp
                   CommandLine.cpp
                   SymbolDb.cpp
                   SymbolDb.hpp
                   MsMapFileReader.cpp
                   MsMapFileReader.hpp
                   GnuMapFileReader.cpp
                   GnuMapFileReader.hpp
                   GnuNmFileReader.cpp
                   GnuNmFileReader.hpp
                   PdbFileReader.cpp
                   PdbFileReader.hpp
                   SymbolFileReader.cpp
                   SymbolFileReader.hpp
                   ${AG_INCLUDE_DIR}/Ag/Private/SymbolEncoding.hpp)

    set_target_properties(SymbolPackager PROPERTIES FOLDER Core)
endif()

if (DEFINED AG_EXT_SYMBOL_PACKAGER)
    message(STATUS "Creating symbol files via external tool: ${AG_EXT_SYMBOL_PACKAGER}")
endif()

function(enable_stacktrace targetName)
    # Fix up difference between CMAKE system ID and generator platform.
    if ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
        set (TARGET_ARCH "AMD64")
    else()
        set(TARGET_ARCH "${CMAKE_HOST_SYSTEM_PROCESSOR}")
    endif()

    if (NOT DEFINED AG_EXT_SYMBOL_PACKAGER)
        set(SymTool "${AG_EXT_SYMBOL_PACKAGER}")
    elseif (NOT "${TARGET_ARCH}" STREQUAL "${CMAKE_HOST_SYSTEM_PROCESSOR}")
        message(STATUS "${targetName} - Not generating symbols due to cross-compilation.")
        return()
    else()
        # Use the SymbolPackager target name.
        set(SymTool "SymbolPackager")
    endif()

    if (DEFINED WIN32)
        # Use the PDB file to extract function symbols.
        add_custom_command(TARGET ${targetName} POST_BUILD
                           COMMAND "${SymTool}" ARGS "$<TARGET_PDB_FILE:${targetName}>"
                                                  -o "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.sym"
                                                  --exe "$<TARGET_FILE:${targetName}>"
                           COMMAND "${SymTool}" ARGS "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.sym"
                                                  -o "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.txt"
                           WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                           COMMENT "Creating stack trace data for ${targetName}...")
    else()
        # Ensure the linker generates a .map file and use it to produce a stack
        # trace symbol file which sits alongside the binary.
        target_link_options(${targetName} PRIVATE "LINKER:-Map=${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_BASE_NAME:${targetName}>.map")

        # install(TARGETS "${targetName}_Symbols" RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

        # EXPERIMENTAL Use nm to extract the symbols, even internal symbols.
        #add_custom_command(TARGET ${targetName} POST_BUILD
        #                   COMMAND nm ARGS -a --demangle "$<TARGET_FILE:${targetName}>" > "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_BASE_NAME:${targetName}>.nm"
        #                   COMMAND SymbolPackager ARGS "{CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_BASE_NAME:${targetName}>.nm"
        #                                          -o "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.sym"
        #                   COMMAND SymbolPackager ARGS "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.sym"
        #                                          -o "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.txt"
        #                   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        #                   COMMENT "Creating stack trace data for ${targetName}...")

        add_custom_command(TARGET ${targetName} POST_BUILD
                           COMMAND "${SymTool}" ARGS "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_BASE_NAME:${targetName}>.map"
                                                  -o "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.sym"
                           COMMAND "${SymTool}" ARGS "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.sym"
                                                  -o "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.txt"
                           WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                           COMMENT "Creating stack trace data for ${targetName}...")
    endif()
endfunction()

#function(enable_stacktrace targetName)
#    # Ensure the linker generates a .map file and use it to produce a stack
#    # trace symbol file which sits alongside the binary.
#    if (DEFINED WIN32)
#        target_link_options(${targetName} PRIVATE "LINKER:/MAP:${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_BASE_NAME:${targetName}>.map")
#    else()
#        target_link_options(${targetName} PRIVATE "LINKER:-Map=${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_BASE_NAME:${targetName}>.map")
#    endif()
#
#    # install(TARGETS "${targetName}_Symbols" RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
#    add_custom_command(TARGET ${targetName} POST_BUILD
#                       COMMAND SymbolPackager ARGS "$<TARGET_FILE_BASE_NAME:${targetName}>.map"
#                                              -o "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.sym"
#                       COMMAND SymbolPackager ARGS "$<TARGET_FILE_BASE_NAME:${targetName}>.sym"
#                                              -o "$<TARGET_FILE_DIR:${targetName}>/$<TARGET_FILE_BASE_NAME:${targetName}>.txt"
#                       WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
#                       COMMENT "Creating stack trace data for ${targetName}...")
#endfunction()

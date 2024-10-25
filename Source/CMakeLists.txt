# CMake configuration for the source code component of Mighty Oak

# Add utility library components.
add_subdirectory("Ag")

set(MO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")
include_directories("${MO_INCLUDE_DIR}")

# ARM Emulator Modules
add_subdirectory(AsmTools)
add_subdirectory(ArmEmu)

if ("${Qt6_FOUND}")
    message("-- [Optional] Configuring ARM Debugger project")
    add_subdirectory(ArmDebugger)
endif()
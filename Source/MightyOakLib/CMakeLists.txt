# CMakeLists configuration for the Emulator application library which exists
# to allow portions of the emulator application to be unit tested.

set(MO_LIB_INC_DIR "${MO_INCLUDE_DIR}/MightyOakLib")

# Define the library.
ag_add_library(MightyOakLib FOLDER      ARM
                            SOURCES     "PreCompiledHeader.hpp"
                                        "${MO_LIB_INC_DIR}/EmulatorApp.hpp"
                                        "EmulatorApp.cpp"
                                        "${MO_LIB_INC_DIR}/CliOptions.hpp"
                                        "CliOptions.cpp"
                            HEADERS     "${MO_INCLUDE_DIR}/MightyOakLib.hpp"
                            PUBLIC_LIBS ArmEmu Sdl3Tools)

target_precompile_headers(MightyOakLib PRIVATE [["PreCompiledHeader.hpp"]])

# Add the current directory so that the PCH can be found when referenced by
# PCH-generating source files in the build tree.
target_include_directories(MightyOakLib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

list(APPEND DOC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}"
                     "${MO_INCLUDE_DIR}/MightyOakLib.hpp"
                     "${MO_LIB_INC_DIR}")

source_group(Framework FILES
             "${MO_LIB_INC_DIR}/EmulatorApp.hpp"
             "EmulatorApp.cpp"
             "${MO_LIB_INC_DIR}/CliOptions.hpp"
             "CliOptions.cpp"
)

source_group(Build FILES
             "PreCompiledHeader.hpp"
)

# Define the unit test harness.
ag_add_test_app(MightyOak_Tests TEST_LIB MightyOakLib
                                SOURCES  "Test/Test_CliOptions.cpp"
                                         )

target_include_directories(MightyOak_Tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# CMakeLists configuration for the ARM Debugger Script library.

ag_add_library(ArmDbg     FOLDER      ARM
                           SOURCES     "ScriptLexer.cpp"
                                       "ScriptLexer.hpp"
                                       "CommandDefs.cpp"
                                       "${MO_INCLUDE_DIR}/ArmDbg/CommandDefs.hpp"
                                       "CommandParser.cpp"
                                       "CommandParser.hpp"
                                       "StateFormatter.cpp"
                                       "StateFormatter.hpp"
                                       "DebugSession.cpp"
                                       "${MO_INCLUDE_DIR}/ArmDbg/DebugSession.hpp"
                                       "ScriptRunner.cpp"
                                       "${MO_INCLUDE_DIR}/ArmDbg/ScriptRunner.hpp"
                           WIN_SOURCES "PipeServer.cpp"
                                       "PipeServer.hpp"
                                       "PipeClient.cpp"
                                       "PipeClient.hpp"
                           HEADERS     "${MO_INCLUDE_DIR}/ArmDbg.hpp"
                           PUBLIC_LIBS ArmEmu AsmTools)

source_group(Parsing FILES
             "ScriptLexer.cpp"
             "ScriptLexer.hpp"
             "CommandDefs.cpp"
             "${MO_INCLUDE_DIR}/ArmDbg/CommandDefs.hpp"
             "CommandParser.cpp"
             "CommandParser.hpp")

source_group(Session FILES
             "StateFormatter.cpp"
             "StateFormatter.hpp"
             "DebugSession.cpp"
             "${MO_INCLUDE_DIR}/ArmDbg/DebugSession.hpp"
             "ScriptRunner.cpp"
             "${MO_INCLUDE_DIR}/ArmDbg/ScriptRunner.hpp")

source_group(IPC FILES
             "PipeServer.cpp"
             "PipeServer.hpp"
             "PipeClient.cpp"
             "PipeClient.hpp")

source_group(Interface FILES
             "${MO_INCLUDE_DIR}/ArmDbg.hpp")

# Define the unit test harness.
set(ARMDBG_TEST_SOURCES "Test/Test_Main.cpp"
                        "Test/Test_ScriptLexer.cpp"
                        "Test/Test_CommandParser.cpp"
                        "Test/Test_StateFormatter.cpp"
                        "Test/Test_DebugSession.cpp"
                        "Test/Test_ScriptRunner.cpp")

if(DEFINED WIN32)
    list(APPEND ARMDBG_TEST_SOURCES "Test/Test_PipeProtocol.cpp")
endif()

ag_add_test_app(ArmDbg_Tests   TEST_LIB ArmDbg
                               SOURCES  ${ARMDBG_TEST_SOURCES})

target_include_directories(ArmDbg_Tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(ArmDbg_Tests PRIVATE AsmTools)

# CMakeLists configuration for the ARM Processor Emulator library

# Define the library.
add_ag_library(ArmEmu   FOLDER      ARM
                        SOURCES     ArmCore.hpp
                                    ArmCore.cpp
                                    Hardware.inl
                                    TestBedHardware.inl
                                    RegisterFile.inl
                                    ARMv2CoreRegisterFile.inl
                                    AluInstructions.inl
                                    DataTransferInstructions.inl
                                    InstructionPipeline.inl
                                    ExecutionUnit.inl
                                    SystemConfigurations.inl
                                    ArmSystem.inl

                                    IrqSink.hpp
                                    SystemResources.hpp
                                    SystemResources.cpp
                                    ArmEmu.cpp
                                    Hardware.hpp
                                    Hardware.cpp
                                    RegisterFile.hpp
                                    RegisterFile.cpp
                                    AluOperations.h
                                    ArithmeticLogicUnit.hpp
                                    ArithmeticLogicUnit.cpp
                                    DataTransfer.hpp
                                    DataTransfer.cpp
                                    InstructionPipeline.hpp
                                    InstructionPipeline.cpp
                                    TestSystem.hpp
                                    TestSystem.cpp
                        HEADERS     ${MO_INCLUDE_DIR}/ArmEmu.hpp
                        PUBLIC_LIBS AgCore AsmTools)

if(DEFINED CMAKE_ASM_MASM_COMPILER AND "${USE_ASM}")
    target_sources(ArmEmu PRIVATE AluOperations_Win32_x64.asm)
else()
    target_sources(ArmEmu PRIVATE AluOperations_NoArch.cpp)
endif()

# Define the unit test harness.
add_ag_test_app(ArmEmu_Tests    TEST_LIB ArmEmu
                                SOURCES  TestTools.cpp
                                         TestTools.hpp
                                         TestConstraints.cpp
                                         TestConstraints.hpp
                                         Test_Constraints.cpp
                                         Test_Core.cpp
                                         Test_RegisterFile.cpp
                                         Test_Hardware.cpp
                                         Test_AluOperations.cpp
                                         Test_ALU.cpp
                                         Test_DataTransfer.cpp)

# We need the assembly language tools to perform the tests.
target_link_libraries(ArmEmu_Tests PRIVATE AsmTools)

add_cli_app(EmuPerf_Test FOLDER ARM
                         NAME "EmuPerfTest"
                         DESCRIPTION "Measures the performance of different emulated system configurations."
                         VERSION "${PROJECT_VERSION}"
                         SOURCES EmuPerfTest_Main.cpp
                         LIBS AgCore ArmEmu)

set(DhyrstoneSourceIn "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/ArmEmu/Dhrystone2_1.arm")

cmake_path(ABSOLUTE_PATH DhyrstoneSourceIn
           NORMALIZE OUTPUT_VARIABLE DhrystoneSource)

# Add a custom command to assemble the Dhrystone source code into
# ARM machine code.
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Dhrystone.bin"
                   COMMAND AAsm
                   ARGS "${DhrystoneSource}" "-o"
                        "${CMAKE_CURRENT_BINARY_DIR}/Dhrystone.bin"
                   DEPENDS "${DhrystoneSource}"
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   COMMENT "Assembling ${DhrystoneSource}...")

# Embed the ARM machine code in the program source code.
add_static_data(EmuPerf_Test "DhrystoneProgram.hpp" BINARY
                SOURCES "${CMAKE_CURRENT_BINARY_DIR}/Dhrystone.bin")

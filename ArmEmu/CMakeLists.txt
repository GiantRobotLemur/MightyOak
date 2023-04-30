# CMakeLists configuration for the ARM Processor Emulator library

# Define the library.
add_ag_library(ArmEmu   FOLDER      ARM
                        SOURCES     ArmCore.hpp
                                    ArmCore.cpp
                                    IrqSink.hpp
                                    Hardware.inl
                                    TestBedHardware.inl
                                    RegisterFile.inl
                                    ARMv2CoreRegisterFile.inl
                                    AluInstructions.inl
                                    AluOperations.h
                                    DataTransferInstructions.inl
                                    InstructionPipeline.inl
                                    ExecutionUnit.inl
                                    SystemConfigurations.inl
                                    ArmSystem.inl
                                    ArmEmu.cpp
                        HEADERS     ${MO_INCLUDE_DIR}/ArmEmu.hpp
                        PUBLIC_LIBS AgCore)

if(DEFINED CMAKE_ASM_MASM_COMPILER AND "${USE_ASM}")
    target_sources(ArmEmu PRIVATE AluOperations_Win32_x64.asm)
    source_group(Emulation FILES AluOperations_Win32_x64.asm)
else()
    target_sources(ArmEmu PRIVATE AluOperations_NoArch.cpp)
    source_group(Emulation FILES AluOperations_NoArch.cpp)
endif()

source_group(Tools FILES
             ArmCore.hpp
             ArmCore.cpp)

source_group(Emulation FILES
             Hardware.inl
             TestBedHardware.inl
             RegisterFile.inl
             ARMv2CoreRegisterFile.inl
             AluInstructions.inl
             AluOperations.h
             DataTransferInstructions.inl
             InstructionPipeline.inl
             ExecutionUnit.inl
             SystemConfigurations.inl
             ArmSystem.inl)

source_group(Interface FILES
             IrqSink.hpp
             ${MO_INCLUDE_DIR}/ArmEmu.hpp)

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
                         LIBS AgCore AsmTools ArmEmu)

set(DhyrstoneSourceIn "${PROJECT_SOURCE_DIR}/Tests/ArmEmu/Dhrystone2_1.arm")

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

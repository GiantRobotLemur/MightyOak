# CMakeLists configuration for the ARM Processor Emulator library

# Define the library.
ag_add_library(ArmEmu   FOLDER      ARM
                        SOURCES     ArmCore.hpp
                                    ArmCore.cpp
                                    Hardware.inl
                                    TestBedHardware.inl
                                    MemcHardware.cpp
                                    MemcHardware.hpp
                                    RegisterFile.inl
                                    ARMv2CoreRegisterFile.inl
                                    AluInstructions.inl
                                    AluOperations.h
                                    DataTransferInstructions.inl
                                    InstructionDecoder.inl
                                    ARMv2InstructionDecoder.inl
                                    InstructionPipeline.inl
                                    ExecutionUnit.inl
                                    SystemConfigurations.inl
                                    ArmSystem.inl
                                    ArmEmu.cpp
                                    EmuOptions.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/EmuOptions.hpp
                                    AddressMap.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/AddressMap.hpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/IKeyboardController.hpp
                                    AcornKeyboardController.cpp
                                    AcornKeyboardController.hpp
                                    IOC.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/IOC.hpp
                                    VIDC10.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/VIDC10.hpp
                                    ArmSystemBuilder.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/ArmSystemBuilder.hpp
                                    ExecutionMetrics.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/ExecutionMetrics.hpp
                                    GuestEventQueue.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/GuestEventQueue.hpp
                                    SystemContext.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/SystemContext.hpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/HostMessageID.hpp
                                    ArmSystem.cpp
                                    ${MO_INCLUDE_DIR}/ArmEmu/ArmSystem.hpp
                        HEADERS     ${MO_INCLUDE_DIR}/ArmEmu.hpp
                        PUBLIC_LIBS AgCore readerwriterqueue)

if(DEFINED CMAKE_ASM_MASM_COMPILER AND "${USE_ASM}")
    target_sources(ArmEmu PRIVATE AluOperations_Win32_x64.asm)
    source_group(Emulation FILES AluOperations_Win32_x64.asm)
else()
    target_sources(ArmEmu PRIVATE AluOperations_NoArch.cpp)
    source_group(Emulation FILES AluOperations_NoArch.cpp)
endif()

list(APPEND DOC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}"
                     "${MO_INCLUDE_DIR}/ArmEmu.hpp"
                     "${MO_INCLUDE_DIR}/ArmEmu")

source_group(Tools FILES
             ArmCore.hpp
             ArmCore.cpp)

source_group(Emulation FILES
             Hardware.inl
             TestBedHardware.inl
             MemcHardware.cpp
             MemcHardware.hpp
             AcornKeyboardController.cpp
             AcornKeyboardController.hpp
             RegisterFile.inl
             ARMv2CoreRegisterFile.inl
             AluInstructions.inl
             AluOperations.h
             DataTransferInstructions.inl
             InstructionDecoder.inl
             ARMv2InstructionDecoder.inl
             InstructionPipeline.inl
             ExecutionUnit.inl
             SystemConfigurations.inl
             ArmSystem.inl)

source_group(Interface FILES
             EmuOptions.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/EmuOptions.hpp
             ArmEmu.cpp
             ${MO_INCLUDE_DIR}/ArmEmu.hpp
             AddressMap.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/AddressMap.hpp
             ${MO_INCLUDE_DIR}/ArmEmu/IKeyboardController.hpp
             IOC.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/IOC.hpp
             VIDC10.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/VIDC10.hpp
             ArmSystemBuilder.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/ArmSystemBuilder.hpp
             ExecutionMetrics.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/ExecutionMetrics.hpp
             GuestEventQueue.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/GuestEventQueue.hpp
             ${MO_INCLUDE_DIR}/ArmEmu/HostMessageID.hpp
             SystemContext.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/SystemContext.hpp
             ArmSystem.cpp
             ${MO_INCLUDE_DIR}/ArmEmu/ArmSystem.hpp)

# Define the unit test harness.
ag_add_test_app(ArmEmu_Tests    TEST_LIB ArmEmu
                                SOURCES  Test/TestTools.cpp
                                         Test/TestTools.hpp
                                         Test/LoggerDevice.cpp
                                         Test/LoggerDevice.hpp
                                         Test/TestConstraints.cpp
                                         Test/TestConstraints.hpp
                                         Test/TestExecTools.cpp
                                         Test/TestExecTools.hpp
                                         Test/Test_Constraints.cpp
                                         Test/Test_Core.cpp
                                         Test/Test_RegisterFile.cpp
                                         Test/Test_Hardware.cpp
                                         Test/Test_MemcHardware.cpp
                                         Test/Test_MemcSystem.cpp
                                         Test/Test_AluOperations.cpp
                                         Test/Test_ALU.cpp
                                         Test/Test_DataTransfer.cpp
                                         Test/Test_CoProcessor.cpp
                                         Test/Test_Options.cpp
                                         Test/Test_GuestEventQueue.cpp
                                         Test/Test_ArmSystemBuilder.cpp
                                         Test/Test_Main.cpp)

target_include_directories(ArmEmu_Tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# We need the assembly language tools to perform the tests.
target_link_libraries(ArmEmu_Tests PRIVATE AsmTools)

ag_add_cli_app(EmuPerf_Test FOLDER ARM
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
ag_add_static_data(EmuPerf_Test "DhrystoneProgram.hpp" BINARY
                   SOURCES "${CMAKE_CURRENT_BINARY_DIR}/Dhrystone.bin")

# Add a custom command to assemble the MEMC Test System ROM source code into
# ARM machine code.
set(MemcTestSystemSourceIn "${PROJECT_SOURCE_DIR}/Tests/ArmEmu/MemcTestSystemRom.arm")
set(MemcTestRomImagePath "${CMAKE_CURRENT_BINARY_DIR}/MemcTestRom.bin")

cmake_path(ABSOLUTE_PATH MemcTestSystemSourceIn
           NORMALIZE OUTPUT_VARIABLE MemcTestSystemSource)

add_custom_command(OUTPUT "${MemcTestRomImagePath}"
                   COMMAND AAsm
                   ARGS "${MemcTestSystemSource}" "-o" "${MemcTestRomImagePath}"
                   DEPENDS "${MemcTestSystemSource}"
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   COMMENT "Assembling ${MemcTestSystemSource}...")

# Embed the ARM machine code in the program source code.
ag_add_static_data(EmuPerf_Test "MemcTestRom.hpp" BINARY
                   SOURCES "${MemcTestRomImagePath}")

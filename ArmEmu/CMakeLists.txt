# CMakeLists configuration for the ARM Processor Emulator library

# Define the library.
add_ag_library(ArmEmu   FOLDER      ARM
                        SOURCES     ArmCore.hpp
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
                        PUBLIC_LIBS AgCore)

if(DEFINED CMAKE_ASM_MASM_COMPILER AND "${USE_ASM}")
    target_sources(ArmEmu PRIVATE AluOperations_Win32_x64.asm)
else()
    target_sources(ArmEmu PRIVATE AluOperations_NoArch.cpp)
endif()


# Define the unit test harness.
add_ag_test_app(ArmEmu_Tests    TEST_LIB ArmEmu
                                SOURCES  TestTools.cpp
                                         TestTools.hpp
                                         Test_Core.cpp
                                         Test_AluOperations.cpp
                                         Test_ALU.cpp
                                         Test_DataTransfer.cpp)

# We need the assembly language tools to perform the tests.
target_link_libraries(ArmEmu_Tests PRIVATE AsmTools)
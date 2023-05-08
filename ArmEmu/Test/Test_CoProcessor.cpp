//! @file Test_CoProcessor.cpp
//! @brief The definition of unit tests verifying the execution of generic
//! co-processor instructions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>
#include "ArmEmu.hpp"

#include "TestTools.hpp"
#include "TestExecTools.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
const CoreTestParams armV2CPAccessDenied[] = {
    { TLOC, "MRC_ReadCPReg",    "R9=0xCAFEBABE,Mode=Svc26",
                                "R9=0xCAFEBABE,PC=0x10",
                                "MRC CP12,0,R9,CR4,CR0,0" },

    { TLOC, "MCR_WriteCP",  "R9=0xCAFEBABE,Mode=Svc26",
                            "R9=0xCAFEBABE,PC=0x10",
                            "MCR CP9,0,R9,CR0,CR0,0" },

    { TLOC, "CDP_Invalid",  "Mode=Svc26",
                            "PC=0x10",
                            "CDP CP5,0,CR2,CR0,CR1,0" },

    { TLOC, "LDC_Invalid",  "R2=0x8100,Mode=Svc26",
                            "PC=0x10",
                            "LDC CP11,CR0,[R2]" },

    { TLOC, "STC_Invalid",  "R2=0x8100,Mode=Svc26",
                            "PC=0x10",
                            "STC CP1,CR0,[R2]" },
};

const CoreTestParams armV2aCP15Access[] = {
    // MCR CP15.
    { TLOC, "MRC_ReadCP15_CR0", "R9=0xCAFEBABE,Mode=Svc26",
                                "R9=0x41560300",
                                "MRC CP15,0,R9,CR0,CR0,0" },
    { TLOC, "MRC_ReadCP15_CR1", "R8=0xCAFEBABE,Mode=Svc26",
                                "R8=0x0",
                                "MRC CP15,0,R8,CR1,CR0,0" },
    { TLOC, "MRC_ReadCP15_CR2", "CP15,CR2=2,R7=0xCAFEBABE,Mode=Svc26",
                                "R7=0x2",
                                "MRC CP15,0,R7,CR2,CR0,0" },
    { TLOC, "MRC_ReadCP15_CR3", "CP15,CR3=0xDEADBEEF,R7=0xCAFEBABE,Mode=Svc26",
                                "R7=0xDEADBEEF",
                                "MRC CP15,0,R7,CR3,CR0,0" },
    { TLOC, "MRC_ReadCP15_CR4", "CP15,CR4=0xDEADBEEF,R7=0xCAFEBABE,Mode=Svc26",
                                "R7=0xDEADBEEF",
                                "MRC CP15,0,R7,CR4,CR0,0" },
    { TLOC, "MRC_ReadCP15_CR5", "CP15,CR5=0xDEADBEEF,R7=0xCAFEBABE,Mode=Svc26",
                                "R7=0xDEADBEEF",
                                "MRC CP15,0,R7,CR5,CR0,0" },

    { TLOC, "MRC_ReadCP15_Unprivileged",    "R9=0xCAFEBABE,Mode=Usr26",
                                            "R9=0xCAFEBABE,PC=0x10",
                                            "MRC CP15,0,R9,CR0,CR0,0" },
    { TLOC, "MRC_ReadCP15_Extranious1", "R9=0xCAFEBABE,Mode=Svc26",
                                        "R9=0xCAFEBABE,PC=0x10",
                                        "MRC CP15,0,R9,CR0,CR3,0" },
    { TLOC, "MRC_ReadCP15_Extranious2", "R9=0xCAFEBABE,Mode=Svc26",
                                        "R9=0xCAFEBABE,PC=0x10",
                                        "MRC CP15,1,R9,CR0,CR0,0" },
    { TLOC, "MRC_ReadCP15_Extranious3", "R9=0xCAFEBABE,Mode=Svc26",
                                        "R9=0xCAFEBABE,PC=0x10",
                                        "MRC CP15,0,R9,CR0,CR0,1" },
    { TLOC, "MRC_ReadNotCP15",  "R9=0xCAFEBABE,Mode=Svc26",
                                "R9=0xCAFEBABE,PC=0x10",
                                "MRC CP11,0,R9,CR0,CR0,0" },

    // MRC CP15
    { TLOC, "MCR_WriteCP15_CR0",    "R9=0xCAFEBABE,Mode=Svc26",
                                    "R9=0xCAFEBABE,CP15,CR0=0x41560300",
                                    "MCR CP15,0,R9,CR0,CR0,0" },
    { TLOC, "MCR_WriteCP15_CR1",    "R8=0xCAFEBABE,Mode=Svc26",
                                    "CP15,CR1=0",
                                    "MCR CP15,0,R8,CR1,CR0,0" },
    { TLOC, "MCR_WriteCP15_CR2",    "CP15,CR2=2,R7=0xCAFEBABE,Mode=Svc26",
                                    "CP15,CR2=6",
                                    "MCR CP15,0,R7,CR2,CR0,0" },
    { TLOC, "MCR_WriteCP15_CR3",    "CP15,CR3=0xDEADBEEF,R7=0xCAFEBABE,Mode=Svc26",
                                    "CP15,CR3=0xCAFEBABE",
                                    "MCR CP15,0,R7,CR3,CR0,0" },
    { TLOC, "MCR_WriteCP15_CR4",    "CP15,CR4=0xDEADBEEF,R7=0xCAFEBABE,Mode=Svc26",
                                    "CP15,CR4=0xCAFEBABE",
                                    "MCR CP15,0,R7,CR4,CR0,0" },
    { TLOC, "MCR_WriteCP15_CR5",    "CP15,CR5=0xDEADBEEF,R7=0xCAFEBABE,Mode=Svc26",
                                    "CP15,CR5=0xCAFEBABE",
                                    "MCR CP15,0,R7,CR5,CR0,0" },

    { TLOC, "MCR_WriteCP15_Unprivileged",   "R9=0xCAFEBABE,Mode=Usr26",
                                            "R9=0xCAFEBABE,PC=0x10",
                                            "MCR CP15,0,R9,CR0,CR0,0" },
    { TLOC, "MCR_WriteCP15_Extranious1",    "R9=0xCAFEBABE,Mode=Svc26",
                                            "R9=0xCAFEBABE,PC=0x10",
                                            "MCR CP15,0,R9,CR0,CR3,0" },
    { TLOC, "MCR_WriteCP15_Extranious2",    "R9=0xCAFEBABE,Mode=Svc26",
                                            "R9=0xCAFEBABE,PC=0x10",
                                            "MCR CP15,1,R9,CR0,CR0,0" },
    { TLOC, "MCR_WriteCP15_Extranious3",    "R9=0xCAFEBABE,Mode=Svc26",
                                            "R9=0xCAFEBABE,PC=0x10",
                                            "MCR CP15,0,R9,CR0,CR0,1" },
    { TLOC, "MCR_WriteNotCP15", "R9=0xCAFEBABE,Mode=Svc26",
                                "R9=0xCAFEBABE,PC=0x10",
                                "MCR CP4,0,R9,CR1,CR7,0" },

    // Show other instruction on CP15 are invalid.
    { TLOC, "CDP_CP15", "Mode=Svc26",
                        "PC=0x10",
                        "CDP CP15,0,CR2,CR0,CR1,0" },

    { TLOC, "LDC_CP15", "R2=0x8100,Mode=Svc26",
                        "PC=0x10",
                        "LDC CP15,CR0,[R2]" },

    { TLOC, "STC_CP15", "R2=0x8100,Mode=Svc26",
                        "PC=0x10",
                        "STC CP15,CR0,[R2]" },
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Registers test for generic co-processor instructions for each type
//! of system they should be tested against.
void registerCoProcessorExecTests()
{
    // Register tests to show no access to co-processors on the ARM 2.
    RegisterExecTests<ArmV2TestSystemTraits>("ARM2_CoProcessor", armV2CPAccessDenied,
                                             std::size(armV2CPAccessDenied));

    // Repeat the test for the ARMv2a architecture.
    RegisterExecTests<ArmV2aTestSystemTraits>("ARM3_CoProcessor", armV2CPAccessDenied,
                                              std::size(armV2CPAccessDenied));
    RegisterExecTests<ArmV2aTestSystemTraits>("ARM3_CoProcessor", armV2aCP15Access,
                                              std::size(armV2aCP15Access));
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


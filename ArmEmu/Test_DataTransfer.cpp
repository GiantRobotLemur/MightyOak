//! @file Test_DataTransfer.cpp
//! @brief The definition of unit test for the emulation of data transfer
//! instructions of a virtual ARM processor.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/Core/Exception.hpp"
#include "ArmEmu.hpp"

#include "TestTools.hpp"

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

using RegRequirement = std::pair<CoreRegister, uint32_t>;
using RegReqList = std::initializer_list<RegRequirement>;
using MemRequirement = std::pair<uint32_t, uint32_t>;
using MemReqList = std::initializer_list<MemRequirement>;

struct DTPoint
{
public:
    std::string_view Name;
    std::string_view SourceCode;
    std::vector<RegRequirement> Registers;
    std::vector<MemRequirement> Memory;

    DTPoint(const char *name, const char *source,
            const RegReqList &regs,
            const MemReqList &mem) :
        Name(name),
        SourceCode(source),
        Registers(regs),
        Memory(mem)
    {
    }
};

std::ostream &operator<<(std::ostream &os, const DTPoint &value)
{
    return os << value.Name;
}

class DataTransfer : public testing::TestWithParam<DTPoint>
{
};

TEST_P(DataTransfer, ExecuteCode)
{
    try
    {
        const auto &testInfo = GetParam();
        IArmSystemUPtr specimen = createUserModeTestSystem(testInfo.SourceCode.data());

        for (const auto &resultReq : testInfo.Registers)
        {
            uint32_t initialValue = specimen->getCoreRegister(resultReq.first);
            uint32_t expectedValue = 0;

            switch (resultReq.first)
            {
            case CoreRegister::R14: expectedValue = 0x0C000003u; break; // The results of raising reset.
            case CoreRegister::CPSR: expectedValue = 0x0C000003u; break;
            case CoreRegister::PC: expectedValue = 0x00000000u; break; // Before pipelining.
            default: break;
            }

            EXPECT_TRUE(isEqualReg(resultReq.first, initialValue, expectedValue));
        }

        ExecutionMetrics metrics = specimen->run();

        EXPECT_GT(metrics.CycleCount, 1u);

        for (const auto &resultReg : testInfo.Registers)
        {
            uint32_t finalValue = specimen->getCoreRegister(resultReg.first);

            EXPECT_TRUE(isEqualReg(resultReg.first, finalValue, resultReg.second));
        }

        for (const auto &resultMem : testInfo.Memory)
        {
            uint32_t finalValue = 0;
            EXPECT_TRUE(specimen->readFromLogicalAddress(resultMem.first,
                                                         sizeof(uint32_t),
                                                         &finalValue));

            EXPECT_TRUE(isEqualMemWord(resultMem.first, finalValue, resultMem.second));
        }
    }
    catch (const Ag::Exception &error)
    {
        auto result = ::testing::AssertionFailure();
        result << error.getDomain() << ": " << error.getMessage() << std::endl;
        result << error.getDetail();

        EXPECT_TRUE(result);
    }
}


////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
DTPoint testPoints[] = {

// LDR
{ "LDR-PCRelative", "LDR R0,myData : B end : .myData: EQUD 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0xCAFEBABEu }, },
    { /* No memory requirements */ }
},

{ "LDR-RegisterNoOffset", "ADR R0,myData : LDR R1,[R0] : B end : .myData: EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0xDEADBEEFu }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedImmedOffset", "ADR R0,myData : LDR R1,[R0,#4] : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0xCAFEBABEu }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedImmedOffsetWriteBack", "ADR R0,myData : LDR R1,[R0,#4]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0xCAFEBABEu }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedRegOffset", "ADR R0,myData : MOV R1,#4 : LDR R2,[R0,R1] : B end : .myData: EQUD 0xCAFEBEEF, 0xBABEDEAD : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 4u }, { CoreRegister::R2, 0xBABEDEADu }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedRegOffsetWriteBack", "ADR R0,myData : MOV R1,#4 : LDR R2,[R0,R1]! : B end : .myData: EQUD 0xCAFEBEEF, 0xBABEDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 4u }, { CoreRegister::R2, 0xBABEDEADu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedRegLsl", "ADR R0,myData : MOV R1,#1 : LDR R2,[R0,R1,LSL #2]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 1u }, { CoreRegister::R2, 0xBEEFDEADu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedRegLsr", "ADR R0,myData : MOV R1,#16 : LDR R2,[R0,R1,LSR #2]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xBEEFDEADu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedRegAsr", "ADR R0,myData : MOV R1,#16 : LDR R2,[R0,R1,ASR #2]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xBEEFDEADu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedRegRor", "ADR R0,myData : MOV R1,#16 : LDR R2,[R0,R1,ROR #2]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xBEEFDEADu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedRegRrx", "ADR R0,myData : MOV R1,#8 : LDR R2,[R0,R1,RRX]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 8u }, { CoreRegister::R2, 0xBEEFDEADu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedByte", "ADR R0,myData : LDRB R1,[R0,#5]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8015u }, { CoreRegister::R1, 0xBAu }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedUnaligned1", "ADR R0,myData : LDR R1,[R0,#5]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8015u }, { CoreRegister::R1, 0xBECAFEBAu }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedUnaligned2", "ADR R0,myData : LDR R1,[R0,#2]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8012u }, { CoreRegister::R1, 0xBEEFDEADu }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedUnaligned3", "ADR R0,myData : LDR R1,[R0,#3]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8013u }, { CoreRegister::R1, 0xADBEEFDEu }, },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedAddrException", "MOV R0,#&F0000000 : LDR R1,[R0,#3]!",
    { { CoreRegister::R0, 0xF0000000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u },  },
    { /* No memory requirements */ }
},

{ "LDR-PreIndexedDataAbort", "MOV R0,#&00F00000 : LDR R1,[R0,#3]!",
    { { CoreRegister::R0, 0x00F00000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu },  },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedImmedOffset", "ADR R0,myData : LDR R1,[R0],#4 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0xDEADBEEFu }, },
    { /* No memory requirements */ }
},

// TODO: Test accessing protected memory in SVC mode.
{ "LDR-PostIndexedImmediateUserPrivilages", "ADR R0,myData : LDRT R1,[R0],#4 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0xDEADBEEFu }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedRegOffset", "ADR R0,myData : MOV R1,#4 : LDR R2,[R0],R1 : B end : .myData: EQUD 0xCAFEBEEF, 0xBABEDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 4u }, { CoreRegister::R2, 0xCAFEBEEFu }, },
    { /* No memory requirements */ }
},

// TODO: Test accessing protected memory in SVC mode.
{ "LDR-PostIndexedRegOffsetUserPrivilages", "ADR R0,myData : MOV R1,#4 : LDRT R2,[R0],R1 : B end : .myData: EQUD 0xCAFEBEEF, 0xBABEDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 4u }, { CoreRegister::R2,0xCAFEBEEFu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedRegLsl", "ADR R0,myData : MOV R1,#1 : LDR R2,[R0],R1,LSL #2 : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 1u }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedRegLsr", "ADR R0,myData : MOV R1,#16 : LDR R2,[R0],R1,LSR #2 : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedRegAsr", "ADR R0,myData : MOV R1,#16 : LDR R2,[R0],R1,ASR #2 : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedRegRor", "ADR R0,myData : MOV R1,#16 : LDR R2,[R0],R1,ROR #2 : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedRegRrx", "ADR R0,myData : MOV R1,#8 : LDR R2,[R0],R1,RRX : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 8u }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedByte", "ADR R0,myData : LDRB R1,[R0],#5 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8015u }, { CoreRegister::R1, 0xEFu }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedUnaligned1", "ADR R0,myData + 5 : LDR R1,[R0],#4 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8019u }, { CoreRegister::R1, 0xBECAFEBAu }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedUnaligned2", "ADR R0,myData + 2 : LDR R1,[R0],#4 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8016u }, { CoreRegister::R1, 0xBEEFDEADu }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedUnaligned3", "ADR R0,myData + 3 : LDR R1,[R0],#4 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8017u }, { CoreRegister::R1, 0xADBEEFDEu }, },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedAddrException", "MOV R0,#&F0000000 : LDR R1,[R0],#3",
    { { CoreRegister::R0, 0xF0000000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u },  },
    { /* No memory requirements */ }
},

{ "LDR-PostIndexedDataAbort", "MOV R0,#&00F00000 : LDR R1,[R0],#3",
    { { CoreRegister::R0, 0x00F00000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu },  },
    { /* No memory requirements */ }
},


// STR
{ "STR-PCRelative", "MVN R0,#&F000000A : STR R0,myData : B end : .myData: EQUD 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x0FFFFFF5u }, },
    { { 0x8010, 0x0FFFFFF5u }, }
},

{ "STR-RegisterNoOffset", "ADR R0,myData : MOV R1,#&5A : STR R1,[R0] : B end : .myData: EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0x5Au }, },
    { { 0x8014u, 0x5Au }, }
},

{ "STR-PreIndexedImmedOffset", "ADR R0,myData : MVN R1,#&42 : STR R1,[R0,#4] : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0xFFFFFFBDu }, },
    { { 0x8014, 0xDEADBEEF }, { 0x8018, 0xFFFFFFBDu } }
},

{ "STR-PreIndexedImmedOffsetWriteBack", "ADR R0,myData : MOV R1,#69 : STR R1,[R0,#4]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 69u }, },
    { { 0x8014, 0xDEADBEEF }, { 0x8018, 69u } }
},

{ "STR-PreIndexedRegOffset", "ADR R0,myData : MOV R1,#4 : MVN R2,#&FC000003 : STR R2,[R0,R1] : B end : .myData: EQUD 0xCAFEBEEF, 0xBABEDEAD : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 4u }, { CoreRegister::R2, 0x03FFFFFCu }, },
    { { 0x8018, 0xCAFEBEEF }, { 0x801C, 0x03FFFFFCu } }
},

{ "STR-PreIndexedRegOffsetWriteBack", "ADR R0,myData : MOV R1,#4 : MVN R2,#&A8000003 : STR R2,[R0,R1]! : B end : .myData: EQUD 0xCAFEBEEF, 0xBABEDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 4u }, { CoreRegister::R2, 0x57FFFFFCu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xCAFEBEEF }, { 0x801C, 0x57FFFFFCu } }
},

{ "STR-PreIndexedRegLsl", "ADR R0,myData : MOV R1,#1 : MVN R2,#&A8000003 : STR R2,[R0,R1,LSL #2]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 1u }, { CoreRegister::R2, 0x57FFFFFCu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xCAFEBABE }, { 0x801C, 0x57FFFFFCu } }
},

{ "STR-PreIndexedRegLsr", "ADR R0,myData : MOV R1,#16 : MVN R2,#&E9000000 : STR R2,[R0,R1,LSR #2]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0x16FFFFFFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xCAFEBABE }, { 0x801C, 0x16FFFFFFu } }
},

{ "STR-PreIndexedRegAsr", "ADR R0,myData : MOV R1,#16 : MVN R2,#&0E900000 : STR R2,[R0,R1,ASR #2]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xF16FFFFFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xCAFEBABE }, { 0x801C, 0xF16FFFFFu } }
},

{ "STR-PreIndexedRegRor", "ADR R0,myData : MOV R1,#16 : MVN R2,#&00530000 : STR R2,[R0,R1,ROR #2]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xFFACFFFFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xCAFEBABE }, { 0x801C, 0xFFACFFFFu } }
},

{ "STR-PreIndexedRegRrx", "ADR R0,myData : MOV R1,#8 : MVN R2,#&00069000 : STR R2,[R0,R1,RRX]! : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 8u }, { CoreRegister::R2, 0xFFF96FFFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xCAFEBABE }, { 0x801C, 0xFFF96FFFu } }
},

{ "STR-PreIndexedByte", "ADR R0,myData : MOV R1,#&00000990 : STRB R1,[R0,#5]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8019u }, { CoreRegister::R1, 0x990u }, },
    { { 0x8014, 0xDEADBEEF }, { 0x8018, 0xCAFE90BEu } }
},

// NOTE: Addresses have bits 0 and 1 forced to 0 for STR instructions (see ARMv2 Data Sheet, page 26).
{ "STR-PreIndexedUnaligned1", "ADR R0,myData : MVN R1,#&3000000D : STR R1,[R0,#5]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8019u }, { CoreRegister::R1, 0xCFFFFFF2u }, },
    { { 0x8014, 0xDEADBEEF }, { 0x8018, 0xCFFFFFF2u } }
},

{ "STR-PreIndexedUnaligned2", "ADR R0,myData : MVN R1,#&5000000A : STR R1,[R0,#2]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8016u }, { CoreRegister::R1, 0xAFFFFFF5u }, },
    { { 0x8014, 0xAFFFFFF5u }, { 0x8018, 0xCAFEBABE } }
},

{ "STR-PreIndexedUnaligned3", "ADR R0,myData : MVN R1,#&00076000 : STR R1,[R0,#3]! : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8017u }, { CoreRegister::R1, 0xFFF89FFFu }, },
    { { 0x8014, 0xFFF89FFFu }, { 0x8018, 0xCAFEBABE } }
},

{ "STR-PreIndexedAddrException", "MOV R0,#&F0000000 : STR R1,[R0,#3]!",
    { { CoreRegister::R0, 0xF0000000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u },  },
    { /* No memory requirements */ }
},

{ "STR-PreIndexedDataAbort", "MOV R0,#&00F00000 : STR R1,[R0,#3]!",
    { { CoreRegister::R0, 0x00F00000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu },  },
    { /* No memory requirements */ }
},

{ "STR-PostIndexedImmedOffset", "ADR R0,myData : MVN R1,#&42 : STR R1,[R0],#4 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 0xFFFFFFBDu }, },
    { { 0x8014, 0xFFFFFFBDu }, { 0x8018, 0xCAFEBABE } }
},

// TODO: Try this in SVC mode.
{ "STR-PostIndexedImmedOffsetUserPrivileges", "ADR R0,myData : MOV R1,#69 : STRT R1,[R0],#4 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 69u }, },
    { { 0x8014, 69u }, { 0x8018, 0xCAFEBABE } }
},

{ "STR-PostIndexedRegOffset", "ADR R0,myData : MOV R1,#4 : MVN R2,#&FC000003 : STR R2,[R0],R1 : B end : .myData: EQUD 0xCAFEBEEF, 0xBABEDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 4u }, { CoreRegister::R2, 0x03FFFFFCu }, },
    { { 0x8018, 0x03FFFFFCu }, { 0x801C, 0xBABEDEAD } }
},

// TODO: Try this in SVC mode.
{ "STR-PostIndexedRegOffsetUserPrivileges", "ADR R0,myData : MOV R1,#4 : MVN R2,#&A8000003 : STRT R2,[R0],R1 : B end : .myData: EQUD 0xCAFEBEEF, 0xBABEDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 4u }, { CoreRegister::R2, 0x57FFFFFCu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0x57FFFFFCu }, { 0x801C, 0xBABEDEAD } }
},

{ "STR-PostIndexedRegLsl", "ADR R0,myData : MOV R1,#1 : MVN R2,#&A8000003 : STR R2,[R0],R1,LSL #2 : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 1u }, { CoreRegister::R2, 0x57FFFFFCu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0x57FFFFFCu }, { 0x801C, 0xBEEFDEAD } }
},

{ "STR-PostIndexedRegLsr", "ADR R0,myData : MOV R1,#16 : MVN R2,#&E9000000 : STR R2,[R0],R1,LSR #2 : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0x16FFFFFFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0x16FFFFFFu }, { 0x801C, 0xBEEFDEAD } }
},

{ "STR-PostIndexedRegAsr", "ADR R0,myData : MOV R1,#16 : MVN R2,#&0E900000 : STR R2,[R0],R1,ASR #2 : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xF16FFFFFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xF16FFFFFu }, { 0x801C, 0xBEEFDEAD } }
},

{ "STR-PostIndexedRegRor", "ADR R0,myData : MOV R1,#16 : MVN R2,#&00530000 : STR R2,[R0],R1,ROR #2 : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 16u }, { CoreRegister::R2, 0xFFACFFFFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xFFACFFFFu }, { 0x801C, 0xBEEFDEAD } }
},

{ "STR-PostIndexedRegRrx", "ADR R0,myData : MOV R1,#8 : MVN R2,#&00069000 : STR R2,[R0],R1,RRX : B end : .myData: EQUD 0xCAFEBABE, 0xBEEFDEAD : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 8u }, { CoreRegister::R2, 0xFFF96FFFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8018, 0xFFF96FFFu }, { 0x801C, 0xBEEFDEAD } }
},

{ "STR-PostIndexedByte", "ADR R0,myData + 3 : MOV R1,#&00000990 : STRB R1,[R0],#2 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8019u }, { CoreRegister::R1, 0x990u }, },
    { { 0x8014, 0x90ADBEEF }, { 0x8018, 0xCAFEBABE } }
},

// NOTE: Addresses have bits 0 and 1 forced to 0 for STR instructions (see ARMv2 Data Sheet, page 26).
{ "STR-PostIndexedUnaligned1", "ADR R0,myData + 1 : MVN R1,#&3000000D : STR R1,[R0],#2 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8017u }, { CoreRegister::R1, 0xCFFFFFF2u }, },
    { { 0x8014, 0xCFFFFFF2u }, { 0x8018, 0xCAFEBABE } }
},

{ "STR-PostIndexedUnaligned2", "ADR R0,myData + 2 : MVN R1,#&5000000A : STR R1,[R0],#1 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x8017u }, { CoreRegister::R1, 0xAFFFFFF5u }, },
    { { 0x8014, 0xAFFFFFF5u }, { 0x8018, 0xCAFEBABE } }
},

{ "STR-PostIndexedUnaligned3", "ADR R0,myData + 3 : MVN R1,#&00076000 : STR R1,[R0],#3 : B end : .myData: EQUD 0xDEADBEEF, 0xCAFEBABE : .end",
    { { CoreRegister::R0, 0x801Au }, { CoreRegister::R1, 0xFFF89FFFu }, },
    { { 0x8014, 0xFFF89FFFu }, { 0x8018, 0xCAFEBABE } }
},

{ "STR-PostIndexedAddrException", "MOV R0,#&F0000000 : STR R1,[R0],#3",
    { { CoreRegister::R0, 0xF0000000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u },  },
    { /* No memory requirements */ }
},

{ "STR-PostIndexedDataAbort", "MOV R0,#&00F00000 : STR R1,[R0],#3",
    { { CoreRegister::R0, 0x00F00000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu },  },
    { /* No memory requirements */ }
},

// LDM
{ "LDMIA-SingleReg", "ADR R0,myData : LDMIA R0,{R1} : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-MultiReg", "ADR R0,myData : LDMIA R0,{R1-R3} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::R3, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-PC-Only", "ADR R0,myData : LDMIA R0,{R15} : .myData : EQUD $ + 8 : MOV R1,#1",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-PC+PSR", "ADR R0,myData : LDMIA R0,{R15}^ : .myData : EQUD ($ + 8) OR &F0000000 : MOV R1,#1",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0xF0000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-SingleReg-WriteBack", "ADR R0,myData : LDMIA R0!,{R1} : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-MultiReg-WriteBack", "ADR R0,myData : LDMIA R0!,{R1-R3} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::R3, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-WriteBack-OverBase", "ADR R0,myData : LDMIA R0!,{R0-R2} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0xDEADBEEFu }, { CoreRegister::R1, 0xCAFEBABEu }, { CoreRegister::R2, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-PC-Only-WriteBack", "ADR R0,myData : LDMIA R0!,{R15} : .myData : EQUD $ + 8 : MOV R1,#1",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-PC+PSR-WriteBack", "ADR R0,myData : LDMIA R0!,{R15}^ : .myData : EQUD ($ + 8) OR &F0000000 : MOV R1,#1",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0xF0000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-AddressException", "MOV R0,#&4000000 : LDMIA R0!,{R1-R5}",
    { { CoreRegister::R0, 0x4000000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u }, },
    { /* No memory requirements */ }
},

{ "LDMIA-NoAddressException", "MVN R0,#&FC000003 : LDMIA R0!,{R1-R5}",
    { { CoreRegister::R0, 0x4000010u }, { CoreRegister::R1, 0xE121FF7Fu }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

// Reinstate Data Abort tests when Data Abort conditions in the test system are better understood.
//{ "LDMIA-DataAbort", "MOV R0,#&10000 : LDMIA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x10000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu }, },
//    { /* No memory requirements */ }
//},
//
//{ "LDMIA-NoDataAbort", "MOV R0,#&9000 : SUB R0,R0,#4 : LDMIA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x9010u }, { CoreRegister::R1, 0u }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
//    { /* No memory requirements */ }
//},

{ "LDMIB-SingleReg", "ADR R0,myData - 4 : LDMIB R0,{R1} : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-MultiReg", "ADR R0,myData - 4 : LDMIB R0,{R1-R3} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::R3, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-PC-Only", "ADR R0,myData - 4 : LDMIB R0,{R15} : .myData : EQUD $ + 8 : MOV R1,#1",
    { { CoreRegister::R0, 0x8008u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-PC+PSR", "ADR R0,myData - 4 : LDMIB R0,{R15}^ : .myData : EQUD ($ + 8) OR &F0000000 : MOV R1,#1",
    { { CoreRegister::R0, 0x8008u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0xF0000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-SingleReg-WriteBack", "ADR R0,myData - 4 : LDMIB R0!,{R1} : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-MultiReg-WriteBack", "ADR R0,myData - 4 : LDMIB R0!,{R1-R3} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::R3, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-WriteBack-OverBase", "ADR R0,myData - 4 : LDMIB R0!,{R0-R2} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0xDEADBEEFu }, { CoreRegister::R1, 0xCAFEBABEu }, { CoreRegister::R2, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-PC-Only-WriteBack", "ADR R0,myData - 4 : LDMIB R0!,{R15} : .myData : EQUD $ + 8 : MOV R1,#1",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-PC+PSR-WriteBack", "ADR R0,myData - 4 : LDMIB R0!,{R15}^ : .myData : EQUD ($ + 8) OR &F0000000 : MOV R1,#1",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0xF0000000u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-AddressException", "MOV R0,#&4000000 : SUB R0,R0,#4 : LDMIB R0!,{R1-R5}",
    { { CoreRegister::R0, 0x3FFFFFCu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8014u }, { CoreRegister::PC, 0x20u }, },
    { /* No memory requirements */ }
},

{ "LDMIB-NoAddressException", "MOV R0,#&4000000 : SUB R0,R0,#8 : LDMIB R0!,{R1-R5}",
    { { CoreRegister::R0, 0x400000Cu }, { CoreRegister::R1, 0xE121FF7Fu }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

// Reinstate Data Abort tests when Data Abort conditions in the test system are better understood.
//{ "LDMIB-DataAbort", "MOV R0,#&10000 : LDMIB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x10000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu }, },
//    { /* No memory requirements */ }
//},
//
//{ "LDMIB-NoDataAbort", "MOV R0,#&9000 : SUB R0,R0,#8 : LDMIB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x900Cu }, { CoreRegister::R1, 0u }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
//    { /* No memory requirements */ }
//},



{ "LDMDA-SingleReg", "ADR R0,myData : LDMDA R0,{R1} : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-MultiReg", "ADR R0,myData + 8 : LDMDA R0,{R1-R3} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::R3, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-PC-Only", "ADR R0,myData : LDMDA R0,{R15} : .myData : EQUD $ + 8 : MOV R1,#1",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-PC+PSR", "ADR R0,myData : LDMDA R0,{R15}^ : .myData : EQUD ($ + 8) OR &F0000000 : MOV R1,#1",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0xF0000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-SingleReg-WriteBack", "ADR R0,myData : LDMDA R0!,{R1} : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-MultiReg-WriteBack", "ADR R0,myData + 8 : LDMDA R0!,{R1-R3} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::R3, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-WriteBack-OverBase", "ADR R0,myData + 8 : LDMDA R0!,{R0-R2} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0xDEADBEEFu }, { CoreRegister::R1, 0xCAFEBABEu }, { CoreRegister::R2, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-PC-Only-WriteBack", "ADR R0,myData : LDMDA R0!,{R15} : .myData : EQUD $ + 8 : MOV R1,#1",
    { { CoreRegister::R0, 0x8008u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-PC+PSR-WriteBack", "ADR R0,myData : LDMDA R0!,{R15}^ : .myData : EQUD ($ + 8) OR &F0000000 : MOV R1,#1",
    { { CoreRegister::R0, 0x8008u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0xF0000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDA-AddressException", "MOV R0,#&8000000 : LDMDA R0!,{R1-R5}",
    { { CoreRegister::R0, 0x8000000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u }, },
    { /* No memory requirements */ }
},

// NOTE: Can't decrement from a valid address to a 26-bit address exception-raising address.
//{ "LDMDA-NoAddressException", "MOV R0,#0 : LDMDA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x4000010u }, /* Wrong */ { CoreRegister::R1, 0xE121FF7Fu}, {CoreRegister::R2, 0xDFDFDFDFu}, {CoreRegister::R5, 0xDFDFDFDFu}, {CoreRegister::CPSR, 0x00u},},
//    { /* No memory requirements */ }
//},

// Reinstate Data Abort tests when Data Abort conditions in the test system are better understood.
//{ "LDMDA-DataAbort", "MOV R0,#&10000 : LDMDA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x10000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu }, },
//    { /* No memory requirements */ }
//},
//
//{ "LDMDA-NoDataAbort", "MOV R0,#&4000000 : SUB R0,R0,#&8000 : LDMDA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x3FF7FE0u }, { CoreRegister::R1, 0u }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
//    { /* No memory requirements */ }
//},

{ "LDMDB-SingleReg", "ADR R0,myData + 4 : LDMDB R0,{R1} : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-MultiReg", "ADR R0,myData + 12 : LDMDB R0,{R1-R3} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::R3, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-PC-Only", "ADR R0,myData + 4 : LDMDB R0,{R15} : .myData : EQUD $ + 8 : MOV R1,#1",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-PC+PSR", "ADR R0,myData + 4 : LDMDB R0,{R15}^ : .myData : EQUD ($ + 8) OR &F0000000 : MOV R1,#1",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0xF0000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-SingleReg-WriteBack", "ADR R0,myData + 4 : LDMDB R0!,{R1} : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-MultiReg-WriteBack", "ADR R0,myData + 12 : LDMDB R0!,{R1-R3} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0xDEADBEEFu }, { CoreRegister::R2, 0xCAFEBABEu }, { CoreRegister::R3, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-WriteBack-OverBase", "ADR R0,myData + 12 : LDMDB R0!,{R0-R2} : B end : .myData : EQUD 0xDEADBEEF, 0xCAFEBABE, 0xB00B1E5 : .end",
    { { CoreRegister::R0, 0xDEADBEEFu }, { CoreRegister::R1, 0xCAFEBABEu }, { CoreRegister::R2, 0xB00B1E5u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-PC-Only-WriteBack", "ADR R0,myData + 4 : LDMDB R0!,{R15} : .myData : EQUD $ + 8 : MOV R1,#1",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-PC+PSR-WriteBack", "ADR R0,myData + 4 : LDMDB R0!,{R15}^ : .myData : EQUD ($ + 8) OR &F0000000 : MOV R1,#1",
    { { CoreRegister::R0, 0x800Cu }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0xF0000000u }, },
    { /* No memory requirements */ }
},

{ "LDMDB-AddressException", "MOV R0,#&8000000 : LDMDB R0!,{R1-R5}",
    { { CoreRegister::R0, 0x8000000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u }, },
    { /* No memory requirements */ }
},

// NOTE: Can't decrement from a valid address to a 26-bit address exception-raising address.
//{ "LDMDB-NoAddressException", "MOV R0,#0 : LDMDB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x4000010u }, /* Wrong */ { CoreRegister::R1, 0xE121FF7Fu}, {CoreRegister::R2, 0xDFDFDFDFu}, {CoreRegister::R5, 0xDFDFDFDFu}, {CoreRegister::CPSR, 0x00u},},
//    { /* No memory requirements */ }
//},

// Reinstate Data Abort tests when Data Abort conditions in the test system are better understood.
//{ "LDMDB-DataAbort", "MOV R0,#&10000 : LDMDB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x10000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu }, },
//    { /* No memory requirements */ }
//},
//
//{ "LDMDB-NoDataAbort", "MOV R0,#&4000000 : SUB R0,R0,#&8000 : LDMDB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x3FF7FE0u }, { CoreRegister::R1, 0u }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
//    { /* No memory requirements */ }
//},




// STM
{ "STMIA-SingleReg", "ADR R0,myData : MVN R1,#&F000000F : STMIA R0,{R1} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0x0FFFFFF0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8014, 0x0FFFFFF0 }, },
},

{ "STMIA-MultiReg", "ADR R0,myData : MVN R1,#&F000000F : MOV R2,R1,ROR #4 : MOV R3,R2,ROR #8 : STMIA R0,{R1-R3} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 0x0FFFFFF0u }, { CoreRegister::R2, 0x00FFFFFFu }, { CoreRegister::R3, 0xFF00FFFFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x801C, 0x0FFFFFF0 }, { 0x8020, 0x00FFFFFFu }, { 0x8024, 0xFF00FFFFu }, }
},

{ "STMIA-PC+PSR", "ADR R0,myData : MOVS R1,#0 : STMIA R0,{R15} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::CPSR, 0x40000000u }, },
    { { 0x8014, 0x40008018 }, }
},

{ "STMIA-SingleReg-WriteBack", "ADR R0,myData : MOV R1,#42 : STMIA R0!,{R1} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 42u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8014, 42u }, }
},

{ "STMIA-MultiReg-WriteBack", "ADR R0,myData : MOV R1,#&F000000F : MOV R2,R1,ROR #8 : MOV R3,R2,ROR #8 : STMIA R0!,{R1-R3} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8028u }, { CoreRegister::R1, 0xF000000Fu }, { CoreRegister::R2, 0x0FF00000u }, { CoreRegister::R3, 0x000FF000u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x801C, 0xF000000Fu }, { 0x8020, 0x0FF00000u }, { 0x8024, 0x000FF000u }, }
},

// NOTE: Storing a base register which isn't first in the list and using
// write-back should produce a different result.
// See ARM Family Data Manual Page 2-41.
{ "STMIA-WriteBack-OverBase", "ADR R0,myData : MOV R1,#&F000000F : MOV R2,R1,ROR #8 : STMIA R0!,{R0-R2} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8024 }, { CoreRegister::R1, 0xF000000Fu }, { CoreRegister::R2, 0x0FF00000u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8018, 0x8018u }, { 0x801C, 0xF000000Fu }, { 0x8020, 0x0FF00000u }, }
},

{ "STMIA-AddressException", "MOV R0,#&4000000 : STMIA R0!,{R1-R5}",
    { { CoreRegister::R0, 0x4000000u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u }, },
    { /* No memory requirements */ }
},

{ "STMIA-NoAddressException", "MVN R0,#&FC000003 : SUB R0,R0,#4 : STMIA R0!,{R1-R5}",
    { { CoreRegister::R0, 0x400000Cu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

// Reinstate Data Abort tests when Data Abort conditions in the test system are better understood.
//{ "STMIA-DataAbort", "MOV R0,#&10000 : STMIA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x10000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu }, },
//    { /* No memory requirements */ }
//},
//
//{ "STMIA-NoDataAbort", "MOV R0,#&9000 : SUB R0,R0,#4 : STMIA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x9010u }, { CoreRegister::R1, 0u }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
//    { /* No memory requirements */ }
//},

{ "STMIB-SingleReg", "ADR R0,myData - 4 : MVN R1,#&F000000F : STMIB R0,{R1} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 0x0FFFFFF0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8014, 0x0FFFFFF0 }, },
},

{ "STMIB-MultiReg", "ADR R0,myData - 4 : MVN R1,#&F000000F : MOV R2,R1,ROR #4 : MOV R3,R2,ROR #8 : STMIB R0,{R1-R3} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 0x0FFFFFF0u }, { CoreRegister::R2, 0x00FFFFFFu }, { CoreRegister::R3, 0xFF00FFFFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x801C, 0x0FFFFFF0 }, { 0x8020, 0x00FFFFFFu }, { 0x8024, 0xFF00FFFFu }, }
},

{ "STMIB-PC+PSR", "ADR R0,myData - 4 : MOVS R1,#0 : STMIB R0,{R15} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::CPSR, 0x40000000u }, },
    { { 0x8014, 0x40008018 }, }
},

{ "STMIB-SingleReg-WriteBack", "ADR R0,myData - 4 : MOV R1,#42 : STMIB R0!,{R1} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 42u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8014, 42u }, }
},

{ "STMIB-MultiReg-WriteBack", "ADR R0,myData - 4 : MOV R1,#&F000000F : MOV R2,R1,ROR #8 : MOV R3,R2,ROR #8 : STMIB R0!,{R1-R3} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8024u }, { CoreRegister::R1, 0xF000000Fu }, { CoreRegister::R2, 0x0FF00000u }, { CoreRegister::R3, 0x000FF000u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x801C, 0xF000000Fu }, { 0x8020, 0x0FF00000u }, { 0x8024, 0x000FF000u }, }
},

// NOTE: Storing a base register which isn't first in the list and using
// write-back should produce a different result.
// See ARM Family Data Manual Page 2-41.
{ "STMIB-WriteBack-OverBase", "ADR R0,myData - 4 : MOV R1,#&F000000F : MOV R2,R1,ROR #8 : STMIB R0!,{R0-R2} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8020 }, { CoreRegister::R1, 0xF000000Fu }, { CoreRegister::R2, 0x0FF00000u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8018, 0x8014u }, { 0x801C, 0xF000000Fu }, { 0x8020, 0x0FF00000u }, }
},

{ "STMIB-AddressException", "MVN R0,#&FC000003 : STMIB R0!,{R1-R5}",
    { { CoreRegister::R0, 0x3FFFFFCu }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x20u }, },
    { /* No memory requirements */ }
},

{ "STMIB-NoAddressException", "MOV R0,#&4000000 : SUB R0,R0,#8 : STMIB R0!,{R1-R5}",
    { { CoreRegister::R0, 0x400000Cu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

// Reinstate Data Abort tests when Data Abort conditions in the test system are better understood.
//{ "STMIB-DataAbort", "MOV R0,#&10000 : STMIB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x10000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu }, },
//    { /* No memory requirements */ }
//},
//
//{ "STMIB-NoDataAbort", "MOV R0,#&9000 : SUB R0,R0,#4 : STMIB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x9010u }, { CoreRegister::R1, 0u }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
//    { /* No memory requirements */ }
//},

{ "STMDA-SingleReg", "ADR R0,myData : MVN R1,#&F000000F : STMDA R0,{R1} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 0x0FFFFFF0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8014, 0x0FFFFFF0 }, },
},

{ "STMDA-MultiReg", "ADR R0,myData + 8 : MVN R1,#&F000000F : MOV R2,R1,ROR #4 : MOV R3,R2,ROR #8 : STMDA R0,{R1-R3} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8024u }, { CoreRegister::R1, 0x0FFFFFF0u }, { CoreRegister::R2, 0x00FFFFFFu }, { CoreRegister::R3, 0xFF00FFFFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x801C, 0x0FFFFFF0 }, { 0x8020, 0x00FFFFFFu }, { 0x8024, 0xFF00FFFFu }, }
},

{ "STMDA-PC+PSR", "ADR R0,myData : MOVS R1,#0 : STMDA R0,{R15} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::CPSR, 0x40000000u }, },
    { { 0x8014, 0x40008018 }, }
},

{ "STMDA-SingleReg-WriteBack", "ADR R0,myData : MOV R1,#42 : STMDA R0!,{R1} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8010u }, { CoreRegister::R1, 42u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8014, 42u }, }
},

{ "STMDA-MultiReg-WriteBack", "ADR R0,myData + 8 : MOV R1,#&F000000F : MOV R2,R1,ROR #8 : MOV R3,R2,ROR #8 : STMDA R0!,{R1-R3} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 0xF000000Fu }, { CoreRegister::R2, 0x0FF00000u }, { CoreRegister::R3, 0x000FF000u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x801C, 0xF000000Fu }, { 0x8020, 0x0FF00000u }, { 0x8024, 0x000FF000u }, }
},

// NOTE: Storing a base register which isn't first in the list and using
// write-back should produce a different result.
// See ARM Family Data Manual Page 2-41.
{ "STMDA-WriteBack-OverBase", "ADR R0,myData + 8 : MOV R1,#&F000000F : MOV R2,R1,ROR #8 : STMDA R0!,{R0-R2} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8014 }, { CoreRegister::R1, 0xF000000Fu }, { CoreRegister::R2, 0x0FF00000u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8018, 0x8020u }, { 0x801C, 0xF000000Fu }, { 0x8020, 0x0FF00000u }, }
},

{ "STMDA-AddressException", "MOV R0,#&4000000 : ADD R0,R0,#20 : STMDA R0!,{R1-R5}",
    { { CoreRegister::R0, 0x4000014u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8014u }, { CoreRegister::PC, 0x20u }, },
    { /* No memory requirements */ }
},

{ "STMDA-NoAddressException", "MOV R0,#&4000000 : ADD R0,R0,#12 : STMDA R0!,{R1-R5}",
    { { CoreRegister::R0, 0x3FFFFF8u }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

// Reinstate Data Abort tests when Data Abort conditions in the test system are better understood.
//{ "STMDA-DataAbort", "MOV R0,#&10000 : STMDA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x10000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu }, },
//    { /* No memory requirements */ }
//},
//
//{ "STMDA-NoDataAbort", "MOV R0,#&9000 : SUB R0,R0,#4 : STMDA R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x9010u }, { CoreRegister::R1, 0u }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
//    { /* No memory requirements */ }
//},

{ "STMDB-SingleReg", "ADR R0,myData + 4 : MVN R1,#&F000000F : STMDB R0,{R1} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::R1, 0x0FFFFFF0u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8014, 0x0FFFFFF0 }, },
},

{ "STMDB-MultiReg", "ADR R0,myData + 12 : MVN R1,#&F000000F : MOV R2,R1,ROR #4 : MOV R3,R2,ROR #8 : STMDB R0,{R1-R3} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8028u }, { CoreRegister::R1, 0x0FFFFFF0u }, { CoreRegister::R2, 0x00FFFFFFu }, { CoreRegister::R3, 0xFF00FFFFu }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x801C, 0x0FFFFFF0 }, { 0x8020, 0x00FFFFFFu }, { 0x8024, 0xFF00FFFFu }, }
},

{ "STMDB-PC+PSR", "ADR R0,myData + 4: MOVS R1,#0 : STMDB R0,{R15} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8018u }, { CoreRegister::CPSR, 0x40000000u }, },
    { { 0x8014, 0x40008018 }, }
},

{ "STMDB-SingleReg-WriteBack", "ADR R0,myData + 4: MOV R1,#42 : STMDB R0!,{R1} : B end : .myData : EQUD 0 : .end",
    { { CoreRegister::R0, 0x8014u }, { CoreRegister::R1, 42u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8014, 42u }, }
},

{ "STMDB-MultiReg-WriteBack", "ADR R0,myData + 12 : MOV R1,#&F000000F : MOV R2,R1,ROR #8 : MOV R3,R2,ROR #8 : STMDB R0!,{R1-R3} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x801Cu }, { CoreRegister::R1, 0xF000000Fu }, { CoreRegister::R2, 0x0FF00000u }, { CoreRegister::R3, 0x000FF000u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x801C, 0xF000000Fu }, { 0x8020, 0x0FF00000u }, { 0x8024, 0x000FF000u }, }
},

// NOTE: Storing a base register which isn't first in the list and using
// write-back should produce a different result.
// See ARM Family Data Manual Page 2-41.
{ "STMDB-WriteBack-OverBase", "ADR R0,myData + 12 : MOV R1,#&F000000F : MOV R2,R1,ROR #8 : STMDB R0!,{R0-R2} : B end : .myData : EQUD 0, 0, 0 : .end",
    { { CoreRegister::R0, 0x8018 }, { CoreRegister::R1, 0xF000000Fu }, { CoreRegister::R2, 0x0FF00000u }, { CoreRegister::CPSR, 0x00000000u }, },
    { { 0x8018, 0x8024u }, { 0x801C, 0xF000000Fu }, { 0x8020, 0x0FF00000u }, }
},

{ "STMDB-AddressException", "MOV R0,#&4000000 : ADD R0,R0,#20 : STMDB R0!,{R1-R5}",
    { { CoreRegister::R0, 0x4000014u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8014u }, { CoreRegister::PC, 0x20u }, },
    { /* No memory requirements */ }
},

{ "STMDB-NoAddressException", "MOV R0,#&4000000 : ADD R0,R0,#16 : STMDB R0!,{R1-R5}",
    { { CoreRegister::R0, 0x3FFFFFCu }, { CoreRegister::CPSR, 0x00u }, },
    { /* No memory requirements */ }
},

// Reinstate Data Abort tests when Data Abort conditions in the test system are better understood.
//{ "STMDB-DataAbort", "MOV R0,#&10000 : STMDB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x10000u }, { CoreRegister::R1, 0u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8010u }, { CoreRegister::PC, 0x1Cu }, },
//    { /* No memory requirements */ }
//},
//
//{ "STMDB-NoDataAbort", "MOV R0,#&9000 : SUB R0,R0,#4 : STMDB R0!,{R1-R5}",
//    { { CoreRegister::R0, 0x9010u }, { CoreRegister::R1, 0u }, { CoreRegister::R2, 0xDFDFDFDFu }, { CoreRegister::R5, 0xDFDFDFDFu }, { CoreRegister::CPSR, 0x00u }, },
//    { /* No memory requirements */ }
//},

// SWP
{ "SWP-Word", "MOV R0,#&5000000A : ADR R1,myData : SWP R2,R0,[R1] : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x8014u },{ CoreRegister::R2, 0xDEADBEEFu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8014, 0x5000000Au } },
},

{ "SWP-Word-Unaligned-1", "MOV R0,#&5000000A : ADR R1,myData + 1 : SWP R2,R0,[R1] : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x8015u },{ CoreRegister::R2, 0xEFDEADBEu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8014, 0x5000000Au } },
},

{ "SWP-Word-Unaligned-2", "MOV R0,#&5000000A : ADR R1,myData + 2 : SWP R2,R0,[R1] : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x8016u },{ CoreRegister::R2, 0xBEEFDEADu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8014, 0x5000000Au } },
},

{ "SWP-Word-Unaligned-3", "MOV R0,#&5000000A : ADR R1,myData + 3 : SWP R2,R0,[R1] : B end : .myData : EQUD 0xDEADBEEF : .end",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x8017u },{ CoreRegister::R2, 0xADBEEFDEu }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8014, 0x5000000Au } },
},

{ "SWP-Word-AddressException", "MOV R0,#&5000000A : MOV R1,#&4000000 : SWP R2,R0,[R1]",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x4000000u },{ CoreRegister::R2, 0x00u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8014u }, { CoreRegister::PC, 0x20u },  },
    { /* No memory requirements */ },
},

{ "SWP-Byte", "MOV R0,#&5000000A : ADR R1,myData : SWPB R2,R0,[R1] : B end : .myData : EQUD 0x44332211 : .end",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x8014u },{ CoreRegister::R2, 0x00000011u }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8014, 0x4433220Au } },
},

{ "SWP-Byte-Unaligned-1", "MOV R0,#&5000000A : ADR R1,myData + 1 : SWPB R2,R0,[R1] : B end : .myData : EQUD 0x44332211 : .end",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x8015u },{ CoreRegister::R2, 0x00000022u }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8014, 0x44330A11u } },
},

{ "SWP-Byte-Unaligned-2", "MOV R0,#&5000000A : ADR R1,myData + 2 : SWPB R2,R0,[R1] : B end : .myData : EQUD 0x44332211 : .end",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x8016u },{ CoreRegister::R2, 0x00000033u }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8014, 0x440A2211u } },
},

{ "SWP-Byte-Unaligned-3", "MOV R0,#&5000000A : ADR R1,myData + 3 : SWPB R2,R0,[R1] : B end : .myData : EQUD 0x44332211 : .end",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x8017u },{ CoreRegister::R2, 0x00000044u }, { CoreRegister::CPSR, 0x00u }, },
    { { 0x8014, 0x0A332211u } },
},

{ "SWP-Byte-AddressException", "MOV R0,#&5000000A : MOV R1,#&4000000 : SWPB R2,R0,[R1]",
    { { CoreRegister::R0, 0x5000000Au }, { CoreRegister::R1, 0x4000000u },{ CoreRegister::R2, 0x00u }, { CoreRegister::CPSR, 0x08000003u }, { CoreRegister::R14, 0x8014u }, { CoreRegister::PC, 0x20u },  },
    { /* No memory requirements */ },
},

};

} // Anonymous

INSTANTIATE_TEST_SUITE_P(CPU, DataTransfer, testing::ValuesIn(testPoints));

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


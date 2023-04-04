//! @file Test_SimpleInstructions.cpp
//! @brief The definition of unit tests to verify assembling simple
//! instruction types.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "TestTools.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which specifies source code and the machine code it creates.
class PositiveAssemblyTestPoint : public BaseTestPoint
{
private:
    String _source;
    std::vector<uint32_t> _results;
public:
    // Construction
    PositiveAssemblyTestPoint(const TestLocation &loc, const char *name, const char *source,
                              uint32_t results) :
        BaseTestPoint(loc, name),
        _source(source),
        _results({ results })
    {
    }

    PositiveAssemblyTestPoint(const TestLocation &loc, const char *name, const char *source,
                              const std::initializer_list<uint32_t> &results) :
        BaseTestPoint(loc, name),
        _source(source),
        _results(results)
    {
    }

    ~PositiveAssemblyTestPoint() = default;

    // Accessors
    string_cref_t getSource() const { return _source; }

    // Operations
    ::testing::AssertionResult isSuccessful(const Messages &log) const
    {
        if (log.hasErrors())
        {
            auto result = testing::AssertionFailure();

            result << "Assembly failed unexpectedly\n";
            appendLog(result, log);

            return result;
        }
        else
        {
            return testing::AssertionSuccess();
        }
    }

    const std::vector<uint32_t> &getResults() const { return _results; }
};

//! @brief An object which specifies source code expected to fail assembly.
class NegativeAssemblyTestPoint : public BaseTestPoint
{
private:
    String _source;
    String _messageFragment;
    int _errorLine;
public:
    // Construction
    NegativeAssemblyTestPoint(const TestLocation &loc, const char *name, const char *source,
                              const char *messageFrag, int line) :
        BaseTestPoint(loc, name),
        _source(source),
        _messageFragment(messageFrag),
        _errorLine(line)
    {
    }

    ~NegativeAssemblyTestPoint() = default;

    // Accessors
    string_cref_t getSource() const { return _source; }

    // Operations
    ::testing::AssertionResult hasExpectedError(const Messages &log) const
    {
        bool hasMatch = false;

        for (const auto &entry : log.getMessages())
        {
            if ((entry.getLocation().LineNo == _errorLine) &&
                entry.getMessage().contains(_messageFragment))
            {
                hasMatch = true;
                break;
            }
        }

        if (hasMatch)
        {
            return ::testing::AssertionSuccess();
        }
        else
        {
            auto failure = ::testing::AssertionFailure();
            failure << "Expected failure not found.";

            if (log.isEmpty() == false)
            {
                failure << '\n';
                appendLog(failure, log);
            }

            return failure;
        }
    }
};

//! @brief A basic fixture which assembles code for testing.
class AssemblyFixture : public BaseTestFixture
{
public:
    // Construction/Destruction
    AssemblyFixture() :
        BaseTestFixture("Assembly")
    {
    }

protected:
    // Accessors
    virtual const Options &getAssemblyOptions() const
    {
        static std::unique_ptr<Options> defaultOpts;

        if (!defaultOpts)
        {
            defaultOpts = std::make_unique<Options>();
            defaultOpts->setInstructionSet(InstructionSet::ArmV7);
        }

        return *defaultOpts;
    }
};

//! @brief A fixture which should successfully assemble code.
class PositiveAssemblyTest : public BaseTestFixture
{
private:
    // Internal Fields
    PositiveAssemblyTestPoint _point;
    Options _options;
public:
    // Construction/Destruction
    explicit PositiveAssemblyTest(const PositiveAssemblyTestPoint &point) :
        BaseTestFixture("AssemblyTests"),
        _point(point)
    {
        // Select the most restrictive instruction so that any advanced
        // instructions need to be specifically allowed.
        _options.setInstructionSet(InstructionSet::ArmV2);
        _options.setLoadAddress(0x10000);
    }

    virtual void TestBody() override
    {
        Messages log;
        ObjectCode code = assembleText(_point.getSource(), _options, log);

        EXPECT_TRUE(_point.isSuccessful(log));
        ASSERT_EQ(code.getCodeSize(), _point.getResults().size() * 4u);

        const uint32_t *words = reinterpret_cast<const uint32_t *>(code.getCode());
        const auto &expected = _point.getResults();

        for (size_t i = 0, c = _point.getResults().size(); i < c; ++i)
        {
            EXPECT_TRUE(equalHex(expected[i], words[i]));
        }
    }
};

//! @brief A fixture in which code should fail to assemble.
class NegativeAssemblyTest : public BaseTestFixture
{
private:
    // Internal Fields
    NegativeAssemblyTestPoint _point;
    Options _options;

public:
    // Construction/Destruction
    explicit NegativeAssemblyTest(const NegativeAssemblyTestPoint &point) :
        BaseTestFixture("AssemblyTests"),
        _point(point)
    {
        // Select the most restrictive instruction so that any advanced
        // instructions need to be specifically allowed.
        _options.setInstructionSet(InstructionSet::ArmV2);
    }

    virtual void TestBody() override
    {
        Messages log;
        ObjectCode code = assembleText(_point.getSource(),
                                       _options, log);

        EXPECT_TRUE(_point.hasExpectedError(log));
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////

} // TED

void registerAssemblyTests()
{
    PositiveAssemblyTestPoint positivePoints[] = {
        { LOC, "Swi", "SWI 0xFF", { 0xEF0000FF } },
        { LOC, "Swi_Eq", "SWIEQ 0xCAFEBE", { 0x0FCAFEBE } },
        { LOC, "Swi_Expr", "SWIMI 123 + $ - 9", { 0x4F010072u } },

        { LOC, "Branch", "B $ + &CafeB8", { 0xEA32BFACu } },
        { LOC, "Branch_Min", "B $ + 8", { 0xEA000000 } },
        { LOC, "Branch_Link","BL $ + 32", { 0xEB000006 } },
        { LOC, "Branch_Cond", "BLS $ - 16", { 0x9AFFFFFA } } ,
        { LOC, "Branch_Link_Cond", "BLLS $ - 96", { 0x9BFFFFE6 } },
        { LOC, "Branch_Expr", "BLLT 121 + $ - 9", { 0xBB00001Au } },

        { LOC, "Mul_Simple", "MUL R1, R2, R3", 0xE0010392 },
        { LOC, "Mul_Condition", "MULLT R13, R14, R15", 0xB00D0F9E },
        { LOC, "Mul_Psr", "MULS R4, R5, R6", 0xE0140695 },

        { LOC, "Mla_Simple", "MLA R1, R2, R3, R4", 0xE0214392 },
        { LOC, "Mla_Condition", "MLAPL R13, R14, R15, R0", 0x502D0F9E },
        { LOC, "Mla_Psr", "MLAS R7, R8, R9, R10", 0xE037A998 },

        { LOC, "Umull_Simple", "%ARMv4: UMULL R1, R2, R3, R4", 0xE0821493 },
        { LOC, "Umull_Condition", "%ARMv4: UMULLPL R13, R14, R15, R0", 0x508ED09F },
        { LOC, "Umull_Psr", "%ARMv4: UMULLS R7, R8, R9, R10", 0xE0987A99 },

        { LOC, "Umlal_Simple", "%ARMv4: UMLAL R1, R2, R3, R4", 0xE0A21493 },
        { LOC, "Umlal_Condition", "%ARMv4: UMLALPL R13, R14, R15, R0", 0x50AED09F },
        { LOC, "Umlal_Psr", "%ARMv4: UMLALS R7, R8, R9, R10", 0xE0B87A99 },

        { LOC, "Smull_Simple", "%ARMv4: SMULL R1, R2, R3, R4", 0xE0C21493 },
        { LOC, "Smull_Condition", "%ARMv4: SMULLPL R13, R14, R15, R0", 0x50CED09F },
        { LOC, "Smull_Psr", "%ARMv4: SMULLS R7, R8, R9, R10", 0xE0D87A99 },

        { LOC, "Smlal_Simple", "%ARMv4: SMLAL R1, R2, R3, R4", 0xE0E21493 },
        { LOC, "Smlal_Condition", "%ARMv4: SMLALPL R13, R14, R15, R0", 0x50EED09F },
        { LOC, "Smlal_Psr", "%ARMv4: SMLALS R7, R8, R9, R10", 0xE0F87A99 },

        { LOC, "Add_Immediate", "ADD R1, R2, #3", 0xE2821003 },
        { LOC, "And_Register", "AND R1, R2, R3", 0xE0021003 },
        { LOC, "Adc_LSL_Register", "ADCNES R9, R10, R11, LSL R12", 0x10BA9C1B },
        { LOC, "Sub_LSR_Immediate", "SUBCS R4, R5, R6, LSR #7", 0x204543A6 },
        { LOC, "Rsb_ASR_Immediate", "RSBHSS R9, R10, R11, ASR #12", 0x207A964B },
        { LOC, "Rsc_ROR_Immediate", "RSCS R14, R15, R0, ROR #1", 0xE0FFE0E0 },
        { LOC, "Bic_RRX", "BICVSS R9, R10, R11, RRX", 0x61DA906B },
        { LOC, "Eor_Register", "EOR R15, R0, R1", 0xE020F001 },
        { LOC, "Sub_Register_Prs", "SUBS R9, R10, R11", 0xE05A900B },
        { LOC, "Mov_Register", "MOV R6, R7", 0xE1A06007 },
        { LOC, "Move_ProbRegister", "MOV R6, R3", 0xE1A06003 },
        { LOC, "Mvn_Register_Psr", "MVNS R2, R3", 0xE1F02003 },
        { LOC, "Cmp_Register", "CMP R5, R6", 0xE1550006 },
        { LOC, "Teq_Register_Psr", "%26Bit: TEQP R13, R14", 0xE13DF00E },
        { LOC, "Mov_SmallImmediate", "MOV R6, #7", 0xE3A06007 },
        { LOC, "Mov_BigImmediate", "MOV R4, #&F00000", 0xE3A0460F },
        { LOC, "Mov_SplitImmediate", "MOV R9, #&C0000003", 0xE3A0910F },


        { LOC, "Ldr_PreImmed", "LDR R0, [R1, #8]", 0xE5910008 },
        { LOC, "Str_PreReg", "STREQ R9, [R10, R11]", 0x078A900B },
        { LOC, "Ldr_PreLsl", "LDRB R4, [R5, R6, LSL #7]",0xE7D54386 },
        { LOC, "Str_PreLsr", "STRHI R1, [R2, R3, LSR #4]", 0x87821223 },
        { LOC, "Ldr_PreAsr", "LDRNE R7, [R8, R9, ASR #10]", 0x17987549 },
        { LOC, "Str_PerRor", "STRVC R14, [R15, R0, ROR #1]", 0x778FE0E0 },
        { LOC, "Ldr_PreRrx", "LDR R10, [R11, R12, RRX]", 0xE79BA06C },
        { LOC, "Str_PreWriteBack", "STRB R2, [R3, #16]!", 0xE5E32010 },
        { LOC, "Ldr_NoOffset", "LDRVS R9, [R10]", 0x659A9000 },
        { LOC, "Str_PostImmed", "STR R3, [R4], #20", 0xE4843014 },
        { LOC, "Ldr_PostReg", "LDR R5, [R6], R7", 0xE6965007 },
        { LOC, "Str_PostLsl", "STRT R6, [R7], R8, LSL #9", 0xE6A76488 },
        { LOC, "Ldr_PostLsr", "LDRB R0, [R1], R2, LSR #3", 0xE6D101A2 },
        { LOC, "Str_PostAsr", "STRBT R3, [R4], R5, ASR #6", 0xE6E43345 },
        { LOC, "Ldr_PostRor", "LDRNV R2, [R3], R4, ROR #5", 0xF69322E4 },
        { LOC, "Str_PostRrx", "STRLO R1, [R2], R3, RRX", 0x36821063 },

        { LOC, "Ldr_PreNegImmed", "LDR R0, [R1, #-8]", 0xE5110008 },
        { LOC, "Str_PreNegReg", "STREQ R9, [R10, -R11]", 0x070A900B },
        { LOC, "Ldr_PostNegReg", "LDR R5, [R6], -R7", 0xE6165007 },
        { LOC, "Str_PostNegImmed", "STR R3, [R4], #-20", 0xE4043014 },


        { LOC, "Str_PreImmedH",  "%ARMv4: STRH  R0, [R1, #-8]", 0xE14100B8 },
        { LOC, "Ldr_PreImmedH",  "%ARMv4: LDRH  R0, [R1, #8]", 0xE1D100B8 },
        { LOC, "Ldr_PreImmedSB", "%ARMv4: LDRSB R0, [R1, #8]", 0xE1D100D8 },
        { LOC, "Ldr_PreImmedSH", "%ARMv4: LDRSH R0, [R1, #8]", 0xE1D100F8 },
        { LOC, "Str_PreRegH", "%ARMv4: STREQH R9, [R10, R11]", 0x018A90BB },
        { LOC, "Ldr_PreRegH", "%ARMv4: LDREQH R9, [R10, -R11]", 0x011A90BB },
        { LOC, "Ldr_PreRegSH", "%ARMv4: LDREQSH R9, [R10, R11]", 0x019A90FB },
        { LOC, "Ldr_PreRegSB", "%ARMv4: LDREQSB R9, [R10, R11]", 0x019A90DB },
        { LOC, "Str_PreWriteBackH", "%ARMv4: STRH R2, [R3, #24]!", 0xE1E321B8 },
        { LOC, "Ldr_PreWriteBackH", "%ARMv4: LDRH R2, [R3, #-24]!", 0xE17321B8 },
        { LOC, "Ldr_PreWriteBackSB", "%ARMv4: LDRSB R2, [R3, #24]!", 0xE1F321D8 },
        { LOC, "Ldr_PreWriteBackSH", "%ARMv4: LDRSH R2, [R3, #24]!", 0xE1F321F8 },
        { LOC, "Str_NoOffsetH", "%ARMv4: STRVSH R9, [R10]", 0x61CA90B0 },
        { LOC, "Ldr_NoOffsetH", "%ARMv4: LDRVSH R9, [R10]", 0x61DA90B0 },
        { LOC, "Ldr_NoOffsetSB", "%ARMv4: LDRVSSB R9, [R10]", 0x61DA90D0 },
        { LOC, "Ldr_NoOffsetSH", "%ARMv4: LDRVSSH R9, [R10]", 0x61DA90F0 },
        { LOC, "Str_PostImmedH", "%ARMv4: STRH R3, [R4], #-20", 0xE04431B4 },
        { LOC, "Ldr_PostImmedH", "%ARMv4: LDRH R3, [R4], #20", 0xE0D431B4 },
        { LOC, "Ldr_PostImmedSB", "%ARMv4: LDRSB R3, [R4], #20", 0xE0D431D4 },
        { LOC, "Ldr_PostImmedSH", "%ARMv4: LDRSH R3, [R4], #20", 0xE0D431F4 },
        { LOC, "Str_PostRegH", "%ARMv4: STRH R5, [R6], R7", 0xE08650B7 },
        { LOC, "Ldr_PostRegH", "%ARMv4: LDRH R5, [R6], -R7", 0xE01650B7 },
        { LOC, "Ldr_PostRegSB", "%ARMv4: LDRSB R5, [R6], R7", 0xE09650D7 },
        { LOC, "Ldr_PostRegSH", "%ARMv4: LDRSH R5, [R6], R7", 0xE09650F7 },

        { LOC, "Ldm_IA", "LDMIA R0, {R1}", 0xE8900002u },
        { LOC, "Stm_IB", "STMNEIB R6, {R0-R4}", 0x1986001Fu },
        { LOC, "Ldm_DA", "LDMCCDA R8, {R10, R12-PC}", 0x3818F400u },
        { LOC, "Stm_DB", "STMVSDB R1, {R0, R2-R7}", 0x690100FDu },
        { LOC, "Ldm_FA", "LDMFA R13, {R8-R11}", 0xE81D0F00u },
        { LOC, "Stm_FA", "STMFA SP!, {R0-R4}", 0xE9AD001Fu },
        { LOC, "Ldm_FD", "LDMFD R13!, {R8-R11}", 0xE8BD0F00u },
        { LOC, "Stm_FD", "STMFD R13, {R0-R4}^", 0xE94D001Fu },
        { LOC, "Ldm_EA", "LDMEA R13!, {R8-R11}", 0xE93D0F00u },
        { LOC, "Stm_EA", "STMEA R13, {R0-R4}", 0xE88D001Fu },
        { LOC, "Ldm_ED", "LDMED sp, {R8-R11}", 0xE99D0F00u },
        { LOC, "Stm_ED", "STMED R13!, {R0-R4}^", 0xE86D001Fu },

        { LOC, "Adr_Forward", "ADR R1, &10010", 0xE28F1008u },
        { LOC, "Adr_Backwards", "ADR R4, &FFF0", 0xE24F4018u },

        { LOC, "Bkpt_Simple", "BKPT &DEAD", 0xE12DEA7Du },
        { LOC, "Bkpt_Zero", "BKPT 0", 0xE1200070u },
        { LOC, "Bkpt_Condition", "BKPTAL 42", 0xE120027Au },

        { LOC, "Mrs_CPSR", "%ARMv3: MRS R9,CPSR",   0xE10F9000u },
        { LOC, "Mrs_SPSR", "%ARMv3: MRSCC R4,SPSR", 0x314F4000u },

        { LOC, "Msr_RegCPSR", "%ARMv3: MSR CPSR,R11", 0xE12FF00Bu },
        { LOC, "Msr_RegSPSR", "%ARMv3: MSR SPSR,R4",  0xE16FF004u },
        { LOC, "Msr_RegCPSRAll", "%ARMv3: MSR CPSR_all,R11", 0xE12FF00Bu },
        { LOC, "Msr_RegSPSRAll", "%ARMv3: MSR SPSR_all,R4",  0xE16FF004u },
        { LOC, "Msr_RegCPSRFlg", "%ARMv3: MSR CPSR_FLG,R11", 0xE128F00Bu },
        { LOC, "Msr_RegSPSRFlg", "%ARMv3: MSR SPSR_FLG,R11", 0xE168F00Bu },
        { LOC, "Msr_RegCPSRCtl", "%ARMv3: MSR CPSR_Ctl,R11", 0xE121F00Bu },
        { LOC, "Msr_RegSPSRCtl", "%ARMv3: MSR SPSR_ctl,R11", 0xE161F00Bu },
        { LOC, "Msr_RegCPSRSuffix", "%ARMv3: MSR CPSR_sc,R11", 0xE125F00Bu },
        { LOC, "Msr_RegSPSRSuffix", "%ARMv3: MSR SPSR_xf,R11", 0xE16AF00Bu },

        { LOC, "Msr_ImmedCPSR", "%ARMv3: MSR CPSR,#&F0000000",  0xE32FF20Fu },
        { LOC, "Msr_ImmedSPSR", "%ARMv3: MSR SPSR,#&0F000000",  0xE36FF40Fu },
        { LOC, "Msr_ImmedCPSRAll", "%ARMv3: MSR CPSR_all,#&F0000000",  0xE32FF20Fu },
        { LOC, "Msr_ImmedSPSRAll", "%ARMv3: MSR SPSR_all,#&0F000000",  0xE36FF40Fu },
        { LOC, "Msr_ImmedCPSRFlg", "%ARMv3: MSR CPSR_Flg,#&F0000000",  0xE328F20Fu },
        { LOC, "Msr_ImmedSPSRFlg", "%ARMv3: MSR SPSR_FLG,#&0F000000",  0xE368F40Fu },
        { LOC, "Msr_ImmedCPSRCtl", "%ARMv3: MSR CPSR_ctl,#&0F",  0xE321F00Fu },
        { LOC, "Msr_ImmedSPSRCtl", "%ARMv3: MSR SPSR_CTL,#&F0",  0xE361F0F0u },
        { LOC, "Msr_ImmedCPSRcx", "%ARMv3: MSR CPSR_cx,#&A50",  0xE323FEA5u },
        { LOC, "Msr_ImmedSPSRfs", "%ARMv3: MSR SPSR_fs,#&05A00000",  0xE36CF65Au },

        { LOC, "Cdp_Simple", "CDP CP12,9,CR1,cr2,cR3,2",  0xEE921C43u },
        { LOC, "Cdp_Condition", "CDPLT CP0,0,CR9,cr14,Cr13,7",  0xBE0E90EDu },

        { LOC, "Mcr_Simple", "MCR CP12,7,SP,CR2,CR3,1",  0xEEE2DC33u },
        { LOC, "Mcr_Condition", "MCRVS CP9,3,R7,CR10,CR4,6",  0x6E6A79D4u },

        { LOC, "Mrc_Simple", "mrc CP12,7,SP,CR2,CR3,1",  0xEEF2DC33u },
        { LOC, "Mrc_Condition", "MrCHI CP9,3,R7,CR10,CR4,6",  0x8E7A79D4u },

        { LOC, "Stc_NoOffset", "STC CP12,CR0,[R1]", 0xED810C00 },
        { LOC, "Ldc_NoOffset", "LDCL CP9,CR4,[R7]", 0xEDD74900 },
        { LOC, "Stc_PreOffset", "STCL CP2,CR0,[R1,#12]", 0xEDC10203 },
        { LOC, "Ldc_PreOffset", "LDC CP11,CR4,[R7,#96]", 0xED974B18 },
        { LOC, "Stc_PreNegOffset", "STCL CP2,CR0,[R1,#-12]", 0xED410203 },
        { LOC, "Ldc_PreNegOffset", "LDC CP11,CR4,[R7,#-96]", 0xED174B18 },
        { LOC, "Stc_PreWriteBack", "STCEQ CP2,CR0,[R1,#8]!", 0x0DA10202 },
        { LOC, "Ldc_PreWriteBack", "LDCGT CP11,CR4,[R7,#52]!", 0xCDB74B0D },
        { LOC, "Stc_PostOffset", "STCL CP2,CR0,[R1],#12", 0xECC10203 },
        { LOC, "Ldc_PostOffset", "LDC CP11,CR4,[R7],#96", 0xEC974B18 },
        { LOC, "Stc_PostNegOffset", "STCLTL CP2,CR0,[R1],#-12",  0xBC410203 },
        { LOC, "Ldc_PostNegOffset", "LDCLSL CP11,CR4,[R7],#-96", 0x9C574B18 },
    };

    registerTestPoints<PositiveAssemblyTest>("AssemblyTests",
                                             positivePoints,
                                             arraySize(positivePoints));

    NegativeAssemblyTestPoint negativePoints[] = {
        { LOC, "SWI_BadExpr", "SWIMI \"Hello!\"", "incompatible data type", 1 },
        { LOC, "SWI_TooBig", "SWI 0xCAFEBABE", "too big", 1 },
        { LOC, "Branch_BadExpr", "BLE \"Hello!\"", "incompatible data type", 1 },
        { LOC, "Branch_OffsetTooHigh", "BL $ + 1024 * 1024 * 33", "beyond the range", 1 },
        { LOC, "Branch_OffsetTooLow", "BL $ + (1024 * 1024 * -33)", "beyond the range", 1 },
        { LOC, "Bkpt_CommentTooBig", "BKPT 0xDEADBEEF", "too big", 1 },
        { LOC, "Bkpt_Condition", "BKPTVS 1", "cannot be conditionally executed", 1 },
        { LOC, "Bkpt_NoComment", "BKPTVS", "comment expression", 1 },
        { LOC, "Ldr_HalfWordArmV3", "%ARMv3: LDRH  R0, [R1, #8]", "instruction set", 1 },
        { LOC, "Str_HalfWordNotSupported", "%ARMv4: STRSH R0, [R1, #8]", "not support", 1 },
        { LOC, "Str_SignedByteNotSupported", "%ARMv4: STRSB R0, [R1, #8]", "not support", 1 },
        { LOC, "Ldr_BadSignedImmediate", "%ARMv4: LDRSH R0, [R1, #260]", "immediate offset", 1 },
        { LOC, "Msr_RegSPSRRepeatedSuffix", "%ARMv3: MSR SPSR_xfx,R11", "invalid suffix", 1 },
        { LOC, "Cdp_OpCode1OutOfRange", "CDPCS cp0,42,CR9,cr14,Cr13,7", "first op-code", 1 },
        { LOC, "Cdp_OpCode2OutOfRange", "CDPPL cP12,0,CR2,cr0,Cr11,9", "second op-code", 1 },
        { LOC, "Mcr_OpCode1OutOfRange", "MCR CP12,8,SP,CR2,CR3,1", "first op-code", 1 },
        { LOC, "Mcr_OpCode2OutOfRange", "MCRVS CP9,3,R7,CR10,CR4,8", "second op-code", 1 },
        { LOC, "Mrc_OpCode1OutOfRange", "mrc CP12,11,SP,CR2,CR3,1", "first op-code", 1 },
        { LOC, "Mrc_OpCode2OutOfRange", "MrCHI CP9,3,R7,CR10,CR4,69", "second op-code", 1 },
    };

    registerTestPoints<NegativeAssemblyTest>("AssemblyTests",
                                             negativePoints,
                                             arraySize(negativePoints));
}

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////


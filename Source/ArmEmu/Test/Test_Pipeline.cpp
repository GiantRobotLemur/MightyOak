//! @file ArmEmu/Test/Test_Pipeline.cpp
//! @brief The definition of unit tests for exception vector handling and
//! return in the emulation of a virtual ARM processor pipeline.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2026
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
#include "TestExecTools.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Shared Assembly Code Strings
////////////////////////////////////////////////////////////////////////////////

// Group 1a-1f: SWI with MOVS PC, R14 return
const char swiMovsReturn[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"B swiHandler        ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".swiHandler\n"
"MOV R0, #1\n"
"MOVS PC, R14\n"
".main\n"
"MOV R0, #0\n"
"SWI 0\n"
"MOV R1, #42\n";

// Group 1g: SWI with LDM return (RISC OS pattern)
const char swiLdmReturn[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"B swiHandler        ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".swiHandler\n"
"STMFD R13!, {R10, R11, R12, R14}\n"
"MOV R10, #0xFF\n"
"MOV R11, #0xFE\n"
"MOV R12, #0xFD\n"
"LDMFD R13!, {R10, R11, R12, PC}^\n"
".main\n"
"MOV R0, #0\n"
"SWI 0\n"
"MOV R1, #42\n";

// Group 1i: SWI R14 contains PSR bits - extract top 4 bits of R14
const char swiR14Psr[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"B swiHandler        ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".swiHandler\n"
"MOV R5, R14, LSR #28\n"
"MOVS PC, R14\n"
".main\n"
"MOV R0, #0\n"
"SWI 0\n"
"MOV R1, #42\n";

// Group 1j: SWI multiple calls - handler increments R0
const char swiMultipleCalls[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"B swiHandler        ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".swiHandler\n"
"ADD R0, R0, #1\n"
"MOVS PC, R14\n"
".main\n"
"MOV R0, #0\n"
"SWI 0\n"
"SWI 0\n"
"MOV R1, #42\n";

// Group 1k: SWI return via SUBS PC, R14, #0
const char swiSubsReturn[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"B swiHandler        ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".swiHandler\n"
"MOV R0, #1\n"
"SUBS PC, R14, #0\n"
".main\n"
"MOV R0, #0\n"
"SWI 0\n"
"MOV R1, #42\n";

// Group 2: Undefined instruction with MOVS return
// 0xE7F000F0 is a guaranteed undefined instruction on ARM
const char undefMovsReturn[] =
"B main              ; 0x00: Reset\n"
"B undefHandler      ; 0x04: Undefined\n"
"BKPT 2              ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".undefHandler\n"
"MOV R0, #1\n"
"MOVS PC, R14\n"
".main\n"
"MOV R0, #0\n"
"EQUD 0xEC000000\n"
"MOV R1, #42\n";

// Group 3a: Data Abort - skip (SUBS PC, R14, #4)
// Load from unmapped address triggers data abort.
// R14 = faulting_addr + 8, so R14-4 = faulting_addr+4 = next instruction.
const char dataAbortSkip[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"BKPT 2              ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"B abortHandler      ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".abortHandler\n"
"MOV R0, #1\n"
"SUBS PC, R14, #4\n"
".main\n"
"MOV R2, #0x10000\n"
"LDR R3, [R2]\n"
"MOV R4, #42\n";

// Group 3b: Data Abort - retry (SUBS PC, R14, #8)
// Handler fixes R2 to point to mapped memory, retries the LDR.
const char dataAbortRetry[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"BKPT 2              ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"B abortHandler      ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".abortHandler\n"
"MOV R2, #0x8100\n"
"SUBS PC, R14, #8\n"
".main\n"
"MOV R2, #0x10000\n"
"LDR R3, [R2]\n"
"MOV R4, #42\n";

// Group 3c: Data Abort - store skip
const char dataAbortStoreSkip[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"BKPT 2              ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"B abortHandler      ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".abortHandler\n"
"MOV R0, #1\n"
"SUBS PC, R14, #4\n"
".main\n"
"MOV R2, #0x10000\n"
"MOV R3, #0xFF\n"
"STR R3, [R2]\n"
"MOV R4, #42\n";

// Group 4a: Address Exception - LDR with address > 26 bits (0x4000000)
const char addrExceptionLoadSkip[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"BKPT 2              ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"B addrHandler       ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".addrHandler\n"
"MOV R0, #1\n"
"SUBS PC, R14, #4\n"
".main\n"
"MOV R2, #0x4000000\n"
"LDR R3, [R2]\n"
"MOV R4, #42\n";

// Group 4b: Address Exception - STR variant
const char addrExceptionStoreSkip[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"BKPT 2              ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"B addrHandler       ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".addrHandler\n"
"MOV R0, #1\n"
"SUBS PC, R14, #4\n"
".main\n"
"MOV R2, #0x4000000\n"
"MOV R3, #0xFF\n"
"STR R3, [R2]\n"
"MOV R4, #42\n";

// Group 5a: SWI LDM restores all caller regs {R0-R3, R14}
const char swiLdmAllCallerRegs[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"B swiHandler        ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".swiHandler\n"
"STMFD R13!, {R0, R1, R2, R3, R14}\n"
"MOV R0, #0xFF\n"
"MOV R1, #0xFE\n"
"MOV R2, #0xFD\n"
"MOV R3, #0xFC\n"
"LDMFD R13!, {R0, R1, R2, R3, PC}^\n"
".main\n"
"MOV R0, #0x11\n"
"MOV R1, #0x22\n"
"MOV R2, #0x33\n"
"MOV R3, #0x44\n"
"SWI 0\n"
"MOV R4, #42\n";

// Group 5b: SWI LDM preserves flags
const char swiLdmPreservesFlags[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"B swiHandler        ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".swiHandler\n"
"STMFD R13!, {R14}\n"
"LDMFD R13!, {PC}^\n"
".main\n"
"MOV R0, #0\n"
"SWI 0\n"
"MOV R1, #42\n";

// Group 6: IRQ handler - sets R5=1, disables IRQ in R14 to prevent re-entry
const char irqSubsReturn[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"BKPT 2              ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"B irqHandler        ; 0x18: IRQ\n"
"BKPT 7              ; 0x1C: FIQ\n"
".irqHandler\n"
"MOV R5, #1\n"
"ORR R14, R14, #0x08000000\n"
"SUBS PC, R14, #4\n"
".main\n"
"MOV R0, #42\n";

// Group 7: FIQ handler - sets R5=1, disables both IRQ+FIQ in R14
// Also clobbers FIQ-banked R8, R9 to test banking.
const char fiqSubsReturn[] =
"B main              ; 0x00: Reset\n"
"BKPT 1              ; 0x04: Undefined\n"
"BKPT 2              ; 0x08: SWI\n"
"BKPT 3              ; 0x0C: Prefetch Abort\n"
"BKPT 4              ; 0x10: Data Abort\n"
"BKPT 5              ; 0x14: Address Exception\n"
"BKPT 6              ; 0x18: IRQ\n"
"B fiqHandler        ; 0x1C: FIQ\n"
".fiqHandler\n"
"MOV R5, #1\n"
"MOV R8, #0xFF\n"
"MOV R9, #0xEE\n"
"ORR R14, R14, #0x0C000000\n"
"SUBS PC, R14, #4\n"
".main\n"
"MOV R0, #42\n";

////////////////////////////////////////////////////////////////////////////////
// Test Parameter Arrays
////////////////////////////////////////////////////////////////////////////////

// Group 1: SWI Entry and Return
const CoreTestParams swiTests[] = {
    // 1a. Basic SWI, handler does MOVS PC, R14
    { TLOC, "SWI_ReturnViaMovsPC",
      "",
      "R0=1,R1=42",
      swiMovsReturn },

    // 1b. All four flags preserved through SWI
    { TLOC, "SWI_PreservesFlags_NZCV",
      "Status=NZCV",
      "R0=1,R1=42,Status=NZCV",
      swiMovsReturn },

    // 1c. Subset of flags NZ
    { TLOC, "SWI_PreservesFlags_NZ",
      "Status=NZ",
      "R0=1,R1=42,Status=NZ",
      swiMovsReturn },

    // 1d. Subset of flags CV
    { TLOC, "SWI_PreservesFlags_CV",
      "Status=CV",
      "R0=1,R1=42,Status=CV",
      swiMovsReturn },

    // 1e. SWI from User mode, returns to User mode
    { TLOC, "SWI_FromUserMode",
      "CPSR=0",
      "R0=1,R1=42,Mode=USR26",
      swiMovsReturn },

    // 1f. SWI from User mode preserves N,C flags
    { TLOC, "SWI_FromUserMode_PreservesFlags",
      "CPSR=0xA0000000",
      "R0=1,R1=42,Mode=USR26,Status=NC",
      swiMovsReturn },

    // 1g. SWI return via LDMFD {R10-R12,PC}^ (RISC OS pattern)
    { TLOC, "SWI_ReturnViaLDM",
      "R13=0x10000,R10=0xAA,R11=0xBB,R12=0xCC",
      "R1=42,R10=0xAA,R11=0xBB,R12=0xCC,R13=0x10000",
      swiLdmReturn },

    // 1h. SWI LDM return from User mode
    { TLOC, "SWI_ReturnViaLDM_FromUserMode",
      "CPSR=0,R13=0x10000,R10=0xAA,R11=0xBB,R12=0xCC",
      "R1=42,R10=0xAA,R11=0xBB,R12=0xCC,Mode=USR26",
      swiLdmReturn },

    // 1i. Verify R14 captures pre-SWI PSR bits (N+C = 0xA)
    { TLOC, "SWI_R14_ContainsPSR",
      "Status=NC",
      "R5=0xA,R1=42,Status=NC",
      swiR14Psr },

    // 1j. Two SWIs in sequence, handler increments R0 each call
    { TLOC, "SWI_MultipleCalls",
      "",
      "R0=2,R1=42",
      swiMultipleCalls },

    // 1k. SWI return via SUBS PC, R14, #0
    { TLOC, "SWI_ReturnViaSubs",
      "",
      "R0=1,R1=42",
      swiSubsReturn },
};

// Group 2: Undefined Instruction Return
const CoreTestParams undefTests[] = {
    // 2a. Trigger via guaranteed undefined, return to next instruction
    { TLOC, "Undef_ReturnViaMovsPC",
      "",
      "R0=1,R1=42",
      undefMovsReturn },

    // 2b. Preserves flags through undefined exception
    { TLOC, "Undef_PreservesFlags",
      "Status=NZC",
      "R0=1,R1=42,Status=NZC",
      undefMovsReturn },

    // 2c. Undefined from User mode
    { TLOC, "Undef_FromUserMode",
      "CPSR=0",
      "R0=1,R1=42,Mode=USR26",
      undefMovsReturn },
};

// Group 3: Data Abort Return
const CoreTestParams dataAbortTests[] = {
    // 3a. Load from unmapped address, handler skips
    { TLOC, "DataAbort_Skip",
      "",
      "R0=1,R4=42",
      dataAbortSkip },

    // 3b. Handler fixes base register, retries LDR
    { TLOC, "DataAbort_Retry",
      "LWORD[8100]=0xDEADBEEF",
      "R2=0x8100,R3=0xDEADBEEF,R4=42",
      dataAbortRetry },

    // 3c. STR to unmapped address, handler skips
    { TLOC, "DataAbort_StoreSkip",
      "",
      "R0=1,R4=42",
      dataAbortStoreSkip },

    // 3d. Preserves flags through data abort
    { TLOC, "DataAbort_PreservesFlags",
      "Status=NV",
      "R0=1,R4=42,Status=NV",
      dataAbortSkip },
};

// Group 4: Address Exception Return
const CoreTestParams addrExceptionTests[] = {
    // 4a. LDR with address > 26 bits, handler skips
    { TLOC, "AddrException_LoadSkip",
      "",
      "R0=1,R4=42",
      addrExceptionLoadSkip },

    // 4b. STR with address > 26 bits, handler skips
    { TLOC, "AddrException_StoreSkip",
      "",
      "R0=1,R4=42",
      addrExceptionStoreSkip },

    // 4c. Preserves flags through address exception
    { TLOC, "AddrException_PreservesFlags",
      "Status=ZC",
      "R0=1,R4=42,Status=ZC",
      addrExceptionLoadSkip },
};

// Group 5: LDM Return Edge Cases
const CoreTestParams ldmEdgeCaseTests[] = {
    // 5a. Save/restore {R0-R3, R14}, handler modifies R0-R3
    { TLOC, "SWI_LDM_RestoresAllCallerRegs",
      "R13=0x10000",
      "R0=0x11,R1=0x22,R2=0x33,R3=0x44,R4=42",
      swiLdmAllCallerRegs },

    // 5b. LDMFD {PC}^ restores flags
    { TLOC, "SWI_LDM_PreservesFlags",
      "R13=0x10000,Status=NV",
      "R1=42,Status=NV",
      swiLdmPreservesFlags },
};

// Group 6: IRQ Entry and Return
const CoreTestParams irqTests[] = {
    // 6a. Basic IRQ, handler sets R5=1, returns
    { TLOC, "IRQ_ReturnViaSubs",
      "CPSR=0x00000003,IrqStatus=2",
      "R5=1,R0=42",
      irqSubsReturn },

    // 6b. Verify non-banked registers preserved
    { TLOC, "IRQ_PreservesRegisters",
      "R1=0x11,R2=0x22,R3=0x33,R4=0x44,CPSR=0x00000003,IrqStatus=2",
      "R5=1,R0=42,R1=0x11,R2=0x22,R3=0x33,R4=0x44",
      irqSubsReturn },

    // 6c. IRQ while in User26 mode
    { TLOC, "IRQ_FromUserMode",
      "CPSR=0,IrqStatus=2",
      "R5=1,R0=42,Mode=USR26",
      irqSubsReturn },

    // 6d. Status flags preserved through IRQ return
    { TLOC, "IRQ_PreservesFlags",
      "CPSR=0xA0000003,IrqStatus=2",
      "R5=1,R0=42,Status=NC",
      irqSubsReturn },
};

// Group 7: FIQ Entry and Return
const CoreTestParams fiqTests[] = {
    // 7a. Basic FIQ, handler sets R5=1, returns
    { TLOC, "FIQ_ReturnViaSubs",
      "CPSR=0x00000003,IrqStatus=1",
      "R5=1,R0=42",
      fiqSubsReturn },

    // 7b. Verify non-FIQ-banked registers (R0-R7) preserved
    { TLOC, "FIQ_PreservesRegisters",
      "R1=0x11,R2=0x22,R3=0x33,CPSR=0x00000003,IrqStatus=1",
      "R5=1,R0=42,R1=0x11,R2=0x22,R3=0x33",
      fiqSubsReturn },

    // 7c. FIQ while in User26 mode
    { TLOC, "FIQ_FromUserMode",
      "CPSR=0,IrqStatus=1",
      "R5=1,R0=42,Mode=USR26",
      fiqSubsReturn },

    // 7d. Verify FIQ banking of R8-R12 (handler clobbers FIQ copies)
    { TLOC, "FIQ_BankedRegisters",
      "R8=0x88,R9=0x99,CPSR=0x00000003,IrqStatus=1",
      "R5=1,R0=42,R8=0x88,R9=0x99",
      fiqSubsReturn },

    // 7e. Status flags preserved through FIQ return
    { TLOC, "FIQ_PreservesFlags",
      "CPSR=0x50000003,IrqStatus=1",
      "R5=1,R0=42,Status=ZV",
      fiqSubsReturn },
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Functions
////////////////////////////////////////////////////////////////////////////////
//! @brief Registers exception vector tests for each type of system they
//! should be tested against.
void registerPipelineTests()
{
    // Register exception tests against basic ARMv2 implementation.
    RegisterExceptionTests<ArmV2TestSystemTraits<false>>("ARMv2_ExceptionReturn",
        swiTests, std::size(swiTests));
    RegisterExceptionTests<ArmV2TestSystemTraits<false>>("ARMv2_ExceptionReturn",
        undefTests, std::size(undefTests));
    RegisterExceptionTests<ArmV2TestSystemTraits<false>>("ARMv2_ExceptionReturn",
        dataAbortTests, std::size(dataAbortTests));
    RegisterExceptionTests<ArmV2TestSystemTraits<false>>("ARMv2_ExceptionReturn",
        addrExceptionTests, std::size(addrExceptionTests));
    RegisterExceptionTests<ArmV2TestSystemTraits<false>>("ARMv2_ExceptionReturn",
        ldmEdgeCaseTests, std::size(ldmEdgeCaseTests));
    RegisterExceptionTests<ArmV2TestSystemTraits<false>>("ARMv2_ExceptionReturn",
        irqTests, std::size(irqTests));
    RegisterExceptionTests<ArmV2TestSystemTraits<false>>("ARMv2_ExceptionReturn",
        fiqTests, std::size(fiqTests));

    // Repeat for the ARMv2aS architecture.
    RegisterExceptionTests<ArmV2aSTestSystemTraits<false>>("ARMv2aS_ExceptionReturn",
        swiTests, std::size(swiTests));
    RegisterExceptionTests<ArmV2aSTestSystemTraits<false>>("ARMv2aS_ExceptionReturn",
        undefTests, std::size(undefTests));
    RegisterExceptionTests<ArmV2aSTestSystemTraits<false>>("ARMv2aS_ExceptionReturn",
        dataAbortTests, std::size(dataAbortTests));
    RegisterExceptionTests<ArmV2aSTestSystemTraits<false>>("ARMv2aS_ExceptionReturn",
        addrExceptionTests, std::size(addrExceptionTests));
    RegisterExceptionTests<ArmV2aSTestSystemTraits<false>>("ARMv2aS_ExceptionReturn",
        ldmEdgeCaseTests, std::size(ldmEdgeCaseTests));
    RegisterExceptionTests<ArmV2aSTestSystemTraits<false>>("ARMv2aS_ExceptionReturn",
        irqTests, std::size(irqTests));
    RegisterExceptionTests<ArmV2aSTestSystemTraits<false>>("ARMv2aS_ExceptionReturn",
        fiqTests, std::size(fiqTests));

    // Repeat for the ARMv2a architecture.
    RegisterExceptionTests<ArmV2aTestSystemTraits<false>>("ARMv2a_ExceptionReturn",
        swiTests, std::size(swiTests));
    RegisterExceptionTests<ArmV2aTestSystemTraits<false>>("ARMv2a_ExceptionReturn",
        undefTests, std::size(undefTests));
    RegisterExceptionTests<ArmV2aTestSystemTraits<false>>("ARMv2a_ExceptionReturn",
        dataAbortTests, std::size(dataAbortTests));
    RegisterExceptionTests<ArmV2aTestSystemTraits<false>>("ARMv2a_ExceptionReturn",
        addrExceptionTests, std::size(addrExceptionTests));
    RegisterExceptionTests<ArmV2aTestSystemTraits<false>>("ARMv2a_ExceptionReturn",
        ldmEdgeCaseTests, std::size(ldmEdgeCaseTests));
    RegisterExceptionTests<ArmV2aTestSystemTraits<false>>("ARMv2a_ExceptionReturn",
        irqTests, std::size(irqTests));
    RegisterExceptionTests<ArmV2aTestSystemTraits<false>>("ARMv2a_ExceptionReturn",
        fiqTests, std::size(fiqTests));
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

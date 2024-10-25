//! @file ArmEmu/Test_ALU.cpp
//! @brief The definition of unit tests for the emulation of the Arithmetic
//! Logic Unit of an ARM processor core.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "TestTools.hpp"
#include "TestExecTools.hpp"

namespace Mo {
namespace Arm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
// Parameters for tests compatible with all ARM ALUs.
const CoreTestParams basicAlu[] = {
    // Verify no 'S' suffix leaves status flags unaffected.
    { TLOC, "MOV_Immediate", "R0=0", "R0=42,Status=0", "MOV R0,#42" },
    { TLOC, "MOV_Immediate_V_Preserved", "Status=V,R0=0", "R0=42,Status=V", "MOV R0,#42" },
    { TLOC, "MOV_Zero", "R2=69", "R2=0,Status=0", "MOV R2,#0" },
    { TLOC, "MOV_Zero_V_Preserved", "Status=V,R2=42", "R2=0,Status=V", "MOV R2,#0" },
    { TLOC, "MOV_NegImmediate", "", "R3=0xF0000000,Status=0", "MOV R3,#0xF0000000" },
    { TLOC, "MOV_NegImmediate_V_Preserved", "Status=V", "R3=0xF0000000,Status=V", "MOV R3,#0xF0000000" },

    // Verify no 'S' suffix ignores carry-out from the barrel shifter.
    { TLOC, "MOV_LSL_NoCarryOut", "R4=0xF0000000", "R4=0xF0000000,R5=0xE0000000,Status=0", "MOV R5,R4,LSL #1" },
    { TLOC, "MOV_LSL_NoCarryOut_V_Preserved", "Status=V,R4=0xF0000000", "R4=0xF0000000,R5=0xE0000000,Status=V", "MOV R5,R4,LSL #1" },
    { TLOC, "MOV_LSL_32", "R3=32,R4=1", "R3=32,R4=1,R5=0,Status=0", "MOV R5,R4,LSL R3" },
    { TLOC, "MOV_LSL_32_V_Preserved", "R3=32,R4=1,Status=V", "R3=32,R4=1,R5=0,Status=V", "MOV R5,R4,LSL R3" },
    { TLOC, "MOV_LSL_33", "R3=33,R4=1", "R3=33,R4=1,R5=0,Status=0", "MOV R5,R4,LSL R3" },
    { TLOC, "MOV_LSL_33_V_Preserved", "R3=33,R4=1,Status=V", "R3=33,R4=1,R5=0,Status=V", "MOV R5,R4,LSL R3" },
    { TLOC, "MOV_LSR_NoCarryOut", "R4=3", "R4=3,R5=1,Status=0", "MOV R5,R4,LSR #1" },
    { TLOC, "MOV_LSR_NoCarryOut_V_Preserved", "R4=3,Status=V", "R4=3,R5=1,Status=V", "MOV R5,R4,LSR #1" },
    { TLOC, "MOV_LSR_32", "R3=32,R4=0xF0000000", "R3=32,R4=0xF0000000,R5=0,Status=0", "MOV R5,R4,LSR R3" },
    { TLOC, "MOV_LSR_32_V_Preserved", "R3=32,R4=0xF0000000,Status=V", "R3=32,R4=0xF0000000,R5=0,Status=V", "MOV R5,R4,LSR R3" },
    { TLOC, "MOV_LSR_33", "R3=33,R4=0xF0000000", "R3=33,R4=0xF0000000,R5=0,Status=0", "MOV R5,R4,LSR R3" },
    { TLOC, "MOV_LSR_33_V_Preserved", "R3=33,R4=0xF0000000,Status=V", "R3=33,R4=0xF0000000,R5=0,Status=V", "MOV R5,R4,LSR R3" },
    { TLOC, "MOV_ASR_NoCarryOut", "R4=0xF0000001", "R4=0xF0000001,R5=0xF8000000,Status=0", "MOV R5,R4,ASR #1" },
    { TLOC, "MOV_ASR_NoCarryOut_V_Preserved", "R4=0xF0000001,Status=V", "R4=0xF0000001,R5=0xF8000000,Status=V", "MOV R5,R4,ASR #1" },
    { TLOC, "MOV_ASR_32", "R3=32,R4=0x80000000", "R3=32,R4=0x80000000,R5=0xFFFFFFFF,Status=0", "MOV R5,R4,ASR R3" },
    { TLOC, "MOV_ASR_32_V_Preserved", "R3=32,R4=0x80000000,Status=V", "R3=32,R4=0x80000000,R5=0xFFFFFFFF,Status=V", "MOV R5,R4,ASR R3" },
    { TLOC, "MOV_ROR_NoCarryOut", "R4=1", "R4=1,R5=0x80000000,Status=0", "MOV R5,R4,ROR #1" },
    { TLOC, "MOV_ROR_NoCarryOut_V_Preserved", "R4=1,Status=V", "R4=1,R5=0x80000000,Status=V", "MOV R5,R4,ROR #1" },
    { TLOC, "MOV_ROR_32", "R3=32,R4=0xF0000000", "R3=32,R4=0xF0000000,R5=0xF0000000", "MOV R5,R4,ROR R3" },
    { TLOC, "MOV_ROR_32_V_Preserved", "R3=32,R4=0xF0000000,Status=V", "R3=32,R4=0xF0000000,R5=0xF0000000,Status=V", "MOV R5,R4,ROR R3" },
    { TLOC, "MOV_ROR_MoreThan32", "R3=48,R4=0xF000", "R3=48,R4=0xF000,R5=0xF0000000,Status=0", "MOV R5,R4,ROR R3" },
    { TLOC, "MOV_ROR_MoreThan32_V_Preserved", "R3 = 48,R4 = 0xF000,Status=V", "R3=48,R4=0xF000,R5=0xF0000000,Status=V", "MOV R5,R4,ROR R3" },
    { TLOC, "MOV_RRX_NoCarryOut", "R4=1", "R4=1,R5=0,Status=0", "MOV R5,R4,RRX" },
    { TLOC, "MOV_RRX_NoCarryOut_V_Preserved", "R4=1,Status=V", "R4=1,R5=0,Status=V", "MOV R5,R4,RRX" },
    { TLOC, "MOV_PreserveStatus", "R3=0,Status=CN", "R3=0,R4=0,Status=CN", "MOV R6, R3" },
    { TLOC, "MOV_PreserveStatus_V_Preserved", "R3=0,Status=NCV", "R3=0,R6=0,Status=NCV", "MOV R6, R3" },

    // Verify 'S' changes status flags.
    { TLOC, "MOVS_Immediate", "R4=0,Status=N", "R4=42,Status=0", "MOVS R4,#42" },
    { TLOC, "MOVS_Immediate_V_Preserved", "R4=0,Status=VN", "R4=42,Status=V", "MOVS R4,#42" },
    { TLOC, "MOVS_NegImmediate", "R1=0,Status=Z", "R1=0xC0000003,Status=N", "MOVS R1,#&C0000003" },
    { TLOC, "MOVS_NegImmediate_V_Preserved", "R1=0,Status=VZ", "R1=0xC0000003,Status=VN", "MOVS R1,#&C0000003" },
    { TLOC, "MOVS_Zero", "R11=42,Status=0", "R11=0,Status=Z", "MOVS R11,#0" },
    { TLOC, "MOVS_Zero_V_Preserved", "R11=69,Status=VN", "R11=0,Status=VZ", "MOVS R11,#0" },

    // Verify 'S' suffix propagates carry-out from the barrel shifter.
    { TLOC, "MOVS_LSL_NoCarryOut", "R4=0x30000000,Status=C", "R4=0x30000000,R5=0x60000000,Status=0", "MOVS R5,R4,LSL #1" },
    { TLOC, "MOVS_LSL_NoCarryOut_V_Preserved", "R4=0x30000000,Status=VC", "R4=0x30000000,R5=0x60000000,Status=V", "MOVS R5,R4,LSL #1" },
    { TLOC, "MOVS_LSL_CarryOut", "R4=0xF0000000,Status=Z", "R4=0xF0000000,R5=0xE0000000,Status=NC", "MOVS R5,R4,LSL #1" },
    { TLOC, "MOVS_LSL_CarryOut_V_Preserved", "R4=0xF0000000,Status=ZV", "R4=0xF0000000,R5=0xE0000000,Status=NCV", "MOVS R5,R4,LSL #1" },
    { TLOC, "MOVS_LSL_Immed_0", "R3=0,Status=C", "R3=0,R6=0,Status=CZ", "MOVS R6,R3,LSL #0" },
    { TLOC, "MOVS_LSL_Immed_0_V_Preserved", "R3=0,Status=CV", "R3=0,R6=0,Status=CZV", "MOVS R6,R3,LSL #0" },
    { TLOC, "MOVS_LSL_32", "R3=32,R4=1,Status=N", "R3=32,R4=1,R5=0,Status=CZ", "MOVS R5,R4,LSL R3" },
    { TLOC, "MOVS_LSL_32_V_Preserved", "R3=32,R4=1,Status=NV", "R3=32,R4=1,R5=0,Status=CZV", "MOVS R5,R4,LSL R3" },
    { TLOC, "MOVS_LSL_33", "R3=33,R4=1,Status=CN", "R3=33,R4=1,R5=0,Status=Z", "MOVS R5,R4,LSL R3" },
    { TLOC, "MOVS_LSL_33_V_Preserved", "R3=33,R4=1,Status=CNV", "R3=33,R4=1,R5=0,Status=ZV", "MOVS R5,R4,LSL R3" },
    { TLOC, "MOVS_LSR_NoCarryOut", "R4=2,Status=CZ", "R4=2,R5=1,Status=0", "MOVS R5,R4,LSR #1" },
    { TLOC, "MOVS_LSR_NoCarryOut_V_Preserved", "R4=2,Status=V", "R4=2,R5=1,Status=V", "MOVS R5,R4,LSR #1" },
    { TLOC, "MOVS_LSR_CarryOut", "R4=3,Status=N", "R4=3,R5=1,Status=C", "MOVS R5,R4,LSR #1" },
    { TLOC, "MOVS_LSR_CarryOut_V_Preserved", "R4=3,Status=V", "R4=3,R5=1,Status=VC", "MOVS R5,R4,LSR #1" },
    { TLOC, "MOVS_LSR_32", "R3=32,R4=0xF0000000,Status=N", "R3=32,R4=0xF0000000,R5=0,Status=CZ", "MOVS R5,R4,LSR R3" },
    { TLOC, "MOVS_LSR_32_V_Preserved", "R3=32,R4=0xF0000000,Status=NV", "R3=32,R4=0xF0000000,R5=0,Status=CZV", "MOVS R5,R4,LSR R3" },
    { TLOC, "MOVS_LSR_32_Immed", "R4=0xF0000000,Status=N", "R4=0xF0000000,R5=0,Status=CZ", "MOVS R5,R4,LSR #32" },
    { TLOC, "MOVS_LSR_32_Immed_V_Preserved", "R4=0xF0000000,Status=NV", "R4=0xF0000000,R5=0,Status=CZV", "MOVS R5,R4,LSR #32" },
    { TLOC, "MOVS_LSR_33", "R3=33,R4=0xF0000000,Status=N", "R3=33,R4=0xF0000000,R5=0,Status=Z", "MOVS R5,R4,LSR R3" },
    { TLOC, "MOVS_LSR_33_V_Preserved", "R3=33,R4=0xF0000000,Status=NV", "R3=33,R4=0xF0000000,R5=0,Status=ZV", "MOVS R5,R4,LSR R3" },
    { TLOC, "MOVS_ASR_NoCarryOut", "R4=0xF0000000,Status=CZ", "R4=0xF0000000,R5=0xF8000000,Status=N", "MOVS R5,R4,ASR #1" },
    { TLOC, "MOVS_ASR_NoCarryOut_V_Preserved", "R4=0xF0000000,Status=VC", "R4=0xF0000000,R5=0xF8000000,Status=NV", "MOVS R5,R4,ASR #1" },
    { TLOC, "MOVS_ASR_CarryOut", "R4=0xF0000001,Status=Z", "R4=0xF0000001,R5=0xF8000000,Status=NC", "MOVS R5,R4,ASR #1" },
    { TLOC, "MOVS_ASR_CarryOut_V_Preserved", "R4=0xF0000001,Status=VZ", "R4=0xF0000001,R5=0xF8000000,Status=NCV", "MOVS R5,R4,ASR #1" },
    { TLOC, "MOVS_ASR_32", "R3=32,R4=0x80000000,Status=Z", "R3=32,R4=0x80000000,R5=0xFFFFFFFF,Status=NC", "MOVS R5,R4,ASR R3" },
    { TLOC, "MOVS_ASR_32_V_Preserved", "R3=32,R4=0x80000000,Status=ZV", "R3=32,R4=0x80000000,R5=0xFFFFFFFF,Status=NCV", "MOVS R5,R4,ASR R3" },
    { TLOC, "MOVS_ASR_32_Immed", "R4=0x80000000,Status=Z", "R4=0x80000000,R5=0xFFFFFFFF,Status=NC", "MOVS R5,R4,ASR #32" },
    { TLOC, "MOVS_ASR_32_Immed_V_Preserved", "R4=0x80000000,Status=VZ", "R4=0x80000000,R5=0xFFFFFFFF,Status=VNC", "MOVS R5,R4,ASR #32" },
    { TLOC, "MOVS_ROR_NoCarryOut", "R4=2,Status=C", "R4=2,R5=1,Status=0", "MOVS R5,R4,ROR #1" },
    { TLOC, "MOVS_ROR_NoCarryOut_V_Preserved", "R4=2,Status=VC", "R4=2,R5=1,Status=V", "MOVS R5,R4,ROR #1" },
    { TLOC, "MOVS_ROR_CarryOut", "R4=1,Status=Z", "R4=1,R5=0x80000000,Status=NC", "MOVS R5,R4,ROR #1" },
    { TLOC, "MOVS_ROR_CarryOut_V_Preserved", "R4=1,Status=VZ", "R4=1,R5=0x80000000,Status=VNC", "MOVS R5,R4,ROR #1" },
    { TLOC, "MOVS_ROR_32", "R3=32,R4=0xF0000000,Status=Z", "R3=32,R4=0xF0000000,R5=0xF0000000,Status=CN", "MOVS R5,R4,ROR R3" },
    { TLOC, "MOVS_ROR_32_V_Preserved", "R3=32,R4=0xF0000000,Status=VZ", "R3=32,R4=0xF0000000,R5=0xF0000000,Status=VCN", "MOVS R5,R4,ROR R3" },
    { TLOC, "MOVS_ROR_MoreThan32", "R3=48,R4=0xF000,Status=Z", "R3=48,R4=0xF000,R5=0xF0000000,Status=NC", "MOVS R5,R4,ROR R3" },
    { TLOC, "MOVS_ROR_MoreThan32_V_Preserved", "R3=48,R4=0xF000,Status=VZ", "R3=48,R4=0xF000,R5=0xF0000000,Status=VNC", "MOVS R5,R4,ROR R3" },
    { TLOC, "MOVS_RRX_NoCarryOut", "R4=2,Status=0", "R4=2,R5=1,Status=0", "MOVS R5,R4,RRX" },
    { TLOC, "MOVS_RRX_NoCarryOut_V_Preserved", "R4=2,Status=V", "R4=2,R5=1,Status=V", "MOVS R5,R4,RRX" },
    { TLOC, "MOVS_RRX_CarryInOnly", "R4=2,Status=C", "R4=2,R5=0x80000001,Status=N", "MOVS R5,R4,RRX" },
    { TLOC, "MOVS_RRX_CarryInOnly_V_Preserved", "R4=2,Status=CV", "R4=2,R5=0x80000001,Status=NV", "MOVS R5,R4,RRX" },
    { TLOC, "MOVS_RRX_CarryOut", "R4=1,Status=N", "R4=1,R5=0,Status=ZC", "MOVS R5,R4,RRX" },
    { TLOC, "MOVS_RRX_CarryOut_V_Preserved", "R4=1,Status=NV", "R4=1,R5=0,Status=VZC", "MOVS R5,R4,RRX" },
    { TLOC, "MOVS_RRX_CarryInAndOut", "R4=1,Status=CN", "R4=1,R5=0x80000000,Status=NC", "MOVS R5,R4,RRX" },
    { TLOC, "MOVS_RRX_CarryInAndOut_V_Preserved", "R4=1,Status=CN", "R4=1,R5=0x80000000,Status=NC", "MOVS R5,R4,RRX" },
    { TLOC, "MOVS_PreserveCarry", "R3=0,R6=42,Status=C", "R3=0,R6=0,Status=CZ", "MOVS R6, R3" },
    { TLOC, "MOVS_PreserveCarry_V_Preserved", "R3=0,R6=42,Status=CV", "R3=0,R6=0,Status=CZV", "MOVS R6, R3" },

    // AND
    { TLOC, "AND_NoFlags", "R9=0xFF,R10=0xFF0,Status=0", "R9=0xFF,R10=0xFF0,R11=0xF0,Status=0", "AND R11,R9,R10" },
    { TLOC, "AND_Flags", "R9=0xFF000000,R10=0xF000000F,Status=0", "R9=0xFF000000,R10=0xF000000F,R11=0xF0000000,Status=0", "AND R11,R9,R10" },
    { TLOC, "ANDS_Flags", "R9=0xFF000000,R10=0xF000000F,Status=0", "R9=0xFF000000,R10=0xF000000F,R11=0xF0000000,Status=N", "ANDS R11,R9,R10" },

    // EOR
    { TLOC, "EOR_NoFlags", "R9=0xFF,R10=0xFF0,Status=0", "R9=0xFF,R10=0xFF0,R11=0xF0F,Status=0", "EOR R11,R9,R10" },
    { TLOC, "EORS_NoFlags", "R9=0xFF,R10=0xFF0,Status=0", "R9=0xFF,R10=0xFF0,R11=0xF0F,Status=0", "EORS R11,R9,R10" },
    { TLOC, "EOR_Flags", "R9=0xFF,Status=CN", "R9=0xFF,R10=0,Status=CN", "EOR R10,R9,R9" },
    { TLOC, "EORS_Flags", "R9=0xFF,Status=CN", "R9=0xFF,R10=0,Status=CZ", "EORS R10,R9,R9" },

    // TST
    { TLOC, "TST_Flags", "R9=0xFF,R10=0xFF00,Status=NC", "R9=0xFF,R10=0xFF00,Status=ZC", "TST R10, R9" },

    // TEQ
    { TLOC, "TEQ_Flags", "R9=0xFF,R10=0x80000000,Status=ZCV", "R9=0xFF,R10=0x80000000,Status=NCV", "TEQ R10, R9" },

    // ORR
    { TLOC, "ORR_NoFlags", "R9=0xFF,R10=0xFF000,Status=CVN", "R9=0xFF,R10=0xFF000,R11=0xFF0FF,Status=CVN", "ORR R11,R9,R10" },
    { TLOC, "ORRS_NoFlags", "R9=0xFF,R10=0xFF000,Status=CVN", "R9=0xFF,R10=0xFF000,R11=0xFF0FF,Status=CV", "ORRS R11,R9,R10" },
    { TLOC, "ORR_Flags", "R9=0xFF000000,R10=0xF000000F,Status=CVN", "R9=0xFF000000,R10=0xF000000F,R11=0xFF00000F,Status=CVN", "ORR R11,R9,R10" },
    { TLOC, "ORRS_Flags", "R9=0xFF000000,R10=0xF000000F,Status=CVZ", "R9=0xFF000000,R10=0xF000000F,R11=0xFF00000F,Status=CVN", "ORRS R11,R9,R10" },

    // BIC
    { TLOC, "BIC_NoFlags", "R9=0xFF,R10=0xFF0,Status=Z", "R9=0xFF,R10=0xFF0,R11=0x0F,Status=Z", "BIC R11,R9,R10" },
    { TLOC, "BICS_NoFlags", "R9=0xFF,R10=0xFF0,Status=Z", "R9=0xFF,R10=0xFF0,R11=0x0F,Status=0", "BICS R11,R9,R10" },
    { TLOC, "BIC_Flags", "R9=0xFF000000,R10=0xF000000F,Status=CVN", "R9=0xFF000000,R10=0xF000000F,R11=0x0F000000,Status=CVN", "BIC R11,R9,R10" },
    { TLOC, "BICS_Flags", "R9=0xF000000F,Status=CVN", "R9=0xF000000F,R11=0,Status=CVZ", "BICS R10,R9,R9" },

    // MVN
    { TLOC, "MVN_NoFlags", "R9=42,Status=0", "R9=0x00FFFFFF,Status=0", "MVN R9,#0xFF000000" },
    { TLOC, "MVNS_NoFlags", "R9=42,Status=0", "R9=0x00FFFFFF,Status=0", "MVNS R9,#0xFF000000" },
    { TLOC, "MVN_Flags", "R9=69,Status=0", "R9=0xFFFFFF00,Status=0", "MVN R9,#0x000000FF" },
    { TLOC, "MVNS_Flags", "R9=69,Status=0", "R9=0xFFFFFF00,Status=N", "MVNS R9,#0x000000FF" },

    // ADD
    { TLOC, "ADD_NoFlags", "R0=1,R1=9,Status=0", "R0=1,R1=2,Status=0", "ADD R1,R0,#1" },
    { TLOC, "ADD_Overflow", "R0=0x7FFFFFFF,R1=1,Status=0", "R0=0x7FFFFFFF,R1=0x80000000,Status=0", "ADD R1,R0,#1" },
    { TLOC, "ADD_Carry", "R0=0xFFFFFFFF,R1=5,Status=0", "R0=0xFFFFFFFF,R1=0,Status=0", "ADD R1,R0,#1" },
    { TLOC, "ADD_Zero",  "R0=0xFFFFFFFE,R1=5,Status=0", "R0=0xFFFFFFFE,R1=0,Status=0", "ADD R1,R0,#2" },
    { TLOC, "ADD_Negative", "R0=0xFFFFFFFE,R1=0xFFFFFFFF,Status=0", "R0=0xFFFFFFFE,R1=0xFFFFFFFF,Status=0", "ADD R1,R0,#1" },
    { TLOC, "ADDS_NoFlags", "R0=1,R1=9,Status=0", "R0=1,R1=2,Status=0", "ADDS R1,R0,#1" },
    { TLOC, "ADDS_Overflow", "R0=0x7FFFFFFF,R1=1,Status=0", "R0=0x7FFFFFFF,R1=0x80000000,Status=NV", "ADDS R1,R0,#1" },
    { TLOC, "ADDS_Carry", "R0=0xFFFFFFFF,R1=5,Status=0", "R0=0xFFFFFFFF,R1=0,Status=CZ", "ADDS R1,R0,#1" },
    { TLOC, "ADDS_Zero",  "R0=0xFFFFFFFE,R1=5,Status=0", "R0=0xFFFFFFFE,R1=0,Status=CZ", "ADDS R1,R0,#2" },
    { TLOC, "ADDS_Negative", "R0=0xFFFFFFFE,R1=0xFFFFFFFF,Status=0", "R0=0xFFFFFFFE,R1=0xFFFFFFFF,Status=N", "ADDS R1,R0,#1" },

    // ADC
    { TLOC, "ADC_NoFlagsCC", "R0=1,Status=0", "R0=1,R1=2,Status=0", "ADC R1,R0,#1" },
    { TLOC, "ADC_NoFlagsCS", "R0=1,Status=C", "R0=1,R1=3,Status=C", "ADC R1,R0,#1" },
    { TLOC, "ADC_Overflow", "R0=0x7FFFFFFF,Status=0", "R0=0x7FFFFFFF,R1=0x80000000,Status=0", "ADC R1,R0,#1" },
    { TLOC, "ADC_Carry", "R0=0xFFFFFFFF,Status=0", "R0=0xFFFFFFFF,R1=1,Status=0", "ADC R1,R0,#2" },
    { TLOC, "ADC_Zero", "R0=0xFFFFFFFF,Status=0", "R0=0xFFFFFFFF,R1=0,Status=0", "ADC R1,R0,#1" },
    { TLOC, "ADC_Negative", "R0=0xFFFFFFFE,Status=0", "R0=0xFFFFFFFE,R1=0xFFFFFFFF,Status=0", "ADC R1,R0,#1" },
    { TLOC, "ADCS_NoFlagsCC", "R0=1,Status=0", "R0=1,R1=2,Status=0", "ADCS R1,R0,#1" },
    { TLOC, "ADCS_NoFlagsCS", "R0=1,Status=C", "R0=1,R1=3,Status=0", "ADCS R1,R0,#1" },
    { TLOC, "ADCS_Overflow", "R0=0x7FFFFFFF,Status=0", "R0=0x7FFFFFFF,R1=0x80000000,Status=NV", "ADCS R1,R0,#1" },
    { TLOC, "ADCS_Carry", "R0=0xFFFFFFFF,Status=0", "R0=0xFFFFFFFF,R1=1,Status=C", "ADCS R1,R0,#2" },
    { TLOC, "ADCS_Zero", "R0=0xFFFFFFFF,Status=0", "R0=0xFFFFFFFF,R1=0,Status=ZC", "ADCS R1,R0,#1" },
    { TLOC, "ADCS_Negative", "R0=0xFFFFFFFE,Status=0", "R0=0xFFFFFFFE,R1=0xFFFFFFFF,Status=N", "ADCS R1,R0,#1" },

    // SUB
    { TLOC, "SUB_NoFlags", "R0=2,Status=0", "R0=2,R1=1,Status=0", "SUB R1,R0,#1" },
    { TLOC, "SUB_Overflow", "R0=0x80000000,Status=0", "R0=0x80000000,R1=0x7FFFFFFF,Status=0", "SUB R1,R0,#1" },
    { TLOC, "SUB_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,R2=1,Status=0", "SUB R2,R1,R0" },
    { TLOC, "SUB_Zero", "R0=1,Status=0", "R0=1,R1=0,Status=0", "SUB R1,R0,#1" },
    { TLOC, "SUB_Negative", "R0=0,Status=0", "R0=0,R1=0xFFFFFFFF,Status=0", "SUB R1,R0,#1" },
    { TLOC, "SUBS_NoFlags", "R0=2,Status=0", "R0=2,R1=1,Status=0", "SUBS R1,R0,#1" },
    { TLOC, "SUBS_Overflow", "R0=0x80000000,Status=0", "R0=0x80000000,R1=0x7FFFFFFF,Status=V", "SUBS R1,R0,#1" },
    { TLOC, "SUBS_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,R2=1,Status=C", "SUBS R2,R1,R0" },
    { TLOC, "SUBS_Zero", "R0=1,Status=0", "R0=1,R1=0,Status=Z", "SUBS R1,R0,#1" },
    { TLOC, "SUBS_Negative", "R0=0,Status=0", "R0=0,R1=0xFFFFFFFF,Status=NC", "SUBS R1,R0,#1" },

    // RSB
    { TLOC, "RSB_NoFlags", "R0=1,Status=0", "R0=1,R1=1,Status=0", "RSB R1,R0,#2" },
    { TLOC, "RSB_Overflow", "R0=1,Status=0", "R0=1,R1=0x7FFFFFFF,Status=0", "RSB R1,R0,#&80000000" },
    { TLOC, "RSB_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,R2=1,Status=0", "RSB R2,R0,R1" },
    { TLOC, "RSB_Zero", "R0=1,Status=0", "R0=1,R1=0,Status=0", "RSB R1,R0,#1" },
    { TLOC, "RSB_Negative", "R0=1,Status=0", "R0=1,R1=0xFFFFFFFF,Status=0", "RSB R1,R0,#0" },
    { TLOC, "RSBS_NoFlags", "R0=1,Status=0", "R0=1,R1=1,Status=0", "RSBS R1,R0,#2" },
    { TLOC, "RSBS_Overflow", "R0=1,Status=0", "R0=1,R1=0x7FFFFFFF,Status=V", "RSBS R1,R0,#&80000000" },
    { TLOC, "RSBS_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,R2=1,Status=C", "RSBS R2,R0,R1" },
    { TLOC, "RSBS_Zero", "R0=1,Status=0", "R0=1,R1=0,Status=Z", "RSBS R1,R0,#1" },
    { TLOC, "RSBS_Negative", "R0=1,Status=0", "R0=1,R1=0xFFFFFFFF,Status=NC", "RSBS R1,R0,#0" },

    // SBC
    { TLOC, "SBC_NoFlagsCC", "R0=2,Status=0", "R0=2,R1=1,Status=0", "SBC R1,R0,#1" },
    { TLOC, "SBC_NoFlagsCS", "R0=3,Status=C", "R0=3,R1=1,Status=C", "SBC R1,R0,#1" },
    { TLOC, "SBC_Overflow", "R0=0x80000000,Status=0", "R0=0x80000000,R1=0x7FFFFFFF,Status=0", "SBC R1,R0,#1" },
    { TLOC, "SBC_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,R2=1,Status=0", "SBC R2,R1,R0" },
    { TLOC, "SBC_Zero", "R0=1,Status=0", "R0=1,R1=0,Status=0", "SBC R1,R0,#1" },
    { TLOC, "SBC_Negative", "R0=0,Status=0", "R0=0,R1=0xFFFFFFFF,Status=0", "SBC R1,R0,#1" },
    { TLOC, "SBCS_NoFlagsCC", "R0=2,Status=0", "R0=2,R1=1,Status=0", "SBCS R1,R0,#1" },
    { TLOC, "SBCS_NoFlagsCS", "R0=3,Status=C", "R0=3,R1=1,Status=0", "SBCS R1,R0,#1" },
    { TLOC, "SBCS_Overflow", "R0=0x80000000,Status=0", "R0=0x80000000,R1=0x7FFFFFFF,Status=V", "SBCS R1,R0,#1" },
    { TLOC, "SBCS_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,R2=1,Status=C", "SBCS R2,R1,R0" },
    { TLOC, "SBCS_Zero", "R0=1,Status=0", "R0=1,R1=0,Status=Z", "SBCS R1,R0,#1" },
    { TLOC, "SBCS_Negative", "R0=0,Status=0", "R0=0,R1=0xFFFFFFFF,Status=NC", "SBCS R1,R0,#1" },

    // RSC
    { TLOC, "RSC_NoFlagsCC", "R0=1,Status=0", "R0=1,R1=1,Status=0", "RSC R1,R0,#2" },
    { TLOC, "RSC_NoFlagsCS", "R0=1,Status=C", "R0=1,R1=1,Status=C", "RSC R1,R0,#3" },
    { TLOC, "RSC_Overflow", "R0=1,Status=0", "R0=1,R1=0x7FFFFFFF,Status=0", "RSC R1,R0,#0x80000000" },
    { TLOC, "RSC_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,R2=1,Status=0", "RSC R2,R0,R1" },
    { TLOC, "RSC_Zero", "R0=1,Status=0", "R0=1,R1=0,Status=0", "RSC R1,R0,#1" },
    { TLOC, "RSC_Negative", "R0=1,Status=0", "R0=1,R1=0xFFFFFFFF,Status=0", "RSC R1,R0,#0" },
    { TLOC, "RSCS_NoFlagsCC", "R0=1,Status=0", "R0=1,R1=1,Status=0", "RSCS R1,R0,#2" },
    { TLOC, "RSCS_NoFlagsCS", "R0=1,Status=C", "R0=1,R1=1,Status=0", "RSCS R1,R0,#3" },
    { TLOC, "RSCS_Overflow", "R0=1,Status=0", "R0=1,R1=0x7FFFFFFF,Status=V", "RSCS R1,R0,#0x80000000" },
    { TLOC, "RSCS_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,R2=1,Status=C", "RSCS R2,R0,R1" },
    { TLOC, "RSCS_Zero", "R0=1,Status=0", "R0=1,R1=0,Status=Z", "RSCS R1,R0,#1" },
    { TLOC, "RSCS_Negative", "R0=1,Status=0", "R0=1,R1=0xFFFFFFFF,Status=NC", "RSCS R1,R0,#0" },

    // CMP
    { TLOC, "CMP_NoFlags", "R0=2,Status=0", "R0=2,Status=0", "CMP R0,#1" },
    { TLOC, "CMP_Overflow", "R0=0x80000000,Status=0", "R0=0x80000000,Status=V", "CMP R0,#1" },
    { TLOC, "CMP_Carry", "R0=0xFFFFFFFF,R1=0,Status=0", "R0=0xFFFFFFFF,R1=0,Status=C", "CMP R1,R0" },
    { TLOC, "CMP_Zero", "R0=1,Status=0", "R0=1,Status=Z", "CMP R0,#1" },
    { TLOC, "CMP_Negative", "R0=0,Status=0", "R0=0,Status=NC", "CMP R0,#1" },

    // CMN
    { TLOC, "CMN_NoFlags", "R0=1,Status=0", "R0=1,Status=0", "CMN R0,#1" },
    { TLOC, "CMN_Overflow", "R0=0x7FFFFFFF,Status=0", "R0=0x7FFFFFFF,Status=NV", "CMN R0,#1" },
    { TLOC, "CMN_Carry", "R0=0xFFFFFFFF,Status=0", "R0=0xFFFFFFFF,Status=C", "CMN R0,#2" },
    { TLOC, "CMN_Zero", "R0=0xFFFFFFFF,Status=0", "R0=0xFFFFFFFF,Status=CZ", "CMN R0,#1" },
    { TLOC, "CMN_Negative", "R0=0xFFFFFFFE,Status=0", "R0=0xFFFFFFFE,Status=N", "CMN R0,#1" },
};

// Tests for ALU operations specific to configurations with combined PC/PSR.
const CoreTestParams basic26BitAlu[] = {
    { TLOC, "TSTP_Flags", "R0=0xF0000000,CPSR=0xF0000000", "R0=0xF0000000,CPSR=0xA0000000", "%26bit : TSTP R0,#&AC000003" },
    { TLOC, "TEQP_Flags", "R1=0xF0000000,CPSR=0x40000000", "R1=0xF0000000,CPSR=0xB0000000", "%26bit : TEQP R1,PC" },
    { TLOC, "CMPP_NoFlags", "R0=0,CPSR=0", "R0=0,CPSR=0", "%26bit : CMPP R0,#0" },
    { TLOC, "CMPP_Flags", "R0=0xF0000000,CPSR=0", "R0=0xF0000000,CPSR=0xA0000000", "%26bit : CMPP R0,#0x50000000" },
    { TLOC, "CMNP_NoFlags", "R0=0,CPSR=0", "R0=0,CPSR=0", "%26bit : CMNP R0,#0" },
    { TLOC, "CMNP_Flags", "R0=0x10000000,CPSR=0", "R0=0x10000000,CPSR=0x90000000", "%26bit : CMNP R0,#0x80000000" },
};

} // Anonymous namespace

//! @brief Registers ALU unit tests against each type of emulated system they
//! should be tested on.
void registerAluExecTests()
{
    // Register test points against basic ARMv2 implementation.
    RegisterExecTests<ArmV2TestSystemTraits>("ARMv2_ALU", basicAlu, std::size(basicAlu));
    RegisterExecTests<ArmV2TestSystemTraits>("ARMv2_ALU", basic26BitAlu, std::size(basic26BitAlu));

    // Repeat tests for the ARM 3 core.
    RegisterExecTests<ArmV2aTestSystemTraits>("ARMv2a_ALU", basicAlu, std::size(basicAlu));
    RegisterExecTests<ArmV2aTestSystemTraits>("ARMv2a_ALU", basic26BitAlu, std::size(basic26BitAlu));
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


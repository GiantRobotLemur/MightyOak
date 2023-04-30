//! @file Test_RegisterFile.cpp
//! @brief The definition of unit tests of RegisterFile-based templates.
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

#include "TestTools.hpp"

#include "Hardware.inl"
#include "ARMv2CoreRegisterFile.inl"

using namespace Ag::Arm;

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
struct BasicHardware
{
    uint8_t IrqMask;
    bool IsPrivilagedMode;

    BasicHardware() :
        IrqMask(0),
        IsPrivilagedMode(false)
    {
    }

    void updateIrqMask(uint8_t mask, uint8_t significantBits) noexcept
    {
        IrqMask = (IrqMask & ~significantBits) | (mask & significantBits);
    }

    void setPrivilegedMode(bool isPrivilaged) noexcept
    {
        IsPrivilagedMode = isPrivilaged;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
template<typename T>
class RegisterFile26 : public testing::Test
{
public:
};

// Explicitly instantiate the whole class to ensure it at least compiles.
template class Ag::Arm::ARMv2CoreRegisterFile<::BasicHardware>;

struct ARMv2RegisterTraits
{
    using Hardware = ::BasicHardware;
    using RegisterFile = ARMv2CoreRegisterFile<::BasicHardware>;
};

using RegisterFile26Implementations = testing::Types<ARMv2RegisterTraits>;

TYPED_TEST_SUITE(RegisterFile26, RegisterFile26Implementations);

TYPED_TEST(RegisterFile26, Reset)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to UsrMode26 at 32K, IRQs enabled.
    specimen.setPSR(0x0);
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseReset();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::PipelineChange);
    EXPECT_TRUE(isEqualHex(specimen.getPC(), 0x00000000u));
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008000u));
    EXPECT_TRUE(isEqualHex(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBits));
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_TRUE(isEqualHex(platform.IrqMask, 0x03));
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, ResetNoModeChange)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to SvcMode26 at 32K, IRQs enabled.
    specimen.setPSR(Ag::toScalar(ProcessorMode::Svc26));
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseReset();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::FlushPipeline);
    EXPECT_TRUE(isEqualHex(specimen.getPC(), 0x00000000u));
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008003u));
    EXPECT_TRUE(isEqualHex(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBits));
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_TRUE(isEqualHex(platform.IrqMask, 0x03));
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, UndefinedInstruction)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to UsrMode26 at 32K, IRQs enabled.
    specimen.setPSR(0x0);
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseUndefinedInstruction();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::PipelineChange);
    EXPECT_EQ(specimen.getPC(), 0x00000004u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008000u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, UndefinedInstructionNoModeChange)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to SvcMode26 at 32K, IRQs enabled.
    specimen.setPSR(Ag::toScalar(ProcessorMode::Svc26));
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseUndefinedInstruction();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::FlushPipeline);
    EXPECT_EQ(specimen.getPC(), 0x00000004u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008003u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, SoftwareInterrupt)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to UsrMode26 at 32K, IRQs enabled.
    specimen.setPSR(0x0);
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseSoftwareInterrupt();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::PipelineChange);
    EXPECT_EQ(specimen.getPC(), 0x00000008u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008000u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, SoftwareInterruptNoModeChange)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to SvcMode26 at 32K, IRQs enabled.
    specimen.setPSR(Ag::toScalar(ProcessorMode::Svc26));
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseSoftwareInterrupt();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::FlushPipeline);
    EXPECT_EQ(specimen.getPC(), 0x00000008u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008003u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, PreFetchAbort)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to UsrMode26 at 32K, IRQs enabled.
    specimen.setPSR(0x0);
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raisePreFetchAbort();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::PipelineChange);
    EXPECT_EQ(specimen.getPC(), 0x0000000Cu);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008000u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, PreFetchAbortNoModeChange)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to SvcMode26 at 32K, IRQs enabled.
    specimen.setPSR(Ag::toScalar(ProcessorMode::Svc26));
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raisePreFetchAbort();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::FlushPipeline);
    EXPECT_EQ(specimen.getPC(), 0x0000000Cu);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008003u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, DataAbort)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to UsrMode26 at 32K, IRQs enabled.
    specimen.setPSR(0x0);
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseDataAbort();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::PipelineChange);
    EXPECT_EQ(specimen.getPC(), 0x00000010u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008000u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, DataAbortNoModeChange)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to SvcMode26 at 32K, IRQs enabled.
    specimen.setPSR(Ag::toScalar(ProcessorMode::Svc26));
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseDataAbort();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::FlushPipeline);
    EXPECT_EQ(specimen.getPC(), 0x00000010u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008003u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, AddressException)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to UsrMode26 at 32K, IRQs enabled.
    specimen.setPSR(0x0);
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseAddressException();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::PipelineChange);
    EXPECT_EQ(specimen.getPC(), 0x00000014u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008000u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, AddressExceptionNoModeChange)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to SvcMode26 at 32K, IRQs enabled.
    specimen.setPSR(Ag::toScalar(ProcessorMode::Svc26));
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.raiseAddressException();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::FlushPipeline);
    EXPECT_EQ(specimen.getPC(), 0x00000014u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008003u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, HandleInterrupt)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to UsrMode26 at 32K, IRQs enabled.
    specimen.setPSR(0x0);
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.handleIrq();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::PipelineChange);
    EXPECT_EQ(specimen.getPC(), 0x00000018u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008000u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Irq26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, HandleInterruptNoModeChange)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to SvcMode26 at 32K, IRQs enabled.
    specimen.setPSR(Ag::toScalar(ProcessorMode::Irq26));
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.handleIrq();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::FlushPipeline);
    EXPECT_EQ(specimen.getPC(), 0x00000018u);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008002u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBit);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Irq26);
    EXPECT_EQ(platform.IrqMask, IrqState::IrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, HandleFastInterrupt)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to UsrMode26 at 32K, IRQs enabled.
    specimen.setPSR(0x0);
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.handleFirq();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::PipelineChange);
    EXPECT_EQ(specimen.getPC(), 0x0000001Cu);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008000u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBits);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::FastIrq26);
    EXPECT_EQ(platform.IrqMask, IrqState::GuestIrqsMask);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, HandleFastInterruptNoModeChange)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to SvcMode26 at 32K, IRQs enabled.
    specimen.setPSR(Ag::toScalar(ProcessorMode::FastIrq26));
    specimen.setPC(0x8000);

    uint32_t resetResult = specimen.handleFirq();

    EXPECT_EQ(resetResult & ExecResult::PipelineChange, ExecResult::FlushPipeline);
    EXPECT_EQ(specimen.getPC(), 0x0000001Cu);
    EXPECT_TRUE(isEqualHex(specimen.getRn(GeneralRegister::R14), 0x00008001u));
    EXPECT_EQ(specimen.getPSR() & PsrMask26::IrqDisableBits, PsrMask26::IrqDisableBits);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::FastIrq26);
    EXPECT_EQ(platform.IrqMask, IrqState::GuestIrqsMask);
    EXPECT_TRUE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, GetPSR)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled.
    specimen.raiseReset();

    EXPECT_EQ(specimen.getPSR(), 0x0C000003u);
}

TYPED_TEST(RegisterFile26, SetPSR)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled.
    specimen.raiseReset();

    // Set FIRQs disabled, Carry, oVerflow and Firq26 mode.
    EXPECT_EQ(specimen.setPSR(0x34000001), ExecResult::ModeChange);

    EXPECT_EQ(specimen.getPSR(), 0x34000001u);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::FastIrq26);
    EXPECT_EQ(platform.IrqMask, IrqState::FastIrqPending);
    EXPECT_TRUE(platform.IsPrivilagedMode);

    // Set User26 mode, Negative, IRQs enabled.
    EXPECT_EQ(specimen.setPSR(0x80000000), ExecResult::ModeChange);
    EXPECT_EQ(specimen.getMode(), ProcessorMode::User26);
    EXPECT_EQ(platform.IrqMask, 0u);
    EXPECT_FALSE(platform.IsPrivilagedMode);

    // Set User26 mode, Zero.
    EXPECT_EQ(specimen.setPSR(0x40000000), 0u);
    EXPECT_EQ(platform.IrqMask, 0u);
    EXPECT_FALSE(platform.IsPrivilagedMode);
}

TYPED_TEST(RegisterFile26, SetStatusFlags)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled.
    specimen.raiseReset();

    // Confirm the base state.
    uint32_t baseState = specimen.getPSR();
    EXPECT_EQ(baseState & PsrMask::Status, 0u);

    // Alter the status flag state.
    specimen.setStatusFlags(PsrMask::LowCarry | PsrMask::LowZero | 0xC0);

    // Verify only the status flags changed.
    EXPECT_EQ(specimen.getPSR(),
              baseState | PsrMask::Carry | PsrMask::Zero);
}

TYPED_TEST(RegisterFile26, UpdatePSR)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled.
    specimen.raiseReset();

    // Verify state.
    EXPECT_EQ(specimen.getPSR(), 0x0C000003u);

    // Update privileged and status bits (no mode change).
    EXPECT_EQ(specimen.updatePSR(PsrMask::Overflow |
                                 Ag::toScalar(ProcessorMode::Svc26)), 0u);

    // Verify updated state.
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(specimen.getPSR(), 0x10000003u);

    // Switch to user mode.
    EXPECT_EQ(specimen.updatePSR(Ag::toScalar(ProcessorMode::User26)),
              ExecResult::ModeChange);

    // Verify new state.
    EXPECT_EQ(specimen.getMode(), ProcessorMode::User26);
    EXPECT_EQ(specimen.getPSR(), 0u);

    // Update privileged and status bits (no mode change allowed).
    EXPECT_EQ(specimen.updatePSR(PsrMask::Overflow |
                                 Ag::toScalar(ProcessorMode::Svc26)), 0u);

    // Verify status flag set, but mode not changed.
    EXPECT_EQ(specimen.getPSR(),
              PsrMask::Overflow | Ag::toScalar(ProcessorMode::User26));
}

TYPED_TEST(RegisterFile26, GetAndSetPC)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled.
    specimen.raiseReset();

    // Verify base state (at the reset vector).
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(specimen.getPC(), 0u);

    // Try to set PC, status flags and mode.
    specimen.setPC(0x8000 | PsrMask::Status | Ag::toScalar(ProcessorMode::Irq26));

    // Verify only the PC was set.
    EXPECT_EQ(specimen.getMode(), ProcessorMode::Svc26);
    EXPECT_EQ(specimen.getPC(), 0x8000u);
}

TYPED_TEST(RegisterFile26, GetRn)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled, R4 = &DEADBEEF, PC = 0x8000
    specimen.raiseReset();
    specimen.setRn(GeneralRegister::R4, 0xDEADBEEF);
    specimen.setPC(0x8000);
    specimen.setStatusFlags(PsrMask::LowOverflow);

    // Verify getting non-R15 returns the original value.
    EXPECT_EQ(specimen.getRn(GeneralRegister::R4), 0xDEADBEEFu);

    // Verify getting R15 returns the PC-only.
    EXPECT_EQ(specimen.getRn(GeneralRegister::R15), 0x8000u);
}

TYPED_TEST(RegisterFile26, SetRn)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled, R4 = &DEADBEEF, PC = 0x8000
    specimen.raiseReset();
    specimen.setRn(GeneralRegister::R4, 0xDEADBEEF);
    specimen.setPC(0x8000);
    specimen.setStatusFlags(PsrMask::LowOverflow);

    // Verify assigning to R15 only changes the PC, no mode change.
    EXPECT_EQ(specimen.setRn(GeneralRegister::R15, 0x1000 | PsrMask::Carry),
              ExecResult::FlushPipeline);

    // Verify getting non-R15 returns the original value.
    EXPECT_EQ(specimen.getRn(GeneralRegister::R4), 0xDEADBEEFu);

    // Verify getting R15 returns the PC-only.
    EXPECT_EQ(specimen.getPSR(),
              PsrMask::Overflow | Ag::toScalar(ProcessorMode::Svc26) |
              PsrMask26::IrqDisableBits);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R15), 0x1000u);
}

TYPED_TEST(RegisterFile26, GetUserRn)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Usr26, IRQs enabled, R4 = &DEADBEEF, R13 = 0xCAFEBABE
    specimen.raiseReset();
    specimen.setPSR(Ag::toScalar(ProcessorMode::User26));
    specimen.setRn(GeneralRegister::R7, 0xDEADBEEF);
    specimen.setRn(GeneralRegister::R8, 0x42694269u);
    specimen.setRn(GeneralRegister::R14, 0xCAFEBABE);

    // Verify base state.
    EXPECT_EQ(specimen.getMode(), ProcessorMode::User26);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R7), 0xDEADBEEF);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R8), 0x42694269u);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R14), 0xCAFEBABE);

    // Verify user register state.
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R7), 0xDEADBEEF);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R8), 0x42694269u);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R14), 0xCAFEBABE);

    // Switch mode.
    EXPECT_EQ(specimen.handleFirq(), ExecResult::PipelineChange);

    // Verify differences in banked register contents while in User mode.
    EXPECT_EQ(specimen.getRn(GeneralRegister::R7), 0xDEADBEEF);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R8), 0u);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R14), 0u);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R7), 0xDEADBEEF);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R8), 0x42694269u);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R14), 0xCAFEBABE);
}

TYPED_TEST(RegisterFile26, SetUserRn)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Firq26 mode, IRQs disabled, R4 = &DEADBEEF, R13 = 0xCAFEBABE
    specimen.raiseReset();
    specimen.setPSR(Ag::toScalar(ProcessorMode::FastIrq32));
    specimen.setUserRn(GeneralRegister::R7, 0xDEADBEEF);
    specimen.setUserRn(GeneralRegister::R8, 0x42694269);
    specimen.setUserRn(GeneralRegister::R14, 0xCAFEBABE);

    // Verify base state.
    EXPECT_EQ(specimen.getMode(), ProcessorMode::FastIrq26);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R7), 0xDEADBEEF);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R8), 0u);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R14), 0u);

    // Verify differences in banked register contents while not in User mode.
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R7), 0xDEADBEEF);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R8), 0x42694269u);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R14), 0xCAFEBABE);

    // Switch to user mode.
    EXPECT_EQ(specimen.setPSR(Ag::toScalar(ProcessorMode::User26)),
              ExecResult::ModeChange);

    // Verify state from user mode.
    EXPECT_EQ(specimen.getMode(), ProcessorMode::User26);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R7), 0xDEADBEEF);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R8), 0x42694269u);
    EXPECT_EQ(specimen.getRn(GeneralRegister::R14), 0xCAFEBABE);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R7), 0xDEADBEEF);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R8), 0x42694269u);
    EXPECT_EQ(specimen.getUserRn(GeneralRegister::R14), 0xCAFEBABE);
}

TYPED_TEST(RegisterFile26, GetRm)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled, R4 = &DEADBEEF, PC = 0x8000
    specimen.raiseReset();
    specimen.setRn(GeneralRegister::R4, 0xDEADBEEF);
    specimen.setPC(0x8000);
    specimen.setStatusFlags(PsrMask::LowOverflow);

    // Verify getting non-R15 returns the original value.
    EXPECT_EQ(specimen.getRm(GeneralRegister::R4), 0xDEADBEEFu);

    // Verify getting R15 returns the PC and PSR.
    EXPECT_EQ(specimen.getRm(GeneralRegister::R15),
              0x8000u | Ag::toScalar(ProcessorMode::Svc26) |
              PsrMask::Overflow | PsrMask26::IrqDisableBits);
}

TYPED_TEST(RegisterFile26, GetRs)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled, R4 = &DEADBEEF, PC = 0x8000
    specimen.raiseReset();
    specimen.setRn(GeneralRegister::R4, 0xDEADBEEF);
    specimen.setPC(0x8000);
    specimen.setStatusFlags(PsrMask::LowOverflow);

    // Verify getting non-R15 returns the original value.
    EXPECT_EQ(specimen.getRs(GeneralRegister::R4), 0xDEADBEEFu);

    // Verify getting R15 returns the PC + 4 only.
    EXPECT_EQ(specimen.getRs(GeneralRegister::R15), 0x8004u);
}

TYPED_TEST(RegisterFile26, GetRd)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled, R4 = &DEADBEEF, PC = 0x8000
    specimen.raiseReset();
    specimen.setRn(GeneralRegister::R4, 0xDEADBEEF);
    specimen.setPC(0x8000);
    specimen.setStatusFlags(PsrMask::LowOverflow);

    // Verify getting non-R15 returns the original value.
    EXPECT_EQ(specimen.getRd(GeneralRegister::R4), 0xDEADBEEFu);

    // Verify getting R15 returns the PSR only.
    EXPECT_EQ(specimen.getRd(GeneralRegister::R15),
              Ag::toScalar(ProcessorMode::Svc26) |
              PsrMask::Overflow | PsrMask26::IrqDisableBits);
}

//TYPED_TEST(RegisterFile26, SetRdNoStatusUpdatePrivilaged)
//{
//    typename TypeParam::Hardware platform;
//    typename TypeParam::RegisterFile specimen(platform);
//
//    // Set to Srv26, IRQs disabled, R4 = &DEADBEEF, PC = 0x8000
//    specimen.raiseReset();
//    specimen.setPC(0x8000);
//    specimen.setStatusFlags(PsrMask::LowOverflow);
//
//    // Verify the base state.
//    EXPECT_EQ(specimen.getPC(), 0x8000u);
//    EXPECT_EQ(specimen.getPSR(),
//              PsrMask::Overflow | PsrMask26::IrqDisableBits |
//              Ag::toScalar(ProcessorMode::Svc26));
//
//    // Set a non-R15 register.
//    EXPECT_EQ(specimen.setRd(GeneralRegister::R2, 0xDEADBEEF, false), 0u);
//
//    // Verify the register was set and PC/PSR unchanged..
//    EXPECT_EQ(specimen.getRn(GeneralRegister::R2), 0xDEADBEEF);
//    EXPECT_EQ(specimen.getPC(), 0x8000u);
//    EXPECT_EQ(specimen.getPSR(),
//              PsrMask::Overflow | PsrMask26::IrqDisableBits |
//              Ag::toScalar(ProcessorMode::Svc26));
//
//    // Set an R15 value.
//    EXPECT_EQ(specimen.setRd(GeneralRegister::R15,
//                             0x1000 | PsrMask::Carry | Ag::toScalar(ProcessorMode::FastIrq26),
//                             false), ExecResult::FlushPipeline);
//
//    // Verify only the PC was updated.
//    EXPECT_EQ(specimen.getPC(), 0x1000u);
//    EXPECT_EQ(specimen.getPSR(),
//              PsrMask::Overflow | PsrMask26::IrqDisableBits |
//              Ag::toScalar(ProcessorMode::Svc26));
//}

//TYPED_TEST(RegisterFile26, SetRdNoStatusUpdateNonPrivilaged)
//{
//    typename TypeParam::Hardware platform;
//    typename TypeParam::RegisterFile specimen(platform);
//
//    // Set to User26, IRQs enabled, R4 = &DEADBEEF, PC = 0x8000
//    specimen.raiseReset();
//    specimen.setPC(0x8000);
//    specimen.setPSR(Ag::toScalar(ProcessorMode::User26) | PsrMask::Overflow);
//
//    // Verify the base state.
//    EXPECT_EQ(specimen.getPC(), 0x8000u);
//    EXPECT_EQ(specimen.getPSR(),
//              PsrMask::Overflow | Ag::toScalar(ProcessorMode::User26));
//
//    // Set a non-R15 register.
//    EXPECT_EQ(specimen.setRd(GeneralRegister::R2, 0xDEADBEEF, false), 0u);
//
//    // Verify the register was set and PC/PSR unchanged..
//    EXPECT_EQ(specimen.getRn(GeneralRegister::R2), 0xDEADBEEF);
//    EXPECT_EQ(specimen.getPC(), 0x8000u);
//    EXPECT_EQ(specimen.getPSR(),
//              PsrMask::Overflow | Ag::toScalar(ProcessorMode::User26));
//
//    // Set an R15 value.
//    EXPECT_EQ(specimen.setRd(GeneralRegister::R15,
//                             0x1000 | PsrMask::Carry | Ag::toScalar(ProcessorMode::FastIrq26),
//                             false), ExecResult::FlushPipeline);
//
//    // Verify only the PC was updated.
//    EXPECT_EQ(specimen.getPC(), 0x1000u);
//    EXPECT_EQ(specimen.getPSR(),
//              PsrMask::Overflow | Ag::toScalar(ProcessorMode::User26));
//}


TYPED_TEST(RegisterFile26, SetRdPsrUpdatePrivilaged)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled, R4 = &DEADBEEF, PC = 0x8000
    specimen.raiseReset();
    specimen.setPC(0x8000);
    specimen.setStatusFlags(PsrMask::LowOverflow);

    // Verify the base state.
    EXPECT_EQ(specimen.getPC(), 0x8000u);
    EXPECT_EQ(specimen.getPSR(),
              PsrMask::Overflow | PsrMask26::IrqDisableBits |
              Ag::toScalar(ProcessorMode::Svc26));

    // Set a non-R15 register with status flag update.
    EXPECT_EQ(specimen.setRdAndStatus(GeneralRegister::R2, 0xDEADBEEF, PsrMask::LowCarry), 0u);

    // Verify the register was set and PC/PSR unchanged.
    EXPECT_EQ(specimen.getRn(GeneralRegister::R2), 0xDEADBEEF);
    EXPECT_EQ(specimen.getPC(), 0x8000u);
    EXPECT_EQ(specimen.getPSR(),
              PsrMask::Carry | PsrMask26::IrqDisableBits |
              Ag::toScalar(ProcessorMode::Svc26));

    // Set an R15 value, update status flags from value not status.
    EXPECT_EQ(specimen.setRdAndStatus(GeneralRegister::R15,
                                      0x1000 | PsrMask::Carry | Ag::toScalar(ProcessorMode::FastIrq26),
                                      PsrMask::LowOverflow),
              ExecResult::FlushPipeline | ExecResult::ModeChange);

    // Verify PC and all parts of the PSR were updated.
    EXPECT_EQ(specimen.getPC(), 0x1000u);
    EXPECT_EQ(specimen.getPSR(),
              PsrMask::Carry | Ag::toScalar(ProcessorMode::FastIrq26));
}

TYPED_TEST(RegisterFile26, SetRdPsrUpdateNonPrivilaged)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to User26, IRQs enabled, R4 = &DEADBEEF, PC = 0x8000
    specimen.raiseReset();
    specimen.setPC(0x8000);
    specimen.setPSR(Ag::toScalar(ProcessorMode::User26) | PsrMask::Overflow);

    // Verify the base state.
    EXPECT_EQ(specimen.getPC(), 0x8000u);
    EXPECT_EQ(specimen.getPSR(),
              PsrMask::Overflow | Ag::toScalar(ProcessorMode::User26));

    // Set a non-R15 register.
    EXPECT_EQ(specimen.setRdAndStatus(GeneralRegister::R2, 0xDEADBEEF, PsrMask::LowCarry), 0u);

    // Verify the register was set and only status flags updated.
    EXPECT_EQ(specimen.getRn(GeneralRegister::R2), 0xDEADBEEF);
    EXPECT_EQ(specimen.getPC(), 0x8000u);
    EXPECT_EQ(specimen.getPSR(),
              PsrMask::Carry | Ag::toScalar(ProcessorMode::User26));

    // Set an R15 value and update status from value not status.
    EXPECT_EQ(specimen.setRdAndStatus(GeneralRegister::R15,
                                      0x1000 | PsrMask::Negative | Ag::toScalar(ProcessorMode::FastIrq26),
                                      PsrMask::LowCarry),
              ExecResult::FlushPipeline);

    // Verify the PC and only user-accessible portions of the PSR were updated.
    EXPECT_EQ(specimen.getPC(), 0x1000u);
    EXPECT_EQ(specimen.getPSR(),
              PsrMask::Negative | Ag::toScalar(ProcessorMode::User26));
}

TYPED_TEST(RegisterFile26, GetRx)
{
    typename TypeParam::Hardware platform;
    typename TypeParam::RegisterFile specimen(platform);

    // Set to Srv26, IRQs disabled, R4 = &DEADBEEF, PC = 0x8000
    specimen.raiseReset();
    specimen.setRn(GeneralRegister::R4, 0xDEADBEEF);
    specimen.setPC(0x8000);
    specimen.setStatusFlags(PsrMask::LowOverflow);

    // Verify getting non-R15 returns the original value.
    EXPECT_EQ(specimen.getRx(GeneralRegister::R4), 0xDEADBEEFu);

    // Verify getting R15 returns the PC + 4 and PSR.
    EXPECT_EQ(specimen.getRx(GeneralRegister::R15),
              0x8004u | Ag::toScalar(ProcessorMode::Svc26) |
              PsrMask::Overflow | PsrMask26::IrqDisableBits);
}

// TODO: Perform some more checks on banked register switching.

////////////////////////////////////////////////////////////////////////////////


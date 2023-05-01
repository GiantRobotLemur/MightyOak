//! @file ArmEmu/TestTools.cpp
//! @brief The definition of various tools used in unit tests for the ARM
//! processor emulator.
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

#include "Ag/Core/Format.hpp"
#include "Ag/Core/Variant.hpp"

#include "ArmSystem.inl"
#include "SystemConfigurations.inl"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {
namespace Arm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct GenerateBreakPoint
{
private:
    Asm::InstructionInfo _instruction;
    uint16_t _index;
public:
    GenerateBreakPoint() :
        _index(0)
    {
        _instruction.reset(Asm::InstructionMnemonic::Bkpt,
                           Asm::OperationClass::Breakpoint);
    }

    uint32_t operator()()
    {
        _instruction.getBreakpointParameters().Comment = _index++;

        Ag::String error;
        uint32_t instruction = 0;
        if (_instruction.assemble(instruction, 0x0000, error) == false)
        {
            throw Ag::OperationException("Could not assemble numbered breakpoint.");
        }

        return instruction;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////


} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
::testing::AssertionResult isEqualHex(uint8_t lhs, uint8_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X2} vs 0x{1:X2}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualHex(uint16_t lhs, uint16_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X4} vs 0x{1:X4}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualHex(uint32_t lhs, uint32_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X8} vs 0x{1:X8}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualHex(uint64_t lhs, uint64_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "0x{0:X8} vs 0x{1:X8}",
                         message, { lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualReg(CoreRegister regId, uint32_t lhs, uint32_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "{0}: 0x{1:X8} vs 0x{2:X8}",
                         message, { coreRegisterToString(regId), lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

::testing::AssertionResult isEqualMemWord(uint32_t addr, uint32_t lhs, uint32_t rhs)
{
    if (lhs == rhs)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        std::string message;
        Ag::appendFormat(Ag::FormatInfo::getNeutral(),
                         "MEM 0x{0:X6}: 0x{1:X8} vs 0x{2:X8}",
                         message, { addr, lhs, rhs });

        return ::testing::AssertionFailure() << message;
    }
}

//! @brief Constructs an implementation of an object representing an emulation
//! of an ARM-based system for testing to run specified code assembled at the
//! 32KB address mark in user mode.
//! @param[in] assembler The assembly language source code to assemble into
//! the RAM at 32KB and run up to the first breakpoint.
IArmSystemUPtr createUserModeTestSystem(const char *assembler)
{
    // Assemble code into emulated RAM.
    Asm::Options opts;
    opts.setLoadAddress(TestBedHardware::RamBase);
    opts.setInstructionSet(Asm::InstructionSet::ArmV4);

    std::string source;
    source.append("%26bit\n");
    source.append("TSTP PC,#0\n"); // Force a switch to User mode with IRQs enabled.
    source.append(assembler);

    Asm::Messages log;
    Asm::ObjectCode ramObjectCode = Asm::assembleText(source, opts, log);

    if (log.hasErrors())
    {
        std::string builder;

        for (const auto &msg : log.getMessages())
        {
            if (builder.empty() == false)
            {
                builder.push_back('\n');
            }

            appendAgString(builder, msg.toString());
        }

        throw Ag::CustomException("Assembly", "Could not assemble test code.",
                                  builder);
    }

    // Create a ROM image filled with breakpoints.
    std::vector<uint32_t> rom;

    std::generate_n(std::back_inserter(rom),
                    TestBedHardware::RomSize / 4,
                    GenerateBreakPoint());

    // Create an instruction which at the hardware reset vector which
    // branches to the first work in memory.
    Asm::InstructionInfo resetBranch(Asm::InstructionMnemonic::B,
                                     Asm::OperationClass::Branch);
    resetBranch.getBranchParameters().Address = TestBedHardware::RamBase;

    Ag::String error;
    if (resetBranch.assemble(rom.front(), TestBedHardware::RomBase, error) == false)
    {
        throw Ag::OperationException("Could not assemble reset vector.");
    }

    //TestSystem *testSystem = new TestSystem();
    auto *testSystem = new ArmSystem<ArmV2TestSystemTraits>();

    // Fill the ROM with breakpoints and a branch to RAM on reset.
    testSystem->writeToLogicalAddress(TestBedHardware::RomBase,
                                      static_cast<uint32_t>(rom.size() * 4),
                                      rom.data());

    // Copy the assembled code into RAM.
    testSystem->writeToLogicalAddress(TestBedHardware::RamBase,
                                      static_cast<uint32_t>(ramObjectCode.getCodeSize()),
                                      ramObjectCode.getCode());

    // Assemble a breakpoint at the end of the program.
    resetBranch.reset(Asm::InstructionMnemonic::Bkpt,
                      Asm::OperationClass::Breakpoint);
    resetBranch.getBreakpointParameters().Comment = 0xFFFF;

    uint32_t bkptInstruction;
    if (resetBranch.assemble(bkptInstruction, 0x0000, error) == false)
    {
        throw Ag::OperationException("Could not assemble final break point.");
    }

    // Write the final breakpoint to RAM.
    testSystem->writeToLogicalAddress(static_cast<uint32_t>(TestBedHardware::RamBase + ramObjectCode.getCodeSize()),
                                      4, &bkptInstruction);

    // Return a unique pointer to the emulated system.
    return IArmSystemUPtr(testSystem);
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


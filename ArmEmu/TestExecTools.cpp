//! @file ArmEmu/TestExecTools.cpp
//! @brief The definition of tools which allow execution of instructions on
//! various configurations of emulated system configurations for the purposes
//! of validation.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "TestExecTools.hpp"
#include "AsmTools.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Prepares an emulated system for testing.
//! @param[in] systemUnderTest The system to prepare, which is assumed to have
//! a test bed memory map of 32KB ROM and 32 KB RAM.
//! @param[in] source The assembly language source code to assemble and install
//! in the test system ROM.
//! @details The ROM is filled with sequential break point instructions. But
//! the reset vector is encoded with a branch to the first byte of assembled code
//! installed in the RAM. The system is switched to user mode before being
//! returned.
::testing::AssertionResult prepareTestSystem(IArmSystem *systemUnderTest,
                                             const std::string_view &source)
{
    Asm::Options opts;
    opts.setLoadAddress(TestBedHardware::RamBase);
    opts.setInstructionSet(Asm::InstructionSet::ArmV4);

    std::string sourceCode(source);

    // Append a break point to the end of the code.
    sourceCode.append("\nBKPT 0xFFFF\n");

    Asm::Messages log;
    Asm::ObjectCode objectCode = Asm::assembleText(sourceCode, opts, log);

    if (log.hasErrors())
    {
        ::testing::AssertionResult result = ::testing::AssertionFailure() <<
            "Failed to assemble test instructions:\n";

        for (const auto &msg : log.getMessages())
        {
            result << msg.toString().getUtf8Bytes() << '\n';
        }

        return result;
    }
    else
    {
        // Install the code in the RAM of the test system.
        systemUnderTest->writeToLogicalAddress(TestBedHardware::RamBase,
                                               static_cast<uint32_t>(objectCode.getCodeSize()),
                                               objectCode.getCode());

        // Fill the ROM with sequential break point instructions.
        Ag::String error;
        Asm::InstructionInfo bkptInstruction;
        bkptInstruction.reset(Asm::InstructionMnemonic::Bkpt,
                              Asm::OperationClass::Breakpoint);
        auto &bkptInfo = bkptInstruction.getBreakpointParameters();
        uint32_t op;

        for (uint32_t romAddr = TestBedHardware::RomBase;
             romAddr < TestBedHardware::RomEnd; romAddr += 4)
        {
            bkptInfo.Comment = static_cast<uint16_t>(romAddr >> 2);

            if (bkptInstruction.assemble(op, romAddr, error))
            {
                // Write the breakpoint instruction into the ROM.
                systemUnderTest->writeToLogicalAddress(romAddr, 4, &op);
            }
            else
            {
                std::string message = "Failed to assemble BKPT instruction: ";
                Ag::appendAgString(message, error);

                return ::testing::AssertionFailure() << message;
            }
        }

        // Assemble a branch instruction at the reset vector to jump
        // to the code we want to run.
        Asm::InstructionInfo resetBranchInstruction;
        resetBranchInstruction.reset(Asm::InstructionMnemonic::B,
                                     Asm::OperationClass::Branch);
        auto &branchInfo = resetBranchInstruction.getBranchParameters();
        branchInfo.Address = TestBedHardware::RamBase;

        if (resetBranchInstruction.assemble(op, 0x0000, error))
        {
            // Write the breakpoint instruction into the ROM.
            systemUnderTest->writeToLogicalAddress(0x0000, 4, &op);
        }
        else
        {
            std::string message = "Failed to assemble reset branch instruction: ";
            Ag::appendAgString(message, error);

            return ::testing::AssertionFailure() << message;
        }

        // Switch to 26-bit or 32-bit user mode.
        systemUnderTest->setCoreRegister(CoreRegister::CPSR, 0);

        return ::testing::AssertionSuccess();
    }
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////


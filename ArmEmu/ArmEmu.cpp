//! @file ArmEmu/ArmEmu.cpp
//! @brief The definition of public functions provided by the ArmEmu library.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iterator>

#include "Ag/Core/Utils.hpp"
#include "ArmEmu.hpp"
#include "AsmTools.hpp"

#include "TestSystem.hpp"

namespace Ag {
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

        String error;
        uint32_t instruction = 0;
        if (_instruction.assemble(instruction, 0x0000, error) == false)
        {
            throw Ag::OperationException("Could not assemble numbered breakpoint.");
        }

        return instruction;
    }
};

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an implementation of an object representing an emulation
//! of an ARM-based system for testing to run specified code assembled at the
//! 32KB address mark in user mode.
//! @param[in] assembler The assembly language source code to assemble into
//! the RAM at 32KB and run up to the first breakpoint.
IArmSystemUPtr createUserModeTestSystem(const char *assembler)
{
    // Assemble code into emulated RAM.
    Asm::Options opts;
    opts.setLoadAddress(Hardware::PhysicalRamBase);
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
                    Hardware::PhysicalRamBase / 4,
                    GenerateBreakPoint());

    // Create an instruction which at the hardware reset vector which
    // branches to the first work in memory.
    Asm::InstructionInfo resetBranch(Asm::InstructionMnemonic::B,
                                     Asm::OperationClass::Branch);
    resetBranch.getBranchParameters().Address = Hardware::PhysicalRamBase;

    String error;
    if (resetBranch.assemble(rom.front(), 0x0000, error) == false)
    {
        throw Ag::OperationException("Could not assemble reset vector.");
    }

    TestSystem *testSystem = new TestSystem();

    // Fill the ROM with breakpoints and a branch to RAM on reset.
    testSystem->getResources().loadMainRom(rom.data(),
                                           static_cast<uint32_t>(rom.size() * 4));

    // Copy the assembled code into RAM.
    std::memcpy(testSystem->getResources().getRam(),
                ramObjectCode.getCode(),
                ramObjectCode.getCodeSize());

    // Assemble a breakpoint at the end of the program.
    resetBranch.reset(Asm::InstructionMnemonic::Bkpt,
                      Asm::OperationClass::Breakpoint);
    resetBranch.getBreakpointParameters().Comment = 0xFFFF;

    if (resetBranch.assemble(*reinterpret_cast<uint32_t *>(testSystem->getResources().getRam() + ramObjectCode.getCodeSize()),
                             0x0000, error) == false)
    {
        throw Ag::OperationException("Could not assemble final break point.");
    }

    // Perform a hardware reset.
    testSystem->getProcessor().reset();

    // Return a unique pointer to the emulated system.
    return std::unique_ptr<IArmSystem>(testSystem);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Gets text identifying a CoreRegister value.
//! @param[in] regId The identifier of the register to get display text for.
//! @return The display text representing the register.
const char *coreRegisterToString(CoreRegister regId)
{
    static const char *coreRegIds[] = {
        "R0", "R1", "R2", "R3",
        "R4", "R5", "R6", "R7",
        "R8", "R9", "R10", "R11",
        "R12", "R13", "R14", "R15",
        "SPSR", "CPSR", "PC",
    };

    static_assert(arraySize(coreRegIds) == toScalar(CoreRegister::Max),
                  "CoreRegister type out of sync with strings.");

    if (regId < CoreRegister::Max)
    {
        return coreRegIds[toScalar(regId)];
    }
    else
    {
        return "(unknown Core Register)";
    }
}

}} // namespace Ag::Arm
////////////////////////////////////////////////////////////////////////////////


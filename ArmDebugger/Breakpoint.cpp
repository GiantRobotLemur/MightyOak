//! @file ArmDebugger/Breakpoint.cpp
//! @brief The definition of a class which represents a breakpoint set in the
//! emulated system by replacing an instruction with BKPT.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"
#include "AsmTools/InstructionInfo.hpp"
#include "ArmEmu/ArmSystem.hpp"

#include "Breakpoint.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Breakpoint Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
bool Breakpoint::CompareById::operator()(const Breakpoint &lhs,
                                         const Breakpoint &rhs) const
{
    return lhs.getBreakpointID() < rhs.getBreakpointID();
}

bool Breakpoint::CompareByAddress::operator()(const Breakpoint &lhs,
                                              const Breakpoint &rhs) const
{
    bool isLess = false;

    if (lhs.isLogicalAddress() == rhs.isLogicalAddress())
    {
        isLess = (lhs.getAddress() < rhs.getAddress());
    }
    else
    {
        isLess = rhs.isLogicalAddress();
    }

    return isLess;
}

Breakpoint::Breakpoint() :
    _hostAddress(nullptr),
    _address(0),
    _originalInstruction(0),
    _breakpointID(0),
    _isLogicalAddress(false),
    _isSet(false)
{
}

Breakpoint::Breakpoint(Arm::IArmSystem *emulator, uint32_t address,
                       uint16_t id, bool isLogicalAddr) :
    _hostAddress(nullptr),
    _address(address & ~3u),
    _originalInstruction(0),
    _breakpointID(id),
    _isLogicalAddress(isLogicalAddr),
    _isSet(false)
{
    if (emulator != nullptr)
    {
        uint32_t physicalAddr = 0;

        if (isLogicalAddr)
        {
            Arm::PageMapping mapping;

            if (emulator->logicalToPhysicalAddress(_address, mapping))
            {
                uint32_t pageOffset = _address - mapping.VirtualBaseAddr;
                physicalAddr = mapping.PageBaseAddr + pageOffset;
            }
            else
            {
                return;
            }
        }
        else
        {
            physicalAddr = address;
        }

        // Calculate the host address which back the address in the
        // emulated system.
        const auto &addrMap = emulator->getReadAddresses();
        Arm::IAddressRegionPtr region;
        uint32_t offset, regionLeft;

        if (addrMap.tryFindRegion(physicalAddr, region, offset, regionLeft) &&
            (region->getType() == Arm::RegionType::HostBlock))
        {
            auto hostRegion = static_cast<Arm::IHostBlockPtr>(region);

            _hostAddress = Ag::offsetPtr<uint32_t>(hostRegion->getHostAddress(), offset);
            _originalInstruction = *_hostAddress;
        }
    }
}

bool Breakpoint::isValid() const { return _hostAddress != nullptr; }
bool Breakpoint::isEnabled() const { return _isSet; }
bool Breakpoint::isLogicalAddress() const { return _isLogicalAddress; }
uint32_t Breakpoint::getAddress() const { return _address; }
uint32_t Breakpoint::getInstruction() const { return _originalInstruction; }
uint16_t Breakpoint::getBreakpointID() const { return _breakpointID; }

bool Breakpoint::apply()
{
    if ((_hostAddress != nullptr) && (_isSet == false))
    {
        using namespace Asm;

        // Craft a bkpt instruction to insert into emulated memory.
        InstructionInfo breakPt(InstructionMnemonic::Bkpt,
                                OperationClass::Breakpoint);
        breakPt.getBreakpointParameters().Comment = _breakpointID;

        Ag::String error;
        uint32_t instruction;

        if (breakPt.assemble(instruction, _address, error))
        {
            // Overwrite the original instruction in memory.
            *_hostAddress = instruction;

            _isSet = true;
        }
    }

    return _isSet;
}

void Breakpoint::remove()
{
    if ((_hostAddress != nullptr) && _isSet)
    {
        // Overwrite the breakpoint instruction in memory.
        *_hostAddress = _originalInstruction;
        _isSet = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


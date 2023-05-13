//! @file ArmEmu/ArmSystemTraits.inl
//! @brief The declaration of a template which uses traits to instantiate
//! emulators for different system configurations.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_SYSTEM_INL__
#define __ARM_EMU_SYSTEM_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu/ArmSystem.hpp"
#include "ArmEmu/EmuOptions.hpp"
#include "SystemConfigurations.inl"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief A template which implements IArmSystem using various different
//! configurations specified by a traits type.
template<typename TSysTraits>
struct ArmSystem : public IArmSystem
{
private:
    // Internal Types
    using Hardware = typename TSysTraits::HardwareType;
    using RegisterFile = typename TSysTraits::RegisterFileType;
    using ExecutionUnit = typename TSysTraits::ExecutionUnitType;

    // Internal Fields
    Hardware _hardware;
    RegisterFile _registers;
    ExecutionUnit _execUnit;
    AddressMap _addrDecoderReadMap;
    AddressMap _addrDecoderWriteMap;
public:
    // Construction/Destruction
    //! @brief Constructs an emulator for a system which has no additional
    //! hardware over and above what the hardware types supplies.
    //! @param[in] options An object describing the preferred configuration of
    //! the emulated system.
    ArmSystem(const Options &options) :
        _hardware(options),
        _registers(_hardware),
        _execUnit(_hardware, _registers)
    {
        // Set the hardware to the power-on state.
        reset();

    }

    //! @brief Constructs an emulator for a system which as additional host
    //! mapped blocks of RAM, ROM or memory mapped I/O.
    //! @param[in] options An object describing the preferred configuration of
    //! the emulated system.
    //! @param[in] read The additional mappings of regions of memory which can
    //! be read over and above standard devices, ROM and RAM.
    //! @param[in] write The additional mappings of regions of memory which can
    //! be written over and above standard devices, ROM and RAM.
    ArmSystem(const Options &options, const AddressMap &read,
              const AddressMap &write) :
        _hardware(options, read, write),
        _registers(_hardware),
        _execUnit(_hardware, _registers),
        _addrDecoderReadMap(read),
        _addrDecoderWriteMap(write)
    {
        // Set the hardware and processor to the power-on state.
        reset();
    }

    virtual ~ArmSystem() = default;

    // Accessors
    Hardware &getHardare() { return _hardware; }
    const Hardware &getHardare() const { return _hardware; }
    RegisterFile &getRegisters() { return _registers; }
    const RegisterFile &getRegisters() const { return _registers; }

    // Operations
    void reset()
    {
        // Set the hardware to the power-on state.
        _hardware.reset();

        // Set the instruction pipeline to a known start-up state.
        _registers.raiseReset();
    }

    // Overrides
    virtual ProcessorMode getMode() const override
    {
        return _registers.getMode();
    }

    virtual uint32_t getCoreRegister(CoreRegister id) const
    {
        uint32_t result = 0;

        switch (id)
        {
        case CoreRegister::SPSR:
            // TODO: Create an interface for this, if possible.
            result = _registers.getPSR();
            break;

        case CoreRegister::CPSR:
            result = _registers.getPSR();
            break;

        case CoreRegister::PC:
            result = _registers.getPC();
            break;

        default:
            if (id <= CoreRegister::R15)
            {
                result = _registers.getRm(static_cast<GeneralRegister>(Ag::toScalar(id)));
            }
            break;
        }

        return result;
    }

    virtual void setCoreRegister(CoreRegister id, uint32_t value) override
    {
        switch (id)
        {
        case CoreRegister::SPSR:
            // TODO: Create an interface for this, if possible.
            break;

        case CoreRegister::CPSR:
            _registers.setPSR(value);
            break;

        case CoreRegister::PC:
            _registers.setPC(value);
            break;

        default:
            if (id <= CoreRegister::R15)
            {
                // Set and preserve status flags, unless writing to R15 on
                // a 26-bit system.
                _registers.setRdAndStatus(static_cast<GeneralRegister>(Ag::toScalar(id)), value,
                                          static_cast<uint8_t>(_registers.getPSR() >> PsrShift::Status));
            }
            break;
        }
    }

    virtual const AddressMap &getReadAddresses() const override
    {
        return _hardware.getReadAddressMap();
    }

    virtual const AddressMap &getWriteAddresses() const override
    {
        return _hardware.getWriteAddressMap();
    }

    virtual bool logicalToPhysicalAddress(uint32_t logicalAddr,
                                          uint32_t &physAddr) const override
    {
        return _hardware.logicalToPhysicalAddress(logicalAddr, physAddr);
    }

    // Operations
    virtual ExecutionMetrics run()  override
    {
        return _execUnit.runPipeline(false);
    }

    virtual ExecutionMetrics runSingleStep() override
    {
        return _execUnit.runPipeline(true);
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

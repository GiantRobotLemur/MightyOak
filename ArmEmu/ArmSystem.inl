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
// Dependant Header Files
////////////////////////////////////////////////////////////////////////////////
#include "ArmEmu.hpp"
#include "SystemConfigurations.inl"

namespace Ag {
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

public:
    // Construction/Destruction
    ArmSystem() :
        _registers(_hardware),
        _execUnit(_hardware, _registers)
    {
        // Set the instruction pipeline to a known start-up state.
        _registers.raiseReset();
    }

    virtual ~ArmSystem() = default;

    // Accessors
    Hardware &getHardare() { return _hardware; }
    const Hardware &getHardare() const { return _hardware; }
    RegisterFile &getRegisters() { return _registers; }
    const RegisterFile &getRegisters() const { return _registers; }

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
                result = _registers.getRm(static_cast<GeneralRegister>(toScalar(id)));
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
                _registers.setRdAndStatus(static_cast<GeneralRegister>(toScalar(id)), value,
                                          static_cast<uint8_t>(_registers.getPSR() >> PsrShift::Status));
            }
            break;
        }
    }

    virtual uint32_t readFromLogicalAddress(uint32_t logicalAddr, uint32_t length,
                                            void *buffer) const override
    {
        MemoryMapping mapping;
        uint32_t bytesRead = 0;
        uint32_t currentAddr = logicalAddr;

        // Bless me father, for I have sinned. But a thousand promises that
        // I won't ACTUALLY write to the object, I just need to translate
        // some addresses...
        Hardware *writableHw = const_cast<Hardware *>(&_hardware);
        uint8_t *target = reinterpret_cast<uint8_t *>(buffer);

        while (bytesRead < length)
        {
            if (writableHw->tryMapLogicalAddress(currentAddr, true, mapping))
            {
                uint32_t offset = currentAddr - mapping.GuestAddress;
                uint32_t maxRead = mapping.Size - offset;
                uint32_t readLength = std::min(maxRead, length - bytesRead);

                // Transfer the bytes.
                std::memcpy(target + bytesRead,
                            static_cast<uint8_t *>(mapping.HostAddress) + offset,
                            readLength);

                bytesRead += readLength;
                currentAddr += readLength;
            }
            else
            {
                // The region of memory did not map to host memory.
                break;
            }
        }

        return bytesRead;
    }

    virtual void writeToLogicalAddress(uint32_t logicalAddr, uint32_t length,
                                       const void *buffer) override
    {
        MemoryMapping mapping;
        uint32_t bytesWritten = 0;
        uint32_t currentAddr = logicalAddr;

        const uint8_t *source = reinterpret_cast<const uint8_t *>(buffer);

        while (bytesWritten < length)
        {
            if (_hardware.tryMapLogicalAddress(currentAddr, false, mapping))
            {
                uint32_t offset = currentAddr - mapping.GuestAddress;
                uint32_t maxWrite = mapping.Size - offset;
                uint32_t writeLength = std::min(maxWrite, length - bytesWritten);

                // Transfer the bytes.
                std::memcpy(static_cast<uint8_t *>(mapping.HostAddress) + offset,
                            source + bytesWritten,
                            writeLength);

                bytesWritten += writeLength;
                currentAddr += writeLength;
            }
            else
            {
                // The region of memory did not map to host memory.
                break;
            }
        }
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

}} // namespace Ag::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/ArmSystemTraits.inl
//! @brief The declaration of a template which uses traits to instantiate
//! emulators for different system configurations.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_SYSTEM_INL__
#define __ARM_EMU_SYSTEM_INL__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstring>
#include <set>
#include <type_traits>

#include "Ag/Core/Utils.hpp"

#include "ArmEmu/ArmSystem.hpp"
#include "ArmEmu/IDiagnosticSink.hpp"
#include "ArmEmu/Display.hpp"
#include "ArmEmu/GuestEventQueue.hpp"
#include "ArmEmu/EmuOptions.hpp"
#include "ArmEmu/SystemContext.hpp"
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
    GuestEventQueueUPtr _eventQueue;
    SystemContext _interop;
    Hardware _hardware;
    RegisterFile _registers;
    ExecutionUnit _execUnit;
    AddressMap _addrDecoderReadMap;
    AddressMap _addrDecoderWriteMap;
    HardwareDevicePool _devices;
    std::atomic_bool _isRunning;

    // Internal Functions

    //! @brief Performs shared initialisation tasks from the constructor.
    void initialise()
    {
        // Gather all devices we know about.
        IHardwareDeviceCollection allDevices;
        allDevices.reserve(64);

        for (auto &devicePtr : _devices)
        {
            // The device was newly inserted, connect it.
            allDevices.push_back(devicePtr.get());
        }

        for (uint8_t i = 0; i < 2; ++i)
        {
            AddressMap &map = (i == 0) ? _addrDecoderReadMap : _addrDecoderWriteMap;

            for (auto &mapping : map.getMappings())
            {
                // The device was newly inserted, connect it.
                allDevices.push_back(mapping.Region);
            }
        }

        // Register integral hardware which doesn't appear on the address maps.
        _hardware.addIntegralHardware(allDevices);

        // Index and connect all devices together and to inter-op services.
        _interop.connectAllDevices(allDevices);

        // Allow hardware to connect to (possibly external) devices.
        _hardware.connect(_interop);

        // Connect the execution unit to the same devices, if needed.
        _execUnit.connect();
    }
public:
    // Construction/Destruction
    //! @brief Constructs an emulator for a system which has no additional
    //! hardware over and above what the hardware types supplies.
    //! @param[in] options An object describing the preferred configuration of
    //! the emulated system.
    ArmSystem(const Options &options) :
        _eventQueue(Ag::createUniqueAligned<GuestEventQueue>(reinterpret_cast<uintptr_t>(static_cast<IArmSystem *>(this)))),
        _interop(options, *_eventQueue.get(), this),
        _hardware(options),
        _registers(_hardware),
        _execUnit(_hardware, _registers, _interop),
        _addrDecoderReadMap(_hardware.createMasterReadMap()),
        _addrDecoderWriteMap(_hardware.createMasterWriteMap()),
        _isRunning(false)
    {
        // Perform shared initialisation.
        initialise();

        // Set the hardware to the power-on state.
        reset();
    }

    //! @brief Constructs an emulator for a system which as additional host
    //! mapped blocks of RAM, ROM or memory mapped I/O.
    //! @param[in] options An object describing the preferred configuration of
    //! the emulated system.
    //! @param[in] devices The set of hardware devices to be owned by the
    //! newly created object.
    //! @param[in] read The additional mappings of regions of memory which can
    //! be read over and above standard devices, ROM and RAM.
    //! @param[in] write The additional mappings of regions of memory which can
    //! be written over and above standard devices, ROM and RAM.
    ArmSystem(const Options &options, HardwareDevicePool &&devices,
              const AddressMap &read, const AddressMap &write) :
        _eventQueue(Ag::createUniqueAligned<GuestEventQueue>(reinterpret_cast<uintptr_t>(static_cast<IArmSystem *>(this)))),
        _interop(options, *_eventQueue.get(), this),
        _hardware(options, read, write),
        _registers(_hardware),
        _execUnit(_hardware, _registers, _interop),
        _addrDecoderReadMap(_hardware.createMasterReadMap()),
        _addrDecoderWriteMap(_hardware.createMasterWriteMap()),
        _devices(std::move(devices)),
        _isRunning(false)
    {
        // Perform shared initialisation.
        initialise();

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
    // Inherited from IArmSystem.
    virtual bool isRunning() const override { return _isRunning; }

    // Inherited from IArmSystem.
    virtual ProcessorMode getMode() const override
    {
        return _registers.getMode();
    }

    // Inherited from IArmSystem.
    virtual uint32_t getCoreRegister(CoreRegister id) const
    {
        uint32_t result = 0;

        switch (id)
        {
        case CoreRegister::SPSR:
            result = _registers.getSPSR();
            break;

        case CoreRegister::CPSR:
            result = _registers.getPSR();
            break;

        case CoreRegister::PC:
            result = _registers.getPC();

            if (!_execUnit.isFlushPending())
            {
                result -= 8;
            }
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

    // Inherited from IArmSystem.
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

            // Ensure the new PC points to the next instruction as if
            // it has been overwritten internally.
            _execUnit.flushPipeline();
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

    // Inherited from IArmSystem.
    virtual const AddressMap &getReadAddresses() const override
    {
        return _addrDecoderReadMap;
    }

    // Inherited from IArmSystem.
    virtual const AddressMap &getWriteAddresses() const override
    {
        return _addrDecoderWriteMap;
    }

    // Inherited from IArmSystem.
    virtual bool logicalToPhysicalAddress(uint32_t logicalAddr,
                                          PageMapping &mapping) const override
    {
        return _hardware.logicalToPhysicalAddress(logicalAddr, mapping);
    }

    // Inherited from IArmSystem.
    virtual bool tryFindDevice(Ag::string_cref_t name,
                               IHardwareDevicePtr &device) const override
    {
        return _interop.tryFindDevice(name, device);
    }

    // Operations
    // Inherited from IArmSystem.
    virtual ExecutionMetrics run()  override
    {
        Ag::ValueScope<std::atomic_bool, bool> isRunning(_isRunning, true);

        return _execUnit.runPipeline(0);
    }

    // Inherited from IArmSystem.
    virtual ExecutionMetrics runSingleStep() override
    {
        Ag::ValueScope<std::atomic_bool, bool> isRunning(_isRunning, true);
        return _execUnit.runPipeline(-1);
    }

    // Inherited from IArmSystem.
    virtual ExecutionMetrics runLimited(int32_t maxCycles) override
    {
        Ag::ValueScope<std::atomic_bool, bool> isRunning(_isRunning, true);

        return _execUnit.runPipeline(static_cast<int32_t>(maxCycles));
    }

    // Inherited from IArmSystem.
    virtual void raiseHostInterrupt() override
    {
        _hardware.setHostIrq(true);
    }

    // Inherited from IArmSystem.
    virtual bool tryGetNextMessage(GuestEvent &next) override
    {
        return _eventQueue->tryDeque(next);
    }
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

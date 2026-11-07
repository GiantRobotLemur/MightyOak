//! @file ArmEmu/I2CBus.hpp
//! @brief The declaration of an object which emulates a bit-banged I2C bus
//! connected to IOC control pins C0 (SDA) and C1 (SCL).
//! @author GiantRobotLemur@na-se.co.uk - and Claude Code.
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_I2C_BUS_HPP__
#define __ARM_EMU_I2C_BUS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <cstddef>

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An interface for devices attached to the I2C bus.
struct II2CDevice
{
    virtual ~II2CDevice() = default;

    //! @brief Gets the 7-bit I2C address of this device.
    virtual uint8_t getI2CAddress() const = 0;

    //! @brief Called when the device is selected and a byte has been received.
    //! @param[in] data The byte received from the bus master.
    //! @param[in] isFirst True if this is the first byte after the address
    //! (typically a register/sub-address).
    //! @retval true The device acknowledges the byte.
    //! @retval false The device does not acknowledge.
    virtual bool onI2CWrite(uint8_t data, bool isFirst) = 0;

    //! @brief Called when the bus master reads a byte from this device.
    //! @param[out] data Receives the byte to send to the bus master.
    //! @retval true A byte was produced and should be sent.
    //! @retval false The device has no more data to send.
    virtual bool onI2CRead(uint8_t &data) = 0;

    //! @brief Called when a STOP condition is detected, ending the transaction.
    virtual void onI2CStop() = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Emulates a bit-banged I2C bus connected to IOC control pins
//! C0 (SDA) and C1 (SCL).
//! @details Monitors changes to the SDA and SCL lines driven by the CPU
//! via the IOC control register. Detects START/STOP conditions, clocks in
//! address and data bytes, dispatches to registered II2CDevice implementations,
//! and drives SDA for ACK/read data.
class I2CBus
{
public:
    // Public Constants
    static constexpr size_t MaxDevices = 4;

    // Construction/Destruction
    I2CBus();
    ~I2CBus() = default;

    // Operations
    void addDevice(II2CDevice *device);

    //! @brief Called when the IOC control register is written, updating the
    //! state of the SDA and SCL lines.
    //! @param[in] sda The new state of the SDA line (C0). True = high/released.
    //! @param[in] scl The new state of the SCL line (C1). True = high/released.
    //! @param[out] sdaInput Receives the state the I2C bus drives onto the SDA
    //! input pin. True = high (not pulling down), false = pulled low (ACK or data 0).
    void update(bool sda, bool scl, bool &sdaInput);

    //! @brief Resets the I2C bus state.
    void reset();

private:
    // Internal Types
    enum class BusState : uint8_t
    {
        Idle,           // Bus is idle, waiting for START
        AddressBits,    // Receiving 7-bit address + R/W bit
        AddressAck,     // Sending ACK for address byte
        DataBitsWrite,  // Receiving data byte from master
        DataAckWrite,   // Sending ACK for received data byte
        DataBitsRead,   // Sending data byte to master
        DataAckRead,    // Waiting for master ACK/NACK of read data
    };

    // Internal Functions
    II2CDevice *findDevice(uint8_t address7bit);

    // Internal Fields
    II2CDevice *_devices[MaxDevices];
    II2CDevice *_selectedDevice;
    uint8_t _deviceCount;
    BusState _state;
    uint8_t _shiftRegister;
    uint8_t _bitCount;
    bool _isRead;
    bool _lastSDA;
    bool _lastSCL;
    bool _sdaDriven;  // True if the bus is pulling SDA low
    bool _isFirstByte;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmEmu/I2CBus.cpp
//! @brief The definition of an object which emulates a bit-banged I2C bus
//! connected to IOC control pins C0 (SDA) and C1 (SCL).
//! @author GiantRobotLemur@na-se.co.uk - and Claude Code.
//! @date 2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "I2CBus.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// I2CBus Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an I2C bus with no attached devices.
I2CBus::I2CBus() :
    _devices{},
    _selectedDevice(nullptr),
    _deviceCount(0),
    _state(BusState::Idle),
    _shiftRegister(0),
    _bitCount(0),
    _isRead(false),
    _lastSDA(true),
    _lastSCL(true),
    _sdaDriven(false),
    _isFirstByte(true)
{
}

//! @brief Registers an I2C device on the bus.
//! @param[in] device A pointer to the device to register. Must remain valid
//! for the lifetime of the bus.
void I2CBus::addDevice(II2CDevice *device)
{
    if ((device != nullptr) && (_deviceCount < MaxDevices))
    {
        _devices[_deviceCount++] = device;
    }
}

//! @brief Called when the IOC control register is written, updating the
//! state of the SDA and SCL lines.
//! @param[in] sda The new state of the SDA line (C0). True = high/released.
//! @param[in] scl The new state of the SCL line (C1). True = high/released.
//! @param[out] sdaInput Receives the state the I2C bus drives onto the SDA
//! input pin.
void I2CBus::update(bool sda, bool scl, bool &sdaInput)
{
    // Detect START condition: SDA falls while SCL is high.
    if (_lastSDA && !sda && scl)
    {
        _state = BusState::AddressBits;
        _shiftRegister = 0;
        _bitCount = 0;
        _selectedDevice = nullptr;
        _sdaDriven = false;
        _isFirstByte = true;

        _lastSDA = sda;
        _lastSCL = scl;
        sdaInput = !_sdaDriven;
        return;
    }

    // Detect STOP condition: SDA rises while SCL is high.
    if (!_lastSDA && sda && scl)
    {
        if (_selectedDevice != nullptr)
        {
            _selectedDevice->onI2CStop();
        }

        _state = BusState::Idle;
        _selectedDevice = nullptr;
        _sdaDriven = false;

        _lastSDA = sda;
        _lastSCL = scl;
        sdaInput = !_sdaDriven;
        return;
    }

    // Detect rising edge of SCL (clock in data).
    bool sclRising = !_lastSCL && scl;

    // Detect falling edge of SCL (prepare data for next clock).
    bool sclFalling = _lastSCL && !scl;

    _lastSDA = sda;
    _lastSCL = scl;

    switch (_state)
    {
    case BusState::Idle:
        // Nothing to do.
        break;

    case BusState::AddressBits:
        if (sclRising)
        {
            // Clock in a bit (MSB first). The address is 7 bits + 1 R/W bit.
            _shiftRegister = (_shiftRegister << 1) | (sda ? 1 : 0);
            _bitCount++;

            if (_bitCount >= 8)
            {
                // We have the full address byte.
                uint8_t addr7 = _shiftRegister >> 1;
                _isRead = (_shiftRegister & 1) != 0;
                _selectedDevice = findDevice(addr7);
                _bitCount = 0;
                _state = BusState::AddressAck;
            }
        }
        break;

    case BusState::AddressAck:
        // _bitCount == 0: waiting for first SCL falling to assert ACK.
        // _bitCount == 1: ACK asserted, waiting for SCL rising (master samples).
        // _bitCount == 2: ACK sampled, waiting for SCL falling to release and transition.
        if (sclFalling && _bitCount == 0)
        {
            // First falling edge after address byte: drive ACK if device found.
            _sdaDriven = (_selectedDevice != nullptr);
            _bitCount = 1;
        }
        else if (sclRising && _bitCount == 1)
        {
            // Master is sampling our ACK on the rising edge.
            _bitCount = 2;
        }
        else if (sclFalling && _bitCount == 2)
        {
            // ACK has been sampled. Release SDA and move to data phase.
            _sdaDriven = false;
            _bitCount = 0;
            _shiftRegister = 0;

            if (_selectedDevice == nullptr)
            {
                // No device responded, go back to idle.
                _state = BusState::Idle;
            }
            else if (_isRead)
            {
                // Prepare data byte to send to master.
                _shiftRegister = 0xFF;
                _selectedDevice->onI2CRead(_shiftRegister);
                _state = BusState::DataBitsRead;

                // Drive the first data bit immediately since we are already
                // on the falling edge that DataBitsRead would normally use.
                _sdaDriven = !((_shiftRegister >> 7) & 1);
            }
            else
            {
                _state = BusState::DataBitsWrite;
            }
        }
        break;

    case BusState::DataBitsWrite:
        if (sclRising)
        {
            // Clock in data bit from master.
            _shiftRegister = (_shiftRegister << 1) | (sda ? 1 : 0);
            _bitCount++;

            if (_bitCount >= 8)
            {
                _bitCount = 0;
                _state = BusState::DataAckWrite;
            }
        }
        break;

    case BusState::DataAckWrite:
        // _bitCount == 0: waiting for first SCL falling to assert ACK.
        // _bitCount == 1: ACK asserted, waiting for SCL rising (master samples).
        // _bitCount == 2: ACK sampled, waiting for SCL falling to release.
        if (sclFalling && _bitCount == 0)
        {
            // First falling edge after data byte: ACK if device accepted.
            bool ack = false;

            if (_selectedDevice != nullptr)
            {
                ack = _selectedDevice->onI2CWrite(_shiftRegister, _isFirstByte);
            }

            _isFirstByte = false;
            _sdaDriven = ack;
            _bitCount = 1;
        }
        else if (sclRising && _bitCount == 1)
        {
            // Master sampling our ACK.
            _bitCount = 2;
        }
        else if (sclFalling && _bitCount == 2)
        {
            // ACK sampled, release SDA, ready for next byte.
            _sdaDriven = false;
            _bitCount = 0;
            _shiftRegister = 0;
            _state = BusState::DataBitsWrite;
        }
        break;

    case BusState::DataBitsRead:
        if (sclFalling)
        {
            if (_bitCount >= 8)
            {
                // Byte complete, release SDA and wait for master ACK/NACK.
                _sdaDriven = false;
                _state = BusState::DataAckRead;
            }
            else
            {
                // Drive the next data bit (MSB first).
                _sdaDriven = !((_shiftRegister >> (7 - _bitCount)) & 1);
            }
        }
        else if (sclRising)
        {
            // Bit has been sampled by master.
            _bitCount++;
        }
        break;

    case BusState::DataAckRead:
        if (sclRising)
        {
            // Master sends ACK (SDA low) or NACK (SDA high).
            if (!sda)
            {
                // ACK - master wants more data.
                _bitCount = 0;
                _shiftRegister = 0xFF;

                if (_selectedDevice != nullptr)
                {
                    _selectedDevice->onI2CRead(_shiftRegister);
                }

                _state = BusState::DataBitsRead;
            }
            else
            {
                // NACK - master is done reading.
                _state = BusState::Idle;
            }
        }
        break;
    }

    sdaInput = !_sdaDriven;
}

//! @brief Resets the I2C bus to idle state.
void I2CBus::reset()
{
    _selectedDevice = nullptr;
    _state = BusState::Idle;
    _shiftRegister = 0;
    _bitCount = 0;
    _isRead = false;
    _lastSDA = true;
    _lastSCL = true;
    _sdaDriven = false;
    _isFirstByte = true;
}

//! @brief Searches for a registered device with the specified 7-bit address.
//! @param[in] address7bit The 7-bit I2C address to search for.
//! @return A pointer to the device, or nullptr if not found.
II2CDevice *I2CBus::findDevice(uint8_t address7bit)
{
    for (uint8_t i = 0; i < _deviceCount; ++i)
    {
        if (_devices[i]->getI2CAddress() == address7bit)
        {
            return _devices[i];
        }
    }

    return nullptr;
}

}} // namespace Mo::Arm
////////////////////////////////////////////////////////////////////////////////

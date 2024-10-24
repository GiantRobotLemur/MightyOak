//! @file ArmEmu/AcornKeyboardController.hpp
//! @brief The declaration of an object which represents the 87C51 keyboard
//! controller, translating between key press and mouse movement and the
//! internal KART protocol.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_ACORN_KEYBOARD_CONTROLLER_HPP__
#define __ARM_EMU_ACORN_KEYBOARD_CONTROLLER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/LinearSortedMap.hpp"

#include "ArmEmu/IKeyboardController.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class IOC;

//! @brief An object which represents the 87C51 keyboard controller,
//! translating between key press and mouse movement and the internal
//! KART protocol.
class AcornKeyboardController : public IKeyboardController
{
public:
    // Public Constants
    static constexpr uint8_t HRST          = 0xFF;
    static constexpr uint8_t RAK1          = 0xFE;
    static constexpr uint8_t RAK2          = 0xFD;
    static constexpr uint8_t RQPD_Bits     = 0x40;
    static constexpr uint8_t RQPD_Mask     = 0x0F;
    static constexpr uint8_t PDAT_Bits     = 0xE0;
    static constexpr uint8_t PDAT_Mask     = 0x0F;
    static constexpr uint8_t RQID          = 0x20;
    static constexpr uint8_t KBID_Bits     = 0x80;
    static constexpr uint8_t KBID_Mask     = 0x3F;
    static constexpr uint8_t KDDA_Bits     = 0xC0;
    static constexpr uint8_t KDDA_Mask     = 0x0F;
    static constexpr uint8_t KUDA_Bits     = 0xD0;
    static constexpr uint8_t KUDA_Mask     = 0x0F;
    static constexpr uint8_t RQMP          = 0x22;
    static constexpr uint8_t MDAT_Bits     = 0x00;
    static constexpr uint8_t MDAT_Mask     = 0x7F;
    static constexpr uint8_t BACK          = 0x3F;
    static constexpr uint8_t NACK          = 0x30;
    static constexpr uint8_t SACK          = 0x31;
    static constexpr uint8_t MACK          = 0x32;
    static constexpr uint8_t SMAK          = 0x33;
    static constexpr uint8_t LEDS_Bits     = 0x00;
    static constexpr uint8_t LEDS_Mask     = 0x07;
    static constexpr uint8_t PRST          = 0x21;

    // Construction/Destruction
    AcornKeyboardController();
    virtual ~AcornKeyboardController() = default;

    // Accessors

    // Operations
    void receiveKARTByte(uint8_t nextByte);

    // Overrides
    virtual Ag::string_cref_t getName() const override;
    virtual Ag::string_cref_t getDescription() const override;
    virtual void connect(const ConnectionContext &context) override;

    virtual void keyDown(uint32_t hostScanCode) override;
    virtual void keyUp(uint32_t hostScanCode) override;
    virtual void mouseButtonDown(MouseButton button) override;
    virtual void mouseButtonUp(MouseButton button) override;
    virtual void mouseDelta(int32_t deltaX, int32_t deltaY) override;
    virtual void setKeyMapping(const ScanCodeMapping *mappings, size_t count) override;
private:
    // Internal Types
    enum class ControllerState : uint8_t
    {
        PreReset,
        ReceivedHRST,
        ReceivedRAK1,
        Initialised,
        SentKeyRow,
        SentKeyColumn,

    };

    // Internal Functions

    // Internal Fields
    Ag::String _name;
    Ag::String _description;
    ScanCodeMap _scanCodeMap;
    IOC *_ioController;
    uint32_t _scanCodeBeingSent;
    ControllerState _state;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

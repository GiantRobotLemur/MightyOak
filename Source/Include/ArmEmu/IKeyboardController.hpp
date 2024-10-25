//! @file ArmEmu/IKeyboardController.hpp
//! @brief The declaration of an interface to a hardware device representing
//! the an emulated keyboard and mouse for the host to manipulate.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_EMU_KEYBOARD_CONTROLLER_HPP__
#define __ARM_EMU_KEYBOARD_CONTROLLER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "AddressMap.hpp"

namespace Mo {
namespace Arm {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An interface to a hardware device representing the an emulated
//! keyboard and mouse for the host to manipulate.
class IKeyboardController : public IHardwareDevice
{
public:
    // Public Types
    using ScanCodeMapping = std::pair<uint32_t, uint32_t>;
    using ScanCodeMap = Ag::LinearSortedMap<ScanCodeMapping::first_type,
                                            ScanCodeMapping::second_type>;

    enum MouseButton : uint32_t
    {
        LeftButton = 1,
        MiddleButton = 2,
        RightButton = 4,
        ButtonMask = 7
    };

    // Construction/Destruction
    virtual ~IKeyboardController() = default;

    // Accessors

    // Operations
    //! @brief Reports a key press.
    //! @param[in] hostScanCode The host scan code to map to a native key
    //! press and report.
    virtual void keyDown(uint32_t hostScanCode) =0;

    //! @brief Reports a key release.
    //! @param[in] hostScanCode The host scan code to map to a native key
    //! press and report.
    virtual void keyUp(uint32_t hostScanCode) = 0;

    //! @brief Reports a mouse button press.
    //! @param[in] button The code representing the button released.
    virtual void mouseButtonDown(MouseButton button) = 0;

    //! @brief Reports a mouse button release.
    //! @param[in] button The code representing the button released.
    virtual void mouseButtonUp(MouseButton button) = 0;

    //! @brief Reports mouse movement to the guest system.
    //! @param[in] deltaX The horizontal delta in mickeys.
    //! @param[in] deltaY The vertical delta in mickeys.
    virtual void mouseDelta(int32_t deltaX, int32_t deltaY) = 0;

    //! @brief Sets the mapping from native host key values to Acorn keyboard
    //! scan codes.
    //! @param[in] mappings An array of mappings of native key codes to Acorn
    //! keyboard scan codes.
    //! @param[in] count The count of element in the array.
    virtual void setKeyMapping(const ScanCodeMapping *mappings, size_t count) = 0;
};

}} // namespace Mo::Arm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmDebugger/UI/RegisterViewWidget.hpp
//! @brief The declaration of a widget which displays the current status of the
//! processor registers.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_REGISTER_VIEW_WIDGET_HPP__
#define __ARM_DEBUGGER_REGISTER_VIEW_WIDGET_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QWidget>

#include "ArmEmu/ArmSystem.hpp"

#include "ui_RegisterViewWidget.h"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Arm {
class Options;
class IArmSystem;
}


//! @brief A widget which displays the current status of the processor registers.
class RegisterViewWidget : public QWidget
{
Q_OBJECT
public:
    // Construction/Destruction
    RegisterViewWidget(QWidget *ownerWidget);
    virtual ~RegisterViewWidget() = default;

    // Accessors

    // Operations

    // Signal Handlers
private slots:
    void onSessionStarted(const Arm::Options &options, Arm::IArmSystem *emulator);
    void onSessionEnded(Arm::IArmSystem *emulator);
    void onSessionPaused(Arm::IArmSystem *emulator);
    void onSessionResumed(Arm::IArmSystem *emulator);
    void onSingleStep(Arm::IArmSystem *emulator);

private:
    // Internal Types
    struct RegisterWidget
    {
        QLabel *Label;
        QLineEdit *Field;
        Arm::CoreRegister Register;
        bool IsReadOnly;

        RegisterWidget();
        RegisterWidget(QLabel *label, QLineEdit *field, Arm::CoreRegister reg);
    };

    struct PsrFlagWidget
    {
        QLabel *Label;
        QCheckBox *Field;
        uint8_t Bit;

        PsrFlagWidget();
        PsrFlagWidget(uint8_t bit, QLabel *label, QCheckBox *field);
    };

    // Internal Functions
    void sampleState(Arm::IArmSystem *emulator);
    void enableEditing(bool canEdit);

    // Internal Fields
    Ui::RegisterViewWidget _ui;
    std::vector<RegisterWidget> _registers;
    std::vector<PsrFlagWidget> _psrBits;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

//! @file ArmDebugger/UI/NewSessionDialog.hpp
//! @brief The declaration of an object which manages a dialog to specify the
//! parameters of a new emulator session.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_NEW_SESSION_DIALOG_HPP__
#define __ARM_DEBUGGER_NEW_SESSION_DIALOG_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QDialog>

#include "ui_NewSessionDialog.h"

#include "ArmEmu/EmuOptions.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which manages a dialog to specify the parameters of a new
//! emulator session.
class NewSessionDialog : public QDialog
{
public:
    // Construction/Destruction
    NewSessionDialog(QWidget *ownerWidget, const Arm::Options *initialOpts = nullptr);
    virtual ~NewSessionDialog() = default;

    // Accessors
    bool isStartPaused() const;
    const Arm::Options &getOptions() const;

    // Operations

    // Overrides
protected:
    virtual void showEvent(QShowEvent *args) override;
    virtual void accept() override;
private slots:
    void onSysArchSelected(int selectedItem);
    void onSysRomSelected(int selectedItem);
    void onBrowseForSysRomImage();

private:
    // Internal Types

    // Internal Functions
    void onArchitectureChanged(Arm::SystemModel arch);
    void onSystemRomChanged(Arm::SystemROMPreset romPreset);
    void updateOptionsFromUI();

    // Internal Fields
    Ui::NewSessionDialog _ui;
    Arm::Options _options;
    bool _startPaused;
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

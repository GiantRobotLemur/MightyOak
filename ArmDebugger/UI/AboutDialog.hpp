//! @file ArmDebugger/UI/AboutDialog.hpp
//! @brief The declaration of an object representing the About This Program
//! dialog box.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_ABOUT_DIALOG_HPP__
#define __ARM_DEBUGGER_ABOUT_DIALOG_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QDialog>

#include "ui_AboutDialog.h"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing the About This Program dialog box.
class AboutDialog : public QDialog
{
public:
    // Construction/Destruction
    AboutDialog(QWidget *ownerObject);
    virtual ~AboutDialog() = default;

private:
    // Internal Fields
    Ui::AboutDialog _ui;
};

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////

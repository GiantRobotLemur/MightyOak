//! @file ArmDebugger/UI/AboutDialog.cpp
//! @brief The definition of an object representing the About This Program
//! dialog box.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AboutDialog.hpp"
#include "Version.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// AboutDialog Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
AboutDialog::AboutDialog(QWidget *ownerObject) :
    QDialog(ownerObject)
{
    _ui.setupUi(this);

    _ui._versionText->setText(tr("Version ") + MIGHTY_OAK_VERSION_TEXT);

    // TODO: Make SVG image correct size.
}

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


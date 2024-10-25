//! @file ArmDebugger/UI/EditOrdinalMappingDialog.hpp
//! @brief The declaration of a dialog which can edit sets of name/number
//! mappings.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_EDIT_ORDINAL_MAPPING_DIALOG_HPP__
#define __ARM_DEBUGGER_EDIT_ORDINAL_MAPPING_DIALOG_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QDialog>

#include "SessionSettings.hpp"

#include "ui_EditOrdinalMappingDialog.h"

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
class QAbstractTableModel;

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A dialog which can edit sets of name/number mappings.
class EditOrdinalMappingDialog : public QDialog
{
public:
    // Construction/Destruction
    EditOrdinalMappingDialog(QWidget *ownerWidget);
    virtual ~EditOrdinalMappingDialog() = default;

    // Accessors
    OrdinalMap getMappings() const;

    // Operations
    void initialise(const QString &domain, const QString &idName,
                    const QString &valueName, const OrdinalMap &ordinals,
                    const QString &fileExt);
private slots:
    // Signal Handlers
    void onAddRow();
    void onDeleteRow();
    void onImport();
    void onExport();

private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    Ui::EditOrdinalMappingDialog _ui;
    QAbstractTableModel *_ordinalModel;
    QString _domain;
    QString _fileExt;
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

//! @file ArmDebugger/UI/EditDisplayOptionsDialog.cpp
//! @brief The definition of an object representing a dialog used to edit
//! the display of emulated memory.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "UI/EditDisplayOptionsDialog.hpp"

#include "AsmTools/InstructionInfo.hpp"
#include "AsmTools/Options.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t StackUseMask = Asm::InstructionInfo::UseStackModesOnR13 |
                                  Asm::InstructionInfo::UseStackModesAlways;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void updateCheckBox(QCheckBox *widget, uint32_t flags, uint32_t flag)
{
    widget->setChecked((flags & flag) != 0);
}

void updateFromCheckBox(QCheckBox *widget, uint32_t &flags, uint32_t flag)
{
    if (widget->isChecked())
    {
        flags |= flag;
    }
}

int updateComboBox(QComboBox *widget, uint32_t selectedValue)
{
    const int count = widget->count();
    int selectedIndex = -1;
    bool isOK;

    for (int i = 0; (i < count) && (selectedIndex < 0); ++i)
    {
        uint32_t value = widget->itemData(i).toUInt(&isOK);

        if (isOK && (value == selectedValue))
        {
            selectedIndex = i;
        }
    }

    widget->setCurrentIndex(std::max(selectedIndex, 0));

    return selectedIndex;
}

void updateFromComboBox(QComboBox *widget, uint32_t &flags)
{
    int selectedIndex = widget->currentIndex();

    if (selectedIndex >= 0)
    {
        bool isOK;
        uint32_t value = widget->itemData(selectedIndex).toUInt(&isOK);

        if (isOK)
        {
            flags |= value;
        }
    }
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// EditDisplayOptionsDialog Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
EditDisplayOptionsDialog::EditDisplayOptionsDialog(QWidget *ownerWidget) :
    QDialog(ownerWidget)
{
    _ui.setupUi(this);

    QComboBox *list = _ui._processorVariantList;
    list->clear();
    list->addItem(tr("ARM v2"), QVariant(Asm::InstructionInfo::ARMv2));
    list->addItem(tr("ARM v2a"), QVariant(Asm::InstructionInfo::ARMv2a));
    list->addItem(tr("ARM v3"), QVariant(Asm::InstructionInfo::ARMv3));
    list->addItem(tr("ARM v4"), QVariant(Asm::InstructionInfo::ARMv4));
    list->addItem(tr("ARM v5"), QVariant(Asm::InstructionInfo::ARMv5));
    list->addItem(tr("ARM v6"), QVariant(Asm::InstructionInfo::ARMv6));
    list->addItem(tr("ARM v7"), QVariant(Asm::InstructionInfo::ARMv7));

    list = _ui._stackModesList;
    list->clear();
    list->addItem(tr("Core modes"), QVariant(0u));
    list->addItem(tr("Stack modes - R13 only"), QVariant(Asm::InstructionInfo::UseStackModesOnR13));
    list->addItem(tr("Stack modes - Always"), QVariant(Asm::InstructionInfo::UseStackModesAlways));

    list = _ui._repeatedDataTypeList;
    list->addItem(tr("Bytes (8-bits)"), QVariant(Ag::toScalar(Asm::DirectiveDataType::Byte)));
    list->addItem(tr("Half Words (16-bits)"), QVariant(Ag::toScalar(Asm::DirectiveDataType::HalfWord)));
    list->addItem(tr("Words (32-bits)"), QVariant(Ag::toScalar(Asm::DirectiveDataType::Word)));
    list->addItem(tr("Long Words (64-bits)"), QVariant(Ag::toScalar(Asm::DirectiveDataType::LongWord)));
}

const MemoryViewOptions &EditDisplayOptionsDialog::getOptions() const
{
    return _options;
}

void EditDisplayOptionsDialog::setOptions(const MemoryViewOptions &options)
{
    _options = options;
    updateToUI();
}

void EditDisplayOptionsDialog::updateToUI()
{
    const uint32_t formatOpts = _options.getAssemblyFormatFlags();
    updateCheckBox(_ui._offsetAddrFieldsCheckBox, formatOpts, Asm::FormatterOptions::ShowOffsets);
    updateCheckBox(_ui._coreRegAliasCheckBox, formatOpts, Asm::FormatterOptions::UseCoreRegAliases);
    updateCheckBox(_ui._decimalImmedsCheckBox, formatOpts, Asm::FormatterOptions::UseDecimalImmediates);
    updateCheckBox(_ui._commentAsDecimalCheckBox, formatOpts, Asm::FormatterOptions::UseDecimalComments);
    updateCheckBox(_ui._basicStyleHexCheckBox, formatOpts, Asm::FormatterOptions::UseBasicStyleHex);

    updateComboBox(_ui._processorVariantList, _options.getDissemblyFlags() & Asm::InstructionInfo::ModelMask);
    updateCheckBox(_ui._allowFPACheckBox, _options.getDissemblyFlags(), Asm::InstructionInfo::AllowFPA);
    updateComboBox(_ui._stackModesList, _options.getDissemblyFlags() & StackUseMask);

    const uint32_t displayOpts = _options.getDisplayFlags();
    updateCheckBox(_ui._showRawDataCheckBox, displayOpts, MemoryViewOptions::ShowRawData);
    updateCheckBox(_ui._showCommentsCheckBox, displayOpts, MemoryViewOptions::ShowComments);
    updateCheckBox(_ui._wrapOnWordBreaksCheckBox, displayOpts, MemoryViewOptions::WrapStringDataOnWordBreaks);
    updateComboBox(_ui._repeatedDataTypeList, Ag::toScalar(_options.getReptitiveDataType()));

}

void EditDisplayOptionsDialog::updateFromUI()
{
    uint32_t formatOpts = 0;
    updateFromCheckBox(_ui._offsetAddrFieldsCheckBox, formatOpts, Asm::FormatterOptions::ShowOffsets);
    updateFromCheckBox(_ui._coreRegAliasCheckBox, formatOpts, Asm::FormatterOptions::UseCoreRegAliases);
    updateFromCheckBox(_ui._decimalImmedsCheckBox, formatOpts, Asm::FormatterOptions::UseDecimalImmediates);
    updateFromCheckBox(_ui._commentAsDecimalCheckBox, formatOpts, Asm::FormatterOptions::UseDecimalComments);
    updateFromCheckBox(_ui._basicStyleHexCheckBox, formatOpts, Asm::FormatterOptions::UseBasicStyleHex);

    uint32_t disasmOpts = 0;
    updateComboBox(_ui._processorVariantList, disasmOpts);
    updateFromCheckBox(_ui._allowFPACheckBox, disasmOpts, Asm::InstructionInfo::AllowFPA);
    updateComboBox(_ui._stackModesList, disasmOpts);

    uint32_t displayOpts = 0;
    updateFromCheckBox(_ui._showRawDataCheckBox, displayOpts, MemoryViewOptions::ShowRawData);
    updateFromCheckBox(_ui._showCommentsCheckBox, displayOpts, MemoryViewOptions::ShowComments);
    updateFromCheckBox(_ui._wrapOnWordBreaksCheckBox, displayOpts, MemoryViewOptions::WrapStringDataOnWordBreaks);
    updateComboBox(_ui._repeatedDataTypeList, Ag::toScalar(_options.getReptitiveDataType()));
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


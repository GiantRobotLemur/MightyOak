//! @file ArmDebugger/UI/NewSessionDialog.cpp
//! @brief The definition of an object which manages a dialog to specify the
//! parameters of a new emulator session.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "NewSessionDialog.hpp"

#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

#include "Ag/Core/Format.hpp"
#include "Ag/Core/Utils.hpp"

#include "Ag/QtInterop/Conversion.hpp"

namespace Mo {

using namespace Mo::Arm;

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
template<typename TEnum, typename TEnumSymbol = Ag::EnumSymbol<TEnum>>
void fillComboBoxFromEnum(QComboBox &list, const Ag::EnumInfo<TEnum, TEnumSymbol> &info,
                          TEnum selectedItem)
{
    list.clear();
    int selectedIndex = 0;

    for (const auto &symbol : info.getSymbols())
    {
        QString text = QString::fromUtf8(symbol.getDisplayName());

        if (symbol.getId() == selectedItem)
        {
            selectedIndex = list.count();
        }

        list.addItem(text, Ag::toScalar(symbol.getId()));
    }

    list.setCurrentIndex(selectedIndex);
}

template<typename TEnum, typename TEnumSymbol = Ag::EnumSymbol<TEnum>>
void fillComboBoxFromEnum(QComboBox &list, const Ag::EnumInfo<TEnum, TEnumSymbol> &info,
                          const std::initializer_list<TEnum> &values,
                          TEnum selectedId)
{
    list.clear();
    int selectedIndex = 0;

    for (const auto &id : values)
    {
        const auto &symbol = info.getSymbolById(id);
        QString text = QString::fromUtf8(symbol.getDisplayName());

        if (selectedId == id)
        {
            selectedIndex = list.count();
        }

        list.addItem(text, Ag::toScalar(id));
    }

    list.setCurrentIndex(selectedIndex);
}

bool selectComboRawItem(QComboBox &list, uint32_t rawData)
{
    int count = list.count();

    for (int i = 0; i < count; ++i)
    {
        if (list.itemData(i).toUInt() == rawData)
        {
            list.setCurrentIndex(i);
            return true;
        }
    }

    return false;
}

template<typename TEnum>
bool selectComboItem(QComboBox &list, TEnum itemId)
{
    return selectComboRawItem(list, Ag::toScalar<TEnum>(itemId));
}

uint32_t getSelectedData(QComboBox &list)
{
    int itemIndex = list.currentIndex();

    return list.itemData(itemIndex).toUInt();
}

template<typename TEnum>
TEnum getSelectedItem(QComboBox &list)
{
    return Ag::forceFromScalar<TEnum>(getSelectedData(list));
}

ProcessorModel fillCPUList(QComboBox &list, SystemModel model)
{
    ProcessorModel selectedModel = ProcessorModel::ARM2;
    int selectedIndex = list.currentIndex();
    bool hasModel = false;

    if (list.currentIndex() >= 0)
    {
        ProcessorModel currentModel = Ag::forceFromScalar<ProcessorModel>(list.itemData(selectedIndex).toUInt());

        switch (model)
        {
        case SystemModel::TestBed:
        case SystemModel::Archimedies:
            hasModel = (currentModel < ProcessorModel::ARM610) &&
                (currentModel != ProcessorModel::ARM250);
            break;

        case SystemModel::ASeries:
            hasModel = (currentModel >= ProcessorModel::ARM250) &&
                (currentModel < ProcessorModel::ARM610);
            break;

        case SystemModel::RiscPC:
            hasModel = (currentModel >= ProcessorModel::ARM610);
            break;
        }

        if (hasModel)
        {
            // Preserve the current selection.
            selectedModel = currentModel;
        }
    }

    if (!hasModel)
    {
        // Select a model based on the underlying architecture.
        switch (model)
        {
        case SystemModel::TestBed:
        case SystemModel::Archimedies:
            selectedModel = ProcessorModel::ARM2;
            break;

        case SystemModel::ASeries:
            selectedModel = ProcessorModel::ARM250;
            break;

        case SystemModel::RiscPC:
            selectedModel = ProcessorModel::ARM610;
            break;
        }
    }

    switch (model)
    {
    case SystemModel::TestBed:
        fillComboBoxFromEnum(list, getProcessorModelType(),
            { ProcessorModel::ARM2,
              ProcessorModel::ARM250,
              ProcessorModel::ARM3,
              ProcessorModel::ARM3_FPA },
            selectedModel);
        break;

    case SystemModel::Archimedies:
        fillComboBoxFromEnum(list, getProcessorModelType(),
            { ProcessorModel::ARM2,
              ProcessorModel::ARM3,
              ProcessorModel::ARM3_FPA },
            selectedModel);
        break;

    case SystemModel::ASeries:
        fillComboBoxFromEnum(list, getProcessorModelType(),
            { ProcessorModel::ARM250,
              ProcessorModel::ARM3,
              ProcessorModel::ARM3_FPA },
            selectedModel);
        break;

    case SystemModel::RiscPC:
        fillComboBoxFromEnum(list, getProcessorModelType(),
            { ProcessorModel::ARM610,
              ProcessorModel::ARM710,
              ProcessorModel::ARM710_FPA,
              ProcessorModel::ARM810,
              ProcessorModel::ARM810_FPA,
              ProcessorModel::StrongARM,
              ProcessorModel::StrongARM_FPA },
            selectedModel);
        break;
    }

    return selectedModel;
}

uint32_t fillRAMList(QComboBox &list, SystemModel model)
{
    uint32_t ramSizeKb = 0;
    int selectedIndex = list.currentIndex();
    bool hasSize = false;

    if (selectedIndex >= 0)
    {
        ramSizeKb = static_cast<uint32_t>(list.itemData(selectedIndex).toUInt());

        if (model == SystemModel::RiscPC)
        {
            hasSize = Options::isValidRiscPCRAMSize(ramSizeKb);
        }
        else
        {
            hasSize = Options::isValidMemcRAMSize(ramSizeKb);
        }
    }

    if (!hasSize)
    {
        ramSizeKb = (model == SystemModel::RiscPC) ? 4096 : 1024;
    }

    list.clear();
    selectedIndex = -1;

    auto addRamSize = [&list, &ramSizeKb, &selectedIndex](uint32_t sizeKb)
        {
            std::string buffer;
            Ag::FormatInfo format(Ag::LocaleInfo::getDisplay());
            Ag::appendFileSize(format, buffer, static_cast<uint64_t>(sizeKb) << 10);
            int index = list.count();
            list.addItem(QString::fromStdString(buffer), sizeKb);

            if (sizeKb == ramSizeKb)
            {
                selectedIndex = index;
            }
        };

    if (model == SystemModel::RiscPC)
    {
        // There are four SIMMs, each can be empty or contain
        // 0, 4, 8, 16, 32 or 64 MB. At least one must be populated.
        std::vector<uint32_t> possibleSizes;

        // TODO: Enumerate all possible sizes, don't worry about duplication.
        possibleSizes.push_back(4096);


        std::sort(possibleSizes.begin(), possibleSizes.end());
        auto last = std::unique(possibleSizes.begin(), possibleSizes.end());
        possibleSizes.erase(last, possibleSizes.end());

        for (uint32_t ramSize : possibleSizes)
        {
            addRamSize(ramSize);
        }
    }
    else
    {
        addRamSize(512);
        addRamSize(1024);
        addRamSize(2048);
        addRamSize(4096);
        addRamSize(8192);
        addRamSize(12 << 10);
        addRamSize(16 << 10);
    }

    if (selectedIndex >= 0)
    {
        list.setCurrentIndex(selectedIndex);
    }

    return ramSizeKb;
}

SystemROMPreset fillROMSelection(QComboBox &list, SystemModel model)
{
    SystemROMPreset selectedRom = SystemROMPreset::Custom;
    int selectedIndex = list.currentIndex();
    bool hasPreset = false;

    if (selectedIndex >= 0)
    {
        selectedRom = Ag::forceFromScalar<SystemROMPreset>(list.itemData(selectedIndex).toUInt());

        switch (model)
        {
        case SystemModel::TestBed:
            // Only custom allowed
            selectedRom = SystemROMPreset::Custom;
            hasPreset = true;
            break;

        case SystemModel::Archimedies:
            hasPreset = (selectedRom < SystemROMPreset::RiscOS_3_50);
            break;

        case SystemModel::ASeries:
            hasPreset = (selectedRom >= SystemROMPreset::RiscOS_3_00) &&
                (selectedRom < SystemROMPreset::RiscOS_3_50);
            break;

        case SystemModel::RiscPC:
            hasPreset = (selectedRom >= SystemROMPreset::RiscOS_3_50);
            break;
        }
    }

    if (!hasPreset)
    {
        switch (model)
        {
        case SystemModel::TestBed:
            selectedRom = SystemROMPreset::Custom;
            break;

        case SystemModel::Archimedies:
            selectedRom = SystemROMPreset::RiscOS_2_00;
            break;

        case SystemModel::ASeries:
            selectedRom = SystemROMPreset::RiscOS_3_11;
            break;

        case SystemModel::RiscPC:
            selectedRom = SystemROMPreset::RiscOS_3_50;
            break;
        }
    }

    switch (model)
    {
    case SystemModel::TestBed:
        fillComboBoxFromEnum(list, getSystemROMPresetType(),
                             { SystemROMPreset::Custom },
                             selectedRom);
        break;

    case SystemModel::Archimedies:
        fillComboBoxFromEnum(list, getSystemROMPresetType(),
                            { SystemROMPreset::Custom,
                              SystemROMPreset::Arthur_0_30,
                              SystemROMPreset::Arthur_1_20,
                              SystemROMPreset::RiscOS_2_00,
                              SystemROMPreset::RiscOS_2_01,
                              SystemROMPreset::RiscOS_3_00,
                              SystemROMPreset::RiscOS_3_10,
                              SystemROMPreset::RiscOS_3_11,
                              SystemROMPreset::RiscOS_3_19, },
                            selectedRom);
        break;

    case SystemModel::ASeries:
        fillComboBoxFromEnum(list, getSystemROMPresetType(),
                            { SystemROMPreset::Custom,
                              SystemROMPreset::RiscOS_3_00,
                              SystemROMPreset::RiscOS_3_10,
                              SystemROMPreset::RiscOS_3_11,
                              SystemROMPreset::RiscOS_3_19, },
                            selectedRom);
        break;

    case SystemModel::RiscPC:
        selectedRom = SystemROMPreset::RiscOS_3_50;
        fillComboBoxFromEnum(list, getSystemROMPresetType(),
                            { SystemROMPreset::Custom,
                              SystemROMPreset::RiscOS_3_50,
                              SystemROMPreset::RiscOS_3_60,
                              SystemROMPreset::RiscOS_3_70,
                              SystemROMPreset::RiscOS_3_71, },
                            selectedRom);
        break;
    }

    return selectedRom;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// NewSessionDialog Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
NewSessionDialog::NewSessionDialog(QWidget *ownerWidget,
                                   const Arm::Options *initialOpts /*= nullptr*/) :
    QDialog(ownerWidget),
    _startPaused(true)
{
    _ui.setupUi(this);

    if (initialOpts == nullptr)
    {
        // Set defaults.
    }
    else
    {
        _options = *initialOpts;
    }

    fillComboBoxFromEnum(*_ui._sysArchList, Mo::Arm::getSystemModelType(),
                         _options.getHardwareArchitecture());

    onArchitectureChanged(_options.getHardwareArchitecture());
    selectComboItem(*_ui._cpuList, _options.getProcessorVariant());
    _ui._cpuSpeed->setValue(static_cast<int>(_options.getProcessorSpeedMHz()));
    selectComboRawItem(*_ui._ramSizeList, _options.getRamSizeKb());
    selectComboItem(*_ui._systemRomPresetList, _options.getSystemRom());

    onSystemRomChanged(_options.getSystemRom());

    _ui._startPausedCheckBox->setChecked(_startPaused);

    connect(_ui._sysArchList, &QComboBox::currentIndexChanged,
            this, &NewSessionDialog::onSysArchSelected);
    connect(_ui._systemRomPresetList, &QComboBox::currentIndexChanged,
            this, &NewSessionDialog::onSysRomSelected);
    connect(_ui._selectSysRomImageButton, &QAbstractButton::clicked,
            this, &NewSessionDialog::onBrowseForSysRomImage);
}

bool NewSessionDialog::isStartPaused() const
{
    return _startPaused;
}

const Arm::Options &NewSessionDialog::getOptions() const
{
    return _options;
}

void NewSessionDialog::showEvent(QShowEvent *args)
{
    QDialog::showEvent(args);

    _ui._selectSysRomImageButton->setMaximumWidth(_ui._selectSysRomImageButton->height());
    setMaximumHeight(height());
}

void NewSessionDialog::accept()
{
    updateOptionsFromUI();

    Ag::String error;

    if (_options.validate(error))
    {
        QDialog::accept();
    }
    else
    {
        QString messageText = Ag::Qt::toQString(error);

        QMessageBox::warning(this, tr("Invalid Session Options"),
                             messageText, QMessageBox::Ok);
    }
}

void NewSessionDialog::onSysArchSelected(int selectedItem)
{
    bool isOK = false;
    uint32_t rawSysArch = _ui._sysArchList->itemData(selectedItem).toUInt(&isOK);

    if (isOK)
    {
        onArchitectureChanged(Ag::forceFromScalar<SystemModel>(rawSysArch));
    }
}

void NewSessionDialog::onSysRomSelected(int selectedItem)
{
    bool isOK = false;
    uint32_t rawSysRom = _ui._systemRomPresetList->itemData(selectedItem).toUInt(&isOK);

    if (isOK)
    {
        onSystemRomChanged(Ag::forceFromScalar<SystemROMPreset>(rawSysRom));
    }
}

void NewSessionDialog::onBrowseForSysRomImage()
{
    QFileDialog browser(this);
    browser.setWindowTitle(tr("Select System ROM Image"));
    browser.setFileMode(QFileDialog::ExistingFile);
    browser.setNameFilters({ tr("ROM Images (*.rom *.bin)"),
                             tr("All Files (*.*)")
                           });

    if (browser.exec() == QDialog::Accepted)
    {
        QStringList result = browser.selectedFiles();

        if (result.isEmpty() == false)
        {
            _ui._systemRomCustomPath->setText(QDir::toNativeSeparators(result.first()));
        }
    }
}

void NewSessionDialog::onArchitectureChanged(Arm::SystemModel arch)
{
    // The architecture has changed. Update related controls.
    fillCPUList(*_ui._cpuList, arch);
    fillRAMList(*_ui._ramSizeList, arch);

    SystemROMPreset romType = fillROMSelection(*_ui._systemRomPresetList, arch);

    onSystemRomChanged(romType);
}

void NewSessionDialog::onSystemRomChanged(Arm::SystemROMPreset romPreset)
{
    bool isCustom = (romPreset == SystemROMPreset::Custom);

    _ui._systemRomCustomPath->setEnabled(isCustom);
    _ui._selectSysRomImageButton->setEnabled(isCustom);

    QString romPath = Ag::Qt::toQString(_options.getRomPath());
    _ui._systemRomCustomPath->setText(romPath);
}

void NewSessionDialog::updateOptionsFromUI()
{
    _options.setHardwareArchitecture(getSelectedItem<SystemModel>(*_ui._sysArchList));
    _options.setProcessorVariant(getSelectedItem<ProcessorModel>(*_ui._cpuList));
    _options.setProcessorSpeedMHz(static_cast<uint16_t>(_ui._cpuSpeed->value()));
    _options.setRamSizeKb(getSelectedData(*_ui._ramSizeList));
    _options.setSystemRom(getSelectedItem<SystemROMPreset>(*_ui._systemRomPresetList));

    if (_options.getSystemRom() == SystemROMPreset::Custom)
    {
        _options.setCustomRom(Ag::Qt::pathFromQString(_ui._systemRomCustomPath->text()));
    }

    _startPaused = _ui._startPausedCheckBox->isChecked();
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


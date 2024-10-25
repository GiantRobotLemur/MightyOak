//! @file ArmDebugger/UI/RegisterViewWidget.cpp
//! @brief The definition of a widget which displays the current status of the
//! processor registers.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "RegisterViewWidget.hpp"

#include "Ag/Core/Format.hpp"
#include "Ag/Core/Utils.hpp"
#include "Ag/Core/VariantTypes.hpp"

#include "DebuggerApp.hpp"
#include "EmulatorSession.hpp"

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

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// RegisterViewWidget Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
RegisterViewWidget::RegisterViewWidget(QWidget *ownerWidget) :
    QWidget(ownerWidget)
{
    _ui.setupUi(this);

    _registers.reserve(16);
    _registers.emplace_back(_ui._r0Label, _ui._r0Value, Arm::CoreRegister::R0);
    _registers.emplace_back(_ui._r1Label, _ui._r1Value, Arm::CoreRegister::R1);
    _registers.emplace_back(_ui._r2Label, _ui._r2Value, Arm::CoreRegister::R2);
    _registers.emplace_back(_ui._r3Label, _ui._r3Value, Arm::CoreRegister::R3);
    _registers.emplace_back(_ui._r4Label, _ui._r4Value, Arm::CoreRegister::R4);
    _registers.emplace_back(_ui._r5Label, _ui._r5Value, Arm::CoreRegister::R5);
    _registers.emplace_back(_ui._r6Label, _ui._r6Value, Arm::CoreRegister::R6);
    _registers.emplace_back(_ui._r7Label, _ui._r7Value, Arm::CoreRegister::R7);
    _registers.emplace_back(_ui._r8Label, _ui._r8Value, Arm::CoreRegister::R8);
    _registers.emplace_back(_ui._r9Label, _ui._r9Value, Arm::CoreRegister::R9);
    _registers.emplace_back(_ui._r10Label, _ui._r10Value, Arm::CoreRegister::R10);
    _registers.emplace_back(_ui._r11Label, _ui._r11Value, Arm::CoreRegister::R11);
    _registers.emplace_back(_ui._r12Label, _ui._r12Value, Arm::CoreRegister::R12);
    _registers.emplace_back(_ui._r13Label, _ui._r13Value, Arm::CoreRegister::R13);
    _registers.emplace_back(_ui._r14Label, _ui._r14Value, Arm::CoreRegister::R14);
    _registers.emplace_back(_ui._r15Label, _ui._r15Value, Arm::CoreRegister::R15);
    _registers.emplace_back(_ui._psrLabel, _ui._psrValue, Arm::CoreRegister::CPSR);
    _registers.emplace_back(_ui._pcLabel, _ui._pcValue, Arm::CoreRegister::PC);

    _psrBits.reserve(6);
    _psrBits.emplace_back(31, _ui._nLabel, _ui._nCheckBox);
    _psrBits.emplace_back(30, _ui._zLabel, _ui._zCheckBox);
    _psrBits.emplace_back(29, _ui._cLabel, _ui._cCheckBox);
    _psrBits.emplace_back(28, _ui._vLabel, _ui._vCheckBox);
    _psrBits.emplace_back(26, _ui._iLabel, _ui._iCheckBox);
    _psrBits.emplace_back(25, _ui._fLabel, _ui._fCheckBox);

    if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        auto *session = &app->getSession();

        connect(session, &EmulatorSession::sessionStarted,
                this, &RegisterViewWidget::onSessionStarted);
        connect(session, &EmulatorSession::sessionEnded,
                this, &RegisterViewWidget::onSessionEnded);
        connect(session, &EmulatorSession::sessionPaused,
                this, &RegisterViewWidget::onSessionPaused);
        connect(session, &EmulatorSession::sessionResumed,
                this, &RegisterViewWidget::onSessionResumed);
        connect(session, &EmulatorSession::sessionSingleStep,
                this, &RegisterViewWidget::onSingleStep);
    }

    enableEditing(false);
    _ui._modeField->setText(tr("Not running"));
}

void RegisterViewWidget::onSessionStarted(const Arm::Options &options,
                                          Arm::IArmSystem *emulator)
{
    // Define the location of the Interrupt and Fast interrupt disable
    // flags in the PSR.
    uint8_t iFlag = 26;
    uint8_t fFlag = 25;

    if (options.getProcessorVariant() >= Arm::ProcessorModel::ARM610)
    {
        iFlag = 7;
        fFlag = 6;
    }

    for (auto &psrBitWidget : _psrBits)
    {
        if (psrBitWidget.Label == _ui._iLabel)
            psrBitWidget.Bit = iFlag;
        else if (psrBitWidget.Label == _ui._fLabel)
            psrBitWidget.Bit = fFlag;
    }

    sampleState(emulator);
    enableEditing(true);
}

void RegisterViewWidget::onSessionEnded(Arm::IArmSystem */*emulator*/)
{
    enableEditing(false);
    _ui._modeField->setText(tr("Not running"));

    // TODO: Blank the register contents?
}

void RegisterViewWidget::onSessionPaused(Arm::IArmSystem *emulator)
{
    sampleState(emulator);
    enableEditing(true);
}

void RegisterViewWidget::onSessionResumed(Arm::IArmSystem *emulator)
{
    // TODO: Commit changes to the registers?
    sampleState(emulator);
    enableEditing(false);

    _ui._modeField->setText(tr("Running"));
}

void RegisterViewWidget::onSingleStep(Arm::IArmSystem *emulator)
{
    sampleState(emulator);
}

RegisterViewWidget::RegisterWidget::RegisterWidget() :
    Label(nullptr),
    Field(nullptr),
    Register(Arm::CoreRegister::Max),
    IsReadOnly(false)
{
}

RegisterViewWidget::RegisterWidget::RegisterWidget(QLabel *label,
                                                   QLineEdit *field,
                                                   Arm::CoreRegister reg) :
    Label(label),
    Field(field),
    Register(reg),
    IsReadOnly((field ==nullptr) || field->isReadOnly())
{
}

RegisterViewWidget::PsrFlagWidget::PsrFlagWidget() :
    Label(nullptr),
    Field(nullptr),
    Bit(0)
{
}

RegisterViewWidget::PsrFlagWidget::PsrFlagWidget(uint8_t bit, QLabel *label,
                                                 QCheckBox *field) :
    Label(label),
    Field(field),
    Bit(bit)
{
}

void RegisterViewWidget::sampleState(Arm::IArmSystem *emulator)
{
    Ag::FormatInfo format(Ag::LocaleInfo::getNeutral());
    std::string buffer;
    buffer.reserve(12);
    buffer.push_back('0');
    buffer.push_back('x');

    format.setMinimumWholeDigits(8);
    format.setRadix(16);
    format.enableUpperCase(true);

    for (auto &&regWidget : _registers)
    {
        uint32_t value = emulator->getCoreRegister(regWidget.Register);

        // Truncate the buffer back to the hexadecimal prefix.
        buffer.erase(2);

        // Format the value using upper case, like Qt can't ;-(
        Ag::appendValue(format, buffer, value);
        QString text = QString::fromUtf8(buffer.c_str(), buffer.length());

        regWidget.Field->setText(text);
    }

    std::string_view modeText = Arm::getProcessorModeType().toDisplayName(emulator->getMode());
    _ui._modeField->setText(QString::fromUtf8(modeText.data(), modeText.length()));

    uint32_t psr = emulator->getCoreRegister(Arm::CoreRegister::CPSR);

    for (const auto &psrFlag : _psrBits)
    {
        const uint32_t mask = 1u << psrFlag.Bit;

        psrFlag.Field->setChecked((psr & mask) != 0);
    }
}

void RegisterViewWidget::enableEditing(bool canEdit)
{
    for (const auto &reg : _registers)
    {
        reg.Field->setEnabled(canEdit);
    }

    for (const auto &psrFlag : _psrBits)
    {
        psrFlag.Field->setEnabled(canEdit);
    }

    _ui._modeField->setEnabled(canEdit);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


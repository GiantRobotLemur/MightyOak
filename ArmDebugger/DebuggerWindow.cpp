//! @file ArmDebugger/UI/DebuggerWindow.cpp
//! @brief The definition of an object representing the main application window.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>

#include <QLayout>
#include <QBoxLayout>

#include "Ag/QtInterop/Conversion.hpp"

#include "ActionManager.hpp"
#include "DebuggerActions.hpp"
#include "DebuggerApp.hpp"
#include "DebuggerWindow.hpp"
#include "EmulatorSession.hpp"
#include "UI/MemoryViewWidget.hpp"
#include "UI/OutputViewWidget.hpp"
#include "UI/RegisterViewWidget.hpp"


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
// DebuggerWindow Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
DebuggerWindow::DebuggerWindow() :
    QMainWindow(nullptr),
    _registersDock(nullptr),
    _outputDock(nullptr),
    _memoryView(nullptr),
    _registersView(nullptr),
    _outputView(nullptr),
    _memoryRegionList(nullptr),
    _gotoAddrField(nullptr)
{
    setupWindow();

    if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        setupWidgets(app);
        setupMenus(app);
    }
}

void DebuggerWindow::closeEvent(QCloseEvent *args)
{
    if (auto app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
    {
        if (app->tryExit() == false)
        {
            // The use cancelled the closure.
            args->ignore();
        }
    }
}

void DebuggerWindow::onSessionStarted(const Arm::Options &options, Arm::IArmSystem *emulator)
{
    auto &addrMappings = emulator->getReadAddresses().getMappings();

    _memoryRegionList->clear();

    const QString regionFormat = tr("%1 (0x%2-0x%3)");
    const QChar padding('0');
    QVariant itemData;
    MemoryRegion logicalRAM;

    if (options.getHardwareArchitecture() != Arm::SystemModel::TestBed)
    {
        // Add a region for logical memory.
        QString itemText = regionFormat.arg(tr("Logical RAM"))
            .arg(0, 8, 16, padding)
            .arg(0x1000000, 8, 16, padding);

        logicalRAM.BaseAddress = 0;
        logicalRAM.Length = 0x1000000;
        logicalRAM.IsLogical = true;

        itemData.setValue(logicalRAM);
        _memoryRegionList->addItem(itemText, itemData);

        // TODO: Do something sane for ARMv3+ architectures.
    }

    // Tell the memory view which memory addresses are logically mapped.
    _memoryView->setLogicallyMappedMemory(logicalRAM);

    for (const Arm::AddressMap::Mapping &mapping : addrMappings)
    {
        QString regionName = Ag::Qt::toQString(mapping.Region->getName());

        QString itemText = regionFormat.arg(regionName)
            .arg(mapping.Address, 8, 16, padding)
            .arg(mapping.End, 8, 16, padding);

        itemData.setValue(MemoryRegion(mapping.Address,
                                       mapping.End - mapping.Address,
                                       false));

        _memoryRegionList->addItem(itemText, itemData);
    }

    _memoryRegionList->setEnabled(_memoryRegionList->count() > 0);
}

void DebuggerWindow::onSessionEnded(Arm::IArmSystem */*emulator*/)
{
    _memoryRegionList->clear();
    _memoryRegionList->setDisabled(true);

    _gotoAddrField->setText("0x00000000");
    _gotoAddrField->setDisabled(true);
}

void DebuggerWindow::onMemoryRegionSelected(int regionIndex)
{
    QVariant itemData = _memoryRegionList->itemData(regionIndex);
    MemoryRegion region = itemData.value<MemoryRegion>();

    _memoryView->displayRegion(region);
}

void DebuggerWindow::setupWindow()
{
    setWindowTitle(QCoreApplication::applicationName());
    setWindowIcon(QIcon(":/images/ArmDebugger.svg"));
    setMinimumSize(700, 500);

    statusBar()->showMessage(tr("Ready..."));
}

void DebuggerWindow::setupWidgets(DebuggerApp *app)
{
    const auto &actions = app->getActions();

    _memoryView = new MemoryViewWidget(this, app->getSession().getSettings());
    setCentralWidget(_memoryView);

    // Create memory view toolbar.
    QToolBar *codeViewBar = addToolBar(tr("Code View"));
    codeViewBar->setAllowedAreas(Qt::ToolBarArea::TopToolBarArea |
                                    Qt::ToolBarArea::BottomToolBarArea);
    QLayout *layout = codeViewBar->layout();
    layout->setSpacing(4);

    QLabel *regionLabel = new QLabel(codeViewBar);
    regionLabel->setText(tr("Region:"));
    codeViewBar->addWidget(regionLabel);

    _memoryRegionList = new QComboBox(codeViewBar);
    QSizePolicy sizePolicy = _memoryRegionList->sizePolicy();
    sizePolicy.setHorizontalStretch(1);
    _memoryRegionList->setSizePolicy(sizePolicy);
    codeViewBar->addWidget(_memoryRegionList);
    connect(_memoryRegionList, &QComboBox::currentIndexChanged,
            this, &DebuggerWindow::onMemoryRegionSelected);

    codeViewBar->addAction(actions.getAction(Action::ResumeSession));
    codeViewBar->addAction(actions.getAction(Action::PauseSession));
    codeViewBar->addAction(actions.getAction(Action::ExecOver));
    codeViewBar->addAction(actions.getAction(Action::ExecInto));
    codeViewBar->addAction(actions.getAction(Action::ExecOut));

    _gotoAddrField = new QLineEdit(codeViewBar);
    _gotoAddrField->setText("0x00000000");
    codeViewBar->addWidget(_gotoAddrField);

    codeViewBar->addAction(actions.getAction(Action::GotoPC));

    // Create side panel.
    _registersDock = new QDockWidget(tr("Registers"), this);
    _registersDock->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea |
                                    Qt::DockWidgetArea::RightDockWidgetArea);
    _registersView = new RegisterViewWidget(_registersDock);
    _registersDock->setWidget(_registersView);

    _outputDock = new QDockWidget(tr("Output"), this);
    _outputDock->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea |
                                    Qt::DockWidgetArea::RightDockWidgetArea);

    _outputView = new OutputViewWidget(_outputDock);
    _outputDock->setWidget(_outputView);

    // NOTE: Keep to this ordering for the registers to appear first.
    addDockWidget(Qt::LeftDockWidgetArea, _registersDock);
    addDockWidget(Qt::LeftDockWidgetArea, _outputDock);
    tabifyDockWidget(_outputDock, _registersDock);

    onSessionEnded(nullptr);

    // Connect to the emulator.
    auto &emulator = app->getSession();
    connect(&emulator, &EmulatorSession::sessionStarted,
            this, &DebuggerWindow::onSessionStarted);
    connect(&emulator, &EmulatorSession::sessionEnded,
            this, &DebuggerWindow::onSessionEnded);
}

void DebuggerWindow::setupMenus(DebuggerApp *app)
{
    const auto &actions = app->getActions();

    QMenu *currentMenu = menuBar()->addMenu(tr("&File"));
    currentMenu->addAction(actions.getAction(Action::NewSession));
    currentMenu->addAction(actions.getAction(Action::OpenSession));
    currentMenu->addAction(actions.getAction(Action::SaveSession));
    currentMenu->addAction(actions.getAction(Action::SaveSessionAs));
    currentMenu->addSeparator();
    currentMenu->addAction(actions.getAction(Action::Exit));

    currentMenu = menuBar()->addMenu(tr("&Edit"));
    currentMenu->addAction(actions.getAction(Action::EditSWIs));
    currentMenu->addAction(actions.getAction(Action::EditLabels));

    currentMenu = menuBar()->addMenu(tr("&View"));
    QAction *action = currentMenu->addAction(tr("Show &Registers"));
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::toggled, _registersDock, &QDockWidget::setVisible);

    action = currentMenu->addAction(tr("Show &Output"));
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::toggled, _outputDock, &QDockWidget::setVisible);

    currentMenu->addAction(actions.getAction(Action::EditMemoryDisplayOptions));

    currentMenu = menuBar()->addMenu(tr("&Session"));
    currentMenu->addAction(actions.getAction(Action::ResumeSession));
    currentMenu->addAction(actions.getAction(Action::PauseSession));
    currentMenu->addAction(actions.getAction(Action::StopSession));
    currentMenu->addAction(actions.getAction(Action::RestartSession));

    currentMenu = menuBar()->addMenu(tr("&Debug"));
    currentMenu->addAction(actions.getAction(Action::ExecInto));
    currentMenu->addAction(actions.getAction(Action::ExecOver));
    currentMenu->addAction(actions.getAction(Action::ExecOut));

    currentMenu = menuBar()->addMenu(tr("&Help"));
    currentMenu->addAction(actions.getAction(Action::About));
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


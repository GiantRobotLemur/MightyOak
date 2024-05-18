//! @file ArmDebugger/DebuggerApp.cpp
//! @brief The definition of an object representing the root of the application
//! object hierarchy.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <QFileDialog>
#include <QMessageBox>

#include "Ag/Core/Utils.hpp"

#include "CommandLineOptions.hpp"
#include "DebuggerApp.hpp"
#include "DebuggerWindow.hpp"
#include "DebuggerActions.hpp"
#include "Version.hpp"

#include "SessionSettings.hpp"
#include "UI/AboutDialog.hpp"
#include "UI/NewSessionDialog.hpp"
#include "UI/EditOrdinalMappingDialog.hpp"
#include "UI/EditDisplayOptionsDialog.hpp"

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
// DebuggerApp Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
DebuggerApp::DebuggerApp(int &argc, char *argv[]) :
    QApplication(argc, argv),
    _session(this),
    _isModified(false)
{
    QCoreApplication::setApplicationName("ARM Debugger");
    QCoreApplication::setApplicationVersion(MIGHTY_OAK_VERSION_TEXT);
    QCoreApplication::setOrganizationName("GiantRobotLemur");
    QCoreApplication::setOrganizationDomain("na-se.co.uk");

    connect(&_session, &EmulatorSession::sessionPaused,
            this, &DebuggerApp::onEmulatorStopped);
    connect(&_session, &EmulatorSession::sessionResumed,
            this, &DebuggerApp::onEmulatorRunning);
    connect(&_session, &EmulatorSession::sessionStarted,
            this, &DebuggerApp::onEmulatorCreated);
    connect(&_session, &EmulatorSession::sessionEnded,
            this, &DebuggerApp::onEmulatorDestroyed);
}

const ActionManager &DebuggerApp::getActions() const
{
    return _actions;
}

QMainWindow *DebuggerApp::getMainWindow() const
{
    return _mainWindow.get();
}

EmulatorSession &DebuggerApp::getSession()
{
    return _session;
}

const EmulatorSession &DebuggerApp::getSession() const
{
    return _session;
}

const QString &DebuggerApp::getSessionFileName() const
{
    return _sessionFileName;
}

bool DebuggerApp::tryExit()
{
    bool canExit = true;

    if (_session.getEmulator())
    {
        int result = promptToEndRunningSession();

        if (result == QMessageBox::Cancel)
        {
            canExit = false;
        }
        else if (result == QMessageBox::Save)
        {
            // Don't exit if the user cancels.
            canExit = saveSession(false);
        }

        if (canExit)
        {
            // Ensure the session is stopped.
            _session.destroy();
        }
    }

    return canExit;
}

bool DebuggerApp::initialise(const QStringList &cliArgs, int &processResult)
{
    bool canContinue = false;
    processResult = 1;

    // Process command line arguments.
    CommandLineOptions options;
    QString message;
    QString title;

    if (options.tryParse(cliArgs, message))
    {
        processResult = 0;

        if (options.needsUI())
        {
            if (options.getSessionFileName().isEmpty() == false)
            {
                if (_session.getSettings().tryLoad(options.getSessionFileName(), message))
                {
                    _sessionFileName = options.getSessionFileName();
                }
                else
                {
                    title = tr("Session Settings Failure");
                    processResult = 1;
                }
            }

            CliAction action = options.getAction();

            if (action == CliAction::ConfigureSession)
            {
                NewSessionDialog sessionDialog(_mainWindow.get(),
                                               &_session.getSessionOptions());

                if (sessionDialog.exec() == QDialog::Accepted)
                {
                    // Create a new emulation session based on the new options.
                    _session.getSettings().setEmulatorOptions(sessionDialog.getOptions());

                    action = sessionDialog.isStartPaused() ? CliAction::ConfigureSession :
                                                             CliAction::RunSession;
                    canContinue = true;
                }
                else
                {
                    processResult = 0;
                }
            }
            else
            {
                canContinue = true;
            }

            if (canContinue)
            {
                defineActions();
                onEmulatorDestroyed();

                // Create application window.
                DebuggerWindow *window = new DebuggerWindow();
                _mainWindow = window;

                updateTitle();
                window->show();

                if (action != CliAction::None)
                {
                    // Create an emulator session.
                    _session.create(_session.getSessionOptions());

                    if (action == CliAction::RunSession)
                    {
                        // TODO: Set the session running.
                        // _session.resume();
                    }
                }
            }
        }
    }
    else
    {
        title = tr("Invalid Command Line");
    }

    if (processResult != 0)
    {
        if (Ag::isStdoutEnabled())
        {
            // Write the error to the console.
            QByteArray narrowTitle = title.toLocal8Bit();
            QByteArray narrowError = message.toLocal8Bit();
            printf("%s: %s\n", narrowTitle.data(), narrowError.data());
        }
        else
        {
            // Show the error in the UI.
            QMessageBox::critical(nullptr, title, message, QMessageBox::Ok);
        }

        canContinue = false;
    }

    return canContinue;
}

void DebuggerApp::onNewSession()
{
    NewSessionDialog sessionDialog(_mainWindow.get(),
                                   &_session.getSessionOptions());

    if (sessionDialog.exec() == QDialog::Accepted)
    {
        _isModified = true;
        _sessionFileName.clear();
        updateTitle();

        // Create a new emulation session based on the new options.
        _session.create(sessionDialog.getOptions());
    }
}

void DebuggerApp::onOpenSession()
{
    if (_session.getEmulator())
    {
        int result = promptToEndRunningSession();

        if (result == QMessageBox::Cancel)
        {
            return;
        }
        else if (result == QMessageBox::Save)
        {
            // The user cancelled when prompted or the save failed.
            if (saveSession(false) == false)
                return;
        }

        _sessionFileName.clear();
    }

    QFileDialog fileBrowser(_mainWindow);

    fileBrowser.setFileMode(QFileDialog::ExistingFile);
    fileBrowser.setNameFilters({ tr("Session files (*.session)"),
                                 tr("All files (*)") });
    fileBrowser.setWindowTitle(tr("Open Session File"));

    if (fileBrowser.exec() == QDialog::Accepted)
    {
        QString message;
        QString fileName = fileBrowser.selectedFiles().first();

        if (_session.getSettings().tryLoad(fileName, message))
        {
            _session.destroy();
            _sessionFileName = QDir::toNativeSeparators(fileName);
            _isModified = false;

            updateTitle();
        }
        else
        {
            QMessageBox::warning(_mainWindow, tr("Load Session Failure"),
                                 message, QMessageBox::Ok);
        }
    }
}

void DebuggerApp::onSaveSession()
{
    if (saveSession(false))
    {
        updateTitle();
    }
}

void DebuggerApp::onSaveSessionAs()
{
    if (saveSession(true))
    {
        updateTitle();
    }
}

void DebuggerApp::onEditSWIs()
{
    EditOrdinalMappingDialog editMappings(_mainWindow);
    editMappings.initialise(tr("SWI Definition"), tr("Number"), tr("SWI Name"),
                            _session.getSettings().getSwiMap(), "swi");

    if (editMappings.exec() == QDialog::Accepted)
    {
        // Update SWI ordinals.
        _session.getSettings().setSwiMap(editMappings.getMappings());
        _isModified = true;
        updateTitle();
    }
}

void DebuggerApp::onEditLabels()
{
    EditOrdinalMappingDialog editMappings(_mainWindow);
    editMappings.initialise(tr("Address Label"), tr("Address"), tr("Symbol"),
                            _session.getSettings().getSymbolMap(), "sym");

    if (editMappings.exec() == QDialog::Accepted)
    {
        // Update address symbols.
        _session.getSettings().setSymbolMap(editMappings.getMappings());
        _isModified = true;
        updateTitle();
    }
}

void DebuggerApp::onEditDisplayOptions()
{
    EditDisplayOptionsDialog editDisplay(_mainWindow);
    editDisplay.setOptions(_session.getSettings().getMemoryViewOptions());

    if (editDisplay.exec() == QDialog::Accepted)
    {
        // Update the view options.
        _session.getSettings().setMemoryViewOptions(editDisplay.getOptions());

        // TODO: Emit signal to update the UI.

        // Mark the session as modified.
        _isModified = true;
        updateTitle();
    }
}

void DebuggerApp::onShowHelpAbout()
{
    AboutDialog aboutDialog(_mainWindow.get());

    aboutDialog.exec();
}

void DebuggerApp::onExit()
{
    if (tryExit())
    {
        exit(0);
    }
}

void DebuggerApp::onEmulatorCreated()
{
    _actions.updateActionState(true, {
        Action::ResumeSession,
        Action::RestartSession,
        Action::StopSession,
        Action::ExecOver,
        Action::ExecInto,
        Action::ExecOut,
        Action::GotoPC,
     });

    _actions.updateActionState(false, {
        Action::PauseSession });
}

void DebuggerApp::onEmulatorDestroyed()
{
    _actions.updateActionState(false, {
        Action::ResumeSession,
        Action::PauseSession,
        Action::RestartSession,
        Action::StopSession,
        Action::ExecOver,
        Action::ExecInto,
        Action::ExecOut,
        Action::GotoPC,
    });
}

void DebuggerApp::onEmulatorRunning()
{
    updateActions(true);
}

void DebuggerApp::onEmulatorStopped()
{
    updateActions(false);
}

void DebuggerApp::defineActions()
{
    _actions.reserve(16);

    QAction *currentAction = _actions.addAction(Action::NewSession, ActionGroup::Application, tr("&New"), ":/images/NewSession.svg");
    currentAction->setShortcut(QKeySequence(QKeySequence::New));
    currentAction->setToolTip(tr("Creates a new emulator session."));
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onNewSession);

    currentAction = _actions.addAction(Action::OpenSession, ActionGroup::Application, tr("&Open"), ":/images/OpenSession.svg");
    currentAction->setShortcut(QKeySequence(QKeySequence::Open));
    currentAction->setToolTip("Opens a new emulator session from a file.");
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onOpenSession);

    currentAction = _actions.addAction(Action::SaveSession, ActionGroup::Application, tr("&Save"), ":/images/SaveSession.svg");
    currentAction->setShortcut(QKeySequence(QKeySequence::Save));
    currentAction->setToolTip("Save the current emulator session settings and annotations.");
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onSaveSession);

    currentAction = _actions.addAction(Action::SaveSessionAs, ActionGroup::Application, tr("Save &As..."), ":/images/SaveSessionAs.svg");
    currentAction->setShortcut(QKeySequence(QKeySequence::SaveAs));
    currentAction->setToolTip("Save the current emulator session settings and annotations as a new file.");
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onSaveSessionAs);

    currentAction = _actions.addAction(Action::Exit, ActionGroup::Application, tr("&Exit"), ":/images/Exit.svg");
    currentAction->setToolTip("Stops the current emulator session and closes the application.");
#ifdef _WIN32
    currentAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_F4));
#else
    currentAction->setShortcut(QKeySequence(QKeySequence::Quit));
#endif
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onExit);

    currentAction = _actions.addAction(Action::EditSWIs, ActionGroup::Session, tr("Edit &SWI Names"));
    currentAction->setToolTip("Edit the mapping from SWI numbers to names.");
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onEditSWIs);

    currentAction = _actions.addAction(Action::EditLabels, ActionGroup::Session, tr("Edit &Address Labels"));
    currentAction->setToolTip("Edit symbolic address labels.");
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onEditLabels);

    currentAction = _actions.addAction(Action::EditMemoryDisplayOptions, ActionGroup::Session, tr("Memory &Display Options..."));
    currentAction->setToolTip("Edit options for displaying memory contents.");
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onEditDisplayOptions);

    currentAction = _actions.addAction(Action::ResumeSession, ActionGroup::Session, tr("&Continue"), ":/images/ResumeSession.svg");
    currentAction->setShortcut(QKeySequence(Qt::Key_F5));
    currentAction->setToolTip("Resume execution of the current emulator session.");
    connect(currentAction, &QAction::triggered, &_session, &EmulatorSession::resume);

    currentAction = _actions.addAction(Action::PauseSession, ActionGroup::Session, tr("&Pause"), ":/images/PauseSession.svg");
    currentAction->setShortcut(QKeySequence(Qt::Key_F6));
    currentAction->setToolTip("Pause execution of the current emulator session.");
    connect(currentAction, &QAction::triggered, &_session, &EmulatorSession::pause);

    currentAction = _actions.addAction(Action::StopSession, ActionGroup::Session, tr("&Stop"), ":/images/StopSession.svg");
    currentAction->setToolTip("Stops execution of the current emulator session.");
    connect(currentAction, &QAction::triggered, &_session, &EmulatorSession::stop);

    currentAction = _actions.addAction(Action::RestartSession, ActionGroup::Session, tr("&Restart"), ":/images/RestartSession.svg");
    currentAction->setToolTip("Starts a new emulator session using current settings.");
    connect(currentAction, &QAction::triggered, &_session, &EmulatorSession::restart);

    currentAction = _actions.addAction(Action::ExecOver, ActionGroup::Debug, tr("&Step Over"), ":/images/ExecOver.svg");
    currentAction->setShortcut(QKeySequence(Qt::Key_F10));
    currentAction->setToolTip("Executes to the instruction at the next address, skipping branches.");
    connect(currentAction, &QAction::triggered, &_session, &EmulatorSession::stepOver);

    currentAction = _actions.addAction(Action::ExecInto, ActionGroup::Debug, tr("&Step Into"), ":/images/ExecInto.svg");
    currentAction->setShortcut(QKeySequence(Qt::Key_F11));
    currentAction->setToolTip("Executes to the next instruction following branches.");
    connect(currentAction, &QAction::triggered, &_session, &EmulatorSession::step);

    currentAction = _actions.addAction(Action::ExecOut, ActionGroup::Debug, tr("&Step Out"), ":/images/ExecOut.svg");
    currentAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F11));
    currentAction->setToolTip("Executes until the current subroutine returns.");
    connect(currentAction, &QAction::triggered, &_session, &EmulatorSession::stepOut);

    currentAction = _actions.addAction(Action::GotoPC, ActionGroup::CodeView, tr("&Goto PC"), ":/images/GotoPC.svg");
    currentAction->setShortcut(QKeySequence(Qt::Key_F12));
    currentAction->setToolTip("Displays the instruction at the current program counter address.");

    currentAction = _actions.addAction(Action::About, ActionGroup::Help, tr("&About..."));
    currentAction->setShortcut(QKeySequence(QKeySequence::HelpContents));
    currentAction->setToolTip("Displays information about the program.");
    connect(currentAction, &QAction::triggered, this, &DebuggerApp::onShowHelpAbout);

    _actions.reindex();
}

int DebuggerApp::promptToEndRunningSession()
{
    QMessageBox prompt(_mainWindow);
    prompt.setIcon(QMessageBox::Question);
    prompt.setWindowTitle(tr("Session in Progress"));
    prompt.setText(tr("A session is currently running.\nWould you like to stop it and optionally save its settings?"));
    prompt.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    prompt.setDefaultButton(QMessageBox::Save);

    return prompt.exec();
}

bool DebuggerApp::saveSession(bool forceNewFile)
{
    bool isSaved = false;
    QString fileName = _sessionFileName;

    if (forceNewFile || fileName.isEmpty())
    {
        // Prompt the user for a file name to save to.
        QFileDialog fileBrowser(_mainWindow);

        fileBrowser.setFileMode(QFileDialog::AnyFile);
        fileBrowser.setNameFilters({ tr("Session files (*.session)"),
                                     tr("All files (*)") });
        fileBrowser.setWindowTitle(tr("Save File As"));
        fileBrowser.setAcceptMode(QFileDialog::AcceptSave);

        if (fileBrowser.exec() == QDialog::Accepted)
        {
            fileName = fileBrowser.selectedFiles().first();
        }
        else
        {
            fileName.clear();
        }
    }

    if (fileName.isEmpty() == false)
    {
        // Attempt to save.
        QString message;

        if (_session.getSettings().trySave(fileName, message))
        {
            isSaved = true;
            _sessionFileName = fileName;
            _isModified = false;
        }
        else
        {
            QMessageBox::warning(_mainWindow, tr("Save Error"),
                                 message, QMessageBox::Ok);
        }
    }

    return isSaved;
}

void DebuggerApp::updateTitle()
{
    if (_mainWindow)
    {
        QString title = QCoreApplication::applicationName();

        if (_sessionFileName.isEmpty() == false)
        {
            title.append(" - ");
            title.append(_sessionFileName);
        }

        if (_isModified)
        {
            title.append(' ');
            title.append(tr("(modified)"));
        }

        _mainWindow->setWindowTitle(title);
    }
}

void DebuggerApp::updateActions(bool isEmulatorRunning)
{
    _actions.updateActionState(isEmulatorRunning, {
        Action::PauseSession,
        Action::StopSession
    });

    _actions.updateActionState(!isEmulatorRunning, {
        Action::ResumeSession,
        Action::RestartSession,
        Action::ExecOver,
        Action::ExecInto,
        Action::ExecOut,
        Action::GotoPC,
    });
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////


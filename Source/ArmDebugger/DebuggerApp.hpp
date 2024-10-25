//! @file ArmDebugger/DebuggerApp.hpp
//! @brief The declaration of an object representing the root of the application
//! object hierarchy.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_APP_HPP__
#define __ARM_DEBUGGER_APP_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QApplication>
#include <QMainWindow>
#include <QPointer>

#include "ActionManager.hpp"
#include "EmulatorSession.hpp"
#include "SessionSettings.hpp"

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
//! @brief An object representing the root of the application object hierarchy.
class DebuggerApp : public QApplication
{
Q_OBJECT
public:
    // Construction/Destruction
    DebuggerApp(int &argc, char *argv[]);
    virtual ~DebuggerApp() = default;

    // Accessors
    const ActionManager &getActions() const;
    QMainWindow *getMainWindow() const;
    EmulatorSession &getSession();
    const EmulatorSession &getSession() const;
    const QString &getSessionFileName() const;

    // Operations
    bool tryExit();
    bool initialise(const QStringList &cliArgs, int &processResult);
private slots:
    void onNewSession();
    void onOpenSession();
    void onSaveSession();
    void onSaveSessionAs();
    void onEditSWIs();
    void onEditLabels();
    void onEditDisplayOptions();
    void onShowHelpAbout();
    void onExit();

    void onEmulatorCreated();
    void onEmulatorDestroyed();
    void onEmulatorRunning();
    void onEmulatorStopped();
private:
    // Internal Types

    // Internal Functions
    void defineActions();
    int promptToEndRunningSession();
    bool saveSession(bool forceNewFile);
    void updateTitle();
    void updateActions(bool isEmulatorRunning);

    // Internal Fields
    QPointer<QMainWindow> _mainWindow;
    ActionManager _actions;
    EmulatorSession _session;
    QString _sessionFileName;
    bool _isModified;
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
